/* 
    The ID attribute macro creates a form entry for an attribute which
    requires the name of another object instance.  The Macro displays
    a command widget with the name of the attribute and a label
    widget labeled with the name of the object instance, or NULL if none.  
    Selecting the command widget with the name of the attribute causes 
    help information to be displayed.  The name of the object instance
    is modified through direct manipulation with the object.
*/
#define id_attribute(fcontext, fname, fparent, fwidth, fheight, fvert, fpage, flabel)\
fname/**/_/**/fcontext/**/_label: command_widget {\
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
fname/**/_/**/fcontext/**/_text: label_widget {\
  Attributes:\
    parent: fparent;\
    height: 20;\
    width: 70;\
    vert_distance: fvert;\
    horiz_distance: fwidth + 10;\
    label_text: "?????";\
    make_visible(fpage)\
}
