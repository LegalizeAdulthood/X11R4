
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter4.c,v 1.2 89/10/31 15:10:51 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter4.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter4.c,v $
 * Revision 1.2  89/10/31  15:10:51  annm
 * fixing lint problems
 * 
 * Revision 1.1  89/09/21  06:32:57  mss
 * Initial revision
 * 
 * Revision 1.6  89/08/17  16:20:28  mss
 * Fixed bad ifdef 0
 * 
 * Revision 1.5  89/06/15  08:44:27  mss
 * commented out error return for empty loop
 * 
 * Revision 1.4  89/06/14  17:49:19  mss
 * Fixed return value so that rval is correctly set
 * 
 * Revision 1.3  89/06/14  14:09:08  annm
 * changed NOSTYLES_err to NO_STYLES_err
 * 
 * Revision 1.2  89/06/13  16:07:46  annm
 * added NOSTYLES_err return for when there are no styles.,
 * Also added checking of the kind arguement.
 * 
 * Revision 1.1  89/04/24  14:30:37  mss
 * Initial revision
 * 

*/



#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter4.c,v $";
#endif

/* iter4.c: main module for performing style iteration */



#define TK_IMPL_ITER
#define TK_GVARS_ITER4
#define TK_IMPL_ATTR
#define TK_IMPL_CONST
#define TK_DEBUG
#define TK_MM
#define TK_IMPL_DOC
#define TK_TKI_TKERR

#include <debug.h>
#include <mm.h>
#include <iter.h>
#include <tkerr.h>

#undef TK_IMPL_ITER
#undef TK_GVARS_ITER4
#undef TK_IMPL_ATTR
#undef TK_IMPL_CONST
#undef TK_DEBUG
#undef TK_MM
#undef TK_IMPL_DOC
#undef TK_TKI_TKERR


/* Local forward declarations */



ITERATOR_type MakeStylesIterator(document, kind)
    IN DOCUMENT_type	document;
    IN VALUE_type	kind;
{

    /* put local dcl's here */

    ITERATOR_type	    rval;	/* the returned value */
    INTERNAL_ITERATOR_type  irval;	/* internal version of return value */
    INTERNAL_DOCUMENT_type  idoc;	/* coerced document pointer */
    INT_type		    ignoreError;/* Place to hold ignored status */
    INT_type		    dummy;	/* For error returns of msgs */


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Validate the parameters */
    /* Garbage for the document? */
    if ( (ERROR_DOCUMENT == document) || 
	 ((DOCUMENT_type) 0 == document)){
	rval = ERROR_ITERATOR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeStylesIterator]: NULL or ");
	dummy = TKERR_Format("ERROR constituent passed as root");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check kind */
    if( kind != PRESENTATION_STYLE && kind != LAYOUT_STYLE ){
	rval = ERROR_ITERATOR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MakeStylesIterator]: bad kind value - got " );
	dummy = TKERR_FormatInt("%d", (INT_type) kind );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    idoc = (INTERNAL_DOCUMENT_type) document;

/* Leave here until arg resolved */
#ifdef ReturnErrorForNoStylesInIter
    /* make sure the styles list is not empty */
    if( kind == PRESENTATION_STYLE &&
	idoc->presentation_styles_list.forward == 
	    &(idoc->presentation_styles_list) &&
	idoc->presentation_styles_list.back ==
	    &(idoc->presentation_styles_list) ){
	rval = ERROR_ITERATOR;
	TKError = NO_STYLES_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MakeStylesIterator]: no presentation styles" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else if( idoc->layout_styles_list.forward ==
		&(idoc->layout_styles_list) &&
	     idoc->layout_styles_list.back ==
		&(idoc->layout_styles_list) ){
	rval = ERROR_ITERATOR;
	TKError = NO_STYLES_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MakeStylesIterator]: no layout styles" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
#endif

    /* What kind of style? */
    switch(kind) {
	case PRESENTATION_STYLE:
	case LAYOUT_STYLE:
	    irval = (INTERNAL_ITERATOR_type)MM_Malloc(
			(INT_type) sizeof(ITER_state_type));
	    if ( ( (POINTER_type) irval) == ERROR_POINTER) {
		rval = ERROR_ITERATOR;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[MakeStylesIterator]: Could not ");
		dummy = TKERR_Format("allocate space for new iterator");
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	    rval = (ITERATOR_type) irval;
	    if (kind == PRESENTATION_STYLE) {
		irval->constituentType = PresentationStyleList;
		irval->iteratorRoot = idoc->presentation_styles_list.forward;
	    }
	    else {
		irval->constituentType = LayoutStyleList;
		irval->iteratorRoot = idoc->layout_styles_list.forward;
	    }
	    break;

	default:
	    rval = ERROR_ITERATOR;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[MakeStylesIterator]: Unknown ");
	    dummy = TKERR_FormatInt("style kind %d",
				    (INT_type) kind);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;

    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

