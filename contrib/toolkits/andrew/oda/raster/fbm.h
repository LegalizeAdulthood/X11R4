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

/*\
 *  This code has been taken from the Fuzzy Bitmap library, as
 *  noted below.  It has been modified in several ways:  Input/
 *  Output has been changed to match Raster Graphics Tool Kit
 *  specifications; debugging output has been removed; error
 *  handling has been modified for RGTK specs; procedures
 *  have been made static where appropriate; and minor changes
 *  to more closely resemble RGTK coding conventions.
 *
 *  Modifications by Andrew Werth.
 *
\*/


/*****************************************************************
 * fbm.h: FBM Library 0.9 (Beta Test)  07-Mar-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * fbm.h: Fuzzy Bitmap Definition
 *
 * USAGE
 *	# include <fbm.h>
 *
 * EDITLOG
 *	LastEditDate = Tue Mar  7 19:52:53 1989 - Michael Mauldin
 *	LastFileName = /usr2/mlm/src/misc/fbm/fbm.h
 *
 * HISTORY
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu.
 *
 * 20-Aug-88  Michael Mauldin (mlm) at Carnegie-Mellon University
 *	Created.
 *****************************************************************/

#ifndef TK_INCLUDED_fbm
#define	TK_INCLUDED_fbm

#ifdef TK_TKI_fbm
#define TK_fbm
#else
#ifdef TK_IMPL_fbm
#define	TK_fbm
#else
#define AP_fbm
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_fbm.

*/



/* 
    EVERYONE SEES THIS
*/



/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_fbm

#undef FILE
typedef int FILE;
#undef stdin
#define stdin ((FILE *) 0)

long get_long ();
int put_long ();
int get_short ();
int put_short ();
int machine_byte_order ();
int fbmwrite();
int fbmread();
int fbmprintf();

int read_iff();
int write_iff();
int read_sun();
int write_sun();
int read_gif();
int write_gif();

#define FBM_MAX_TITLE		80	/* For title and credits */

/* An FBM bitmap header in memory */
typedef struct fbm_hdr_struct {
	int	cols;			/* Width in pixels */
	int	rows;			/* Height in pixels */
	int	planes;			/* Depth (1 for B+W, 3 for RGB) */
	int	bits;			/* Bits per pixel */
	int	physbits;		/* Bits to store each pixel */
	int	rowlen;			/* Length of a row in bytes */
	int	plnlen;			/* Length of a plane in bytes */
	int	clrlen;			/* Length of color map */
	double	aspect;			/* ratio of Y to X of one pixel */
	char	title[FBM_MAX_TITLE];	/* Null terminated title */
	char	credits[FBM_MAX_TITLE];	/* Null terminated credits */
} FBMHDR;


/* FBM bitmap headers in files 
 * (null terminated 12 character ascii strings)
 */

typedef struct fbm_filehdr_struct {
	char	magic[8];		/* 2 bytes FBM_MAGIC number */
	char	cols[8];		/* Width in pixels */
	char	rows[8];		/* Height in pixels */
	char	planes[8];		/* Depth (1 for B+W, 3 for RGB) */
	char	bits[8];		/* Bits per pixel */
	char	physbits[8];		/* Bits to store each pixel */
	char	rowlen[12];		/* Length of a row in bytes */
	char	plnlen[12];		/* Length of a plane in bytes */
	char	clrlen[12];		/* Length of colormap in bytes */
	char	aspect[12];		/* ratio of Y to X of one pixel */
	char	title[FBM_MAX_TITLE];	/* Null terminated title */
	char	credits[FBM_MAX_TITLE];	/* Null terminated credits */
} FBMFILEHDR;

/* An FBM bitmap in memory */
typedef struct fbm_struct {
	FBMHDR hdr;			/* Bitmap header */
	unsigned char *cm;		/* Pointer to colormap */
	unsigned char *bm;		/* Pointer to raw bits */
} FBM;

#define NEXTMCH(F,S,L) (((L) > 0) ? ((L)--, *(S)++) : (*fbm_GetByte)(fbm_Parameter))

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_fbm

#define FBMGETC(x)	((*fbm_GetByte)(fbm_Parameter))
#define FBMPUTC(c,fp)	((*fbm_PutByte)(fbm_Parameter, c))


#undef BLACK
#undef WHITE
#define BLACK			0		/* For 8bit files */
#define WHITE			255		/* For 8bit files */
#define BYTE			256		/* For 8bit files */

#define BIG			1		/* msb first byte order */
#define LITTLE			0		/* lsb first byte order */

#define BYTESPERLINE		32		/* For PostScript output */

#define BLANKS		"                                             "
#define SKIPARG	while (*++(*argv)); --(*argv)
#define CLRARG		strncpy (*argv, BLANKS, strlen (*argv)); \
			while (*++(*argv)); --(*argv)

#define FMT_ATK	1	/*   Andrew toolkit raster format */
#define FMT_FACE	2	/*   Bennet Yee's 1bit Face format */
#define FMT_FBM	3	/* + Fuzzy bitmap format */
#define FMT_GIF	4	/*   Compuserve Graphics Interchange */
#define FMT_IFF	5	/*   Amiga Interchange Format File */
#define FMT_LEAF	6	/*   InterLeaf image format */
#define FMT_MCP	7	/*   Macpaint format */
#define FMT_PBM	8	/*   Poskanzer 1bit format */
#define FMT_PCX	9	/*   PCX format */
#define FMT_SUN	10	/* + Sun rasterfile */
#define FMT_TIFF	11	/*   Tagged IFF, Next, Macintosh */
#define FMT_X11	12	/*   X11 format */

#define FMTCHAR ".ABFGILMPZSTX"

#define DEF_8BIT	FMT_FBM
#define DEF_1BIT	FMT_SUN

#define FBM_MAGIC	"%bitmap"
#define BM_MAGIC	('!' << 8 | '!')
#define PCX_MAGIC	0xa
#define GIF_MAGIC	"GIF87a"
#define IFF_MAGIC	"FORM"
#define SUN_MAGIC	0x59a66a95



#ifdef TK_GVARS_flsun

typedef struct rasterfile {
	long ras_magic;
	long ras_width;
	long ras_height;
	long ras_depth;
	long ras_length;
	long ras_type;
	long ras_maptype;
	long ras_maplength;
} RASHDR;

#define RT_STANDARD	1
#define RMT_NONE	0
#define RMT_EQUAL_RGB	1
#define RMT_RAW	2

#define RED 0
#define GRN 1
#define BLU 2

#endif
#ifdef TK_GVARS_fliff
    static UnPackRow();
    static parse_form ();
    static read_iff_body ();
    static build_bmhd ();
    static build_cmap ();
    static build_body ();
#endif
#ifdef TK_GVARS_flgifr
    static ReadColorMap();
    static IgnoreExtention();
    static GetCode();
    static LWZReadByte();
    static ReadInterlaced();
    static FReadRaster();
#endif
#ifdef TK_GVARS_flgifw
    static GetGIFPixel();
#endif



#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif














/* Functions */
char *strncpy(), *malloc();
long time (), get_long ();
int get_short ();


