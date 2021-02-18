static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/cam.c,v 1.19 89/07/24 14:55:19 little Exp $";


/*
$Log:	cam.c,v $
 * Revision 1.19  89/07/24  14:55:19  little
 * ops removal
 * 
 * Revision 1.18  89/04/07  15:18:28  little
 * no changes
 * 
 * Revision 1.17  89/03/28  12:52:47  serpent
 * changed  #ifdef 0   to   #if 0  to allow sun compile.
 * 
 * Revision 1.16  89/02/28  18:53:07  little
 * move some killer debug output drom debug1 to debug3
 * 
 * Revision 1.15  89/02/28  18:08:06  little
 * fix methods so the method snippet can be empty and can also
 * have more thatn one statement.
 * 
 * Revision 1.14  89/02/27  17:02:18  little
 * add cam_fix_news routine, change cam_fix_cc_used_vars so it will work
 * with the "new" data items created by cam_fix_news, add & fix some debug
 * 
 * Revision 1.13  89/02/15  17:35:18  little
 * add copyright notice
 * 
 * Revision 1.12  89/01/11  13:49:03  little
 * no changes
 * 
 * Revision 1.11  89/01/11  13:46:09  little
 * no changes
 * 
 * Revision 1.10  89/01/11  13:36:33  little
 * no changes
 * 
 * Revision 1.9  89/01/11  13:31:58  little
 * no change
 * 
 * Revision 1.8  89/01/11  13:30:29  little
 * no changes
 * 
 * Revision 1.7  89/01/11  12:54:05  little
 * no changes
 * 
 * Revision 1.6  89/01/09  16:28:18  little
 * add some comments about handling "empty" code snippets later on
 * 
 * Revision 1.5  88/12/14  10:40:05  little
 * add cam_get_new function, which find the sd elements which are parameters
 * to the new function in a creation condition
 * and modify analyusis to the cam_get_news
 * 
 * Revision 1.4  88/11/03  15:32:59  little
 * fix things so creations conditions can have relations between sd components
 * and vc variables.  fixed cam_fix_cc_used_vars to step throught the list
 * correctly.  setting the loop_through_list iteration var inside the loop
 * was a loser.
 * 
 * Revision 1.3  88/11/01  08:41:04  little
 * but some debug output under DEBUG variable control
 * 
 * Revision 1.2  88/10/31  15:02:32  little
 * fix analysis of creation condition used vars, to cull out "duplicates" from
 * the used var list.  and make other necessary changes to use the new list
 * correctly.
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

#include <strings.h>
#include <stdio.h>
#include "listPack.h"
#include "global.h"
#include "isc.h"
#include "c22.h"
#include "cam.h"
#include "cus.h"

#define INVALID_C_CODE "ERROR, DO NOT COMPILE\n"

/* ???? maybe do not need */
boolean csc_is_sd ();

/* --------------------------------------------     cam_rep_eq */
static int cam_rep_eq (ptr, element)
NODE ptr;
NODE element;
{  int ret_val;

   DEBUG2 (fprintf (stderr,
		    "cam_rep_eq: enter, ptr = %x, element = %x\n",
		    ptr, element);
           fprintf (stderr, "ptr_rep = %s, element_rep = %s\n",
		    cus_sym_representation (ptr),
		    cus_sym_representation (element)););

   ret_val = !strcmp (cus_sym_representation (ptr),
		      cus_sym_representation (element));

   DEBUG2 (fprintf (stderr, "cam_rep_eq: exit, ret_val = %d\n",
		    ret_val););
   return (ret_val);
}					/* end cam_rep_eq */


/* --------------------------------------------     cam_fix_cc_used_vars   */
static void cam_fix_cc_used_vars (vc)
cus_sym vc;
{  LIST new_list, old_list;
   caddr_t test, look;

   DEBUG2 (fprintf (stderr, "cam_fix_cc_used_vars: enter\n\t vc = %s\n",
		    cus_sym_representation (vc)););

   c22_slang_line_num = (int) cus_get_prop (vc, cc_lineno_prop);

   old_list = (LIST) cus_get_prop (vc, cc_used_vars_prop);
   DEBUG3 (fprintf (stderr, "cam_fix_cc_used_vars: old cc used vars = \n");
      cud_print_tree (stderr, old_list, ""););

			/* save old value in case we need it later */
   cus_put_prop (vc, old_cc_used_vars_prop, old_list);

   new_list = make_list ();
   DEBUG3 (fprintf (stderr, "cam_fix_cc_used_vars: old_list length = %d\n",
	   list_length (old_list)););
   loop_through_list (old_list, look, caddr_t)
      {if (!cus_is_sym (look))
         test = get_list_pos (look, 2);
       else
         test = look;
      DEBUG3 (fprintf(stderr, "cam_fix_cc_used_vars: test = %s, u_name = %s\n",
	      cus_sym_representation (test),
	      cus_get_prop (test, u_name_prop)););
      if ((cus_get_prop (test, u_name_prop) ||

/* ???? maybe to use ca2_get_sd_sym instead of csc_is_sd */

	   csc_is_sd (test))
		&&
          !search_list (new_list, test, cam_rep_eq))
         {DEBUG3 (fprintf (stderr,
		  "cam_fix_cc_used_vars: %s added to new_list\n",
		  cus_sym_representation (test)););
         add_to_tail (new_list, test);
         }
      }
   DEBUG3 (fprintf (stderr, "cam_fix_cc_used_vars: modified cc used vars\n");
      cud_print_tree (stderr, new_list, ""););
   cus_put_prop (vc, cc_used_vars_prop, new_list);

   c22_slang_line_num = 0;

   DEBUG2 (fprintf (stderr, "cam_fix_cc_used_vars: exit\n"););

   return;
}					/* end cam_fix_cc_used_vars */


/* --------------------------------------------     cam_get_news */
static void cam_get_news (vc, cc)
cus_sym vc;
LIST cc;
{  char *elem;
   LIST old_prop;

   DEBUG2 (fprintf (stderr, "cam_get_news: enter\n\tcc = \n");
	   cud_print_tree (stderr, cc, "\t"););

   if (cus_is_sym (cc))
      return;

   loop_through_list (cc, elem, char *)
      {if (!cus_is_sym (elem))
         {if ((cus_sym) get_list_pos (elem, 1) == car_function_call_symbol)
            {DEBUG3 (fprintf (stderr, "cam_get_news: got function call\n"););
            if ((cus_sym) get_list_pos (elem, 2) == car_new_function)
               {DEBUG3 (fprintf (stderr, "cam_get_news: got new call\n"););
               if (!cus_is_sym (get_list_pos (elem, 3)))
                  {COMPILE_ERROR (fprintf (stderr,
                      "new function parameter must be simple item\n"););
                  }
                else
                  {DEBUG3 (fprintf (stderr, "cam_get_news: new call OK\n"););
                  old_prop = (LIST) cus_get_prop (vc, cc_uses_new_prop);
                  add_to_head (old_prop, get_list_pos (elem, 3));
                  cus_put_prop (vc, cc_uses_new_prop, old_prop);
                  }
               }
            }
             else			/* is not a call to new */
               {cam_get_news (vc, elem);
               }
         }
          else				/* is not a functin call */
            {cam_get_news (vc, elem);
            }
 
      }
   return;
}					/* end cam_get_news */


/* --------------------------------------------     cam_fix_news     */
void cam_fix_news (vc, cc)
cus_sym vc;
LIST cc;
{  char * elem;
   cus_sym new_parm;
   string new_parm_string;
   int new;

   DEBUG2 (fprintf (stderr, "cam_fix_news: enter\n\tvc = %s, cc = \n",
             cus_sym_representation (vc));
          cud_print_tree (stderr, cc, "   "););

   if (cus_is_sym (cc))
    goto DONE;

   loop_through_list (cc, elem, char *)
      {if (!cus_is_sym (elem))
         {DEBUG3 (fprintf (stderr, "cam_fix_news: checking for new call\n"););
         if (((cus_sym) get_list_pos (elem, 1) == car_function_call_symbol) &&
              ((cus_sym) get_list_pos (elem, 2) == car_new_function))
            {DEBUG3 (fprintf(stderr, "cam_fix_news: got a new funct call\n"););
            if (!cus_is_sym (get_list_pos (elem, 3)))
               {COMPILE_ERROR (fprintf (stderr,
                   "new function parameter must be simple item\n"););
               }
             else
               {DEBUG3(fprintf(stderr,"cam_fix_news:new param not a tree\n"););
               new_parm = (cus_sym) get_list_pos (elem, 3);
               if ((cus_get_prop (new_parm, is_constant_prop)) &&
                   ((idd_data_types) cus_get_prop (new_parm, current_type_prop)
                              == idd_string))
                  {new_parm_string = cus_get_prop (new_parm,
					 	   current_value_prop);
                  if (strcspn (new_parm_string, ".") !=
                       strlen (new_parm_string))
                     {COMPILE_ERROR (fprintf (stderr,
                         "new function parameter must be simple item\n"););
                     }
                   else
                     {DEBUG3 (fprintf (stderr,
			"cam_fix_news: making new new parameter = %s\n",
			new_parm_string););
                     new_parm = cus_add_sym (new_parm_string, id_sym,
                                              &new, WITH_CONTEXT);
                     DEBUG3 (fprintf (stderr,
			"cam_fix_news: add_sym new = %d\n", new););
                     delete_list_pos (elem, 3);
                     insert_list_pos (elem, new_parm, 3);
                     add_to_tail (cus_get_prop (vc, cc_used_vars_prop),
                                  new_parm);
                     }
                  }
                else
                  {COMPILE_ERROR (fprintf (stderr,
		     "new parameter must be a string\n"););
                  }
               }
            }
          else				/* not a new function call */
            {cam_fix_news (vc, elem);
            }
         }
      }

DONE:

   DEBUG2 (fprintf (stderr, "cam_fix_news: exit\n"););

   return;
}                                       /* end cam_fix_news */


/* --------------------------------------------     cam_fix_cc     */
void cam_fix_cc (vc)
cus_sym vc;
{  LIST cc_used_vars, fixed_cc, old_cc, fixed_uses_new, temp_list;

   DEBUG2 (fprintf (stderr, "cam_fix_cc: enter, vc = %s\n",
	   cus_sym_representation (vc)););

   old_cc = (LIST) cus_get_prop (vc, creation_condition_prop);
   c22_slang_line_num = (int) cus_get_prop (vc, cc_lineno_prop);

   cc_used_vars = (LIST) cus_get_prop (vc, cc_used_vars_prop);

   DEBUG3 (fprintf (stderr, "cam_fix_cc: cc_used_vars = \n");
   	cud_print_tree (stderr, cc_used_vars, "   "););

   DEBUG3 (fprintf (stderr, "cam_fix_cc: old cc for %s\n",
	cus_sym_representation (vc));
   	cud_print_tree (stderr, old_cc, "   "););

   cam_fix_cc_used_vars (vc);

   fixed_cc = cam_fix_cc_tree (vc, old_cc, cc_used_vars);
   DEBUG3 (fprintf (stderr, "cam_fix_cc: cc after cam_fix_cc_tree\n");
   	cud_print_tree (stderr, fixed_cc, ""););

   cus_put_prop (vc, fixed_creation_condition_prop, fixed_cc);

   cus_put_prop (vc, cc_uses_new_prop, make_list());
   temp_list = make_list ();
				/* make a single element tree outa the cc
				   this makes the loop_through_list work
				   for the first time into cam_get_news
				*/
   add_to_head (temp_list, old_cc);
   cam_get_news (vc, temp_list);
   free_list (temp_list, NOT_NODES);
   fixed_uses_new = (LIST) cus_get_prop (vc, cc_uses_new_prop);
				/* code further on depends on the fact the
				   the cc_uses_new_prop is NULL if there
				   is no use of the new function in the cc &
				   we had to out an empty list in as the value
				   so cam_get_news could work correctly */
   if (list_length (fixed_uses_new) < 1)
      cus_put_prop (vc, cc_uses_new_prop, NULL);
   DEBUG3 (fprintf (stderr, "cam_fix_cc: refined cc_uses_new_prop\n");
      cud_print_tree (stderr, fixed_uses_new, "\t"););
   c22_slang_line_num = 0;

   return;
}					/* end cam_fix_cc */


/* --------------------------------------------     cam_fix_cc_tree */
LIST cam_fix_cc_tree (vc, old_cc, cc_used_vars)
cus_sym vc;
LIST old_cc;
LIST cc_used_vars;
{  LIST new_cc;
   char *elem;

   DEBUG2 (fprintf (stderr, "cam_fix_cc_tree: enter\n\told_cc =\n");
      cud_print_tree (stderr, old_cc, "\t"););

   if (cus_is_sym (old_cc))
       {if (cus_get_prop (old_cc, is_built_in_prop))
          return (old_cc);
        else
         return ((LIST) cam_fix_var (vc, old_cc));
       }
      
   new_cc = make_list ();

   loop_through_list (old_cc, elem, char *)
      {DEBUG3 (fprintf (stderr, "cam_fix_cc_tree: loop top, elem = \n");
         cud_print_tree (stderr, elem, "   "););
      if (cus_is_sym (elem))
         {DEBUG3 (fprintf (stderr, "cam_fix_cc_tree: elem = %s\n",
		cus_sym_representation (elem)););
         if (cus_get_prop (elem, is_built_in_prop))
            {DEBUG3 (fprintf (stderr, "cam_fix_cc_tree: elem is built in\n"););
            add_to_tail (new_cc, elem);
            }
           else
            add_to_tail (new_cc, cam_fix_var (vc, elem));
         }
        else		/* not sym, must be a list */
         {if ((cus_sym) get_list_pos (elem, 1) == car_dot_symbol)
            add_to_tail (new_cc, cam_fix_dot_tree (vc, (LIST) elem));
           else
             add_to_tail (new_cc, cam_fix_cc_tree
					(vc, (LIST) elem, cc_used_vars));
         }
      }			/* end loop_through_list (old_cc  */

   return (new_cc);
}					/* end cam_fix_cc_tree */


/* --------------------------------------------     cam_fix_var */
static cus_sym cam_fix_var (vc, old_var)
cus_sym vc;
cus_sym old_var;
{  cus_sym new_var;
   char new_name [257];
   int var_pos, new;

   new_var = old_var;
   if (old_var == car_self_symbol)
      {new_var = cus_add_sym ("!!! SLANG COMPILER ERROR", id_sym, &new,
			      NO_CONTEXT);
      cus_put_prop (new_var, is_ok_id_name_prop, 1);
      COMPILE_ERROR (fprintf (stderr,
	 "cannot use SELF in a creation condition\n");)
      }
   else if ((var_pos = cam_find_var (vc, old_var)) != -1)
      {sprintf (new_name, "PARM_%d", var_pos);
      new_var = cus_add_sym (new_name, id_sym, &new, NO_CONTEXT);
      cus_put_prop (new_var, is_ok_id_name_prop, 1);
      }
   else
      {DEBUG3 (fprintf (stderr, "cam_fix_var: couldn't fix the var = %s\n",
               cus_sym_representation (old_var)););
      }
   return (new_var);
}					/* end cam_fix_var */


/* --------------------------------------------     cam_fix_dot_tree */
static LIST cam_fix_dot_tree (vc, old_tree)
cus_sym vc;
LIST old_tree;
{  LIST new_tree;
   int tree_pos, new;
   char new_name [257];
   cus_sym new_var;

   DEBUG2 (fprintf (stderr, "cam_fix_dot_tree: enter\n   old tree =\n");
      cud_print_tree (stderr, old_tree, "   "););
   new_tree = old_tree;
   if ((tree_pos = cam_find_dot_tree (vc, old_tree)) != -1)
      {sprintf (new_name, "PARM_%d", tree_pos);
      new_var = cus_add_sym (new_name, id_sym, &new, NO_CONTEXT);
      cus_put_prop (new_var, is_ok_id_name_prop, 1);
      new_tree = make_list ();
      DEBUG3 (fprintf (stderr, "cam_fix_dot_tree: new new_tree = \n");
         cud_print_tree (stderr, new_tree, "   "););
      add_to_tail (new_tree, car_cc_dot_symbol);
      DEBUG3 (fprintf(stderr, "cam_fix_dot_tree: first part new_tree = \n");
         cud_print_tree (stderr, new_tree, "   "););
      add_to_tail (new_tree, new_var);
      DEBUG3 (fprintf(stderr,"cam_fix_dot_tree: second part new_tree = \n");
         cud_print_tree (stderr, new_tree, "   "););
      add_to_tail (new_tree, get_list_pos (old_tree, 3));
      DEBUG3 (fprintf(stderr,"cam_fix_dot_tree: third part new_tree = \n");
         cud_print_tree (stderr, new_tree, "   "););
      }
   DEBUG2 (fprintf (stderr, "cam_fix_dot_tree: exit\n   new_tree = \n");
      cud_print_tree (stderr, new_tree, "   "););
   return (new_tree);
}					/* end cam_fix_dot_tree */


/* --------------------------------------------     cam_find_dot_tree */
static int cam_find_dot_tree (vc, tree)
cus_sym vc;
LIST tree;
{  int found, parm_cnt, parm_list_len;
   char *look;
   LIST cc_used_vars;

   cc_used_vars = (LIST) cus_get_prop (vc, cc_used_vars_prop);
   found = 0;
   parm_list_len = list_length (cc_used_vars);
   for (parm_cnt = 1;
        parm_cnt <= parm_list_len && (found == 0);
	parm_cnt++)
      {look = get_list_pos (cc_used_vars, parm_cnt);
      if (look == get_list_pos (tree, 2))
         found = parm_cnt;
      }
   return (found - 1);
}					/* end cam_dot_tree */


/* --------------------------------------------     cam_find_var */
static int cam_find_var (vc, var)
cus_sym vc;
cus_sym var;
{  int found, parm_cnt, parm_list_len;
   char *look;
   LIST cc_used_vars;

   DEBUG2 (fprintf (stderr, "cam_find_var: enter\n\t");
     fprintf (stderr, "vc = %s\n", cus_sym_representation (vc)););
   cc_used_vars = (LIST) cus_get_prop (vc, cc_used_vars_prop);
   DEBUG3 (fprintf (stderr, "cam_find_var: cc used vars\n");
      cud_print_tree (stderr, cc_used_vars, ""););
   found = 0;
   parm_list_len = list_length (cc_used_vars);
   if (cus_is_sym (var))
      {DEBUG3 (fprintf (stderr, "cam_find_var: var is a sym & = %s\n",
	           cus_sym_representation (var)););
      for (parm_cnt = 1;
	    ((parm_cnt <= parm_list_len) && (found == 0));
	    parm_cnt++)
         {look = get_list_pos (cc_used_vars, parm_cnt);
         if (!strcmp (cus_sym_representation (look),
                      cus_sym_representation (var)))
            found = parm_cnt;
         }
      }

   DEBUG2 (fprintf (stderr, "cam_find_var:exit\n   return = %d\n",
                    (found - 1)););

   return (found - 1);
}					/* end cam_find_var */


#if  0
/* ??? take out cam_dot_equal, if it really is not needed */

/* --------------------------------------------     cam_dot_equal  */
static boolean cam_dot_equal (look, var)
LIST look;
LIST var;
{  
   if (((cus_sym) get_list_pos (look, 1) == car_dot_symbol) &&
       ((cus_sym) get_list_pos (var , 1) == car_dot_symbol))
      {if ((get_list_pos (look, 2) == get_list_pos (var, 2)) &&
           (get_list_pos (look, 3) == get_list_pos (var, 3)))
          return (TRUE);
       else
         return (FALSE);
      }
    else
      return (FALSE);
}					/* end cam_dot_equal */
#endif


/* --------------------------------------------     cam_fix_methods     */
void cam_fix_methods (obj)
cus_sym obj;
{  LIST else_elem, if_statement, condition_expression,
        then_statements, methods_list, method_snip;
   cus_sym method, method_att;
   int a_line_num, new;

		/* fix up the methods so they look like if statement trees,
	    	   this makes code generation easier, since the "standard"
		   if statement processing can be used
		*/

   else_elem = NULLlist;
   a_line_num = 0;

   if (methods_list = (LIST) cus_get_prop (obj, methods_prop))
      {method_att = cus_add_sym ("method", attribute_sym, &new, WITH_CONTEXT);
      if (!new)
         cus_put_prop (method_att, type_prop, attribute_sym);
      cus_put_prop (method_att, u_name_prop, cus_genname ("ATT"));
      cus_put_prop (method_att, parent_object_prop, obj);
      loop_through_list (methods_list, method, cus_sym)
         {if (method_snip = (LIST) cus_get_prop (method, value_snippet_prop))
            {if_statement = make_list();
            if (else_elem)
               add_to_head (if_statement, else_elem);
         
            then_statements = copy_list (method_snip, NOT_NODES);
				/* get rid of first element which is the
				   method_handler_snippet symbol
				*/
            get_from_head (then_statements);

   				/* get the line number for the lexically first
   				   method which has a snippet
   				*/
            if (!a_line_num)
               a_line_num = get_head_val (method_snip);
            add_to_head (if_statement, then_statements);
         
            condition_expression = make_list();
            add_to_head (condition_expression,
         		cus_string_constant (cus_sym_representation (method)));
            add_to_head (condition_expression, car_method_symbol);
            add_to_head (condition_expression, car_equal_symbol);
            add_to_head (if_statement, condition_expression);
         
            add_to_head_val (if_statement, car_if_symbol,
   			     get_head_val (method_snip));
         
            else_elem = make_list ();
            add_to_head (else_elem, if_statement);
            }		/* end if we have a method snippet */
         }		/* end loop through methods_list */
      }			/* end if (methods_list) */

				/* at end of loop, else_elem contains the
				   entire snippet, comprising all the methods.
				   Give it the line number of the first non-empty
				   method.
				*/
   if (else_elem)
      add_to_head_val
	(else_elem,
         cus_gensym ("fixed_method_snippet", snippet_sym),
	 a_line_num);

   cus_put_prop (obj, fixed_method_snippet_prop, else_elem);
   cus_put_prop (obj, fixed_method_prop, method_att);

   DEBUG3 (fprintf (stderr,
	   "cam_fix_methods: fixed_method_snippet_prop for %s =\n",
	   cus_sym_representation (obj));
      cud_print_tree (stderr, else_elem, "   "););

   return;
}					/* end cam_fix_methods */


/* 
???? cam_fix_atts must be able to handle "empty" code snippets
*/
/* --------------------------------------------     cam_fix_atts */
void cam_fix_atts (obj)
cus_sym obj;
{  LIST new_att_list, old_att_list;
   caddr_t rhs, s_name, att, combined_att;
   LIST combined_snippet, old_snippet;

   DEBUG2 (fprintf (stderr, "cam_fix_atts: enter\n   object = %s\n",
		    cus_sym_representation (obj)););

   combined_snippet = NULL;
   new_att_list = make_list ();
   if (old_att_list = (LIST) cus_get_prop (obj, attributes_prop))
      {loop_through_list (old_att_list, att, caddr_t)
         {DEBUG3 (fprintf (stderr, "cam_fix_atts: att = %s\n",
              cus_sym_representation (att)););
         old_snippet = (LIST) cus_get_prop (att, value_snippet_prop);
         s_name = get_list_pos (old_snippet, 1);
         rhs = get_list_pos (get_list_pos (old_snippet, 2), 3);
         if (cus_is_sym (s_name) &&
             ((cus_sym_type)cus_get_prop(s_name, type_prop)==assignment_sym) &&
             cus_is_sym (rhs) &&
             cus_get_prop (rhs, is_constant_prop))
            {DEBUG3(fprintf(stderr,"cam_fix_atts: got a combinable att in \n");
               cud_print_tree (stderr, old_snippet, ""););
            if (!combined_snippet)
               {combined_snippet = old_snippet;
               add_to_tail (new_att_list, att);
               }
             else
               {add_to_tail (combined_snippet, get_list_pos (old_snippet, 2));
               }
            }
          else
            {DEBUG3 (fprintf (stderr,
		     "cam_fix_atts: did NOT get a combinable att in \n");
               cud_print_tree (stderr, old_snippet, ""););
            add_to_tail (new_att_list, att);
            }
         }
      }
   DEBUG3 (fprintf (stderr, "cam_fix_atts: new attributes_prop = \n");
      cud_print_tree (stderr, new_att_list, ""););
   cus_put_prop (obj, attributes_prop, new_att_list);
   free_list (old_att_list, NOT_NODES);
   DEBUG3 (fprintf (stderr, "cam_fix_atts: enit\n"););
   return;
}


