/*

ODA Toolkit Validation Suite

FILE: vtest7.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 7: <blank>


*/

#include <odatk.h>

#define NULL_CHAR_PTR ((CHAR_type *)0)

extern INT_type	TKError;		/* ToolKit error code */
extern INT_type PrtOD();		/* Print to od */

static INIT_PARMS_type init_parms = {
    BOOL_true,
    (INT_type)0,
    BOOL_true,
    BOOL_true,
    NULL_FILTER_ROUTINE
}; /* gets second parameter at runtime */


DOCUMENT_type Vtest7(od)
    /*IN*/ INT_type od;
{

    INT_type	    rval;
    DOCUMENT_type   test_doc;
    CONSTITUENT	    temp_constituent;


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
     * Create the document profile constituent.
     */

    temp_constituent = MakeDocumentProfile(test_doc);

    if (temp_constituent == ERROR_CONSTITUENT) {
	PrtOD(od, "The document profile could not be created.\n");
	goto LEAVE;
    }


    /*
     * Create generic and specific logical roots.
     */

    temp_constituent = MakeComponent(test_doc,
	at_OBJECT_TYPE_doc_logical_root,GENERIC_COMPONENT);

    if (temp_constituent == ERROR_CONSTITUENT) {
	PrtOD(od, "The generic logical root could not be created.\n");
	goto LEAVE;
    }

    temp_constituent = MakeComponent(test_doc,
	at_OBJECT_TYPE_doc_logical_root,SPECIFIC_COMPONENT);

    if (temp_constituent == ERROR_CONSTITUENT) {
	PrtOD(od, "The specific logical root could not be created.\n");
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
