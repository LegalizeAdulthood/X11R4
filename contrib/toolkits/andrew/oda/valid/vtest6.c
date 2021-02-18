/*


ODA Toolkit Validation Suite

FILE: vtest6.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 6: Object Identifiers

This routine creates an ODA document to test use
of ASN.1 Object Identifiers by generating those objects
to which they refer.

Specifically: { 2 8 0 0 }, External Data Type
	  and { 2 8 1 5 11}, Default-Value-Lists

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


DOCUMENT_type Vtest6(od)
    /*IN*/ INT_type od;
{

    INT_type	    rval;
    DOCUMENT_type   test_doc;
    CONSTITUENT	    temp_constituent;
    at_DEFAULT_VALUE_LISTS_type
		    def_val_lists;


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
     * Label the document structures.
     */

    rval = LabelDocument(test_doc);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not label document structures.\n");
	goto LEAVE;
    }


    /*
     * Generate those objects which use ASN.1 Object
     * Identifiers not tested in the other test functions.
     */

    /* Object Identifier { 2 8 1 5 11 }: Default-Value-Lists */

    /* Initialize default value lists. */
    rval = InitializeDefaultValueLists(test_doc, &def_val_lists,
	ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not initialize default value lists.\n");
	goto LEAVE;
    }

    /* Unspecify it and initialize it again. */
    rval = UnspecifyDefaultValueLists(&def_val_lists);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not unspecify default value lists.\n");
	goto LEAVE;
    }
    rval = InitializeDefaultValueLists(test_doc, &def_val_lists,
	ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not re-initialize default value lists.\n");
	goto LEAVE;
    }

    /* Object Identifier { 2 8 0 0 }: External Data Type */
    /* NOTE: This is currently not defined in ISO 8613. */
    /* See 8613-5: 5.1. */


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
