/***************************************************************************

 MagicMenu - Intuition PopupMenu enhancement

 Copyright (C) 1993-1997 by Martin Korndörfer
 Copyright (C) 1997-2001 by Olaf `Olsen' Barthel
 Copyright (C) 2001 by Stephan Rupprecht, Jens Langner

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 MagicMenu Official Support Site :  http://www.magicmenu.de/

 $Id$

***************************************************************************/

/*#define DEBUG*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

#ifdef __STORMGCC__
#undef BltBitMap
#undef BltMaskBitMapRastPort
#endif

#ifndef min
#define min(a,b)	((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a,b)	((a)>(b)?(a):(b))
#endif

/******************************************************************************/

STATIC BOOL MenuNotDrawn;
STATIC BOOL SubNotDrawn;

/******************************************************************************/

//#define DEMO_MENU

/******************************************************************************/

struct LayerMsg
{
	struct Layer *		Layer;
	struct Rectangle	Bounds;
};

struct FatHook
{
	struct Hook		Hook;
	struct BitMap *		BitMap;
};

VOID SAVEDS __ASM
WindowBackfillRoutine(
	REG(a0, struct FatHook *	Hook),
	REG(a2, struct RastPort *	RPort),
	REG(a1, struct LayerMsg *	Bounds))
{
	LONG FromX,FromY;

	FromX = Bounds->Bounds.MinX - Bounds->Layer->bounds.MinX;
	FromY = Bounds->Bounds.MinY - Bounds->Layer->bounds.MinY;

	BltBitMap(
		Hook->BitMap,FromX,FromY,
		RPort->BitMap,Bounds->Bounds.MinX,Bounds->Bounds.MinY,
		Bounds->Bounds.MaxX - Bounds->Bounds.MinX + 1,
		Bounds->Bounds.MaxY - Bounds->Bounds.MinY + 1,
		MINTERM_COPY,~0,NULL);
}

/******************************************************************************/

STATIC VOID
CopyBackWindow(struct Window * win)
{
	if(win != NULL && win->UserData != NULL)
	{
		struct FatHook * hook = (struct FatHook *)win->UserData;

		BltBitMap(
			win->WScreen->RastPort.BitMap,win->LeftEdge,win->TopEdge,
			hook->BitMap,0,0,
			win->Width,win->Height,
			MINTERM_COPY,~0,NULL);
	}
}

STATIC VOID
CloseCommonWindow(
	struct Window ** windowPtr,
	struct BackgroundCover ** backgroundCoverPtr)
{
	struct Window * window = (*windowPtr);
	struct BackgroundCover * backgroundCover = (*backgroundCoverPtr);

	if(window != NULL)
	{
		struct FatHook * hook;

		hook = (struct FatHook *)window->UserData;
		CloseWindow(window);

		disposeBitMap(hook->BitMap, FALSE);
		FreeVec(hook);

		(*windowPtr) = NULL;
	}

	if(backgroundCover != NULL)
	{
		DeleteBackgroundCover(backgroundCover);
		(*backgroundCoverPtr) = NULL;
	}
}

STATIC BOOL
OpenCommonWindow(
	LONG Left,
	LONG Top,
	LONG Width,
	LONG Height,
	LONG Level,
	struct Window ** resultWindowPtr,
	struct BackgroundCover ** resultBackgroundCoverPtr)
{
	struct Window *Window = NULL;
	struct BackgroundCover * bgc = NULL;
	LONG OriginalHeight,OriginalWidth,ShadowSize;
	struct BitMap * CustomBitMap;
	struct FatHook * Hook;
	LONG Black;
	BOOL success = FALSE;

	if(V39 && MenXENBlack)
		Black = MenXENBlack;
	else
	{
		Black = MenDarkEdge;
		if(Black == MenLightEdge)
			Black = 0;
	}

	if(Look3D && AktPrefs.mmp_CastShadows && Black != 0)
	{
		ShadowSize = SHADOW_SIZE + Level * 2;

		OriginalWidth	= Width;
		OriginalHeight	= Height;

		Width	+= ShadowSize;
		Height	+= ShadowSize;

		if(Left + Width > MenScr->Width)
			Width = MenScr->Width - Left;

		if(Top + Height > MenScr->Height)
			Height = MenScr->Height - Top;
 	}
	else
		ShadowSize = 0;

	CustomBitMap = allocBitMap(GetBitMapDepth(MenScr->RastPort.BitMap),Width,Height,MenScr->RastPort.BitMap,FALSE);

	Hook = AllocVec(sizeof(*Hook),MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR);

	if(CustomBitMap != NULL && Hook != NULL)
	{
		BltBitMap(MenScr->RastPort.BitMap,Left,Top,CustomBitMap,0,0,Width,Height,MINTERM_COPY,~0,NULL);

		bgc = CreateBackgroundCover(CustomBitMap,0,0,Width,Height,Level==0&&!GlobalPopUp);

		Hook->Hook.h_Entry	= (HOOKFUNC)WindowBackfillRoutine;
		Hook->BitMap		= CustomBitMap;

		Window = OpenWindowTags(NULL,
			WA_Left,			Left,
			WA_Top,				Top,
			WA_Width,			Width,
			WA_Height,			Height,
			WA_CustomScreen,	MenScr,
			WA_Flags,			WFLG_BORDERLESS | WFLG_RMBTRAP | WFLG_SIMPLE_REFRESH,
			WA_BackFill,		Hook,
			WA_ScreenTitle,		MenWin->ScreenTitle,
		TAG_DONE);

		if(Window != NULL)
		{
			Window->UserData = (APTR)Hook;

			if(ShadowSize > 0 && (OriginalWidth < Width || OriginalHeight < Height))
			{
				struct RastPort *RPort;

				RPort = Window->RPort;

				SetPens(RPort, Black, 0, JAM1);
				SetAfPt(RPort, Crosshatch, 1);

				if(OriginalWidth < Width)
					DrawShadow(RPort, OriginalWidth, ShadowSize, OriginalWidth + ShadowSize - 1, OriginalHeight - 1, RIGHT_PART);

				if(OriginalHeight < Height)
					DrawShadow(RPort, ShadowSize, OriginalHeight, OriginalWidth + ShadowSize - 1, OriginalHeight + ShadowSize - 1, BOTTOM_PART);

				SetAfPt (RPort, NULL, 0);
			}

			success = TRUE;
		}
	}

	if(Window == NULL)
	{
		DeleteBackgroundCover(bgc);
		disposeBitMap(CustomBitMap, FALSE);
		FreeVec(Hook);
	}

	(*resultWindowPtr) = Window;
	(*resultBackgroundCoverPtr) = bgc;

	return(success);
}

/******************************************************************************/

#define NUM_MENU_PENS 21

STATIC ULONG MenuColours[NUM_MENU_PENS * 3];
STATIC ULONG *MenuColour;
STATIC LONG MenuPens[NUM_MENU_PENS];
STATIC BOOL MenuPensAllocated = FALSE;

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

	*MenuColour++ = (red<<24)|(red<<16)|(red<<8)|red;
	*MenuColour++ = (green<<24)|(green<<16)|(green<<8)|green;
	*MenuColour++ = (blue<<24)|(blue<<16)|(blue<<8)|blue;
}

STATIC VOID
FreeMenuColours (struct ColorMap *cmap)
{
	if (V39 && MenuPensAllocated)
	{
		LONG i;

		for (i = 0; i < NUM_MENU_PENS; i++)
		{
			if(MenuPens[i] != -1)
			{
				ReleasePen (cmap, MenuPens[i]);
				MenuPens[i] = -1;
			}
		}

		MenuPensAllocated = FALSE;
	}
}

STATIC BOOL
AllocateMenuColours (struct ColorMap *cmap)
{
	ULONG *colour;
	LONG i;

	colour = MenuColours;

	MenuPensAllocated = TRUE;

	for (i = 0; i < NUM_MENU_PENS; i++)
	{
		ULONG	r, g, b;
		/* Stephan: StormGCC/gnuc fix */
		r = *colour++;
		g = *colour++;
		b = *colour++;

		if ((MenuPens[i] = AllocateColour(cmap, r,g,b)) == -1)
		{
			FreeMenuColours (cmap);
			return (FALSE);
		}
	}

	return (TRUE);
}

/******************************************************************************/

/* Warum eine eigene Routine? Das Original im ROM macht weit mehr als es muß
 * und außerdem brauche ich weiter unten TxBaseline. Man könnte noch ein paar
 * Optimierungen einbauen, aber das ist leider recht knifflig.
 */
VOID
LocalPrintIText(struct RastPort *rp,struct IntuiText *itext,WORD left,WORD top)
{
	struct TextFont *font;
	UBYTE style;

	do
	{
		SetPens(rp,itext->FrontPen,itext->BackPen,itext->DrawMode);

		if(itext->ITextFont)
		{
			if(font = OpenFont(itext->ITextFont))
			{
				SetFont(rp,font);

				if(style = itext->ITextFont->ta_Style & 0x3F)
					SetSoftStyle(rp,style,0x3F);
			}
		}
		else
			font = NULL;

		Move(rp,left + itext->LeftEdge,top + itext->TopEdge + rp->TxBaseline);
		Text(rp,itext->IText,strlen(itext->IText));

		if(font)
		{
			CloseFont(font);

			if(style)
				SetSoftStyle(rp,0,0x3F);
		}
	}
	while(itext = itext->NextText);
}

#define PrintIText LocalPrintIText

/******************************************************************************/

STATIC VOID
LocalDrawImage(
	struct RastPort * rp,
	struct Image * image,
	LONG left,
	LONG top)
{
	if(image->Depth != -1)
	{
		LONG x,y;

		x = left + image->LeftEdge;
		y = top + image->TopEdge;

		if(image->NextImage == (struct Image *)IMAGE_MAGIC && (NOT V39 || (GetBitMapAttr(rp->BitMap,BMA_FLAGS) & BMF_INTERLEAVED) == 0))
		{
			SHOWMSG("has magic");
			SHOWVALUE(image->PlanePick);

			if(image->PlanePick != 0)
			{
				PLANEPTR mask;

				mask = (PLANEPTR)image[1].ImageData;
				if(mask != NULL)
				{
					struct BitMap bm;

					CreateBitMapFromImage(image,&bm);
					BltMaskBitMapRastPort(&bm,0,0,rp,x,y,image->Width,image->Height,ABC|ABNC|ANBC,mask);
				}
			}
		}
		else
		{
			if(image->PlanePick == 0)
			{
				LONG pen,mode;

				pen		= GetDrawMode(rp);
				mode	= GetFgPen(rp);

				SetFgPen(rp,image->PlaneOnOff);
				SetDrawMode(rp,JAM1);

				RectFill(rp,x,y,x+image->Width-1,y+image->Height-1); // Stephan

				SetFgPen(rp,pen);
				SetDrawMode(rp,mode);
			}
			else
			{
				struct BitMap bm;

				CreateBitMapFromImage(image,&bm);

				BltBitMapRastPort(&bm,0,0,rp,x,y,image->Width,image->Height,MINTERM_COPY);
			}
		}
	}
	else
	{
		DrawImage(rp,image,left,top);
	}
}

/******************************************************************************/

VOID
DrawMenuItem (struct RastPort *rp, struct MenuItem *Item, LONG x, LONG y, UWORD CmdOffs, BOOL GhostIt, BOOL Highlighted, WORD Left,WORD Width)
{
	struct IntuiText *Text;
	struct IntuiText MyText;
	struct Image MyImage;
	struct Image *Image;
	UWORD StartX, StartY;
	UWORD ZwWidth, NW;
	LONG z1;
	BOOL DrawCheck, Ghosted;

	/* Der Rest wird später initialisiert, wenn man ihn brauchen sollte. */
	CommandText.TopEdge = 0;
	CommandText.ITextFont = &MenTextAttr;

	StartX = x + Item->LeftEdge;
	StartY = y + Item->TopEdge;

	Ghosted = (GhostIt || !(Item->Flags & ITEMENABLED));

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
			if(Text->IText != NULL) /* Der Idiotentest... */
			{
				MyText = *Text;

				MyText.NextText = NULL;

				if (!LookMC || !AktPrefs.mmp_MarkSub || strcmp (MyText.IText, "»") != 0)
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

					NW = IntuiTextLength (&MyText) + MyText.LeftEdge;
					if (NW > ZwWidth)
						ZwWidth = NW;
				}

				Text = Text->NextText;
			}
		}
	}
	else
	{
		struct Image *ThatImage;

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
				if (ThatImage->PlanePick == 0 && ThatImage->Height <= 2)
				{
					if(LookMC)
					{
						if(!AktPrefs.mmp_DblBorder && AktPrefs.mmp_SeparatorBarStyle != 0)
						{
							LONG Top;

							Top = StartY + ThatImage->TopEdge;

							SetFgPen (rp, MenDarkEdge);
							DrawLine(rp,Left + 1,Top,Left + Width - 1,Top);

							SetFgPen (rp, MenLightEdge);
							DrawLine(rp,Left,Top + 1,Left + Width - 2,Top + 1);

							SetFgPen (rp, MenSectGrey);
							WritePixel (rp,Left,Top);
							WritePixel (rp,Left + Width - 1,Top + 1);
						}
						else
						{
							SetFgPen (rp, MenSeparatorGrey0);
							DrawLine(rp,StartX + ThatImage->LeftEdge, StartY + ThatImage->TopEdge,
								StartX + ThatImage->LeftEdge + ThatImage->Width - 2, StartY + ThatImage->TopEdge);
							WritePixel(rp,StartX + ThatImage->LeftEdge, StartY + ThatImage->TopEdge + 1);

							SetFgPen (rp, MenSeparatorGrey2);
							DrawLine (rp, StartX + ThatImage->LeftEdge + 1, StartY + ThatImage->TopEdge + 1,
								StartX + ThatImage->LeftEdge + ThatImage->Width - 1, StartY + ThatImage->TopEdge + 1);
							WritePixel(rp,StartX + ThatImage->LeftEdge + ThatImage->Width - 1, StartY + ThatImage->TopEdge);
						}
					}
					else
					{
						if(ThatImage->PlaneOnOff == MenBackGround)
						{
							struct Image OtherImage;

							OtherImage = *ThatImage;

							OtherImage.PlaneOnOff = MenTextCol;

							DrawImage (rp, &OtherImage, StartX, StartY);
						}
						else
							DrawImage (rp, ThatImage, StartX, StartY);
					}
				}
				else
					DrawImage (rp, ThatImage, StartX, StartY);
			}
			else
				DrawImage (rp, ThatImage, StartX, StartY);

			CommandText.TopEdge = ThatImage->TopEdge;

			Image = Image->NextImage;
		}
		ZwWidth = 0;
	}

	if (DrawCheck)
	{
		struct Image *WhichImage;
		LONG TopEdge;

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
					NoCheckImage->NextImage = (struct Image *)IMAGE_MAGIC;

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

		/* Das Checkmark-Zeichen wird an der Oberkante der Textzeile
		 * ausgerichtet.
		 */
		TopEdge = CommandText.TopEdge;

		/* Unter den Eintrag sollte das Zeichen aber auch nicht rutschen. */
		if(TopEdge > 0 && TopEdge + WhichImage->Height > Item->Height)
		{
			if((TopEdge = Item->Height - WhichImage->Height) < 0)
				TopEdge = 0;
		}

		LocalDrawImage (rp, WhichImage, StartX, StartY + TopEdge);
	}

	z1 = StartX + Item->Width;

	if (Item->Flags & COMMSEQ)
	{
		struct Image *WhichImage;
		LONG TopEdge;
		LONG BaseOffset;

		CommText[0] = Item->Command;

		CommText[1] = 0;

		if (LookMC)
		{
			if(Ghosted)
				WhichImage = CommandImageGhosted;
			else
			{
				if(Highlighted)
					WhichImage = CommandImageActive;
				else
					WhichImage = CommandImage;
			}
		}
		else
			WhichImage = CommandImage;

		CommandText.FrontPen = MenTextCol;
		CommandText.BackPen = MenBackGround;
		CommandText.DrawMode = JAM2;

		if (LookMC)
		{
			CommandText.DrawMode = JAM1;

			if (Ghosted)
			{
				CommandText.FrontPen = MenStdGrey2;
				PrintIText (rp, &CommandText, z1 - CmdOffs + 1, StartY + 1);

				CommandText.FrontPen = MenStdGrey0;
			}
			else
				CommandText.FrontPen = (Highlighted) ? MenHiCol : MenTextCol;
		}

		PrintIText (rp, &CommandText, z1 - CmdOffs, StartY);

		/* Das Amiga-Zeichen wird an der Grundlinie des Zeichensatzes
		 * ausgerichtet.
		 */

		BaseOffset = rp->TxBaseline - (WhichImage->Height - 1);

		if(!LookMC)
		{
			if(BaseOffset < 0)
				BaseOffset = 0;
		}

		if((TopEdge = CommandText.TopEdge + BaseOffset) < 0)
			TopEdge = 0;

		/* Unter den Eintrag sollte das Zeichen aber auch nicht rutschen. */
		if(TopEdge > 0 && TopEdge + WhichImage->Height > Item->Height)
		{
			if((TopEdge = Item->Height - WhichImage->Height) < 0)
				TopEdge = 0;
		}

		LocalDrawImage (rp, WhichImage,z1 - WhichImage->Width - CmdOffs - 2, StartY + TopEdge);
	}
	else if ((Item->SubItem) && (AktPrefs.mmp_MarkSub))
	{
		if (ZwWidth < Item->Width - SubArrowImage->Width - 5)
		{
			struct Image *WhichImage;
			LONG TopEdge;

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

			/* Der Pfeil wird an der Grundlinie des Zeichensatzes
			 * ausgerichtet.
			 */
			if((TopEdge = CommandText.TopEdge + rp->TxBaseline - WhichImage->Height) < 0)
				TopEdge = 0;

			/* Unter den Eintrag sollte der Pfeil aber auch nicht rutschen. */
			if(TopEdge > 0 && TopEdge + WhichImage->Height > Item->Height)
			{
				if((TopEdge = Item->Height - WhichImage->Height) < 0)
					TopEdge = 0;
			}

			LocalDrawImage (rp, WhichImage, z1 - WhichImage->Width - 3, StartY + TopEdge);
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
			CloseCommonWindow (&SubBoxWin, &SubBoxBackground);
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
		FreeRPort (SubBoxBitMap, SubBoxLayerInfo, SubBoxLayer, &SubBoxBackground);
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

	D(("%s",__FUNC__));

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

			if((!(Item->Flags & ITEMENABLED) || SubBoxGhosted || BoxGhosted) && MenuMode != MODE_KEYBOARD && !AktPrefs.mmp_HighlightDisabled)
			{
				D(("->abort"));
				return(TRUE);
			}

			if (Look3D)
			{
				if ((Item->Flags & ITEMENABLED) && (!SubBoxGhosted) && (!BoxGhosted))
				{
					if (LookMC)
					{
						HiRect (SubBoxDrawRPort, l, t, w, h, TRUE, SubBoxBackground);

						DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, TRUE, SubBoxDrawLeft, w);
					}
					else
					{
						if (HiFlags == HIGHIMAGE)
							DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, TRUE, SubBoxDrawLeft, w);
						else
							CompRect (SubBoxDrawRPort, l, t, w, h);
					}
				}

				if (!(Item->Flags & ITEMENABLED) || SubBoxGhosted || BoxGhosted)
				{
					if (!LookMC || MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled)
					{
						if(AktPrefs.mmp_DrawFrames)
							Draw3DRect (SubBoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);

						if (!LookMC)
							GhostRect (SubBoxDrawRPort, l, t, w, h);
					}
				}
				else
				{
					if(AktPrefs.mmp_DrawFrames)
						Draw3DRect (SubBoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
				}
			}
			else
			{
				if (MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled || ((Item->Flags & ITEMENABLED) && (!SubBoxGhosted) && (!BoxGhosted)))
				{
					if (HiFlags == HIGHIMAGE)
						DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, TRUE, SubBoxDrawLeft, w);
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

	D(("%s",__FUNC__));

	if (Item)
	{
		if (!GetSubItemContCoor (Item, &t, &l, &w, &h))
			return (FALSE);
		else
		{
			HiFlags = Item->Flags & HIGHFLAGS;

			if (HiFlags != HIGHCOMP && HiFlags != HIGHBOX && HiFlags != HIGHIMAGE)
				return (TRUE);

			if((!(Item->Flags & ITEMENABLED) || SubBoxGhosted || BoxGhosted) && MenuMode != MODE_KEYBOARD && !AktPrefs.mmp_HighlightDisabled)
			{
				D(("->abort"));
				return(TRUE);
			}

			if (LookMC)
			{
				HiRect (SubBoxDrawRPort, l, t, w, h, FALSE, SubBoxBackground);
				DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE, SubBoxDrawLeft, w);
			}
			else if (Look3D)
			{
				if (!LookMC && HiFlags != HIGHIMAGE && (Item->Flags & ITEMENABLED) && (!SubBoxGhosted) && (!BoxGhosted))
					CompRect (SubBoxDrawRPort, l, t, w, h);

				DrawNormRect (SubBoxDrawRPort, l, t, w, h);
				DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE, SubBoxDrawLeft, w);
			}
			else
			{
				if (HiFlags != HIGHIMAGE)
				{
					if (MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled || ((Item->Flags & ITEMENABLED) && (!SubBoxGhosted) && (!BoxGhosted)))
					{
						if (HiFlags == HIGHBOX)
							CompRect (SubBoxDrawRPort, l - 4, t - 2, w + 8, h + 4);

						CompRect (SubBoxDrawRPort, l, t, w, h);
					}
				}
				DrawMenuItem (SubBoxDrawRPort, Item, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE, SubBoxDrawLeft, w);
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

	FillBackground(RPort, Left, Top, Left + SubBoxWidth - 1, Top + SubBoxHeight - 1,SubBoxBackground);

	SetFont (RPort, MenFont);
	DrawSmooth3DRect (RPort, Left, Top, SubBoxWidth, SubBoxHeight, TRUE, ScrHiRes, DblBorder);

	SubBoxGhosted = BoxGhosted || !(Item->Flags & ITEMENABLED);

	ZwItem = SubBoxItems;
	while (ZwItem)
	{
		DrawMenuItem (RPort, ZwItem, SubBoxLeftOffs + Left, SubBoxTopOffs + Top, SubBoxCmdOffs, SubBoxGhosted, FALSE, 0, SubBoxWidth);
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

	SubNotDrawn = FALSE;

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
		if(CANNOT OpenCommonWindow(SubBoxLeft,SubBoxTop,SubBoxWidth,SubBoxHeight,2,&SubBoxWin,&SubBoxBackground))
		{
			CleanUpMenuSubBox ();
			return (FALSE);
		}

		SubBoxDrawRPort = SubBoxWin->RPort;
		SubBoxDrawLeft = 0;
		SubBoxDrawTop = 0;

		DrawMenuSubBoxContents (Item, SubBoxDrawRPort, SubBoxDrawLeft, SubBoxDrawTop);
	}
	else
	{
		if (!InstallRPort (SubBoxLeft, SubBoxTop, SubBoxDepth, SubBoxWidth, SubBoxHeight, &SubBoxRPort, &SubBoxBitMap, &SubBoxLayerInfo, &SubBoxLayer, &SubBoxCRect,
											&SubBoxBackground, 2))
		{
			CleanUpMenuSubBox ();
			return (FALSE);
		}

		if(SubBoxBackground != NULL)
		{
			SubBoxBackground->bgc_Left = 0;
			SubBoxBackground->bgc_Top = 0;
		}

		DrawMenuSubBoxContents (Item, SubBoxRPort, 0, 0);

		SwapRPortClipRect (&ScrRPort, SubBoxCRect);
		SubBoxDrawRPort = &ScrRPort;
		SubBoxDrawLeft = SubBoxLeft;
		SubBoxDrawTop = SubBoxTop;

		if(SubBoxBackground != NULL)
		{
			SubBoxBackground->bgc_Left = SubBoxDrawLeft;
			SubBoxBackground->bgc_Top = SubBoxDrawTop;
		}
	}

	SetFont (SubBoxDrawRPort, MenFont);
	SetPens (SubBoxDrawRPort, MenTextCol, 0, JAM1);

	MenuSubBoxSwapped = TRUE;

	AktSubItem = NULL;


	if (ActivateItem)
	{
		if (MenuMode == MODE_KEYBOARD)
		{
			if (AktItemRmb->AktSubItem != NULL)
				ChangeAktSubItem (AktItemRmb->AktSubItem, AktItemRmb->AktSubItemNum);
			else
				SelectNextSubItem (-1);
		}

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
			CloseCommonWindow (&BoxWin, &BoxBackground);
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
		FreeRPort (BoxBitMap, BoxLayerInfo, BoxLayer, &BoxBackground);
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

	D(("%s",__FUNC__));

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

			if((!(Item->Flags & ITEMENABLED) || BoxGhosted) && MenuMode != MODE_KEYBOARD && !AktPrefs.mmp_HighlightDisabled)
			{
				D(("->abort"));
				return(TRUE);
			}

			if (Look3D)
			{
				if ((Item->Flags & ITEMENABLED) && (!BoxGhosted))
				{
					if (LookMC)
					{
						HiRect (BoxDrawRPort, l, t, w, h, TRUE, BoxBackground);
						DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, TRUE, BoxDrawLeft, w);
					}
					else
					{
						if (HiFlags == HIGHIMAGE)
							DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, TRUE, BoxDrawLeft, w);
						else
							CompRect (BoxDrawRPort, l, t, w, h);
					}
				}

				if (!(Item->Flags & ITEMENABLED) || BoxGhosted)
				{
					if (!LookMC || MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled)
					{
						if(AktPrefs.mmp_DrawFrames)
							Draw3DRect (BoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
						if (!LookMC)
							GhostRect (BoxDrawRPort, l, t, w, h);
					}
				}
				else
				{
					if(AktPrefs.mmp_DrawFrames)
						Draw3DRect (BoxDrawRPort, l, t, w, h, DblBorder, FALSE, FALSE);
				}
			}
			else
			{
				if (MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled || ((Item->Flags & ITEMENABLED) && (!BoxGhosted)))
				{
					if (HiFlags == HIGHIMAGE)
						DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, TRUE, BoxDrawLeft, w);
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

	D(("%s",__FUNC__));

	if (Item)
	{
		if (!GetItemContCoor (Item, &t, &l, &w, &h))
			return (FALSE);
		else
		{
			HiFlags = Item->Flags & HIGHFLAGS;

			if (HiFlags != HIGHCOMP && HiFlags != HIGHBOX && HiFlags != HIGHIMAGE)
				return (TRUE);

			if((!(Item->Flags & ITEMENABLED) || BoxGhosted) && MenuMode != MODE_KEYBOARD && !AktPrefs.mmp_HighlightDisabled)
			{
				D(("->abort"));
				return(TRUE);
			}

			if (LookMC)
			{
				HiRect (BoxDrawRPort, l, t, w, h, FALSE, BoxBackground);
				DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE, BoxDrawLeft, w);
			}
			else if (Look3D)
			{
				if (HiFlags != HIGHIMAGE && (Item->Flags & ITEMENABLED) && (!BoxGhosted))
					CompRect (BoxDrawRPort, l, t, w, h);
				DrawNormRect (BoxDrawRPort, l, t, w, h);
				DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE, BoxDrawLeft, w);
			}
			else
			{
				if (HiFlags != HIGHIMAGE)
				{
					if (MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled || ((Item->Flags & ITEMENABLED) && (!BoxGhosted)))
					{
						if (HiFlags == HIGHBOX)
							CompRect (BoxDrawRPort, l - 4, t - 2, w + 8, h + 4);
						CompRect (BoxDrawRPort, l, t, w, h);
					}
				}
				DrawMenuItem (BoxDrawRPort, Item, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE, BoxDrawLeft, w);
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

	FillBackground(RPort, Left, Top, Left + BoxWidth - 1, Top + BoxHeight - 1,BoxBackground);

	SetFont (RPort, MenFont);
	DrawSmooth3DRect (RPort, Left, Top, BoxWidth, BoxHeight, TRUE, ScrHiRes, DblBorder);

	SetPens (RPort, MenBackGround, 0, JAM1);

	ZwItem = BoxItems;
	while (ZwItem)
	{
		DrawMenuItem (RPort, ZwItem, BoxLeftOffs + Left, BoxTopOffs + Top, BoxCmdOffs, BoxGhosted, FALSE, 0, BoxWidth);
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
	BOOL NoSubs;

	NoSubs = TRUE;

	for(ZwItem = Menu->FirstItem ; ZwItem ; ZwItem = ZwItem->NextItem)
	{
		if(ZwItem->SubItem)
		{
			NoSubs = FALSE;
			break;
		}
	}

	MenuNotDrawn = FALSE;

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
		if(CANNOT OpenCommonWindow(BoxLeft,BoxTop,BoxWidth,BoxHeight,1,&BoxWin,&BoxBackground))
		{
			CleanUpMenuBox ();
			return (FALSE);
		}

		BoxDrawRPort = BoxWin->RPort;
		BoxDrawLeft = 0;
		BoxDrawTop = 0;

		DrawMenuBoxContents (Menu, BoxDrawRPort, BoxDrawLeft, BoxDrawTop);

		if(!NoSubs)
			CopyBackWindow(BoxWin);
	}
	else
	{
		if (!InstallRPort (BoxLeft, BoxTop, BoxDepth, BoxWidth, BoxHeight, &BoxRPort, &BoxBitMap, &BoxLayerInfo, &BoxLayer, &BoxCRect,
											&BoxBackground, 1))
		{
			CleanUpMenuBox ();
			return (FALSE);
		}

		if(BoxBackground != NULL)
		{
			BoxBackground->bgc_Left = 0;
			BoxBackground->bgc_Top = 0;
		}

		DrawMenuBoxContents (Menu, BoxRPort, 0, 0);

		SwapRPortClipRect (&ScrRPort, BoxCRect);
		BoxDrawRPort = &ScrRPort;
		BoxDrawLeft = BoxLeft;
		BoxDrawTop = BoxTop;

		if(BoxBackground != NULL)
		{
			BoxBackground->bgc_Left = BoxDrawLeft;
			BoxBackground->bgc_Top = BoxDrawTop;
		}
	}

	SetFont (BoxDrawRPort, MenFont);
	SetPens (BoxDrawRPort, MenTextCol, 0, JAM1);

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
	LONG t, l, h, w, offset;

	D(("%s",__FUNC__));

	if (Menu)
	{
		if (!GetMenuContCoor (Menu, &t, &l, &w, &h))
			return (FALSE);
		else
		{
			/* Neu in 2.22: In LoRes werden die Menütitel korrekt positioniert. */
			if (!ScrHiRes && !StripPopUp)
				offset = 2;
			else
				offset = 4;

			t += StripDrawTop;
			l += StripDrawLeft;
			if (MenuMode == MODE_KEYBOARD)
				CheckDispClipVisible (l + (StripLeft - StripDrawLeft), t + (StripTop - StripDrawTop), l + w - 1, t + h - 1);

			if(!(Menu->Flags & MENUENABLED) && MenuMode != MODE_KEYBOARD && !AktPrefs.mmp_HighlightDisabled)
			{
				D(("->abort"));
				return(TRUE);
			}

			if (LookMC)
			{
				SetFont (StripDrawRPort, MenFont);
				SetDrawMode (StripDrawRPort, JAM1);

				if (Menu->Flags & MENUENABLED)
				{
					HiRect (StripDrawRPort, l, t, StripPopUp ? w : w - 1, h + 1, TRUE, StripBackground);
					SetFgPen (StripDrawRPort, MenHiCol);

					PlaceText (StripDrawRPort, l + offset, t + 1 + StripDrawRPort->TxBaseline, Menu->MenuName, -1);
				}
				else
				{
					HiRect (StripDrawRPort, l, t, StripPopUp ? w : w - 1, h + 1, FALSE, StripBackground);

					SetFgPen (StripDrawRPort, MenStdGrey2);

					PlaceText (StripDrawRPort, l + offset+1, t + 1 + StripDrawRPort->TxBaseline + 1, Menu->MenuName, -1);

					SetFgPen (StripDrawRPort, MenStdGrey0);

					PlaceText (StripDrawRPort, l + offset, t + 1 + StripDrawRPort->TxBaseline, Menu->MenuName, -1);
				}

				if (StripPopUp)
				{
					if ((Menu->Flags & MENUENABLED) || MenuMode == MODE_KEYBOARD || AktPrefs.mmp_HighlightDisabled)
					{
						if(AktPrefs.mmp_DrawFrames)
							Draw3DRect (StripDrawRPort, l, t, w, h + 1, AktPrefs.mmp_DblBorder, FALSE, FALSE);
					}
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
				if(!StripPopUp || AktPrefs.mmp_DrawFrames)
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
	LONG t, l, h, w, offset;

	D(("%s",__FUNC__));

	if (Menu)
	{
		GetMenuContCoor (Menu, &t, &l, &w, &h);
		if (t >= 0 && t < StripHeight && l >= 0 && l < StripWidth)
		{
			/* Neu in 2.22: In LoRes werden die Menütitel korrekt positioniert. */
			if (!ScrHiRes && !StripPopUp)
				offset = 2;
			else
				offset = 4;

			t += StripDrawTop;
			l += StripDrawLeft;

			if(!(Menu->Flags & MENUENABLED) && MenuMode != MODE_KEYBOARD && !AktPrefs.mmp_HighlightDisabled)
			{
				D(("->abort"));
				return;
			}

			if (LookMC)
			{
				SetFont (StripDrawRPort, MenFont);
				HiRect (StripDrawRPort, l, t, w, h + 1, FALSE, StripBackground);
				SetDrawMode (StripDrawRPort, JAM1);

				if (Menu->Flags & MENUENABLED)
				{
					SetFgPen (StripDrawRPort, MenTextCol);

					PlaceText (StripDrawRPort, l + offset, t + 1 + StripDrawRPort->TxBaseline, Menu->MenuName, -1);
				}
				else
				{
					SetFgPen (StripDrawRPort, MenStdGrey2);

					PlaceText (StripDrawRPort, l + offset+1, t + 1 + StripDrawRPort->TxBaseline + 1, Menu->MenuName, -1);

					SetFgPen (StripDrawRPort, MenStdGrey0);

					PlaceText (StripDrawRPort, l + offset, t + 1 + StripDrawRPort->TxBaseline, Menu->MenuName, -1);
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

					PlaceText (StripDrawRPort, Menu->LeftEdge + StripLeftOffs + offset, Menu->TopEdge + StripTopOffs + 1 + StripDrawRPort->TxBaseline, Menu->MenuName, -1);

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
			CloseCommonWindow (&StripWin, &StripBackground);
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
		FreeRPort (StripBitMap, StripLayerInfo, StripLayer, &StripBackground);
		StripRPort = NULL;
	}

	AktMenu = NULL;

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
		*y1 = Menu->TopEdge /* + StripTopOffs */ ;  /* StripTopOffs lasse ich weg, besser zu bedienen */

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
		{
			if (AktSubItem)
				AktSubItem->Flags |= HIGHITEM;
			else
			{
				SubItemNum = NOSUB;
				AktSubItem = NULL;
			}
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
		if (!AktPrefs.mmp_NonBlocking)
			CleanUpMenuSubBox (); /* ZZZ */

		DrawNormItem (AktItem);
		AktItem->Flags &= ~HIGHITEM;
		AktItem = NULL;
		ItemNum = NOITEM;

		CleanUpMenuSubBox (); /* ZZZ */
	}

	if (NewItem)
	{
		ItemNum = NewItemNum;

		if (DrawHiItem (AktItem = NewItem))
		{
			AktItem->Flags |= HIGHITEM;
			if (MenuMode != MODE_KEYBOARD)
			{
				if(AktPrefs.mmp_Delayed)
					SubNotDrawn = TRUE;
				else
					DrawMenuSubBox (AktMenu, AktItem, TRUE);
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
		if (!AktPrefs.mmp_NonBlocking)
		{
			CleanUpMenuSubBox (); /* ZZZ */
			CleanUpMenuBox ();
		}

		DrawNormMenu (AktMenu);
		AktMenu = NULL;
		MenuNum = NOMENU;

		CleanUpMenuSubBox (); /* ZZZ */
		CleanUpMenuBox ();
	}

	if (NewMenu)
	{
		MenuNum = NewMenuNum;
		if (DrawHiMenu (AktMenu = NewMenu))
		{
			if (MenuMode != MODE_KEYBOARD)
			{
				if(AktPrefs.mmp_Delayed)
					MenuNotDrawn = TRUE;
				else
					DrawMenuBox (AktMenu, TRUE);
			}
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

	PrevNum = z1 = 0;
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
				if(AktSubItem->Flags & ITEMENABLED)
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
			if (!AktPrefs.mmp_NonBlocking)
				CleanUpMenuSubBox (); /* ZZZ */

			if (AktItem)
			{
				if(AktItem->Flags & ITEMENABLED)
					DrawNormItem (AktItem);

				AktItem->Flags &= ~HIGHITEM;
				AktItem = NULL;
			}

			CleanUpMenuSubBox (); /* ZZZ */

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
		if (!AktPrefs.mmp_NonBlocking)
		{
			CleanUpMenuSubBox (); /* ZZZ */
			CleanUpMenuBox ();
		}

		if (AktMenu)
		{
			if(AktMenu->Flags & MENUENABLED)
				DrawNormMenu (AktMenu);
			AktMenu = NULL;
		}

		CleanUpMenuSubBox (); /* ZZZ */
		CleanUpMenuBox ();

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

	FillBackground(RPort, Left, Top, Left + StripWidth - 1, Top + StripHeight - 1,StripBackground);

	SetFont (RPort, MenFont);

	if (StripPopUp)
		DrawSmooth3DRect (RPort, Left, Top, StripWidth, StripHeight, TRUE, ScrHiRes, DblBorder);
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

			/* Neu in 2.22: die Titel für LoRes-Menüs werden richtig
			 *              positioniert.
			 */
			if (!ScrHiRes)
				X -= 2;
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

			PlaceText (RPort, X + 1, Y + 1, ZwMenu->MenuName, -1);

			SetFgPen (RPort, MenStdGrey0);
		}
		else
			SetFgPen (RPort, MenTextCol);

		PlaceText (RPort, X, Y, ZwMenu->MenuName, -1);

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

	CopyMem(StdRemapArray,DreiDRemapArray,sizeof(StdRemapArray));
	CopyMem(StdRemapArray,DreiDGhostedRemapArray,sizeof(StdRemapArray));
	CopyMem(StdRemapArray,DreiDActiveRemapArray,sizeof(StdRemapArray));

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
/*  UWORD CheckWidth,CommandWidth;*/

	IsBlocking = !AktPrefs.mmp_NonBlocking;
	ThisTask = FindTask(NULL);

	memset(StdRemapArray,0,sizeof(StdRemapArray));

	ResetMenuColours();

	MenuStripSwapped = FALSE;

	ScrRPort = MenScr->RastPort;
	SetDrawMask (&ScrRPort, ~0);

	SetDrawMode (&ScrRPort, JAM1);

	SetAfPt (&ScrRPort, NULL, 0);

	StripDepth = GetBitMapDepth (MenScr->RastPort.BitMap);
	ScrHiRes = (MenScr->Flags & SCREENHIRES) != NULL;

	if(!(DrawInfo = GetScreenDrawInfo (MenScr)))
		return(FALSE);

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

	/* Jetzt wird die Größe der vorgegebenen Bilder ermittelt. */
/*
	if(ScrHiRes)
	{
		CheckWidth = CHECKWIDTH;
		CommandWidth = COMMWIDTH;
	}
	else
	{
		CheckWidth = LOWCHECKWIDTH;
		CommandWidth = LOWCOMMWIDTH;
	}

	ObtainGlyphs (MenWin, &CheckImage, &CommandImage);

	if(CheckImage)
	{
		CheckWidth = CheckImage->Width;
		CheckImage = NULL;
	}

	if(CommandImage)
	{
		CommandWidth = CommandImage->Width;
		CommandImage = NULL;
	}
*/
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

		if(AktPrefs.mmp_PreferScreenColours)
		{
			STATIC UWORD SixPens[] = { SHINEPEN, SHADOWPEN, BACKGROUNDPEN, TEXTPEN, FILLTEXTPEN, FILLPEN};
			ULONG Six[6][3];
			LONG i;

			for(i = 0 ; i < 6 ; i++)
				GetRGB32(ColorMap,DrawInfo->dri_Pens[SixPens[i]],1,&Six[i][0]);

			if(StripDepth > 3)
			{
				red = (Six[0][0] >> 24) + (Six[1][0] >> 24);
				green = (Six[0][1] >> 24) + (Six[1][1] >> 24);
				blue = (Six[0][2] >> 24) + (Six[1][2] >> 24);

				AddMenuColour8 ((red + 1) / 2, (green + 1) / 2, (blue + 1) / 2);  // SectGrey

				red = Six[2][0] >> 24;
				green = Six[2][1] >> 24;
				blue = Six[2][2] >> 24;

				AddMenuColour8 (red - 26, green - 26, blue - 26);  // StdGrey0
				AddMenuColour8 (red, green, blue);  // StdGrey1
				AddMenuColour8 (red + 26, green + 26, blue + 26);  // StdGrey2

				red = Six[5][0] >> 24;
				green = Six[5][1] >> 24;
				blue = Six[5][2] >> 24;

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

			for(i = 0 ; i < 6 ; i++)
				AddMenuColour (Six[i][0],Six[i][1],Six[i][2]);
		}
		else
		{
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
		}

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
			MenFillCol = *Pen;

#ifdef DEMO_MENU
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
#endif	/* DEMO_MENU */

			MenSeparatorGrey0 = MenStdGrey0;
			MenSeparatorGrey2 = MenStdGrey2;

			if(MenSeparatorGrey0 == MenBackGround || MenSeparatorGrey2 == MenBackGround)
			{
				MenSeparatorGrey0 = MenDarkEdge;
				MenSeparatorGrey2 = MenLightEdge;
			}

/*    LookMC = (MenTextCol != MenBackGround && MenBackGround != MenFillCol && MenHiCol != MenFillCol && MenStdGrey0 != MenStdGrey2);*/
			/* 2.23: Eigentlich sollte es reichen, wenn man den Text lesen kann und
			 *       sieht, welchen Eintrag man gerade am Wickel hat.
			 */
			LookMC = (MenTextCol != MenBackGround && MenTextCol != MenFillCol && MenHiCol != MenFillCol && MenStdGrey0 != MenBackGround);

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

			CopyMem(StdRemapArray,DreiDRemapArray,sizeof(StdRemapArray));

			DreiDRemapArray[MenMenuTextCol] = MenTextCol;
			DreiDRemapArray[MenMenuBackCol] = MenBackGround;
		}

		ObtainGlyphs (MenWin, &CheckImage, &CommandImage);

		if (CheckImage)
		{
			MakeRemappedImage (&Ok, &CheckImageRmp, CheckImage, StripDepth, DreiDRemapArray);
			NoMXImage = &NoCheckImage2Pl;
			CopyImageDimensions (NoMXImage, CheckImage);

			if (LookMC)
			{
				MakeRemappedImage (&Ok, &CheckImageGhosted, CheckImage, StripDepth, DreiDGhostedRemapArray);
				MakeRemappedImage (&Ok, &CheckImageActive, CheckImage, StripDepth, DreiDActiveRemapArray);
			}

			CheckImage = CheckImageRmp;
			MXImage = CheckImage;

			DoFlipMX = FALSE;
			DoFlipCheck = FALSE;
		}

		if (CommandImage)
		{
			DoFlipCommand = FALSE;
			MakeRemappedImage (&Ok, &CommandImageRmp, CommandImage, StripDepth, DreiDRemapArray);
			if (LookMC)
			{
				MakeRemappedImage (&Ok, &CommandImageGhosted, CommandImage, StripDepth, DreiDGhostedRemapArray);
				MakeRemappedImage (&Ok, &CommandImageActive, CommandImage, StripDepth, DreiDActiveRemapArray);
			}

			CommandImage = CommandImageRmp;
		}

		if (LookMC)
		{
			BOOL SpecialCheckImage = (CheckImage != NULL);

			SetMCPens(FALSE);

			/* Warum? */
			/*Ok = TRUE;*/

			if (MenFont->tf_YSize >= 11)
			{
				if (!CommandImage)
				{
					MakeRemappedImage (&Ok, &CommandImageRmp, &AmigaNormal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CommandImageGhosted, &AmigaGhosted, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CommandImageActive, &AmigaNormal, MaxMapDepth, DreiDActiveRemapArray);

					CommandImage = CommandImageRmp;
				}

				if (!CheckImage)
				{
					MakeRemappedImage (&Ok, &CheckImageRmp, &CheckNormal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CheckImageGhosted, &CheckGhosted, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CheckImageActive, &CheckNormal, MaxMapDepth, DreiDActiveRemapArray);

					CheckImage = CheckImageRmp;
				}

				NoCheckImage = &NoCheckImage2Pl;

				NoCheckImage->PlaneOnOff = MenBackGround;
				NoCheckImage->NextImage = (struct Image *)IMAGE_MAGIC;
				CopyImageDimensions (NoCheckImage, CheckImage);

				MakeRemappedImage (&Ok, &SubArrowImageRmp, &ArrowNormal, MaxMapDepth, StdRemapArray);
				MakeRemappedImage (&Ok, &SubArrowImageGhosted, &ArrowGhosted, MaxMapDepth, StdRemapArray);
				MakeRemappedImage (&Ok, &SubArrowImageActive, &ArrowNormal, MaxMapDepth, DreiDActiveRemapArray);

				SubArrowImage = SubArrowImageRmp;

				if (!SpecialCheckImage)
				{
					MakeRemappedImage (&Ok, &MXImageRmp, &MXDownNormal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &NoMXImageRmp, &MXUpNormal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &MXImageGhosted, &MXDownGhosted, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &NoMXImageGhosted, &MXUpGhosted, MaxMapDepth, StdRemapArray);

					MXImage = MXImageRmp;
					NoMXImage = NoMXImageRmp;

					MakeRemappedImage (&Ok, &MXImageActive, &MXDownNormal, MaxMapDepth, DreiDActiveRemapArray);
					MakeRemappedImage (&Ok, &NoMXImageActive, &MXUpNormal, MaxMapDepth, DreiDActiveRemapArray);

					DoFlipMX = FALSE;
				}
			}
			else
			{
				if (!CommandImage)
				{
					MakeRemappedImage (&Ok, &CommandImageRmp, &Amiga8_Normal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CommandImageGhosted, &Amiga8_Ghosted, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CommandImageActive, &Amiga8_Normal, MaxMapDepth, DreiDActiveRemapArray);

					CommandImage = CommandImageRmp;
				}

				if (!CheckImage)
				{
					MakeRemappedImage (&Ok, &CheckImageRmp, &Check8_Normal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CheckImageGhosted, &Check8_Ghosted, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &CheckImageActive, &Check8_Normal, MaxMapDepth, DreiDActiveRemapArray);

					CheckImage = CheckImageRmp;
				}

				NoCheckImage = &NoCheckImage2Pl;

				NoCheckImage->PlaneOnOff = MenBackGround;
				NoCheckImage->NextImage = (struct Image *)IMAGE_MAGIC;

				CopyImageDimensions (NoCheckImage, CheckImage);

				MakeRemappedImage (&Ok, &SubArrowImageRmp, &Arrow8_Normal, MaxMapDepth, StdRemapArray);
				MakeRemappedImage (&Ok, &SubArrowImageGhosted, &Arrow8_Ghosted, MaxMapDepth, StdRemapArray);
				MakeRemappedImage (&Ok, &SubArrowImageActive, &Arrow8_Normal, MaxMapDepth, DreiDActiveRemapArray);

				SubArrowImage = SubArrowImageRmp;

				if (!SpecialCheckImage)
				{
					MakeRemappedImage (&Ok, &MXImageRmp, &MXDown8_Normal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &NoMXImageRmp, &MXUp8_Normal, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &MXImageGhosted, &MXDown8_Ghosted, MaxMapDepth, StdRemapArray);
					MakeRemappedImage (&Ok, &NoMXImageGhosted, &MXUp8_Ghosted, MaxMapDepth, StdRemapArray);

					MXImage = MXImageRmp;
					NoMXImage = NoMXImageRmp;

					MakeRemappedImage (&Ok, &MXImageActive, &MXDown8_Normal, MaxMapDepth, DreiDActiveRemapArray);
					MakeRemappedImage (&Ok, &NoMXImageActive, &MXUp8_Normal, MaxMapDepth, DreiDActiveRemapArray);

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

				if (MenFont->tf_YSize >= 11)
				{
					if (!CheckImage)
					{
						CheckImage = &CheckNormal4;
						DoFlipCheck = TRUE;
					}

					if (!MXImage)
					{
						MXImage = &MXDownNormal4;
						NoMXImage = &MXUpNormal4;
						DoFlipMX = TRUE;
					}

					if (!CommandImage)
					{
						CommandImage = &AmigaNormal4;
						DoFlipCommand = TRUE;
					}

					SubArrowImage = &ArrowNormal4;
				}
				else
				{
					if (!CheckImage)
					{
						CheckImage = &CheckNormal8_4;
						DoFlipCheck = TRUE;
					}

					if (!MXImage)
					{
						MXImage = &MXDownNormal8_4;
						NoMXImage = &MXUpNormal8_4;
						DoFlipMX = TRUE;
					}

					if (!CommandImage)
					{
						CommandImage = &AmigaNormal8_4;
						DoFlipCommand = TRUE;
					}

					SubArrowImage = &ArrowNormal8_4;
				}
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
		NoCheckImage->NextImage = (struct Image *)IMAGE_MAGIC;
		NoMXImage->NextImage = (struct Image *)IMAGE_MAGIC;

		if (Ok && DoFlipMX)
		{
			MakeRemappedImage (&Ok, &MXImageRmp, MXImage, StripDepth, StdRemapArray);
			if (Ok && NoMXImage)
			{
				MakeRemappedImage (&Ok, &NoMXImageRmp, NoMXImage, StripDepth, StdRemapArray);
				if (NOT Ok)
					FreeRemappedImage (MXImage);
			}

			MXImage = MXImageRmp;
			NoMXImage = NoMXImageRmp;
		}

		if (Ok && DoFlipCommand)
		{
			MakeRemappedImage (&Ok, &CommandImageRmp, CommandImage, StripDepth, StdRemapArray);
			CommandImage = CommandImageRmp;
		}

		if (Ok && DoFlipCheck)
		{
			MakeRemappedImage (&Ok, &CheckImageRmp, CheckImage, StripDepth, StdRemapArray);
			CheckImage = CheckImageRmp;
		}

		if (Ok && DoFlipSubArrow)
		{
			MakeRemappedImage (&Ok, &SubArrowImageRmp, SubArrowImage, StripDepth, StdRemapArray);
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
		LONG height;

		/* Neu in 2.16: Höhe der Menüs in der Bildschirmtitelzeile
		 *              wird aus der Höhe der Screen-Titelzeile
		 *              "errechnet".
		 */
		height = MenScr->BarHeight+1;

		StripPopUp = FALSE;
		StripHeight = height;
		StripWidth = MenScr->Width;
		StripTopOffs = 0;
		StripLeftOffs = 0;
		StripMinHeight = height-1;
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
		if(CANNOT OpenCommonWindow(StripLeft,StripTop,StripWidth,StripHeight,0,&StripWin,&StripBackground))
		{
			CleanUpMenuStrip ();
			return (FALSE);
		}

		StripDrawRPort = StripWin->RPort;
		StripDrawLeft = 0;
		StripDrawTop = 0;

		DrawMenuStripContents (StripDrawRPort, StripDrawLeft, StripDrawTop);

		CopyBackWindow(StripWin);
	}
	else
	{
		if (!InstallRPort (StripLeft, StripTop, StripDepth, StripWidth, StripHeight, &StripRPort, &StripBitMap, &StripLayerInfo, &StripLayer, &StripCRect,
											&StripBackground, 0))
		{
			CleanUpMenuStrip ();
			return (FALSE);
		}

		if(StripBackground != NULL)
		{
			StripBackground->bgc_Left = 0;
			StripBackground->bgc_Top = 0;
		}

		DrawMenuStripContents (StripRPort, 0, 0);

		SwapRPortClipRect (&ScrRPort, StripCRect);
		StripDrawRPort = &ScrRPort;
		StripDrawLeft = StripLeft;
		StripDrawTop = StripTop;

		if(StripBackground != NULL)
		{
			StripBackground->bgc_Left = StripLeft;
			StripBackground->bgc_Top = StripTop;
		}
	}

	SetFont (StripDrawRPort, MenFont);
	SetPens (StripDrawRPort, MenTextCol, 0, JAM1);

	AktMenu = NULL;
	MenuStripSwapped = TRUE;

	if (ActivateMenu)
	{
		if (MenuMode == MODE_KEYBOARD)
		{
			if (AktMenuRemember->AktMenu != NULL)
				ChangeAktMenu (AktMenuRemember->AktMenu, AktMenuRemember->AktMenuNum);
			else
				SelectNextMenu (-1);
		}

		if (MenuMode != MODE_KEYBOARD && (MenuMode != MODE_SELECT || SelectSpecial))
			LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
	}

	return (TRUE);
}

VOID
Shoot()
{
	if(MenuNotDrawn)
	{
		if(AktMenu)
			DrawMenuBox (AktMenu, TRUE);
		else
			MenuNotDrawn = FALSE;
	}

	if(SubNotDrawn)
	{
		if(AktMenu && AktItem)
			DrawMenuSubBox (AktMenu, AktItem, TRUE);
		else
			SubNotDrawn = FALSE;
	}
}
