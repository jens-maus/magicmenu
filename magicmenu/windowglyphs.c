/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

typedef struct WindowGlyphNode
{
  struct MinNode Link;

  struct Window *Window;
  struct Image *AmigaGlyph;
}
WindowGlyphNode;

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

	if (Node = (WindowGlyphNode *) AllocVecPooled (sizeof (WindowGlyphNode), MEMF_ANY | MEMF_PUBLIC))
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
