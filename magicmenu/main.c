/*
**	$Id$
**
**	:ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif	/* _GLOBAL_H */

#ifdef _M68030
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") 68030 version\r\n";
#else
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") Generic 68k version\r\n";
#endif // _M68030

#ifdef _M68030
#define  PROCESSOR   "68030"
#else
#define  PROCESSOR   "68000"
#endif // _M68030

#define MAJORVERS      "2.0"

int
CXBRK (void)
{
  return (0);
}				/* Disable Lattice Ctrl-C Handl. */
/* void chkabort(void)	{ return; } */

/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*	       Menu Funktionen						    */
/* */
/* ************************************************************************ */
/* ************************************************************************ */


void
Activate (void)
{
  if (!CxChanged)
  {
    ActivateCxObj (Broker, TRUE);
    MagicActive = TRUE;
  }
}

void
Deactivate (void)
{
  if (!CxChanged)
  {
    ActivateCxObj (Broker, FALSE);
    MagicActive = FALSE;
  }
}

void
FreeMenuRemember (struct MenuRemember *Remember)
{
  struct MenuRmb *MenRmb, *NextMenu;
  struct ItemRmb *ItemRmb, *NextItem;

  MenRmb = Remember->FirstMenu;

  while (MenRmb)
  {
    ItemRmb = MenRmb->FirstItem;

    while (ItemRmb)
    {
      NextItem = ItemRmb->NextItem;
      FreeVecPooled (ItemRmb);
      ItemRmb = NextItem;
    }

    NextMenu = MenRmb->NextMenu;
    FreeVecPooled (MenRmb);
    MenRmb = NextMenu;
  }

  FreeVecPooled (Remember);
}

struct MenuRemember *
MakeMenuRemember (struct Window *Win)
{
  struct MenuItem *ZwItem;
  LONG NW, NH, N1, N2;
  struct IntuiText *IntTxt;
  BOOL ScrHiRes, HasCheck;
  struct MenuRemember *NewRemember;
  struct MenuRmb *NewMenRmb;
  struct ItemRmb *NewItemRmb;
  struct Menu *LookMenu;
  struct MenuItem *LookItem;
  struct TextAttr MenTextAttr;
  struct Screen *MenScr;
  struct Image *IntImg;
  long Length;

  if (Win->MenuStrip == NULL)
    return (NULL);

  if (!(NewRemember = AllocVecPooled (sizeof (struct MenuRemember), MEMF_CLEAR)))
      return (NULL);

  MenScr = Win->WScreen;

  AskFont (MenScr->BarLayer->rp, &MenTextAttr);

  ScrHiRes = (MenScr->Flags & SCREENHIRES) != NULL;

  NewRemember->MenWindow = Win;
  NewRemember->MenuStrip = Win->MenuStrip;

  NewRemember->AktMenuNum = MENUNULL;
  NewRemember->AktMenu = NULL;

  LookMenu = Win->MenuStrip;
  while (LookMenu)
  {
    if (!(NewMenRmb = AllocVecPooled (sizeof (struct MenuRmb), MEMF_CLEAR)))
    {
      FreeMenuRemember (NewRemember);
      return (NULL);
    }

    NewMenRmb->Menu = LookMenu;
    NewMenRmb->AktItemNum = 0;
    NewMenRmb->AktItem = 0;

    NewMenRmb->Height = 0;
    NewMenRmb->Width = 0;
    NewMenRmb->ZwTop = 0x7fff;
    NewMenRmb->ZwLeft = 0x7fff;
    NewMenRmb->CmdOffs = 0;

    HasCheck = FALSE;

    ZwItem = LookMenu->FirstItem;

    while (ZwItem)
    {
      if (!HasCheck && (ZwItem->Flags & CHECKIT) && (!ZwItem->SubItem) && (ZwItem->Flags & HIGHNONE) != HIGHNONE)
	HasCheck = TRUE;

      CommandText.ITextFont = &MenTextAttr;

      NW = ZwItem->LeftEdge + ZwItem->Width;
      NH = ZwItem->TopEdge + ZwItem->Height;

      if (ZwItem->Flags & ITEMTEXT)
      {
	IntTxt = (struct IntuiText *) ZwItem->ItemFill;

	while (IntTxt)
	{
	  if (IntTxt->ITextFont == NULL)
	  {
	    IntTxt->ITextFont = &MenTextAttr;
	    N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
	    N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
	    IntTxt->ITextFont = NULL;
	  }
	  else
	  {
	    N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
	    N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
	    CommandText.ITextFont = IntTxt->ITextFont;
	  }

	  if (N1 > NW)
	    NW = N1;

	  if (N2 > NH)
	    NH = N2;

	  IntTxt = IntTxt->NextText;
	}
      }
      else
      {
	IntImg = (struct Image *) ZwItem->ItemFill;
	while (IntImg)
	{
	  N1 = IntImg->LeftEdge + IntImg->Width + ZwItem->LeftEdge;
	  if (N1 > NW)
	    NW = N1;

	  N1 = IntImg->TopEdge + IntImg->Height + ZwItem->TopEdge;
	  if (N1 > NH)
	    NH = N1;

	  IntImg = IntImg->NextImage;
	}
      }

      if (NW > NewMenRmb->Width)
	NewMenRmb->Width = NW;
      if (NH > NewMenRmb->Height)
	NewMenRmb->Height = NH;

      if (ZwItem->LeftEdge < NewMenRmb->ZwLeft)
	NewMenRmb->ZwLeft = ZwItem->LeftEdge;
      if (ZwItem->TopEdge < NewMenRmb->ZwTop)
	NewMenRmb->ZwTop = ZwItem->TopEdge;

      if (ZwItem->Flags & COMMSEQ)
      {
	CommText[0] = ZwItem->Command;
	CommText[1] = 0;

	Length = IntuiTextLength (&CommandText);
	if (Length > NewMenRmb->CmdOffs)
	  NewMenRmb->CmdOffs = Length;
      }

      ZwItem = ZwItem->NextItem;
    }

    NewMenRmb->Height -= NewMenRmb->ZwTop;
    NewMenRmb->Width -= NewMenRmb->ZwLeft;

    NewMenRmb->NextMenu = NewRemember->FirstMenu;
    NewRemember->FirstMenu = NewMenRmb;

    if (NewMenRmb->Height != 0 && NewMenRmb->Width != 0)
    {
      if (ScrHiRes)
      {
	NewMenRmb->Height += 6;
	NewMenRmb->Width += 10;
	NewMenRmb->LeftBorder = 5;
	NewMenRmb->TopBorder = 3;
      }
      else
      {
	NewMenRmb->Height += 6;
	NewMenRmb->Width += 6;
	NewMenRmb->TopBorder = 3;
	NewMenRmb->LeftBorder = 3;
      }

      NewMenRmb->TopOffs = NewMenRmb->TopBorder - NewMenRmb->ZwTop;
      NewMenRmb->LeftOffs = NewMenRmb->LeftBorder - NewMenRmb->ZwLeft;

      LookItem = LookMenu->FirstItem;

      while (LookItem)
      {
	if (!(NewItemRmb = AllocVecPooled (sizeof (struct ItemRmb), MEMF_CLEAR)))
	{
	  FreeMenuRemember (NewRemember);
	  return (NULL);
	}

	NewItemRmb->Item = LookItem;
	NewItemRmb->AktSubItemNum = 0;
	NewItemRmb->AktSubItem = NULL;

	NewItemRmb->Height = 0;
	NewItemRmb->Width = 0;
	NewItemRmb->ZwTop = 0x7fff;
	NewItemRmb->ZwLeft = 0x7fff;
	NewItemRmb->CmdOffs = 0;
	NewItemRmb->Detached = FALSE;
	NewItemRmb->Detachable = TRUE;

	ZwItem = LookItem->SubItem;

	HasCheck = FALSE;

	while (ZwItem)
	{
	  if (!HasCheck && (ZwItem->Flags & CHECKIT) && (!ZwItem->SubItem) && (ZwItem->Flags & HIGHNONE) != HIGHNONE)
	    HasCheck = TRUE;

	  CommandText.ITextFont = &MenTextAttr;

	  NW = ZwItem->LeftEdge + ZwItem->Width;
	  NH = ZwItem->TopEdge + ZwItem->Height;

	  if (ZwItem->Flags & ITEMTEXT)
	  {
	    IntTxt = (struct IntuiText *) ZwItem->ItemFill;
	    while (IntTxt)
	    {
	      if (IntTxt->ITextFont == NULL)
	      {
		IntTxt->ITextFont = &MenTextAttr;
		N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
		N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
		IntTxt->ITextFont = NULL;
	      }
	      else
	      {
		N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
		N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
		CommandText.ITextFont = IntTxt->ITextFont;
	      }

	      if (N1 > NW)
		NW = N1;

	      if (N2 > NH)
		NH = N2;

	      IntTxt = IntTxt->NextText;
	    }
	  }
	  else
	  {
	    IntImg = (struct Image *) ZwItem->ItemFill;
	    while (IntImg)
	    {
	      N1 = IntImg->LeftEdge + IntImg->Width + ZwItem->LeftEdge;
	      if (N1 > NW)
		NW = N1;

	      N1 = IntImg->TopEdge + IntImg->Height + ZwItem->TopEdge;
	      if (N1 > NH)
		NH = N1;

	      IntImg = IntImg->NextImage;
	    }
	  }

	  if (NW > NewItemRmb->Width)
	    NewItemRmb->Width = NW;
	  if (NH > NewItemRmb->Height)
	    NewItemRmb->Height = NH;

	  if (ZwItem->LeftEdge < NewItemRmb->ZwLeft)
	    NewItemRmb->ZwLeft = ZwItem->LeftEdge;
	  if (ZwItem->TopEdge < NewItemRmb->ZwTop)
	    NewItemRmb->ZwTop = ZwItem->TopEdge;

	  if (ZwItem->Flags & COMMSEQ)
	  {
	    CommText[0] = ZwItem->Command;
	    CommText[1] = 0;

	    Length = IntuiTextLength (&CommandText);
	    if (Length > NewItemRmb->CmdOffs)
	      NewItemRmb->CmdOffs = Length;
	  }

	  ZwItem = ZwItem->NextItem;
	}

	NewItemRmb->Height -= NewItemRmb->ZwTop;
	NewItemRmb->Width -= NewItemRmb->ZwLeft;

	if (NewItemRmb->Height != NULL && NewItemRmb->Width != NULL)
	{
	  if (ScrHiRes)
	  {
	    NewItemRmb->Height += 6;
	    NewItemRmb->Width += 10;
	    NewItemRmb->LeftBorder = 5;
	    NewItemRmb->TopBorder = 3;
	  }
	  else
	  {
	    NewItemRmb->Height += 6;
	    NewItemRmb->Width += 6;
	    NewItemRmb->TopBorder = 3;
	    NewItemRmb->LeftBorder = 3;
	  }

	  NewItemRmb->TopOffs = NewItemRmb->TopBorder - NewItemRmb->ZwTop;
	  NewItemRmb->LeftOffs = NewItemRmb->LeftBorder - NewItemRmb->ZwLeft;

	}

	NewItemRmb->NextItem = NewMenRmb->FirstItem;
	NewMenRmb->FirstItem = NewItemRmb;

	LookItem = LookItem->NextItem;
      }

    }

    LookMenu = LookMenu->NextMenu;
  }

  return (NewRemember);

}

void
FreeGlobalRemember (void)
{
  struct MenuRemember *NextRemember;

  ObtainSemaphore (RememberSemaphore);

  while (GlobalRemember)
  {
    NextRemember = GlobalRemember->NextRemember;
    FreeMenuRemember (GlobalRemember);
    GlobalRemember = NextRemember;
  }

  ReleaseSemaphore (RememberSemaphore);
}

BOOL
MakeGlobalRemember (void)
{
  struct Screen *Scr;
  struct Window *Win;
  struct MenuRemember *NewMenuRemember;
  long ILock;

  ILock = LockIBase (0l);

  ObtainSemaphore (RememberSemaphore);

  Scr = IntuitionBase->FirstScreen;

  while (Scr)
  {
    Win = Scr->FirstWindow;

    while (Win)
    {
      if (Win->MenuStrip)
      {
	if (!(NewMenuRemember = MakeMenuRemember (Win)))
	{
	  FreeGlobalRemember ();
	  ReleaseSemaphore (RememberSemaphore);
	  UnlockIBase (ILock);
	  return (FALSE);
	}
	NewMenuRemember->NextRemember = GlobalRemember;
	GlobalRemember = NewMenuRemember;
      }
      Win = Win->NextWindow;
    }
    Scr = Scr->NextScreen;
  }

  ReleaseSemaphore (RememberSemaphore);

  UnlockIBase (ILock);

  return (TRUE);
}

void
ClearRemember (struct Window *Win)
{
  struct MenuRemember *LookRemember;
  struct MenuRemember *LastRemember;
  struct MenuRemember *NextRemember;

  ObtainSemaphore (RememberSemaphore);

  LookRemember = GlobalRemember;
  LastRemember = NULL;
  while (LookRemember)
  {
    NextRemember = LookRemember->NextRemember;
    if (LookRemember->MenWindow == Win)
    {
      if (LastRemember)
	LastRemember->NextRemember = LookRemember->NextRemember;
      else
	GlobalRemember = LookRemember->NextRemember;

      FreeMenuRemember (LookRemember);
    }
    else
      LastRemember = LookRemember;

    LookRemember = NextRemember;
  }

  ReleaseSemaphore (RememberSemaphore);
}

BOOL
UpdateRemember (struct Window *Window)
{
  struct MenuRemember *NewRemember;
  struct MenuRmb *LookMenRmb;
  struct ItemRmb *LookItemRmb;

  ObtainSemaphore (RememberSemaphore);

  ClearRemember (Window);

  if (!(NewRemember = MakeMenuRemember (Window)))
    return (FALSE);

  NewRemember->NextRemember = GlobalRemember;
  GlobalRemember = NewRemember;

  AktMenuRemember = NewRemember;

  if (AktMenu)
  {
    LookMenRmb = NewRemember->FirstMenu;
    AktMenRmb = NULL;
    while (LookMenRmb && (AktMenRmb == NULL))
    {
      if (LookMenRmb->Menu == AktMenu)
	AktMenRmb = LookMenRmb;
      LookMenRmb = LookMenRmb->NextMenu;
    }
    if (!AktMenRmb)
    {
      ReleaseSemaphore (RememberSemaphore);
      return (FALSE);
    }

    if (AktItem)
    {
      LookItemRmb = AktMenRmb->FirstItem;
      AktItemRmb = NULL;
      while (LookItemRmb && (AktItemRmb == NULL))
      {
	if (LookItemRmb->Item == AktItem)
	  AktItemRmb = LookItemRmb;
	LookItemRmb = LookItemRmb->NextItem;
      }
      if (!AktItemRmb)
      {
	ReleaseSemaphore (RememberSemaphore);
	return (FALSE);
      }
    }
  }
  ReleaseSemaphore (RememberSemaphore);
  return (TRUE);
}

void
SetRemember (struct Window *Win)
{
  register struct MenuRemember *NewRemember;

  ObtainSemaphore (RememberSemaphore);

  if (Win->MenuStrip)
  {
    ClearRemember (Win);

    NewRemember = MakeMenuRemember (Win);
    NewRemember->NextRemember = GlobalRemember;
    GlobalRemember = NewRemember;
  }

  ReleaseSemaphore (RememberSemaphore);
}

void
ResetMenu (struct Menu *Menu, BOOL MenNull)
{
  struct Menu *ZwMenu;
  struct MenuItem *ZwItem, *ZwSubItem;

  ZwMenu = Menu;
  while (ZwMenu)
  {
    ZwItem = ZwMenu->FirstItem;
    while (ZwItem)
    {
      ZwSubItem = ZwItem->SubItem;
      while (ZwSubItem)
      {
	ZwSubItem->Flags &= ~(ISDRAWN | HIGHITEM | MENUTOGGLED);
	if (MenNull)
	  ZwSubItem->NextSelect = MENUNULL;
	ZwSubItem = ZwSubItem->NextItem;
      }

      ZwItem->Flags &= ~(ISDRAWN | HIGHITEM | MENUTOGGLED);
      if (MenNull)
	ZwItem->NextSelect = MENUNULL;
      ZwItem = ZwItem->NextItem;
    }

    ZwMenu->Flags &= ~MIDRAWN;
    ZwMenu = ZwMenu->NextMenu;
  }
}

void
CleanUpMenu (void)
{
  struct Message *msg;

  CleanUpMenuSubBox ();
  CleanUpMenuBox ();
  CleanUpMenuStrip ();

  if (LayersLocked)
  {
    UnlockLayers (&MenScr->LayerInfo);
    UnlockLayerInfo (&MenScr->LayerInfo);
    LayersLocked = FALSE;
  }

  MenWin->Flags &= ~WFLG_MENUSTATE;

  if (CxChanged)
  {
    ActivateCxObj (Broker, FALSE);

    SetFilterIX (MouseFilter, &MouseIX);

    DeleteCxObjAll (MouseMoveFilter);

    DeleteCxObjAll (ActKbdFilter);

    DeleteCxObjAll (TickFilter);

    if(TickSigNum != -1)
	FreeSignal(TickSigNum);

    TickSigNum = -1;

    while (msg = GetMsg (CxMsgPort))
    {
      if (!CheckReply (msg))
	ReplyMsg (msg);
    }

    ActivateCxObj (Broker, TRUE);

    CxChanged = FALSE;
  }
}

BOOL
MenuSelected (BOOL LastSelect)
{
  struct MenuItem *SelItem, *ZwItem;
  BOOL SubItem;
  ULONG MExcl;
  LONG t, l, h, w;

  SelItem = NULL;

  if (AktSubItem)
  {
    SelItem = AktSubItem;
    SubItem = TRUE;
  }
  else if (AktItem && (!AktItem->SubItem))
  {
    SelItem = AktItem;
    SubItem = FALSE;
  }

  if (!SelItem)
    return (FALSE);
  if (!(SelItem->Flags & ITEMENABLED))
    return (FALSE);
  if (SubItem && SubBoxGhosted)
    return (FALSE);
  if (BoxGhosted)
    return (FALSE);

  if (SelItem->Flags & CHECKIT)
  {
    if (SelItem->MutualExclude)
    {
      if (!(SelItem->Flags & CHECKED))
      {
	MExcl = SelItem->MutualExclude;

	SelItem->Flags |= CHECKED;
	if (SubItem)
	{
	  ZwItem = SubBoxItems;
	  DrawNormSubItem (SelItem);
	  DrawHiSubItem (SelItem);
	}
	else
	{
	  ZwItem = BoxItems;
	  DrawNormItem (SelItem);
	  DrawHiItem (SelItem);
	}

	while (ZwItem)
	{
	  if (ZwItem != SelItem && (MExcl & 0x1) && (ZwItem->Flags & CHECKIT) && (ZwItem->Flags & CHECKED))
	  {
	    ZwItem->Flags &= ~CHECKED;
	    if (SubItem)
	    {
	      if (GetSubItemContCoor (ZwItem, &t, &l, &w, &h))
		DrawMenuItem (SubBoxDrawRPort, ZwItem, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE);
	    }
	    else
	    {
	      if (GetItemContCoor (ZwItem, &t, &l, &w, &h))
		DrawMenuItem (BoxDrawRPort, ZwItem, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE);
	    }
	  }

	  MExcl = MExcl >> 1;
	  ZwItem = ZwItem->NextItem;
	}
      }
      else if (SelItem->Flags & MENUTOGGLE)
      {
	SelItem->Flags &= ~CHECKED;
	if (SubItem)
	{
	  DrawNormSubItem (SelItem);
	  DrawHiSubItem (SelItem);
	}
	else
	{
	  DrawNormItem (SelItem);
	  DrawHiItem (SelItem);
	}
      }
    }
    else if ((!LastSelect) || (!(SelItem->Flags & MENUTOGGLED)))
    {
      SelItem->Flags ^= CHECKED;
      if (SubItem)
      {
	DrawNormSubItem (SelItem);
	DrawHiSubItem (SelItem);
      }
      else
      {
	DrawNormItem (SelItem);
	DrawHiItem (SelItem);
      }
    }
  }

  if (SelItem->Flags & MENUTOGGLED)
    return (TRUE);

  SelItem->Flags |= MENUTOGGLED;

  SelItem->NextSelect = MENUNULL;

  if (!FirstSelectedMenu)
  {
    FirstSelectedMenu = SelItem;
    FirstMenuNum = FULLMENUNUM (MenuNum, ItemNum, SubItemNum);
  }

  if (LastSelectedMenu)
    LastSelectedMenu->NextSelect = FULLMENUNUM (MenuNum, ItemNum, SubItemNum);

  LastSelectedMenu = SelItem;

  return (TRUE);
}

BOOL
CheckCxMsgAct (CxMsg * Msg, struct timerequest ** TimeReq, BOOL * Cancel)
{
  ULONG MsgID, MsgType;
  APTR MsgData;
  struct InputEvent Event;
  BOOL Shift, Ende, IsSingle, OutOfBounds;
  short ZwNum;
  char CharBuf[12], ASCIIChar;
  struct MenuItem *SavItem, *SavSubItem;
  struct Menu *SavMenu;
  UWORD SavMenuNum, SavItemNum, SavSubItemNum;

#ifdef TESTTIME
  UWORD z1;
  struct timeval StartTime, StopTime;
  char ZwStr[100];

#endif

  ULONG CurrSecs, CurrMics;


  *Cancel = FALSE;
  Ende = FALSE;

  if (*TimeReq && (Msg == (CxMsg *) * TimeReq))
  {
    Ende = *Cancel = TRUE;
    FreeVecPooled (*TimeReq);
    *TimeReq = NULL;
  }
  else
  {
    if (CheckReply ((struct Message *) Msg))
      return (FALSE);

    MsgID = CxMsgID (Msg);
    MsgType = CxMsgType (Msg);
    MsgData = CxMsgData (Msg);
    Event = *((struct InputEvent *) MsgData);
    ReplyMsg ((struct Message *) Msg);

    if (*TimeReq)
    {
      if (!CheckIO (*TimeReq))
      {
	AbortIO (*TimeReq);
	WaitIO (*TimeReq);
      }
      FreeVecPooled (*TimeReq);
      *TimeReq = NULL;
    }

    *TimeReq = SendTimeRequest (TimerIO, AktPrefs.mmp_TimeOut, 0, CxMsgPort);

    switch (MsgType)
    {
    case CXM_IEVENT:
      switch (MsgID)
      {
      case EVT_KEYBOARD:
	Shift = (NULL != (Event.ie_Qualifier & (IEQUALIFIER_RSHIFT | IEQUALIFIER_LSHIFT)));
	if (Event.ie_Code == HELP && (MenWin->IDCMPFlags & IDCMP_MENUHELP) != NULL)
	{
	  Ende = HelpPressed = TRUE;
	  FirstMenuNum = FULLMENUNUM (MenuNum, ItemNum, SubItemNum);
	  break;
	}
	if (Event.ie_Code == CTRL)
	{
	  SavSubItem = AktSubItem;
	  SavSubItemNum = SubItemNum;
	  SavItem = AktItem;
	  SavItemNum = ItemNum;
	  SavMenu = AktMenu;
	  SavMenuNum = MenuNum;
#ifdef TESTTIME
	  if (Shift)
	  {
	    GetSysTime (&StartTime);
	    for (z1 = 0; z1 < 200; z1++)
	    {
	      CleanUpMenuSubBox ();
	      CleanUpMenuBox ();
	      CleanUpMenuStrip ();
	      Ende = !DrawMenuStrip (StripPopUp, Look3D, FALSE);
	      if (SavMenu)
	      {
		DrawHiMenu (AktMenu = SavMenu);
		MenuNum = SavMenuNum;
		if (!DrawMenuBox (AktMenu, FALSE))
		  DisplayBeep (MenScr);

		if (SavItem)
		{
		  DrawHiItem (AktItem = SavItem);
		  ItemNum = SavItemNum;
		  SavItem->Flags |= HIGHITEM;
		  if (!DrawMenuSubBox (AktMenu, SavItem, FALSE))
		    DisplayBeep (MenScr);

		  if (SavSubItem)
		  {
		    DrawHiSubItem (AktSubItem = SavSubItem);
		    AktSubItem->Flags |= HIGHITEM;
		    SubItemNum = SavSubItemNum;
		  }
		}
	      }
	    }
	    GetSysTime (&StopTime);
	    SubTime (&StopTime, &StartTime);
	    Move (&MenScr->RastPort, 5, 50);
	    SetDrMd (&MenScr->RastPort, JAM2);
	    SetAPen (&MenScr->RastPort, 2);
	    SetBPen (&MenScr->RastPort, 1);
	    SPrintf (ZwStr, "Secs: %ld, Micro: %ld", StopTime.tv_secs, StopTime.tv_micro);
	    Text (&MenScr->RastPort, ZwStr, strlen (ZwStr));
	  }
	  else
#endif
	  {
	    CleanUpMenuSubBox ();
	    CleanUpMenuBox ();
	    CleanUpMenuStrip ();
	    Ende = !DrawMenuStrip (StripPopUp, (Look3D == LOOK_3D) ? LOOK_2D :
					       (StripPopUp) ? AktPrefs.mmp_PULook : AktPrefs.mmp_PDLook, FALSE);
	    if (SavMenu)
	    {
	      DrawHiMenu (AktMenu = SavMenu);
	      MenuNum = SavMenuNum;
	      if (!DrawMenuBox (AktMenu, FALSE))
		DisplayBeep (MenScr);

	      if (SavItem)
	      {
		DrawHiItem (AktItem = SavItem);
		ItemNum = SavItemNum;
		SavItem->Flags |= HIGHITEM;
		if (!DrawMenuSubBox (AktMenu, SavItem, FALSE))
		  DisplayBeep (MenScr);

		if (SavSubItem)
		{
		  DrawHiSubItem (AktSubItem = SavSubItem);
		  AktSubItem->Flags |= HIGHITEM;
		  SubItemNum = SavSubItemNum;
		}
	      }
	    }
	  }
	}

	if ((!StripPopUp) && (MenuMode == MODE_KEYBOARD))
	{
	  MenuMode = MODE_KEYBOARD;
	  if (MenuSubBoxSwapped)
	  {
	    switch (Event.ie_Code)
	    {
	    case RETURN:
	    case ENTER:
	      Ende = (MenuSelected (!Shift) && (!Shift));
	      break;

	    case ESC:
	      CleanUpMenuSubBox ();
	      break;

	    case CRSLEFT:
	      CleanUpMenuSubBox ();
	      CleanUpMenuBox ();
	      if (Shift)
		SelectNextMenu (-1);
	      else
	      {
		ZwNum = MenuNum;
		if (SelectPrevMenu (MenuNum) == ZwNum)
		  SelectPrevMenu (0x7f);
	      }
	      DrawMenuBox (AktMenu, TRUE);
	      break;

	    case CRSRIGHT:
	      CleanUpMenuSubBox ();
	      CleanUpMenuBox ();
	      if (Shift)
		SelectPrevMenu (0x7f);
	      else
	      {
		ZwNum = MenuNum;
		if (SelectNextMenu (MenuNum) == ZwNum)
		  SelectNextMenu (-1);
	      }
	      DrawMenuBox (AktMenu, TRUE);
	      break;

	    case CRSUP:
	      if (Shift)
		SelectNextSubItem (-1);
	      else
	      {
		ZwNum = SubItemNum;
		if (SelectPrevSubItem (SubItemNum) == ZwNum)
		  SelectPrevSubItem (0x7f);
	      }
	      break;
	    case CRSDOWN:
	      if (Shift)
		SelectPrevSubItem (0x7f);
	      else
	      {
		ZwNum = SubItemNum;
		if (SelectNextSubItem (SubItemNum) == ZwNum)
		  SelectNextSubItem (-1);
	      }
	      break;

	    default:
	      if (MapRawKey (&Event, CharBuf, 10, NULL) == 1)
	      {
		ASCIIChar = CharBuf[0];
		if (FindSubItemChar (ASCIIChar, &IsSingle) && IsSingle)
		  Ende = (MenuSelected (!Shift) && (!Shift));
	      }
	      break;
	    }
	  }
	  else if (MenuBoxSwapped)
	  {
	    switch (Event.ie_Code)
	    {
	    case RETURN:
	    case ENTER:
	      if (AktItem->SubItem)
		DrawMenuSubBox (AktMenu, AktItem, TRUE);
	      else
		Ende = (MenuSelected (!Shift) && (!Shift));
	      break;

	    case ESC:
	      CleanUpMenuBox ();
	      break;

	    case CRSLEFT:
	      CleanUpMenuBox ();
	      if (Shift)
		SelectNextMenu (-1);
	      else
	      {
		ZwNum = MenuNum;
		if (SelectPrevMenu (MenuNum) == ZwNum)
		  SelectPrevMenu (0x7f);
	      }
	      DrawMenuBox (AktMenu, TRUE);
	      break;

	    case CRSRIGHT:
	      CleanUpMenuBox ();
	      if (Shift)
		SelectPrevMenu (0x7f);
	      else
	      {
		ZwNum = MenuNum;
		if (SelectNextMenu (MenuNum) == ZwNum)
		  SelectNextMenu (-1);
	      }
	      DrawMenuBox (AktMenu, TRUE);
	      break;

	    case CRSUP:
	      if (Shift)
		SelectNextItem (-1);
	      else
	      {
		ZwNum = ItemNum;
		if (SelectPrevItem (ItemNum) == ZwNum)
		  SelectPrevItem (0x7f);
	      }
	      break;

	    case CRSDOWN:
	      if (Shift)
		SelectPrevItem (0x7f);
	      else
	      {
		ZwNum = ItemNum;
		if (SelectNextItem (ItemNum) == ZwNum)
		  SelectNextItem (-1);
	      }
	      break;
	    default:
	      if (MapRawKey (&Event, CharBuf, 10, NULL) == 1)
	      {
		ASCIIChar = CharBuf[0];
		if (FindItemChar (ASCIIChar, &IsSingle) && IsSingle)
		{
		  if (AktItem->SubItem)
		    DrawMenuSubBox (AktMenu, AktItem, TRUE);
		  else
		    Ende = (MenuSelected (!Shift) && (!Shift));
		}
	      }
	      break;
	    }
	  }
	  else
	  {
	    switch (Event.ie_Code)
	    {
	    case RETURN:
	    case ENTER:
	    case CRSDOWN:
	      DrawMenuBox (AktMenu, TRUE);
	      break;

	    case ESC:
	      Ende = TRUE;
	      *Cancel = TRUE;
	      break;

	    case CRSLEFT:
	      if (Shift)
		SelectNextMenu (-1);
	      else
	      {
		ZwNum = MenuNum;
		if (SelectPrevMenu (MenuNum) == ZwNum)
		  SelectPrevMenu (0x7f);
	      }
	      break;

	    case CRSRIGHT:
	      if (Shift)
		SelectPrevMenu (0x7f);
	      else
	      {
		ZwNum = MenuNum;
		if (SelectNextMenu (MenuNum) == ZwNum)
		  SelectNextMenu (-1);
	      }
	      break;
	    default:
	      if (MapRawKey (&Event, CharBuf, 10, NULL) == 1)
	      {
		ASCIIChar = CharBuf[0];
		if (FindMenuChar (ASCIIChar, &IsSingle) && IsSingle)
		  DrawMenuBox (AktMenu, TRUE);
	      }
	      break;
	    }
	  }
	}
	else
	  Ende = *Cancel = (Event.ie_Code == ESC);
	break;
      case EVT_MOUSEMENU:
	if ((Event.ie_Code == IECODE_RBUTTON) && (MenWin->DMRequest != NULL))
	{
	  CurrentTime (&CurrSecs, &CurrMics);
	  if (DoubleClick (DMROldSecs, DMROldMics, CurrSecs, CurrMics))
	  {
	    GlobalDMRequest = TRUE;
	    Ende = TRUE;
	    *Cancel = TRUE;
	    break;
	  }
	}
	Shift = (NULL != (Event.ie_Qualifier & (IEQUALIFIER_RSHIFT | IEQUALIFIER_LSHIFT)));
	if (SelectSpecial)
	{
	  OutOfBounds = LookMouse (MenScr->MouseX, MenScr->MouseY, FALSE);
	  if (Event.ie_Code == (IECODE_RBUTTON | IECODE_UP_PREFIX))
	  {
	    SelectSpecial = FALSE;
	    Ende = (OutOfBounds || MenuSelected (TRUE));
	    *Cancel = OutOfBounds;
	  }
	  else
	  {
	    if (Event.ie_Code == IECODE_LBUTTON)
	      MenuSelected (FALSE);
	  }
	}
	else if (MenuMode == MODE_STD)
	{
	  Ende = (Event.ie_Code == (IECODE_RBUTTON | IECODE_UP_PREFIX));

	  LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
	  if (Event.ie_Code == IECODE_LBUTTON)
	    MenuSelected (FALSE);

	  if (Ende)
	    MenuSelected (TRUE);
	}
	else if (MenuMode == MODE_STICKY)
	{
	  Ende = (Event.ie_Code == IECODE_RBUTTON);

	  OutOfBounds = LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
	  if (Event.ie_Code == IECODE_LBUTTON)
	    Ende = OutOfBounds || (MenuSelected (!Shift) && (!Shift));
	  else if (Ende)
	    MenuSelected (TRUE);
	}
	else if (MenuMode == MODE_SELECT || MenuMode == MODE_KEYBOARD)
	{
	  Ende = *Cancel = (Event.ie_Code == IECODE_RBUTTON);

	  if (!Ende && (Event.ie_Code & IECODE_LBUTTON))
	  {
	    MenuMode = MODE_SELECT;
	    if (Event.ie_Code == IECODE_LBUTTON)
	      Ende = LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
	    else
	      Ende = (MenuSelected (!Shift) && (!Shift) && (Event.ie_Code == (IECODE_LBUTTON | IECODE_UP_PREFIX)));
	  }
	}
	break;

      case EVT_MOUSEMOVE:
	Shift = (NULL != (Event.ie_Qualifier & (IEQUALIFIER_RSHIFT | IEQUALIFIER_LSHIFT)));
	if ((MenuMode != MODE_SELECT || SelectSpecial) && MenuMode != MODE_KEYBOARD)
	  LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
	if (MenuMode == MODE_STD)
	{
	  Ende = (!(Event.ie_Qualifier & IEQUALIFIER_RBUTTON));
	  if (Ende || (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON) != NULL)
	    MenuSelected (TRUE);
	}
	if (MenuMode == MODE_STICKY)
	{
	  if (!Shift && (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON))
	    Ende = MenuSelected (TRUE);
	  else if (Ende || (Shift && (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON)))
	    MenuSelected (TRUE);
	}
	if (MenuMode == MODE_SELECT && (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON))
	  LookMouse (MenScr->MouseX, MenScr->MouseY, FALSE);
	break;
      case EVT_KBDMENU:
	Ende = TRUE;
	*Cancel = TRUE;
	break;
      default:
	break;
      }
      break;
    default:
      break;

    }
  }
  return (Ende);
}

void
ProcessIntuiMenu (void)
{
  ULONG SigMask, Signals;
  BOOL Ende, DoWait;
  struct Message *Msg;
  struct timerequest *TimeReq, *LLayerTimeReq;
  BOOL Cancel, TickReceived;
  struct MenuItem *SavItem, *SavSubItem;
  struct Menu *SavMenu;
  UWORD SavMenuNum, SavItemNum, SavSubItemNum;

  Ende = FALSE;
  TickReceived = FALSE;

  TimeReq = SendTimeRequest (TimerIO, AktPrefs.mmp_TimeOut, 0, CxMsgPort);
  LLayerTimeReq = SendTimeRequest(TimerIO, 0, 500000, CxMsgPort);

  while (!Ende)
  {
    SigMask = CxSignalMask | TickSigMask;

    DoWait = TRUE;

    if (Msg = GetMsg (CxMsgPort))
    {
      DoWait = FALSE;

      if((struct timerequest *)Msg == LLayerTimeReq)
      {
	  FreeVecPooled(LLayerTimeReq);
	  LLayerTimeReq = NULL;
	  if(TickReceived)
	  {
	      LLayerTimeReq = SendTimeRequest(TimerIO, 0, 500000, CxMsgPort);
	      TickReceived = FALSE;
	  }
	  else
	  {
	      SavSubItem = AktSubItem;
	      SavSubItemNum = SubItemNum;
	      SavItem = AktItem;
	      SavItemNum = ItemNum;
	      SavMenu = AktMenu;
	      SavMenuNum = MenuNum;

	      CleanUpMenuSubBox ();
	      CleanUpMenuBox ();
	      CleanUpMenuStrip ();

	      if (LayersLocked)
	      {
		UnlockLayers (&MenScr->LayerInfo);
		UnlockLayerInfo (&MenScr->LayerInfo);
		LayersLocked = FALSE;
	      }

	      Delay(1);
	      LockLayerInfo (&MenScr->LayerInfo);
	      LockLayers (&MenScr->LayerInfo);
	      LayersLocked = TRUE;

	      Ende = Cancel = !DrawMenuStrip (StripPopUp, Look3D, FALSE);
	      if (SavMenu)
	      {
		DrawHiMenu (AktMenu = SavMenu);
		MenuNum = SavMenuNum;
		if (!DrawMenuBox (AktMenu, FALSE))
		  DisplayBeep (MenScr);

		if (SavItem)
		{
		  DrawHiItem (AktItem = SavItem);
		  ItemNum = SavItemNum;
		  SavItem->Flags |= HIGHITEM;
		  if (!DrawMenuSubBox (AktMenu, SavItem, FALSE))
		    DisplayBeep (MenScr);

		  if (SavSubItem)
		  {
		    DrawHiSubItem (AktSubItem = SavSubItem);
		    AktSubItem->Flags |= HIGHITEM;
		    SubItemNum = SavSubItemNum;
		  }
		}
	      }
	  }
      }
      else
	  Ende = CheckCxMsgAct ((CxMsg *) Msg, &TimeReq, &Cancel);
    }

    if (DoWait)
    {
      Signals = Wait (SigMask);
    }

    if((Signals & TickSigMask) != 0)
	TickReceived = TRUE;
  }

  if (TimeReq)
  {
    if (!CheckIO (TimeReq))
    {
      AbortIO (TimeReq);
      WaitIO (TimeReq);
    }
    FreeVecPooled (TimeReq);
  }

  if (LLayerTimeReq)
  {
    if (!CheckIO (LLayerTimeReq))
    {
      AbortIO (LLayerTimeReq);
      WaitIO (LLayerTimeReq);
    }
    FreeVecPooled (LLayerTimeReq);
  }

  if (Cancel)
  {
    FirstSelectedMenu = NULL;
    LastSelectedMenu = NULL;
    FirstMenuNum = MENUNULL;
  }
}

void
EndIntuiMenu (void)
{
  ObtainSemaphore (GetPointerSemaphore);

  MenScr = NULL;
  MenWin = NULL;
  MenStrip = NULL;

  ReleaseSemaphore (GetPointerSemaphore);

  ReleaseSemaphore (MenuActSemaphore);
}

/* ZZZ: Probleme wenn global optimizer angeschaltet wird. */

#if 0
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
#endif
/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*	       Commodity Funktionen					    */
/* */
/* ************************************************************************ */
/* ************************************************************************ */


BOOL
CheckCxMsg (CxMsg * Msg)
{
  BOOL Ende;
  ULONG MsgID, MsgType;
  APTR MsgData;
  ULONG MyLock;
  BOOL Ok;

  ULONG CurrSecs, CurrMics;

  static WORD MenSaveMouseX = -1;
  static WORD MenSaveMouseY = -1;

  struct InputEvent *MyNewEvent;

  struct ViewPortExtra *VPExtra;
  struct ColorMap *cm;

  static struct InputEvent Event;
  static BOOL IgnoreMouse = FALSE;
  static BOOL DoWait = FALSE;
  static struct Screen *OldMenScr = NULL;

  static WORD OrigLeftEdge = 0;
  static WORD OrigTopEdge = 0;

  static struct TagItem ti[] =
  {VTAG_VIEWPORTEXTRA_GET, NULL,
   VTAG_END_CM, NULL};


  if (CheckReply ((struct Message *) Msg))
    return (FALSE);

  Ende = FALSE;

  MsgID = CxMsgID (Msg);
  MsgType = CxMsgType (Msg);
  MsgData = CxMsgData (Msg);
  if (MsgType == CXM_IEVENT)
  {
    Event = *((struct InputEvent *) MsgData);
  }

  ReplyMsg ((struct Message *) Msg);

  MyLock = LockIBase (0);
  if (IntuitionBase->ActiveWindow)
    MenScr = IntuitionBase->ActiveWindow->WScreen;
  else
    MenScr = NULL;

  UnlockIBase (MyLock);

  if (OldMenScr != MenScr)
  {
    IgnoreMouse = DoWait = FALSE;
  }

  GlobalDMRequest = FALSE;

  switch (MsgType)
  {
  case CXM_IEVENT:
    switch (MsgID)
    {
    case EVT_KBDMENU:
      if (AktPrefs.mmp_KCEnabled && (MenScr != NULL))
      {
	if (!IgnoreMouse)
	{
	  OrigTopEdge = MenScr->TopEdge;
	  OrigLeftEdge = MenScr->LeftEdge;
	}
	cm = MenScr->ViewPort.ColorMap;
	ti[0].ti_Tag = VTAG_VIEWPORTEXTRA_GET;
	ti[0].ti_Data = NULL;
	ti[1].ti_Tag = VTAG_END_CM;
	if (VideoControl (cm, ti) == NULL)
	{
	  VPExtra = (struct ViewPortExtra *) ti[0].ti_Data;
	  MenDispClip = VPExtra->DisplayClip;
	  if (!IgnoreMouse)
	    MoveScreen (MenScr, (OrigLeftEdge < 0) ? (-OrigLeftEdge) : 0,
			(OrigTopEdge < 0) ? (-OrigTopEdge) : 0);
	}
	else
	{
	  MenDispClip.MinX = 0;
	  MenDispClip.MinY = 0;
	  MenDispClip.MaxX = MenScr->Width - 1;
	  MenDispClip.MaxY = MenScr->Height - 1;
	}

	if (AktPrefs.mmp_KCGoTop && MenScr->MouseY > MenScr->BarHeight)
	{
	  MenSaveMouseX = MenScr->MouseX;
	  MenSaveMouseY = MenScr->MouseY;
	  MoveMouse (MenDispClip.MaxX - 16 - MenScr->LeftEdge, MenScr->BarHeight - 1, TRUE, &Event, MenScr);
	  DoWait = TRUE;
	  IgnoreMouse = TRUE;
	  OldMenScr = MenScr;

	}
	else
	{
	  if (DoWait)
	    Delay (1);
	  DoWait = FALSE;
	  OldMenScr = MenScr;
	  if (DoIntuiMenu (MODE_KEYBOARD, FALSE, TRUE))
	    IgnoreMouse = FALSE;
	  else
	    IgnoreMouse = TRUE;

	  if (MenSaveMouseX != -1)
	  {
	    MoveMouse (MenSaveMouseX, MenSaveMouseY, FALSE, NULL, OldMenScr);
	    MenSaveMouseX = -1;
	  }

	  MoveScreen (OldMenScr, OrigLeftEdge - (WORD) OldMenScr->LeftEdge,
		      OrigTopEdge - (WORD) OldMenScr->TopEdge);

	}
      }
      break;
    case EVT_MOUSEMENU:
      if (GlobalDMRCount > 0)
      {
	GlobalDMRCount--;
	break;
      }
      if ((IgnoreMouse || ActivateCxObj (MouseTransl, TRUE)) && MenScr != NULL)
      {
	if (!IgnoreMouse)
	{
	  CurrentTime (&CurrSecs, &CurrMics);
	  if (GlobalLastWinDMREnable && DoubleClick (DMROldSecs, DMROldMics, CurrSecs, CurrMics))
	  {
	    GlobalDMRequest = TRUE;
	  }
	  else
	  {
	    DMROldSecs = CurrSecs;
	    DMROldMics = CurrMics;
	    if ((AktPrefs.mmp_MenuType == MT_AUTO && MenScr->MouseY <= MenScr->BarHeight) || AktPrefs.mmp_MenuType == MT_PULLDOWN)
	      Ok = DoIntuiMenu (AktPrefs.mmp_PDMode, FALSE, FALSE);
	    else
	      Ok = DoIntuiMenu (AktPrefs.mmp_PUMode, TRUE, FALSE);

	    if ((!Ok) && (!GlobalDMRequest))
	    {
	      if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_CLEAR | MEMF_PUBLIC))
	      {
		ActivateCxObj (MouseTransl, FALSE);
		*MyNewEvent = Event;
		MyNewEvent->ie_NextEvent = NULL;

		InputIO->io_Data = (APTR) MyNewEvent;
		InputIO->io_Length = sizeof (struct InputEvent);

		InputIO->io_Command = IND_WRITEEVENT;
		DoIO ((struct IORequest *) InputIO);
		FreeVecPooled (MyNewEvent);
	      }
	      else
		DisplayBeep (NULL);
	    }
	  }

	  if (GlobalDMRequest)
	  {
	    GlobalDMRequest = FALSE;
	    if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_CLEAR | MEMF_PUBLIC))
	    {
	      ActivateCxObj (MouseTransl, FALSE);
	      GlobalDMRCount = 1;
	      *MyNewEvent = Event;
	      MyNewEvent->ie_NextEvent = NULL;
	      InputIO->io_Data = (APTR) MyNewEvent;
	      InputIO->io_Length = sizeof (struct InputEvent);

	      InputIO->io_Command = IND_WRITEEVENT;
	      DoIO ((struct IORequest *) InputIO);

	      GetSysTime (&(MyNewEvent->ie_TimeStamp));
	      MyNewEvent->ie_Class = IECLASS_RAWMOUSE;
	      MyNewEvent->ie_Code = IECODE_RBUTTON | IECODE_UP_PREFIX;
	      MyNewEvent->ie_Qualifier = PeekQualifier ();
	      MyNewEvent->ie_NextEvent = NULL;
	      DoIO ((struct IORequest *) InputIO);

	      *MyNewEvent = Event;
	      GetSysTime (&(MyNewEvent->ie_TimeStamp));
	      MyNewEvent->ie_NextEvent = NULL;
	      DoIO ((struct IORequest *) InputIO);

	      FreeVecPooled (MyNewEvent);
	    }
	    else
	      DisplayBeep (NULL);
	  }
	}
	else
	{
	  OldMenScr = MenScr;
	  DoIntuiMenu (MODE_KEYBOARD, FALSE, FALSE);
	  IgnoreMouse = FALSE;
	  MoveScreen (OldMenScr, OrigLeftEdge - (WORD) OldMenScr->LeftEdge,
		      OrigTopEdge - (WORD) OldMenScr->TopEdge);
	}
      }
      break;
    case EVT_POPPREFS:
      StartPrefs();
      break;
    default:
      break;
    }
    break;
  case CXM_COMMAND:
    switch (MsgID)
    {
    case CXCMD_DISABLE:
      Deactivate ();
      break;
    case CXCMD_ENABLE:
      Activate ();
      break;
    case CXCMD_KILL:
      Ende = CheckEnde ();
      break;
    case CXCMD_UNIQUE:
    case CXCMD_APPEAR:
	StartPrefs();
      break;
    case CXCMD_DISAPPEAR:
	StopPrefs();
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }

  if (DoGlobalQuit)
    Ende = TRUE;

  return (Ende);
}


BOOL CheckMMMsgPort(struct MMMessage *MMMsg)
{
    BOOL	Ende;

    Ende = FALSE;

    switch(MMMsg->Class)
    {
	case MMC_REMOVE:
	    ReplyMsg(MMMsg);
	    Ende = CheckEnde();
	    break;
	case MMC_NEWCONFIG:
	    AktPrefs = *(struct MMPrefs *)MMMsg->Ptr1;
	    ReplyMsg(MMMsg);
	    break;
	case MMC_GETCONFIG:
	    MMMsg->Ptr1 = &AktPrefs;
	    MMMsg->Arg1 = MagicActive;
	    MMMsg->Ptr2 = Cx_Popkey;
	    ReplyMsg(MMMsg);
	    break;
	case MMC_ENABLE:
	    if(MMMsg->Arg1)
		Activate();
	    else
		Deactivate();
	    ReplyMsg(MMMsg);
	    break;
    }
    return(Ende);
}

void
ProcessCommodity (void)
{
  ULONG SigMask, SigRec, MMMsgPortMask;
  BOOL Ende, DoWait;
  struct Message *Msg;
  struct MMMessage *MMMsg;

  MMMsgPortMask = 1l << MMMsgPort->mp_SigBit;

  Ende = FALSE;

  while (!Ende)
  {
    SigMask = CxSignalMask | MMMsgPortMask | SIGBREAKF_CTRL_C;

    DoWait = TRUE;

    if (Msg = GetMsg (CxMsgPort))
    {
      DoWait = FALSE;
      Ende = CheckCxMsg ((CxMsg *) Msg);
    }

    if (MMMsg = (struct MMMessage *)GetMsg(MMMsgPort))
    {
	DoWait = FALSE;
	Ende = CheckMMMsgPort(MMMsg);
    }

    if (DoWait)
    {
      SigRec = Wait (SigMask);

      if (SigRec & SIGBREAKF_CTRL_C)
      {
	Ende = CheckEnde ();
      }
    }
  }
}


void StopPrefs(void)
{
    struct  Task	*Task;

    if(Task = FindTask("MMPrefs"))
	Signal(Task,SIGBREAKF_CTRL_C);
}


BOOL StartPrefs(void)
{
    char	ZwStr[232];

    strcpy(ZwStr,ConfigPath);
    AddPart(ZwStr,"MMPrefs",230);

    SystemTags(ZwStr,
		    SYS_Input,	    Open("nil:",MODE_OLDFILE),
		    SYS_Output,     Open("nil:",MODE_OLDFILE),
		    SYS_Asynch,     TRUE,
		    NP_StackSize,   5000,
		    NP_Name,	    "MMPrefs",
		    TAG_DONE);
    return(TRUE);
}


/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*	       Supportfunktionen für MAIN				    */
/* */
/* ************************************************************************ */
/* ************************************************************************ */


char *
strupc (char *Str)
{
  char *SavStr;

  SavStr = Str;
  while (*Str)
  {
    *Str = toupper (*Str);
    Str++;
  }

  return (SavStr);
}

void
MyArgString (char *Result, struct DiskObject *DO, const char *TT, const char *Default, LONG Len, BOOL Upcase)
{
  char *Found;

  Found = FindToolType (DO->do_ToolTypes, (char *) TT);
  if (Found)
    strncpy (Result, Found, Len);
  else
    strncpy (Result, Default, Len);

  if (Upcase)
    strupc (Result);
}

LONG
MyArgInt (struct DiskObject *DO, const char *TT, LONG Default)
{
  char Str[80];
  LONG Result;

  MyArgString (Str, DO, TT, "", 79, FALSE);
  if (stcd_l (Str, &Result) == 0)
    Result = Default;

  return (Result);
}

void
CheckArguments (void)
{
  char FName[60];
  struct DiskObject *DiskObj;

  strcpy (FName, "progdir:");
  strcat (FName, ProgName);

  if (!(DiskObj = GetDiskObjectNew (FName)))
    return;

  Cx_Pri = MyArgInt (DiskObj, TT_CX_PRI, DT_CX_PRI);

  MyArgString (Cx_PopupStr, DiskObj, TT_CX_POPUP, DT_CX_POPUP, ANSWER_LEN, FALSE);
  MyArgString (Cx_Popkey, DiskObj, TT_CX_POPKEY, DT_CX_POPKEY, LONGANSWER_LEN, FALSE);

  FreeDiskObject (DiskObj);

  Cx_Popup = FALSE;

  if (strcmp (Cx_PopupStr, "YES") == 0)
    Cx_Popup = TRUE;
}


BOOL LoadPrefs(char *ConfigFile, BOOL Report)
{
    BPTR	    FH;
    struct MMPrefs  ZwPrefs;
    ULONG	    Len;

    if(! (FH = Open(ConfigFile,MODE_OLDFILE)))
    {
	if(Report)
	    SimpleRequest(NULL,"MagicMenu Error", "Couldn't open configuration file!","Ok",NULL,0,0);
	return(FALSE);
    }

    Len = Read(FH,&ZwPrefs,sizeof(struct MMPrefs));

    Close(FH);

    if(Len == sizeof(struct MMPrefs) && ZwPrefs.mmp_Version == MMP_MAGIC)
    {
	CopyMem(&ZwPrefs,&AktPrefs,sizeof(struct MMPrefs));
	return(TRUE);
    }
    else
    {
	if(Report)
	    SimpleRequest(NULL,"MagicMenu Error", "Error in configuration file!","Ok",NULL,0,0);
	return(FALSE);
    }
}



void
CloseAll (void)
{
  struct Message *msg;
  struct MMMessage *MMMsg;

  if (RememberSemaphore)
    ObtainSemaphore (RememberSemaphore);

  if (GetPointerSemaphore)
    ObtainSemaphore (GetPointerSemaphore);

  if (MenuActSemaphore)
    ObtainSemaphore (MenuActSemaphore);

  /*****************************************************************************************/

	WindowGlyphExit();

  /*****************************************************************************************/

  if(MMMsgPort)
  {
      RemPort(MMMsgPort);
      while(MMMsg = (struct MMMessage *)GetMsg(MMMsgPort))
      {
	  MMMsg->Class = MMC_VOID;
	  ReplyMsg(MMMsg);
      }
      DeleteMsgPort(MMMsgPort);
      MMMsgPort = NULL;
  }

  if (Broker)
  {
    DeleteCxObjAll (Broker);
    Broker = NULL;
  }

  if (LibPatches)
  {
    RemovePatches();
    LibPatches = FALSE;
  }

  if (GlobalRemember)
  {
    FreeGlobalRemember ();
    GlobalRemember = NULL;
  }

  if (CxMsgPort)
  {
    while (msg = GetMsg (CxMsgPort))
    {
      if (!CheckReply (msg))
	ReplyMsg (msg);
    }
    DeletePort (CxMsgPort);
    CxMsgPort = NULL;
  }

  if (InputIO)
  {
    if (!CheckIO (InputIO))
    {
      AbortIO (InputIO);
      WaitIO (InputIO);
    }

    CloseDevice ((struct IORequest *) InputIO);
    InputBase = NULL;

    DeleteIORequest ((struct IORequest *) InputIO);
    InputIO = NULL;

    DeletePort (InputPort);
    InputPort = NULL;
  }

  if (TimerIO)
  {
    if (!CheckIO (TimerIO))
    {
      AbortIO (TimerIO);
      WaitIO (TimerIO);
    }

    CloseDevice ((struct IORequest *) TimerIO);
    TimerBase = NULL;

    DeleteIORequest ((struct IORequest *) TimerIO);
    TimerIO = NULL;

    DeletePort (TimerPort);
    TimerPort = NULL;
  }

  if (RememberSemaphore)
  {
    FreeVecPooled (RememberSemaphore);
    RememberSemaphore = NULL;
  }

  if (GetPointerSemaphore)
  {
    FreeVecPooled (GetPointerSemaphore);
    GetPointerSemaphore = NULL;
  }

  if (MenuActSemaphore)
  {
    FreeVecPooled (MenuActSemaphore);
    MenuActSemaphore = NULL;
  }


  if (KeymapBase)
  {
    CloseLibrary (KeymapBase);
    KeymapBase = NULL;
  }

  if (GadToolsBase)
  {
    CloseLibrary (GadToolsBase);
    GadToolsBase = NULL;
  }

  if (CxBase)
  {
    CloseLibrary (CxBase);
    CxBase = NULL;
  }

  if (IconBase)
  {
    CloseLibrary (IconBase);
    IconBase = NULL;
  }

  if (UtilityBase)
  {
    CloseLibrary (UtilityBase);
    UtilityBase = NULL;
  }

  if (LayersBase)
  {
    CloseLibrary ((struct Library *) LayersBase);
    LayersBase = NULL;
  }

  if (GfxBase)
  {
    CloseLibrary ((struct Library *) GfxBase);
    GfxBase = NULL;
  }

  if (IntuitionBase)
  {
    CloseLibrary ((struct Library *) IntuitionBase);
    IntuitionBase = NULL;
  }

  /*****************************************************************************************/

  MemoryExit();

  /*****************************************************************************************/
}

void
ExitTrap (void)
{
  CloseAll ();
  Delay (50);
}

void
ErrorPrc (const char *ErrTxt)
{
  static struct IntuiText NoOS2Body2 =
  {2, 1, JAM1, 5, 15, NULL, (UBYTE *) "works with AmigaOS 2.04 or later!", NULL};

  static struct IntuiText NoOS2Body1 =
  {2, 1, JAM1, 5, 5, NULL, (UBYTE *) "Sorry dude, this fine program only", &NoOS2Body2};

  static struct IntuiText NoOS2Neg =
  {2, 1, JAM1, 6, 3, NULL, (UBYTE *) "What a pity!", NULL};

  if (!IntuitionBase)
  {
    if (IntuitionBase = (struct IntuitionBase *) OpenLibrary ((char *) "intuition.library", 0))
    {
      AutoRequest (NULL, &NoOS2Body1, NULL, &NoOS2Neg, NULL, NULL, 320, 65);
    }
    exit (40);
  }

  SimpleRequest (NULL, "MagicMenu Error", ErrTxt, "Hmpf", NULL, 0, 0);
  exit (40);
}

/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*	       MAIN							    */
/* */
/* ************************************************************************ */
/* ************************************************************************ */

void
main (int argc, char **argv)
{
  BOOL Ok;
  LONG error;
  LONG z1;
  char *SPtr;
  char ZwStr[200];

  if (argc == 0)
  {
    WBMsg = (struct WBStartup *) argv;
    strncpy (ProgName, WBMsg->sm_ArgList->wa_Name, 31);
  }
  else
  {
    WBMsg = NULL;
    z1 = strlen (argv[0]);
    SPtr = (char *) argv[0];
    while (z1 >= 0 && SPtr[z1] != '/' && SPtr[z1] != ':')
      z1--;
    strncpy (ProgName, &argv[0][z1 + 1], 31);
  }

  /*****************************************************************************************/

  WindowGlyphInit();

  /*****************************************************************************************/

  onexit (ExitTrap);

  if (!(IntuitionBase = (struct IntuitionBase *) OpenLibrary ((char *) "intuition.library", 37)))
    ErrorPrc ("");

  if (!(GfxBase = (struct GfxBase *) OpenLibrary ((char *) "graphics.library", 37)))
      ErrorPrc ("Can't open graphics library!");

  GfxVersion = GfxBase->LibNode.lib_Version;

  if (!MemoryInit())
    ErrorPrc ("Cannot set up memory allocator");

  if (!(LayersBase = (struct Library *) OpenLibrary ((char *) "layers.library", 37)))
    ErrorPrc ("Can't open layers library!");

  if (!(UtilityBase = OpenLibrary ("utility.library", 37)))
    ErrorPrc ("Can't open utility library!");

  if (!(IconBase = OpenLibrary ((char *) "icon.library", 37)))
    ErrorPrc ("Can't open icon library!");

  if (!(CxBase = OpenLibrary ((char *) "commodities.library", 37L)))
    ErrorPrc ("Can't open commodities library!");

  if (!(KeymapBase = OpenLibrary ((char *) "keymap.library", 37L)))
    ErrorPrc ("Can't open keymap library!");

  if (!(GetPointerSemaphore = AllocVecPooled (sizeof (struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR)))
      ErrorPrc ("Can't allocate semaphore 1!");

  InitSemaphore (GetPointerSemaphore);

  if (!(MenuActSemaphore = AllocVecPooled (sizeof (struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR)))
      ErrorPrc ("Can't allocate semaphore 2!");

  InitSemaphore (MenuActSemaphore);

  if (!(RememberSemaphore = AllocVecPooled (sizeof (struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR)))
      ErrorPrc ("Can't allocate semaphore 3!");

  InitSemaphore (RememberSemaphore);


  Ok = (NULL != (TimerPort = CreateMsgPort ()));

  if (Ok)
    Ok = (NULL != (TimerIO = (struct timerequest *)
		 CreateIORequest (TimerPort, sizeof (struct timerequest))));

  if (Ok)
    Ok = (NULL == OpenDevice ((char *) TIMERNAME, UNIT_VBLANK, TimerIO, 0));
  if (!Ok)
    ErrorPrc ("Can't initialize timer.device!");

  TimerBase = (struct Library *) TimerIO->tr_node.io_Device;

  Ok = (NULL != (InputPort = CreateMsgPort ()));

  if (Ok)
    Ok = (NULL != (InputIO = (struct IOStdReq *)
		   CreateIORequest (InputPort, sizeof (struct IOStdReq))));

  if (Ok)
    Ok = (NULL == OpenDevice ((char *) INPUTNAME, NULL, InputIO, 0));
  if (!Ok)
    ErrorPrc ("Can't initialize input.device!");

  InputBase = (struct Library *) InputIO->io_Device;

  CheckArguments ();

  AktPrefs = DefaultPrefs;

  if(GetVar(PATHENV,ConfigPath,200,0) == -1)
  {
      Cx_Popup = TRUE;
      strcpy(ConfigPath, "");
  }

  strcpy(ConfigFile,ConfigPath);
  if(! AddPart(ConfigFile,"MagicMenu.config",231))
      ErrorPrc("Can't create Config filename!");

  if(! LoadPrefs(ConfigFile,FALSE))
    Cx_Popup = TRUE;

  if(! (MMMsgPort = CreateMsgPort()))
    ErrorPrc("Can't open MagicMenu MessagePort!");

  MMMsgPort->mp_Node.ln_Name = MMPORT_NAME;
  MMMsgPort->mp_Node.ln_Pri = 0;

  AddPort(MMMsgPort);

  if (!(CxMsgPort = CreateMsgPort ()))
    ErrorPrc ("Can't open CxMsgPort!");

  CxSignalMask = 1l << CxMsgPort->mp_SigBit;


  IntuiMessagePending = 0;
  StripRPort = NULL;
  MenuStripSwapped = FALSE;
  BoxRPort = NULL;
  MenuBoxSwapped = FALSE;
  SubBoxRPort = NULL;
  MenuSubBoxSwapped = FALSE;
  CxChanged = FALSE;
  TickSigNum = -1;

  ObtainSemaphore (RememberSemaphore);

  if(InstallPatches())
    LibPatches = TRUE;
  else
  {
    ReleaseSemaphore (RememberSemaphore);
    ErrorPrc ("Cannot install system patches!");
  }

  if (!MakeGlobalRemember ())
  {
    ReleaseSemaphore (RememberSemaphore);
    ErrorPrc ("Cannot make Menu Remember!");
  }

  ReleaseSemaphore (RememberSemaphore);

  SPrintf (TitleText, "MagicMenu " MAJORVERS " Rev. %ld.%ld (%s):",VERSION,REVISION,DATE);
  NewBroker.nb_Title = TitleText;

  NewBroker.nb_Port = CxMsgPort;
  NewBroker.nb_Pri = Cx_Pri;

  if (!(Broker = (LONG *) CxBroker (&NewBroker, &error)))
  {
    if (error != CBERR_DUP)
      ErrorPrc ("Can't install Broker, System error!");
    CloseAll ();
    return;
  }

  if (!(MouseFilter = CxFilter (MouseKey)))
    ErrorPrc ("Can't install mouse filter");
  SetFilterIX (MouseFilter, &MouseIX);
  AttachCxObj (Broker, MouseFilter);

  if (!(MouseSender = CxSender (CxMsgPort, EVT_MOUSEMENU)))
    ErrorPrc ("Can't install mouse sender");
  AttachCxObj (MouseFilter, MouseSender);

  if (!(MouseTransl = CxTranslate (NULL)))
    ErrorPrc ("Can't install mouse translator");
  AttachCxObj (MouseFilter, MouseTransl);


  if (!(KbdFilter = HotKey (AktPrefs.mmp_KCKeyStr, CxMsgPort, EVT_KBDMENU)))
    ErrorPrc ("Keyboard hotkey sequence invalid");
  AttachCxObj (Broker, KbdFilter);

  if (!(StdKbdFilter = CxFilter (MouseKey)))
    ErrorPrc ("Can't install RAmiga - RCommand keyboard filter");
  SetFilterIX (StdKbdFilter, &StdKbdIX);
  AttachCxObj (Broker, StdKbdFilter);

  if (!(StdKbdSender = CxSender (CxMsgPort, EVT_KBDMENU)))
    ErrorPrc ("Can't install RAmiga - RCommand keyboard sender");
  AttachCxObj (StdKbdFilter, StdKbdSender);

  if (!(StdKbdTransl = CxTranslate (NULL)))
    ErrorPrc ("Can't install RAmiga - RCommand keyboard translator");
  AttachCxObj (StdKbdFilter, StdKbdTransl);

  ActivateCxObj (StdKbdFilter, (AktPrefs.mmp_KCRAltRCommand == 1));


  if (!(PrefsFilter = HotKey (Cx_Popkey, CxMsgPort, EVT_POPPREFS)))
    ErrorPrc ("Popup hotkey sequence invalid");
  AttachCxObj (Broker, PrefsFilter);
/*
  SPrintf(ZwStr,"MagicMenu " MAJORVERS " BETA Rev. %ld.%ld (%s)\n\n"
		"© 1992-1995 Martin Korndörfer\n\n"
		"This is an internal BETA-RELEASE!\n\n"
		"Do not distribute!\nUse at your own risk!",VERSION,REVISION,DATE);

  SimpleRequest(NULL,"MagicMenu",ZwStr,"Ok",NULL,5,0);
*/
  Activate ();

  if (Cx_Popup)
    StartPrefs();

  ProcessCommodity ();

  CloseAll ();

  exit (0);
}
