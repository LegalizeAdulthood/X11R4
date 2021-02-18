/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ckcontent.c,v 1.2 89/09/08 17:48:05 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ckcontent.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	ckcontent.c,v $
 * Revision 1.2  89/09/08  17:48:05  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:34:57  mss
 * Initial revision
 * 
 * Revision 1.1  89/01/05  18:47:45  jaap
 * Initial revision
 * 
 */

#ifndef lint
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ckcontent.c,v $";
#endif
/*
 * directly stolen from marks version
 * slightly cleanup (printf statemants etc.)
 */

#include <ext.h>

INT_type checkForSpecial(curArchClass, contentType, contentMode)
ASN1_OBJECT_ID_type	    curArchClass;
INT_type *contentType, *contentMode;{

    INT_type rval;

    rval = SUCCESS_INT;

    if (at_CONTENT_ARCH_CLASS_proc_cc == curArchClass) {
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	goto LEAVE;
    }
    if (at_CONTENT_ARCH_CLASS_fp_cc == curArchClass) {
	*contentType = CharContent;
	*contentMode = FormattedProcContent;
	goto LEAVE;
    }
    if (at_CONTENT_ARCH_CLASS_form_cc == curArchClass) {
	*contentType = CharContent;
	*contentMode = FormattedContent;
	goto LEAVE;
    }
    if (at_CONTENT_ARCH_CLASS_form_rg == curArchClass) {
	*contentType = RasterContent;
	*contentMode = FormattedContent;
	goto LEAVE;
    }
    if (at_CONTENT_ARCH_CLASS_fp_rg == curArchClass) {
	*contentType = RasterContent;
	*contentMode = FormattedProcContent;
	goto LEAVE;
    }
    if (at_CONTENT_ARCH_CLASS_form_gg == curArchClass) {
	*contentType = GeoContent;
	*contentMode = FormattedContent;
	goto LEAVE;
    }

    /* Not one of the built in values */
    rval = ERROR_INT;

LEAVE:
    return rval;

}


INT_type idEqual(lSeq, rSeq)
ASN1_OBJECT_ID_type lSeq, rSeq; {
    INT_type i;
    INT_type rval;

    rval = SUCCESS_INT;

    if (lSeq->length != rSeq->length) return 0;
    for (i=0; i<lSeq->length; i++) {
	if (lSeq->sequence_value.ints[i] != rSeq->sequence_value.ints[i]){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
    }
LEAVE:
    return rval;
}

INT_type checkDetails(curArchClass, contentType, contentMode)
ASN1_OBJECT_ID_type	    curArchClass;
INT_type *contentType, *contentMode;{


    INT_type	    rval;

    /* Check the actual bits to see if they match */

    rval = SUCCESS_INT;

    /* For now I am lazy and am only interested in processable characters */
    if (ERROR_INT != idEqual(curArchClass, at_CONTENT_ARCH_CLASS_proc_cc)) {
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	goto LEAVE;
    }

    if (ERROR_INT != idEqual(curArchClass, at_CONTENT_ARCH_CLASS_fp_cc)) {
	*contentType = CharContent;
	*contentMode = FormattedProcContent;
	goto LEAVE;
    }
    if (ERROR_INT != idEqual(curArchClass, at_CONTENT_ARCH_CLASS_fp_rg)) {
	*contentType = RasterContent;
	*contentMode = FormattedProcContent;
	goto LEAVE;
    }

    rval = ERROR_INT;
LEAVE:
    return rval;

}

INT_type GetContentType(specConstituent, contentType, contentMode)
CONSTITUENT specConstituent;
INT_type *contentType, *contentMode;{

    ASN1_OBJECT_ID_type	    curArchClass;
    VALUE_type		    curContentType;
    INT_type		    rval;

    rval = SUCCESS_INT;

    /* See if a content architecture class is present */
    if (ERROR_INT != GetAttr(specConstituent, at_CONTENT_ARCH_CLASS, (POINTER_type) &curArchClass, /* Inherited */ BOOL_true, ((PARM_MASK_type *)0))) {
	/* Has a value, so use it  -- first check for special values */
	if (ERROR_INT != checkForSpecial(curArchClass, contentType, contentMode)) {
	    goto LEAVE;
	}
	if (ERROR_INT != checkDetails(curArchClass, contentType, contentMode)) {
	    goto LEAVE;
	}
	fprintf(stderr, "Unknown content architecture class, using text\n");
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	rval = ERROR_INT;
	goto LEAVE;
    }

    if (ERROR_INT != GetAttr(specConstituent, at_CONTENT_TYPE, (POINTER_type) &curContentType, /*Inherited*/ BOOL_true, ((PARM_MASK_type *)0))) {
	/* No arch class, but we have a content type */
	fprintf(stderr, "Cannot handle content types -- only for CCITT compatibility\n");
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	goto LEAVE;
    }

    /* nothing there -- make something up */
    fprintf(stderr, "missing both content architecture class and content type, using text\n");
    *contentType = CharContent;
    *contentMode = ProcessableContent;

LEAVE:
    return rval;
}

