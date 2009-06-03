struct IFFHandle *
OpenImageFile(UBYTE *Name)
{
	struct IFFHandle *Handle;

	if(Handle = AllocIFF())
	{
		if(Handle -> iff_Stream = Open(Name,MODE_OLDFILE))
		{
			InitIFFasDOS(Handle);

			if(!OpenIFF(Handle,IFFF_READ))
				return(Handle);

			Close(Handle -> iff_Stream);
		}

		FreeIFF(Handle);
	}

	return(NULL);
}

VOID
CloseImageFile(struct IFFHandle *Handle)
{
	CloseIFF(Handle);

	Close(Handle -> iff_Stream);

	FreeIFF(Handle);
}

BYTE
ReadImageBody(struct IFFHandle *Handle,struct BitMap *BitMap,struct BitMapHeader *BitMapHeader)
{
	USHORT			 i,j,k;
	UBYTE			 Value,SoFar,Compr,Depth;
	BYTE			 ChkVal;
	LONG			 Height,Width;
	PLANEPTR		 Planes[9];

	BYTE			*AuxBuff,*AuxBuff2;

	struct ContextNode	*ContextNode;

	if(ContextNode = CurrentChunk(Handle))
	{
		Width	= BitMap -> BytesPerRow;
		Height	= BitMap -> Rows;
		Depth	= BitMap -> Depth;
		Compr	= BitMapHeader -> compression;

		if(Compr > 1)
			return(FALSE);

		for(i = 0 ; i < Depth ; i++)
			Planes[i] = BitMap -> Planes[i];

		for(i = Depth ; i < 9 ; i++)
			Planes[i] = NULL;

		if(BitMapHeader -> masking == 1)
			Depth++;

		if(AuxBuff = (BYTE *)AllocVec(ContextNode -> cn_Size,MEMF_PUBLIC))
		{
			if(ReadChunkBytes(Handle,AuxBuff,ContextNode -> cn_Size) == ContextNode -> cn_Size)
			{
				AuxBuff2 = AuxBuff;

				if(Compr == 0)
				{
					for(k = 0 ; k < Height ; k++)
					{
						for(j = 0 ; j < Depth ; j++)
						{
							if(Planes[j])
							{
								CopyMem(AuxBuff,Planes[j],Width);
								Planes[j] += Width;
							}

							AuxBuff += Width;
						}
					}
				}

				if(Compr == 1)
				{
					for(k = 0 ; k < Height ; k++)
					{
						for(j = 0 ; j < Depth ; j++)
						{
							for(SoFar = 0 ; SoFar < Width ; )
							{
								ChkVal = *AuxBuff;
								AuxBuff++;

								if(ChkVal > 0)
								{
									if(Planes[j])
									{
										CopyMem(AuxBuff,Planes[j],ChkVal + 1);

										Planes[j] += ChkVal + 1;
									}

									AuxBuff += ChkVal + 1;

									SoFar += ChkVal + 1;
								}
								else
								{
									if(ChkVal != -128)
									{
										Value = *AuxBuff;
										AuxBuff++;

										for(i = 0 ; i <= -ChkVal ; i++)
										{
											if(Planes[j])
												*Planes[j]++ = Value;

											SoFar++;
										}
									}
								}
							}
						}
					}
				}

				FreeVec(AuxBuff2);

				return(TRUE);
			}

			FreeVec(AuxBuff);
		}
	}

	return(FALSE);
}

BYTE
ReadImageHeader(struct IFFHandle *Handle,ULONG *ViewModes,BYTE *NumCols,UWORD *Colours,struct BitMapHeader *BitMapHeader)
{
	STATIC ULONG ImageProps[] =
	{
		'ILBM','BMHD',
		'ILBM','CMAP',
		'ILBM','CAMG'
	};

	struct StoredProperty *Prop;

	if(!PropChunks(Handle,&ImageProps[0],4))
	{
		if(!StopChunk(Handle,'ILBM','BODY'))
		{
			if(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				if(Prop = FindProp(Handle,'ILBM','BMHD'))
				{
					CopyMem(Prop -> sp_Data,BitMapHeader,sizeof(struct BitMapHeader));

					if(BitMapHeader -> nPlanes > 0 && BitMapHeader -> nPlanes <= 8)
					{
						if(ViewModes)
						{
							extern struct GfxBase *GfxBase;

							*ViewModes = NULL;

							if(BitMapHeader -> pageHeight > ((GfxBase -> DisplayFlags & PAL) ? 256 : 200))
								*ViewModes |= LACE;

							if(BitMapHeader -> pageWidth >= 640)
								*ViewModes |= HIRES;

							if(BitMapHeader -> nPlanes == 6)
								*ViewModes |= HAM;

							if(Prop = FindProp(Handle,'ILBM','CAMG'))
								*ViewModes = *(ULONG *)Prop -> sp_Data;

							*ViewModes &= ~(SPRITES | VP_HIDE | GENLOCK_AUDIO | GENLOCK_VIDEO);

							if(ModeNotAvailable(*ViewModes))
								*ViewModes &= 0xFFFF;

							if(ModeNotAvailable(*ViewModes))
								return(FALSE);
						}

						if(NumCols && Colours)
						{
							UBYTE *ColourArray;
							SHORT  i,R,G,B;

							if(!(Prop = FindProp(Handle,'ILBM','CMAP')))
								return(FALSE);

							ColourArray = (UBYTE *)Prop -> sp_Data;

							*NumCols = Prop -> sp_Size / 3;

							for(i = 0 ; i < *NumCols ; i++)
							{
								R = ColourArray[i * 3 + 0] >> 4;
								G = ColourArray[i * 3 + 1] >> 4;
								B = ColourArray[i * 3 + 2] >> 4;

								Colours[i] = (R << 8) | (G << 4) | (B);
							}
						}

						return(TRUE);
					}
				}
			}
		}
	}

	return(FALSE);
}

VOID
DeleteBitMap(struct BitMap *BitMap)
{
	FreeVec(BitMap -> Planes[0]);
	FreeVec(BitMap);
}

struct BitMap *
CreateBitMap(BYTE Depth,UWORD Width,UWORD Height)
{
	struct BitMap	*BitMap;
	BYTE		 i;

	if(BitMap = (struct BitMap *)AllocVec(sizeof(struct BitMap),MEMF_PUBLIC|MEMF_CLEAR))
	{
		InitBitMap(BitMap,Depth,Width,Height);

		if(!(BitMap -> Planes[0] = AllocVec(BitMap -> BytesPerRow * BitMap -> Rows * BitMap -> Depth,MEMF_CHIP||MEMF_CLEAR)))
		{
			FreeVec(BitMap);

			return(NULL);
		}
		else
		{
			for(i = 1 ; i < BitMap -> Depth ; i++)
				BitMap -> Planes[i] = &BitMap -> Planes[i - 1][BitMap -> BytesPerRow * BitMap -> Rows];

			return(BitMap);
		}
	}
	else
		return(NULL);
}
