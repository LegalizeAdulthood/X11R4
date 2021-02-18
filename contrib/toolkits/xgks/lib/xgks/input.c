/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
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
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *	All rights reserved.
 *
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * functions to manipulate the list of logical input devices
 * 
 * $Header: input.c,v 4.0 89/08/31 16:21:12 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/input.c,v $
 *
 * $Log:	input.c,v $
 * Revision 4.0  89/08/31  16:21:12  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.15  89/06/05  10:16:37  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.14  89/05/22  09:19:24  bruce
 * PTR# c1177:	Modify calls to Update Prompt routines to insure
 * 		they do not dereference NULL
 * 
 * Revision 3.13  89/03/15  16:39:25  amy
 * PTR c1164	XgksIProcessXEvent:  check for ws_is_closing 
 * 		before continuing processing of event.
 * 
 * Revision 3.12  89/02/24  12:40:18  amy
 * PTR c1157	XgksIDevDisable:  undo fix that suppressed erasure
 * 		of locator device.  Locator lines are drawn in XOR
 * 		mode, so they must be erased here, otherwise they
 * 		would disappear when later
 * 		redrawn.
 * 
 * Revision 3.11  89/02/24  12:04:52  amy
 * PTR c1151	XgksIProcessXEvent:  filtered out keyboard input for
 * 		input devices except choice and string.
 * 		XgksIDevDisable:  undid part of previous fix-- used to
 * 		erase all devices on the screen.  Changed this to keep
 * 		all the devices from flashing.  But because stroke is
 * 		drawn with XOR, stroke echoing is erased.
 * 
 * Revision 3.10  89/02/04  15:24:20  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.9  88/12/16  12:33:45  amy
 * PTR c1133	XgksIProcessXEvent:  record event_id when processing a
 * 		GKS input event, for simultaneous events detection.
 * 
 * Revision 3.8  88/11/30  12:47:33  amy
 * PTR c1023	XgksIDevDisable:  commented out calls to Xgks???UpdatePrompt
 * 		to suppress erasure of input devices from screen.
 * 		This function is called before outputting a primitive to the
 * 		display.  Then, when primitive has been displayed, XgksIDevEnable
 * 		is called which redraws input devices.  By suppressing the
 * 		initial erasure of the devices, the screen is updated more quickly.
 * 
 * Revision 3.7  88/09/28  07:35:01  amy
 * MIT	XgksIProcessEvent:  in calls to StrUpdatePrompt and ChoUpdatePrompt,
 * 	remove fourth parameter, newdcpt, which was removed from functions' definitions.
 * 
 * Revision 3.6  88/09/21  08:55:01  amy
 * MIT	XgksIDevNew:  initialize class to GNCLASS (from FALSE).
 * 	All calls to Xgks*UpdatePrompt:
 * 	change from five parameters to four.
 * 	cast NULL pointers to be of correct type.
 * 
 * Revision 3.5  88/08/17  07:23:38  amy
 * AUG  XgksIProcessXEvent:  for choice, added check for approp. event for pet
 * 	  (e.g., keypress or button press).
 *      XgksIProcessXEvent:  for string, added check for event = keypress.
 *      XgksIDevDisable:  added check for ECHO before calling UpdatePrompt.
 *      XgksIDevEnable:  added check for ECHO before calling UpdatePrompt.
 * 
 * Revision 1.2  88/07/26  17:52:06  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:48:52  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: input.c,v 4.0 89/08/31 16:21:12 amy Exp $";

#include "gks_implem.h" /* d1 */
#include <X11/cursorfont.h>
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

/*
 * Initialise the input device list
 */

XgksInitIDev( ws )
	WS_STATE_ENTRY *ws;
{
	XSetWindowAttributes xswa;

	ws->in_dev_list = NULL;
	if (ws->ewstype == X_WIN) {
		xswa.cursor = XCreateFontCursor( ws->dpy, XC_crosshair);
		XChangeWindowAttributes( ws->dpy, ws->win, CWCursor, &xswa);
	}
}
/*
 * Add a logical device to the list
 */

void
XgksIDevAdd( ws, idev )
	WS_STATE_ENTRY *ws;
	INPUT_DEV *idev;
{
	idev->next = ws->in_dev_list;
	ws->in_dev_list = idev;
}

/* 
 * Cleaning up on input device queue.
 * This routine will free up all memory assoicated with input device structure
 *
 */

XgksIDevDelete (ws)
	WS_STATE_ENTRY *ws;
{
	INPUT_DEV *idev = ws->in_dev_list;

	while ( idev != NULL ) {
		switch (idev->class) {
			case GLOCATOR : XgksLocDelete( ws, idev ); break;
			case GISTROKE : XgksStkDelete( ws, idev ); break;
			case GVALUATOR: XgksValDelete( ws, idev ); break;
			case GCHOICE  : XgksChoDelete( ws, idev ); break;
			case GPICK    : XgksPicDelete( ws, idev ); break;
			case GISTRING : XgksStrDelete( ws, idev ); break;
		}
		idev = ws->in_dev_list->next;
		free (ws->in_dev_list);
		ws->in_dev_list = idev;
	}
}
/*
 * Allocate memory for a new logical input device, returns pointer
 * to the new device structure, or NULL is malloc fails.
 *   Some Default initilation are done ...
 *
 *     idev->class    = GNCLASS;
 *     idev->dev      = INVALID;
 *     idev->active   = FALSE;
 *     idev->touched  = FALSE;
 *     idev->breakhit = FALSE;
 *     idev->next = NULL;
 *
 */

INPUT_DEV *
XgksIDevNew ()
{
	INPUT_DEV *new;

	if ( (new = (INPUT_DEV *) malloc (sizeof (INPUT_DEV))) != NULL) {
		new->class   = GNCLASS;		/*MIT*/
		new->dev     = INVALID;
		new->active  = FALSE;
		new->touched = FALSE;
		new->breakhit= FALSE;
		new->next = NULL;
	}
	return (new);
}
/*
 * Search the input device list for a specific device.  Return the device or
 * a NULL pointer.
 */

INPUT_DEV *
XgksIDevLookup( ws, dev, class )
	WS_STATE_ENTRY *ws;
	Gint dev;
	Giclass class;
{
	INPUT_DEV *id;

	for( id= ws->in_dev_list; id != NULL; id = id->next)
		if ( (id->class == class) && (id->dev == dev) )
			return( id );
	return( (INPUT_DEV *) NULL );
}

/*
 * Process X events and see if they trigger any GKS logical input devices
 *
 * XEvent can be one of: MotionNotify
 */
XgksIProcessXEvent( xev )
	XEvent *xev;
{
	int i;
	WS_STATE_ENTRY *ws;
	INPUT_DEV *idev;
	Bool breakhit;
	KeySym ksym;
	Gpoint dcpt;
	XPoint xpt;
	Glimit *ea;
#define InEchoArea	( dcpt.x >= ea->xmin && dcpt.x <= ea->xmax \
			&& dcpt.y >= ea->ymin && dcpt.y <= ea->ymax )

	XMotionEvent *xmev;

	/* simultaneous events have same ID! */ /* PTR c1133 */
        static int current_event_id = 0;        /* PTR c1133 */
#define MAX_EVENT_ID 32767                      /* PTR c1133 */
	
	xmev = (XMotionEvent *)xev;
/* Find the gks workstation associated with the X window */
	for( i=0; i<MAX_OPEN_WS; i++) {
		if ( xgks_state.openedws[i].ws_id == INVALID) continue; /*c1147*/
		if ( xgks_state.openedws[i].win == xmev->window )	/*c1147*/
			break;
	}
	if ( i == MAX_OPEN_WS )		/* NOT FOUND */
		return;

	ws = xgks_state.openedws[i].ws;					/*c1147*/

	/* if the closing of this ws has already been initiated, */ /* c1164 */
	/* don't try to do anything.  (by the time we get to the */ /* c1164 */
        /* inquire color map the actual X window may be gone -   */ /* c1164 */
        /* producing an X error!)         DWO                    */ /* c1164 */
        if (ws->ws_is_closing)                                      /* c1164 */
	   return;                                                  /* c1164 */

	switch( xev->type ) {
	case ButtonPress:
	case ButtonRelease:
	case MotionNotify:
	case KeyPress:
	/* transform point from X space to DC space */
		xpt.x = xmev->x;
		xpt.y = xmev->y;
		XToDc( ws, &xpt, &dcpt );

		if (xev->type == KeyPress) {
			XLookupString( xev, NULL, 0, &ksym, NULL);
			breakhit = (ksym == XK_Pause) || (ksym == XK_Break) ? True: False;
		}
		else
			breakhit = False;
#ifdef IDEVDEBUG
		fprintf(stderr, "XgksIProcessXEvent() xmev %d %d xpt=%d %d dcpt %f %f\n", xmev->x, xmev->y, xpt.x, xpt.y, dcpt.x, dcpt.y);
#endif

	 /* increment curr event ID.  (used */  /* PTR c1133 */
        /* to determine if two events are  */  /* PTR c1133 */ 
	/* simultaneous events - they are  */  /* PTR c1133 */ 
        /* if they have the same ID        */  /* PTR c1133 */
	if (++current_event_id > MAX_EVENT_ID) /* PTR c1133 */ 
	  current_event_id = 0;                /* PTR c1133 */

	/* check each active input device for trigger */
		for( idev=ws->in_dev_list; idev != NULL; idev = idev->next) {
			if ( idev->active == False)
				continue;
		/* check echo area */
			switch( idev->class ) {
			case GLOCATOR:
				ea = &idev->data.loc.initst.e_area;
				if ( InEchoArea )
					if (breakhit)
						idev->breakhit = True;
					else
					    if (xev->type!=KeyPress) /* c1151 */
						XgksLocUpdatePrompt( ws, idev, PROMPTMOVE, &dcpt, xmev, current_event_id); /* PTR c1133 */
				break;
			case GCHOICE:
				ea = &idev->data.cho.initst.e_area;
#ifdef IDEVDEBUG
	fprintf(stderr, "XgksIProcessXEvent: dcpt %f %f Cho earea = %f %f %f %f\n",
		  dcpt.x, dcpt.y, ea->xmin, ea->xmax, ea->ymin, ea->ymax);
if ( InEchoArea ) 
	fprintf(stderr, "XgksIProcessXEvent: calling XgksChoUpdatePrompt\n");
#endif
				if ( InEchoArea ) 
					if (breakhit)
						idev->breakhit = True;
					else{
	if(((idev->data.cho.initst.pet==3)&& (xev->type!=KeyPress))  /* AUG */
        ||((idev->data.cho.initst.pet==1)&&(xev->type==KeyPress))    /* AUG */
        ||((idev->data.cho.initst.pet==2)&&(xev->type==KeyPress)))   /* AUG */

						XgksChoUpdatePrompt( ws, idev, PROMPTMOVE, xmev, current_event_id);} /*MIT*/ /* PTR c1133 */
				break;
			case GVALUATOR:	
				ea = &idev->data.val.initst.e_area;
				if ( InEchoArea ) 
					if (breakhit)
						idev->breakhit = True;
					else
					    if (xev->type!=KeyPress) /* c1151 */
						XgksValUpdatePrompt( ws, idev, PROMPTMOVE, &dcpt, xmev, current_event_id); /* PTR c1133 */
				break;
			case GPICK:
				ea = &idev->data.pic.initst.e_area;
#ifdef IDEVDEBUG
if ( InEchoArea ) 
	fprintf(stderr, "XgksIProcessXEvent: device GPICK --> InEchoArea .. %f %f %f %f ?\n",
		  ea->xmin, ea->xmax, ea->ymin, ea->ymax);
else fprintf(stderr, "XgksIProcessXEvent: device GPICK --> InEchoArea .. %f %f %f %f ?\n",
		  ea->xmin, ea->xmax, ea->ymin, ea->ymax);
#endif
				if ( InEchoArea ) 
					if (breakhit)
						idev->breakhit = True;
					else
					    if (xev->type!=KeyPress) /* c1151 */
						XgksPicUpdatePrompt( ws, idev, &dcpt, xmev, current_event_id);	/*MIT*/ /* PTR c1133 */
				break;
			case GISTRING:
				ea = &idev->data.str.initst.e_area;
				if ( InEchoArea )
					if (breakhit)
						idev->breakhit = True;
					else {
			if(xev->type==KeyPress)				/* AUG */
						XgksStrUpdatePrompt( ws, idev, PROMPTMOVE, xmev, current_event_id);} /*MIT*/ /* PTR c1133 */ 
				break;
			case GISTROKE:
				ea = &idev->data.stk.initst.e_area;
				if ( InEchoArea )
					if (breakhit)
						idev->breakhit = True;
					else
					   if (xev->type!=KeyPress) /* c1151 */
						XgksStkUpdatePrompt( ws, idev, PROMPTMOVE, &dcpt, xmev, current_event_id); /* PTR c1133 */
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}
/*
 * Disable all input devices for a given workstation.
 *	Basically, just get their prompts off the screen so that GKS can
 *	produce some output.
 */
static DisCount = 0;

XgksIDevDisable( ws )
	WS_STATE_ENTRY *ws;
{
	INPUT_DEV *idev;

	DisCount++;

	if (ws->ewstype != X_WIN) return;

#ifdef IDEVDEBUG
	fprintf(stderr, "XgksIDevDisable() DisCount = %d\n", DisCount);
#endif
	if (DisCount > 1)	/* already done */
		return;

	XgksSIGIO_OFF(ws->dpy);	/*c1147*/ /* d1 */

/* check each active input device for trigger */
	for( idev=ws->in_dev_list; idev != NULL; idev = idev->next) {
		if ( idev->active == False)
			continue;
	/* check echo area */
		switch( idev->class ) {
		case GLOCATOR:
			/* this call is not commented out because  *//* c1157 */
                        /* loclines are draw in XOR mode. So if we *//* c1157 */
                        /* didn't erase it now, it would disappear *//* c1157 */
                        /* when we tried to redraw it later!       *//* c1157 */
			if(idev->data.loc.initst.esw==GECHO) /* AUG     c1157 */
			  XgksLocUpdatePrompt( ws, idev, PROMPTOFF,  /* c1157 */
                             (Gpoint *)NULL,(XMotionEvent *)NULL,-1);/* c1157 */
                             /*MIT*/ /* PTR c1133 */ /* DWO */       /* c1157 */
			break;
		case GCHOICE:
			/* This call has been commented out because it caused */
                        /* all the devices to flash for each primitive drawn  */
                        /* (This was the cause of PTR c1023  -  DWO)          */
			/*if(idev->data.cho.initst.esw==GECHO)*/ /* AUG */
			/*XgksChoUpdatePrompt( ws, idev, PROMPTOFF, (XMotionEvent *)NULL, -1); */ /*MIT*/ /* PTR c1023 */ /* PTR c1133 */ 
			break;
		case GPICK:
			/* This call has been commented out because it caused */
                        /* all the devices to flash for each primitive drawn  */
                        /* (This was the cause of PTR c1023  -  DWO)          */
			/*if(idev->data.pic.initst.esw==GECHO)*/ /* AUG */
			/*XgksPicUpdatePrompt( ws, idev, (Gpoint *)NULL, (XMotionEvent *)NULL, -1);*/  /*MIT*/ /* PTR c1023 */ /* PTR c1133 */
			break;
		case GVALUATOR:
			/* This call has been commented out because it caused */
                        /* all the devices to flash for each primitive drawn  */
                        /* (This was the cause of PTR c1023  -  DWO)          */
			/*if(idev->data.val.initst.esw==GECHO)*/ /* AUG */
			/*XgksValUpdatePrompt( ws, idev, PROMPTOFF, (Gpoint *)NULL, (XMotionEvent *)NULL, -1);*/  /*MIT*/ /* PTR c1023 */ /* PTR c1133 */ 
			break;
		case GISTRING:
			/* This call has been commented out because it caused */
                        /* all the devices to flash for each primitive drawn  */
                        /* (This was the cause of PTR c1023  -  DWO)          */
			/*if(idev->data.str.initst.esw==GECHO)*/ /* AUG */
			/*XgksStrUpdatePrompt( ws, idev, PROMPTOFF, (XKeyPressedEvent *)NULL, -1);*/  /*MIT*/ /* PTR c1023 */ /* PTR c1133 */ 
			break;
		case GISTROKE:
			/* this call is not commented out because  *//* c1151 */
                        /* strokes are drawn in XOR mode.  So if we*//* c1151 */
                        /* didn't erase it now, it would disappear *//* c1151 */
                        /* when we tried to redraw it later!       *//* c1151 */
			if(idev->data.stk.initst.esw==GECHO)/* AUG *//* c1151 */
			  XgksStkUpdatePrompt( ws, idev, PROMPTOFF, (Gpoint *)NULL, (XMotionEvent *)NULL, -1);  /*MIT*/ /* PTR c1133 */ /* c1151 */
			break;
		default:
			break;
		}
	}
}
/*
 * Enable all input devices for a given workstation.
 */
XgksIDevEnable( ws )
	WS_STATE_ENTRY *ws;
{
	INPUT_DEV *idev;

	DisCount--;


	if (ws->ewstype != X_WIN) return;

#ifdef IDEVDEBUG
	fprintf(stderr, "XgksIDevEnable() DisCount = %d\n", DisCount);
#endif
	if (DisCount > 0)	/* only disable on last request */
		return;

/* check each active input device for trigger */
	for( idev=ws->in_dev_list; idev != NULL; idev = idev->next) {
		if ( idev->active == False)
			continue;
	/* check echo area */
		switch( idev->class ) {
		case GLOCATOR:
			if(idev->data.loc.initst.esw==GECHO)		/* AUG */
			XgksLocUpdatePrompt( ws, idev, PROMPTON, (Gpoint *)NULL, (XMotionEvent *)NULL, -1); /*MIT*/ /* PTR c1133 */ 
			break;
		case GCHOICE:
			if(idev->data.cho.initst.esw==GECHO)		/* AUG */
			XgksChoUpdatePrompt( ws, idev, PROMPTON, (XMotionEvent *)NULL, -1); /*MIT*/ /* PTR c1133 */
			break;
		case GPICK:	
			if(idev->data.pic.initst.esw==GECHO)		/* AUG */
			XgksPicUpdatePrompt( ws, idev, &(idev->data.pic.curpos), (XMotionEvent *)NULL, -1); /*MIT*/ /* PTR c1133 c1177 */
			break;
		case GVALUATOR:
			if(idev->data.val.initst.esw==GECHO)		/* AUG */
			XgksValUpdatePrompt( ws, idev, PROMPTON, &(idev->data.val.CurPos), (XMotionEvent *)NULL, -1); /*MIT*/ /* PTR c1133 c1177 */
			break;
		case GISTRING:
			if(idev->data.str.initst.esw==GECHO)		/* AUG */
			XgksStrUpdatePrompt( ws, idev, PROMPTON, (XKeyPressedEvent *)NULL, -1); /*MIT*/ /* PTR c1133 */
			break;
		case GISTROKE:
			if(idev->data.stk.initst.esw==GECHO)		/* AUG */
			XgksStkUpdatePrompt( ws, idev, PROMPTON, &(idev->data.stk.stkbuf[idev->data.stk.editpos]), (XMotionEvent *)NULL, -1); /*MIT*/ /* PTR c1133 */
			break;
		default:
			break;
		}
	}
	XgksSIGIO_ON(ws->dpy);		/*c1147*/ /* d1 */
}
