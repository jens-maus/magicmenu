/*
**	$Id$
**
**	:ts=8
*/

//#define DEMO_MENU

/******************************************************************************/

#ifndef _SYSTEM_HEADERS_H
#include "system_headers.h"
#endif	/* _SYSTEM_HEADERS_H */

/******************************************************************************/

#include "MagicMenuPrefs_rev.h"

STRPTR VersTag = VERSTAG;

/******************************************************************************/

#define CATCOMP_ARRAY

#include "magicmenu.h"

/******************************************************************************/

long __stack = 16000;

/******************************************************************************/

#ifndef _STORAGE_H
#include "storage.h"
#endif	/* _STORAGE_H */

/******************************************************************************/

#include "prefs.h"

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

	FALSE,					/* Transparency */
	FALSE,					/* HighlightDisabled */
	0,					/* SeparatorBarStyle */
	FALSE,					/* VerifyPatches */
	FALSE,					/* FixPatches */
};

struct MMPrefs CurrentPrefs;
struct MMPrefs InitialPrefs;

struct MMPrefs TestPrefs;
BOOL TestPrefsValid;

/******************************************************************************/

UBYTE MagicColours[8][3] =
{
	0x95,0x95,0x95,
	0x00,0x00,0x00,
	0xFF,0xFF,0xFF,
	0x3B,0x67,0xA2,
	0x7B,0x7B,0x7B,
	0xAF,0xAF,0xAF,
	0xAA,0x90,0x7C,
	0xFF,0xA9,0x97
};

/******************************************************************************/

struct StorageItem PrefsStorage[] =
{
	DECLARE_ITEM(MMPrefs,mmp_MenuType,		SIT_UBYTE,	"MenuType"),
	DECLARE_ITEM(MMPrefs,mmp_Enabled,		SIT_BOOLEAN,	"Enabled"),
	DECLARE_ITEM(MMPrefs,mmp_MarkSub,		SIT_BOOLEAN,	"MarkSub"),
	DECLARE_ITEM(MMPrefs,mmp_DblBorder,		SIT_BOOLEAN,	"DblBorder"),
	DECLARE_ITEM(MMPrefs,mmp_NonBlocking,		SIT_BOOLEAN,	"NonBlocking"),
	DECLARE_ITEM(MMPrefs,mmp_KCEnabled,		SIT_BOOLEAN,	"KCEnabled"),
	DECLARE_ITEM(MMPrefs,mmp_KCGoTop,		SIT_BOOLEAN,	"KCGoTop"),
	DECLARE_ITEM(MMPrefs,mmp_KCRAltRCommand,	SIT_BOOLEAN,	"KCAltRCommand"),
	DECLARE_ITEM(MMPrefs,mmp_PUCenter,		SIT_BOOLEAN,	"KCPUCenter"),
	DECLARE_ITEM(MMPrefs,mmp_PreferScreenColours,	SIT_BOOLEAN,	"PreferScreenColours"),
	DECLARE_ITEM(MMPrefs,mmp_Delayed,		SIT_BOOLEAN,	"Delayed"),
	DECLARE_ITEM(MMPrefs,mmp_DrawFrames,		SIT_BOOLEAN,	"DrawFrames"),
	DECLARE_ITEM(MMPrefs,mmp_CastShadows,		SIT_BOOLEAN,	"CastShadows"),
	DECLARE_ITEM(MMPrefs,mmp_PDMode,		SIT_UBYTE,	"PDMode"),
	DECLARE_ITEM(MMPrefs,mmp_PDLook,		SIT_UBYTE,	"PDLook"),
	DECLARE_ITEM(MMPrefs,mmp_PUMode,		SIT_UBYTE,	"PUMode"),
	DECLARE_ITEM(MMPrefs,mmp_PULook,		SIT_UBYTE,	"PULook"),
	DECLARE_ITEM(MMPrefs,mmp_KCKeyStr,		SIT_TEXT,	"KCKeyStr"),
	DECLARE_ITEM(MMPrefs,mmp_Precision,		SIT_WORD,	"Precision"),

	DECLARE_ITEM(MMPrefs,mmp_LightEdge.R,		SIT_ULONG,	"LightEdge.R"),
	DECLARE_ITEM(MMPrefs,mmp_LightEdge.G,		SIT_ULONG,	"LightEdge.G"),
	DECLARE_ITEM(MMPrefs,mmp_LightEdge.B,		SIT_ULONG,	"LightEdge.B"),
	DECLARE_ITEM(MMPrefs,mmp_DarkEdge.R,		SIT_ULONG,	"DarkEdge.R"),
	DECLARE_ITEM(MMPrefs,mmp_DarkEdge.G,		SIT_ULONG,	"DarkEdge.G"),
	DECLARE_ITEM(MMPrefs,mmp_DarkEdge.B,		SIT_ULONG,	"DarkEdge.B"),
	DECLARE_ITEM(MMPrefs,mmp_Background.R,		SIT_ULONG,	"Background.R"),
	DECLARE_ITEM(MMPrefs,mmp_Background.G,		SIT_ULONG,	"Background.G"),
	DECLARE_ITEM(MMPrefs,mmp_Background.B,		SIT_ULONG,	"Background.B"),
	DECLARE_ITEM(MMPrefs,mmp_TextCol.R,		SIT_ULONG,	"Text.R"),
	DECLARE_ITEM(MMPrefs,mmp_TextCol.G,		SIT_ULONG,	"Text.G"),
	DECLARE_ITEM(MMPrefs,mmp_TextCol.B,		SIT_ULONG,	"Text.B"),
	DECLARE_ITEM(MMPrefs,mmp_HiCol.R,		SIT_ULONG,	"HighText.R"),
	DECLARE_ITEM(MMPrefs,mmp_HiCol.G,		SIT_ULONG,	"HighText.G"),
	DECLARE_ITEM(MMPrefs,mmp_HiCol.B,		SIT_ULONG,	"HighText.B"),
	DECLARE_ITEM(MMPrefs,mmp_FillCol.R,		SIT_ULONG,	"Fill.R"),
	DECLARE_ITEM(MMPrefs,mmp_FillCol.G,		SIT_ULONG,	"Fill.G"),
	DECLARE_ITEM(MMPrefs,mmp_FillCol.B,		SIT_ULONG,	"Fill.B"),

	DECLARE_ITEM(MMPrefs,mmp_Transparency,		SIT_BOOLEAN,	"Transparency"),	/* == item #37 */
	DECLARE_ITEM(MMPrefs,mmp_HighlightDisabled,	SIT_BOOLEAN,	"HighlightDisabled"),
	DECLARE_ITEM(MMPrefs,mmp_SeparatorBarStyle,	SIT_UBYTE,	"SeparatorBarStyle"),
	DECLARE_ITEM(MMPrefs,mmp_VerifyPatches,		SIT_BOOLEAN,	"VerifyPatches"),
	DECLARE_ITEM(MMPrefs,mmp_FixPatches,		SIT_BOOLEAN,	"FixPatches"),
};

/******************************************************************************/

#define NUM_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

#define SPREAD(c)	(0x01010101UL * (c))

enum
{
	MODE_Edit,
	MODE_Save,
	MODE_Use
};

#define MAX_FILENAME_LENGTH 256

#define REG(x) register __##x
#define ASM __asm
#define SAVE_DS __saveds
#define STACK_ARGS __stdargs

/******************************************************************************/

enum
{
	GAD_Pager=1000,
	GAD_PageGroup,
	GAD_GradientSlider,
	GAD_ColorWheel,
	GAD_WhichPen,
	GAD_Save,
	GAD_Use,
	GAD_Test,
	GAD_Cancel,
	GAD_ModelPager,
	GAD_Red,
	GAD_Green,
	GAD_Blue,
	GAD_Hue,
	GAD_Saturation,
	GAD_Brightness,
	GAD_PDMode,
	GAD_PDLook,
	GAD_PUMode,
	GAD_PULook,
	GAD_PUCenter,
	GAD_MenuType,
	GAD_MarkSub,
	GAD_DblBorder,
	GAD_NonBlocking,
	GAD_KCEnabled,
	GAD_KCGoTop,
	GAD_KCRAltRCommand,
	GAD_KCKeyStr,
	GAD_Precision,
	GAD_PreferScreenColours,
	GAD_Delayed,
	GAD_DrawFrames,
	GAD_CastShadows,
	GAD_Sample,
	GAD_HighlightDisabled,
	GAD_Transparency,
	GAD_SeparatorBarStyle,
	GAD_VerifyPatches,
	GAD_FixPatches
};

enum
{
	MEN_Open,
	MEN_SaveAs,
	MEN_About,
	MEN_Quit,

	MEN_Reset,
	MEN_LastSaved,
	MEN_Restore,

	MEN_CreateIcons,
	MEN_RGB,
	MEN_HSB
};

/******************************************************************************/

struct LoadThatColour
{
	WORD One,Which;
	ULONG Red,Green,Blue;
};

/******************************************************************************/

extern struct ExecBase	*SysBase;
extern struct Library	*DOSBase;

/******************************************************************************/

struct IntuitionBase	*IntuitionBase;
struct GfxBase		*GfxBase;
struct Library		*UtilityBase;
struct Library		*AslBase;
struct Library		*LocaleBase;
struct Library		*IconBase;
struct Library		*GTLayoutBase;

/******************************************************************************/

struct Catalog		*Catalog;

/******************************************************************************/

struct Library		*ColorWheelBase;
struct Library		*GradientSliderBase;

struct ColorWheelRGB	 ColorRGB;
struct ColorWheelHSB	 ColorHSB;

/******************************************************************************/

#define GRADIENT_PENS	 8
#define COLOUR_PENS	 21

WORD			 GradientPens[GRADIENT_PENS+1];
WORD			 GradientPensUsed;

LONG			 Pens[COLOUR_PENS];
BOOL			 GotPens;

/******************************************************************************/

BOOL			 V39;
WORD			 ProgramMode;
BOOL			 CreateIcons;

/******************************************************************************/

LONG			 CX_Priority;
BOOL			 CX_PopUp;
UBYTE			 CX_PopKey[200];

/******************************************************************************/

struct Screen		*CustomScreen;
struct Screen		*PubScreen;
LayoutHandle		*Handle;
struct Window		*Window;
struct Menu		*Menu;
UBYTE			 WindowTitle[300];

/******************************************************************************/

WORD			 WhichPen;
BOOL			 RGB_Mode;

/******************************************************************************/

struct FileRequester	*FileRequester;
BPTR			 HomeDir;
UBYTE			 FileName[MAX_FILENAME_LENGTH];

/******************************************************************************/

extern struct Image	 Demo_8_Image;
extern struct Image	 Demo_11_Image;

/******************************************************************************/

struct BitMap		*SampleMenu;
LONG			 SampleMenuWidth,SampleMenuHeight;

/******************************************************************************/

struct Hook		 LocaleHook;
struct Hook		 SampleRefreshHook;

/******************************************************************************/

struct MsgPort		*ReplyPort;
struct MsgPort		*GlobalPort;

/******************************************************************************/

 /* This is how a linked list of directory search paths looks like. */

struct Path
{
  BPTR path_Next;               /* Pointer to next entry */
  BPTR path_Lock;               /* The drawer in question; may be NULL */
};

BPTR
ClonePath (BPTR StartPath)
{
  struct Path *First, *Last, *List, *New;

  for (List = BADDR (StartPath), First = Last = NULL; List; List = BADDR (List->path_Next))
  {
    if (List->path_Lock)
    {
      if (New = AllocVec (sizeof (struct Path), MEMF_ANY))
      {
        if (New->path_Lock = DupLock (List->path_Lock))
        {
          New->path_Next = NULL;

          if (Last)
            Last->path_Next = MKBADDR (New);

          if (!First)
            First = New;

          Last = New;
        }
        else
        {
          FreeVec (New);
          break;
        }
      }
      else
        break;
    }
  }

  return (MKBADDR (First));
}

VOID
AttachCLI (struct WBStartup * Startup)
{
  struct CommandLineInterface *DestCLI;

  /* Note: FreeDosObject can't free it, but the DOS */
  /*       process termination code can. */

  if (DestCLI = AllocDosObject (DOS_CLI, NULL))
  {
    struct MsgPort *ReplyPort;
    struct Process *Dest;

    DestCLI->cli_DefaultStack = 4096 / sizeof (ULONG);

    Dest = (struct Process *) FindTask (NULL);

    Dest->pr_CLI = MKBADDR (DestCLI);
    Dest->pr_Flags |= PRF_FREECLI;  /* Mark for cleanup */

    Forbid ();

    ReplyPort = Startup->sm_Message.mn_ReplyPort;

    /* Does the reply port data point somewhere sensible? */

    if (ReplyPort && (ReplyPort->mp_Flags & PF_ACTION) == PA_SIGNAL && TypeOfMem (ReplyPort->mp_SigTask))
    {
      struct Process *Father;

      /* Get the address of the process that sent the startup message */

      Father = (struct Process *) ReplyPort->mp_SigTask;

      /* Just to be on the safe side... */

      if (Father->pr_Task.tc_Node.ln_Type == NT_PROCESS)
      {
        struct CommandLineInterface *SourceCLI;

        /* Is there a CLI attached? */

        if (SourceCLI = BADDR (Father->pr_CLI))
        {
          STRPTR Prompt;

          /* Clone the other CLI data. */

          if (Prompt = (STRPTR) BADDR (SourceCLI->cli_Prompt))
            SetPrompt (&Prompt[1]);

          if (SourceCLI->cli_DefaultStack > DestCLI->cli_DefaultStack)
            DestCLI->cli_DefaultStack = SourceCLI->cli_DefaultStack;

          if (SourceCLI->cli_CommandDir)
            DestCLI->cli_CommandDir = ClonePath (SourceCLI->cli_CommandDir);
        }
      }
    }

    Permit ();
  }
}

/******************************************************************************/

struct Library *
SafeOpenLibrary(STRPTR Name,LONG Version)
{
	struct Library *Base;

	Forbid();

	if(Base = (struct Library *)FindName(&SysBase->LibList,FilePart(Name)))
	{
		if(Base->lib_Version < Version)
			RemLibrary(Base);
	}

	Permit();

	return(OpenLibrary(Name,Version));
}

/******************************************************************************/

STRPTR
GetString(ULONG ID)
{
	STRPTR Builtin;

	if(ID < NUM_ELEMENTS(CatCompArray) && CatCompArray[ID].cca_ID == ID)
		Builtin = CatCompArray[ID].cca_Str;
	else
	{
		LONG Left,Mid,Right;

		Left	= 0;
		Right	= NUM_ELEMENTS(CatCompArray) - 1;

		do
		{
			Mid = (Left + Right) / 2;

			if(ID < CatCompArray[Mid].cca_ID)
				Right	= Mid - 1;
			else
				Left	= Mid + 1;
		}
		while(ID != CatCompArray[Mid].cca_ID && Left <= Right);

		if(ID == CatCompArray[Mid].cca_ID)
			Builtin = CatCompArray[Mid].cca_Str;
		else
			Builtin = "";
	}

	if(Catalog)
	{
		STRPTR String = GetCatalogStr(Catalog,ID,Builtin);

		if(String[0])
			return(String);
	}

	return(Builtin);
}

/******************************************************************************/

LONG SAVE_DS STACK_ARGS
PrecisionDispFunc(struct Gadget *UnusedGadget,WORD Offset)
{
	LONG Which;

	if(Offset == -1)
		Which = MSG_PRECISION1_TXT;
	else if (Offset >= PRECISION_IMAGE && Offset < PRECISION_ICON)
		Which = MSG_PRECISION2_TXT;
	else if (Offset >= PRECISION_ICON && Offset < PRECISION_GUI)
		Which = MSG_PRECISION3_TXT;
	else
		Which = MSG_PRECISION4_TXT;

	return((LONG)GetString(Which));
}

/******************************************************************************/

VOID
AddIcon(STRPTR Name)
{
	STATIC UWORD DefaultPrefsIconI1Data[] =
	{
	/* Plane 0 */
	    0x0000,0x0000,0x0004,0x0000,0x0000,0x0000,0x0001,0x0000,
	    0x0000,0x07FF,0x8000,0x4000,0x0000,0x1800,0x6000,0x1000,
	    0x0000,0x20FC,0x1000,0x0800,0x0000,0x4102,0x0800,0x0C00,
	    0x0000,0x4082,0x0800,0x0C00,0x0000,0x4082,0x0800,0x0C00,
	    0x0000,0x2104,0x0800,0x0C00,0x0000,0x1E18,0x1000,0x0C00,
	    0x0000,0x0060,0x2000,0x0C00,0x0000,0x0080,0xC000,0x0C00,
	    0x0000,0x0103,0x0000,0x0C00,0x0000,0x021C,0x0000,0x0C00,
	    0x0000,0x0108,0x0000,0x0C00,0x0000,0x00F0,0x0000,0x0C00,
	    0x0000,0x0108,0x0000,0x0C00,0x0000,0x0108,0x0000,0x0C00,
	    0x4000,0x00F0,0x0000,0x0C00,0x1000,0x0000,0x0000,0x0C00,
	    0x0400,0x0000,0x0000,0x0C00,0x01FF,0xFFFF,0xFFFF,0xFC00,
	    0x0000,0x0000,0x0000,0x0000,
	/* Plane 1 */
	    0xFFFF,0xFFFF,0xFFF8,0x0000,0xD555,0x5555,0x5556,0x0000,
	    0xD555,0x5000,0x5555,0x8000,0xD555,0x47FF,0x9555,0x6000,
	    0xD555,0x5F03,0xE555,0x5000,0xD555,0x3E55,0xF555,0x5000,
	    0xD555,0x3F55,0xF555,0x5000,0xD555,0x3F55,0xF555,0x5000,
	    0xD555,0x5E53,0xF555,0x5000,0xD555,0x4147,0xE555,0x5000,
	    0xD555,0x551F,0xD555,0x5000,0xD555,0x557F,0x1555,0x5000,
	    0xD555,0x54FC,0x5555,0x5000,0xD555,0x55E1,0x5555,0x5000,
	    0xD555,0x54F5,0x5555,0x5000,0xD555,0x5505,0x5555,0x5000,
	    0xD555,0x54F5,0x5555,0x5000,0xD555,0x54F5,0x5555,0x5000,
	    0x3555,0x5505,0x5555,0x5000,0x0D55,0x5555,0x5555,0x5000,
	    0x0355,0x5555,0x5555,0x5000,0x0000,0x0000,0x0000,0x0000,
	    0x0000,0x0000,0x0000,0x0000
	};

	STATIC struct Image DefaultPrefsIconI1 =
	{
	    0, 0,			/* Upper left corner */
	    54, 23, 2,			/* Width, Height, Depth */
	    DefaultPrefsIconI1Data,		/* Image data */
	    0x0003, 0x0000,		/* PlanePick, PlaneOnOff */
	    NULL			/* Next image */
	};

	STATIC UBYTE *DefaultPrefsIconTools[] =
	{
	    "ACTION=USE",
	    NULL
	};

	STATIC struct DiskObject DefaultPrefsIcon =
	{
	    WB_DISKMAGIC,		/* Magic Number */
	    WB_DISKVERSION,		/* Version */
	    {				/* Embedded Gadget Structure */
		NULL,			/* Next Gadget Pointer */
		0, 0, 54, 23,		/* Left,Top,Width,Height */
		GFLG_GADGIMAGE | GADGBACKFILL,	/* Flags */
		GACT_RELVERIFY | GACT_IMMEDIATE,		/* Activation Flags */
		BOOLGADGET,		/* Gadget Type */
		(APTR)&DefaultPrefsIconI1,	/* Render Image */
		NULL,			/* Select Image */
		NULL,			/* Gadget Text */
		NULL,			/* Mutual Exclude */
		NULL,			/* Special Info */
		0,			/* Gadget ID */
		NULL,			/* User Data */
	    },
	    WBPROJECT,			/* Icon Type */
	    (char *)"MagicMenuPrefs",		/* Default Tool */
	    DefaultPrefsIconTools,			/* Tool Type Array */
	    NO_ICON_POSITION,		/* Current X */
	    NO_ICON_POSITION,		/* Current Y */
	    NULL,			/* Drawer Structure */
	    NULL,			/* Tool Window */
	    0				/* Stack Size */
	};

	if(CreateIcons)
	{
		if(IconBase = OpenLibrary("icon.library",37))
		{
			struct DiskObject *Icon;

			if(Icon = GetDiskObjectNew("ENV:sys/def_pref"))
			{
				Icon->do_DefaultTool = DefaultPrefsIcon.do_DefaultTool;

				PutDiskObject(Name,Icon);

				FreeDiskObject(Icon);
			}
			else
				PutDiskObject(Name,&DefaultPrefsIcon);

			CloseLibrary(IconBase);
		}
	}
}

/******************************************************************************/

VOID
InitMMMessage(struct MMMessage *Message)
{
	memset(Message,0,sizeof(*Message));

	Message->Message.mn_ReplyPort = ReplyPort;
	Message->Message.mn_Length = sizeof(*Message);
}

BOOL
AskPrefs(struct MMPrefs *Prefs,STRPTR CxPopKey)
{
	struct MsgPort *MMPort;
	struct MMMessage Message;
	BOOL Result;

	Result = FALSE;

	InitMMMessage(&Message);

	Message.Class = MMC_GETCONFIG;

	Forbid();

	if(MMPort = FindPort(MMPORT_NAME))
	{
		PutMsg(MMPort,(struct Message *)&Message);

		WaitPort(ReplyPort);
		GetMsg(ReplyPort);

		if(Message.Class == MMC_GETCONFIG)
		{
			CopyMem(Message.Ptr1,Prefs,sizeof(*Prefs));

			if(CxPopKey)
				strncpy(CxPopKey,Message.Ptr2,99);

			Result = TRUE;
		}
	}

	Permit();

	return(Result);
}

BOOL
NewPrefs(struct MMPrefs *Prefs)
{
	struct MsgPort *MMPort;
	struct MMMessage Message;
	BOOL Result;

	Result = FALSE;

	InitMMMessage(&Message);

	Message.Class = MMC_NEWCONFIG;
	Message.Ptr1 = Prefs;
	Message.Ptr2 = CX_PopKey;

	Prefs->mmp_Magic = MMPREFS_MAGIC;
	Prefs->mmp_Version = MMPREFS_VERSION;

	Forbid();

	if(MMPort = FindPort(MMPORT_NAME))
	{
		PutMsg(MMPort,(struct Message *)&Message);

		WaitPort(ReplyPort);
		GetMsg(ReplyPort);

		if(Message.Class == MMC_NEWCONFIG)
			Result = TRUE;
	}

	Permit();

	return(Result);
}

/******************************************************************************/

VOID
SPrintf(STRPTR buffer,STRPTR formatString,...)
{
	va_list varArgs;

	va_start(varArgs,formatString);
	RawDoFmt(formatString,varArgs,(VOID (*)())"\x16\xC0\x4E\x75",buffer);
	va_end(varArgs);
}

/******************************************************************************/

VOID
UpdateGradient(struct MMPrefs *Prefs,WORD WhichPen)
{
	struct ColorWheelRGB ColorRGB,*Colours;
	struct ColorWheelHSB ColorHSB;
	struct ViewPort *VPort;
	LONG i;

	Colours = (struct ColorWheelRGB *)&Prefs->mmp_LightEdge;

	ConvertRGBToHSB(&Colours[WhichPen],&ColorHSB);

	VPort = &PubScreen->ViewPort;

	for(i = 0 ; i < GradientPensUsed ; i++)
	{
		ColorHSB.cw_Brightness = SPREAD((255 - (255 * i) / (GradientPensUsed - 1)));
		ConvertHSBToRGB(&ColorHSB,&ColorRGB);

		SetRGB32(VPort,GradientPens[i],ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
	}
}

VOID
ClampColour(ULONG *RGB,LONG Red,LONG Green,LONG Blue)
{
	if(Red > 255)
		Red = 255;
	else if (Red < 0)
		Red = 0;

	if(Green > 255)
		Green = 255;
	else if (Green < 0)
		Green = 0;

	if(Blue > 255)
		Blue = 255;
	else if (Blue < 0)
		Blue = 0;

	*RGB++ = SPREAD(Red);
	*RGB++ = SPREAD(Green);
	*RGB   = SPREAD(Blue);
}

VOID
UpdateSample(struct MMPrefs *Prefs)
{
	struct LoadThatColour LoadTable[13+1];
	struct ColorWheelRGB *Colours;
	LONG i,Red,Green,Blue;

	for(i = 0 ; i < 13 ; i++)
	{
		LoadTable[i].One	= 1;
		LoadTable[i].Which	= Pens[8+i];
	}

	LoadTable[13].One = 0;

	Colours = (struct ColorWheelRGB *)&Prefs->mmp_LightEdge;

	Red	= (Prefs->mmp_LightEdge.R >> 24) + (Prefs->mmp_DarkEdge.R >> 24);
	Green	= (Prefs->mmp_LightEdge.G >> 24) + (Prefs->mmp_DarkEdge.G >> 24);
	Blue	= (Prefs->mmp_LightEdge.B >> 24) + (Prefs->mmp_DarkEdge.B >> 24);

	ClampColour((ULONG *)&LoadTable[0].Red,(Red + 1) / 2,(Green + 1) / 2,(Blue + 1) / 2);

	Red	= Prefs->mmp_Background.R >> 24;
	Green	= Prefs->mmp_Background.G >> 24;
	Blue	= Prefs->mmp_Background.B >> 24;

	ClampColour((ULONG *)&LoadTable[1].Red,Red - 26,Green - 26,Blue - 26);
	ClampColour((ULONG *)&LoadTable[2].Red,Red,Green,Blue);
	ClampColour((ULONG *)&LoadTable[3].Red,Red + 26,Green + 26,Blue + 26);

	Red	= Prefs->mmp_FillCol.R >> 24;
	Green	= Prefs->mmp_FillCol.G >> 24;
	Blue	= Prefs->mmp_FillCol.B >> 24;

	ClampColour((ULONG *)&LoadTable[4].Red,Red - 26,Green - 26,Blue - 26);
	ClampColour((ULONG *)&LoadTable[5].Red,Red,Green,Blue);
	ClampColour((ULONG *)&LoadTable[6].Red,Red + 26,Green + 26,Blue + 26);

	for(i = 0 ; i < 6 ; i++)
	{
		LoadTable[7+i].Red	= Colours[i].cw_Red;
		LoadTable[7+i].Green	= Colours[i].cw_Green;
		LoadTable[7+i].Blue	= Colours[i].cw_Blue;
	}

	LoadRGB32(&PubScreen->ViewPort,(ULONG *)LoadTable);
}

/******************************************************************************/

VOID
ChangeWheelColour(struct MMPrefs *Prefs,WORD WhichPen)
{
	struct ColorWheelRGB *Colours;
	Object *GradientSlider;
	ULONG Max;

	Colours = (struct ColorWheelRGB *)&Prefs->mmp_LightEdge;

	LT_SetAttributes(Handle,GAD_ColorWheel,
		WHEEL_RGB,&Colours[WhichPen],
	TAG_DONE);

	Max = 0;

	LT_GetAttributes(Handle,GAD_GradientSlider,
		LABO_Object,&GradientSlider,
	TAG_DONE);

	GetAttr(GRAD_MaxVal,GradientSlider,&Max);

	if(Max > 0)
	{
		struct ColorWheelHSB ColorHSB;
		ULONG Position;

		ConvertRGBToHSB(&Colours[WhichPen],&ColorHSB);

		Position = (Max * (ColorHSB.cw_Brightness >> 24)) / 255;

		if(Position > Max)
			Position = Max,

		LT_SetAttributes(Handle,GAD_GradientSlider,
			GRAD_CurVal,Position,
		TAG_DONE);
	}

	UpdateGradient(Prefs,WhichPen);
}

VOID
UpdateRGB(struct MMPrefs *Prefs,UWORD WhichPen)
{
	struct ColorWheelRGB *Colour;

	Colour = &((struct ColorWheelRGB *)&Prefs->mmp_LightEdge)[WhichPen];

	LT_SetAttributes(Handle,GAD_Red,
		GTSL_Level,Colour->cw_Red >> 24,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_Green,
		GTSL_Level,Colour->cw_Green >> 24,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_Blue,
		GTSL_Level,Colour->cw_Blue >> 24,
	TAG_DONE);
}

VOID
UpdateHSB(struct MMPrefs *Prefs,UWORD WhichPen)
{
	struct ColorWheelHSB ColourHSB;

	ConvertRGBToHSB(&((struct ColorWheelRGB *)&Prefs->mmp_LightEdge)[WhichPen],&ColourHSB);

	LT_SetAttributes(Handle,GAD_Hue,
		GTSL_Level,ColourHSB.cw_Hue >> 24,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_Saturation,
		GTSL_Level,ColourHSB.cw_Saturation >> 24,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_Brightness,
		GTSL_Level,ColourHSB.cw_Brightness >> 24,
	TAG_DONE);
}

VOID
ChangePen(struct MMPrefs *Prefs,UWORD WhichPen)
{
	ChangeWheelColour(Prefs,WhichPen);
	UpdateRGB(Prefs,WhichPen);
	UpdateHSB(Prefs,WhichPen);
}

VOID
ChangeColour(struct MMPrefs *Prefs,WORD WhichPen)
{
	struct ColorWheelRGB *Colours;
	Object *ColorWheel;

	LT_GetAttributes(Handle,GAD_ColorWheel,
		LABO_Object,&ColorWheel,
	TAG_DONE);

	Colours = (struct ColorWheelRGB *)&Prefs->mmp_LightEdge;

	GetAttr(WHEEL_RGB,ColorWheel,(ULONG *)&Colours[WhichPen]);

	UpdateGradient(Prefs,WhichPen);
	UpdateSample(Prefs);
}

/******************************************************************************/

VOID
UpdateSlidersAndStuff(struct MMPrefs *Prefs,UWORD WhichPen,UWORD GadgetID,UWORD MsgCode)
{
	struct ColorWheelRGB *Colours;
	struct ColorWheelHSB ColorHSB;

	Colours = (struct ColorWheelRGB *)&Prefs->mmp_LightEdge;

	switch(GadgetID)
	{
		case GAD_Red:

			Colours[WhichPen].cw_Red = SPREAD(MsgCode);
			ChangeWheelColour(Prefs,WhichPen);

			UpdateHSB(Prefs,WhichPen);
			ChangeColour(Prefs,WhichPen);

			break;

		case GAD_Green:

			Colours[WhichPen].cw_Green = SPREAD(MsgCode);
			ChangeWheelColour(Prefs,WhichPen);

			UpdateHSB(Prefs,WhichPen);
			ChangeColour(Prefs,WhichPen);

			break;

		case GAD_Blue:

			Colours[WhichPen].cw_Blue = SPREAD(MsgCode);
			ChangeWheelColour(Prefs,WhichPen);

			UpdateHSB(Prefs,WhichPen);
			ChangeColour(Prefs,WhichPen);

			break;

		case GAD_Saturation:

			ConvertRGBToHSB(&Colours[WhichPen],&ColorHSB);
			ColorHSB.cw_Saturation = SPREAD(MsgCode);
			ConvertHSBToRGB(&ColorHSB,&Colours[WhichPen]);
			ChangeWheelColour(Prefs,WhichPen);

			UpdateRGB(Prefs,WhichPen);
			ChangeColour(Prefs,WhichPen);

			break;

		case GAD_Hue:

			ConvertRGBToHSB(&Colours[WhichPen],&ColorHSB);
			ColorHSB.cw_Hue = SPREAD(MsgCode);
			ConvertHSBToRGB(&ColorHSB,&Colours[WhichPen]);
			ChangeWheelColour(Prefs,WhichPen);

			UpdateRGB(Prefs,WhichPen);
			ChangeColour(Prefs,WhichPen);

			break;

		case GAD_Brightness:

			ConvertRGBToHSB(&Colours[WhichPen],&ColorHSB);
			ColorHSB.cw_Brightness = SPREAD(MsgCode);
			ConvertHSBToRGB(&ColorHSB,&Colours[WhichPen]);
			ChangeWheelColour(Prefs,WhichPen);

			UpdateRGB(Prefs,WhichPen);
			ChangeColour(Prefs,WhichPen);

			break;

		default:

			ChangeColour(Prefs,WhichPen);
			UpdateRGB(Prefs,WhichPen);
			UpdateHSB(Prefs,WhichPen);
			break;
	}
}

/******************************************************************************/

STRPTR ASM SAVE_DS
LocaleHookFunc(REG(a0) struct Hook *UnusedHook,REG(a2) APTR Unused,REG(a1) LONG ID)
{
	return(GetString(ID));
}

/******************************************************************************/

LONG ASM SAVE_DS
SampleRefreshHookFunc(REG(a0) struct Hook *UnusedHook,REG(a2) LayoutHandle *Handle,REG(a1) RefreshMsg *Message)
{
	BltBitMapRastPort(SampleMenu,0,0,Handle->Window->RPort,Message->Left,Message->Top,Message->Width,Message->Height,ABC | ABNC | NABC | NABNC);
	return(0);
}

/******************************************************************************/

VOID
ShowRequest(STRPTR Text,...)
{
	struct EasyStruct Easy;
	va_list VarArgs;

	Easy.es_StructSize	= sizeof(struct EasyStruct);
	Easy.es_Flags		= NULL;
	Easy.es_Title		= "MagicMenu";
	Easy.es_TextFormat	= Text;
	Easy.es_GadgetFormat	= "Ok";

	if(GTLayoutBase)
		LT_LockWindow(Window);

	va_start(VarArgs,Text);
	EasyRequestArgs(Window,&Easy,NULL,(APTR)VarArgs);
	va_end(VarArgs);

	if(GTLayoutBase)
		LT_UnlockWindow(Window);
}

/******************************************************************************/

VOID
ShowError(LONG ID,LONG Error)
{
	UBYTE FaultBuffer[100];

	Fault(Error,NULL,FaultBuffer,sizeof(FaultBuffer));

	ShowRequest("%s\n%s",GetString(ID),FaultBuffer);
}

/******************************************************************************/

LONG
ReadPrefs(STRPTR Name,struct MMPrefs *HerePlease)
{
	struct MMPrefs LocalPrefs;
	LONG NumItemsFound;
	LONG Error;

	memset(&LocalPrefs,0,sizeof(LocalPrefs));

	if(!(Error = RestoreData(Name,"MagicMenu",MMPREFS_VERSION,PrefsStorage,ITEM_TABLE_SIZE(PrefsStorage),&LocalPrefs,&NumItemsFound)))
	{
		CopyMem(&LocalPrefs,HerePlease,sizeof(struct MMPrefs));
	}
	else
	{
		if(NumItemsFound == 37 && Error == ERROR_REQUIRED_ARG_MISSING)
		{
			CopyMem(&LocalPrefs,HerePlease,sizeof(struct MMPrefs));
			Error = 0;
		}
	}

	return(Error);

/*
	BPTR FileHandle;
	LONG Error;

	if(!(FileHandle = Open(Name,MODE_OLDFILE)))
		Error = IoErr();
	else
	{
		struct MMPrefs LocalPrefs;

		if(Read(FileHandle,&LocalPrefs,sizeof(LocalPrefs)) != sizeof(LocalPrefs))
			Error = IoErr();
		else
		{
			if(LocalPrefs.mmp_Version == MMP_MAGIC && LocalPrefs.mmp_Size == sizeof(struct MMPrefs))
			{
				Error = 0;

				CopyMem(&LocalPrefs,HerePlease,sizeof(struct MMPrefs));
			}
			else
				Error = ERROR_OBJECT_WRONG_TYPE;
		}

		Close(FileHandle);
	}

	return(Error);
*/
}

LONG
WritePrefs(STRPTR Name,struct MMPrefs *ThisPlease)
{
	return(StoreData(Name,"MagicMenu",MMPREFS_VERSION,PrefsStorage,ITEM_TABLE_SIZE(PrefsStorage),ThisPlease));

/*
	BPTR FileHandle;
	LONG Error;

	if(!(FileHandle = Open(Name,MODE_NEWFILE)))
		Error = IoErr();
	else
	{
		if(Write(FileHandle,ThisPlease,sizeof(*ThisPlease)) == sizeof(*ThisPlease))
			Error = 0;
		else
			Error = IoErr();

		Close(FileHandle);
	}

	return(Error);
*/
}

/******************************************************************************/

VOID
DeleteBitMap(struct BitMap *BitMap, LONG Width, LONG Height)
{
	if(BitMap)
	{
		LONG i;

		WaitBlit();

		for(i = 0 ; i < BitMap->Depth ; i++)
		{
			if(BitMap->Planes[i])
				FreeRaster(BitMap->Planes[i], Width, Height);
		}

		FreeVec(BitMap);
	}
}

struct BitMap *
CreateBitMap(LONG Depth,LONG Width,LONG Height)
{
	struct BitMap *BitMap;
	LONG Extra;

	if(Depth > 8)
		Extra = sizeof(PLANEPTR) * (Depth - 8);
	else
		Extra = 0;

	if(BitMap = AllocVec(sizeof(struct BitMap) + Extra,MEMF_ANY|MEMF_CLEAR|MEMF_PUBLIC))
	{
		LONG i;

		InitBitMap(BitMap,Depth,Width,Height);

		for(i = 0 ; i < BitMap->Depth ; i++)
		{
			if(!(BitMap->Planes[i] = AllocRaster(Width,Height)))
			{
				DeleteBitMap(BitMap,Width,Height);

				return(NULL);
			}
		}
	}

	return(BitMap);
}

VOID
CreateBitMapFromImage(struct Image *Image,struct BitMap *BitMap)
{
	PLANEPTR Data;
	ULONG Modulo;
	LONG i;

	memset(BitMap,0,sizeof(*BitMap));
	InitBitMap (BitMap,Image->Depth,Image->Width,Image->Height);

	Modulo = BitMap->BytesPerRow * BitMap->Rows;
	Data = (PLANEPTR)Image->ImageData;

	for(i = 0 ; i < BitMap->Depth ; i++)
	{
		BitMap->Planes[i] = Data;
		Data += Modulo;
	}
}

VOID
RecolourBitMap (struct BitMap *Src, struct BitMap *Dst, UBYTE * Mapping, LONG DestDepth, LONG Width, LONG Height)
{
	extern VOID __asm RemapBitMap(REG(a0) struct BitMap *srcbm,REG(a1) struct BitMap *destbm,REG(a2) UBYTE *table,REG(d0) LONG width);

	WaitBlit();
	RemapBitMap(Src,Dst,Mapping,Width);
}

/******************************************************************************/

VOID
ReleasePens(struct ColorMap *ColorMap)
{
	LONG i;

	for(i = 0 ; i < COLOUR_PENS ; i++)
	{
		if(Pens[i] != -1)
		{
			ReleasePen(ColorMap,Pens[i]);
			Pens[i] = -1;
		}
	}

	for(i = 0 ; i < GradientPensUsed ; i++)
	{
		if(GradientPens[i] != -1)
		{
			ReleasePen(ColorMap,GradientPens[i]);
			GradientPens[i] = -1;
		}
	}

	GradientPensUsed = 0;
}

BOOL
AllocatePens(struct ColorMap *ColorMap)
{
	STATIC Tag Tags[] =
	{
		OBP_Precision, PRECISION_IMAGE,
		OBP_FailIfBad, TRUE,

		TAG_DONE
	};

	BOOL GotPens;
	LONG i;

	for(i = 0 ; i < 8 ; i++)
		Pens[i] = ObtainBestPenA(ColorMap,SPREAD(MagicColours[i][0]),SPREAD(MagicColours[i][1]),SPREAD(MagicColours[i][2]),(struct TagItem *)Tags);

	for(i = 8 ; i < COLOUR_PENS ; i++)
		Pens[i] = ObtainPen(ColorMap,-1,0,0,0,PEN_EXCLUSIVE|PEN_NO_SETCOLOR);

	GotPens = TRUE;

	for(i = 0 ; i < COLOUR_PENS ; i++)
	{
		if(Pens[i] == -1)
		{
			GotPens = FALSE;
			break;
		}
	}

	if(GotPens)
	{
		GradientPensUsed = 0;

		for(i = 0 ; i < GRADIENT_PENS ; i++)
		{
			GradientPens[i] = ObtainPen(ColorMap,-1,0,0,0,PEN_EXCLUSIVE|PEN_NO_SETCOLOR);

			if(GradientPens[i] != -1)
				GradientPensUsed++;
			else
			{
				if(GradientPensUsed < 2)
					GotPens = FALSE;

				break;
			}
		}
	}

	if(!GotPens)
		ReleasePens(ColorMap);

	return(GotPens);
}

/******************************************************************************/

VOID
CloseAll(VOID)
{
	DeleteMsgPort(ReplyPort);

	if(GlobalPort)
	{
		Forbid();

		RemPort(GlobalPort);
		FreeVec(GlobalPort);

		Permit();
	}

	if(Menu)
	{
		ClearMenuStrip(Handle->Window);

		LT_DisposeMenu(Menu);
	}

	if(Handle)
		LT_DeleteHandle(Handle);

	DeleteBitMap(SampleMenu,SampleMenuWidth,SampleMenuHeight);

	if(V39 && PubScreen)
		ReleasePens(PubScreen->ViewPort.ColorMap);

	if(IntuitionBase)
	{
		if(CustomScreen)
			CloseScreen(CustomScreen);
		else
			UnlockPubScreen(NULL,PubScreen);
	}

	if(FileRequester)
		FreeAslRequest(FileRequester);

	if(HomeDir)
		CurrentDir(HomeDir);

	if(Catalog)
		CloseCatalog(Catalog);

	CloseLibrary(GradientSliderBase);
	CloseLibrary(ColorWheelBase);

	CloseLibrary(LocaleBase);

	CloseLibrary(GTLayoutBase);

	CloseLibrary(AslBase);

	CloseLibrary(UtilityBase);

	CloseLibrary(GfxBase);

	CloseLibrary(IntuitionBase);
}

STRPTR
OpenAll(struct WBStartup *StartupMsg)
{
	LONG i,Error;
	BOOL NewFileName;
	struct MsgPort *SomePort;

	for(i = 0 ; i < NUM_ELEMENTS(Pens) ; i++)
		Pens[i] = -1;

	for(i = 0 ; i < NUM_ELEMENTS(GradientPens) ; i++)
		GradientPens[i] = -1;

	ProgramMode = MODE_Edit;
	CreateIcons = TRUE;
	RGB_Mode = TRUE;

	CX_Priority = 0;
	CX_PopUp = TRUE;
	strcpy(CX_PopKey,"control alt space");

	NewFileName = FALSE;

	strcpy(FileName,"ENV:MagicMenu.prefs");

	if(!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37)))
		return("intuition.library V37");

	if(!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37)))
		return("graphics.library V37");

	V39 = (BOOL)(GfxBase->LibNode.lib_Version >= 39);

	if(!(UtilityBase = OpenLibrary("utility.library",37)))
		return("utility.library V37");

	if(!(AslBase = OpenLibrary("asl.library",37)))
		return("asl.library V37");

	if(!(GTLayoutBase = SafeOpenLibrary("gtlayout.library",37)))
		return("gtlayout.library V37");

	if(LocaleBase = OpenLibrary("locale.library",38))
	{
		STATIC ULONG LocaleTags[] =
		{
			OC_BuiltInLanguage, (ULONG)"english",

			TAG_DONE
		};

		Catalog = OpenCatalogA(NULL,"magicmenu.catalog",(struct TagItem *)LocaleTags);
	}

		/* Das hier geht leider nicht anders. Das colorwheel.gadget hat eine Macke
		 * in der LibInit-Routine. Läßt sich die graphics.library V39 nicht öffnen,
		 * setzt sie noch einen Alert() ab, aber dann wird beim Umladen des Stacks
		 * zuviel vom Stack genommen. Aus diesem Zustand kommt sie nicht mehr
		 * heile heraus.
		 */

	if(V39)
	{
		ColorWheelBase = OpenLibrary("gadgets/colorwheel.gadget",39);
		GradientSliderBase = OpenLibrary("gadgets/gradientslider.gadget",39);
	}

	if(StartupMsg)
	{
		if(IconBase = OpenLibrary("icon.library",0))
		{
			struct DiskObject *Icon;
			LONG WhichArg;

			if(StartupMsg->sm_NumArgs > 1)
			{
				WhichArg = 1;

				strcpy(FileName,StartupMsg->sm_ArgList[WhichArg].wa_Name);

				NewFileName = TRUE;
			}
			else
				WhichArg = 0;

			HomeDir = CurrentDir(StartupMsg->sm_ArgList[WhichArg].wa_Lock);

			if(Icon = GetDiskObject(StartupMsg->sm_ArgList[WhichArg].wa_Name))
			{
				char *Result;

				if(WhichArg)
				{
					if(Result = FindToolType(Icon->do_ToolTypes,"ACTION"))
					{
						if(MatchToolValue(Result,"EDIT"))
							ProgramMode = MODE_Edit;
						else if (MatchToolValue(Result,"SAVE"))
							ProgramMode = MODE_Save;
						else if (MatchToolValue(Result,"USE"))
							ProgramMode = MODE_Use;
					}
				}
				else
				{
					if(FindToolType(Icon->do_ToolTypes,"EDIT"))
						ProgramMode = MODE_Edit;
					else if (FindToolType(Icon->do_ToolTypes,"SAVE"))
						ProgramMode = MODE_Save;
					else if (FindToolType(Icon->do_ToolTypes,"USE"))
						ProgramMode = MODE_Use;

					if(Result = FindToolType(Icon->do_ToolTypes,"CREATEICONS"))
					{
						if(MatchToolValue(Result,"NO"))
							CreateIcons = FALSE;
					}

					if(Result = FindToolType(Icon->do_ToolTypes,"PUBSCREEN"))
						PubScreen = LockPubScreen(Result);
				}

				if(Result = FindToolType(Icon->do_ToolTypes,"CX_PRIORITY"))
				{
					StrToLong(Result,&CX_Priority);

					if(CX_Priority < -128)
						CX_Priority = -128;
					else if (CX_Priority > 127)
						CX_Priority = 127;
				}

				if(Result = FindToolType(Icon->do_ToolTypes,"CX_POPUP"))
				{
					if(MatchToolValue(Result,"NO"))
						CX_PopUp = FALSE;
				}

				if(Result = FindToolType(Icon->do_ToolTypes,"CX_POPKEY"))
					strncpy(CX_PopKey,Result,sizeof(CX_PopKey));

				FreeDiskObject(Icon);
			}

			CloseLibrary(IconBase);
		}
	}
	else
	{
		struct
		{
			STRPTR From;
			LONG Edit;
			LONG Use;
			LONG Save;
			STRPTR PubScreen;
			LONG *CX_Priority;
			STRPTR CX_PopKey;
			STRPTR CX_PopUp;
		} Params;

		struct RDArgs *Args;

		memset(&Params,0,sizeof(Params));

		if(Args = ReadArgs("FROM,EDIT/S,USE/S,SAVE/S,PUBSCREEN/K,CX_PRIORITY/N/K,CX_POPKEY/K,CX_POPUP/K",(LONG *)&Params,NULL))
		{
			if(Params.From)
			{
				strncpy(FileName,Params.From,sizeof(FileName));
				NewFileName = TRUE;
			}

			if(Params.Edit)
				ProgramMode = MODE_Edit;
			else if (Params.Use)
				ProgramMode = MODE_Use;
			else if (Params.Save)
				ProgramMode = MODE_Save;

			if(Params.PubScreen)
				PubScreen = LockPubScreen(Params.PubScreen);

			if(Params.CX_Priority)
			{
				CX_Priority = *Params.CX_Priority;

				if(CX_Priority < -128)
					CX_Priority = -128;
				else if (CX_Priority > 127)
					CX_Priority = 127;
			}

			if(Params.CX_PopKey)
				strncpy(CX_PopKey,Params.CX_PopKey,sizeof(CX_PopKey));

			if(Params.CX_PopUp)
			{
				if(!Stricmp(Params.CX_PopUp,"NO"))
					CX_PopUp = FALSE;
			}

			FreeArgs(Args);
		}
	}

	Forbid();

	if(SomePort = FindPort("« MagicMenu Preferences »"))
	{
		Signal(SomePort->mp_SigTask,SIGBREAKF_CTRL_F);
		Permit();

		return("");
	}

	if(GlobalPort = (struct MsgPort *)AllocVec(sizeof(struct MsgPort),MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR))
	{
		GlobalPort->mp_Node.ln_Name = "« MagicMenu Preferences »";
		GlobalPort->mp_Node.ln_Pri = 1;
		GlobalPort->mp_SigTask = FindTask(NULL);
		GlobalPort->mp_Flags = PA_IGNORE;

		AddPort(GlobalPort);
	}

	Permit();

	if(!GlobalPort)
		return("global port");

	if(!(ReplyPort = CreateMsgPort()))
		return("reply port");

	if(!(FileRequester = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
		ASLFR_InitialDrawer,	"SYS:Prefs/Presets",
		ASLFR_InitialPattern,	"#?.prefs",
		ASLFR_Flags2,		FRF_REJECTICONS,
	TAG_DONE)))
		return("file requester");

	CopyMem(&DefaultPrefs,&CurrentPrefs,sizeof(CurrentPrefs));

	if(NewFileName)
	{
		Error = ReadPrefs(FileName,&CurrentPrefs);

		if(Error != 0 && Error != ERROR_OBJECT_NOT_FOUND)
		{
			ShowError(MSG_ERROR_READING_PREFERENCES_TXT,Error);

			if(ProgramMode != MODE_Edit)
				ProgramMode = MODE_Edit;
		}
	}
	else
	{
		if(!AskPrefs(&CurrentPrefs,CX_PopKey))
			ReadPrefs(FileName,&CurrentPrefs);
	}

	if(ProgramMode != MODE_Edit)
		return(NULL);

	CopyMem(&CurrentPrefs,&InitialPrefs,sizeof(CurrentPrefs));

	if(!PubScreen)
	{
		if(!(PubScreen = LockPubScreen(NULL)))
			return("public screen");
	}

	if(V39 && ColorWheelBase && GradientSliderBase)
	{
		struct Screen *WhichScreen;
		struct ColorMap *CMap;

		WhichScreen = PubScreen;

		CMap = PubScreen->ViewPort.ColorMap;

		GotPens = AllocatePens(CMap);

		if(!GotPens)
		{
/*
			struct LoadThatColour Colours[8+1];
			LONG i;

			for(i = 0 ; i < 8 ; i++)
			{
				Colours[i].One		= 1;
				Colours[i].Which	= (i < 4) ? i : (1<<5) - 4 + i - 4;
				Colours[i].Red		= SPREAD(MagicColours[i][0]);
				Colours[i].Green	= SPREAD(MagicColours[i][1]);
				Colours[i].Blue		= SPREAD(MagicColours[i][2]);
			}

			Colours[i].One = 0;
*/
			if(CustomScreen = OpenScreenTags(NULL,
				SA_LikeWorkbench,	TRUE,
				SA_Depth,		5,
				SA_Title,		VERS " (" DATE ")",
				SA_SharePens,		TRUE,
				SA_Behind,		TRUE,
/*				SA_Colors32,		Colours,*/
			TAG_DONE))
			{
				if(GotPens = AllocatePens(CustomScreen->ViewPort.ColorMap))
				{
					CMap = CustomScreen->ViewPort.ColorMap;

					WhichScreen = CustomScreen;
				}
			}
		}

		if(GotPens)
		{
			struct Image *WhichImage;
			LONG MaxPen,MaxDepth;

			MaxPen = -1;

			for(i = 0 ; i < COLOUR_PENS ; i++)
			{
				if(Pens[i] > MaxPen)
					MaxPen = Pens[i];
			}

			MaxDepth = min(8,GetBitMapAttr(WhichScreen->RastPort.BitMap,BMA_DEPTH));

			for(i = 1 ; i <= 8 ; i++)
			{
				if(MaxPen <= (1<<i)-1)
				{
					MaxDepth = i;
					break;
				}
			}

			if(WhichScreen->Font->ta_YSize >= 11)
				WhichImage = &Demo_11_Image;
			else
				WhichImage = &Demo_8_Image;

			if(SampleMenu = CreateBitMap(MaxDepth,SampleMenuWidth = WhichImage->Width,SampleMenuHeight = WhichImage->Height))
			{
				struct BitMap WhichBitMap;
				UBYTE Mapping[32];
				LONG i;

				memset(Mapping,0,sizeof(Mapping));

				for(i = 0 ; i < 21 ; i++)
					Mapping[i+1] = Pens[i];

				CreateBitMapFromImage(WhichImage,&WhichBitMap);

				RecolourBitMap(&WhichBitMap,SampleMenu,Mapping,MaxDepth,SampleMenuWidth,SampleMenuHeight);
			}
			else
				GotPens = FALSE;

			if(CustomScreen && GotPens)
			{
				UnlockPubScreen(NULL,PubScreen);
				PubScreen = CustomScreen;
				ScreenToFront(CustomScreen);
			}
		}

		if(!GotPens)
		{
			ReleasePens(CMap);

			if(CustomScreen)
			{
				CloseScreen(CustomScreen);
				CustomScreen = NULL;
			}
		}
	}

	LocaleHook.h_Entry = (HOOKFUNC)LocaleHookFunc;
	SampleRefreshHook.h_Entry = (HOOKFUNC)SampleRefreshHookFunc;

	if(GotPens)
	{
		UpdateGradient(&CurrentPrefs,WhichPen = 0);
		UpdateSample(&CurrentPrefs);
	}

	if(!(Handle = LT_CreateHandleTags(CustomScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_CloningPermitted,	FALSE,

		CustomScreen ? TAG_IGNORE : LAHN_PubScreen,PubScreen,
	TAG_DONE)))
		return("window");

	LT_New(Handle,
		LA_Type,VERTICAL_KIND,
	TAG_DONE);
	{
		STATIC BYTE InitialPage = 0;

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			STATIC LONG GroupLabelTable[] =
			{
				MSG_PAGELABEL1_TXT,
				MSG_PAGELABEL2_TXT,
				MSG_PAGELABEL3_TXT,
				-1
			};

			if(!GotPens)
				GroupLabelTable[2] = -1;

			LT_New(Handle,
				LA_Type,	TAB_KIND,
				LA_ID,		GAD_Pager,
				LA_BYTE,	&InitialPage,
				LATB_FullWidth,	TRUE,
				LATB_LabelTable,GroupLabelTable,
				LATB_AutoPageID,GAD_PageGroup,
				LATB_TabKey,	TRUE,
			TAG_DONE);

			LT_EndGroup(Handle);
		}

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
			LA_ID,		GAD_PageGroup,
			LAGR_ActivePage,InitialPage,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,HORIZONTAL_KIND,
				TAG_DONE);
				{
					STATIC LONG UsageLabelTable[] =
					{
						MSG_USAGELABEL1_TXT,
						MSG_USAGELABEL2_TXT,
						MSG_USAGELABEL3_TXT,
						-1
					};

					STATIC LONG LookLabelTable[] =
					{
						MSG_LOOKLABEL1_TXT,
						MSG_LOOKLABEL2_TXT,
						MSG_LOOKLABEL3_TXT,
						-1
					};

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
						LA_LabelID,	MSG_PULL_DOWN_GROUP_TXT,
						LAGR_NoIndent,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	CYCLE_KIND,
							LA_ID,		GAD_PDMode,
							LA_LabelID,	MSG_USAGE_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_PDMode,
							LACY_LabelTable,UsageLabelTable,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	CYCLE_KIND,
							LA_ID,		GAD_PDLook,
							LA_LabelID,	MSG_LOOK_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_PDLook,
							LACY_LabelTable,LookLabelTable,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
						LA_LabelID,	MSG_POP_UP_GROUP_TXT,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	CYCLE_KIND,
							LA_ID,		GAD_PUMode,
							LA_LabelID,	MSG_USAGE_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_PUMode,
							LACY_LabelTable,UsageLabelTable,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	CYCLE_KIND,
							LA_ID,		GAD_PULook,
							LA_LabelID,	MSG_LOOK_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_PULook,
							LACY_LabelTable,LookLabelTable,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	CHECKBOX_KIND,
							LA_ID,		GAD_PUCenter,
							LA_LabelID,	MSG_CENTRE_BOXES_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_PUCenter,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
					LA_LabelID,	MSG_GENERAL_GROUP_TXT,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						STATIC LONG TypeLabelTable[] =
						{
							MSG_TYPELABEL1_TXT,
							MSG_TYPELABEL2_TXT,
							MSG_TYPELABEL3_TXT,
							-1
						};

						STATIC LONG StyleLabelTable[] =
						{
							MSG_SEPARATOR_STYLE1_TXT,
							MSG_SEPARATOR_STYLE2_TXT,
							-1
						};

						LT_New(Handle,
							LA_Type,	CYCLE_KIND,
							LA_ID,		GAD_MenuType,
							LA_LabelID,	MSG_TYPE_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_MenuType,
							LACY_LabelTable,TypeLabelTable,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	CYCLE_KIND,
							LA_ID,		GAD_SeparatorBarStyle,
							LA_LabelID,	MSG_SEPARATOR_STYLE_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_SeparatorBarStyle,
							LACY_LabelTable,StyleLabelTable,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,BLANK_KIND,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	HORIZONTAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_MarkSub,
								LA_LabelID,	MSG_MARK_SUBMENUS_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_MarkSub,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_DblBorder,
								LA_LabelID,	MSG_DOUBLE_BORDERS_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_DblBorder,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_NonBlocking,
								LA_LabelID,	MSG_NON_BLOCKING_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_NonBlocking,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_HighlightDisabled,
								LA_LabelID,	MSG_HIGHLIGHT_DISABLED_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_HighlightDisabled,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_VerifyPatches,
								LA_LabelID,	MSG_VERIFY_PATCHES_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_VerifyPatches,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,BLANK_KIND,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_Delayed,
								LA_LabelID,	MSG_MENUS_OPEN_DELAYED_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_Delayed,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_DrawFrames,
								LA_LabelID,	MSG_DRAW_FRAME_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_DrawFrames,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_CastShadows,
								LA_LabelID,	MSG_CAST_DROP_SHADOWS_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_CastShadows,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_Transparency,
								LA_LabelID,	MSG_TRANSPARENCY_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_Transparency,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_ID,		GAD_FixPatches,
								LA_LabelID,	MSG_FIX_PATCHES_GAD,
								LA_BYTE,	&CurrentPrefs.mmp_FixPatches,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	CHECKBOX_KIND,
					LA_ID,		GAD_KCEnabled,
					LA_LabelID,	MSG_KEYBOARD_CONTROL_ENABLED_GAD,
					LA_BYTE,	&CurrentPrefs.mmp_KCEnabled,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	CHECKBOX_KIND,
					LA_ID,		GAD_KCGoTop,
					LA_LabelID,	MSG_MOVE_MOUSE_GAD,
					LA_BYTE,	&CurrentPrefs.mmp_KCGoTop,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	CHECKBOX_KIND,
					LA_ID,		GAD_KCRAltRCommand,
					LA_LabelID,	MSG_ACTIVATE_WITH_GAD,
					LA_BYTE,	&CurrentPrefs.mmp_KCRAltRCommand,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	STRING_KIND,
					LA_ID,		GAD_KCKeyStr,
					LA_LabelID,	MSG_KEY_SHORTCUT_GAD,
					LA_Chars,	20,
					LA_STRPTR,	CurrentPrefs.mmp_KCKeyStr,
					GTST_MaxChars,	199,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			if(GotPens)
			{
				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,HORIZONTAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
							LAGR_NoIndent,	TRUE,
						TAG_DONE);
						{
							STATIC LONG PenNameLabels[] =
							{
								MSG_BRIGHT_EDGES_TXT,
								MSG_DARK_EDGES_TXT,
								MSG_BACKGROUND_TXT,
								MSG_TEXT_TXT,
								MSG_SELECTED_TEXT_TXT,
								MSG_SELECTED_BACKGROUND_TXT,
								-1
							};

							LT_New(Handle,
								LA_ID,			GAD_Sample,
								LA_Type,		FRAME_KIND,
								LAFR_InnerWidth,	SampleMenuWidth,
								LAFR_InnerHeight,	SampleMenuHeight,
								LAFR_DrawBox,		TRUE,
								LAFR_RefreshHook,	&SampleRefreshHook,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,		CYCLE_KIND,
								LA_ID,			GAD_WhichPen,
								LA_WORD,		&WhichPen,
								LACY_LabelTable,	PenNameLabels,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,HORIZONTAL_KIND,
							TAG_DONE);
							{
								LT_New(Handle,
									LA_Type,		BOOPSI_KIND,
									LA_ID,			GAD_GradientSlider,
									LA_Chars,		4,
									LABO_ExactHeight,	SampleMenuHeight + 4,
									LABO_ClassInstance,	NULL,
									LABO_ClassName,		"gradientslider.gadget",
									PGA_Freedom,		LORIENT_VERT,
									GA_FollowMouse,		TRUE,
									GRAD_PenArray,		GradientPens,
									GRAD_KnobPixels,	8,
									GA_FollowMouse,		TRUE,
									GA_RelVerify,		TRUE,
								TAG_DONE);

								LT_New(Handle,
									LA_Type,		BOOPSI_KIND,
									LA_ID,			GAD_ColorWheel,
									LABO_ExactWidth,	SampleMenuHeight + 4,
									LABO_ExactHeight,	SampleMenuHeight + 4,
									LABO_ClassInstance,	NULL,
									LABO_ClassName,		"colorwheel.gadget",
									LABO_TagLink,		WHEEL_GradientSlider,
									LABO_TagScreen,		WHEEL_Screen,
									LABO_Link,		GAD_GradientSlider,
									WHEEL_RGB,		&CurrentPrefs.mmp_LightEdge,
									GA_FollowMouse,		TRUE,
									GA_RelVerify,		TRUE,
								TAG_DONE);

								LT_EndGroup(Handle);
							}

							LT_New(Handle,
								LA_ID,			GAD_ModelPager,
								LA_Type,		VERTICAL_KIND,
								LAGR_SameSize,		TRUE,
								LAGR_ActivePage,	RGB_Mode,
							TAG_DONE);
							{
								struct ColorWheelHSB ColorHSB;

								ConvertRGBToHSB(&CurrentPrefs.mmp_LightEdge,&ColorHSB);

								LT_New(Handle,
									LA_Type,	VERTICAL_KIND,
									LAGR_SameSize,	TRUE,
								TAG_DONE);
								{
									LT_New(Handle,
										LA_Type,		LEVEL_KIND,
										LA_ID,			GAD_Hue,
										LA_Chars,		10,
										LA_LabelID,		MSG_HUE_TXT,
										GTSL_Level,		ColorHSB.cw_Hue >> 24,
										GTSL_Min,		0,
										GTSL_Max,		255,
										GTSL_LevelFormat,	"%3ld",
									TAG_DONE);

									LT_New(Handle,
										LA_Type,		LEVEL_KIND,
										LA_ID,			GAD_Saturation,
										LA_LabelID,		MSG_SATURATION_TXT,
										GTSL_Level,		ColorHSB.cw_Saturation >> 24,
										GTSL_Min,		0,
										GTSL_Max,		255,
										GTSL_LevelFormat,	"%3ld",
									TAG_DONE);

									LT_New(Handle,
										LA_Type,		LEVEL_KIND,
										LA_ID,			GAD_Brightness,
										LA_LabelID,		MSG_BRIGHTNESS_TXT,
										GTSL_Level,		ColorHSB.cw_Brightness >> 24,
										GTSL_Min,		0,
										GTSL_Max,		255,
										GTSL_LevelFormat,	"%3ld",
									TAG_DONE);

									LT_EndGroup(Handle);
								}

								LT_New(Handle,
									LA_Type,	VERTICAL_KIND,
									LAGR_SameSize,	TRUE,
								TAG_DONE);
								{
									LT_New(Handle,
										LA_Type,		LEVEL_KIND,
										LA_ID,			GAD_Red,
										LA_Chars,		10,
										LA_LabelID,		MSG_RED_TXT,
										GTSL_Level,		CurrentPrefs.mmp_LightEdge.R >> 24,
										GTSL_Min,		0,
										GTSL_Max,		255,
										GTSL_LevelFormat,	"%3ld",
									TAG_DONE);

									LT_New(Handle,
										LA_Type,		LEVEL_KIND,
										LA_ID,			GAD_Green,
										LA_LabelID,		MSG_GREEN_TXT,
										GTSL_Level,		CurrentPrefs.mmp_LightEdge.G >> 24,
										GTSL_Min,		0,
										GTSL_Max,		255,
										GTSL_LevelFormat,	"%3ld",
									TAG_DONE);

									LT_New(Handle,
										LA_Type,		LEVEL_KIND,
										LA_ID,			GAD_Blue,
										LA_LabelID,		MSG_BLUE_TXT,
										GTSL_Level,		CurrentPrefs.mmp_LightEdge.B >> 24,
										GTSL_Min,		0,
										GTSL_Max,		255,
										GTSL_LevelFormat,	"%3ld",
									TAG_DONE);

									LT_EndGroup(Handle);
								}

								LT_EndGroup(Handle);
							}

							LT_EndGroup(Handle);
						}

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		LEVEL_KIND,
							LA_ID,			GAD_Precision,
							LA_WORD,		&CurrentPrefs.mmp_Precision,
							LA_LabelID,		MSG_PRECISION_GAD,
							LA_Chars,		10,
							GTSL_Min,		PRECISION_EXACT,
							GTSL_Max,		PRECISION_GUI,
							GTSL_DispFunc,		PrecisionDispFunc,
							GTSL_LevelFormat,	"%s",
							LASL_FullCheck,		TRUE,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,	CHECKBOX_KIND,
							LA_ID,		GAD_PreferScreenColours,
							LA_LabelID,	MSG_PREFER_SCREEN_COLOURS_GAD,
							LA_BYTE,	&CurrentPrefs.mmp_PreferScreenColours,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}
			}

			LT_EndGroup(Handle);
		}

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	XBAR_KIND,
				LAXB_FullSize,	TRUE,
			TAG_DONE);

			LT_EndGroup(Handle);
		}

		LT_New(Handle,
			LA_Type,	HORIZONTAL_KIND,
			LAGR_Spread,	TRUE,
			LAGR_SameSize,	TRUE,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	BUTTON_KIND,
				LA_ID,		GAD_Save,
				LA_LabelID,	MSG_SAVE_GAD,
				LABT_ExtraFat,	TRUE,
				LA_NoKey,	TRUE,
			TAG_DONE);

			LT_New(Handle,
				LA_Type,	BUTTON_KIND,
				LA_ID,		GAD_Use,
				LA_LabelID,	MSG_USE_GAD,
				LA_NoKey,	TRUE,
			TAG_DONE);

			LT_New(Handle,
				LA_Type,	BUTTON_KIND,
				LA_ID,		GAD_Test,
				LA_LabelID,	MSG_TEST_GAD,
				LA_NoKey,	TRUE,
			TAG_DONE);

			LT_New(Handle,
				LA_Type,	BUTTON_KIND,
				LA_ID,		GAD_Cancel,
				LA_LabelID,	MSG_CANCEL_GAD,
				LA_NoKey,	TRUE,
			TAG_DONE);

			LT_EndGroup(Handle);
		}

		LT_EndGroup(Handle);
	}

	if(!(Menu = LT_NewMenuTags(
		LAMN_LayoutHandle,Handle,

		LAMN_TitleID, 				MSG_PROJECT_TITLE_MEN,
			LAMN_ItemID,			MSG_OPEN_MEN,
				LAMN_ID,		MEN_Open,

			LAMN_ItemID,			MSG_SAVE_AS_MEN,
				LAMN_ID,		MEN_SaveAs,

			LAMN_ItemText,			NM_BARLABEL,

			LAMN_ItemID,			MSG_ABOUT_MEN,
				LAMN_ID,		MEN_About,

			LAMN_ItemText,			NM_BARLABEL,

			LAMN_ItemID,			MSG_QUIT_MEN,
				LAMN_ID,		MEN_Quit,

		LAMN_TitleID, 				MSG_EDIT_TITLE_MEN,
			LAMN_ItemID,			MSG_RESET_TO_DEFAULTS_MEN,
				LAMN_ID,		MEN_Reset,

			LAMN_ItemID,			MSG_LAST_SAVED_MEN,
				LAMN_ID,		MEN_LastSaved,

			LAMN_ItemID,			MSG_RESTORE_MEN,
				LAMN_ID,		MEN_Restore,

		LAMN_TitleID, 				MSG_SETTINGS_TITLE_MEN,
			LAMN_ItemID,			MSG_CREATE_ICONS_MEN,
				LAMN_ID,		MEN_CreateIcons,
				LAMN_CheckIt,		TRUE,
				LAMN_Checked,		CreateIcons,
				LAMN_Toggle,		TRUE,

			LAMN_ItemID,			MSG_SLIDER_COLOR_MODEL_MEN,
				LAMN_SubID,		MSG_MODEL_RGB_MEN,
					LAMN_ID,	MEN_RGB,
					LAMN_MutualExclude,	2,
					LAMN_CheckIt,	TRUE,
					LAMN_Checked,	RGB_Mode,

				LAMN_SubID,		MSG_MODEL_HSB_MEN,
					LAMN_ID,	MEN_HSB,
					LAMN_MutualExclude,	1,
					LAMN_CheckIt,	TRUE,
					LAMN_Checked,	!RGB_Mode,

#ifdef DEMO_MENU
		LAMN_TitleText, 		"Demo",
			LAMN_ItemText,		"Checkmark",
				LAMN_CheckIt,	TRUE,
				LAMN_Checked,	TRUE,
			LAMN_ItemText,		"M\0Shortcut",
			LAMN_ItemText,		NM_BARLABEL,
			LAMN_ItemText,		"Submenu",
				LAMN_SubText,		"Submenu Item",
			LAMN_ItemText,		"G\0Ghosted",
				LAMN_CheckIt,	TRUE,
				LAMN_Checked,	TRUE,
				LAMN_Disabled,	TRUE,
			LAMN_ItemText,		"Ghosted Sub",
				LAMN_Disabled,	TRUE,
				LAMN_SubText,		"Submenu Item",
#endif	/* DEMO_MENU */
	TAG_DONE)))
		return("menu");

	SPrintf(WindowTitle,GetString(MSG_PREFSTITLE_TXT),VERSION,REVISION,CX_PopKey);

	if(!(Window = LT_Build(Handle,
		LAWN_TitleText,		WindowTitle,
		LAWN_IDCMP,		IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK,
		LAWN_SmartZoom,		TRUE,
		LAWN_AutoRefresh,	TRUE,
		LAWN_MaxPen,		-1,
		WA_SimpleRefresh,	TRUE,
		WA_DepthGadget,		TRUE,
		WA_DragBar,		TRUE,
		WA_Activate,		TRUE,
	TAG_DONE)))
		return("window");

	SetMenuStrip(Window,Menu);

	return(NULL);
}

/******************************************************************************/

VOID
UpdateSettings(struct MMPrefs *Prefs)
{
	SPrintf(WindowTitle,GetString(MSG_PREFSTITLE_TXT),VERSION,REVISION,CX_PopKey);

	SetWindowTitles(Window,WindowTitle,(STRPTR)~0);

	LT_SetAttributes(Handle,GAD_PDMode,
		GTCY_Active,	Prefs->mmp_PDMode,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PDLook,
		GTCY_Active,	Prefs->mmp_PDLook,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PUMode,
		GTCY_Active,	Prefs->mmp_PUMode,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PULook,
		GTCY_Active,	Prefs->mmp_PULook,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PUCenter,
		GTCB_Checked,	Prefs->mmp_PUCenter,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_MenuType,
		GTCY_Active,	Prefs->mmp_MenuType,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_MarkSub,
		GTCB_Checked,	Prefs->mmp_MarkSub,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_DblBorder,
		GTCB_Checked,	Prefs->mmp_DblBorder,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_NonBlocking,
		GTCB_Checked,	Prefs->mmp_NonBlocking,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_KCEnabled,
		GTCB_Checked,	Prefs->mmp_KCEnabled,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_KCGoTop,
		GTCB_Checked,	Prefs->mmp_KCGoTop,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_KCRAltRCommand,
		GTCB_Checked,	Prefs->mmp_KCRAltRCommand,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PreferScreenColours,
		GTCB_Checked,	Prefs->mmp_PreferScreenColours,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_Delayed,
		GTCB_Checked,	Prefs->mmp_Delayed,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_DrawFrames,
		GTCB_Checked,	Prefs->mmp_DrawFrames,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_CastShadows,
		GTCB_Checked,	Prefs->mmp_CastShadows,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_KCKeyStr,
		GTST_String,	Prefs->mmp_KCKeyStr,
	TAG_DONE);

	CopyMem(&Prefs->mmp_LightEdge,&CurrentPrefs.mmp_LightEdge,6 * sizeof(CurrentPrefs.mmp_LightEdge));

	if(GotPens)
	{
		LT_SetAttributes(Handle,GAD_Precision,
			GTSL_Level,	Prefs->mmp_Precision,
		TAG_DONE);

		LT_SetAttributes(Handle,GAD_WhichPen,
			GTCY_Active,WhichPen = 0,
		TAG_DONE);

		ChangeWheelColour(&CurrentPrefs,WhichPen);
		UpdateRGB(&CurrentPrefs,WhichPen);
		UpdateHSB(&CurrentPrefs,WhichPen);

		UpdateSample(&CurrentPrefs);
		UpdateGradient(&CurrentPrefs,WhichPen);
	}
}

/******************************************************************************/

VOID
EventLoop(struct WBStartup *StartupMsg)
{
	ULONG Signals,WindowMask;
	BOOL Done;
	LONG Error;

	WindowMask = 1L<<Handle->Window->UserPort->mp_SigBit;

	Done = FALSE;

	do
	{
		Signals = Wait(WindowMask | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F);

		if(Signals & (SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_E))
			break;

		if(Signals & SIGBREAKF_CTRL_F)
			LT_ShowWindow(Handle,TRUE);

		if(Signals & WindowMask)
		{
			struct IntuiMessage *Message;
			struct MenuItem *Item;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			UWORD MsgCode;
			WORD MsgX,MsgY;

			while(Message = LT_GetIMsg(Handle))
			{
				MsgClass	= Message->Class;
				MsgCode		= Message->Code;
				MsgGadget	= Message->IAddress;
				MsgX		= Message->MouseX;
				MsgY		= Message->MouseY;

				LT_ReplyIMsg(Message);

				switch(MsgClass)
				{
					case IDCMP_CLOSEWINDOW:

						Done = TRUE;
						break;

					case IDCMP_GADGETUP:
					case IDCMP_GADGETDOWN:

						switch(MsgGadget->GadgetID)
						{
							case GAD_Test:

								LT_LockWindow(Window);

								Forbid();

								if(!FindPort(MMPORT_NAME))
								{
									BPTR In,Out;

									Permit();

									if(In = Open("NIL:",MODE_NEWFILE))
									{
										if(Out = Open("NIL:",MODE_NEWFILE))
										{
											if(StartupMsg && !Cli())
												AttachCLI(StartupMsg);

											SystemTags("MagicMenu",
												SYS_Input,	In,
												SYS_Output,	Out,
												SYS_Asynch,	TRUE,
												NP_Name,	"MagicMenu",
											TAG_DONE);
										}
										else
											Close(In);
									}
								}
								else
									Permit();

								TestPrefsValid = AskPrefs(&TestPrefs,NULL);
								NewPrefs(&CurrentPrefs);

								LT_UnlockWindow(Window);

								break;

							case GAD_Save:

								if(Error = WritePrefs("ENVARC:MagicMenu.prefs",&CurrentPrefs))
								{
									ShowError(MSG_ERROR_WRITING_PREFERENCES_TXT,Error);
									break;
								}

								/* Falls thru to... */

							case GAD_Use:

								WritePrefs("ENV:MagicMenu.prefs",&CurrentPrefs);
								NewPrefs(&CurrentPrefs);
								TestPrefsValid = FALSE;

								/* Falls thru to... */

							case GAD_Cancel:

								Done = TRUE;
								break;

							case GAD_WhichPen:

								if(GotPens)
									ChangePen(&CurrentPrefs,MsgCode);

								break;

							default:

								if(GotPens)
								{
									UpdateSlidersAndStuff(&CurrentPrefs,WhichPen,MsgGadget->GadgetID,MsgCode);

									if(MsgClass == IDCMP_GADGETUP && GTLayoutBase->lib_Version >= 47 && V39)
									{
										if((GAD_Red <= MsgGadget->GadgetID && MsgGadget->GadgetID <= GAD_Brightness) || MsgGadget->GadgetID == GAD_GradientSlider || MsgGadget->GadgetID == GAD_ColorWheel)
										{
											if(GetBitMapAttr(Handle->Window->RPort->BitMap,BMA_DEPTH) > 8)
												LT_Redraw(Handle,GAD_Sample);
										}
									}
								}

								break;
						}

						break;

					case IDCMP_MOUSEBUTTONS:

						if(MsgCode == SELECTDOWN && GotPens)
						{
							LONG Pen;

							if(Pen = ReadPixel(Handle->Window->RPort,MsgX,MsgY))
							{
								LONG i;

								for(i = 15 ; i <= 20 ; i++)
								{
									if(Pens[i] == Pen)
									{
										LT_SetAttributes(Handle,GAD_WhichPen,
											GTCY_Active,i - 15,
										TAG_DONE);

										ChangePen(&CurrentPrefs,i - 15);
										break;
									}
								}
							}
						}

						break;

					case IDCMP_MOUSEMOVE:

						if(GotPens)
							UpdateSlidersAndStuff(&CurrentPrefs,WhichPen,MsgGadget->GadgetID,MsgCode);

						break;

					case IDCMP_MENUPICK:

						while(MsgCode != MENUNULL)
						{
							if(Item = ItemAddress(Menu,MsgCode))
							{
								switch(LAMN_Item_ID(Item))
								{
									case MEN_About:

										ShowRequest(GetString(MSG_ABOUT_TXT),VERSION,REVISION,DATE);
										break;

									case MEN_Open:

										LT_LockWindow(Window);

										if(AslRequestTags(FileRequester,
											ASLFR_Window,		Window,
											ASLFR_TitleText,	GetString(MSG_OPEN_PREFERENCES_TXT),
											ASLFR_Flags1,		FRF_DOPATTERNS | FRF_PRIVATEIDCMP,
										TAG_DONE))
										{
											if(FileRequester->fr_File[0])
											{
												UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

												strncpy(LocalBuffer,FileRequester->fr_Drawer,sizeof(LocalBuffer));

												if(AddPart(LocalBuffer,FileRequester->fr_File,sizeof(LocalBuffer)))
												{
													Error = ReadPrefs(LocalBuffer,&CurrentPrefs);

													if(Error != 0)
														ShowError(MSG_ERROR_READING_PREFERENCES_TXT,Error);
													else
														UpdateSettings(&CurrentPrefs);
												}
											}
										}

										LT_UnlockWindow(Window);

										break;

									case MEN_SaveAs:

										LT_LockWindow(Window);

										if(AslRequestTags(FileRequester,
											ASLFR_Window,		Window,
											ASLFR_TitleText,	GetString(MSG_SAVE_PREFERENCES_TXT),
											ASLFR_Flags1,		FRF_DOPATTERNS | FRF_PRIVATEIDCMP | FRF_DOSAVEMODE,
										TAG_DONE))
										{
											if(FileRequester->fr_File[0])
											{
												UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

												strncpy(LocalBuffer,FileRequester->fr_Drawer,sizeof(LocalBuffer));

												if(AddPart(LocalBuffer,FileRequester->fr_File,sizeof(LocalBuffer)))
												{
													Error = WritePrefs(LocalBuffer,&CurrentPrefs);

													if(Error != 0)
														ShowError(MSG_ERROR_WRITING_PREFERENCES_TXT,Error);
													else
														AddIcon(LocalBuffer);
												}
											}
										}

										LT_UnlockWindow(Window);

										break;

									case MEN_Quit:

										Done = TRUE;
										break;

									case MEN_Reset:

										CopyMem(&DefaultPrefs,&CurrentPrefs,sizeof(CurrentPrefs));

										UpdateSettings(&CurrentPrefs);
										break;

									case MEN_LastSaved:

										LT_LockWindow(Window);

										Error = ReadPrefs("ENVARC:MagicMenu.prefs",&CurrentPrefs);

										if(Error != 0)
											ShowError(MSG_ERROR_READING_PREFERENCES_TXT,Error);
										else
											UpdateSettings(&CurrentPrefs);

										LT_UnlockWindow(Window);

										break;

									case MEN_Restore:

										CopyMem(&InitialPrefs,&CurrentPrefs,sizeof(CurrentPrefs));

										UpdateSettings(&CurrentPrefs);
										break;

									case MEN_CreateIcons:

										if(Item->Flags & CHECKED)
											CreateIcons = TRUE;
										else
											CreateIcons = FALSE;

										break;

									case MEN_RGB:

										LT_SetAttributes(Handle,GAD_ModelPager,
											LAGR_ActivePage,RGB_Mode = TRUE,
										TAG_DONE);
										break;

									case MEN_HSB:

										LT_SetAttributes(Handle,GAD_ModelPager,
											LAGR_ActivePage,RGB_Mode = FALSE,
										TAG_DONE);
										break;
								}

								MsgCode = Item->NextSelect;
							}
						}

						break;
				}
			}
		}
	}
	while(!Done);

	if(TestPrefsValid)
		NewPrefs(&TestPrefs);
}

/******************************************************************************/

int
main(int argc,char **argv)
{
	struct WBStartup *StartupMsg;
	STRPTR Result;
	LONG Error;
	int ReturnCode;

	if(SysBase->LibNode.lib_Version < 37)
	{
		((struct Process *)FindTask(NULL))->pr_Result2 = ERROR_INVALID_RESIDENT_LIBRARY;
		return(RETURN_FAIL);
	}

	SetProgramName("MagicMenuPrefs");

	ReturnCode = RETURN_OK;

	if(argc)
		StartupMsg = NULL;
	else
		StartupMsg = (struct WBStartup *)argv;

	Result = OpenAll(StartupMsg);

	if(Result)
	{
		if(Result[0])
		{
			if(argc)
			{
				Printf("MagicMenu: ");
				Printf(GetString(MSG_SETUP_FAILURE_TXT),Result);
				Printf("\n");
			}
			else
			{
				if(IntuitionBase)
					ShowRequest(GetString(MSG_SETUP_FAILURE_TXT),Result);
			}

			ReturnCode = RETURN_FAIL;
		}
	}
	else
	{
		switch(ProgramMode)
		{
			case MODE_Edit:

				EventLoop(StartupMsg);
				break;

			case MODE_Save:

				if(Error = WritePrefs("ENVARC:MagicMenu.prefs",&CurrentPrefs))
					ShowError(MSG_ERROR_WRITING_PREFERENCES_TXT,Error);
				else
				{
					WritePrefs("ENV:MagicMenu.prefs",&CurrentPrefs);
					NewPrefs(&CurrentPrefs);
				}

				break;

			case MODE_Use:

				NewPrefs(&CurrentPrefs);
				break;
		}
	}

	CloseAll();

	return(ReturnCode);
}
