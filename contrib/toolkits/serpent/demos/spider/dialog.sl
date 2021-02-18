    VC:	dialog
  
      /*
      ** Sub-VC to "sensor":
      **   The dialog VC controls the creation of dialog boxes, which allow the
      **   user to change various sensor attributes.  A dialog box is created
      **   when a sensor is selected.
      */
  
      CREATION CONDITION:	(display_dialog)
  
#     define LABEL_HEIGHT	(25)
#     define LABEL_WIDTH	(75)
  
      VARIABLES:
  
        count:		2;
  
      OBJECTS:
  
        the_dialog_form:	form_widget
  
        {
  
        ATTRIBUTES:
  
  	parent:		     the_window;
  	width:		     LABEL_WIDTH * 2 + 4;
  	height:		     (4 * LABEL_HEIGHT) + SENSOR_HEIGHT + 7;
  	background_color:    "white";
  	border_width:	     2;
  	border_color:	     "white";

  	vert_distance:	     TOP_TO_MID_CC + (CC_HEIGHT / 2) + 20;
  	horiz_distance:	     SIDE_TO_MIDDLE - LABEL_WIDTH;

  
        } /* end the_dialog_form */
  
        name_label:	label_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH * 2;
  	  height:		LABEL_HEIGHT - 2;
  	  background_color:	"white";
  
  	  foreground_color:	"black";
  
  	  label_text:		sensor_sdd.site;
  	  justify:		CENTER;

          border_width: 0;  
          horiz_distance: 0;

        } /* end name_label */
  
        status_label:	label_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH;
  	  height:		LABEL_HEIGHT;
  	  background_color:	"white";
  	  vert_distance:	LABEL_HEIGHT;
  	  horiz_distance:	0;

  	  foreground_color:	"black";
  
  	  label_text:		"STATUS";
  	  justify:		LEFT;
  
        } /* end status_label */
  
        status_text:	text_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH;
  	  height:		LABEL_HEIGHT;
  	  background_color:	"white";

  	  vert_distance:	LABEL_HEIGHT;
  	  horiz_distance:	LABEL_WIDTH;
  
  	  foreground_color:	"black";
  	  editable:		1;
  	  edit_type:		2;
  	  max_length:		50;
  
  	  text_buffer:
  	  {
  	    IF (sensor_sdd.status = GREEN_STATUS) THEN
  	      text_buffer := "green";
  
  	    ELSE IF (sensor_sdd.status = YELLOW_STATUS) THEN
  	      text_buffer := "yellow";
  
  	    ELSE IF (sensor_sdd.status = RED_STATUS) THEN
  	      text_buffer := "red";
  
  	/*  ELSE do nothing */
  
  	    ENDIF;
  	  }
  
  	METHODS:
  
  	  send:
  	  {
  
  	      IF (text_buffer = "green") THEN
		sensor_sdd.status := GREEN_STATUS;
  
  	      ELSE IF (text_buffer = "yellow") THEN
		sensor_sdd.status := YELLOW_STATUS;
  
  	      ELSE IF (text_buffer = "red") THEN
		sensor_sdd.status := RED_STATUS;
  
  	  /*  ELSE do nothing */
  
  	      ENDIF;
  	      display_dialog := FALSE;
  	  }
  
        } /* end status_text */
  
        etro_label:  label_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH;
  	  height:		LABEL_HEIGHT;
  	  background_color:	"white";

  	  vert_distance:	2 * LABEL_HEIGHT;
  	  horiz_distance:	0;
  
  	  foreground_color:	"black";
  
  	  label_text:		"ETRO";
  	  justify:		LEFT;
  
        } /* end etro_label */
  
        etro_text:  text_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH;
  	  height:		LABEL_HEIGHT;
  	  background_color:	"white";

  	  vert_distance:	2 * LABEL_HEIGHT;
  	  horiz_distance:	LABEL_WIDTH;
  
  	  foreground_color:	"black";
  	  editable:		1;
  	  edit_type:		2;
  	  max_length:		50;
  
  	  text_buffer: sensor_sdd.etro;
  
  
  	METHODS:
  
  	  send:
  	  {
            sensor_sdd.etro := text_buffer;
  	  }
  
        } /* end etro_text */
  
        rfo_label: label_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH;
  	  height:		LABEL_HEIGHT;
  	  background_color:	"white";

  	  vert_distance:	3 * LABEL_HEIGHT;
  	  horiz_distance:	0;
  
  	  foreground_color:	"black";
  
  	  label_text:		"RFO";
  	  justify:		LEFT;
  
        } /* end rfo_label */
  
        rfo_text:	text_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		LABEL_WIDTH;
  	  height:		LABEL_HEIGHT;
  	  background_color:	"white";

  	  vert_distance:	3 * LABEL_HEIGHT;
  	  horiz_distance:	LABEL_WIDTH;
  
  	  foreground_color:	"black";
  	  editable:		1;
  	  edit_type:		0;
  
  	  text_buffer:		sensor_sdd.rfo;
  	  max_length:		50;
  
        } /* end rfo_text */
  
        ok_button: command_widget
  
        {
  
  	ATTRIBUTES:
  
  	  parent:		the_dialog_form;
  	  width:		SENSOR_WIDTH;
  	  height:		SENSOR_HEIGHT;
  	  background_color:	"white";

  	  foreground_color:	"black";
  
  	  vert_distance:	4 * LABEL_HEIGHT + 5;
  	  horiz_distance:	20;
  
  	  label_text:		"OK";
  
  	METHODS:
  
  	  notify:
  	  {
  	      status_text.send_buffer := 1;
  	      etro_text.send_buffer := 1;
  	  }
  
        } /* end ok_button */
  
    ENDVC  dialog
