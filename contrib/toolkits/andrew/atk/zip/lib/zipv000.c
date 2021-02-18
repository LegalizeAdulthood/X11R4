/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipv000.c,v 2.7 89/08/30 16:33:08 sg08 Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipv000.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipv000.c,v 2.7 89/08/30 16:33:08 sg08 Exp $";
#endif /* lint */

/* zipv000.c	Zip View-object	-- general			      */
/* Author	TC Peters					      */
/* Information Technology Center	   Carnegie-Mellon University */


/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Zip View-object -- general	

MODULE	zipv000.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This is the suite of Methods that support general facilities
	of the Zip View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  03/31/88	Created (TCP)
  05/15/89	Improve performance by pre-checking TransferMode (TCP)
		Also avoid unnecessary SetClippingRect usage
   08/30/89	Adjust zipview_Condition to use graphic_SOURCE and ~graphic_SOURCE instread of graphic_BLACK and graphic_WHITE (SCG)

END-SPECIFICATION  ************************************************************/

#include "class.h"
#include "view.ih"
#include "fontdesc.ih"
#include "zipobj.ih"
#include "zip.ih"
#include "zipv.ih"

#define  Data			(self->data_object)
#define  View			(self)
#define  Objects(i)		((*self->objects)[i])
#define  PaneExceptionHandler	(self->pane_exception_handler)

struct graphic *
zipview__Define_Graphic( self, font, pattern )
  register struct zipview		 *self;
  register struct fontdesc		 *font;
  register unsigned char		  pattern;
  {
  register struct graphic		 *graphic = NULL;
/*===*/
#define max_table_size  50
register int			  i;
static struct table
  {
  struct fontdesc		 *font;
  unsigned char			  pattern;
  struct graphic		 *graphic;
  }				  table[max_table_size];

  IN(zipview__Define_Graphic);
  DEBUGct(Pattern,pattern);
  if ( font )
   for ( i = 0; i < max_table_size; i++ )
    {
    if ( font == table[i].font  &&  pattern == table[i].pattern )
      {
      DEBUGdt(Old Entry,i);
      graphic = table[i].graphic;
      break;
      }
    if ( table[i].font == NULL )
      {
      DEBUGdt(New Entry,i);
      table[i].font = font;
      table[i].pattern = pattern;
      table[i].graphic = graphic =
	fontdesc_CvtCharToGraphic( font, zipview_GetDrawable( self ), pattern );
      break;
      }
    }
/*===*/
  DEBUGdt(I,i);
  DEBUGxt(Graphic,graphic);
  OUT(zipview__Define_Graphic);
  return  graphic;
  }

struct fontdesc *
zipview__Contextual_Figure_Font( self, figure )
  register struct zipview		 *self;
  register zip_type_figure		  figure;
  {
  register struct fontdesc		 *font;

  IN(zipview__Contextual_Figure_Font);
  if ( figure->zip_figure_font )
    font = Fonts->zip_fonts_vector
	[figure->zip_figure_font].zip_fonts_table_font;
  else 
  if ( figure->zip_figure_image->zip_image_font )
    font = Fonts->zip_fonts_vector
	[figure->zip_figure_image->zip_image_font].zip_fonts_table_font;
  else
  if ( font = zip_Superior_Image_Font( Data, figure->zip_figure_image->zip_image_superior ) )
    {}
  else
  if ( figure->zip_figure_image->zip_image_stream->zip_stream_font )
    font = Fonts->zip_fonts_vector
	[figure->zip_figure_image->zip_image_stream->zip_stream_font].zip_fonts_table_font;
  else
    font = (struct fontdesc *)zip_Define_Font( Data, "andy12", NULL );
  DEBUGxt(Font,font);
  OUT(zipview__Contextual_Figure_Font);
  return  font;
  }

struct fontdesc *
zipview__Select_Contextual_Figure_Font( self, figure )
  register struct zipview		 *self;
  register zip_type_figure		  figure;
  {
  register struct fontdesc		 *font;

  IN(zipview__Select_Contextual_Figure_Font);
  if ( font = zipview_Contextual_Figure_Font( self, figure ) )
    zipview_SetFont( self, font );
  DEBUGxt(Font,font);
  OUT(zipview__Select_Contextual_Figure_Font);
  return  font;
  }

boolean
zipview__Condition( self, pane, figure, action )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  register zip_type_figure		  figure;
  register int				  action;
  {
  register boolean			  condition = true;

  IN(zipview__Condition);
  if (( pane && figure && figure->zip_figure_zoom_level <= pane->zip_pane_zoom_level )
       || ( pane && !figure ))
    {
    switch ( action )
      {
      case  zip_draw:
	DEBUG(DRAW);
        if ( pane->zip_pane_state.zip_pane_state_inverted )
	  {
	  if ( zipview_GetTransferMode( self ) != ~graphic_SOURCE )
            zipview_SetTransferMode( self, ~graphic_SOURCE );
	  }
	else
        if ( pane->zip_pane_state.zip_pane_state_paint_inverted )
	  {
	  if ( zipview_GetTransferMode( self ) != graphic_INVERT )
	    zipview_SetTransferMode( self, graphic_INVERT );
	  }
	else
        if ( pane->zip_pane_state.zip_pane_state_paint_copy )
	  {
	  if ( zipview_GetTransferMode( self ) != graphic_COPY )
	    zipview_SetTransferMode( self, graphic_COPY );
	  }
	else
	  {
  	  if ( zipview_GetTransferMode( self ) != graphic_SOURCE )
	    zipview_SetTransferMode( self, graphic_SOURCE );
	  }
	break;
      case  zip_clear:
	DEBUG(CLEAR);
        if ( ! pane->zip_pane_state.zip_pane_state_inverted )
	  {
	  if ( zipview_GetTransferMode( self ) != graphic_WHITE )
            zipview_SetTransferMode( self, graphic_WHITE );
	  }
	else
	  {
	  if ( zipview_GetTransferMode( self ) != graphic_BLACK )
	    zipview_SetTransferMode( self, graphic_BLACK );
	  }
	break;
      }
    }
    else  condition = false;
  OUT(zipview__Condition);
  return condition;
  }

zipview__Set_Clip_Area( self, pane, l, t, w, h )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  register long				  l, t, w, h;
  {
  struct rectangle			  rectangle;

  IN(zipview__Set_Clip_Area);
  rectangle.left   = l;
  rectangle.top    = t;
  rectangle.width  = w;
  rectangle.height = h;
  zipview_SetClippingRect( self, &rectangle );
  OUT(zipview__Set_Clip_Area);
  }

zipview__Set_Pane_Clip_Area( self, pane )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  {
  struct rectangle			  rect, clip;

  IN(zipview__Set_Pane_Clip_Area);
  rect.left   = pane->zip_pane_left + pane->zip_pane_border_thickness;
  rect.top    = pane->zip_pane_top  + pane->zip_pane_border_thickness;
  rect.width  = zipview_Pane_Width( self, pane ) -
			 (2 * pane->zip_pane_border_thickness);
  rect.height = zipview_Pane_Height(self, pane ) -
			 (2 * pane->zip_pane_border_thickness);
  zipview_GetClippingRect( self, &clip );
  if ( rect.left  != clip.left   ||  rect.top    != clip.top  ||
       rect.width != clip.width  ||  rect.height != clip.height )
    {
    zipview_SetClippingRect( self, &rect );
    }
  OUT(zipview__Set_Pane_Clip_Area);
  }

zipview__Reset_Pane_Clip_Area( self, pane )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  {
  IN(zipview__Reset_Pane_Clip_Area);
  zipview_ClearClippingRect( self );
  OUT(zipview__Reset_Pane_Clip_Area);
  }

zipview_Mark_Pane_Exposed( self, pane )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  {
  IN(zipview_Mark_Pane_Exposed);
  pane->zip_pane_state.zip_pane_state_exposed = true;
  pane->zip_pane_state.zip_pane_state_hidden = false;
  pane->zip_pane_state.zip_pane_state_removed = false;
  OUT(zipview_Mark_Pane_Exposed);
  }

zipview_Mark_Pane_Hidden( self, pane )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  {
  IN(zipview_Mark_Pane_Hidden);
  pane->zip_pane_state.zip_pane_state_exposed = false;
  pane->zip_pane_state.zip_pane_state_hidden = true;
  pane->zip_pane_state.zip_pane_state_removed = false;
  OUT(zipview_Mark_Pane_Hidden);
  }

boolean
zipview__Proximate_Figure_Point( self, pane, figure, X, Y, x, y )
  register struct zipview		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		  X, Y, x, y;
  {
  register long				  status = false;

  IN(zipview__Proximate_Figure_Point);
  if ( X >= (x - zip_proximal_distance)  &&
       X <= (x + zip_proximal_distance)  &&
       Y >= (y - zip_proximal_distance)  &&
       Y <= (y + zip_proximal_distance)  )
    status = true;
  OUT(zipview__Proximate_Figure_Point);
  return status;
  }

long
zipview__Proximate_Enclosure( self, pane, left, top, right, bottom, x, y )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  register zip_type_pixel		  left, top, right, bottom;
  register zip_type_pixel		  x, y;
  {
  register long				  status = false;

  IN(zipview__Proximate_Enclosure);
  if ( x >= left  &&  x <= right  &&
       y >= top   &&  y <= bottom )
    status = true;
  OUT(zipview__Proximate_Enclosure);
  return status;
  }

long
zipview__Try_Pane_Exception_Handler( self, pane )
  register struct zipview		 *self;
  register zip_type_pane		  pane;
  {
  IN(zipview__Try_Pane_Exception_Handler);
  if ( PaneExceptionHandler )
    {
    (*PaneExceptionHandler)( self, pane );
    OUT(zipview__Try_Pane_Exception_Handler);
    return true;
    }
    else
    {
    OUT(zipview__Try_Pane_Exception_Handler);
    return zip_Try_general_Exception_Handler( Data );
    }
  }
