/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

UWORD
SendIntuiMessage (
		   ULONG Class,
		   UWORD * Code,
		   UWORD Qualifier,
		   APTR IAddress,
		   struct Window *ReceivingWindow,
		   ULONG IntuiLock,
		   BOOL WaitForReply)
{
  struct IntuiMessage *Sent;
  ULONG TimeMask;
  ULONG ReplyMask;
  UWORD Result;
  struct Message *Message;
  ULONG Signals;
  BOOL Done;

  if (!(Sent = AllocVecPooled (sizeof (struct ExtIntuiMessage), MEMF_PUBLIC | MEMF_CLEAR)))
  {
    if (WaitForReply)
      UnlockIBase (IntuiLock);

    return (SENDINTUI_NOPORT);
  }

  Sent->ExecMessage.mn_Node.ln_Type = NT_MESSAGE;
  Sent->ExecMessage.mn_Length = sizeof (struct IntuiMessage) - sizeof (struct Message);		/* Intuition macht's auch so... */
  Sent->ExecMessage.mn_ReplyPort = IMsgReplyPort;

  Sent->Class = Class;
  Sent->Code = *Code;
  Sent->Qualifier = Qualifier | IEQUALIFIER_RELATIVEMOUSE;
  Sent->IAddress = IAddress;

  CurrentTime (&Sent->Seconds, &Sent->Micros);

  if (!(ReceivingWindow->IDCMPFlags & IDCMP_DELTAMOVE))
  {
    Sent->MouseX = ReceivingWindow->MouseX;
    Sent->MouseY = ReceivingWindow->MouseY;
  }

  Sent->IDCMPWindow = ReceivingWindow;

  Result = SENDINTUI_OK;

  Forbid ();

  if (ReceivingWindow->UserPort)
  {
    PutMsg (ReceivingWindow->UserPort, Sent);
    IMsgReplyCount++;
  }
  else
  {
    FreeVecPooled (Sent);
    WaitForReply = FALSE;	/* Es kann keine Antwort geben. */
  }

  Permit ();

  if (WaitForReply)
  {
    UnlockIBase (IntuiLock);

    TimeMask = PORTMASK (TimeoutPort);
    ReplyMask = PORTMASK (IMsgReplyPort);

    TimeoutRequest->tr_node.io_Command = TR_ADDREQUEST;
    TimeoutRequest->tr_time.tv_secs = 3;
    TimeoutRequest->tr_time.tv_micro = 0;

    SetSignal (0, TimeMask);
    SendIO ((struct IORequest *) TimeoutRequest);

    Done = FALSE;

    do
    {
      Signals = Wait (ReplyMask | TimeMask);

      if (Signals & ReplyMask)
      {
	while (Message = (struct IntuiMessage *) GetMsg (IMsgReplyPort))
	{
	  if (Message == Sent)
	  {
	    Done = TRUE;

	    *Code = Sent->Code;
	  }

	  FreeVecPooled (Message);
	  IMsgReplyCount--;
	}

	if (Done)
	  break;
      }

      if (Signals & TimeMask)
      {
	Done = TRUE;

	Result = SENDINTUI_TIMEOUT;
      }
    }
    while (!Done);

    if (!CheckIO ((struct IORequest *) TimeoutRequest))
      AbortIO ((struct IORequest *) TimeoutRequest);

    WaitIO ((struct IORequest *) TimeoutRequest);
  }

  return (Result);
}
