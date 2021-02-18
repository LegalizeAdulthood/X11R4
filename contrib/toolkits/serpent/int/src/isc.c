static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/isc.c,v 1.10 89/10/06 17:00:34 rcs Exp $";

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
|     $Log:	isc.c,v $
 * Revision 1.10  89/10/06  17:00:34  rcs
 * new status codee
 * 
 * Revision 1.9  89/09/07  17:20:38  trm
 * added routine isc_fget_status to interface with ada.   trm.
 * 
 * Revision 1.8  89/02/09  19:06:30  ejh
 * added copyright notice
 * 
 * Revision 1.7  89/01/04  16:58:18  rcs
 * added removed_data_object status code
 * 
 * Revision 1.6  88/12/21  14:11:40  rcs
 * Updated status code strings data structure
 * Removed temp fix from isc_print_status
 * 
 * Revision 1.5  88/12/21  10:09:50  rcs
 * temp fix for error in isc_print_status
 * 
 * Revision 1.4  88/12/19  17:36:46  rcs
 * added static array of status name strings
 * added isc_get_status_string
 * modified isc_print_status to use isc_get_status_string
 * 
 * Revision 1.3  88/11/02  15:52:52  rcs
 * *** empty log message ***
 * 
 * Revision 1.2  88/11/02  14:15:35  rcs
 * moved debug variable to interface file - rCs
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

#define isc_main
#include "isc.h"                /* status code                              */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(isc_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(isc_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(isc_debug, statement)

/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- Retained Data ---------------------------------------*/

isc_status_codes isc_status = ok;

static char *isc_status_code_strings[] = {
  "ok",
  "invalid handle",
  "invalid id",
  "not available",
  "out of memory",
  "system operation failed",
  "null memory ptr",
  "invalid request",
  "not found",
  "empty",
  "bad parameter",
/*
   Interface status codes.
*/
  "already recording",
  "invalid transaction handle",
  "invalid process handle",
  "invalid received transaction handle",
  "null element name",
  "null component name",
  "null mailbox name",
  "null ill file name",
  "element not a record",
  "overflow",
  "undefined conversion",
  "io failure",
  "end of file",
  "inconsistent jour file",
  "invalid journal file",
  "invalid change type",
  "not implemented",
  "shm out of bounds",
  "component not changed",
/*
   Dialogue compiler status codes.
*/
  "c22 no sd definition",
/*
   Symbol table status codes.
*/
  "cus no property",
  "cus null symbol",
/*
   Dialogue compiler status codes.
*/
/*
   Object base status codes.
*/
  "invalid data type",
  "invalid symbol",
  "symbol not found",
  "invalid shared data",
  "invalid data object",
/*
   Dialogue manager runtime status codes.
*/ 
  "illegal boolean operation",
  "operation undefined types",
  "unknown handling",
  "handling undefined",
  "illegal conversion",
  "failed conversion",
  "illegal assignment",
  "illegal arithmetic operation",
  "null operand",
  "null first operand",
  "null second operand",
/*
   Dialogue manager runtime status codes.
*/
  "unknown type",
  "no st entry",
  "no ops wme",
  "bad ops store symbol",
  "bad ops store symbol array",
  "no id name",
  "cannot make wme",
  "no template entry",
  "cannot get symbol type",
  "cannot get ask property",
  "cannot find element",
/*
   Dialogue Editor Application (DEA) status codes.
*/ 
  "invalid dialogue",
  "invalid vc",
  "invalid list",
  "invalid integer",
  "invalid string",
  "invalid buffer",
  "invalid object",
  "invalid attribute",
  "invalid method",
  "invalid object instance",
  "invalid variable",
/*
   Serpent X status codes.
*/
/*
   Add new status codes here:
*/
  "removed data object",
  "xxx",
/*
   Buffer for error.
*/
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!",
  "UPDATE STATUS CODES STRINGS!!!"
};

/*--------------------------------------------------------------------------*\
|  Name: isc_get_status_string
|
|  Description:
|     The isc_get_status_string function returns the string corresponding
|     to the current status.
\*--------------------------------------------------------------------------*/
string isc_get_status_string()
{                                         /* local type definitions         */
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "isc_get_status_string: call.\n");
  )
/*
   Return.
*/
  DEBUG2(
    fprintf(
      stderr, 
      "isc_get_status_string: status string = %s.\n",
      isc_status_code_strings[(int)isc_status]
    );
    fprintf(stderr, "isc_get_status_string: exiting.\n");
  )
  return isc_status_code_strings[(int)isc_status];
}                                         /* end isc_get_status_string      */

/*--------------------------------------------------------------------------*\
|  Name: isc_print_status
|
|  Description:
|     Prints out a user defined string and the current status.
\*--------------------------------------------------------------------------*/
void isc_print_status(error_msg)
string error_msg;
{                                         /* local type definitions         */
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "isc_print_status: call.\n");
  )
/*
   Print user defined string and the current status.
*/
  fprintf(
    stderr, 
    "%s: %s.\n", 
    error_msg, 
    isc_get_status_string()
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isc_print_status: exiting.\n");
  )
  return;
}                                         /* end isc_print_status           */

/*--------------------------------------------------------------------------*\
|  Name: isc_validate
|
|  Description:
|     Validates a handle.
\*--------------------------------------------------------------------------*/
boolean isc_validate(handle, validation)
int *handle;
isc_validation_stamp validation;
{                                         /* local type definitions         */
  boolean valid = false;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "isc_validate: call.\n");
    fprintf(stderr, "isc_validate: handle = %x.\n", handle);
    fprintf(stderr, "isc_validate: validation = %d.\n", validation);
  )
/*
   Print appropriate error message to stderr.
*/
  switch (validation) {

    case isc_process_record_vs:
      if (*handle == (int)isc_process_record_vs) {
        valid = true;
      }
      break;

    case isc_transaction_record_vs:
        DEBUG3(
          fprintf(stderr, "isc_validate: transaction record.\n");
        );
      if (*handle == (int)isc_transaction_record_vs) {
        DEBUG3(
          fprintf(stderr, "isc_validate: transaction record valid.\n");
        );
        valid = true;
      }
      break;

    case isc_rcvd_tran_vs:
      if (*handle == (int)isc_rcvd_tran_vs) {
        valid = true;
      }
      break;
  }
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isc_validate: %d.\n", valid);
    fprintf(stderr, "isc_validate: exiting.\n");
  )
  return valid;

}                                         /* end isc_validate           */
 

/*--------------------------------------------------------------------------*\
|  Name: isc_fget_status
|
|  Description:
|     The isc_fget_status function returns the status corresponding
|     to the current status to an ada application.
\*--------------------------------------------------------------------------*/
isc_status_codes isc_fget_status()
{                                         /* local type definitions         */
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "isc_fget_status: call.\n");
  )
 

  DEBUG2(
    fprintf(
      stderr, 
      "isc_fget_status: status = %d.\n",
      isc_status
    );
    fprintf(stderr, "isc_fget_status: exiting.\n");
  )
 

  return get_status ();
}                                         /* end isc_fget_status      */
