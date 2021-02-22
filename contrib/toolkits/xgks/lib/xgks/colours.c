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
 *  All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * colours.c - functions and data for the colour table.
 *
 */

#include "gks_implem.h"
#include "metafile.h"
#include "x/colours.h"
#include "x/xevent.h"

static int loadcache();

Gint XgksMaxColours(Gchar *ws_type);

/*$F
 * gsetcolourrep(ws_id, idx, rep) - SET COLOR REPRESENTATION
 *
 * Gint ws_id;            workstation identifier
 * Gint    idx;            which entry to change
 * Gcobundl *rep        colour intensities.
 *
 * returns 0=OK, or one of 7, 20, 25, 33, 35, 36, 93, 96
 *
 * See also: ANSI Standard p.105
 */
Gint gsetcolourrep(ws_id, idx, rep)
    Gint ws_id;
    Gint  idx;
    Gcobundl *rep;
{
    WS_STATE_PTR ws;
    int i,status;

/* check for proper gks operating state */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgsetcolourrep)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetcolourrep)

/* check if ws is opened */
    GKSERROR (((ws = OPEN_WSID(ws_id))==NULL), 25, errgsetcolourrep)

/* check workstation category */
    GKSERROR ((WS_CAT(ws) == GMI), 33, errgsetcolourrep)
    GKSERROR ((WS_CAT(ws) == GINPUT) ,35, errgsetcolourrep)
    GKSERROR ((WS_CAT(ws) == GWISS) ,36, errgsetcolourrep)

/* check for valid idx */
    GKSERROR ((!WS_AVAIL_COLOUR(ws, idx)) ,93, errgsetcolourrep)

/* check for colours within range 0..1 */
    GKSERROR (((rep->red < 0.0 || rep->red > 1.0) || (rep-> green < 0.0 || rep->green > 1.0) ||
           (rep->blue < 0.0 || rep->blue > 1.0)) ,96, errgsetcolourrep)

/* STEP 2: send it to the workstation */
    status = xXgksSetColourRep(ws, idx, rep);
    GKSERROR ((status != 0) ,93, errgsetcolourrep)

    /* if this is the 1st call to setcolourrep, alloc & init */
    /* storage of set values (ws->set_colour_rep[]) */
    if (ws->set_colour_rep == (Gcobundl *)NULL)
       {
       /* allocate  & init memory for set colour   */
       /* rep table for use in ginqcolourrep */
       ws->set_colour_rep = (Gcobundl *)
             calloc(ws->wscolour, sizeof(Gcobundl));
       GKSERROR ((!ws->set_colour_rep),300,errgsetcolourrep)
       for (i=0; i < ws->wscolour; i++)
         {
         ws->set_colour_rep[i].red = NOT_SET;
         ws->set_colour_rep[i].green = NOT_SET;
         ws->set_colour_rep[i].blue = NOT_SET;
         }
       }

    /* remember values used for inqcolourrep(set) */
    ws->set_colour_rep[idx].red = rep->red;
    ws->set_colour_rep[idx].green = rep->green;
    ws->set_colour_rep[idx].blue = rep->blue;

    if (ws->ewstype == MO)
        XgksMoSetColourRep (ws, idx, rep);

    return(0);
}

/*$F
 * ginqcolourrep(ws_id, idx, type, rep) - INQUERY COLOR REPRESENTATION
 *
 * Gint ws_id;            workstation identifier
 * Gint    idx;            which entry to change
 * Gqtype type;                 type of return values
 * Gcobundl *rep        colour intensities.
 *
 * returns 0=OK, or one of 7, 20, 25, 33, 35, 36, 93
 *
 * See also: ANSI Standard p.164
 */
Gint ginqcolourrep(ws_id, idx, type, rep)
    Gint ws_id;
    Gint  idx;
    Gqtype type;
    Gcobundl *rep;
{
    WS_STATE_PTR ws;
    int status;

/* check for proper gks operating state */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqcolourrep)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqcolourrep)

/* check if ws is opened */
    GKSERROR (((ws = OPEN_WSID(ws_id))==NULL), 25, errginqcolourrep)

/* check workstation category */
    GKSERROR ((WS_CAT(ws) == GMI), 33, errginqcolourrep)
    GKSERROR ((WS_CAT(ws) == GINPUT) ,35, errginqcolourrep)
    GKSERROR ((WS_CAT(ws) == GWISS) ,36, errginqcolourrep)

/* check for valid idx */
    GKSERROR ((!WS_AVAIL_COLOUR(ws, idx)) ,93, errginqcolourrep)

    /* if the user wants SET, and some have been set, and */
    /* idx has been set....                               */
    if (type == GSET  &&  ws->set_colour_rep != NULL  &&
           ws->set_colour_rep[idx].red >= 0.0 )
       { /* give him SET... */
       rep->red = ws->set_colour_rep[idx].red;
       rep->green = ws->set_colour_rep[idx].green;
       rep->blue = ws->set_colour_rep[idx].blue;
       }
    else /* type == GREALIZED */
       { /* give him REALIZED */
       /* call the X routine */
       status = xXgksInqColourRep(ws, idx, type, rep);
       GKSERROR ((status != 0) ,93, errginqcolourrep)
       }

    return(0);
}

/*$F
 * INQUIRE LIST OF COLOR INDICES
 *
 * errors 0, 7, 20, 25, 33, 35, 36
 *
 * See also: ANSI Standard p.164
 */

Gint ginqcolourindices(ws_id, indices)
    Gint ws_id;
    Gintlist *indices;
{
    WS_STATE_ENTRY *ws;
    int i;

/* check for proper gks operating state */
    GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqcolourindices)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqcolourindices)

/* check if ws is opened */
    GKSERROR (((ws = OPEN_WSID(ws_id))==NULL), 25, errginqcolourindices)

/* check workstation category */
    GKSERROR ((WS_CAT(ws) == GMI), 33, errginqcolourindices)
    GKSERROR ((WS_CAT(ws) == GINPUT), 35, errginqcolourindices)
    GKSERROR ((WS_CAT(ws) == GWISS), 36, errginqcolourindices)

    indices->number = ws->wscolour;
    indices->integers = (Gint *) malloc( sizeof( int ) * ws->wscolour );
    GKSERROR (indices->integers == NULL, 300, errginqcolourindices)
    for( i=0; i<ws->wscolour; i++ )
        indices->integers[i] = i;

    return( OK );
}

/*$F
 * INQUIRE COLOUR FACILITIES
 *
 * errors 0, 8, 22, 23, 39
 *
 * See also: ANSI Standard p.180
 */

Gint ginqcolourfacil(ws_type, fac )
    Gchar *ws_type;
    Gcofac *fac;
{
    EWSTYPE ewstype;

/* check for proper gks operating state */
    GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqcolourfacil);

/* check valid ws_type */
    ewstype = XgksWsTypeToEnum( ws_type );
    GKSERROR( ewstype == WST_INVALID, 22, errginqcolourfacil);
    GKSERROR( ewstype != X_WIN, 39, errginqcolourfacil);

/* set up the return values */
    fac->colours = XgksMaxColours( ws_type );
    GKSERROR( fac->colours < 0, 22, errginqcolourfacil);
    fac->coavail = ( fac->colours > 2 ) ? GCOLOUR : GMONOCHROME;
    fac->predefined = fac->colours;

    return( OK );
}

/*$F
 * INQUIRE PREDEFINED COLOUR REPRESENTATION
 *
 * errors 0, 8, 22, 23, 39, 86
 *
 * See also: ANSI Standard p.180
 */

/* cache */
static Display *CprevDpy = NULL;
static int Cncolours = 0;
static XColor *Ccolours = NULL;

Gint ginqpredcolourrep( ws_type, index, rep )
    Gchar *ws_type;
    Gint index;
    Gcobundl *rep;
{
    EWSTYPE ewstype;
    char *server;
    int i;
    Display *dpy;
    char *getenv();

/* check for proper gks operating state */
    GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredcolourrep);

/* check valid ws_type */
    ewstype = XgksWsTypeToEnum( ws_type );
    GKSERROR( ewstype == WST_INVALID, 22, errginqpredcolourrep);

/* check workstation category */
    GKSERROR( ewstype != X_WIN, 39, errginqpredcolourrep);

/* check valid colour index */
    GKSERROR( (index < 0), 93, errginqpredcolourrep);

/* ASK THE SERVER */
/* default server is in the Unix environment */
    if ((server = ws_type) == NULL)
        server = getenv( "DISPLAY" );

/* check for existing connection to this server. */
    for( i=0; i<MAX_OPEN_WS; i++ ) {
        if (xgks_state.openedws[i].ws_id == INVALID ||
            xgks_state.openedws[i].ws->ewstype != X_WIN)
            continue;
        if (STRCMP( xgks_state.openedws[i].ws->wstype, server ) == 0)
            break;
    }
/* for the AIX port the SIGIO_OFF was postponed until the display is known #d1 */
    if ( i < MAX_OPEN_WS ) {    /* found a connection */
        dpy = xgks_state.openedws[i].ws->dpy;
        XgksSIGIO_OFF(dpy);
        if ((i=loadcache(dpy, index)) != 0) {
            XgksSIGIO_ON(dpy);
            return(i);
        }
    }
    else {                /* build a connection */
        dpy = XOpenDisplay( server );
        XgksSIGIO_OFF(dpy);
        if ( dpy == NULL ) {
            XgksSIGIO_ON(dpy);
            return( -1 );
        }
        if ((i=loadcache(dpy, index)) != 0) {
            XCloseDisplay( dpy );
            XgksSIGIO_ON(dpy);
            return( i );
        }
        XCloseDisplay( dpy );
    }
    XgksSIGIO_ON(dpy);
    rep->red = (Gfloat)Ccolours[index].red / 65535.0;
    rep->green = (Gfloat)Ccolours[index].green / 65535.0;
    rep->blue = (Gfloat)Ccolours[index].blue / 65535.0;

    return( OK );
}

static int
loadcache(dpy, index)
    Display *dpy;
    Gint index;
{
    Colormap cmap;
    int i, ncolours;
    XColor *clr;

    if ((CprevDpy != dpy) || (CprevDpy == NULL)) {
        ncolours = DisplayCells( dpy, DefaultScreen( dpy ) );
        if ( index >= ncolours ) {
            gerrorhand( 93, errginqpredcolourrep, xgks_state.gks_err_file );
            return( 93 );
        }
        if (Ccolours != NULL)
            free(Ccolours);
        Ccolours = (XColor *)malloc( sizeof(XColor) * ncolours);
        if ( Ccolours == NULL ) {
            gerrorhand( 300, errginqpredcolourrep, xgks_state.gks_err_file );
            CprevDpy = NULL;
            return( 300 );
        }
                for (i=0, clr=Ccolours; i<ncolours; i++, clr++) {
                        clr->pixel = i;
                        clr->flags = DoRed | DoGreen | DoBlue;
                }
        cmap = DefaultColormap( dpy, DefaultScreen(dpy) );
        XQueryColors(dpy, cmap, Ccolours, ncolours);
        CprevDpy = dpy;
        Cncolours = ncolours;
    }
    else {
        if ( index >= Cncolours ) {
            gerrorhand( 93, errginqpredcolourrep, xgks_state.gks_err_file );
            return( 93 );
        }
    }
    return( 0 );
}

/*
 * find the number of colour table entries supportted by an X server.
 * returns the number of entries or -1 if the server does not respond.
 */
int XgksMaxColours( server )
    char *server;
{
    int i, colours;
    Display *dpy;
    char *getenv();


/* wait till dpy is known to turn SIGIO off  AIX PORT #d1 */

/* default server is in the Unix environment */
    if (server == NULL)
        server = getenv( "DISPLAY" );

/* check for existing connection to this server. */
    for( i=0; i<MAX_OPEN_WS; i++ ) {
        if (xgks_state.openedws[i].ws_id == INVALID
           || xgks_state.openedws[i].ws->ewstype != X_WIN)
            continue;
        if (STRCMP( xgks_state.openedws[i].ws->wstype, server ) == 0)
            break;
    }
    if ( i < MAX_OPEN_WS ) {    /* found a connection */
        dpy = xgks_state.openedws[i].ws->dpy;
        XgksSIGIO_OFF(dpy);
        colours = DisplayCells( dpy, DefaultScreen( dpy ) );
    }
    else {                /* build a connection */
        dpy = XOpenDisplay( server );
        XgksSIGIO_OFF(dpy);
        if ( dpy == NULL )
            return( -1 );
        colours = DisplayCells( dpy, DefaultScreen( dpy ) );
        XCloseDisplay( dpy );
    }

    XgksSIGIO_ON(dpy);

    return( colours );
}
