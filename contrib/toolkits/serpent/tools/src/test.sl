#include "saw.ill"

|||

VARIABLES:

  hit_me: true;
  exit_me: false;

OBJECTS:

  the_window: form_widget {

    ATTRIBUTES:

      width: 250;
      height: 110;
      background_color: "white";

  }

vc: validate_hit

  creation condition: (hit_me)

OBJECTS:

  button_cc:  command_widget {

    ATTRIBUTES:

      parent:  the_window;
      width: 100;
      height: 40;
      background_color: "white";
      foreground_color: "black";
      from_vert: NULL;
      vert_distance: 10;
      from_horiz: NULL;
      horiz_distance: 75;
      label_text: "Hit me.";
      justify: 1;
      border_width: 2;
      border_color: "black";

    METHODS:

      notify: 
        {
          hit_me := false;
          exit_me := true;
        }
  }
end vc validate_hit

vc: exit_it

  creation condition: (exit_me)

  objects:

  button_cc:  command_widget {

    ATTRIBUTES:

      parent:  the_window;
      width: 100;
      height: 40;
      background_color: "white";
      foreground_color: "black";
      from_vert: NULL;
      vert_distance: 10;
      from_horiz: NULL;
      horiz_distance: 75;
      label_text: "Quit";
      justify: 1;
      border_width: 2;
      border_color: "black";

    METHODS:

      notify: 
        {
          exit ();
        }
    }


    display:  label_widget {

      ATTRIBUTES:

        parent: the_window;
        background_color: "white";
        foreground_color: "black";
        from_vert: NULL;
        from_horiz: NULL;
        vert_distance: 80;
        label_text: "Serpent tested successfully.";
        justify: 1;

    }

end vc exit_it

