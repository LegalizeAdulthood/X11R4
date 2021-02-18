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
/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Hdwr.c,v 3.3 89/12/10 20:43:59 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Hdwr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Hdwr.c,v 3.3 89/12/10 20:43:59 probe Exp $";
static char sccsid[] = "@(#)apa16hdwr.c	3.1 88/09/22 09:30:43";
#endif

#include "Xmd.h"

#include "OScompiler.h"
#include "apa16Hdwr.h"
	/*
	 * variables and functions to manipulate the apa16
	 * rasterop hardware
	 */

int	apa16Qoffset;
int	apa16Qoverflow = 0;
int	apa16Qmerge_status = FALSE;
int	apa16Qmerge_mode_old;
int	apa16Qmerge_mode_new;

#ifdef NOT_X
#define ErrorF printf
#endif

/***============================================================***/

unsigned short apa16_rop2stype[16] = {
	/* GXclear */		0,
	/* GXand */		1,
	/* GXandReverse */	2,
	/* GXcopy */		9,
	/* GXandInverted */	8,
	/* GXnoop */		0,
	/* GXxor */		0xa,
	/* GXor */		0xb,
	/* GXnor */		0,
	/* GXequiv */		0,
	/* GXinvert */		0,
	/* GXorReverse */	0,
	/* GXcopyInverted */	0,
	/* GXorInverted */	0xc,
	/* GXnand */		0xe,
	/* GXSet */		0};

unsigned short apa16_rop2logop[16] = {
	/* GXclear */		0x0,
	/* GXand */		0x1,
	/* GXandReverse */	0x2,
	/* GXcopy */		0x3,
	/* GXandInverted */	0x8,
	/* GXnoop */		0x9,
	/* GXxor */		0xa,
	/* GXor */		0xb,
	/* GXnor */		BAD_LOGOP,
	/* GXequiv */		BAD_LOGOP,
	/* GXinvert */		BAD_LOGOP,
	/* GXorReverse */	BAD_LOGOP,
	/* GXcopyInverted */	BAD_LOGOP,
	/* GXorInverted */	0xc,
	/* GXnand */		0xe,
	/* GXSet */		0xf};

/*
 * apa16CheckQ()
 * 
 * This routine is called from the IBM common code to handle the
 * overflow queue.  Sometimes, the queue space is larger than the
 * maximum number of instructions (1023), so with this routine,
 * we can take advantage of the extra space.
 *
 * It is only called from the event-checking loop.  The reason it
 * is required is that if there is a sudden stop of graphic activity
 * that there be some method to tell the hardware that it has more
 * queued commands.  The commands must not be left lingering, as
 * this will have the wrong effect on the screen.  Also, the queue
 * counter going to zero seems to have unpredictable results; it
 * appears that the next instruction if it appears like a branch will
 * be executed sending us to la-la land.
 */

apa16CheckQ()
{
    /*
     * The following lines have been commented out since we are
     * not currently using overflow.  Some glitches have been
     * seen in the queue handling, and until all the problems
     * are found, it is easier leaving out this new feature.
     */
#if 0
    while ( ((QUEUE_CNTR&0x3ff) < 0x3ff) && apa16Qoverflow ) {
	INCR_QUEUE_CNTR();
	apa16Qoverflow--;
    }
#endif
}
