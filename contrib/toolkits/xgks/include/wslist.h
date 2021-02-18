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
 * $Header: wslist.h,v 4.0 89/08/31 16:35:47 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/wslist.h,v $
 *
 * $Log:	wslist.h,v $
 * Revision 4.0  89/08/31  16:35:47  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.18  89/03/15  16:36:27  amy
 * PTR c1164	Added ws_is_closing field to workstation state list
 * 		structure.
 * 
 * Revision 3.17  89/02/20  13:52:19  amy
 * PTR c1152	Added set_colour_rep pointer to ws state list.
 * 
 * Revision 3.16  89/02/04  15:33:51  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.15  88/12/08  17:12:37  amy
 * Changed Colourmap back to Colormap.
 * 
 * Revision 3.14  88/12/07  15:06:20  amy
 * Changed spellings from color to colour.
 * 
 * Revision 3.13  88/11/18  14:15:13  amy
 * PTR c1068	Added the filestat field to the workstation state list to
 * 	retain metafile error condition flags (see changes to metafile.c for
 * 	this PTR).
 * 
 * Revision 3.12  88/09/26  09:54:40  amy
 * MIT	DcToX, NdcToX:  cast float as short before assigning to another short.
 * 
 * Revision 3.11  88/08/19  13:45:02  amy
 * AUG  The following fields were added to the WS_STATE_PTR structure:
 *        plinegc,
 *        pmarkgc
 *        fillareagc
 *        textgc
 *        last_pline_rectangle
 *        last_pmarker_rectangle
 *        last_farea_rectangle
 *        last_text_rectangle
 *        last_dash_index
 * 
 * Revision 3.10  88/08/15  06:52:48  amy
 * PTR c1001  WS state table modified for message problem fix.
 * 
 * Revision 1.3  88/08/08  15:47:34  bruce
 * PTR# c1001 - Two fields had to be added to the WS state table to manage 
 * workstation messages.
 * 
 * Revision 1.2  88/07/26  15:01:18  owens
 * DWO c1012: changed name of VALID_WSID to OPEN_WSID and added new VALID_WSID
 * 
 * Revision 1.1  88/07/21  10:47:32  david
 * Initial revision
 *  
 *
 */

#ifndef  WSLIST_H
#define  WSLIST_H

#include <stdio.h>
#include "gks_defines.h"		/* Implementaion dependent constants and structures */
#include "primitive.h"			/* Primitive structures */
#include "wdt.h"			/* Workstation description table structure */


/*
 * NDC to DC, DC to X, and NDC to X transformations are stored in the
 * workstation structure using the WS_TRANS structure.
 */
typedef struct {
	Gfloat xScale, xTrans;
	Gfloat yScale, yTrans;
} WS_TRANS;

/* EWSTYPE - enum workstation type */
typedef enum {
	WST_INVALID,
	X_WIN,
	WISS,
	MI,
	MO
} EWSTYPE;


typedef struct {		       /* dash list type definition  */
	int dn;			       /* dash list length  */
	char dashl[17];		       /* dash list         */
} DashList;

extern DashList xgksDASHES[10];	/* defined in x/xpline.c */ /*c1147*/


typedef struct ws_struct {
	Gint		ws_id;			/* Workstation identifier */
	Gchar		*conn;			/* Workstation Connection and type */
	Gchar		*wstype;		/* workstation type */
	EWSTYPE		ewstype;		/* enum ws type */

	Gint		ws_is_closing;		/* true = closing */ /* c1164 */

	/* UNIX - implementation of MI/MO needs the a file pointer */
	Gfile		*mfp;			/* Need this for MI/MO */
	Gint		filestat;		/* File status for MI/MO   c1068 */
	
	/* Entries in this group do not exist for workstation of categories INPUT and MI */
	
	Gwsstate	wsstate;		/* Workstation state [ACTIVE/INACTIVE] */
	
	/* Entries in this group do not exist for workstation of categoeies INPUT, WISS, MI */
	
	Gstore		primi_store;		/* state of storage of non-segment primitives */
	Gpoint 		size;			/* Workstation DC space size */
	Gwsti		wsti;			/* Workstation transformation information */
	Gwsdus		wsdus;			/* Workstation defferal & update state */
	Gint		(*redrawfuncp)();	/* User defined function called each time */
						/* after workstation redraw */

	/* Entries in this group do not exist for workstation of categoeies INPUT, WISS, MI, MO */
	
	Glnbundl	lnbundl_table[MAX_BUNDL_TBL];	/* Polyline bundle table */
	Gmkbundl	mkbundl_table[MAX_BUNDL_TBL];	/* Polymarker bundle table */
	Gtxbundl	txbundl_table[MAX_BUNDL_TBL];	/* Text bundle table */
	Gflbundl	flbundl_table[MAX_BUNDL_TBL];	/* Fill area bundle table */
	Gptbundl	ptbundl_table[MAX_BUNDL_TBL];	/* Pattern bundle table */

	/* Segments associated with this workstation --
	       this structure is very important when we want to clear a ws */
	WS_SEG_LIST  *seglist;
	WS_SEG_LIST  *seg_insertpt;
	Gint	      seg_list_dirty;	/* Flag indicatinf that ws->seglist
					   needs to be re-arrange before next gks-redraw */
	/* A note on implementation, this list will be mantain by routines in 
	   segment.c and should only be change by routines in it ! */

	/* logical input devices are implemented as a linked list of 
	   all the devices that have been used */
	INPUT_DEV	*in_dev_list;

	Glimit		clip;			/* Intersection between NDC-viewport and WS_window */

	/* Following output primitive list are for non-segment primitives associated 
	   with this workstation */
	OUT_PRIMI 	primi_list;		/* First primitve is always a CLIP_REC */
	OUT_PRIMI 	*primi_insert_pt;	/* Primitive manager needs this for efficient insertion */
        OUT_PRIMI       *message_pt;            /* Points to mesg. prim. in list;  NULL if none  PTR c1001*/
        OUT_PRIMI       *bef_message;           /* Points to node before mesg in list    PTR c1001*/

	WS_TRANS	ndctodctrans;        /* transformation constants from NDC space to DC space */

	/* Following are X_implementation dependent stuff .. */

	Display		*dpy;           /* the display ID */
	Window		win;            /* the window ID */
	unsigned long	event_mask;     /* the initial window event mask */
	GC		gc;             /* the window graphics context */
	GC              plinegc;        /* graphics contexts for specific  AUG */
        GC              pmarkgc;        /* primitives 			   AUG */
        GC              fillareagc;					/* AUG */
        GC              textgc;						/* AUG */
	Colormap        dclmp;          /* the screen default colour map ID */
	Colormap        wclmp;          /* the window colourmap ID */
	Gpoint		wbound;		/* Current x window bound */
	Gint		wscolour;	/* Number of available colour on the ws */
	Gcobundl        *set_colour_rep;/* colours set by user */ /* c1152 */
	Gint		wsfg, wsbg;	/* foreground and background pixel values */
	WS_TRANS	dctoxtrans;     /* transformation constants from DC space to X_WIN space */
	WS_TRANS	ndctoxtrans;	/* composite of ndctodc and dctox */

	XRectangle      xclip;          /* the clip area in the X window */
	XRectangle      last_pline_rectangle;		/* AUG */	
        XRectangle      last_pmarker_rectangle;		/* AUG */
        XRectangle      last_farea_rectangle;		/* AUG */
        XRectangle      last_text_rectangle;		/* AUG */
        Gint            last_dash_index;		/* AUG */
        Gint            last_message_width;   
} *WS_STATE_PTR, WS_STATE_ENTRY ;

#define NOT_SET (-9.99)

Gwscat XgksWsCategory();
EWSTYPE XgksWsTypeToEnum( /* Gchar *wstype */ );
WS_STATE_PTR XgksValidWsId();

/* DWO 7/26/88  VALID_WSID(i) used to check to see if there was a workstation open with     */
/*              the name i.  I renamed this function OPEN_WSID and created a new VALID_WSID */
/*              that checks if the ws id is valid (non-negative).  This was all done in the */
/*              name of PTR c1012: many functions returned error 25 (ws not open) in cases  */
/*              where error 20 (ws invalid) would be more appropriate.                      */

#define VALID_WSID(i)	((i) >= 0)            /* c1012 */  
#define OPEN_WSID(i)    (XgksValidWsId(i))   /* c1012 */ 
#define WS_CAT(t)	(XgksWsCategory(t))

#define NdcToDc(ws, ndc, dc) { \
	/* WS_STATE_ENTRY *ws; Gpoint *ndc, *dc; */  \
	(dc)->x = (ndc)->x * (ws)->ndctodctrans.xScale + (ws)->ndctodctrans.xTrans; \
	(dc)->y = (ndc)->y * (ws)->ndctodctrans.yScale + (ws)->ndctodctrans.yTrans; \
	} 

#define DcToNdc(ws, dc, ndc) { \
	/* WS_STATE_ENTRY *ws; Gpoint *dc, *ndc; */  \
	(ndc)->x = ((dc)->x - (ws)->ndctodctrans.xTrans) / (ws)->ndctodctrans.xScale; \
	(ndc)->y = ((dc)->y - (ws)->ndctodctrans.yTrans) / (ws)->ndctodctrans.yScale; \
	}

#define DcToX(ws, dc, xpt) { \
	/* WS_STATE_ENTRY *ws; Gpoint *dc; XPoint *xpt; */  \
	(xpt)->x = (short)((dc)->x * (ws)->dctoxtrans.xScale + (ws)->dctoxtrans.xTrans); \
	(xpt)->y = (short)(ws->wbound.y - ((dc)->y * (ws)->dctoxtrans.yScale + (ws)->dctoxtrans.yTrans)); \
	} 	/*MIT*/

#define XToDc(ws, xpt, dc) { \
	/* WS_STATE_ENTRY *ws; Gpoint *dc; XPoint *xpt; */  \
	(dc)->x = ((xpt)->x - (ws)->dctoxtrans.xTrans) / (ws)->dctoxtrans.xScale; \
	(dc)->y = ((ws->wbound.y - (xpt)->y) - (ws)->dctoxtrans.yTrans) / (ws)->dctoxtrans.yScale; \
	}

#define NdcToX(ws, ndc, xpt) { \
	/* WS_STATE_ENTRY *ws; Gpoint *ndc; XPoint *xpt; */  \
	(xpt)->x = (short)((ndc)->x * (ws)->ndctoxtrans.xScale + (ws)->ndctoxtrans.xTrans); \
	(xpt)->y = (short)(ws->wbound.y - ((ndc)->y * (ws)->ndctoxtrans.yScale + (ws)->ndctoxtrans.yTrans)); \
	}	/*MIT*/ 

#define XToNdc(ws, xpt, ndc) { \
	/* WS_STATE_ENTRY *ws; Gpoint *ndc; XPoint *xpt; */  \
	(ndc)->x = ((xpt)->x - (ws)->ndctoxtrans.xTrans) / (ws)->ndctoxtrans.xScale; \
	(ndc)->y = ((ws->wbound.y - (xpt)->y) - (ws)->ndctoxtrans.yTrans) / (ws)->ndctoxtrans.yScale; \
	}

#endif
