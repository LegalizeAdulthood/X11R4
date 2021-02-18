/* libpbm.h - internal header file for libpbm portable bitmap library
*/

#ifndef _LIBPBM_H_
#define _LIBPBM_H_

/* Magic constants. */

#define PBM_MAGIC1 'P'
#define PBM_MAGIC2 '1'
#define RPBM_MAGIC2 '4'
#define PBM_FORMAT (PBM_MAGIC1 * 256 + PBM_MAGIC2)
#define RPBM_FORMAT (PBM_MAGIC1 * 256 + RPBM_MAGIC2)

/* And here are some routines internal to the pbm library. */

char pbm_getc( /* FILE *file */ );
unsigned char pbm_getrawbyte( /* FILE *file */ );
int pbm_getint( /* FILE *file */ );

int pbm_readmagicnumber( /* FILE *file */ );

void pbm_readpbminitrest( /* FILE *file, int *colsP, int *rowsP */ );

#endif /*_LIBPBM_H_*/
