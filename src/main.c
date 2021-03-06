/***************************************************************************

 MagicMenu - The Intuition Menus Enhancer
 Copyright (C) 1993-1997 by Martin Kornd�rfer
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

#ifndef _GLOBAL_H
#include "Global.h"
#endif /* _GLOBAL_H */

#if defined(_M68020) || defined(_M68030) || defined(_M68040)
#ifdef __MIXEDBINARY__
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") 020+/ppc fatbinary\n";
#else
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") 020+ version\r\n";
#endif
#else
STRPTR VersTag = "\0$VER: " VERS " (" DATE ") Generic 68k version\r\n";
#endif // _M68030

#if defined(_M68020) || defined(_M68030) || defined(_M68040)
#define  PROCESSOR   "68020"
#else
#define  PROCESSOR   "68000"
#endif // _M68030

/******************************************************************************/
#ifdef __STORMGCC__
long __stacksize = 8192;

#undef BltBitMap
#undef WritePixelArray
#else
long __stack = 8192;
#endif

/******************************************************************************/

#ifndef _STORAGE_H
#include "storage.h"
#endif	/* _STORAGE_H */

/******************************************************************************/

 /* This is how a linked list of directory search paths looks like. */

struct Path
{
	BPTR path_Next;	/* Pointer to next entry */
	BPTR path_Lock;	/* The drawer in question; may be NULL */
};

BPTR
ClonePath (BPTR StartPath)
{
	struct Path *First, *Last, *List, *New;

	for (List = BADDR (StartPath), First = Last = NULL; List; List = BADDR (List->path_Next))
	{
		if (List->path_Lock)
		{
			if (New = AllocVec (sizeof (struct Path), MEMF_ANY))
			{
				if (New->path_Lock = DupLock (List->path_Lock))
				{
					New->path_Next = NULL;

					if (Last)
						Last->path_Next = MKBADDR (New);

					if (!First)
						First = New;

					Last = New;
				}
				else
				{
					FreeVec (New);
					break;
				}
			}
			else
				break;
		}
	}

	return (MKBADDR (First));
}

VOID
AttachCLI (struct WBStartup * Startup)
{
	struct CommandLineInterface *DestCLI;

	/* Note: FreeDosObject can't free it, but the DOS */
	/*       process termination code can. */

	if (DestCLI = AllocDosObject (DOS_CLI, NULL))
	{
		struct MsgPort *ReplyPort;
		struct Process *Dest;

		DestCLI->cli_DefaultStack = 4096 / sizeof (ULONG);

		Dest = (struct Process *) FindTask (NULL);

		Dest->pr_CLI = MKBADDR (DestCLI);
		Dest->pr_Flags |= PRF_FREECLI;  /* Mark for cleanup */

		Forbid ();

		ReplyPort = Startup->sm_Message.mn_ReplyPort;

		/* Does the reply port data point somewhere sensible? */

		if (ReplyPort && (ReplyPort->mp_Flags & PF_ACTION) == PA_SIGNAL && TypeOfMem (ReplyPort->mp_SigTask))
		{
			struct Process *Father;

			/* Get the address of the process that sent the startup message */

			Father = (struct Process *) ReplyPort->mp_SigTask;

			/* Just to be on the safe side... */

			if (Father->pr_Task.tc_Node.ln_Type == NT_PROCESS)
			{
				struct CommandLineInterface *SourceCLI;

				/* Is there a CLI attached? */

				if (SourceCLI = BADDR (Father->pr_CLI))
				{
					STRPTR Prompt;

					/* Clone the other CLI data. */

					if (Prompt = (STRPTR) BADDR (SourceCLI->cli_Prompt))
						SetPrompt (&Prompt[1]);

					if (SourceCLI->cli_DefaultStack > DestCLI->cli_DefaultStack)
						DestCLI->cli_DefaultStack = SourceCLI->cli_DefaultStack;

					if (SourceCLI->cli_CommandDir)
						DestCLI->cli_CommandDir = ClonePath (SourceCLI->cli_CommandDir);
				}
			}
		}

		Permit ();
	}
}

/******************************************************************************/

#define CATCOMP_ARRAY
#include "locale/magicmenu.h"

STRPTR
GetString (ULONG ID)
{
	STRPTR Builtin;

	if (ID < NUM_ELEMENTS (CatCompArray) && CatCompArray[ID].cca_ID == ID)
		Builtin = CatCompArray[ID].cca_Str;
	else
	{
		LONG Left, Mid, Right;

		Left = 0;
		Right = NUM_ELEMENTS (CatCompArray) - 1;

		do
		{
			Mid = (Left + Right) / 2;

			if (ID < CatCompArray[Mid].cca_ID)
				Right = Mid - 1;
			else
				Left = Mid + 1;
		}
		while (ID != CatCompArray[Mid].cca_ID && Left <= Right);

		if (ID == CatCompArray[Mid].cca_ID)
			Builtin = CatCompArray[Mid].cca_Str;
		else
			Builtin = "";
	}

	if (Catalog)
	{
		STRPTR String = GetCatalogStr (Catalog, ID, Builtin);

		if (String[0])
			return (String);
	}

	return (Builtin);
}

/******************************************************************************/

void LoadBackFill( STRPTR backfill )
{
	if (BackFillBM != NULL)
	{
		WaitBlit();
		FreeBitMap(BackFillBM);
		BackFillBM = NULL;
	}

	if (DataTypesBase != NULL && backfill[0] != '\0' && CyberGfxBase != NULL)
	{
		Object	*BackFillObj;

		if (BackFillObj = NewDTObject(backfill,
			DTA_SourceType, DTST_FILE,
			DTA_GroupID, GID_PICTURE,
			PDTA_Remap, FALSE,
			PDTA_DestMode, PMODE_V43,
			TAG_DONE))
		{
			struct BitMap			*bm;
			struct BitMapHeader	*bmh;

			GetDTAttrs (BackFillObj,
				PDTA_BitMap, (ULONG) &bm,
				PDTA_BitMapHeader, (ULONG) &bmh,
				TAG_DONE);

			if( BackFillBM = AllocBitMap( bmh->bmh_Width, bmh->bmh_Height, 24L, SHIFT_PIXFMT(PIXFMT_RGB24)|BMF_MINPLANES|BMF_SPECIALFMT, NULL ) )
			{
				BFWidth = bmh->bmh_Width;
				BFHeight = bmh->bmh_Height;

				if(bm != NULL)
				{
					LONG	depth;

					if( ( depth = GetBitMapAttr(bm, BMA_DEPTH) ) > 8L )
					{
						BltBitMap( bm, 0,0, BackFillBM, 0,0, bmh->bmh_Width, bmh->bmh_Height, 0xc0, 0xff, NULL );
					}
					/* Wandle Daten nach 24bit, normalerweise macht das eigentlich der pictdt
					 * auch f�r uns. Nur leider nicht die cgx version :(
					 */
					else
					{
						UBYTE	*chunky, *cmap;

						GetAttr( PDTA_ColorRegisters, BackFillObj, (ULONG *) &cmap );

						if( chunky = AllocVecPooled( bmh->bmh_Width*4, MEMF_ANY ) )
						{
							struct BitMap	*tbm;

							if( tbm = AllocBitMap( bmh->bmh_Width, 1L, depth, BMF_CLEAR, bm ) )
							{
								struct RastPort	 trp, rp;
								UBYTE					*rgb = &chunky[bmh->bmh_Width];
								WORD					 x, y;

								InitRastPort( &trp );
								trp.BitMap = tbm;
								rp = trp;

								for( y = 0; y < bmh->bmh_Height; y++ )
								{
									UBYTE	*p = rgb;

									rp.BitMap = bm;
									ReadPixelLine8( &rp, 0,y, bmh->bmh_Width, chunky, &trp );

									for( x = 0; x < bmh->bmh_Width; x++ )
									{
										UWORD	num = chunky[ x ] * 3;

										*p++ = cmap[ num ];
										*p++ = cmap[ num + 1 ];
										*p++ = cmap[ num + 2 ];
									}

									rp.BitMap = BackFillBM;
									WritePixelArray( rgb, 0,0,0, &rp, 0,y, bmh->bmh_Width, 1L, RECTFMT_RGB );
								}

								WaitBlit();
								FreeBitMap( tbm );
							}

							FreeVecPooled( chunky );
						}
					}
				}
				else
				{
					UBYTE	*rgb;
					/* Hiermit ersparen wir uns ein Layout, wenn _nicht_ der
					 * cgx pictdt installiert ist (dieser liefert sofort
					 * eine 24bit BitMap bei hi-/truecolor Daten).
					 */
					if( rgb = AllocVecPooled( bmh->bmh_Width * 3, MEMF_ANY ) )
					{
						struct RastPort	rp;
						UWORD					y;

						InitRastPort( &rp );
						rp.BitMap = BackFillBM;

						for( y = 0; y < bmh->bmh_Height; y++ )
						{
							DoMethod( BackFillObj, PDTM_READPIXELARRAY,
								(ULONG) rgb, PBPAFMT_RGB, 0, 0,y, bmh->bmh_Width, 1L );

							WritePixelArray( rgb, 0,0,0, &rp, 0,y, bmh->bmh_Width, 1L, RECTFMT_RGB );
						}

						FreeVecPooled( rgb );
					}
				}
			}

			DisposeDTObject( BackFillObj );
		}
		else Complain( GetString( MSG_CANNOT_LOAD_BACKFILL ) );
	}
}

/******************************************************************************/

VOID
Activate (VOID)
{
	if (!CxChanged)
	{
		if(NOT AktPrefs.mmp_VerifyPatches || AllPatchesOnTop())
		{
			ActivateCxObj (Broker, TRUE);
			MagicActive = TRUE;
		}
	}
}

VOID
Deactivate (VOID)
{
	if (!CxChanged)
	{
		ActivateCxObj (Broker, FALSE);
		MagicActive = FALSE;
	}
}

/******************************************************************************/

VOID
FreeMenuRemember (struct MenuRemember *Remember)
{
	struct MenuRmb *MenRmb, *NextMenu;
	struct ItemRmb *ItemRmb, *NextItem;

	ENTER();

	MenRmb = Remember->FirstMenu;

	while (MenRmb)
	{
		ItemRmb = MenRmb->FirstItem;

		while (ItemRmb)
		{
			NextItem = ItemRmb->NextItem;
			FreeVecPooled (ItemRmb);
			ItemRmb = NextItem;
		}

		NextMenu = MenRmb->NextMenu;
		FreeVecPooled (MenRmb);
		MenRmb = NextMenu;
	}

	LEAVE();

	FreeVecPooled (Remember);
}

struct MenuRemember *
MakeMenuRemember (struct Window *Win)
{
	struct MenuItem *ZwItem;
	LONG NW, NH, N1, N2;
	struct IntuiText *IntTxt;
	BOOL ScrHiRes, HasCheck;
	struct MenuRemember *NewRemember;
	struct MenuRmb *NewMenRmb;
	struct ItemRmb *NewItemRmb;
	struct Menu *LookMenu;
	struct MenuItem *LookItem;
	struct TextAttr MenTextAttr;
	struct Screen *MenScr;
	struct Image *IntImg;
	long Length;

	if (Win->MenuStrip == NULL)
	return (NULL);

	if (!(NewRemember = AllocVecPooled (sizeof (struct MenuRemember), MEMF_CLEAR)))
	return (NULL);

	MenScr = Win->WScreen;

	AskFont (MenScr->BarLayer->rp, &MenTextAttr);

	ScrHiRes = (MenScr->Flags & SCREENHIRES) != NULL;

	NewRemember->MenWindow = Win;
	NewRemember->MenuStrip = Win->MenuStrip;

	NewRemember->AktMenuNum = MENUNULL;
	NewRemember->AktMenu = NULL;

	LookMenu = Win->MenuStrip;
	while (LookMenu)
	{
	if (!(NewMenRmb = AllocVecPooled (sizeof (struct MenuRmb), MEMF_CLEAR)))
	{
	FreeMenuRemember (NewRemember);
	return (NULL);
	}

	NewMenRmb->Menu = LookMenu;
	NewMenRmb->AktItemNum = 0;
	NewMenRmb->AktItem = 0;

	NewMenRmb->Height = 0;
	NewMenRmb->Width = 0;
	NewMenRmb->ZwTop = 0x7fff;
	NewMenRmb->ZwLeft = 0x7fff;
	NewMenRmb->CmdOffs = 0;

	HasCheck = FALSE;

	ZwItem = LookMenu->FirstItem;

	while (ZwItem)
	{
	if (!HasCheck && (ZwItem->Flags & CHECKIT) && (!ZwItem->SubItem) && (ZwItem->Flags & HIGHNONE) != HIGHNONE)
	HasCheck = TRUE;

	CommandText.ITextFont = &MenTextAttr;

	NW = ZwItem->LeftEdge + ZwItem->Width;
	NH = ZwItem->TopEdge + ZwItem->Height;

	if (ZwItem->Flags & ITEMTEXT)
	{
	IntTxt = (struct IntuiText *) ZwItem->ItemFill;

	while (IntTxt)
	{
	if (IntTxt->ITextFont == NULL)
	{
	IntTxt->ITextFont = &MenTextAttr;
	N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
	N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
	IntTxt->ITextFont = NULL;
	}
	else
	{
	N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
	N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
	CommandText.ITextFont = IntTxt->ITextFont;
	}

	if (N1 > NW)
	NW = N1;

	if (N2 > NH)
	NH = N2;

	IntTxt = IntTxt->NextText;
	}
	}
	else
	{
	IntImg = (struct Image *) ZwItem->ItemFill;
	while (IntImg)
	{
	N1 = IntImg->LeftEdge + IntImg->Width + ZwItem->LeftEdge;
	if (N1 > NW)
	NW = N1;

	N1 = IntImg->TopEdge + IntImg->Height + ZwItem->TopEdge;
	if (N1 > NH)
	NH = N1;

	IntImg = IntImg->NextImage;
	}
	}

	if (NW > NewMenRmb->Width)
	NewMenRmb->Width = NW;
	if (NH > NewMenRmb->Height)
	NewMenRmb->Height = NH;

	if (ZwItem->LeftEdge < NewMenRmb->ZwLeft)
	NewMenRmb->ZwLeft = ZwItem->LeftEdge;
	if (ZwItem->TopEdge < NewMenRmb->ZwTop)
	NewMenRmb->ZwTop = ZwItem->TopEdge;

	if (ZwItem->Flags & COMMSEQ)
	{
	CommText[0] = ZwItem->Command;
	CommText[1] = 0;

	Length = IntuiTextLength (&CommandText);
	if (Length > NewMenRmb->CmdOffs)
	NewMenRmb->CmdOffs = Length;
	}

	ZwItem = ZwItem->NextItem;
	}

	NewMenRmb->Height -= NewMenRmb->ZwTop;
	NewMenRmb->Width -= NewMenRmb->ZwLeft;

	NewMenRmb->NextMenu = NewRemember->FirstMenu;
	NewRemember->FirstMenu = NewMenRmb;

	if (NewMenRmb->Height != 0 && NewMenRmb->Width != 0)
	{
	if (ScrHiRes)
	{
	NewMenRmb->Height += 6;
	NewMenRmb->Width += 10;
	NewMenRmb->LeftBorder = 5;
	NewMenRmb->TopBorder = 3;
	}
	else
	{
	NewMenRmb->Height += 6;
	NewMenRmb->Width += 6;
	NewMenRmb->TopBorder = 3;
	NewMenRmb->LeftBorder = 3;
	}

	NewMenRmb->TopOffs = NewMenRmb->TopBorder - NewMenRmb->ZwTop;
	NewMenRmb->LeftOffs = NewMenRmb->LeftBorder - NewMenRmb->ZwLeft;

	LookItem = LookMenu->FirstItem;

	while (LookItem)
	{
	if (!(NewItemRmb = AllocVecPooled (sizeof (struct ItemRmb), MEMF_CLEAR)))
	{
	FreeMenuRemember (NewRemember);
	return (NULL);
	}

	NewItemRmb->Item = LookItem;
	NewItemRmb->AktSubItemNum = 0;
	NewItemRmb->AktSubItem = NULL;

	NewItemRmb->Height = 0;
	NewItemRmb->Width = 0;
	NewItemRmb->ZwTop = 0x7fff;
	NewItemRmb->ZwLeft = 0x7fff;
	NewItemRmb->CmdOffs = 0;

	ZwItem = LookItem->SubItem;

	HasCheck = FALSE;

	while (ZwItem)
	{
	if (!HasCheck && (ZwItem->Flags & CHECKIT) && (!ZwItem->SubItem) && (ZwItem->Flags & HIGHNONE) != HIGHNONE)
	HasCheck = TRUE;

	CommandText.ITextFont = &MenTextAttr;

	NW = ZwItem->LeftEdge + ZwItem->Width;
	NH = ZwItem->TopEdge + ZwItem->Height;

	if (ZwItem->Flags & ITEMTEXT)
	{
	IntTxt = (struct IntuiText *) ZwItem->ItemFill;
	while (IntTxt)
	{
	if (IntTxt->ITextFont == NULL)
	{
	IntTxt->ITextFont = &MenTextAttr;
	N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
	N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
	IntTxt->ITextFont = NULL;
	}
	else
	{
	N1 = IntTxt->LeftEdge + IntuiTextLength (IntTxt) + ZwItem->LeftEdge;
	N2 = IntTxt->TopEdge + IntTxt->ITextFont->ta_YSize + ZwItem->TopEdge;
	CommandText.ITextFont = IntTxt->ITextFont;
	}

	if (N1 > NW)
	NW = N1;

	if (N2 > NH)
	NH = N2;

	IntTxt = IntTxt->NextText;
	}
	}
	else
	{
	IntImg = (struct Image *) ZwItem->ItemFill;
	while (IntImg)
	{
	N1 = IntImg->LeftEdge + IntImg->Width + ZwItem->LeftEdge;
	if (N1 > NW)
	NW = N1;

	N1 = IntImg->TopEdge + IntImg->Height + ZwItem->TopEdge;
	if (N1 > NH)
	NH = N1;

	IntImg = IntImg->NextImage;
	}
	}

	if (NW > NewItemRmb->Width)
	NewItemRmb->Width = NW;
	if (NH > NewItemRmb->Height)
	NewItemRmb->Height = NH;

	if (ZwItem->LeftEdge < NewItemRmb->ZwLeft)
	NewItemRmb->ZwLeft = ZwItem->LeftEdge;
	if (ZwItem->TopEdge < NewItemRmb->ZwTop)
	NewItemRmb->ZwTop = ZwItem->TopEdge;

	if (ZwItem->Flags & COMMSEQ)
	{
	CommText[0] = ZwItem->Command;
	CommText[1] = 0;

	Length = IntuiTextLength (&CommandText);
	if (Length > NewItemRmb->CmdOffs)
	NewItemRmb->CmdOffs = Length;
	}

	ZwItem = ZwItem->NextItem;
	}

	NewItemRmb->Height -= NewItemRmb->ZwTop;
	NewItemRmb->Width -= NewItemRmb->ZwLeft;

	if (NewItemRmb->Height != NULL && NewItemRmb->Width != NULL)
	{
	if (ScrHiRes)
	{
	NewItemRmb->Height += 6;
	NewItemRmb->Width += 10;
	NewItemRmb->LeftBorder = 5;
	NewItemRmb->TopBorder = 3;
	}
	else
	{
	NewItemRmb->Height += 6;
	NewItemRmb->Width += 6;
	NewItemRmb->TopBorder = 3;
	NewItemRmb->LeftBorder = 3;
	}

	NewItemRmb->TopOffs = NewItemRmb->TopBorder - NewItemRmb->ZwTop;
	NewItemRmb->LeftOffs = NewItemRmb->LeftBorder - NewItemRmb->ZwLeft;

	}

	NewItemRmb->NextItem = NewMenRmb->FirstItem;
	NewMenRmb->FirstItem = NewItemRmb;

	LookItem = LookItem->NextItem;
	}

	}

	LookMenu = LookMenu->NextMenu;
	}

	return (NewRemember);

}

VOID
FreeGlobalRemember (VOID)
{
	struct MenuRemember *NextRemember;

	ENTER();

	ObtainSemaphore (RememberSemaphore);

	while (GlobalRemember)
	{
		NextRemember = GlobalRemember->NextRemember;
		FreeMenuRemember (GlobalRemember);
		GlobalRemember = NextRemember;
	}

	ReleaseSemaphore (RememberSemaphore);

	LEAVE();
}

BOOL
MakeGlobalRemember (VOID)
{
	struct Screen *Scr;
	struct Window *Win;
	struct MenuRemember *NewMenuRemember;
	long ILock;

	ENTER();

	ILock = LockIBase (NULL);

	ObtainSemaphore (RememberSemaphore);

	Scr = IntuitionBase->FirstScreen;

	while (Scr)
	{
		Win = Scr->FirstWindow;

		while (Win)
		{
			if (Win->MenuStrip)
			{
				if (!(NewMenuRemember = MakeMenuRemember (Win)))
				{
					FreeGlobalRemember ();
					ReleaseSemaphore (RememberSemaphore);
					UnlockIBase (ILock);
					LEAVE();
					return (FALSE);
				}
				NewMenuRemember->NextRemember = GlobalRemember;
				GlobalRemember = NewMenuRemember;
			}
			Win = Win->NextWindow;
		}
		Scr = Scr->NextScreen;
	}

	ReleaseSemaphore (RememberSemaphore);

	UnlockIBase (ILock);

	LEAVE();

	return (TRUE);
}

VOID
ClearRemember (struct Window * Win)
{
	struct MenuRemember *LookRemember;
	struct MenuRemember *LastRemember;
	struct MenuRemember *NextRemember;

	ENTER();

	ObtainSemaphore (RememberSemaphore);

	LookRemember = GlobalRemember;
	LastRemember = NULL;
	while (LookRemember)
	{
		NextRemember = LookRemember->NextRemember;
		if (LookRemember->MenWindow == Win)
		{
			if (LastRemember)
				LastRemember->NextRemember = LookRemember->NextRemember;
			else
				GlobalRemember = LookRemember->NextRemember;

			FreeMenuRemember (LookRemember);
		}
		else
			LastRemember = LookRemember;

		LookRemember = NextRemember;
	}

	ReleaseSemaphore (RememberSemaphore);

	LEAVE();
}

BOOL
UpdateRemember (struct Window *Window)
{
	struct MenuRemember *NewRemember;
	struct MenuRmb *LookMenRmb;
	struct ItemRmb *LookItemRmb;

	ENTER();

	ObtainSemaphore (RememberSemaphore);

	ClearRemember (Window);

	if (!(NewRemember = MakeMenuRemember (Window)))
	{
		ReleaseSemaphore (RememberSemaphore); /* STEPHAN */
		LEAVE();
		return (FALSE);
	}

	NewRemember->NextRemember = GlobalRemember;
	GlobalRemember = NewRemember;

	AktMenuRemember = NewRemember;

	if (AktMenu)
	{
		LookMenRmb = NewRemember->FirstMenu;
		AktMenRmb = NULL;
		while (LookMenRmb && (AktMenRmb == NULL))
		{
			if (LookMenRmb->Menu == AktMenu)
				AktMenRmb = LookMenRmb;
			LookMenRmb = LookMenRmb->NextMenu;
		}
		if (!AktMenRmb)
		{
			ReleaseSemaphore (RememberSemaphore);
			LEAVE();
			return (FALSE);
		}

		if (AktItem)
		{
			LookItemRmb = AktMenRmb->FirstItem;
			AktItemRmb = NULL;
			while (LookItemRmb && (AktItemRmb == NULL))
			{
				if (LookItemRmb->Item == AktItem)
					AktItemRmb = LookItemRmb;
				LookItemRmb = LookItemRmb->NextItem;
			}
			if (!AktItemRmb)
			{
				ReleaseSemaphore (RememberSemaphore);
				LEAVE();
				return (FALSE);
			}
		}
	}
	ReleaseSemaphore (RememberSemaphore);
	LEAVE();
	return (TRUE);
}

VOID
SetRemember (struct Window * Win)
{
	struct MenuRemember *NewRemember;

	ObtainSemaphore (RememberSemaphore);

	if (Win->MenuStrip)
	{
		ClearRemember (Win);

		NewRemember = MakeMenuRemember (Win);
		NewRemember->NextRemember = GlobalRemember;
		GlobalRemember = NewRemember;
	}

	ReleaseSemaphore (RememberSemaphore);
}

VOID
ResetMenu (struct Menu *Menu, BOOL MenNull)
{
	struct Menu *ZwMenu;
	struct MenuItem *ZwItem, *ZwSubItem;

	ENTER();

	ZwMenu = Menu;
	while (ZwMenu)
	{
		ZwItem = ZwMenu->FirstItem;
		while (ZwItem)
		{
			ZwSubItem = ZwItem->SubItem;
			while (ZwSubItem)
			{
				ZwSubItem->Flags &= ~(ISDRAWN | HIGHITEM | MENUTOGGLED);
				if (MenNull)
					ZwSubItem->NextSelect = MENUNULL;
				ZwSubItem = ZwSubItem->NextItem;
			}

			ZwItem->Flags &= ~(ISDRAWN | HIGHITEM | MENUTOGGLED);
			if (MenNull)
				ZwItem->NextSelect = MENUNULL;
			ZwItem = ZwItem->NextItem;
		}

		ZwMenu->Flags &= ~MIDRAWN;
		ZwMenu = ZwMenu->NextMenu;
	}

	LEAVE();
}

VOID
CleanUpMenu (VOID)
{
	ENTER();

	CleanUpMenuSubBox ();
	CleanUpMenuBox ();
	CleanUpMenuStrip ();

	if (LayersLocked)
	{
		UnlockLayers (&MenScr->LayerInfo);
		UnlockLayerInfo (&MenScr->LayerInfo);
		LayersLocked = FALSE;
	}

	MenWin->Flags &= ~WFLG_MENUSTATE;

	ResetBrokerSetup ();

	LEAVE();
}

BOOL
MenuSelected (BOOL LastSelect)
{
	struct MenuItem *SelItem, *ZwItem;
	BOOL SubItem;
	ULONG MExcl;
	LONG t, l, h, w;

	ENTER();

	SelItem = NULL;

	if (AktSubItem)
	{
		SelItem = AktSubItem;
		SubItem = TRUE;
	}
	else if (AktItem && (!AktItem->SubItem))
	{
		SelItem = AktItem;
		SubItem = FALSE;
	}
	else
	{
		SubItem = FALSE;
	}

	if (!SelItem)
		return (FALSE);

	if (!(SelItem->Flags & ITEMENABLED))
		return (FALSE);

	if (SubItem && SubBoxGhosted)
		return (FALSE);

	if (BoxGhosted)
		return (FALSE);

	if (SelItem->Flags & CHECKIT)
	{
		if (SelItem->MutualExclude)
		{
			if (!(SelItem->Flags & CHECKED))
			{
				MExcl = SelItem->MutualExclude;

				SelItem->Flags |= CHECKED;

				if (SubItem)
				{
					ZwItem = SubBoxItems;
					DrawNormSubItem (SelItem);
					DrawHiSubItem (SelItem);
				}
				else
				{
					ZwItem = BoxItems;
					DrawNormItem (SelItem);
					DrawHiItem (SelItem);
				}

				while (ZwItem)
				{
					if (ZwItem != SelItem && (MExcl & 0x1) && (ZwItem->Flags & CHECKIT) && (ZwItem->Flags & CHECKED))
					{
						ZwItem->Flags &= ~CHECKED;

						if (SubItem)
						{
							if (GetSubItemContCoor (ZwItem, &t, &l, &w, &h))
								DrawMenuItem (SubBoxDrawRPort, ZwItem, SubBoxDrawLeft + SubBoxLeftOffs, SubBoxDrawTop + SubBoxTopOffs, SubBoxCmdOffs, SubBoxGhosted, FALSE, SubBoxDrawLeft, w);
						}
						else
						{
							if (GetItemContCoor (ZwItem, &t, &l, &w, &h))
								DrawMenuItem (BoxDrawRPort, ZwItem, BoxDrawLeft + BoxLeftOffs, BoxDrawTop + BoxTopOffs, BoxCmdOffs, BoxGhosted, FALSE, BoxDrawLeft, w);
						}
					}

					MExcl = MExcl >> 1;
					ZwItem = ZwItem->NextItem;
				}
			}
			else if (SelItem->Flags & MENUTOGGLE)
			{
				SelItem->Flags &= ~CHECKED;

				if (SubItem)
				{
					DrawNormSubItem (SelItem);
					DrawHiSubItem (SelItem);
				}
				else
				{
					DrawNormItem (SelItem);
					DrawHiItem (SelItem);
				}
			}
		}
		else if ((!LastSelect) || (!(SelItem->Flags & MENUTOGGLED)))
		{
			SelItem->Flags ^= CHECKED;

			if (SubItem)
			{
				DrawNormSubItem (SelItem);
				DrawHiSubItem (SelItem);
			}
			else
			{
				DrawNormItem (SelItem);
				DrawHiItem (SelItem);
			}
		}
	}

	if (SelItem->Flags & MENUTOGGLED)
		return (TRUE);

	SelItem->Flags |= MENUTOGGLED;

	SelItem->NextSelect = MENUNULL;

	if (!FirstSelectedMenu)
	{
		FirstSelectedMenu = SelItem;
		FirstMenuNum = FULLMENUNUM (MenuNum, ItemNum, SubItemNum);
	}

	if (LastSelectedMenu)
		LastSelectedMenu->NextSelect = FULLMENUNUM (MenuNum, ItemNum, SubItemNum);

	LastSelectedMenu = SelItem;

	LEAVE();

	return (TRUE);
}

BOOL
CheckCxMsgAct (CxMsg * Msg, BOOL * Cancel)
{
	ULONG MsgID, MsgType;
	APTR MsgData;
	struct InputEvent Event;
	BOOL Shift, Ende, IsSingle, OutOfBounds;
	short ZwNum;
	char CharBuf[12], ASCIIChar;
	struct MenuItem *SavItem, *SavSubItem;
	struct Menu *SavMenu;
	UWORD SavMenuNum, SavItemNum, SavSubItemNum;
	ULONG CurrSecs, CurrMics;

	ENTER();

	*Cancel = FALSE;
	Ende = FALSE;

	if (CheckReply ((struct Message *) Msg))
		return (FALSE);

	MsgID = CxMsgID (Msg);
	MsgType = CxMsgType (Msg);
	MsgData = CxMsgData (Msg);
	Event = *((struct InputEvent *) MsgData);
	ReplyMsg ((struct Message *) Msg);

	if (MsgType == CXM_IEVENT)
	{
		switch (MsgID)
		{
		case EVT_KEYBOARD:
			Shift = (NULL != (Event.ie_Qualifier & (IEQUALIFIER_RSHIFT | IEQUALIFIER_LSHIFT)));
			if (Event.ie_Code == HELP && (MenWin->IDCMPFlags & IDCMP_MENUHELP) != NULL)
			{
				Ende = HelpPressed = TRUE;
				FirstMenuNum = FULLMENUNUM (MenuNum, ItemNum, SubItemNum);
				break;
			}

			if (Event.ie_Code == CTRL && ((StripPopUp && AktPrefs.mmp_PULook != LOOK_2D) || (!StripPopUp && AktPrefs.mmp_PDLook != LOOK_2D)))
			{
				SavSubItem = AktSubItem;
				SavSubItemNum = SubItemNum;
				SavItem = AktItem;
				SavItemNum = ItemNum;
				SavMenu = AktMenu;
				SavMenuNum = MenuNum;

				CleanUpMenuSubBox ();
				CleanUpMenuBox ();
				CleanUpMenuStrip ();
				Ende = !DrawMenuStrip (StripPopUp, (Look3D == LOOK_3D) ? LOOK_2D :
				 (StripPopUp) ? AktPrefs.mmp_PULook : AktPrefs.mmp_PDLook, FALSE);
				if (SavMenu)
				{
					DrawHiMenu (AktMenu = SavMenu);
					MenuNum = SavMenuNum;
					DrawMenuBox (AktMenu, FALSE);

					if (SavItem)
					{
						DrawHiItem (AktItem = SavItem);
						ItemNum = SavItemNum;
						SavItem->Flags |= HIGHITEM;
						DrawMenuSubBox (AktMenu, SavItem, FALSE);

						if (SavSubItem)
						{
							DrawHiSubItem (AktSubItem = SavSubItem);
							AktSubItem->Flags |= HIGHITEM;
							SubItemNum = SavSubItemNum;
						}
					}
				}
			}

			if ((!StripPopUp) && (MenuMode == MODE_KEYBOARD))
			{
				MenuMode = MODE_KEYBOARD;
				if (MenuSubBoxSwapped)
				{
					switch (Event.ie_Code)
					{
					case KEYCODE_RETURN:
					case KEYCODE_ENTER:
						Ende = (MenuSelected (!Shift) && (!Shift));
						break;

					case ESC:
						if(Shift)
							*Cancel = Ende = TRUE;
						else
							CleanUpMenuSubBox ();
						break;

					case CRSLEFT:
						CleanUpMenuSubBox ();
						CleanUpMenuBox ();
						if (Shift)
							SelectNextMenu (-1);
						else
						{
							ZwNum = MenuNum;
							if (SelectPrevMenu (MenuNum) == ZwNum)
								SelectPrevMenu (0x7f);
						}
						DrawMenuBox (AktMenu, TRUE);
						break;

					case CRSRIGHT:
						CleanUpMenuSubBox ();
						CleanUpMenuBox ();
						if (Shift)
							SelectPrevMenu (0x7f);
						else
						{
							ZwNum = MenuNum;
							if (SelectNextMenu (MenuNum) == ZwNum)
								SelectNextMenu (-1);
						}
						DrawMenuBox (AktMenu, TRUE);
						break;

					case CRSUP:
						if (Shift)
							SelectNextSubItem (-1);
						else
						{
							ZwNum = SubItemNum;
							if (SelectPrevSubItem (SubItemNum) == ZwNum)
								SelectPrevSubItem (0x7f);
						}
						break;
					case CRSDOWN:
						if (Shift)
							SelectPrevSubItem (0x7f);
						else
						{
							ZwNum = SubItemNum;
							if (SelectNextSubItem (SubItemNum) == ZwNum)
								SelectNextSubItem (-1);
						}
						break;

					default:
						if (MapRawKey (&Event, CharBuf, 10, NULL) == 1)
						{
							ASCIIChar = CharBuf[0];
							if (FindSubItemChar (ASCIIChar, &IsSingle) && IsSingle)
								Ende = (MenuSelected (!Shift) && (!Shift));
						}
						break;
					}
				}
				else if (MenuBoxSwapped)
				{
					switch (Event.ie_Code)
					{
					case KEYCODE_RETURN:
					case KEYCODE_ENTER:
						if (AktItem->SubItem)
							DrawMenuSubBox (AktMenu, AktItem, TRUE);
						else
							Ende = (MenuSelected (!Shift) && (!Shift));
						break;

					case ESC:
						CleanUpMenuBox ();
						break;

					case CRSLEFT:
						CleanUpMenuBox ();
						if (Shift)
							SelectNextMenu (-1);
						else
						{
							ZwNum = MenuNum;
							if (SelectPrevMenu (MenuNum) == ZwNum)
								SelectPrevMenu (0x7f);
						}
						DrawMenuBox (AktMenu, TRUE);
						break;

					case CRSRIGHT:
						CleanUpMenuBox ();
						if (Shift)
							SelectPrevMenu (0x7f);
						else
						{
							ZwNum = MenuNum;
							if (SelectNextMenu (MenuNum) == ZwNum)
								SelectNextMenu (-1);
						}
						DrawMenuBox (AktMenu, TRUE);
						break;

					case CRSUP:
						if (Shift)
							SelectNextItem (-1);
						else
						{
							ZwNum = ItemNum;
							if (SelectPrevItem (ItemNum) == ZwNum)
								SelectPrevItem (0x7f);
						}
						break;

					case CRSDOWN:
						if (Shift)
							SelectPrevItem (0x7f);
						else
						{
							ZwNum = ItemNum;
							if (SelectNextItem (ItemNum) == ZwNum)
								SelectNextItem (-1);
						}
						break;
					default:
						if (MapRawKey (&Event, CharBuf, 10, NULL) == 1)
						{
							ASCIIChar = CharBuf[0];
							if (FindItemChar (ASCIIChar, &IsSingle) && IsSingle)
							{
								if (AktItem->SubItem)
									DrawMenuSubBox (AktMenu, AktItem, TRUE);
								else
									Ende = (MenuSelected (!Shift) && (!Shift));
							}
						}
						break;
					}
				}
				else
				{
					switch (Event.ie_Code)
					{
					case KEYCODE_RETURN:
					case KEYCODE_ENTER:
					case CRSDOWN:
						DrawMenuBox (AktMenu, TRUE);
						break;

					case ESC:
						Ende = TRUE;
						*Cancel = TRUE;
						break;

					case CRSLEFT:
						if (Shift)
							SelectNextMenu (-1);
						else
						{
							ZwNum = MenuNum;
							if (SelectPrevMenu (MenuNum) == ZwNum)
								SelectPrevMenu (0x7f);
						}
						break;

					case CRSRIGHT:
						if (Shift)
							SelectPrevMenu (0x7f);
						else
						{
							ZwNum = MenuNum;
							if (SelectNextMenu (MenuNum) == ZwNum)
								SelectNextMenu (-1);
						}
						break;
					default:
						if (MapRawKey (&Event, CharBuf, 10, NULL) == 1)
						{
							ASCIIChar = CharBuf[0];
							if (FindMenuChar (ASCIIChar, &IsSingle) && IsSingle)
								DrawMenuBox (AktMenu, TRUE);
						}
						break;
					}
				}
			}
			else
				Ende = *Cancel = (Event.ie_Code == ESC);
			break;

		case EVT_MOUSEMENU:
			if ((Event.ie_Code == IECODE_RBUTTON) && (MenWin->DMRequest != NULL))
			{
				CurrentTime (&CurrSecs, &CurrMics);
				if (DoubleClick (DMROldSecs, DMROldMics, CurrSecs, CurrMics))
				{
					GlobalDMRequest = TRUE;
					Ende = TRUE;
					*Cancel = TRUE;
					break;
				}
			}
			Shift = (NULL != (Event.ie_Qualifier & (IEQUALIFIER_RSHIFT | IEQUALIFIER_LSHIFT)));
			if (SelectSpecial)
			{
				OutOfBounds = LookMouse (MenScr->MouseX, MenScr->MouseY, FALSE);
				if (Event.ie_Code == (IECODE_RBUTTON | IECODE_UP_PREFIX))
				{
					SelectSpecial = FALSE;
					Ende = (OutOfBounds || MenuSelected (TRUE));
					*Cancel = OutOfBounds;
				}
				else
				{
					if (Event.ie_Code == IECODE_LBUTTON)
						MenuSelected (FALSE);
				}
			}
			else if (MenuMode == MODE_STD)
			{
				Ende = (Event.ie_Code == (IECODE_RBUTTON | IECODE_UP_PREFIX));

				LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
				if (Event.ie_Code == IECODE_LBUTTON)
					MenuSelected (FALSE);

				if (Ende)
					MenuSelected (TRUE);
			}
			else if (MenuMode == MODE_STICKY)
			{
				Ende = (Event.ie_Code == IECODE_RBUTTON);

				OutOfBounds = LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
				if (Event.ie_Code == IECODE_LBUTTON)
					Ende = OutOfBounds || (MenuSelected (!Shift) && (!Shift));
				else if (Ende)
					MenuSelected (TRUE);
			}
			else if (MenuMode == MODE_SELECT || MenuMode == MODE_KEYBOARD)
			{
				Ende = *Cancel = (Event.ie_Code == IECODE_RBUTTON);

				if (!Ende && (Event.ie_Code & IECODE_LBUTTON))
				{
					MenuMode = MODE_SELECT;
					if (Event.ie_Code == IECODE_LBUTTON)
						Ende = LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);
					else
						Ende = (MenuSelected (!Shift) && (!Shift) && (Event.ie_Code == (IECODE_LBUTTON | IECODE_UP_PREFIX)));
				}
			}
			break;

		case EVT_MOUSEMOVE:

			Shift = (NULL != (Event.ie_Qualifier & (IEQUALIFIER_RSHIFT | IEQUALIFIER_LSHIFT)));

			if ((MenuMode != MODE_SELECT || SelectSpecial) && MenuMode != MODE_KEYBOARD)
				LookMouse (MenScr->MouseX, MenScr->MouseY, TRUE);

			if (MenuMode == MODE_STD)
			{
				Ende = (!(Event.ie_Qualifier & IEQUALIFIER_RBUTTON));

				if (Ende || (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON) != NULL)
					MenuSelected (TRUE);
			}

			if (MenuMode == MODE_STICKY)
			{
				if (!Shift && (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON))
					Ende = MenuSelected (TRUE);
				else if (Ende || (Shift && (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON)))
					MenuSelected (TRUE);
			}

			if (MenuMode == MODE_SELECT && (Event.ie_Qualifier & IEQUALIFIER_LEFTBUTTON))
				LookMouse (MenScr->MouseX, MenScr->MouseY, FALSE);

			break;

		case EVT_KBDMENU:
			Ende = TRUE;
			*Cancel = TRUE;
			break;
		default:
			break;
		}
	}

	LEAVE();

	return (Ende);
}

VOID
ProcessIntuiMenu (VOID)
{
	ULONG SigMask, Signals;
	BOOL Ende;
	BOOL Cancel;
	ULONG TickCounter;
	ULONG TimeMask;
	BOOL Poll, Ticking;
	LONG LastX,LastY,NewX,NewY,DeltaX,DeltaY;

	ENTER();

	Ende = FALSE;
	Cancel = FALSE;
	Ticking = TRUE;

	TimeMask = PORTMASK (TimerPort);
	SigMask = CxSignalMask | TickSigMask | TimeMask;

	TickCounter = 0;

	LastX = LastY = 0;

	StartTimeRequest (TimerIO, 0, MILLION / 4);

	Poll = FALSE;

	while (!Ende)
	{
		if (Poll)
			Signals = SetSignal (0, SigMask) & SigMask;
		else
			Signals = Wait (SigMask);

		if (Signals & TimeMask)
		{
			WaitIO (TimerIO);

			if (TickCounter++ > 2)
			{
				Cancel = TRUE;

				Ticking = FALSE;

				break;
			}
			else
			{
				StartTimeRequest (TimerIO, 0, MILLION / 4);
			}
		}

		if (Signals & TickSigMask)
		{
			TickCounter = 0;

			if(AktPrefs.mmp_Delayed)
			{
				NewX = MenScr->MouseX;
				NewY = MenScr->MouseY;

				DeltaX = abs(LastX - NewX);
				DeltaY = abs(LastY - NewY);

				LastX = NewX;
				LastY = NewY;

/*				if(DeltaX < 3 && DeltaY < 3)*/

				if(StripPopUp)
				{
					if(DeltaY < 3)
						Shoot();
				}
				else
				{
					if(DeltaX < 3 && DeltaY < 3)
						Shoot();
				}
			}
		}

		if ((Signals & CxSignalMask) || Poll)
		{
			struct Message *Msg;

			if (Msg = GetMsg (CxMsgPort))
			{
				TickCounter = 0;

				Ende = CheckCxMsgAct ((CxMsg *) Msg, &Cancel);
				Poll = TRUE;
			}
			else
				Poll = FALSE;
		}
	}

	if (Ticking)
		StopTimeRequest (TimerIO);

	if (Cancel)
	{
		FirstSelectedMenu = NULL;
		LastSelectedMenu = NULL;
		FirstMenuNum = MENUNULL;
	}

	LEAVE();
}

VOID
EndIntuiMenu (BOOL ReleaseMenuAct)
{
	ENTER();

	ObtainSemaphore (GetPointerSemaphore);

	MenScr = NULL;
	MenWin = NULL;
	MenStrip = NULL;

	ReleaseSemaphore (GetPointerSemaphore);

	if (ReleaseMenuAct)
		ReleaseSemaphore (MenuActSemaphore);

	LEAVE();
}

BOOL
CheckCxMsg (CxMsg * Msg)
{
	BOOL Ende;
	ULONG MsgID, MsgType;
	APTR MsgData;
	ULONG MyLock;
	BOOL Ok;

	ULONG CurrSecs, CurrMics;

	static WORD MenSaveMouseX = -1;
	static WORD MenSaveMouseY = -1;

	struct InputEvent *MyNewEvent;

	struct ViewPortExtra *VPExtra;
	struct ColorMap *cm;

	static struct InputEvent Event;
	static BOOL IgnoreMouse = FALSE;
	static BOOL DoWait = FALSE;
	static struct Screen *OldMenScr = NULL;

	static WORD OrigLeftEdge = 0;
	static WORD OrigTopEdge = 0;

	static struct TagItem ti[] =
	{VTAG_VIEWPORTEXTRA_GET, NULL,
	 VTAG_END_CM, NULL};

	ENTER();

	if (CheckReply ((struct Message *) Msg))
		return (FALSE);

	Ende = FALSE;

	MsgID = CxMsgID (Msg);
	MsgType = CxMsgType (Msg);
	MsgData = CxMsgData (Msg);
	if (MsgType == CXM_IEVENT)
	{
		Event = *((struct InputEvent *) MsgData);
	}

	ReplyMsg ((struct Message *) Msg);

	MyLock = LockIBase (0);
	if (IntuitionBase->ActiveWindow)
		MenScr = IntuitionBase->ActiveWindow->WScreen;
	else
		MenScr = NULL;

	UnlockIBase (MyLock);

	if (OldMenScr != MenScr)
	{
		IgnoreMouse = DoWait = FALSE;
	}

	GlobalDMRequest = FALSE;

	switch (MsgType)
	{
	case CXM_IEVENT:
		switch (MsgID)
		{
		case EVT_KBDMENU:
			if (AktPrefs.mmp_KCEnabled && (MenScr != NULL))
			{
				if (!IgnoreMouse)
				{
					OrigTopEdge = MenScr->TopEdge;
					OrigLeftEdge = MenScr->LeftEdge;
				}
				cm = MenScr->ViewPort.ColorMap;
				ti[0].ti_Tag = VTAG_VIEWPORTEXTRA_GET;
				ti[0].ti_Data = NULL;
				ti[1].ti_Tag = VTAG_END_CM;
				if (VideoControl (cm, ti) == NULL)
				{
					VPExtra = (struct ViewPortExtra *) ti[0].ti_Data;
					MenDispClip = VPExtra->DisplayClip;
					if (!IgnoreMouse)
						MoveScreen (MenScr, (OrigLeftEdge < 0) ? (-OrigLeftEdge) : 0,
										(OrigTopEdge < 0) ? (-OrigTopEdge) : 0);
				}
				else
				{
					MenDispClip.MinX = 0;
					MenDispClip.MinY = 0;
					MenDispClip.MaxX = MenScr->Width - 1;
					MenDispClip.MaxY = MenScr->Height - 1;
				}

				if (AktPrefs.mmp_KCGoTop && MenScr->MouseY > MenScr->BarHeight)
				{
					MenSaveMouseX = MenScr->MouseX;
					MenSaveMouseY = MenScr->MouseY;
					MoveMouse (MenDispClip.MaxX - 16 - MenScr->LeftEdge, MenScr->BarHeight - 1, TRUE, &Event, MenScr);
					DoWait = TRUE;
					IgnoreMouse = TRUE;
					OldMenScr = MenScr;

				}
				else
				{
					if (DoWait)
						Delay (1);
					DoWait = FALSE;
					OldMenScr = MenScr;
					if (DoIntuiMenu (MODE_KEYBOARD, FALSE, TRUE))
						IgnoreMouse = FALSE;
					else
						IgnoreMouse = TRUE;

					if (MenSaveMouseX != -1)
					{
						MoveMouse (MenSaveMouseX, MenSaveMouseY, FALSE, NULL, OldMenScr);
						MenSaveMouseX = -1;
					}

					MoveScreen (OldMenScr, OrigLeftEdge - (WORD) OldMenScr->LeftEdge,
										OrigTopEdge - (WORD) OldMenScr->TopEdge);

				}
			}
			break;

		case EVT_MOUSEMENU:

			if (GlobalDMRCount > 0)
			{
				GlobalDMRCount--;
				break;
			}

			if ((IgnoreMouse || ActivateCxObj (MouseTransl, TRUE)) && MenScr != NULL)
			{
				if (!IgnoreMouse)
				{
					CurrentTime (&CurrSecs, &CurrMics);

					if (GlobalLastWinDMREnable && DoubleClick (DMROldSecs, DMROldMics, CurrSecs, CurrMics))
					{
						GlobalDMRequest = TRUE;
					}
					else
					{
						DMROldSecs = CurrSecs;
						DMROldMics = CurrMics;

						if ((AktPrefs.mmp_MenuType == MT_AUTO && MenScr->MouseY <= MenScr->BarHeight) || AktPrefs.mmp_MenuType == MT_PULLDOWN)
							Ok = DoIntuiMenu (AktPrefs.mmp_PDMode, FALSE, FALSE);
						else
							Ok = DoIntuiMenu (AktPrefs.mmp_PUMode, TRUE, FALSE);

						if ((!Ok) && (!GlobalDMRequest))
						{
							if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_CLEAR | MEMF_PUBLIC))
							{
								ActivateCxObj (MouseTransl, FALSE);
								*MyNewEvent = Event;
								MyNewEvent->ie_NextEvent = NULL;

								InputIO->io_Data = (APTR) MyNewEvent;
								InputIO->io_Length = sizeof (struct InputEvent);

								InputIO->io_Command = IND_WRITEEVENT;
								DoIO ((struct IORequest *) InputIO);
								FreeVecPooled (MyNewEvent);
							}
						}
					}

					if (GlobalDMRequest)
					{
						GlobalDMRequest = FALSE;
						if (MyNewEvent = AllocVecPooled (sizeof (struct InputEvent), MEMF_CLEAR | MEMF_PUBLIC))
						{
							ActivateCxObj (MouseTransl, FALSE);
							GlobalDMRCount = 1;
							*MyNewEvent = Event;
							MyNewEvent->ie_NextEvent = NULL;
							InputIO->io_Data = (APTR) MyNewEvent;
							InputIO->io_Length = sizeof (struct InputEvent);

							InputIO->io_Command = IND_WRITEEVENT;
							DoIO ((struct IORequest *) InputIO);

							GetSysTime (&(MyNewEvent->ie_TimeStamp));
							MyNewEvent->ie_Class = IECLASS_RAWMOUSE;
							MyNewEvent->ie_Code = IECODE_RBUTTON | IECODE_UP_PREFIX;
							MyNewEvent->ie_Qualifier = PeekQualifier ();
							MyNewEvent->ie_NextEvent = NULL;
							DoIO ((struct IORequest *) InputIO);

							*MyNewEvent = Event;
							GetSysTime (&(MyNewEvent->ie_TimeStamp));
							MyNewEvent->ie_NextEvent = NULL;
							DoIO ((struct IORequest *) InputIO);

							FreeVecPooled (MyNewEvent);
						}
					}
				}
				else
				{
					OldMenScr = MenScr;
					DoIntuiMenu (MODE_KEYBOARD, FALSE, FALSE);
					IgnoreMouse = FALSE;
					MoveScreen (OldMenScr, OrigLeftEdge - (WORD) OldMenScr->LeftEdge,
										OrigTopEdge - (WORD) OldMenScr->TopEdge);
				}
			}
			break;
		case EVT_POPPREFS:
			StartPrefs ();
			break;
		default:
			break;
		}
		break;
	case CXM_COMMAND:
		switch (MsgID)
		{
		case CXCMD_DISABLE:
			Deactivate ();
			break;
		case CXCMD_ENABLE:
			Activate ();
			break;
		case CXCMD_KILL:
			Ende = CheckEnde ();
			break;
		case CXCMD_UNIQUE:
		case CXCMD_APPEAR:
			StartPrefs ();
			break;
		case CXCMD_DISAPPEAR:
			StopPrefs ();
			break;
		default:
			break;
		}
		break;
	}

	if (DoGlobalQuit)
		Ende = TRUE;

	LEAVE();

	return (Ende);
}

BOOL
CheckMMMsgPort (struct MMMessage * MMMsg)
{
	BOOL Ende;

	ENTER();

	Ende = FALSE;

	switch (MMMsg->Class)
	{
	case MMC_REMOVE:
		ReplyMsg (MMMsg);
		Ende = CheckEnde ();
		break;
	case MMC_NEWCONFIG:

		if(((struct MMPrefs *) MMMsg->Ptr1)->mmp_Magic != MMPREFS_MAGIC || ((struct MMPrefs *) MMMsg->Ptr1)->mmp_Version < MMPREFS_VERSION)
		{
			ReplyMsg (MMMsg);
			break;
		}

		ActivateCxObj (Broker, FALSE);

		if (Stricmp (((struct MMPrefs *) MMMsg->Ptr1)->mmp_KCKeyStr, AktPrefs.mmp_KCKeyStr))
		{
			CxObj *NewKbdFilter;

			if (NewKbdFilter = HotKey (((struct MMPrefs *) MMMsg->Ptr1)->mmp_KCKeyStr, CxMsgPort, EVT_KBDMENU))
			{
				RemoveCxObj (KbdFilter);
				DeleteCxObj (KbdFilter);
				AttachCxObj (Broker, KbdFilter = NewKbdFilter);
			}
		}

		if (Stricmp (((struct MMPrefs *) MMMsg->Ptr1)->mmp_BackFill, AktPrefs.mmp_BackFill))
		{
			LoadBackFill( ((struct MMPrefs *) MMMsg->Ptr1)->mmp_BackFill );
		}

		AktPrefs = *(struct MMPrefs *) MMMsg->Ptr1;

		if (MMMsg->Ptr2)
		{
			STRPTR String;

			String = MMMsg->Ptr2;

			if (Stricmp (String, Cx_Popkey))
			{
				CxObj *NewPrefsFilter;

				if (NewPrefsFilter = HotKey (Cx_Popkey, CxMsgPort, EVT_POPPREFS))
				{
					RemoveCxObj (PrefsFilter);
					DeleteCxObj (PrefsFilter);
					AttachCxObj (Broker, PrefsFilter = NewPrefsFilter);
				}
			}
		}

		ActivateCxObj(StdKbdFilter, (AktPrefs.mmp_KCRAltRCommand != FALSE) && (AktPrefs.mmp_KCEnabled != FALSE));

		if(KbdFilter)
			ActivateCxObj(KbdFilter, AktPrefs.mmp_KCEnabled != FALSE);

		ActivateCxObj (Broker, TRUE);

		ReplyMsg (MMMsg);
		break;
	case MMC_GETCONFIG:
		MMMsg->Ptr1 = &AktPrefs;
		MMMsg->Arg1 = MagicActive;
		MMMsg->Ptr2 = Cx_Popkey;
		ReplyMsg (MMMsg);
		break;
	case MMC_ENABLE:
		if (MMMsg->Arg1)
			Activate ();
		else
			Deactivate ();
		ReplyMsg (MMMsg);
		break;
	}

	LEAVE();

	return (Ende);
}

BOOL
RealWindow (struct Window * ThisWindow)
{
	if (TypeOfMem (ThisWindow))
	{
		struct Screen *Screen;

		for (Screen = IntuitionBase->FirstScreen; Screen; Screen = Screen->NextScreen)
		{
			if (Screen == ThisWindow->WScreen)
			{
				struct Window *Window;

				for (Window = Screen->FirstWindow; Window; Window = Window->NextWindow)
				{
					if (Window == ThisWindow)
						return (TRUE);
				}

				return (FALSE);
			}
		}
	}

	return (FALSE);
}

VOID
ProcessCommodity (VOID)
{
	ULONG SigMask, SigRec, MMMsgPortMask, IMsgReplyMask;
	BOOL Ende, DoWait;
	struct Message *Msg;
	struct MMMessage *MMMsg;
	struct IntuiMessage *IMsg;

	ENTER();

	MMMsgPortMask = PORTMASK (MMMsgPort);
	IMsgReplyMask = PORTMASK (IMsgReplyPort);

	Ende = FALSE;

	while (!Ende)
	{
		SigMask = CxSignalMask | MMMsgPortMask | SIGBREAKF_CTRL_C | IMsgReplyMask;

		DoWait = TRUE;

		if(AktPrefs.mmp_VerifyPatches && MagicActive && NOT AllPatchesOnTop())
		{
			Deactivate();
		}

		if (Msg = GetMsg (CxMsgPort))
		{
			DoWait = FALSE;
			Ende = CheckCxMsg ((CxMsg *) Msg);
		}

		if (MMMsg = (struct MMMessage *) GetMsg (MMMsgPort))
		{
			DoWait = FALSE;
			Ende = CheckMMMsgPort (MMMsg);
		}

		/* Was hier zur�ckkommt, sollten Verify-Messages sein. */
		while (IMsg = (struct IntuiMessage *) GetMsg (IMsgReplyPort))
		{
			/* Wer zu sp�t kommt, den bestraft das Leben. Hier werden die
			 * zu sp�t beantworteten IDCMP_MENUVERIFY-Messages
			 * abgewickelt.
			 */
			if (IMsg->Class == IDCMP_MENUVERIFY && (IMsg->Code == MENUHOT || IMsg->Code == MENUCANCEL))
			{
				ULONG IntuiLock;

				/* N�tig f�r RealWindow() und damit das Fenster bis zum Schlu� vorhanden bleibt. */
				IntuiLock = LockIBase (NULL);

				/* Gibt es das Fenster noch? */
				if (RealWindow (IMsg->IDCMPWindow))
				{
					UWORD Code;

					/* Falls das Men� abgebrochen wurde, kommt zuerst IDCMP_MOUSEBUTTONS. */
					if (IMsg->Code == MENUCANCEL && (IMsg->IDCMPWindow->IDCMPFlags & IDCMP_MOUSEBUTTONS))
					{
						Code = MENUUP;
						SendIntuiMessage (IDCMP_MOUSEBUTTONS, &Code, PeekQualifier (), NULL, IMsg->IDCMPWindow, NULL, FALSE);
					}

					/* Standardbehandlung... */
					Code = MENUNULL;
					SendIntuiMessage (IDCMP_MENUPICK, &Code, PeekQualifier (), NULL, IMsg->IDCMPWindow, NULL, FALSE);
				}

				UnlockIBase (IntuiLock);
			}

			FreeVecPooled (IMsg);
			IMsgReplyCount--;
		}

		if (DoWait)
		{
			SigRec = Wait (SigMask);

			if (SigRec & SIGBREAKF_CTRL_C)
			{
				Ende = CheckEnde ();
			}
		}
	}

	LEAVE();
}

VOID
StopPrefs (VOID)
{
	struct MsgPort *SomePort;

	Forbid ();

	if (SomePort = FindPort ("� MagicMenu Preferences �"))
		Signal (SomePort->mp_SigTask, SIGBREAKF_CTRL_C);

	Permit ();
}

VOID
StartPrefs (VOID)
{
	BPTR In, Out;

	if (In = Open ("NIL:", MODE_NEWFILE))
	{
		if (Out = Open ("NIL:", MODE_NEWFILE))
		{
			if (WBMsg && !Cli ())
				AttachCLI (WBMsg);

			SystemTags ("MagicMenuPrefs",
				SYS_Input, In,
				SYS_Output, Out,
				SYS_Asynch, TRUE,
				NP_Name, "MagicMenuPrefs",
				NP_StackSize, 8192L,
				TAG_DONE);
		}
		else
			Close (In);
	}
}

VOID
MyArgString (char *Result, struct DiskObject *DO, char *TT, char *Default, LONG Len, BOOL Upcase)
{
	char *Found;

	Found = FindToolType (DO->do_ToolTypes, (char *) TT);

	if (Found)
		strncpy (Result, Found, Len);
	else
		strncpy (Result, Default, Len);

	if (Upcase)
	{
		UBYTE *c;

		c = (UBYTE *) Result;

		while (*c)
			*c++ = ToUpper (*c);
	}
}

LONG
MyArgInt (struct DiskObject *DO, char *TT, LONG Default)
{
	char Str[80];
	LONG Result;

	MyArgString (Str, DO, TT, "", 79, FALSE);

	if (StrToLong (Str, &Result) <= 0)
		Result = Default;

	return (Result);
}

VOID
CheckArguments (struct WBStartup * startupMsg)
{
	UBYTE FName[MAX_FILENAME_LENGTH];
	struct DiskObject *DiskObj;
	BPTR cd;

	if(startupMsg != NULL)
	{
		/* Stephan: Mu� in das ProgVerzeichnis cd'en, sonst
		 * geht's nicht. StormC spezifisch ?
		 */
		cd = CurrentDir( startupMsg->sm_ArgList[0].wa_Lock );
		strcpy (FName, startupMsg->sm_ArgList[0].wa_Name);
	}
	else
	{
		strcpy (FName, "PROGDIR:");
		strcat (FName, ProgName);
	}

	IconBase = OpenLibrary ("icon.library", 0);
	if (IconBase != NULL)
	{
		DiskObj = GetDiskObject (FName);
		if (DiskObj != NULL)
		{
			Cx_Pri = MyArgInt (DiskObj, TT_CX_PRI, DT_CX_PRI);

			MyArgString (Cx_PopupStr, DiskObj, TT_CX_POPUP, DT_CX_POPUP, ANSWER_LEN, FALSE);
			MyArgString (Cx_Popkey, DiskObj, TT_CX_POPKEY, DT_CX_POPKEY, LONGANSWER_LEN, FALSE);
#ifdef __MIXEDBINARY__
			{
			extern BOOL PPCDirect;
			char answer[ANSWER_LEN + 1];

			MyArgString (answer, DiskObj, "ENABLEPPC", "YES", ANSWER_LEN, FALSE);

			if (!Stricmp (answer, "YES"))
				PowerPCBase = OpenLibrary("powerpc.library",14);

			MyArgString (answer, DiskObj, "PPCDIRECT", "NO", ANSWER_LEN, FALSE);
			PPCDirect = (Stricmp (answer, "YES") == 0L);
			}
#endif
			if (!Stricmp (Cx_PopupStr, "YES"))
				Cx_Popup = TRUE;
			else
				Cx_Popup = FALSE;

			FreeDiskObject (DiskObj);
		}

		CloseLibrary (IconBase);
	}

	if(startupMsg != NULL)
		CurrentDir(cd);
}

BOOL
LoadPrefs (char *Name, BOOL Report)
{
	STATIC struct StorageItem PrefsStorage[] =
	{
		DECLARE_ITEM(MMPrefs,mmp_MenuType,				SIT_UBYTE,		"MenuType"),
		DECLARE_ITEM(MMPrefs,mmp_Enabled,				SIT_BOOLEAN,	"Enabled"),
		DECLARE_ITEM(MMPrefs,mmp_MarkSub,				SIT_BOOLEAN,	"MarkSub"),
		DECLARE_ITEM(MMPrefs,mmp_DblBorder,				SIT_BOOLEAN,	"DblBorder"),
		DECLARE_ITEM(MMPrefs,mmp_NonBlocking,			SIT_BOOLEAN,	"NonBlocking"),
		DECLARE_ITEM(MMPrefs,mmp_KCEnabled,				SIT_BOOLEAN,	"KCEnabled"),
		DECLARE_ITEM(MMPrefs,mmp_KCGoTop,				SIT_BOOLEAN,	"KCGoTop"),
		DECLARE_ITEM(MMPrefs,mmp_KCRAltRCommand,		SIT_BOOLEAN,	"KCAltRCommand"),
		DECLARE_ITEM(MMPrefs,mmp_PUCenter,				SIT_BOOLEAN,	"KCPUCenter"),
		DECLARE_ITEM(MMPrefs,mmp_PreferScreenColours,	SIT_BOOLEAN,	"PreferScreenColours"),
		DECLARE_ITEM(MMPrefs,mmp_Delayed,				SIT_BOOLEAN,	"Delayed"),
		DECLARE_ITEM(MMPrefs,mmp_DrawFrames,			SIT_BOOLEAN,	"DrawFrames"),
		DECLARE_ITEM(MMPrefs,mmp_CastShadows,			SIT_BOOLEAN,	"CastShadows"),
		DECLARE_ITEM(MMPrefs,mmp_PDMode,				SIT_UBYTE,		"PDMode"),
		DECLARE_ITEM(MMPrefs,mmp_PDLook,				SIT_UBYTE,		"PDLook"),
		DECLARE_ITEM(MMPrefs,mmp_PUMode,				SIT_UBYTE,		"PUMode"),
		DECLARE_ITEM(MMPrefs,mmp_PULook,				SIT_UBYTE,		"PULook"),
		DECLARE_ITEM(MMPrefs,mmp_KCKeyStr,				SIT_TEXT,		"KCKeyStr"),
		DECLARE_ITEM(MMPrefs,mmp_Precision,				SIT_WORD,		"Precision"),

		DECLARE_ITEM(MMPrefs,mmp_LightEdge.R,			SIT_ULONG,		"LightEdge.R"),
		DECLARE_ITEM(MMPrefs,mmp_LightEdge.G,			SIT_ULONG,		"LightEdge.G"),
		DECLARE_ITEM(MMPrefs,mmp_LightEdge.B,			SIT_ULONG,		"LightEdge.B"),
		DECLARE_ITEM(MMPrefs,mmp_DarkEdge.R,			SIT_ULONG,		"DarkEdge.R"),
		DECLARE_ITEM(MMPrefs,mmp_DarkEdge.G,			SIT_ULONG,		"DarkEdge.G"),
		DECLARE_ITEM(MMPrefs,mmp_DarkEdge.B,			SIT_ULONG,		"DarkEdge.B"),
		DECLARE_ITEM(MMPrefs,mmp_Background.R,			SIT_ULONG,		"Background.R"),
		DECLARE_ITEM(MMPrefs,mmp_Background.G,			SIT_ULONG,		"Background.G"),
		DECLARE_ITEM(MMPrefs,mmp_Background.B,			SIT_ULONG,		"Background.B"),
		DECLARE_ITEM(MMPrefs,mmp_TextCol.R,				SIT_ULONG,		"Text.R"),
		DECLARE_ITEM(MMPrefs,mmp_TextCol.G,				SIT_ULONG,		"Text.G"),
		DECLARE_ITEM(MMPrefs,mmp_TextCol.B,				SIT_ULONG,		"Text.B"),
		DECLARE_ITEM(MMPrefs,mmp_HiCol.R,				SIT_ULONG,		"HighText.R"),
		DECLARE_ITEM(MMPrefs,mmp_HiCol.G,				SIT_ULONG,		"HighText.G"),
		DECLARE_ITEM(MMPrefs,mmp_HiCol.B,				SIT_ULONG,		"HighText.B"),
		DECLARE_ITEM(MMPrefs,mmp_FillCol.R,				SIT_ULONG,		"Fill.R"),
		DECLARE_ITEM(MMPrefs,mmp_FillCol.G,				SIT_ULONG,		"Fill.G"),
		DECLARE_ITEM(MMPrefs,mmp_FillCol.B,				SIT_ULONG,		"Fill.B"),

		DECLARE_ITEM(MMPrefs,mmp_Transparency,			SIT_BOOLEAN,	"Transparency"),	/* == item #37 */
		DECLARE_ITEM(MMPrefs,mmp_HighlightDisabled,		SIT_BOOLEAN,	"HighlightDisabled"),
		DECLARE_ITEM(MMPrefs,mmp_SeparatorBarStyle,		SIT_UBYTE,		"SeparatorBarStyle"),
		DECLARE_ITEM(MMPrefs,mmp_VerifyPatches,			SIT_BOOLEAN,	"VerifyPatches"),
		DECLARE_ITEM(MMPrefs,mmp_FixPatches,			SIT_BOOLEAN,	"FixPatches"),
		DECLARE_ITEM(MMPrefs,mmp_BackFill,				SIT_TEXT,		"BackFill"),
		DECLARE_ITEM(MMPrefs,mmp_PDTransparent,		SIT_BOOLEAN,	"PDTransparent"),
		DECLARE_ITEM(MMPrefs,mmp_TransHighlight,	SIT_BOOLEAN,	"TransHighlight"),
		DECLARE_ITEM(MMPrefs,mmp_TransBackfill,		SIT_BOOLEAN,	"TransBackfill")
	};

	struct MMPrefs LocalPrefs;
	LONG NumItemsFound;
	LONG Error;

	memset(&LocalPrefs,0,sizeof(LocalPrefs));

	Error = RestoreData(Name,"MagicMenu",MMPREFS_VERSION,PrefsStorage,ITEM_TABLE_SIZE(PrefsStorage),&LocalPrefs,&NumItemsFound);
	if(Error != 0)
	{
		if(NumItemsFound < 37 || Error != ERROR_REQUIRED_ARG_MISSING)
		{
			if(Report)
				ShowRequest(GetString(MSG_ERROR_IN_CONFIGURATION_FILE_TXT));

			return(FALSE);
		}
	}

	CopyMem(&LocalPrefs,&AktPrefs,sizeof(struct MMPrefs));
	return(TRUE);

	/*
	BPTR FH;
	struct MMPrefs ZwPrefs;
	ULONG Len;

	if (!(FH = Open (ConfigFile, MODE_OLDFILE)))
	{
		if (Report)
			ShowRequest (GetString (MSG_COULD_NOT_OPEN_TXT));
		return (FALSE);
	}

	Len = Read (FH, &ZwPrefs, sizeof (struct MMPrefs));

	Close (FH);

	if (Len == sizeof (struct MMPrefs) && ZwPrefs.mmp_Version == MMP_MAGIC)
	{
		memcpy (&AktPrefs, &ZwPrefs, sizeof (struct MMPrefs));
		return (TRUE);
	}
	else
	{
		if (Report)
			ShowRequest (GetString (MSG_ERROR_IN_CONFIGURATION_FILE_TXT));
		return (FALSE);
	}
	*/
}

VOID
ResetBrokerSetup ()
{
	ENTER();

	if (CxChanged)
	{
		struct Message *msg;
		while (msg = GetMsg (CxMsgPort))
		{
			if (!CheckReply (msg))
			{
				ReplyMsg (msg);
			}
		}

		ActivateCxObj (Broker, FALSE);

		SetFilterIX (MouseFilter, &MouseIX);

		RemoveCxObj (ActKbdFilter);

		RemoveCxObj (MouseMoveFilter);

		RemoveCxObj (MousePositionFilter);

		RemoveCxObj (MouseNewPositionFilter);

		RemoveCxObj (TickFilter);

		while (msg = GetMsg (CxMsgPort))
		{
			if (!CheckReply (msg))
			{
				ReplyMsg (msg);
			}
		}

		ActivateCxObj (Broker, TRUE);

		CxChanged = FALSE;
	}

	LEAVE();
}

VOID
ChangeBrokerSetup ()
{
	ENTER();

	if (!CxChanged)
	{
		struct Message *msg;

		ActivateCxObj (Broker, FALSE);

		SetFilterIX (MouseFilter, &ActiveMouseIX);

		AttachCxObj (Broker, ActKbdFilter);
		AttachCxObj (Broker, MouseMoveFilter);
		AttachCxObj (Broker, MousePositionFilter);
		AttachCxObj (Broker, MouseNewPositionFilter);
		AttachCxObj (Broker, TickFilter);

		while (msg = GetMsg (CxMsgPort))
		{
			if (!CheckReply (msg))
				ReplyMsg (msg);
		}

		ActivateCxObj (Broker, TRUE);
		CxChanged = TRUE;
	}

	LEAVE();
}

VOID
CleanupMenuActiveData ()
{
	if (CxBase)
	{
		DeleteCxObjAll (ActKbdFilter);
		ActKbdFilter = NULL;

		DeleteCxObjAll (MouseMoveFilter);
		MouseMoveFilter = NULL;

		DeleteCxObjAll (MousePositionFilter);
		MousePositionFilter = NULL;

		DeleteCxObjAll (MouseNewPositionFilter);
		MouseNewPositionFilter = NULL;

		DeleteCxObjAll (TickFilter);
		TickFilter = NULL;

		FreeSignal (TickSigNum);
		TickSigNum = -1;
	}
}

BOOL
SetupMenuActiveData ()
{
	if (!(ActKbdFilter = CxFilter (NULL)))
		return (FALSE);

	SetFilterIX (ActKbdFilter, &ActiveKbdIX);

	if (!(ActKbdSender = CxSender (CxMsgPort, EVT_KEYBOARD)))
		return (FALSE);

	AttachCxObj (ActKbdFilter, ActKbdSender);

	if (!(ActKbdTransl = CxTranslate (NULL)))
		return (FALSE);

	AttachCxObj (ActKbdFilter, ActKbdTransl);

	if (!(MouseMoveFilter = CxFilter (NULL)))
		return (FALSE);

	SetFilterIX (MouseMoveFilter, &ActiveMouseMoveIX);

	if (!(MouseMoveSender = CxSender (CxMsgPort, EVT_MOUSEMOVE)))
		return (FALSE);

	AttachCxObj (MouseMoveFilter, MouseMoveSender);

	if (!(MousePositionFilter = CxFilter (NULL)))
		return (FALSE);

	SetFilterIX (MousePositionFilter, &ActiveMousePositionIX);

	if (!(MousePositionSender = CxSender (CxMsgPort, EVT_MOUSEMOVE)))
		return (FALSE);

	AttachCxObj (MousePositionFilter, MousePositionSender);

	if (!(MouseNewPositionFilter = CxFilter (NULL)))
		return (FALSE);

	SetFilterIX (MouseNewPositionFilter, &ActiveMouseNewPositionIX);

	if (!(MouseNewPositionSender = CxSender (CxMsgPort, EVT_MOUSEMOVE)))
		return (FALSE);

	AttachCxObj (MouseNewPositionFilter, MouseNewPositionSender);

	if (!(TickFilter = CxFilter (NULL)))
		return (FALSE);

	SetFilterIX (TickFilter, &TickIX);

	if ((TickSigNum = AllocSignal (-1)) == -1)
		return (FALSE);

	TickSigMask = 1L << TickSigNum;

	if (!(TickSignal = CxSignal (FindTask (NULL), TickSigNum)))
		return (FALSE);

	AttachCxObj (TickFilter, TickSignal);

	return (TRUE);
}

VOID
CloseAll (VOID)
{
	struct Message *msg;
	struct MMMessage *MMMsg;

#ifdef AMITHLON
	if( x86elf != NULL ) close_elf( x86elf );
#endif

/*  StopHihoTask ();*/

	if (LocaleBase)
	{
		CloseCatalog (Catalog);
		CloseLibrary (LocaleBase);
	}

	if (RememberSemaphore)
		ObtainSemaphore (RememberSemaphore);

	if (GetPointerSemaphore)
		ObtainSemaphore (GetPointerSemaphore);

	if (MenuActSemaphore)
		ObtainSemaphore (MenuActSemaphore);

	/*****************************************************************************************/

	WindowGlyphExit ();

	/*****************************************************************************************/

	if (MMMsgPort)
	{
		RemPort (MMMsgPort);
		while (MMMsg = (struct MMMessage *) GetMsg (MMMsgPort))
		{
			MMMsg->Class = MMC_VOID;
			ReplyMsg (MMMsg);
		}
		DeleteMsgPort (MMMsgPort);
	}

	CleanupMenuActiveData ();

	if (Broker)
		DeleteCxObjAll (Broker);

	if (LibPatches)
		RemovePatches ();

	if (GlobalRemember)
		FreeGlobalRemember ();

	if (CxMsgPort)
	{
		while (msg = GetMsg (CxMsgPort))
		{
			if (!CheckReply (msg))
				ReplyMsg (msg);
		}
		DeleteMsgPort (CxMsgPort);
	}

	if (InputIO)
	{
		if (!CheckIO (InputIO))
		{
			AbortIO (InputIO);
			WaitIO (InputIO);
		}

		CloseDevice ((struct IORequest *) InputIO);

		DeleteIORequest ((struct IORequest *) InputIO);

		DeleteMsgPort (InputPort);
	}

	if (TimerIO)
	{
		if (!CheckIO (TimerIO))
		{
			AbortIO (TimerIO);
			WaitIO (TimerIO);
		}

		CloseDevice ((struct IORequest *) TimerIO);

		DeleteIORequest ((struct IORequest *) TimerIO);

		DeleteMsgPort (TimerPort);
	}

	if (IMsgReplyPort)
	{
		struct Message *Message;

		while (Message = GetMsg (IMsgReplyPort));

		DeleteMsgPort (IMsgReplyPort);
	}

	if (TimeoutRequest)
	{
		if (TimeoutRequest->tr_node.io_Device)
			CloseDevice ((struct IORequest *) TimeoutRequest);

		DeleteIORequest (TimeoutRequest);
	}

	if (TimeoutPort)
		DeleteMsgPort (TimeoutPort);

	if (CyberGfxBase != NULL)
		CloseLibrary (CyberGfxBase);

	if (KeymapBase)
		CloseLibrary (KeymapBase);

	if (GadToolsBase)
		CloseLibrary (GadToolsBase);

	if (CxBase)
		CloseLibrary (CxBase);

	if (UtilityBase)
		CloseLibrary (UtilityBase);

	if (LayersBase)
		CloseLibrary ((struct Library *) LayersBase);

	if (GfxBase)
		CloseLibrary ((struct Library *) GfxBase);

	if (IntuitionBase)
		CloseLibrary ((struct Library *) IntuitionBase);
#ifdef __MIXEDBINARY__
	if (PowerPCBase)
		CloseLibrary(PowerPCBase);
#endif
	if (DataTypesBase)
	{
		if (BackFillBM)
		{
			WaitBlit();
			FreeBitMap(BackFillBM);
			BackFillBM = NULL;
		}
		CloseLibrary(DataTypesBase);
	}
	/*****************************************************************************************/

	MemoryExit ();

	/*****************************************************************************************/
}

VOID
Complain (char *ErrTxt)
{
	if (IntuitionBase && ErrTxt[0])
		ShowRequest (GetString (MSG_SETUP_FAILURE_TXT), ErrTxt);
}

VOID
ErrorPrc (char *ErrTxt)
{
	Complain (ErrTxt);

	CloseAll ();

	exit (RETURN_FAIL);
}
#ifdef __STORMGCC__
void
wbmain(struct WBStartup *wbmsg)
{
	main(0L,(char**)wbmsg);
}
#endif

int
main (int argc, char **argv)
{
	BOOL Ok;
	LONG error;
	LONG z1;
	char *SPtr;

	ENTER();

	if(SysBase->lib_Version < 37)
	{
		((struct Process *)FindTask(NULL))->pr_Result2 = ERROR_INVALID_RESIDENT_LIBRARY;
		return(RETURN_FAIL);
	}

	if (argc == 0)
	{
		WBMsg = (struct WBStartup *) argv;
		strncpy (ProgName, WBMsg->sm_ArgList->wa_Name, 31);
	}
	else
	{
		WBMsg = NULL;
		z1 = strlen (argv[0]);
		SPtr = (char *) argv[0];
		while (z1 >= 0 && SPtr[z1] != '/' && SPtr[z1] != ':')
			z1--;
		strncpy (ProgName, &argv[0][z1 + 1], 31);
	}

	Forbid();

	if(FindPort(MMPORT_NAME))
	{
		Permit();

		StartPrefs();

		return(RETURN_OK);
	}
	else
		Permit();

	SetProgramName("MagicMenu");

	/*****************************************************************************************/

	WindowGlyphInit ();

	/*****************************************************************************************/

	if (LocaleBase = OpenLibrary ("locale.library", 38))
	{
		STATIC ULONG LocaleTags[] =
		{
			OC_BuiltInLanguage, (ULONG) "english",

			TAG_DONE
		};

		Catalog = OpenCatalogA (NULL, "magicmenu.catalog", (struct TagItem *) LocaleTags);
	}

	if (!(IntuitionBase = (struct IntuitionBase *) OpenLibrary ("intuition.library", 37)))
		ErrorPrc ("");

	if( FindSemaphore( "cybergraphics.library" ) )
		CyberGfxBase = OpenLibrary("cybergraphics.library",0);

	if (!(GfxBase = (struct GfxBase *) OpenLibrary ("graphics.library", 37)))
		ErrorPrc ("graphics.library V37");

	if (!(UtilityBase = OpenLibrary ("utility.library", 37)))
		ErrorPrc ("utility.library V37");

	DataTypesBase = OpenLibrary ("datatypes.library", 39);

	V39 = (BOOL) (GfxBase->LibNode.lib_Version >= 39);

	if (!MemoryInit ())
		ErrorPrc ("memory allocator");

	if (!(LayersBase = (struct Library *) OpenLibrary ("layers.library", 37)))
		ErrorPrc ("layers.library V37");

	if (!(GadToolsBase = (struct Library *) OpenLibrary ("gadtools.library", 37)))
		ErrorPrc ("gadtools.library V37");

	if (!(CxBase = OpenLibrary ("commodities.library", 37L)))
		ErrorPrc ("commodities.library V37");

	if (!(KeymapBase = OpenLibrary ("keymap.library", 37L)))
		ErrorPrc ("keymap.library V37");

	if (!(GetPointerSemaphore = AllocVecPooled (sizeof (struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR)))
		ErrorPrc ("semaphore #1");

	InitSemaphore (GetPointerSemaphore);

	if (!(MenuActSemaphore = AllocVecPooled (sizeof (struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR)))
		ErrorPrc ("semaphore #2");

	InitSemaphore (MenuActSemaphore);

	if (!(RememberSemaphore = AllocVecPooled (sizeof (struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR)))
		ErrorPrc ("semaphore #3");

	if (!(IMsgReplyPort = CreateMsgPort ()))
		ErrorPrc ("IMsg reply port");

	if (!(TimeoutPort = CreateMsgPort ()))
		ErrorPrc ("timeout reply port");

	if (!(TimeoutRequest = (struct timerequest *) CreateIORequest (TimeoutPort, sizeof (struct timerequest))))
		ErrorPrc ("timeout request");

	if (OpenDevice (TIMERNAME, UNIT_VBLANK, (struct IORequest *) TimeoutRequest, NULL))
		ErrorPrc ("timer.device");

	InitSemaphore (RememberSemaphore);


	Ok = (NULL != (TimerPort = CreateMsgPort ()));

	if (Ok)
		Ok = (NULL != (TimerIO = (struct timerequest *)CreateIORequest (TimerPort, sizeof (struct timerequest))));

	if (Ok)
		Ok = (NULL == OpenDevice ((char *) TIMERNAME, UNIT_VBLANK, TimerIO, 0));

	if (!Ok)
		ErrorPrc ("timer.device");

	TimerBase = (struct Library *) TimerIO->tr_node.io_Device;

	Ok = (NULL != (InputPort = CreateMsgPort ()));

	if (Ok)
		Ok = (NULL != (InputIO = (struct IOStdReq *)CreateIORequest (InputPort, sizeof (struct IOStdReq))));

	if (Ok)
		Ok = (NULL == OpenDevice ((char *) INPUTNAME, NULL, InputIO, 0));
	if (!Ok)
		ErrorPrc ("input.device");

	InputBase = (struct Library *) InputIO->io_Device;

/*	if (!(PeekQualifier () & IEQUALIFIER_LALT))*/
/*		StartHihoTask ();*/

	CheckArguments ((argc == 0) ? ((struct WBStartup *)argv) : NULL);

	AktPrefs = DefaultPrefs;

	if (!LoadPrefs ("ENV:MagicMenu.prefs", FALSE))
		Cx_Popup = TRUE;

	if (!(MMMsgPort = CreateMsgPort ()))
		ErrorPrc ("MagicMenu MsgPort");

	MMMsgPort->mp_Node.ln_Name = MMPORT_NAME;
	AddPort (MMMsgPort);

	if (!(CxMsgPort = CreateMsgPort ()))
		ErrorPrc ("CxMsgPort");

	CxSignalMask = 1l << CxMsgPort->mp_SigBit;

	StripRPort = NULL;
	MenuStripSwapped = FALSE;
	BoxRPort = NULL;
	MenuBoxSwapped = FALSE;
	SubBoxRPort = NULL;
	MenuSubBoxSwapped = FALSE;
	CxChanged = FALSE;
	TickSigNum = -1;

	LoadBackFill( AktPrefs.mmp_BackFill );
#ifdef AMITHLON
	if (check_emulation() != 0L)
	{
		if( x86elf = open_elf("PROGDIR:mmx86.elf") )
			BlurAndTintPixelBufferX86 = find_slowcall( x86elf, "BlurAndTintPixelBufferX86" );
/*		else Complain( "Can't load x86 support code" );	*/
	}
#endif
	ObtainSemaphore (RememberSemaphore);

	if (InstallPatches ())
		LibPatches = TRUE;
	else
	{
		ReleaseSemaphore (RememberSemaphore);
		ErrorPrc ("system patches");
	}

	if (!MakeGlobalRemember ())
	{
		ReleaseSemaphore (RememberSemaphore);
		ErrorPrc ("menu cache");
	}

	ReleaseSemaphore (RememberSemaphore);

	SPrintf (TitleText, "MagicMenu %ld.%ld (%s)", VERSION, REVISION, DATE);
	NewBroker.nb_Title = TitleText;

	NewBroker.nb_Port = CxMsgPort;
	NewBroker.nb_Pri = Cx_Pri;

	if (!(Broker = (LONG *) CxBroker (&NewBroker, &error)))
	{
		if (error != CBERR_DUP)
			ErrorPrc ("input broker");
		CloseAll ();
		return;
	}

	if (!(MouseFilter = CxFilter (NULL)))
		ErrorPrc ("mouse filter");
	SetFilterIX (MouseFilter, &MouseIX);
	AttachCxObj (Broker, MouseFilter);

	if (!(MouseSender = CxSender (CxMsgPort, EVT_MOUSEMENU)))
		ErrorPrc ("mouse sender");
	AttachCxObj (MouseFilter, MouseSender);

	if (!(MouseTransl = CxTranslate (NULL)))
		ErrorPrc ("mouse translator");
	AttachCxObj (MouseFilter, MouseTransl);


	if(AktPrefs.mmp_KCKeyStr[0])
	{
		if (!(KbdFilter = HotKey (AktPrefs.mmp_KCKeyStr, CxMsgPort, EVT_KBDMENU)))
			Complain (GetString (MSG_KEYBOARD_HOTKEY_SEQUENCE_INVALID_TXT));
		else
			AttachCxObj (Broker, KbdFilter);
	}

	if (!(StdKbdFilter = CxFilter (NULL)))
		ErrorPrc ("RAmiga - RCommand keyboard filter");
	SetFilterIX (StdKbdFilter, &StdKbdIX);

	if (!(StdKbdSender = CxSender (CxMsgPort, EVT_KBDMENU)))
		ErrorPrc ("RAmiga - RCommand keyboard sender");
	AttachCxObj (StdKbdFilter, StdKbdSender);

	if (!(StdKbdTransl = CxTranslate (NULL)))
		ErrorPrc ("RAmiga - RCommand keyboard translator");
	AttachCxObj (StdKbdFilter, StdKbdTransl);

	AttachCxObj (Broker, StdKbdFilter);

	ActivateCxObj(StdKbdFilter, (AktPrefs.mmp_KCRAltRCommand != FALSE) && (AktPrefs.mmp_KCEnabled != FALSE));

	if(KbdFilter)
		ActivateCxObj(KbdFilter, AktPrefs.mmp_KCEnabled != FALSE);

	if(Cx_Popkey[0] != '\0')
	{
		if (!(PrefsFilter = HotKey (Cx_Popkey, CxMsgPort, EVT_POPPREFS)))
			Complain (GetString (MSG_POPUP_HOTKEY_SEQUENCE_INVALID_TXT));
		else
			AttachCxObj (Broker, PrefsFilter);
	}

	if (!(SetupMenuActiveData ()))
		ErrorPrc ("active menu data");

	Activate ();

	if (Cx_Popup)
		StartPrefs ();

	ProcessCommodity ();

	LEAVE();

	CloseAll ();

	return (0);
}
