#include "saw.ill"
#include "dm.ill"

|||
/*
This is the slang program to demonstrate use of Serpent in constructing
a visual programming interface.  

Initially created is a form upon which a menu of operations is presented, a
menu of commands are also presented, as well as a menu of colors.

There is a view controller template which is instantiated for each component
created.  It controls that particular operation and is related to the dm.sdd
component component_sdd.  Each component has an operation associated with it
as well as input and output lines.

There is another view controller template which is instantiated for each
input argument which has only output lines.

*/


#define LEFT 0
#define CENTER 1
#define RIGHT 2

#define BODY_COLOR "black"
#define BACKGROUND_COLOR "white"
#define NORMAL_FONT "*schoo*24*"

#include "create.sl"
#include "slv.sl"

EXTERNALS:  void ipb_start_recording (string, string);
EXTERNALS:  void printf (string, string);

/*--------------------------------------------------------------------------*\
|  several objects are defined here.  
|   1. a form for the background of all of the objects
|   2. buttons to create a new component
|   3. buttons for commands
|   4. buttons for color selection
\*--------------------------------------------------------------------------*/
  Variables:

    component_id: NULL;

    line_id: NULL;			/* the line_sdd id for the current 
					   line*/
    value_id: NULL;			/* the component or arg id for
					   current value*/
    source_x;				/* position for source  */
    source_y;				/* of line*/

    target_x;
    target_y;

    message: "Construct designed expression";
    debug: FALSE;
    op_menu: 650;
    command_menu: 725;
    send_flag: 0;
    current_command;
    init_x;
    init_y;

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
           if (component_id <> NULL) then
              init_x := selected_x;
              init_y := selected_y;
              put_component_value ( component_id, "create", TRUE);
           end if;

           }
         }   


        create_op(create_add,op_menu,40,component_id,+)

        create_op(create_minus,op_menu,75,component_id,-)

        create_op(create_mult,op_menu,110,component_id,*)

        create_op(create_div,op_menu,145,component_id,/)

	create_arg: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Arg";

	   width: 50;
	   height: 30;

           x: op_menu;
           y: 180;
           horiz_distance: x;
           vert_distance: y;
         Methods:
           notify: {
             component_id := create_sd_instance(
             "arg_sdd",
             "DM_BOX");
             put_component_value ( component_id, "create", FALSE);
             message := "Select position for argument";
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
         width: 760;
         height: 20;
         label_text: message;
         vert_distance: 625;
         horiz_distance: 51;
         font: "9x15";
       }


      title_bar_label: label_widget {
        Attributes:
          parent: main_background;
          border_width: 0;
          background_color: "black";
          foreground_color: "white";
          height: 30;
          label_text: "VISUALLY PROGRAMMABLE CALCULATOR";
          width: 800;
          horiz_distance: 1;
          vert_distance: 1;
          justify: CENTER;
          font: NORMAL_FONT;
         }

      calc_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Calc";

	   width: 50;
	   height: 30;

           x: command_menu;
           y: 40;
           horiz_distance: x;
           vert_distance: y;

         Methods:

	  notify: {
            send_flag := 1;
          }
         }

	debug_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Debug";

	   width: 50;
	   height: 30;

           x: command_menu;
           y: 75;
           horiz_distance: x;
           vert_distance: y;

         METHODS:

          notify: {
           if (debug) then
            debug := FALSE;
           else
            debug := TRUE;
           end if;
	}

      }

      delete_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Delete";

	   width: 50;
	   height: 30;

           x: command_menu;
           y: 110;
           horiz_distance: x;
           vert_distance: y;
 
         METHODS:

          notify: {
            current_command := "delete";
          }
        }

      connect_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Connect";

	   width: 50;
	   height: 30;

           x: command_menu;
           y: 145;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
           current_command := "connect";
           message := "Select source for value";
          }
         }

      dis_connect_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Dis-con";

	   width: 50;
	   height: 30;

           x: command_menu;
           y: 180;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
           current_command := "disconnect";
           message := "Select one end of the line to disconnect";
          }
         }

      record_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Record";

	   width: 50;
	   height: 30;

           x: command_menu;
           y: 215;
           horiz_distance: x;
           vert_distance: y;

        Methods:

         notify: {
           ipb_start_recording ("visual.record","see comunications");
          }
         }

      quit_object: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Quit";

	   width: 50;
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







#include "comp.sl"

#include "arg.sl"

#include "line.sl"
