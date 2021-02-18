/*      $Header: /u/04c/mnt/integ/int/include/RCS/isc.h,v 1.17 89/10/05 17:21:10 rcs Exp $ */

/*--------------------------------------------------------------------------*\
| 
|  Name:  Status Code Abstraction (isc)
|
|  Description:  
|     The status code abstraction provides a repository for status code
|     information.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     isc.h - contains the external definition for the status code 
|	      abstraction.
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
|  Owner:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     Oct. 19, 1987 : Module created.
|     $Log:	isc.h,v $
 * Revision 1.17  89/10/05  17:21:10  rcs
 * new error type
 * 
 * Revision 1.16  89/02/13  12:10:18  ejh
 * added copyright notice
 * 
 * Revision 1.15  89/02/10  11:00:43  rcs
 * added FATAL_ERROR macro but then commented it out
 * since reed imbeds it in each of his files
 * 
 * Revision 1.14  89/01/04  17:01:08  rcs
 * added removed_data_object status code
 * 
 * Revision 1.13  88/12/21  14:16:27  rcs
 * Modified status codes to have contiguous numbers
 * 
 * Revision 1.12  88/12/19  17:38:03  rcs
 * added check_status_nr macro
 * added isc_get_status_string external definition
 * 
 * Revision 1.11  88/11/02  14:16:22  rcs
 * moved debug variable from c file - rCs
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

#ifndef isc
#define isc

/*--------------------------------------------------------------------------*\
|  Includes: ipb
| 
|  Description:  
|     The following is a list of all the necessary includes for the ipb
|     abstraction.
| 
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include "global.h"             /* Global type defs and constants           */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: isc_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef isc_main
  int isc_debug = 1;
#else
  extern int isc_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: set_status
|
|  Description:
|     The set status routine sets status to a specified value.
|
\* Syntax:                                                                  */

#define set_status(val) isc_status=val

/*
|  Parameters:
|     val - New status value.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: get_status
|
|  Description:
|     Returns the current system status.
|
\* Syntax:                                                                  */

#define get_status() isc_status

/*
|  Parameters:
|     None.
|
|  Returns:
|     The current status.
|
|  Statuses:
|     none.
|
\*--------------------------------------------------------------------------*/

#define error_val -1

/*--------------------------------------------------------------------------*\
|  Name: Serpent Debug
|
|  Description:
|     The Serpent debug macro is used as part of a debug/trace facility.
|     The macros check to see if a certain level of debug is enabled.
|     If so, they execute a single statement or sequence of statements
|     supplied by the caller and then flush stderr.
|
\* Syntax:                                                                  */

#ifndef NDEBUG

#define SERPENT_DEBUG1(flag, statement)\
  if(flag > 0) {\
    statement;\
    fprintf (stderr, "      file %s, line %d\n", __FILE__, __LINE__);\
    fflush(stderr);\
  }

#define SERPENT_DEBUG2(flag, statement)\
  if(flag > 1) {\
    statement;\
    fflush(stderr);\
  }

#define SERPENT_DEBUG3(flag, statement)\
  if(flag > 2) {\
    statement;\
    fprintf (stderr, "      file %s, line %d\n", __FILE__, __LINE__);\
    fflush(stderr);\
  }

#else

#define SERPENT_DEBUG1(flag, statement)
#define SERPENT_DEBUG2(flag, statement)
#define SERPENT_DEBUG3(flag, statement)

#endif

/*
|  Parameters:
|     flag - The specific debug variable to examine to determine if debug
|            is on at a given level.
|     statement - Statement or sequence of statements to execute.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:  FATAL_ERROR
|
|  Description:
|     The fatal error macro causes debug information to be printed 
|     to stderr, a core dump and an exit.
|
\* Syntax:                                                                  */

#if 0
/* ??? this is not included here until we fix everyhting else */
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

#endif

/*
|  Parameters:
|     statement - Statement or sequence of statements to execute.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: Check Status
|
|  Description:
|     The check status macros are used to check the status of a returning
|     call, and if debug1 is enabled to print out the status.
|
\* Syntax:                                                                  */

#define check_status(string)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    return;\
  }

#define check_null(result, string, status)\
  if (result == NULL) {\
    DEBUG1(\
      fprintf(stderr, string);\
      fflush(stderr);\
    );\
    set_status(status);\
    return;\
  }

#define check_error(result, string, status)\
  if (result == -1) {\
    DEBUG1(\
      perror(string);\
      fflush(stderr);\
    );\
    set_status(status);\
    return;\
  }

#define rcheck_status(string, return_value)\
  if (get_status()) {\
    DEBUG1(\
      isc_print_status(string);\
      fflush(stderr);\
    );\
    return(return_value);\
  }

#define rcheck_null(result, string, status, return_value)\
  if (result == NULL) {\
    DEBUG1(\
      fprintf(stderr, string);\
      fflush(stderr);\
    );\
    set_status(status);\
    return(return_value);\
  }

#define rcheck_error(result, string, status, return_value)\
  if (result == -1) {\
    DEBUG1(\
      perror(string);\
      fflush(stderr);\
    );\
    set_status(status);\
    return(return_value);\
  }

/*
|  Parameters:
|     result - Resultant value to check for error condition.
|     string - String to print on error condition.
|     status - Status to be set.
|     return_value - Value to return to calling routine.
|
|  Returns:
|     Void.
|
|  Statuses:
|     None.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: isc_status_codes
|
|  Description:
|     The isc status codes type enumerates the possible returned status
|     values from the interface layer.
|
\* Definition:                                                              */

	typedef enum status_codes {
/*
   Fairly general status codes.
*/
	  ok 					=0,
	  invalid_handle			=1,
	  invalid_id				=2,
	  not_available				=3,
	  out_of_memory				=4,
	  system_operation_failed		=5,
	  null_memory_ptr			=6,
	  invalid_request			=7,
	  not_found				=8,
	  empty					=9,
	  bad_parameter				=10,
/*
   Interface status codes.
*/
	  already_recording			=11,
          invalid_transaction_handle    	=12,
          invalid_process_handle		=13,
	  invalid_received_transaction_handle	=14,
	  null_element_name			=15,
	  null_component_name			=16,
	  null_mailbox_name			=17,
	  null_ill_file_name			=18,
          element_not_a_record			=19,
          overflow				=20,
          undefined_conversion			=21,
          io_failure                            =22,
          end_of_file                           =23,
          inconsistent_jour_file                =24,
          invalid_journal_file                  =25,
          invalid_change_type			=26,
          not_implemented			=27,
          shm_out_of_bounds			=28,
          component_not_changed			=29,
/*
   Dialogue compiler status codes
*/
          c22_no_sd_definition                  =30,
/*
   Symbol table status codes
*/
          cus_no_property			=31,
          cus_null_symbol			=32,
/*
   Dialogue compiler status codes
*/
/*
   Object base status codes
*/
          invalid_data_type			=33,
          invalid_symbol			=34,
          symbol_not_found			=35,
          invalid_shared_data			=36,
          invalid_data_object			=37,
/*
   Dialogue manager runtime status codes.
*/
          illegal_boolean_operation		=38,
          operation_undefined_types		=39,
	  unknown_handling			=40,
	  handling_undefined			=41,
	  illegal_conversion			=42,
	  failed_conversion			=43,
	  illegal_assignment			=44,
 	  illegal_arithmetic_operation		=45,
	  null_operand				=46,
	  null_first_operand			=47,
	  null_second_operand			=48,
/*
   Dialogue manager runtime status codes.
*/
          dpg_unknown_type                      =49,
          dpg_no_st_entry                       =50,
          dpg_no_ops_wme                        =51,
          dpg_bad_ops_store_symbol              =52,
          dpg_bad_ops_store_symbol_array        =53,
          dpg_no_id_name                        =54,
          dpg_cannot_make_wme                   =55,
          dpg_no_template_entry                 =56,
          dpg_cannot_get_symbol_type            =57,
          dpg_cannot_get_ask_prop               =58,
	  dra_cannot_find_element		=59,
/*
   Dialogue Editor Application (DEA) status codes.
*/
          invalid_dialogue 			=60,
          invalid_vc 				=61,
          invalid_list 				=62,
          invalid_integer 			=63,
          invalid_string 			=64,
          invalid_buffer 			=65,
          invalid_object                        =66,
          invalid_attribute                     =67,
          invalid_method                        =68,
          invalid_object_instance               =69,
          invalid_variable	                =70,
/*
   Serpent X status codes.
*/
/*
   Add new status codes here:
*/
	  removed_data_object			=71,	/* database status */
	  xxx					=72
	} isc_status_codes;

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: isc_validation_stamp
|
|  Description:
|     The isc validation stamps type enumerates the validation stamps for
|     the various private types defined within interface layer.
|
\* Definition:                                                              */

	typedef enum validation_stamp {
	  isc_process_record_vs		=1,
	  isc_transaction_record_vs	=2,
	  isc_rcvd_tran_vs		=3
	} isc_validation_stamp;

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable: isc_status
|
|  Description:
|     The isc status variable contains the status for the interface layer.
|
\* Definition:                                                              */

extern isc_status_codes isc_status;

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: isc_get_status_string
|
|  Description:
|     The isc_get_status_string function returns the string corresponding
|     to the current status.
|
\* Syntax:                                                                  */

        string isc_get_status_string();

/*
|  Parameters:
|     None.
|
|  Returns:
|     the string corresponding to the current status.
|
|  Statuses:
|     none.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: isc_print_status
|
|  Description:
|     Prints out a user defined error message annd the current status.
|
\* Syntax:                                                                  */

        void isc_print_status(
	  /* error_msg : in string */
	);

/*
|  Parameters:
|     error_msg - User defined error message.
|
|  Returns:
|     Void.
|
|  Statuses:
|     none.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: isc_validate
|
|  Description:
|     Validates a handle.
|
\* Syntax:                                                                  */

        boolean isc_validate(
	  /* handle : in char * 			*/
	  /* validation : in isc_validation_stamp 	*/
	);

/*
|  Parameters:
|     handle - Handle to be validated.
|     validation - Validation stamp to examine for.
|
|  Returns:
|     True if handle is valid, false if handle is invalid.
|
|  Statuses:
|     none.
|
\*--------------------------------------------------------------------------*/
#endif

