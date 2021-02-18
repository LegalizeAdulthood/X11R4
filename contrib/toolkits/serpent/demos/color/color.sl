#include "saw.ill"

|||

   OBJECTS:

     the_window: form_widget
     {

     ATTRIBUTES:

      width: 640;
      height: 645;

      background_color: "black";
      }


     the_cc:  command_widget

     {

     ATTRIBUTES:

       parent:  the_window;
       width: 60;
       height: 40;
       background_color: "green";
       foreground_color: "black";

       vert_distance: 250;
       horiz_distance: 310;

       label_text: "CMC";
       justify: 1;

       border_width:  3;
       border_color:  "white";

    METHODS:

      notify:
        {
         IF (the_cc.background_color = "green")
         THEN
            the_cc.background_color := "red";
         ELSE
            the_cc.background_color := "green";
         ENDIF;
        }
      }


