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
#undef ReadPixelArray
#undef WritePixelArray
#undef BltBitMap
#undef CreateUpfrontHookLayer
#endif

#ifndef max
#define max(a,b)	((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(a,b)	((a)<(b)?(a):(b))
#endif

/*****************************************************************************************/

#define EXTRACT_BITS(v,offset,width) (((v) >> (offset)) & ((1 << width)-1))

/*****************************************************************************************/

#define BRIGHTNESS(r,g,b) (((WORD)r) * 38 + ((WORD)g) * 76 + ((WORD)b) * 14) / 128

/*****************************************************************************************/

#define CATCOMP_NUMBERS
#include "locale/magicmenu.h"

/*****************************************************************************************/

ULONG __ASM CallOpenWindow (REG(a0, struct NewWindow *NW), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallOpenWindowTagList (REG(a0, struct NewWindow *NW), REG(a1, struct TagItem *TI), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallClearMenuStrip (REG(a0, struct Window *W), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallSetMenuStrip (REG(a0, struct Window *W), REG(a1, struct Menu *MS), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallResetMenuStrip (REG(a0, struct Window *W), REG(a1, struct Menu *MS), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallCloseWindow (REG(a0, struct Window *W), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallActivateWindow (REG(a0, struct Window *W), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallWindowToFront (REG(a0, struct Window *W), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallWindowToBack (REG(a0, struct Window *W), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallMoveWindowInFrontOf (REG(a0, struct Window *Window), REG(a1, struct Window *Behind), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallModifyIDCMP (REG(a0, struct Window *window), REG(d0, ULONG flags), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallOffMenu (REG(a0, struct Window *window), REG(d0, ULONG number), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallOnMenu (REG(a0, struct Window *window), REG(d0, ULONG number), REG(a6, struct IntuitionBase *IntuitionBase));
LONG __ASM CallLendMenus (REG(a0, struct Window *FromWindow),REG(a1, struct Window *ToWindow), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallRefreshWindowFrame (REG(a0, struct Window *), REG(a6, struct IntuitionBase *));
ULONG __ASM CallSetWindowTitles (REG(a0, struct Window *), REG(a1, STRPTR), REG(a2, STRPTR), REG(a6, struct IntuitionBase *));
struct RastPort *__ASM CallObtainGIRPort (REG(a0, struct GadgetInfo *GInfo), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallOpenScreen (REG(a0, struct NewScreen *NS), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallOpenScreenTagList (REG(a0, struct NewScreen *NS), REG(a1, struct TagItem *TI), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallCloseScreen (REG(a0, struct Screen *S), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallScreenToFront (REG(a0, struct Screen *S), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallScreenToBack (REG(a0, struct Screen *S), REG(a6, struct IntuitionBase *IntuitionBase));
ULONG __ASM CallScreenDepth (REG(a0, struct Screen *S), REG(d0, ULONG flags), REG(a1, APTR reserved), REG(a6, struct IntuitionBase *IntuitionBase));

/*****************************************************************************************/

BOOL
MMCheckParentScreen (struct Window *Window,BOOL PlayItSafe)
{
	BOOL Wait;

	SafeObtainSemaphoreShared (GetPointerSemaphore);

	Wait = (MenScr == Window->WScreen) && (PlayItSafe || IsBlocking) && (FindTask(NULL) != ThisTask);

	ReleaseSemaphore (GetPointerSemaphore);

	if (!Wait)
		return (FALSE);

	ObtainSemaphore (MenuActSemaphore);

	return (TRUE);
}

BOOL
MMCheckScreen (void)
{
	BOOL Wait;

	SafeObtainSemaphoreShared (GetPointerSemaphore);

	Wait = (MenScr != NULL) && (FindTask(NULL) != ThisTask);

	ReleaseSemaphore (GetPointerSemaphore);

	if (!Wait)
		return (FALSE);

	ObtainSemaphore (MenuActSemaphore);

	return (TRUE);
}

BOOL
MMCheckWindow (struct Window * Win)
{
	BOOL Wait;

	SafeObtainSemaphoreShared (GetPointerSemaphore);

	Wait = (MenWin == Win) && (FindTask(NULL) != ThisTask);

	ReleaseSemaphore (GetPointerSemaphore);

	if (!Wait)
		return (FALSE);

	ObtainSemaphore (MenuActSemaphore);

	return (TRUE);
}

/*****************************************************************************************/

ULONG __ASM SAVEDS
MMOpenWindow (REG(a0, struct NewWindow * NW))
{
	ULONG Win;

	D(("|%s| in OpenWindow patch",FindTask (NULL)->tc_Node.ln_Name));

	if (MMCheckScreen ())
	{
		Win = CallOpenWindow (NW, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Win = CallOpenWindow (NW, IntuitionBase);

	/*****************************************************************************************/

	if (Win)
		RegisterGlyphs ((struct Window *) Win, NW, NULL);

	/*****************************************************************************************/

	return (Win);
}

ULONG __ASM SAVEDS
MMOpenWindowTagList (
	REG(a0, struct NewWindow * NW),
	REG(a1, struct TagItem * TI))
{
	ULONG Win;

	D(("|%s| in OpenWindowTagList patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckScreen ())
	{
		Win = CallOpenWindowTagList (NW, TI, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Win = CallOpenWindowTagList (NW, TI, IntuitionBase);

	/*****************************************************************************************/

	if (Win && (FindTask(NULL) != ThisTask))
		RegisterGlyphs ((struct Window *) Win, NW, TI);

	/*****************************************************************************************/

	return (Win);
}

ULONG __ASM SAVEDS
MMClearMenuStrip (REG(a0, struct Window * W))
{
	ULONG Res;

	D(("|%s| in ClearMenuStrip patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckWindow (W))
	{
		Res = CallClearMenuStrip (W, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
		return (Res);
	}
	else
	{
		return (CallClearMenuStrip (W, IntuitionBase));
	}
}

ULONG __ASM SAVEDS
MMSetMenuStrip (
	REG(a0, struct Window * W),
	REG(a1, struct Menu * MI))
{
	ULONG Res;

	D(("|%s| in SetMenuStrip patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckWindow (W))
	{
		Res = CallSetMenuStrip (W, MI, IntuitionBase);
		ClearRemember (W);
		ReleaseSemaphore (MenuActSemaphore);
		return (Res);
	}
	else
	{
		Res = CallSetMenuStrip (W, MI, IntuitionBase);
		ClearRemember (W);
		return (Res);
	}
}

ULONG __ASM SAVEDS
MMResetMenuStrip (
	REG(a0, struct Window * W),
	REG(a1, struct Menu * MI))
{
	ULONG Res;

	D(("|%s| in ResetMenuStrip patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckWindow (W))
	{
		Res = CallResetMenuStrip (W, MI, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
		return (Res);
	}
	else
	{
		Res = CallResetMenuStrip (W, MI, IntuitionBase);
		return (Res);
	}
}

ULONG __ASM SAVEDS
MMCloseWindow (REG(a0, struct Window * W))
{
	ULONG Res;
	BOOL CallSelf;

	CallSelf = (FindTask(NULL) == ThisTask);

	D(("%s| in CloseWindow patch", FindTask (NULL)->tc_Node.ln_Name));

	if (MMCheckScreen ())
	{
		ClearRemember (W);
		Res = CallCloseWindow (W, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
	{
		if(!CallSelf)
			ClearRemember (W);
		Res = CallCloseWindow (W, IntuitionBase);
	}

	/*****************************************************************************************/

	if(!CallSelf)
		DiscardWindowGlyphs (W);

	/*****************************************************************************************/

	return (Res);
}

ULONG __ASM SAVEDS
MMActivateWindow (REG(a0, struct Window * W))
{
	ULONG Result;

	D(("|%s| in ActivateWindow patch", FindTask (NULL)->tc_Node.ln_Name));
	if (AttemptSemaphore (MenuActSemaphore))
	{
		Result = CallActivateWindow (W, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
		return (Result);
	}
	else
		return (FALSE);
}

ULONG __ASM SAVEDS
MMWindowToFront (REG(a0, struct Window * W))
{
	ULONG Result;

	/* Bug-Kompatibilität :( */
	if(!RealWindow(W))
		return(FALSE);

	D(("|%s| in WindowToFront patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckParentScreen (W, TRUE))
	{
		Result = CallWindowToFront (W, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallWindowToFront (W, IntuitionBase);

	return (Result);
}

ULONG __ASM SAVEDS
MMWindowToBack (REG(a0, struct Window * W))
{
	ULONG Result;

	D(("|%s| in WindowToBack patch", FindTask (NULL)->tc_Node.ln_Name));

	if (MMCheckParentScreen (W, TRUE))
	{
		Result = CallWindowToBack (W, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallWindowToBack (W, IntuitionBase);

	return (Result);
}

ULONG __ASM SAVEDS
MMModifyIDCMP (
	REG(a0, struct Window * window),
	REG(d0, ULONG flags))
{
	ULONG Result;

	D(("|%s| in ModifyIDCMP patch",FindTask (NULL)->tc_Node.ln_Name));

	if (MMCheckWindow (window))
	{
		Result = CallModifyIDCMP (window, flags, IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallModifyIDCMP (window, flags, IntuitionBase);

	return (Result);
}

ULONG __ASM SAVEDS
MMOffMenu (
	REG(a0, struct Window * window),
	REG(d0, ULONG number))
{
	ULONG Result;

	D(("%s| in OffMenu patch", FindTask (NULL)->tc_Node.ln_Name));

	if (MMCheckWindow (window))
	{
		Result = CallOffMenu (window, number, IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallOffMenu (window, number, IntuitionBase);

	return (Result);
}

ULONG __ASM SAVEDS
MMOnMenu (
	REG(a0, struct Window * window),
	REG(d0, ULONG number))
{
	ULONG Result;

	D(("|%s| in OnMenu patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckWindow (window))
	{
		Result = CallOnMenu (window, number, IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallOnMenu (window, number, IntuitionBase);

	return (Result);
}

struct RastPort *__ASM SAVEDS
MMObtainGIRPort (REG(a0, struct GadgetInfo *GInfo))
{
	D(("|%s| in ObtainGIRPort patch", FindTask (NULL)->tc_Node.ln_Name));
	if (!AktPrefs.mmp_NonBlocking && GInfo)  /* GInfo kann tatsächlich (!) NULL sein. */
	{
		SafeObtainSemaphoreShared (GetPointerSemaphore);

		if (MenScr == GInfo->gi_Screen)
		{
			struct RastPort *Result;

			ReleaseSemaphore (GetPointerSemaphore);

			if (AttemptSemaphore (MenuActSemaphore))
			{
				Result = CallObtainGIRPort (GInfo, IntuitionBase);

				ReleaseSemaphore (MenuActSemaphore);
			}
			else
				Result = NULL;

			return (Result);
		}

		ReleaseSemaphore (GetPointerSemaphore);
	}

	return (CallObtainGIRPort (GInfo, IntuitionBase));
}

ULONG __ASM SAVEDS
MMRefreshWindowFrame (REG(a0, struct Window * W))
{
	ULONG Result;

	/* Bug-Kompatibilität :( */
	if(!RealWindow(W))
		return(FALSE);

	D(("|%s| in RefreshWindowFrame patch", FindTask (NULL)->tc_Node.ln_Name));
	if (MMCheckParentScreen (W, FALSE))
	{
		Result = CallRefreshWindowFrame (W, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallRefreshWindowFrame (W, IntuitionBase);

	return (Result);
}

ULONG __ASM SAVEDS
MMSetWindowTitles (
	REG(a0, struct Window * W),
	REG(a1, STRPTR WindowTitle),
	REG(a2, STRPTR ScreenTitle))
{
	ULONG Result;

	/* Bug-Kompatibilität :( */
	if(!RealWindow(W))
		return(FALSE);

	D(("|%s| in SetWindowTitles patch", FindTask (NULL)->tc_Node.ln_Name));

	if (MMCheckParentScreen (W, FALSE))
	{
		Result = CallSetWindowTitles (W, WindowTitle, ScreenTitle, IntuitionBase);
		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Result = CallSetWindowTitles (W, WindowTitle, ScreenTitle, IntuitionBase);

	return (Result);
}

ULONG __ASM SAVEDS
MMOpenScreen (REG(a0, struct NewScreen *NS))
{
	ULONG Scr;

	if (MMCheckScreen ())
	{
		Scr = CallOpenScreen (NS, IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Scr = CallOpenScreen (NS, IntuitionBase);

	return(Scr);
}

ULONG __ASM SAVEDS
MMOpenScreenTagList (
	REG(a0, struct NewScreen *NS),
	REG(a1, struct TagItem *TI))
{
	ULONG Scr;

	if (MMCheckScreen ())
	{
		Scr = CallOpenScreenTagList (NS, TI, IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		Scr = CallOpenScreenTagList (NS, TI, IntuitionBase);

	return(Scr);
}

ULONG __ASM SAVEDS
MMCloseScreen (REG(a0, struct Screen *S))
{
	ULONG rc;

	if (MMCheckScreen ())
	{
		rc = CallCloseScreen(S,IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		rc = CallCloseScreen(S,IntuitionBase);

	return(rc);
}

ULONG __ASM SAVEDS
MMScreenToFront (REG(a0, struct Screen *S))
{
	ULONG rc;

	if (MMCheckScreen ())
	{
		rc = CallScreenToFront(S,IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		rc = CallScreenToFront(S,IntuitionBase);

	return(rc);
}

ULONG __ASM SAVEDS
MMScreenToBack (REG(a0, struct Screen *S))
{
	ULONG rc;

	if (MMCheckScreen ())
	{
		rc = CallScreenToBack(S,IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		rc = CallScreenToBack(S,IntuitionBase);

	return(rc);
}

ULONG __ASM SAVEDS
MMScreenDepth (
	REG(a0, struct Screen *S),
	REG(d0, ULONG flags),
	REG(a1, ULONG reserved))
{
	ULONG rc;

	if (MMCheckScreen ())
	{
		rc = CallScreenDepth(S,flags,reserved,IntuitionBase);

		ReleaseSemaphore (MenuActSemaphore);
	}
	else
		rc = CallScreenDepth(S,flags,reserved,IntuitionBase);

	return(rc);
}

LONG __ASM SAVEDS
MMLendMenus (
	REG(a0, struct Window *FromWindow),
	REG(a1, struct Window *ToWindow))
{
	D(("|%s| in LendMenus patch", FindTask (NULL)->tc_Node.ln_Name));

	RegisterLending(FromWindow,ToWindow);

	return (CallLendMenus (FromWindow, ToWindow, IntuitionBase));
}

/*****************************************************************************************/


struct FatImage {
	struct Image	Image;
	struct Image	Mask;
	UWORD				BytesPerRow;
	UWORD				Rows;
};


VOID
CreateBitMapFromImage (struct Image * Image, struct BitMap * BitMap)
{
	PLANEPTR Data;
	ULONG Modulo;
	LONG i, Mask;

	memset(BitMap,0,sizeof(*BitMap));
	InitBitMap (BitMap, Image->Depth, Image->Width, Image->Height);

	Modulo = BitMap->BytesPerRow * BitMap->Rows;
	Data = (PLANEPTR) Image->ImageData;

	for (i = 0, Mask = 1; i < BitMap->Depth; i++, Mask += Mask)
	{
		if (Image->PlanePick & Mask)
		{
			BitMap->Planes[i] = Data;

			Data += Modulo;
		}
		else
		{
			if (Image->PlaneOnOff & Mask)
				BitMap->Planes[i] = (PLANEPTR) ~ 0;
		}
	}
}

VOID
RecolourBitMap (struct BitMap *Src, struct BitMap *Dst, UBYTE * Mapping, LONG DestDepth, LONG Width, LONG Height)
{
	extern VOID __ASM RemapBitMap(REG(a0, struct BitMap *srcbm),REG(a1, struct BitMap *destbm),REG(a2, UBYTE *table),REG(d0, LONG width));

	WaitBlit();
	RemapBitMap(Src,Dst,Mapping,Width);
}

VOID
MakeRemappedImage(
	BOOL *			good,
	struct Image **	DestImage,
	struct Image *	SrcImage,
	UWORD			Depth,
	UBYTE *			RemapArray)
{
	if(NOT (*good))
		return;

	if (Depth > 8)
		Depth = 8;

	(*DestImage) = (struct Image *) AllocVecPooled (sizeof (struct FatImage), MEMF_ANY | MEMF_CLEAR);
	if ((*DestImage) != NULL)
	{
		struct Image * image;
		struct Image * mask;

		image	= (*DestImage);
		mask	= image + 1;

		image->LeftEdge = SrcImage->LeftEdge;
		image->TopEdge = SrcImage->TopEdge;
		image->Width = SrcImage->Width;
		image->Height = SrcImage->Height;
		image->Depth = Depth;
		image->NextImage = (struct Image *)IMAGE_MAGIC;

		(*mask) = (*image);
		mask->Depth = 1;
		mask->PlanePick = 1;

		if (SrcImage->PlanePick || SrcImage->Depth == -1)
		{
			struct BitMap Dst;

			image->PlanePick = (1L << Depth) - 1;

			memset(&Dst,0,sizeof(Dst));
			InitBitMap (&Dst, Depth, SrcImage->Width, SrcImage->Height);

			//image->ImageData = (UWORD *) AllocVec (Dst.BytesPerRow * Dst.Rows * Dst.Depth, MEMF_CHIP);
			//mask->ImageData = (UWORD *) AllocVec (Dst.BytesPerRow * Dst.Rows, MEMF_CHIP);
			/* Stephan: Mittels AllocRaster können wir auch FASTRAM bekommen */
			image->ImageData = (UWORD *) AllocRaster( Dst.BytesPerRow * 8 * Dst.Depth, Dst.Rows );
			mask->ImageData = (UWORD *) AllocRaster( Dst.BytesPerRow * 8, Dst.Rows );

			((struct FatImage *)image)->BytesPerRow = Dst.BytesPerRow;
			((struct FatImage *)image)->Rows = Dst.Rows;

			if (image->ImageData != NULL && mask->ImageData != NULL)
			{
				STATIC UBYTE MaskRemapArray[16] =
				{
					1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1
				};

				UBYTE Mask;

				CreateBitMapFromImage (image, &Dst);

				if(SrcImage->Depth == -1)
					Mask = 0xFF;
				else
					Mask = (1L<<SrcImage->Depth)-1;

				if (SrcImage->Depth == -1 || ((SrcImage->PlanePick & Mask) != Mask))
				{
					struct RastPort TempRPort;

					InitRastPort (&TempRPort);

					if (TempRPort.BitMap = allocBitMap (Depth, SrcImage->Width, SrcImage->Height, NULL, TRUE))
					{
						DrawImage (&TempRPort, SrcImage, -SrcImage->LeftEdge, -SrcImage->TopEdge);

						RecolourBitMap (TempRPort.BitMap, &Dst, RemapArray, Depth, SrcImage->Width, SrcImage->Height);

						CreateBitMapFromImage (mask, &Dst);
						RecolourBitMap (TempRPort.BitMap, &Dst, MaskRemapArray, 1, SrcImage->Width, SrcImage->Height);

						disposeBitMap (TempRPort.BitMap, TRUE);

						return;
					}
				}
				else
				{
					struct BitMap Src;

					CreateBitMapFromImage (SrcImage, &Src);

					RecolourBitMap (&Src, &Dst, RemapArray, Depth, SrcImage->Width, SrcImage->Height);

					CreateBitMapFromImage (mask, &Dst);
					RecolourBitMap (&Src, &Dst, MaskRemapArray, 1, SrcImage->Width, SrcImage->Height);

					return;
				}
			}
		}
		else
		{
			image->PlaneOnOff = RemapArray[SrcImage->PlaneOnOff];

			return;
		}

		FreeRemappedImage (*DestImage);
		(*DestImage) = NULL;
	}

	(*good) = FALSE;
}

VOID
FreeRemappedImage(struct Image * Image)
{
	if(Image != NULL)
	{
		WaitBlit();

		//FreeVec(Image[0].ImageData);
		//FreeVec(Image[1].ImageData);

		FreeRaster( Image[0].ImageData, ((struct FatImage *)Image)->BytesPerRow * 8 * Image[0].Depth, ((struct FatImage *)Image)->Rows );
		FreeRaster( Image[1].ImageData, ((struct FatImage *)Image)->BytesPerRow * 8, ((struct FatImage *)Image)->Rows );

		FreeVecPooled(Image);
	}
}

VOID
StartTimeRequest(struct timerequest *TimeRequest,ULONG Seconds,ULONG Micros)
{
	TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
	TimeRequest->tr_time.tv_secs	= Seconds;
	TimeRequest->tr_time.tv_micro	= Micros;

	SetSignal(0,PORTMASK(TimeRequest->tr_node.io_Message.mn_ReplyPort));
	SendIO(TimeRequest);
}

VOID
StopTimeRequest(struct timerequest *TimeRequest)
{
	if(!CheckIO(TimeRequest))
		AbortIO(TimeRequest);

	WaitIO(TimeRequest);
}

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

	va_start(VarArgs,Text);
	EasyRequestArgs(NULL,&Easy,NULL,(APTR)VarArgs);
	va_end(VarArgs);
}

BOOL
CheckReply (struct Message * Msg)
{
	if (Msg->mn_Node.ln_Type == NT_REPLYMSG)
	{
		FreeVecPooled (Msg);
		return (TRUE);
	}
	else
		return (FALSE);
}

BOOL
CheckEnde (void)
{
	if (IMsgReplyCount > 0)
	{
		ShowRequest (GetString(MSG_CANNOT_UNINSTALL_TXT));

		return (FALSE);
	}

	if (LibPatches)
	{
		RemovePatches ();
		LibPatches = FALSE;
	}

	return (TRUE);
}

/*****************************************************************************************/

struct FatBitMap
{
	UWORD         Width;
	UWORD         Height;
	struct BitMap BitMap;
};

void
disposeBitMap (struct BitMap *BitMap, BOOL IsChipMem)
{
	WaitBlit ();

	if (V39 && !IsChipMem)
		FreeBitMap (BitMap);
	else
	{
		if(BitMap != NULL)
		{
			struct FatBitMap * fat;
			LONG i;

			fat = (struct FatBitMap *)(((ULONG)BitMap) - offsetof(struct FatBitMap,BitMap));

			for(i = 0 ; i < BitMap->Depth ; i++)
			{
				if(BitMap->Planes[i] != NULL)
					FreeRaster(BitMap->Planes[i], fat->Width, fat->Height);
			}

			FreeVecPooled(fat);
		}
	}
}

struct BitMap *
allocBitMap (LONG Depth, LONG Width, LONG Height, struct BitMap *Friend, BOOL WantChipMem)
{
	struct BitMap *BitMap;
	BOOL Error;
	LONG z1;

	Error = FALSE;

	if (V39 && !WantChipMem)
		return (AllocBitMap (Width, Height, Depth, BMF_MINPLANES|BMF_DISPLAYABLE, Friend));
	else
	{
		struct FatBitMap *fat;
		LONG Extra;

		if(Depth > 8)
			Extra = sizeof(PLANEPTR) * (Depth - 8);
		else
			Extra = 0;

		fat = AllocVecPooled (sizeof (*fat) + Extra, MEMF_ANY|MEMF_CLEAR);
		if(fat != NULL)
		{
			BitMap = &fat->BitMap;

			fat->Width  = Width;
			fat->Height = Height;
		}
		else
		{
			BitMap = NULL;
		}

		if (BitMap != NULL)
		{
			InitBitMap (BitMap, Depth, Width, Height);

			for (z1 = 0; z1 < Depth; z1++)
			{
				if (!(BitMap->Planes[z1] = (PLANEPTR) AllocRaster (Width, Height)))
				{
					Error = TRUE;
					break;
				}
			}

			if (Error)
			{
				disposeBitMap (BitMap, TRUE);
				return (NULL);
			}

			return (BitMap);
		}
	}

	return (NULL);
}

/*****************************************************************************************/

void
FreeRPort (
	struct BitMap *BitMap,
	struct Layer_Info *LayerInfo,
	struct Layer *Layer,
	struct BackgroundCover ** BackgroundCoverPtr)
{
	if (Layer)
		DeleteLayer (NULL, Layer);

	if (LayerInfo)
		DisposeLayerInfo (LayerInfo);

	disposeBitMap (BitMap, FALSE);

	DeleteBackgroundCover((*BackgroundCoverPtr));
	(*BackgroundCoverPtr) = NULL;
}

STATIC ULONG
Dummy (VOID)
{
	return (0);
}

STATIC struct Hook DummyHook =
{
	{NULL}, (HOOKFUNC) Dummy
};

struct Hook *
GetNOPFillHook ()
{
	if (LayersBase->lib_Version >= 39)
		return (LAYERS_NOBACKFILL);
	else
		return (&DummyHook);
}

BOOL
InstallRPort (
	LONG Left,
	LONG Top,
	LONG Depth,
	LONG Width,
	LONG Height,
	struct RastPort ** RastPortPtr,
	struct BitMap ** BitMapPtr,
	struct Layer_Info ** LayerInfoPtr,
	struct Layer ** LayerPtr,
	struct ClipRect ** ClipRectPtr,
	struct BackgroundCover ** BackgroundCoverPtr,
	LONG Level)
{
	struct BitMap *Friend;
	struct BitMap *BitMap;
	struct Layer_Info *LayerInfo;
	struct Layer *Layer;
	struct ClipRect * ClipRect;
	LONG OriginalHeight,OriginalWidth,ShadowSize;
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

	Friend = MenScr->RastPort.BitMap;

	if(Look3D && AktPrefs.mmp_CastShadows && Black != 0)
	{
		ShadowSize = SHADOW_SIZE + Level * 2;

		OriginalWidth  = Width;
		OriginalHeight = Height;

		Width  += ShadowSize;
		Height += ShadowSize;

		if(Left + Width > MenScr->Width)
			Width = MenScr->Width - Left;

		if(Top + Height > MenScr->Height)
			Height = MenScr->Height - Top;
	}
	else
	{
		ShadowSize = 0;

		OriginalWidth  = Width;
		OriginalHeight = Height;
	}

	(*BitMapPtr) = NULL;
	(*RastPortPtr) = NULL;
	(*LayerPtr) = NULL;
	(*LayerInfoPtr) = NULL;
	(*ClipRectPtr) = NULL;
	(*BackgroundCoverPtr) = NULL;

	BitMap = allocBitMap (Depth, Width, Height, Friend, FALSE);
	if (BitMap != NULL)
	{
		ClipRect = GetClipRect(BitMap,Left,Top,Left + Width - 1,Top + Height - 1);
		if(ClipRect != NULL)
		{
			LayerInfo = NewLayerInfo ();
			if (LayerInfo != NULL)
			{
				Layer = CreateUpfrontHookLayer (LayerInfo, BitMap, 0, 0, Width - 1, Height - 1, LAYERSIMPLE, GetNOPFillHook (), NULL);
				if (Layer != NULL)
				{
					struct BackgroundCover *bgc;

					bgc = CreateBackgroundCover(Friend,Left,Top,OriginalWidth,OriginalHeight,Level==0&&!GlobalPopUp);

					(*BitMapPtr) = BitMap;
					(*RastPortPtr) = Layer->rp;
					(*LayerPtr) = Layer;
					(*LayerInfoPtr) = LayerInfo;
					(*ClipRectPtr) = ClipRect;
					(*BackgroundCoverPtr) = bgc;

					if(ShadowSize > 0 && (OriginalWidth < Width || OriginalHeight < Height))
					{
						struct RastPort *RPort;

						RPort = Layer->rp;

						SetAfPt (RPort, Crosshatch, 1);

						SetPens(RPort, Black, 0,JAM1);

						if(OriginalWidth < Width)
							BltBitMap(Friend,Left + OriginalWidth,Top,BitMap,OriginalWidth,0,Width - OriginalWidth,Height,MINTERM_COPY,~0,NULL);

						if(OriginalHeight < Height)
							BltBitMap(Friend,Left,Top + OriginalHeight,BitMap,0,OriginalHeight,Width,Height - OriginalHeight,MINTERM_COPY,~0,NULL);

						if(OriginalWidth < Width)
							DrawShadow(RPort, OriginalWidth, ShadowSize, OriginalWidth + ShadowSize - 1, OriginalHeight - 1, RIGHT_PART);

						if(OriginalHeight < Height)
							DrawShadow(RPort, ShadowSize, OriginalHeight, OriginalWidth + ShadowSize - 1, OriginalHeight + ShadowSize - 1, BOTTOM_PART);

						SetAfPt (RPort, NULL, 0);
					}

					success = TRUE;
				}
				else
				{
					DisposeLayerInfo (LayerInfo);
				}
			}

			if(NO success)
				FreeVec(ClipRect);
		}

		if(NO success)
			disposeBitMap (BitMap, FALSE);
	}

	return (success);
}

void
SwapRPortClipRect(struct RastPort *RPort,struct ClipRect *ClipRect)
{
	struct BitMap *Temp;
	LONG Left,Top,Width,Height;

	Left	= ClipRect->bounds.MinX;
	Top	= ClipRect->bounds.MinY;
	Width	= ClipRect->bounds.MaxX - ClipRect->bounds.MinX + 1;
	Height	= ClipRect->bounds.MaxY - ClipRect->bounds.MinY + 1;

	if(Temp = allocBitMap(GetBitMapDepth(RPort->BitMap),Width,Height,RPort->BitMap,FALSE))
	{
		BltBitMap(RPort->BitMap,Left,Top,Temp,0,0,Width,Height,MINTERM_COPY,~0,NULL);
		BltBitMap(ClipRect->BitMap,0,0,RPort->BitMap,Left,Top,Width,Height,MINTERM_COPY,~0,NULL);
		BltBitMap(Temp,0,0,ClipRect->BitMap,0,0,Width,Height,MINTERM_COPY,~0,NULL);

		disposeBitMap(Temp,FALSE);
	}
	else
	{
		BltBitMap(RPort->BitMap,Left,Top,ClipRect->BitMap,0,0,Width,Height,MINTERM_B_XOR_C,~0,NULL);
		BltBitMap(ClipRect->BitMap,0,0,RPort->BitMap,Left,Top,Width,Height,MINTERM_B_XOR_C,~0,NULL);
		BltBitMap(RPort->BitMap,Left,Top,ClipRect->BitMap,0,0,Width,Height,MINTERM_B_XOR_C,~0,NULL);
	}
}

struct ClipRect *
GetClipRect (
	struct BitMap *BitMap,
	LONG x1,
	LONG y1,
	LONG x2,
	LONG y2)
{
	struct ClipRect *CRect;

	if (CRect = AllocVecPooled (sizeof (struct ClipRect), MEMF_CLEAR))
	{
		CRect->BitMap = BitMap;
		CRect->bounds.MinX = x1;
		CRect->bounds.MinY = y1;
		CRect->bounds.MaxX = x2;
		CRect->bounds.MaxY = y2;
	}
	return (CRect);
}

void
CheckDispClipVisible (WORD MinX, WORD MinY, WORD MaxX, WORD MaxY)
{
	WORD x1, y1, x2, y2;

	x1 = -MenScr->LeftEdge;
	y1 = -MenScr->TopEdge;

	x2 = MenDispClip.MaxX + x1;
	y2 = MenDispClip.MaxY + y1;

	if (MinX < x1 || MinY < y1 || MaxX > x2 || MaxY > y2)
	{
		x1 = MinX + (MaxX - MinX) / 2 - MenDispClip.MaxX / 2;
		if (x1 < 0)
			x1 = 0;

		y1 = MinY + (MaxY - MinY) / 2 - MenDispClip.MaxY / 2;
		if (y1 < 0)
			y1 = 0;

		MoveScreen (MenScr, -MenScr->LeftEdge - x1, -MenScr->TopEdge - y1);
	}
}

void
Draw3DRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height,
						BOOL Upward, BOOL HiRes, BOOL DoubleBorder)
{
	if (Width <= 0 || Height <= 0)
		return;

	if (Upward)
		SetFgPen (rp, MenLightEdge);

	else
		SetFgPen (rp, MenDarkEdge);

	/* Nur zur Kompatibilität... */
	if(Look3D)
		HiRes = FALSE;

	if (HiRes && (!LookMC))
	{
		//      2<------1-
		//      |4
		//      ||
		//      ||
		//      |v
		//      v5
		//      3

		DrawLinePairs (rp, 3, x + Width - 2, y,
		                      x, y,
		                      x, y + Height - 1);

		DrawLine (rp, x + 1, y + 1,
		              x + 1, y + Height - 2);

		if (DoubleBorder)
		{
			//      2<------1-
			//      |4    96
			//      ||    ||
			//      ||    ||
			//      ||    v|
			//      |v    av
			//      v58<---7
			//      3

			DrawLinePairs (rp, 3, x + Width - 3, y + 1,
			                      x + Width - 3, y + Height - 2,
			                      x + 3, y + Height - 2);

			DrawLine (rp, x + Width - 4, y + 2,
			              x + Width - 4, y + Height - 2);
		}
	}
	else
	{
		//      2<----1
		//      |
		//      |
		//      |
		//      |
		//      v
		//      3

		DrawLinePairs (rp, 3, x + Width - 1, y,
		                      x, y,
		                      x, y + Height - 1);

		if (DoubleBorder)
		{
			//      2<----1
			//      |    4
			//      |    |
			//      |    |
			//      |    v
			//      v6<--5
			//      3

			DrawLinePairs (rp, 3, x + Width - 2, y + 1,
			                      x + Width - 2, y + Height - 2,
			                      x + 1, y + Height - 2);
		}
	}

	if (Upward)
		SetFgPen (rp, MenDarkEdge);

	else
		SetFgPen (rp, MenLightEdge);

	if (HiRes && (!LookMC))
	{
		//                 3
		//                5^
		//                ^|
		//                ||
		//                ||
		//                4|
		//      -1-------->2

		DrawLinePairs (rp, 3, x + 1, y + Height - 1,
		                      x + Width - 1, y + Height - 1,
		                      x + Width - 1, y);

		DrawLine (rp, x + Width - 2, y + Height - 2,
		              x + Width - 2, y + 1);

		if (DoubleBorder)
		{
			//                 3
			//        7---->8 5^
			//        ^a      ^|
			//        |^      ||
			//        ||      ||
			//        |9      ||
			//        6       4|
			//      -1-------->2

			DrawLinePairs (rp, 3, x + 2, y + Height - 2,
			                      x + 2, y + 1,
			                      x + Width - 4, y + 1);

			DrawLine (rp, x + 3, y + Height - 3,
			              x + 3, y + 2);
		}
	}
	else
	{
		//                 |
		//                 3
		//                 ^
		//                 |
		//                 |
		//                 |
		//      -1-------->2

		DrawLinePairs (rp, 3, x + 1, y + Height - 1,
		                      x + Width - 1, y + Height - 1,
		                      x + Width - 1, y + 1);

		if (DoubleBorder)
		{
			//                 |
			//       5------->63
			//       ^         ^
			//       |         |
			//       |         |
			//       4         |
			//      -1-------->2

			DrawLinePairs (rp, 3, x + 1, y + Height - 2,
			                      x + 1, y + 1,
			                      x + Width - 2, y + 1);
		}
	}

	if (LookMC && !DoubleBorder)
	{
		SetFgPen (rp, MenSectGrey);
		WritePixel (rp, x, y + Height - 1);
		WritePixel (rp, x + Width - 1, y);
	}
}

void
DrawSmooth3DRect (
	struct RastPort *rp,
	LONG x,
	LONG y,
	LONG Width,
	LONG Height,
	BOOL Upward,
	BOOL HiRes,
	BOOL DoubleBorder)
{
	Draw3DRect (rp,x,y,Width,Height,Upward,HiRes,DoubleBorder);

	if (LookMC && !DoubleBorder)
	{
		if(Upward)
			SetFgPen (rp, MenStdGrey0);
		else
			SetFgPen (rp, MenStdGrey2);

		DrawLinePairs(rp, 3,
			x+2,y+Height-2,
			x+Width-2,y+Height-2,
			x+Width-2,y+2);

		if(Upward)
			SetFgPen (rp, MenStdGrey2);
		else
			SetFgPen (rp, MenStdGrey0);

		DrawLinePairs(rp, 3,
			x+Width-3,y+1,
			x+1,y+1,
			x+1,y+Height-3);
	}
}

void
DrawNormRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height)
{
	if (Width <= 0 || Height <= 0)
		return;

	SetFgPen (rp, MenBackGround);

	//      1-------->2
	//      ^         |
	//      |         |
	//      |         |
	//      |         v
	//      4<--------3

	DrawLinePairs (rp, 5, x, y,
	                      x + Width - 1, y,
	                      x + Width - 1, y + Height - 1,
	                      x, y + Height - 1,
	                      x, y);
}

void
GhostRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height)
{
	static UWORD GhostPattern[] = {0x1111, 0x4444};
	static UWORD CrosshatchPattern[] = {0x5555, 0xAAAA};

	UWORD *Pattern;

	if (Width <= 0 || Height <= 0)
		return;

	if(Look3D)
		Pattern = CrosshatchPattern;
	else
		Pattern = GhostPattern;

	SetAfPt (rp, Pattern, 1);

	// Hier gibt es ein Konsistenzproblem. Das Ghosting der Menüs,
	// wie es Intuition durchführt, ist an sich schon inkonsistent
	// mit dem von Gadgets. Macht man es wie Intuition, muß man
	// hier den Pen MenBackGround nehmen. Versucht man es konsistent
	// zu halten, sollte man eigentlich MenDarkEdge nehmen.
	//    -olsen

	SetPens(rp, MenBackGround, 0, JAM1);
	RectFill(rp, x, y, x + Width - 1, y + Height - 1);

	SetAfPt (rp, NULL, 0);
	SetFgPen (rp, MenTextCol);
}

void
CompRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height)
{
	if (Width > 0 && Height > 0)
	{
		SetDrawMask (rp, MenComplMsk);

		SetPens (rp, MenComplMsk, 0, COMPLEMENT);

		RectFill (rp, x, y, x + Width - 1, y + Height - 1);

		SetDrawMask (rp, ~0);

		SetDrMd (rp, JAM1);
	}
}

VOID
HiRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height, BOOL Highlighted, struct BackgroundCover * bgc)
{
	if (Width > 0 && Height > 0)
	{
		if(Highlighted)
			HighlightBackground (rp, x, y, x + Width - 1, y + Height - 1,bgc);
		else
			FillBackground (rp, x, y, x + Width - 1, y + Height - 1,bgc);
	}
}

BOOL
MoveMouse (LONG NewX, LONG NewY, BOOL AddEvent, struct InputEvent *Event, struct Screen *Scr)
{
	struct InputEvent *MyNewEvent;
	struct IEPointerPixel *MyNewPPixel;
	BOOL success = FALSE;

	if (MyNewPPixel = AllocVecPooled (sizeof (struct IEPointerPixel), MEMF_PUBLIC | MEMF_CLEAR))
	{
		if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR))
		{
			MyNewPPixel->iepp_Screen = Scr;
			MyNewPPixel->iepp_Position.X = max(0,NewX);
			MyNewPPixel->iepp_Position.Y = max(0,NewY);
			MyNewEvent->ie_Class = IECLASS_NEWPOINTERPOS;
			MyNewEvent->ie_SubClass = IESUBCLASS_PIXEL;
			MyNewEvent->ie_Code = IECODE_NOBUTTON;
			MyNewEvent->ie_EventAddress = (APTR) MyNewPPixel;

			InputIO->io_Command = IND_WRITEEVENT;
			InputIO->io_Data = (APTR) MyNewEvent;
			InputIO->io_Length = sizeof (struct InputEvent);
			DoIO ((struct IORequest *) InputIO);

			if (AddEvent)
			{
				CopyMem (Event, MyNewEvent, sizeof (struct InputEvent));
				MyNewEvent->ie_NextEvent = NULL;

				InputIO->io_Data = (APTR) MyNewEvent;
				InputIO->io_Length = sizeof (struct InputEvent);
				InputIO->io_Command = IND_WRITEEVENT;
				DoIO ((struct IORequest *) InputIO);
			}

			success = TRUE;

			FreeVecPooled (MyNewEvent);
		}

		FreeVecPooled (MyNewPPixel);
	}

	return (success);
}

VOID
SPrintf (STRPTR buffer, STRPTR formatString,...)
{
	va_list varArgs;

	va_start (varArgs, formatString);
	RawDoFmt (formatString, varArgs, (VOID (*)())(VOID (*))"\x16\xC0\x4E\x75", buffer);
	va_end (varArgs);
}

VOID
SetPens (struct RastPort * RPort, ULONG FgPen, ULONG BgPen, ULONG DrawMode)
{
	if (V39)
		SetABPenDrMd (RPort, FgPen, BgPen, DrawMode);
	else
	{
		if (FgPen != RPort->FgPen)
			SetAPen (RPort, FgPen);

		if (BgPen != RPort->BgPen)
			SetAPen (RPort, BgPen);

		if (DrawMode != RPort->DrawMode)
			SetDrMd (RPort, DrawMode);
	}
}

LONG
GetDrawMode(struct RastPort *rp)
{
	LONG old;

	if (V39)
		old = GetDrMd (rp);
	else
		old = rp->DrawMode;

	return(old);
}

LONG
GetFgPen(struct RastPort * rp)
{
	LONG old;

	if (V39)
		old = GetAPen (rp);
	else
		old = rp->FgPen;

	return(old);
}

VOID
SetFgPen (struct RastPort *rp, LONG pen)
{
	LONG old;

	if (V39)
		old = GetAPen (rp);
	else
		old = rp->FgPen;

	if (old != pen)
		SetAPen (rp, pen);
}

VOID
SetDrawMode (struct RastPort *rp, LONG mode)
{
	LONG old;

	if (V39)
		old = GetDrMd (rp);
	else
		old = rp->DrawMode;

	if (old != mode)
		SetDrMd (rp, mode);
}

VOID
SetDrawMask (struct RastPort *rp, LONG mask)
{
	if (V39)
		SetWriteMask (rp, mask);
	else
		rp->Mask = mask;
}

ULONG
GetBitMapDepth (struct BitMap *bm)
{
	if (V39)
	{
		UWORD	d;

		d = GetBitMapAttr( bm, BMA_DEPTH );

		if( CyberGfxBase )
		{
			if( GetCyberMapAttr( bm, CYBRMATTR_ISCYBERGFX ) )
			{
				UWORD t;

				t = GetCyberMapAttr( bm, CYBRMATTR_DEPTH );
				/*
				**	Need this when SAVEMEM is activated
				*/
				if( t > 8 ) d = t;
			}
		}

		return d;
	}

	return (bm->Depth);
}

LONG
findchar (char *Text, char Zeichen)
{
	LONG z1;

	z1 = 0;
	while (*Text && (*Text++ != Zeichen))
		z1++;
	if (*Text)
		return (z1);
	else
		return (-1);
}

VOID
SafeObtainSemaphoreShared (struct SignalSemaphore * Semaphore)
{
	if (SysBase->lib_Version >= 39)
		ObtainSemaphoreShared (Semaphore);
	else
	{
		if (!AttemptSemaphoreShared (Semaphore))
		{
			if (!AttemptSemaphore (Semaphore))
				ObtainSemaphoreShared (Semaphore);
		}
	}
}

/******************************************************************************/

VOID
DrawLine (struct RastPort *rp, LONG x0, LONG y0, LONG x1, LONG y1)
{
	if (x0 == x1 || y0 == y1)
	{
		if (x0 > x1)
			x0 ^= x1, x1 ^= x0, x0 ^= x1;

		if (y0 > y1)
			y0 ^= y1, y1 ^= y0, y0 ^= y1;

		RectFill (rp, x0, y0, x1, y1);
	}
	else
	{
		Move (rp, x0, y0);
		Draw (rp, x1, y1);
	}
}

VOID
DrawLinePairs (struct RastPort *rp, LONG totalPairs, LONG left, LONG top,...)
{
	LONG x0, y0, x1, y1;
	LONG *pairs;
	va_list args;

	x0 = left;
	y0 = top;

	va_start (args, top);

	pairs = (LONG *) args;

	while (--totalPairs > 0)
	{
		x1 = *pairs++;
		y1 = *pairs++;

		DrawLine (rp, x0, y0, x1, y1);

		x0 = x1;
		y0 = y1;
	}

	va_end (args);
}

/******************************************************************************/

VOID
PlaceText (struct RastPort *RPort, LONG Left, LONG Top, STRPTR String, LONG Len)
{
	if (Len < 0)
		Len = strlen (String);

	if (Len > 0)
	{
		Move (RPort, Left, Top);
		Text (RPort, String, Len);
	}
}

/******************************************************************************/

LONG
AllocateColour(struct ColorMap *ColorMap,ULONG Red,ULONG Green,ULONG Blue)
{
	LONG colour;

	colour = ObtainBestPen(ColorMap, Red, Green, Blue,
		OBP_FailIfBad,	TRUE,
		OBP_Precision,	AktPrefs.mmp_Precision,
	TAG_DONE);

	return(colour);
}

/******************************************************************************/

STATIC APTR ShadowBuffer = NULL;
STATIC LONG ShadowBufferSize = 0;

BOOL
AllocateShadowBuffer(LONG width,LONG height)
{
	LONG bytesNeeded = 3 * width * height;
	BOOL success;

	if(ShadowBuffer == NULL || ShadowBufferSize < bytesNeeded)
	{
#ifdef __MIXEDBINARY__
		if (PowerPCBase != NULL)
		{
			if( ShadowBuffer != NULL )
				FreeVecPooled(((ULONG *)ShadowBuffer)[-1]);

			if( ShadowBuffer = AllocVecPooled(bytesNeeded+56,MEMF_PUBLIC) )
			{
				ULONG	p = (((ULONG)ShadowBuffer)+39)&0xffffffe0;
				((ULONG *)p)[-1] = ShadowBuffer;
				ShadowBuffer = p;
			}
		}
		else
#endif
		{
			FreeVecPooled(ShadowBuffer);
			ShadowBuffer = AllocVecPooled(bytesNeeded,MEMF_ANY);
		}
		if(ShadowBuffer != NULL)
			ShadowBufferSize = bytesNeeded;
	}

	success = (BOOL)(ShadowBuffer != NULL);

	return(success);
}

/******************************************************************************/

VOID
FillBackground(struct RastPort * rp,LONG minX,LONG minY,LONG maxX,LONG maxY,struct BackgroundCover * bgc)
{
	SetPens (rp, MenBackGround, 0, JAM1);

	SHOWVALUE(minX);
	SHOWVALUE(minY);

	if(bgc != NULL)
	{
		SHOWVALUE(bgc->bgc_Left);
		SHOWVALUE(bgc->bgc_Top);

		BltBitMapRastPort(bgc->bgc_BitMap,minX - bgc->bgc_Left,minY - bgc->bgc_Top,rp,minX,minY,maxX-minX+1,maxY-minY+1,MINTERM_COPY);
	}
	else
	{
		RectFill (rp, minX, minY, maxX, maxY);
	}
}

/******************************************************************************/

VOID
DeleteBackgroundCover(struct BackgroundCover * bgc)
{
	if(bgc != NULL)
	{
		disposeBitMap(bgc->bgc_BitMap, FALSE);
		FreeVecPooled(bgc);
	}
}

STATIC int __inline
GetAverageLine(UBYTE *pix,LONG mod)
{
	int sum;

	sum = ((UWORD)pix[-mod-3]) + ((UWORD)pix[-mod]) + ((UWORD)pix[-mod+3]) +
	      ((UWORD)pix[    -3]) + ((UWORD)pix[   0]) + ((UWORD)pix[    +3]) +
	      ((UWORD)pix[ mod-3]) + ((UWORD)pix[ mod]) + ((UWORD)pix[ mod+3]);

	return((sum + 8) / 9);
}

STATIC VOID
BlurPixelBuffer(UBYTE * pix,LONG width,LONG height)
{
	LONG x,y,mod;

	mod = width*3;

	for(y = 0 ; y < height-1 ; y++)
	{
		if(y > 0)
		{
			for(x = 0 ; x < width-1 ; x++)
			{
				if(x > 0)
				{
					(*pix) = GetAverageLine(pix,mod);	pix++;
					(*pix) = GetAverageLine(pix,mod);	pix++;
					(*pix) = GetAverageLine(pix,mod);	pix++;
				}
				else
				{
					pix += 3;
				}
			}

			pix += 3;
		}
		else
		{
			pix += mod;
		}
	}
}

STATIC VOID
DarkenPixelBuffer(UBYTE * pix,LONG width,LONG height)
{
	LONG x,y;

	for(y = 0 ; y < height ; y++)
	{
		for(x = 0 ; x < width ; x++)
		{
			(*pix) = max(0,((WORD)(*pix)) - 63);pix++;
			(*pix) = max(0,((WORD)(*pix)) - 63);pix++;
			(*pix) = max(0,((WORD)(*pix)) - 63);pix++;
		}
	}
}

STATIC VOID
BrightenPixelBuffer(UBYTE * pix,LONG width,LONG height)
{
	LONG x,y;

	for(y = 0 ; y < height ; y++)
	{
		for(x = 0 ; x < width ; x++)
		{
			(*pix) = min(255,((WORD)(*pix)) + 63);pix++;
			(*pix) = min(255,((WORD)(*pix)) + 63);pix++;
			(*pix) = min(255,((WORD)(*pix)) + 63);pix++;
		}
	}
}

/*
STATIC VOID
OldTintPixelBuffer(UBYTE * pix,LONG width,LONG height,int r,int g,int b)
{
	UBYTE red[256];
	UBYTE green[256];
	UBYTE blue[256];
	LONG x,y,i;

	for(i = 0 ; i < 256 ; i++)
		red[i] = min(255,31 + (r * i + 254) / 255);

	for(i = 0 ; i < 256 ; i++)
		green[i] = min(255,31 + (g * i + 254) / 255);

	for(i = 0 ; i < 256 ; i++)
		blue[i] = min(255,31 + (b * i + 254) / 255);

	for(y = 0 ; y < height ; y++)
	{
		for(x = 0 ; x < width ; x++)
		{
			(*pix) = red[(*pix)];	pix++;
			(*pix) = green[(*pix)];	pix++;
			(*pix) = blue[(*pix)];	pix++;
		}
	}
}
*/

STATIC VOID
TintPixelBuffer(UBYTE * pix,LONG width,LONG height,int r,int g,int b)
{
	LONG x,y;

	for(y = 0 ; y < height ; y++)
	{
		for(x = 0 ; x < width ; x++)
		{
			(*pix) = min(255,((*pix) + r + 1) / 2);	pix++;
			(*pix) = min(255,((*pix) + g + 1) / 2);	pix++;
			(*pix) = min(255,((*pix) + b + 1) / 2);	pix++;
		}
	}
}

STATIC BOOL
TintPixelBuffer2(UBYTE * pix,LONG width,LONG height,struct BitMap *bm, LONG bwidth, LONG bheight)
{
	UBYTE	*buf;

	if( buf = AllocVecPooled( bwidth*3, MEMF_PUBLIC ) )
	{
		struct RastPort	rp;
		LONG 					x,y;

		InitRastPort( &rp );
		rp.BitMap = bm;

		for(y = 0 ; y < height ; y++)
		{
			ReadPixelArray( buf, 0,0, 0, &rp, 0, y % bheight, bwidth, 1, RECTFMT_RGB );

			for(x = 0 ; x < width ; x++)
			{
				int r,g,b, t = (x % bwidth)*3;

				r = buf[t++];
				g = buf[t++];
				b = buf[t];

				(*pix) = min(255,((*pix) + r + 1) / 2);	pix++;
				(*pix) = min(255,((*pix) + g + 1) / 2);	pix++;
				(*pix) = min(255,((*pix) + b + 1) / 2);	pix++;
			}
		}

		FreeVecPooled( buf );

		return FALSE;
	}

	return TRUE;
}

#ifdef __MIXEDBINARY__
struct BlitArgs {
	UBYTE	*src;
	ULONG	 srcmod, srcfmt;
	LONG	 srcx, srcy;

	UBYTE	*dst;
	ULONG	 dstmod, dstfmt;
	LONG	 dstx, dsty;

	LONG	 width, height;

	UBYTE	*buf;
};

BOOL PPCDirect = FALSE;

extern VOID BlurAndTintPixelBuffer2PPCDirect(struct BlitArgs *blit,struct BitMap *bm,LONG bmwidth, LONG bmheight);
extern VOID BlurAndTintPixelBufferPPC(UBYTE * pix,LONG width,LONG height,int r,int g,int b);
extern VOID BlurAndTintPixelBufferPPCDirect(struct BlitArgs *blit, int r,int g,int b);
extern VOID BlurAndTintPixelBuffer2PPC(UBYTE * pix,LONG width,LONG height,struct BitMap *bm,LONG bmwidth, LONG bmheight);
#endif


#define MOD(x,y) ((UWORD)(x) % (UWORD)(y))
#define MIN(x,y)	((x)>(y)?(y):(x))
#define RECTSIZEX(r) ((r)->MaxX-(r)->MinX+1)
#define RECTSIZEY(r) ((r)->MaxY-(r)->MinY+1)


void CopyTiledBitMap(struct BitMap *Src, WORD SrcOffsetX, WORD SrcOffsetY, WORD SrcSizeX, WORD SrcSizeY, struct BitMap *Dst, struct Rectangle *DstBounds)
{
	WORD FirstSizeX;				   /* the width of the rectangle to blit as the first column */
	WORD FirstSizeY;				   /* the height of the rectangle to blit as the first row */
	WORD SecondMinX;				   /* the left edge of the second column */
	WORD SecondMinY;				   /* the top edge of the second column */
	WORD SecondSizeX;				   /* the width of the second column */
	WORD SecondSizeY;				   /* the height of the second column */
	WORD Pos;						   /* used as starting position in the "exponential" blit */
	WORD Size;						   /* used as bitmap size in the "exponential" blit */

	FirstSizeX = MIN(SrcSizeX - SrcOffsetX, RECTSIZEX(DstBounds));		/* the width of the first tile, this is either the rest of the tile right to SrcOffsetX or the width of the dest rect, if the rect is narrow */
	SecondMinX = DstBounds->MinX + FirstSizeX;	/* the start for the second tile (if used) */
	SecondSizeX = MIN(SrcOffsetX, DstBounds->MaxX - SecondMinX + 1);	/* the width of the second tile (we want the whole tile to be SrcSizeX pixels wide, if we use SrcSizeX-SrcOffsetX pixels for the left part we'll use SrcOffsetX for the right part) */

	FirstSizeY = MIN(SrcSizeY - SrcOffsetY, RECTSIZEY(DstBounds));		/* the same values are calculated for y direction */
	SecondMinY = DstBounds->MinY + FirstSizeY;
	SecondSizeY = MIN(SrcOffsetY, DstBounds->MaxY - SecondMinY + 1);

	BltBitMap(Src, SrcOffsetX, SrcOffsetY, Dst, DstBounds->MinX, DstBounds->MinY, FirstSizeX, FirstSizeY, 0xC0, -1, NULL);		/* blit the first piece of the tile */
	if (SecondSizeX > 0)			   /* if SrcOffset was 0 or the dest rect was to narrow, we won't need a second column */
		BltBitMap(Src, 0, SrcOffsetY, Dst, SecondMinX, DstBounds->MinY, SecondSizeX, FirstSizeY, 0xC0, -1, NULL);
	if (SecondSizeY > 0)			   /* is a second row necessary? */
	{
		BltBitMap(Src, SrcOffsetX, 0, Dst, DstBounds->MinX, SecondMinY, FirstSizeX, SecondSizeY, 0xC0, -1, NULL);
		if (SecondSizeX > 0)
			BltBitMap(Src, 0, 0, Dst, SecondMinX, SecondMinY, SecondSizeX, SecondSizeY, 0xC0, -1, NULL);
	}

  /* this loop generates the first row of the tiles */
	for (Pos = DstBounds->MinX + SrcSizeX, Size = MIN(SrcSizeX, DstBounds->MaxX - Pos + 1); Pos <= DstBounds->MaxX;)
	{
		BltBitMap(Dst, DstBounds->MinX, DstBounds->MinY, Dst, Pos, DstBounds->MinY, Size, MIN(SrcSizeY, RECTSIZEY(DstBounds)), 0xC0, -1, NULL);
		Pos += Size;
		Size = MIN(Size << 1, DstBounds->MaxX - Pos + 1);
	}

  /* this loop blit the first row down several times to fill the whole dest rect */
	for (Pos = DstBounds->MinY + SrcSizeY, Size = MIN(SrcSizeY, DstBounds->MaxY - Pos + 1); Pos <= DstBounds->MaxY;)
	{
		BltBitMap(Dst, DstBounds->MinX, DstBounds->MinY, Dst, DstBounds->MinX, Pos, RECTSIZEX(DstBounds), Size, 0xC0, -1, NULL);
		Pos += Size;
		Size = MIN(Size << 1, DstBounds->MaxY - Pos + 1);
	}
}

struct BackgroundCover *
CreateBackgroundCover(
	struct BitMap *	friend,
	LONG left,
	LONG top,
	LONG width,
	LONG height,
	BOOL strip)
{
	struct BackgroundCover * bgc = NULL;

	if((AktPrefs.mmp_Transparency || BackFillBM != NULL) && LookMC && CyberGfxBase != NULL && (GetBitMapDepth(friend) >= 15) && AllocateShadowBuffer(width,height))
	{
		if( ! GlobalPopUp && ! AktPrefs.mmp_PDTransparent && ! BackFillBM )
			return NULL;

		bgc = AllocVecPooled(sizeof(*bgc),MEMF_ANY|MEMF_CLEAR);
		if(bgc != NULL)
		{
			bgc->bgc_BitMap = allocBitMap(GetBitMapDepth(friend), width, height, friend, FALSE);
			if(bgc->bgc_BitMap != NULL)
			{
				struct RastPort	rp;

				bgc->bgc_Width = width;
				bgc->bgc_Height = height;

				InitRastPort(&rp);
				rp.BitMap = bgc->bgc_BitMap;

				if( ! AktPrefs.mmp_TransBackfill )
				{
					if (BackFillBM != NULL)
					{
						struct Rectangle	rect = {
							0,0, width-1, height-1
						};
						CopyTiledBitMap( BackFillBM, 0,0, BFWidth,BFHeight, bgc->bgc_BitMap, &rect );
						return bgc;
					}
				}
				/* Fülle den Hintergrund mit BARBLOCKPEN, wenn wir den
				 * MenuStrip eines PullDownMenüs haben. Dies sieht (wohl?)
				 * besser aus, da der Titeltext nicht mehr durchscheint.
				 */
				if(strip&&!BackFillBM)
				{
					ULONG	tab[3], r, g, b;

					GetRGB32( MenScr->ViewPort.ColorMap, MenMenuBackCol, 1, tab );
					r = ( tab[0] >> 24 ) + ( AktPrefs.mmp_Background.R >> 24 ) + 1;
					g = ( tab[1] >> 24 ) + ( AktPrefs.mmp_Background.G >> 24 ) + 1;
					b = ( tab[2] >> 24 ) + ( AktPrefs.mmp_Background.B >> 24 ) + 1;
					r  = min(255,r/2);
					g  = min(255,g/2);
					b  = min(255,b/2);
					FillPixelArray(&rp,0,0,width,height,r<<16|g<<8|b);

					return bgc;
				}
#ifdef __MIXEDBINARY__
				if(PowerPCBase != NULL)
				{
					if( PPCDirect )
					{
						struct BlitArgs blit;
						APTR	 handle, handle2;

						if(handle = LockBitMapTags(bgc->bgc_BitMap,
							LBMI_PIXFMT, &blit.dstfmt,
							LBMI_BYTESPERROW, &blit.dstmod,
							LBMI_BASEADDRESS, &blit.dst,
							TAG_DONE))
						{
							if(handle2 = LockBitMapTags(friend,
								LBMI_PIXFMT, &blit.srcfmt,
								LBMI_BYTESPERROW, &blit.srcmod,
								LBMI_BASEADDRESS, &blit.src,
								TAG_DONE))
							{
								blit.width = width;
								blit.height = height;
								blit.srcx = left;
								blit.srcy = top;
								blit.dstx = 0;
								blit.dsty = 0;
								blit.buf = ShadowBuffer;

								if (BackFillBM) BlurAndTintPixelBuffer2PPCDirect(&blit, BackFillBM, BFWidth, BFHeight );
								else BlurAndTintPixelBufferPPCDirect(&blit, AktPrefs.mmp_Background.R >> 24,AktPrefs.mmp_Background.G >> 24,AktPrefs.mmp_Background.B >> 24);

								UnLockBitMap(handle2);
							}

							UnLockBitMap(handle);
						}
					}
					else
					{
						rp.BitMap = friend;
						ReadPixelArray(ShadowBuffer,0,0,width*3,&rp,left,top,width,height,RECTFMT_RGB);

						if(!BackFillBM) BlurAndTintPixelBufferPPC(ShadowBuffer,width,height,AktPrefs.mmp_Background.R >> 24,AktPrefs.mmp_Background.G >> 24,AktPrefs.mmp_Background.B >> 24);
						else BlurAndTintPixelBuffer2PPC(ShadowBuffer,width,height,BackFillBM,BFWidth,BFHeight);

						rp.BitMap = bgc->bgc_BitMap;
						WritePixelArray(ShadowBuffer,0,0,width*3,&rp,0,0,width,height,RECTFMT_RGB);
					}
				}
				else
#endif
				{
#if 0
					BltBitMap( friend, left,top, bgc->bgc_BitMap, 0,0, width,height, 0xc0, 0xff, NULL );
					ReadPixelArray(ShadowBuffer,0,0,width*3,&rp,0,0,width,height,RECTFMT_RGB);
#else
					rp.BitMap = friend;
					ReadPixelArray(ShadowBuffer,0,0,width*3,&rp,left,top,width,height,RECTFMT_RGB);
#endif

#ifdef AMITHLON
					if( BlurAndTintPixelBufferX86 != NULL )
					{
						ULONG	rgb;

						rgb  = ( AktPrefs.mmp_Background.R >> 8  ) & 0x00ff0000;
						rgb |= ( AktPrefs.mmp_Background.G >> 16 ) & 0x0000ff00;
						rgb |= ( AktPrefs.mmp_Background.B >> 24 ) & 0x000000ff;

						BlurAndTintPixelBufferX86(ShadowBuffer,(width<<16)|height,rgb);
					}
					else
#endif
					{
						BlurPixelBuffer(ShadowBuffer,width,height);
						if( BackFillBM ) TintPixelBuffer2(ShadowBuffer,width,height,BackFillBM,BFWidth,BFHeight);
						else TintPixelBuffer(ShadowBuffer,width,height,AktPrefs.mmp_Background.R >> 24,AktPrefs.mmp_Background.G >> 24,AktPrefs.mmp_Background.B >> 24);
					}

					rp.BitMap = bgc->bgc_BitMap;
					WritePixelArray(ShadowBuffer,0,0,width*3,&rp,0,0,width,height,RECTFMT_RGB);
				}
			}
			else
			{
				FreeVecPooled(bgc);
				bgc = NULL;
			}
		}
	}

	return(bgc);
}

STATIC VOID
DarkenPixelBufferRect(UBYTE * pix,LONG left,LONG top,LONG width,LONG height,LONG mod,LONG delta)
{
	UBYTE * p;
	LONG x,y;

	pix += 3*left + mod*top;

	for(y = 0 ; y < height ; y++)
	{
		p = pix;

		for(x = 0 ; x < width ; x++)
		{
			(*p) = max(0,((WORD)(*p)) - delta); p++;
			(*p) = max(0,((WORD)(*p)) - delta); p++;
			(*p) = max(0,((WORD)(*p)) - delta); p++;
		}

		pix += mod;
	}
}

/******************************************************************************/

VOID
DrawShadow(struct RastPort * rp,LONG minX,LONG minY,LONG maxX,LONG maxY,LONG part)
{
	BOOL done = FALSE;

	if(CyberGfxBase != NULL)
	{
		LONG width,height;

		width	= (maxX - minX) + 1;
		height	= (maxY - minY) + 1;

		if(AllocateShadowBuffer(width,height) && GetBitMapDepth(rp->BitMap) >= 15)
		{
			const int shadowLevel = 256/4;

			ReadPixelArray(ShadowBuffer,0,0,width*3,rp,minX,minY,width,height,RECTFMT_RGB);

			if(part == RIGHT_PART)
			{
				DarkenPixelBufferRect(ShadowBuffer,0,1,width-1,height-1,width*3,shadowLevel-1);
				DarkenPixelBufferRect(ShadowBuffer,0,0,width-1,1,width*3,(shadowLevel/2)-1);
				DarkenPixelBufferRect(ShadowBuffer,width-1,1,1,height-1,width*3,(shadowLevel/2)-1);
			}
			else
			{
				DarkenPixelBufferRect(ShadowBuffer,1,0,width-2,height-1,width*3,shadowLevel-1);
				DarkenPixelBufferRect(ShadowBuffer,0,0,1,height-1,width*3,(shadowLevel/2)-1);
				DarkenPixelBufferRect(ShadowBuffer,width-1,0,1,height-1,width*3,(shadowLevel/2)-1);
				DarkenPixelBufferRect(ShadowBuffer,1,height-1,width-2,1,width*3,(shadowLevel/2)-1);
			}

			WritePixelArray(ShadowBuffer,0,0,width*3,rp,minX,minY,width,height,RECTFMT_RGB);
			done = TRUE;
		}
	}

	if(NOT done)
		RectFill(rp,minX,minY,maxX,maxY);
}

/******************************************************************************/

VOID
HighlightBackground(struct RastPort * rp,LONG minX,LONG minY,LONG maxX,LONG maxY,struct BackgroundCover * bgc)
{
	BOOL done = FALSE;

	if(bgc != NULL)
	{
		LONG width,height;

		width = maxX-minX+1;
		height = maxY-minY+1;

		if(AllocateShadowBuffer(width,height))
		{
			ReadPixelArray(ShadowBuffer,0,0,width*3,rp,minX,minY,width,height,RECTFMT_RGB);

			if( ! AktPrefs.mmp_TransHighlight )
				BrightenPixelBuffer(ShadowBuffer,width,height);
			else
				TintPixelBuffer(ShadowBuffer,width,height, AktPrefs.mmp_FillCol.R >> 24, AktPrefs.mmp_FillCol.G >> 24, AktPrefs.mmp_FillCol.B >> 24);

			WritePixelArray(ShadowBuffer,0,0,width*3,rp,minX,minY,width,height,RECTFMT_RGB);

			done = TRUE;
		}
	}

	if(NOT done)
	{
		SetPens (rp, MenFillCol, 0, JAM1);
		RectFill (rp, minX, minY, maxX, maxY);
	}
}
