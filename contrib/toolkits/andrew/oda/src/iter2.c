
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter2.c,v 1.3 89/10/31 15:07:26 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter2.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter2.c,v $
 * Revision 1.3  89/10/31  15:07:26  annm
 * fixing lint problmes
 * 
 * Revision 1.2  89/10/26  14:10:47  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:32:11  mss
 * Initial revision
 * 
 * Revision 1.15  89/04/24  14:29:14  mss
 * Fixed typo in error message;
 * Added new code and rearranged some old code
 * to accomodate style iterators in NextConstituent
 * 
 * Revision 1.14  89/01/24  12:16:41  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.13  89/01/09  13:52:51  jr
 * Align with new API.
 * 
 * Revision 1.12  88/12/23  16:52:53  mss
 * Cleaned up some error handling;
 * Moved generic iteration processing into iter3
 * Added some more error checking
 * 

*/



#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter2.c,v $";
#endif

/* iter2.c: main module for performing logical/layout iteration */



#define TK_IMPL_ITER
#define TK_GVARS_ITER2
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
#undef TK_GVARS_ITER2
#undef TK_IMPL_ATTR
#undef TK_IMPL_CONST
#undef TK_DEBUG
#undef TK_MM
#undef TK_IMPL_DOC
#undef TK_TKI_TKERR


#define	INITIALNUMOFELTS    ((INT_type) 10)

/* Local forward declarations */

static INT_type ITER_ShiftDownNodeArray(/*iterator,amount*/);
static INT_type ITER_ShiftUpNodeArray(/*iterator,amount*/);
static INT_type ITER_AddKidsToQueue(/*iter*/);
static INT_type ITER_AddSpecificKids(/*iter*/);
static CONSTITUENT ITER_NextSubgraphConstituent(/*iter*/);


ITERATOR_type MakeSubgraphIterator(root, parent_order, walk_order)
    IN CONSTITUENT  root;
    IN VALUE_type   parent_order;
    IN VALUE_type   walk_order;
{

    ITERATOR_type	    rval;	/* the returned value */
    INTERNAL_ITERATOR_type  irval;	/* internal version of return value */
    INTERNAL_CONSTITUENT    rootNode;	/* coerced root */
    INT_type		    ignoreError;/* Place to hold ignored status */
    INT_type		    dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Validate the parameters */
    /* Garbage for the root node? */
    if ( (ERROR_CONSTITUENT == root) || (NULL_CONSTITUENT == root) ||
	 ((CONSTITUENT) 0 == root)){
	rval = ERROR_ITERATOR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: NULL or ");
	dummy = TKERR_Format("ERROR constituent passed as root");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rootNode = (INTERNAL_CONSTITUENT) root;
    /* Is it a node in the logical or layout tree? */
    if ( (CONST_IsLogical(rootNode) == BOOL_false) &&
	 (CONST_IsLayout(rootNode) == BOOL_false)) {
	rval = ERROR_ITERATOR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: Root is ");
	dummy = TKERR_Format("neither a logical nor a layout ");
	dummy = TKERR_Format("constituent");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Is parent_order value OK? */
    if ( (parent_order != PARENTS_FIRST) &&
	 (parent_order != CHILDREN_FIRST) ) {
	rval = ERROR_ITERATOR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: Parent_order ");
	dummy = TKERR_FormatInt("has illegal value %d",
				(INT_type) parent_order);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Is walk_order value OK? */
    if ( (walk_order != BREADTH_FIRST) && (walk_order != DEPTH_FIRST)) {
	rval = ERROR_ITERATOR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: Walk_order ");
	dummy = TKERR_FormatInt("has illegal value %d", (INT_type) walk_order);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Parameters OK, build the necessary block */
    irval = (INTERNAL_ITERATOR_type)MM_Malloc(
		    (INT_type) sizeof(ITER_state_type));
    if ( ( (POINTER_type) irval) == ERROR_POINTER) {
	rval = ERROR_ITERATOR;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: Could not ");
        dummy = TKERR_Format("allocate space for new iterator");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Fill in everything for the next iteration */
    irval->before_kids = parent_order;
    irval->breadth_first = walk_order;
    if (CONST_IsSpecific(rootNode)) {
	irval->constituentType = SpecificTree;
    }
    else if (CONST_IsGeneric(rootNode)) {
	irval->constituentType = GenericTree;
    }
    else {
	/* In theory, we already checked for styles, so everything else is illegal */
	/* Already have error, so only one error return */
	ignoreError = MM_Free((POINTER_type)irval);
	rval = ERROR_CONSTITUENT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: Internal ");
	dummy = TKERR_Format("error: Tree type neither specific ");
	dummy = TKERR_Format("nor generic");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    irval->iteratorRoot = rootNode;
    irval->nodeMarkerValue = ++last_debug_counter;
    /* Initialize the array of node to process */
    irval->nodeArrayActualSize = (INITIALNUMOFELTS-1);
    irval->nodeArrayFilledMark = 0;
    irval->nodeArray = (ITER_p_toBeVisited_type)
      MM_Malloc(INITIALNUMOFELTS*sizeof(ITER_toBeVisited_type));
    if (ERROR_POINTER == (POINTER_type) irval) {
        /* Couldn't get the storage, release what we have
                  and return error */
        ignoreError = MM_Free((POINTER_type) irval);
	rval = ERROR_CONSTITUENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[MakeSubgraphIterator]: Could not ");
	dummy = TKERR_Format("allocate memory for node queue for ");
	dummy = TKERR_Format("iterator");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    irval->nodeArray[0].objectToVisit = rootNode;
    irval->nodeArray[0].childrenAlreadyProcessed = BOOL_false;
    /* And mark the node as having been entered into the
           queue for processing */
    rootNode->debug_counter = irval->nodeMarkerValue;
    rval = (ITERATOR_type) irval;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


static CONSTITUENT ITER_NextSubgraphConstituent(iter)
    IN INTERNAL_ITERATOR_type	iter;
{
    CONSTITUENT	rval;		    /* the returned value */
    INT_type	dummy;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* See if anything left */
    if (iter->nodeArrayFilledMark < 0) {
	/* All done, no more nodes -- note we will return as many times as asked for */
	rval = NULL_CONSTITUENT;
	goto LEAVE;
    }

    /* Have the kids been done yet?, if not, get the kids */
    if (iter->nodeArray[0].childrenAlreadyProcessed == BOOL_false) {
	/* Go get the kids and place into queue */
	if (ERROR_INT == ITER_AddKidsToQueue(iter)) {
	    rval = ERROR_CONSTITUENT;
	    /* TKError set already */
	    goto LEAVE;
	}
    }

    /* See if we made it or have to go another round with a 
	prepended set of children */
    if (iter->nodeArray[0].childrenAlreadyProcessed == BOOL_true) {
	rval = (CONSTITUENT) iter->nodeArray[0].objectToVisit;
	if (ERROR_INT == ITER_ShiftDownNodeArray(iter,(INT_type)1)) {
	    rval = ERROR_CONSTITUENT;
	    /* TKError set already */
	    goto LEAVE;
	}
    }
    else {
	/* Do it again, this time with prepended children! */
	rval = NextConstituent((ITERATOR_type)iter);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );
}


CONSTITUENT NextConstituent(iterator)
    IN ITERATOR_type	iterator;
{
    /* put local dcl's here */

    CONSTITUENT	rval;		    /* the returned value */
    INTERNAL_ITERATOR_type  iter;   /* Internal version of iterator */
    INT_type	dummy;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Validate the parameter */
    if ( (iterator == ((ITERATOR_type) 0)) ||
	 (iterator == ERROR_ITERATOR)) {
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[NextConstituent]: NULL or ERROR ");
	dummy = TKERR_Format("iterator given as parameter");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Make a usable version of iterator state */
    iter = (INTERNAL_ITERATOR_type) iterator;

    switch(iter->constituentType){

	case PresentationStyleList:
	    if (iter->iteratorRoot == &(iter->iteratorRoot->document->presentation_styles_list)){
		rval = NULL_CONSTITUENT;
	    }
	    else {
		rval = (CONSTITUENT) iter->iteratorRoot;
		iter->iteratorRoot = iter->iteratorRoot->forward;
	    }
	    break;

	case LayoutStyleList:
	    if (iter->iteratorRoot == &(iter->iteratorRoot->document->layout_styles_list)){
		rval = NULL_CONSTITUENT;
	    }
	    else {
		rval = (CONSTITUENT) iter->iteratorRoot;
		iter->iteratorRoot = iter->iteratorRoot->forward;
	    }
	    break;

	case SpecificTree:
	case GenericTree:
	    /* Note: the following sets TKError appropriately */
	    rval = ITER_NextSubgraphConstituent(iter);
	    if (rval == ERROR_CONSTITUENT) {
		goto LEAVE;
	    }
	    break;

	default:
	    rval = ERROR_CONSTITUENT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[NextConstituent]: Internal error: ");
	    dummy = TKERR_Format("tree type neither specific nor ");
	    dummy = TKERR_FormatInt("generic=>%d", iter->constituentType);
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



INT_type DeleteIterator(iterator)
    IN ITERATOR_type	iterator;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned value */
    INTERNAL_ITERATOR_type  iter;
    INT_type	oldrval;    /* old error value pending another try */
    INT_type	oldTKError; /* old error value pending another try */
    INT_type	dummy;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Try to do a little error checking */
    if ( (iterator == (ITERATOR_type) 0) ||
	 (iterator == ERROR_ITERATOR)) {
	/* Garbage in, error out */
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[DeleteIterator]: NULL or ERROR ");
	dummy = TKERR_Format("iterator given as parameter");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Try to free the array */
    iter = (INTERNAL_ITERATOR_type) iterator;
    switch(iter->constituentType) {
	case GenericTree:
	case SpecificTree:
	    rval = MM_Free((POINTER_type)iter->nodeArray);
	    break;
	case PresentationStyleList:
	case LayoutStyleList:
	    rval = SUCCESS_INT;
	    break;
	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DeleteIterator]: Unknown iterator ");
	    dummy = TKERR_FormatInt("type %d", (INT_type) iter->constituentType);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
    }

    if (rval == ERROR_INT) {
	/* Try to preserve error condition */
	oldTKError = TKError;
	oldrval = rval;
	rval = MM_Free((POINTER_type) iter);
	if (rval != ERROR_INT) {
	    /* Return original error so user knows about it */
	    rval = oldrval;
	    TKError = oldTKError;
	}
    }
    else {
	rval = MM_Free((POINTER_type) iter);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


static INT_type ITER_ValidateArraySize(iter, desiredSize)
    IN OUT INTERNAL_ITERATOR_type	iter;
    IN	   INT_type			desiredSize;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned function value */
    POINTER_type    p;	    /* temp to hold returned realloc value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;
    if (iter->nodeArrayActualSize < desiredSize) {
        p = MM_Realloc((POINTER_type) iter->nodeArray,
                       (INT_type) ((desiredSize+1)*sizeof(ITER_toBeVisited_type)));
	if (p == ERROR_POINTER) {
	    rval = ERROR_INT;
	    /* TKError already set */
	    goto LEAVE;
	}
	iter->nodeArray = (ITER_p_toBeVisited_type) p;
	iter->nodeArrayActualSize = desiredSize;
    }

    /* Otherwise we have enough, so do nothing */

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



static INT_type ITER_ShiftDownNodeArray(iter, amount)
    IN OUT INTERNAL_ITERATOR_type	iter;
    IN	   INT_type			amount;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned value */
    INT_type	i;	    /* Loop index */
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;
    if (amount > (iter->nodeArrayFilledMark+1)) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[ITER_ShiftDownNodeArray]: Internal ");
	dummy = TKERR_Format("error: end of queue beyond ");
        dummy = TKERR_FormatInt("allocated queue elements %d",amount);
	dummy = TKERR_FormatInt(" > %d",
		        (INT_type) (iter->nodeArrayFilledMark+1) );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    for(i=0;i<=(iter->nodeArrayFilledMark-amount);i++) {
        iter->nodeArray[i].childrenAlreadyProcessed =
	  iter->nodeArray[i+amount].childrenAlreadyProcessed;
	iter->nodeArray[i].objectToVisit =
	  iter->nodeArray[i+amount].objectToVisit;
    }
    iter->nodeArrayFilledMark = iter->nodeArrayFilledMark - amount;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



static INT_type ITER_ShiftUpNodeArray(iter, amount)
    IN OUT INTERNAL_ITERATOR_type	iter;
    IN	   INT_type			amount;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned value */
    INT_type	i;	    /* Loop index */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    if (ERROR_INT == ITER_ValidateArraySize(iter,
			iter->nodeArrayFilledMark+amount)) {
	rval = ERROR_INT;
	/* TKError already set */
	goto LEAVE;
    }

    for(i=iter->nodeArrayFilledMark;i>=0;i--) {
        iter->nodeArray[i+amount].childrenAlreadyProcessed =
	  iter->nodeArray[i].childrenAlreadyProcessed;
	iter->nodeArray[i+amount].objectToVisit =
	  iter->nodeArray[i].objectToVisit;
    }
    iter->nodeArrayFilledMark = iter->nodeArrayFilledMark+amount;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



static INT_type ITER_AddKidsToQueue(iter)
    IN OUT INTERNAL_ITERATOR_type	iter;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned value */
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Mark parent as having been processed */
    iter->nodeArray[0].childrenAlreadyProcessed = BOOL_true;

    switch(iter->constituentType) {
	case GenericTree:
	    rval = ITER_AddGenericKids(iter);
	    break;
	case SpecificTree:
	    rval = ITER_AddSpecificKids(iter);
	    break;
	case PresentationStyleList:
	case LayoutStyleList:
	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
            dummy = TKERR_Format("[ITER_AddKidsToQueue]: Internal ");
	    dummy = TKERR_Format("error: unknown constituent ");
	    dummy = TKERR_FormatInt("type %d",iter->constituentType);
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




static INT_type ITER_AddSpecificKids(iter)
    IN OUT INTERNAL_ITERATOR_type	iter;
{

    /* put local dcl's here */

    INT_type	rval;	    /* the returned value */
    INT_type	i;	    /* Loop index */
    ATTR_attribute_t	    attr;
    at_SUBORDINATES_type    kidSequence;
    ITER_p_toBeVisited_type tempNodes;
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* The specific kids are found in at_SUBORDINATES */
    if (!CONST_UnsafeHasAttr(iter->nodeArray[0].objectToVisit,
			      at_SUBORDINATES)) {
	/* No kids, so all done */
	goto LEAVE;
    }
    /* Get the attribute */
    attr = ATTR_FindAttr( (CONSTITUENT) iter->nodeArray[0].objectToVisit,
			  at_SUBORDINATES);
    if (attr == ERROR_ATTRIBUTE) {
	    rval = ERROR_INT;
	    goto LEAVE;
    }
    kidSequence = GET_SEQUENCE_CONSTITUENT(attr);
    tempNodes = (ITER_p_toBeVisited_type)
      MM_Malloc((kidSequence->length)*sizeof(ITER_toBeVisited_type));
    if (ERROR_POINTER == (POINTER_type) tempNodes) {
	/* Couldn't get the storage, return error */
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[ITER_AddSpecificKids]: Could not ");
	dummy = TKERR_Format("allocate temporary queue for pending ");
	dummy = TKERR_Format("nodes");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    /* Copy over the kids for later merging */
    for(i=0;i<kidSequence->length;i++){
	if(((INTERNAL_CONSTITUENT)(kidSequence->sequence_value.constituents[i])) -> debug_counter == 
	   iter->nodeMarkerValue){
	    rval = ERROR_INT;
	    TKError = WALK_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ITER_AddSpecificKids]: Specific node ");
	    dummy = TKERR_Format("visited more than once");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
        tempNodes[i].objectToVisit = (INTERNAL_CONSTITUENT)
	  kidSequence->sequence_value.constituents[i];
	tempNodes[i].objectToVisit->debug_counter = iter->nodeMarkerValue;
        tempNodes[i].childrenAlreadyProcessed = BOOL_false;

    }

    /* process kids and current node together */
    if (ERROR_INT == ITER_AddKidsToParent(iter,
                                   tempNodes,kidSequence->length)) {
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



INT_type ITER_AddKidsToParent(iter,tempNodes, numKids)
    IN OUT INTERNAL_ITERATOR_type	iter;
    IN ITER_p_toBeVisited_type		tempNodes;
    IN INT_type				numKids;
{

    /* put local dcl's here */
    INT_type                rval;            /* the returned value */
    INT_type                arrayIndex, kidIndex;/* Loop index */
    /* Save parent node for queue rearrangement */
    INTERNAL_CONSTITUENT    curParentNode;	
    INT_type		    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;


    /* First make sure there is room to add kids */
    if (ERROR_INT == ITER_ValidateArraySize(iter,
                             iter->nodeArrayFilledMark+numKids)) {
	rval = ERROR_INT;
	/* TKError already set */
	goto LEAVE;
    }

    /* Four cases to consider: parent before, parent after, breadth, depth */
    if ((iter->before_kids==PARENTS_FIRST) &&
	 (iter->breadth_first == BREADTH_FIRST) ) {
	/* new array = parent rest kids , therefore, just append kids */
        for(arrayIndex=iter->nodeArrayFilledMark+1,kidIndex=0;
            kidIndex<numKids; arrayIndex++, kidIndex++) {
	    iter->nodeArray[arrayIndex].objectToVisit =
	      tempNodes[kidIndex].objectToVisit;
            iter->nodeArray[arrayIndex].childrenAlreadyProcessed =
	      BOOL_false;
	}
	/* And adjust filled point */
	iter->nodeArrayFilledMark =
	  iter->nodeArrayFilledMark + numKids;
    }
    else if ((iter->before_kids==PARENTS_FIRST) &&
	      (iter->breadth_first == DEPTH_FIRST) ) {
	/* new array = parent kids rest, so shift rest
	    down for kids, then add parent and kids */
        /* remember parent -- not strictly necessary since current
	  shift implementation is non-destructive, but replacement
	  might be different */
	curParentNode = iter->nodeArray[0].objectToVisit;
	if (ERROR_INT == ITER_ShiftUpNodeArray(iter, numKids)) {
	    rval = ERROR_INT;
	    /* TKError already set */
	    goto LEAVE;
	}
	/* Everything shifted up, now fill in necessary values -- note:
	    overwriting shifted parent value with last kid) */
	iter->nodeArray[0].objectToVisit = curParentNode;
	iter->nodeArray[0].childrenAlreadyProcessed = BOOL_true;
	for (arrayIndex=1, kidIndex=0; kidIndex<numKids;
	     arrayIndex++, kidIndex++){
            iter->nodeArray[arrayIndex].objectToVisit =
	      tempNodes[kidIndex].objectToVisit;
            iter->nodeArray[arrayIndex].childrenAlreadyProcessed =
	      BOOL_false;
	}
    }
    else if ((iter->before_kids==PARENTS_FIRST) &&
	      (iter->breadth_first == BREADTH_FIRST) ) {
	/* unimplemented */
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[ITER_AddKidsToParent]: Post-order ");
        dummy = TKERR_Format("Breadth-first search (parents after ");
        dummy = TKERR_Format("children) is not implemented");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else if ((iter->before_kids==PARENTS_FIRST) &&
	      (iter->breadth_first == DEPTH_FIRST) ) {
	/* new array = kids, parent rest, so shift up same for kids */
	/* and fill them in */
	if (ERROR_INT == ITER_ShiftUpNodeArray(iter, numKids)) {
	    rval = ERROR_INT;
	    /* TKError already set */
	    goto LEAVE;
	}
	/* Everything shifted up, now prepend kids -- could use same index, but
	    might change later) */
	for (arrayIndex=0, kidIndex=0; kidIndex<numKids;
	     arrayIndex++, kidIndex++){
	    iter->nodeArray[arrayIndex].objectToVisit =
	      tempNodes[kidIndex].objectToVisit;
            iter->nodeArray[arrayIndex].childrenAlreadyProcessed =
	      BOOL_false;
	}
    }
    else {
	/* internal error */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
        dummy=TKERR_Format("[ITER_AddKidsToParent]: Internal error:");
	dummy=TKERR_FormatInt(" illegal walk order (%d) ",
			      (INT_type) iter->breadth_first);
	dummy = TKERR_FormatInt("or parent order (%d)",
				(INT_type) iter->before_kids);
	dummy = TKERR_EndErrMsg();
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
