/*

ODA Toolkit Validation Suite

FILE: vtest4.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 4: Content Portion

This routine creates an ODA document with a CHARACTER_CONTENT
content portion.  Note that this also tests the use of
the [APPLICATION 2] ASN.1 application tag and the Text-Units
{2 8 1 5 12} ASN.1 Object Identifier.

Chars2Sequence is called to convert a null-terminated string to
an at_CONTENT_INFORMATION_type object, which is the same as
SEQUENCE_BYTE_type.

*/

#include <odatk.h>

#define NULL_CHAR_PTR ((CHAR_type *)0)

extern INT_type	TKError;		/* ToolKit error code */
extern INT_type PrtOD();		/* Print to od */
extern SEQUENCE_type Chars2Sequence();	/* Conv. "" to BYTE seq. */

static INIT_PARMS_type init_parms = {
    BOOL_true,
    (INT_type)0,
    BOOL_true,
    BOOL_true,
    NULL_FILTER_ROUTINE
}; /* gets second parameter at runtime */


DOCUMENT_type Vtest4(od)
    /*IN*/ INT_type od;
{

    INT_type	    rval;
    DOCUMENT_type   test_doc;
    CONSTITUENT	    temp_constituent;
    CONSTITUENT	    char_content;
    STRING_type	    content_id_str;
    STRING_type	    id_str_ptr;

    at_CONTENT_INFORMATION_type content_info;


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
     * Make a character content portion.
     */

    char_content = MakeContent(test_doc, CHARACTER_CONTENT);
    if (char_content == ERROR_CONSTITUENT) {
	PrtOD(od,
	    "The character content portion could not be created.\n");
	goto LEAVE;
    }


    /*
     * Set implemented attributes. See ISO 8613-2 5.9.1.
     */

    content_id_str = Chars2String("0 1 2 3");
    if (content_id_str == ERROR_STRING) {
	PrtOD(od, "Could not make logical content ID string.\n");
	goto LEAVE;
    }

    rval = SetAttr(char_content, at_CONTENT_ID_LOGICAL,
	(POINTER_type) content_id_str, ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Logical content ID for the character content\n");
	PrtOD(od, "portion could not be set.\n");
	goto LEAVE;
    }

    content_id_str = Chars2String("0 1 2 4");
    if (content_id_str == ERROR_STRING) {
	PrtOD(od, "Could not make layout content ID string.\n");
	goto LEAVE;
    }

    rval = SetAttr(char_content, at_CONTENT_ID_LAYOUT,
	(POINTER_type) content_id_str, ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Layout content ID for the character content\n");
	PrtOD(od, "portion could not be set.\n");
	goto LEAVE;
    }


    /*
     * Alternatively, do it with LabelDocument.  This will
     * supercede any previous labels.
     */

    rval = LabelDocument(test_doc);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not label document structures.\n");
	goto LEAVE;
    }

    /*
    rval = GetAttr(char_content, at_CONTENT_ID_LOGICAL, id_str_ptr,
	BOOL_false, ((PARM_MASK_type *) 0));
    if (rval == ERROR_INT) {
	PrtOD(od,"Could not look at content ID.\n");
	goto LEAVE;
    }
    */


    /*
     * Set the content information (the "contents" of the
     * content portion).  Use a sequence of BYTE_type where
     * each byte is a character.
     */

    content_info = (at_CONTENT_INFORMATION_type)
	Chars2Sequence("This is a character content portion.");

    if (content_info ==
	    (at_CONTENT_INFORMATION_type)ERROR_SEQUENCE) {
	PrtOD(od, "Could not convert string to content-info.\n");
	goto LEAVE;
    }

    rval = SetAttr(char_content, at_CONTENT_INFORMATION,
	((SEQUENCE_BYTE_type)content_info), ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not set content information attribute.\n");
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
