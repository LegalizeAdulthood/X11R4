/*
 *	Original Source: ./lib/Xaw/Viewport.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

#ifndef lint
static char Xrcsid[] = "$XConsortium: Viewport.c,v 1.28 88/10/14 15:31:37 swick Exp $";
#endif lint
/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
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

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#include <X11/IntrinsicP.h>
#include <IntrinsicI.h>
#include <X11/XawMisc.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Core.h>
#include <St/StCom.h>
#include <St/StPpStViwP.h>
#include "StPpStViwI.h"


static void CreateShadow();
static void Setmainargs();
static void Settitleargs();
static void Setmarginargs();
static void Setcomargs();
static int MinMaxcheck();
static GC GetNormalGC(), GetGrayGC();
static void WindowAbleProc();
static void Redisplay();

static Boolean defFalse = False;


#define offset(field) XtOffset(StPopupSheetViewWidget,stpopupsheetview.field)
static XtResource resources[] = {
    {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
	 XtOffset(StPopupSheetViewWidget,core.border_width), XtRImmediate, (caddr_t)2 },
    {StNbottomMargin, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(bottommargin), XtRImmediate, (caddr_t)0 },
    {StNtopMargin, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(topmargin), XtRImmediate, (caddr_t)0 },
    {StNleftMargin, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(leftmargin), XtRImmediate, (caddr_t)0 },
    {StNrightMargin, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(rightmargin), XtRImmediate, (caddr_t)0 },
    {StNmaxHeight, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(maxheight), XtRImmediate, (caddr_t)65535 },
    {StNmaxWidth, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(maxwidth), XtRImmediate, (caddr_t)65535 },
    {StNminHeight, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(minheight), XtRImmediate, (caddr_t)0 },
    {StNminWidth, StCMargin, XtRDimension, sizeof(Dimension),
	 offset(minwidth), XtRImmediate, (caddr_t)0 },
    {StNtitle, StCtitle, XtRString, sizeof(String),
	 offset(titlename), XtRString, NULL },
    {StNtitleBackground, XtCBackground, XtRPixel, sizeof(Pixel),
	 offset(titlebackground), XtRString, "XtDefaultBackground" },
    {StNtitleBackgroundPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	 offset(titlebackgroundpixmap), XtRInt, (caddr_t)&XtUnspecifiedPixmap },
    {StNtitleBorderColor, XtCBorderColor, XtRPixel, sizeof(Pixel),
	 offset(titlebordercolor), XtRString, "XtDefaultForeground" },
    {StNtitleBorderPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	 offset(titleborderpixmap), XtRInt, (caddr_t)&XtUnspecifiedPixmap },
    {StNtitleBorderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
	 offset(titleborderwidth), XtRImmediate, (caddr_t)2 },
    {StNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	 offset(titlefont), XtRString, "XtDefaultFont" },
    {StNtitleJustify, XtCJustify, XtRJustify, sizeof(XtJustify),
	 offset(titlejustify), XtRImmediate, (caddr_t)XtJustifyCenter },
    {StNtitleForeground, XtCForeground, XtRPixel, sizeof(Pixel),
	 offset(titleforeground), XtRString, "XtDefaultForeground" },
    {StNwindowable, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(windowable), XtRImmediate, (caddr_t)True },
};
#undef offset

static void Initialize(), Destroy(), Resize(), Realize(), ChangeManaged();
static Boolean SetValues();
static XtGeometryResult GeometryManager(), PreferredGeometry();

#define superclass	(&compositeClassRec)
StPopupSheetViewClassRec stPopupSheetViewClassRec = {
  { /* core_class fields */
    /* superclass	  */	(WidgetClass) superclass,
    /* class_name	  */	"StPopupSheetView",
    /* widget_size	  */	sizeof(StPopupSheetViewRec),
    /* class_initialize	  */	NULL,
    /* class_part_init    */    NULL,
    /* class_inited	  */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	Realize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	Redisplay, 
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback_private	  */	NULL,
    /* tm_table    	  */	NULL,
    /* query_geometry     */    PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager	  */	GeometryManager,
    /* change_managed	  */	ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },
  { /* view_class fields */
    /* empty		  */	0
  }
};


WidgetClass stPopupSheetViewWidgetClass = (WidgetClass)&stPopupSheetViewClassRec;

static void CreateShadow(vw)
	StPopupSheetViewWidget vw;
{
	static XPoint shadowp[7];
	static XPoint shadowwp[8];

	if (!XtIsRealized(vw)) {
		return;
	}

	shadowp[0].x = ViewWwidth - ViewWshadowwidth; 
	shadowp[0].y = ViewWshadowwidth;
	shadowp[1].x = ViewWwidth - ViewWshadowwidth;
	shadowp[1].y = ViewWheight - ViewWshadowwidth;
	shadowp[2].x = ViewWshadowwidth;
	shadowp[2].y = ViewWheight - ViewWshadowwidth;
	shadowp[3].x = ViewWshadowwidth; 
	shadowp[3].y = ViewWheight; 
	shadowp[4].x = ViewWwidth;
	shadowp[4].y = ViewWheight;
	shadowp[5].x = ViewWwidth; 
	shadowp[5].y = ViewWshadowwidth;
	shadowp[6].x = ViewWwidth - ViewWshadowwidth; 
	shadowp[6].y = ViewWshadowwidth;


	shadowwp[0].x = ViewWwidth - ViewWshadowwidth; 
	shadowwp[0].y = ViewWshadowwidth;
	shadowwp[1].x = ViewWwidth - ViewWshadowwidth;
	shadowwp[1].y = ViewWheight - ViewWshadowwidth;
	shadowwp[2].x = ViewWshadowwidth;
	shadowwp[2].y = ViewWheight - ViewWshadowwidth;
	shadowwp[3].x = ViewWshadowwidth; 
	shadowwp[3].y = ViewWheight;
	shadowwp[4].x = ViewWwidth - ViewWshadowwidth;
	shadowwp[4].y = ViewWheight;
	shadowwp[5].x = ViewWwidth;
	shadowwp[5].y = ViewWheight - ViewWshadowwidth;
	shadowwp[6].x = ViewWwidth; 
	shadowwp[6].y = ViewWshadowwidth;
	shadowwp[7].x = ViewWwidth - ViewWshadowwidth; 
	shadowwp[7].y = ViewWshadowwidth;

	XFillRectangle(XtDisplay(vw), XtWindow(vw), 
		ViewWnormalGC, 0, 0,
		ViewWwidth, ViewWheight);

	if (ViewWwindowable) {
		XFillPolygon(XtDisplay(vw), XtWindow(vw), 
			ViewWgrayGC, shadowwp,
			XtNumber(shadowwp),
			Complex,CoordModeOrigin);
	}
	else {
		XFillPolygon(XtDisplay(vw), XtWindow(vw), 
			ViewWgrayGC, shadowp,
			XtNumber(shadowp),
			Complex,CoordModeOrigin);
	}
}


static GC GetNormalGC(vw, w)
	StPopupSheetViewWidget vw;
	Widget w;
{
	XGCValues	values;
	unsigned long mask;

	values.background = 0;
	mask = GCBackground ; 


	return XtGetGC((Widget)w, mask, &values);
}

static GC GetGrayGC(vw,w)
	StPopupSheetViewWidget vw;
	Widget w;
{
	XGCValues	values;

	values.background = 0;
	values.foreground = ViewWbordercolor;
	values.fill_style = FillTiled;
	values.tile       = XmuCreateStippledPixmap(XtScreen((Widget)w),
				BlackPixelOfScreen(XtScreen((Widget)w)),
						0,
						ViewWdepth);

	return XtGetGC((Widget)w, 
		               GCBackground |
		               GCForeground |
		               GCTile | 
		               GCFillStyle,
		   &values);
}


static void CreatecomWidget(w)
	Widget w;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
	static Arg comargs[10];
	Cardinal ac;

	ViewWwindcomwidth = 30 ; 
	ViewWwindcomheight = 30 ;

	Setcomargs(vw, comargs);
	ViewWwindcomW = (Widget)XtCreateManagedWidget("windcom", 
				 stComWidgetClass,
				 (Widget)vw, comargs, XtNumber(comargs));
}

static void Setmainargs(w, args)
	Widget w;
    	Arg *args;
{
	
	Cardinal ac;
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
    	Arg *mainargs = args;
    	
	ac = 0;
	XtSetArg(mainargs[ac], XtNwidth, ViewWmainwidth), 		  ac++;
	XtSetArg(mainargs[ac], XtNheight, ViewWmainheight), 	  	  ac++;
	XtSetArg(mainargs[ac], XtNbackground, ViewWbackground),	  ac++;
	XtSetArg(mainargs[ac], XtNbackgroundPixmap, ViewWbackgroundpixmap),ac++;
	XtSetArg(mainargs[ac], XtNborderColor, ViewWbordercolor),	  ac++;
	XtSetArg(mainargs[ac], XtNborderPixmap, ViewWborderpixmap), ac++;
	XtSetArg(mainargs[ac], XtNborderWidth, ViewWmainborder_width),   ac++;
}

static void Settitleargs(w, args)
	Widget w;
    	Arg *args;
{
	
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
    	Arg *labelargs = args;
	Cardinal ac;
	ac = 0;
	XtSetArg(labelargs[ac], XtNlabel, ViewWtitle), 	  ac++;
	XtSetArg(labelargs[ac], XtNbackground, ViewWtitlebackground),	  ac++;
	XtSetArg(labelargs[ac], XtNbackgroundPixmap, ViewWtitlebackgroundpixmap),ac++;
	XtSetArg(labelargs[ac], XtNborderColor, ViewWtitlebordercolor),	  ac++;
	XtSetArg(labelargs[ac], XtNborderPixmap, ViewWtitleborderpixmap), ac++;
	XtSetArg(labelargs[ac], XtNborderWidth, ViewWtitleborderwidth),   ac++;
	XtSetArg(labelargs[ac], XtNfont, ViewWtitlefont), 		  ac++;
	XtSetArg(labelargs[ac], XtNforeground, ViewWtitleforeground), 	  ac++;
	XtSetArg(labelargs[ac], XtNjustify, ViewWtitlejustify), 	  ac++;
	XtSetArg(labelargs[ac], XtNwidth, ViewWmainwidth), 		  ac++;
	XtSetArg(labelargs[ac], XtNheight, ViewWtitleheight), 		  ac++;
	XtSetArg(labelargs[ac], XtNx, ViewWtitleX), 			  ac++;
	XtSetArg(labelargs[ac], XtNy, ViewWtitleY), 			  ac++;
}

static void Setmarginargs(w, args)
	Widget w;
    	Arg *args;
{
	
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
    	Arg *marginargs = args;
	Cardinal ac;
	ac = 0;
	XtSetArg(marginargs[ac], XtNborderWidth, 0), 			  ac++;
	XtSetArg(marginargs[ac], XtNwidth, ViewWmarginwidth), 		  ac++;
	XtSetArg(marginargs[ac], XtNheight, ViewWmarginheight), 	  ac++;
	XtSetArg(marginargs[ac], XtNx, ViewWmarginX), 			  ac++;
	XtSetArg(marginargs[ac], XtNy, ViewWmarginY), 			  ac++;

}

static void Setcomargs(w, args)
	Widget w;
    	Arg *args;
{
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
    	Arg *comargs = args;
	Cardinal ac;
	static XtCallbackRec callbacks[] = {
		{WindowAbleProc, NULL},
		{NULL, NULL},
	};
	ac = 0;
	XtSetArg(comargs[ac], XtNbackground, (XtArgVal)ViewWbackground), 	ac++;
	XtSetArg(comargs[ac], XtNbackgroundPixmap, (XtArgVal)ViewWbackgroundpixmap), 	ac++;
	XtSetArg(comargs[ac], XtNwidth, (XtArgVal)ViewWwindcomwidth), 	ac++;
	XtSetArg(comargs[ac], XtNheight,(XtArgVal)ViewWwindcomheight), 	ac++;
	XtSetArg(comargs[ac], XtNx, 	(XtArgVal)ViewWwindcomX), 	ac++;
	XtSetArg(comargs[ac], XtNy, 	(XtArgVal)ViewWwindcomY), 	ac++;
	XtSetArg(comargs[ac], XtNborderWidth, (XtArgVal)ViewWmainborder_width), 		ac++;
	XtSetArg(comargs[ac], XtNforeground, (XtArgVal)ViewWbordercolor), 		ac++;
	XtSetArg(comargs[ac], XtNborderPixmap, (XtArgVal)ViewWborderpixmap), 		ac++;
	XtSetArg(comargs[ac], XtNcallback, (XtArgVal)callbacks), 	ac++;
	callbacks[0].closure = (caddr_t)vw;
}

static int MinMaxcheck(w, n, flag)
	Widget w;
    	Dimension n ;
	Boolean flag;
{
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
	if(flag){
		if((ViewWminwidth < n) && (n < ViewWmaxwidth)){
			return(TRUE);
		}
	}else{
		if((ViewWminheight < n) && (n < ViewWmaxheight)){
			return(TRUE);
		}
	}
	return(FALSE);
}

static void WindowAbleProc(w, closure, callData)
	Widget w;
	caddr_t closure; /* StPopupSheetViewWidget */
	caddr_t callData;
{
	Dimension rwidth,rheight;
    	XtWidgetGeometry request, reply;
    	XtGeometryResult result;
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)closure;
	ShellWidget shellW = (ShellWidget)XtParent(vw);

	shellW->shell.popped_up = FALSE;
	shellW->shell.spring_loaded = FALSE;

	XUngrabPointer(XtDisplay(w), CurrentTime);
	if(shellW->shell.grab_kind != XtGrabNone){
		XtRemoveGrab(shellW);
		shellW->shell.grab_kind = XtGrabNone;  
	}

	ViewWiswindow = TRUE;
	XtDestroyWidget(ViewWwindcomW);
	ViewWwindcomW = NULL;
	request.request_mode &= CWWidth | CWHeight;
	request.width = ViewWmainwidth ;
	request.height = ViewWmainheight ;
	XtResizeWidget( shellW, ViewWmainwidth + ViewWmainborder_width*2 ,
			    ViewWmainheight  + ViewWmainborder_width*2 ,  0  ); 
	XtMoveWidget(ViewWmainW, ViewWmainX , ViewWmainY );
	XtResizeWidget( ViewWmainW, ViewWmainwidth ,
			    ViewWmainheight  , ViewWmainborder_width ); 
}

/* ARGSUSED */

static void Initialize(request, new)
	Widget request, new;
{
	
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)new;
    	static Arg shellarg[1];
    	static Arg mainargs[7];
    	static Arg labelargs[13];
    	static Arg marginargs[5];
    	static Arg comargs[10];
	Cardinal n,ac;
	ShellWidget shellW ;
        ViewWmainW = ViewWtitleW = ViewWmainW = 
        ViewWmarginW = ViewWwindcomW =
        ViewWchildW =  (Widget)NULL;
	ViewWnormalGC = ViewWnormalGC = (GC)NULL;
        ViewWiswindow =  ViewWreparent = 
	ViewWsetval = FALSE;

        ViewWmainborder_width = ViewWborder_width;
        ViewWmainwidth = ViewWwidth ;
        ViewWmainheight = ViewWheight ;
    	ViewWshadowwidth =  Shadowwidth;
        ViewWwidth = ViewWwidth + ViewWmainborder_width*2 + ViewWshadowwidth;
        ViewWheight = ViewWheight + ViewWmainborder_width*2 + ViewWshadowwidth;
        ViewWborder_width = 0;
    
        ViewWnormalGC = GetNormalGC(vw,vw);
        ViewWgrayGC = GetGrayGC(vw,vw);

	shellW = (ShellWidget)XtParent(vw);
	XtSetArg(shellarg[0], XtNborderWidth, 0);
	XtSetValues(shellW, shellarg, 1); 

	Setmainargs(vw, mainargs);
	ViewWmainW = (Widget)XtCreateManagedWidget("main", windowObjClass,
			(Widget)vw, mainargs, XtNumber(mainargs));
        if(ViewWtitle){
		Settitleargs(vw, labelargs);
		ViewWtitleW = (Widget)XtCreateManagedWidget("title", labelWidgetClass,
			(Widget)vw, labelargs, XtNumber(labelargs));
	}else{
		ViewWtitleborderwidth = 0;
	}
	Setmarginargs(vw, marginargs);
	ViewWmarginW = (Widget)XtCreateManagedWidget("margin", widgetClass,
			(Widget)vw, marginargs, XtNumber(marginargs));
}

static void Realize(widget, value_mask, attributes)
	Widget widget;
	XtValueMask *value_mask;
	XSetWindowAttributes *attributes;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)widget;


	*value_mask |= CWBitGravity;
	attributes->bit_gravity = NorthWestGravity;
	(*superclass->core_class.realize)(widget, value_mask, attributes);

	XtRealizeWidget( ViewWmainW );
	if(ViewWtitle != NULL){
		XtRealizeWidget( ViewWtitleW );
	}
	XtRealizeWidget( ViewWmarginW );

        if (ViewWchildW) {
		XtMoveWidget( ViewWchildW, (Position)0, (Position)0 );
		XtRealizeWidget( ViewWchildW );
		XReparentWindow( XtDisplay(ViewWchildW), XtWindow(ViewWchildW), 
			XtWindow(ViewWmarginW),
			 (Position)0, (Position)0 );
		XtMapWidget( ViewWchildW );

        }
	if(ViewWwindowable && !ViewWiswindow){
		CreatecomWidget(vw);
		XtRealizeWidget( ViewWwindcomW );
	}
}

/* ARGSUSED */
static void Redisplay(w, event, region)
	Widget w;
	XEvent *event;	
	Region region;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;
	Resize(vw);

}

static void Destroy(w)
	Widget w;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)w;


	if (ViewWnormalGC != (GC)NULL) {
		XtReleaseGC(XtDisplay(vw), ViewWnormalGC);
		ViewWnormalGC = (GC)NULL;
	}
	if (ViewWgrayGC != (GC)NULL) {
		XtReleaseGC(XtDisplay(vw), ViewWgrayGC);
		ViewWgrayGC = (GC)NULL;
	}
}

/* ARGSUSED */
static Boolean SetValues(current, request, new)
	Widget current, request, new;
{
        StPopupSheetViewWidget vw = (StPopupSheetViewWidget)new;
        StPopupSheetViewWidget cw = (StPopupSheetViewWidget)current;
    	static Arg mainargs[7];
    	static Arg labelargs[13];
    	static Arg marginargs[5];
    	static Arg comargs[10];
	Cardinal n,ac;
	ShellWidget shellW = (ShellWidget)XtParent(vw);
    
        if (vw->core.border_width != cw->stpopupsheetview.mainborder_width ){
        	ViewWmainborder_width = ViewWborder_width;
        	ViewWborder_width = 0;
	}
        if (vw->core.width != cw->stpopupsheetview.mainwidth ||
        	vw->core.height != cw->stpopupsheetview.mainheight ){
        	if (!ViewWiswindow && ViewWwindowable){
    			ViewWshadowwidth = Shadowwidth;
		}
        	if (ViewWiswindow || !ViewWwindowable){
    			ViewWshadowwidth = 0 ;
		}
        	if (vw->core.width != cw->stpopupsheetview.mainwidth ){
			if(MinMaxcheck(vw, vw->core.width, TRUE)){
        			ViewWmainwidth = ViewWwidth;
        			ViewWwidth = ViewWwidth + ViewWshadowwidth + ViewWmainborder_width*2;
			}
		}
        	if (vw->core.height != cw->stpopupsheetview.mainheight ){
			if(MinMaxcheck(vw, vw->core.height, FALSE)){
        			ViewWmainheight = ViewWheight;
        			ViewWheight = ViewWheight + ViewWshadowwidth + ViewWmainborder_width*2;
			}
		}
		ViewWsetval = TRUE;
	}
	XtResizeWidget( shellW, ViewWwidth ,
			    ViewWheight  ,  0  ); 
	Setmainargs(vw, mainargs);
	XtSetValues(ViewWmainW, mainargs, XtNumber(mainargs)); 
	XtResizeWidget( ViewWmainW, ViewWmainwidth ,
			    ViewWmainheight  ,  ViewWmainborder_width); 
	 
        if (vw->stpopupsheetview.windowable != cw->stpopupsheetview.windowable && vw->stpopupsheetview.windowable  == FALSE) {
	        XtDestroyWidget(ViewWwindcomW);
	        ViewWwindcomW = NULL;	
	        ViewWiswindow = FALSE;	
	}
        if (vw->stpopupsheetview.windowable != cw->stpopupsheetview.windowable && vw->stpopupsheetview.windowable  == TRUE && !ViewWiswindow) {
		CreatecomWidget(vw);
		XtRealizeWidget( ViewWwindcomW );
	}
        if (vw->stpopupsheetview.titlename != cw->stpopupsheetview.titlename && vw->stpopupsheetview.titlename == NULL) {
	        XtDestroyWidget(ViewWtitleW);
	        ViewWtitleW = NULL;	
	        ViewWtitleborderwidth = 0;	
	}
        if (vw->stpopupsheetview.titlename != cw->stpopupsheetview.titlename && cw->stpopupsheetview.titlename == NULL) {
		Settitleargs(vw, labelargs);
		ViewWtitleW = (Widget)XtCreateManagedWidget("title", labelWidgetClass,
			(Widget)vw, labelargs, XtNumber(labelargs));
		XtRealizeWidget( ViewWtitleW );
	}
	if(ViewWtitle != NULL && cw->stpopupsheetview.titlename != NULL ){
		Settitleargs(vw, labelargs);
		XtSetValues(ViewWtitleW, labelargs, XtNumber(labelargs)); 
	}

	Setmarginargs(vw, marginargs);
	XtSetValues(ViewWmarginW, marginargs, XtNumber(marginargs)); 
	if(ViewWwindowable && !ViewWiswindow && cw->stpopupsheetview.windcomW != NULL)
	{
		Setcomargs(vw, comargs);
		XtSetValues(ViewWwindcomW, comargs, XtNumber(comargs)); 

	}

	return TRUE;
}


static void ChangeManaged(widget)
	Widget widget;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)widget;
	register int num_children = vw->composite.num_children;
	register Widget child, *childP;
	register int i;

	child = (Widget)NULL;
	for (childP=vw->composite.children, i=0; i < num_children; childP++, i++) {
		if (XtIsManaged(*childP)
	    		&& *childP != ViewWmainW
	    		&& *childP != ViewWtitleW
	    		&& *childP != ViewWmarginW
	    		&& *childP != ViewWwindcomW)
		{
        		child = *childP;
       	        	break;
		}
	}

	if (child != ViewWchildW) {
		ViewWchildW = child;
		if (child) {
	    		XtResizeWidget( child, ViewWmarginwidth ,
				    ViewWmarginheight , 0 );
			if (!XtIsRealized(child)) { 
		    		XtMoveWidget( child, 0, 0 );
#ifdef notdef
		    /* this is dirty, but it saves the following code: */
		    		XtRealizeWidget( child );
		    		XReparentWindow( XtDisplay(vw), XtWindow(child),
				     XtWindow(ViewWmarginW),
				     (Position)0, (Position)0 );
		       		if (child->core.mapped_when_managed)
					XtMapWidget( child );
#else notdef
		    		if (XtIsRealized(vw)) {
			    		XtRealizeWidget( child );
		    		}
#endif notdef
		    		ViewWreparent = True;
			}
			else if (!ViewWreparent) { 
		    		XReparentWindow( XtDisplay(vw), XtWindow(child),
				     	XtWindow(ViewWmarginW),
				        (Position)0, (Position)0 );
		    		ViewWreparent = True;
		    		if (child->core.mapped_when_managed)
					XtMapWidget( child );
		 	} 
		   	XtMoveWidget( child, 0,
				    0 );
		}
	}

#ifdef notdef
	(*superclass->composite_class.change_managed)( widget );
#endif
}



static void Resize(widget)
	Widget widget;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)widget;
	ShellWidget shellW = (ShellWidget)XtParent(vw);
	int n;

	ViewWshadowwidth = ViewWiswindow ? 0 : Shadowwidth;
	if(ViewWsetval){
    		ViewWwidth =  ViewWiswindow ? (ViewWmainwidth + ViewWmainborder_width*2) : (ViewWmainwidth + ViewWmainborder_width*2 + Shadowwidth);
    		ViewWheight =  ViewWiswindow ? (ViewWmainheight + ViewWmainborder_width*2) : (ViewWmainheight + ViewWmainborder_width*2 + Shadowwidth);
		ViewWsetval = FALSE;
	}
	if((ViewWminwidth < (ViewWiswindow ? (ViewWwidth - ViewWmainborder_width*2) : (ViewWwidth - ViewWmainborder_width*2 - Shadowwidth))) &&
	((ViewWiswindow ? (ViewWwidth - ViewWmainborder_width*2) : (ViewWwidth - ViewWmainborder_width*2 - Shadowwidth)) < ViewWmaxwidth)){
    		ViewWmainwidth =  ViewWiswindow ? (ViewWwidth - ViewWmainborder_width*2) : (ViewWwidth - ViewWmainborder_width*2 - Shadowwidth);
	}else{
    		ViewWwidth =  ViewWiswindow ? (ViewWmainwidth + ViewWmainborder_width*2) : (ViewWmainwidth + ViewWmainborder_width*2 + Shadowwidth);
	}
	if((ViewWminheight < (ViewWiswindow ? (ViewWheight - ViewWmainborder_width*2) : (ViewWheight - ViewWmainborder_width*2 - Shadowwidth))) &&
	((ViewWiswindow ? (ViewWheight - ViewWmainborder_width*2) : (ViewWheight - ViewWmainborder_width*2 - Shadowwidth)) < ViewWmaxheight)){
    		ViewWmainheight =  ViewWiswindow ? (ViewWheight - ViewWmainborder_width*2) : (ViewWheight - ViewWmainborder_width*2 - Shadowwidth);
	}else{
    		ViewWheight =  ViewWiswindow ? (ViewWmainheight + ViewWmainborder_width*2) : (ViewWmainheight + ViewWmainborder_width*2 + Shadowwidth);
	}
	XtResizeWidget( shellW, ViewWwidth ,
			    ViewWheight  ,  0  ); 
	XtMoveWidget(ViewWmainW, ViewWmainX, ViewWmainY);
	XtResizeWidget(ViewWmainW, ViewWmainwidth ,
				ViewWmainheight , ViewWmainborder_width);
	if(ViewWtitle != NULL){
		static Arg titlearg[1];

		XtMoveWidget(ViewWtitleW, ViewWtitleX, ViewWtitleY);
		XtResizeWidget(ViewWtitleW, ViewWtitlewidth,
				ViewWtitleheight, ViewWtitleborderwidth);
		XtSetArg(titlearg[0], XtNjustify, ViewWtitlejustify);
	}
	XtMoveWidget(ViewWmarginW, ViewWmarginX, ViewWmarginY);
	XtResizeWidget(ViewWmarginW, ViewWmarginwidth, 
			ViewWmarginheight,  ViewWchildW->core.border_width ); 

	if(ViewWwindowable && !ViewWiswindow)
	{
		XtMoveWidget(ViewWwindcomW, ViewWwindcomX, ViewWwindcomY);
	}


	if (ViewWchildW) {
		XtResizeWidget( ViewWchildW, (Dimension)ViewWmarginwidth,
		       	 (Dimension)ViewWmarginheight, (Dimension)0 );
		XtMoveWidget(ViewWchildW, 0, 0);
	}
	if(!ViewWiswindow){
		CreateShadow(vw);
	}
}


/* Semi-public routines */

static XtGeometryResult GeometryManager(child, request, reply)
	Widget child;
	XtWidgetGeometry *request, *reply;
{
	StPopupSheetViewWidget vw = (StPopupSheetViewWidget)child->core.parent;
	Boolean rWidth = (Boolean)(request->request_mode & CWWidth);
	Boolean rHeight = (Boolean)(request->request_mode & CWHeight);
	XtWidgetGeometry allowed, myrequest;
	XtGeometryResult result;

	if (child != vw->stpopupsheetview.childW
	    || request->request_mode & ~(CWWidth | CWHeight | CWBorderWidth)
	    || ((request->request_mode & CWBorderWidth)
	    && request->border_width > 0))
		return XtGeometryNo;

	allowed = *request;
	result = XtGeometryYes;


	if (rWidth && ViewWmarginwidth != request->width) {
		if ( request->width  > ViewWmarginwidth)
		{
		    allowed.width = ViewWmarginwidth ;
		    result = XtGeometryAlmost;
		}
	}
	if (rHeight && ViewWmarginheight != request->height) {
		if ( request->height  > ViewWmarginheight)
		{
	   		allowed.height = ViewWmarginheight ;
	    		result = XtGeometryAlmost;
		}
	}
	*reply = allowed;
	if (result == XtGeometryYes) {
		if (rWidth)  child->core.width = request->width;
		if (rHeight) child->core.height = request->height;
		myrequest.request_mode = 0;
	}

	return result;
}

static XtGeometryResult PreferredGeometry(vw, constraints, reply)
	Widget vw;
	XtWidgetGeometry *constraints, *reply;
{
	if (((StPopupSheetViewWidget)vw)->stpopupsheetview.childW != NULL)
		return XtQueryGeometry( ((StPopupSheetViewWidget)vw)->stpopupsheetview.childW,
	        constraints, reply );
	else
		return XtGeometryYes;
}
