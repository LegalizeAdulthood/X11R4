
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/label1.c,v 1.5 89/11/01 15:50:56 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/label1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	label1.c,v $
 * Revision 1.5  89/11/01  15:50:56  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.4  89/10/31  15:19:37  annm
 * fixing lint problems
 * 
 * Revision 1.3  89/10/26  14:11:20  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/10/16  13:05:43  annm
 * fixed bad TKERR macro calls 
 * 
 * Revision 1.1  89/09/21  06:33:27  mss
 * Initial revision
 * 
 * Revision 1.22  89/05/13  16:45:41  mss
 * Added error messages for error conditions;
 * Added garbage collection of labels
 * 
 * Revision 1.21  89/05/11  16:50:35  annm
 * removed delete of sequence in AssignIdAttr - was zapping the
  * root id causing label to fail
 * 
 * Revision 1.20  89/01/24  12:17:14  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.19  88/11/10  10:54:58  mss
 * Removed call to RemAttr when a label already
 * exists. (Call to SetAttr will automatically
 * remove it.)
 * 
 * Revision 1.18  88/11/09  17:37:38  jr
 * Fix bug in inc'ing counter.
 * 
 * Revision 1.17  88/09/06  13:12:31  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.16  88/09/06  12:02:31  annm
 * no change
 * 
 * Revision 1.15  88/08/10  12:33:42  jr
 * Fix up labeling of recursive
 * structures.
 * 
 * Revision 1.14  88/08/09  13:46:23  jr
 * Walk generic structure now.
 * 
 * Revision 1.13  88/08/07  20:39:42  jr
 * Labeling generic structures now
 * works.
 * 
 * Revision 1.12  88/07/28  13:57:54  jr
 * Remove #if[n]def 0
 * 
 * Revision 1.11  88/07/05  16:56:46  jr
 * Does lots more
 * 
*/

#define TK_IMPL_LABEL
#define TK_GVARS_LABEL1
#define TK_IMPL_DOC
#define TK_IMPL_CONST
#define TK_IMPL_ATTR
#define TK_TKI_STR
#define TK_TKI_FMT
#define TK_TKI_DEBUG
#define TK_TKI_TKERR

#include <parms.h>
#include <str.h>
#include <label.h>
#include <acd.h>
#include <fmt.h>
#include <debug.h>
#include <tkerr.h>

#undef TK_IMPL_LABEL
#undef TK_GVARS_LABEL1
#undef TK_IMPL_DOC
#undef TK_IMPL_CONST
#undef TK_IMPL_ATTR
#undef TK_TKI_STR
#undef TK_TKI_FMT
#undef TK_TKI_DEBUG

/* Forward declarations */
static INT_type	LabelSpecificStructure();
static INT_type	WalkAndLabelSpecificStructure();
static INT_type	AssignIdAttr();
static STRING_type	CreateAndAssignId();
static INT_type	LabelContentPortions();
static INT_type	Ndigits();
static INT_type	LabelGenericStructure();
static INT_type	WalkAndLabelGenericStructure();
static INT_type	LabelConstructionExpr();
static INT_type	LabelConstructionTerm();
static INT_type	LabelSeqConstructionTerm();
static INT_type	LabelConstructionFactor();

INT_type LabelStructure(root)
    IN CONSTITUENT	root;
{
    INT_type		    rval;   /* The returned value */
    INTERNAL_CONSTITUENT    iroot;  /* Internal type for root */
    INT_type		    dummy;  /* For error messages */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    iroot = (INTERNAL_CONSTITUENT) root;

    /* Make sure that constituent is reasonable! */
    if ((root == NULL_CONSTITUENT) || (root == (CONSTITUENT) 0) ||
	 (root == ERROR_CONSTITUENT)){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	    TKERR_Format("[LabelStructure]: Constituent is a null or error constituent");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Make sure arg is a root */
    if (!CONST_IsDocLayRoot(iroot) && !CONST_IsDocLogRoot(iroot)) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	    TKERR_Format("[LabelStructure]: Constituent is not a root");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* For now, only handle specific structures */
    if (CONST_IsDocLayRootCl(iroot) || CONST_IsDocLogRootCl(iroot)) {
	++last_debug_counter;
	rval = LabelGenericStructure(iroot, last_debug_counter);
    } else {
	/* Pass buck to label specific structure */
	rval = LabelSpecificStructure(iroot);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type LabelStyles(document, islayout)
    IN DOCUMENT_type	document;
    IN BOOL_type	islayout;
{
	     INT_type		    rval;    /* The returned value */
    REGISTER INTERNAL_DOCUMENT_type idocument;
    REGISTER INTERNAL_CONSTITUENT   base;
    REGISTER INTERNAL_CONSTITUENT   style;
    REGISTER STRING_type	    id;
    REGISTER INT_type		    attrid;
    REGISTER INT_type		    i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Assign base string for identifier & get pointer to base of linked list */
    idocument = (INTERNAL_DOCUMENT_type) document;

    if (islayout) {
	base = &idocument->layout_styles_list;
	attrid = at_LAYOUT_STYLE_IDENTIFIER;
	id = Chars2String("4");
    } else {
	base = &idocument->presentation_styles_list;
	attrid = at_PRESENTATION_STYLE_ID;
	id = Chars2String("5");
    }
    if (id == ERROR_STRING) {
	rval = ERROR_INT;
	/* TKError set by Chars2String */
	goto LEAVE;
    }

    /* Step through list & assign id's */
    for (i=0, style=base->forward; style!=base; i++, style=style->forward) {
	if (CreateAndAssignId(attrid, style, i, id) == ERROR_STRING) {
	    /* TKError set by CreateAndAssignId -- ignore error return of Delete */
	    rval = DeleteString(id);
	    rval = ERROR_INT;
	    goto LEAVE;
	}
    }

    /* If here, it went okay */
    rval = DeleteString(id);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type LabelDocument(document)
    IN DOCUMENT_type	document;
{
	     INT_type		    rval;    /* The returned value */
    REGISTER INTERNAL_DOCUMENT_type doc;
    REGISTER INT_type		    i;
	     CONSTITUENT	    roots[4];

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    doc = (INTERNAL_DOCUMENT_type) document;

    roots[0] = doc->generic_logical_root;
    roots[1] = doc->specific_logical_root;
    roots[2] = doc->generic_layout_root;
    roots[3] = doc->specific_layout_root;

    /* Loop & do it */
    rval = SUCCESS_INT;
    for (i=0; i<4; i++) {
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
        if (roots[i] != NULL_CONSTITUENT) {
	    rval = LabelStructure(roots[i]);
	}
    }

    /* Do the styles */
    if (rval != ERROR_INT) {
	if (doc->layout_styles_list.forward != &doc->layout_styles_list) {
	    rval = LabelStyles(document, BOOL_true);
	}
    }
    if (rval != ERROR_INT) {
	if (doc->presentation_styles_list.forward !=
		    &doc->presentation_styles_list) {
	    rval = LabelStyles(document, BOOL_false);
	}
    }	    

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type LABEL_Final()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type LABEL_Init()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

/********************\
* 		     *
* Internal routines  *
* 		     *
\********************/

static INT_type LabelSpecificStructure(root)
    IN INTERNAL_CONSTITUENT root;
{
    INT_type	rval;	    /* The returned value */
    BOOL_type	layout;
    STRING_type	id;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if layout or logical */
    layout = CONST_IsDocLayRootNc(root);

    /* Construct root identifier */
    id = Chars2String(layout ? "1" : "3");
    if (id == ERROR_STRING) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Assign identifier to root */
    rval = AssignIdAttr(root, at_OBJECT_IDENTIFIER, id);

    /* Have whole structure labeled */
    if (rval != ERROR_INT) {
	rval = WalkAndLabelSpecificStructure(root, layout, id);
    }

    /* Done with root's label */
    rval = DeleteString(id);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

/* Node has already been labeled */

static INT_type WalkAndLabelSpecificStructure(node, layout, id)
    IN OUT INTERNAL_CONSTITUENT	node;
    IN     BOOL_type		layout;
    IN	   STRING_type		id;
{
    INT_type			rval;
    ATTR_attribute_t		attr;
    SEQUENCE_CONSTITUENT_type	sons;
    REGISTER INT_type		i;
    STRING_type			newid;
    INT_type			dummy;	    /* for err msgs */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See what kind of constituent */
    if (CONST_IsBasic(node)) {
	/* Could only have "Content Portions" */
	rval = LabelContentPortions(node, layout, id);
	goto LEAVE;
    }

    /* Might have "Subordinates" */
    if (!CONST_UnsafeHasAttr(node, at_SUBORDINATES)) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Supposed to have "Subordinates" -- make sure */
    attr = ATTR_FindAttr( (CONSTITUENT) node, at_SUBORDINATES);
    if (attr == ERROR_ATTRIBUTE) {
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	    TKERR_Format("[WalkAndLabelSpecificStructure]: constituent missing subordinates");
	dummy = TKERR_EndErrMsg();
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Walk through each child & label him */
    sons = GET_SEQUENCE_CONSTITUENT(attr);
    for (i=0; i<sons->length; i++) {
	newid =
	 CreateAndAssignId(at_OBJECT_IDENTIFIER,
	    (INTERNAL_CONSTITUENT) sons->sequence_value.constituents[i], i, id);
	if (newid == ERROR_STRING) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = WalkAndLabelSpecificStructure(
		(INTERNAL_CONSTITUENT) sons->sequence_value.constituents[i],
		layout, newid);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* All done with new id, so garbage collect it */
	rval = DeleteString(newid);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static STRING_type CreateAndAssignId(attrid, node, index, parentid)
    IN	   INT_type		attrid;
    IN OUT INTERNAL_CONSTITUENT	node;
    IN	   INT_type		index;
    IN	   STRING_type		parentid;
{
    STRING_type	    rval;
    INT_type	    digits;
    INT_type	    old_length;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Make copy of parent's id */
    rval = CopySequence(parentid);
    if (rval == ERROR_SEQUENCE) {
	rval = ERROR_STRING;
	goto LEAVE;
    }

    /* Make room for new numeral */
    digits = Ndigits(index);
    old_length = rval->length;
    if (ExpandSequence(rval, (INT_type) (digits+1)) == ERROR_INT) {    /* +1 is for ' ' */
	rval = ERROR_STRING;
	goto LEAVE;
    }

    /* Add ' ' & new numeral */
    rval -> sequence_value.chars[old_length] = ' ';
    if (FMT_FormatInt(&rval->sequence_value.chars[old_length+1],
		      digits, "%d", index) == ERROR_INT) {
	rval = ERROR_STRING;
	goto LEAVE;
    }

    /* Have id assigned */
    if (AssignIdAttr(node, attrid, rval) == ERROR_INT) {
	rval = ERROR_STRING;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

/* N >= 0 */

static INT_type Ndigits(n)
    IN INT_type n;
{
    REGISTER INT_type	temp;
    REGISTER INT_type	count;

    /* Special case small integers */
    if (n < 10) {
	return 1;
    } else if (n < 100) {
	return 2;
    } else if (n < 1000) {
	return 3;
    }

    /* Do it the hard way */
    for (temp=n, count=1; temp>0; count++) {
	temp /= 10;
    }
    return count;
}

static INT_type AssignIdAttr(node, attrid, id)
    IN OUT INTERNAL_CONSTITUENT	node;
    IN	   INT_type		attrid;
    IN	   STRING_type		id;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

#ifdef ROUT_I_WISHED_FOR
    /* If the attribute already has a value, remove it */
    if (CONST_UnsafeHasAttr(node, attrid)) {
	rval = RemAttr(node, attrid);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    rval = CONST_UnsafePutAttr(node, attrid, id, NO_PARMS);
#else
    rval = SetAttr((CONSTITUENT) node, attrid, (POINTER_type) id, NO_PARMS);

#endif

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type LabelContentPortions(node, layout, id)
    IN INTERNAL_CONSTITUENT  node;
    IN BOOL_type	    layout;
    IN STRING_type	    id;
{
    INT_type			rval;
    ATTR_attribute_t		attr;
    SEQUENCE_CONSTITUENT_type	cps;
    REGISTER INT_type		i;
    REGISTER INT_type		attrid;
    REGISTER STRING_type	val;
    INT_type			dummy;	/* for err msgs */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Make sure it's a basic logical object */
    if (!CONST_IsBasLog(node)) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	    TKERR_Format("[LabelContentPortions]: constituent not a basic");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* See if it has any "Content Portions" */
    if (!CONST_UnsafeHasAttr(node, at_CONTENT_PORTIONS)) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* It has some, extract attribute */
    attr = ATTR_FindAttr( (CONSTITUENT) node, at_CONTENT_PORTIONS);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	    TKERR_Format("[LabelContentPortions]: missing content portions attribute");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    cps = GET_SEQUENCE_CONSTITUENT(attr);

    /* See if layout/logical */
    attrid = (layout ? at_CONTENT_ID_LAYOUT : at_CONTENT_ID_LOGICAL);

    for (i=0; i<cps->length; i++) {
	val = CreateAndAssignId(
		attrid,
		(INTERNAL_CONSTITUENT) cps->sequence_value.constituents[i],
		i, id);
	if (val == ERROR_STRING) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	/* Done with label, garbage collect it */
	rval = DeleteString(val);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type LabelGenericStructure(root, counter)
    IN INTERNAL_CONSTITUENT root;
    IN INT_type		    counter;
{
    INT_type	rval;
    BOOL_type	layout;
    STRING_type	id;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if layout or logical */
    layout = CONST_IsDocLayRootCl(root);

    /* Construct root identifier */
    id = Chars2String(layout ? "0" : "2");
    if (id == ERROR_STRING) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Assign identifier to root */
    rval = AssignIdAttr(root, at_OBJECT_CLASS_IDENTIFIER, id);

    /* Now, label entire structure */
    if (rval != ERROR_INT) {
	rval = WalkAndLabelGenericStructure(root, layout, id, counter);
    }

    /* Garbage collect the root string */
    rval = DeleteString(id);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WalkAndLabelGenericStructure(node, layout, id, counter)
    IN INTERNAL_CONSTITUENT node;
    IN BOOL_type	    layout;
    IN STRING_type	    id;
    IN INT_type		    counter;
{
    INT_type		rval;
    ATTR_attribute_t	attr;
    INT_type		dummy;	/* for error messages */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Indicate we've been here */
    node -> debug_counter = counter;

    /* See what kind of constituent */
    if (CONST_IsBasic(node)) {
	/* Could only have "Content Portions" or "Content Generator" */
	if (CONST_UnsafeHasAttr(node, at_CONTENT_PORTIONS)) {
	    TKError = UNIMPL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy =
	      TKERR_Format("[WalkAndLabelGenericStructure]: cannot label generic content portions");
	    dummy = TKERR_EndErrMsg();
	}
	goto LEAVE;
    }

    /* Might have "Generator for Subordinates" */
    if (!CONST_UnsafeHasAttr(node, at_GEN_FOR_SUBORDINATES)) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Supposed to have "Generator for Subordinates" -- make sure */
    attr = ATTR_FindAttr( (CONSTITUENT) node, at_GEN_FOR_SUBORDINATES);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	  TKERR_Format("[WalkAndLabelGenericStructure]: missing generator for subordinates attribute");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Have construction expression labeled */
    rval = LabelConstructionExpr(GET_CONSTRUCTION_EXPR(attr),
				 layout, id, counter, (INT_type) 0);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type LabelConstructionExpr(expr, layout, id, counter, index)
    IN CONSTRUCTION_EXPR_type	*expr;
    IN BOOL_type		layout;
    IN STRING_type		id;
    IN INT_type			counter;
    IN INT_type			index;
{
    INT_type	rval;
    INT_type	dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    switch (expr->construction_tag) {

	case TERM_tag:
	    rval = LabelConstructionTerm(&expr->construction_value.term,
					 layout, id, counter, index);
	    break;

	case SEQUENCE_tag:
	case AGGREGATE_tag:
	case CHOICE_tag:
	    rval =
		LabelSeqConstructionTerm(expr->construction_value.construction,
					 layout, id, counter);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy =
	      TKERR_FormatInt("[LabelConstructionExpr]: unknown construction tag %d",
			   (INT_type) expr->construction_tag);
	    dummy = TKERR_EndErrMsg();
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type LabelConstructionTerm(term, layout, id, counter, index)
    IN CONSTRUCTION_TERM_type	*term;
    IN BOOL_type		layout;
    IN STRING_type		id;
    IN INT_type			counter;
    IN INT_type			index;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = LabelConstructionFactor(&term->factor, layout, id, counter, index);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type LabelConstructionFactor(factor, layout, id, counter, index)
    IN CONSTRUCTION_FACTOR_type	*factor;
    IN BOOL_type		layout;
    IN STRING_type		id;
    IN INT_type			counter;
    IN INT_type			index;
{
    INT_type		    rval;
    INTERNAL_CONSTITUENT    node;
    STRING_type		    newid;
    INT_type		    dummy;  /* for error messages */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    switch (factor->factor_tag) {
	case OBJECT_CLASS_ID_tag:
	    node = (INTERNAL_CONSTITUENT) factor -> factor_value.object_class_id;
	    if (node->debug_counter == counter) {
		rval = SUCCESS_INT;
		goto LEAVE;
	    }
	    newid = CreateAndAssignId(at_OBJECT_CLASS_IDENTIFIER,
				      node, index, id);
	    if (newid == ERROR_STRING) {
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    rval = WalkAndLabelGenericStructure(node, layout, newid, counter);
	    if (rval == ERROR_INT){
		goto LEAVE;
	    }

	    /* Garbage collect the label */
	    rval = DeleteString(newid);
	    break;

	case EXPR_tag:
	    rval = LabelConstructionExpr(factor->factor_value.expr,
					 layout, id, counter, index);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy =
	      TKERR_FormatInt("[LabelConstructionFactor]: unknown factor tag %d",
			   (INT_type) factor->factor_tag);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type LabelSeqConstructionTerm(seq, layout, id, counter)
    IN SEQUENCE_CONSTRUCTION_TERM_type	seq;
    IN BOOL_type			layout;
    IN STRING_type			id;
    IN INT_type				counter;
{
	     INT_type		    rval;
    REGISTER CONSTRUCTION_TERM_type *terms;
    REGISTER INT_type		    length;
    REGISTER INT_type		    i;

    length = seq -> length;
    terms = seq -> sequence_value.construction_terms;
    rval = SUCCESS_INT;
    for (i=0; i<length && rval!=ERROR_INT; i++) {
	rval = LabelConstructionTerm(&terms[i], layout, id, counter, i);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
