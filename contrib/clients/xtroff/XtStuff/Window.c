#ifndef lint
static char rcsid[] = "$Header: Window.c,v 1.2 89/04/21 03:30:12 xwindows Exp $";
#endif lint

/*
 * Window.c - Window widget
 *
 * Author:      Chris Peterson
 *              M.I.T.  
 *              Bldg. E40 - Project Athena
 *              77 Massachussets Ave.
 *              Cambridge, MA 02139
 * 
 * Date:        Tue Oct 27 1987
 *
 * Based upon the label widget by Charles Haynes.
 */

/* 
 *  This is the window widget, its purpose is to allow windows to be
 *  used with the toolkit.  I has a foreground color, as core does not
 *  and it is best if this resource is owned by the widget.
 *  It will also add a pixmap to the background of your window.
 *
 *                                Chris Peterson - 2/9/88
 */

#include <X11/IntrinsicP.h>
#include "Window.h"
#include "WindowP.h"
#include <X11/StringDefs.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static XtResource resources[] = {
    {XtNpixmap, XtCpixmap, XtRString, sizeof(caddr_t),
       XtOffset(WindowWidget, window.pixmap), XtRString, (caddr_t) NULL},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
       XtOffset(WindowWidget, window.foreground), XtRString, "Black"},
};

static void Realize();

WindowClassRec windowClassRec = {
  {
/* core_class fields */	
    /* superclass         */    (WidgetClass) &widgetClassRec,
    /* class_name         */    "Window",
    /* widget_size        */    sizeof(WindowRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    NULL,
    /* initialize_hook    */    NULL,
    /* realize            */    Realize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    NULL,
    /* expose             */    NULL,
    /* set_values         */    NULL,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */    NULL,
#ifdef XtCAccelerators
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension		  */	NULL
#endif /* XtCAccelerators */
  }
};

WidgetClass windowWidgetClass = (WidgetClass)&windowClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void Realize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
  WindowWidget lw = (WindowWidget)w;
  if (lw->window.pixmap != NULL) {
    *valueMask |= CWBackPixmap;
    *valueMask &= ~CWBackPixel;
    attributes->background_pixmap = lw->window.pixmap;
  }

  XtCreateWindow(w, (Cardinal) InputOutput, (Visual *)CopyFromParent,
		 *valueMask, attributes);
} /* Realize */
