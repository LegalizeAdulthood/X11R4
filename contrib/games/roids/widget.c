/*
 * Copyright 1989 Digital Equipment Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Digital Equipment Corporation makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * DIGITAL EQUIPMENT CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Terry Weissman
 *          weissman@decwrl.dec.com
 */

/* widget.c -- handle things that make the playing field behave as a widget. */

#include "roids.h"

void HandleFocus();
void HandleStruct();

static XtActionsRec actions[] = {
    {"grab-focus",	(XtActionProc) TakeFocus},
    {"rotate-left",	RotateLeft},
    {"rotate-right",	RotateRight},
    {"rotate-off",	RotateOff},
    {"thrust-on",	ThrustOn},
    {"thrust-off",	ThrustOff},
    {"rotate-to-point",	RotateToPoint},
    {"stop-rotate",	StopRotateToPoint},
    {"rotate-moved",	RotateMouseMoved},
    {"fire",		Fire},
    {"quit",		Quit},
};

static char defaultTranslation[] =
    "<Btn1Down>:	grab-focus() rotate-to-point()\n\
     Button1<PtrMoved>:	rotate-moved()\n\
     <Btn1Up>:		stop-rotate()\n\
     <Btn2Down>:	thrust-on()\n\
     <Btn2Up>:		thrust-off()\n\
     <Btn3Down>:	fire()\n\
     <KeyDown>z:	rotate-left()\n\
     <KeyUp>z:		rotate-off()\n\
     <KeyDown>x:	rotate-right()\n\
     <KeyUp>x:		rotate-off()\n\
     <KeyDown>\\,:	thrust-on()\n\
     <KeyUp>\\,:	thrust-off()\n\
     <KeyDown>.:	fire()\n\
     <KeyDown>q:	quit()";






static void ClassInitialize() {}

static void Initialize(request, w)
Widget request, w;
{
    gamewidget = (RoidsWidget) w;
    XtAddEventHandler(toplevel, (EventMask) FocusChangeMask, FALSE,
		      HandleFocus, (Opaque) NULL);
    XtAddEventHandler(toplevel, (EventMask) StructureNotifyMask, FALSE,
		      HandleStruct, (Opaque) NULL);
}


static void Realize(w, valueMask, attributes)
Widget w;
Mask *valueMask;
XSetWindowAttributes *attributes;
{
    int i;
    XGCValues values;

/*
 * Safety check.
 */

    if (w->core.width == 0) w->core.width = 1;
    if (w->core.height == 0) w->core.height = 1;

    XtCreateWindow(w, (unsigned int) InputOutput, (Visual *) CopyFromParent,
		   *valueMask, attributes);
    gamewidth = w->core.width;
    gameheight = w->core.height;
    gamewindow = w->core.window;
    values.foreground = w->core.background_pixel;
    values.background = w->core.background_pixel;
    backgc = XtGetGC(w, (XtGCMask) GCForeground | GCBackground, &values);
    values.foreground = shippixel;
    shipgc = XtGetGC(w, (XtGCMask) GCForeground | GCBackground, &values);
    values.foreground = rockpixel;
    rockgc = XtGetGC(w, (XtGCMask) GCForeground | GCBackground, &values);
    values.foreground = shotpixel;
    shotgc = XtGetGC(w, (XtGCMask) GCForeground | GCBackground, &values);
    InitShip();
    InitRocks();
    InitShot();
    InitScore();
}


static void HandleExpose(w, event)
Widget w;
XEvent *event;
{
    if (event->xexpose.count) return;
    if (!shipdestroyed)
	PaintShip(shipgc);
    PaintAllRocks();
    PaintAllShots();
    XSync(dpy, 0);
}


static Boolean wason = TRUE;	/* Whether the user had autorepeat set. */

void Quit()
{
    if (wason) XAutoRepeatOn(dpy);
    XCloseDisplay(dpy);
    printf("Score: %d\n", score);
    exit(0);
}

static void HandleFocus(w, closure, event)
Widget w;
Opaque closure;
XEvent *event;
{
    XKeyboardState info;
    if (event->type == FocusIn) {
	XGetKeyboardControl(dpy, &info);
	wason = info.global_auto_repeat;
	XAutoRepeatOff(dpy);
	XSync(dpy, 0);
	if (shiptimerid == NULL)
	    shiptimerid = XtAddTimeOut(shipwait, MoveShip, (Opaque) MoveShip);
	if (rocktimerid == NULL)
	    rocktimerid = XtAddTimeOut(rockwait, MoveRocks,
				       (Opaque) MoveRocks);
	if (shottimerid == NULL)
	    shottimerid = XtAddTimeOut(shotwait, MoveShots,
				       (Opaque) MoveShots);
    } else if (event->type == FocusOut) {
	if (wason) XAutoRepeatOn(dpy);
	if (shiptimerid) 
	    XtRemoveTimeOut(shiptimerid);
	shiptimerid = NULL;
	if (rocktimerid)
	    XtRemoveTimeOut(rocktimerid);
	rocktimerid = NULL;
	if (shottimerid)
	    XtRemoveTimeOut(shottimerid);
	shottimerid = NULL;
    }
}

static void HandleStruct(w, closure, event)
Widget w;
Opaque closure;
XEvent *event;
{
}

static void Destroy() {}

static void Resize(w) 
Widget w;
{
/* 
 * Resize the playing field.
 */
    gamewidth = w->core.width;
    gameheight = w->core.height;
}

static Boolean SetValues() {}

static Boolean TakeFocus()
{
    XSetInputFocus(dpy, gamewindow, RevertToPointerRoot, CurrentTime);
    return TRUE;
}

RoidsClassRec roidsClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &widgetClassRec,
    /* class_name         */    "Roids",
    /* widget_size        */    sizeof(RoidsRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initiali*/	NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook	  */	NULL,
    /* realize            */    Realize,
    /* actions            */    actions,
    /* num_actions        */    XtNumber(actions),
    /* resources          */    NULL,
    /* num_resources      */    (Cardinal) 0,
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    Resize,
    /* expose             */    HandleExpose,
    /* set_values         */    SetValues,
    /* set_values_hook	  */	NULL,
    /* set_values_almost  */	NULL,
    /* get_values_hook	  */	NULL,
    /* accept_focus       */    TakeFocus,
    /* version		  */	XtVersion,
    /* callback_private	  */	NULL,
    /* tm_table		  */	defaultTranslation,
    /* query_geometry	  */	NULL,
    /* display_accelerator*/	NULL,
    /* extension`	  */	NULL,
  },{
    /* mumble             */    0       /* Make C compiler happy   */
  }
};

WidgetClass roidsWidgetClass = (WidgetClass)&roidsClassRec;
