/* libpgm.h - internal header file for libpgm portable graymap library
*/

#ifndef _LIBPGM_H_
#define _LIBPGM_H_

/* Magic constants. */

#define PGM_MAGIC1 'P'
#define PGM_MAGIC2 '2'
#define RPGM_MAGIC2 '5'
#define PGM_FORMAT (PGM_MAGIC1 * 256 + PGM_MAGIC2)
#define RPGM_FORMAT (PGM_MAGIC1 * 256 + RPGM_MAGIC2)


/* And here are some routines internal to the pgm library. */

void pgm_readpgminitrest( /* FILE *file, int *colsP, int *rowsP, int *maxvalP */ );

#endif /*_LIBPGM_H_*/
