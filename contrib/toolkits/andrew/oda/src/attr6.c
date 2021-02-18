
/* 
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr6.c,v 1.3 89/10/31 14:11:18 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr6.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr6.c,v $
 * Revision 1.3  89/10/31  14:11:18  annm
 * fixing lint problems
 * 
 * Revision 1.2  89/10/16  15:30:20  annm
 * fixing problems with macro arguements
 * fixing problems with macro arguements
 * 
 * Revision 1.1  89/09/21  06:23:36  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/12  09:10:38  mss
 * Removed #
 * 
 * Revision 1.11  89/04/11  15:33:28  annm
 * I can't remember what I did here
 * 
 * Revision 1.10  88/12/19  15:57:04  annm
 * fixed missing parameter to GetDefaults call in InheritForContentPortion
 * 
 * Revision 1.9  88/11/29  11:05:54  annm
 * fixed bug in ATTR_FindLocal - rval not set to success int as first statement of procedure (Ann's paradyme).
 * 
 * Revision 1.8  88/09/21  15:47:42  annm
 * added tkerr stuff
 * 
 * Revision 1.7  88/09/06  13:10:15  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.6  88/08/30  15:10:40  annm
 * changed _constant's to _const's
 * 
 * Revision 1.5  88/08/16  18:12:31  annm
 * changes to permit inheritance from default value lists
 * 
 * Revision 1.4  88/08/15  18:18:28  annm
 * no change
 * 
 * Revision 1.3  88/08/10  15:42:48  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults for the currently implemented aunion.h types
 * 
 * Revision 1.2  88/08/01  18:45:31  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.1  88/08/01  13:44:07  annm
 * Initial revision
 * 
*/



#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR6
#define TK_TKI_TKERR

#include <parms.h>
#include <acd.h>
#include <rnames.h>
#include <attr0.h>
#include <errors.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR6
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */


INT_type ATTR_FindLocal( constituent, attribute, value, mask)
    IN CONSTITUENT	    constituent;
    IN INT_type		    attribute;
    OUT POINTER_type	    value;
    OUT PARM_MASK_type	    *mask;
{

    /*
	Strategy:
	    1) See if the constituent has the attribute -
		if no the return NOATTR_err
	    2) if has the attribute then:
		a) Find the attribute (If this fails return
		    INTERNAL_err)
		b) get the index into the type table.
		c) get the base address of the attribute's
		    value 
	    3) If the type is not int, value or constituent:
		a) allocate space for a copy (If this fails
		    return
		b) copy 
		c) set up the parm mask returned to the user
		d) set up the return of the value to the user
	    4) if the type is int, value or constituent:
		just stuff the return value out to the user
		N.B. no parm mask is stuffed out to the user
    */

    INT_type rval;

    ATTR_attribute_t CurrentValue;

    INT_type TypeIndex;

    POINTER_type BasePtr;
    POINTER_type CopyPtr;

    INT_type SlaveRval;

    PARM_MASK_type WhatWeWantMask;
    PARM_MASK_type WhatWeHaveMask;
    PARM_MASK_type SterileMask;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* First: make sure the constituent has subordinates */
    SlaveRval = HasAttr( constituent, attribute );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[ATTR_FindLocal]: HasAttr failed with attribute = %d", 
	    attribute  );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = ERROR_INT;
	TKError = NOATTR_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FindLocal]: the constituent doesn't have attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* 
	Second: find subordinates in the constituent's
	hash buckets.
    */
    CurrentValue = ATTR_FindAttr( constituent, attribute );
    if( CurrentValue == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FindLocal]: Constituent has the attribute but FindAttr fails" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }	

    TypeIndex = ATTR_IndexIntoTypes[attribute];

    /* first get the base address */
    BasePtr = (*ATTR_AttrTypeTbl[TypeIndex].get)(CurrentValue);
    if( BasePtr == ERROR_POINTER ){
	/* assert: lower level set TKError */
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = ATTR_SterilizeParmMask( attribute, &WhatWeWantMask );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FindLocal]: can get a sterile mask for attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    /* save copy of the sterile mask for later on */
    /* see comment below */
    SterileMask = WhatWeWantMask;
    
    if( TypeIndex != INT_const && TypeIndex != VALUE_const &&
	TypeIndex != CONSTITUENT_const ){
	/* next allocate space for the copy */
	CopyPtr = (*ATTR_AttrTypeTbl[TypeIndex].allocate)(BasePtr);
	if( CopyPtr == ERROR_POINTER ){
	    /* assert: lower level set TKError */
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	WhatWeHaveMask = (PARM_MASK_type) 0;
	SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].copy)(
		/* source */ BasePtr, 
		/* source mask - what's there  */ CurrentValue->parm_mask, 
		/* what we want */ WhatWeWantMask, 
		/* what we have */ &WhatWeHaveMask,
		/* destination */ CopyPtr);
	if( SlaveRval == ERROR_INT ){
	    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].free)(CopyPtr);
	    TKError = INTERNAL_err;
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FindLocal]: can not perform copy of value for attribute  " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	/* set up return to the user */

	* (POINTER_type *) value = CopyPtr;

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
	    *mask = WhatWeHaveMask & SterileMask;
	}

    }
    else if( TypeIndex == INT_const  ){
	/* get returned base address - deref and stuff */
	* (INT_type *) value = * (INT_type *) BasePtr;
    }
    else if( TypeIndex == VALUE_const ){
	/* get returned base address - deref and stuff */
	* (VALUE_type *) value = * (VALUE_type *) BasePtr;
    }
    else {
	/* for INT, VALUE and CONSTITUENT get get's the value to return */
	* (CONSTITUENT *) value = (CONSTITUENT) BasePtr;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_InhertForObject( constituent, attribute, value, mask )
    IN CONSTITUENT	    constituent;
    IN INT_type		    attribute;
    OUT POINTER_type	    value;
    OUT PARM_MASK_type	    *mask;
{

    INT_type rval;

    INT_type SlaveRval;

    PARM_MASK_type  wanted_mask;
    PARM_MASK_type  destination_mask;

    POINTER_type    destination;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* Assume success */
    rval = SUCCESS_INT;

    /* 
	Set up the wanted and destination masks, and the 
	initially empty destination.
    */

    SlaveRval = ATTR_SterilizeParmMask( attribute, &wanted_mask );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    destination_mask = (PARM_MASK_type) 0;
    destination = (POINTER_type) 0;

    /* 
	First look on the constituent and associated
	style if any.
    */

    SlaveRval = ATTR_InheritFromConstAndStyle( constituent,
		    attribute, value, mask, 
		    wanted_mask, &destination_mask, &destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: some has set TKError */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	goto LEAVE;
    }

    /*
	Second: look for an object class, and if found inherit
    */

    SlaveRval = ATTR_InheritFromObjClass( constituent,
		    attribute, value, mask,
		    wanted_mask, &destination_mask, &destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	goto LEAVE;
    }

    /*
	PUT CODE FOR RESOURCE DOCUMENT HERE
    */

    /*
	Attempt to inherit from the default value lists.
    */
    SlaveRval = ATTR_InheritFromDVL( constituent, attribute, value,
		    mask, wanted_mask, &destination_mask, &destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	goto LEAVE;
    }
    
    /*
	Go get defaults - note this will get everything we
	still need and set up the return to the user.
	This will look for defaults in the document profile
	and for ISO 8613 defaults (if needed).
    */

    SlaveRval = ATTR_GetDefaults( constituent, attribute, value, mask, 
		wanted_mask, &destination_mask, &destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_INT;
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



INT_type ATTR_InheritForContentPortion( constituent, attribute, value, mask )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
{

    INT_type rval;

    INT_type SlaveRval;

    PARM_MASK_type  wanted_mask;
    PARM_MASK_type  destination_mask;

    POINTER_type    destination;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* Assume success */
    rval = SUCCESS_INT;

    /* 
	Set up the wanted and destination masks, and the 
	initially empty destination.
    */

    SlaveRval = ATTR_SterilizeParmMask( attribute, &wanted_mask );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    destination_mask = (PARM_MASK_type) 0;
    destination = (POINTER_type) 0;

    /* 
	First look on the constituent 
    */

    SlaveRval = ATTR_InheritFromConst( constituent,
		    attribute, value, mask, 
		    wanted_mask, &destination_mask, &destination );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: some has set TKError */
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	/* ASSERT: return to the user has been established */
	goto LEAVE;
    }

    /*
	Go get defaults - note this will get everything we
	still need and set up the return to the user
    */

    /* ASSERT: if there is a problem then TKError gets set */
    SlaveRval = ATTR_GetDefaults( constituent, attribute, value, mask, 
		    wanted_mask, &destination_mask, &destination );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
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




