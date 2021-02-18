/***********************************************************************
 *
 * MenuBox.c - MenuBox composite widget
 * Robert Blumen blumen@arisia.xerox.com blumen@cad.berkeley.edu
 *
 * MenuBox.c - code for MenuBox widget
 *
 * Converted to X11 Release 3 by
 *	Win Treese
 *	Digital Equipment Corp.
 *	treese@crl.dec.com
 ***********************************************************************/

/***********************************************************
Copyright 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/XawMisc.h>
#include	<X11/MenuBoxP.h>

static void ChangeManaged();

static Dimension defVSpace = 1;

static XtResource resources[] = {
	{ XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension), 
		  XtOffset(MenuBoxWidget, box.v_space), XtRDimension,
	          (caddr_t) &defVSpace },
};

							      
MenuBoxClassRec menuboxClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &boxClassRec,
    /* class_name         */    "MenuBox",
    /* widget_size        */    sizeof(MenuBoxRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    NULL,
    /* initialize_hook    */	NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    XtInheritResize,
    /* expose             */    NULL,
    /* set_values         */    NULL,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension	  */	NULL,
  },{
/* composite_class fields */
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
  },{
/* Box class fields */
    /* empty		  */	0,
  },{
/* MenuBox class fields */
   /* empty               */    0,
  },
};

WidgetClass menuboxWidgetClass = (WidgetClass)&menuboxClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

/*----------------------------------------------------------------------

This procedure makes sure that all the boxes are the same width;
Could check first that children are subclass of labelWidget, but
don't bother.
*/

/* ARGSUSED */
static void
ChangeManaged(w)
	Widget w;
{
	int j;
	Dimension width = 0;
	MenuBoxWidget mbw = (MenuBoxWidget) w;
	WidgetClass wc;
	XtWidgetProc wpr;
	Widget  child;
	XtWidgetGeometry intended, preferred;

/* get max width of current children */
	width = 0;
	intended.request_mode = 0;

	for (j = 0; j < mbw->composite.num_children; j++) {
		child = mbw->composite.children[j];
		if (child->core.managed) {
			XtQueryGeometry(child, &intended, &preferred);
			AssignMax(width, preferred.width);
		}
	}

/* make any one smaller than max width larger */
	for (j = 0; j < mbw->composite.num_children; j++) {
		child = mbw->composite.children[j];
		if (child->core.width != width)
			XtResizeWidget(child, width, child->core.height,
				       child->core.border_width);
	}


/* call superclass changed managed to do layout */
	wc = XtSuperclass(w);
	wpr = ((CompositeWidgetClass) wc)->composite_class.change_managed;
	(*wpr)(w);

	return;
}
