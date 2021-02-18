/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidchartv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/chart/RCS/chartv.ch,v 1.3 89/09/06 17:28:42 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Chart View-object

MODULE	chartv.ch

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Chart View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  03/23/89	Created (TCP)
  09/01/89	Upgrade to V1.0

END-SPECIFICATION  ************************************************************/

#define  chartv_VERSION		    1

class chartv : aptv
  {
overrides:

  SetDataObject( struct chart * );
  FullUpdate( enum view_UpdateType type, long left, long top, long width, long height );
  ReceiveInputFocus();
  LoseInputFocus();
  GetApplicationLayer()					returns struct view *;
  DeleteApplicationLayer( struct view * view );
  Hit( enum view_MouseAction action, long x, long y, long n )	returns struct view *;
  Print( FILE *file, char *processor, char *finalFormat, boolean topLevel );

methods:

  SetChartAttribute( long code_value )			returns long;
  ChangeChartAttribute( long code_value )		returns long;
  ChartAttribute( long attribute_code )			returns long;
  CurrentItem()						returns struct chart_item *;
  SetDebug( boolean state );

classprocedures:

  Create( chartv_Specification, char *anchor )		returns struct chartv *;
  InitializeClass()					returns boolean;
  InitializeObject( struct chartv *self )		returns boolean;
  FinalizeObject( struct chartv *self );

data:

  struct chartv_instance     *instance;
  };

/***  Item Border Styles  ***/
#define  chartv_Rectangle		    (0)
#define  chartv_Circle			    (1<<0)
#define  chartv_Oval			    (1<<1)
#define  chartv_RoundAngle		    (1<<2)
#define  chartv_Roundangle		    (1<<2)

/****  Placements  ***/
#define  chartv_Center			    (0)
#define  chartv_Left			    (1<<0)
#define  chartv_Right			    (1<<1)
#define  chartv_Top			    (1<<2)
#define  chartv_Bottom			    (1<<3)

/***  Highlight Styles ***/
#define  chartv_Invert			    (0)
#define  chartv_Border			    (1<<0)
#define  chartv_Bold			    (1<<1)
#define  chartv_Italic			    (1<<2)
#define  chartv_Pale			    (1<<3)

/***  Arrangements ***/
#define  chartv_Horizontal		    (0)
#define  chartv_Vertical		    (1<<0)

/***  Attribute Macros  ***/

#define  chartv_Arrangement(x)		    chartv_arrangement,		(long) x
#define  chartv_BackgroundShade(x)	    chartv_backgroundshade,	(long) x
#define  chartv_BorderStyle(x)		    chartv_borderstyle,		(long) x
#define  chartv_BorderSize(x)		    chartv_bordersize,		(long) x
#define  chartv_Cursor(x)		    chartv_cursor,		(long) x
#define  chartv_CursorFontName(x)	    chartv_cursorfontname,	(long) x
#define  chartv_Datum(x)		    chartv_datum,		(long) x
#define  chartv_HitHandler(x)		    chartv_hithandler,		(long) x
#define  chartv_ItemBorderStyle(x)	    chartv_itemborderstyle,	(long) x
#define  chartv_ItemBorderSize(x)	    chartv_itembordersize,	(long) x
#define  chartv_ItemHighlightStyle(x)	    chartv_itemhighlightstyle,	(long) x
#define  chartv_LabelFontName(x)	    chartv_labelfontname,	(long) x
#define  chartv_ScaleFontName(x)	    chartv_scalefontname,	(long) x
#define  chartv_TitleBorderStyle(x)	    chartv_titleborderstyle,	(long) x
#define  chartv_TitleBorderSize(x)	    chartv_titlebordersize,	(long) x
#define  chartv_TitleCaptionFontName(x)	    chartv_titlecaptionfontname,(long) x
#define  chartv_TitleDataObjectHandler	    chartv_titledataobjecthandler, (long) x
#define  chartv_TitleHighlightStyle(x)	    chartv_titlehighlightstyle,	(long) x
#define  chartv_TitleHitHandler(x)	    chartv_titlehithandler,	(long) x
#define  chartv_TitlePlacement(x)	    chartv_titleplacement,	(long) x
#define  chartv_TitleViewObjectHandler	    chartv_titleviewobjecthandler, (long) x

/***  Attribute Codes  ***/

#define  chartv_arrangement		    1
#define  chartv_backgroundshade		    2
#define  chartv_borderstyle		    3
#define  chartv_bordersize		    4
#define  chartv_cursor	    		    5
#define  chartv_cursorfontname  	    6
#define  chartv_datum			    7
#define  chartv_hithandler		    8
#define  chartv_itemborderstyle		    10
#define  chartv_itembordersize		    11
#define  chartv_itemhighlightstyle	    12
#define  chartv_labelfontname		    13
#define  chartv_scalefontname		    14
#define  chartv_titleborderstyle	    20
#define  chartv_titlebordersize		    21
#define  chartv_titlecaptionfontname	    22
#define  chartv_titlehighlightstyle	    23
#define  chartv_titlehithandler		    24
#define  chartv_titleplacement		    25
#define  chartv_titledataobjecthandler	    26
#define  chartv_titleviewobjecthandler	    27

typedef struct chartv_specification  chartv_Specification;
struct  chartv_specification
  {
  char	attribute;
  long	value;
  };
