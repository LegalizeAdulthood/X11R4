/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        List.c
 **
 **   Project:     X Widgets
 **
 **   Description: Code and class record for List widget
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


#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/ScrollBar.h>
#include <Xw/Arrow.h>
#include <Xw/Valuator.h>
#include <Xw/ScrollBarP.h>
#include <Xw/BBoard.h>
#include <Xw/BBoardP.h>
#include <Xw/SWindow.h>
#include <Xw/SWindowP.h>
#include <Xw/SText.h>
#include <Xw/ListRC.h>
#include <Xw/ListRCP.h>
#include <Xw/List.h>
#include <Xw/ListP.h>
#include <stdio.h>

#define BUTTONDOWN 1
#define CTRLDOWN   2
#define XtNdoubleClick	"doubleClick"

static void Initialize();
static void Realize();
static void Resize();
static void InsertChild();
static XtSetValuesFunc SetValues();
static XtGeometryResult GeometryManager();
static void ChangeManaged();
static void ClassInitialize();
static void EnterElement(),LeaveElement();
static void BBEnter(),BBLeave();
static void ExSelect(),ExUnSelect();
static void SelectElement(),UnSelectElement();
static void BrowseElement();
static void OrderList();
static void ListDestroy();
static XtInitProc ConstraintInitialize();
static XtSetValuesFunc ConstraintSetValues();
static void HighlightWidget();
static void UnhighlightWidget();
static void DisplayList();
static void UpdateSelectedList();
static void PadList();
static void RemoveWidget();
/*static void DumpList();
static void RenumberList();*/

/*****************************************************************************
 *
 *  Translation tables for List. We have 3: The default, a set that 
 *  we attach to each element of the list, and a set we attach to the
 *  BBoard inside of the scrolled window
 *
 ***************************************************************************/
static char defaultTranslations[] = {
    "<EnterWindow>:          enter()\n\
     <LeaveWindow>:          leave()"
};

static char BBXlations[] = {
    "<Enter>:          EnterBB()\n\
     <Leave>:          LeaveBB()"
};

static XtTranslations bbXlations;


static char ElementXlations[] = { 
    "Shift<Btn1Down>:	AppendSelect() \n\
     Shift<Btn1Up>:	AppendUnSelect() \n\
     Shift<KeyDown>Select:   AppendSelect() \n\
     Shift<KeyUp>Select:     AppendUnSelect() \n\
     <Btn1Down>:	ElementSelect() \n\
     <Btn1Up>:		ElementUnSelect() \n\
     <KeyDown>Select:   ElementSelect() \n\
     <KeyUp>Select:     ElementUnSelect() \n\
     Button1<Enter>:	ElementEnter() \n\
     Button1<Leave>:	ElementLeave() \n\
     <Enter>:		ElementEnter() \n\
     <Leave>:		ElementLeave() "

};

static XtTranslations elementXlations;

/*****************************************************************************
 *
 *  Actions Lists
 *
 ***************************************************************************/
static XtActionsRec actionsList[] =
{
  {"enter", (XtActionProc) _XwManagerEnter},
  {"leave", (XtActionProc) _XwManagerLeave},
};

static XtActionsRec BBActionsList[] =
{
  {"EnterBB", (XtActionProc) BBEnter},
  {"LeaveBB", (XtActionProc) BBLeave},
};

static XtActionsRec XwListActions[] =
{
  {"ElementEnter",    (XtActionProc) EnterElement},
  {"ElementLeave",    (XtActionProc) LeaveElement},
  {"ElementEnter",    (XtActionProc) EnterElement},
  {"ElementLeave",    (XtActionProc) LeaveElement},
  {"AppendSelect",    (XtActionProc) ExSelect},
  {"AppendUnSelect",  (XtActionProc) ExUnSelect},
  {"ElementSelect",   (XtActionProc) SelectElement},
  {"ElementUnSelect", (XtActionProc) UnSelectElement},

};

static Arg Args[25];
static Boolean InInit;
static int minusOne = (-1);
static int zero = 0;

/****************************************************************
 *
 * List Resources
 *
 ****************************************************************/

static XtResource ListConstraintResources[] = 
{

   {
      XtNrowPosition, XtCRowPosition, XtRInt, sizeof (int),
      XtOffset (XwListConstraints, row),
      XtRInt, (caddr_t ) &minusOne
   },
   {
      XtNcolumnPosition, XtCColumnPosition, XtRInt, sizeof (int),
      XtOffset (XwListConstraints, column),
      XtRInt, (caddr_t ) &minusOne
   },

};

static XtResource resources[] = 
{

   {
      XtNnumColumns, XtCNumColumns, XtRInt, sizeof (int),
      XtOffset (XwListWidget,list.numcolumns),
      XtRString, "1"
   },
   {
      XtNselectionMethod, XtCSelectionMethod, XtRSelectionMethod, sizeof (int),
      XtOffset (XwListWidget,list.Selection),
      XtRString, "Single"
   },
   {
      XtNselectionStyle, XtCSelectionStyle, XtRSelectionStyle, sizeof (int),
      XtOffset (XwListWidget,list.Style),
      XtRString, "Instant"
   },
   {
      XtNelementHighlight, XtCElementHighlight, XtRElementHighlight, sizeof (int),
      XtOffset (XwListWidget,list.Method),
      XtRString, "Border"
   },
   {
      XtNselectionBias,XtCSelectionBias, XtRSelectionBias, sizeof (int),
      XtOffset (XwListWidget,list.Bias),
      XtRString, "No_Bias"
   },
   {
      XtNcolumnWidth, XtCColumnWidth, XtRInt, sizeof (int),
      XtOffset (XwListWidget,list.ColumnWidth),
      XtRInt, (caddr_t ) &zero
   },
   {
      XtNelementHeight, XtCElementHeight, XtRInt, sizeof (int),
      XtOffset (XwListWidget,list.ElementHeight),
      XtRInt, (caddr_t ) &zero
   },
   {
      XtNselectedElements, XtCSelectedElements, XtRPointer, sizeof (caddr_t),
      XtOffset (XwListWidget,list.SelectedList),
      XtRPointer, NULL
   },
   {
      XtNdestroyMode, XtCDestroyMode, XtRDestroyMode, sizeof (int),
      XtOffset (XwListWidget,list.DestroyMode),
      XtRString, "Shrink_Column"
   },
   {
      XtNnumSelectedElements, XtCNumSelectedElements, XtRInt, sizeof (int),
      XtOffset (XwListWidget,list.NumSelected),
      XtRInt, (caddr_t ) &zero
   },

   {  
      XtNdoubleClick, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwListWidget,list.DoubleClick),
      XtRPointer, (caddr_t) NULL
   },

};



/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

XwListClassRec XwlistClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &XwswindowClassRec,
    /* class_name         */    "List",
    /* widget_size        */    sizeof(XwListRec),
    /* class_initialize   */    ClassInitialize,
    /* class_partinit     */    NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* Init hook	  */    NULL,
    /* realize            */    Realize,
    /* actions		  */	actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    ListDestroy,
    /* resize             */    Resize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    (XtSetValuesFunc) SetValues,
    /* set values hook    */    NULL,
    /* set values almost  */    XtInheritSetValuesAlmost,
    /* get values hook    */    NULL,
    /* accept_focus       */    NULL,
    /* Version            */    XtVersion,
    /* PRIVATE cb list    */    NULL,
    /* TM Table           */    defaultTranslations,
    /* query_geom         */    NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	InsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
				NULL,
  },{
/* Constraint class Init */
    /* Constraint res.   */	ListConstraintResources,
    /* Num. of resources */	XtNumber(ListConstraintResources),
    /* Size of data      */	sizeof(XwListConstraintRec),
    /* INitialize	 */	(XtInitProc) ConstraintInitialize,
    /* Destroy		 */	NULL,
    /* Set Values	 */     (XtSetValuesFunc) ConstraintSetValues,
				NULL,
  },{
/* manager_class fields */
   /* traversal handler   */    XtInheritTraversalProc,
   /* translations        */	NULL,
  },{
/* Scrolled Window class - none */     
     /* mumble */               0
  },{
/* List class - none */     
     /* Class Init */           FALSE
  }	
};

WidgetClass XwlistWidgetClass = (WidgetClass)&XwlistClassRec;

/************************************************************************
 *
 *  KidKilled 
 *
 *  Destroy callback for the list elements.
 *
 ************************************************************************/

 static XtCallbackProc KidKilled(w,closure,call_data)
    Widget w;
    caddr_t  closure, call_data;
{
    XwListWidget 	lw;
    XwManagerWidgetClass superclass;
    int 	i,j;
    XwListConstraintRec *cr;
    cr = (XwListConstraintRec *) w->core.constraints;

    if (XtClass(w) == widgetClass) return;

    lw = (XwListWidget )((w->core.parent)->core.parent)->core.parent;
    superclass = (XwManagerWidgetClass)XwlistrowColWidgetClass;

    if (lw->core.being_destroyed) return;

    RemoveWidget(w,lw);
    if (cr->selected)
        UpdateSelectedList(lw);

    (*superclass->composite_class.change_managed)(lw->list.rcmanager);
}

/************************************************************************
 *									*
 *  RemoveWidget - remove a widget from the child list and reorder it	*
 *   		   according to the destroy mode.			*
 *									*
 ************************************************************************/
static void RemoveWidget(w,lw)
    Widget w;
    XwListWidget lw;
{

    XwListRowColWidget	rcw;
    WidgetList 		rckids;
    int			numkids,row,col,i,j,oldrow,oldcol;
    Widget		z;

    XwListConstraintRec *cr;
    cr = (XwListConstraintRec *) w->core.constraints;

    rcw = lw->list.rcmanager;
    rckids = rcw->composite.children;
    numkids = rcw->composite.num_children;

    row = cr->row;
    col = cr->column;

    switch(lw->list.DestroyMode){

    case XwNO_SHRINK:			/* Create a dummy widget in the place 	*/
					/* of the one that's dead.		*/
	    XtSetArg(Args[0], XtNrowPosition, (XtArgVal) cr->row);
	    XtSetArg(Args[1], XtNcolumnPosition, (XtArgVal) cr->column);
	    XtSetArg(Args[2], XtNborderWidth, (XtArgVal) 0);
	    z = XtCreateManagedWidget("ListDummy", widgetClass,
			       lw, (ArgList)Args, 3);
	    XtRealizeWidget(z);
	    break;

    case XwSHRINK_COLUMN:		/* Shuffle the column. */
                                        /* Find the widget at the same col,
                                           next row (re the dead one)   */
	    row++;
            while (numkids &&
                   !((((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
	                row) &&
	           (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
	                col)))
                { numkids--; rckids++;}
            while (numkids)
	    {
	    	((XwListConstraintRec *)(*rckids)->core.constraints)->row = (row - 1);
		row++;
                while (numkids &&
                   !((((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
	                row) &&
	           (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
	                col)))
                      { numkids--; rckids++;}
	    }
	    lw->list.RowNum[col]--;
            OrderList(lw);
	    break;

    case XwSHRINK_ALL:			/* Shuffle the whole mess.    */
    					/* First, find the right spot */
	    while (numkids &&
		   (((XwListConstraintRec *)(*rckids)->core.constraints)->row < 
		      row))
	    {
	    	rckids++; numkids--;
	    }

	    while (numkids &&
		    (((XwListConstraintRec *)(*rckids)->core.constraints)->row <=
		       row) &&
		    (((XwListConstraintRec *)(*rckids)->core.constraints)->column <
		       col))
	    {
	    	rckids++; numkids--;
	    }

	    if (numkids) 
	    {
		oldrow = row;
		oldcol = col;
		while (numkids--) 
		{
		    while (numkids && !(((XwListConstraintRec *)
			    (*rckids)->core.managed)))
		    { numkids--; rckids++;}
		    row = ((XwListConstraintRec *)(*rckids)->core.constraints)->row;
		    col = ((XwListConstraintRec *)(*rckids)->core.constraints)->column;
		    ((XwListConstraintRec *)(*rckids)->core.constraints)->row = oldrow;
		    ((XwListConstraintRec *)(*rckids)->core.constraints)->column = oldcol;
		    oldrow = row;
		    oldcol = col;
		    rckids++;
	    	}
	    }
	    OrderList(lw);
	    break;
     }
    PadList(lw,FALSE);
}

/*****************************************************************************
 *
 *  ClassInitialize
 *
 ***************************************************************************/

static void ClassInitialize()
{
    XwListWidgetClass myclass;
    XwManagerWidgetClass superclass;


    XwRegisterConverters();
    myclass = (XwListWidgetClass) XwlistWidgetClass;
    superclass = (XwManagerWidgetClass)XwswindowWidgetClass;
    myclass->composite_class.geometry_manager =
       superclass->composite_class.geometry_manager;
    myclass->core_class.resize = superclass->core_class.resize;

    XtAddActions(XwListActions,XtNumber(XwListActions));
    XtAddActions(BBActionsList,XtNumber(BBActionsList));

    bbXlations = XtParseTranslationTable(BBXlations);
    elementXlations = XtParseTranslationTable(ElementXlations);
}


/*****************************************************************************
 *
 *  Initialize
 *
 ***************************************************************************/

static void Initialize(request, new)
    XwListWidget request, new;
{
    Widget 	bw;
    int		i;
    XwListRowColWidgetClass superclass;


/****************
 *
 *  Bounds check the size.
 *
 ****************/

    if (request->core.width==0 || (request->core.height==0))
    {
        if (request->core.width==0)
	    new->core.width = DEFAULT_WIDTH;
	if (request->core.height==0)
	    new->core.height = DEFAULT_HEIGHT;
    }

    new->list.LastElementRow = 0;
    new->list.LastElementColumn = 0;
    new->list.LastHLWidget = (Widget )0;
    new->list.StartWidget = (Widget )0;
    new->list.EndWidget = (Widget )0;
    new->list.Event = 0;
    new->list.NumSelected = 0;
    new->list.AppendInProgress = FALSE;
    new->list.FromPadList = FALSE;
    new->list.DummyDestroy = FALSE;
    new->list.InChangeManaged = FALSE;
    new->list.CMRemove = FALSE;
    new->list.InInit = TRUE;
    new->list.MaxHeight = (int *)XtMalloc(sizeof(int) * new->list.numcolumns);
    new->list.MaxWidth = (int *)XtMalloc(sizeof(int) * new->list.numcolumns);
    new->list.RowNum = (int *)XtMalloc(sizeof(int) * new->list.numcolumns);

    for (i=0;i<request->list.numcolumns;i++)
    {
	new->list.MaxHeight[i] = 0;
	new->list.MaxWidth[i] = 0;
	new->list.RowNum[i] = 0;
    }
    new->list.JumboHeight = 0;
    new->list.JumboWidth = 0;

/****************
 *
 *  Create the widgets.
 *
 ****************/

    XtOverrideTranslations(new->swindow.bboard, bbXlations);

    XtSetArg (Args[0],XtNlayout, (XtArgVal) XwMINIMIZE);
    XtSetArg (Args[1],XtNhSpace, (XtArgVal) 0);
    XtSetArg (Args[2],XtNvSpace, (XtArgVal) 0);
    XtSetArg (Args[3],XtNcolumns, (XtArgVal) new->list.numcolumns);
    XtSetArg (Args[4],XtNborderWidth, 0);

    new->list.rcmanager =
        (XwListRowColWidget) XtCreateWidget("ListRCManager",
					    XwlistrowColWidgetClass,
					    (Widget) new,
					    Args,5);
    XtManageChild(new->list.rcmanager);
/****************
 *
 *  Point the ListRC constraint stuff at my routines and resources.
 *
 ****************/
    superclass = (XwListRowColWidgetClass)XwlistrowColWidgetClass;
    superclass->constraint_class.resources = ((XwListWidgetClass ) 
    					XtClass(new))->constraint_class.resources;
    superclass->constraint_class.num_resources = ((XwListWidgetClass ) 
    					XtClass(new))->constraint_class.num_resources;
    superclass->constraint_class.constraint_size = ((XwListWidgetClass ) 
    					XtClass(new))->constraint_class.constraint_size;
    superclass->constraint_class.set_values = ((XwListWidgetClass ) 
    					XtClass(new))->constraint_class.set_values;

    new->list.InInit = FALSE;    
}

/************************************************************************
 *
 *  ConstraintInitialize
 *
 ************************************************************************/

static XtInitProc ConstraintInitialize (request, new)
Widget request, new;

{
    XwListConstraintRec *constraintRec;
    XwListWidget lw;
    WidgetList	 rckids;
    int 	 numkids;

    lw  = (XwListWidget )new->core.parent;
    constraintRec = (XwListConstraintRec *) new->core.constraints;

    constraintRec->originalWidth = new->core.width;
    constraintRec->originalHeight = new->core.height;
    constraintRec->selected = FALSE;
    constraintRec->highlighted = FALSE;
    constraintRec->managed = FALSE;

    if (XtIsSubclass(request,XwprimitiveWidgetClass))
	constraintRec->fg = ((XwPrimitiveWidget )request)->primitive.foreground;
    else {
	if (!(XtIsSubclass(request,XwswindowWidgetClass)) &&
	    (XtClass(request) != widgetClass) &&
	    !(lw->swindow.InInit) &&
	    !(lw->list.InInit))
	XtWarning("List: Can't invert a non-primitive widget");
	constraintRec->fg = (Pixel) request->core.background_pixel;
    }
    constraintRec->bg = (Pixel )request->core.background_pixel;

/****************
 *
 *  If the row and column are both -1, then assign a default value to it.
 *  If there are dummies in the list, then find the first one and replace
 *  it. Otherwise add it at the end of the list.
 *
 ****************/

    if ((constraintRec->row) < 0 && (constraintRec->column < 0) && 
	!(lw->swindow.InInit) && !(lw->list.InInit))
    {

/*****   Fix this later - don't insert on the last row empty columns ***
	rckids = ((XwListWidget )lw->list.rcmanager)->composite.children;
	numkids = ((XwListWidget )lw->list.rcmanager)->composite.num_children;

	while (numkids)
	{
	    if ((XtClass(*rckids) == widgetClass) &&
	        (XtClass(request) != widgetClass))
	    {
	        constraintRec->row =
	         ((XwListConstraintRec *)(*rckids)->core.constraints)->row;
		constraintRec->column=
		 ((XwListConstraintRec *)(*rckids)->core.constraints)->column;
		return;
	    }
	    rckids++; numkids--;
	}
*****/
	constraintRec->column = lw->list.LastElementColumn;
	constraintRec->row = lw->list.LastElementRow;
	if (++(lw->list.LastElementColumn) >= lw->list.numcolumns)
	{
	    lw->list.LastElementColumn = 0;
	    lw->list.LastElementRow++;
	}
    }
}


/*****************************************************************************
 *
 *  Realize
 *
 ***************************************************************************/
static void Realize(w, p_valueMask, attributes)
    register XwListWidget w;
    Mask *p_valueMask;
    XSetWindowAttributes *attributes;
{
    XwListRowColWidget	rcw;
    XwBulletinWidget	bbw;
    WidgetList 		rckids;
    int			numkids,NewWidth,NewHeight,
			MaxRows,i,j;
    Widget		z;
    XwManagerWidgetClass superclass;
    Mask valueMask = *p_valueMask;

    valueMask |= CWBitGravity;
    if (XtIsSubclass(w,XwmanagerWidgetClass))
     	attributes->bit_gravity = NorthWestGravity;
    else
     	attributes->bit_gravity = ForgetGravity;

    rcw = w->list.rcmanager;
    rckids = rcw->composite.children;

    PadList(w,FALSE);

/****************
 *
 *  Resize the widgets as required.
 *
 ****************/
    for (i = 0; i < w->list.numcolumns;i++)
    {
        rckids = rcw->composite.children;
	rckids += i;
	if (w->list.RowNum[i]) 
	    numkids = w->list.RowNum[i];
	else numkids = 0;

	NewWidth = (w->list.ColumnWidth) ? w->list.ColumnWidth 
					 : w->list.MaxWidth[i];
	NewHeight = (w->list.ElementHeight) ? w->list.ElementHeight
					    : w->list.MaxHeight[i];
	while (numkids--)
	{    
	    if (((Widget) (*rckids))->core.width != NewWidth  ||
	        ((Widget) (*rckids))->core.height != NewHeight)
	    {
		XtResizeWidget(*rckids,NewWidth,NewHeight,
			       ((Widget )*rckids)->core.border_width);
	    }
	    rckids+= w->list.numcolumns;
    	}
	if (w->list.JumboWidth < w->list.MaxWidth[i])
	    w->list.JumboWidth = w->list.MaxWidth[i];
	if (w->list.JumboHeight < w->list.MaxHeight[i])
	    w->list.JumboHeight = w->list.MaxHeight[i];
    }

    if (w->list.JumboHeight)
    {
	XtSetArg (Args[0],XtNgranularity, (XtArgVal) w->list.JumboHeight);
	XtSetValues((Widget ) w,Args,1);        
    }

    XtCreateWindow(w, InputOutput, (Visual *)CopyFromParent,
	valueMask, attributes);

    superclass = (XwManagerWidgetClass)XwswindowWidgetClass;
    (*superclass->core_class.realize)(w, p_valueMask, attributes);

/******
    superclass = (XwManagerWidgetClass)XwbulletinWidgetClass;
    (*superclass->core_class.realize)(w->swindow.bboard, p_valueMask, attributes);

    superclass = (XwManagerWidgetClass)XwlistrowColWidgetClass;
    (*superclass->core_class.realize)((w->list.rcmanager),p_valueMask, attributes);;
*******/
/****************
 *
 * The following calls have to be here to insure proper geometry 
 * request communication among all these managers when the kids have
 * been inserted before realization.
 *
 ****************/

    superclass = (XwManagerWidgetClass )XtClass((Widget )
    				        (XwListWidget)w->swindow.bboard);
    (*superclass->core_class.realize)(w, p_valueMask, attributes);

    superclass = (XwManagerWidgetClass)XwlistrowColWidgetClass;
    (*superclass->composite_class.change_managed)(w->list.rcmanager);

    superclass = (XwManagerWidgetClass)XwswindowWidgetClass;
    (*superclass->composite_class.change_managed)(w);

    _XwRegisterName(w);

 } /* Realize */


/************************************************************************
 *
 *  InsertChild
 *
 ************************************************************************/
static void InsertChild(w)
    Widget	w;
{
    int _XW_NARGS = 0;
    ArgList	args = NULL;
    Cardinal  *p_num_args = (Cardinal *) &_XW_NARGS;

    XwManagerWidgetClass superclass;
    XwListConstraintRec *constraintRec;
    XEvent 		DummyEvent;

    XwListWidget	mom;
    WidgetList		rckids,wp;
    XtTranslations 	xlations;
    Widget 		x,z;
    int			numkids,i,j,k,NewWidth,NewHeight;

/****************
 *
 *  Funky stuff here. If this widget is one that I created, then insert
 *  it and manage it. If it's not mine, reparent it and make it a child
 *  of the rows/columns manager, and attach some translations to it.
 *
 ****************/
    mom = (XwListWidget )w->core.parent;
    while (!XtIsSubclass(mom,XwlistWidgetClass))
        mom = (XwListWidget )mom->core.parent;

    superclass = (XwManagerWidgetClass)XwswindowWidgetClass;
    
    if ((mom->swindow.InInit) ||
	(mom->list.InInit))
	    
    {
	(*superclass->composite_class.insert_child)(w, args, p_num_args);
	XtManageChild(w);
    }
    else
    {
	constraintRec = (XwListConstraintRec *) w->core.constraints;
	i = constraintRec->column;
	j = constraintRec->row;

	if (i >= mom->list.numcolumns)
	{
	    XtWarning("List: Invalid column - element renumbered");
            j = 0;
	    for (i = 0; i < mom->list.numcolumns;i++)
	    j = ( mom->list.RowNum[i] > j) ? mom->list.RowNum[i] : j;

	    constraintRec->column = 0;
	    i = constraintRec->column;
	    constraintRec->row = j + 1;
    	    j = constraintRec->row;
	}

/****************
 *
 *  Add a destroy callback on all real elements.
 *
 ****************/
	if (XtClass(w) != widgetClass)
	{
	    XtRemoveCallback(w,XtNdestroyCallback,KidKilled,NULL);
	    XtAddCallback(w,XtNdestroyCallback,KidKilled,NULL);
	}

	mom->list.RowNum[i] = (mom->list.RowNum[i] > (j + 1)) ? mom->list.RowNum[i] 
							: (j + 1);
	superclass = (XwManagerWidgetClass )XtClass((Widget )(XwListWidget)mom->list.rcmanager);

	w->core.parent = (Widget )mom->list.rcmanager;
	if (w->core.width > mom->list.MaxWidth[i])
	    mom->list.MaxWidth[i] = w->core.width;
	if (w->core.height > mom->list.MaxHeight[i])
	    mom->list.MaxHeight[i] = w->core.height;


/****************
 *
 *  Now reorder the list so it's in the order:
 *  c0,r0; c1,r0...cn,r0;c0,r1; c1,r1...cn,r1;c0,rn; c1,rn...cn,rn;
 *
 ****************/

	rckids = ((XwListWidget )mom->list.rcmanager)->composite.children;
	numkids = ((XwListWidget )mom->list.rcmanager)->composite.num_children;

	if (!numkids)			/* If the first child, insert it */
	{	
    	    for (k=0; k < mom->list.numcolumns; k++)
	    {
	    	if (!mom->list.MaxWidth[k])
		    mom->list.MaxWidth[k] = mom->list.MaxWidth[i];
	    	if (!mom->list.MaxHeight[k])
		    mom->list.MaxHeight[k] = mom->list.MaxHeight[i];
    	    }
	    (*superclass->composite_class.insert_child)(w, args, p_num_args);

	    if (!XtIsRealized(mom->list.rcmanager))
    	    {
                XtSetArg (Args[0],XtNlayout, (XtArgVal) XwMINIMIZE);
                XtSetValues( mom->list.rcmanager,Args,1);        
            }

	    if (XtIsRealized(mom))
	    {
	  	NewWidth = (mom->list.ColumnWidth) ? mom->list.ColumnWidth 
					 : mom->list.MaxWidth[i];
	    	NewHeight = (mom->list.ElementHeight) ? mom->list.ElementHeight
					    : mom->list.MaxHeight[i];
	    	XtResizeWidget(w,NewWidth,NewHeight,w->core.border_width);
	    }

	    XtOverrideTranslations(w, elementXlations);

	    if (XtIsRealized(mom))
    	    {
	        if (!(mom->list.FromPadList))
		   PadList(mom,FALSE);		/* If realized, let the RC widget have	*/
						/* a chance to re-layout.		*/
		(*superclass->composite_class.change_managed)(mom->list.rcmanager);
	    }
	    return;
	}
/****************
 *
 *  Look for the widget's spot in the child list.
 *
 ****************/
	while (numkids &&
		(((XwListConstraintRec *)(*rckids)->core.constraints)->row < 
		 constraintRec->row))
	{
	    rckids++; numkids--;
	}

	while (numkids &&
		(((XwListConstraintRec *)(*rckids)->core.constraints)->row <=
		       constraintRec->row) &&
		(((XwListConstraintRec *)(*rckids)->core.constraints)->column <
			constraintRec->column))
	{
	    rckids++; numkids--;
	}
/****************
 *
 * Found it. Now,look to see if we are trying to insert a dummy on top of 
 * a real. If so, don't do it. Else insert it.
 *
 ****************/
	    if (numkids &&
	        (((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
		       constraintRec->row) &&
	       (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
		       constraintRec->column) &&
	       (XtClass(w) != widgetClass) &&
	       (XtClass(*rckids) != widgetClass) &&
	       ((*rckids)->core.managed))
 	    {
		XtWarning("List: Inserting an element at an existing row,column");
	    }
	    if (numkids &&
	        (((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
		       constraintRec->row) &&
	       (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
		       constraintRec->column) &&
	       (XtClass(w) == widgetClass) &&
	       (XtClass(*rckids) != widgetClass) &&
	       ((*rckids)->core.managed))
 	    {
/* Don't insert a dummy on a real - must be a better way to do this! */
	    }
            else
					/* If it's a replacement for a dummy, 	*/
		      			/* delete the dummy from the list.    	*/
	    {
		if (numkids &&
		   (((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
		       constraintRec->row) &&
		   (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
		       constraintRec->column) &&
		     (XtClass(*rckids) == widgetClass) &&
		     (XtClass(w) != widgetClass))
		{
/****************
 *
 *  The R2 toolkit uses a hokey 2-phase destroy, triggered by XtNextEvent.
 *  So, since we need the entire process to complete before continuing,
 *  but we don't want to block forever, we construct a dummy event, stuff
 *  it on the queue and pop it off. If it's a normal event , we push it back.
 *
 ****************/
		     mom->list.DummyDestroy = TRUE;
		     z = *rckids;
		     XtDestroyWidget(z);
		     DummyEvent.type = (-1);	
		     XPutBackEvent(XtDisplay(mom),&DummyEvent);
		     DummyEvent.type = 0;	
		     XtNextEvent(&DummyEvent);
		     if (DummyEvent.type != (-1))
			 XPutBackEvent(XtDisplay(mom),&DummyEvent);
		     mom->list.DummyDestroy = FALSE;
		}

/****************
 *
 *  OK, the kid is real - insert it into the RC manager, 
 *  reparent it and size it.
 *
 ****************/
		(*superclass->composite_class.insert_child)(w, args, p_num_args);
	        w->core.parent = (Widget )mom->list.rcmanager;

		if (XtIsRealized(mom))
		{
		  	NewWidth = (mom->list.ColumnWidth) ? mom->list.ColumnWidth 
						 : mom->list.MaxWidth[i];
		    	NewHeight = (mom->list.ElementHeight) ? mom->list.ElementHeight
						    : mom->list.MaxHeight[i];
		    	XtResizeWidget(w,NewWidth,NewHeight,w->core.border_width);
		}

		XtOverrideTranslations(w, elementXlations);

/****************
 *
 *  Scan the list again for the right spot to insert the child.
 *
 ****************/
		rckids = ((XwListWidget )mom->list.rcmanager)->composite.children;
		numkids = ((XwListWidget )mom->list.rcmanager)->composite.num_children;

		while (numkids &&
                       (((XwListConstraintRec *)(*rckids)->
			     core.constraints)->row <  constraintRec->row))
		{rckids++; numkids--;}

	    	while (numkids &&
		      (((XwListConstraintRec *)(*rckids)->core.constraints)
					->row <= constraintRec->row) &&
		      (((XwListConstraintRec *)(*rckids)->core.constraints)
					->column < constraintRec->column))
		 {rckids++; numkids--;}
/****************
 *
 *  Found the spot - insert it into the list, and shuffle the rest of the
 *  list down by one.
 *
 ****************/
		 x = *rckids;
		 wp = rckids;
		 *wp = w;

		 while (--numkids > 0)
		 {
		     wp++;
		     z = *wp;
		     *wp = x;
		     x = z;
		 }
	    }
	}					/* end else (!InInit) */

    if (XtIsRealized(mom))
    {
        if (!(mom->list.FromPadList))
	   PadList(mom,FALSE);		/* If realized, let the RC widget have	*/
					/* a chance to re-layout.		*/
	(*superclass->composite_class.change_managed)(mom->list.rcmanager);
    }
}

/**************************************************************************
 *
 *  ListDestroy - free up my memory
 *
 ************************************************************************/
static void ListDestroy (lw)
  XwListWidget lw;
{
    XtFree(lw->list.MaxHeight);
    XtFree(lw->list.MaxWidth);
    XtFree(lw->list.RowNum);
}


/*****************************************************************************
 *
 *  ReSize
 *
 ***************************************************************************/

static void Resize(lw)
    XwListWidget	lw;
{
/**** Currently inherited from SWINDOW ****/    
} /* Resize */



/************************************************************************
 *
 *  GeometryManager
 *
 ************************************************************************/

static XtGeometryResult GeometryManager (w, request, reply)
Widget w;
XtWidgetGeometry * request;
XtWidgetGeometry * reply;

{
/**** Currently inherited from SWINDOW ****/    
}


/*****************************************************************************
 *
 *  ChangeManaged
 *
 ***************************************************************************/
static void ChangeManaged(lw)
    XwListWidget lw;
{
    XwManagerWidgetClass superclass;
/****
    if (lw->list.DummyDestroy || lw->list.FromPadList) return;
    lw->list.InChangeManaged = TRUE;
    RenumberList(lw);
    lw->list.InChangeManaged = FALSE;
*****/
    superclass = (XwManagerWidgetClass)XwswindowWidgetClass;
    (*superclass->composite_class.change_managed)(lw);

}
     


/*****************************************************************************
 *
 *  SetValues
 *
 ***************************************************************************/

static XtSetValuesFunc SetValues (current, request, new)
    XwListWidget current, request, new;
{
    Boolean     flag = FALSE;

    if ((request->list.Selection != XwSINGLE) &&
        (request->list.Selection != XwMULTIPLE))
    {
        XtWarning("List: Bad Selection Method");
        new->list.Selection = current->list.Selection;
    }

    if ((request->list.Style != XwINSTANT) &&
        (request->list.Style != XwSTICKY))
    {
        XtWarning("List: Bad Selection Style");
        new->list.Style = current->list.Style;
    }

    if ((request->list.Method != XwBORDER) &&
        (request->list.Method != XwINVERT))
    {
        XtWarning("List: Bad HighLight Method");
        new->list.Method = current->list.Method;
    }

    if ((request->list.Bias != XwNO_BIAS) &&
        (request->list.Bias != XwCOL_BIAS) &&
        (request->list.Bias != XwROW_BIAS)) 
    {
        XtWarning("List: Bad Selection Bias");
        new->list.Bias = current->list.Bias;
    }

    if ((request->list.DestroyMode != XwNO_SHRINK) &&
	(request->list.DestroyMode != XwSHRINK_COLUMN) &&
	(request->list.DestroyMode != XwSHRINK_ALL))
    {
        XtWarning("List: Bad Destroy Mode");
        new->list.DestroyMode = current->list.DestroyMode;
    }

    if (request->list.SelectedList != current->list.SelectedList) 
    {
        XtWarning("List: Cannot Set Selection List from SetValues()");
        new->list.SelectedList = current->list.SelectedList;
    }
    if (request->list.NumSelected != current->list.NumSelected) 
    {
        XtWarning("List: Cannot Set Selection List Count from SetValues()");
        new->list.NumSelected = current->list.NumSelected;
    }
    if (request->list.numcolumns != current->list.numcolumns) 
    {
        XtWarning("List: Cannot change number of columns");
        new->list.numcolumns = current->list.numcolumns;
    }

    return ((XtSetValuesFunc) flag);
 }

/*****************************************************************************
 *
 *  Constraint SetValues
 *
 ***************************************************************************/

static XtSetValuesFunc ConstraintSetValues (current, request, new)
    Widget current, request, new;
{
    XwListConstraintRec *constraintRec,*OldCR;
    XwListWidget	lw;
    XwManagerWidgetClass superclass;
    WidgetList		rckids,wp;
    XEvent 		DummyEvent;
    int			numkids,i;
    Boolean		Clean;
    superclass = (XwManagerWidgetClass)XwlistrowColWidgetClass;

    lw = (XwListWidget )((current->core.parent)->core.parent)->core.parent;
    constraintRec = (XwListConstraintRec *) request->core.constraints;
    OldCR = (XwListConstraintRec *) current->core.constraints;

    if (((constraintRec->column != OldCR->column) ||
        (constraintRec->row != OldCR->row)) &&
	!(lw->swindow.InInit) &&
	!(lw->list.InInit))
    {
        if (constraintRec->column >= lw->list.numcolumns)
	{
	    XtWarning("List: Invalid column number");
	    return ((XtSetValuesFunc) FALSE);
	}
        OldCR->column = constraintRec->column;
        OldCR->row = constraintRec->row;

/****	PadList(lw,FALSE);
	XSync(XtDisplay(lw),0);
*****/
	OrderList(lw);

/****************
 *
 *  See if someone has changed a real widget to the same location
 *  as a dummy. If so, delete the dummy.
 *
 ****************/

	rckids = ((XwListWidget )lw->list.rcmanager)->composite.children;
	numkids = ((XwListWidget )lw->list.rcmanager)->composite.num_children;
	while (numkids > 1)
	{
	    if ((((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
                	 OldCR->row) &&
	        (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
	        	 OldCR->column) &&
	        (XtClass(*rckids) != widgetClass) &&
	        (*rckids != current) &&
   		(XtIsManaged(*rckids)))
    	    {
	        XtWarning("List: Conflicting Row, Column locations");
	    }

	    if ((((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
                	 OldCR->row) &&
	      	(((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
	        	 OldCR->column) &&
	        (XtClass(*rckids) == widgetClass))
	    {
		lw->list.DummyDestroy = TRUE;
		XtDestroyWidget(*rckids);
		DummyEvent.type = (-1);	
		XPutBackEvent(XtDisplay(lw),&DummyEvent);
		DummyEvent.type = 0;	
		XtNextEvent(&DummyEvent);
		if (DummyEvent.type != (-1))
		    XPutBackEvent(XtDisplay(lw),&DummyEvent);
		lw->list.DummyDestroy = FALSE;
	        (*superclass->composite_class.change_managed)(lw->list.rcmanager);
	        return ((XtSetValuesFunc) TRUE);
	     }
	     rckids++; numkids--;
	  }
    }
    else
	return ((XtSetValuesFunc) FALSE);
 }


/***************************************************************************
 *
 *  PadList
 *
 *  Create dummy widgets to pad out the RC manager so it looks "right".
 *
 ***************************************************************************/
static void PadList(w,CheckManaged)
    register XwListWidget w;
    Boolean  CheckManaged;
{
    int			MaxRows,i,j,k,numkids;
    Boolean 		Create;
    Widget		z;
    WidgetList		rckids;

    MaxRows = 0;

    for (i = 0; i < w->list.numcolumns;i++)
	MaxRows = ( w->list.RowNum[i] > MaxRows) ? w->list.RowNum[i]
					         : MaxRows;

    w->list.FromPadList = TRUE;

    if (w->list.rcmanager->composite.num_children)
    for (i = 0; i < w->list.numcolumns;i++)
	for (j = 0; j < MaxRows; j++)
	{
/****************
 * 
 *   First scan the list to see if there's already a widget there.
 *
 ****************/
	    rckids = ((XwListWidget )w->list.rcmanager)->composite.children;
	    numkids = ((XwListWidget )w->list.rcmanager)->composite.num_children;
            while (numkids &&
                   !((((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
	                j) &&
	           (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
	                i)))
                { numkids--; rckids++;}
/****************
 *
 *  See if we care whether the one we found is managed. If we do, look through
 *  each element with the same row,column loc, and if there is no mamaged
 *  one, create a dummy anyway.
 *
 ****************/
   	    if (numkids && CheckManaged)
	    {
		Create = TRUE;
                while (numkids &&
                       (((XwListConstraintRec *)(*rckids)->core.constraints)->row ==
	                   j) &&
	               (((XwListConstraintRec *)(*rckids)->core.constraints)->column ==
	                   i))
                { 
		    if (XtIsManaged(*rckids)) Create = FALSE;
                    numkids--; 
		    rckids++;
		}		
		numkids = !Create;
	    }
/****************
 *
 *  If no widget, create one.
 *
 ****************/
	    if (!numkids)
	    {
	    	XtSetArg(Args[0], XtNrowPosition, (XtArgVal) j);
	    	XtSetArg(Args[1], XtNcolumnPosition, (XtArgVal) i);
	    	XtSetArg(Args[2], XtNborderWidth, (XtArgVal) 0);
	    	XtSetArg(Args[3], XtNwidth, (XtArgVal) (w->list.MaxWidth[i]));
	        XtSetArg(Args[4], XtNheight, (XtArgVal) (w->list.MaxHeight[i]));
	        z = XtCreateManagedWidget("ListDummy", widgetClass,
			       w, (ArgList)Args, 5);
		if (XtIsRealized (w)) XtRealizeWidget(z);
            }
	}
    w->list.FromPadList = FALSE;
}

/************************************************************************
 *                                                                      *
 *  OrderList - do a simple bubblesort on the element list.             *
 *              ("Why a *bubble* sort?" Because the list should never   *
 *                get so out of whack that it will matter.)             *
 *                                                                      *
 ************************************************************************/
static void OrderList(mom)
    XwListWidget	mom;
{
    XwListConstraintRec *constraintRec;
    WidgetList		wp1,wp2;
    Widget 		x,z;
    Boolean             Percolated;
    int			numkids,i,j,k;

    wp1 = ((XwListWidget )mom->list.rcmanager)->composite.children;
    wp2 = wp1; wp2++;
    numkids = ((XwListWidget )mom->list.rcmanager)->composite.num_children;
    Percolated = FALSE;
/****************
 *
 *  Look for BO (bad order) widgets
 *
 ****************/

    while (numkids > 1)
    {
        if(((((XwListConstraintRec *)(*wp2)->core.constraints)->row <=
            ((XwListConstraintRec *)(*wp1)->core.constraints)->row) &&
           (((XwListConstraintRec *)(*wp2)->core.constraints)->column < 
            ((XwListConstraintRec *)(*wp1)->core.constraints)->column) &&
            (numkids > 1)) ||
	   ((((XwListConstraintRec *)(*wp2)->core.constraints)->row <
            ((XwListConstraintRec *)(*wp1)->core.constraints)->row) &&
            (numkids > 1)))
	{
                                                /* Found one - unlink it */
            Percolated = TRUE;
            z = *wp2;
            wp2++;
            wp1++;
            while (--numkids > 0)
                *wp1++ = *wp2++;
            *wp1 = z;                           /* Add the bad one at the tail */
            constraintRec = (XwListConstraintRec *) z->core.constraints;
            wp1 = ((XwListWidget )mom->list.rcmanager)->composite.children;
            numkids = ((XwListWidget )mom->list.rcmanager)->composite.num_children;

      	    while (numkids &&
		(((XwListConstraintRec *)(*wp1)->core.constraints)->row < 
		 constraintRec->row))
            {
	        wp1++; numkids--;
	    }
	    while (numkids &&
		  (((XwListConstraintRec *)(*wp1)->core.constraints)->row <=
		       constraintRec->row) &&
		  (((XwListConstraintRec *)(*wp1)->core.constraints)->column <
			constraintRec->column))
	    {
	        wp1++; numkids--;
	    }
/****************
 *
 *  Found the spot - insert it into the list, and shuffle the rest of the
 *  list down by one.
 ****************/

	    x = *wp1;
	    wp2 = wp1;
            *wp2 = z;

            while (--numkids > 0)
	    {
	        wp2++;
	        z = *wp2;
	        *wp2 = x;
	        x = z;
	     }

        }                                       /* Fi */
        else
        {
            wp1++;wp2++; numkids--;
	}
    }                                           /* Elihw */

    if (Percolated) OrderList(mom);             /* Do it till we've clubbed */

}


/************************************************************************
 *
 * Element Enter
 *
 ************************************************************************/
static void EnterElement(w,event,params,num_params)
    Widget w;
    XEnterWindowEvent *event;
    char   *params;
    int    num_params;
{
    XwListConstraintRec *StartCR,*EndCR,*LastCR,*CurCR,*cr;
    XwListWidget lw;
    XwListRowColWidget	rcw;
    WidgetList	 rckids;
    int         numkids;
    
    CurCR =  (XwListConstraintRec *) w->core.constraints;

    lw = (XwListWidget) w->core.parent->core.parent->core.parent;

    if (lw->list.LastHLWidget == 0) lw->list.LastHLWidget = w;
    if (lw->list.StartWidget == 0) lw->list.StartWidget = w;
    if (lw->list.EndWidget == 0) lw->list.EndWidget = w;

    LastCR =  (XwListConstraintRec *)(lw->list.LastHLWidget)->core.constraints;
    StartCR =  (XwListConstraintRec *)(lw->list.StartWidget)->core.constraints;
    EndCR =  (XwListConstraintRec *)(lw->list.EndWidget)->core.constraints;

    lw->list.LastEnterX = event->x;
    lw->list.LastEnterY = event->y;
    rcw = lw->list.rcmanager;
    rckids = rcw->composite.children;
    numkids = rcw->composite.num_children;

    if ((lw->list.Event) & BUTTONDOWN)
    {
        if (lw->list.Selection == XwSINGLE)
	{
	    UnhighlightWidget(lw->list.LastHLWidget,lw);
	    ((XwListConstraintRec *)((Widget )lw->list.LastHLWidget)->
	   	core.constraints)->selected = FALSE;
	    HighlightWidget(w,lw);
	    ((XwListConstraintRec *)(w->core.constraints))->selected = TRUE;
	}
	else
	{
            if (lw->list.Bias == XwROW_BIAS)
            {
                if ((CurCR->row > EndCR->row) ||
                    ((CurCR->row == EndCR->row) && 
                     (CurCR->column > EndCR->column))) 
                        lw->list.EndWidget = w;
                else
                    if ((CurCR->row < StartCR->row) ||
                        ((CurCR->row == StartCR->row) && 
                         (CurCR->column < StartCR->column))) 
                            lw->list.StartWidget = w;
                    else
                        if (lw->list.LastHLWidget == lw->list.StartWidget)
                            lw->list.StartWidget = w;
                        else
                            lw->list.EndWidget = w;
		StartCR =  (XwListConstraintRec *)(lw->list.StartWidget)->
				core.constraints;
		EndCR =  (XwListConstraintRec *)(lw->list.EndWidget)->
				core.constraints;

		while (numkids--)
		{
        	    cr = (XwListConstraintRec *)((Widget )*rckids)
			     ->core.constraints;

		    if (((cr->row > StartCR->row)     &&
			 (cr->row < EndCR->row))	  ||

			((cr->row == StartCR->row)    &&
		         (StartCR->row != EndCR->row) &&
			 (cr->column >= StartCR->column)) ||

			((cr->row == EndCR->row)      &&
		         (StartCR->row != EndCR->row) &&
			 (cr->column <= EndCR->column))	  ||

		        ((StartCR->row == EndCR->row) &&
			 (cr->row == StartCR->row) &&
			 (cr->column >= StartCR->column) &&
			 (cr->column <= EndCR->column)))
		    {
		        cr->selected = TRUE;
    			HighlightWidget(*rckids,lw);
		    }
		    else if (!lw->list.AppendInProgress)
		    {
		        cr->selected = FALSE;
    			UnhighlightWidget(*rckids,lw);
		    }
		    rckids++;
		}
            }

            if (lw->list.Bias == XwCOL_BIAS)
            {
                if ((CurCR->column > EndCR->column) ||
                    ((CurCR->column == EndCR->column) && 
                     (CurCR->row > EndCR->row))) 
                        lw->list.EndWidget = w;
                else
                    if ((CurCR->column < StartCR->column) ||
                        ((CurCR->column == StartCR->column) && 
                         (CurCR->row < StartCR->row))) 
                            lw->list.StartWidget = w;
                    else
                        if (lw->list.LastHLWidget == lw->list.StartWidget)
                            lw->list.StartWidget = w;
                        else
                            lw->list.EndWidget = w;
		StartCR =  (XwListConstraintRec *)(lw->list.StartWidget)->
				core.constraints;
		EndCR =  (XwListConstraintRec *)(lw->list.EndWidget)->
				core.constraints;

		while (numkids--)
		{
        	    cr = (XwListConstraintRec *)((Widget )*rckids)
			     ->core.constraints;

		    if (((cr->column > StartCR->column)     &&
			 (cr->column < EndCR->column))	  ||

			((cr->column == StartCR->column)    &&
		         (StartCR->column != EndCR->column) &&
			 (cr->row >= StartCR->row)) ||

			((cr->column == EndCR->column)      &&
		         (StartCR->column != EndCR->column) &&
			 (cr->row <= EndCR->row))	  ||

		        ((StartCR->column == EndCR->column) &&
			 (cr->column == StartCR->column) &&
			 (cr->row >= StartCR->row) &&
			 (cr->row <= EndCR->row)))

		    {
		        cr->selected = TRUE;
    			HighlightWidget(*rckids,lw);
		    }
		    else if (!lw->list.AppendInProgress)
		    {
		        cr->selected = FALSE;
    			UnhighlightWidget(*rckids,lw);
		    }
		    rckids++;
		}
            }

            if (lw->list.Bias == XwNO_BIAS)
            {
		HighlightWidget(w,lw);
		CurCR->selected = TRUE;
            }
	}
	lw->list.LastHLWidget = w;	
        UpdateSelectedList(lw);
    }
}

/************************************************************************
 *
 * Element Leave
 *
 ************************************************************************/
static void LeaveElement(w,event,params,num_params)
    Widget w;
    XLeaveWindowEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;
    XwListConstraintRec *constraintRec;

    constraintRec = (XwListConstraintRec *) w->core.constraints;

    lw = (XwListWidget)w->core.parent->core.parent->core.parent;

    lw->list.LastLeaveX = event->x;
    lw->list.LastLeaveY = event->y;

    if ((lw->list.Event) & BUTTONDOWN)
    {
    }
}

/************************************************************************
 *
 * Element Select
 *
 ************************************************************************/
static void SelectElement(w,event,params,num_params)
    Widget w;
    XEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;
    XwListConstraintRec *constraintRec;
    XwListRowColWidget	rcw;
    WidgetList	 rckids;
    int         numkids;



    lw = (XwListWidget)w->core.parent->core.parent->core.parent;

    if (lw->list.Style == XwSTICKY && (!lw->list.AppendInProgress))
    {
	rcw = lw->list.rcmanager;
	rckids = rcw->composite.children;
	numkids = rcw->composite.num_children;
	while (numkids--)
	{
	    constraintRec = (XwListConstraintRec *) (*rckids)->core.constraints;	
	    if (constraintRec->selected)
	    {
		UnhighlightWidget(*rckids,lw);
		constraintRec->selected = FALSE;
	    }
	    rckids++;
        }
    }	
    constraintRec = (XwListConstraintRec *) w->core.constraints;
    XUngrabPointer(XtDisplay(w),CurrentTime);    
    XGrabPointer(XtDisplay(w),XtWindow(w),TRUE,
		 (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | 
		  LeaveWindowMask | Button1MotionMask | PointerMotionMask),
		 GrabModeAsync,GrabModeAsync,None,NULL,CurrentTime);    

    HighlightWidget(w,lw);
    constraintRec->selected = TRUE;
    lw->list.Event |= BUTTONDOWN;
    lw->list.LastHLWidget = w;
    lw->list.StartWidget = w;
    lw->list.EndWidget = w;
    UpdateSelectedList(lw);
}

/************************************************************************
 *
 * Element UnSelect
 *
 ************************************************************************/
static void UnSelectElement(w,event,params,num_params)
    Widget w;
    XEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;
    XwListConstraintRec *constraintRec;
    XwListRowColWidget	rcw;
    WidgetList 		rckids;
    int			i;

    lw = (XwListWidget)w->core.parent->core.parent->core.parent;

    rcw = lw->list.rcmanager;
    rckids = rcw->composite.children;
    i = rcw->composite.num_children;

    XUngrabPointer(XtDisplay(w),CurrentTime);    

    while (i--)
    {
	constraintRec = (XwListConstraintRec *) (*rckids)->core.constraints;	
	if (constraintRec->selected)
	{
	    if (XtClass(*rckids) != widgetClass)
		XtCallCallbacks(*rckids,XtNselect,NULL);
	    if (lw->list.Style == XwINSTANT)
	    {
	    	UnhighlightWidget(*rckids,lw);
	    	constraintRec->selected = FALSE;
	    }
	}
	rckids++;
    }		
    lw->list.Event &= ~BUTTONDOWN;
}
/**********
 ************************************************************************
 *
 * Element DoubleClick
 *
 ************************************************************************
static void BrowseElement(w,event,params,num_params)
    Widget w;
    XEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;
    XwListConstraintRec *constraintRec;
    int			i;
    lw = (XwListWidget)w->core.parent->core.parent->core.parent;

    XtCallCallbacks(lw,XtNdoubleClick,w);
    lw->list.Event &= ~BUTTONDOWN;
}
*************/
/************************************************************************
 *
 * Append Select
 *
 ************************************************************************/
static void ExSelect(w,event,params,num_params)
    Widget w;
    XEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;

    lw = (XwListWidget)w->core.parent->core.parent->core.parent;
    lw->list.AppendInProgress = TRUE;
    SelectElement(w,event,params,num_params);

}

/************************************************************************
 *
 * Append UnSelect
 *
 ************************************************************************/
static void ExUnSelect(w,event,params,num_params)
    Widget w;
    XEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;

    lw = (XwListWidget)w->core.parent->core.parent->core.parent;


    lw->list.AppendInProgress = FALSE;
    UnSelectElement(w,event,params,num_params);

}

/************************************************************************
 *
 * BBoard Enter
 *
 ************************************************************************/
static void BBEnter(w,event,params,num_params)
    Widget w;
    XEnterWindowEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;
    int lr = -1;
    int ud = -1;
    int BBWidth, BBHeight;

    lw = (XwListWidget)w->core.parent;

    BBWidth = ((Widget ) lw->swindow.bboard)->core.width;
    BBHeight = ((Widget )lw->swindow.bboard)->core.height;

    lw->list.BBEnterX = event->x;
    lw->list.BBEnterY = event->y;

    if (event->x <= (int )(BBWidth >> 2)) lr = 0;
    else if (event->x >= (int )(BBWidth - (BBWidth >> 2))) lr = 1;
    if (event->y <= (int )(BBHeight >> 2)) ud = 0;
    else if (event->y >= (int )(BBHeight - (BBHeight >> 2))) ud = 1;
}

/************************************************************************
 *
 * BBoard Leave
 *
 ************************************************************************/
static void BBLeave(w,event,params,num_params)
    Widget w;
    XLeaveWindowEvent *event;
    char   *params;
    int    num_params;
{
    XwListWidget lw;
    XwListRowColWidget rw;
    int lr = -1;
    int ud = -1;
    int BBWidth, BBHeight;
    int	lump,newy,newx,ht;

    lw = (XwListWidget)w->core.parent;

    ht = 0;

    BBWidth = ((Widget ) lw->swindow.bboard)->core.width;
    BBHeight = ((Widget )lw->swindow.bboard)->core.height;

    lw->list.BBLeaveX = event->x;
    lw->list.BBLeaveY = event->y;

    if (event->x <= (lw->swindow.bboard->core.x)) lr = 0;
    else if (event->x >= BBWidth)  lr = 1;

    if (event->y <= lw->swindow.bboard->core.y) ud = 0;
    else if (event->y >= BBHeight ) ud = 1;

    if ((lw->list.Event) & BUTTONDOWN)
    {
	lump = ((XwScrollBarWidget )lw->swindow.vScrollBar)->
	       scrollbar.granularity;
	rw = (XwListRowColWidget )lw->swindow.bbChild;
	if (ud == 1)			/* Lower Edge */
	{
	    if ((abs(rw->core.y - lump) + BBHeight) > rw->core.height)
		newy= (-(rw->core.height - BBHeight));
	    else newy = rw->core.y - lump;
 	    if ((newy <= 0) && (newy != rw->core.y))
	    {
		XtMoveWidget(rw,rw->core.x,newy);
		XWarpPointer(XtDisplay(lw),None,XtWindow(lw),0,0,0,0,
			     event->x,(BBHeight- (lump >> 1)));
		XtSetArg (Args[0],XtNsliderOrigin, (XtArgVal) abs(newy));
		XtSetValues((Widget )lw->swindow.vScrollBar,Args,1);

	    }
	}
	else if (ud == 0)		/* Upper edge */
	{
	    if ((rw->core.y + lump) > ht)
		newy= ht;
	    else newy = rw->core.y + lump;
	    if ((newy <= 0) && (newy != rw->core.y))
	    {
		XtMoveWidget(rw,rw->core.x,newy);
		XWarpPointer(XtDisplay(lw),None,XtWindow(lw),0,0,0,0,
			     event->x,(lump >> 1));
		XtSetArg (Args[0],XtNsliderOrigin, (XtArgVal) abs(newy));
		XtSetValues((Widget )lw->swindow.vScrollBar,Args,1);

	    }
	}

	if (lr == 1)			/* Right Edge */
	{
	    if ((abs(rw->core.x - lump) + BBWidth) > rw->core.width)
		newx = (-(rw->core.width - BBWidth));
	    else newx = rw->core.x - lump;
 	    if ((newx <= 0) && (newx != rw->core.x))
	    {
		XtMoveWidget(rw,newx,rw->core.y);
		XWarpPointer(XtDisplay(lw),None,XtWindow(lw),0,0,0,0,
			     (BBWidth - (lump >> 1)),event->y);
		XtSetArg (Args[0],XtNsliderOrigin, (XtArgVal) abs(newx));
		XtSetValues((Widget )lw->swindow.hScrollBar,Args,1);

	    }
	}
	else if (lr == 0)		/* Left edge */
	{
	    if ((rw->core.x + lump) > ht)
		newx = ht;
	    else newx = rw->core.x + lump;
	    if ((newx <= 0) && (newx != rw->core.x))
	    {
		XtMoveWidget(rw,newx,rw->core.y);
		XWarpPointer(XtDisplay(lw),None,XtWindow(lw),0,0,0,0,
			     (lump >> 1),event->y);
		XtSetArg (Args[0],XtNsliderOrigin, (XtArgVal) abs(newx));
		XtSetValues((Widget )lw->swindow.hScrollBar,Args,1);

	    }
	}

    }		
}

/************************************************************************
 *                                                                      *
 *  HighlightWidget - highlight a selected widget according to the      *
 *                    current mode.                                     *
 *                                                                      *
 ************************************************************************/
static void HighlightWidget(w,lw)
    Widget       w;
    XwListWidget lw;
{
    XwListConstraintRec *conRec;

    conRec =  (XwListConstraintRec *) w->core.constraints;

    if (!(conRec->highlighted) && (XtClass(w) != widgetClass))
    {
	if (lw->list.Method == XwBORDER)
	    _XwHighlightBorder(w);
	else
	{
	    XtSetArg(Args[0],XtNforeground, (XtArgVal) conRec->bg);
            XtSetArg(Args[1],XtNbackground, (XtArgVal) conRec->fg);
            XtSetValues(w,Args,2);
        }
        conRec->highlighted = TRUE;
	conRec->lastHL = lw->list.Method;
    }
}
/************************************************************************
 *                                                                      *
 *  UnhighlightWidget - Unhighlight a selected widget according to the  *
 *                  current mode.                                       *
 *                                                                      *
 ************************************************************************/
static void UnhighlightWidget(w,lw)
    Widget       w;
    XwListWidget lw;
{
    XwListConstraintRec *conRec;

    conRec =  (XwListConstraintRec *) w->core.constraints;

    if ((conRec->highlighted) && (XtClass(w) != widgetClass))
    {
        if (conRec->lastHL == XwBORDER)
            _XwUnhighlightBorder(w);
        else
        {
            XtSetArg(Args[0],XtNforeground, (XtArgVal) conRec->fg);
            XtSetArg(Args[1],XtNbackground, (XtArgVal) conRec->bg);
            XtSetValues(w,Args,2);
        }
        conRec->highlighted = FALSE;
    }
}
/************************************************************************
 *                                                                      *
 *  UpdateSelectedList - traverse the element list and construct a list *
 *                       of selected elements.                          *
 *                                                                      *
 ************************************************************************/
static void UpdateSelectedList(lw)
    XwListWidget  lw;
{
    XwListConstraintRec *constraintRec;
    XwListRowColWidget	rcw;
    WidgetList	 rckids;
    int         numkids;

    rcw = lw->list.rcmanager;
    rckids = rcw->composite.children;
    numkids = rcw->composite.num_children;
    lw->list.NumSelected = 0;
    while (numkids--)
    {
        constraintRec = (XwListConstraintRec *) (*rckids)->core.constraints;	
	if (constraintRec->selected)
	{
            lw->list.NumSelected++;
            lw->list.SelectedList = (WidgetList )XtRealloc(lw->list.SelectedList,
                                            (sizeof(Widget) * lw->list.NumSelected));
            lw->list.SelectedList[(lw->list.NumSelected)-1] = *rckids;
        }
	    rckids++;
    }
 }
/*****************************************
static void DumpList(lw,s)
  XwListWidget lw;
  char *s;
{
    WidgetList	 rckids;
    int         numkids,i;
    XwListConstraintRec *constraintRec;
	rckids = ((XwListWidget )lw->list.rcmanager)->composite.children;
	numkids = ((XwListWidget )lw->list.rcmanager)->composite.num_children;
if (numkids)
{
printf("%s List is:\n",s);
i=numkids;
for(i=0;i < numkids; i++)
{
constraintRec = (XwListConstraintRec *) (rckids[i])->core.constraints;
printf("List[%d] (%s) column, row = %d,%d, ",i,rckids[i]->core.name,constraintRec->column,constraintRec->row);
printf("(x,y,w,h) = (%d,%d,%d,%d)",rckids[i]->core.x,rckids[i]->core.y,rckids[i]->core.width,rckids[i]->core.height);
if (XtIsManaged(rckids[i])) printf("(Managed)\n");
else printf("\n");
}
fflush(stdout);
}
}
****************************************/
