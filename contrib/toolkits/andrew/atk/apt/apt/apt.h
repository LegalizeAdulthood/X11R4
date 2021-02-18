/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidapt_h = "$Header $";
#endif

/*
    $Log:	apt.h,v $
 * Revision 1.2  89/08/03  16:01:56  gk5g
 * Replaced #include <sys/time.h> with andrewos.h.
 * 
 * Revision 1.1  89/04/28  17:45:42  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Apt Symbolics

MODULE	apt.h

VERSION	0.0

NOTICE	IBM Internal Use Only

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This are symbolics for apt-based programs.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  02/23/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  ASSERT( c, m )	    if ( !(c) )  printf( "Assert Failure: %s\n", "m" );

#ifndef DEBUG
char    debug;
#ifdef  DB
#define  IN( x )	    if ( debug ) printf( ">x\n" );
#define  OUT( x )	    if ( debug ) printf( "<x\n" );
#define  DEBUG( y )	    if ( debug ) printf( " %s\n", "y" );
#define  DEBUGc( y )	    if ( debug ) printf( " '%c'\n", "y" );
#define  DEBUGct( t, y )    if ( debug ) printf( " t: '%c'\n", y );
#define  DEBUGs( y )	    if ( debug ) printf( " '%s'\n", "y" );
#define  DEBUGst( t, y )    if ( debug ) printf( " t: '%s'\n", y );
#define  DEBUGd( y )	    if ( debug ) printf( " %d\n", y );
#define  DEBUGdt( t, y )    if ( debug ) printf( " t: %d\n", y );
#define  DEBUGlt( t, y )    if ( debug ) printf( " t: %D\n", y );
#define  DEBUGgt( t, y )    if ( debug ) printf( " t: %g\n", y );
#define  DEBUGx( y )	    if ( debug ) printf( " %x\n", y );
#define  DEBUGxt( t, y )    if ( debug ) printf( " t: %x\n", y );
#else
#define  IN( x )
#define  OUT( x )
#define  DEBUG( y )
#define  DEBUGc( y )
#define  DEBUGct( t, y )
#define  DEBUGs( y )
#define  DEBUGst( t, y )
#define  DEBUGd( y ) 
#define  DEBUGdt( t, y )
#define  DEBUGlt( t, y )
#define  DEBUGgt( t, y )
#define  DEBUGx( y ) 
#define  DEBUGxt( t, y )
#endif  DB
#endif  DEBUG

#define  ok	0
#define  failure 1

#define  yes	1
#define  no	0
#define  true	1
#define  false	0
#define  on	1
#define  off	0


#include  <andrewos.h>
static  struct timeval	  tod, tzone;
static  float	    	  elapsed_time;

#define  StartTimer	  {gettimeofday( &tod, &tzone );\
			  elapsed_time = (tod.tv_sec * 1000) + (tod.tv_usec / 1000);}
#define  PrintTimer(x)	  {gettimeofday( &tod, &tzone );\
			  elapsed_time = ((tod.tv_sec * 1000) + (tod.tv_usec / 1000)) - elapsed_time;\
			  printf( "%s -- %0.2g Seconds\n", "x", elapsed_time/1000 );}

