/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* zipefi00.h	Zip Subroutine Library Image Objects Header		      */
/* Author	TC Peters						      */
/* Information Technology Center		   Carnegie-Mellon University */

/*
    $Log:	zipimage.h,v $
 * Revision 2.3  89/02/08  16:49:40  ghoti
 * change copyright notice
 * 
 * Revision 2.2  88/11/18  21:15:38  tom
 * Add line_width field.
 * 
 * Revision 2.1  88/09/27  18:13:21  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:32:17  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:44:11  tom
 * Initial revision
 * 
 * Revision 1.1  87/10/28  21:38:14  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Zip Subroutine Library Image Objects Header

MODULE	zipefi00.h

NOTICE	IBM Internal Use Only

DESCRIPTION
	This file is to be included in the compilation of both client-programs
	and the Zip Subroutine Library modules. It defines the Image structure
	and symbolics relatedf to that object.

HISTORY
  05/01/85	Created (TCP)
  12/10/85	Dropped Image Chain-units (TCP)
  01/04/86	Dropped Image left_peer and image_next -- use nesting-cells (TCP)
  05/15/86	Added zip_Containing_... casting objects (RM LeVine)
  07/15/86	Changed the zip_image_font so that it's short and
		 not a pointer to the font structure (RML)
  07/15/86	Added "hidden" state for images (TCP)
  08/12/86	Add casting for zip_Next_Image, et al (RML)
  08/15/86	Add zip_image_visibility (TCP)
  09/05/86	Add deleted state (TCP)
  09/22/86	Add 'transient' state (TCP)
  01/27/87	Change types to pixel/point appropriately (TCP)
		Remove figure_tail
  01/30/87	Add Unhooked state (TCP)
  07/16/87	Add zip_Image_Root (TCP)
  07/21/87	Add "detail" and "style" to Image-object (TCP)
  03/31/88	Revised for ATK (TCP)
  11/17/88	Add line_width field (TCP/SG)

END-SPECIFICATION  ************************************************************/

#define  zip_image_exposed			  1
#define  zip_image_hidden			  2

typedef  struct zip_image_attributes		  zip_type_image_attributes;

struct zip_image_attributes
  {
  unsigned int					  zip_image_attribute_group		:1;
  unsigned int					  zip_image_attribute_Y			:1;
  unsigned int					  zip_image_attribute_Z			:1;
  };


typedef  struct zip_image_state			  zip_type_image_state;

struct zip_image_state
  {
  unsigned int					  zip_image_state_points_exposed	:1;
  unsigned int					  zip_image_state_points_highlighted	:1;
  unsigned int					  zip_image_state_deleted		:1;
  unsigned int					  zip_image_state_transient		:1;
  unsigned int					  zip_image_state_unhooked		:1;
  };

typedef  struct zip_image_mode			  zip_type_image_mode;
struct zip_image_mode
  {
  unsigned int					  zip_image_mode_shaded			:1;
  unsigned int					  zip_image_mode_patterned		:1;
  };


typedef  struct zip_image			 *zip_type_image;

struct zip_image
  {
  char						 *zip_image_name;
  struct zip_stream				 *zip_image_stream;
  zip_type_point				  zip_image_least_x;
  zip_type_point				  zip_image_greatest_y;
  zip_type_point				  zip_image_least_y;
  zip_type_point				  zip_image_greatest_x;
  char						 *zip_image_text;
  short						  zip_image_font;
  zip_type_image_attributes			  zip_image_attributes;
  zip_type_image_state				  zip_image_state;
  unsigned char	    				  zip_image_visibility;
  zip_type_image_mode				  zip_image_mode;
  unsigned char					  zip_image_style;
  unsigned char					  zip_image_type;
  unsigned char					  zip_image_zoom_level;
  unsigned char					  zip_image_detail_level;
  unsigned char					  zip_image_line_width;
  unsigned char					  zip_image_color;
  union
    {
    unsigned char				  zip_image_pattern;
    unsigned char				  zip_image_shade;
    }			zip_image_fill;
  zip_type_image				  zip_image_superior;
  zip_type_image				  zip_image_inferior;
  zip_type_image				  zip_image_peer;
  zip_type_figure				  zip_image_figure_anchor;
  char						 *zip_image_client_data;
  };
