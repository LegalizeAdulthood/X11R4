/*--------------------------------------------------------------------------*\
|
|  Macro:  dual_title_bar
|
|  Description:
|     The dual_title_bar macro provides a menu bar which contains
|     both a title label widget and an uneditable name field.
|
|  Parameters:
|
|     fname - The name of the objects.
|     fparent   - parent (form) of the object.
|     flabel1 - Label text for first field.
|     fwidth1   - width of the label in bits.
|     flabel2 - Label text for second field.  Must be a snippet. 
|     fwidth2   - width of the editable field.
|
|  History:
|     $Log:	dtb.sl,v $
% Revision 1.1  89/10/10  22:05:59  serpent
% Initial revision
% 
Revision 1.2  89/08/03  16:40:35  rcs
i can't remember

|
\*--------------------------------------------------------------------------*/
#define dual_title_bar(fname, fparent, flabel1, fwidth1, flabel2, fwidth2)\
     fname/*filler*/1: label_widget {\
       ATTRIBUTES:\
         parent: fparent;\
         border_width: 0;\
         background_color: "black";\
         foreground_color: "white";\
         height: 18;\
         label_text: flabel1;\
         width: fwidth1;\
         horiz_distance: 1;\
         vert_distance: 1;\
         justify: LEFT;\
     }\
\
     fname/*filler*/2: label_widget {\
       ATTRIBUTES:\
         parent: fparent;\
         border_width: 0;\
         background_color: "black";\
         foreground_color: "white";\
         height: 18;\
         label_text: flabel2;\
         width: fwidth2;\
         horiz_distance: fwidth1;\
         vert_distance: 1;\
         justify: LEFT;\
     }
   
