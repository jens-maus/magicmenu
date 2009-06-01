/***************************************************************************

 MagicMenu - The Intuition Menus Enhancer
 Copyright (C) 1993-1997 by Martin Korndörfer
 Copyright (C) 1997-2001 by Olaf 'Olsen' Barthel
 Copyright (C) 2001-2009 by Stephan Rupprecht, Jens Langner
 Copyright (C) 2009      by MagicMenu Open Source Team

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 MagicMenu project:  http://sourceforge.net/projects/magicmenu/

 $Id$

***************************************************************************/

/******************************************************************************/

#ifndef _SYSTEM_HEADERS_H
#include "system_headers.h"
#endif	/* _SYSTEM_HEADERS_H */

#ifndef _STORAGE_H
#include "storage.h"
#endif	/* _STORAGE_H */

/******************************************************************************/

extern struct Library	*SysBase;
extern struct Library	*DOSBase;
extern struct Library	*UtilityBase;

/******************************************************************************/

STATIC LONG
LPrintf(LONG Error,BPTR FileHandle,STRPTR FormatString,...)
{
	if(Error == 0)
	{
		va_list VarArgs;

		va_start(VarArgs,FormatString);

		if(VFPrintf(FileHandle,FormatString,VarArgs) < 0)
			Error = IoErr();

		va_end(VarArgs);
	}

	return(Error);
}

/******************************************************************************/

STATIC LONG
GetLine(BPTR FileHandle,STRPTR Buffer)
{
	while(TRUE)
	{
		if(FGets(FileHandle,Buffer,1023) == NULL)
			return(IoErr());
		else
		{
			LONG Len;

			Len = strlen(Buffer);

			while(Len > 0 && Buffer[Len - 1] == '\n')
				Len--;

			if(Len > 0)
			{
				Buffer[Len] = 0;

				return(0);
			}
		}
	}
}

STATIC LONG
PrepareItem(APTR That,WORD Type,WORD Size,STRPTR String)
{
	if(Type == SIT_STRPTR || Type == SIT_TEXT)
	{
		LONG Len;

		String++;

		Len = strlen(String);

		if(Len > 0 && String[Len - 1] == '\"')
		{
			STRPTR There;
			UBYTE c;

			String[Len - 1] = 0;

			if(Type == SIT_STRPTR)
			{
				There = *(STRPTR *)That;
				Size = 256;
			}
			else
				There = (STRPTR)That;

			Size--;

			while(Size > 0 && (c = *String++))
			{
				if(c == '\\')
				{
					if(c = *String++)
					{
						if(c == '\\')
							*There++ = c;
						else
						{
							if(c >= '0' && c <= '3' && String[0] >= '0' && String[0] <= '7' && String[1] >= '0' && String[1] <= '7')
							{
								*There++ = ((c & 3) << 6) + ((String[0] & 7) << 3) + (String[1] & 7);

								String += 2;
							}
							else
								return(ERROR_OBJECT_WRONG_TYPE);
						}
					}
					else
						return(ERROR_OBJECT_WRONG_TYPE);
				}
				else
					*There++ = c;

				Size--;
			}

			*There = 0;
		}
		else
			return(ERROR_OBJECT_WRONG_TYPE);
	}
	else if (Type == SIT_BOOL || Type == SIT_BOOLEAN)
	{
		BOOL Which;

		if(!Stricmp(String,"Yes"))
			Which = TRUE;
		else if (!Stricmp(String,"No"))
			Which = FALSE;
		else
			return(ERROR_OBJECT_WRONG_TYPE);

		if(Type == SIT_BOOL)
			*(BOOL *)That = Which;
		else
			*(UBYTE *)That = Which;
	}
	else
	{
		ULONG Unsigned;
		LONG Signed;

		if(String[0] == '0' && String[1] == 'x')
		{
			UBYTE c;

			String += 2;

			Unsigned = 0;

			while(c = *String++)
			{
				if(c <= '9')
					c = (c & 15);
				else
					c = (c & 15) + 9;

				Unsigned = (Unsigned * 16) + c;
			}
		}
		else
			StrToLong(String,&Signed);

		switch(Type)
		{
			case SIT_BYTE:

				*(BYTE *)That = Signed;
				break;

			case SIT_UBYTE:

				*(UBYTE *)That = Unsigned;
				break;

			case SIT_WORD:

				*(WORD *)That = Signed;
				break;

			case SIT_UWORD:

				*(UWORD *)That = Unsigned;
				break;

			case SIT_LONG:

				*(LONG *)That = Signed;
				break;

			case SIT_ULONG:

				*(ULONG *)That = Unsigned;
				break;
		}
	}

	return(0);
}

/******************************************************************************/

LONG
RestoreData(STRPTR Name,STRPTR Type,LONG Version,struct StorageItem *Items,LONG NumItems,APTR DataPtr,LONG * NumItemsPtr)
{
	STRPTR LocalBuffer;
	LONG Error;

	if(NumItemsPtr != NULL)
		(*NumItemsPtr) = 0;

	if(!(LocalBuffer = (STRPTR)AllocVec(1024,MEMF_ANY)))
		Error = IoErr();
	else
	{
		BPTR FileHandle;

		if(!(FileHandle = Open(Name,MODE_OLDFILE)))
			Error = IoErr();
		else
		{
			LONG ThisVersion;

			if(!(Error = GetLine(FileHandle,LocalBuffer)))
			{
				LONG Len;

				Len = strlen(LocalBuffer);

				Error = ERROR_OBJECT_WRONG_TYPE;

				if(!Strnicmp(LocalBuffer,Type,strlen(Type)) && LocalBuffer[Len - 1] == ':')
				{
					LocalBuffer[Len - 1] = 0;

					if(StrToLong(FilePart(LocalBuffer),&ThisVersion) > 0)
					{
						if(ThisVersion >= Version)
							Error = 0;
					}
				}
			}

			if(!Error)
			{
				STRPTR Buffer;
				LONG i;

				for(i = 0 ; i < NumItems ; i++)
					Items[i].si_Found = 0;

				while(!(Error = GetLine(FileHandle,LocalBuffer)))
				{
					Buffer = LocalBuffer;

					while(*Buffer == ' ' || *Buffer == '\t')
						Buffer++;

					if(LocalBuffer[0] == '#')
					{
						for(i = 0 ; i < NumItems ; i++)
						{
							if(Items[i].si_Found == 0)
							{
								Error = ERROR_REQUIRED_ARG_MISSING;
								break;
							}
						}

						break;
					}
					else
					{
						STRPTR Argument;

						Argument = NULL;

						for(i = 0 ; i < strlen(Buffer) ; i++)
						{
							if(Buffer[i] == '=')
							{
								Argument = &Buffer[i+1];
								Buffer[i] = 0;
								break;
							}
						}

						if(Argument == NULL)
						{
							Error = ERROR_KEY_NEEDS_ARG;
							break;
						}
						else
						{
							LONG Index;

							Index = -1;

							for(i = 0 ; i < NumItems ; i++)
							{
								if(!Stricmp(Buffer,Items[i].si_Name))
								{
									Index = i;
									break;
								}
							}

							if(Index != -1)
							{
								if(Items[Index].si_Found++ > 0)
								{
									Error = ERROR_TOO_MANY_ARGS;
									break;
								}

								if(Error = PrepareItem((APTR)(((ULONG)DataPtr) + Items[Index].si_Offset),Items[Index].si_Type,Items[Index].si_Size,Argument))
									break;

								if(NumItemsPtr != NULL)
								{
									if(Index == 0 || (Index > 0 && Items[Index-1].si_Found))
										(*NumItemsPtr) = 1 + (*NumItemsPtr);
								}
							}
						}
					}
				}
			}

			Close(FileHandle);
		}

		FreeVec(LocalBuffer);
	}

	return(Error);
}

/******************************************************************************/

LONG
StoreData(STRPTR Name,STRPTR Type,LONG Version,struct StorageItem *Items,LONG NumItems,APTR DataPtr)
{
	STRPTR LocalBuffer;
	LONG Error;

	if(!(LocalBuffer = (STRPTR)AllocVec(1024,MEMF_ANY)))
		Error = IoErr();
	else
	{
		BPTR FileHandle;

		if(!(FileHandle = Open(Name,MODE_NEWFILE)))
			Error = IoErr();
		else
		{
			APTR That;
			LONG i,j,k,Len;
			STRPTR String;

			SetVBuf(FileHandle,NULL,BUF_FULL,1024);

			Error = LPrintf(0,FileHandle,"%s/%ld:\n",Type,Version);

			for(i = 0 ; i < NumItems ; i++)
			{
				That = (APTR)(((ULONG)DataPtr) + Items[i].si_Offset);

				Error = LPrintf(Error,FileHandle,"\t%s=",Items[i].si_Name);

				switch(Items[i].si_Type)
				{
					case SIT_BYTE:

						Error = LPrintf(Error,FileHandle,"%ld\n",*(BYTE *)That);
						break;

					case SIT_UBYTE:

						Error = LPrintf(Error,FileHandle,"0x%02lx\n",*(UBYTE *)That);
						break;

					case SIT_WORD:

						Error = LPrintf(Error,FileHandle,"%ld\n",*(WORD *)That);
						break;

					case SIT_BOOL:

						Error = LPrintf(Error,FileHandle,"%s\n",*(BOOL *)That ? "Yes" : "No");
						break;

					case SIT_BOOLEAN:

						Error = LPrintf(Error,FileHandle,"%s\n",*(UBYTE *)That ? "Yes" : "No");
						break;

					case SIT_UWORD:

						Error = LPrintf(Error,FileHandle,"0x%04lx\n",*(UWORD *)That);
						break;

					case SIT_LONG:

						Error = LPrintf(Error,FileHandle,"%ld\n",*(LONG *)That);
						break;

					case SIT_ULONG:

						Error = LPrintf(Error,FileHandle,"0x%08lx\n",*(ULONG *)That);
						break;

					case SIT_TEXT:
					case SIT_STRPTR:

						if(Items[i].si_Type == SIT_TEXT)
							String = (STRPTR)That;
						else
							String = *(STRPTR *)That;

						Len = strlen(String);

						for(j = k = 0 ; k < 1023 && j < Len ; j++)
						{
							if(String[j] < ' ' || String[j] == '\"' || String[j] == '*' || (String[j] >= 127 && String[j] <= 160))
							{
								LocalBuffer[k++] = '\\';
								LocalBuffer[k++] = '0' + (String[j] >> 6);
								LocalBuffer[k++] = '0' + ((String[j] >> 3) & 7);
								LocalBuffer[k++] = '0' + (String[j] & 7);
							}
							else if (String[j] == '\\')
							{
								LocalBuffer[k++] = '\\';
								LocalBuffer[k++] = '\\';
							}
							else
							{
								LocalBuffer[k++] = String[j];
							}
						}

						LocalBuffer[k] = 0;

						Error = LPrintf(Error,FileHandle,"\"%s\"\n",LocalBuffer);

						break;

					default:

						Error = ERROR_OBJECT_WRONG_TYPE;
						break;
				}
			}

			Error = LPrintf(Error,FileHandle,"#\n");

			if(!Close(FileHandle))
				Error = IoErr();

			if(Error != 0)
				DeleteFile(Name);
		}

		FreeVec(LocalBuffer);
	}

	return(Error);
}
