/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

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
ULONG __asm CallCloseWindow (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallActivateWindow (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
ULONG __asm CallWindowToFront (REG(a0) struct Window *W, REG(a6) struct IntuitionBase *IntuitionBase);
BOOL __asm CallModifyIDCMP (REG(a0) struct Window *window, REG(d0) ULONG flags, REG(a6) struct IntuitionBase *IntuitionBase);
struct RastPort *__asm CallObtainGIRPort (REG(a0) struct GadgetInfo *GInfo, REG(a6) struct IntuitionBase *IntuitionBase);
struct Layer *__asm CallCreateUpfrontHookLayer (REG(a0) struct Layer_Info *LayerInfo, REG(a1) struct BitMap *BitMap, REG(d0) LONG x0, REG(d1) LONG y0, REG(d2) LONG x1, REG(d3) LONG y1, REG(d4) ULONG Flags, REG(a3) struct Hook *Hook, REG(a2) struct BitMap *Super, REG(a6) struct Library *LayersBase);

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


BOOL
MMCheckWindow (struct Window * Win)
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

  if (Win)
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

  DB (kprintf ("|%s| in CloseWindow patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (MMCheckScreen ())
  {
    ClearRemember (W);
    Res = CallCloseWindow (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
  {
    ClearRemember (W);
    Res = CallCloseWindow (W, IntuitionBase);
  }

  /*****************************************************************************************/

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

  DB (kprintf ("|%s| in WindowToFront patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallWindowToFront (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}


ULONG __asm __saveds
MMWindowToBack (REG(a0) struct Window * W)

{
  ULONG Result;

  DB (kprintf ("|%s| in WindowToBack patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallWindowToBack (W, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}


ULONG __asm __saveds
MMMoveWindowInFrontOf (REG(a0) struct Window * Window,REG(a1) struct Window *Behind)

{
  ULONG Result;

  DB (kprintf ("|%s| in MoveWindowInFrontOf patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallMoveWindowInFrontOf (Window, Behind, IntuitionBase);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}


BOOL __asm __saveds
MMModifyIDCMP (REG(a0) struct Window * window,
               REG(d0) ULONG flags)

{
  BOOL Result;

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

struct RastPort *__asm __saveds
MMObtainGIRPort (REG(a0) struct GadgetInfo *GInfo)
{
  DB (kprintf ("|%s| in ObtainGIRPort patch\n", FindTask (NULL)->tc_Node.ln_Name));

  if (!AktPrefs.mmp_NonBlocking && GInfo)  /* GInfo kann tats�chlich (!) NULL sein. */
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

struct Layer *__saveds __asm
MMCreateUpfrontHookLayer (
                           REG(a0) struct Layer_Info *LayerInfo,
                           REG(a1) struct BitMap *BitMap,
                           REG(d0) LONG x0,
                           REG(d1) LONG y0,
                           REG(d2) LONG x1,
                           REG(d3) LONG y1,
                           REG(d4) ULONG Flags,
                           REG(a3) struct Hook *Hook,
                           REG(a2) struct BitMap *Super,
                           REG(a6) struct Library *LayersBase)
{
  struct Layer *Layer;

  /* Bei SuperBitMap wird nichts ge�ndert */

  if (Flags & LAYERSUPER)
    Layer = CallCreateUpfrontHookLayer (LayerInfo, BitMap, x0, y0, x1, y0, Flags, Hook, Super, LayersBase);
  else
  {
    /* Wichtig: Der Layer wird gleich nach der
       *          Erzeugung ver�ndert und es mu�
       *          verhindert werden, da� ein anderer
       *          Task an den Layers herumpfuscht, w�hrend
       *          hier noch gearbeitet wird.
     */

    LockLayerInfo (LayerInfo);

    /* Der Layer wird als Simple-Refresh und ohne Backfill-Funktion
       * erzeugt. Das hat zwei Vorteile: da der Layer im Hintergrund
       * erzeugt wird, entstehen automatisch Damage-Regions, wenn es
       * sich um einen Smart-Refresh-Layer handelt; dies wird vermieden,
       * indem er als Simple-Refresh-Layer erzeugt wird. Au�erdem wird
       * das Ausf�llen des Layers bis nach seiner Erzeugung verz�gert.
     */

    if (Layer = CreateBehindHookLayer (LayerInfo, BitMap, x0, y0, x1, y1, (Flags & ~LAYERSMART) | LAYERSIMPLE, LAYERS_NOBACKFILL, Super))
    {
      /* In den Vordergrund damit, Backdrop-Layers werden automatisch ber�cksichtigt */

      UpfrontLayer (NULL, Layer);

      /* Die urspr�nglichen Flags (LAYERSIMPLE oder LAYERSMART)
         * werden wieder eingetragen
       */

      Layer->Flags = (Layer->Flags & ~(LAYERSIMPLE | LAYERSMART)) | (Flags & (LAYERSIMPLE | LAYERSMART));

      /* Der urspr�ngliche Hook ebenfalls */

      Layer->BackFill = Hook;

      /* Weg mit der Damage-Liste, dieser Layer liegt im Vordergrund */

      ClearRegion (Layer->DamageList);

      /* Jetzt noch weg mit den Refresh-Flags */

      Layer->Flags &= ~(LAYERIREFRESH | LAYERIREFRESH2 | LAYERREFRESH);

      /* Jetzt wird die entsprechende Backfillfunktion aufgerufen */

      if (Hook != LAYERS_NOBACKFILL)
        DoHookClipRects (Hook, Layer->rp, NULL);
    }

    /* Wieder loslassen */

    UnlockLayerInfo (LayerInfo);
  }

  /* Und fertig... */

  return (Layer);
}

struct Layer *__asm
MMCreateUpfrontLayer (
                       REG(a0) struct Layer_Info *LayerInfo,
                       REG(a1) struct BitMap *BitMap,
                       REG(d0) LONG x0,
                       REG(d1) LONG y0,
                       REG(d2) LONG x1,
                       REG(d3) LONG y1,
                       REG(d4) ULONG Flags,
                       REG(a2) struct BitMap *Super,
                       REG(a6) struct Library *LayersBase)
{
  /* Sollte so eigentlich nicht n�tig sein, aber die layers.library springt nicht
   * durch den LVO in diese Routine.
   */
  return (CreateUpfrontHookLayer (LayerInfo, BitMap, x0, y0, x1, y1, Flags, LAYERS_BACKFILL, Super));
}

/*****************************************************************************************/

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

BOOL
RecolourBitMap (struct BitMap *Src, struct BitMap *Dst, UBYTE * Mapping, LONG DestDepth, LONG Width, LONG Height)
{
#if 1
	extern VOID __asm RemapBitMap(REG(a0) struct BitMap *srcbm,REG(a1) struct BitMap *destbm,REG(a2) UBYTE *table,REG(d0) LONG width);

	WaitBlit();
	RemapBitMap(Src,Dst,Mapping,Width);

	return(TRUE);
#endif
#if 0
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

      InitBitMap (FullMap, max(Src->Depth,DestDepth), Width, Height);

      for (i = 0; i < FullMap->Depth; i++)
        FullMap->Planes[i] = SingleMap->Planes[0];

      /* Clear the destination bitmap. */

      BltBitMap (Dst, 0, 0, Dst, 0, 0, Width, Height, MINTERM_ZERO, ~0, NULL);

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

        /* Isolate the pixels to match the colour specified in `i'. */

        BltBitMap (Src, 0, 0, FullMap, 0, 0, Width, Height, MINTERM_B_AND_C, i, NULL);

        if (Mask ^ i)
          BltBitMap (Src, 0, 0, FullMap, 0, 0, Width, Height, MINTERM_NOT_B_AND_C, Mask ^ i, NULL);

        /* Set the pixels in the destination bitmap, use the designated colour. */

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
#endif
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
    WaitBlit ();
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

  memcpy (NewIOReq, OrigIOReq, sizeof (struct timerequest));

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
    SimpleRequest (NULL, "MagicMenu", GetString(MSG_CANNOT_UNINSTALL_TXT), "Ok", NULL, 0, 0);

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

  if (V39 && !IsChipMem)
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

  if (V39 && !WantChipMem)
/*  return (AllocBitMap (Width, Height, Depth, (AktPrefs.mmp_ChunkyPlanes || Depth >= 8) ? BMF_MINPLANES : NULL, Friend));*/
    return (AllocBitMap (Width, Height, Depth, BMF_MINPLANES, Friend));
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
FreeRPort (struct BitMap *BitMap,
           struct Layer_Info *LayerInfo,
           struct Layer *Layer,
           LONG Width, LONG Height)

{
  if (Layer)
    DeleteLayer (NULL, Layer);

  if (LayerInfo)
    DisposeLayerInfo (LayerInfo);

  if (BitMap)
    disposeBitMap (BitMap, Width, Height, FALSE);
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
              struct RastPort ** RastPortPtr,
              struct BitMap ** BitMapPtr,
              struct Layer_Info ** LayerInfoPtr,
              struct Layer ** LayerPtr,
              struct BitMap * Friend)

{
  struct BitMap *BitMap;
  struct Layer_Info *LayerInfo;
  struct Layer *Layer;

  *BitMapPtr = NULL;
  *RastPortPtr = NULL;
  *LayerPtr = NULL;
  *LayerInfoPtr = NULL;

  if (BitMap = allocBitMap (Depth, Width, Height, Friend, FALSE))
  {
    if (LayerInfo = NewLayerInfo ())
    {
      if (Layer = CreateBehindHookLayer (LayerInfo, BitMap, 0, 0, Width - 1, Height - 1, LAYERSIMPLE, GetNOPFillHook (), NULL))
      {
        *BitMapPtr = BitMap;
        *RastPortPtr = Layer->rp;
        *LayerPtr = Layer;
        *LayerInfoPtr = LayerInfo;

        return (TRUE);
      }
      DisposeLayerInfo (LayerInfo);
    }
    disposeBitMap (BitMap, Width, Height, FALSE);
  }

  return (FALSE);
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

		disposeBitMap(Temp,Width,Height,FALSE);
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
        memcpy (MyNewEvent, Event, sizeof (struct InputEvent));
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
