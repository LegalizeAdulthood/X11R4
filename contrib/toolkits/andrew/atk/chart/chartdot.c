/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#ifndef lint
static char *rcsidchartdot = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/chart/RCS/chartdot.c,v 1.8 89/09/07 16:50:15 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Dot Chart View-object

MODULE	chartdot.c

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
  03/28/89	Created (TCP)
  05/04/89	Eliminate Dot connecting-lines (now in chartlin formatter) (TCP)
  05/08/89	Use ZipDot20 for Dot-faces (TCP)
  05/10/89	Depend upon pre-computed data-points in DrawChart (TCP)
  05/31/89	Add classID parameter in FinalizeObject (TCP)
  06/02/89	Use super-class Hit method (TCP)
  09/05/89	Upgrade to V1.0 (TCP)

END-SPECIFICATION  ************************************************************/

#include <math.h>
#include "graphic.ih"
#include "observe.ih"
#include "view.ih"
#include "im.ih"
#include "rect.h"
#include "apt.h"
#include "aptv.ih"
#include "chartobj.ih"
#include "chart.ih"
#include "chartdot.eh"
#include <ctype.h>

#define  Bounds			(chartdot_ChartBounds(self))
#define  Left			(chartdot_ChartLeft(self))
#define  Top			(chartdot_ChartTop(self))
#define  Width			(chartdot_ChartWidth(self))
#define  Height			(chartdot_ChartHeight(self))
#define  Right			(chartdot_ChartRight(self))
#define  Bottom			(chartdot_ChartBottom(self))

#define  Items			(chartdot_Items(self))
#define  ItemBounds(s)		(chartdot_ItemBounds(self,(s)))
#define  ItemLeft(s)		(chartdot_ItemLeft(self,(s)))
#define  ItemTop(s)		(chartdot_ItemTop(self,(s)))
#define  ItemWidth(s)		(chartdot_ItemWidth(self,(s)))
#define  ItemHeight(s)		(chartdot_ItemHeight(self,(s)))
#define  ItemCenter(s)		(chartdot_ItemCenter(self,(s)))
#define  ItemMiddle(s)		(chartdot_ItemMiddle(self,(s)))
#define  ItemX(s)		(chartdot_ItemX(self,(s)))
#define  ItemY(s)		(chartdot_ItemY(self,(s)))
#define  NextItem(s)		(chartdot_NextItem(self,(s)))

#define  PixelsPerUnit		(chartdot_PixelsPerUnit( self ))
#define  PixelsPerInterval	(chartdot_PixelsPerInterval( self ))

static struct fontdesc		*dot_font;
#define  DotFont		(dot_font)

boolean 
chartdot__InitializeObject( classID, self)
  register struct classheader	 *classID;
  register struct chartdot	 *self;
  {
  IN(chartdot_InitializeObject);
  chartdot_SetShrinkIcon( self, 'e', "icon12", "DotChart", "andysans10b" );
  chartdot_SetHelpFileName( self, "/usr/andy/help/ez.help"/*=== ===*/ );
  OUT(chartdot_InitializeObject);
  return  TRUE;
  }

void
chartdot__FinalizeObject( classID, self )
  register struct classheader	 *classID;
  register struct chartdot	 *self;
  {}

void
chartdot__SetDebug( self, state )
  register struct chartdot	 *self;
  register char			  state;
  {
  IN(chartdot_SetDebug);
  super_SetDebug( self, debug = state );
  OUT(chartdot_SetDebug);
  }

char *
chartdot__Moniker( self )
  register struct chartdot   *self;
  {
  IN(chartdot_Moniker);
  OUT(chartdot_Moniker);
  return  "Dot";
  }

void
chartdot__DrawChart( self )
  register struct chartdot	     *self;
  {
  register struct chart_item_shadow  *shadow = Items;

  IN(chartdot_DrawChart);
  if ( DotFont == NULL )
    DotFont = chartdot_BuildFont( self, "zipdot20", NULL );
  chartdot_SetFont( self, DotFont );
  while ( shadow )
    {
    DEBUGdt(Value,chart_ItemAttribute( Data, shadow->item, chart_ItemValue(0) ));
    chartdot_MoveTo( self, ItemX(shadow), ItemY(shadow) );
    chartdot_DrawString( self, "C", NULL );
    shadow = NextItem(shadow);
    }
  OUT(chartdot_DrawChart);
  }

void
chartdot__PrintChart( self )
  register struct chartdot	     *self;
  {
  register long			      i, left, top, width,
				      count = chart_ItemCount( Data );
  register struct chart_item	     *chart_item = chart_ItemAnchor( Data );
  register struct aptv_path	     *path;
  register struct aptv_point	     *path_point;

  IN(chartdot_PrintChart);
  path = (struct aptv_path *)
     malloc( sizeof(struct aptv_path) + 5 * sizeof(struct aptv_point) );
  DEBUGdt(ItemCount,count);
  width = (Width / ((count) ? count : 1)) - 1;
  left = Left +( width / 2) + (Width - ((width + 1) * count)) / 2;
  chartdot_SetPrintLineWidth( self, 1 );
  path->count = 5;
  path_point = &path->points[0];
  for ( i = 0; i < chart_ItemCount( Data)  &&  chart_item; i++ )
    {
    DEBUGdt(Value,chart_ItemAttribute( Data, chart_item, chart_ItemValue(0) ));
    top = Bottom - (chart_ItemAttribute( Data, chart_item, chart_ItemValue(0) ) * PixelsPerUnit);
    path_point = &path->points[0];
    path_point->x = left;	    path_point->y = top;	    path_point++;
    path_point->x = left + 5;	    path_point->y = top;	    path_point++;
    path_point->x = left + 5;	    path_point->y = top + 5;	    path_point++;
    path_point->x = left;	    path_point->y = top + 5;	    path_point++;
    path_point->x = left;	    path_point->y = top;
    chartdot_SetPrintGrayLevel( self, 0.7 );
    chartdot_PrintPathFilled( self, path );
    chartdot_SetPrintGrayLevel( self, 0.0 );
    chartdot_PrintPath( self, path );
    left += width + 1;
    chart_item = chart_NextItem( Data, chart_item );
    }
  if ( path )  free( path );
  OUT(chartdot_PrintChart);
  }
