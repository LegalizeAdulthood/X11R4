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
 *      All rights reserved.
 *
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * functions to manipulate the list of logical input devices
 *
 */

#include <X11/cursorfont.h>
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

#include "gks_implem.h"

void XgksChoDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);
void XgksLocDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);
void XgksPicDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);
void XgksStkDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);
void XgksStrDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);
void XgksValDelete(WS_STATE_ENTRY *ws, INPUT_DEV *idev);

Gint XgksChoUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, XMotionEvent *xmev, int event_id);
Gint XgksLocUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, Gpoint *newdcpt, XMotionEvent *xmev, int event_id);
Gint XgksPicUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    Gpoint *newdcpt, XMotionEvent *xmev, int event_id);
Gint XgksStkUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, Gpoint *newdcpt, XMotionEvent *xmev, int event_id);
Gint XgksStrUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, XKeyPressedEvent *xev, int event_id);
Gint XgksValUpdatePrompt(WS_STATE_ENTRY *ws, INPUT_DEV *idev,
    PromptStatus pstate, Gpoint *newdcpt, XMotionEvent *xmev, int event_id);

int XgksSIGIO_OFF(Display *dpy);
int XgksSIGIO_ON(Display *dpy);

/*
 * Initialise the input device list
 */

void XgksInitIDev(WS_STATE_ENTRY *ws)
{
    XSetWindowAttributes xswa;

    ws->in_dev_list = NULL;
    if (ws->ewstype == X_WIN)
    {
        xswa.cursor = XCreateFontCursor(ws->dpy, XC_crosshair);
        XChangeWindowAttributes(ws->dpy, ws->win, CWCursor, &xswa);
    }
}

/*
 * Add a logical device to the list
 */
void XgksIDevAdd(WS_STATE_ENTRY *ws, INPUT_DEV *idev)
{
    idev->next = ws->in_dev_list;
    ws->in_dev_list = idev;
}

/*
 * Cleaning up on input device queue.
 * This routine will free up all memory assoicated with input device structure
 *
 */

void XgksIDevDelete(WS_STATE_ENTRY *ws)
{
    INPUT_DEV *idev = ws->in_dev_list;

    while (idev != NULL)
    {
        switch (idev->class)
        {
        case GLOCATOR:
            XgksLocDelete(ws, idev);
            break;
        case GISTROKE:
            XgksStkDelete(ws, idev);
            break;
        case GVALUATOR:
            XgksValDelete(ws, idev);
            break;
        case GCHOICE:
            XgksChoDelete(ws, idev);
            break;
        case GPICK:
            XgksPicDelete(ws, idev);
            break;
        case GISTRING:
            XgksStrDelete(ws, idev);
            break;
        }
        idev = ws->in_dev_list->next;
        free(ws->in_dev_list);
        ws->in_dev_list = idev;
    }
}

/*
 * Allocate memory for a new logical input device, returns pointer
 * to the new device structure, or NULL is malloc fails.
 *   Some Default initilation are done ...
 *
 *     idev->class    = GNCLASS;
 *     idev->dev      = INVALID;
 *     idev->active   = FALSE;
 *     idev->touched  = FALSE;
 *     idev->breakhit = FALSE;
 *     idev->next = NULL;
 *
 */

INPUT_DEV *XgksIDevNew(void)
{
    INPUT_DEV *new;

    if ((new = (INPUT_DEV *) malloc(sizeof(INPUT_DEV))) != NULL)
    {
        new->class = GNCLASS;
        new->dev = INVALID;
        new->active = FALSE;
        new->touched = FALSE;
        new->breakhit = FALSE;
        new->next = NULL;
    }
    return (new);
}

/*
 * Search the input device list for a specific device.  Return the device or
 * a NULL pointer.
 */

INPUT_DEV *XgksIDevLookup(WS_STATE_ENTRY *ws, Gint dev, Giclass class)
{
    INPUT_DEV *id;

    for (id = ws->in_dev_list; id != NULL; id = id->next)
        if ((id->class == class) && (id->dev == dev))
            return (id);
    return ((INPUT_DEV *) NULL);
}

/*
 * Process X events and see if they trigger any GKS logical input devices
 *
 * XEvent can be one of: MotionNotify
 */
void XgksIProcessXEvent(XEvent *xev)
{
    int i;
    WS_STATE_ENTRY *ws;
    INPUT_DEV *idev;
    Bool breakhit;
    KeySym ksym;
    Gpoint dcpt;
    XPoint xpt;
    Glimit *ea;
#define InEchoArea (dcpt.x >= ea->xmin && dcpt.x <= ea->xmax \
    && dcpt.y >= ea->ymin && dcpt.y <= ea->ymax)

    XMotionEvent *xmev;

    /* simultaneous events have same ID! */
    static int current_event_id = 0;
#define MAX_EVENT_ID 32767

    xmev = (XMotionEvent *) xev;
    /* Find the gks workstation associated with the X window */
    for (i = 0; i < MAX_OPEN_WS; i++)
    {
        if (xgks_state.openedws[i].ws_id == INVALID)
            continue;
        if (xgks_state.openedws[i].win == xmev->window)
            break;
    }
    if (i == MAX_OPEN_WS) /* NOT FOUND */
        return;

    ws = xgks_state.openedws[i].ws;

    /* if the closing of this ws has already been initiated, */
    /* don't try to do anything.  (by the time we get to the */
    /* inquire color map the actual X window may be gone -   */
    /* producing an X error!) */
    if (ws->ws_is_closing)
        return;

    switch (xev->type)
    {
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
    case KeyPress:
        /* transform point from X space to DC space */
        xpt.x = xmev->x;
        xpt.y = xmev->y;
        XToDc(ws, &xpt, &dcpt);

        if (xev->type == KeyPress)
        {
            XLookupString((XKeyEvent *) xev, NULL, 0, &ksym, NULL);
            breakhit = (ksym == XK_Pause) || (ksym == XK_Break) ? True : False;
        }
        else
            breakhit = False;
#ifdef IDEVDEBUG
        fprintf(stderr, "XgksIProcessXEvent() xmev %d %d xpt=%d %d dcpt %f %f\n", xmev->x, xmev->y, xpt.x, xpt.y, dcpt.x, dcpt.y);
#endif

        /* increment curr event ID.  (used */
        /* to determine if two events are  */
        /* simultaneous events - they are  */
        /* if they have the same ID        */
        if (++current_event_id > MAX_EVENT_ID)
            current_event_id = 0;

        /* check each active input device for trigger */
        for (idev = ws->in_dev_list; idev != NULL; idev = idev->next)
        {
            if (idev->active == False)
                continue;
            /* check echo area */
            switch (idev->class)
            {
            case GLOCATOR:
                ea = &idev->data.loc.initst.e_area;
                if (InEchoArea)
                    if (breakhit)
                        idev->breakhit = True;
                    else if (xev->type != KeyPress)
                        XgksLocUpdatePrompt(ws, idev, PROMPTMOVE, &dcpt, xmev, current_event_id);
                break;
            case GCHOICE:
                ea = &idev->data.cho.initst.e_area;
#ifdef IDEVDEBUG
                fprintf(stderr, "XgksIProcessXEvent: dcpt %f %f Cho earea = %f %f %f %f\n",
                    dcpt.x, dcpt.y, ea->xmin, ea->xmax, ea->ymin, ea->ymax);
                if (InEchoArea)
                    fprintf(stderr, "XgksIProcessXEvent: calling XgksChoUpdatePrompt\n");
#endif
                if (InEchoArea)
                    if (breakhit)
                        idev->breakhit = True;
                    else
                    {
                        if (((idev->data.cho.initst.pet == 3) && (xev->type != KeyPress))
                            || ((idev->data.cho.initst.pet == 1) && (xev->type == KeyPress))
                            || ((idev->data.cho.initst.pet == 2) && (xev->type == KeyPress)))

                            XgksChoUpdatePrompt(ws, idev, PROMPTMOVE, xmev, current_event_id);
                    }
                break;
            case GVALUATOR:
                ea = &idev->data.val.initst.e_area;
                if (InEchoArea)
                    if (breakhit)
                        idev->breakhit = True;
                    else if (xev->type != KeyPress)
                        XgksValUpdatePrompt(ws, idev, PROMPTMOVE, &dcpt, xmev, current_event_id);
                break;
            case GPICK:
                ea = &idev->data.pic.initst.e_area;
#ifdef IDEVDEBUG
                if (InEchoArea)
                    fprintf(stderr, "XgksIProcessXEvent: device GPICK --> InEchoArea .. %f %f %f %f ?\n",
                        ea->xmin, ea->xmax, ea->ymin, ea->ymax);
                else
                    fprintf(stderr, "XgksIProcessXEvent: device GPICK --> InEchoArea .. %f %f %f %f ?\n",
                        ea->xmin, ea->xmax, ea->ymin, ea->ymax);
#endif
                if (InEchoArea)
                    if (breakhit)
                        idev->breakhit = True;
                    else if (xev->type != KeyPress)
                        XgksPicUpdatePrompt(ws, idev, &dcpt, xmev, current_event_id);
                break;
            case GISTRING:
                ea = &idev->data.str.initst.e_area;
                if (InEchoArea)
                    if (breakhit)
                        idev->breakhit = True;
                    else
                    {
                        if (xev->type == KeyPress)
                            XgksStrUpdatePrompt(ws, idev, PROMPTMOVE, (XKeyEvent *) xmev, current_event_id);
                    }
                break;
            case GISTROKE:
                ea = &idev->data.stk.initst.e_area;
                if (InEchoArea)
                    if (breakhit)
                        idev->breakhit = True;
                    else if (xev->type != KeyPress)
                        XgksStkUpdatePrompt(ws, idev, PROMPTMOVE, &dcpt, xmev, current_event_id);
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}

/*
 * Disable all input devices for a given workstation.
 *      Basically, just get their prompts off the screen so that GKS can
 *      produce some output.
 */
static int DisCount = 0;

void XgksIDevDisable(WS_STATE_ENTRY *ws)
{
    INPUT_DEV *idev;

    DisCount++;

    if (ws->ewstype != X_WIN)
        return;

#ifdef IDEVDEBUG
    fprintf(stderr, "XgksIDevDisable() DisCount = %d\n", DisCount);
#endif
    if (DisCount > 1) /* already done */
        return;

    XgksSIGIO_OFF(ws->dpy);

    /* check each active input device for trigger */
    for (idev = ws->in_dev_list; idev != NULL; idev = idev->next)
    {
        if (idev->active == False)
            continue;
        /* check echo area */
        switch (idev->class)
        {
        case GLOCATOR:
            /* this call is not commented out because  */
            /* loclines are draw in XOR mode. So if we */
            /* didn't erase it now, it would disappear */
            /* when we tried to redraw it later!       */
            if (idev->data.loc.initst.esw == GECHO)
                XgksLocUpdatePrompt(ws, idev, PROMPTOFF,
                    (Gpoint *) NULL, (XMotionEvent *) NULL, -1);
            break;
        case GCHOICE:
            /* This call has been commented out because it caused */
            /* all the devices to flash for each primitive drawn  */
            /*if(idev->data.cho.initst.esw==GECHO)*/
            /*XgksChoUpdatePrompt( ws, idev, PROMPTOFF, (XMotionEvent *)NULL, -1);
                        break;
                case GPICK:
                        /* This call has been commented out because it caused */
            /* all the devices to flash for each primitive drawn  */
            /*if(idev->data.pic.initst.esw==GECHO)*/
            /*XgksPicUpdatePrompt( ws, idev, (Gpoint *)NULL, (XMotionEvent *)NULL, -1);*/
            break;
        case GVALUATOR:
            /* This call has been commented out because it caused */
            /* all the devices to flash for each primitive drawn  */
            /*if(idev->data.val.initst.esw==GECHO)*/
            /*XgksValUpdatePrompt( ws, idev, PROMPTOFF, (Gpoint *)NULL, (XMotionEvent *)NULL, -1);*/
            break;
        case GISTRING:
            /* This call has been commented out because it caused */
            /* all the devices to flash for each primitive drawn  */
            /*if(idev->data.str.initst.esw==GECHO)*/
            /*XgksStrUpdatePrompt( ws, idev, PROMPTOFF, (XKeyPressedEvent *)NULL, -1);*/
            break;
        case GISTROKE:
            /* this call is not commented out because  */
            /* strokes are drawn in XOR mode.  So if we*/
            /* didn't erase it now, it would disappear */
            /* when we tried to redraw it later!       */
            if (idev->data.stk.initst.esw == GECHO)
                XgksStkUpdatePrompt(ws, idev, PROMPTOFF, (Gpoint *) NULL, (XMotionEvent *) NULL, -1);
            break;
        default:
            break;
        }
    }
}

/*
 * Enable all input devices for a given workstation.
 */
void XgksIDevEnable(WS_STATE_ENTRY *ws)
{
    INPUT_DEV *idev;

    DisCount--;

    if (ws->ewstype != X_WIN)
        return;

#ifdef IDEVDEBUG
    fprintf(stderr, "XgksIDevEnable() DisCount = %d\n", DisCount);
#endif
    if (DisCount > 0) /* only disable on last request */
        return;

    /* check each active input device for trigger */
    for (idev = ws->in_dev_list; idev != NULL; idev = idev->next)
    {
        if (idev->active == False)
            continue;
        /* check echo area */
        switch (idev->class)
        {
        case GLOCATOR:
            if (idev->data.loc.initst.esw == GECHO)
                XgksLocUpdatePrompt(ws, idev, PROMPTON, (Gpoint *) NULL, (XMotionEvent *) NULL, -1);
            break;

        case GCHOICE:
            if (idev->data.cho.initst.esw == GECHO)
                XgksChoUpdatePrompt(ws, idev, PROMPTON, (XMotionEvent *) NULL, -1);
            break;

        case GPICK:
            if (idev->data.pic.initst.esw == GECHO)
                XgksPicUpdatePrompt(ws, idev, &(idev->data.pic.curpos), (XMotionEvent *) NULL, -1);
            break;
        case GVALUATOR:
            if (idev->data.val.initst.esw == GECHO)
            {
                Gpoint tmp = { idev->data.val.CurPos, idev->data.val.CurPos };
                XgksValUpdatePrompt(ws, idev, PROMPTON, &tmp, (XMotionEvent *) NULL, -1);
                if (tmp.x != idev->data.val.CurPos)
                    idev->data.val.CurPos = tmp.x;
                else
                    idev->data.val.CurPos = tmp.y;
            }
            break;
        case GISTRING:
            if (idev->data.str.initst.esw == GECHO)
                XgksStrUpdatePrompt(ws, idev, PROMPTON, (XKeyPressedEvent *) NULL, -1);
            break;
        case GISTROKE:
            if (idev->data.stk.initst.esw == GECHO)
                XgksStkUpdatePrompt(ws, idev, PROMPTON, &(idev->data.stk.stkbuf[idev->data.stk.editpos]), (XMotionEvent *) NULL, -1);
            break;
        default:
            break;
        }
    }
    XgksSIGIO_ON(ws->dpy);
}
