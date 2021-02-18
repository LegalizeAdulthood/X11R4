/*-------------------------------------------------------------------------*\
|   
|  Macro:  help_template
|
|  Description:  
|     The help_template provides the base form for help information.
|
\*-------------------------------------------------------------------------*/
#define help_template(variable, label, text, text_height)\
VC : variable/**/_vc\
\
  Creation Condition : (variable)\
\
  Objects:\
\
    variable/**/_form: form_widget {\
      Attributes:\
        parent: background;\
        width: 502;\
        height: text_height + 102;\
        allow_user_move: true;\
/* ?? the next 4 attrs can be removed when the compiler bug is fixed */\
        horiz_distance: 0;\
        vert_distance: 0;\
        x: 0;\
        y: 0;\
      Methods:\
        move: {\
          horiz_distance := x;\
          vert_distance := y;\
        }\
    }\
\
    variable/**/_title: label_widget {\
      Attributes:\
        parent: variable/**/_form;\
        background_color: "black";\
        foreground_color: "white";\
        height: 18;\
        label_text: label;\
        width: 500;\
        horiz_distance: 1;\
        vert_distance: 1;\
        justify: CENTER;\
    }\
\
    variable/**/_text: text_widget {\
      Attributes:\
        parent: variable/**/_form;\
        border_color: "white";\
        text_buffer: text;\
        vert_distance: 40;\
        horiz_distance: 40;\
        editable: FALSE;\
        width: 420;\
        height: text_height;\
    }\
\
    button(\
      ok_button,\
      variable/**/_form,\
      text_height + 60,	/* distance from top of form	*/\
      NULL,\
      225,	/* distance from left edge of form  */\
      NULL,\
      "OK",\
      {\
        variable := false;\
      }\
    )\
\
end VC variable/**/_vc
