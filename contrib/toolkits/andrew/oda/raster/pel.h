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
 *  File:   pel.h
 *  Author: Andrew Werth
 *
 *  Header info for pel setting, clearing, and status routines.
 *
 */

#ifndef TK_INCLUDED_pel
#define TK_INCLUDED_pel

#ifdef TK_TKI_pel
#define TK_pel
#else
#ifdef TK_IMPL_pel
#define	TK_pel
#else
#define AP_pel
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_pel.

*/



/* 
    EVERYONE SEES THIS
*/

#define BIT_CLEARED	(0)
#define BIT_SET		(1)

#define BIT_WHITE	(0)
#define BIT_BLACK	(1)

/* All rasters are currently bitmaps, so pel type == PEL_BITS */

#define PEL_BITS	(0)
#define PEL_RGB		(1)
#define PEL_HSI		(2)
#define PEL_CMAP	(3)

typedef struct {
    TAG_type	pel_tag;

    union {
	struct {		    /* Values for R, G, and B	*/
	    INT_type	red;
	    INT_type	green;
	    INT_type	blue;
	} RGB;
	struct {		    /* Values for H, S, and I	*/
	    INT_type	hue;	    
	    INT_type	saturation;
	    INT_type	intensity;
	} HSI;
	BYTE_type   bit;	    /* Bitmap, either on or off */
	INT_type    colormap;	    /* Index into a colormap	*/
    } pel;
} PEL_type;



INT_type    AssignBit();
VALUE_type  GetBit();

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_pel

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_pel


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif


/*  End of file   */
