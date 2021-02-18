  VC: sensor_line /* sub to dog */

    /*
    ** Sub-VC to main vc:
    **   The sensor_line VC controls the display of the communication lines
    **   from a sensor to the correlation centers.
    */

    CREATION CONDITION: (new ("communication_line_sdd"))

    VARIABLES:

      comm_line_sd:	    get_bound_sd_instance (get_parent_vc (SELF));
      sensor_sd:	    get_component_value (comm_line_sd, "from_sensor");
      /* Get the id of the VC associated with the "from" sensor. */
      sensor_vc:	    get_vc_id (sensor_sd, "sensor");

      /* Can't reference the sensor_vc variables directly, since we're
      ** working with an id, not a static variable.
      */
      sensor_line_yloc:     get_variable_value (sensor_vc,
				"sensor_yloc") + (SENSOR_HEIGHT / 2);
      left_sensor_object:   get_object (sensor_vc, "left_sensor");
      right_sensor_object:  get_object (sensor_vc, "right_sensor");

      line_height:	    sensor_line_yloc - TOP_TO_MID_CC;

      line_status: {if (communication_line_sdd.status = GREEN_STATUS) then
                       line_status := "green";
                    else if (communication_line_sdd.status = YELLOW_STATUS) then
                       line_status := "yellow";
                    else
                       line_status := "red";
                    end if;}

      x_pos: { if (communication_line_sdd.to_cc = CMC_CODE) then
                   x_pos := LEFT_SENSOR_HOOK + 6;
               else
                   x_pos := RIGHT_CC_SENSOR_HOOK + CC_LINE_STUB + 2;
               end if;}

    OBJECTS:

      non_selectable_line(line_widget,
                          the_window,
                          x_pos,
                          sensor_line_yloc,
                          LEFT_CC_SENSOR_HOOK - LEFT_SENSOR_HOOK - CC_LINE_STUB - 5,
                          "x",
                          line_status)

  ENDVC  sensor_line
