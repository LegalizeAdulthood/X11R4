/* ppm.h - header file for libppm portable pixelmap library
*/

#ifndef _PPM_H_
#define _PPM_H_

#include "pgm.h"

typedef gray pixval;

#ifdef PPM_PACKCOLORS

#define PPM_MAXMAXVAL 1023
typedef unsigned long pixel;
#define PPM_GETR(p) (((p) & 0x3ff00000) >> 20)
#define PPM_GETG(p) (((p) & 0xffc00) >> 10)
#define PPM_GETB(p) ((p) & 0x3ff)
#define PPM_ASSIGN(p,red,grn,blu) (p) = ((pixel) (red) << 20) | ((pixel) (grn) << 10) | (pixel) (blu)
#define PPM_EQUAL(p,q) ((p) == (q))

#else /*PPM_PACKCOLORS*/

#define PPM_MAXMAXVAL PGM_MAXMAXVAL
typedef struct
    {
    pixval r, g, b;
    } pixel;
#define PPM_GETR(p) ((p).r)
#define PPM_GETG(p) ((p).g)
#define PPM_GETB(p) ((p).b)
#define PPM_ASSIGN(p,red,grn,blu) do { (p).r = (red); (p).g = (grn); (p).b = (blu); } while ( 0 )
#define PPM_EQUAL(p,q) ( (p).r == (q).r && (p).g == (q).g && (p).b == (q).b )

#endif /*PPM_PACKCOLORS*/

/* Declarations of routines. */

#define ppm_allocarray( cols, rows ) ((pixel **) pm_allocarray( cols, rows, sizeof(pixel) ))
#define ppm_allocrow( cols ) ((pixel *) pm_allocrow( cols, sizeof(pixel) ))
#define ppm_freearray( pixelrow, rows ) pm_freearray( pixelrow, rows )
#define ppm_freerow( pixelrow ) pm_freerow( pixelrow )

pixel **ppm_readppm( /* FILE *file, int *colsP, int *rowsP, pixval *maxvalP */ );
void ppm_readppminit( /* FILE *file, int *colsP, int *rowsP, pixval *maxvalP, int *formatP */ );
void ppm_readppmrow( /* FILE *file, pixel *pixelrow, int cols, pixval maxval, int format */ );

void ppm_writeppm( /* FILE *file, pixel **pixels, int cols, int rows, pixval maxval */ );
void ppm_writeppminit( /* FILE *file, int cols, int rows, pixval maxval */ );
void ppm_writeppmrow( /* FILE *file, pixel *pixelrow, int cols, pixval maxval */ );

pixel ppm_backgroundpixel( /* pixel **pixels, int cols, int rows */ );

extern pixval ppm_pbmmaxval;
/* This is the maxval used when a PPM program reads a PBM file.  Normally
** it is 1; however, for some programs, a larger value gives better results
*/

/* Color scaling macro -- to make writing ppmtowhatever easier. */

#define PPM_CSCALE(newp,p,oldmaxval,newmaxval) \
    PPM_ASSIGN( (newp), \
	        (int) PPM_GETR(p) * (newmaxval) / (oldmaxval), \
	        (int) PPM_GETG(p) * (newmaxval) / (oldmaxval), \
	        (int) PPM_GETB(p) * (newmaxval) / (oldmaxval) )

/* Luminance macro. */

#define PPM_LUMIN(p) ( 0.299 * PPM_GETR(p) + 0.587 * PPM_GETG(p) + 0.114 * PPM_GETB(p) )

#endif /*_PPM_H_*/
