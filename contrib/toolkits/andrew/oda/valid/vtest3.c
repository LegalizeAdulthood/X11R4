/*

ODA Toolkit Validation Suite

FILE: vtest3.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 3: Presentation Styles

This routine creates an ODA document with a presentation
style and a layout style.  Aside from testing the
implementation of styles, it tests the use of the
Style Identifier [APPLICATION 5] Application Class Tag
and the Presentation-Style-Descriptor and Layout-
Style-Descriptor {2 8 1 5 10} ASN.1 Object Identifiers.

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


DOCUMENT_type Vtest3(od)
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
     * Create a presentation style descriptor.
     */

    temp_constituent = MakeStyle(test_doc, PRESENTATION_STYLE);

    if (temp_constituent == ERROR_CONSTITUENT) {
	PrtOD(od, "The presentation style could not be created.\n");
	goto LEAVE;
    }


    /*
     * Create a layout style descriptor.
     */

    temp_constituent = MakeStyle(test_doc, LAYOUT_STYLE);

    if (temp_constituent == ERROR_CONSTITUENT) {
	PrtOD(od, "The layout style could not be created.\n");
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
