
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr9.c,v 1.1 89/09/21 06:24:36 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr9.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr9.c,v $
 * Revision 1.1  89/09/21  06:24:36  mss
 * Initial revision
 * 
 * Revision 1.10  89/09/12  09:11:37  mss
 * Removed #
 * 
 * Revision 1.9  89/05/24  14:48:49  mss
 * Changed checking of attributes for basic log DVLs
 * 
 * Revision 1.8  89/03/09  15:54:12  annm
 * moved the subroutines specific to DVL defaulting from attr8.c to attr9.c
 * 
 * Revision 1.7  89/02/23  20:39:46  annm
 * minor tweek
 * 
 * Revision 1.6  89/01/17  16:57:04  annm
 * changes for DVL defaulting
 * 
 * Revision 1.5  88/09/21  15:48:01  annm
 * added tkerr stuff
 * 
 * Revision 1.4  88/09/06  14:09:08  annm
 * fixed a giant mess - blasted UNIX that lets you access a file simulatneously !!!
 * 
 * Revision 1.2.1.1  88/09/06  14:05:23  annm
 * changed include "" to <>
 * 
 * Revision 1.2  88/08/30  15:11:48  annm
 * no change
 * 
 * Revision 1.1  88/08/16  18:13:42  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr9.c,v $";
#endif

/* Put .c template comment here */



#define	TK_IMPL_ATTR
#define TK_IMPL_CONST
#define TK_GVARS_ATTR9
#define TK_TKI_TKERR

#include <acd.h>
#include <seq.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_IMPL_CONST
#undef TK_GVARS_ATTR9
#undef TK_TKI_TKERR



INT_type ATTR_InheritFromDVL( constituent, attribute, value, mask,
    wanted_mask, destination_mask, destination )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    IN OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type SlaveRval;

    INTERNAL_CONSTITUENT ThisConstituent;
    INTERNAL_CONSTITUENT SomeConstituent;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    ThisConstituent = (struct CONSTITUENT_struct *) constituent;

    /* 
	start backing up the tree looking for default value lists 
    */
    for( SomeConstituent = ThisConstituent->parent;
	 SomeConstituent != (struct CONSTITUENT_struct *) NULL_CONSTITUENT;
	 SomeConstituent = SomeConstituent->parent ){

	SlaveRval = ATTR_AttemptInheritFromDVL( constituent,
			(CONSTITUENT) SomeConstituent,
			attribute, value, mask, wanted_mask, destination_mask,
			destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	if( SlaveRval == ALL_DONE ){
	    rval = ALL_DONE;
	    goto LEAVE;
	}
    }

    /* 
	ASSERT: If get here the loop ended on a NULL_CONSTITUENT
	and we are still not done.
    */

    rval = NOT_DONE;
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_AttemptInheritFromDVL( constituent, ancestor, attribute,
	value, mask, wanted_mask, destination_mask, destination )
    IN CONSTITUENT constituent;
    IN CONSTITUENT ancestor;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    IN OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type SlaveRval;

    ATTR_attribute_t AttrStructPtr;

    CONSTITUENT TheObjectClass;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    SlaveRval = ATTR_InheritFromDVLConstituent( constituent, 
		    ancestor, attribute,
		    value, mask, wanted_mask, destination_mask, destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	rval = ALL_DONE;
	goto LEAVE;
    }

    /* see if the constituent has an object class */
    SlaveRval = HasAttr( ancestor, at_OBJECT_CLASS );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TkError has been set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = NOT_DONE;
	goto LEAVE;
    }
    
    /* ASSERT: HasAttr must have returned HAS_ATTR */
    AttrStructPtr = ATTR_FindAttr( ancestor, at_OBJECT_CLASS );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
 	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_AttemptInheritFromDVL]: constituent has an object class attribute  " ); 
	dummy = TKERR_Format(" but can not find it" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* now get the value */
    TheObjectClass = (CONSTITUENT) GET_CONSTITUENT( AttrStructPtr );

    /* 
	FINALLY: attempt to inherit from the object class and
	any associated style.
    */
    SlaveRval = ATTR_InheritFromDVLConstituent( constituent, 
		    TheObjectClass,
		    attribute, value, mask, wanted_mask,
		    destination_mask, destination );

    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	rval = ALL_DONE;
    }
    else {
	/* SlaveRval must = NOT_DONE */
	rval = NOT_DONE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_InheritFromDVLConstituent( constituent, SomeConstituent, 
    	attribute,
	value, mask, wanted_mask, destination_mask, destination )
    IN CONSTITUENT constituent;
    IN CONSTITUENT SomeConstituent;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    IN OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type SlaveRval;

    at_DEFAULT_VALUE_LISTS_type *TheDVL;

    ATTR_attribute_t AttrStructPtr;

    CONSTITUENT DVLConstituent;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* First see if the constituent has the default value list attribute */
    SlaveRval = HasAttr( SomeConstituent, at_DEFAULT_VALUE_LISTS );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TkError has been set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = NOT_DONE;
	goto LEAVE;
    }
    
    /* ASSERT: HasAttr must have returned HAS_ATTR */
    AttrStructPtr = ATTR_FindAttr( SomeConstituent, at_DEFAULT_VALUE_LISTS );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_InheritFromDVLConstituent]: constituent has a dvl" ); 
	dummy = TKERR_Format(" but can not find it" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* now get the value */
    TheDVL = AttrStructPtr->attribute_value.at_DEFAULT_VALUE_LISTS_field;

    /* attempt to find the right kind of object on the DVL */
    DVLConstituent = ATTR_FindRightObjectType( constituent, 
			attribute, TheDVL );
    if( DVLConstituent == ERROR_CONSTITUENT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( DVLConstituent == NULL_CONSTITUENT ){
	rval = NOT_DONE;
	goto LEAVE;
    }

    /* 
	FINALLY: attempt to inherit from the DVL Constituent and
	any associated style.
    */
    SlaveRval = ATTR_InheritFromConstAndStyle( DVLConstituent,
		    attribute, value, mask, wanted_mask,
		    destination_mask, destination );

    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	rval = ALL_DONE;
    }
    else {
	/* SlaveRval must = NOT_DONE */
	rval = NOT_DONE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}




CONSTITUENT ATTR_FindRightObjectType( constituent, attribute, TheDVL )
    IN CONSTITUENT  constituent;
    IN INT_type	    attribute;
    IN at_DEFAULT_VALUE_LISTS_type *TheDVL;
{

    CONSTITUENT rval;

    VALUE_type ConstituentKind;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    ConstituentKind = ATTR_GetConstituentKind( constituent, attribute );
    if( ConstituentKind == ERROR_VALUE ){
	/* lower level subroutine dealt with the error return */
	rval = NULL_CONSTITUENT;
	goto LEAVE;
    }

    rval = ATTR_FindConstituent( ConstituentKind, TheDVL );
    /* 
	if there is an error then the lower level will set up the 
	error return 
    */
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



VALUE_type ATTR_GetConstituentKind( constituent, attribute )
    IN CONSTITUENT  constituent;
    IN INT_type	    attribute;
{

    VALUE_type rval;

    INTERNAL_CONSTITUENT    TheConstituent;
    INT_type		    dummy;
    CHAR_type *		    dummyStr;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /* make sure that the constituent can be on a dvl */
    /* form of test is !( what it should be ) */
    if( !( CONST_IsPage( TheConstituent ) || 
	    CONST_IsFrame( TheConstituent ) ||
	    CONST_IsBlock( TheConstituent ) ||
	    CONST_IsCompLog( TheConstituent ) || 
	    CONST_IsBasLog( TheConstituent ) 
	 )
    ){
	rval = ERROR_VALUE;
	goto LEAVE;
    }

    if( CONST_IsPage( TheConstituent ) ){
	if( attribute == at_DIMENSIONS || 
	  attribute == at_TRANSPARENCY ||
	  attribute == at_COLOUR || 
	  attribute == at_PAGE_POSITION ||
	  attribute == at_MEDIUM_TYPE 
	    /* N.B. only the attributes that can go on composite pages -
		n.b. we do NOT support basic pages */
	){
	    rval = DVL_PAGE;
	    goto LEAVE;
	}
	else {
	    rval = ERROR_VALUE;
	    goto LEAVE;
	}
    }

    if( CONST_IsFrame( TheConstituent ) ){
	if( attribute == at_POSITION || 
	  attribute == at_DIMENSIONS ||
	  attribute == at_BORDER || 
	  attribute == at_LAYOUT_PATH ||
	  attribute == at_PERMITTED_CATEGORIES || 
	  attribute == at_TRANSPARENCY ||
	  attribute == at_COLOUR ){
	    rval = DVL_FRAME;
	    goto LEAVE;
	}
	else {
	    rval = ERROR_VALUE;
	    goto LEAVE;
	}
    }

    if( CONST_IsCompLog( TheConstituent ) ){
	if( attribute == at_PROTECTION || 
	  attribute == at_LAYOUT_STYLE ||
	  IsLayoutDirective( attribute ) == SUCCESS_INT ){
	    rval = DVL_COMP;
	    goto LEAVE;
	}
	else {
	    rval = ERROR_VALUE;
	    goto LEAVE;
	}
    }

    if( CONST_IsBlock( TheConstituent ) ){
	if( attribute == at_PRESENTATION_STYLE || 
	  attribute == at_CONTENT_ARCH_CLASS ||
	  attribute == at_CONTENT_TYPE ||
	  attribute == at_POSITION ||
	  attribute == at_DIMENSIONS ||
	  attribute == at_BORDER ||
	  attribute == at_TRANSPARENCY ||
	  attribute == at_COLOUR ||
	  IsPresentationAttribute( attribute ) == SUCCESS_INT 
	){
	    rval = DVL_BLOCK;
	    goto LEAVE;
	}
	else {
	    rval = ERROR_VALUE;
	    goto LEAVE;
	}
    }

    if( CONST_IsBasLog( TheConstituent ) ){
	if( attribute == at_PRESENTATION_STYLE ||
	   attribute == at_CONTENT_ARCH_CLASS ||
	   attribute == at_PROTECTION ||
	   attribute == at_LAYOUT_STYLE ||
	   IsLayoutDirective( attribute ) != ERROR_INT ||
	   IsPresentationAttribute( attribute ) != ERROR_INT
	){
	    rval = DVL_BAS;
	    goto LEAVE;
	}
	else {
	    rval = ERROR_VALUE;
	    dummyStr = GetAttrName(attribute);
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    if (dummyStr != (CHAR_type *) 0) {
		dummy = TKERR_Format("[ATTR_GetConstituentKind]: Can not get defaults for ");
		dummy = TKERR_Format(dummyStr );
		dummy = TKERR_Format(" on basic logical object");
	    }
	    else {
		dummy = TKERR_FormatInt(
				 "[ATTR_GetConstituentKind]: Can not get defaults for attribute %d on basic logical object", (INT_type) attribute );
	    }
	    dummy = TKERR_Format(" but can not find it" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    /* it was none of the above - therefore fail */
    rval = ERROR_VALUE;
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type IsLayoutDirective( attribute )
    IN INT_type	attribute;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch( attribute ){

    case at_BLK_ALGNM:
    case at_CONCATENATION:
    case at_FILL_ORDER:
    case at_INDIVISIBILITY:
    case at_LAYOUT_CATEGORY:
    case at_LAYOUT_OBJECT_CLASS:
    case at_NEW_LAYOUT_OBJECT:
    case at_OFFSET:
    case at_SAME_LAYOUT_OBJECT:
    case at_SEPARATION:
    case at_SYNCHRONIZATION:
	rval = SUCCESS_INT;
	break;

    default:
	rval = ERROR_INT;
	break;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type IsPresentationAttribute( attribute )
    IN INT_type	attribute;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = ERROR_INT;

    if( attribute == at_TRANSPARENCY ||
	attribute == at_COLOUR || attribute == at_BORDER ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    if( ATTR_IsCCPresentation( attribute ) == BOOL_true ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }

       if( ATTR_IsRGPresentation( attribute ) == BOOL_true ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    if( ATTR_IsGGPresentation( attribute ) == BOOL_true ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



CONSTITUENT ATTR_FindConstituent( ConstituentKind, TheDVL )
    IN VALUE_type   ConstituentKind;
    IN at_DEFAULT_VALUE_LISTS_type *TheDVL;
{

    CONSTITUENT rval;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Assume failure */
    rval = NULL_CONSTITUENT;

    switch( ConstituentKind ){

    case DVL_PAGE:
	rval = TheDVL->page_attributes; 
	break;

    case DVL_FRAME:
	rval = TheDVL->frame_attributes;
	break;

    case DVL_BLOCK:
	rval = TheDVL->block_attributes;
	break;

    case DVL_COMP:
	rval = TheDVL->composite_logical_attributes;
	break;

    case DVL_BAS:
	rval = TheDVL->basic_logical_attributes;
	break;
    
    default:
	/* failure return */
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

