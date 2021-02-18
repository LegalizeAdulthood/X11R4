
/*

ODA Toolkit Validation Suite

FILE: vtest5.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 5: Sequence Construct

This routine creates and performs various operations
on SEQUENCE constructs.

Currently none of the SEQUENCEs are stored in the document
structure;  all testing, therefore, is during the run
time of this function, not during the test file
comparison.

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


DOCUMENT_type Vtest5(od)
    /*IN*/ INT_type od;
{

    INT_type	    rval;
    DOCUMENT_type   test_doc;
    CONSTITUENT	    temp_constituent;
    SEQUENCE_type   temp_sequence;


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
     * Make a SEQUENCE, expand it, contract it, and delete it.
     */

    /* ASN1_OBJECT_ID sequence */

    temp_sequence = MakeSequence(SEQUENCE_ASN1_OBJECT_ID_tag, (INT_type) 3);
    if (temp_sequence == ERROR_SEQUENCE) {
	PrtOD(od, "Could not create ASN1_OBJECT_ID sequence.\n");
	goto LEAVE;
    }
    rval = ExpandSequence(temp_sequence, (INT_type) 3);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not expand ASN1_OBJECT_ID sequence.\n");
	goto LEAVE;
    }
    rval = ContractSequence(temp_sequence, (INT_type) 5);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not contract ASN1_OBJECT_ID sequence.\n");
	goto LEAVE;
    }
    rval = DeleteSequence(temp_sequence);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not delete ASN1_OBJECT_ID sequence.\n");
	goto LEAVE;
    }

    /* ATOMIC_STR_EXPR sequence */

    temp_sequence = MakeSequence(SEQUENCE_ATOMIC_STR_EXPR_tag, (INT_type) 3);
    if (temp_sequence == ERROR_SEQUENCE) {
	PrtOD(od, "Could not create ATOMIC_STR_EXPR sequence.\n");
	goto LEAVE;
    }
    rval = ExpandSequence(temp_sequence, (INT_type) 3);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not expand ATOMIC_STR_EXPR sequence.\n");
	goto LEAVE;
    }
    rval = ContractSequence(temp_sequence, (INT_type) 5);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not contract ATOMIC_STR_EXPR sequence.\n");
	goto LEAVE;
    }
    rval = DeleteSequence(temp_sequence);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not delete ATOMIC_STR_EXPR sequence.\n");
	goto LEAVE;
    }

    /* BYTE sequence */

    temp_sequence = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) 3);
    if (temp_sequence == ERROR_SEQUENCE) {
	PrtOD(od, "Could not create BYTE sequence.\n");
	goto LEAVE;
    }
    rval = ExpandSequence(temp_sequence, (INT_type) 3);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not expand BYTE sequence.\n");
	goto LEAVE;
    }
    rval = ContractSequence(temp_sequence, (INT_type) 5);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not contract BYTE sequence.\n");
	goto LEAVE;
    }
    rval = DeleteSequence(temp_sequence);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not delete BYTE sequence.\n");
	goto LEAVE;
    }

    /* CONSTITUENT sequence */

    temp_sequence = MakeSequence(SEQUENCE_CONSTITUENT_tag, (INT_type) 3);
    if (temp_sequence == ERROR_SEQUENCE) {
	PrtOD(od, "Could not create CONSTITUENT sequence.\n");
	goto LEAVE;
    }
    rval = ExpandSequence(temp_sequence, (INT_type) 3);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not expand CONSTITUENT sequence.\n");
	goto LEAVE;
    }
    rval = ContractSequence(temp_sequence, (INT_type) 5);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not contract CONSTITUENT sequence.\n");
	goto LEAVE;
    }
    rval = DeleteSequence(temp_sequence);
    if (rval == ERROR_INT) {
	PrtOD(od,"Cound not delete CONSTITUENT sequence.\n");
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
