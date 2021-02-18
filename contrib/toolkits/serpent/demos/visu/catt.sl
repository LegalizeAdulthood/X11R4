/*--------------------------------------------------------------------------*\
|  Macro: create_attr
|
|  Description:
|    The create_attr macro creates an object_instance_attribute_sdd
|    instance and sets the appropriate fields.
|
|  IMPORTANT NOTE:
|    If you leave a space between first two parms in the macro the 
|    macro will not work.
\*--------------------------------------------------------------------------*/
#define create_attr(fname,fparent,fvalue,fprev,fcur)\
fcur := create_sd_instance(\
  "object_instance_attribute_sdd",\
  "DM_BOX"\
);\
put_component_value(\
fprev,\
"attr_list",\
fcur);\
put_component_value(\
  fcur,\
  "name",\
  "fname"\
);\
put_component_value(\
  fcur,\
  "object",\
  fparent\
);\
put_component_value(\
  fcur,\
  "value",\
  fvalue\
);

