/*
**	$Id$
**
**	:ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif	/* _GLOBAL_H */

UWORD
SendIntuiMessage (ULONG Class, UWORD * Code, UWORD Qualifier,
		  APTR IAddress, struct Window * RecWin, UWORD TimeOutSec,
		  struct MsgPort * DestPort)

{
  struct IntuiMessage *Msg;
  struct timerequest *TimeReq;
  struct MsgPort *MyPort;
  BOOL Ende;
  UWORD Result;
  struct Message *Message;
  ULONG SignalMask;


  if (!DestPort)
  {
    if (!(MyPort = CreateMsgPort ()))
      return (SENDINTUI_NOPORT);
  }
  else
    MyPort = DestPort;

  if (!(Msg = AllocVecPooled (sizeof (struct IntuiMessage), MEMF_PUBLIC)))
      return (SENDINTUI_NOPORT);

  Msg->ExecMessage.mn_Node.ln_Type = NT_MESSAGE;
  Msg->ExecMessage.mn_Length = sizeof (struct IntuiMessage);

  Msg->ExecMessage.mn_ReplyPort = MyPort;

  Msg->Class = Class;
  Msg->Code = *Code;
  Msg->Qualifier = Qualifier;
  Msg->IAddress = IAddress;
  if (RecWin->IDCMPFlags & IDCMP_DELTAMOVE)
  {
    Msg->MouseX = 0;		/* Wir kennen die Mausdifferenz leider nicht... */
    Msg->MouseY = 0;
  }
  else
  {
    Msg->MouseX = RecWin->MouseX;
    Msg->MouseY = RecWin->MouseY;
  }
  Msg->Seconds = 0;
  Msg->Micros = 0;
  Msg->IDCMPWindow = RecWin;
  Msg->SpecialLink = NULL;

  TimeReq = NULL;

  if (RecWin->UserPort)
  {
    if (TimeOutSec > 0)
      TimeReq = SendTimeRequest (TimerIO, TimeOutSec, 0, MyPort);

    PutMsg (RecWin->UserPort, Msg);
    SignalMask = 1 << MyPort->mp_SigBit;

    if (DestPort)
    {
      IntuiMessagePending++;
      return (SENDINTUI_OK);
    }

    Ende = FALSE;

    while (!Ende)
    {
      while (!(Message = GetMsg (MyPort)))
	Wait (SignalMask);

      if (TimeReq && (Message == (struct Message *) TimeReq))
      {
	Ende = TRUE;
	Result = SENDINTUI_TIMEOUT;
	FreeVecPooled (TimeReq);
	/* Der Port wird NICHT gelöscht, es könnte ja noch Antwort
	   kommen */
      }

      if (Message == Msg)
      {
	Ende = TRUE;
	Result = SENDINTUI_OK;
	*Code = Msg->Code;
	FreeVecPooled (Msg);

	if (TimeReq)
	{
	  if (!CheckIO (TimeReq))
	  {
	    AbortIO (TimeReq);
	    WaitIO (TimeReq);
	  }
	  FreeVecPooled (TimeReq);
	}

	DeleteMsgPort (MyPort);
      }
    }
  }
  else
    Result = SENDINTUI_OK;

  return (Result);
}
