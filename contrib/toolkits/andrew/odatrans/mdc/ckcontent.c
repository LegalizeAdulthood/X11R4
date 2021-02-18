/************************************************************
(c) Copyright 1988, 1989 McDonnell Douglas Corporation
McDonnell Douglas Corporation (MDC) is allowing this Computer
Software to be provided at no cost. MDC does not warrant that the
operation of the Software will be uninterrupted or error free.

MDC makes no warranties of any kind, expressed or implied, including
any implied warranty of merchantability or fitness for a particular purpose.

In no event shall MDC be liable under any legal or equitable theory
(including, but not limited to, contract, negligence, unintentional
misrepresentation or strict liability) for any damages whatsoever,
including, but not limited to direct, indirect or consequential damages,
even if MDC has notice of the possibility of such damages,
or for any claim against any party, inclduing any claim for patent,
copyright, or trade secret infringement.
************************************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/ckcontent.c,v 1.2 89/09/08 17:36:26 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/ckcontent.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/ckcontent.c,v 1.2 89/09/08 17:36:26 mss Exp $";
#endif

/*
  $Log:	ckcontent.c,v $
 * Revision 1.2  89/09/08  17:36:26  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:27:47  mss
 * Initial revision
 * 
 * Revision 1.2  88/12/19  14:33:38  mss
 * Updated for demo version of MDC's interleaf translator
 * 
 * Revision 1.1  88/08/29  15:02:25  mss
 * Initial revision
 * 


*/

#include <odatk.h>
#include <ckcontent.h>

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
	printf("Unknown content architecture class, using text\n");
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	rval = ERROR_INT;
	goto LEAVE;
    }

    if (ERROR_INT != GetAttr(specConstituent, at_CONTENT_TYPE, (POINTER_type) &curContentType, /*Inherited*/ BOOL_true, ((PARM_MASK_type *)0))) {
	/* No arch class, but we have a content type */
	printf("Cannot handle content types -- only for CCITT compatibility\n");
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	goto LEAVE;
    }

    /* nothing there -- make something up */
    printf("missing both content architecture class and content type, using text\n");
    *contentType = CharContent;
    *contentMode = ProcessableContent;

LEAVE:
    return rval;
}

