
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn12.c,v 1.3 89/11/01 11:11:51 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn12.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn12.c,v $
 * Revision 1.3  89/11/01  11:11:51  jr
 * Fiex for bad calls to IO_WriteBytes
 * 
 * Revision 1.2  89/10/31  14:40:22  jr
 * Casting errors.
 * 
 * Revision 1.1  89/09/21  06:42:14  mss
 * Initial revision
 * 
 * Revision 1.15  88/11/17  15:05:53  mz0k
 * Taking care of the parameters for the at_SEPARATION
 * 
 * Revision 1.14  88/11/01  14:46:17  mz0k
 * Added TKError messages
 * 
 * Revision 1.13  88/10/21  11:58:24  jr
 * Change IO_WriteChar -> WriteByte
 * (a must with new buffered
 * output).
 * 
 * Revision 1.12  88/10/18  17:54:42  mz0k
 * added the case of ALL_PARMS for at_OFFSET
 * 
 * Revision 1.11  88/10/18  14:24:05  mz0k
 * added at_OFFSET with parameters
 * 
 * Revision 1.10  88/09/30  12:45:54  mss
 * Removed illegal register qualifier
 * 
 * Revision 1.9  88/09/22  18:01:18  mss
 * 'attrid = at_LOGICAL_SOURCE' --> 'attrid == at_LOGICAL_SOURCE'
 * 
 * Revision 1.8  88/09/14  17:07:14  mz0k
 * removed the "ifdef NOETESTED" from around the "AppendSequence's"
 * 
 * Revision 1.7  88/09/06  13:14:46  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.6  88/08/26  16:34:12  mz0k
 * Fixed name too long: WASN1_CompositeLogicalAttributes
 * 
 * Revision 1.5  88/08/23  11:43:21  mz0k
 * Changed the 'presentation attributes' under the defautl value lists.
 * 
 * Revision 1.4  88/08/15  10:46:15  mz0k
 * Fixed the at_PROTECTION according to new part 5
 * 
 * Revision 1.3  88/08/04  14:25:37  mz0k
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn12.c,v $";
#endif

#define TK_IMPL_WASN1
#define TK_GVARS_WASN12
#define TK_IMPL_WODIF
#define TK_IMPL_ATTR
#define TK_TKI_CONST
#define TK_TKI_IO
#define TK_TKI_PARMS
#define TK_TKI_TKERR

#include <seq.h>
#include <wasn1.h>
#include <tags.h>
#include <parms.h>

#undef TK_IMPL_WASN1
#undef TK_GVARS_WASN12
#undef TK_IMPL_WODIF
#undef TK_IMPL_ATTR
#undef TK_TKI_CONST
#undef TK_TKI_IO
#undef TK_TKI_PARMS
#undef TK_TKI_TKERR


/* 
     Content_Portion_Identifier	    ::= [APPLICATION 0] IMPLICIT PrintableString
         -- only digits and space are used in the present version
         -- of the standard;  other characters are reserved for extensions

    Object-or-Class-Identifier	    ::= [APPLICATION 1] IMPLICIT PrintableString
        -- only digits and space are used in the present version
        -- of the standard;  other characters are reserved for extensions;
        -- a "null" value is represented by empty string.

    Style_Identifier			    ::=[APPLICATION 5] IMPLICIT Printablestring
        -- only digits and space are used in the present version
        -- of the standard;  other characters are reserved for extensions;
        -- a "null" value is represented by an empty string
*/

/* This is to write the constituent identifiers */
INT_type WASN1_Strings(od, object, tag, attrid)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	   tag;    /* The tag of the identifier */
    IN INT_type     attrid;
{
	/* This routine is supposed to write the attributes of
	 various string types. e.g. object identifier, objcet class
	 identifier( of Printable String type); user visible name,
	 user readable comments (of octet string type).
	 
	 In order to write these various string types with this 
	 single routine, it requires that the tag passed in should not
	 be DEFAULT_ID, i.e. the idoctet should have been made
	 already at the calling time.
	 */

    INT_type	rval;
    ATTR_attribute_t    attr;
    STRING_type   string;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

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
    
    /* Get the pointer to the string  */
    string = GET_STRING(attr);

    /* Then write the length-octet */
    rval = WASN1_Length(od, string->length);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    /* And then write the content of the string */
    rval = IO_WriteChars(od, string->sequence_value.chars, string->length);

LEAVE:
#ifdef DEBUG
    if( Traceroutine() ) {
	   Leaving();
    }
#endif

    return rval;
}

/* Write the idoctet and the length-octet of a SEQUENCE,   SEQUENCE-OF,  SET OR SET-OF value */
INT_type WASN1_Header(od, tag, length)
    IN	INT_type    od;
    IN	INT_type    tag;
    IN	INT_type    length;
{
    INT_type	rval;

    /* STRATEGIES:
      
    The tag is always  from the higher level.

    */


#ifdef DEBUG
    if( Traceroutine() ) {
	Entering();
    }
#endif

    
    rval = IO_WriteByte(od, MakeByte(tag));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    rval = WASN1_Length(od, length);
    

LEAVE:
#ifdef DEBUG
    if( Traceroutine() ) {
	   Leaving();
    }
#endif

    return rval;
    }




INT_type WASN1_Constituent_LastId(od, object, idattrid)
    IN INT_type	od;
    IN CONSTITUENT  object;
    IN INT_type idattrid;
{
	/* This routine is basicly  for writing the last integer of an identifier, 
	    a sequence of integers,  which applies to the at_SUBORDINATS and the
	    at_CONTENT_PORTIONS attributes.

	    This last integer is condsidered a string of digits, thus has a
	    UNIVERSAL_CLASS .
	    */
    
	     INT_type		rval;
	     ATTR_attribute_t	attr;
	     STRING_type	string;
	     INT_type		tagfield;
    REGISTER INT_type		i;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif
    
    /* Does the identifier attibute exist? */
    if(HasAttr(object, idattrid) == NO_ATTR) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Find the identifier attribute */
    attr = ATTR_FindAttr(object, idattrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    /* Notice that the attr is a pointer to a STRING_type value 
      consisting of digits and spaces, eg. '1 3 7 23 44 2'
      */
    string= GET_STRING(attr);
    
    /* Get the last non-space sub-string of 'string' */
    for (i=string->length-1; (string->sequence_value.chars[i] != ' ') && (i>=0); i--) {};

    /* In the case of 'subordinates', there should be more than one non-space
	strings, in the case of 'contents', it can't be a space. 
	*/
    if (((i <= 0) && (idattrid == at_OBJECT_IDENTIFIER)) || (i==string->length-1)){
	rval = ERROR_INT;
	goto LEAVE;
    }

    tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, NUMERICSTRING_TAG);

    /* Write the last string. . . the Idoctet first */
    rval = IO_WriteByte(od, MakeByte(tagfield));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    /* The length octet */
    rval = IO_WriteByte(od, (BYTE_type) (string->length-i-1));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    /* The content octets */
    for (++i; (i<= string->length-1) && (rval != ERROR_INT); i++) {
	rval = IO_WriteByte(od, (BYTE_type) string->sequence_value.chars[i]);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_ConstituentId(od, object, attrid, idattrid, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    attrid;	/* Attribute of object with CONATITUENT value */
    IN INT_type	    idattrid;	/* Attribute of the constituent that contains identifier */
    IN INT_type    tag;
{
	/* This routine is supposed to write the identifier of a constituent */

    INT_type			rval;	/* The returned value */
    ATTR_attribute_t		attr;
struct    CONSTITUENT_struct	*constituent;


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

    /* Now, have the constituent identifier written */
    
    constituent = GET_CONSTITUENT(attr);
    /* Does the identifier attibute exist? */
    if(HasAttr((CONSTITUENT) constituent, idattrid) == NO_ATTR) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    rval = WASN1_Strings(od, (CONSTITUENT) constituent, tag, idattrid);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* IN the case of at_LOGICAL_SOURCE, add this constituent to the 
	SequenceOfLogClass. */
    if (attrid == at_LOGICAL_SOURCE) {
	rval = ExpandSequence(SequenceOfLogClass, (INT_type) 1);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	SequenceOfLogClass->sequence_value.constituents[SequenceOfLogClass
	->length -1] = (CONSTITUENT) constituent;
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
INT_type WASN1_Binding_Pair(od, object)
    IN INT_type	    od;
    IN BINDING_PAIR_type object;
{
    INT_type	rval;
    INT_type	tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* The Set Header for the Binding-Pair */
    rval = WASN1_Header(od, DEFAULT_ID, INDEFINITE_LENGTH);
    if(rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write the binding identifier first */
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
    rval = WASN1_Integer(od, tagfield, object.binding_identifier);
    if( rval =ERROR_INT) {
	goto LEAVE;
    }

    /* Then the binding value */
    switch (object.value.binding_pair_tag) {
	    case STRING_EXPR_tag:
		rval = WASN1_StringExpression(od,object);
		break;
	    case NUMERIC_EXPR_tag:
		rval = WASN1_NumericExpression(od, object);
		break;
	    case OBJECT_ID_EXPR_tag:
		rval = WASN1_ObjectIdExpression(od, object);
		break;

	    case INTEGER_tag:
		rval = WASN1_Integer(od, DEFAULT_ID, /* ? */ object.value.binding_pair_value.integer);
		break;
	    case STRING_tag:
		rval = WASN1_String(od, object); /* ? */
		break;
	    case CONSTITUENT_tag:
		rval = WASN1_ConstituentId(od, object);
		break;

	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_Binding_pair]: Invalid object.value.binding_pair_tag value: 0d%d", (INT_type) object.value.binding_pair_tag);
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
    }
    
    /* Close the set of Binding-Pair */
    rval = WASN1_EOC(od);
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
#endif

INT_type WASN1_Separation(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    INT_type rval;
    ATTR_attribute_t	attr;  
    at_SEPARATION_type	*object_body;
    INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, at_SEPARATION);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Get the separation value */
    object_body = GET_at_SEPARATION(attr);

    /* Then write the set header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write out the parts */
    if (((attr->parm_mask & LEADING_EDGE_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Integer(od, tagfield, object_body->leading_edge);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    if (((attr->parm_mask & TRAILING_EDGE_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Integer(od, tagfield, object_body->trailing_edge);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    if (((attr->parm_mask & CENTRE_SEPARATION_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_Integer(od, tagfield, object_body->centre_separation);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Close the set */
    rval = WASN1_EOC(od);
	
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


INT_type WASN1_Offset(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    /* Notice the special dealing with parameters */

    INT_type rval;
    ATTR_attribute_t	attr;
    at_OFFSET_type	*object_body;
    INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, at_OFFSET);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Get the offset value */
    object_body = GET_at_OFFSET(attr);

    /* Then write the set header */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write out the parts */
    if (((attr->parm_mask & LEADING_OFFSET_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 3);
	rval = WASN1_Integer(od, tagfield, object_body->leading_offset);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if (((attr->parm_mask & TRAILING_OFFSET_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_Integer(od, tagfield, object_body->trailing_offset);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if (((attr->parm_mask & LEFT_HAND_OFFSET_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Integer(od, tagfield, object_body->left_hand_offset);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if (((attr->parm_mask & RIGHT_HAND_OFFSET_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Integer(od, tagfield, object_body->right_hand_offset);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Close the set */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_LayoutObjectType(od, object, attrid)
    IN INT_type		od;
    IN CONSTITUENT	object;
    IN INT_type		attrid;
{
    INT_type		rval;
    LAYOUT_OBJECT_type	*newobject;
    INT_type	tagfield[4];
    ATTR_attribute_t	attr;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, attrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Get the pointer to the struct of  layout_object_type */
    newobject = GET_LAYOUT_OBJECT(attr);

    /* Make the tagfields for each of the four cases */
    if (attrid == at_INDIVISIBILITY) {
	tagfield[0] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	tagfield[1] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	tagfield[2] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	tagfield[3] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 15);
    }
    else { /* at_NEW_LAYOUT_OBJECT */
	tagfield[0] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 7);
	tagfield[1] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 8);
	tagfield[2] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 9);
	tagfield[3] = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 16);
    }

    /* See which field is present in the union */
    switch (newobject->layout_object_tag) {

	    case LAYOUT_OBJ_DESC_tag:		
		if (HasAttr(newobject->layout_object_value.constituent, at_OBJECT_CLASS_IDENTIFIER)==NO_ATTR) {
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		
		rval = WASN1_Strings(od, newobject->layout_object_value.constituent, tagfield[0], at_OBJECT_CLASS_IDENTIFIER);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;

	    case LAYOUT_CATEGORY_tag:
		/* @@@ Take the layout category as a string for now @@@ */
		/* Write the idoctet */
		rval = IO_WriteByte(od, MakeByte(tagfield[1]));
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}

		/* Then write the length-octet */
		rval = WASN1_Length(od, (newobject->layout_object_value.layout_category)->length);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		/* And then write the content of the string */
		rval = IO_WriteChars(od,
	(newobject->layout_object_value.layout_category)->sequence_value.chars,
	(newobject->layout_object_value.layout_category)->length);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;

	    case OBJECT_TYPE_PAGE_tag:
		rval = WASN1_Integer(od, tagfield[2], (INT_type) 2);
		break;

	    case NULL_tag:
		rval = IO_WriteByte(od, MakeByte(tagfield[3]));
		if (rval != ERROR_INT) {
		    rval = IO_WriteByte(od, (BYTE_type) 0);
		}
		break;

	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_LayoutObjectType]: Invalid newobject->layout_object_tag value: 0d%d", (INT_type) newobject->layout_object_tag);
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

#ifdef NOTTESTED
INT_type WASN1_SameLayoutObjectType(od, object, tag)
    IN INT_type		od;
    IN CONSTITUENT	object;
    IN INT_type		tag;
{
    INT_type			rval;
    REGISTER INT_type		tagfield;
    at_SAME_LAYOUT_OBJECT_type	*sameobject;
    ATTR_attribute_t		attr;
    INT_type		dummy;

    /* The Toolkit representation of the at_SAME_LAYOUT_OBJECT does not
      agree to the asn1 definition. So we skip over it. 
      */

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Get the attribute first */
    attr = ATTR_FindAttr(object, at_NEW_LAYOUT_OBJECT);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Write the set header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Get the pointer to the struct of same layout object */
    sameobject = GET_at_SAME_LAYOUT_OBJECT(attr);

    /* See which field is present in the union */
    switch (sameobject->layout_object_tag) {

	    case LAYOUT_OBJECT_DESC_tag:		
		if (HasAttr(newobject->layout_object_value.constituent, at_OBJECT_CLASS_IDENTIFEIR)==NO_ATTR) {
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 7);
		rval = WASN1_Strings(od, newobject->layout_object_value.constituent, tagfield, at_OBJECT_CLASS_IDENTIFIER);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;

	    case LAYOUT_CATEGORY_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 8);
		/* Take the layout category as a string for now */
		/* Write the idoctet */
		rval = IO_WriteByte(od, MakeByte(tagfield));
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}

		/* Then write the length-octet */
		rval = WASN1_Length(od, (newobject->layout_object_value.layout_category)->length);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		/* And then write the content of the string */
		rval = IO_WriteChars(od, (newobject->layout_object_value.layout_category)->sequence_value.chars, (newobject->layout_object_value.layout_category)->length);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;

	    case OBJECT_TYPE_PAGE_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 9);
		rval = WASN1_Integer(od, tagfield, (INT_type) 2);
		break;

	    case NULL_tag:
		tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, NULL_TAG);
		rval = IO_WriteByte(od, MakeByte(tagfield));
		if (rval != ERROR_INT) {
		    rval = IO_WriteByte(od, (BYTE_type) 0);
		}
		break;

	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_SameLayoutObjectType]: Invalid sameobject->layout_object_tag value: 0d%d", (INT_type) sameobject->layout_object_tag);
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
#endif

INT_type WASN1_CompLogicalAttributes(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Write the set header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each attribute */
    if(HasAttr(object, at_PROTECTION)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 8);
	rval = WASN1_ValueType(od, object, tagfield, at_PROTECTION);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if(HasAttr(object, at_LAYOUT_STYLE)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 10);
	rval = WASN1_ConstituentId(od, object, at_LAYOUT_STYLE, at_LAYOUT_STYLE_IDENTIFIER, tagfield);
	if (rval ==ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Close the set */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


INT_type WASN1_BasicLogicalAttributes(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* Write the set header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each attribute */

    /* The at_PROTECTION first */
    if(HasAttr(object, at_PROTECTION)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 8);
	rval = WASN1_ValueType(od, object, tagfield, at_PROTECTION);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Then the layout style identifier */
    if(HasAttr(object, at_LAYOUT_STYLE)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 10);
	rval = WASN1_ConstituentId(od, object, at_LAYOUT_STYLE, at_LAYOUT_STYLE_IDENTIFIER, tagfield);
	if (rval ==ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* The presentation style identifier */
    if(HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 9);
	rval = WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE, at_PRESENTATION_STYLE_ID, tagfield);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* The content architecture class of the presentation attributes */

    if(HasAttr(object, at_CONTENT_ARCH_CLASS)==HAS_ATTR) {
	/* The set header first */
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* The sequence of integers for the content arch class */
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, OBJECT_IDENTIFIER_TAG);
	rval = WASN1_Asn1ObjectId(od, object, tagfield, at_CONTENT_ARCH_CLASS); 
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* And the EOC */   
	rval = WASN1_EOC(od);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Close the set */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

