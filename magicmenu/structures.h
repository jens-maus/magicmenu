/*
**   $Id$
**
**   :ts=8
*/

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
