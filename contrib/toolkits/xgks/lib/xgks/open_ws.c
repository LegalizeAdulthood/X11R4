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
 * Urbana, IL    61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * gopenws - functions for opening a new workstation
 *            gopenws()
 *            gclosews()
 *
 */

#include <string.h>
#include "gks_implem.h"

static void XgksDeleteOpenWs(Gint ws_id);
static int XgksSetWsPtr(Gint ws_id, WS_STATE_PTR ws);
static int XgksAllocNewWs(Gint ws_id);
static WS_STATE_PTR XgksNewWs(void);
static int XgksOneOpenWs(void);
void XgksReDrawWs(WS_STATE_PTR ws, OUT_PRIMI *primi);
void XgksDrawToWs(WS_STATE_PTR ws, OUT_PRIMI *primi);
Gint XgksMiOpenWs(WS_STATE_PTR ws);
Gint XgksMiCloseWs(WS_STATE_PTR ws);
Gint XgksMoOpenWs(WS_STATE_PTR ws);
Gint XgksMoCloseWs(WS_STATE_PTR ws);
Gint XgksMoClearWs(WS_STATE_PTR ws, Gclrflag control_flag);
void XgksInitWssPlines(WS_STATE_PTR ws);
void XgksInitWssPmarkers(WS_STATE_PTR ws);
void XgksInitWssFillArea(WS_STATE_PTR ws);
void XgksInitWssText(WS_STATE_PTR ws);
void XgksInitWssTrans(WS_STATE_PTR ws);
void XgksInitIDev(WS_STATE_PTR ws);
void XgksIDevDelete(WS_STATE_PTR ws);
void XgksIDevDisable(WS_STATE_PTR ws);
void XgksIDevEnable(WS_STATE_PTR ws);
void XgksDeleteAllSeg(WS_STATE_PTR ws);
void XgksDelAllMoSeg(WS_STATE_PTR ws);
void XgksUnpendPendingTrans(WS_STATE_PTR ws);
void XgksDeletePrimi(OUT_PRIMI *head, OUT_PRIMI **insert_pt);
void XgksReDrawSeg(WS_STATE_PTR ws, Gint seg_id);
void XgksUpdateWsClip(WS_STATE_PTR ws, Glimit *bound);
void XgksInsertMesgPrimi(WS_STATE_PTR ws, OUT_PRIMI *primi);
void XgksInsertPrimi(OUT_PRIMI **insert_pt, OUT_PRIMI *elm);
void XgksWsWinInterset(WS_STATE_PTR ws, Glimit *v, Glimit *clip);

Gint xXgksOpenWs(WS_STATE_PTR wk);
Gint xXgksCloseWs(WS_STATE_PTR ws);
void xXgksClearWs(WS_STATE_PTR ws);
void xXgksUpdateClip(WS_STATE_PTR ws);
Gint xXgksMesg(WS_STATE_PTR ws, MESG_ST *mesg);
Gint xXgksCellarray(WS_STATE_PTR ws, CELL_ARRAY_ST *cell_ptr);
Gint xXgksFillArea(WS_STATE_PTR ws, FILL_AREA_ST *fill_ptr);
Gint xXgksPolyLine(WS_STATE_PTR ws, PLINE_ST *plin_ptr);
Gint xXgksPolyMarker(WS_STATE_PTR ws, PMARK_ST *pmk_ptr);
Gint xXgksText(WS_STATE_PTR ws, TEXT_ST *text_ptr);

/*$F
 * gopenws(ws_id, connection, ws_type) - OPEN WORKSTATION
 *
 * Gint  ws_id;            User selected workstation identifier.
 * Gchar *connection        Not implemented YET !!
 * Gchar *ws_type;        Which workstation type.
 *
 * Returns: 0, 8, 20, 21, 22, 23, 24, 26, 28, 42
 *
 * See also: ANSI standard p.74
 */

Gint gopenws(Gint ws_id, Gchar *connection, Gchar *ws_type)
{

    WS_STATE_PTR ws;    /* workstation state list */
    EWSTYPE ewstype;    /* corresponding enum-valued ws_type */
    char *getenv();
    int status;         /* AUG */

/* check proper state */
    GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgopenws)           /*c1147*/

/* DWO 7/26/88  changed inline check ws_id < 0 to macro call for PTR c1012 */
/* check for valid ws_id  ..  ws_id must be positive */
    GKSERROR ((!VALID_WSID(ws_id)) ,20, errgopenws)  /* c1012 */

/* Check for valid connection */
    if (connection == (char *)NULL)
        connection = getenv( "DISPLAY" );

    GKSERROR ((STRLEN(connection) == 0) ,21, errgopenws)        /* c1176 */

/* check valid ws_type */
        if (ws_type == (char *)NULL)
                ws_type = connection; /* if type==NULL type = connection  c1078 */

        GKSERROR ((STRLEN(ws_type) == 0) ,23, errgopenws)       /* c1176 */

    ewstype = XgksWsTypeToEnum( ws_type );
    GKSERROR ( (ewstype == WST_INVALID) ,23, errgopenws)

/* check workstation already open */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR ((OPEN_WSID(ws_id) != NULL) ,24, errgopenws)  /* c1012 */

/* update <openedws> array in gks_state list */
    GKSERROR ((XgksAllocNewWs(ws_id) != 0) ,42, errgopenws)

/* create the new workstation list structure */
    GKSERROR ((!(ws=XgksNewWs())) ,300, errgopenws)

    ws->ws_id = ws_id;
    ws->wsstate = GINACTIVE;
    ws->wstype = (Gchar *) malloc( STRLEN( ws_type ) +1);       /* c1176 */
    GKSERROR( ws->wstype == NULL ,300, errgopenws)
    STRCPY( ws->wstype, ws_type);       /* c1176 */
    ws->ewstype = ewstype;
    ws->ws_is_closing = FALSE;  /* not in the process of closing */ /* c1164 */

    GKSERROR(((ws->conn = (Gchar *)malloc((unsigned)(STRLEN(connection) +1))) == NULL) ,300, errgopenws)        /* c1176 */
    STRCPY((ws->conn), connection);     /* c1176 */

    XgksSetWsPtr(ws_id, ws);        /* add this ws ptr into openedws[] array */
    ws->dpy = NULL;                 /* c1167 */
    switch (ewstype) {
        case X_WIN : /* open the workstation as X-11 window */
                      if ((status=xXgksOpenWs(ws)) != 0) {      /* AUG */ /*c1147*/
                free((char *)ws->conn);
                free((char *)ws);
                XgksDeleteOpenWs(ws_id);    /* update openedws[] array */
             gerrorhand(status, errgopenws, xgks_state.gks_err_file); /* AUG *//*c1147*/
                return(status);                                 /* c1096 */
                      }
                      break;
        case MI    : if ( (ws->mfp=fopen(ws->conn, "r")) == NULL) {
                free((char *)ws->conn);
                free((char *)ws);
                XgksDeleteOpenWs(ws_id);    /* update openedws[] array */
                gerrorhand(26, errgopenws, xgks_state.gks_err_file);    /*c1147*/
                return(26);
                      }
                GKSERROR ((XgksMiOpenWs ( ws ) != 0), 26, errgopenws) /* c1068 */
                break;
        case MO    : if ( (ws->mfp=fopen(ws->conn, "w")) == NULL) {
                free((char *)ws->conn);
                free((char *)ws);
                XgksDeleteOpenWs(ws_id);    /* update openedws[] array */
                gerrorhand(26, errgopenws, xgks_state.gks_err_file);    /*c1147*/
                return(26);
                      }
                 XgksMoOpenWs ( ws );
                 xgks_state.open_mo++;                                  /*c1147*/
                 break;
        case WISS  : /* If opening WISS, make sure there's no other WISS being opened */
                      GKSERROR ((xgks_state.wiss_id != INVALID), 28, errgopenws); /*c1147*/
                      xgks_state.wiss_id = ws_id;                       /*c1147*/
                      break;
    }

    if (xgks_state.gks_state == GGKOP)                                   /*c1147*/
        xgks_state.gks_state = GWSOP;    /* change state to 1 ws open */ /*c1147*/

    XgksInitWssPlines(ws);        /* init POLYLINE BUNDLE TABLE */
    XgksInitWssPmarkers(ws);    /* POLYMARKER BUNDLE TABLE */
    XgksInitWssFillArea(ws);    /* FILL AREA BUNDLE */
    XgksInitWssText(ws);        /* TEXT BUNDLE TABLE */

/* Only meaningful for X_WIN */
    if (ewstype == X_WIN) {
        XgksInitIDev( ws );        /* initialize input devices */
        XgksInitWssTrans(ws);        /* workstation transformation */
    }

    return(OK);
}

/*$F
 * gclosews(ws_id) - CLOSE WORKSTATION
 *
 * Gint ws_id;            Workstation to close.
 *
 * Returns: 0, 7, 20, 25, 29, 147
 *
 *         Error 147 (EventQueue OverFlow) is not checked
 *
 * See also: ANSI standard p.75
 */

Gint gclosews(Gint ws_id)
{

    WS_STATE_PTR ws;

/* check gks in proper state */
    GKSERROR ((xgks_state.gks_state==GGKOP || xgks_state.gks_state==GGKCL) ,7, errgclosews) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgclosews)   /* c1012 */

/* check for workstation not open yet */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgclosews)  /* c1012 */

/* check for workstation still active */
    GKSERROR ((ws->wsstate == GACTIVE) ,29, errgclosews)

/* delete all input devices associated with this ws */
/* NOTE: This is what caused ptr c1120 (key presses as ws closes cause */
/*       X error).  New events entered after the gflushevents but      */
/*       before the ws was completely gone.  This left an event in the */
/*       queue pointing to a non-existant ws.  Also - the devices on a */
/*       workstation were never being handled when the workstation     */
/*       closed.  All the memory dynamically allocated for devices was */
/*       being lost.  So... we call XgksIDevDelete to destroy the      */
/*       devices so no new events will enter and also free the memory  */
/*       used by the devices.  (XgksIDevDelete was written, but never  */
/*       called.  This seems to be the place it was meant for.) (DWO)  */
    if (ws->ewstype != MO && ws->ewstype != WISS) /* c1167 */   /* c1139 */
        XgksIDevDelete (ws);                 /* PTR c1120 */    /* c1139 */

/* XgksDeleteAllSeg was moved after XgksIDevDelete to stop the devices     */
/* from flashing as the ws closes.  XgksDeleteAllSeg calls XgksIDevDisable */
/* which erases all the devices, then deletes the segments, then calls     */
/* XgksIDevEnable to redraw all the devices.  This caused them to flash.   */
/* Now, the devices are deleted before the segments.                       */
/*      (Done while fixing ptr c1023  -  DWO)                              */
/* delete all segments from this ws, and at the same time   */  /* c1141 */
/* delete this ws from all segments associated with it      */  /* c1141 */
/* (XgksDelAllWsSeg would be a better name)                 */  /* c1141 */
    if (ws->ewstype != MO)                                      /* c1139 */
        XgksDeleteAllSeg (ws);               /* PTR c1023 */    /* c1139 */
    else                                                        /* c1141 */
        XgksDelAllMoSeg( ws );                                  /* c1141 */

/* mark the ws as closing to prevent any interrupts to jump */ /* c1164 */
/* in while the ws is in an unstable state.  (if a change   */ /* c1164 */
/* focus came in after the xXgksCloseWs, but before the     */ /* c1164 */
/* completion of gclosews, GKS used to try to inquire the   */ /* c1164 */
/* color map of an X window that was already gone)  DWO     */ /* c1164 */
    ws->ws_is_closing = TRUE;                                  /* c1164 */

/* House keeping for workstation consistency */
    switch (ws->ewstype) {
        case X_WIN : /* tell x-window system to destroy the window */
                 xXgksCloseWs(ws);                              /*c1147*/
                 /* Flush the event queue for this workstation */
                 gflushevents (ws_id, GNCLASS, 0);
                 break;
        case MI    : XgksMiCloseWs (ws);
                 break;
        case MO    : XgksMoCloseWs (ws);
                 xgks_state.open_mo--;                          /*c1147*/
                 break;
        case WISS  : xgks_state.wiss_id = INVALID;              /*c1147*/
                 break;
    }

/* take workstation out of list and free it up */
    XgksDeleteOpenWs (ws_id);        /* remove ws from openedws array in gkslist */
    free(ws->conn);
    free(ws->wstype);
    free((char *)ws);

/* clean up stuff */
    if (XgksOneOpenWs() == FALSE )    /* that was the last workstation open */
        xgks_state.gks_state = GGKOP; /* set state to gks open no ws open */ /*c1147*/

/* everything went ok */
    return(OK);
}
/*$F
 * gclearws(ws_id, control_flag) - CLEAR WORKSTATION
 *
 * Gint ws_id;            workstation to clear
 * Gclrflag control_flag;    clear display GCONDITIONALLY | GALWAYS
 *
 * returns: 0 = OK, or one of 6, 20, 25, 33, 35
 *
 * See also: ANSI standard p.77
 */
Gint gclearws(Gint ws_id, Gclrflag control_flag)
{
    WS_STATE_PTR ws;

/* check for proper operating state */
    GKSERROR ((xgks_state.gks_state != GWSOP && xgks_state.gks_state != GWSAC) ,6, errgclearws) /*c1147*/

/* DWO 7/26/88  changed inline check ws_id < 0 to macro call for PTR c1012 */
/* Check for valid ws_id  ..  ws_id must be positive */
    GKSERROR ((!VALID_WSID(ws_id)) ,20, errgclearws)  /* c1012 */

/* check for valid ws_id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgclearws)  /* c1012 */

/* check for proper category */
    GKSERROR ((WS_CAT(ws) == GMI) ,33, errgclearws)

    GKSERROR ((WS_CAT(ws) == GINPUT),35, errgclearws)

    if (ws->ewstype == MO)
        XgksMoClearWs (ws, control_flag);

/* STEP 2: tell the workstation */
    if (control_flag == GCONDITIONALLY && ws->wsdus.dspsurf == GEMPTY)
        return (OK);

    if (ws->ewstype != MO) {                                    /* c1139 */
        XgksIDevDisable( ws );                                  /* c1139 */
        xXgksClearWs(ws);                                       /* c1139 *//*c1147*/
    }                                                           /* c1139 */
    if (ws->wsti.wstus == GPENDING) XgksUnpendPendingTrans(ws);

    if (ws->ewstype != MO) {                                          /* c1139 */
        XgksDeletePrimi (&(ws->primi_list), &(ws->primi_insert_pt));  /* c1139 */
        ws->message_pt = NULL;    /* Clear any messages   PTR c1001*/ /* c1139 */
        XgksDeleteAllSeg (ws);                                        /* c1139 */
        XgksIDevEnable( ws );                                         /* c1139 */
    }                                                                 /* c1139 */

    ws->wsdus.nframe = GNO;
    ws->wsdus.dspsurf = GEMPTY;

    return (OK);
}
/*
 * XgksSetWsPtr (ws_id, ws)
 *  Gint    ws_id;
 *  WS_STATE_PTR  ws;
 *
 * set the entry in openedws[].ws_id = ws_id  of gkslist to corresponding ptr
 *
 *
 */
static int XgksSetWsPtr (ws_id, ws)
Gint  ws_id;
WS_STATE_PTR ws;
{
   Gint i;

   for (i=0; i<MAX_OPEN_WS; i++)
         if (xgks_state.openedws[i].ws_id == ws_id) {                   /*c1147*/
        xgks_state.openedws[i].ws = ws;                                 /*c1147*/
        return(OK);
    }
   return (INVALID);
}
/*
 * XgksAllocNewWs (ws_id)
 *  Gint  ws_id;
 *
 * tries to allocate a slot in openedws[] in gks state list and assign ws_id into the slot
 *
 * return INVALID if there's no empty slot
 *
 */
static int XgksAllocNewWs (ws_id)
Gint ws_id;
{
   Gint i;

   for (i=0; i<MAX_OPEN_WS; i++)
         if (xgks_state.openedws[i].ws_id == INVALID) {                 /*c1147*/
        xgks_state.openedws[i].ws_id = ws_id;                           /*c1147*/
        return(OK);
    }
   return (INVALID);
}
/*
 * XgksDeleteOpenWs (ws_id)
 *   Gint ws_id;
 *
 * delete <ws_id> from openedws[] in gks state list by setting corresponding entry to INVALID
 *
 */
static void XgksDeleteOpenWs (ws_id)            /*MIT*/
Gint  ws_id;
{
    Gint i;

    for (i=0; i<MAX_OPEN_WS; i++) {
        if (xgks_state.openedws[i].ws_id == ws_id) {                    /*c1147*/
            xgks_state.openedws[i].ws_id = INVALID;                     /*c1147*/
            xgks_state.openedws[i].win = INVALID;                       /*c1147*/
            xgks_state.openedws[i].ws = NULL;                           /*c1147*/
            return;                     /*MIT*/
        }
    }
}
/* WS_STATE_PTR
 * XgksValidWsId (ws_id)
 *   Gint  ws_id;
 *
 * See if <ws_id> is already opened in the openedws[] array, if yes return pointer to
 *   the ws_state_list, else return NULL
 *
 */
WS_STATE_PTR XgksValidWsId(Gint ws_id)
{
    Gint i;

    for (i=0; i<MAX_OPEN_WS; i++) {
        if (xgks_state.openedws[i].ws_id == ws_id)  return(xgks_state.openedws[i].ws); /*c1147*/
    }
    return (NULL);
}

/* WS_STATE_PTR
 * XgksNewWs ()
 *
 *   Allocate a new ws entry
 *
 */
static WS_STATE_PTR XgksNewWs(void)
{
    WS_STATE_PTR new;

    if ((new = (WS_STATE_PTR)malloc((unsigned) sizeof(WS_STATE_ENTRY))) != NULL) {
        new->ws_id = 0;
        new->wsstate = GINACTIVE;
        new->conn    = NULL;
        new->mfp     = NULL;
        new->clip = xgks_state.cliprec.rec;             /*MIT*/ /*c1147*/
        new->primi_list.pid = CLIP_REC;
        new->primi_list.seg_cnt = 0;            /*MIT*/
        new->primi_list.primi.clip.rec = xgks_state.cliprec.rec;/*c1147*/
        new->primi_list.primi.clip.segment = FALSE;
        new->primi_list.next = NULL;
        new->primi_insert_pt = &(new->primi_list);
        new->message_pt = NULL;    /* Initialize message ptr    PTR# c1001 */
        new->seglist = NULL;
        new->seg_insertpt = NULL;
        new->seg_list_dirty = FALSE;
        new->size.x = WS_MAX_DCX;
        new->size.y = WS_MAX_DCY;
        new->primi_store = GYES;
        new->redrawfuncp = NULL;
    }
    return(new);
}
/*
 * XgksOneOpenWs ()
 *          return TRUE if there is at least one open ws
 *          else return FALSE
 */
static int XgksOneOpenWs(void)
{
    Gint i;

    for (i=0; i<MAX_OPEN_WS; i++) {
        if (xgks_state.openedws[i].ws_id != INVALID)  return(TRUE);     /*c1147*/
    }
    return (FALSE);
}
/*
 * Convert a workstation type string to the enumerated version
 */
EWSTYPE XgksWsTypeToEnum(Gchar *ws_type)
{
    if (STRCMP(ws_type, "MI") == 0)                return( MI );        /* c1176 */
    else if (STRCMP(ws_type, "MO") == 0)            return( MO );       /* c1176 */
    else if (STRCMP(ws_type, "WISS") == 0)            return( WISS );   /* c1176 */
    else if (ws_type == NULL)                return( X_WIN );
    else if (index( ws_type, ':') != 0 )            return( X_WIN );
    else return( WST_INVALID );
}

/*
 * XgksWsCategory (ws)
 * WS_STATE_PTR ws;  - return the categorey of the workstation pointed
 *               by the state pointer
 */
Gwscat XgksWsCategory(WS_STATE_PTR ws)
{
    if (ws->ewstype == X_WIN) return (GOUTIN);
    else if (ws->ewstype == WISS) return (GWISS);
    else if (ws->ewstype == MI) return (GMI);
    else if (ws->ewstype == MO) return (GMO);
    else return ((Gwscat) INVALID);
}

/*
 * XgksXReDrawWs (ws)
 *  WS_STATE_PTR ws;        this is an X initiated redraw.
 */
void XgksXReDrawWs(WS_STATE_PTR ws)
{
    OUT_PRIMI *primi= &(ws->primi_list);
    WS_SEG_LIST *seg_ptr= ws->seglist;
    Gint     cnt;

    Glimit tmp_wsclip;    /* Have to save current ws clip value */
    Glimit tmp_nonsegclip;    /* Have to save current non_segment clip value */

    tmp_wsclip = ws->clip;    /* Save the current ws->clip value */
    while (primi != NULL) {
        XgksReDrawWs (ws, primi);
        tmp_nonsegclip = ws->clip;    /* save clip region for non-segment primitives */
        cnt = primi->seg_cnt;
        while (cnt-- > 0) {
            XgksReDrawSeg (ws, seg_ptr->seg);
            seg_ptr = seg_ptr->next;
        }
        ws->clip = tmp_nonsegclip;    /* restore non-segment primitive clip regions */
        xXgksUpdateClip(ws);                                    /*c1147*/
        primi = primi->next;
    }
    ws->clip = tmp_wsclip;    /* restore the old ws->clip value */
    xXgksUpdateClip(ws);                                        /*c1147*/
}

/*
 * XgksOutputToWs (primi)
 * OUT_PRIMI *primi    - output primitive to all, ALL active outputable workstations
 *
 */
void XgksOutputToWs(OUT_PRIMI *primi)
{
    Gint cnt;

    for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {         /* send to all active workstations */
        if (xgks_state.activews[cnt].ws_id != INVALID) {                /*c1147*/
            switch (xgks_state.activews[cnt].ws->ewstype) {             /*c1147*/
            case X_WIN:    XgksDrawToWs(xgks_state.activews[cnt].ws, primi); /*c1147*/
                    break;
            case MO:
            case MI:
            case WISS:    break;
            }
        }
    }
}

/* XgksReDrawWs (ws, primi)
 * WS_STATE_PTR ws;        -- decode primi->pid and call the corresponding X-routine
 * OUT_PRIMI *primi;           to draw out the primitive to window
 *
 * Well not disable any input devices
 */
void XgksReDrawWs (ws, primi)
WS_STATE_PTR ws;
OUT_PRIMI *primi;
{
    switch (primi->pid) {
        case PLINE     : xXgksPolyLine (ws, &(primi->primi.pline));     /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                       break;
        case PMARK     : xXgksPolyMarker (ws, &(primi->primi.pmark));   /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case FILL_AREA  : xXgksFillArea (ws, &(primi->primi.fill_area));/*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case CLIP_REC   : if (primi->primi.clip.segment == TRUE) {
                    XgksUpdateWsClip(ws, &(primi->primi.clip.rec));
                  } else {
                    ws->clip = primi->primi.clip.rec;
                      xXgksUpdateClip (ws);                     /*c1147*/
                  }
                  break;
        case TEXT      : xXgksText (ws, &(primi->primi.text));  /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case MESG      : xXgksMesg (ws, &(primi->primi.mesg));  /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case CELL_ARRAY : xXgksCellarray(ws, &(primi->primi.cell_array)); /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case GDP    : break;
    }
}

/* XgksDrawToWs (ws, primi)
 * WS_STATE_PTR ws;        -- decode primi->pid and call the corresponding X-routine
 * OUT_PRIMI *primi;           to draw out the primitive to window
 *
 * Well disable all input devices before output and Enable them AFTER output
 */
void XgksDrawToWs(WS_STATE_PTR ws, OUT_PRIMI *primi)
{
    XgksIDevDisable(ws);    /* Disable all input devices */
    switch (primi->pid) {
        case PLINE     : xXgksPolyLine (ws, &(primi->primi.pline));     /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                       break;
        case PMARK     : xXgksPolyMarker (ws, &(primi->primi.pmark));   /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case FILL_AREA  : xXgksFillArea (ws, &(primi->primi.fill_area));/*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case CLIP_REC   : if (primi->primi.clip.segment == TRUE) {
                     XgksUpdateWsClip (ws, &(primi->primi.clip.rec));
                  } else {
                      ws->clip = primi->primi.clip.rec;
                      xXgksUpdateClip (ws);                     /*c1147*/
                  }
                  break;
        case TEXT      : xXgksText (ws, &(primi->primi.text));  /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case MESG      : xXgksMesg (ws, &(primi->primi.mesg));  /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case CELL_ARRAY : xXgksCellarray(ws, &(primi->primi.cell_array)); /*c1147*/
                  ws->wsdus.dspsurf = GNOTEMPTY;
                  break;
        case GDP    : break;
    }
    XgksIDevEnable(ws);        /* Disable all input devices */
}

/*
 * XgksAppendWsPrimi (primi)
 * OUT_PRIMI *primi;     - append the primitive entry to workstation non segment list
 *
 */
void XgksAppendWsPrimi(OUT_PRIMI *primi)
{
    Gint  cnt;

    for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {      /* send to all active workstations */
        if ( (xgks_state.activews[cnt].ws_id != INVALID)
                && (xgks_state.activews[cnt].ws->primi_store == GYES)
                && (xgks_state.activews[cnt].ws->ewstype != MO)) /* c1139 *//*c1147*/
            if (primi->pid == MESG)   /* Check if prim. is MESG        c1001 */
                XgksInsertMesgPrimi(xgks_state.activews[cnt].ws, primi);    /*c1147*/
            else          /* message primitives have to be inserted differently */
               XgksInsertPrimi(&(xgks_state.activews[cnt].ws->primi_insert_pt), primi); /*c1147*/
    }
}

/*
 * XgksAppendWsClip () - check if opened_ws->primi_insert_pt->pid == CLIP_REC
 *               if yes, update clip.rec to cliprec.rec, else
 *               append a new CLIP_REC to the list
 */
void XgksAppendWsClip(Glimit *rec)
{
    Gint cnt;
    OUT_PRIMI *clip;

    for (cnt=0; cnt < MAX_OPEN_WS; cnt++) {
        if (xgks_state.openedws[cnt].ws_id != INVALID) {                /*c1147*/
        if (xgks_state.openedws[cnt].ws->primi_insert_pt->pid == CLIP_REC) { /*c1147*/
                XgksWsWinInterset(xgks_state.openedws[cnt].ws, rec, &(xgks_state.openedws[cnt].ws->primi_insert_pt->primi.clip.rec)); /*c1147*/
         } else {
            clip = XgksNewPrimi();
            clip->pid = CLIP_REC;
            clip->primi.clip.segment = FALSE;
                XgksWsWinInterset(xgks_state.openedws[cnt].ws, rec, &(clip->primi.clip.rec)); /*c1147*/
                XgksInsertPrimi(&(xgks_state.openedws[cnt].ws->primi_insert_pt), clip); /*c1147*/
             }
            XgksUpdateWsClip (xgks_state.openedws[cnt].ws, rec);        /*c1147*/
        }
    }
}
