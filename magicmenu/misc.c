/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

/*****************************************************************************************/

ULONG __asm CallOpenWindow (register __a0 struct NewWindow *NW);
ULONG __asm CallOpenWindowTagList (register __a0 struct NewWindow *NW, register __a1 struct TagItem *TI);
ULONG __asm CallClearMenuStrip (register __a0 struct Window *W);
ULONG __asm CallSetMenuStrip (register __a0 struct Window *W, register __a1 struct Menu *MS);
ULONG __asm CallResetMenuStrip (register __a0 struct Window *W, register __a1 struct Menu *MS);
ULONG __asm CallCloseWindow (register __a0 struct Window *W);
ULONG __asm CallCloseWindow (register __a0 struct Window *W);
ULONG __asm CallActivateWindow (register __a0 struct Window *W);
ULONG __asm CallWindowToFront (register __a0 struct Window *W);
BOOL __asm CallModifyIDCMP (register __a0 struct Window *window, register __d0 ULONG flags);
struct RastPort *__asm CallObtainGIRPort (register __a0 struct GadgetInfo *GInfo);

/*****************************************************************************************/

BOOL
MMCheckScreen (void)
{
  BOOL Wait;

  SafeObtainSemaphoreShared (GetPointerSemaphore);

  Wait = (MenScr != NULL);

  ReleaseSemaphore (GetPointerSemaphore);

  if (!Wait)
    return (FALSE);

  ObtainSemaphore (MenuActSemaphore);

  return (TRUE);
}


BOOL __asm
MMCheckWindow (register __a0 struct Window * Win)
{
  BOOL Wait;

  SafeObtainSemaphoreShared (GetPointerSemaphore);

  Wait = (MenWin == Win);

  ReleaseSemaphore (GetPointerSemaphore);

  if (!Wait)
    return (FALSE);

  ObtainSemaphore (MenuActSemaphore);

  return (TRUE);
}


ULONG __asm __saveds
MMOpenWindow (register __a0 struct NewWindow * NW)

{
  ULONG Win;

  DB (kprintf ("|%s| in OpenWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckScreen ())
  {
    Win = CallOpenWindow (NW);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Win = CallOpenWindow (NW);

  /*****************************************************************************************/

  if (Win)
    RegisterGlyphs ((struct Window *) Win, NW, NULL);

  /*****************************************************************************************/

  return (Win);
}


ULONG __asm __saveds
MMOpenWindowTagList (register __a0 struct NewWindow * NW,
		     register __a1 struct TagItem * TI)

{
  ULONG Win;

  DB (kprintf ("|%s| in OpenWindowTagList patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckScreen ())
  {
    Win = CallOpenWindowTagList (NW, TI);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Win = CallOpenWindowTagList (NW, TI);

  /*****************************************************************************************/

  if (Win)
    RegisterGlyphs ((struct Window *) Win, NW, TI);

  /*****************************************************************************************/

  return (Win);
}



ULONG __asm __saveds
MMClearMenuStrip (register __a0 struct Window * W)

{
  ULONG Res;

  DB (kprintf ("|%s| in ClearMenuStrip patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (W))
  {
    Res = CallClearMenuStrip (W);
    ReleaseSemaphore (MenuActSemaphore);
    return (Res);
  }
  else
  {
    return (CallClearMenuStrip (W));
  }
}

ULONG __asm __saveds
MMSetMenuStrip (register __a0 struct Window * W,
		register __a1 struct Menu * MI)

{
  ULONG Res;

  DB (kprintf ("|%s| in SetMenuStrip patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (W))
  {
    Res = CallSetMenuStrip (W, MI);
    ClearRemember (W);
    ReleaseSemaphore (MenuActSemaphore);
    return (Res);
  }
  else
  {
    Res = CallSetMenuStrip (W, MI);
    ClearRemember (W);
    return (Res);
  }
}


ULONG __asm __saveds
MMResetMenuStrip (register __a0 struct Window * W,
		  register __a1 struct Menu * MI)

{
  ULONG Res;

  DB (kprintf ("|%s| in ResetMenuStrip patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (W))
  {
    Res = CallResetMenuStrip (W, MI);
    ReleaseSemaphore (MenuActSemaphore);
    return (Res);
  }
  else
  {
    Res = CallResetMenuStrip (W, MI);
    return (Res);
  }
}


ULONG __asm __saveds
MMCloseWindow (register __a0 struct Window * W)

{
  ULONG Res;

  DB (kprintf ("|%s| in CloseWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckScreen ())
  {
    ClearRemember (W);
    Res = CallCloseWindow (W);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
  {
    ClearRemember (W);
    Res = CallCloseWindow (W);
  }

  /*****************************************************************************************/

  DiscardWindowGlyphs (W);

  /*****************************************************************************************/

  return (Res);
}


ULONG __asm __saveds
MMActivateWindow (register __a0 struct Window * W)

{
  ULONG Result;

  DB (kprintf ("|%s| in ActivateWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallActivateWindow (W);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}


ULONG __asm __saveds
MMWindowToFront (register __a0 struct Window * W)

{
  ULONG Result;

  DB (kprintf ("|%s| in WindowToFront patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallWindowToFront (W);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}


BOOL __asm __saveds
MMModifyIDCMP (register __a0 struct Window * window,
	       register __d0 ULONG flags)

{
  BOOL Result;

  DB (kprintf ("|%s| in ModifyIDCMP patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckWindow (window))
  {
    Result = CallModifyIDCMP (window, flags);

    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Result = CallModifyIDCMP (window, flags);

  return (Result);
}

struct RastPort *__asm __saveds
MMObtainGIRPort (register __a0 struct GadgetInfo *GInfo)
{
  DB (kprintf ("|%s| in ObtainGIRPort patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (!AktPrefs.mmp_NonBlocking && GInfo)	/* GInfo kann tats�chlich (!) NULL sein. */
  {
    SafeObtainSemaphoreShared (GetPointerSemaphore);

    if (MenScr == GInfo->gi_Screen)
    {
      struct RastPort *Result;

      ReleaseSemaphore (GetPointerSemaphore);

      if (AttemptSemaphore (MenuActSemaphore))
      {
	Result = CallObtainGIRPort (GInfo);

	ReleaseSemaphore (MenuActSemaphore);
      }
      else
	Result = NULL;

      return (Result);
    }

    ReleaseSemaphore (GetPointerSemaphore);
  }

  return (CallObtainGIRPort (GInfo));
}

	/* A couple of useful minterms. */

#define MINTERM_ZERO		0
#define MINTERM_ONE		ABC | ABNC | ANBC | ANBNC | NABC | NABNC | NANBC | NANBNC
#define MINTERM_COPY		ABC | ABNC | NABC | NABNC
#define MINTERM_NOT_C		ABNC | ANBNC | NABNC | NANBNC
#define MINTERM_B_AND_C		ABC | NABC
#define MINTERM_NOT_B_AND_C	ANBC | NANBC
#define MINTERM_B_OR_C		ABC | ABNC | NABC | NABNC | ANBC | NANBC

	/* CreateBitMapFromImage(struct Image *Image,struct BitMap *BitMap):

	 *      Turn an Intuition Image into a Gfx BitMap.
	 */

VOID
CreateBitMapFromImage (const struct Image * Image, struct BitMap * BitMap)
{
  PLANEPTR Data;
  ULONG Modulo;
  LONG i, Mask;

  InitBitMap (BitMap, Image->Depth, Image->Width, Image->Height);

  Modulo = BitMap->BytesPerRow * BitMap->Rows;
  Data = (PLANEPTR) Image->ImageData;

  for (i = 0; i < BitMap->Depth; i++)
  {
    Mask = (1 << i);

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

	/* RecolourBitMap():

	 *      Remap a BitMap to use a different colour selection.
	 */

BOOL
RecolourBitMap (struct BitMap *Src, struct BitMap *Dst, UBYTE * Mapping, LONG DestDepth, LONG Width, LONG Height)
{
  struct BitMap *SingleMap;

  /* Create a single bitplane bitmap. */

  if (SingleMap = allocBitMap (1, Width, Height, NULL, TRUE))
  {
    struct BitMap *FullMap;

    /* Create a dummy bitmap. */

    if (FullMap = (struct BitMap *) AllocVecPooled (sizeof (struct BitMap), MEMF_ANY))
    {
      LONG i, Mask = (1L << Src->Depth) - 1;

      /* Make the dummy bitmap use the
       * single bitmap in all planes.
       */

      InitBitMap (FullMap, DestDepth, Width, Height);

      for (i = 0; i < DestDepth; i++)
	FullMap->Planes[i] = SingleMap->Planes[0];

      /* Clear the destination bitmap. */

      BltBitMap (Dst, 0, 0, Dst, 0, 0, Width, Height, MINTERM_ZERO, 0xFF, NULL);

      /* Is colour zero to be mapped to a non-zero colour? */

      if (Mapping[0])
      {
	/* Clear the single plane bitmap. */

	BltBitMap (SingleMap, 0, 0, SingleMap, 0, 0, Width, Height, MINTERM_ZERO, 1, NULL);

	/* Merge all source bitplane data. */

	BltBitMap (Src, 0, 0, FullMap, 0, 0, Width, Height, MINTERM_B_OR_C, Mask, NULL);

	/* Invert the single plane bitmap, to give us
	 * the zero colour bitmap we can work with.
	 */

	BltBitMap (SingleMap, 0, 0, SingleMap, 0, 0, Width, Height, MINTERM_NOT_C, 1, NULL);

	/* Now set all the bits for colour zero. */

	BltBitMap (FullMap, 0, 0, Dst, 0, 0, Width, Height, MINTERM_B_OR_C, Mapping[0], NULL);
      }

      /* Run down the colours. */

      for (i = 1; i <= Mask; i++)
      {
	/* Set the single plane bitmap to all 1's. */

	BltBitMap (SingleMap, 0, 0, SingleMap, 0, 0, Width, Height, MINTERM_ONE, 1, NULL);

	/* Isolate the pixels to match the colour
	 * specified in `i'.
	 */

	BltBitMap (Src, 0, 0, FullMap, 0, 0, Width, Height, MINTERM_B_AND_C, i, NULL);

	if (Mask ^ i)
	  BltBitMap (Src, 0, 0, FullMap, 0, 0, Width, Height, MINTERM_NOT_B_AND_C, Mask ^ i, NULL);

	/* Set the pixels in the destination bitmap,
	 * use the designated colour.
	 */

	BltBitMap (FullMap, 0, 0, Dst, 0, 0, Width, Height, MINTERM_B_OR_C, Mapping[i], NULL);
      }

      /* Free the temporary bitmap. */

      FreeVecPooled (FullMap);

      /* Free the single plane bitmap. */

      disposeBitMap (SingleMap, Width, Height, TRUE);

      /* Return the result. */

      return (TRUE);
    }

    disposeBitMap (SingleMap, Width, Height, TRUE);
  }

  return (FALSE);
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

      InitBitMap (&Dst, Depth, SrcImage->Width, SrcImage->Height);

      if ((*DestImage)->ImageData = (UWORD *) AllocVec (Dst.BytesPerRow * Dst.Rows * Dst.Depth, MEMF_CHIP))
      {
	CreateBitMapFromImage (*DestImage, &Dst);

	if (SrcImage->Depth == -1)
	{
	  struct RastPort TempRPort;

	  InitRastPort (&TempRPort);

	  if (TempRPort.BitMap = allocBitMap (Depth, SrcImage->Width, SrcImage->Height, NULL, TRUE))
	  {
	    BOOL Result;

	    DrawImage (&TempRPort, SrcImage, -SrcImage->LeftEdge, -SrcImage->TopEdge);

	    Result = RecolourBitMap (TempRPort.BitMap, &Dst, RemapArray, Depth, SrcImage->Width, SrcImage->Height);

	    disposeBitMap (TempRPort.BitMap, SrcImage->Width, SrcImage->Height, TRUE);

	    if (Result)
	      return (TRUE);
	  }
	}
	else
	{
	  struct BitMap Src;

	  CreateBitMapFromImage (SrcImage, &Src);

	  if (RecolourBitMap (&Src, &Dst, RemapArray, Depth, SrcImage->Width, SrcImage->Height))
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
    FreeVec (Image->ImageData);
    FreeVecPooled (Image);
  }
}


struct timerequest *
SendTimeRequest (struct timerequest *OrigIOReq,
		 ULONG Seconds,
		 ULONG Micros,
		 struct MsgPort *ReplyPort)

{
  struct timerequest *NewIOReq;

  if (!OrigIOReq)
    return (NULL);

  if (!(NewIOReq = AllocVecPooled (sizeof (struct timerequest), MEMF_PUBLIC)))
      return (NULL);

  CopyMem (OrigIOReq, NewIOReq, sizeof (struct timerequest));

  NewIOReq->tr_node.io_Message.mn_ReplyPort = ReplyPort;
  NewIOReq->tr_node.io_Command = TR_ADDREQUEST;
  NewIOReq->tr_time.tv_secs = Seconds;
  NewIOReq->tr_time.tv_micro = Micros;

  SetSignal (0, 1L << ReplyPort->mp_SigBit);
  SendIO (NewIOReq);

  return (NewIOReq);
}


long __stdargs
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
  struct Window *ReqWindow;
  struct MsgPort *MyPort;
  struct timerequest *TimeReq;
  struct Message *Message;
  BOOL DoWait, Ende;
  long Answer;
  ULONG IDCMP, MyIDCMP, *IDCMPPtr;
  ULONG SignalMask;

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

  if (!Seconds)
    return (EasyRequest (RefWindow, &ReqStruct, IDCMPPtr, Arg1));
  else
  {

    ReqWindow = BuildEasyRequestArgs (RefWindow, &ReqStruct, *IDCMPPtr, Arg1);

    if (ReqWindow == NULL || ReqWindow == (struct Window *) 1)
      return ((long) ReqWindow);

    if (!(MyPort = CreateMsgPort ()))
      return (FALSE);
    else
    {
      if (TimeReq = SendTimeRequest (TimerIO, Seconds, 0, MyPort))
      {
	SignalMask = 1 << ReqWindow->UserPort->mp_SigBit |
	  1 << MyPort->mp_SigBit;

	do
	{
	  DoWait = TRUE;
	  Ende = FALSE;

	  if (Message = GetMsg (MyPort))
	  {
	    if (Message == (struct Message *) TimeReq)
	    {
	      Ende = TRUE;
	      DoWait = FALSE;
	      Answer = -2;
	    }
	  }

	  if (!Ende)
	  {
	    IDCMP = *IDCMPPtr;
	    Answer = SysReqHandler (ReqWindow, &IDCMP, FALSE);
	    if (Answer > -1)
	    {
	      *IDCMPPtr = IDCMP;
	      Ende = TRUE;
	      DoWait = FALSE;
	    }
	  }

	  if (DoWait)
	    Wait (SignalMask);

	}
	while (!Ende);

	if (!CheckIO (TimeReq))
	{
	  AbortIO (TimeReq);
	  WaitIO (TimeReq);
	}
	FreeVecPooled (TimeReq);

      }
      DeleteMsgPort (MyPort);
    }

    FreeSysRequest (ReqWindow);

    return (Answer);
  }
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
    if (SimpleRequest (NULL, "Magic Menu Message", "Sorry, Magic Menu cannot uninstall itself\nbecause there are unreplied messages pending!", "Ok|Force uninstall", NULL, 0, 0))
      return (FALSE);
  }

  if (LibPatches)
  {
    RemovePatches ();
    LibPatches = FALSE;
  }

  return (TRUE);
}

void
disposeBitMap (struct BitMap *BitMap, LONG Width, LONG Height, BOOL IsChipMem)
{
  LONG z1;

  WaitBlit ();

  if (GfxVersion >= 39 && !IsChipMem)
    FreeBitMap (BitMap);
  else
  {
    for (z1 = 0; z1 < BitMap->Depth; z1++)
    {
      if (BitMap->Planes[z1])
	FreeRaster (BitMap->Planes[z1], Width, Height);
    }

    FreeVecPooled (BitMap);
  }
}


struct BitMap *
allocBitMap (LONG Depth, LONG Width, LONG Height, struct BitMap *Friend, BOOL WantChipMem)
{
  struct BitMap *BitMap;
  BOOL Error;
  LONG z1;

  Error = FALSE;

  if (GfxVersion >= 39 && !WantChipMem)
    return (AllocBitMap (Width, Height, Depth, (AktPrefs.mmp_ChunkyPlanes || Depth > 8) ? BMF_MINPLANES : NULL, Friend));
  else
  {
    if (BitMap = AllocVecPooled (sizeof (struct BitMap), NULL))
    {
      InitBitMap (BitMap, Depth, Width, Height);

      for (z1 = 0; z1 < Depth; z1++)
      {
	if (!(BitMap->Planes[z1] = (PLANEPTR) AllocRaster (Width, Height)))
	  Error = TRUE;
      }

      if (Error)
      {
	disposeBitMap (BitMap, Width, Height, TRUE);
	return (NULL);
      }

      return (BitMap);
    }
  }

  return (NULL);
}


void
FreeRPort (struct RastPort *RastPort,
	   struct BitMap *BitMap,
	   struct Layer_Info *LayerInfo,
	   struct Layer *Layer,
	   LONG Depth, LONG Width, LONG Height)

{
  if (Layer)
  {
    if (!DeleteLayer (0l, Layer))
    {
      DisplayBeep (NULL);
      return;
    }
  }
  else
  {
    if (RastPort)
      FreeVecPooled (RastPort);
  }

  if (LayerInfo)
    DisposeLayerInfo (LayerInfo);

  if (BitMap)
    disposeBitMap (BitMap, Width + 10, Height + 10, FALSE);
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
InstallRPort (LONG Depth, LONG Width, LONG Height,
	      struct RastPort ** RastPort,
	      struct BitMap ** BitMap,
	      struct Layer_Info ** LayerInfo,
	      struct Layer ** Layer,
	      struct BitMap * Friend)

{
  *BitMap = NULL;
  *RastPort = NULL;
  *Layer = NULL;
  *LayerInfo = NULL;
  if ((*BitMap = allocBitMap (Depth, Width + 10, Height + 10, Friend, FALSE)) != NULL)
  {
    if (AktPrefs.mmp_UseLayer)
    {
      if (*LayerInfo = NewLayerInfo ())
      {
	if (*Layer = CreateUpfrontHookLayer (*LayerInfo, *BitMap, 0, 0, Width, Height, LAYERSIMPLE, GetNOPFillHook (), NULL))	

	{
	  *RastPort = (*Layer)->rp;
	  return (TRUE);
	}
	DisposeLayerInfo (*LayerInfo);
      }
    }
    else
    {
      if (*RastPort = AllocVecPooled (sizeof (struct RastPort), NULL))
      {
	InitRastPort (*RastPort);
	(*RastPort)->BitMap = *BitMap;
	return (TRUE);
      }
    }
    disposeBitMap (*BitMap, Width + 10, Height + 10, FALSE);
  }
  return (FALSE);
}


struct ClipRect *
GetClipRect (struct BitMap *BitMap,
	     LONG x1, LONG y1, LONG x2, LONG y2)

{
  struct ClipRect *CRect;

  if (CRect = AllocVecPooled (sizeof (struct ClipRect) + 200, MEMF_CLEAR))
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
  if (Width <= 0 || Height <= 0)
    return;

  SetAfPt (rp, DitherPattern, 1);

  // Hier gibt es ein Konsistenzproblem. Das Ghosting der Men�s,
  // wie es Intuition durchf�hrt, ist an sich schon inkonsistent
  // mit dem von Gadgets. Macht man es wie Intuition, mu� man
  // hier den Pen MenBackGround nehmen. Versucht man es konsistent
  // zu halten, sollte man eigentlich MenDarkEdge nehmen.
  //    -olsen

  SetPens (rp, MenBackGround, 0, JAM1);		

  RectFill (rp, x, y, x + Width - 1, y + Height - 1);

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
HiRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height, BOOL Highlited)

{
  if (Width > 0 && Height > 0)
  {
      SetPens (rp, (Highlited) ? MenFillCol : MenBackGround, 0, JAM1);	

      RectFill (rp, x, y, x + Width - 1, y + Height - 1);
  }
}


BOOL
MoveMouse (UWORD NewX, UWORD NewY, BOOL AddEvent, struct InputEvent * Event, struct Screen * Scr)

{
  struct InputEvent *MyNewEvent;
  struct IEPointerPixel *MyNewPPixel;
  BOOL success = FALSE;

  if (MyNewPPixel = AllocVecPooled (sizeof (struct IEPointerPixel), MEMF_PUBLIC | MEMF_CLEAR))
  {
    if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR))
    {
      MyNewPPixel->iepp_Screen = Scr;
      MyNewPPixel->iepp_Position.X = NewX;
      MyNewPPixel->iepp_Position.Y = NewY;
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
  if (GfxVersion >= 39)
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

  if (GfxVersion >= 39)
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

  if (GfxVersion >= 39)
    old = GetDrMd (rp);
  else
    old = rp->DrawMode;

  if (old != mode)
    SetDrMd (rp, mode);
}

VOID
SetDrawMask (struct RastPort *rp, LONG mask)
{
  if (GfxVersion >= 39)
    SetWriteMask (rp, mask);
  else
    rp->Mask = mask;
}

ULONG
GetBitMapDepth (struct BitMap *BitMap)
{
  if (GfxVersion >= 39)
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
  if (SysBase->LibNode.lib_Version >= 39)
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
