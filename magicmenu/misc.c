/*
**   $Id$
**
**   :ts=8
*/

/*#define DEBUG*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

/*****************************************************************************************/

#define EXTRACT_BITS(v,offset,width) (((v) >> (offset)) & ((1 << width)-1))

/*****************************************************************************************/

#define CATCOMP_NUMBERS
#include "magicmenu.h"

/*****************************************************************************************/

ULONG __asm CallOpenWindow (REG(a0) struct NewWindow *NW, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallOpenWindowTagList (REG(a0) struct NewWindow *NW, REG(a1) struct TagItem *TI, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallClearMenuStrip (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallSetMenuStrip (REG(a0) struct Window *W, REG(a1) struct Menu *MS, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallResetMenuStrip (REG(a0) struct Window *W, REG(a1) struct Menu *MS, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallCloseWindow (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallActivateWindow (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallWindowToFront (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallWindowToBack (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallMoveWindowInFrontOf (REG(a0) struct Window *Window, REG(a1) struct Window *Behind, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallModifyIDCMP (REG(a0) struct Window *window, REG(d0) ULONG flags, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallOffMenu (REG(a0) struct Window *window, REG(d0) ULONG number, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallOnMenu (REG(a0) struct Window *window, REG(d0) ULONG number, REG(a6) struct IntuitionBase *IntuitionBase);
LONG __asm CallLendMenus (REG(a0) struct Window *FromWindow,REG(a1) struct Window *ToWindow, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallRefreshWindowFrame (REG(a0) struct Window *, REG(a6) struct IntuitionBase *);
ULONG __asm CallSetWindowTitles (REG(a0) struct Window *, REG(a1) STRPTR, REG(a2) STRPTR, REG(a6) struct IntuitionBase *);
struct RastPort *__asm CallObtainGIRPort (REG(a0) struct GadgetInfo *GInfo, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallOpenScreen (REG(a0) struct NewScreen *NS, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallOpenScreenTagList (REG(a0) struct NewScreen *NS, REG(a1) struct TagItem *TI, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallCloseScreen (REG(a0) struct Screen *S, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallScreenToFront (REG(a0) struct Screen *S, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallScreenToBack (REG(a0) struct Screen *S, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallScreenDepth (REG(a0) struct Screen *S, REG(d0) ULONG flags, REG(a1) reserved, REG(a6) struct IntuitionBase *IntuitionBase);

/*****************************************************************************************/

#if 0
#ifdef DB
#undef DB
#endif	/* DB */

/*#define DB(x)	;*/
#define DB(x) x

extern void kprintf(const char *,...);
#endif

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

ULONG __asm __saveds
MMOpenWindow (REG(a0) struct NewWindow * NW)
{
  ULONG Win;

  DB (kprintf ("|%s| in OpenWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMOpenWindowTagList (REG(a0) struct NewWindow * NW,
                     REG(a1) struct TagItem * TI)
{
  ULONG Win;

  DB (kprintf ("|%s| in OpenWindowTagList patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMClearMenuStrip (REG(a0) struct Window * W)
{
  ULONG Res;

  DB (kprintf ("|%s| in ClearMenuStrip patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMSetMenuStrip (REG(a0) struct Window * W,
                REG(a1) struct Menu * MI)
{
  ULONG Res;

  DB (kprintf ("|%s| in SetMenuStrip patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMResetMenuStrip (REG(a0) struct Window * W,
                  REG(a1) struct Menu * MI)
{
  ULONG Res;

  DB (kprintf ("|%s| in ResetMenuStrip patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMCloseWindow (REG(a0) struct Window * W)
{
  ULONG Res;
  BOOL CallSelf;

  CallSelf = (FindTask(NULL) == ThisTask);

  DB (kprintf ("|%s| in CloseWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMActivateWindow (REG(a0) struct Window * W)
{
  ULONG Result;

  DB (kprintf ("|%s| in ActivateWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallActivateWindow (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}

ULONG __asm __saveds
MMWindowToFront (REG(a0) struct Window * W)
{
  ULONG Result;

  /* Bug-Kompatibilität :( */
  if(!RealWindow(W))
    return(FALSE);

  DB (kprintf ("|%s| in WindowToFront patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckParentScreen (W, TRUE))
  {
    Result = CallWindowToFront (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallWindowToFront (W, IntuitionBase);

  return (Result);
}

ULONG __asm __saveds
MMWindowToBack (REG(a0) struct Window * W)
{
  ULONG Result;

  DB (kprintf ("|%s| in WindowToBack patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckParentScreen (W, TRUE))
  {
    Result = CallWindowToBack (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallWindowToBack (W, IntuitionBase);

  return (Result);
}

ULONG __asm __saveds
MMModifyIDCMP (REG(a0) struct Window * window,
               REG(d0) ULONG flags)
{
  ULONG Result;

  DB (kprintf ("|%s| in ModifyIDCMP patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (window))
  {
    Result = CallModifyIDCMP (window, flags, IntuitionBase);

    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallModifyIDCMP (window, flags, IntuitionBase);

  return (Result);
}

ULONG __asm __saveds
MMOffMenu (REG(a0) struct Window * window,
               REG(d0) ULONG number)
{
  ULONG Result;

  DB (kprintf ("|%s| in OffMenu patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (window))
  {
    Result = CallOffMenu (window, number, IntuitionBase);

    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallOffMenu (window, number, IntuitionBase);

  return (Result);
}

ULONG __asm __saveds
MMOnMenu (REG(a0) struct Window * window,
               REG(d0) ULONG number)
{
  ULONG Result;

  DB (kprintf ("|%s| in OnMenu patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (window))
  {
    Result = CallOnMenu (window, number, IntuitionBase);

    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallOnMenu (window, number, IntuitionBase);

  return (Result);
}

struct RastPort *__asm __saveds
MMObtainGIRPort (REG(a0) struct GadgetInfo *GInfo)
{
  DB (kprintf ("|%s| in ObtainGIRPort patch\n", FindTask (NULL)->tc_Node.ln_Name));

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

ULONG __asm __saveds
MMRefreshWindowFrame (REG(a0) struct Window * W)
{
  ULONG Result;

  /* Bug-Kompatibilität :( */
  if(!RealWindow(W))
    return(FALSE);

  DB (kprintf ("|%s| in RefreshWindowFrame patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckParentScreen (W, FALSE))
  {
    Result = CallRefreshWindowFrame (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallRefreshWindowFrame (W, IntuitionBase);

  return (Result);
}

ULONG __asm __saveds
MMSetWindowTitles (REG(a0) struct Window * W, REG(a1) STRPTR WindowTitle, REG(a2) STRPTR ScreenTitle)
{
  ULONG Result;

  /* Bug-Kompatibilität :( */
  if(!RealWindow(W))
    return(FALSE);

  DB (kprintf ("|%s| in SetWindowTitles patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckParentScreen (W, FALSE))
  {
    Result = CallSetWindowTitles (W, WindowTitle, ScreenTitle, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallSetWindowTitles (W, WindowTitle, ScreenTitle, IntuitionBase);

  return (Result);
}

ULONG __asm __saveds
MMOpenScreen (REG(a0) struct NewScreen *NS)
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

ULONG __asm __saveds
MMOpenScreenTagList (REG(a0) struct NewScreen *NS, REG(a1) struct TagItem *TI)
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

ULONG __asm __saveds
MMCloseScreen (REG(a0) struct Screen *S)
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

ULONG __asm __saveds
MMScreenToFront (REG(a0) struct Screen *S)
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

ULONG __asm __saveds
MMScreenToBack (REG(a0) struct Screen *S)
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

ULONG __asm __saveds
MMScreenDepth (REG(a0) struct Screen *S, REG(d0) ULONG flags, REG(a1) reserved)
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

LONG __asm __saveds
MMLendMenus (REG(a0) struct Window *FromWindow,REG(a1) struct Window *ToWindow)
{
  DB (kprintf ("|%s| in LendMenus patch\n", FindTask (NULL)->tc_Node.ln_Name));

  RegisterLending(FromWindow,ToWindow);

  return (CallLendMenus (FromWindow, ToWindow, IntuitionBase));
}

/*****************************************************************************************/

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
	extern VOID __asm RemapBitMap(REG(a0) struct BitMap *srcbm,REG(a1) struct BitMap *destbm,REG(a2) UBYTE *table,REG(d0) LONG width);

	WaitBlit();
	RemapBitMap(Src,Dst,Mapping,Width);
}

BOOL
MakeRemappedImage (struct Image ** DestImage, struct Image * SrcImage,
                   UWORD Depth, UBYTE * RemapArray)
{
  if (Depth > 8)
    Depth = 8;

  if (*DestImage = (struct Image *) AllocVecPooled (sizeof (struct Image), MEMF_ANY | MEMF_CLEAR))
  {
    (*DestImage)->LeftEdge = SrcImage->LeftEdge;
    (*DestImage)->TopEdge = SrcImage->TopEdge;
    (*DestImage)->Width = SrcImage->Width;
    (*DestImage)->Height = SrcImage->Height;
    (*DestImage)->Depth = Depth;

    if (SrcImage->PlanePick || SrcImage->Depth == -1)
    {
      struct BitMap Dst;

      (*DestImage)->PlanePick = (1L << Depth) - 1;

      memset(&Dst,0,sizeof(Dst));
      InitBitMap (&Dst, Depth, SrcImage->Width, SrcImage->Height);

      if ((*DestImage)->ImageData = (UWORD *) AllocVec (Dst.BytesPerRow * Dst.Rows * Dst.Depth, MEMF_CHIP))
      {
        UBYTE Mask;

        CreateBitMapFromImage (*DestImage, &Dst);

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

            disposeBitMap (TempRPort.BitMap, TRUE);

            return (TRUE);
          }
        }
        else
        {
          struct BitMap Src;

          CreateBitMapFromImage (SrcImage, &Src);

          RecolourBitMap (&Src, &Dst, RemapArray, Depth, SrcImage->Width, SrcImage->Height);

          return (TRUE);
        }
      }
    }
    else
    {
      (*DestImage)->PlaneOnOff = RemapArray[SrcImage->PlaneOnOff];

      return (TRUE);
    }

    FreeRemappedImage (*DestImage);
    *DestImage = NULL;
  }

  return (FALSE);
}

VOID
FreeRemappedImage (struct Image * Image)
{
  if (Image)
  {
    WaitBlit ();
    FreeVec (Image->ImageData);
    FreeVecPooled (Image);
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
    return (AllocBitMap (Width, Height, Depth, BMF_MINPLANES, Friend));
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
FreeRPort (struct BitMap *BitMap,
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
InstallRPort (LONG Left,LONG Top,LONG Depth, LONG Width, LONG Height,
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

          bgc = CreateBackgroundCover(Friend,Left,Top,OriginalWidth,OriginalHeight);

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
GetClipRect (struct BitMap *BitMap,
             LONG x1, LONG y1, LONG x2, LONG y2)
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
DrawSmooth3DRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height,
                  BOOL Upward, BOOL HiRes, BOOL DoubleBorder)
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

void
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
GetBitMapDepth (struct BitMap *BitMap)
{
  if (V39)
    return (GetBitMapAttr (BitMap, BMA_DEPTH));
  else
    return (BitMap->Depth);
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
		FreeVecPooled(ShadowBuffer);

		ShadowBuffer = AllocVecPooled(bytesNeeded,MEMF_ANY);
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

BOOL Transparency = FALSE;

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

STATIC VOID
TintPixelBuffer(UBYTE * pix,LONG width,LONG height,int r,int g,int b)
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

struct BackgroundCover *
CreateBackgroundCover(
	struct BitMap *	friend,
	LONG left,
	LONG top,
	LONG width,
	LONG height)
{
	struct BackgroundCover * bgc = NULL;

	if(Transparency && LookMC && CyberGfxBase != NULL && AllocateShadowBuffer(width,height) && GetCyberMapAttr(friend,CYBRMATTR_DEPTH) >= 15)
	{
		bgc = AllocVecPooled(sizeof(*bgc),MEMF_ANY|MEMF_CLEAR);
		if(bgc != NULL)
		{
			bgc->bgc_BitMap = allocBitMap(GetBitMapDepth(friend), width, height, friend, FALSE);
			if(bgc->bgc_BitMap != NULL)
			{
				struct RastPort rp;

				BltBitMap(friend,left,top,bgc->bgc_BitMap,0,0,width,height,MINTERM_COPY,~0,NULL);

				bgc->bgc_Width = width;
				bgc->bgc_Height = height;

				InitRastPort(&rp);
				rp.BitMap = bgc->bgc_BitMap;

				ReadPixelArray(ShadowBuffer,0,0,width*3,&rp,0,0,width,height,RECTFMT_RGB);

				BlurPixelBuffer(ShadowBuffer,width,height);

				WritePixelArray(ShadowBuffer,0,0,width*3,&rp,0,0,width,height,RECTFMT_RGB);
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

		if(AllocateShadowBuffer(width,height) && GetCyberMapAttr(rp->BitMap,CYBRMATTR_DEPTH) >= 15)
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

			BrightenPixelBuffer(ShadowBuffer,width,height);

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
