/*
**	$Id$
**
**	:ts=8
*/

#include <exec/types.h>
#include <stddef.h>

/******************************************************************************/

enum
{
	SIT_BYTE,
	SIT_UBYTE,
	SIT_WORD,
	SIT_BOOL,
	SIT_BOOLEAN,
	SIT_UWORD,
	SIT_LONG,
	SIT_ULONG,
	SIT_STRPTR,
	SIT_TEXT
};

struct StorageItem
{
	ULONG	si_Offset;
	WORD	si_Size;
	WORD	si_Type;
	STRPTR	si_Name;
};

#define DECLARE_ITEM(s,e,n)	offsetof(struct s,e),sizeof(((struct s *)0)->e),n)
#define ITEM_TABLE_SIZE(t)	(sizeof(t)/sizeof(t[0]))
