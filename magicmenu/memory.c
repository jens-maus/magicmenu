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

#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/execbase.h>

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>

#include <pragmas/exec_pragmas.h>

#include <string.h>

#include <SDI_compiler.h>

extern struct ExecBase *SysBase;


	/* pools.lib */

APTR __ASM AsmCreatePool (REG (d0, ULONG MemFlags), REG (d1, ULONG PuddleSize), REG (d2, ULONG ThreshSize), REG (a6,
			  struct ExecBase *ExecBase));
VOID __ASM AsmDeletePool (REG (a0, APTR PoolHeader), REG (a6,
		       struct ExecBase *ExecBase));
APTR __ASM AsmAllocPooled (REG (a0, APTR PoolHeader), REG (d0, ULONG Size), REG (a6,
			struct ExecBase *ExecBase));
VOID __ASM AsmFreePooled (REG (a0, APTR PoolHeader), REG (a1, APTR Memory), REG (d0, ULONG MemSize), REG (a6,
		       struct ExecBase *ExecBase));

STATIC struct SignalSemaphore MemorySemaphore;
STATIC APTR MemoryPool;

APTR
AllocVecPooled (ULONG Size, ULONG Flags)
{
  if (!Size)
    return (NULL);
  else
  {
    ULONG *Chunk;

    Size = (Size + 7) & ~7;

    ObtainSemaphore (&MemorySemaphore);
    if (SysBase->LibNode.lib_Version >= 39L)	Chunk = (ULONG *) AllocPooled (MemoryPool, sizeof (ULONG) + Size);
    else Chunk = (ULONG *) AsmAllocPooled (MemoryPool, sizeof (ULONG) + Size, SysBase);
    ReleaseSemaphore (&MemorySemaphore);

    if (Chunk)
    {
      *Chunk++ = sizeof (ULONG) + Size;

      if (Flags & MEMF_CLEAR)
      {
	ULONG *Mem, Longs;

	Mem = Chunk;
	Longs = Size / sizeof (ULONG);

	do
	  *Mem++ = 0;
	while (--Longs);
      }
    }

    return (Chunk);
  }
}

VOID
FreeVecPooled (APTR Memory)
{
  if (Memory && MemoryPool)
  {
    ULONG *Chunk = Memory;

    ObtainSemaphore (&MemorySemaphore);
    if (SysBase->LibNode.lib_Version >= 39L) FreePooled (MemoryPool, &Chunk[-1], Chunk[-1]);
    else AsmFreePooled (MemoryPool, &Chunk[-1], Chunk[-1], SysBase);
    ReleaseSemaphore (&MemorySemaphore);
  }
}

VOID
MemoryExit (VOID)
{
  if (MemoryPool)
  {
    if (SysBase->LibNode.lib_Version >= 39L) DeletePool (MemoryPool);
    else AsmDeletePool (MemoryPool, SysBase);

    MemoryPool = NULL;
  }
}

BOOL
MemoryInit (VOID)
{
  InitSemaphore (&MemorySemaphore);

  if (SysBase->LibNode.lib_Version >= 39L) MemoryPool = CreatePool (MEMF_ANY | MEMF_PUBLIC, 8192, 8192);
  else MemoryPool = AsmCreatePool (MEMF_ANY | MEMF_PUBLIC, 8192, 8192, SysBase);

  return (MemoryPool!=NULL);
}
