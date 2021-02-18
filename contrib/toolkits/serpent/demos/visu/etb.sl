/*  this file defines the table abstraction for inclusion in a
    slang program.  There are three entry points to the abstraction:

    table_define   which creates an empty table
    title_bar    which adds entries to the table
*/
#define editable_title_bar(fname, fparent, flabel, fwidth1, fvalue, fwidth2)\
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
fname/*filler*/_text: text_widget {\
 Attributes:\
   parent: fparent;\
   border_width: 0;\
   background_color: "black";\
   foreground_color: "white";\
   width: fwidth2;\
   height: 20;\
   edit_type: 2; /* XttextEdit */\
   scroll_horizontal: 1;\
   scroll_vertical: 0;\
   text_buffer: fvalue;\
   vert_distance: 1;\
   horiz_distance: fwidth1;\
   send_buffer: send_flag;\
 Methods:\
   send: {\
     fvalue := fname/**/_text.text_buffer;\
     send_flag := 0;\
  }\
}

   
