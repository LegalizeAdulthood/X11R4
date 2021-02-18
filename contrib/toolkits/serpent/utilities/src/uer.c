static char rcsid [] = "$Header: /u/04c/mnt/integ/utilities/src/RCS/uer.c,v 1.9 89/08/31 19:03:30 bmc Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  Error recovery utility (uer)
|
|  Description:  
|     The error recovery utility provides a means of recording, retrieving
|     and printing error information.
|
|  Component of:
|     Utilities Layer.
|
|  Related files:
|     uer.h - contains the external definition for the error recovery utility.
|     uer.c - contains the implementation for the error recovery utility.
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
|     Dec. 2, 1988 : Module created.
$Log:	uer.c,v $
 * Revision 1.9  89/08/31  19:03:30  bmc
 * Moved definition of uer_error_message to uer.h where it belongs.
 * 
 * Revision 1.8  89/08/31  14:31:42  bmc
 * Added definition of uer_error_msg.
 * 
 * Revision 1.7  89/05/22  11:17:38  bmc
 * Changed to accomodate modifications to DM routines.
 * 
 * Revision 1.6  89/02/21  23:17:35  rcs
 * improved the error printing for data access functions
 * 
 * Revision 1.5  89/02/01  21:56:10  rcs
 * modified uer_print_error so it works in some cases
 * 
 * Revision 1.4  89/01/30  14:32:30  rcs
 * added uer_report_da_id_error routine
 * added uer_report_da_id_str_error routine
 * added uer_report_da_id_str_val_error routine
 * modified uer_report_unary_op_error to work with adi's
 * modified uer_report_binary_op_error to work with adi's
 * modified uer_print_error to support most of the above changes
 * 
 * Revision 1.3  88/12/19  17:32:27  rcs
 * added unary ops code, among other things
 * 
 * Revision 1.2  88/12/02  15:22:27  rcs
 * you don't want to know
 * 
 * Revision 1.1  88/12/02  13:30:29  rcs
 * Initial revision
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


/*-------------------- Includes --------------------------------------------*/

#define uer_main
#include "dob.h"		/* object base routines			    */
#include "uer.h"                /* status code                              */

#include "dro.h"                /* Run time operations                      */
#include "dra.h"                /* data access routines                     */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(uer_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(uer_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(uer_debug, statement)

#define BINOP_ERROR 0
#define UNOP_ERROR 1
#define DA_ID_ERROR 2
#define DA_ID_STR_ERROR 3
#define DA_ID_STR_VAL_ERROR 4

/*-------------------- Type Definitions ------------------------------------*/

typedef struct {
  string error_msg;
  dro_atomic_data_item op1, op2, id, name, value;
} uer_error_struct;

/*-------------------- Retained Data ---------------------------------------*/

static int uer_error_type;		/* error type			    */
static uer_error_struct uer_error_data;	/* error information		    */

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_da_id_error
|
|  Description:
|     The uer_report_da_id_error can be used to report an error during
|     a data access (dra) function which accepts a single id as a parameter. 
|     Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
\*--------------------------------------------------------------------------*/
void uer_report_da_id_error(error_msg, id)
string error_msg;
dro_atomic_data_item id;
{                                         /* local type definitions         */
  int rc;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_da_id_error: call.\n");
    fprintf(stderr, "uer_report_da_id_error: error_msg = %s.\n", error_msg);
    fprintf(stderr, "uer_report_da_id_error: id.dob_handle = %#x.\n",
	 id.dob_handle);
    fprintf(
      stderr, 
      "uer_report_da_id_error: id.component_name = %s.\n",
      id.component_name
    );
  );
/*
   Set error type.
*/
  uer_error_type = DA_ID_ERROR;
/*
   Record error information.
*/
  uer_error_data.error_msg = error_msg;
  uer_error_data.id = id;
/*
   Generate an error signal (SIGUSR1)
*/
  rc = kill(getpid(), SIGUSR1);
  check_error(rc, "uer_report_da_id_error", system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_da_id_error: exiting.\n");
  )
  return;
}                                         /* end uer_report_da_id_error    */

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_da_id_str_error
|
|  Description:
|     The uer_report_da_id_str_error can be used to report an error during
|     a data access (dra) function which accepts an id and a string as 
|     parameters. Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
\*--------------------------------------------------------------------------*/
void uer_report_da_id_str_error(error_msg, id, name)
string error_msg;
dro_atomic_data_item id;
dro_atomic_data_item name;
{                                         /* local type definitions         */
  int rc;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_da_id_str_err: call.\n");
    fprintf(stderr, "uer_report_da_id_str_err: error_msg = %s.\n", error_msg);
    fprintf(stderr, "uer_report_da_id_str_error: id.dob_handle = %#x.\n",
	 id.dob_handle);
    fprintf(
      stderr, 
      "uer_report_da_id_str_error: id.component_name = %d.\n", 
      id.component_name
    );
    fprintf(
      stderr, 
      "uer_report_da_id_str_error: name.dob_handle = %#x.\n", 
      name.dob_handle
    );
    fprintf(
      stderr, 
      "uer_report_da_id_str_erroror: name.component_name = %s.\n", 
      name.component_name
    );
  );
/*
   Set error type.
*/
  uer_error_type = DA_ID_STR_ERROR;
/*
   Record error information.
*/
  uer_error_data.error_msg = error_msg;
  uer_error_data.id = id;
  uer_error_data.name = name;
/*
   Generate an error signal (SIGUSR1)
*/
  rc = kill(getpid(), SIGUSR1);
  check_error(rc, "uer_report_da_id_str_error", system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_da_id_str_error: exiting.\n");
  )
  return;

}                                         /* end uer_report_da_id_str_error */

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_da_id_str_val_error
|
|  Description:
|     The uer_report_da_id_str_val_error can be used to report an error during
|     a data access (dra) function which accepts an id, a string and a value 
|     as parameters. Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
\*--------------------------------------------------------------------------*/
void uer_report_da_id_str_val_error(error_msg, id, name, value)
string error_msg;
dro_atomic_data_item id;
dro_atomic_data_item name;
dro_atomic_data_item value;
{                                         /* local type definitions         */
  int rc;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_da_id_str_val_error: call.\n");
    fprintf(
      stderr, 
      "uer_report_da_id_str_val_error: error_msg = %s.\n", 
      error_msg
    );
    fprintf(stderr, "uer_report_da_id_str_val_err: id.dob_handle = %d.\n", id.dob_handle);
    fprintf(
      stderr, 
      "uer_report_da_id_str_val_err: id.component_name = %d.\n", 
      id.component_name
    );
    fprintf(
      stderr, 
      "uer_report_da_id_str_val_error: name.dob_handle = %d.\n", 
      name.dob_handle
    );
    fprintf(
      stderr, 
      "uer_report_da_id_str_val_error: name.component_name = %s.\n", 
      name.component_name
    );
    fprintf(
      stderr, 
      "uer_report_da_id_str_val_error: value.dob_handle = %d.\n", 
      value.dob_handle
    );
    fprintf(
      stderr, 
      "uer_report_da_id_str_val_error: value.component_name = %d.\n", 
      value.component_name
    );
  );
/*
   Set error type.
*/
  uer_error_type = DA_ID_STR_VAL_ERROR;
/*
   Record error information.
*/
  uer_error_data.error_msg = error_msg;
  uer_error_data.id = id;
  uer_error_data.name = name;
  uer_error_data.value = value;
/*
   Generate an error signal (SIGUSR1)
*/
  rc = kill(getpid(), SIGUSR1);
  check_error(rc, "uer_report_da_id_str_val_error", system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_da_id_str_val_error: exiting.\n");
  )
  return;

}                                     /* end uer_report_da_id_str_val_error */

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_unary_op_error
|
|  Description:
|     The uer_report_unary_op_error can be used to report an error during
|     a binary operation.  Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
\*--------------------------------------------------------------------------*/
void uer_report_unary_op_error(error_msg, op1)
string error_msg;
dro_atomic_data_item op1;
{                                         /* local type definitions         */
  int rc;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_unary_op_err: call.\n");
    fprintf(stderr, "uer_report_unary_op_err: error_msg = %s.\n", error_msg);
    fprintf(stderr, "uer_report_unary_op_err: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(
      stderr, 
      "uer_report_unary_op_err: op1.component_name = %d.\n", 
      op1.component_name
    );
  );
/*
   Set error type.
*/
  uer_error_type = UNOP_ERROR;
/*
   Record error information.
*/
  uer_error_data.error_msg = error_msg;
  uer_error_data.op1 = op1;
/*
   Generate an error signal (SIGUSR1)
*/
  rc = kill(getpid(), SIGUSR1);
  check_error(rc, "uer_report_unary_op_err", system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_unary_op_err: exiting.\n");
  )
  return;

}                                         /* end uer_report_unary_op_err    */

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_binary_op_error
|
|  Description:
|     The uer_report_binary_op_error can be used to report an error during
|     a binary operation.  Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
\*--------------------------------------------------------------------------*/
void uer_report_binary_op_error(error_msg, op1, op2)
string error_msg;
dro_atomic_data_item op1, op2;
{                                         /* local type definitions         */
  int rc;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_binop_err: call.\n");
    fprintf(stderr, "uer_report_binop_err: error_msg = %s.\n", error_msg);
    fprintf(stderr, "uer_report_binop_err: op1 = %x.\n", op1);
    fprintf(stderr, "uer_report_binop_err: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(
      stderr, 
      "uer_report_binop_err: op1.component_name = %s.\n",
      op1.component_name
    );
    fprintf(stderr, "uer_report_binop_err: op2 = %x.\n", op2);
    fprintf(stderr, "uer_report_binop_err: op2.dob_handle = %d.\n", op2.dob_handle);
    fprintf(
      stderr, 
      "uer_report_binop_err: op2.component_name = %s.\n", 
      op2.component_name
    );
  );
/*
   Set error type.
*/
  uer_error_type = BINOP_ERROR;
/*
   Record error information.
*/
  uer_error_data.error_msg = error_msg;
  uer_error_data.op1 = op1;
  uer_error_data.op2 = op2;
/*
   Generate an error signal (SIGUSR1)
*/
  rc = kill(getpid(), SIGUSR1);
  check_error(rc, "uer_report_binop_err", system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_report_binop_err: exiting.\n");
  )
  return;

}                                         /* end uer_report_binop_err       */

/*--------------------------------------------------------------------------*\
|  Routine: uer_print_value
|
|  Description:
|     The uer_print_value routine determines the type of the error and
|     prints appropriately formated information out to stderr.
\*--------------------------------------------------------------------------*/
void uer_print_value(op, type)
dro_atomic_data_item op;
idd_data_types type;
{                                         /* local type definitions         */
  int temp_integer;
  double temp_double;
  string temp_string;
  boolean is_defined;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_print_value: call.\n");
    fprintf(stderr, "uer_print_value: op.dob_handle = %d.\n", op.dob_handle);
    fprintf(
      stderr, 
      "uer_print_value: op.component_name = %s.\n",
      op.component_name
    );
    fprintf(stderr, "uer_print_value: type = %d.\n", type);
  );
/*
   If operand is undefined print out string "undefined"; otherwise
   print out the value.  If the dro_undefined operation fails 
   the assumption is made that the value is unknown and uer_print_value
   returns without printing a value.
*/
  is_defined = dro_undefined(op);
  check_status("uer_print_value: bad status from dro_undefined");

  if (is_defined) {			/* operand undefined		    */
    fprintf(
      stderr, 
      "value = UNDEFINED.\n"
    );
  }					/* end if operand undefind	    */
  else {				/* operand defined		    */
    switch (type) {
      case idd_boolean:
      case idd_integer:
      case idd_id:

        temp_integer = (int)dob_ask(op.dob_handle, op.component_name);
        if (!get_status()) {		/* operation successful		    */
          fprintf(
            stderr, 
            "value = %d.\n", 
            temp_integer
          );
	}				/* end if operation successful	    */
        else {				/* op failed			    */
          fprintf(stderr, "value unknown.\n");
        }					/* end else op failed	    */

        break;

      case idd_real:

        temp_double = *((double *)dob_ask(op.dob_handle, op.component_name));
        if (!get_status()) {		/* operation successful		    */
          fprintf(
            stderr, 
            "value = %f.\n", 
            temp_double
          );
	}				/* end if operation successful	    */
        else {				/* op failed			    */
          fprintf(stderr, "value unknown.\n");
        }					/* end else op failed	    */

        break;

      case idd_string:
      case idd_buffer:      /* case idd_buffer should really use print buff */

        temp_string = (string)dob_ask(op.dob_handle, op.component_name);
        if (!get_status()) {		/* operation successful		    */
          fprintf(
            stderr, 
            "value = %s.\n", 
            temp_string
          );
	}				/* end if operation successful	    */
        else {				/* op failed			    */
          fprintf(stderr, "value unknown.\n");
        }					/* end else op failed	    */

        break;

     default:
        break;
    }			     		/* end switch on data type	    */
  }					/* end else operand defined	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_print_value: exiting.\n");
  )
  return;

}                                         /* end uer_print_value 	    */


/*--------------------------------------------------------------------------*\
|  Routine: uer_get_nontemp_symbol
|
|  Description:
|     Returns the id of the first non temp symbol in a reference chain.
\*--------------------------------------------------------------------------*/
dob_data_object uer_get_nontemp_symbol(object)
dob_data_object object;
{                                         /* local type definitions         */
  cus_sym_type symbol_type;
  idd_data_types data_type;
/*
   Initialization.
*/
  set_status(ok);

  DEBUG2(
    fprintf(stderr, "uer_get_nontemp_symbol: call.\n");
    fprintf(stderr, "uer_get_nontemp_symbol: object = %#x.\n", object);
  );

  symbol_type = dob_get_symbol_type(object);
  rcheck_status(
    "uer_get_nontemp_symbol: bad status from dob_get_symbol_type",
     dob_null_object
  );
/*
   Find a non-temp symbol.
*/
  while (symbol_type == temp_sym) {    /* symbol type is a temp            */

    data_type = dob_get_data_type(object, NULL);
    rcheck_status(
      "uer_get_nontemp_symbol: bad status from dob_ask",
       dob_null_object
    );

    DEBUG3(
      fprintf(stderr, "uer_get_nontemp_symbol: data_type = %d.\n", data_type);
    );

    if (data_type != idd_id) {
      break;
    }

    DEBUG3(
      fprintf(stderr, "uer_get_nontemp_symbol: object = %#x.\n", object);
    );

    object = (dob_data_object)dob_ask(object, NULL);
    rcheck_status(
      "uer_get_nontemp_symbol: bad status from dob_ask",
      dob_null_object
    );

    symbol_type = dob_get_symbol_type(object);
    rcheck_status(
      "uer_get_nontemp_symbol: bad status from dob_get_symbol_type",
       dob_null_object
    );

  }                            /* end while symbol type is a temp          */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_get_nontemp_symbol: object = %#x.\n", object);
    fprintf(stderr, "uer_get_nontemp_symbol: exiting.\n");
  )

  return (object);

}                                         /* end uer_get_nontemp_symbol	    */

/*--------------------------------------------------------------------------*\
|  Routine: uer_print_error
|
|  Description:
|     The uer_print_error routine determines the type of the error and
|     prints apporpriately formate information out to stderr.
\*--------------------------------------------------------------------------*/
void uer_print_error()
{                                         /* local type definitions         */
  int rc;
  idd_data_types temp_type;
  string temp_name;
  caddr_t temp_value;
  boolean is_defined;
  string temp_string;
  cus_sym_type temp_symbol_type;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "uer_print_error: call.\n");
  );
/*
   Print the error information associated with the error.
*/
  switch (uer_error_type) {	/* switch on error type			    */

    case BINOP_ERROR:

       DEBUG3(
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.op1.dob_handle = %d.\n",
            uer_error_data.op1.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.op1.component_name = %s.\n",
            uer_error_data.op1.component_name
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.op2.dob_handle = %d.\n",
            uer_error_data.op2.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.op2.component_name = %s.\n",
            uer_error_data.op2.component_name
          );
       );
/*
   Print error message.
*/
      fprintf(stderr, "%s", uer_error_data.error_msg);

/*-------------------  LHS Operand -----------------------------------------*/
/*
   Print the name if available.  
*/
      temp_name = dob_get_real_name(uer_error_data.op1.dob_handle);
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "lhs: name = %s", 
          temp_name
        );
/*
   Print out the component name if there is one.
*/
        if (uer_error_data.op1.component_name != NULL) {
          fprintf(stderr, ".%s\t", uer_error_data.op1.component_name);
	}
        else {
          fprintf(stderr, "\t");
	}
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "lhs: name unknown\t");
      }					/* end else op failed		    */
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.op1.dob_handle, 
        uer_error_data.op1.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "type = %s\t", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "type unknown\t");
      }					/* end else op failed		    */
/*
   Print the value if available.
*/
      uer_print_value(uer_error_data.op1, temp_type);

/*-------------------  RHS Operand -----------------------------------------*/
/*
   Print the name if available.  
*/
      temp_name = dob_get_real_name(uer_error_data.op2.dob_handle);
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "rhs: name = %s", 
          temp_name
        );
/*
   Print out the component name if there is one.
*/
        if (uer_error_data.op2.component_name != NULL) {
          fprintf(stderr, ".%s\t", uer_error_data.op2.component_name);
	}
        else {
          fprintf(stderr, "\t");
	}
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "rhs: name unknown\t");
      }					/* end else op failed		    */
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.op2.dob_handle, 
        uer_error_data.op2.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "type = %s\t", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "type unknown\t");
      }					/* end else op failed		    */
/*
   Print the value if available.
*/
      uer_print_value(uer_error_data.op2, temp_type);

      break;

/*---------------------------- UNOP_ERROR ----------------------------------*/

    case UNOP_ERROR:

       DEBUG3(
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.op1.dob_handle = %d.\n",
            uer_error_data.op1.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.op1.component_name = %s.\n",
            uer_error_data.op1.component_name
          );
       );
/*
   Print error message.
*/
      fprintf(stderr, "%s", uer_error_data.error_msg);
/*
   Print the name if available.  
*/
      temp_name = dob_get_real_name(uer_error_data.op1.dob_handle);
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "lhs: name = %s", 
          temp_name
        );
/*
   Print out the component name if there is one.
*/
        if (uer_error_data.op1.component_name != NULL) {
          fprintf(stderr, ".%s\t", uer_error_data.op1.component_name);
	}
        else {
          fprintf(stderr, "\t");
	}
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "lhs: name unknown\t");
      }					/* end else op failed		    */
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.op1.dob_handle, 
        uer_error_data.op1.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "type = %s\t", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "type unknown\t");
      }					/* end else op failed		    */
/*
   Print the value if available.
*/
      uer_print_value(uer_error_data.op1, temp_type);

      break;

/*---------------------------- DA_ID_ERROR ---------------------------------*/

    case DA_ID_ERROR:

       DEBUG3(
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.id.dob_handle = %d.\n",
            uer_error_data.id.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.id.component_name = %s.\n",
            uer_error_data.id.component_name
          );
       );
/*
   Print error message.
*/
      fprintf(stderr, "%s", uer_error_data.error_msg);
/*
   Get the first nontemp id.
*/
      if ((uer_error_data.id.dob_handle = uer_get_nontemp_symbol(
             uer_error_data.id.dob_handle
          )) == dob_null_object) 
      {
        DEBUG1(
          fprintf(stderr, "uer_print_error: can't find non-temp symbol.\n");
        );
      }
/*
   Print the name if available.  
*/
      temp_name = dob_get_real_name(uer_error_data.id.dob_handle);
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "operand: name = %s", 
          temp_name
        );
/*
   Print out the component name if there is one.
*/
        if (uer_error_data.id.component_name != NULL) {
          fprintf(stderr, ".%s\t", uer_error_data.id.component_name);
	}
        else {
          fprintf(stderr, "\t");
	}
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "operand: name unknown\t");
      }					/* end else op failed		    */
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.id.dob_handle, 
        uer_error_data.id.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "type = %s.\n", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "type unknown.\n");
      }					/* end else op failed		    */
/*
   If type is id, print out the name and type of the data item being
   referenced, if available.
*/
      if (temp_type == idd_id) {	/* id type			    */
/*
   The symbol could either be a variable containing an id or an actual id.  
   If the symbol type is a variable print out the name and type of 
   the thing being referenced.
*/
        temp_symbol_type = dob_get_symbol_type(uer_error_data.id.dob_handle);
        check_status("uer_print_error: bad status from dob_get_symbol_type");

        if (temp_symbol_type == variable_sym) {	/* variable symbol	    */
/*
   Get the actual id.
*/
          uer_error_data.id.dob_handle = (dob_data_object)dob_ask(
            uer_error_data.id.dob_handle, 
            NULL
          );
          check_status("uer_print_error: bad status from dob_ask");
/*
   Print the name of the referenced item if available.  
*/
          temp_name = dob_get_real_name(uer_error_data.id.dob_handle);
          if (!get_status()) {		/* operation successful		    */
            fprintf(
              stderr, 
              "referenced item: name = %s", 
              temp_name
            );
/*
   Print out the component name of the referenced item if there is one.
*/
            if (uer_error_data.id.component_name != NULL) {
              fprintf(stderr, ".%s\t", uer_error_data.id.component_name);
            }
            else {
              fprintf(stderr, "\t");
            }
          }				/* end if operation successful	    */
          else {			/* op failed			    */
            fprintf(stderr, "referenced item: name unknown\t");
          }				/* end else op failed		    */
/*
   Print the type of the referenced item, if available.
*/
          temp_type = dob_get_data_type(
            uer_error_data.id.dob_handle, 
            uer_error_data.id.component_name
          );
          if (!get_status()) {		/* operation successful		    */
             fprintf(
              stderr, 
              "type = %s.\n", 
              idd_get_type_string(temp_type)
             );
          }				/* end if operation successful	    */
          else {			/* op failed			    */
            fprintf(stderr, "type unknown.\n");
          }				/* end else op failed		    */
        }				/* end if variable symbol	    */

      }					/* end if id type		    */

      break;

    case DA_ID_STR_ERROR:

       DEBUG3(
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.id.dob_handle = %d.\n",
            uer_error_data.id.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.id.component_name = %s.\n",
            uer_error_data.id.component_name
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.name.dob_handle = %d.\n",
            uer_error_data.name.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.name.component_name = %s.\n",
            uer_error_data.name.component_name
          );
       );
/*
   Print error message.
*/
      fprintf(stderr, "%s", uer_error_data.error_msg);

/*-------------- ID Parameter ----------------------------------------------*/

/*
   Get the first nontemp id.
*/
      if ((uer_error_data.id.dob_handle = uer_get_nontemp_symbol(
             uer_error_data.id.dob_handle
          )) == dob_null_object) 
      {
        DEBUG1(
          fprintf(stderr, "uer_print_error: can't find non-temp symbol.\n");
        );
      }
/*
   Print the name if available.  
*/
      temp_name = dob_get_real_name(uer_error_data.id.dob_handle);
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "operand: name = %s", 
          temp_name
        );
/*
   Print out the component name if there is one.
*/
        if (uer_error_data.id.component_name != NULL) {
          fprintf(stderr, ".%s\t", uer_error_data.id.component_name);
	}
        else {
          fprintf(stderr, "\t");
	}
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "operand: name unknown\t");
      }					/* end else op failed		    */
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.id.dob_handle, 
        uer_error_data.id.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "type = %s.\n", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "type unknown.\n");
      }					/* end else op failed		    */
/*
   If type is id, print out the name and type of the data item being
   referenced, if available.
*/
      if (temp_type == idd_id) {	/* id type			    */
/*
   The symbol could either be a variable containing an id or an actual id.  
   If the symbol type is a variable print out the name and type of 
   the thing being referenced.
*/
        temp_symbol_type = dob_get_symbol_type(uer_error_data.id.dob_handle);
        check_status("uer_print_error: bad status from dob_get_symbol_type");

        if (temp_symbol_type == variable_sym) {	/* variable symbol	    */
/*
   Get the actual id.
*/
          uer_error_data.id.dob_handle = (dob_data_object)dob_ask(
            uer_error_data.id.dob_handle, 
            NULL
          );
          check_status("uer_print_error: bad status from dob_ask");
/*
   Print the name of the referenced item if available.  
*/
          temp_name = dob_get_real_name(uer_error_data.id.dob_handle);
          if (!get_status()) {		/* operation successful		    */
            fprintf(
              stderr, 
              "referenced item: name = %s", 
              temp_name
            );
/*
   Print out the component name of the referenced item if there is one.
*/
            if (uer_error_data.id.component_name != NULL) {
              fprintf(stderr, ".%s\t", uer_error_data.id.component_name);
            }
            else {
              fprintf(stderr, "\t");
            }
          }				/* end if operation successful	    */
          else {			/* op failed			    */
            fprintf(stderr, "referenced item: name unknown\t");
          }				/* end else op failed		    */
/*
   Print the type of the referenced item, if available.
*/
          temp_type = dob_get_data_type(
            uer_error_data.id.dob_handle, 
            uer_error_data.id.component_name
          );
          if (!get_status()) {		/* operation successful		    */
             fprintf(
              stderr, 
              "type = %s.\n", 
              idd_get_type_string(temp_type)
             );
          }				/* end if operation successful	    */
          else {			/* op failed			    */
            fprintf(stderr, "type unknown.\n");
          }				/* end else op failed		    */
        }				/* end if variable symbol	    */

      }					/* end if id type		    */

/*------------------- Name Parameter ---------------------------------------*/

/*
   Get the first nontemp id.
*/
      if ((uer_error_data.name.dob_handle = uer_get_nontemp_symbol(
             uer_error_data.name.dob_handle
          )) == dob_null_object) 
      {
        DEBUG1(
          fprintf(stderr, "uer_print_error: can't find non-temp symbol.\n");
        );
      }
/*
   Get the type.
*/
      temp_type = dob_get_data_type(
        uer_error_data.name.dob_handle, 
        uer_error_data.name.component_name
      );
      check_status("uer_print_error: bad status from dob_get_data_type");
/*
   Print the value if available.
*/
      is_defined = dro_undefined(uer_error_data.name);
      check_status("uer_print_error: bad status from dro_undefined");

      if (is_defined) {              /* operand undefined            */
        fprintf(
          stderr,
          "Data item name is UNDEFINED.\n"
        );
      }                                 /* end if operand undefind          */
      else {				/* operand defined		    */
        if (temp_type == idd_string) {	/* name is of type string	    */

          temp_string = (string)dob_ask(
            uer_error_data.name.dob_handle, 
            uer_error_data.name.component_name
          );
          if (!get_status()) {          /* operation successful             */
            fprintf(
              stderr,
              "Data item name is '%s'.\n",
              temp_string
            );
          }                             /* end if operation successful      */
          else {                        /* op failed                        */
            fprintf(stderr, "Data item name unknown.\n");
	  }                                     /* end else op failed       */

	}				/* end if name is of type string    */
        else {				/* name not of type string	    */
          fprintf(
            stderr,
            "Data item name is not a string.\n"
          );
	}				/* end else name not of type string */
      }					/* end else operand defined	    */

      break;

    case DA_ID_STR_VAL_ERROR:

       DEBUG3(
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.id.dob_handle = %d.\n",
            uer_error_data.id.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.id.component_name = %s.\n",
            uer_error_data.id.component_name
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.name.dob_handle = %d.\n",
            uer_error_data.name.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.name.component_name = %s.\n",
            uer_error_data.name.component_name
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.value.dob_handle = %d.\n",
            uer_error_data.value.dob_handle
          );
          fprintf(
            stderr, 
            "uer_print_error: uer_error_data.value.component_name = %s.\n",
            uer_error_data.value.component_name
          );
       );
/*
   Print error message.
*/
      fprintf(stderr, "%s", uer_error_data.error_msg);

/*-------------- ID Parameter ----------------------------------------------*/

/*
   Get the first nontemp id.
*/
      if ((uer_error_data.id.dob_handle = uer_get_nontemp_symbol(
             uer_error_data.id.dob_handle
          )) == dob_null_object) 
      {
        DEBUG1(
          fprintf(stderr, "uer_print_error: can't find non-temp symbol.\n");
        );
      }
/*
   Print the name if available.  
*/
      temp_name = dob_get_real_name(uer_error_data.id.dob_handle);
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "operand: name = %s", 
          temp_name
        );
/*
   Print out the component name if there is one.
*/
        if (uer_error_data.id.component_name != NULL) {
          fprintf(stderr, ".%s\t", uer_error_data.id.component_name);
	}
        else {
          fprintf(stderr, "\t");
	}
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "operand: name unknown\t");
      }					/* end else op failed		    */
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.id.dob_handle, 
        uer_error_data.id.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "type = %s.\n", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "type unknown.\n");
      }					/* end else op failed		    */
/*
   If type is id, print out the name and type of the data item being
   referenced, if available.
*/
      if (temp_type == idd_id) {	/* id type			    */
/*
   The symbol could either be a variable containing an id or an actual id.  
   If the symbol type is a variable print out the name and type of 
   the thing being referenced.
*/
        temp_symbol_type = dob_get_symbol_type(uer_error_data.id.dob_handle);
        check_status("uer_print_error: bad status from dob_get_symbol_type");

        if (temp_symbol_type == variable_sym) {	/* variable symbol	    */
/*
   Get the actual id.
*/
          uer_error_data.id.dob_handle = (dob_data_object)dob_ask(
            uer_error_data.id.dob_handle, 
            NULL
          );
          check_status("uer_print_error: bad status from dob_ask");
/*
   Print the name of the referenced item if available.  
*/
          temp_name = dob_get_real_name(uer_error_data.id.dob_handle);
          if (!get_status()) {		/* operation successful		    */
            fprintf(
              stderr, 
              "referenced item: name = %s", 
              temp_name
            );
/*
   Print out the component name of the referenced item if there is one.
*/
            if (uer_error_data.id.component_name != NULL) {
              fprintf(stderr, ".%s\t", uer_error_data.id.component_name);
            }
            else {
              fprintf(stderr, "\t");
            }
          }				/* end if operation successful	    */
          else {			/* op failed			    */
            fprintf(stderr, "referenced item: name unknown\t");
          }				/* end else op failed		    */
/*
   Print the type of the referenced item, if available.
*/
          temp_type = dob_get_data_type(
            uer_error_data.id.dob_handle, 
            uer_error_data.id.component_name
          );
          if (!get_status()) {		/* operation successful		    */
             fprintf(
              stderr, 
              "type = %s.\n", 
              idd_get_type_string(temp_type)
             );
          }				/* end if operation successful	    */
          else {			/* op failed			    */
            fprintf(stderr, "type unknown.\n");
          }				/* end else op failed		    */
        }				/* end if variable symbol	    */

      }					/* end if id type		    */

/*------------------- Name Parameter ---------------------------------------*/

/*
   Get the first nontemp id.
*/
      if ((uer_error_data.name.dob_handle = uer_get_nontemp_symbol(
             uer_error_data.name.dob_handle
          )) == dob_null_object) 
      {
        DEBUG1(
          fprintf(stderr, "uer_print_error: can't find non-temp symbol.\n");
        );
      }
/*
   Get the type.
*/
      temp_type = dob_get_data_type(
        uer_error_data.name.dob_handle, 
        uer_error_data.name.component_name
      );
      check_status("uer_print_error: bad status from dob_get_data_type");
/*
   Print the value if available.
*/
      is_defined = dro_undefined(uer_error_data.name);
      check_status("uer_print_error: bad status from dro_undefined");

      if (is_defined) {              /* operand undefined            */
        fprintf(
          stderr,
          "Data item name is UNDEFINED.\n"
        );
      }                                 /* end if operand undefind          */
      else {				/* operand defined		    */
        if (temp_type == idd_string) {	/* name is of type string	    */

          temp_string = (string)dob_ask(
            uer_error_data.name.dob_handle, 
            uer_error_data.name.component_name
          );
          if (!get_status()) {          /* operation successful             */
            fprintf(
              stderr,
              "Specified name: %s.\n",
              temp_string
            );
          }                             /* end if operation successful      */
          else {                        /* op failed                        */
            fprintf(stderr, "Data item name unknown.\n");
	  }                                     /* end else op failed       */

	}				/* end if name is of type string    */
        else {				/* name not of type string	    */
          fprintf(
            stderr,
            "Data item name is not a string.\n"
          );
	}				/* end else name not of type string */
      }					/* end else operand defined	    */

/*------------------- Value Parameter --------------------------------------*/

/*
   Get the first nontemp id.
*/
      if ((uer_error_data.value.dob_handle = uer_get_nontemp_symbol(
             uer_error_data.value.dob_handle
          )) == dob_null_object) 
      {
        DEBUG1(
          fprintf(stderr, "uer_print_error: can't find non-temp symbol.\n");
        );
      }
/*
   Print the type, if available.
*/
      temp_type = dob_get_data_type(
        uer_error_data.value.dob_handle, 
        uer_error_data.value.component_name
      );
      if (!get_status()) {		/* operation successful		    */
        fprintf(
          stderr, 
          "Specified value:  type = %s\t", 
          idd_get_type_string(temp_type)
        );
      }					/* end if operation successful	    */
      else {				/* op failed			    */
        fprintf(stderr, "Specified value:  type unknown\t");
      }					/* end else op failed		    */
/*
   Print the value if available.
*/
      uer_print_value(uer_error_data.value, temp_type);

      break;

  }				/* end switch on error type		    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "uer_print_error: exiting.\n");
  )
  return;

}                                         /* end uer_print_error       */
