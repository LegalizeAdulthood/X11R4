/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

#ifndef TK_INCL_rgreal
#define TK_INCL_rgreal

/* File: rgdbg.c */

#ifndef USE_LONG_NAMES

#define Entering	    rgentr
#define Leaving		    rgleav

/* File: rginit.c */

#define InitRGToolKit	    rgintk
#define FinalRGToolKit	    rgfitk

#define INIT_RGParms	    rgvrgp
#define RGError		    rgverr

/* File: ioandy.c */

#define ANDY_ReadRaster	    rganrd
#define ANDY_WriteRaster    rganwr

/* File: iox.c */

#define X_ReadRaster	    rgxrd
#define X_WriteRaster	    rgxwr

/* File: iog3.c */

#define G3_ReadRaster	    rgg3rd
#define G3_DecodeOne	    rgg3do
#define G3_NextRun	    rgg3nr
#define G3_Traverse	    rgg3tr
#define G3_GetBit	    rgg3gb
#define G3_WriteRaster	    rgg3wr
#define G3_EncodeOne	    rgg3eo
#define G3_PutRunLength	    rgg3pr
#define G3_PutEOL	    rgg3pe
#define G4_WriteRaster	    rgg4wr

/* File: iog32.c */

#define G3_WriteBitmap2	    rgg3w2
#define G4_WriteBitmap	    rgg4wb

/* File: iog33.c */

#define G3_ReadBitmap2	    rgg3r2
#define	G3_PutBit	    rgg3pb

/* File: iog4.c */

#define G4_ReadRaster	    rgg4rd
#define G4_NextRun	    rgg4nr
#define G4_Traverse	    rgg4tr
#define G4_GetBit	    rgg4gb

/* File: iomacp.c */

#define MACP_ReadRaster	    rgmcrd
#define MACP_WriteRaster    rgmcwr

/* File: ioobm.c */

#define OBM_WriteRaster	    rgobwr
#define OBM_ReadRaster	    rgobrd

/* File: iopbm.c */

#define PBM_ReadRaster	    rgpbrd
#define PBM_WriteRaster	    rgpbwr

/* File: iops.c */

#define PS_WriteRaster	    rgpswr

/* File: ioetc.c */

#define ETC_ReadRaster	    rgetrd
#define ETC_GetNumber	    rgetgn
#define ETC_PutNumber	    rgetpn

#define ETC_MagicBuffer	    rgvemb
#define ETC_MagicSize	    rgvems

/* File: rglist.c */

#define LST_MakeList	    rglsml
#define LST_AddElement	    rglsad
#define LST_DeleteElement   rglsde
#define LST_MoveHead	    rglsmh
#define LST_NextElement	    rglsne
#define LST_FreeList	    rglsfl

/* File: rgmem.c */

#define MEM_Malloc	    rgmmma
#define MEM_Realloc	    rgmmre
#define MEM_Free	    rgmmfr

#define MEM_Alloced	    rgvmal

/* File: pel.c */

#define AssignBit	    rgpeab
#define AssignPel	    rgpeap
#define GetBit		    rgpegb
#define GetPel		    rgpegp

#define PEL_BitOffL	    rgvbfl
#define PEL_BitOffR	    rgvbfr
#define PEL_BitOnL	    rgvbnl
#define PEL_BitOnR	    rgvbnr

/* File: rasio.c */

#define ReadRaster	    rgrird
#define ReadMemRaster	    rgrirm
#define WriteRaster	    rgriwr
#define WriteMemRaster	    rgriwm

#define RIO_FromMemory	    rgvfrm

/* File: raster.c */

#define RAST_InitRasterList rgrain
#define RasterHeight	    rgrarh
#define RasterWidth	    rgrarw
#define RasterStatus	    rgrast
#define RasterPelFormat	    rgrapf
#define MakeRaster	    rgramr
#define DeleteRaster	    rgradr
#define RAST_FinalRasterList rgrafi
#define RAST_AllocRaster    rgraal
#define RAST_SwapAlloc	    rgrasa
#define RAST_SwapIn	    rgrasi
#define HoldRaster	    rgraho
#define ReleaseRaster	    rgrare
#define ExtractRaster	    rgraex
#define InsertRaster	    rgrais

/* File: iofbm.c */

#define FBM_ReadRaster	    rgfbrd
#define FBM_WriteRaster	    rgfbwr

/* File: fliff.c */

#define write_iff	    rgfiwr
#define read_iff	    rgfird

/* File: flgifw.c */

#define write_gif	    rgfgwr

/* File: flgifr.c */

#define read_gif	    rgfgrd

/* File: flgifc.c */

#define compress	    rgfgcm

/* File: flsun.c */

#define write_sun	    rgfswr
#define read_sun	    rgfsrd

/* File: flalfb.c */

#define alloc_fbm	    rgfaal
#define free_fbm	    rgfafr

/* File: flbyte.c */

#define fbmprintf	    rgfbpf
#define fbmread		    rgfbre
#define fbmwrite	    rgfbwi
#define get_long	    rgfbgl
#define get_short	    rgfbgs
#define put_long	    rgfbpl
#define put_short	    rgfbps
#define machine_byte_order  rgfbbo

#define fbm_GetByte	    rgvfgb
#define fbm_PutByte	    rgvfpb
#define fbm_Parameter	    rgvfpa

/* File: g3code.c */

#define BlackTreeTop	    rgvbtt
#define WhiteTreeTop	    rgvwtt
#define bl_make		    rgvblm
#define bl_term		    rgvblt
#define wt_make		    rgvwhm
#define wt_term		    rgvwht


/* file flgife */
#define	GIFEncode	    rggfen
#define	GIFNextPixel	    rggfnp

#endif


#endif

/* End of file rgreal.h */

