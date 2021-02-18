/***********************************************************
		Copyright IBM Corporation 1988

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
/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology

                     All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16RArea.c,v 1.2 89/08/28 10:24:12 hkbirke Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16RArea.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16RArea.c,v 1.2 89/08/28 10:24:12 hkbirke Exp $";
static char sccsid[] = "@(#)apa16rarea.c	3.1 88/09/22 09:31:08";
#endif

#include "X.h"
#include "scrnintstr.h"

#include "mfb.h"

#include "OScompiler.h"
#include "apa16Hdwr.h"

/* apa16 Replicate Area -- A Divide & Conquer Algorithm
 * an "apa16" Helper Function For Stipples And Tiling
 * P. Shupak 9/88
 */

void
apa16replicateArea( x, y, goalWidth, goalHeight, currentHoriz, currentVert )
register int x ;
register int y ;
register int goalWidth ;
register int goalHeight ;
register int currentHoriz ;
register int currentVert ;
{
register int offset ;
register unsigned long int cmd ;

	APA16_GET_CMD( ROP_RECT_COPY, GXcopy, cmd ) ;
	QUEUE_WAIT();

	for ( ;
	      ( offset = currentHoriz << 1 ) <= goalWidth ;
	      currentHoriz = offset ) {
		COPY_RECT( cmd,  
			x + offset, y + currentVert,
			x + currentHoriz, y + currentVert,
			currentHoriz, currentVert ) ;
	}
	if ( offset = goalWidth - currentHoriz ) {
		COPY_RECT( cmd,  
			x + goalWidth, y + currentVert,
			x + offset, y + currentVert,
			offset, currentVert ) ;
	}
	x += goalWidth ; /* All Done For x dimension */
	for ( ;
	      ( offset = currentVert << 1 ) <= goalHeight ;
	      currentVert = offset ) {
		COPY_RECT( cmd,  
			x, y + offset,
			x, y + currentVert,
			goalWidth, currentVert ) ;
	}
	if ( offset = goalHeight - currentVert ) {
		COPY_RECT( cmd,  
			x, y + goalHeight,
			x, y + offset,
			goalWidth, offset ) ;
	}

	return ;
}


