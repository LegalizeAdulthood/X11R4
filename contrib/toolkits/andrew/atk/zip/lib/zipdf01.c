/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipdf01.c,v 2.7 89/05/10 19:12:43 sg08 Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipdf01.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/zip/lib/RCS/zipdf01.c,v 2.7 89/05/10 19:12:43 sg08 Exp $";
#endif /* lint */

/* zipedf01.c	Zip Data-object	-- Figures  Generics		      */
/* Author	TC Peters					      */
/* Information Technology Center	   Carnegie-Mellon University */


/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Zip Data-object -- Figures  Generics

MODULE	zipedf01.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This is the suite of Methods that support the Figure facilities
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
  11/17/88	Add Superior_Image_Line_Width (TCP/SCG)
  05/10/89	Improve behaviour of Superior_Image_Line_Width (SCG)

END-SPECIFICATION  ************************************************************/

#include <andrewos.h>
#include "class.h"
#include "dataobj.ih"
#include "zip.ih"
#include <sys/stat.h>
#include <sys/dir.h>
#include <ctype.h>

#define	 Data			      self


char
zip__Superior_Image_Pattern( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register char			      pattern = NULL;

  while ( image )
    {
    if ( image->zip_image_fill.zip_image_pattern )
      {
      pattern = image->zip_image_fill.zip_image_pattern;
      break;
      }
    image = image->zip_image_superior;
    }
  return pattern;
  }

char
zip__Superior_Image_Shade( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register char			      shade = NULL;

  while ( image )
    {
    if ( image->zip_image_fill.zip_image_shade )
      {
      shade = image->zip_image_fill.zip_image_shade;
      break;
      }
    image = image->zip_image_superior;
    }
  return shade;
  }

unsigned char
zip__Superior_Image_Line_Width( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register unsigned char	      width = 255;

  while ( image )
    {
    if ( image->zip_image_line_width != 255 )
      {
      width = image->zip_image_line_width;
      break;
      }
    image = image->zip_image_superior;
    }
  return width;
  }

char *
zip__Superior_Image_Text( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register char			     *text = NULL;

  while ( image )
    {
    if ( image->zip_image_text )
      {
      text = image->zip_image_text;
      break;
      }
    image = image->zip_image_superior;
    }
  return text;
  }


struct fontdesc *
zip__Superior_Image_Font( self, image )
  register struct zip		     *self;
  register zip_type_image	      image;
  {
  register struct fontdesc 	     *font = NULL;

  while ( image )
    {
    if ( image->zip_image_font )
      {
      font = Fonts->
		zip_fonts_vector[image->zip_image_font].zip_fonts_table_font;
      break;
      }
    image = image->zip_image_superior;
    }
  return font;
  }
