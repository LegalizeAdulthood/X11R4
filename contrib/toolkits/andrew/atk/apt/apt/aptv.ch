/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidaptv_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/apt/apt/RCS/aptv.ch,v 1.2 89/06/05 18:27:37 gk5g Exp $";
#endif

/*
    $Log:	aptv.ch,v $
Revision 1.2  89/06/05  18:27:37  gk5g
Added method aptv_QueryDirectoryName().

Revision 1.1  89/04/28  17:45:48  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Apt View-object

MODULE	aptv.H

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Apt View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  02/23/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  aptv_VERSION		    1

#include "rect.h"

/*  Options  */

#define  aptv_Iconified		    (1<<0)
#define  aptv_SuppressControl	    (1<<1)
#define  aptv_SuppressBorder	    (1<<2)
#define  aptv_SuppressEnclosures    (1<<3)

/*  Print Options  */
#define  aptv_PrintPortrait	    (1<<0)
#define  aptv_PrintLandScape	    (1<<1)
#define  aptv_PrintLandscape	    (1<<1)
#define  aptv_PrintPreserveAspect   (1<<2)
#define  aptv_PrintFillPage	    (1<<3)

struct aptv_area
  {
  short				      size;
  struct rectangle		      bound;
  struct fontdesc		     *font;
  char				     *font_name;
  long				      strings_count;
  char				  *((*strings)[]);
  };

struct aptv_enclosure
  {
  struct rectangle		      bound;
  struct aptv_area		      areas[4];
  };

#define  EnclosureCount		      6

struct aptv_point
  {
  long				      x;
  long				      y;
  };

struct aptv_path
  {
  long				      count;
  struct aptv_point		      points[1];
  };


struct aptv_options
  {
  unsigned int		   controls_suppressed	    : 1;
  unsigned int		   border_suppressed	    : 1;
  unsigned int		   enclosures_suppressed    : 1;
  unsigned int		   initialize_iconified	    : 1;
  };

struct aptv_states
  {
  unsigned int		   initialized		    : 1;
  unsigned int		   inputfocus		    : 1;
  unsigned int		   shrinking		    : 1;
  unsigned int		   shrunk		    : 1;
  unsigned int		   help_displayed	    : 1;
  };


class aptv : view
  {
overrides:

  DesiredSize( long width, long height, enum view_DSpass pass, long *dWidth, long *dheight )
	returns enum view_DSattributes;
  FullUpdate( enum view_UpdateType type, long left, long top, long width, long height );
  Hit( enum view_MouseAction action, long x, long y, long n )	returns struct view *;
  SetDataObject( struct apt * );

methods:

  SetOptions( long options );
  SetDimensions( long width, long height );
  BuildFont( char *font_name, short *height )		    returns struct fontdesc *;
  DrawBoundedString( char *string, struct fontdesc *font,
	       struct rectangle *bounds, long center, long middle, long mode );
  ClearBoundedString( char *string, struct fontdesc *font,
		struct rectangle *bounds, long center, long middle, long mode );
  Within( long x, long y, struct rectangle *bounds )	    returns boolean;
  SetShrinkIcon( char icon, char *icon_font_name, char *title, char *title_font_name );
  Shrink();
  ShrinkView( struct aptv *aptv );
  Expand();
  ExpandView( struct aptv *aptv );
  ClearBody();

  PrintObject( FILE *file, char *processor, char *finalFormat, boolean topLevel, printer );
  PrintContinue();
  OpenPrintStream( FILE *file, processor, format, level ) returns boolean;
  ClosePrintStream();
  SetPrintStream( struct aptv_print_stream *stream );
  PreservePrintState();
  RestorePrintState();
  PrintBox( long left, long top, long width, long height, long mode );
  PrintRoundBox( long left, long top, long width, long height, long mode );
  PrintFilledRoundBox( long left, long top, long width, long height, long mode, long shade );
  PrintLine( long x1, long y1, long x2, long y2 );
  PrintCircle( long x1, long y1, long radius );
  PrintSlice( long x1, long y1, long radius, long start_angle, long end_angle,
	       long shade_numerator, long shade_denominator, long mode  );
  PrintMoveTo( long x, long y );
  PrintLineTo( long x, long y );
  SetPrintOrigin( long left, long top );
  SetPrintUnitDimensions( float inch_width, float inch_height );
  SetPrintPageDimensions( float inch_width, float inch_height );
  SetPrintResolution( float pixels_per_inch );
  SetPrintOptions( long options );
  SetPrintLineWidth( long width );
  SetPrintGrayLevel( float level );
  SetPrintPath( struct aptv_path *path );
  PrintPath( struct aptv_path *path );
  PrintPathFilled( struct aptv_path *path );
  SetPrintFont( char *font_name );
  ResetPrintFont( );
  PrintString( long x1, long y1, char *string, long mode );

  SetHelpString( char *string );
  SetHelpFileName( char *file_name);
  UseWaitCursor();
  UseNormalCursor();
  UseInvisibleCursor();
  Query( query, default_response, response )		    returns long;
  QueryFileName( query, response )			    returns long;
  QueryDirectoryName( query, response )			    returns long;
  Announce( message )					    returns long;

macromethods:

  IconFont()	    ((self)->icon_font)
  CursorFont()	    ((self)->cursor_font)
  PrintStream()	    ((self)->print_stream)
  Bounds()	    (&(self)->enclosures[0].bound)
  BodyBounds()	    (&(self)->enclosures[5].bound)
  BodyLeft()	    (aptv_BodyBounds(self)->left)
  BodyTop()	    (aptv_BodyBounds(self)->top)
  BodyWidth()	    (aptv_BodyBounds(self)->width)
  BodyHeight()	    (aptv_BodyBounds(self)->height)
  BodyCenter()	    (aptv_BodyLeft(self) + aptv_BodyWidth(self)/2)
  BodyRight()	    (aptv_BodyLeft(self) + aptv_BodyWidth(self) - 1)
  BodyMiddle()	    (aptv_BodyTop(self)  + aptv_BodyHeight(self)/2)
  BodyBottom()	    (aptv_BodyTop(self)  + aptv_BodyHeight(self) - 1)
  BypassUpdate()    ((self)->bypass_update)

classprocedures:

  InitializeObject( struct aptv *self ) returns boolean;
  FinalizeObject( struct aptv *self );

data:

  struct apt			    *data_object;
  struct aptv_options		     options;
  struct aptv_states		     states;
  struct fontdesc		    *icon_font, *cursor_font, *default_font;
  struct text			    *help_text;
  struct textview		    *help_textview;
  char				    *help_string;
  char				    *help_file_name;
  struct aptv_enclosure		     enclosures[EnclosureCount];
  struct rectangle		     shrinker_bounds;
  struct rectangle		     helper_bounds;
  long				     original_width, original_height;
  struct graphic		    *gray_fill, *white_fill;
  struct aptv_print_stream	    *print_stream;
  char				     shrink_icon[2];
  struct fontdesc		    *shrink_icon_font;
  struct rectangle		     shrink_icon_bounds;
  char				    *shrink_title;
  struct fontdesc		    *shrink_title_font;
  struct rectangle		     shrink_title_bounds;
  long				     dimension_width, dimension_height;
  char				     bypass_update;
  struct cursor			    *cursor;
  };
