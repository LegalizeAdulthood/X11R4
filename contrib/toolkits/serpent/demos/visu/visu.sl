
#include "saw.ill"
#include "dm.ill"

|||
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

#include "etb.sl"
#include "button.sl"
#include "catt.sl"
#include "cwd.sl"
#include "visi.sl"
#include "satt.sl"

#define LEFT 0
#define CENTER 1
#define RIGHT 2

EXTERNALS:  void ipb_start_recording (string, string);
#define record ipb_start_recording

EXTERNALS:  string cus_genname (string);
#define genname cus_genname

/*--------------------------------------------------------------------------*\
|  two objects are defined here.  
|   1. a form for the background of all of the objects
|   2. a button to create a new command widget for management
\*--------------------------------------------------------------------------*/
  Variables:

    front_attr;				/* holds forward pointer for children
 						list*/
    current_id;				/* holds current node id*/
    object_instance_sdi;		/* id for object instance*/
    original_x: 600;
    original_y: 10;
    original_width: 140;
    original_height: 20;
    record_off: TRUE;

   OBJECTS:

        main_background:     form_widget
        {ATTRIBUTES:
           width:  765;
           height: 645;
         }   

	create: command_widget
	{ATTRIBUTES:
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

/*        turn recording off so can run in demo directory
          if (record_off) then
             record("visualize.record","see communication in visualization");
             record_off := FALSE;
          end if;
*/

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
           "name",
           genname("CW")
         );

         put_component_value(
           object_instance_sdi,
           "visible",
           TRUE
         );

        create_attr(parent,object_instance_sdi,parent_cw_default,
              object_instance_sdi,current_id)

        front_attr := current_id;
        create_attr(x,object_instance_sdi,0,front_attr,current_id)

        front_attr := current_id;
        create_attr(y,object_instance_sdi,0,front_attr,current_id)

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

#include "cw.sl"

