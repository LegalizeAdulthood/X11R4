/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidbushv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/bush/RCS/bushv.ch,v 1.4 89/09/08 13:38:01 gk5g Exp $";
#endif

/*
    $Log:	bushv.ch,v $
Revision 1.4  89/09/08  13:38:01  gk5g
Added entry_filep to keep a handle of the currently visited file.

Revision 1.3  89/07/28  15:45:54  gk5g
Added new instance variables to hold state regarding the new and improved way in which we handle viewing entries (files).  Now we simply have a generic view and dataobject pair, as well as a ApplicationLayer view pointer.  The added functionality allows the proper ApplicationLayer to be used when view a specific view-type. That is, now the rasterview is used instead of just relying on textview to do something like this for us.

Revision 1.2  89/07/14  14:05:24  gk5g
Added new instance var, tree_type_node move_node, to support directory mv'ing from the nodes_level.

Revision 1.1  89/04/28  21:24:09  tom
Initial revision

Revision 1.1  89/04/28  21:20:44  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Bush View-object

MODULE	bushv.ch

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Bush View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  08/21/85	Created (TCP)
  01/15/89	Convert to ATK (GW Keim)

END-SPECIFICATION  ************************************************************/

#define MAXEDITORS		    25

class bushv: aptv{
    classprocedures:
	InitializeClass() returns boolean;
	InitializeObject( struct bushv *self ) returns boolean;
	FinalizeObject( struct bushv *self ) returns void;
	Create( char object ) returns struct bushv *;
    overrides:
	FullUpdate( enum view_UpdateType Type, long left, long top, long width, long height ) returns void;
	Hit( enum view_MouseAction action, long x, long y, long numberOfClicks ) returns struct view *;
	PostMenus( struct menulist *menulist ) returns void;
	PostKeyState( struct keystate *kstate ) returns void;
	SetDataObject( struct bush *bush ) returns void;
	GetApplicationLayer() returns struct view *;
	ReceiveInputFocus() returns void;
    data:
	struct bush		*bush;
	tree_type_node		 current_node, 
				 initial_node,
	                         move_node;
	struct Dir_Entry	*current_entry;
	int			 editor_index;
	char			 editor_program[1025];
	char			*editor_choices[MAXEDITORS];
	struct suite		*control_view;
	struct suite		*entries_view;
	struct treev		*dir_tree_view;
	struct view		*entry_view, *entry_view_application_layer;
	struct dataobject	*entry;
	FILE			*entry_filep;
	int			 num_prev_selected;
	struct lpair		*lp;
	int			 object;
	int			 sortmode;
	struct keystate		*keyState;
	struct keymap		*kmap;
	struct cursor		*cursor;
	struct menulist		*menulist;
	boolean			 detail;
	boolean			 top_level_inset, ignore_fullupdate;
	int			 debug;
};


