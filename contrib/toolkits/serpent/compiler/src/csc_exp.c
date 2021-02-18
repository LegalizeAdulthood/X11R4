static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/csc_exp.c,v 1.20 89/10/23 19:54:36 ejh Exp $";

/*
$Log:	csc_exp.c,v $
 * Revision 1.20  89/10/23  19:54:36  ejh
 * defined memoryPack
 * 
 * Revision 1.19  89/09/22  13:52:19  serpent
 * no change
 * 
 * Revision 1.18  89/09/22  13:47:33  rcs
 * fixed extra paren error with string_constant_adi
 * 
 * Revision 1.17  89/09/20  12:02:46  little
 * fix forward referencing to work for attributes and references to
 * attributes which are not "defined" in the slang program.  spr #0104
 * 
 * Revision 1.16  89/07/24  14:53:49  little
 * ops removal
 * 
 * Revision 1.15  89/04/25  09:18:01  ljb
 * generate code for user defined functions
 * (this hasn't been tested yet, I am checking this in so
 * that Reed can fix another bug
 * 
 * Revision 1.14  89/03/28  12:56:47  serpent
 * changed  #ifdef 0  to  #if 0  to allow sun compile.
 * 
 * Revision 1.13  89/02/27  17:08:39  little
 * add some debug print
 * 
 * Revision 1.12  89/02/09  16:31:35  ejh
 * added copyright notice
 * 
 * Revision 1.11  89/02/03  16:00:17  little
 * change so that simple data item references are actually de-references
 * 
 * Revision 1.10  89/02/01  16:18:23  little
 * allow such attributes as -->  width: command.width, where width
 * is "overloaded"
 * 
 * Revision 1.9  89/01/26  17:33:41  little
 * add capability for lhs of assignment to be a qualified name (ie, use dot
 * notation)
 * 
 * Revision 1.8  89/01/17  16:56:07  little
 * provide for "forward" reference and "single" vc dot notation meaning
 * you can have the following: vc, vc.obj, vc.obj.att, vc.var, obj.att
 * vc.bound_sd_item, vc.bound_sd_item.att, bound_sd_item.att, bound_sd_item
 * as a reference
 * 
 * Revision 1.7  89/01/11  19:01:15  little
 * add and modify debug print
 * 
 * Revision 1.6  89/01/09  16:30:10  little
 * make forward reference to items work
 * .,
 * 
 * Revision 1.5  88/12/15  13:56:08  little
 * fix reference to simple "visible" data items.
 * 
 * Revision 1.4  88/12/14  10:47:16  little
 * start adding dot notation.  the coded added for ture dot notation (using
 * the dot qualifier) does not yet work.  however, accessing any name
 * (vc, object, var, att) which is visible due to scope now works.
 * 
 * Revision 1.3  88/10/31  15:13:34  little
 * change csc_gets_write to print out the representation of the lhs instead
 * of sel when it gets an attribute_sym.  this allows use to "collect"
 * attributes which are set to constant expressions into one
 * routine.
 * 
 * Revision 1.2  88/10/04  17:34:55  little
 * add log comment.
 * add some debug code to csc_exp_write
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
#include "ca2.h"
#include "csc.h"
#include "cus.h"
#include "cud.h"
#include "idd.h"
#include "isd.h"

void csc_reference_write ();
static void csc_find_reference ();
static boolean csc_exp_in_cc;


#define INVALID_C_CODE "ERROR, DO NOT COMPILE\n"
#define GET TRUE
#define PUT FALSE

#define redo_bound_prop(sym,prop,save_list,new_value) \
      add_to_tail_val (save_list, \
		       cus_get_prop (sym, prop), \
                       prop); \
      cus_put_prop (sym, prop, new_value);



/*--------------------------------------------------------------------------*\
|  Routine: csc_look_in_elem
|
|  Description:
|     looks in the element type of the object to see if the candidate is
|	a component
\*--------------------------------------------------------------------------*/
static cus_sym csc_look_in_elem (candidate, obj)
cus_sym candidate;
cus_sym obj;
{  LIST look_list;
   cus_sym found;
   string obj_type, cand_name;
   int the_length, the_offset, the_position;
   idd_data_definition the_dd;

   cand_name = (string) cus_sym_representation (candidate);
   DEBUG2 (fprintf(stderr,"csc_look_in_elem enter: candidate = %s, obj = %s\n",
		   cand_name, cus_sym_representation (obj)););

   obj_type = cus_sym_representation (cus_get_prop (obj, object_type_prop));
   the_dd = (idd_data_definition) cus_get_prop (
				     cus_get_prop (obj, data_definition_prop),
				     idd_handle_prop);
   found = (cus_sym) NULL;
   if (the_dd)
				/* now make sure that the candidate is really
				   a component of the obj
				*/
      {if (idd_validate_component (the_dd, obj_type, cand_name))
         found = candidate;
      }

   DEBUG2 (fprintf (stderr, "csc_look_in_elem exit: found = %s\n",
		    cus_sym_representation (found)););
   return (found);
}					/* end csc_look_in_elem */


/* -------------------------------------------     csc_try_redefined */
static boolean csc_try_redefined (stream, ref, self)
FILE *stream;
cus_sym ref;
cus_sym self;
/*
??? how about selfs which are other than attributes ?
*/
{  cus_sym parent_vc, obj, found;
   LIST objects;
   boolean redefine_it;
   string ref_string;

   DEBUG2 (fprintf (stderr, "csc_try_redefined enter: ref = %s, self = %s\n",
                    cus_sym_representation (ref),
                    cus_sym_representation (self)););

   redefine_it = FALSE;
   ref_string = cus_sym_representation (ref);
   found = NULL;
   objects = (LIST) NULL;

				/* if we are in an attribute snippet */
   if ((cus_sym_type) cus_get_prop (self, type_prop) == attribute_sym)
      {
				/* check to see if atribute is a component
				   of the object itself by looking into
				   the object type element
				*/
      if (!redefine_it)
         {obj = (cus_sym) cus_get_prop (self, parent_object_prop);
         redefine_it = (boolean) csc_look_in_elem (ref, obj);
         if (redefine_it)
            cus_put_prop (ref, redefined_prop, obj);

         }
				/* see if there is another object in the
				   parent vc with this name
				*/
      if (!redefine_it &&
          (parent_vc = (cus_sym) cus_get_prop (
                                  cus_get_prop (self, parent_object_prop),
   			          parent_vc_prop)) &&
          (objects = (LIST) cus_get_prop (parent_vc, objects_prop)))
         {objects = copy_list (objects, NOT_NODES);
         obj = (cus_sym) get_list_next (objects, NULL);
         DEBUG3 (fprintf (stderr,
   		       "\tlooking for potentials, # objects = %d\n",
   		       list_length (objects)););
         while ((obj != (cus_sym) NULL) && (found == (cus_sym) NULL))
            {DEBUG3 (fprintf (stderr, "\tobj = %s\n",
   			   cus_sym_representation (obj)););
            if (!strcmp (cus_sym_representation (obj), ref_string))
               {found = obj;
               DEBUG3 (fprintf (stderr, "\tfound = %s\n",
   		       cus_sym_representation (found)););
               cus_put_prop (ref, redefined_prop, obj);
               redefine_it = TRUE;
               }
             else
               obj = (cus_sym) get_list_next (objects, obj);
            }
         }
      }

   if (!redefine_it)
      {COMPILE_ERROR (fprintf (stderr,
             "csc_try_redefined: %s is probably undefined\n",
             ref_string););
      fprintf (stream, INVALID_C_CODE);
      }

   if (objects)
      free_list (objects, NOT_NODES);

   DEBUG2 (fprintf (stderr, "csc_try_redefined exit: redefine_it = %d\n",
		    redefine_it););

   return (redefine_it);
}					/* end csc_try_redefined */


/* -------------------------------------------     csc_names_equal */
static int csc_names_equal (ptr, element)
NODE ptr;
NODE element;
{  boolean r_equal;
   string ele_name;

   ele_name = cus_sym_representation (element);
   DEBUG2 (fprintf (stderr, "csc_names_equal enter: ptr = %s, element = %s\n",
	            ptr, ele_name););

   r_equal = (!strcmp (ptr, ele_name));

   DEBUG2 (fprintf (stderr, "csc_names_equal exit: r_equal = %d\n", r_equal););
   return (r_equal);
}					/* end csc_names_equal */


/* -------------------------------------------     csc_look_in_vc */
static cus_sym csc_look_in_vc (candidate, vc)
cus_sym candidate;
cus_sym vc;
{  LIST look_list;
   cus_sym found;
   string cand_name;

   cand_name = (string) cus_sym_representation (candidate);
   DEBUG2 (fprintf(stderr, "csc_look_in_vc enter: candidate = %s, vc = %s\n",
		   cand_name, cus_sym_representation (vc)););

   found = (cus_sym) NULL;

   look_list = (LIST) cus_get_prop (vc, variables_prop);
   if (!found && look_list)
      found = (cus_sym) search_list (look_list, cand_name, csc_names_equal);

   look_list = (LIST) cus_get_prop (vc, objects_prop);
   if (!found && look_list)
      found = (cus_sym) search_list (look_list, cand_name, csc_names_equal);

   look_list = (LIST) cus_get_prop (vc, sub_vcs_prop);
   if (!found && look_list)
      found = (cus_sym) search_list (look_list, cand_name, csc_names_equal);

/* ?? do we need to worry about the "creating sd"? (i {reed} think not) */

   DEBUG2 (fprintf (stderr, "csc_look_in_vc exit: found = %s\n",
		    cus_sym_representation (found)););
   return (found);
}					/* end csc_look_in_vc */


/* -------------------------------------------     csc_dot_comp_write*/
static void csc_dot_comp_write (stream, exp, self, context, elem_num, att_num,
				is_get)
FILE *stream;
LIST exp;
cus_sym self;
cus_context_type context;
int elem_num, att_num;
boolean is_get;
{  cus_sym elem, att;
   int the_length, the_offset, the_position;
   idd_data_definition ill;

   DEBUG2 (fprintf (stderr,
	"csc_dot_comp_write: enter\n\telem_num = %d, att_num = %d\n",
	elem_num, att_num););

   att = (cus_sym) get_list_pos (exp, att_num);
   elem = (cus_sym) get_list_pos (exp, elem_num);

   DEBUG3 (fprintf (stderr, "\telement= %s, att = %s\n",
		  cus_sym_representation (elem),
		  cus_sym_representation (att)););

   ill = (idd_data_definition) cus_get_prop (cus_get_prop (elem,
							  data_definition_prop),
                                             idd_handle_prop);

   DEBUG3 (fprintf (stderr, "\till = %x\n", ill););

   if (!idd_validate_component (
          ill,
          cus_sym_representation (cus_get_prop (elem, object_type_prop)),
          cus_sym_representation (att)))
      {COMPILE_ERROR (fprintf (stderr,
  	    "csc_dot_comp_write: elem = %s does not contain attribute = %s\n",
	    cus_sym_representation (elem),
	    cus_sym_representation (att)););
      fprintf (stream, INVALID_C_CODE);
      goto DONE;
      }

   csc_find_reference (stream, att, context,
      		       cus_get_prop (elem, owned_context_prop),
      		       self,
		       cus_sym_representation (elem), is_get);

DONE:
   DEBUG2 (fprintf (stderr, "csc_dot_comp_write exit\n");)
   return;
}					/* end csc_dot_comp_write */


/* -------------------------------------------     csc_try_bound_elem */
static LIST csc_try_bound_elem (cand_sym)
cus_sym cand_sym;
{  cus_sym ill, target_vc;
   cus_context_type context;
   LIST old_values, ret_val;

   DEBUG2 (fprintf (stderr, "csc_try_bound_elem enter: cand_sym = %s, %x\n",
		    cus_sym_representation (cand_sym), cand_sym););

   ret_val = (LIST) NULL;
   if (target_vc = (cus_sym) cus_get_prop (cand_sym, created_vcs_prop))
      {DEBUG3 (fprintf (stderr,
			"\tfound bound sd element = %s\n",
			cus_sym_representation (cand_sym)););
      ill = ca2_get_sd_sym (cus_sym_representation (cand_sym));
      if (!ill)
        FATAL_ERROR (fprintf (stderr,
			"csc_try_bound: could not find ill file for %s\n",
			cus_sym_representation (cand_sym)););
      old_values = make_list ();

      redo_bound_prop (cand_sym, data_definition_prop, old_values, ill);
      redo_bound_prop (cand_sym, object_type_prop, old_values, cand_sym);
      redo_bound_prop (cand_sym, type_prop, old_values, bound_elem_sym);
      redo_bound_prop (cand_sym, parent_vc_prop, old_values, target_vc);

      ret_val = old_values;
      }

   DEBUG2 (fprintf (stderr, "csc_try_bound_elem exit: ret_val = %x\n",
		    ret_val););
   return (ret_val);
}					/* end csc_try_bound_elem */


/* -------------------------------------------     csc_restore_bound_elem */
static void csc_restore_bound_elem (sym, old_values)
cus_sym_type sym;
LIST old_values;
{  caddr_t value;
   cus_property_type prop;

   DEBUG2 (fprintf (stderr, "csc_restore_bound_elem enter: sym = %s\n",
	cus_sym_representation (sym)););

   if (old_values)
      {prop =  (cus_property_type) get_head_val (old_values);
      value = get_from_head (old_values);
      while (value)
         {cus_put_prop (sym, prop, value);
         prop = (cus_property_type) get_head_val (old_values);
         value = get_from_head (old_values);
         }
      }

   free_list (old_values, NOT_NODES);

   DEBUG2 (fprintf (stderr, "csc_restore_bound_elem exit:\n"););
   return;
}					/* end csc_restore_bound_elem */


/* -------------------------------------------     csc_object_dot_write */
static void csc_object_dot_write (stream, exp, self, is_get, context)
FILE *stream;
LIST exp;
cus_sym self;
boolean is_get;
cus_context_type context;
{  cus_sym_type old_type;

   if (!get_list_pos (exp, 3))
      FATAL_ERROR (fprintf (stderr,
	   "csc_object_dot_write: could not find the att in obj.att\n"););
   old_type = (cus_sym_type) cus_get_prop (get_list_pos (exp, 3),
			 		   type_prop);
   cus_put_prop (get_list_pos (exp, 3), type_prop, attribute_sym);
   csc_dot_comp_write (stream, exp, self, context, 2, 3, is_get);
   cus_put_prop (get_list_pos (exp, 3), type_prop, old_type);
   return;
}


/* -------------------------------------------     csc_dot_write*/
static void csc_dot_write (stream, exp, self, is_get)
FILE *stream;
LIST exp;
cus_sym self;
boolean is_get;
{  char * elem;
   int counter;
   cus_sym_type first_type, second_type;
   cus_sym look_obj, target_vc, var, parent, obj;
   cus_context_type context;
   LIST obj_list, save_props;
   cus_sym_type old_type;
   boolean found_it;
   string target;


/* this guy will "handle" all data item references and will NOT
   invoke csc_exp_write recursively */

   DEBUG2 (fprintf (stderr, "csc_dot_write enter\n");
          fprintf (stderr, "\texp = \n");
	  cud_print_tree (stderr, exp, "\t\t");
          fprintf (stderr, "self = %s, is_get = %d\n",
		cus_sym_representation (self), is_get););

   context = cus_current_context ();

   DEBUG3 (fprintf (stderr, "\tcurrent context = %d\n", context);
          fprintf (stderr, "\tdot tree element types & contexts\n");
          loop_through_list (exp, elem, char *)
             fprintf (stderr, "\t\t%s %d\n",
		      cus_type_print_val (cus_get_prop (elem, type_prop)),
		      cus_sym_context (elem)););

   first_type = (cus_sym_type) cus_get_prop (get_list_pos (exp, 2),
					       type_prop);
   DEBUG3 (fprintf (stderr, "\tfirst_type = %s\n",
		    cus_type_print_val (first_type)););
   switch (first_type)
      {case vc_sym:
         second_type = (cus_sym_type) cus_get_prop (get_list_pos (exp, 3),
						      type_prop);
         DEBUG3 (fprintf (stderr, "\tsecond_type = %s\n",
		    cus_type_print_val (second_type)););
         switch (second_type)
            {case object_sym:
               obj = csc_look_in_vc (get_list_pos (exp, 3),
				     get_list_pos (exp, 2));
      					/* if vc does not contain object, we
      					   have a user error */
               if (!obj)
                  {COMPILE_ERROR (fprintf (stderr,
            	  "csc_dot_write: vc = %s does not contain object = %s\n",
            		   cus_sym_representation (get_list_pos (exp, 2)),
            		   cus_sym_representation (get_list_pos (exp, 3))););
                  fprintf (stream, INVALID_C_CODE);
                  goto DONE;
                  }
               if (get_list_pos (exp, 4))
                  {old_type = (cus_sym_type) cus_get_prop (get_list_pos
							     (exp, 4),
							   type_prop);
                  cus_put_prop (get_list_pos (exp, 4), type_prop,
				 attribute_sym);
	          csc_dot_comp_write (stream, exp, self, context, 3, 4, is_get);
                  cus_put_prop (get_list_pos (exp, 4), type_prop,
				 old_type);
                  }
                else
                  {parent = (cus_sym) cus_get_prop (obj, parent_vc_prop);
                  if (!parent)
                     FATAL_ERROR (fprintf (stderr,
            "csc_dot_write: cannot get parent object for att in obj.att\n"););
                  csc_find_reference (stream, obj, context,
			         cus_get_prop (parent, owned_context_prop),
				 self, "ERROR FROM csc_dot_write 0", is_get);
                  }
               break;

            case variable_sym:
               var = csc_look_in_vc (get_list_pos (exp, 3),
				     get_list_pos (exp, 2));
      					/* if vc does not contain vart, we
      					   have a user error */
               if (!var)
                  {COMPILE_ERROR (fprintf (stderr,
      		  "csc_dot_write: vc = %s does not contain variable = %s\n",
      		   cus_sym_representation (get_list_pos (exp, 2)),
      		   cus_sym_representation (get_list_pos (exp, 3))););
                  fprintf (stream, INVALID_C_CODE);
                  goto DONE;
                  }
               parent = (cus_sym) cus_get_prop (var, parent_vc_prop);
               csc_find_reference (stream, var, context,
      			           cus_get_prop (parent, owned_context_prop),
      				   self, "ERROR FROM csc_dot_write 1", is_get);
               break;

            case id_sym:
            case bound_elem_sym:
				/* if this is the creating sd as
				   indicated by having the created_vcs_prop
				   set */
               if (save_props = csc_try_bound_elem (get_list_pos (exp, 3)))
                  {DEBUG3 (fprintf (stderr,
			"\t0 found bound sd element = %s\n",
			cus_sym_representation (get_list_pos (exp, 3))););
                  if (get_list_pos (exp, 4))
                     {old_type = (cus_sym_type) cus_get_prop (get_list_pos
								(exp, 4),
					                      type_prop);
                     cus_put_prop (get_list_pos (exp, 4), type_prop,
				    bound_comp_sym);
                     csc_dot_comp_write (stream, exp, self, context, 3, 4,
					 is_get);
                     cus_put_prop (get_list_pos (exp, 4), type_prop,
				    old_type);
                     }
                   else
                     {target_vc = (cus_sym)cus_get_prop (get_list_pos (exp, 3),
							  parent_vc_prop);
                     csc_find_reference (stream, get_list_pos(exp,3), context,
   			         cus_get_prop (target_vc, owned_context_prop),
   				 self, "ERROR FROM csc_dot_write 2", is_get);
                     }
                  csc_restore_bound_elem (get_list_pos (exp, 3), save_props);
                  }
                 else
                   {COMPILE_ERROR (fprintf (stderr,
		      "csc_dot_write: 0 vc = %s does not contain item = %s\n",
		      cus_sym_representation (get_list_pos (exp, 2)),
		      cus_sym_representation (get_list_pos (exp, 3))););
                   fprintf (stream, INVALID_C_CODE);
                   goto DONE;
                   }
               break;

            default:
                COMPILE_ERROR (fprintf (stderr,
		      "csc_dot_write: 1 vc = %s does not contain item = %s\n",
		      cus_sym_representation (get_list_pos (exp, 2)),
		      cus_sym_representation (get_list_pos (exp, 3))););
                fprintf (stream, INVALID_C_CODE);
                goto DONE;
            }
         break;

      case object_sym:
         csc_object_dot_write (stream, exp, self, is_get, context);
         break;


      case id_sym:
         DEBUG3 (fprintf (stderr, "\t1st item is id_sym\n"););
				/* this might be a forward reference, see if
				   the first element is another object in the
				   parent vc
				*/

				/* first find the parent vc */
         if ((cus_sym_type) cus_get_prop (self, type_prop) == attribute_sym)
            {parent = (cus_sym) cus_get_prop (self, parent_object_prop);
            if (parent)
               parent = (cus_sym) cus_get_prop (parent, parent_vc_prop);
            }
           else
            parent = NULL;
				/* now look through the obj list for the
				   reference
				*/
         found_it = FALSE;
         if ((parent) &&
             (obj_list = (LIST) cus_get_prop (parent, objects_prop)))
            {look_obj = (cus_sym) get_list_next (obj_list, NULL);
            target = cus_sym_representation (get_list_pos (exp, 2));
            DEBUG3 (fprintf (stderr, "csc_dot_write: target = %s\n", target););
            while ((look_obj != NULL) && (!found_it))
               {found_it = !strcmp (target, cus_sym_representation (look_obj));
               DEBUG3 (fprintf (stderr, "csc_dot_write: look = %s\n",
               			cus_sym_representation (look_obj)););
               if (!found_it)
                  look_obj = (cus_sym) get_list_next (obj_list, look_obj);
               }
            }
				/* found the reference in the parent vc's
				   object list.  replace the symbol in the dot
				   tree with the "real" one and access like
				   an object.att reference
				*/
         if (found_it)
            {delete_list_pos (exp, 2);
            insert_list_pos (exp, look_obj, 2);
            csc_object_dot_write (stream, exp, self, is_get, context);
            break;
            }

					/* %%%%%%%%%%%%%%%%%%%%%%% */
					/* FALL THROUGH IS OK HERE */
					/* %%%%%%%%%%%%%%%%%%%%%%% */

/* ?? do we need to do a refine somehere around here ??*/

      case bound_elem_sym:

				/* if this is the creating sd as
				   indicated by having the created_vcs_prop
				   set */
         if (save_props = csc_try_bound_elem (get_list_pos (exp, 2)))
            {DEBUG3 (fprintf (stderr,
			"\t1 found bound sd element = %s\n",
			cus_sym_representation (get_list_pos (exp, 2))););
            if (get_list_pos (exp, 3))
               {old_type = (cus_sym_type) cus_get_prop (get_list_pos(exp, 3),
							type_prop);
               cus_put_prop (get_list_pos(exp, 3), type_prop, bound_comp_sym);
               csc_dot_comp_write (stream, exp, self, context, 2, 3, is_get);
               cus_put_prop (get_list_pos(exp, 3), type_prop, old_type);
               }
             else
               {target_vc = (cus_sym)cus_get_prop (get_list_pos (exp, 3),
		 				   parent_vc_prop);
               csc_find_reference (stream, get_list_pos(exp,2), context,
   			         cus_get_prop (target_vc, owned_context_prop),
   				 self, "ERROR FROM csc_dot_write 3", is_get);
               }
             csc_restore_bound_elem (get_list_pos (exp, 2), save_props);
             }
          else
            {COMPILE_ERROR (fprintf (stderr,
		        "csc_dot_write: 0 %s cannot be accessed\n",
			     cus_sym_representation (get_list_pos (exp, 2))););
            fprintf (stream, INVALID_C_CODE);
            goto DONE;
            }
          break;

      default:
         COMPILE_ERROR (fprintf (stderr,
	        "csc_dot_write: 1 %s cannot be accessed\n",
		 cus_sym_representation (get_list_pos (exp, 2))););
            fprintf (stream, INVALID_C_CODE);
            goto DONE;
      }

DONE:
   DEBUG2 (fprintf (stderr, "csc_dot_write exit\n"););

   return;
}					/* end csc_dot_write */


/*
??? csc_cc_dot_write only works for pseudo dot notation in creation conditions
only sd gets are supported, it should probably go away when real dot notation
comes in
*/
/* -------------------------------------------     csc_cc_dot_write*/
static void csc_cc_dot_write (stream, exp, self)
FILE *stream;
LIST exp;
cus_sym self;
{  cus_sym element, component, ill_entry;
   idd_data_types data_type;

   DEBUG2 (fprintf (stderr, "csc_cc_dot_write enter: exp = \n");
	  cud_print_tree (stderr, exp, "\t");
          fprintf (stderr, "self = %s\n",
		cus_sym_representation (self)););

   element = (cus_sym) get_list_pos (exp, 2);
   if (!cus_is_sym (element))
      {COMPILE_ERROR (fprintf (stderr, "first parameter to dot not symbol"););
      return;
      }
   component = (cus_sym) get_list_pos (exp, 3);
   if (!cus_is_sym (component))
      {COMPILE_ERROR (fprintf (stderr, "second parameter to dot not symbol"););
      return;
      }
/*
?? add error checking for legal element.component reference in cc
the follwoing code, does some of the error checking
*/
#if  0
   ill_entry = ca2_get_sd_sym (cus_sym_representation (element));
   if (ill_entry == NULL)
      {COMPILE_ERROR (fprintf (stderr, "%s not in shared data\n",
		 cus_sym_representation (element)););
      return;
      }
   data_type = isd_get_shared_data_type (cus_sym_representation (element),
					 cus_sym_representation (component));
   if (get_status ())
      {COMPILE_ERROR (fprintf (stderr, "%s not a component of %s\n",
		cus_sym_representation (element),
		cus_sym_representation (component)););
      return;
      }
#endif

   fprintf (stream, "dra_get_component_value (\n");
   fprintf (stream, "   dob_make_id_constant_adi (%s), \n",
	cus_sym_representation (element));
   fprintf (stream, "   dob_make_string_constant_adi (\"%s\")   )\n",
	cus_sym_representation (component));

   DEBUG2 (fprintf (stderr, "csc_cc_dot_write exit\n"););
   return;
}					/* end csc_cc_dot_write */


/* -------------------------------------------     csc_exp_write*/
void csc_exp_write (stream, exp, self, in_cc)
FILE *stream;
LIST exp;
cus_sym self;
boolean in_cc;
{  int cnt, param_offset;
   cus_sym pseudo_fun_name;

   set_status (ok);
   csc_exp_in_cc = in_cc;

   fprintf (stream, "(" );

   if (cus_is_sym (exp))
      /* expression is a single symbol.  ie. constant or variable*/
      {if (cus_get_prop (exp, is_constant_prop))
         csc_constant_write (stream, exp, self);
       else if ((cus_sym) exp == car_undefined_symbol)
         fprintf (stream, "dob_make_undefined_adi () \n");
       else
         csc_reference_write (stream, exp, self);
      }
    else

      /*expression is compound expression.  ie. function or 
        dot notation (arithmetic operators are functions)*/

      {pseudo_fun_name = (cus_sym) get_list_pos (exp, 1);
      DEBUG3 (fprintf (stderr, "csc_exp_write: pseudo_fun_name 1 = %s\n",
	       cus_sym_representation (pseudo_fun_name)););
      if (pseudo_fun_name == car_function_call_symbol)
         {pseudo_fun_name = (cus_sym) get_list_pos (exp, 2);
         param_offset = 2;
         }
       else
         {param_offset = 1;
         }

      DEBUG3 ( fprintf (stderr, "csc_exp_write: pseudo_fun_name 2 = %s\n",
	       cus_sym_representation (pseudo_fun_name));
         if (!strcmp (cus_sym_representation (pseudo_fun_name),
					   "put_variable_value"))
            {fprintf (stderr, "csc_exp_write: got put_variable_value\n");
            });

      if (pseudo_fun_name == car_dot_symbol)
         csc_dot_write (stream, exp, self, GET);
       else if (pseudo_fun_name == car_cc_dot_symbol)
          csc_cc_dot_write (stream, exp, self);
       else
         csc_fun_call_write (stream, exp, self, param_offset);
      }

   fprintf (stream, ")\n" );
   cs2_fflush (stream);

   return;
}					/*  end csc_exp_write */


/* -------------------------------------------     csc_get_parent */
static cus_sym csc_get_parent (child)
cus_sym child;
{  cus_sym the_type, ret_val;

   DEBUG2 (fprintf (stderr, "csc_get_parent enter: child = %s\n",
    			    cus_sym_representation (child)););

   ret_val = (cus_sym) NULL;
   the_type = (cus_sym) cus_get_prop (child, type_prop);

   switch (the_type)
      {case vc_sym:
      case object_sym:
      case variable_sym:
         ret_val = (cus_sym) cus_get_prop (child, parent_vc_prop);
         break;
      case bound_comp_sym:
      case attribute_sym:
         ret_val = (cus_sym) cus_get_prop (child, parent_object_prop);
         break;
      default:
         FATAL_ERROR (fprintf (stderr,
			"csc_get_parent got a child of unknown type, %s\n",
			cus_type_print_val (the_type)););
         break;
      }

   DEBUG2 (fprintf (stderr, "csc_get_parent exit: child = %s\n",
    			    cus_sym_representation (child)););
   return (ret_val);
}					/* end csc_get_parent */


/* -------------------------------------------     csc_contexts_equal */
static int csc_contexts_equal (ptr, element)
NODE ptr;
NODE element;
{
   return (ptr == cus_get_prop (element, owned_context_prop));
}


/* -------------------------------------------     csc_context_climb_out */
boolean csc_context_climb_out (stream, context, target_context, self)
FILE *stream;
cus_sym self;
cus_context_type context;
cus_context_type target_context;
{  cus_sym obj, parent;
   cus_sym_type the_type;
   LIST look_list;
   boolean ret_val;

   DEBUG2 (fprintf (stderr,
"csc_context_climb_out enter: self = %s, context = %d, target_context = %d\n",
    cus_sym_representation (self), context, target_context););

   the_type = (cus_sym_type) cus_get_prop (self, type_prop);
   DEBUG3 (fprintf (stderr, "csc_context_climb_out: self type = %s\n",
	   cus_type_print_val (the_type)););

/* ?? do we need to worry about sd bound to created vc ?? */
/* if so, look in cus_put_prop (vc, creating_sd_prop, parm); */

   if (context != target_context)
      {DEBUG3 (fprintf (stderr,
			"csc_context_climb_out: not in target context\n"););
      if (the_type == vc_sym)
         {look_list = (LIST) cus_get_prop (self, objects_prop);
         if (look_list &&
             (obj = (cus_sym) search_list (look_list, target_context,
					 csc_contexts_equal)))
            {DEBUG3 (fprintf (stderr,
      			"csc_context_climb_out: at target object\n"););
            fprintf (stream, "dob_make_id_constant_adi (SELF)\n");
            ret_val = TRUE;
            goto DONE;
            }
         }

      parent = csc_get_parent (self);
      if (!parent)
         {ret_val = FALSE;
         goto DONE;
         }
      if (the_type == vc_sym)
         {DEBUG3 (fprintf (stderr,
			   "csc_context_climb_out: going for parent\n"););
         fprintf (stream, "dra_get_parent_vc (\n");
         }
      ret_val = csc_context_climb_out (stream, cus_get_prop (parent,
				       owned_context_prop),
		    	     	       target_context, parent);
      if (the_type == vc_sym)
         fprintf (stream, ")\n");
      }
    else
      {DEBUG3 (fprintf (stderr,
			"csc_context_climb_out: at target vc\n"););
      fprintf (stream, "dob_make_id_constant_adi (SELF)\n");
      ret_val = TRUE;
      }

DONE:
   DEBUG2 (fprintf (stderr, "csc_context_climb_out exit: ret_val = %d\n\n",
		    ret_val););
   return (ret_val);
}					/* end csc_context_climb_out */


/* -------------------------------------------     csc_climb_out */
static boolean csc_climb_out (stream, ref, context, target_context, self)
FILE *stream;
cus_sym ref;
cus_context_type context;
cus_context_type target_context;
cus_sym self;
{  cus_sym guy, the_type;
   LIST guys_to_unredefine;
   boolean ret_val, need_rite_paren;

   DEBUG2 (fprintf (stderr, "csc_climb_out enter: ");
    fprintf (stderr,"ref = %s, context = %d, target_context = %d, self = %s\n",
        cus_sym_representation (ref), context, target_context,
        cus_sym_representation (self)););

   guys_to_unredefine = make_list ();

TRY_AGAIN:
   need_rite_paren = false;

   the_type = (cus_sym) cus_get_prop (self, type_prop);
   DEBUG3 (fprintf (stderr, "csc_path_build: self's type = %s\n",
	   cus_type_print_val (the_type)););

   switch (the_type)
				/* need to get to a thingie which has context,
				   atts and vars do not have context. */
      {case attribute_sym:
      case variable_sym:
      case bound_comp_sym:
         DEBUG3 (fprintf (stderr,
		"csc_climb_out: getting self's parent\n"););
         fprintf (stream, "dra_get_parent_vc (\n");
         need_rite_paren = true;
         break;
      case id_sym:
         if (csc_try_redefined (stream, ref, self))
            {add_to_tail (guys_to_unredefine, ref);
            goto TRY_AGAIN;
            }
         return;
         break;
				/* need do nothing for vc or obj, already 
				   "in" self's context */ 
      case vc_sym:
      case object_sym:
         break;
      default:
         FATAL_ERROR (fprintf (stderr,
	     "csc_climb_out: found unknown type of %d for self named %s\n",
	     the_type, cus_sym_representation (self)));
      }      

   ret_val = csc_context_climb_out (stream, context, target_context, self);

   if (need_rite_paren)
      fprintf (stream, ")\n");

   loop_through_list (guys_to_unredefine, guy, cus_sym)
      cus_put_prop (guy, redefined_prop, NULL);
   free_list (guys_to_unredefine, NOT_NODES);

   DEBUG2 (fprintf (stderr, "csc_climb_out exit: ret_val = %d\n", ret_val););
   return (ret_val);
}					/* end csc_climb_out */


/* -------------------------------------------     csc_find_reference */
static void csc_find_reference (stream, ref, context, target_context, self,
				object_name, is_get)
FILE *stream;
cus_sym ref;
cus_context_type context;
cus_context_type target_context;
cus_sym self;
string object_name;
boolean is_get;
{  cus_sym_type ref_type, self_type;
   cus_sym good_ref;
   boolean need_ref,need_object, need_parent, need_extra_rite_paren, need_more;
   char ref_code [256];
   char get_put [10];

   DEBUG2 (fprintf (stderr, "csc_find_reference enter: ");
    fprintf (stderr,
       "ref = %s, context = %d, target_context = %d, self = %s, is_get = %d\n",
        cus_sym_representation (ref), context, target_context,
        cus_sym_representation (self), is_get););

  if (is_get)
      strcpy (get_put, "get");
   else
      strcpy (get_put, "put");

   self_type = (cus_sym_type) cus_get_prop (self, type_prop);
   ref_type =  (cus_sym_type) cus_get_prop (ref, type_prop);
   good_ref = (cus_sym) NULL;
   need_more = TRUE;
   need_parent = FALSE;
   need_object = FALSE;
   need_extra_rite_paren = FALSE;
   need_ref = FALSE;

				/* look for the reference in the local area */

   DEBUG3 (fprintf (stderr, "\tself_type = %s, ref_type = %s\n",
		    cus_type_print_val (self_type),
		    cus_type_print_val (ref_type));)

   if (context != target_context)
      goto SKIP_LOCAL_CHECK;

   switch (self_type)
				/* if we are in a variable snippet, look for
				   the reference in the parent vc */
				   
      {case variable_sym:
         good_ref = csc_look_in_vc (ref,
					cus_get_prop (self, parent_vc_prop));
         break;
				/* if we are in a attribute, first look in the
				   parent object. if nothing there, then look
				   in the parent vc */
      case attribute_sym:
         if (good_ref =
	        csc_look_in_elem (ref, cus_get_prop (self, parent_object_prop)))
/*
?? what about just making and adi with self & comp name, instead of calling
    get comp value
*/
            {fprintf (stream, "dra_%s_component_value (", get_put);
	    fprintf (stream, "dob_make_id_constant_adi (SELF),");
            fprintf (stream, "dob_make_string_constant_adi (\"%s\")%s\n",
                     cus_sym_representation (good_ref), is_get?")": "");
            need_more = FALSE;
            break;
            }
         good_ref =
	        csc_look_in_vc (ref, cus_get_prop (
				      cus_get_prop (self, parent_object_prop),
				      parent_vc_prop));
         break;
				/* if we are in create or destroy action
				   snippet (the type of self is vc), just look
				   in the vc */
       case vc_sym:
         good_ref = csc_look_in_vc (ref, self);
         break;
      default:
         COMPILE_ERROR (fprintf (stderr,
		 "csc_find_reference: got an illegal self type of %s\n",
		cus_type_print_val (self_type)););
         fprintf (stream, INVALID_C_CODE);
         good_ref = (cus_sym) TRUE;
         need_more = FALSE;
         break;
      }

SKIP_LOCAL_CHECK:
   DEBUG3 (fprintf (stderr, "\tneed_more = %d, good_ref = %x\n",
		    need_more, good_ref););

   if (need_more)
      {need_parent = (self_type != vc_sym) && good_ref;
      DEBUG3 (fprintf (stderr, "\tneed_parent = %d\n", need_parent););
      switch (ref_type)
         {case object_sym:
            sprintf (ref_code, "dra_%s_object", get_put);
            need_ref = TRUE;
            break;
         case vc_sym:
            strcpy (ref_code, "");
            break;
         case variable_sym:
            sprintf (ref_code, "dra_%s_variable_value", get_put);
            need_ref = TRUE;
            break;
         case attribute_sym:
            sprintf (ref_code, "dra_%s_component_value (dra_get_object ",
                    get_put);
            need_extra_rite_paren = TRUE;
            need_object = TRUE;
            need_ref = TRUE;
            break;
         case bound_elem_sym:
            if (!is_get)
               {COMPILE_ERROR (fprintf (stderr,
      	           "csc_find_reference: illegal store into element named %s\n",
      	           cus_sym_representation (ref)););
               fprintf (stream, INVALID_C_CODE);
               strcpy (ref_code, "");
               }
             else
               strcpy (ref_code, "dra_get_creating_sd");
            break; 
         case bound_comp_sym:
            sprintf (ref_code, "dra_%s_component_value (dra_get_creating_sd",
		     get_put);
            need_extra_rite_paren = TRUE;
            need_ref = TRUE;
            break;
         default:
            COMPILE_ERROR (fprintf (stderr,
   		 "csc_find_reference: got an illegal ref type of %s\n",
   		cus_type_print_val (ref_type)););
            strcpy (ref_code, INVALID_C_CODE);
            break;
         }
      DEBUG3 (fprintf (stderr, "\tref_code = %s\n", ref_code););
      if (strlen (ref_code))
         fprintf (stream, "%s (\n", ref_code);
      if (need_parent)
         fprintf (stream, "dra_get_parent_vc (");
      if (good_ref)
         {if ((cus_sym_type) cus_get_prop (good_ref, type_prop) == vc_sym)
   				/* cannot get a vc "locally", this is a forward
   				   reference to a sub vc */
            {COMPILE_ERROR (fprintf (stderr,
      	  "csc_find_reference: illegal forward reference of sub-vc named %s\n",
      	        cus_sym_representation (good_ref)););
            fprintf (stream, INVALID_C_CODE);
            }
          else
				/* we found ref locally, it is easily
				   accessable via SELF */
            fprintf (stream, "dob_make_id_constant_adi (SELF)");
         }
       else
				/* we did not find the reference "locally"
				   go look up the context tree */
         if (!csc_climb_out (stream, ref, context, target_context, self))
            {COMPILE_ERROR (fprintf (stderr,
      	                     "csc_find_reference: cannot reference item %s\n",
      	                     cus_sym_representation (ref)););
            fprintf (stream, INVALID_C_CODE);
            }
      if (need_parent)
         fprintf (stream, ")");
      if (need_object)
         fprintf (stream, ",\n dob_make_string_constant_adi (\"%s\")\n",
		object_name);
      if (need_extra_rite_paren)
         fprintf (stream, ")");
      if (need_ref)
         fprintf (stream, ",\n dob_make_string_constant_adi (\"%s\")\n",
                     cus_sym_representation (ref));
      if ((strlen (ref_code) > 0) && is_get)
         fprintf (stream, ")\n");
      }

   DEBUG2 (fprintf (stderr, "csc_find_reference exit"););

   return;
}					/* end csc_find_reference */


/* -------------------------------------------     csc_gets_write */
void csc_gets_write (stream, gets_statement, self)
FILE *stream;
LIST gets_statement;
cus_sym self;
{  cus_sym lhs, lhs_type, parent;
   cus_context_type current_context;
   LIST save_props;

   set_status (ok);

   DEBUG2 (fprintf (stderr, "csc_gets_write: enter\n");
      fprintf (stderr, "\tgets_statement = \n");
      cud_print_tree (stderr, gets_statement, "\t");
      fprintf (stderr, "\t self = %s\n", cus_sym_representation (self)););

   if (list_length (gets_statement) != 3)
      FATAL_ERROR (fprintf (stderr, "bad gets_statement\n");
                   cud_print_tree (gets_statement);)

   lhs = (cus_sym) get_list_pos (gets_statement, 2);
   if (!cus_is_sym (lhs))
      {DEBUG3 (fprintf (stderr, "csc_gets_write: got a dot tree\n"););
      csc_dot_write (stream, lhs, self, PUT);
      fprintf (stream, ",\n");
      goto DO_RHS;
      }
   lhs_type = (cus_sym) cus_get_prop (lhs, type_prop);

   current_context = cus_current_context ();

   DEBUG3 (fprintf (stderr, "csc_gets_write: lhs = %s\n",
	    cus_sym_representation (lhs)););

   if (lhs == car_self_symbol)
      {COMPILE_ERROR (fprintf (stderr, "cannot assign to self\n"););
      fprintf (stream, INVALID_C_CODE);
      goto DONE;
      }

   switch (lhs_type)
      {case variable_sym:
         DEBUG3 (fprintf (stderr,
	   "csc_gets_write: lhs is a variable, parent vc is %s\n",
           cus_sym_representation (cus_get_prop (lhs, parent_vc_prop))););
         fprintf (stream, "dra_put_variable_value (\n");
         parent = (cus_sym) cus_get_prop (lhs, parent_vc_prop);

         if (!csc_climb_out (stream, lhs, current_context,
                        cus_get_prop (parent, owned_context_prop), self))
            {COMPILE_ERROR (fprintf (stderr,
      	                     "csc_gets_write: cannot reference item %s\n",
      	                     cus_sym_representation (lhs)););
            fprintf (stream, INVALID_C_CODE);
            }

         fprintf (stream, ", \n dob_make_string_constant_adi (\"%s\"),\n",
                  cus_sym_representation (lhs));
         break;
      case attribute_sym:
/* ??? check to ensure att is "in" the element this obj is defined as, probably
   ???  can use csc_look_in_elem when it is fixed  */
         DEBUG3 (fprintf (stderr,
   	   "csc_gets_write: lhs is an attribute, parent object is %s\n\n",
           cus_sym_representation (cus_get_prop (lhs, parent_object_prop))););
         fprintf (stream, "%s (\n", cus_get_prop (car_gets_symbol,
   		  c_function_name_prop));
         fprintf (stream, "dob_make_adi (SELF, \"%s\"),\n",
   		cus_sym_representation (lhs));
         break;
      case id_sym:
         if (save_props = csc_try_bound_elem (lhs))
            {COMPILE_ERROR (fprintf (stderr,
	       "csc_gets_write: illegal store into element named %s\n",
               cus_sym_representation (lhs)););
             csc_restore_bound_elem (lhs, save_props);
             }
          else
            {COMPILE_ERROR (fprintf (stderr,
	       "csc_gets_write: %s is probably an undefined variable\n",
               cus_sym_representation (lhs)););
            }
         fprintf (stream, INVALID_C_CODE);
         break;
      default:
         FATAL_ERROR (fprintf (stderr,
		         "csc_gets_write: got illegal type of %s for lhs\n",
   		         cus_type_print_val (lhs_type));)
         break;
      }

DO_RHS:
   csc_exp_write (stream, (LIST)get_list_pos (gets_statement, 3), self, FALSE);
   fprintf (stream, ");\n");


DONE:
   cs2_fflush (stream);
   DEBUG2 (fprintf (stderr, "csc_gets_write: exit\n"));

   return;
}					/* end csc_gets_write */



/* -------------------------------------------     csc_constant_write */
void csc_constant_write (stream, the_const, self)
FILE *stream;
cus_sym the_const;
cus_sym self;
{  idd_data_types type;
   double *real_ptr;

   type = (idd_data_types) cus_get_prop (the_const, current_type_prop);
   switch (type)
      {case idd_string:
         DEBUG3 (fprintf (stderr,
		  "csc_constant_write: string = %s\n",
		  cus_get_prop (the_const, current_value_prop)););
         fprintf (stream, "dob_make_string_constant_adi (\"%s\")\n",
		  cus_get_prop (the_const, current_value_prop));
         break;
					/* integer type guys */
      case idd_boolean:
         fprintf (stream, "dob_make_boolean_constant_adi (%d)\n",
		  cus_get_prop (the_const, current_value_prop));
         break;
      case idd_integer:
         fprintf (stream, "dob_make_integer_constant_adi (%d)\n",
		  cus_get_prop (the_const, current_value_prop));
         break;
      case idd_id:
         fprintf (stream, "dob_make_id_constant_adi (%d)\n",
		  cus_get_prop (the_const, current_value_prop));
         break;
      case idd_real:
         real_ptr =  (double *) cus_get_prop (the_const, current_value_prop);
         fprintf (stream, "dob_make_real_constant_adi (%f)\n", *real_ptr);
         break;
      case idd_record:
         FATAL_ERROR (fprintf (stderr, "record constant encountered\n"))
         break;
      case idd_buffer:
         FATAL_ERROR (fprintf (stderr, "buffer constant encountered\n"))
         break;
      default:
         FATAL_ERROR (fprintf (stderr,
		"constant of unknown type = %d encountered\n", type););
         break;
      }
   return;
}					/* end csc_constant_write */


/* -------------------------------------------     csc_reference_write */
void csc_reference_write (stream, ref, self)
FILE *stream;
cus_sym ref;
cus_sym self;
{  cus_sym guy, parent;
   cus_sym_type ref_type;
   cus_context_type current_context;
   LIST guys_to_unredefine, save_props;
   string ref_string;

   ref_string = cus_sym_representation (ref);

   DEBUG2 (fprintf(stderr,"csc_reference_write enter: ref = %s, self = %s\n",
	    ref_string,
	    cus_sym_representation (self)););

   guys_to_unredefine = make_list ();
   current_context = cus_current_context ();

TRY_AGAIN:
   ref_type = (cus_sym_type) cus_get_prop (ref, type_prop);
   if ((ref != self) &&
       (ref != car_self_symbol) &&
       (!cus_get_prop (ref, is_ok_id_name_prop)))

				/* "complex" ref reference, need to generate
				   "navigation" function call(s) */
      {DEBUG3 (fprintf (stderr, "\tcomplex ref\n");
               fprintf (stderr,
		"\tcurrent context = %d\n", current_context);
               fprintf (stderr, "\tref type = %s, context = %d\n",
		      cus_type_print_val (ref_type),
		      cus_sym_context (ref));
               if (ref_type == (cus_sym_type) attribute_sym)
                  parent = (cus_sym) cus_get_prop (ref, parent_object_prop);
                else
                  parent = (cus_sym) cus_get_prop (ref, parent_vc_prop);
               if (parent)
                  fprintf (stderr, "\tref parent = %s\n",
		   	   cus_sym_representation (parent));
               if ((cus_sym) cus_get_prop (self, type_prop) ==
		   (cus_sym) attribute_sym)
                  parent = (cus_sym) cus_get_prop (self, parent_object_prop);
                else
                  parent = (cus_sym) cus_get_prop (self, parent_vc_prop);
               if (parent)
                  fprintf (stderr, "\tself parent = %s\n",
		   	   cus_sym_representation (parent)););

      switch (ref_type)
         {case vc_sym:
            DEBUG3 (fprintf (stderr,
	      "\tcomplex ref is a vc, parent is %s\n",
   	      cus_sym_representation(cus_get_prop (ref, parent_vc_prop)));)
            csc_find_reference (stream, ref, current_context,
			  cus_get_prop (ref, owned_context_prop), self,
			  "ERROR FROM csc_reference_write 0", GET);
            break;
         case object_sym:
            DEBUG3 (fprintf (stderr,
		"\tcomplex ref is an object, parent is %s\n",
		cus_sym_representation(cus_get_prop (ref, parent_vc_prop))););
            parent = (cus_sym) cus_get_prop (ref, parent_vc_prop);
            csc_find_reference (stream, ref, current_context,
			  cus_get_prop (parent, owned_context_prop), self,
			  "ERROR FROM csc_reference_write 1", GET);
            break;
         case variable_sym:
            DEBUG3 (fprintf (stderr,
	    "\tcomplex ref is a variable, parent vc is %s\n",
   	         cus_sym_representation (cus_get_prop (ref, parent_vc_prop)));)
            parent = (cus_sym) cus_get_prop (ref, parent_vc_prop);
            csc_find_reference (stream, ref, current_context,
		                cus_get_prop (parent, owned_context_prop),
				self, "ERROR FROM csc_reference_write 2", GET);
            break;

         case attribute_sym:
/* ??? do we need a call to csc_find_reference here ?
     if so be careful with using the last parameter  */
            DEBUG3 (fprintf (stderr,
			"\tcomplex ref is an attribute, parent object is %s\n",
   	    cus_sym_representation (cus_get_prop (ref, parent_object_prop)));)
            fprintf (stream, "dob_make_adi (SELF, \"%s\")\n",
   	    	     ref_string);
            break;

          case id_sym:
             if (save_props = csc_try_bound_elem (ref))
                {DEBUG3 (fprintf (stderr,
    			"\tfound bound sd element = %s\n", ref_string););
                parent = (cus_sym) cus_get_prop (ref, parent_vc_prop);
                csc_find_reference (stream, ref, current_context,
    			  cus_get_prop (parent, owned_context_prop), self,
    			  "ERROR FROM csc_reference_write 5", GET);
                 csc_restore_bound_elem (ref, save_props);
                 }
              else if (csc_try_redefined (stream, ref, self))
                {add_to_tail (guys_to_unredefine, ref);
                goto TRY_AGAIN;
                }
             break;

         case bound_elem_sym:
            parent = (cus_sym) cus_get_prop (ref, parent_vc_prop);
            DEBUG3 (fprintf (stderr,
		"\tcomplex ref is a bound_elem_sym, parent is %s\n",
		cus_sym_representation(parent)););
            csc_find_reference (stream, ref, current_context,
			  cus_get_prop (parent, owned_context_prop), self,
			  "ERROR FROM csc_reference_write 3", GET);
            break;

#if  0
/* ?? maybe this is never used, and sould be deleted so default takes over */
         case bound_comp_sym:
            parent = (cus_sym) cus_get_prop (ref, parent_vc_prop);
            DEBUG3 (fprintf (stderr,
		"\tcomplex ref is a bound_comp_sym, parent is %s\n",
		cus_sym_representation(parent)););
            csc_find_reference (stream, ref, current_context,
			  cus_get_prop (parent, owned_context_prop), self,
			  "ERROR FROM csc_reference_write 4", GET);
            break;
#endif

         default:
            FATAL_ERROR (fprintf (stderr,
	  "csc_reference_write: got unknown type of %s for complex self/ref\n",
   		cus_type_print_val (ref_type));)
            break;
         }
      }
    else
				/* "simple"  ref reference */
      {if (ref == car_self_symbol)
         ref_type = (cus_sym_type) cus_get_prop (self, type_prop);
       else
         ref_type = (cus_sym_type) cus_get_prop (ref, type_prop);
      DEBUG3 (fprintf (stderr, "\tsimple ref, ref_type = %s\n",
	         cus_type_print_val (ref_type)););
      switch (ref_type)
         {case vc_sym:
            DEBUG3 (fprintf (stderr, "\tref is a vc, parent object is %s\n",
   	      cus_sym_representation(cus_get_prop (ref, parent_object_prop)));)
            fprintf (stream, "dob_make_id_constant_adi (SELF)\n");
            break;
         case object_sym:
            FATAL_ERROR (fprintf(stderr,
			"csc_reference_write got a ref which is an object\n");)
            break;
         case variable_sym:
            DEBUG3 (fprintf (stderr, "\tref is a variable, parent vc is %s\n",
   	         cus_sym_representation (cus_get_prop (ref, parent_vc_prop)));)
            fprintf (stream, "dob_make_id_constant_adi (SELF)\n");
            break;
         case attribute_sym:
            DEBUG3 (fprintf (stderr,
			"\tref is an attribute, parent object is %s\n",
   	   cus_sym_representation (cus_get_prop (ref, parent_object_prop)));)
            if (ref == car_self_symbol)
               fprintf (stream, "dob_make_id_constant_adi (SELF)\n");
             else
               fprintf (stream, "dob_make_adi (SELF, \"%s\")\n",
   		ref_string);
            break;
         case id_sym:
            DEBUG3 (fprintf (stderr, "\tref is id_sym\n"););
            if (!cus_get_prop (ref, is_ok_id_name_prop))
               {if (csc_try_redefined (stream, ref, self))
                  {add_to_tail (guys_to_unredefine, ref);
                  goto TRY_AGAIN;
                  }
               }
             else
               fprintf (stream, "dob_make_adi (%s, \"\")\n",
                        ref_string);
            break;
         default:
            FATAL_ERROR (fprintf (stderr,
		  "csc_reference_write got unknown type of %s for self/ref\n",
   		cus_type_print_val (ref_type));)
            break;
         }
      }

   loop_through_list (guys_to_unredefine, guy, cus_sym)
      cus_put_prop (guy, redefined_prop, NULL);
   free_list (guys_to_unredefine, NOT_NODES);

   DEBUG2 (fprintf (stderr, "csc_reference_write: exit\n"););

   return;
}					/* end csc_reference_write */


