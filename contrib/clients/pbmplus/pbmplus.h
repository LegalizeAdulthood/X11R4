/* pbmplus.h - header file for PBM, PGM, PPM, and PNM
*/

#ifndef _PBMPLUS_H_
#define _PBMPLUS_H_

#if ! ( defined(BSD) || defined(SYSV) )
/* CONFIGURE: If your system is >= 4.2BSD, set the BSD option; if you're a
** System V site, set the SYSV option.  If you're an ANSI C site, you're
** probably better off setting SYSV.  This mostly has to do with string
** functions.
*/
#define BSD
/* #define SYSV */
#endif

/* CONFIGURE: If you want to enable writing "raw" files, set this option.
** "Raw" files are smaller, and much faster to read and write, but you
** must have a filesystem that allows all 256 ASCII characters to be read
** and written.  Also, you will no longer be able to mail P?M files without 
** using uuencode or the equivalent.  Note that reading "raw" files works
** whether writing is enabled or not.
*/
#define PBMPLUS_RAWBITS

/* CONFIGURE: On some systems, the putc() macro is broken and will return
** EOF when you write out a 255.  For example, ULTRIX does this.  This
** only matters if you have defined RAWBITS.  To test whether your system
** is broken this way, go ahead and compile things with RAWBITS defined,
** and then try "pbmmake -b 8 1 > file".  If it works, fine.  If not,
** define BROKENPUTC1 and try again - if that works, good.  Otherwise,
** BROKENPUTC2 is guaranteed to work, although it's 1.9 times slower.
*/
/* #define PBMPLUS_BROKENPUTC1 */
/* #define PBMPLUS_BROKENPUTC2 */

#ifdef PBMPLUS_BROKENPUTC1
#undef putc
/* This is a fixed version of putc() that should work on most Unix systems. */
#define putc(x,p) (--(p)->_cnt>=0? ((int)(unsigned char)(*(p)->_ptr++=(unsigned char)(x))) : _flsbuf((unsigned char)(x),p))
#endif /*PBMPLUS_BROKENPUTC1*/
#ifdef PBMPLUS_BROKENPUTC2
#undef putc
/* For this one, putc() becomes a function, defined in pbm/libpbm1.c. */
#endif /*PBMPLUS_BROKENPUTC2*/

/* CONFIGURE: PGM can store gray values as either bytes or shorts.  For most
** applications, bytes will be big enough, and the memory savings can be
** substantial.  However, if you need more than 8 bits of resolution, then
** define this symbol.
**
** If you are not making PGM, you can ignore this.
*/
/* #define PGM_BIGGRAYS */

/* CONFIGURE: Normally, PPM handles a pixel as a struct of three grays.
** It can also be configured to pack the three values into a single longword,
** 10 bits each.  If you have configured PGM with the PGM_BIGGRAYS option
** (store grays as shorts), AND you don't need more than 10 bits for each
** color component, AND you care more about memory use than speed, then
** this option might be a win.  Under these circumstances it will make
** some of the programs use 1.5 times less space,  but all of the programs
** will run about 1.4 times slower.
**
** If you are not using PGM_BIGGRAYS, then this option is useless -- it
** doesn't save any space, but it still slows things down.
**
** If you are not making PPM, you can ignore this.
*/
/* #define PPM_PACKCOLORS */

/* CONFIGURE: uncomment this to enable debugging checks. */
/* #define DEBUG */

/* End of configurable definitions. */

/* Variable-sized arrays definitions. */

char **pm_allocarray( /* int cols, int rows, size */ );
char *pm_allocrow( /* int cols, size */);
void pm_freearray( /* char **its, int rows */ );
void pm_freerow( /* char *itrow */ );

/* Error handling definitions. */

extern char *pm_progname;	/* every main() must assign argv[0] to this */
void pm_message( /* char *fmt, arg, arg, arg, arg, arg */ );
void pm_error( /* char *fmt, arg, arg, arg, arg, arg */ ); /* doesn't return */
void pm_perror( /* char *reason */ );			/* doesn't return */
void pm_usage( /* char *usage */ );			/* doesn't return */

/* File open/close that handles "-" as stdin and checks errors. */

FILE *pm_openr( /* char *name */ );
void pm_close( /* FILE *f */ );

/* Endian I/O. */

int pm_readbigshort( /* FILE *in, short *sP */ );
int pm_writebigshort( /* FILE *out, short s */ );
int pm_readbiglong( /* FILE *in, long *lP */ );
int pm_writebiglong( /* FILE *out, long l */ );
int pm_readlittleshort( /* FILE *in, short *sP */ );
int pm_writelittleshort( /* FILE *out, short s */ );
int pm_readlittlelong( /* FILE *in, long *lP */ );
int pm_writelittlelong( /* FILE *out, long l */ );

#endif /*_PBMPLUS_H_*/
