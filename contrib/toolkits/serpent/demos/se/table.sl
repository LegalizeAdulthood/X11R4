/* $Log:	table.sl,v $
% Revision 1.1  89/10/10  22:32:48  serpent
% Initial revision
% 
Revision 1.4  89/06/19  16:23:19  ljb
fix up rcs header
    */
/*  
   this file defines the table abstraction for inclusion in a
   slang program.  There are three entry points to the abstraction:

   table_entry    which adds entries to the table
*/
#define quotify(value) "value"

#define table_entry(fcontext, fname,fparent,fwidth, fheight, fvert, fpage, flabel)\
fname/**/_attribute_/**/fcontext/**/_label: command_widget {\
Attributes:\
   parent: fparent;\
   label_text: flabel;\
   height: fheight;\
   width: fwidth;\
   vert_distance: fvert;\
   justify: LEFT;\
   border_color: "white";\
   make_visible(fpage)\
 Methods:\
   notify: {\
     fname/**/_/**/fcontext/**/_help := TRUE;\
   }\
}\
\
fname/**/_attribute_/**/fcontext/**/_text: text_widget {\
 Attributes:\
   parent: fparent;\
   width: 290 - fwidth;\
   height: fheight;\
   edit_type: 2;\
   vert_distance: fvert + 5;\
   horiz_distance: fwidth + 10;\
   border_color: "white";\
   send_buffer: send_flag;\
   text_buffer: fname/**/_/**/fcontext/**/_attr;\
   make_visible(fpage)\
 Methods:\
   send: {\
   fname/**/_/**/fcontext/**/_attr := text_buffer;\
     send_flag := 0;\
   }\
}

