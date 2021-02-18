static char rcsid [] =
        "$Header: /u/04c/mnt/integ/dm/src/RCS/dra.c,v 1.37 89/11/02 16:16:33 little Exp $";
/*--------------------------------------------------------------------------*\
|
|  Name:  Run Time Data Access Utility (dra)
|
|  Description:  
|     The run time data access utility provides run time access to various
|     data components within the object base.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     dra.c - contains the implementation for the run time data access 
|             utility.
|     dra.h - contains the external definition for the run time data
|             access utility.
|
|  Project:
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     July 16, 1988 : Module created.
$Log:	dra.c,v $
 * Revision 1.37  89/11/02  16:16:33  little
 * no changes
 * 
 * Revision 1.36  89/10/05  16:57:56  rcs
 * various mods to support typed buffers
 * 
 * Revision 1.35  89/09/01  14:06:56  bmc
 * Changed dra_id_exists to handle null pointers and variables containing
 * null values.
 * 
 * Revision 1.34  89/08/31  19:00:08  bmc
 * Added dra_id_exists() function.
 * 
 * Revision 1.33  89/08/29  17:40:50  little
 * add dra_exit function
 * 
 * Revision 1.32  89/07/24  14:59:36  little
 * ops removal
 * 
 * Revision 1.31  89/05/22  11:11:15  bmc
 * Now explicitly include 'uer.h'.  Related to changes to 'dob.c', 'dob.h'
 * and 'uer.h'.
 * 
 * Revision 1.30  89/05/19  17:26:34  bmc
 * Changed to accomodate new definition of 'dro_atomic_data_item'.  (See
 * 'dro.h' for more information.)  Also, changed calls to dob routines
 * to be consistent with modifications in 'dob.h'.
 * 
 * Revision 1.29  89/03/28  11:26:33  serpent
 * cahnge ifdef 0 to if 0 so the stupid sun compiler can compile it
 * 
 * Revision 1.28  89/03/01  17:06:58  rcs
 * fixed dra_put_component_value to work with different typed data items
 * 
 * Revision 1.27  89/02/23  14:29:07  rcs
 * changed dra_get_name to not accept sd id's as valid parameters
 * 
 * Revision 1.26  89/02/21  23:19:07  rcs
 * fixed invocation of uer routines
 * 
 * Revision 1.25  89/02/09  17:41:29  ejh
 * added copyright notice
 * 
 * Revision 1.24  89/02/07  16:19:02  rcs
 * fixed error with dubug print
 * 
 * Revision 1.23  89/02/06  17:43:31  rcs
 * put undo before id validation
 * 
 * Revision 1.22  89/02/01  14:57:50  rcs
 * modified dra_new to return a boolean adi of TRUE
 * 
 * Revision 1.21  89/01/30  13:45:59  rcs
 * added run time error checking
 * added support for object_sym
 * added dra_get_type routine
 * added dra_get_name routine
 * 
 * Revision 1.20  89/01/04  15:38:33  little
 * propagate undefined parameters in all "user callable" routines
 * 
 * Revision 1.19  88/12/22  11:15:25  rcs
 * nothing in particular
 * 
 * Revision 1.18  88/12/09  17:50:47  little
 * no changes.
 * 
 * Revision 1.17  88/11/11  16:25:03  little
 * change dra_new to really check if something is new.
 * 
 * Revision 1.16  88/11/04  17:02:46  little
 * change DEBUG1 to print d22_slang_source
 * 
 * Revision 1.15  88/10/27  19:10:31  little
 * change dra_delete_temp to use dob_temp_release
 * 
 * Revision 1.14  88/10/27  17:53:10  little
 * add some additional error printout to dra_validate_thingie
 * 
 * Revision 1.13  88/10/26  17:21:40  little
 * add some better debug output to dra_find_thingie
 * 
 * Revision 1.12  88/10/07  15:41:37  little
 * not modified.
 * 
 * Revision 1.11  88/10/03  09:16:42  little
 * modify create_element to take 2 parameters and add delete_element
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

#define dra_main
#include "dra.h"                          /* own include file		    */
#include "dgo.h"
#include "dob.h"
#include "dob_make.h"
#include "dro.h"
#include "d22_debug.h"
#include "cus.h"
#include "iid.h"
#include "ipc.h"
#include "uer.h"			/* error reporting facility	    */
#include <signal.h>


#define MISC_BUF_LEN 	(256)		/* for error msgs, commands, etc. */

/*  ??? this should be in isc.h	*/

#define FATAL_ERROR(statement)\
   {  long pid;\
      char command [MISC_BUF_LEN];\
   fprintf(stderr, "!!!!!     DM FATAL ERROR, at or near SLANG line %d\n",\
        d22_slang_line_num);\
   fprintf (stderr, "\t  in %s\n", d22_slang_source);\
   fprintf(stderr, "   file %s, line %d\n", __FILE__, __LINE__);\
   statement;\
   fflush(stderr);\
   pid = getpid();\
   sprintf(command, "gcore %d", pid);\
   system(command);\
   sprintf(command, "mv core.%d core", pid);\
   system(command);\
   exit(1);\
   }


/*
??? LOOP_THROUGH_LIST definition should be in an h file
*/
#define LOOP_THROUGH_LIST( list, reg ) \
    for( reg = LISThead( list ); reg != NULLlist; reg = reg -> next )

#define DEBUG1(statement) DM_RT_DEBUG1(dra_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(dra_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(dra_debug, statement)

#define UER_GET_PARENT_VC "get_parent_vc"
#define UER_GET_CREATING_SD "get_bound_sd_instance"
#define UER_GET_NAME "get_name"
#define UER_GET_TYPE "get_declaration_type"
#define UER_GET_VC_ID "get_vc_id"
#define UER_GET_VARIABLE_VALUE "get_variable_value"
#define UER_PUT_VARIABLE_VALUE "put_variable_value"
#define UER_GET_OBJECT "get_object"
#define UER_GET_COMPONENT_VALUE "get_component_value"
#define UER_PUT_COMPONENT_VALUE "put_component_value"
#define UER_CREATE_ELEMENT "create_sd_instance"
#define UER_DESTROY_ELEMENT "destroy_sd_instance"
#define DRA_NEW "new"

#define dra_validate dra_validate_thingie

/*--------------------------------------------------------------------------*\
|  Routine: dra_validate_thingie
| 
|  Description:  
|     ensures that thingie is of the required type, follows temp "links" to
|	find the "real" thingie
\*--------------------------------------------------------------------------*/
dob_data_object dra_validate_thingie (thingie, required_type)
dob_data_object thingie;
cus_sym_type required_type;
{ cus_sym_type symbol_type;

   DEBUG2 (fprintf (stderr, "entering dra_validate_thingie\n");
	fprintf (stderr, "thingie = %d, required_type = %s\n",
		thingie, cus_type_print_val (required_type));)

   symbol_type = dob_get_symbol_type (thingie);
   rcheck_status(
	"dra_validate_thingie: bad status from first dob_get_symbol_type", 
   	dob_null_object);

   while (symbol_type == temp_sym) {	/* symbol type is a temp	    */

      thingie = (dob_data_object) dob_ask (thingie, NULL);

      symbol_type = dob_get_symbol_type(thingie);
      if (get_status ()) {
        DEBUG1(
          fprintf(stderr, "dra_validate_thingie:bad status from while loop"); 
          fprintf(stderr, " for dob_get_symbol_type, required_type = %s",
	          cus_type_print_val (required_type));
          fprintf(stderr, " thingie = %d\n", thingie);
        );
   	return (dob_null_object);
      }

   }				/* end while symbol type is a temp	    */

   if (symbol_type != required_type) {
      thingie = dob_null_object;
   }

   DEBUG2 (fprintf (stderr, "leaving dra_validate_thingie:\n");
      fprintf (stderr, "   thingie = %d\n", thingie);)
   return (thingie);
}

/*--------------------------------------------------------------------------*\
|  Routine: dra_find_thingie
| 
|  Description:  
|     finds a particular thingie (vc,var,object) from a thingie instances list
\*--------------------------------------------------------------------------*/
dob_data_object dra_find_thingie (parent, thingie_type, thingie_name)
dob_data_object parent;
cus_sym_type thingie_type;
string thingie_name;

{  dob_data_object found_thingie;
   LIST instances_list;
   register LIST instance;
   cus_sym template, instance_sym, parent_sym;
   string original_name;

   DEBUG2 (fprintf (stderr, "entering dra_find_thingie:\n");
      fprintf (stderr,"\tparent = %d, thingie_type = %s, thingie_name = %s\n",
	parent, cus_type_print_val (thingie_type), thingie_name);
   );

   found_thingie = dob_null_object;

   parent_sym = dob_get_symbol_entry(parent);
   rcheck_status(
     "dra_find_thingie: dob_get_symbol_entry of parent_sym", 
     dob_null_object
   );

   switch (thingie_type)
      {case variable_sym:
         instances_list = (LIST) cus_get_prop (parent_sym, variables_prop);
         rcheck_status ("dra_find_thingie cannot get variables_prop",
			dob_null_object);
         break;
      case object_sym:
         instances_list = (LIST) cus_get_prop (parent_sym, objects_prop);
         rcheck_status ("dra_find_thingie cannot get objects_prop",
			dob_null_object);
         break;
      case sd_sym:
         instances_list = (LIST) cus_get_prop (parent_sym, created_vcs_prop);
         rcheck_status ("dra_find_thingie cannot get created_vcs_prop",
			dob_null_object);
         break;
      case vc_sym:
         instances_list = (LIST) cus_get_prop (parent_sym, sub_vcs_prop);
         rcheck_status ("dra_find_thingie cannot get created_vcs_prop",
			dob_null_object);
         break;
      default:
         FATAL_ERROR (fprintf (stderr,
		"dra_find_thingie got an unknown type of %s\n",
		cus_type_print_val (thingie_type));)
         break;
      }			/* end switch (thingie_type) */

   if (!instances_list || (list_length (instances_list) == 0))
      goto FINISHED;

   LOOP_THROUGH_LIST (instances_list, instance)
       {DEBUG2 (fprintf (stderr, "dra_find_thingie: instance = %d\n",
			 (dob_data_object) (LISTnode (instance)));)
       instance_sym = dob_get_symbol_entry
		((dob_data_object) LISTnode (instance));
       rcheck_status
		("dra_find_thingie could not get sym tab entry for instance",
 		dob_null_object);
       template = (cus_sym) cus_get_prop (instance_sym, template_prop);
       rcheck_status(
		"dra_find_thingie could not get instance's template_prop",
 		dob_null_object);
       original_name = cus_get_prop (dob_get_symbol_entry
				             ((dob_data_object)template),
				     original_name_prop);
       rcheck_status(
		"dra_find_thingie could not get template's original_name",
 		dob_null_object);
      if (!strcmp (original_name, thingie_name))
         {if (found_thingie == dob_null_object)
            found_thingie = (dob_data_object) (LISTnode (instance));
          else
            FATAL_ERROR (fprintf (stderr, "found_thingie found two thingies:");
              fprintf (stderr, "%d & %d\n", LISTnode (instance),
		       found_thingie);)
         }

      }


FINISHED:

   DEBUG2 (fprintf (stderr, "leaving dra_find_thingie:\n");
      fprintf (stderr, "   found_thingie = %d\n", found_thingie);
   );
   return (found_thingie);
}


/*--------------------------------------------------------------------------*\
|  Routine: dra_delete_temp
| 
|  Description:  
|     deletes the parameter adi if it is a temp
\*--------------------------------------------------------------------------*/
void dra_delete_temp (candidate)
dro_atomic_data_item candidate;
{ cus_sym_type symbol_type;
				/* Get the symbol type of the candidate */

   symbol_type = dob_get_symbol_type (candidate.dob_handle);
   check_status ("dra_delete_temp: bad status from dob_get_symbol_type");

				/* Delete the candidate symbol if it is a
				   temp  */
   if (symbol_type == temp_sym)
      {dob_temp_release (candidate.dob_handle);
      check_status("dra_delete_temp: bad status from dob_temp_release");
      }
   return;
}					/* end dra_delete_temp          */


/*--------------------------------------------------------------------------*\
|  Routine: dra_exit
| 
|  Description:  
|     exits a dialogue and notifies parent process
\*--------------------------------------------------------------------------*/
void dra_exit ()
{
   DEBUG2 (fprintf (stderr, "entering dra_exit\n"););

				/* notify my parent */
   kill (getppid (), SIGINT);
				/* kill me */
   kill (getpid (), SIGINT);

   DEBUG2 (fprintf (stderr, "exiting dra_exit\n"););
   return;
}

/*--------------------------------------------------------------------------*\
|  Routine: dra_id_exists
| 
|  Description:  
|     Determines whether the an ID points to something that actually exists.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_id_exists (id_parm)
dro_atomic_data_item id_parm;
{
  dro_atomic_data_item temp;
  dob_data_object data_object;
  cus_sym_type symbol_type;
  boolean undefined;
  boolean exists;


  /*
    id_parm represents a container.  What it contains (i.e., points to)
    is what we're ultimately checking for existence.
  */
  set_status(ok);
  DEBUG2
    (
    (void) fprintf (stderr, "dra_id_exists: call.\n");
    (void) fprintf (stderr, "dra_id_exists: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    (void) fprintf (stderr, 
      "dra_id_exists: id_parm.component_name = %s.\n",
      id_parm.component_name);
    );

  exists = false;
					/*
					  Make sure what id_parm contains
					  is not undefined.  (dro_undefined
					  dereferences the id parameter).
					*/
  undefined = dro_undefined (id_parm);
  if (get_status () != ok)
    {
    					/*
					  dro_undefined barfed.  Kill the
					  dialogue.
					*/
    (void) sprintf (uer_error_msg,
      "dra_id_exists: can't tell if id is undefined or not.  Status = %s\n",
      isc_get_status_string ());
    uer_report_da_id_error (uer_error_msg, id_parm);
    }

  else if (! undefined)
    {
    data_object = id_parm.dob_handle;
    symbol_type = dob_get_symbol_type (data_object);
    if ( get_status () != ok )
      {
					/*
					  The container is invalid.  Report
					  error and kill the dialogue.
					*/
      (void) sprintf (uer_error_msg,
	"dra_id_exists: can't get symbol type for id parameter.  Status = %s\n",
	isc_get_status_string ());
      uer_report_da_id_error (uer_error_msg, id_parm);
      }

					/*
					  Keep dereferencing the container
					  through any intermediate temps
					  until we reach the real thingie
					  the container points to.
					*/
    while ( symbol_type == temp_sym )
      {
      data_object = (dob_data_object) dob_ask (data_object, NULL);
      if ( get_status () != ok )
	{
					/*
					  Couldn't dereference a container
					  that's supposed to have been okay.
					  Major error.  Kill the dialogue.
					*/
	(void) sprintf (uer_error_msg,
	  "dra_id_exists: can't dereference id.  Status = %s\n",
	  isc_get_status_string ());
	uer_report_da_id_error (uer_error_msg, id_parm);
	}

      else if (! dob_validate_object (data_object) )
	{
	symbol_type = null_sym;
	}

      else
	{
	symbol_type = dob_get_symbol_type (data_object);
	if ( get_status () != ok )
	  {
					/*
					  This should never happen.  Kill
					  the dialogue.
					*/
	  (void) sprintf (uer_error_msg,
	    "dra_id_exists: can't get symbol type for temp.  Status = %s\n",
	    isc_get_status_string ());
	  uer_report_da_id_error (uer_error_msg, id_parm);
	  }
	}
      } /* end while */

    exists = (boolean) (symbol_type != null_sym);
    } /* end else */

  temp = dob_make_boolean_constant_adi (exists);
  return temp;
}					/* end dra_id_exists	    */



/*--------------------------------------------------------------------------*\
|  Routine: dra_get_sub_vc
| 
|  Description:  
|     Gets the id of the named sub-vc for the specified vc
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_sub_vc (parent_vc, vc_template_name)
dro_atomic_data_item parent_vc;
dro_atomic_data_item vc_template_name;

{  dro_atomic_data_item temp;
   dob_data_object real_id, vc_id;
   string vc_template_name_string;
   idd_data_types temp_type;

   set_status(ok);			/* begin			    */

   DEBUG2(fprintf(stderr, "dra_get_sub_vc: call.\n");
     fprintf(stderr, "dra_get_sub_vc: parent_vc.dob_handle = %#x.\n",
	parent_vc.dob_handle);
     fprintf(stderr, "dra_get_sub_vc: parent_vc.component_name = %s.\n",
             parent_vc.component_name);
     fprintf (stderr, "dra_get_sub_vc: vc_template_name.dob_handle = %#x.\n",
	      vc_template_name.dob_handle);
     fprintf(stderr, "dra_get_sub_vc: vc_template_name.component_name = %s.\n",
             vc_template_name.component_name););

				/* Create a temp to return the result in,
				   and set its initial value to undefined  */
   temp.component_name = NULL;
   temp.dob_handle = dob_temp_create();
   rcheck_status("dra_get_sub_vc: bad status from dob_temp_create", temp);
   dob_variable_tell (temp.dob_handle, idd_undefined, NULL);
   rcheck_status (
 	"dra_get_sub_vc: bad status from dob_variable_tell of undefined",
	temp);

				/* initialize "returned" values */
   temp_type = idd_undefined;
   vc_id = dob_null_object;

				/* check arguments for undefinedness */
   if (dro_undefined (parent_vc) || dro_undefined (vc_template_name))
      goto FINISHED;

				/* Check for valid id type of vc */
   if ((real_id = dra_validate_thingie (parent_vc.dob_handle, vc_sym)) ==
	dob_null_object)
      {set_status(invalid_symbol);
      DEBUG1(
        isc_print_status("dra_get_sub_vc");
      );
      goto FINISHED;
      }

   vc_template_name_string = (string)dro_ask2 (vc_template_name);
   vc_id = dra_find_thingie (real_id, vc_sym, vc_template_name_string);
   if (vc_id != dob_null_object)
      temp_type = idd_id;

FINISHED:
		/* Update the temp with the appropriate value and type */
   dob_variable_tell (temp.dob_handle, temp_type, (char *) vc_id);
   rcheck_status( "dra_get_sub_vc: bad status from dob_variable_tell",
     temp);
 
				/* delete parameters, if temp */
   dra_delete_temp (parent_vc);
   rcheck_status (
      "dra_get_sub_vc: bad status from dra_delete_temp for parent_vc", temp);
   dra_delete_temp (vc_template_name);
   rcheck_status (
      "dra_get_sub_vc: bad status from dra_delete_temp for vc_template_name",
      temp);

   DEBUG2(fprintf(stderr, "dra_get_sub_vc: temp.dob_handle = %#x.\n",
	temp.dob_handle);
      fprintf(stderr, "dra_get_sub_vc: exiting.\n"););

   return temp;
}					/* end dra_get_sub_vc	    */

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_parent_vc
| 
|  Description:  
|     Gets the id of the parent vc for the specified vc or object. 
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_parent_vc(id_parm)
dro_atomic_data_item id_parm;
{					/* begin			    */
  dro_atomic_data_item temp;
  cus_sym symbol_table_entry;
  dob_data_object real_id, parent_id;
  boolean id_parm_undef;
/*
   Initialize.
*/
  set_status(ok);

  DEBUG2(
    fprintf(stderr, "dra_get_parent_vc: call.\n");
    fprintf(stderr, "dra_get_parent_vc: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(
      stderr, 
      "dra_get_parent_vc: id_parm.component_name = %s.\n",
      id_parm.component_name
    );
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;

  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id(
    "dro_get_parent_vc: bad status from dob_temp_create", 
    UER_GET_PARENT_VC,
    temp
  );
/* 
   Check for undefined argument.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id(
    "dro_get_parent_vc: bad status from dro_undefined", 
    UER_GET_PARENT_VC,
    temp
  );
/*
   If argument is undefined propogate undefined.
*/
  if (id_parm_undef) {			/* id undefined			    */
/* 
   Propogate undefined.
*/
    dob_variable_tell(temp.dob_handle, idd_undefined, 0);
    uer_rcheck_status_da_id(
      "dra_get_parent_vc: bad status from dob_variable_tell",
      UER_GET_PARENT_VC,
      temp
    );
  }					/* end if undefined		    */
  else {				/* id defined			    */
/* 
   Generate an error for any symbol that is not a vc, object or variable.
*/
    if (((real_id = dra_validate(id_parm.dob_handle, vc_sym)) == dob_null_object) &&
      ((real_id = dra_validate(id_parm.dob_handle, object_sym)) == dob_null_object) &&
      ((real_id = dra_validate(id_parm.dob_handle, variable_sym)) == dob_null_object)) {

      DEBUG1(
        fprintf(stderr, "dra_get_parent_vc: invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_PARENT_VC);
      uer_report_da_id_error(uer_error_msg, id_parm);

    }
/* 
   Get handle for symbol table entry.  
*/
    symbol_table_entry = dob_get_symbol_entry(real_id);
    uer_rcheck_status_da_id(
      "dra_get_parent_vc: bad status from dob_get_symbol_entry", 
      UER_GET_PARENT_VC,
      temp
    );
/* 
   Get the parent vc property for the object and update temp.  
*/
    parent_id = (dob_data_object)cus_get_prop(symbol_table_entry, parent_vc_prop);
    uer_rcheck_status_da_id(
      "dra_get_parent_vc: bad status from cus_get_prop", 
      UER_GET_PARENT_VC,
      temp
    );
/* 
   Update the temp with the appropriate value and type 
*/
    dob_variable_tell(temp.dob_handle, idd_id, parent_id);
    uer_rcheck_status_da_id(
      "dra_get_parent_vc: bad status from dob_variable_tell",
      UER_GET_PARENT_VC,
      temp
    );

  }					/* end else id defined		    */
/* 
   Delete id, if temp 
*/
  dra_delete_temp(id_parm);
  uer_rcheck_status_da_id(
    "dra_get_parent_vc: bad status from dra_delete_temp",
    UER_GET_PARENT_VC,
    temp
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_get_parent_vc: temp.dob_handle = %#x.\n",
	temp.dob_handle);
    fprintf(stderr, "dra_get_parent_vc: exiting.\n");
  );

  return temp;
}					/* end dra_get_parent_vc	    */

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_creating_sd
| 
|  Description:  
|     Gets the id of the shared data element which causes the specified
|     vc to be created.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_creating_sd(id_parm)
dro_atomic_data_item id_parm;
{  
  dro_atomic_data_item temp;
  cus_sym symbol_table_entry;
  dob_data_object real_vc_id, sd_id;
  boolean id_parm_undef;
/*
   Initialize.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(fprintf(stderr, "dra_get_creating_sd: call.\n");
    fprintf(stderr, "dra_get_creating_sd: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(stderr, "dra_get_creating_sd: id.component_name = %s.\n", 
      id_parm.component_name);
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;
  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id(
    "dro_get_creating_sd: bad status from dob_temp_create", 
    UER_GET_CREATING_SD,
    temp
  );
/*
   Check for undefined argument.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id(
    "dro_get_creating_sd: bad status from dro_undefined", 
    UER_GET_CREATING_SD,
    temp
  );
/*
   If parameter is undefined then result is undefined.
*/  
  if (id_parm_undef) {             	 	/* id_parm undefined        */
/*
   Propogate undefined.
*/
    dob_variable_tell(temp.dob_handle, idd_undefined, 0);
    uer_rcheck_status_da_id(
      "dro_get_creating_sd: bad status from dob_variable_tell",
      UER_GET_CREATING_SD,
      temp
    );
  }                                     /* end if undefined                 */
  else {                                /* id defined                       */
/* 
   Generate an error for any symbol type other than a vc.
*/
    if ((real_vc_id = dra_validate(id_parm.dob_handle, vc_sym)) == dob_null_object) { 

      DEBUG1(
        fprintf(stderr, "dra_get_creating_sd: id has invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_CREATING_SD);
      uer_report_da_id_error(uer_error_msg, id_parm);

    }						/* end if invalid type	    */
/* 
   Get handle for symbol table entry.  
*/
    symbol_table_entry = dob_get_symbol_entry(real_vc_id);
    uer_rcheck_status_da_id(
      "dra_get_creating_sd: bad status from dob_get_symbol_entry", 
      UER_GET_CREATING_SD,
      temp
    );
/*
   Get the parent vc property for the object and update temp.
*/
    sd_id = (dob_data_object)cus_get_prop(symbol_table_entry, creating_sd_prop);
    uer_rcheck_status_da_id(
      "dra_get_creating_sd: bad status from cus_get_prop", 
      UER_GET_CREATING_SD,
      temp
    );
/* 
   Update the temp with the appropriate value and type.
*/
    dob_variable_tell(temp.dob_handle, idd_id, (char *)sd_id);
    uer_rcheck_status_da_id(
      "dra_get_creating_sd: bad status from dob_variable_tell",
      UER_GET_CREATING_SD,
      temp
    );
  }                                     /* end else id defined              */
/* 
   delete parameter, if temp 
*/
  dra_delete_temp(id_parm);
  uer_rcheck_status_da_id(
    "dra_get_creating_sd: bad status from dra_delete_temp", 
    UER_GET_CREATING_SD,
    temp
  );

  DEBUG2(
    fprintf(stderr, "dra_get_creating_sd: temp.dob_handle = %#x.\n",
	temp.dob_handle);
    fprintf(stderr, "dra_get_creating_sd: exiting.\n");
  );

  return temp;
}					/* end dra_get_creating_sd	    */


/*--------------------------------------------------------------------------*\
|  Routine: dra_get_vc_id
| 
|  Description:  
|     Gets the id of a specific vc "tied" to an sd
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_vc_id(id_parm, name_parm)
dro_atomic_data_item id_parm;
dro_atomic_data_item name_parm;
{  
  dro_atomic_data_item temp;
  dob_data_object real_sd_id, created_vc_id;
  string vc_template_name_string;
  idd_data_types temp_type;
  boolean id_parm_undef, name_parm_undef;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(
    fprintf(stderr, "dra_get_vc_id: call.\n");
    fprintf(stderr, "dra_get_vc_id: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(stderr, "dra_get_vc_id: id.component_name = %s.\n", 
      id_parm.component_name);
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;
  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id_str(
    "dro_get_vc_id: bad status from dob_temp_create", 
    UER_GET_VC_ID,
    temp
  );
/*
   Check for undefind argument.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_vc_id: bad status from dro_undefined for id_parm", 
    UER_GET_VC_ID,
    temp
  );

  name_parm_undef = dro_undefined(name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_vc_id: bad status from dro_undefined for name_parm", 
    UER_GET_VC_ID,
    temp
  );
/*
   If either argument is undefined then the result is undefined.
*/
  if (id_parm_undef || name_parm_undef) {	/* an argument is undefined */

    dob_variable_tell(temp.dob_handle, idd_undefined, NULL);
    uer_rcheck_status_da_id_str(
      "dra_get_vc_id: bad status from dob_variable_tell for undefined", 
      UER_GET_VC_ID,
      temp
    );
    
  }					/* end if an argument is undefined  */
  else {				/* defined arguments		    */
/*
   Generate an error for any symbol type other than sd.
*/
    if ((real_sd_id = dra_validate(id_parm.dob_handle, sd_sym)) == dob_null_object) {

      DEBUG1(
        fprintf(stderr, "dra_get_vc_id: id has invalid symbol type.\n");
      );
 
      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_VC_ID);
      uer_report_da_id_error(uer_error_msg, id_parm);

    }						/* end if invalid type	    */
/*
   Get a pointer to the vc template name we're looking for.
*/
    vc_template_name_string = (string)dro_ask2(name_parm);
    uer_rcheck_status_da_id_str(
      "dra_get_vc_id: bad status from dro_ask2 for name_parm", 
      UER_GET_VC_ID,
      temp
    );
/* 
   Find the particular vc.
*/
    created_vc_id = dra_find_thingie(
      real_sd_id, 
      sd_sym,
      vc_template_name_string
    );
    uer_rcheck_status_da_id_str(
      "dra_get_vc_id: bad status from dra_find_thingie for created_vc_id", 
      UER_GET_VC_ID,
      temp
    );
/*
   If vc found update the temp.
*/
    if (created_vc_id != dob_null_object) {	/* vc found		    */
/* 
   Update the temp with the appropriate value and type 
*/
      dob_variable_tell(temp.dob_handle, idd_id, (char *)created_vc_id);
      uer_rcheck_status_da_id_str(
        "dra_get_vc_id: bad status from dob_variable_tell",
        UER_GET_VC_ID,
        temp
      );
    }						/* end if vc found	    */
/*
   Else vc not found report error condition.
*/
    else {				/* vc not found			    */

      DEBUG1(
        fprintf(stderr, "dra_get_vc_id: vc not found.\n");
      );

      sprintf(uer_error_msg, UER_NOT_FOUND, UER_GET_VC_ID);
      uer_report_da_id_error(uer_error_msg, id_parm);

    }					/* end else vc not found	    */

  }                                     /* end else arguments defined       */
/* 
   Delete temp parameters.
*/
  dra_delete_temp(id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_vc_id: bad status from dra_delete_temp id_parm", 
    UER_GET_VC_ID,
    temp
  );

  dra_delete_temp (name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_vc_id: bad status from dra_delete_temp name_parm", 
    UER_GET_VC_ID,
    temp
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_get_vc_id: temp.dob_handle = %#x.\n", temp.dob_handle);
    fprintf(stderr, "dra_get_vc_id: exiting.\n");
  );

  return temp;
}					/* end dra_get_vc_id		    */

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_name
| 
|  Description:  
|     Gets the name of a view controller template, shared data item or
|     object.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_name(id_parm)
dro_atomic_data_item id_parm;
{					/* begin			    */
  dro_atomic_data_item temp;
  cus_sym symbol_table_entry;
  dob_data_object real_id, template_id;
  string name;
  cus_sym_type symbol_type;
  boolean id_parm_undef;
/*
   Initialize.
*/
  set_status(ok);

  DEBUG2(
    fprintf(stderr, "dra_get_name: call.\n");
    fprintf(stderr, "dra_get_name: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(
      stderr, 
      "dra_get_name: id_parm.component_name = %s.\n",
      id_parm.component_name
    );
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;

  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id(
    "dra_get_name: bad status from dob_temp_create", 
    UER_GET_NAME,
    temp
  );
/* 
   Check for undefined argument.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id(
    "dra_get_name: bad status from dro_undefined", 
    UER_GET_NAME,
    temp
  );

  if (id_parm_undef) {		/* id_parm undefined			    */
/* 
   Propogate undefined.
*/
    dob_variable_tell(temp.dob_handle, idd_undefined, 0);
    uer_rcheck_status_da_id(
      "dra_get_name: bad status from dob_variable_tell",
      UER_GET_NAME,
      temp
    );
  }					/* end if undefined		    */
  else {				/* id_parm defined		    */
/* 
   Generate an error for a symbol type other than vc, object or variable.
*/
    if (((real_id = dra_validate(id_parm.dob_handle, vc_sym)) == dob_null_object) &&
        ((real_id = dra_validate(id_parm.dob_handle, object_sym)) == dob_null_object) &&
        ((real_id = dra_validate(id_parm.dob_handle, variable_sym)) == dob_null_object))
    {

      DEBUG1(
        fprintf(stderr, "dra_get_name: invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_NAME);
      uer_report_da_id_error(uer_error_msg, id_parm);

    }

    DEBUG3(
      fprintf(stderr, "dra_get_name: real_id = %d.\n", real_id);
    );
/*
   Get the name.
*/
    name = dob_get_real_name(real_id);
    uer_rcheck_status_da_id(
      "dra_get_name: bad status from dob_get_name", 
      UER_GET_NAME,
      temp
    );
/* 
   Update the temp with the appropriate value and type.
*/
    dob_variable_tell(temp.dob_handle, idd_string, name);
    uer_rcheck_status_da_id(
      "dra_get_name: bad status from dob_variable_tell",
      UER_GET_NAME,
      temp
    );
  }					/* end else id_parm defined 	    */
/* 
   Delete id_parm, if temp 
*/
  dra_delete_temp(id_parm);
  uer_rcheck_status_da_id(
    "dra_get_name: bad status from dra_delete_temp",
    UER_GET_NAME,
    temp
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_get_name: temp.dob_handle = %#x.\n", temp.dob_handle);
    fprintf(stderr, "dra_get_name: exiting.\n");
  );

  return temp;
}					/* end dra_get_name		    */

/* ??? does this need to work for attributes and shared data components
       as well for internal purposes? */
/*--------------------------------------------------------------------------*\
|  Routine: dra_get_type
| 
|  Description:  
|     Gets the declaration type of the id.  Valid types are vc, object, 
|     sd and variable.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_type(id_parm)
dro_atomic_data_item id_parm;
{					/* begin			    */
  dro_atomic_data_item temp;
  cus_sym symbol_table_entry;
  cus_sym_type symbol_type;
  dob_data_object real_id, template_id;
  string original_name;
  boolean id_parm_undef;
/*
   Initialize.
*/
  set_status(ok);

  DEBUG2(
    fprintf(stderr, "dra_get_type: call.\n");
    fprintf(stderr, "dra_get_type: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(
      stderr, 
      "dra_get_type: id_parm.component_name = %s.\n",
      id_parm.component_name
    );
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;

  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id(
    "dro_get_parent_vc: bad status from dob_temp_create", 
    UER_GET_TYPE,
    temp
  );
/* 
   Check for undefined argument.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id(
    "dro_get_parent_vc: bad status from dro_undefined", 
    UER_GET_TYPE,
    temp
  );

  if (id_parm_undef) {		/* id undefined			    */
/* 
   Propogate undefined.
*/
    dob_variable_tell(temp.dob_handle, idd_undefined, 0);
    uer_rcheck_status_da_id(
      "dra_get_type: bad status from dob_variable_tell",
      UER_GET_TYPE,
      temp
    );
  }					/* end if undefined		    */
  else {				/* id_parm defined		    */
/* 
   Check for valid id type 
*/
    if (((real_id = dra_validate(id_parm.dob_handle, vc_sym)) == dob_null_object) &&
    ((real_id = dra_validate(id_parm.dob_handle, sd_sym)) == dob_null_object) &&
    ((real_id = dra_validate(id_parm.dob_handle, variable_sym)) == dob_null_object) &&
    ((real_id = dra_validate(id_parm.dob_handle, object_sym)) == dob_null_object))
    {

      DEBUG1(
        fprintf(stderr, "dra_get_type:  invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_TYPE);
      uer_report_da_id_error(uer_error_msg, id_parm);

  } 
/*
   Get the symbol type for the specified data item.
*/
    symbol_type = dob_get_symbol_type(real_id);
    uer_rcheck_status_da_id(
      "dra_get_type: bad status from dob_get_symbol_type",
      UER_GET_TYPE,
      temp
    );
/* 
   Update the temp with the appropriate value and type.
*/
    dob_variable_tell(
      temp.dob_handle, 
      idd_string,
      cus_get_sym_type_string(symbol_type)
    );
    uer_rcheck_status_da_id(
      "dra_get_type: bad status from dob_variable_tell",
      UER_GET_TYPE,
      temp
    );

  }					/* end else id_parm defined	    */
/* 
   Delete id_parm, if temp 
*/
  dra_delete_temp(id_parm);
  uer_rcheck_status_da_id(
    "dra_get_type: bad status from dra_delete_temp",
    UER_GET_TYPE,
    temp
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_get_type: temp.dob_handle = %#x.\n", temp.dob_handle);
    fprintf(stderr, "dra_get_type: exiting.\n");
  );

  return temp;
}					/* end dra_get_type		    */

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_object
| 
|  Description:  
|     Gets the id of the named object within a vc instance.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_object(id_parm, name_parm)
dro_atomic_data_item id_parm;
dro_atomic_data_item name_parm;
{  
  dro_atomic_data_item temp;
  dob_data_object real_vc_id, object_id;
  string object_name_string;
  boolean id_parm_undef, name_parm_undef;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dra_get_object: call.\n");
    fprintf(stderr, "dra_get_object: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(stderr, "dra_get_object: id.component_name = %s.\n",
	id_parm.component_name);
  );
/*  
   Create a temp to return the result in.
*/
  temp.component_name = NULL;

  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id_str(
    "dro_get_object: bad status from dob_temp_create", 
    UER_GET_OBJECT,
    temp
  );
/* 
   Check for undefined arguments.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_object: bad status from dro_undefined for id_parm", 
    UER_GET_OBJECT,
    temp
  );

  name_parm_undef = dro_undefined(name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_object: bad status from dro_undefined for name_parm", 
    UER_GET_OBJECT,
    temp
  );

  if (id_parm_undef || name_parm_undef) {	/* undefined arguments	    */

    dob_variable_tell (temp.dob_handle, idd_undefined, NULL);
    uer_rcheck_status_da_id_str(
      "dra_get_object: bad status from dob_variable_tell for undefined", 
      UER_GET_OBJECT,
      temp
    );

  }					/* end if undefined arguments	    */
  else {				/* arguments are defined	    */
/* 
   Generate an error for anything other than an object symbol.  
*/
    if ((real_vc_id = dra_validate (id_parm.dob_handle, vc_sym)) == dob_null_object) {

      DEBUG1(
        fprintf(stderr, "dra_get_object: id has invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_OBJECT);
      uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);

    }						/* end if invalid type	    */
/* 
   Get a pointer to the object name.  
*/
    object_name_string = (string)dro_ask2(name_parm);
    uer_rcheck_status_da_id_str(
      "dra_get_object: bad status from dro_ask2 for name_parm", 
      UER_GET_OBJECT,
      temp
    );

    object_id = dra_find_thingie(real_vc_id, object_sym, object_name_string);
    uer_rcheck_status_da_id_str(
      "dra_get_object: bad status from dra_find_thingie for object_id", 
      UER_GET_OBJECT,
      temp
    );
/*
   Real object.
*/
    if (object_id != dob_null_object) {	/* object found			    */
/* 
   Update the temp with the appropriate value and type 
*/
      dob_variable_tell(temp.dob_handle, idd_id, (char *)object_id);
      uer_rcheck_status_da_id_str(
        "dra_get_object: bad status from dob_variable_tell",
        UER_GET_OBJECT,
        temp
      );
    }				/* end if object found			    */
/*
   Else object not found report error condition.
*/
    else {				/* object not found		    */

      DEBUG1(
        fprintf(stderr, "dra_get_object: vc not found.\n");
      );

      sprintf(uer_error_msg, UER_NOT_FOUND, UER_GET_OBJECT);
      uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);

    }					/* end else object not found	    */

  }                                     /* end else arguments defined       */
/*
   Delete temp parameters.
*/
  dra_delete_temp (id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_object: bad status from delete_temp for id_parm",
    UER_GET_OBJECT,
    temp
  );

  dra_delete_temp (name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_object: bad status from delete_temp for name_parm",
    UER_GET_OBJECT,
    temp
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_get_object: temp.dob_handle = %#x\n", temp.dob_handle);
    fprintf(stderr, "dra_get_object: exiting.\n");
  );

  return temp;
}					/* end dra_get_object	    */

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_component_value
| 
|  Description:  
|     Gets the value of the specified component.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_component_value(id_parm, name_parm)
dro_atomic_data_item id_parm;
dro_atomic_data_item name_parm;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  dob_data_object real_sd_id;
  idd_data_types temp_type;
  string component_name_string;
  caddr_t temp_value;
  boolean id_parm_undef, name_parm_undef;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dra_get_comp_value: call.\n");
    fprintf(stderr, "dra_get_comp_value: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(stderr, "dra_get_comp_value: id_parm.component_name = %s.\n", 
      id_parm.component_name);
    fprintf(
      stderr, 
      "dra_get_comp_value: name_parm.dob_handle = %#x.\n",
	name_parm.dob_handle);
    fprintf(
      stderr, "dra_get_comp_value: name_parm.component_name = %s.\n", 
      name_parm.component_name);
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;

  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id_str(
    "dra_get_comp_value: bad status from dob_temp_create", 
    UER_GET_COMPONENT_VALUE,
    temp
  );
/*
   Check for undefined arguments.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_comp_value: bad status from dro_undefined for id_parm", 
    UER_GET_COMPONENT_VALUE,
    temp
  );

  name_parm_undef = dro_undefined(name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_comp_value: bad status from dro_undefined for name_parm", 
    UER_GET_COMPONENT_VALUE,
    temp
  );

  if (id_parm_undef || name_parm_undef) {

    dob_variable_tell (temp.dob_handle, idd_undefined, NULL);
    uer_rcheck_status_da_id_str(
     "dra_get_comp_value: bad status from dob_variable_tell for undefined", 
      UER_GET_COMPONENT_VALUE,
      temp
    );

  }                                     /* end if undefined arguments       */
  else {                                /* arguments are defined            */
/* 
   Generate an error for any symbol type other than shared data or an 
   object.
*/
    if (((real_sd_id = dra_validate(id_parm.dob_handle, sd_sym)) == dob_null_object) &&
       ((real_sd_id = dra_validate(id_parm.dob_handle, object_sym)) == dob_null_object))
    {

      DEBUG1(
        fprintf(stderr, "dra_get_comp_value: id has invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_COMPONENT_VALUE);
      uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);

    }                                           /* end if invalid type      */
/* 
   Get the component name string.  
*/
    component_name_string = (string)dro_ask2(name_parm);
    uer_rcheck_status_da_id_str(
      "dra_get_comp_value: bad status from dro_ask2", 
      UER_GET_COMPONENT_VALUE,
      temp
    );
/* 
   Get the type and value of the specified component.  
*/
    temp_type = dob_get_base_data_type(real_sd_id, component_name_string);
    uer_rcheck_status_da_id_str(
      "dra_get_comp_value: bad status from get_data_type", 
      UER_GET_COMPONENT_VALUE,
      temp
    );

    temp_value = dob_ask(real_sd_id, component_name_string);
     uer_rcheck_status_da_id_str(
      "dra_get_comp_value: bad status from dob_ask", 
      UER_GET_COMPONENT_VALUE,
      temp
    );
/* 
   Update the temp with the appropriate value and type.  
*/
    dob_variable_tell (temp.dob_handle, temp_type, temp_value);
    uer_rcheck_status_da_id_str ( 
      "dra_get_comp_value: bad status from dob_variable_tell", 
      UER_GET_COMPONENT_VALUE,
      temp);

  }                                     /* end else arguments defined       */
/*
   Delete temp parameters.
*/
  dra_delete_temp (id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_comp_value: bad status from dra_delete_temp sd_id",
    UER_GET_COMPONENT_VALUE,
    temp
  );
  
  dra_delete_temp (name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_comp_value: bad status from dra_delete_temp name_parm",
    UER_GET_COMPONENT_VALUE,
    temp
  );
 
  DEBUG2(
    fprintf(stderr, "dra_get_comp_value: temp.dob_handle = %#x.\n",
	temp.dob_handle);
    fprintf(stderr, "dra_get_comp_value: exiting.\n");
  );

  return temp;
}					/* end dra_get_comp_value	    */

/*
??? test setting an attribute to self
*/
/*--------------------------------------------------------------------------*\
|  Routine: dra_put_component_value
| 
|  Description:  
|     Puts a specified value into a component of a shared data element.
\*--------------------------------------------------------------------------*/
void dra_put_component_value(id_parm, name_parm, value)
dro_atomic_data_item id_parm, name_parm, value;
{					/* local type definition	    */
  dob_data_object real_sd_id;
  string component_name_string;
  idd_data_types temp_type;
  caddr_t temp_value;
  boolean id_parm_undef, name_parm_undef;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(
    fprintf(stderr, "dra_put_comp_value: call.\n");
    fprintf(stderr, "dra_put_comp_value: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(stderr, "dra_put_comp_value: id.component_name = %s.\n", 
      id_parm.component_name);
  );
/*
   Generate an error for anything other than a shared data or object symbol.
*/
  if ( ((real_sd_id = dra_validate(id_parm.dob_handle, sd_sym)) == dob_null_object) &&
       ((real_sd_id = dra_validate(id_parm.dob_handle, object_sym)) == dob_null_object)) 
  {

    DEBUG1(
      fprintf(stderr, "dra_put_comp_value: id has invalid symbol type.\n");
    );

    sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_PUT_COMPONENT_VALUE);
    uer_report_da_id_str_val_error(uer_error_msg, id_parm, name_parm, value);

  }                                           /* end if invalid type      */
/* 
   Get the component name string.  
*/
  component_name_string = (string)dro_ask2(name_parm);
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from dro_ask2",
    UER_PUT_COMPONENT_VALUE
  );
/*
   Get the base to type to get the actual typed value of the stuff to be 
   stored.
*/
  temp_type = dob_get_base_data_type(
    real_sd_id,
    component_name_string
  );
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from dob_get_base_data_type",
    UER_PUT_COMPONENT_VALUE
  );

  temp_value = drc_get_atv(value, temp_type);
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from drc_get_atv",
    UER_PUT_COMPONENT_VALUE
  );
/*
   Assign the value to the specified component.
*/
  dob_shared_data_tell(real_sd_id, component_name_string, temp_value);
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from dob_shared_data_tell",
    UER_PUT_VARIABLE_VALUE
  );
/* 
   Delete the parameters if they are temps 
*/
  dra_delete_temp(id_parm);
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from dra_delete_temp id_parm",
    UER_PUT_COMPONENT_VALUE
  );

  dra_delete_temp (name_parm);
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from dra_delete_temp name_parm",
    UER_PUT_COMPONENT_VALUE
  );

  dra_delete_temp (value);
  uer_check_status_da_id_str_val(
    "dra_put_comp_value: bad status from dra_delete_temp",
    UER_PUT_COMPONENT_VALUE
  );

/*
   Return.
*/
  DEBUG2(fprintf(stderr, "dra_put_comp_value: exiting.\n"););

  return;
}					/* end dra_put_comp_value	    */



/*--------------------------------------------------------------------------*\
|  Routine: dra_get_variable_value
| 
|  Description:  
|     Gets the value of the specified variable.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_get_variable_value(id_parm, name_parm)
dro_atomic_data_item id_parm;
dro_atomic_data_item name_parm;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  dob_data_object real_vc_id, variable_id;
  idd_data_types temp_type;
  string variable_name_string;
  caddr_t temp_value;
  boolean id_parm_undef, name_parm_undef;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(
    fprintf(stderr, "dra_get_variable_value: call\n");
    fprintf(stderr, "   : id_parm.dob_handle = %#x\n", id_parm.dob_handle);
    fprintf(stderr, "   : id_parm.component_name = %s\n", 
      id_parm.component_name);
    fprintf(
      stderr, 
      "dra_get_variable_value: name_parm.dob_handle = %#x\n", 
      name_parm.dob_handle
    );
  );
/* 
   Create a temp to return the result in.
*/
  temp.component_name = NULL;
  temp.dob_handle = dob_temp_create();
  uer_rcheck_status_da_id_str(
    "dra_get_variable_value: bad status from dob_temp_create", 
    UER_GET_VARIABLE_VALUE,
    temp
  );
/*
   Check for undefined arguments.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_variable_value: bad status from dro_undefined of id_parm_undef", 
    UER_GET_VARIABLE_VALUE,
    temp
  );

  name_parm_undef = dro_undefined(name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_variable_value: bad status from dro_undefined for name_parm_undef", 
    UER_GET_VARIABLE_VALUE,
    temp
  );
/*
   If either argument is undefined then the result is undefined.
*/
  if (id_parm_undef || name_parm_undef) {	/* undefined arguments     */
    dob_variable_tell (temp.dob_handle, idd_undefined, NULL);
    uer_rcheck_status_da_id_str(
      "dra_get_variable_value: bad status from dob_variable_tell for undefined",
      UER_GET_VARIABLE_VALUE,
     temp
    );
  }                                     /* end if undefined arguments       */
  else {                                /* defined arguments                */
/*
   Generate an error for any symbol type otehr than a vc.
*/
   if ((real_vc_id = dra_validate(id_parm.dob_handle, vc_sym))
	== dob_null_object)
  {

     DEBUG1(
       fprintf(stderr, "dra_get_variable_value: id has invalid symbol type.\n");
     );

     sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_GET_VARIABLE_VALUE);
     uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);

   }                                           /* end if invalid type      */
/* 
   Get the variable name string.  
*/
    variable_name_string = (string)dro_ask2(name_parm);
    uer_rcheck_status_da_id_str(
      "dra_get_variable_value: bad status from dro_ask2", 
      UER_GET_VARIABLE_VALUE,
      temp
    );
/* 
   get the correct variable, do not need to check for errors, 
   find_thingie craps out if there are 
*/
    variable_id = dra_find_thingie(
      real_vc_id, 
      variable_sym,
      variable_name_string
    );
/*
    Check for valid id type of parent 
*/
    if (variable_id == dob_null_object) {/* variable not found		    */
      
      DEBUG1(
        fprintf(stderr, "dra_get_variable_value: variable not found.\n");
      );

      sprintf(uer_error_msg, UER_NOT_FOUND, UER_GET_VARIABLE_VALUE);
      uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);

    }					/* end if variable not found	    */
/* 
   Get the type and value of the specified variable 
*/
    temp_type = dob_get_base_data_type(variable_id, NULL);
    uer_rcheck_status_da_id_str(
      "dra_get_variable_value: bad status from dob_get_base_data_type",
      UER_GET_VARIABLE_VALUE,
      temp
    );

    temp_value = dob_ask(variable_id, NULL);
    uer_rcheck_status_da_id_str(
      "dra_get_variable_value: bad status from dob_ask", 
      UER_GET_VARIABLE_VALUE,
      temp
    );
/* 
   put the value into a temp, along with the type 
*/
    dob_variable_tell (temp.dob_handle, temp_type, temp_value);
    uer_rcheck_status_da_id_str(
      "dra_get_variable_value: bad status from dob_variable_tell", 
      UER_GET_VARIABLE_VALUE,
      temp
    );
  }                                     /* end else arguments defined       */
/* 
   delete the parameters if the are temporary
*/
  dra_delete_temp (id_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_variable_value: bad status from dra_delete_temp id_parm", 
    UER_GET_VARIABLE_VALUE,
    temp
  );

  dra_delete_temp (name_parm);
  uer_rcheck_status_da_id_str(
    "dra_get_variable_value: bad status from dra_delete_temp name_parm",
    UER_GET_VARIABLE_VALUE,
    temp
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_get_variable_value: temp.dob_handle = %#x.\n",
	temp.dob_handle);
    fprintf(stderr, "dra_get_variable_value: exiting.\n");
  );

  return temp;
}					/* end dra_get_variable_value	    */

/*--------------------------------------------------------------------------*\
|  Routine: dra_put_variable_value
| 
|  Description:  
|     Puts a specified value into a variable.  If the arguments (other
|     than the value) are undefined this is basically a no-op.
\*--------------------------------------------------------------------------*/
void dra_put_variable_value(id_parm, name_parm, value)
dro_atomic_data_item id_parm, name_parm, value;
{					/* local type definition	    */
  dob_data_object real_vc_id, variable_id;
  idd_data_types temp_type;
  string variable_name_string;
  caddr_t temp_value;
  boolean id_parm_undef, name_parm_undef;
/*
   Initialize.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(
    fprintf(stderr, "dra_put_var_value: call.\n");
    fprintf(stderr, "dra_put_var_value: id_parm.dob_handle = %#x.\n",
	id_parm.dob_handle);
    fprintf(stderr, "dra_put_var_value: id_parm.component_name = %s.\n", 
      id_parm.component_name);
    fprintf(stderr, "dra_put_var_value: name_parm.dob_handle = %#x.\n",
	name_parm.dob_handle);
    fprintf(stderr, "dra_put_var_value: name_parm.component_name = %s.\n", 
      name_parm.component_name);
    fprintf(stderr, "dra_put_var_value: value.dob_handle = %#x.\n",
	value.dob_handle);
    fprintf(stderr, "dra_put_var_value: value.component_name = %s.\n", 
      value.component_name);
  );
/*
   Check for undefined arguments.
*/
  id_parm_undef = dro_undefined(id_parm);
  uer_check_status_da_id_str_val(
    "dra_put_var_value: bad status from dro_undefined of id_parm_undef",
    UER_PUT_VARIABLE_VALUE
  );

  name_parm_undef = dro_undefined(name_parm);
  uer_check_status_da_id_str_val(
    "dra_put_var_value: bad status from dro_undefined of name_parm_undef",
    UER_PUT_VARIABLE_VALUE
  );
/*
   If arguments are undefined return without doing anything.
*/
  if (id_parm_undef || name_parm_undef) {	/*  undefined arguments	    */

    DEBUG3(
      fprintf(stderr, "dra_put_var_value: undefined id or name parms.\n");
    );

    return;
  }					/* end if undefined arguments	    */
  else {                                /* defined arguments                */
/* 
   Generate an error for any sympl that is not a vc.
*/
    if ((real_vc_id = dra_validate (id_parm.dob_handle, vc_sym))
	== dob_null_object)
    {

      DEBUG1(
        fprintf(stderr, "dra_put_var_value: id has invalid symbol type.\n");
      );

      sprintf(uer_error_msg, UER_INVALID_SYMBOL, UER_PUT_VARIABLE_VALUE);
      uer_report_da_id_str_val_error(uer_error_msg, id_parm, name_parm, value);

    }                                           /* end if invalid type      */
/* 
   Get the variable name string.  
*/
    variable_name_string = (string)dro_ask2(name_parm);
    uer_check_status_da_id_str_val(
      "dra_put_var_value: bad status from dro_ask2",
      UER_PUT_VARIABLE_VALUE
    );

    DEBUG3(
      fprintf(stderr, "dra_put_var_value: variable_name_string = %s.\n",
        variable_name_string
      );
    );

/*
   Get the correct variable.
*/
    variable_id = dra_find_thingie(
      real_vc_id, 
      variable_sym, 
      variable_name_string
    );
    uer_check_status_da_id_str_val(
      "dra_put_var_value: bad status from dra_find_thingie of variable_id",
      UER_PUT_VARIABLE_VALUE
    );
/*
   If the varible is not found generate an error.
*/
    if (variable_id == dob_null_object) {/* variable not found		    */

      DEBUG1(
        fprintf(stderr, "dra_get_var_value: variable not found.\n");
      );

      sprintf(uer_error_msg, UER_NOT_FOUND, UER_PUT_VARIABLE_VALUE);
      uer_report_da_id_str_val_error(uer_error_msg, id_parm, name_parm, value);

    }					/* end if variable not found	    */
/* 
   Get the type and value of the stuff to be stored 
*/
    temp_type = dob_get_base_data_type(value.dob_handle, value.component_name);
    uer_check_status_da_id_str_val(
      "dra_put_var_value: bad status from dob_get_base_data_type",
      UER_PUT_VARIABLE_VALUE
    );

    temp_value = dob_ask(value.dob_handle, value.component_name);
    uer_check_status_da_id_str_val(
      "dra_put_var_value: bad status from dob_ask",
      UER_PUT_VARIABLE_VALUE
    );
/* 
   Update the temp with the appropriate value and type.  
*/
    dob_variable_tell(variable_id, temp_type, temp_value);
    uer_check_status_da_id_str_val(
      "dra_put_var_value: bad status from dob_variable_tell",
      UER_PUT_VARIABLE_VALUE
    );
  }                                     /* end else arguments defined       */
/*
   Delete temp parameters.
*/
  dra_delete_temp (id_parm);
  uer_check_status_da_id_str_val(
    "dra_put_var_value: bad status from dra_delete_temp",
    UER_PUT_VARIABLE_VALUE
  );

  dra_delete_temp (name_parm);
  uer_check_status_da_id_str_val(
    "dra_put_var_value: bad status from dra_delete_temp",
    UER_PUT_VARIABLE_VALUE
  );

  dra_delete_temp (value);
  uer_check_status_da_id_str_val(
    "dra_put_var_value: bad status from dra_delete_temp",
    UER_PUT_VARIABLE_VALUE
  );

  DEBUG2(fprintf(stderr, "dra_put_var_value: exiting.\n"););

  return;
}					/* end dra_put_var_value	    */


/*	dra_find_mailbox is not used for now.  it was used earlier and then
	was not needed.  it was left just in case it is needed again. */

#if  0
/*--------------------------------------------------------------------------*\
|  Routine: dra_find_mailbox
| 
|  Description:  
|     finds the mailbox corresponding to an element name
\*--------------------------------------------------------------------------*/
/*
?? dra_find_mailbox does things very simplistically, it finds the "first"
tap which has an element with the name in question and gets corresponding the
mailbox.  there is no allowance for overlading of element names between taps,
but that is ok beause there is also no allowance for that in slang, either.
*/
static ipc_mailbox dra_find_mailbox (element_name)
string element_name;

{  LIST tap_list;
   isd_process tap;
   idd_data_definition dd;
   ipc_mailbox the_mailbox;

   DEBUG2 (fprintf (stderr, "dra_find_mailbox: call\n");
      fprintf (stderr, "element_name = %s\n", element_name););

   set_status (ok);
   tap_list = (LIST) cus_get_prop (d22_god_vc, taps_prop);
   loop_through_list (tap_list, tap, isd_process)
      {dd = isd_get_data_definition (tap);
      if (!get_status ())
         {the_mailbox = isd_get_mailbox (tap);
         rcheck_status ("dra_find_mailbox: call to isd_get_mailbox failed\n",
		the_mailbox);
         DEBUG3 (fprintf (stderr, "dra_find_mailbox: exit\n");
              fprintf (stderr, "the_mailbox = %d\n", the_mailbox););
         return (the_mailbox);
         }
      }
   set_status (dra_cannot_find_element);
   DEBUG2 (fprintf (stderr, "dra_find_mailbox: exit (ERROR)\n");
      fprintf (stderr, "the_mailbox = %d\n", the_mailbox););
   return (the_mailbox);
}					/* end dra_find_mailbox */
#endif


/*--------------------------------------------------------------------------*\
|  Routine: dra_delete_element
| 
|  Description:  
|     Creates a shared data element.
\*--------------------------------------------------------------------------*/
void dra_delete_element (element)
dro_atomic_data_item element;
{  
  dob_data_object element_id;

  DEBUG2 (fprintf(stderr, "dra_dra_delete_element: call\n");
    fprintf(stderr, "dra_delete_element: element.dob_handle = %#x.\n",
		element.dob_handle);
    fprintf(stderr, "dra_delete_element: element.component_name = %s.\n", 
      element.component_name););

                                /* check arguments for undefinedness */
   if (dro_undefined (element))
      goto FINISHED;

   element_id = (dob_data_object) dro_ask2 (element);
   check_status ("dra_delete_element: bad status from dob_ask for element");

   DEBUG3 (fprintf (stderr, "dra_delete_element: element id = %d\n",
		    element_id););
   dob_destroy_data_object (element_id);
   check_status("dra_delete_element: bad status from dob_destroy_data_object");

FINISHED:

   DEBUG2 (fprintf(stderr, "dra_delete_element: exiting.\n"););

   return;
}


/*--------------------------------------------------------------------------*\
|  Routine: dra_create_element
| 
|  Description:  
|     Creates a shared data element.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_create_element (element_adi, mailbox_adi)
dro_atomic_data_item element_adi;
dro_atomic_data_item mailbox_adi;
{  
  dro_atomic_data_item temp;
  dob_data_object object_id;
  string element_string, mailbox_string;
  ipc_mailbox mailbox;
  idd_data_types temp_type;
   

  set_status(ok);			/* begin			    */

  DEBUG2 (fprintf(stderr, "dra_dra_create_element: call\n");
    fprintf(stderr, "dra_create_element: element_adi.dob_handle = %#x.\n",
		element_adi.dob_handle);
    fprintf(stderr, "dra_create_element: element_adi.component_name = %s.\n", 
      element_adi.component_name););

                                /* Create a temp to return the result in,
                                   and set its initial value to undefined  */
  temp.component_name = NULL;
  temp.dob_handle = dob_temp_create ();
  rcheck_status ("dra_create_element: bad status from dob_temp_create", temp);
  dob_variable_tell (temp.dob_handle, idd_undefined, NULL);
  rcheck_status (
        "dra_create_element: bad status from dob_variable_tell for undefined",
        temp);

				/* initialize "returned" values */
   temp_type = idd_undefined;
   object_id = dob_null_object;

                                /* check arguments for undefinedness */
   if (dro_undefined (element_adi) || dro_undefined (mailbox_adi))
      goto FINISHED;
				/* find the element name */
   element_string = (string) dro_ask2 (element_adi);
   rcheck_status (
	"dra_create_element: bad status from dob_ask for element_adi",
	temp);
   DEBUG3 (fprintf (stderr, "dra_create_element: element name = %s\n",
		    element_string););

				/* find the mailbox name */
   mailbox_string = (string) dro_ask2 (mailbox_adi);
   rcheck_status (
	"dra_create_element: bad status from dob_ask for mailbox_adi",
	temp);
   DEBUG3 (fprintf (stderr, "dra_create_element: mailbox name = %s\n",
		    mailbox_string););

				/* get the mailbox object corresponding to the
				   mailbox string name */
   mailbox = ipc_init (mailbox_string);
   DEBUG3 (fprintf (stderr, "dra_create_element: mailbox object = %d\n",
		    mailbox););

				/* now make the object and register its
				   creation */
   object_id = dob_slang_obj_create (element_string, mailbox, NULL, NULL);
   DEBUG3 (fprintf (stderr, "dra_create_element: new sd id = %d\n",
		    object_id););
   if (object_id != dob_null_object)
      temp_type = idd_id;

FINISHED:
                /* Update the temp with the appropriate value and type */
   dob_variable_tell (temp.dob_handle, temp_type, (char *) object_id);
   rcheck_status( "dra_create_element: bad status from dob_variable_tell",
    temp);

                        /* delete parameters, if they are temps */
   dra_delete_temp (element_adi);
   rcheck_status (
	"dra_create_element: bad status from delete_temp for element_adi",
        temp);

   DEBUG2 (fprintf(stderr, "dra_create_element: temp.dob_handle = %#x\n",
	temp.dob_handle);
    fprintf(stderr, "dra_create_element: exiting.\n"););

   return temp;
}					/* end dra_create_element */

/*--------------------------------------------------------------------------*\
|  Routine: dra_new
|
|  Description:
|     Creates a shared data element.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dra_new (element)
dro_atomic_data_item element;
{
  dro_atomic_data_item temp;

  DEBUG2(
    fprintf(stderr, "dra_new: call.\n");
  );

  temp = dob_make_boolean_constant_adi (1);

  DEBUG2(
    fprintf(stderr, "dra_new: exit.\n");
  );

  return temp;

}

#if 0

/*--------------------------------------------------------------------------*\
|  Routine: dra_new
| 
|  Description:  
|     Determines if a view controller should be instantiated
|     for the given combination of shared data instance, vc template,
|     and parent vc.
\*--------------------------------------------------------------------------*/
boolean dra_new(element, vc_template_name, parent_vc)
dro_atomic_data_item element, vc_template_name, parent_vc;
{  					/* begin			    */
  dob_data_object real_element, real_parent_vc;
  cus_sym element_ste, parent_vc_ste;
  boolean ret_val = false;
/*
   Initialize.
*/
  set_status(ok);

  DEBUG2(
    fprintf(stderr, "dra_new: call.\n");
    fprintf(stderr, "dra_new: element = %x.\n", element);
    fprintf(stderr, "dra_new: element.dob_handle = %#x.\n", element.dob_handle);
    fprintf(
      stderr, 
      "dra_new: element.component_name = %s.\n", 
      element.component_name
    );
    fprintf(stderr, "dra_new: vc_template_name = %x.\n", vc_template_name);
    fprintf(
      stderr, 
      "dra_new: vc_template_name.dob_handle = %#x.\n", 
      vc_template_name.id
    );
    fprintf(
      stderr, 
      "dra_new: vc_template_name.component_name = %s.\n", 
      vc_template_name.component_name
    );
    fprintf(stderr, "dra_new: parent_vc = %x.\n", parent_vc);
    fprintf(stderr, "dra_new: parent_vc.dob_handle = %#x.\n",
	parent_vc.dob_handle);
    fprintf(
      stderr, 
      "dra_new: parent_vc.component_name = %s.\n", 
      parent_vc.component_name
    );
  );
/*
   Check to make sure that the element is a real shared data instance.
*/
  if ((real_element = dra_validate(element.dob_handle, sd_sym))
	== dob_null_object)
  {

    DEBUG1(
      fprintf(stderr, "dra_new: element parm has invalid symbol type.\n");
    );

    sprintf(uer_error_msg, UER_INVALID_SYMBOL, DRA_NEW);
    uer_report_new_error(uer_error_msg, element, vc_template_name, parent_vc);

  }                                             /* end if invalid type      */
/*
   Make sure that the parent_vc is a real view controller instance.
*/
  if ((real_parent_vc = dra_validate(element.dob_handle, vc_sym))
	== dob_null_object)
  {

    DEBUG1(
      fprintf(stderr, "dra_new: parent vc has invalid symbol type.\n");
    );

    sprintf(uer_error_msg, UER_INVALID_SYMBOL, DRA_NEW);
    uer_report_new_error(uer_error_msg, element, vc_template_name, parent_vc);

  }                                             /* end if invalid type      */
/* 
   ??? can any of the parameters be undefined?  if so it should
   probably be treated as a run time error.
*/
/*
  Get symbol table entry for the element.
*/
  element_ste = dob_get_symbol_entry(real_element);
  uer_rcheck_status_new(
    "dra_new: bad status from dob_get_symbol_entry"
  );
/*
   Get the list of vcs instantiated for this property.
*/
  vc_list = (LIST)cus_get_prop(
    element_ste,
    created_vcs_prop
  );
  uer_rcheck_status_new(
    "dra_new: bad status from cus_get_prop of created_vcs_prop"
  );
/*
   Get the actual template name.
*/
  vc_template_name_string = (string)dro_ask2(vc_template_name);
  uer_rcheck_status_new(
    "dra_new: bad status from dro_ask2 for vc_template_name"
  );
/*
   Examine list vc list to determine if this vc has already been
   created or if it needs to be created.
*/
  loop_through_list(vc_list, vc_id, dob_data_object) {/* while list not empty */
/*
   Get the real name for the vc
*/
    real_name = dob_get_real_name(vc_id);
    uer_rcheck_status_new(
      "dra_new: bad status from cus_get_prop of created_vcs_prop"
    );
/*
   Determine if a new vc should be created.
*/
    if (strcmp(vc_template_name_string, real_name) == 0) { /* same name	    */

      symbol_table_entry =  dob_get_symbol_entry(vc_id);
      uer_rcheck_status_new(
        "dra_new: bad status from cus_get_prop of created_vcs_prop"
      );

      temp_parent_vc = (dob_data_object)cus_get_prop(
        symbol_table_entry, 
        template_prop
      );
      uer_rcheck_status_new(
        "dra_new: bad status from cus_get_prop of created_vcs_prop"
      );
/*
   If the parent vc is the same then return true.
*/
      if (temp_parent_vc == parent_vc) {
        ret_val = true;
      }
    }				/* end if names compare equal		    */

  }				/* end loop through list		    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dra_new: ret_val = %d.\n", ret_val);
    fprintf(stderr, "dra_new: exit.\n");
  );

  return ret_val;
}					/* end dra_new */


#endif
