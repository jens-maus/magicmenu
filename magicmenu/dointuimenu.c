/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

BOOL
DoIntuiMenu (UWORD NewMenuMode, BOOL PopUp, BOOL SendMenuDown)
{
  struct Window *ZwWin;
  UWORD Code, Err;
  struct InputEvent *NewEvent;
  struct Window *OriginalMenWin;

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

  OriginalMenWin = MenWin;
  MenStrip = MenWin->MenuStrip;
  MenScr = MenWin->WScreen;

  /* Leiht dem Fenster ein anderes seine Menüs? */
  if(V39)
  {
    struct Window *OtherGuy;

    if(OtherGuy = FindLending(MenWin))
    {
      MenWin = OtherGuy;
      MenStrip = OtherGuy->MenuStrip;
      MenScr = OtherGuy->WScreen;
    }
  }

  /* Ist mit diesem Fenster etwas zu machen? */
  if ((!MenStrip) || (MenWin->Flags & WFLG_RMBTRAP) || (MenWin->ReqCount != 0))
  {
    /* Intuition wieder starten. */
    UnlockIBase (IBaseLock);
    ReleaseSemaphore (GetPointerSemaphore);
    return (FALSE);
  }

  /* Ab jetzt kann wieder lesend auf MenWin, MenScr, MenStrip
   * zugegriffen werden.
   */
  ReleaseSemaphore (GetPointerSemaphore);

  MenuMode = NewMenuMode;

  DoGlobalQuit = FALSE;
  FirstSelectedMenu = NULL;
  LastSelectedMenu = NULL;
  FirstMenuNum = MENUNULL;
  MenuNum = NOMENU;
  ItemNum = NOITEM;
  SubItemNum = NOSUB;

  if (MenWin->IDCMPFlags & IDCMP_MENUVERIFY)
  {
    DB (kprintf ("has menuverify\n"));
    /* Nachricht verschicken (beinhaltet UnlockIBase()) */
    Code = MENUHOT;
    Err = SendIntuiMessage (IDCMP_MENUVERIFY, &Code, PeekQualifier (), NULL, MenWin, IBaseLock, TRUE);

    if (Code == MENUCANCEL || Err != SENDINTUI_OK)
    {
      DB (kprintf ("code=%ld err=%ld\n", Code, Err));
      EndIntuiMenu (FALSE);
      return (TRUE);
    }

    DB (kprintf ("get going again\n"));

    /* Intuition wieder starten. */
    IBaseLock = LockIBase (NULL);
  }

  /* Jetzt werden alle inaktiven Fenster des Bildschirms
   * benachrichtigt, die IDCMP_MENUVERIFY gesetzt haben.
   */

  for (ZwWin = MenScr->FirstWindow; ZwWin != NULL; ZwWin = ZwWin->NextWindow)
  {
    if (ZwWin != MenWin && (ZwWin->IDCMPFlags & IDCMP_MENUVERIFY))
    {
      /* Nachricht verschicken; fire and forget. */
      Code = MENUWAITING;
      SendIntuiMessage (IDCMP_MENUVERIFY, &Code, PeekQualifier (), NULL, ZwWin, NULL, FALSE);
    }
  }

  /* Das war die letzte Handlung, zu der Intuition
   * gesperrt werden mußte.
   */
  UnlockIBase (IBaseLock);

  /* Das Menü wird jetzt endlich aktiv; Operationen in diesem
   * Bildschirm, in diesem Fenster, in diesem Menü werden
   * warten müssen.
   */
  ObtainSemaphore (MenuActSemaphore);

  MenWin->Flags |= WFLG_MENUSTATE;

  GlobalLastWinDMREnable = (MenWin->DMRequest != NULL);

  HelpPressed = FALSE;

  if (!AktPrefs.mmp_NonBlocking)
  {
    LockLayerInfo (&MenScr->LayerInfo);
    LockLayers (&MenScr->LayerInfo);
    LayersLocked = TRUE;
  }

  ResetMenu (MenStrip, TRUE);

  ChangeBrokerSetup ();

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
    NewEvent->ie_EventAddress = OriginalMenWin;
    NewEvent->ie_Qualifier = PeekQualifier ();

    InputIO->io_Data = (APTR) NewEvent;
    InputIO->io_Length = sizeof (struct InputEvent);

    InputIO->io_Command = IND_WRITEEVENT;
    DoIO ((struct IORequest *) InputIO);

    FreeVecPooled (NewEvent);
  }

  /* Jetzt kommt die letzte Maßnahme. Die Fenster, die vorher die
   * MENUVERIFY/MENUWAITING-Nachricht bekommen hatten, erhalten jetzt
   * noch MOUSEBUTTONS/MENUUP.
   */

  IBaseLock = LockIBase (NULL);

  for (ZwWin = MenScr->FirstWindow; ZwWin != NULL; ZwWin = ZwWin->NextWindow)
  {
    if (ZwWin != MenWin && (ZwWin->IDCMPFlags & (IDCMP_MENUVERIFY | IDCMP_MOUSEBUTTONS)) == (IDCMP_MENUVERIFY | IDCMP_MOUSEBUTTONS))
    {
      /* Nachricht verschicken; fire and forget. */
      Code = MENUUP;
      SendIntuiMessage (IDCMP_MOUSEBUTTONS, &Code, PeekQualifier (), NULL, ZwWin, NULL, FALSE);
    }
  }

  UnlockIBase (IBaseLock);

  EndIntuiMenu (TRUE);

  return (TRUE);
}
