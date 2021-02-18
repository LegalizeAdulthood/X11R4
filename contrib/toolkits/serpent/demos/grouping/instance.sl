/*--------------------------------------------------------------------------*\
|   
|  Name:  instance.sl
|
|  Description:  
|     This file contains the slang code which defines the individual
|       command widget instances and their actions
|
|  Component of:
|     grouping - grouping functions demonstrations
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
|     Sept 6, 1989 : Module created.
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
|  Name: instance_vc
|
|  Description:
|    The instance_vc instantiates on a new instance_sdd in 
|    the shared data base as long as a postion has been selected
\*--------------------------------------------------------------------------*/
VC: instance_vc

  Creation Condition:  (new("instance_sdd"))

  Variables:

     base_x: { if (instance_sdd.group_id = NULL) then
                 base_x := 0;
               else if (get_sd_value (instance_sdd.group_id,
                         "exist")) then
                 base_x := get_sd_value(
                                 instance_sdd.group_id,
                                 "x_offset");
               end if;
             }

     base_y: { if (instance_sdd.group_id = NULL) then
                 base_y := 0;
               else if (get_sd_value (instance_sdd.group_id,
                         "exist")) then
                 base_y := get_sd_value(
                                 instance_sdd.group_id,
                                 "y_offset");
               end if;
             }

     local_offset_x;
     local_offset_y;

     toggle_color: { if (instance_sdd.group_id <> NULL) then
                       toggle_color := get_sd_value(
                            instance_sdd.group_id,
                            "group_color");
                     else
                       toggle_color := "white";
                     end if;}

     temp_x;
     temp_y;

     Objects:

       component_instance: command_widget {
         Attributes:
           parent: main_background;
           x: local_offset_x + base_x;
           y: local_offset_y + base_y;
           horiz_distance: x;
           vert_distance: y;
           width: 70;
           height: 40;
           background_color: toggle_color;
           foreground_color: "black";
  	   label_text: instance_sdd.instance_name;
	   justify: CENTER;
           allow_user_move: TRUE;
           allow_user_resize: FALSE;

         Methods:

          notify: {
            if (current_command = "delete_instance") then
             destroy_sd_instance( 
               get_bound_sd_instance(
                instance_vc));
            else if (current_command = "add_instance_to_group") then
               instance_sdd.group_id := current_id;

               /* now we must calculate the adjustment for the base*/

               temp_x := base_x;
               base_x := get_component_value (current_id, "x_offset");
               local_offset_x := local_offset_x - base_x + temp_x;

               temp_y := base_y;
               base_y := get_component_value (current_id, "y_offset");
               local_offset_y := local_offset_y - base_y + temp_y;

            else if (current_command = "delete_from_group") then

               instance_sdd.group_id := NULL;               
               base_x := 0;
               local_offset_x := component_instance.x;
               base_y := 0;
               local_offset_y := component_instance.y;

            end if;
           }
          move: {
            temp_x := x - (local_offset_x + base_x);
            temp_y := y - (local_offset_y + base_y);

            if (instance_sdd.group_id = NULL) then
                base_x := base_x + temp_x;
                base_y := base_y + temp_y;
            else
                put_component_value(instance_sdd.group_id,
                  "x_offset",
                  get_component_value (instance_sdd.group_id,
                   "x_offset") + temp_x);

                put_component_value(instance_sdd.group_id,
                  "y_offset",
                  get_component_value (instance_sdd.group_id,
                   "y_offset") + temp_y);
            end if;
           }
	}


  On Create: {
       local_offset_x := init_x;
       local_offset_y := init_y;
   }

  end VC instance_vc;

