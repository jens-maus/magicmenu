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

/* Magic Menu Prefs Header */

typedef unsigned char BOOLEAN;

struct RGBSet
{
  ULONG R, G, B;
};

struct MMPrefs
{
  ULONG mmp_Magic;
  LONG mmp_Version;

  UBYTE mmp_MenuType;

  BOOLEAN mmp_Enabled;
  BOOLEAN mmp_MarkSub;
  BOOLEAN mmp_DblBorder;
  BOOLEAN mmp_NonBlocking;
  BOOLEAN mmp_KCEnabled;
  BOOLEAN mmp_KCGoTop;
  BOOLEAN mmp_KCRAltRCommand;
  BOOLEAN mmp_PUCenter;
  BOOLEAN mmp_PreferScreenColours;
  BOOLEAN mmp_Delayed;
  BOOLEAN mmp_DrawFrames;
  BOOLEAN mmp_CastShadows;

  UBYTE mmp_PDMode;
  UBYTE mmp_PDLook;

  UBYTE mmp_PUMode;
  UBYTE mmp_PULook;

  char mmp_KCKeyStr[200];

  WORD mmp_Precision;

  struct RGBSet mmp_LightEdge;
  struct RGBSet mmp_DarkEdge;
  struct RGBSet mmp_Background;
  struct RGBSet mmp_TextCol;
  struct RGBSet mmp_HiCol;
  struct RGBSet mmp_FillCol;

  BOOLEAN mmp_Transparency;
  BOOLEAN mmp_HighlightDisabled;
  UBYTE mmp_SeparatorBarStyle;
  BOOLEAN mmp_VerifyPatches;
  BOOLEAN mmp_FixPatches;
  
  char mmp_BackFill[256];
  
  BOOLEAN mmp_PDTransparent;
  BOOLEAN mmp_TransHighlight;
  BOOLEAN mmp_TransBackfill;
};

enum
{
  MT_PULLDOWN,
  MT_POPUP,
  MT_AUTO
};

enum
{
  LOOK_2D,
  LOOK_3D,
  LOOK_MC
};

enum
{
  MODE_STD,
  MODE_STICKY,
  MODE_SELECT,
  MODE_KEYBOARD
};

#define CONFIG_NAME "MagicMenu.config"
#define PATHENV "MagicMenuPath"

struct MMMessage
{
  struct Message Message;

  ULONG Class;

  ULONG Arg1;
  ULONG Arg2;
  ULONG Arg3;

  APTR Ptr1;
  APTR Ptr2;
  APTR Ptr3;
};

enum
{
  MMC_VOID,
  MMC_NEWCONFIG,
  MMC_REMOVE,
  MMC_GETCONFIG,
  MMC_ENABLE
};

#define MMPORT_NAME "MagicMenu"

#define MMPREFS_MAGIC 20041970
#define MMPREFS_VERSION 2
