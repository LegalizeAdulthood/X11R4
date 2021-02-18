/*  $Log:	create.sl,v $
% Revision 1.2  89/10/20  09:26:06  ljb
% change font
% 
% Revision 1.1  89/10/10  22:45:26  serpent
% Initial revision
% 
Revision 1.1  89/08/07  16:51:16  trm
Initial revision
    */
/*---------------------------------------------------------------------*\
|     Macro: create operator information in shared data
|
|     Usage: this macros is used after a definition of
|            a create operator.  It creates a command widget
|            and the appropriate method which creates shared data
|            and fills in as much as reasonable
|
|     Entry: create_op(flabel,x_pos,y_pos,locvar1,op_code)
|             args: flabel: label for command widget
|                   x_pos: x position of widget
|                   y_pos: y position of widget
|                   locvar1: local variable for component id
|                   op_code: operation code
|
\*--------------------------------------------------------------------*/
#define create_op(flabel,x_pos,y_pos,locvar1,op_code)\
flabel: command_widget\
        {ATTRIBUTES:\
           parent: main_background;\
\
           label_text: "op_code";\
           font: NORMAL_FONT;\
\
           width: 50;\
           height: 30;\
\
           x: x_pos;\
           y: y_pos;\
           horiz_distance: x;\
           vert_distance: y;\
\
         METHODS:\
\
notify: {\
locvar1 := create_sd_instance(\
           "component_sdd",\
           "DM_BOX"\
         );\
\
   put_component_value(\
           locvar1,\
           "input1_value_id",\
           NULL\
         );\
\
   put_component_value(\
           locvar1,\
           "input2_value_id",\
           NULL\
         );\
\
   put_component_value(\
           locvar1,\
           "operator",\
           "op_code"\
         );\
\
             put_component_value ( locvar1, "create", FALSE);\
\
             message := "Select position for argument";\
	}}
