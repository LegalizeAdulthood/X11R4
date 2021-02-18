/*--------------------------------------------------------------------------*\
|  Macro: menu_item
|
|  Description:
|    Creates a menu item object.
\*--------------------------------------------------------------------------*/
#define menu_item(name, parent_id, position, text, method)\
name: command_widget {			/* menu item			   */\
  ATTRIBUTES:\
/*\
   Variable attributes.\
*/\
    parent: parent_id;\
    vert_distance: position * 20;\
    label_text: text;\
/*\
    Constant core attributes.\
*/\
    width: 148;\
    height: 18;\
    border_color: "white";\
/*\
    Constant form constraints.\
*/\
    horiz_distance: 0;\
    justify: LEFT;\
  METHODS:\
    notify: method\
}				/* end menu_item object			    */
