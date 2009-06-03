** DoRevision Header ** Header built automatically - do not edit! **********
*
*	(C) Copyright 1991 by Olaf Barthel
*
*	Name .....:	Format.asm
*	Created ..:	Wednesday 24-Jul-91 14:21
*	Revision .:	1
*
*	Date		Author		Comment
*	=========	========	====================
*	24-Jul-91	Olsen		Created this file!
*
***************************************************************************/

CALL	macro
	xref	_LVO\1
	jsr	_LVO\1(a6)
	endm

	csect	text,0,0,1,2

	xdef	_Printf

_Printf:
	movem.l	d2/a6,-(sp)

	move.l	12(sp),d1
	lea	16(sp),a0
	move.l	a0,d2

	move.l	_DOSBase(a4),a6

	CALL	VPrintf

	movem.l	(sp)+,d2/a6
	rts

	csect	__MERGED

	xref	_DOSBase

	end
