/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

/******************************************************************************/

#define SHADOW_SIZE 6

STATIC VOID
AddShadow (struct RastPort *RPort, LONG Width, LONG Height)
{
  STATIC UWORD Crosshatch[2] =
  {0x5555, 0xAAAA};

  SetAfPt (RPort, Crosshatch, 1);

  SetFgPen (RPort, MenXENBlack);
  SetDrawMode (RPort, JAM1);

  RectFill (RPort, Width, SHADOW_SIZE, Width + SHADOW_SIZE - 1, Height - 1);
  RectFill (RPort, SHADOW_SIZE, Height, Width + SHADOW_SIZE - 1, Height + SHADOW_SIZE - 1);

  SetAfPt (RPort, NULL, 0);
}

STATIC VOID
ClampShadow (struct Screen *screen, LONG left, LONG top, LONG width, LONG height, LONG * widthPtr, LONG * heightPtr)
{
  width += SHADOW_SIZE;
  height += SHADOW_SIZE;

  if (left + width > screen->Width)
    width = screen->Width - left;

  if (top + height > screen->Height)
    height = screen->Height - top;

  *widthPtr = width;
  *heightPtr = height;
}

/******************************************************************************/

#define NUM_MENU_PENS 21

STATIC ULONG MenuColours[NUM_MENU_PENS * 3];
STATIC ULONG *MenuColour;
STATIC LONG MenuPens[NUM_MENU_PENS];

STATIC VOID
ResetMenuColours (VOID)
{
  LONG i;

  MenuColour = MenuColours;

  for (i = 0; i < NUM_MENU_PENS; i++)
    MenuPens[i] = -1;
}

STATIC VOID
AddMenuColour (ULONG red, ULONG green, ULONG blue)
{
  *MenuColour++ = red;
  *MenuColour++ = green;
  *MenuColour++ = blue;
}

STATIC VOID
AddMenuColour8 (LONG red, LONG green, LONG blue)
{
  if (red > 255)
    red = 255;
  else if (red < 0)
    red = 0;

  if (blue > 255)
    green = 255;
  else if (green < 0)
    green = 0;

  if (blue > 255)
    blue = 255;
  else if (blue < 0)
    blue = 0;

  *MenuColour++ = 0x01010101UL * red;
  *MenuColour++ = 0x01010101UL * green;
  *MenuColour++ = 0x01010101UL * blue;
}

STATIC VOID
FreeMenuColours (struct ColorMap *cmap)
{
  if (V39)
  {
    LONG i;

    for (i = 0; i < NUM_MENU_PENS; i++)
    {
      ReleasePen (cmap, MenuPens[i]);
      MenuPens[i] = -1;
    }
  }
}

STATIC BOOL
AllocateMenuColours (struct ColorMap *cmap)
{
  STATIC Tag Tags[] =
  {
    OBP_Precision, PRECISION_GUI,
    OBP_FailIfBad, TRUE,

    TAG_DONE
  };

  ULONG *colour;
  LONG i;

  colour = MenuColours;

  for (i = 0; i < NUM_MENU_PENS; i++)
  {
    if ((MenuPens[i] = ObtainBestPenA (cmap, *colour++, *colour++, *colour++, (struct TagItem *) Tags)) == -1)
    {
      FreeMenuColours (cmap);
      return (FALSE);
    }
  }

  return (TRUE);
}

/******************************************************************************/

VOID
DrawMenuItem (struct RastPort *rp, struct MenuItem *Item, LONG x, LONG y, UWORD CmdOffs, BOOL GhostIt, BOOL Highlighted)
{
  struct IntuiText *Text;
  struct IntuiText MyText;
  struct Image MyImage;
  struct Image *Image;
  UWORD StartX, StartY;
  UWORD ImageYOffs;
  UWORD ZwWidth, NW;
  LONG z1;
  BOOL DrawCheck, YOffsOk, Ghosted;

  StartX = x + Item->LeftEdge;
  StartY = y + Item->TopEdge;

  ImageYOffs = ((UWORD) Item->Height) / 2;

  CommandText.FrontPen = MenTextCol;
  CommandText.BackPen = MenBackGround;
  CommandText.DrawMode = JAM2;
  CommandText.ITextFont = &MenTextAttr;
  CommandText.TopEdge = 0;

  Ghosted = (GhostIt || !(Item->Flags & ITEMENABLED));

  YOffsOk = FALSE;

  DrawCheck = ((Item->Flags & CHECKIT) && (!Item->SubItem) && (Item->Flags & HIGHNONE) != HIGHNONE);
  if (Item->Flags & ITEMTEXT)
  {
    if (Highlighted && (Item->Flags & HIGHFLAGS) == HIGHIMAGE)
      Text = (struct IntuiText *) Item->SelectFill;
    else
      Text = (struct IntuiText *) Item->ItemFill;

    ZwWidth = 0;

    while (Text)
    {
      MyText = *Text;

      MyText.NextText = NULL;

      if (!LookMC || strcmp (MyText.IText, "»") != 0)
      {
        if (LookMC && Ghosted)
        {
          MyText.DrawMode = JAM1;
          MyText.FrontPen = MenStdGrey2;

          PrintIText (rp, &MyText, StartX + 1, StartY + 1);

          MyText.DrawMode = JAM1;
          MyText.FrontPen = MenStdGrey0;

          PrintIText (rp, &MyText, StartX, StartY);
        }
        else
        {
          if (LookMC)
          {
            MyText.FrontPen = (Highlighted) ? MenHiCol : MenTextCol;
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
            else if (MyText.DrawMode == JAM2 && (MyText.BackPen == MenBackGround || (V39 && MyText.BackPen == 2)))
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
    struct Image *ThatImage;

    CommandText.ITextFont = &MenTextAttr;

    if (Highlighted)
      Image = (struct Image *) Item->SelectFill;
    else
      Image = (struct Image *) Item->ItemFill;

    while (Image)
    {
      if(Image->NextImage)
      {
        MyImage = *Image;

        MyImage.NextImage = NULL;

        ThatImage = &MyImage;
      }
      else
        ThatImage = Image;

      if (Look3D)
      {
        if (LookMC && ThatImage->PlanePick == 0 && ThatImage->Height <= 2)
        {
          SetFgPen (rp, MenStdGrey0);
          RectFill (rp, StartX + ThatImage->LeftEdge, StartY + ThatImage->TopEdge,
                    StartX + ThatImage->LeftEdge + ThatImage->Width - 1, StartY + ThatImage->TopEdge);

          SetFgPen (rp, MenStdGrey2);
          RectFill (rp, StartX + ThatImage->LeftEdge, StartY + ThatImage->TopEdge + 1,
                    StartX + ThatImage->LeftEdge + ThatImage->Width - 1, StartY + ThatImage->TopEdge + 1);
        }
        else
         DrawImage (rp, ThatImage, StartX, StartY);
      }
      else
        DrawImage (rp, ThatImage, StartX, StartY);

      Image = Image->NextImage;
    }
    ZwWidth = 0;
  }

  if (DrawCheck)
  {
    struct Image *WhichImage;

    if (LookMC)
    {
      if (Item->MutualExclude == 0 || !MXImageRmp)
      {
        if (Item->Flags & CHECKED)
        {
          if(Ghosted)
            WhichImage = CheckImageGhosted;
          else
          {
            if(Highlighted)
              WhichImage = CheckImageActive;
            else
              WhichImage = CheckImage;
          }
        }
        else
        {
          NoCheckImage->PlaneOnOff = (Highlighted) ? MenFillCol : MenBackGround;

          WhichImage = NoCheckImage;
        }
      }
      else
      {
        if (Item->Flags & CHECKED)
        {
          if (Ghosted)
            WhichImage = MXImageGhosted;
          else
          {
            if (Highlighted)
              WhichImage = MXImageActive;
            else
              WhichImage = MXImageRmp;
          }
        }
        else
        {
          if (Ghosted)
            WhichImage = NoMXImageGhosted;
          else
          {
            if (Highlighted)
              WhichImage = NoMXImageActive;
            else
              WhichImage = NoMXImageRmp;
          }
        }
      }
    }
    else
    {
      if (Item->MutualExclude == 0)
      {
        if (Item->Flags & CHECKED)
          WhichImage = CheckImage;
        else
          WhichImage = NoCheckImage;
      }
      else
      {
        if (Item->Flags & CHECKED)
          WhichImage = MXImage;
        else
          WhichImage = NoMXImage;
      }
    }

    DrawImage (rp, WhichImage, StartX, StartY + (short) ImageYOffs - (WhichImage->Height / 2));
  }

  z1 = StartX + Item->Width;

  if (Item->Flags & COMMSEQ)
  {
    CommText[0] = Item->Command;

    CommText[1] = 0;

    if (LookMC)
    {
      if (Ghosted)
      {
        DrawImage (rp, CommandImageGhosted, z1 - CommandImageGhosted->Width - CmdOffs - 2, StartY + ImageYOffs - (CommandImageGhosted->Height / 2));

        CommandText.FrontPen = MenStdGrey2;
        PrintIText (rp, &CommandText, z1 - CmdOffs + 1, StartY + 1);

        CommandText.FrontPen = MenStdGrey0;
        CommandText.DrawMode = JAM1;
      }
      else
      {
        DrawImage (rp, (Highlighted) ? CommandImageActive : CommandImage, z1 - CommandImageGhosted->Width - CmdOffs - 2, StartY + ImageYOffs - (CommandImageGhosted->Height / 2));

        CommandText.FrontPen = (Highlighted) ? MenHiCol : MenTextCol;
        CommandText.DrawMode = JAM1;
      }
    }
    else
    {
      DrawImage (rp, CommandImage, z1 - CommandImage->Width - CmdOffs - 2, StartY + ImageYOffs - (CommandImage->Height / 2));
    }

    PrintIText (rp, &CommandText, z1 - CmdOffs, StartY);
  }
  else if ((Item->SubItem) && (AktPrefs.mmp_MarkSub == 1))
  {
    if (ZwWidth < Item->Width - SubArrowImage->Width - 5)
    {
      struct Image *WhichImage;

      if (LookMC)
      {
        if(Highlighted)
          WhichImage = SubArrowImageActive;
        else
        {
          if(Ghosted)
            WhichImage = SubArrowImageGhosted;
          else
            WhichImage = SubArrowImage;
        }
      }
      else
        WhichImage = SubArrowImage;

      DrawImage (rp, WhichImage, z1 - WhichImage->Width - 3, StartY + ImageYOffs - (WhichImage->Height / 2));
    }
  }

  if (!LookMC && (GhostIt || !(Item->Flags & ITEMENABLED)))
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

VOID
CleanUpMenuSubBox (VOID)
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
    if (AktPrefs.mmp_NonBlocking)
    {
      if (SubBoxWin)
      {
        CloseWindow (SubBoxWin);
        SubBoxWin = NULL;
      }
    }
    else
    {
      SwapRPortClipRect (&ScrRPort, SubBoxCRect);
      FreeVecPooled (SubBoxCRect);
      SubBoxCRect = NULL;
    }

    MenuSubBoxSwapped = NULL;
  }

  if (SubBoxRPort)
  {
    FreeRPort (SubBoxBitMap, SubBoxLayerInfo, SubBoxLayer, SubBoxWidth, SubBoxHeight);
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
          if (LookMC)
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

        if (!(Item->Flags & ITEMENABLED) || SubBoxGhosted || BoxGhosted)
        {
          if (!LookMC || MenuMode == MODE_KEYBOARD)
          {
            Draw3DRect (SubBoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
            if (!LookMC)
              GhostRect (SubBoxDrawRPort, l, t, w, h);
          }
        }
        else
          Draw3DRect (SubBoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
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
        if (!LookMC && HiFlags != HIGHIMAGE && Item->Flags & ITEMENABLED && (!SubBoxGhosted) && (!BoxGhosted))
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

VOID
DrawMenuSubBoxContents (struct MenuItem *Item, struct RastPort *RPort, UWORD Left, UWORD Top)
{
  struct MenuItem *ZwItem;

  SetPens (RPort, MenBackGround, 0, JAM1);

  RectFill (RPort, Left, Top, Left + SubBoxWidth - 1, Top + SubBoxHeight - 1);

  SetFont (RPort, MenFont);
  Draw3DRect (RPort, Left, Top, SubBoxWidth, SubBoxHeight, TRUE, ScrHiRes, DblBorder);

  SubBoxGhosted = BoxGhosted || !(Item->Flags & ITEMENABLED);

  ZwItem = SubBoxItems;
  while (ZwItem)
  {
    DrawMenuItem (RPort, ZwItem, SubBoxLeftOffs + Left, SubBoxTopOffs + Top, SubBoxCmdOffs, SubBoxGhosted, FALSE);
    ZwItem = ZwItem->NextItem;
  }
}

BOOL
DrawMenuSubBox (struct Menu *Menu, struct MenuItem *Item, BOOL ActivateItem)
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

  SubBoxDepth = GetBitMapDepth (MenScr->RastPort.BitMap);

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

  SubBoxRightEdge = SubBoxWidth - SubBoxLeftBorder * 2;

  if (AktPrefs.mmp_NonBlocking)
  {
    LONG windowWidth, windowHeight;

    ClampShadow (MenScr, SubBoxLeft, SubBoxTop, SubBoxWidth, SubBoxHeight, &windowWidth, &windowHeight);

    if (!(SubBoxWin = OpenWindowTags (NULL,
                                      WA_Left, SubBoxLeft,
                                      WA_Top, SubBoxTop,
                                      WA_Width, windowWidth,
                                      WA_Height, windowHeight,
                                      WA_CustomScreen, MenScr,
                                      WA_Borderless, TRUE,
                                      WA_Activate, FALSE,
                                      WA_RMBTrap, TRUE,
                                      WA_ScreenTitle, MenWin->ScreenTitle,
                                      WA_BackFill, GetNOPFillHook (),
                                      TAG_DONE)))
    {
      CleanUpMenuSubBox ();
      return (FALSE);
    }

    SubBoxDrawRPort = SubBoxWin->RPort;
    SubBoxDrawLeft = 0;
    SubBoxDrawTop = 0;

    if (LookMC)
      AddShadow (SubBoxDrawRPort, SubBoxWidth, SubBoxHeight);

    DrawMenuSubBoxContents (Item, SubBoxDrawRPort, SubBoxDrawLeft, SubBoxDrawTop);
  }
  else
  {
    if (!InstallRPort (SubBoxDepth, SubBoxWidth, SubBoxHeight, &SubBoxRPort, &SubBoxBitMap, &SubBoxLayerInfo, &SubBoxLayer,
                       MenScr->RastPort.BitMap))
    {
      CleanUpMenuSubBox ();
      return (FALSE);
    }

    DrawMenuSubBoxContents (Item, SubBoxRPort, 0, 0);

    if (!(SubBoxCRect = GetClipRect (SubBoxBitMap, SubBoxLeft, SubBoxTop, SubBoxLeft + SubBoxWidth - 1, SubBoxTop + SubBoxHeight - 1)))
    {
      CleanUpMenuSubBox ();
      return (FALSE);
    }

    SwapRPortClipRect (&ScrRPort, SubBoxCRect);
    SubBoxDrawRPort = &ScrRPort;
    SubBoxDrawLeft = SubBoxLeft;
    SubBoxDrawTop = SubBoxTop;
  }

  SetFont (SubBoxDrawRPort, MenFont);
  SetPens (SubBoxDrawRPort, MenTextCol, 0, JAM1);

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

VOID
CleanUpMenuBox (VOID)
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
    if (AktPrefs.mmp_NonBlocking)
    {
      if (BoxWin)
      {
        CloseWindow (BoxWin);
        BoxWin = NULL;
      }
    }
    else
    {
      SwapRPortClipRect (&ScrRPort, BoxCRect);
      FreeVecPooled (BoxCRect);
      BoxCRect = NULL;
    }
    MenuBoxSwapped = NULL;
  }

  if (BoxRPort)
  {
    FreeRPort (BoxBitMap, BoxLayerInfo, BoxLayer, BoxWidth, BoxHeight);
    BoxRPort = NULL;
  }

  if (AktMenu)
    AktMenu->Flags &= ~MIDRAWN;
  AktItem = NULL;
  DisarmMenu();
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
          if (LookMC)
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

        if (!(Item->Flags & ITEMENABLED) || BoxGhosted)
        {
          if (!LookMC || MenuMode == MODE_KEYBOARD)
          {
            Draw3DRect (BoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
            if (!LookMC)
              GhostRect (BoxDrawRPort, l, t, w, h);
          }
        }
        else
          Draw3DRect (BoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
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
        HiRect (BoxDrawRPort, l, t, w, h, FALSE);
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

VOID
DrawMenuBoxContents (struct Menu *Menu, struct RastPort *RPort, UWORD Left, UWORD Top)
{
  struct MenuItem *ZwItem;
  LONG x1, x2;

  SetPens (RPort, MenBackGround, 0, JAM1);

  RectFill (RPort, Left, Top, Left + BoxWidth - 1, Top + BoxHeight - 1);

  SetFont (RPort, MenFont);
  Draw3DRect (RPort, Left, Top, BoxWidth, BoxHeight, TRUE, ScrHiRes, DblBorder);

  SetPens (RPort, MenBackGround, 0, JAM1);

  if (!StripPopUp && Look3D && (!LookMC) && (!DblBorder))
  {
    x1 = Menu->LeftEdge - BoxLeft + StripLeftOffs + ((ScrHiRes) ? 2 : 1);
    if (x1 < 0)
      x1 = 0;
    x2 = x1 + Menu->Width - ((ScrHiRes) ? 4 : 2);
    if (x2 >= BoxWidth)
      x2 = BoxWidth - 1;
    DrawLine (RPort, Left + x1, Top,
              Left + x2, Top);

    if (!BoxGhosted)
    {
      SetFgPen (RPort, MenComplMsk);

      DrawLine (RPort, Left + x1, Top,
                Left + x2, Top);
      SetDrawMode (RPort, JAM1);

      SetDrawMask (RPort, 0xff);

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
DrawMenuBox (struct Menu *Menu, BOOL ActivateItem)
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

  BoxDepth = GetBitMapDepth (MenScr->RastPort.BitMap);

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
    if (LookMC)
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

  BoxRightEdge = BoxWidth - BoxLeftBorder * 2;

  BoxGhosted = !(Menu->Flags & MENUENABLED);

  if (AktPrefs.mmp_NonBlocking)
  {
    LONG windowWidth, windowHeight;

    ClampShadow (MenScr, BoxLeft, BoxTop, BoxWidth, BoxHeight, &windowWidth, &windowHeight);

    if (!(BoxWin = OpenWindowTags (NULL,
                                   WA_Left, BoxLeft,
                                   WA_Top, BoxTop,
                                   WA_Width, windowWidth,
                                   WA_Height, windowHeight,
                                   WA_CustomScreen, MenScr,
                                   WA_Borderless, TRUE,
                                   WA_Activate, FALSE,
                                   WA_RMBTrap, TRUE,
                                   WA_ScreenTitle, MenWin->ScreenTitle,
                                   WA_BackFill, GetNOPFillHook (),
                                   TAG_DONE)))
    {
      CleanUpMenuBox ();
      return (FALSE);
    }

    BoxDrawRPort = BoxWin->RPort;
    BoxDrawLeft = 0;
    BoxDrawTop = 0;

    if (LookMC)
      AddShadow (BoxDrawRPort, BoxWidth, BoxHeight);

    DrawMenuBoxContents (Menu, BoxDrawRPort, BoxDrawLeft, BoxDrawTop);
  }
  else
  {
    if (!InstallRPort (BoxDepth, BoxWidth, BoxHeight, &BoxRPort, &BoxBitMap, &BoxLayerInfo, &BoxLayer,
                       MenScr->RastPort.BitMap))
    {
      CleanUpMenuBox ();
      return (FALSE);
    }

    DrawMenuBoxContents (Menu, BoxRPort, 0, 0);

    if (!(BoxCRect = GetClipRect (BoxBitMap, BoxLeft, BoxTop, BoxLeft + BoxWidth - 1, BoxTop + BoxHeight - 1)))
    {
      CleanUpMenuBox ();
      return (FALSE);
    }

    SwapRPortClipRect (&ScrRPort, BoxCRect);
    BoxDrawRPort = &ScrRPort;
    BoxDrawLeft = BoxLeft;
    BoxDrawTop = BoxTop;
  }

  SetFont (BoxDrawRPort, MenFont);
  SetPens (BoxDrawRPort, MenTextCol, 0, JAM1);

  MenuBoxSwapped = TRUE;

  AktItem = NULL;
  DisarmMenu();

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
        SetDrawMode (StripDrawRPort, JAM1);

        if (Menu->Flags & MENUENABLED)
        {
          HiRect (StripDrawRPort, l, t, StripPopUp ? w : w - 1, h + 1, TRUE);
          SetFgPen (StripDrawRPort, MenHiCol);

          Move (StripDrawRPort, l + 4, t + 1 + StripDrawRPort->TxBaseline);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
        }
        else
        {
          HiRect (StripDrawRPort, l, t, StripPopUp ? w : w - 1, h + 1, FALSE);

          SetFgPen (StripDrawRPort, MenStdGrey2);

          Move (StripDrawRPort, l + 5, t + 1 + StripDrawRPort->TxBaseline + 1);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));

          SetFgPen (StripDrawRPort, MenStdGrey0);

          Move (StripDrawRPort, l + 4, t + 1 + StripDrawRPort->TxBaseline);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
        }

        if (StripPopUp)
        {
          if ((Menu->Flags & MENUENABLED) || MenuMode == MODE_KEYBOARD)
            Draw3DRect (StripDrawRPort, l, t, w, h + 1, AktPrefs.mmp_DblBorder, FALSE, FALSE);
        }
        else
        {
          Draw3DRect (StripDrawRPort, l, t, w - 1, h + 1, TRUE, (ScrHiRes && (!DblBorder)), FALSE);
          SetFgPen (StripDrawRPort, MenStdGrey0);

          SetDrawMode (StripDrawRPort, JAM1);

          DrawLine (StripDrawRPort, l + w - 1, t + 1,
                    l + w - 1, t + h - 1);
        }
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
GhostMenu (struct Menu * Menu, struct RastPort * RPort, UWORD Left, UWORD Top)
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

VOID
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
        SetDrawMode (StripDrawRPort, JAM1);

        if (Menu->Flags & MENUENABLED)
        {
          SetFgPen (StripDrawRPort, MenTextCol);

          Move (StripDrawRPort, l + 4, t + 1 + StripDrawRPort->TxBaseline);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
        }
        else
        {
          SetFgPen (StripDrawRPort, MenStdGrey2);

          Move (StripDrawRPort, l + 5, t + 1 + StripDrawRPort->TxBaseline + 1);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));

          SetFgPen (StripDrawRPort, MenStdGrey0);

          Move (StripDrawRPort, l + 4, t + 1 + StripDrawRPort->TxBaseline);
          Text (StripDrawRPort, Menu->MenuName, strlen (Menu->MenuName));
        }

        if (!StripPopUp)
        {
          SetFgPen (StripDrawRPort, MenDarkEdge);

          RectFill (StripDrawRPort, l, t + h, l + w - 1, t + h);
          SetFgPen (StripDrawRPort, MenLightEdge);

          RectFill (StripDrawRPort, l, t, l + w - 1, t);
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
          SetFgPen (StripDrawRPort, MenTextCol);

          SetFont (StripDrawRPort, MenFont);

          Move (StripDrawRPort, Menu->LeftEdge + StripLeftOffs + 4,
             Menu->TopEdge + StripTopOffs + 1 + StripDrawRPort->TxBaseline);
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

VOID
CleanUpMenuStrip (VOID)
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
  FreeRemappedImage (MXImageGhosted);
  FreeRemappedImage (NoMXImageGhosted);
  FreeRemappedImage (MXImageActive);
  FreeRemappedImage (NoMXImageActive);

  FreeRemappedImage (CommandImageRmp);
  FreeRemappedImage (CommandImageGhosted);
  FreeRemappedImage (CommandImageActive);

  FreeRemappedImage (SubArrowImageRmp);
  FreeRemappedImage (SubArrowImageGhosted);
  FreeRemappedImage (SubArrowImageActive);

  MXImageRmp = NoMXImageRmp = MXImageGhosted = NoMXImageGhosted = MXImageActive = NoMXImageActive = NULL;

  CheckImageRmp = NoCheckImage = CommandImageRmp = SubArrowImageRmp = NULL;
  CheckImageGhosted = CommandImageGhosted = SubArrowImageGhosted = NULL;
  CheckImageActive = CommandImageActive = SubArrowImageActive = NULL;

  FreeMenuColours (MenScr->ViewPort.ColorMap);

  if (MenuStripSwapped)
  {
    if (AktPrefs.mmp_NonBlocking)
    {
      if (StripWin)
        CloseWindow (StripWin);
      StripWin = NULL;
    }
    else
    {
      SwapRPortClipRect (&ScrRPort, StripCRect);
      FreeVecPooled (StripCRect);
      StripCRect = NULL;
    }
    MenuStripSwapped = FALSE;
  }

  if (StripRPort)
  {
    FreeRPort (StripBitMap, StripLayerInfo, StripLayer, StripWidth, StripHeight);
    StripRPort = NULL;
  }

  AktMenu = NULL;
  DisarmMenu();

  if (MenOpenedFont)
  {
    CloseFont (MenOpenedFont);
    MenOpenedFont = NULL;
  }
}

VOID
GetSubItemCoors (struct MenuItem *Item, UWORD * x1, UWORD * y1, UWORD * x2, UWORD * y2)
{
  *x1 = Item->LeftEdge + SubBoxLeftOffs + SubBoxLeft;
  *y1 = Item->TopEdge + SubBoxTopOffs + SubBoxTop;
  *x2 = *x1 + Item->Width - 1;
  *y2 = *y1 + Item->Height - 1;
}

VOID
GetItemCoors (struct MenuItem *Item, UWORD * x1, UWORD * y1, UWORD * x2, UWORD * y2)
{
  *x1 = Item->LeftEdge + BoxLeftOffs + BoxLeft;
  *y1 = Item->TopEdge + BoxTopOffs + BoxTop;
  *x2 = *x1 + Item->Width - 1;
  *y2 = *y1 + Item->Height - 1;
}

VOID
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

VOID
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

  Search = ToUpper (Search);

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
      Found = ToUpper (LookChar) == Search;

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

VOID
ChangeAktItem (struct MenuItem *NewItem, UWORD NewItemNum)
{
  if (AktItem)
  {
    CleanUpMenuSubBox ();
    DrawNormItem (AktItem);
    AktItem->Flags &= ~HIGHITEM;
    AktItem = NULL;
    ItemNum = NOITEM;
    DisarmMenu();
  }

  if (NewItem)
  {
    ItemNum = NewItemNum;
    if (DrawHiItem (AktItem = NewItem))
    {
      AktItem->Flags |= HIGHITEM;
      if (MenuMode != MODE_KEYBOARD)
      {
/*        if(LookMC)*/
/*          ArmMenu();*/
/*        else*/
        {
          if (!DrawMenuSubBox (AktMenu, AktItem, TRUE))
            DisplayBeep (MenScr);
        }
      }
    }
    else
    {
      ItemNum = NOITEM;
      AktItem = NULL;
      DisarmMenu();
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

  Search = ToUpper (Search);

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
      Found = ToUpper (LookChar) == Search;

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

VOID
ChangeAktMenu (struct Menu *NewMenu, UWORD NewMenuNum)
{
  if (AktMenu)
  {
    CleanUpMenuSubBox ();
    CleanUpMenuBox ();
    DrawNormMenu (AktMenu);
    AktMenu = NULL;
    DisarmMenu();
    MenuNum = NOMENU;
  }

  if (NewMenu)
  {
    MenuNum = NewMenuNum;
    if (DrawHiMenu (AktMenu = NewMenu))
    {
      if (MenuMode != MODE_KEYBOARD)
      {
/*        if(LookMC)*/
/*          ArmMenu();*/
/*        else*/
        {
          if (!DrawMenuBox (AktMenu, TRUE))
            DisplayBeep (MenScr);
        }
      }
    }
    else
    {
      MenuNum = NOMENU;
      AktMenu = NULL;
      DisarmMenu();
    }
  }
}

BOOL
FindMenuChar (char Search, BOOL * Single)
{
  struct Menu *ZwMenu, *OldAktMenu;
  UWORD ZwMenuNum;
  BOOL FirstFound, AnotherFound, Found;

  Search = ToUpper (Search);

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

    Found = (ZwMenu && ToUpper (ZwMenu->MenuName[0]) == Search);

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
LookMouse (UWORD MouseX, UWORD MouseY, BOOL NewSelect) /* True, wenn ausserhalb des Menüs */
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
        DisarmMenu();
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
      DisarmMenu();
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

VOID
CopyImageDimensions (struct Image *Dest, struct Image *Source)
{
  Dest->LeftEdge = Source->LeftEdge;
  Dest->TopEdge = Source->TopEdge;
  Dest->Height = Source->Height;
  Dest->Width = Source->Width;
}

VOID
DrawMenuStripContents (struct RastPort *RPort, UWORD Left, UWORD Top)
{
  struct Menu *ZwMenu;
  UWORD ZwY, X, Y;

  SetPens (RPort, MenBackGround, 0, JAM1);

  RectFill (RPort, Left, Top, Left + StripWidth - 1, Top + StripHeight - 1);

  SetFont (RPort, MenFont);

  if (StripPopUp)
    Draw3DRect (RPort, Left, Top, StripWidth, StripHeight, TRUE, ScrHiRes, DblBorder);
  else
  {
    if (LookMC)
    {
      SetDrawMode (RPort, JAM1);

      SetFgPen (RPort, MenLightEdge);
      DrawLinePairs (RPort, 3, Left, Top + StripHeight - 2,
                     Left, Top,
                     Left + StripWidth - 2, Top);
      SetFgPen (RPort, MenDarkEdge);
      DrawLinePairs (RPort, 3, Left + 1, Top + StripHeight - 1,
                     Left + StripWidth - 1, Top + StripHeight - 1,
                     Left + StripWidth - 1, Top + 1);

      SetFgPen (RPort, MenSectGrey);
      WritePixel (RPort, Left, Top + StripHeight - 1);
      WritePixel (RPort, Left + StripWidth - 1, Top);
    }
    else
    {
      SetFgPen (RPort, MenTrimPen);
      SetDrawMode (RPort, JAM1);

      DrawLine (RPort, Left, Top + StripHeight - 1,
                Left + StripWidth - 1, Top + StripHeight - 1);
    }
  }

  SetDrawMode (RPort, JAM1);

  ZwMenu = MenStrip;
  ZwY = StripTopOffs;
  while (ZwMenu)
  {
    if (!StripPopUp)
    {
      X = ZwMenu->LeftEdge + StripLeftOffs + 4;
      Y = Top + ZwMenu->TopEdge + StripTopOffs + 1 + RPort->TxBaseline;
    }
    else
    {
      X = Left + StripLeftOffs + 4;
      Y = Top + ZwY + 1 + RPort->TxBaseline;
      ZwY += MenFont->tf_YSize + STRIP_YDIST;
    }

    if (LookMC && !ZwMenu->Flags & MENUENABLED)
    {
      SetFgPen (RPort, MenStdGrey2);

      Move (RPort, X + 1, Y + 1);
      Text (RPort, ZwMenu->MenuName, strlen (ZwMenu->MenuName));

      SetFgPen (RPort, MenStdGrey0);
    }
    else
      SetFgPen (RPort, MenTextCol);

    Move (RPort, X, Y);
    Text (RPort, ZwMenu->MenuName, strlen (ZwMenu->MenuName));

    if (!LookMC && !ZwMenu->Flags & MENUENABLED)
      GhostMenu (ZwMenu, RPort, Left, Top);

    ZwMenu = ZwMenu->NextMenu;
  }
}

VOID
SetMCPens(BOOL Vanilla)
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

  StdRemapArray[9] = MenStdGrey0;
  StdRemapArray[10] = MenStdGrey1;
  StdRemapArray[11] = MenStdGrey2;

  memcpy(DreiDRemapArray,StdRemapArray,sizeof(StdRemapArray));
  memcpy(DreiDGhostedRemapArray,StdRemapArray,sizeof(StdRemapArray));
  memcpy(DreiDActiveRemapArray,StdRemapArray,sizeof(StdRemapArray));

  DreiDActiveRemapArray[ 8] = MenFillCol;
  DreiDActiveRemapArray[ 9] = MenActiveGrey0;
  DreiDActiveRemapArray[10] = MenActiveGrey1;
  DreiDActiveRemapArray[11] = MenActiveGrey2;

  if(Vanilla)
  {
    DreiDRemapArray[MenMenuTextCol] = MenTextCol;
    DreiDRemapArray[MenMenuBackCol] = MenBackGround;

    DreiDActiveRemapArray[MenMenuTextCol] = MenHiCol;
    DreiDActiveRemapArray[MenMenuBackCol] = MenFillCol;

    DreiDGhostedRemapArray[MenMenuTextCol] = MenStdGrey0;
    DreiDGhostedRemapArray[MenMenuBackCol] = MenBackGround;
  }
}

BOOL
DrawMenuStrip (BOOL PopUp, UBYTE NewLook, BOOL ActivateMenu)
{
  struct DrawInfo *DrawInfo;
  struct MenuRemember *LookRemember;
  struct Image *ZwCheckImage, *ZwCommandImage;
  struct ColorMap *ColorMap;
  BOOL Ok, UseLow;
  BOOL DoFlipCheck, DoFlipCommand, DoFlipMX, DoFlipSubArrow;
  WORD ZwWert;
  struct Menu *ZwMenu;
  UWORD *Pens;
  LONG MaxMapPen,MaxMapDepth;

  memset(StdRemapArray,0,sizeof(StdRemapArray));

  MenuStripSwapped = FALSE;

  ScrRPort = MenScr->RastPort;
  SetDrawMask (&ScrRPort, ~0);

  SetDrawMode (&ScrRPort, JAM1);

  SetAfPt (&ScrRPort, NULL, 0);

  StripDepth = GetBitMapDepth (MenScr->RastPort.BitMap);
  ScrHiRes = (MenScr->Flags & SCREENHIRES) != NULL;

  DrawInfo = GetScreenDrawInfo (MenScr);
  Pens = DrawInfo->dri_Pens;

  if (MenOpenedFont = OpenFont (MenScr->Font))
    MenFont = MenOpenedFont;
  else
    MenFont = DrawInfo->dri_Font;

  SetFont (&ScrRPort, MenFont);
  AskFont (&ScrRPort, &MenTextAttr);

  Look3D = FALSE;
  LookMC = FALSE;

  Ok = TRUE;

  CheckImage = MXImage = NoMXImage = CommandImage = NoCheckImage = NULL;

  MXImageRmp = NoMXImageRmp = MXImageGhosted = NoMXImageGhosted = MXImageActive = NoMXImageActive = NULL;
  CheckImageRmp = CommandImageRmp = SubArrowImageRmp = NULL;
  CheckImageGhosted = CommandImageGhosted = SubArrowImageGhosted = NULL;
  CheckImageActive = CommandImageActive = SubArrowImageActive = NULL;

  DoFlipCheck = DoFlipCommand = DoFlipMX = DoFlipSubArrow = FALSE;

  ColorMap = MenScr->ViewPort.ColorMap;

  if (NewLook == LOOK_MC && StripDepth >= 2 && V39 && ScrHiRes)
  {
    LONG red, green, blue;

    MenMenuTextCol = Pens[BARDETAILPEN];
    MenMenuBackCol = Pens[BARBLOCKPEN];

    ResetMenuColours ();

    AddMenuColour8 (0x95, 0x95, 0x95);  // Grey0
    AddMenuColour8 (0x00, 0x00, 0x00);  // Black
    AddMenuColour8 (0xFF, 0xFF, 0xFF);  // White
    AddMenuColour8 (0x3B, 0x67, 0xA2);  // Blue
    AddMenuColour8 (0x7B, 0x7B, 0x7B);  // Grey1
    AddMenuColour8 (0xAF, 0xAF, 0xAF);  // Grey2
    AddMenuColour8 (0xAA, 0x90, 0x7C);  // Beige
    AddMenuColour8 (0xFF, 0xA9, 0x97);  // Pink

    if(StripDepth > 3)
    {
	red = (AktPrefs.mmp_LightEdge.R >> 24) + (AktPrefs.mmp_DarkEdge.R >> 24);
	green = (AktPrefs.mmp_LightEdge.G >> 24) + (AktPrefs.mmp_DarkEdge.G >> 24);
	blue = (AktPrefs.mmp_LightEdge.B >> 24) + (AktPrefs.mmp_DarkEdge.B >> 24);

	AddMenuColour8 ((red + 1) / 2, (green + 1) / 2, (blue + 1) / 2);  // SectGrey

	red = AktPrefs.mmp_Background.R >> 24;
	green = AktPrefs.mmp_Background.G >> 24;
	blue = AktPrefs.mmp_Background.B >> 24;

	AddMenuColour8 (red - 26, green - 26, blue - 26);  // StdGrey0
	AddMenuColour8 (red, green, blue);  // StdGrey1
	AddMenuColour8 (red + 26, green + 26, blue + 26);  // StdGrey2

	red = AktPrefs.mmp_FillCol.R >> 24;
	green = AktPrefs.mmp_FillCol.G >> 24;
	blue = AktPrefs.mmp_FillCol.B >> 24;

	AddMenuColour8 (red - 26, green - 26, blue - 26);  // ActiveGrey0
	AddMenuColour8 (red, green, blue);  // ActiveGrey1
	AddMenuColour8 (red + 26, green + 26, blue + 26);  // ActiveGrey2
    }
    else
    {
	AddMenuColour8 (0x7B, 0x7B, 0x7B);  // SectGrey
	AddMenuColour8 (0x7B, 0x7B, 0x7B);  // StdGrey0
	AddMenuColour8 (0x95, 0x95, 0x95);  // StdGrey1
	AddMenuColour8 (0xAF, 0xAF, 0xAF);  // StdGrey2

	AddMenuColour8 (0x7B, 0x7B, 0x7B);  // ActiveGrey0
	AddMenuColour8 (0x95, 0x95, 0x95);  // ActiveGrey1
	AddMenuColour8 (0xAF, 0xAF, 0xAF);  // ActiveGrey2
    }

    AddMenuColour (AktPrefs.mmp_LightEdge.R, AktPrefs.mmp_LightEdge.G, AktPrefs.mmp_LightEdge.B);
    AddMenuColour (AktPrefs.mmp_DarkEdge.R, AktPrefs.mmp_DarkEdge.G, AktPrefs.mmp_DarkEdge.B);
    AddMenuColour (AktPrefs.mmp_Background.R, AktPrefs.mmp_Background.G, AktPrefs.mmp_Background.B);
    AddMenuColour (AktPrefs.mmp_TextCol.R, AktPrefs.mmp_TextCol.G, AktPrefs.mmp_TextCol.B);
    AddMenuColour (AktPrefs.mmp_HiCol.R, AktPrefs.mmp_HiCol.G, AktPrefs.mmp_HiCol.B);
    AddMenuColour (AktPrefs.mmp_FillCol.R, AktPrefs.mmp_FillCol.G, AktPrefs.mmp_FillCol.B);

    Look3D = TRUE;

    if (AllocateMenuColours (ColorMap))
    {

      LONG *Pen;

      Pen = MenuPens;

      MenXENGrey0 = *Pen++;
      MenXENBlack = *Pen++;
      MenXENWhite = *Pen++;
      MenXENBlue = *Pen++;
      MenXENGrey1 = *Pen++;
      MenXENGrey2 = *Pen++;
      MenXENBeige = *Pen++;
      MenXENPink = *Pen++;

      MenSectGrey = *Pen++;

      MenStdGrey0 = *Pen++;
      MenStdGrey1 = *Pen++;
      MenStdGrey2 = *Pen++;

      MenActiveGrey0 = *Pen++;
      MenActiveGrey1 = *Pen++;
      MenActiveGrey2 = *Pen++;

      MenLightEdge = *Pen++;
      MenDarkEdge = *Pen++;
      MenBackGround = *Pen++;
      MenTextCol = *Pen++;
      MenHiCol = *Pen++;
      MenFillCol = *Pen++;

  /*
      MenXENGrey0 = 1;
      MenXENBlack = 2;
      MenXENWhite = 3;
      MenXENBlue = 4;
      MenXENGrey1 = 5;
      MenXENGrey2 = 6;
      MenXENBeige = 7;
      MenXENPink = 8;

      MenSectGrey = 9;

      MenStdGrey0 = 10;
      MenStdGrey1 = 11;
      MenStdGrey2 = 12;

      MenActiveGrey0 = 13;
      MenActiveGrey1 = 14;
      MenActiveGrey2 = 15;

      MenLightEdge = 16;
      MenDarkEdge = 17;
      MenBackGround = 18;
      MenTextCol = 19;
      MenHiCol = 20;
      MenFillCol = 21;
  */
      LookMC = (MenTextCol != MenBackGround && MenHiCol != MenFillCol && MenStdGrey0 != MenStdGrey2);

      if(LookMC)
      {
        LONG i;

        MaxMapPen = -1;
        for(i = 0 ; i < NUM_MENU_PENS ; i++)
        {
          if(MenuPens[i] > MaxMapPen)
            MaxMapPen = MenuPens[i];
        }

        MaxMapDepth = min(8,StripDepth);

        for(i = 1 ; i <= 8 ; i++)
        {
          if(MaxMapPen <= (1<<i)-1)
          {
            MaxMapDepth = i;
            break;
          }
        }
      }
    }
    else
      LookMC = FALSE;

    if (!LookMC)
      FreeMenuColours (ColorMap);

    DblBorder = (AktPrefs.mmp_DblBorder == 1);
  }

  if ((NewLook == LOOK_3D || NewLook == LOOK_MC) && (!LookMC) && StripDepth >= 2)
  {
    MenLightEdge = Pens[SHINEPEN];
    MenDarkEdge = Pens[SHADOWPEN];
    MenBackGround = Pens[BACKGROUNDPEN];
    MenTextCol = Pens[TEXTPEN];
    MenHiCol = Pens[HIGHLIGHTTEXTPEN];
    MenFillCol = Pens[FILLPEN];
    MenBlockPen = Pens[BLOCKPEN];
    MenDetailPen = Pens[DETAILPEN];
    MenMenuTextCol = Pens[BARDETAILPEN];
    MenMenuBackCol = Pens[BARBLOCKPEN];

    MenTrimPen = MenBlockPen;
    MenComplMsk = MenFillCol;
    Look3D = (MenTextCol != MenBackGround && MenLightEdge != MenDarkEdge);
    LookMC = FALSE;
    DblBorder = (AktPrefs.mmp_DblBorder == 1);
  }

  if (Look3D)
  {
    if (LookMC)
    {
      SetMCPens(TRUE);
    }
    else
    {
      StdRemapArray[0] = MenBackGround;
      StdRemapArray[1] = MenDarkEdge;
      StdRemapArray[2] = MenLightEdge;
      StdRemapArray[3] = MenFillCol;

      memcpy(DreiDRemapArray,StdRemapArray,sizeof(StdRemapArray));

      DreiDRemapArray[MenMenuTextCol] = MenTextCol;
      DreiDRemapArray[MenMenuBackCol] = MenBackGround;
    }

    ObtainGlyphs (MenWin, &CheckImage, &CommandImage);

    if (CheckImage)
    {
      if (Ok)
        Ok = MakeRemappedImage (&CheckImageRmp, CheckImage, StripDepth, DreiDRemapArray);
      NoMXImage = &NoCheckImage2Pl;
      CopyImageDimensions (NoMXImage, CheckImage);

      if (LookMC)
      {
        if (Ok)
          MakeRemappedImage (&CheckImageGhosted, CheckImage, StripDepth, DreiDGhostedRemapArray);
        if (Ok)
          MakeRemappedImage (&CheckImageActive, CheckImage, StripDepth, DreiDActiveRemapArray);
      }

      CheckImage = CheckImageRmp;
      MXImage = CheckImage;

      DoFlipMX = FALSE;
      DoFlipCheck = FALSE;
    }

    if (CommandImage)
    {
      DoFlipCommand = FALSE;
      if (Ok)
        Ok = MakeRemappedImage (&CommandImageRmp, CommandImage, StripDepth, DreiDRemapArray);
      if (LookMC)
      {
        if (Ok)
          Ok = MakeRemappedImage (&CommandImageGhosted, CommandImage, StripDepth, DreiDGhostedRemapArray);
        if (Ok)
          Ok = MakeRemappedImage (&CommandImageActive, CommandImage, StripDepth, DreiDActiveRemapArray);
      }

      CommandImage = CommandImageRmp;
    }

    if (LookMC)
    {
      BOOL SpecialCheckImage = (CheckImage != NULL);

      SetMCPens(FALSE);

      Ok = TRUE;

      if (MenFont->tf_YSize >= 11)
      {
        if (!CommandImage)
        {
          if (Ok)
            Ok = MakeRemappedImage (&CommandImageRmp, &AmigaNormal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&CommandImageGhosted, &AmigaGhosted, MaxMapDepth, StdRemapArray);

          if (Ok)
            Ok = MakeRemappedImage (&CommandImageActive, &AmigaNormal, MaxMapDepth, DreiDActiveRemapArray);

          CommandImage = CommandImageRmp;
        }

        if (!CheckImage)
        {
          if (Ok)
            Ok = MakeRemappedImage (&CheckImageRmp, &CheckNormal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&CheckImageGhosted, &CheckGhosted, MaxMapDepth, StdRemapArray);

          if (Ok)
            Ok = MakeRemappedImage (&CheckImageActive, &CheckNormal, MaxMapDepth, DreiDActiveRemapArray);

          CheckImage = CheckImageRmp;
        }

        NoCheckImage = &NoCheckImage2Pl;

        NoCheckImage->PlaneOnOff = MenBackGround;
        CopyImageDimensions (NoCheckImage, CheckImage);

        if (Ok)
          Ok = MakeRemappedImage (&SubArrowImageRmp, &ArrowNormal, MaxMapDepth, StdRemapArray);
        if (Ok)
          Ok = MakeRemappedImage (&SubArrowImageGhosted, &ArrowGhosted, MaxMapDepth, StdRemapArray);

        if (Ok)
          Ok = MakeRemappedImage (&SubArrowImageActive, &ArrowNormal, MaxMapDepth, DreiDActiveRemapArray);

        SubArrowImage = SubArrowImageRmp;

        if (!SpecialCheckImage)
        {
          if (Ok)
            Ok = MakeRemappedImage (&MXImageRmp, &MXDownNormal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&NoMXImageRmp, &MXUpNormal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&MXImageGhosted, &MXDownGhosted, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&NoMXImageGhosted, &MXUpGhosted, MaxMapDepth, StdRemapArray);

          MXImage = MXImageRmp;
          NoMXImage = NoMXImageRmp;

          if (Ok)
            Ok = MakeRemappedImage (&MXImageActive, &MXDownNormal, MaxMapDepth, DreiDActiveRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&NoMXImageActive, &MXUpNormal, MaxMapDepth, DreiDActiveRemapArray);

          DoFlipMX = FALSE;
        }
      }
      else
      {
        if (!CommandImage)
        {
          if (Ok)
            Ok = MakeRemappedImage (&CommandImageRmp, &Amiga8_Normal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&CommandImageGhosted, &Amiga8_Ghosted, MaxMapDepth, StdRemapArray);

          if (Ok)
            Ok = MakeRemappedImage (&CommandImageActive, &Amiga8_Normal, MaxMapDepth, DreiDActiveRemapArray);

          CommandImage = CommandImageRmp;
        }

        if (!CheckImage)
        {
          if (Ok)
            Ok = MakeRemappedImage (&CheckImageRmp, &Check8_Normal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&CheckImageGhosted, &Check8_Ghosted, MaxMapDepth, StdRemapArray);

          if (Ok)
            Ok = MakeRemappedImage (&CheckImageActive, &Check8_Normal, MaxMapDepth, DreiDActiveRemapArray);

          CheckImage = CheckImageRmp;
        }

        NoCheckImage = &NoCheckImage2Pl;

        NoCheckImage->PlaneOnOff = MenBackGround;
        CopyImageDimensions (NoCheckImage, CheckImage);

        if (Ok)
          Ok = MakeRemappedImage (&SubArrowImageRmp, &Arrow8_Normal, MaxMapDepth, StdRemapArray);
        if (Ok)
          Ok = MakeRemappedImage (&SubArrowImageGhosted, &Arrow8_Ghosted, MaxMapDepth, StdRemapArray);

        if (Ok)
          Ok = MakeRemappedImage (&SubArrowImageActive, &Arrow8_Normal, MaxMapDepth, DreiDActiveRemapArray);

        SubArrowImage = SubArrowImageRmp;

        if (!SpecialCheckImage)
        {
          if (Ok)
            Ok = MakeRemappedImage (&MXImageRmp, &MXDown8_Normal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&NoMXImageRmp, &MXUp8_Normal, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&MXImageGhosted, &MXDown8_Ghosted, MaxMapDepth, StdRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&NoMXImageGhosted, &MXUp8_Ghosted, MaxMapDepth, StdRemapArray);

          MXImage = MXImageRmp;
          NoMXImage = NoMXImageRmp;

          if (Ok)
            Ok = MakeRemappedImage (&MXImageActive, &MXDown8_Normal, MaxMapDepth, DreiDActiveRemapArray);
          if (Ok)
            Ok = MakeRemappedImage (&NoMXImageActive, &MXUp8_Normal, MaxMapDepth, DreiDActiveRemapArray);

          DoFlipMX = FALSE;
        }
      }
    }
    else
    {
      if (ScrHiRes)
      {
        if (V39 && (MenWin->Flags & WFLG_NEWLOOKMENUS) != 0 && DrawInfo->dri_CheckMark)
          UseLow = (DrawInfo->dri_CheckMark->Width <= 16);
        else
          UseLow = (MenWin->CheckMark->Width <= 14);

        if (!CheckImage)
        {
          CheckImage = (UseLow) ? &CheckImageLow2Pl : &CheckImage2Pl;
          DoFlipCheck = TRUE;
        }

        if (!MXImage)
        {
          MXImage = (UseLow) ? &MXImageLow2Pl : &MXImage2Pl;
          NoMXImage = (UseLow) ? &NoMXImageLow2Pl : &NoMXImage2Pl;
          DoFlipMX = TRUE;
        }

        if (!CommandImage)
        {
          CommandImage = &AmigaImage2Pl;
          DoFlipCommand = TRUE;
        }

        SubArrowImage = &SubArrowImage2Pl;
      }
      else
      {
        if (!CheckImage)
        {
          CheckImage = &CheckImageLow2Pl;
          DoFlipCheck = TRUE;
        }

        if (!MXImage)
        {
          MXImage = &MXImageLow2Pl;
          NoMXImage = &NoMXImageLow2Pl;
          DoFlipMX = TRUE;
        }

        if (!CommandImage)
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
    if (DrawInfo->dri_Version >= 2 && DrawInfo->dri_NumPens > 9 && V39)
    {
      MenLightEdge = Pens[BARDETAILPEN];
      MenDarkEdge = Pens[BARDETAILPEN];
      MenBackGround = Pens[BARBLOCKPEN];
      MenTextCol = Pens[BARDETAILPEN];
      MenFillCol = Pens[BARBLOCKPEN];
      MenBlockPen = Pens[BARBLOCKPEN];
      MenDetailPen = Pens[BARDETAILPEN];
      MenTrimPen = Pens[BARTRIMPEN];
      MenComplMsk = (MenTextCol | MenBackGround);
      NoCheckImage = NoMXImage = NULL;
      CheckImage = DrawInfo->dri_CheckMark;
      if (!CheckImage)
        CheckImage = MenWin->CheckMark;
      MXImage = CheckImage;

      CommandImage = DrawInfo->dri_AmigaKey;

      DoFlipCommand = FALSE;
      DoFlipCheck = DoFlipMX = FALSE;

      StdRemapArray[0] = MenBlockPen;
      StdRemapArray[1] = MenDetailPen;
    }
    if (!V39 || (MenWin->Flags & WFLG_NEWLOOKMENUS) == 0)
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


    ObtainGlyphs (MenWin, &ZwCheckImage, &ZwCommandImage);
    if (ZwCheckImage)
    {
      CheckImage = ZwCheckImage;
      DoFlipCheck = FALSE;
    }
    if (ZwCommandImage)
    {
      CommandImage = ZwCommandImage;
      DoFlipCommand = FALSE;
    }

    if (ScrHiRes)
    {
      if (!CommandImage)
      {
        CommandImage = &AmigaImage1Pl;
        DoFlipCommand = TRUE;
      }

      SubArrowImage = &SubArrowImage1Pl;
      DoFlipSubArrow = TRUE;
    }
    else
    {
      if (!CommandImage)
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

  if (!LookMC)
  {
    NoCheckImage->PlaneOnOff = NoMXImage->PlaneOnOff = MenBackGround;

    if (Ok && DoFlipMX)
    {
      Ok = MakeRemappedImage (&MXImageRmp, MXImage, StripDepth, StdRemapArray);
      if (Ok && NoMXImage)
      {
        Ok = MakeRemappedImage (&NoMXImageRmp, NoMXImage, StripDepth, StdRemapArray);
        if (!Ok)
          FreeRemappedImage (MXImage);
      }

      MXImage = MXImageRmp;
      NoMXImage = NoMXImageRmp;
    }

    if (Ok && DoFlipCommand)
    {
      Ok = MakeRemappedImage (&CommandImageRmp, CommandImage, StripDepth, StdRemapArray);
      CommandImage = CommandImageRmp;
    }

    if (Ok && DoFlipCheck)
    {
      Ok = MakeRemappedImage (&CheckImageRmp, CheckImage, StripDepth, StdRemapArray);
      CheckImage = CheckImageRmp;
    }

    if (Ok && DoFlipSubArrow)
    {
      Ok = MakeRemappedImage (&SubArrowImageRmp, SubArrowImage, StripDepth, StdRemapArray);
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

    if(LookMC)
      StripHeight++;
  }

  if (AktPrefs.mmp_NonBlocking)
  {
    LONG windowWidth, windowHeight;

    ClampShadow (MenScr, StripLeft, StripTop, StripWidth, StripHeight, &windowWidth, &windowHeight);

    if (!(StripWin = OpenWindowTags (NULL,
                                     WA_Left, StripLeft,
                                     WA_Top, StripTop,
                                     WA_Width, windowWidth,
                                     WA_Height, windowHeight,
                                     WA_CustomScreen, MenScr,
                                     WA_Borderless, TRUE,
                                     WA_Activate, FALSE,
                                     WA_RMBTrap, TRUE,
                                     WA_ScreenTitle, MenWin->ScreenTitle,
                                     WA_BackFill, GetNOPFillHook (),
                                     TAG_DONE)))
    {
      CleanUpMenuStrip ();
      return (FALSE);
    }

    StripDrawRPort = StripWin->RPort;
    StripDrawLeft = 0;
    StripDrawTop = 0;

    if (LookMC && StripPopUp)
      AddShadow (StripDrawRPort, StripWidth, StripHeight);

    DrawMenuStripContents (StripDrawRPort, StripDrawLeft, StripDrawTop);
  }
  else
  {
    if (!InstallRPort (StripDepth, StripWidth, StripHeight, &StripRPort, &StripBitMap, &StripLayerInfo, &StripLayer,
                       MenScr->RastPort.BitMap))
    {
      CleanUpMenuStrip ();
      return (FALSE);
    }

    DrawMenuStripContents (StripRPort, 0, 0);

    if (!(StripCRect = GetClipRect (StripBitMap, StripLeft, StripTop, StripLeft + StripWidth - 1, StripTop + StripHeight - 1)))
    {
      CleanUpMenuStrip ();
      return (FALSE);
    }

    SwapRPortClipRect (&ScrRPort, StripCRect);
    StripDrawRPort = &ScrRPort;
    StripDrawLeft = StripLeft;
    StripDrawTop = StripTop;
  }

  SetFont (StripDrawRPort, MenFont);
  SetPens (StripDrawRPort, MenTextCol, 0, JAM1);

  AktMenu = NULL;
  MenuStripSwapped = TRUE;
  DisarmMenu();


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
