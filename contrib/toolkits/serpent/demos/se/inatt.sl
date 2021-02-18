/* $Log:	inatt.sl,v $
% Revision 1.1  89/10/10  22:12:10  serpent
% Initial revision
% 
Revision 1.3  89/06/19  16:28:07  ljb
fix u fix up rcs header
   */
/*--------------------------------------------------------------------------*\
|  Macro: initialize_attr 
|
|  Description:
|    This macro contains initialization for the variables which
|    are defined initially at the object level vc
|    these are:
|    name_attr_sdi  which holds the index of the shared data item
|                    which describes the attribute name
|    name_attr_value  the local value of the attribute

\*--------------------------------------------------------------------------*/
#define initialize_attr(fname,fcontext,flink)\
fname/**/_/**/fcontext/**/_sdi := flink;\
fname/**/_/**/fcontext/**/_attr := get_component_value(\
flink, "value");\
flink := get_component_value (\
flink, "attr_list");

