/*  $Log:	slv.sl,v $
% Revision 1.1  89/10/10  22:47:56  serpent
% Initial revision
% 
Revision 1.1  89/08/07  16:53:24  trm
Initial revision
    */
/*---------------------------------------------------------------------*\
|     Macro: move end points for line into dm shared data
|
|     Usage: this macros is used when the end points for a line
|            are known.  It sets these values into dm shared data
|
|     Entry: set_line_values(fline_id,x1,y1,x2,y2)
|             args: fline_id: the id of the shared data element
|                   x1: the x value of the start of the line
|                   y1: the y value of the start of the line
|                   x2: the x value of the end of the line
|                   y2: the y value of the end of the line
|
|     Entry: set_start_line_values(fline_id,x1,y1)
|             args: fline_id: the id of the shared data element
|                   x1: the x value of the start of the line
|                   y1: the y value of the start of the line
|
|     Entry: set_end_line_values(fline_id,x2,y2)
|             args: fline_id: the id of the shared data element
|                   x1: the x value of the end of the line
|                   y1: the y value of the end of the line
\*--------------------------------------------------------------------*/
#define set_line_start_values(fline_id,x1,y1)\
\
   put_component_value(\
           fline_id,\
           "start_x",\
           x1\
         );\
\
   put_component_value(\
           fline_id,\
           "start_y",\
           y1\
         );

#define set_line_end_values(fline_id,x2,y2)\
\
   put_component_value(\
           fline_id,\
           "end_x",\
           x2\
         );\
\
   put_component_value(\
           fline_id,\
           "end_y",\
           y2\
         );

#define set_line_values(fline_id,x1,y1,x2,y2)\
set_line_start_values(fline_id,x1,y1)\
set_line_end_values(fline_id,x2,y2)
