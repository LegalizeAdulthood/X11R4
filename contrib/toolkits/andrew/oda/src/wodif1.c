


/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif1.c,v 1.2 89/10/26 14:13:48 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif1.c,v $


(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.
 
$Log:	wodif1.c,v $
 * Revision 1.2  89/10/26  14:13:48  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:44:02  mss
 * Initial revision
 * 
 * Revision 1.28  89/05/18  17:57:07  annm
 * WriteODIF will now autmatically label the document if the InitParms require
 * 
 * Revision 1.27  89/03/22  15:53:36  annm
 * changes for the new dvl's
 * 
 * Revision 1.26  89/01/24  12:19:15  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.25  88/11/16  20:11:59  jr
 * Fix problem with WODIF_DocumentProfile
 * being used in static initialization
 * before def'n (hoses VMS).
 * 
 * Revision 1.24  88/11/01  14:48:46  mz0k
 * Added TKError messages
 * 
 * Revision 1.23  88/10/21  16:44:47  mz0k
 * added WODIF_DocumentProfile
 * 
 * Revision 1.22  88/10/13  22:06:35  mz0k
 * moved the '#ifdef NONTESTED' wrap from the raster stuff
 * 
 * Revision 1.20  88/09/30  15:53:36  mss
 * Rearranged static procedures so that defined before used in tables
 * 
 * Revision 1.19  88/09/30  12:43:43  mz0k
 * changed the tagfield of the CONCATENATION to PRIMITIVE
 * 
 * Revision 1.18  88/09/14  17:07:38  mz0k
 * removed the "ifdef NOETESTED" from around the "AppendSequence's"
 * 
 * Revision 1.17  88/09/07  14:43:49  mz0k
 * Fixed LayoutDirectives according to the new part 5.
 * 
 * Revision 1.16  88/09/06  13:15:13  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.15  88/09/06  12:03:13  annm
 * no change
 * 
 * Revision 1.14  88/08/29  14:10:07  jr
 * dp_INTERCHANGE_FORMAT_CLASS_a ->
 * dp_INTERCHANGE_FMT_CLASS_a
 * 
 * Revision 1.13  88/08/26  16:31:54  mz0k
 * Fixed name too long: WODIF_GeometricGraphicsAttributes and WASN1_CompositeLogicalAttributes
 * 
 * Revision 1.12  88/08/15  10:47:08  mz0k
 * Fixed the at_PROTECTION according to new part 5
 * 
 * Revision 1.11  88/08/04  13:48:32  mz0k
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif1.c,v $";
#endif

#define TK_TKI_IO
#define TK_IMPL_ATTR
#define TK_IMPL_DOC
#define TK_IMPL_CONST
#define TK_GVARS_WODIF1
#define TK_IMPL_WODIF
#define TK_IMPL_ASN1
#define TK_IMPL_WASN1
#define TK_TKI_SEQ
#define TK_TKI_TKERR
#define TK_TKI_LABEL
#define TK_IMPL_INIT

#include <wodif.h>
#include <seq.h>
#include <tkerr.h>
#include <label.h>
#include <init.h>

#undef TK_TKI_IO
#undef TK_IMPL_ATTR
#undef TK_IMPL_DOC
#undef TK_IMPL_CONST
#undef TK_GVARS_WODIF1
#undef TK_IMPL_WODIF
#undef TK_IMPL_ASN1
#undef TK_IMPL_WASN1
#undef TK_TKI_SEQ
#undef TK_TKI_TKERR
#undef TK_TKI_LABEL
#undef TK_IMPL_INIT

/* Forward declarations */
static INT_type	WODIF_SpecificLogical();
static INT_type	WODIF_WalkSpecificLogical();
static INT_type	WODIF_LogicalObjDesc();
static INT_type	WODIF_LogicalObjDescBody();
static INT_type	WODIF_Bindings();

/*
    Interchange-Data-Unit	    ::= CHOICE {
	document-profile		    [0] IMPLICIT Document-Profile-Descriptor,
	layout-object-class		    [1] IMPLICIT Layout-Class-Descriptor,
	layout-object			    [2] IMPLICIT Layout-Object-Descriptor,
	content-portion			    [3] IMPLICIT Text-Unit,
	logical-object-class		    [5] IMPLICIT Logical-Class-Descriptor,
	logical-object			    [6] IMPLICIT Logical-Object-Descriptor,
	presentation-style		    [7] IMPLICIT Presentation-Style-Descriptor,
	layout-style			    [8] IMPLICIT Layout-Style-Descriptor
    }

    Order of the interchange data elements must be as follows (this is interchange
    format class A):

	document profile descriptor
	layout object class descriptors
	logical object class descriptors
	text units representing generic content portions
	presentation style descriptors
	layout style descriptors
	layout object descriptors
	logical object descriptors
	text units representing specific content portions
*/

static INT_type WODIF_SpecificLogical(od, document)
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
    /* If no specific logical structure, handle it */
    if (TheDocument->specific_logical_root == NULL_CONSTITUENT) {
	/* Is this okay, or must it be here ??? */
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /*
       Walk the specific logical structure & spit out
       Logical-Object-Descriptor's
    */
    rval = WODIF_WalkSpecificLogical(od, TheDocument->specific_logical_root);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_DocumentProfile(od, document)
    IN INT_type	    od;
    IN DOCUMENT_type	    document;
{
    INT_type	rval;	    /* The returned value */
    INTERNAL_DOCUMENT_type TheDocument;
    REGISTER INT_type	    tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    /* If no document profile, good news. */
    if (TheDocument->document_profile == NULL_CONSTITUENT) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Write out the Document Profile */
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
    rval = WODIF_DocumentProDescriptor(od, TheDocument->document_profile, tagfield);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WriteODIF(od, document, format) 
    IN INT_type		od;
    IN DOCUMENT_type	document;
    IN VALUE_type	format;
{

    INT_type		rval;	/* The returned value */

    REGISTER INT_type	i;

    INT_type		dummy;

    INT_type		SlaveRval;
    static INT_type (*WriteProcs[])() = {
	WODIF_DocumentProfile,
	WODIF_GenericLayout,
	WODIF_GenericLogical,
	WODIF_GenericContent,
	WODIF_PresentationStyle,
	WODIF_LayoutStyle,
	WODIF_SpecificLayout,
	WODIF_SpecificLogical,
	WODIF_SpecificContent
    };

#define NPROCS	((INT_type)(sizeof(WriteProcs) / sizeof(WriteProcs[0])))

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Interchange format class B unimplemented */
    if (format == dp_INTERCHANGE_FMT_CLASS_b) {
	rval = ERROR_INT;
	TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[WriteODIF]: The interchange class B is not  implemented" );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }

    /* Have document labeled@@@@ */
	

    /* Make a sequence for the specific content portions first */
    SequenceOfSpeContents = MakeSequence(SEQUENCE_CONSTITUENT_tag,  (INT_type) 0);
    if (SequenceOfSpeContents == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    /* Make a sequence for the generic content portions  */
    SequenceOfGenContents = MakeSequence(SEQUENCE_CONSTITUENT_tag,  (INT_type) 0);
    if (SequenceOfGenContents == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    /* Make a sequence for the logical object class */
    SequenceOfLogClass = MakeSequence(SEQUENCE_CONSTITUENT_tag, (INT_type) 0);
    if (SequenceOfLogClass == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* see if we need to label the doucment */
    if( InitParms->automatic_label == BOOL_true ){
	SlaveRval = LabelDocument( document );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[WriteODIF]: can not label the document" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    /* Write out each part in order */
    for (i=0; i<NPROCS; i++) {
	rval = (*WriteProcs[i])(od, document);
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

static INT_type WODIF_WalkSpecificLogical(od, constituent)
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
   
    /* Write out the attributes of the logical object descriptor first */
    tagfield = MakeTag( CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 6);
    rval = WODIF_LogicalObjDesc(od, constituent, tagfield);
 
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
		rval = WODIF_WalkSpecificLogical(od, children->sequence_value.constituents[i]);
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

/*
    Logical-Object-Descriptor		::= SEQUENCE {
	object-type			    Logical-Object-Type OPTIONAL,
	descriptor-body			    Logical-Object-Descriptor-Body OPTIONAL
    }
*/

static INT_type WODIF_LogicalObjDesc(od, object, tag)
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

    /* Now have subfields written */
    
    if (HasAttr(object, at_OBJECT_TYPE)==NO_ATTR) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[WODIF_LogicalObjDesc]: The Logical Object doesn't have an at_OBJECT_TYPE." );
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
	rval = WODIF_LogicalObjDescBody(od, object, tagfield);
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


/*
    Logical-Object-Descriptor-Body  ::= SET {
	object-identifier		Object-or-Class-Identifier OPTIONAL,
	subordinates			[0] IMPLICIT SEQUENCE OF NumericString OPTIONAL,
	content-portions		[1] IMPLICIT SEQUENCE OF NumericString OPTIONAL,
	object-class			[2] IMPLICIT Object-or-Class-Identifier OPTIONAL,
	presentation-attributes		[6] IMPLICIT Presentation-Attributes OPTIONAL,
	default-value-lists		[7] IMPLICIT Default-Value-Lists-Logical OPTIONAL,
	user-readable-comments		[8] IMPLICIT Comment-String OPTIONAL,
	bindings			[9] IMPLICIT SET OF Binding-Pair OPTIONAL,
	content-generator	       [10] IMPLICIT String-Expression OPTIONAL,
	user-visible-name	       [14] IMPLICIT Comment-String OPTIONAL,
	presentation-style	       [17] IMPLICIT Style-Identifier OPTIONAL,
	layout-style		       [19] IMPLICIT Style-Identifier OPTIONAL,
	property-attributes	       [20] IMPLICIT Property-Attributes OPTIONAL,
	layout-directives	       [25] IMPLICIT Layout-Directives OPTIONAL
    }
*/

static INT_type WODIF_LogicalObjDescBody(od, object, tag)
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
    if ((rval != ERROR_INT) && (HasAttr(object, at_OBJECT_IDENTIFIER) == HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Strings(od, object, tagfield, at_OBJECT_IDENTIFIER);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_SUBORDINATES) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WODIF_SequenceConstituents(od, object, at_SUBORDINATES, at_OBJECT_IDENTIFIER, tagfield, BOOL_false);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_CONTENT_PORTIONS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WODIF_SequenceConstituents(od, object, at_CONTENT_PORTIONS, at_CONTENT_ID_LOGICAL, tagfield, BOOL_false);
    }
    if (rval != ERROR_INT && (HasAttr(object, at_OBJECT_CLASS) == HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
        rval = WASN1_ConstituentId(od, object, at_OBJECT_CLASS, at_OBJECT_CLASS_IDENTIFIER, tagfield);
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
#endif

    if((rval != ERROR_INT) && (HasAttr(object, at_USER_VISIBLE_NAME)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 14);
	rval = WASN1_Strings(od, object, tagfield, at_USER_VISIBLE_NAME);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 17);
	rval = WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE, at_PRESENTATION_STYLE_ID, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_STYLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 19);
	rval = WASN1_ConstituentId(od, object, at_LAYOUT_STYLE, at_LAYOUT_STYLE_IDENTIFIER, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PROTECTION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 20);
	rval = WASN1_ValueType(od, object, tagfield, at_PROTECTION);
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

INT_type WODIF_SequenceConstituents(od, object, attrid, idattrid, tag, isclass)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    attrid;	/* Attribute of object with sequence of constituents value */
    IN INT_type	    idattrid;	/* Attribute of each constituent that contains identifier */
    IN INT_type	    tag;
    IN BOOL_type    isclass;	/* Is the at_CONTENT_PORTIONS from a specific or generic
				 structure */

{
     INT_type			rval;	/* The returned value */
    ATTR_attribute_t		attr;
    SEQUENCE_CONSTITUENT_type	constituents;
    REGISTER INT_type		i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Point to the appropriate attribute */
    attr = ATTR_FindAttr(object, attrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);   

    /* Now, have each constituent identifier written */
    constituents = GET_SEQUENCE_CONSTITUENT(attr);
    for (i=0; rval!=ERROR_INT && i<constituents->length; i++) {
	rval = WASN1_Constituent_LastId(od, constituents->sequence_value.constituents[i], idattrid);
    }
 
    /*  Save the sequence of constituents of CONTENT PORTIONS for
	later use. 
	*/
    if ((rval != ERROR_INT) && (attrid == at_CONTENT_PORTIONS)) {
	if (isclass) {
	    rval = AppendSequence(SequenceOfGenContents, constituents);
	    if (rval ==	ERROR_INT)  {
		goto LEAVE;
	    }
	}
	else {
	    rval = AppendSequence(SequenceOfSpeContents, constituents);
	    if (rval ==	ERROR_INT)  {
		goto LEAVE;
	    }
	}
    }

    /* Close the sequence */
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


#ifdef NOTTESTED
INT_type WODIF_Final()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WODIF_Init()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
#endif 

/* Presentation-Attributes	    ::= SET {
      content-type			Content-Type OPTIONAL;
      content-architecture-class	Content-Architecture-Class OPTIONAL;
      character-attributes		[0] IMPLICIT Character-Attribtes OPTIONAL;
      raster-graphics-attributes	[1] IMPLICIT Raster-Graphics-Attributes OPTIONAL;
      geometrics-graphics-attributes	[2] IMPLICIT Geometrics-Graphics-Attributes OPTIONAL;}

	-- Character-Attributes is defined in part 6
	-- Raster-Graphics-Attributes is defined in part 7
	-- Geometrics-Graphics-Attributes is defined in part 8
	-- the following tags are reserved for additional types
	-- of presentation attributes:
	    -- [3] composite graphics 
	    -- [4] audio
	    -- [5] dynamic-graphics
 */
INT_type WODIF_PresentationAttributes(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type			rval;	/* The returned value */
    INT_type			tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* Write the set header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);   

    /* Now have each of the set element written */
    if ((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_TYPE) == HAS_ATTR)){
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, 2);
	rval = WASN1_ValueType(od, object, tagfield, at_CONTENT_TYPE);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_ARCH_CLASS)==HAS_ATTR)) {
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, OBJECT_IDENTIFIER_TAG);
	rval = WASN1_Asn1ObjectId(od, object, tagfield, at_CONTENT_ARCH_CLASS);
    }
    /* Follows the character content attributes */
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WODIF_CharacterAttributes(od, object, tagfield);
    }
    /* Follows the raster graphics attributes */
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WODIF_RasterGraphicsAttributes(od, object, tagfield);
    }

#ifdef NOTTESTED
    /* Then the geometric graphics attributes */
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WODIF_GeometricGrAttributes(od, object, tagfield);
    }
#endif
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


INT_type WODIF_DefaultValueListsLogical(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
	/* Notice that the default-value-lists is represented as a list
            of constituents in the API.  In order to write out those attributes
            represented in ASN1,  it's necessary to go through all these 
           constituents and, for each constituent,  make sure what it is,
            and decide what attributes could be defaultable in that specific
             constituent according to Table 1 of setction 5.3.3.5 of Part 2 of 
            ISO 8613.
	       */

    INT_type			rval;
    REGISTER INT_type		tagfield;
    at_DEFAULT_VALUE_LISTS_type *TheDVL;
    REGISTER INT_type		i;
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

    /* Write the set header for the default-value-lists-logical */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Now,  look through the constituents and find out the constituent of the
      composite logical objects  and write the attributes out */
    TheDVL = GET_at_DEFAULT_VALUE_LISTS(attr);

    if(TheDVL->composite_logical_attributes != NULL_CONSTITUENT){
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	rval = WASN1_CompLogicalAttributes(od, 
		    TheDVL->composite_logical_attributes, tagfield);
	if(rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if ( TheDVL->basic_logical_attributes != NULL_CONSTITUENT ) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 6);
	rval = WASN1_BasicLogicalAttributes(od, 
		    TheDVL->basic_logical_attributes, tagfield);
	if(rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    
    /* Close the sequence */
    rval = WASN1_EOC(od);
        
	
LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}


    
#ifdef NOTTESTED
static INT_type WODIF_Bindings(od, object, attrid, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    attrid;
    IN INT_type	    tag;
{
    INT_type	    rval;
    SEQUENCE_BINDING_PAIR_type bodypt;
    ATTR_attrinute_t attr;
    REGISTER INT_type i;

#ifdef DEBUG
    if( Traceroutine() ) {
	Entering();
    }
#endif


    /* Find the bindings attribute */
    attr = ATTR_FindAttr(object, idattrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* The Bindings header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if(rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Get the pointer to the sequence of binding-pairs */
    bodypt = GET_SEQUENCE_BINDING_PAIR(attr);

    for (i=0; (i<=bodypt->length-1) && (rval != ERROR_INT); i++) {
	rval = WASN1_Binding_Pair(od, bodypt->sequence_value.binding_pairs[i]);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
#endif


INT_type WODIF_LayoutDirectives(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
	
    INT_type	    rval;
    INT_type	    tagfield;

#ifdef DEBUG
    if( Traceroutine() ) {
	Entering();
    }
#endif

    /* Write the set header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each attribute follows */
    
    if((rval != ERROR_INT) && (HasAttr(object, at_INDIVISIBILITY)==HAS_ATTR)) {
	rval = WASN1_LayoutObjectType(od, object, at_INDIVISIBILITY);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_SEPARATION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_Separation(od, object, tagfield);
    }
    if((rval != ERROR_INT) &&(HasAttr(object, at_OFFSET)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 4);
	rval = WASN1_Offset(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_CONCATENATION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 6);
	rval = WASN1_ValueType(od, object, tagfield, at_CONCATENATION);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, at_NEW_LAYOUT_OBJECT)==HAS_ATTR)) {
	rval = WASN1_LayoutObjectType(od, object, at_NEW_LAYOUT_OBJECT);
    }

/* ??????????????????????????????????????? */

    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_OBJECT_CLASS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 11);
	rval = WASN1_ConstituentId(od, object, at_LAYOUT_OBJECT_CLASS, at_OBJECT_CLASS_IDENTIFIER, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_CATEGORY)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 12);
	rval = WASN1_Strings(od, object, tagfield, at_LAYOUT_CATEGORY);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_BLK_ALGNM)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 14);
	rval = WASN1_ValueType(od, object, tagfield, at_BLK_ALGNM);
    }


    /* If everything goes well, close the set */
    if (rval !=	ERROR_INT ) {
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
