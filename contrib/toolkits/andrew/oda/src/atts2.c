
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts2.c,v 1.1 89/09/21 06:25:33 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts2.c,v $

(c) Copyright 1989 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	atts2.c,v $
 * Revision 1.1  89/09/21  06:25:33  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/12  09:12:07  mss
 * Removed #
 * 
 * Revision 1.3  89/05/23  16:14:27  mss
 * Fixed Style attribute proceedures to use correct call
 * of MakeStyle
 * 
 * Revision 1.2  89/04/28  21:33:28  mss
 * Added SetPresAttr and SetLayoutAttr
 * 
 * Revision 1.1  89/04/25  16:30:57  mss
 * Initial revision
 * 

*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts2.c,v $";
#endif



#define TK_IMPL_ATTS
#define TK_IMPL_CONST
#define TK_GVARS_ATTS2
#define TK_TKI_TKERR

#include <atts.h>
#include <acd.h>
#include <parms.h>

#undef TK_IMPL_ATTS
#undef TK_GVARS_ATTS2
#undef TK_TKI_TKERR

/* Forward declarations */
static INT_type SetStyleAttr(/*constituent, attribute, value, mask, styleAttribute*/);


/*

    This file contains some utility procedures for attribute/constituent manipulation

*/



DOCUMENT_type GetDocument(constituent)
CONSTITUENT constituent;
{

    /* put local dcl's here */
    INTERNAL_CONSTITUENT    iconstituent;
    DOCUMENT_type	    rval;	    /* the returned value */
    INT_type		    dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Validate the parameter */
    if ((constituent == ((CONSTITUENT) 0)) ||
	(constituent == NULL_CONSTITUENT) ||
	(constituent == ERROR_CONSTITUENT)) {
	rval = ERROR_DOCUMENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[GetDocument]: NULL or ERROR constituent passed" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Make sure that the document value is reasonable */
    iconstituent = (INTERNAL_CONSTITUENT) constituent;
    if (((DOCUMENT_type) iconstituent->document == (DOCUMENT_type) 0) ||
	((DOCUMENT_type) iconstituent->document == ERROR_DOCUMENT)) {
	rval = ERROR_DOCUMENT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[GetDocument]: Internal error: found ERROR" );
	dummy = TKERR_Format( " or null document in constituent" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (DOCUMENT_type) iconstituent->document;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}




static INT_type SetStyleAttr(constituent, attribute, value, mask, styleAttribute)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN POINTER_type	value;
    IN PARM_MASK_type	mask;
    IN INT_type		styleAttribute;
{

    INT_type	rval;
    CONSTITUENT	styleConstituent;
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (HasAttr(constituent, styleAttribute)) {
	   case HAS_ATTR:
	       rval = GetAttr(constituent, styleAttribute,
			      (POINTER_type) &styleConstituent,
			      /* not inherited */ BOOL_false,
			      ((PARM_MASK_type *) 0));
	       break;

	   case NO_ATTR:
	       styleConstituent = MakeStyle(GetDocument(constituent),
					    styleAttribute == at_LAYOUT_STYLE ?
					    LAYOUT_STYLE : PRESENTATION_STYLE);
	       if (styleConstituent == ERROR_CONSTITUENT) {
		   rval = ERROR_INT;
	       }
	       else {
		   rval = SetAttr(constituent, styleAttribute,
				  (POINTER_type) styleConstituent,
				  ALL_PARMS);
	       }
	       break;

	   case ERROR_INT:
	       rval = ERROR_INT;
	       goto LEAVE;

	   default:
	       TKError = INTERNAL_err;
	       dummy = TKERR_StartErrMsg();
	       dummy = TKERR_Format( "[SetStyleAttr]: Unknown HasAttr" );
	       dummy = TKERR_FormatInt(" value %d", rval);
	       dummy = TKERR_EndErrMsg();
	       rval = ERROR_INT;
	       goto LEAVE;
    }

    if (rval != ERROR_INT) {
	rval = SetAttr( styleConstituent, attribute, value, mask);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );


}



INT_type SetPresAttr(constituent, attribute, value, mask)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN POINTER_type	value;
    IN PARM_MASK_type	mask;
{

    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SetStyleAttr( constituent, attribute, value,
			 mask, at_PRESENTATION_STYLE);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );


}



INT_type SetLayoutAttr(constituent, attribute, value, mask)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN POINTER_type	value;
    IN PARM_MASK_type	mask;
{

    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SetStyleAttr( constituent, attribute, value,
			 mask, at_LAYOUT_STYLE);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );


}

