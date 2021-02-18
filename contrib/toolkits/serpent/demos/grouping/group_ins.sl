/*--------------------------------------------------------------------------*\
|   
|  Name:  group_ins.sl
|
|  Description:  
|     This file contains the slang code which defines the 
|       group widget instances and their actions
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
|  Name: group_instance_vc
|
|  Description:
|    The group_instance_vc instantiates on a new group_sdd in 
|    the shared data base
\*--------------------------------------------------------------------------*/
VC: group_instance_vc

  Creation Condition:  (new("group_sdd"))

  Variables:

     sdd_id;
     y_offset;

     Objects:

       component_instance: command_widget {
         Attributes:
           parent: main_background;
           x: group_menu_x;
           y: y_offset;
           horiz_distance: x;
           vert_distance: y;
           width: 70;
           height: 30;
           background_color: "white";
           foreground_color: "black";
  	   label_text: group_sdd.group_name;
	   justify: CENTER;
           allow_user_move: FALSE;
           allow_user_resize: FALSE;
           sensitive: group_sdd.exist;

         Methods:

          notify: {

           if (current_command = "delete_group") then
            put_component_value(
              sdd_id,
              "exist",
              FALSE);
           else if (current_command = "add_instance_to_group") then
              current_id :=  get_bound_sd_instance(group_instance_vc);
           else if (current_command = "toggle_group") then
                if ( group_sdd.group_color = "white") then
                    group_sdd.group_color := "black";
                else 
                    group_sdd.group_color := "white";
                end if;
           end if;
           }
	}


  On Create: {
       y_offset := group_y_offset;
       group_y_offset := group_y_offset + 35;
       sdd_id := current_id;
   }

  end VC instance_vc;

