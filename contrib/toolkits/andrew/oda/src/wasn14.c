
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn14.c,v 1.1 89/09/21 06:43:00 mss Exp Locker: jr $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn14.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn14.c,v $
 * Revision 1.1  89/09/21  06:43:00  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/06  11:21:06  annm
 * fixing problems with names
 * 
 * Revision 1.11  88/11/01  14:46:54  mz0k
 * Added TKError messages
 * 
 * Revision 1.10  88/10/18  17:06:57  mz0k
 * added ALL_PARMS case for the rg_CLIPPING
 * 
 * Revision 1.9  88/10/14  17:46:40  mz0k
 * fixed raster coding attributes
 * 
 * Revision 1.8  88/10/13  22:01:31  mz0k
 * added raster attributes and raster coding attributes
 * 
 * Revision 1.7  88/09/25  18:07:03  mz0k
 * typo fixed
 * 
 * Revision 1.6  88/09/23  16:49:36  mz0k
 * permitted categories tag
 * 
 * Revision 1.5  88/09/06  13:15:06  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/04  17:46:22  jr
 * Oops...forward defns were wrong thing
 * to change.
 * 
 * Revision 1.3  88/08/04  17:42:39  jr
 * Fix up forward defn's.
 * 
 * Revision 1.2  88/08/04  14:31:33  mz0k
 * ci wasn14.c wodif3.c
 * 
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn14.c,v $";
#endif

#define TK_IMPL_WASN1
#define TK_GVARS_WASN14
#define TK_IMPL_ATTR
#define TK_TKI_CONST
#define TK_TKI_IO
#define TK_TKI_PARMS
#define TK_TKI_TKERR

#include <wasn1.h>
#include <tags.h>
#include <parms.h>

#undef TK_IMPL_WASN1
#undef TK_GVARS_WASN14
#undef TK_IMPL_ATTR
#undef TK_TKI_CONST
#undef TK_TKI_IO
#undef TK_TKI_PARMS
#undef TK_TKI_TKERR

/* Forward definitions */

INT_type WASN1_PageAttributes(od, object, tag, object_type)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
    IN VALUE_type   object_type;
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
    
    if(HasAttr(object, at_DIMENSIONS)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_DefaultDimensions(od, object, tagfield);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_TRANSPARENCY)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, at_TRANSPARENCY);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_PATH)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 4);
	rval = WASN1_ValueType(od, object, tagfield, at_LAYOUT_PATH);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_PAGE_POSITION)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 5);
	rval = WASN1_IntPair(od, object, tagfield, at_PAGE_POSITION);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_MEDIUM_TYPE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 6);
	rval = WASN1_MediumType(od, object, tagfield);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_COLOUR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 11);
	rval = WASN1_ValueType(od, object, tagfield, at_COLOUR);
    }
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
#ifdef BASICPAGE
    /* Following are the attributes defaultable only for the basic pages --- which we don't support */
    if (object_type == at_OBJECT_TYPE_page) {

	/* There is a mistake in the Table 1 of 5.3.5.5 in Part 2 of the
	 oda manual.  Obviously, the 'presentation attributes' includes
	 the 'content-architecture' and the 'content-type'.  So for the
	 'presentation attributes', we just consider these two.
	     */
	
	if((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_ARCH_CLASS)==HAS_ATTR)) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }

	    tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, OBJECT_IDENTIFIER_TAG);
	    rval = WASN1_Asn1ObjectId(od, object, tagfield, at_CONTENT_ARCH_CLASS); 
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	
	    rval = WASN1_EOC(od);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}

	if(HasAttr(object, at_CONTENT_TYPE)==HAS_ATTR) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 3);
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }

	    tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 2);
	    rval = WASN1_ValueType(od, object, tagfield, at_CONTENT_TYPE);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }

	    rval = WASN1_EOC(od);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}

	/* The presentation style */
	if (HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR) {
	    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 9);
	    rval = WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE,
				     at_PRESENTATION_STYLE_ID, tagfield);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
    }
#endif

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


INT_type WASN1_MediumType(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type		rval, val;
    REGISTER INT_type	tagfield;
    ATTR_attribute_t	attr;
    at_MEDIUM_TYPE_type	*medium_type;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif


    attr = ATTR_FindAttr(object, at_MEDIUM_TYPE);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    medium_type = GET_at_MEDIUM_TYPE(attr);

    /* Write the sequence header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each attribute */
    
    /* First the normal-page-size --- a sequence */
    tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
    rval = WASN1_Integer(od, tagfield, medium_type->nominal_page_size.first);
    if (rval != ERROR_INT) {
	rval = WASN1_Integer(od, tagfield, medium_type->nominal_page_size.second);
    }
    if (rval != ERROR_INT) {
	goto LEAVE;
    }

    rval = WASN1_EOC(od);

    /* Then the side-of-sheet */
    if (rval != ERROR_INT) {
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
	switch (medium_type->side_of_sheet) {
		case at_MEDIUM_TYPE_unspecified:
		    val = 0;
		    break;
		case at_MEDIUM_TYPE_recto:
		    val = 1;
		    break;
		case at_MEDIUM_TYPE_verso:
		    val = 2;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[WASN1_MediumType]: Invalid medium_type->side_of_sheet: 0d%d", (INT_type) medium_type->side_of_sheet);
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
	rval = WASN1_Integer(od, tagfield, val);
    }

    /* Close the sequence of medium_type */
    if (rval != ERROR_INT) {
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

INT_type WASN1_DefaultDimensions(od, object, tag)
    IN INT_type		    od;
    IN CONSTITUENT	    object;
    IN INT_type		    tag;
{
    INT_type		rval;
    REGISTER INT_type	tagfield;
    ATTR_attribute_t	attr;
    at_DIMENSIONS_type	*dimensions;

    INT_type dummy;

    /* There are duplicated definitions of the "dimensions" in part5-2. 
      For the one in the default value lists,  we assume that they are both
     FIXED_DIMENSION's.
   */

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif


    attr = ATTR_FindAttr(object, at_DIMENSIONS);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    dimensions = GET_at_DIMENSIONS(attr);

    /* Write the sequence header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each attribute */

    if ((dimensions->horizontal_dimension.dimensions_tag != FIXED_DIMENSIONS_tag) || 
	  (dimensions->vertical_dimension.dimensions_tag != FIXED_DIMENSIONS_tag)) {
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[WASN1_DefaultDimensions]: can only deal with fixed dimensions" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
    rval = WASN1_Integer(od, tagfield, dimensions->horizontal_dimension.
			  dimensions_body.fixed_dimension);
    if (rval ==ERROR_INT) {
	goto LEAVE;
    }
    
    rval = WASN1_Integer(od, tagfield, dimensions->vertical_dimension.
			  dimensions_body.fixed_dimension);

    /* Close the sequence  */
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


INT_type WASN1_Dimensions(od, object, tag)
    IN INT_type		    od;
    IN CONSTITUENT	    object;
    IN INT_type		    tag;
{
    INT_type		rval;
    REGISTER INT_type	tagfield, i;
    ATTR_attribute_t	attr;
    at_DIMENSIONS_type	*dimensions; 
    DIMENSION_type	horiz_or_vertic;
    INT_type		dummy;

    /* There are duplicated definitions of the "dimensions" in part5-2. 
      This is the one in the layout structures.
   */

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif


    attr = ATTR_FindAttr(object, at_DIMENSIONS);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    dimensions = GET_at_DIMENSIONS(attr);


    /* Write the sequence header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each of the two Dimensions */

    horiz_or_vertic = dimensions->horizontal_dimension;
    for (i=0; i<=1; i++) {
	
	switch (horiz_or_vertic.dimensions_tag) {
	    case FIXED_DIMENSIONS_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
		rval = WASN1_Integer(od, tagfield, horiz_or_vertic.
				     dimensions_body.fixed_dimension);
		break;
	    case VARIABLE_PAGE_HEIGHT_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
		rval = WASN1_Integer(od, tagfield, horiz_or_vertic.
				     dimensions_body.fixed_dimension);
		break;
	    case RULE_A_tag:
	    case RULE_B_tag:
		if (horiz_or_vertic.dimensions_tag == RULE_A_tag) {
		    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
		}
		else {
		    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
		}
		rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
		rval = WASN1_Integer(od, tagfield, horiz_or_vertic.
				     dimensions_body.rule.minimum_dimension);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
		rval = WASN1_Integer(od, tagfield, horiz_or_vertic.
				     dimensions_body.rule.maximum_dimension);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;
	    default:
		rval =ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_Dimemsions]: Invalid horiz_or_vertic.dimensions_tag: 0d%d", (INT_type) horiz_or_vertic.dimensions_tag);
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
	}

	horiz_or_vertic = dimensions->vertical_dimension;
    }	



    /* Close the sequence  */
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


INT_type WASN1_FrameAttributes(od, object, tag)
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
    
    if(HasAttr(object, at_POSITION)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WASN1_DefaultPosition(od, object, tagfield);
    } 
    if(HasAttr(object, at_DIMENSIONS)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_DefaultDimensions(od, object, tagfield);
    }  

    if((rval != ERROR_INT) && (HasAttr(object, at_TRANSPARENCY)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, at_TRANSPARENCY);
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_LAYOUT_PATH)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 4);
	rval = WASN1_ValueType(od, object, tagfield, at_LAYOUT_PATH);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_PERMITTED_CATEGORIES)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 7);
	rval = WASN1_SequenceStrings(od, object, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_COLOUR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 11);
	rval = WASN1_ValueType(od, object, tagfield, at_COLOUR);
    }
#ifdef NOTTESTED
    if((rval != ERROR_INT) && (HasAttr(object, at_BORDER)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 12);
	rval = WASN1_Border(od, object, tagfield);
    }
#endif
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

INT_type WASN1_DefaultPosition(od, object, tag)
    IN INT_type		    od;
    IN CONSTITUENT	    object;
    IN INT_type		    tag;
{
    INT_type		rval;
    REGISTER INT_type	tagfield;
    ATTR_attribute_t	attr;
    at_POSITION_type	*position;

    INT_type dummy;

    /* Note: 
        There are duplicated definitions of the "position" in part5-2. 
      For the one in the default value lists,  we assume that they are both
     FIXED_POSITION's.
 */


#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif


    attr = ATTR_FindAttr(object, at_POSITION);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    position = GET_at_POSITION(attr);

    /* Write the sequence header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then each attribute */

    if (position->position_tag == FIXED_POSITION_tag) {

	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Integer(od, tagfield,
			     position->position_body.fixed_position.horizontal_position);
	if (rval ==ERROR_INT) {
	    goto LEAVE;
	}
    
	rval = WASN1_Integer(od, tagfield,
			     position->position_body.fixed_position.vertical_position);
	if (rval ==ERROR_INT) {
	    goto LEAVE;
	}
    }
    else {
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[WASN1_DefaultPosition]: can only deal with rixed position" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Close the sequence  */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


INT_type WASN1_BlockAttributes(od, object, tag)
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
    
    if(HasAttr(object, at_POSITION)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WASN1_DefaultPosition(od, object, tagfield);
    } 
    if(HasAttr(object, at_DIMENSIONS)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_DefaultDimensions(od, object, tagfield);
    }  

    if((rval != ERROR_INT) && (HasAttr(object, at_TRANSPARENCY)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, at_TRANSPARENCY);
    }

    /* There is a mistake in the Table 1 of 5.3.5.5 in Part 2 of the
	 oda manual.  Obviously, the 'presentation attributes' includes
	 the 'content-architecture' and the 'content-type'.  So for the
	 'presentation attributes', we just consider these two.
  */
	
    if((rval != ERROR_INT) && (HasAttr(object, at_CONTENT_ARCH_CLASS)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, OBJECT_IDENTIFIER_TAG);
	rval = WASN1_Asn1ObjectId(od, object, tagfield, at_CONTENT_ARCH_CLASS); 
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	
	rval = WASN1_EOC(od);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if(HasAttr(object, at_CONTENT_TYPE)==HAS_ATTR) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 3);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	tagfield = MakeTag(APPLICATION_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, at_CONTENT_TYPE);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	
	rval = WASN1_EOC(od);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    if((rval != ERROR_INT) && (HasAttr(object, at_PRESENTATION_STYLE)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 9);
	rval = WASN1_ConstituentId(od, object, at_PRESENTATION_STYLE,
				 at_PRESENTATION_STYLE_ID, tagfield);
    }
    if((rval != ERROR_INT) && (HasAttr(object, at_COLOUR)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 11);
	rval = WASN1_ValueType(od, object, tagfield, at_COLOUR);
    }
#ifdef NOTTESTED
    if((rval != ERROR_INT) && (HasAttr(object, at_BORDER)==HAS_ATTR)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 12);
	rval = WASN1_Border(od, object, tagfield);
    }
#endif
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



INT_type WASN1_RgClipping(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;
    ATTR_attribute_t	attr;
    rg_CLIPPING_type	*clipping;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif



    /* get the attribute */
    attr = ATTR_FindAttr(object, rg_CLIPPING);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    clipping = GET_rg_CLIPPING(attr);

    /* Write the set header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then the first int_pair */
    if (((attr->parm_mask & COORD1_parm) != 0) || (attr->parm_mask == ALL_PARMS)){
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
	rval = WASN1_Integer(od, tagfield, clipping->coord1.first);
	if (rval != ERROR_INT) {
	    rval = WASN1_Integer(od, tagfield, clipping->coord1.second);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
	/* the EOC */
	rval = WASN1_EOC(od);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* the second int_pair */
    if (((attr->parm_mask & COORD2_parm) != 0) || (attr->parm_mask == ALL_PARMS)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
	rval = WASN1_Integer(od, tagfield, clipping->coord2.first);
	if (rval != ERROR_INT) {
	    rval = WASN1_Integer(od, tagfield, clipping->coord2.second);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
	/* the EOC */
	rval = WASN1_EOC(od);
    }

    /* The EOC for the sequence */
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


INT_type WASN1_RgPelSpacing(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;
    ATTR_attribute_t	attr;
    rg_PEL_SPACING_type	*pel_spacing;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif


    /* write the header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* get the attribute */
    attr = ATTR_FindAttr(object, rg_PEL_SPACING);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    pel_spacing = GET_rg_PEL_SPACING(attr);

    if (pel_spacing->null) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = IO_WriteByte(od, MakeByte(tagfield));
	/* the length should be 0x00 */
	if (rval != ERROR_INT) {
	    rval = IO_WriteByte(od, (BYTE_type) 0);
	}
    }
    else {
	/* the sequence header first */
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);

	/* the two integers */
	if (rval != ERROR_INT) {
	    tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
	    rval = WASN1_Integer(od, tagfield, pel_spacing->length);
	    if (rval != ERROR_INT) {
		rval = WASN1_Integer(od,tagfield, pel_spacing->pel_spaces);
	    }
	}

	/* The EOC */
	if (rval != ERROR_INT) {
	    rval = WASN1_EOC(od);
	}
    }

    /* The EOC for the CHOICE */
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


INT_type WASN1_RasterCodingAttributes(od, object)
    IN INT_type	    od;
    IN CONSTITUENT  object;
{   /*
      We avoid writing an empty set for this.
	  */
    
    INT_type rval;
    REGISTER INT_type	tagfield;
    BOOL_type		header_written;
    ATTR_attribute_t	attr;
    INT_type		intvalue;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* make the tagfield for the set header for use in case of a non-empty set */
    header_written = BOOL_false;
    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type)2);


    if(HasAttr(object, rg_NUMBER_OF_PELS_PER_LINE)==HAS_ATTR) {
	if (!header_written) {
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    header_written = BOOL_true;
	}

	/* get the attribute */
	attr = ATTR_FindAttr(object, rg_NUMBER_OF_PELS_PER_LINE);
	if (attr == ERROR_ATTRIBUTE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	intvalue = GET_INT(attr);

	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	rval = WASN1_Integer(od, tagfield, intvalue);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    if(HasAttr(object, rg_NUMBER_OF_LINES)==HAS_ATTR) {
	if (!header_written) {
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    header_written = BOOL_true;
	}

	/* get the attribute */
	attr = ATTR_FindAttr(object, rg_NUMBER_OF_LINES);
	if (attr == ERROR_ATTRIBUTE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	intvalue = GET_INT(attr);

	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	rval = WASN1_Integer(od, tagfield, intvalue);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    if(HasAttr(object, rg_COMPRESSION)==HAS_ATTR) {
	if (!header_written) {
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    header_written = BOOL_true;
	}

	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_ValueType(od, object, tagfield, rg_COMPRESSION);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    if(HasAttr(object, rg_NUMBER_DISCARDED_PELS)==HAS_ATTR) {
	if (!header_written) {
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    header_written = BOOL_true;
	}

	/* get the attribute */
	attr = ATTR_FindAttr(object, rg_NUMBER_DISCARDED_PELS);
	if (attr == ERROR_ATTRIBUTE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	intvalue = GET_INT(attr);

	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 3);
	rval = WASN1_Integer(od, tagfield, intvalue);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }


    if (header_written) {
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


INT_type WASN1_RgImageDimensions(od, object, tag)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
{
    
    INT_type		rval, val;
    REGISTER INT_type	tagfield;
    ATTR_attribute_t	attr;
    rg_IMAGE_DIMENSIONS_type	*dimensions;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif


    /* write the header first */
    rval = WASN1_Header(od, tag, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }


    /* get the attribute */
    attr = ATTR_FindAttr(object, rg_IMAGE_DIMENSIONS);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    dimensions = GET_rg_IMAGE_DIMENSIONS(attr);

    switch (dimensions->image_dimensions_tag) {
	    case WIDTH_CONTROLLED_tag:
		/* The header of the sequence */
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
		rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		/* the int-pair */
		tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
		rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.width_controlled.first);
		if (rval != ERROR_INT) {
		    rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.width_controlled.second);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		}
		/* the EOC */
		rval = WASN1_EOC(od);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;
	    case HEIGHT_CONTROLLED_tag:
		/* The header of the sequence */
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
		rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		/* the int-pair */
		tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
		rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.height_controlled.first);
		if (rval != ERROR_INT) {
		    rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.height_controlled.second);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		}
		/* the EOC */
		rval = WASN1_EOC(od);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;
	    case AREA_CONTROLLED_tag:
		/* The header of the sequence */
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
		rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		/* The integers */
		tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
		rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.area_controlled.minimum_width);
		if (rval != ERROR_INT) {
		    rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.area_controlled.preferred_width);
		}
		if (rval != ERROR_INT) {
		    rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.area_controlled.minimum_height);
		}
		if (rval != ERROR_INT) {
		    rval = WASN1_Integer(od, tagfield, dimensions->image_dimensions_value.area_controlled.preferred_height);
		}

		/* the value */
		switch (dimensions->image_dimensions_value.area_controlled.aspect_ratio_flag) {
		    case rg_IMAGE_DIMENSIONS_fixed:
			val = 0;
			break;   
		    case rg_IMAGE_DIMENSIONS_variable:
			val = 1;
			break;   
		    default:
			rval = ERROR_INT;
			TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[WASN1_RgImageDimemsions]: Invalid dimensions->image_dimensions_value.area_controlled.aspect_ratio_flag: 0d%d", (INT_type) dimensions->image_dimensions_value.area_controlled.aspect_ratio_flag);
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		}
		rval = WASN1_Integer(od, tagfield, val);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		
		/* don't forget about the EOC of the area-controlled! */
		rval = WASN1_EOC(od);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;
	    case AUTOMATIC_tag:
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 3);
		rval = IO_WriteByte(od, MakeByte(tagfield));
		/* the length should be 0x00 */
		if (rval != ERROR_INT) {
		    rval = IO_WriteByte(od, (BYTE_type) 0);
		}
		break;
	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_RgImageDimemsions]: Invalid dimensions->image_dimensions_tag: 0d%d", (INT_type) dimensions->image_dimensions_tag);
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
    }
    
    /* The EOC of the CHOICE */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


