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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelSave.c,v 1.2 88/11/07 17:27:10 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelSave.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelSave.c,v 1.2 88/11/07 17:27:10 paul Exp $" ;
#endif

#include "mpelHdwr.h"
#include "mpelFifo.h"

void
mpelSaveState()
{
#if defined(AIXrt)
	mpelSaveFonts() ;
	MPELWaitFifo() ;
#else
	ErrorF( "mpelSaveState: Function Called ???\n" ) ;
#endif
	return ;
}

void
mpelRestoreState()
{
#if defined(AIXrt)
	MPELWaitFifo() ;
	MPEL_PCR = PCR_BYTE_ORDER ;
	mpelRestoreFonts() ;
	mpelInitPatterns() ;
/*  Remove this line due to reordering of save state routine (DSF 9/28/88)
	mpelRevalidateCursor() ;  */
	MPEL_COMM_REASON = 0 ;
	MPEL_COMM_REQ = MPELCMD_ENTER_FIFO ;
	MPEL_PCR &= ~PCR_INTR_TMS ;
	MPEL_PCR |= PCR_INTR_TMS ;
#else
	ErrorF( "mpelRestoreState: Function Called ???\n" ) ;
#endif
	return ;
}
