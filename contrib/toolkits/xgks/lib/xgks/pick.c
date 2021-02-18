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
 * pick.c - routines for XGKS PICK input device
 *    ginitpick()
 *    gsetpickmode()
 *    greqpick()
 *    gsamplepick()
 *
 *    ginqdefpick()
 *    ginqpickst()
 * 
 * $Header: pick.c,v 4.0 89/08/31 16:23:36 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/pick.c,v $
 *
 * $Log:	pick.c,v $
 * Revision 4.0  89/08/31  16:23:36  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.24  89/06/15  17:17:18  bruce
 * PTR# c1175:	Check GKSERROR condition for valid segment names.
 * 
 * Revision 3.23  89/06/05  10:17:34  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.22  89/05/22  09:23:07  bruce
 * PTR# c1177:	Modified statements to insure NULL is not dereferenced.
 * 
 * Revision 3.21  89/03/15  20:20:27  bruce
 * PTR# c2046:	insure initial status is set and
 * 		returned correctly.
 * 
 * Revision 3.20  89/02/13  13:08:19  amy
 * PTR c1145	XgksPicUpdatePrompt:  pass variable idev by value, not
 * 		reference in calling XgksFindPickSeg, to insure that highlighting
 * 		occurs appropriately in REQUEST mode.
 * 
 * Revision 3.19  89/02/04  15:27:26  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.18  88/12/20  15:14:03  amy
 * PTR c1032	gsamplepick:  call to XgksFindPickSeg.
 * 		XgksPicUpdatePrompt: call to XgksFindPickSeg changed to include
 * 		parameter to indicate if return pickid is needed.
 * 
 * Revision 3.17  88/12/16  12:40:49  amy
 * PTR c1133	XgksPicUpdatePrompt:  add event id to parameter list.
 * 		XgksEnqueueEvent calls:  add event id to parameter list.
 * 
 * Revision 3.16  88/12/08  12:23:34  amy
 * PTR c1130	GKSERROR and gerrorhand calls:  changed function name paramter
 * 		from character string to enum. type value.
 * 
 * Revision 3.15  88/11/04  10:37:54  amy
 *  PTR c1057	gsetpickmode:  set up SIGALRM signal handler if mode = GEVENT.
 * 
 * Revision 3.14  88/11/01  13:33:20  amy
 * PTR c1087	ginqdefpick:  added check for error 22, ws type invalid.
 * 
 * Revision 3.13  88/09/22  09:42:03  amy
 * MIT	XgksPicUpdatePrompt:  eliminate unused formal parameter pstate.
 * 	XgksEnqueueEvent:  cast null pointer passed in as char *.
 * 
 * Revision 3.12  88/08/18  06:51:20  amy
 * AUG  gsamplepick:  changed check for valid device, and SAMPLE mode.
 * 
 * Revision 3.11  88/08/11  08:57:01  amy
 * PTR c1012  ginitpick, ginqpickst, greqpick, gsamplepick & gsetpickmode:
 * added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.3  88/07/28  14:37:38  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:53:20  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:51  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: pick.c,v 4.0 89/08/31 16:23:36 amy Exp $";
 
#include <signal.h>		/*c1057*/
#include "gks_implem.h"                 /* d1 */
 
 
/* As under current implementation only pet #1 and #3 are being supported */
#define SUPPORTED_PICK_PROMPT(pet)    (pet==1 || pet==3)

extern  XgksAwaitInterrupt();	/*  Declare  Interrupt function   PTR# c1057  */
 
 
/*$F
 * ginitpick(ws_id, dev, init, pet, area, record) - INITIALISE PICK
 *
 * Gint     ws_id;        workstation identifier
 * int        dev;        pick device number 
 * Gpick    *init;        initial pick pointer
 * Gint        pet;        Prompt and echo type
 * Glimit   *area;        Echo area pointer in DC
 * Gpickrec *record;        pick data record pointer
 *
 * returns: 0, 7, 20, 25, 37, 51, 140, 141, 144, 145, 146, 152, 2000
 *
 * NOTE : As there's no implementation dependent data record being used,
 *        error 146 is not checked and will never occur.
 *
 * See Also: ANSI Standard p.127
 */
ginitpick(ws_id, dev, init, pet, area, record) 
Gint ws_id, dev, pet;
Gpick *init;
Glimit *area;
Gpickrec *record;
{
    WS_STATE_PTR ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
 
/* STEP 1: check for errors */
/*    gks in proper state? */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errginitpick) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginitpick)   /* c1012 */

/* open workstation ? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR ((!(ws=OPEN_WSID(ws_id))) , 25, errginitpick)  /* c1012 */
 
/* check category */
    GKSERROR ((WS_CAT(ws) != GOUTIN) , 37, errginitpick)
 
/* rectangle defintion ok */
    GKSERROR ((area->xmin >= area->xmax || area->ymin >= area->ymax) , 51, errginitpick)
 
/* valid pick device number */
    GKSERROR ((dev < 1), 140, errginitpick)
 
/* prompt and echo type supported */
    GKSERROR ((!SUPPORTED_PICK_PROMPT(pet)) , 144, errginitpick)
 
/* echo area within display space */
    GKSERROR ((area->xmin<0 || area->xmax>ws->size.x || area->ymin<0 || area->ymax>ws->size.y) , 145, errginitpick)
 
/* initial values ok */
/* pick id can take on any value */
    GKSERROR ((init->seg < 1) , 152, errginitpick)	/* c1175 */
 
    GKSERROR ((init->status != GP_OK && init->status != GP_NOPICK &&
        init->status != GP_NONE) , 2000, errginitpick)
 
/* Check if the device already exist */
    if ( (idev = XgksIDevLookup(ws, dev, GPICK)) == NULL) {
    /* Build a new input device structure */
        gcvalues.function = GXxor;
        idev = XgksIDevNew ();
        idev->gc = XCreateGC (ws->dpy, ws->win, GCFunction, &gcvalues);
        idev->class = GPICK;
        idev->dev = dev;
        idev->data.pic.initst.mode = GREQUEST;    /* initialize to GREQUEST */
        idev->data.pic.initst.esw  = GECHO;
    /* Add into workstation input device queue */
        XgksIDevAdd (ws, idev);
    } else {
    /* pick device must be in REQUEST mode */
        GKSERROR ((idev->data.pic.initst.mode!=GREQUEST), 141, errginitpick)
    }
    idev->data.pic.initst.pick = *init;
    idev->data.pic.initst.pet = pet;
    idev->data.pic.initst.e_area = *area;
    idev->data.pic.initst.record = *record;
    return(0);
}

/*$F
 * gsetpickmode(ws_id, dev, mode, echo) - SET PICK MODE
 *
 * Gint ws_id;            workstation identifier
 * Gint    dev;            pick device number
 * Gimode mode,            operating mode (GREQUEST | GSAMPLE | GEVENT)
 * Gesw  echo;            echo switch (GECHO | GNOECHO)
 *
 * Returns: 0, 7, 20, 25, 37, 140, 143, 2000
 *
 * See Also: ANSI Standard p.130
 */
gsetpickmode(ws_id, dev, mode, echo)
Gint ws_id, dev;
Gimode mode;
Gesw echo;
{
    WS_STATE_PTR ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
 
/* STEP 1: check for errors */
/*     gks in proper state */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgsetpickmode) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetpickmode)   /* c1012 */

/* workstation id valid and open */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR ((!(ws=OPEN_WSID(ws_id))) , 25, errgsetpickmode)  /* c1012 */
 
/* check category */
    GKSERROR ((WS_CAT(ws) != GOUTIN) , 37, errgsetpickmode)
 
/* valid pick device */
    GKSERROR ((dev < 1) , 140, errgsetpickmode)
 
/* check enumerated type values */
    GKSERROR (((mode != GREQUEST && mode != GSAMPLE && mode != GEVENT) || 
           (echo != GECHO && echo != GNOECHO) ) , 2000, errgsetpickmode)
 
/* STEP 2: tell the workstation */
    if ( (idev = XgksIDevLookup(ws, dev, GPICK)) == NULL ) {
    /* We have to create one with default values */
#ifdef IDEVPICKDEBUG
    fprintf(stderr, "gsetpickmode : device does not exist ..ws_id=%d, dev=%d\n",ws_id, dev);
#endif
        idev = XgksIDevNew();
        idev->class = GPICK;
        idev->dev = dev;
        gcvalues.function = GXxor;
        idev->gc = XCreateGC(ws->dpy, ws->win, GCFunction, &gcvalues);
        idev->data.pic.initst.mode = GREQUEST;
        idev->data.pic.initst.esw  = GECHO;
        idev->data.pic.initst.pet = 1;
        idev->data.pic.initst.pick.status = GP_NOPICK; /* c2046 */
        idev->data.pic.initst.pick.seg = INVALID;
        idev->data.pic.initst.pick.pickid = INVALID;
        idev->data.pic.initst.e_area.xmin = 0.0;
        idev->data.pic.initst.e_area.xmax = ws->size.x;
        idev->data.pic.initst.e_area.ymin = 0.0;
        idev->data.pic.initst.e_area.ymax = ws->size.y;
        idev->data.pic.initst.record.pet1.data = NULL; 
        XgksIDevAdd (ws, idev);
    }
    idev->data.pic.initst.mode = mode;
    idev->data.pic.initst.esw  = echo;
 
    if ( mode == GEVENT )
       signal( SIGALRM, XgksAwaitInterrupt); /* Set signal handler for event mode  PTR# c1057 */
    if ( mode == GSAMPLE || mode == GEVENT ) idev->active = TRUE;
 
    return(0);
}

/*$F
 * greqpick(ws_id, dev, respons) - REQUEST PICK
 *
 * Gint ws_id;            workstation identifier
 * Gint    dev;            pick device number
 * Gpick *respons        Response of the pick
 *      status -- GP_OK, GP_NOPICK, GP_NONE
 *      seg    -- picked seg id
 *      pickid -- pickid
 *
 * returns: 0, 7, 20, 25, 37, 140, 141 
 *
 * See Also: ANSI Standard p.134 
 */ 
greqpick(ws_id, dev, response)
Gint ws_id,dev;
Gpick *response;
{
    WS_STATE_PTR ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
 
/* STEP 1: check for errors */
/*     gks in proper state */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgreqpick) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgreqpick)   /* c1012 */

/* workstation id open */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR ((!(ws=OPEN_WSID(ws_id))) , 25, errgreqpick)  /* c1012 */
 
/* check category */
    GKSERROR ((WS_CAT(ws) != GOUTIN) , 37, errgreqpick)
 
/* valid pick device */
    GKSERROR ((dev < 1) , 140, errgreqpick)
 
/* ask the workstation for the device */
        if ( (idev = XgksIDevLookup(ws, dev, GPICK)) == NULL ) {
        /* We have to create one with default values */
#ifdef IDEVPICKDEBUG
    fprintf(stderr, "greqpick : device does not exist ..ws_id=%d, dev=%d\n",ws_id, dev);
#endif
                idev = XgksIDevNew();
                idev->class = GPICK;
                idev->dev = dev;
                gcvalues.function = GXxor;
                idev->gc = XCreateGC(ws->dpy, ws->win, GCFunction, &gcvalues);
                idev->data.pic.initst.mode = GREQUEST;
                idev->data.pic.initst.esw  = GECHO;
                idev->data.pic.initst.pet = 1;
                idev->data.pic.initst.pick.status = GP_NOPICK; /* c2046 */
                idev->data.pic.initst.pick.seg = INVALID;
                idev->data.pic.initst.pick.pickid = INVALID;
                idev->data.pic.initst.e_area.xmin = 0.0;
                idev->data.pic.initst.e_area.xmax = ws->size.x;
                idev->data.pic.initst.e_area.ymin = 0.0;
                idev->data.pic.initst.e_area.ymax = ws->size.y;
                idev->data.pic.initst.record.pet1.data = NULL; 
        XgksIDevAdd (ws, idev);
        } 
    else {
        GKSERROR ((idev->data.pic.initst.mode !=GREQUEST), 141, errgreqpick)
    }
 
/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );
 
    idev->active = TRUE;
 
/* wait for trigger or break */
    idev->touched = FALSE;
    idev->breakhit = FALSE;
    while ( (idev->touched == FALSE) && (idev->breakhit == FALSE) )
        sigpause (0);
 
    idev->active = FALSE;
 
    *response = idev->data.pic.initst.pick;
 
    if (idev->breakhit == TRUE) response->status = GP_NONE;
 
    return(0);
}
/*$F
 * gsamplepick(ws_id, dev, response) - SAMPLE PICK
 *
 * Gint ws_id;            workstation identifier
 * Gint    dev;            pick device number
 * Gpick *respons        Response of the pick
 *      status -- GP_OK, GP_NOPICK, GP_NONE
 *      seg    -- picked seg id
 *      pickid -- pickid
 *
 * Returns: 0, 7, 20, 25, 37, 140, 142
 *
 * See Also: ANSI Standard p.137
 */
gsamplepick(ws_id, dev, response)
Gint ws_id, dev;
Gpick *response;
{
    WS_STATE_PTR ws;
    INPUT_DEV *idev;
    Gpoint    ndcpt;
 
/* STEP 1: check for errors */
/*     gks in proper state */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgsamplepick) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsamplepick)   /* c1012 */

/* workstation id open */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR ((!(ws=OPEN_WSID(ws_id))) , 25, errgsamplepick)  /* c1012 */
 
/* check category */
    GKSERROR ((WS_CAT(ws) != GOUTIN) , 37, errgsamplepick)
 
/* valid pick device */
    GKSERROR ((dev<1), 140, errgsamplepick)			/* AUG */
    idev=XgksIDevLookup(ws, dev, GPICK);			/* AUG */
 
/* is current mode SAMPLE ? */
   GKSERROR ((idev==NULL) || (idev->data.pic.initst.mode != GSAMPLE), 142, errgsamplepick)	/* AUG */
 
/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );
 
/* Grep the current position in pick data record and figure out the segment name */
    if (idev->breakhit == TRUE) return (OK);
 
    DcToNdc (ws, &(idev->data.pic.curpos), &ndcpt);
    XgksFindPickSeg(ws, &ndcpt, response,idev,2);           /* c1032 */
 
    return(0);
}
 
/*    Pick inquiries */
 
/*$F
 * ginqpickst(ws_id, dev, type, state) - INQUIRE PICK DEVICE STATE
 * Gint  ws_id        workstation identifier
 * Gint  dev;        pick device number
 * Gqtype type;         type of return value
 * Gpickst *state    Oitput of current pick state
 *
 * Errors: 7, 20, 25, 37, 140, 2000
 *
 * See Also: ANSI Standard p.169
 */
ginqpickst (ws_id, dev, type, state)
Gint ws_id, dev;
Gqtype type;
Gpickst *state;
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqpickst ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqpickst)   /* c1012 */

/* check for ws_id, if correspond to opened ws */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginqpickst )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN), 37, errginqpickst)
 
/* check enumeration */
    GKSERROR( (type != GSET && type != GREALIZED ), 2000, errginqpickst )
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errginqpickst )
 
/* Copy the data to the user's structure */
    if ((idev = XgksIDevLookup( ws, dev, GPICK )) == NULL) {
         state->mode = GREQUEST;
                state->esw  = GECHO;
                state->pet = 1;
                state->pick.status = GP_NOPICK; /* c2046 */
                state->pick.seg = INVALID;
                state->pick.pickid = INVALID;
                state->e_area.xmin = 0.0;
                state->e_area.xmax = ws->size.x;
                state->e_area.ymin = 0.0;
                state->e_area.ymax = ws->size.y;
                state->record.pet1.data = NULL; 
    }
    else {
        *state = idev->data.pic.initst;
        /* if idev->data.loc.initst.record pointed anywhere, it would
         * be copied here.
         */
    }
 
    return( OK );
}
 
/*$F
 *
 * ginqdefpick(type, dev, data) - INQUIRE DEFAULT DEVICE DATA
 * Gchar *type;      workstation type string
 * Gint  dev;        pick device number
 * Gdefpick *data;    output default pick device data structure
 *
 * Returns all information in the parameters.
 * Errors: 0, 8, 22, 23, 38, 140
 *
 * See Also: ANSI Standard p.188
 */
ginqdefpick(type, dev ,data)
Gchar *type; 
Gint dev;
Gdefpick *data;
{
    EWSTYPE ewstype;
    int i;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqdefpick )	/*c1147*/
 
/* valid wsid? */
    ewstype = XgksWsTypeToEnum( type );
    GKSERROR( (ewstype == WST_INVALID), 22, errginqdefpick )	/*c1087*/
 
/* valid workstation type (assumes all INPUT and OUTIN workstations are X_WIN */
    GKSERROR( ewstype != X_WIN, 38, errginqdefpick)
 
/* valid locator device? */
    GKSERROR( (dev < 1), 140, errginqdefpick)
 
/* STEP 2: set up the return values */
    data->pets.number    = 2;
    data->pets.integers    = (Gint *) malloc( sizeof( Gint ) * data->pets.number);
    if ( data->pets.integers==NULL) {
        free( data );
        gerrorhand( 300, errginqdefpick, xgks_state.gks_err_file);	/*c1147*/
        return( 300 );
    }
    for( i=0; i<2; i++)
        data->pets.integers[i] = i+1;
 
    data->e_area.xmin    = 0.0;
    data->e_area.xmax    = WS_MAX_DCX;
    data->e_area.ymin    = 0.0;
    data->e_area.ymax    = WS_MAX_DCY;
    data->record.pet1.data    = NULL;
 
    return(OK);
}
/*
 * XgksPicUpdatePrompt - update the locator prompt
 */
XgksPicUpdatePrompt( ws, idev, newdcpt, xmev, event_id ) /*MIT*/ /* PTR c1133 */
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    Gpoint *newdcpt;
    XMotionEvent *xmev;
    int event_id;       /* PTR c1133 */
{
    Gpick  *data;
    Gpoint ndcpt;
    XRectangle rect;
 
    rect.x = 0; rect.y = 0;
    rect.width = ws->wbound.x; rect.height = ws->wbound.y;
 
    XSetClipRectangles(ws->dpy, idev->gc, 0, 0, &rect, 1, Unsorted);
 
/* pick point must lie within the workstation window */
    DcToNdc( ws, newdcpt, &ndcpt);
 
#define IPICK idev->data.pic.initst.pick
    switch (idev->data.pic.initst.mode) {
        case GREQUEST:
            if (xmev == NULL || xmev->type != ButtonRelease)  /* c1177 */
                break;
            idev->touched = TRUE;
            if (idev->breakhit == TRUE) 
                IPICK.status = GP_NONE;
     	    else 
               XgksFindPickSeg(ws, &ndcpt, &(IPICK),idev,1); /* c1032 c1145 */
            break;
        case GSAMPLE : 
            idev->data.pic.curpos = *newdcpt;
	    if (xmev == NULL || xmev->type != ButtonRelease)	/* c1032 */ /*c1177*/
                break;                                     /* c1032 */
            XgksFindPickSeg(ws, &ndcpt,&(IPICK),idev,0);   /* c1032 */
#undef IPICK
            break;
        case GEVENT  :
            if (xmev == NULL || xmev->type != ButtonRelease)  /* c1177 */
                break;
            data = (Gpick *) malloc( sizeof (Gpick) );
            if (data == NULL)
	    { 
                gerrorhand( 300, errXgksPicUpdatePrompt, xgks_state.gks_err_file); /*c1147*/
		return(300);
	    }
            else {
                if (idev->breakhit == TRUE) 
                    data->status = GP_NONE;
                else {if (XgksFindPickSeg(ws,&ndcpt,data,idev,1)== GP_OK) { /*c1032*/
                    data->status = GP_OK;
                     } else data->status = GP_NOPICK; }
                 XgksEnqueueEvent(ws->ws_id, idev->dev, GPICK, 
                   (char *)data,-1); /*MIT*/ /* PTR c1133 */
            }
            break;
        default :
            break;
    }
#ifdef notdef
    /* XgksFindPickSeg () will always return pickid of the first non-clip
       primitive in the segment */
    /* This should be undefed... after prompt #2 is implemented */
 
    if (idev->data.pic.initst.pet == 2) {
        Call a funtion to figure out exact pickid from 
        the <ndcpt> and <segment> (returned from  XgksFindPickSeg)
    */
#endif
            
 
    XFlush(ws->dpy);
    return( 0 );
}
/*
 * XgksPicDelete -- Free Everything in the pick-device structure ...
 *             BUT, BUT not the pointer to the structure itself
 *            calling program still needs that
 */
XgksPicDelete (ws, idev)
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
{
    XFreeGC( ws->dpy, idev->gc );
}
