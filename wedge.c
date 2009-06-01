/***************************************************************************

 MagicMenu - Intuition PopupMenu enhancement

 Copyright (C) 1993-1997 by Martin Korndörfer
 Copyright (C) 1997-2001 by Olaf `Olsen' Barthel
 Copyright (C) 2001 by Stephan Rupprecht, Jens Langner

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 MagicMenu Official Support Site :  http://www.magicmenu.de/

 $Id$

***************************************************************************/

/*#define DEBUG*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

#ifdef __STORMGCC__
#define __far
#endif

/****************************************************************************/

STATIC APTR __ASM SAVEDS LocalSetFunction(REG(a1, struct Library * library),
                                 REG(a0, WORD funcOffset),
                                 REG(d0, APTR newFunction),
                                 REG(a6, struct Library * SysBase));

/****************************************************************************/

#define JMP_ABS 0x4EF9

/****************************************************************************/

#define SEMAPHORE_NAME		"MagicMenu Patches"
#define SEMAPHORE_VERSION	13

/****************************************************************************/

typedef struct LibraryVector
{
  UWORD Command;
  APTR Location;
} LibraryVector;

#define WEDGE_MAGIC 0x1511

typedef struct Wedge
{
  UWORD Command;
  APTR Location;

  UWORD Magic;
  APTR PointBack;
  APTR OldLocation;
  struct PatchEntry * Patch;
} Wedge;

typedef struct PatchEntry
{
  APTR Base;
  LONG Version;
  Wedge *WedgeCode;
  LONG Offset;
  APTR NewRoutine;
  ULONG *OldRoutine;
} PatchEntry;

/****************************************************************************/

extern LONG __far LVOOpenWindow;
extern LONG __far LVOOpenWindowTagList;
extern LONG __far LVOCloseScreen;
extern LONG __far LVOOpenScreen;
extern LONG __far LVOOpenScreenTagList;
extern LONG __far LVOCloseScreen;
extern LONG __far LVOScreenToFront;
extern LONG __far LVOScreenToBack;
extern LONG __far LVOScreenDepth;
extern LONG __far LVOClearMenuStrip;
extern LONG __far LVOSetMenuStrip;
extern LONG __far LVOResetMenuStrip;
extern LONG __far LVOCloseWindow;
extern LONG __far LVOActivateWindow;
extern LONG __far LVOWindowToFront;
extern LONG __far LVOWindowToBack;
extern LONG __far LVOModifyIDCMP;
extern LONG __far LVOObtainGIRPort;
extern LONG __far LVOLendMenus;
extern LONG __far LVOOffMenu;
extern LONG __far LVOOnMenu;
extern LONG __far LVOSetWindowTitles;
extern LONG __far LVORefreshWindowFrame;

extern LONG __far LVOCreateUpfrontHookLayer;
extern LONG __far LVOCreateUpfrontLayer;

extern LONG __far LVOSetFunction;
extern LONG __far LVOForbid;
extern LONG __far LVOPermit;

STATIC PatchEntry PatchTable[] =
{
  &IntuitionBase, 37, NULL, (LONG) & LVOCloseScreen, (APTR) MMCloseScreen, &OldCloseScreen,
  &IntuitionBase, 37, NULL, (LONG) & LVOOpenScreen, (APTR) MMOpenScreen, &OldOpenScreen,
  &IntuitionBase, 37, NULL, (LONG) & LVOOpenScreenTagList, (APTR) MMOpenScreenTagList, &OldOpenScreenTagList,
  &IntuitionBase, 37, NULL, (LONG) & LVOScreenToFront, (APTR) MMScreenToFront, &OldScreenToFront,
  &IntuitionBase, 37, NULL, (LONG) & LVOScreenToBack, (APTR) MMScreenToBack, &OldScreenToBack,
  &IntuitionBase, 39, NULL, (LONG) & LVOScreenDepth, (APTR) MMScreenDepth, &OldScreenDepth,

  &IntuitionBase, 37, NULL, (LONG) & LVOCloseWindow, (APTR) MMCloseWindow, &OldCloseWindow,
  &IntuitionBase, 37, NULL, (LONG) & LVOOpenWindow, (APTR) MMOpenWindow, &OldOpenWindow,
  &IntuitionBase, 37, NULL, (LONG) & LVOOpenWindowTagList, (APTR) MMOpenWindowTagList, &OldOpenWindowTagList,
  &IntuitionBase, 37, NULL, (LONG) & LVOClearMenuStrip, (APTR) MMClearMenuStrip, &OldClearMenuStrip,
  &IntuitionBase, 37, NULL, (LONG) & LVOSetMenuStrip, (APTR) MMSetMenuStrip, &OldSetMenuStrip,
  &IntuitionBase, 37, NULL, (LONG) & LVOResetMenuStrip, (APTR) MMResetMenuStrip, &OldResetMenuStrip,
  &IntuitionBase, 37, NULL, (LONG) & LVOActivateWindow, (APTR) MMActivateWindow, &OldActivateWindow,
  &IntuitionBase, 37, NULL, (LONG) & LVOWindowToFront, (APTR) MMWindowToFront, &OldWindowToFront,
  &IntuitionBase, 37, NULL, (LONG) & LVOWindowToBack, (APTR) MMWindowToBack, &OldWindowToBack,
  &IntuitionBase, 37, NULL, (LONG) & LVOModifyIDCMP, (APTR) MMModifyIDCMP, &OldModifyIDCMP,
  &IntuitionBase, 37, NULL, (LONG) & LVOObtainGIRPort, (APTR) MMObtainGIRPort, &OldObtainGIRPort,
  &IntuitionBase, 37, NULL, (LONG) & LVOOffMenu, (APTR) MMOffMenu, &OldOffMenu,
  &IntuitionBase, 37, NULL, (LONG) & LVOOnMenu, (APTR) MMOnMenu, &OldOnMenu,
  &IntuitionBase, 37, NULL, (LONG) & LVOSetWindowTitles, (APTR) MMSetWindowTitles, &OldSetWindowTitles,
  &IntuitionBase, 37, NULL, (LONG) & LVORefreshWindowFrame, (APTR) MMRefreshWindowFrame, &OldRefreshWindowFrame,
  &IntuitionBase, 39, NULL, (LONG) & LVOLendMenus, (APTR) MMLendMenus, &OldLendMenus,

  &SysBase,       37, NULL, (LONG) & LVOSetFunction, (APTR) LocalSetFunction, &OldSetFunction,
};

#define PATCHCOUNT (sizeof(PatchTable) / sizeof(PatchEntry))

typedef struct PatchSemaphore
{
  struct SignalSemaphore Semaphore;

  struct Process *Owner;
  LONG Version;

  Wedge Table[PATCHCOUNT];
  UBYTE Name[sizeof (SEMAPHORE_NAME)];
} PatchSemaphore;

STATIC PatchSemaphore *Patches;

/****************************************************************************/

APTR __ASM CallSetFunction(REG(a1, struct Library * library),REG(a0, LONG funcOffset),REG(d0,APTR newFunction),REG(a6,struct Library * SysBase));

STATIC APTR __ASM SAVEDS
LocalSetFunction(
	REG(a1, struct Library * library),
	REG(a0, WORD funcOffset),
	REG(d0, APTR newFunction),
	REG(a6, struct Library * SysBase))
{
	LibraryVector * lv = (LibraryVector *)(((ULONG)library) + funcOffset);
	Wedge * w = (Wedge *)lv->Location;
	BOOL useEnable;
	APTR result;

	if(library == SysBase && (funcOffset == (LONG)&LVOForbid || funcOffset == (LONG)&LVOPermit))
	{
		useEnable = TRUE;
		Disable();
	}
	else
	{
		useEnable = FALSE;
		Forbid();
	}

	D(("library |%s| offset %ld",library->lib_Node.ln_Name,funcOffset));
	SHOWVALUE(w);
	SHOWVALUE(w->Magic);
	SHOWVALUE(w->PointBack);

	if(AktPrefs.mmp_FixPatches && lv->Command == JMP_ABS && w->Magic == WEDGE_MAGIC && w->PointBack == w)
	{
		SHOWMSG("one of our patches");

		result = w->OldLocation;

		w->OldLocation = (APTR)newFunction;
		(*w->Patch->OldRoutine) = (ULONG)newFunction;
	}
	else
	{
		SHOWMSG("none of our patches");

		result = CallSetFunction(library,funcOffset,newFunction,SysBase);
	}

	if(useEnable)
		Enable();
	else
		Permit();

	return(result);
}

/****************************************************************************/

VOID
RemovePatches ()
{
  if (Patches)
  {
    LONG i;

    ObtainSemaphore (Patches);

    Forbid ();

    for (i = 0; i < PATCHCOUNT ; i++)
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

    for (i = 0; i < PATCHCOUNT ; i++)
    {
      if((*(struct Library **)PatchTable[i].Base)->lib_Version >= PatchTable[i].Version)
      {
        Patches->Table[i].Command = JMP_ABS;
        Patches->Table[i].Location = PatchTable[i].NewRoutine;
        Patches->Table[i].Magic = WEDGE_MAGIC;
        Patches->Table[i].Patch = &PatchTable[i];
        Patches->Table[i].PointBack = &Patches->Table[i];

        if (Patches->Table[i].OldLocation == NULL)
	  Patches->Table[i].OldLocation = SetFunction (*(struct Library **)PatchTable[i].Base, PatchTable[i].Offset, (ULONG (*)()) & Patches->Table[i]);

        (*PatchTable[i].OldRoutine) = (ULONG) Patches->Table[i].OldLocation;
        PatchTable[i].WedgeCode = &Patches->Table[i];
      }
    }

    CacheClearU ();

    Permit ();

    ReleaseSemaphore (Patches);

    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}

/*****************************************************************************************/

BOOL
AllPatchesOnTop(VOID)
{
	struct Library ** libPtr;
	LibraryVector * lv;
	BOOL result = TRUE;
	LONG i;

	Forbid();

	for(i = 0 ; i < PATCHCOUNT ; i++)
	{
		libPtr = PatchTable[i].Base;

		lv = (LibraryVector *)((ULONG)(*libPtr) + PatchTable[i].Offset);
		if(lv->Location != &Patches->Table[i])
		{
			result = FALSE;
			break;
		}
	}

	Permit();

	return(result);
}
