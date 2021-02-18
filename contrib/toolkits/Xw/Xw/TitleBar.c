/*************************************<+>******************************
 **********************************************************************
 **
 **   File:   TitleBar.c
 **
 **   Project:     X Widgets
 **
 **   Description: Code/Definitions for widget class.
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


/**********************************************************************
 *
 *  Includes used by and procedures declared by TitleBar.
 *
 *********************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/SText.h>
#include <Xw/TitleBar.h>
#include <Xw/TitleBarP.h>

/* Procedures for Core Class Initialization */

static void ClassInitialize();
static void Initialize();
static void Destroy();
static void Resize();
static void Realize();
static void Redisplay();
static Boolean SetValues();

/* Procedures for Composite Class Initialization */

static XtGeometryResult GeometryManager();
static void ChangeManaged();
static void InsertChild();
static void DeleteChild();

/* Procedures for Constraint Class Initializaiton */

static void ConstraintInitialize();
static Boolean ConstraintSetValues();

/* Translation Procedures */

static void Enter();
static void Leave();
static void Select();
static void Release();

/* Private procedures */

static void ValidateInputs();
static void SetSTextArgs();
static void DoLayout();
static void LinkPrecedence();
static void LinkPosition();
static void UnLinkPrecedence();
static void UnLinkPosition();

/**********************************************************************
 *
 * Default translations and Action list for TitleBar. 
 *
 * See Toolkit documentation.
 *
 *********************************************************************/
static char defaultTranslation[] = {
	"<EnterWindow>:	enter() \n\
	<LeaveWindow>:	leave() \n\
	<Btn1Down>:	select() \n\
	<Btn1Up>:	release()"
};

static XtActionsRec actionsList[] = {
	{"enter",	(XtActionProc) Enter},
	{"leave",	(XtActionProc) Leave},
	{"select",	(XtActionProc) Select},
	{"release",	(XtActionProc) Release},
};

/**********************************************************************
 *
 * Resource Definition and defaults for TitleBar.
 *
 * See Toolkit documentation.
 *
 *********************************************************************/
static XtResource resources[] = {
	{ XtNtitlePrecedence,
		XtCTitlePrecedence,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_precedence),
		XtRString,
		"0"
	},
	{ XtNtitleRegion,
		XtCTitleRegion,
		XtRAlignment,
		sizeof(XwAlignment),
		XtOffset(XwTitleBarWidget,titlebar.title_region),
		XtRString,
		"Center"
	},
	{ XtNtitlePosition,
		XtCTitlePosition,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_position),
		XtRString,
		"0"
	},
	{ XtNtitleRPadding,
		XtCTitleRPadding,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_rpadding),
		XtRString,
		"1"
	},
	{ XtNtitleLPadding,
		XtCTitleLPadding,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_lpadding),
		XtRString,
		"1"
	},
	{ XtNtitleBorderWidth,
		XtCBorderWidth,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_borderwidth),
		XtRString,
		"0"
	},
	{ XtNtitleHSpace,
		XtCHSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_hspace),
		XtRString,
		"2"
	},
	{ XtNtitleVSpace,
		XtCVSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.title_vspace),
		XtRString,
		"2"
	},
	{ XtNtitleSelect,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.title_select),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNtitleRelease,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.title_release),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNtitleNextTop,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.title_nexttop),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNvSpace,
		XtCVSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.internal_height),
		XtRString,
		"2"
	},
	{ XtNhSpace,
		XtCHSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.internal_width),
		XtRString,
		"2"
	},
	{ XtNtitleForeground,
		XtCForeground,
		XtRPixel,
		sizeof(Pixel),
		XtOffset(XwTitleBarWidget,titlebar.title_foreground),
		XtRString,
		"black"
	},
	{ XtNtitleBackground,
		XtCBackground,
		XtRPixel,
		sizeof(Pixel),
		XtOffset(XwTitleBarWidget,titlebar.title_background),
		XtRString,
		"white"
	},
	{ XtNenter,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.enter),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNleave,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.leave),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNselect,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.select),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNrelease,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.release),
		XtRCallback,
		(caddr_t) NULL
	},
	{ XtNnextTop,
		XtCCallback,
		XtRCallback,
		sizeof(caddr_t),
		XtOffset (XwTitleBarWidget, titlebar.next_top),
		XtRCallback,
		(caddr_t) NULL
	},
	/*
	 * Resources for primitive part of internal StaticText widget
	 */
	{ XtNhighlightColor,
		XtCForeground,
		XtRPixel,
		sizeof (Pixel),
		XtOffset (XwTitleBarWidget, titlebar.highlight_color),
		XtRString,
		"Black"
	},
	{ XtNhighlightStyle,
		XtCHighlightStyle,
		XtRHighlightStyle,
		sizeof (int),
		XtOffset (XwTitleBarWidget, titlebar.highlight_style),
		XtRString,
		"pattern_border"
	},
	{ XtNhighlightTile,
		XtCHighlightTile,
		XtRTileType,
		sizeof (int),
		XtOffset (XwTitleBarWidget, titlebar.highlight_tile),
		XtRString,
		"50_percent_foreground"
	},
	{ XtNhighlightThickness,
		XtCHighlightThickness,
		XtRInt,
		sizeof (int),
		XtOffset (XwTitleBarWidget, titlebar.highlight_thickness),
		XtRString,
		"2"
	},
	{ XtNtraversalType,
		XtCTraversalType,
		XtRTraversalType,
		sizeof (int),
		XtOffset (XwTitleBarWidget, titlebar.traversal_type),
		XtRString,
		"off_no_highlight"
	},
	/* 
	 * Resources for internal static text widget.
	 */
	{ XtNstring,
		XtCString,
		XtRString,
		sizeof(char *),
		XtOffset(XwTitleBarWidget, titlebar.string),
		XtRString,
		NULL
	},
	{ XtNalignment,
		XtCAlignment,
		XtRAlignment,
		sizeof(XwAlignment),
		XtOffset(XwTitleBarWidget,titlebar.title_string_alignment),
		XtRString,
		"Left"
	},
	{ XtNwrap,
		XtCWrap,
		XtRBoolean,
		sizeof(Boolean),
		XtOffset(XwTitleBarWidget,titlebar.wrap),
		XtRString,
		"TRUE"
	},
	{ XtNlineSpace,
		XtCLineSpace,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarWidget,titlebar.line_space),
		XtRString,
		"0"
	},
	{ XtNfont,
		XtCFont,
		XtRFontStruct,
		sizeof(XFontStruct *),
		XtOffset(XwTitleBarWidget,titlebar.font),
		XtRString,
		"Fixed"
	}
};

/**********************************************************************
 *
 * Constraint Resource Definition and defaults for TitleBar.
 *
 * See Toolkit documentation.
 *
 *********************************************************************/
static XtResource constraints[] = {
	{ XtNregion,
		XtCAlignment,
		XtRAlignment,
		sizeof(XwAlignment),
		XtOffset(XwTitleBarConstraints, region),
		XtRString,
		"None"
	},
	{ XtNposition,
		XtCPosition,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarConstraints, position),
		XtRString,
		"1"
	},
	{ XtNlPadding,
		XtCLPadding,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarConstraints, lpadding),
		XtRString,
		"2"
	},
	{ XtNrPadding,
		XtCRPadding,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarConstraints, rpadding),
		XtRString,
		"2"
	},
	{ XtNprecedence,
		XtCPrecedence,
		XtRInt,
		sizeof(int),
		XtOffset(XwTitleBarConstraints, precedence),
		XtRString,
		"1"
	}
};

/**********************************************************************
 *
 * Class record for TitleBar.
 *
 *********************************************************************/
XwTitleBarClassRec XwtitleBarClassRec = {
   {  /* core class part */
      (WidgetClass) &XwmanagerClassRec, /* superclass            */
      "TitleBar",                       /* class_name            */
      sizeof(XwTitleBarRec),            /* widget_size           */
      (XtProc) ClassInitialize,         /* class_initialize      */
      FALSE,                            /* class_inited          */
	  NULL,                             /* class_part_initialize */
      (XtInitProc) Initialize,          /* initialize            */
      NULL,                             /* initialize_hook       */
      (XtRealizeProc) Realize,          /* realize               */
      actionsList,                      /* actions               */
      XtNumber(actionsList),            /* num_actions           */
      resources,                        /* resources             */
      XtNumber(resources),              /* num_resources         */
      NULLQUARK,                        /* xrm_class             */
      TRUE,                             /* compress_motion       */
      TRUE,                             /* compress_exposure     */
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */
      (XtWidgetProc) Destroy,           /* destroy               */
      (XtWidgetProc) Resize,            /* resize                */
      (XtExposeProc) Redisplay,         /* expose                */
      (XtSetValuesFunc) SetValues,      /* set_values            */
      NULL,                             /* set_values_hook       */
      NULL,                             /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus          */
	  XtVersion,                        /* version               */
      NULL,                             /* callback private      */
	  defaultTranslation,               /* tm_table              */
      NULL,                              /* query_geometry        */
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
   },

   {  /* composite class part */
      (XtGeometryHandler) GeometryManager, /*  geometry_manager    */
      (XtWidgetProc) ChangeManaged,        /*  change_managed      */
      (XtArgsProc) InsertChild,            /*  insert_child        */
      (XtArgsProc) DeleteChild,            /*  delete_child        */
      NULL,
   },

   {  /* constraint class part */
      constraints,                           /* constraint resource set   */
      XtNumber(constraints),                 /* num_resources             */
      sizeof(XwTitleBarConstraintRec),       /* size of the data          */
      (XtInitProc) ConstraintInitialize,     /* contraint initilize proc  */
      NULL,                                  /* contraint destroy proc    */
      (XtSetValuesFunc) ConstraintSetValues, /* contraint set values proc */
      NULL,
   },

   {  /*  manager class     */
      NULL        /*  traversal handler */
   },

   {  /*  titlebar class    */
      0           /*  mumble      */
   }
};

WidgetClass XwtitlebarWidgetClass = (WidgetClass) &XwtitleBarClassRec;
WidgetClass XwtitleBarWidgetClass = (WidgetClass) &XwtitleBarClassRec;

/*************************************<->*************************************
 *
 *	static void ClassInitialize()
 *
 *   Description:
 *   -----------
 *		This procedure does runtime initializtion of the Class record.
 *		See X Toolkit documentation.
 *
 *   Inputs:
 *   ------
 *		None.
 * 
 *************************************<->***********************************/
static void ClassInitialize()
{
	  
	XwManagerWidgetClass
		superclass = (XwManagerWidgetClass) XwmanagerWidgetClass;
	XwTitleBarWidgetClass
		myclass = (XwTitleBarWidgetClass) XwtitlebarWidgetClass;

        XwtitleBarClassRec.manager_class.traversal_handler = 
	  (XwTraversalProc) mgr_traversal;
}

/*************************************<->*************************************
 *
 *	static void ValidateInputs(tbw)
 *		XwTitleBarWidget tbw;
 *
 *   Description:
 *   -----------
 *		This procedure checks for illegal input values and fixes them.
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void ValidateInputs(tbw)
	XwTitleBarWidget tbw;
{
	XwTitleBarPart *tbp;

	tbp = &(tbw->titlebar);
	if (tbp->internal_width < 0)
	{
		XtWarning("XwTitleBarWidget: hSpace less than zero; setting to zero.");
		tbp->internal_width = 0;
	}

	if (tbp->internal_height < 0)
	{
		XtWarning("XwTitleBarWidget: vSpace less than zero; setting to zero.");
		tbp->internal_height = 0;
	}
}

/*************************************<->*************************************
 *
 *	static void SetSTextArgs(tbw,args)
 *		XwTitleBarWidget	tbw;
 *		ArgList	args;
 *
 *   Description:
 *   -----------
 *		Not a particularly interesting or elegant procedure.  This procedure
 *		initializes the StaticText arglist.  Done as a separate procedure in 
 *		order to de-clutter the Initialize procedure.
 *
 *   Inputs:
 *   ------
 *		It is presumed that the values in tbw are valid and that args has
 *		enough space.
 * 
 *************************************<->***********************************/
static void SetSTextArgs(tbw,args)
	XwTitleBarWidget	tbw;
	ArgList	args;
{
	XwTitleBarPart	*tbp;

	tbp = &(tbw->titlebar);

	XtSetArg(args[0], XtNforeground,
		(XtArgVal)tbp->title_foreground);
	XtSetArg(args[1], XtNbackground,
		(XtArgVal)tbp->title_background);
	XtSetArg(args[2], XtNhighlightColor,
			(XtArgVal) tbp->highlight_color);
	XtSetArg(args[3], XtNhighlightStyle,
			(XtArgVal) tbp->highlight_style);
	XtSetArg(args[4], XtNhighlightTile,
			(XtArgVal) tbp->highlight_tile);
	XtSetArg(args[5], XtNhighlightThickness,
			(XtArgVal) tbp->highlight_thickness);
	XtSetArg(args[6], XtNtraversalType,
			(XtArgVal) tbp->traversal_type);
	XtSetArg(args[7], XtNstring,
			(XtArgVal) tbp->string);
	XtSetArg(args[8], XtNalignment,
			(XtArgVal) tbp->title_region);
	XtSetArg(args[9], XtNwrap,
			(XtArgVal) tbp->wrap);
	XtSetArg(args[10], XtNlineSpace,
			(XtArgVal) tbp->line_space);
	XtSetArg(args[11], XtNfont,
			(XtArgVal) tbp->font);
	XtSetArg(args[12], XtNregion,
			(XtArgVal) tbp->title_region);
	XtSetArg(args[13], XtNposition,
			(XtArgVal) tbp->title_position);
	XtSetArg(args[14], XtNrPadding,
			(XtArgVal) tbp->title_rpadding);
	XtSetArg(args[15], XtNlPadding,
			(XtArgVal) tbp->title_lpadding);
	XtSetArg(args[16], XtNprecedence,
			(XtArgVal) tbp->title_precedence);
	XtSetArg(args[17], XtNhSpace,
			(XtArgVal) tbp->title_hspace);
	XtSetArg(args[18], XtNvSpace,
			(XtArgVal) tbp->title_vspace);
	XtSetArg(args[19], XtNborderWidth,
			(XtArgVal) tbp->title_borderwidth);
	XtSetArg(args[20], XtNselect,
			(XtArgVal) tbp->title_select);
	XtSetArg(args[21], XtNrelease,
			(XtArgVal) tbp->title_release);
	XtSetArg(args[22], XtNnextTop,
			(XtArgVal) tbp->title_nexttop);
}

/*************************************<->*************************************
 *
 *	static void Initialize(request,new)
 *		XwTitleBarWidget request, new;
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
static void Initialize(request,new)
	XwTitleBarWidget request, new;
{
	XwTitleBarPart	*tbp;
	Arg	stextargs[26], stextvals[5];
	Widget	*w;
	char *s;
	int	i;

	tbp = &(new->titlebar);
	ValidateInputs(new);

	/*
	 * Initialize private fields...
	 */
	if (request->core.height)
		new->titlebar.height_given = TRUE;
	else
		new->titlebar.height_given = FALSE;

	if (request->core.width)
		new->titlebar.width_given = TRUE;
	else
		new->titlebar.width_given = FALSE;

	tbp->last_region = XwALIGN_RIGHT;

	/*
	 * Make pointers safe.
	 */
	tbp->lpositions = NULL;
	tbp->cpositions = NULL;
	tbp->rpositions = NULL;

	/*
	 * Allocate and initialize the precedence array.
	 */
	tbp->precedence = (Widget *)XtMalloc(XwTB_CHUNK*sizeof(Widget));
	tbp->size_precedence = XwTB_CHUNK;
	tbp->num_precedence = 0;
	w = tbp->precedence; 
	for(i=0; i<tbp->size_precedence; i++)
		w[i] = NULL;

	if (tbp->string)
	{
		/*
		 * Copy the input string into local space.
		 */
		s = XtMalloc(XwStrlen(tbp->string)+1);
		strcpy(s,tbp->string);
		tbp->string = s;

		/*
		 * Set up for default regions of other children.
		 */
		switch (tbp->title_region)
		{
			case XwALIGN_NONE:
				tbp->title_region = XwALIGN_LEFT;
			case XwALIGN_LEFT:
				tbp->leftover1 = XwALIGN_CENTER;
				tbp->leftover2 = XwALIGN_RIGHT;
			break;
			case XwALIGN_CENTER:
				tbp->leftover1 = XwALIGN_LEFT;
				tbp->leftover2 = XwALIGN_RIGHT;
			break;
			case XwALIGN_RIGHT:
				tbp->leftover1 = XwALIGN_LEFT;
				tbp->leftover2 = XwALIGN_CENTER;
			break;
		}

		SetSTextArgs(new,stextargs);

		/*
		 * WARNING -- Any changes in the number of args set in 
		 * SetSTextArgs had better be reflected below!
		 */
		XtSetArg(stextargs[23],XtNheight,0);
		XtSetArg(stextargs[24],XtNwidth,0);

		/*
		 * Create the thing.
		 */
		tbp->text = (Widget) XtCreateWidget("TitleText",
			XwstatictextWidgetClass,(Widget) new,stextargs,25);
		

		/*
		 * Honor initial size, or pick a reasonable one.
		 */
		if (!new->titlebar.width_given)
			new->core.width += (3 * (((Widget) tbp->text)->core.width +
				(2 * ((Widget) tbp->text)->core.border_width)) + 
				(2 * tbp->internal_width));

		if (!new->titlebar.height_given)
			new->core.height += ((Widget) tbp->text)->core.height +
				(2 * ((Widget) tbp->text)->core.border_width) + 
				(2 * tbp->internal_height);
		
		/*
		 * Manage the thing.
		 */
		XtManageChild(tbp->text);
	}
	else
	{
		/*
		 * Just do something, even if it is brain damaged.
		 */
		if (!new->titlebar.width_given)
			new->core.width += 2 * tbp->internal_width + 75;
		if (!new->titlebar.height_given)
			new->core.height += 2 * tbp->internal_height + 25;
	}

}

/*************************************<->*************************************
 *
 *	static void ConstraintInitialize(request,new)
 *		Widget request,new;
 *
 *   Description:
 *   -----------
 *
 *   Inputs:
 *   ------
 * 
 *************************************<->***********************************/
static void ConstraintInitialize(request,new)
	Widget request,new;
{
	XwTitleBarConstraintRec *tbcp = XwTB_Constraints(new);
	XwTitleBarWidget	parent = (XwTitleBarWidget) new->core.parent;
	XwAlignment	*last_region;
	XwAlignment	leftover1 = parent->titlebar.leftover1;
	XwAlignment	leftover2 = parent->titlebar.leftover2;

	last_region = &(parent->titlebar.last_region);

	/*
	 * Handle illegal inputs.
	 */
	if (tbcp->lpadding < 0)
	{
		XtWarning("XwTitleBarConstraints: lPadding less than zero; setting to 1");
		tbcp->lpadding = 1;
	}

	if (tbcp->rpadding < 0)
	{
		XtWarning("XwTitleBarConstraints: rPadding less than zero; setting to 1");
		tbcp->rpadding = 1;
	}

	/*
	 * Initialize region.
	 */
	if (tbcp->region == XwALIGN_NONE)
        {
		if (!parent->titlebar.string)
                {
			switch (*last_region)
			{
				case XwALIGN_LEFT:
					*last_region = tbcp->region = XwALIGN_CENTER;
				break;
				case XwALIGN_CENTER:
					*last_region = tbcp->region = XwALIGN_RIGHT;
				break;
				case XwALIGN_RIGHT:
					*last_region = tbcp->region = XwALIGN_LEFT;
				break;
			}
                }
		else
                {
			if (*last_region == leftover1)
				*last_region = tbcp->region = leftover2;
			else
				*last_region = tbcp->region = leftover1;
                }
        }

	/*
	 * Make layout information safe.
	 */
	tbcp->hidden_by_titlebar = FALSE;
	tbcp->padding_honored = TRUE;

	/*
	 * Make the list pointers safe.
	 */
	tbcp->next_position = NULL;
	tbcp->prec_sibling = NULL;
}

/*************************************<->*************************************
 *
 *	static void Destroy(tbw)
 *		XwTitleBarWidget	tbw;
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
static void Destroy(tbw)
	XwTitleBarWidget	tbw;
{
	XtFree(tbw->titlebar.string);
	XtFree(tbw->titlebar.precedence);
}

/*************************************<->*************************************
 *
 *	static void LinkPrecedence(w)
 *		Widget	w;
 *
 *   Description:
 *   -----------
 *		A lot could be said here, but without pictures it wouldn't make much
 *		sense.  Essentially, the precedence array is an array of pointers to
 *		all of the children of the same precedence.  The array is sorted in 
 *		order of highest display priority (lowest value of precedence).  This
 *		procedure add widgets to the precedence structure.
 *
 *   Inputs:
 *   ------
 *		A child of a titlebar.
 * 
 *************************************<->***********************************/
static void LinkPrecedence(w)
	Widget	w;
{
	XwTitleBarConstraintRec	*tbcp = XwTB_Constraints(w);
	Boolean	unique = FALSE;
	XwTitleBarWidget	parent;
	Widget	*prec;
	Widget	q1, q2;
	int i, j;
	int	*num_prec, *size_prec;
	int	curprec = tbcp->precedence;

	/*
	 * Let's save ourselves some typing and the CPU
	 * some pointer arithmetic.
	 */
	parent = (XwTitleBarWidget) w->core.parent;
	prec = parent->titlebar.precedence;
	num_prec = &(parent->titlebar.num_precedence);
	size_prec = &(parent->titlebar.size_precedence);

	/*
	 * Is this the first?
	 */
	if (!(*num_prec))
	{
		prec[0] = w;
		(*num_prec)++;
		return;
	}

	/*
	 * Find where this child should fit in.
	 */
	for (i=0; i<*num_prec; i++)
	{
		if (XwTB_Constraints(prec[i])->precedence < curprec)
			continue;
		if (XwTB_Constraints(prec[i])->precedence == curprec)
			break;
		if (XwTB_Constraints(prec[i])->precedence > curprec)
		{
			unique = TRUE;
			break;
		}
	}

	if (i == *num_prec)
		unique = TRUE;

	/*
	 * We have an entirely new value of precedence.
	 */
	if (unique)
	{
		(*num_prec)++;
		/*
		 * Do we need more table space?
		 */
		if (*num_prec > *size_prec)
		{
			j = *size_prec;
			parent->titlebar.precedence = (Widget *) XtRealloc(prec,
				(*size_prec + XwTB_CHUNK));
			*size_prec += XwTB_CHUNK;
			prec = parent->titlebar.precedence;
			for (;j < *size_prec; j++)
				prec[j] = NULL;
		}

		/*
		 * Add to the end of the array.
		 */
		if (!prec[i])
		{
			prec[i] = w;
		}
		else
			/*
			 * Add in the middle of the array.
			 */
			for (q1 = w; i < *num_prec; i++)
			{
				q2 = prec[i];
				prec[i] = q1;
				q1 = q2;
			}

	}	
	else
	{
		/*
		 * Link into the appropriate precedence list.
		 */
		q1 = prec[i];
		prec[i] = w;
		XwTB_Constraints(w)->prec_sibling = q1;
	}
}

/*************************************<->*************************************
 *
 *	static void LinkPosition(w)
 *		Widget	w;
 *
 *   Description:
 *   -----------
 *		Another hairy structure manipulation.  Each region has a position list.
 *		Each position list is sorted in increasing order of position.  This 
 *		procedure inserts a child into the appropriate position list.
 *
 *   Inputs:
 *   ------
 *		A child of the titleBar.
 * 
 *************************************<->***********************************/
static void LinkPosition(w)
	Widget	w;
{
	XwTitleBarConstraintRec	*tbcp = XwTB_Constraints(w);
	int	wpos = tbcp->position;
	Widget *p, *q;
	XwTitleBarWidget parent = (XwTitleBarWidget) w->core.parent;

	/*
	 * Get the head of the appropriate list.
	 */
	switch (tbcp->region)
	{
		case XwALIGN_LEFT:
			p = (Widget *) &(parent->titlebar.lpositions);
		break;
		case XwALIGN_CENTER:
			p = (Widget *) &(parent->titlebar.cpositions);
		break;
		case XwALIGN_RIGHT:
			p = (Widget *) &(parent->titlebar.rpositions);
		break;
	}

	if (!(*p))
	{
		/*
		 * Add the first one.
		 */
		*p = w;
		(XwTB_Constraints(w))->next_position = NULL;
		return;
	}

	q = NULL;
	while (*p)
	{
		/*
		 * Find out where it goes.  This loop exits with *q 
		 * pointing to the widget before w and *p pointing to
		 * the widget after w.
		 */
		if (wpos == XwTB_Constraints((*p))->position)
		{
			/* OOPS!  Can't have two widgets with the same position
			 * value.  Change it.  
			 *
			 * Notice that this generates no error message.  This is 
			 * because the default value is always the same.  Perhaps 
			 * a SetValues to a taken value should generate an error.
			 *
			 * What do you think?
			 */
			wpos++;
			tbcp->position = wpos;
		}
		else if (wpos > XwTB_Constraints((*p))->position)
		{
			q = p;
			p = &(XwTB_Constraints((*p))->next_position);
			if ((*p) && (wpos < XwTB_Constraints((*p))->position))
				break;
		}
		else
			break;
	}

	/*
	 * Point w to the next highest position.
	 */
	(XwTB_Constraints(w))->next_position = *p;

	if (!q)
		/*
		 * Insert at the head of the list.
		 */
		*p = w;
	else
		/*
		 * Insert in the middle of the list.
		 */
		(XwTB_Constraints((*q)))->next_position = w;
}

/*************************************<->*************************************
 *
 *	static void InsertChild(w,args,num_args)
 *		Widget	w;
 *		ArgList	args;
 *		Cardinal	num_args;
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
	Cardinal	*num_args = (Cardinal *) &_XW_NARGS;

	XwManagerWidgetClass	superclass;

	superclass = (XwManagerWidgetClass) XwmanagerWidgetClass;
	(*superclass->composite_class.insert_child)(w,args,num_args);

	LinkPrecedence(w);
	LinkPosition(w);
}

/*************************************<->*************************************
 *
 *	static void UnLinkPrecedence(w)
 *		Widget	w;
 *
 *   Description:
 *   -----------
 *		This procedure removes a widget from the precedence structure.
 *
 *   Inputs:
 *   ------
 *		The widget to be removed.
 * 
 *************************************<->***********************************/
static void UnLinkPrecedence(w)
	Widget	w;
{
	XwTitleBarConstraintRec	*tbcp = XwTB_Constraints(w);
	Widget	*prec = ((XwTitleBarWidget) w->core.parent)->
		titlebar.precedence;
	XwTitleBarWidget	parent;
	int	*num_prec;
	int	*size_prec;
	Widget	p, q; 
	int	i,j;

	parent = (XwTitleBarWidget) w->core.parent;
	size_prec = &(parent->titlebar.size_precedence);
	num_prec = &(parent->titlebar.num_precedence);
	i = 0;
	/*
	 * Find w in the precedence structure.
	 */
	while (i < *num_prec)
	{
		if (XwTB_Constraints(prec[i])->precedence == tbcp->precedence)
		{
			break;
		}
		i++;
	}
	if (prec[i] == w)
        {
		if (XwTB_Constraints(w)->prec_sibling == NULL)
		{
			/*
			 * If it was a unique precedence, remove it and shorten
			 * the precedence array.
			 */
			prec[i] = NULL;
			for(j=(i+1); j < *num_prec; j++)
				prec[(j-1)] = prec[j];
			(*num_prec)--;
			/*
			 * You could do a reallocation here, but I think that
			 * the chances of ever really needing it are very low
			 * so in the intrest of performance, I don't do it.
			 */
		}
		else
		{
			/*
			 * Not unique, but head of the list.  Replace
			 * the head with the next widget of this precedence.
			 */
			prec[i] = XwTB_Constraints(w)->prec_sibling;
		}
        }
	else
	{
		/*
		 * Remove w from the shared precedence list.
		 */
		p = prec[i];
		q = XwTB_Constraints(p)->prec_sibling;
		while (q != w)
		{
			p = q;
			q = XwTB_Constraints(p)->prec_sibling;
		}
		XwTB_Constraints(p)->prec_sibling = 
			XwTB_Constraints(q)->prec_sibling;
	}
}

/*************************************<->*************************************
 *
 *	static void UnLinkPosition(w)
 *		Widget	w;
 *
 *   Description:
 *   -----------
 *		Removes a widget from the position list.
 *
 *   Inputs:
 *   ------
 *		The widget to be removed.
 * 
 *************************************<->***********************************/
static void UnLinkPosition(w)
	Widget	w;
{
	XwTitleBarConstraintRec	*tbcp = XwTB_Constraints(w);
	XwTitleBarWidget	parent = (XwTitleBarWidget) w->core.parent;
	Widget *p, *q;

	switch (tbcp->region)
	{
		case XwALIGN_LEFT:
			p = (Widget *) &(parent->titlebar.lpositions);
		break;
		case XwALIGN_CENTER:
			p = (Widget *) &(parent->titlebar.cpositions);
		break;
		case XwALIGN_RIGHT:
			p = (Widget *) &(parent->titlebar.rpositions);
		break;
	}

	while ((*p) && (*p != w))
		p = &((XwTB_Constraints((*p)))->next_position);
	
	if (*p)
		*p = (XwTB_Constraints(w))->next_position;
	else
		XtError("XwTitleBarWidget: Position constraint corrupted");
}

/*************************************<->*************************************
 *
 *	static void DeleteChild(w)
 *		Widget	w;
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
static void DeleteChild(w)
	Widget	w;
{
	XwManagerWidgetClass	superclass;

	UnLinkPrecedence(w);
	UnLinkPosition(w);

	superclass = (XwManagerWidgetClass) XwmanagerWidgetClass;
	(*superclass->composite_class.delete_child)(w);
}

/*************************************<->*************************************
 *
 *	static Boolean ConstraintSetValues(current,request,new,last)
 *		Widget	current, request, new;
 *		Boolean	last;
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
static Boolean ConstraintSetValues(current,request,new)
	Widget	current, request, new;
{
	Boolean flag = FALSE;
	XwAlignment	newregion;
	int	newprec, newpos;

	XwTitleBarConstraintRec	*newcp = XwTB_Constraints(new);
	XwTitleBarConstraintRec	*curcp = XwTB_Constraints(current);
	XwTitleBarConstraintRec	*reqcp = XwTB_Constraints(request);


	/*
	 * Unfortunately, both new and newcp both point to temporary
	 * space which has no relation to the TitleBar's child list.
	 * The next_position and prec_sibling constraint fields of 
	 * TitleBar's other children, if modified, must point to current 
	 * not new.
	 * 
	 * newcp will be copied onto curcp after this procedure 
	 * returns.  This means that all changes must be made to 
	 * newcp.  However, because of the above paragraph, all child
	 * list pointers must refer to current.  Sooooooo, we attach
	 * newcp to current and modify the lists appropriately.
	 */

	if (newcp->precedence != curcp->precedence)
	{
		newprec = newcp->precedence;
		newcp->precedence = curcp->precedence;
		current->core.constraints = (caddr_t) newcp;
		UnLinkPrecedence(current);
		newcp->precedence = newprec;
		LinkPrecedence(current);
		current->core.constraints = (caddr_t) curcp;
		flag |= TRUE;
	}

	if (newcp->region != curcp->region)
	{
		/*
		 * If the position field has also changed it will be
		 * handled here also.
		 */
		newregion = newcp->region;
		newcp->region = curcp->region;
		current->core.constraints = (caddr_t) newcp;
		UnLinkPosition(current);
		newcp->region = newregion;
		LinkPosition(current);
		current->core.constraints = (caddr_t) curcp;
		flag |= TRUE;
	}
	else if (newcp->position != curcp->position)
	{
		/*
		 * Just the position has changed, so we don't have
		 * to do any region tricks.
		 */
		newpos = newcp->position;
		newcp->position = curcp->position;
		current->core.constraints = (caddr_t) newcp;
		UnLinkPosition(current);
		newcp->position = newpos;
		LinkPosition(current);
		current->core.constraints = (caddr_t) curcp;
		flag |= TRUE;
	}

	if ((newcp->lpadding != curcp->lpadding) ||
		(newcp->rpadding != curcp->rpadding))
	{
		flag |= TRUE;
	}

	if (flag)
	{
		current->core.constraints = (caddr_t) newcp;
		DoLayout(new->core.parent);
		current->core.constraints = (caddr_t) curcp;
	}


	return(flag);
}

/*************************************<->*************************************
 *
 *	static void Realize(tbw,value_mask,attributes)
 *		XwTitleBarWidget	tbw;
 *		Mask *value_mask;
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
static void Realize(tbw,value_mask,attributes)
	XwTitleBarWidget	tbw;
	Mask *value_mask;
	XSetWindowAttributes	*attributes;
{
	DoLayout(tbw);
	XtCreateWindow(tbw,InputOutput,(Visual *) CopyFromParent,
		*value_mask,attributes);
	_XwRegisterName(tbw);
}

/*************************************<->*************************************
 *
 *	static int CollapsePadding(tbw)
 *		XwTitleBarWidget	tbw;
 *
 *   Description:
 *   -----------
 *		Takes padding away from the child with the lowest display priority
 *		(highest value of precedence).  Returns the amount of space regained.
 *
 *		If there is no padding available to be collapsed, returns -1.
 *
 *   Inputs:
 *   ------
 *		A title bar widget.
 * 
 *************************************<->***********************************/
static int CollapsePadding(tbw)
	XwTitleBarWidget	tbw;
{
	Widget *prec = tbw->titlebar.precedence;
	Widget p;
	int num_prec = tbw->titlebar.num_precedence;
	int i;
	int	pad = -1;

	for (i=(num_prec-1); i >= 0; i--)
	{
		p = prec[i];
		while(p)
		{
			if ((p->core.managed) &&
				(!(XwTB_Constraints(p)->hidden_by_titlebar)) &&
				(XwTB_Constraints(p)->padding_honored))
			{
				XwTB_Constraints(p)->padding_honored = FALSE;
				pad = XwTB_Constraints(p)->lpadding +
					XwTB_Constraints(p)->lpadding;
				return(pad ? pad - 1 : pad);
			}
			p = XwTB_Constraints(p)->prec_sibling;
		}
	}
	return(-1);
}

/*************************************<->*************************************
 *
 *	static void HideWidget(w)
 *		Widget	w;
 *
 *   Description:
 *   -----------
 *		Hides a widget by moving it out of the displayed region of 
 *		the window.
 *
 *   Inputs:
 *   ------
 *		A Widget to be hidden.
 * 
 *************************************<->***********************************/
static void HideWidget(w)
	Widget	w;
{
	int	x,y;
	
	x = w->core.width + (2 * w->core.border_width) + 5;
	y = w->core.height + (2 * w->core.border_width) + 5;
	x = (-1) * x;
	y = (-1) * y;
	XtMoveWidget(w,x,y);
	XwTB_Constraints(w)->hidden_by_titlebar = TRUE;
}

/*************************************<->*************************************
 *
 *	static int HideLowPrecedenceWidget(tbw)
 *		XwTitleBarWidget	tbw;
 *
 *   Description:
 *   -----------
 *		Hides the visible child with the lowest display priority.
 *
 *		NOTE:  This code assumes that all padding has been collapsed
 *		       before widgets are hidden.
 *
 *   Inputs:
 *   ------
 *		A title bar widget.
 * 
 *************************************<->***********************************/
static int HideLowPrecedenceWidget(tbw)
	XwTitleBarWidget	tbw;
{
	Widget *prec = tbw->titlebar.precedence;
	Widget p;
	int num_prec = tbw->titlebar.num_precedence;
	int i,x;

	for (i=(num_prec-1); i >= 0; i--)
	{
		p = prec[i];
		while(p)
		{
			if ((p->core.managed) &&
				(!(XwTB_Constraints(p)->hidden_by_titlebar)))
			{
				HideWidget(p);
				x = p->core.width + (2 * p->core.border_width);
				/*
				 * Implicit Assumption:
				 *
				 *     Padding has already been collapsed to
				 *     either zero or one.
				 */
				if ((XwTB_Constraints(p)->lpadding + 
					XwTB_Constraints(p)->lpadding) > 0)
					x++;
				return(x);
			}
			p = XwTB_Constraints(p)->prec_sibling;
		}
	}
	return(0);
}

/*************************************<->*************************************
 *
 *	static int SumRegion(tbw,region)
 *		XwTitleBarWidget	tbw;
 *		XwAlignment	region;
 *
 *   Description:
 *   -----------
 *		Returns the amount of space required to display a region
 *		of the title bar.
 *
 *   Inputs:
 *   ------
 *		A title bar and the region to be summed.
 * 
 *************************************<->***********************************/
static int SumRegion(tbw,region)
	XwTitleBarWidget	tbw;
	XwAlignment	region;
{
	XwTitleBarPart	*tbp;
	Widget	p = NULL;
	int	sum = 0;

	tbp = &(tbw->titlebar);
	switch (region)
	{
		case XwALIGN_LEFT:
			p = tbp->lpositions;
		break;
		case XwALIGN_RIGHT:
			p = tbp->rpositions;
		break;
		case XwALIGN_CENTER:
			p = tbp->cpositions;
		break;
	}
	while (p)
	{
		if ((p->core.managed) &&
			(!(XwTB_Constraints(p)->hidden_by_titlebar)))
		{
			sum += ((int)p->core.width + 
				(2 * (int)p->core.border_width));
			if (XwTB_Constraints(p)->padding_honored)
				sum += (XwTB_Constraints(p)->lpadding +
					XwTB_Constraints(p)->rpadding);
			else if ((XwTB_Constraints(p)->lpadding +
				XwTB_Constraints(p)->rpadding) > 0)
				sum++;
		}
		p = XwTB_Constraints(p)->next_position;
	}
	return(sum);
}

/*************************************<->*************************************
 *
 *	static void DoLayout(tbw)
 *		XwTitleBarWidget	tbw;
 *
 *   Description:
 *   -----------
 *		Lays the children out into the title bar, collapsing padding and 
 *		hiding children as appropriate.
 *
 *   Inputs:
 *   ------
 *		A title bar.
 * 
 *************************************<->***********************************/
static void DoLayout(tbw)
	XwTitleBarWidget	tbw;
{
	XwTitleBarPart	*tbp;
	Arg	args[1];
	Widget	p = NULL;
	WidgetList kids = tbw->composite.children;
	int	i, y, cspace, Lx, Cx, Rx;
	int	target, extent, diff;
	int	height_sav, width_sav;
	int	num_managed = 0;	
	int	tH = 0;

	tbp = &(tbw->titlebar);
	
	/*
	 * Trivial case.
	 */
	if ((!tbp->lpositions) && (!tbp->cpositions) && (!tbp->rpositions))
		return;

	/* 
	 * Save these so we can smash them later.
	 */
	height_sav = tbp->internal_height;
	width_sav = tbp->internal_width;

	/*
	 * Speaking of smashing...
	 *
	 * Smash internal_height if it doesn't leave any room
	 * for children.
	 */
	if ((extent = (int)tbw->core.height - (2 * tbp->internal_height)) 
		<= 0)
	{
		extent = (int)tbw->core.height;
		tbp->internal_height = 0;
	}

	/*
	 * Each time we lay out we start from scratch.
	 */
	for (i=0; i < tbw->composite.num_children; i++)
		if (kids[i]->core.managed)
		{
			XwTB_Constraints(kids[i])->hidden_by_titlebar = FALSE;
			XwTB_Constraints(kids[i])->padding_honored = TRUE;
			num_managed++;
		}

	/*
	 * How much space do we have to work with?
	 */
	target = (int)tbw->core.width - (2 * tbp->internal_width);

	/* 
	 * What is our wish list size?
	 */
	extent = SumRegion(tbw,XwALIGN_LEFT) + 
		SumRegion(tbw,XwALIGN_CENTER) + SumRegion(tbw,XwALIGN_RIGHT);

	/*
	 * If Santa can't afford our wish list, then trim it
	 * down to meet his budget.
	 */
	while (extent > target)
		if ((diff = CollapsePadding(tbw)) < 0)
			break;
		else
			extent -= diff;
	if (extent > target)
		if (tbp->internal_width > 0)
			tbp->internal_width = 1;
		else
			tbp->internal_width = 0;
	target = tbw->core.width - (2 * tbp->internal_width);
	while ((extent > target) && (extent > 0))
		extent -= HideLowPrecedenceWidget(tbw); 

	/*
	 * There's not even space to display one measely
	 * child.  Take the child with the highest display
	 * priority and let him be clipped.
	 */
	if (extent <= 0 )
	{
		for (i=0; i < tbp->size_precedence; i++)
		{
			p = tbp->precedence[i];
			while (p)
			{
				if (p->core.managed)
				{
					/*
					 * KLUDGE!  Done because "break" will only
					 * exit one loop.
					 */
					i = tbp->size_precedence;
					break;
				}
				p = XwTB_Constraints(p)->prec_sibling;
			}
		}
		if (p)
			XtMoveWidget(p,0,tbp->internal_height);
		tbw->titlebar.internal_height = height_sav;
		tbw->titlebar.internal_width = width_sav;
		/*
		 * Now wasn't that easy!
		 */
		return;
	}

	/*
	 * All children are forced to vertically span the title bar,
	 * so they are all laid out with the same y value.
	 */
	y = tbp->internal_height;

	/*
	 * All children are forced to vertically span the title bar,
	 * so they all have the same height.
	 */
	tH = tbw->core.height - (2 * tbp->internal_height);

	/*
	 * Set the left to right starting point and the right to left
	 * starting point.
	 */
	Lx = tbp->internal_width;
	Rx = (int) tbw->core.width - (int) tbp->internal_width;

	/*
	 * Layout the left region.
	 */
	if (p = tbp->lpositions)
	{
		while (p)
		{
			/*
			 * Because of all the convoluted structure accesses
			 * this looks kind of messy.  The process is simple.
			 * Just lay out what was left after the budget cuts.
			 */
			if ((p->core.managed) &&
				(!(XwTB_Constraints(p)->hidden_by_titlebar)))
			{
				if (XwTB_Constraints(p)->padding_honored)
					Lx += XwTB_Constraints(p)->lpadding;
				Lx += p->core.border_width;
				XtConfigureWidget(p,Lx,y,p->core.width,
					(tH - (2 * p->core.border_width)),p->core.border_width);
				Lx += (int)p->core.width + p->core.border_width;
				if (XwTB_Constraints(p)->padding_honored)
					Lx += XwTB_Constraints(p)->lpadding;
				else if ((XwTB_Constraints(p)->lpadding + 
						XwTB_Constraints(p)->lpadding) > 0)
						Lx++;
			}
			p = XwTB_Constraints(p)->next_position;
		}
	}

	/*
	 * Layout the right region.
	 */
	if (p = tbp->rpositions)
	{
		while (p)
		{
			if ((p->core.managed) &&
				(!(XwTB_Constraints(p)->hidden_by_titlebar)))
			{
				if (XwTB_Constraints(p)->padding_honored)
					Rx -= XwTB_Constraints(p)->rpadding;
				Rx -= (int) p->core.border_width;
				Rx -= (int) p->core.width;
				XtConfigureWidget(p,Rx,y,p->core.width,
					(tH - (2 * p->core.border_width)),p->core.border_width);
				if (XwTB_Constraints(p)->padding_honored)
					Rx -= XwTB_Constraints(p)->lpadding;
				else if ((XwTB_Constraints(p)->lpadding + 
						XwTB_Constraints(p)->lpadding) > 0)
						Rx--;
			}
			p = XwTB_Constraints(p)->next_position;
		}
	}

	/*
	 * Layout the center region.
	 */
	if (p = tbp->cpositions)
	{
		/*
		 * Try to center the center region
		 */
		cspace = SumRegion(tbw,XwALIGN_CENTER);
		Cx = (int) (tbw->core.width / 2 ) - (int) (cspace / 2);
		if ((diff = (Lx - Cx)) > 0)
			Cx += diff;
		else if ((diff = (Rx - (Cx + cspace))) < 0)
			Cx += diff;
		while (p)
		{
			if ((p->core.managed) &&
				(!(XwTB_Constraints(p)->hidden_by_titlebar)))
			{
				if (XwTB_Constraints(p)->padding_honored)
					Cx += XwTB_Constraints(p)->lpadding;
				Cx += p->core.border_width;
				XtConfigureWidget(p,Cx,y,p->core.width,
					(tH - (2 * p->core.border_width)),p->core.border_width);
				Cx += (int)p->core.width + p->core.border_width;
				if (XwTB_Constraints(p)->padding_honored)
					Cx += XwTB_Constraints(p)->lpadding;
				else if (((XwTB_Constraints(p)->lpadding + 
					XwTB_Constraints(p)->lpadding) > 0) &&
					(XwTB_Constraints(p)->next_position))
					Cx++;
			}
			p = XwTB_Constraints(p)->next_position;
		}
	}
	tbp->internal_width = width_sav;
	tbp->internal_height = height_sav;
}

/*************************************<->*************************************
 *
 *	static void ChangeManaged(tbw)
 *		XwTitleBarWidget	tbw;
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
static void ChangeManaged(tbw)
	XwTitleBarWidget	tbw;
{
	XtGeometryResult	result;
	int tW,tH,cH,rH,rW,i;
	WidgetList	kids = tbw->composite.children;

	if (tbw->manager.layout == XwMINIMIZE)
	{
		/*
		 * Find the new optimum size and try to be that size.
		 */
		tW = 2 * tbw->titlebar.internal_width;
		for(i=0; i < tbw->composite.num_children; i++)
			if (kids[i]->core.managed)
				tW += kids[i]->core.width + 
					(2 * kids[i]->core.border_width) +
					XwTB_Constraints(kids[i])->lpadding +
					XwTB_Constraints(kids[i])->rpadding;
		tH = 0;
		for(i=0; i < tbw->composite.num_children; i++)
			if (kids[i]->core.managed)
			{
				cH = kids[i]->core.height +
					(2 * kids[i]->core.border_width);
				if (cH > tH)
					tH = cH;
			}
		tH += 2 * tbw->titlebar.internal_height;
	
		result = XtMakeResizeRequest(tbw,tW,tH,&rW,&rH);
		switch (result)
		{
			case XtGeometryAlmost:
				result = XtMakeResizeRequest(tbw,rW,rH,&tW,&tH);
			case XtGeometryYes:
				case XtGeometryNo:
			default:
			break;
		}
	}

	DoLayout(tbw);
}

/*************************************<->*************************************
 *
 *	static void Resize(tbw)
 *		XwTitleBarWidget	tbw;
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
static void Resize(tbw)
	XwTitleBarWidget	tbw;
{
	tbw->titlebar.width_given = TRUE;
	tbw->titlebar.height_given = TRUE;
	DoLayout(tbw);
}

/*************************************<->*************************************
 *
 *	static XtGeometryResult GeometryManager(w,request,reply)
 *		Widget	w;
 *		XtWidgetGeometry	*request, *reply;
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
static XtGeometryResult GeometryManager(w,request,reply)
	Widget	w;
	XtWidgetGeometry	*request, *reply;
{
	XwTitleBarWidget	parent = (XwTitleBarWidget) w->core.parent;
	WidgetList	kids = parent->composite.children;
	XtGeometryResult	result;
	int	tH,tW,cH,i,rW,rH;

	/*
	 * Hack for Geometry Management Design flaw in toolkit.
	 */
	if ((request->request_mode & CWX) ||
		(request->request_mode & CWX))
	{
		return(XtGeometryNo);
	}

	if ((request->request_mode == CWSibling) ||
		(request->request_mode == CWStackMode) ||
		(request->request_mode == (CWSibling & CWStackMode)))
		return(XtGeometryYes);

	if (parent->manager.layout == XwMINIMIZE)
	{
		if (request->request_mode & CWBorderWidth)
			w->core.border_width = request->border_width;
		if (request->request_mode & CWHeight)
			if ((request->height + (2 * request->border_width)) >
				(parent->core.height))
				{
					if (request->border_width > 0)
						reply->border_width = 1;
					if (parent->core.height > 2)
						reply->height = parent->core.height - 2;
					else
						reply->height = request->height;
					return(XtGeometryAlmost);
				}
			else
				w->core.height = request->height;
		if (request->request_mode & CWWidth)
			w->core.width = request->width;

		tW = 2 * parent->titlebar.internal_width;
		for(i=0; i < parent->composite.num_children; i++)
			if (kids[i]->core.managed)
				tW += kids[i]->core.width + 
					(2 * kids[i]->core.border_width) +
					XwTB_Constraints(kids[i])->lpadding +
					XwTB_Constraints(kids[i])->rpadding;
		tH = 0;
		for(i=0; i < parent->composite.num_children; i++)
			if (kids[i]->core.managed)
			{
				cH = kids[i]->core.height +
					(2 * kids[i]->core.border_width);
				if (cH > tH)
					tH = cH;
			}
		tH += 2 * parent->titlebar.internal_height;

		result = XtMakeResizeRequest(parent,tW,tH,&rW,&rH);
		switch (result)
		{
			case XtGeometryAlmost:
				result = XtMakeResizeRequest(parent,rW,rH,&tW,&tH);
			case XtGeometryNo:
			case XtGeometryYes:
			default:
			break;
		}
	}
	else
	{
		/*
		 * If only Height or BorderWidth deny the request.
		 */
		if ((request->request_mode == (CWHeight & CWBorderWidth)) ||
			(request->request_mode == CWHeight) ||
			(request->request_mode == CWBorderWidth))
			return(XtGeometryNo);

		/*
		 * If width, return yes only if height and border_width
		 * were NOT asked for.
		 */
		if (request->request_mode & CWWidth)
			if (request->request_mode != CWWidth)
			{
				if ((request->request_mode & CWHeight) ||
					(request->request_mode & CWBorderWidth))
				{
					reply->height = w->core.height;
					reply->border_width = w->core.border_width;
					return(XtGeometryAlmost);
				}
			}
	}

	DoLayout(parent);

	return(XtGeometryYes);
}

/*************************************<->*************************************
 *
 *	static Boolean SetValues(cur,req,new)
 *		XwTitleBarWidget	cur,req,new;
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
	XwTitleBarWidget	cur,req,new;
{
	Boolean flag = FALSE;
	Arg	args[25];
	char	*s;
	int	i,sW,sH,cH;
	WidgetList	kids = cur->composite.children;

	if (new->titlebar.string)
	{
		if (new->titlebar.string != cur->titlebar.string)
		{
			s = XtMalloc(XwStrlen(new->titlebar.string)+1);
			strcpy(s,new->titlebar.string);
			XtFree(cur->titlebar.string);
			new->titlebar.string = s;
		}
		SetSTextArgs(new,args);
		XtSetValues(new->titlebar.text,args,23);
		flag |= TRUE;
	}
	
	if (req->manager.layout == XwMINIMIZE)
	{
		sW = 2 * new->titlebar.internal_width;
		for(i=0; i < new->composite.num_children; i++)
			if (kids[i]->core.managed)
				sW += kids[i]->core.width + 
					(2 * kids[i]->core.border_width) +
					XwTB_Constraints(kids[i])->lpadding +
					XwTB_Constraints(kids[i])->rpadding;
		new->core.width = sW;

		sH = 0;
		for(i=0; i < new->composite.num_children; i++)
			if (kids[i]->core.managed)
			{
				cH = kids[i]->core.height +
					(2 * kids[i]->core.border_width);
				if (cH > sH)
					sH = cH;
			}
		sH += 2 * new->titlebar.internal_height;
		new->core.height = sH;
	}

	return(flag);
}

/*************************************<->*************************************
 *
 *	static void Select(tbw,event,params,num_params)
 *		XwTitleBarWidget	tbw;
 *		XEvent	*event;
 *		String	*params;
 *		Cardinal	num_params;
 *
 *   Description:
 *   -----------
 *		The action proc for selection on the title bar.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void Select(tbw,event,params,num_params)
	XwTitleBarWidget	tbw;
	XEvent	*event;
	String	*params;
	Cardinal	num_params;
{
	XtCallCallbacks(tbw,XtNselect,event);
}

/*************************************<->*************************************
 *
 *	static void Release(tbw,event,params,num_params)
 *		XwTitleBarWidget	tbw;
 *		XEvent	*event;
 *		String	*params;
 *		Cardinal	num_params;
 *
 *   Description:
 *   -----------
 *		The action proc for release on the title bar.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void Release(tbw,event,params,num_params)
	XwTitleBarWidget	tbw;
	XEvent	*event;
	String	*params;
	Cardinal	num_params;
{
	XtCallCallbacks(tbw,XtNrelease,event);
}

/*************************************<->*************************************
 *
 *	static void Leave(tbw,event,params,num_params)
 *		XwTitleBarWidget	tbw;
 *		XEvent	*event;
 *		String	*params;
 *		Cardinal	num_params;
 *
 *   Description:
 *   -----------
 *		The action proc for leave event on the title bar.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void Leave(tbw,event,params,num_params)
	XwTitleBarWidget	tbw;
	XEvent	*event;
	String	*params;
	Cardinal	num_params;
{
	XtCallCallbacks(tbw,XtNenter,event);
}

/*************************************<->*************************************
 *
 *	static void Enter(tbw,event,params,num_params)
 *		XwTitleBarWidget	tbw;
 *		XEvent	*event;
 *		String	*params;
 *		Cardinal	num_params;
 *
 *   Description:
 *   -----------
 *		The action proc for an enter event on the title bar.
 *
 *   Inputs:
 *   ------
 *		See X Toolkit documentation.
 * 
 *************************************<->***********************************/
static void Enter(tbw,event,params,num_params)
	XwTitleBarWidget	tbw;
	XEvent	*event;
	String	*params;
	Cardinal	num_params;
{
	XtCallCallbacks(tbw,XtNenter,event);
}

/*************************************<->*************************************
 *
 *	static void Redisplay(tbw)
 *		XwTitleBarWidget	tbw;
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
static void Redisplay(tbw)
	XwTitleBarWidget	tbw;
{
	/*
	DoLayout(tbw);
	*/
}
