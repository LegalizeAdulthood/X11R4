
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr7.c,v 1.3 89/11/01 15:47:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr7.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr7.c,v $
 * Revision 1.3  89/11/01  15:47:43  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.2  89/10/31  14:11:37  annm
 * fixing lint problems
 * 
 * Revision 1.1  89/09/21  06:23:59  mss
 * Initial revision
 * 
 * Revision 1.11  89/09/12  09:10:56  mss
 * Removed #
 * 
 * Revision 1.10  89/04/11  15:32:55  annm
 * I can't remember what I did here
 * 
 * Revision 1.9  88/09/30  15:40:39  annm
 * fixing VMS bugs
 * 
 * Revision 1.8  88/09/21  15:47:51  annm
 * added tkerr stuff
 * 
 * Revision 1.7  88/09/12  12:59:34  annm
 * buffed rough edge in end of defaulting condition handling for related parms.
 * 
 * Revision 1.6  88/09/06  13:10:22  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.5  88/09/01  14:19:32  annm
 * restructured the constituent inherit routine to put the code that actually
 * does the inherit in its own subr. Dealing with the termination of inheritance is still done in in the constituent inherit routine.
 * 
 * Revision 1.4  88/08/30  15:11:30  annm
 * changed _constant's to _const's
 * 
 * Revision 1.3  88/08/16  18:13:33  annm
 * no change
 * 
 * Revision 1.2  88/08/15  18:16:53  annm
 * changes associated with addition of code
 * for finding document profile application
 * defaults
 * 
 * Revision 1.1  88/08/10  15:42:53  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr7.c,v $";
#endif



#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR7
#define TK_TKI_TKERR

#include <acd.h>
#include <rnames.h>
#include <attr0.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR7
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */



INT_type ATTR_InheritFromObjClass( constituent, attribute, value, mask,
	wanted_mask, destination_mask, destination )
    IN CONSTITUENT	    constituent;
    IN INT_type		    attribute;
    OUT POINTER_type	    value;
    OUT PARM_MASK_type	    *mask;
    IN PARM_MASK_type	    wanted_mask;
    IN OUT PARM_MASK_type   *destination_mask;
    IN OUT POINTER_type	    *destination;
{

    INT_type rval;

    INT_type SlaveRval;
    
    ATTR_attribute_t	AttrStructPtr;

    CONSTITUENT	    TheObjectClass;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* First see if the constituent has the object class attribute */
    SlaveRval = HasAttr( constituent, at_OBJECT_CLASS );
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
    AttrStructPtr = ATTR_FindAttr( constituent, at_OBJECT_CLASS );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_InheritFromObjClass]: constituent has attribute " ); 
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" but can not find the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* now get the value */
    TheObjectClass = (CONSTITUENT) GET_CONSTITUENT( AttrStructPtr );

    /* 
	FINALLY: attempt to inherit from the object class and
	any associated style.
    */
    SlaveRval = ATTR_InheritFromConstAndStyle( TheObjectClass,
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



INT_type ATTR_InheritFromConstAndStyle( constituent,
	attribute, value, mask, wanted_mask, destination_mask, 
	destination )
    IN CONSTITUENT	    constituent;
    IN INT_type		    attribute;
    OUT POINTER_type	    value;
    OUT PARM_MASK_type	    * mask;
    IN PARM_MASK_type	    wanted_mask;
    IN OUT PARM_MASK_type   *destination_mask;
    IN OUT POINTER_type	    *destination;
{

    INT_type rval;

    INT_type SlaveRval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* first attempt to inherit from the constituent */

    SlaveRval = ATTR_InheritFromConst( constituent, attribute, value, mask,
		    wanted_mask, destination_mask, destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
	goto LEAVE;	
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	rval = ALL_DONE;
	goto LEAVE;
    }
    
    /*
	Now try styles - note that because the attributes
	that can go on presentation styles versus
	layout styles are disjoint, both styles
	can be examined in arbitrary order.
    */
    
    /* PRESENTATION STYLE */
    SlaveRval = ATTR_InheritFromStyle( constituent, attribute, value, 
		    mask, wanted_mask, destination_mask, destination,
		    /* style to look on */ at_PRESENTATION_STYLE );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	rval = ALL_DONE;
	goto LEAVE;
    }

    /* LAYOUT STYLE */
    SlaveRval = ATTR_InheritFromStyle( constituent, attribute, value, 
		    mask, wanted_mask, destination_mask, destination,
		    /* style to look on */ at_LAYOUT_STYLE );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	rval = ALL_DONE;
    }
    else {
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



INT_type ATTR_InheritFromStyle( constituent, attribute, value, mask,
	wanted_mask, destination_mask, destination,
	StyleId )
    IN CONSTITUENT	    constituent;
    IN INT_type		    attribute;
    OUT POINTER_type	    value;
    OUT PARM_MASK_type	    * mask;
    IN PARM_MASK_type	    wanted_mask;
    IN OUT PARM_MASK_type   *destination_mask;
    IN OUT POINTER_type	    *destination;
    IN INT_type		    StyleId;
{
    
    INT_type rval;

    INT_type SlaveRval;
    
    ATTR_attribute_t	AttrStructPtr;

    CONSTITUENT	    TheStyle;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* First see if the constituent has the object class attribute */
    SlaveRval = HasAttr( constituent, StyleId );
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
    AttrStructPtr = ATTR_FindAttr( constituent, StyleId );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_InheritFromStyle]: constituent has attribute " ); 
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" but can not find the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* now get the value */
    TheStyle = (CONSTITUENT) GET_CONSTITUENT( AttrStructPtr );

    /* 
	FINALLY: attempt to inherit from the object class and
	any associated style.
    */
    SlaveRval = ATTR_InheritFromConst( TheStyle,
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



INT_type ATTR_InheritFromConst( constituent, attribute, value, mask, 
	wanted_mask, destination_mask, destination )
    IN CONSTITUENT	    constituent;
    IN INT_type		    attribute;
    OUT POINTER_type	    value;
    OUT PARM_MASK_type	    *mask;
    IN PARM_MASK_type	    wanted_mask;
    IN OUT PARM_MASK_type   *destination_mask;
    IN OUT POINTER_type	    *destination;
{

    INT_type rval;

    INT_type SlaveRval;
    
    INT_type	    dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    SlaveRval = ATTR_LocateConstAttr( constituent, attribute, wanted_mask,
		    destination_mask, destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
    if( SlaveRval == ERROR_INT ){
	/* ASSET: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }

    if( SlaveRval == ALL_DONE ){
	/* We set up the return to the user */
	dummy = ATTR_SetUpReturnToUser( attribute, value, mask, 
	    destination_mask, destination );
	if( dummy == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	}
	else {
	    /* return is correctly set  */
	    rval = ALL_DONE;
	}
	goto LEAVE;
    }

    if( SlaveRval == TIME_TO_STOP ){
	/* 
	    ASSERT: must be related parameters attribute
	    and part of the value has been found - go get
	    the defaults.
	*/
	dummy = ATTR_GetDefaults( constituent, attribute, value,
		mask, wanted_mask, destination_mask, destination );
	if( dummy == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	goto LEAVE;
    }

    /* keep going - AreWeDone must have returned NOT_DONE */
    rval = NOT_DONE;
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_LocateConstAttr( constituent, attribute, wanted_mask, destination_mask, destination )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    IN OUT POINTER_type *destination;

{

    INT_type rval;

    INT_type SlaveRval;

    ATTR_attribute_t AttrStructPtr;

    INT_type TypeIndex;

    POINTER_type source;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* First see if the constituent has the attribute */
    SlaveRval = HasAttr( constituent, attribute );
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
    AttrStructPtr = ATTR_FindAttr( constituent, attribute );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_LocateConstAttr]: constituent has attribute " ); 
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" but can not find the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    TypeIndex = ATTR_IndexIntoTypes[attribute];

    source = (*ATTR_AttrTypeTbl[TypeIndex].get)(AttrStructPtr);

    SlaveRval = ATTR_LoadDestination( attribute, source,
		    /* source mask */ AttrStructPtr->parm_mask,
		    wanted_mask, destination_mask, destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }

    rval = ALL_DONE;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_LoadDestination( attribute, source, source_mask,
	wanted_mask, destination_mask, destination )
    IN INT_type attribute;
    IN POINTER_type source;
    IN PARM_MASK_type source_mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type TypeIndex;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;
	
    TypeIndex = ATTR_IndexIntoTypes[attribute];

    if( *destination == (POINTER_type) 0 ){
	*destination = (*ATTR_AttrTypeTbl[TypeIndex].allocate)(source);
	if( *destination == ERROR_POINTER ){
	    /* ASSERT: TKError has been set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}
    }

    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].copy)(source,
		    source_mask, wanted_mask, destination_mask,
		    *destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)(*destination);
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



INT_type ATTR_AreWeDone( attribute, wanted_mask, destination_mask )
    IN INT_type attribute;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
{

    /*
	XOR - AND <BHAM!!!> YOU'RE READY TO GO

	ASSERT: If we get in here the attribute is defaultable.
    
	ASSERT: This code assumes that a destination mask != 0
		means that something has been loaded into the
		destination. Conversly, a destination mask
		== (PARM_MASK_type) 0 means that nothing has been
		loaded into the destination.

	If the attribute has parameters that default independently
	    the two masks must be identical to be done.

	If the attribute has parameters that default together
	    the destination_mask != 0 means we are done.

	If the attribute doesn't have parameters then
	    the destination_mask != 0 means that we are done.

    */

    INT_type rval;

    INT_type KindOfAttr;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    KindOfAttr = ATTR_KindOfAttr( attribute );

    switch( KindOfAttr ){

    case ATTR_INDEPENDENT_PARM:
	if( ( wanted_mask ^ *destination_mask ) == (PARM_MASK_type ) 0 ){
	    rval = ALL_DONE;    
	}
	else {
	    rval = NOT_DONE;
	}
	break;

    case ATTR_RELATED_PARM:
	if( (wanted_mask ^ *destination_mask ) == (PARM_MASK_type) 0 ){
	    rval = ALL_DONE;
	}
	else if( *destination_mask != (PARM_MASK_type) 0 ){
	    rval = TIME_TO_STOP;
	}
	else {
	    rval = NOT_DONE;
	}
	break;

    case ATTR_DEFAULTS_NO_PARM:
	if( *destination_mask != (PARM_MASK_type) 0 ){
	    rval = ALL_DONE;
	}
	else {
	    rval = NOT_DONE;
	}
	break;

    default:
	/* UGH */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_AreWeDone]: bad kind of attribute " ); 
	dummy = TKERR_EndErrMsg();
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



INT_type ATTR_SetUpReturnToUser( attribute, value, mask, 
		    return_mask, return_value )
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type *return_mask;
    IN POINTER_type *return_value;
{


    INT_type rval;

    INT_type TypeIndex;

    POINTER_type TheReturnValue;
    PARM_MASK_type TheReturnMask;

    INT_type SlaveRval;
    PARM_MASK_type SterileMask;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    TypeIndex = ATTR_IndexIntoTypes[attribute];

    TheReturnValue = *return_value;
    TheReturnMask = *return_mask;

    if( TypeIndex == INT_const ){
	* (INT_type *) value = * (INT_type *) TheReturnValue;
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)(TheReturnValue);
    }
    else if( TypeIndex == VALUE_const ){
	* (VALUE_type *) value = * (VALUE_type *) TheReturnValue;
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)(TheReturnValue);
    }
    else if( TypeIndex == CONSTITUENT_const ){
	* (CONSTITUENT *) value = (CONSTITUENT) TheReturnValue;
    }
    else {
	/* it has structure type */
	* (POINTER_type *) value = TheReturnValue;
	if( mask != (PARM_MASK_type *) 0 ){

	    /* 
		The alert code reader will no doubt wonder why the
		return mask is AND'ed with a SterileMask before return
		to the user <sic!>. For an attribute with related parms,
		the canonical copy routine will set the destination_mask
		to NO_PARMS (if the source mask is NO_PARMS) to stop
		the inheritance. The canonical default will then OR in
		bits to the destination mask as it copies over the default
		into the destination. However, the NO_PARMS bit is
		still on - and this is a crumb that should not be
		returned to the user. Thus, by AND'ing with the
		sterile mask all crumb bits get zapped.
	    */
	    SlaveRval = ATTR_SterilizeParmMask( attribute, &SterileMask );
	    if( SlaveRval == ERROR_INT ){
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    *mask = TheReturnMask & SterileMask;
	}
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );


}



INT_type ATTR_GetDefaults( constituent, attribute, value, mask, wanted_mask,
    destination_mask, destination )
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	OUT POINTER_type value;
	OUT PARM_MASK_type *mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type dummy;

    INT_type SlaveRval;

    INT_type TypeIndex;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* 
	Attempt to get document profile defaults
    */

    SlaveRval = ATTR_GetDocProfDefaults( constituent, attribute, value,
		    mask, wanted_mask, destination_mask, destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	dummy = ATTR_SetUpReturnToUser( attribute, value, mask,
		destination_mask, destination );
	if( dummy == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	}
	goto LEAVE;
    }

    /*
	As a last resort get the ISO 8613 defaults.
    */

    TypeIndex = ATTR_IndexIntoTypes[attribute];

    /* ASSERT: if there is a problem TKError gets set */

    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].iso_default)( constituent, 
		attribute, wanted_mask, destination_mask, destination );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	if( *destination != (POINTER_type) 0 ){
	    dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)( *destination );
	}
	goto LEAVE;
    }

    dummy = ATTR_SetUpReturnToUser( attribute, value, mask, destination_mask,
		destination );    
    if( dummy == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}
