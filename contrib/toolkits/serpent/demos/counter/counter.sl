#include "saw.ill"

|||

/*----------------------------------------------------------------------
|   This demo puts two objects on the screen.  One is a selectable button
|    which is labelled "push".  The other shows how many times the button
|    has been pushed.  This demonstrates the automatic propogation of
|    dependencies within Serpent
|-----------------------------------------------------------------------*/

   VARIABLES:
     counter : 0;

   OBJECTS:
     background: form_widget
     { ATTRIBUTES:
      width: 640;
      height: 645; }


     display:  label_widget

     { ATTRIBUTES:

       parent:  background;
       width: 60;
       height: 40;
       vert_distance: 150;
       horiz_distance: 310;
       label_text: counter;
       justify: 1; }

     push_button:  command_widget

     { ATTRIBUTES:

       parent:  background;

       width: 60;
       height: 40;
       vert_distance: 250;
       horiz_distance: 310;
       label_text: "PUSH";
       justify: 1;

    METHODS:

      notify:
        {
         counter := counter + 1;
        }
    }

