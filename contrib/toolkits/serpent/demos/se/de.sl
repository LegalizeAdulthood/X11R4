/*--------------------------------------------------------------------------*\
|  VC: dialogue_vc
|
|  Description:
|    Displays a form widget containing command buttons for the
|    various dialogue components.
\*--------------------------------------------------------------------------*/
  VC: dialogue_vc

    Creation condition: (new("dialogue_sdd"))

    Variables:

      selected_command;
      display_include_menu: false;
      get_file_name: false;
      dialogue_changed: false;
      vc_list_sd: NULL;
      vc_sd: NULL;
      object_list_sd: NULL;
      object_sd: NULL;
      send_flag: 0;
      prologue_text;
      variables_text;
      initialization_text;
      termination_text;
/*
   The following variables are used to prevent menu items which
   create shared data elements from creating them more than once.
*/
      prologue_displayed: false;
      includes_displayed: false;
      variables_displayed: false;
      initialization_displayed: false;
      termination_displayed: false;
      vcs_displayed: false;
      objects_displayed: false;

    Objects:

      dialogue_form: form_widget {
        Attributes:
          parent: background;
          width: 151;
          height: 162;
          allow_user_move: true;
/* ?? the next 4 attrs can be removed as soon as the compiler bug is fixed */
          horiz_distance: 0;
          vert_distance: 0;
          x: 0;
          y: 0;
        Methods:
          move: {
            horiz_distance := x;
            vert_distance := y;
          }
      }

      editable_title_bar(
        dialogue_form_title,  
        dialogue_form,
	"Dialogue:",
        64,
        dialogue_sdd.name,
        86
      )

      menu_item(
        prolog_menu_item, 
        dialogue_form,
        1, 
        "Prologue",
        {
/*
   If the prologue for this dialogue is not already displayed, display it
   and set the the display flag to true.
*/
          if (NOT prologue_displayed) then

            prologue_displayed := true;

            message := "Enter prologue text.";

            prologue_text := create_sd_instance("text_sdd", "DM_BOX");
            put_sd_value(prologue_text, "title", "Prologue:");
            put_sd_value(prologue_text, "title_length", 9);
            put_sd_value(prologue_text, "text", dialogue_sdd.prologue);
            put_sd_value(prologue_text, "parent", dialogue_sdd.name);
            put_sd_value(prologue_text, "return_value", dialogue_sdd);
            put_sd_value(prologue_text, "return_comp", "prologue");
            put_sd_value(prologue_text, "help_var", "prologue_help");
            put_sd_value(prologue_text, "disp_parent", get_parent_vc(SELF));
            put_sd_value(prologue_text, "display_var", "prologue_displayed");

          end if;

        }
      )

      menu_item(
        include_menu_item, 
        dialogue_form,
        2, 
        "Includes",
        { 
/*
   Display the include menu if the technologies window is not displayed.
*/
          if (NOT includes_displayed) then
            includes_displayed := true;
            display_include_menu := true; 
          end if;
        }
      )

      menu_item(
        variables_menu_item, 
        dialogue_form,
        3, 
        "Variables",
        {
/*
   If the variables for this dialogue is not already displayed, display it
   and set the display flag to true.
*/
          if (NOT variables_displayed) then

            variables_displayed := true;

            message := "Enter variable definitions.";

            variables_text := create_sd_instance("text_sdd", "DM_BOX");
            put_sd_value(variables_text, "title", "Variables:");
            put_sd_value(variables_text, "title_length", 10);
            put_sd_value(variables_text, "text", dialogue_sdd.variable);
            put_sd_value(variables_text, "parent", dialogue_sdd.name);
            put_sd_value(variables_text, "return_value", dialogue_sdd);
            put_sd_value(variables_text, "return_comp", "variable");
            put_sd_value(variables_text, "help_var", "variables_help");
            put_sd_value(variables_text, "disp_parent", get_parent_vc(SELF));
            put_sd_value(
              variables_text, 
              "display_var", 
              "variables_displayed"
            );
          end if;

        }
      )


      menu_item(
        initialization_menu_item, 
        dialogue_form,
        4, 
        "Initialization",
        {
/*
   If the initialization for this dialogue is not already displayed, display
   it and set the display flag to true.
*/
          if (NOT initialization_displayed) then

            initialization_displayed := true;

            message := "Enter intialization actions.";

            initialization_text := create_sd_instance("text_sdd", "DM_BOX");
            put_sd_value(initialization_text, "title", "Initialization:");
            put_sd_value(initialization_text, "title_length", 15);
            put_sd_value(
              initialization_text, "text", dialogue_sdd.initialization
            );
            put_sd_value(initialization_text, "parent", dialogue_sdd.name);
            put_sd_value(initialization_text, "return_value", dialogue_sdd);
            put_sd_value(
              initialization_text, "return_comp", "initialization"
            );
            put_sd_value(
              initialization_text, "help_var", "initialization_help"
            );
            put_sd_value(
              initialization_text, "disp_parent", get_parent_vc(SELF)
            );
            put_sd_value(
              initialization_text, "display_var", "initialization_displayed"
            );

          end if;

        }
      )

      menu_item(
        termination_menu_item, 
        dialogue_form,
        5, 
        "Termination",
        {
/*
   If the termination for this dialogue is not already displayed, display
   it and set the display flag to true.
*/
          if (NOT termination_displayed) then

            termination_displayed := true;

            message := "Enter intialization actions.";

            termination_text := create_sd_instance("text_sdd", "DM_BOX");
            put_sd_value(termination_text, "title", "Termination:");
            put_sd_value(termination_text, "title_length", 12);
            put_sd_value(termination_text, "text", dialogue_sdd.termination);
            put_sd_value(termination_text, "parent", dialogue_sdd.name);
            put_sd_value(termination_text, "return_value", dialogue_sdd);
            put_sd_value(termination_text, "return_comp", "termination");
            put_sd_value(termination_text, "help_var", "termination_help");
            put_sd_value(
              termination_text, "disp_parent", get_parent_vc(SELF)
            );
            put_sd_value(
              termination_text, "display_var", "termination_displayed"
            );

          end if;

        }
      )

#ifndef PROTOTYPE
      menu_item(
        object_menu_item, 
        dialogue_form,
        6, 
        "Objects",
        {
          selected_command := create_sd_instance("command_sdd", "DE_BOX");
/*
          command_sdd[selected_command].type = get_topmost_objects_cmd;
*/
          put_sd_value(
           selected_command, 
           "type", 
           get_topmost_objects_cmd
          );
/*
   Allow for the creation of a new object list.
*/
          object_list_sd := create_sd_instance("object_list_sdd", "DM_BOX");

/* object_list_sd.parent = dialogue_sdd; */

          put_sd_value(
            object_list_sd,
            "parent",
            dialogue_sdd
          );

        }
      )
#else
      menu_item(
        object_menu_item, 
        dialogue_form,
        6, 
        "Objects",
        {
/*
   If the objects for this dialogue are not already displayed, display them
   and set the the display flag to true.
*/
          if (NOT objects_displayed) then

            objects_displayed := true;

/*
   Allow for the creation of a new object list and save in a local variable
   so we can get rid of it later.
*/
            object_list_sd := create_sd_instance(
              "object_list_sdd", 
              "DM_BOX"
            );
/*
    Create a new object_sdd element and assign the name component to "Fred"
    and the type to "Command Widget".
*/

/* object_list_sd.parent = dialogue_sdd; */

            put_sd_value(
              object_list_sd,
              "parent",
              dialogue_sdd
            );

/*
   Create at least one object to appear on the list.  We'll name this
   object "New" and give it an object type of command widget.
*/

            object_sd := create_sd_instance("object_sdd", "DM_BOX");

/* object_sd.name = "New"; */

            put_sd_value(
              object_sd,
              "name",
              "New"
            );

/* object_sd.parent = dialogue_sdd; */

            put_sd_value(
              object_sd,
              "parent",
              dialogue_sdd
            );

/* object_sd.type = command_widget_type; */

            put_sd_value(
              object_sd,
              "type",
              command_widget_type
            );

          end if;
        }
      )
#endif

#ifndef PROTOTYPE
      menu_item(
        vc_menu_item, 
        dialogue_form,
        6, 
        "View Controllers",
        {
          selected_command := create_sd_instance("command_sdd", "DE_BOX");
/*
          command_sdd[selected_command].type = get_topmost_vcs_cmd;
*/
          put_sd_value(
           selected_command, 
           "type", 
           get_topmost_vcs_cmd
          );
/*
   Allow for the creation of a new vc list.
*/
          vc_list_sd := create_sd_instance("vc_list_sdd", "DM_BOX");

/* vc_list_sd.parent = dialogue_sdd; */

          put_sd_value(
            vc_list_sd,
            "parent",
            dialogue_sdd
          );

/*
    Create a new command_sdd element to get the list of view controllers
    defined for this dialogue.
*/

          selected_command := create_sd_instance("command_sdd", "DM_BOX");

/* command_sd.type = get_subvcs_cmd; */

          put_sd_value(
            selected_command,
            "type",
            get_subvcs_cmd
          );
        }

      )
#else
      menu_item(
        vc_menu_item, 
        dialogue_form,
        7, 
        "View Controllers",
        {
/*
   If the vcs for this dialogue are not already displayed, display them
   and set the the display flag to true.
*/
          if (NOT vcs_displayed) then

            vcs_displayed := true;
/*
   Allow for the creation of a new vc list.
*/
            vc_list_sd := create_sd_instance("vc_list_sdd", "DM_BOX");

/* vc_list_sd.parent = dialogue_sdd; */

            put_sd_value(
              vc_list_sd,
              "parent",
              dialogue_sdd
            );
/*
    Create a new vc_sdd element and assign the name component to "New"
    and the parent id to the parent.
*/
            vc_sd := create_sd_instance("vc_sdd", "DM_BOX");

/* vc_sd.name = "New"; */

            put_sd_value(
              vc_sd,
              "name",
              "New"
            );

/* vc_sd.parent = dialogue_sdd; */

            put_sd_value(
              vc_sd,
              "parent",
              dialogue_sdd
            );
          end if;
        }
      )
#endif
/*
   On create increment the x and y positions and assign to the corresponding
   attributes in the dialogue form.
*/
    On Create: {				/* actions on create	    */

      message := "Dialogue loaded.";

      current_dialogue_sd := dialogue_sdd;

      x_position := x_position + 20;
      y_position := y_position + 20;

      dialogue_form.horiz_distance := x_position;
      dialogue_form.vert_distance := y_position;

    }						/* end actions on create    */

/*------ Object editor component of the Structure Editor -------------------*/

#include "deo.sl"

/*--------------------------------------------------------------------------*\
|  VC: include_vc
|
|  Description:
|    Displays a menu which allows the user to include sdds, technologies,
|    functions, or add functions.
\*--------------------------------------------------------------------------*/
    VC: include_menu_vc

      Creation condition: (display_include_menu)

      Variables:

        parental_unit: dialogue_sdd;
        technology_list_sd: NULL;
        technology_sd: NULL;
        
      Objects:

        include_menu_form: form_widget {
          Attributes:
            parent: background;
            width: 151;
            height: 142;
            allow_user_move: true;
            horiz_distance: 500;
            vert_distance: 200;
            x: 0;
            y: 0;
          Methods:
            move: {
              horiz_distance := x;
              vert_distance := y;
            }
        }

        dual_title_bar(
          include_menu_form_title,  
          include_menu_form,
          "Includes:",
          64,
          dialogue_sdd.name,
          86
        )

        menu_item(
          sdd_menu_item, 
          include_menu_form,
          1, 
          "Shared Data Definition",
          { display_include_menu := false; }
        ) 

#ifndef PROTOTYPE
        menu_item(
          tech_menu_item, 
          include_menu_form,
          2, 
          "Technologies",
          { 
            display_include_menu := false; 
          }
        )
#else
      menu_item(
        tech_menu_item, 
        include_menu_form,
        2, 
        "Technologies",
        {
/*
   Allow for the creation of a technology list and save in a local variable
   so we can get rid of it later.
*/
          technology_list_sd := create_sd_instance(
            "technology_list_sdd", 
            "DM_BOX"
          );

/* technology_list_sd.parent = dialogue_sdd; */

          put_sd_value(
            technology_list_sd,
            "parent",
            dialogue_sdd
          );
/*
    Create a new technology_sdd element and assign the name 
    to "Athena Widgets".
*/
          technology_sd := create_sd_instance("technology_sdd", "DM_BOX");

/* technology_sd.name = "Athena Widgets"; */

          put_sd_value(
            technology_sd,
            "name",
            "Athena Widgets"
          );

/* technology_sd.parent = dialogue_sdd; */

          put_sd_value(
            technology_sd,
            "parent",
            dialogue_sdd
          );
/*
    Create a new technology_sdd element and assign the name 
    to "Softcopy Mapping System".
*/
          technology_sd := create_sd_instance("technology_sdd", "DM_BOX");

/* technology_sd.name = "Softcopy Mapping System"; */

          put_sd_value(
            technology_sd,
            "name",
            "Softcopy Mapping System"
          );

/* technology_sd.parent = dialogue_sdd; */

          put_sd_value(
            technology_sd,
            "parent",
            dialogue_sdd
          );
/*
   Cause this meny to be removed from the display.
*/   
            display_include_menu := false; 
        }
      )
#endif

        menu_item(
          prolog_menu_item, 
          include_menu_form,
          3, 
          "Function Library",
          { display_include_menu := false; }
        )

        button(
          cancel_button,
          include_menu_form,
          100,
          NULL,
          50,
          NULL,
          "Cancel",
          {
            display_include_menu := false;
          }
        )

    end VC include_menu_vc

/*--------------------------------------------------------------------------*\
|  VC: technologies_vc
|
|  Description:
|    Allows the user to examine a list of technologies which have been 
|    integrated into serpent and registered with the editor and select
|    which technologies should be included with the current dialogue.
\*--------------------------------------------------------------------------*/
    VC: technologies_vc

      Creation condition: (new("technology_list_sdd"))

      Variables:

        technology_list_length: 0;

        parental_unit: dialogue_sdd;

      Objects:

        technologies_form: form_widget {
          Attributes:
            parent: background;
            width: 302;
            height: 82 + (20 * technology_list_length);
            allow_user_move: true;
            horiz_distance: 500;
            vert_distance: 200;
            x: 0;
            y: 0;
          Methods:
            move: {
              horiz_distance := x;
              vert_distance := y;
            }
        }

        dual_title_bar(
          technologies_form_title,  
          technologies_form,
          "Technologies:",
          84,
          dialogue_sdd.name,
          216
        ) 

        big_button(
          ok_button,
          technologies_form,
          50 + (20 * technology_list_length),
          NULL,
          45,
          NULL,
          "OK",
          {
            includes_displayed := false;
            destroy_sd_instance(technology_list_sdd);
          }
        )

        big_button(
          cancel_button,
          technologies_form,
          50 + (20 * technology_list_length),
          NULL,
          120,
          NULL,
          "Cancel",
          {
            includes_displayed := false;
            destroy_sd_instance(technology_list_sdd);
          }
        )

        big_button(
          help_button,
          technologies_form,
          50 + (20 * technology_list_length),
          NULL,
          195,
          NULL,
          "Help",
          {
            technologies_help := true;
          }
        )

/*--------------------------------------------------------------------------*\
|  VC: technology_vc
|
|  Description:
|    Displays a selectable technology item.
\*--------------------------------------------------------------------------*/
      VC: technology_vc

        Creation condition:
          (
            (new("technology_sdd")) AND 
            (technology_sdd.parent = parental_unit)
          )

        Variables:
          selected: true;	/* local status of technology	*/
          list_position;	/* position of technology in list	*/
 
        Objects:

          select_item(
            technology, 
            technologies_form,
            list_position,
            technology_sdd.name,
            selected
          ) 

        On Create: {
          technology_list_length := technology_list_length + 1;
          list_position := technology_list_length;
        }
/*
   Make sure the bound shared data instance is destroyed so it
   is not hanging around getting ready to screw things up the
   next time this vc's parent is instantiated.
*/
        On Destroy: {
            destroy_sd_instance(technology_sdd);
        }

      end VC technology_vc

    end VC technologies_vc

/*--------------------------------------------------------------------------*\
|  VC: vc_list_vc
|
|  Description:
|    Displays the lists of vc's defined at a certain level.  Instantiates
|    on new vc list shared data element.
\*--------------------------------------------------------------------------*/
    VC: vc_list_vc

      Creation condition: (new("vc_list_sdd"))

      Variables:

        vc_list_length: 0;

        parental_unit: vc_list_sdd.parent;

      Objects:

        vc_list_form: form_widget {		/* vc form object	    */
          Attributes:
            parent: background;
            width: 151;
            height: 82 + (20 * vc_list_length); 
            allow_user_move: true;
            horiz_distance: 0;
            vert_distance: 0;
            x: 0;
            y: 0;
          Methods:
            move: {
              horiz_distance := x;
              vert_distance := y;
            }
        }			/* end VC form object			    */

        dual_title_bar(		/* vc title object			    */
          vc_title,  
          vc_list_form,
          "VC List:",
          60,
          {
            if (get_sd_value(parental_unit, "name") = UNDEFINED) then
              label_text := "Unnamed";
            else
              label_text := get_sd_value(
                parental_unit,
                "name"
              );
            endif;
          },
          90
        )			/* end VC title object			    */
/*
   The return button causes the list of vcs to be removed.
*/
        button(
          return_button,
          vc_list_form,
          40 + (20 * vc_list_length),
          NULL,
          50,
	  NULL,
          "Return",
          {
/*
   Destroy the vc_list_sdd instance that caused this vc to be created.
*/
            vcs_displayed := false;
            destroy_sd_instance(vc_list_sdd);
            
          }
        )
/*
   On Create increment the x and y positions for the next object.
*/
      On Create: {

        x_position := x_position + 20;
        y_position := y_position + 20;

        vc_list_form.horiz_distance := x_position;
        vc_list_form.vert_distance := y_position;

      }

/*--------------------------------------------------------------------------*\
|  VC: vc_button_vc
|
|  Description:
|    For each new subvc that goes on this vc list add a button.
\*--------------------------------------------------------------------------*/
      VC: vc_button_vc

        Creation condition: 

          ((new("vc_sdd")) AND (vc_sdd.parent = parental_unit))

        Variables:
          display_vc: false;
          list_position;        /* position of vc in list       */

        Objects:

          menu_item(
            vc_menu_item, 
            vc_list_form,
            list_position,
            (
              get_sd_value(
                vc_sdd,
                "name"
              )
            ),
            {
              display_vc := true;
            }
          )

        On Create: {
          vc_list_length := vc_list_length + 1;
          list_position := vc_list_length;
        }
/*
   Make sure the bound shared data instance is destroyed so it
   is not hanging around getting ready to screw things up the
   next time this vc's parent is instantiated.
*/
        On Destroy: {
          destroy_sd_instance(vc_sdd);
        }

/*--------------------------------------------------------------------------*\
|  VC: vc_vc
|
|  Description:
|    Displays a VC template
\*--------------------------------------------------------------------------*/
        VC: vc_vc

          Creation condition: (display_vc)

          Variables:
            display_comments: false;
            display_creation_condition: false;
            display_variables: false;
            display_on_create: false;
            display_on_destroy: false;
            vc_list_sd: NULL;
            display_are_you_sure: false;
            changes: 0;	!* the number of changes made below this vc *!
            object_list_sd: NULL;
            object_sd: NULL;
            send_flag: 0;

          Objects:

            vc_form: form_widget {
              Attributes:
                parent: background;
                width: 151;		/* width + 2 * border_width	    */
                height: 222;
                allow_user_move: true;
/* ??? next four attrs temp fix	*/
                horiz_distance: 0;
                vert_distance: 0;
                x: 0;
                y: 0;
              Methods:
                move: {
                  horiz_distance := x;
                  vert_distance := y;
                }
            }

            editable_title_bar(
              vc_form_title,  
              vc_form,
              "VC:",
              30,
              vc_sdd.name,
              120
            )

            menu_item(
              cc_menu_item, 
              vc_form,
              1, 
              "Comments",
              { display_comments := true; }
            )

            menu_item(
              comments_menu_item, 
              vc_form,
              2, 
              "Creation condition",
              { display_creation_condition := true; }
            )

            menu_item(
              variables_menu_item, 
              vc_form,
              3, 
              "Variables",
              { display_variables := true; }
            )

            menu_item(
              on_create_menu_item, 
              vc_form,
              4, 
              "On Create",
              { display_on_create := true; }
            )

            menu_item(
              on_destroy_menu_item, 
              vc_form,
              5, 
              "On Destroy",
              { display_on_destroy := true; }
            )

            menu_item(
              objects_menu_item, 
              vc_form,
              6, 
              "Objects",
              {
/*
   Allow for the creation of a new object list and save in a local variable
   so we can get rid of it later.
*/
                object_list_sd := create_sd_instance(
                  "object_list_sdd", 
                  "DM_BOX"
                );
/*
    Create a new object_sdd element and assign the name component to "New"
    and the type to "Command Widget".
*/

/* object_list_sd.parent = vc_sdd; */

                put_sd_value(
                  object_list_sd,
                  "parent",
                  vc_sdd
                );

/*
   Create at least one object to appear on the list.  We'll name this
   object "New" and give it an object type of command widget.
*/

                object_sd := create_sd_instance("object_sdd", "DM_BOX");

/* object_sd.name = "New"; */

                put_sd_value(
                  object_sd,
                  "name",
                  "New"
                );

/* object_sd.parent = vc_sdd; */

                put_sd_value(
                  object_sd,
                  "parent",
                  vc_sdd
                );

/* object_sd.type = command_widget_type; */

                put_sd_value(
                  object_sd,
                  "type",
                  command_widget_type
                );

              }
            )

#ifndef PROTOTYPE

            menu_item(
              subvcs_menu_item, 
              vc_form,
              7, 
              "Subview Controllers",
              {
                selected_command := create_sd_instance(
                  "command_sdd", 
                  "DE_BOX"
                );
/*
                command_sdd[selected_command].type = get_topmost_vcs_cmd;
*/
                put_sd_value(
                 selected_command, 
                 "type", 
                 get_topmost_vcs_cmd
                );

              }
            )

#else
            menu_item(
              subvcs_menu_item, 
              vc_form,
              7, 
              "Subview Controllers",
              {
/*
   Allow for the creation of a new vc list and save in a local variable
   so we can get rid of it later.
*/
                vc_list_sd := create_sd_instance("vc_list_sdd", "DM_BOX");
/*
    Create a new vc_sdd element and assign the name component to "New".
*/

/* vc_list_sd.parent = vc_sdd; */

                put_sd_value(
                  vc_list_sd,
                  "parent",
                  vc_sdd
                );

                vc_sd := create_sd_instance("vc_sdd", "DM_BOX");

/* vc_sd.name = "New"; */

                put_sd_value(
                  vc_sd,
                  "name",
                  "New"
                );

/* vc_sd.parent = vc_sdd; */

                put_sd_value(
                  vc_sd,
                  "parent",
                  vc_sdd
                );

              }
            )
#endif

            button(
              return_button,
              vc_form,
              20,
              subvcs_menu_item,
              50,
	      NULL,
              "Return",
              {
                display_vc := false;
/*
   Remove subvc list, if approriate.
*/
                if (vc_list_sd <> NULL) then
                  if (changes = 0) then
                    destroy_sd_instance(vc_list_sd);
                  else
		    display_are_you_sure :=  true;
                  endif;
                endif;
              }
            )
/*
   On Create increment the x and y positions for the next object.
*/
          On Create: {

            x_position := x_position + 20;
            y_position := y_position + 20;

            vc_form.horiz_distance := x_position;
            vc_form.vert_distance := y_position;

          }

          On Destroy: {

            if (object_list_sd <> NULL) then
              destroy_sd_instance(object_list_sd);
            end if;

          }

        end VC vc_vc			/* displays vc template		    */

      end VC vc_button_vc		/* created on new vc_sdd	    */

    end VC vc_list_vc			/* created on new vc_list_sdd	    */

  end VC dialogue_vc			/* created on new dialoge_sdd	    */
