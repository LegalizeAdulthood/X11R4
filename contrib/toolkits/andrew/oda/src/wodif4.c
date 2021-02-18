
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif4.c,v 1.1 89/09/21 06:45:13 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif4.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif4.c,v $
 * Revision 1.1  89/09/21  06:45:13  mss
 * Initial revision
 * 
 * Revision 1.5  89/03/27  12:33:42  mss
 * Eliminated several passed-by-value structures
 * (changed to pass by reference)
 * 
 * Revision 1.4  89/01/24  12:20:09  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.3  88/11/04  01:37:17  mz0k
 * Allowed primitive tag for the Structured Name
 * 
 * Revision 1.2  88/10/21  16:46:18  mz0k
 * added font stuffs
 * 
 * Revision 1.1  88/10/13  22:18:55  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$ $";
#endif

#define TK_TKI_IO
#define TK_IMPL_ATTR
#define TK_TKI_DOC
#define TK_IMPL_CONST
#define TK_GVARS_WODIF4
#define TK_IMPL_WODIF
#define TK_IMPL_ASN1
#define TK_IMPL_WASN1
#define TK_TKI_DEBUG
#define TK_TKI_FONT
#define TK_TKI_SEQ

#include <wodif.h>
#include <tags.h>
#include <font.h>
#include <seq.h>

#undef TK_TKI_IO
#undef TK_IMPL_ATTR
#undef TK_TKI_DOC
#undef TK_IMPL_CONST
#undef TK_GVARS_WODIF4
#undef TK_IMPL_WODIF
#undef TK_IMPL_ASN1
#undef TK_IMPL_WASN1
#undef TK_TKI_DEBUG
#undef TK_TKI_FONT
#undef TK_TKI_SEQ

/* Forward declarations */
static INT_type WODIF_DocumentCharacteristics();
static INT_type WODIF_AdditionalDocChars();
static INT_type WODIF_FontsList();
static INT_type WODIF_FontPair();
static INT_type WODIF_FontReference();

INT_type WODIF_RasterGraphicsAttributes(od, object, tag)
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
    if ((rval != ERROR_INT) && (HasAttr(object, rg_PEL_PATH) == HAS_ATTR)){
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_ValueType(od, object, tagfield, rg_PEL_PATH);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_LINE_PROGRESSION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_ValueType(od, object, tagfield, rg_LINE_PROGRESSION);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_PEL_TRANS_DENSITY)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, rg_PEL_TRANS_DENSITY);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_INITIAL_OFFSET)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_IntPair(od, object, tagfield, rg_INITIAL_OFFSET);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_CLIPPING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 4);
	rval = WASN1_RgClipping(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_PEL_SPACING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	rval = WASN1_RgPelSpacing(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_SPACING_RATIO)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 6);
	rval = WASN1_IntPair(od, object, tagfield, rg_SPACING_RATIO);
    }
    if((rval != ERROR_INT) && (HasAttr(object, rg_IMAGE_DIMENSIONS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 7);
	rval = WASN1_RgImageDimensions(od, object, tagfield);
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


INT_type WODIF_DocumentProDescriptor(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{
    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the header for the document profile */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);

    /* Now have each field written */
    
    /* Let me jump over all the stuffs for now except for the font-list @@@@ */
    
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WODIF_DocumentCharacteristics(od, object, tagfield);
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

static INT_type WODIF_DocumentCharacteristics(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{
    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the header  */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);

    /* Now have each field written */
    
    /* Let me jump over all the stuffs for now except for the font-list @@@@ */
    
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 9);
	rval = WODIF_AdditionalDocChars(od, object, tagfield);
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

static INT_type WODIF_AdditionalDocChars(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{  
	/* SET {
	 unit-scaling	    [3] IMPLICIT SEQUENCE {INTEGER, INTEGER} OPTIONAL,
	 font-list		    [2] IMPLICIT Font-List OPTIONAL;}
    */

    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the header  */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);

    /* Now have each field written */
    
    if((rval != ERROR_INT) && (HasAttr(object, dp_UNIT_SCALING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_IntPair(od, object, tagfield, dp_UNIT_SCALING);
    }
    if ((rval != ERROR_INT) && (HasAttr(object, dp_FONTS_LIST)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WODIF_FontsList(od, object, tagfield);
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

static INT_type WODIF_FontsList(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type    tag;
{  
	/* 
    */

    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield, i;
    ATTR_attribute_t	    attr;
    SEQUENCE_FONT_PAIR_type font_pairs;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

     attr = ATTR_FindAttr(object, dp_FONTS_LIST);   
     if (attr == ERROR_ATTRIBUTE) {
	 rval = ERROR_INT;
	 goto LEAVE;
     }

     /* Write the header for the sequence */
     rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
     if (rval == ERROR_INT) {
	 goto LEAVE;
     }
     
     /* Get the pointer to the sequence of font-pairs */
     font_pairs = GET_SEQUENCE_FONT_PAIR(attr);

    /* Now the monstrous fonts-list.  It's a sequence of set {font-identifier,  font-reference}. */

    /* The header for the set itself  */
     tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SET_TAG);
     for (i=0; i< font_pairs->length; i++) {
	 rval = WODIF_FontPair(od, tagfield, &font_pairs->sequence_value.font_pairs[i]);
	 if (rval == ERROR_INT) {
	     goto LEAVE;
	 }
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

static INT_type WODIF_FontPair(od, tag, font_pair)
    IN INT_type		od;
    IN INT_type		tag;
    IN FONT_PAIR_type	*font_pair;
{  
	/* 
    */

    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


     /* Write the header for the set */
     rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
     if (rval == ERROR_INT) {
	 goto LEAVE;
     }
     

    /*  {font-identifier,  font-reference}. */
     tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
     rval = WASN1_Integer(od, tagfield, font_pair->font_id);
     if (rval == ERROR_INT) {
	 goto LEAVE;
     }

     tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
     rval = WODIF_FontReference(od, tagfield, &font_pair->font_reference);
     if	(rval == ERROR_INT) {	
	 goto LEAVE;
     }

    /* If still okay, close up */
     rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WODIF_FontReference(od, tag, reference)
    IN INT_type		od;
    IN INT_type		tag;
    IN FONT_REFERENCE_type  *reference;
{  
	/* 
    */

    INT_type	rval;	    /* The returned value */
    REGISTER INT_type	    tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


     /* Write the header for the sequence */
     rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
     if (rval == ERROR_INT) {
	 goto LEAVE;
     }
     
     /* Here it goes */

     /* Assume that the default name prefix is always absent */

     if (rval != ERROR_INT) {
	 if (reference->fontname.structured_name_tag == STRUCTURED_NAME_ID_tag) {
	     tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	     rval = WASN1_StructuredName(od, tagfield, &reference->fontname);
	 }
	 else if (reference->fontname.structured_name_tag == STRUCTURED_NAME_SEQ_tag) {
	     tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	     rval = WASN1_StructuredName(od, tagfield, &reference->fontname);
	 }
     }

     
     if (rval != ERROR_INT) {
	 if (reference->family.structured_name_tag == STRUCTURED_NAME_ID_tag) {
	     tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 5);
	     rval = WASN1_StructuredName(od, tagfield, &reference->family);
	 }
	 else if (reference->family.structured_name_tag == STRUCTURED_NAME_SEQ_tag) {
	     tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	     rval = WASN1_StructuredName(od, tagfield, &reference->family);
	 }
     }

     if ((rval != ERROR_INT) && (reference->posture != ISO_POSTURE_unspec)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 6);
	 rval = WASN1_Integer(od, tagfield, reference->posture);
     }

     if ((rval != ERROR_INT) && (reference->weight != ISO_WEIGHT_unspec)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 7);
	 rval = WASN1_Integer(od, tagfield, reference->weight);
     }

     if ((rval != ERROR_INT) && (reference->propwdth != ISO_PROPWDTH_unspec)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 8);
	 rval = WASN1_Integer(od, tagfield, reference->propwdth);
     }

     if ((rval != ERROR_INT) && (reference->dsnsize.second != 0)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 12);
	 rval = WASN1_Real(od, tagfield, &reference->dsnsize);
     }
     
     if ((rval != ERROR_INT) && (reference->dsclass != NULL_SEQUENCE)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 15);
	 rval = WASN1_SeqIntsInStruct(od, tagfield, reference->dsclass);
     }

     if ((rval != ERROR_INT) && (reference->structur != ISO_STRUCTUR_unspec)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 16);
	 rval = WASN1_Integer(od, tagfield, reference->structur);
     }

     if ((rval != ERROR_INT) && (reference->modes.specified != BOOL_false)) {
	 tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 18);
	 rval = WASN1_Modes(od, tagfield, &reference->modes);
     }
     /* you are here */

    /* If still okay, close up */
     rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}
