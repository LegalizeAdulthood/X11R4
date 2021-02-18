#include "saw.ill"
#include "dm.ill"

||| 

/* This is the slang program to demonstrate use of Serpent in grouping
objects for an interactive editor.  

Initially created is a form with  a menu of commands.

There is a view controller template which is instantiated for each command
widget created.  It controls the position of that particular command widget

There is another view controller template which is instantiated for each
group created.

*/


#define LEFT 0
#define CENTER 1
#define RIGHT 2

EXTERNALS:  string cus_genname (string);
EXTERNALS:  void ipb_start_recording (string, string);
EXTERNALS:  void printf (string, string);

/*--------------------------------------------------------------------------*\
|  several objects are defined here.  
|   1. a form for the background of all of the objects
|   2. buttons to create a new component
|   3. buttons for commands
\*--------------------------------------------------------------------------*/
  Variables:

    command_menu: 725;
    group_menu_x: 650;
    current_command;
    init_x;
    init_y;
    current_id: NULL;
    group_y_offset: 5;

   OBJECTS:

        main_background:     form_widget
        {ATTRIBUTES:
           width:  765;
           height: 645;
           background_color: BACKGROUND_COLOR;
           selected_x: 0;
           selected_y: 0;

         Methods:

          select: {
            if (current_command = "create_instance") then
              current_id := create_sd_instance(
                "instance_sdd",
                "DM_BOX");


              put_component_value(
                current_id,
                "instance_name",
                cus_genname("CW"));


              put_component_value(
                current_id,
                "group_id",
                NULL);

              init_x := selected_x;
              init_y := selected_y;
             end if;

           }
         }   


      create_instance_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Create Instance";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 5;
           horiz_distance: x;
           vert_distance: y;

         Methods:

	  notify: {
            current_command := "create_instance";
          }
         }

	create_group_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Create Group";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 40;
           horiz_distance: x;
           vert_distance: y;

         METHODS:

          notify: {
            current_id := create_sd_instance(
              "group_sdd",
              "DM_BOX");

            put_component_value(
              current_id,
              "x_offset",
              0);

            put_component_value(
              current_id,
              "y_offset",
              0);

            put_component_value(
              current_id,
              "group_name",
              cus_genname("Group"));

            put_component_value(
              current_id,
              "group_color",
              "white");

            put_component_value(
              current_id,
              "exist",
              TRUE);

	}

      }

      delete_instance_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Delete Instance";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 75;
           horiz_distance: x;
           vert_distance: y;
 
         METHODS:

          notify: {
           current_command := "delete_instance";
          }
        }

      delete_group_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Delete Group";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 110;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
           current_command := "delete_group";
          }
         }

      add_instance_to_group_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Add to Group";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 145;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
          current_command := "add_instance_to_group";
          }
         }

      delete_instance_from_group_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Delete From Group";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 180;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
          current_command := "delete_from_group";
          }
         }

      toggle_group_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Toggle Group";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 215;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
          current_command := "toggle_group";
          }
         }

      exit_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Quit";

	   width: 120;
	   height: 30;

           x: command_menu;
           y: 250;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
            exit();
          }
         }


#include "instance.sl"

#include "group_ins.sl"
