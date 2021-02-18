/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        RCManager.c
 **
 **   Project:     X Widgets
 **
 **   Description: The source for the row column manager widget.
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
#include <Xw/RCManagerP.h>
#include <Xw/RCManager.h>


static void            ClassInitialize();
static void      Initialize();
static void   Realize();
static void    Resize();
static void    InsertChild();
static Boolean SetValues();

static void      ChangeManaged();
static XtGeometryResult GeometryManager();

static void ConstraintInitialize();
static Boolean ConstraintSetValues();

static XtGeometryResult XwQueryGeometry();

static void XwRequestedColumnsLayout();
static void XwMaximumColumnsLayout();
static void XwMaximumUnalignedLayout();
static void XwForceSize();
static void XwMaximizeLayout();
static void XwLayoutChildren();
static XtGeometryResult XwMakeSizeRequest();


/*  Row Column Manager Resources  */

static XtResource resources[] =
{
   {
      XtNhSpace, XtCHSpace, XtRDimension, sizeof(Dimension),
      XtOffset(XwRowColWidget, row_col.h_space), XtRString, "4"
   },

   {
      XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension),
      XtOffset(XwRowColWidget, row_col.v_space), XtRString, "4"
   },

   {
      XtNlayoutType, XtCLayoutType, XtRLayoutType, sizeof(int),
      XtOffset(XwRowColWidget, row_col.layout_type),
      XtRString, "requested_columns"
   },

   {
      XtNcolumns, XtCColumns, XtRInt, sizeof(int),
      XtOffset(XwRowColWidget, row_col.columns), XtRString, "1"
   },

   {
      XtNforceSize, XtCForceSize, XtRBoolean, sizeof(Boolean),
      XtOffset(XwRowColWidget, row_col.force_size), XtRString, "False"
   },

   {
      XtNsingleRow, XtCSingleRow, XtRBoolean, sizeof(Boolean),
      XtOffset(XwRowColWidget, row_col.single_row), XtRString, "False"
   },

   {
      XtNmode, XtCMode, XtRRCMode, sizeof(int),
      XtOffset(XwRowColWidget, row_col.mode), XtRString, "n_of_many"
   },
    
};


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

XwRowColClassRec XwrowColClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &XwmanagerClassRec,
    /* class_name         */    "RowCol",
    /* widget_size        */    sizeof(XwRowColRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,				
    /* class_inited       */	FALSE,
    /* initialize         */    (XtInitProc) Initialize,
    /* initialize_hook    */    NULL,
    (XtRealizeProc) Realize,          /* realize	         */	
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterlv   */    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    (XtWidgetProc) Resize,
    /* expose             */	NULL,
    /* set_values         */    (XtSetValuesFunc) SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus       */    NULL,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      NULL,
    /* query_geometry   */      (XtGeometryHandler) XwQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    (XtGeometryHandler) GeometryManager,
    /* change_managed     */    (XtWidgetProc) ChangeManaged,
    /* insert_child	  */	(XtWidgetProc) InsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
				NULL,
  },{
/* constraint_class fields */
   /* resource list        */   NULL,
   /* num resources        */   0,
   /* constraint size      */   sizeof (XwRCConstraintRec),
   /* init proc            */   (XtInitProc) ConstraintInitialize,
   /* destroy proc         */   NULL,
   /* set values proc      */   (XtSetValuesFunc) ConstraintSetValues,
				NULL,
  },{
/* manager_class fields */
   /* traversal handler   */    (XwTraversalProc) XtInheritTraversalProc,
   /* translations        */	NULL,
  },{
/* row column class - none */     
     /* mumble */               0
 }	
};

WidgetClass XwrowColWidgetClass = (WidgetClass)&XwrowColClassRec;


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
Widget   w;
caddr_t  closure;
caddr_t  call_data;
{
   XwRowColWidget mw = (XwRowColWidget) w -> core.parent;
   XwPrimitiveWidget oldChild = NULL;
   register int i;

   if (mw -> row_col.mode != XwONE_OF_MANY) return;


   /*  Ensure that old child is still in the child list  */

   for (i = 0; i < mw -> composite.num_children; i++)
      if (mw -> composite.children[i] == mw -> row_col.selectedChild)
         oldChild = (XwPrimitiveWidget) mw -> row_col.selectedChild;

   mw->row_col.selectedChild = w;

   if ((oldChild != NULL) && ((Widget) oldChild != w))
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

static void InsertChild (w)
    Widget	w;
{
    int _XW_NARGS = 0;
    ArgList	args = NULL;
    Cardinal  *p_num_args = (Cardinal *) &_XW_NARGS;

    Cardinal	    position;
    Cardinal	    i;
    XwRowColWidget cw;
    XwManagerWidgetClass superclass;

    cw = (XwRowColWidget) w->core.parent;
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
           (cw->row_col.mode == XwONE_OF_MANY)     &&     
           (((XwButtonWidget)(w))->button.set))  
       {
          if (cw->row_col.selectedChild != NULL)
          {
             XtSetArg(rcArgs[0], XtNset, FALSE);
	     XtSetValues ((Widget)(cw->row_col.selectedChild),
			  (ArgList)rcArgs, 1);
			      
          }
          cw->row_col.selectedChild = w;
       }
    }
}





/*************************************<->*************************************
 *
 *  Resize 
 *  	Layout the button box according to our current size.
 *
 *************************************<->***********************************/

static void Resize (mw)
XwRowColWidget mw;

{
   if (XtIsRealized (mw)) XwLayoutChildren (mw, False);
}




/************************************************************************
 *
 *  Geometry Manager
 *	Requests to change x,y position are always denied.
 *
 ************************************************************************/

static XtGeometryResult GeometryManager (w, request, reply)
Widget		   w;
XtWidgetGeometry * request;
XtWidgetGeometry * reply;	/* RETURN */

{
   XwRowColWidget mw = (XwRowColWidget) w->core.parent;
   XwRCConstraintRec * constraint = (XwRCConstraintRec *) w->core.constraints;


   /*  If a size change has been requested, make sure that the size  */
   /*  is valid, save the data in the constraint record, force a     */
   /*  resize on the widget, and layout the entire child size out.   */

   if (request->request_mode & (CWWidth | CWHeight | CWBorderWidth))
   {
      if ((request->request_mode & CWWidth) == 0)
         request->width = w->core.width;
      if ((request->request_mode & CWHeight) == 0)
	 request->height = w->core.height;
      if ((request->request_mode & CWBorderWidth) == 0)
         request->border_width = w->core.border_width;

      constraint -> set_width = request -> width;
      constraint -> set_height = request -> height;

      XtResizeWidget (w, request->width,
                      request->height, request->border_width);

      XwLayoutChildren (mw, True);

      return (XtGeometryDone);
   }

   return (XtGeometryYes);
}





/*************************************<->*************************************
 *
 *  ChangeManaged
 *	Loop through the child set saving any new size information in
 *      the childs constraint record.  Then call the child layout routine.
 *
 *************************************<->***********************************/

static void ChangeManaged (mw)
XwRowColWidget mw;

{
   register Widget child;
   register XwRCConstraintRec * constraint;
   register int i;


   for (i = 0; i < mw -> composite.num_children; i++)
   {
      child = mw -> composite.children[i];
      constraint = (XwRCConstraintRec *) child -> core.constraints;

      if (constraint -> set_width == 0)
      {
         constraint -> set_width = child -> core.width;
         constraint -> set_height = child -> core.height;
      }
   }

   XwLayoutChildren (mw, True);
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

static void Initialize (request, new)
XwRowColWidget request, new;

{
   if (new->row_col.columns < 1)
   {
      XtWarning ("RowColumn: Invalid number of columns.");
      new->row_col.columns = 1;
   }

   if (new -> row_col.layout_type != XwREQUESTED_COLUMNS &&
       new -> row_col.layout_type != XwMAXIMUM_COLUMNS   &&
       new -> row_col.layout_type != XwMAXIMUM_UNALIGNED)
   {
      XtWarning ("RowColumn: Invalid layout type setting.");
      new -> row_col.layout_type = XwREQUESTED_COLUMNS;
   }

   if (new -> row_col.force_size != True &&
       new -> row_col.force_size != False)
   {
      new -> row_col.force_size = False;
      XtWarning ("RowColumn: Invalid force size setting.");      
   }

   if (new -> row_col.single_row != True &&
       new -> row_col.single_row != False)
   {
      new -> row_col.single_row = False;
      XtWarning ("RowColumn: Invalid single row setting.");      
   }

   if (new -> row_col.layout_type == XwREQUESTED_COLUMNS &&
       new -> row_col.single_row == True)
   {
      new -> row_col.single_row = False;
      XtWarning ("RowColumn: Invalid single row setting.");      
   }

   if ((new->row_col.mode != XwN_OF_MANY) &&
       (new->row_col.mode != XwONE_OF_MANY))
   {
      XtWarning ("RowColumn: Invalid mode setting.");
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





/*************************************<->*************************************
 *
 *  ConstraintInitialize
 *
 *   Description:
 *   -----------
 *     Initialize the two dimension fields in the constraint record to 0.
 *
 *************************************<->***********************************/

static void ConstraintInitialize (request, new)
XwRowColWidget request, new;

{
   XwRCConstraintRec * constraintRec;

   constraintRec = (XwRCConstraintRec *) new -> core.constraints;

   constraintRec -> set_width = 0;
   constraintRec -> set_height = 0;
}




/************************************************************************
 *
 *  Realize
 *	Create the widget window and create the gc's.
 *
 ************************************************************************/

static void Realize (mw, valueMask, attributes)
XwRowColWidget     mw;
XtValueMask          * valueMask;
XSetWindowAttributes * attributes;

{
   Mask newValueMask = *valueMask;
   XtCreateWindow (mw, InputOutput, (Visual *) CopyFromParent,
		   newValueMask, attributes);
   _XwRegisterName(mw);
   XwLayoutChildren (mw, False);
}





/************************************************************************
 *
 *  Set Values
 *
 *  Description:
 *  -----------
 *    Validate any changes that have been made to the
 *    Row/Col instance part of this widget.
 *
 *  Inputs:
 *   ------
 *    current = original widget;
 *    request = copy of current (?);
 *    new = copy of request which reflects changes made to it by
 *          set values procedures of its superclasses;
 *
 ************************************************************************/

static Boolean SetValues (current, request, new)
XwRowColWidget current, request, new;

{
   XtWidgetGeometry reqGeo;
   XwRowColWidget curbox = (XwRowColWidget) current;
   XwRowColWidget newbox = (XwRowColWidget) new;
    
   if ((curbox->row_col.h_space != newbox->row_col.h_space)         ||
       (curbox->row_col.v_space != newbox->row_col.v_space)         ||
       (curbox->row_col.columns != newbox->row_col.columns)         ||
       (curbox->row_col.layout_type != newbox->row_col.layout_type) ||
       (curbox->row_col.force_size != newbox->row_col.force_size)   ||
       (curbox->row_col.single_row != newbox->row_col.single_row)   ||
       (curbox->row_col.mode != newbox->row_col.mode))
   {
      if (newbox->row_col.columns < 1)
      {
         XtWarning ("RowColumn: invalid number of columns.");
         newbox->row_col.columns = curbox->row_col.columns;
      }

      if (newbox -> row_col.layout_type != XwREQUESTED_COLUMNS &&
          newbox -> row_col.layout_type != XwMAXIMUM_COLUMNS &&
          newbox -> row_col.layout_type != XwMAXIMUM_UNALIGNED)
      {
         XtWarning ("RowColumn: Invalid layout type setting.");
         newbox -> row_col.layout_type = curbox -> row_col.layout_type;
      }

      if (newbox -> row_col.force_size != True &&
          newbox -> row_col.force_size != False)
      {
         newbox -> row_col.force_size = curbox -> row_col.force_size;
         XtWarning ("RowColumn: Invalid force size setting.");      
      }

      if (newbox -> row_col.single_row != True &&
          newbox -> row_col.single_row != False)
      {
         newbox -> row_col.single_row = curbox -> row_col.single_row;
         XtWarning ("RowColumn: Invalid single row setting.");      
      }

      if (newbox -> row_col.layout_type == XwREQUESTED_COLUMNS &&
          newbox -> row_col.single_row == True)
      {
         newbox -> row_col.single_row = False;
         XtWarning ("RowColumn: Invalid single row setting.");      
      }

      if ((newbox->row_col.mode != XwN_OF_MANY) &&
          (newbox->row_col.mode != XwONE_OF_MANY))
      {
         XtWarning ("RowColumn: invalid mode setting.");
         newbox->row_col.mode = curbox->row_col.mode;
      }

      if (XtIsRealized (curbox)) XwLayoutChildren (newbox, True);
   }
    
   return (False);
}




/************************************************************************
 *
 *  ConstraintSetValues
 *	Process changes in the constraint set of a widget.
 *
 ************************************************************************/

static Boolean ConstraintSetValues (current, request, new)
Widget current, request, new;

{
   XwRCConstraintRec * newConstraint;


   /*  Check the geometrys to see if new's contraint record  */
   /*  saved geometry data needs to be updated.              */

   if (XtIsRealized (current))
   {
      newConstraint = (XwRCConstraintRec *) new -> core.constraints;

      if (new -> core.width != current -> core.width)
         newConstraint -> set_width = new -> core.width;
      if (new -> core.height != current -> core.height)
         newConstraint -> set_height = new -> core.height;
   }

   return (False);
}



/************************************************************************
 *
 *  QueryGeometry
 *  	Calculate the width and height of the rc manager and return
 *	the values.
 *
 ************************************************************************/

static XtGeometryResult XwQueryGeometry (mw, request, ret)
XwRowColWidget mw;
XtWidgetGeometry * request;
XtWidgetGeometry * ret;

{
   XwRCLayoutList * layoutList;
   int layoutCount = 0;
   Widget child;
   XwRCConstraintRec * constraint;
   register int i;
   int columnCount, rowCount;
   int managerWidth, managerHeight;
   int saveWidth, saveHeight;
   XtGeometryResult result = XtGeometryYes;


   /*  Hack around bug in XtQueryGeometry  */

   ret -> request_mode = 0;


   /*  Allocate a layout list to use in layout processing, then  */
   /*  loop through the children to get their preferred sizes.   */
   
   if (mw -> composite.num_children != 0)
      layoutList = (XwRCLayoutList *)
         XtMalloc (sizeof (XwRCLayoutList) * mw -> composite.num_children);

   for (i = 0; i < mw -> composite.num_children; i++)
   {
      layoutList[i].widget = NULL;
      child = mw -> composite.children[i];

      if (child -> core.managed)
      {
         constraint = (XwRCConstraintRec *) child -> core.constraints;

         layoutList[layoutCount].widget = child;
         layoutList[layoutCount].x = layoutList[layoutCount].y = 0;
         layoutList[layoutCount].width =
	    layoutList[layoutCount].set_width = constraint -> set_width;
         layoutList[layoutCount].height =
            layoutList[layoutCount].set_height = constraint -> set_height;
         layoutList[layoutCount].border = child -> core.border_width * 2;
         layoutCount++;
      }
   }


   /*  Initialize the managers width and height values  */

   saveWidth = mw -> core.width;
   saveHeight = mw -> core.height;

   if (request-> request_mode & CWWidth)
   {
      mw -> core.width = request -> width;
      if ((request-> request_mode & CWHeight) == 0)
         mw -> core.height = 99999;

   }
   if (request-> request_mode & CWHeight)
   {
      mw -> core.height = request -> height;
      if ((request-> request_mode & CWWidth) == 0)
         mw -> core.width = 99999;
   }

   if ((request-> request_mode & CWWidth) == 0 &&
       (request-> request_mode & CWHeight) == 0)
   {
      mw -> core.width = 99999;
      mw -> core.height = 99999;
   }


   /*  Check for no managed children.  If this is the case       */
   /*  a relayout needs to occur to minimum size otherwise       */
   /*  calculate the layout array according to the layout type.  */

   if (layoutCount == 0)
      managerWidth = managerHeight = 10 + mw -> manager.highlight_thickness;
   else
   {
      if (mw -> row_col.layout_type == XwREQUESTED_COLUMNS)
      {
         columnCount = mw -> row_col.columns;
         XwRequestedColumnsLayout (mw, layoutList, layoutCount, False,
   		 		   columnCount, &rowCount,
			  	   &managerWidth, &managerHeight);
      }

      else if (mw -> row_col.layout_type == XwMAXIMUM_COLUMNS)
         XwMaximumColumnsLayout (mw, layoutList, layoutCount, False,
                                 &columnCount, &rowCount,
                                 &managerWidth, &managerHeight);

      else
         XwMaximumUnalignedLayout (mw, layoutList, layoutCount, False,
	  			   &rowCount, &managerWidth, &managerHeight);
   }


   /*  Calculate the preferred return according to the request  */
   /*  and the returned layout data.                            */

   if ((request-> request_mode & CWWidth) &&
       (request-> request_mode & CWHeight))
   {
      if (mw -> manager.layout == XwMAXIMIZE)
      {
         if (request -> width < managerWidth)
         {
            ret -> request_mode |= CWWidth;
            ret -> width = managerWidth;
            result = XtGeometryAlmost;
         }

         if (request -> height < managerHeight)
         {
            ret -> request_mode |= CWHeight;
            ret -> height = managerHeight;
            result = XtGeometryAlmost;
         }
      }         
      else
      {
         if (request -> width > managerWidth)
         {
            ret -> request_mode |= CWWidth;
            ret -> width = managerWidth;
            result = XtGeometryAlmost;
         }

         if (request -> height > managerHeight)
         {
            ret -> request_mode |= CWHeight;
            ret -> height = managerHeight;
            result = XtGeometryAlmost;
         }
      }
   }

   else if (request-> request_mode & CWWidth)
   {
      ret -> request_mode |= CWHeight;
      ret -> height = managerHeight;
      if (request -> width < managerWidth)
      {
         ret -> request_mode |= CWWidth;
         ret -> width = managerWidth;
      }
      result = XtGeometryAlmost;
   }

   else if (request-> request_mode & CWHeight)
   {
      ret -> request_mode |= CWWidth;
      ret -> width = managerWidth;
      if (request -> height < managerHeight)
      {
         ret -> request_mode |= CWHeight;
         ret -> height = managerHeight;
      }
      result = XtGeometryAlmost;
   }

   else
   {
      ret -> request_mode |= CWWidth;
      ret -> width = managerWidth;
      ret -> request_mode |= CWHeight;
      ret -> height = managerHeight;
      result = XtGeometryAlmost;
   }


   /*  Restore the widget's width and height variables, free  */
   /*  the layout processing list and return the result.      */

   mw -> core.width = saveWidth;
   mw -> core.height = saveHeight;

   if (mw -> composite.num_children != 0)
      XtFree (layoutList);

   return (result);
}





/*************************************<->***********************************
 *
 *  LayoutChildren
 *  	Calculate the positions of each managed child and perform the layout.
 *
 *************************************<->***********************************/

static void XwLayoutChildren (mw, resizable)
XwRowColWidget mw;
Boolean        resizable;

{
   XwRCLayoutList * layoutList;
   int layoutCount = 0;
   Widget child;
   XwRCConstraintRec * constraint;
   register int i;
   int columnCount, rowCount;
   int managerWidth, managerHeight;
   int replyWidth, replyHeight;
   int saveWidth, saveHeight;
   XtGeometryResult requestReturn;
   Boolean moveFlag = False;
   Boolean resizeFlag = False;


   /*  Allocate a layout list to use in layout processing, then  */
   /*  loop through the children to get their preferred sizes.   */
   
   if (mw -> composite.num_children != 0)
      layoutList = (XwRCLayoutList *)
         XtMalloc (sizeof (XwRCLayoutList) * mw -> composite.num_children);

   for (i = 0; i < mw -> composite.num_children; i++)
   {
      layoutList[i].widget = NULL;
      child = mw -> composite.children[i];

      if (child -> core.managed)
      {
         constraint = (XwRCConstraintRec *) child -> core.constraints;

         layoutList[layoutCount].widget = child;
         layoutList[layoutCount].x = layoutList[layoutCount].y = 0;
         layoutList[layoutCount].width =
	    layoutList[layoutCount].set_width = constraint -> set_width;
         layoutList[layoutCount].height =
            layoutList[layoutCount].set_height = constraint -> set_height;
         layoutList[layoutCount].border = child -> core.border_width * 2;
         layoutCount++;
      }
   }


   /*  Check for no managed children.  If this is the case  */
   /*  a relayout needs to occur to minimum size.           */

   if (layoutCount == 0)
   {
      if (resizable)
      {
         managerWidth = managerHeight = 10 + mw->manager.highlight_thickness;
         requestReturn = XtMakeResizeRequest(mw, managerWidth, managerHeight, 
                                                 &replyWidth, &replyHeight);

         if (requestReturn == XtGeometryAlmost)
            XtMakeResizeRequest ((Widget) mw, replyWidth, replyHeight,
                                              NULL, NULL);

      }

      if (mw -> composite.num_children != 0)
         XtFree (layoutList);
      return;
   }


   /*  Calculate the layout array according to the layout type.  */

   if (mw -> row_col.layout_type == XwREQUESTED_COLUMNS)
   {
      columnCount = mw -> row_col.columns;
      XwRequestedColumnsLayout (mw, layoutList, layoutCount, resizable,
				columnCount, &rowCount,
				&managerWidth, &managerHeight);
   }

   else if (mw -> row_col.layout_type == XwMAXIMUM_COLUMNS)
      XwMaximumColumnsLayout (mw, layoutList, layoutCount, resizable,
                              &columnCount, &rowCount,
                              &managerWidth, &managerHeight);

   else
      XwMaximumUnalignedLayout (mw, layoutList, layoutCount, resizable,
				&rowCount, &managerWidth, &managerHeight);


   /*  We now have an array with the set of minimum position for each       */
   /*  widget.  We also have a manager width and height needed for          */
   /*  this layout.  Now process the width and height based on the          */
   /*  current manager size, resizablility, and the manager's layout flag.  */

   switch ((requestReturn = 
               XwMakeSizeRequest (mw, managerWidth, managerHeight, 
                                  &replyWidth, &replyHeight, resizable)))
   {
      case XtGeometryDone:
      case XtGeometryYes:
      break;

      case XtGeometryNo:
         saveWidth = mw -> core.width;
         saveHeight = mw -> core.height;
         mw -> core.height = 99999;
      break;

      case XtGeometryAlmost:
         saveWidth = mw -> core.width;
         saveHeight = mw -> core.height;
         mw -> core.width = replyWidth;
         mw -> core.height = 99999;
      break;
   }

   if (requestReturn == XtGeometryNo || requestReturn == XtGeometryAlmost)
   {
      if (mw -> row_col.layout_type == XwREQUESTED_COLUMNS)
      {
         columnCount = mw -> row_col.columns;
         XwRequestedColumnsLayout (mw, layoutList, layoutCount, False,
   		   		   columnCount, &rowCount,
				   &managerWidth, &managerHeight);
      }

      else if (mw -> row_col.layout_type == XwMAXIMUM_COLUMNS)
         XwMaximumColumnsLayout (mw, layoutList, layoutCount, False,
                                 &columnCount, &rowCount,
                                 &managerWidth, &managerHeight);

      else
         XwMaximumUnalignedLayout (mw, layoutList, layoutCount, False,
				   &rowCount, &managerWidth, &managerHeight);


      mw -> core.width = saveWidth;
      mw -> core.height = saveHeight;

      requestReturn = XwMakeSizeRequest (mw, managerWidth, managerHeight, 
                                             &replyWidth, &replyHeight, True);

      if (requestReturn == XtGeometryAlmost)
      {
         XtMakeResizeRequest ((Widget) mw, replyWidth, replyHeight,
                                           NULL, NULL);
         XtFree (layoutList);
         Resize (mw);
         return;
      }
      else if (requestReturn == XtGeometryNo)
      {
         XtFree (layoutList);
         Resize (mw);
         return;
      }
   }


   /*  Match the child sizes if force size is set.  */

   if (mw -> row_col.force_size)
      XwForceSize (mw, layoutList, layoutCount, columnCount, rowCount);


   /*  Adjust the positions of the widgets for the maximize case  */

   if (mw -> manager.layout == XwMAXIMIZE)
      XwMaximizeLayout (mw, layoutList, layoutCount,
			columnCount, rowCount, managerWidth, managerHeight);


   /*  Relayout the children, making only necessary layout changes.  */

   for (i = 0; i < layoutCount; i++)
   {
      moveFlag = resizeFlag = False;

      if (layoutList[i].x != layoutList[i].widget -> core.x ||
          layoutList[i].y != layoutList[i].widget -> core.y)
            moveFlag = True;

      if (layoutList[i].width != layoutList[i].widget -> core.width ||
          layoutList[i].height != layoutList[i].widget -> core.height)
            resizeFlag = True;

      if (moveFlag && resizeFlag)
         XtConfigureWidget (layoutList[i].widget,
			    layoutList[i].x, layoutList[i].y,
			    layoutList[i].width, layoutList[i].height,
                            layoutList[i].widget -> core.border_width);
      else if (moveFlag)
         XtMoveWidget (layoutList[i].widget,
                       layoutList[i].x, layoutList[i].y);
      else if (resizeFlag)
         XtResizeWidget (layoutList[i].widget,
                         layoutList[i].width, layoutList[i].height,
                         layoutList[i].widget -> core.border_width);
   }


   /*  Free the layout list and return  */

   XtFree (layoutList);
}




/************************************************************************
 *
 *  XwMakeSizeRequest
 *	Make the geometry resize request to the parent and return
 *	the result.
 *
 ************************************************************************/

static XtGeometryResult 
XwMakeSizeRequest (mw, managerWidth, managerHeight, 
                       replyWidth, replyHeight, resizable)
XwRowColWidget mw;
int managerWidth;
int managerHeight;
int * replyWidth;
int * replyHeight;
Boolean resizable;

{
   int requestWidth, requestHeight;


   /*  First see if a geometry request needs to be made.  If so, make the  */
   /*  request.  If YES, return.  If ALMOST, make the request and return.  */
   /*  If NO, continue with the current processing.                        */

   if (mw -> manager.layout == XwMAXIMIZE)
   {
      requestWidth = Max (mw -> core.width, managerWidth);
      requestHeight = Max (mw -> core.height, managerHeight);
   }
   else if  (mw -> manager.layout == XwMINIMIZE)
   {
      requestWidth = managerWidth;
      requestHeight = managerHeight;
   }
      
   if (resizable                       &&
       mw->manager.layout != XwIGNORE  &&
       (requestWidth != mw->core.width || requestHeight != mw->core.height))
      return (XtMakeResizeRequest ((Widget) mw, requestWidth, requestHeight,
                                                replyWidth, replyHeight));
   else
      return (XtGeometryDone);
}





/************************************************************************
 *
 *  XwRequestedColumnsLayout
 *
 ************************************************************************/

static void XwRequestedColumnsLayout (mw, layoutList, layoutCount, resizable,
                                      columnCount, rowCount,
                                      managerWidth, managerHeight)
XwRowColWidget mw;
XwRCLayoutList layoutList[];
int            layoutCount;
Boolean        resizable;
int   columnCount;
int * rowCount;
int * managerWidth;
int * managerHeight;

{
   register int i;
   register int j;
   register int k;
   register int size = mw -> row_col.h_space;
   register int position = 0;
   int hpad = mw -> row_col.h_space;
   int vpad = mw -> row_col.v_space;


   *rowCount = layoutCount / columnCount;
   if (layoutCount % columnCount) *rowCount = *rowCount + 1;


   /*  Set up a loop to process all of the columns to calculate the  */
   /*  x positions for the column.                                   */

   for (i = 0; i < columnCount && i < layoutCount; i++)
   {
      position = size;		/* Current x value for this column    */
      size = 0;			/* Calculated x value for next column */

      for (j = 0, k = i;
           j < *rowCount && k < layoutCount; j++, k += columnCount)
      {
         layoutList[k].x = position;
	 size = Max (size, position + 
                           layoutList[k].width + layoutList[k].border + hpad);
      }
   }
   *managerWidth = size;


   /*  Set up a loop to process all of the rows to calculate the  */
   /*  y positions for the row.                                   */

   position = 0;
   size = vpad;
   
   for (i = 0; i < layoutCount; i += columnCount)
   {
      position += size;		/* Current y value for this row    */
      size = 0;			/* Calculated y value for next row */

      for (j = i; j < i + columnCount && j < layoutCount; j++)
      {
         layoutList[j].y = position;
	 size = Max (size, layoutList[j].height + layoutList[j].border + vpad);
      }
   }
   *managerHeight = position + size;

}



/************************************************************************
 *
 *  XwMaximumColumnsLayout
 *
 ************************************************************************/

static void XwMaximumColumnsLayout (mw, layoutList, layoutCount, resizable,
  				    columnCount, rowCount,
                                    managerWidth, managerHeight)
XwRowColWidget mw;
XwRCLayoutList layoutList[];
int layoutCount;
Boolean resizable;
int * columnCount;
int * rowCount;
int * managerWidth;
int * managerHeight;

{
   int rowY = mw -> row_col.v_space;
   register int i = 0;
   register int size;
   int hpad = mw -> row_col.h_space;
   int vpad = mw -> row_col.v_space;


   *managerWidth = 0;
   *managerHeight = layoutList[0].height + layoutList[i].border + vpad + vpad;


   /*  If the single row flag and resizable flags are set, layout in  */
   /*  a single row.                                                  */
       
   if (mw->row_col.single_row && resizable && mw->manager.layout != XwIGNORE)
   {
      layoutList[0].x = hpad;
      layoutList[0].y = rowY;

      for (i = 1; i < layoutCount; i++)
      {
         layoutList[i].x = layoutList[i - 1].x + layoutList[i - 1].width +
                           layoutList[i - 1].border + hpad;
	 layoutList[i].y = rowY;
         if (*managerHeight < layoutList[i].height + 
                              layoutList[i].border + vpad + vpad)
            *managerHeight = layoutList[i].height + 
                             layoutList[i].border + vpad + vpad;
      }

      *managerWidth = layoutList[i - 1].x + layoutList[i - 1].width +
                      layoutList[i - 1].border + hpad;

      *columnCount = layoutCount;
      *rowCount = 1;
      return;
   }


   /*  Set up a loop to calculate the maximum number of columns then use  */
   /*  the value  to input to the RequestedColumns layout function.       */
   /*  Incrementally decrease i until RequstedColumns returns a needed    */
   /*  width less than the manager's current width.                       */

   for (i = 0, size = hpad; i < layoutCount && size < mw -> core.width; i++)
      size += layoutList[i].width + layoutList[i].border + hpad;

   if (i == 0) i = 1;

   do
   {
      XwRequestedColumnsLayout (mw, layoutList, layoutCount, resizable,
				i, rowCount, managerWidth, managerHeight);
   } while (*managerWidth > mw -> core.width && --i > 0);

   if (i == 0) i = 1;
   *columnCount = i;
}




/************************************************************************
 *
 *  XwMaximumUnalignedLayout
 *
 ************************************************************************/

static void XwMaximumUnalignedLayout (mw, layoutList, layoutCount, resizable,
                                      rowCount, managerWidth, managerHeight)
XwRowColWidget mw;
XwRCLayoutList layoutList[];
int layoutCount;
Boolean resizable;
int * rowCount;
int * managerWidth;
int * managerHeight;

{
   int rowHeight;
   int rowY = mw -> row_col.v_space;
   register int i = 0;
   register int j;
   int hpad = mw -> row_col.h_space;
   int vpad = mw -> row_col.v_space;


   *managerWidth = 0;
   *managerHeight = layoutList[0].widget -> core.height;
   *rowCount = 1;


   /*  Loop through the entire layout list.  Set up the first element  */
   /*  of a row and then generate the horizontal position of each      */
   /*  subsequent element based on the previous element.  If the       */
   /*  single row flag and resizable flags are set, layout in a single */
   /*  row.  Otherwise, break the rows on the current thickness.       */
       
   while (i < layoutCount)
   {
      layoutList[i].x = hpad;
      layoutList[i].y = rowY;
      rowHeight = layoutList[i].height + vpad;
      i++;

      for (j = i; j < layoutCount; j++, i++)
      {
	 if ((mw -> row_col.single_row &&
	      resizable                &&
	      mw->manager.layout != XwIGNORE) ||
	     layoutList[j - 1].x + layoutList[j - 1].width +
	     layoutList[j - 1].border + hpad + layoutList[j].width +
	     layoutList[j].border + hpad <= mw -> core.width)
	 {
	    layoutList[j].x = layoutList[j - 1].x + layoutList[j - 1].width +
                              layoutList[j - 1].border + hpad;
	    layoutList[j].y = rowY;
	    rowHeight = Max (rowHeight, layoutList[j].height +
                                        layoutList[j].border + vpad);
	 }
	 else
	 {
            if (*managerWidth < layoutList[j - 1].x + layoutList[j - 1].width +
                                layoutList[j - 1].border + hpad)
               *managerWidth = layoutList[j - 1].x + layoutList[j - 1].width +
                               layoutList[j - 1].border + hpad;
	    rowY += rowHeight;
            *rowCount = *rowCount + 1;
	    break;
	 }
      }
   }


   if (*rowCount == 1)
      *managerWidth = layoutList[layoutCount - 1].x +
                      layoutList[layoutCount - 1].width +
                      layoutList[layoutCount - 1].border + hpad;
   *managerHeight = rowY + rowHeight;   
}




/************************************************************************
 *
 *  XwForceSize
 *	Force the childrens widths in a column and heights in a row
 *      to match.
 *
 ************************************************************************/

static void XwForceSize (mw, layoutList, layoutCount, columnCount, rowCount)
XwRowColWidget mw;
XwRCLayoutList layoutList[];
int layoutCount;
int columnCount;
int rowCount;

{
   register int i;
   register int j;
   register int k;
   register int size;


   /*  Force the widths in each of the columns to be the same  */

   if (mw -> row_col.layout_type != XwMAXIMUM_UNALIGNED)
   {
      for (i = 0; i < columnCount; i++)
      {
         for (j = i, size = 0; j < layoutCount; j += columnCount)
  	    size = Max (size, layoutList[j].width);

         for (k = i; k < j; k += columnCount)
  	    layoutList[k].width = size;
      }
   }


   /*  Force the heights in each of the rows to be the same  */

   i = 0;
   while (i < layoutCount)
   {
      for (j = i, size = 0;
	   layoutList[j].y == layoutList[i].y && 
           layoutList[j].widget != NULL       &&
           j < layoutCount;
           j++)
	 size = Max (size, layoutList[j].height);

      for (k = i; k < j; k++)
	 layoutList[k].height = size;

      i += j;
   }
}




/************************************************************************
 *
 *  XwMaximizeLayout
 *	Add spacing between the widgets in horizontal and/or vertical
 *      directions.
 *
 ************************************************************************/

static void XwMaximizeLayout (mw, layoutList, layoutCount, columnCount,
			      rowCount, managerWidth, managerHeight)
XwRowColWidget mw;
XwRCLayoutList layoutList[];
int layoutCount;
int columnCount;
int rowCount;
int managerWidth;
int managerHeight;

{
   register int i;
   register int j;
   register int dif;
   register int add;
   register int adjust;
   int currentY;


   /*  Adjust all of the horizontal positions  */

   dif = mw -> core.width - managerWidth;

   if (dif > 0 && mw -> row_col.layout_type != XwMAXIMUM_UNALIGNED)
   {
      add = dif / (columnCount + 1);
      adjust = add;

      if (add > 0)
      {
	 for (i = 0; i < columnCount; i++, dif -= add, adjust += add)
	 {
	    for (j = i; j < layoutCount; j += columnCount)
	       layoutList[j].x += adjust;
	 }
      }

      dif -= add;
      if (dif > 0)
      {
         adjust = 0;

         for (i = 0; i < columnCount; i++)
         {
            if (adjust < dif) adjust++;
            for (j = i; j < layoutCount; j += columnCount)
               layoutList[j].x += adjust;
         }
      }
   }


   /*  Adjust all of the vertical positions  */

   dif = mw -> core.height - managerHeight;

   if (dif > 0)
   {
      add = dif / (rowCount + 1);

      if (add > 0)
      {
	 i = 0;
         adjust = add;

	 while (i < layoutCount)
         {
	    currentY = layoutList[i].y;
	    dif -= add;
            for (j = i;
  	         layoutList[j].y == currentY && layoutList[j].widget != NULL;
	         j++, i++)
	      layoutList[j].y += adjust;
            adjust += add;
	 }
      }
	    
      dif -= add;
      if (dif > 0)
      {
         i = 0;
         adjust = 0;

         while (i < layoutCount)
         {
            currentY = layoutList[i].y;
            if (adjust < dif) adjust++;

            for (j = i;
                 layoutList[j].y == currentY && layoutList[j].widget != NULL;
                 j++, i++)
               layoutList[j].y += adjust;
         }
      }
   }
}
