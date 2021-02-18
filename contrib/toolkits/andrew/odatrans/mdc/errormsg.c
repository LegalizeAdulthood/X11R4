/************************************************************
(c) Copyright 1988, 1989 McDonnell Douglas Corporation
McDonnell Douglas Corporation (MDC) is allowing this Computer
Software to be provided at no cost. MDC does not warrant that the
operation of the Software will be uninterrupted or error free.

MDC makes no warranties of any kind, expressed or implied, including
any implied warranty of merchantability or fitness for a particular purpose.

In no event shall MDC be liable under any legal or equitable theory
(including, but not limited to, contract, negligence, unintentional
misrepresentation or strict liability) for any damages whatsoever,
including, but not limited to direct, indirect or consequential damages,
even if MDC has notice of the possibility of such damages,
or for any claim against any party, inclduing any claim for patent,
copyright, or trade secret infringement.
************************************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/errormsg.c,v 1.2 89/09/08 17:36:49 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/errormsg.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/errormsg.c,v 1.2 89/09/08 17:36:49 mss Exp $";
#endif

/*
  $Log:	errormsg.c,v $
 * Revision 1.2  89/09/08  17:36:49  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:27:50  mss
 * Initial revision
 * 
 * Revision 1.2  88/12/19  14:33:48  mss
 * Updated for demo version of MDC's interleaf translator
 * 
 * Revision 1.2  88/10/01  17:10:37  mss
 * Added new ODIF_err message
 * 
 * Revision 1.1  88/08/05  17:03:32  mss
 * Initial revision
 *  

*/


/* Routine for getting a printable string for an error number */

#include <odatk.h>

char * ErrorMsg(errorNumber)
INT_type errorNumber; {
   switch (errorNumber) {

          case ERROR_INT:
	      return "Unspecified error return code (see TKError)";
          case SUCCESS_INT:
	      return "\"No error\" return code";

          case BADATTR_err:
	      return "Attribute not applicable";
          case BADPARM_err:
	      return "Bad parameter passed to toolkit";
          case DESCRIPTOR_err:
	      return "Descriptor error";
          case EXISTS_err:
	      return "Exists error";
          case INTERNAL_err:
	      return "Internal toolkit error";
          case LOCAL_err:
	      return "Error in local (site) module";
          case NOATTR_err:
	      return "(NOATTR) No attribute associated with object";
          case NOFINAL_err:
	      return "Finalize not called before second intialize";
          case NOGENS_err:
	      return "NOGENS error";
          case NOMEM_err:
	      return "No memory error";
          case NOPROF_err:
	      return "No prof error";
          case NOROOT_err:
	      return "No root error";
          case UNIMPL_err:
	      return "Unimplemented operation";
	  case ODIF_err:
	      return "Unrecognized ODIF stream";
	  default:
	       return "Unknown error code";
   }
}
