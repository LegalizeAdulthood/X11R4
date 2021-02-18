/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipds02.c,v 2.5 89/02/17 18:07:12 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipds02.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipds02.c,v 2.5 89/02/17 18:07:12 ghoti Exp $";
#endif /* lint */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/* zipds02.c	Zip Data-object	-- Stream Output Parsing	      */
/* Author	TC Peters					      */
/* Information Technology Center	   Carnegie-Mellon University */


/*
    $Log:	zipds02.c,v $
 * Revision 2.5  89/02/17  18:07:12  ghoti
 * ifdef/endif,etc. label fixing - courtesy of Ness
 * 
 * Revision 2.4  89/02/08  16:49:11  ghoti
 * change copyright notice
 * 
 * Revision 2.3  89/02/04  19:05:26  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 2.2  88/11/18  21:07:54  tom
 * Change null line-with to 255 indicator.
 * 
 * Revision 2.1  88/09/27  18:11:41  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:28:01  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:43:33  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Zip Data-object -- Stream Output Parsing

MODULE	zipds02.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This is the suite of Methods that support the Stream Writing facilities
	of the Zip Data-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  03/31/88	Created (TCP)
  11/17/88	Handle line_width attribute (TCP/SG)

END-SPECIFICATION  ************************************************************/

#include "class.h"
#include "dataobj.ih"
#include "zipobj.ih"
#include "zip.ih"
#include <ctype.h>


#define	 Data			      (self)
#define	 Objects(i)		    ((*self->objects)[i])


long
zip__Write_Figure( self, figure )
  register struct zip		     *self;
  register struct zip_figure	     *figure;
  {
  register int			     i, status = zip_ok;
  register FILE			     *file =
	figure->zip_figure_image->zip_image_stream->zip_stream_file;
  static char			     horizontal[2] = "?",
				     vertical[2]   = "?",
				     halo[2]       = "?";
  register int			     pseudo_x=0, pseudo_y=0;

  IN(zip__Write_Figure);
  if ( ! figure->zip_figure_state.zip_figure_state_deleted )
    {
/*===    pseudo_x = figure->zip_figure_image->zip_image_stream->zip_stream_pseudo_x_offset;
    pseudo_y = figure->zip_figure_image->zip_image_stream->zip_stream_pseudo_y_offset;===*/
    fprintf( file, "*%c;%d,%d\n", ('A' - 1) + figure->zip_figure_type,
	   figure->zip_figure_point.zip_point_x - pseudo_x,
	   figure->zip_figure_point.zip_point_y - pseudo_y );
    if ( figure->zip_figure_name )
      fprintf( file, "N%s\n", figure->zip_figure_name );
    if ( figure->zip_figure_font )
	fprintf( file, "F%s\n",
		 Fonts->zip_fonts_vector[figure->zip_figure_font].
		    zip_fonts_table_name );  
    if ( figure->zip_figure_datum.zip_figure_text )
      fprintf( file, "T%s\n", figure->zip_figure_datum.zip_figure_text );
    if ( figure->zip_figure_style )
      fprintf( file, "S%d\n", figure->zip_figure_style );
    if ( figure->zip_figure_line_width != 255 )
      fprintf( file, "LW%d\n", figure->zip_figure_line_width );
    if ( figure->zip_figure_line_style )
      fprintf( file, "LS%d\n", figure->zip_figure_line_style );
    if ( figure->zip_figure_zoom_level )
      fprintf( file, "Z%d\n", figure->zip_figure_zoom_level );
    if ( figure->zip_figure_detail_level )
      fprintf( file, "D%d\n", figure->zip_figure_detail_level );
    if ( figure->zip_figure_mode.zip_figure_mode_patterned  &&
	 figure->zip_figure_fill.zip_figure_pattern  )
      fprintf( file, "P%c\n", figure->zip_figure_fill.zip_figure_pattern );
    if ( figure->zip_figure_mode.zip_figure_mode_shaded  &&
	 figure->zip_figure_fill.zip_figure_shade )
      fprintf( file, "G%d\n", figure->zip_figure_fill.zip_figure_shade );
    if ((figure->zip_figure_mode.zip_figure_mode_left    ||
	 figure->zip_figure_mode.zip_figure_mode_center  ||
	 figure->zip_figure_mode.zip_figure_mode_right   ||
	 figure->zip_figure_mode.zip_figure_mode_top     ||
	 figure->zip_figure_mode.zip_figure_mode_middle  ||
	 figure->zip_figure_mode.zip_figure_mode_bottom  ||
	 figure->zip_figure_mode.zip_figure_mode_halo)  )
      {
      if ( figure->zip_figure_mode.zip_figure_mode_left )   *horizontal = 'L';
      if ( figure->zip_figure_mode.zip_figure_mode_center ) *horizontal = 'C';
      if ( figure->zip_figure_mode.zip_figure_mode_right )  *horizontal = 'R';
      if ( figure->zip_figure_mode.zip_figure_mode_top )    *vertical = 'T';
      if ( figure->zip_figure_mode.zip_figure_mode_middle ) *vertical = 'M';
      if ( figure->zip_figure_mode.zip_figure_mode_bottom ) *vertical = 'B';
      if ( figure->zip_figure_mode.zip_figure_mode_halo )   *halo = 'H';
      fprintf( file, "M%s%s\n", horizontal, vertical, halo );
      }
    if ( figure->zip_figure_points )
      {
/*=== Begin HACK of 5/8/87 ===!/
      if ( figure->zip_figure_type == zip_circle_figure  ||
	   figure->zip_figure_type == zip_ellipse_figure  ||
	   figure->zip_figure_type == zip_arc_figure  )
	fprintf( file, ">%d,%d",
		(figure->zip_figure_points->zip_points[0].zip_point_x),
		(figure->zip_figure_points->zip_points[0].zip_point_y) );
	else===*/
	fprintf( file, ">%d,%d",
		figure->zip_figure_points->zip_points[0].zip_point_x - pseudo_x,
		figure->zip_figure_points->zip_points[0].zip_point_y - pseudo_y );
/*=== End HACK of 5/8/87 ===*/
      for ( i = 1; i < figure->zip_figure_points->zip_points_count; i++ )
        fprintf( file, ";%d,%d",
		 figure->zip_figure_points->zip_points[i].zip_point_x - pseudo_x,
		 figure->zip_figure_points->zip_points[i].zip_point_y - pseudo_y );
      fprintf( file, "\n" );
      }
    }
  IN(zip__Write_Figure);
  return status;
  }

long
zip_Enparse_Stream( self, stream )
  register struct zip		     *self;
  register struct zip_stream	     *stream;
  {
  register int			      status = zip_ok;
  register zip_type_image	      image = stream->zip_stream_image_anchor;
  register zip_type_figure	      figure = image->zip_image_figure_anchor;
  
  IN(zip_Enparse_Stream);
  if ( ! (image->zip_image_state.zip_image_state_deleted  ||
	  image->zip_image_state.zip_image_state_transient) )
    status = Write_Image_Beginning( self, image );
  while ( figure  &&  status == zip_ok )
    {
    status = zipobject_Write_Object( Objects(figure->zip_figure_type), figure );
    figure = figure->zip_figure_next;
    }
  if ( image->zip_image_inferior  &&  status == zip_ok )
    status = Write_Inferior_Image( self, image->zip_image_inferior );
  if ( status == zip_ok  &&
       ! (image->zip_image_state.zip_image_state_deleted  ||
	  image->zip_image_state.zip_image_state_transient) )
    status = Write_Image_Ending( self, image );
  stream->zip_stream_states.zip_stream_state_modified = false;
  OUT(zip_Enparse_Stream);
  return status;
  }

static int
Write_Inferior_Image( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register int			      status = zip_ok;
  register zip_type_figure	      figure = image->zip_image_figure_anchor;
  
  IN(Write_Inferior_Image);
  if ( ! image->zip_image_state.zip_image_state_deleted )
    status = Write_Image_Beginning( self, image );
  while ( figure  &&  status == zip_ok )
    {
    status = zipobject_Write_Object( Objects(figure->zip_figure_type), figure );
    figure = figure->zip_figure_next;
    }
  if ( image->zip_image_inferior  &&  status == zip_ok )
    status = Write_Inferior_Image( self, image->zip_image_inferior );
  if ( status == zip_ok  &&  ! image->zip_image_state.zip_image_state_deleted )
    status = Write_Image_Ending( self, image );
  if ( image->zip_image_peer  &&  status == zip_ok )
    status = Write_Inferior_Image( self, image->zip_image_peer );
  OUT(Write_Inferior_Image);
  return status;
  }

static int
Write_Image_Beginning( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register FILE			     *file =
	image->zip_image_stream->zip_stream_file;
  register int			      status = zip_ok, new_line = 0;

  IN(Write_Image_Beginning);
  fprintf( file, "{" );
  if ( status == zip_ok  &&  image->zip_image_name )
    {
    new_line++;
    fprintf( file, "%s\n", image->zip_image_name );
    }
  if ( status == zip_ok  &&  image->zip_image_font )
    fprintf( file, "F%s\n",
		 Fonts->zip_fonts_vector[image->zip_image_font].
		    zip_fonts_table_name );  
  if ( status == zip_ok  &&  image->zip_image_text )
    {
    fprintf( file, "T%s\n", image->zip_image_text );
    }
  if ( status == zip_ok  &&  image->zip_image_zoom_level != 0 )
    {
    fprintf( file, "Z%d\n", image->zip_image_zoom_level );
    }
  if ( status == zip_ok  &&  image->zip_image_mode.zip_image_mode_patterned  &&
       image->zip_image_fill.zip_image_pattern )
    {
    fprintf( file, "P%c\n", image->zip_image_fill.zip_image_pattern );
    }
  if ( status == zip_ok  &&  image->zip_image_mode.zip_image_mode_shaded  &&
       image->zip_image_fill.zip_image_shade )
    {
    fprintf( file, "G%d\n", image->zip_image_fill.zip_image_shade );
    }
  if ( status == zip_ok  &&  image->zip_image_line_width != 255 )
    {
    fprintf( file, "LW%d\n", image->zip_image_line_width );
    }
  if ( status == zip_ok  &&  new_line == 0 )
    {
    fprintf( file, "\n" );
    }
  OUT(Write_Image_Beginning);
  return status;
  }

static int
Write_Image_Ending( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register FILE			     *file =
	image->zip_image_stream->zip_stream_file;
  register int			      status = zip_ok, new_line = 0;

  IN(Write_Image_Ending);
  fprintf( file, "}" );
  if ( status == zip_ok  &&  image->zip_image_name )
    {
    new_line++;
    fprintf( file, "%s\n", image->zip_image_name );
    }
  if ( status == zip_ok  &&  new_line == 0 )
    {
    fprintf( file, "\n" );
    }
  OUT(Write_Image_Ending);
  return status;
  }
