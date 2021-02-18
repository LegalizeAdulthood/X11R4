/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        PanelP.h
 **
 **   Project:     X Widgets
 **
 **   Description: Panel Manager Widget private include file
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


/*********************************************************************
 *
 * Panel Widget Constraint Structure
 *
 *********************************************************************/

typedef struct _XwPanelConstraintRec 
{
	/* Public */
	XwWidgetType	type;
	Boolean	causes_resize;
	/* Private */
	int req_height;
	int	req_width;
	int	req_border_width;
	Boolean	was_managed;
	int height_last_managed;
	int width_last_managed;
	int	border_width_last_managed;
} XwPanelConstraintRec;

/*********************************************************************
 *
 * Panel Widget Private Data
 *
 *********************************************************************/

/* New fields for the Panel widget class record */
typedef struct _XwPanelClassPart {
	int mumble;   /* No new procedures */
} XwPanelClassPart;

/* Full class record declaration */
typedef struct _XwPanelClassRec {
	CoreClassPart	core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XwManagerClassPart	manager_class;
	XwPanelClassPart	panel_class;
} XwPanelClassRec;

extern XwPanelClassRec XwpanelClassRec;

/* New fields for the Panel widget record */
typedef struct _XwPanelPart {
	/* Public */
	int	internal_width;
	int	internal_height;
	int	title_to_pulldown;
	int	wspace_to_sibling;
	Boolean	top_level;
	Boolean	display_title;
	/* Private */
	Boolean	title_on;
	Widget	title;
	Widget	pulldown;
	Widget	work_space;
} XwPanelPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XwPanelRec {
	CorePart	core;
	CompositePart	composite;
	ConstraintPart	constraint;
	XwManagerPart	manager;
	XwPanelPart	panel;
} XwPanelRec;

