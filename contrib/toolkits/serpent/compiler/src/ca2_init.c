static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/ca2_init.c,v 1.15 89/08/31 19:02:44 bmc Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:	ca2_init.c
|
|  Description:
|	This is the initialization routine for SLANG.
|
|  Component of:
|	SLANG processor
|
|  Related files:
|	many yacc, lex, and c files
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Author:  
|	Erik J. Hardy
|	ejh@sei.cmu.edu
|
|  Change History:
|	05 Jul 1988:	module created
$Log:	ca2_init.c,v $
 * Revision 1.15  89/08/31  19:02:44  bmc
 * Added dra_id_exists() function (bound to Slang function ID_EXISTS).
 * 
 * Revision 1.14  89/08/29  17:38:58  little
 * add exit function
 * 
 * Revision 1.13  89/06/14  10:56:15  little
 * change symbol tab init call to tell sym tab to enable redefined processing
 * 
 * Revision 1.12  89/04/25  09:13:58  ljb
 * no changes
 * 
 * Revision 1.11  89/04/07  16:11:11  little
 * change sym tab preload so unary and binary infix operators have
 * unary_operator_prop and binary_operator_prop respectively.
 * 
 * Revision 1.10  89/02/09  16:23:52  ejh
 * added copyright notice
 * 
 * Revision 1.9  89/02/07  10:52:01  ejh
 * fixed NULL to be id with value -1 instead of integer with value 1
 * 
 * Revision 1.8  89/01/17  16:24:18  rcs
 * added get_name and get_declaration_type built in functions to
 * symbol table
 * 
 * Revision 1.7  89/01/11  14:03:44  little
 * fix "installation" of new function in sym tab to also set global var
 * for the function symbol.
 * 
 * Revision 1.6  89/01/10  13:22:45  rcs
 * add alternative names for slang run-time functions
 * 
 * Revision 1.5  88/12/14  10:56:59  little
 * changes the undefined reserved word to not be a constant.  it will be
 * handled correctly in code generation with a special case.
 * 
 * Revision 1.4  88/10/03  08:57:36  little
 * add delete_element routine to symbol table initialize
 * 
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

#ifndef	memoryPack
#define	memoryPack
#endif

#include	"global.h"	/* global definitions */
#include	<stdio.h>	/* for I/O */
#include	<listPack.h>	/* list handling stuff */
#include	"idd.h"		/* to pick up id_types */

/*	SLANG specific includes */

#include	"cus.h"		/* SLANG symbol table stuff */
#include	"c22.h"		/* SLANG globals */
#include	"ca2.h"

#define ca2_add_function(call_as, actual_name, num_operands)\
 a_sym = cus_add_sym(call_as, id_sym, &new, NO_CONTEXT);\
 cus_put_prop(a_sym, is_built_in_prop, 1);\
 cus_put_prop(a_sym, c_function_name_prop, actual_name);\
 cus_put_prop(a_sym, num_operands_prop, num_operands);

/* -------------------------------------------  ca2_init_sym_tab */
void ca2_init_sym_tab ()

{  int	new;
   cus_sym a_sym;

   DEBUG2 (fprintf (stderr, "entering ca2_init_sym_tab\n"));


				/* create the symbol table and notify the sym
				   tab routines that redefined processing is to
				   be performed
				*/

   cus_create_sym_tab (TRUE);

				/* create the universal symbols */
   car_and_symbol =
      cus_add_sym ("and", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_and_symbol, is_reserved_word_prop, 1);
   cus_put_prop (car_and_symbol, is_built_in_prop, 1);
   cus_put_prop (car_and_symbol, c_function_name_prop, "dro_and");
   cus_put_prop (car_and_symbol, num_operands_prop, 2);
   cus_put_prop (car_and_symbol, binary_operator_prop, 1);

   car_attributes_symbol =
      cus_add_sym ("attributes", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_attributes_symbol, is_reserved_word_prop, 1);

   car_creation_condition_symbol =
      cus_add_sym ("creation_condition", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_creation_condition_symbol, is_reserved_word_prop,
      1);

/*	skip do */

   car_else_symbol =
      cus_add_sym ("else", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_else_symbol, is_reserved_word_prop, 1);

/*	skip elseif, endif, endvc, endwhile */

   car_false_symbol =
      cus_add_sym ("false", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_false_symbol, is_reserved_word_prop, 1);
   cus_put_prop (car_false_symbol, is_constant_prop, 1);
   cus_put_prop (car_false_symbol, current_type_prop, idd_boolean);
   cus_put_prop (car_false_symbol, current_value_prop, FALSE);

   car_method_symbol = cus_add_sym ("method", attribute_sym, &new, NO_CONTEXT);
   cus_put_prop (car_method_symbol, is_reserved_word_prop, 1);

   car_if_symbol =
      cus_add_sym ("if", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_if_symbol, is_reserved_word_prop, 1);

   car_null_symbol =
      cus_add_sym ("null", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_null_symbol, is_reserved_word_prop, 1);
   cus_put_prop (car_null_symbol, is_constant_prop, 1);
   cus_put_prop (car_null_symbol, current_type_prop, idd_id);
   cus_put_prop (car_null_symbol, current_value_prop, -1);

   car_self_symbol =
      cus_add_sym ("self", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_self_symbol, is_reserved_word_prop, 1);

   car_objects_symbol =
      cus_add_sym ("objects", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_objects_symbol, is_reserved_word_prop, 1);

/*	skip on_create, on_destroy */

   car_or_symbol =
      cus_add_sym ("or", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_or_symbol, is_reserved_word_prop, 1);
   cus_put_prop (car_or_symbol, is_built_in_prop, 1);
   cus_put_prop (car_or_symbol, c_function_name_prop, "dro_or");
   cus_put_prop (car_or_symbol, num_operands_prop, 2);
   cus_put_prop (car_or_symbol, binary_operator_prop, 1);

/*	skip then */

   car_true_symbol =
      cus_add_sym ("true", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_true_symbol, is_reserved_word_prop, 1);
   cus_put_prop (car_true_symbol, is_constant_prop, 1);
   cus_put_prop (car_true_symbol, current_type_prop, idd_boolean);
   cus_put_prop (car_true_symbol, current_value_prop, TRUE);

   car_undefined_symbol =
      cus_add_sym ("undefined", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_undefined_symbol, is_reserved_word_prop, 1);

/*	skip variables, vc, when_modified */

   car_while_symbol =
      cus_add_sym ("while", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_while_symbol, is_reserved_word_prop, 1);

/*	now all the operators */

/*	skip left and right braces, left and right parens, comma, and
   semicolon */

   car_dot_symbol =
      cus_add_sym (".", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_dot_symbol, num_operands_prop, 2);
   cus_put_prop (car_dot_symbol, is_built_in_prop, 1);
   cus_put_prop (car_dot_symbol, c_function_name_prop, "dro_dot");

   car_cc_dot_symbol =
      cus_add_sym ("cc_.", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_cc_dot_symbol, num_operands_prop, 2);
   cus_put_prop (car_cc_dot_symbol, is_built_in_prop, 1);
   cus_put_prop (car_cc_dot_symbol, c_function_name_prop, "dro_dot");

   car_notequal_symbol =
      cus_add_sym ("<>", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_notequal_symbol, num_operands_prop, 2);
   cus_put_prop (car_notequal_symbol, is_built_in_prop, 1);
   cus_put_prop (car_notequal_symbol, c_function_name_prop, "dro_not_equal");
   cus_put_prop (car_notequal_symbol, binary_operator_prop, 1);

   car_lessequal_symbol =
      cus_add_sym ("<=", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_lessequal_symbol, num_operands_prop, 2);
   cus_put_prop (car_lessequal_symbol, is_built_in_prop, 1);
   cus_put_prop (car_lessequal_symbol, c_function_name_prop,
		"dro_less_than_or_equal");
   cus_put_prop (car_lessequal_symbol, binary_operator_prop, 1);

   car_lessthan_symbol =
      cus_add_sym ("<", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_lessthan_symbol, num_operands_prop, 2);
   cus_put_prop (car_lessthan_symbol, is_built_in_prop, 1);
   cus_put_prop (car_lessthan_symbol, c_function_name_prop, "dro_less");
   cus_put_prop (car_lessthan_symbol, binary_operator_prop, 1);

   car_greaterequal_symbol =
      cus_add_sym (">=", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_greaterequal_symbol, num_operands_prop, 2);
   cus_put_prop (car_greaterequal_symbol, is_built_in_prop, 1);
   cus_put_prop (car_greaterequal_symbol, c_function_name_prop,
		"dro_greater_than_or_equal");
   cus_put_prop (car_greaterequal_symbol, binary_operator_prop, 1);

   car_greaterthan_symbol =
      cus_add_sym (">", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_greaterthan_symbol, num_operands_prop, 2);
   cus_put_prop (car_greaterthan_symbol, is_built_in_prop, 1);
   cus_put_prop (car_greaterthan_symbol, c_function_name_prop, "dro_greater");
   cus_put_prop (car_greaterthan_symbol, binary_operator_prop, 1);

   car_gets_symbol =
      cus_add_sym (":=", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_gets_symbol, num_operands_prop, 2);
   cus_put_prop (car_gets_symbol, is_built_in_prop, 1);
   cus_put_prop (car_gets_symbol, c_function_name_prop, "dro_assign");

/*	skip colon */

   car_equal_symbol =
      cus_add_sym ("=", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_equal_symbol, num_operands_prop, 2);
   cus_put_prop (car_equal_symbol, is_built_in_prop, 1);
   cus_put_prop (car_equal_symbol, c_function_name_prop, "dro_equal");
   cus_put_prop (car_equal_symbol, binary_operator_prop, 1);

   car_plus_symbol =
      cus_add_sym ("+", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_plus_symbol, num_operands_prop, 2);
   cus_put_prop (car_plus_symbol, is_built_in_prop, 1);
   cus_put_prop (car_plus_symbol, c_function_name_prop, "dro_add");
   cus_put_prop (car_plus_symbol, binary_operator_prop, 1);

   car_binary_minus_symbol =
      cus_add_sym ("-", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_binary_minus_symbol, num_operands_prop, 2);
   cus_put_prop (car_binary_minus_symbol, is_built_in_prop, 1);
   cus_put_prop (car_binary_minus_symbol, c_function_name_prop,
		"dro_subtract");
   cus_put_prop (car_binary_minus_symbol, binary_operator_prop, 1);

   car_unary_minus_symbol =
      cus_add_sym ("u-", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_unary_minus_symbol, num_operands_prop, 1);
   cus_put_prop (car_unary_minus_symbol, is_built_in_prop, 1);
   cus_put_prop (car_unary_minus_symbol, c_function_name_prop, "dro_negate");
   cus_put_prop (car_unary_minus_symbol, unary_operator_prop, 1);

   car_not_symbol =
      cus_add_sym ("not", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_not_symbol, is_reserved_word_prop, 1);
   cus_put_prop (car_not_symbol, num_operands_prop, 1);
   cus_put_prop (car_not_symbol, is_built_in_prop, 1);
   cus_put_prop (car_not_symbol, c_function_name_prop, "dro_not");
   cus_put_prop (car_not_symbol, binary_operator_prop, 1);

   car_slash_symbol =
      cus_add_sym ("/", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_slash_symbol, num_operands_prop, 2);
   cus_put_prop (car_slash_symbol, is_built_in_prop, 1);
   cus_put_prop (car_slash_symbol, c_function_name_prop, "dro_divide");
   cus_put_prop (car_slash_symbol, binary_operator_prop, 1);

   car_power_symbol =
      cus_add_sym ("**", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_power_symbol, num_operands_prop, 2);
   cus_put_prop (car_power_symbol, is_built_in_prop, 1);
   cus_put_prop (car_power_symbol, c_function_name_prop, "dro_exponentiate");
   cus_put_prop (car_power_symbol, binary_operator_prop, 1);

   car_star_symbol =
      cus_add_sym ("*", operator_sym, &new, NO_CONTEXT);
   cus_put_prop (car_star_symbol, num_operands_prop, 2);
   cus_put_prop (car_star_symbol, is_built_in_prop, 1);
   cus_put_prop (car_star_symbol, c_function_name_prop, "dro_multiply");
   cus_put_prop (car_star_symbol, binary_operator_prop, 1);

/*	now the others that aren't keywords or operators */

   car_comment_symbol =
      cus_add_sym ("comment", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_comment_symbol, is_comment_prop, 1);

   car_function_call_symbol =
      cus_add_sym ("function_call", id_sym, &new, NO_CONTEXT);

/*----------  Add names of built in functions to the symbol table ----------*/

   ca2_add_function ("get_parent_vc", "dra_get_parent_vc", 1);
   ca2_add_function ("get_creating_sd", "dra_get_creating_sd", 1);
   ca2_add_function ("get_bound_sd_instance", "dra_get_creating_sd", 1);
   ca2_add_function ("get_object", "dra_get_object", 2);
   ca2_add_function ("get_component_value", "dra_get_component_value", 2);
   ca2_add_function ("get_sd_value", "dra_get_component_value", 2);
   ca2_add_function ("put_component_value", "dra_put_component_value", 3);
   ca2_add_function ("put_sd_value", "dra_put_component_value", 3);
   ca2_add_function ("get_vc_id", "dra_get_vc_id", 2);
   ca2_add_function ("get_vc", "dra_get_vc_id", 2);
   ca2_add_function ("get_variable_value", "dra_get_variable_value", 2);
   ca2_add_function ("put_variable_value", "dra_put_variable_value", 3);
   ca2_add_function ("create_element", "dra_create_element", 2);
   ca2_add_function ("create_sd_instance", "dra_create_element", 2);
   ca2_add_function ("delete_element", "dra_delete_element", 1);
   ca2_add_function ("destroy_sd_instance", "dra_delete_element", 1);
   ca2_add_function ("get_name", "dra_get_name", 1);
   ca2_add_function ("get_declaration_type", "dra_get_type", 1);
   ca2_add_function ("exit", "dra_exit", 0);
   ca2_add_function ("id_exists", "dra_id_exists", 1);

   car_new_function = cus_add_sym ("new", id_sym, &new, NO_CONTEXT);
   cus_put_prop (car_new_function, is_built_in_prop, 1);
   cus_put_prop (car_new_function, c_function_name_prop, "dra_new");
   cus_put_prop (car_new_function, num_operands_prop, 1);

   DEBUG2 (fprintf (stderr, "leaving ca2_init_sym_tab\n"));
}


/* ---------------------------------------------  ca2_make_god_vc */
void ca2_make_god_vc ()
{  int new;

      		/* put god vc in with all of its glory */
   car_god_vc = cus_add_sym (c22_god_text, vc_sym, &new, NO_CONTEXT);
   cus_put_prop (car_god_vc, parent_vc_prop, NULL);
   cus_put_prop (car_god_vc, u_name_prop, c22_god_text);
   cus_put_prop (car_god_vc, ill_files_prop, make_list ());
   cus_put_prop (car_god_vc, creation_condition_prop, car_true_symbol);
   cus_put_prop (car_god_vc, cc_used_vars_prop, (char *) make_list ());

   return;
}	


