/* Procedure that produces a human readable form of the odif stream
   as a logical oda structure */

/* Adapted from "samplein" program */


#include <odatk.h>
#include <stdio.h>



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

    /* Dump the debugging output */
    if (ERROR_INT == WriteDocumentState((INT_type) fileno(stdout), NewDoc)){
	fprintf(stderr,"Could not provide debugging output\n");
	goto LEAVE;
    }
    fflush(stderr);
    fflush(stdout);

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
