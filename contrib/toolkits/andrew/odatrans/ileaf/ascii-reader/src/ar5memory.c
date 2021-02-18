
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	ar5memory.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original coding
 */

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"
static int malloc_slop  = 0;
static int realloc_slop = 0;
#define MSLOP (malloc_slop)
#define RSLOP (realloc_slop)
/**--------------------------------------------------------------------------**/
char *ArSafeMalloc(n)
  register long n;
{
  extern char *malloc();
  register char *cP = (char *)malloc(n+MSLOP);

#if defined(BT_DEBUG)	/** BT **/
    ArFprintf(stderr, "ArSafeMalloc(%d) ==> %#x\n", n+MSLOP, cP);
#endif	/** BT **/
    if (cP == NULL) { 
	fprintf(stderr, "malloc(%d) failed.\n", n+MSLOP); 
	exit( -1 ); 
    }
    return( cP );
}
/**--------------------------------------------------------------------------**/
char *ArSafeRealloc(cP, n)
  register char *cP;
  register unsigned n;
{
  extern char *realloc();
  register char *newcP = (char *)realloc(cP, n+RSLOP);

#if defined(BT_DEBUG)	/** BT **/
    ArFprintf(stderr, "ArSafeRealloc(%#x, %d) ==> %#x\n", cP, n+RSLOP, newcP);
#endif	/** BT **/
    if(newcP == NULL) {
	fprintf(stderr, "realloc(%#x,%d) failed.\n",cP,n+RSLOP);
	exit(-1);
    }
    return( newcP );
}
/**--------------------------------------------------------------------------**/
