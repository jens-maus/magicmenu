/*
**	$Id$
**
**	:ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif	/* _GLOBAL_H */

void
DrawMenuItem (struct RastPort *rp, struct MenuItem *Item, LONG x, LONG y, UWORD CmdOffs, BOOL GhostIt, BOOL Highlited)
{
  struct IntuiText *Text;
  struct IntuiText MyText;
  struct Image MyImage;
  struct Image *Image, *RemappedImage;
  UWORD StartX, StartY;
  UWORD ImageYOffs;
  UWORD ZwWidth, NW;
  LONG z1;
  BOOL DrawCheck, YOffsOk, Ghosted, Ok;

  StartX = x + Item->LeftEdge;
  StartY = y + Item->TopEdge;

  ImageYOffs = ((UWORD) Item->Height) / 2;

  CommandText.FrontPen = MenTextCol;
  CommandText.BackPen = MenBackGround;
  CommandText.DrawMode = JAM2;
  CommandText.ITextFont = &MenTextAttr;
  CommandText.TopEdge = 0;

  Ghosted = (GhostIt || ! (Item->Flags & ITEMENABLED));

  YOffsOk = FALSE;

  DrawCheck = ((Item->Flags & CHECKIT) && (!Item->SubItem) && (Item->Flags & HIGHNONE) != HIGHNONE);
  if (Item->Flags & ITEMTEXT)
  {
    if (Highlited && (Item->Flags & HIGHFLAGS) == HIGHIMAGE)
      Text = (struct IntuiText *) Item->SelectFill;
    else
      Text = (struct IntuiText *) Item->ItemFill;

    ZwWidth = 0;

    while (Text)
    {
      MyText = *Text;

      MyText.NextText = NULL;

      if(! LookMC || strcmp(MyText.IText,"»") != 0)
      {
          if(LookMC && Ghosted)
          {
              MyText.DrawMode = JAM1;
              MyText.FrontPen = MenGhostHiCol;

              PrintIText (rp, &MyText, StartX+1, StartY+1);

              MyText.DrawMode = JAM1;
              MyText.FrontPen = MenGhostLoCol;

              PrintIText (rp, &MyText, StartX, StartY);
          }
          else
          {
              if (LookMC)
              {
                  MyText.FrontPen = (Highlited) ? MenHiCol : MenTextCol;
                  MyText.DrawMode = JAM1;
              }
              else if (Look3D)
              {
                if (MyText.BackPen == MenTextCol || MyText.DrawMode == JAM1 || MyText.DrawMode == COMPLEMENT)
                {
                  if (MyText.DrawMode == COMPLEMENT)
                    MyText.DrawMode = JAM1;

                  MyText.FrontPen = MenTextCol;
                  MyText.BackPen = MenBackGround;
                }
                else if (MyText.DrawMode == JAM2 && (MyText.BackPen == MenBackGround || (GfxVersion >= 39 && MyText.BackPen == 2)))
                {
                  MyText.DrawMode = JAM1;
                }
              }
              else
              {
                if (MyText.DrawMode == COMPLEMENT)
                  MyText.DrawMode = JAM1;
              }

              PrintIText (rp, &MyText, StartX, StartY);
          }

          CommandText.ITextFont = MyText.ITextFont;
          CommandText.TopEdge = MyText.TopEdge;

          if (!YOffsOk)
          {
            if (MyText.ITextFont)
              ImageYOffs = (UWORD) MyText.TopEdge + ((UWORD) (MyText.ITextFont->ta_YSize)) / 2;
            else
              ImageYOffs = (UWORD) MyText.TopEdge + ((UWORD) (MenTextAttr.ta_YSize)) / 2;

            YOffsOk = TRUE;
          }

          NW = IntuiTextLength (&MyText) + MyText.LeftEdge;
          if (NW > ZwWidth)
            ZwWidth = NW;
      }

      Text = Text->NextText;
    }
  }
  else
  {
    CommandText.ITextFont = &MenTextAttr;

    if (Highlited)
      Image = (struct Image *) Item->SelectFill;
    else
      Image = (struct Image *) Item->ItemFill;

    while (Image)
    {
      MyImage = *Image;

      MyImage.NextImage = NULL;

      if (Look3D)
      {
          if(LookMC && (MyImage.Depth == 0 || MyImage.ImageData == NULL) && MyImage.Height <= 2)
          {
              SetAPen(rp,MenGhostLoCol);
              RectFill(rp,StartX + MyImage.LeftEdge,StartY + MyImage.TopEdge,
                          StartX + MyImage.LeftEdge + MyImage.Width - 1,StartY + MyImage.TopEdge);
              SetAPen(rp,MenGhostHiCol);
              RectFill(rp,StartX + MyImage.LeftEdge,StartY + MyImage.TopEdge + 1,
                          StartX + MyImage.LeftEdge + MyImage.Width - 1,StartY + MyImage.TopEdge + 1);
          }
          else
          {
              if(LookMC)
                Ok = MakeRemappedImage(&RemappedImage,&MyImage,8,StripDepth,(Highlited) ? DreiDActiveRemapArray :
                                                                                 (Ghosted) ? DreiDGhostedRemapArray :
                                                                                 DreiDRemapArray);
              else
                Ok = MakeRemappedImage(&RemappedImage,&MyImage,8,StripDepth,DreiDRemapArray);

              if(Ok)
              {
                  DrawImage(rp,RemappedImage,StartX,StartY);
                  FreeRemappedImage(RemappedImage);
              }
              else
                  DrawImage (rp, &MyImage, StartX, StartY);
          }
      }
      else
          DrawImage (rp, &MyImage, StartX, StartY);

      Image = Image->NextImage;
    }
    ZwWidth = 0;
  }

  if (DrawCheck)
  {
    if(LookMC)
    {
        if(Item->Flags & CHECKED)
            DrawImage (rp, (Ghosted) ? CheckImageGhosted : (Highlited) ? CheckImageActive : CheckImage,
                                        StartX, StartY + (short) ImageYOffs - (CheckImage->Height / 2));
        else
        {
            NoCheckImage->PlaneOnOff = (Highlited) ? MenFillCol : MenBackGround;
            DrawImage (rp, NoCheckImage, StartX, StartY + (short) ImageYOffs - (NoCheckImage->Height / 2));
        }
    }
    else
    {
        if(Item->MutualExclude == 0)
        {
          if (Item->Flags & CHECKED)
            DrawImage (rp, CheckImage, StartX, StartY + (short) ImageYOffs - (CheckImage->Height / 2));
          else
            DrawImage (rp, NoCheckImage, StartX, StartY + (short) ImageYOffs - (NoCheckImage->Height / 2));
        }
        else if (Item->Flags & CHECKED)
          DrawImage (rp, MXImage, StartX, StartY + (short) ImageYOffs - (MXImage->Height / 2));
        else
          DrawImage (rp, NoMXImage, StartX, StartY + (short) ImageYOffs - (NoMXImage->Height / 2));
    }
  }

  z1 = StartX + Item->Width;

  if (Item->Flags & COMMSEQ)
  {
    CommText[0] = Item->Command;

    CommText[1] = 0;

    if(LookMC)
    {
        if(Ghosted)
        {
            DrawImage (rp, CommandImageGhosted, z1 - CommandImageGhosted->Width - CmdOffs - 2, StartY + ImageYOffs - (CommandImageGhosted->Height / 2));
            CommandText.FrontPen = MenGhostHiCol;
            PrintIText (rp, &CommandText, z1 - CmdOffs + 1, StartY + 1);
            CommandText.FrontPen = MenGhostLoCol;
            CommandText.DrawMode = JAM1;
            PrintIText (rp, &CommandText, z1 - CmdOffs, StartY);
        }
        else
        {
            DrawImage (rp, (Highlited) ? CommandImageActive : CommandImage, z1 - CommandImageGhosted->Width - CmdOffs - 2, StartY + ImageYOffs - (CommandImageGhosted->Height / 2));
            CommandText.FrontPen = (Highlited) ? MenHiCol : MenTextCol;
            CommandText.DrawMode = JAM1;
            PrintIText (rp, &CommandText, z1 - CmdOffs, StartY);
        }
    }
    else
    {
        DrawImage (rp, CommandImage, z1 - CommandImage->Width - CmdOffs - 2, StartY + ImageYOffs - (CommandImage->Height / 2));
        PrintIText (rp, &CommandText, z1 - CmdOffs, StartY);
    }
  }
  else if ((Item->SubItem) && (AktPrefs.mmp_MarkSub == 1))
  {
    if (ZwWidth < Item->Width - SubArrowImage->Width - 5)
    {
        if(LookMC)
            DrawImage (rp, (Highlited) ? SubArrowImageActive :
                           (Ghosted) ? SubArrowImageGhosted : SubArrowImage, z1 - SubArrowImage->Width - 3, StartY + ImageYOffs - (SubArrowImage->Height / 2));
        else
            DrawImage (rp, SubArrowImage, z1 - SubArrowImage->Width - 3, StartY + ImageYOffs - (SubArrowImage->Height / 2));
    }
  }

  if (! LookMC && (GhostIt || !(Item->Flags & ITEMENABLED)))
    GhostRect (rp, StartX, StartY, Item->Width, Item->Height);
}

BOOL
GetSubItemContCoor (struct MenuItem *MenuItem, LONG * t, LONG * l, LONG * w, LONG * h)
{
  *t = MenuItem->TopEdge + SubBoxTopOffs - SelBoxOffs;
  *l = MenuItem->LeftEdge + SubBoxLeftOffs - SelBoxOffs;
  *h = MenuItem->Height + SelBoxOffs * 2;
  *w = MenuItem->Width + SelBoxOffs * 2;
  if (*t < 0 || *l < 0 || *t + *h > SubBoxHeight || *l + *w > SubBoxWidth)
    return (FALSE);
  (*t) += SubBoxDrawTop;
  (*l) += SubBoxDrawLeft;
  return (TRUE);
}

void
CleanUpMenuSubBox (void)
{
  if (AktItemRmb)
  {
    if (AktSubItem)
    {
      AktItemRmb->AktSubItemNum = SubItemNum;
      AktItemRmb->AktSubItem = AktSubItem;
    }
    AktItemRmb = NULL;
  }

  if (MenuSubBoxSwapped)
  {
    if(AktPrefs.mmp_NonBlocking)
    {
        if(SubBoxWin)
        {
            CloseWindow(SubBoxWin);
            SubBoxWin = NULL;
        }
    }
    else if (AktPrefs.mmp_DirectDraw)
    {
      BltBitMap (SubBoxBitMap, SubBoxDrawOffs, 0, ScrRPort.BitMap, SubBoxLeft, SubBoxTop, SubBoxWidth, SubBoxHeight, 0xc0, 0xffff, NULL);
      WaitBlit ();
    }
    else
    {
      SwapBitsRastPortClipRect (&ScrRPort, SubBoxCRect);
      WaitBlit ();
      FreeVecPooled (SubBoxCRect);
      SubBoxCRect = NULL;
    }

    MenuSubBoxSwapped = NULL;
  }

  if (SubBoxRPort)
  {
    FreeRPort (SubBoxRPort, SubBoxBitMap, SubBoxLayerInfo, SubBoxLayer, SubBoxDepth, SubBoxWidth + SubBoxDrawOffs + 16, SubBoxHeight);
    SubBoxRPort = NULL;
  }

  if (AktItem)
    AktItem->Flags &= ~ISDRAWN;
  AktSubItem = NULL;
}

BOOL
DrawHiSubItem (struct MenuItem *Item)
{
  LONG t, l, h, w;
  UWORD HiFlags;

  if (Item)
  {
    if (!GetSubItemContCoor (Item, &t, &l, &w, &h))
      return (FALSE);
    else
    {
      if (MenuMode == MODE_KEYBOARD)
        CheckDispClipVisible (l + (SubBoxLeft - SubBoxDrawLeft), t + (SubBoxTop - SubBoxDrawTop), l + w - 1, t + h - 1);
      HiFlags = Item->Flags & HIGHFLAGS;

      if (HiFlags != HIGHCOMP && HiFlags != HIGHBOX && HiFlags != HIGHIMAGE)
        return (TRUE);

      if (Look3D)
      {
        if (Item->Flags & ITEMENABLED && (!SubBoxGhosted) && (!BoxGhosted))
        {
            if(LookMC)
            {
                HiRect (SubBoxDrawRPort, l, t, w, h, TRUE);
                DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, TRUE);
            }
            else
            {
                if (HiFlags == HIGHIMAGE)
                    DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, TRUE);
                else
                    CompRect (SubBoxDrawRPort, l, t, w, h);
            }
        }
        Draw3DRect (SubBoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
        if ((!(Item->Flags & ITEMENABLED) || SubBoxGhosted || BoxGhosted) && ! LookMC)
          GhostRect (SubBoxDrawRPort, l, t, w, h);
      }
      else
      {
        if (MenuMode == MODE_KEYBOARD || (Item->Flags & ITEMENABLED && (!SubBoxGhosted) && (!BoxGhosted)))
        {
          if (HiFlags == HIGHIMAGE)
            DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, TRUE);
          else
          {
            if (HiFlags == HIGHBOX)
              CompRect (SubBoxDrawRPort, l - 4, t - 2, w + 8, h + 4);
            CompRect (SubBoxDrawRPort, l, t, w, h);
          }
        }
      }
      return (TRUE);
    }
  }
  return (FALSE);
}

BOOL
DrawNormSubItem (struct MenuItem * Item)
{
  LONG t, l, h, w;
  UWORD HiFlags;

  if (Item)
  {
    if (!GetSubItemContCoor (Item, &t, &l, &w, &h))
      return (FALSE);
    else
    {
      HiFlags = Item->Flags & HIGHFLAGS;

      if (HiFlags != HIGHCOMP && HiFlags != HIGHBOX && HiFlags != HIGHIMAGE)
        return (TRUE);

      if (LookMC)
      {
          HiRect (SubBoxDrawRPort, l, t, w, h, FALSE);
          DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE);
      }
      else if (Look3D)
      {
        if (! LookMC && HiFlags != HIGHIMAGE && Item->Flags & ITEMENABLED && (!SubBoxGhosted) && (!BoxGhosted))
          CompRect (SubBoxDrawRPort, l, t, w, h);
        DrawNormRect (SubBoxDrawRPort, l, t, w, h);
        DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE);
      }
      else
      {
        if (HiFlags != HIGHIMAGE)
        {
          if (MenuMode == MODE_KEYBOARD || (Item->Flags & ITEMENABLED && (!SubBoxGhosted) && (!BoxGhosted)))
          {
            if (HiFlags == HIGHBOX)
              CompRect (SubBoxDrawRPort, l - 4, t - 2, w + 8, h + 4);
            CompRect (SubBoxDrawRPort, l, t, w, h);
          }
        }
        DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE);
      }
      return (TRUE);
    }
  }
  return (FALSE);
}

void
DrawMenuSubBoxContents(struct MenuItem *Item, struct RastPort *RPort, UWORD Left, UWORD Top)
{
    register struct MenuItem *ZwItem;

    SetAPen (RPort, MenBackGround);
    SetDrMd (RPort, JAM1);
    RectFill (RPort, Left, Top, Left + SubBoxWidth - 1, Top + SubBoxHeight - 1);

    SetFont (RPort, MenFont);
    Draw3DRect (RPort, Left, Top, SubBoxWidth, SubBoxHeight, TRUE, ScrHiRes, DblBorder);

    SubBoxGhosted = BoxGhosted || ! (Item->Flags & ITEMENABLED);

    ZwItem = SubBoxItems;
    while (ZwItem)
    {
      DrawMenuItem (RPort, ZwItem, SubBoxLeftOffs + Left, SubBoxTopOffs + Top, SubBoxCmdOffs, SubBoxGhosted, FALSE);
      ZwItem = ZwItem->NextItem;
    }
}

BOOL
DrawMenuSubBox (struct Menu * Menu, struct MenuItem * Item, BOOL ActivateItem)
{
  LONG ZwLeft, ZwTop;
  struct ItemRmb *LookItemRmb;
  LONG ItemY1;
  struct MenuItem *ZwItem;

  MenuSubBoxSwapped = FALSE;

  Item->Flags |= ISDRAWN;

  if (!(SubBoxItems = Item->SubItem))
    return (TRUE);

  ObtainSemaphore (RememberSemaphore);

  LookItemRmb = AktMenRmb->FirstItem;
  AktItemRmb = NULL;
  while (LookItemRmb && AktItemRmb == NULL)
  {
    if (LookItemRmb->Item == Item)
      AktItemRmb = LookItemRmb;
    LookItemRmb = LookItemRmb->NextItem;
  }

  ReleaseSemaphore (RememberSemaphore);

  if (AktItemRmb == NULL)
  {
    if (!UpdateRemember (MenWin))
    {
      DoGlobalQuit = TRUE;
      DisplayBeep (NULL);
      return (FALSE);
    }
  }

  SubBoxHeight = AktItemRmb->Height;
  SubBoxWidth = AktItemRmb->Width;
  SubBoxCmdOffs = AktItemRmb->CmdOffs;

  if (SubBoxHeight == 0 || SubBoxWidth == 0)
    return (TRUE);

  SubBoxTopOffs = AktItemRmb->TopOffs;
  SubBoxLeftOffs = AktItemRmb->LeftOffs;
  SubBoxTopBorder = AktItemRmb->TopBorder;
  SubBoxLeftBorder = AktItemRmb->LeftBorder;

  SubBoxDepth = MenScr->BitMap.Depth;

  if (!Look3D)
  {
    SubBoxWidth -= 2;
    SubBoxHeight -= 2;
    SubBoxLeftOffs -= 1;
    SubBoxTopOffs -= 1;
  }

  if (StripPopUp && AktPrefs.mmp_PUCenter)
  {
      ItemY1 = Item->TopEdge + BoxTop + BoxTopOffs;
      ZwItem = AktItemRmb->AktSubItem;
      if (ZwItem)
          ZwTop = ItemY1 + Item->Height / 2 - (ZwItem->TopEdge + ZwItem->Height / 2) - SubBoxTopOffs;
      else
          ZwTop = ItemY1 + Item->Height / 2 - SubBoxHeight / 2;

      SubBoxTop = (ZwTop >= 0) ? ZwTop : 0;
  }
  else
      SubBoxTop = BoxTop + StripTopOffs - 1 + Item->TopEdge + BoxTopOffs - BoxTopBorder + AktItemRmb->ZwTop;

  ZwLeft = BoxLeft + Item->LeftEdge + BoxLeftOffs - BoxLeftBorder + AktItemRmb->ZwLeft;

  if (ZwLeft + SubBoxWidth > MenScr->Width - 1)
  {
      ZwLeft = AktItem->LeftEdge + BoxLeft - SubBoxWidth + BoxLeftOffs + LEFT_OVERLAP;
      SubBoxLeft = (ZwLeft >= 0) ? ZwLeft : 0;
  }
  else if (ZwLeft < 0)
  {
      ZwLeft = AktItem->LeftEdge + AktItem->Width + BoxLeft + BoxLeftOffs - LEFT_OVERLAP;
      SubBoxLeft = (ZwLeft + SubBoxWidth <= MenScr->Width - 1) ? ZwLeft : MenScr->Width - 1 - SubBoxWidth;
  }
  else
      SubBoxLeft = ZwLeft;


  if (SubBoxTop + SubBoxHeight > MenScr->Height - 1)
    SubBoxTop = MenScr->Height - SubBoxHeight;

  if (SubBoxTop < 0 || SubBoxLeft < 0)
    return (FALSE);

  if(AktPrefs.mmp_NonBlocking)
      SubBoxDrawOffs = 0;
  else
      SubBoxDrawOffs = SubBoxLeft % 16;

  SubBoxRightEdge = SubBoxWidth - SubBoxLeftBorder * 2;

  if(! (AktPrefs.mmp_DirectDraw && AktPrefs.mmp_NonBlocking))
  {
      if (!InstallRPort (SubBoxDepth, SubBoxWidth + SubBoxDrawOffs + 16, SubBoxHeight, &SubBoxRPort, &SubBoxBitMap, &SubBoxLayerInfo, &SubBoxLayer,
                         MenScr->RastPort.BitMap))
      {
        CleanUpMenuSubBox ();
        return (FALSE);
      }
  }

  if (AktPrefs.mmp_DirectDraw)
  {
      if(AktPrefs.mmp_NonBlocking)
      {
          if(! (SubBoxWin = OpenWindowTags(NULL,
                            WA_Left,            SubBoxLeft,
                            WA_Top,             SubBoxTop,
                            WA_Width,           SubBoxWidth,
                            WA_Height,          SubBoxHeight,
                            WA_CustomScreen,    MenScr,
                            WA_Borderless,      TRUE,
                            WA_Activate,        FALSE,
                            WA_RMBTrap,         TRUE,
                            WA_SmartRefresh,    TRUE,
                            WA_ScreenTitle,     MenWin->ScreenTitle,
                            TAG_DONE)))
          {
              CleanUpMenuSubBox();
              return(FALSE);
          }

          SubBoxDrawRPort = SubBoxWin->RPort;
          SubBoxDrawLeft = 0;
          SubBoxDrawTop = 0;
      }
      else
      {
          BltBitMap (ScrRPort.BitMap, SubBoxLeft, SubBoxTop, SubBoxBitMap, SubBoxDrawOffs, 0, SubBoxWidth, SubBoxHeight, 0xc0, 0xffff, NULL);
          WaitBlit ();
          SubBoxDrawRPort = &ScrRPort;
          SubBoxDrawLeft = SubBoxLeft;
          SubBoxDrawTop = SubBoxTop;
      }

      DrawMenuSubBoxContents(Item,SubBoxDrawRPort,SubBoxDrawLeft,SubBoxDrawTop);
  }
  else
  {
      DrawMenuSubBoxContents(Item,SubBoxRPort,SubBoxDrawOffs,0);

      if(AktPrefs.mmp_NonBlocking)
      {
          if(! (SubBoxWin = OpenWindowTags(NULL,
                            WA_Left,            SubBoxLeft,
                            WA_Top,             SubBoxTop,
                            WA_Width,           SubBoxWidth,
                            WA_Height,          SubBoxHeight,
                            WA_CustomScreen,    MenScr,
                            WA_SuperBitMap,     SubBoxBitMap,
                            WA_Borderless,      TRUE,
                            WA_Activate,        FALSE,
                            WA_RMBTrap,         TRUE,
                            WA_ScreenTitle,     MenWin->ScreenTitle,
                            TAG_DONE)))
          {
              CleanUpMenuSubBox();
              return(FALSE);
          }

          SubBoxDrawRPort = SubBoxWin->RPort;
          SubBoxDrawLeft = 0;
          SubBoxDrawTop = 0;
      }
      else
      {
          if (!(SubBoxCRect = GetClipRect (SubBoxBitMap, SubBoxLeft, SubBoxTop, SubBoxLeft + SubBoxWidth - 1, SubBoxTop + SubBoxHeight - 1)))
          {
            CleanUpMenuSubBox ();
            return (FALSE);
          }

          SwapBitsRastPortClipRect (&ScrRPort, SubBoxCRect);
          WaitBlit ();
          SubBoxDrawRPort = &ScrRPort;
          SubBoxDrawLeft = SubBoxLeft;
          SubBoxDrawTop = SubBoxTop;
      }
  }

  SetFont(SubBoxDrawRPort,MenFont);
  SetABPenDrMd(SubBoxDrawRPort,MenTextCol,MenBackGround,JAM1);

  MenuSubBoxSwapped = TRUE;

  AktSubItem = NULL;


  if (ActivateItem)
  {
    if (MenuMode == MODE_KEYBOARD)
      if (AktItemRmb->AktSubItem != NULL)
        ChangeAktSubItem (AktItemRmb->AktSubItem, AktItemRmb->AktSubItemNum);
      else
        SelectNextSubItem (-1);

    if (MenuMode != MODE_KEYBOARD && (MenuMode != MODE_SELECT || SelectSpecial))
      LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
  }

  return (TRUE);
}

BOOL
GetItemContCoor (struct MenuItem * MenuItem, LONG * t, LONG * l, LONG * w, LONG * h)
{
  *t = MenuItem->TopEdge + BoxTopOffs - SelBoxOffs;
  *l = MenuItem->LeftEdge + BoxLeftOffs - SelBoxOffs;
  *h = MenuItem->Height + SelBoxOffs * 2;
  *w = MenuItem->Width + SelBoxOffs * 2;
  if (*t < 0 || *l < 0 || *t + *h > BoxHeight || *l + *w > BoxWidth)
    return (FALSE);
  (*t) += BoxDrawTop;
  (*l) += BoxDrawLeft;
  return (TRUE);
}

void
CleanUpMenuBox (void)
{
  if (AktMenRmb)
  {
    if (AktItem)
    {
      AktMenRmb->AktItemNum = ItemNum;
      AktMenRmb->AktItem = AktItem;
    }
    AktMenRmb = NULL;
  }

  if (MenuBoxSwapped)
  {
    if(AktPrefs.mmp_NonBlocking)
    {
        if(BoxWin)
        {
            CloseWindow(BoxWin);
            BoxWin = NULL;
        }
    }
    else if (AktPrefs.mmp_DirectDraw)
    {
        BltBitMap (BoxBitMap, BoxDrawOffs, 0, ScrRPort.BitMap, BoxLeft, BoxTop, BoxWidth, BoxHeight, 0xc0, 0xffff, NULL);
        WaitBlit ();
    }
    else
    {
        SwapBitsRastPortClipRect (&ScrRPort, BoxCRect);
        WaitBlit ();
        FreeVecPooled (BoxCRect);
        BoxCRect = NULL;
    }
    MenuBoxSwapped = NULL;
  }

  if (BoxRPort)
  {
    FreeRPort (BoxRPort, BoxBitMap, BoxLayerInfo, BoxLayer, BoxDepth, BoxWidth + BoxDrawOffs + 16, BoxHeight);
    BoxRPort = NULL;
  }

  if (AktMenu)
    AktMenu->Flags &= ~MIDRAWN;
  AktItem = NULL;
}

BOOL
DrawHiItem (struct MenuItem *Item)
{
  LONG t, l, h, w;
  UWORD HiFlags;

  if (Item)
  {
    if (!GetItemContCoor (Item, &t, &l, &w, &h))
      return (FALSE);
    else
    {
      if (MenuMode == MODE_KEYBOARD)
        CheckDispClipVisible (l + (BoxLeft - BoxDrawLeft), t + (BoxTop - BoxDrawTop), l + w - 1, t + h - 1);

      HiFlags = Item->Flags & HIGHFLAGS;

      if (HiFlags != HIGHCOMP && HiFlags != HIGHBOX && HiFlags != HIGHIMAGE)
        return (TRUE);

      if (Look3D)
      {
          if (Item->Flags & ITEMENABLED && (!BoxGhosted))
          {
              if(LookMC)
              {
                  HiRect (BoxDrawRPort, l, t, w, h, TRUE);
                  DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, TRUE);
              }
              else
              {
                if (HiFlags == HIGHIMAGE)
                  DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, TRUE);
                else
                  CompRect (BoxDrawRPort, l, t, w, h);
              }
          }
          Draw3DRect (BoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
          if ((!(Item->Flags & ITEMENABLED) || BoxGhosted) && ! LookMC)
              GhostRect (BoxDrawRPort, l, t, w, h);
      }
      else
      {
        if (MenuMode == MODE_KEYBOARD || (Item->Flags & ITEMENABLED && (!BoxGhosted)))
        {
          if (HiFlags == HIGHIMAGE)
            DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, TRUE);
          else
          {
            if (HiFlags == HIGHBOX)
              CompRect (BoxDrawRPort, l - 4, t - 2, w + 8, h + 4);
            CompRect (BoxDrawRPort, l, t, w, h);
          }
        }
      }

      return (TRUE);
    }
  }
  return (FALSE);
}

BOOL
DrawNormItem (struct MenuItem * Item)
{
  LONG t, l, h, w;
  UWORD HiFlags;

  if (Item)
  {
    if (!GetItemContCoor (Item, &t, &l, &w, &h))
      return (FALSE);
    else
    {
      HiFlags = Item->Flags & HIGHFLAGS;

      if (HiFlags != HIGHCOMP && HiFlags != HIGHBOX && HiFlags != HIGHIMAGE)
        return (TRUE);

      if (LookMC)
      {
          HiRect (BoxDrawRPort, l, t, w, h,FALSE);
          DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE);
      }
      else if (Look3D)
      {
        if (HiFlags != HIGHIMAGE && Item->Flags & ITEMENABLED && (!BoxGhosted))
          CompRect (BoxDrawRPort, l, t, w, h);
        DrawNormRect (BoxDrawRPort, l, t, w, h);
        DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE);
      }
      else
      {
        if (HiFlags != HIGHIMAGE)
        {
          if (MenuMode == MODE_KEYBOARD || (Item->Flags & ITEMENABLED && (!BoxGhosted)))
          {
            if (HiFlags == HIGHBOX)
              CompRect (BoxDrawRPort, l - 4, t - 2, w + 8, h + 4);
            CompRect (BoxDrawRPort, l, t, w, h);
          }
        }
        DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE);
      }
      return (TRUE);
    }
  }
  return (FALSE);
}

void
DrawMenuBoxContents(struct Menu *Menu, struct RastPort *RPort, UWORD Left, UWORD Top)
{
    register struct MenuItem *ZwItem;
    LONG x1, x2;

    SetAPen (RPort, MenBackGround);
    SetDrMd (RPort, JAM1);
    RectFill (RPort, Left, Top, Left + BoxWidth - 1, Top + BoxHeight - 1);

    SetFont (RPort, MenFont);
    Draw3DRect (RPort, Left, Top, BoxWidth, BoxHeight, TRUE, ScrHiRes, DblBorder);

    SetAPen (RPort, MenBackGround);
    SetDrMd (RPort, JAM1);

    if (!StripPopUp && Look3D && (! LookMC) && (!DblBorder))
    {
        x1 = Menu->LeftEdge - BoxLeft + StripLeftOffs + ((ScrHiRes) ? 2 : 1);
        if (x1 < 0)
          x1 = 0;
        x2 = x1 + Menu->Width - ((ScrHiRes) ? 4 : 2);
        if (x2 >= BoxWidth)
          x2 = BoxWidth - 1;
        Move (RPort, Left + x1, Top);
        Draw (RPort, Left + x2, Top);
        if (!BoxGhosted)
        {
            SetAPen (RPort, MenComplMsk);
            Move (RPort, Left + x1, Top);
            Draw (RPort, Left + x2, Top);
            SetDrMd (RPort, JAM1);
            SetWrMsk (RPort, 0xff);
        }
    }

    ZwItem = BoxItems;
    while (ZwItem)
    {
        DrawMenuItem (RPort, ZwItem, BoxLeftOffs + Left, BoxTopOffs + Top, BoxCmdOffs, BoxGhosted, FALSE);
        ZwItem = ZwItem->NextItem;
    }
}

BOOL
DrawMenuBox (struct Menu * Menu, BOOL ActivateItem)
{
  LONG ZwLeft, ZwTop;
  struct MenuRmb *LookMenRmb;
  LONG MenuY1;
  struct MenuItem *ZwItem;

  MenuBoxSwapped = FALSE;

  BoxItems = Menu->FirstItem;

  Menu->Flags |= MIDRAWN;

  ObtainSemaphore (RememberSemaphore);

  LookMenRmb = AktMenuRemember->FirstMenu;
  AktMenRmb = NULL;
  while (LookMenRmb != NULL && AktMenRmb == NULL)
  {
    if (LookMenRmb->Menu == Menu)
      AktMenRmb = LookMenRmb;
    LookMenRmb = LookMenRmb->NextMenu;
  }

  ReleaseSemaphore (RememberSemaphore);

  if (AktMenRmb == NULL)
  {
    if (!UpdateRemember (MenWin))
    {
      DoGlobalQuit = TRUE;
      DisplayBeep (NULL);
      return (FALSE);
    }
  }

  BoxHeight = AktMenRmb->Height;
  BoxWidth = AktMenRmb->Width;
  BoxCmdOffs = AktMenRmb->CmdOffs;

  if (BoxHeight == 0 || BoxWidth == 0)
    return (TRUE);

  BoxLeftBorder = AktMenRmb->LeftBorder;
  BoxTopBorder = AktMenRmb->TopBorder;

  BoxTopOffs = AktMenRmb->TopOffs;
  BoxLeftOffs = AktMenRmb->LeftOffs;

  BoxDepth = MenScr->BitMap.Depth;

  if (!Look3D)
  {
    BoxWidth -= 2;
    BoxHeight -= 2;
    BoxLeftOffs -= 1;
    BoxTopOffs -= 1;
  }

  if (!StripPopUp)
  {
    if (Menu->Width > BoxWidth)
      BoxWidth = Menu->Width;

    BoxTop = StripMinHeight + StripTopOffs - 1 + AktMenRmb->ZwTop;
    if(LookMC)
        BoxTop += 2;

    ZwLeft = Menu->LeftEdge + AktMenRmb->ZwLeft;

    if (ZwLeft + BoxWidth > MenScr->Width - 1)
      BoxLeft = MenScr->Width - BoxWidth;
    else if (ZwLeft < 0)
      BoxLeft = 0;
    else
      BoxLeft = ZwLeft;

  }
  else
  {
    MenuY1 = MenuNum * (MenFont->tf_YSize + STRIP_YDIST) + StripTop + StripTopOffs;

    if (AktPrefs.mmp_PUCenter)
    {
      ZwItem = AktMenRmb->AktItem;
      if (ZwItem)
        ZwTop = MenuY1 + StripMinHeight / 2 - (ZwItem->TopEdge + ZwItem->Height / 2) - BoxTopOffs;
      else
        ZwTop = MenuY1 + StripMinHeight / 2 - BoxHeight / 2;
      BoxTop = (ZwTop >= 0) ? ZwTop : 0;
    }
    else
      BoxTop = MenuY1 - 3;

    ZwLeft = StripLeft + StripMinWidth - 5;

    if (ZwLeft + BoxWidth > MenScr->Width - 1)
    {
        ZwLeft = StripLeft - BoxWidth + StripLeftOffs + LEFT_OVERLAP;
        BoxLeft = (ZwLeft >= 0) ? ZwLeft : 0;
    }
    else if (ZwLeft < 0)
      BoxLeft = 0;
    else
      BoxLeft = ZwLeft;
  }

  if (BoxTop + BoxHeight > MenScr->Height - 1)
    BoxTop = MenScr->Height - BoxHeight;

  if (BoxWidth > MenScr->Width || BoxHeight > MenScr->Height)
    return (FALSE);

  if(AktPrefs.mmp_NonBlocking)
      BoxDrawOffs = 0;
  else
      BoxDrawOffs = BoxLeft % 16;

  BoxRightEdge = BoxWidth - BoxLeftBorder * 2;

  if(! (AktPrefs.mmp_DirectDraw && AktPrefs.mmp_NonBlocking))
  {
      if (!InstallRPort (BoxDepth, BoxWidth + BoxDrawOffs + 16, BoxHeight, &BoxRPort, &BoxBitMap, &BoxLayerInfo, &BoxLayer,
                         MenScr->RastPort.BitMap))
      {
        CleanUpMenuBox ();
        return (FALSE);
      }
  }

  BoxGhosted = !(Menu->Flags & MENUENABLED);

  if (AktPrefs.mmp_DirectDraw)
  {
      if(AktPrefs.mmp_NonBlocking)
      {
          if(! (BoxWin = OpenWindowTags(NULL,
                            WA_Left,            BoxLeft,
                            WA_Top,             BoxTop,
                            WA_Width,           BoxWidth,
                            WA_Height,          BoxHeight,
                            WA_CustomScreen,    MenScr,
                            WA_Borderless,      TRUE,
                            WA_Activate,        FALSE,
                            WA_RMBTrap,         TRUE,
                            WA_SmartRefresh,    TRUE,
                            WA_ScreenTitle,     MenWin->ScreenTitle,
                            TAG_DONE)))
          {
              CleanUpMenuBox();
              return(FALSE);
          }

          BoxDrawRPort = BoxWin->RPort;
          BoxDrawLeft = 0;
          BoxDrawTop = 0;
      }
      else
      {
          BltBitMap (ScrRPort.BitMap, BoxLeft, BoxTop, BoxBitMap, BoxDrawOffs, 0, BoxWidth, BoxHeight, 0xc0, 0xffff, NULL);
          WaitBlit ();
          BoxDrawRPort = &ScrRPort;
          BoxDrawLeft = BoxLeft;
          BoxDrawTop = BoxTop;
      }

      DrawMenuBoxContents(Menu,BoxDrawRPort,BoxDrawLeft,BoxDrawTop);
  }
  else
  {
      DrawMenuBoxContents(Menu,BoxRPort,BoxDrawOffs,0);

      if(AktPrefs.mmp_NonBlocking)
      {
          if(! (BoxWin = OpenWindowTags(NULL,
                            WA_Left,            BoxLeft,
                            WA_Top,             BoxTop,
                            WA_Width,           BoxWidth,
                            WA_Height,          BoxHeight,
                            WA_CustomScreen,    MenScr,
                            WA_SuperBitMap,     BoxBitMap,
                            WA_Borderless,      TRUE,
                            WA_Activate,        FALSE,
                            WA_RMBTrap,         TRUE,
                            WA_ScreenTitle,     MenWin->ScreenTitle,
                            TAG_DONE)))
          {
              CleanUpMenuBox();
              return(FALSE);
          }

          BoxDrawRPort = BoxWin->RPort;
          BoxDrawLeft = 0;
          BoxDrawTop = 0;
      }
      else
      {
          if (!(BoxCRect = GetClipRect (BoxBitMap, BoxLeft, BoxTop, BoxLeft + BoxWidth - 1, BoxTop + BoxHeight - 1)))
          {
            CleanUpMenuBox ();
            return (FALSE);
          }

          SwapBitsRastPortClipRect (&ScrRPort, BoxCRect);

          WaitBlit ();

          BoxDrawRPort = &ScrRPort;
          BoxDrawLeft = BoxLeft;
          BoxDrawTop = BoxTop;
      }
  }

  SetFont(BoxDrawRPort,MenFont);
  SetABPenDrMd(BoxDrawRPort,MenTextCol,MenBackGround,JAM1);

  MenuBoxSwapped = TRUE;

  AktItem = NULL;

  if (ActivateItem)
  {
    if (MenuMode == MODE_KEYBOARD)
    {
      if (AktMenRmb->AktItem != NULL)
        ChangeAktItem (AktMenRmb->AktItem, AktMenRmb->AktItemNum);
      else
        SelectNextItem (-1);
    }

    if (MenuMode != MODE_KEYBOARD && (MenuMode != MODE_SELECT || SelectSpecial))
      LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
  }

  return (TRUE);
}

BOOL
GetMenuContCoor (struct Menu * Menu, LONG * t, LONG * l, LONG * w, LONG * h)
{
  struct Menu *ZwMen;

  if (!StripPopUp)
  {
    *t = Menu->TopEdge + StripTopOffs;
    *l = Menu->LeftEdge + StripLeftOffs;
    *h = StripMinHeight;
    *w = Menu->Width;
  }
  else
  {
    *t = StripTopOffs;
    ZwMen = MenStrip;
    while (ZwMen != Menu)
    {
      (*t) += MenFont->tf_YSize + STRIP_YDIST;
      ZwMen = ZwMen->NextMenu;
    }
    *l = StripLeftOffs;
    *h = (*t + StripMinHeight <= StripHeight) ? StripMinHeight : StripHeight - *t;
    *w = (*l + StripMinWidth <= StripWidth) ? StripMinWidth : StripWidth - *l;
  }
  if (*t < 0)
    *t = 0;
  if (*l < 0)
    *l = 0;
  if (*t + *h > StripHeight)
    *h = StripHeight - *t;
  if (*l + *w > StripWidth)
    *w = StripWidth - *l;

  if (*t < 0 || *l < 0 || *t + *h > StripHeight || *l + *w > StripWidth)
    return (FALSE);

  return (TRUE);
}

BOOL
DrawHiMenu (struct Menu * Menu)
{
  LONG t, l, h, w;

  if (Menu)
  {
    if (!GetMenuContCoor (Menu, &t, &l, &w, &h))
      return (FALSE);
    else
    {
      t += StripDrawTop;
      l += StripDrawLeft;
      if (MenuMode == MODE_KEYBOARD)
        CheckDispClipVisible (l + (StripLeft - StripDrawLeft), t + (StripTop - StripDrawTop), l + w - 1, t + h - 1);

      if (LookMC)
      {
          SetFont (StripDrawRPort, MenFont);
          SetDrMd (StripDrawRPort, JAM1);
          if (Menu->Flags & MENUENABLED)
          {
              HiRect (StripDrawRPort, l, t, w, h + 1, TRUE);
              Move (StripDrawRPort, l + ((StripPopUp) ? 2 : 3), t + 1 + MenFont->tf_Baseline);
              SetAPen (StripDrawRPort, MenHiCol);
              Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
          }
          else
          {
              Move (StripDrawRPort, l + ((StripPopUp) ? 3 : 4), t + 1 + MenFont->tf_Baseline + 1);
              SetAPen (StripDrawRPort, MenGhostHiCol);
              Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
              Move (StripDrawRPort, l + ((StripPopUp) ? 2 : 3), t + 1 + MenFont->tf_Baseline);
              SetAPen (StripDrawRPort, MenGhostLoCol);
              Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
          }

          Draw3DRect (StripDrawRPort, l, t, w, h + 1, (AktPrefs.mmp_DblBorder || (!StripPopUp)), ((!StripPopUp) && ScrHiRes && (!DblBorder)), FALSE);
      }
      else if (Look3D)
      {
        if (Menu->Flags & MENUENABLED)
          CompRect (StripDrawRPort, l, t, w, h);
        Draw3DRect (StripDrawRPort, l, t, w, h, (AktPrefs.mmp_DblBorder || (!StripPopUp)), ((!StripPopUp) && ScrHiRes && (!DblBorder)), FALSE);
        if (!(Menu->Flags & MENUENABLED))
          GhostRect (StripDrawRPort, l, t, w, h);
      }
      else
      {
        if (Menu->Flags & MENUENABLED)
          CompRect (StripDrawRPort, l, t, w, h);
      }
      return (TRUE);
    }
  }
  return (FALSE);
}

BOOL
GhostMenu (struct Menu * Menu, struct RastPort *RPort, UWORD Left, UWORD Top)
{
  LONG t, l, h, w;

  if (Menu)
  {
    if (!GetMenuContCoor (Menu, &t, &l, &w, &h))
      return (FALSE);
    else
      GhostRect (RPort, Left + l, Top + t, w, h);
    return (TRUE);
  }
  return (FALSE);
}

void
DrawNormMenu (struct Menu *Menu)
{
  LONG t, l, h, w;

  if (Menu)
  {
    GetMenuContCoor (Menu, &t, &l, &w, &h);
    if (t >= 0 && t < StripHeight && l >= 0 && l < StripWidth)
    {
      t += StripDrawTop;
      l += StripDrawLeft;
      if (LookMC)
      {
          SetFont (StripDrawRPort, MenFont);
          HiRect (StripDrawRPort, l, t, w, h + 1, FALSE);
          SetDrMd (StripDrawRPort, JAM1);
          if (Menu->Flags & MENUENABLED)
          {
              Move (StripDrawRPort, l + ((StripPopUp) ? 2 : 3), t + 1 + MenFont->tf_Baseline);
              SetAPen (StripDrawRPort, MenTextCol);
              Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
          }
          else
          {
              Move (StripDrawRPort, l + ((StripPopUp) ? 3 : 4), t + 1 + MenFont->tf_Baseline + 1);
              SetAPen (StripDrawRPort, MenGhostHiCol);
              Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
              Move (StripDrawRPort, l + ((StripPopUp) ? 2 : 3), t + 1 + MenFont->tf_Baseline);
              SetAPen (StripDrawRPort, MenGhostLoCol);
              Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
          }

          if(! StripPopUp)
          {
              SetAPen(StripDrawRPort,MenDarkEdge);
              RectFill(StripDrawRPort,l,t+h,l+w-1,t+h);
          }
      }
      else if (Look3D)
      {
        if (Menu->Flags & MENUENABLED)
          CompRect (StripDrawRPort, l, t, w, h);
        DrawNormRect (StripDrawRPort, l, t, w, h);
        if (!StripPopUp)
        {
          DrawNormRect (StripDrawRPort, l + 1, t, w - 2, h);
          Move (StripDrawRPort, Menu->LeftEdge + StripLeftOffs + 4,
                Menu->TopEdge + StripTopOffs + 1 + MenFont->tf_Baseline);
          SetAPen (StripDrawRPort, MenTextCol);
          SetFont (StripDrawRPort, MenFont);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
          if (!Menu->Flags & MENUENABLED)
            GhostRect (StripDrawRPort, l, t, w, h);
        }
      }
      else
      {
        if (Menu->Flags & MENUENABLED)
          CompRect (StripDrawRPort, l, t, w, h);
      }
    }
  }
}

void
CleanUpMenuStrip (void)
{
  if (AktMenuRemember)
  {
    if (AktMenu)
    {
      AktMenuRemember->AktMenuNum = MenuNum;
      AktMenuRemember->AktMenu = AktMenu;
    }
    AktMenuRemember = NULL;
  }

  FreeRemappedImage (CheckImageRmp);
  FreeRemappedImage (CheckImageGhosted);
  FreeRemappedImage (CheckImageActive);

  FreeRemappedImage (MXImageRmp);
  FreeRemappedImage (NoMXImageRmp);

  FreeRemappedImage (CommandImageRmp);
  FreeRemappedImage (CommandImageGhosted);
  FreeRemappedImage (CommandImageActive);

  FreeRemappedImage (SubArrowImageRmp);
  FreeRemappedImage (SubArrowImageGhosted);
  FreeRemappedImage (SubArrowImageActive);

  CheckImageRmp = NoCheckImage = MXImageRmp = NoMXImageRmp = CommandImageRmp = SubArrowImageRmp = NULL;
  CheckImageGhosted = CommandImageGhosted = SubArrowImageGhosted = NULL;
  CheckImageActive = CommandImageActive = SubArrowImageActive = NULL;

  if(LookMC)
  {
      MyReleasePen(MenScr,MenLightEdge);
      MyReleasePen(MenScr,MenDarkEdge);
      MyReleasePen(MenScr,MenBackGround);
      MyReleasePen(MenScr,MenTextCol);
      MyReleasePen(MenScr,MenHiCol);
      MyReleasePen(MenScr,MenFillCol);
      MyReleasePen(MenScr,MenGhostLoCol);
      MyReleasePen(MenScr,MenGhostHiCol);
      MyReleasePen(MenScr,MenXENGrey0);
      MyReleasePen(MenScr,MenXENBlack);
      MyReleasePen(MenScr,MenXENWhite);
      MyReleasePen(MenScr,MenXENBlue);
      MyReleasePen(MenScr,MenXENGrey1);
      MyReleasePen(MenScr,MenXENGrey2);
      MyReleasePen(MenScr,MenXENBeige);
      MyReleasePen(MenScr,MenXENPink);
  }

  if (MenuStripSwapped)
  {
    if(AktPrefs.mmp_NonBlocking)
    {
        if(StripWin)
            CloseWindow(StripWin);
        StripWin = NULL;
    }
    else if(AktPrefs.mmp_DirectDraw)
    {
        BltBitMap (StripBitMap, StripDrawOffs, 0, ScrRPort.BitMap, StripLeft, StripTop, StripWidth, StripHeight, 0xc0, 0xffff, NULL);
        WaitBlit ();
    }
    else
    {
        SwapBitsRastPortClipRect (&ScrRPort, StripCRect);
        WaitBlit ();
        FreeVecPooled (StripCRect);
        StripCRect = NULL;
    }
    MenuStripSwapped = FALSE;
  }

  if (StripRPort)
  {
    FreeRPort (StripRPort, StripBitMap, StripLayerInfo, StripLayer, StripDepth, StripWidth + StripDrawOffs + 16, StripHeight);
    StripRPort = NULL;
  }

  AktMenu = NULL;

  if(MenOpenedFont)
  {
      CloseFont(MenOpenedFont);
      MenOpenedFont = NULL;
  }
}

void
GetSubItemCoors (struct MenuItem *Item, UWORD * x1, UWORD * y1, UWORD * x2, UWORD * y2)
{
  *x1 = Item->LeftEdge + SubBoxLeftOffs + SubBoxLeft;
  *y1 = Item->TopEdge + SubBoxTopOffs + SubBoxTop;
  *x2 = *x1 + Item->Width - 1;
  *y2 = *y1 + Item->Height - 1;
}

void
GetItemCoors (struct MenuItem *Item, UWORD * x1, UWORD * y1, UWORD * x2, UWORD * y2)
{
  *x1 = Item->LeftEdge + BoxLeftOffs + BoxLeft;
  *y1 = Item->TopEdge + BoxTopOffs + BoxTop;
  *x2 = *x1 + Item->Width - 1;
  *y2 = *y1 + Item->Height - 1;
}

void
GetMenuCoors (struct Menu *Menu, UWORD * x1, UWORD * y1, UWORD * x2, UWORD * y2)
{
  struct Menu *ZwMen;

  if (!StripPopUp)
  {
    *x1 = Menu->LeftEdge + StripLeftOffs;
    *y1 = Menu->TopEdge /* + StripTopOffs */ ;  /* StripTopOffs lasse ich
                                                   weg, besser zu bedienen */
    *x2 = *x1 + Menu->Width - 1;
    *y2 = *y1 + StripMinHeight + StripTopOffs;
  }
  else
  {
    *x1 = StripLeft + StripLeftOffs;
    *x2 = *x1 + StripMinWidth;

    *y1 = StripTop + StripTopOffs;
    ZwMen = MenStrip;
    while (ZwMen != Menu)
    {
      (*y1) += MenFont->tf_YSize + STRIP_YDIST;
      ZwMen = ZwMen->NextMenu;
    }
    *y2 = *y1 + StripMinHeight;
  }
}

void
ChangeAktSubItem (struct MenuItem *NewItem, UWORD NewSubItemNum)
{
  if (AktSubItem)
  {
    DrawNormSubItem (AktSubItem);
    AktSubItem->Flags &= ~HIGHITEM;
    AktSubItem = NULL;
    SubItemNum = NOSUB;
  }

  if (NewItem)
  {
    SubItemNum = NewSubItemNum;
    if (DrawHiSubItem (AktSubItem = NewItem))
      if (AktSubItem)
        AktSubItem->Flags |= HIGHITEM;
      else
      {
        SubItemNum = NOSUB;
        AktSubItem = NULL;
      }
  }
}

BOOL
FindSubItemChar (char Search, BOOL * Single)
{
  struct MenuItem *ZwItem, *OldAktItem;
  UWORD ZwItemNum;
  BOOL FirstFound, AnotherFound, Found;
  char LookChar;

  Search = toupper (Search);

  if (!AktSubItem)
    return (FALSE);

  OldAktItem = AktSubItem;

  ZwItem = AktSubItem;
  ZwItemNum = SubItemNum;

  AnotherFound = FALSE;
  FirstFound = FALSE;

  do
  {
    ZwItem = ZwItem->NextItem;
    ZwItemNum++;

    if (ZwItem == NULL)
    {
      ZwItem = SubBoxItems;
      ZwItemNum = 0;
    }

    if (ZwItem && (ZwItem->Flags & ITEMTEXT) != NULL)
    {
      LookChar = ((struct IntuiText *) ZwItem->ItemFill)->IText[0];
      Found = toupper (LookChar) == Search;
    }
    else
      Found = FALSE;

    if (Found)
    {
      if (!FirstFound)
      {
        FirstFound = TRUE;
        ChangeAktSubItem (ZwItem, ZwItemNum);
      }
      else
        AnotherFound = TRUE;
    }
  }
  while (ZwItem != OldAktItem && (!AnotherFound));

  *Single = (!AnotherFound);
  return (FirstFound);
}

UWORD
SelectNextSubItem (WORD NeuItemNum)
{
  struct MenuItem *ZwItem;
  WORD z1;

  ZwItem = SubBoxItems;

  z1 = 0;
  while (ZwItem && (((ZwItem->Flags & HIGHNONE) == HIGHNONE) || z1 <= NeuItemNum))
  {
    ZwItem = ZwItem->NextItem;
    z1++;
  }
  if (ZwItem && (z1 != SubItemNum || AktSubItem == NULL))
  {
    ChangeAktSubItem (ZwItem, z1);
    return ((UWORD) z1);
  }
  return (SubItemNum);
}

UWORD
SelectPrevSubItem (WORD NeuItemNum)
{
  struct MenuItem *ZwItem, *PrevItem;
  WORD z1, PrevNum;

  ZwItem = SubBoxItems;
  PrevItem = AktSubItem;
  PrevNum = SubItemNum;

  z1 = 0;
  while (ZwItem && (((ZwItem->Flags & HIGHNONE) == HIGHNONE) || z1 < NeuItemNum))
  {
    if ((ZwItem->Flags & HIGHNONE) != HIGHNONE)
    {
      PrevItem = ZwItem;
      PrevNum = z1;
    }
    ZwItem = ZwItem->NextItem;
    z1++;
  }

  if (PrevNum != SubItemNum || AktSubItem == NULL)
  {
    ChangeAktSubItem (PrevItem, PrevNum);
    return ((UWORD) PrevNum);
  }
  return (SubItemNum);
}

void
ChangeAktItem (struct MenuItem *NewItem, UWORD NewItemNum)
{
  if (AktItem)
  {
    CleanUpMenuSubBox ();
    DrawNormItem (AktItem);
    AktItem->Flags &= ~HIGHITEM;
    AktItem = NULL;
    ItemNum = NOITEM;
  }

  if (NewItem)
  {
    ItemNum = NewItemNum;
    if (DrawHiItem (AktItem = NewItem))
    {
      AktItem->Flags |= HIGHITEM;
      if (MenuMode != MODE_KEYBOARD)
      {
        if (!DrawMenuSubBox (AktMenu, AktItem, TRUE))
          DisplayBeep (MenScr);
      }
    }
    else
    {
      ItemNum = NOITEM;
      AktItem = NULL;
    }
  }
}

BOOL
FindItemChar (char Search, BOOL * Single)
{
  struct MenuItem *ZwItem, *OldAktItem;
  UWORD ZwItemNum;
  BOOL FirstFound, AnotherFound, Found;
  char LookChar;

  Search = toupper (Search);

  if (!AktItem)
    return (FALSE);

  OldAktItem = AktItem;

  ZwItem = AktItem;
  ZwItemNum = ItemNum;

  AnotherFound = FALSE;
  FirstFound = FALSE;

  do
  {
    ZwItem = ZwItem->NextItem;
    ZwItemNum++;

    if (ZwItem == NULL)
    {
      ZwItem = BoxItems;
      ZwItemNum = 0;
    }

    if (ZwItem && (ZwItem->Flags & ITEMTEXT) != NULL)
    {
      LookChar = ((struct IntuiText *) ZwItem->ItemFill)->IText[0];
      Found = toupper (LookChar) == Search;
    }
    else
      Found = FALSE;

    if (Found)
    {
      if (!FirstFound)
      {
        FirstFound = TRUE;
        ChangeAktItem (ZwItem, ZwItemNum);
      }
      else
        AnotherFound = TRUE;
    }
  }
  while (ZwItem != OldAktItem && (!AnotherFound));

  *Single = (!AnotherFound);
  return (FirstFound);
}

UWORD
SelectNextItem (WORD NeuItemNum)
{
  struct MenuItem *ZwItem;
  WORD z1;

  ZwItem = BoxItems;
   

    z1 = 0;
  while (ZwItem && (((ZwItem->Flags & HIGHNONE) == HIGHNONE) || z1 <= NeuItemNum))
  {
    ZwItem = ZwItem->NextItem;
    z1++;
  }
  if (ZwItem && (z1 != ItemNum || AktItem == NULL))
  {
    ChangeAktItem (ZwItem, z1);
    return ((UWORD) z1);
  }
  return (ItemNum);
}

UWORD
SelectPrevItem (WORD NeuItemNum)
{
  struct MenuItem *ZwItem, *PrevItem;
  WORD z1, PrevNum;

  ZwItem = BoxItems;
  PrevItem = AktItem;
  PrevNum = ItemNum;

  z1 = 0;
  while (ZwItem && (((ZwItem->Flags & HIGHNONE) == HIGHNONE) || z1 < NeuItemNum))
  {
    if ((ZwItem->Flags & HIGHNONE) != HIGHNONE)
    {
      PrevItem = ZwItem;
      PrevNum = z1;
    }
    ZwItem = ZwItem->NextItem;
    z1++;
  }

  if (PrevNum != ItemNum || AktItem == NULL)
  {
    ChangeAktItem (PrevItem, PrevNum);
    return ((UWORD) PrevNum);
  }
  return (ItemNum);
}

void
ChangeAktMenu (struct Menu *NewMenu, UWORD NewMenuNum)
{
  if (AktMenu)
  {
    CleanUpMenuSubBox ();
    CleanUpMenuBox ();
    DrawNormMenu (AktMenu);
    AktMenu = NULL;
    MenuNum = NOMENU;
  }

  if (NewMenu)
  {
    MenuNum = NewMenuNum;
    if (DrawHiMenu (AktMenu = NewMenu))
    {
      if (MenuMode != MODE_KEYBOARD)
        if (!DrawMenuBox (AktMenu, TRUE))
          DisplayBeep (MenScr);
    }
    else
    {
      MenuNum = NOMENU;
      AktMenu = NULL;
    }
  }
}

BOOL
FindMenuChar (char Search, BOOL * Single)
{
  struct Menu *ZwMenu, *OldAktMenu;
  UWORD ZwMenuNum;
  BOOL FirstFound, AnotherFound, Found;

  Search = toupper (Search);

  if (!AktMenu)
    return (FALSE);

  OldAktMenu = AktMenu;

  ZwMenu = AktMenu;
  ZwMenuNum = MenuNum;

  AnotherFound = FALSE;
  FirstFound = FALSE;

  do
  {
    ZwMenu = ZwMenu->NextMenu;
    ZwMenuNum++;

    if (ZwMenu == NULL)
    {
      ZwMenu = MenStrip;
      ZwMenuNum = 0;
    }

    Found = (ZwMenu && toupper (ZwMenu->MenuName[0]) == Search);
    if (Found)
    {
      if (!FirstFound)
      {
        FirstFound = TRUE;
        ChangeAktMenu (ZwMenu, ZwMenuNum);
      }
      else
        AnotherFound = TRUE;
    }
  }
  while (ZwMenu != OldAktMenu && (!AnotherFound));

  *Single = (!AnotherFound);
  return (FirstFound);
}

UWORD
SelectNextMenu (WORD NeuMenuNum)
{
  struct Menu *ZwMen;
  WORD z1;

  ZwMen = MenStrip;

  z1 = 0;
  while (ZwMen && z1 <= NeuMenuNum)
  {
    ZwMen = ZwMen->NextMenu;
    z1++;
  }
  if (ZwMen && z1 != MenuNum)
  {
    ChangeAktMenu (ZwMen, z1);
    return ((UWORD) z1);
  }
  return (MenuNum);
}

UWORD
SelectPrevMenu (WORD NeuMenuNum)
{
  struct Menu *ZwMen, *PrevMen;
  WORD z1, PrevNum;

  ZwMen = PrevMen = MenStrip;

  z1 = 0;
  while (ZwMen && z1 < NeuMenuNum)
  {
    PrevMen = ZwMen;
    PrevNum = z1;
    ZwMen = ZwMen->NextMenu;
    z1++;
  }

  if (PrevNum != MenuNum)
  {
    ChangeAktMenu (PrevMen, PrevNum);
    return ((UWORD) PrevNum);
  }
  return (MenuNum);
}

BOOL
LookMouse (UWORD MouseX, UWORD MouseY, BOOL NewSelect)
/* True, wenn ausserhalb des Menüs */
{
  UWORD x1, y1, x2, y2;
  UWORD z1;
  struct Menu *ZwMenu;
  struct MenuItem *ZwItem;
  BOOL Weiter;

  if (MenuSubBoxSwapped)
  {
    if (AktSubItem)
    {
      GetSubItemCoors (AktSubItem, &x1, &y1, &x2, &y2);
      if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
        return (FALSE);
    }

    if (!NewSelect)
    {
      if (AktSubItem)
      {
        DrawNormSubItem (AktSubItem);
        AktSubItem->Flags &= ~HIGHITEM;
        AktSubItem = NULL;
      }
      SubItemNum = NOSUB;
      if (MouseX >= SubBoxLeft && MouseX < SubBoxLeft + SubBoxWidth &&
          MouseY >= SubBoxTop && MouseY < SubBoxTop + SubBoxHeight)
      {
        return (FALSE);
      }
      else
      {
        if (MouseX >= BoxLeft && MouseX < BoxLeft + BoxWidth &&
            MouseY >= BoxTop && MouseY < BoxTop + BoxHeight)
        {
          return (FALSE);
        }
        else
        {
          if (MouseX >= StripLeft && MouseX < StripLeft + StripWidth &&
              MouseY >= StripTop && MouseY < StripTop + StripHeight)
            return (FALSE);
        }
      }
      return (TRUE);
    }

    ZwItem = SubBoxItems;
    Weiter = TRUE;
    z1 = 0;
    while (ZwItem && Weiter)
    {
      GetSubItemCoors (ZwItem, &x1, &y1, &x2, &y2);
      if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
      {
        ChangeAktSubItem (ZwItem, z1);
        Weiter = FALSE;
      }
      ZwItem = ZwItem->NextItem;
      z1++;
    }
    if (!Weiter)
      return (FALSE);
    ChangeAktSubItem (NULL, NOSUB);
    if (MouseX >= SubBoxLeft && MouseX < SubBoxLeft + SubBoxWidth &&
        MouseY >= SubBoxTop && MouseY < SubBoxTop + SubBoxHeight)
      return (FALSE);
  }

  if (MenuBoxSwapped)
  {
    if (AktItem)
    {
      GetItemCoors (AktItem, &x1, &y1, &x2, &y2);
      if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
        return (FALSE);
    }

    if (!NewSelect)
    {
      CleanUpMenuSubBox ();
      if (AktItem)
      {
        DrawNormItem (AktItem);
        AktItem->Flags &= ~HIGHITEM;
        AktItem = NULL;
      }
      ItemNum = NOITEM;
      if (MouseX >= BoxLeft && MouseX < BoxLeft + BoxWidth &&
          MouseY >= BoxTop && MouseY < BoxTop + BoxHeight)
      {
        return (FALSE);
      }
      else
      {
        if (MouseX >= StripLeft && MouseX < StripLeft + StripWidth &&
            MouseY >= StripTop && MouseY < StripTop + StripHeight)
          return (FALSE);
      }
      return (TRUE);
    }

    ZwItem = BoxItems;
    Weiter = TRUE;
    z1 = 0;
    while (ZwItem && Weiter)
    {
      GetItemCoors (ZwItem, &x1, &y1, &x2, &y2);
      if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
      {
        ChangeAktItem (ZwItem, z1);
        Weiter = FALSE;
      }
      ZwItem = ZwItem->NextItem;
      z1++;
    }
    if (!Weiter)
      return (FALSE);
    if (AktItem && (!AktItem->SubItem))
      ChangeAktItem (NULL, NOITEM);
    if (MouseX >= BoxLeft && MouseX < BoxLeft + BoxWidth &&
        MouseY >= BoxTop && MouseY < BoxTop + BoxHeight)
      return (FALSE);
  }

  if (AktMenu)
  {
    GetMenuCoors (AktMenu, &x1, &y1, &x2, &y2);
    if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
      return (FALSE);
  }

  if (!NewSelect)
  {
    CleanUpMenuSubBox ();
    CleanUpMenuBox ();
    if (AktMenu)
    {
      DrawNormMenu (AktMenu);
      AktMenu = NULL;
    }
    MenuNum = NOMENU;
    if (MouseX >= StripLeft && MouseX < StripLeft + StripWidth &&
        MouseY >= StripTop && MouseY < StripTop + StripHeight)
      return (FALSE);
    return (TRUE);
  }

  ZwMenu = MenStrip;
  Weiter = TRUE;
  z1 = 0;
  while (ZwMenu && Weiter)
  {
    GetMenuCoors (ZwMenu, &x1, &y1, &x2, &y2);
    if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
    {
      ChangeAktMenu (ZwMenu, z1);
      Weiter = FALSE;
    }
    ZwMenu = ZwMenu->NextMenu;
    z1++;
    if (!Weiter)
      return (FALSE);
  }
  if (MouseX >= StripLeft && MouseX < StripLeft + StripWidth &&
      MouseY >= StripTop && MouseY < StripTop + StripHeight)
    return (FALSE);
  return (TRUE);
}

void
CopyImageDimensions (struct Image *Dest, struct Image *Source)
{
  Dest->LeftEdge = Source->LeftEdge;
  Dest->TopEdge = Source->TopEdge;
  Dest->Height = Source->Height;
  Dest->Width = Source->Width;
}

void
DrawMenuStripContents(struct RastPort *RPort, UWORD Left, UWORD Top)
{
    struct Menu                 *ZwMenu;
    UWORD                       ZwY, X, Y;

    SetAPen (RPort, MenBackGround);
    SetDrMd (RPort, JAM1);
    RectFill (RPort, Left, Top, Left + StripWidth - 1, Top + StripHeight - 1);

    SetFont (RPort, MenFont);

    if (StripPopUp)
        Draw3DRect (RPort, Left, Top, StripWidth, StripHeight, TRUE, ScrHiRes, DblBorder);
    else
    {
        Move (RPort, Top, Top + StripHeight - 1);
        if(LookMC)
            SetAPen (RPort, MenDarkEdge);
        else
            SetAPen (RPort, MenTrimPen);
        SetDrMd (RPort, JAM1);
        Draw (RPort, Left + StripWidth - 1, Top + StripHeight - 1);
    }

    SetDrMd (RPort, JAM1);
    ZwMenu = MenStrip;
    ZwY = StripTopOffs;
    while (ZwMenu)
    {
        if (!StripPopUp)
        {
            X = ZwMenu->LeftEdge + StripLeftOffs + 4;
            Y = Top + ZwMenu->TopEdge + StripTopOffs + 1 + MenFont->tf_Baseline;
        }
        else
        {
            X = Left + StripLeftOffs + 2;
            Y = Top + ZwY + 1 + MenFont->tf_Baseline;
            ZwY += MenFont->tf_YSize + STRIP_YDIST;
        }

        if(LookMC && ! ZwMenu->Flags & MENUENABLED)
        {
            Move(RPort,X+1,Y+1);
            SetAPen(RPort,MenGhostHiCol);
            Text (RPort, ZwMenu->MenuName, strlen (ZwMenu->MenuName));
            SetAPen(RPort,MenGhostLoCol);
        }
        else
            SetAPen(RPort,MenTextCol);

        Move(RPort,X,Y);
        Text (RPort, ZwMenu->MenuName, strlen (ZwMenu->MenuName));

        if (! LookMC && !ZwMenu->Flags & MENUENABLED)
            GhostMenu (ZwMenu,RPort,Left,Top);

        ZwMenu = ZwMenu->NextMenu;
    }
}

BOOL
DrawMenuStrip (BOOL PopUp, UBYTE NewLook, BOOL ActivateMenu)
{
    struct DrawInfo             *DrawInfo;
    struct MenuRemember         *LookRemember;
    struct Image                *ZwCheckImage, *ZwCommandImage;
    struct ColorMap             *ColorMap;
    BOOL                        Ok, UseLow;
    BOOL                        DoFlipCheck, DoFlipCommand, DoFlipMX, DoFlipSubArrow;
    UWORD                       z1;
    WORD                        ZwWert;
    ULONG                       *LPtrD, *LPtrD2, *LPtrD3, *LPtrS;
    struct Menu                 *ZwMenu;

    for(LPtrD = (ULONG *)StdRemapArray, z1 = 0; z1 < 256 / 4; z1++)
        *LPtrD++ = 0;

    MenuStripSwapped = FALSE;

    ScrRPort = MenScr->RastPort;
    SetWrMsk (&ScrRPort, 0xff);
    SetDrMd (&ScrRPort, JAM1);
    SetAfPt (&ScrRPort, NULL, 0);
    SetDrPt (&ScrRPort, 0xffff);

    StripDepth = MenScr->BitMap.Depth;
    ScrHiRes = (MenScr->Flags & SCREENHIRES) != NULL;

    DrawInfo = GetScreenDrawInfo (MenScr);

    MenOpenedFont = NULL;

    if (!(MenOpenedFont = OpenFont (MenScr->Font)))
    {
        MenFont = DrawInfo->dri_Font;
    }
    else
        MenFont = MenOpenedFont;

    SetFont (&ScrRPort, MenFont);
    AskFont (&ScrRPort, &MenTextAttr);

    Look3D = FALSE;
    LookMC = FALSE;

    Ok = TRUE;

    CheckImage = MXImage = NoMXImage = CommandImage = NoCheckImage = NULL;

    CheckImageRmp = MXImageRmp = NoMXImageRmp = CommandImageRmp = SubArrowImageRmp = NULL;
    CheckImageGhosted = CommandImageGhosted = SubArrowImageGhosted = NULL;
    CheckImageActive = CommandImageActive = SubArrowImageActive = NULL;

    DoFlipCheck = DoFlipCommand = DoFlipMX = DoFlipSubArrow = FALSE;

    ColorMap = MenScr->ViewPort.ColorMap;

    if (NewLook == LOOK_MC && StripDepth >= 3 && GfxVersion >= 39 && ScrHiRes &&
        ColorMap->Type >= COLORMAP_TYPE_V39 && ColorMap->PalExtra)
    {
        MenLightEdge = MyObtainPen(MenScr,AktPrefs.mmp_LightEdge.R, AktPrefs.mmp_LightEdge.G, AktPrefs.mmp_LightEdge.B);
        MenDarkEdge = MyObtainPen(MenScr,AktPrefs.mmp_DarkEdge.R, AktPrefs.mmp_DarkEdge.G, AktPrefs.mmp_DarkEdge.B);
        MenBackGround = MyObtainPen(MenScr,AktPrefs.mmp_Background.R, AktPrefs.mmp_Background.G, AktPrefs.mmp_Background.B);
        MenTextCol = MyObtainPen(MenScr,AktPrefs.mmp_TextCol.R, AktPrefs.mmp_TextCol.G, AktPrefs.mmp_TextCol.B);
        MenHiCol = MyObtainPen(MenScr,AktPrefs.mmp_HiCol.R, AktPrefs.mmp_HiCol.G, AktPrefs.mmp_HiCol.B);
        MenFillCol = MyObtainPen(MenScr,AktPrefs.mmp_FillCol.R, AktPrefs.mmp_FillCol.G, AktPrefs.mmp_FillCol.B);
        MenGhostLoCol = MyObtainPen(MenScr,AktPrefs.mmp_GhostLoCol.R, AktPrefs.mmp_GhostLoCol.G, AktPrefs.mmp_GhostLoCol.B);
        MenGhostHiCol = MyObtainPen(MenScr,AktPrefs.mmp_GhostHiCol.R, AktPrefs.mmp_GhostHiCol.G, AktPrefs.mmp_GhostHiCol.B);

/*        MenDarkEdge = MyObtainPen(MenScr,0x00000000, 0x00000000, 0x00000000);
        MenBackGround = MyObtainPen(MenScr,0x95000000, 0x95000000, 0x95000000);
        MenTextCol = MyObtainPen(MenScr,0x00000000, 0x00000000, 0x00000000);
        MenHiCol = MyObtainPen(MenScr,0xffffffff, 0xffffffff, 0xffffffff);
        MenFillCol = MyObtainPen(MenScr,0x3b000000, 0x67000000, 0xA3000000);
        MenGhostLoCol = MyObtainPen(MenScr,0x60000000, 0x60000000, 0x60000000);
        MenGhostHiCol = MyObtainPen(MenScr,0xaf000000, 0xaf000000, 0xaf000000);*/

/*        MenLightEdge = 8;
        MenDarkEdge = 9;
        MenBackGround = 10;
        MenTextCol = 11;
        MenHiCol = 12;
        MenFillCol = 13;
        MenGhostLoCol = 14;
        MenGhostHiCol = 15;*/


        MenXENGrey0 = MyObtainPen(MenScr,0x95000000, 0x95000000, 0x95000000);
        MenXENBlack = MyObtainPen(MenScr,0x00000000, 0x00000000, 0x00000000);
        MenXENWhite = MyObtainPen(MenScr,0xffffffff, 0xffffffff, 0xffffffff);
        MenXENBlue  = MyObtainPen(MenScr,0x3b000000, 0x67000000, 0xA3000000);
        MenXENGrey1 = MyObtainPen(MenScr,0x7b000000, 0x7b000000, 0x7b000000);
        MenXENGrey2 = MyObtainPen(MenScr,0xaf000000, 0xaf000000, 0xaf000000);
        MenXENBeige = MyObtainPen(MenScr,0xaa000000, 0x90000000, 0x7c000000);
        MenXENPink  = MyObtainPen(MenScr,0xffffffff, 0xa9000000, 0x97000000);
        MenMenuTextCol = GetCol (DrawInfo, BARDETAILPEN, 0);
        MenMenuBackCol = GetCol (DrawInfo, BARBLOCKPEN, 1);

        Look3D = TRUE;
        LookMC = (MenTextCol != MenBackGround && MenHiCol != MenFillCol &&
                  (MenGhostLoCol != MenBackGround || MenGhostHiCol != MenBackGround));

        if(! LookMC)
        {
            MyReleasePen(MenScr,MenLightEdge);
            MyReleasePen(MenScr,MenDarkEdge);
            MyReleasePen(MenScr,MenBackGround);
            MyReleasePen(MenScr,MenTextCol);
            MyReleasePen(MenScr,MenHiCol);
            MyReleasePen(MenScr,MenFillCol);
            MyReleasePen(MenScr,MenGhostLoCol);
            MyReleasePen(MenScr,MenGhostHiCol);
            MyReleasePen(MenScr,MenXENGrey0);
            MyReleasePen(MenScr,MenXENBlack);
            MyReleasePen(MenScr,MenXENWhite);
            MyReleasePen(MenScr,MenXENBlue);
            MyReleasePen(MenScr,MenXENGrey1);
            MyReleasePen(MenScr,MenXENGrey2);
            MyReleasePen(MenScr,MenXENBeige);
            MyReleasePen(MenScr,MenXENPink);
        }

        DblBorder = (AktPrefs.mmp_DblBorder == 1);
    }

    if ((NewLook == LOOK_3D || NewLook == LOOK_MC) && (! LookMC) && StripDepth >= 2)
    {
        MenLightEdge = GetCol (DrawInfo, SHINEPEN, 2);
        MenDarkEdge = GetCol (DrawInfo, SHADOWPEN, 1);
        MenBackGround = GetCol (DrawInfo, BACKGROUNDPEN, 0);
        MenTextCol = GetCol (DrawInfo, TEXTPEN, 1);
        MenHiCol = GetCol (DrawInfo, HIGHLIGHTTEXTPEN,2);
        MenFillCol = GetCol (DrawInfo, FILLPEN, 3);
        MenBlockPen = GetCol (DrawInfo, BLOCKPEN, 1);
        MenDetailPen = GetCol (DrawInfo, DETAILPEN, 0);
        MenMenuTextCol = GetCol (DrawInfo, BARDETAILPEN, 0);
        MenMenuBackCol = GetCol (DrawInfo, BARBLOCKPEN, 1);

        MenTrimPen = MenBlockPen;
        MenComplMsk = MenFillCol;
        Look3D = (MenTextCol != MenBackGround && MenLightEdge != MenDarkEdge);
        LookMC = FALSE;
        DblBorder = (AktPrefs.mmp_DblBorder == 1);
    }

    if(Look3D)
    {
        if(LookMC)
        {
            StdRemapArray[0] = MenXENGrey0;
            StdRemapArray[1] = MenXENBlack;
            StdRemapArray[2] = MenXENWhite;
            StdRemapArray[3] = MenXENBlue;
            StdRemapArray[4] = MenXENGrey1;
            StdRemapArray[5] = MenXENGrey2;
            StdRemapArray[6] = MenXENBeige;
            StdRemapArray[7] = MenXENPink;
            StdRemapArray[8] = MenBackGround;

            for(LPtrD = (ULONG *)DreiDRemapArray,
                LPtrD2 = (ULONG *)DreiDGhostedRemapArray,
                LPtrD3 = (ULONG *)DreiDActiveRemapArray,
                LPtrS = (ULONG *)StdRemapArray, z1 = 0; z1 < 256 / 4; z1++)
              *LPtrD3++ = *LPtrD2++ = *LPtrD++ = *LPtrS++;

            DreiDRemapArray[MenMenuTextCol] = MenTextCol;
            DreiDRemapArray[MenMenuBackCol] = MenBackGround;

            DreiDActiveRemapArray[MenMenuTextCol] = MenHiCol;
            DreiDActiveRemapArray[MenMenuBackCol] = MenFillCol;

            DreiDGhostedRemapArray[MenMenuTextCol] = MenGhostLoCol;
            DreiDGhostedRemapArray[MenMenuBackCol] = MenBackGround;
        }
        else
        {
            StdRemapArray[0] = MenBackGround;
            StdRemapArray[1] = MenDarkEdge;
            StdRemapArray[2] = MenLightEdge;
            StdRemapArray[3] = MenFillCol;

            for(LPtrD = (ULONG *)DreiDRemapArray,LPtrS = (ULONG *)StdRemapArray, z1 = 0; z1 < 256 / 4; z1++)
              *LPtrD++ = *LPtrS++;

            DreiDRemapArray[MenMenuTextCol] = MenTextCol;
            DreiDRemapArray[MenMenuBackCol] = MenBackGround;
        }


        ObtainGlyphs(MenWin,&CheckImage,&CommandImage);

        if(CheckImage || CommandImage)
        {
            if(CheckImage)
            {
                if(Ok)
                    Ok = MakeRemappedImage (&CheckImageRmp, CheckImage, 8, StripDepth, DreiDRemapArray);
                NoMXImage = &NoCheckImage2Pl;
                CopyImageDimensions (NoMXImage, CheckImage);

                if(LookMC)
                {
                    if(Ok)
                        MakeRemappedImage (&CheckImageGhosted, CheckImage, 8, StripDepth, DreiDGhostedRemapArray);
                    if(Ok)
                        MakeRemappedImage (&CheckImageActive, CheckImage, 8, StripDepth, DreiDActiveRemapArray);
                }

                CheckImage = CheckImageRmp;
                MXImage = CheckImage;

                DoFlipMX = FALSE;
                DoFlipCheck = FALSE;
            }

            if(CommandImage)
            {
                DoFlipCommand = FALSE;
                if(Ok)
                    Ok = MakeRemappedImage (&CommandImageRmp, CommandImage, 8, StripDepth, DreiDRemapArray);
                if(LookMC)
                {
                    if(Ok)
                        Ok = MakeRemappedImage (&CommandImageGhosted, CommandImage, 8, StripDepth, DreiDGhostedRemapArray);
                    if(Ok)
                        Ok = MakeRemappedImage (&CommandImageActive, CommandImage, 8, StripDepth, DreiDActiveRemapArray);
                }

                CommandImage = CommandImageRmp;
            }
        }

        if(LookMC)
        {
            Ok = TRUE;

            if (! CommandImage)
            {
                StdRemapArray[8] = MenBackGround;
                if(Ok)
                    Ok = MakeRemappedImage (&CommandImageRmp, &AmigaImage4Pl, 8, StripDepth, StdRemapArray);
                if(Ok)
                    Ok = MakeRemappedImage (&CommandImageGhosted, &AmigaImage4PlGhosted, 8, StripDepth, StdRemapArray);

                StdRemapArray[8] = MenFillCol;
                if(Ok)
                    Ok = MakeRemappedImage (&CommandImageActive, &AmigaImage4Pl, 8, StripDepth, StdRemapArray);

                CommandImage = CommandImageRmp;
            }


            if (! CheckImage)
            {
                StdRemapArray[8] = MenBackGround;
                if(Ok)
                    Ok = MakeRemappedImage (&CheckImageRmp, &CheckImage4Pl, 8, StripDepth, StdRemapArray);
                if(Ok)
                    Ok = MakeRemappedImage (&CheckImageGhosted, &CheckImage4PlGhosted, 8, StripDepth, StdRemapArray);

                StdRemapArray[8] = MenFillCol;
                if(Ok)
                    Ok = MakeRemappedImage (&CheckImageActive, &CheckImage4Pl, 8, StripDepth, StdRemapArray);

                CheckImage = CheckImageRmp;
            }

            NoCheckImage = &NoCheckImage2Pl;

            NoCheckImage->PlaneOnOff = MenBackGround;
            CopyImageDimensions (NoCheckImage, CheckImage);

            StdRemapArray[8] = MenBackGround;
            if(Ok)
                Ok = MakeRemappedImage (&SubArrowImageRmp, &SubArrowImage4Pl, 8, StripDepth, StdRemapArray);
            if(Ok)
                Ok = MakeRemappedImage (&SubArrowImageGhosted, &SubArrowImage4PlGhosted, 8, StripDepth, StdRemapArray);

            StdRemapArray[8] = MenFillCol;
            if(Ok)
                Ok = MakeRemappedImage (&SubArrowImageActive, &SubArrowImage4Pl, 8, StripDepth, StdRemapArray);

            SubArrowImage = SubArrowImageRmp;
        }
        else
        {
            if(ScrHiRes)
            {
                if (GfxVersion >= 39 && (MenWin->Flags & WFLG_NEWLOOKMENUS) != 0 && DrawInfo->dri_CheckMark)
                    UseLow = (DrawInfo->dri_CheckMark->Width <= 16);
                else
                    UseLow = (MenWin->CheckMark->Width <= 14);

                if(! CheckImage)
                {
                    CheckImage = (UseLow) ? &CheckImageLow2Pl : &CheckImage2Pl;
                    DoFlipCheck = TRUE;
                }

                if(! MXImage)
                {
                    MXImage = (UseLow) ? &MXImageLow2Pl : &MXImage2Pl;
                    NoMXImage = (UseLow) ? &NoMXImageLow2Pl : &NoMXImage2Pl;
                    DoFlipMX = TRUE;
                }

                if(! CommandImage)
                {
                    CommandImage = &AmigaImage2Pl;
                    DoFlipCommand = TRUE;
                }

                SubArrowImage = &SubArrowImage2Pl;
            }
            else
            {
                if(! CheckImage)
                {
                    CheckImage = &CheckImageLow2Pl;
                    DoFlipCheck = TRUE;
                }

                if (! MXImage)
                {
                    MXImage = &MXImageLow2Pl;
                    NoMXImage = &NoMXImageLow2Pl;
                    DoFlipMX = TRUE;
                }

                if(! CommandImage)
                {
                    CommandImage = &AmigaImageLow2Pl;
                    DoFlipCommand = TRUE;
                }

                SubArrowImage = &SubArrowImageLow2Pl;
            }

            NoCheckImage = &NoCheckImage2Pl;
            CopyImageDimensions (NoCheckImage, CheckImage);
        }
    }
    else
    {
        if (DrawInfo->dri_Version >= 2 && DrawInfo->dri_NumPens > 9 && GfxVersion >= 39)
        {
            MenLightEdge = GetCol (DrawInfo, BARDETAILPEN, 2);
            MenDarkEdge = GetCol (DrawInfo, BARDETAILPEN, 2);
            MenBackGround = GetCol (DrawInfo, BARBLOCKPEN, 1);
            MenTextCol = GetCol (DrawInfo, BARDETAILPEN, 2);
            MenFillCol = GetCol (DrawInfo, BARBLOCKPEN, 1);
            MenBlockPen = GetCol (DrawInfo, BARBLOCKPEN, 1);
            MenDetailPen = GetCol (DrawInfo, BARDETAILPEN, 2);
            MenTrimPen = GetCol (DrawInfo, BARTRIMPEN, 1);
            MenComplMsk = (MenTextCol | MenBackGround);
            NoCheckImage = NoMXImage = NULL;
            CheckImage = DrawInfo->dri_CheckMark;
            if(! CheckImage)
                CheckImage = MenWin->CheckMark;
            MXImage = CheckImage;

            CommandImage = DrawInfo->dri_AmigaKey;

            DoFlipCommand = FALSE;
            DoFlipCheck = DoFlipMX = FALSE;

            StdRemapArray[0] = MenBlockPen;
            StdRemapArray[1] = MenDetailPen;
        }
        if (GfxVersion < 39 || (MenWin->Flags & WFLG_NEWLOOKMENUS) == 0)
        {
            MenLightEdge = MenWin->DetailPen;
            MenDarkEdge = MenWin->DetailPen;
            MenBackGround = MenWin->BlockPen;
            MenTextCol = MenWin->DetailPen;
            MenFillCol = MenWin->BlockPen;
            MenBlockPen = MenWin->BlockPen;
            MenDetailPen = MenWin->DetailPen;
            MenTrimPen = MenWin->BlockPen;
            MenComplMsk = 0xff;

            CheckImage = MenWin->CheckMark;
            NoCheckImage = NoMXImage = NULL;
            MXImage = CheckImage;

            DoFlipCheck = DoFlipMX = FALSE;

            StdRemapArray[0] = MenBlockPen;
            StdRemapArray[1] = MenDetailPen;
        }
        DblBorder = FALSE;


        ObtainGlyphs(MenWin,&ZwCheckImage,&ZwCommandImage);
        if(ZwCheckImage)
        {
            CheckImage = ZwCheckImage;
            DoFlipCheck = FALSE;
        }
        if(ZwCommandImage)
        {
            CommandImage = ZwCommandImage;
            DoFlipCommand = FALSE;
        }

        if(ScrHiRes)
        {
            if(! CommandImage)
            {
                CommandImage = &AmigaImage1Pl;
                DoFlipCommand = TRUE;
            }

            SubArrowImage = &SubArrowImage1Pl;
            DoFlipSubArrow = TRUE;
        }
        else
        {
            if(! CommandImage)
            {
                CommandImage = &AmigaImageLow1Pl;
                DoFlipCommand = TRUE;
            }

            SubArrowImage = &SubArrowImageLow1Pl;
            DoFlipSubArrow = TRUE;
        }

        NoCheckImage = &NoCheckImage2Pl;
        CopyImageDimensions (NoCheckImage, CheckImage);
        NoMXImage = &NoCheckImage2Pl;
        CopyImageDimensions (NoMXImage, CheckImage);
    }


    FreeScreenDrawInfo (MenScr, DrawInfo);

    if(! LookMC)
    {
        NoCheckImage->PlaneOnOff = NoMXImage->PlaneOnOff = MenBackGround;

        if (Ok && DoFlipMX)
        {
            Ok = MakeRemappedImage (&MXImageRmp, MXImage, 8, StripDepth, StdRemapArray);
            if (Ok && NoMXImage)
            {
                Ok = MakeRemappedImage (&NoMXImageRmp, NoMXImage, 8, StripDepth, StdRemapArray);
                if(! Ok)
                    FreeRemappedImage(MXImage);
            }

            MXImage = MXImageRmp;
            NoMXImage = NoMXImageRmp;
        }

        if (Ok && DoFlipCommand)
        {
            Ok = MakeRemappedImage (&CommandImageRmp, CommandImage, 8, StripDepth, StdRemapArray);
            CommandImage = CommandImageRmp;
        }

        if (Ok && DoFlipCheck)
        {
            Ok = MakeRemappedImage (&CheckImageRmp, CheckImage, 8, StripDepth, StdRemapArray);
            CheckImage = CheckImageRmp;
        }

        if (Ok && DoFlipSubArrow)
        {
            Ok = MakeRemappedImage (&SubArrowImageRmp, SubArrowImage, 8, StripDepth, StdRemapArray);
            SubArrowImage = SubArrowImageRmp;
        }
    }

    if (!Ok)
    {
        CleanUpMenuStrip ();
        DoGlobalQuit = TRUE;
        DisplayBeep (NULL);
        return (FALSE);
    }

    ObtainSemaphore (RememberSemaphore);

    LookRemember = GlobalRemember;
    AktMenuRemember = NULL;
    while (LookRemember && !AktMenuRemember)
    {
      if (LookRemember->MenWindow == MenWin && LookRemember->MenuStrip == MenStrip)
        AktMenuRemember = LookRemember;
      LookRemember = LookRemember->NextRemember;
    }

    ReleaseSemaphore (RememberSemaphore);

    if (AktMenuRemember == NULL)
    {
      if (!UpdateRemember (MenWin))
      {
        CleanUpMenuStrip ();
        DoGlobalQuit = TRUE;
        DisplayBeep (NULL);
        return (FALSE);
      }
    }

    if (!PopUp)
    {
      StripPopUp = FALSE;
      StripHeight = MenFont->tf_YSize + 3;
      StripWidth = MenScr->Width;
      StripTopOffs = 0;
      StripLeftOffs = 0;
      StripMinHeight = MenFont->tf_YSize + 2;
      StripMinWidth = 0;
      StripLeft = 0;
      StripTop = 0;
      StripDrawOffs = 0;
    }
    else
    {
      StripPopUp = TRUE;
      StripHeight = 0;
      StripWidth = 0;

      ZwMenu = MenStrip;

      while (ZwMenu)
      {
        if (ZwMenu->Width > StripWidth)
          StripWidth = ZwMenu->Width;
        StripHeight += MenFont->tf_YSize + STRIP_YDIST;
        ZwMenu = ZwMenu->NextMenu;
      }

      if (ScrHiRes)
      {
        StripWidth += 10;
        StripHeight += 6;
        StripLeftOffs = 5;
        StripTopOffs = 3;
        StripMinWidth = StripWidth - 10;
      }
      else
      {
        StripWidth += 6;
        StripHeight += 6;
        StripLeftOffs = 3;
        StripTopOffs = 3;
        StripMinWidth = StripWidth - 6;
      }

      StripMinHeight = MenFont->tf_YSize + 2;

      if (AktMenuRemember->AktMenu)
        ZwWert = (LONG) (MenScr->MouseY) - (AktMenuRemember->AktMenuNum * (MenFont->tf_YSize + STRIP_YDIST) + StripTopOffs + ((MenFont->tf_YSize + STRIP_YDIST) / 2));
      else
        ZwWert = MenScr->MouseY - StripHeight / 2;
      StripTop = (ZwWert > 0) ? ZwWert : 0;
      ZwWert = (LONG) MenScr->MouseX - StripWidth / 2;
      StripLeft = (ZwWert > 0) ? ZwWert : 0;

      if (StripTop + StripHeight >= MenScr->Height)
        StripTop = MenScr->Height - StripHeight - 1;
      if (StripLeft + StripWidth >= MenScr->Width)
        StripLeft = MenScr->Width - StripWidth - 1;

      if(! AktPrefs.mmp_NonBlocking)
          StripDrawOffs = StripLeft % 16;
      else
          StripDrawOffs = 0;
    }

    if(! (AktPrefs.mmp_DirectDraw && AktPrefs.mmp_NonBlocking))
    {
        if (!InstallRPort (StripDepth, StripWidth + StripDrawOffs + 16, StripHeight, &StripRPort, &StripBitMap, &StripLayerInfo, &StripLayer,
                           MenScr->RastPort.BitMap))
        {
            CleanUpMenuStrip ();
            return (FALSE);
        }
    }

    if (AktPrefs.mmp_DirectDraw)
    {
        if(AktPrefs.mmp_NonBlocking)
        {
            if(! (StripWin = OpenWindowTags(NULL,
                              WA_Left,            StripLeft,
                              WA_Top,             StripTop,
                              WA_Width,           StripWidth,
                              WA_Height,          StripHeight,
                              WA_CustomScreen,    MenScr,
                              WA_Borderless,      TRUE,
                              WA_Activate,        FALSE,
                              WA_RMBTrap,         TRUE,
                              WA_SmartRefresh,    TRUE,
                              WA_ScreenTitle,     MenWin->ScreenTitle,
                              TAG_DONE)))
            {
                CleanUpMenuStrip();
                return(FALSE);
            }

            StripDrawRPort = StripWin->RPort;
            StripDrawLeft = 0;
            StripDrawTop = 0;
        }
        else
        {
            BltBitMap (ScrRPort.BitMap, StripLeft, StripTop, StripBitMap, StripDrawOffs, 0, StripWidth, StripHeight, 0xc0, 0xffff, NULL);
            WaitBlit ();

            StripDrawRPort = &ScrRPort;
            StripDrawLeft = StripLeft;
            StripDrawTop = StripTop;
        }

        DrawMenuStripContents(StripDrawRPort,StripDrawLeft,StripDrawTop);
    }
    else
    {
        DrawMenuStripContents(StripRPort,StripDrawOffs,0);

        if(AktPrefs.mmp_NonBlocking)
        {
            if(! (StripWin = OpenWindowTags(NULL,
                              WA_Left,            StripLeft,
                              WA_Top,             StripTop,
                              WA_Width,           StripWidth,
                              WA_Height,          StripHeight,
                              WA_CustomScreen,    MenScr,
                              WA_SuperBitMap,     StripBitMap,
                              WA_Borderless,      TRUE,
                              WA_Activate,        FALSE,
                              WA_RMBTrap,         TRUE,
                              WA_ScreenTitle,     MenWin->ScreenTitle,
                              TAG_DONE)))
            {
                CleanUpMenuStrip();
                return(FALSE);
            }

            StripDrawRPort = StripWin->RPort;
            StripDrawLeft = 0;
            StripDrawTop = 0;
        }
        else
        {
            if (!(StripCRect = GetClipRect (StripBitMap, StripLeft, StripTop, StripLeft + StripWidth - 1, StripTop + StripHeight - 1)))
            {
              CleanUpMenuStrip ();
              return (FALSE);
            }

            SwapBitsRastPortClipRect (&ScrRPort, StripCRect);
            WaitBlit ();

            StripDrawRPort = &ScrRPort;
            StripDrawLeft = StripLeft;
            StripDrawTop = StripTop;
        }
    }

    SetFont(StripDrawRPort,MenFont);
    SetABPenDrMd(StripDrawRPort,MenTextCol,MenBackGround,JAM1);

    AktMenu = NULL;
    MenuStripSwapped = TRUE;


    if (ActivateMenu)
    {
      if (MenuMode == MODE_KEYBOARD)
        if (AktMenuRemember->AktMenu != NULL)
          ChangeAktMenu (AktMenuRemember->AktMenu, AktMenuRemember->AktMenuNum);
        else
          SelectNextMenu (-1);

      if (MenuMode != MODE_KEYBOARD && (MenuMode != MODE_SELECT || SelectSpecial))
        LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
    }

    return (TRUE);
}
