/*
**   $Id$
**
**   :ts=8
*/

/*#define DEBUG*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

typedef struct WindowGlyphNode
{
  struct MinNode Link;

  struct Window *Window;
  struct Image *AmigaGlyph;
  struct Window *LendingTo;
} WindowGlyphNode;

STATIC struct SignalSemaphore WindowGlyphSemaphore;
STATIC struct MinList WindowGlyphList;
STATIC BOOL Inited;

VOID
WindowGlyphExit ()
{
  if (Inited)
  {
    struct Node *Node;

    ObtainSemaphore (&WindowGlyphSemaphore);

    while (Node = RemHead ((struct List *) &WindowGlyphList))
      FreeVecPooled (Node);

    ReleaseSemaphore (&WindowGlyphSemaphore);
  }
}

VOID
WindowGlyphInit ()
{
  InitSemaphore (&WindowGlyphSemaphore);

  NewList ((struct List *) &WindowGlyphList);

  Inited = TRUE;
}

VOID
DiscardWindowGlyphs (struct Window *Window)
{
  if (Inited)
  {
    WindowGlyphNode *Node;

    ObtainSemaphore (&WindowGlyphSemaphore);

    for (Node = (WindowGlyphNode *) WindowGlyphList.mlh_Head; Node->Link.mln_Succ; Node = (WindowGlyphNode *) Node->Link.mln_Succ)
    {
      if (Node->Window == Window)
      {
	Remove ((struct Node *) Node);

	FreeVecPooled (Node);

	break;
      }
    }

    for (Node = (WindowGlyphNode *) WindowGlyphList.mlh_Head; Node->Link.mln_Succ; Node = (WindowGlyphNode *) Node->Link.mln_Succ)
    {
      if (Node->LendingTo == Window)
        Node->LendingTo = NULL;
    }

    ReleaseSemaphore (&WindowGlyphSemaphore);
  }
}

VOID
ObtainGlyphs (struct Window *Window, struct Image **TickGlyph, struct Image **AmigaGlyph)
{
  *TickGlyph = *AmigaGlyph = NULL;

  if (Inited)
  {
    WindowGlyphNode *Node;

    SafeObtainSemaphoreShared (&WindowGlyphSemaphore);

    for (Node = (WindowGlyphNode *) WindowGlyphList.mlh_Head; Node->Link.mln_Succ; Node = (WindowGlyphNode *) Node->Link.mln_Succ)
    {
      if (Node->Window == Window)
      {
	*AmigaGlyph = Node->AmigaGlyph;

	if (Window->CheckMark)
	  *TickGlyph = Window->CheckMark;

	break;
      }
    }

    ReleaseSemaphore (&WindowGlyphSemaphore);
  }
}

struct Window *
FindLending(struct Window *From)
{
	WindowGlyphNode *Node;
	struct Window *Result;

	Result = NULL;

	SafeObtainSemaphoreShared (&WindowGlyphSemaphore);

	for (Node = (WindowGlyphNode *) WindowGlyphList.mlh_Head; Node->Link.mln_Succ; Node = (WindowGlyphNode *) Node->Link.mln_Succ)
	{
		if (Node->Window == From)
		{
			Result = Node->LendingTo;
			break;
		}
	}

	ReleaseSemaphore (&WindowGlyphSemaphore);

	if(Result != NULL)
	{
		D(("window 0x%08lx |%s| uses menus of window 0x%08lx |%s|",From,From->Title,Result,Result->Title));
	}
	else
	{
		SHOWMSG("no menu lending for this one");
	}

	return(Result);
}

VOID
RegisterLending(struct Window *From,struct Window *To)
{
	if (Inited)
	{
		WindowGlyphNode *Node;
		BOOL GotIt;

		GotIt = FALSE;

		ObtainSemaphore (&WindowGlyphSemaphore);

		D(("window 0x%08lx |%s| lending menus to 0x%08lx |%s|",From,From->Title,To,To->Title));

		for (Node = (WindowGlyphNode *) WindowGlyphList.mlh_Head; Node->Link.mln_Succ; Node = (WindowGlyphNode *) Node->Link.mln_Succ)
		{
			if (Node->Window == From)
			{
				GotIt = TRUE;
				Node->LendingTo = To;
				break;
			}
		}

		/* Don't create a new node if we're not lending the
		 * node to any window.
		 */
		if(!GotIt && To != NULL)
		{
			if (Node = (WindowGlyphNode *) AllocVecPooled (sizeof (WindowGlyphNode), MEMF_ANY | MEMF_PUBLIC | MEMF_CLEAR))
			{
				Node->Window = From;
				Node->LendingTo = To;

				AddTail ((struct List *) &WindowGlyphList, (struct Node *) Node);
			}
		}

		ReleaseSemaphore (&WindowGlyphSemaphore);
	}
}

VOID
RegisterGlyphs (struct Window *Window, struct NewWindow *NewWindow, struct TagItem *Tags)
{
  if (Inited)
  {
    if (NewWindow && !Tags)
    {
      if (NewWindow->Flags & WFLG_NW_EXTENDED)
      {
	struct ExtNewWindow *ExtNewWindow = (struct ExtNewWindow *) NewWindow;

	if (TypeOfMem (ExtNewWindow->Extension))
	  Tags = ExtNewWindow->Extension;
      }
    }

    if (Tags)
    {
      struct Image *AmigaGlyph;

      if (AmigaGlyph = (struct Image *) GetTagData (WA_AmigaKey, NULL, Tags))
      {
	WindowGlyphNode *Node;

	if (Node = (WindowGlyphNode *) AllocVecPooled (sizeof (WindowGlyphNode), MEMF_ANY | MEMF_PUBLIC | MEMF_CLEAR))
	{
	  Node->Window = Window;
	  Node->AmigaGlyph = AmigaGlyph;

	  ObtainSemaphore (&WindowGlyphSemaphore);

	  AddTail ((struct List *) &WindowGlyphList, (struct Node *) Node);

	  ReleaseSemaphore (&WindowGlyphSemaphore);
	}
      }
    }
  }
}
