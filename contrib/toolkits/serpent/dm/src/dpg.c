static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/dpg.c,v 1.16 89/08/25 15:44:09 bmc Exp $";

/*
$Log:	dpg.c,v $
 * Revision 1.16  89/08/25  15:44:09  bmc
 * Fixed bug associated with VC deletion.  (SPR #91)
 * Also changed an incorrect diagnostic.
 * 
 * Revision 1.15  89/08/03  21:00:57  bmc
 * No longer prints out 'fnork did not have a method handler' message when
 * there's no method handler.
 * 
 * Revision 1.14  89/07/25  21:50:24  bmc
 * - Changed 'is_reserved_word_prop' to newly-created 'sd_template_prop'.
 * - Dpg_propagate_delete now gets PET id from VC template, not VC instance.
 * 
 * Revision 1.13  89/07/20  16:43:50  bmc
 * Changes for OPS/83 removal.
 * 
 * Revision 1.12  89/05/30  16:24:44  trm
 * changed "#ifdef 0" to more compatible  "if 0".  trm
 * 
 * Revision 1.11  89/05/26  10:59:38  little
 * change thins to work with the new sne structure (dgr_sne)
 * 
 * Revision 1.10  89/05/19  17:29:51  bmc
 * Changed to call 'dob' routines with differently-typed parameters.
 * 
 * Revision 1.9  89/02/09  17:39:18  ejh
 * added copyright notice
 * 
 * Revision 1.8  89/01/30  13:39:51  rcs
 * added support for object_sym
 * 
 * Revision 1.7  89/01/04  11:36:46  rcs
 * no changes
 * 
 * Revision 1.6  88/11/02  13:34:43  little
 * change the error processing for a non-existant method to just print
 * the error and not do a FATAL_ERROR
 * 
 * Revision 1.5  88/10/24  17:12:37  little
 * dpg_propagate_modify has a new (third) parameter.  this is the component
 * name, if there is one otherwise it is "", of the compponent being modified.
 * if the compnonent is "method" the appropriate snippet for the method
 * will be added to the firing line and the component reference list
 * will not be used.
 * 
 * Revision 1.4  88/10/04  17:48:53  little
 * add parent id parameter to dpg_propagate_create
 * 
 * Revision 1.3  88/10/03  09:25:52  little
 * guard some error printout wth debug statements.  they were forgotten on
 * last checkin.
 * 
 * Revision 1.2  88/10/03  09:13:33  little
 * fix names used to set ops components
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

#include <stdio.h>
#include <strings.h>
#include "serpent.h"                    /* serpent externals                */
#include "cus.h"
#include "isc.h"
#include "dpg.h"
#include "dgo.h"
#include "dob.h"
#include "dgr.h"
#include "d22.h"


/*
??? need to put METHOD_NAME in a header file
*/
#define METHOD_NAME "method"


#define rget_ste(object,entry,ret_val)\
   if (!(entry = dob_get_symbol_entry (object)))\
      {set_status (dpg_no_st_entry);\
      return (ret_val);\
      }

#define get_ste(object,entry)\
   if (!(entry = dob_get_symbol_entry (object)))\
      {set_status (dpg_no_st_entry);\
      return;\
      }


#define validate_status(return_status)\
   if (get_status ())\
      {set_status (return_status);\
      return;\
      }

/*
   The following synonyms are used for fields in a dgr_sne structure
   until the names in the structure def. are changed to more meaningful ones.
*/

#define sne_parent_vc	parm_1
#define sne_sd_handle	parm_2

/*
???? maybe do not need dpg_find_match
*/
#if 0
/* --------------------------------------------  dpg_find_match */
int dpg_find_match (candidate, list_element)
char *candidate;
char *list_element;
{  int c_item, l_item;
   boolean equal;

   equal = TRUE;
   c_item = (int) get_list_next (candidate, NULL); 
   l_item = (int) get_list_next (list_element, NULL); 
   while (c_item != NULL && l_item != NULL && equal)
      {equal = (c_item == l_item);
      c_item = (int) get_list_next (candidate, c_item);
      l_item = (int) get_list_next (list_element, l_item);
      }
   return (equal);
}				/* end dpg_find_match */
#endif


/*--------------------------------------------------------------------------*\
|  Routine: copy_sne
|
|  Description:
|     copys a sne (probably automatic data) into a more persistant
|	area, the heap
\*--------------------------------------------------------------------------*/
static dgr_sne *copy_sne (sne)
dgr_sne *sne;
{
   dgr_sne *new_sne;

   if ((new_sne = (dgr_sne *) make_node (sizeof (dgr_sne))) != (dgr_sne *) NULL)
      {new_sne-> routine = sne-> routine ;
      new_sne-> sne_sd_handle = sne-> sne_sd_handle;
      new_sne-> sne_parent_vc = sne-> sne_parent_vc ;
      }

   return (new_sne);
}				/* end copy_sne */


/*--------------------------------------------------------------------------*\
|  Routine: add_to_prop_list (LOCAL)
|
|  Description: 
|     Local routine to add a value to a property list.  This routine takes
|     care of locating the symbol table entry, extracting the list, and 
|     adding the value.
|
|  Arguments:
|     symbol   - The symbol whose property is to be changed.
|     property - The property to be changed.  WARNING: This property MUST
|                be a list.  This routine does NOT check!
|     value    - The value to be added to the list (usually a pointer to the
|                real data).
|  Return Value:
|     false - Something didn't work.  The error was reported.
|     true  - Everything worked fine.
|
\*--------------------------------------------------------------------------*/
static boolean add_to_prop_list (symbol, prop_name, value)
cus_sym		  symbol;
cus_property_type prop_name;
caddr_t		  value;
{
   LIST list;

   list = (LIST) cus_get_prop (symbol, prop_name);
   rcheck_status (
      "add_to_prop_list: can't get property value from list\n",
      false);

   rcheck_null (list,
      "add_to_prop_list: unexpected null property list\n",
      null_memory_ptr,
      false);

   add_to_head (list, value);

   return (true);
}				/* end add_to_prop_list */


/*--------------------------------------------------------------------------*\
|  Routine: get_object_prop (LOCAL)
|
|  Description: 
|     Get a property from the symbol table entry associated with a DOB
|     object.
|
\*--------------------------------------------------------------------------*/
static caddr_t get_object_prop (object, prop_name)
dob_data_object object;
cus_property_type prop_name;
{
   cus_sym ste;

   rget_ste (object, ste, NULL);
   return ((caddr_t) cus_get_prop (ste, prop_name));
}				/* end get_object_prop */


/*--------------------------------------------------------------------------*\
|  Routine: dpg_propagate_reference
|
|  Description:
|     propagates a reference to a data base object
\*--------------------------------------------------------------------------*/
void dpg_propagate_reference (object, component_position)
dob_data_object object;
int component_position;
{  cus_sym_type item_type;
   LIST use_list;
   cus_sym st_entry;
   LIST *use_array;

   DEBUG2 (fprintf (stderr,
	"dpg_propagate_reference called: object = %x, pos = %d\n",
	object, component_position); );
   set_status (ok);
   if (d22_sne == (dgr_sne *) NULL)
      return;
   item_type = dob_get_symbol_type (object); 
   validate_status (dpg_cannot_get_symbol_type);
   switch (item_type) {
      case sd_sym:
      case object_sym:
         get_ste (object, st_entry);
         use_array = (LIST *) cus_get_prop (st_entry, ask_prop);
         validate_status (dpg_cannot_get_ask_prop);
         use_list = use_array [component_position];
         break;
      case variable_sym:
         get_ste (object, st_entry);
         use_list = (LIST) cus_get_prop (st_entry, ask_prop);
         validate_status (dpg_cannot_get_ask_prop);
         break;
      case temp_sym:
         return;
         break;
      default:
         set_status (dpg_unknown_type);
         return;
         break;
      }

   DEBUG3 (fprintf (stderr, "dpg_propagate_reference: d22_sne =\n");
       dgr_print_sne (stderr, d22_sne, "   "); );

      {DEBUG3 (fprintf (stderr,
	"dpg_propagate_reference: old use_list =\n");
         dgr_print_sne_list (stderr, use_list, "   "); );
      add_to_head (use_list, d22_sne);
      DEBUG3 (fprintf (stderr,
		"dpg_propagate_reference: new use_list =\n");
         dgr_print_sne_list (stderr, use_list, "   "); );
      }
   return;
}				/* end dpg_propagate_reference */


/*--------------------------------------------------------------------------*\
|  Routine: dpg_propagate_modify
|
|  Description:
|     propagates modification of a data base object
\*--------------------------------------------------------------------------*/
void dpg_propagate_modify (object, component_position, component_name)
dob_data_object object;
int component_position;
string component_name;
{  cus_sym_type item_type;
   LIST use_list, *use_array;
   int meth_snippet;
   cus_sym st_entry, symbol, template;
   dob_data_object template_object;
   dgr_sne *sne;

   DEBUG2 (fprintf (stderr, "dpg_propagate_modify called: object = %x, pos = %d\n",
	object, component_position); );
   set_status (ok);
   item_type = dob_get_symbol_type (object); 
   validate_status (dpg_cannot_get_symbol_type);
   switch (item_type) {
      case sd_sym:
      case object_sym:
         get_ste (object, st_entry);
				/* if this is a method, do not get dependency
				   list, simply get the snippet number from
				   the object's template and add it directly
				   to the firing line */
         if (!strcmp (component_name, METHOD_NAME))
            {
	    DEBUG3 (fprintf (stderr,
			      "got a method modify, object = %x\n", object););
            if (!(template_object = (dob_data_object) cus_get_prop (st_entry,
							    template_prop)))
               {set_status (dpg_no_template_entry);
               return;
               }
            get_ste (template_object, template);
            meth_snippet = (int) cus_get_prop (template,
					       fixed_method_snippet_prop);
            set_status (ok);
            if (!meth_snippet)
	       /* No method to run.  Quietly return. */
               return;

            DEBUG3 (fprintf (stderr, "snippet = %d\n", meth_snippet););
            sne = (dgr_sne *) (make_node (sizeof (dgr_sne)));
            sne-> routine       = meth_snippet;
            sne-> sne_parent_vc = object;
            sne-> sne_sd_handle = dob_null_object;
            add_to_head (d22_firing_line, sne);
            DEBUG3 (fprintf (stderr,
		"dpg_propagate_modify: new firing line =\n");
                dgr_print_sne_list (stderr, d22_firing_line, "   "); );
            return;
            }
          else
				/* Not a method, get the data item's dependency
				   list.  Dependency lists are kept by
				   component.
				*/
            {
	    use_array = (LIST *) cus_get_prop (st_entry, ask_prop);
            validate_status (dpg_cannot_get_ask_prop);
            use_list = use_array [component_position];
            use_array [component_position] = make_list ();
            }
         break;
      case variable_sym:
         DEBUG3 (fprintf (stderr, "modify, variable_sym\n");)

				/* get the list of thingies that use this
				   variable and add them to the firing line.
				*/
         get_ste (object, st_entry);
         use_list = (LIST) cus_get_prop (st_entry, ask_prop);
         validate_status (dpg_cannot_get_ask_prop);
         cus_put_prop (st_entry, ask_prop, (char *) make_list ());
         break;
      case temp_sym:
         return;
         break;
      default:
         set_status (dpg_unknown_type);
         return;
         break;
      }

   if (!use_list || list_length (use_list) < 1)
      return;
    else
      {DEBUG3 (fprintf (stderr, "dpg_propagate_modify: use_list =\n");
         dgr_print_sne_list (stderr, use_list, "   ");
         fprintf (stderr, "dpg_propagate_modify: old firing line =\n");
         dgr_print_sne_list (stderr, d22_firing_line, "   "); );
      append_list (d22_firing_line, use_list);
      DEBUG3 (fprintf (stderr,
		"dpg_propagate_modify: new firing line =\n");
         dgr_print_sne_list (stderr, d22_firing_line, "   "); );
      }

   return;
}					/* end dpg_propagate_modify */


/*--------------------------------------------------------------------------*\
|  Routine: dpg_propagate_create
|
|  Description:
|     propagates creation of a data base object
\*--------------------------------------------------------------------------*/
void dpg_propagate_create (object, parent)
dob_data_object object;
dob_data_object parent;
{  cus_sym_type item_type;
   cus_sym object_ste, template_ste, sub_vc_ste, sd_ste;
   dob_data_object template_object;
   dob_data_object shared_data, shared_data_template;
   LIST list;
   LIST potential_vcs;
   LIST temp_list;
   dgr_sne *pee, *partial_pee;
   dgr_sne pee_buffer;
   dgr_snippet pet_id;


   DEBUG2 (fprintf (stderr, "entering propagate_create: object = %x\n", object);)
   set_status (ok);

   item_type = dob_get_symbol_type (object); 
   validate_status (dpg_cannot_get_symbol_type);
   switch (item_type) {
      case sd_sym:
      case object_sym:
         get_ste (object, object_ste);
	 template_object = (dob_data_object)cus_get_prop
				(object_ste, sd_template_prop);
         set_status (ok);
	 if (template_object == dob_null_object)
            {
	    set_status (dpg_no_template_entry);
            return;
            }
         get_ste (template_object, template_ste);
	 			/*
		   		   The following must be accomplished:

		   		   1. Add the handle of the SD instance to the
				      instance in the S.D. template.  (Already
				      done in DOB.)
				   2. Zero the list of bound VCs (i.e,
				      created_vcs_prop).  (Already done in DOB.)
				   3. Add the Pet Environment (PEE) for each
				      potential VC interested in an S.D. of
				      this type, to the firing line.
	 			*/
	 potential_vcs = (LIST) cus_get_prop (template_ste, potential_vcs_prop);
	 check_status (
	 "dpg_propagate_create: can't get potential_vcs_prop from sym. tab.\n"
	 );
	 if (list_length (potential_vcs) > 0)
	    {
	    loop_through_list (potential_vcs, partial_pee, dgr_sne *)
	       {
	       			/*
		    		   Allocate a new PEE, copy the partial
		    		   PEE into it, and flesh it out with the
		    		   handle of the shared data element.  The
		    		   PEE is assumed to be freed in the
		    		   associated, compiler-generated PET.
	       			*/
	       pee = copy_sne (partial_pee);
	       check_null ((char *) pee,
		  "dpg_propagate_create: failed to allocate PEE\n",
		  out_of_memory);
	       pee -> sne_sd_handle = (caddr_t) object;
	       add_to_head (d22_firing_line, (NODE) pee);
	       } /* end loop */
	    } /* end if */
         break;

      case variable_sym:
	 break;

      case vc_sym:
         get_ste (object, object_ste);
	 template_object = (dob_data_object)cus_get_prop
					(object_ste, template_prop);
         set_status (ok);
	 if (template_object == dob_null_object)
            {
	    set_status (dpg_no_template_entry);
            return;
            }
         get_ste (template_object, template_ste);
				 /*
				   The sub_vcs_prop, for a template, specifies
				   the symbols of the sub-VC templates 
				   attached to this VC template.  
				 */
	 list = (LIST) cus_get_prop (template_ste, sub_vcs_prop);
	 check_status (
	    "dpg_propagate_create: can't get sub_vcs_prop from sym. tab.\n"
	 );
	 			/*
				  Loop through the list of potential sub-VCs,
				  registering them for possible creation.
	 			*/
	 loop_through_list (list, sub_vc_ste, cus_sym)
	    {
				/*
			   	   Build partial PET environment (PEE) from
			   	   sub-VC's PET id and parent (i.e., the
			   	   just-created) VC's handle.
				*/
	    pet_id = (dgr_snippet) cus_get_prop
				(sub_vc_ste, value_snippet_prop);
	    check_status (
	    "dpg_propagate_create: can't get value_snippet_prop from sym tab.\n"
	    );

				/* pee_buffer contains the partial PEE */
	    pee_buffer.routine       = pet_id;
	    pee_buffer.sne_parent_vc = object; /* parent vc of this sub-vc */
	    pee_buffer.sne_sd_handle = (caddr_t) dob_null_object;
				/*
				   Does this VC depend on the creation of a
				   shared data element?  Creating_sd_prop, for
				   a template, refers to the shared data
				   template that a VC of this 'type' is 
				   interested in.  (Got that?)  Since it's
				   a template, it isn't a dob_handle; rather,
				   it's a symbol table pointer.  (Trust me.)
				*/
	    sd_ste = (cus_sym) cus_get_prop (sub_vc_ste, creating_sd_prop);
            set_status (ok);

	    if ( sd_ste != (cus_sym) NULL ) /* sub-VC depends on shared data */
	       {
	       			/*
				   Sub-VC depends on creation of shared data.
				   First, add the partial PEE to the potential
				   VC list for the shared data template.  Be
				   sure to use a copy, not the real PEE.
				*/
	       pee = copy_sne (& pee_buffer);
	       check_null ((char *) pee,
		  "dpg_propagate_create: failed to allocate PEE\n",
		  out_of_memory);

	       (void) add_to_prop_list (sd_ste, potential_vcs_prop, pee);
				/*
				   Next, for every instance of this type of
				   shared data out there, we have to create
				   a PET for this sub-VC and place it on the
				   firing line.  We already have the partial
				   PEE; we just need the shared data instance
				   handles.
				*/
	       temp_list = (LIST) cus_get_prop (sd_ste, instances_prop);
               set_status (ok);
	       loop_through_list (temp_list, shared_data, dob_data_object)
		  {
		  pee = copy_sne (& pee_buffer);
		  check_null ((char *) pee,
		     "dpg_propagate_create: failed to allocate PEE\n",
		     out_of_memory);
		  pee -> sne_sd_handle = shared_data;
		  add_to_head (d22_firing_line, (NODE) pee);
		  } /* end loop */
	       } /* end if sub-vc depends on creation of shared data */

	    else /* doesn't depend on creation of a shared data element */
	       {
				/*
				   Simply add the partial PEE to the firing
				   line.  In the case of variables and
				   shared data components the partial PEE
				   contains all the information we need,
				   since the only missing parameter is a
				   shared data handle that doesn't apply.
				*/
	       pee = copy_sne (& pee_buffer);
	       add_to_head (d22_firing_line, (NODE) pee);
	       } /* end else doesn't depend on creation of shared data */
	    } /* end loop through all sub VCs */
         break;

      case temp_sym:
         return;
         break;
      default:
         set_status (dpg_unknown_type);
         return;
         break;
      }

   return;
}					/* end dpg_propagate_create */


/*--------------------------------------------------------------------------*\
|  Routine: dpg_propagate_delete
|
|  Description:
|     propagates deletion of a data base object
\*--------------------------------------------------------------------------*/
void dpg_propagate_delete (object)
dob_data_object object;
{  
   cus_sym_type item_type;
   cus_sym vc_ste;
   dob_data_object vc;
   dgr_sne *pee;
   dgr_snippet pet_id;
   LIST list;


   DEBUG2 (
      fprintf (stderr, "dpg_propagate_delete called: object = %x\n", object);
   );
   set_status (ok);
   item_type = dob_get_symbol_type (object); 
   validate_status (dpg_cannot_get_symbol_type);

   switch (item_type)
      {
      case sd_sym:
      case object_sym:
	 {
	 /* LOCAL DATA */
	 dob_data_object parent_vc;
	 dob_data_object vc_template;
	 cus_sym vc_template_ste;
	 dgr_sne pee_buffer;

				/*
				   We have to locate the list of VCs bound
				   to this piece of data and force their PETs
				   to be re-evaluated to see if they, too,
				   must be deleted.  The list is stored in
				   the created_vcs_prop of in the symbol table
				   entry for the shared data.
				*/
	 list = (LIST) get_object_prop (object, created_vcs_prop);
	 if (list_length (list) > 0) /* VCs are bound to this piece of data */
	    {
	    loop_through_list (list, vc, dob_data_object)
	       {
				/*
				   The PEE is built from the data handle,
				   the parent VC and the PET id.  The PET id
				   comes from the VC template, not the VC
				   instance.
				*/
	       get_ste (vc, vc_ste);

				/*
				   Zero out the creating SD property for
				   this bound VC.
				*/
	       cus_put_prop (vc_ste, creating_sd_prop, (caddr_t) NULL);

	       vc_template = (dob_data_object)cus_get_prop
				      (vc_ste, template_prop);
	       set_status (ok);
	       if (vc_template == dob_null_object)
		  {
		  set_status (dpg_no_template_entry);
		  return;
		  }
	       get_ste (vc_template, vc_template_ste);
	       pet_id = (dgr_snippet) cus_get_prop (vc_template_ste,
						     value_snippet_prop);
	       check_status (
	  "dpg_propagate_delete: can't get value_snippet_prop from sym tab.\n"
	       );

	       parent_vc = (dob_data_object) cus_get_prop (vc_ste,
							    parent_vc_prop);
	       check_status (
	       "dpg_propagate_delete: can't get parent vc from symbol table\n"
	       );

	       pee_buffer.routine       = pet_id;
	       pee_buffer.sne_parent_vc = (caddr_t) parent_vc;
	       pee_buffer.sne_sd_handle = (caddr_t) object;
				/*
				   Place a copy of the PEE on the firing line.
				*/
	       pee = copy_sne (& pee_buffer);
	       check_null ((char *) pee,
	          "dpg_propagate_delete: failed to allocate PEE\n",
	          out_of_memory);

	       add_to_head (d22_firing_line, (NODE) pee);
	       } /* end loop */
	    } /* end if there were bound VCs */
	 } /* end case */
	 break;

      case vc_sym:
	 {
	 /* LOCAL DATA */
	 cus_sym object_ste;
	 cus_sym sd_ste;
	 dob_data_object creating_sd;
	 dob_data_object sd_template;
	 LIST copy_of_list;
	 LIST partial_pee_list;
				/*
				   Traverse the list of sub-VCs, removing
				   their partial PEEs from any shared data
				   templates they may be on.
				*/

         get_ste (object, object_ste);
	 list = (LIST) cus_get_prop (object_ste, sub_vcs_prop);
         set_status (ok);
	 if (list_length (list) > 0)
	    loop_through_list (list, vc, dob_data_object)
	       {
	       get_ste (vc, vc_ste);
	       pet_id = (dgr_snippet) cus_get_prop
				   (vc_ste, value_snippet_prop);
	       check_status (
	    "dpg_propagate_delete: can't get value_snippet_prop from sym tab.\n"
	       );
				/*
				   Does sub-VC depend on a shared data element?
				*/
	       creating_sd = (dob_data_object) cus_get_prop
					(vc_ste, creating_sd_prop);
               set_status (ok);
	       if ( (creating_sd != (dob_data_object) NULL) &&
		    (creating_sd != dob_null_object) )
		  {
				/*
				   Sub-VC depends on a shared data element.
				   Remove its partial PEE from the template
				   for that shared data element.
				*/
		  sd_template = (dob_data_object) get_object_prop
				(creating_sd, sd_template_prop);
		  if ( (sd_template != dob_null_object) &&
		       (sd_template != (dob_data_object) NULL) )
		     {
		     partial_pee_list = (LIST) get_object_prop
					(sd_template, potential_vcs_prop);
		     check_status (
			"dpg_propagate_delete: can't get potential_vcs_prop\n"
			);
				/*
				   Run down the list of partial pees, deleting
				   any that match this PET and parent VC.
				   We can't loop through a list and modify its
				   contents at the same time, so we'll loop 
				   through a temporary copy.

??? POSSIBLE TUNING: replace copy with list element look-ahead.
				*/
		     copy_of_list = copy_list (partial_pee_list, NOT_NODES);
		     check_null (
			copy_of_list,
			"dpg_propagate_delete: can't copy list\n",
			out_of_memory
		     );
		     loop_through_list (copy_of_list, pee, dgr_sne *)
			{
			if ( (pee->sne_parent_vc == object) &&
			     (pee->routine == pet_id) )
			   delete_list_node (partial_pee_list, pee);
			} /* end loop */
		     free_list (copy_of_list, NOT_NODES);

		     } /* end if we have a template */
		  } /* end if sub-VC depends on shared data element */
	       } /* end loop */
	 } /* end case */
	 break;

      case variable_sym:
      case temp_sym:
				/* Nothing to do */
	 break;

      default:
         set_status (dpg_unknown_type);
         return;
	 break;

      } /* end switch */
   return;
}					/* end dpg_propagate_delete */

