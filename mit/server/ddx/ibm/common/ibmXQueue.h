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
/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmXQueue.h,v 9.1 88/10/31 23:10:46 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmXQueue.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidqevent = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmXQueue.h,v 9.1 88/10/31 23:10:46 paul Exp $" ;
#endif

/*
* IBM -- Required Event Queue Structures
* If your kernel doesn't give you this, you are in trouble.
*
* Event queue entries
*/

#ifndef _XQEVENT_
#define _XQEVENT_

typedef struct _X_event {
	unsigned short xe_x ; /* x position */
	unsigned short xe_y ; /* y position */
#ifndef i386
	unsigned short xe_time ; /* 10 millisecond units (button only) */
#else
	long xe_time ; /* 1 millisecond units (button only) */
#endif
	unsigned char xe_type ; /* button or motion? */
	unsigned char xe_key ; /* the key (button only) */
	unsigned char xe_direction ; /* which direction (button only) */
	unsigned char xe_device ; /* which device (button only) */
} XEvent ;

/* xe_type field */
#define XE_BUTTON 0 /* button moved */
#define XE_MMOTION 1 /* mouse moved */
#define XE_TMOTION 2 /* tablet moved */

/* xe_direction field */
#define XE_KBTUP 0 /* up */
#define XE_KBTDOWN 1 /* down */
#define XE_KBTRAW 2 /* undetermined */

/* xe_device field */
#define XE_MOUSE 1 /* mouse */
#define XE_DKB 2 /* main keyboard */
#define XE_TABLET 3 /* graphics tablet */
#define XE_AUX 4 /* auxiliary */
#define XE_CONSOLE 5 /* console */

/* The event queue */

typedef struct _X_eventqueue {
	XEvent *events ; /* input event buffer */
	int size ; /* size of event buffer */
	int head ; /* index into events */
	int tail ; /* index into events */
} XEventQueue ;

/* mouse cursor position */
typedef struct _X_cursor {
	short x ;
	short y ;
} XCursor ;

/* mouse motion rectangle */
typedef struct _X_box {
	short bottom ;
	short right ;
	short left ;
	short top ;
} XBox ;

struct x_screen_size {
	int x_x_min ; /* minimum x for mouse */
	int x_y_min ; /* minimum y for mouse */
	int x_x_max ; /* maximum x for mouse */
	int x_y_max ; /* maximum y for mouse */
} ;

struct x_softc {
	struct x_screen_size x_size ; /* screen size for mouse tracking */
	short x_cursor ; /* is the kernel cursor on or off */
	short x_notrack ; /* can the kernel cursor be turned on */
} ;

# endif _XQEVENT_
