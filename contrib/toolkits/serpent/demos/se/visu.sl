/* $Log:	visu.sl,v $
% Revision 1.1  89/10/10  22:38:23  serpent
% Initial revision
% 
Revision 1.3  89/06/20  14:22:17  rcs
modified ve to work as editor component vs stand alone

Revision 1.2  89/06/19  16:24:18  ljb
fix up rcs header
*/
/*
This is the slang program to demonstrate a simple prototyper for command 
widgets.  It allows the end user to create command widgets and manipulate
them based on textual input into a table.

  
Initially created are a form upon which the command widgets are positioned
and a button which when pressed will create another command widget to be
position.

There is a view controller template which is instantiated for each command
widget created.  It controls the input of the attributes and the assigning
of those attributes to the command widgets displayed for modification.

*/

#include "ntb.sl"
#include "button.sl"
#include "catt.sl"
#include "cwd.sl"
#include "visi.sl"
#include "satt.sl"

/*--------------------------------------------------------------------------*\
|  two objects are defined here.  
|   1. a form for the background of all of the objects
|   2. a button to create a new command widget for management
\*--------------------------------------------------------------------------*/
VC: ve_vc

  Creation Condition : (true)

  Variables:

    front_attr;				/* holds forward pointer for children
 						list*/
    current_id;				/* holds current node id*/
    object_instance_sdi;		/* id for object instance*/
    original_x: 600;
    original_y: 10;
    original_width: 140;
    original_height: 20;
    message: "";

   OBJECTS:

        main_background:     form_widget
        {ATTRIBUTES:
           width:  765;
           height: 645;
         }   

	create: command_widget {
          ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Command widget";

	   width: original_width;
	   height: original_height;

           vert_distance: original_y;
           horiz_distance: original_x;
           x: original_x;
           y: original_y;
           allow_user_move: TRUE;

         METHODS:
          notify: {

          record("visualize.record","see communication in visualization");
          }

          move: {

         message := "this message comes from move method";

         object_instance_sdi := create_sd_instance(
           "object_instance_sdd",
           "DM_BOX"
         );

         put_component_value(
           object_instance_sdi,
           "type",
           "command_widget"
         );

         put_component_value(
           object_instance_sdi,
           "visible",
           TRUE
         );

        create_attr(parent,object_instance_sdi,parent_cw_default,
              object_instance_sdi,current_id)

        front_attr := current_id;
        create_attr(x,object_instance_sdi,x,front_attr,current_id)

        front_attr := current_id;
        create_attr(y,object_instance_sdi,y,front_attr,current_id)

        front_attr := current_id;
        create_attr(width,object_instance_sdi,width_cw_default,
                     front_attr,current_id)

        front_attr := current_id;
        create_attr(height,object_instance_sdi,height_cw_default,
                    front_attr,current_id)

        front_attr := current_id;
        create_attr(border_width,object_instance_sdi,border_width_cw_default,
                    front_attr,current_id)

        front_attr := current_id;
        create_attr(border_color,object_instance_sdi,border_color_cw_default,
                    front_attr,current_id)

        front_attr := current_id;
        create_attr(background_color,object_instance_sdi,
                     background_color_cw_default,front_attr,current_id)

        front_attr := current_id;
        create_attr(sensitive,object_instance_sdi,sensitive_cw_default,
                    front_attr,current_id)

        front_attr := current_id;
        create_attr(mapped_when_managed,object_instance_sdi,
                    mapped_when_managed_cw_default,front_attr,current_id)

        front_attr := current_id;
        create_attr(foreground_color,object_instance_sdi,
                     foreground_color_cw_default,front_attr,current_id)

        front_attr := current_id;
        create_attr(label_text,object_instance_sdi,
                     label_text_cw_default,front_attr,current_id)

        front_attr := current_id;
        create_attr(justify,object_instance_sdi,justify_cw_default,
                    front_attr,current_id)

        front_attr := current_id;
        create_attr(internal_width,object_instance_sdi,
                     internal_width_cw_default,front_attr,current_id)

        front_attr := current_id;
        create_attr(internal_height,object_instance_sdi,
                     internal_height_cw_default,front_attr,current_id)

        put_component_value(current_id, "attr_list", NULL);

        x := original_x;
        y := original_y;
	}

      }

      message_bar_label: label_widget {
        Attributes:
          parent: main_background;
          border_width: 0;
          background_color: "black";
          foreground_color: "white";
          height: 20;
          label_text: "Message: ";
          width: 50;
          horiz_distance: 1;
          vert_distance: 625;
          justify: LEFT;
         }


      message_text: label_widget {
        Attributes:
         parent: main_background;
         border_width: 0;
         background_color: "black";
         foreground_color: "white";
         width: 705;
         height: 20;
         label_text: message;
         vert_distance: 625;
         horiz_distance: 51;
       }

#include "cw.sl"

end VC ve_vc;

