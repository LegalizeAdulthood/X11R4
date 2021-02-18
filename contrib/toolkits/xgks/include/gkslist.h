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
 * $Header: gkslist.h,v 4.0 89/08/31 16:34:53 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/gkslist.h,v $
 *
 * $Log:	gkslist.h,v $
 * Revision 4.0  89/08/31  16:34:53  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.12  89/03/16  13:12:44  amy
 * PTR c2048	REDRAWWS:  calls XgksCleanUpWsSegList if irgmode is SUPPRESSED.
 * 
 * Revision 3.11  89/03/15  17:40:56  amy
 * PTR c1159	Added fontdbdir field to gks state list to contain the
 * 		fonts path.
 * 
 * Revision 3.10  89/02/04  15:32:42  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.9  89/01/27  12:56:22  amy
 * Cosmetic change to the log.
 * 
 * Revision 3.8  89/01/27  12:54:48  amy
 * PTR c1145	To GKS state list variables, added focus_ws, pointer to ws which  
 * 		currently has the focus.
 * 
 * Revision 3.7  88/12/29  11:04:12  amy
 * PTR c1139	REDRAWWS macro:  check that ws is not an MO before redrawing
 * 		segments on ws.
 * 
 * Revision 3.6  88/12/22  14:13:35  amy
 * PTR c1142	Added extern statements for current pick id and current open
 * 		segment.
 * 
 * Revision 3.5  88/12/16  11:29:26  amy
 * PTR c1133	Removed variable "simultev."
 * 
 * Revision 3.4  88/11/29  10:08:07  amy
 * PTR c1058	Declare CurEvent an extern.
 * 
 * Revision 3.3  88/08/19  13:17:49  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:47:20  david
 * Initial revision
 *  
 * 
 */

#ifndef  GKSLIST_H
#define  GKSLIST_H


#include "wslist.h"			/* Workstation state list */

/* GKS description table definitions */

#define MAX_TRANS	31		/* Maximum normalization transformation */

/* definition for GKS state list starts here */

typedef struct {
	Gint	ws_id;			/* GKS Ws_id of the pointer */
	Window  win;			/* X window identifier */
	WS_STATE_PTR ws;		/* pointer to the ws state list */
} WS_ENTRY;				/* Each entry of a ws state list */

typedef struct {
	Gwstran		ntrans; 
	Glimit		wc_to_ndc;	/* Transformation maxtrix for wc to ndc mapping */
} NORM_TRANS;

/* c1147: this is the structure which contains the gks state list */
typedef struct {

	EQEntry *event_queue_head;	/* Event queue */
	EQEntry *event_queue_tail;	/* Insert after tail */

/* current event (used in event.c)  definition moved from event.c */
/* for PTR c1058        (DWO)                                     */
	EQEntry CurEvent;                             /* PTR c1058 */

/* gks_state list variables */

	Glevel		level;		/* Level of GKS */
	Gwsmax		wsmax;		/* Workstation maximum numbers (open, active, assoc) */

/* GKS operating state */
	Gos		gks_state;	/* GKS operating state */

	WS_ENTRY	openedws[MAX_OPEN_WS];   /* List of open workstations */
	WS_ENTRY	activews[MAX_ACTIVE_WS]; /* List of active wrkstatns */

	WS_STATE_PTR	focus_ws;	/* ws /w focus */ /* c1145 */

	Glnattr		gks_lnattr;	/* Attributes for polyline */
	Gmkattr		gks_mkattr;	/* Attributes for polymarker */
	Gtxattr		gks_txattr;	/* Attributes for text */
	Gflattr		gks_flattr;	/* Attributes for fill area */
	CHATTR		gks_chattr;	/* Attributes for characters */
	PTATTR		gks_ptattr;	/* Attributes for patterns */

	Gint		cur_ntrans;	/* Current normalization transformation number */

	Gint		wiss_id;	/* Workstation id for WISS-workstation */
	Gint		open_mo;	/* Number of opened meta output files */

/* Normalization number is array index */
	NORM_TRANS   ntrans_list[MAX_TRANS+1];	

/*
 * ntrans_list[ ntrans_priority[0] ] has the highest priority
 * ntrans_list[ ntrans_priority[MAX_TRANS+1] ] has the lowest priority
 */
	int	ntrans_priority[MAX_TRANS+1];

	Gcliprec		cliprec;		/* Clipping rectangle in NDC */


/* c1142:  The following two moved from segments.c to gks state list */
	Gint	gks_open_seg;		/* name of current open seg */
	Gint	gks_pick_id;		/* current pick id */

/* c1147:  The "gks error state list" was added to the rest of the GKS state */
/*    list.                                                                  */
	ERROR_ST	gks_err_state;	/* ON=error occured OFF= no error */
	Gfile		*gks_err_file;	/* file for error messages */

	char    *fontdbdir;             /* dir to load fonts from */ /* c1159 */

} GKS_STATE_LIST;			/* end of GKS state list structure */

extern GKS_STATE_LIST	xgks_state;	/* c1147: this is the gks state list */


/* c1147:  Moved these macros down for clarity's sake. */
#define WcToNdc(w,n) { \
	/* Gpoint *w, *n; */ \
	Glimit *nt = &(xgks_state.ntrans_list[xgks_state.cur_ntrans].wc_to_ndc); \
	(n)->x = ((w)->x * nt->xmin + nt->xmax); \
	(n)->y = ((w)->y * nt->ymin + nt->ymax); \
}

#define NdcToWc(n,w) { \
	/* Gpoint *n, *w; */ \
	Glimit *nt = &(xgks_state.ntrans_list[xgks_state.cur_ntrans].wc_to_ndc); \
	(w)->x = ((n)->x - nt->xmax) / nt->xmin; \
	(w)->y = ((n)->y - nt->ymax) / nt->ymin; \
}

#define NtWcToNdc(t,w,n) { \
	/* Gint t; Gpoint *w, *n; */ \
	Glimit *nt = &(xgks_state.ntrans_list[(t)].wc_to_ndc); \
	(n)->x = ((w)->x * nt->xmin + nt->xmax); \
	(n)->y = ((w)->y * nt->ymin + nt->ymax); \
}

#define NtNdcToWc(t,n,w) { \
	/* Gint t; Gpoint *n, *w; */ \
	Glimit *nt = &(xgks_state.ntrans_list[(t)].wc_to_ndc); \
	(w)->x = ((n)->x - nt->xmax) / nt->xmin; \
	(w)->y = ((n)->y - nt->ymax) / nt->ymin; \
}

#define	MO_OPENED	(xgks_state.open_mo > 0 ? TRUE : FALSE)
/* SUN DOES NOT LIKE THIS, it says "operands of : have incompatible types"
 * #define REDRAWWS(ws)    (void)(((ws)->wsdus.irgmode == GSUPPRESSED) ? ((void)
((ws)->wsdus.nframe = GYES)) : ((void)(XgksReDrawSegWs((ws)))))
 */
#define REDRAWWS(ws) 						\
    {								\
    void XgksCleanUpWsSegList ();				\
        if ((ws)->wsdus.irgmode == GSUPPRESSED) {		\
		XgksCleanUpWsSegList ((ws));			\
                (ws)->wsdus.nframe = GYES; 			\
		}						\
        else if ((ws)->ewstype != MO) XgksReDrawSegWs((ws));	\
    }

#endif
