/* pgm.h - header file for libpgm portable graymap library
*/

#ifndef _PGM_H_
#define _PGM_H_

#include "pbm.h"

#ifdef PGM_BIGGRAYS
typedef unsigned short gray;
#define PGM_MAXMAXVAL 65535
#else /*PGM_BIGGRAYS*/
typedef unsigned char gray;
#define PGM_MAXMAXVAL 255
#endif /*PGM_BIGGRAYS*/

/* Declarations of routines. */

#define pgm_allocarray( cols, rows ) ((gray **) pm_allocarray( cols, rows, sizeof(gray) ))
#define pgm_allocrow( cols ) ((gray *) pm_allocrow( cols, sizeof(gray) ))
#define pgm_freearray( grayrow, rows ) pm_freearray( grayrow, rows )
#define pgm_freerow( grayrow ) pm_freerow( grayrow )

gray **pgm_readpgm( /* FILE *file, int *colsP, int *rowsP, gray *maxvalP */ );
void pgm_readpgminit( /* FILE *file, int *colsP, int *rowsP, gray *maxvalP, int *formatP */ );
void pgm_readpgmrow( /* FILE *file, gray *grayrow, int cols, gray maxval, int format */ );

void pgm_writepgm( /* FILE *file, gray **grays, int cols, int rows, gray maxval */ );
void pgm_writepgminit( /* FILE *file, int cols, int rows, gray maxval */ );
void pgm_writepgmrow( /* FILE *file, gray *grayrow, int cols, gray maxval */ );

extern gray pgm_pbmmaxval;
/* This is the maxval used when a PGM program reads a PBM file.  Normally
** it is 1; however, for some programs, a larger value gives better results
*/

#endif /*_PGM_H_*/
