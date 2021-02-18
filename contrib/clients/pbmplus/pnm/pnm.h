/* pnm.h - header file for libpnm portable anymap library
*/

#ifndef _PNM_H_
#define _PNM_H_

#ifdef PPM

#include "ppm.h"
typedef pixel xel;
typedef pixval xelval;

#define PNM_GET1(p) PPM_GETB(p)
#define PNM_ASSIGN1(p,v) PPM_ASSIGN(p,0,0,v)
#define PNM_EQUAL(p,q) PPM_EQUAL(p,q)

#else /*PPM*/
# ifdef PGM

#include "pgm.h"
typedef gray xel;
typedef gray xelval;
#define PNM_GET1(p) (p)
#define PNM_ASSIGN1(p,v) (p) = (v)
#define PNM_EQUAL(p,q) ( (p) == (q) )

# else /*PGM*/
#  ifdef PBM

#include "pbm.h"
typedef bit xel;
typedef bit xelval;
#define PNM_GET1(p) (p)
#define PNM_ASSIGN1(p,v) (p) = (v)
#define PNM_EQUAL(p,q) ( (p) == (q) )

#  endif /*PBM*/
# endif /*PGM*/
#endif /*PPM*/

/* Declarations of routines. */

#define pnm_allocarray( cols, rows ) ((xel **) pm_allocarray( cols, rows, sizeof(xel) ))
#define pnm_allocrow( cols ) ((xel *) pm_allocrow( cols, sizeof(xel) ))
#define pnm_freearray( xelrow, rows ) pm_freearray( xelrow, rows )
#define pnm_freerow( xelrow ) pm_freerow( xelrow )

xel **pnm_readpnm( /* FILE *file, int *colsP, int *rowsP, xelval *maxvalP, int *formatP */ );
void pnm_readpnminit( /* FILE *file, int *colsP, int *rowsP, xelval *maxvalP, int *formatP */ );
void pnm_readpnmrow( /* FILE *file, xel *xelrow, int cols, xelval maxval, int format */ );

void pnm_writepnm( /* FILE *file, xel **xels, int cols, int rows, xelval maxval, int format */ );
void pnm_writepnminit( /* FILE *file, int cols, int rows, xelval maxval, int format */ );
void pnm_writepnmrow( /* FILE *file, xel *xelrow, int cols, xelval maxval, int format */ );

xel pnm_backgroundxel( /* xel **xels, int cols, int rows, xelval maxval, int format */ );
xel pnm_backgroundxelrow( /* xel *xelrow, int cols, xelval maxval, int format */ );
xel pnm_whitexel( /* xelval maxval, int format */ );
xel pnm_blackxel( /* xelval maxval, int format */ );
xel pnm_invertxel( /* xel x, xelval maxval, int format */ );
void pnm_promoteformat( /* xel **xels, int cols, int rows, xelval maxval, int format, xelval newmaxval, int newformat */ );
void pnm_promoteformatrow( /* xel *xelrow, int cols, xelval maxval, int format, xelval newmaxval, int newformat */ );

#endif /*_PNM_H_*/
