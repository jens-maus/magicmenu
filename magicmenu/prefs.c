/*
**	$Id$
**
**	:ts=4
*/

#ifndef _PREFS_GLOBAL_H
#include "prefs_global.h"
#endif	/* _PREFS_GLOBAL_H */

#ifdef _M68030
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") 68030 version\r\n";
#else
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") Generic 68k version\r\n";
#endif // _M68030

#define MAJORVERS	   "2.0"

enum	{	GAD_PAGER=1000,GAD_PAGEGROUP,
			GAD_PULLDOWN_MODE,GAD_PULLDOWN_LOOK,
			GAD_POPUP_MODE,GAD_POPUP_LOOK,GAD_POPUP_CENTRE,
			GAD_KEY_ENABLED,GAD_KEY_MOVEMOUSE,GAD_KEY_SEQUENCE,
			GAD_KEY_RALTRCOMMAND,
			GAD_TYPE,GAD_MARK_SUBMENUS,GAD_DOUBLE_BORDERS,GAD_TIMEOUT,
			GAD_DIRECTDRAW,GAD_USELAYER,GAD_CHUNKY, GAD_NONBLOCKING,
			GAD_SAVE,GAD_USE,GAD_CANCEL,GAD_REMOVE, GAD_COLORPEN, GAD_GRADSLID, GAD_COLORWHEEL,
			GAD_SAMPLEMENU,
			MEN_ABOUT,MEN_OPEN,MEN_SAVEAS,MEN_QUIT,MEN_RESET,MEN_LASTSAVED,MEN_RESTORE,
			MEN_ENABLE,MEN_DISABLE,MEN_REMOVE
		};

struct Library *
SafeOpenLibrary(STRPTR Name,LONG Version)
{
	struct Library *Base;

	Forbid();

	if(Base = (struct Library *)FindName(&SysBase -> LibList,FilePart(Name)))
	{
		if(Base -> lib_Version < Version)
			RemLibrary(Base);
	}

	Permit();

	return(OpenLibrary(Name,Version));
}

VOID
CloseSettingsGUI()
{
	UWORD	z1;

	if(GUI_Menu)
	{
		ClearMenuStrip(GUI_Handle -> Window);

		LT_DisposeMenu(GUI_Menu);

		GUI_Menu = NULL;
	}

	if(GUI_Handle)
	{
		LT_DeleteHandle(GUI_Handle);

		GUI_Handle = NULL;
	}

	if(SampleMenuImageRmp)
	{
		FreeRemappedImage(SampleMenuImageRmp);
		SampleMenuImageRmp = NULL;
	}

	if(GUI_Screen)
	{
		MyReleasePen(GUI_Screen,MenGhostHiCol);
		MyReleasePen(GUI_Screen,MenGhostLoCol);
		MyReleasePen(GUI_Screen,MenFillCol);
		MyReleasePen(GUI_Screen,MenHiCol);
		MyReleasePen(GUI_Screen,MenTextCol);
		MyReleasePen(GUI_Screen,MenBackground);
		MyReleasePen(GUI_Screen,MenDarkEdge);
		MyReleasePen(GUI_Screen,MenLightEdge);
		MyReleasePen(GUI_Screen,MenXENGrey0);
		MyReleasePen(GUI_Screen,MenXENBlack);
		MyReleasePen(GUI_Screen,MenXENWhite);
		MyReleasePen(GUI_Screen,MenXENBlue);
		MyReleasePen(GUI_Screen,MenXENGrey1);
		MyReleasePen(GUI_Screen,MenXENGrey2);
		MyReleasePen(GUI_Screen,MenXENBeige);
		MyReleasePen(GUI_Screen,MenXENPink);

		MenGhostHiCol = MenGhostLoCol = MenFillCol = MenHiCol = MenTextCol = MenBackground =
		MenDarkEdge = MenLightEdge = MenXENGrey0 = MenXENBlack = MenXENWhite = MenXENBlue =
		MenXENGrey1 = MenXENGrey2 = MenXENBeige = MenXENPink = (UWORD)~0;

		z1 = 0;

		while(GradientPens[z1] != (UWORD)~0)
		{
			MyReleasePen(GUI_Screen,GradientPens[z1]);
			GradientPens[z1] = (UWORD)~0;
			z1++;
		}

		GradPens = 0;

		PensAllocated = FALSE;

		UnlockPubScreen(NULL,GUI_Screen);
		GUI_Screen = NULL;
	}
}


static __asm __saveds
DrawSampleMenuImage(
	register __a0 struct Hook *MyHook,
	register __a2 struct LayoutHandle *GUI_Handle,
	register __a1 RefreshMsg *Msg)
{
	DrawImage(GUI_Handle->Window->RPort,SampleMenuImageRmp,Msg->Left,Msg->Top);
	return(0);
}

static struct Hook DrawSampleMenuHook = {{NULL}, (HOOKFUNC)DrawSampleMenuImage};

BOOL
UpdateSettingsGUI(void)
{
	UWORD			z1;
	struct ColorWheelHSB	ColorHSB;
	struct ColorWheelRGB	ColorRGB;

	if(GUI_Handle)
	{
	LT_SetAttributes(GUI_Handle, GAD_PULLDOWN_MODE,
		GTCY_Active,			AktPrefs.mmp_PDMode,
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_PULLDOWN_LOOK,
		GTCY_Active,			AktPrefs.mmp_PDLook,
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_POPUP_MODE,
		GTCY_Active,			AktPrefs.mmp_PUMode,
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_POPUP_LOOK,
		GTCY_Active,			AktPrefs.mmp_PULook,
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_POPUP_CENTRE,
		GTCB_Checked,			(AktPrefs.mmp_PUCenter == 1),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_KEY_ENABLED,
		GTCB_Checked,			(AktPrefs.mmp_KCEnabled == 1),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_KEY_MOVEMOUSE,
		GTCB_Checked,			(AktPrefs.mmp_KCGoTop == 1),
		GA_Disabled,			(AktPrefs.mmp_KCEnabled == 0),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_KEY_RALTRCOMMAND,
		GTCB_Checked,			(AktPrefs.mmp_KCRAltRCommand == 1),
		GA_Disabled,			(AktPrefs.mmp_KCEnabled == 0),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_KEY_SEQUENCE,
		GTST_String,			AktPrefs.mmp_KCKeyStr,
		GA_Disabled,			(AktPrefs.mmp_KCEnabled == 0),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_NONBLOCKING,
		GTCB_Checked,			(AktPrefs.mmp_NonBlocking == 1),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_DIRECTDRAW,
		GTCB_Checked,			(AktPrefs.mmp_DirectDraw == 0),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_USELAYER,
		GTCB_Checked,			(AktPrefs.mmp_UseLayer == 1 &&
						 AktPrefs.mmp_DirectDraw == 0),
		GA_Disabled,			(AktPrefs.mmp_DirectDraw == 1),
	TAG_DONE);

	if(GfxVersion >= 39)
	{
		LT_SetAttributes(GUI_Handle, GAD_CHUNKY,
			GTCB_Checked,		(AktPrefs.mmp_ChunkyPlanes == 1),
		TAG_DONE);
	}

	LT_SetAttributes(GUI_Handle, GAD_MARK_SUBMENUS,
		GTCB_Checked,			(AktPrefs.mmp_MarkSub == 1),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_DOUBLE_BORDERS,
		GTCB_Checked,			(AktPrefs.mmp_DblBorder == 1),
	TAG_DONE);

	LT_SetAttributes(GUI_Handle, GAD_TIMEOUT,
		GTIN_Number,			AktPrefs.mmp_TimeOut,
	TAG_DONE);

	ColorWheelRGB = *(struct ColorWheelRGB *)AktPrefsCol;

	if(PensAllocated)
	{
		SetRGB32(&GUI_Screen->ViewPort,MenLightEdge,AktPrefs.mmp_LightEdge.R,AktPrefs.mmp_LightEdge.G,AktPrefs.mmp_LightEdge.B);
		SetRGB32(&GUI_Screen->ViewPort,MenDarkEdge,AktPrefs.mmp_DarkEdge.R,AktPrefs.mmp_DarkEdge.G,AktPrefs.mmp_DarkEdge.B);
		SetRGB32(&GUI_Screen->ViewPort,MenBackground,AktPrefs.mmp_Background.R,AktPrefs.mmp_Background.G,AktPrefs.mmp_Background.B);
		SetRGB32(&GUI_Screen->ViewPort,MenTextCol,AktPrefs.mmp_TextCol.R,AktPrefs.mmp_TextCol.G,AktPrefs.mmp_TextCol.B);
		SetRGB32(&GUI_Screen->ViewPort,MenHiCol,AktPrefs.mmp_HiCol.R,AktPrefs.mmp_HiCol.G,AktPrefs.mmp_HiCol.B);
		SetRGB32(&GUI_Screen->ViewPort,MenFillCol,AktPrefs.mmp_FillCol.R,AktPrefs.mmp_FillCol.G,AktPrefs.mmp_FillCol.B);
		SetRGB32(&GUI_Screen->ViewPort,MenGhostLoCol,AktPrefs.mmp_GhostLoCol.R,AktPrefs.mmp_GhostLoCol.G,AktPrefs.mmp_GhostLoCol.B);
		SetRGB32(&GUI_Screen->ViewPort,MenGhostHiCol,AktPrefs.mmp_GhostHiCol.R,AktPrefs.mmp_GhostHiCol.G,AktPrefs.mmp_GhostHiCol.B);
	}
	else
	{
		MyReleasePen(GUI_Screen,*AktPen);

		*AktPen = MyObtainPen(GUI_Screen,AktPrefsCol->R,AktPrefsCol->G,AktPrefsCol->B);

		RemapArray[8] = MenLightEdge;
		RemapArray[9] = MenDarkEdge;
		RemapArray[10] = MenBackground;
		RemapArray[11] = MenTextCol;
		RemapArray[12] = MenHiCol;
		RemapArray[13] = MenFillCol;
		RemapArray[14] = MenGhostLoCol;
		RemapArray[15] = MenGhostHiCol;

		if(SampleMenuImageRmp)
		FreeRemappedImage(SampleMenuImageRmp);

		SampleMenuImageRmp = NULL;

		if(! MakeRemappedImage(&SampleMenuImageRmp,&SampleMenuImage,4,GUI_Screen->RastPort.BitMap->Depth,RemapArray))
		return(FALSE);
	}

	ConvertRGBToHSB((struct ColorWheelRGB *)AktPrefsCol,&ColorHSB);
	if(GradPens > 2)
	{
		z1 = 0;

		while (z1 < GradPens)
		{
		ColorHSB.cw_Brightness = 0xffffffff - ((0xffffffff / GradPens) * z1);
		ConvertHSBToRGB(&ColorHSB,&ColorRGB);

		SetRGB32(&GUI_Screen->ViewPort,GradientPens[z1],
				   ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
		z1++;
		}
	}
	else if(GradPens == 2)
	{
		ColorHSB.cw_Brightness = 0xffffffff;
		ConvertHSBToRGB(&ColorHSB,&ColorRGB);

		SetRGB32(&GUI_Screen->ViewPort,GradientPens[0],
			   ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
	}
	else
	{
		MyReleasePen(GUI_Screen,GradientPens[0]);
		MyReleasePen(GUI_Screen,GradientPens[1]);
		ColorHSB.cw_Brightness = 0xffffffff;
		ConvertHSBToRGB(&ColorHSB,&ColorRGB);
		GradientPens[0] = MyObtainPen(GUI_Screen,ColorRGB.cw_Red, ColorRGB.cw_Green, ColorRGB.cw_Blue);
		GradientPens[1] = MyObtainPen(GUI_Screen,0, 0, 0);
		GradientPens[2] = (UWORD)~0;
		LT_SetAttributes(GUI_Handle,GAD_GRADSLID,
					GRAD_PenArray,	GradientPens,
					TAG_DONE);
	}

	if(ColorWheelBase && GradientSliderBase)
	{
		LT_SetAttributes(GUI_Handle, GAD_COLORWHEEL,
			WHEEL_RGB,		&ColorWheelRGB,
		TAG_DONE);
	}

	if(! LT_RebuildTags(GUI_Handle,FALSE,TAG_DONE))
		return(FALSE);

	return(TRUE);
	}
	return(FALSE);
}


ULONG
OpenSettingsGUI()
{
	BOOL		Ok;
	ULONG		*LPtrD, z1;
	struct		ColorWheelRGB	ColorRGB;
	struct		ColorWheelHSB	ColorHSB;
	struct		ColorMap		*ColorMap;

	MenGhostHiCol = MenGhostLoCol = MenFillCol = MenHiCol = MenTextCol = MenBackground =
		MenDarkEdge = MenLightEdge = MenXENGrey0 = MenXENBlack = MenXENWhite = MenXENBlue =
		MenXENGrey1 = MenXENGrey2 = MenXENBeige = MenXENPink = (UWORD)~0;

	GradientPens[0] = (UWORD)~0;

	if(! (GUI_Screen = LockPubScreen(NULL)))
	{
		CloseSettingsGUI();
		return(0);
	}

	PensAllocated = FALSE;

	ColorMap = GUI_Screen->ViewPort.ColorMap;

	if(GUI_Screen->RastPort.BitMap->Depth >= 4 && GfxVersion >= 39 && ColorWheelBase && GradientSliderBase &&
	   ColorMap->Type >= COLORMAP_TYPE_V39 && ColorMap->PalExtra && ColorMap->PalExtra->pe_NFree >= 8)
	{
		Ok = (MenLightEdge = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_LightEdge.R,AktPrefs.mmp_LightEdge.G,AktPrefs.mmp_LightEdge.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenDarkEdge = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_DarkEdge.R,AktPrefs.mmp_DarkEdge.G,AktPrefs.mmp_DarkEdge.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenBackground = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_Background.R,AktPrefs.mmp_Background.G,AktPrefs.mmp_Background.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenTextCol = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_TextCol.R,AktPrefs.mmp_TextCol.G,AktPrefs.mmp_TextCol.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenHiCol = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_HiCol.R,AktPrefs.mmp_HiCol.G,AktPrefs.mmp_HiCol.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenFillCol = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_FillCol.R,AktPrefs.mmp_FillCol.G,AktPrefs.mmp_FillCol.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenGhostLoCol = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_GhostLoCol.R,AktPrefs.mmp_GhostLoCol.G,AktPrefs.mmp_GhostLoCol.B,PEN_EXCLUSIVE)) != -1;
		if(Ok)
		Ok = (MenGhostHiCol = ObtainPen(GUI_Screen->ViewPort.ColorMap,-1,AktPrefs.mmp_GhostHiCol.R,AktPrefs.mmp_GhostHiCol.G,AktPrefs.mmp_GhostHiCol.B,PEN_EXCLUSIVE)) != -1;

		if(Ok)
		PensAllocated = TRUE;

		if(Ok && ColorMap->PalExtra->pe_NFree > 10)
		{
		GradPens = min(ColorMap->PalExtra->pe_NFree / 8,12);
		ConvertRGBToHSB((struct ColorWheelRGB *)&AktPrefs.mmp_LightEdge,&ColorHSB);
		ColorHSB.cw_Brightness = 0xffffffff;

		z1 = 0;
		while(z1 < GradPens)
		{
			ColorHSB.cw_Brightness = 0xffffffff - ((0xffffffff / GradPens) * z1);
			ConvertHSBToRGB(&ColorHSB,&ColorRGB);

			GradientPens[z1] = ObtainPen(ColorMap,-1,
					 ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue,PEN_EXCLUSIVE);

			if (GradientPens[z1] == -1)
			break;

			z1++;
		}
		GradientPens[z1] = (UWORD)~0;
		}
		else
		GradPens = 0;
	}

	if(! PensAllocated)
	{
		MenLightEdge = MyObtainPen(GUI_Screen,AktPrefs.mmp_LightEdge.R, AktPrefs.mmp_LightEdge.G, AktPrefs.mmp_LightEdge.B);
		MenDarkEdge = MyObtainPen(GUI_Screen,AktPrefs.mmp_DarkEdge.R, AktPrefs.mmp_DarkEdge.G, AktPrefs.mmp_DarkEdge.B);
		MenBackground = MyObtainPen(GUI_Screen,AktPrefs.mmp_Background.R, AktPrefs.mmp_Background.G, AktPrefs.mmp_Background.B);
		MenTextCol = MyObtainPen(GUI_Screen,AktPrefs.mmp_TextCol.R, AktPrefs.mmp_TextCol.G, AktPrefs.mmp_TextCol.B);
		MenHiCol = MyObtainPen(GUI_Screen,AktPrefs.mmp_HiCol.R, AktPrefs.mmp_HiCol.G, AktPrefs.mmp_HiCol.B);
		MenFillCol = MyObtainPen(GUI_Screen,AktPrefs.mmp_FillCol.R, AktPrefs.mmp_FillCol.G, AktPrefs.mmp_FillCol.B);
		MenGhostLoCol = MyObtainPen(GUI_Screen,AktPrefs.mmp_GhostLoCol.R, AktPrefs.mmp_GhostLoCol.G, AktPrefs.mmp_GhostLoCol.B);
		MenGhostHiCol = MyObtainPen(GUI_Screen,AktPrefs.mmp_GhostHiCol.R, AktPrefs.mmp_GhostHiCol.G, AktPrefs.mmp_GhostHiCol.B);
	}

	MenXENGrey0 = MyObtainPen(GUI_Screen,0x95000000, 0x95000000, 0x95000000);
	MenXENBlack = MyObtainPen(GUI_Screen,0x00000000, 0x00000000, 0x00000000);
	MenXENWhite = MyObtainPen(GUI_Screen,0xffffffff, 0xffffffff, 0xffffffff);
	MenXENBlue	= MyObtainPen(GUI_Screen,0x3b000000, 0x67000000, 0xA3000000);
	MenXENGrey1 = MyObtainPen(GUI_Screen,0x7b000000, 0x7b000000, 0x7b000000);
	MenXENGrey2 = MyObtainPen(GUI_Screen,0xaf000000, 0xaf000000, 0xaf000000);
	MenXENBeige = MyObtainPen(GUI_Screen,0xaa000000, 0x90000000, 0x7c000000);
	MenXENPink	= MyObtainPen(GUI_Screen,0xffffffff, 0xa9000000, 0x97000000);

	if(GradPens == 0 && ColorWheelBase && GradientSliderBase)
	{
		ConvertRGBToHSB((struct ColorWheelRGB *)&AktPrefs.mmp_LightEdge,&ColorHSB);
		ColorHSB.cw_Brightness = 0xffffffff;
		ConvertHSBToRGB(&ColorHSB,&ColorRGB);

		GradientPens[0] = ObtainPen(ColorMap,-1,
				 ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue,PEN_EXCLUSIVE);

		if (GradientPens[0] != -1)
		{
		GradientPens[1] = MyObtainPen(GUI_Screen,0, 0, 0);
		GradientPens[2] = (UWORD)~0;
		GradPens = 2;
		}
	}

	if(GradPens == 0 && ColorWheelBase && GradientSliderBase)
	{
		ConvertRGBToHSB((struct ColorWheelRGB *)&AktPrefs.mmp_LightEdge,&ColorHSB);
		ColorHSB.cw_Brightness = 0xffffffff;
		ConvertHSBToRGB(&ColorHSB,&ColorRGB);
		GradientPens[0] = MyObtainPen(GUI_Screen,ColorRGB.cw_Red, ColorRGB.cw_Green, ColorRGB.cw_Blue);
		GradientPens[1] = MyObtainPen(GUI_Screen,0, 0, 0);
		GradientPens[2] = (UWORD)~0;
	}

	for(LPtrD = (ULONG *)RemapArray, z1 = 0; z1 < 256 / 4; z1++)
		*LPtrD++ = 0;

	RemapArray[0] = MenXENGrey0;
	RemapArray[1] = MenXENBlack;
	RemapArray[2] = MenXENWhite;
	RemapArray[3] = MenXENBlue;
	RemapArray[4] = MenXENGrey1;
	RemapArray[5] = MenXENGrey2;
	RemapArray[6] = MenXENBeige;
	RemapArray[7] = MenXENPink;
	RemapArray[8] = MenLightEdge;
	RemapArray[9] = MenDarkEdge;
	RemapArray[10] = MenBackground;
	RemapArray[11] = MenTextCol;
	RemapArray[12] = MenHiCol;
	RemapArray[13] = MenFillCol;
	RemapArray[14] = MenGhostLoCol;
	RemapArray[15] = MenGhostHiCol;

	if(! MakeRemappedImage(&SampleMenuImageRmp,&SampleMenuImage,4,GUI_Screen->RastPort.BitMap->Depth,RemapArray))
	{
		CloseSettingsGUI();
		return(0);
	}


	if(GUI_Handle = LT_CreateHandleTags(GUI_Screen,
	TAG_DONE))
	{
		LT_New(GUI_Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			STATIC BYTE InitialPage = 0;

			LT_New(GUI_Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				static STRPTR Labels[] =
				{
					"Look and usage",
					"Keyboard control",
					"Miscellaneous",
					"Color wheel",
					NULL
				};

				LT_New(GUI_Handle,
					LA_Type,			TAB_KIND,
					LATB_FullWidth, 	TRUE,
					LA_ID,				GAD_PAGER,
					LABT_ExtraFat,		TRUE,
					GTCY_Labels,		Labels,
					LACY_AutoPageID,	GAD_PAGEGROUP,
					LACY_TabKey,		TRUE,
					LA_BYTE,			&InitialPage,
					GA_RelVerify,		TRUE,
					GA_Immediate,		TRUE,
				TAG_DONE);

				LT_EndGroup(GUI_Handle);
			}

			LT_New(GUI_Handle,
				LA_Type,				VERTICAL_KIND,
				LA_ID,					GAD_PAGEGROUP,
				LAGR_ActivePage,		InitialPage,
			TAG_DONE);
			{
				STATIC STRPTR ModeLabels[] =
				{
					"Intuition compatible",
					"Sticky mouse button",
					"Smart select",
					NULL
				};

				STATIC STRPTR LookLabels[] =
				{
					"Standard",
					"3D old",
					"3D MultiColor",
					NULL
				};

				LT_New(GUI_Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(GUI_Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelText,	"Pull Down menu",
					TAG_DONE);
					{
						LT_New(GUI_Handle,
							LA_Type,		CYCLE_KIND,
							LA_ID,			GAD_PULLDOWN_MODE,
							LA_LabelText,	"Usage",
							GTCY_Labels,	ModeLabels,
							GTCY_Active,	AktPrefs.mmp_PDMode,
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CYCLE_KIND,
							LA_ID,			GAD_PULLDOWN_LOOK,
							LA_LabelText,	"Look",
							GTCY_Labels,	LookLabels,
							GTCY_Active,	AktPrefs.mmp_PDLook,
						TAG_DONE);

						LT_EndGroup(GUI_Handle);
					}

					LT_New(GUI_Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelText,	"Pop Up menu",
					TAG_DONE);
					{
						LT_New(GUI_Handle,
							LA_Type,		CYCLE_KIND,
							LA_ID,			GAD_POPUP_MODE,
							LA_LabelText,	"Usage",
							GTCY_Labels,	ModeLabels,
							GTCY_Active,	AktPrefs.mmp_PUMode,
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CYCLE_KIND,
							LA_ID,			GAD_POPUP_LOOK,
							LA_LabelText,	"Look",
							GTCY_Labels,	LookLabels,
							GTCY_Active,	AktPrefs.mmp_PULook,
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_POPUP_CENTRE,
							LA_LabelText,	"Center boxes",
							GTCB_Checked,	(AktPrefs.mmp_PUCenter == 1),
						TAG_DONE);

						LT_EndGroup(GUI_Handle);
					}

					LT_EndGroup(GUI_Handle);
				}

				LT_New(GUI_Handle,
					LA_Type,	HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(GUI_Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelText,	"Key control",
					TAG_DONE);
					{
						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_KEY_ENABLED,
							LA_LabelText,	"Enabled",
							GTCB_Checked,	(AktPrefs.mmp_KCEnabled == 1),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_KEY_MOVEMOUSE,
							LA_LabelText,	"Move mouse to menu bar",
							GTCB_Checked,	(AktPrefs.mmp_KCGoTop == 1),
							GA_Disabled,	(AktPrefs.mmp_KCEnabled == 0),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_KEY_RALTRCOMMAND,
							LA_LabelText,	"Activate with right Amiga-Alt",
							GTCB_Checked,	(AktPrefs.mmp_KCRAltRCommand == 1),
							GA_Disabled,	(AktPrefs.mmp_KCEnabled == 0),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		STRING_KIND,
							LA_ID,			GAD_KEY_SEQUENCE,
							LA_LabelText,	"Start sequence",
							GTST_String,	AktPrefs.mmp_KCKeyStr,
							GTST_MaxChars,	255,
							LA_Chars,		35,
							GA_Disabled,	(AktPrefs.mmp_KCEnabled == 0),
						TAG_DONE);

						LT_EndGroup(GUI_Handle);
					}

					LT_EndGroup(GUI_Handle);
				}

				LT_New(GUI_Handle,
					LA_Type,	HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(GUI_Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelText,	"Drawing methods",
					TAG_DONE);
					{
						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_NONBLOCKING,
							LA_LabelText,	"Non blocking",
							GTCB_Checked,	(AktPrefs.mmp_NonBlocking == 1),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_DIRECTDRAW,
							LA_LabelText,	"Draw in background",
							GTCB_Checked,	(AktPrefs.mmp_DirectDraw == 0),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_USELAYER,
							LA_LabelText,	"Clipping",
							GTCB_Checked,	(AktPrefs.mmp_UseLayer == 1 &&
									 AktPrefs.mmp_DirectDraw == 0),
							GA_Disabled,	(AktPrefs.mmp_DirectDraw == 1),
						TAG_DONE);

						if(GfxVersion >= 39)
						{
							LT_New(GUI_Handle,
								LA_Type,		CHECKBOX_KIND,
								LA_ID,			GAD_CHUNKY,
								LA_LabelText,	"Chunky mode (CyberGraphX)",
								GTCB_Checked,	(AktPrefs.mmp_ChunkyPlanes == 1),
							TAG_DONE);
						}

						LT_EndGroup(GUI_Handle);
					}

					LT_New(GUI_Handle,
						LA_Type,		VERTICAL_KIND,
						LA_LabelText,	"Miscellaneous",
					TAG_DONE);
					{
						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_MARK_SUBMENUS,
							LA_LabelText,	"Mark submenus",
							GTCB_Checked,	(AktPrefs.mmp_MarkSub == 1),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_ID,			GAD_DOUBLE_BORDERS,
							LA_LabelText,	"Double borders",
							GTCB_Checked,	(AktPrefs.mmp_DblBorder == 1),
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,				INTEGER_KIND,
							LA_ID,				GAD_TIMEOUT,
							LA_LabelText,			"Menu timeout",
							LA_Chars,				3,
							LAIN_Min,				1,
							LAIN_Max,				129,
							GTIN_MaxChars,			3,
							STRINGA_Justification,		STRINGRIGHT,
							GTIN_Number,			AktPrefs.mmp_TimeOut,
							LAIN_UseIncrementers,		TRUE,
						TAG_DONE);

						LT_EndGroup(GUI_Handle);
					}

					LT_EndGroup(GUI_Handle);
				}

				LT_New(GUI_Handle,
					LA_Type,	HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(GUI_Handle,
						LA_Type,		HORIZONTAL_KIND,
						LA_LabelText,	"MultiColor selection",
					TAG_DONE);
					{
						LT_New(GUI_Handle,
						LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
						STATIC STRPTR PenLabels[] =
						{
							"Light Edge",
							"Dark Edge",
							"Background",
							"Text",
							"Highlighted text",
							"Highlighted background",
							"Ghosted Dark",
							"Ghosted Light",
							NULL
						};

						LT_New(GUI_Handle,
							LA_Type,		CYCLE_KIND,
							LA_ID,			GAD_COLORPEN,
							/*LA_LabelText, 	  "Element",*/
							GTCY_Labels,		PenLabels,
							GTCY_Active,		0,
						TAG_DONE);

						LT_New(GUI_Handle,
							LA_Type,		FRAME_KIND,
							LAFR_InnerWidth,	SampleMenuImage.Width,
							LAFR_InnerHeight,	SampleMenuImage.Height,
							LAFR_DrawBox,		TRUE,
							LAFR_RefreshHook,	&DrawSampleMenuHook,
						TAG_DONE);

						LT_EndGroup(GUI_Handle);
						}

						AktPen = &MenLightEdge;
						AktPrefsCol = &AktPrefs.mmp_LightEdge;

						ColorWheelRGB = *(struct ColorWheelRGB *)AktPrefsCol;

						LT_New(GUI_Handle,
						LA_Type,	HORIZONTAL_KIND,
						LA_ExtraSpace,	TRUE,
						TAG_DONE);
						{
						if(ColorWheelBase && GradientSliderBase)
						{
							LT_New(GUI_Handle,
								LA_Type,		BOOPSI_KIND,
								LA_ID,		GAD_GRADSLID,
								LA_Chars,		4,
								LABO_ExactHeight,	SampleMenuImage.Height,
								LABO_ClassInstance, NULL,
								LABO_ClassName, "gradientslider.gadget",
								PGA_Freedom,	LORIENT_VERT,
								GA_FollowMouse, TRUE,
								GRAD_PenArray,	GradientPens,
								GRAD_KnobPixels,	8,
								GA_RelVerify,	TRUE,
							TAG_DONE);

							LT_New(GUI_Handle,
								LA_Type,		BOOPSI_KIND,
								LA_ID,		GAD_COLORWHEEL,
								LABO_ExactHeight,	SampleMenuImage.Height,
								LABO_ExactWidth,	SampleMenuImage.Height,
								LABO_ClassInstance, NULL,
								LABO_ClassName, "colorwheel.gadget",
								LABO_TagLink,	WHEEL_GradientSlider,
								LABO_TagScreen, WHEEL_Screen,
								LABO_Link,		GAD_GRADSLID,
								WHEEL_RGB,		&ColorWheelRGB,
								GA_FollowMouse, TRUE,
								GA_RelVerify,	TRUE,
							TAG_DONE);
						}

						LT_EndGroup(GUI_Handle);
						}

						LT_EndGroup(GUI_Handle);
					}

					LT_EndGroup(GUI_Handle);
				}

				LT_EndGroup(GUI_Handle);
			}

			LT_New(GUI_Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				STATIC STRPTR TypeLabels[] =
				{
					"Use pull-down menu only",
					"Use pop-up menu only",
					"Use pull-down menu when pointer in menu bar, pop-up otherwise",
					NULL
				};

				LT_New(GUI_Handle,
					LA_Type,XBAR_KIND,
				TAG_DONE);

				LT_New(GUI_Handle,
					LA_Type,		CYCLE_KIND,
					LA_ID,			GAD_TYPE,
					LA_LabelText,	"Type",
					GTCY_Labels,	TypeLabels,
					GTCY_Active,	AktPrefs.mmp_MenuType,
				TAG_DONE);

				LT_EndGroup(GUI_Handle);
			}

			LT_New(GUI_Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(GUI_Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(GUI_Handle);
			}

			LT_New(GUI_Handle,
				LA_Type,		HORIZONTAL_KIND,
				LAGR_Spread,	TRUE,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(GUI_Handle,
					LA_Type,		BUTTON_KIND,
					LA_ID,			GAD_SAVE,
					LA_LabelText,	"Save",
					LABT_ExtraFat,	TRUE,
					LABT_ReturnKey, TRUE,
				TAG_DONE);

				LT_New(GUI_Handle,
					LA_Type,		BUTTON_KIND,
					LA_ID,			GAD_USE,
					LA_LabelText,	"Use",
				TAG_DONE);

				LT_New(GUI_Handle,
					LA_Type,		BUTTON_KIND,
					LA_ID,			GAD_REMOVE,
					LA_LabelText,	"Remove",
				TAG_DONE);

				LT_New(GUI_Handle,
					LA_Type,		BUTTON_KIND,
					LA_ID,			GAD_CANCEL,
					LA_LabelText,	"Cancel",
					LABT_EscKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(GUI_Handle);
			}

			LT_EndGroup(GUI_Handle);
		}

		if(LT_Build(GUI_Handle,
			LAWN_TitleText, 	WindowTitle,
			LAWN_IDCMP, 	IDCMP_CLOSEWINDOW | IDCMP_MENUPICK | IDCMP_MOUSEMOVE,
			LAWN_SmartZoom, 	TRUE,
			LAWN_Show,			TRUE,
			LAWN_MoveToWindow,	TRUE,
			LAWN_AutoRefresh,	TRUE,
			WA_SimpleRefresh,	TRUE,
			WA_DepthGadget, 	TRUE,
			WA_CloseGadget, 	TRUE,
			WA_DragBar, 		TRUE,
			WA_Activate,		TRUE,
			WA_RMBTrap, 		TRUE,
		TAG_DONE))
		{
			if(GUI_Menu = LT_NewMenuTags(
				LAMN_Screen,			GUI_Handle -> Screen,

				LAMN_TitleText, 		"Project",
					LAMN_ItemText,		"?\0About...",
						LAMN_ID,	MEN_ABOUT,
					LAMN_ItemText,		NM_BARLABEL,
					LAMN_ItemText,		"O\0Open...",
						LAMN_ID,	MEN_OPEN,
					LAMN_ItemText,		"A\0Save as...",
						LAMN_ID,	MEN_SAVEAS,
					LAMN_ItemText,		NM_BARLABEL,
					LAMN_ItemText,		"Q\0Quit",
						LAMN_ID,	MEN_QUIT,
				LAMN_TitleText, 		"Edit",
					LAMN_ItemText,		"Z\0Reset to defaults",
						LAMN_ID,	MEN_RESET,
					LAMN_ItemText,		"L\0Last saved",
						LAMN_ID,	MEN_LASTSAVED,
					LAMN_ItemText,		"S\0Restore",
						LAMN_ID,	MEN_RESTORE,
				LAMN_TitleText, 		"Control",
					LAMN_ItemText,		"+\0Enable",
						LAMN_ID,	MEN_ENABLE,
						LAMN_MutualExclude, 0x2,
						LAMN_CheckIt,	TRUE,
						LAMN_Checked,	MMEnabled,
					LAMN_ItemText,		"-\0Disable",
						LAMN_ID,	MEN_DISABLE,
						LAMN_MutualExclude, 0x1,
						LAMN_CheckIt,	TRUE,
						LAMN_Checked,	! MMEnabled,
					LAMN_ItemText,		NM_BARLABEL,
					LAMN_ItemText,		"Remove",
						LAMN_ID,	MEN_REMOVE,
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

			TAG_DONE))
			{
				SetMenuStrip(GUI_Handle -> Window,GUI_Menu);

				GUI_Handle -> Window -> Flags &= ~WFLG_RMBTRAP;

				return(1UL << GUI_Handle -> Window -> UserPort -> mp_SigBit);
			}
		}
	}

	CloseSettingsGUI();

	return(0);
}

BOOL
HandleSettingsGUI(struct IntuiMessage *Msg)
{
	BOOL Ende;
	UWORD MenuNum, ItemNum;
	UWORD SelectMenu;
	struct MenuItem *MenuAddr;
	struct Gadget *MsgGadget;
	ULONG MsgClass;
	ULONG MsgQualifier;
	UWORD MsgCode;
	ULONG MenId, z1;
	struct		ColorWheelRGB	ColorRGB;
	struct		ColorWheelHSB	ColorHSB;
	APTR		colorwheel;

	Ende = FALSE;

	MsgClass		= Msg -> Class;
	MsgQualifier		= Msg -> Qualifier;
	MsgCode 		= Msg -> Code;
	MsgGadget		= Msg -> IAddress;

	LT_ReplyIMsg (Msg);

	switch (MsgClass)
	{
		case IDCMP_MENUPICK:

			SelectMenu = MsgCode;

			while (SelectMenu != MENUNULL)
			{
				MenuNum = MENUNUM (SelectMenu);
				ItemNum = ITEMNUM (SelectMenu);
				MenuAddr = ItemAddress (GUI_Menu, SelectMenu);
				MenId = (ULONG)LAMN_Item_ID(MenuAddr);

				switch (MenId)
				{
					case MEN_ABOUT:

						LT_LockWindow(GUI_Handle->Window);
						About ();
						LT_UnlockWindow(GUI_Handle->Window);

						break;

					case MEN_OPEN:

						LT_LockWindow(GUI_Handle->Window);
						LoadPrefsAs();
						LT_UnlockWindow(GUI_Handle->Window);

						Ende = ! UpdateSettingsGUI();
						break;

					case MEN_SAVEAS:
						LT_LockWindow(GUI_Handle->Window);
						SavePrefsAs();
						LT_UnlockWindow(GUI_Handle->Window);

						break;

					case MEN_QUIT:
						Ende = TRUE;
						break;

					case MEN_RESET:

						AktPrefs = DefaultPrefs;
						Ende = ! UpdateSettingsGUI();

						break;

					case MEN_LASTSAVED:

						LT_LockWindow(GUI_Handle->Window);
						LoadPrefs(ConfigFile);
						LT_UnlockWindow(GUI_Handle->Window);

						Ende = ! UpdateSettingsGUI();
						break;

					case MEN_RESTORE:

						AskPrefs();
						Ende = ! UpdateSettingsGUI();

						break;

					case MEN_ENABLE:

						SendEnable(TRUE);
						break;

					case MEN_DISABLE:

						SendEnable(FALSE);
						break;

					case MEN_REMOVE:

						SendRemove();
						break;
				}

				SelectMenu = MenuAddr->NextSelect;
			}

			break;

		case IDCMP_GADGETUP:
		case IDCMP_GADGETDOWN:

			switch (MsgGadget->GadgetID)
			{
				case GAD_SAVE:

					NewPrefs();
					LT_LockWindow(GUI_Handle->Window);
					SavePrefs(ConfigFile);
					LT_UnlockWindow(GUI_Handle->Window);
					Ende = TRUE;
					break;

				case GAD_CANCEL:

					Ende = TRUE;
					break;

				case GAD_USE:

					NewPrefs();
					Ende = TRUE;
					break;

				case GAD_REMOVE:
					SendRemove();
					Ende = TRUE;
					break;

				case GAD_PAGER:

					if(MsgCode == 3)
					{
						if(! PensAllocated)
						{
						LT_LockWindow(GUI_Handle->Window);

						SimpleRequest(GUI_Handle->Window,
									 "MagicMenu Prefs","This dialogue needs at least eight allocatable pens\n"
											   "to work correctly.\n"
											   "Unfortunately this screen has not enough colors.\n\n"
											   "The dialogue will still work but much less smooth.\n\n"
											   "Please refer to the manual for details.","Ok",NULL,0,0);

						LT_UnlockWindow(GUI_Handle->Window);
						}
					}
					break;

				case GAD_KEY_SEQUENCE:

					strncpy (AktPrefs.mmp_KCKeyStr, (char *)LT_GetAttributes(GUI_Handle,GAD_KEY_SEQUENCE,TAG_DONE), 199);

					break;

				case GAD_POPUP_CENTRE:

					AktPrefs.mmp_PUCenter = (MsgCode) ? 1 : 0;
					break;

				case GAD_KEY_ENABLED:

					AktPrefs.mmp_KCEnabled = (MsgCode) ? 1 : 0;

					LT_SetAttributes(GUI_Handle, GAD_KEY_MOVEMOUSE,
						GA_Disabled, (AktPrefs.mmp_KCEnabled == 0),
					TAG_DONE);

					LT_SetAttributes(GUI_Handle, GAD_KEY_SEQUENCE,
						GA_Disabled, (AktPrefs.mmp_KCEnabled == 0),
					TAG_DONE);

					LT_SetAttributes(GUI_Handle, GAD_KEY_RALTRCOMMAND,
						GA_Disabled, (AktPrefs.mmp_KCEnabled == 0),
					TAG_DONE);

					break;

				case GAD_KEY_MOVEMOUSE:

					AktPrefs.mmp_KCGoTop = (MsgCode) ? 1 : 0;
					break;

				case GAD_KEY_RALTRCOMMAND:

					AktPrefs.mmp_KCRAltRCommand = (MsgCode) ? 1 : 0;

					break;

				case GAD_TYPE:

					AktPrefs.mmp_MenuType = MsgCode;

					break;

				case GAD_MARK_SUBMENUS:

					AktPrefs.mmp_MarkSub = (MsgCode) ? 1 : 0;
					break;

				case GAD_DOUBLE_BORDERS:

					AktPrefs.mmp_DblBorder = (MsgCode) ? 1 : 0;
					break;

				case GAD_DIRECTDRAW:

					AktPrefs.mmp_DirectDraw = (MsgCode) ? 0 : 1;

					LT_SetAttributes(GUI_Handle, GAD_USELAYER,
						GA_Disabled,	(AktPrefs.mmp_DirectDraw == 1),
						GTCB_Checked,	(AktPrefs.mmp_UseLayer == 1 &&
								 AktPrefs.mmp_DirectDraw == 0),
					TAG_DONE);

					break;

				case GAD_USELAYER:

					AktPrefs.mmp_UseLayer = (MsgCode) ? 1 : 0;
					break;

				case GAD_NONBLOCKING:

					AktPrefs.mmp_NonBlocking = (MsgCode) ? 1 : 0;
					break;

				case GAD_CHUNKY:

					AktPrefs.mmp_ChunkyPlanes = (MsgCode) ? 1 : 0;
					break;

				case GAD_PULLDOWN_MODE:

					AktPrefs.mmp_PDMode = MsgCode;
					break;

				case GAD_POPUP_MODE:

					AktPrefs.mmp_PUMode = MsgCode;
					break;

				case GAD_PULLDOWN_LOOK:

					AktPrefs.mmp_PDLook = MsgCode;
					break;

				case GAD_POPUP_LOOK:

					AktPrefs.mmp_PULook = MsgCode;
					break;

				case GAD_TIMEOUT:

					AktPrefs.mmp_TimeOut = LT_GetAttributes(GUI_Handle,GAD_TIMEOUT,TAG_DONE);
					break;

				case GAD_COLORPEN:

					switch(MsgCode)
					{
						case 0:
						AktPen = &MenLightEdge;
						AktPrefsCol = &AktPrefs.mmp_LightEdge;
						break;
						case 1:
						AktPen = &MenDarkEdge;
						AktPrefsCol = &AktPrefs.mmp_DarkEdge;
						break;
						case 2:
						AktPen = &MenBackground;
						AktPrefsCol = &AktPrefs.mmp_Background;
						break;
						case 3:
						AktPen = &MenTextCol;
						AktPrefsCol = &AktPrefs.mmp_TextCol;
						break;
						case 4:
						AktPen = &MenHiCol;
						AktPrefsCol = &AktPrefs.mmp_HiCol;
						break;
						case 5:
						AktPen = &MenFillCol;
						AktPrefsCol = &AktPrefs.mmp_FillCol;
						break;
						case 6:
						AktPen = &MenGhostLoCol;
						AktPrefsCol = &AktPrefs.mmp_GhostLoCol;
						break;
						case 7:
						AktPen = &MenGhostHiCol;
						AktPrefsCol = &AktPrefs.mmp_GhostHiCol;
						break;
					}

					if(GradientSliderBase && ColorWheelBase)
					{
						ConvertRGBToHSB((struct ColorWheelRGB *)AktPrefsCol,&ColorHSB);
						if(GradPens > 2)
						{
						z1 = 0;

						while (z1 < GradPens)
						{
							ColorHSB.cw_Brightness = 0xffffffff - ((0xffffffff / GradPens) * z1);
							ConvertHSBToRGB(&ColorHSB,&ColorRGB);

							SetRGB32(&GUI_Screen->ViewPort,GradientPens[z1],
								   ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
							z1++;
						}
						}
						else if(GradPens == 2)
						{
						ColorHSB.cw_Brightness = 0xffffffff;
						ConvertHSBToRGB(&ColorHSB,&ColorRGB);

						SetRGB32(&GUI_Screen->ViewPort,GradientPens[0],
								   ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
						}
						else
						{
						MyReleasePen(GUI_Screen,GradientPens[0]);
						MyReleasePen(GUI_Screen,GradientPens[1]);
						ConvertRGBToHSB((struct ColorWheelRGB *)AktPrefsCol,&ColorHSB);
						ColorHSB.cw_Brightness = 0xffffffff;
						ConvertHSBToRGB(&ColorHSB,&ColorRGB);
						GradientPens[0] = MyObtainPen(GUI_Screen,ColorRGB.cw_Red, ColorRGB.cw_Green, ColorRGB.cw_Blue);
						GradientPens[1] = MyObtainPen(GUI_Screen,0, 0, 0);
						GradientPens[2] = (UWORD)~0;
						LT_SetAttributes(GUI_Handle,GAD_GRADSLID,
									GRAD_PenArray,	GradientPens,
									TAG_DONE);
						}

						ColorWheelRGB = *(struct ColorWheelRGB *)AktPrefsCol;

						LT_SetAttributes(GUI_Handle,GAD_COLORWHEEL,
								WHEEL_RGB,	&ColorWheelRGB,
								TAG_DONE);
					}
					break;

				case GAD_COLORWHEEL:
				case GAD_GRADSLID:
					/*DisplayBeep(NULL);*/
					LT_GetAttributes(GUI_Handle,GAD_COLORWHEEL,
							LABO_Object,	  &colorwheel,
							TAG_DONE);

					if(GradientSliderBase && ColorWheelBase && colorwheel)
					{
						GetAttr(WHEEL_RGB,colorwheel,(ULONG *)&ColorRGB);
						*AktPrefsCol = *(struct RGBSet *)&ColorRGB;

						ColorWheelRGB = *(struct ColorWheelRGB *)AktPrefsCol;

						if(! PensAllocated)
						{
						MyReleasePen(GUI_Screen,*AktPen);

						*AktPen = MyObtainPen(GUI_Screen,AktPrefsCol->R,AktPrefsCol->G,AktPrefsCol->B);

						RemapArray[8] = MenLightEdge;
						RemapArray[9] = MenDarkEdge;
						RemapArray[10] = MenBackground;
						RemapArray[11] = MenTextCol;
						RemapArray[12] = MenHiCol;
						RemapArray[13] = MenFillCol;
						RemapArray[14] = MenGhostLoCol;
						RemapArray[15] = MenGhostHiCol;

						if(SampleMenuImageRmp)
							FreeRemappedImage(SampleMenuImageRmp);

						SampleMenuImageRmp = NULL;

						if(! MakeRemappedImage(&SampleMenuImageRmp,&SampleMenuImage,4,GUI_Screen->RastPort.BitMap->Depth,RemapArray))
							Ende = TRUE;
						else
						{
							if(GradPens != 0)
							if(! LT_RebuildTags(GUI_Handle,FALSE,TAG_DONE))
								Ende = TRUE;
						}
						}
						if(! Ende && GradPens == 0)
						{
						MyReleasePen(GUI_Screen,GradientPens[0]);
						MyReleasePen(GUI_Screen,GradientPens[1]);
						ConvertRGBToHSB(&ColorWheelRGB,&ColorHSB);
						ColorHSB.cw_Brightness = 0xffffffff;
						ConvertHSBToRGB(&ColorHSB,&ColorRGB);
						GradientPens[0] = MyObtainPen(GUI_Screen,ColorRGB.cw_Red, ColorRGB.cw_Green, ColorRGB.cw_Blue);
						GradientPens[1] = MyObtainPen(GUI_Screen,0, 0, 0);
						GradientPens[2] = (UWORD)~0;
						LT_SetAttributes(GUI_Handle,GAD_GRADSLID,
									GRAD_PenArray,	GradientPens,
									TAG_DONE);
						if(! LT_RebuildTags(GUI_Handle,FALSE,TAG_DONE))
							Ende = TRUE;
						}
					}
					break;
			}

			break;

		case IDCMP_MOUSEMOVE:
			LT_GetAttributes(GUI_Handle,GAD_COLORWHEEL,
					LABO_Object,	  &colorwheel,
					TAG_DONE);

			if(GradientSliderBase && ColorWheelBase && colorwheel)
			{
				GetAttr(WHEEL_RGB,colorwheel,(ULONG *)&ColorRGB);
				*AktPrefsCol = *(struct RGBSet *)&ColorRGB;

				ColorWheelRGB = *(struct ColorWheelRGB *)AktPrefsCol;

				if(PensAllocated)
				SetRGB32(&GUI_Screen->ViewPort,*AktPen,ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);

				ConvertRGBToHSB(&ColorRGB,&ColorHSB);
				if(GradPens > 2)
				{
				z1 = 0;

				while (z1 < GradPens)
				{
					ColorHSB.cw_Brightness = 0xffffffff - ((0xffffffff / GradPens) * z1);
					ConvertHSBToRGB(&ColorHSB,&ColorRGB);

					SetRGB32(&GUI_Screen->ViewPort,GradientPens[z1],
						   ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
					z1++;
				}
				}
				else if(GradPens == 2)
				{
				ColorHSB.cw_Brightness = 0xffffffff;
				ConvertHSBToRGB(&ColorHSB,&ColorRGB);

				SetRGB32(&GUI_Screen->ViewPort,GradientPens[0],
						   ColorRGB.cw_Red,ColorRGB.cw_Green,ColorRGB.cw_Blue);
				}
			}

			break;

		case IDCMP_CLOSEWINDOW:

			Ende = TRUE;
			break;
	}

	return (Ende);
}


void MainLoop(void)
{
	BOOL Ende;
	struct IntuiMessage *Msg;
	ULONG  Sigs;

	do
	{
	Ende = FALSE;

	Sigs = 0;

	while(! (Msg = LT_GetIMsg(GUI_Handle)) && (Sigs & SIGBREAKF_CTRL_C) == 0)
		Sigs = Wait(1l << GUI_Handle->Window->UserPort->mp_SigBit | SIGBREAKF_CTRL_C);

	if((Sigs & SIGBREAKF_CTRL_C) != 0)
		return;

	Ende = HandleSettingsGUI(Msg);
	}
	while(! Ende);
}


void
About (void)
{
  char ZwStr[500];

  SPrintf (ZwStr, "MagicMenu Prefs " MAJORVERS " Rev. %ld.%ld (%s)\n\nMartin Korndörfer\n&\nOlaf 'Olsen' Barthel\n\nGTLayout.library © Olaf Barthel\n\n"
		  "Martin Korndörfer\nPommernstraße 15\nD-86916 Kaufering, Germany\n\nOlaf Barthel\nBrabeckstraße 35\n"
		  "D-30559 Hannover, Germany\n\nE-Mail:\nm.korndoerfer@nathan.gun.de\nolsen@sourcery.han.de", VERSION,REVISION,DATE);

  SimpleRequest (NULL, "MagicMenu Prefs", ZwStr, "Ok", NULL, 0, 0);
}


VOID __stdargs
SPrintf(STRPTR buffer, STRPTR formatString,...)
{
	va_list varArgs;

	va_start(varArgs,formatString);
	RawDoFmt(formatString,varArgs,(VOID (*)())(VOID (*))"\x16\xC0\x4E\x75",buffer);
	va_end(varArgs);
}


/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*		   Supportfunktionen für MAIN					*/
/* */
/* ************************************************************************ */
/* ************************************************************************ */


BOOL
MakeRemappedImage (struct Image **DestImage, struct Image *SrcImage,
		   UWORD SrcDepth, UWORD Depth, UBYTE *RemapArray)

{
  UWORD WordsPerLine;
  ULONG PlaneSize, PlaneOffs;
  UBYTE SrcPlanes, Plane;
  UWORD Line, Column, Color, NewColor;

  if (*DestImage = (struct Image *) AllocVecPooled (sizeof (struct Image), MEMF_PUBLIC))
  {
	**DestImage = *SrcImage;
	(*DestImage)->ImageData = NULL;
	if (SrcImage->Depth == 0 || SrcImage->ImageData == NULL)
	{
	  (*DestImage)->Depth = 0;
	  (*DestImage)->PlaneOnOff = RemapArray[SrcImage->PlaneOnOff];
	  return (TRUE);
	}
	(*DestImage)->PlanePick = 0xff;
	(*DestImage)->PlaneOnOff = 0;

	WordsPerLine = (SrcImage->Width + 15) / 16;
	PlaneSize = WordsPerLine * SrcImage->Height;
	SrcPlanes = SrcImage->Depth;
	if(SrcPlanes > SrcDepth)
	  SrcPlanes = SrcDepth;
	if ((*DestImage)->ImageData =
	(UWORD *) AllocVec (PlaneSize * Depth * sizeof (UWORD), MEMF_CHIP | MEMF_PUBLIC | MEMF_CLEAR))
	{
	  (*DestImage)->Depth = Depth;
	  for (Line = 0; Line < SrcImage->Height; Line++)
	for (Column = 0; Column < SrcImage->Width; Column++)
	{
	  PlaneOffs = Line * WordsPerLine + Column / 16;
	  Color = 0;
	  for (Plane = 0; Plane < SrcPlanes; Plane++)
		Color |= ((SrcImage->ImageData[Plane * PlaneSize + PlaneOffs] >> (15 - Column % 16)) & 1) << Plane;

	  NewColor = RemapArray[Color];

	  if(NewColor)
	  {
		  for (Plane = 0; Plane < Depth && NewColor != 0; Plane++)
		  {
		if (NewColor & 1)
		  (*DestImage)->ImageData[Plane * PlaneSize + PlaneOffs] |= (1 << (15 - Column % 16));
		NewColor >>= 1;
		  }
	  }
	}
	  return (TRUE);
	}
	else
	{
	  FreeVecPooled (*DestImage);
	  *DestImage = NULL;
	  return (FALSE);
	}
  }
  else
	return (FALSE);
}


void
FreeRemappedImage (struct Image *Image)

{
  if(Image)
  {
    WaitBlit();
	FreeVec (Image->ImageData);
	FreeVecPooled (Image);
  }
}


LONG
MyObtainPen(struct Screen *Screen, ULONG R, ULONG G, ULONG B)

{
	LONG   Best;
	ULONG	Diff, MinDiff, Sum;
	LONG   Col, z1;

	if(GfxVersion >= 39)
	return(ObtainBestPen(Screen->ViewPort.ColorMap,R,G,B,
				OBP_Precision,		PRECISION_GUI,
				TAG_DONE));
	else
	{
	R >>= 28;
	G >>= 28;
	B >>= 28;
	Best = 0;
	MinDiff = ~0;
	for(z1=0; z1 < Screen->RastPort.BitMap->Depth; z1++)
	{
		Col = GetRGB4(Screen->ViewPort.ColorMap,z1);
		Diff = iabs(B - (Col & 0xf));
		Sum = Diff * Diff;
		Diff = iabs(G - ((Col >> 4) & 0xf));
		Sum += (Diff * Diff);
		Diff = iabs(R - ((Col >> 8) & 0xf));
		Sum += (Diff * Diff);
		if(Sum == 0)
		return(z1);
		else if(Sum < MinDiff)
		{
		MinDiff = Sum;
		Best = z1;
		}
	}
	return(Best);
	}
}


void
MyReleasePen(struct Screen *Screen, ULONG Pen)

{
	if(GfxVersion >= 39)
	ReleasePen(Screen->ViewPort.ColorMap,Pen);
}


long
SimpleRequest (struct Window *RefWindow,
		   const char *RequestTitle,
		   const char *RequestText,
		   const char *RequestGadgets,
		   ULONG * IDCMPFlags,
		   ULONG Seconds,
		   APTR Arg1,
		   ...)
{
  struct EasyStruct ReqStruct;
  ULONG MyIDCMP, *IDCMPPtr;

  MyIDCMP = NULL;

  if (IDCMPFlags)
	IDCMPPtr = IDCMPFlags;
  else
	IDCMPPtr = &MyIDCMP;

  ReqStruct.es_StructSize = sizeof (struct EasyStruct);

  ReqStruct.es_Flags = 0;
  ReqStruct.es_Title = (char *) RequestTitle;
  ReqStruct.es_TextFormat = (char *) RequestText;
  ReqStruct.es_GadgetFormat = (char *) RequestGadgets;

  return (EasyRequest (RefWindow, &ReqStruct, IDCMPPtr, Arg1));

}


BOOL AskPrefs(void)
{
	struct	MsgPort *MMPort;
	struct	MMMessage	Msg;

	Msg.Class = MMC_GETCONFIG;
	Msg.Message.mn_ReplyPort = ReplyPort;
	/*Forbid();*/
	if(MMPort = FindPort(MMPORT_NAME))
	{
	PutMsg(MMPort,(struct Message *)&Msg);
	/*Permit();*/

	while(GetMsg(ReplyPort) != (struct Message *)&Msg)
		Wait(1l << ReplyPort->mp_SigBit);

	if(Msg.Class == MMC_GETCONFIG)
	{
		AktPrefs = *(struct MMPrefs *)Msg.Ptr1;
		strncpy(Cx_Popkey,Msg.Ptr2,99);
		MMEnabled = Msg.Arg1;
		return(TRUE);
	}
	}
	else
	/*Permit();*/

	MMEnabled = FALSE;

	return(FALSE);
}


BOOL NewPrefs(void)
{
	struct	MsgPort *MMPort;
	struct	MMMessage	Msg;

	Msg.Class = MMC_NEWCONFIG;
	Msg.Ptr1 = &AktPrefs;
	Msg.Message.mn_ReplyPort = ReplyPort;
	/*Forbid();*/
	if(MMPort = FindPort(MMPORT_NAME))
	{
	PutMsg(MMPort,(struct Message *)&Msg);
	/*Permit();*/

	while(GetMsg(ReplyPort) != (struct Message *)&Msg)
		Wait(1l << ReplyPort->mp_SigBit);

	if(Msg.Class == MMC_NEWCONFIG)
		return(TRUE);
	}
	else
	/*Permit();*/

	LT_MenuControlTags(GUI_Handle->Window,GUI_Menu,
				LAMN_ID,	MEN_DISABLE,
				LAMN_Checked,	TRUE,
				TAG_DONE);
	return(FALSE);
}


BOOL SendEnable(BOOL Enabled)
{
	struct	MsgPort *MMPort;
	struct	MMMessage	Msg;

	Msg.Class = MMC_ENABLE;
	Msg.Arg1 = Enabled;
	Msg.Message.mn_ReplyPort = ReplyPort;
	/*Forbid();*/
	if(MMPort = FindPort(MMPORT_NAME))
	{
	PutMsg(MMPort,(struct Message *)&Msg);
	/*Permit();*/

	while(GetMsg(ReplyPort) != (struct Message *)&Msg)
		Wait(1l << ReplyPort->mp_SigBit);

	LT_MenuControlTags(GUI_Handle->Window,GUI_Menu,
					LAMN_ID,		(Enabled) ? MEN_ENABLE : MEN_DISABLE,
					LAMN_Checked,	TRUE,
					TAG_DONE);
	return(TRUE);
	}
	else
	/*Permit();*/

	MMEnabled = FALSE;
	LT_MenuControlTags(GUI_Handle->Window,GUI_Menu,
				LAMN_ID,	MEN_DISABLE,
				LAMN_Checked,	TRUE,
				TAG_DONE);

	return(FALSE);
}


BOOL SendRemove(void)
{
	struct	MsgPort *MMPort;
	struct	MMMessage	Msg;

	MMEnabled = FALSE;
	LT_MenuControlTags(GUI_Handle->Window,GUI_Menu,
				LAMN_ID,	MEN_DISABLE,
				LAMN_Checked,	TRUE,
				TAG_DONE);

	Msg.Class = MMC_REMOVE;
	Msg.Message.mn_ReplyPort = ReplyPort;
	/*Forbid();*/
	if(MMPort = FindPort(MMPORT_NAME))
	{
	PutMsg(MMPort,(struct Message *)&Msg);
	/*Permit();*/

	while(GetMsg(ReplyPort) != (struct Message *)&Msg)
		Wait(1l << ReplyPort->mp_SigBit);

	if(Msg.Class == MMC_REMOVE)
		return(TRUE);
	}
	else
	/*Permit();*/

	return(FALSE);
}


BOOL LoadPrefs(char *ConfigFile)
{
	BPTR		FH;
	struct MMPrefs	ZwPrefs;
	ULONG		Len;

	if(! (FH = Open(ConfigFile,MODE_OLDFILE)))
	{
	SimpleRequest(NULL,"MagicMenu Prefs Error", "Couldn't open configuration file\n%s!","Ok",NULL,0,ConfigFile);
	return(FALSE);
	}

	Len = Read(FH,&ZwPrefs,sizeof(struct MMPrefs));

	Close(FH);

	if(Len == sizeof(struct MMPrefs) && ZwPrefs.mmp_Version == MMP_MAGIC)
	{
	CopyMem(&ZwPrefs,&AktPrefs,sizeof(struct MMPrefs));
	return(TRUE);
	}
	else
	{
	SimpleRequest(NULL,"MagicMenu Prefs Error", "Error in configuration file\n%s!","Ok",NULL,0,ConfigFile);
	return(FALSE);
	}
}


BOOL LoadPrefsAs(void)
{
	char	ZwStr[232];

	if(! AslRequestTags(FileReq,
			ASLFR_TitleText,		"Open MagicMenu config",
			ASLFR_PositiveText, 	"Open",
			ASLFR_NegativeText, 	"Cancel",
			ASLFR_Flags1,			NULL,
			ASLFR_Flags2,			FRF_REJECTICONS,
			TAG_DONE))
	{
	strncpy(ZwStr,FileReq->fr_Drawer,200);
	AddPart(ZwStr,FileReq->fr_File,231);

	return(LoadPrefs(ZwStr));
	}

	return(FALSE);
}


BOOL SavePrefs(char *ConfigFile)
{
	BPTR		FH;
	ULONG		Len;

	AktPrefs.mmp_Version = MMP_MAGIC;
	AktPrefs.mmp_Size = sizeof(struct MMPrefs);

	if(! (FH = Open(ConfigFile,MODE_NEWFILE)))
	{
	SimpleRequest(NULL,"MagicMenu Prefs Error", "Couldn't create configuration file\n%s!","Ok",NULL,0,ConfigFile);
	return(FALSE);
	}

	Len = Write(FH,&AktPrefs,sizeof(struct MMPrefs));

	Close(FH);

	if(Len != sizeof(struct MMPrefs))
	{
	SimpleRequest(NULL,"MagicMenu Prefs Error", "Error while saving configuration file\n%s!","Ok",NULL,0,ConfigFile);
	return(FALSE);
	}

	return(TRUE);
}


BOOL SavePrefsAs(void)
{
	char	ZwStr[232];

	if(! AslRequestTags(FileReq,
			ASLFR_TitleText,		"Save MagicMenu config",
			ASLFR_PositiveText, 	"Save",
			ASLFR_NegativeText, 	"Cancel",
			ASLFR_Flags1,			FRF_DOSAVEMODE,
			ASLFR_Flags2,			FRF_REJECTICONS,
			TAG_DONE))
	{
	strncpy(ZwStr,FileReq->fr_Drawer,200);
	AddPart(ZwStr,FileReq->fr_File,231);

	return(SavePrefs(ZwStr));
	}

	return(FALSE);
}


void
CloseAll (void)
{
  struct Message *Msg;

  CloseSettingsGUI();

  if(ReplyPort)
  {
	while(Msg = GetMsg(ReplyPort));
	DeleteMsgPort(ReplyPort);
	ReplyPort = NULL;
  }

  if(FileReq)
  {
	FreeAslRequest(FileReq);
	FileReq = NULL;
  }

  if (ColorWheelBase)
  {
	  CloseLibrary(ColorWheelBase);
	  ColorWheelBase = NULL;
  }

  if(GradientSliderBase)
  {
	  CloseLibrary(GradientSliderBase);
	  GradientSliderBase = NULL;
  }

  if (AslBase)
  {
	CloseLibrary ((struct Library *) AslBase);
	GfxBase = NULL;
  }

  if (UtilityBase)
  {
	CloseLibrary (UtilityBase);
	UtilityBase = NULL;
  }

  if (GfxBase)
  {
	CloseLibrary ((struct Library *) GfxBase);
	GfxBase = NULL;
  }

  if (IntuitionBase)
  {
	CloseLibrary ((struct Library *) IntuitionBase);
	IntuitionBase = NULL;
  }

  MemoryExit();
}

void
ExitTrap (void)
{
  CloseAll ();
  Delay (50);
}

void
ErrorPrc (const char *ErrTxt)
{
  static struct IntuiText NoOS2Body2 =
  {2, 1, JAM1, 5, 15, NULL, (UBYTE *) "works with AmigaOS 2.04 or later!", NULL};

  static struct IntuiText NoOS2Body1 =
  {2, 1, JAM1, 5, 5, NULL, (UBYTE *) "Sorry dude, this fine program only", &NoOS2Body2};

  static struct IntuiText NoOS2Neg =
  {2, 1, JAM1, 6, 3, NULL, (UBYTE *) "What a pity!", NULL};

  if (!IntuitionBase)
  {
	if (IntuitionBase = (struct IntuitionBase *) OpenLibrary ((char *) "intuition.library", 0))
	{
	  AutoRequest (NULL, &NoOS2Body1, NULL, &NoOS2Neg, NULL, NULL, 320, 65);
	}
	exit (40);
  }

  SimpleRequest (NULL, "MagicMenu Prefs Error", ErrTxt, "Hmpf", NULL, 0, 0);
  exit (40);
}



/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*		   MAIN 							*/
/* */
/* ************************************************************************ */
/* ************************************************************************ */

void
main (int argc, char **argv)
{
  struct Process *ThisProcess = (struct Process *)FindTask(NULL);
  BOOL	 Ok;
  BPTR	 FH;

  onexit (ExitTrap);

  MenGhostHiCol = MenGhostLoCol = MenFillCol = MenHiCol = MenTextCol = MenBackground =
	  MenDarkEdge = MenLightEdge = MenXENGrey0 = MenXENBlack = MenXENWhite = MenXENBlue =
	  MenXENGrey1 = MenXENGrey2 = MenXENBeige = MenXENPink = (UWORD)~0;

  GradientPens[0] = (UWORD)~0;

  if (!(IntuitionBase = (struct IntuitionBase *) OpenLibrary ((char *) "intuition.library", 37)))
	ErrorPrc ("");

  if (!(GfxBase = (struct GfxBase *) OpenLibrary ((char *) "graphics.library", 37)))
	ErrorPrc ("Can't open graphics library!");

  if (!(UtilityBase = OpenLibrary ("utility.library", 37)))
	ErrorPrc ("Can't open utility library!");

  if (!MemoryInit())
    ErrorPrc ("Cannot set up memory allocator");

  if (!(AslBase = OpenLibrary ("asl.library", 37)))
	ErrorPrc ("Can't open asl library!");

  if(ThisProcess -> pr_HomeDir)
  {
	  if(!(GTLayoutBase = SafeOpenLibrary("PROGDIR:Libs/gtlayout.library",24)))
		  GTLayoutBase = SafeOpenLibrary("PROGDIR:gtlayout.library",24);
  }

  if(!GTLayoutBase)
	  GTLayoutBase = SafeOpenLibrary("Libs/gtlayout.library",24);

  if(!GTLayoutBase)
	  GTLayoutBase = SafeOpenLibrary("gtlayout.library",24);

  if(!GTLayoutBase)
	  ErrorPrc("Can't open gtlayout.library V 24 or better!");

  ColorWheelBase = OpenLibrary ((char *) "gadgets/colorwheel.gadget", 39);
  GradientSliderBase = OpenLibrary ((char *) "gadgets/gradientslider.gadget", 39);

  GfxVersion = GfxBase->lib_Version;

  if(GetVar(PATHENV,ConfigPath,200,0) == -1)
  {
	  strcpy(ConfigPath, "");
	  if(SimpleRequest(NULL,"MagicMenu Prefs","The environment variable 'MAGICMENUPATH' is not set.\n\n"
						  "This variable is required for MagicMenu to locate its\n"
						  "configuration and its prefs tool.\n\n"
						  "Create the variable now?","Yes|No",NULL,0,0))
	  {
	  Ok = NameFromLock(ThisProcess->pr_HomeDir,ConfigPath,200);
	  if(Ok)
		  Ok = SetVar(PATHENV,ConfigPath,-1,GVF_GLOBAL_ONLY);
	  if(Ok)
		  Ok = (FH = Open("envarc:"PATHENV,MODE_NEWFILE));
	  if(Ok)
	  {
		  Ok = (FPuts(FH,ConfigPath) == 0);
		  Close(FH);
	  }
	  if(!Ok)
		  SimpleRequest(NULL,"MagicMenu Prefs","Couldn't create environment variable!","Ok",NULL,0,0);
	  }
  }

  strcpy(ConfigFile,ConfigPath);
  if(! AddPart(ConfigFile,"MagicMenu.config",231))
	  ErrorPrc("Can't create Config filename!");

  if(! (FileReq = AllocAslRequestTags(ASL_FileRequest,
					ASLFR_InitialDrawer,		ConfigPath,
					ASLFR_InitialFile,		".config",
					ASLFR_RejectIcons,		TRUE,
					TAG_DONE)))
	  ErrorPrc("Couldn't allocate ASL request!");


  if(! (ReplyPort = CreateMsgPort()))
	  ErrorPrc("Couldn't create ReplyPort!");

  AktPrefs = DefaultPrefs;
  strcpy(Cx_Popkey,"unknown");

  if(! AskPrefs())
	  LoadPrefs(ConfigFile);

  SPrintf(WindowTitle,"MagicMenu Prefs " MAJORVERS " <%s>",Cx_Popkey);

  OpenSettingsGUI();

  MainLoop();

  CloseSettingsGUI();

  CloseAll();

  exit(0);
}
