/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/errormsg.c,v 1.2 89/09/08 17:48:30 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/errormsg.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/errormsg.c,v 1.2 89/09/08 17:48:30 mss Exp $";
#endif

/*
  $Log:	errormsg.c,v $
 * Revision 1.2  89/09/08  17:48:30  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:34:50  mss
 * Initial revision
 * 
 * Revision 1.1  89/01/05  18:46:55  jaap
 * Initial revision
 * 
 * Revision 1.2  88/10/01  17:10:37  mss
 * Added new ODIF_err message
 * 
 * Revision 1.1  88/08/05  17:03:32  mss
 * Initial revision
 *  

*/


/* Routine for getting a printable string for an error number */

#include <ext.h>

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


errexit(str)
char *str;
{
	fprintf(stderr, "%s: %s\n", progname, str);
	exit(1);
}
