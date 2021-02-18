/*--------------------------------------------------------------------------*\
|  Name: object_list_vc
|
|  Description:
|    Displays the lists of object defined for a given VC.  Instantiates
|    on new object list shared data element.
\*--------------------------------------------------------------------------*/
VC: object_list_vc

  Creation condition: (new("object_list_sdd"))

  Variables:

    object_list_length: 0;

    parental_unit: object_list_sdd.parent;

  Objects:

    object_list_form: form_widget {
      Attributes:
        parent: background;
        width: 151;
        height: 82 + (20 * object_list_length); 
        allow_user_move: true;
/* ?? next four  attributes can come out once the compiler bug is fixed */
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

    dual_title_bar(		/* object title object			    */
      object_title,  
      object_list_form,
      "Object List:",
      80,
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
      70
    )			/* end VC title object			    */
/*
   Cancel button causes the application to quit out of the dialogue
   and the dialogue box to go away.
*/
    button(			/* cancel button object			    */
      return_button,
      object_list_form,
      40 + (20 * object_list_length),
      NULL,
      50,
      NULL,
      "Return",
      {
/*
   Destory the object list shared data element that causes this vc to be 
   created.
*/
        delete_element(get_bound_sd_instance(get_parent_vc(SELF)));
      }
    )			/* end cancel button object		    */
/*
   On Create increment the x and y positions for the next object.
*/
  On Create: {

    x_position := x_position + 20;
    y_position := y_position + 20;

    object_list_form.horiz_distance := x_position;
    object_list_form.vert_distance := y_position;

  }						/* end actions on creat	    */

/*--------------------------------------------------------------------------*\
|  Name: object_button_vc
|
|  Description:
|    For each new subobject that goes on this object list add a button.
\*--------------------------------------------------------------------------*/
  VC: object_button_vc

    Creation condition: 

      ((new("object_sdd")) AND (object_sdd.parent = parental_unit))

    Variables:
      display_sot_types: false;
      display_sot: false;
      object_sdd_type: object_sdd.type;
      list_position;

    Objects:

      menu_item(
        object_menu_item, 
        get_object(get_grandparent(SELF), "object_list_form"), 
        get_one_up(object_list_length),
        (
          get_sd_value(
            get_bound_sd_instance(get_parent_vc(SELF)),
            "name"
          )
        ),
        {
          display_sot_types := true;
        }
      )

    On Create: {
      object_list_length := object_list_length + 1;
      list_position := object_list_length;
    }
/*
   Make sure the bound shared data instance is destroyed so it
   is not hanging around getting ready to screw things up the
   next time this vc's parent is instantiated.
*/
    On Destroy: {
      delete_element(get_bound_sd_instance(SELF));
    }

/*--------------------------------------------------------------------------*\
|  Name: display_sot_types_vc
|
|  Description:
|    The display_sot_types_vc controls the display of a menu
|    containing a list of all the object types available for the saw
|    technology.
\*--------------------------------------------------------------------------*/
VC: display_sot_types_vc

  Creation Condition : (display_sot_types)

  Variables:

    object_type_list_length: 0;

  Objects:

    sot_types_form: form_widget {

      Attributes:
        parent: background;
        width: 152;
        height: 82 + (20 * object_type_list_length);
        allow_user_move: true;
        horiz_distance: 200;
        vert_distance: 200;
        x: 0;
        y: 0;
      Methods:
        move: {
          horiz_distance := x;
          vert_distance := y;
        }
    }                 /* end sot_types_form object          */

    command_widget_title: label_widget {
      Attributes:
        parent: sot_types_form;
        border_width: 0;
        background_color: "black";
        foreground_color: "white";
        height: 18;
        label_text: "Athena Widget Types";
        width: 150;
        horiz_distance: 1;
        vert_distance: 1;
        justify: LEFT;
    }


    button(                 /* cancel button object                     */
      cancel_button,
      sot_types_form,
      40 + (20 * object_type_list_length),
      NULL,
      50,
      NULL,
      "Cancel",
      {
        display_sot_types := false;
      }
    )                       /* end cancel button object                 */

/*--------------------------------------------------------------------------*\
|  Name: display_sot_type_vc
|
|  Description:
|    The display_sot_type_vc controls the display of
|     each menu item on the saw object template types menu.  This menu
|     goes away after anything is selected.
\*--------------------------------------------------------------------------*/
  VC: display_sot_type_vc

    Creation Condition: (new("object_type_sdd"))

    Variables:
      object_instance_sdi;	/* id of created object instance	*/

    Objects:

      menu_item(
       widget_menu_item,
       sot_types_form,
       object_type_sdd.number,
       object_type_sdd.name,
       { 
         display_sot := true;
         display_sot_types := false;
       }
    )

    On Create: {
      object_type_list_length := object_type_list_length + 1;
    }

  end VC display_sot_type_vc;

end VC display_sot_types_vc;

/*--------------------------------------------------------------------------*\
|  Name: object_vc
|
|  Description:
|    Displays a command widget object template.
\*--------------------------------------------------------------------------*/
    VC: object_vc

      Creation condition:
        (display_sot AND (object_sdd_type = command_widget_type))

      Variables:

        object_list_sd: NULL;
        display_are_you_sure: false;
        changes: 0;	!* the number of changes made below this object *!
        send_flag: 0;
/*
   Varibles in which to maintain ids of shared data elements used to edit
   the various attribute snippets.
*/
        parent_text_id;

      Objects:

        object_form: form_widget {
          Attributes:
            parent: background;
            width: 151;		/* width + 2 * border_width	    */
            height: 242;
            allow_user_move: true;
/* ?? next four  attributes can come out once the compiler bug is fixed */
            horiz_distance: 0;
            vert_distance: 0;
            x: 0;
            y: 0;
          Methods:
            move: {
              horiz_distance := x;
              vert_distance := y;
            }
        }				/* end VC form object		    */

        editable_title_bar(
          object_form_title,  
          object_form,
          "Command Widget:",
          100,
          object_sdd.name,
          50
        )

/*
   ???  Each of these menu items is going to need to be instantiated
   in a separate vc with a cc of new("object_type_sdd").  This vc 
   is going to need a subvc which instantiates on a local variable
   and controls the creation/destruction of a shared data element
   which causes the display_text_vc to be invoked.
*/

        menu_item(
          parent_menu_item, 
          object_form,
          1, 
          "Parent",
          {
            parent_text_id := create_sd_instance("text_sdd", "DM_BOX");
            put_sd_value(parent_text_id, "title", "Parent Attribute:");
            put_sd_value(parent_text_id, "title_length", 16);
            put_sd_value(parent_text_id, "parent", "New");
            put_sd_value(
              parent_text_id, 
              "text", 
              ""
            );
          }
        )

        menu_item(
          label_text_menu_item, 
          object_form,
          2, 
          "Label Text",
          { 
          }
        )

        menu_item(
          x_menu_item, 
          object_form,
          3, 
          "X",
          { 
          }
        )

        menu_item(
          y_menu_item, 
          object_form,
          4, 
          "Y",
          { 
          }
        )

        menu_item(
          width_menu_item, 
          object_form,
          5, 
          "Width",
          { 
          }
        )

        menu_item(
          height_menu_item, 
          object_form,
          6, 
          "Height",
          { 
          }
        )

        menu_item(
          foreground_color_menu_item, 
          object_form,
          7, 
          "Foreground Color",
          { 
          }
        )

        menu_item(
          background_color_menu_item, 
          object_form,
          8, 
          "Background Color",
          { 
          }
        )

        button(			/* cancel button object		    */
          return_button,
          object_form,
          200,
          NULL,
          50,
          NULL,
          "Return",
          {
            display_sot := false;
          }
        )				/* end cancel button object	    */
/*
   On Create increment the x and y positions for the next object.
*/
          On Create: {

            x_position := x_position + 20;
            y_position := y_position + 20;

            object_form.horiz_distance := x_position;
            object_form.vert_distance := y_position;

          }

    end VC object_vc		/* displays object template    */

  end VC object_button_vc		/* created on new object_sdd	    */

end VC object_list_vc
