/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
#include <errno.h>
#include <error.h>
#include <ext.h>

fatal(s, s1, s2, s3, s4)	/* should be a flag on error */
	char *s, *s1, *s2, *s3, *s4;
{
	error(FATAL, s, s1, s2, s3, s4);
}

warn(s, s1, s2, s3, s4)
	char *s, *s1, *s2, *s3, *s4;
{
	error(WARN, s, s1, s2, s3, s4);
}

error(die, s, s1, s2, s3, s4)
	int die;
	char *s, *s1, *s2, *s3, *s4;
{
	extern char *cmdname, *sys_errlist[];
	extern int errno, sys_nerr;

	fprintf(stderr, "%s: ", cmdname);
	fprintf(stderr, s, s1, s2, s3, s4);
	if (errno > 0 && errno < sys_nerr)
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	fprintf(stderr, "\n");
	if (die) {
		exit(1);
	} else
		errno = 0;
}


/*
 * Routine for getting a printable string for an error number
 * out of the ODA toolkit
 */

char * ErrorMsg(errorNumber)
INT_type errorNumber;
{
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
