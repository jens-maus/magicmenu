/*
**	$Id$
**
**	:ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif	/* _GLOBAL_H */

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

/*****************************************************************************************/

BOOL
MMCheckScreen (void)
{
  BOOL Wait;

  ObtainSemaphore (GetPointerSemaphore);

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

  ObtainSemaphore (GetPointerSemaphore);

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

  if (MMCheckScreen ())
  {
    Win = CallOpenWindow (NW);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Win = CallOpenWindow (NW);

  /*****************************************************************************************/

        if(Win)
                RegisterGlyphs((struct Window *)Win,NW,NULL);

  /*****************************************************************************************/

  return (Win);
}


ULONG __asm __saveds
MMOpenWindowTagList (register __a0 struct NewWindow * NW,
                     register __a1 struct TagItem * TI)

{
  ULONG Win;

  if (MMCheckScreen ())
  {
    Win = CallOpenWindowTagList (NW, TI);
    ReleaseSemaphore (MenuActSemaphore);
  }
  else
    Win = CallOpenWindowTagList (NW, TI);

  /*****************************************************************************************/

        if(Win)
                RegisterGlyphs((struct Window *)Win,NW,TI);

  /*****************************************************************************************/

  return (Win);
}



ULONG __asm __saveds
MMClearMenuStrip (register __a0 struct Window * W)

{
  ULONG Res;

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

        DiscardWindowGlyphs(W);

  /*****************************************************************************************/

  return (Res);
}


ULONG __asm __saveds
MMActivateWindow (register __a0 struct Window * W)

{
  ULONG Result;

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

  if (AttemptSemaphore (MenuActSemaphore))
  {
    Result = CallWindowToFront (W);
    ReleaseSemaphore (MenuActSemaphore);
    return (Result);
  }
  else
    return (FALSE);
}



/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*             Verschiedenes                                                */
/* */
/* ************************************************************************ */
/* ************************************************************************ */


UWORD
GetCol (struct DrawInfo * DrawInfo, UWORD PenNum, UWORD Default)


{
  UWORD Pen;

  Pen = DrawInfo->dri_Pens[PenNum] & 0xff;
  return ((UWORD) ((Pen <= 31) ? Pen : Default));
}

LONG
MyObtainPen(struct Screen *Screen, ULONG R, ULONG G, ULONG B)

{
    LONG   Best;
    ULONG   Diff, MinDiff, Sum;
    LONG   Col, z1;

    if(GfxVersion >= 39)
        return(ObtainBestPen(Screen->ViewPort.ColorMap,R,G,B,
                                OBP_Precision,      PRECISION_GUI,
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


void
ComplementImage (struct Image *Image)

{
  UWORD Words, z1;

  Words = (Image->Width + 15) / 16 * Image->Height * Image->Depth;
  for (z1 = 0; z1 < Words; z1++)
    Image->ImageData[z1] ^= 0xffff;
}


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
    if (Image->ImageData)
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

  *NewIOReq = *OrigIOReq;

  NewIOReq->tr_node.io_Message.mn_ReplyPort = ReplyPort;
  NewIOReq->tr_node.io_Command = TR_ADDREQUEST;
  NewIOReq->tr_time.tv_secs = Seconds;
  NewIOReq->tr_time.tv_micro = Micros;

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
    IntuiMessagePending--;
    return (TRUE);
  }
  return (FALSE);
}

BOOL
CheckEnde (void)

{
        UWORD Antw;

        if (IntuiMessagePending > 0)
        {
                Antw = SimpleRequest (NULL, "Magic Menu Message", "Sorry, Magic Menu cannot uninstall itself\nbecause there are unreplied messages pending!", "Ok|Force uninstall", NULL, 0, 0);

                if (Antw == 1)
                        return (FALSE);

                CxMsgPort = CreateMsgPort ();
                IntuiMessagePending = 0;
        }

        if(LibPatches)
        {
                RemovePatches();
                LibPatches = FALSE;
        }

        return(TRUE);
}


UWORD
CheckSendIntui (UWORD Error, const char *Typ)

{
  char ZwStr[200];

  switch (Error)
  {
  case SENDINTUI_OK:
  case SENDINTUI_NOUSERPORT:
    return (Error);
  case SENDINTUI_TIMEOUT:
    SPrintf (ZwStr, "Timeout occured:\nProgram doesn't respond to message!\nMessage Class: %s", Typ);
    SimpleRequest (NULL, "Magic Menu Message", ZwStr, "Ok", NULL, 0, 0);
    break;
  case SENDINTUI_NOPORT:
    DisplayBeep (NULL);
    break;
  }
  return (Error);
}

void
disposeBitMap (struct BitMap *BitMap, LONG Depth, LONG Width, LONG Height)

{
  LONG z1, NWidth;

  if (GfxVersion >= 39)
    FreeBitMap (BitMap);
  else
  {
    NWidth = (((Width - 1) / 64) + 1) * 64;

    for (z1 = 0; z1 < Depth; z1++)
    {
      if (BitMap->Planes[z1])
        FreeRaster (BitMap->Planes[z1], NWidth, Height);
    }

    FreeVecPooled (BitMap);
  }
}


struct BitMap *
allocBitMap (LONG Depth, LONG Width, LONG Height, struct BitMap *Friend)

{
  struct BitMap *BitMap;
  BOOL Error;
  LONG z1, NWidth;

  Error = FALSE;

  if (GfxVersion >= 39)
    return (AllocBitMap (Width, Height, Depth, (AktPrefs.mmp_ChunkyPlanes) ? NULL : BMF_MINPLANES, Friend));
  else
  {
    if (BitMap = AllocVecPooled (sizeof (struct BitMap), NULL))
    {
      InitBitMap (BitMap, Depth, Width, Height);

      NWidth = (((Width - 1) / 64) + 1) * 64;

      for (z1 = 0; z1 < Depth; z1++)
      {
        if (!(BitMap->Planes[z1] = (PLANEPTR) AllocRaster (NWidth, Height)))
          Error = TRUE;
      }

      if (Error)
      {
        disposeBitMap (BitMap, Depth, Width, Height);
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

  WaitBlit ();                  /* Well, strange, aber ohne gehts nicht... */

  if (LayerInfo)
    DisposeLayerInfo (LayerInfo);

  if (BitMap)
    disposeBitMap (BitMap, Depth, Width + 10, Height + 10);
}


STATIC ULONG Dummy(VOID) { return(0); }

STATIC struct Hook DummyHook = {{NULL}, (HOOKFUNC)Dummy};

BOOL
InstallRPort (LONG Depth, LONG Width, LONG Height,
              struct RastPort **RastPort,
              struct BitMap **BitMap,
              struct Layer_Info **LayerInfo,
              struct Layer **Layer,
              struct BitMap *Friend)

{
/*    Depth = (Depth > 2) ? 2 : Depth; */
  *BitMap = NULL;
  *RastPort = NULL;
  *Layer = NULL;
  *LayerInfo = NULL;
  if ((*BitMap = allocBitMap (Depth, Width + 10, Height + 10, Friend)) != NULL)
  {
    if (AktPrefs.mmp_UseLayer)
    {
      if (*LayerInfo = NewLayerInfo ())
      {
        struct Hook *Hook;

        if(LayersBase -> lib_Version >= 39)
                Hook = LAYERS_NOBACKFILL;
        else
                Hook = &DummyHook;

        if (*Layer = CreateUpfrontHookLayer (*LayerInfo, *BitMap, 0, 0, Width, Height, LAYERSIMPLE, Hook, NULL))        // Olsen
        {
//              SetRast((*Layer)->rp,0);        // Olsen

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
    disposeBitMap (*BitMap, Depth, Width + 10, Height + 10);
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
    SetAPen (rp, MenLightEdge);
  else
    SetAPen (rp, MenDarkEdge);

  if (HiRes && (! LookMC))
  {
    Move (rp, x + Width - 2, y);
    Draw (rp, x, y);
    Draw (rp, x, y + Height - 1);
    Move (rp, x + 1, y);
    Draw (rp, x + 1, y + Height - 2);
    if (DoubleBorder)
    {
      Move (rp, x + Width - 3, y + 2);
      Draw (rp, x + Width - 3, y + Height - 2);
      Draw (rp, x + 3, y + Height - 2);
      Move (rp, x + Width - 4, y + 3);
      Draw (rp, x + Width - 4, y + Height - 2);
    }
  }
  else
  {
    Move (rp, x + Width - 1, y);
    Draw (rp, x, y);
    Draw (rp, x, y + Height - 2);
    if (DoubleBorder)
    {
      Move (rp, x + Width - 2, y + 2);
      Draw (rp, x + Width - 2, y + Height - 2);
      Draw (rp, x + 1, y + Height - 2);
    }
  }

  if (Upward)
    SetAPen (rp, MenDarkEdge);
  else
    SetAPen (rp, MenLightEdge);

  if (HiRes && (! LookMC))
  {
    Move (rp, x + 1, y + Height - 1);
    Draw (rp, x + Width - 1, y + Height - 1);
    Draw (rp, x + Width - 1, y);
    Move (rp, x + Width - 2, y + Height - 1);
    Draw (rp, x + Width - 2, y + 1);
    if (DoubleBorder)
    {
      Move (rp, x + 2, y + Height - 2);
      Draw (rp, x + 2, y + 1);
      Draw (rp, x + Width - 4, y + 1);
      Move (rp, x + 3, y + Height - 3);
      Draw (rp, x + 3, y + 2);
    }
  }
  else
  {
    Move (rp, x, y + Height - 1);
    Draw (rp, x + Width - 1, y + Height - 1);
    Draw (rp, x + Width - 1, y + 1);
    if (DoubleBorder)
    {
      Move (rp, x + 1, y + Height - 2);
      Draw (rp, x + 1, y + 1);
      Draw (rp, x + Width - 2, y + 1);
    }
  }
}


void
DrawNormRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height)

{
  if (Width <= 0 || Height <= 0)
    return;

  SetAPen (rp, MenBackGround);

  Move (rp, x, y);
  Draw (rp, x + Width - 1, y);
  Draw (rp, x + Width - 1, y + Height - 1);
  Draw (rp, x, y + Height - 1);
  Draw (rp, x, y);
}


void
GhostRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height)

{
  if (Width <= 0 || Height <= 0)
    return;

  SetAfPt (rp, DitherPattern, 1);
  SetAPen (rp, MenBackGround);
  SetDrMd (rp, JAM1);

  RectFill (rp, x, y, x + Width - 1, y + Height - 1);

  SetAfPt (rp, NULL, 0);
  SetAPen (rp, MenTextCol);
}


void
CompRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height)

{
  if (Width <= 0 || Height <= 0)
    return;

  SetWrMsk (rp, MenComplMsk);
  SetDrMd (rp, COMPLEMENT);
  SetAPen (rp, MenComplMsk);
  RectFill (rp, x, y, x + Width - 1, y + Height - 1);

  SetWrMsk (rp, 0xff);
  SetDrMd (rp, JAM1);
}


void
HiRect (struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height, BOOL Highlited)

{
  if (Width <= 0 || Height <= 0)
    return;

  SetDrMd (rp, JAM1);
  SetAPen (rp, (Highlited) ? MenFillCol : MenBackGround);
  RectFill (rp, x, y, x + Width - 1, y + Height - 1);
}


BOOL
MoveMouse (UWORD NewX, UWORD NewY, BOOL AddEvent, struct InputEvent * Event, struct Screen * Scr)

{
  struct InputEvent *MyNewEvent;
  struct IEPointerPixel *MyNewPPixel;

  if (MyNewPPixel = AllocVecPooled (sizeof (struct IEPointerPixel), MEMF_PUBLIC | MEMF_CLEAR))
  {
    if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR))
    {
      MyNewPPixel->iepp_Screen = Scr;
      MyNewPPixel->iepp_Position.X = NewX;
      MyNewPPixel->iepp_Position.Y = NewY;
      MyNewEvent->ie_Class = IECLASS_NEWPOINTERPOS;
      MyNewEvent->ie_SubClass = IESUBCLASS_PIXEL;
      MyNewEvent->ie_Qualifier = NULL;
      MyNewEvent->ie_Code = IECODE_NOBUTTON;
      MyNewEvent->ie_EventAddress = (APTR) MyNewPPixel;
      MyNewEvent->ie_NextEvent = NULL;

      InputIO->io_Data = (APTR) MyNewEvent;
      InputIO->io_Length = sizeof (struct InputEvent);

      InputIO->io_Command = IND_WRITEEVENT;
      DoIO ((struct IORequest *) InputIO);

      if (AddEvent)
      {
        *MyNewEvent = *Event;
        MyNewEvent->ie_NextEvent = NULL;

        InputIO->io_Data = (APTR) MyNewEvent;
        InputIO->io_Length = sizeof (struct InputEvent);

        InputIO->io_Command = IND_WRITEEVENT;
        DoIO ((struct IORequest *) InputIO);
      }

      FreeVecPooled (MyNewEvent);
      FreeVecPooled (MyNewPPixel);
    }
    else
      return (FALSE);
  }
  else
    return (FALSE);

  return (TRUE);
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
/*             Prefs Funktionen                                             */
/* */
/* ************************************************************************ */
/* ************************************************************************ */


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
