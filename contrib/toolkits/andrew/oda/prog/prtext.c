/* Procedure that produces the text in an ODIF file */


#include <odatk.h>
#include <stdio.h>

/* forward declarations */

INT_type ExtractContents();
INT_type ExtractText();
INT_type ExtractRaster();
INT_type ExtractGraphics();

main ()
{
    DOCUMENT_type	NewDoc;	    /* Document read in */
    INT_type		IntStatus;  /* Results from tool kit calls */
    INIT_PARMS_type	parms;	    /* Telling tool kit what to do */

    /* Start off everything */
    parms.automatic_label = BOOL_true;
    parms.error_od = fileno(stderr);
    parms.format_errors = BOOL_true;
    parms.print_warnings = BOOL_true;
    parms.output_filter = NULL_FILTER_ROUTINE;

    IntStatus = LOCAL_Init();
    if (IntStatus == LCL_ERROR_INT) {
	fprintf(stderr,"Could not initialize local facility, aborting\n");
	goto LEAVE;
    }
    fflush(stderr);
    fflush(stdout);

    IntStatus = InitToolKit(&parms);
    if (IntStatus == ERROR_INT) {
	fprintf(stderr,"Could not initialize toolkit, aborting\n");
	goto LEAVE;
    }
    fflush(stderr);
    fflush(stdout);

    /* Get the document */
    NewDoc = ReadODIF((INT_type) fileno(stdin));
    if (ERROR_DOCUMENT == NewDoc) {
	fprintf(stderr, "Could not read the ODIF, aborting\n");
	goto LEAVE;
    }
    fflush(stderr);
    fflush(stdout);


    /* Dump the text */
    if (ERROR_INT == ExtractContents(NewDoc)){
	goto LEAVE;
    }

    /* Shut down everything */
    if (ERROR_INT == FinalToolKit()) {
	fprintf(stderr,"Could not finalize tool kit\n");
    }
    if (LCL_ERROR_INT == LOCAL_Final()){
	fprintf(stderr,"Could not finalize local\n");
    }


LEAVE:
    fflush(stderr);
    fflush(stdout);

}

INT_type ExtractContents(Doc)
DOCUMENT_type Doc;
{
    INT_type rval;	/* status of function */
    CONSTITUENT		DocRoot;    /* Root of specific logical tree */
    ITERATOR_type	MainWalker; /* Control for tree walk */
    at_OBJECT_TYPE_type	ObjectType; /* For the GetAttr call */
    CONSTITUENT		CurConstituent;	/* Loop index */
    INT_type		IntStatus;	/* Tool kit return values */
    rval = SUCCESS_INT;

    /* Walk the document */
    DocRoot =  FindDocumentRoot(Doc, SPECIFIC_DOC_LOGICAL_ROOT);
    if (ERROR_CONSTITUENT == DocRoot){
	fprintf(stderr,"Could not find logical root\n");
	rval = ERROR_INT;
	goto LEAVE;
    }

    MainWalker = MakeSubgraphIterator(DocRoot, PARENTS_FIRST, DEPTH_FIRST);
    if (MainWalker == ERROR_ITERATOR) {
	fprintf(stderr,"Could not create walker over specific logical structure\n");
	rval = ERROR_INT;
	goto LEAVE;
    }

    while ((NULL_CONSTITUENT != (CurConstituent = NextConstituent(MainWalker))) &&
	    (ERROR_CONSTITUENT != CurConstituent)) {
	/* See if at base of tree */
	IntStatus = GetValueAttr(CurConstituent, at_OBJECT_TYPE, &ObjectType, BOOL_false);
	if (IntStatus == ERROR_INT) {
	    fprintf(stderr,"Could not retrieve object type\n");
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	if (ObjectType == at_OBJECT_TYPE_bas_logical_obj) {
	    /* Have a basic, get the contents */
	    switch (GetContentKind(CurConstituent)) {
		case PROC_CC_kind:
		case FP_CC_kind:
		case FORM_CC_kind:
		    rval = ExtractText(CurConstituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;

		case FORM_RG_kind:
		case FP_RG_kind:
		    rval = ExtractRaster(CurConstituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;

		case FORM_GG_kind:
		    rval = ExtractGraphics(CurConstituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;

		case ERROR_kind:
		default:
		   fprintf(stderr,"Could not content kind\n");
		   rval	= ERROR_INT;
		   goto LEAVE;

	    }

	}

    }
    if (ERROR_CONSTITUENT == CurConstituent) {
	fprintf(stderr,"Error value from next constituent\n");
	rval = ERROR_INT;
	goto LEAVE;
    }
    else if (ERROR_INT == DeleteIterator(MainWalker)) {
	fprintf(stderr,"Error deleting iterator\n");
	rval = ERROR_INT;
	goto LEAVE;
    }



LEAVE:
    return rval;
}

INT_type ExtractText(basic)
CONSTITUENT basic;
{
    INT_type		rval;	    /* return value from function */
    SEQUENCE_BYTE_type	theText;    /* bytes of returned content */
    INT_type		i;	    /* Loop for printing characters */

    rval = SUCCESS_INT;

    /* Retrieve content using ODA rules */
    theText = GetContent(basic);
    if (theText == ERROR_SEQUENCE) {
	fprintf(stderr,"Error getting content\n");
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Got the content, just shove it out. 
      Many systems have a better way, but this is safe and easy */
    for	(i=0;i<theText->length;i++){
	printf("%c", theText->sequence_value.bytes[i]);
    }
    /* Should track "concatenation" to see if CRs should be inserted */
    printf("\n");

    if (ERROR_INT == DeleteSequence(theText)) {
	fprintf(stderr,"Error deleting content\n");
	rval = ERROR_INT;
	goto LEAVE;
    }

LEAVE:
    return rval;

}

INT_type ExtractRaster(basic)
CONSTITUENT basic;
{
    INT_type		rval;

    rval = SUCCESS_INT;

    printf("\n<Ignoring raster content>\n");

LEAVE:
    return rval;

}

INT_type ExtractGraphics(basic)
CONSTITUENT basic;
{
    INT_type		rval;

    rval = SUCCESS_INT;

    printf("\n<Ignoring graphics content>\n");

LEAVE:
    return rval;

}
