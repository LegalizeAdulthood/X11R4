#ifndef lint
static char rcsid[] = "$Header: /ai/nil/var/scr/moraes/xtroff/XtStuff/RCS/Menu.c,v 1.1 89/11/15 21:37:33 moraes Exp $";
#endif lint

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.  
 * 
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * Menu.c -
 *   
 * Author:       Robert P. Shen
 *               Malcolm Forbes
 *               Digital Equipment Corporation
 *               ESG Systems Engineering
 *
 * Date:         February 1, 1988
 */

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	"x11.h"
#ifdef R4
# include	<X11/Xmu/Misc.h>
# include	<X11/Xmu/Xmu.h>
# include 	<X11/Xaw/Label.h>
#else
# include	<X11/XawMisc.h>
# include	<X11/Xmu.h>
# include 	<X11/Label.h>
#endif
#include	"MenuP.h"
#include	"MenuItem.h"
#include	"CheckItem.h"
#include	<ctype.h>

/****************************************************************
 *
 * Menu Resources
 *
 ****************************************************************/

static XtOrientation init_orient = XtorientVertical;
static XtResource resources[] = {
    {XtNxMargin, XtCHSpace, XtRInt, sizeof(int),
	 XtOffset(MenuWidget, menu.x_margin), XtRString, "0"},
    {XtNyMargin, XtCVSpace, XtRInt, sizeof(int),
	 XtOffset(MenuWidget, menu.y_margin), XtRString, "0"},
    {XtNx, XtCPosition, XtRPosition, sizeof(Position),
         XtOffset(MenuWidget,core.x),XtRString, "800"},
    {XtNy, XtCPosition, XtRPosition, sizeof(Position),
         XtOffset(MenuWidget,core.y),XtRString, "800"},
    {XtNborder,"MyBorder",XtRPixel,sizeof(Pixel),
         XtOffset(MenuWidget,core.border_pixel),XtRString, "Black"},
/*
    {XtNborder,XtCBorderColor,XtRPixel,sizeof(Pixel),
         XtOffset(MenuWidget,core.border_pixel),XtRString, "White"},
*/
    {XtNmenuForeground, XtCForeground,XtRPixel, sizeof(Pixel),
         XtOffset(MenuWidget,menu.menu_foreground_pixel),XtRString, "Black"},
    {XtNcheckForeground, XtCForeground,XtRPixel, sizeof(Pixel),
         XtOffset(MenuWidget,menu.check_foreground_pixel),XtRString, "Black"},
    {XtNbackground, XtCBackground,XtRPixel, sizeof(Pixel),
         XtOffset(MenuWidget,core.background_pixel),XtRString, "White"},
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
         XtOffset(MenuWidget,core.border_width),XtRString, "2"},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
         XtOffset(MenuWidget,menu.orient),XtRString, "vertical"},
    {XtNmenuItemsList, "MenuItemsList", XtRPointer, sizeof(MenuItems),
         XtOffset(MenuWidget,menu.items),XtRPointer, NULL},
};
/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();
static void Realize();
static void Resize();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static void ChangeManaged();
static void ClassInitialize();

MenuClassRec menuClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "Menu",
    /* widget_size        */    sizeof(MenuRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */				NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook 	  */		NULL,
    /* realize            */    Realize,
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
    /* set_values_almost  */	NULL,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */	NULL,
    /* query_geometry     */    NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	NULL,	/* Inherit from superclass */
    /* delete_child	  */	NULL,	/* Inherit from superclass */
  },{
    /* mumble		  */	0	/* Make C compiler happy   */
  }
};

WidgetClass menuWidgetClass = (WidgetClass)&menuClassRec;
static Dimension no_resize_width, no_resize_height;




static void CvtStringToOrientation();

static XrmQuark	XrmQEvertical;
static XrmQuark	XrmQEhorizontal;
/* ARGSUSED */
static void CvtStringToOrientation(args, pargs, fromVal, toVal)
    XrmValuePtr	args;
    Cardinal	pargs;
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XtOrientation	e;
    XrmQuark    q;
    char	*s = (char *) fromVal->addr;
    char        lowerName[1000];
    int		i;

    if (s == NULL) return;

    for (i=0; i<=strlen(s); i++) {
        char c = s[i];
	lowerName[i] = isupper(c) ? (char) tolower(c) : c;
    }

    q = XrmStringToQuark(lowerName);

    toVal->size = sizeof(XtOrientation);
    toVal->addr = (caddr_t) &e;

    if (q == XrmQEvertical)   { e = XtorientVertical;   return; }
    if (q == XrmQEhorizontal) { e = XtorientHorizontal; return; }

    toVal->size = 0;
    toVal->addr = NULL;
};

/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

static void ClassInitialize()
{
    CompositeWidgetClass superclass;
    MenuWidgetClass myclass;

    myclass = (MenuWidgetClass) menuWidgetClass;
    superclass = (CompositeWidgetClass) myclass->core_class.superclass;

    /* Inherit insert_child and delete_child from Composite */
    myclass->composite_class.insert_child =
        superclass->composite_class.insert_child;
    myclass->composite_class.delete_child =
        superclass->composite_class.delete_child;

    XrmQEvertical   = XrmStringToQuark("vertical");
    XrmQEhorizontal = XrmStringToQuark("horizontal");

    XtAddConverter(XtRString, XtROrientation, CvtStringToOrientation,
		   NULL, (Cardinal)0 );
}

/*
 *
 * Do a layout, either actually assigning positions, or just calculating size.
 * Returns TRUE on success; FALSE if it couldn't make things fit.
 *
 */

static void horizontal_layout_plan(menuw, width, height, position)
     MenuWidget menuw;
     Dimension *width, *height;
{
  Widget kid;
  Dimension w, h;
  Dimension child_w, child_h, x, y, x_margin, y_margin;
  int i;

    /* Menu width and height */
    w = x = child_w = x_margin = menuw->menu.x_margin;
    h = y = child_h = y_margin = menuw->menu.y_margin;

    for (i = 0; i < menuw->composite.num_children; i++) {
	kid = menuw->composite.children[i];
	if (kid->core.managed) {
	    /* Compute menu width */
	    child_w = kid->core.width + 2*kid->core.border_width + x_margin;
	    AssignMax(child_h, kid->core.height);
	    w += child_w;
	    h = child_h + 2*kid->core.border_width+2* y_margin;

	    if (position) {

	      XtMoveWidget(kid, (int)x, (int)y);
              XtResizeWidget(kid,kid->core.width,child_h,kid->core.border_width);

            }
	    x = w;
	} /* if managed */
    } /* for */

  *width = w;
  *height = h;

}

static void redeploy_kids(menuw)
     MenuWidget menuw;
{
  Widget kid;
  int i;
  Dimension menu_width, kid_width;
  Dimension x, y;
  Dimension x_margin, y_margin;

  menu_width = menuw->core.width;
  x = x_margin = menuw->menu.x_margin;
  y = y_margin = menuw->menu.y_margin;

  for (i = 0; i <menuw->composite.num_children; i++) {
    kid = menuw->composite.children[i];
    if (kid->core.managed) {
      kid_width = menu_width - 2*kid->core.border_width - 2*x_margin;
/*
      XtResizeWidget(kid,kid_width,kid->core.height,kid->core.border_width);
*/
      XtMoveWidget(kid, (int)x, (int)y);
      y += kid->core.height + 2*kid->core.border_width + y_margin;
    }
  }
}


/* ARGSUSED */
static LayoutPlan(menuw, width, height, replyWidth, replyHeight, position)
    MenuWidget	menuw;
    Dimension		width, height;
    Dimension		*replyWidth, *replyHeight;	/* RETURN */
    Boolean		position;	/* actually reposition the windows? */
{
    Cardinal  i;
    Dimension w, h;	/* Width and height needed for menu panel 	*/
    Dimension x, y;
    Dimension child_w, child_h;
    Dimension x_margin, y_margin;  /* Local copy of menuw->menu.x_margin 	*/
    Dimension hlayout_width, hlayout_height;
    Widget    kid;	/* Current menu 				*/

    /* Menu width and height */
    w = x = x_margin = menuw->menu.x_margin;
    h = y = y_margin = menuw->menu.y_margin;

    for (i = 0; i < menuw->composite.num_children; i++) {
	kid = menuw->composite.children[i];
	if (kid->core.managed) {
	    /* Compute menu width */
	    child_w = kid->core.width + 2*kid->core.border_width + 2*x_margin;
	    child_h = kid->core.height + 2*kid->core.border_width + y_margin;
	    AssignMax(w, child_w);
	    h += child_h + y_margin;

	    if (position) {

	      XtMoveWidget(kid, (int)x, (int)y);
              XtResizeWidget(kid,(width-2*kid->core.border_width-2*x_margin),
		     kid->core.height,kid->core.border_width);

            }
	    y = h;
	} /* if managed */
    } /* for */

    h += y_margin;

    *replyWidth = Max(w, 1);
    *replyHeight = Max(h, 1);
}

/*
 *
 * Calculate preferred size, given constraining box
 *
 */

static Boolean PreferredSize(menuw, width, height, replyWidth, replyHeight)
    MenuWidget	menuw;
    Dimension		width, height;
    Dimension		*replyWidth, *replyHeight;
{
    LayoutPlan(menuw, width, height, replyWidth, replyHeight, FALSE);
    return ((*replyWidth <= width) && (*replyHeight <= height));
}

/*
 *
 * Actually layout the menu panel
 *
 */

static void Resize(menuw)
    MenuWidget	menuw;
{
    Widget kid;
    Dimension junk, width, height;

    if (menuw->menu.orient == XtorientVertical) {
      LayoutPlan((MenuWidget)menuw, menuw->core.width, menuw->core.height,
	     &junk, &junk, TRUE);
    }
    else if (menuw->menu.orient == XtorientHorizontal) {
      	horizontal_layout_plan (menuw, &width, &height, TRUE);
      }
/*
    redeploy_kids(w);
*/
} /* Resize */

/*
 *
 * Try to do a new layout within the current width and height;
 * if that fails try to do it within the box returned by PreferredSize.
 *
 * TryNewLayout just says if it's possible, and doesn't actually move the kids
 */

static Boolean TryNewLayout(menuw)
    MenuWidget	menuw;
{
    Dimension		width, height, junk_w, junk_h;

    if (!PreferredSize(menuw, menuw->core.width, menuw->core.height, &width, &height))
	(void) PreferredSize(menuw, width, height, &width, &height);

    if ((menuw->core.width == width) && (menuw->core.height == height)) {
        /* Same size */
	return (TRUE);
    }

    /* let's see if our parent will go for a new size. */
    switch (XtMakeResizeRequest((Widget) menuw, width, height, &width, &height)) {

	case XtGeometryYes:
	    return (TRUE);

	case XtGeometryNo:
	    if ((width <= menuw->core.width) && (height <= menuw->core.height))
	        return (TRUE);
	    else
	        return (FALSE);

	case XtGeometryAlmost:
	    if (! PreferredSize(menuw, width, height, &junk_w, &junk_h))
	        return (FALSE);
	    (void) XtMakeResizeRequest((Widget) menuw, width, height, 
					&width, &height);
	    return (TRUE);
    }
}

/*
 *
 * Geometry Manager
 *
 */

/*ARGSUSED*/
static XtGeometryResult GeometryManager(w, request, reply)
    Widget		w;
    XtWidgetGeometry	*request;
    XtWidgetGeometry	*reply;	/* RETURN */

{
    Dimension	width, height, borderWidth;
    MenuWidget menuw;

    /* Position request always denied */
    if (request->request_mode & (CWX | CWY))
        return (XtGeometryNo);

    /* Size changes must see if the new size can be accomodated */
    if (request->request_mode & (CWWidth | CWHeight | CWBorderWidth)) {

	/* Make all three fields in the request valid */
	if ((request->request_mode & CWWidth) == 0)
	    request->width = w->core.width;
	if ((request->request_mode & CWHeight) == 0)
	    request->height = w->core.height;
        if ((request->request_mode & CWBorderWidth) == 0)
	    request->border_width = w->core.border_width;

	/* Save current size and set to new size */
	width = w->core.width;
	height = w->core.height;
	borderWidth = w->core.border_width;
	w->core.width = request->width;
	w->core.height = request->height;
	w->core.border_width = request->border_width;

	/* Decide if new layout works: (1) new menu is smaller,
	   (2) new menu fits in existing Menu, (3) Menu can be
	   expanded to allow new menu to fit */

	menuw = (MenuWidget) w->core.parent;

/* whenever a child changes his geometry, we attempt to
 * change ours to be the minimum enclosing size...
	if (((request->width + request->border_width <= width + borderWidth) &&
	    (request->height + request->border_width <= height + borderWidth))
	|| PreferredSize(menuw, menuw->core.width, menuw->core.height, &junk, &junk)
	|| TryNewLayout(menuw)) {
 */
	if (TryNewLayout(menuw)) {
	    /* Fits in existing or new space, relayout */
	    Resize((Widget)menuw);
	    return (XtGeometryYes);
	} else {
	    /* Cannot satisfy request, change back to original geometry */
	    w->core.width = width;
	    w->core.height = height;
	    w->core.border_width = borderWidth;
	    return (XtGeometryNo);
	}
    }; /* if any size changes requested */

    /* Any stacking changes don't make a difference, so allow if that's all */
    return (XtGeometryYes);
}

static void ChangeManaged(w)
    MenuWidget w;
{
    Dimension hlayout_width, h_layout_height, kid_height;
    Dimension x_margin, y_margin;
    Dimension width, height;

    if (w->menu.orient == XtorientVertical)
      {

	(void) TryNewLayout((MenuWidget)w);

	Resize(w);
/*
    redeploy_kids(w);
*/
      }
    else if (w->menu.orient == XtorientHorizontal)
      {
	horizontal_layout_plan (w, &width, &height, FALSE);
	switch (XtMakeResizeRequest((Widget)w, width, height, &width, &height))
	  {
	    case XtGeometryYes: break;
            case XtGeometryNo: break;
            case XtGeometryAlmost: break;
	  }
	horizontal_layout_plan (w, &width, &height, TRUE);
      }
}

/* ARGSUSED */
static void Initialize(request, new)
	Widget	request, new;
    {
	MenuWidget	newmenuw = (MenuWidget)new;
	Widget	cmd;
	MenuItems	items;
	int		i, j;
/*	char		str[32];		*/
/*	static char	check_pad[] = "    ";	*/
/*	static char	space_pad[] = "    ";	*/

	static Arg labelArgs[] = {
	        {XtNbackground,  (XtArgVal) NULL},
		{XtNforeground,  (XtArgVal) NULL}
	    };
	static Arg menuArgs[] = {
	        {XtNbackground,  (XtArgVal) NULL},
		{XtNforeground,  (XtArgVal) NULL},
	        {XtNborderWidth, (XtArgVal) 0},
		{XtNcallback,    (XtArgVal) NULL}
	    };
	static Arg checkArgs[] = {
	        {XtNbackground,  (XtArgVal) NULL},
		{XtNforeground,  (XtArgVal) NULL},
	        {XtNborderWidth, (XtArgVal) 0},
		{XtNcheckState, (XtArgVal)  TRUE},
		{XtNcallback,    (XtArgVal) NULL}
	    };
	static Arg uncheckArgs[] = {
	        {XtNbackground,  (XtArgVal) NULL},
		{XtNforeground,  (XtArgVal) NULL},
	        {XtNborderWidth, (XtArgVal) 0},
		{XtNcheckState, (XtArgVal) FALSE},
		{XtNcallback,    (XtArgVal) NULL}
	    };


/* ||| What are consequences of letting height, width be 0? If okay, then
       Initialize can be NULL */

    if (newmenuw->core.width == 0)
        newmenuw->core.width = ((newmenuw->menu.x_margin != 0)
			      ? newmenuw->menu.x_margin : 1);	/* MOD */
    if (newmenuw->core.height == 0)
	newmenuw->core.height = ((newmenuw->menu.y_margin != 0)
			       ? newmenuw->menu.y_margin : 1);	/* MOD */

    XtSetArg(labelArgs[0], XtNbackground,newmenuw->menu.menu_foreground_pixel);
    XtSetArg(labelArgs[1], XtNforeground,newmenuw->core.background_pixel);
    XtSetArg(menuArgs[0], XtNbackground,newmenuw->core.background_pixel);
    XtSetArg(menuArgs[1],XtNforeground,newmenuw->menu.menu_foreground_pixel);
    XtSetArg(checkArgs[0], XtNbackground, newmenuw->core.background_pixel);
    XtSetArg(checkArgs[1],XtNforeground,newmenuw->menu.check_foreground_pixel);
    XtSetArg(uncheckArgs[0], XtNbackground, newmenuw->core.background_pixel);
    XtSetArg(uncheckArgs[1],XtNforeground,newmenuw->menu.check_foreground_pixel);
    cmd = XtCreateManagedWidget (newmenuw->core.name,
       				labelWidgetClass, new,
				labelArgs, XtNumber(labelArgs));

    if( newmenuw->menu.items )
    {
	    XtCallbackList  temprec;
	    
	    items = newmenuw->menu.items;
	    for (; items->proc; items++) {     /* at least proc must exist! */
		temprec = (XtCallbackList)XtMalloc(sizeof(XtCallbackRec)*2);
		temprec->callback = items->proc;
		temprec->closure = items->closure;
		(temprec+1)->callback = NULL;
		(temprec+1)->closure = NULL;
		if (items->flags & (XtitemCheckable | XtitemIsChecked)) {
		    XtSetArg(checkArgs[4], XtNcallback, temprec);
		    XtSetArg(uncheckArgs[4], XtNcallback, temprec);
		    cmd = XtCreateManagedWidget (items->text,
					checkitemWidgetClass, new,
					(items->flags & XtitemIsChecked ?
						(ArgList) checkArgs :
						(ArgList) uncheckArgs),
					(items->flags & XtitemIsChecked ?
					 XtNumber(checkArgs) :
					 XtNumber(uncheckArgs)));

	    }
		else {
		    XtSetArg(menuArgs[3], XtNcallback, temprec);
		    cmd = XtCreateManagedWidget (items->text,
					menuitemWidgetClass, new,
						 (ArgList) menuArgs,
						 XtNumber(menuArgs));
		}
		XtSetSensitive (cmd, !(items->flags & XtitemUnselectable));
	}
    }
} /* Initialize */

/* ||| Should Realize just return a modified mask and attributes?  Or will some
   of the other parameters change from class to class? */
static void Realize(w, valueMask, attributes)

    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    MenuWidget newmenuw = (MenuWidget)w;

    attributes->bit_gravity = NorthWestGravity;
    *valueMask |= CWBitGravity;

    XtCreateWindow( w, (unsigned)InputOutput, (Visual *)CopyFromParent,
		    *valueMask, attributes);
} /* Realize */

/*
 *
 * Set Values
 *
 */

/* ARGSUSED */
static Boolean SetValues (current, request, new, last)
    Widget current, request, new;
    Boolean last;
{
    /* ||| Old code completely bogus, need background, etc. then relayout */

    /*XtSetValuesGeometryRequest( current, new, (XtWidgetGeometry *)NULL );*/

    /* ||| should handle XtGeometryAlmost */

    return (FALSE);
}
