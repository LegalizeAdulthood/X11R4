/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidorgv_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/org/RCS/orgv.ch,v 1.8 89/08/23 15:39:33 tom Exp $";
#endif

/*
    $Log:	orgv.ch,v $
Revision 1.8  89/08/23  15:39:33  tom
Revise to Version 1.0

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Org View-Class

MODULE	org.ch

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Org View-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  05/24/89	Add ExposeDescription (TCP)
  05/25/89	Add Arrangement variable (TCP)
  06/06/89	Add override for Update method (TCP)
  06/07/89	Add First-time variable (TCP)
  08/23/89	Remove Create method (TCP)

END-SPECIFICATION  ************************************************************/


#define  orgv_VERSION	      1

class orgv : aptv
  {
overrides:

  SetDataObject( struct org *data );
  DesiredSize( long width, long height, enum view_DSpass pass, long *dWidth, long *dheight )
								returns enum view_DSattributes;
  FullUpdate( enum view_UpdateType type, long left, long top, long width, long height );
  Update();
  ReceiveInputFocus();
  LoseInputFocus();
  GetApplicationLayer()						returns struct view *;
  DeleteApplicationLayer( struct view * view );
  Hit( enum view_MouseAction action, long x, long y, long n)	returns struct view *;
  Print( FILE *file, char *processor, char *finalFormat, boolean topLevel );
  ObservedChanged( struct view *changed, long value );

methods:

  SetHitHandler( (long *handler)(), struct view *anchor );
  SetDebug( boolean state );

macromethods:

  CurrentNode()    		      ((self)->current_node)

classprocedures:

  InitializeClass()					returns boolean;
  InitializeObject( struct orgv *self )			returns boolean;
  FinalizeObject( struct orgv *self );

data:

  struct org			 *data_object;
  struct lpair			 *pair_view;
  struct treev			 *tree_view_object;
  struct text			 *description_text;
  struct textview		 *description_textview;
  struct view			 *description_textview_scroll;
  struct tree_node		 *previous_node, *exploded_node;
  struct suite			 *suite_object;
  struct view			 *anchor;
  struct view		       *(*hit_handler)();
  struct rectangle		  bounds;
  struct menulist		 *menu;
  struct keystate		 *keystate;
  long				  last_modified, initial_node_count,
				  description_last_modified;
  short				  arrangement, node_border_style, node_connector_style,
				  background_shade;
  boolean			  input_focus, expose_controls, controls_exposed,
				  application, application_layer, fold, first_time,
				  expose_description, description_exposed, read_only,
				  exploded, ignore_loseinputfocus, ignore_fullupdate;
  };
