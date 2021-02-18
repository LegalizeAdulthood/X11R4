/*
 *	Original Source: ./lib/Xaw/Command.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

#ifndef lint
static char Xrcsid[] = "$XConsortium: Command.c,v 1.46 88/11/01 16:57:33 swick Exp $";
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

/*
 * StCom.c - StCom widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <IntrinsicI.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <ctype.h>
#include <X11/XawMisc.h>
#include <St/StComP.h>
#include "StComI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] =
    " Button1<EnterWindow>:	set()			\n\
      <LeaveWindow>:		unset()			\n\
     <Btn1Down>:		set()			\n\
     <Btn1Up>:			notify() 		";

#define offset(field) XtOffset(StComWidget, field)
static XtResource resources[] = { 

   {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      offset(core.border_width), XtRImmediate, (caddr_t) 0},
   {XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t), 
      offset(com.callbacks), XtRCallback, (caddr_t)NULL},
};
#undef offset

static XtActionsRec actionsList[] =
{
  {"set",		Set},
  {"notify",		Notify},
  {"unset",		Unset},
};

#define SuperClass ((LabelWidgetClass)&labelClassRec)

StComClassRec stComClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StCom",				/* class_name		  */
    sizeof(StComRec),			/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    XtInheritResize,			/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    defaultTranslations,		/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* LabelClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* StComClass fields initialization */
};

  /* for public consumption */
WidgetClass stComWidgetClass = (WidgetClass) &stComClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/


static GC GetNormalGC(cw)
StComWidget cw;
{
    XGCValues	values;
    unsigned long mask;

    values.background = ComWbackground;
    values.foreground = ComWforeground;
    values.line_width   = ComWline_width ;
    values.tile       = ComWlinepixmap;
    if (ComWlinepixmap > XtUnspecifiedPixmap) {
    	   mask = GCBackground | 
		  GCForeground | 
		  GCLineWidth | 
		  GCTile | 
		  GCFillStyle;
	    values.fill_style = FillTiled;
    }
    else {
    	   mask = GCBackground | 
		  GCForeground | 
		  GCLineWidth | 
		  GCFillStyle;
	    values.fill_style = FillSolid;
    }


    return XtGetGC((Widget)cw, mask, &values);
}

static GC GetGrayGC(cw)
StComWidget cw;
{
    XGCValues	values;

    values.background = ComWbackground;
    values.foreground = ComWforeground;
    values.fill_style = FillTiled;
    values.tile       = XmuCreateStippledPixmap(XtScreen((Widget)cw),
				BlackPixelOfScreen(XtScreen((Widget)cw)),
						0,
						ComWdepth);

    return XtGetGC((Widget)cw, 
		               GCBackground |
		               GCForeground |
		               GCTile | 
		               GCFillStyle,
		   &values);
}


static void CreatecomPixmap(w,drawable,is)
Widget w;
Drawable drawable;
Boolean is;
{
	StComWidget cw = (StComWidget)w;
	XPoint points[4], pointss[7];
	static Arg bitmaparg[2];
	Cardinal ac;
	Dimension shadowwidth = 10;

	Dimension round = ComWline_width%2 ? 1 : 0;

	ac = 0;
	points[ac].x = ComWwidth - shadowwidth - ComWline_width ;
	points[ac].y = ComWline_width/2 - round , 	ac++;
	points[ac].x = ComWline_width/2 - round;
	points[ac].y = ComWline_width/2 - round, 					ac++;
	points[ac].x = ComWline_width/2 - round ;
	points[ac].y = ComWheight - shadowwidth - ComWline_width ,		ac++;
	points[ac].x = ComWwidth - shadowwidth - ComWline_width ;
	points[ac].y = ComWline_width/2 - round , 	ac++;

	ac = 0;
	pointss[ac].x = ComWwidth - shadowwidth ;
	pointss[ac].y = 0 , 	ac++;
	pointss[ac].x = 0 ;
	pointss[ac].y = ComWheight - shadowwidth ,		ac++;
	pointss[ac].x = 0 ;
	pointss[ac].y = ComWheight ,		ac++;
	pointss[ac].x = 3 ;
	pointss[ac].y = ComWheight ,		ac++;
	pointss[ac].x = ComWwidth ;
	pointss[ac].y = 3 , 	ac++;
	pointss[ac].x = ComWwidth ;
	pointss[ac].y = 0 , 	ac++;
	pointss[ac].x = ComWwidth - shadowwidth ;
	pointss[ac].y = 0 , 	ac++;
        ComWnormalGC = GetNormalGC(cw);
        ComWgrayGC = GetGrayGC(cw);
	XFillPolygon(XtDisplay(cw), drawable, 
		ComWgrayGC, pointss, XtNumber(pointss),Complex,CoordModeOrigin);

	XDrawLines(XtDisplay(cw), drawable, 
		ComWnormalGC, points, XtNumber(points),CoordModeOrigin);

}

static void Initialize(request, new)
 Widget request, new;
{
    StComWidget cw = (StComWidget) new;
    Widget parent = XtParent((Widget)cw);

    ComWline_width = ComWborder_width ;	
    ComWlinepixmap = ComWborderpixmap ;	
    ComWborder_width = 0;	
    ComWset = FALSE;
    ComWfirst = TRUE;
}

/***************************
*
*  Action Procedures
*
***************************/

/* ARGSUSED */
static void Set(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  StComWidget cw = (StComWidget)w;
  ComWset = TRUE;
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  StComWidget cw = (StComWidget)w;
  if ( ComWset == TRUE ){
	ComWset = FALSE;
  }
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  StComWidget cw = (StComWidget)w;
  if(ComWset){
	ComWswitchon = TRUE;
  	XtCallCallbacks(w, XtNcallback, ComWswitchon);
  }
}

/*
 * Repaint the widget window
 */

/************************
*
*  REDISPLAY (DRAW)
*
************************/

/* ARGSUSED */
static void Redisplay(w, event, region)
	Widget w;
	XEvent *event;	
	Region region;
{

	StComWidget cw = (StComWidget)w;
	Drawable drawable;
	GC gc;


	if(ComWfirst) {
		
		ComWnormalShape = XCreatePixmap(XtDisplay(w), XtWindow(w),
				ComWwidth, ComWheight, ComWdepth);
		CreatecomPixmap(w, ComWnormalShape, FALSE);
	

		ComWfirst = FALSE;

	}
	drawable = ComWnormalShape;
	
	gc = ComWnormalGC;
	XCopyArea(XtDisplay(w), drawable, XtWindow(w), gc, 0, 0,
			ComWwidth, ComWheight, 0, 0);

}



static void FreeWdigetImage(w)
	Widget w;
{
	StComWidget cw = (StComWidget)w;

	if (!ComWfirst) {
		XFreePixmap(XtDisplay(w), ComWnormalShape);
		ComWfirst = TRUE;
	}
}



/* ARGSUSED */
static void Destroy(w)
    Widget w;
{
    StComWidget cw = (StComWidget)w;


    FreeWdigetImage(w);
}


/*
 * Set specified arguments into widget
 */
/* ARGSUSED */
static Boolean SetValues (current, request, new)
    Widget current, request, new;
{
    StComWidget oldbw = (StComWidget) current;
    StComWidget reqbw = (StComWidget) request;
    StComWidget cw = (StComWidget) new;
    Boolean redisplay = FALSE;

    if (XtLField(oldbw,foreground) != ComWforeground ||
	XtCField(oldbw, background_pixel) != ComWbackground ||
	XtCField(oldbw, border_width) != ComWborder_width ||
	XtCField(oldbw, border_pixel) != ComWborderpixel ||
	XtCField(oldbw, border_pixmap) != ComWborderpixmap )
    	{
		if (XtCField(oldbw, border_width) != ComWborder_width){
			ComWline_width = ComWborder_width;
			ComWborder_width = 0;
		}


		if (XtCField(oldbw, border_pixmap) != ComWborderpixmap){
			ComWlinepixmap = ComWborderpixmap;
		}

	XtReleaseGC(new, ComWnormalGC);
	ComWnormalGC = GetNormalGC(cw);

	XtReleaseGC(new, ComWgrayGC);
	ComWgrayGC = GetGrayGC(cw);



	redisplay = TRUE;
    }
    if (XtCField(reqbw, width) != ComWwidth ||
        XtCField(reqbw, height) != ComWheight) {
		ComWwidth = reqbw->core.width;
		ComWheight = reqbw->core.height;
	redisplay = TRUE;
    }

    if (XtCField(oldbw, width) != ComWwidth || 
        XtCField(oldbw, height) != ComWheight) {

	redisplay = TRUE;
    }

    if (redisplay == TRUE) {

        FreeWdigetImage(cw);    
    }

    return (redisplay ||
	    XtSField(oldbw, switchon) != ComWswitchon );
}
