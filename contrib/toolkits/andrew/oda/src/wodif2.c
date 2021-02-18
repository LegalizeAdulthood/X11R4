
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif2.c,v 1.3 89/10/31 15:05:28 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif2.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif2.c,v $
 * Revision 1.3  89/10/31  15:05:28  jr
 * Casting errors fixed.
 * 
 * Revision 1.2  89/10/31  12:35:35  jr
 * Initialize rval in several
 * places.
 * 
 * Revision 1.1  89/09/21  06:44:26  mss
 * Initial revision
 * 
 * Revision 1.23  89/04/11  15:56:29  annm
 * no change
 * 
 * Revision 1.22  89/01/24  12:19:26  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.21  89/01/18  16:04:31  annm
 * chaged cc_CHAR_IMAGE_ORIENTATION's to cc_CHAR_ORIENTATION's
 * 
 * Revision 1.20  88/11/04  01:44:00  mz0k
 * Again,  this is the same as version 1.17
 * 
 * Revision 1.17  88/11/01  14:49:10  mz0k
 * Added TKError messages
 * 
 * Revision 1.16  88/10/14  10:05:52  jr
 * 1st -> 1ST
 * 
 * Revision 1.15  88/10/13  22:08:40  mz0k
 * corrected the tags for cc_INITIAL_OFFSET and cc_KERNING_OFFSET
 * 
 * Revision 1.14  88/10/12  12:03:05  mz0k
 * turned off the switiches left on
 * 
 * Revision 1.13  88/09/30  14:31:47  mss
 * Fixed use of register, undefined macro
 * Got rid of structure passing in arguments
 * 
 * Revision 1.12  88/09/20  18:43:13  mz0k
 * forward declaration for RODIF_LogicalObjClassBody()
 * 
 * Revision 1.11  88/09/14  17:07:50  mz0k
 * removed the "ifdef NOETESTED" from around the "AppendSequence's"
 * 
 * Revision 1.10  88/09/07  14:45:28  mz0k
 * Fixed LayoutDirectives according to the new part 5.
 * 
 * Revision 1.9  88/09/06  13:15:45  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.8  88/08/15  10:47:18  mz0k
 * Fixed the at_PROTECTION according to new part 5
 * 
 * Revision 1.7  88/08/10  16:09:57  jr
 * cc_Itemisation -> cc_ITEMISATION
 * 
 * Revision 1.6  88/08/04  14:28:13  mz0k
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif2.c,v $";
#endif

#define TK_TKI_IO
#define TK_IMPL_ATTR
#define TK_IMPL_DOC
#define TK_IMPL_CONST
#define TK_GVARS_WODIF2
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
#undef TK_GVARS_WODIF2
#undef TK_IMPL_WODIF
#undef TK_IMPL_ASN1
#undef TK_IMPL_WASN1
#undef TK_TKI_DEBUG
#undef TK_TKI_TKERR

/* Forward declarations */
static INT_type	WODIF_ContentPortions();
static INT_type	WODIF_PresentationStyleDesc();
static INT_type	WODIF_LayoutStyleDescriptor();
static INT_type	WODIF_WalkGenericLogical();
static INT_type	WODIF_LogicalObjClassDesc();
static INT_type	WODIF_LogicalObjClassDescBody();

INT_type WODIF_CharacterAttributes(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type			rval;
    REGISTER INT_type			tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* Write the set header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);   

    /* Now have each of the set element written */
    if ((rval != ERROR_INT) && (HasAttr(object, cc_CHARACTER_PATH) == HAS_ATTR)){
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_ValueType(od, object, tagfield, cc_CHARACTER_PATH);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_LINE_PROGRESSION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_ValueType(od, object, tagfield, cc_LINE_PROGRESSION);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_CHAR_ORIENTATION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, cc_CHAR_ORIENTATION);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_INITIAL_OFFSET)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_IntPair(od, object, tagfield, cc_INITIAL_OFFSET);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_CHARACTER_SPACING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 6);
	rval = WASN1_IntType(od, object, tagfield, cc_CHARACTER_SPACING);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_LINE_SPACING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 7);
	rval = WASN1_IntType(od, object,tagfield, cc_LINE_SPACING);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_ALIGNMENT)==HAS_ATTR)) {
	tagfield= MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 8);
	rval = WASN1_ValueType(od, object, tagfield, cc_ALIGNMENT);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, cc_LINE_LAYOUT_TABLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 9);
	rval = WASN1_LineLayoutTable(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_GRAPHIC_RENDITION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 10);
	rval = WASN1_GraphicRendition(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_FORMATTING_INDICATOR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 11);
	rval = WASN1_ValueType(od, object, tagfield, cc_FORMATTING_INDICATOR);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_CHARACTER_FONTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 12);
	rval = WASN1_CharacterFonts(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_GRAPHIC_CHAR_SUBREP)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 13);
	rval = WASN1_IntType(od, object, tagfield, cc_GRAPHIC_CHAR_SUBREP);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_ITEMISATION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 14);
	rval = WASN1_Itemisation(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_WIDOW_SIZE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 15);
	rval = WASN1_IntType(od, object, tagfield, cc_WIDOW_SIZE);
    }
    if((rval !=	ERROR_INT) && (HasAttr(object, cc_ORPHAN_SIZE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 16);
	rval = WASN1_IntType(od, object, tagfield, cc_ORPHAN_SIZE);
    }
    if((rval !=	ERROR_INT) && (HasAttr(object, cc_GRAPHIC_CHARACTER_SETS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 17);
	rval = WASN1_SequenceBytes(od, object, tagfield, cc_GRAPHIC_CHARACTER_SETS);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_INDENTATION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 19);
	rval = WASN1_IntType(od, object,tagfield, cc_INDENTATION);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_KERNING_OFFSET)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 20);
	rval = WASN1_IntPair(od, object, tagfield, cc_KERNING_OFFSET);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_PROP_LINE_SPACING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 21);
	rval = WASN1_ValueType(od, object, tagfield, cc_PROP_LINE_SPACING);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_PAIRWISE_KERNING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 22);
	rval = WASN1_ValueType(od, object, tagfield, cc_PAIRWISE_KERNING);
    }
    if((rval != ERROR_INT) && (HasAttr(object, cc_1ST_LINE_OFFSET)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 23);
	rval = WASN1_IntType(od, object,tagfield, cc_1ST_LINE_OFFSET);
    }
    if((rval !=	ERROR_INT) && (HasAttr(object, cc_CODE_EXT_ANNOUNCERS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 24);
	rval = WASN1_SequenceBytes(od, object, tagfield, cc_CODE_EXT_ANNOUNCERS);

    }
    
    /* Close the set */
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


INT_type WODIF_GenericContent(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    /* Notice that the  constituents for GENERIC CONTENT PORTIONS have been
           saved in the sequence of 'SequenceOfGenContents' while walking the 
          object class descriptors.
      */

    INT_type	rval;	
    REGISTER INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    rval = SUCCESS_INT;
    for (i=0; (i<=SequenceOfGenContents->length-1) && (rval!=ERROR_INT); i++) {
	rval = WODIF_ContentPortions(od,
			SequenceOfGenContents->sequence_value.constituents[i]);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WODIF_SpecificContent(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    /* Notice that the CONTENT PORTIONS have been saved in the sequence
      of 'SequenceOfSpeContents' while traversing the logical object descriptor
      */

    INT_type	rval;	
    REGISTER INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    rval = SUCCESS_INT;
    for (i=0; (i<=SequenceOfSpeContents->length-1) && (rval!=ERROR_INT); i++) {
	rval = WODIF_ContentPortions(od,
		SequenceOfSpeContents->sequence_value.constituents[i]);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_ContentPortions(od, object)
    IN INT_type	    od;
    IN CONSTITUENT  object;
{
    INT_type	rval;	 
REGISTER INT_type	tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the sequence header for the TEXT UNIT first */
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);

    /* Then each part */
    if (rval != ERROR_INT) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SET_TAG);
	rval = WASN1_ContentPortionAttributes(od, object, tagfield);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_INFORMATION)==HAS_ATTR)) {
	/* NOTICE: We ignore the 'teletex string' choice */
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, OCTETSTRING_TAG);
	rval = WASN1_SequenceBytes(od, object, tagfield, at_CONTENT_INFORMATION);
    }

    /* Close the sequence */
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

INT_type WODIF_PresentationStyle(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    INT_type	rval;	    /* The returned value */
    INTERNAL_DOCUMENT_type TheDocument;
    REGISTER INTERNAL_CONSTITUENT	style;
    REGISTER INTERNAL_CONSTITUENT	head;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    TheDocument = (INTERNAL_DOCUMENT_type) document;
    head = &TheDocument->presentation_styles_list;
    if (head->forward == head) {
	/* Is this OK? */
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Walk through the Presentation Styles and spit out the presentation style descriptors */
    for (style=head->forward; style != head; style=style->forward) {
	rval = WODIF_PresentationStyleDesc(od, (CONSTITUENT) style);
	if (rval == ERROR_INT) {
	    goto LEAVE;
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

static INT_type WODIF_PresentationStyleDesc(od, object)
    IN INT_type		    od;
    IN CONSTITUENT	    object;
{
    INT_type	rval;	 
REGISTER INT_type	tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the sequence header first */
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 7);
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    
    /* Then each attribute */
    if((rval != ERROR_INT) && (HasAttr(object, at_PRESENTATION_STYLE_ID)==HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 5);
	rval = WASN1_Strings(od, object, tagfield, at_PRESENTATION_STYLE_ID);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_READABLE_COMMENTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Strings(od, object, tagfield, at_USER_READABLE_COMMENTS);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_VISIBLE_NAME)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Strings(od, object, tagfield, at_USER_VISIBLE_NAME);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_TRANSPARENCY)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, at_TRANSPARENCY);
    }
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WODIF_PresentationAttributes(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_COLOUR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 4);
	rval = WASN1_ValueType(od, object, tagfield, at_COLOUR);
    }
#ifdef NOTTESTED
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	rval = WASN1_Border(od, object, tagfield);
    }
#endif
    /* Close the sequence */
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

INT_type WODIF_LayoutStyle(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    INT_type	rval;	    /* The returned value */
    INTERNAL_DOCUMENT_type TheDocument;
    REGISTER INTERNAL_CONSTITUENT	style;
    REGISTER INTERNAL_CONSTITUENT	head;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    TheDocument = (INTERNAL_DOCUMENT_type) document;
    head = &TheDocument->layout_styles_list;
    if (head->forward == head) {
	/* Is this OK? */
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Walk through the Presentation Styles and spit out the presentation style descriptors */
    for (style=head->forward; style != head; style=style->forward) {
	rval = WODIF_LayoutStyleDescriptor(od, (CONSTITUENT) style);
	if (rval == ERROR_INT) {
	    goto LEAVE;
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

static INT_type WODIF_LayoutStyleDescriptor(od, object)
    IN INT_type		    od;
    IN CONSTITUENT	    object;
{
    INT_type	rval;	 
REGISTER INT_type	tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the set header first */
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 8);
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    
    /* Then each attribute */
    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_STYLE_IDENTIFIER)==HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 5);
	rval = WASN1_Strings(od, object, tagfield, at_LAYOUT_STYLE_IDENTIFIER);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_READABLE_COMMENTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Strings(od, object, tagfield, at_USER_READABLE_COMMENTS);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_VISIBLE_NAME)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Strings(od, object, tagfield, at_USER_VISIBLE_NAME);
    }
    if (rval != ERROR_INT)  {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 4);
	rval = WODIF_LayoutDirectives(od, object, tagfield);
    }

    /* Close the sequence */
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

INT_type WODIF_GenericLogical(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
    INT_type		    rval;	    /* The returned value */
    INTERNAL_DOCUMENT_type  TheDocument;
    REGISTER INT_type	    i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    /* If no generic logical structure, handle it */
    if (TheDocument->generic_logical_root == NULL_CONSTITUENT) {
	/* Is this okay, or must it be here ??? */
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /*
       Walk the generic logical structure & spit out
       Logical-Object-Class-Descriptor's
    */
    last_debug_counter++;
    rval = WODIF_WalkGenericLogical(od,
			(INTERNAL_CONSTITUENT) TheDocument->generic_logical_root,
			last_debug_counter);

    /* Then write out the logical object classes in the sequence of SequenceOfLogClass */
    for (i=0; i<= SequenceOfLogClass->length-1; i++) {
	rval = WODIF_WalkGenericLogical(od, (INTERNAL_CONSTITUENT) 
		SequenceOfLogClass->sequence_value.constituents[i], last_debug_counter);
	if (rval == ERROR_INT)
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

static INT_type WODIF_WalkGenericLogical(od, constituent, loopvalue)
    IN INT_type	    od;
    IN INTERNAL_CONSTITUENT  constituent;
    IN INT_type	    loopvalue;
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
    if ( constituent->debug_counter == loopvalue) {
	goto LEAVE;
    }

    /* Walk this node */
    constituent->debug_counter = last_debug_counter;

    /* Write out the attributes of the logical object class descriptor first */
    tagfield = MakeTag( CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, 5);
    rval = WODIF_LogicalObjClassDesc(od, (CONSTITUENT) constituent, tagfield);
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
	    islayout = BOOL_false;
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

INT_type WODIF_WalkConstructionExpr(od, expr, islayout)
    IN INT_type			od;
    IN CONSTRUCTION_EXPR_type	*expr;
    IN BOOL_type		islayout;
{
    INT_type			    rval;
    REGISTER INT_type		    i;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* 'expr' is a pointer to a CONSTRUCTION_EXPR_type */
	    switch ( expr->construction_tag) {
		    case TERM_tag:
			rval = WODIF_WalkConstructionTerm(od, &expr->construction_value.term, islayout);
			break;
		    case SEQUENCE_tag:
		    case AGGREGATE_tag:
		    case CHOICE_tag:
			for (i=0; i<=(expr->construction_value.construction)->length-1; i++) {
			    rval = WODIF_WalkConstructionTerm(od, &((expr->
					construction_value.construction)->
                                        sequence_value.construction_terms[i]),
					islayout);
			    if (rval == ERROR_INT) {
				goto LEAVE;
			    }
			}
			break;
		    default:
			rval = ERROR_INT;
			TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt(
    "[WODIF_WalkConstructionExpr]: Invalid expr->construction_tag value: 0d%d",
    (INT_type) expr->construction_tag);
			dummy = TKERR_EndErrMsg();
#endif
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
    
INT_type WODIF_WalkConstructionTerm(od, term, islayout)
    IN INT_type			od;
    IN CONSTRUCTION_TERM_type	*term;
    IN BOOL_type		islayout;
{
    INT_type			    rval;
    INT_type		dummy;
    
#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* At the time of walking, just forget the REQ, OPT, REP or THE OPT_REP tags */
    switch (term->factor.factor_tag) {
	    case OBJECT_CLASS_ID_tag:
		if (islayout) {
		    rval = WODIF_WalkGenericLayout(od, (INTERNAL_CONSTITUENT) 
term->factor.factor_value.object_class_id, last_debug_counter);
		}
		else {
		    rval = WODIF_WalkGenericLogical(od, (INTERNAL_CONSTITUENT) 
		term->factor.factor_value.object_class_id, last_debug_counter);
		}
		break;
	    case EXPR_tag:
		rval = WODIF_WalkConstructionExpr(od, term->factor.factor_value.expr,
						  islayout);
		break;
	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt(
    "[WODIF_WalkConstructionTerm]: Invalid term->factor.factor_tag value: 0d%d",
    (INT_type) term->factor.factor_tag);
		dummy = TKERR_EndErrMsg();
#endif
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

static INT_type WODIF_LogicalObjClassDesc(od, object, tag)
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
    
    /* Now have subfields written -- make the at_OBJECT_TYPE mandatory@@@ */
    if (HasAttr(object, at_OBJECT_TYPE)==NO_ATTR) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[WODIF_LogicalObjClassDesc]: The Logical Object Class doesn't have an at_OBJECT_TYPE." );
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
	rval = WODIF_LogicalObjClassDescBody(od, object, tagfield);
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
    
/*
    Logical-Object-Class-Descriptor-Body  ::= SET {
	object-class-identifier		Object-or-Class-Identifier OPTIONAL,
	generator-for-subordinates	Generator OPTIONAL,
	content-portions		[1] IMPLICIT SEQUENCE OF NumericString OPTIONAL,
	presentation-attributes		[6] IMPLICIT Presentation-Attributes OPTIONAL,
	default-value-lists		[7] IMPLICIT Default-Value-Lists-Logical OPTIONAL,
	user-readable-comments		[8] IMPLICIT Comment-String OPTIONAL,
	bindings			[9] IMPLICIT SET OF Binding-Pair OPTIONAL,
	content-generator	       [10] IMPLICIT String-Expression OPTIONAL,
	user-visible-name	       [14] IMPLICIT Comment-String OPTIONAL,
	presentation-style	       [17] IMPLICIT Style-Identifier OPTIONAL,
	layout-style		       [19] IMPLICIT Style-Identifier OPTIONAL,
	property-attributes	       [20] IMPLICIT Property-Attributes OPTIONAL,
	resource		       [24] IMPLICIT INTEGER OPTIONAL,
	layout-directives	       [25] IMPLICIT Layout-Directives OPTIONAL
    }
*/

static INT_type WODIF_LogicalObjClassDescBody(od, object, tag)
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
    if ((rval != ERROR_INT) && (HasAttr(object, 
	at_OBJECT_CLASS_IDENTIFIER)==HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Strings(od, object, tagfield, at_OBJECT_CLASS_IDENTIFIER);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_GEN_FOR_SUBORDINATES) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WODIF_WriteGenerator(od, object, tagfield);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_CONTENT_PORTIONS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WODIF_SequenceConstituents(od, object, at_CONTENT_PORTIONS, at_CONTENT_ID_LOGICAL, tagfield, BOOL_true);
    }
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 6);
        rval = WODIF_PresentationAttributes(od, object, tagfield);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, at_DEFAULT_VALUE_LISTS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 7);
	rval = WODIF_DefaultValueListsLogical(od, object, tagfield);
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
    if((rval != ERROR_INT) && (HasAttr(object, at_USER_VISIBLE_NAME)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 14);
	rval = WASN1_Strings(od, object, tagfield, at_USER_VISIBLE_NAME);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 17);
	rval =WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE, at_PRESENTATION_STYLE_ID, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_STYLE_IDENTIFIER)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 19);
	rval = WASN1_ConstituentId(od, object, at_LAYOUT_STYLE, at_LAYOUT_STYLE_IDENTIFIER, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PROTECTION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 20);
	rval = WASN1_ValueType(od, object, tagfield, at_PROTECTION);
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

INT_type WODIF_WriteGenerator(od, object, tag)
    IN INT_type		od;
    IN CONSTITUENT	object;
    IN INT_type		tag;
{
    INT_type			    rval;	    /* The returned value */
    ATTR_attribute_t		    attr;
    at_GEN_FOR_SUBORDINATES_type    *expr;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

     attr = ATTR_FindAttr(object, at_GEN_FOR_SUBORDINATES);
     if (attr == ERROR_ATTRIBUTE) {
	 rval = ERROR_INT;
	 goto LEAVE;
     }

     /* Write the header for the generator */
     rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
     if (rval == ERROR_INT) {
	 goto LEAVE;
     }
     
     /* Get the pointer to the construction-expression */
     expr = GET_CONSTRUCTION_EXPR(attr);

     rval = WODIF_WriteConstructionExpr(od, expr);
     if (rval == ERROR_INT) {
	 goto LEAVE;
     }
     
     /* The EOC for the sconstruction expresion */
     rval = WASN1_EOC(od);
     
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WODIF_WriteConstructionExpr(od, expr)
    IN INT_type			od;
    IN CONSTRUCTION_EXPR_type	*expr;
{
    INT_type			    rval;
    REGISTER INT_type		    i;
    REGISTER INT_type		    tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* 'expr' is a pointer to a CONSTRUCTION_EXPR_type */
	    switch ( expr->construction_tag) {
		    case TERM_tag:
			tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED,
					   (INT_type) 3);
			rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
			if (rval == ERROR_INT) {
			    goto LEAVE;
			}
			rval = WODIF_WriteConstructionTerm(od,
				&expr->construction_value.term);
			if (rval != ERROR_INT) {
			    rval = WASN1_EOC(od);
			}
			goto LEAVE;
		    case SEQUENCE_tag:
			tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED,
					   (INT_type) 0);
			break;
		    case AGGREGATE_tag:
			tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED,
					   (INT_type) 1);
			break;
		    case CHOICE_tag:
			tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED,
					   (INT_type) 2);
			break;
		    default:
			rval =ERROR_INT;
			TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[WODIF_WriteConstructionExpr]: Invalid expr->construction_tag value: 0d%d", (INT_type) expr->construction_tag );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
	    }

	    /* The sequence of construction terms */
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    for (i=0; i<=(expr->construction_value.construction)->length-1; i++) {
		rval = WODIF_WriteConstructionTerm(od,
			  &((expr->construction_value.
			     construction)->
			     sequence_value.construction_terms[i]));
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
	    }
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

INT_type WODIF_WriteConstructionTerm(od, term)
    IN INT_type			od;
    IN CONSTRUCTION_TERM_type	*term;
{
    INT_type			    rval;
    REGISTER INT_type		    tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Write the header for the constrcution term first */
    switch (term->term_tag) {
	    case REQ_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
		break;

	    case OPT_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
		break;

	    case REP_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
		break;

	    case OPT_REP_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
		break;

	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WODIF_WriteConstructionTerm]: Invalid term->term_tag value: 0d%d", (INT_type) term->term_tag );
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
    }

    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT ) {
	goto LEAVE;
    }
    rval = WODIF_WriteConstructionFactor(od, &(term->factor));

    /* The end of content marker */
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

INT_type WODIF_WriteConstructionFactor(od, factor)
    IN INT_type			od;
    IN CONSTRUCTION_FACTOR_type	*factor;
{
    INT_type			    rval;
    REGISTER INT_type		    tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    
    switch (factor->factor_tag) {
	    case OBJECT_CLASS_ID_tag:
		if (HasAttr(factor->factor_value.object_class_id, at_OBJECT_CLASS_IDENTIFIER)==NO_ATTR) {
		    rval = ERROR_INT;
		    goto LEAVE;
		}

		tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 1);
		rval = WASN1_Strings(od, factor->factor_value.object_class_id,
				     tagfield, at_OBJECT_CLASS_IDENTIFIER);
		break;

	    case EXPR_tag:
		rval = WODIF_WriteConstructionExpr(od, factor->factor_value.expr);
		break;

	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WODIF_WriteConstructionFactor]: Invalid factor->factor_tag value: 0d%d", (INT_type) factor->factor_tag );
		dummy = TKERR_EndErrMsg();
#endif
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
