/*--------------------------------------------------------------------------*\
|  Macro: move_attr
|
|  Description:
|    This macro contains a definition for the put component macro
|    which moves a value from a local variable into shared data
|    and for the get component macro which moves a value from shared data
|    into a local variable
\*--------------------------------------------------------------------------*/
#define put_component(fname,context)\
put_component_value(\
  fname/**/_/**/context/**/_sdi,\
  "fname",\
  fname/**/_/**/context/**/_attr\
);

#define get_component(fname,context)\
  fname/**/_/**/context/**/_attr :=\
  get_component_value(\
  fname/**/_/**/context/**/_sdi,\
  "fname"\
);
