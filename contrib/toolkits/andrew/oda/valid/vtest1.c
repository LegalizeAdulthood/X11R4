/*

ODA Toolkit Validation Suite

FILE: vtest1.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 1: Empty Document

This routine creates an empty ODA document.

*/

#include <odatk.h>

#define NULL_CHAR_PTR ((CHAR_type *)0)

extern INT_type	TKError;		/* ToolKit error code */
extern INT_type PrtOD();		/* Print to od */

static INIT_PARMS_type init_parms = {
    BOOL_true,
    ((INT_type)0),
    BOOL_true,
    BOOL_true,
    NULL_FILTER_ROUTINE
}; /* gets second parameter at runtime */


DOCUMENT_type Vtest1(od)
    /*IN*/ INT_type	od;
{

    INT_type	    rval;
    DOCUMENT_type   test_doc;


    /*
     * Initialize the Toolkit.
     */

    init_parms.error_od = od;
    rval = InitToolKit(&init_parms);

    if (rval == ERROR_INT) {
	PrtOD(od, "The ToolKit could not be initialized.\n");
	goto LEAVE;
    }

    /*
     * Create the data structure for the ODA document.
     */

    test_doc = MakeDocument();

    if (test_doc == ERROR_DOCUMENT) {
	PrtOD(od,"The document could not be created.\n");
	goto LEAVE;
    }


    /*
     * Finalize the Toolkit.
     */

    rval = FinalToolKit();

    if (rval == ERROR_INT) {
	PrtOD(od,"The ToolKit could not be finalized.\n");
	goto LEAVE;
    }

    return(test_doc);

LEAVE:
QUIETLEAVE:
    return(ERROR_DOCUMENT);

    /* Specific error code will be found in TKError. */
}
