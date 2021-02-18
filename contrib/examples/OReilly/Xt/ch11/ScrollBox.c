/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 * scrollBox.c - scrollBox composite widget
 * 
 */

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>

#include	"ScrollBoxP.h"

#include	<stdio.h>

#define INITIAL_WIDTH 300
#define INITIAL_HEIGHT 300

/****************************************************************
 *
 * scrollBox Resources
 *
 ****************************************************************/

static XtResource resources[] = {
    {XtNhSpace, XtCHSpace, XtRDimension, sizeof(Dimension),
	 XtOffset(ScrollBoxWidget, scrollBox.h_space), XtRImmediate, (caddr_t)4},
    {XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension),
	 XtOffset(ScrollBoxWidget, scrollBox.v_space), XtRImmediate, (caddr_t)4},
};

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();
static void Resize();
static Boolean SetValues();
static void ChangeManaged();
static XtGeometryResult QueryGeometry();

ScrollBoxClassRec scrollBoxClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "scrollBox",
    /* widget_size        */    sizeof(ScrollBoxRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
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
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	QueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    NULL,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },{
/* scrollBox class fields */
    /* empty		  */	0,
  }
};

WidgetClass scrollBoxWidgetClass = (WidgetClass)&scrollBoxClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

/*
 * Do a layout, either actually assigning positions, or just 
 * calculating size.
 */

/* ARGSUSED */
static DoLayout(sbw)
ScrollBoxWidget sbw;
{
	Widget main, vscroll, hscroll;
	Widget child;
	Dimension mw, mh;   /* main window */
	Dimension vh;   /* vertical scrollbar length (height) */
	Dimension hw;   /* horizontal scrollbar length (width) */
	Position vx;
	Position hy;
	int i;

	if (sbw->composite.num_children != 3)
		fprintf(stderr, "scrollBox: must manage exactly three widgets.\n");

	for (i = 0; i < sbw->composite.num_children; i++) {
		child = sbw->composite.children[i];
		if (!XtIsManaged(child))
			fprintf(stderr, "scrollBox: all three widgets must be managed.\n");
	}

	/*
	 * Child one is the main window, two is the vertical scrollbar,
	 * and three is the horizontal scrollbar.
	 */
	main = sbw->composite.children[0];
	vscroll = sbw->composite.children[1];
	hscroll = sbw->composite.children[2];

	/* 
	 * Size all three widgets so that space is fully utilized.
	 */
	mw = sbw->core.width - (2 * sbw->scrollBox.h_space) - 
		vscroll->core.width - (2 * vscroll->core.border_width) -
		(2 * main->core.border_width);

	mh = sbw->core.height - (2 * sbw->scrollBox.v_space) - 
		hscroll->core.height - (2 * hscroll->core.border_width) -
		(2 * main->core.border_width);

	vx = main->core.x + mw + sbw->scrollBox.h_space + main->core.border_width + vscroll->core.border_width; 

	hy = main->core.y + mh + sbw->scrollBox.v_space + main->core.border_width + hscroll->core.border_width; 

	vh = mh;   /* scrollbars are always same length as main window */
	hw = mw;

	XtResizeWidget(main, mw, mh);

	XtResizeWidget(vscroll, vscroll->core.width, vh);
	XtMoveWidget(vscroll, vx, vscroll->core.y);

	XtResizeWidget(hscroll, hw, hscroll->core.height);
	XtMoveWidget(hscroll, hscroll->core.x, hy);
}

/*
 * Calculate preferred size.  We can't just use the current sizes
 * of the children, because that calculation would always end up with 
 * our current size.  Could query each child, and use that size to 
 * recalculate a size for us, then if it ends up being larger
 * than width and height passed in, accept bounding box.  
 * However, we know our children and they don't have any particular
 * preferred geometry, except the bigger the better.
 * Therefore, if the parent suggested a size, we'll take it.
 */

/* ARGSUSED */
static XtGeometryResult QueryGeometry(widget, request, reply_return)
ScrollBoxWidget widget;
XtWidgetGeometry *request, *reply_return;
{
    XtGeometryResult result;

    request->request_mode &= CWWidth | CWHeight;

    if (request->request_mode == 0)
	/* parent isn't going to change w or h, so nothing to re-compute */
	return XtGeometryYes;

    /* if proposed size is large enough, accept it.  Otherwise,
     * suggest our arbitrary initial size. */

    if (request->request_mode & CWHeight)
	if (request->height < INITIAL_HEIGHT) {
            result = XtGeometryAlmost;
            reply_return->height = INITIAL_HEIGHT;
            reply_return->request_mode &= CWHeight;
        }
        else
            result = XtGeometryYes;

    if (request->request_mode & CWWidth)
	if (request->width < INITIAL_WIDTH) {
            result = XtGeometryAlmost;
            reply_return->width = INITIAL_WIDTH;
            reply_return->request_mode &= CWWidth;
        }

        return(result);
}

/*
 * Actually layout the scrollBox
 */

static void Resize(w)
ScrollBoxWidget w;
{
    DoLayout(w);
}

static void ChangeManaged(w)
ScrollBoxWidget w;
{
    DoLayout(w);
}

/* ARGSUSED */
static void Initialize(request, new)
    Widget request, new;
{
    ScrollBoxWidget newsbw = (ScrollBoxWidget)new;

    if (newsbw->core.width == 0)
        newsbw->core.width = INITIAL_WIDTH;

    if (newsbw->core.height == 0)
	newsbw->core.height = INITIAL_HEIGHT;

} /* Initialize */

/* ARGSUSED */
static Boolean SetValues(current, request, new)
ScrollBoxWidget current, request, new;
{
   /* need to relayout if h_space or v_space change */
    if ((new->scrollBox.h_space != current->scrollBox.h_space) ||
                   (new->scrollBox.v_space != current->scrollBox.v_space))
        DoLayout(new);

    return False;
}
