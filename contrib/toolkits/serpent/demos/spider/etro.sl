
    VC: etro

      /*
      ** ETRO - Estimated Time to Return of Operation
      **
      ** This VC controls the creation of the ETRO box that shows up next to
      ** a sensor when that sensor's status is not green.
      */
      CREATION CONDITION:	((sensor_sdd.status = YELLOW_STATUS) OR
				 (sensor_sdd.status = RED_STATUS))

#     define etro_parent  the_window

      OBJECTS:

	left_etro: label_widget

	{

	ATTRIBUTES:

	  parent:		etro_parent;

	  width:		SENSOR_WIDTH;
	  height:		SENSOR_HEIGHT + 6;
	  background_color:     "white";

	  foreground_color:     "black";
	  vert_distance:	sensor_yloc;
	  horiz_distance:	SENSOR_TO_SIDE - SENSOR_WIDTH - 2;


	  label_text:		sensor_sdd.etro;
	  justify:		CENTER;
	  internal_width:	5;
	  internal_height:	5;
	}

	right_etro: label_widget

	{

	ATTRIBUTES:

	  parent:               etro_parent;

          width:                SENSOR_WIDTH;
          height:               SENSOR_HEIGHT + 6;
          background_color:     "white";
          foreground_color:     "black";

          vert_distance:        sensor_yloc;
	  horiz_distance:	XMAX - SENSOR_TO_SIDE + 5;

          label_text:           sensor_sdd.etro;
          justify:              CENTER;
          internal_width:       5;
          internal_height:      5;
	}

    ENDVC  etro
