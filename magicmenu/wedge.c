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
#define SEMAPHORE_VERSION	2

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
  Wedge *WedgeCode;
  LONG Offset;
  APTR NewRoutine;
  ULONG *OldRoutine;
}
PatchEntry;

STATIC PatchEntry PatchTable[] =
{
  NULL, (LONG) & LVOCloseWindow, (APTR) MMCloseWindow, &OldCloseWindow,
  NULL, (LONG) & LVOOpenWindow, (APTR) MMOpenWindow, &OldOpenWindow,
  NULL, (LONG) & LVOOpenWindowTagList, (APTR) MMOpenWindowTagList, &OldOpenWindowTagList,
  NULL, (LONG) & LVOClearMenuStrip, (APTR) MMClearMenuStrip, &OldClearMenuStrip,
  NULL, (LONG) & LVOSetMenuStrip, (APTR) MMSetMenuStrip, &OldSetMenuStrip,
  NULL, (LONG) & LVOResetMenuStrip, (APTR) MMResetMenuStrip, &OldResetMenuStrip,
  NULL, (LONG) & LVOActivateWindow, (APTR) MMActivateWindow, &OldActivateWindow,
  NULL, (LONG) & LVOWindowToFront, (APTR) MMWindowToFront, &OldWindowToFront,
  NULL, (LONG) & LVOModifyIDCMP, (APTR) MMModifyIDCMP, &OldModifyIDCMP,
  NULL, (LONG) & LVOObtainGIRPort, (APTR) MMObtainGIRPort, &OldObtainGIRPort,
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

    for (i = 0; i < PATCHCOUNT; i++)
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

    if (Patches->Version < SEMAPHORE_VERSION || Patches->Owner)
    {
      ReleaseSemaphore (Patches);

      Patches = NULL;

      Permit ();

      return (TRUE);
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

    for (i = 0; i < PATCHCOUNT; i++)
    {
      Patches->Table[i].Command = JMP_ABS;
      Patches->Table[i].Location = PatchTable[i].NewRoutine;

      if (!Patches->Table[i].OldLocation)
	Patches->Table[i].OldLocation = SetFunction ((struct Library *) IntuitionBase, PatchTable[i].Offset, (ULONG (*)()) & Patches->Table[i]);

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
