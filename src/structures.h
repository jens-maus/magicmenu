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
