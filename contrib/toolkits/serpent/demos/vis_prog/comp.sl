/*--------------------------------------------------------------------------*\
|   
|  Name:  Component
|
|  Description:  
|     This file contains the slang code which defines the individual
|       component actions
|
|  Component of:
|     vis_prog - visual programming demonstration.
|
|  Project:
|     Serpent
|     User Interface Project
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Len Bass
|     ljb@sei.cmu.edu  
|
|  Change History:
|     June 22, 1989 : Module created.
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: component_vc
|
|  Description:
|    The component_vc instantiates on a new component_sdd in 
|    the shared data base. 
\*--------------------------------------------------------------------------*/
VC: component_vc

  Creation Condition:  (new("component_sdd") and component_sdd.create)

  Variables:


     input1_line_id: NULL;	/* ids of lines in and out*/
     input2_line_id: NULL;	/* of this component */
     output_line_id: NULL;
     target_x;
     target_y;
     form_x;
     form_y;
     component_value:{ 
                if (component_sdd.input1_value_id <> NULL
                    and
                   component_sdd.input2_value_id <> NULL) then

               if (component_sdd.operator = "+") then

                   component_sdd.value := get_sd_value (
                        component_sdd.input1_value_id, "value")
                            +
                        get_sd_value (
                        component_sdd.input2_value_id, "value");
               end if;

               if (component_sdd.operator = "-") then

                   component_sdd.value := get_sd_value (
                        component_sdd.input1_value_id, "value")
                            -
                        get_sd_value (
                        component_sdd.input2_value_id, "value");
               end if;

               if (component_sdd.operator = "*") then

                   component_sdd.value := get_sd_value (
                        component_sdd.input1_value_id, "value")
                            *
                        get_sd_value (
                        component_sdd.input2_value_id, "value");
               end if;

               if (component_sdd.operator = "/") then

                   component_sdd.value := get_sd_value (
                        component_sdd.input1_value_id, "value")
                            /
                        get_sd_value (
                        component_sdd.input2_value_id, "value");

               end if;
             end if;}

       local_debug: {if (debug or output_line_id = NULL) then
                       local_debug := TRUE;
                     else
                       local_debug := FALSE;
                     end if;
                     }

     Objects:

       input1_instance: command_widget {
         Attributes:
           parent: component_form;
           x: 0;
           y: 10;
           horiz_distance: x;
           vert_distance: y;
           width: 20;
           height: 2;
           background_color: BODY_COLOR;
           foreground_color: BODY_COLOR;

        Methods:

           notify: {
             if (current_command = "connect") then
                if (line_id = NULL) then
		   message := "Must select source first";
                else 
                   input1_line_id := line_id;
                   component_sdd.input1_value_id := value_id;
                   target_x := component_form.horiz_distance;
                   target_y := component_form.vert_distance +
       			input1_instance.y;
                   set_line_values(line_id,source_x,source_y,target_x,
					target_y)
		   line_id := NULL;
                   message := "";
		end if;
	     else if (current_command = "disconnect" and 
                      id_exists (input1_line_id)) then

		/* delete line input1_id */
                destroy_sd_instance(input1_line_id);
             else
                target_x := 0;
             end if;
          }
        }

       input2_instance: command_widget {
         Attributes:
           parent: component_form;
           x: 0;
           y: 30;;
           horiz_distance: x;
           vert_distance: y;
           width: 20;
           height: 2;
           background_color: BODY_COLOR;
           foreground_color: BODY_COLOR;

        Methods:

           notify: {
             if (current_command = "connect") then
                if (line_id = NULL) then
		   message := "Must select source first";
                else 
                   input2_line_id := line_id;
                   component_sdd.input2_value_id := value_id;
                   target_x := component_form.horiz_distance;
                   target_y := component_form.vert_distance +
       			input2_instance.y;
                   set_line_values(line_id,source_x,source_y,target_x,
					target_y)
		   line_id := NULL;
                   message := "";
		end if;
	     else if (current_command = "disconnect" and 
                   id_exists (input2_line_id) ) then
		/* delete line input2_id */

                destroy_sd_instance(input2_line_id);
		target_x := 0;
             else
                target_x := 0;
             end if;
          }
        }

       output_instance: command_widget {
         Attributes:
           parent: component_form;
           x: 94;
           y: {if (local_debug) then
                 y := 15;
               else
                 y := 20;
               end if;
               }

           horiz_distance: x;
           vert_distance: y;
           width: 40;
           height: {if (local_debug) then
                 height := 10;
               else
                 height := 2;
               end if;
               }

           background_color: "black";
           foreground_color: {if (local_debug) then
                 foreground_color := "white";
               else
                 foreground_color := "black";
               end if;
               }

          label_text: { if (component_sdd.value = UNDEFINED) then
                            label_text := "   ";
                        else
                            label_text := component_sdd.value;
                        end if;
               }

        Methods:

           notify: {
             if (current_command = "connect") then
                if (line_id <> NULL) then
		   message := "Have already selected source, hit connect to reset";
                else 
		   line_id := create_sd_instance("line_sdd",
  				"DM_BOX");
                   set_line_values(line_id,0,0,0,0)
                   output_line_id := line_id;
                   value_id := get_creating_sd(component_vc);

                   source_x := component_form.horiz_distance + 
                       output_instance.x + output_instance.width - 2;
                   source_y := component_form.vert_distance +
       			output_instance.y + output_instance.height / 2;
                   message := "Select target for value";
		end if;
	     else if (current_command = "disconnect" and
                        id_exists (output_line_id)) then
		/* delete line output_line_id */

                destroy_sd_instance(output_line_id);
		target_x := 0;
                output_line_id := NULL;
             else
                target_x := 0;
             end if;
          }
     }

       component_form: form_widget {
         Attributes:
           parent: main_background;
           horiz_distance: form_x;
           vert_distance: form_y;
           allow_user_move: TRUE;
           width: 130;
           height: 42;
           border_width: 0;
 	   x: horiz_distance;
           y: vert_distance;

         Methods:

           move: {

             horiz_distance := x;
             vert_distance := y;

		/* we must move the position of the end points of affected
		  	lines */

             if ( id_exists (input1_line_id) ) then
                   target_x := x - 2;
                   target_y := y + input1_instance.y;
                   set_line_end_values(input1_line_id,target_x,target_y)
              end if;

             if ( id_exists (input2_line_id)) then
                   target_x := x - 2;
                   target_y := y + input2_instance.y;
                   set_line_end_values(input2_line_id,target_x,target_y)
              end if;

             if ( id_exists (output_line_id)) then
                   target_x := x + output_instance.x + output_instance.width - 2;
                   target_y := y + output_instance.y + output_instance.height / 2;
                   set_line_start_values(output_line_id,target_x,target_y)
              end if;
          }
       }


       component_instance: command_widget {
         Attributes:
           parent: component_form;
           x: 22;
           y: 0;
           horiz_distance: x;
           vert_distance: y;
           width: 70;
           height: 40;
           background_color: BACKGROUND_COLOR;
           foreground_color: BODY_COLOR;
  	   label_text: component_sdd.operator;
           font: NORMAL_FONT;
	   justify: CENTER;

         Methods:

          notify: {

            if (current_command = "delete") then
              /* first kill the lines, then kill the object*/

                if (id_exists(input1_line_id)) then
                     destroy_sd_instance(input1_line_id);
                  end if;

                if (id_exists(input2_line_id)) then
                     destroy_sd_instance(input2_line_id);
                  end if;

                if (id_exists(output_line_id)) then
                     destroy_sd_instance(output_line_id);
                  end if;

                destroy_sd_instance(get_creating_sd(component_vc));
             end if;
            }
	}


  On Create: {
       form_x := init_x;
       form_y := init_y;
   }

  end VC component_vc;

