/*--------------------------------------------------------------------------*\
|   
|  Name:  Line
|
|  Description:  
|     This file contains the slang code which defines the actions
|      when a line is created
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
|     July 12, 1989 : Module created.
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
|  Name: line_vc
|
|  Description:
|    The line_vc instantiates on a new line_sdd in 
|    the shared data base. 
\*--------------------------------------------------------------------------*/
VC: line_vc

  Creation Condition:  (new("line_sdd") & ((line_sdd.start_x + 
                   line_sdd.start_y + line_sdd.end_x + line_sdd.end_y) > 0))

  Variables:

     line_length: (line_sdd.end_x - line_sdd.start_x) / 2;
     height_difference: line_sdd.end_y - line_sdd.start_y;

VC: actual_line_vc

   Creation Condition: (line_length > 0)
     Objects:

       line_1: label_widget {
         Attributes:
           parent: main_background;
           horiz_distance: line_sdd.start_x;
           vert_distance: line_sdd.start_y;
           x: horiz_distance;
           y: vert_distance;
           width: line_length;
           height: 1;
           background_color: BODY_COLOR;
           foreground_color: BODY_COLOR;
  	   label_text: " ";
	}

       line_2: label_widget {
         Attributes:
           parent: main_background;
           horiz_distance: line_sdd.start_x + line_length;
           vert_distance: {if (height_difference > 0) then
                            vert_distance := line_sdd.start_y;
                           else
                            vert_distance := line_sdd.end_y;
                           end if;
                           }
           x: horiz_distance;
           y: vert_distance;
           width: 1;
           height: { if (height_difference > 0) then
                       height := height_difference;
                     else if (height_difference < 0) then
                       height := -height_difference;
                     else
                       height := 1;
                     end if;
                   }
           background_color: BODY_COLOR;
           foreground_color: BODY_COLOR;
  	   label_text: " ";
	}

       line_3: label_widget {
         Attributes:
           parent: main_background;
           horiz_distance: line_sdd.start_x + line_length;
           vert_distance: line_sdd.end_y;
           x: horiz_distance;
           y: vert_distance;
           width: line_length;
           height: 1;
           background_color: BODY_COLOR;
           foreground_color: BODY_COLOR;
  	   label_text: " ";
	}

   end VC actual_line_vc;

  end VC line_vc;

