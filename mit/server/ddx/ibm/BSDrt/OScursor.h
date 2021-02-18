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
#ifndef OS_CURS
#define OS_CURS 1

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
/* $Header: /andrew/X11/R3src/Xbeta/server/ddx/ibm/BSDrt/RCS/OScursor.h,v 9.0 88/10/16 22:57:24 erik Exp $ */
/* $Source: /andrew/X11/R3src/Xbeta/server/ddx/ibm/BSDrt/RCS/OScursor.h,v $ */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidoscursor = "$Header: /andrew/X11/R3src/Xbeta/server/ddx/ibm/BSDrt/RCS/OScursor.h,v 9.0 88/10/16 22:57:24 erik Exp $";
#endif


#define OS_TellCursorPosition( x, y ) { \
		    XCursor pos; \
		    pos.x = x ; \
		    pos.y = y ; \
		    ioctl(BSDEmulatorFD,QIOCSMSTATE,(caddr_t)&pos); \
}

extern	int	BSDSetCursorPosition();
extern	void	BSDPointerNonInterestBox();
extern	void	BSDConstrainCursor();
extern	void	BSDCursorLimits();

#define	OS_SetCursorPosition		BSDSetCursorPosition
#define	OS_PointerNonInterestBox	BSDPointerNonInterestBox
#define OS_ConstrainCursor		BSDConstrainCursor
#define	OS_CursorLimits			BSDCursorLimits

#endif /* ndef OS_CURSOR */
