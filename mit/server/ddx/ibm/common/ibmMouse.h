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
#ifndef IBM_MOUSE
#define IBM_MOUSE 1

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmMouse.h,v 9.1 88/10/31 23:10:18 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmMouse.h,v $ */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidrtmouse = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmMouse.h,v 9.1 88/10/31 23:10:18 paul Exp $";
#endif

extern	DevicePtr	 ibmPtr;
extern	int		 ibmDoPointerAccel;
extern	int		 ibmPointerThreshold;
extern	int		 ibmPointerAccelNumerator;
extern	int		 ibmPointerAccelDenominator;
extern	char		*mouse_device;
extern	void		 ibmChangePointerControl();

/*	defined in OS specific layer  */
extern	int		 osMouseProc();

#define	ibmAccelerate(x,y)	\
	{ \
	if (ibmDoPointerAccel&&(ABS((x)+(y))>ibmPointerThreshold)) {\
	    (x)= ((x)*ibmPointerAccelNumerator)/ibmPointerAccelDenominator;\
	    (y)= ((y)*ibmPointerAccelNumerator)/ibmPointerAccelDenominator;\
	}\
	}

#endif /* IBM_MOUSE */
