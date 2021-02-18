/*--------------------------------------------------------------------------*\
|  Macro: define_attr 
|
|  Description:
|    This macro contains a definition for the variables which
|    are defined initially at the object level vc
|    these are:
|    name_attr_sdi  which holds the index of the shared data item
|                    which describes the attribute name
|    name_attr_value  the local value of the attribute
|    name_attr_help   does help exist for this attribute
\*--------------------------------------------------------------------------*/
#define define_attr(fname,fcontext)\
fname/**/_/**/fcontext/**/_sdi;\
fname/**/_/**/fcontext/**/_help: FALSE;\
fname/**/_/**/fcontext/**/_attr;


