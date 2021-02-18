/*      $Header: /u/04c/mnt/new_integ/utilities/include/RCS/cus_priv.h,v 1.1 89/10/07 23:48:54 serpent Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  cus_priv.h
|
|  Version: 1.0
|
|  Change History:
|	9 jun 88 - add the cus_property_type_print_value array
|	10 may 88 - initial coding
|
|  Description:  
|	the package internal header file for the compiler symbol table package 
|
|  Component of:
|	compiler symbol table package
|
|  Related files:
|	cus.c - compiler symbol package routines
|	cus.h - the header file utilized by users of the symbol table package
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
$Log:	cus_priv.h,v $
 * Revision 1.1  89/10/07  23:48:54  serpent
 * Initial revision
 * 
 * Revision 1.21  89/09/29  15:36:52  little
 *  add name_text_prop, this is getting boring
 * 
 * Revision 1.20  89/09/29  14:09:28  little
 * add creation_condition_text_prop
 * 
 * Revision 1.19  89/09/29  14:02:43  little
 * add the comment text props
 * 
 * Revision 1.18  89/09/28  14:23:56  little
 * add the text snippet properties
 * 
 * Revision 1.17  89/09/26  18:09:44  bmc
 * Added properties for dialogue editor application.
 * 
 * Revision 1.16  89/07/25  21:42:30  bmc
 * Added sd_template_prop
 * 
 * Revision 1.15  89/06/16  17:38:26  little
 * move the props added in the last change to the end of the list
 * 
 * Revision 1.14  89/06/16  16:37:59  bmc
 * Added potential_vcs_prop and saved_sne_prop to cus_property_type_print_value
 * array.
 * 
 * Revision 1.13  89/06/16  16:29:43  bmc
 * Checked out to test a CM problem, at Mr. CM's request.
 * 
 * Revision 1.12  89/06/09  14:29:46  little
 * add template, current_type, current_value, & original_name slots
 *   to symbol structure
 * add defintions for cus_prop_sets & cus_prop_gets which are arrays which
 *   are used to check count of prop usages
 * 
 * Revision 1.11  89/05/11  15:43:18  little
 * put the redefined_prop into the sy,bol structure (it used to be on the 
 * property list)
 * 
 * Revision 1.10  89/04/07  17:48:45  ljb
 * add properties for saving shared data info across vcs
 * and for user defined functions
 * 
 * Revision 1.9  89/04/03  16:41:32  little
 * add the snippet_text_prop, which is used by the editor to keep the
 * text form of a snippet.
 * 
 * Revision 1.8  89/02/26  10:44:04  little
 * add cc_lineno_prop
 * 
 * Revision 1.7  89/02/10  14:32:04  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/02/01  16:24:12  little
 * add alternate_print_string_prop
 * 
 * Revision 1.5  89/01/26  16:31:38  little
 * delete some entries from property name array to bring it back into
 * correspondance with the property enum in cus.h
 * 
 * Revision 1.4  89/01/18  14:21:18  ejh
 * new ill file preamble properties
 * 
 * Revision 1.3  89/01/17  16:30:19  little
 * add bound_elem_sym and bound_comp_sym to cus_symbol_type_print_value
 * 
 * Revision 1.2  88/12/20  13:15:50  little
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

#ifndef cus_private
#define cus_private
#define memoryPack              /* use memoryPack rather than mallocPack    */


/*--------------------------------------------------    SYMBOLIC CONSTANTS */

#define HASH_SIZE 1013
#define BASE_CONTEXT 1
#define NO_CONTEXT_CONTEXT 0
#define SYM_IDENTIFIER 0X2AAA

/*-----------------------------------------------------     TYPEDEFS */

/*--------------------------------------------------------------------------*\
|  Description:
|	structure for keeping the symbol.  it is instantiations of this
|	structure which are placed into the hash table
\* Definition:                                                              */

	typedef struct cus_name_bucket
	   {char* name;
	   cus_sym sym;
	   LIST outer_contexts;
	   LIST inner_contexts;
	   }  cus_name_bucket;

/*--------------------------------------------------------------------------*\
|  Description:
|	pointer for the bucket structure
\* Definition:                                                              */

	typedef struct cus_name_bucket *cus_name_bucket_ptr;


/*--------------------------------------------------------------------------*\
|  Description:
|	actual symbol concrete representation.  note that the properties type,
|	template, current_type, current_value, original_name, & redefined are
|	actually kept here instead of on the property list.
|	
\* Definition:                                                              */

	typedef struct cus_sym_entry
	   {int ident;
           char* name;
	   cus_name_bucket_ptr bucket;
	   cus_sym_type type;
           caddr_t redefined;
           caddr_t template;
           caddr_t current_type;
           caddr_t current_value;
           caddr_t original_name;
	   cus_context_type context;
	   LIST properties;
	   } cus_sym_entry;


/*--------------------------------------------------------------------------*\
|  Description:
|	concrete represenation of a property.  the property names in the
|	property list are represented as zimmerman list tag values.  the tag
|	value is simply the property name enum value.  an item of type
|	cus_priv_property is what is stored in the
|	list element as the property value.  it is done this way instead of
|	just putting the property value straight into the list because there
|	is no zimmerman function to change the list element associated with
|	a value tag.  so when changing a property value, we have to get the
|	list element (which is really a pointer to the property value {which
|	itself could be a pointer}) and then change it to point to the new
|	value.
\* Definition:                                                              */

	typedef struct property
	   {char *the_value;
	   } cus_priv_property;


/*--------------------------------------------------------------------------*\
|  Description:
|	pointer to a property
\* Definition:                                                              */

	typedef cus_priv_property *cus_priv_property_ptr;


/*--------------------------------------------------------------------------*\
|  Description:
|	concrete representation for the symbol table
\* Definition:                                                              */

	typedef struct 
	   {int tag;
	   cus_context_type current_context;
	   HASH the_table;
	   LIST old_contexts;
	   }  cus_table_type;


/* --------------------------------------------------- MACRO DEFINITIONS */
#define NULLst_sym ((cus_sym) 0)
#define NULLst ((cus_table_type*) 0)
#endif

/*-----------------------------------------------------     STATIC DATA */


/*--------------------------------------------------------------------------*\
|  Description:  
|     the symbol type print values
|
| 
\* Definition:                                                              */

static char *cus_symbol_type_print_value [] = {
   "null_sym",
   "vc_sym",
   "object_sym",
   "attribute_sym",
   "function_sym",
   "function_call_sym",
   "variable_sym",
   "snippet_sym",
   "operator_sym",
   "if_sym",
   "while_sym",
   "comment_sym",
   "ill_data_definition_sym",
   "assignment_sym",
   "id_sym",
   "sd_sym",
   "temp_sym",
   "constant_sym",
   "ill_sym",
   "bound_elem_sym",
   "bound_comp_sym"
	 };



/*--------------------------------------------------------------------------*\
|  Description:  
|     the property names print values
| 
\* Definition:                                                              */

static char *cus_property_type_print_value [] = {
	"instances_prop",
	"template_prop",
	"parent_vc_prop",
	"objects_prop",
	"variables_prop",
	"sub_vcs_prop",
	"creation_condition_prop",
	"methods_prop",
	"create_actions_snippet_prop",
	"destroy_actions_snippet_prop",
	"type_prop",
	"attributes_prop",
	"constant_value_prop",
	"value_snippet_prop",
	"modified_snippet_prop",
	"current_type_prop",
	"current_value_prop",
	"initial_value_snippet_prop",
	"s_tree_prop",
	"set_vars_prop",
	"used_vars_prop",
	"op_type_prop",
	"owned_context_prop",
	"result_type_prop",
	"c_function_name_prop",
	"num_operands_prop",
	"operator_type_prop",
	"data_definition_prop",
	"add_operator_type",
	"subtract_operator_type",
	"object_type_prop", /* 30 */
	"mailbox_prop",
	"is_reserved_word_prop",
        "is_constant_prop",
	"is_comment_prop",
	"vc_comments_prop",
        "variable_comments_prop",
	"object_comments_prop",
        "cc_used_vars_prop",
        "ask_prop",
	"u_name_prop",
	"fixed_method_snippet_prop",
	"parent_object_prop",
	"c_proc_number_prop",
	"fixed_creation_condition_prop",
	"generic_magic_id_prop",
	"specific_magic_id_prop",
        "ill_file_name_prop",
	"mailbox_name_prop",
	"execution_name_prop",
        "exec_command_prop",
        "idd_handle_prop",
	"id_prop",
	"ill_files_prop",
	"taps_prop",
        "original_name_prop",
        "creating_sd_prop",
	"vce_prop",
	"is_built_in_prop",
	"fixed_method_prop",
	"is_ok_id_name_prop",
	"created_vcs_prop",
	"old_cc_used_vars_prop",
	"pet_proc_name_prop",
	"cc_uses_new_prop",
	"user_defined_prop",
	"redefined_prop",
	"alternate_print_string_prop",
	"cc_lineno_prop",
	"binary_operator_prop",
	"unary_operator_prop",
	"snippet_text_prop",
        "old_created_vcs_prop",
        "old_parent_vc_prop",
        "old_owned_context_prop",
        "parameter_conversion_functions_prop",
        "returned_value_conversion_function_prop",
	"potential_vcs_prop",
	"saved_sne_prop",
        "sd_template_prop",
	"file_name_prop",
	"prologue_comments_prop",
	"macros_prop",
	"externs_prop",
	"technologies_prop",
	"shared_data_prop",
	"value_snippet_text_prop",
	"create_actions_snippet_text_prop",
	"destroy_actions_snippet_text_prop",
 	"initial_value_snippet_text_prop",
	"vc_comments_text_prop",
	"variable_comments_text_prop",
	"object_comments_text_prop",
	"creation_condition_text_prop",
	"name_text_prop",

/*
        ADD ALL PROPERTY PRINT VALUES BEFORE THIS ONE
*/
	"THIS IS THE NONEXISTANT PROPERTY, OOOOPS!!!"
	 };			/* END */

/*--------------------------------------------------------------------------*\
|  Description:
|     array containing counts of number of times each property has been set
|
\* Definition:                                                              */

				/* rest of array is initailized to 0 also */
static int cus_prop_sets [last_prop_DO_NOT_USE + 1] =  {0};


/*--------------------------------------------------------------------------*\
|  Description:
|     array containing counts of number of times each property has been fetched
|
\* Definition:                                                              */

				/* rest of array is initailized to 0 also */
static int cus_prop_gets [last_prop_DO_NOT_USE + 1]  = {0};


/* there are definition here for both where the data actually has space
   allocated and where the data is referenced via extern declarations */

#ifdef cus_top  /* data space declaration */
int cus_gensym_seed = 0;
cus_table_type *cus_sym_tab = NULLst;
cus_context_type cus_context_seed = BASE_CONTEXT;

#else   /* data reference via externs */

extern int cus_gensym_seed;
extern cus_table_type *cus_sym_tab;
extern cus_context_type cus_context_seed;
#endif


