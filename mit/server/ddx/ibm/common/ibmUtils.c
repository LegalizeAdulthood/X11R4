/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/common/RCS/ibmUtils.c,v 30.1 89/02/13 20:58:14 paul Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/common/RCS/ibmUtils.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/common/RCS/ibmUtils.c,v 30.1 89/02/13 20:58:14 paul Exp $" ;
#endif

#include "OSio.h"

#include "ibmTrace.h"

int ibmTrace ;

#ifdef NOT_X
#define ErrorF printf
#endif

/***==================================================================***/

int ibmQuietFlag = 0 ;

/*VARARGS1*/
/*ARGSUSED*/
void
ibmInfoMsg( str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 )
char *str ;
{
if ( !ibmQuietFlag )
	(void) ErrorF( str, arg1, arg2, arg3, arg4, arg5,
		       arg6, arg7, arg8, arg9 ) ;
return ;
}

/***==================================================================***/

void
ddxGiveUp()
{
    TRACE( ( "ddxGiveUp()" ) ) ;
    OS_CapsLockFeedback( 0 ) ;
    OS_GiveUp() ;
    return ;
}

/***==================================================================***/

void
AbortDDX()
{
    TRACE( ( "AbortDDX()" ) ) ;
#ifdef IBM_PLX
    p_coldstart();
#endif
    OS_CapsLockFeedback( 0 ) ;
    OS_Abort() ;
    return ;
}

/***==================================================================***/

/*
 * abort the server when someone calls through a bogus pointer, which
 * was usually initialized by the structure assignment method (xxxData.c)
 */
void
ibmAbort()
{
extern void abort() ;

	ErrorF(
   "ibmAbort: invalid call though function pointer\n... FATAL exiting...\n" ) ;
	abort() ;
	/*NOTREACHED*/
}

/***==================================================================***/

#ifdef IBM_SPECIAL_MALLOC
#include <stdio.h>
#include <signal.h>

int ibmShouldDumpArena = 0 ;
static char *ibmArenaFile = 0 ;

static
ibmMarkDumpArena()
{
    ibmShouldDumpArena = 1 ;
    return 0;
}

ibmDumpArena()
{
FILE  *mfil;
static	char	fileName[100];
static	int	dumpNum= 0;

   (void) sprintf( fileName, ibmArenaFile, dumpNum++ ) ;
   mfil = fopen( fileName, "a" ) ;
   if ( !mfil ) {
	ErrorF( "Couldn't open %s to dump arena, ignored\n", fileName ) ;
	return 0 ;
   }
   else {
	ErrorF( "Dumping malloc arena to %s\n", fileName ) ;
	plumber( mfil ) ;
	(void) fflush( mfil ) ;
	(void) fclose( mfil ) ;
   }
   ibmShouldDumpArena = 0 ;
   return 1 ;
}

ibmNoteHit()
{
static int old= 4;

   ErrorF("received SIGTERM\n");
   old= SetMallocCheckLevel(old);
   return 1 ;
}

int
ibmSetupPlumber( name )
register char *name ;
{
extern exit() ;

    ibmInfoMsg( "Setting up plumber to dump to %s\n", name ) ;
    (void) unlink( ibmArenaFile = name ) ;
    (void) signal( SIGUSR1, ibmMarkDumpArena ) ;
#if !defined(AIXrt)
    (void) signal( SIGEMT, ibmDumpArena ) ;
#endif
    (void) signal( SIGTERM, ibmNoteHit ) ;
    (void) signal( SIGUSR2, exit ) ;
    return 1 ;
}

#endif /* IBM_SPECIAL_MALLOC */

/***==================================================================***/
/* Debugging Aids */
/* print_pattern() & print_event() are just for debugging */
#if defined(DEBUG) && !defined(NDEBUG)

/* these don't work with AIX compiler */
/* #if !defined(AIXrt) && !defined(AIX386) && !defined(IBM_OS_HAS_HFT) */
#if !defined(AIXrt) 
#define PUT_BIT(d,b)   (((d)&(((unsigned)1)<<(b)))?ErrorF("*"):ErrorF("."))

void
print_pattern(width,height,data)
int	width,height;
char	*data;
{
char	*tmp=data;
int	row,bit;
unsigned data_byte;
int	bits_left;

    TRACE(("print_pattern( width= %d, height= %d, data= 0x%x )\n",
		width,height,data));

    for (row=0;row<height;row++) {
	ErrorF("0x");
	for (bit=0;bit<(width+7)/8;bit++) {
	   ErrorF("%02x",*tmp++);
	}
	ErrorF("\n");
    }
    for (row=0;row<height;row++) {
	for (bits_left=width;bits_left>0;bits_left-=8) {
	    data_byte= *data++;
	    for (bit=7;bit>=(bits_left>8?0:8-bits_left);bit--) {
		PUT_BIT(data_byte,bit);
	    }
	}
	ErrorF("\n");
    }
    return;
}

/***==================================================================***/

#ifdef IBM_OS_HAS_X_QUEUE

/*
#include "X.h"
#include "cursorstr.h"
#include "miscstruct.h"
#include "scrnintstr.h"

#include "ibmScreen.h"
*/

#include "OSio.h"

/* SORRY, This just doesn't work anymore ( and I don't know why ! ) *
void
print_event(xE)
XEvent	*xE;
{

    TRACE(("print_event( xE= 0x%x )\n",xE));

    ErrorF("Event(%d,%d): ",xE->xe_x,xE->xe_y);
    switch (xE->xe_device) {
	case XE_MOUSE:		ErrorF("mouse "); break;
	case XE_DKB:		ErrorF("keyboard "); break;
	case XE_TABLET:		ErrorF("tablet "); break;
	case XE_AUX:		ErrorF("aux "); break;
	case XE_CONSOLE:	ErrorF("console "); break;
	default:		ErrorF("unknown(%d) ",xE->xe_device); break;
    }
    if (xE->xe_type==XE_BUTTON) {
	ErrorF("button ");
	if	(xE->xe_direction==XE_KBTUP)	ErrorF("up ");
	else if	(xE->xe_direction==XE_KBTDOWN)	ErrorF("down ");
	else if	(xE->xe_direction==XE_KBTRAW)	ErrorF("raw ");
	else 			ErrorF("unknown(%d) ",xE->xe_direction);
	ErrorF("(key= %d)",xE->xe_key);
    }
    else if (xE->xe_type==XE_MMOTION)	ErrorF("MMOTION");
    else if (xE->xe_type==XE_TMOTION)	ErrorF("TMOTION");
    ErrorF("\n");
    return;
}
 * SORRY, This just doesn't work anymore ( and I don't know why ! ) */

#endif /* IBM_OS_HAS_X_QUEUE */

#endif /* !AIXrt */

#endif /* Debugging Aids */
