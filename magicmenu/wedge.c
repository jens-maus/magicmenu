/*
**   $Id$
**
**   :ts=8
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

#define JMP_ABS 0x4EF9

#define SEMAPHORE_NAME		"MagicMenu Patches"
#define SEMAPHORE_VERSION	4

typedef struct Wedge
{
  UWORD Command;
  APTR Location;

  APTR OldLocation;
  UWORD Pad;
}
Wedge;

typedef struct PatchEntry
{
  APTR Base;
  Wedge *WedgeCode;
  LONG Offset;
  APTR NewRoutine;
  ULONG *OldRoutine;
}
PatchEntry;

extern LONG __far LVOOpenWindow;
extern LONG __far LVOOpenWindowTagList;
extern LONG __far LVOClearMenuStrip;
extern LONG __far LVOSetMenuStrip;
extern LONG __far LVOResetMenuStrip;
extern LONG __far LVOCloseWindow;
extern LONG __far LVOActivateWindow;
extern LONG __far LVOWindowToFront;
extern LONG __far LVOWindowToBack;
extern LONG __far LVOMoveWindowInFrontOf;
extern LONG __far LVOModifyIDCMP;
extern LONG __far LVOObtainGIRPort;

extern LONG __far LVOCreateUpfrontHookLayer;
extern LONG __far LVOCreateUpfrontLayer;

STATIC PatchEntry PatchTable[] =
{
  &IntuitionBase, NULL, (LONG) & LVOCloseWindow, (APTR) MMCloseWindow, &OldCloseWindow,
  &IntuitionBase, NULL, (LONG) & LVOOpenWindow, (APTR) MMOpenWindow, &OldOpenWindow,
  &IntuitionBase, NULL, (LONG) & LVOOpenWindowTagList, (APTR) MMOpenWindowTagList, &OldOpenWindowTagList,
  &IntuitionBase, NULL, (LONG) & LVOClearMenuStrip, (APTR) MMClearMenuStrip, &OldClearMenuStrip,
  &IntuitionBase, NULL, (LONG) & LVOSetMenuStrip, (APTR) MMSetMenuStrip, &OldSetMenuStrip,
  &IntuitionBase, NULL, (LONG) & LVOResetMenuStrip, (APTR) MMResetMenuStrip, &OldResetMenuStrip,
  &IntuitionBase, NULL, (LONG) & LVOActivateWindow, (APTR) MMActivateWindow, &OldActivateWindow,
  &IntuitionBase, NULL, (LONG) & LVOWindowToFront, (APTR) MMWindowToFront, &OldWindowToFront,
  &IntuitionBase, NULL, (LONG) & LVOWindowToBack, (APTR) MMWindowToBack, &OldWindowToBack,
  &IntuitionBase, NULL, (LONG) & LVOMoveWindowInFrontOf, (APTR) MMMoveWindowInFrontOf, &OldMoveWindowInFrontOf,
  &IntuitionBase, NULL, (LONG) & LVOModifyIDCMP, (APTR) MMModifyIDCMP, &OldModifyIDCMP,
  &IntuitionBase, NULL, (LONG) & LVOObtainGIRPort, (APTR) MMObtainGIRPort, &OldObtainGIRPort,

  &LayersBase, NULL, (LONG) & LVOCreateUpfrontHookLayer, (APTR) MMCreateUpfrontHookLayer, &OldCreateUpfrontHookLayer,
  &LayersBase, NULL, (LONG) & LVOCreateUpfrontLayer, (APTR) MMCreateUpfrontLayer, &OldCreateUpfrontLayer,
};

#define PATCHCOUNT (sizeof(PatchTable) / sizeof(PatchEntry))

typedef struct PatchSemaphore
{
  struct SignalSemaphore Semaphore;

  struct Process *Owner;
  LONG Version;

  Wedge Table[PATCHCOUNT];
  UBYTE Name[sizeof (SEMAPHORE_NAME)];
}
PatchSemaphore;

STATIC PatchSemaphore *Patches;

VOID
RemovePatches ()
{
  if (Patches)
  {
    LONG i;

    ObtainSemaphore (Patches);

    Forbid ();

    for (i = 0; i < PATCHCOUNT - (V39 ? 0 : 2) ; i++)
    {
      if (PatchTable[i].WedgeCode)
      {
	PatchTable[i].WedgeCode->Location = PatchTable[i].WedgeCode->OldLocation;

	PatchTable[i].WedgeCode = NULL;
      }
    }

    CacheClearU ();

    Permit ();

    Patches->Owner = NULL;

    ReleaseSemaphore (Patches);

    Patches = NULL;
  }
}

BOOL
InstallPatches ()
{
  Forbid ();

  if (Patches = (PatchSemaphore *) FindSemaphore (SEMAPHORE_NAME))
  {
    ObtainSemaphore (Patches);

    if (Patches->Version != SEMAPHORE_VERSION || Patches->Owner)
    {
      ReleaseSemaphore (Patches);

      Patches = NULL;

      Permit ();

      return (FALSE);
    }
  }
  else
  {
    if (Patches = (PatchSemaphore *) AllocMem (sizeof (PatchSemaphore), MEMF_ANY | MEMF_PUBLIC | MEMF_CLEAR))
    {
      strcpy (Patches->Name, SEMAPHORE_NAME);

      Patches->Semaphore.ss_Link.ln_Name = Patches->Name;
      Patches->Semaphore.ss_Link.ln_Pri = 1;

      Patches->Version = SEMAPHORE_VERSION;
      Patches->Owner = (struct Process *) FindTask (NULL);

      AddSemaphore (Patches);

      ObtainSemaphore (Patches);
    }
  }

  Permit ();

  if (Patches)
  {
    LONG i;

    Forbid ();

    for (i = 0; i < PATCHCOUNT - (V39 ? 0 : 2); i++)
    {
      Patches->Table[i].Command = JMP_ABS;
      Patches->Table[i].Location = PatchTable[i].NewRoutine;

      if (!Patches->Table[i].OldLocation)
	Patches->Table[i].OldLocation = SetFunction ((struct Library *) (*(ULONG *)PatchTable[i].Base), PatchTable[i].Offset, (ULONG (*)()) & Patches->Table[i]);

      *PatchTable[i].OldRoutine = (ULONG) Patches->Table[i].OldLocation;
      PatchTable[i].WedgeCode = &Patches->Table[i];
    }

    CacheClearU ();

    Permit ();

    ReleaseSemaphore (Patches);

    return (TRUE);
  }
  else
    return (FALSE);
}
