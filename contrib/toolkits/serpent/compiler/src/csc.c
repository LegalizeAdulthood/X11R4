static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/csc.c,v 1.30 89/10/23 19:53:26 ejh Exp $";

/*
$Log:	csc.c,v $
 * Revision 1.30  89/10/23  19:53:26  ejh
 * defined memoryPack
 * 
 * Revision 1.29  89/09/01  11:04:21  little
 * add name of thingiee (vc, obj, var, att) to runtime error printe striung
 * 
 * Revision 1.27  89/07/24  14:53:22  little
 * ops removal
 * 
 * Revision 1.26  89/05/26  10:49:37  little
 * change generated c code to use the new sne structure
 * 
 * Revision 1.25  89/05/22  17:23:29  bmc
 * Changed some of the generated code to accomodate changes in DM run-time
 * routines.
 * 
 * Revision 1.24  89/04/28  14:33:28  ljb
 * user defined functions debugged
 * new routine read_func_defs added
 * 
 * Revision 1.23  89/04/27  11:16:18  little
 * fix up generated c code for creation condition so everything is
 * correctly done wrt when the creation is true or false (move the
 * call to Ops_is_remove up "inside" the condtion).
 * 
 * Revision 1.22  89/04/25  09:15:51  ljb
 * generate calls to user defined functions
 * (this code hasn't been tested yet, I am checking this in
 * so that Reed can fix another bug)
 * 
 * Revision 1.21  89/04/07  17:49:56  ljb
 * save shared data information across vcs
 * 
 * Revision 1.20  89/03/28  12:54:18  serpent
 * changed  #ifdef 0  to  #if 0  to allow sun compile.
 * 
 * Revision 1.19  89/03/15  13:56:09  rcs
 * no changes
 * 
 * Revision 1.18  89/03/08  12:01:28  little
 * make the routine identification comments output in c code
 * not dependent on c22_output_debug_code
 * 
 * Revision 1.17  89/02/27  17:08:06  little
 * add some debug print
 * 
 * Revision 1.16  89/02/15  17:37:41  little
 * take out all to do with the "modified" file
 * 
 * Revision 1.15  89/02/09  16:30:25  ejh
 * added copyright notice
 * 
 * Revision 1.14  89/02/01  16:20:40  little
 * add test to see if there is a call to new in cc and compiler
 * found no "creating" sd; this is a compiler internal error
 * 
 * Revision 1.13  89/01/30  14:17:36  little
 * fix up so that the routine pet routine definition parameter list is
 * built correctly.
 * 
 * Revision 1.12  89/01/26  17:33:06  little
 * fix some debug statments which were at the wrong level
 * 
 * Revision 1.11  89/01/17  16:54:37  little
 * add some new properties for the "bound" sd data item when it
 * is present in a creation condition.
 * 
 * Revision 1.10  89/01/11  14:41:16  little
 * add generation of call to Ops_is_remove to the else of the
 * check of the creation condition
 * 
 * Revision 1.9  88/12/14  10:59:20  little
 * just a changes to some comments
 * 
 * Revision 1.8  88/12/04  13:24:53  little
 * fix the generated code fot the slang while loopp so that dro_boolean
 * is called in the condition.
 * 
 * Revision 1.7  88/11/08  14:06:07  little
 * generate code at the edn of each snippet to set d22_slang_source to null
 * string, so debug code "in between" snippet calls will not print the
 * particulars of the last snippet called.
 * 
 * Revision 1.6  88/11/04  17:05:33  little
 * generate code to make use of the d22_slang_source runtiem var
 * 
 * Revision 1.5  88/10/24  17:08:13  little
 * change c code generated for methods to take the object id as a parameter,
 * instead of a pointer to the id.
 * 
 * Revision 1.4  88/10/04  17:32:45  little
 * add log comment.
 * add parameter number check to csc_fun_call_write
 * 
*/

/*--------------------------------------------------------------------------*\
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

#define memoryPack

#include <strings.h>
#include <stdio.h>
#include "listPack.h"
#include "global.h"
#include "isc.h"
#include "c22.h"
#include "cs2.h"
#include "ca2.h"
#include "csc.h"
#include "cus.h"
#include "cud.h"
#include "iid.h"
#include "idd.h"
#include "isd.h"

#define INVALID_C_CODE "ERROR, DO NOT COMPILE\n"

static char csc_code_element_name [1024];

/*--------------------------------------------------------------------------*\
|  Routine: csc_statements_write
|
|  Description:
|     generate c code for the a "set" of statements
\*--------------------------------------------------------------------------*/
void csc_statements_write (stream, statements, current_statement, self)
FILE *stream;
LIST statements;
char *current_statement;
cus_sym self;
{ 

   set_status (ok);
   DEBUG2 (fprintf (stderr, "entering csc_statements_write\n");
      fprintf (stderr, "statements = \n");
      cud_print_tree (stderr, statements, "");
      fprintf (stderr, "current_statement = \n");
      cud_print_tree (stderr, current_statement, ""))

   while (current_statement != NULL)
      {csc_statement_write (stream, current_statement, self);
      current_statement = get_list_next (statements, current_statement);
      }
   return;
}					/* end csc_statements_write */

/*--------------------------------------------------------------------------*\
|  Routine: csc_if_write
|
|  Description:
|     generate c code for an if statement
\*--------------------------------------------------------------------------*/
void csc_if_write (stream, if_statement, self)
FILE *stream;
LIST if_statement;
cus_sym self;
{  LIST statements;

   set_status (ok);

   DEBUG2 (fprintf (stderr, "entering csc_if_write\n");
      fprintf (stderr, "if_statement = \n");
      cud_print_tree (stderr, if_statement, ""))

   fprintf (stream, "if (dro_boolean\n");
				/* process the condition */
   csc_exp_write (stream, get_list_pos (if_statement, 2), self, FALSE);
   fprintf (stream, ")\n");
   fprintf (stream, "{\n");
				/* process the then clause */
   statements = (LIST) get_list_pos (if_statement, 3);
   csc_statements_write (stream, statements, get_list_next (statements, NULL),
			 self);
   fprintf (stream, "}\n");
				/* process else clause if there is one */
   statements = (LIST) get_list_pos (if_statement, 4);
   if (statements)
      {fprintf (stream, "else\n");
      fprintf (stream, "{\n");
      csc_statements_write (stream,
			    statements, get_list_next (statements, NULL),
			    self);
      fprintf (stream, "}\n");
      }
   cs2_fflush (stream);
   return;
}					/* end csc_if_write */

/*--------------------------------------------------------------------------*\
|  Routine: csc_while_write
|
|  Description:
|     generate c code for a while statement
\*--------------------------------------------------------------------------*/
void csc_while_write (stream, while_statement, self)
FILE *stream;
LIST while_statement;
cus_sym self;
{  LIST statements;

   set_status (ok);

   fprintf (stream, "while (dro_boolean\n");
   csc_exp_write (stream, get_list_pos (while_statement, 2), self, FALSE);
   fprintf (stream, ")\n");

   fprintf (stream, "{\n");
   statements = (LIST) get_list_pos (while_statement, 3);
   csc_statements_write (stream, statements, get_list_next (statements, NULL),
			 self);
   fprintf (stream, "}\n");
   cs2_fflush (stream);
   return;
}					/* end csc_while_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_fun_parm_write
|
|  Description:
|     generate c code for the parameter to a function
\*--------------------------------------------------------------------------*/
void csc_fun_parm_write (stream,  cur_ptr, self, parameter_converter,
                        parameter_count)
FILE *stream;
LIST parameter_converter;
cus_sym self;
char *cur_ptr;
int parameter_count;

{
  /*this procedure generates code for a single parameter of a function.  
    If it is not the first parameter then a  ,  is generated prior
    to generating anything else.  If the parameter is to an external
    function and needs to be converted, then the call to the
    conversion routine is generated.*/

   idd_data_types param_type;

   if (parameter_count != 1) fprintf(stream, ",");

   if (parameter_converter != NULL) param_type = (idd_data_types)
                   get_list_pos (parameter_converter,parameter_count);

   if (parameter_converter != NULL && param_type != idd_no_data_type)	
          {              /*we have parameters to be converted*/
           fprintf(stream,"drc_get_atv (\n");
          }

   csc_exp_write (stream, cur_ptr, self, FALSE);

   if (parameter_converter != NULL && param_type != idd_no_data_type)
          {
            fprintf(stream,",%d)",param_type);
          }
   return;
}      					/* end    csc_fun_parm_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_fun_call_write
|
|  Description:
|     generate c code for a function call
\*--------------------------------------------------------------------------*/
void csc_fun_call_write (stream, fun_call_statement, self, param_offset)
FILE *stream;
LIST fun_call_statement;
cus_sym self;
int param_offset;

{  char *cur_ptr;
   int num_operands;
   cus_sym funct;
   char *function_converter;
   LIST parameter_converter;
   int parameter_count;

   set_status (ok);

   num_operands = list_length (fun_call_statement) - param_offset;
   funct = (cus_sym) get_list_pos (fun_call_statement, param_offset);
   if (!cus_get_prop (funct, is_built_in_prop))
      {COMPILE_ERROR (fprintf (stderr, "%s is not a defined function\n",
	 cus_sym_representation (funct)););
      return;
      }
   if (num_operands != (int) cus_get_prop (funct, num_operands_prop))
      {COMPILE_ERROR (fprintf (stderr,
                     "function has incorrect number of parameters\n");
         fprintf (stderr, "got %d, expected %d\n",
                  num_operands,
                  (int) cus_get_prop (funct, num_operands_prop));
         cud_print_tree (stderr,  fun_call_statement, ""););
      return;
      }

/*now generate the code for the function.*/

   function_converter = (char *)cus_get_prop(funct,
                returned_value_conversion_function_prop);

   parameter_converter = (LIST)cus_get_prop(funct,
                parameter_conversion_functions_prop);

   if(function_converter) fprintf(stream,"%s(\n",function_converter);

   fprintf (stream, "%s (\n", cus_get_prop (funct, c_function_name_prop));

   parameter_count = 1;
   
   if (cur_ptr = get_list_pos (fun_call_statement, param_offset+1))
      {
        csc_fun_parm_write (stream,  cur_ptr, self, parameter_converter,
                        parameter_count);
      
      while ((cur_ptr = get_list_next (fun_call_statement, cur_ptr)) != NULL)
        {
         parameter_count++;
         csc_fun_parm_write  (stream,  cur_ptr, self, parameter_converter,
                        parameter_count);
        }
      }

   if(function_converter) fprintf(stream,")");

   fprintf(stream, ")");

   set_status (ok);

   return;
}					/* end csc_fun_call_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_comment_write
|
|  Description:
|     generate c code for a comment
\*--------------------------------------------------------------------------*/
void csc_comment_write (stream, comment_statement, self)
FILE *stream;
LIST comment_statement;
cus_sym self;
{  char *rest;

   set_status (ok);

   fprintf (stream, "#if  0\n");
   fprintf (stream, "/* %s */\n",
      cus_get_prop (get_list_pos (comment_statement, 2), current_value_prop));
   fprintf (stream, "#endif\n");
   cs2_fflush (stream);
   if (rest = get_list_pos (comment_statement, 3))
      csc_comment_write (stream, rest, self);
   return;
}					/* end csc_comment_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_statement_write
|
|  Description:
|     generate c code for a statement
\*--------------------------------------------------------------------------*/
void csc_statement_write (stream, statement, self)
FILE *stream;
LIST statement;
cus_sym self;
{  cus_sym head;

   set_status (ok);

   DEBUG2 (fprintf (stderr, "entering csc_statement_write\n");
      fprintf (stderr, "statement = \n");
      cud_print_tree (stderr, statement, ""))

   head = (cus_sym) get_list_head (statement);
   c22_slang_line_num = get_head_val (statement);
   fprintf (stream, "d22_slang_line_num = %d;\n", c22_slang_line_num);
   if (head == car_if_symbol)
      csc_if_write (stream, statement, self);
    else if (head == car_while_symbol)
      csc_while_write (stream, statement, self);
    else if (head == car_function_call_symbol)
      {csc_fun_call_write (stream, statement, self,2);
       fprintf (stream, ";\n");
       cs2_fflush (stream);}
    else if (head ==  car_gets_symbol)
      csc_gets_write (stream, statement, self);
    else if (head == car_comment_symbol)
      csc_comment_write (stream, statement, self);
    else
      FATAL_ERROR (fprintf (stderr, "UNKNOWN STATEMENT TREE\n");
         cud_print_tree (stderr, statement, "   ");)
   return;
}					/* end csc_statement_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_snip_write
|
|  Description:
|     generate c code for a snippet
\*--------------------------------------------------------------------------*/
void csc_snip_write (stream, snippet, self, build_sne)
FILE *stream;
LIST snippet;
cus_sym self;
boolean build_sne;
{  char *snippet_name;

   set_status (ok);

   DEBUG2 (fprintf (stderr, "entering csc_snip_write\n");
      fprintf (stderr, "snippet = \n");
      cud_print_tree (stderr, snippet, ""))

   if (snippet)
      {snippet_name = get_list_next (snippet, NULL);
      csc_generic_call_write (snippet_name,
			      cus_sym_representation (snippet_name));
      DEBUG3 (fprintf (stderr, "starting snippet %s\n",
   		cus_sym_representation (snippet_name)))

			/* build routine specification */
      fprintf (stream, "void %s (sne)\n",
	cus_sym_representation (snippet_name));
      fprintf (stream, "dgr_sne *sne;\n");
      fprintf (stream, "{   dgr_sne *save_sne;\n");
      fprintf (stream, "    caddr_t save_source_name;\n");
      fprintf (stream, "    static char current_source_name [] = \"%s\";\n",
	       csc_code_element_name);
      fprintf (stream, "    dob_data_object SELF;\n\n");

			/* build routine body */
      if (c22_output_debug_code)
         {fprintf (stream, "DEBUG2 (fprintf (stderr, \"entering %s\\n\");\n",
	       cus_sym_representation (snippet_name));
         fprintf (stream, "\tfprintf (stderr, \"sne = %%X\\n\", sne))\n");
         }
      fprintf (stream, "save_source_name = d22_slang_source;\n");
      fprintf (stream, "d22_slang_source = current_source_name;\n");
      c22_slang_line_num = get_head_val (snippet);
      fprintf (stream, "SELF = sne-> parm_1;\n");
      fprintf (stream, "d22_slang_line_num = %d;\n", c22_slang_line_num);

      fprintf (stream, "save_sne = d22_sne;\n");
      if (build_sne)
         fprintf (stream,
		   "d22_sne = sne;\n");
       else
         fprintf (stream, "d22_sne = (dgr_sne *) NULL;\n");

      cs2_fflush (stream);
      csc_statements_write (stream, snippet,
			    get_list_next (snippet, snippet_name), self);

      fprintf (stream, "d22_sne = save_sne;\n");
      fprintf (stream, "d22_slang_line_num = -1;\n");
      fprintf (stream, "d22_slang_source = save_source_name;\n");
      if (c22_output_debug_code)
         {fprintf (stream, "DEBUG2 (fprintf (stderr, \"leaving %s\\n\");)\n",
	       cus_sym_representation (snippet_name));
         }
      fprintf (stream, "return;\n");
      fprintf (stream, "}\t/* end %s */\n\n\n\n",
	cus_sym_representation (snippet_name));
      cs2_fflush (stream);

      }

   return ;
}					/* end csc_snip_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_action_prologue
|
|  Description:
|     generate c code for the prologue for create/destroy actions
\*--------------------------------------------------------------------------*/
static void csc_action_prologue (stream, type, vc)
FILE *stream;
string type;
cus_sym vc;
{
   sprintf (csc_code_element_name, "%s action, vc = %s",
        type, 
	cus_sym_representation (vc));
   fprintf (stream, "\t/* %s action, vc = %s, u_name = %s */\n",
	type,
	cus_sym_representation (vc),
	cus_get_prop (vc, u_name_prop));
   return;
}					/* end csc_action_prologue */


/*--------------------------------------------------------------------------*\
|  Routine: csc_action_write
|
|  Description:
|     generate c code for create/destroy actions
\*--------------------------------------------------------------------------*/
void csc_action_write (type, vc)
string type;
cus_sym vc;
{  LIST snippet;

   set_status (ok);

   if (!strcmp (type, "create"))
      {snippet = (LIST) cus_get_prop (vc, create_actions_snippet_prop);
      if (snippet)
         {csc_action_prologue (cs2_c_cact_stream, type, vc);
         csc_action_prologue (cs2_c_proc_header0_stream, type, vc);
         csc_snip_write (cs2_c_cact_stream, snippet, vc, FALSE);
         fprintf (cs2_c_cact_stream, "\n\n");
         }
      }
    else if (!strcmp (type, "destroy"))
      {snippet = (LIST) cus_get_prop (vc, destroy_actions_snippet_prop);
      if (snippet)
         {csc_action_prologue (cs2_c_dact_stream, type, vc);
         csc_action_prologue (cs2_c_proc_header0_stream, type, vc);
         csc_snip_write (cs2_c_dact_stream, snippet, vc, FALSE);
         fprintf (cs2_c_dact_stream, "\n\n");
         }
      }
    else
      FATAL_ERROR (fprintf (stderr,
			"csc_action_write called with invalid type of %s\n",
		   type))

   return ;
}					/* end csc_action_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_meth_prologue
|
|  Description:
|     generate c code for the prologue for a method
\*--------------------------------------------------------------------------*/
static void csc_meth_prologue (stream, obj)
FILE *stream;
cus_sym obj;
{  cus_sym parent_vc;

   parent_vc = (cus_sym) cus_get_prop (obj, parent_vc_prop);
   sprintf (csc_code_element_name, "methods, object = %s, vc = %s",
        cus_sym_representation (obj),
	cus_sym_representation (parent_vc));
   fprintf (stream,
      "\t/* method for object = %s, u_name = %s\n\tvc = %s, u_name = %s */\n",
	cus_sym_representation (obj),
        cus_get_prop (obj, u_name_prop),
	cus_sym_representation (parent_vc),
	cus_get_prop (parent_vc, u_name_prop));
   return;
}					/* end csc_meth_prologue */


/*--------------------------------------------------------------------------*\
|  Routine: csc_meth_write
|
|  Description:
|     generate c code for a method
\*--------------------------------------------------------------------------*/
void csc_meth_write (obj)
cus_sym obj;
{  LIST snippet;
   cus_sym self;
   char *snippet_name;

   set_status (ok);

   DEBUG2 (fprintf (stderr, "entering csc_meth_write\n");
      fprintf (stderr, "obj = %x\n", obj););

   snippet = (LIST) cus_get_prop (obj, fixed_method_snippet_prop);
   if (!snippet)
      return;

   csc_meth_prologue (cs2_c_meth_stream, obj);
   csc_meth_prologue (cs2_c_proc_header0_stream, obj);
   self = (cus_sym) cus_get_prop (obj, fixed_method_prop);

   snippet_name = get_list_next (snippet, NULL);
   csc_generic_call_write (snippet_name,cus_sym_representation (snippet_name));
   DEBUG3 (fprintf (stderr, "starting snippet %s\n",
		cus_sym_representation (snippet_name)))

			/* build routine specification */
   fprintf (cs2_c_meth_stream, "void %s (sne)\n",
	cus_sym_representation (snippet_name));
   fprintf (cs2_c_meth_stream, "dgr_sne *sne;\n");
   fprintf (cs2_c_meth_stream, "{   dgr_sne *save_sne;\n");
   fprintf (cs2_c_meth_stream, "    caddr_t save_source_name;\n");
   fprintf (cs2_c_meth_stream,
	    "    static char current_source_name [] = \"%s\";\n",
	       csc_code_element_name);
   fprintf (cs2_c_meth_stream, "    dob_data_object SELF;\n\n");

			/* build routine body */
   fprintf (cs2_c_meth_stream, "save_source_name = d22_slang_source;\n");
   fprintf (cs2_c_meth_stream, "d22_slang_source = current_source_name;\n");
   if (c22_output_debug_code)
      {fprintf (cs2_c_meth_stream,
		"DEBUG2 (fprintf (stderr, \"entering %s\\n\");\n",
	       	cus_sym_representation (snippet_name));
      fprintf (cs2_c_meth_stream,
	       "\tfprintf (stderr, \"SELF = %%d\\n\", SELF))\n");
      }
   fprintf (cs2_c_meth_stream, "SELF = sne-> parm_1;\n");

			/* now the code to see if we are in the "rite" 
			   snippet */
   fprintf (cs2_c_meth_stream,
	    "if (strcmp (\"%s\",\n", cus_get_prop (obj, u_name_prop));
   fprintf (cs2_c_meth_stream, "   cus_sym_representation (\n");
   fprintf (cs2_c_meth_stream, "      dob_get_symbol_entry (\n");
   fprintf (cs2_c_meth_stream,
  "         cus_get_prop (dob_get_symbol_entry (SELF), template_prop)))))\n");
   fprintf (cs2_c_meth_stream, "   {free_node (sne);\n");
   fprintf (cs2_c_meth_stream, "   return;\n");
   fprintf (cs2_c_meth_stream, "   }\n\n");

			/* now for the "guts" of the body */
   c22_slang_line_num = get_head_val (snippet);
   fprintf (cs2_c_meth_stream,
	"d22_slang_line_num = %d;\n", c22_slang_line_num);

   fprintf (cs2_c_meth_stream, "save_sne = d22_sne;\n");
   fprintf (cs2_c_meth_stream, "d22_sne = (dgr_sne *) NULL;\n");

   cs2_fflush (cs2_c_meth_stream);
   csc_statements_write (cs2_c_meth_stream, snippet,
			 get_list_next (snippet, snippet_name), self);

   fprintf (cs2_c_meth_stream, "d22_sne = save_sne;\n");
   fprintf (cs2_c_meth_stream, "d22_slang_source = save_source_name;\n");
   fprintf (cs2_c_meth_stream, "free_node (sne);\n");
   if (c22_output_debug_code)
      fprintf (cs2_c_meth_stream,
	       "DEBUG2 (fprintf (stderr, \"leaving %s\\n\");)\n",
	       cus_sym_representation (snippet_name));
   fprintf (cs2_c_meth_stream, "return;\n");
   fprintf (cs2_c_meth_stream, "}\t/* end %s */\n\n\n\n",
	cus_sym_representation (snippet_name));
   cs2_fflush (cs2_c_meth_stream);

   return ;
}					/* end csc_meth_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_att_prologue
|
|  Description:
|     generate c code for the prologue for an attribute
\*--------------------------------------------------------------------------*/
static void csc_att_prologue (stream, att)
FILE *stream;
cus_sym att;
{  cus_sym parent_obj, parent_vc;

   parent_obj = (cus_sym) cus_get_prop (att, parent_object_prop);
   parent_vc = (cus_sym) cus_get_prop (parent_obj, parent_vc_prop);

   sprintf (csc_code_element_name, "attribute = %s, object = %s, vc = %s",
	cus_sym_representation (att),
	cus_sym_representation (parent_obj),
	cus_sym_representation (parent_vc));

   fprintf (stream, "\t/* attribute = %s, u_name = %s */\n",
	cus_sym_representation (att), cus_get_prop (att, u_name_prop));

   fprintf (stream, "\t/* object = %s, u_name = %s */\n",
	cus_sym_representation (parent_obj),
	cus_get_prop (parent_obj, u_name_prop));

   fprintf (stream, "\t/* vc = %s, u_name = %s */\n",
	cus_sym_representation (parent_vc),
	cus_get_prop (parent_vc, u_name_prop));
   return;
}					/*  end csc_att_prologue */


/*--------------------------------------------------------------------------*\
|  Routine: csc_att_write
|
|  Description:
|     generate c code for an attribute
\*--------------------------------------------------------------------------*/
void csc_att_write (att)
cus_sym att;
{  LIST snippet;

   set_status (ok);

   snippet = (LIST) cus_get_prop (att, value_snippet_prop);
   if (snippet)
      {csc_att_prologue (cs2_c_att_stream, att);
      csc_att_prologue (cs2_c_proc_header0_stream, att);
      csc_snip_write (cs2_c_att_stream, snippet, att, TRUE);
      fprintf (cs2_c_att_stream, "\n\n");
      }

   return ;
}					/* end csc_att_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_var_prologue
|
|  Description:
|     generate c code for the prologue for a var
\*--------------------------------------------------------------------------*/
static void csc_var_prologue (stream, var)
FILE *stream;
cus_sym var;
{  cus_sym parent_vc;

   parent_vc = (cus_sym) cus_get_prop (var, parent_vc_prop);

   sprintf (csc_code_element_name, "variable = %s, vc = %s",
	cus_sym_representation (var),
	cus_sym_representation (parent_vc));

   fprintf (stream,
	"\t/* variable = %s, u_name = %s\n\tvc = %s, u_name = %s */\n",
	cus_sym_representation (var),
	cus_get_prop (var, u_name_prop),
	cus_sym_representation (parent_vc),
        cus_get_prop (parent_vc, u_name_prop));
   return;
}					/* end csc_var_prologue */


/*--------------------------------------------------------------------------*\
|  Routine: csc_var_write
|
|  Description:
|     generate c code for a var
\*--------------------------------------------------------------------------*/
void csc_var_write (var)
cus_sym var;
{  LIST snippet;

   set_status (ok);

   snippet = (LIST) cus_get_prop (var, initial_value_snippet_prop);
   if (snippet)
      {csc_var_prologue (cs2_c_var_stream, var);
      csc_var_prologue (cs2_c_proc_header0_stream, var);
      csc_snip_write (cs2_c_var_stream, snippet, var, TRUE);
      fprintf (cs2_c_var_stream, "\n\n");
      }

   return ;
}					/* end csc_var_write */


/*--------------------------------------------------------------------------*\
|  Routine: csc_is_sd
|
|  Description:
|     checks to see if the parameter is a shared data element
\*--------------------------------------------------------------------------*/
boolean csc_is_sd (parm)
char * parm;
{  cus_sym ill_entry;

   DEBUG2 (fprintf (stderr, "entering csc_is_sd, parm =\n");
      cud_print_tree (stderr, parm, "   "))

   if (cus_is_sym (parm))
      ill_entry = ca2_get_sd_sym (cus_sym_representation (parm));
   else
/*
???? there is some hair here, change when going to full dot notation
*/
      ill_entry = ca2_get_sd_sym (cus_sym_representation
					(get_list_pos (parm, 2)));

   DEBUG2 (fprintf (stderr, "returned value = %s\n", ill_entry != NULL ?
					     "TRUE" : "FALSE"))
   return (ill_entry != NULL);
}					/* end csc_is_sd */


/*--------------------------------------------------------------------------*\
|  Routine: csc_cc_prologue
|
|  Description:
|     generates c code for the prologue for a creation cond
\*--------------------------------------------------------------------------*/
static void csc_cc_prologue (stream, vc)
FILE *stream;
cus_sym vc;
{
   sprintf (csc_code_element_name, "creation condition, vc = %s",
	cus_sym_representation (vc));
   fprintf (stream, "\t/*  pet for %s vc */\n", cus_sym_representation (vc));
   return;
}					/* csc_cc_prologue */


/*--------------------------------------------------------------------------*\
|  Routine: csc_cc_write
|
|  Description:
|     generates c code for a creation cond
\* --------------------------------------------------------------------------*/
void csc_cc_write (vc)
cus_sym vc;
{  string p_proc_name, pet_name;
   LIST creation_condition, parm_list, old_parm_list;
   int parm_cnt, parm_list_len;
   int sd_id_parm;
   caddr_t old_parm, parm;
   cus_context_type target_context;
   boolean climbed_out;
   cus_sym_type parm_type;

   set_status (ok);

   DEBUG2 (fprintf (stderr, "csc_cc_write enter: vc = %s\n",
                    cus_sym_representation (vc)););

   if (vc == car_god_vc)                /* don't need a pet for god */
      return;

   c22_slang_line_num = get_head_val (cus_get_prop (vc,
                                                    creation_condition_prop));

   pet_name =  cus_get_prop (vc, u_name_prop);
   p_proc_name = make_node (strlen (pet_name) + 5);
   strcat (strcpy (p_proc_name, pet_name), "_PET");
   DEBUG3 (fprintf (stderr, "\tPET proc name = %s\n", p_proc_name);)
   cus_put_prop (vc, pet_proc_name_prop, p_proc_name);

   csc_cc_prologue (cs2_c_pet_stream, vc);
   csc_cc_prologue (cs2_c_proc_header0_stream, vc);

   csc_generic_call_write (vc, p_proc_name);

			/* build routine specification */
   fprintf (cs2_c_pet_stream, "void %s (sne)\n", p_proc_name);
   fprintf (cs2_c_pet_stream, "dgr_sne *sne;\n");

   old_parm_list = (LIST) cus_get_prop (vc, old_cc_used_vars_prop);
   parm_list = (LIST) cus_get_prop (vc, cc_used_vars_prop);
   parm_list_len = list_length (parm_list);
   sd_id_parm = iid_null_id;
   for (parm_cnt = 0; parm_cnt < parm_list_len; parm_cnt++)
      {parm = get_list_pos (parm_list, parm_cnt + 1);
      DEBUG3 (fprintf (stderr, "csc_cc_write: parm = %s\n",
                       cus_sym_representation (parm)););
      if (csc_is_sd (parm))
         {sd_id_parm = parm_cnt; 
/*
         save old values attached to creating sd (parm)
         to be restored at ENDVC
*/
         cus_put_prop (vc, creating_sd_prop, parm);

         cus_put_prop (vc, old_created_vcs_prop,
                           cus_get_prop (parm,created_vcs_prop));
         cus_put_prop (vc, old_parent_vc_prop,
                             cus_get_prop (parm,parent_vc_prop));
         cus_put_prop (vc, old_owned_context_prop,
                          cus_get_prop (parm, owned_context_prop));

         cus_put_prop (parm, created_vcs_prop, vc);
         cus_put_prop (parm, parent_vc_prop, vc);
         cus_put_prop (parm, owned_context_prop,
		       cus_get_prop (vc, owned_context_prop));
         DEBUG3 (fprintf (stderr, "\tsetting creating_sd_prop to %s\n",
                          cus_sym_representation (parm)););
	 }
      }

				/* build routine body */
				/* routine local variables */
   fprintf (cs2_c_pet_stream, "{  dgr_sne *save_sne;\n");
   fprintf (cs2_c_pet_stream, "   LIST parm_list;\n");
   fprintf (cs2_c_pet_stream, "   dob_data_object SELF;\n");

   for (parm_cnt = 0; parm_cnt < parm_list_len; parm_cnt++)
      {fprintf (cs2_c_pet_stream, "   dob_data_object PARM_%d;\n", parm_cnt);
      }
   
   fprintf (cs2_c_pet_stream, "   caddr_t save_source_name;\n");
   fprintf (cs2_c_pet_stream,
	    "   static char current_source_name [] = \"%s\";\n",
	       csc_code_element_name);
   fprintf (cs2_c_pet_stream, "   dob_data_object the_vc;\n\n");

				/* routine prologue */
   fprintf (cs2_c_pet_stream, "save_source_name = d22_slang_source;\n");
   fprintf (cs2_c_pet_stream, "d22_slang_source = current_source_name;\n");
   if (c22_output_debug_code)
      fprintf (cs2_c_pet_stream,
	       "DEBUG2 (fprintf(stderr, \"entering %s\\n\");)\n",
       	       p_proc_name);

   fprintf (cs2_c_pet_stream, "SELF = sne-> parm_1;\n");

   for (parm_cnt = 0; parm_cnt < parm_list_len; parm_cnt++)
      {parm = get_list_pos (parm_list, parm_cnt + 1);
      if (parm_cnt == sd_id_parm)
         fprintf (cs2_c_pet_stream, "PARM_%d = sne-> parm_2;\n", sd_id_parm);
        else
         {if (!cus_is_sym (parm))
            FATAL_ERROR (fprintf
			  (stderr,
			  "csc_cc_write: new parameter not symbol for vc %s\n",
		         cus_sym_representation (vc)));
         parm_type = (cus_sym_type) cus_get_prop (parm, type_prop);
	 old_parm = get_list_pos (old_parm_list, parm_cnt + 1);
         fprintf (cs2_c_pet_stream,
	"PARM_%d = (dob_data_object) dra_find_thingie (dro_ask2 (\n",parm_cnt);

         switch (parm_type)
            {case vc_sym:
	       target_context = (cus_context_type)
				  cus_get_prop (parm, owned_context_prop);
               break;
            case object_sym:
	       target_context = (cus_context_type)
				  cus_get_prop (cus_get_prop (parm,
							      parent_vc_prop),
						owned_context_prop);
               break;
            case variable_sym:
	       target_context = (cus_context_type)
				  cus_get_prop (cus_get_prop (parm,
							      parent_vc_prop),
						owned_context_prop);
               break;
            default:
            FATAL_ERROR (fprintf
			  (stderr,
       "csc_cc_write: illegal parameter type of %s for %s symbol for vc %s\n",
		         cus_type_print_val (parm_type),
			 cus_sym_representation (vc)));
            }
         (void) csc_context_climb_out (cs2_c_pet_stream,
				       cus_current_context (),
		  		       target_context,
				       cus_get_prop(vc,parent_vc_prop));

         fprintf (cs2_c_pet_stream, "), %s, \"%s\");\n",
		  cus_type_print_val (parm_type),
		  cus_sym_representation (parm));
         }
      }
   
/*
??? call dpt_rite rule when not new and have sd
look into (cc of true w/o a creatingsd) and (a cc w/creating sd and no new)
cc_uses_new_prop
*/
   if ((sd_id_parm != iid_null_id) && !(cus_get_prop (vc, cc_uses_new_prop)))
      {fprintf (cs2_c_pet_stream,
	"if (!dpt_rite_rule (SELF, PARM_%d))\n", sd_id_parm);
      fprintf (cs2_c_pet_stream, "\treturn;\n");
      }

   fprintf (cs2_c_pet_stream, "save_sne = d22_sne;\n");
   fprintf (cs2_c_pet_stream, "d22_sne = sne;\n");
   fprintf (cs2_c_pet_stream, "parm_list = make_list ();\n");
   fprintf (cs2_c_pet_stream,
	    "(void) add_to_tail (parm_list, SELF);\n");
   if (cus_get_prop (vc, cc_uses_new_prop))
      {for (parm_cnt = 0; parm_cnt < parm_list_len; parm_cnt++)
         {fprintf(cs2_c_pet_stream,
		  "(void) add_to_tail (parm_list, PARM_%d);\n", parm_cnt);
         }
      }
   fprintf (cs2_c_pet_stream, "the_vc = dpt_vc_exists (\"%s\", parm_list);\n",
	pet_name);

				/* process the condition */
   fprintf (cs2_c_pet_stream, "if (");
   if (cus_get_prop (vc, cc_uses_new_prop))
      {if (sd_id_parm != iid_null_id)
         fprintf (cs2_c_pet_stream, "(dob_validate_object (PARM_%d)) && ",
		  sd_id_parm);
       else
         {fprintf (cs2_c_pet_stream, INVALID_C_CODE);
         COMPILE_ERROR (fprintf (stderr,
  "new parameter is not a shared data element in creation condition for %s\n",
	    cus_sym_representation (vc)););
         }
      }

   fprintf (cs2_c_pet_stream, "dro_boolean (\n");
   if (creation_condition =
		(LIST) cus_get_prop (vc, fixed_creation_condition_prop))
      csc_exp_write (cs2_c_pet_stream, creation_condition, vc, TRUE);
    else
      FATAL_ERROR (fprintf (stderr, "no creation condition for %s\n",
		cus_sym_representation (vc)))
   fprintf (cs2_c_pet_stream, "))\n");
   fprintf (cs2_c_pet_stream, "\t{if (the_vc == dob_null_object)\n");
   fprintf (cs2_c_pet_stream,
	"\t\tdob_vc_create (SELF,\n");
   fprintf (cs2_c_pet_stream,
	"\t\t\t(int) cus_get_prop (cus_look_up (\"%s\"), id_prop),\n",
	cus_get_prop (vc, u_name_prop));
   fprintf (cs2_c_pet_stream, "\t\t\tparm_list,\n");
   if (sd_id_parm == iid_null_id)
      {fprintf (cs2_c_pet_stream,
	"\t\t\t(int) cus_get_prop (dob_get_symbol_entry (SELF),\n");
      fprintf (cs2_c_pet_stream,"\t\t\t   creating_sd_prop));\n");
      }
    else
      fprintf (cs2_c_pet_stream,
	"\t\t\tPARM_%d);\n", sd_id_parm);

   fprintf (cs2_c_pet_stream, "\t  else\n");
   fprintf (cs2_c_pet_stream, "\t\tfree_list (parm_list, NOT_NODES);\n");
   fprintf (cs2_c_pet_stream, "\t}\n");
   fprintf (cs2_c_pet_stream, "  else\n");

   fprintf (cs2_c_pet_stream, "\t{free_list (parm_list, NOT_NODES);\n");
   fprintf (cs2_c_pet_stream, "\tif (the_vc != dob_null_object)\n");
   fprintf (cs2_c_pet_stream, "\t\t{dob_destroy_data_object (the_vc);\n");
   fprintf (cs2_c_pet_stream, "\t\t}\n");
   fprintf (cs2_c_pet_stream, "\t}\n");
   fprintf (cs2_c_pet_stream, "d22_sne = save_sne;\n");
   fprintf (cs2_c_pet_stream, "d22_slang_source = save_source_name;\n");
   if (c22_output_debug_code)
      fprintf (cs2_c_pet_stream,
	       "DEBUG2 (fprintf(stderr, \"leaving %s\\n\");)\n",
       	       p_proc_name);
   fprintf (cs2_c_pet_stream, "return;\n");
   fprintf (cs2_c_pet_stream, "}\n\n\n");
   cs2_fflush (cs2_c_pet_stream);

   return ;
}					/* end csc_cc_write  */


/*--------------------------------------------------------------------------*\
|  Routine: csc_generic_call_write
|
|  Description:
|     writes info wrt the "generic" calling mechanism used at run-time
|	onto the headers files
\*--------------------------------------------------------------------------*/
void csc_generic_call_write (sym, proc_name)
cus_sym sym;
string proc_name;
{  

   fprintf (cs2_c_proc_header0_stream, "void %s ();\t/***** %d *****/\n\n",
	proc_name, cs2_current_c_proc_number);
   fprintf (cs2_c_proc_header1_stream, "\t%s,\n", proc_name);
   cus_put_prop (sym, c_proc_number_prop, cs2_current_c_proc_number);
   cs2_current_c_proc_number++;
   return;
}					/* end csc_generic_call_write */
