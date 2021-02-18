/*--------------------------------------------------------------------------*\
|  Macros: data access
|
|  Description:
|    The following macros are used to get/put variables in specific,
|    often referenced locations.  This will become obsolete when dot
|    notation arrives.
\*--------------------------------------------------------------------------*/
#define quotify(value) "value"

#define get_local(variable) get_variable_value(get_parent_vc(SELF),"variable")
#define put_local(variable, value)\
	put_variable_value(get_parent_vc(SELF),"variable",value)

#define get_grandparent(thing) get_parent_vc(get_parent_vc(thing))
#define get_great_grandparent(thing)\
	get_parent_vc(get_parent_vc(get_parent_vc(thing)))

#define get_one_up(variable)\
	get_variable_value(get_grandparent(SELF),"variable")
#define put_one_up(variable, value)\
	put_variable_value(get_grandparent(SELF),"variable",value)
