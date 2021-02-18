/*-----------------------------------------------------------------*\
|  Macro: attribute_cc
|
|  Description:
|    the attribute_cc macro creates the actions on creation for
|    a vc which depends upon the existence of an attribute
|
\*----------------------------------------------------------------*/
#define attribute_cc(fname,context)\
On create:{\
if (fname/**/_/**/context/**/_sdi = NULL) then\
fname/**/_/**/context/**/_sdi := get_bound_sd_instance(get_parent_vc(SELF));\
endif;\
if (fname/**/_/**/context/**/_attr = UNDEFINED) then\
get_component(fname,context);\
endif;}
