
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn13.c,v 1.6 89/11/01 11:12:24 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn13.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn13.c,v $
 * Revision 1.6  89/11/01  11:12:24  jr
 * Fiex for bad calls to IO_WriteBytes
 * 
 * Revision 1.5  89/11/01  10:05:20  mss
 * Fixed typo in cc_LINE_LAYOUT_TABLE
 * 
 * Revision 1.4  89/11/01  09:42:05  mss
 * Rearranged comments so that braces match
 * 
 * Revision 1.3  89/10/31  14:53:55  jr
 * Casting problems fixed.
 * 
 * Revision 1.2  89/10/26  14:13:23  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:42:39  mss
 * Initial revision
 * 
 * Revision 1.20  89/03/27  12:32:58  mss
 * Eliminated several passed-by-value structures
 * (changed to pass by reference)
 * 
 * Revision 1.19  89/01/18  18:44:59  annm
 * #if 0'ed out the code refering to the HORIZONTAL_COORDINATE_parm, VERTICAL_COORDINATE_parm, START_EDGE_OFFSET_parm and END_EDGE_OFFSET_parm
 * 
 * Revision 1.18  88/11/01  14:46:35  mz0k
 * Added TKError messages
 * 
 * Revision 1.17  88/10/28  12:56:47  mz0k
 * fixed the tags for the CharacterFonts
 * Allowed zero and negative integers
 * 
 * Revision 1.16  88/10/21  16:30:14  mz0k
 * added WASN1_Real
 * added WASN1_SeqByteInStrucut
 * 
 * Revision 1.15  88/10/18  17:56:12  mz0k
 * added the case of ALL_PARMS for cc_INITIAL_OFFSET
 * 
 * Revision 1.14  88/10/18  14:25:33  mz0k
 * considered parameters for cc_INITIAL_OFFSET and cc_KERNING_OFFSET
 * 
 * Revision 1.13  88/10/13  21:59:20  mz0k
 * added more attributes to the GetIntPair
 * 
 * Revision 1.12  88/10/10  17:38:14  mz0k
 * tagfield --> tag in WASN1_ContentPortionAttributes
 * 
 * Revision 1.11  88/10/07  11:41:57  mss
 * Changed SequenceBytes so that writing 0 bytes does
 * not close file prematurely
 * 
 * Revision 1.10  88/10/06  18:33:26  mz0k
 * irregular use of register variable
 * 
 * Revision 1.9  88/09/22  14:56:08  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.8  88/09/14  17:07:26  mz0k
 * removed the "ifdef NOETESTED" from around the "AppendSequence's"
 * 
 * Revision 1.7  88/09/06  13:14:55  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.6  88/08/11  09:08:21  jr
 * Missed a cc_Itemisation.
 * 
 * Revision 1.5  88/08/08  15:19:08  mz0k
 * Fixed the first component of the sequence of ints
 * 
 * Revision 1.3  88/08/04  14:25:42  mz0k
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn13.c,v $";
#endif


#define TK_IMPL_WASN1
#define TK_GVARS_WASN13
#define TK_IMPL_ATTR
#define TK_TKI_CONST
#define TK_TKI_IO
#define TK_TKI_TKERR

#include <parms.h>
#include <wasn1.h>

#undef TK_IMPL_WASN1
#undef TK_GVARS_WASN13
#undef TK_IMPL_ATTR
#undef TK_TKI_CONST
#undef TK_TKI_IO
#undef TK_TKI_TKERR



INT_type WASN1_IntPair(od, object, tag, attrid)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
    IN INT_type	    attrid;
{
	/* This routine assumes that:
	 A) The attribute it's writing is a SEQUENCE or a SET of two subattribute;
           */

    INT_type		rval;	    /* The returned value */
    ATTR_attribute_t	attr;
    INT_PAIR_type	*int_pair;
    INT_type	tagfield[2];
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Get the attribute & look at it */
    attr = ATTR_FindAttr(object, attrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    
    /* Write the sequence header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
   
    /* Get the integer pair and write them */
    int_pair = GET_INT_PAIR(attr);

    switch (attrid) {
	    case at_PAGE_POSITION:  
	    case cc_INITIAL_OFFSET:
	    case rg_INITIAL_OFFSET:
		tagfield[0] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
		tagfield[1] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
		break;
	    case cc_KERNING_OFFSET:
		tagfield[0] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
		tagfield[1] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	    case rg_SPACING_RATIO:
	    case dp_UNIT_SCALING:
		tagfield[0] = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
		tagfield[1] = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
		break;
	    default:
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_IntPair]: Invalid attribute id: 0d%d", attrid);
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
    }

    /* cc_INITIAL_OFFSET has parms, thus deal with it specifically */
    if (attrid == cc_INITIAL_OFFSET) {
#if 0
/* Jonthan - please get rid of this - Ann */
	if (((attr->parm_mask & HORIZONTAL_COORDINATE_parm) != 0) || (attr->parm_mask == ALL_PARMS)){
#endif
	    rval = WASN1_Integer(od, tagfield[0], int_pair->first);
	    if (rval ==	ERROR_INT) {
		goto LEAVE;
	    }
#if 0
/* Jonthan - please get rid of this - Ann */
	}
	if (((attr->parm_mask & VERTICAL_COORDINATE_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
#endif
	    rval = WASN1_Integer(od, tagfield[1], int_pair->second);
	    if (rval ==	ERROR_INT) {
		goto LEAVE;
	    }
#if 0
/* Jonthan - please get rid of this - Ann */
	}
#endif
    }
    else if (attrid == cc_KERNING_OFFSET) {
	/* the cc_KERNING_OFFSET is also special */
#if 0
/* Jonthan - please get rid of this - Ann */
	if (((attr->parm_mask & START_EDGE_OFFSET_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
#endif
	    rval = WASN1_Integer(od, tagfield[0], int_pair->first);
	    if (rval ==	ERROR_INT) {
		goto LEAVE;
	    }
#if 0
/* Jonthan - please get rid of this - Ann */
	}
	if (((attr->parm_mask & END_EDGE_OFFSET_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
#endif
	    rval = WASN1_Integer(od, tagfield[1], int_pair->second);
	    if (rval ==	ERROR_INT) {
		goto LEAVE;
	    }
#if 0
/* Jonthan - please get rid of this - Ann */
	}
#endif
    }
    else {
	rval = WASN1_Integer(od, tagfield[0], int_pair->first);
	if (rval != ERROR_INT) {
	    rval = WASN1_Integer(od, tagfield[1], int_pair->second);
	}
    }

    /* Close the sequence or set */
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


INT_type WASN1_Real(od, tag, int_pair)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN INT_PAIR_type	*int_pair;
{
	/* To write an int_pair in a structure.  (Those INT_PAIR_type's which are attributes are done with WASN1_IntPair.)
           */

    INT_type		rval;	    /* The returned value */
    INT_type		tagfield;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header first */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Then the two integers */
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
    rval = WASN1_Integer(od, tagfield, int_pair->first);
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Integer(od, tagfield, int_pair->second);
    }
    

    /* Close the sequence  */
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


INT_type WASN1_IntType(od, object, tag, attrid)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
    IN INT_type	    attrid;
{
    INT_type		rval;	    /* The returned value */
    ATTR_attribute_t	attr;
    INT_type		intvalue;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Get the attribute & look at it */
    attr = ATTR_FindAttr(object, attrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
   
    /* Get the integer and write it */
    intvalue = GET_INT(attr);

    rval = WASN1_Integer(od, tag, intvalue);


LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}


INT_type WASN1_LineLayoutTable(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type			rval, val;
    ATTR_attribute_t		attr;
    SEQUENCE_TAB_STOP_type	sequence;
    REGISTER INT_type		tagfield;
    REGISTER INT_type		i;
static STRING_type		reference, alignment_string;
    INT_type		dummy;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Point to the appropriate attribute */
    attr = ATTR_FindAttr(object, cc_LINE_LAYOUT_TABLE);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);  

    /* Notice that the attr is a pointer to a sequence of TAB_STOP_type values */
    sequence = GET_SEQUENCE_TAB_STOP(attr);
    for(i=0; (i <= sequence->length-1) && (rval != ERROR_INT); i++) {

	/* Write the set header for each of the tabulation-stop */
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SET_TAG);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* Then each of the attribute, which is a field of a structure in API */

	  /* First the tabulation reference */
	reference = (sequence->sequence_value.tab_stops[i]).tab_reference;
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = IO_WriteByte(od, MakeByte(tagfield));
	if (rval != ERROR_INT) {
	    rval = WASN1_Length(od, reference->length);
	}
	if (rval != ERROR_INT) {
	    rval = IO_WriteChars(od, reference->sequence_value.chars, reference->length);
	}

	  /* Then the tabulation postion, a VALUE_type */

#if 0
/***** Comment out for now -- type change in 3rd arg. */
	if (rval != ERROR_INT) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	    rval = WASN1_Integer(od, tagfield, (sequence->sequence_value.tab_stops[i]).tab_reference);
	}
#endif
	
	  /* Then the alignment */
	if (rval != ERROR_INT) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	    switch ((sequence->sequence_value.tab_stops[i]).alignment) {
		    case cc_LINE_LAYOUT_TABLE_start_alnd:
			val = 0;
			break;
		    case cc_LINE_LAYOUT_TABLE_end_alnd:
			val = 1;
			break;
		    case cc_LINE_LAYOUT_TABLE_centred:
			val = 2;
			break;
		    case cc_LINE_LAYOUT_TABLE_alnd_arnd:
			val = 3;
			break;
		    default:
			rval = ERROR_INT;
			TKError = INTERNAL_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[WASN1_LineLayoutTable]: Invalid alignment value: 0d%d", (INT_type) (sequence->sequence_value.tab_stops[i]).alignment);
			dummy = TKERR_EndErrMsg();
			goto LEAVE;
	    }
	    rval = WASN1_Integer(od, tagfield, val);
	}

	  /* Then the alignment string */
	if (rval != ERROR_INT) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	    alignment_string = (sequence->sequence_value.tab_stops[i]).alignment_string;
	    rval = IO_WriteByte(od, MakeByte(tagfield));
	    if (rval != ERROR_INT) {
		rval = WASN1_Length(od, alignment_string->length);
	    }
	    if (rval != ERROR_INT) {
		rval = IO_WriteChars(od, alignment_string->sequence_value.chars, alignment_string->length);
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


INT_type WASN1_GraphicRendition(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type rval;
    SEQUENCE_INT_type	sequence;
    ATTR_attribute_t	attr;
    REGISTER INT_type	tagfield;
    REGISTER INT_type   i;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, cc_GRAPHIC_RENDITION);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the set header for the graphic rendition */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Get the sequence of ints and write them */
    sequence = GET_SEQUENCE_INT(attr);
    tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
    for	(i=0; (i<=sequence->length-1) && (rval != ERROR_INT); i++) {
	rval = WASN1_Integer(od, tagfield, sequence->sequence_value.ints[i]);
    }
    
    /* Close the set */
    if (rval !=	ERROR_INT) {
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

INT_type WASN1_CharacterFonts(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type			rval;
    SEQUENCE_INT_PAIR_type	sequence;
    ATTR_attribute_t		attr;
    REGISTER INT_type		tagfield;
    REGISTER INT_type		i;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, cc_CHARACTER_FONTS);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the set header for the character fonts */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Get the sequence of pair-of-ints and write them, each of which is a set of two ints */
    sequence = GET_SEQUENCE_INT_PAIR(attr);
    
    for	(i=0; (i<=sequence->length-1) && (rval != ERROR_INT); i++) {
	
	/* Write the set header for the font-type first */
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) i);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	
	/* Then each of the two integers */
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Integer(od, tagfield, sequence->sequence_value.int_pairs[i].first);
	if (rval != ERROR_INT) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	    rval = WASN1_Integer(od, tagfield, sequence->sequence_value.int_pairs[i].second);
	}
	
	/* Close the set of font-type */
	if (rval != ERROR_INT ) {
	    rval = WASN1_EOC(od);
	}
    }

    /* Close the set of character fonts */
    if (rval !=	ERROR_INT) {
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

INT_type WASN1_Itemisation(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type			rval;	 
    REGISTER INT_type		tagfield;
    ATTR_attribute_t		attr;
    cc_ITEMISATION_type		*itemisation;
    INT_type			val;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, cc_ITEMISATION);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the set header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Get the struct form the attr */
    itemisation= GET_cc_ITEMISATION(attr);
    
    /* Write each of the attributes in the set . . . the identifier-alignment first */

    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
    switch (itemisation->identifier_alignment) {
	    case cc_ITEMISATION_no_itemisation:
		val = 0;
		break;
	    case cc_ITEMISATION_start_aligned:
		val = 1;
		break;
	    case cc_ITEMISATION_end_aligned:
		val = 2;
		break;
	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_Itemisation]: Invalid value: 0d%d", (INT_type) itemisation->identifier_alignment);
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
    }
    rval = WASN1_Integer(od, tagfield, val);

    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Integer(od, tagfield, itemisation->identifier_start_offset);
    }
    if (rval != ERROR_INT) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_Integer(od, tagfield, itemisation->identifier_end_offset);
    }
    /* Close the set */
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




INT_type WASN1_ContentPortionAttributes(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type	rval;	 
REGISTER INT_type	tagfield;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the set header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);

    /* Then each attribute */
    if((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_ID_LAYOUT)==HAS_ATTR)) {
	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Strings(od, object, tagfield, at_CONTENT_ID_LAYOUT);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_ID_LOGICAL)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 4);
	rval = WASN1_Strings(od, object, tagfield, at_CONTENT_ID_LOGICAL);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_TYPE_OF_CODING)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 6);
	rval = WASN1_Asn1ObjectId(od, object, tagfield, at_TYPE_OF_CODING);
    }

    /* no character_coding_attributes are defined so far */
    if (rval != ERROR_INT) {
	rval = WASN1_RasterCodingAttributes(od, object);
    }
    /* forget about the geometric graphic coding and the ext-cont-arch-coding-attributes for now */
    
    if((rval != ERROR_INT) && (HasAttr(object, at_ALT_REP)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type ) 3);
	rval = WASN1_Strings(od, object, tagfield, at_ALT_REP);
    }

    /* Close the set */
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

INT_type WASN1_SequenceBytes(od, object, tag, attrid)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
    IN INT_type	    attrid;
{
	/* This is to write the SEQUENCE_BYTE_type data as an attribute */

    INT_type	rval;	 
    ATTR_attribute_t	attr;
    SEQUENCE_BYTE_type	sequence;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the attribute  first */
    attr = ATTR_FindAttr(object, attrid);
    if (attr ==  ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the idoctet */
    rval = IO_WriteByte(od, MakeByte(tag));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Get the sequence of bytes */
    sequence = GET_SEQUENCE_BYTE(attr);

    /* Then write the length-octet */
    rval = WASN1_Length(od, sequence->length);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* And then write the contents of those bytes -- note: */
    /* should not write out 0 length bytes, since */
    /* that tells the IO_writebytes routine to close the file */
    if (sequence->length != 0) {
	rval = IO_WriteBytes(od, sequence->sequence_value.bytes,
			     sequence->length);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_SeqBytesInStruct(od, tag, seqbyte)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN SEQUENCE_BYTE_type   seqbyte;
{
	/* This routine is to write the SEQUENCE_BYTE_type datas in a structure. */

    INT_type	rval;	

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Write the idoctet */
    rval = IO_WriteByte(od, MakeByte(tag));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Then write the length-octet */
    rval = WASN1_Length(od, seqbyte->length);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* And then write the contents of those bytes -- note: */
    /* should not write out 0 length bytes, since */
    /* that tells the IO_writebytes routine to close the file */
    if (seqbyte->length != 0) {
	rval = IO_WriteBytes(od, seqbyte->sequence_value.bytes,
			     seqbyte->length);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_Asn1ObjectId(od, object, tag, attrid)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
    IN INT_type	    attrid;
{   /* Make this a definite length to avoid mis-interpretation of 
      two consecutive 0's as an EOC.  */

    INT_type	rval;	 
    ATTR_attribute_t	attr;
    ASN1_OBJECT_ID_type	sequence;
    INT_type		nbytes;
    INT_type		component;
    BYTE_type		val;
    REGISTER INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    nbytes = 0;

    /* Get the attribute  first */
    attr = ATTR_FindAttr(object, attrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the idoctet */
    rval = IO_WriteByte(od, MakeByte(tag));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Get the sequence of ints */
    sequence = GET_SEQUENCE_INT(attr);

    /* Get the length value and write the length octet */
    nbytes = Nbytes((INT_type)((sequence->sequence_value.ints[0] * 40) + 
		     sequence->sequence_value.ints[1]), (INT_type) 7);
    for (i=2; i<=sequence->length-1; i++) {
	nbytes += Nbytes(sequence->sequence_value.ints[i], (INT_type) 7);
    }

    rval = WASN1_Length(od, nbytes);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write the content octets */
    /* The first component */
    component = (sequence->sequence_value.ints[0] * 40)+
                                sequence->sequence_value.ints[1]; 

    i = 1;
    while (i<=sequence->length-1)  {
	nbytes = Nbytes(component, (INT_type) 7);
	for (; rval != ERROR_INT && nbytes>0; nbytes--) {
	    if (nbytes == 1) {
		val = (BYTE_type) (component & 0x7F);
	    }
	    else {
		val = (BYTE_type) ((component >> ((nbytes-1)*7)) | 0x80);
	    }            
	    rval = IO_WriteByte(od,  val);
	}
	/* The next component */
	component = sequence->sequence_value.ints[++i];
    }

    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

#ifdef NOTTESTED
INT_type WASN1_Border(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type			rval;	 
    REGISTER INT_type		tagfield;
    ATTR_attribute_t		attr;
    at_BORDER_type		*border;
    INT_type			val;

    /* The part5-2 is obsolute on the "border",  skip over this */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, at_BORDER);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the set header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Get the struct form the attr */
    border = GET_at_BORDER(attr);
    

    /* ************************************************************************ */
    /* Close the set */
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
#endif

INT_type WASN1_SequenceStrings(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type			rval;	 
    ATTR_attribute_t		attr;
    SEQUENCE_STRING_type	sequence;
    STRING_type			string;
    REGISTER INT_type		i, tagfield;

    /* Applicable to at_PERMITTED_CATEGORIES  */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the attribute  first */
    attr = ATTR_FindAttr(object, at_PERMITTED_CATEGORIES);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the idoctet */
    rval = IO_WriteByte(od, MakeByte(tag));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Get the sequence of string */
    sequence = GET_SEQUENCE_STRING(attr);

    /* Then write the length-octet */
    rval = WASN1_Length(od, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* And then write the sequence of strings,  taking these for OCTET-STRINGS @@@ */
    for(i=0; i<=sequence->length-1; i++) {
	/* The idoctet */
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, PRINTABLESTRING_TAG);
	rval = IO_WriteByte(od, MakeByte(tagfield));
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	string = sequence->sequence_value.strings[i];
	
	/* The length */
	rval = WASN1_Length(od, string->length);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The content of the string */
	rval = IO_WriteChars(od, string->sequence_value.chars, string->length);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Close the sequence */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
