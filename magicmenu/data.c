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

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

struct Task *ThisTask;

struct Library *CyberGfxBase;
struct Library *CxBase;
struct Library *IconBase;
struct Library *LayersBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *TimerBase;
struct Library *InputBase;
struct Library *KeymapBase;
struct Library *UtilityBase;

struct Library *LocaleBase;
struct Catalog *Catalog;

BOOL V39;

struct MsgPort *TimerPort;
struct timerequest *TimerIO;

struct MsgPort *InputPort;
struct IOStdReq *InputIO;

BOOL MagicActive;

CxObj *Broker;
CxObj *KbdFilter;
CxObj *StdKbdFilter;
CxObj *StdKbdSender;
CxObj *StdKbdTransl;
CxObj *PrefsFilter;
CxObj *MouseFilter;
CxObj *MouseSender;
CxObj *MouseTransl;
CxObj *ActKbdFilter;
CxObj *ActKbdSender;
CxObj *ActKbdTransl;
CxObj *MouseMoveFilter;
CxObj *MouseMoveSender;
CxObj *MousePositionFilter;
CxObj *MousePositionSender;
CxObj *MouseNewPositionFilter;
CxObj *MouseNewPositionSender;
CxObj *TickFilter;
CxObj *TickSignal;
struct MsgPort *CxMsgPort;
struct MsgPort *ReplyPort;
struct MsgPort *MMMsgPort;
LONG TickSigNum = -1;
ULONG TickSigMask;

struct MsgPort *IMsgReplyPort;
ULONG IMsgReplyCount;

struct MsgPort *TimeoutPort;
struct timerequest *TimeoutRequest;

struct SignalSemaphore *GetPointerSemaphore;
struct SignalSemaphore *MenuActSemaphore;
struct SignalSemaphore *RememberSemaphore;

BOOL LayersLocked;
BOOL CxChanged;
BOOL LibPatches;


struct TextFont *MenFont;
struct TextFont *MenOpenedFont;

struct MenuItem *FirstSelectedMenu;
struct MenuItem *LastSelectedMenu;
UWORD FirstMenuNum;
UWORD MenuNum;
UWORD SubItemNum;
UWORD ItemNum;

UWORD MenLightEdge;
UWORD MenDarkEdge;
UWORD MenBackGround;
UWORD MenTextCol;
UWORD MenHiCol;
UWORD MenFillCol;
UWORD MenBlockPen;
UWORD MenDetailPen;
UWORD MenTrimPen;
UWORD MenComplMsk;
UWORD MenMenuTextCol;
UWORD MenMenuBackCol;
UWORD MenGhostLoCol;
UWORD MenGhostHiCol;
UWORD MenXENGrey0;
UWORD MenXENBlack;
UWORD MenXENWhite;
UWORD MenXENBlue;
UWORD MenXENGrey1;
UWORD MenXENGrey2;
UWORD MenXENBeige;
UWORD MenXENPink;
UWORD MenSectGrey;
UWORD MenStdGrey0;
UWORD MenStdGrey1;
UWORD MenStdGrey2;
UWORD MenActiveGrey0;
UWORD MenActiveGrey1;
UWORD MenActiveGrey2;

UWORD MenSeparatorGrey0;
UWORD MenSeparatorGrey2;

UBYTE SelBoxOffs;
BOOL Look3D;
BOOL LookMC;
BOOL ScrHiRes;
BOOL HelpPressed;
BOOL DblBorder;

struct MenuRemember *GlobalRemember;
struct MenuRemember *AktMenuRemember;
struct MenuRmb *AktMenRmb;
struct ItemRmb *AktItemRmb;

UWORD StripWidth;
UWORD StripHeight;
UWORD StripDepth;
UWORD StripTopOffs;
WORD StripLeftOffs;	/* Ab 2.22 ohne Vorzeichen! */
UWORD StripMinHeight;
UWORD StripMinWidth;
UWORD StripLeft;
UWORD StripTop;
UWORD StripDrawLeft;
UWORD StripDrawTop;
UWORD StripDrawOffs;
struct RastPort *StripRPort, *StripDrawRPort;
struct BitMap *StripBitMap;
struct Layer_Info *StripLayerInfo;
struct Layer *StripLayer;
struct ClipRect *StripCRect;
struct Window *StripWin;
struct BackgroundCover *StripBackground;
BOOL StripPopUp;
BOOL MenuStripSwapped;

UWORD BoxWidth;
UWORD BoxHeight;
WORD BoxTop;
WORD BoxLeft;
WORD BoxDrawTop;
WORD BoxDrawLeft;
UWORD BoxDepth;
WORD BoxTopOffs;
WORD BoxLeftOffs;
WORD BoxLeftBorder;
WORD BoxTopBorder;
UWORD BoxDrawOffs;
UWORD BoxRightEdge;
UWORD BoxCmdOffs;
BOOL BoxGhosted;
struct RastPort *BoxRPort, *BoxDrawRPort;
struct BitMap *BoxBitMap;
struct Layer_Info *BoxLayerInfo;
struct Layer *BoxLayer;
struct ClipRect *BoxCRect;
struct MenuItem *BoxItems;
struct Window *BoxWin;
struct BackgroundCover *BoxBackground;
BOOL MenuBoxSwapped;

UWORD SubBoxWidth;
UWORD SubBoxHeight;
WORD SubBoxTop;
WORD SubBoxLeft;
WORD SubBoxDrawTop;
WORD SubBoxDrawLeft;
UWORD SubBoxDepth;
WORD SubBoxTopOffs;
WORD SubBoxLeftOffs;
WORD SubBoxLeftBorder;
WORD SubBoxTopBorder;
UWORD SubBoxDrawOffs;
UWORD SubBoxRightEdge;
UWORD SubBoxCmdOffs;
BOOL SubBoxGhosted;
struct RastPort *SubBoxRPort, *SubBoxDrawRPort;
struct BitMap *SubBoxBitMap;
struct Layer_Info *SubBoxLayerInfo;
struct Layer *SubBoxLayer;
struct ClipRect *SubBoxCRect;
struct MenuItem *SubBoxItems;
struct Window *SubBoxWin;
struct BackgroundCover *SubBoxBackground;
BOOL MenuSubBoxSwapped;

struct Menu *AktMenu;
struct MenuItem *AktItem;
struct MenuItem *AktSubItem;

ULONG DMROldSecs;
ULONG DMROldMics;

BOOL GlobalDMRequest;
BOOL GlobalLastWinDMREnable;
WORD GlobalDMRCount;

/* ************************************************************************ */
/* ************************************************************************ */
/*/
/*             Vorbesetzte Strukturen                                       */
/*/
/* ************************************************************************ */
/* ************************************************************************ */

struct MMPrefs DefaultPrefs =
{
	MMPREFS_MAGIC,				/* Magic */
	MMPREFS_VERSION,			/* Version */
	MT_AUTO,				/* MenuType */
	TRUE,					/* Enabled */
	TRUE,					/* MarkSub */
	FALSE,					/* DblBorder */
	TRUE,					/* NonBlocking */
	TRUE,					/* KCEnabled */
	TRUE,					/* KCGoTop */
	TRUE,					/* KCRAltRCommand */
	TRUE,					/* PUCenter */
	FALSE,					/* PreferScreenColours */
	FALSE,					/* Delayed */
	TRUE,					/* DrawFrame */
	TRUE,					/* CastShadows */

	MODE_STD,				/* PDMode */
	LOOK_MC,				/* PDLook */
	MODE_STD,				/* PUMode */
	LOOK_MC,				/* PULook */

	"ramiga space",				/* KCKeyStr */

	PRECISION_GUI,				/* Precision */

	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,	/* LightEdge */
	0x00000000, 0x00000000, 0x00000000,	/* DarkEdge */
	0x95959595, 0x95959595, 0x95959595,	/* Background */
	0x00000000, 0x00000000, 0x00000000,	/* TextCol */
	0x00000000, 0x00000000, 0x00000000,	/* HiCol */
	0x3B3B3B3B, 0x67676767, 0xa3a3a3a3,	/* FillCol */
};


char CommText[5] = "";
struct IntuiText CommandText =
{0, 0, JAM2, 0, 1, NULL, (UBYTE *) CommText, NULL};

struct NewBroker NewBroker =
{
  NB_VERSION,
  MAGIC_MENU_NAME,
  NULL,
  MAGIC_MENU_DESCR,
  NBU_UNIQUE | NBU_NOTIFY,
  COF_SHOW_HIDE,		/* Kein COF_SHOW_HIDE */
  0
};

struct InputXpression TickIX =
{
  IX_VERSION,
  IECLASS_TIMER,
  0, 0,
  0, 0,
  0
};

struct InputXpression StdKbdIX =
{
  IX_VERSION,
  IECLASS_RAWKEY,
  0, IECODE_UP_PREFIX,
  (IEQUALIFIER_RALT | IEQUALIFIER_RCOMMAND),
  0xffff & ~(IEQUALIFIER_RELATIVEMOUSE),
  0
};

struct InputXpression MouseIX =
{
  IX_VERSION,
  IECLASS_RAWMOUSE,
  IECODE_RBUTTON, 0xff,
  0, (IEQUALIFIER_CONTROL | IEQUALIFIER_LALT | IEQUALIFIER_RALT |
      IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND | IEQUALIFIER_LEFTBUTTON | IEQUALIFIER_MIDBUTTON),
  0
};

struct InputXpression ActiveMouseIX =
{
  IX_VERSION,
  IECLASS_RAWMOUSE,
  0, 0xff & ~(IECODE_RBUTTON | IECODE_LBUTTON | IECODE_MBUTTON | IECODE_UP_PREFIX),
  0, 0, 0
};

struct InputXpression ActiveMouseMoveIX =
{
  IX_VERSION,
  IECLASS_RAWMOUSE,
  0, 0,
  0, 0, 0
};

struct InputXpression ActiveMousePositionIX =
{
  IX_VERSION,
  IECLASS_POINTERPOS,
  0, 0,
  0, 0, 0
};

struct InputXpression ActiveMouseNewPositionIX =
{
  IX_VERSION,
  IECLASS_NEWPOINTERPOS,
  0, 0,
  0, 0, 0
};

struct InputXpression ActiveKbdIX =
{
  IX_VERSION,
  IECLASS_RAWKEY,
  0, IECODE_UP_PREFIX,
  0, 0, 0
};


UWORD __chip MXImage1PlPattern[] =
{0x3ffe, 0x0000, 0x6003, 0x0000, 0xc7f1, 0x8000, 0xcff9, 0x8000, 0xcff9, 0x8000,
 0xcff9, 0x8000, 0xc7f1, 0x8000, 0x6003, 0x0000, 0x3ffe, 0x0000};

struct Image MXImage1Pl =
{1, -1, 17, 9, 1, MXImage1PlPattern, 1, 0, NULL};


UWORD __chip NoMXImage1PlPattern[] =
{0x3ffe, 0x0000, 0x6003, 0x0000, 0xc001, 0x8000, 0xc001, 0x8000, 0xc001, 0x8000,
 0xc001, 0x8000, 0xc001, 0x8000, 0x6003, 0x0000, 0x3ffe, 0x0000};

struct Image NoMXImage1Pl =
{1, -1, 17, 9, 1, NoMXImage1PlPattern, 1, 0, NULL};


UWORD __chip MXImageLow1PlPattern[] =
{0x7fc0, 0xc060, 0x9f20, 0xbfa0, 0xbfa0, 0xbfa0, 0x9f20, 0xc060, 0x7fc0};

struct Image MXImageLow1Pl =
{1, -1, 11, 9, 1, MXImageLow1PlPattern, 1, 0, NULL};


UWORD __chip NoMXImageLow1PlPattern[] =
{0x7fc0, 0xc060, 0x8020, 0x8020, 0x8020, 0x8020, 0x8020, 0xc060, 0x7fc0};

struct Image NoMXImageLow1Pl =
{1, -1, 11, 9, 1, NoMXImageLow1PlPattern, 1, 0, NULL};


UWORD __chip MXImage2PlPattern[] =
{0x3ffe, 0x0000, 0x6000, 0x0000, 0xc7f0, 0x0000, 0xcff8, 0x0000, 0xcff8, 0x0000,
 0xcff8, 0x0000, 0xc7f0, 0x0000, 0x6000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0003, 0x0000, 0x07f1, 0x8000, 0x0ff9, 0x8000, 0x0ff9, 0x8000, 0x0ff9, 0x8000,
 0x07f1, 0x8000, 0x0003, 0x0000, 0x3ffe, 0x0000};

struct Image MXImage2Pl =
{1, -1, 17, 9, 2, MXImage2PlPattern, 3, 0, NULL};


UWORD __chip NoMXImage2PlPattern[] =
{0x0000, 0x0000, 0x0003, 0x0000, 0x0001, 0x8000, 0x0001, 0x8000, 0x0001, 0x8000,
 0x0001, 0x8000, 0x0001, 0x8000, 0x0003, 0x0000, 0x3ffe, 0x0000, 0x3ffe, 0x0000,
 0x6000, 0x0000, 0xc000, 0x0000, 0xc000, 0x0000, 0xc000, 0x0000, 0xc000, 0x0000,
 0xc000, 0x0000, 0x6000, 0x0000, 0x0000, 0x0000};

struct Image NoMXImage2Pl =
{1, -1, 17, 9, 2, NoMXImage2PlPattern, 3, 0, NULL};


UWORD __chip MXImageLow2PlPattern[] =
{0x7fc0, 0xc000, 0x9f00, 0xbf80, 0xbf80, 0xbf80, 0x9f00, 0xc000, 0x0000, 0x0000,
 0x0060, 0x1f20, 0x3fa0, 0x3fa0, 0x3fa0, 0x1f20, 0x0060, 0x7fc0};

struct Image MXImageLow2Pl =
{1, -1, 11, 9, 2, MXImageLow2PlPattern, 3, 0, NULL};


UWORD __chip NoMXImageLow2PlPattern[] =
{0x0000, 0x0060, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0060, 0x7fc0, 0x7fc0,
 0xc000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xc000, 0x0000};

struct Image NoMXImageLow2Pl =
{1, -1, 11, 9, 2, NoMXImageLow2PlPattern, 3, 0, NULL};


UWORD __chip CheckImage2PlPattern[] =
{0x0000, 0x0000, 0x0007, 0x8000, 0x000F, 0x8000, 0x001E, 0x0000, 0x003C, 0x0000, 0x7878,
 0x0000, 0x3CF0, 0x0000, 0x0FE0, 0x0000, 0x07C0, 0x0000, 0x0380, 0x0000,
 0x000F, 0x0000, 0x001F, 0x0000, 0x003C, 0x0000, 0x0078, 0x0000, 0xF0F0, 0x0000, 0xF9E0,
 0x0000, 0x1FC0, 0x0000, 0x0F80, 0x0000, 0x0700, 0x0000, 0x0000, 0x0000};

struct Image CheckImage2Pl =
{0, -1, 17, 10, 2, CheckImage2PlPattern, 3, 0, NULL};





UWORD __chip CheckImageLow2PlPattern[] =
{0x0000, 0x0038, 0x0078, 0x00F0, 0x01E0, 0x63C0, 0x3780, 0x1F00, 0x0E00, 0x0400,
 0x0070, 0x00F0, 0x01E0, 0x03C0, 0xC780, 0xEF00, 0x3E00, 0x1C00, 0x0800, 0x0000};

struct Image CheckImageLow2Pl =
{0, -1, 13, 10, 2, CheckImageLow2PlPattern, 3, 0, NULL};

struct Image NoCheckImage2Pl =
{0, -1, 17, 10, 0, NULL, 0, 0, NULL};

struct Image NoCheckImageLow2Pl =
{0, -1, 13, 10, 0, NULL, 0, 0, NULL};


UWORD __chip AmigaImage1PlPattern[] =
{0x3fff, 0xf800, 0xfffc, 0x3e00, 0xfff0, 0x3e00, 0xffc6, 0x3e00, 0xff1e, 0x3e00,
 0xfc00, 0x3e00, 0xe0fc, 0x0e00, 0x3fff, 0xf800};

struct Image AmigaImage1Pl =
{0, 0, 23, 8, 1, AmigaImage1PlPattern, 1, 0, NULL};


UWORD __chip AmigaImageLow1PlPattern[] =
{0x7ff8, 0xff1c, 0xfe1c, 0xfc9c, 0xf99c, 0xf01c, 0xc30c, 0x7ff8};

struct Image AmigaImageLow1Pl =
{0, 0, 14, 8, 1, AmigaImageLow1PlPattern, 1, 0, NULL};


UWORD __chip AmigaImage2PlPattern[] =
{0x0000, 0x0000, 0x0007, 0xc000, 0x001f, 0xc000, 0x007f, 0xc000, 0x01f3, 0xc000,
 0x07ff, 0xc000, 0x3fff, 0xf000, 0x3f03, 0xf000, 0x000f, 0x8000, 0x003f, 0x8000,
 0x00ff, 0x8000, 0x03f3, 0x8000, 0x0fcf, 0x8000, 0x7fff, 0x8000, 0x7e07, 0xe000,
 0x0000, 0x0000};

struct Image AmigaImage2Pl =
{1, -1, 23, 8, 2, AmigaImage2PlPattern, 3, 0, NULL};


UWORD __chip AmigaImageLow2PlPattern[] =
{0x0000, 0x00f0, 0x01f0, 0x03f0, 0x0770, 0x0ff0, 0x3ff8, 0x3ef8, 0x01e0, 0x03e0,
 0x07e0, 0x0f60, 0x1ee0, 0x7fe0, 0x7cf0, 0x0000};

struct Image AmigaImageLow2Pl =
{0, -1, 14, 8, 2, AmigaImageLow2PlPattern, 3, 0, NULL};


UWORD __chip SubArrowImage2PlPattern[] =
{0x8000, 0xe000, 0xf800, 0xfe00, 0xfe00, 0xf800, 0xe000, 0x8000, 0x0000, 0x6000,
 0x7800, 0x7e00, 0x7f80, 0x7e00, 0x7800, 0x6000};

struct Image SubArrowImage2Pl =
{0, 0, 9, 8, 2, SubArrowImage2PlPattern, 3, 0, NULL};


UWORD __chip SubArrowImage1PlPattern[] =
{0x8000, 0xe000, 0xf800, 0xfe00, 0xfe00, 0xf800, 0xe000, 0x8000};

struct Image SubArrowImage1Pl =
{1, 0, 7, 8, 1, SubArrowImage1PlPattern, 1, 0, NULL};


UWORD __chip SubArrowImageLow2PlPattern[] =
{0x8000, 0xc000, 0xe000, 0xf000, 0xf000, 0xe000, 0xc000, 0x8000, 0x0000, 0x4000,
 0x6000, 0x7000, 0x7800, 0x7000, 0x6000, 0x4000};

struct Image SubArrowImageLow2Pl =
{0, 0, 5, 8, 2, SubArrowImageLow2PlPattern, 3, 0, NULL};


UWORD __chip SubArrowImageLow1PlPattern[] =
{0x8000, 0xc000, 0xe000, 0xf000, 0xf000, 0xe000, 0xc000, 0x8000};

struct Image SubArrowImageLow1Pl =
{0, 0, 4, 8, 1, SubArrowImageLow1PlPattern, 1, 0, NULL};



/******************************************************************************/
static UWORD __chip Amiga8_NormalBitMap[36] =
{
	0x7FF0,0xFFFC,0xC41C,0xCD5C,0xD6BC,0xC55C,0xDFFC,0x7FF8,
	0x0000,

	0x0000,0x0000,0x1BF0,0x12A0,0x0940,0x1AA0,0x1FE0,0x2000,
	0x0000,

	0x0000,0x7FF0,0x6000,0x6140,0x62A0,0x6540,0x4000,0x2000,
	0x0000,

	0xFFFE,0x8006,0x8006,0x8006,0x8006,0x8006,0x8006,0xE00E,
	0xFFFE
};

struct Image Amiga8_Normal =
{
	0,0,
	15,9,4,
	Amiga8_NormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip Amiga8_GhostedBitMap[36] =
{
	0x7FF0,0x800C,0xBFFE,0xBEBE,0xBD5E,0xBABE,0xBFFE,0x7FFE,
	0x3FFC,

	0x0000,0x7FF0,0x5BE2,0x53E2,0x4BE2,0x5FE2,0x4002,0x0006,
	0x3FFC,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFFE,0xFFFE,0xFFFE,0xFFFE,0xFFFE,0xFFFE,0xFFFE,0xFFFE,
	0xFFFE
};

struct Image Amiga8_Ghosted =
{
	0,0,
	15,9,4,
	Amiga8_GhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip Check8_NormalBitMap[36] =
{
	0x0020,0x0070,0x00F0,0x9160,0x7AC0,0x3F80,0x1B00,0x0E00,
	0x0000,

	0x00E0,0x0180,0x0340,0xF600,0x4100,0x2A00,0x1400,0x0800,
	0x0000,

	0x0020,0x0000,0x0000,0x9080,0x0500,0x0A00,0x0000,0x0000,
	0x0000,

	0xFF90,0xFF00,0xFE10,0x0C30,0xC070,0xE0F0,0xF1F0,0xFBF0,
	0xFFF0
};

struct Image Check8_Normal =
{
	0,0,
	12,9,4,
	Check8_NormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip Check8_GhostedBitMap[36] =
{
	0x0000,0x0070,0x00F0,0x01E0,0x3BC0,0x1F80,0x0F00,0x0600,
	0x0400,

	0x0000,0x0000,0x0010,0x00A0,0x0140,0x0A80,0x0500,0x0200,
	0x0400,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,
	0xFFF0
};

struct Image Check8_Ghosted =
{
	0,0,
	12,9,4,
	Check8_GhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXDown8_NormalBitMap[36] =
{
	0x3FC0,0x7FE0,0x7EC0,0x6DC0,0x7FC0,0x7FC0,0x7F80,0x2000,
	0x0000,

	0x0000,0x0000,0x01A0,0x1220,0x0420,0x0020,0x0060,0x1FC0,
	0x0000,

	0x0000,0x0000,0x0000,0x0A00,0x0400,0x0000,0x0000,0x0000,
	0x0000,

	0xC070,0x9FF0,0xBE50,0xA4D0,0xB1D0,0xBBD0,0xFF90,0xE030,
	0xFFF0
};

struct Image MXDown8_Normal =
{
	0,0,
	12,9,4,
	MXDown8_NormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXDown8_GhostedBitMap[36] =
{
	0x3F80,0x6000,0x41A0,0x5B20,0x4E20,0x4420,0x0060,0x1FC0,
	0x0000,

	0x0040,0x1FE0,0x3F60,0x3EE0,0x35E0,0x3BE0,0x7FE0,0x3FC0,
	0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,
	0xFFF0
};

struct Image MXDown8_Ghosted =
{
	0,0,
	12,9,4,
	MXDown8_GhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXUp8_NormalBitMap[36] =
{
	0x0040,0x1FE0,0x3FF0,0x3FF0,0x3FF0,0x3FF0,0x7FF0,0x3FF0,
	0x1FE0,

	0x3F80,0x7FC0,0x7FC0,0x7FC0,0x7FC0,0x7FC0,0x3F80,0x0000,
	0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xC070,0x9FF0,0xBFD0,0xBFD0,0xBFD0,0xBFD0,0xFF90,0xE030,
	0xFFF0
};

struct Image MXUp8_Normal =
{
	0,0,
	12,9,4,
	MXUp8_NormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXUp8_GhostedBitMap[36] =
{
	0x3F80,0x6000,0x4030,0x4030,0x4030,0x4030,0x0070,0x1FF0,
	0x1FE0,

	0x3FC0,0x7FE0,0x7FD0,0x7FD0,0x7FD0,0x7FD0,0x7F90,0x2030,
	0x1FE0,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,
	0xFFF0
};

struct Image MXUp8_Ghosted =
{
	0,0,
	12,9,4,
	MXUp8_GhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip Arrow8_NormalBitMap[20] =
{
	0x0000,0x4000,0x7800,0xF800,0x6000,

	0xC000,0xF000,0xC000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,

	0x3800,0x4800,0x4800,0xB800,0xF800
};

struct Image Arrow8_Normal =
{
	0,0,
	5,5,4,
	Arrow8_NormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip Arrow8_GhostedBitMap[20] =
{
	0xC000,0xB000,0xB800,0x7800,0x6000,

	0xC000,0xF000,0xC800,0xB800,0x6000,

	0x0000,0x0000,0x0000,0x0000,0x0000,

	0xF800,0xF800,0xF800,0xF800,0xF800
};

struct Image Arrow8_Ghosted =
{
	0,0,
	5,5,4,
	Arrow8_GhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip AmigaNormalBitMap[96] =
{
	0x7FFE,0x0000,0xFFFF,0x0000,0xC007,0x0000,0xC257,0x0000,
	0xC6AF,0x0000,0xCB57,0x0000,0xC2AF,0x0000,0xC557,0x0000,
	0xC007,0x0000,0xFFFF,0x0000,0x7FFE,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0002,0x0000,0x1FFC,0x0000,0x1DA8,0x0000,
	0x1950,0x0000,0x14A8,0x0000,0x1D50,0x0000,0x1AA8,0x0000,
	0x0000,0x0000,0x2002,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x7FFC,0x0000,0x6000,0x0000,0x6050,0x0000,
	0x60A8,0x0000,0x6150,0x0000,0x62A8,0x0000,0x6550,0x0000,
	0x7FF8,0x0000,0x4000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0x8001,0x8000,0x8001,0x8000,0x8001,0x8000,
	0x8001,0x8000,0x8001,0x8000,0x8001,0x8000,0x8001,0x8000,
	0x8001,0x8000,0x8001,0x8000,0xFFFF,0x8000,0xFFFF,0x8000
};

struct Image AmigaNormal =
{
	0,0,
	17,12,4,
	AmigaNormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip AmigaGhostedBitMap[96] =
{
	0x7FFE,0x0000,0x8003,0x0000,0xBFFF,0x8000,0xBFAF,0x8000,
	0xBF57,0x8000,0xBEAF,0x8000,0xBD57,0x8000,0xBAAF,0x8000,
	0xBFFF,0x8000,0xBFFF,0x8000,0x7FFF,0x8000,0x3FFF,0x0000,

	0x0000,0x0000,0x7FFC,0x0000,0x5FF8,0x8000,0x5DF8,0x8000,
	0x59F8,0x8000,0x55F8,0x8000,0x5FF8,0x8000,0x5FF8,0x8000,
	0x4000,0x8000,0x4000,0x8000,0x0001,0x8000,0x3FFF,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000
};

struct Image AmigaGhosted =
{
	0,0,
	17,12,4,
	AmigaGhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip CheckNormalBitMap[48] =
{
	0x0012,0x002F,0x0057,0x00A6,0x914C,0xFE98,0x6DB0,0x3FE0,
	0x1DC0,0x0D80,0x0700,0x0000,

	0x001E,0x0030,0x0064,0x00C0,0xF1A0,0x8360,0x58C0,0x2D80,
	0x1700,0x0A00,0x0400,0x0000,

	0x0002,0x0000,0x0008,0x0018,0x9030,0x0060,0x0AC0,0x0D80,
	0x0500,0x0000,0x0000,0x0000,

	0xFFF1,0xFFE0,0xFFC1,0xFF83,0x0F07,0x860F,0xC01F,0xE03F,
	0xF07F,0xF8FF,0xFDFF,0xFFFF
};

struct Image CheckNormal =
{
	0,0,
	16,12,4,
	CheckNormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip CheckGhostedBitMap[48] =
{
	0x0000,0x000F,0x001F,0x0036,0x004C,0x78B8,0x3DF0,0x1FE0,
	0x0FC0,0x0780,0x0300,0x0200,

	0x0000,0x0000,0x0001,0x000A,0x0034,0x0068,0x18D0,0x0DA0,
	0x0740,0x0280,0x0100,0x0200,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF
};

struct Image CheckGhosted =
{
	0,0,
	16,12,4,
	CheckGhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXDownNormalBitMap[48] =
{
	0x0000,0x0FF0,0x1FF8,0x1FB0,0x1B30,0x1E70,0x1FF0,0x1FF0,
	0x1FE0,0x0800,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0068,0x0488,0x0388,0x0108,0x0008,
	0x0018,0x07F0,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0240,0x0280,0x0100,0x0000,
	0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0xF01F,0xE7FF,0xEF97,0xE917,0xE837,0xEC77,0xEEF7,
	0xFFE7,0xF80F,0xFFFF,0xFFFF
};

struct Image MXDownNormal =
{
	0,0,
	16,12,4,
	MXDownNormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXDownGhostedBitMap[48] =
{
	0x0000,0x0FE0,0x1800,0x1068,0x14E8,0x17C8,0x1388,0x1108,
	0x0018,0x07F0,0x0000,0x0000,

	0x0000,0x0010,0x07F8,0x0FD8,0x0F98,0x0BB8,0x0D78,0x0EF8,
	0x1FF8,0x0FF0,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF
};

struct Image MXDownGhosted =
{
	0,0,
	16,12,4,
	MXDownGhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXUpNormalBitMap[48] =
{
	0x0000,0x0010,0x07F8,0x0FFC,0x0FFC,0x0FFC,0x0FFC,0x0FFC,
	0x1FFC,0x0FFC,0x07F8,0x0000,

	0x0000,0x0FE0,0x1FF0,0x1FF0,0x1FF0,0x1FF0,0x1FF0,0x1FF0,
	0x0FE0,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0xF01F,0xE7FF,0xEFF7,0xEFF7,0xEFF7,0xEFF7,0xEFF7,
	0xFFE7,0xF80F,0xFFFF,0xFFFF
};

struct Image MXUpNormal =
{
	0,0,
	16,12,4,
	MXUpNormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip MXUpGhostedBitMap[48] =
{
	0x0000,0x0FE0,0x1800,0x100C,0x100C,0x100C,0x100C,0x100C,
	0x001C,0x07FC,0x07F8,0x0000,

	0x0000,0x0FF0,0x1FF8,0x1FF4,0x1FF4,0x1FF4,0x1FF4,0x1FF4,
	0x1FE4,0x080C,0x07F8,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF
};

struct Image MXUpGhosted =
{
	0,0,
	16,12,4,
	MXUpGhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip ArrowNormalBitMap[28] =
{
	0x0000,0x4000,0x7000,0x7E00,0x7E00,0xF800,0x6000,

	0xC000,0xF000,0xFC00,0xF000,0xC000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x3E00,0x4E00,0x7200,0x7200,0x4E00,0xBE00,0xFE00
};

struct Image ArrowNormal =
{
	0,0,
	7,7,4,
	ArrowNormalBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip ArrowGhostedBitMap[28] =
{
	0xC000,0xB000,0x8C00,0x8E00,0xBE00,0x7800,0x6000,

	0xC000,0xF000,0xFC00,0xF200,0xCE00,0xB800,0x6000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFE00,0xFE00,0xFE00,0xFE00,0xFE00,0xFE00,0xFE00
};

struct Image ArrowGhosted =
{
	0,0,
	7,7,4,
	ArrowGhostedBitMap,
	0x0F,0x00,
	NULL
};

static UWORD __chip AmigaNormalBitMap4[48] =
{
	0x7FFE,0x0000,0x8003,0x0000,0x8007,0x0000,0x8207,0x0000,
	0x8607,0x0000,0x8A07,0x0000,0x8007,0x0000,0x8007,0x0000,
	0x8007,0x0000,0xBFFF,0x0000,0x7FFE,0x0000,0x0000,0x0000,

	0x7FFE,0x0000,0x8003,0x0000,0x9FFC,0x0000,0x9DA9,0x0000,
	0x9950,0x0000,0x94A9,0x0000,0x9D50,0x0000,0x9AA9,0x0000,
	0x8000,0x0000,0xA001,0x0000,0x2AAA,0x0000,0x0000,0x0000
};

struct Image AmigaNormal4 =
{
	0,0,
	17,12,2,
	AmigaNormalBitMap4,
	0x03,0x00,
	NULL
};

static UWORD __chip CheckNormalBitMap4[24] =
{
	0x0000,0x001E,0x003E,0x007C,0x00B8,0xF930,0x4E60,0x24C0,
	0x1180,0x0B00,0x0600,0x0000,

	0x0018,0x0020,0x005A,0x00B4,0xC128,0x0A10,0x2420,0x0040,
	0x0480,0x0500,0x0200,0x0000
};

struct Image CheckNormal4 =
{
	0,0,
	15,12,2,
	CheckNormalBitMap4,
	0x03,0x00,
	NULL
};

static UWORD __chip MXDownNormalBitMap4[24] =
{
	0x0000,0x1FE0,0x3FF0,0x3F60,0x32E0,0x38E0,0x3DE0,0x3FE0,
	0x3FC0,0x1000,0x0000,0x0000,

	0x0000,0x0020,0x0FF0,0x1FF0,0x1BB0,0x1270,0x18F0,0x1DF0,
	0x3FF0,0x1FE0,0x0000,0x0000
};

struct Image MXDownNormal4 =
{
	0,0,
	15,12,2,
	MXDownNormalBitMap4,
	0x03,0x00,
	NULL
};

static UWORD __chip MXUpNormalBitMap4[24] =
{
	0x0000,0x0020,0x0010,0x0018,0x0018,0x0018,0x0018,0x0018,
	0x2038,0x1FF8,0x0FF0,0x0000,

	0x0000,0x1FE0,0x3010,0x2008,0x2008,0x2008,0x2008,0x2008,
	0x2008,0x1018,0x0FF0,0x0000
};

struct Image MXUpNormal4 =
{
	0,0,
	15,12,2,
	MXUpNormalBitMap4,
	0x03,0x00,
	NULL
};

static UWORD __chip ArrowNormalBitMap4[14] =
{
	0x0000,0x0000,0x0000,0x0E00,0x3E00,0xF800,0x6000,

	0xC000,0xB000,0x8C00,0x8200,0x8E00,0xB800,0x6000
};

struct Image ArrowNormal4 =
{
	0,0,
	7,7,2,
	ArrowNormalBitMap4,
	0x03,0x00,
	NULL
};

static UWORD __chip AmigaNormalBitMap8_4[18] =
{
	0x7FF0,0x800C,0xA41C,0xAC1C,0xB41C,0xA01C,0xBFFC,0x7FF8,
	0x0000,

	0x7FF0,0x8004,0xBBF4,0xB2A4,0xA944,0xBAA4,0xBFE4,0x6008,
	0x0000
};

struct Image AmigaNormal8_4 =
{
	0,0,
	14,9,2,
	AmigaNormalBitMap8_4,
	0x03,0x00,
	NULL
};

static UWORD __chip CheckNormalBitMap8_4[18] =
{
	0x0000,0x00E0,0x01E0,0x03C0,0x7D80,0x2B00,0x1600,0x0C00,
	0x0000,

	0x0080,0x0100,0x02A0,0xC540,0x0880,0x0100,0x0A00,0x0400,
	0x0000
};

struct Image CheckNormal8_4 =
{
	0,0,
	11,9,2,
	CheckNormalBitMap8_4,
	0x03,0x00,
	NULL
};

static UWORD __chip MXDownNormalBitMap8_4[18] =
{
	0x7F80,0xFFC0,0xFD80,0xCB80,0xF780,0xFF80,0xFF00,0x4000,
	0x0000,

	0x0080,0x3FC0,0x7FC0,0x69C0,0x63C0,0x77C0,0xFFC0,0x7F80,
	0x0000
};

struct Image MXDownNormal8_4 =
{
	0,0,
	11,9,2,
	MXDownNormalBitMap8_4,
	0x03,0x00,
	NULL
};

static UWORD __chip MXUpNormalBitMap8_4[18] =
{
	0x0080,0x0040,0x0060,0x0060,0x0060,0x0060,0x80E0,0x7FE0,
	0x3FC0,

	0x7F80,0xC040,0x8020,0x8020,0x8020,0x8020,0x8020,0x4060,
	0x3FC0
};

struct Image MXUpNormal8_4 =
{
	0,0,
	11,9,2,
	MXUpNormalBitMap8_4,
	0x03,0x00,
	NULL
};

static UWORD __chip ArrowNormalBitMap8_4[10] =
{
	0x0000,0x0000,0x3800,0xF800,0x6000,

	0xC000,0xB000,0x8800,0xB800,0x6000
};

struct Image ArrowNormal8_4 =
{
	0,0,
	5,5,2,
	ArrowNormalBitMap8_4,
	0x03,0x00,
	NULL
};
/******************************************************************************/

ULONG OldOpenWindow;
ULONG OldOpenWindowTagList;
ULONG OldClearMenuStrip;
ULONG OldSetMenuStrip;
ULONG OldResetMenuStrip;
ULONG OldCloseWindow;
ULONG OldActivateWindow;
ULONG OldWindowToFront;
ULONG OldWindowToBack;
ULONG OldModifyIDCMP;
ULONG OldObtainGIRPort;
ULONG OldLendMenus;
ULONG OldOffMenu;
ULONG OldOnMenu;
ULONG OldSetWindowTitles;
ULONG OldRefreshWindowFrame;
ULONG OldCloseScreen;
ULONG OldOpenScreen;
ULONG OldOpenScreenTagList;
ULONG OldScreenToFront;
ULONG OldScreenToBack;
ULONG OldScreenDepth;

ULONG OldCreateUpfrontHookLayer;
ULONG OldCreateUpfrontLayer;

ULONG OldSetFunction;

LONG Cx_Pri;
BOOL Cx_Popup;
char Cx_PopupStr[ANSWER_LEN + 1];
char Cx_Popkey[LONGANSWER_LEN + 1];

BOOL DoGlobalQuit;

char TitleText[41];

UBYTE __aligned StdRemapArray[256];
UBYTE __aligned DreiDRemapArray[256];
UBYTE __aligned DreiDGhostedRemapArray[256];
UBYTE __aligned DreiDActiveRemapArray[256];

struct MMPrefs AktPrefs;
char ConfigPath[201];
char ConfigFile[232];

UBYTE MenuMode;

BOOL SelectSpecial;

ULONG CxSignalMask;

BOOL IsBlocking;
struct Window *MenWin;
struct Screen *MenScr;
struct Menu *MenStrip;
struct Rectangle MenDispClip;
struct RastPort ScrRPort;
struct TextAttr MenTextAttr;

struct Image * CheckImage;
struct Image * CheckImageActive;
struct Image * CheckImageGhosted;
struct Image * CheckImageGhostedRmp;
struct Image * CheckImageRmp;

struct Image * CommandImage;
struct Image * CommandImageActive;
struct Image * CommandImageGhosted;
struct Image * CommandImageGhostedRmp;
struct Image * CommandImageRmp;

struct Image * MXImage;
struct Image * MXImageActive;
struct Image * MXImageGhosted;
struct Image * MXImageRmp;

struct Image * NoCheckImage;
struct Image * NoCheckImageActive;
struct Image * NoCheckImageRmp;

struct Image * NoMXImage;
struct Image * NoMXImageActive;
struct Image * NoMXImageGhosted;
struct Image * NoMXImageRmp;

struct Image * SubArrowImage;
struct Image * SubArrowImageActive;
struct Image * SubArrowImageGhosted;
struct Image * SubArrowImageGhostedRmp;
struct Image * SubArrowImageRmp;

ULONG IBaseLock;

struct WBStartup *WBMsg;
char ProgName[32];

/******************************************************************************/

UWORD Crosshatch[2] = {0x5555, 0xAAAA};

/******************************************************************************/

BOOL GlobalPopUp;
