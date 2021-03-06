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
 * X GKS logical input device CHOICE.
 * 
 * $Header: choice.c,v 4.0 89/08/31 16:19:58 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/choice.c,v $
 *
 * $Log:	choice.c,v $
 * Revision 4.0  89/08/31  16:19:58  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.27  89/06/15  08:18:16  bruce
 * PTR# c1180:	Corrected values returned by InquireDefaultChoiceDeviceData
 * 		It now returns 3 valid pets instead of 1.
 * 
 * Revision 3.26  89/06/05  10:08:10  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1176:	Replaced string function calls with macros that check for
 * 		NULL pointers before making the call.
 * 
 * Revision 3.25  89/03/29  09:50:46  bruce
 * PTR# c1168:	Choice menu position now recalculated when window is 
 * 		resized.
 * 
 * Revision 3.24  89/03/15  20:21:11  bruce
 * ptr# c2046:	insure that initial status is set and
 * 		returned correctly.
 * 
 * Revision 3.23  89/02/04  15:22:27  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.22  88/12/19  17:09:29  amy
 * PTR c1148	ginitchoice, gsetchoicemode, greqchoice:  replace
 * 		a malloc with a call to XgksIDevNew.
 * 
 * Revision 3.21  88/12/16  12:29:30  amy
 * PTR c1133	Calls to XgksChoUpdatePrompt:  add -1 for event_id parameter.
 * 		Calls to XgksEnqueueEvent:  add event_id to parameter
 * 		list.
 * 
 * Revision 3.20  88/12/13  12:02:21  amy
 * PTR c1019	ginitchoice:  
 * 	Removed incorrect check for valid initial choice (pet 1).
 * 	Added check for initial choice <= 0 (all pets).
 * 	Added check for initial choice in set of existing function keys
 * 	(pets 1 and 2).
 * 	Added check for initial choice flagged on (pet 2).
 * 
 * Revision 3.19  88/12/06  16:38:20  amy
 * PTR c1130	GKSERROR and gerrorhand calls:  changed function name parameter
 * 		from character string to enum. type value.
 * 
 * Revision 3.18  88/12/01  11:04:41  amy
 * New 4.3 C compiler--
 * 	XgksChoUpdatePrompt:  initialize GoodKey to False to avoid warning.
 * 
 * Revision 3.17  88/11/05  11:13:15  amy
 * PTR c1030	ginitchoice, XgksChoUpdatePrompt:  move code that
 * 		calculates menu values so that choice knows where menu choices
 * 		are on the screen, and consequently, which one user has chosen
 * 		with echo off.
 * 
 * Revision 3.16  88/11/04  08:36:50  amy
 * PTR c1057	gsetchoicemode:  set up SIGALRM signal handler if mode = GEVENT.
 * 
 * Revision 3.15  88/11/01  13:32:46  amy
 * PTR c1087	ginqdefchoice:  added check for error 22, ws type invalid.
 * 
 * Revision 3.14  88/09/20  15:03:49  amy
 * MIT  Include strings.h.
 * 	 Call XgksChoUpdatePrompt with all four parameters.
 * 	 Redefine XgksChoUpdatePrompt to take four parameters.
 * 	 Call XgksEnqueueEvent with data parameter cast as a char *.
 * 
 * Revision 3.13  88/08/16  13:39:46  amy
 * AUG  gsetchoicemode, gsamplechoice:  merged in changes from August tape.
 * 
 * Revision 3.12  88/08/11  08:21:49  amy
 * ginitchoice, ginqchoicest, gsamplechoice, greqchoice & gsetchoicemode:  
 * Added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.3  88/07/28  11:02:40  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:51:33  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:48:25  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: choice.c,v 4.0 89/08/31 16:19:58 amy Exp $";
 
#include "gks_implem.h" /* d1 */
#include <signal.h>			/* c1057 */
#include <strings.h>			/* MIT */

extern  XgksAwaitInterrupt();	/*  Declare  Interrupt function   PTR# c1057  */
 
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
 
#define DEF_CHO_FONT    "8x13"
static XFontStruct *MFontInfo = (XFontStruct *)NULL;
 
/*
 *  INITIALISE CHOICE
 *
 * returns: 0, 7, 20, 25, 38, 51, 120, 122, 123, 140, 141, 144, 145, 146, 152, 300
 */
 
#define MenuPadV 4        /* These macros defs had to be moved  PTR# c1030 */
#define MenuPadH MenuPadV
 
ginitchoice( ws_id, dev, init, pet, area, record )
    Gint ws_id;
    Gint dev;
    Gchoice *init;
    Gint pet;
    Glimit *area;
    Gchoicerec *record;
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    XGCValues gcvalues;
    Gprflag *f;
    char **d, **s;
    int number, i;						/* c1019 */
#if 0   /* following line commented out for now (MW) */		/* c1019 */
    int keycode; 						/* c1019 */
#endif								/* c1019 */
    int maxwidth, width;   /* These vars were added to compute string */
    char **menustr;        /* menu dimensions that used to be computed */
    Gpoint dcpt;           /* in the PROMPTON: portion of the prompt */
    XRectangle rect;       /* update routine that isn't called when there's */
    XPoint xpt;            /* no echo     PTR# c1030 */
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:ginitchoice()\n");
#endif
/* STEP 1: check for errors */
/*    gks in proper state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginitchoice ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginitchoice)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginitchoice )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errginitchoice)
 
/* valid echo area */
    GKSERROR( (area->xmin > area->xmax || area->ymin > area->ymax), 51, errginitchoice )
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errginitchoice )
 
/* valid and supported prompt mode? */
    GKSERROR( (pet < 1) || (pet > 3), 144, errginitchoice)
 
/* Echo inside display space? */
    GKSERROR( (area->xmin < 0 || area->xmax > ws->size.x
        || area->ymin < 0 || area->ymax > ws->size.y),
        145, errginitchoice )
 
/* initial values valid */                                         /* c1019 */
/* rewritten for correctness and to resolve PTR c1019 */           /* c1019 */
    switch ( pet ) {                                               /* c1019 */
    case 1:        /* no data */                                   /* c1019 */
        /* initial value ok (make sure key in range) */            /* c1019 */
        GKSERROR( (init->status == GC_OK) && (init->choice < 1), 152, errginitchoice)                                                               /* c1019 */
#if 0   /* following block commented out for now (MW) */           /* c1019 */
        /* (make sure key exists) */                               /* c1019 */
        keycode =                                                  /* c1019 */
        XKeysymToKeycode( ws->dpy,                                 /* c1019 */
                          (KeySym)(XK_F1+init->choice - 1) );      /* c1019 */
        GKSERROR( (init->status == GC_OK) && (keycode == 0), 152, errginitchoice)                                                                   /* c1019 */
#endif
        break;                                                     /* c1019 */
    case 2:        /* array of prompt flags ON | OFF */            /* c1019 */
        GKSERROR( (record->pet2.number < 1), 146, errginitchoice)   /* c1019 */
        /* initial value ok (make sure key in range) */            /* c1019 */
        GKSERROR( (init->status == GC_OK) && (init->choice < 1), 152, errginitchoice)                                                               /* c1019 */
#if 0   /* following block commented out for now (MW) */           /* c1019 */
        /* (make sure key exists) */                               /* c1019 */
        keycode =                                                  /* c1019 */
        XKeysymToKeycode( ws->dpy,                                 /* c1019 */
                          (KeySym)(XK_F1+init->choice - 1) );      /* c1019 */
        GKSERROR( (init->status == GC_OK) && (keycode == 0), 152, errginitchoice)                                                                   /* c1019 */
#endif
        /* (make sure key is flagged on) */                        /* c1019 */
        GKSERROR( (init->status == GC_OK) && (record->pet2.enable[init->choice-1] == GPROFF), 152, errginitchoice)                                  /* c1019 */
        break;                                                     /* c1019 */
    case 3:        /* menu strings */                              /* c1019 */
        GKSERROR( (record->pet3.number < 1), 146, errginitchoice)   /* c1019 */
        /* initial value ok (make sure choice is in range) */      /* c1019 */
        GKSERROR( (init->status == GC_OK) && ((init->choice > record->pet2.number) || (init->choice < 1)), 152, errginitchoice)                     /* c1019 */
        break;                                                     /* c1019 */
    case 4:        /* strings that can be typed in */              /* c1019 */
        break;                                                     /* c1019 */
    case 5:        /* segment */                                   /* c1019 */
        break;                                                     /* c1019 */
    default:                                                       /* c1019 */
        break;                                                     /* c1019 */
    }                                                              /* c1019 */

    if ((pet > 0) && (pet < 4)) {                                  /* c1019 */
    /* Set up the gcvalues */                                      /* c1019 */
        gcvalues.function = GXcopy;
        gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = 0;
        gcvalues.line_style = LineSolid;
        gcvalues.fill_style = FillSolid;
        XgksSIGIO_OFF(ws->dpy);		/*c1147*/ /* d1 */
        if (MFontInfo == NULL)
            MFontInfo = XLoadQueryFont( ws->dpy, DEF_CHO_FONT );
        XgksSIGIO_ON(ws->dpy);		/*c1147*/ /* d1 */
        gcvalues.font = MFontInfo->fid;
    }								 /* c1019 */
 
    if ((idev = XgksIDevLookup( ws, dev, GCHOICE)) == NULL) {
    /* Create the Input Device structure */
        idev = XgksIDevNew();  /* PTR# c1148 */
        GKSERROR( (idev == NULL), 300, errginitchoice)
        idev->class = GCHOICE;
        idev->dev = dev;
        idev->active = False;
        idev->gc = XCreateGC( ws->dpy, ws->win, 
            GCFunction | GCForeground | GCBackground |
            GCLineWidth | GCLineStyle | GCFillStyle | GCFont,
            &gcvalues);
        idev->data.cho.initst.mode = GREQUEST;
        idev->data.cho.initst.esw = GECHO;
 
    /* link the new device into the list */
        idev->next = ws->in_dev_list;
        ws->in_dev_list = idev;
    }
    else {
    /* if the device is not in REQUEST mode, not allowed to initialize it */
        GKSERROR( idev->data.cho.initst.mode != GREQUEST, 141, errginitchoice )
        XChangeGC( ws->dpy, idev->gc,
            GCFunction | GCForeground | GCBackground |
            GCLineWidth | GCLineStyle | GCFillStyle | GCFont,
            &gcvalues);
    }
    idev->data.cho.initst.choice = *init;
    idev->data.cho.initst.pet    = pet;
    idev->data.cho.initst.e_area = *area;
    idev->data.cho.initst.record = *record;
 
/* copy any data pointed at from the record */
    switch( pet ) {
    default:
    case 1:
        break;
    case 2:
        number = record->pet2.number;
        f = idev->data.cho.initst.record.pet2.enable = (Gprflag *)malloc( sizeof( Gprflag ) * number);
        GKSERROR( (f == NULL), 300, errginitchoice)
        for( i = 0; i<number; i++)
            f[i] = record->pet2.enable[i];
        break;
    case 4:
    case 5:
        break;
    case 3:        /* copy strings in data record */
        number = record->pet3.number;
        d = idev->data.cho.initst.record.pet3.strings = (char **)malloc( sizeof (char *) * number);
        GKSERROR( (d == NULL), 300, errginitchoice)
        for( i=0, s=record->pet3.strings; i<number; i++, d++, s++) {
            *d = (char *) malloc( STRLEN( *s ) +1);	/* c1176 */
            GKSERROR( (*d == NULL), 300, errginitchoice)
            STRCPY( *d, *s );	/* c1176 */
        }

    /* The code from this line to the end of the case switch was moved from
       the PROMPTON: portion of the prompt update procedure because these
       numbers are needed when the echo is off and the prompt update code
       isn't called then   PTR# c1030  */

        dcpt.x = idev->data.cho.initst.e_area.xmin;
        dcpt.y = idev->data.cho.initst.e_area.ymax;
        DcToX( ws, &dcpt, &xpt );
        rect.x = xpt.x;
        rect.y = xpt.y;
     
        dcpt.x = idev->data.cho.initst.e_area.xmax;
        dcpt.y = idev->data.cho.initst.e_area.ymin;
        DcToX( ws, &dcpt, &xpt );
        rect.width = xpt.x - rect.x;
        rect.height = xpt.y - rect.y;
     
    /* figure out size of menu */
        menustr = record->pet3.strings;
        maxwidth = XTextWidth( MFontInfo, *menustr, STRLEN( *menustr) );	/* c1176 */
        for( i=1, menustr++; i< number; i++, menustr++) {
            width = XTextWidth( MFontInfo, *menustr, STRLEN( *menustr) );	/* c1176 */
            if (width > maxwidth) maxwidth = width;
	}
        idev->data.cho.origin.x = rect.x;
        idev->data.cho.origin.y = rect.y;
        idev->data.cho.iheight = 
	 	MFontInfo->ascent+MFontInfo->descent+(MenuPadV << 1);
        idev->data.cho.height = idev->data.cho.iheight * number;
        idev->data.cho.width = maxwidth + (MenuPadH << 1);
 
	/*  End of changes for PTR# c1030  */
    }
    return(0);
}

/*
 * SET CHOICE MODE
 *
 * returns: 0, 7, 20, 25, 38, 140, 143
 */
 
gsetchoicemode(ws_id, dev, mode, echo)
    Gint ws_id;
    Gint dev;
    Gimode mode;
    Gesw echo;
{
    XGCValues gcvalues;
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:gsetchoicemode()\n");
#endif
/* STEP 1: check for errors */
/*    gks in proper state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsetchoicemode ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetchoicemode)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgsetchoicemode )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgsetchoicemode)
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errgsetchoicemode )
 
/* check enumerations */
    GKSERROR( ((mode != GREQUEST && mode != GSAMPLE && mode != GEVENT)
            || (echo != GECHO && echo != GNOECHO)),
        2000, errgsetchoicemode )
 
    if ((idev = XgksIDevLookup( ws, dev, GCHOICE)) == NULL) {
    /* Create the Input Device structure */
        idev = XgksIDevNew();    /* c1148 */
        GKSERROR( (idev == NULL), 300, errgsetchoicemode)	/* ALP */
        idev->class = GCHOICE;
        idev->dev = dev;
        idev->active = False;
        gcvalues.function = GXcopy;
        gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = 0;
        gcvalues.line_style = LineSolid;
        gcvalues.fill_style = FillSolid;
        XgksSIGIO_OFF(ws->dpy);		/*c1147*/ /* d1 */
        if (MFontInfo == NULL)
            MFontInfo = XLoadQueryFont( ws->dpy, DEF_CHO_FONT );
        XgksSIGIO_ON(ws->dpy);		/*c1147*/ /* d1 */
        gcvalues.font = MFontInfo->fid;
        idev->gc = XCreateGC( ws->dpy, ws->win, 
            GCFunction | GCForeground | GCBackground |
            GCLineWidth | GCLineStyle | GCFillStyle | GCFont,
            &gcvalues);
        idev->data.cho.initst.mode = GREQUEST;
        idev->data.cho.initst.esw = GECHO;
        idev->data.cho.initst.choice.status = GC_NOCHOICE;	/* AUG */
	idev->data.cho.initst.choice.choice = 1;		/* c1153 */
        idev->data.cho.initst.pet = 1;
        idev->data.cho.initst.e_area.xmin = 0.0;
        idev->data.cho.initst.e_area.xmax = ws->size.x;
        idev->data.cho.initst.e_area.ymin = 0.0;
        idev->data.cho.initst.e_area.ymax = ws->size.y;
        idev->data.cho.initst.record.pet1.data = NULL;
 
    /* link the new device into the list */
        idev->next = ws->in_dev_list;
        ws->in_dev_list = idev;
    }
    else {
        if ((idev->active == True) && (idev->data.cho.initst.esw == GECHO))
           XgksChoUpdatePrompt( ws, idev, PROMPTOFF, (XMotionEvent *)NULL,-1); /*MIT*/ /* PTR c1133 */
    }
    idev->data.cho.initst.mode = mode;
    idev->data.cho.initst.esw = echo;
 
    if (mode == GSAMPLE || mode == GEVENT) {
        idev->data.cho.curcho = 0;
	if (mode == GEVENT) 
	   signal( SIGALRM, XgksAwaitInterrupt); /* Set up signal call PTR# c1057  */
        idev->active = True;
        if ( echo == GECHO )
            XgksChoUpdatePrompt( ws, idev, PROMPTON, (XMotionEvent *)NULL,-1); /*MIT*/ /* PTR c1133 */ 
    }
    else    /* GREQUEST */
        idev->active = False;
 
    return(0);
}

/*
 * REQUEST CHOICE
 *
 * returns: 0, 7, 20, 25, 38, 140, 141
 */
 
greqchoice( ws_id, dev, response )
    Gint ws_id;
    Gint dev;
    Gchoice *response;
{
    XGCValues gcvalues;
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:greqchoice()\n");
#endif
/* STEP 1: check for errors */
/*    gks in proper state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgreqchoice ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgreqchoice)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgreqchoice )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgreqchoice)
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errgreqchoice )
 
    if ((idev = XgksIDevLookup( ws, dev, GCHOICE)) == NULL) {
    /* Create the Input Device structure */
        idev = XgksIDevNew(); /* c1148 */
        GKSERROR( (idev == NULL), 300, errgreqchoice)
        idev->class = GCHOICE;
        idev->dev = dev;
        idev->active = False;
        gcvalues.function = GXcopy;
        gcvalues.foreground = ws->wsfg;
        gcvalues.background = ws->wsbg;
        gcvalues.line_width = 0;
        gcvalues.line_style = LineSolid;
        gcvalues.fill_style = FillSolid;
        XgksSIGIO_OFF(ws->dpy);		/*c1147*/ /* d1 */
        if (MFontInfo == NULL)
            MFontInfo = XLoadQueryFont( ws->dpy, DEF_CHO_FONT );
        XgksSIGIO_ON(ws->dpy);		/*c1147*/ /* d1 */
        gcvalues.font = MFontInfo->fid;
        idev->gc = XCreateGC( ws->dpy, ws->win, 
            GCFunction | GCForeground | GCBackground |
            GCLineWidth | GCLineStyle | GCFillStyle | GCFont,
            &gcvalues);
        idev->data.cho.initst.mode = GREQUEST;
        idev->data.cho.initst.esw = GECHO;
        idev->data.cho.initst.choice.status = GC_NOCHOICE; /* c2046 */
	idev->data.cho.initst.choice.choice = 1;		/* c1153 */
        idev->data.cho.initst.pet = 1;
        idev->data.cho.initst.e_area.xmin = 0.0;
        idev->data.cho.initst.e_area.xmax = ws->size.x;
        idev->data.cho.initst.e_area.ymin = 0.0;
        idev->data.cho.initst.e_area.ymax = ws->size.y;
        idev->data.cho.initst.record.pet1.data = NULL;
 
    /* link the new device into the list */
        idev->next = ws->in_dev_list;
        ws->in_dev_list = idev;
    }
    else {
        GKSERROR( (idev->data.cho.initst.mode != GREQUEST), 141, errgreqchoice)
    }
/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );
 
    idev->data.cho.curcho = 0;
    if ( idev->data.cho.initst.esw == GECHO )
        XgksChoUpdatePrompt( ws, idev, PROMPTON, (XMotionEvent *)NULL,-1); /*MIT*/ /* PTR c1133 */
    idev->active = True;
 
/* wait for trigger or break */
    idev->touched = False;
    idev->breakhit = False;
    while ( (idev->touched == False) && (idev->breakhit == False) )
        sigpause( 0 );
 
    idev->active = False;
    if ( idev->data.cho.initst.esw == GECHO )
        XgksChoUpdatePrompt( ws, idev, PROMPTOFF, (XMotionEvent *)NULL,-1); /*MIT*/ /* PTR c1133 */ 
 
    if ( idev->breakhit == True ) {
        response->status = GC_NONE;
    }
    else {
        response->status = (idev->data.cho.curcho == 0) ? GC_NOCHOICE : GC_OK;
        response->choice = idev->data.cho.curcho;
    }
        
    return(0);
}

/*
 * SAMPLE CHOICE
 *
 * returns: 0, 7, 20, 25, 38, 140, 142
 */
 
gsamplechoice( ws_id, dev, response )
    Gint ws_id;
    Gint dev;
    Gchoice *response;
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:gsamplechoice()\n");
#endif
/* STEP 1: check for errors */
/*    gks in proper state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsamplechoice ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsamplechoice)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgsamplechoice )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgsamplechoice)
 
/* valid choice device number */			/* AUG */
    GKSERROR( (dev < 1), 140, errgsamplechoice )
 
    idev = XgksIDevLookup( ws, dev, GCHOICE );		/* AUG */

/* In SAMPLE mode? (if NULL then implicitly in request mode) */	/* AUG */
  GKSERROR( (idev == NULL) || (idev->data.cho.initst.mode != GSAMPLE), 142, errgsamplechoice)  /* AUG */

    /* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );
 
    response->status = (idev->data.cho.curcho == 0) ? GC_NOCHOICE : GC_OK;
    response->choice = idev->data.cho.curcho;
 
    return(0);
}

/*
 * INQUIRE CHOICE DEVICE STATE
 *
 * errors: 0, 7, 20, 25, 38, 140
 *
 * See Also: ANSI Standard p.168
 */
 
ginqchoicest( ws_id, dev, state )
    Gint ws_id;
    Gint dev;
    Gchoicest *state;
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    Gprflag *f;
    char **s, **d;
    int number, i;
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:ginqchoicest()\n");
#endif
/* STEP 1: check for errors */
/*    gks in proper state? */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqchoicest ) /*c1147*/
 
/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqchoicest)   /* c1012 */

/* open wsid? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
    GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginqchoicest )  /* c1012 */
 
/* valid workstation type */
    GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errginqchoicest)
 
/* valid locator device number */
    GKSERROR( (dev < 1), 140, errginqchoicest )
 
    if ((idev = XgksIDevLookup( ws, dev, GCHOICE)) == NULL) {
        state->mode = GREQUEST;
        state->esw = GECHO;
        state->choice.status = GC_NOCHOICE;
        state->choice.choice = 1;		/* c1153 */
        state->pet = 1;
        state->e_area.xmin = 0.0;
        state->e_area.xmax = ws->size.x;
        state->e_area.ymin = 0.0;
        state->e_area.ymax = ws->size.y;
        state->record.pet1.data = NULL;
    }
    else {
        *state = idev->data.cho.initst;
    /* copy any data pointed at from the record */
        switch( idev->data.cho.initst.pet ) {
        default:
        case 1:
            break;
        case 2:
            number = idev->data.cho.initst.record.pet2.number;
            f = state->record.pet2.enable = (Gprflag *)malloc( sizeof( Gprflag ) * number);
            GKSERROR( (f == NULL), 300, errginqchoicest)
            for( i = 0; i<number; i++)
                f[i] = idev->data.cho.initst.record.pet2.enable[i];
            break;
        case 4:
        case 5:
            break;
        case 3:        /* copy strings in data record */
            number = idev->data.cho.initst.record.pet3.number;
            d = state->record.pet3.strings = (char **)malloc( sizeof(char *) * number);
            s = idev->data.cho.initst.record.pet3.strings;
            GKSERROR( (d == NULL), 300, errginqchoicest)
            for( i=0; i<number; i++, d++, s++) {
                *d = (char *) malloc( STRLEN( *s ) +1);		/* c1176 */
                GKSERROR( (*d == NULL), 300, errginqchoicest)
                STRCPY( *d, *s );		/* c1176 */
            }
        }
    }
 
    return( 0 );
}

/*
 * INQUIRE DEFAULT CHOICE DEVICE DATA
 *
 * errors: 0, 8, 22, 23, 38, 140
 *
 * See Also: ANSI Standard p.187
 */
 
ginqdefchoice( type, dev, data )
    Gchar *type;
    Gint dev;
    Gdefchoice *data;
{
    EWSTYPE ewstype;
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:ginqdefchoice()\n");
#endif
/* STEP 1: check for errors. */
/* proper gks state? */
    GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqdefchoice ) /*c1147*/
 
/* valid wsid? */
    ewstype = XgksWsTypeToEnum( type );
    GKSERROR( ewstype == WST_INVALID, 22, errginqdefchoice )	/*c1087*/
 
/* valid workstation type (assumes all INPUT and OUTIN workstations are X_WIN */
    GKSERROR( ewstype != X_WIN, 38, errginqdefchoice)
 
/* valid locator device? */
    GKSERROR( (dev < 1), 140, errginqdefchoice)
 
/* STEP 2: set up the return values */
    data->choices = 0x7fff;
    data->pets.number = 3;    /* c1180 */
    data->pets.integers = (Gint *) malloc( sizeof( int )*3 );
    GKSERROR( (data->pets.integers == NULL), 300, errginqdefchoice)
    data->pets.integers[0] = 1;
    data->pets.integers[1] = 2;
    data->pets.integers[2] = 3;
    data->e_area.xmin = 0.0;
    data->e_area.xmax = WS_MAX_DCX;
    data->e_area.ymin = 0.0;
    data->e_area.ymax = WS_MAX_DCY;
 
    data->record.pet3.number = 0;
    data->record.pet3.strings = (char **) NULL;
    data->record.pet3.data = (char *) NULL;
 
    return(OK);
}

/*
 * XgksChoUpdatePrompt
 */
 
#define MenuBG ws->wsbg
#define MenuFG ws->wsfg
 
#define MenuTop        idev->data.cho.origin.y
#define MenuLeft    idev->data.cho.origin.x
#define MenuWid        idev->data.cho.width
#define MenuHt        idev->data.cho.height
#define MenuIHt        idev->data.cho.iheight
 
#define CurItem        idev->data.cho.curcho
 
#define DisplayLine( pos, str, fg, bg ) { \
    XSetForeground( ws->dpy, idev->gc, bg); \
    XFillRectangle( ws->dpy, ws->win, idev->gc, pos.x, pos.y, MenuWid, MenuIHt); \
    XSetForeground( ws->dpy, idev->gc, fg); \
    XDrawString( ws->dpy, ws->win, idev->gc, pos.x+MenuPadH, pos.y+MenuPadV+MFontInfo->ascent, str, (str != NULL) ? strlen(str) : 0); \
} /* macro changed for PTR# c1176 */
 
XgksChoUpdatePrompt( ws, idev, pstate, xmev, event_id )	/*MIT*/ /* PTR c1133 */
        WS_STATE_ENTRY *ws;
        INPUT_DEV *idev;
        PromptStatus pstate;
        XMotionEvent *xmev;
	int event_id;          /* PTR c1133 */ 
{
        Gpoint dcpt;
        XRectangle rect;
        XPoint xpt, itempos;
    int i, number;		/*c1030*/
    char **menustr;
    Gchoice *data;
    KeySym ksym;
    Bool GoodKey;
 
#ifdef CHODEBUG
    fprintf(stderr, "xgks:XgksChoUpdatePrompt()\n");
#endif
/* set up clip area */
    dcpt.x = idev->data.cho.initst.e_area.xmin;
    dcpt.y = idev->data.cho.initst.e_area.ymax;
    DcToX( ws, &dcpt, &xpt );
    rect.x = xpt.x;
    rect.y = xpt.y;
    idev->data.cho.origin.x = rect.x;  /* c1168 */
    idev->data.cho.origin.y = rect.y;  /* c1168 */
 
    dcpt.x = idev->data.cho.initst.e_area.xmax;
    dcpt.y = idev->data.cho.initst.e_area.ymin;
    DcToX( ws, &dcpt, &xpt );
    rect.width = xpt.x - rect.x;
    rect.height = xpt.y - rect.y;
 
    XSetClipRectangles( ws->dpy, idev->gc, 0, 0, &rect, 1, Unsorted );
 
    switch( pstate ) {
    case PROMPTON:
        switch( idev->data.cho.initst.pet ) {
        case 1:
        case 2:
            break;
        case 3:
            /* menu size is now computed in ginitchoice()   PTR# c1030  */
            /* display the menu */
	    number = idev->data.cho.initst.record.pet3.number;	/*c1030*/
            /* Full size background */
            XSetForeground( ws->dpy, idev->gc, MenuFG );
            XFillRectangle( ws->dpy, ws->win, idev->gc, MenuLeft, MenuTop, MenuWid+4, MenuHt+4 );
            /* Items */
            menustr = idev->data.cho.initst.record.pet3.strings;
            itempos.x = MenuLeft+2;
            itempos.y = MenuTop+2;
            for( i=0; i < number; i++, itempos.y += MenuIHt, menustr++ ) {
                if ( idev->data.cho.curcho == (i+1) )
                    DisplayLine( itempos, *menustr, MenuBG, MenuFG )
                else
                    DisplayLine( itempos, *menustr, MenuFG, MenuBG )
            }
            break;
        default:
            break;
        }
        break;
    case PROMPTOFF:
        switch( idev->data.cho.initst.pet ) {
        case 1:
        case 2:
            break;
        case 3: /* Wipe out the menu with the Background color */
            XSetForeground( ws->dpy, idev->gc, MenuBG );
            XFillRectangle( ws->dpy, ws->win, idev->gc, MenuLeft, MenuTop, MenuWid+4, MenuHt+4 );
            
            break;
        default:
            break;
        }
        break;
    case PROMPTMOVE:
	GoodKey = False;			/* New 4.3 C compiler: Initialize */
        switch( idev->data.cho.initst.pet ) {
        case 1:    /* No display, just check button */
            if (xmev->type == KeyPress) {
                XLookupString( xmev, NULL, 0, &ksym, NULL);
                if ( ksym >= XK_F1 && ksym <= XK_F35) {
                    CurItem = ksym - XK_F1 +1;
                    GoodKey = True;
                }
                else
                    GoodKey = False;
            }
            break;
        case 2:    /* No display, just check button */
            if (xmev->type == KeyPress) {
                XLookupString( xmev, NULL, 0, &ksym, NULL);
                i = ksym - XK_F1;
                if ( ksym >= XK_F1 && ksym <= XK_F35
                && i < idev->data.cho.initst.record.pet2.number
                && idev->data.cho.initst.record.pet2.enable[i] == GPRON) {
                    CurItem = i+1;
                    GoodKey = True;
                }
                else
                    GoodKey = False;
            }
            break;
        case 3: 
            number = idev->data.cho.initst.record.pet3.number;
            menustr = idev->data.cho.initst.record.pet3.strings;
            itempos.x = MenuLeft+2;
            itempos.y = MenuTop+2;
            for( i= -1; i < number; i++, itempos.y += MenuIHt) {
                if (((XPointerMovedEvent *)xmev)->y < itempos.y)
                    break;
            }
            if (CurItem == (i+1))    /* already highlighted */
                break;
 
            if ( CurItem > 0 ) {    /* unhighlight current item */
                itempos.y = MenuTop +2 + (CurItem - 1) * MenuIHt;
            if(idev->data.cho.initst.esw==GECHO)
                            DisplayLine( itempos, menustr[CurItem-1], MenuFG, MenuBG );
            }
                
            CurItem = (i<0 || i==number) ? 0 : i+1;
 
            if ( CurItem > 0 ) {    /* highlight new current item */
                itempos.y = MenuTop +2 + (CurItem - 1) * MenuIHt;
            if(idev->data.cho.initst.esw==GECHO)
                            DisplayLine( itempos, menustr[CurItem-1], MenuBG, MenuFG )
            }
            break;
        default:
            break;
        }
        switch(idev->data.cho.initst.mode ) {
        default:
        case GREQUEST:
            if ((xmev->type == ButtonRelease) || ((xmev->type == KeyPress) && (GoodKey == True))) {
                idev->touched = True;
                XBell( ws->dpy, 0);
            }
            break;
        case GSAMPLE:
            break;
        case GEVENT:
            if ((xmev->type == ButtonRelease) || ((xmev->type == KeyPress) && (GoodKey == True))) {
                data = (Gchoice *) malloc( sizeof (Gchoice) );
                if ( data == NULL )
		{
                    gerrorhand(300,errXgksChoUpdatePrompt,xgks_state.gks_err_file); /*c1147*/
		    return(300);
		}
                else {
                    XBell( ws->dpy, 0);
                    data->status = (CurItem == 0) ? GC_NOCHOICE : GC_OK;
                    data->choice = CurItem;
                    XgksEnqueueEvent( ws->ws_id, idev->dev, GCHOICE, (char *)data,event_id); /* PTR c1133 */ 
									/*MIT*/
                }
            }
            break;
        }
        break;
    default:
        break;
    }
    XFlush( ws->dpy );
    return( 0 );
}

/*
 * Delete all structures used to maintain a choice logical input device.
 */
XgksChoDelete( ws, idev )
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
{
    int i;
    char **s;
 
    XFreeGC( ws->dpy, idev->gc );
 
    switch( idev->data.cho.initst.pet ) {
    case 1:
        break;
    case 2:
        free( idev->data.cho.initst.record.pet2.enable );
        break;
    case 4:
    case 3:    /* free the strings and the array that holds the strings */
        s = idev->data.cho.initst.record.pet3.strings;
        for( i=0; i < idev->data.cho.initst.record.pet3.number; i++, s++)
            free( *s );
        free( idev->data.cho.initst.record.pet3.strings );
        break;
    default:
        break;
    }
}
