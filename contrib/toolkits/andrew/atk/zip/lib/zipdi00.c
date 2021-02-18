/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipdi00.c,v 2.6 89/11/02 11:18:02 sg08 Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipdi00.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipdi00.c,v 2.6 89/11/02 11:18:02 sg08 Exp $";
#endif /* lint */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/* zipedi.c	Zip Data-object	-- Images			      */
/* Author	TC Peters					      */
/* Information Technology Center	   Carnegie-Mellon University */


/*
    $Log:	zipdi00.c,v $
 * Revision 2.6  89/11/02  11:18:02  sg08
 * Properly turn on mode fields on calls to Set_Image_Shade and Set_Image_Pattern.
 * 
 * Revision 2.5  89/02/17  18:06:45  ghoti
 * ifdef/endif,etc. label fixing - courtesy of Ness
 * 
 * Revision 2.4  89/02/08  16:48:54  ghoti
 * change copyright notice
 * 
 * Revision 2.3  89/02/04  18:52:47  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 2.2  88/11/18  21:04:50  tom
 * Add Set_Image_Line_Width.
 * 
 * Revision 2.1  88/09/27  18:10:50  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:26:26  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:43:16  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Zip Data-object -- Imagess

MODULE	zipedi.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This is the suite of Methods that support the Imagee facilities
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
  11/17/88	Add Set_Image_Line_Width (TCP/SCG)
    11/2/89	Turn on modes in Set_Image_{Shade,Pattern} (SCG)

END-SPECIFICATION  ************************************************************/

#include "class.h"
#include "dataobj.ih"
#include "zip.ih"

#define	 Data			      self
#define  SetStreamModified	      {image->zip_image_stream->\
					zip_stream_states.zip_stream_state_modified = 1;}


static
Allocate_Image( self, stream, image, name )
  register struct zip		     *self;
  register struct zip_stream	     *stream;
  register struct zip_image	    **image;
  register char			     *name;
  {
  register int			      status = zip_ok;

  IN(Allocate_Image);
  if ( (*image = (zip_type_image) calloc( 1, sizeof(struct zip_image) )) != NULL )
    {
    (*image)->zip_image_greatest_x = -999999999;
    (*image)->zip_image_least_x    =  999999999;
    (*image)->zip_image_greatest_y = -999999999;
    (*image)->zip_image_least_y    =  999999999;
    (*image)->zip_image_stream = stream;
    (*image)->zip_image_stream->zip_stream_states.zip_stream_state_modified = 1;
    (*image)->zip_image_line_width = 255;
    if ( name  &&  *name )
      status = zip_Set_Image_Name( self, *image, name );
    }
    else  status = zip_insufficient_image_space;
  OUT(Allocate_Image);
  return  status;
  }

long
zip__Create_Peer_Image( self, image, name, stream, peer )
  register struct zip		     *self;
  register struct zip_image	    **image;
  register char			     *name;
  register struct zip_stream	     *stream;
  register struct zip_image	     *peer;
  {
  register long			      status = zip_success;
  register zip_type_image	      peer_ptr = peer;

  IN(zip_Create_Peer_Image);
  if ( stream )
    {
    if ( peer_ptr )
      {
      if ( (status = Allocate_Image( self, stream, image, name )) == zip_success )
        {
        if ( status == zip_success )
	  {  
          (*image)->zip_image_peer = peer_ptr->zip_image_peer;
          peer_ptr->zip_image_peer = *image;
          (*image)->zip_image_superior = peer_ptr->zip_image_superior;
	  }
        }
      }
      else  status = zip_Create_Inferior_Image( self, image, name, stream, NULL );
    }
    else  status = zip_stream_non_existent;
  OUT(zip_Create_Peer_Image);
  return status;
  }

long
zip__Create_Inferior_Image( self, image, name, stream, superior )
  register struct zip		     *self;
  register struct zip_image	    **image;
  register char			     *name;
  register struct zip_stream	     *stream;
  register struct zip_image	     *superior;
  {
  register long			      status = zip_success;
  register zip_type_image	      superior_ptr = superior, image_ptr;

  IN(zip_Create_Inferior_Image);
  if ( stream )
    {
    if ( (status = Allocate_Image( self, stream, image, name )) == zip_success )
      {
      if ( superior_ptr )
        {
        if ( image_ptr = superior_ptr->zip_image_inferior )
	  {
	  while ( image_ptr->zip_image_peer )
            image_ptr = image_ptr->zip_image_peer;
	  image_ptr->zip_image_peer = *image;
	  }
	  else /* Superior does not yet have an Inferior */
	  {
	  superior_ptr->zip_image_inferior = *image;
	  }
        (*image)->zip_image_superior = superior_ptr;
        }
        else /* Superior is Null */
        {
        if ( image_ptr = stream->zip_stream_image_anchor )
	  {
  	  while ( image_ptr->zip_image_peer )
            image_ptr = image_ptr->zip_image_peer;
	  image_ptr->zip_image_peer = *image;
	  }
	  else /* Must be first Image added to Stream */
	  {
	  stream->zip_stream_image_anchor = *image;
	  }
	}
      }
    }
    else  status = zip_stream_non_existent;
  OUT(zip_Create_Inferior_Image);
  return status;
  }

long
zip__Destroy_Image( self, image )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register int			      status = zip_success;
  register zip_type_figure	      figure, figure_ptr;

  IN(zip__Destroy_Image);
  if ( image )
    {
    SetStreamModified;
    figure = image->zip_image_figure_anchor;
    while ( figure  &&  status == zip_success )
      {
      figure_ptr = figure->zip_figure_next;
      status = zip_Destroy_Figure( self, figure );
      figure = figure_ptr;
      }
    if ( image->zip_image_inferior  &&  status == zip_success )
      status = Destroy_Inferior_Image( self, image->zip_image_inferior );
    if (! image->zip_image_state.zip_image_state_unhooked )
      zip_Unhook_Image( self, image );
    Free_Image( self, image );
    }
    else  status = zip_image_non_existent;
  OUT(zip__Destroy_Image);
  return status;
  }

static int
Destroy_Inferior_Image( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register int			      status = zip_success;
  register zip_type_figure	      figure_ptr, figure;

  IN(Destroy_Inferior_Image);
  figure = image->zip_image_figure_anchor;
  while ( figure  &&  status == zip_success )
    {
    figure_ptr = figure->zip_figure_next;
    status = zip_Destroy_Figure( self, figure );
    figure = figure_ptr;
    }
  if ( image->zip_image_inferior  &&  status == zip_success )
    status = Destroy_Inferior_Image( self, image->zip_image_inferior );
  if ( image->zip_image_peer  &&  status == zip_success )
    status = Destroy_Inferior_Image( self, image->zip_image_peer );
  if (! image->zip_image_state.zip_image_state_unhooked )
    zip_Unhook_Image( self, image );
  Free_Image( self, image );
  OUT(Destroy_Inferior_Image);
  return  status;
  }

static
Free_Image( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  IN(Free_Image);
  if ( image->zip_image_name )
    {
    DEBUGst(Image-name,image->zip_image_name);
    DEBUGst(Stream-name,zip_Containing_Image_Stream( self, image )->zip_stream_name);
    symtab_delete( ((zip_type_stream)
	zip_Containing_Image_Stream( self, image ))->zip_stream_symbol_table,
	   image->zip_image_name );
    free( image->zip_image_name );
    }
  if ( image->zip_image_text )
    free( image->zip_image_text );
  free( image );
  OUT(Free_Image);
  }

long /*=== HOOK s/b in EDITING module???  ====*/
zip__Hook_Peer_Image( self, image, peer_image )
  register struct zip		     *self;
  register struct zip_image	     *image, *peer_image;
  {
  register int			      status = zip_success;

  IN(zip_Hook_Peer_Image);
  if ( image  &&  peer_image )
    {
    if ( image->zip_image_state.zip_image_state_unhooked )
      {
      image->zip_image_peer = peer_image->zip_image_peer;
      peer_image->zip_image_peer = image;
      image->zip_image_superior = peer_image->zip_image_superior;
      image->zip_image_stream = peer_image->zip_image_stream;
      Propagate_Image_Stream_Ptr( image );

/*=== should stream's greatest/least cells be modified ??? ===*/

      zip_Set_Image_Name( self, image, zip_Image_Name( self, image ) );
      image->zip_image_state.zip_image_state_unhooked = false;
      SetStreamModified;
      }
      else
      status = zip_failure; /*=== s/b "Already Hooked" ===*/
    }
    else
    status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Hook_Peer_Image);
  return status;
  }

long
zip__Hook_Inferior_Image( self, image, superior_image )
  register struct zip		     *self;
  register struct zip_image	     *image, *superior_image;
  {
  register int			      status = zip_success;

  IN(zip_Hook_Inferior_Image);
  if ( image  &&  superior_image )
    {
    if ( image->zip_image_state.zip_image_state_unhooked )
      {
      image->zip_image_superior = superior_image;
      image->zip_image_peer = superior_image->zip_image_inferior;
      superior_image->zip_image_inferior = image;
      image->zip_image_stream = superior_image->zip_image_stream;
      Propagate_Image_Stream_Ptr( self, image );

/*=== should stream's greatest/least cells be modified ??? ===*/

      zip_Set_Image_Name( self, image, zip_Image_Name( self, image ) );
      image->zip_image_state.zip_image_state_unhooked = false;
      SetStreamModified;
      }
      else
      status = zip_failure; /*=== s/b "Already Hooked" ===*/
    }
    else
    status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Hook_Inferior_Image);
  return status;
  }

static
Propagate_Image_Stream_Ptr( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register zip_type_image	      image_ptr;
  register zip_type_image	      superior_image, peer_image;

  peer_image = image->zip_image_peer;
  image->zip_image_peer = NULL;
  superior_image = image->zip_image_superior;
  image->zip_image_superior = NULL;
  image_ptr = image;
  while ( image_ptr = zip_Next_Image( self, image_ptr ) )
    {
    image_ptr->zip_image_stream = image->zip_image_stream;
    }
  image->zip_image_peer = peer_image;
  image->zip_image_superior = superior_image;
  return zip_success;
  }

long
zip__Unhook_Image( self, image )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register struct zip_image	     *image_ptr;
  register int			      status = zip_success;

  IN(zip_Unhook_Image);
  if ( image )
    {
    if ( ! image->zip_image_state.zip_image_state_unhooked )
      {
      SetStreamModified;
      if ( image->zip_image_superior )
	{
	if ( image->zip_image_superior->zip_image_inferior == image )
	  {
          image->zip_image_superior->zip_image_inferior = image->zip_image_peer;
	  }
          else
	  {
	  image_ptr = image->zip_image_superior->zip_image_inferior;
	  while ( image_ptr->zip_image_peer != image )
	    image_ptr = image_ptr->zip_image_peer;
	  image_ptr->zip_image_peer = image->zip_image_peer;
	  }
	image->zip_image_superior = NULL;
	}
	else
	{
	image->zip_image_stream->zip_stream_image_anchor = NULL;
	}
      image->zip_image_peer = NULL;
/*===
      image->zip_image_stream = NULL;
      Propagate_Image_Stream_Ptr( self, image );
===*/
/*=== should stream's greatest/least cells be modified ??? ===*/

      image->zip_image_state.zip_image_state_unhooked = true;
      }
      else  status = zip_failure; /*=== s/b "Already Un-Hooked" ===*/
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Unhook_Image);
  return status;
  }

long
zip__Set_Image_Name( self, image, name )
  register struct zip		     *self;
  register struct zip_image	     *image;
  register char			     *name;
  {
  register int			      status = zip_success;
  zip_type_image		      duplicate;

  IN(zip_Set_Image_Name);
  if ( image )
    {
    DEBUGst( Given Name,name);
    DEBUGst( Image Name,image->zip_image_name);
    SetStreamModified;
/*=== 8/5/87
    if ( image->zip_image_name )
      *image->zip_image_name = '\0';
===*/
    if ( name  &&  *name != '\0' )
      {
      if ( symtab_find( image->zip_image_stream->zip_stream_symbol_table,
		      name, &duplicate ) == 0
/*===something wrong in symtab===*/ && strcmp(name,duplicate->zip_image_name)==0  )
        {
        status = zip_duplicate_image_name;
/*===
printf( "DUP IMAGE NAME '%s' (Stream '%s')\n",name,
image->zip_image_stream->zip_stream_name);
===*/
        }
        else
        {
        if ( (image->zip_image_name = (char *)malloc( strlen( name ) + 1 )) == NULL )
          status = zip_insufficient_image_space;
          else
          {
          strcpy( image->zip_image_name, name );
          symtab_add( image->zip_image_stream->zip_stream_symbol_table,
  		    image->zip_image_name, image );
          }
        }
      }
      else
      {
      if ( image->zip_image_name )
        free( image->zip_image_name );
      image->zip_image_name = NULL;
      }
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Set_Image_Name);
  return status;
  }

long
zip__Set_Image_Text( self, image, text )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register int			      status = zip_success;

  IN(zip_Set_Image_Text);
  if ( image )
    {
    DEBUGst( Given Text,text);
    DEBUGst( Image Name,image->zip_image_name);
    SetStreamModified;
    if ( text )
      if ( (image->zip_image_text = (char *) malloc( strlen( text ) + 1 )) != NULL )
        strcpy( image->zip_image_text, text );
        else
        status = zip_insufficient_image_space;
      else image->zip_image_text = NULL;
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Set_Image_Text);
  return status;
  }

long
zip__Set_Image_Pattern( self, image, pattern )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register int			      status = zip_success;

  IN(zip_Set_Image_Pattern);
  if ( image )
    {
    DEBUGct( Given Pattern,pattern);
    DEBUGst( Image Name,image->zip_image_name);
    SetStreamModified;
    image->zip_image_fill.zip_image_pattern = pattern;
    image->zip_image_mode.zip_image_mode_patterned = on;
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Set_Image_Pattern);
  return status;
  }

long
zip__Set_Image_Shade( self, image, shade )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register int			      status = zip_success;

  IN(zip_Set_Image_Shade);
  if ( image )
    {
    DEBUGct( Given Shade,shade);
    DEBUGst( Image Name,image->zip_image_name);
    SetStreamModified;
    image->zip_image_fill.zip_image_shade = shade;
    image->zip_image_mode.zip_image_mode_shaded = on;
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Set_Image_Shade);
  return status;
  }

long
zip__Set_Image_Font( self, image, font_name )
  register struct zip		     *self;
  register struct zip_image	     *image;
  register char			     *font_name;
  {
  register int			      status = zip_success;

  IN(zip_Set_Image_Font);
  if ( image )
    {
    DEBUGst( Given Font,font_name);
    DEBUGst( Image Name,image->zip_image_name);
    SetStreamModified;
    if ( font_name  &&  *font_name )
      zip_Define_Font( self, font_name, &(image->zip_image_font) );
      else image->zip_image_font = NULL;
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Set_Image_Font);
  return status;
  }

long
zip__Set_Image_Line_Width( self, image, width )
  register struct zip		     *self;
  register struct zip_image	     *image;
  register long			      width;
  {
  register int			      status = zip_success;

  IN(zip_Set_Image_Line_Width);
  if ( image )
    {
    DEBUGct( Given Width,width);
    DEBUGst( Image Name,image->zip_image_name);
    SetStreamModified;
    image->zip_image_line_width = width;
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip_Set_Image_Line_Width);
  return status;
  }

long
zip__Adjust_Image_Point_Suite( self, image, x_delta, y_delta )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register int			      status = zip_success;
  register zip_type_figure	      figure_ptr;

  IN(zip__Adjust_Image_Point_Suite);
  if ( image )
    {
    SetStreamModified;
    figure_ptr = image->zip_image_figure_anchor;
    while ( figure_ptr  &&  status == zip_success )
      {
      status = zip_Adjust_Figure_Point_Suite( self, figure_ptr, x_delta, y_delta );
      figure_ptr = figure_ptr->zip_figure_next;
     }
    if ( image->zip_image_inferior  &&  status == zip_success )
      status = Adjust_Inferior_Image_Point_Suite( self, image->zip_image_inferior,
						  x_delta, y_delta );
    }
    else  status = zip_image_non_existent;
  ZIP_STATUS();
  OUT(zip__Adjust_Image_Point_Suite);
  return status;
  }

static int
Adjust_Inferior_Image_Point_Suite( self, image, x_delta, y_delta )
  register struct zip		     *self;
  register zip_type_image	      image;
  register int			      x_delta, y_delta;
  {
  register int			      status = zip_success;
  register zip_type_figure	      figure_ptr;

  IN(Adjust_Inferior_Image_Point_Suite);
  figure_ptr = image->zip_image_figure_anchor;
  while ( figure_ptr  &&  status == zip_success )
    {
    status = zip_Adjust_Figure_Point_Suite( self, figure_ptr, x_delta, y_delta );
    figure_ptr = figure_ptr->zip_figure_next;
    }
  if ( image->zip_image_inferior  &&  status == zip_success )
    status = Adjust_Inferior_Image_Point_Suite( self, image->zip_image_inferior,
						      x_delta, y_delta );
  if ( image->zip_image_peer  &&  status == zip_success )
    status = Adjust_Inferior_Image_Point_Suite( self, image->zip_image_peer,
						      x_delta, y_delta );
  OUT(Adjust_Inferior_Image_Point_Suite);
  return status;
  }

struct zip_image *
zip__Image( self, name )
  register struct zip		     *self;
  register char			     *name;
  {
  zip_type_image		      image = NULL;
  register zip_type_stream_chain      stream_link =
					StreamAnchor;
  register int			      status = zip_success;

  IN(zip__Image);
  while ( stream_link )
    {
    if ( stream_link->zip_stream_chain_ptr->zip_stream_symbol_table )
       if ( symtab_find( stream_link->zip_stream_chain_ptr->zip_stream_symbol_table,
	     name, &image ) == 0 )
	  break;
    stream_link = stream_link->zip_stream_chain_next;
    }
  ZIP_STATUS();
  OUT(zip__Image);
  return  image;
  }

struct zip_image *
zip__Stream_Image( self, stream, name )
  register struct zip		     *self;
  register struct zip_stream	     *stream;
  register char			     *name;
  {
  zip_type_image		      image = NULL;
  register int			      status = zip_success;

  IN(zip__Stream_Image);
  if ( stream  &&  name )
    if ( stream->zip_stream_symbol_table )
      symtab_find( stream->zip_stream_symbol_table, name, &image );
  ZIP_STATUS();
  OUT(zip__Stream_Image);
  return  image;
  }

struct zip_image *
zip__Next_Image( self, image )
  register struct zip		     *self;
  register struct zip_image	     *image;
  {
  register zip_type_image	      next_image = NULL;

  IN(zip__Next_Image);
  if ( !( next_image = image->zip_image_inferior ) )
     if ( !( next_image = image->zip_image_peer ) )
        {
        next_image = image->zip_image_superior;
        while ( next_image )
          if ( next_image->zip_image_peer )
            {
            next_image = next_image->zip_image_peer;
	    break;
	    }
            else
	    next_image = next_image->zip_image_superior;
        }
  OUT(zip__Next_Image);  
  return  next_image;
  }

 zip_type_image 
zip__Image_Left_Peer( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register zip_type_image	      peer = NULL, candidate;

  IN(zip__Image_Left_Peer);
  if ( image  &&  image->zip_image_superior )
    {
    candidate = image->zip_image_superior->zip_image_inferior;
    while( candidate != image )
      {
      peer = candidate;
      candidate = candidate->zip_image_peer;
      }
    }
  OUT(zip__Image_Left_Peer);
  return  peer;
  }

int
zip__Set_Image_Extrema( self, image, x, y )
  register struct zip		     *self;
  register zip_type_image	      image;
  register zip_type_point	      x, y;
  {
  IN(zip__Set_Image_Extrema);
  if ( x < image->zip_image_least_x )
    {
    image->zip_image_least_x = x;
    DEBUGdt(Least    X,x );
    }
  if ( x > image->zip_image_greatest_x )
    { 
    image->zip_image_greatest_x = x;
    DEBUGdt(Greatest X,x );
    }
  if ( y < image->zip_image_least_y )
    {
    image->zip_image_least_y = y;
    DEBUGdt(Least    Y,y );
    }
  if ( y > image->zip_image_greatest_y )
    {
    image->zip_image_greatest_y = y;
    DEBUGdt(Greatest Y,y );
    }
  if ( image->zip_image_superior )
    zip_Set_Image_Extrema( self, image->zip_image_superior, x, y );
  OUT(zip__Set_Image_Extrema);
  return zip_success;
  }

/*===
static int
ZIP_Balance_Image_Extrema( self, image )
  register struct zip		     *self;
  register zip_type_image		  image;
  {
  IN(ZIP_Balance_Image_Extrema);
  if ( abs(image->zip_image_least_x) < abs(image->zip_image_greatest_x) )
      image->zip_image_least_x    = -abs(image->zip_image_greatest_x);
      else
      image->zip_image_greatest_x =  abs(image->zip_image_least_x);
  if ( abs(image->zip_image_least_y) < abs(image->zip_image_greatest_y) )
      image->zip_image_least_y    = -abs(image->zip_image_greatest_y);
      else
      image->zip_image_greatest_y =  abs(image->zip_image_least_y);
  OUT(ZIP_Balance_Image_Extrema);
  return zip_success;
  }
===*/
