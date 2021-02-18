/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
#ifndef MPEL_H_SEEN
#define	MPEL_H_SEEN	1
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpel.h,v 1.5 89/01/20 13:18:22 paul Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpel.h,v $ */

#ifndef lint
static char *rcsidmpel = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpel.h,v 1.5 89/01/20 13:18:22 paul Exp $" ;
#endif

/*
 * definitions of mpel specific values
 */

#define	MPEL_WIDTH	1024
#define	MPEL_HEIGHT	1024
#define	MPELMAXPLANES	8
#define	MPEL_ALLPLANES	( ~ ( (~0) << MPELMAXPLANES ) ) /* 0xFF */

/* The mpel is available with either the 4 or 8 bit versions of the video dac */
#if !defined(DAC_MPEL16M)
#define MPEL_BITS_PER_RGB	4
#else
#define MPEL_BITS_PER_RGB	8
#endif

#define	MPEL_BLACK_PIXEL 0
#define	MPEL_WHITE_PIXEL 1

/* stuff for mpel line trash */
typedef struct {
	unsigned short int mask ;
	unsigned short int factor ;
} mpelUserLineStyle ;
typedef struct {
	unsigned int LineType ;
	mpelUserLineStyle LineStyle ;
} mpelPrivGC, *mpelPrivGCPtr ;
/* line styles */
#define MPEL_SOLIDLINE		1
#define MPEL_USERLINESTYLE	50
#define MPEL_USERLINESTYLE_CONT	150

extern int mfbGCPrivateIndex ;

#endif /* ndef MPEL_H_SEEN */
