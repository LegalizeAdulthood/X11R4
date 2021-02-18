/*--------------------------------------------------------------------------*\
|   
|  Name:  Arg
|
|  Description:  
|     This file contains the slang code which defines the actions
|      for input arguments
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
|  Name: argument_vc
|
|  Description:
|    The argument_vc instantiates on a new arg_sdd in 
|    the shared data base. 
\*--------------------------------------------------------------------------*/
VC: argument_vc

  Creation Condition:  (new("arg_sdd") and arg_sdd.create)

  Variables:

     output_line_id: NULL;
     form_x;
     form_y;

     local_debug: {if (debug or output_line_id = NULL) then
                       local_debug := TRUE;
                     else
                       local_debug := FALSE;
                     end if;
                     }

     Objects:

       output_instance: command_widget {
         Attributes:
           parent: form_background;
           x: 142;
           y: {if (local_debug) then
                 y := 5;
               else
                 y := 10;
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
           label_text: { if (arg_sdd.value = UNDEFINED) then
                             label_text := "   ";
                         else
                             label_text := arg_sdd.value;
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
                   value_id := get_creating_sd(argument_vc);
                   source_x := form_background.horiz_distance +
                       output_instance.x + output_instance.width;
                   source_y := form_background.vert_distance +
                        output_instance.y + output_instance.height / 2;
                   message := "Select target for value";
                end if;
             else if (current_command = "disconnect" and 
                id_exists (output_line_id)) then
                
                /* delete line output_line_id */

                destroy_sd_instance(output_line_id);
             else if (current_command = "delete") then
                if (id_exists (output_line_id)) then
                  destroy_sd_instance(output_line_id);
                end if;
                destroy_sd_instance(get_creating_sd(argument_vc));

             end if;
          }
     }


       form_background: form_widget{
         Attributes:
           parent: main_background;
           horiz_distance: form_x;
           vert_distance: form_y;
           height: 22;
           width: 186;
           allow_user_move:  TRUE;
           x: horiz_distance;
           y: vert_distance;
           border_width: 0;

         Methods:

           move: {

             horiz_distance := x;
             vert_distance := y;

             if ( id_exists (output_line_id)) then
                   source_x := form_background.horiz_distance +
                       output_instance.x + output_instance.width;
                   source_y := form_background.vert_distance +
                        output_instance.y + output_instance.height / 2;
                   set_line_start_values(output_line_id,source_x,source_y)
              end if;
          }
         }

       arg_position: text_widget {
         Attributes:
           parent: form_background;
           horiz_distance: 0;
           vert_distance: 0;
           width: 140;
           height: 20;
           background_color: BACKGROUND_COLOR;
           foreground_color: BODY_COLOR;
           text_buffer: arg_sdd.value;
           edit_type: 2;
           send_buffer: send_flag;

        Methods:

         send: {
           arg_sdd.value := text_buffer;
           send_flag := 0;
          }
        }

  On Create: {
       message := "Move to desired position";
       form_x := init_x;
       form_y := init_y;
   }

  end VC argument_vc;

