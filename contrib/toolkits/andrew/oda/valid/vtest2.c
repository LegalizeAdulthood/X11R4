/*

ODA Toolkit Validation Suite

FILE: vtest2.c

Copyright (c) 1989 Carnegie Mellon University

Validation Test 2: Document Profile

This routine contains most of the tests relating to the
document profile.

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

static dp_UNIT_SCALING_type unit_scaling = {
    1,
    1
};


DOCUMENT_type Vtest2(od)
    /*IN*/ INT_type	od;
{

    INT_type	    rval;
    DOCUMENT_type   test_doc;
    CONSTITUENT	    doc_profile;
    CONSTITUENT	    temp_constituent;
    STRING_type	    doc_description;

    dp_DOCUMENT_DATE_AND_TIME_type
		    date_time_str;

    NAME_ORG_type   *author_name_org; /* gets vals at runtime */
    PERSONAL_NAME_type
		    *author_name;     /* gets vals at runtime */
    SEQUENCE_NAME_ORG_type
		    authors_seq;


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

    doc_profile = MakeDocumentProfile(test_doc);

    if (doc_profile == ERROR_CONSTITUENT) {
	PrtOD(od, "The document profile could not be created.\n");
	goto LEAVE;
    }

    /* Can also get document profile by "finding" it. */

    doc_profile = FindDocumentProfile(test_doc);
    if (doc_profile == ERROR_CONSTITUENT) {
	PrtOD(od,
	    "The just-created document profile could not be found.\n");
	goto LEAVE;
    }


    /*
     * Set the UNIT_SCALING attribute for the document
     * profile.
     */

    rval = SetAttr(doc_profile, dp_UNIT_SCALING,
	&unit_scaling, ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od,"The dp_UNIT_SCALING attribute could not be set.\n");
	goto LEAVE;
    }


    /*
     * Set the DOCUMENT_DATE_AND_TIME attribute.
     * This tests Application Class Tag 2 [APPLICATION 2].
     */

    date_time_str = (dp_DOCUMENT_DATE_AND_TIME_type)
	Chars2String("1/1/89 10:00:00");
    if (date_time_str == ERROR_STRING) {
	PrtOD(od, "Could not convert date_time_str.\n");
	goto LEAVE;
    }
    rval = SetAttr(doc_profile, dp_DOCUMENT_DATE_AND_TIME,
	date_time_str, ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not set DOCUMENT_DATE_AND_TIME.\n");
	goto LEAVE;
    }


    /*
     * The following 'Document-Description' attributes use
     * ASN.1 Character-Data, which tests Application Class
     * Tag 3 [APPLICATION 3].
     * According to the ASN.1 specification, Character-Data
     * strings must include explicit CRLF.
     */

    doc_description = Chars2String("Document Profile Tests\r\n");
    if (doc_description == ERROR_STRING) {
	PrtOD(od, "Could not convert doc_description.\n");
	goto LEAVE;
    }
    rval = SetAttr(doc_profile, dp_TITLE, doc_description,
	ALL_PARMS);
    if (rval== ERROR_INT) {
	PrtOD(od, "Could not set TITLE.\n");
	goto LEAVE;
    }

    doc_description =Chars2String(
	"Tests dealing with the Document Profile\r\n");
    if (doc_description == ERROR_STRING) {
	PrtOD(od, "Could not convert doc_description.\n");
	goto LEAVE;
    }
    rval = SetAttr(doc_profile, dp_SUBJECT, doc_description,
	ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not set SUBJECT.\n");
	goto LEAVE;
    }


    /*
     * Set the AUTHORS attribute.  This tests Application
     * Class Tag 6 [APPLICATION 6].
     */

    /* Create NAME_ORG sequence required by AUTHORS attribute. */

    authors_seq = MakeSequence(SEQUENCE_NAME_ORG_tag, (INT_type) 1);
    if (authors_seq == ERROR_SEQUENCE) {
	PrtOD(od, "Could not create SEQUENCE_NAME_ORG.\n");
	goto LEAVE;
    }

    author_name_org = &(authors_seq->sequence_value.name_orgs[0]);
    author_name = &((*author_name_org).name);


    /* Set the PERSONAL_NAME data structure in NAME_ORG. */

    author_name->surname = Chars2Sequence("Biafra");
    if (author_name->surname == ERROR_SEQUENCE) {
	PrtOD(od, "Could not set PERSONAL_NAME.surname.\n");
	goto LEAVE;
    }
    author_name->givenname = Chars2Sequence("Jello");
    if (author_name->givenname == ERROR_SEQUENCE) {
	PrtOD(od, "Could not set PERSONAL_NAME.givenname.\n");
	goto LEAVE;
    }
    author_name->initials = Chars2Sequence("T.");
    if (author_name->initials == ERROR_SEQUENCE) {
	PrtOD(od, "Could not set PERSONAL_NAME.initials.\n");
	goto LEAVE;
    }
    author_name->title = Chars2Sequence("Mr.");
    if (author_name->title == ERROR_SEQUENCE) {
	PrtOD(od, "Could not set PERSONAL_NAME.title.\n");
	goto LEAVE;
    }


    /* Set rest of NAME_ORG data structure. */

    author_name_org->name_is_present = BOOL_true;
    author_name_org->organization =
	Chars2String("Information Technology Center");
    if (author_name_org->organization == ERROR_STRING) {
	PrtOD(od, "Could not set NAME_ORG.organization.\n");
	goto LEAVE;
    }

    rval = SetAttr(doc_profile, dp_AUTHORS, authors_seq,
	ALL_PARMS);
    if (rval == ERROR_INT) {
	PrtOD(od, "Could not set AUTHORS attribute.\n");
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
     *  Finalize the Toolkit.
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
