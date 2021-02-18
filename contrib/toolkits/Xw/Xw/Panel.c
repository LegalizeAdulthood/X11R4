/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        Panel.c
 **
 **   Project:     X Widgets
 **
 **   Description: Panel Manager Widget code and class record
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




#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/Panel.h>
#include <Xw/PanelP.h>

/* Public Procedures -- Class Methods */

static void Initialize();
static void Realize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();
static XtGeometryResult GeometryManager();
static void ChangeManaged();
static void InsertChild();
static void DeleteChild();
static void ConstraintInitialize();
static Boolean ConstraintSetValues();

/* Private Procedures */
static void Layout();
static void GetMinSize();
static XtGeometryResult AdjustWS();

/*************************************<->*************************************
 *
 *   Description:  default translation table for class: Panel Manager
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
 *   Description:  action list for class: Panel
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *
 *************************************<->***********************************/
static XtActionsRec actionsList[] = NULL;


/* Resource definitions and defaults for Panel */

static XtResource resources[] = 
{
	{ XtNhSpace,
		XtCHSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwPanelWidget, panel.internal_width),
		XtRString,
		"0"
	},
	{ XtNvSpace,
		XtCVSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwPanelWidget, panel.internal_height),
		XtRString,
		"0"
	},
	{ XtNtitleToMenuPad,
		XtCTitleToMenuPad,
		XtRInt,
		sizeof(int),
		XtOffset(XwPanelWidget, panel.title_to_pulldown),
		XtRString,
		"0"
	},
	{ XtNworkSpaceToSiblingPad,
		XtCWorkSpaceToSiblingPad,
		XtRInt,
		sizeof(int),
		XtOffset(XwPanelWidget, panel.wspace_to_sibling),
		XtRString,
		"0"
	},
	{ XtNtopLevel,
		XtCTopLevel,
		XtRBoolean,
		sizeof(Boolean),
		XtOffset(XwPanelWidget, panel.top_level),
		XtRString,
		"TRUE"
	},
	{ XtNdisplayTitle,
		XtCDisplayTitle,
		XtRBoolean,
		sizeof(Boolean),
		XtOffset(XwPanelWidget, panel.display_title),
		XtRString,
		"TRUE"
	}
};


/* Constraint resource definitions and defaults for children of Panel */

static XtResource constraintResources[] = 
{
	{ XtNwidgetType,
		XtCWidgetType,
		XtRWidgetType,
		sizeof(XwWidgetType),
		XtOffset(XwPanelConstraints,type),
		XtRString,
		"unknown"
	},
	{
		XtNcausesResize,
		XtCCausesResize,
		XtRBoolean,
		sizeof(Boolean),
		XtOffset(XwPanelConstraints, causes_resize),
		XtRString,
		"TRUE"
	}
};


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

XwPanelClassRec XwpanelClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &XwmanagerClassRec,
    /* class_name         */    "Panel",
    /* widget_size        */    sizeof(XwPanelRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,				
    /* class_inited       */	FALSE,
    /* initialize         */    (XtInitProc) Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    (XtRealizeProc) Realize,
    /* actions		      */	actionsList,
    /* num_actions	      */	XtNumber(actionsList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterlv   */    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    (XtWidgetProc) Resize,
    /* expose             */    (XtExposeProc) Redisplay,
    /* set_values         */    (XtSetValuesFunc) SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */	(XtGeometryHandler) QueryGeometry, 
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    (XtGeometryHandler) GeometryManager,
    /* change_managed     */    (XtWidgetProc) ChangeManaged,
    /* insert_child	  */	    (XtArgsProc) InsertChild,
    /* delete_child	  */	    (XtArgsProc) DeleteChild,
				NULL,
  },{
/* constraint_class fields */
   /* resource list        */   constraintResources,
   /* num resources        */   XtNumber(constraintResources),
   /* constraint size      */   sizeof(XwPanelConstraintRec),
   /* init proc            */   (XtInitProc) ConstraintInitialize,
   /* destroy proc         */   NULL,
   /* set values proc      */   (XtSetValuesFunc) ConstraintSetValues,
				NULL
  },{
/* manager_class fields */
   /* traversal handler   */    (XwTraversalProc) XtInheritTraversalProc,
   /* translations        */	NULL,
  },{
/* bulletin board class - none */     
    /* mumble  */               0
 }	
};

WidgetClass XwpanelWidgetClass = (WidgetClass)&XwpanelClassRec;


/*************************************<->*************************************
 *
 *	static void Initialize(req,new)
 *		XwPanelWidget	req, new;
 *
 *   Description:
 *   -----------
 *		The initialize procedure for this widget.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void Initialize(req,new)
	XwPanelWidget	req, new;
{
	XwPanelPart	*pp;

	pp = &(new->panel);

	/*
	 * Make the private data "safe"
	 */
	pp->title = NULL;
	pp->pulldown = NULL;
	pp->work_space = NULL;
	pp->title_on = (pp->top_level ? pp->display_title : TRUE);

	/*
	 * Get a legal size.
	 */
	if (pp->internal_width < 0)
	{
		XtWarning("XwPanelWidget: HSpace cannot be less than zero.  Setting to 0.");
		pp->internal_width = 0;
	}
	if (pp->internal_height < 0)
	{
		XtWarning("XwPanelWidget: VSpace cannot be less than zero.  Setting to 0.");
		pp->internal_height = 0;
	}
	
	if (!new->core.height)
		new->core.height = (pp->internal_height ? 
			(2 * pp->internal_height) : 100);
	
	if (!new->core.width)
		new->core.width = (pp->internal_width ? 
			(2 * pp->internal_width) : 100);
}

/*************************************<->*************************************
 *
 *	static void Resize(pw)
 *		XwPanelWidget	pw;
 *
 *   Description:
 *   -----------
 *		Called to resize the Panel.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void Resize(pw)
	XwPanelWidget	pw;
{
	XtWidgetGeometry	new, rep;
	XtGeometryResult	q_res;
	XwPanelConstraintRec	*pcp;

	if ((pw->panel.pulldown) && (pw->panel.pulldown->core.managed))
	{
		new.request_mode = CWWidth;
		new. width = pw->core.width - (2 * (pw->panel.internal_width + 
		pw->panel.work_space->core.border_width));
		q_res = XtQueryGeometry(pw->panel.pulldown,&new,&rep);
		if ((q_res != XtGeometryNo) &&
			(rep.request_mode & CWHeight))
		{
			/*
			 * We need to size the widget.  Layout will handle
			 * the position.
			 */
			XtResizeWidget(pw->panel.pulldown,new.width,rep.height,
				pw->panel.pulldown->core.border_width);
			pcp = (XwPanelConstraintRec *) 
				pw->panel.pulldown->core.constraints;
			pcp->req_width = (int)pw->panel.pulldown->core.width;
			pcp->req_height = (int)pw->panel.pulldown->core.height;
		}
	}
	Layout(pw);
}

/*************************************<->*************************************
 *
 *	static Boolean SetValues(cur,req,new)
 *		XwPanelWidget	cur, req, new;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static Boolean SetValues(cur,req,new)
	XwPanelWidget	cur, req, new;
{
	XwPanelPart	*pp;
	int	min_w, min_h;
	Boolean	flag = FALSE;

	pp = &(new->panel);
	pp->title_on = (pp->top_level ? pp->display_title : TRUE);

	if (new->panel.internal_width != cur->panel.internal_width)
		if (pp->internal_width < 0)
		{
			XtWarning("XwPanelWidget: HSpace cannot be less than zero.  Setting to 0.");
			pp->internal_width = 0;
		}
		else
		{
			new->core.width = 2 * pp->internal_width;
			flag = TRUE;
		}

	if (new->panel.internal_width != cur->panel.internal_width)
		if (pp->internal_height < 0)
		{
			XtWarning("XwPanelWidget: VSpace cannot be less than zero.  Setting to 0.");
			pp->internal_height = 0;
		}
		else
		{
			new->core.height = 2 * pp->internal_height;
			flag = TRUE;
		}
	
	if (req->manager.layout != XwIGNORE)
		GetMinSize(new,&min_w,&min_h);

	if ((req->core.width == cur->core.width) &&
		(req->manager.layout != XwIGNORE))
		new->core.width = min_w;
	else
		new->core.width = req->core.width;
	if ((req->core.height == cur->core.height) &&
		(req->manager.layout != XwIGNORE))
		new->core.height = min_h;
	else
		new->core.height = req->core.height;
	
	return(flag);
}

/*************************************<->*************************************
 *
 * static void GetMinSize(pw,w,h)
 *		XwPanelWidget	pw;
 *		int	*w, *h;
 *
 *   Description:
 *   -----------
 *		Determins Panels optimum size given the current children.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void GetMinSize(pw,w,h)
	XwPanelWidget	pw;
	int	*w, *h;
{
	int t_w, m_w, w_w, min_h, min_w;
	XwPanelConstraintRec	*tpcp, *mpcp, *wpcp;
	Widget	title, pulldown, work_space;
	XwPanelPart	*pp;

	pp = &(pw->panel);

	/*
	 * Only process the title if it is flagged for display.
	 */
	title = ((pp->title && pp->title_on && pp->title->core.managed) 
		? pp->title : NULL);
	pulldown = ((pp->pulldown && pp->pulldown->core.managed)
		? pp->pulldown : NULL);
	work_space = ((pp->work_space && pp->work_space->core.managed) 
		? pp->work_space : NULL);
	
	/*
	 * Get the constraint records.
	 */
	tpcp = (XwPanelConstraintRec *) 
		(title ? title->core.constraints : NULL);
	mpcp = (XwPanelConstraintRec *) 
		(pulldown ? pulldown->core.constraints : NULL);
	wpcp = (XwPanelConstraintRec *) 
		(work_space ? work_space->core.constraints : NULL);
	
	/*
	 * Calculate the height based on the children.
	 */
	min_h = (2 * pp->internal_height) +
		(title ?
			(tpcp->req_height + (2 * tpcp->req_border_width)) : 0) +
		(pulldown ? 
			(mpcp->req_height + (2 * mpcp->req_border_width)) : 0) + 
		(work_space ?
			wpcp->req_height + (2 * wpcp->req_border_width): 0);

	if ((title && pp->title_on) && pulldown)
		min_h += pp->title_to_pulldown;

	if (work_space && ((title && pp->title_on) || pulldown))
			min_h += pp->wspace_to_sibling;

	/*
	 * Never return a zero.
	 */
	min_h = ((min_h > 0) ? min_h : 1);

	/*
	 * Get the children's widths.
	 */
	t_w = (title ? 
		tpcp->req_width + 
			(2 * title->core.border_width) :
		0);
	m_w = (pulldown ? 
		mpcp->req_width + 
			(2 * pulldown->core.border_width) :
		0);
	w_w = (work_space ? 
		wpcp->req_width + 
			(2 * work_space->core.border_width) :
		0);

	/*
	 * Find the largest.
	 */
	if ((t_w >= m_w) && (t_w >= w_w))
		min_w = t_w;
	else if ((m_w > t_w) && (m_w >= w_w))
		min_w = m_w;
	else if ((w_w > t_w) && (w_w > m_w))
		min_w = w_w;
	
	/*
	 * Add in the padding.
	 */
	min_w += 2 * pp->internal_width;

	/*
	 * Never return a zero.
	 */
	min_w = ((min_w > 0) ? min_w : 1);

	/*
	 * Assign the return variables.
	 */
	*w = min_w;
	*h = min_h;
}

/*************************************<->*************************************
 *
 *	static XtGeometryResult QueryGeometry(pw,intended,reply)
 *		XwPanelWidget	pw;
 *		XtWidgetGeometry	*intended, *reply;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static XtGeometryResult QueryGeometry(pw,intended,reply)
	XwPanelWidget	pw;
	XtWidgetGeometry	*intended, *reply;
{
	XwPanelPart	*pp;
	int min_h, min_w;
	XtGeometryMask	mode;

	/*
	 * See how big that we'e like to be.
	 */
	GetMinSize(pw, &min_w, &min_h);

	/*
	 * Process the query.
	 */
	if ((mode & CWWidth) && (mode & CWHeight))
	{
		if ((intended->width >= min_w) && (intended->height >= min_h))
		{
			reply->width = intended->width;
			reply->height = intended->height;
			return(XtGeometryYes);
		}
		else if ((intended->width < min_w) && 
			(intended->height < min_h))
		{
			reply->width = min_w;
			reply->height = min_h;
			return(XtGeometryNo);
		}
		else if (intended->width >= min_w)
		{
			reply->width = intended->width;
			reply->height = min_h;
			return(XtGeometryAlmost);
		}
		else if (intended->height >= min_h)
		{
			reply->height = intended->height;
			reply->width = min_w;
			return(XtGeometryAlmost);
		}
	}
	else if (mode & CWWidth)
	{
		if (intended->width >= min_w)
		{
			reply->width = intended->width;
			reply->height = min_h;
			return(XtGeometryYes);
		}
		else
		{
			reply->width = min_w;
			reply->height = min_h;
			return(XtGeometryNo);
		}
	}
	else if (mode & CWHeight)
	{
		if (intended->height >= min_h)
		{
			reply->height = intended->height;
			reply->width = min_w;
			return(XtGeometryYes);
		}
		else
		{
			reply->width = min_w;
			reply->height = min_h;
			return(XtGeometryNo);
		}
	}
	else
		return(XtGeometryYes);
}

/*************************************<->*************************************
 *
 *	static void UpdateWidgetAccordingToFlags(w,req)
 *	Widget		w;
 *	XtWidgetGeometry	*req;
 *
 *   Description:
 *   -----------
 *		Bind req to w.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void UpdateWidgetAccordingToFlags(w,req)
	Widget		w;
	XtWidgetGeometry	*req;
{
	Mask	mask = req->request_mode;

	if (mask & CWX)
		w->core.x = req->x;
	if (mask & CWY)
		w->core.y = req->y;
	if (mask & CWWidth)
		w->core.width = req->width;
	if (mask & CWHeight)
		w->core.height = req->height;
	if (mask & CWBorderWidth)
		w->core.border_width = req->border_width;
}

/*************************************<->*************************************
 *
 *	static void ChangeWidgetAccordingToFlags(w,req)
 *	Widget		w;
 *	XtWidgetGeometry	*req;
 *
 *   Description:
 *   -----------
 *		Bind req to w.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void ChangeWidgetAccordingToFlags(w,req)
	Widget		w;
	XtWidgetGeometry	*req;
{
	Mask	mask = req->request_mode;
	int	x,y,width,height,bw;

	if (mask & CWX)
		x = req->x;
	else
		x = w->core.x;
	if (mask & CWY)
		y = req->y;
	else
		y = w->core.y;
	if (mask & CWWidth)
		width = req->width;
	else
		width = w->core.width;
	if (mask & CWHeight)
		height = req->height;
	else
		height = w->core.height;
	if (mask & CWBorderWidth)
		bw = req->border_width;
	else
		bw = w->core.border_width;
	
	XtConfigureWidget(w,x,y,width,height,bw);
}

/*************************************<->*************************************
 *
 *	static void ClearMeaninglessFlags(w, req, rep)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *
 *   Description:
 *   -----------
 *		Clear flags which ask for changes to current values.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void ClearMeaninglessFlags(w, req, rep)
	Widget		w;
	XtWidgetGeometry	*req, *rep;
{
	if ((req->request_mode & CWX) &&
		(req->x == w->core.x))
		req->request_mode &= ~CWX;
	if ((req->request_mode & CWY) &&
		(req->y == w->core.y))
		req->request_mode &= ~CWY;
	if ((req->request_mode & CWWidth) &&
		(req->width == w->core.width))
		req->request_mode &= ~CWWidth;
	if ((req->request_mode & CWHeight) &&
		(req->height == w->core.height))
		req->request_mode &= ~CWHeight;
	if ((req->request_mode & CWBorderWidth) &&
		(req->border_width == w->core.border_width))
		req->request_mode &= ~CWBorderWidth;
}

/*************************************<->*************************************
 *
 *	static Boolean HandleTrivialGeometryManagement(w, req, rep, result)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *		XtGeometryResult	*result;
 *
 *   Description:
 *   -----------
 *		Handles the easy cases.  Returns TRUE if it was able to determin the
 *		correct result, and FALSE otherwise.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static Boolean HandleTrivialGeometryManagement(w, req, rep, result)
    Widget		w;
    XtWidgetGeometry	*req, *rep;
	XtGeometryResult	*result;
{
	XwPanelPart	*pp; 
	XwPanelWidget	pw = (XwPanelWidget) w->core.parent;
	Mask	mask = (Mask) req->request_mode;
	int	min_w, min_h;
	Boolean	illegal = FALSE;

	pp = (XwPanelPart *) &(pw->panel);
	*result = XtGeometryNo;

	/*
	 * Was anything really asked for?
	 */
	if (!mask)
	{
		*result = XtGeometryYes;
		return(TRUE);
	}

	if ((w != pp->title) && 
		(w != pp->pulldown) && 
		(w != pp->work_space))
	{
		if (mask == CWBorderWidth)
		{
			rep->request_mode |= (CWX | CWY);
			rep->x = -1 * (req->width + 10 + (2 * req->border_width));
			rep->y = -1 * (req->height + 10 + (2 * req->border_width));
			rep->border_width = req->border_width;
			*result = XtGeometryAlmost;
			return(TRUE);
		}
		if (!(mask & CWBorderWidth))
			/*
			 * This simplifies our life.
			 */
			req->border_width = w->core.border_width;

		if (mask & CWWidth)
		{
			if (mask & CWX)
			{
				if ((rep->x = (-1 * (req->width + 10 +
					(2 * req->border_width)))) < req->x)
					illegal |= TRUE;
			}
			else
			{
				if ((rep->x = (-1 * (req->width + 10 +
					(2 * req->border_width)))) < w->core.x)
				{
					rep->request_mode |= CWX;
					illegal |= TRUE;
				}
			}
		}
		if (mask & CWHeight)
		{
			if (mask & CWY)
			{
				if ((rep->y = (-1 * (req->height + 10 +
					(2 * req->border_width)))) < req->y)
					illegal |= TRUE;
			}
			else
			{
				if ((rep->y = (-1 * (req->height + 10 +
					(2 * req->border_width)))) < w->core.y)
				{
					rep->request_mode |= CWY;
					illegal |= TRUE;
				}
			}
		}
		if ((mask == CWX) &&
			((rep->x = (-1 * (w->core.width + 10 +
			(2 * req->border_width)))) < req->x))
			illegal |= TRUE;
		if ((mask == CWY) &&
			((rep->y = (-1 * (w->core.height + 10 +
			(2 * req->border_width)))) < req->y))
			illegal |= TRUE;
		if (illegal)
			*result = XtGeometryAlmost;
		else
		{
			if ((w->core.x > 0) && (w->core.y > 0))
			{
				/*
				 * The widget has changed to type XwUNKNOWN.
				 * We know that because it has changed from positive
				 * x,y to valid negative x,y.
				 *
				 * We need to re-layout the Panel.
				 */

				 GetMinSize(pw,&min_w,&min_h);
				 XtMakeResizeRequest(pw,min_w,min_h,NULL,NULL);
				 Layout(pw);
			}
			UpdateWidgetAccordingToFlags(w,req);
			*result = XtGeometryYes;
		}
		return(TRUE);
	}

	/*
	 * If just stack stuff, say yes.
	 */
	if (((mask & CWStackMode) || (mask & CWSibling)) &&
		!(mask & CWWidth) &&
		!(mask & CWHeight) &&
		!(mask & CWBorderWidth))
		{
			UpdateWidgetAccordingToFlags(w,req);
			*result = XtGeometryYes;
			return(TRUE);
		}

	/*
	 * Do we even care about what the children want?
	 */
	if (pw->manager.layout == XwIGNORE)
	{
		*result = XtGeometryNo;
		return(TRUE);
	}

	return(FALSE);
}

/*************************************<->*************************************
 *
 *	static XtGeometryResult AdjustWS(w,req,rep)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *
 *   Description:
 *   -----------
 *		Sees if a height request can be accomodated by changing the work
 *		space widget's size.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static XtGeometryResult AdjustWS(w,req,rep)
    Widget		w;
    XtWidgetGeometry	*req, *rep;
{
	XwPanelWidget	pw = (XwPanelWidget) w->core.parent;
	XwPanelPart	*pp;
	int	diff,avail;
	int	temp = 0;
	Boolean allshowing = FALSE;
	
	pp = (XwPanelPart *) &(pw->panel);

	/*
	 * Trivial case.
	 */
	if ((!pp->work_space) || (w == pp->work_space))
		return(XtGeometryNo);

	/*
	 * Determine minimum height required by Panel to
	 * display the current title, the current pulldown
	 * and a work space 1 pixel high.
	 */
	temp += pp->internal_height;
	temp += (pp->title ? 
		(pp->title->core.height + (2 * pp->title->core.border_width)) 
		:
		0);
	temp += ((pp->pulldown && pp->title) ?
		(pp->title_to_pulldown)
		:
		0);
	temp += (pp->pulldown ? 
		(pp->pulldown->core.height +
		(2 * pp->pulldown->core.border_width))
		:
		0);
	temp += (((pp->title || pp->pulldown) && (pp->work_space)) ?
		pp->wspace_to_sibling
		:
		0);
	temp += (pp->work_space ?
		1 + (2 * pp->work_space->core.border_width)
		:
		0);
	temp += pp->internal_height;

	if (temp < pw->core.height)
		allshowing = TRUE;

	/* 
	 * How much growing room is available?
	 */
	avail = (pp->work_space->core.height + pp->wspace_to_sibling) - 1;

	/*
	 * What is the difference requested by this widget?
	 */
	diff = 
		((req->request_mode & CWHeight) ?
			(req->height - w->core.height) :
			0)
		+
		((req->request_mode & CWBorderWidth) ?
			(2 * (req->border_width - w->core.border_width)) :
			0);

	/*
	 * Set up X and Y for an Almost return.
	 */
	rep->x = w->core.x;
	rep->y = w->core.y;

	if (diff > 0)
	{
		/* 
		 * Grow 
		 */
		if (!allshowing)
			/*
			 * No space
			 */
			return(XtGeometryNo);
	
		if (diff <= avail)
		{
			/*
			 * Enough space to satisfy the request.
			 */
			rep->height = req->height;
			return(XtGeometryYes);
		}
		else
		{
			/*
			 * Not enough space to satisfy the request.
			 */
			rep->border_width = w->core.border_width;
			rep->height = w->core.height + avail;
			return(XtGeometryAlmost);
		}
	}
	else
		/*
		 * Shrink
		 */
		return(XtGeometryYes);
}

/*************************************<->*************************************
 *
 *	static void BindWSAdjust(w,req,rep)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *
 *   Description:
 *   -----------
 *		Adjusts the height of the work space child to accomodate a sibling's
 *		resize request.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void BindWSAdjust(w,req,rep)
    Widget		w;
    XtWidgetGeometry	*req, *rep;
{
	XwPanelWidget	pw = (XwPanelWidget) w->core.parent;
	XwPanelPart	*pp;
	int	diff,avail,ny,ty,nh,nw;
	int	temp = 0;
	Boolean allshowing = FALSE;

	pp = (XwPanelPart *) &(pw->panel);

	if (req->request_mode & CWWidth)
		w->core.width = req->width;
	if (req->request_mode & CWHeight)
		w->core.height = req->height;
	if (req->request_mode & CWBorderWidth)
		w->core.border_width = req->border_width;

	diff = w->core.height - req->height;
	if (w == pp->title)
	{
		/*
		 * Move the pulldown and adjust the work space.
		 */
		if (pp->pulldown)
		{
			ny = ty = pp->internal_height + w->core.height +
				(2 * w->core.border_width) + pp->title_to_pulldown +
				pp->pulldown->core.border_width;
			XtMoveWidget(pp->pulldown,pp->internal_width,ny);
			ny = ty + pp->pulldown->core.height +
				pp->pulldown->core.border_width +
				pp->wspace_to_sibling +
				pp->work_space->core.border_width;
			nh = pw->core.height - pp->internal_height - ny;
			nw = pw->core.width - (2 * ( pp->internal_width
				+ pp->work_space->core.border_width));
			XtConfigureWidget(pp->work_space,pp->internal_width,ny,
				nw,nh,pp->work_space->core.border_width);
		}
		else
		{
			ny = ty = pp->internal_height + w->core.width +
				w->core.border_width + pp->wspace_to_sibling +
				pp->work_space->core.border_width;
			nh = pw->core.height - pp->internal_height - ny;
			nw = pw->core.width - (2 * pp->internal_width);
			XtConfigureWidget(pp->work_space,pp->internal_width,ny,
				nw,nh,pp->work_space->core.border_width);
		}
	}
	else /* w == pulldown */
	{
		ny = ty = pp->internal_height + w->core.width +
			w->core.border_width + pp->wspace_to_sibling +
			pp->work_space->core.border_width;
		nh = pw->core.height - pp->internal_height - ny;
		nw = pw->core.width - (2 * pp->internal_width);
		XtConfigureWidget(pp->work_space,pp->internal_width,ny,
			nw,nh,pp->work_space->core.border_width);
	}
}

/*************************************<->*************************************
 *
 *	static XtGeometryResult ManageNRChild(w, req, rep)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *
 *   Description:
 *   -----------
 *		Process a geometry request from a child which cannot cause the
 *		Panel to resize.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static XtGeometryResult ManageNRChild(w, req, rep)
    Widget		w;
    XtWidgetGeometry	*req, *rep;
{
	XwPanelConstraintRec	*pcp;
	XwPanelWidget	pw = (XwPanelWidget) w->core.parent;
	XwPanelPart	*pp;
	XtGeometryResult	Pres;
	Mask	mask = req->request_mode;

	pp = (XwPanelPart *) &(pw->panel);
	pcp = (XwPanelConstraintRec *) w->core.constraints;

	if (mask & CWWidth)
	{
		/*
		 * A change in width can cause a resize request, so the 
		 * best that can be returned is Almost.
		 */
		rep->width = w->core.width;
		if ((mask & CWHeight) || (mask & CWBorderWidth))
		{
			/*
			 * See if the height is available.
			 */
			Pres = AdjustWS(w,req,rep);
			switch (Pres)
			{
				case XtGeometryNo:
					if ((mask & CWStackMode) || (mask & CWSibling))
						return(XtGeometryAlmost);
					return(XtGeometryNo);
				break;
				case XtGeometryYes:
				case XtGeometryAlmost:
					return(XtGeometryAlmost);
				break;
				/*
				default:
					XtError("XwPanelWidget: Internal error in NCR");
				break;
				*/
			}
		}
		else if ((mask & CWStackMode) || (mask & CWSibling))
		{
			rep->width = w->core.width;
			return(XtGeometryAlmost);
		}
		else 
			return(XtGeometryNo);
	}
	else if ((mask & CWHeight) || (mask & CWBorderWidth))
	{
		/*
		 * See if the height is available.
		 */
		Pres = AdjustWS(w,req,rep);
		switch (Pres)
		{
			case XtGeometryNo:
				if ((mask & CWStackMode) || (mask & CWSibling))
					return(XtGeometryAlmost);
				return(XtGeometryNo);
			break;
			case XtGeometryYes:
				UpdateWidgetAccordingToFlags(w,req);
				Layout(pw);
				return(XtGeometryYes);

			break;
			case XtGeometryAlmost:
				return(XtGeometryAlmost);
			break;
			/*
			default:
				XtError("XwPanelWidget: Internal error in NCR");
			break;
			*/
		}
	}
	else
	{
		/*
		 * A visible child asked for (X || Y)
		 *
		 * We could do the XtConfigure and Almost trick, but
		 * it's not worth it.
		 * We shouldn't get here.
		 */
		Layout(pw);
		return(XtGeometryNo);
	}
}

/*************************************<->*************************************
 *
 *	static XtGeometryResult ManageCRChild(w, req, rep)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *
 *   Description:
 *   -----------
 *		Process a geometry request from a child which can cause the
 *		Panel to resize.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static XtGeometryResult ManageCRChild(w, req, rep)
    Widget		w;
    XtWidgetGeometry	*req, *rep;
{
	XwPanelConstraintRec	*pcp;
	XwPanelWidget	pw = (XwPanelWidget) w->core.parent;
	XwPanelPart	*pp;
	int	cmin_w, cmin_h, nmin_w, nmin_h;
	XtWidgetGeometry	myreq, panelreq, panelrep;
	XtGeometryResult	Wrep, Pres;
	Mask	mask = (Mask) req->request_mode;
	Boolean	XYflag = TRUE;

	pp = (XwPanelPart *) &(pw->panel);
	pcp = (XwPanelConstraintRec *) w->core.constraints;
	panelreq.request_mode = 0;

	/*
	 * Just how big would we like to be?
	 */
	GetMinSize(pw,&nmin_w,&nmin_h);

	/*
	 * Initialize the reply 
	 * This simplifies the case where Panel's 
	 * parent returns Almost.
	 */
	panelrep.width = pw->core.width;
	panelrep.height = pw->core.height;

	/*
	 * If an X or Y has been asked for we have to handle
	 * it special.
	 */
	if ((mask & CWX) || (mask & CWY))
		XYflag = FALSE;

	/*
	 * Will we need to change width?
	 */
	Wrep = XtGeometryYes;
	if (nmin_w != pw->core.width)
	{
		panelreq.width = nmin_w;
		panelreq.request_mode |= CWWidth;
	}
	else if (mask & CWWidth)
	{
		/*
		 * The child has asked for a new width that we cannot
		 * satisfy (min_w did not change).  The best that we can
		 * do is return Almost.  However, we cannot ask our parent
		 * for a height because we might be told yes and be resized
		 * to the requested height (a problem if we intend to just
		 * return an Alomst).
		 *
		 * Punt..
		 */
		rep->width = w->core.width;
		Wrep = XtGeometryNo;
	}

	/*
	 * Will we need to change height?
	 */
	if (nmin_h != pw->core.height)
	{
		panelreq.height = nmin_h;
		panelreq.request_mode |= CWHeight;
	}

	/*
	 * Do we need to ask our parent?
	 */
	if (panelreq.request_mode)
	{
		Pres = XtMakeGeometryRequest(pw,&panelreq,&panelrep);
	}
	else
		Pres = XtGeometryYes;

	switch (Pres)
	{
		case XtGeometryYes:
			if ((Wrep != XtGeometryNo) && (XYflag))
			{
				/*
				 * The request can be approved.  Update the core
				 * fields of the requesting widget.
				 */
				UpdateWidgetAccordingToFlags(w,req);
				/*
				 * Layout will only handle setting the children to the
				 * new width (if any).  We need to handle the new
				 * height explicitly.
				 *
				 * We requested the nmin_h which was made up of the 
				 * requested heights stored in the constraint record,
				 * so we will set the children to their requested 
				 * heights.
				 */
				if (pw->panel.title)
				{
					pcp = (XwPanelConstraintRec *) 
						pw->panel.title->core.constraints;
					XtResizeWidget(pw->panel.title,
						pw->panel.title->core.width,
						pcp->req_height,
						pw->panel.title->core.border_width);
				}
				if (pw->panel.pulldown)
				{
					pcp = (XwPanelConstraintRec *) 
						pw->panel.pulldown->core.constraints;
					XtResizeWidget(pw->panel.pulldown,
						pw->panel.pulldown->core.width,
						pcp->req_height,
						pw->panel.pulldown->core.border_width);
				}
				if (pw->panel.work_space)
				{
					pcp = (XwPanelConstraintRec *) 
						pw->panel.work_space->core.constraints;
					XtResizeWidget(pw->panel.work_space,
						pw->panel.work_space->core.width,
						pcp->req_height,
						pw->panel.work_space->core.border_width);
				}
				/*
				 * Now do the layout.
				 */
				Layout(pw);
				return(XtGeometryYes);
			}
			/*
			 * Here comes a hack.  We're going to return Almost
			 * when we should return Done.  This is because there
			 * was no other way to ask for the height.
			 */
			ChangeWidgetAccordingToFlags(w,rep);
			Layout(pw);
			if (!XYflag)
			{
				rep->x = w->core.x;
				rep->y = w->core.y;
			}
			return(XtGeometryAlmost);
		break;
		case XtGeometryNo:
			if ((mask & CWHeight) || (mask & CWBorderWidth))
			{
				/*
				 * We know that we can't change the width so
				 * set the reply appropriately.
				 */
				rep->width = w->core.width;
				Pres = AdjustWS(w,req,rep);
				switch (Pres)
				{
					case XtGeometryNo:
						return(XtGeometryNo);
					break;
					case XtGeometryYes:
						if (!(mask & CWWidth))
						{
							UpdateWidgetAccordingToFlags(w,req);
							Layout(pw);
							/*
							 * Should be XtGeometryDone...
							 */
							return(XtGeometryYes);
						}
						else
							return(XtGeometryAlmost);
					break;
					case XtGeometryAlmost:
						return(XtGeometryAlmost);
					break;
					/*
					default:
						XtError("XwPanelWidget: Internal error CR");
					break;
					*/
				}
			}
			else
				return(XtGeometryNo);
		case XtGeometryAlmost:
			rep->width = panelrep.width - (2 * pp->internal_width) -
				(2 * w->core.border_width);
			rep->border_width = w->core.border_width;
			rep->height = w->core.height + panelrep.height -
				pw->core.height - (2 * w->core.border_width);
			return(XtGeometryAlmost);
		break;
		/*
		default:
			 *
			 * We shouldn't get here.
			 *
			XtWarning("XwPanelWidget: Internal error CR");
		break;
		*/
	}
}

/*************************************<->*************************************
 *
 * static XtGeometryResult GeometryManager(w, req, rep)
 *		Widget		w;
 *		XtWidgetGeometry	*req, *rep;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static XtGeometryResult GeometryManager(w, req, rep)
    Widget		w;
    XtWidgetGeometry	*req, *rep;
{
	static XtWidgetGeometry	myreq;
	XwPanelConstraintRec	*pcp;
	XwPanelWidget	pw = (XwPanelWidget) w->core.parent;
	XwPanelPart	*pp;
	XtWidgetGeometry	panelreq, panelrep;
	XtGeometryResult	Wrep, Hrep, Pres;

	pp = (XwPanelPart *) &(pw->panel);
	pcp = (XwPanelConstraintRec *) w->core.constraints;
	panelreq.request_mode = 0;
 
	/*
	 * Make sure that req and rep point to different things,
	 * and initialize it properly.
	 */
	if (req == rep)
	{
		rep = &myreq;
	}

	rep->request_mode = req->request_mode;
	rep->stack_mode = req->stack_mode;
	rep->sibling = req->sibling;
	rep->x = req->x;
	rep->y = req->y;
	rep->width = req->width;
	rep->height = req->height;
	rep->border_width = req->border_width;


	/*
	 * If the child can cause resizes, note what the child thinks is 
	 * its optimum geometry.
	 */
	if (pcp->causes_resize)
	{
		if (req->request_mode & CWWidth)
			pcp->req_width = req->width;
		if (req->request_mode & CWHeight)
			pcp->req_height = req->height;
		if (req->request_mode & CWBorderWidth)
			pcp->req_border_width = req->border_width;
	}
	
	ClearMeaninglessFlags(w,req,rep);

	if (HandleTrivialGeometryManagement(w,req,rep,&Pres))
		return(Pres);

	if (pcp->causes_resize)
		return(ManageCRChild(w,req,rep));
	else
		return(ManageNRChild(w,req,rep));
}

/*************************************<->*************************************
 *
 *	static void Layout(pw)
 *		XwPanelWidget pw;
 *
 *   Description:
 *   -----------
 *		Lays out the Panel's children within the given size of the panel.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void Layout(pw)
	XwPanelWidget pw;
{
	XwPanelPart	*pp;
	Widget	title, pulldown, work_space;
	int x,y,w,h;

	pp = &(pw->panel);
	title = ((pp->title && pp->title->core.managed) ? pp->title : NULL);
	pulldown = ((pp->pulldown && pp->pulldown->core.managed)
		? pp->pulldown : NULL);
	work_space = ((pp->work_space && pp->work_space->core.managed) 
		? pp->work_space : NULL);
	w = pw->core.width - (2 * pp->internal_width);
	if (w <= 0)
		/*
		 * We can't fit any children, so just leave every
		 * child at its previous width.  Title is as good
		 * as any, so use it.
		 */
		w = title->core.width + (2 * title->core.border_width);

	/*
	 * Hide the title if it is not supposed to be displayed.
	 */
	if ((title) && (title->core.managed) && !(pp->title_on))
	{
		x = -1 * (title->core.width + 10 +
			(2 * title->core.border_width));
		y = -1 * (title->core.height + 10 +
			(2 * title->core.border_width));
		XtMoveWidget(title,x,y);
	}

	x = pw->panel.internal_width;
	y = pw->panel.internal_height;
	if ((title) && (title->core.managed) && (pp->title_on))
	{
		XtConfigureWidget(title,
			x,
			y,
			(w - (2 * title->core.border_width)),
			title->core.height,
			title->core.border_width);
		y += title->core.height + (2 * title->core.border_width);
	}
	if ((pulldown) && (pulldown->core.managed))
	{
		if ((title) && (title->core.managed) && (pp->title_on))
			y += pp->title_to_pulldown;
		XtConfigureWidget(pulldown,
			x,
			y,
			(w - (2 * pulldown->core.border_width)),
			pulldown->core.height, pulldown->core.border_width);
		y += pulldown->core.height + (2 * pulldown->core.border_width);
	}
	if ((work_space) && (work_space->core.managed))
	{
		if (((pp->title) && (pp->title->core.managed) && (pp->title_on))
			||
			((pp->pulldown) && (pp->pulldown->core.managed)))
			y += pp->wspace_to_sibling;
		h = pw->core.height - y - (2 * work_space->core.border_width) -
			pp->internal_height;
		if (h <= 0)
			h = work_space->core.height;
		XtConfigureWidget(work_space,
			x,
			y,
			(w - (2 * work_space->core.border_width)),
			h,
			work_space->core.border_width);
	}
}

/*************************************<->*************************************
 *
 *	static void ChangeManaged(pw)
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 *
 *************************************<->***********************************/
static void ChangeManaged(pw)
    XwPanelWidget pw;
{
	int min_h, min_w, i, x, y;
	WidgetList	kids;
	XwPanelConstraintRec	*pcp;
	XtWidgetGeometry	new, rep;
	XtGeometryResult	q_res;

	kids = pw->composite.children;
	if (!XtIsRealized(pw))
	{
		for (i=0; i < pw->composite.num_children; i++)
		{
			pcp = (XwPanelConstraintRec *) kids[i]->core.constraints;
			pcp->was_managed = kids[i]->core.managed;
		}
		return;
	}

	for (i=0; i < pw->composite.num_children; i++)
	{
		/*
		 * Hide unmanaged chiledren and any extra managed children.
		 */
		if (((kids[i] != pw->panel.title) &&
			(kids[i] != pw->panel.pulldown) &&
			(kids[i] != pw->panel.work_space)) ||
			!(kids[i]->core.managed))
		{
			x = -1 * (kids[i]->core.width + 10 +
				(2 * kids[i]->core.border_width));
			y = -1 * (kids[i]->core.height + 10 +
				(2 * kids[i]->core.border_width));
			XtMoveWidget(kids[i],x,y);
		}
		/*
		 * Newly re-managed children get their requested 
		 * geometry updated.
		 *
		 * Newly unmanaged children get their current 
		 * geometry saved.
		 */
		pcp = (XwPanelConstraintRec *) kids[i]->core.constraints;
		if (pcp->was_managed != kids[i]->core.managed)
		{
			if (kids[i]->core.managed)
			{
				if (pcp->height_last_managed !=
					kids[i]->core.height)
					pcp->req_height = kids[i]->core.height;
				if (pcp->width_last_managed !=
					kids[i]->core.width)
					pcp->req_width = kids[i]->core.width;
				if (pcp->border_width_last_managed !=
					kids[i]->core.border_width)
					pcp->req_border_width = kids[i]->core.border_width;
			}
			else
			{
				pcp->height_last_managed = kids[i]->core.height;
				pcp->width_last_managed = kids[i]->core.width;
				pcp->border_width_last_managed = 
					kids[i]->core.border_width;
			}
			pcp->was_managed = kids[i]->core.managed;
		}
	}

	if ((pw->panel.pulldown) && (pw->panel.pulldown->core.managed))
	{
		new.request_mode = CWWidth;
		new.width = pw->core.width - (2 * (pw->panel.internal_width + 
			pw->panel.pulldown->core.border_width));
		q_res = XtQueryGeometry(pw->panel.pulldown,&new,&rep);
		if ((q_res != XtGeometryNo) &&
			(rep.request_mode & CWHeight))
		{
			/*
			 * We need to size the widget.  Layout will handle
			 * the position.
			 */
			XtResizeWidget(pw->panel.pulldown,new.width,rep.height,
				pw->panel.pulldown->core.border_width);
			pcp = (XwPanelConstraintRec *) 
				pw->panel.pulldown->core.constraints;
			pcp->req_width = pw->panel.pulldown->core.width;
			pcp->req_height = pw->panel.pulldown->core.height;
		}
	}

	if (pw->manager.layout != XwIGNORE)
	{
		GetMinSize(pw,&min_w,&min_h);
		XtMakeResizeRequest(pw,min_w,min_h,NULL,NULL);
	}

	Layout(pw);
}

/*************************************<->*************************************
 *
 *	static void InsertChild(w,args,n)
 *		Widget	w;
 *		ArgList	args;
 *		Cardinal	*n;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void InsertChild(w)
	Widget	w;
{
    int _XW_NARGS = 0;
	ArgList	args = NULL;
	Cardinal	*n = (Cardinal *) &_XW_NARGS;

	XwManagerWidgetClass	superclass;
	XwPanelConstraintRec	*pcp;
	XwPanelWidget	pw;

	superclass = (XwManagerWidgetClass) XwmanagerWidgetClass;
	(*superclass->composite_class.insert_child)(w,args,n);
	
	pcp = (XwPanelConstraintRec *) w->core.constraints;
	pw = (XwPanelWidget) w->core.parent;
	switch (pcp->type)
	{
		case XwTITLE:
			if (!pw->panel.title ||
				pw->panel.title->core.being_destroyed)
				pw->panel.title = w;
		break;
		case XwPULLDOWN:
			if (!pw->panel.pulldown ||
				pw->panel.pulldown->core.being_destroyed)
				pw->panel.pulldown = w;
		break;
		case XwWORK_SPACE:
			if (!pw->panel.work_space ||
				pw->panel.work_space->core.being_destroyed)
				pw->panel.work_space = w;
		break;
		case XwUNKNOWN:
		default:
		break;
	}
}

/*************************************<->*************************************
 *
 *	static void DeleteChild(w,args,n)
 *		Widget	w;
 *		ArgList	args;
 *		Cardinal	*n;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void DeleteChild(w,args,n)
	Widget	w;
	ArgList	args;
	Cardinal	*n;
{
	XwManagerWidgetClass	superclass;
	XwPanelConstraintRec	*pcp;
	XwPanelWidget	pw;
	
	/*
	 * I would like to:

	if (XtIsSubClass(w,ShellWidgetClass))
		return;

	 * but the stupid C linker would link all of shell.o out of
	 * libXt.a with Panel.  This is very space inefficient, so
	 * we will merely ASSUME that if the child has a null 
	 * constraint pointer, the child is a shell child.  
	 * 
	 * Ideally * DeleteChild would not be called for Shell children
	 * (like InsertChild) <flame of DEC and MIT deleted>.
	 */
	pcp = (XwPanelConstraintRec *) w->core.constraints;

	if (pcp == NULL)
		return;

	pw = (XwPanelWidget) w->core.parent;
	switch (pcp->type)
	{
		case XwTITLE:
			if (pw->panel.title == w)
				pw->panel.title = NULL;
		break;
		case XwPULLDOWN:
			if (pw->panel.pulldown == w)
				pw->panel.pulldown = NULL;
		break;
		case XwWORK_SPACE:
			if (pw->panel.work_space == w)
				pw->panel.work_space = NULL;
		break;
		default:
		break;
	}

	superclass = (XwManagerWidgetClass) XwmanagerWidgetClass;
	(*superclass->composite_class.delete_child)(w,args,n);
}

/*************************************<->*************************************
 *
 *	static void ConstraintInitialize(req, new)
 *		Widget	req, new;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void ConstraintInitialize(req, new)
	Widget	req, new;
{
	XwPanelConstraintRec	*pcp;

	pcp = (XwPanelConstraintRec *) new->core.constraints;

	switch (pcp->type)
	{
		case XwTITLE:
		case XwPULLDOWN:
		case XwWORK_SPACE:
		case XwUNKNOWN:
		break;
		default:
			pcp->type = XwUNKNOWN;
		break;
	}

	pcp->req_height = new->core.height;
	pcp->req_width = new->core.width;
	pcp->req_border_width = new->core.border_width;
	pcp->was_managed = FALSE;
}

/*************************************<->*************************************
 *
 *	static Boolean ConstraintSetValues(cur,req,new)
 *		Widget	cur, req, new;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static Boolean ConstraintSetValues(cur,req,new)
	Widget	cur, req, new;
{
	XwPanelConstraintRec	*cpcp,*rpcp,*npcp;
	XwPanelWidget	pw;
	Boolean	flag = FALSE;
	Boolean	approved = FALSE;

	pw = (XwPanelWidget) cur->core.parent;
	cpcp = (XwPanelConstraintRec *) cur->core.constraints;
	rpcp = (XwPanelConstraintRec *) req->core.constraints;
	npcp = (XwPanelConstraintRec *) new->core.constraints;
	
	if (rpcp->type != cpcp->type)
	{
		flag = TRUE;
		switch (rpcp->type)
		{
			case XwTITLE:
				if (!pw->panel.title ||
					pw->panel.title->core.being_destroyed)
				{
					approved = TRUE;
					pw->panel.title = cur;
				}
			break;
			case XwPULLDOWN:
				if (!pw->panel.pulldown ||
					pw->panel.pulldown->core.being_destroyed)
				{
					approved = TRUE;
					pw->panel.pulldown = cur;
				}
			break;
			case XwWORK_SPACE:
				if (!pw->panel.work_space ||
					pw->panel.work_space->core.being_destroyed)
				{
					approved = TRUE;
					pw->panel.work_space = cur;
				}
			break;
			case XwUNKNOWN:
			default:
				approved = TRUE;
				/*
				 * Hide the child.
				 */
				new->core.x = -1 * (cur->core.width + 10 +
						(2 * cur->core.border_width));
				new->core.y = -1 * (cur->core.height + 10 +
						(2 * cur->core.border_width));
			break;
		}
		if (approved)
		{
			switch (cpcp->type)
			{
				case XwTITLE:
					if (pw->panel.title == cur)
						pw->panel.title = NULL;
				break;
				case XwPULLDOWN:
					if (pw->panel.pulldown == cur)
						pw->panel.pulldown = NULL;
				break;
				case XwWORK_SPACE:
					if (pw->panel.work_space == cur)
						pw->panel.work_space = NULL;
				break;
				case XwUNKNOWN:
					/*
					 * Move the child into the visible region of the 
					 * panel.  This will cause a request to Panel's 
					 * GeometryManager.
					 */
					new->core.x = pw->panel.internal_width;
					new->core.y = pw->panel.internal_height;
				default:
				break;
			}
			npcp->type = rpcp->type;
		}
	}
	if (rpcp->causes_resize != cpcp->causes_resize)
		npcp->causes_resize = rpcp->causes_resize;
	
	if (new->core.height != cur->core.height)
		npcp->req_height = new->core.height;
	
	if (new->core.width != cur->core.width)
		npcp->req_width = new->core.width;
	
	if (new->core.border_width != cur->core.border_width)
		npcp->req_border_width = new->core.border_width;
}

/*************************************<->*************************************
 *
 *	static void Redisplay(pw)
 *		XwPanelWidget	pw;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void Redisplay(pw)
	XwPanelWidget	pw;
{
}

/*************************************<->*************************************
 *
 *	static void Realize(pw, vmask, attributes)
 *		XwPanelWidget	pw;
 *		XtValueMask	*vmask;
 *		XSetWindowAttributes	*attributes;
 *
 *   Description:
 *   -----------
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void Realize(pw, vmask, attributes)
	XwPanelWidget	pw;
	XtValueMask	*vmask;
	XSetWindowAttributes	*attributes;
{
	XwPanelConstraintRec	*pcp;
	Widget	title, pulldown, wspace;
	XtValueMask	mask = *vmask;
	int	min_w, min_h;

	if (pw->manager.layout != XwIGNORE)
	{
		title = pw->panel.title;
		pulldown = pw->panel.pulldown;
		wspace = pw->panel.work_space;
	
		/*
		 * Get the optimum sizes.
		 * We haven't talked to our children since Insert child time.
		 */
		if (title)
		{
			pcp = (XwPanelConstraintRec *) title->core.constraints;
			pcp->req_height = title->core.height;
			pcp->req_width = title->core.width;
			pcp->req_border_width = title->core.border_width;
		}
	
		if (pulldown)
		{
			pcp = (XwPanelConstraintRec *) pulldown->core.constraints;
			pcp->req_height = pulldown->core.height;
			pcp->req_width = pulldown->core.width;
			pcp->req_border_width = pulldown->core.border_width;
		}
	
		if (wspace)
		{
			pcp = (XwPanelConstraintRec *) wspace->core.constraints;
			pcp->req_height = wspace->core.height;
			pcp->req_width = wspace->core.width;
			pcp->req_border_width = wspace->core.border_width;
		}
	
		GetMinSize(pw,&min_w,&min_h);
	
		XtMakeResizeRequest(pw,min_w,min_h,NULL,NULL);
	}
	
	Layout(pw);
	
	XtCreateWindow(pw,InputOutput,(Visual *) CopyFromParent,
		mask,attributes);
	
	_XwRegisterName(pw);
}
