/*************************************************************************

	C Programmer's toolbox

	Module Name:	miscPack
	Purpose:	Utility routines for c programs

	Author:		Robert A Zimmermann, Carnegie -Mellon University
			Department of Electrical and Computer Engineering

	History:	Notes:

	23 Feb 84	Created module out of routines from the layout
			system, and other programs
	
	25 Jun 84	Added fopen_file and fatal_function

	13 Jul 84	Added flush_print, fopen_input, fopen_output,
			and check_switches
	
	02 Oct 84	Changed 'random' to be n_random so I could use
			4.2 utilities
	
 *************************************************************************

random numbers:

	<void>	initRandom( (long) seed )
				initialize the random number seed

	int	n_random( max )	Returns an integer in the range 0..max-1

utilities

	FILE *	fopen_file ( filename, filetype, filestring )
				Returns the filepointer

	<void>	flush_print( format, parameter )
				Does a fprintf( stderr ) and a fflush( stderr )

	<void>	swapPtrs( ptr1, ptr2 )
				*ptr1 = *ptr2 and
				*ptr2 = *ptr1

abort program:

	<void>	fatal_function( func )
				func will be performed by 'fatal'.

	<void>	fatal( code, string, parm )
				Abort program routine

/*
 * Constants and external routines
 */

#define  USE_BSD	0

#include <stdio.h>			/* UXIX standard I/O defs	*/

#include <stringPack.h>			/* from the toolbox		*/


/*
 * Random number generator
 */

static	long seed;

long	initRandom( newSeed )
long	newSeed;
    {
#if USE_BSD
    srandom( (int) seed );
#endif
    seed = newSeed;			/* Reasonably random number  */
    }


int	n_random (maximum)
int	maximum;
    {
    long temp;
#if USE_BSD
    long random();

    temp = random();
#else
    temp  = seed;			/* magic numbers generate random  */
    temp *= 2053;			/* numbers.  I forgot where the   */
    temp += 13849;			/* algorithm was originally found */
    seed  = temp;
    temp >>= 4;				/* drop un-random low-bits	  */
#endif
    if (maximum) return( abs(temp) % maximum );
		 return( 0 );
    }

/*
 * Abort program: fatal( EXIT_CODE, "STRING", parameter for string )
 */

static void (*last_function)() = NULL;

	fatal_function( func )
void	(*func)();
    {
    last_function = func;
    }


	fatal( code, str, parm )
int	code, parm;
STRING	str;
    {
    if (last_function != NULL)			/* If there is one...	   */
	(*last_function)();			/* Do termination function */

    fprintf( stderr, "Fatal error:  " );	/* Write closing message   */
    fprintf( stderr, str, parm );
    fprintf( stderr, "\n" );
    exit   ( code  );				/* exit program		   */
    }


/*
 * Swap two pointers.  This routine assumes the pointers are to char, but
 * unless someone really messes up the O/S, any pointers should do
 */

swapPtrs( ptr1, ptr2 )
char ** ptr1, ** ptr2;
    {
    register char * temp;

    temp  = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
    }

/*
 * Debug or pure noise printing
 */

/*VARARGS*/			/* may be called without parameter	*/

	flush_print( format, parameter )
STRING	format, parameter;
    {
    fprintf( stdout, format, parameter );
    fflush ( stdout );
    }

/*
 * Open a file for me
 */

FILE *	fopen_file( filename, filetype, filestring )
STRING	filename, * filetype, * filestring;
    {
    register FILE * temp;

    if ((temp = fopen( filename, filetype )) == NULL)
	printf ("Could not open %s '%s'\n", filestring, filename );
    else
	printf ("%s is '%s'\n", filestring, filename );
    return temp;
    }
