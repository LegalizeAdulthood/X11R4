/* $Log:	satt.sl,v $
% Revision 1.1  89/10/10  22:19:17  serpent
% Initial revision
% 
Revision 1.2  89/06/19  16:22:30  ljb
fix up rcs header
   */
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
