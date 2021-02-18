#define boolean_attribute(fcontext, fname, fparent, fwidth, fheight, fvert, fpage, flabel)\
fname/**/_attr_label_text: command_widget {\
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
fname/**/_attr_yes_button: command_widget {\
  Attributes:\
    parent: fparent;\
    vert_distance: fvert;\
    horiz_distance: fwidth + 20;\
    label_text: "Yes";\
    width: 50;\
    height: 20;\
    make_visible(fpage)\
    background_color: {\
      if (fname/**/_/**/fcontext/**/_attr = TRUE) then\
        background_color := "black";\
      else\
        background_color := "white";\
      end if;\
    }\
    foreground_color: {\
      if (fname/**/_/**/fcontext/**/_attr = TRUE) then\
        foreground_color := "white";\
      else\
        foreground_color := "black";\
      end if;\
    }\
  Methods:\
    notify: {\
      fname/**/_/**/fcontext/**/_attr := TRUE;\
    }\
}\
fname/**/_attr_no_button: command_widget {\
  Attributes:\
    parent: fparent;\
    vert_distance: fvert;\
    horiz_distance: fwidth + 90;\
    label_text: "No";\
    width: 50;\
    height: 20;\
    make_visible(fpage)\
    background_color: {\
      if (fname/**/_/**/fcontext/**/_attr = FALSE) then\
        background_color := "black";\
      else\
        background_color := "white";\
      end if;\
    }\
    foreground_color: {\
      if (fname/**/_/**/fcontext/**/_attr = FALSE) then\
        foreground_color := "white";\
      else\
        foreground_color := "black";\
      end if;\
    }\
 Methods:\
   notify: {\
    fname/**/_/**/fcontext/**/_attr := FALSE;\
   }\
}
