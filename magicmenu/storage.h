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
	LONG	si_Size;
	WORD	si_Type;
	WORD	si_Found;
	STRPTR	si_Name;
};

#define DECLARE_ITEM(s,e,t,n)	offsetof(struct s,e),sizeof(((struct s *)0)->e),t,0,n
#define ITEM_TABLE_SIZE(t)	(sizeof(t)/sizeof(t[0]))

/******************************************************************************/

LONG RestoreData(STRPTR Name,STRPTR Type,LONG Version,struct StorageItem *Items,LONG NumItems,APTR DataPtr,LONG * NumItemsPtr);
LONG StoreData(STRPTR Name,STRPTR Type,LONG Version,struct StorageItem *Items,LONG NumItems,APTR DataPtr);
