
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter3.c,v 1.3 89/10/31 15:09:17 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter3.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter3.c,v $
 * Revision 1.3  89/10/31  15:09:17  annm
 * fixing lint problems
 * 
 * Revision 1.2  89/10/26  14:11:06  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:32:32  mss
 * Initial revision
 * 
 * Revision 1.3  89/01/24  12:16:52  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.2  89/01/09  13:53:12  jr
 * Align with new API.
 * 
 * Revision 1.1  88/12/23  16:54:14  mss
 * Initial revision
 * 

*/



#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter3.c,v $";
#endif

/* iter3.c: main module for performing generic iteration */



#define TK_IMPL_ITER
#define TK_GVARS_ITER3
#define TK_IMPL_ATTR
#define TK_IMPL_CONST
#define TK_DEBUG
#define TK_MM
#define TK_IMPL_DOC
#define TK_TKI_TKERR

#include <mm.h>
#include <iter.h>
#include <tkerr.h>
#include <tags.h>

#undef TK_IMPL_ITER
#undef TK_GVARS_ITER3
#undef TK_IMPL_ATTR
#undef TK_IMPL_CONST
#undef TK_DEBUG
#undef TK_MM
#undef TK_IMPL_DOC
#undef TK_TKI_TKERR



/* Local forward declarations */
static INT_type ITER_WalkConsExpr();
static INT_type ITER_WalkConsTerm();
static INT_type ITER_WalkSeqConsTerm();
static INT_type ITER_WalkConsFactor();
static INT_type ITER_GetObjectID();


INT_type ITER_AddGenericKids(iter)
IN OUT INTERNAL_ITERATOR_type	iter;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned value */
    ATTR_attribute_t	    attr;
    at_GEN_FOR_SUBORDINATES_type    *kidTree;
    ITER_p_toBeVisited_type tempNodes;
    INT_type		    numTempNodes;
    INT_type		    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* The specific kids are found in at_GEN_FOR_SUBORDINATES */
    if (!CONST_UnsafeHasAttr(iter->nodeArray[0].objectToVisit,
			      at_GEN_FOR_SUBORDINATES)) {
	/* No kids, so all done */
	goto LEAVE;
    }
    /* Get the attribute */
    attr = ATTR_FindAttr(
		(CONSTITUENT) iter->nodeArray[0].objectToVisit,
			  at_GEN_FOR_SUBORDINATES);
    if (attr == ERROR_ATTRIBUTE) {
	    rval = ERROR_INT;
	    goto LEAVE;
    }
    kidTree = GET_CONSTRUCTION_EXPR(attr);
    /* Note: this next statement depend on MM_Malloc
      returning a pointer at nothing. It exists so that
      the iter module has a legal pointer to give
      to realloc later */
    tempNodes = (ITER_p_toBeVisited_type)
      MM_Malloc((INT_type) (0*sizeof(ITER_toBeVisited_type)));
    numTempNodes = 0;
    if (ERROR_POINTER == (POINTER_type) tempNodes) {
	/* Couldn't get the storage, return error */
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[ITER_AddGenericKids]: Could not ");
	dummy = TKERR_Format("allocate initial queue size for ");
	dummy = TKERR_Format("nodes");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = ITER_WalkConsExpr(kidTree, iter, &tempNodes, &numTempNodes);
    if (rval == ERROR_INT) {
	/* TKError, TKErrorMsg should already be set */
	goto LEAVE;
    }

    /* process kids and current node together */
    if (ERROR_INT == ITER_AddKidsToParent(iter,
			tempNodes, numTempNodes)) {
	rval = ERROR_INT;
	/* TKError already set */
	goto LEAVE;
    }
    /* Get rid of temporary copy of nodes */
    rval == MM_Free((POINTER_type) tempNodes);


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

static INT_type ITER_WalkConsExpr(expr, iter, refTempNodes, refNumTempNodes)
IN CONSTRUCTION_EXPR_type	*expr;
IN OUT INTERNAL_ITERATOR_type	iter;
IN OUT ITER_p_toBeVisited_type	*refTempNodes;
IN OUT INT_type			*refNumTempNodes;
{
    INT_type		    rval;
    INT_type		    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (expr->construction_tag) {

	case TERM_tag:
	    rval = ITER_WalkConsTerm(&expr->construction_value.term,
				      iter, refTempNodes, refNumTempNodes);
	    break;

	case SEQUENCE_tag:
	case AGGREGATE_tag:
	case CHOICE_tag:
	    rval = ITER_WalkSeqConsTerm(expr->construction_value.construction,
					iter, refTempNodes, refNumTempNodes);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ITER_WalkConsExpr]: Illegal ");
	    dummy = TKERR_FormatInt("construction expression tag %d",
				(INT_type) expr->construction_tag);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


static INT_type ITER_WalkConsTerm(term, iter, refTempNodes, refNumTempNodes)
IN CONSTRUCTION_TERM_type	*term;
IN OUT INTERNAL_ITERATOR_type	iter;
IN OUT ITER_p_toBeVisited_type	*refTempNodes;
IN OUT INT_type			*refNumTempNodes;
{
    INT_type		    rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ITER_WalkConsFactor(&term->factor,iter, 
				 refTempNodes, refNumTempNodes);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type ITER_WalkSeqConsTerm(seq, iter, refTempNodes, refNumTempNodes)
IN SEQUENCE_CONSTRUCTION_TERM_type seq;
IN OUT INTERNAL_ITERATOR_type	iter;
IN OUT ITER_p_toBeVisited_type	*refTempNodes;
IN OUT INT_type			*refNumTempNodes;
{
	     INT_type		    rval;
    REGISTER CONSTRUCTION_TERM_type *terms;
    REGISTER INT_type		    length;
    REGISTER INT_type		    i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    length = seq -> length;
    terms = seq -> sequence_value.construction_terms;
    rval = SUCCESS_INT;
    for (i=0; i<length && rval!=ERROR_INT; i++) {
	rval = ITER_WalkConsTerm(&terms[i],iter, 
				 refTempNodes, refNumTempNodes);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif


    return rval;
}

static INT_type ITER_WalkConsFactor(factor, iter, refTempNodes, refNumTempNodes)
IN CONSTRUCTION_FACTOR_type	*factor;
IN OUT INTERNAL_ITERATOR_type	iter;
IN OUT ITER_p_toBeVisited_type	*refTempNodes;
IN OUT INT_type			*refNumTempNodes;
{
    INT_type		    rval;
    INT_type		    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (factor->factor_tag) {

	case OBJECT_CLASS_ID_tag:
	    rval = ITER_GetObjectID((INTERNAL_CONSTITUENT) factor->factor_value.object_class_id,
	      iter, refTempNodes, refNumTempNodes); 
	    break; 

	case EXPR_tag:
	    rval = ITER_WalkConsExpr(factor->factor_value.expr,
				     iter, refTempNodes, refNumTempNodes);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ITER_WalkConsFactor]: Illegal ");
	    dummy = TKERR_FormatInt("factor tag %d",
				(INT_type) factor->factor_tag);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif


    return rval;
}


static INT_type ITER_GetObjectID(node, iter, refTempNodes, refNumTempNodes)
IN INTERNAL_CONSTITUENT		node;
IN OUT INTERNAL_ITERATOR_type	iter;
IN OUT ITER_p_toBeVisited_type	*refTempNodes;
IN OUT INT_type			*refNumTempNodes;
{
    INT_type			    rval;
    INT_type			    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* See if we've already been here */
    if (node->debug_counter == iter->nodeMarkerValue){
	goto LEAVE;
    }

    /* Add it walked list */
    node->debug_counter = iter->nodeMarkerValue;

    /* Add to list of children of the parent node */
    *refTempNodes = (ITER_p_toBeVisited_type)
      MM_Realloc( (POINTER_type) *refTempNodes,
		  (INT_type) ((*refNumTempNodes + 1)*sizeof(ITER_toBeVisited_type)));
    if (ERROR_POINTER == (POINTER_type) *refTempNodes) {
	/* Couldn't get the storage, return error */
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[ITER_GetObjectID]: Could not ");
	dummy = TKERR_Format("expand children array");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    (*refTempNodes)[*refNumTempNodes].objectToVisit = (INTERNAL_CONSTITUENT) node;
    (*refTempNodes)[*refNumTempNodes].childrenAlreadyProcessed = BOOL_false;
    (*refNumTempNodes)++;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

