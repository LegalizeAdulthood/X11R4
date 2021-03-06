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
 * Locator logical input device functions
 * 
 * $Header: locator.c,v 4.1 89/09/07 11:49:12 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/locator.c,v $
 *
 * $Log:	locator.c,v $
 * Revision 4.1  89/09/07  11:49:12  amy
 * PTR #c1175	ginitloc:  fixed check for error 152 to compare
 * 		initial position passed in to WC limits of transformation
 * 		passed in.
 * 
 * Revision 4.0  89/08/31  16:22:20  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.30  89/07/06  16:31:39  amy
 * No change.
 * 
 * Revision 3.29  89/06/15  17:16:37  bruce
 * PTR# c1175:	Added a check in ginitloc to check initial locator position.
 * 
 * Revision 3.28  89/06/05  10:17:14  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.27  89/05/22  09:44:26  bruce
 * PTR# c1174:	Changed macro call to WS_FILL_TYPE because of argument change
 * 		when negative hatch styles were added.
 * 
 * Revision 3.26  89/02/04  15:25:47  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.25  88/12/19  17:10:49  amy
 * PTR c1148	ginitloc, gsetlocmode, and greqloc:  replace malloc with a
 * 		call to XgksIDevNew.
 * 
 * Revision 3.24  88/12/16  12:37:28  amy
 * PTR c1133	XgksLocUpdatePrompt:  add event id to parameter list.
 * 		XgksEnqueueEvent calls:  add event id to parameter list.
 * 
 * Revision 3.23  88/12/08  11:06:12  amy
 * PTR c1130	GKSERROR and gerrorhand calls:  changed function name parameter
 * 		from character string to enum. type value.
 * Changed spelling to color to colour.
 * 
 * Revision 3.22  88/11/15  17:31:52  amy
 * New 4.3 C compiler--
 * 	ginitloc:  WS_AVAIL_COLOUR using gcvalues.foreground, cast the parameter
 * 		as an int, to avoid compiler warning message.
 * 		Initialize didx (dash index) to 0.
 * 
 * Revision 3.21  88/11/04  10:48:58  amy
 * PTR c1057	gsetlocmode:  set up SIGALRM signal handler if mode = GEVENT.
 * 
 * Revision 3.20  88/11/01  13:31:38  amy
 * PTR c1087	ginqdefloc:  added check for error 22, ws type invalid.
 * 
 * Revision 3.19  88/09/21  13:19:28  amy
 * MIT	Call XgksLocUpdatePrompt with some value for every formal parameter.
 * 	greqloc, gsampleloc, XgksLocUpdatePrompt:  declare global functions.
 * 	Call XgksEnqueueEvent with some value for every formal parameter.
 * 
 * Revision 3.18  88/08/17  13:06:01  amy
 * AUG  gsampleloc:  changed check for valid input device number.
 *      gsampleloc:  changed check for SAMPLE mode.
 * 
 * Revision 3.17  88/08/11  08:32:13  amy
 * PTR c1012  ginitloc, ginqlocst, greqloc, gsampleloc & gsetlocmode:
 * added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.5  88/07/28  14:29:43  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.4  88/07/27  13:44:49  bruce
 * PTR# c1013  Code was added or changed to check for errors 63
 * and 64 correctly.
 * 
 * Revision 1.3  88/07/26  17:52:41  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.2  88/07/26  16:49:54  bruce
 * Checked in for DWO
 * 
 * Revision 1.1  88/07/21  10:49:21  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: locator.c,v 4.1 89/09/07 11:49:12 amy Exp $";
 
#include "gks_implem.h"                 /* d1 */
#include <signal.h>		/*c1057*/

extern  XgksAwaitInterrupt();	/*  Declare  Interrupt function   PTR# c1057  */
 
/*
 * Initialise Locator 
 *
 * Returns: 0, 7, 25, 38, 51, 60, 64, 65, 80, 83, 85, 93, 140, 141, 144, 145, 300, 2000
 */
 
ginitloc( ws_id, dev, init, pet, area, record)
    Gint ws_id;        /* workstation identifier */
    Gint dev;        /* locator device number */
    Gloc *init;        /* initial locator pointer */
    Gint pet;        /* prompt and echo type */
    Glimit *area;        /* echo area pointer */
    Glocrec *record;    /* locator data record pointer */
{
#define  TRANSFORM   xgks_state.ntrans_list

    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
    int didx;        /* Dash InDeX */

    didx = 0;        /* Initialize dash index */
    
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginitloc ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginitloc)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginitloc )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errginitloc)
 
/* valid echo area */
    GKSERROR( (area->xmin > area->xmax || area->ymin > area->ymax), 51, errginitloc )
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errginitloc )
 
/* valid and supported prompt mode? */
    GKSERROR( pet>5, 144, errginitloc)
    GKSERROR( (pet==0) || (pet>5), 144, errginitloc)
    if (pet<0) pet = 1;
 
/* Echo inside display space? */
    GKSERROR( (area->xmin < 0 || area->xmax > ws->size.x
        || area->ymin < 0 || area->ymax > ws->size.y),
        145, errginitloc )
 
/* Valid initial locator position?              c1175 */
    GKSERROR ((init->position.x < TRANSFORM[init->transform].ntrans.w.xmin ||
               init->position.x > TRANSFORM[init->transform].ntrans.w.xmax ||
               init->position.y < TRANSFORM[init->transform].ntrans.w.ymin ||
               init->position.y > TRANSFORM[init->transform].ntrans.w.ymax),
               152, errginitloc)    


/* CHECK PROMPT DEPENDENT DATA */
    switch( pet ) {
    case 1:        /* none */
    case 2:
    case 3:
        gcvalues.function = GXinvert;
        gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = 0;
        gcvalues.line_style = LineSolid;
        break;
    case 4:
        /* valid attribute flag */
        GKSERROR( ((record->pet4.acf != GCURRENT) && (record->pet4.acf != GSPECIFIED) ),
            2000, errginitloc)
        
        if (record->pet4.acf == GSPECIFIED) {
            GKSERROR( (record->pet4.ln.type != GINDIVIDUAL) && (record->pet4.ln.type != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet4.ln.width != GINDIVIDUAL) && (record->pet4.ln.width != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet4.ln.colour != GINDIVIDUAL) && (record->pet4.ln.colour != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet4.ln.line < 1) || (record->pet4.ln.line >= MAX_BUNDL_TBL),
                60, errginitloc)
            GKSERROR( (record->pet4.ln.bundl.type == 0), 63, errginitloc) /* c1013 */
            GKSERROR( ! WS_LINE_TYPE(record->pet4.ln.bundl.type), 64, errginitloc)
            GKSERROR( ! WS_AVAIL_COLOUR(ws, (record->pet4.ln.bundl.colour)), 93, errginitloc)
            GKSERROR( (record->pet4.ln.bundl.width < 0.0), 65, errginitloc)
        }
        else {
            record->pet4.ln.type    = xgks_state.gks_lnattr.type;	/*c1147*/
            record->pet4.ln.width    = xgks_state.gks_lnattr.width;	/*c1147*/
            record->pet4.ln.colour    = xgks_state.gks_lnattr.colour;	/*c1147*/
            record->pet4.ln.line    = xgks_state.gks_lnattr.line;	/*c1147*/
            record->pet4.ln.bundl    = xgks_state.gks_lnattr.bundl;	/*c1147*/
        }
#define LNBUNDLE    ws->lnbundl_table[record->pet4.ln.line]
        gcvalues.function = GXinvert;
        gcvalues.foreground = (record->pet4.ln.colour == GBUNDLED)?
            LNBUNDLE.colour : record->pet4.ln.bundl.colour;
        if ( ! WS_AVAIL_COLOUR( ws, (int)gcvalues.foreground))
            gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = (int)((record->pet4.ln.width == GBUNDLED)?
            LNBUNDLE.width : record->pet4.ln.bundl.width);
        gcvalues.line_style = (int)((record->pet4.ln.type == GBUNDLED)?
            LNBUNDLE.type : record->pet4.ln.bundl.type);
        didx = gcvalues.line_style;
        if (didx < 0 ) didx += 3;
        else if (didx > 0) didx += 1;
        /* if (didx == 0) doesn't matter */
 
        gcvalues.line_style = (gcvalues.line_style == GLN_SOLID) ?
            LineSolid : LineOnOffDash;
#ifdef LOCDEBUG
fprintf(stderr, "initloc() pet 4, style %s\n",
(gcvalues.line_style == LineOnOffDash) ? "LineOnOffDash":"LineSolid");
#endif
#undef LNBUNDLE
            
        break;
    case 5:
        /* valid attribute flag */
        GKSERROR( ((record->pet5.acf != GCURRENT) && (record->pet5.acf != GSPECIFIED) ),
            2000, errginitloc)
        /* valid echo primitive */
        GKSERROR( ((record->pet5.pfcf != GPF_POLYLINE) && (record->pet5.pfcf != GPF_FILLAREA) ),
            2000, errginitloc)
        if (record->pet5.pfcf == GPF_POLYLINE ) {
            if (record->pet5.acf == GSPECIFIED ) {
            GKSERROR( (record->pet5.attr.ln.type != GINDIVIDUAL) && (record->pet5.attr.ln.type != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet5.attr.ln.width != GINDIVIDUAL) && (record->pet5.attr.ln.width != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet5.attr.ln.colour != GINDIVIDUAL) && (record->pet5.attr.ln.colour != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet5.attr.ln.line < 1) || (record->pet5.attr.ln.line >= MAX_BUNDL_TBL),
                60, errginitloc)
            GKSERROR( record->pet5.attr.ln.bundl.type == 0, 63, errginitloc)  /* c1013 */
            GKSERROR( ! WS_LINE_TYPE(record->pet5.attr.ln.bundl.type), 64, errginitloc)
            GKSERROR( ! WS_AVAIL_COLOUR (ws, (record->pet5.attr.ln.bundl.colour)), 93, errginitloc)
            GKSERROR( (record->pet5.attr.ln.bundl.width < 0.0), 65, errginitloc)
            }
            else {
            record->pet5.attr.ln.type    = xgks_state.gks_lnattr.type;	   /*c1147*/
            record->pet5.attr.ln.width    = xgks_state.gks_lnattr.width;   /*c1147*/
            record->pet5.attr.ln.colour    = xgks_state.gks_lnattr.colour; /*c1147*/
            record->pet5.attr.ln.line    = xgks_state.gks_lnattr.line;     /*c1147*/
            record->pet5.attr.ln.bundl    = xgks_state.gks_lnattr.bundl;   /*c1147*/
            }
 
#define LNBUNDLE    ws->lnbundl_table[record->pet5.attr.ln.line]
            gcvalues.function = GXinvert;
            gcvalues.foreground = (record->pet5.attr.ln.colour == GBUNDLED)?
            LNBUNDLE.colour : record->pet5.attr.ln.bundl.colour;
            if ( ! WS_AVAIL_COLOUR( ws, (int)gcvalues.foreground ))
              gcvalues.foreground = ws->wsfg;
            gcvalues.background = ws->wsbg;
            gcvalues.line_width = (int)((record->pet5.attr.ln.width == GBUNDLED)?
            LNBUNDLE.width : record->pet5.attr.ln.bundl.width);
            gcvalues.line_style = (int)((record->pet5.attr.ln.type == GBUNDLED)?
            LNBUNDLE.type : record->pet5.attr.ln.bundl.type);
            didx = gcvalues.line_style;
            if (didx < 0 ) didx += 3;
            else if (didx > 0) didx += 1;
            /* if (didx == 0) doesn't matter */
 
            gcvalues.line_style = (gcvalues.line_style == GLN_SOLID) ?
            LineSolid : LineOnOffDash;
#ifdef LOCDEBUG
fprintf(stderr, "initloc() pet 5, style %s\n",
(gcvalues.line_style == LineSolid) ? "LineSolid" : "LineOnOffDash");
#endif
#undef LNBUNDLE
            
        }
        else if ( record->pet5.pfcf == GPF_FILLAREA ) {
            if (record->pet5.acf == GSPECIFIED ) {
            GKSERROR( (record->pet5.attr.fl.inter != GINDIVIDUAL) && (record->pet5.attr.fl.inter != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet5.attr.fl.style != GINDIVIDUAL) && (record->pet5.attr.fl.style != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet5.attr.fl.colour != GINDIVIDUAL) && (record->pet5.attr.fl.colour != GBUNDLED),
                2000, errginitloc)
            GKSERROR( (record->pet5.attr.fl.fill <0 ) || (record->pet5.attr.fl.fill >= MAX_BUNDL_TBL ),
                80, errginitloc)
                        GKSERROR( (record->pet5.attr.fl.bundl.inter != GHOLLOW)
                                && (record->pet5.attr.fl.bundl.inter != GSOLID)
                                && (record->pet5.attr.fl.bundl.inter != GPATTERN)
                                && (record->pet5.attr.fl.bundl.inter != GHATCH),
                                83, errginitloc)
            GKSERROR( ! WS_FILL_TYPE(record->pet5.attr.fl.bundl.inter,
				     record->pet5.attr.fl.bundl.style), 85, errginitloc)    /* c1174 */
            GKSERROR( ! WS_AVAIL_COLOUR (ws, (record->pet5.attr.fl.bundl.colour)), 93, errginitloc)
            }
            else {
            record->pet5.attr.fl.inter    = xgks_state.gks_flattr.inter;   /*c1147*/
            record->pet5.attr.fl.style    = xgks_state.gks_flattr.style;   /*c1147*/
            record->pet5.attr.fl.colour    = xgks_state.gks_flattr.colour; /*c1147*/
            record->pet5.attr.fl.fill    = xgks_state.gks_flattr.fill;	   /*c1147*/
            record->pet5.attr.fl.bundl    = xgks_state.gks_flattr.bundl;   /*c1147*/
            }
            /* ONLY SUPPORTING HOLLOW FILLS */
#define FLBUNDLE    ws->flbundl_table[record->pet5.attr.fl.fill]
            gcvalues.function = GXinvert;
            gcvalues.foreground = (record->pet5.attr.fl.colour == GBUNDLED)?
            FLBUNDLE.colour : record->pet5.attr.fl.bundl.colour;
            if ( ! WS_AVAIL_COLOUR( ws, (int)gcvalues.foreground ))
               gcvalues.foreground = ws->wsfg;
            gcvalues.background = ws->wsbg;
            gcvalues.line_width = 0;
            gcvalues.line_style = LineSolid;
#undef FLBUNDLE
        }
        break;
    default:
        break;
    }
 
    if ((idev = XgksIDevLookup( ws, dev, GLOCATOR )) == NULL) {
    /* Create the Input Device structure */
        idev = XgksIDevNew();   /*c1148*/
        GKSERROR( (idev == NULL), 300, errginitloc)
        idev->class = GLOCATOR;
        idev->dev = dev;
        idev->active = False;
        if ((gcvalues.line_style == LineSolid) && (gcvalues.line_width == 1))
            gcvalues.line_width--;
#ifdef LOCDEBUG
fprintf(stderr, "width %d, style %s didx %d\n", gcvalues.line_width, (gcvalues.line_style == LineSolid)?"LineSolid":"LineOnOffDash", didx);
#endif
        idev->gc = XCreateGC( ws->dpy, ws->win, 
            GCFunction | GCForeground |GCBackground | GCLineWidth
            | GCLineStyle, &gcvalues);
        if (gcvalues.line_style == LineOnOffDash) {
#ifdef LOCDEBUG
fprintf(stderr, "XSetDashes(): gc %d didx %d dash length %d dashes %d %d %d\n", idev->gc, didx,
xgksDASHES[didx].dn,  /*c1147*/
xgksDASHES[didx].dashl[0], xgksDASHES[didx].dashl[1], xgksDASHES[didx].dashl[2]); /*c1147*/
#endif
            XSetDashes( ws->dpy, idev->gc, 0, xgksDASHES[didx].dashl, xgksDASHES[didx].dn); /*c1147*/
        }
        idev->data.loc.initst.mode = GREQUEST;
        idev->data.loc.initst.esw = GECHO;
 
    /* link the new device into the list */
        idev->next = ws->in_dev_list;
        ws->in_dev_list = idev;
    }
    else {
    /* if the device is not in REQUEST mode, not allowed to initialize it */
        GKSERROR( idev->data.loc.initst.mode != GREQUEST, 141, errginitloc )
        if ((gcvalues.line_style == LineSolid) && (gcvalues.line_width == 1))
            gcvalues.line_width--;
        XChangeGC( ws->dpy, idev->gc,
            GCFunction | GCForeground | GCBackground | GCLineWidth
            | GCLineStyle, &gcvalues);
        if (gcvalues.line_style == LineOnOffDash)
            XSetDashes( ws->dpy, idev->gc, 0, xgksDASHES[didx].dashl, xgksDASHES[didx].dn); /*c1147*/
    }
    idev->data.loc.initst.loc = *init;
    idev->data.loc.initst.pet = pet;
    idev->data.loc.initst.e_area = *area;
    idev->data.loc.initst.record = *record;
 
    return( OK );
}
 
/*
 *  SET LOCATOR MODE
 *
 * Returns: 0, 7, 25, 38, 140, 300, 2000
 */
 
gsetlocmode( ws_id, dev, mode, echo )
    Gint ws_id;        /* workstation identifier */
    Gint dev;        /* locator device number */
    Gimode mode;        /* operating mode */
    Gesw echo;        /* echo switch */
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsetlocmode ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetlocmode)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgsetlocmode )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgsetlocmode)
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errgsetlocmode )
 
/* check enumerations */
    GKSERROR( ((mode != GREQUEST && mode != GSAMPLE && mode != GEVENT)
            || (echo != GECHO && echo != GNOECHO)),
        2000, errgsetlocmode )
 
    if ((idev = XgksIDevLookup( ws, dev, GLOCATOR )) == NULL) {
    /* Create the Input Device structure */
        idev = XgksIDevNew();   /*c1148*/
        GKSERROR( (idev == NULL), 300, errgsetlocmode)
    /* load it with the default values */
        idev->class = GLOCATOR;
        idev->dev = dev;
        idev->active = False;
        gcvalues.function = GXinvert;
        gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = 0;
        gcvalues.line_style = LineSolid;
        idev->gc = XCreateGC( ws->dpy, ws->win, 
            GCFunction | GCForeground | GCBackground | GCLineWidth
            | GCLineStyle, &gcvalues);
        idev->data.loc.initst.mode = GREQUEST;
        idev->data.loc.initst.esw = GECHO;
        idev->data.loc.initst.loc.transform = 0;
        idev->data.loc.initst.loc.position.x = 0.5;
        idev->data.loc.initst.loc.position.y = 0.5;
        idev->data.loc.initst.pet = 1;
        idev->data.loc.initst.e_area.xmin = 0.0;
        idev->data.loc.initst.e_area.xmax = ws->size.x;
        idev->data.loc.initst.e_area.ymin = 0.0;
        idev->data.loc.initst.e_area.ymax = ws->size.y;
        idev->data.loc.initst.record.pet1.data = NULL;
 
    /* link the new device into the list */
        idev->next = ws->in_dev_list;
        ws->in_dev_list = idev;
    }
    else {
        if ((idev->active == True) && (idev->data.loc.initst.esw == GECHO))
            XgksLocUpdatePrompt( ws, idev, PROMPTOFF, (Gpoint *)NULL,
		(XMotionEvent *)NULL,-1);  /*MIT*/ /* PTR c1133 */
    }
 
    idev->data.loc.initst.mode = mode;
    idev->data.loc.initst.esw = echo;
 
    if ( mode == GSAMPLE || mode == GEVENT ) {
        Gpoint ndcpt;
 
        NtWcToNdc( idev->data.loc.initst.loc.transform, &idev->data.loc.initst.loc.position, &ndcpt );
        NdcToDc( ws, &ndcpt, &idev->data.loc.initpos);
        idev->data.loc.curpos = idev->data.loc.initpos;
	if ( mode == GEVENT )
	   signal( SIGALRM, XgksAwaitInterrupt); /* Set signal handler for event mode  PTR# c1057 */
        idev->active = True;
        if ( echo == GECHO )
            XgksLocUpdatePrompt( ws, idev, PROMPTON, (Gpoint *)NULL,
		(XMotionEvent *)NULL,-1);  /*MIT*/ /* PTR c1133 */
    }
    else    /* GREQUEST */
        idev->active = False;
 
    return( OK );
}
 
/*
 *  REQUEST LOCATOR
 *
 * Returns: 0, 7, 20, 25, 38, 140, 141, 300
 */
greqloc( ws_id, dev, response )
    Gint ws_id;        /* workstation identifier */
    Gint dev;        /* locator device number */
    Gqloc *response;    /* OUT locator response */
{
	/*
	 * declare global functions
	 */
        void XgksFindNTrans();		/*MIT*/

    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
    Gpoint ndcpt;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgreqloc ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgreqloc)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgreqloc )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgreqloc)
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errgreqloc )
 
/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );
 
    if ((idev = XgksIDevLookup( ws, dev, GLOCATOR )) == NULL) {
    /* Create the Input Device structure */
        idev = XgksIDevNew();   /*c1148*/
        GKSERROR( (idev == NULL), 300, errgreqloc)
    /* load it with the default values */
        idev->class = GLOCATOR;
        idev->dev = dev;
        idev->active = False;
        gcvalues.function = GXinvert;
        gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = 0;
        gcvalues.line_style = LineSolid;
        idev->gc = XCreateGC( ws->dpy, ws->win, 
            GCFunction | GCForeground | GCBackground | GCLineWidth
            | GCLineStyle, &gcvalues);
        idev->data.loc.initst.mode = GREQUEST;
        idev->data.loc.initst.esw = GECHO;
        idev->data.loc.initst.loc.transform = 0;
        idev->data.loc.initst.loc.position.x = 0.5;
        idev->data.loc.initst.loc.position.y = 0.5;
        idev->data.loc.initst.pet = 1;
        idev->data.loc.initst.e_area.xmin = 0.0;
        idev->data.loc.initst.e_area.xmax = ws->size.y;
        idev->data.loc.initst.e_area.ymin = 0.0;
        idev->data.loc.initst.e_area.ymax = ws->size.y;
        idev->data.loc.initst.record.pet1.data = NULL;
 
    /* link the new device into the list */
        idev->next = ws->in_dev_list;
        ws->in_dev_list = idev;
    }
    else {
        GKSERROR( (idev->data.loc.initst.mode != GREQUEST), 141, errgreqloc )
    }
/* initialise current values */
    NtWcToNdc( idev->data.loc.initst.loc.transform, &idev->data.loc.initst.loc.position, &ndcpt );
    NdcToDc( ws, &ndcpt, &idev->data.loc.initpos);
    idev->data.loc.curpos = idev->data.loc.initpos;
 
    if ( idev->data.loc.initst.esw == GECHO )
        XgksLocUpdatePrompt( ws, idev, PROMPTON, (Gpoint *)NULL,
	    (XMotionEvent *)NULL,-1);   /*MIT*/ /* PTR c1133 */ 
    idev->active = True;
 
/* wait for trigger or break */
    idev->touched = False;
    idev->breakhit = False;
    while( (idev->touched == False) && (idev->breakhit == False) )
        sigpause( 0 );
 
#ifdef LOCDEBUG
    fprintf(stderr, "greqloc() touched %d %s breakhit %d %s\n",idev->touched,(idev->touched==True)? "True":"False", idev->breakhit,(idev->breakhit == True)?"True":"False");
#endif
 
    idev->active = False;
    if ( idev->data.loc.initst.esw == GECHO )
        XgksLocUpdatePrompt( ws, idev, PROMPTOFF, (Gpoint *)NULL,
		(XMotionEvent *)NULL,-1);	/*MIT*/ /* PTR c1133 */
 
    if ( (idev->breakhit == True) ) {
        response->status = GNONE;
    }
    else {
        response->status = GOK;
        DcToNdc( ws, &idev->data.loc.curpos, &ndcpt);
        XgksFindNTrans( &ndcpt, &response->loc );    /* find the ntrans and WC point */
    }
    return( OK );
}
 
/*
 *  SAMPLE LOCATOR
 *
 * returns 0=ok or 7, 20, 25, 38, 140, 142
 */
 
gsampleloc( ws_id, dev, response)
    Gint ws_id;        /* workstation identifier */
    Gint dev;        /* locator device number */
    Gloc *response;        /* OUT locator response */
{
		/*
	 * declare global functions
	 */
        void XgksFindNTrans();		/*MIT*/

    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    Gpoint ndcpt;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsampleloc ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsampleloc)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgsampleloc )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgsampleloc)
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errgsampleloc )			/* AUG */
    idev = XgksIDevLookup( ws, dev, GLOCATOR );			/* AUG */
 
/* In SAMPLE mode? */
    GKSERROR( (idev == NULL) || (idev->data.loc.initst.mode != GSAMPLE), 142, errgsampleloc )	/* AUG */
 
/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );
 
/* Convert current measure to WC space */
    DcToNdc( ws, &idev->data.loc.curpos, &ndcpt);
    XgksFindNTrans( &ndcpt, response );    /* find the ntrans and WC point */
 
    return( OK );
}
 
/*$F
 * INQUIRE LOCATOR DEVICE STATE
 *
 * Errors: 0, 7, 20, 25, 38, 140, 2000
 *
 */
 
ginqlocst( ws_id, dev, type, state)
    Gint ws_id;        /* workstation identifier */
    Gint dev;        /* locator device number */
    Gqtype type;        /* type of returned values */
    Glocst *state;        /* OUT current locator state */
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqlocst ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqlocst)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginqlocst )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errginqlocst)
 
/* check enumeration */
    GKSERROR( (type != GSET && type != GREALIZED ), 2000, errginqlocst )
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errginqlocst )
 
/* Copy the data to the user's structure */
    if ((idev = XgksIDevLookup( ws, dev, GLOCATOR )) == NULL) {
        state->mode = GREQUEST;
        state->esw = GECHO;
        state->loc.transform = 0;
        state->loc.position.x = 0.5;
        state->loc.position.y = 0.5;
        state->pet = 1;
        state->e_area.xmin = 0.0;
        state->e_area.xmax = ws->size.x;
        state->e_area.ymin = 0.0;
        state->e_area.ymax = ws->size.y;
        state->record.pet1.data = NULL;
    }
    else {
        *state = idev->data.loc.initst;
        /* if idev->data.loc.initst.record pointed anywhere, it would
         * be copied here.
         */
    }
 
    return( OK );
}
 
/*
 * INQUIRE DEFAULT LOCATOR DEVICE DATA
 *
 * Errors: 0, 8, 22, 23, 38, 140
 *
 * data->pets.integers are malloc'ed by GKS;
 * it is the user's responsibility to free these structures.
 */
 
ginqdefloc( type, dev, data )
    Gchar *type;        /* worsktation type string */
    Gint dev;        /* locator device number */
    Gdefloc *data;        /* OUT default locator device data structure */
{
    EWSTYPE ewstype;
    int i;
 
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqdefloc ) /*c1147*/
 
/* valid ws type? */
    ewstype = XgksWsTypeToEnum( type );
    GKSERROR( (ewstype == WST_INVALID), 22, errginqdefloc )	/*c1087*/
 
/* valid workstation type (assumes all INPUT and OUTIN workstations are X_WIN */
    GKSERROR( ewstype != X_WIN, 38, errginqdefloc)
 
/* valid locator device? */
    GKSERROR( (dev < 1), 140, errginqdefloc)
 
/* STEP 2: set up the return values */
    data->position.x    = 0.5;
    data->position.y    = 0.5;
    data->pets.number    = 5;
    data->pets.integers    = (Gint *) malloc( sizeof( Gint ) * data->pets.number);
    if ( data->pets.integers==NULL) {
        free( data );
        gerrorhand( 300, errginqdefloc, xgks_state.gks_err_file);	/*c1147*/
        return( 300 );
    }
    for( i=0; i<5; i++)
        data->pets.integers[i] = i+1;
 
    data->e_area.xmin    = 0.0;
    data->e_area.xmax    = WS_MAX_DCX;
    data->e_area.ymin    = 0.0;
    data->e_area.ymax    = WS_MAX_DCY;
    data->record.pet1.data    = NULL;
 
    return(OK);
}
 
/*
 *  XgksLocUpdatePrompt - update the locator prompt
 */
 
XgksLocUpdatePrompt( ws, idev, pstate, newdcpt, xmev, event_id ) /* PTR c1133 */
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    PromptStatus pstate;
    Gpoint *newdcpt;
    XMotionEvent *xmev;
    int event_id;               /* PTR c1133 */
{
	/*
	 * declare global functions
	 */
        void XgksFindNTrans();		/*MIT*/

    Gpoint ndcpt;
    Gloc *data;
    XRectangle rect;
    XSegment segs[2];
    XPoint xpt, xinitpt, xpts[2];
 
#ifdef LOCDEBUG
    fprintf(stderr, "XgksLocUpdatePrompt( %s )\n", (pstate==PROMPTON)?"PROMPTON":(pstate==PROMPTOFF)?"PROMPTOFF":"PROMPTMOVE");
#endif
    rect.x = 0;
    rect.y = 0;
    rect.width = ws->wbound.x;
    rect.height = ws->wbound.y;
    XSetClipRectangles(ws->dpy, idev->gc, 0, 0, &rect, 1, Unsorted);
 
    switch( pstate ) {
    case PROMPTON:
        switch( idev->data.loc.initst.pet ) {
        default:
        case 1:
        case 3:        /* tracking cross uses X cursor */
            break;
        case 2:
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            segs[0].x1 = 0;        segs[0].y1 = xpt.y;
            segs[0].x2 = ws->wbound.x; segs[0].y2 = xpt.y;
            segs[1].x1 = xpt.x;    segs[1].y1 = 0;
            segs[1].x2 = xpt.x;    segs[1].y2 = ws->wbound.y;
            XDrawSegments( ws->dpy, ws->win, idev->gc, segs, 2);
            break;
        case 4:
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            xpts[0].x = xinitpt.x;    xpts[0].y = xinitpt.y;
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            xpts[1].x = xpt.x;    xpts[1].y = xpt.y;
            XDrawLines( ws->dpy, ws->win, idev->gc, xpts, 2, CoordModeOrigin);
            /*
            XDrawLine( ws->dpy, ws->win, idev->gc, xinitpt.x, xinitpt.y, xpt.x, xpt.y );
            */
            break;
        case 5:
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            XDrawRectangle( ws->dpy, ws->win, idev->gc,
                xinitpt.x, xinitpt.y, xpt.x - xinitpt.x, xpt.y - xinitpt.y );
            break;
        }
        break;
    case PROMPTOFF:
        switch( idev->data.loc.initst.pet ) {
        default:
        case 1:
        case 3:        /* tracking cross uses X cursor */
            break;
        case 2:
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            segs[0].x1 = 0;        segs[0].y1 = xpt.y;
            segs[0].x2 = ws->wbound.x; segs[0].y2 = xpt.y;
            segs[1].x1 = xpt.x;    segs[1].y1 = 0;
            segs[1].x2 = xpt.x;    segs[1].y2 = ws->wbound.y;
            XDrawSegments( ws->dpy, ws->win, idev->gc, segs, 2);
            break;
        case 4:
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            xpts[0].x = xinitpt.x;    xpts[0].y = xinitpt.y;
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            xpts[1].x = xpt.x;    xpts[1].y = xpt.y;
            XDrawLines( ws->dpy, ws->win, idev->gc, xpts, 2, CoordModeOrigin);
            /*
            XDrawLine( ws->dpy, ws->win, idev->gc, xinitpt.x, xinitpt.y, xpt.x, xpt.y );
            */
            break;
        case 5:
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            XDrawRectangle( ws->dpy, ws->win, idev->gc,
                xinitpt.x, xinitpt.y, xpt.x - xinitpt.x, xpt.y - xinitpt.y );
            break;
        }
        break;
    case PROMPTMOVE:
#ifdef LOCDEBUG
        fprintf(stderr,"XgksLocUpdatePrompt( DC= %f %f )\n", newdcpt->x, newdcpt->y);
        fprintf(stderr,"XgksLocUpdatePrompt: DcToX trans = xTrans=%f xScale=%f  yTrans=%f yScale=%f\n", ws->ndctodctrans.xTrans, ws->ndctodctrans.xScale, ws->ndctodctrans.yTrans, ws->ndctodctrans.yScale);
        fprintf(stderr,"XgksLocUpdatePrompt: ndc (%f %f)\n",
             ((newdcpt)->x - (ws)->ndctodctrans.xTrans) / (ws)->ndctodctrans.xScale,
             ((newdcpt)->y - (ws)->ndctodctrans.yTrans) / (ws)->ndctodctrans.yScale);
 
#endif
    /* locator point must lie within the workstation window */
        DcToNdc( ws, newdcpt, &ndcpt);
        /* this macro fails here for some unknown reason
        */
#ifdef BLOCDEBUG
        fprintf(stderr,"\nXgksLocUpdatePrompt( DC= %f %f NDC %f %f )\n", newdcpt->x, newdcpt->y, ndcpt.x, ndcpt.y);
#endif
 
        ndcpt.x = ((newdcpt)->x - (ws)->ndctodctrans.xTrans) / (ws)->ndctodctrans.xScale;
        ndcpt.y = ((newdcpt)->y - (ws)->ndctodctrans.yTrans) / (ws)->ndctodctrans.yScale;
#ifdef BLOCDEBUG
        fprintf(stderr,"XgksLocUpdatePrompt( DC= %f %f NDC %f %f )\n", newdcpt->x, newdcpt->y, ndcpt.x, ndcpt.y);
/*
        fprintf(stderr, "XgksLocUpdatePrompt( workstation window= %f %f %f %f )\n",
        ws->wsti.current.w.xmin, ws->wsti.current.w.xmax,
        ws->wsti.current.w.ymin, ws->wsti.current.w.ymax);
*/
#endif
 
        if (ndcpt.x < ws->wsti.current.w.xmin
                || ndcpt.x > ws->wsti.current.w.xmax
                || ndcpt.y < ws->wsti.current.w.ymin
                || ndcpt.y > ws->wsti.current.w.ymax )
            return( 0 );
 
        switch( idev->data.loc.initst.pet ) {
        default:
        case 1:
        case 3:        /* tracking cross uses X cursor */
            idev->data.loc.curpos = *newdcpt;
            break;
        case 2:
            /* erase old prompt */
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            segs[0].x1 = 0;        segs[0].y1 = xpt.y;
            segs[0].x2 = ws->wbound.x; segs[0].y2 = xpt.y;
            segs[1].x1 = xpt.x;    segs[1].y1 = 0;
            segs[1].x2 = xpt.x;    segs[1].y2 = ws->wbound.y;
        if(idev->data.loc.initst.esw==GECHO)
                             XDrawSegments( ws->dpy, ws->win, idev->gc, segs, 2);
            /* draw new prompt */
            idev->data.loc.curpos = *newdcpt;
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            segs[0].x1 = 0;        segs[0].y1 = xpt.y;
            segs[0].x2 = ws->wbound.x; segs[0].y2 = xpt.y;
            segs[1].x1 = xpt.x;    segs[1].y1 = 0;
            segs[1].x2 = xpt.x;    segs[1].y2 = ws->wbound.y;
        if(idev->data.loc.initst.esw==GECHO)
                       XDrawSegments( ws->dpy, ws->win, idev->gc, segs, 2);
            break;
        case 4:
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            xpts[0].x = xinitpt.x;    xpts[0].y = xinitpt.y;
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            xpts[1].x = xpt.x;    xpts[1].y = xpt.y;
        if(idev->data.loc.initst.esw==GECHO)
                           XDrawLines( ws->dpy, ws->win, idev->gc, xpts, 2, CoordModeOrigin);
            /*
            XDrawLine( ws->dpy, ws->win, idev->gc, xinitpt.x, xinitpt.y, xpt.x, xpt.y );
            */
            idev->data.loc.curpos = *newdcpt;
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            segs[0].x1 = xinitpt.x;    segs[0].y1 = xinitpt.y;
            DcToX( ws, &idev->data.loc.curpos, &xpt);
            segs[0].x2 = xpt.x;    segs[0].y2 = xpt.y;
                if(idev->data.loc.initst.esw==GECHO)
               XDrawSegments( ws->dpy, ws->win, idev->gc, segs, 1);
            /*
            XDrawLine( ws->dpy, ws->win, idev->gc, xinitpt.x, xinitpt.y, xpt.x, xpt.y );
            */
            break;
        case 5:
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            DcToX( ws, &idev->data.loc.curpos, &xpt);
                if(idev->data.loc.initst.esw==GECHO)
            XDrawRectangle( ws->dpy, ws->win, idev->gc,
                xinitpt.x, xinitpt.y, xpt.x - xinitpt.x, xpt.y - xinitpt.y );
            idev->data.loc.curpos = *newdcpt;
            DcToX( ws, &idev->data.loc.initpos, &xinitpt);
            DcToX( ws, &idev->data.loc.curpos, &xpt);
                if(idev->data.loc.initst.esw==GECHO)
            XDrawRectangle( ws->dpy, ws->win, idev->gc,
                xinitpt.x, xinitpt.y, xpt.x - xinitpt.x, xpt.y - xinitpt.y );
            break;
        }
        switch(idev->data.loc.initst.mode ) {
        default:
        case GREQUEST:
            if (xmev->type != ButtonRelease)
                break;
            idev->touched = True;
            XBell( ws->dpy, 0);
            break;
        case GSAMPLE:
            break;
        case GEVENT:
            if (xmev->type != ButtonRelease)
                break;
            data = (Gloc *) malloc( sizeof (Gloc) );
            if ( data == NULL )
	    {
                gerrorhand( 300, errXgksLocUpdatePrompt, xgks_state.gks_err_file ); /*c1147*/
		  return (300);
	    }
            else {
                XBell( ws->dpy, 0);
                XgksFindNTrans( &ndcpt, data );
                XgksEnqueueEvent( ws->ws_id, idev->dev, GLOCATOR,
		    (char *)data,event_id); /*MIT*/ /* PTR c1133 */
            }
            break;
        }
        break;
    default:
        break;
    }
 
    XFlush(ws->dpy);
    return( 0 );
}

/*
 * free all memory associate with a locator logical input device
 */
XgksLocDelete( ws, idev )
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
{
    XFreeGC( ws->dpy, idev->gc );
}
