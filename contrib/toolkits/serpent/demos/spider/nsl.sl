/*---------------------------------------------------------------------*\
|     Macro: construct non_selectable line
|
|     Usage: this macro is used whenever a line which is not
|	     selectable is to be created.  It creates a label
|            widget of a small width.
|
|     Entry: non_selectable_line(flabel,fparent,x_pos,y_pos,length,color)
|             args: flabel: label for label widget
|                   fparent: parent for label widget
|                   x_pos: x position of widget
|                   y_pos: y position of widget
|                   length: length of the line to be created
|                   orient: "x" if line is parallel to x axis
|                                "y" if line is parallel to y axis
|                   fcolor: color of line
|
\*--------------------------------------------------------------------*/
#define non_selectable_line(flabel,fparent,x_pos,y_pos,length,orient,fcolor)\
       flabel: label_widget {\
         Attributes:\
           parent: fparent;\
           horiz_distance: x_pos;\
           vert_distance: y_pos;\
           x: horiz_distance;\
           y: vert_distance;\
           width: {if (orient = "x") then\
                    width := length;\
                   else\
                    width := 1;\
                   end if;}\
           height: {if (orient = "y") then\
                    height := length;\
                   else\
                    height := 1;\
                   end if;}\
           background_color: fcolor;\
           foreground_color: fcolor;\
           label_text: " ";\
        }
