/* libppm.h - internal header file for libppm portable pixmap library
*/

#ifndef _LIBPPM_H_
#define _LIBPPM_H_

/* Magic constants. */

#define PPM_MAGIC1 'P'
#define PPM_MAGIC2 '3'
#define RPPM_MAGIC2 '6'
#define PPM_FORMAT (PPM_MAGIC1 * 256 + PPM_MAGIC2)
#define RPPM_FORMAT (PPM_MAGIC1 * 256 + RPPM_MAGIC2)


/* And here are some routines internal to the ppm library. */

void ppm_readppminitrest( /* FILE *file, int *colsP, int *rowsP, int *maxvalP */ );

#endif /*_LIBPPM_H_*/
