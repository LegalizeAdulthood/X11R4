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

This slang program is not very general.  That is, the addition of something
beside a command widget would require essentially replicating the entire
program.  It is intended as an example to demonstrate the use of Serpent
with an application.
*/

/*--------------------------------------------------------------------------*\
|  two objects are defined here.  
|   1. a form for the background of all of the objects
|   2. a button to create a new command widget for management
\*--------------------------------------------------------------------------*/

   OBJECTS:

        main_background:     form_widget
        {ATTRIBUTES:
           width:  765;
           height: 645;
         }   

	create: command_widget
	{ATTRIBUTES:
           parent: main_background;
  
	   label_text: "Create command widget";

	   width: 140;
	   height: 20;

           vert_distance: 10;
           horiz_distance: 600;

         METHODS:
          notify: {
            create_element("command", "DM_BOX");
          }
	}

/*-------------------------------------------------------------------------
|   The "new" function will create a view controller instance whenever
|   a new instance of the command element is added to shared data.
-------------------------------------------------------------------*/

  VC: command_instance

    CREATION CONDITION:  (new("command"))

/*
    the components being set represent those variables which can be
    modified through the attribute form.  The settings are default values to
    be modified.  The modifications are automatically
    displayed in the realization window
*/
    VARIABLES:

     send_flag : 0;
     local_variable : 1;
     command_x : 0;
     command_y : 0;
     command_w : 100;
     command_h : 20;

     OBJECTS:

/*-----------------------------------------------------------------------
     The following objects form a table which allows the end user
     to input attributes textually
------------------------------------------------------------------------*/

        att_background:     form_widget
        {ATTRIBUTES:
           width:  200;
           height: 200;
         }

    /*----------------------------------------------------------------------*\
    | the ok_button signals the text widgets with their attributes
    | to send their current values
    \*----------------------------------------------------------------------*/

      ok_button: command_widget
	{ATTRIBUTES:
           parent: att_background;  

	   justify: 1;	/* center */
	   label_text: "OK";

	   width:  50;
	   height: 20;
           from_horiz: NULL;
           horiz_distance: 75;
           from_vert: NULL;
           vert_distance: 160;

         METHODS:
          notify: {
            send_flag := 1;
	  }
       }

    /*----------------------------------------------------------------------*\
    |  The next two objects represent an entry in the
    |  table which provides the text: "text" and the value
    |  This allows modification of the text within the constructed widget
    \*----------------------------------------------------------------------*/

      att_text_label: label_widget
       {ATTRIBUTES:

         parent: att_background;

         justify: 0;	/* left */
         label_text: "text:";

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 0;
         horiz_distance: 0;

         height: 25;
         width: 50;
       }

      att_text_value: text_widget
       {ATTRIBUTES:

         parent: att_background;

         width: 175;
         height: 25;

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 5;
         horiz_distance: 50;

         edit_type: 2;

         send_buffer: send_flag;   /* value of 1 is message to send text*/

         text_buffer: "<label>";

        METHODS:
         send: {
           command.text := text_buffer;
           send_flag := 0;
          }
        }

    /*----------------------------------------------------------------------*\
    |  The next two objects represent an entry in the
    |  table which provides the text: "height" and the value
    |  This allows modification of the size of the constructed widget
    \*----------------------------------------------------------------------*/

      att_height_label: label_widget
       {ATTRIBUTES:

         parent: att_background;

         justify: 0;
         label_text: "height        :";

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 25;
         horiz_distance: 0;

         height: 25;
         width: 105;
       }

      att_height_value: text_widget
       {ATTRIBUTES:

         parent: att_background;

         width: 120;
         height: 25;

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 30;
         horiz_distance: 105;

         edit_type: 2;

         send_buffer: send_flag;   /* value of 1 is message to send text*/

         text_buffer: command_h ;

        METHODS:
         send: {
           command.height := text_buffer;
           send_flag := 0;
          }
        }

    /*----------------------------------------------------------------------*\
    |  The next two objects represent an entry in the
    |  table which provides the text: "width" and the value
    |  This allows modification of the size of the constructed widget
    \*----------------------------------------------------------------------*/

      att_width_label: label_widget
       {ATTRIBUTES:

         parent: att_background;

         justify: 0;
         label_text: "width         :";

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 50;
         horiz_distance: 0;

         height: 25;
         width: 105;
       }

      att_width_value: text_widget
       {ATTRIBUTES:

         parent: att_background;

         width: 120;
         height: 25;

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 55;
         horiz_distance: 105;

         edit_type: 2;

         send_buffer: send_flag;   /* value of 1 is message to send text*/

         text_buffer: command_w;

        METHODS:
         send: {
           command.width := text_buffer;
           send_flag := 0;
          }
        }

    /*----------------------------------------------------------------------*\
    |  The next two objects represent an entry in the
    |  table which provides the text: "horiz_distance" and the value
    |  This allows modification of the position of the constructed widget
    \*----------------------------------------------------------------------*/

      att_horiz_label: label_widget
       {ATTRIBUTES:

         parent: att_background;

         justify: 0;
         label_text: "x             :";

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 75;
         horiz_distance: 0;

         height: 25;
         width: 105;
       }

      att_horiz_value: text_widget
       {ATTRIBUTES:

         parent: att_background;
         width: 120;
         height: 25;

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";
         vert_distance: 80;
         horiz_distance: 105;
         edit_type: 2;
         send_buffer: send_flag;   /* value of 1 is message to send text*/
         text_buffer: command_x;
	 max_length: 10 ;

        METHODS:
         send: {
           command.horiz_distance := text_buffer;
           send_flag := 0;
          }
        }

    /*----------------------------------------------------------------------*\
    |  The next two objects represent an entry in the
    |  table which provides the text: "vert_distance" and the value
    |  This allows modification of the position of the constructed widget
    \*----------------------------------------------------------------------*/

      att_vert_label: label_widget
       {ATTRIBUTES:

         parent: att_background;

         justify: 0;
         label_text: "y             :";

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 100;
         horiz_distance : 0;

         height: 25;
         width: 105;
       }

      att_vert_value: text_widget
       {ATTRIBUTES:

         parent: att_background;

         width: 120;
         height: 25;

         from_horiz: NULL;
         from_vert: NULL;

         border_color: "white";

         vert_distance: 105;
         horiz_distance: 105;

         edit_type: 2;

         send_buffer: send_flag;   /* value of 1 is message to send text*/

         text_buffer: command_y;

        METHODS:
         send: {
           command.vert_distance := text_buffer;
           send_flag := 0;
         }
       }

    /*----------------------------------------------------------------------*\
    |  The following object actually displays the
    |  widget which is created and which has
    |  attributes specified from the table
    \*----------------------------------------------------------------------*/

    VC: command_widget_instance

      Creation Condition: (local_variable = 1)

      Objects:

	command_real: command_widget
	{ATTRIBUTES:
           parent: main_background;
  	   label_text: att_text_value.text_buffer;
           width: att_width_value.text_buffer;
           height: att_height_value.text_buffer;
	   justify: 1;		/* center */
           vert_distance: att_vert_value.text_buffer;
           horiz_distance: att_horiz_value.text_buffer;
           x: horiz_distance;
           y: vert_distance;
           allow_user_resize: TRUE;
           allow_user_move: TRUE;

        METHODS:
	  move: {
            command_x := x;
            command_y := y;
          }
	  resize: {
            command_x := x;
            command_y := y;
            command_w := width;
            command_h := height;
          }
	}
    ENDVC command_widget_instance

  ENDVC command_instance
