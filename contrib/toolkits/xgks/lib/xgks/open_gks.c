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
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * open_gks.c - functions for opening and closing gks.
 *		open_gks()
 *		close_gks()
 * 
 * $Header: open_gks.c,v 4.0 89/08/31 16:23:13 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/open_gks.c,v $
 *
 * $Log:	open_gks.c,v $
 * Revision 4.0  89/08/31  16:23:13  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.14  89/06/05  10:17:28  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.13  89/05/22  09:21:23  bruce
 * PTR# c1178:	Added defines that will declare default attribute bundles
 * 		globally to work around compiler glitch.
 * 
 * Revision 3.12  89/02/04  15:27:03  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.11  89/01/27  12:39:24  amy
 * Cosmetic change to Log.
 * 
 * Revision 3.10  89/01/27  12:36:29  amy
 * PTR c1145	Define focus_ws as pointer to the
 * 		workstation which currently has the focus, and initialize it to NULL in
 *		gopengks.
 * 
 * Revision 3.9  88/12/22  14:11:38  amy
 * PTR c1142	Added current pick id and name of current open segment to GKS
 * 		state list (moved from segments.c).
 * 
 * Revision 3.8  88/12/16  12:39:49  amy
 * PTR c1133	XgksInitGksStateList:  initialize current event.
 * 
 * Revision 3.7  88/12/08  12:01:16  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 3.6  88/11/29  10:05:00  amy
 * PTR c1058	XgksInitGksStateList:  added code to initialize the current 
 *		report.  Added entry in GKS state list for current event report.
 * 
 * Revision 3.5  88/09/22  09:09:56  amy
 * MIT	gopengks:  declare internal functions.
 * 	XgksInitGksErrorStateList:  declare static void.
 * 	XgksInitGksStateList:  declare static void.
 * 
 * Revision 3.4  88/08/18  06:23:42  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.2  88/07/26  17:53:11  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:43  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: open_gks.c,v 4.0 89/08/31 16:23:13 amy Exp $";

static char *IBMCOPYRIGHT = "(C) Copyright IBM CORPORATION 1988";
static char *COPYRIGHT = "(C) Copyright  1987, 1988 by The University of Illinois Board of Trustees.";

#include "gks_implem.h"                 /* d1 */
#define fillarea_c              /* c1178 */
#define text_c                  /* c1178 */
#define polylines_c             /* c1178 */
#define polymarkers_c           /* c1178 */
#include "fillarea.h"           /* c1178 */
#include "text.h"               /* c1178 */
#include "polylines.h"          /* c1178 */
#include "polymarkers.h"        /* c1178 */
#undef fillarea_c               /* c1178 */
#undef text_c                   /* c1178 */
#undef polylines_c              /* c1178 */
#undef polymarkers_c            /* c1178 */

/* gks state list */
/* c1147: the whole gks state list is now in a big structure (see gkslist.h) */
GKS_STATE_LIST	xgks_state;					/* c1147 */

/* c1147: had to change all the initialization code, of course */
/*    (moved to XgksInitGksStateList)                          */



/*$F
 * gopengks(err_file, memory) - OPEN GKS - initialises gks data structures.
 *
 * Gfile *err_file;       file where gks is to print error messages.
 * int memory;		maximum buffer space, ignored in this implementation.
 * 
 * returns 0 - opened ok
 *	   1 - gks not in proper state
 *       200 - specified error file is invalid
 *
 * See also: Ansi standard p. 74
 */
gopengks(err_file, memory)
	Gfile *err_file;
	Glong memory;
{
	/*
	 * declare internal functions
	 */
	static void XgksInitGksErrorStateList();		/*MIT*/
	static void XgksInitGksStateList();			/*MIT*/


/* specified error file is not opened yet */
	GKSERROR ((err_file == NULL) ,200, errgopengks)

/* can't open if not closed */
	GKSERROR ((xgks_state.gks_state != GGKCL) ,1, errgopengks)	/*c1147*/

	XgksInitGksErrorStateList(err_file);
	XgksInitGksStateList();
	return(0);
}



/*$F
 * gclosegks() - CLOSE GKS - sets the operating state to GGKCL if current state is GGKOP
 *
 * returns: 0 - closed ok
 *	    2 - gks not in proper state
 *
 * See also: ANSI standard p.74
 */

gclosegks()
{

/* check for proper state */
	GKSERROR ((xgks_state.gks_state != GGKOP) ,2, errgclosegks)	/*c1147*/

	xgks_state.gks_state = GGKCL;					/*c1147*/
	return(0);
}

/*
* XgksInitGksErrorStateList(err_file) - initialise data for error processing.
*
* Gfile *err_file;
*/

static void XgksInitGksErrorStateList(err_file)		/*MIT*/
Gfile *err_file;
{
	xgks_state.gks_err_file = err_file; /* save the error file  */ /*c1147*/
	xgks_state.gks_err_state = ERR_OFF; /* no error has occured */ /*c1147*/
	/* rest of list is undefined until input queue overflow */
}


/*
* XgksInitGksStateList() - called by open_gks() to initialise the gks state list.
*
* returns nothing
*/

static void XgksInitGksStateList()		/*MIT*/
{
	Gint i;

	/* c1147:  additional initialization made necessary by bundling of */
	/*    gks state list into a single structure (this initialization  */
	/*    was formerly done globally at the declaration)               */ 
	xgks_state.level = GL2C;	/* Level of GKS */		/* c1147 */
	/* Workstation maximum numbers (open, active, assoc) */		/* c1147 */
	xgks_state.wsmax.open = MAX_OPEN_WS;				/* c1147 */
	xgks_state.wsmax.active = MAX_ACTIVE_WS;			/* c1147 */
	xgks_state.wsmax.assoc = MAX_ASSOC_WS;				/* c1147 */

	xgks_state.wiss_id = INVALID;					/*c1147*/
		/* Switch to keep track of Wiss's open and close */
	xgks_state.open_mo = 0;	/* Number of open meta output files */  /*c1147*/

	/* Set all workstations to close and inactive */
        for (i=0; i<MAX_OPEN_WS; i++) {
	    xgks_state.openedws[i].ws_id = INVALID;			/*c1147*/
	    xgks_state.openedws[i].win = INVALID;			/*c1147*/
	    xgks_state.openedws[i].ws = NULL;				/*c1147*/
	}
	for (i=0; i<MAX_ACTIVE_WS; i++) {
	    xgks_state.activews[i].ws_id = INVALID;			/*c1147*/
	    xgks_state.activews[i].win  = INVALID;			/*c1147*/
	    xgks_state.activews[i].ws = NULL;				/*c1147*/
	}

	xgks_state.gks_state = GGKOP;	/* set state to GKs OPen */	/*c1147*/
	XgksInitGksAsf();		/* initialize asfs */
	XgksInitGksPlines();	/* initialise polyline stuff */
	XgksInitGksPmarkers();	/* initialise polymarker stuff */
	XgksInitGksFillArea();	/* initialise fill area stuff */
	XgksInitGksSegments();    /* init segment menagement and state list */
	XgksInitGksText();	/* initialise text stuff */
	XgksInitGksTrans();	/* initialise transformation stuff */

	XgksInitGksM ();	/* Initialise metafile system */

	xgks_state.event_queue_head = NULL;			/*c1147*/
	xgks_state.event_queue_tail = NULL;			/*c1147*/

	xgks_state.CurEvent.next = (EQEntry *)NULL;    /* PTR c1058 */ /*c1147*/
        xgks_state.CurEvent.event.ws = 0;              /* PTR c1058 */ /*c1147*/
        xgks_state.CurEvent.event.dev = 0;             /* PTR c1058 */ /*c1147*/
        xgks_state.CurEvent.event.class = GNCLASS;     /* PTR c1058 */ /*c1147*/
        xgks_state.CurEvent.data = (char *)NULL;       /* PTR c1058 */ /*c1147*/
	xgks_state.CurEvent.id = -999;                 /* PTR c1133 */ /*c1147*/
	xgks_state.focus_ws = (WS_STATE_PTR) NULL;     /* PTR c1145 */ /*c1147*/

}
