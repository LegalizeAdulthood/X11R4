static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/dob.c,v 1.42 89/10/23 19:40:52 ejh Exp $";
/*--------------------------------------------------------------------------*\
|
|  Name:  Object Base Manager  (dob)
|
|  Description: 
|     The object base manager provides a common means for accessing data
|     elements within the dialogue manager.  The object base manager provide
|     a front end to both the symbol table and the shared data component
|     of the dialogue manager
|
|  Component of
|     Dialogue Manager Layer
|
|  Related files
|     dob.c - contains the implementation for the object base manager
|     dob.h - contains the external definition for the object base  manager
|
|  Project
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author: 
|     Robert C. Seacord
|     rcs@sei.cmu.edu 
|
|  Change History
|     May 24, 1988 : Module created
$Log:	dob.c,v $
 * Revision 1.42  89/10/23  19:40:52  ejh
 * cosmetics
 * 
 * Revision 1.41  89/10/05  16:57:00  rcs
 * various mods to support typed buffers
 * 
 * Revision 1.40  89/09/12  21:56:35  bmc
 * Fixed dob_variable_tell to update the variable value (by updating the
 * symbol table current_value_prop) when the old value is NULL.
 * 
 * Revision 1.39  89/09/07  17:27:11  trm
 * fixed remove_shared_data case in dob_transaction;
 * also, changed some object checks to dob_validate_object.   trm.
 * 
 * Revision 1.38  89/09/01  14:05:43  bmc
 * Changed dob_validate_object to check for NULL properly.  Changed
 * rcheck_object and check_object macros to reports errors as DEBUG3
 * rather than DEBUG1.
 * 
 * Revision 1.37  89/08/31  19:00:48  bmc
 * Referencing a removed object in dob_get_symbol_type no longer results
 * in an unconditional error message.  (Necessary to support addition of
 * dra_id_exists() function.)
 * 
 * Revision 1.36  89/08/25  15:40:21  bmc
 * Fixed bug associated with VC deletion.  (SPR #91)
 * 
 * Revision 1.35  89/08/23  14:04:49  bmc
 * 1) Functions now check for NULL and dob_null_object when validating input
 *    parameters of type dob_data_object.  Macros check_object() and
 *    rcheck_object() added to facilitate these checks.  (SPR #98)
 * 2) dob_get_real_name no longer looks in the hash table.  (SPR #83)
 * 
 * Revision 1.34  89/07/25  21:47:43  bmc
 * - Changed 'is_reserved_word_prop' to newly-created 'sd_template_prop'.
 * - Fixed bug in dob_destroy_data_object.  It was using a template object
 *   without checking to make sure cus_get_prop actually returned one.
 * - Fixed a status code related bug in dob_destroy_data_object.
 * 
 * Revision 1.33  89/07/24  14:58:21  little
 * ops removal
 * 
 * Revision 1.32  89/06/15  15:28:24  little
 * more fixin for dob_assign (string again)
 * 
 * Revision 1.31  89/06/14  18:18:28  little
 * fix the string assignment in dob_assign, the strncpy was one off
 * 
 * Revision 1.30  89/06/14  10:57:40  little
 * fix so "changes" to sd components which are the same as the original value
 *   do not get sent to the respective tap.
 * 
 * Revision 1.29  89/05/22  11:10:33  bmc
 * Now explicitly include 'uer.h'.  Related to changes to 'dob.c', 'dob.h'
 * and 'uer.h'.
 * 
 * Revision 1.28  89/05/19  17:17:54  bmc
 * Objects in the object base are now referred to via handles (pointers)
 * as well as ids.  Client routines no longer operate on ids.  Use of
 * pointers allowed elimination of many get_hashtable() calls.
 * 
 * Revision 1.27  89/04/13  20:59:31  little
 * place the checking of the temp free list for duplicates in dob_temp_release
 * under debug3 control.
 * 
 * Revision 1.26  89/03/28  12:51:18  serpent
 * change #ifdef 0  to  #if 0  to allow sun compile.
 * 
 * Revision 1.25  89/03/10  18:24:35  rcs
 * fixed bug in dob_variable_tell
 * 
 * Revision 1.24  89/03/01  17:09:01  rcs
 * modified dob_temp_release to generate a FATAL_ERROR when releasing
 * already 
 * released temps.
 * 
 * Revision 1.23  89/02/23  14:28:33  rcs
 * fixed dob_get_real_name to return the name, not the type of objects
 * 
 * Revision 1.22  89/02/21  13:39:58  rcs
 * fixed problem with null strings in dob_variable_tell
 * 
 * Revision 1.21  89/02/09  17:37:17  ejh
 * added copyright notice
 * 
 * Revision 1.20  89/02/01  21:56:00  rcs
 * fixed debug print in dob_get_real_name
 * 
 * Revision 1.19  89/01/30  13:34:55  rcs
 * changed objects from having sd_sym to object_sym
 * added dob_get_real_name routine
 * modified dob_validate_object to recognize objects which have
 *   been removed from the object base
 * 
 * Revision 1.18  89/01/11  18:56:26  little
 * make dob_undefined work with vc, var, and id type symbols (ie,
 * they are determined to not be undefined).
 * add some debug print
 * 
 * Revision 1.17  89/01/04  16:58:12  rcs
 * modified dob_destroy_data_object to leave objects in the objectbase but
 * mark them as deleted
 * modified transaction update to not fail when receiving a modify for
 * a deleted object but print out a warning
 * modified all other dob routines to set status to removed_data_object
 * and return when reference object has been deleted
 * 
 * Revision 1.16  88/12/14  18:03:50  rcs
 * added actions on destroy
 * 
 * Revision 1.15  88/12/14  09:47:23  little
 * change the order that var and sub-vc instances appear on their respective
 * lists within a vc to reflect the lexical ordering from the slang
 * program
 * 
 * Revision 1.14  88/12/06  15:52:11  rcs
 * fixed create_element to intialize dm shared data to UNDEFINED
 * 
 * Revision 1.13  88/11/08  14:03:47  little
 * fix delete objecct so that when sd is deleted, any necessary changes are
 * made to the parent vc if the sd is part of a vc.  also, change
 * delete object to not actually delete the thing until all sub-things
 * have been deleted.
 * 
 * Revision 1.12  88/11/04  17:01:54  little
 * change DEBUG1 to print d22_slang_source
 * 
 * Revision 1.11  88/10/31  15:20:46  little
 * add some error print
 * 
 * Revision 1.10  88/10/28  19:01:59  little
 * add some extra error print out to destroy_object in the destroy vc
 * variables area
 * 
 * Revision 1.9  88/10/27  19:09:45  little
 * add dob_temp_release routine and make dob_temp_create use cached temps
 * 
 * Revision 1.8  88/10/26  17:23:09  little
 * no changes, stupid checkin process
 * 
 * Revision 1.7  88/10/26  11:11:17  little
 * added the dob_validate_object routine, which checks to see if
 * an object exists
 * 
 * Revision 1.6  88/10/24  17:15:44  little
 * change calls to dpg_propagate_modify to use the new (third) parameter,
 * which is the name of the component being modified.
 * 
 * Revision 1.5  88/10/07  15:39:27  little
 * not modified.
 * 
 * Revision 1.4  88/10/04  17:47:17  little
 * add parent id parameter to propagate_create calls
 * 
 * Revision 1.3  88/10/03  09:12:52  little
 * add dm shared data
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

#define dob_main
#define memoryPack

#include <strings.h>
#include "listPack.h"
#include "dob.h"
#include "dgr.h"
#include "din.h"
#include "d22_debug.h"
#include "iid.h"
#include "isd_priv.h"
#include "cud.h"
#include "uer.h"

#define FATAL_ERROR(statement)\
   {  long pid;\
      char command [256];\
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

      /* The "check_object" and "rcheck_object" macros are supersets of the
	 "check_null" and "rcheck_null" macros defined in "isc.h". They exist
	 solely to check the validity of a dob object handle.  Unlike
	 "check_null", et al, these macros must check for more than simply a
	 NULL pointer. In both macros, the "object" argument must be of type
	 "dob_data_object". */

#define rcheck_object(object,msg_prefix,ret_val) \
  if ( (object == dob_null_object) || (object == NULL) ) \
    {\
    DEBUG3 (\
      (void) fprintf (stderr, "%s: null object specified.\n", msg_prefix);\
    );\
    set_status (invalid_data_object);\
    return(ret_val);\
    }

#define check_object(object,msg_prefix) \
  if ( (object == dob_null_object) || (object == NULL) ) \
    {\
    DEBUG3 (\
      (void) fprintf (stderr, "%s: null object specified.\n", msg_prefix);\
    );\
    set_status (invalid_data_object);\
    return;\
    }

#define TEMP_NAME_BUF_SIZE    (50)
#define VAR_SYMBOL_PREFIX     "VAR_"
#define SD_SYMBOL_PREFIX      "SD_"
#define ROOM_LEFT_IN_VAR_BUF  (TEMP_NAME_BUF_SIZE - sizeof (VAR_SYMBOL_PREFIX))
#define ROOM_LEFT_IN_SD_BUF   (TEMP_NAME_BUF_SIZE - sizeof (SD_SYMBOL_PREFIX))

/*
??? d22_temp_free_list deninition should be in h file we can use here
*/
extern int set_debug; /******************* REMOVE THIS!! ********************/
extern caddr_t d22_temp_free_list;

/*
??? put LOOP_THROUGH_LIST in an h file soon
*/
#define LOOP_THROUGH_LIST( list, reg ) \
    for( reg = LISThead( list ); reg != NULLlist; reg = reg -> next )

#define DEBUG1(statement) DM_RT_DEBUG1(dob_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(dob_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(dob_debug, statement)

extern int d22_slang_line_num;

#define MAX_HASH 256

typedef enum {
  symbol_table_type,
  shared_data_table_type,
  removed
} table_types;

typedef struct {
  iid_id_type id;
  table_types table_type;
  string element_name;
  cus_sym ste;
  caddr_t data;					/* ptr to shared data	    */
  ipc_mailbox mailbox;
} data_object_record;

/*--------------------------------------------------------------------------*\
|  The following macro, while admittedly ugly, dumps the entire contents
|  of a dob_data_object to standard error.  It's useful primarily for
|  debugging.
|
|  Input arguments:
|	the_object - A variable of type dob_data_object.  If the object
|		     is null, the macro prints that out.  Otherwise, it
|		     casts the object to a pointer to data_object_record,
|		     and dumps out every field on a separate line, preceded
|		     by a tab.
|	prefix     - A character string to print out before printing the
|		     fields of the record.  Typically, this value will be
|		     the name of the module.
\*--------------------------------------------------------------------------*/

#define dump_data_object(the_object,prefix) \
{\
fprintf(stderr,"%s: the_object",prefix); \
if (the_object == dob_null_object) \
  fprintf(stderr," is dob_null_object\n"); \
else \
  fprintf(stderr," = %#x\n\tid = %d\n\ttable_type = %d\n\telement_name = %s\n\tste = %#x\n\tdata = %#x\n\tmailbox = %#x\n",\
  ((data_object_record *)the_object),\
  ((data_object_record *)the_object)->id,\
  ((data_object_record *)the_object)->table_type,\
  ((data_object_record *)the_object)->element_name == NULL ? "NULL" : ((data_object_record *)the_object)->element_name,\
  ((data_object_record *)the_object)->ste,\
  ((data_object_record *)the_object)->data,\
  ((data_object_record *)the_object)->mailbox);\
}

/*-------------------- Retained Data ---------------------------------------*/

static HASH dob_id_table;	/* hash table of ids stored in obj base     */

/*--------------------------------------------------------------------------*\
|  Routine: dob_undefined
| 
|  Description:  
|     Returns true if undefined.
\*--------------------------------------------------------------------------*/
boolean dob_undefined (op1_type, op1_data)
idd_data_types op1_type;
caddr_t op1_data;
{					/* local type definition	    */
  boolean value;

  set_status (ok);

  DEBUG2 (fprintf (stderr, "dob_undefined: call.\n");
    fprintf (stderr, "dob_undefined: op1_type = %s.\n", 
      idd_get_type_string (op1_type));
    fprintf (stderr, "dob_undefined:  op1_data = %x.\n", op1_data););
/* 
   Type determines which version of undefined to compare with.
*/
    switch (op1_type) {			
      case idd_boolean:
        value = ((boolean)op1_data == UNDEFINED_BOOLEAN);
        break;
      case idd_integer:
        value = ((int)op1_data == UNDEFINED_INTEGER);
        break;
      case idd_real:
        value = ((double)*op1_data == UNDEFINED_REAL);
        break;
      case idd_string:
        value = (!strcmp((string)op1_data, UNDEFINED_STRING));
        break;
      case idd_record:
        set_status(operation_undefined_types);
        return(value);
      case idd_id:
        value = ((iid_id_type)op1_data == UNDEFINED_ID);
        break;
      case idd_buffer:
        value = ((int)*op1_data == UNDEFINED_BUFFER_LENGTH); 
        break;
      case idd_undefined:
        value = true;
        break;
				/* ???? what about default case */
    }

  DEBUG2 (fprintf (stderr, "dob_undefined: value = %d.\n", value);
    fprintf (stderr, "dob_undefined: exiting.\n"););

  return value;
}					/* end dob_undefined		    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_match_id
|
|  Description: 
|     Internal routine to compare an identifier with the id in a shared 
|     data record.
\*--------------------------------------------------------------------------*/
static int dob_match_id(id, data_object)
iid_id_type id;
data_object_record *data_object;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */
  return (data_object->id == id);
} 					/* end dob_match_id		    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_hash_id
|
|  Description: 
|     Internal function which will convert an id into indices
|     in the array of hash lists.
\*--------------------------------------------------------------------------*/
static int dob_hash_id(id)
iid_id_type id;
{ 					/* local type definitions	    */
  set_status (ok);

				/* Return a value in the right range */
  return ((((int)id) & 077777) % MAX_HASH);
} 					/* end dob_hash_id		    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_init
|
|  Description: 
|     Performs necessary intialization of the object base manager
\*--------------------------------------------------------------------------*/
void dob_init()
{                                         /* local type definitions	    */
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_init: call.\n"););
/*
   Create the id hash table
*/
  dob_id_table = make_hashtable(MAX_HASH, dob_hash_id, dob_match_id);
  check_null (dob_id_table,
	"dob_init.main:  out of memory during make id hashtable.",
	out_of_memory);
/*
   Return
*/
  DEBUG2 (fprintf(stderr, "dob_init: exiting.\n"););
  return;
}                                         /* end dob_init		    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_validate_object
|
|  Description: 
|     Checks to see if an object is still in the data base; returns true
|	if it is, else false.
\*--------------------------------------------------------------------------*/
boolean dob_validate_object (object)
dob_data_object object;
{
   boolean ret_val;

   set_status (ok);
   DEBUG2 (fprintf (stderr, "dob_validate_object: call.");
      fprintf (stderr, "dob_validate_object: object = %x.\n", object););

   rcheck_object (object, "dob_validate_object:", false);
   if (((data_object_record *) object) -> table_type != removed)

	/* Object has not been removed. */
     ret_val = true;
   else
     ret_val = false;

   DEBUG2 (fprintf (stderr, "dob_validate_object: exiting");
      fprintf (stderr, "  ret_val = %d\n", ret_val););
   return (ret_val);
}

/*--------------------------------------------------------------------------*\
|  Routine: dob_assign
|
|  Description:
|     Assigns a value to a container based on the specified type.
|     Only makes the assignment if the new value differs from the one in 
|     the container.
|     LOCAL ROUTINE.
|
|  Arguments:
|     container           - The address of the container to receive the value.
|                           This container is presumed to hold the old value,
|                           if there is one.
|     value               - Either the address of the value to place in the
|                           container, or the value itself (casted to caddr_t),
|                           depending upon the type.
|     type                - The data type.
|     container_length    - The length of the container, in bytes.  Should be
|                           set to 0 if it doesn't apply.  This parameter
|                           is currently only examined for strings and ids.
|
|  Return Value:
|     true  - The assignment actually took place and ought to be propagated.
|     false - The assignment did not take place, since the old and new values
|             were identical.
\*--------------------------------------------------------------------------*/
static boolean dob_assign(container, value, type, container_length)
caddr_t container;
caddr_t value;
idd_data_types type;
int container_length;
{
  boolean assigned = false;
  int source_length;
/* 
   The following pointers exist solely to make the code more readable.  
   Typecasting combined with double de-referencing leads to really ugly code.
*/
  int *ip;
  iid_id_type *idp;
  double *dp;
  char *sp;
  idd_buffer_type *bp;
  int str_copy_offset;

  DEBUG2(
    fprintf(stderr, "dob_assign: call.\n");
    fprintf(stderr, "dob_assign: container = %x.\n", container);
    fprintf(stderr, "dob_assign: value = %x.\n", value);
    fprintf(stderr, "dob_assign: type = %s.\n", idd_get_type_string(type));
    fprintf(stderr, "dob_assign: container_length = %d.\n", container_length);
  );

  switch (type) {		/* switch on type			    */

/*--------------------------------------------------------------------------*\
|  Boolean, integer
\*--------------------------------------------------------------------------*/
    case idd_boolean:
    case idd_integer:
/* 
   "value" represents immediate data in this case.  "container" represents 
    a pointer to an integer.
*/
      ip = (int *) container;
      if ((int)value != *ip) {	/* value changed	    */

        *ip = (int) value;
        assigned = true;

      }					/* end if integer value changed     */
      break;

/*--------------------------------------------------------------------------*\
|  ID
\*--------------------------------------------------------------------------*/
    case idd_id:
/* 
   "value" represents a pointer to an object.  "container" represents a 
    pointer to an "id". The logic for handling IDs is kind of flaky.
    Whoever called us (probably the dialog) thinks that "value" represents 
    the id of some object.  However, it's really a pointer to an object.  The
    problem is that we may have received an invalid pointer: (1) the compiler
    may have put the value "iid_null_id" into the pointer, since it thinks
    it's dealing with ids; (2) someone somewhere may have stored or passed a
    value of "dob_null_object".  In either of those cases, we have to store a
    null id value, since this data is possibly being passed through a
    transaction.  Otherwise, it's safe to dereference the pointer.
*/
      idp = (iid_id_type *) container;
      if ( ( ((iid_id_type) value) == iid_null_id ) ||
           ( ((iid_id_type) value) == UNDEFINED_ID ) ||
           ( ((dob_data_object) value) == dob_null_object ) ) {
	*idp = iid_null_id;
	assigned = true;
      }
      else {
/*
	Safe to dereference.  We cheat a little here, and use "container_length"
	to determine whether we're dealing with shared data or not.  We
	have to process differently for shared data.
*/
	if (container_length > 0) {
	  /* Shared data.  "Value" represents an object. */

	  if (*idp != ((data_object_record *) value) -> id) {
	    *idp = ((data_object_record *) value) -> id;
	    assigned = true;
	  }
	}

	else {
	  /* Handle like an integer. */
	  if (*idp != (iid_id_type) value) {
	    *idp = (iid_id_type) value;
	    assigned = true;
	  }
	}
      }
      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/
    case idd_real:

/*
      "value" represents a pointer to a double in this case.  So, "container"
      represents a pointer to a pointer to a double, and must be dereferenced
      accordingly.  (Yech!)
*/
      dp = *((double **) container); /* now we have a pointer to a double */
      if (*((double *)value) != *dp) { /* value changed */

        *dp = *((double *)value);
        assigned = true;

      }				/* end if real value changed	    */
 
      break;

/*--------------------------------------------------------------------------*\
|  String
\*--------------------------------------------------------------------------*/
    case idd_string:

/*
      "value" represents a pointer to a string.  What "container" depends on
      who called us.   Initially, assume it's a "char *"
*/
      source_length = strlen (value);
      if (container_length == 0)
         str_copy_offset = 1;
        else
         str_copy_offset = 0;
      if ((strcmp(value, (char *) container) != 0) || (source_length == 0)) {
			/* strings are different or new string is null    */

/*
	If container_length is 0, then we have to make sure the container is
	large enough to handle the source string.  If it isn't, we allocate a
	new one.  If container_length is greater than 0, then the container is
	a fixed length (typically for a shared data component), and we simply
	truncate the source string if necessary.
*/

	sp = (char *) container;
	if (container_length == 0) { /* variable length container */

				/* Container represents a "char **".  */

	  sp = *((char **) container);
	  container_length = strlen (sp);
	  if (container_length < source_length) {
	    /* old string too small  */

	    free_node(sp);

	    sp = (caddr_t) make_node(source_length+1);
	    check_null(
	      sp,
	      "dob_assign: out of memory during make node for string.\n",
	      out_of_memory
	    );
	    *((char **) container) = sp;
	    container_length = source_length;

	  }				/* end if old string too small	    */
	} /* end else container length is variable */

	(void) strncpy (sp, value, container_length + str_copy_offset);

        assigned = true;

      }				/* end if string value changed      */

      break;

/*--------------------------------------------------------------------------*\
|  Records
\*--------------------------------------------------------------------------*/
/*
   Records are not currently supported
*/
    case idd_record:

      DEBUG1(fprintf(stderr, "dob_assign: idd_record type invalid.\n"););
      set_status(invalid_data_type);
      break;

/*--------------------------------------------------------------------------*\
|  Buffers
\*--------------------------------------------------------------------------*/
    case idd_buffer:
/*
      "value" represents a pointer to a buffer.  So, "container" represents
      a pointer to a pointer to a buffer.
*/
      if (container_length == 0)	/* variable */
         bp = *((idd_buffer_type **) container);
      else
         bp = ((idd_buffer_type *) container);

      if (buffcmp((idd_buffer_type *)value, bp) != true) {

/*
   Buffer contents are not equal.  Free the old buffer body if it exists,
   and allocate a new one large enough to handle the new value.
*/
	if (bp->body != NULL) {

	  free_node(bp->body);
	  bp->body = NULL;

	}

	buffcpy(bp, (idd_buffer_type *)value);
        check_status("dob_assign: bad status from buffcpy.");

        assigned = true;

      }				/* end if value changed	    */

      break;

/*--------------------------------------------------------------------------*\
|  Undefined
\*--------------------------------------------------------------------------*/
/*
   For undefined type there is no associated value so simply break.
*/
    case idd_undefined:
      assigned = true;
      break;

/*--------------------------------------------------------------------------*\
|  Default
\*--------------------------------------------------------------------------*/
    default:

      DEBUG1(fprintf(stderr, "dob_assign: invalid data type.\n"););
      set_status(invalid_data_type);
      break;

  }				/* end switch on data type		    */

  DEBUG2(
    fprintf(stderr, "dob_assign: assigned = %d.\n", assigned);
    fprintf(stderr, "dob_assign: exiting.\n");
  );

  return (assigned);
}					  /* end dob_assign */

/*--------------------------------------------------------------------------*\
|  Routine: dob_sd_create
|
|  Description: 
|     Internal routine to create a shared data object in the object base
|     without knowing if this was caused by an external transaction or
|     by a dialogue writer request.
|
|     This routine is used by both dob_slang_obj_create and transaction
|     update.
\*--------------------------------------------------------------------------*/
static data_object_record *dob_sd_create (element_name, mailbox)
string element_name;
ipc_mailbox mailbox;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  int length, components;
  int i;				/* counter			    */
  isd_process process;
  idd_data_definition data_definition;
  static char temp_name [TEMP_NAME_BUF_SIZE];
  LIST temp_list;
  LIST *ask_property;
  char great_big_string [2048];
  cus_sym element_template;

  set_status(ok);
  DEBUG2 (fprintf(stderr, "dob_sd_create: call.\n");
    fprintf (stderr, "dob_sd_create: element name = %s.\n", element_name);
    fprintf(stderr, "dob_sd_create: mailbox = %x.\n", mailbox););

/*
   Create a new data object record, initialize table type field and se
   mailbox field
*/
  data_object = (data_object_record *)make_node(sizeof(data_object_record));
  rcheck_null (data_object,
     "dob_sd_create: out of memory during make node data obj rec.\n",
     out_of_memory, NULL);

  data_object->table_type = shared_data_table_type;
  data_object->mailbox = mailbox;
/*
   Allocate memory for the element name and save in data object record
*/
  data_object->element_name = (string)make_node(strlen(element_name)+1);
  rcheck_null (data_object->element_name,
     "dob_sd_create: out of memory during make node element name.\n",
     out_of_memory, NULL);

  strcpy(data_object->element_name, element_name);
/*
   Determine size of memory required to maintain an instance o
   the shared data element and allocate the memory
*/
  process = isd_getfrom_process_table(data_object->mailbox);
  rcheck_status ("dob_sd_create: bad status from idd_getfrom_process_table.",
     NULL);

  data_definition = isd_get_data_definition(process);
  rcheck_status("dob_sd_create: bad status from idd_get_data_def.", NULL);

  length = idd_get_length (data_definition, element_name);
  rcheck_status ("dob_sd_create: bad status from idd_get_length.", NULL);

  data_object->data = make_node(length);
  rcheck_null (data_object->data,
    "dob_sd_create: out of memory during make node sd instance.\n",
    out_of_memory, NULL);
/*
   Initialize data to UNDEFINED.
*/
  isd_init_sd_element (data_definition, element_name, data_object->data);
  rcheck_status ("dob_sd_create: bad status from idd_init_sd_element.", NULL);
/*
   Generate a unique symbol in the symbol table for the shared data
   element.  Be sure not to overwrite buffer.
*/
  (void) strcpy (temp_name, SD_SYMBOL_PREFIX);
  (void) strncat (temp_name, data_object->element_name, ROOM_LEFT_IN_SD_BUF);

  data_object->ste = cus_gensym(temp_name, sd_sym);
  rcheck_status("dob_sd_create: bad status from cus_gensym.", NULL);
/*
   Determine the size of the array based on the number of component
   in the element
*/
  components = idd_get_number_of_components(data_definition,element_name) + 1;
  rcheck_status ("dob_sd_create: bad status from idd_get_#_of_comps.", NULL);
/*
   Create the ask property list.  This array of lists keeps track of
   who references each component in the item.  The array is indexed by
   the component's position within the element.
*/
  ask_property = (LIST *) make_node(components * (sizeof(LIST)));
  for (i = 0; i < components; i++) {
    ask_property[i] = make_list(); 
/*
    ask_set = (char *) make_list();
*/
    rcheck_null (ask_property[i],
	"dob_sd_create: out of memory during make list ask prop.\n",
	out_of_memory, NULL);
    }
/*
   Put ask property in shared data symbol table entry
*/
  cus_put_prop(data_object->ste, ask_prop, (char *) ask_property);
  rcheck_status("dob_sd_create: bad status put ask_prop.", NULL);
/*
   Create created vcs property
*/
  temp_list = make_list();
  rcheck_null (temp_list, "dob_sd_create: out of memory during make_list.\n",
    out_of_memory, NULL);

  cus_put_prop (data_object->ste, created_vcs_prop, (char *) temp_list);
  rcheck_status ("dob_sd_create: bad status from cus_put_prop.", NULL);

				/* put in data definition property */
  cus_put_prop (data_object->ste, data_definition_prop, data_definition);
  rcheck_status ("dob_sd_create: bad status from cus_put_prop.", NULL);

                                /* now get the element's template, add the
				   element instance to  the template and add
				   the template to the instance.  pay close
				   attention, bunky.
                                */
  strcpy (great_big_string, SD_QUALIFIER_STRING);
  strcat (great_big_string, element_name);
  element_template = cus_look_up (great_big_string);
  if (!element_template)
     FATAL_ERROR (fprintf (stderr,
       "dob_sd_create: cannot find element template for %s\n",element_name););

  temp_list = (LIST) cus_get_prop (element_template, instances_prop);
  add_to_head (temp_list, data_object);

  cus_put_prop (data_object->ste, sd_template_prop,
	cus_get_prop (element_template, id_prop));

  DEBUG2 (fprintf (stderr, "dob_sd_create: exiting.\n"););

  return data_object;
}                                         /* end dob_sd_create */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_object_id
|
|  Description: 
|     Converts a dob_data_object into its corresponding id.  Not to be
|     used lightly!
\*--------------------------------------------------------------------------*/
iid_id_type dob_get_object_id (object)
dob_data_object object;
{                                         /* local type definitions	    */
  set_status (ok);
  rcheck_null(object, "dob_get_object_id: null object specified\n",
    invalid_data_object, iid_null_id);

  return ( ((data_object_record *) object) -> id );
}

/*--------------------------------------------------------------------------*\
|  Routine: dob_transaction_update
|
|  Description: 
|     Updates the object base according to changes specified by the
|     transaction
\*--------------------------------------------------------------------------*/
void dob_transaction_update(transaction)
isd_trans transaction;
{                                         /* local type definitions	    */
  iid_id_type id;
  isd_change_type change_type;
  string element_name;
  ipc_mailbox mailbox;
  isd_process process;
  isd_trans return_transaction;
  data_object_record *data_object;
  data_object_record *object_to_remove;
  LIST changed_component_list;
  register LIST component;
  int length, offset, position;

/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_transaction_update: call.\n");
    fprintf(stderr, "dob_trans_update: transaction = %x.\n", transaction););

/*
   Get the senders mailbox and process record.
*/
  mailbox = isd_get_sender(transaction);
  check_status("dob_trans_update: bad status from isd_get_sender.");

  process = isd_getfrom_process_table(mailbox);
  check_status("dob_trans_update: bad status from isd_getfrom_process_table."
  );
/*
   Specify the data definition for the TAP from which the transaction was
   received.
*/
  isd_specify_data_definition(transaction, process);
  check_status(
    "dob_trans_update: bad status from isd_specify_data_definition.");
/*
   Get the id of the first changed element from the transaction
*/
  id = isd_get_first_changed_element(transaction);
  check_status("dob_trans_update: bad status from isd_get_1st_chgd_elem.");
/*
   Update the object base according to changes specified by the transaction
*/
  while (id != iid_null_id) {		/* while not out of changed elems   */

    DEBUG2 (fprintf(stderr,
	"dob_transaction_update: id of current transaction element = %d\n",
	id););
/*
   Get the name and change type of the current element
*/
    element_name = isd_get_element_name(transaction, id);
    check_status("dob_trans_update: bad status from isd_get_element_name.");

    change_type = isd_get_change_type(transaction, id);
    check_status("dob_trans_update: bad status from isd_get_change_type.");

    DEBUG3 (fprintf(stderr,
	"dob_trans_update: change type = %d\n", change_type););
/*
   If newly created element add to object base
*/
    switch (change_type) {		/* switch on change type	    */

/*--------------------------------------------------------------------------*\
|  Create a new element in shared data.					     
\*--------------------------------------------------------------------------*/
      case isd_create:			/* newly created element	    */
/*
   Create the shared data object in the object base and set the id to the
   element id from the transaction
*/
        data_object = dob_sd_create(element_name, mailbox);
        check_status("dob_trans_update: bad status from dob_sd_create.");
        data_object->id = id;
/*
   Put handle of created shared data element in id property.

??? The id_prop property name probably ought to be changed.
*/
        cus_put_prop(data_object->ste, id_prop, (char *) data_object);
        check_status("dob_trans_update: bad status put id_prop.");
/*
   Add data object to hashtable using the id returned from add shared
   data
*/
        add_to_hashtable(dob_id_table, data_object, data_object->id);
/*
   Get shared data element from the interface
*/
        data_object->data = isd_get_shared_data(transaction, id, NULL);
        check_status ("dob_trans_update: bad status from isd_get_sd.");
/*
   Propogate create for shared data object
*/
        dpg_propagate_create (data_object, dob_null_object);
        check_status (
		"dob_trans_update: bad status from dpg_propagate_create.");

        break;

/*--------------------------------------------------------------------------*\
|  Modify an existing element in shared data.				     
\*--------------------------------------------------------------------------*/

      case isd_modify:			/* element modified		    */
/*
   Get the data object record from the id table
*/
        data_object = (data_object_record *)get_from_hashtable(dob_id_table,
          id);
        check_null(data_object,
          "dob_trans_update: bad status from get_from_hash of data obj.\n",
          not_found);
/*
   If the object has been removed from the object base print a warning 
   message to standard error.
*/
        if (data_object->table_type == removed)	/* object removed   */
          fprintf(stderr, UER_DELETED_OBJECT, data_object->id,
            data_object->element_name);
/*
   Make modifications if the element is not marked as removed in the database.
*/
        else {  				/* object not removed       */
/*
   Incorporate changes into shared data object
*/
          isd_incorporate_changes(transaction, id, data_object->data);
          check_status("dob_trans_update: bad status from isd_inc_changes.");
/*
   Propogate changes for each changed component
*/  
          changed_component_list = isd_create_changed_component_list(
	    transaction, data_object->id);
          check_status(
            "dob_trans_update: bad status from isd_create_changed_comp_list."
          );

          LOOP_THROUGH_LIST (changed_component_list, component) {
            idd_get_component_lop(((rcvd_tran *)transaction)->data_definition,
	      element_name, (string) (LISTnode (component)), &length, &offset,
              &position);
            dpg_propagate_modify(data_object, position,
	      (string)(LISTnode(component)));
            check_status(
		"dob_trans_update: bad status from dpg_propagate_modify.");
          }

          isd_destroy_changed_component_list(changed_component_list);
          check_status(
            "dob_trans_update: bad status from isd_destroy_changed_comp_list."
          );
	}			/* end else object has not been removed	    */

        break;

/*--------------------------------------------------------------------------*\
|  Remove an existing element from shared data.				     
\*--------------------------------------------------------------------------*/

      case isd_remove:			/* remove element		    */
/*
   Get the data object record from the id table
*/
        object_to_remove  = (data_object_record *)get_from_hashtable(
          dob_id_table, id);
        check_null(object_to_remove,
          "dob_trans_update: bad status from get_from_hash of data obj.\n",
          not_found);
/*
   Delete the element from shared data - cast it to object, NOT ptr to obj.
*/
        dob_destroy_data_object( (dob_data_object) object_to_remove );
        check_status("dob_trans_update: bad status from dob_dstry_data_obj.");

        break;

/*--------------------------------------------------------------------------*\
|  Add requested element to transaction.				     
\*--------------------------------------------------------------------------*/

      case isd_get:			/* get element			    */
/*
   Get the data object record from the id table
*/
        data_object = (data_object_record *)get_from_hashtable(dob_id_table,
          id);
        check_null(data_object,
          "dob_trans_update: bad status from get_from_hash of data obj.\n",
          not_found);
/*
   If the object has been removed from the object base print a warning 
   message to standard error.
*/
        if (data_object->table_type == removed)	/* object removed   */
          fprintf(stderr, UER_DELETED_OBJECT, data_object->id, 
            data_object->element_name); 
/*
   Otherwise get the requested element.
*/
        else { 		 			/* object in object base    */
/*
   Find transaction associated with specified mailbox
*/
          return_transaction = dtt_transaction_ask(data_object->mailbox);
          check_status("dob_trans_update: bad status from dob_trans_ask.");
/*
   Put shared data to transaction
*/
          isd_put_shared_data(return_transaction, id,
            data_object->element_name, NULL, data_object->data);
          check_status("dob_trans_update: bad status from isd_put_shd_data.");

	}			/* end else object in object base     */

        break;
/*
   Otherwise error
*/
      default:				/* invalid change type		    */

        set_status(invalid_change_type);
        DEBUG1(isc_print_status("dob_trans_update:"););
        return;

    }					/* end switch on change type	    */
  id = isd_get_next_changed_element (transaction);
  check_status
     ("dob_transaction_update: bad status from isd_get_next_changed_element");

  }					/* end while not out of changes     */
/*
   All done purge transaction
*/
  isd_purge_transaction(transaction);
  check_status("dob_trans_update: bad status from isd_purge_transaction.");
/*
   Return
*/
  DEBUG2 (fprintf(stderr, "dob_transaction_update: exiting.\n"););

  return;
}                                         /* end dob_transaction_update */

/*--------------------------------------------------------------------------*\
|  Routine: dob_create_symbol_table_entry
|
|  Description: 
|     Creates an object in the object base for an existing symbol table
|     handle.  Returns a handle to the object.
\*--------------------------------------------------------------------------*/
dob_data_object dob_create_symbol_table_entry(symbol_table_handle)
cus_sym symbol_table_handle;
{                                         /* local type definitions	    */
  data_object_record *data_object;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_create_symbol_table_entry: call.\n"););

/*
   Create a new data object record and intialize table type field an
   data fields
*/
  data_object = (data_object_record *)make_node(sizeof(data_object_record));
  rcheck_null(data_object,
    "dob_crt_symtble_entry: out of memory during make_node of data_object.\n",
    out_of_memory, dob_null_object);

  data_object->table_type = symbol_table_type;

  data_object->ste = symbol_table_handle;
/*
   Create a new id and insert in data object structure
*/
  data_object->id = iid_create_id();
  rcheck_status("dob_create_sym_table_entry: bad status from iid_create_id.",
     dob_null_object);
/*
  Add data object to hash table
*/
  add_to_hashtable(dob_id_table, data_object, data_object->id);
/*
   Return.  Cast the pointer to the private dob_data_object type.
*/
  DEBUG2 (fprintf(stderr, "dob_create_symbol_table_entry: exiting.\n"););

  return ((dob_data_object) data_object);
}                                      /* end dob_create_symbol_table_entry */

/*
???  maybe dob_slang_obj_create should be renamed 
*/
/*--------------------------------------------------------------------------*\
|  Routine: dob_slang_obj_create
|
|  Description: 
|     Creates a shared data instance and notifies the specified process
\*--------------------------------------------------------------------------*/
dob_data_object dob_slang_obj_create (element_name, mailbox, template,
	parent_vc)
string element_name;
ipc_mailbox mailbox;
dob_data_object template;
dob_data_object parent_vc;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  isd_trans transaction;

  set_status(ok);  		          /* begin                          */

  DEBUG2 ( fprintf(stderr, "dob_slang_obj_create: call.\n");
    fprintf ( stderr,
      "    element name = %s, mailbox = %d, template = %x, parent_vc = %x\n",
      element_name, mailbox, template, parent_vc););

			/* Create the shared data object in the object base */
  data_object = dob_sd_create(element_name, mailbox);
  rcheck_status (
    "dob_slang_obj_create: bad status from dob_sd_create.", dob_null_object);
/* 
   Find transaction associated with specified mailbox
*/
  transaction = dtt_transaction_ask(mailbox);
  DEBUG3 (fprintf (stderr, "dob_slang_obj_create: transaction = %x\n",
	transaction););
  rcheck_status ( "dob_slang_obj_create: bad status from dob_trana_ask.",
      dob_null_object);
   if (transaction)		/* having a transaction => that this is not
				   dm shared data, go ahead and place add in
				   transaction */

				/* Add shared data to transaction.  This is
				   going to overwrite the existing id in the
				   data object record */
      {data_object->id = isd_add_shared_data (transaction, element_name,
					     NULL, NULL);
      rcheck_status (
	"dob_slang_obj_create: bad status from isd_add_shrd_data.",
	dob_null_object);
       }
    else			/* must be dm shared data, just get an id,
				   no need to put it in a transaction */
      data_object->id = iid_create_id ();

				/* Add data object to hashtable using the id
				   returned from add share data */
  add_to_hashtable(dob_id_table, data_object, data_object->id);

				 /* Put handle of created shared data element
				    in id property */
  cus_put_prop (data_object->ste, id_prop, (char *) data_object);
  rcheck_status ("dob_slang_obj_create: bad status put id_prop.",
    dob_null_object);

/* 
  Put handle of parent VC in symbol table PARENT_VC property.  Note: If
  there is no parent VC, we want to store "dob_null_object".  The compiler
  may have generated code that passes a NULL, rather than "dob_null_object",
  so we have to test for both conditions when determining whether the
  parent VC is defined.
*/
  if ( (parent_vc == dob_null_object) || (parent_vc == NULL) )
   cus_put_prop (data_object->ste, parent_vc_prop, (caddr_t) dob_null_object);
  else
    cus_put_prop (data_object->ste, parent_vc_prop, (caddr_t) parent_vc);
  rcheck_status ("dob_slang_obj_create: bad status put parent_vc_prop.",
    dob_null_object);

/* 
  Put handle of template in symbol table TEMPLATE property.  Note: If there
  there is no template, we want to store "dob_null_object".  The compiler
  may have generated code that passes a NULL, rather than "dob_null_object",
  so we have to test for both conditions when determining whether the
  template is defined.
*/

  if ( (template == dob_null_object) || (template == NULL) )
    cus_put_prop(data_object->ste, template_prop, (caddr_t) dob_null_object);
  else
    cus_put_prop(data_object->ste, template_prop, (caddr_t) template);
  rcheck_status( "dob_slang_obj_create: bad status put template_prop.",
    dob_null_object);

				/* Propogate create */
  dpg_propagate_create(data_object, parent_vc);
  rcheck_status (
	"dob_slang_obj_create: bad status from dpg_propagate_create.",
    	dob_null_object);

  DEBUG2 (fprintf(stderr, "dob_slang_obj_create: exiting. handle = %x\n",
	(long) data_object););

  return ((dob_data_object) data_object);
}                                         /* end dob_slang_obj_create */

/*--------------------------------------------------------------------------*\
|  Routine: dob_shared_data_tell
|
|  Description: 
|     Updates a shared data component.  The type of the data is assumed 
|     to be the type of the shared data component.
\*--------------------------------------------------------------------------*/
void dob_shared_data_tell(shared_data, component_name, data)
dob_data_object shared_data;
string component_name;
caddr_t data;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  int length, offset, position;
  idd_buffer_type *data_buffer;
  idd_buffer_type *sd_buffer;
  idd_data_types data_type;
  isd_trans transaction;
  isd_process process;
  idd_data_definition data_definition;
  boolean propagate_ok = false;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "dob_shared_data_tell: call.\n");
    fprintf(stderr, "shared_data = %x, component = %s, data = %d\n",
      shared_data, component_name, data););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  check_object(shared_data, "dob_shared_data_tell:");

  data_object = (data_object_record *)shared_data;
  DEBUG3(
    fprintf (stderr, "dob_shared_data_tell: shared_data->element_name = %s\n",
      data_object->element_name););
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1(isc_print_status("dob_shared_data_tell:"););
    return;
  }
/*
   Get length and offset of specified component within element
*/
  process = isd_getfrom_process_table(data_object->mailbox);
  check_status("dob_sd_tell: bad status from idd_getfrom_process_tale.");

  data_definition = isd_get_data_definition(process);
  check_status("dob_shared_data_tell: bad status from idd_get_data_def.");

  idd_get_component_lop(data_definition, data_object->element_name,
    component_name, &length, &offset, &position);
  check_status("dob_shared_data_tell: bad status from idd_get_comp_lop.");
/*
   Get component data type
*/
  data_type = idd_get_shared_data_type(data_definition,
    data_object->element_name, component_name);
  check_status("dob_shared_data_tell: bad status from idd_get_sd_type.");
/*
   Copy data into data base. 
*/
  propagate_ok = dob_assign((caddr_t) ((char *) data_object->data + offset),
    (caddr_t) data, data_type, length);
/*
  Only propagate changes if an assignment was actually made.
*/

  if (propagate_ok) {
    dpg_propagate_modify (shared_data, position, component_name);
    check_status("dob_shared_data_tell: bad status from dpg_propagate_modify.");
  
  				/* Get current transaction associated with
  				   mailbox */
    transaction = dtt_transaction_ask (data_object->mailbox);
  
  				/* having a transaction => not dm sd, go ahead
  				   and update data in transaction */
     if (transaction)
  				/* Add change to current transaction */
        {isd_put_shared_data (transaction, data_object->id,
	    data_object->element_name, component_name,
	    data_object->data + offset);
        check_status (
	  "dob_shared_data_tell: bad status from isd_add_shared_data.");
        }
  } 				/* end if (propagate_ok) */
/*
   Return 
*/
  DEBUG2(fprintf(stderr, "dob_shared_data_tell: exiting.\n"););
  return;
}                                         /* end dob_shared_data_tell */

/*--------------------------------------------------------------------------*\
|  Routine: dob_variable_create
|
|  Description: 
|     Creates a variable instance in the the symbol table and the appropriate
|     object base mapping and returns a unique object handle.
\*--------------------------------------------------------------------------*/
dob_data_object dob_variable_create(parent_vc, variable_template)
dob_data_object parent_vc; 
dob_data_object variable_template;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  LIST temp_list;
  cus_sym template_entry, parent_vc_entry;
  static char temp_name [TEMP_NAME_BUF_SIZE];
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "dob_variable_create: call.\n");
    fprintf(stderr, "dob_variable_create: parent vc = %x.\n", parent_vc);
    fprintf(stderr, "dob_variable_create: variable template = %x.\n",
      variable_template););
/*
   Create a new data object record and intialize table type field
*/
  data_object =(data_object_record *)make_node(sizeof(data_object_record));
  rcheck_null(data_object,
    "dob_variable_create: out of memory during make node of data obj rec.\n",
    out_of_memory, dob_null_object);

  data_object->table_type = symbol_table_type;
/*
   Create a new id and insert in data object structure
*/
  data_object->id = iid_create_id();
  rcheck_status("dob_variable_create: bad status from iid_create_id.",
    dob_null_object);
/*
   Add data object to hashtable
*/
  add_to_hashtable(dob_id_table, data_object, data_object->id);
/*
   Create a name based on the symbol type ("var") and the variable name
*/
  template_entry = dob_get_symbol_entry(variable_template);
  rcheck_status("dob_variable_create: bad status from get_get_symbol_entry.",
    dob_null_object);

  (void) strcpy (temp_name, VAR_SYMBOL_PREFIX);
  (void) strncat (temp_name, cus_sym_representation(template_entry),
    ROOM_LEFT_IN_VAR_BUF);

  rcheck_status("dob_variable_create: bad status from cus_sym_represent.",
    dob_null_object);
/*
   Create a variable in the symbol table and insert in data object structure
*/
  data_object->ste = cus_gensym(temp_name, variable_sym);
  rcheck_status("dob_variable_create: bad status from cus_gensym.",
    dob_null_object);
/*
   Put handle of created variable in id property
*/
  cus_put_prop(data_object->ste, id_prop, (char *) data_object);
  rcheck_status("dob_variable_create: bad status put id_prop.",
    dob_null_object);

/*
???  is this correct  to "propagate" undefines in arithmetic/boolean fcns
*/

/*
   Set the intial current type property to undefined
*/
  cus_put_prop(data_object->ste, current_type_prop, (char *) idd_undefined);
  rcheck_status( "dob_variable_create: bad status put current type property.",
    dob_null_object);
/*
   Set the intial current value property to 0
*/
  cus_put_prop(data_object->ste, current_value_prop, 0);
  rcheck_status("dob_variable_create: bad status put current type property.",
    dob_null_object);
/*
   Put parent vc handle in parent vc property of newly created variable instance
*/
  cus_put_prop(data_object->ste, parent_vc_prop, (char *) parent_vc);
  rcheck_status("dob_variable_create: bad status put parent_vc_prop.",
    dob_null_object);
/*
   Put variable template handle in template property of newly created
   variable instance
*/
  cus_put_prop(data_object->ste, template_prop, (char *) variable_template);
  rcheck_status("dob_variable_create: bad status put template prop.",
    dob_null_object);
/*
   Put ask property in variable
*/
  temp_list = make_list();

  cus_put_prop(data_object->ste, ask_prop, (char *) temp_list);
  rcheck_status("dob_variable_create: bad status put ask prop.",
    dob_null_object);
/*
   Add var instance handle to the list of instances defined for the variable
   template. 
*/
  temp_list = (LIST)cus_get_prop(template_entry, instances_prop);
  rcheck_status("dob_variable_create: bad status get subvcs prop.",
    dob_null_object);
 
  add_to_head(temp_list, (char *) data_object);
/*
   Add variable instance handle to the list of variables defined for the
   parent vc
*/
  parent_vc_entry = dob_get_symbol_entry(parent_vc);
  rcheck_status("dob_variable_create: bad status from dob_get_symbol_entry.",
    dob_null_object);

  temp_list = (LIST)cus_get_prop(parent_vc_entry, variables_prop);
  rcheck_status("dob_variable_create: bad status get varibles prop.",
    dob_null_object);
 
  add_to_tail(temp_list, data_object);
/*
   Propogate create for var
*/
  dpg_propagate_create (data_object, parent_vc);
    rcheck_status(
      "dob_variable_create: bad status from dpg_propagate_create.",
      dob_null_object);
  DEBUG2(fprintf(stderr,
	"dob_variable_create: exiting. object = %x\n",data_object););
  return ((dob_data_object) data_object);
}                                         /* end dob_variable_create	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_variable_init
|
|  Description: 
|     Runs the initialization snippet for a specified variable
\*--------------------------------------------------------------------------*/
void dob_variable_init(variable)
dob_data_object variable;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  dob_data_object variable_template;
  cus_sym template_entry;
  dgr_sne *sne;

  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_variable_init: call.\n");
    fprintf(stderr, "dob_variable_init: variable = %x.\n", variable););

/*
   Convert the private data object pointer to something that's easier to use.
*/
  check_object (variable, "dob_variable_init:");
  data_object = (data_object_record *) variable;
  DEBUG2(fprintf (stderr, "dob_variable_init: variable->element_name = %s\n",
	data_object->element_name);
  );
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1(isc_print_status("dob_variable_init:"););
    return;
  }
/*
   Check to make sure variable is in symbol table and not in shared data
*/
  if (data_object->table_type != symbol_table_type) {	/* invalid symbol   */
    set_status(invalid_symbol);
    DEBUG1(isc_print_status("dob_variable_init:"););
    return;
  }					/* end if invalid symbol	    */
/*
   Get the handle of the variable template
*/
  variable_template = (dob_data_object)cus_get_prop(data_object->ste,
    template_prop);
  check_status("dob_variable_init: bad status get template prop.");
/*
   Get the symbol table entry for the template symbol
*/
  template_entry = dob_get_symbol_entry(variable_template);
  check_status("dob_variable_init: bad status from dob_get_symbol_entry.");

  DEBUG2 (fprintf (stderr, "dob_variable_init: var template = %s\n",
	cus_sym_representation (template_entry));)

				/* Get and run snippet for var */
  sne = (dgr_sne *) (make_node (sizeof (dgr_sne)));
  sne-> routine = (dgr_snippet) cus_get_prop(template_entry,
    	initial_value_snippet_prop);
  check_status ("dob_variable_init: bad status get var snippet prop.");
  sne-> parm_1 = (caddr_t) data_object;
  sne-> parm_2 = (caddr_t) dob_null_object;
  dgr_run_snippet (sne);
  check_status ("dob_variable_init: bad status from dgr_run_snippet.");
		/* !! DO NOT FREE THE SNE HERE.  IT WILL BE USED DURING
		      PROPAGATE REFERENCES AND SUBSEQUENT PROPAGATE
		      MODIFIES.  IT IS FREE-ED WHEN THE VARIABLE IS DELETED  */
  cus_put_prop (data_object->ste, saved_sne_prop, sne);
  DEBUG2 (fprintf (stderr, "dob_variable_init: exiting.\n"););
  return;
}                                         /* end dob_variable_init */

/*--------------------------------------------------------------------------*\
|  Routine: dob_temp_release
|
|  Description: 
|     Releases a temp instance and adds it to the free list
\*--------------------------------------------------------------------------*/
void dob_temp_release (temp_object)
dob_data_object temp_object;
{  data_object_record *data_object;
   data_object_record *p;
   int free_list_size;

   set_status (ok);
   DEBUG2 (fprintf (stderr, "dob_temp_release: call, ");
           fprintf (stderr, "temp_object = %x\n", temp_object););

  DEBUG3(fprintf(stderr, "dob_temp_release: d22_temp_free_list = %x.\n", 
      d22_temp_free_list););

/*
   Convert the private data object pointer to something that's easier to use.
*/

   check_object (temp_object, "dob_temp_release:");
   data_object = (data_object_record *) temp_object;
  DEBUG2 (fprintf (stderr, "dob_temp_release: temp->element_name = %s\n",
	data_object->element_name););

				/* ensure that the free list is valid and
				   does not have duplicate entries
				*/
   DEBUG3 (p = (data_object_record *)d22_temp_free_list;
      free_list_size = 0;
      while (p != NULL) {
        if (p->id == data_object->id) {
          FATAL_ERROR (
	    fprintf("dob_temp_release:  temp already on list!!!\n");
	    fprintf("dob_temp_release:  object pointer = %x.\n", data_object);
	  );
        }
        free_list_size = free_list_size + 1;
        p = (data_object_record *)p->data;
      }

     fprintf (stderr, "dob_temp_release:  free_list_size = %d.\n",
      free_list_size););

   data_object->data = d22_temp_free_list;
   d22_temp_free_list = (caddr_t) data_object;

  DEBUG3(fprintf(stderr, "dob_temp_release: d22_temp_free_list = %x.\n", 
      d22_temp_free_list););

   DEBUG2(fprintf (stderr, "dob_temp_release: exit.\n"););

   return;
}				/* end dob_temp_release */

/*--------------------------------------------------------------------------*\
|  Routine: dob_temp_create
|
|  Description: 
|     Creates a temp instance in the the symbol table and the appropriate
|     object base mapping and returns a unique object handle
\*--------------------------------------------------------------------------*/
dob_data_object dob_temp_create()
{                                         /* local type definitions	    */
  data_object_record *data_object;

  set_status(ok);
  DEBUG2 (fprintf(stderr, "dob_temp_create: call.\n"););

  DEBUG3(fprintf(stderr, "dob_temp_create: d22_temp_free_list = %x.\n", 
      d22_temp_free_list););

   if (!d22_temp_free_list) {	/* if we have no cached temps */

     DEBUG3(fprintf(stderr, "dob_temp_create: no cached temps.\n"););
   
				/* Create a new data object record and
				   intialize table type field */
      data_object = (data_object_record *)
	make_node(sizeof(data_object_record));
      rcheck_null (data_object,
         "dob_temp_create: out of memory during make node of data obj rec.\n",
         out_of_memory, dob_null_object);

      data_object->table_type = symbol_table_type;
				/* Create a new id and insert in data object
				   structure */
      data_object->id = iid_create_id();
      rcheck_status ("dob_temp_create: bad status from iid_create_id.",
	dob_null_object);

				/* Add data object to hashtable */
      add_to_hashtable(dob_id_table, data_object, data_object->id);

				/* Create a temp in the symbol table and
				   insert in data object structure */
      data_object->ste = cus_gensym("temp", temp_sym);
      rcheck_status ("dob_temp_create: bad status from cus_gensym.",
	dob_null_object);

				/* Put handle of created temp in id property */
      cus_put_prop(data_object->ste, id_prop, (char *) data_object);
      rcheck_status ("dob_temp_create: bad status put id_prop.",
	dob_null_object);

      }
    else			/* there are guys on the free list, get one */
      {

       DEBUG3(fprintf(stderr, "dob_temp_create: cached temps.\n"););

      data_object = (data_object_record *) d22_temp_free_list;
      d22_temp_free_list = data_object->data;
      }

				/* Set the intial current type property to
				   undefined */
   cus_put_prop (data_object->ste, current_type_prop, (char *) idd_undefined);
   rcheck_status ("dob_temp_create: bad status put current type property.",
    	dob_null_object);

			/* Set the intial current value property to 0*/
   cus_put_prop (data_object->ste, current_value_prop, (char *) 0);
   rcheck_status( "dob_temp_create: bad status put current value property.",
    	dob_null_object);

  DEBUG3(fprintf(stderr, "dob_temp_create: d22_temp_free_list = %x.\n", 
      d22_temp_free_list););

  DEBUG2 (fprintf (stderr, "dob_temp_create: exiting. object = %x\n",
   data_object););

  return ((dob_data_object) data_object);
}                                         /* end dob_temp_create	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_variable_tell
|
|  Description: 
|     Updates the type and constant value properties associated with an
|     existing variable or temp instance in the symbol table if different from
|     existing data.  If the type has changed it is assumed that the
|     value has changed as well.  Variable tell performs no implicit type
|     conversion
\*--------------------------------------------------------------------------*/
void dob_variable_tell(variable, type, value)
dob_data_object variable;
idd_data_types type;
caddr_t value;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  idd_data_types old_type;
  char *old_value;		/* the old value in the object base	    */
  boolean type_changed = false;
  boolean value_changed = false;
  cus_sym_type symbol_type;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */

  DEBUG2 (fprintf(stderr, "dob_variable_tell: call.\n");
    fprintf(stderr, "dob_variable_tell: variable = %x.\n", variable);
    fprintf(
      stderr, "dob_variable_tell: type = %s.\n", idd_get_type_string(type));
    fprintf(stderr, "dob_variable_tell: value = %d.\n", value););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  check_object(variable, "dob_variable_tell:");
  data_object = (data_object_record *) variable;
  DEBUG2(fprintf(stderr, "dob_variable_tell: variable->element_name = %s\n",
	data_object->element_name););
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1(isc_print_status("dob_variable_tell:"););
    return;
  }
/*
   Check to make sure symbol is in symbol table and not in shared data
*/

  if (data_object->table_type != symbol_table_type) {	/* invalid symbol   */
    set_status(invalid_symbol);
    DEBUG1(isc_print_status("dob_variable_tell"););
    return;
  }					/* end if invalid symbol	    */
/*
   Get the type prop from the symbol table
*/
  symbol_type = (cus_sym_type)cus_get_prop(data_object->ste, type_prop);
  check_status("dob_variable_tell: bad status get type_prop.");

/*
   If symbol is not a variable AND not a temp then it is a invalid symbol
*/
  if ((symbol_type != variable_sym) && (symbol_type != temp_sym)) {
					/* not variable nor temp symbol	    */
    set_status(invalid_symbol);
    DEBUG1(isc_print_status("dob_variable_tell:"););
    return;
  }						/* end if invalid symbol    */

/*
   Get the existing current type property value from the symbol table
*/
  old_type = (idd_data_types)cus_get_prop(data_object->ste,current_type_prop);
  check_status("dob_variable_tell: bad status get current_type_prop.");

/*
   Get the existing current value property value from the symbol table
*/
  old_value = (char *)cus_get_prop(data_object->ste, current_value_prop);
  check_status("dob_variable_tell: bad status get current_value_prop.");
  if (old_value == NULL)
/*
   We need to be absolutely sure to update the value in the symbol table
*/
      value_changed = true;
/* 
   If value is undefined, then type should  be also.
*/
  if (dob_undefined (type, value))
    type = idd_undefined;
/*
   If type changed delete deallocate old storage, allocate new storage and
   set new type property
*/
  if (type != old_type) {	/* type changed				    */
/*
   Reallocate the storage associated with the old value
*/
    switch (old_type) {		/* switch on type			    */

/*--------------------------------------------------------------------------*\
|  Boolean, integer, id, undefined
\*--------------------------------------------------------------------------*/
/*
   These types have no associated storage so simply break;
*/
      case idd_boolean:
      case idd_integer:
      case idd_id:
      case idd_undefined:

        break;

/*--------------------------------------------------------------------------*\
|  Real, string
\*--------------------------------------------------------------------------*/
/*
   These types consists of a pointer to a single node
*/
      case idd_real:
      case idd_string:

        free_node(old_value);

        break;

/*--------------------------------------------------------------------------*\
|  Records
\*--------------------------------------------------------------------------*/
/*
   Record variables are not currently supported in Serpent
*/
      case idd_record:

        DEBUG1(fprintf(stderr,
	  "dob_variable_tell: idd_record type invalid.\n"););

        set_status(invalid_data_type);
        return;

/*--------------------------------------------------------------------------*\
|  Buffer
\*--------------------------------------------------------------------------*/
/*
   Destroy the existing buffer
*/
      case idd_buffer:

        ubo_destroy_buffer ((idd_buffer_type *) old_value);

        break;

/*--------------------------------------------------------------------------*\
|  Default
\*--------------------------------------------------------------------------*/
      default:

        DEBUG1(fprintf(stderr, 
            "dob_variable_tell:old_type is invalid data type.\n"););

        set_status(invalid_data_type);
        return;

    }				/* end switch on data type		    */

/*
   Allocate storage for new type
*/
    switch (type) {		/* switch on type			    */

/*--------------------------------------------------------------------------*\
|  Boolean, integer, id, undefined
\*--------------------------------------------------------------------------*/
/*
   These types have no associated storage so simply break;
*/
      case idd_boolean:
      case idd_integer:
      case idd_id:
      case idd_undefined:

        break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/
/*
   Allocate storage for real value
*/
      case idd_real:

        old_value = make_node(sizeof(double));
        check_null(old_value,
          "dob_variable_tell: out of memory during make node of double.\n",
          out_of_memory);

        break;

/*--------------------------------------------------------------------------*\
|  String
\*--------------------------------------------------------------------------*/
/*
   Allocate storage the size of the string data
*/
      case idd_string:

        old_value = make_node(strlen(value)+1);
        check_null(old_value,
          "dob_variable_tell: out of memory during make node of string.\n",
          out_of_memory);

        break;

/*--------------------------------------------------------------------------*\
|  Record
\*--------------------------------------------------------------------------*/
/*
   Records are not currently supported
*/
      case idd_record:

        DEBUG1(fprintf(stderr,
	  "dob_variable_tell: idd_record type invalid.\n"););

        set_status(invalid_data_type);

        return;

/*--------------------------------------------------------------------------*\
|  Buffer
\*--------------------------------------------------------------------------*/
/*
   Allocate storage for the buffer structure only
*/
      case idd_buffer:

        old_value = (char *)ubo_create_buffer();
        check_status("dob_variable_tell: bad status from ubo_create_buff.\n");

        break;

/*--------------------------------------------------------------------------*\
|  Default
\*--------------------------------------------------------------------------*/
      default:
        DEBUG1(fprintf(stderr,
	  "dob_variable_tell: type is invalid data type.\n"););
        set_status(invalid_data_type);
        return;

    }				/* end switch on data type		    */
/*
   Update current type property
*/
    cus_put_prop(data_object->ste, current_type_prop, (char *) type);
    check_status("dob_variable_tell: bad status put current_type_prop.");

    type_changed = true;

  }				/* end if type changed			    */

				/*
				   If the value changed (based on type)
				   copy in the new value and set update
				   value flag.  Note: The
				   container_length parameter is
				   unconditionally 0 here, since
				   variables are never of a fixed
				   length.

				   value_changed may already be set,
				   way at the top of the routine.
				*/
   if (dob_assign ((caddr_t) &old_value, value, type, 0))
     value_changed = true;

/*
   Update current value property if required
*/
   if (value_changed) {		/* update value necessary		    */

     cus_put_prop(data_object->ste, current_value_prop, old_value);

    check_status("dob_variable_tell: bad status put current_value_prop.");

  }				/* end if update value necessary	    */
/*
    Propogate changes if necessary
*/
  if (type_changed || value_changed) { /* propagate changes necessary */

    dpg_propagate_modify (variable, -1, "");
    check_status("dob_variable_tell: bad status dpg_propagate_modify.");
  }				/* end if propagate changes necessary	    */
/*
   Return
*/

  DEBUG2 (fprintf(stderr, "dob_variable_tell: exiting.\n"););
  return;
}                                         /* end dob_variable_tell */

/*--------------------------------------------------------------------------*\
|  Routine: dob_object_create
|
|  Description: 
|     Creates a object instance in the the symbol table and the appropriate
|     object base mapping and returns a unique handle
\*--------------------------------------------------------------------------*/
dob_data_object dob_object_create(parent_vc, object_template)
dob_data_object parent_vc;
dob_data_object object_template;
{                                         /* local type definitions	    */
  dob_data_object object;
  string element_name;
  cus_sym obj_template_entry, parent_vc_entry, symbol_table_entry;
  LIST temp_list;
  ipc_mailbox mailbox;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_object_create: call.\n");
    fprintf(stderr, "dob_object_create: parent vc = %x.\n", parent_vc);
    fprintf(stderr,
      "dob_object_create: object template = %x.\n", object_template););
/*
   Get the symbol table entries for the parent vc and object template ids
*/
  parent_vc_entry = dob_get_symbol_entry(parent_vc);
  rcheck_status("dob_object_create: bad status from dob_get_symbol_entry.",
    dob_null_object);

  obj_template_entry = dob_get_symbol_entry(object_template);
  rcheck_status("dob_object_create: bad status from dob_get_symbol_entry.",
    dob_null_object);
/*
   Get the mailbox and element name from object template properties
*/
  element_name = (string)cus_get_prop(obj_template_entry, object_type_prop);
  rcheck_status("dob_object_create: bad status get object type prop.",
    dob_null_object);

  mailbox = (ipc_mailbox)cus_get_prop(obj_template_entry, mailbox_name_prop);
  rcheck_status("dob_object_create: bad status get mailbox prop.",
    dob_null_object);
/*
   Create shared data object then change the type to an object prop.
*/
  object = dob_slang_obj_create(element_name, mailbox, object_template,
	parent_vc);
  rcheck_status("dob_object_create: bad status from dob_slang_obj_create.",
    dob_null_object);

  symbol_table_entry = dob_get_symbol_entry(object);
  rcheck_status("dob_object_create: bad status from dob_get_symbol_entry.",
    dob_null_object);

  cus_put_prop(symbol_table_entry, type_prop, object_sym);
  rcheck_status(
    "dob_object_create: bad status from cus_put_prop of type_prop.",
    dob_null_object);
/*
   Add object instance handle to the list of object instances defined for the
   object template
*/
  temp_list = (LIST)cus_get_prop(obj_template_entry, instances_prop);
  rcheck_status("dob_object_create: bad status get instances prop.",
    dob_null_object);
 
  add_to_head(temp_list, object);
/*
   Add object instance handle to the list of objects defined for the parent vc
*/
  temp_list = (LIST)cus_get_prop(parent_vc_entry, objects_prop);
  rcheck_status("dob_object_create: bad status get objects prop.",
    dob_null_object);
 
  add_to_tail(temp_list, object);
/*
   Return
*/
  DEBUG2 (fprintf(stderr,
    "dob_object_create: exiting. object = %x\n",object););
  return ((dob_data_object) object);
}                                         /* end dob_object_create */

/*--------------------------------------------------------------------------*\
|  Routine: dob_attribute_init
|
|  Description: 
|     Initializes all the attributes for a given object
\*--------------------------------------------------------------------------*/
void dob_attribute_init(object)
dob_data_object object;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  dob_data_object object_template;
  dgr_sne *sne;
  LIST attribute_list;
  cus_sym attribute_entry; 
  register LIST attribute;
  LIST sne_list;

  set_status(ok);
  DEBUG2 (fprintf(stderr, "dob_attribute_init: call.\n");
    fprintf(stderr, "dob_attribute_init: object = %x.\n", object););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  check_object (object, "dob_attribute_init:");
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_attribute_init:"););
    return;
  }
/*
   Check to make sure object is shared data
*/
  if (data_object->table_type != shared_data_table_type) {
    set_status(invalid_symbol);
    DEBUG1 (isc_print_status("dob_attribute_init:"););
    return;
  }					/* end if invalid symbol	    */
/*
   Get the id of the object template
*/
  object_template = (dob_data_object)cus_get_prop(data_object->ste,
    template_prop);
  check_status("dob_attribute_init: bad status get template prop.");
/*
   Get list of attributes from object template
*/
  attribute_list = (LIST)cus_get_prop(dob_get_symbol_entry (object_template),
    attributes_prop);
  check_status("dob_attribute_init: bad status get attributes prop.");

   sne_list = make_list ();
				/* Run snippets for each attribute */
  if (list_length (attribute_list) > 0)
   LOOP_THROUGH_LIST (attribute_list, attribute) 
    {
/*
??? maybe to make the template attributes_prop list of ids instead of symbol
    attribute_entry = dob_get_symbol_entry (LISTnode (attribute));
*/
    attribute_entry = (cus_sym) (LISTnode (attribute));
    check_status("attribute_init: bad status from get_symbol_entry for att.");

				/* get and run snippet for att */
    sne = (dgr_sne *) (make_node (sizeof (dgr_sne)));
    sne-> routine = (dgr_snippet) cus_get_prop (attribute_entry,
	value_snippet_prop);
    check_status("dob_attribute_init: bad status from get attr snippt prop.");

    sne-> parm_1 = (caddr_t) data_object;
    sne-> parm_2 = (caddr_t) dob_null_object;
    dgr_run_snippet (sne);
    check_status ("dob_attribute_init: bad status from dgr_run_snippet.");
                /* !! DO NOT FREE THE SNE HERE.  IT WILL BE USED DURING
                      PROPAGATE REFERENCES AND SUBSEQUENT PROPAGATE
                      MODIFIES.  IT IS FREE-ED WHEN THE CONTAINING OBJECT IS
		      DELETED.  */
    add_to_head (sne_list, sne);
    }

  cus_put_prop (data_object->ste, saved_sne_prop, sne_list);

  DEBUG2 (fprintf(stderr, "dob_attribute_init: exiting.\n"););
  return;
}                                         /* end dob_attribute_init */

/*--------------------------------------------------------------------------*\
|  Routine: dob_vc_create
|
|  Description: 
|     Creates a vc instance in the the symbol table and the appropriate
|     object base mapping and returns a unique id
\*--------------------------------------------------------------------------*/
dob_data_object dob_vc_create(parent_vc, vc_template, vce, creating_sd)
dob_data_object parent_vc, vc_template, creating_sd;
LIST vce;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  static char temp_name [TEMP_NAME_BUF_SIZE];
  cus_sym template_entry, parent_vc_entry, creating_sd_entry; 
  LIST temp_list, variable_list, object_list;
  dgr_sne *sne;
  register LIST the_var, the_obj;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_vc_create: call.\n");
    fprintf(stderr, "dob_vc_create: parent vc = %x.\n", parent_vc);
    fprintf(stderr, "dob_vc_create: vc template = %x.\n", vc_template);
    fprintf(stderr, "dob_vc_create: creating sd = %x.\n", creating_sd););

/*-------------------- Create VC -------------------------------------------*/

/*
   Create a new data object record and intialize table type field
*/
  data_object = (data_object_record *)make_node(sizeof(data_object_record));
  rcheck_null(data_object,
    "dob_vc_create: out of memory during make_node of data_object.\n",
    out_of_memory, dob_null_object);

  data_object->table_type = symbol_table_type;
/*
   Create a new id and insert in data object structure
*/
  data_object->id = iid_create_id();
  rcheck_status("dob_vc_create: bad status from iid_create_id.",
    dob_null_object);
/*
   Add data object to hashtable
*/
  add_to_hashtable(dob_id_table, data_object, data_object->id);
/*
   Create a name based on the symbol type ("var") and the variable name
*/
  template_entry = dob_get_symbol_entry(vc_template);
  rcheck_status("dob_vc_create: bad status from dob_get_symbol_entry.",
    dob_null_object);

  (void) strcpy (temp_name, VAR_SYMBOL_PREFIX);
  (void) strncat (temp_name, cus_sym_representation(template_entry),
    ROOM_LEFT_IN_VAR_BUF);
  rcheck_status("dob_vc_create: bad status from cus_sym_representation.",
    dob_null_object);
/*
   Create a vc in the symbol table and insert in data object structure
*/
  data_object->ste = cus_gensym(temp_name, vc_sym);
  rcheck_status("dob_vc_create: bad status from cus_gensym.",
    dob_null_object);
/*
   Put handle of created temp in id property
*/
  cus_put_prop(data_object->ste, id_prop, (char *) data_object);
  rcheck_status( "dob_vc_create: bad status put id_prop.",
    dob_null_object);
/*
   Put vce list pointer in vce property of newly created vc instance
*/
  cus_put_prop(data_object->ste, vce_prop, (char *) vce);
  rcheck_status("dob_vc_create: bad status put vce property.",
    dob_null_object);
/*
   Put parent vc id in parent vc property of newly created vc instance
*/
  cus_put_prop(data_object->ste, parent_vc_prop, (char *) parent_vc);
  rcheck_status("dob_vc_create: bad status put parent_vc_prop.",
	dob_null_object);
/*
   Put template vc id in template property of newly created vc instance
*/
  cus_put_prop(data_object->ste, template_prop, (char *) vc_template);
  rcheck_status("dob_vc_create: bad status put template property.",
    dob_null_object);
/*
   Put creating sd handle in creating sd property of newly created vc instance
*/
  cus_put_prop(data_object->ste, creating_sd_prop, (char *) creating_sd);
  rcheck_status( "dob_vc_create: bad status put creating_sd_prop.",
    dob_null_object);
/*
   If a creating sd exists add the created vc to the list of created vcs
   associated with the creating sd
*/
  if (creating_sd != dob_null_object) {	/* ! null creating sd		    */
    creating_sd_entry = dob_get_symbol_entry(creating_sd);
    rcheck_status("dob_vc_create: bad status from dob_get_symbol_entry.",
      dob_null_object);

    temp_list = (LIST)cus_get_prop(creating_sd_entry, created_vcs_prop);
    rcheck_status("dob_vc_create: bad status get instances prop.",
      dob_null_object);
 
    add_to_head(temp_list, data_object);
  }					/* end if ! null creating sd	    */
/*
   Create sub_vc property
*/
  temp_list = make_list();
  rcheck_null(temp_list, "dob_vc_create: out of memory during make_list.\n",
    out_of_memory, dob_null_object);

  cus_put_prop(data_object->ste, sub_vcs_prop, (char *) temp_list);
  rcheck_status( "dob_vc_create: bad status from cus_put_prop.",
    dob_null_object);
/*
   Add vc instance handle to list of vc instanced defined for the vc template
*/
  temp_list = (LIST)cus_get_prop(template_entry, instances_prop);
  rcheck_status("dob_vc_create: bad status get instances prop.",
    dob_null_object);
 
  add_to_head(temp_list, data_object);
/*
   Add vc instance handle to the list of subvcs defined for the parent vc
*/
  if (parent_vc != dob_null_object) {	/* null parent vc		    */
    parent_vc_entry = dob_get_symbol_entry(parent_vc);
    rcheck_status("dob_vc_create: bad status from dob_get_symbol_entry.",
      dob_null_object);

    temp_list = (LIST)cus_get_prop(parent_vc_entry, sub_vcs_prop);
    rcheck_status("dob_vc_create: bad status get subvcs prop.",
      dob_null_object);
 
    add_to_tail(temp_list, data_object);

  }					/* end if parent vc is null	    */

/*-------------------- Create Variables ------------------------------------*/

/*
   Create variables property.  The variables property consists of the lis
   of variable ids associated with the vc
*/
  temp_list = make_list();
  rcheck_null(temp_list, "dob_vc_create: out of memory during make_list.\n",
    out_of_memory, dob_null_object);

  cus_put_prop(data_object->ste, variables_prop, (char *) temp_list);
  rcheck_status("dob_vc_create: bad status from put of object prop.",
    dob_null_object);
/*
   Create all variables defined for vc template.
*/
  variable_list = (LIST)cus_get_prop( template_entry, variables_prop);
  rcheck_status("dob_vc_create: bad status get variables prop.",
    dob_null_object);

  DEBUG3 (fprintf (stderr, "vc_create: variable list\n");
     cud_print_tree (stderr, variable_list, "   ");)

  if (list_length (variable_list) > 0)
    LOOP_THROUGH_LIST (variable_list, the_var)
      dob_variable_create (data_object,
      (dob_data_object) cus_get_prop ((cus_sym) (LISTnode (the_var)), id_prop));

/*-------------------- Create Objects --------------------------------------*/

/*
   Create objects property.  The objects property consists of the list
   of object ids associated with the vc
*/
  temp_list = make_list();
  rcheck_null(temp_list,
    "dob_vc_create: out of memory during make_list for object properties.\n",
    out_of_memory, dob_null_object);

  cus_put_prop(data_object->ste, objects_prop, (char *) temp_list);
  rcheck_status("dob_vc_create: bad status from put of objects_prop.",
    dob_null_object);
/*
   Create all objects defined for vc template.
*/
  object_list = (LIST)cus_get_prop(template_entry, objects_prop);
  rcheck_status("dob_vc_create: bad status get objects prop.",
    dob_null_object);

  if (list_length (object_list) > 0)
    LOOP_THROUGH_LIST ((LIST) object_list, the_obj)
     dob_object_create (data_object,
     (dob_data_object)cus_get_prop ((cus_sym) (LISTnode (the_obj)), id_prop));

/*-------------------- Initialize Variables  -------------------------------*/

  variable_list = (LIST) cus_get_prop (data_object->ste, variables_prop);
  rcheck_status("dob_vc_create: bad status get variables prop.",
    dob_null_object);

  if (list_length (variable_list) > 0)
    LOOP_THROUGH_LIST (variable_list, the_var)
      {dob_variable_init ((iid_id_type) (LISTnode (the_var)));
      }

/*-------------------- Initialize Attributes -------------------------------*/

  object_list = (LIST)cus_get_prop(data_object->ste, objects_prop);
  rcheck_status("dob_vc_create: bad status get objects prop.",
    dob_null_object);

  if (list_length (object_list) > 0)
    LOOP_THROUGH_LIST ((LIST) object_list, the_obj)
        dob_attribute_init ((iid_id_type) (LISTnode (the_obj)));

/*-------------------- Run Creation Actions  -------------------------------*/

  sne = (dgr_sne *) (make_node (sizeof (dgr_sne)));
  sne-> routine = (dgr_snippet) cus_get_prop (template_entry,
      create_actions_snippet_prop);
  rcheck_status ("dob_vc_create: bad status get create actions snippet prop.",
	dob_null_object);
  sne-> parm_1 = (caddr_t) data_object;
  sne-> parm_2 = (caddr_t) dob_null_object;
  dgr_run_snippet (sne);
  rcheck_status ("dob_vc_create: bad status from dgr_run_snippet.",
    	dob_null_object);
				/* do not need the sne anymore */
  free_node (sne);

				/* Propagate create for vc */
  dpg_propagate_create (data_object, parent_vc);
  rcheck_status ("dob_vc_create: bad status from dpg_propagate_create.",
    	dob_null_object);

/*
   Return
*/
  DEBUG2 (fprintf(stderr, "dob_vc_create: exiting. object = %x\n",
	data_object););
  return ((dob_data_object) data_object);
}                                         /* end dob_vc_create		    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_ask
|
|  Description: 
|     Gets data out of the object base.
\*--------------------------------------------------------------------------*/
caddr_t dob_ask(data_object, component_name)
dob_data_object data_object;
string component_name;
{
  return (dob_base_ask(data_object, component_name, TRUE, FALSE));
}

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_data_value
|
|  Description: 
|     Gets data out of the object base and forces reevaluation of buffers,
|     i.e.  return the buffers data instead of the buffer as data.
\*--------------------------------------------------------------------------*/
caddr_t dob_get_data_value(data_object, component_name)
dob_data_object data_object;
string component_name;
{
  return (dob_base_ask(data_object, component_name, TRUE, TRUE));
}

/*--------------------------------------------------------------------------*\
|  Routine: dob_base_ask
|
|  Description: 
|     Gets data out of the object base.
\*--------------------------------------------------------------------------*/
caddr_t dob_base_ask(object, component_name, propagate, eval_buffer)
dob_data_object object;
string component_name;
boolean propagate, eval_buffer;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  data_object_record *linked_data_object;
  caddr_t data;
  cus_sym_type symbol_type;
  int length, offset, position;
  isd_process process;
  idd_data_definition data_definition;
  LIST component_list;
  idd_data_types data_type;
  register LIST component;
  iid_id_type id;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(fprintf(stderr, "dob_base_ask: call.\n");
    fprintf(stderr, "   : object = %#x\n", object);
    fprintf(stderr, "   : component_name = %s\n", component_name);
    fprintf(stderr, "   : propagate = %d\n", propagate););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object(object, "dob_base_ask:", (caddr_t) NULL);
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_base_ask:"););
    return NULL;
  }
/*
   If the data object is in shared data return a pointer to either the
   element or the component if component name is specified
*/
  if (data_object->table_type == shared_data_table_type) { /* shared data   */
/*
   Get the data definition associated with the mailbox for the current
   data object
*/
    process = isd_getfrom_process_table(data_object->mailbox);
    rcheck_status("dob_sd_tell: bad status from idd_getfrom_process_tale.",
      NULL);

    data_definition = isd_get_data_definition(process);
    rcheck_status("dob_shared_data_tell: bad status from idd_get_data_def.",
      NULL);
/*
   If the component name is not specified return the data pointer.
*/
    if (component_name == NULL) {	/* component not specified	    */
      data = data_object->data;
/*
   Propagate reference for each changed component
*/
      component_list = idd_create_component_list(data_definition,
        data_object);
      rcheck_status(
	"dob_base_ask: bad status from idd_create_component_list.", NULL);

      if (propagate)
        LOOP_THROUGH_LIST (component_list, component)
           {idd_get_component_lop (data_definition,
		data_object->element_name, (string) (LISTnode (component)),
		&length, &offset, &position);
           dpg_propagate_reference (data_object, position);
           rcheck_status(
             "dob_base_ask: bad status from propagate_reference 4 null comp.",
	     NULL);
           }

      idd_destroy_component_list(component_list);
      rcheck_status(
	"dob_base_ask: bad status from idd_destroy_component_list.",
        NULL);

    }					/* end if component not specified   */
    else {				/* component specified		    */
/*
   Get length and offset of specified component within element
*/
      idd_get_component_lop(data_definition, data_object->element_name,
        component_name, &length, &offset, &position);
      rcheck_status("dob_base_ask: bad status from idd_get_comp_lop.", NULL);

      data = data_object->data + offset;
/*
   Get component data type.
*/
      data_type = idd_get_shared_data_type(data_definition,
        data_object->element_name, component_name);
/*
   If the data type is buffer, get the actual data type of the buffer.
*/
      if (eval_buffer && (data_type == idd_buffer))
        data_type = ((idd_buffer_type *)data)->type;
/*
   Handle special cases.
*/
      if ((data_type == idd_integer) ||
          (data_type == idd_boolean))
/*
   The data stored in the object base is an integer.  "data" (as
   set above) points to this integer.  We have to de-reference the
   pointer and cast it to integer to get the value, since integers
   are stored as immediate data.
*/
        data = (caddr_t) (* ((int *) data));

      else if (data_type == idd_id) {
/*
   VERY SPECIAL CASE:  We have an ID in the object base.  In order to
   return the actual data, we have to:

   1 - De-reference the shared data pointer to get the ID value.
   2 - Get the object base data item from the hash table using the ID.
*/
	id = (* ((iid_id_type *) data));

/* 
   Now, we have the id.  Check for NULL or UNDEFINED.   Always return 
   dob_null_object in this case.
*/
	if ((id == iid_null_id) || (id == UNDEFINED_ID))
	  data = (caddr_t) dob_null_object;
	else {
	  linked_data_object = (data_object_record *)get_from_hashtable(
	    dob_id_table, id);
	  rcheck_null(linked_data_object,
	    "dob_base_ask: can't get linked data object from hash table.\n",
	    symbol_not_found, (caddr_t) NULL);
/*
   The caller wants the handle of this data object, not the actual
   data.
*/
	  data = (caddr_t) linked_data_object;
	}
      } /* end else data_type == idd_id */

      if (propagate)
        {dpg_propagate_reference(data_object, position);
        rcheck_status(
          "dob_base_ask: bad status from propagate_reference 4 specfic comp.",
          NULL);
        }

    }					/* end else component specified	    */
  }					/* end if in shared data	    */
/*
   If the data is in the symbol table and is a variable or temp return the
   current value, otherwise set status to invalid_symbol.  Only propagate
   reference if it is a variable
*/
  else if (data_object->table_type == symbol_table_type) { /* symbol table  */

    DEBUG3 (fprintf(stderr, "dob_base_ask: table type is symbol table.\n"););

/*
   Get the type prop from the symbol table
*/
    symbol_type = (cus_sym_type)cus_get_prop(data_object->ste, type_prop);
    rcheck_status("dob_base_ask: bad status get type_prop.", NULL);

    if (symbol_type == variable_sym) {		/* variable symbol	    */

      DEBUG3 (fprintf(stderr, "dob_base_ask: symbol type is variable.\n"););

      data = cus_get_prop(data_object->ste, current_value_prop);
      rcheck_status("dob_base_ask: bad status get current_value_prop.", NULL);

      if (propagate)
        {dpg_propagate_reference(data_object, -1);
        rcheck_status(
	  "dob_base_ask: bad status from dpg_propagate_reference 4 sym tab.",
          NULL);
        }

    }						/* end if variable symbol   */

    else if (symbol_type == temp_sym)  {
      DEBUG3 (fprintf(stderr, "dob_base_ask: symbol type is temp.\n"););
					/* variable or temp symbol	    */
      data = cus_get_prop(data_object->ste, current_value_prop);
      DEBUG3 (fprintf(stderr, "dob_base_ask: current value property = %d.\n",
         data););

      rcheck_status("dob_base_ask: bad status get current_value_prop.", NULL);

    }						/* end if temp symbol   */

    else {					/* invalid symbol	    */
      set_status(invalid_symbol);
      DEBUG1(isc_print_status("dob_base_ask:"););
      return NULL;
    }						/* end if invalid symbol    */

  }					/* end else if symbol table	    */

  else {				/* invalid data object		    */
    set_status(invalid_data_object);
    DEBUG1(isc_print_status("dob_base_ask:"););
    return NULL;
  }				/* end else invalid data object		    */
/*
   Return
*/
  DEBUG2 (fprintf(stderr,
    "dob_base_ask: exiting, returning data = %#x\n", data););
  return data;
}					/* end dob_base_ask	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_symbol_type
|
|  Description:  
|     Returns the type of the specified symbol
\*--------------------------------------------------------------------------*/
cus_sym_type dob_get_symbol_type(symbol)
dob_data_object symbol;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  cus_sym_type type_property;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_get_symbol_type: call.");
    fprintf(stderr, "  symbol = %x\n", symbol););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object (symbol, "dob_get_symbol_type:", (cus_sym_type) NULL);
  data_object = (data_object_record *) symbol;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG3 (isc_print_status("dob_get_symbol_type:"););
    return null_sym;
  }
/*
   Get the type prop from the symbol table
*/
  type_property = (cus_sym_type)cus_get_prop(data_object->ste, type_prop);
  rcheck_status("dob_get_symbol_type: bad status get type_prop.",
	type_property);
/*
   Return
*/
  DEBUG2 (fprintf(stderr, "dob_get_symbol_type: exiting. type = %s\n",
	cus_type_print_val (type_property)););
  return type_property;
}                                         /* end dob_get_symbol_type */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_symbol_entry
|
|  Description: 
|     Returns the symbol table entry corresponding to the id or null id
|     one does not exist
\*--------------------------------------------------------------------------*/
cus_sym dob_get_symbol_entry(symbol)
dob_data_object symbol;
{                                         /* local type definitions	    */
  data_object_record *data_object;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_get_symbol_entry: call.");
    fprintf(stderr, "  symbol = %x\n", symbol););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object (symbol, "dob_get_symbol_entry:", (cus_sym) NULL);
  data_object = (data_object_record *) symbol;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_get_symbol_entry:"););
    return NULL;
  }
/*
   Return
*/
  DEBUG2 (fprintf(stderr, "dob_get_symbol_entry: exiting.\n"););
  return data_object->ste;
}                                         /* end dob_get_symbol_entry */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_name
|
|  Description: 
|     Returns the name of the vc, object, variable or shared data element
|     represented by the ID.  This name is the name generated by the
|     compiler.
|
|     This is a pointer directly to the data in the object base.  
|                    ***  DO NOT MUNGE  ***
\*--------------------------------------------------------------------------*/
string dob_get_name(object)
dob_data_object object;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  string symbol_name;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_get_name: call.\n");
    fprintf(stderr, "dob_get_name: object = %x.\n", object););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object (object, "dob_get_name:", NULL);
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_get_name:"););
    return NULL;
  }
/*
   If the data object is in shared data return a pointer to the element name
*/
  if (data_object->table_type == shared_data_table_type) { /* shared data   */
    DEBUG2 (fprintf(stderr, "dob_get_name: symbol name = %s.\n",
        data_object->element_name);
      fprintf(stderr, "dob_get_name: exiting.\n"););
    return data_object->element_name;
  }					/* end if in shared data	    */

  else if (data_object->table_type == symbol_table_type) { /* symbol table  */
    symbol_name = (string)cus_sym_representation(data_object->ste);
    rcheck_status("dob_get_name: bad status get current_value_prop.", NULL);

    DEBUG2 (fprintf(stderr, "dob_get_name: symbol name = %s.\n",
        symbol_name);
      fprintf(stderr, "dob_get_name: exiting.\n"););

    return symbol_name;
  }					/* end else if symbol table	    */

  else {				/* invalid symbol		    */
    set_status(invalid_symbol);
    DEBUG1(isc_print_status("dob_get_name:"););
    return NULL;
  }					/* end else invalid symbol	    */
}					/* end dob_get_name		    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_real_name
|
|  Description: 
|     Returns the name of the vc, object, variable or shared data element
|     represented by the ID.  This name is the real name specified in 
|     the slang dialogue.
\*--------------------------------------------------------------------------*/
string dob_get_real_name(object)
dob_data_object object;
{                                         /* local type definitions	    */
  data_object_record *data_object;
  string real_name;
  dob_data_object template;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(fprintf(stderr, "dob_get_real_name: call.\n");
    fprintf(stderr, "dob_get_real_name: dob_data_object = %x.\n", object););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object (object, "dob_get_real_name:", NULL);
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_get_real_name:"););
    return NULL;
  }
/*
   Get the template handle.
*/
  template = (dob_data_object)cus_get_prop(data_object->ste, template_prop);
  rcheck_status("dob_get_real_name: bad status from get of template_prop.",
    NULL);
/*
   Get the original name property from the template for the data item.
*/
  real_name = (string)cus_get_prop(dob_get_symbol_entry(template),
    original_name_prop);
  rcheck_status(
    "dob_get_real_name: bad status from get of original_name_prop.",
    NULL);
/*
   Return.
*/
  DEBUG2(fprintf(stderr, "dob_get_real_name: name = %s.\n", real_name);
    fprintf(stderr, "dob_get_real_name: exiting.\n"););

  return real_name;

}					/* end dob_get_real_name	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_base_data_type
|
|  Description: 
|     Gets the base type of the specified shared data object.  This means
|     if the data item is actually a buffer this routine returns a buffer.
\*--------------------------------------------------------------------------*/
idd_data_types dob_get_base_data_type(object, component_name)
dob_data_object object;
string component_name;
{                                         /* local type definitions	    */
  idd_data_types data_type;
  data_object_record *data_object;
  cus_sym_type symbol_type;
  isd_process process;
  idd_data_definition data_definition;
/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_get_base_data_type: call\n");
      fprintf(stderr, "object = %x, component_name = %s\n", object,
	component_name););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object (object, "dob_get_base_data_type:", idd_no_data_type);
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_get_base_data_type:"););
    return idd_no_data_type;
  }
/*
   If the data is in shared data return the type of the component identified
   by the second parameter
*/
  if (data_object->table_type == shared_data_table_type) { /* shared data   */
/*
   Query data type of component within shared data element
*/
    DEBUG3 (fprintf (stderr," \tdata object is shared_data_table_type\n"););
    process = isd_getfrom_process_table(data_object->mailbox);
    rcheck_status(
      "dob_get_base_data_type: bad status from idd_getfrom_proc_tale.",
      idd_no_data_type);

    data_definition = isd_get_data_definition(process);
    rcheck_status("dob_get_base_data_type: bad status from idd_get_data_def.",
      idd_no_data_type);

    DEBUG3 (fprintf(stderr, "\tdata definition = %x.\n", data_definition);
      fprintf(stderr, "\tdata_object->element_name = %s.\n",
         data_object->element_name);
      fprintf(stderr, "\tcomponent_name = %s.\n", component_name););

    data_type = idd_get_shared_data_type(data_definition,
      data_object->element_name, component_name);

    if (get_status ())
      {DEBUG1 (fprintf (stderr,
	"dob_get_base_data_type: bad status from idd_get_sd_type,");
        fprintf (stderr, "for element = %s, coponent = %s\n",
	   data_object->element_name, component_name););
      }
  }
/*
   Else if the data is in the symbol table return the type if var or temp
*/
  else if (data_object->table_type == symbol_table_type) { /* symbol table  */
/*
   Determine the type prop of the symbol
*/
    DEBUG3 (fprintf (stderr," \tdata object is symbol_table_type\n"););
    symbol_type = (cus_sym_type)cus_get_prop(data_object->ste, type_prop);
    rcheck_status("dob_get_base_data_type: bad status get type_prop.",
      idd_no_data_type);
/*
   If symbol is a variable or temp return get the data type, otherwise
   set status to invalid symbol and return;
*/
    if ((symbol_type == variable_sym) || (symbol_type == temp_sym)) {
					/* variable or temp symbol	    */
      data_type = (idd_data_types)cus_get_prop(data_object->ste,
        current_type_prop);
      rcheck_status(
        "dob_get_base_data_type: bad status get current_value_prop.",
        idd_no_data_type);
    }
    else {
      DEBUG3(fprintf(stderr,
	    "\tdata object is not variable_sym or temp_sym\n"););
       data_type = idd_id;
    }
  }					/* end else if symbol table	    */
/*
   If the data not in either table return invalid shared data
*/
  else {
    DEBUG3(fprintf(stderr, "\tdata object is unknown type\n"););
    set_status(invalid_shared_data);
    DEBUG1(isc_print_status("dob_get_base_data_type:"););

    return idd_no_data_type;
  }					/* end if invalid symbol	    */
/*
   Return
*/
  DEBUG2(fprintf(stderr, "dob_get_base_data_type: data_type = %s\n.", 
      idd_get_type_string(data_type));
    fprintf(stderr, "dob_get_base_data_type: exiting."););

  return data_type;
}                                         /* end dob_get_base_data_type	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_data_type
|
|  Description: 
|     Gets the data type of the specified shared data object
\*--------------------------------------------------------------------------*/
idd_data_types dob_get_data_type(object, component_name)
dob_data_object object;
string component_name;
{                                         /* local type definitions	    */
  idd_data_types data_type;
  data_object_record *data_object;
  cus_sym_type symbol_type;
  isd_process process;
  idd_data_definition data_definition;
  caddr_t data;
  int length, offset, position;

/*
   Initialization
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_get_data_type: call\n");
      fprintf(stderr, "object = %x, component_name = %s\n", object,
	component_name););
/*
   Convert the private data object pointer to something that's easier to use.
*/
  rcheck_object (object, "dob_get_data_type:", idd_no_data_type);
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_get_data_type:"););
    return idd_no_data_type;
  }
/*
   If the data is in shared data return the type of the component identified
   by the second parameter
*/
  if (data_object->table_type == shared_data_table_type) { /* shared data   */
/*
   Query data type of component within shared data element
*/
    DEBUG3(fprintf(stderr," \tdata object is shared_data_table_type\n"););
    process = isd_getfrom_process_table(data_object->mailbox);
    rcheck_status("dob_get_data_type: bad status from idd_getfrom_proc_tale.",
      idd_no_data_type);

    data_definition = isd_get_data_definition(process);
    rcheck_status("dob_get_data_type: bad status from idd_get_data_def.",
      idd_no_data_type);

    DEBUG3(
      fprintf(stderr, "\tdata definition = %x.\n", data_definition);
      fprintf(stderr, "\tdata_object->element_name = %s.\n",
        data_object->element_name);
      fprintf(stderr, "\tcomponent_name = %s.\n", component_name););

    data_type = idd_get_shared_data_type(data_definition,
      data_object->element_name, component_name);

    if (get_status()) {		/* bad status				*/
      DEBUG1(fprintf(stderr,
          "dob_get_data_type: bad status from idd_get_sd_type,");
        fprintf(stderr, "for element = %s, coponent = %s\n",
	   data_object->element_name, component_name););
    }				/* end if bad status			*/
/*
   If data type is buffer get the actual data type from the actual data.
*/
    if (data_type == idd_buffer) {	/* buffer			*/
/*
   If the component name is not specified return the data pointer.
*/
      if (component_name == NULL)	/* component not specified	    */
        data = data_object->data;
      else {				/* component specified		    */
/*
   Get length and offset of specified component within element
*/
        idd_get_component_lop(data_definition, data_object->element_name,
          component_name, &length, &offset, &position);
        rcheck_status(
          "dob_get_data_type: bad status from idd_get_comp_lop.", 
          idd_no_data_type);

        data = data_object->data + offset;
/*
   Get component data type.
*/
        data_type = ((idd_buffer_type *)data)->type;

      }					/* end else component specified	    */
    }					/* end if buffer		*/
  }				/* end if shared data			*/
/*
   Else if the data is in the symbol table return the type if var or temp
*/
  else if (data_object->table_type == symbol_table_type) { /* symbol table  */
/*
   Determine the type prop of the symbol
*/
    DEBUG3(fprintf(stderr," \tdata object is symbol_table_type\n"););
    symbol_type = (cus_sym_type)cus_get_prop(data_object->ste, type_prop);
    rcheck_status("dob_get_data_type: bad status get type_prop.",
      idd_no_data_type);
/*
   If symbol is a variable or temp return get the data type, otherwise
   assume it is an id?
*/
    if ((symbol_type == variable_sym) || (symbol_type == temp_sym)) {
					/* variable or temp symbol	    */
      data_type = (idd_data_types)cus_get_prop(data_object->ste,
        current_type_prop);
      rcheck_status("dob_get_data_type: bad status get current_value_prop.",
        idd_no_data_type);
/*
   If data type is buffer get the actual data type from the actual data.
*/
      if (data_type == idd_buffer) {	/* buffer			*/
        data = cus_get_prop(data_object->ste, current_value_prop);
        data_type = ((idd_buffer_type *)data)->type;
      }					/* end if buffer		*/
    }					/* end if variable or temp sym	*/
/*
   If not a variable or temp sym assume it's an id?
*/
    else {				/* not a variable or temp sym	*/
      DEBUG3(fprintf(stderr,
	"\tdata object is not variable_sym or temp_sym\n"););
      data_type = idd_id;
    }
  }					/* end else if symbol table	    */
/*
   If the data not in either table return invalid shared data
*/
  else {
    set_status(invalid_shared_data);
    DEBUG1(
      isc_print_status("dob_get_data_type: data object is unknown type.\n"););

    return idd_no_data_type;
  }					/* end if invalid symbol	    */
/*
   Return
*/
  DEBUG2(fprintf(stderr, "dob_get_data_type: data_type = %s.\n", 
      idd_get_type_string(data_type));
    fprintf(stderr, "dob_get_data_type: exiting.\n"););

  return data_type;
}                                         /* end dob_get_data_type	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_delete_buffers
|
|  Description: 
|     Deletes buffer bodies
\*--------------------------------------------------------------------------*/
void dob_delete_buffers(data_definition, data_object, component_name)
idd_data_definition data_definition;
data_object_record *data_object;
string component_name;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  int length, offset, position;
  idd_buffer_type *buffer;

  DEBUG2(fprintf(stderr, "dob_delete_buffers: call.\n");
    fprintf(stderr, "dob_delete_buffers: data definition = %x.\n",
      data_definition);
    fprintf(stderr, "dob_delete_buffers: element change = %x.\n",
      data_object);
    fprintf(stderr, "dob_delete_buffers: component name = %s.\n",
      component_name););

/*
   Get the length, offset and position of the component.
*/
  idd_get_component_lop(data_definition, data_object->element_name,
    component_name, &length, &offset, &position);
  check_status("dob_delete_buffers: status from idd_get_lngthNoff.");
/*
   Get component data type
*/
  data_type = idd_get_shared_data_type(data_definition,
    data_object->element_name, component_name);
  check_status("dob_delete_buffers: status from idd_get_shared_data_type.");
/*
   If component is of buffer type free buffer storage.
*/
  if (data_type == idd_buffer) {	/* buffer type			    */

    buffer = (idd_buffer_type *)((char *)data_object->data + offset);

    DEBUG3 (fprintf(stderr, "dob_delete_buffers: buffer length = %d.\n",
        buffer->length);
      fprintf(stderr, "dob_delete_buffers: buffer body = %s.\n",
        buffer->body););

    if (buffer->body != NULL)		/* buffer exists		    */
      free_node(buffer->body);
  }					/* end if buffer type		    */
/*
   Return
*/
  return;
}					/* end dob_delete_buffers	    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_destroy_data_object
|
|  Description: 
|     Destroys the data object associated with the specified id and free 
|     all associated storage.
\*--------------------------------------------------------------------------*/
void dob_destroy_data_object(object)
dob_data_object object;

{                                         /* local type definitions	    */
  data_object_record *data_object;
  dob_data_object creating_sd;
  isd_process process;
  idd_data_definition data_definition;
  LIST component_list, temp_list, sne_list;
  cus_sym_type symbol_type;
  cus_sym temp_entry;
  dob_data_object parent_vc, template;
  isd_trans transaction;
  register LIST elem;
  dgr_sne *sne;

  set_status(ok);
   DEBUG2(fprintf(stderr, "dob_destroy_data_object: call.\n");
    fprintf (stderr, "dob_destroy_data_object: object = %x.\n", object););

/*
   Convert the private data object pointer to something that's easier to use.
*/
  check_object (object, "dob_destroy_data_object:");
  data_object = (data_object_record *) object;
/*
   If object has been removed set set status to 'removed_data_object' and
   return.
*/
  if (data_object->table_type == removed) {	/* object removed   */
    set_status(removed_data_object);
    DEBUG1 (isc_print_status("dob_destroy_data_object:"););
    return;
  }
/*
   Destroy shared data object
*/
  if (data_object->table_type == shared_data_table_type) { /* shared data   */

    DEBUG3 (fprintf (stderr, "destroying shared data item = %x\n", object););

				/* if sd has attribute snes, delete them */
    if (sne_list = (LIST) cus_get_prop (data_object->ste, saved_sne_prop))
       {loop_through_list (sne_list, sne, dgr_sne *) free_node (sne);
       free_list (sne_list, NOT_NODES);
       }
    else
      set_status (ok); /* status won't be okay if cus_get_prop fails */

/*
   Get the list of components defined for the shared data element.
*/
    process = isd_getfrom_process_table(data_object->mailbox);
    check_status(
	"dob_dest_data_obj: bad status from idd_getfrom_process_table.");
 
    data_definition = isd_get_data_definition(process);
    check_status("dob_dest_data_obj: bad status from idd_get_data_def.");
 
    component_list = idd_create_component_list(data_definition,
      data_object->element_name);
    check_status("dob_dest_data_obj: bad status from idd_create_comp_list.");

    for_all_2list(component_list, data_definition, data_object,
      dob_delete_buffers);

    idd_destroy_component_list(component_list);
    check_status("dob_dest_data_obj: bad status from idd_destroy_comp_list.");
/* 
   Notify the "owner" of destruction 
*/
    transaction = dtt_transaction_ask (data_object-> mailbox);
    check_status ("dob_dest_data_obj: bad status from dtt_transaction_ask");
/* 
   a transaction => not a dm sd, go ahead and place a remove in transaction
*/
    if (transaction) {
      isd_remove_shared_data (transaction, data_object->element_name,
	data_object->id);
      check_status (
	"dob_dest_data_obj: bad status from isd_remove_shared_data.");
    }

    DEBUG3 (fprintf (stderr,
	"dob_dest_data_obj: checking to see if object has parent vc.\n"););

/* 
   if object has a parent_vc_prop, remove the object from the parent's 
   objects_prop 
*/
    parent_vc = (dob_data_object)cus_get_prop(data_object->ste,
	parent_vc_prop);
    if ( dob_validate_object (parent_vc) ) {
      DEBUG3 (fprintf(stderr,
          "dob_dest_data_obj: object has a parent vc, take outa parent\n"););
     temp_entry = dob_get_symbol_entry (parent_vc);
     check_status(
       "dob_dstry_data_obj: bad status from dob_get_sym_entry of parent vc in object");
      temp_list = (LIST)cus_get_prop (temp_entry, objects_prop);
      check_status("dob_destroy_data_obj: bad status get objects prop");
/* 
   Remove object from list of variables for parent vc. 
*/
      delete_list_node (temp_list, data_object);
    }

                               /* now remove the element's instance from the
				  template
                                */
  template = (dob_data_object) cus_get_prop (data_object->ste, template_prop);
			       /*
			  Make sure there's actually a template before
			  attempting to dereference it.  (There may not
			  be; see "dob_slang_obj_create".)
			       */
  if ( dob_validate_object (template) ) 
    delete_list_node (cus_get_prop (((data_object_record *)template)->ste,
	instances_prop), data_object);

/* 
   Destroy the shared data element.  
*/
    free_node(data_object->data);

    dpg_propagate_delete(object);
    check_status (
      "dob_dest_data_obj: bad status from dpg_propagate_delete for sd");

  }					/* end if in shared data	    */

/*
   Else if the data is in the symbol table destroy the symbol and delete
   all references.
*/
  else if (data_object->table_type == symbol_table_type) { /* symbol table  */
/*
   Get the type prop from the symbol table.
*/
    symbol_type = (cus_sym_type)cus_get_prop(data_object->ste, type_prop);
    check_status("dob_destroy_data_object: bad status get type_prop.");

					/* if symbol has a sne, delete it */
    if (sne = (dgr_sne *) cus_get_prop (data_object->ste, saved_sne_prop))
       free_node (sne);

/*
   If symbol is not a variable AND not a temp then it is a invalid symbol.
*/
    switch (symbol_type) {		/* switch on symbol type	    */

/*--------------------------------------------------------------------------*\
|   VC symbol								     
\*--------------------------------------------------------------------------*/

      case vc_sym:
         DEBUG3 (fprintf (stderr, "destroying vc = %x\n", object););
					/*
					   Get symbol table entry, and use it
					   to locate the creating shared data
					   element, if there is one.
					*/
	temp_entry = dob_get_symbol_entry (object);
	check_status 
	  ("dob_destroy_data_object: bad status from dob_get_symbol_entry");
	creating_sd = (dob_data_object) cus_get_prop (temp_entry,
	    creating_sd_prop);
					/*
					   If this VC depends on shared data,
					   but is going away because a variable
					   changed, then we have to remove its
					   identifier from the list of bound
					   VCs attached to the shared data 
					   element.  (The SD is staying around,
					   after all.)
					*/
        if ( (creating_sd != (dob_data_object) NULL) &&
	     (creating_sd != dob_null_object) &&
	     (((data_object_record *) creating_sd) -> table_type != removed) )
	{
					/*
					   This VC is bound to a piece of
					   shared data.  Before deleting the
					   VC, remove it from the list of
					   bound VC instances associated w/
					   the shared data.
					*/
	  temp_entry = dob_get_symbol_entry (creating_sd);
	  check_status
	    ("dob_destroy_data_object: bad status from dob_get_symbol_entry");
	  temp_list = (LIST) cus_get_prop (temp_entry, created_vcs_prop);
	  check_status
	    ("dob_destroy_data_object: bad status from cus_get_prop");
	  if (delete_list_node (temp_list, object) == (NODE) NULL)
	  {
					/* Damn!  It wasn't there! */
	    FATAL_ERROR (fprintf (stderr,
		"dob_destroy_data_object: VC %#0x is not on creating SD's\n",
		object);
	      fprintf (stderr, "created_vcs_prop list.\n"););
	  }
	}

/*
   Get vc template handle from template property of the vc.
*/
        template = (dob_data_object)cus_get_prop(data_object->ste,
	  template_prop);
        check_status("dob_destroy_data_obj: bad status get template_prop.");
/*
   Get symbol table entry handle for vc template.
*/
  	temp_entry = dob_get_symbol_entry(template);
	check_status(
	  "dob_dstry_data_obj: bad status from dob_get_sym_entry.");

					/* Run destroy actions for vc */
        sne = (dgr_sne *) (make_node (sizeof (dgr_sne)));
        sne-> routine =(dgr_snippet)cus_get_prop(temp_entry,
	  destroy_actions_snippet_prop);
        check_status (
	"dob_destroy_data_obj: bad status get destroy actions snippet prop.");
        sne-> parm_1 = (caddr_t) data_object;
        sne-> parm_2 = (caddr_t) dob_null_object;
        dgr_run_snippet (sne);
        check_status ("dob_dstry_data_obj: bad status from destroy snippet.");
                                /* do not need the sne anymore */
        free_node (sne);

/*
   Destroy vce property list
*/
       	temp_list = (LIST)cus_get_prop(data_object->ste, vce_prop);
	check_status("dob_destroy_data_obj: bad status get vce prop.");

        free_list(temp_list, NOT_NODES);
/* 
   Get object property list 
*/
       	temp_list = (LIST)cus_get_prop(data_object->ste, objects_prop);
	check_status("dob_destroy_data_obj: bad status get objects prop.");

        DEBUG3 (fprintf(stderr, "number of vc objects = %d\n", 
            list_length(temp_list)););
/* 
   Destroy all vc's objects 
*/
        LOOP_THROUGH_LIST (temp_list, elem) {
          dob_destroy_data_object ((iid_id_type) (LISTnode (elem)));
	  check_status ("dob_dest_data_obj: bad status destroying vc obj");
	}
/* 
   Free object property list 
*/
        free_list(temp_list, NOT_NODES);
/* 
   Get variables property list 
*/
	temp_list = (LIST)cus_get_prop(data_object->ste, variables_prop);
	check_status("dob_destroy_data_obj: bad status get variables prop.");

        DEBUG3 (fprintf(stderr, "number of vc vars = %d\n",
	  list_length(temp_list)););
/* 
   Destroy all vc's vars 
*/
        LOOP_THROUGH_LIST (temp_list, elem) {
          dob_destroy_data_object((dob_data_object)(LISTnode (elem)));
          if (get_status ()) {
            fprintf (stderr,
	      "dob_dest_data_obj: bad status destroying vc var = %x\n",
              (dob_data_object) (LISTnode (elem)));
          }
        }
/* 
   Destroy variables property list 
*/
        free_list(temp_list, NOT_NODES);
/*
   Get subvcs property list 
*/
	temp_list = (LIST)cus_get_prop(data_object->ste, sub_vcs_prop);
	check_status("dob_destroy_data_obj: bad status get subvcs prop.");

        DEBUG3 (fprintf(stderr, "number of vc subvcs = %d\n", 
            list_length(temp_list)););
/* 
   Destroy all vc's subvcs 
*/
        LOOP_THROUGH_LIST (temp_list, elem) {
          dob_destroy_data_object((dob_data_object) (LISTnode (elem)));
	  check_status ("dob_dest_data_obj: bad status destroying subvc");
	}
/* 
   Destroy subvcs property list 
*/
        free_list(temp_list, NOT_NODES);
/*
   Get parent vc handle from parent vc property of vc.
*/
        parent_vc = (dob_data_object)cus_get_prop(data_object->ste,
          parent_vc_prop);
        check_status("dob_destroy_data_obj: bad status get parent_vc_prop.");
/*
   Get symbol table entry handle for parent vc.
*/
  	temp_entry = dob_get_symbol_entry(parent_vc);
	check_status(
	  "dob_dstry_data_obj: bad status from dob_get_sym_entry.");
/*
   Get the list of subvcs for the parent vc
*/
	temp_list = (LIST)cus_get_prop(temp_entry, sub_vcs_prop);
	check_status("dob_destroy_data_obj: bad status get subvcs prop.");
/*
   Remove vc from list of subvcs for parent vc.
*/
        delete_list_node(temp_list, data_object);
/*
   Get vc template handle from template property of the vc.
*/
        template = (dob_data_object)cus_get_prop(data_object->ste,
          template_prop);
        check_status("dob_destroy_data_obj: bad status get template_prop.");
/*
   Get symbol table entry handle for vc template.
*/
  	temp_entry = dob_get_symbol_entry(template);
	check_status(
	  "dob_dstry_data_obj: bad status from dob_get_sym_entry.");
/*
   Get the list of instances for the vc template.
*/
	temp_list = (LIST)cus_get_prop(temp_entry, instances_prop);
	check_status("dob_destroy_data_obj: bad status get instances prop.");
/*
   Remove vc from list of instances for vc template.
*/
        delete_list_node(temp_list, data_object);

	dpg_propagate_delete (object);
        check_status (
	   "dob_dest_data_obj: bad status from dpg_propagate_delete for vc");
  
        break;

/*--------------------------------------------------------------------------*\
|   Variable symbol							     
\*--------------------------------------------------------------------------*/

      case variable_sym:
      DEBUG3 (fprintf (stderr, "destroying var = %x\n", object););

/*
   Get parent vc handle from parent vc property of variable.
*/
        parent_vc = (dob_data_object)cus_get_prop(data_object->ste,
          parent_vc_prop);
        check_status("dob_destroy_data_obj: bad status get parent_vc_prop.");
/*
   Get symbol table entry handle for parent vc.
*/
  	temp_entry = dob_get_symbol_entry(parent_vc);
	check_status(
	  "dob_dstry_data_obj: bad status from dob_get_sym_entry.");
/*
   Get the list of variables for the parent vc
*/
	temp_list = (LIST)cus_get_prop(temp_entry, variables_prop);
	check_status("dob_destroy_data_obj: bad status get variables prop.");
/*
   Remove variable from list of variables for parent vc.
*/
        delete_list_node(temp_list, data_object);
/*
   Get variable template handle from template property of variable.
*/
        template = (dob_data_object)cus_get_prop(data_object->ste,
          template_prop);
        check_status("dob_destroy_data_obj: bad status get template_prop.");
/*
   Get symbol table entry handle for variable template.
*/
  	temp_entry = dob_get_symbol_entry(template);
	check_status(
	  "dob_dstry_data_obj: bad status from dob_get_sym_entry.");
/*
   Get the list of instances for the variable template
*/
	temp_list = (LIST)cus_get_prop(temp_entry, instances_prop);
	check_status("dob_destroy_data_obj: bad status get instances prop.");
/*
   Remove variable from list of instances for variable template.
*/
        delete_list_node(temp_list, data_object);
       
	dpg_propagate_delete (object);
        check_status (
	   "dob_dest_data_obj: bad status from dpg_propagate_delete for var");

        break;

/*--------------------------------------------------------------------------*\
|   Temp symbol								     
\*--------------------------------------------------------------------------*/

      case temp_sym:
      DEBUG3 (fprintf (stderr, "destroying temp = %x\n", object););

/*
   This case doesn't require any special processing, just break switch so we
   don't cause an error condition to occur.
*/
        break;

/*--------------------------------------------------------------------------*\
|   Default case							     
\*--------------------------------------------------------------------------*/

      default:
        set_status(invalid_symbol);
        DEBUG1(isc_print_status("dob_destroy_data_object:"););
        return;
    }					/* switch on symbol type	    */
  }					/* end else if symbol table	    */
/*
   If the data not in either table return invalid shared data.
*/
  else {
    set_status(invalid_shared_data);
    DEBUG1(isc_print_status("dob_destroy_data_object:"););
    return;
  }					/* end if invalid symbol	    */
/* 
   Instead of deleteing the object from the object base we now mark
   it as being deleted, so that we can distinguish between references
   to deleted elements and those which have never existed.
*/
#if 0
  data_object = (data_object_record *)delete_from_hashtable(dob_id_table, 
    object);
  check_null(data_object,
   "dob_get_symbol_type:bad status from delete_from_hashtable of data obj.\n",
    not_found);
#endif

  data_object->table_type = removed;
/*
   Since both symbols and shared data have symbol table entries, delete
   the symbol table entry.
*/
  if (cus_delete_sym(data_object->ste) == false) {	/* !deleted */
    set_status(symbol_not_found);
    DEBUG1(isc_print_status("dob_destroy_data_object:"););
    return;
  }						/* end if not deleted	    */
/* 
   Return.  
*/
  DEBUG2 (fprintf(stderr, "dob_destroy_data_object: exiting.\n"););

  return;
}                                         /* end dob_destroy_data_object    */

/*--------------------------------------------------------------------------*\
|  Routine: dob_cleanup
|
|  Description: 
|     Performs necessary cleanup of the object base manager
\*--------------------------------------------------------------------------*/
void dob_cleanup()
{                                         /* local type defcleanupions	    */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2 (fprintf(stderr, "dob_cleanup: call.\n"););
/*
   Destroy the id hash table.
*/
/*
?? should free nodes when freeing hashtable, but it is not "working"
*/
  free_hashtable(dob_id_table, AND_NODES);
/*
   Return.
*/
  DEBUG2 ( fprintf(stderr, "dob_cleanup: exiting.\n"););
  return;
}                                         /* end dob_cleanup		    */
