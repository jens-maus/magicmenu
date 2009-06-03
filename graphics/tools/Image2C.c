#define ARG_TEMPLATE	"File/A,C=Colours/K,B=BitMap/K,I=Image/K"

enum	{	ARG_FILE,ARG_COLOURS,ARG_BITMAP,ARG_IMAGE };

struct ExecBase		*SysBase;
struct DosLibrary	*DOSBase;
struct Library		*IFFParseBase;
struct GfxBase		*GfxBase;

struct IFFHandle *	OpenImageFile(UBYTE *Name);
VOID			CloseImageFile(struct IFFHandle *Handle);
BYTE			ReadImageBody(struct IFFHandle *Handle,struct BitMap *BitMap,struct BitMapHeader *BitMapHeader);
BYTE			ReadImageHeader(struct IFFHandle *Handle,ULONG *ViewModes,BYTE *NumCols,UWORD *Colours,struct BitMapHeader *BitMapHeader);
VOID			DeleteBitMap(struct BitMap *BitMap);
struct BitMap *		CreateBitMap(BYTE Depth,UWORD Width,UWORD Height);
BYTE			CheckAbort(VOID);
LONG __saveds		Main(VOID);

LONG __saveds
Main()
{
	LONG ReturnCode = RETURN_FAIL;

	SysBase = *(struct ExecBase **)4;

	if(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37))
	{
		if(IFFParseBase = OpenLibrary("iffparse.library",0))
		{
			if(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0))
			{
				struct RDArgs	*ArgsPtr;
				UBYTE		**Args;

				if(Args = (UBYTE **)AllocVec(sizeof(UBYTE *) * 4,MEMF_PUBLIC|MEMF_CLEAR))
				{
					if(ArgsPtr = ReadArgs(ARG_TEMPLATE,(APTR)Args,NULL))
					{
						if(Args[ARG_COLOURS] || Args[ARG_BITMAP] || Args[ARG_IMAGE])
						{
							struct BitMap		*BitMap;
							struct IFFHandle	*Handle;
							UWORD			 Colours[32];
							BYTE			 NumCols;
							struct BitMapHeader	 Header;

							if(Handle = OpenImageFile(Args[ARG_FILE]))
							{
								if(ReadImageHeader(Handle,NULL,&NumCols,Colours,&Header))
								{
									if(BitMap = CreateBitMap(Header . nPlanes,Header . w,Header . h))
									{
										if(ReadImageBody(Handle,BitMap,&Header))
										{
											if(Args[ARG_COLOURS])
											{
												SHORT i;

												Printf("UWORD %s[%ld] =\n{\n\t",Args[ARG_COLOURS],NumCols);

												for(i = 0 ; i < NumCols ; i++)
												{
													if(i == NumCols - 1)
														Printf("0x%04lx",Colours[i]);
													else
													{
														if(i && !(i % 4))
															Printf("\n\t");

														Printf("0x%04lx,",Colours[i]);
													}

													if(CheckAbort())
													{
														ReturnCode = RETURN_WARN;

														goto Quit;
													}
												}

												Printf("\n};\n\n");
											}

											if(Args[ARG_BITMAP])
											{
												LONG	 i,j,Count = (BitMap -> BytesPerRow * BitMap -> Rows) >> 1;
												UWORD	*Plane;

												Printf("UWORD %s[%ld] =\n{\n\t",Args[ARG_BITMAP],Count * BitMap -> Depth);

												for(i = 0 ; i < BitMap -> Depth ; i++)
												{
													Plane = (UWORD *)BitMap -> Planes[i];

													for(j = 0 ; j < Count ; j++)
													{
														if(j && !(j % 8))
															Printf("\n\t");

														if(i == BitMap -> Depth - 1 && j == Count - 1)
															Printf("0x%04lx",Plane[j]);
														else
															Printf("0x%04lx,",Plane[j]);

														if(CheckAbort())
														{
															ReturnCode = RETURN_WARN;

															goto Quit;
														}
													}

													if(i != BitMap -> Depth - 1)
														Printf("\n\n\t");
												}

												Printf("\n};\n\n");
											}

											if(Args[ARG_IMAGE])
											{
												Printf("struct Image %s =\n{\t0,0,\n",Args[ARG_IMAGE]);
												Printf("\t%ld,%ld,%ld,\n",Header . w,Header . h,Header . nPlanes);
												Printf("\t%s,\n",Args[ARG_BITMAP] ? Args[ARG_BITMAP] : "BitMap");
												Printf("\t0x%02lx,0x00,\n",(1 << Header . nPlanes) - 1);
												Printf("\tNULL\n};\n");
											}

											ReturnCode = RETURN_OK;
										}
										else
											Printf("Image2C: Couldn't read file \"%s\".\n",Args[ARG_FILE]);

Quit:										DeleteBitMap(BitMap);
									}
									else
										Printf("Image2C: Not enough memory for BitMap.\n");
								}
								else
									Printf("Image2C: Couldn't examine file \"%s\".\n",Args[ARG_FILE]);

								CloseImageFile(Handle);
							}
							else
								Printf("Image2C: Couldn't open file \"%s\".\n",Args[ARG_FILE]);
						}
						else
							Printf("Image2C: Required argument(s) missing!\n");

						FreeArgs(ArgsPtr);
					}
					else
						PrintFault(IoErr(),"Image2C");

					FreeVec(Args);
				}

				CloseLibrary(GfxBase);
			}
			else
				Printf("Image2C: Failed to open graphics.library\n");

			CloseLibrary(IFFParseBase);
		}
		else
			Printf("Image2C: Failed to open iffparse.library.\n");

		CloseLibrary(DOSBase);
	}

	return(ReturnCode);
}

BYTE
CheckAbort()
{
	if(SetSignal(0,0) & SIGBREAKF_CTRL_C)
	{
		SetSignal(0,SIGBREAKF_CTRL_C);

		Printf("\n***BREAK: Image2C\a\n");

		return(TRUE);
	}

	return(FALSE);
}
