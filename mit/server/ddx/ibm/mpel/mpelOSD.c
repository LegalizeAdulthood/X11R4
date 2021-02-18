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
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelOSD.c,v 6.3 88/11/10 16:47:28 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelOSD.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelOSD.c,v 6.3 88/11/10 16:47:28 paul Exp $" ;
#endif

#if defined(AIXrt)
#include "OSio.h"
#include <sys/hft.h>
#include "hftUtils.h"
extern int hftQFD ;
#else
#if defined(BSDrt)
#include <fcntl.h>
#include "bsdIO.h"
#else
	******** ERROR ********
#endif
#endif

#include "ibmTrace.h"

extern void ibmInfoMsg() ;

/***==================================================================***/

#if defined(AIXrt)

int
mpelProbe()
{
register int fd ;

    TRACE( ( "mpelProbe()\n" ) ) ;

    if ( ( fd = hftProbe( HFT_MEGAPEL_ID ) ) < 0 ) {
	ibmInfoMsg( "Didn't find an IBM 5081 Graphics Adapter\n" ) ;
	return -1 ;
    }
    else  {
	ibmInfoMsg( "Found an IBM 5081 Graphics Adapter\n" ) ;
	return fd ;
    }
    /*NOTREACHED*/
}

#else
#if defined(BSDrt)
int
mpelProbe()
{
register int fd ;
struct screen_control scr ;

    TRACE( ( "mpelProbe()\n" ) ) ;

    if ( ( fd = open( "/dev/mpel", O_RDWR | O_NDELAY | O_EXCL ) ) < 0 )
	return -1 ;

    scr.device= CONS_MPEL ;
    if ( ioctl( fd, SCRIOCGETF, &scr ) < 0 )  {
	ErrorF( "mpelProbe: ioctl SCRIOCGETF failed\n" ) ;
	(void) close( fd ) ;
	return -1 ;
    }
    if ( scr.switches & CONSDEV_PRESENT ) {
	ibmInfoMsg( "Found a megapel\n" ) ;
	return fd ;
    }
    else {
	(void) close( fd ) ;
	return -1 ;
    }
/*NOTREACHED*/
}

#else
	******** ERROR ********
#endif
#endif
