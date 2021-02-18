/*--------------------------------------------------------------------------*\
|  Macro: select_item
|
|  Description:
|    Creates a selectable menu item object.
\*--------------------------------------------------------------------------*/
#define select_item(name, parent_id, position, text, selectable)\
name/**/_select_button: command_widget {\
  Attributes:\
    parent: parent_id;\
    vert_distance: position * 20 + 10;\
    label_text: {\
      if (selectable) then\
        label_text := "X";\
      else\
        label_text := "";\
      end if;\
    }\
    width: 14;\
    height: 14;\
    horiz_distance: 5;\
    justify: LEFT;\
  Methods:\
    notify: {\
      selectable := NOT selectable;\
    }\
}\
name/**/_label: label_widget {\
  Attributes:\
    parent: parent_id;\
    vert_distance: position * 20;\
    label_text: text;\
    width: 168;\
    height: 18;\
    border_color: "white";\
    horiz_distance: 30;\
    justify: LEFT;\
}
