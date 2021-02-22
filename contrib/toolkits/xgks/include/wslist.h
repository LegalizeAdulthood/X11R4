/*
 *              Copyright IBM Corporation 1989
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
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 */

#ifndef  WSLIST_H
#define  WSLIST_H

#include <stdio.h>
#include "gks_defines.h"                /* Implementaion dependent constants and structures */
#include "primitive.h"                  /* Primitive structures */
#include "wdt.h"                        /* Workstation description table structure */


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


typedef struct {                       /* dash list type definition  */
        int dn;                        /* dash list length  */
        char dashl[17];                /* dash list         */
} DashList;

extern DashList xgksDASHES[10]; /* defined in x/xpline.c */ /*c1147*/


typedef struct ws_struct {
        Gint            ws_id;                  /* Workstation identifier */
        Gchar           *conn;                  /* Workstation Connection and type */
        Gchar           *wstype;                /* workstation type */
        EWSTYPE         ewstype;                /* enum ws type */

        Gint            ws_is_closing;          /* true = closing */ /* c1164 */

        /* UNIX - implementation of MI/MO needs the a file pointer */
        Gfile           *mfp;                   /* Need this for MI/MO */
        Gint            filestat;               /* File status for MI/MO   c1068 */
        
        /* Entries in this group do not exist for workstation of categories INPUT and MI */
        
        Gwsstate        wsstate;                /* Workstation state [ACTIVE/INACTIVE] */
        
        /* Entries in this group do not exist for workstation of categoeies INPUT, WISS, MI */
        
        Gstore          primi_store;            /* state of storage of non-segment primitives */
        Gpoint          size;                   /* Workstation DC space size */
        Gwsti           wsti;                   /* Workstation transformation information */
        Gwsdus          wsdus;                  /* Workstation defferal & update state */
        Gint            (*redrawfuncp)();       /* User defined function called each time */
                                                /* after workstation redraw */

        /* Entries in this group do not exist for workstation of categoeies INPUT, WISS, MI, MO */
        
        Glnbundl        lnbundl_table[MAX_BUNDL_TBL];   /* Polyline bundle table */
        Gmkbundl        mkbundl_table[MAX_BUNDL_TBL];   /* Polymarker bundle table */
        Gtxbundl        txbundl_table[MAX_BUNDL_TBL];   /* Text bundle table */
        Gflbundl        flbundl_table[MAX_BUNDL_TBL];   /* Fill area bundle table */
        Gptbundl        ptbundl_table[MAX_BUNDL_TBL];   /* Pattern bundle table */

        /* Segments associated with this workstation --
               this structure is very important when we want to clear a ws */
        WS_SEG_LIST  *seglist;
        WS_SEG_LIST  *seg_insertpt;
        Gint          seg_list_dirty;   /* Flag indicatinf that ws->seglist
                                           needs to be re-arrange before next gks-redraw */
        /* A note on implementation, this list will be mantain by routines in 
           segment.c and should only be change by routines in it ! */

        /* logical input devices are implemented as a linked list of 
           all the devices that have been used */
        INPUT_DEV       *in_dev_list;

        Glimit          clip;                   /* Intersection between NDC-viewport and WS_window */

        /* Following output primitive list are for non-segment primitives associated 
           with this workstation */
        OUT_PRIMI       primi_list;             /* First primitve is always a CLIP_REC */
        OUT_PRIMI       *primi_insert_pt;       /* Primitive manager needs this for efficient insertion */
        OUT_PRIMI       *message_pt;            /* Points to mesg. prim. in list;  NULL if none  PTR c1001*/
        OUT_PRIMI       *bef_message;           /* Points to node before mesg in list    PTR c1001*/

        WS_TRANS        ndctodctrans;        /* transformation constants from NDC space to DC space */

        /* Following are X_implementation dependent stuff .. */

        Display         *dpy;           /* the display ID */
        Window          win;            /* the window ID */
        unsigned long   event_mask;     /* the initial window event mask */
        GC              gc;             /* the window graphics context */
        GC              plinegc;        /* graphics contexts for specific */
        GC              pmarkgc;        /* primitives */
        GC              fillareagc;
        GC              textgc;
        Colormap        dclmp;          /* the screen default colour map ID */
        Colormap        wclmp;          /* the window colourmap ID */
        Gpoint          wbound;         /* Current x window bound */
        Gint            wscolour;       /* Number of available colour on the ws */
        Gcobundl        *set_colour_rep;/* colours set by user */ /* c1152 */
        Gint            wsfg, wsbg;     /* foreground and background pixel values */
        WS_TRANS        dctoxtrans;     /* transformation constants from DC space to X_WIN space */
        WS_TRANS        ndctoxtrans;    /* composite of ndctodc and dctox */

        XRectangle      xclip;          /* the clip area in the X window */
        XRectangle      last_pline_rectangle;
        XRectangle      last_pmarker_rectangle;
        XRectangle      last_farea_rectangle;
        XRectangle      last_text_rectangle;
        Gint            last_dash_index;
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

#define VALID_WSID(i)   ((i) >= 0)            /* c1012 */  
#define OPEN_WSID(i)    (XgksValidWsId(i))   /* c1012 */ 
#define WS_CAT(t)       (XgksWsCategory(t))

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
        }

#define XToDc(ws, xpt, dc) { \
        /* WS_STATE_ENTRY *ws; Gpoint *dc; XPoint *xpt; */  \
        (dc)->x = ((xpt)->x - (ws)->dctoxtrans.xTrans) / (ws)->dctoxtrans.xScale; \
        (dc)->y = ((ws->wbound.y - (xpt)->y) - (ws)->dctoxtrans.yTrans) / (ws)->dctoxtrans.yScale; \
        }

#define NdcToX(ws, ndc, xpt) { \
        /* WS_STATE_ENTRY *ws; Gpoint *ndc; XPoint *xpt; */  \
        (xpt)->x = (short)((ndc)->x * (ws)->ndctoxtrans.xScale + (ws)->ndctoxtrans.xTrans); \
        (xpt)->y = (short)(ws->wbound.y - ((ndc)->y * (ws)->ndctoxtrans.yScale + (ws)->ndctoxtrans.yTrans)); \
        }

#define XToNdc(ws, xpt, ndc) { \
        /* WS_STATE_ENTRY *ws; Gpoint *ndc; XPoint *xpt; */  \
        (ndc)->x = ((xpt)->x - (ws)->ndctoxtrans.xTrans) / (ws)->ndctoxtrans.xScale; \
        (ndc)->y = ((ws->wbound.y - (xpt)->y) - (ws)->ndctoxtrans.yTrans) / (ws)->ndctoxtrans.yScale; \
        }

#endif
