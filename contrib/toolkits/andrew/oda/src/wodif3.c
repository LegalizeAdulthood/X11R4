

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif3.c,v 1.2 89/10/31 15:09:50 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif3.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif3.c,v $
 * Revision 1.2  89/10/31  15:09:50  jr
 * Some casting problems & missing args fixed.
 * 
 * Revision 1.1  89/09/21  06:44:51  mss
 * Initial revision
 * 
 * Revision 1.16  89/03/22  15:53:58  annm
 * changes for the new dvl's
 * 
 * Revision 1.15  89/01/24  12:19:57  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.14  88/11/04  01:42:10  mz0k
 * It's a shame,  but this is the same as version 1.12
 * 
 * Revision 1.12  88/11/01  14:49:34  mz0k
 * Added TKError messages
 * 
 * Revision 1.11  88/10/13  22:10:43  mz0k
 * corrected the tags for at_PAGE_POSITION
 * 
 * Revision 1.10  88/10/12  11:53:33  mz0k
 * turned off switches left on.
 * 
 * Revision 1.9  88/09/30  14:09:43  mss
 * FIxed typo in undefined macro being undefined (TK_TKI_DOC)
 * 
 * Revision 1.8  88/09/27  18:31:52  mz0k
 * added the at_APPLICATION_COMMENTS
 * 
 * Revision 1.7  88/09/23  16:53:10  mz0k
 * fixed the default value lists for layout structures
 * 
 * Revision 1.6  88/09/22  14:56:27  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.5  88/09/06  13:16:38  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/04  14:32:21  mz0k
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif3.c,v $";
#endif

#define TK_TKI_IO
#define TK_IMPL_ATTR
#define TK_IMPL_DOC
#define TK_IMPL_CONST
#define TK_GVARS_WODIF3
#define TK_IMPL_WODIF
#define TK_IMPL_ASN1
#define TK_IMPL_WASN1
#define TK_TKI_DEBUG
#define TK_TKI_TKERR

#include <wodif.h>
#include <tags.h>

#undef TK_TKI_IO
#undef TK_IMPL_ATTR
#undef TK_IMPL_DOC
#undef TK_IMPL_CONST
#undef TK_GVARS_WODIF3
#undef TK_IMPL_WODIF
#undef TK_IMPL_ASN1
#undef TK_IMPL_WASN1
#undef TK_TKI_DEBUG
#undef TK_TKI_TKERR

/* Forward declarations */
static INT_type WODIF_LayoutObjClassDesc();
static INT_type WODIF_LayoutObjClassDescBody();
static INT_type WODIF_DefaultValueListsLayout();
static INT_type WODIF_WalkSpecificLayout();
static INT_type WODIF_LayoutObjDesc();
static INT_type WODIF_LayoutObjDescBody();

INT_type WODIF_GenericLayout(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    INT_type	rval;	    /* The returned value */
    INTERNAL_DOCUMENT_type TheDocument;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    /* If no generic layout structure, handle it */
    if (TheDocument->generic_layout_root == NULL_CONSTITUENT) {
	/* Is this okay, or must it be here ??? */
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /*
       Walk the generic layout structure & spit out
       Layout-Object-Class-Descriptor's
    */
    last_debug_counter++;
    rval = WODIF_WalkGenericLayout(od,
			    (INTERNAL_CONSTITUENT) TheDocument->generic_layout_root,
			    last_debug_counter);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WODIF_WalkGenericLayout(od, constituent, loopvalue)
    IN INT_type			od;
    IN INTERNAL_CONSTITUENT	constituent;
    IN INT_type			loopvalue;
{
    INT_type			    rval;	    /* The returned value */
    ATTR_attribute_t		    attr;
    at_GEN_FOR_SUBORDINATES_type    *children;
    REGISTER INT_type		    tagfield;
    BOOL_type			    islayout;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif
   
    rval = SUCCESS_INT;

    /* See if we have visited this node already */
    if (constituent->debug_counter == loopvalue) {
	goto LEAVE;
    }
    
    /* Otherwise walk the current node */
    constituent->debug_counter = last_debug_counter;

    /* Write out the attributes of the layout object class descriptor first */
    tagfield = MakeTag( CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
    rval = WODIF_LayoutObjClassDesc(od, (CONSTITUENT) constituent, tagfield);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Then look at the immediate subordinates recursively */
    if (HasAttr((CONSTITUENT) constituent, at_GEN_FOR_SUBORDINATES) == HAS_ATTR) {
	    attr = ATTR_FindAttr((CONSTITUENT) constituent, at_GEN_FOR_SUBORDINATES);
	    if (attr == ERROR_ATTRIBUTE) {
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    children = GET_CONSTRUCTION_EXPR(attr);
	    
	    /* 'children' is a pointer to a CONSTRUCTION_EXPR_type */
	    islayout = BOOL_true;
	    rval = WODIF_WalkConstructionExpr(od, children, islayout);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_LayoutObjClassDesc(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{
    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Lay down SEQUENCE info */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Now have subfields written --- make the at_OBJECT_TYPE mandatory */
    if (HasAttr(object, at_OBJECT_TYPE)==NO_ATTR) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[WODIF_LayoutObjClassDesc]: The Layout Object Class Description doesn't have an at_OBJECT_TYPE." );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
    else {
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
	rval = WASN1_ValueType(od, object, tagfield, at_OBJECT_TYPE);
    }
    if (rval != ERROR_INT) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SET_TAG);
	rval = WODIF_LayoutObjClassDescBody(od, object, tagfield);
    }

    /* If all went okay, write end of content value */
    if (rval != ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
   
static INT_type WODIF_LayoutObjClassDescBody(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{
    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	tagfield;   

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* Set header info */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);

    /* Now look at each part */
    if ((rval != ERROR_INT) && (HasAttr(object, at_OBJECT_CLASS_IDENTIFIER) 
         ==HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Strings(od, object, tagfield, at_OBJECT_CLASS_IDENTIFIER);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_GEN_FOR_SUBORDINATES) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WODIF_WriteGenerator(od, object, tagfield);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_CONTENT_PORTIONS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WODIF_SequenceConstituents(od, object, at_CONTENT_PORTIONS, 
		at_CONTENT_ID_LAYOUT, tagfield, BOOL_true);
    }

/* The part5-2 of the position is entirely obsolete,  so forget about it for now */
    
    if (rval != ERROR_INT && (HasAttr(object, at_DIMENSIONS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 4);
	rval = WASN1_Dimensions(od, object, tagfield);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_TRANSPARENCY) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	rval = WASN1_ValueType(od, object, tagfield, at_TRANSPARENCY);
    }
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 6);
        rval = WODIF_PresentationAttributes(od, object, tagfield);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, at_DEFAULT_VALUE_LISTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 7);
	rval = WODIF_DefaultValueListsLayout(od, object, tagfield);
    } 

    if ((rval != ERROR_INT) && (HasAttr(object, at_USER_READABLE_COMMENTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE,(INT_type) 8);
	rval = WASN1_Strings(od, object, tagfield, at_USER_READABLE_COMMENTS);
    }
#ifdef NOTTESTED
    if((rval != ERROR_INT) && (HasAttr(object, at_BINDINGS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 9);
	rval = WASN1_Bindings(od, object, at_BINDINGS, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_GENERATOR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 10);
	rval = WASN1_ContentGenerator(od, object, at_BINDINGS, tagfield);
    }
#endif
    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_PATH)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 11);
	rval = WASN1_ValueType(od, object, tagfield, at_LAYOUT_PATH);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PERMITTED_CATEGORIES)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 13);
	rval = WASN1_SequenceStrings(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_VISIBLE_NAME)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 14);
	rval = WASN1_Strings(od, object, tagfield, at_USER_VISIBLE_NAME);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PAGE_POSITION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 15);
	rval = WASN1_IntPair(od, object, tagfield, at_PAGE_POSITION);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_MEDIUM_TYPE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 16);
	rval = WASN1_MediumType(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 17);
	rval = WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE, at_PRESENTATION_STYLE_ID, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_LOGICAL_SOURCE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 18);
	rval = WASN1_ConstituentId(od, object, at_LOGICAL_SOURCE, 
		at_OBJECT_CLASS_IDENTIFIER, tagfield);
    }
    
    if((rval != ERROR_INT) && (HasAttr(object, at_BALANCE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 21);
	rval = WODIF_SequenceConstituents(od, object, at_BALANCE, 
		at_OBJECT_CLASS_IDENTIFIER, tagfield, BOOL_true);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_COLOUR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 22);
	rval = WASN1_ValueType(od, object, tagfield, at_COLOUR);
    }
    /* Take the at_RESOURCE for a string for now */
    if((rval != ERROR_INT) && (HasAttr(object, at_RESOURCE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 24);
	rval = WASN1_Strings(od, object, tagfield, at_RESOURCE);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, at_APPLICATION_COMMENTS))==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 25);
	rval = WASN1_Strings(od, object, tagfield, at_APPLICATION_COMMENTS);
    }
    
    

    /* If still okay, close up */
    if (rval != ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_DefaultValueListsLayout(od, object, tag)
    IN INT_type			od;
    IN CONSTITUENT		object;
    IN INT_type			tag;
{
    INT_type			rval;
    REGISTER INT_type		tagfield;
    at_DEFAULT_VALUE_LISTS_type *TheDVL;
    ATTR_attribute_t		attr;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Point to the appropriate attribute */
    attr = ATTR_FindAttr(object, at_DEFAULT_VALUE_LISTS);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the set header for the default-value-lists-layout */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Get the list of constituents and write out the defaultable attributes */
    TheDVL = GET_at_DEFAULT_VALUE_LISTS(attr);

    if( TheDVL->page_attributes != NULL_CONSTITUENT ){
        tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WASN1_PageAttributes(od, TheDVL->page_attributes, 
		    tagfield, at_OBJECT_TYPE_page);
    }

    if( TheDVL->frame_attributes != NULL_CONSTITUENT ){
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_FrameAttributes(od, TheDVL->frame_attributes, tagfield);
    }

    if( TheDVL->block_attributes != NULL_CONSTITUENT ){
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 4);
	rval = WASN1_BlockAttributes(od, TheDVL->block_attributes, tagfield);
    }
		    
    if (rval != ERROR_INT) {
	rval = WASN1_EOC(od);
    }
	
LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



INT_type WODIF_SpecificLayout(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    INT_type	rval;	    /* The returned value */
    INTERNAL_DOCUMENT_type TheDocument;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    /* If no specific layout structure, handle it */
    if (TheDocument->specific_layout_root == NULL_CONSTITUENT) {
	/* Is this okay, or must it be here ??? */
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /*
       Walk the specific layout structure & spit out
       Layout-Object-Descriptor's
    */
    rval = WODIF_WalkSpecificLayout(od, TheDocument->specific_layout_root);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_WalkSpecificLayout(od, constituent)
    IN INT_type	    od;
    IN CONSTITUENT  constituent;
{
    INT_type			rval;	    /* The returned value */
    ATTR_attribute_t		attr;
    SEQUENCE_CONSTITUENT_type	children;
    REGISTER INT_type		i;
    REGISTER INT_type		tagfield;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif
   
    /* Write out the attributes of the layout object descriptor first */
    tagfield = MakeTag( CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, 6);
    rval = WODIF_LayoutObjDesc(od, constituent, tagfield);
 
    /* Then look at the immediate subordinates recursively */
   if (rval != ERROR_INT) {
	if (HasAttr(constituent, at_SUBORDINATES) == HAS_ATTR) {
	    attr = ATTR_FindAttr(constituent, at_SUBORDINATES);
	    if (attr == ERROR_ATTRIBUTE) {
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    children = GET_SEQUENCE_CONSTITUENT(attr);
	    for (i=0; i<children->length; i++) {
		rval = WODIF_WalkSpecificLayout(od, children->sequence_value.constituents[i]);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
	    }
	}
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_LayoutObjDesc(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{
    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Lay down SEQUENCE info */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Now have subfields written --- make the at_OBJECT_TYPE mandatory */
    if (HasAttr(object, at_OBJECT_TYPE)==NO_ATTR) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[WODIF_LayoutObjDesc]: The Layout Object doesn't have an at_OBJECT_TYPE." );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
    else {
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
	rval = WASN1_ValueType(od, object, tagfield, at_OBJECT_TYPE);
    }
    
    if (rval != ERROR_INT) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SET_TAG);
	rval = WODIF_LayoutObjDescBody(od, object, tagfield);
    }

    /* If all went okay, write end value */
    if (rval != ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_LayoutObjDescBody(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{
    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	tagfield;   

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* Set header info */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);

    /* Now look at each part */
    if ((rval != ERROR_INT) && (HasAttr(object, at_OBJECT_IDENTIFIER) 
         ==HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Strings(od, object, tagfield, at_OBJECT_IDENTIFIER);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_SUBORDINATES) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WODIF_SequenceConstituents(od, object, at_SUBORDINATES, at_OBJECT_IDENTIFIER, tagfield, BOOL_false);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_CONTENT_PORTIONS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WODIF_SequenceConstituents(od, object, at_CONTENT_PORTIONS, 
		at_CONTENT_ID_LAYOUT, tagfield, BOOL_false);
    }

/* The part5-2 of the position is entirely obsolete,  so forget about it for now */
    
    if (rval != ERROR_INT && (HasAttr(object, at_DIMENSIONS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 4);
	rval = WASN1_Dimensions(od, object, tagfield);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_TRANSPARENCY) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	rval = WASN1_ValueType(od, object, tagfield, at_TRANSPARENCY);
    }
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 6);
        rval = WODIF_PresentationAttributes(od, object, tagfield);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, at_DEFAULT_VALUE_LISTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 7);
	rval = WODIF_DefaultValueListsLayout(od, object, tagfield);
    } 

    if ((rval != ERROR_INT) && (HasAttr(object, at_USER_READABLE_COMMENTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE,(INT_type) 8);
	rval = WASN1_Strings(od, object, tagfield, at_USER_READABLE_COMMENTS);
    }
#ifdef NOTTESTED
    if((rval != ERROR_INT) && (HasAttr(object, at_BINDINGS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 9);
	rval = WASN1_Bindings(od, object, at_BINDINGS, tagfield);
    }
#endif
    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_PATH)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 11);
	rval = WASN1_ValueType(od, object, tagfield, at_LAYOUT_PATH);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_IMAGING_ORDER) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 12);
	rval = WODIF_SequenceConstituents(od, object, at_IMAGING_ORDER, at_OBJECT_IDENTIFIER, tagfield, BOOL_false);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PERMITTED_CATEGORIES)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 13);
	rval = WASN1_SequenceStrings(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_VISIBLE_NAME)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 14);
	rval = WASN1_Strings(od, object, tagfield, at_USER_VISIBLE_NAME);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PAGE_POSITION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 15);
	rval = WASN1_IntPair(od, object, tagfield, at_PAGE_POSITION);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_MEDIUM_TYPE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 16);
	rval = WASN1_MediumType(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 17);
	rval = WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE, at_PRESENTATION_STYLE_ID, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_BALANCE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 21);
	rval = WODIF_SequenceConstituents(od, object, at_BALANCE, 
		at_OBJECT_CLASS_IDENTIFIER, tagfield, BOOL_true);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_COLOUR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 22);
	rval = WASN1_ValueType(od, object, tagfield, at_COLOUR);
    }
#ifdef NOTTESTED
    if((rval != ERROR_INT) && (HasAttr(object, at_BORDER)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 23);
	rval = WASN1_Border(od, object, tagfield);
    }
#endif

    /* If still okay, close up */
    if (rval != ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}
