/*--------------------------------------------------------------------------*\
|
|  Macro:  editable_title_bar
|
|  Description:
|     The editable_title_bar macro creates an editable title bar including
|     a label widget, which displays a title, an editable field for a 
|     name, and a command button which will cause the value in the 
|     editable field to be returned.
|
|  Parameters:
|     fname	- unique name.
|     fparent	- parent (form) of the object.
|     flabel	- text widget label.
|     fwidth1	- width of the label in bits.
|     fvalue	- contents of editable field.
|     fwidth2	- width of the editable field.
|
|  History:
|     $Log:	etb.sl,v $
% Revision 1.1  89/10/10  22:07:10  serpent
% Initial revision
% 
Revision 1.2  89/07/18  09:53:40  rcs
changed stuff here too

|
\*--------------------------------------------------------------------------*/
#define editable_title_bar(fname, fparent, flabel, fwidth1, fvalue, fwidth2)\
fname/**/_label: label_widget {\
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
fname/**/_text: text_widget {\
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
   text_buffer: {\
     if (fvalue = UNDEFINED) then\
       text_buffer := "Undefined";\
     else\
       text_buffer := fvalue;\
     endif;\
   }\
   vert_distance: 1;\
   horiz_distance: fwidth1;\
   send_buffer: send_flag;\
 Methods:\
   send: {\
     fvalue := fname/**/_text.text_buffer;\
     send_flag := 0;\
  }\
}\
fname/**/_commit: command_widget {\
 Attributes:\
   parent: fparent;\
   border_width: 1;\
   border_color: "white";\
   label_text: "";\
   background_color: "black";\
   foreground_color: "white";\
   height: 14;\
   width: 14;\
   horiz_distance: fwidth1 + fwidth2 - 18;\
   vert_distance: 3;\
  METHODS:\
    notify: { fname/**/_text.send_buffer := 1; }\
}
