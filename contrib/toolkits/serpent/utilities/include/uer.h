/* $Header: /u/04c/mnt/integ/utilities/include/RCS/uer.h,v 1.12 89/11/02 17:53:20 rcs Exp $ */
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
$Log:	uer.h,v $
 * Revision 1.12  89/11/02  17:53:20  rcs
 * fixed problem with null uer_message
 * 
 * Revision 1.11  89/10/05  17:18:37  rcs
 * made mods to support typed buffers
 * 
 * Revision 1.10  89/08/31  19:04:08  bmc
 * Revised definition of uer_error_msg.
 * 
 * Revision 1.9  89/08/31  14:30:14  bmc
 * Added definitions for uer_error_msg and UER_ERROR_BUF_LEN.
 * 
 * Revision 1.8  89/05/22  11:16:52  bmc
 * Changed to accomodate modifications to DM routines.
 * 
 * Revision 1.7  89/02/21  23:18:21  rcs
 * fixed macros and error messages
 * 
 * Revision 1.6  89/01/30  14:34:58  rcs
 * added error msg constants
 * modified uer_check_status_unop to print debug info
 * modified uer_rcheck_status_unop to print debug info
 * ditto remainder of macros
 * added uer_rcheck_status_da_id, uer_rcheck_status_da_id_str,
 *   uer_rcheck_status_da_id_str_val macros
 * added uer_report_da_id_error routine header
 * added uer_report_da_id_str_error routine header
 * added uer_report_da_id_str_val_error routine header
 * 
 * Revision 1.5  89/01/05  11:36:55  rcs
 * no changes.
 * 
 * Revision 1.3  88/12/19  17:33:42  rcs
 * added run time error message constants
 * 
 * Revision 1.2  88/12/02  15:24:53  rcs
 * you don't want to know
 * 
 * Revision 1.1  88/12/02  13:32:30  rcs
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

#ifndef uer
#define uer
/*--------------------------------------------------------------------------*\
|  Includes: uer
|
|  Description:
|     The following includes are required by the uer component.
|
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include "global.h"             /* Global type defs and constants           */
#include "signal.h"             /* C Signal Stuff                           */
#include "iid.h"                /* id abstraction                         */
#include "idd.h"                /* Data definitions                         */

/*
\*--------------------------------------------------------------------------*/

#define UER_ERROR_BUF_LEN  (256)

/*--------------------------------------------------------------------------*\
|  Global Variable: uer_error_msg
|
|  Description:
|     Buffer for error messages passed to the uer_report functions.
|     Buffer is guaranteed to be at least UER_ERROR_BUF_LEN bytes long.
|
\* Definition:                                                              */

#ifdef uer_main
  char uer_msg_buf [UER_ERROR_BUF_LEN];
  char *uer_error_msg = uer_msg_buf;
#else
  extern char *uer_error_msg;
#endif

/*--------------------------------------------------------------------------*\
|  Global Variable: uer_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef uer_main
  int uer_debug = 1;
#else
  extern int uer_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constants: run time warnings
|
|  Description:
|     The following constants define run time warning messages to the end
|     user.
|
\* Definition:                                                              */

#define UER_DELETED_OBJECT "Warning:  Illegal reference.\n\
A reference was made to a deleted  object in the shared database.\n\
	Object ID = %d.		Element Name = %s.\n"

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constants: run time error messages
|
|  Description:
|     The following constants define run time error messages to the end
|     user.
|
\* Definition:                                                              */

#define UER_PROCESSING_ERROR "Error:  %s.\n\
Occured during processing of the '%s' operation.\n"

#define UER_ILLEGAL_RECORD_EQUATE "Error:  illegal comparison.\n\
Record types can not be compared using the '=' operator.\n"

#define UER_UNDEFINED_COMPARISON "Error:  illegal comparison.\n\
Operands are of incompatible type for comparison using the '%s' operator.\n"

#define UER_ILLEGAL_BUFF_COMPARISON "Error:  illegal comparison.\n\
Buffers can not be compared using the '%s' operator.\n"

#define UER_ILLEGAL_UNDEF_COMPARISON "Error:  illegal comparison.\n\
One or more operands are undefined in '%s' operator.\n"

#define UER_ILLEGAL_ASSIGNMENT "Error:  illegal assignment.\n\
The value of the right hand side (rhs) expression can not be converted\n\
to the type of the left hand side (lhs) identifier.\n"

#define UER_RECORD_UNDEFINED "Error:  illegal assignment.\n\
Record types can not be assigned a value of UNDEFINED.\n"

#define UER_ILLEGAL_ARITH_CONVERSION "Error:  illegal arithmetic operation.\n\
Operands are of incompatible type for '%s' operation.\n"

#define UER_ILLEGAL_TEST "Error:  illegal boolean test.\n\
Boolean expression has illegal type.\n"

#define UER_UNDEFINED_TEST "Error:  illegal boolean test.\n\
Boolean expression can not be undefined.\n"

#define UER_OP_UNDEF_TYPES "Error:  illegal arithmetic operation.\n\
Operands are of illegal type for '%s' operation.\n"

#define UER_INVALID_SYMBOL "Error: invalid declaration type.\n\
Declaration type of operand is invalid in '%s' operation.\n"

#define UER_NOT_FOUND "Error: not found.\n\
Data item not found in '%s' operation.\n"

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_check_status_unop
|
|  Description:
|     The uer_check_status_unop macro is used to check the status in a unary
|     operation and record errors.  The uer_check_status macro assumes
|     that the local variable op1 is defined and set to an appropriate value.
|
\* Syntax:                                                                  */

#define uer_check_status_unop(string, operand)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR, isc_get_status_string(),operand);\
    uer_report_unary_op_error(uer_error_msg, op1);\
    return;\
  }

/*
|  Parameters:
|     op1 - errnoneous ADI to be passed as part of error notification.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_rcheck_status_unop
|
|  Description:
|     The uer_rcheck_status_unop macro is used to check status in unary
|     operations,  record errors, and return an appropriate value.  The 
|     uer_check_status macro assumes that the local variables op1_type, 
|     op2_type, op1_data, op2_data are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_rcheck_status_unop(string, operand, return_value)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR, isc_get_status_string(), operand);\
    uer_report_unary_op_error(uer_error_msg, op1);\
    return(return_value);\
  }

/*
|  Parameters:
|     return_value - Value to return to calling routine.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_check_status_binop
|
|  Description:
|     The uer_check_status_binop macro is used to check the status in a binary
|     operation and record errors.  The uer_check_status macro assumes
|     that the local variables op1_type, op2_type, op1_data, and op2_data
|     are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_check_status_binop(string, operand)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR,\
             isc_get_status_string(), operand);\
    uer_report_binary_op_error(uer_error_msg, op1, op2);\
    return;\
  }

/*
|  Parameters:
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_check_null_binop
|
|  Description:
|     The uer_check_null_binop macro is used to check the status in a binary
|     operation and record errors.  The uer_check_status macro assumes
|     that the local variables op1_type, op2_type, op1_data, and op2_data
|     are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_check_null_binop(result, string, status, operand)\
  if (result == NULL) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    set_status(status);\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR,\
             isc_get_status_string(), operand);\
    uer_report_binary_op_error(uer_error_msg, op1, op2);\
    return;\
  }

/*
|  Parameters:
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_rcheck_status_binop
|
|  Description:
|     The uer_rcheck_status_binop macro is used to check status in binary
|     operations,  record errors, and return an appropriate value.  The 
|     uer_check_status macro assumes that the local variables op1_type, 
|     op2_type, op1_data, op2_data are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_rcheck_status_binop(string, operand, return_value)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR, isc_get_status_string(), operand);\
    uer_report_binary_op_error(uer_error_msg, op1, op2);\
    return(return_value);\
  }

/*
|  Parameters:
|     return_value - Value to return to calling routine.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_check_status_da_id
|
|  Description:
|     The uer_check_status_da_id macro is used to check the status in a binary
|     operation and record errors.  The uer_check_status macro assumes
|     that the local variables op1_type, op2_type, op1_data, and op2_data
|     are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_check_status_da_id(string, operand)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR,\
             isc_get_status_string(), operand);\
    uer_report_da_id_error(uer_error_msg, id_parm);\
    return;\
  }

/*
|  Parameters:
|     string - Name of the routine for trace facility.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_rcheck_status_da_id
|
|  Description:
|     The uer_rcheck_status_da_id macro is used to check status in binary
|     operations,  record errors, and return an appropriate value.  The 
|     uer_check_status macro assumes that the local variables op1_type, 
|     op2_type, op1_data, op2_data are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_rcheck_status_da_id(string, operand, return_value)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR, isc_get_status_string(), operand);\
    uer_report_da_id_error(uer_error_msg, id_parm);\
    return(return_value);\
  }

/*
|  Parameters:
|     string - Name of the routine for trace facility.
|     return_value - Value to return to calling routine.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_check_status_da_id_str
|
|  Description:
|     The uer_check_status_da_id_str macro is used to check the status in a binary
|     operation and record errors.  The uer_check_status macro assumes
|     that the local variables op1_type, op2_type, op1_data, op2_data
|     are defined and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_check_status_da_id_str(string, operand)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR,\
             isc_get_status_string(), operand);\
    uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);\
    return;\
  }

/*
|  Parameters:
|     string - Name of the routine for trace facility.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_rcheck_status_da_id_str
|
|  Description:
|     The uer_rcheck_status_da_id_str macro is used to check status in 
|     access operations,  record errors, and return an appropriate value.  
|     The uer_checuer_rcheck_status_da_id_str macro assumes that 
|     local variables id_parm, and name_parm are all set 
|     appropriately. 
|
\* Syntax:                                                                  */

#define uer_rcheck_status_da_id_str(string, operand, return_value)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR, isc_get_status_string(), operand);\
    uer_report_da_id_str_error(uer_error_msg, id_parm, name_parm);\
    return(return_value);\
  }

/*
|  Parameters:
|     string - Name of the routine for trace facility.
|     return_value - Value to return to calling routine.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_check_status_da_id_str_val
|
|  Description:
|     The uer_check_status_da_id_str_val macro is used to check status of access 
|     operations and record errors.  The uer_check_status macro assumes
|     that the local variables id_parm, and name_parm are defined 
|     and set to appropriate values.
|
\* Syntax:                                                                  */

#define uer_check_status_da_id_str_val(string, operand)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR,\
             isc_get_status_string(), operand);\
    uer_report_da_id_str_val_error(uer_error_msg, id_parm, name_parm, value);\
    return;\
  }

/*
|  Parameters:
|     string - Name of the routine for trace facility.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macro: uer_rcheck_status_da_id_str_val
|
|  Description:
|     The uer_rcheck_status_da_id_str_val macro is used to check status in 
|     access operations,  record errors, and return an appropriate value.  
|     The uer_checuer_rcheck_status_da_id_str_val macro assumes that 
|     local variables id_parm, and name_parm are all set 
|     appropriately. 
|
\* Syntax:                                                                  */

#define uer_rcheck_status_da_id_str_val(string, operand, return_value)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    sprintf(uer_error_msg, UER_PROCESSING_ERROR, isc_get_status_string(), operand);\
    uer_report_da_id_str_val_error(uer_error_msg, id_parm, name_parm, value);\
    return(return_value);\
  }

/*
|  Parameters:
|     string - Name of the routine for trace facility.
|     return_value - Value to return to calling routine.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_da_id_error
|
|  Description:
|     The uer_report_da_id_error can be used to report an error during
|     a data access (dra) function which accepts a single id as a parameter.
|     Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
|
\* Syntax:                                                                  */

	void uer_report_da_id_error(
	  /* error_msg	: in string */
	  /* id		: in dro_atomic_data_item */
	);


/*
|  Parameters:
|     error_msg - String to print on error condition.
|     id - Value of id with which data access routine was called.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_da_id_str_error
|
|  Description:
|     The uer_report_da_id_str_error can be used to report an error during
|     a data access (dra) function which accepts an id and a string as
|     parameters. Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
|
\* Syntax:                                                                  */

	void uer_report_da_id_str_error(
	  /* error_msg	: in string */
	  /* id		: in dro_atomic_data_item */
	  /* name	: in dro_atomic_data_item */
	);


/*
|  Parameters:
|     error_msg - String to print on error condition.
|     id - Value of id with which data access routine was called.
|     name - Value of the string with which data access routine was called.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_da_id_str_val_error
|
|  Description:
|     The uer_report_da_id_str_val_error can be used to report an error during
|     a data access (dra) function which accepts an id, a string and a value
|     as parameters. Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
|
\* Syntax:                                                                  */

	void uer_report_da_id_str_val_error(
	  /* error_msg	: in string */
	  /* id		: in dro_atomic_data_item */
	  /* name	: in dro_atomic_data_item */
	  /* value	: in dro_atomic_data_item */
	);


/*
|  Parameters:
|     error_msg - String to print on error condition.
|     id - Value of id with which data access routine was called.
|     name - Value of the string with which data access routine was called.
|     value - Value of the value with which data access routine was called.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_unary_op_error
|
|  Description:
|     The uer_report_unary_op_error can be used to report an error during
|     a unary operation.  Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
|
\* Syntax:                                                                  */

	void uer_report_unary_op_error(
	  /* error_msg	: in string		*/
	  /* op1_type	: in idd_data_types	*/
	  /* op1_value	: in caddr_t		*/
	);

/*
|  Parameters:
|     error_msg - Description of error.
|     op1_type - Type of the lhs operator.
|     op1_value - Value of the lhs operator.
|     
|  Returns:
|     Void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_report_binary_op_error
|
|  Description:
|     The uer_report_binary_op_error can be used to report an error during
|     a binary operation.  Error information is stored in static variables
|     and a signal is generated for an application supplied error handler
|     to print the information.
|
\* Syntax:                                                                  */

	void uer_report_binary_op_error(
	  /* error_msg	: in string				*/
	  /* op1      	: in dro_atomic_data_item		*/
  	  /* op2      	: in dro_atomic_data_item		*/
	);

/*
|  Parameters:
|     error_msg - Description of error.
|     op1 - The lhs operator.
|     op2 - The rhs operator.
|     
|  Returns:
|     Void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_get_nontemp_symbol
|
|  Description:
|     Returns the id of the first non temp symbol in a reference chain.
|
\* Syntax:                                                                  */

	dob_data_object uer_get_nontemp_symbol(
	  /* object: in dob_data_object */
	);

/*
|  Parameters:
|     object - handle of the first in chain.
|
|  Returns:
|     The handle of the first nontemp symbol.
|
|  Statuses:
|     ok, invalid_data_object.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: uer_print_error
|
|  Description:
|     The uer_print_error routine determines the type of the error and
|     prints apporpriately formate information out to stderr.
|
\* Syntax:                                                                  */

	void uer_print_error();

/*
|  Parameters:
|     none.
|
|  Returns:
|     void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/
#endif
