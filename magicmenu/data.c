/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

struct Library *CxBase;
struct Library *IconBase;
struct Library *LayersBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *TimerBase;
struct Library *InputBase;
struct Library *KeymapBase;
struct Library *UtilityBase;

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
CxObj *TickFilter;
CxObj *TickSignal;
struct MsgPort *CxMsgPort;
struct MsgPort *ReplyPort;
struct MsgPort *MMMsgPort;
char MouseKey[] = "rawkey";
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
UWORD StripLeftOffs;
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
  MMP_MAGIC, sizeof (struct MMPrefs),
  MT_AUTO,			/* MenuType */
  {1,				/* Enabled */
   1,				/* MarkSub */
   0,				/* DblBorder */
   0,				/* UseLayer */
   1,				/* DirectDraw */
   1,				/* ChunkyPlanes */
   0,				/* NonBlocking */
   1,				/* KCEnabled */
   1,				/* KCGoTop */
   1,				/* KCRAltRCommand */
   1,				/* PUCenter */
   0},				/* Dummy */
  MODE_STD,			/* PDMode */
  LOOK_MC,			/* PDLook */
  MODE_STD,			/* PUMode */
  LOOK_MC,			/* PULook */
  "rcommand space",		/* KCKeyStr */
  10,				/* TimeOut */
  {0xffffffff, 0xffffffff, 0xffffffff},		/* LightEdge */
  {0x00000000, 0x00000000, 0x00000000},		/* DarkEdge */
  {0x95000000, 0x95000000, 0x95000000},		/* Background */
  {0x00000000, 0x00000000, 0x00000000},		/* TextCol */
  {0x00000000, 0x00000000, 0x00000000},		/* HiCol */
  {0x3b000000, 0x67000000, 0xA3000000},		/* FillCol */
  {0x60000000, 0x60000000, 0x60000000},		/* GhostLoCol */
  {0xaf000000, 0xaf000000, 0xaf000000},		/* GhostHiCol */
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

struct InputXpression ActiveKbdIX =
{
  IX_VERSION,
  IECLASS_RAWKEY,
  0, IECODE_UP_PREFIX,
  0, 0, 0
};

UWORD DitherPattern[] =
{0x1111, 0x4444};




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
static UWORD __chip AmigaNormalBitMap[] =
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
	0x8001,0x8000,0x8001,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
};

struct Image AmigaNormal =
{
	0,0,
	17,12,4,
	AmigaNormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip AmigaGhostedBitMap[] =
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
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
};

struct Image AmigaGhosted =
{
	0,0,
	17,12,4,
	AmigaGhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip ArrowNormalBitMap[] =
{
	0x0000,0x4000,0x7000,0x7E00,0x7E00,0xF800,0x6000,

	0xC000,0xB000,0x8C00,0x8000,0x8000,0x8000,0x0000,

	0x0000,0x4000,0x7000,0x7000,0x4000,0x0000,0x0000,

	0x3E00,0x0E00,0x0200,0x0200,0x0E00,0xBE00,0xFE00,
};

struct Image ArrowNormal =
{
	0,0,
	7,7,4,
	ArrowNormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip ArrowGhostedBitMap[] =
{
	0xC000,0xB000,0x8C00,0x8E00,0xBE00,0xF800,0x6000,

	0xC000,0xF000,0xFC00,0xF200,0xCE00,0xB800,0x6000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFE00,0xFE00,0xFE00,0xFE00,0xFE00,0xFE00,0xFE00,
};

struct Image ArrowGhosted =
{
	0,0,
	7,7,4,
	ArrowGhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip CheckNormalBitMap[] =
{
	0x0012,0x0000,0x002F,0x0000,0x0057,0x0000,0x00A6,0x0000,
	0x914C,0x0000,0xFE98,0x0000,0x6DB0,0x0000,0x3FE0,0x0000,
	0x1DC0,0x0000,0x0D80,0x0000,0x0700,0x0000,

	0x001E,0x0000,0x0030,0x0000,0x0064,0x0000,0x00C0,0x0000,
	0xF1A0,0x0000,0x8360,0x0000,0x58C0,0x0000,0x2D80,0x0000,
	0x1700,0x0000,0x0A00,0x0000,0x0400,0x0000,

	0x0002,0x0000,0x0000,0x0000,0x0008,0x0000,0x0018,0x0000,
	0x9030,0x0000,0x0060,0x0000,0x0AC0,0x0000,0x0D80,0x0000,
	0x0500,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFF1,0x8000,0xFFE0,0x8000,0xFFC1,0x8000,0xFF83,0x8000,
	0x0F07,0x8000,0x860F,0x8000,0xC01F,0x8000,0xE03F,0x8000,
	0xF07F,0x8000,0xF8FF,0x8000,0xFDFF,0x8000,
};

struct Image CheckNormal =
{
	0,0,
	17,11,4,
	CheckNormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip CheckGhostedBitMap[] =
{
	0x0000,0x0000,0x000F,0x0000,0x001F,0x0000,0x0036,0x0000,
	0x004C,0x0000,0x78B8,0x0000,0x3DF0,0x0000,0x1FE0,0x0000,
	0x0FC0,0x0000,0x0780,0x0000,0x0300,0x0000,0x0200,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0001,0x0000,0x000A,0x0000,
	0x0034,0x0000,0x0068,0x0000,0x18D0,0x0000,0x0DA0,0x0000,
	0x0740,0x0000,0x0280,0x0000,0x0100,0x0000,0x0200,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
};

struct Image CheckGhosted =
{
	0,0,
	17,12,4,
	CheckGhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXDownNormalBitMap[] =
{
	0x0000,0x0000,0x1FE0,0x0000,0x3030,0x0000,0x2040,0x0000,
	0x2440,0x0000,0x2C80,0x0000,0x2700,0x0000,0x2200,0x0000,
	0x3000,0x0000,0x1000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0020,0x0000,0x00D0,0x0000,
	0x0910,0x0000,0x0710,0x0000,0x0210,0x0000,0x0010,0x0000,
	0x1030,0x0000,0x0FE0,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0FC0,0x0000,0x1F20,0x0000,
	0x16A0,0x0000,0x1560,0x0000,0x1AE0,0x0000,0x1DE0,0x0000,
	0x0FC0,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0xE03F,0x8000,0xC03F,0x8000,0xC00F,0x8000,
	0xC00F,0x8000,0xC00F,0x8000,0xC00F,0x8000,0xC00F,0x8000,
	0xF00F,0x8000,0xF01F,0x8000,0xFFFF,0x8000,
};

struct Image MXDownNormal =
{
	0,0,
	17,11,4,
	MXDownNormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXDownGhostedBitMap[] =
{
	0x0000,0x0000,0x1FE0,0x0000,0x3010,0x0000,0x20D0,0x0000,
	0x29D0,0x0000,0x2F90,0x0000,0x2710,0x0000,0x2210,0x0000,
	0x2030,0x0000,0x1FE0,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0FE0,0x0000,0x1FB0,0x0000,
	0x1F30,0x0000,0x1770,0x0000,0x1AF0,0x0000,0x1DF0,0x0000,
	0x1FF0,0x0000,0x0FE0,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
};

struct Image MXDownGhosted =
{
	0,0,
	17,11,4,
	MXDownGhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXUpNormalBitMap[] =
{
	0x0000,0x0000,0x0020,0x0000,0x0FF0,0x0000,0x1FF8,0x0000,
	0x1FF8,0x0000,0x1FF8,0x0000,0x1FF8,0x0000,0x1FF8,0x0000,
	0x3FF8,0x0000,0x1FF8,0x0000,0x0FF0,0x0000,

	0x0000,0x0000,0x1FC0,0x0000,0x3000,0x0000,0x2000,0x0000,
	0x2000,0x0000,0x2000,0x0000,0x2000,0x0000,0x2000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0FE0,0x0000,0x1FE0,0x0000,
	0x1FE0,0x0000,0x1FE0,0x0000,0x1FE0,0x0000,0x1FE0,0x0000,
	0x1FC0,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0xE03F,0x8000,0xC01F,0x8000,0xC00F,0x8000,
	0xC00F,0x8000,0xC00F,0x8000,0xC00F,0x8000,0xC00F,0x8000,
	0xE00F,0x8000,0xF01F,0x8000,0xFFFF,0x8000,
};

struct Image MXUpNormal =
{
	0,0,
	17,11,4,
	MXUpNormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXUpGhostedBitMap[] =
{
	0x0000,0x0000,0x1FE0,0x0000,0x3010,0x0000,0x2018,0x0000,
	0x2018,0x0000,0x2018,0x0000,0x2018,0x0000,0x2018,0x0000,
	0x2038,0x0000,0x1FF8,0x0000,0x0FF0,0x0000,

	0x0000,0x0000,0x1FC0,0x0000,0x3FE0,0x0000,0x3FE8,0x0000,
	0x3FE8,0x0000,0x3FE8,0x0000,0x3FE8,0x0000,0x3FE8,0x0000,
	0x1FC8,0x0000,0x0018,0x0000,0x0FF0,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
	0xFFFF,0x8000,0xFFFF,0x8000,0xFFFF,0x8000,
};

struct Image MXUpGhosted =
{	0,0,
	17,11,4,
	MXUpGhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip Amiga8_NormalBitMap[] =
{
	0x7FE0,0xFFF0,0xD870,0xDAF0,0xC570,0xC070,0xFFF0,0x7FE0,
	0x0000,

	0x0000,0x0020,0x07C0,0x1500,0x1A80,0x0000,0x2020,0x0000,
	0x0000,

	0x0000,0x7FC0,0x7000,0x6280,0x6500,0x7F80,0x4000,0x0000,
	0x0000,

	0xFFF8,0x8018,0x8018,0x8018,0x8018,0x8018,0x8018,0xFFF8,
	0xFFF8,
};

struct Image Amiga8_Normal =
{
	0,0,
	13,9,4,
	Amiga8_NormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip Amiga8_GhostedBitMap[] =
{
	0x7FE0,0x8030,0xAFF8,0xBD78,0xBAF8,0xBFF8,0xBFF8,0x7FF8,
	0x3FF0,

	0x0000,0x7FC0,0x5788,0x4788,0x5F88,0x4008,0x4008,0x0018,
	0x3FF0,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF8,0xFFF8,0xFFF8,0xFFF8,0xFFF8,0xFFF8,0xFFF8,0xFFF8,
	0xFFF8,
};

struct Image Amiga8_Ghosted =
{
	0,0,
	13,9,4,
	Amiga8_GhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip Check8_NormalBitMap[] =
{
	0x0020,0x0070,0x00F0,0x9160,0x7AC0,0x3F80,0x1B00,0x0E00,
	0x0000,

	0x00E0,0x0180,0x0340,0xF600,0x4100,0x2A00,0x1400,0x0800,
	0x0000,

	0x0020,0x0000,0x0000,0x9080,0x0500,0x0A00,0x0000,0x0000,
	0x0000,

	0xFF90,0xFF00,0xFE10,0x0C30,0xC070,0xE0F0,0xF1F0,0xFBF0,
	0xFFF0,
};

struct Image Check8_Normal =
{
	0,0,
	12,9,4,
	Check8_NormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip Check8_GhostedBitMap[] =
{
	0x0000,0x0070,0x00F0,0x01E0,0x3BC0,0x1F80,0x0F00,0x0600,
	0x0400,

	0x0000,0x0000,0x0010,0x00A0,0x0140,0x0A80,0x0500,0x0200,
	0x0400,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,
	0xFFF0,
};

struct Image Check8_Ghosted =
{
	0,0,
	12,9,4,
	Check8_GhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXDown8_NormalBitMap[] =
{
	0x0000,0x3FC0,0x6060,0x4080,0x4900,0x4E00,0x4400,0x6000,
	0x2000,

	0x0000,0x0000,0x0040,0x01A0,0x1220,0x0420,0x0020,0x2060,
	0x1FC0,

	0x0000,0x0000,0x1F80,0x3E40,0x2EC0,0x35C0,0x3BC0,0x1F80,
	0x0000,

	0xFFF0,0xC070,0x8070,0x8010,0x8010,0x8010,0x8010,0xE010,
	0xE030,
};

struct Image MXDown8_Normal =
{
	0,0,
	12,9,4,
	MXDown8_NormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXDown8_GhostedBitMap[] =
{
	0x0000,0x3FC0,0x6020,0x41A0,0x5B20,0x4E20,0x4420,0x4060,
	0x3FC0,

	0x0000,0x0000,0x1FC0,0x3F60,0x3EE0,0x35E0,0x3BE0,0x3FE0,
	0x1FC0,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,
	0xFFF0,
};

struct Image MXDown8_Ghosted =
{
	0,0,
	12,9,4,
	MXDown8_GhostedBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXUp8_NormalBitMap[] =
{
	0x0040,0x1FE0,0x3FF0,0x3FF0,0x3FF0,0x3FF0,0x3FF0,0x1FF0,
	0x1FE0,

	0x3F80,0x6000,0x4000,0x4000,0x4000,0x4000,0x0000,0x0000,
	0x0000,

	0x0000,0x1FC0,0x3FC0,0x3FC0,0x3FC0,0x3FC0,0x7F80,0x2000,
	0x0000,

	0xC070,0x8030,0x8010,0x8010,0x8010,0x8010,0x8010,0xC030,
	0xFFF0,
};

struct Image MXUp8_Normal =
{
	0,0,
	12,9,4,
	MXUp8_NormalBitMap,
	0xF,0x00,
	NULL
};

static UWORD __chip MXUp8_GhostedBitMap[] =
{
	0x3FC0,0x6020,0x4030,0x4030,0x4030,0x4030,0x4070,0x3FF0,
	0x1FE0,

	0x3F80,0x7FC0,0x7FD0,0x7FD0,0x7FD0,0x7FD0,0x3F90,0x0030,
	0x1FE0,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,

	0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,
	0xFFF0,
};

struct Image MXUp8_Ghosted =
{
	0,0,
	12,9,4,
	MXUp8_GhostedBitMap,
	0xF,0x00,
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
ULONG OldModifyIDCMP;
ULONG OldObtainGIRPort;

ULONG OldCreateUpfrontHookLayer;
ULONG OldCreateUpfrontLayer;

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

struct Window *MenWin;
struct Screen *MenScr;
struct Menu *MenStrip;
struct Rectangle MenDispClip;
struct RastPort ScrRPort;
struct TextAttr MenTextAttr;
struct Image *CheckImage, *NoCheckImage;
struct Image *MXImage, *NoMXImage;
struct Image *CommandImage, *SubArrowImage;
struct Image *CommandImageGhosted, *CheckImageGhosted;
struct Image *SubArrowImageGhosted;
struct Image *CheckImageActive, *NoCheckImageActive;
struct Image *CommandImageActive, *SubArrowImageActive;
struct Image *CheckImageRmp, *NoCheckImageRmp;
struct Image *CommandImageRmp, *SubArrowImageRmp;
struct Image *CommandImageGhostedRmp, *CheckImageGhostedRmp;
struct Image *SubArrowImageGhostedRmp;

struct Image *MXImageRmp;
struct Image *NoMXImageRmp;
struct Image *MXImageGhosted;
struct Image *NoMXImageGhosted;
struct Image *MXImageActive;
struct Image *NoMXImageActive;

ULONG IBaseLock;

struct WBStartup *WBMsg;
char ProgName[32];
