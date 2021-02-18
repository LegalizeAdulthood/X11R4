/* $Log:	se.sl,v $
% Revision 1.1  89/10/10  22:26:19  serpent
% Initial revision
% 
Revision 1.5  89/08/03  17:32:22  rcs
changed the way function defs work, for one thing

Revision 1.4  89/07/18  09:52:53  rcs
changed stuff here too

Revision 1.3  89/06/21  15:14:12  rcs
integrated with visulalize.slang and new macros
	*/

#define DEMO
#define PROTOTYPE

#ifndef PROTOTYPE
#include "de.ill"
#else
#include "dm.ill"
#endif

#include "saw.ill"
|||
/*--------------------------------------------------------------------------*\
|   
|  Name:  Serpent Editor (se)
|
|  Description:  
|     This is the topmost component for the dialogue editor dialogue.
|     It provides the environment and system level menus in which the
|     dialogue editor and visualization tool live.
|
|  Component of:
|     Serpent editor dialogue layer.
|
|  Related files:
|     se.slang - contains the slang source for the serpent editor.
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
|     Feb. 24, 1989 : Module created.
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

/*------------------- constants --------------------------------------------*/

#define LEFT 0
#define CENTER 1
#define RIGHT 2

#define TITLE_FONT_WIDTH 7

#define FONT_WIDTH 5
#define FONT_HEIGHT 7

#define NO_CURRENT_CMD 0

/*  Edit commands	*/

#define DELETE_CMD 1
#define VIEW_MODIFY_CMD 2
#define COPY_CMD 3
#define MOVE_CMD 4
#define REPEAT_CMD 5

/* Prototype Commands */

#define DISPLAY_OBJ_CMD 6
#define REMOVE_OBJ_CMD 7
#define ADD_OBJ_CMD 8
#define INSTANTIATE_OBJ_CMD 9
#define MODIFY_OBJ_CMD 10
#define ASSOCIATE_OBJ_CMD 11
#define DISASSOCIATE_OBJ_CMD 12
#define DELETE_OBJ_CMD 13

/* Attribute value constants. */

#define DISPLAY_WIDTH 1152
#define DISPLAY_HEIGHT 900
#define ATTR_HEIGHT 25 + 10

/*  New function names	*/

#define record ipb_start_recording

/*------------------- includes ---------------------------------------------*/

#include "dt.sl"
#include "command.sl"
#include "sawobj.sl"
#include "sawatt.sl"
#include "da.sl"
#include "menu1.sl"
#include "bar1.sl"
#include "button.sl"
#include "dtb.sl"
#include "etb.sl"
#include "text.sl"
#include "table.sl"
#include "idatt.sl"
#include "ht.sl"
#include "page.sl"
#include "batt.sl"
#include "catt.sl"
#include "select.sl"

/*------------------- externals --------------------------------------------*/

  Externals:

    void ipb_start_recording(string, string);


/*------------------- variables --------------------------------------------*/

  Variables:

    current_dialogue_sd: NULL;  /* of shared data for the "current" dlg */
    changed: false;		/* has current dialogue been changed	*/
    message: "";		/* used to display error message	*/
/*
   System menu options.
*/
    display_dialogue_menu: false;
    display_edit_menu: false;
    display_prototype_menu: false;
    display_rescue_menu: false;
    display_fonts_menu: false;
    display_help_menu: false;

    display_get_file_name_form: false;
    display_are_you_sure: false;

    current_command: NO_CURRENT_CMD;
/*
   Command forms.
*/
    display_associate_cmd_form: false;
    display_unassociated_objects: false;
    display_saw_object_types: false;
/*
   Current x and y position of language component forms.  These vars are
   incremented by 20 for each displayed form, mod the screen size in 
   each direction.
*/
    x_position: 20;
    y_position: 45;

    sensitive: false;
    
    error_foreground_color: "white";
    error_background_color: "red";

    hilite_foreground_color: "white";
    hilite_background_color: "blue";

    normal_foreground_color: "white";
    normal_background_color: "blue";

#ifdef PROTOTYPE
    temp_id;		/* temporary variable to hold ids in */
#endif


/*------------ Help Flags --------------------------------------------------*/
/*
   Dialogue commands.
*/
    load_cmd_help: false;
    save_cmd_help: false;
    revert_cmd_help: false;
    quit_cmd_help: false;
/*
   Edit commands.
*/
    copy_cmd_help: false;
    delete_cmd_help: false;
    move_cmd_help: false;
    repeat_cmd_help: false;
/*
   Prototype commands.
*/
    associate_cmd_help: false;
    instantiate_cmd_help: false;
/*
   Slang language help.
*/
    comments_help: false;
    prologue_help: false;
    variables_help: false;
    initialization_help: false;
    termination_help: false;
/*
   General technology help.
*/
    technologies_help: false;
/*
   Saw technology objects.
*/
    command_widget_help: false;
    label_widget_help: false;
    text_widget_help: false;
/*
  Saw technology object attributes.
*/
    parent_attribute_help: false;
    x_attribute_help: false;
    y_attribute_help: false;
    width_attribute_help: false;
    height_attribute_help: false;
    border_color_attribute_help: false;
    border_width_attribute_help: false;
    background_color_attribute_help: false;
    sensitive_attribute_help: false;
    mapped_when_managed_attribute_help: false;
    vert_distance_attribute_help: false;
    from_vert_attribute_help: false;
    horiz_distance_attribute_help: false;
    from_horiz_attribute_help: false;
    resizable_attribute_help: false;
    allow_resize_attribute_help: false;
    max_height_attribute_help: false;
    min_height_attribute_help: false;
    skip_adjust_attribute_help: false;
    foreground_color_attribute_help: false;
    label_text_attribute_help: false;
    justify_attribute_help: false;
    internal_width_attribute_help: false;
    internal_height_attribute_help: false;

  Objects:

    background: form_widget {
      Attributes:
        width: DISPLAY_WIDTH;
        height: DISPLAY_HEIGHT;
        border_color: "white";
    }

    system_menu_form: form_widget {
      Attributes:
        parent: background;
        width: DISPLAY_WIDTH;
        vert_distance: 0;
        horiz_distance: 0;
        height: 22;
        background_color: "black";
    }

    menu_bar_item(
      dialogue_menu_bar_item, 
      system_menu_form, 
      0, 
      "Dialogue",
      {
        display_dialogue_menu := true;
      }
    )

    menu_bar_item(
      edit_menu_bar_item, 
      system_menu_form, 
      70, 
      "Edit",
      {
        display_edit_menu := true;
      }
    )

    menu_bar_item(
      prototype_menu_bar_item, 
      system_menu_form, 
      140, 
      "Prototype",
      {
        display_prototype_menu := true;
      }
    )

    menu_bar_item(
      rescue_menu_bar_item, 
      system_menu_form, 
      210, 
      "Rescue",
      {
        display_rescue_menu := true;
      }
    )

    menu_bar_item(
      font_menu_bar_item, 
      system_menu_form, 
      280, 
      "Fonts",
      {
        display_fonts_menu := true;
      }
    ) 

    menu_bar_item(
      help_menu_bar_item, 
      system_menu_form, 
      350, 
      "Help",
      {
        display_help_menu := true;
      }
    )

#ifdef PROTOTYPE

  On create: {				/* actions on create		    */
/*
   Create a dialogue element to instantiate the dialogue VC.
*/
    current_dialogue_sd := create_sd_instance("dialogue_sdd", "DM_BOX");

    put_sd_value(
      current_dialogue_sd,
      "name",
      "Unnamed"
    );

    put_sd_value(
      current_dialogue_sd,
      "prologue",
      "Initial prologue text."
    );

    put_sd_value(
      current_dialogue_sd,
      "variable",
      "Initial variable text."
    );

    put_sd_value(
      current_dialogue_sd,
      "initialization",
      "Initial initialization text."
    );

    put_sd_value(
      current_dialogue_sd,
      "termination",
      "Initial termination text."
    );
/*
   The following are definitions of the object types supported by
   the prototyper.  These are normally provided by the application.
*/
    temp_id := create_sd_instance("object_type_sdd", "DM_BOX");
    put_sd_value(
      temp_id,
      "name",
      "Command Widget"
    );
    put_sd_value(
      temp_id,
      "type",
      command_widget_type
    );
    put_sd_value(
      temp_id,
      "number",
      1
    );

    temp_id := create_sd_instance("object_type_sdd", "DM_BOX");
    put_sd_value(
      temp_id,
      "name",
      "Line Object"
    );
    put_sd_value(
      temp_id,
      "type",
      line_object_type
    );
    put_sd_value(
      temp_id,
      "number",
      2
    );
  }					/* end actions on create	    */

#endif

/*--------------------------------------------------------------------------*\
|  Name: dialogue_menu_vc
|
|  Description:
|    The edit menu allows the user to select a dialogue command.
\*--------------------------------------------------------------------------*/
  VC: dialogue_menu_vc 

    Creation condition: (display_dialogue_menu)

    Variables:

    Objects:

      dialogue_menu_form: form_widget {

        Attributes:
          parent: background;
          width: 151;
          height: 122;
          border_width: 2;
          vert_distance: 25;
          horiz_distance: 0;
      }

      menu_item(
        load_menu_item, 
        dialogue_menu_form,
        0, 
        "Load",
        {
          display_dialogue_menu := false;
        }
      )

#ifndef PROTOTYPE

      menu_item(
        save_menu_item, 
        dialogue_menu_form,
        1, 
        "Save",
        {
/*
   If file name not defined, get file name.
*/
          display_get_file_name_form := true;

          selected_command := create_sd_instance("command_sdd", "DE_BOX");
/*
          command_sdd[selected_command].type = file_cmd;
*/
          put_sd_value(
            selected_command,
            "type",
            file_cmd
          );
/*
          command_sdd[selected_command].parameters = get_creating_sd(SELF);
*/
          put_sd_value(
           selected_command,
           "parameters",
           get_creating_sd(get_parent_vc(SELF))
          );

          display_dialogue_menu := false;

        }
      )
#else
      menu_item(
        save_menu_item, 
        dialogue_menu_form,
        1, 
        "Save",
        {
/*
   If file name not defined, get file name.
*/
          display_get_file_name_form := true;

          display_dialogue_menu := false;

        }
      )
#endif

      menu_item(
        file_menu_item, 
        dialogue_menu_form,
        2, 
        "Save As...",
        {
          display_get_file_name_form := true;
          display_dialogue_menu := false;
        }
      )

      menu_item(
        revert_menu_item, 
        dialogue_menu_form, 
        3, 
        "Revert",
        {
          display_dialogue_menu := false;
        }
      ) 

      menu_item(
        quit_menu_item, 
        dialogue_menu_form,
        4, 
        "Quit",
        {
          if (changed) then
            display_are_you_sure := true;
          else
            destroy_sd_instance(current_dialogue_sd);
          endif;
          display_dialogue_menu := false;
        }
      )

      menu_item(
        exit_menu_item, 
        dialogue_menu_form,
        5, 
        "Exit",
        {
          display_dialogue_menu := false;
        }
      )

  end VC dialogue_menu_vc

/*--------------------------------------------------------------------------*\
|  Name: edit_menu
|
|  Description:
|    The edit menu allows the user to select an edit command.
\*--------------------------------------------------------------------------*/
  VC: edit_menu_vc

    Creation condition: (display_edit_menu)

    Variables:

    Objects:

      edit_menu_form: form_widget {

        Attributes:
/* 
    Core attributes.
*/
          parent: background;
          width: 151;
          height: 122;
          border_width: 2;
          vert_distance: 25;
          horiz_distance: 70;
      }			/* end dialogue form object		    */

      menu_item(
        copy_menu_item, 
        edit_menu_form, 
        0, 
        "Copy",
        {
          current_command := COPY_CMD;
          display_edit_menu := false;
        }
      )

      menu_item(
        delete_menu_item, 
        edit_menu_form, 
        1, 
        "Delete",
        {
          current_command := DELETE_CMD;
          display_edit_menu := false;
        }
      )

      menu_item(
        move_menu_item, 
        edit_menu_form, 
        2, 
        "Move",
        {
          current_command := MOVE_CMD;
          display_edit_menu := false;
        }
      )

      menu_item(
        repeat_menu_item, 
        edit_menu_form, 
        3, 
        "Repeat",
        {
          current_command := REPEAT_CMD;
          display_edit_menu := false;
        }
      ) 

      menu_item(
        view_modify_menu_item, 
        edit_menu_form, 
        4, 
        "View/Modify",
        {
          current_command := VIEW_MODIFY_CMD;
          display_edit_menu := false;
        }
      )

      menu_item(
        exit_menu_item, 
        edit_menu_form, 
	5, 
        "Exit",
        {
          current_command := NO_CURRENT_CMD;
          display_edit_menu := false;
        }
      )

  end VC edit_menu_vc

/*--------------------------------------------------------------------------*\
|  Name: prototype_menu
|
|  Description:
|    The prototype menu allows the user to select a prototyping command.
\*--------------------------------------------------------------------------*/
  VC: prototype_menu_vc

    Creation condition: (display_prototype_menu)

    Variables:

    Objects:

      prototype_menu_form: form_widget {

        Attributes:
          parent: background;
          width: 151;
          height: 182;
          border_width: 2;
          vert_distance: 25;
          horiz_distance: 140;
      }			/* end dialogue form object		    */

      menu_item(
        display_menu_item, 
        prototype_menu_form, 
        0, 
        "Display",
        {
          current_command := DISPLAY_OBJ_CMD;
          display_prototype_menu := false;
        }
      )

      menu_item(
        remove_menu_item, 
        prototype_menu_form, 
        1, 
        "Remove",
        {
          current_command := REMOVE_OBJ_CMD;
          display_prototype_menu := false;
        }
      )

      menu_item(
        add_menu_item, 
        prototype_menu_form, 
        2, 
        "Add",
        {
          current_command := ADD_OBJ_CMD;
          display_saw_object_types := true;
          display_prototype_menu := false;
        }
      )

      menu_item(
        instantiate_menu_item, 
        prototype_menu_form, 
        3, 
        "Instantiate",
        {
          current_command := INSTANTIATE_OBJ_CMD;
          display_prototype_menu := false;
        }
      ) 

      menu_item(
        modify_menu_item, 
        prototype_menu_form, 
        4, 
        "View/Modify",
        {
          current_command := MODIFY_OBJ_CMD;
          display_prototype_menu := false;
        }
      )

      menu_item(
        associate_menu_item, 
        prototype_menu_form, 
	5, 
        "Associate",
        {
          current_command := ASSOCIATE_OBJ_CMD;
          display_prototype_menu := false;
          display_associate_cmd_form := true;
        }
      )

      menu_item(
        disassociate_menu_item, 
        prototype_menu_form, 
	6, 
        "Disassociate",
        {
          current_command := DISASSOCIATE_OBJ_CMD;
          display_prototype_menu := false;
        }
      )

      menu_item(
        delete_menu_item, 
        prototype_menu_form, 
	7, 
        "Delete",
        {
          current_command := DELETE_OBJ_CMD;
          display_prototype_menu := false;
        }
      )

      menu_item(
        exit_menu_item, 
        prototype_menu_form, 
	8, 
        "Exit",
        {
          current_command := NO_CURRENT_CMD;
          display_prototype_menu := false;
        }
      )

  end VC prototype_menu_vc

/*--------------------------------------------------------------------------*\
|  Name: display_text_vc
|
|  Description:
|    Generalized view controller mechanism to read in text.  Instantiated
|    on a new text_sdd.  Communication backwards is through changes to
|    the shared data element.
\*--------------------------------------------------------------------------*/
  VC: display_text_vc

    Creation condition: (new("text_sdd"))

    Objects:

      text_form: form_widget {
        Attributes:
          parent: background;
          width: 410;
          height: 251;
          allow_user_move: true;
          vert_distance: DISPLAY_HEIGHT / 2 - height / 2;
          horiz_distance: DISPLAY_WIDTH / 2 - width / 2;
          x: 0;
          y: 0;
        Methods:
          move: {
             horiz_distance := x;
             vert_distance := y;
           }
      }

      dual_title_bar(
        text_title,
        text_form,
        text_sdd.title,
        text_sdd.title_length * TITLE_FONT_WIDTH,
        text_sdd.parent,
        409 - (text_sdd.title_length * TITLE_FONT_WIDTH)
      )

      text_text: text_widget {
        Attributes:
          parent: text_form;
          width: 80 * FONT_WIDTH;
          height: 24 * FONT_HEIGHT;
          vert_distance: 23;
          resizable: 0;
          allow_resize: 0;
          edit_type: 2; /* XttextEdit */
          max_length: 256;
          scroll_vertical: 1;
          word_break: 1;
          text_buffer: text_sdd.text;
        Methods:
          send: {
/* 
   Set the displayed variable to false so this text can be selected again.
*/
            put_variable_value(
              text_sdd.disp_parent, 
              text_sdd.display_var, 
              false
            );
/* 
   Return text buffer in specified location.
*/
            put_sd_value(
              text_sdd.return_value,
              text_sdd.return_comp,
              text_buffer
            );
/*
   Destroy the shared data instance that caused this vc to be created.
*/
            destroy_sd_instance(text_sdd);
          }
      }

      button(
        ok_button,
        text_form,
        20,
        text_text,
        105,
        NULL,
        "OK",
        {
          text_text.send_buffer := 1;
        }
      )

       button(
        cancel_button,
        text_form,
        20,
        text_text,
        175,
        NULL,
        "Cancel",
        {
/* 
   Set the displayed variable to false so this text can be selected again.
*/
          put_variable_value(
            text_sdd.disp_parent, 
            text_sdd.display_var, 
            false
          );
/*
   Destroy the shared data instance that caused this vc to be created.
*/
          destroy_sd_instance(text_sdd);
        }
      )
/*
   The help_button provides help information for the specific 
   text window.  When selected, the help_button sets the variable
   specified in the text_sdd to true.  NOTE:  It is assumed this
   variable exists in the topmost layer of the dialogue.
*/
       button(
        help_button,
        text_form,
        20,
        text_text,
        245,
        NULL,
        "Help",
        {
          put_variable_value(get_grandparent(SELF), text_sdd.help_var, true);
        }
      )

  end VC display_text_vc

/*--------------------------------------------------------------------------*\
|  Name: get_file_name_vc
|
|  Description:
|    Gets the name of the file in which to save the dialogue.
\*--------------------------------------------------------------------------*/
  VC: get_file_name_vc

    Creation condition: (display_get_file_name_form)

    Objects:

      get_file_name_form: form_widget {
        Attributes:
          parent: background;
          width: 302;
          height: 142;
          allow_user_move: true;
          horiz_distance: 600;
          vert_distance: 200;
          x: 0;
          y: 0;
        Methods:
          move: {
             horiz_distance := x;
             vert_distance := y;
           }
      }

      get_file_name_title: label_widget {
        Attributes:
         parent: get_file_name_form;
         background_color: "black";
         foreground_color: "white";
         height: 18;
         label_text: "Save Command";
         width: 298;
         horiz_distance: 1;
         vert_distance: 1;
     }

     get_file_name_label: label_widget {
       Attributes:
         parent: get_file_name_form;
         border_color: "white";
         label_text: "Save dialogue as:";
         justify: LEFT;
         horiz_distance: 18;
         vert_distance: 30;
      }

      current_object_text: text_widget {
        Attributes:
          parent: get_file_name_form;
          width: 260;
          height: 20;
          edit_type: 2;
/* ??? what if the name is undefined? */
          text_buffer: get_sd_value(current_dialogue_sd, "file_name");
          vert_distance: 60;
          horiz_distance: 20;
          send_buffer: 0;
        Methods:
          send: {
/* ??? do we want to do any error checking for valid file names? */
            put_sd_value(current_dialogue_sd, "file_name", text_buffer);
            display_get_file_name_form := false;
          }
      }

      big_button(
        save_button,
        get_file_name_form,
        100,
        NULL,
        45,
        NULL,
        "Save",
        {
            current_object_text.send_buffer := 1;
        }
     ) 

      big_button( 
        cancel_button,
        get_file_name_form,
        100,
        NULL,
        120,
        NULL,
        "Cancel",
        {
          current_command := NO_CURRENT_CMD;
          display_get_file_name_form := false;
        }
      ) 

      big_button(
        help_button,
        get_file_name_form,
        100,
        NULL,
        195,
        NULL, 
        "Help" ,
        { 
          save_cmd_help := true;
        }
      ) 

  end VC get_file_name_vc

/*----- Include the source code for the dialogue editor --------------------*/

#include "de.sl"

/*----- Include the source code for the visualization tool -----------------*/

#include "visu.sl"

/*----- Include the source code for the help displays----- -----------------*/

#include "help.sl"
