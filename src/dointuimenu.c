/***************************************************************************

 MagicMenu - The Intuition Menus Enhancer
 Copyright (C) 1993-1997 by Martin Korndörfer
 Copyright (C) 1997-2001 by Olaf 'Olsen' Barthel
 Copyright (C) 2001-2009 by Stephan Rupprecht, Jens Langner
 Copyright (C) 2009      by MagicMenu Open Source Team

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 MagicMenu project:  http://sourceforge.net/projects/magicmenu/

 $Id$

***************************************************************************/

/*#define DEBUG*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

BOOL
DoIntuiMenu (UWORD NewMenuMode, BOOL PopUp, BOOL SendMenuDown)
{
  struct Window *ZwWin;
  UWORD Code, Err;
  struct InputEvent *NewEvent;
  BOOL lending = FALSE;
  ENTER();
  /* Zum Zeichnen des Hintergrundes müssen wir das vormerken.
   * Diese Flag entscheidet, ob der Transparenzmodus wirklich
   * aktiv werden darf.
   */
  GlobalPopUp = PopUp;

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

  MenStrip = MenWin->MenuStrip;
  MenScr = MenWin->WScreen;

  /* Leiht dem Fenster ein anderes seine Menüs? */
  if(V39)
  {
    struct Window *OtherGuy;

    if(OtherGuy = FindLending(MenWin))
    {
      SHOWMSG("menu lending active");

      D(("this window 0x%08lx |%s|",MenWin,MenWin->Title));

      MenWin = OtherGuy;
      MenStrip = OtherGuy->MenuStrip;
      MenScr = OtherGuy->WScreen;

      D(("Other window 0x%08lx |%s|",MenWin,MenWin->Title));

      lending = TRUE;
    }
  }

  /* Ist mit diesem Fenster etwas zu machen? */
  if ((!MenStrip) || (MenWin->Flags & WFLG_RMBTRAP) || (MenWin->ReqCount != 0))
  {
    /* Intuition wieder starten. */
    UnlockIBase (IBaseLock);
    ReleaseSemaphore (GetPointerSemaphore);
	LEAVE();
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
    D(("has menuverify"));
    /* Nachricht verschicken (beinhaltet UnlockIBase()) */
    Code = MENUHOT;
    Err = SendIntuiMessage (IDCMP_MENUVERIFY, &Code, PeekQualifier (), NULL, MenWin, IBaseLock, TRUE);

    if (Code == MENUCANCEL || Err != SENDINTUI_OK)
    {
      D(("code=%ld err=%ld", Code, Err));
      EndIntuiMenu (FALSE);
	  LEAVE();
      return (TRUE);
    }

    D(("get going again"));

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

  CleanUpMenu ();

  ResetMenu (MenStrip, FALSE);

  if(lending)
  {
    Code = FirstMenuNum;

    IBaseLock = LockIBase (NULL);
    SendIntuiMessage ((HelpPressed) ? IDCMP_MENUHELP : IDCMP_MENUPICK, &Code, PeekQualifier (), MenWin, MenWin, NULL, FALSE);
    UnlockIBase (IBaseLock);
  }
  /* Das hier ist der Knackpunkt. Das Fenster kann verschwinden (!) bevor
   * die IntuiMessage erzeugt und ausgeliefert ist. Vielleicht wäre es
   * besser, die IntuiMessage selbst zu erzeugen und auszuliefern.
   *
   * Eigentlich nicht. Intuition ist schlau genug, damit klarzukommen.
   */
  else if (NewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR))
  {
    NewEvent->ie_Class = (HelpPressed) ? IECLASS_MENUHELP : IECLASS_MENULIST;

    D(("sending with window 0x%08lx |%s|",MenWin,MenWin->Title));

    NewEvent->ie_Code = FirstMenuNum;
    NewEvent->ie_EventAddress = MenWin;
    NewEvent->ie_Qualifier = PeekQualifier ();

    InputIO->io_Data = (APTR) NewEvent;
    InputIO->io_Length = sizeof (struct InputEvent);

    InputIO->io_Command = IND_WRITEEVENT;
    DoIO ((struct IORequest *) InputIO);

    FreeVecPooled (NewEvent);
  }
  else
  {
    SHOWMSG("not enough memory for notification message");
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
  LEAVE();
  return (TRUE);
}
