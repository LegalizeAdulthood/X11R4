/*

ODA Toolkit Validation Suite

FILE: vstruc.h

Copyright (c) 1989 Carnegie Mellon University

These are the function references and data structures used
by both valid.c and mkdoc.c.  Although the two programs
are compiled separately, all references are made through this
file to eliminate any chance for discrepancies.

This file is #included in mkdoc.c and valid.c.

*/


extern DOCUMENT_type Vtest1();
extern DOCUMENT_type Vtest2();
extern DOCUMENT_type Vtest3();
extern DOCUMENT_type Vtest4();
extern DOCUMENT_type Vtest5();
extern DOCUMENT_type Vtest6();
extern DOCUMENT_type Vtest7();

/*
This structure contains information about the test programs.

  *name:         Test program name
  *description:  Short description of test
  (*init)():	 Routine to initialize TK;  NULL_INT_ROUTINE if none
  *input_file:   Input file name;	    NULL_CHAR_PTR if none
  (*munge)():	 Routine to munge document; NULL_DOC_ROUTINE if none
  *output_file:	 Output file name;	    NULL_CHAR_PTR if none
*/

static struct {

    CHAR_type	    *name;
    CHAR_type	    *description;
    INT_type	    (*init)();
    CHAR_type	    *input_file;
    DOCUMENT_type   (*munge)();
    CHAR_type	    *output_file;

} tests[] = {

    /* Test 0 */    {"test0",		    "simple TK link test",
		       NULL_INT_ROUTINE,    NULL_CHAR_PTR,
		       NULL_DOC_ROUTINE,    NULL_CHAR_PTR },

    /* Test 1 */    {"test1",		    "empty document test",
		       NULL_INT_ROUTINE,    "std1.out",
		       Vtest1,		    "test1.out" },

    /* Test 2 */    {"test2",		    "document profile test",
		       NULL_INT_ROUTINE,    "std2.out",
		       Vtest2,		    "test2.out" },

    /* Test 3 */    {"test3",		    "presentation-style test",
		       NULL_INT_ROUTINE,    "std3.out",
		       Vtest3,		    "test3.out" },

    /* Test 4 */    {"test4",		    "character-content test",
		       NULL_INT_ROUTINE,    "std4.out",
		       Vtest4,		    "test4.out" },

    /* Test 5 */    {"test5",		    "SEQUENCE test",
		       NULL_INT_ROUTINE,    "std5.out",
		       Vtest5,		    "test5.out" },

    /* Test 6 */    {"test6",		    "ASN.1 object ident. test",
		       NULL_INT_ROUTINE,    "std6.out",
		       Vtest6,		    "test6.out" },

    /* Test 7 */    {"test7",		    "undefined",
		       NULL_INT_ROUTINE,    "std7.out",
		       Vtest7,		    "test7.out" }

};

#define NTESTS	(sizeof(tests) / sizeof(tests[0]))
