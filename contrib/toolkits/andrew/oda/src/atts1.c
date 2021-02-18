
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts1.c,v 1.1 89/09/21 06:25:17 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts1.c,v $

(c) Copyright 1989 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	atts1.c,v $
 * Revision 1.1  89/09/21  06:25:17  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/12  09:11:51  mss
 * Removed #
 * 
 * Revision 1.2  89/03/27  12:32:24  mss
 * Shortedn name of parameter to less tahn 31 chars
 * 
 * Revision 1.1  89/03/21  17:13:17  mss
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts1.c,v $";
#endif



#define TK_IMPL_ATTS
#define TK_GVARS_ATTS1
#define TK_TKI_TKERR

#include <atts.h>
#include <acd.h>
#include <values.h>
#include <parms.h>

#undef TK_IMPL_ATTS
#undef TK_GVARS_ATTS1
#undef TK_TKI_TKERR

/*

    This file contains some utility procedures for attribute structures

*/



INT_type UnspecifyDefaultValueLists(default_value_lists)
	IN OUT at_DEFAULT_VALUE_LISTS_type *default_value_lists;
{

    INT_type rval;
    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if (default_value_lists == (at_DEFAULT_VALUE_LISTS_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[UnspecifyDefaultValueLists]: null pointer ");
	dummy = TKERR_Format("passed for default value lists");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    default_value_lists->page_attributes =
        default_value_lists->frame_attributes =
        default_value_lists->block_attributes =
        default_value_lists->composite_logical_attributes =
	default_value_lists->basic_logical_attributes =
	NULL_CONSTITUENT;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


static INT_type ATTS_FillInOneDefaultConst(doc, ref_constituent,
					   create_flag, object_type)
	IN DOCUMENT_type doc;
	IN OUT CONSTITUENT *ref_constituent;
	IN PARM_MASK_type create_flag;
	IN VALUE_type object_type;
{
    INT_type	rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if (create_flag) {
	*ref_constituent = MakeComponent(doc, object_type, DVL_COMPONENT);
	if (*ref_constituent == ERROR_CONSTITUENT) {
	    rval = ERROR_INT; /* TKErr and message should have already been set */
	    goto LEAVE;
	}
    }
    else {
	*ref_constituent = NULL_CONSTITUENT;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );


}




INT_type InitializeDefaultValueLists(doc,default_value_lists,constituent_mask)
	IN DOCUMENT_type doc;
	IN OUT at_DEFAULT_VALUE_LISTS_type *default_value_lists;
	IN PARM_MASK_type constituent_mask;
{

    INT_type	rval;
    INT_type	dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    /* Validate the document parameter */
    if ( (doc == ERROR_DOCUMENT) || (doc == (DOCUMENT_type) 0)) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[InitializeDefaultValueList]: null pointer ");
	dummy = TKERR_Format("or ERROR_DOCUMENT passed for document");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Validate the attribute parameter */
    if (default_value_lists == (at_DEFAULT_VALUE_LISTS_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[InitializeDefaultValueList]: null pointer ");
	dummy = TKERR_Format("passed for default value lists");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Initial validation of the mask parameter */
    if (constituent_mask == (PARM_MASK_type) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[InitializeDefaultValueList]: null value ");
	dummy = TKERR_Format("passed for constituent parameter mask");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /*  Rearrange the parm mask parameter */
    if (constituent_mask == ALL_PARMS) {
	constituent_mask = PAGE_ATTRIBUTES_parm | FRAME_ATTRIBUTES_parm |
	                   BLOCK_ATTRIBUTES_parm | COMP_LOGICAL_ATTRIBUTES_parm |
	                   BAS_LOGICAL_ATTRIBUTES_parm;
    }
    else if (constituent_mask == NO_PARMS) {
	constituent_mask = (PARM_MASK_type) 0;
    }

    if (constituent_mask & ~(PAGE_ATTRIBUTES_parm | FRAME_ATTRIBUTES_parm |
	                   BLOCK_ATTRIBUTES_parm | COMP_LOGICAL_ATTRIBUTES_parm |
	                   BAS_LOGICAL_ATTRIBUTES_parm)) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[InitializeDefaultValueList]: extraneous ");
	dummy = TKERR_Format("bits enabled in constituent parameter mask");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Now add the constituents, one at a time */
    rval = ATTS_FillInOneDefaultConst(doc, &default_value_lists->page_attributes,
	(PARM_MASK_type) constituent_mask & PAGE_ATTRIBUTES_parm, at_OBJECT_TYPE_page);
    if (rval != ERROR_INT) {
	rval = ATTS_FillInOneDefaultConst(doc,
		     &default_value_lists->frame_attributes,
	            (PARM_MASK_type) constituent_mask & FRAME_ATTRIBUTES_parm,
		     at_OBJECT_TYPE_frame);
    }
    if (rval != ERROR_INT) {
	rval = ATTS_FillInOneDefaultConst(doc,
                     &default_value_lists->block_attributes,
		     (PARM_MASK_type) constituent_mask & BLOCK_ATTRIBUTES_parm,
		      at_OBJECT_TYPE_block);
    }
    if (rval != ERROR_INT) {
	rval = ATTS_FillInOneDefaultConst(doc,
		      &default_value_lists->composite_logical_attributes,
		      (PARM_MASK_type) constituent_mask & COMP_LOGICAL_ATTRIBUTES_parm,
		      at_OBJECT_TYPE_comp_logical_obj);
    }
    if (rval != ERROR_INT) {
	rval = ATTS_FillInOneDefaultConst(doc,
		      &default_value_lists->basic_logical_attributes,
		      (PARM_MASK_type) constituent_mask & BAS_LOGICAL_ATTRIBUTES_parm,
		      at_OBJECT_TYPE_bas_logical_obj);
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTS_Final()
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Nothing to do */

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTS_Init()
{

    INT_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* nothing to do */

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
