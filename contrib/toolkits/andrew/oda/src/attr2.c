
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr2.c,v 1.2 89/10/31 13:20:15 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr2.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr2.c,v $
 * Revision 1.2  89/10/31  13:20:15  jr
 * Casting problems.
 * 
 * Revision 1.1  89/09/21  06:19:34  mss
 * Initial revision
 * 
 * Revision 1.43  89/09/12  09:06:17  mss
 * Removed #
 * 
 * Revision 1.42  89/01/17  16:56:00  annm
 * changes for DVL defaulting
 * 
 * Revision 1.41  88/11/03  15:08:21  annm
 * fixed butg in SterilizeMask - at_DIMENSIONS sterile mask was not being set correctly
 * 
 * Revision 1.40  88/09/30  15:40:15  annm
 * fixing VMS bugs
 * 
 * Revision 1.39  88/09/23  15:11:52  annm
 * fixed blasted bug in AddToConstituent - free the APPROPRIATE outboard storage after the FindAndRemove !
 * 
 * Revision 1.38  88/09/21  15:46:01  annm
 * added tkerr stuff
 * 
 * Revision 1.37  88/09/06  13:08:50  annm
 * no change
 * 
 * Revision 1.36  88/09/06  12:01:10  annm
 * no change
 * 
 * Revision 1.35  88/09/01  14:17:39  annm
 * changed blasted " to <>
 * 
 * Revision 1.34  88/08/25  16:11:29  annm
 * added tree unthreader for delete constituent
 * 
 * Revision 1.33  88/08/23  14:01:30  annm
 * no change
 * 
 * Revision 1.32  88/08/22  19:42:36  annm
 * cc_KERNING_OFFSET and cc_INITIAL_OFFSET are now treated as NOT having related parameters (code commented out in switch statements).
 * 
 * Revision 1.31  88/08/16  18:12:15  annm
 * added tree threading for setattr of at_SUBORDINATES
 * 
 * Revision 1.30  88/08/10  15:42:11  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults for the currently implemented aunion.h types
 * 
 * Revision 1.29  88/08/02  17:26:07  annm
 * added comment in the attribute classfier
 * 
 * Revision 1.28  88/08/01  18:44:35  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.27  88/07/26  16:50:06  annm
 * fixed FindAndRemove to do the remove when the attribute is found and is not the first one on the list
 * 
 * Revision 1.26  88/07/18  18:09:44  annm
 * changes associated with having GetAttr
 * use the canonical procedures for operating on types
 * 
 * Revision 1.25  88/07/15  18:20:26  annm
 * changes for using canonical procedures in SetAttr
 * 
 * Revision 1.24  88/07/14  18:07:41  annm
 * changes for use of aunion.h canonical procedures in SetAttr
 * 
 * Revision 1.23  88/07/14  17:48:14  annm
 * changes to use canonical procedures for operating on aunion.h types for the SetAttr stuff
 * 
 * Revision 1.22  88/06/27  16:19:51  annm
 * ATTR_FindAttr now sets TKError to NO_ATTR when the attribute requested is not on the constituent
 * 
 * Revision 1.21  88/06/17  15:03:03  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.20  88/06/09  15:41:37  annm
 * fixed interface for int/value attributes in Get ... Attr code
 * 
 * Revision 1.19  88/06/08  16:53:07  annm
 * dealt with line too long in copyright notice and changed
 * #end's to #endif's
 * 
 * Revision 1.18  88/06/08  14:34:09  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr2.c,v $";
#endif

/* Put .c template comment here */



#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR2
#define TK_IMPL_CONST
#define TK_TKI_STR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <acd.h>
#include <attr0.h>
#include <str.h>
#include <mm.h>
#include <parms.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR2
#undef TK_IMPL_CONST
#undef TK_TKI_STR
#undef TK_TKI_MM
#undef TK_TKI_TKERR



INT_type ATTR_AddToConstituent( constituent, AttrStructPtr )
    IN CONSTITUENT	constituent;
    IN ATTR_attribute_t	AttrStructPtr;
{

    /*
	Link the attribute structure pointed to by 
	AttrStructPtr into the constituent hash buckets
	and set the which_attributes field in the constituent
	to show that this attribute is present.

	DANGER: Right now we will replace an existing
	attribute value without	complaint !!!

    */

    /* 
	Strategy:
	    1) If the attribute is there now:
		a) remove the attribute from the
		    constituent's hash buckets
		b) TO DO: free the storage.
	    2) set the constituent's which_attributes
		field.
	    3) link the attribute structure into the
		constituent's hash buckets.

    */

    INT_type	    rval;

    INT_type	    BucketNumber;
	/* number of the hash bucket for the attribute */
    ATTR_attribute_t	CurrentAttr;
	/* used when this attribute is already present */
    INTERNAL_CONSTITUENT TheConstituent;

    INT_type	    dummy;	/* ignored return value */
    POINTER_type    temp_ptr;
    INT_type	    TypeIndex;  /* into the type branch table */

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif


    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /*
	First: if the attribute is there now remove the
	    attribute from the constituent's hash buckets.
    */
    if( CONST_UnsafeHasAttr( TheConstituent, 
	AttrStructPtr->attribute_id) 
    ){

	/*
	    Prevent replace of at_OBJECT_TYPE
	*/
	if( AttrStructPtr->attribute_id == at_OBJECT_TYPE ){
	    rval = ERROR_INT;
	    TKError = NOREPLACE_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AddToConstituent]: Can not replace at_OBJECT_TYPE during SetAttr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	CurrentAttr = ATTR_FindAndRemoveAttr( constituent,
	    AttrStructPtr->attribute_id);
	if( CurrentAttr != ERROR_ATTRIBUTE ){
	    /* delete the storage */
	    TypeIndex = ATTR_IndexIntoTypes[AttrStructPtr->attribute_id];
	    temp_ptr = (*ATTR_AttrTypeTbl[TypeIndex].get)(CurrentAttr);
	    dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)(temp_ptr);
	    dummy = MM_Free((POINTER_type) CurrentAttr);
	}
	else {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
    }

    /* Second: set the constituent's which_attribute field */
    CONST_UnsafeSetAttr( TheConstituent, 
	AttrStructPtr->attribute_id);

    /* 
	Third: link the attribute structure into the constituent's
	hash buckets for attributes. Added at the front of the list.
    */
    BucketNumber = ATTR_Hash( AttrStructPtr->attribute_id );
    AttrStructPtr->next = TheConstituent->attr_hash_buckets[BucketNumber];
	/* new node points at first on list */
    TheConstituent->attr_hash_buckets[BucketNumber] = AttrStructPtr;
	/* new node now first on list */

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
    
}




INT_type ATTR_Hash( attrid )
    IN INT_type	attrid;
{

    /* ASSERT: check of attrid isn't necessary */

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval =  attrid % NBUCKETS;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



ATTR_attribute_t ATTR_FindAndRemoveAttr( constituent, attrid )
    IN CONSTITUENT  constituent;
    IN INT_type	    attrid;
{

    /*
	Find the structure for attrid in the constituent's 
	hash buckets and return pointer to it.
	Return ERROR_ATTRIBUTE if not there.
    */

    ATTR_attribute_t	rval;
    INT_type		BucketNumber;
    ATTR_attribute_t	SomeAttr;
    ATTR_attribute_t	PreviousAttr;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    BucketNumber = ATTR_Hash( attrid );

    /*
	check for deleting the first element on the 
	singly linked list.
    */

    if( TheConstituent->attr_hash_buckets[BucketNumber] == NULL_ATTRIBUTE ){
	rval = ERROR_ATTRIBUTE;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FindAndRemoveAttr]: which attributes say has attr " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attrid].attrid_name );
	dummy = TKERR_Format( " but hash bucket empty" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( TheConstituent->attr_hash_buckets[BucketNumber]->attribute_id
	== attrid ){
	/* first node on list */
	SomeAttr = TheConstituent->attr_hash_buckets[BucketNumber];	
	TheConstituent->attr_hash_buckets[BucketNumber] =
	    SomeAttr->next;
	rval = SomeAttr;
	goto LEAVE;
    }

    /* NOTE: use of comma - SEQUENTIAL EXECUTION REQUIRED */
    /* NOTE: the && assumes/requires left to right execution */
    for( PreviousAttr = TheConstituent->attr_hash_buckets[BucketNumber] ,
	 SomeAttr = PreviousAttr->next;
	 SomeAttr != NULL_ATTRIBUTE &&
	    SomeAttr->attribute_id != attrid;
	 PreviousAttr = SomeAttr , 
	 SomeAttr = SomeAttr->next) {};

    if( SomeAttr == NULL_ATTRIBUTE ){
	rval = ERROR_ATTRIBUTE;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FindAndRemoveAttr]: which attributes say has attr " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attrid].attrid_name );
	dummy = TKERR_Format( " but not in hash list" );
	dummy = TKERR_EndErrMsg();
    }
    else {
	rval = SomeAttr;
	PreviousAttr->next = SomeAttr->next;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}


ATTR_attribute_t ATTR_FindAttr(constituent, attrid)
    IN CONSTITUENT  constituent;
    IN INT_type	    attrid;
{

    REGISTER ATTR_attribute_t	rval;

    INTERNAL_CONSTITUENT    TheConstituent;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    for (rval=TheConstituent->attr_hash_buckets[ATTR_Hash(attrid)];
	    rval!=NULL_ATTRIBUTE;
	    rval=rval->next) {

	if (rval->attribute_id == attrid) {
	    goto LEAVE;
	}
    }

    rval = ERROR_ATTRIBUTE;
    TKError = NO_ATTR;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format(
	"[ATTR_FindAttr]: can not find the attribute  " ); 
    dummy = TKERR_Format( ATTR_AttrNameTbl[attrid].attrid_name );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}



INT_type ATTR_KindOfAttr(attribute)
    IN INT_type attribute;
{

    /*
	This subroutine is used in the GetAttr process.
	Given an attribute, this subroutine will determine
	the kind of the attribute. The options are:
	
	    an attribute that doesnot default (i.e. is
		mandatory or non-mandatory)
	    an attribute that does default but does not
		have parameters
	    an attribute that does default and has
		parameters that default independently
	    an attribute that defaults and has parameters
		that don't default independently

    */

    /* Dont forget the problem attribute:
	rg_NUMBER_OF_PELS_PER_LINE.
      And what about rg_NUMBER_OF_LINES - is this REALLY
	non-mandaotry ???!!!
    */

    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch(attribute){

	case at_DIMENSIONS:
        case at_MEDIUM_TYPE:
        case at_OFFSET:
        case at_SEPARATION:
	    rval = ATTR_INDEPENDENT_PARM;
	    break;
	case at_POSITION:
	case at_BORDER:
/*
        case cc_KERNING_OFFSET:
        case cc_INITIAL_OFFSET:
*/
	case rg_CLIPPING:
	    rval = ATTR_RELATED_PARM;
	    break;
	case at_OBJECT_TYPE:
	case at_OBJECT_IDENTIFIER:
	case at_CONTENT_GENERATOR:
	case at_OBJECT_CLASS:
	case at_SUBORDINATES:
	case at_CONTENT_PORTIONS:
	case at_PRESENTATION_STYLE:
	case at_DEFAULT_VALUE_LISTS:
	case at_IMAGING_ORDER:
	case at_LAYOUT_STYLE:
	case at_CONTENT_ID_LOGICAL:
	case at_CONTENT_ID_LAYOUT:
	case at_CONTENT_INFORMATION:
	case at_ALT_REP:
	    rval = ATTR_DOESNT_DEFAULT;
	    break;
	default:
	    rval = ATTR_DEFAULTS_NO_PARM;
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



INT_type ATTR_SterilizeParmMask(attribute, SterilizedMask)
    IN INT_type		attribute;
    OUT PARM_MASK_type	*SterilizedMask;
{

    /*
	This subroutine is used by GetAttr. Passed an attribute,
	it is the responsibility to return a parm mask for use
	in the inheritance process. This parm mask must indicate all
	parameters that apply to the given attribute. For attributes
	with parameters, the sterilized mask is simply the or of the
	appropriate define'd _parm's. For attribute without parameters
	ALL_PARMS is returned.
    */

    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    switch(attribute){

	case at_DIMENSIONS:
	    *SterilizedMask = (PARM_MASK_type) 
		( HORIZONTAL_DIMENSION_parm | VERTICAL_DIMENSION_parm );

	    break;
        case at_MEDIUM_TYPE:
	    *SterilizedMask = (PARM_MASK_type) 
		( NOMINAL_PAGE_SIZE_parm | SIDE_OF_SHEET_parm );
	    break;	    
        case at_OFFSET:
	    *SterilizedMask = (PARM_MASK_type) 
		( LEADING_OFFSET_parm | TRAILING_OFFSET_parm |
		  LEFT_HAND_OFFSET_parm | RIGHT_HAND_OFFSET_parm );
	    break;	    
        case at_SEPARATION:
	    *SterilizedMask = (PARM_MASK_type) 
		( LEADING_EDGE_parm | TRAILING_EDGE_parm |
		  CENTRE_SEPARATION_parm );
	    break;
	case at_POSITION:
	    *SterilizedMask = (PARM_MASK_type)
		( HORIZONTAL_POSITION_parm | VERTICAL_POSITION_parm |
		 POS_LEADING_OFFSET_parm | POS_TRAILING_OFFSET_parm |
		POS_RIGHT_HAND_OFFSET_parm | POS_LEFT_HAND_OFFSET_parm |
		POS_LEADING_EDGE_parm | POS_TRAILING_EDGE_parm |
		POS_ALIGNMENT_parm | POS_CENTRE_SEPARATION_parm |
		POS_FILL_ORDER_parm );
	    break;
	case at_BORDER:
	    *SterilizedMask = (PARM_MASK_type)
		( LEFT_HAND_EDGE_parm | RIGHT_HAND_EDGE_parm |
		  BORDER_TRAILING_EDGE_parm | BORDER_LEADING_EDGE_parm );
	    break;
/*
        case cc_KERNING_OFFSET:
	    *SterilizedMask = (PARM_MASK_type)
		( START_EDGE_OFFSET_parm | END_EDGE_OFFSET_parm );
	    break;
        case cc_INITIAL_OFFSET:
	    *SterilizedMask = (PARM_MASK_type)
		( HORIZONTAL_COORDINATE_parm | VERTICAL_COORDINATE_parm );
	    break;	    
*/
	case rg_CLIPPING:
	    *SterilizedMask = (PARM_MASK_type)
		( COORD1_parm | COORD2_parm );
	    break;

	default:
	    *SterilizedMask = ALL_PARMS;
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



INT_type ATTR_CheckAndCleanMask( attribute, UserMask, CleanMask )
    IN INT_type	attribute;
    IN PARM_MASK_type UserMask;
    OUT PARM_MASK_type *CleanMask;
{

    /*
	This routine is used in SetAttr to clean up the mask
	passed in by the user. Specifically, if the mask is
	ALL_PARMS and the attribute has parameters then the 
	appropriate mask is obtained and returned in the CleanMask
	parameter. For attributes without parameters UserMask is
	returned.
    */

    INT_type rval;

    INT_type dummy;

    INT_type KindOfAttr;

    PARM_MASK_type SterilizedMask;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    dummy = ATTR_SterilizeParmMask( attribute, &SterilizedMask );
	    /* 
		SterilizedMask is either:
		    All_PARMS for attrs with NO parms
		    or
		    a sterilized mask for attrs with parms
	    */

    KindOfAttr = ATTR_KindOfAttr( attribute );

    if( KindOfAttr == ATTR_INDEPENDENT_PARM ||
	KindOfAttr == ATTR_RELATED_PARM ){
	/* Attribute has parameters */
	if( UserMask == NO_PARMS ){
	    *CleanMask = NO_PARMS;
	}
	else if( UserMask == ALL_PARMS ){
	    *CleanMask = SterilizedMask;
	}
	else {
	    *CleanMask = UserMask & SterilizedMask;
	    if( *CleanMask == (PARM_MASK_type) 0 ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[ATTR_CheckAndCleanMask]: clean mask value of (PARM_MASK_type) 0" ); 
		dummy = TKERR_EndErrMsg();
	    }
	}
    }
    else {
	/* 
	    The attribute doesn't have parameters.
	    The user's mask is irrelevent.
	*/
	*CleanMask = ALL_PARMS;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_ThreadTree( constituent, value )
    IN CONSTITUENT constituent;
    IN POINTER_type value;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_CONSTITUENT_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_ThreadTree]: need a sequence of constituents" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = 0; i < TheSequence->length; i++ ){

	TheConstituent = (struct CONSTITUENT_struct *) 
	    TheSequence->sequence_value.constituents[i];

	TheConstituent->parent = (struct CONSTITUENT_struct *) constituent;
    }
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_UnThreadTree( constituent )
    IN CONSTITUENT constituent;
{

    INT_type rval;

    INT_type SlaveRval;

    ATTR_attribute_t AttrStructPtr;

    INT_type i;

    INTERNAL_CONSTITUENT sibling;

    SEQUENCE_CONSTITUENT_type Subordinates;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    SlaveRval = HasAttr( constituent, at_SUBORDINATES );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_UnThreadTree]: Constituent doesn't have the subordinates attribute" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	/* trivially the tree is unthreaded */
	goto LEAVE;
    }

    AttrStructPtr = ATTR_FindAttr( constituent, at_SUBORDINATES );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_UnThreadTree]: constituent has the subordinates attribute but can not find it" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    Subordinates = GET_SEQUENCE_CONSTITUENT( AttrStructPtr );

    for( i = 0; i < Subordinates->length; i++ ){
	
	sibling = (INTERNAL_CONSTITUENT) 
	    Subordinates->sequence_value.constituents[i];

	sibling->parent = (struct CONSTITUENT_struct *) NULL_CONSTITUENT;
    }
    
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}
