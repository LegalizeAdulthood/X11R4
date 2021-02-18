/*--------------------------------------------------------------------------*\
|   
|  Name:  Command Widget
|
|  Description:  
|     This file contains the slang code which defines the UI for
|     command widget visualization.
|
|  Component of:
|     Dialogue editor dialogue.
|
|  Related files:
|     cw.slang - contains the source for the command widget.
|
|  Project:
|     Serpent
|     User Interface Project
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     April 7, 1989 : Module created.
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/
#include "page.sl"
#include "idatt.sl"
#include "table.sl"
#include "batt.sl"
#include "datt.sl"
#include "inatt.sl"

/* Attribute value constants. */

#define DISPLAY_WIDTH 1152
#define DISPLAY_HEIGHT 900
#define ATTR_HEIGHT 25 + 10

/*--------------------------------------------------------------------------*\
|  Name: cw_vc
|
|  Description:
|    The cw_vc instantiates on a new object_instance_sdd in 
|    the shared data base.  Depending on how local variables are set
\*--------------------------------------------------------------------------*/
VC: cw_vc

  Creation Condition:  (new("object_instance_sdd") and 
    (object_instance_sdd.type = "command_widget"))

  Variables:
    display_attributes: false;
    display_instance: object_instance_sdd.visible;
    current_id;
    parent_id: NULL;
/*
    The variables being set represent those variables which can be
    modified through the command widget attribute form.  Modifications 
    are automatically displayed in the visualization window.
*/

    define_attr(parent,cw)
    define_attr(x,cw)
    define_attr(y,cw)
    define_attr(width,cw)
    define_attr(height,cw)
    define_attr(border_color,cw)
    define_attr(border_width,cw)
    define_attr(background_color,cw)
    define_attr(sensitive,cw)
    define_attr(mapped_when_managed,cw)
    define_attr(foreground_color,cw)
    define_attr(label_text,cw)
    define_attr(justify,cw)
    define_attr(internal_width,cw)
    define_attr(internal_height,cw)

  On Create: {

    current_id := object_instance_sdd.attr_list;
    initialize_attr(parent,cw,current_id)
    initialize_attr(x,cw,current_id)
    initialize_attr(y,cw,current_id)
    initialize_attr(width,cw,current_id)
    initialize_attr(height,cw,current_id)
    initialize_attr(border_width,cw,current_id)
    initialize_attr(border_color,cw,current_id)
    initialize_attr(background_color,cw,current_id)
    initialize_attr(sensitive,cw,current_id)
    initialize_attr(mapped_when_managed,cw,current_id)
    initialize_attr(foreground_color,cw,current_id)
    initialize_attr(label_text,cw,current_id)
    initialize_attr(justify,cw,current_id)
    initialize_attr(internal_width,cw,current_id)
    initialize_attr(internal_height,cw,current_id)
   }
/*--------------------------------------------------------------------------*\
|  Name: cws_attrs_vc
|
|  Description:
|    The cws_attrs_vc controls the display of the attributes form
|    associated with a particular cw instance.
\*--------------------------------------------------------------------------*/
  VC: cws_attrs_vc

    Creation Condition:  (display_attributes)

    Variables:

      send_flag: 0;	/* used to collect text from text widgets	*/
      page: 1;		/* controls display of attribute pages		*/

      parental_unit: get_bound_sd_instance(get_parent_vc(SELF));

      command_widget_help: false;

    Objects:

      cw_attributes_form: form_widget {
        Attributes:

          width:  300;
          height: 295;
      }

      page_form:  form_widget {
        Attributes:

          parent: cw_attributes_form;
          width: 300;
          height: 15;
          background_color: "black";
          border_width: 0;
          horiz_distance: 6;
          vert_distance: 1;
       }

      editable_title_bar(cw_title,
        page_form,
        "Command Widget:",
        100,
        object_instance_sdd.name,
        115
      )

      page_label: label_widget {
        Attributes:
          parent: page_form;
          background_color: "black";
          foreground_color: "white";
          height: 16;
          label_text: "Page";
          width: 32;
          horiz_distance: 215;
          vert_distance: 0;
          justify: LEFT;
      }

      page_selector(1, page_form, 249)

      page_selector(2, page_form, 264)

      page_selector(3, page_form, 279)

      cw_attributes: form_widget {
        Attributes:
          parent: cw_attributes_form;
          width: 290;
          height: 210;
          vert_distance: 28;
          horiz_distance: 6;
      }

      button(
        done_button,
        cw_attributes_form,
        260,
        NULL,
        10,
        NULL,
        "Done",
        {	
         display_attributes := false;
        }
      ) 

      button(
        apply_button,
        cw_attributes_form,
        260,
        NULL,
        65,
        NULL,
        "Apply",
        {	
          send_flag := 1;
        }
      ) 

      button(
        commit_button,
        cw_attributes_form,
        260,
        NULL,
        120,
        NULL,
        "Commit",
       {
      save_attr(parent,cw) 
      save_attr(x,cw)
      save_attr(y,cw)
      save_attr(width,cw)
      save_attr(height,cw)
      save_attr(border_width,cw)
      save_attr(border_color,cw)
      save_attr(background_color,cw)
      save_attr(sensitive,cw)
      save_attr(mapped_when_managed,cw)
      save_attr(foreground_color,cw)
      save_attr(label_text,cw)
      save_attr(justify,cw)
      save_attr(internal_width,cw)
      save_attr(internal_height,cw)
        }
      ) 


      button(
        cancel_button,
        cw_attributes_form,
        260,
        NULL,
        175,
        NULL,
        "Restore",
        {
          display_attributes := false;
        }
      ) 

      button(                 /* cancel button object                     */
        delete_button,
        cw_attributes_form,
        260,
        NULL,
        230,
        NULL,
        "Delete",
        {
          command_widget_help := TRUE;
        }
      )                       /* end cancel button object                 */

/*--------------------------------------------------------------------------*\
|  Name: page1_attributes follow
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_parent_attribute
|
|  Description:
|    The cw_parent_attribute controls the display of the 
|    parent attribute of the command widget.
\*--------------------------------------------------------------------------*/

        id_attribute(cw,			/* context		*/
          parent, 				/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          5,					/* vertical distance	*/
	  1,					/* page number		*/
          "parent          :")			/* label text		*/



/*--------------------------------------------------------------------------*\
|  Name:  cw_label_text_attribute
|
|  Description:
|    The cw_label_text_attribute controls the display of the 
|    label_text attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
	  label_text,				/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          ATTR_HEIGHT,				/* vertical distance	*/
          1,                                    /* page number          */
          "label text      :")			/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_x_attribute
|
|  Description:
|    The cw_x_attribute controls the display of the 
|    x attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
	  x, 				/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          2 * ATTR_HEIGHT,			/* vertical distance	*/
          1,                                    /* page number          */
          "x               :")			/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_y_attribute
|
|  Description:
|    The cw_y_attribute controls the display of the 
|    y attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
	  y, 				/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          3 * ATTR_HEIGHT,			/* vertical distance	*/
          1,                                    /* page number          */
          "y               :")			/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_width_attribute
|
|  Description:
|    The cw_width_attribute controls the display of the 
|    width attribute of the command widget.
\*--------------------------------------------------------------------------*/


        table_entry(cw,				/* context		*/
	  width,	 			/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          4 * ATTR_HEIGHT,			/* vertical distance	*/
          1,                                    /* page number          */
          "width           :")			/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_height_attribute
|
|  Description:
|    The cw_height_attribute controls the display of the 
|    height attribute of the command widget.
\*--------------------------------------------------------------------------*/


        table_entry(cw,				/* context		*/
	  height,	 			/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          5 * ATTR_HEIGHT,			/* vertical distance	*/
          1,                                    /* page number          */
          "height          :")			/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_foreground_color_attribute
|
|  Description:
|    The cw_foreground_color_attribute controls the display 
|    of the foreground_color attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
	  foreground_color,	 		/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
          22,					/* height		*/
          6 * ATTR_HEIGHT,			/* vertical distance	*/
          1,                                    /* page number          */
          "foreground color:")			/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_background_color_attribute
|
|  Description:
|    The cw_background_color_attribute controls the display 
|    of the background_color attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
	  background_color, 			/* name			*/
          cw_attributes,			/* parent		*/
          110,					/* width		*/
           22,					/* height		*/
          7 * ATTR_HEIGHT,			/* vertical distance	*/
          1,                                    /* page number          */
          "background color:")			/* label text		*/

/*---------------------------------------------end of page 1 attributes */

/*--------------------------------------------------------------------------*\
|  Name: page2_attributes
|
/*--------------------------------------------------------------------------*\
|  Name:  cw_border_width_attribute
|
|  Description:
|    The cw_border_width_attribute controls the display of the 
|    border_width attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/*context		*/
	  border_width,		 		/* name			*/
          cw_attributes,			/* parent		*/
          130,					/* width		*/
          22,					/* height		*/
          5,					/* vertical distance	*/
          2,                                    /* page number          */
          "border width       :")		/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name:  cw_border_color_attribute
|
|  Description:
|    The cw_border_color_attribute controls the display of the 
|    border_color attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
	  border_color,		 		/* name			*/
          cw_attributes,			/* parent		*/
          130,					/* width		*/
          22,					/* height		*/
          1 * ATTR_HEIGHT,			/* vertical distance	*/
          2,                                    /* page number          */
          "border color       :")		/* label text		*/


/*--------------------------------------------------------------------------*\
|  Name:  cw_internal_width_attribute
|
|  Description:
|    The cw_internal_width_attribute controls the display 
|    of the internal_width attribute of the command widget.
\*--------------------------------------------------------------------------*/
        table_entry(cw,				/* context		*/
	  internal_width,			/* name			*/
          cw_attributes,			/* parent		*/
          130,					/* width		*/
          22,					/* height		*/
          2 * ATTR_HEIGHT,			/* vertical distance	*/
          2,                                    /* page number          */
          "internal width     :")		/* label text		*/


/*--------------------------------------------------------------------------*\
|  Name:  cw_internal_height_attribute
|
|  Description:
|    The cw_internal_height_attribute controls the display 
|    of the internal_height attribute of the command widget.
\*--------------------------------------------------------------------------*/

        table_entry(cw,				/* context		*/
          internal_height, 			/* name			*/
          cw_attributes,			/* parent		*/
          130,					/* width		*/
          22,					/* height		*/
          3 * ATTR_HEIGHT,			/* vertical distance	*/
          2,                                    /* page number          */
          "internal height    :")		/* label text		*/

/*--------------------------------------------------------------------------*\
|  Name: page 3 attributes
|
/*--------------------------------------------------------------------------*\
|  Name:  cw_sensitive_attribute
|
|  Description:
|    The cw_sensitive_attribute controls the display of the 
|    sensitive attribute of the command widget.
\*--------------------------------------------------------------------------*/

         boolean_attribute(cw,				/* context	*/
           sensitive, 
           cw_attributes, 
           130, 
           22, 
           5, 
           3,                                    /* page number          */
           "sensitive          :")

/*--------------------------------------------------------------------------*\
|  Name:  cw_justify_attribute
|
|  Description:
|    The cw_justify_attribute controls the display of the 
|    justify attribute of the command widget.
\*--------------------------------------------------------------------------*/


        justify_label_text: command_widget {
          Attributes:
            parent: cw_attributes; 
            label_text: "justify:"; 
            height: 22; 
            width: 56; 
            vert_distance: 1 * ATTR_HEIGHT; 
            justify: LEFT; 
            border_color: "white"; 
            make_visible(3)		/* make visibile only when page = 2 */
          Methods:
            notify: {
              justify_cw_help := TRUE;
            }
        }

        justify_attr_left_button: command_widget {
          Attributes:
            parent: cw_attributes;
            vert_distance: 1 * ATTR_HEIGHT; 
            horiz_distance: 90;
            label_text: "Left";
            width: 50;
            height: 20; 
 
            background_color: {
              if (justify_cw_attr = LEFT) then
                background_color := "black";
              else
                background_color := "white";
              end if;
            }
            
            foreground_color: {
              if (justify_cw_attr = LEFT) then
                foreground_color := "white";
              else
                foreground_color := "black";
              end if;
            }

            justify: LEFT;
            make_visible(3)		/* make visibile only when page = 3 */

          Methods:
           notify: {
             justify_cw_attr := LEFT;
           }
        }

        justify_attr_center_button: command_widget {
          Attributes:
            parent: cw_attributes;
            vert_distance: 1 * ATTR_HEIGHT; 
            horiz_distance: 160;
            label_text: "Center";
            width: 50;
            height: 20; 

            background_color: {
              if (justify_cw_attr = CENTER) then 
                background_color := "black";
              else
                background_color := "white";
              end if;
            }
             
            foreground_color: {
              if (justify_cw_attr = CENTER) then
                foreground_color := "white";
              else
                foreground_color := "black";
              end if;
            }
            make_visible(3)		/* make visibile only when page = 3 */

          Methods:
            notify: {
              justify_cw_attr := CENTER;
            }
        } 

        justify_attr_right_button: command_widget {

          Attributes:
            parent: cw_attributes;
            vert_distance: 1 * ATTR_HEIGHT; 
            horiz_distance: 230;
            label_text: "Right";
            width: 50;
            height: 20; 
  
            background_color: {
              if (justify_cw_attr = RIGHT) then
                background_color := "black";
              else
                background_color := "white";
              end if;
            }
            
            foreground_color: {
              if (justify_cw_attr = RIGHT) then
                foreground_color := "white";
              else
                foreground_color := "black";
               end if;
            }

            justify: RIGHT;
            make_visible(3)		/* make visibile only when page = 3 */

          Methods:
            notify: {
              justify_cw_attr := RIGHT;
            }
         }

 
/*--------------------------------------------------------------------------*\
|  Name:  cw_mapped_when_managed_attribute
|
|  Description:
|    The cw_mapped_when_managed_attribute controls the display 
|    of the mapped_when_managed attribute of the command widget.
\*--------------------------------------------------------------------------*/

         boolean_attribute(cw,			/* context		*/
           mapped_when_managed, 
           cw_attributes, 
           130, 
           22, 
           2 * ATTR_HEIGHT, 
           3,                                    /* page number          */
           "mapped when managed:")


  end VC cws_attrs_vc;

/*--------------------------------------------------------------------------*\
|  Name: cw_instance_vc
|
|  Description:
|    The command_instance_vc controls the display of command widget instances
|    in the visualization window.
\*--------------------------------------------------------------------------*/
  VC: cw_instance_vc

     Creation Condition: (display_instance)

     Objects:

       command_real: command_widget {
         Attributes:
           parent: main_background;
           x: x_cw_attr;
           y: y_cw_attr;
           horiz_distance: x_cw_attr;
           vert_distance: y_cw_attr;
           width: width_cw_attr;
           height: height_cw_attr;
           border_width: border_width_cw_attr;
           border_color: border_color_cw_attr;
           background_color: background_color_cw_attr;
           sensitive: sensitive_cw_attr;
           mapped_when_managed: mapped_when_managed_cw_attr;
           foreground_color: foreground_color_cw_attr;
  	   label_text: label_text_cw_attr;
	   justify: justify_cw_attr;
	   internal_width: internal_width_cw_attr;
	   internal_height: internal_height_cw_attr;
           allow_user_move:  1 = 1;
           allow_user_resize: 1 = 1;

         Methods:

           notify: {
             display_attributes := TRUE;
           }

           move: {
             x_cw_attr := x;
             y_cw_attr := y;
           }

           resize: {
             x_cw_attr := x;
             y_cw_attr := y;
             width_cw_attr := width;
             height_cw_attr := height;
           }
	}

  end VC cw_instance_vc;


end VC cw_vc;
