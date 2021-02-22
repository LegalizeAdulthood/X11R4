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
 * string.c - routines for GKS STRING input device
 *
 */

#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

#include <strings.h>
#include <signal.h>
#include <unistd.h>

#include "gks_implem.h"

#define DEF_STR_BUFSIZ    1024
#define DEF_STR_FONT    "8x13"

static XFontStruct *MFontInfo = (XFontStruct *)NULL;

static Bool XgksCreateDefString(WS_STATE_ENTRY *ws, Gint dev, INPUT_DEV **idevp);

int XgksSIGIO_OFF(Display *dpy);
int XgksSIGIO_ON(Display *dpy);
Gint XgksStrUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, XKeyPressedEvent *xev, int event_id);
void XgksStrDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);

/*
 * INITIALISE STRING
 *
 * Returns: 0, 7, 20, 25, 38, 51, 140, 141, 144, 145, 146, 152, 300
 */

Gint ginitstring(Gint ws_id, Gint dev, Gchar *init, Gint pet, Glimit *area, Gstringrec *record)
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;

/* STEP 1: check for errors */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginitstring)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginitstring)

/* open wsid? */
        GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginitstring )

/* valid workstation type */
        GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errginitstring)

/* valid echo area */
        GKSERROR( (area->xmin >= area->xmax || area->ymin >= area->ymax), 51, errginitstring )

/* valid string device number */
        GKSERROR( (dev < 1), 140, errginitstring )

/* valid and supported prompt mode? */
        GKSERROR( (pet != 1), 144, errginitstring)

/* Echo inside display space? */
        GKSERROR( (area->xmin < 0 || area->xmax > ws->size.x
                || area->ymin < 0 || area->ymax > ws->size.y),
                145, errginitstring )

/* data record contents valid */
    GKSERROR( (record->pet1.bufsiz <= 0), 146, errginitstring)
    GKSERROR( (record->pet1.position <= 0), 146, errginitstring)
    GKSERROR( (record->pet1.position > (STRLEN(init) + 1)), 146, errginitstring)
    GKSERROR( (STRLEN(init) > record->pet1.bufsiz), 154, errginitstring)

        if ((idev = XgksIDevLookup( ws, dev, GISTRING)) == NULL) {
        /* Create the Input Device structure */
        GKSERROR( XgksCreateDefString( ws, dev, &idev ), 300, errginitstring)
        }
        else {
        /* if the device is not in REQUEST mode, not allowed to initialize it */
                GKSERROR( idev->data.str.initst.mode != GREQUEST, 141, errginitstring )
        }
    if ( idev->data.str.initst.string != NULL)
        free( idev->data.str.initst.string );
        idev->data.str.initst.string = (Gchar *)malloc( STRLEN(init) +1);
    GKSERROR( idev->data.str.initst.string == NULL, 300, errginitstring)
    STRCPY( idev->data.str.initst.string, init);
        idev->data.str.initst.pet    = pet;
        idev->data.str.initst.e_area = *area;
        idev->data.str.initst.record = *record;
        idev->data.str.initst.record.pet1.position--;
    free( idev->data.str.strbuf );
    idev->data.str.strbuf = (Gchar *) malloc( sizeof( char ) * idev->data.str.initst.record.pet1.bufsiz);
    GKSERROR( idev->data.str.strbuf == NULL, 300, errginitstring)
    idev->data.str.strbuf[0] = '\0';

    return(0);
}

/*
 * SET STRING MODE
 *
 * Returns: 0, 7, 20, 25, 38, 140, 143, 500
 */

Gint gsetstringmode(Gint ws_id, Gint dev, Gimode mode, Gesw echo)
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;

/* STEP 1: check for errors */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsetstringmode)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetstringmode)

/* open wsid? */
        GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgsetstringmode )

/* valid workstation type */
        GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgsetstringmode)

/* valid string device number */
        GKSERROR( (dev < 1), 140, errgsetstringmode )

/* check enumerations */
    GKSERROR( ((mode != GREQUEST && mode != GSAMPLE && mode != GEVENT)
        || (echo != GECHO && echo != GNOECHO) ),
        500, errgsetstringmode);

        if ((idev = XgksIDevLookup( ws, dev, GISTRING)) == NULL) {
        /* Create the Input Device structure */
        GKSERROR( XgksCreateDefString( ws, dev, &idev ), 300, errgsetstringmode)
        }
        else {
        if ((idev->active == True) && (idev->data.str.initst.esw == GECHO))
        XgksStrUpdatePrompt( ws, idev, PROMPTOFF,
             (XKeyPressedEvent *)NULL,-1 );
        }
    idev->data.str.initst.mode = mode;
    idev->data.str.initst.esw = echo;

    if (mode == GSAMPLE || mode == GEVENT) {
        STRCPY( idev->data.str.strbuf, idev->data.str.initst.string );
        idev->data.str.editpos = idev->data.str.initst.record.pet1.position;
        if ( mode == GEVENT )
           signal( SIGALRM, XgksAwaitInterrupt); /* Set signal handler for event mode */
        idev->active = True;
        if ( echo == GECHO )
        XgksStrUpdatePrompt( ws, idev, PROMPTON,
          (XKeyPressedEvent *)NULL,-1 );
    }
    else    /* GREQUEST */
        idev->active = False;

    return(0);
}

/*
 * REQUEST STRING
 *
 * Returns: 0, 7, 20, 25, 38, 140, 141
 */

Gint greqstring(Gint ws_id, Gint dev, Gqstring *response)
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;

/* STEP 1: check for errors */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgreqstring)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgreqstring)

/* open wsid? */
        GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgreqstring )

/* valid workstation type */
        GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgreqstring)

/* valid string device number */
        GKSERROR( (dev < 1), 140, errgreqstring )

        if ((idev = XgksIDevLookup( ws, dev, GISTRING)) == NULL) {
        /* Create the Input Device structure */
        GKSERROR( XgksCreateDefString( ws, dev, &idev ), 300, errgreqstring)
        }
    else {
        GKSERROR((idev->data.str.initst.mode !=GREQUEST), 141 , errgreqstring)
    }

/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );

/* set up initial value */
    STRCPY( idev->data.str.strbuf, idev->data.str.initst.string);
    idev->data.str.editpos = idev->data.str.initst.record.pet1.position;
    if ( idev->data.str.initst.esw == GECHO )
        XgksStrUpdatePrompt( ws, idev, PROMPTON,
          (XKeyPressedEvent *)NULL,-1 );
    idev->active = True;

/* wait for trigger or break */
    idev->touched = False;
    idev->breakhit = False;
    while ( (idev->touched == False) && (idev->breakhit == False) )
        pause();

    idev->active = False;
    if ( idev->data.str.initst.esw == GECHO )
        XgksStrUpdatePrompt( ws, idev, PROMPTOFF,
           (XKeyPressedEvent *)NULL, -1 );

    if ( idev->breakhit == True ) {
        response->status = GNONE;
    }
    else {
        response->status = GOK;
        STRCPY( response->string, idev->data.str.strbuf );
    }

    return(0);
}

/*
 * SAMPLE STRING
 *
 * Returns: 0, 7, 20, 25, 38, 140, 142
 *
 * NOTE: response must be allocated by the user and should be at least as
 *    large as the buffer size specified in the call to ginitstring() or
 *    the default size DEF_STR_BUFSIZ.
 */

Gint gsamplestring(Gint ws_id, Gint dev, Gchar *response)
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;

/* STEP 1: check for errors */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsamplestring)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsamplestring)

/* open wsid? */
        GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgsamplestring )

/* valid workstation type */
        GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgsamplestring)

/* valid string device number */
        GKSERROR( (dev < 1), 140, errgsamplestring )

        idev = XgksIDevLookup( ws, dev, GISTRING);
        GKSERROR( (idev == NULL) || (idev->data.str.initst.mode != GSAMPLE), 142, errgsamplestring)

/* Make sure the workstation is up to date */
    gupdatews( ws_id, GPERFORM );

    STRCPY( response, idev->data.str.strbuf );

    return(0);
}

/*
 * INQUIRE STRING DEVICE STATE
 *
 * Errors: 0, 7, 20, 25, 38, 140
 *
 * NOTE: state->string is malloc'ed by GKS and must be freed by the user.
 */

Gint ginqstringst(Gint ws_id, Gint dev, Gstringst *state)
{
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;

/* STEP 1: check for errors */
    GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqstringst)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqstringst)

/* open wsid? */
        GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errginqstringst )

/* valid workstation type */
        GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errginqstringst)

/* valid string device number */
        GKSERROR( (dev < 1), 140, errginqstringst )

        if ((idev = XgksIDevLookup( ws, dev, GISTRING)) == NULL) {
        /* Create the Input Device structure */
        GKSERROR( XgksCreateDefString( ws, dev, &idev ), 300, errginqstringst)
        }
    *state = idev->data.str.initst;
        state->record.pet1.position++;
    state->string = (Gchar *)malloc( STRLEN(idev->data.str.initst.string) +1);
    GKSERROR( state->string == NULL, 300, errginqstringst)
    STRCPY( state->string, idev->data.str.initst.string );

    return( 0 );
}

/*
 * INQUIRE DEFAULT STRING DEVICE DATA
 *
 * Errors: 0, 8, 22, 23, 38, 140
 */

Gint ginqdefstring(Gchar *type, Gint dev, Gdefstring *data)
{
    EWSTYPE ewstype;

/* STEP 1: check for errors */
    GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqdefstring)

/* valid wsid? */
    ewstype = XgksWsTypeToEnum( type );
    GKSERROR( ewstype == WST_INVALID, 22, errginqdefstring )
    GKSERROR( ewstype != X_WIN, 38, errginqdefstring )

/* valid string dev number */
    GKSERROR( dev < 1, 140, errginqdefstring )

    data->bufsiz = DEF_STR_BUFSIZ;
    data->pets.number = 1;
    data->pets.integers = (Gint *)malloc( sizeof( int ) );
    GKSERROR( data->pets.integers == NULL, 300, errginqdefstring)
    data->pets.integers[0] = 1;
    data->e_area.xmin = 0.0;
    data->e_area.xmax = WS_MAX_DCX;
    data->e_area.ymin = 0.0;
    data->e_area.ymax = WS_MAX_DCY;
    data->record.pet1.bufsiz = DEF_STR_BUFSIZ;
    data->record.pet1.position = 1;
    data->record.pet1.data = NULL;

    return( 0 );
}

/*
 * create a default string device.  returns True if cannot create.
 */
static Bool XgksCreateDefString(WS_STATE_ENTRY *ws, Gint dev, INPUT_DEV **idevp)
{
    INPUT_DEV *idev;
    XGCValues gcvalues;

/* Create the Input Device structure */
    idev = XgksIDevNew();
    if (idev == NULL)
        return( True );
    idev->class = GISTRING;
    idev->dev = dev;
    idev->active = False;
    gcvalues.function = GXcopy;
    gcvalues.foreground = ws->wsfg;
    gcvalues.background = ws->wsbg;
    gcvalues.fill_style = FillSolid;
    if (MFontInfo == NULL) {
        XgksSIGIO_OFF(ws->dpy);
        MFontInfo = XLoadQueryFont( ws->dpy, DEF_STR_FONT );
        XgksSIGIO_ON(ws->dpy);
    }
    gcvalues.font = MFontInfo->fid;

    idev->gc = XCreateGC( ws->dpy, ws->win,
        GCFunction | GCForeground | GCBackground |
        GCFillStyle | GCFont, &gcvalues);
    idev->data.str.initst.mode = GREQUEST;
    idev->data.str.initst.esw = GECHO;
    idev->data.str.initst.string = NULL;
    idev->data.str.initst.pet = 1;
    idev->data.str.initst.e_area.xmin = 0.0;
    idev->data.str.initst.e_area.xmax = ws->size.x;
    idev->data.str.initst.e_area.ymin = 0.0;
    idev->data.str.initst.e_area.ymax = ws->size.y;
    idev->data.str.initst.record.pet1.bufsiz = DEF_STR_BUFSIZ;
    idev->data.str.initst.record.pet1.position = 0;
    idev->data.str.initst.record.pet1.data = NULL;

    idev->data.str.strbuf = (Gchar *) malloc( sizeof( char ) * DEF_STR_BUFSIZ);
    if (idev->data.str.strbuf == NULL)
        return( True );
    idev->data.str.strbuf[0] = '\0';
    idev->data.str.editpos = 0;
    idev->data.str.curpos.x = 0;
    idev->data.str.curpos.y = MFontInfo->ascent;

/* link the new device into the list */
    idev->next = ws->in_dev_list;
    ws->in_dev_list = idev;

    *idevp = idev;
    return( False );
}

/*
 * XgksStrUpdatePrompt
 */

#define MIN(a,b)  (( a > b ) ? b : a )
#define FG    ws->wsfg
#define BG    ws->wsbg
#define PADH    2
#define PADV    2
#define StrX    idev->data.str.curpos.x
#define StrY    idev->data.str.curpos.y
#define StrPos    idev->data.str.editpos
#define Str    idev->data.str.strbuf
#define StrBufSiz idev->data.str.initst.record.pet1.bufsiz
#define StrInitPos idev->data.str.initst.record.pet1.position

Gint XgksStrUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, XKeyPressedEvent *xev, int event_id)
{
    Gpoint dcpt;
    XPoint xpt;
    XRectangle rect;
    KeySym ksym;
    char keybuf[2];
    Gchar *data;
    int count, wid;
    int place;

/* Set up clipping area */
    dcpt.x = idev->data.str.initst.e_area.xmin;
    dcpt.y = idev->data.str.initst.e_area.ymax;
    DcToX( ws, &dcpt, &xpt );
    rect.x = xpt.x;
    rect.y = xpt.y;

    dcpt.x = idev->data.str.initst.e_area.xmax;
    dcpt.y = idev->data.str.initst.e_area.ymin;
    DcToX( ws, &dcpt, &xpt );
    rect.width = MIN (xpt.x - rect.x, XTextWidth (MFontInfo, "W", StrBufSiz + 1));
         /* box width is limited to length of input buf or size of echo area  */
    rect.height = xpt.y - rect.y;

    XSetClipRectangles( ws->dpy, idev->gc, 0, 0, &rect, 1, Unsorted );

/* do the prompting. NOTE: We know that there is only one prompt type (pet==1)*/
    switch( pstate ) {
    case PROMPTON:
    /* New prompt, must display whole string and find cursor position */
        XSetForeground( ws->dpy, idev->gc, BG);
        XFillRectangle( ws->dpy, ws->win, idev->gc, rect.x, rect.y,
            rect.width, MFontInfo->ascent + MFontInfo->descent + PADV + PADV );
        XSetForeground( ws->dpy, idev->gc, FG);
        XDrawRectangle( ws->dpy, ws->win, idev->gc, rect.x, rect.y,
            rect.width -1, MFontInfo->ascent + MFontInfo->descent + PADV + PADV );
    /* X text string Y coordinate specifies the baseline */
        StrX = rect.x + PADH;
        StrY = rect.y + MFontInfo->ascent + PADV;
    /*  Draw initial string  */
        XDrawString( ws->dpy, ws->win, idev->gc, StrX, StrY, Str, STRLEN( Str ) );
    /* text cursor */
        /* Start cursor at current cursor position - this option is called on update/repaint */
        StrX += XTextWidth( MFontInfo, Str, StrPos );
        XFillRectangle( ws->dpy, ws->win, idev->gc, StrX,
            StrY-MFontInfo->ascent, MFontInfo->max_bounds.width,
            MFontInfo->ascent + MFontInfo->descent );
        XSetForeground( ws->dpy, idev->gc, BG);
        if (StrPos < STRLEN(Str))
           XDrawString( ws->dpy, ws->win, idev->gc, StrX, StrY, &(Str[StrPos]), 1 );
                                                  /* This code shows char under cursor */
        break;
    case PROMPTOFF:
    /* cover the area with the background colour */
        XSetForeground( ws->dpy, idev->gc, BG);
        XFillRectangle( ws->dpy, ws->win, idev->gc, rect.x, rect.y,
      rect.width, MFontInfo->ascent + MFontInfo->descent + PADV + PADV + PADV );
        break;
    case PROMPTMOVE:
    /* turn off text cursor */
    if(idev->data.str.initst.esw==GECHO){
                XSetForeground( ws->dpy, idev->gc, BG);
                XFillRectangle(ws->dpy, ws->win, idev->gc, StrX,
            StrY-MFontInfo->ascent, MFontInfo->max_bounds.width,
            MFontInfo->ascent + MFontInfo->descent );
            XSetForeground( ws->dpy, idev->gc, FG);
               }
    /* interpret and display character just typed. */
        count = XLookupString( xev, keybuf, 1, &ksym, NULL );
        keybuf[1] = '\0';
        if ( ksym == XK_BackSpace || keybuf[0] == '\b' ) {
            if ( StrPos != 0 ) {
            /* Delete last character */
                wid = XTextWidth( MFontInfo, &(Str[ --StrPos]), 1);
                StrX -= wid;
            if(idev->data.str.initst.esw==GECHO){
            /* Redraw char cursor was just on and fix rectangle */
                    XDrawString( ws->dpy, ws->win, idev->gc, StrX + wid, StrY, &(Str[StrPos+1]), 1 );
                    XDrawRectangle( ws->dpy, ws->win, idev->gc, rect.x, rect.y,
                    rect.width -1, MFontInfo->ascent + MFontInfo->descent + PADV + PADV );
                             }
            }
        }
        else if (ksym == XK_Delete) { /* user hit Delete key - delete chars past cursor */
           for (place = StrPos; place < STRLEN(Str); place++ ) /* rem char from Str */
              Str[place] = Str[place+1];
           if(idev->data.str.initst.esw==GECHO){  /* only redraw if ECHO */
                 XSetForeground( ws->dpy, idev->gc, BG);
                 XFillRectangle( ws->dpy, ws->win, idev->gc, StrX,   /* clear string from StrPos on */
                StrY-MFontInfo->ascent, xpt.x - StrX - PADH,
                 MFontInfo->ascent + MFontInfo->descent );
                XSetForeground( ws->dpy, idev->gc, FG);
                /* Redraw new Str from StrPos on */
                XDrawString( ws->dpy, ws->win, idev->gc, StrX, StrY, &(Str[StrPos]), STRLEN(Str)-StrPos);
                /* Redraw rectangle */
                XDrawRectangle( ws->dpy, ws->win, idev->gc, rect.x, rect.y,
                  rect.width -1, MFontInfo->ascent + MFontInfo->descent + PADV + PADV );
           }
        }
        else if (ksym == XK_Linefeed || ksym == XK_Return
            || (count > 0 && (keybuf[0] == '\n' || keybuf[0] == '\r')) ) {
        /* enter pressed, accept input string */
            switch( idev->data.str.initst.mode ) {
            case GREQUEST:
                idev->touched = True;
                XBell( ws->dpy, 0 );
                break;
            case GSAMPLE:
                break;
            case GEVENT:
                data = (Gchar *)malloc( STRLEN( Str ) +1 );
                if (data == NULL)
                {
                    gerrorhand( 300, errXgksStrUpdatePrompt, xgks_state.gks_err_file);
                    return(300);
                }
                
                    XBell( ws->dpy, 0 );
                    STRCPY( data, Str );
                    XgksEnqueueEvent( ws->ws_id, idev->dev, GISTRING, (char *)data, event_id );
                
                break;
            }
        }
        else if (ksym == XK_Break) {
        }
        else if (ksym == XK_Tab) {
        }
        else if (count > 0) {
        /* if we're too close to the right edge, don't accept it */
            if (((StrX + MFontInfo->max_bounds.width + PADH) > xpt.x) ||
                (StrPos == StrBufSiz)) {      /* check if input exceeds BufSiz */
                XBell( ws->dpy, 0 );
            }
            else {
            if (Str[StrPos] == '\0')   /* if we're at end of Str, move null char out 1 pos */ 
               Str[StrPos + 1] = '\0';
        /* Copy new char into Str */
            Str[StrPos++] = keybuf[0];
            if(idev->data.str.initst.esw==GECHO)
        /* show the new character */
               XDrawString( ws->dpy, ws->win, idev->gc, StrX, StrY, keybuf, 1 );
            StrX +=  XTextWidth( MFontInfo, keybuf, 1);
            }
        }

    /* turn on text cursor */
    if(idev->data.str.initst.esw==GECHO){
        XSetForeground( ws->dpy, idev->gc, FG);
        XFillRectangle( ws->dpy, ws->win, idev->gc, StrX,
            StrY-MFontInfo->ascent, MFontInfo->max_bounds.width,
            MFontInfo->ascent + MFontInfo->descent );
        XSetForeground( ws->dpy, idev->gc, BG);
        if (StrPos < STRLEN(Str))
            XDrawString( ws->dpy, ws->win, idev->gc, StrX, StrY, &(Str[StrPos]), 1 );
                                                /*  This code shows the char under cursor */
                }
        break;
    default:
        break;
    }
    keybuf[0] = ' ';  /* clear buf so a following shift isn't seen as another bksp  PTR# c1047*/
    XFlush( ws->dpy );
    return( 0 );
}

/*
 * Delete all structures used to maintain a string logical input device.
 */
void XgksStrDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev)
{
    XFreeGC( ws->dpy, idev->gc );

    free( idev->data.str.strbuf );
    if ( idev->data.str.initst.string != NULL )
        free( idev->data.str.initst.string );
}
