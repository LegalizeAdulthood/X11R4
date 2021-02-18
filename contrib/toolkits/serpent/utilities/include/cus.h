/*      $Header: /u/04c/mnt/new_integ/utilities/include/RCS/cus.h,v 1.23 89/10/08 00:03:29 serpent Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  cus.h 
|
|  Version: 1.0
|
|  Change History:
|	9 jun 88 - add change warning comment to the property enum typedef
|	10 may 88 - new module
|
|  Description:  
|	the header file utilized by users of the symbol table package
|
|  Component of:
|	compiler symbol table
|
|  Related files:
|	cus.c - compiler symbol table package
|	cus_priv.h - symbol table package internal header
|
|  Project:
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Original Author:  
|     Reed Little
|     rcs@sei.cmu.edu  
$Log:	cus.h,v $
 * Revision 1.23  89/10/08  00:03:29  serpent
 * short names: referred to cus_priv.h instead of cus_private.h
 * 
 * Revision 1.22  89/10/02  13:28:11  rcs
 * removed stupid include of serpent.h
 * 
 * Revision 1.21  89/09/29  15:36:21  little
 * add name_text_prop, this is getting boring
 * 
 * Revision 1.20  89/09/29  14:09:01  little
 * add creation_condition_text_prop
 * 
 * Revision 1.19  89/09/29  14:02:19  little
 * add the comment text props
 * 
 * Revision 1.18  89/09/28  14:22:09  little
 * add the text snippet properties
 * 
 * Revision 1.17  89/09/26  18:08:37  bmc
 * Added properties for dialogue editor application
 * 
 * Revision 1.16  89/07/25  21:41:25  bmc
 * Added sd_template_prop
 * 
 * Revision 1.15  89/06/16  17:35:52  little
 * move the props added in the last change to the end of the list
 * 
 * Revision 1.14  89/06/16  16:32:14  bmc
 * Added potential_vcs_prop and saved_sne_prop to property enumeration.
 * 
 * Revision 1.13  89/06/14  11:00:03  little
 * change the sym tba init routine to take a param which indicates whether to
 * perform redefined processing
 * 
 * Revision 1.12  89/06/09  14:28:02  little
 * add last_prop_DO_NOT_USE as a "terminator" in prop enum list
 * 
 * Revision 1.11  89/04/07  17:47:37  ljb
 * add properties for saving shared data information across vcs
 *  and for user written functions
 * 
 * Revision 1.10  89/04/03  16:40:27  little
 * add the snippet_text_prop, which is used by the editor to keep the
 * text form of a snippet.
 * 
 * Revision 1.9  89/02/26  10:42:42  little
 * add cc_lineno_prop (it contains the input file line # on which the last
 * line of the cc appears)
 * 
 * Revision 1.8  89/02/10  14:29:46  ejh
 * added copyright notice
 * 
 * Revision 1.7  89/02/01  16:23:24  little
 * add alternate_print_string_prop 
 * 
 * Revision 1.6  89/01/18  14:20:03  ejh
 * new ill file preamble properties
 * 
 * Revision 1.5  89/01/17  16:29:07  little
 * add bound_elem_sym and bound_comp_sym to cus_sym_type
 * 
 * Revision 1.4  89/01/17  16:06:15  rcs
 * added header for cus_get_sym_type_string routine
 * 
 * Revision 1.3  88/12/20  15:31:29  ejh
 * removed extraneous comment props
 * 
 * Revision 1.2  88/12/20  13:14:55  little
 * add user_defined_prop and redefined_prop
 * 
|
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#ifndef cus
#define cus

#include "global.h"
#include "listPack.h"
#include "hashPack.h"


/*--------------------------------------------------    SYMBOLIC CONSTANTS */
#define NO_CONTEXT 0
#define WITH_CONTEXT 1

/*-----------------------------------------------------     TYPEDEFS */

/*--------------------------------------------------------------------------*\
|  Type: cus_symbol_table
|
|  Description:
|     Provides a handle to a symbol table.
|
\* Definition:                                                              */

typedef private			cus_symbol_table;
#define cus_no_symbol_table	((cus_symbol_table) NULL)
#define CUS_NO_SYMBOL_TABLE	cus_no_symbol_table

/*--------------------------------------------------------------------------*\
|  Description:  
|     the exported type for users to access contexts
| 
|	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
|	DO NOT FORGET TO UPDATE THE NAME ARRAY WHEN MODIFYING THIS ENUM 
|		IT IS NAMED cus_symbol_type_print_value AND IS IN
|		THE FILE NAMED cus_priv.h
|
|	ALSO, DO NOT PUT IN ANY EXPLICIT ASSIGNMENTS FOR ENUM MEMBER
|		VALUES
|
|	ADD ALL NEW TYPES TO THE END OF THE LIST
|
|        ALSO, DO NOT FORGET TO UPDATE THE cus_sym_type_strings NAME 
|               ARRAY IN cus.c after modifying this enum.
|	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
|
\* Definition:                                                              */
typedef int cus_context_type;

/*--------------------------------------------------------------------------*\
|  Description:  
|     the exported symbol type definition
| 
\* Definition:                                                              */
typedef enum sym_type {
   null_sym,
   vc_sym,
   object_sym,
   attribute_sym,
   function_sym,
   function_call_sym,
   variable_sym,
   snippet_sym,
   operator_sym,
   if_sym,
   while_sym,
   comment_sym,
   ill_data_definition_sym,
   assignment_sym,
   id_sym,
   sd_sym,
   temp_sym,
   constant_sym,
   ill_sym,
   bound_elem_sym,
   bound_comp_sym
   }  cus_sym_type;

/*--------------------------------------------------------------------------*\
|  Description:  
|     the exported legal property names
|
|	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
|	DO NOT FORGET TO UPDATE THE NAME ARRAY WHEN MODIFYING THIS ENUM 
|		IT IS NAMED cus_property_type_print_value AND IS IN
|		THE FILE NAMED cus_priv.h
|
|	ADD ALL NEW PROPERTIES TO THE END OF THE LIST
|
|	ALSO, DO NOT PUT IN ANY EXPLICIT ASSIGNMENTS FOR ENUM MEMBER
|		VALUES
|	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
|
\* Definition:                                                              */

typedef enum property_type {
   instances_prop,			/* list of instance ids	          */
   template_prop,			/* id of template	          */
   parent_vc_prop,			/* id of parent vc                */
   objects_prop,			/* list of object ids             */
   variables_prop,			/* list of variable ids           */
   sub_vcs_prop,			/* list of subvc ids              */
   creation_condition_prop,
   methods_prop,
   create_actions_snippet_prop,
   destroy_actions_snippet_prop,
   type_prop,
   attributes_prop,
   constant_value_prop,
   value_snippet_prop,
   modified_snippet_prop,
   current_type_prop,			/* idd_data_types		  */
   current_value_prop,			/* value specified in variable	  */
   initial_value_snippet_prop,
   s_tree_prop,
   set_vars_prop,
   used_vars_prop,
   op_type_prop,
   owned_context_prop,
   result_type_prop,
   c_function_name_prop,
   num_operands_prop,
   operator_type_prop,
   data_definition_prop,
   add_operator_type,
   subtract_operator_type,
   object_type_prop,
   mailbox_prop,
   is_reserved_word_prop,
   is_constant_prop,
   is_comment_prop,
   vc_comments_prop,
   variable_comments_prop,
   object_comments_prop,
   cc_used_vars_prop,
   ask_prop,
   u_name_prop,
   fixed_method_snippet_prop,
   parent_object_prop,
   c_proc_number_prop,
   fixed_creation_condition_prop,

/*	ill file preamble stuff */

   generic_magic_id_prop,
   specific_magic_id_prop,
   ill_file_name_prop,
   mailbox_name_prop,
   execution_name_prop,
   exec_command_prop,

   idd_handle_prop,
   id_prop,
   ill_files_prop,
   taps_prop,
   original_name_prop,
   creating_sd_prop,
   vce_prop,
   is_built_in_prop,
   fixed_method_prop,
   is_ok_id_name_prop,
   created_vcs_prop,
   old_cc_used_vars_prop,
   pet_proc_name_prop,
   cc_uses_new_prop,
   user_defined_prop,
   redefined_prop,
   alternate_print_string_prop,
   cc_lineno_prop,
   binary_operator_prop,
   unary_operator_prop,
   snippet_text_prop,
   old_created_vcs_prop,		/*these three prop*/
   old_parent_vc_prop,			/*  were added to save*/ 
   old_owned_context_prop,		/*  information across vc use*/
   parameter_conversion_functions_prop,  /*for use with c functions*/
   returned_value_conversion_function_prop,
   potential_vcs_prop,                  /* list of vcs interested in s.d. */
   saved_sne_prop,
   sd_template_prop,                    /* SDD template for shared data item */

/* 	properties used solely by dialogue editor application */

   file_name_prop,			/* name of file containing dialogue */
   prologue_comments_prop,		/* comments from dialogue prologue */
   macros_prop,				/* macros from dialogue prologue */
   externs_prop,			/* dialogue external declarations */
   technologies_prop,			/* technologies used by dialogue */
   shared_data_prop,			/* shared data used by dialogue */
   eve_vc_prop,				/* holds handle of top level vc */
   symbol_table_prop,			/* handle of containing symbol table */
   dialogue_context_prop,		/* sym. tab. context for a dialogue */

   value_snippet_text_prop,		/* attribute snippet text */
   create_actions_snippet_text_prop,	/* create actions snippet text */
   destroy_actions_snippet_text_prop,	/* destroy actions snippet text */
   initial_value_snippet_text_prop,	/* variable snippet text */

   vc_comments_text_prop,		/* text version of prop */
   variable_comments_text_prop,		/* text version of prop */
   object_comments_text_prop,		/* text version of prop */
   creation_condition_text_prop,	/* text version of prop */
   name_text_prop,			/* text version of prop */

/*
	ADD ALL PROPERTIES BEFORE THIS ONE
*/
   last_prop_DO_NOT_USE
 } cus_property_type;   /* END */


/*--------------------------------------------------------------------------*\
|  Description:  
|     the exported type for users to access symbol handles.
| 
\* Definition:                                                              */
struct cus_sym_entry;
typedef struct cus_sym_entry *cus_sym;


/*--------------------------------------------------    FUNCTIONS */

/*--------------------------------------------------------------------------*\
|  Routine: cus_get_sym_type_string
|
|  Description:
|     The cus_get_sym_type_string function returns the string corresponding
|     to the specified symbol type.
|
\* Syntax:                                                  */

	string cus_get_sym_type_string(
	  /* symbol_type in : cus_sym_type */
	);

/*
|  Parameters:
|	symbol_type - Symbol type enumeration value.
|
|  Returns:
|	The string corresponding to symbol_type.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: cus_create_sym_tab
|
|  Description:
|     make a symbol table item
|
\* Syntax:                                                  		    */
	void cus_create_sym_tab (
	  /* do_redefine_flag in : boolean */
	);

/*
|  Parameters:
|	do_redefine_flag - true indicates redefine processing should occur,
|	   false saays do not do redefine processing
|
|  Returns:
|	nothing
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: cus_add_sym
|
|  Description:
|	adds a symbol to the symbol table (potentially)
\* Syntax:                                                  */
        cus_sym cus_add_sym (
/*           name : in string, */
/*           type : in cus_sym_type, */
/*           new : out int*, */
/*           worry_about_context : in int */
	);
/*
|  Parameters:
|	name - the string name of the symbol
|	type - the type of the symbol
|	new - indicator if anything was actually added to the table
|	worry_about_context - indicator as to if context should matter
|  Returns:
|	The symbol handle.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: cus_delete_sym
|
|  Description:
|	Deletes a specified symbol from the symbol table.
|
\* Syntax:		                                                    */

        boolean cus_delete_sym(
	  /* sym : in out cus_sym */
	);

/*
|  Parameters:
|	sym - Symbol to be deleted.
|
|  Returns:
|	True if delete; false otherwise.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: cus_look_up
|
|  Description:
|	look up a symbol in the table, if there return the handle, else
|	return NULL.
\* Syntax:                                                  */
	cus_sym cus_look_up (
/*           name : in char* */
	);
/*
|  Parameters:
|	name - the string representing the desired symbol
|  Returns:
|	the handle for the symbol or NULL.
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_sym_representation
|
|  Description:
|	look up a symbol in the table, if there return the handle, else
|	return NULL.
\* Syntax:                                                  */
	char *cus_sym_representation (
/*           sym : in cus_sym */
	);
/*
|  Parameters:
|	sym - a symbol handle
|  Returns:
|	the print string representation for the symbol
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_put_prop
|
|  Description:
|       (re)places a property value on the symbol's property list
\* Syntax:							*/
	void cus_put_prop (
/*            sym : in cus_sym, */
/*            property_name : in cus_property_type, */
/*            property_value : in char* */
	);
/*
|  Parameters:
|	sym - a symbol handle
|	property_name - the property to set
|	property_value - the value to place in the property list
|  Returns:
|	nothing
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_get_prop
|
|  Description:
|       retrieves a property value from the symbol's property list
\* Syntax:							*/
	char *cus_get_prop (
/*            sym : in cus_sym, */
/*             property_name : in cus_property_type */
	);
/*
|  Parameters:
|	sym - a symbol handle
|	property_name - the property to fetch
|  Returns:
|	The value of the property_name property or NULL
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_create_nu_context
|
|  Description:
|	creates a new context
\* Syntax:							*/
	cus_context_type cus_create_nu_context ();
/*
|  Parameters:
|	none
|  Returns:
|	The new context identifier
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_activate_context
|
|  Description:
|	activate an existing context
\* Syntax:							*/
	void cus_activate_context (
/*            context : in cus_context_type */
	);
/*
|  Parameters:
|	context - the context to which the symbol table is to be configured
|  Returns:
|	nothing
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_current_context
|
|  Description:
|	get the current symbol table contest
\* Syntax:							*/
	cus_context_type cus_current_context ();
/*
|  Parameters:
|	none
|  Returns:
|	The current context
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_deactivate_context
|
|  Description:
|	"pops" back to the parent of the current context
\* Syntax:							*/
	cus_context_type cus_deactivate_context (
	);
/*
|  Parameters:
|	none
|  Returns:
|	The new context
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_sym_context
|
|  Description:
|	find the context in which a symbol was defined
\* Syntax:							*/
	cus_context_type cus_sym_context (
/*           sym : in cus_sym */
	);
/*
|  Parameters:
|	sym - a symbol handle
|  Returns:
|	context in which sym was defined
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_gensym
|
|  Description:
|	generate a unique symbol and place in the symbol table
\* Syntax:							*/
	cus_sym cus_gensym (
/*            prefix : in char* */
/*            type : in cus_sym_type */
	);
/*
|  Parameters:
|	prefix - the prefix string, should be composed of printing
|		ascii characters.
|	type - type for the generated symbol
|  Returns:
|	The handle of the generated symbol.
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_walk_sym_tab
|
|  Description:
|	walk through the entire symbol table applying a user defined function
|	to all the symbols
\* Syntax:							*/
	void cus_walk_sym_tab (
/*           the_funct : in int (*the_funct) () */
/*            the_thing : in char*) */
	);
/*
|  Parameters:
|	the_funct - a user supplied int valued function.
|	the_thing - the magic free parameter to the_funct.
|  Returns:
|	nothing
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_is_sym
|
|  Description:
|	determine if the parameter is a symbol handle
\* Syntax:                                                      */
	int cus_is_sym (
/*	    sym : in cus_sym */
        );
/*
|  Parameters:
|	sym - the handle to be checked
|  Returns:
|	non-zero if the parameter is a symbol handle, zero otherwise
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_string_constant
|
|  Description:
|	add a string constant to symbol table
\* Syntax:                                                      */
	cus_sym cus_string_constant (
/*	    the_string : in string */
        );
/*
|  Parameters:
|	the_string - string value of constant
|  Returns:
|	the resulting symbol
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_genname
|
|  Description:
|	create a unique string
\* Syntax:                                                      */
	char *cus_genname (
/*	    prefix : in string */
        );
/*
|  Parameters:
|	prefix - the prefix string, should be composed of printing
|		ascii characters.
|  Returns:
|	pointer to the resulting "uniquified" string
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_prop_print_val
|
|  Description:
|	get the print string for a propertY
\* Syntax:                                                      */
	string cus_prop_print_val (
/*	    property : in cus_property_type */
        );
/*
|  Parameters:
|	property - the property for which the print string is desired
|  Returns:
|	pointer to the resulting string
|
|  NOTE:
|	no check is made to ensure the property parameter is a valid property.
|	if it is not valid, rather startling strings will be returned
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_type_print_val
|
|  Description:
|	get the print string for a propertY
\* Syntax:                                                      */
	string cus_type_print_val (
/*	    type : in cus_type_print_val */
        );
/*
|  Parameters:
|	type - the symbol type for which the print string is desired
|  Returns:
|	pointer to the resulting string
|
|  NOTE:
|	no check is made to ensure the type parameter is a valid type.
|	if it is not valid, rather startling strings will be returned
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: cus_print_property_list
|
|  Description:
|	print all the properties for a symbol
\* Syntax:                                                      */
	void cus_print_property_list (
/*	    stream: in FILE *, 		*/
/*	    sym: in cus_sym,		*/
/*	    indent: in string		*/
/*	    their_funct: in int (*) () */
        );
/*
|  Parameters:
|	stream - stream on which to print the property list
|	sym - symbol whose property list to be printer
|	indent - indentation to use for the print
|	their_funct - optional user supplied four parameter function which
|		is handed the properties, one at a time.  first parameter
|		is the stream on which to print, second parameter is the
|		indentation to use, third parameter
|		is the symbol,  & fourth parameter is a property.  this
|		provides for user control when printing the property values.
|		NULL should be used when the user want the default
|		property print as described below.
|  Returns:
|	nothing
|
|  NOTE:
|	the default print of is such that property values (w/the exception of
|	the type_prop) are not
|	printed because it is difficult to know what type each of the property
|	values are w/o apriori putting it in the cus code.
\*--------------------------------------------------------------------------*/


#endif

