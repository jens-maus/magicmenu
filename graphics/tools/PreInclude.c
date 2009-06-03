#include <libraries/iffparse.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <graphics/gfx.h>
#include <exec/memory.h>

#include <clib/iffparse_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

struct BitMapHeader
{
	UWORD	w, h;
	WORD	x, y;
	UBYTE	nPlanes;
	UBYTE	masking;
	UBYTE	compression;
	UBYTE	pad1;
	UWORD	transparentColor;
	UBYTE	xAspect,yAspect;
	WORD	pageWidth,pageHeight;
};
