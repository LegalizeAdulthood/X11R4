/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ListRC.c
 **
 **   Project:     X Widgets
 **
 **   Description: Code and class record for ListRowCol widget
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/




#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/ScrollBar.h>
#include <Xw/Arrow.h>
#include <Xw/Valuator.h>
#include <Xw/BBoard.h>
#include <Xw/SWindow.h>
#include <Xw/SWindowP.h>

#include <Xw/ListRCP.h>
#include <Xw/ListRC.h>
#include <Xw/ListP.h>
#include <Xw/List.h>

static void Initialize();
static void Resize();
static void Realize();
static void InsertChild();
static XtSetValuesFunc SetValues();
static Boolean TryNewLayout();
static XtGeometryResult GeometryManager();
static void ChangeManaged();



/*************************************<->*************************************
 *
 *
 *   Description:  default translation table for class: RowColumn Manager
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/

static char defaultTranslations[] = 
    "<EnterWindow>:          enter() \n\
     <LeaveWindow>:          leave() \n\
     <FocusIn>:              focusIn()";



/*************************************<->*************************************
 *
 *
 *   Description:  action list for class: CheckBox
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *
 *************************************<->***********************************/

static XtActionsRec actionsList[] =
{
  {"enter", (XtActionProc) _XwManagerEnter},
  {"leave", (XtActionProc) _XwManagerLeave},
  {"focusIn", (XtActionProc) _XwManagerFocusIn},  
};

/****************************************************************
 *
 * RowCol Resources
 *
 ****************************************************************/

static XtResource resources[] = {
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
         XtOffset(XwListRowColWidget,core.width), XtRString, "10"},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
         XtOffset(XwListRowColWidget,core.height), XtRString, "10"},
    {XtNhSpace, XtCHSpace, XtRDimension, sizeof(Dimension),
	 XtOffset(XwListRowColWidget, row_col.h_space), XtRString, "4"},
    {XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension),
	 XtOffset(XwListRowColWidget, row_col.v_space), XtRString, "4"},
    {XtNcolumns, XtCColumns, XtRInt, sizeof(int),
	 XtOffset(XwListRowColWidget, row_col.columns), XtRString, "1"},
    {XtNmode, XtCMode, XtRRCMode, sizeof(int),
         XtOffset(XwListRowColWidget, row_col.mode), XtRString, "n_of_many"},
    
};

/****************
 *
 *  Constraint Resources
 *
 ****************/
static XtResource ConstraintResources[] = 
{

   {
      XtNrowPosition, XtCRowPosition, XtRInt, sizeof (int),
      XtOffset (XwListConstraints, row),
      XtRString, "-1"
   },
   {
      XtNcolumnPosition, XtCColumnPosition, XtRInt, sizeof (int),
      XtOffset (XwListConstraints, column),
      XtRString, "-1"
   },

};

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

XwListRowColClassRec XwlistrowColClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &XwmanagerClassRec,
    /* class_name         */    "ListRC",
    /* widget_size        */    sizeof(XwListRowColRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,				
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    Realize,
    /* actions		  */	actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterlv   */    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    (XtSetValuesFunc) SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus       */    NULL,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      defaultTranslations,
    /* query_geometry   */	NULL, 
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	InsertChild,	      /* Basically Inherited */
    /* delete_child	  */	XtInheritDeleteChild,	/*  from Composite */
				NULL,
  },{
/* Constraint class Init */
    /* Constraint res.   */	ConstraintResources,
    /* Num. of resources */	XtNumber(ConstraintResources),
    /* Size of data      */	sizeof(XwListConstraintRec),
    /* INitialize	 */	NULL,
    /* Destroy		 */	NULL,
    /* Set Values	 */     NULL,
				NULL,
  },{
/* manager_class fields */
   /* traversal handler   */    XtInheritTraversalProc,
   /* translations        */	NULL,
  },{
/* row column class - none */     
     /* mumble */               0
 }	
};

WidgetClass XwlistrowColWidgetClass = (WidgetClass)&XwlistrowColClassRec;


/*************************************<->*************************************
 *
 *  HandleSet
 *
 *   Description:
 *   -----------
 *     This procedure simply returns unless the "mode" is XwONE_OF_MANY.
 *     If the mode is 1_of_many, then the child, w, becomes the selected
 *     child. (Recall that this manager attaches a callback to each
 *     child which is a subclass of XwPrimitive). Execute release
 *     procedure of any currently selected child.
 *
 *   Inputs:
 *   ------
 *     w = widget which was just selected.
 *     closure and call_data are NULL.
 * 
 *   Outputs:
 *   -------
 *     The previously selected child, if it is not the same as w,
 *     has its release procedure directly invoked.
 *
 *************************************<->***********************************/

static void HandleSet(w, closure, call_data)
    Widget      w;
    caddr_t     closure;
    caddr_t     call_data;
{
    XwListRowColWidget mw;
    Widget child;
    XwPrimitiveWidget oldChild;
    int i;

    mw = (XwListRowColWidget) w->core.parent;

    if (mw->row_col.mode != XwONE_OF_MANY) return;

    oldChild = (XwPrimitiveWidget)mw->row_col.selectedChild;
    mw->row_col.selectedChild = w;

    if ((oldChild != NULL) && ((Widget)oldChild != w))
      {
        if(XtIsSubclass((Widget)oldChild, XwbuttonWidgetClass) && 
           !((XwButtonWidget)(oldChild))->button.set) return;
   
	 if(XtIsSubclass((Widget)oldChild, XwprimitiveWidgetClass))
            (*(((XwPrimitiveWidgetClass)
	      (oldChild -> core.widget_class)) ->
	        primitive_class.release_proc)) (oldChild, (XEvent *)NULL);
      }
}


/*************************************<->*************************************
 *
 *  InsertChild
 *
 *   Description:
 *   -----------
 *    Use the standard Composite Insert Child procedure to get it into
 *    the children list, then if its a subclass of primitive attach a
 *    callback (on selection) to allow us to implement the 1_of_many mode.
 *    Further, if its a button (has a set field) and the mode is 1_of_many
 *    then make sure no other button is set and make this the currently
 *    selected child.
 *
 *   Inputs:
 *   ------
 *     w = child to be inserted.
 *     p_num_args and args are not used.
 *
 *************************************<->***********************************/

static Arg rcArgs[2];

static void InsertChild(w)
    Widget	w;
{
    int _XW_NARGS = 0;
    ArgList	args = NULL;
    Cardinal  *p_num_args = (Cardinal *) &_XW_NARGS;

    Cardinal	    position;
    Cardinal	    i;
    XwListRowColWidget cw;
    XwManagerWidgetClass superclass;

    cw = (XwListRowColWidget) w->core.parent;
    superclass = (XwManagerWidgetClass)XwmanagerWidgetClass;
    (*superclass->composite_class.insert_child)(w, args, p_num_args);

  /***************************************************************** 
   * IF THIS CHILD IS A SUBCLASS OF PRIMITVE (I.E., ITS ONE OF OURS)
   * THEN ATTACH A CALLBACK SO THAT WE GET CALLED WHEN ITS POKED.
   * THIS ALLOWS US TO HANDLE THE 1_OF_MANY MODE.
   ******************************************************************/
    if (XtIsSubclass (w, XwprimitiveWidgetClass))
      {
       XtAddCallback(w, XtNselect, (XtCallbackProc)HandleSet, NULL);    

       /************************************************************
        * FURTHER, IF THIS CHILD IS A SUBCLASS OF BUTTON, AND THE
        * MODE OF THIS ROWCOL MGR IS 1_OF_MANY, AND THIS BUTTON IS
        * SET, THEN UNSELECT ANY BUTTON THAT MAY CURRENTLY BE SET AND
        * MAKE THIS BUTTON THE CURRENTLY SELECTED CHILD.
        **************************************************************/
       if ((XtIsSubclass (w, XwbuttonWidgetClass)) &&  
           (cw->row_col.mode == XwONE_OF_MANY) &&     
               (((XwButtonWidget)(w))->button.set))  
         {
           if (cw->row_col.selectedChild != NULL)
             {
	       XtSetArg(rcArgs[0], XtNset, FALSE);
	       XtSetValues((Widget)(cw->row_col.selectedChild),
			      (ArgList)rcArgs, 1);
	     }
           cw->row_col.selectedChild = w;
         }
      }
}


/*************************************<->*************************************
 *
 *  DoLayout
 *
 *   Description:
 *   -----------
 * Do a layout, either actually assigning positions, or just calculating size.
 * Returns TRUE on success; FALSE if it couldn't make things fit.
 *
 *************************************<->***********************************/

static Boolean DoLayout(mw, width, height, replyWidth, replyHeight, position)
    XwListRowColWidget	mw;
    Dimension		width, height;
    Dimension		*replyWidth, *replyHeight;	/* RETURN */
    Boolean		position;	/* actually reposition the windows? */
{
    Cardinal  i, j, numItems, numCols, numRows, xtra, odd;
    Cardinal xPos, yPos, xPad, yPad;
    Dimension colWidth, minWidth, rowHeight, minHeight, itemHeight;
    Widget box;   /* Current button 				*/

    xtra = odd = 0;
    numCols = mw->row_col.columns;
    
    /* Manager width and height */
    xPad = mw->row_col.h_space;
    yPad = mw->row_col.v_space;

    
      
  /*
   * If there are no items then any space will work 
   */

   if ((numItems=mw->manager.num_managed_children) <= 0)
      {
        if ((replyWidth != NULL) && (replyHeight != NULL))
	{
	   *replyWidth = 0;
	   *replyHeight = 0;
	}
	return(TRUE);
     }



   
  /*
   * First, compute the minimum width (less padding) needed for
   * these items.
   */

   minWidth = minHeight = 0;

   for (i=0; i < numCols; i++)
      {
	 colWidth = 0;
	 for (j=i; j < numItems; j+= numCols)
	  {
	    box = (Widget) mw->manager.managed_children[j];
	    colWidth =  Max (colWidth, (box->core.width +
			      2*box->core.border_width));
	 }
	 minWidth += colWidth;
      }



   /*
    * Next compute the minimum height (less padding) needed for these
    * items.
    */

   for (i=0; i < numItems; i+= numCols)
      {
	 rowHeight=0;
	 for (j=i; (j < i+numCols) && (j < numItems); j++)
	    {
	       box = (Widget) mw->manager.managed_children[j];
 	       itemHeight = box->core.height + 2*box->core.border_width;
	       rowHeight = Max (rowHeight, itemHeight);
	    }
	 minHeight += rowHeight;
      }


   /*
    * Now finish computing the necessary width and height to contain
    * these items.
    */

   minWidth += (numCols + 1) * xPad;
   numRows = (numItems + numCols - 1) / numCols;
   minHeight +=  yPad * (numRows + 1);



  /*
   * If we are really going to position these buttons then now is
   * the time to do it.
   */

  
   if (position)
    {

       if ((mw->manager.layout == XwMAXIMIZE) && (minWidth < width))
	  {
            xtra = (width-minWidth)/(numCols+1);
	    odd = (xtra*(numCols+1) == (width-minWidth)) ? 0 : 1;
	    xPad += xtra;
	  }
       
      /*
       * Now we need to go through and set the X value for each item
       * This requires computing the column widths again.
       */

       xPos = xPad + odd;
   
       for (i=0; i < numCols; i++)
          {
	     colWidth = 0;
    	     for (j=i; j < numItems; j += numCols)
	     {
	        box = (Widget) mw->manager.managed_children[j];
	        colWidth =  Max (colWidth, (box->core.width +
			        2*box->core.border_width));
	        box->core.x = xPos;
	     }
  	     xPos += colWidth + xPad;
           }
   


       /*
        * Now we need to go through and set the Y value for each item
        * This requires computing the row heights again.
        * Then, in the same loop, call the routine that adjusts the x,y
        * values on the button window.
        */

       odd =0;
       if ((mw->manager.layout == XwMAXIMIZE) && (minHeight < height))
	  {
            xtra = (height-minHeight)/(numRows+1);
	    odd = (xtra*(numRows+1) == (height-minHeight)) ? 0 : 1;
	    yPad += xtra;
	  }

        yPos = yPad + odd;
          
        for (i=0; i < numItems; i += numCols)
           {
	     rowHeight=0;
   	     for (j=i; (j < (i+numCols)) && (j < numItems); j++)
	      {
	        box = (Widget) mw->manager.managed_children[j];
                rowHeight = Max(rowHeight, box->core.height +
				  2*box->core.border_width);
	      }

	     for (j=i; (j < (i+numCols)) && (j < numItems); j++)
	        {
	          box = (Widget) mw->manager.managed_children[j];
		  box->core.y = yPos + (rowHeight -
				    (box->core.height +
				     2*box->core.border_width))/2;
		  
                   if (XtIsRealized((Widget)box))
                           XMoveWindow(XtDisplay((Widget)box),
   			               XtWindow((Widget)box),
			               box->core.x, box->core.y);

	        }
	 
	      yPos += rowHeight + yPad;
            }
    }

   if (replyWidth != NULL)  *replyWidth = minWidth;
   if (replyHeight != NULL) *replyHeight = minHeight;


   if ((width < minWidth) || (height < minHeight)) return (FALSE);

   return (TRUE);
}

/*************************************<->*************************************
 *
 *  Realize
 *
 *************************************<->***********************************/
static void Realize(w, p_valueMask, attributes)
    register XwListRowColWidget w;
    Mask *p_valueMask;
    XSetWindowAttributes *attributes;
{
    static Dimension junk;
    Mask valueMask = *p_valueMask;

    valueMask |= CWBitGravity;

    (void) TryNewLayout(w);
    DoLayout(w, w->core.width, w->core.height, &junk, &junk, TRUE);

    if (w->core.width == 0) w->core.width = w->core.parent->core.width;
    if (w->core.height == 0) w->core.height = w->core.parent->core.height;

/*	    XCreateWindow (
		XtDisplay (w),
		(w->core.parent ?
		    w->core.parent->core.window :
		    w->core.screen->root),
		w->core.x, w->core.y,
		w->core.width, w->core.height,
		w->core.border_width, (int) w->core.depth,
		InputOutput, (Visual *)CopyFromParent, valueMask, attributes);
*/
    XtCreateWindow(w, InputOutput, (Visual *)CopyFromParent,
	valueMask, attributes);
}

/*************************************<->*************************************
 *
 *  Resize 
 *
 *   Description:
 *   -----------
 *    Actually layout the button box according to our current size.
 *
 *
 *************************************<->***********************************/

static void Resize(mw)
    XwListRowColWidget	mw;
{
    static Dimension junk;

    DoLayout(mw, mw->core.width, mw->core.height, &junk, &junk, TRUE);
} /* Resize */



/*************************************<->*************************************
 *
 * TryNewLayout
 *
 * Try to do a new layout within the current width and height;
 * If our layout mode is XwIGNORE our current size is considered just fine, so
 *   just return.
 * If the needed size is the same as our current size return.
 * If our layout mode is XwMAXIMIZE then use which ever is larger, our
 *   current size or the size returned from DoLayout, for a Geometry request.
 *
 * If our geometry request gets an Almost, take whatever is offered.
 *
 * Note that TryNewLayout just says if it's possible, and doesn't actually 
 * move the kids
 ******************************************************************************/
static Boolean TryNewLayout(mw)
    XwListRowColWidget	mw;
{
    Dimension		width, height;
    XtWidgetGeometry request, reply;

    (void) DoLayout(mw, mw->core.width, mw->core.height, &width, &height,FALSE);

    if ((mw->manager.layout == XwIGNORE) || 	/* don't care if it fits */
        ((mw->core.width == width) && (mw->core.height == height))) 
           	return (TRUE);   /* Same size */

    
    if (mw->manager.layout == XwMAXIMIZE)
       {
	  width = Max (mw->core.width, width);
	  height = Max (mw->core.height, height);
       }

    if (width == 0 && height == 0) return (TRUE);

    switch (XtMakeResizeRequest
	         ((Widget) mw, width, height, &width, &height))
    {
	case XtGeometryYes:
	    return (TRUE);

	case XtGeometryNo:
	    return (FALSE);

	case XtGeometryAlmost:
	    if (! DoLayout(mw, width, height, &width, &height, FALSE))
	        return (FALSE);
	    (void) XtMakeResizeRequest((Widget) mw, width, height, 
					&width, &height);
	    return (TRUE);
    }
}


/**********************************************************************
 *
 * Geometry Manager
 *
 * Requests to change x,y position are always denied.
 **********************************************************************/

/*ARGSUSED*/
static XtGeometryResult GeometryManager(w, request, reply)
    Widget		w;
    XtWidgetGeometry	*request;
    XtWidgetGeometry	*reply;	/* RETURN */

{
    Dimension	width, height, borderWidth, junk;
    XwListRowColWidget mw;

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

	/* Decide if new layout works: (1) new button is smaller,
	   (2) new button fits in existing Manager, (3) Manager can be
	   expanded to allow new button to fit */

	mw = (XwListRowColWidget) w->core.parent;
	if (TryNewLayout(mw))
       {
	    /* Fits in existing or new space, relayout */
	    w->core.width = width;
  	    w->core.height = height;
	    w->core.border_width = borderWidth;
	    XtResizeWidget(w, request->width, request->height,
			    request->border_width);
    	    Resize(mw);
	    return (XtGeometryDone); /* We've already changed it */
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

/*************************************<->*************************************
 *
 *  ChangeManaged
 *
 *   Description:
 *   -----------
 *     Restructure the manager's "managed children" list.  Then re-lay
 *     out children.
 *
 *************************************<->***********************************/
static void ChangeManaged(mw)
    XwListRowColWidget mw;
{
    XwListWidget 	lw;
    XwManagerWidgetClass superclass;

    lw = (XwListWidget )mw->core.parent->core.parent;
    superclass = (XwManagerWidgetClass)XwlistWidgetClass;
    
    /* Put "managed children" info together again */

    if (!lw->list.InChangeManaged)
        (*superclass->composite_class.change_managed)(lw);    

    _XwReManageChildren ((XwManagerWidget)mw);

    /* Reconfigure the row column box */
    
    (void) TryNewLayout(mw);
    Resize(mw);

}

/*************************************<->*************************************
 *
 *  Initialize
 *
 *   Description:
 *   -----------
 *     Validate the row/col instance fields, and ensure that we are
 *     not requesting a 0x0 widget.  Also, clear selectedChild field
 *     so we don't think there's a selected child and core dump.
 *
 *************************************<->***********************************/

static void Initialize(request, new)
    XwListRowColWidget request, new;
{
	  if (new->row_col.h_space < 0)
	  {
	     XtWarning("Initialize: invalid h_space setting.");
	     new->row_col.h_space = 4;
	  }
	  if (new->row_col.v_space < 0)
	  {
	     XtWarning("Initialize: invalid v_space setting.");
	     new->row_col.v_space = 4;
	  }
	  if (new->row_col.columns < 1)
	  {
	     XtWarning("Initialize: invalid number of columns.");
	     new->row_col.columns = 1;
	  }
	  if ((new->row_col.mode != XwN_OF_MANY) &&
	     (new->row_col.mode != XwONE_OF_MANY))
	  {
	     XtWarning("Initialize: invalid mode setting.");
	     new->row_col.mode = XwN_OF_MANY;
	  }

   new->row_col.selectedChild = NULL;

  /* DONT ALLOW WIDTH/HEIGHT TO BE 0, OR X WILL DO  BAD THINGS TO US */

    if (new->core.width == 0)
        new->core.width =
	    ((new->row_col.h_space != 0) ? new->row_col.h_space : 10);
    if (new->core.height == 0)
	new->core.height = 
	    ((new->row_col.v_space != 0) ? new->row_col.v_space : 10);

    
} /* Initialize */

/****************************************************
 *
 * Set Values
 *
 * Description:
 * -----------
 *   Validate any changes that have been made to the
 *   Row/Col instance part of this widget.
 *
 *   Inputs:
 *   ------
 *    current = original widget;
 *    request = copy of current (?);
 *    new = copy of request which reflects changes made to it by
 *          set values procedures of its superclasses;

 *****************************************************/

static XtSetValuesFunc SetValues (current, request, new)
    XwListRowColWidget current, request, new;
{
    XtWidgetGeometry	reqGeo;
    XwListRowColWidget curbox = (XwListRowColWidget) current;
    XwListRowColWidget newbox = (XwListRowColWidget) new;
    Boolean  flag = FALSE;    /* our return value */
    
    if ((curbox->row_col.h_space != newbox->row_col.h_space) ||
        (curbox->row_col.v_space != newbox->row_col.v_space) ||
        (curbox->row_col.columns != newbox->row_col.columns) ||
        (curbox->row_col.mode != newbox->row_col.mode))
       {
	  if (newbox->row_col.h_space < 0)
	  {
	     XtWarning("SetValues: invalid h_space setting.");
	     newbox->row_col.h_space = curbox->row_col.h_space;
	  }
	  if (newbox->row_col.v_space < 0)
	  {
	     XtWarning("SetValues: invalid v_space setting.");
	     newbox->row_col.v_space = curbox->row_col.v_space;
	  }
	  if (newbox->row_col.columns < 1)
	  {
	     XtWarning("SetValues: invalid number of columns.");
	     newbox->row_col.columns = curbox->row_col.columns;
	  }
	  if ((newbox->row_col.mode != XwN_OF_MANY) &&
	     (newbox->row_col.mode != XwONE_OF_MANY))
	  {
	     XtWarning("SetValues: invalid mode setting.");
	     newbox->row_col.mode = curbox->row_col.mode;
	  }
          Resize(newbox);	  
          return((XtSetValuesFunc) TRUE);
       }
    
    return((XtSetValuesFunc) FALSE );
 }

