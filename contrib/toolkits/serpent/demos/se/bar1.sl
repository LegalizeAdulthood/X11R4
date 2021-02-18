/*--------------------------------------------------------------------------*\
|  Macro: menu_bar_item
|
|  Description:
|    Creates a menu item object.
\*--------------------------------------------------------------------------*/
#define menu_bar_item(name, parent_id, distance, text, method)\
name: command_widget {			/* menu item			   */\
  ATTRIBUTES:\
/*\
   Variable attributes.\
*/\
    parent: parent_id;\
    horiz_distance: distance;\
    label_text: text;\
/*\
    Constant attributes.\
*/\
    width: 67;\
    height: 20;\
    foreground_color: "white";\
    background_color: "black";\
    vert_distance: 0;\
\
  METHODS:\
    notify: method\
}
