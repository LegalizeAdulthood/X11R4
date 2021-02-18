/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        SWindow.c
 **
 **   Project:     X Widgets
 **
 **   Description: Scrolled Window Manager Widget
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
#include <Xw/BBoard.h>
#include <Xw/Valuator.h>
#include <Xw/Arrow.h>
#include <Xw/ScrollBarP.h>
#include <Xw/BBoardP.h>
#include <Xw/ValuatorP.h>
#include <Xw/ArrowP.h>
#include <Xw/SWindow.h>
#include <Xw/SWindowP.h>

static void Initialize();
static void Realize();
static void Resize();
static void InsertChild();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static void ChangeManaged();
static void ClassInitialize();


#define SB_BORDER_WIDTH 1

/*************************************<->*************************************
 *
 *   Description:  default translation table for class: Scrolled Window
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/
static char defaultTranslations[] = {
    "<EnterWindow>:          enter()\n\
     <LeaveWindow>:          leave()"
};


/*************************************<->*************************************
 *
 *   Description:  action list for class: Scrolled Window
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *
 *************************************<->***********************************/
static XtActionsRec actionsList[] =
{
  {"enter", (XtActionProc) _XwManagerEnter},
  {"leave", (XtActionProc) _XwManagerLeave},
};

static XwSWindowWidget sww;
static Boolean InInit;
static Arg vSBArgs[30];
static Arg hSBArgs[30];
static Arg bBArgs[20];
/*  */
static XwSWindowClassRec swcr;


/****************************************************************
 *
 * Scrolled Window Resources
 *
 ****************************************************************/

static XtResource resources[] = 
{
   {
     XtNhighlightColor, XtCForeground, XtRPixel, sizeof (Pixel),
     XtOffset (XwSWindowWidget, swindow.highlight_color),
     XtRString, "Black"
   },

   {
     XtNhighlightTile, XtCHighlightTile, XtRTileType, sizeof (int),
     XtOffset (XwSWindowWidget, swindow.highlight_tile),
     XtRString, "50_foreground"
   },

   {
     XtNhighlightStyle, XtCHighlightStyle, XtRHighlightStyle, sizeof (int),
     XtOffset (XwSWindowWidget, swindow.highlight_style),
     XtRString, "pattern_border"
   },

   {
     XtNtraversalType, XtCTraversalType, XtRTraversalType, sizeof (int),
     XtOffset (XwSWindowWidget, swindow.traversal_type),
     XtRString, "highlight_off"
   },

   {
      XtNvSliderMin, XtCVSliderMin, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.vmin),
      XtRInt, NULL
   },

   {
      XtNvSliderMax, XtCVSliderMax, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.vmax),
      XtRInt, NULL
   },

   {
      XtNvSliderOrigin, XtCVSliderOrigin, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.vOrigin),
      XtRInt, NULL
   },

   {
      XtNvSliderExtent, XtCVSliderExtent, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.vExtent),
      XtRInt, NULL
   },

   {
      XtNhSliderMin, XtCHSliderMin, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.hmin),
      XtRInt, NULL
   },

   {
      XtNhSliderMax, XtCHSliderMax, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.hmax),
      XtRInt, NULL
   },

   {
      XtNhSliderOrigin, XtCHSliderOrigin, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.hOrigin),
      XtRInt, NULL
   },

   {
      XtNhSliderExtent, XtCHSliderExtent, XtRInt, sizeof (int),
      XtOffset(XwSWindowWidget, swindow.hExtent),
      XtRInt, NULL
   },

   {
      XtNinitialDelay, XtCInitialDelay, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.initial_delay),
      XtRString, "500"
   },

   {
      XtNrepeatRate, XtCRepeatRate, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.repeat_rate),
      XtRString, "100"
   },

   {
      XtNgranularity, XtCGranularity, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.granularity),
      XtRString, "10"
   },

   {  
      XtNvScrollEvent, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwSWindowWidget, swindow.vScrollEvent),
      XtRPointer, (caddr_t) NULL
   },

   {  
      XtNhScrollEvent, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwSWindowWidget, swindow.hScrollEvent),
      XtRPointer, (caddr_t) NULL
   },

/************************  Sorry. Can't set these now. **************
 *   {
 *      XtNvsbX, XtCVsbX, XtRInt, sizeof (int),
 *      XtOffset (XwSWindowWidget, swindow.vsbX),
 *      XtRString, "-1"
 *   },
 *   {
 *      XtNvsbY, XtCVsbY, XtRInt, sizeof (int),
 *      XtOffset (XwSWindowWidget, swindow.vsbY),
 *      XtRString, "-1"
 *   },
 *   {
 *      XtNvsbHeight, XtCVsbHeight, XtRInt, sizeof (int),
 *      XtOffset (XwSWindowWidget, swindow.vsbHeight),
 *      XtRString, "0"
 *   },
 *
 *   {
 *      XtNhsbX, XtCHsbX, XtRInt, sizeof (int),
 *      XtOffset (XwSWindowWidget, swindow.hsbX),
 *      XtRString, "-1"
 *   },
 *   {
 *      XtNhsbY, XtCHsbY, XtRInt, sizeof (int),
 *      XtOffset (XwSWindowWidget, swindow.hsbY),
 *      XtRString, "-1"
 *   },
 *   {
 *      XtNhsbWidth, XtCHsbWidth, XtRInt, sizeof (int),
 *      XtOffset (XwSWindowWidget, swindow.hsbWidth),
 *      XtRString, "0"
 *   },
 *******************************************************************/
   {
      XtNvsbWidth, XtCVsbWidth, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.vsbWidth),
      XtRString, "20"
   },

   {
      XtNhsbHeight, XtCHsbHeight, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.hsbHeight),
      XtRString, "20"
   },
	  
   {
      XtNinitialDelay, XtCInitialDelay, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.initial_delay),
      XtRString, "500"
   },
   {
      XtNforceHorizontalSB, XtCForceHorizontalSB, XtRBoolean, sizeof (Boolean),
      XtOffset (XwSWindowWidget, swindow.forceHSB),
      XtRBoolean, FALSE
   },
   {
      XtNforceVerticalSB, XtCForceVerticalSB, XtRBoolean, sizeof (Boolean),
      XtOffset (XwSWindowWidget, swindow.forceVSB),
      XtRBoolean, FALSE
   },
   {
      XtNinitialX, XtCInitialX, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.initialX),
      XtRString, "0"
   },
   {
      XtNinitialY, XtCInitialY, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.initialY),
      XtRString, "0"
   },
   {
      XtNborderPad, XtCBorderPad, XtRInt, sizeof (int),
      XtOffset (XwSWindowWidget, swindow.pad),
      XtRString, "4"
   },

};



/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

XwSWindowClassRec XwswindowClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &XwmanagerClassRec,
    /* class_name         */    "ScrolledWindow",
    /* widget_size        */    sizeof(XwSWindowRec),
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
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    (XtSetValuesFunc )SetValues,
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
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	InsertChild,	/* Basically Inherited */
    /* delete_child	  */	NULL,	/* Inherit from superclass */
				NULL,
  },{
/* Constraint class Init */
    NULL,
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
      
  },{
/* manager_class fields */
   /* traversal handler   */    NULL,
   /* translations        */	NULL,
  },{
/* Scrolled Window class - none */     
     /* mumble */               0
 }	
};

WidgetClass XwswindowWidgetClass = (WidgetClass)&XwswindowClassRec;
WidgetClass XwscrolledWindowWidgetClass = (WidgetClass)&XwswindowClassRec;


/************************************************************************
 *
 *   Callback Functions
 *
 *   These are the callback routines for the scrollbar actions.
 *
 ************************************************************************/

extern void VertSliderMove();
extern void HorizSliderMove();
extern void HorizAreaSelect();
extern void VertAreaSelect();

static XtCallbackRec VSCallBack[] =
{
   {VertSliderMove, (caddr_t) NULL},
   {NULL,           (caddr_t) NULL},
};

static XtCallbackRec HSCallBack[] =
{
   {HorizSliderMove, (caddr_t) NULL},
   {NULL,           (caddr_t) NULL},
};

static XtCallbackRec VACallBack[] =
{
   {VertAreaSelect, (caddr_t) NULL},
   {NULL,           (caddr_t) NULL},
};

static XtCallbackRec HACallBack[] =
{
   {HorizAreaSelect, (caddr_t) NULL},
   {NULL,           (caddr_t) NULL},
};


/************************************************************************
 *
 *  VertSliderMove
 *
 *  Callback for the sliderMoved resource of the vertical scrollbar.
 *
 ************************************************************************/
static  void  VertSliderMove(w,closure,call_data)
    Widget w;
    caddr_t  closure, call_data;
{
    XwSWindowWidget sw;

    sw = (XwSWindowWidget )w->core.parent;
    sw->swindow.vOrigin = (int )call_data;
    if (!sw->swindow.bbChild->core.being_destroyed)
    {
    XtMoveWidget(sw->swindow.bbChild,
		 (Position ) (sw->swindow.bbChild->core.x),
		 (Position ) -((int ) call_data));
    XtCallCallbacks(sw,XtNvScrollEvent,call_data);
    }
}

/************************************************************************
 *
 *  VertAreaSelect
 *
 *  Callback for the AreaSelect resource of the vertical scrollbar.
 *
 ************************************************************************/
static  void  VertAreaSelect(w,closure,call_data)
    Widget w;
    caddr_t  closure, call_data;
{
    XwSWindowWidget sw;

    sw = (XwSWindowWidget )w->core.parent;
    sw->swindow.vOrigin = (int )call_data;
    if (!sw->swindow.bbChild->core.being_destroyed)
    {
    XtMoveWidget(sw->swindow.bbChild,
		 (Position ) (sw->swindow.bbChild->core.x),
		 (Position ) -((int ) call_data));
    XtSetArg (vSBArgs[0],XtNsliderOrigin, (XtArgVal) (call_data));
    XtSetValues(w,vSBArgs,1);
    XtCallCallbacks(sw,XtNvScrollEvent,call_data);
    }
}

/************************************************************************
 *
 *  HorizSliderMove
 *
 *  Callback for the sliderMoved resource of the horizontal scrollbar.
 *
 ************************************************************************/
static void  HorizSliderMove(w,closure,call_data)
    Widget w;
    caddr_t  closure, call_data;
{
    XwSWindowWidget sw;

    sw = (XwSWindowWidget )w->core.parent;
    sw->swindow.vOrigin = (int )call_data;
    if (!sw->swindow.bbChild->core.being_destroyed)
    {
    XtMoveWidget(sw->swindow.bbChild,
		 (Position ) -((int) call_data),
 		 (Position ) (sw->swindow.bbChild->core.y));
    XtCallCallbacks(sw,XtNhScrollEvent,call_data);
    }

}
/************************************************************************
 *
 *  HorizAreaSelect
 *
 *  Callback for the AreaSelect resource of the vertical scrollbar.
 *
 ************************************************************************/
static  void  HorizAreaSelect(w,closure,call_data)
    Widget w;
    caddr_t  closure, call_data;
{
    XwSWindowWidget sw;

    sw = (XwSWindowWidget )w->core.parent;
    sw->swindow.vOrigin = (int )call_data;
    if (!sw->swindow.bbChild->core.being_destroyed)
    {
    XtMoveWidget(sw->swindow.bbChild,
		 (Position ) -((int) call_data),
 		 (Position ) (sw->swindow.bbChild->core.y));
    XtSetArg (vSBArgs[0],XtNsliderOrigin, (XtArgVal) (call_data));
    XtSetValues(w,vSBArgs,1);
    XtCallCallbacks(sw,XtNhScrollEvent,call_data);
    }
      
}

/************************************************************************
 *
 *  KidKilled 
 *
 *  Destroy callback for the BB child widget.
 *
 ************************************************************************/

 static XtCallbackProc KidKilled(w,closure,call_data)
    Widget w;
    caddr_t  closure, call_data;
{
    XwSWindowWidget sw;
    
    sw = (XwSWindowWidget )(w->core.parent)->core.parent;
    sw->swindow.bbChild = NULL;
    Resize(sw);
}


/*************************************<->*************************************
 *
 *  ClassInitialize
 *
 *************************************<->***********************************/

static void ClassInitialize()
{
    XwSWindowWidgetClass myclass;
    XwManagerWidgetClass superclass;


    XwRegisterConverters();
    myclass = (XwSWindowWidgetClass) XwswindowWidgetClass;
    XwswindowClassRec.manager_class.traversal_handler =
       (XwTraversalProc) mgr_traversal;

    /* Inherit delete_child proc from Manager */
    superclass = (XwManagerWidgetClass)XwmanagerWidgetClass;
    myclass->composite_class.delete_child =
       superclass->composite_class.delete_child;

}


/*************************************<->*************************************
 *
 *  Initialize
 *
 *************************************<->***********************************/

static void Initialize(request, new)
    XwSWindowWidget request, new;
{
    Widget bw;
    int    ht,myht;
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

/****************
 *
 *  Create the widgets.
 *
 ****************/
    new->swindow.InInit = TRUE;
    ht = 0;

    XtSetArg (bBArgs[0],XtNx,(XtArgVal) ht);
    XtSetArg (bBArgs[1],XtNy,(XtArgVal) ht);
    XtSetArg (bBArgs[4],XtNhighlightThickness,(XtArgVal) 0);
    XtSetArg (bBArgs[5],XtNborderWidth,(XtArgVal) 0);

    new->swindow.vsbX =  new->core.width;
    XtSetArg (vSBArgs[0],XtNx,(XtArgVal) (new->swindow.vsbX));

    new->swindow.vsbY = 0;
    XtSetArg (vSBArgs[1],XtNy,(XtArgVal) (new->swindow.vsbY));

    new->swindow.hsbX  = 0;
    XtSetArg (hSBArgs[0],XtNx,(XtArgVal) (new->swindow.hsbX));

    new->swindow.hsbY = new->core.height;
    XtSetArg (hSBArgs[1],XtNy,(XtArgVal) (new->swindow.hsbY));


    new->swindow.initWidth = new->swindow.vsbX - ((ht << 1) );
    new->swindow.initHeight = new->swindow.hsbY - ((ht << 1) );
    if (new->swindow.initWidth < 0) new->swindow.initWidth = 1;
    if (new->swindow.initHeight < 0) new->swindow.initHeight = 1;

    XtSetArg (bBArgs[2],XtNwidth,(XtArgVal) ((int ) (new->swindow.initWidth)));
    XtSetArg (bBArgs[3],XtNheight,(XtArgVal) ((int ) (new->swindow.initHeight)));
    XtSetArg (bBArgs[6],XtNlayout, (XtArgVal) XwSWINDOW);
    new->swindow.bboard =
        (XwBulletinWidget) XtCreateWidget("BulletinBoard",
					       XwbulletinWidgetClass,
					       (Widget) new,bBArgs,7);

    bw =(Widget ) new->swindow.bboard;
    

    XtSetArg (vSBArgs[2],XtNwidth,(XtArgVal ) (new->swindow.vsbWidth));

    new->swindow.vsbHeight = new->swindow.initHeight+ (ht << 1);
    
    XtSetArg (vSBArgs[3],XtNheight, (XtArgVal) (new->swindow.vsbHeight));

    XtSetArg (vSBArgs[4],XtNslideOrientation,(XtArgVal) (XwVERTICAL));

    XtSetArg (vSBArgs[5], XtNforeground,
             (XtArgVal) new -> manager.foreground);

    XtSetArg (vSBArgs[6], XtNbackground,
             (XtArgVal) new -> core.background_pixel);

    XtSetArg (vSBArgs[7], XtNhighlightColor,
             (XtArgVal) new -> swindow.highlight_color);

    XtSetArg (vSBArgs[8], XtNhighlightTile,
             (XtArgVal) new -> swindow.highlight_tile);

    XtSetArg (vSBArgs[9], XtNhighlightStyle,
             (XtArgVal) new -> swindow.highlight_style);

    XtSetArg (vSBArgs[10], XtNhighlightThickness,
             (XtArgVal) new -> manager.highlight_thickness);

    XtSetArg (vSBArgs[11], XtNtraversalType,
             (XtArgVal) new -> swindow.traversal_type);

    XtSetArg (vSBArgs[12], XtNbackgroundTile,
	     (XtArgVal) new -> manager.background_tile);

    XtSetArg (vSBArgs[13],XtNsliderMoved, (XtArgVal) VSCallBack);

    
    if (new -> swindow.vmin == NULL)
	new -> swindow.vmin = 0;
    XtSetArg (vSBArgs[14],XtNsliderMin, (XtArgVal) (new->swindow.vmin));

    if (new -> swindow.vmax == NULL)
	new->swindow.vmax =  new->core.height + ht;
    XtSetArg (vSBArgs[15],XtNsliderMax, (XtArgVal) (new->swindow.vmax));

    if (new -> swindow.vOrigin == NULL)
	new -> swindow.vOrigin = 0;

    XtSetArg (vSBArgs[16],XtNsliderOrigin, (XtArgVal) new->swindow.vOrigin);

    if (new -> swindow.vExtent == NULL)
	if ((int )(bw->core.height - bw->core.y) < 10)
	    new->swindow.vExtent = new->swindow.vmax - new->swindow.vOrigin;
	else
            new->swindow.vExtent =  (int ) ((bw->core.height - bw->core.y) / 10);

    XtSetArg (vSBArgs[17],XtNsliderExtent, (XtArgVal) (new->swindow.vExtent));
    
    XtSetArg (vSBArgs[18],XtNareaSelected, (XtArgVal) VACallBack);
	      
    

    new->swindow.hsbWidth = new->swindow.initWidth + (ht << 1);

    XtSetArg (hSBArgs[2],XtNwidth,(XtArgVal ) (new->swindow.hsbWidth));

    XtSetArg (hSBArgs[3],XtNheight, (XtArgVal)  (new->swindow.hsbHeight));

    XtSetArg (hSBArgs[4],XtNslideOrientation,(XtArgVal) (XwHORIZONTAL));
    XtSetArg (hSBArgs[5], XtNforeground,
             (XtArgVal) new -> manager.foreground);

    XtSetArg (hSBArgs[6], XtNbackground,
             (XtArgVal) new -> core.background_pixel);

    XtSetArg (hSBArgs[7], XtNhighlightColor,
             (XtArgVal) new -> swindow.highlight_color);

    XtSetArg (hSBArgs[8], XtNhighlightTile,
             (XtArgVal) new -> swindow.highlight_tile);

    XtSetArg (hSBArgs[9], XtNhighlightStyle,
             (XtArgVal) new -> swindow.highlight_style);

    XtSetArg (hSBArgs[10], XtNhighlightThickness,
             (XtArgVal) new -> manager.highlight_thickness);

    XtSetArg (hSBArgs[11], XtNtraversalType,
             (XtArgVal) new -> swindow.traversal_type);

    XtSetArg (hSBArgs[12], XtNbackgroundTile,
	     (XtArgVal) new -> manager.background_tile);

    XtSetArg (hSBArgs[13],XtNsliderMoved, (XtArgVal) HSCallBack);

/****************
 *
 *  Set up the valuators.
 *
 ****************/
    
    if (new -> swindow.hmin == NULL)
	new -> swindow.hmin = 0;
    XtSetArg (hSBArgs[14],XtNsliderMin, (XtArgVal) (new->swindow.hmin));

    if (new -> swindow.hmax == NULL)
	new -> swindow.hmax =  new->core.width + ht; 
    XtSetArg (hSBArgs[15],XtNsliderMax, (XtArgVal) (new->swindow.hmax));

    if (new -> swindow.hOrigin == NULL)
	new -> swindow.hOrigin = 0;

    XtSetArg (hSBArgs[16],XtNsliderOrigin, (XtArgVal) 	new -> swindow.hOrigin);

    if (new -> swindow.hExtent == NULL)
	if ((int )(bw->core.width - bw->core.x) < 10)
	    new->swindow.hExtent = new->swindow.hmax - new->swindow.hOrigin;
	else
           new -> swindow.hExtent =  (int ) ((bw->core.width - bw->core.x) / 10);

    XtSetArg (hSBArgs[17],XtNsliderExtent, (XtArgVal) (new->swindow.hExtent));

    XtSetArg (hSBArgs[18],XtNareaSelected, (XtArgVal) HACallBack);
    

    XtSetArg (vSBArgs[19],XtNborderWidth,(XtArgVal) SB_BORDER_WIDTH);
    XtSetArg (hSBArgs[19],XtNborderWidth,(XtArgVal) SB_BORDER_WIDTH);

    XtSetArg(vSBArgs[20], XtNgranularity,
				    (XtArgVal ) new->swindow.granularity);
    XtSetArg(hSBArgs[20], XtNgranularity,
				    (XtArgVal ) new->swindow.granularity);

    new->swindow.vScrollBar =
	(XwScrollBarWidget) XtCreateWidget("ScrollBar", XwscrollbarWidgetClass,
					   (Widget) new,vSBArgs,21);
    new->swindow.hScrollBar =
	(XwScrollBarWidget) XtCreateWidget("ScrollBar", XwscrollbarWidgetClass,
					   (Widget) new,hSBArgs,21);
    new->swindow.bbChild = NULL;
    new->swindow.InInit = FALSE;    
}



/*************************************<->*************************************
 *
 *  Realize
 *
 *************************************<->***********************************/
static void Realize(w, p_valueMask, attributes)
    register XwSWindowWidget w;
    Mask *p_valueMask;
    XSetWindowAttributes *attributes;
{
    Widget bw;
    int	ht;
    Mask valueMask = *p_valueMask;

    bw = (Widget ) w ->swindow.bboard;
     valueMask |= CWBitGravity;
    if (XtIsSubclass(w,XwmanagerWidgetClass))
     	attributes->bit_gravity = NorthWestGravity;
    else
     	attributes->bit_gravity = ForgetGravity;

    ht = 0;

/****************
 *
 * The new toolkit doesn't tell us if the bboard grew since creation,
 * so we force it back to our size if needed.
 *
 ****************/ 
    if ((w->swindow.initWidth != w->swindow.bboard->core.width) ||
        (w->swindow.initHeight != w->swindow.bboard->core.height))
	XtResizeWidget(w->swindow.bboard,w->swindow.initWidth,
		       w->swindow.initHeight, 
		       w->swindow.bboard->core.border_width);

/*    SetValuators(w);*/

    XtCreateWindow(w, InputOutput, (Visual *)CopyFromParent,
	valueMask, attributes);

    if (w->swindow.bbChild != NULL)
    {
	XtSetArg(bBArgs[0],XtNx,(XtArgVal ) w->swindow.initialX);
	XtSetArg(bBArgs[1],XtNy,(XtArgVal ) w->swindow.initialY);
    	XtSetValues(w->swindow.bbChild,bBArgs,2);
	Resize(w);
    }
    _XwRegisterName(w);
 } /* Realize */


/************************************************************************
 *
 *  ClearBorder
 *	Clear the right and bottom border area and save 
 *	the frames width and height.
 *
 ************************************************************************/

static void ClearBorder (sw)
XwSWindowWidget sw;

{
    int ht,oldwidth,oldheight;

   if (XtIsRealized (sw))
   {

      ht = sw->manager.highlight_thickness;
      oldwidth = sw -> swindow.initWidth + (ht << 1);
      oldheight = sw -> swindow.initHeight + (ht << 1);

         XClearArea (XtDisplay (sw), XtWindow (sw),
                  oldwidth - 2, 0, 2, oldheight, FALSE);
         XClearArea (XtDisplay (sw), XtWindow (sw),
                  0, 0, oldwidth, 2, FALSE);

         XClearArea (XtDisplay (sw), XtWindow (sw),
                  0, oldheight - 2, oldwidth, 2, FALSE);
         XClearArea (XtDisplay (sw), XtWindow (sw),
                  0, 0, 2, oldheight, FALSE);
   }

}



/*************************************<->*************************************
 *
 *  InsertChild
 *
 *************************************<->***********************************/


static void InsertChild(w)
    Widget	w;
{
    int _XW_NARGS = 0;
    ArgList	args = NULL;
    Cardinal  *p_num_args = (Cardinal *) &_XW_NARGS;

    XwManagerWidgetClass superclass;
    XwSWindowWidget   mom;
    Widget x;
    int    ht;
/****************
 *
 *  Funky stuff here. If this widget is one that I created, then insert
 *  it and manage it. If it's not mine, reparent it and make it a child
 *  of the bboard manager.
 *
 ****************/

    mom = (XwSWindowWidget )w->core.parent;
    ht = 0;
    superclass = (XwManagerWidgetClass)XwmanagerWidgetClass;
    
    if ((w == (Widget )mom->swindow.hScrollBar) ||
	(w == (Widget )mom->swindow.vScrollBar) ||
	(w == (Widget )mom->swindow.bboard)     ||
	(mom->swindow.InInit))
    {
	(*superclass->composite_class.insert_child)(w, args, p_num_args);
	XtManageChild(w);
    }
    else
    {
	if (mom->swindow.bbChild == NULL)
	{
	    mom->swindow.bbChild = w;
	    w->core.parent = (Widget )mom->swindow.bboard;
	    XtAddCallback(w,XtNdestroyCallback,KidKilled,NULL);
	    (*superclass->composite_class.insert_child)(w, args, p_num_args);

	    if (!mom->swindow.initialY)
	        mom->swindow.vOrigin = abs(mom->swindow.initialY);
	    if (!mom->swindow.initialX)
	        mom->swindow.hOrigin = abs(mom->swindow.initialX);

            if (mom->swindow.hOrigin < mom->swindow.hmin)
	        mom->swindow.hOrigin = mom->swindow.hmin;
            if (mom->swindow.vOrigin < mom->swindow.vmin)
	        mom->swindow.vOrigin = mom->swindow.vmin;


	    XtSetArg(bBArgs[0],XtNx,(XtArgVal ) mom->swindow.initialX);
	    XtSetArg(bBArgs[1],XtNy,(XtArgVal ) mom->swindow.initialY);
	    XtSetValues(w,bBArgs,2);

	    ChangeManaged(mom);
	    XtSetArg (vSBArgs[0],XtNsliderOrigin, (XtArgVal) mom->swindow.vOrigin);
	    XtSetArg (hSBArgs[0],XtNsliderOrigin, (XtArgVal) mom->swindow.hOrigin);
	    XtSetValues(mom->swindow.vScrollBar,vSBArgs,1);
	    XtSetValues(mom->swindow.hScrollBar,hSBArgs,1);


	}
	else
	    XtWarning("ScrolledWindow: No more than one widget in window\n");
    }

}


/*************************************<->*************************************
 *
 *  ReSize
 *
 *   Description:
 *   -----------
 *   Recompute the size of the bboard and scroll bars.
 *
 *************************************<->***********************************/

static void Resize(sw)
    XwSWindowWidget	sw;
{
    int bbWidth,bbHeight,newx,newy,newpos,
	viswidth,visheight,ht,pad,myht, bw,
	kidwidth,kidheight;

 /*   if (!XtIsRealized(sw)) return;		 No resize if not realized */


    ht = 0;


    if (sw->swindow.bbChild == NULL) 		/* If no kid, just keep the */
    {						/* scrollbars invisible.    */
	XtMoveWidget(sw->swindow.vScrollBar,sw->core.width,
		     sw->swindow.vScrollBar->core.y);
	XtMoveWidget(sw->swindow.hScrollBar,sw->swindow.hScrollBar->core.x,
		     sw->core.height);
	sw->swindow.initWidth = sw->core.width - ((ht << 1) );
	sw->swindow.initHeight = sw->core.height - ((ht << 1) );

        XtConfigureWidget(sw->swindow.bboard, ht, ht,
		          sw->swindow.initWidth, sw->swindow.initHeight,
                          SB_BORDER_WIDTH);
	return;
    }

    pad = sw->swindow.pad;
    bw = sw->swindow.bbChild->core.border_width;

    newx = sw->swindow.bbChild->core.x;
    newy = sw->swindow.bbChild->core.y;
    if (abs(newx) < sw->swindow.hmin)
	newx = -(sw->swindow.hmin);
    if (abs(newy) < sw->swindow.vmin)
	newy = -(sw->swindow.vmin);

    kidwidth = sw->swindow.bbChild->core.width + (bw * 2);
    kidheight = sw->swindow.bbChild->core.height + (bw * 2);

    viswidth = kidwidth - abs(newx);
    if (viswidth <= 0) viswidth = 1;

    visheight = kidheight - abs(newy);
    if (visheight <= 0) visheight = 1;

    bbWidth = sw->core.width - (sw->swindow.vsbWidth + (SB_BORDER_WIDTH << 1) + 
		(ht*2 + pad));

    bbHeight =sw->core.height - (sw->swindow.hsbHeight + (SB_BORDER_WIDTH << 1) + 
		(ht*2 + pad));

    if (((sw->core.width - (ht*2)) >= kidwidth) &&
	((sw->core.height - (ht*2)) >= kidheight) &&
	!sw->swindow.forceVSB )
	bbWidth = sw->core.width - (ht*2);

    if (((sw->core.height - (ht*2)) >=  kidheight) &&
	 ((sw->core.width - (ht*2)) >= kidwidth) &&
	!sw->swindow.forceHSB )
	bbHeight = sw->core.height - (ht*2);

    if (bbHeight <= 0) bbHeight = 1;
    if (bbWidth <= 0) bbWidth = 1;

    if (!sw->swindow.forceHSB && 
    	((bbWidth >= kidwidth) ||
	 (bbWidth >= viswidth)))
    {
	sw->swindow.hExtent = (bbWidth < kidwidth) ?
				bbWidth : kidwidth;
    	if (bbWidth >= kidwidth)
	{
	    bbHeight =(sw->core.height - (ht << 1));
	    if (bbHeight <= 0) bbHeight = 1;
	    sw->swindow.vsbHeight = bbHeight  + (ht << 1);
	    sw->swindow.vExtent = ( bbHeight > (sw->swindow.vmax - ht)) ?
				   (sw->swindow.vmax - ht) : bbHeight;
	    sw->swindow.hExtent = sw->swindow.hmax - ht;
            if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
	        sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
            if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
	        sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
	    XtSetArg(vSBArgs[0],XtNsliderExtent, (XtArgVal) 
						 sw->swindow.vExtent);
	    XtSetValues(sw->swindow.vScrollBar,vSBArgs,1);
    	}
	newx =  sw->swindow.bbChild->core.x + (bbWidth - viswidth);
	if (newx > 0) 
	    newx = 0;
	if (abs(newx) < sw->swindow.hmin)
	   newx = -(sw->swindow.hmin);
	sw->swindow.hOrigin = abs(newx);

        if (sw->swindow.hOrigin < sw->swindow.hmin)
	    sw->swindow.hOrigin = sw->swindow.hmin;

	sw->swindow.hsbWidth = bbWidth  + (ht << 1);
        if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
	    sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
	XtSetArg(hSBArgs[0],XtNsliderExtent, (XtArgVal) sw->swindow.hExtent);
	XtSetArg(hSBArgs[1],XtNsliderOrigin, 
		 (XtArgVal) sw->swindow.hOrigin);
	XtSetValues(sw->swindow.hScrollBar,hSBArgs,2);
    }
    else
    {
	XtManageChild(sw->swindow.hScrollBar);
	if (((sw->swindow.hmax - sw->swindow.hmin) < bbWidth) &&
	    sw->swindow.forceHSB)
	    sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hmin;
	else
	    sw->swindow.hExtent = bbWidth;
	sw->swindow.hOrigin = abs(newx);

        if (sw->swindow.hOrigin < sw->swindow.hmin)
	    sw->swindow.hOrigin = sw->swindow.hmin;

        if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
	    sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
	sw->swindow.hsbWidth = bbWidth  + (ht << 1);
	XtSetArg(hSBArgs[0],XtNsliderExtent, (XtArgVal) sw->swindow.hExtent);
	XtSetArg(hSBArgs[1],XtNsliderOrigin, 
		 (XtArgVal) sw->swindow.hOrigin);
	XtSetValues(sw->swindow.hScrollBar,hSBArgs,2);
    }


    if (!sw->swindow.forceVSB && 
	((bbHeight >= kidheight) ||
	 (bbHeight >= visheight)))
    {
	sw->swindow.vExtent = (bbHeight < kidheight) ?
				bbHeight : kidheight;
	if (bbHeight >= kidheight)
	{
	    bbWidth = (sw->core.width - (ht << 1));
	    if (bbWidth <= 0) bbWidth = 1;
	    sw->swindow.hsbWidth = bbWidth  + (ht << 1);
	    sw->swindow.hExtent = ( bbWidth > (sw->swindow.hmax - ht)) ?
				   (sw->swindow.hmax-ht) : bbWidth;
	    sw->swindow.vExtent = sw->swindow.vmax - ht ;
            if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
	        sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
            if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
	        sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
	    XtSetArg(hSBArgs[0],XtNsliderExtent, (XtArgVal) 
						 sw->swindow.hExtent);
	    XtSetValues(sw->swindow.hScrollBar,hSBArgs,1);
	}
	newy =  sw->swindow.bbChild->core.y + (bbHeight - visheight);

	if (newy > 0) 
	    newy = 0;
	if (abs(newy) < sw->swindow.vmin)
	   newy = -(sw->swindow.vmin);
	sw->swindow.vOrigin = abs(newy);

        if (sw->swindow.vOrigin < sw->swindow.vmin)
	    sw->swindow.vOrigin = sw->swindow.vmin;

	sw->swindow.vsbHeight = bbHeight  + (ht << 1);
        if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
	    sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
	XtSetArg(vSBArgs[0],XtNsliderExtent, (XtArgVal) sw->swindow.vExtent);
	XtSetArg(vSBArgs[1],XtNsliderOrigin, 
		 (XtArgVal) sw->swindow.vOrigin);
	XtSetValues(sw->swindow.vScrollBar,vSBArgs,2);
    }
    else
    {
	XtManageChild(sw->swindow.vScrollBar);
	if (((sw->swindow.vmax - sw->swindow.vmin) < bbHeight) &&
	    sw->swindow.forceVSB)
	    sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vmin;
	else
	    sw->swindow.vExtent = bbHeight;
	sw->swindow.vOrigin = abs(newy);

        if (sw->swindow.vOrigin < sw->swindow.vmin)
	    sw->swindow.vOrigin = sw->swindow.vmin;

        if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
	    sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
	sw->swindow.vsbHeight = bbHeight  + (ht << 1);
	XtSetArg(vSBArgs[0],XtNsliderExtent, (XtArgVal) sw->swindow.vExtent);
	XtSetArg(vSBArgs[1],XtNsliderOrigin, 
		 (XtArgVal) sw->swindow.vOrigin);
	XtSetValues(sw->swindow.vScrollBar,vSBArgs,2);
    }

    XtConfigureWidget(sw->swindow.bboard, ht, ht, bbWidth, bbHeight,
                      SB_BORDER_WIDTH);

    ClearBorder(sw);
    sw->swindow.initWidth = bbWidth;
    sw->swindow.initHeight = bbHeight;

    if (((bbHeight < kidheight) &&
        (sw->core.width > sw->swindow.vsbWidth )) ||
	sw->swindow.forceVSB)
        XtConfigureWidget(sw->swindow.vScrollBar,
		     (sw->core.width - sw->swindow.vsbWidth ),
		     0, (sw->swindow.vsbWidth - (SB_BORDER_WIDTH << 1)),
                     sw->swindow.vsbHeight,
		     SB_BORDER_WIDTH);
    else
        XtConfigureWidget(sw->swindow.vScrollBar,
		     sw->core.width,
		     0, (sw->swindow.vsbWidth - (SB_BORDER_WIDTH << 1)),
                     sw->swindow.vsbHeight,
		     SB_BORDER_WIDTH);
    if (((bbWidth < kidwidth) &&
        (sw->core.height > sw->swindow.hsbHeight)) ||
	sw->swindow.forceHSB)
        XtConfigureWidget(sw->swindow.hScrollBar, 0, 
		      (sw->core.height  - sw->swindow.hsbHeight),
		      sw->swindow.hsbWidth, 
		      (sw->swindow.hsbHeight - (SB_BORDER_WIDTH << 1)),
                      SB_BORDER_WIDTH);
    else
        XtConfigureWidget(sw->swindow.hScrollBar, 0, 
		      sw->core.height,
		      sw->swindow.hsbWidth, 
		      (sw->swindow.hsbHeight - (SB_BORDER_WIDTH << 1)),
                      SB_BORDER_WIDTH);

    if (newx != (sw->swindow.bbChild->core.x + bw)  ||
	newy != (sw->swindow.bbChild->core.y + bw))

         XtMoveWidget(sw->swindow.bbChild,newx,newy);
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
    XwSWindowWidget sw;
    int		    hsmax,vsmax,bw,ht,i;
/****************
 *
 *   If this request is from the bboard, update the valuators.
 *
 *   If the request is for an X or Y change only, the request is denied
 *   since things could get wierd. If you want to move yourself, why use
 *   a scrolled window? If the request is for X or Y, and also for a height
 *   or width change, we return Almost. If it's for Width or height only,
 *   we return All Clear.
 *
 ****************/

    sw = (XwSWindowWidget ) w->core.parent;
    while (!XtIsSubclass(sw,XwswindowWidgetClass))
        sw = (XwSWindowWidget )sw->core.parent;

    ht = 0;
    bw = (sw->swindow.bbChild->core.border_width << 1);
    if (sw->swindow.bbChild == w)
    {
	if (request -> request_mode & CWX || request -> request_mode & CWY)
	    if (request->request_mode & CWWidth || 
		request->request_mode & CWHeight)
	    {
		reply->x = sw->swindow.bbChild->core.x;
		reply->y = sw->swindow.bbChild->core.y;
		reply->width = request->width;
		reply->height = request->height;
		return((XtGeometryResult) XtGeometryAlmost);
	    }
	    else
		return((XtGeometryResult) XtGeometryNo);
	else
	    {
		hsmax = request->width + bw;
		if (hsmax < sw->swindow.hScrollBar->scrollbar.extent)
		    hsmax = sw->swindow.hScrollBar->scrollbar.max;
		vsmax = request->height + bw;
		if (vsmax < sw->swindow.vScrollBar->scrollbar.extent)
		    vsmax = sw->swindow.vScrollBar->scrollbar.max;
		if (request->request_mode & CWWidth)
		{
		    i = 1;
		    sw->swindow.hmax = hsmax;
		    if (sw->swindow.hExtent > (sw->swindow.hmax - sw->swindow.hmin))
		    {
		    	sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hmin;
		    	XtSetArg (hSBArgs[1],XtNsliderExtent, 
						   (XtArgVal) sw->swindow.hExtent);
			i=2;
		    }
		    if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
		    {
		    	sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
		    	XtSetArg (hSBArgs[1],XtNsliderExtent, 
				   (XtArgVal) sw->swindow.hExtent);
			i=2;
		    }
		    XtSetArg (hSBArgs[0],XtNsliderMax, (XtArgVal) hsmax);
		    XtSetValues((Widget ) sw->swindow.hScrollBar,hSBArgs,i);
		    w->core.width = request->width;
		}

		if (request->request_mode & CWHeight)
		{
		    sw->swindow.vmax = vsmax;
		    i = 1;
		    if (sw->swindow.vExtent > (sw->swindow.vmax - sw->swindow.vmin))
		    {
		    	sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vmin;
		    	XtSetArg (vSBArgs[1],XtNsliderExtent, 
						   (XtArgVal) sw->swindow.vExtent);
			i=2;
		    }
		    if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
		    {
		    	sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
		    	XtSetArg (vSBArgs[1],XtNsliderExtent, 
				   (XtArgVal) sw->swindow.vExtent);
			i=2;
		    }
		    XtSetArg (vSBArgs[0],XtNsliderMax,(XtArgVal) vsmax);   
		    XtSetValues((Widget ) sw->swindow.vScrollBar,vSBArgs,i);
		    w->core.height = request->height;
		}
		if (request->request_mode & CWBorderWidth)
		{
    		    sw->swindow.vmax =  w->core.height + bw;
		    XtSetArg (vSBArgs[0],XtNsliderMax,sw->swindow.vmax);
		    XtSetValues((Widget ) sw->swindow.vScrollBar,vSBArgs,1);
    		    sw->swindow.hmax =  w->core.width + bw;
		    XtSetArg (hSBArgs[0],XtNsliderMax,sw->swindow.hmax);
		    XtSetValues((Widget ) sw->swindow.hScrollBar,hSBArgs,1);
		    w->core.border_width = request->border_width;
		}
		Resize(sw);		
		return ((XtGeometryResult) XtGeometryYes);
	    }
    }
    return ((XtGeometryResult) XtGeometryYes);
	

}



/*************************************<->*************************************
 *
 *  ChangeManaged
 *
 *************************************<->***********************************/
static void ChangeManaged(sw)
    XwSWindowWidget sw;
{
    int	ht,hsmax,vsmax,i,bw;
    
    /* Put "managed children" info together again */
    _XwReManageChildren ((XwManagerWidget)sw);

    if (sw->swindow.bbChild) 
    {
    ht = 0;
    bw = (sw->swindow.bbChild->core.border_width << 1);

    hsmax = sw->swindow.bbChild->core.width + bw;
    if (hsmax < sw->swindow.hScrollBar->scrollbar.extent)
	hsmax = sw->swindow.hScrollBar->scrollbar.max;
    sw->swindow.hmax = hsmax;
    i = 1;
    if (sw->swindow.hExtent > (sw->swindow.hmax - sw->swindow.hmin))
    {
    	sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hmin;
    	XtSetArg (hSBArgs[1],XtNsliderExtent, 
				   (XtArgVal) sw->swindow.hExtent);
	i=2;
    }
    if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
    {
    	sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
    	XtSetArg (hSBArgs[1],XtNsliderExtent, 
				   (XtArgVal) sw->swindow.hExtent);
	i=2;
    }
    XtSetArg (hSBArgs[0],XtNsliderMax, (XtArgVal) hsmax);
    XtSetValues((Widget ) sw->swindow.hScrollBar,hSBArgs,i);

    vsmax = sw->swindow.bbChild->core.height + bw;
    i = 1;
    if (vsmax < sw->swindow.vScrollBar->scrollbar.extent)
	vsmax = sw->swindow.vScrollBar->scrollbar.max;
    sw->swindow.vmax = vsmax;
    if (sw->swindow.vExtent > (sw->swindow.vmax - sw->swindow.vmin))
    {
    	sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vmin;
    	XtSetArg (vSBArgs[1],XtNsliderExtent, 
				   (XtArgVal) sw->swindow.vExtent);
	i=2;
    }
    if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
    {
    	sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
    	XtSetArg (vSBArgs[1],XtNsliderExtent, 
				   (XtArgVal) sw->swindow.vExtent);
	i=2;
    }
    XtSetArg (vSBArgs[0],XtNsliderMax, (XtArgVal) vsmax);
    XtSetValues((Widget ) sw->swindow.vScrollBar,vSBArgs,i);
    Resize(sw);
    }
    
}
     


/*************************************<->*************************************
 *
 *  SetValues
 *
 *************************************<->***********************************/

static Boolean SetValues (current, request, new)
    XwSWindowWidget current, request, new;
{
    int	VargCount = 0;
    int HargCount = 0;
    Boolean Flag = FALSE;

/****************
 *
 *  So the grunge begins. First, check the resources that are common
 *  between scrollbars.
 *
 ****************/

    if (new->manager.foreground != current->manager.foreground)
    {
    	XtSetArg(vSBArgs[VargCount], XtNforeground,(XtArgVal ) new->manager.foreground);
    	XtSetArg(hSBArgs[HargCount], XtNforeground,(XtArgVal ) new->manager.foreground);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.highlight_color != current->swindow.highlight_color)
    {
    	XtSetArg(vSBArgs[VargCount], XtNhighlightColor,
				    (XtArgVal ) new->swindow.highlight_color);
    	XtSetArg(hSBArgs[HargCount], XtNhighlightColor,
				    (XtArgVal ) new->swindow.highlight_color);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.highlight_tile != current->swindow.highlight_tile)
    {
    	XtSetArg(vSBArgs[VargCount], XtNhighlightTile,
				    (XtArgVal ) new->swindow.highlight_tile);
    	XtSetArg(hSBArgs[HargCount], XtNhighlightTile,
				    (XtArgVal ) new->swindow.highlight_tile);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.highlight_style != current->swindow.highlight_style)
    {
    	XtSetArg(vSBArgs[VargCount], XtNhighlightStyle,
				    (XtArgVal ) new->swindow.highlight_style);
    	XtSetArg(hSBArgs[HargCount], XtNhighlightStyle,
				    (XtArgVal ) new->swindow.highlight_style);
    	VargCount++;
    	HargCount++;
    }

    if (new->manager.highlight_thickness != current->manager.highlight_thickness)
    {
    	XtSetArg(vSBArgs[VargCount], XtNhighlightThickness,
				    (XtArgVal ) new->manager.highlight_thickness);
    	XtSetArg(hSBArgs[HargCount], XtNhighlightThickness,
				    (XtArgVal ) new->manager.highlight_thickness);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.traversal_type != current->swindow.traversal_type)
    {
    	XtSetArg(vSBArgs[VargCount], XtNtraversalType,
				    (XtArgVal ) new->swindow.traversal_type);
    	XtSetArg(hSBArgs[HargCount], XtNtraversalType,
				    (XtArgVal ) new->swindow.traversal_type);
    	VargCount++;
    	HargCount++;
    }

    if (new->manager.background_tile != current->manager.background_tile)
    {
    	XtSetArg(vSBArgs[VargCount], XtNbackgroundTile,
				    (XtArgVal ) new->manager.background_tile);
    	XtSetArg(hSBArgs[HargCount], XtNbackgroundTile,
				    (XtArgVal ) new->manager.background_tile);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.initial_delay != current->swindow.initial_delay)
    {
    	XtSetArg(vSBArgs[VargCount], XtNinitialDelay,
				    (XtArgVal ) new->swindow.initial_delay);
    	XtSetArg(hSBArgs[HargCount], XtNinitialDelay,
				    (XtArgVal ) new->swindow.initial_delay);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.repeat_rate != current->swindow.repeat_rate)
    {
    	XtSetArg(vSBArgs[VargCount], XtNrepeatRate,
				    (XtArgVal ) new->swindow.repeat_rate);
    	XtSetArg(hSBArgs[HargCount], XtNrepeatRate,
				    (XtArgVal ) new->swindow.repeat_rate);
    	VargCount++;
    	HargCount++;
    }

    if (new->swindow.granularity != current->swindow.granularity)
    {
    	XtSetArg(vSBArgs[VargCount], XtNgranularity,
				    (XtArgVal ) new->swindow.granularity);
    	XtSetArg(hSBArgs[HargCount], XtNgranularity,
				    (XtArgVal ) new->swindow.granularity);
    	VargCount++;
    	HargCount++;
    }


/****************
 *
 *  Now look at the separate fields.
 *
 ****************/

    if (new->swindow.vmin != current->swindow.vmin)
    {
    	XtSetArg(vSBArgs[VargCount], XtNvSliderMin,
					(XtArgVal ) new->swindow.vmin);
    	VargCount++;
    }

    if (new->swindow.vmax != current->swindow.vmax)
    {
    	XtSetArg(vSBArgs[VargCount], XtNvSliderMax,
					(XtArgVal ) new->swindow.vmax);
    	VargCount++;
    }
    if (new->swindow.vOrigin != current->swindow.vOrigin)
    {
    	XtSetArg(vSBArgs[VargCount], XtNvSliderOrigin,
					(XtArgVal ) new->swindow.vOrigin);
    	VargCount++;
    }
    if (new->swindow.vExtent != current->swindow.vExtent)
    {
    	XtSetArg(vSBArgs[VargCount], XtNvSliderExtent,
					(XtArgVal ) new->swindow.vExtent);
    	VargCount++;
    }

    if (new->swindow.hmin != current->swindow.hmin)
    {
    	XtSetArg(hSBArgs[HargCount], XtNhSliderMin,
					(XtArgVal ) new->swindow.hmin);
    	HargCount++;
    }

    if (new->swindow.hmax != current->swindow.hmax)
    {
    	XtSetArg(hSBArgs[HargCount], XtNhSliderMax,
					(XtArgVal ) new->swindow.hmax);
    	HargCount++;
    }
    if (new->swindow.hOrigin != current->swindow.hOrigin)
    {
    	XtSetArg(hSBArgs[HargCount], XtNhSliderOrigin,
					(XtArgVal ) new->swindow.hOrigin);
    	HargCount++;
    }
    if (new->swindow.hExtent != current->swindow.hExtent)
    {
    	XtSetArg(hSBArgs[HargCount], XtNhSliderExtent,
					(XtArgVal ) new->swindow.hExtent);
    	HargCount++;
    }

    if (VargCount != 0)
    {
	XtSetValues((Widget ) current->swindow.vScrollBar,vSBArgs,VargCount);
	Resize(new);
        Flag = TRUE;
    }
    if (HargCount != 0)
    {
	XtSetValues((Widget ) current->swindow.hScrollBar,hSBArgs,HargCount);
	Resize(new);
        Flag = TRUE;
    }
    return (Flag);
 }

