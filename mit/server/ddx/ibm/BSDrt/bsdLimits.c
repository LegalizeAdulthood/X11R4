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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/BSDrt/RCS/bsdLimits.c,v 1.1 88/11/10 21:34:12 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/BSDrt/RCS/bsdLimits.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/BSDrt/RCS/bsdLimits.c,v 1.1 88/11/10 21:34:12 paul Exp $";
#endif

#include <sys/types.h>
#include <sys/time.h>		/* For resource-limit calls */
#include <sys/resource.h>	/* For resource-limit calls */

#include "ibmTrace.h"

/* Make sure the process doesn't bang against any resource limits !! */
/* Set each of CPU, FSIZE, DATA, STACK, CORE, & RSS to the maximum allowed !! */

void
setResourceLimits()
{
struct rlimit res ;

	TRACE( ( "setResourceLimits()\n" ) ) ;

if ( getrlimit( RLIMIT_CPU, &res ) )
	ErrorF( "Can't getrlimit( RLIMIT_CPU, &res ) !\n" ) ;
else {
	res.rlim_cur = res.rlim_max ;
	if ( setrlimit( RLIMIT_CPU, &res ) )
		ErrorF( "Can't setrlimit( RLIMIT_CPU, &res ) !\n" ) ;
}
if ( getrlimit( RLIMIT_FSIZE, &res ) )
	ErrorF( "Can't getrlimit( RLIMIT_FSIZE, &res ) !\n" ) ;
else {
	res.rlim_cur = res.rlim_max ;
	if ( setrlimit( RLIMIT_FSIZE, &res ) )
		ErrorF( "Can't setrlimit( RLIMIT_FSIZE, &res ) !\n" ) ;
}
if ( getrlimit( RLIMIT_DATA, &res ) )
	ErrorF( "Can't getrlimit( RLIMIT_DATA, &res ) !\n" ) ;
else {
	res.rlim_cur = res.rlim_max ;
	if ( setrlimit( RLIMIT_DATA, &res ) )
		ErrorF( "Can't setrlimit( RLIMIT_DATA, &res ) !\n" ) ;
}
if ( getrlimit( RLIMIT_STACK, &res ) )
	ErrorF( "Can't getrlimit( RLIMIT_STACK, &res ) !\n" ) ;
else {
	res.rlim_cur = res.rlim_max ;
	if ( setrlimit( RLIMIT_STACK, &res ) )
		ErrorF( "Can't setrlimit( RLIMIT_STACK, &res ) !\n" ) ;
}
if ( getrlimit( RLIMIT_CORE, &res ) )
	ErrorF( "Can't getrlimit( RLIMIT_CORE, &res ) !\n" ) ;
else {
	res.rlim_cur = res.rlim_max ;
	if ( setrlimit( RLIMIT_CORE, &res ) )
		ErrorF( "Can't setrlimit( RLIMIT_CORE, &res ) !\n" ) ;
}
if ( getrlimit( RLIMIT_RSS, &res ) )
	ErrorF( "Can't getrlimit( RLIMIT_RSS, &res ) !\n" ) ;
else {
	res.rlim_cur = res.rlim_max ;
	if ( setrlimit( RLIMIT_RSS, &res ) )
		ErrorF( "Can't setrlimit( RLIMIT_RSS, &res ) !\n" ) ;
}

return ;
}
