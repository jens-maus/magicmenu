/*
**	$Id$
**
**	:ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif	/* _GLOBAL_H */

/* ZZZ: Probleme wenn global optimizer angeschaltet wird. */

BOOL
DoIntuiMenu (UWORD NewMenuMode, BOOL PopUp, BOOL SendMenuDown)
{
  struct Message *msg;
  struct Window *ZwWin;
  UWORD Code, Err;
  struct Screen *FrontScreen;
  struct InputEvent *NewEvent;

  ObtainSemaphore (GetPointerSemaphore);

  IBaseLock = LockIBase (0l);

  if (!(MenWin = IntuitionBase->ActiveWindow))
  {
    UnlockIBase (IBaseLock);
    ReleaseSemaphore (GetPointerSemaphore);
    return (TRUE);
  }
  MenScr = MenWin->WScreen;
  MenStrip = MenWin->MenuStrip;

  UnlockIBase (IBaseLock);

  ObtainSemaphore (MenuActSemaphore);

  ReleaseSemaphore (GetPointerSemaphore);

  IBaseLock = LockIBase (0l);

  MenuMode = NewMenuMode;

  DoGlobalQuit = FALSE;
  FirstSelectedMenu = NULL;
  LastSelectedMenu = NULL;
  FirstMenuNum = MENUNULL;
  MenuNum = NOMENU;
  ItemNum = NOITEM;
  SubItemNum = NOSUB;

  if ((!MenStrip) || (MenWin->Flags & WFLG_RMBTRAP) || (MenWin->ReqCount != 0))
  {
    UnlockIBase (IBaseLock);
    EndIntuiMenu ();
    return (FALSE);
  }

  if (MenWin->IDCMPFlags & IDCMP_MENUVERIFY)
  {
    UnlockIBase (IBaseLock);
    ReleaseSemaphore(MenuActSemaphore);
    Code = MENUHOT;
    Err = CheckSendIntui (SendIntuiMessage (IDCMP_MENUVERIFY,
                                            &Code, PeekQualifier (), NULL, MenWin, IntuiTimeOut, NULL), "MENUVERIFY / MENUHOT");

    ObtainSemaphore (MenuActSemaphore);

    if (Err != SENDINTUI_OK || Code == MENUCANCEL)
    {
      EndIntuiMenu ();
      return (TRUE);
    }

    IBaseLock = LockIBase (0l);
  }

  ZwWin = MenScr->FirstWindow;

  UnlockIBase (IBaseLock);

  while (ZwWin)
  {
    if (ZwWin != MenWin && ZwWin->IDCMPFlags & IDCMP_MENUVERIFY)
    {
      Code = MENUWAITING;
      ReleaseSemaphore(MenuActSemaphore);
      Err = CheckSendIntui (SendIntuiMessage (IDCMP_MENUVERIFY,
                                              &Code, PeekQualifier (), NULL, ZwWin, IntuiTimeOut, NULL), "MENUVERIFY / MENUWAITING");
      ObtainSemaphore (MenuActSemaphore);
      if (Err != SENDINTUI_OK)
      {
        EndIntuiMenu ();
        return (TRUE);
      }
    }

    ZwWin = ZwWin->NextWindow;
  }

  IBaseLock = LockIBase (0l);

  FrontScreen = IntuitionBase->FirstScreen;

  UnlockIBase (IBaseLock);

  if (FrontScreen != MenScr)
    ScreenToFront (MenScr);

  MenWin->Flags |= WFLG_MENUSTATE;

  GlobalLastWinDMREnable = (MenWin->DMRequest != NULL);

  HelpPressed = FALSE;

  if(! AktPrefs.mmp_NonBlocking)
  {
      LockLayerInfo (&MenScr->LayerInfo);
      LockLayers (&MenScr->LayerInfo);
      LayersLocked = TRUE;
  }

  ResetMenu (MenStrip, TRUE);

  ActivateCxObj (Broker, FALSE);

  SetFilterIX (MouseFilter, &ActiveMouseIX);

  if (!(ActKbdFilter = CxFilter (MouseKey)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }

  SetFilterIX (ActKbdFilter, &ActiveKbdIX);
  AttachCxObj (Broker, ActKbdFilter);

  if (!(ActKbdSender = CxSender (CxMsgPort, EVT_KEYBOARD)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }

  AttachCxObj (ActKbdFilter, ActKbdSender);

  if (!(ActKbdTransl = CxTranslate (NULL)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }
  AttachCxObj (ActKbdFilter, ActKbdTransl);

  if (!(MouseMoveFilter = CxFilter (MouseKey)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }
  SetFilterIX (MouseMoveFilter, &ActiveMouseMoveIX);
  AttachCxObj (Broker, MouseMoveFilter);

  if (!(MouseMoveSender = CxSender (CxMsgPort, EVT_MOUSEMOVE)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }
  AttachCxObj (MouseMoveFilter, MouseMoveSender);

  if (!(TickFilter = CxFilter (MouseKey)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }
  SetFilterIX (TickFilter, &TickIX);
  AttachCxObj (Broker, TickFilter);

  if((TickSigNum = AllocSignal(-1)) == -1)
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }

  TickSigMask = 1l << TickSigNum;

  if (!(TickSignal = CxSignal (FindTask(NULL), TickSigNum)))
  {
    CleanUpMenu ();
    EndIntuiMenu ();
    return (TRUE);
  }
  AttachCxObj (TickFilter, TickSignal);

  while (msg = GetMsg (CxMsgPort))
  {
    if (!CheckReply (msg))
      ReplyMsg (msg);
  }

  ActivateCxObj (Broker, TRUE);
  CxChanged = TRUE;

  if (DrawMenuStrip (PopUp, ((PopUp) ? AktPrefs.mmp_PULook : AktPrefs.mmp_PDLook), TRUE))
  {
    SelectSpecial = (MenuMode == MODE_SELECT);
    ProcessIntuiMenu ();
  }
  else
    DisplayBeep (NULL);

  CleanUpMenu ();

  ResetMenu (MenStrip, FALSE);

  if (NewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR))
  {
    NewEvent->ie_Class = (HelpPressed) ? IECLASS_MENUHELP : IECLASS_MENULIST;

    NewEvent->ie_Code = FirstMenuNum;
    NewEvent->ie_EventAddress = MenWin;
    NewEvent->ie_NextEvent = NULL;
    NewEvent->ie_Qualifier = PeekQualifier ();

    InputIO->io_Data = (APTR) NewEvent;
    InputIO->io_Length = sizeof (struct InputEvent);

    InputIO->io_Command = IND_WRITEEVENT;
    DoIO ((struct IORequest *) InputIO);

    FreeVecPooled (NewEvent);
  }

  if (FrontScreen != MenScr)
    ScreenToFront (FrontScreen);

  EndIntuiMenu ();

  return (TRUE);
}
