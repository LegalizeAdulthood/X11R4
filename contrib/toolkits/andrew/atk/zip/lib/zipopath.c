/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* zipopath.c	Zip Object -- Path					      */
/* Author	TC Peters						      */
/* Information Technology Center		   Carnegie-Mellon University */

static char	rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipopath.c,v 2.11 89/09/08 17:42:11 ghoti Exp $";

/*
    $Log:	zipopath.c,v $
 * Revision 2.11  89/09/08  17:42:11  ghoti
 * removal of unused variables
 * 
 * Revision 2.10  89/09/08  14:16:49  sg08
 * Modify Draw to handle rubberbanding on Edit Modifications as well as work right on Builds.
 * 
 * Revision 2.9  89/08/30  16:26:42  sg08
 * Removed excess SetTransferMode activity. Accomodate non-refresh on builds.
 * 
 * Revision 2.8  89/05/19  20:38:28  tom
 * Improve X performance by pre-checking TransferMode.
 * 
 * Revision 2.7  89/05/01  22:14:34  tom
 * Use special symbolic font-names.
 * 
 * Revision 2.6  89/02/08  16:50:56  ghoti
 * change copyright notice
 * 
 * Revision 2.5  89/02/07  19:53:53  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 2.4  88/11/18  21:11:26  tom
 * Handle variable line-widths.
 * 
 * Revision 2.3  88/11/16  18:30:46  tom
 * null.
 * 
 * Revision 2.2  88/10/11  20:33:13  tom
 * Change Printing interface to remove pane and figure args.
 * 
 * Revision 2.1  88/09/27  18:16:35  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:40:44  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:45:40  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- Internal Facility Suite  *********************************

TITLE	Zip Object -- Path

MODULE	zipopath.c

NOTICE	IBM Internal Use Only

DESCRIPTION

.......................

HISTORY
  04/13/88	Created (TC Peters)
  05/01/89	Use symbolic font-names (TCP)
  05/15/89	Improve performance by pre-checking TransferMode (TCP)
   08/24/89	Remove excess SetTransferMode() activity in Draw() (SCG)
   09/08/89	Fix Draw() to handle rubberbanding in both Builds and Edit Modifications (SCG)

END-SPECIFICATION  ************************************************************/

#include "class.h"
#include "zipobj.ih"
#include "zipopath.eh"

char
zipopath__Object_Icon( self )
  register struct zipopath		 *self;
  {
  IN(zipopath__Object_Icon);
  OUT(zipopath__Object_Icon);
  return  'F';
  }

char
zipopath__Object_Icon_Cursor( self )
  register struct zipopath		 *self;
  {
  IN(zipopath__Object_Icon_Cursor);
  OUT(zipopath__Object_Icon_Cursor);
  return  'L';
  }

char
zipopath__Object_Datastream_Code( self )
  register struct zipopath		 *self;
  {
  IN(zipopath__Object_Datastream_Code);
  OUT(zipopath__Object_Datastream_Code);
  return  'H';
  }

long
zipopath__Show_Object_Properties( self, pane, figure )
  register struct zipopath		 *self;
  register zip_type_pane		  pane;
  register zip_type_figure		  figure;
  {
  zipview_Announce( View, "Draw Path from Start to Finish in free-form." );
  return  zip_ok;
  }

long
zipopath__Build_Object( self, pane, action, x, y, clicks, X, Y )
  register struct zipopath		 *self;
  register zip_type_pane		  pane;
  register long				  action, x, y, clicks;
  register zip_type_point		  X, Y;
  {
  register long				  status = zip_ok;
  long					  position = 0;
  static long				  initial_x, initial_y,
					  initial_X, initial_Y,
					  prior_X, prior_Y;

  IN(zipopath__Build_Object);
  switch ( action )
    {
    case view_LeftDown:
      initial_x = x;  initial_y = y;
      initial_X = prior_X = X;  initial_Y = prior_Y = Y;
      if ( (status = zip_Create_Figure( Data, &CurrentFigure, NULL,
		zip_path_figure, CurrentImage, position )) == zip_success )
        {
        zipopath_Set_Object_Point( self, CurrentFigure,
		zip_figure_origin_point, X, Y );
  	CurrentFigure->zip_figure_zoom_level =
	    pane->zip_pane_zoom_level;
	pane->zip_pane_edit->zip_pane_edit_build_figure = false;
	pane->zip_pane_edit->zip_pane_edit_last_point_id = zip_figure_origin_point;
	zip_Set_Figure_Shade( Data, CurrentFigure,
			      pane->zip_pane_edit->zip_pane_edit_current_shade );
	}
      break;
    case view_LeftUp:
      if ( pane->zip_pane_edit->zip_pane_edit_last_point_id == zip_figure_origin_point )
	{
	zipedit_Delete_Figure( Edit, CurrentFigure, pane );
	break;
	}
      if ( abs(initial_x - x) <= 10  &&  abs(initial_y - y) <= 10 )
	{ DEBUG(Close Path); X = initial_X;  Y = initial_Y; }
      /* Fall-thru */
    case view_LeftMovement:
      if ( CurrentFigure )
	{
	if ( X != prior_X  ||  Y != prior_Y )
	  {
          zipopath_Set_Object_Point( self, CurrentFigure,
		     ++pane->zip_pane_edit->zip_pane_edit_last_point_id,
		     prior_X = X, prior_Y = Y );
          zipview_Draw_Figure( View, CurrentFigure, pane );
	  pane->zip_pane_edit->zip_pane_edit_build_figure = false;
	  }
	}
      break;
    }
  OUT(zipopath__Build_Object);
  return  status;
  }

long
zipopath__Draw_Object( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  if ( zipview_Condition( View, pane, figure, zip_draw ) )
    status = Draw( self, figure, pane, zip_draw );
  OUT(zipopath__Draw_Object);
  return  status;
  }

long
zipopath__Clear_Object( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  IN(zipopath__Clear_Object);
  if ( zipview_Condition( View, pane, figure, zip_clear ) )
    status = Draw( self, figure, pane, zip_clear );
  OUT(zipopath__Clear_Object);
  return  status;
  }

static
Draw( self, figure, pane, action )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register short			  action;
  {
  register long				  status = zip_ok, i, count, id;
  register unsigned char		  shade, allocated = false,
					  closure = false, width;
  struct point				  point_vector[101];
  register struct point			 *points = point_vector;

  IN(Draw);
  count = figure->zip_figure_points->zip_points_count + 1;
  DEBUGdt(Point-count,count);
  if ( count > 100 )
    {
    allocated = true;
    points = (struct point *) malloc( count * sizeof(struct point) );
    }
  points[0].x = window_x_point;  points[0].y = window_y_point;
  DEBUGdt(  X,points[0].x); DEBUGdt(  Y,points[0].y);
  for ( i = 1; i < count; i++ )
    {
    points[i].x = window_x_points(i-1);
    points[i].y = window_y_points(i-1);
    DEBUGdt(X,points[i].x); DEBUGdt(Y,points[i].y);
    }
  if ( points[0].x == points[count-1].x  &&  points[0].y == points[count-1].y )
    closure = true;
  if ( closure  && (shade = zip_Contextual_Figure_Shade( Data, figure )) )
    {
    if ( View->mouse_action != view_LeftMovement  &&  action == zip_draw )
      {
      if ( (shade = ('0' + ((shade + 10) / 10)) - 1) > '9' )  shade = '9';
      DEBUGdt(Shade-index,shade);
      zipview_FillPolygon( View, points, count,
        zipview_Define_Graphic( View, zip_Define_Font( Data, ShadeFontName, NULL ), shade ) );
      }
      else
      if ( action == zip_clear )
	{
        zipview_FillPolygon( View, points, count,
          zipview_Define_Graphic( View, zip_Define_Font( Data, ShadeFontName, NULL ), shade ) );
	}
    }
  if ( (width = zip_Contextual_Figure_Line_Width( Data, figure )) > 0  ||
       View->mouse_action == view_LeftMovement )
    {
    if ( zipview_GetLineWidth( View ) != width )
      zipview_SetLineWidth( View, width );
    if ( pane->zip_pane_edit && pane->zip_pane_state.zip_pane_state_paint_inverted )
     {
	id = pane->zip_pane_edit->zip_pane_edit_last_point_id - 1;
        if ( id == count - 1 )
	{
	  zipview_MoveTo( View, points[id - 1].x, points[id - 1].y );
	  zipview_DrawLineTo( View, points[id].x, points[id].y );
	  if ( closure ) zipview_DrawLineTo( View, points[0].x, points[0].y );
	}
        else
	{
          if ( id )
          {
  	    zipview_MoveTo( View, points[id - 1].x, points[id - 1].y );
	    zipview_DrawLineTo( View, points[id].x, points[id].y );
	    if ( id + 1  < count )
	      zipview_DrawLineTo( View, points[id + 1].x, points[id + 1].y );
	  }
          else
	  {
	    zipview_MoveTo( View, points[1].x, points[1].y );
  	    zipview_DrawLineTo( View, points[0].x, points[0].y );
            if ( closure )
  	     zipview_DrawLineTo( View, points[count - 2].x, points[count - 2].y );
	  }
	}
     }
     else if ( closure ) zipview_DrawPolygon( View, points, count );
     else zipview_DrawPath( View, points, count );
    }
  if ( ExposePoints )
    zipopath_Expose_Object_Points( self, figure, pane );
  if ( HighlightPoints )
    zipopath_Highlight_Object_Points( self, figure, pane );
  if ( allocated )   free( points );
  OUT(Draw);
  return  status;
  }

long
zipopath__Print_Object( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  pc, status = zip_ok;
  zip_type_point_pairs	    		  points = NULL;

  IN(zipopath__Print_Object);
  if ( (status = zip_Allocate_Figure_Points_Vector( Data, &points )) == zip_success )
    {
    points->zip_points_count = 0;
    for ( pc = 0; pc < figure->zip_figure_points->zip_points_count; pc++ )
      {
      if ( status == zip_success )
        {
        points->zip_points[pc].zip_point_x = print_x_points(pc);
        points->zip_points[pc].zip_point_y = print_y_points(pc);
        points->zip_points_count++;
        status = zip_Enlarge_Figure_Points_Vector( Data, &points );
        }
      }
    zipprint_Set_Shade( Print, figure->zip_figure_fill.zip_figure_shade );
    zipprint_Set_Line_Width( Print, zip_Contextual_Figure_Line_Width( Data, figure ) );
    zipprint_Draw_Multi_Line( Print, figure->zip_figure_points->zip_points_count,
			      print_x_point, print_y_point, points );
    free( points );
    }
  OUT(zipopath__Print_Object);
  return  status;
  }

long
zipopath__Within_Object( self, figure, pane, x, y )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		  x, y;
  {
  register long				  distance = -1,
					  X1, Y1, X2, Y2,
					  X, Y, XA, XB, YA, YB;

  IN(zipopath__Within_Object);
  X1 = window_x_point;	    Y1 = window_y_point;
  X2 = window_x_points(0);  Y2 = window_y_points(0);
  if ( x >= X1  &&  x <= X2  &&  y >= Y1  &&  y <= Y2 )
    {
    if ( (XA = abs(x - X1)) < (XB = abs(x - X2)) )
      X = XA;
      else
      X = XB;
    if ( (YA = abs(y - Y1)) < (YB = abs(y - Y2)) )
      Y = YA;
      else
      Y = YB;
    distance = (X * X) + (Y * Y);
    }
  OUT(zipopath__Within_Object);
  return  distance;
  }

long
zipopath__Proximate_Object_Points( self, figure, pane, x, y )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		  x, y;
  {
  register int				  i, point = 0;

  IN(zipopath__Proximate_Object_Points);

  if ( zipview_Proximate_Figure_Point( View, pane, figure,
				       window_x_point, window_y_point, x, y ) )
    point = zip_figure_origin_point;
    else
    for ( i = 0;
	   i < figure->zip_figure_points->zip_points_count;
	    i++ )
      if (  zipview_Proximate_Figure_Point( View, pane, figure,
				     window_x_points(i),
				     window_y_points(i), x, y ) )
	{
	point = zip_figure_auxiliary_point + i;
        break;
	}

  OUT(zipopath__Proximate_Object_Points);
  return  point;
  }

boolean
zipopath__Enclosed_Object( self, figure, pane, x, y, w, h )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		  x, y, w, h;
  {
  register boolean			  enclosed = true;
  register long				  i, right = x + w, bottom = y + h;
  register zip_type_pixel		  X, Y;

  IN(zipopath__Enclosed_Object);
  X = window_x_point;  Y = window_y_point;
  if ( X < x  ||  Y < y  ||  X > right  ||  Y > bottom )
    enclosed = false;
  for ( i = 0; enclosed == true  &&
	i < figure->zip_figure_points->zip_points_count; i++ )
    {
    X = window_x_points(i);  Y = window_y_points(i);
    if ( X < x  ||  Y < y  ||  X > right  ||  Y > bottom )
      enclosed = false;
    }
  OUT(zipopath__Enclosed_Object);
  return  enclosed;
  }

long
zipopath__Object_Enclosure( self, figure, pane, x, y, w, h )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		 *x, *y, *w, *h;
  {
  register zip_type_pixel		  max_x, min_x, max_y, min_y, X, Y;
  register long				  i;

  IN(zipopath__Object_Enclosure);
  max_x = min_x = window_x_point;  max_y = min_y = window_y_point;
  for ( i = 0; i < figure->zip_figure_points->zip_points_count; i++ )
    {
    if ( (X = window_x_points(i)) > max_x )
      max_x = X;
      else
      if ( X < min_x )
        min_x = X;
    if ( (Y = window_y_points(i)) > max_y )
      max_y = Y;
      else
      if ( Y < min_y )
        min_y = Y;
    }
  *x = min_x;  *y = min_y;
  *w = abs(max_x - min_x);
  *h = abs(max_y - min_y);
  OUT(zipopath__Object_Enclosure);
  return  zip_ok;
  }

long
zipopath__Highlight_Object_Points( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  i, status = zip_ok;

  IN(zipopath__Highlight_Object_Points);
  zipedit_Highlight_Point( Edit, pane, window_x_point, window_y_point );
  for ( i = 0; i < figure->zip_figure_points->zip_points_count; i++ )
    zipedit_Highlight_Point( Edit, pane, window_x_points(i), window_y_points(i) );
  OUT(zipopath__Highlight_Object_Points);
  return  status;
  }

long
zipopath__Normalize_Object_Points( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  i, status = zip_ok;

  IN(zipopath__Normalize_Object_Points);
  zipedit_Normalize_Point( Edit, pane, window_x_point, window_y_point );
  for ( i = 0; i < figure->zip_figure_points->zip_points_count; i++ )
    zipedit_Normalize_Point( Edit, pane, window_x_points(i), window_y_points(i) );
  OUT(zipopath__Normalize_Object_Points);
  return  status;
  }

long
zipopath__Expose_Object_Points( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  i, status = zip_ok;

  IN(zipopath__Expose_Object_Points);
  zipedit_Expose_Point( Edit, pane, figure, figure_x_point, figure_y_point );
  for ( i = 0; i < figure->zip_figure_points->zip_points_count; i++ )
    zipedit_Expose_Point( Edit, pane, figure, figure_x_points(i), figure_y_points(i) );
  OUT(zipopath__Expose_Object_Points);
  return  status;
  }

long
zipopath__Hide_Object_Points( self, figure, pane )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  i, status = zip_ok;

  IN(zipopath__Hide_Points);
  zipedit_Hide_Point( Edit, pane, figure, figure_x_point, figure_y_point );
  for ( i = 0; i < figure->zip_figure_points->zip_points_count; i++ )
    zipedit_Hide_Point( Edit, pane, figure, figure_x_points(i), figure_y_points(i) );
  OUT(zipopath__Hide_Points);
  return  status;
  }

long
zipopath__Set_Object_Point( self, figure, point, x, y )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register int				  point;
  register zip_type_point		  x, y;
  {
  register long				  status = zip_ok;

  IN(zipopath__Set_Object_Point);
  if ( point == zip_figure_origin_point )
    {
    if ( zip_Contextual_Figure_Shade( Data, figure ) &&
         figure->zip_figure_points  &&
	 figure->zip_figure_points->zip_points_count > 2  &&
         figure->zip_figure_points->zip_points
	  [figure->zip_figure_points->zip_points_count - 1].zip_point_x ==
	    figure->zip_figure_point.zip_point_x  &&
         figure->zip_figure_points->zip_points
	  [figure->zip_figure_points->zip_points_count - 1].zip_point_y ==
	    figure->zip_figure_point.zip_point_y
	)
      { DEBUG(Closed Filled Figure);
      figure->zip_figure_points->zip_points
	[figure->zip_figure_points->zip_points_count - 1].zip_point_x = x;
      figure->zip_figure_points->zip_points
	[figure->zip_figure_points->zip_points_count - 1].zip_point_y = y;
      }
    figure->zip_figure_point.zip_point_x = x;
    figure->zip_figure_point.zip_point_y = y;
    }
  else
  if ( point == zip_figure_auxiliary_point )
    {
    if ( figure->zip_figure_points == NULL )
      if ( (status = zip_Allocate_Figure_Points_Vector(
		    Data, &figure->zip_figure_points )) == zip_success )
	figure->zip_figure_points->zip_points_count = 1;
    if ( status == zip_success )
      {
      figure->zip_figure_points->zip_points[0].zip_point_x = x; 
      figure->zip_figure_points->zip_points[0].zip_point_y = y;
      }
    }
  else
  if ( (point - zip_figure_auxiliary_point) <
       figure->zip_figure_points->zip_points_count )
    {
    figure->zip_figure_points->zip_points[point -
	zip_figure_auxiliary_point].zip_point_x = x;
    figure->zip_figure_points->zip_points[point -
	zip_figure_auxiliary_point].zip_point_y = y;
    }
  else
  if ( (point - zip_figure_auxiliary_point) == figure->zip_figure_points->zip_points_count )
    {
    if ( (status = zip_Enlarge_Figure_Points_Vector(
	    Data, &figure->zip_figure_points )) == zip_success )
      {
      figure->zip_figure_points->zip_points[point - zip_figure_auxiliary_point].zip_point_x = x;
      figure->zip_figure_points->zip_points[point - zip_figure_auxiliary_point].zip_point_y = y;
      figure->zip_figure_points->zip_points_count++;
      }
    }
  else status = zip_failure; /*=== zip_invalid_point_type (more than 1 beyond allocation) ===*/
  if ( status == zip_success )
    {
    zip_Set_Image_Extrema( Data, figure->zip_figure_image, x, y );
    zip_Set_Stream_Extrema( Data, figure->zip_figure_image->zip_image_stream,
			    figure->zip_figure_image );
    }
  OUT(zipopath__Set_Object_Point);
  return  status;
  }

long
zipopath__Object_Point( self, figure, point, x, y )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register long				  point;
  register zip_type_point		  *x, *y;
  {
  register long				  status = zip_ok;

  IN(zipopath__Object_Point);
/*===*/  *x = *y = 0;
  OUT(zipopath__Object_Point);
  return  status;
  }

long
zipopath__Adjust_Object_Point_Suite( self, figure, x_delta, y_delta )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  register zip_type_point		  x_delta, y_delta;
  {
  register long				  i, status = zip_ok;

  IN(zipopath__Adjust_Object_Point_Suite);
  figure_x_point += x_delta;
  figure_y_point += y_delta;
  for ( i = 0; i < figure->zip_figure_points->zip_points_count; i++ )
    {
    figure->zip_figure_points->zip_points[i].zip_point_x += x_delta;
    figure->zip_figure_points->zip_points[i].zip_point_y += y_delta;
    zip_Set_Image_Extrema( Data, figure->zip_figure_image,
		figure->zip_figure_points->zip_points[i].zip_point_x,
		figure->zip_figure_points->zip_points[i].zip_point_y  );
/*===handle both extrema 7/20/86===*/
/*===have extrema check for REDUCTIONS as well as EXPANSIONS 7/20/86===*/
    zip_Set_Stream_Extrema( Data, figure->zip_figure_image->zip_image_stream,
			    figure->zip_figure_image );
    }
  OUT(zipopath__Adjust_Object_Point_Suite);
  return  status;
  }


long
zipopath__Set_Object_Shade( self, figure, shade )
  register struct zipopath		 *self;
  register zip_type_figure		  figure;
  {
  IN(zipopath__Set_Object_Shade);
  figure->zip_figure_fill.zip_figure_shade = shade;
  if ( shade >= 1  &&  shade <= 100 )
    figure->zip_figure_mode.zip_figure_mode_shaded = on;
    else
    figure->zip_figure_mode.zip_figure_mode_shaded = off;
  OUT(zipopath__Set_Object_Shade);
  return  zip_ok;
  }
