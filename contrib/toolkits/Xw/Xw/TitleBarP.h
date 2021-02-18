/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        TitleBar.h
 **
 **   Project:     X Widgets
 **
 **   Description: TitleBar manager widget public include file
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

#define XwTB_CHUNK 30
#define XwTB_Constraints(widget) ((XwTitleBarConstraintRec *)widget->core.constraints)

typedef struct _XwTitleBarConstraintRec
{
/*
 * Public members; can be set via resources.
 */
	XwAlignment	region;
	int        	position;
	int        	lpadding;
	int        	rpadding;
	int        	precedence;
/*
 * Private members; set by Title Bar methods only.
 */
	Boolean	padding_honored;
	Boolean	hidden_by_titlebar;
	Widget	prec_sibling;
	Widget	next_position;
} XwTitleBarConstraintRec;

typedef struct _XwTitleBarClassPart 
{
	int mumble;
} XwTitleBarClassPart;

typedef struct _XwTitleBarClassRec 
{
	CoreClassPart     	core_class;
	CompositeClassPart  composite_class;
	ConstraintClassPart	constraint_class;
	XwManagerClassPart 	manager_class;
	XwTitleBarClassPart	titlebar_part;
} XwTitleBarClassRec;

extern XwTitleBarClassRec	XwtitlebarClassRec;

typedef struct _XwTitleBarPart 
{

/*
 * Public members; can be set via resources.
 */
 	XwAlignment	title_region;
	XwAlignment	title_string_alignment;
	Pixel	title_foreground;
	Pixel	title_background;
 	int	title_hspace;
	int	title_vspace;
	int	title_position;
	int	title_rpadding;
	int	title_lpadding;
	int	title_borderwidth;
	int	title_precedence;
	XtTranslations	title_translations;
	XtCallbackList	title_select;
 	XtCallbackList	title_release;
 	XtCallbackList	title_nexttop;
	int	internal_height;
	int	internal_width;
	XtCallbackList	enter;
	XtCallbackList	leave;
	XtCallbackList	select;
 	XtCallbackList	release;
 	XtCallbackList	next_top;
	Pixel	foreground;
	Pixel	highlight_color;
	int	highlight_style;
	int	highlight_tile;
	int	highlight_thickness;
	int	traversal_type;
 	char	*string;
 	Boolean	wrap;
 	int	line_space;
 	XFontStruct	*font;
/*
 * Private members; cannot be set via resources.
 */
	Widget	text;
	Boolean	width_given;
	Boolean	height_given;
	Boolean in_layout;
	XwAlignment	last_region;
	XwAlignment	leftover1;
	XwAlignment	leftover2;
	int	size_precedence;
	int	num_precedence;
	Widget	*precedence; /* used as a pointer to a Widget array */
	Widget	lpositions;
	Widget	cpositions;
	Widget	rpositions;
} XwTitleBarPart;

typedef struct _XwTitleBarRec
{
	CorePart	core;
	CompositePart	composite;
	ConstraintPart	constraint;
	XwManagerPart	manager;
	XwTitleBarPart	titlebar;
} XwTitleBarRec;
