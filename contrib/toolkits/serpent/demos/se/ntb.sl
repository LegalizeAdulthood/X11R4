/* $Log:	ntb.sl,v $
% Revision 1.1  89/10/10  22:17:04  serpent
% Initial revision
% 
Revision 1.2  89/06/19  16:20:37  ljb
fix up rcs header 
j.
  */
/*  this file defines the title of a table abstraction for inclusion in a
    slang program.  The entry points to the abstraction:

    table_define   which creates an empty table
    title_bar    which adds entries to the table
*/
#define non_editable_bar(fname, fparent, flabel, fwidth1, fvalue, fwidth2)\
fname/*filler*/_label: label_widget {\
 Attributes:\
   parent: fparent;\
   border_width: 0;\
   background_color: "black";\
   foreground_color: "white";\
   height: 20;\
   label_text: flabel;\
   width: fwidth1;\
   horiz_distance: 1;\
   vert_distance: 1;\
   justify: LEFT;\
}\
fname/*filler*/_text: label_widget {\
 Attributes:\
   parent: fparent;\
   border_width: 0;\
   background_color: "black";\
   foreground_color: "white";\
   width: fwidth2;\
   height: 20;\
   label_text: fvalue;\
   vert_distance: 1;\
   horiz_distance: fwidth1;\
}

   
