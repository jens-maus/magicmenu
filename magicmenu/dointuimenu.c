/*
**	$Id$
**
**	:ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif	/* _GLOBAL_H */

BOOL
DoIntuiMenu (UWORD NewMenuMode, BOOL PopUp, BOOL SendMenuDown)
{
  struct Message *msg;
  struct Window *ZwWin;
  UWORD Code, Err;
  struct Screen *FrontScreen;
  struct InputEvent *NewEvent;

  /* Zugriff auf MenWin, MenScr und MenStrip sperren. */
  ObtainSemaphore (GetPointerSemaphore);

  /* Intuition anhalten. */
  IBaseLock = LockIBase (NULL);

  /* Ist gerade ein Fenster aktiv? */
  if (!(MenWin = IntuitionBase->ActiveWindow))
  {
    /* Falls nicht, die ganze Prozedur rückgängig machen. */
    UnlockIBase (IBaseLock);
    ReleaseSemaphore (GetPointerSemaphore);
    return (TRUE);
  }

  /* MenScr und MenStrip übernehmen. */
  MenScr = MenWin->WScreen;
  MenStrip = MenWin->MenuStrip;

  /* Intuition wieder starten. */
  UnlockIBase (IBaseLock);

  /* Das Menü ist jetzt aktiv; Operationen in diesem
   * Bildschirm, in diesem Fenster, in diesem Menü werden
   * warten müssen.
   */
  ObtainSemaphore (MenuActSemaphore);

  /* Ab jetzt kann wieder lesend auf MenWin, MenScr, MenStrip
   * zugegriffen werden.
   */
  ReleaseSemaphore (GetPointerSemaphore);

  /* Intuition anhalten. */
  IBaseLock = LockIBase (NULL);

  MenuMode = NewMenuMode;

  DoGlobalQuit = FALSE;
  FirstSelectedMenu = NULL;
  LastSelectedMenu = NULL;
  FirstMenuNum = MENUNULL;
  MenuNum = NOMENU;
  ItemNum = NOITEM;
  SubItemNum = NOSUB;

  /* Ist mit diesem Fenster etwas zu machen? */
  if ((!MenStrip) || (MenWin->Flags & WFLG_RMBTRAP) || (MenWin->ReqCount != 0))
  {
    /* Intuition wieder starten. */
    UnlockIBase (IBaseLock);
    /* MenWin, MenScr, MenStrip löschen, MenuActSemaphore loslassen. */
    EndIntuiMenu ();
    return (FALSE);
  }

  if (MenWin->IDCMPFlags & IDCMP_MENUVERIFY)
  {
    /* Nachricht verschicken (beinhaltet UnlockIBase()) */
    Code = MENUHOT;
    Err = SendIntuiMessage (IDCMP_MENUVERIFY,&Code, PeekQualifier (), NULL, MenWin, IBaseLock, TRUE);

    /* Abgebrochen wird, falls für die Message nicht genügend
     * Speicher da war, die Aktion abgebrochen wurde oder der
     * Himmel einstürzt. In jedem Fall sollte das Fenster
     * aber noch offen sein.
     */
    if (Err != SENDINTUI_OK || Code == MENUCANCEL)
    {
      EndIntuiMenu ();
      return (TRUE);
    }

    /* Intuition wieder starten. */
    IBaseLock = LockIBase (NULL);
  }

  /* Jetzt werden alle inaktiven Fenster des Bildschirms
   * benachrichtigt, die IDCMP_MENUVERIFY gesetzt haben.
   */

  for(ZwWin = MenScr->FirstWindow ; ZwWin != NULL ; ZwWin = ZwWin->NextWindow)
  {
    if (ZwWin != MenWin && (ZwWin->IDCMPFlags & IDCMP_MENUVERIFY))
    {
      /* Nachricht verschicken; fire and forget. */
      Code = MENUWAITING;
      SendIntuiMessage (IDCMP_MENUVERIFY,&Code, PeekQualifier (), NULL, ZwWin, NULL, FALSE);
    }
  }

  /* Vordersten Bildschirm merken. */
  FrontScreen = IntuitionBase->FirstScreen;

  /* Das war die letzte Handlung, zu der Intuition
   * gesperrt werden mußte. Jetzt ist nur noch ein
   * einziges Lock aktiv: MenuActSemaphore.
   */
  UnlockIBase(IBaseLock);

  /* Den Bildschirm mit dem Menü in den Vordergrund holen. */
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

  /* Das hier ist der Knackpunkt. Das Fenster kann verschwinden (!) bevor
   * die IntuiMessage erzeugt und ausgeliefert ist. Vielleicht wäre es
   * besser, die IntuiMessage selbst zu erzeugen und auszuliefern.
   *
   * Eigentlich nicht. Intuition ist schlau genug, damit klarzukommen.
   */
  if (NewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR))
  {
    NewEvent->ie_Class = (HelpPressed) ? IECLASS_MENUHELP : IECLASS_MENULIST;

    NewEvent->ie_Code = FirstMenuNum;
    NewEvent->ie_EventAddress = MenWin;
    NewEvent->ie_Qualifier = PeekQualifier ();

    InputIO->io_Data = (APTR) NewEvent;
    InputIO->io_Length = sizeof (struct InputEvent);

    InputIO->io_Command = IND_WRITEEVENT;
    DoIO ((struct IORequest *) InputIO);

    FreeVecPooled (NewEvent);
  }

  /* Falls dieser Bildschirm in den Vordergrund geholt
   * wurde, wird er wieder zurückgeschickt.
   */
  if (FrontScreen != MenScr)
    ScreenToBack (MenScr);

  EndIntuiMenu ();

  return (TRUE);
}
