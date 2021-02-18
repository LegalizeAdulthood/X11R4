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

/*
 *  File:   iofbm.c
 *  Author: Andrew Werth
 *
 *  Input/Output for the graphics formats supported by Michael
 *  Mauldin's Fuzzy Bitmap Package.
 *
 */

#define TK_TKI_mem
#define TK_TKI_pel
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_iofbm

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <pel.h>
#include <raster.h>
#include <rasio.h>
#include <iofbm.h>
#include <fbm.h>

#undef TK_GVARS_iofbm
#undef TK_IMPL_fbm
#undef TK_TKI_fbm
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_pel
#undef TK_TKI_mem

extern BYTE_type PEL_BitOnL[];

/*\
 *  FBM_READRASTER:  Read a raster through the FBM package.
\*/

RASTER_type *FBM_ReadRaster(GetByte, Parameter, Format)
    INT_type	    (*GetByte)();
    POINTER_type    Parameter;
    FORMAT_type	    *Format;
{
    RASTER_type	    *rval;
    RASTER_type	    *raster;	/* RGTK version of raster   */
    FBM		    image;	/* For read_gif, its image  */
    CHAR_type	    magic[32];	/* Buffer to hold maginc #  */
    INT_type	    mlen;	/* For read_gif, magic len  */
    INT_type	    result;	/* Does read_gif work?	    */
    INT_type	    x;
    INT_type	    y;
    BYTE_type	    *bmp;	/* Bit map pointer	    */
    BYTE_type	    byte;	/* Current octet	    */

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    mlen = 0;

    /* Execute the read using the Fuzzy Bitmap Package */

    switch(Format->format_tag) {
	case FORMAT_GIF:
	    result = read_gif(&image,GetByte,Parameter,magic,mlen);
	    break;
	case FORMAT_SUN:
	    result = read_sun(&image,GetByte,Parameter,magic,mlen);
	    break;
	case FORMAT_IFF:
	    result = read_iff(&image,GetByte,Parameter,magic,mlen);
	    break;
	default:
	    rval = RG_ERROR_RASTER;
	    RGError = RG_UNIMPL_err;
	    goto LEAVE;
    }

    /* Get the width and height from the FBM data structure */

    if (result == RG_ERROR_INT) {
	rval = RG_ERROR_RASTER;
	goto LEAVE;
    }

    raster = MakeRaster(image.hdr.cols,image.hdr.rows);

    for (y = 0; y < raster->height; y++) {
	byte = 0;
	bmp = &(image.bm[y * image.hdr.rowlen]);
	for (x = 0; x < raster->width; x++) {
	    if (*bmp == BLACK) {
		byte |= PEL_BitOnL[x & 7];
	    }
	    bmp++;
	    if ((x & 0x07) == 7) {
		raster->rows[y].swap_body.pels[x >> 3] = byte;
		byte = 0;
	    }
	}
	if ((x & 0x07) != 0) {
	    raster->rows[y].swap_body.pels[x >> 3] = byte;
	}

    }

    rval = raster;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  FBM_WRITERASTER:  Write a bitmap to output using the FBM
 *		      package.
\*/

INT_type FBM_WriteRaster(PutByte, Parameter, Format, raster)
    INT_type	    (*PutByte)();
    POINTER_type    Parameter;
    FORMAT_type	    *Format;
    RASTER_type	    *raster;
{
    INT_type	    rval;
    FBM		    image;
    INT_type	    y;
    INT_type	    x;
    BYTE_type	    *bmp;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    image.hdr.cols	= raster->width;
    image.hdr.rows	= raster->height;
    image.hdr.planes	= 1;
    image.hdr.bits	= 1;
    image.hdr.physbits	= 1;
    image.hdr.rowlen	= 16 * ((raster->width + 15) / 16);
    image.hdr.plnlen	= image.hdr.rowlen * raster->height;
    image.hdr.clrlen	= 2;
    image.hdr.aspect	= 1.0;
    image.hdr.title[0]  = '\0';
    image.hdr.credits[0]= '\0';

    if (Format->format_tag == FORMAT_GIF) {
	image.hdr.clrlen = 6; 
    }
    alloc_fbm(&image);

    for (y = 0; y < raster->height; y++) {
	bmp = &(image.bm[y * image.hdr.rowlen]);
	for (x = 0; x < raster->width; x++) {
	    *bmp = (GetBit(raster, x, y) > 0) ? 0 : 1;
	    bmp++;
	}	
    }

    switch (Format->format_tag) {
	case FORMAT_GIF:
	    image.hdr.clrlen = 6;
	    rval = write_gif(&image, PutByte, Parameter);
	    break;
	case FORMAT_IFF:
	    image.hdr.clrlen = 2;
	    rval = write_iff(&image, PutByte, Parameter);
	    break;
	case FORMAT_SUN:
	    rval = write_sun(&image, PutByte, Parameter);
	    break;
	default:
	    rval = RG_ERROR_INT;
	    RGError = RG_BADPARM_err;
	    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}



/*  End of file .c  */
