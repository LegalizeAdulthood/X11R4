
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr1.c,v 1.2 89/10/31 12:47:25 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr1.c,v $
 * Revision 1.2  89/10/31  12:47:25  jr
 * Casting for use of sizeof.
 * 
 * Revision 1.1  89/09/21  06:15:50  mss
 * Initial revision
 * 
 * Revision 1.70  89/09/01  14:17:27  annm
 * fixing USES_LONG_NAMES problems
 * 
 * Revision 1.69  89/04/11  15:37:01  annm
 * I can't remember what I did here
 * 
 * Revision 1.68  89/02/09  15:07:07  annm
 * minor tweek in setattr
 * 
 * Revision 1.67  89/01/26  12:41:05  annm
 * no change
 * 
 * Revision 1.66  88/12/13  19:52:17  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.65  88/11/10  10:54:31  mss
 * Changed error message in RemAttr to match
 * routine name
 * 
 * Revision 1.64  88/10/25  11:09:09  mss
 * Changed order of error checking in get/set attr;
 * Changed error messages so that they give more info
 * 
 * Revision 1.63  88/10/24  11:04:26  mss
 * Added check for NO_PARMS and ALL_PARMS as
 * passed in values to GetAttr (not clear if
 * this is correct or not, since it is not clear
 * if mask should be examined if the attribute has
 * no parameters.)
 * 
 * Revision 1.62  88/09/30  15:41:52  annm
 * no change
 * 
 * Revision 1.61  88/09/23  15:11:10  annm
 * now free attr struct and outboard storage when failures occur 'later on' in SetAttr
 * 
 * Revision 1.60  88/09/12  15:56:18  annm
 * fixed bugs in tkerr calls and added dependency on <tkerr.h>
 * 
 * Revision 1.59  88/09/12  12:57:37  annm
 * added TKError message stuff
 * 
 * Revision 1.58  88/09/06  13:05:52  annm
 * changed ""'s in includes to <>
 * 
 * Revision 1.57  88/09/06  12:00:48  annm
 * no change
 * 
 * Revision 1.56  88/08/25  16:10:55  annm
 * setattr now also threads tree for content portions to 
 * support collaspe content portions operation
 * 
 * Revision 1.55  88/08/16  18:11:43  annm
 * added tree threading for setattr of at_SUBORDINATES
 * 
 * Revision 1.54  88/08/10  15:41:57  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults for the currently implemented aunion.h types
 * 
 * Revision 1.53  88/08/01  18:44:03  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN
  * (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.52  88/07/29  12:10:45  jr
 * Rmove GetValueName (it is in
 * debug1.c now).
 * 
 * Revision 1.51  88/07/29  10:35:02  mss
 * Changed MOATTR to NOATTR;
 * Change HasParm to HasIndepParm (with changes in semantics)
 * 
 * Revision 1.50  88/07/28  13:44:49  mss
 * Changed defintion of GetAttr so that null pointer for
 * parm means do not return value, not an error. Also,
 * validation of parm is always done to check for
 * gratuitous use of all_parms and no_parms
 * 
 * Revision 1.49  88/07/27  14:51:25  mss
 * Implemented GetAttrName function
 * 
 * Revision 1.48  88/07/27  14:03:28  mss
 * Added parameter validation for parm pointer in
 * GetAttr before using it (make guesses about
 * common errors)
 * 
 * Revision 1.47  88/07/27  13:31:24  mss
 * Changed GetAttr so that the parameter mask returnvalue
 * is assigned only for those attributes with
 * parameters
 * 
 * Revision 1.46  88/07/26  18:41:13  annm
 * added copy, allocate, get and set canonical procedures for at_OFFSET_type and at_SEPARATION_type
 * 
 * Revision 1.45  88/07/19  13:09:07  mss
 * typo
 * 
 * Revision 1.44  88/07/18  18:09:37  annm
 * changes associated with having GetAttr
 * use the canonical procedures for operating on types
 * 
 * Revision 1.43  88/07/15  18:20:18  annm
 * changes for using canonical procedures in SetAttr
 * 
 * Revision 1.42  88/07/14  18:07:29  annm
 * changes for use of aunion.h canonical procedures in SetAttr
 * 
 * Revision 1.41  88/07/14  17:47:48  annm
 * changes to use canonical procedures for operating on aunion.h types for the SetAttr stuff
 * 
 * Revision 1.40  88/07/07  19:14:47  annm
 * tweek
 * 
 * Revision 1.39  88/07/07  18:33:45  mss
 * Changed GetAttrName and GetValueName so they
 * return C strings (char *) rather than ODA string_types
 * 
 * Revision 1.38  88/06/17  15:02:33  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.37  88/06/13  17:27:08  annm
 * changed Set Int/Value Attr parm mask parameter back to ALL_PARMS
 * 
 * Revision 1.36  88/06/13  17:20:36  annm
 * changed the parm mask in Set INT/VALUE
 * Attr to (PARM_MASK_type) 0 
 * 
 * Revision 1.35  88/06/13  17:15:53  annm
 * change the ALL_PARM refs in SetIntAttr
 * and SetValueAttr to (PARM_MASK_type *) 0
 * 
 * Revision 1.34  88/06/13  15:35:00  jr
 * Fix some ref's to ALL_PARMS
 * Add include for parms.h
 * 
 * Revision 1.33  88/06/13  13:49:35  jr
 * Fix type of value arg to GetAttr
 * 
 * Revision 1.32  88/06/13  13:02:23  annm
 * changed param passed by Set/Get
 * Int/Values to ALL_PARM from 0... UGH !
 * 
 * Revision 1.31  88/06/10  17:27:41  annm
 * chnaged error return for attribute value out
 * of range from BADATTR_err (which is incorrect
 * given what is written in the API) to BADPARM_err
 * 
 * Revision 1.30  88/06/09  15:55:02  annm
 * no change
 * 
 * Revision 1.29  88/06/09  15:41:02  annm
 * fixed interface for int/value attributes in Get ... Attr code
 * 
 * Revision 1.28  88/06/08  16:52:56  annm
 * dealt with line too long in copyright notice and changed
 * #end's to #endif's
 * 
 * Revision 1.27  88/06/08  14:33:58  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr1.c,v $";
#endif

/* Put .c template comment here */


#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR1
#define TK_GVARS_ATTR0
#define TK_IMPL_CONST
#define TK_TKI_STR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <parms.h>
#include <acd.h>
#include <str.h>
#include <mm.h>
#include <attr0.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR1
#undef TK_IMPL_CONST
#undef TK_TKI_STR
#undef TK_TKI_MM
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */


INT_type GetAttr(constituent, attribute, value, inherited, mask)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    OUT POINTER_type	value;
    IN BOOL_type	inherited;
    OUT PARM_MASK_type	*mask;	   
{

    /*
	Strategy:
	    1) check the arguements:
		a) the constituent is checked for obvious problems
		b) attribute is checked for in range and 
		    implemented
		c) value is accepted as an act of faith
		d) inherted must be either BOOL_true or BOOL_false
		c) mask is accepted as an act of faith
	    2) convert constituent to an INTERNAL_CONSTITUENT and
		sterilize the mask
	    3) figure out how to obtain the value of the attribute
    */

    INT_type		    rval;	    /* the returned value */

    INTERNAL_CONSTITUENT    TheConstituent;

    INT_type		    KindOfAttribute;

    INT_type		    dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* First: check the arguements */

    /* check the atttribute (first, so we can provide better msgs later */
    if( attribute < MIN_ATTR || attribute > MAX_ATTR ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[GetAttr]: attribute out of range (%d)",
				attribute);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check the constituent */
    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = 
	    TKERR_Format( "[GetAttr]: constituent parameter ERROR or NULL " );
	dummy = TKERR_Format("getting ");
	dummy = TKERR_Format( GetAttrName(attribute) );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }


    /* check the inherited */
    if( inherited != BOOL_true && inherited != BOOL_false ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = 
	    TKERR_Format( "[GetAttr]: inherited not BOOL_true or BOOL_false " );
	dummy = TKERR_Format("getting " );
	dummy = TKERR_Format( GetAttrName(attribute) );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check the Parm mask for illegal, but common values */
    if ( (((PARM_MASK_type) mask) == NO_PARMS) ||
	 (((PARM_MASK_type) mask) == ALL_PARMS) ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[GetAttr]: parm mask pointer must ");
	dummy = TKERR_Format( "be pointer, not ALL_PARMS or NO_PARMS ");
	dummy = TKERR_Format(" getting ");
	dummy = TKERR_Format( GetAttrName(attribute) );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* 
	Second: convert the constituent to an INTERNAL_CONSTITUENT
    */

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /* Third: figure out how to find the attribute */

    if( inherited == BOOL_false ){
	/* ASSERT: lower level will set TKError if an error occurs */
	rval = ATTR_FindLocal( constituent, attribute, value,
		mask );
	goto LEAVE;
    }
    
    /* ASSERT: Below this point, inherited == BOOL_true */


    if( CONST_IsDocProf( TheConstituent ) ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    if( CONST_IsLayoutStyle( TheConstituent ) ||
	CONST_IsPresentStyle( TheConstituent ) ){
	/* For now this is an error - if varient of get
	    approved, will need to inherit for styles */
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    if( CONST_IsObjCl( TheConstituent ) ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    KindOfAttribute = ATTR_KindOfAttr( attribute );

    if( CONST_IsObjNc( TheConstituent ) &&
	( KindOfAttribute == ATTR_RELATED_PARM ||  
	    KindOfAttribute == ATTR_INDEPENDENT_PARM ||
	    KindOfAttribute == ATTR_DEFAULTS_NO_PARM ) ){
	/* ASSERT: if error occurs, TKError will be set */
	rval = ATTR_InhertForObject( constituent, attribute,
		value, mask );
	goto LEAVE;
    }

    if( CONST_IsContent( TheConstituent ) &&
	( KindOfAttribute == ATTR_RELATED_PARM ||
	    KindOfAttribute == ATTR_INDEPENDENT_PARM  ||
	    KindOfAttribute == ATTR_DEFAULTS_NO_PARM ) ){
	/* ASSERT: given check above must be specific content.
	    If an error occurs then the lower levels set TKError.
	*/
	rval = ATTR_InheritForContentPortion( constituent,
		attribute, value, mask );
	goto LEAVE;
    }


    /* ASSERT: from this point, the only possibilities
	are that the attribute is mandatory/non-mandatory
	and associated with either an object or content
	portion. This is an error */

    rval = ERROR_INT;
    TKError = BADPARM_err;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}


INT_type GetIntAttr( constituent, attribute, value, inherited)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    OUT INT_type	*value;
    IN BOOL_type	inherited;
{

    INT_type	    rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = GetAttr( constituent, attribute, (POINTER_type) value,
		inherited, (PARM_MASK_type *) 0);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type GetValueAttr( constituent, attribute, value, inherited)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    OUT VALUE_type	*value;
    IN BOOL_type	inherited;
{

    INT_type	    rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = GetAttr( constituent, attribute, (POINTER_type) value, 
		inherited, (PARM_MASK_type *) 0);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



CHAR_type *GetAttrName(attribute)
    IN INT_type		attribute;
{

    CHAR_type	*rval;	    /* the returned value */

    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* check the attribute */
    if( attribute < MIN_ATTR || attribute > MAX_ATTR ) {
	rval = (CHAR_type *) 0;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[GetAttrName]: attribute out of range" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = ATTR_AttrNameTbl[attribute].oda_name;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type GetParmMask(constituent, attribute, mask)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    OUT PARM_MASK_type	*mask;
{

    /*
	Strategy:
	    1) check the constituent to ensure that it is not
		ERROR_CONSTITUENT or NULL_CONSTITUENT
	    2) check the attribute to ensure that the 
		attribute value is in range.
	    3) call HasAttr to see if the constituent
		has the attribute
	    4) if hasattr then find the attribute and
		set up return of parm mask (n.b. if the
		find attr fails then return error).
	    5) otherwise - return error
    */

    INT_type	rval;	    /* the returned value */

    INT_type	SlaveRval;

    ATTR_attribute_t TheAttribute;

    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* first check the constituent */
    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[GetParmMask]: the constituent value is bad - got ERROR or NULL CONSTITUENT" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( attribute < MIN_ATTR || attribute > MAX_ATTR ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[GetParmMask]: the attribute value is out of range - got " );
	dummy = TKERR_FormatInt( "%d", attribute );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* see if the constituent has the attribute */    
    SlaveRval = HasAttr( constituent, attribute );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = ERROR_INT;
	TKError = NOATTR_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[GetParmMask]: the constituent does not have the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* find the attribute */
    TheAttribute = ATTR_FindAttr( constituent, attribute );
    if( TheAttribute == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[GetParmMask]: the constituent has the attribute but can't find it" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* set up the return */
    *mask = TheAttribute->parm_mask;    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}

INT_type HasAttr(constituent, attribute)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
{

    INT_type	rval;	    /* the returned value */
    INTERNAL_CONSTITUENT    TheConstituent;

    INT_type dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* check the constituent */
    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    if( attribute < MIN_ATTR || attribute > MAX_ATTR ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[HasAttr]: attribute out of range" );
	dummy = TKERR_EndErrMsg();
    }
    if( CONST_UnsafeHasAttr( TheConstituent, attribute ) ) {
	rval = HAS_ATTR;
    }
    else {
	rval = NO_ATTR;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



INT_type RemAttr(constituent, attribute)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
{

    /*
	Strategy:

	    1) check the constituent to ensure that it is not
		NULL_CONSTITUENT or ERROR_CONSTITUENT
	    2) check the attribute to ensure that it is in range
	    3) call HasAttr to determine if this constiuent has 
		the attribute
	    4) if the constituent does not have the attribute the
		make a NOATTR_err return; 
	    5) Otherwise call ATTR_FindAndRemove to locate the attribute
		and remove it from the constituent's attribute
		list system. Then call the canonical free routine to 
		free the space associated with the attribute value.
		Finally, free the attribute structure itself.
    */

    INT_type	rval;	    /* the returned value */

    INT_type	SlaveRval;

    ATTR_attribute_t TheAttribute;

    INT_type	TypeIndex;

    POINTER_type TheValue;

    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* first check the constituent */
    if( constituent == ERROR_CONSTITUENT || 
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[RemAttr]: constituent value is bad (ERROR or NULL CONSTITUENT)" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* second check the attribute */
    if( attribute < MIN_ATTR || attribute > MAX_ATTR ){
	rval = ERROR_INT;
	TKError = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[RemAttr]: attribute value out of range - got" );
	dummy = TKERR_FormatInt( "%d", attribute );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = HasAttr( constituent, attribute );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == NOATTR_err ){
	rval = ERROR_INT;
	TKError = NOATTR_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[RemAttr]: the constituent does not have the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheAttribute = ATTR_FindAndRemoveAttr( constituent, attribute );
    if( TheAttribute == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[RemAttr]: constituent has the attribute but I can't find and remove it" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* now free the space associated with the attribute value */

    TypeIndex = ATTR_IndexIntoTypes[attribute];

    TheValue = (*ATTR_AttrTypeTbl[TypeIndex].get)(TheAttribute);
    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].free)(TheValue);
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[RemAttr]: can not free the space for the attribute value" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* now free the attribute structure itself */
    SlaveRval = MM_Free( (POINTER_type) TheAttribute );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[RemAttr]: can not free the space for the attribute structure" );
	dummy = TKERR_EndErrMsg();
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



INT_type SetAttr(constituent, attribute, value, mask)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN POINTER_type	value;
    IN PARM_MASK_type	mask;
{

    /*
	Strategy:
	    1) check the arguements:
		a) constituent is checked for obvious errors
		b) attribute is checked to ensure that it is 
		    in range and currently implemented
		c) value is checked by lower level routines
		    specific to each attribute type in aunion.h
		d) mask is check by lower level routines
		    specific to each attribute

	    2) allocate an attribute structure and the attribute value
	
	    3) load up the common fields in the attribute structure

	    4) set attribute specific fields in the attribute
		structure

	    5) add the attribute to the constituent and set
		the which_attributes field in the constituent

	    6) if the attribute is at_SUBORDINATES then thread
		the tree

    */

    INT_type	rval;	    /* the returned value */

    INT_type	TypeIndex;  /* index into the types table */
    INT_type	SlaveRval;  /* outcome of the type specific subrs */
    INT_type    dummy;	    /* ignored return */

    POINTER_type TheStorage; /* for the attribute value */

    POINTER_type	p;
	/* temp ptr for allocating space for the attribute */
    ATTR_attribute_t	AttrStructPtr; 
	/* ptr to the attr struct */

    PARM_MASK_type dest_mask;

    PARM_MASK_type CleanMask;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* First check the attribute parameter (for better msgs later) */
    if( attribute < MIN_ATTR || attribute > MAX_ATTR ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[SetAttr]: attribute out of range (%d)",
			     attribute);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check the constituent */
    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SetAttr]: constituent parameter ERROR or NULL");
	dummy = TKERR_Format(" setting ");
	dummy = TKERR_Format (GetAttrName(attribute) );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }


    SlaveRval = ATTR_CheckAndCleanMask( attribute, mask, 
		    &CleanMask );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
 
    TypeIndex = ATTR_IndexIntoTypes[attribute];

    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].check_set_attr_value)
		    (constituent, attribute, value, CleanMask);
    /* NOTE: that right now we are lax about checking */
    if(SlaveRval == ERROR_INT && TKError != UNIMPL_err ){
	/* ASSERT: check routine sets TKError */
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Second allocate space of the attribute value */
    TheStorage = (*ATTR_AttrTypeTbl[TypeIndex].allocate)(value);
    if( TheStorage == ERROR_POINTER ){
	/* ASSERT: TKError was set in the allocate routine */
	/* N.B. With the check 'disabled' as above this is */
	/* where we catch unimplemented attributes */
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Second allocate space for the attribute structure */
    p = MM_Malloc((INT_type) sizeof( struct ATTR_attribute_struct));
    if( p == ERROR_POINTER ){
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)( TheStorage );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[SetAttr]: can't get memory for attribute structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    AttrStructPtr = (ATTR_attribute_t) p;

    /* Third: load the common fields of the attribute */
    AttrStructPtr->attribute_id = attribute;
    AttrStructPtr->parm_mask = CleanMask;

    /* load the attribute_value field */
    SlaveRval =(*ATTR_AttrTypeTbl[TypeIndex].set)(AttrStructPtr, TheStorage);
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError was set in the set routine */
	rval = ERROR_INT;
	dummy = MM_Free( p ); 
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)( TheStorage );
	goto LEAVE;
    }

    /* Fourth: load the attribute specific fields */
    dest_mask = (PARM_MASK_type) 0;
    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].copy)( /*src*/ value, 
		    /* what is there in the source */ CleanMask,
		    /* what we want to copy */ CleanMask,
		    &dest_mask, /* dest */ TheStorage );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: Lower level routine set TKError */
	rval = ERROR_INT;
	dummy = MM_Free( p ); 
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)( TheStorage );
	goto LEAVE;
    }

    /*
	Fifth: Add the attribute to the constituent and
	set the which_attributes field in the constituent.
    */
    if( ATTR_AddToConstituent( constituent, AttrStructPtr )
	== ERROR_INT 
    ){
	/* ASSERT: Lower level routine set TKError */
	/* lower level routine frees storage on error */
	rval = ERROR_INT;
	dummy = MM_Free( p ); 
	dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)( TheStorage );
	goto LEAVE;
    }

    /* thread the tree if needed */
    if( attribute == at_SUBORDINATES ||
	attribute == at_CONTENT_PORTIONS ){
	SlaveRval = ATTR_ThreadTree( constituent, value );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    dummy = MM_Free( p ); 
	    dummy = (*ATTR_AttrTypeTbl[TypeIndex].free)( TheStorage );
	    goto LEAVE;
	}
    }
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type SetIntAttr(constituent, attribute, value )
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN INT_type		value;
{

    INT_type	rval;

    INT_type	TheInt;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheInt = value;

    rval = SetAttr( constituent, attribute, (POINTER_type) &TheInt, 
		/* mask */ ALL_PARMS );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );


}



INT_type SetValueAttr(constituent, attribute, value )
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN VALUE_type	value;
{


    INT_type	rval;	    /* the returned value */

    VALUE_type	TheValue;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheValue = value;

    rval = SetAttr( constituent, attribute, (POINTER_type) &TheValue,
		/* mask */ ALL_PARMS );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type UnspecParms(constituent, attribute, unspec)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
    IN PARM_MASK_type	unspec;	  
{

    /* 
	Strategy:
	    1) first check the constituent to ensure that it is not
		ERROR_CONSTITUENT or NULL_CONSTITUENT
	    2) check the attribute to ensure that it is in bounds
	    3) get a clean mask to tidy up what the user sent me
	    4) call HasAttr to ensure that the constituent has
		the attribute
	    5) if the constituent has the attribute, then find the
		    attribute
	    6) if the find attribute succeeds then store the 
		clean mask in the attribute structure
    */

    INT_type	rval;	    /* the returned value */

    PARM_MASK_type CleanMask;

    INT_type	SlaveRval;

    ATTR_attribute_t TheAttribute;

    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[UnspecParms]: the constituent is ERROR_CONSTITUENT or NULL_CONSTITUENT" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( attribute < MIN_ATTR || attribute > MAX_ATTR ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[UnspecParms]: the attribute value is out of range - got " );
	dummy = TKERR_FormatInt( "%d", attribute );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = ATTR_CheckAndCleanMask( attribute, 
		    /* user's */ unspec,
		    /* my clean mask */ &CleanMask );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = HasAttr( constituent, attribute );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = ERROR_INT;
	TKError = NOATTR_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[UnspecParms]: the constituent does not have the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheAttribute = ATTR_FindAttr( constituent, attribute );
    if( TheAttribute == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[UnspecParms]: constituent has the attribute but I can't find it <weemp!>" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheAttribute->parm_mask = CleanMask;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
    	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_Final()
{

    INT_type	rval;	    /* the returned value */

    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[ATTR_Final]: Not Implemented" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_Init()
{

    INT_type	rval;	    /* the returned value */

#ifndef USE_LONG_NAMES

#define THE_ERROR_ATTRIBUTE gaatt
#define THE_NULL_ATTRIBUTE ganatt

#else

#define THE_ERROR_ATTRIBUTE The_Error_Attribute
#define THE_NULL_ATTRIBUTE The_Null_Attribute

#endif

    static struct ATTR_attribute_struct THE_ERROR_ATTRIBUTE;
    static struct ATTR_attribute_struct THE_NULL_ATTRIBUTE;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* set up the error attribute */
    ERROR_ATTRIBUTE = &THE_ERROR_ATTRIBUTE;
    NULL_ATTRIBUTE = &THE_NULL_ATTRIBUTE;

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
