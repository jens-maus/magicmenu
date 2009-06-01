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

#include <exec/types.h>
#include <stddef.h>

/******************************************************************************/

enum
{
	SIT_BYTE,
	SIT_UBYTE,
	SIT_WORD,
	SIT_BOOL,
	SIT_BOOLEAN,
	SIT_UWORD,
	SIT_LONG,
	SIT_ULONG,
	SIT_STRPTR,
	SIT_TEXT
};

struct StorageItem
{
	ULONG	si_Offset;
	LONG	si_Size;
	WORD	si_Type;
	WORD	si_Found;
	STRPTR	si_Name;
};

#define DECLARE_ITEM(s,e,t,n)	offsetof(struct s,e),sizeof(((struct s *)0)->e),t,0,n
#define ITEM_TABLE_SIZE(t)	(sizeof(t)/sizeof(t[0]))

/******************************************************************************/

LONG RestoreData(STRPTR Name,STRPTR Type,LONG Version,struct StorageItem *Items,LONG NumItems,APTR DataPtr,LONG * NumItemsPtr);
LONG StoreData(STRPTR Name,STRPTR Type,LONG Version,struct StorageItem *Items,LONG NumItems,APTR DataPtr);
