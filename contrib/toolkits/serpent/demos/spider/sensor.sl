  VC:  sensor

    /*
    ** Sub-VC to "dog":
    **   The sensor VC controls the creation of each sensor, including the
    **   lines from the sensor to the correlation centers.
    */

    CREATION CONDITION: (new ("sensor_sdd"))

#   define sensor_parent the_window

    VARIABLES:

      sensor_yloc;	/* set during ON CREATE */
      cc_line_hook_y:	sensor_yloc + (SENSOR_HEIGHT / 2);
      display_dialog:	FALSE;

    OBJECTS:

      right_sensor:  command_widget

      {

      ATTRIBUTES:

        /* core stuff */

        parent:			sensor_parent;

        width:			SENSOR_WIDTH;
        height:			SENSOR_HEIGHT;

        border_width:		3;
        border_color:		"white";

        background_color:
	{
	  IF (sensor_sdd.status = GREEN_STATUS) THEN
		background_color := "green";
          ELSE IF (sensor_sdd.status = YELLOW_STATUS) THEN
		background_color := "yellow";
          ELSE IF (sensor_sdd.status = RED_STATUS) THEN
		background_color := "red";
          ENDIF;
	}


        foreground_color:
	{
          IF (sensor_sdd.status = RED_STATUS) THEN
		foreground_color := "white";
          ELSE
		foreground_color := "black";
          ENDIF;
	}

	vert_distance:		sensor_yloc;
	horiz_distance:		RIGHT_SENSOR_HOOK;

        label_text:		sensor_sdd.site_abbr;
        justify:		CENTER;
        internal_width:		5;
        internal_height:	5;

      METHODS:

	notify:
	{
	  /*
	  ** Set display_dialog to true to force the display of a dialog box.
	  */
	  display_dialog := TRUE;
	}

      }

      left_sensor: command_widget

      {

      ATTRIBUTES:


        parent:			sensor_parent;

        width:			SENSOR_WIDTH;
        height:			SENSOR_HEIGHT;

        border_width:		3;
        border_color:		"white";

        background_color:
	{
	  IF (sensor_sdd.status = GREEN_STATUS) THEN
		background_color := "green";

          ELSE IF (sensor_sdd.status = YELLOW_STATUS) THEN
		background_color := "yellow";

          ELSE IF (sensor_sdd.status = RED_STATUS) THEN
		background_color := "red";

       /* ELSE do nothing */

          ENDIF;
	}


        foreground_color:
	{
          IF (sensor_sdd.status = RED_STATUS) THEN
            foreground_color := "white";
          ELSE
            foreground_color := "black";
          ENDIF;
	}

	vert_distance:		sensor_yloc;
	horiz_distance:		SENSOR_TO_SIDE;

        label_text:		sensor_sdd.site_abbr;
        justify:		CENTER;
        internal_width:		5;
        internal_height:	5;

      METHODS:

	notify:
	{
	  /*
	  ** Set display_dialog to true to force the display of a dialog box.
	  */
	  display_dialog := TRUE;
	}
      }

      ON CREATE:
      {
	/*
	** Increment the sensor number (located in the main VC, "dog").
	** Using that number, determine the Y location of the sensor that
	** caused the instantiation of this VC.
	*/
	sensor_number := sensor_number + 1;
	sensor_yloc   := SENSOR_TO_TOP + 
		   ( (sensor_number - 1) * (SENSOR_HEIGHT + SENSOR_TO_SENSOR));
      }

#include "etro.sl"
#include "dialog.sl"

  ENDVC  sensor
