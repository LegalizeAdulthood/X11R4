#include "saw.ill"

|||

/*----------------------------------------------------------------------------
|  This demonstrates the use of Slang to produce a menu bar with two tear off
|   menus.  Initially there is a menu bar presented to the user with two
|   options - Colors and Sizes.  When the user chooses one of those options
|   another menu comes up, the user must position this menu and it stays up
|   until the user chooses "Remove" on that menu.
|------------------------------------------------------------------------*/

Variables:

  display_colors_submenu : FALSE;
  display_sizes_submenu : FALSE;

Objects:

  menu_bar_form: form_widget
  {
    Attributes:

    height:500;
    width: 500;
  }
  colors_menu_item: command_widget
   {
    Attributes:
      parent        : menu_bar_form;
      horiz_distance: 0;
      vert_distance : 0;

      height        : 20;
      width         : 50;
      label_text          : "Colors";

    Methods:

      notify:
      {
        display_colors_submenu := TRUE;
      }
   }

  sizes_menu_item: command_widget
   {
    Attributes:

      parent        : menu_bar_form;

      horiz_distance: 50;
      vert_distance : 0;

      height        : 20;
      width         : 50;

      label_text          : "Sizes";

    Methods:

      notify:
      {
        display_sizes_submenu := TRUE;
      }
    }

/*-------- colors_submenu view controller ----------------*/

VC:  colors_submenu

Creation Condition:  (display_colors_submenu)

Objects:

  color_menu_form: form_widget {
    Attributes:
      parent: menu_bar_form;

      horiz_distance: 0;
      vert_distance : 0;

      height: 100;
      width: 50;

}

  blue_menu_item: command_widget
  {
    Attributes:
    
      parent        : color_menu_form;
      horiz_distance: 0;
      vert_distance : 0;

      height: 20;
      width: 50;

      label_text: "Blue";
      background_color: "blue";
      foreground_color: "white";

    Methods:
/*   This method would be invoked when the end-user selected the blue
     button
      notify:{ code would be placed here to set something to blue
     }*/
    }

  red_menu_item: command_widget 
  {
    Attributes:
      parent        : color_menu_form;

      horiz_distance: 0;
      vert_distance : 25;

      height: 20;
      width: 50;

      label_text: "Red";
      background_color: "red";
      foreground_color: "white";
    Methods:
/*   This method would be invoked when the end-user selected the red
     button
      notify:{ code would be placed here to set something to red
     }*/
   }

  remove_menu_item: command_widget
   {

    Attributes:
      parent        : color_menu_form;
      horiz_distance: 0;
      vert_distance : 60;

      height: 20;
      width: 50;

      label_text: "Remove";

    Methods:

      notify:
      {
        display_colors_submenu := FALSE;
      }
   }

  ENDVC colors_submenu

/*-------- sizes_submenu view controller -----------------
|    the strucuture of this menu is the same as the structure
|    of the color sub-menu and so it is omitted
|--------------------------------------------------------*/
 
