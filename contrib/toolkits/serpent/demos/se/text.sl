/*--------------------------------------------------------------------------*\
|  Macro: text_input
|
|  Description:
|    Creates a text input window consisting of a form, a title bar, a text
|    widget, and an "ok" and "cancel" button.
\*--------------------------------------------------------------------------*/
#define text_input(guy, prompt, prompt_length)\
guy/**/_form: form_widget {\
  Attributes:\
    parent: background;\
    vert_distance: 500;\
    horiz_distance: 500;\
    width: 80 * FONT_WIDTH + 10;\
    height: 251;\
}	/* end form object		    */\
\
dual_title_bar( /* vc title object  */\
  guy/**/_title,\
  guy/**/_form,\
  prompt,\
  prompt_length * TITLE_FONT_WIDTH,\
 {\
    if (get_component_value(parental_unit, "name") = UNDEFINED) then\
      label_text := "Undefined";\
    else\
      label_text := get_component_value(parental_unit, "name");\
    endif;\
  },\
  408 - (prompt_length * TITLE_FONT_WIDTH)\
)       /* end vc title object      */\
\
guy/**/_text: text_widget {\
\
  Attributes:\
    parent: guy/**/_form;\
    width: 80 * FONT_WIDTH;\
    height: 24 * FONT_HEIGHT;\
    vert_distance: 23;\
    resizable: 0;\
    allow_resize: 0;\
    edit_type: 2; /* XttextEdit */\
    max_length: 256;\
/*    scroll_horizontal: 0;\
    scroll_on_overflow: 1;*/\
    scroll_vertical: 1;\
    word_break: 1;\
    text_buffer: "/* Enter information below */\n";\
  Methods:\
    send: {\
      dialogue_sdd.name := text_buffer;\
      display_/**/guy := false;\
    }\
}	/* end text object    */\
\
button(\
  ok_button,\
  guy/**/_form,\
  20,\
  guy/**/_text,\
  125,\
  NULL,\
  "OK",\
  {\
    guy/**/_text.send_buffer := 1;\
  }\
)\
\
 button(\
  cancel_button,\
  guy/**/_form,\
  20,\
  guy/**/_text,\
  235,\
  NULL,\
  "Cancel",\
  {\
    display_/**/guy := false;\
  }\
)

