/*--------------------------------------------------------------------------*\
|  Macro: save_attr 
|
|  Description:
|    This macro contains saving in shared data for the variables which
|    are defined initially at the object level vc
|    these are:
|    name_attr_sdi  which holds the index of the shared data item
|                    which describes the attribute name
|    name_attr_value  the local value of the attribute

\*--------------------------------------------------------------------------*/
#define save_attr(fname,fcontext)\
put_component_value (\
fname/**/_/**/fcontext/**/_sdi,\
"value",\
fname/**/_/**/fcontext/**/_attr);
