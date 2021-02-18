/*--------------------------------------------------------------------------*\
|
|  Name:	cay.y
|
|  Description:
|	This is the grammar for SLANG.
|
|  Component of:
|	SLANG processor (interpreter and compiler)
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
$Log:	cay.y,v $
 * Revision 1.20  89/10/23  20:00:16  ejh
 * switched over to u22_copy_string
 * 
 * Revision 1.19  89/09/20  12:01:03  little
 * change the rather strange use of idd_get_component_lop to check and
 * see if a component is in an element to use idd_validate_component instead
 * change the elseif processing to correctly catch the last else in
 * a conditional with several (more than 1) elseifs.
 * 
 * Revision 1.18  89/08/07  16:41:41  ljb
 * put _ after dob_make
 * 
 * Revision 1.17  89/08/03  12:13:37  ejh
 * fixed parameter list in external function declarations
 * 
 * Revision 1.16  89/07/31  14:33:50  ejh
 * put the handling of externals (formerly function defs) inside lex and yacc
 * 
 * Revision 1.15  89/07/24  13:48:51  ejh
 * added snippets to variables
 * 
 * Revision 1.14  89/04/07  16:13:59  little
 * add vc, object, method, attribute, & var counters
 * 
 * Revision 1.13  89/02/27  17:04:44  little
 * cls
 * add call to cam_fix_news in the creation condition recognizer.  this
 * routine converts string parameters to new function into actual data
 * items with the same name as the string value.
 * 
 * Revision 1.12  89/02/22  16:23:04  rcs
 * no change
 * 
 * Revision 1.11  89/02/09  16:28:59  ejh
 * added copyright notice
 * 
 * Revision 1.10  89/01/26  17:32:24  little
 * add capability for lhs of assignment to be a qualified name
 * 
 * Revision 1.9  89/01/04  14:41:51  ejh
 * added optional eos after ENDVC name
 * 
 * Revision 1.8  88/12/21  17:01:27  ejh
 * new syntax for expressions
 * 
 * Revision 1.7  88/12/20  15:35:04  ejh
 * removed extraneous comments; added '&' and '|'; redefined real numbers
 * returned by lex to (double *); removed all references to WHENMODIFIED;
 * made code snippets optional in aoc and aod; allowed for code snippets to
 * be empty ({}); allowed for semicolon after any snippet; redefined eos to
 * be any number > 0 of semicolons, followed by any number >= 0 of comments
 * 
 * Revision 1.6  88/12/19  14:21:42  little
 * add some checks to 
 * 
 * 
 * add some code to check for redeclaration of names within the same scope. this
 * code needed to bne smart enuf to "know" about forward references.
 * 
 * Revision 1.5  88/12/14  10:44:18  little
 * 
 * add xLEFTSQUARE xRIGHTSQUARE in anticipation of using them later on.
 * allow eve to have variables, objects, creation and destruction actions
 * start building dot notation trees in a way which will support implementation
 * in code generation.
 * 
 * Revision 1.4  88/10/31  15:05:26  little
 * change expression statement trees for attributes to have the
 * type of assignment_sym to differentiate between those and "real" snippet
 * trees.
 * 
 * Revision 1.3  88/10/25  10:31:42  little
 * move the id message to a place where yacc will accept it.
 * 
 * Revision 1.2  88/10/24  17:03:23  little
 * take when_modified production out of recognizer
 * 
|	01 Mar 1988:	module created
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

%{
static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/cay.y,v 1.20 89/10/23 20:00:16 ejh Exp $";

#define	memoryPack

#include	"global.h"	/* global definitions */
#include	<stdio.h>	/* for I/O */
#include	"listPack.h"	/* list handling stuff */
#include	"string.h"	/* string handling stuff */
#include	"idd.h"		/* to pick up id_types and data_definitions */

/*	SLANG specific includes */

#include	"u22.h"		/* Serpent universals */
#include	"cus.h"		/* SLANG symbol table stuff */
#include	"c22.h"		/* SLANG globals */
#include	"ca2.h"
#include	"cas.h"

cus_sym	current_vc,
	current_method,
	current_object,
	current_type,
	current_attribute,
	current_variable;

cus_sym	the_sym,
	this;

int	new;

extern	int	yylineno;

int	variable_lineno,
	av_lineno,
	method_lineno,
	snippet_lineno,
	if_lineno,
	loop_lineno;

LIST	vc_list,
	the_list,
	if_list,
	temp_cc_list,
	attribute_list;

STACK	the_stack,
	if_stack;

string	the_type,
	object_type_name;

int	the_length,
	the_offset,
	the_position;

int	num_vc, num_obj, num_meth, num_att, num_var, num_parms;

boolean	in_cc;

%}
/*--------------------------------------------------------------------------*\
|  Type: union
| 
|  Description:
|	union of types for yacc/lex
| 
\* Definition:                                                              */

	%union	{
		int		xint;
		string		xstring;
		char		*xcpointer;
		double		*xreal;
		LIST		xLIST;
		cus_sym		xcus_sym;
		idd_data_types	xidd_data_types;
		}

/*                                                                          *\
|
|  Components:
|	xint		for returning an integer for a token
|	xstring		for returning a character string
|	xcpointer	for returning a general purpose character pointer
|	xreal		for returning a pointer to a double (real)
|	xLIST		for returning a LIST
|	xcus_sym	for returning a cus_sym
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constants: all of the tokens (and those rules that need to be declared)
| 
|  Description:
|	these are all of the tokens and rules used by yacc for the CLEM
|	processor
| 
\* Definition:                                                              */

	%token	<xint>	xCOLON xDOT xCOMMA xSEMICOLON xAMPERSAND xBAR
	%token	<xint>	xLEFTPAREN xRIGHTPAREN
	%token	<xint>	xLEFTCURLY xRIGHTCURLY
	%token	<xint>	xLEFTSQUARE xRIGHTSQUARE
	%token	<xint>	xEQUAL xNOTEQUAL xGETS xLESSTHAN xLESSEQUAL
	%token	<xint>	xGREATERTHAN xGREATEREQUAL
	%token	<xint>	xPLUS xMINUS xSTAR xSLASH xPOWER

	%token	<xint>	xAND xATTRIBUTES xCREATIONCONDITION xDO xELSE xELSEIF
	%token	<xint>	xENDIF xENDVC xENDWHILE xEXTERNALS xFALSE xIF xMETHODS
	%token	<xint>	xNOT xNULL xOBJECTS xONCREATE xONDESTROY xOR xTHEN
	%token	<xint>	xTRUE xUNDEFINED xVARIABLES xVC xWHILE

	%token	<xint>	xADI xBOOLEAN xBUFFER xID xINTEGER xREAL xSTRING xVOID

	%token	<xint>	xINTEGERNUMBER

	%token	<xreal>	xREALNUMBER

	%token	<xstring>	xNAME xQUOTE xCOMMENT

/*	and now all the rules that need to be declared */

	%type	<xstring>	external_type

	%type	<xcpointer>	expression boolean_expression term factor id
	%type	<xcpointer>	arithmetic_expression signed_id expressions
	%type	<xcpointer>	opt_expressions qualified_name
	%type	<xcpointer>	relational_expression

	%type	<xcus_sym>	relational_operator logical_operator constant
	%type	<xcus_sym>	multiplication_operator unary_operator
	%type	<xcus_sym>	addition_operator equality_operator

	%type	<xcus_sym>	variable_declaration object_declaration
	%type	<xcus_sym>	boolean_constant attribute_value
	%type	<xcus_sym>	method_handler

	%type	<xidd_data_types>	parm

	%type	<xLIST>	variable_declarations opt_variable_declarations
	%type	<xLIST>	av_choice vv_choice opt_gets_exp object_declarations
	%type	<xLIST>	attribute_values opt_attribute_values
	%type	<xLIST>	method_handlers opt_method_handlers eos

	%type	<xLIST>	opt_code_snippet code_snippet opt_statements
	%type	<xLIST>	statements statement assignment_statement opt_else
	%type	<xLIST>	conditional_statement if_first opt_if_middle if_middle
	%type	<xLIST>	imperative_statement loop_statement boolean_condition
	%type	<xLIST> opt_first_expression opt_first_boolean_expression
	%type	<xLIST>	opt_first_arithmetic_expression opt_first_term
	%type	<xLIST> opt_first_factor opt_unary_operator
	%type	<xLIST>	opt_first_relational_expression
	%type	<xLIST>	opt_comments comments function_call

/*	%start isn't really necessary in this case, as yacc will take the
	first production it finds as the start, but it's a good habit to get
	into, especially as you can define multiple starting points */

	%start	slang_program

	%%

/*                                                                          *\
\*--------------------------------------------------------------------------*/

/*	unfortunately, the : and first choice must appear on the top line */

slang_program:	opt_comments
		{
		num_vc = 0;
		num_obj = 0;
		num_meth = 0;
		num_att = 0;
		num_var = 0;
		the_stack = make_stack ();
		if_stack = make_stack ();
		vc_list = make_list ();
		current_vc = car_god_vc;
		if ($1 != NULL)
		    cus_put_prop (current_vc, vc_comments_prop, $1);
		(void) add_to_tail (vc_list, current_vc);
		}
		opt_externals component_list opt_vcs
		{
		get_from_head (vc_list);	/* pop god from vc_list */
		cus_put_prop (car_god_vc, sub_vcs_prop, vc_list);
		DEBUG2 (fprintf (stdout, "Number of VCs = %d\n", num_vc);
		    fprintf (stdout, "Number of Objects = %d\n", num_obj);
		    fprintf (stdout, "Number of Methods = %d\n", num_meth);
		    fprintf (stdout, "Number of Attributes = %d\n", num_att);
		    fprintf (stdout, "Number of Variables = %d\n", num_var));
		}

opt_vcs:	/* empty string */

	|	vcs

	;

vcs:		vc

	|	vcs vc

	;

vc:		xVC xCOLON xNAME opt_comments
		{
		DEBUG3 (fprintf (stderr, "VC: %s\n", $3));
		current_vc = cus_add_sym ($3, vc_sym, &new, WITH_CONTEXT);
		cus_put_prop (current_vc, u_name_prop, cus_genname ("VC"));
		cus_put_prop (current_vc, parent_vc_prop,
		    get_list_head (vc_list));
		(void) add_to_tail (vc_list, current_vc);
		(void) push_stack (the_stack, vc_list);
		vc_list = make_list ();
		(void) add_to_tail (vc_list, current_vc);
		if ((! new) && (cus_get_prop (current_vc, user_defined_prop)))
		    c22_error (VC_EXISTS, $3, yylineno);
		cus_put_prop (current_vc, user_defined_prop, 1);
		cus_put_prop (current_vc, type_prop, vc_sym);
		}
		creation_condition
		{
		cus_put_prop (current_vc, owned_context_prop,
		    cus_create_nu_context ());
		DEBUG3 (fprintf (stderr, "VC: %s, context = %d\n", $3,
		    cus_current_context ()));
		}
		component_list opt_vcs xENDVC xNAME opt_eos
/*
??? need to check the end vc name is same as vc name
*/
		{
		DEBUG3 (fprintf (stderr, "END VC: %s\n", $11));
		if ($4 != NULL)
		    cus_put_prop (get_list_head (vc_list), vc_comments_prop,
			$4);
		cus_put_prop (get_from_head (vc_list), sub_vcs_prop, vc_list);
		vc_list = (LIST) pop_stack (the_stack);
		cus_deactivate_context ();
		num_vc++;
		}

opt_externals:	/* empty string */

	|	externals

	;

externals:	external

	|	externals external

	;

external:	xEXTERNALS xCOLON opt_external_declarations

opt_external_declarations:	/* empty string */

	|	external_declarations

	;

external_declarations:	external_declaration

	|	external_declarations external_declaration

	;

external_declaration:	external_type
		{
		num_parms = 0;
		}
		xNAME xLEFTPAREN
		{
		the_sym = cus_add_sym ($3, id_sym, &new, NO_CONTEXT);
		if (!new)
		    c22_error (FUNCTION_DEFINITION, $3, yylineno);
		cus_put_prop (the_sym, is_built_in_prop, 1);
		if (strcmp ($1, "void"))
		    {
		    the_type = strcpy (make_node (strlen ("dob_make_") +
			strlen ($1) + strlen ("_constant_adi") + 1),
			"dob_make_");
		    (void) strcat (the_type, $1);
		    (void) strcat (the_type, "_constant_adi");
		    cus_put_prop (the_sym,
			returned_value_conversion_function_prop, the_type);
		    }
		cus_put_prop (the_sym, c_function_name_prop, $3);
		the_list = make_list ();
		cus_put_prop (the_sym, parameter_conversion_functions_prop,
		    the_list);
		}
		opt_parm_list xRIGHTPAREN eos
		{
		cus_put_prop (the_sym, num_operands_prop, num_parms);
		}

external_type:	xBOOLEAN
		{
		$$ = u22_copy_string ("boolean");
		}

	|	xBUFFER
		{
		$$ = u22_copy_string ("buffer");
		}

	|	xID
		{
		$$ = u22_copy_string ("id");
		}

	|	xINTEGER
		{
		$$ = u22_copy_string ("integer");
		}

	|	xREAL
		{
		$$ = u22_copy_string ("real");
		}

	|	xSTRING
		{
		$$ = u22_copy_string ("string");
		}

	|	xVOID
		{
		$$ = u22_copy_string ("void");
		}

	;

opt_parm_list:	/* empty string */

	|	parm_list

	;

parm_list:	parm
		{
		(void) add_to_tail (the_list, $1);
		num_parms = num_parms + 1;
		}

	|	parm_list xCOMMA parm
		{
		(void) add_to_tail (the_list, $3);
		num_parms = num_parms + 1;
		}

	;

parm:		xADI
		{
		$$ = idd_no_data_type;
		}

	|	xBOOLEAN
		{
		$$ = idd_boolean;
		}

	|	xBUFFER
		{
		$$ = idd_buffer;
		}

	|	xID
		{
		$$ = idd_id;
		}

	|	xINTEGER
		{
		$$ = idd_integer;
		}

	|	xREAL
		{
		$$ = idd_real;
		}

	|	xSTRING
		{
		$$ = idd_string;
		}

	;

component_list:	opt_variables opt_objects opt_aoc opt_aod

creation_condition:	xCREATIONCONDITION
		{
		DEBUG3 (fprintf (stderr, "CC:\n"));
		cus_put_prop (current_vc, cc_used_vars_prop, make_list ());
		in_cc = TRUE;
		}
		xCOLON boolean_condition
		{
		cus_put_prop (current_vc, creation_condition_prop, $4);
		temp_cc_list = make_list ();
		add_to_head (temp_cc_list, $4);
		cam_fix_news (current_vc, temp_cc_list);
		free_list (temp_cc_list, NOT_NODES);
		cus_put_prop (current_vc, cc_lineno_prop, yylineno);
		in_cc = FALSE;
		}

opt_variables:	/* empty string */

	|	xVARIABLES
		{
		DEBUG3 (fprintf (stderr, "VARS:\n"));
		}
		xCOLON opt_variable_declarations
		{
		cus_put_prop (current_vc, variables_prop, $4);
		}

	;

opt_variable_declarations:	/* empty string */
		{
		$$ = NULL;
		}

	|	variable_declarations

	;

variable_declarations:	variable_declaration
		{
		the_list = make_list ();
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	|	variable_declarations variable_declaration
		{
		(void) add_to_tail ($1, $2);
		}

	;

variable_declaration:	xNAME
		{
		current_variable = cus_add_sym ($1, variable_sym, &new,
		    WITH_CONTEXT);
		cus_put_prop (current_variable, u_name_prop,
		    cus_genname ("VAR"));
		if ((! new) && (cus_get_prop (current_variable,
		    user_defined_prop)))
		    c22_error (VAR_EXISTS, $1, yylineno);
		cus_put_prop (current_variable, user_defined_prop, 1);
		cus_put_prop (current_variable, type_prop, variable_sym);
		variable_lineno = yylineno;
		}
		vv_choice
		{
		cus_put_prop (current_variable, parent_vc_prop, current_vc);
		if ($3 != NULL)
		    cus_put_prop (current_variable,
			initial_value_snippet_prop, $3);
		$$ = current_variable;
		num_var++;
		}

vv_choice:	xCOLON code_snippet
		{
		if ($2 != NULL)
		    {
		    the_sym = cus_gensym ("initial_value_snippet",
			snippet_sym);
		    (void) add_to_head_val ($2, the_sym, variable_lineno);
		    }
		$$ = $2;
		}

	|	opt_gets_exp eos
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    the_sym = cus_gensym ("initial_value_snippet",
			 snippet_sym);
		    (void) add_to_tail_val (the_list, the_sym,
			variable_lineno);
		    (void) add_to_tail (the_list, $1);
		    $$ = the_list;
		    }
		if ($2 != NULL)
		    cus_put_prop (current_variable, variable_comments_prop,
			$2);
		}

	;

opt_gets_exp:	/* empty string */
		{
		$$ = NULL;
		}

	|	xCOLON
		{
		DEBUG3 (fprintf (stderr, "COLON:\n"));
		}
		expression
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_gets_symbol, yylineno);
		(void) add_to_tail (the_list, current_variable);
		(void) add_to_tail (the_list, $3);
		$$ = the_list;
		}

	;

opt_objects:	/* empty string */

	|	objects

	;

objects:	xOBJECTS
		{
		DEBUG3 (fprintf (stderr, "OBJECTS:\n"));
		}
		xCOLON object_declarations
		{
		cus_put_prop (current_vc, objects_prop, $4);
		}

object_declarations:	object_declaration
		{
		the_list = make_list ();
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	|	object_declarations object_declaration
		{
		(void) add_to_tail ($1, $2);
		}

	;

object_declaration:	xNAME
		{
		DEBUG3 (fprintf (stderr, "OBJECT DECLARATION: %s\n", $1));
		current_object = cus_add_sym ($1, object_sym, &new,
		    WITH_CONTEXT);
		cus_put_prop (current_object, u_name_prop,
		    cus_genname ("OBJ"));
		if ((! new) && (cus_get_prop (current_object,
		    user_defined_prop)))
		    c22_error (OBJECT_EXISTS, $1, yylineno);
		cus_put_prop (current_object, user_defined_prop, 1);
		cus_put_prop (current_object, type_prop, object_sym);
		cus_put_prop (current_object, owned_context_prop,
		    cus_create_nu_context ());
                DEBUG3 (fprintf (stderr, "OBJECT: %s, context = %d\n", $1,
		    cus_current_context ()););
		cus_put_prop (current_object, parent_vc_prop, current_vc);
		}
		xCOLON object_type
		{
		(void) cus_deactivate_context ();
		$$ = current_object;
		num_obj++;
		}

object_type:	xNAME opt_comments xLEFTCURLY
		{
		DEBUG3 (fprintf (stderr, "OBJECT TYPE\n"));
		if ($2 != NULL)
		    cus_put_prop (current_object, object_comments_prop, $2);
		object_type_name = u22_copy_string ($1);
		this = ca2_get_sd_sym ($1);
		if (this == NULL)
		    c22_error (UNKNOWN_OBJECT_TYPE, $1, yylineno);
		else
		    cus_put_prop (current_object, data_definition_prop, this);
/*
?? should the object_type use cus_add_sym or cus_look_ups and then cus_add_sym
*/
		current_type = cus_add_sym ($1, sd_sym, &new, NO_CONTEXT);
		cus_put_prop (current_object, object_type_prop, current_type);
		}
		opt_attributes opt_methods xRIGHTCURLY

opt_attributes:	/* empty string */

	|	xATTRIBUTES
		{
		DEBUG3 (fprintf (stderr, "ATTRIBUTES:\n"));
		}
		xCOLON opt_attribute_values
		{
		if ($4 != NULL)
		    cus_put_prop (current_object, attributes_prop, $4);
		}

	;

opt_attribute_values:	/* empty string */
		{
		$$ = NULL;
		}

	|	attribute_values

	;

attribute_values:	attribute_value
		{
		the_list = make_list ();
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	|	attribute_values attribute_value
		{
		(void) add_to_tail ($1, $2);
		}

	;

attribute_value:	xNAME xCOLON
		{
		DEBUG3 (fprintf (stderr, "\t%s\n", $1));
		av_lineno = yylineno;
		current_attribute = cus_add_sym ($1, attribute_sym, &new,
		    WITH_CONTEXT);
		if (!new)
		    cus_put_prop (current_attribute, type_prop,
			attribute_sym);
		cus_put_prop (current_attribute, u_name_prop,
		    cus_genname ("ATT"));
		}
		av_choice
		{
		cus_put_prop (current_attribute, value_snippet_prop, $4);
		cus_put_prop (current_attribute, parent_object_prop,
		    current_object);
		if (this == NULL ||
		    (!idd_validate_component ((idd_data_definition)
			    cus_get_prop (this, idd_handle_prop),
			    object_type_name,
			    $1)))
		       c22_error (UNKNOWN_ATTRIBUTE, $1, av_lineno);
		$$ = current_attribute;
		num_att++;
		}

av_choice:	code_snippet
		{
		if ($1 != NULL)
		    {
		    the_sym = cus_gensym ("av_code_snippet", snippet_sym);
		    (void) add_to_head_val ($1, the_sym, av_lineno);
		    }
		}

	|	expression eos
		{
		attribute_list = make_list ();
				/* use a symbol type of assignment_sym so
				   we can tell the difference between this
			 	   and the full blown code snippet version
				   later on in code generation */
		the_sym = cus_gensym ("av_expression_snippet",
		    assignment_sym);
		(void) add_to_tail_val (attribute_list, the_sym, av_lineno);
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_gets_symbol, av_lineno);
		(void) add_to_tail (the_list, current_attribute);
		(void) add_to_tail (the_list, $1);
		(void) add_to_tail (attribute_list, the_list);
		if ($2 != NULL)
		    (void) append_list (attribute_list, $2);
		$$ = attribute_list;
		}

	;

opt_methods:	/* empty string */

	|	xMETHODS
		{
		DEBUG3 (fprintf (stderr, "METHODS:\n"));
		}
		xCOLON opt_method_handlers
		{
		if ($4 != NULL)
		    cus_put_prop (current_object, methods_prop, $4);
		}

	;

opt_method_handlers:	/* empty string */
		{
		$$ = NULL;
		}

	|	method_handlers

	;

method_handlers:	method_handler
		{
		the_list = make_list ();
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	|	method_handlers method_handler
		{
		(void) add_to_tail ($1, $2);
		}

	;

method_handler:	xNAME xCOLON
		{
		DEBUG3 (fprintf (stderr, "\t%s\n", $1));
		method_lineno = yylineno;
		}
		code_snippet
		{
		the_sym = cus_gensym ("method_handler_snippet", snippet_sym);
		if ($4 != NULL)
		    (void) add_to_head_val ($4, the_sym, method_lineno);
		current_method = cus_add_sym ($1, id_sym, &new, WITH_CONTEXT);
		cus_put_prop (current_method, u_name_prop,
		    cus_genname ("METH"));
		cus_put_prop (current_method, value_snippet_prop, $4);
		cus_put_prop (current_method, parent_object_prop,
		    current_object);
		$$ = current_method;
		num_meth++;
		}

opt_aoc:	/* empty string */

	|	xONCREATE
		{
		DEBUG3 (fprintf (stderr, "ON CREATE\n"));
		}
		xCOLON opt_code_snippet
		{
		if ($4 != NULL)
		    {
		    the_sym = cus_gensym ("create_actions_snippet",
			snippet_sym);
		    (void) add_to_head_val ($4, the_sym, snippet_lineno);
		    cus_put_prop (current_vc, create_actions_snippet_prop,
			$4);
		    }
		}

	;

opt_aod:	/* empty string */

	|	xONDESTROY
		{
		DEBUG3 (fprintf (stderr, "ON DESTROY\n"));
		}
		xCOLON opt_code_snippet
		{
		if ($4 != NULL)
		    {
		    the_sym = cus_gensym ("destroy_actions_snippet",
			snippet_sym);
		    (void) add_to_head_val ($4, the_sym, snippet_lineno);
		    cus_put_prop (current_vc, destroy_actions_snippet_prop,
			$4);
		    }
		}

	;

opt_code_snippet:	/* empty string */
		{
		$$ = NULL;
		}

	|	code_snippet

	;

code_snippet:	xLEFTCURLY
		{
		DEBUG3 (fprintf (stderr, "SNIPPET\n"));
		}
		opt_comments opt_statements xRIGHTCURLY opt_eos
		{
/*
		if ($3 != NULL)
		    {
		    if ($4 != NULL)
			(void) add_to_tail ($3, $4);
		    $$ = $3;
		    }
		else
		    $$ = $4;
*/
		if ($3 != NULL && $4 != NULL)
			(void) add_to_head ($4, $3);
		$$ = $4;
		}

opt_statements:	/* empty string */
		{
		$$ = NULL;
		}

	|	statements

	;

statements:	statement eos
		{
		snippet_lineno = yylineno;
		the_list = make_list ();
		(void) add_to_tail (the_list, $1);
		if ($2 != NULL)
		    (void) add_to_tail (the_list, $2);
		$$ = the_list;
		}

	|	statements statement eos
		{
		(void) add_to_tail ($1, $2);
		if ($3 != NULL)
		    (void) add_to_tail (the_list, $3);
		}

	;

statement:	conditional_statement

	|	assignment_statement

	|	loop_statement

	|	imperative_statement

	;

conditional_statement:	if_first opt_if_middle opt_else xENDIF
		{
		the_list = make_list ();
		(void) append_list (the_list, $1);
		if ($2 != NULL)
		    (void) add_to_tail (the_list, $2);
		$$ = the_list;
		}

if_first:	xIF
		{
		DEBUG3 (fprintf (stderr, "IF THEN\n"));
		if_lineno = yylineno;
		}
		boolean_condition xTHEN statements
		{
		if_list = make_list ();
		(void) add_to_head (if_list, $5);
		(void) add_to_head (if_list, $3);
		(void) add_to_head_val (if_list, car_if_symbol, if_lineno);
		$$ = if_list;
		}

opt_if_middle:	/* empty string */
		{
		$$ = NULL;
		}

	|	if_middle ;


if_middle:	xELSEIF
		{
		DEBUG3 (fprintf (stderr, "ELSE IF\n"));
		if_lineno = yylineno;
		}
		boolean_condition xTHEN statements
		{
		if_list = make_list ();
		(void) add_to_tail_val (if_list, car_if_symbol, if_lineno);
		(void) add_to_tail (if_list, $3);
		(void) add_to_tail (if_list, $5);
		the_list = make_list ();
		(void) add_to_tail (the_list, if_list);
		$$ = the_list;
		}
	|	if_middle xELSEIF
		{
		DEBUG3 (fprintf (stderr, "complex ELSE IF\n"));
		if_lineno = yylineno;
		}
		boolean_condition xTHEN statements
		{
		the_list = make_list ();
		(void) add_to_tail (if_list, the_list);
		if_list = make_list ();
		(void) add_to_tail_val (if_list, car_if_symbol, if_lineno);
		(void) add_to_tail (if_list, $4);
		(void) add_to_tail (if_list, $6);
		(void) add_to_tail (the_list, if_list);
		} ;


opt_else:	/* empty string */
		{
		$$ = NULL;
		}
	|	xELSE
		{
		DEBUG3 (fprintf (stderr, "ELSE\n"));
		(void) push_stack (if_stack, if_list);
		}
		statements
		{
		if_list = (LIST) pop_stack (if_stack);
		(void) add_to_tail (if_list, $3);
		} ;


assignment_statement:	qualified_name xGETS
		{
		DEBUG3 (fprintf (stderr, "ASSIGN:\n"));
		}
		expression
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_gets_symbol, yylineno);
		(void) add_to_tail (the_list, $1);
		(void) add_to_tail (the_list, $4);
		$$ = the_list;
		}

imperative_statement:	function_call

loop_statement:	xWHILE
		{
		DEBUG3 (fprintf (stderr, "WHILE\n"));
		loop_lineno = yylineno;
		}
		boolean_condition opt_xDO statements xENDWHILE
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_while_symbol,
		    loop_lineno);
		(void) add_to_tail (the_list, $3);
		(void) add_to_tail (the_list, $5);
		$$ = the_list;
		}

opt_xDO:	/* empty string */

	|	xDO

	;

boolean_condition:	xLEFTPAREN
		{
		DEBUG3 (fprintf (stderr, "BOOL COND:\n"));
		}
		expression xRIGHTPAREN
		{
		$$ = (LIST) $3;
		}

expression:	opt_first_expression boolean_expression
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_first_expression:	/* empty string */
		{
		$$ = NULL;
		}

	|	expression logical_operator
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, $2, yylineno);
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	;

boolean_expression:	opt_first_boolean_expression relational_expression
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_first_boolean_expression:	/* empty string */
		{
		$$ = NULL;
		}

	|	boolean_expression equality_operator
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, $2, yylineno);
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	;

relational_expression:	opt_first_relational_expression arithmetic_expression
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_first_relational_expression:	/* empty string */
		{
		$$ = NULL;
		}

	|	relational_expression relational_operator
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, $2, yylineno);
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	;

arithmetic_expression:	opt_first_arithmetic_expression term
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_first_arithmetic_expression:	/* empty string */
		{
		$$ = NULL;
		}

	|	arithmetic_expression addition_operator
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, $2, yylineno);
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	;

term:		opt_first_term factor
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_first_term:	/* empty string */
		{
		$$ = NULL;
		}

	|	term multiplication_operator
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, $2, yylineno);
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	;

factor:		opt_first_factor signed_id
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_first_factor:	/* empty string */
		{
		$$ = NULL;
		}

	|	factor xPOWER
		{
		DEBUG3 (fprintf (stderr, "POWER\n"));
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_power_symbol, yylineno);
		(void) add_to_tail (the_list, $1);
		$$ = the_list;
		}

	;

signed_id:	opt_unary_operator id
		{
		if ($1 != NULL)
		    {
		    the_list = make_list ();
		    (void) append_list (the_list, $1);
		    (void) add_to_tail (the_list, $2);
		    $$ = (char *) the_list;
		    }
		else
		    $$ = $2;
		}

opt_unary_operator:	/* empty string */
		{
		$$ = NULL;
		}

	|	unary_operator
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, $1, yylineno);
		$$ = the_list;
		}

	;

id:		qualified_name
		{
		if (in_cc)
		    (void) add_to_tail (cus_get_prop (current_vc,
			cc_used_vars_prop), $1);
		}

	|	constant
		{
		$$ = (char *) $1;
		}

	|	xLEFTPAREN
		{
		DEBUG3 (fprintf (stderr, "ID in parens\n"));
		}
		expression xRIGHTPAREN
		{
		$$ = $3;
		}

	|	function_call
		{
		$$ = (char *) $1;
		}

	;

qualified_name:	xNAME
		{
		DEBUG3 (fprintf (stderr, "NAME: %s\n", $1));
		if (!(the_sym = cus_look_up ($1)))
/*
?? it mite be an error (semantic?) if not already in the symbol table 
*/
		    the_sym = cus_add_sym ($1, id_sym, &new, WITH_CONTEXT);
		$$ = (char *) the_sym;
		}

	|	xNAME xDOT qualified_name
		{
		DEBUG3 (fprintf (stderr, "DOTNAME: %s.%s\n", $1, $2));
		the_list = make_list ();
		(void) add_to_tail (the_list, car_dot_symbol);
		if (!(the_sym = cus_look_up ($1)))
/*
?? it mite be an error (semantic?) if not already in the symbol table 
*/
		    the_sym = cus_add_sym ($1, id_sym, &new, WITH_CONTEXT);
		(void) add_to_tail (the_list, the_sym);
		(void) append_list (the_list, cas_fix_dot_tree ($3));
		$$ = (char *) the_list;
		}

	;

constant:	xINTEGERNUMBER
		{
		DEBUG3 (fprintf (stderr, "INTEGER: %d\n", $1));
		the_sym = cus_gensym ("integer", id_sym);
		cus_put_prop (the_sym, is_constant_prop, 1);
		cus_put_prop (the_sym, current_type_prop, idd_integer);
		cus_put_prop (the_sym, current_value_prop, $1);
		$$ = the_sym;
		}

	|	xREALNUMBER
		{
		DEBUG3 (fprintf (stderr, "REAL: %.5f\n", *$1));
		the_sym = cus_gensym ("real", id_sym);
		cus_put_prop (the_sym, is_constant_prop, 1);
		cus_put_prop (the_sym, current_type_prop, idd_real);
		cus_put_prop (the_sym, current_value_prop, $1);
		$$ = the_sym;
		}

	|	boolean_constant

	|	xQUOTE
		{
		DEBUG3 (fprintf (stderr, "QUOTE: %s\n", $1));
		$$ = cus_string_constant ($1);
		}

	|	xUNDEFINED
		{
		DEBUG3 (fprintf (stderr, "UNDEFINED\n"));
		$$ = car_undefined_symbol;
		}

	|	xNULL
		{
		DEBUG3 (fprintf (stderr, "NULL\n"));
		$$ = car_null_symbol;
		}

	;

boolean_constant:	xTRUE
		{
		DEBUG3 (fprintf (stderr, "TRUE\n"));
		$$ = car_true_symbol;
		}

	|	xFALSE
		{
		DEBUG3 (fprintf (stderr, "FALSE\n"));
		$$ = car_false_symbol;
		}

	;

function_call:	xNAME
		{
		DEBUG3 (fprintf (stderr, "FUNCTION: %s\n", $1));
		}
		xLEFTPAREN opt_expressions xRIGHTPAREN
		{
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_function_call_symbol,
		    yylineno);
/*
?? mite not want to an add , if already there (preloaded built-ins)
*/
		(void) add_to_tail (the_list, cus_add_sym ($1, id_sym, &new,
		    NO_CONTEXT));
		if ($4 != NULL)
		    (void) append_list (the_list, $4);
		$$ = the_list;
		}

opt_expressions:	/* empty string */
		{
		$$ = NULL;
		}

	|	expressions

	;

expressions:	expression
		{
		the_list = make_list ();
		(void) add_to_tail (the_list, $1);
		$$ = (char *) the_list;
		}

	|	expressions xCOMMA expression
		{
		(void) add_to_tail ($1, $3);
		}

	;

logical_operator:	xAND
		{
		DEBUG3 (fprintf (stderr, "AND\n"));
		$$ = car_and_symbol;
		}

	|	xAMPERSAND
		{
		DEBUG3 (fprintf (stderr, "AMPERSAND\n"));
		$$ = car_and_symbol;
		}

	|	xOR
		{
		DEBUG3 (fprintf (stderr, "OR\n"));
		$$ = car_or_symbol;
		}

	|	xBAR
		{
		DEBUG3 (fprintf (stderr, "BAR\n"));
		$$ = car_or_symbol;
		}

	;

equality_operator:	xEQUAL
		{
		DEBUG3 (fprintf (stderr, "EQUALS\n"));
		$$ = car_equal_symbol;
		}

	|	xNOTEQUAL
		{
		DEBUG3 (fprintf (stderr, "NOT EQUAL\n"));
		$$ = car_notequal_symbol;
		}

	;

relational_operator:	xLESSTHAN
		{
		DEBUG3 (fprintf (stderr, "LESS THAN\n"));
		$$ = car_lessthan_symbol;
		}

	|	xLESSEQUAL
		{
		DEBUG3 (fprintf (stderr, "LESS OR EQUAL\n"));
		$$ = car_lessequal_symbol;
		}

	|	xGREATERTHAN
		{
		DEBUG3 (fprintf (stderr, "GREATER THAN\n"));
		$$ = car_greaterthan_symbol;
		}

	|	xGREATEREQUAL
		{
		DEBUG3 (fprintf (stderr, "GREATER OR EQUAL\n"));
		$$ = car_greaterequal_symbol;
		}

	;

addition_operator:	xPLUS
		{
		DEBUG3 (fprintf (stderr, "PLUS\n"));
		$$ = car_plus_symbol;
		}

	|	xMINUS
		{
		DEBUG3 (fprintf (stderr, "MINUS\n"));
		$$ = car_binary_minus_symbol;
		}

	;

multiplication_operator:	xSTAR
		{
		DEBUG3 (fprintf (stderr, "TIMES\n"));
		$$ = car_star_symbol;
		}

	|	xSLASH
		{
		DEBUG3 (fprintf (stderr, "DIVIDED BY\n"));
		$$ = car_slash_symbol;
		}

	;

unary_operator:	xNOT
		{
		DEBUG3 (fprintf (stderr, "NOT\n"));
		$$ = car_not_symbol;
		}

	|	xMINUS
		{
		DEBUG3 (fprintf (stderr, "MINUS (unary)\n"));
		$$ = car_unary_minus_symbol;
		}

	;

opt_eos:	/* empty string */

	|	eos

	;

eos:		semi opt_comments
		{
		$$ = $2;
		}

semi:		xSEMICOLON

	|	semi xSEMICOLON

	;

opt_comments:	/* empty string */
		{
		$$ = NULL;
		}

	|	comments

	;

comments:	xCOMMENT
		{
		DEBUG3 (fprintf (stderr, "COMMENT: %s\n", $1));
		the_list = make_list ();
		(void) add_to_tail (the_list, car_comment_symbol);
		the_sym = cus_gensym ("comment", comment_sym);
		cus_put_prop (the_sym, is_constant_prop, 1);
		cus_put_prop (the_sym, current_type_prop, idd_string);
		cus_put_prop (the_sym, current_value_prop, $1);
		(void) add_to_tail_val (the_list, the_sym, yylineno);
		$$ = the_list;
		}

	|	comments xCOMMENT
		{
		DEBUG3 (fprintf (stderr, "COMMENT: %s\n", $2));
		the_list = make_list ();
		(void) add_to_tail_val (the_list, car_comment_symbol,
		    yylineno);
		the_sym = cus_gensym ("comment", comment_sym);
		cus_put_prop (the_sym, is_constant_prop, 1);
		cus_put_prop (the_sym, current_type_prop, idd_string);
		cus_put_prop (the_sym, current_value_prop, $2);
		(void) add_to_tail (the_list, the_sym);
		(void) append_list ($1, the_list);
		}

	;

	%%
