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

struct MenuRemember
{
  struct Window *MenWindow;
  struct Menu *MenuStrip;

  struct Menu *AktMenu;

  struct MenuRmb *FirstMenu;

  struct MenuRemember *NextRemember;

  ULONG AktMenuNum;
};

struct MenuRmb
{
  struct Menu *Menu;

  struct MenuItem *AktItem;

  struct ItemRmb *FirstItem;

  struct MenuRmb *NextMenu;

  ULONG AktItemNum;

  WORD Width, Height, TopOffs, LeftOffs, ZwLeft, ZwTop;
  WORD LeftBorder, TopBorder, CmdOffs;
};

struct ItemRmb
{
  struct MenuItem *Item;

  struct MenuItem *AktSubItem;

  struct ItemRmb *NextItem;

  ULONG AktSubItemNum;

  WORD Width, Height, TopOffs, LeftOffs, ZwLeft, ZwTop;
  WORD LeftBorder, TopBorder, CmdOffs;
};

/******************************************************************************/

struct BackgroundCover
{
	UWORD		bgc_Left;
	UWORD		bgc_Top;
	UWORD		bgc_Width;
	UWORD		bgc_Height;
	struct BitMap *	bgc_BitMap;
};
