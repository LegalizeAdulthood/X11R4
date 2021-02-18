

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr3.c,v 1.3 89/10/31 13:36:34 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr3.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr3.c,v $
 * Revision 1.3  89/10/31  13:36:34  annm
 * fixing lint bugs
 * 
 * Revision 1.2  89/10/16  13:06:28  annm
 * fixed bad TKERR macro calls 
 * 
 * Revision 1.1  89/09/21  06:22:31  mss
 * Initial revision
 * 
 * Revision 1.47  89/09/12  09:09:51  mss
 * Removed #
 * 
 * Revision 1.46  89/06/29  14:46:47  annm
 * added checking to CONSTITUENT_CkVal to check for the obvious wrong values (e.g. ERROR_CONSTITUENT, NULL_CONSTITUENT, (CONSTITUENT) 0 and (CONSTITUENT) ERROR_POINTER )
 * 
 * Revision 1.45  89/03/16  15:29:25  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.44  89/03/09  15:52:21  annm
 * changes for the new DAPD stuff
 * 
 * Revision 1.43  89/02/23  20:38:47  annm
 * fixes for the new DVL stuff
 * 
 * Revision 1.42  89/01/30  18:25:40  annm
 * no change
 * 
 * Revision 1.41  89/01/30  13:21:23  annm
 * n
 * cast out stubs for SEQUENCE_NAME_ORG_type
 * 
 * Revision 1.40  89/01/26  12:40:01  annm
 * added goto LEAVE in concluding if - in interest of future modification
 * 
 * Revision 1.39  89/01/20  13:28:02  annm
 * added iso 8613 default for at_CONTENT_ARCH_CLASS and fixed typo
 * 
 * Revision 1.38  89/01/19  14:53:30  annm
 * primping the Check value code - tightened up restrictions on presentation attributes going on content dependent DVL/DAPD components. 
 * ALso tightened up on presentation attribute placement to account for the shared versus logical versus layout presentation attributes.
 * 
 * Revision 1.37  89/01/17  16:56:09  annm
 * changes for DVL defaulting
 * 
 * Revision 1.36  88/12/13  19:54:49  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.35  88/10/20  16:31:38  annm
 * 
 * 
 * Revision 1.34  88/10/14  10:35:53  mss
 * 1st => 1ST
 * 
 * Revision 1.33  88/10/14  09:53:34  jr
 * Fix checking for rg coding attributes.
 * 
 * Revision 1.32  88/09/21  15:46:17  annm
 * added tkerr stuff
 * 
 * Revision 1.31  88/09/19  16:54:43  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.30  88/09/06  13:09:13  annm
 * no change
 * 
 * Revision 1.29  88/09/06  12:01:20  annm
 * no change
 * 
 * Revision 1.28  88/08/29  13:16:02  jr
 * rg_NUMBER_OF_DISCARDED_PELS -> rg_NUMBER_DISCARDED_PELS
 * 
 * Revision 1.27  88/08/22  19:43:12  annm
 * moved canonical procedures for STRING_type and VALUE_type to the new attr10
 * 
 * Revision 1.26  88/08/15  18:16:01  annm
 * fidging with the iso default routines to get the blasted
 * parm mask set righ
 * 
 * Revision 1.25  88/08/10  15:42:18  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults for the currently implemented aunion.h types
 * 
 * Revision 1.24  88/08/03  13:56:09  annm
 * added setting of destination_mask in the copy's to get the return to the user to work right
 * 
 * Revision 1.23  88/08/01  18:45:12  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.22  88/07/19  13:26:06  annm
 * micro tweek for type compatibility with Get of CONSTITUETN
 * 
 * Revision 1.21  88/07/15  18:20:31  annm
 * changes for using canonical procedures in SetAttr
 * 
 * Revision 1.20  88/07/14  17:48:49  annm
 * changes to use canonical procedures for operating on aunion.h types for the SetAttr stuff
 * 
 * Revision 1.19  88/06/17  15:03:55  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.18  88/06/10  16:45:53  annm
 * fix up of treatment of the parm mask
 * 
 * Revision 1.17  88/06/08  16:55:01  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.16  88/06/08  14:34:20  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr3.c,v $";
#endif

/* Put .c template comment here */

/*

    This file contains the canonical procedures
    following aunion.h types:

    Type Name		    

    Constituent		    
    INT_type
    SEQUENCE_BYTE_type
    SEQUENCE_CONSTITUENT_type
    SEQUENCE_INT_type

    At present, only the Copy, Alloc, Set, Get and iso default
    canonical procedures are implemented. The others
    are still in stubs.c

*/



#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR3
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_STR
#define TK_TKI_MM
#define TK_TKI_TKERR
#define TK_TKI_INIT

#include <rnames.h>
#include <acd.h>
#include <seq.h>
#include <mm.h>
#include <parms.h>
#include <values.h>
#include <str.h>
#include <tkerr.h>
#include <init.h>

#include <attr0.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR3
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_STR
#undef TK_TKI_MM
#undef TK_TKI_TKERR
#undef TK_TKI_INIT


/* put #define's and declarations local to this file here */



INT_type CONSTITUENT_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /*
	For constituent the mask is ignored.

	The source is the constituent.

	The destination is the address to store in.
    */

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* just to be safe */
    *destination_mask = source_mask;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type CONSTITUENT_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = value;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type CONSTITUENT_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
b		Entering();
	}
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type CONSTITUENT_Set(AttrStructPtr, TheStorage)
	IN OUT	ATTR_attribute_t AttrStructPtr;
	IN	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif


    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.CONSTITUENT_field = 
	(struct CONSTITUENT_struct *) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type CONSTITUENT_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (CONSTITUENT) GET_CONSTITUENT( AttrStructPtr );

    rval = (POINTER_type) rval;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type CONSTITUENT_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type CONSTITUENT:
	    at_LAYOUT_OBJECT_CLASS 
	    at_OBJECT_CLASS
	    at_PRESENTATION_STYLE
	    at_LOGICAL_SOURCE
	    at_LAYOUT_STYLE
	    at_SYNCHRONIZATION 

	Masks are irrelevent.

    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

    CONSTITUENT ConstituentValue;
    
#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_LAYOUT_OBJECT_CLASS:
    case at_SYNCHRONIZATION:
	if( !( CONST_IsLayoutStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[CONSTITUENT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_OBJECT_CLASS:
	if( !( CONST_IsSpecific( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[CONSTITUENT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a specific object" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_PRESENTATION_STYLE:
	if( !( CONST_IsBasic( TheConstituent ) ||
		CONST_IsDVLBasic( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[CONSTITUENT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a basic" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_LOGICAL_SOURCE:
	if( !( CONST_IsFrameCl( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[CONSTITUENT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a frame object class" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_LAYOUT_STYLE:
	if( !( CONST_IsLogical( TheConstituent ) ||
		CONST_IsDVLLogical( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[CONSTITUENT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go in the logical structure" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[CONSTITUENT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must have CONSTITUENT type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }
    
    /* Value checking possible only WITH the TAGGED structures */

    /* check for the obvious wrong values */

    ConstituentValue = (CONSTITUENT) value;

    if( ConstituentValue == ERROR_CONSTITUENT ||
	ConstituentValue == NULL_CONSTITUENT ||
	ConstituentValue == (CONSTITUENT) 0 ||
	ConstituentValue == (CONSTITUENT) ERROR_POINTER ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTITUENT_CkVal]: obviously bad constituent value of " );
	dummy = TKERR_FormatInt("%d", (INT_type) ConstituentValue );
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



INT_type CONSTITUENT_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type CONSTITUENT that do NOT default:
	    at_OBJECT_CLASS
	    at_PRESENTATION_STYLE
	    at_LOGICAL_SOURCE
	    at_LAYOUT_STYLE
	Attributes with type CONSTITUENT that DO default:
	    at_LAYOUT_OBJECT_CLASS (N.B. will get called only
		    for objects NOT styles, or object classes
	    at_SYNCHRONIZATION (N.B. will get called only
		    for objects NOT styles, or object classes.

	Masks are irrelevent.

    */

    INT_type rval;

    CONSTITUENT *TheConstituent;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    /* NULL_CONSTITUENT return */
    case at_LAYOUT_OBJECT_CLASS:
    case at_SYNCHRONIZATION:
	TheConstituent = (CONSTITUENT *) destination;
	*TheConstituent = (POINTER_type) NULL_CONSTITUENT;	    
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[CONSTITUENT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( "does not have CONTSITUENT type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }    

    *destination_mask = wanted_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type INT_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /*
	The source mask is ignored for int.
	
	The source is the address of the INT_type value.

	The destination is the address to store in.

	ASSERT: wanted_mask is always ALL_PARMS.
    */

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    * (INT_type *) destination = * (INT_type *) source;

    *destination_mask = source_mask;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type INT_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = MM_Malloc( (INT_type) sizeof( INT_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[INT_Alloc]: Can not get memory for and INT_type" );
	dummy = TKERR_EndErrMsg();
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



INT_type INT_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free( storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type INT_Set(AttrStructPtr, TheStorage)
	IN OUT	ATTR_attribute_t AttrStructPtr;
	IN	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.INT_field = (INT_type *) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type INT_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (POINTER_type) AttrStructPtr->attribute_value.INT_field;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

 return( rval );
}



INT_type INT_CkVal( constituent, attribute, value, mask )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    IN POINTER_type value;
    IN PARM_MASK_type mask;
{

    /*
	Attributes with INT_type:

	    dp_NUMBER_OBJECTS_PER_PAGE
	    dp_DOCUMENT_SIZE
	    dp_NUMBER_OF_PAGES
	    cc_CHARACTER_SPACING
	    cc_1ST_LINE_OFFSET
	    cc_GRAPHIC_CHAR_SUBREP
	    cc_LINE_SPACING
	    cc_INDENTATION
	    cc_ORPHAN_SIZE
	    cc_WIDOW_SIZE
	    rg_NUMBER_OF_LINES
	    rg_NUMBER_OF_PELS_PER_LINE
	    rg_NUMBER_DISCARDED_PELS
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;
    
    INT_type dummy;

    INT_type TheValue;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif


    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case dp_NUMBER_OBJECTS_PER_PAGE:
    case dp_DOCUMENT_SIZE:
    case dp_NUMBER_OF_PAGES:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_CHARACTER_SPACING:
    case cc_LINE_SPACING:
	if( !( CONST_IsBlock( TheConstituent ) ||
	       CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_INDENTATION:
    case cc_ORPHAN_SIZE:
    case cc_WIDOW_SIZE:
	if( !( CONST_IsPresentStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;


    case cc_1ST_LINE_OFFSET:
	if( !( CONST_IsBlock( TheConstituent ) ||
	       CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent)  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case rg_NUMBER_OF_LINES:
    case rg_NUMBER_OF_PELS_PER_LINE:
    case rg_NUMBER_DISCARDED_PELS:
	if( !( CONST_IsRasterContent( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a raster content portion" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[INT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have INT_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* now examie the actual value */

    TheValue = * (INT_type *) value;

    /* 
	N.B. cc_1ST_LINE_FORMAT is not tested because it can be any int
	value.
    */

    switch( attribute ){
    
    /* > zero */
    case cc_CHARACTER_SPACING:
    case cc_LINE_SPACING:
    case cc_ORPHAN_SIZE:
    case cc_WIDOW_SIZE:
    case rg_NUMBER_OF_LINES:
	if( TheValue <= (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[INT_CkVal]: INT_type value must be > 0 for " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    /* >= zero */
    case cc_INDENTATION:
    case rg_NUMBER_OF_PELS_PER_LINE:
    case rg_NUMBER_DISCARDED_PELS:
	if( TheValue < (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[INT_CkVal]: INT_type value must be >= 0 for " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    /* currently unimplemented */
    case dp_NUMBER_OBJECTS_PER_PAGE:
    case dp_DOCUMENT_SIZE:
    case dp_NUMBER_OF_PAGES:    
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[INT_CkVal]: can not check document profile attributes" );
	dummy = TKERR_EndErrMsg();
	break;

    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type INT_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type INT_type that DO NOT default:
	    dp_NUMBER_OF_OBJECTS_PER_PAGE
	    dp_DOCUMENT_SIZE
	    dp_NUMBER_OF_PAGES
	    rg_NUMBER_OF_LINES (UGH)
	    rg_NUMBER_OF_PELS_PER_LINE (UGH)
	Attributes with type INT_type that DO default:
	    cc_CHARACTER_SPACING (UGH)
	    cc_1ST_LINE_OFFSET
	    cc_GRAPHIC_CHAR_SUBREP
	    cc_LINE_SPACING (UGH)
	    cc_INDENTATION
	    cc_ORPHAN_SIZE
	    cc_WIDOW_SIZE
	    rg_NUMBER_DISCARDED_PELS (UGH)

	Masks are irrelevent.
    */

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    /* first get space */

    switch( attribute ){

    case cc_1ST_LINE_OFFSET:
    case cc_GRAPHIC_CHAR_SUBREP:
    case cc_INDENTATION:
    case cc_ORPHAN_SIZE:
    case cc_WIDOW_SIZE:
	if( *destination == (POINTER_type) 0){
	    *destination = MM_Malloc( (INT_type) sizeof( INT_type ) );
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( 
		    "[INT_Deftl]: Can't get memory for an INT_type" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[INT_Deflt}: Already have memory and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case rg_NUMBER_DISCARDED_PELS:
    case cc_CHARACTER_SPACING:
    case cc_LINE_SPACING:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[INT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " iso 8613 default currently unimplemented" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[INT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have INT_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    switch( attribute ){

    /* default value of (INT_type) zero */
    case cc_1ST_LINE_OFFSET:
    case cc_GRAPHIC_CHAR_SUBREP:
    case cc_INDENTATION:
	* (INT_type *) *destination = (INT_type) 0;
	break;

    /* default value of (INT_type) one */
    case cc_ORPHAN_SIZE:
    case cc_WIDOW_SIZE:
	* (INT_type *) *destination = (INT_type) 1;
	break;

    }

    *destination_mask = wanted_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_BYTE_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /*
	The source mask is ignored.

	The source is a sequence type.
    
	The destination is a sequence type.

	ASSERT: wanted_mask is ALL_PARMS.
    */

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    if( (SEQUENCE_type) source == NULL_SEQUENCE ){
	*destination_mask = source_mask;
	goto LEAVE;
    }

    rval = SEQ_CopySequenceElements( (SEQUENCE_type) source,
	    (SEQUENCE_type) destination );

    if( rval != ERROR_INT ){
	*destination_mask = source_mask;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type SEQUENCE_BYTE_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSequence;
    SEQUENCE_type TheSource;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    TheSource = (SEQUENCE_type) value;

    if( TheSource == NULL_SEQUENCE ){
	rval = (POINTER_type) NULL_SEQUENCE;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_BYTE_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_BYTE_Alloc]: value isn't SEQUENCE BYTE" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_BYTE_tag,
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type SEQUENCE_BYTE_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if(TraceRoutine()){
	    Leaving();
	}
#endif

    return rval;
}



INT_type SEQUENCE_BYTE_Set(AttrStructPtr, TheStorage)
	IN OUT	ATTR_attribute_t AttrStructPtr;
	IN	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_BYTE_field =
	(SEQUENCE_BYTE_type) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type SEQUENCE_BYTE_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (POINTER_type) GET_SEQUENCE_BYTE( AttrStructPtr );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type SEQUENCE_BYTE_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE BYTE:
	    at_APPLICATION_COMMENTS 
	    at_CONTENT_INFORMATION
	    dp_PROFILE_CHARACTER_SETS
	    dp_COMMENTS_CHARACTER_SETS
	    dp_ALT_REP_CHARACTER_SETS
	    dp_ADDITIONAL_INFORMATION
	    cc_CODE_EXT_ANNOUNCERS 
	    cc_GRAPHIC_CHAR_SETS 

    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {

    case at_APPLICATION_COMMENTS:
	if( !( CONST_IsComponent( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_BYTE_CkVal]: at_APPLICATION_COMMENTS must go on a component" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	break;

    case at_CONTENT_INFORMATION:
	if( !( CONST_IsContent( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_BYTE_CkVal]: at_CONTENT_INFORMATION must go on a content portion" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case dp_PROFILE_CHARACTER_SETS:
    case dp_COMMENTS_CHARACTER_SETS:
    case dp_ALT_REP_CHARACTER_SETS:
    case dp_ADDITIONAL_INFORMATION:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_BYTE_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_CODE_EXT_ANNOUNCERS:
	if( !( CONST_IsBlock( TheConstituent ) ||
	       CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent )  ||
		CONST_IsDAPDBlock( TheConstituent) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_BYTE_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_GRAPHIC_CHARACTER_SETS:    
	if( !( CONST_IsBlock( TheConstituent ) ||
	       CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_BYTE_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_BYTE_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must have SEQUENCE_BYTE_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_BYTE_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BYTE_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of bytes" );
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



INT_type SEQUENCE_BYTE_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE BYTE that do NOT default:
	    at_CONTENT_INFORMATION
	    dp_PROFILE_CHARACTER_SETS
	    dp_COMMENTS_CHARACTER_SETS
	    dp_ALT_REP_CHARACTER_SETS
	    dp_ADDITIONAL_INFORMATION
	
	Attributes with type SEQUENCE BYTE that DO default:
	    at_APPLICATION_COMMENTS (objects only)
	    cc_CODE_EXT_ANNOUNCERS (UGH)
	    cc_GRAPHIC_CHAR_SETS (UGH)

	The masks are irrelevent.
    */

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

   rval = SUCCESS_INT;

    switch( attribute ){

    /* default value is sequence with length zero */
    case at_APPLICATION_COMMENTS:
	if( *destination == (POINTER_type) 0 ){
	    TheSequence = MakeSequence( SEQUENCE_BYTE_tag,
				(INT_type) 0 );
	    if( TheSequence == ERROR_SEQUENCE ){
		/* ASSERT: TKError got set */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    *destination = (POINTER_type) TheSequence;
	    *destination_mask = wanted_mask;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_BYTE_Deflt]: have memory and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case cc_CODE_EXT_ANNOUNCERS:
    case cc_GRAPHIC_CHARACTER_SETS:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_BYTE_Deftl]: " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " iso 8613 default not implemented" );
	dummy = TKERR_EndErrMsg();
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_BYTE_Deftl]: " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have type SEQUENCE BYTE" );
	dummy = TKERR_EndErrMsg();
	break;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_CONSTITUENT_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    INT_type rval;

    SEQUENCE_type Source;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /*
	The source mask is ignored.

	The source is a sequence type.
    
	The destination is a sequence type.

	ASSERT: wanted_mask is ALL_PARMS
    */

    Source = (SEQUENCE_type) source;

    if( Source == NULL_SEQUENCE ){
	rval = SUCCESS_INT;
    }
    else {
	rval = SEQ_CopySequenceElements( (SEQUENCE_type) source,
	    (SEQUENCE_type) destination );
    }

    if( rval != ERROR_INT ){
	*destination_mask = source_mask;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type SEQUENCE_CONSTITUENT_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSource;
    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    TheSource = (SEQUENCE_type) value;

    if( TheSource == NULL_SEQUENCE ){
	rval = value;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_CONSTITUENT_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_CONSTITUENT_Alloc]: " );
	dummy = TKERR_Format( " value doesn't have tag SEQUENCE_CONSTITUENT_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_CONSTITUENT_tag, 
		TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type SEQUENCE_CONSTITUENT_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSequence = (SEQUENCE_type) storage;

    if( TheSequence == NULL_SEQUENCE ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_CONSTITUENT_Set(AttrStructPtr, TheStorage)
	IN OUT	ATTR_attribute_t AttrStructPtr;
	IN	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_CONSTITUENT_field =
	(SEQUENCE_CONSTITUENT_type) TheStorage;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}

POINTER_type SEQUENCE_CONSTITUENT_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (POINTER_type) GET_SEQUENCE_CONSTITUENT( AttrStructPtr );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type SEQUENCE_CONSTITUENT_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE CONSTITUENT:
	    at_BALANCE 
	    at_SUBORDINATES 
	    at_CONTENT_PORTIONS
	    at_IMAGING_ORDER
	    dp_DOC_APP_PROF_DEFAULTS
	    dp_CODING_ATTRIBUTES
	    dp_PRESENTATION_FEATURES

	The masks are irrelevent.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {

    case at_BALANCE:
	/* composite layout components without immed. subord. blocks */
	/* N.B. can't test for subords. */
	if( !( CONST_IsLayout( TheConstituent ) &&
	       CONST_IsComposite( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_CONSTITUENT_CkVal]: at_BALANCE can go only on composite layout components");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_SUBORDINATES:
	/* composite objects */
	if( !( CONST_IsComposite( TheConstituent ) &&
	       CONST_IsSpecific( TheConstituent) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_CONSTITUENT_CkVal]: at_SUBORDINATES must go on a composite object");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_CONTENT_PORTIONS:
	/* basic components */
	if( !( CONST_IsBasic( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_CONSTITUENT_CkVal]: at_CONTENT_PORTIONS must go on a basic component");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_IMAGING_ORDER:
	/* composite page or frame */
	if( !( CONST_IsPage( TheConstituent )  ||
	       CONST_IsFrame( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_CONSTITUENT_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a page or a frame" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    case dp_DOC_APP_PROF_DEFAULTS:
    case dp_CODING_ATTRIBUTES:
    case dp_PRESENTATION_FEATURES:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_CONSTITUENT_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_CONSTITUENT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have SEQUENCE_CONSTITUENT_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( attribute == at_BALANCE &&
	TheSequence != NULL_SEQUENCE &&
	TheSequence->sequence_tag != SEQUENCE_CONSTITUENT_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_CONSTITUENT_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of constituents or NULL_SEQUENCE" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else if( attribute != at_BALANCE &&
	    TheSequence->sequence_tag != SEQUENCE_CONSTITUENT_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_CONSTITUENT_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of constituents" );
	dummy = TKERR_EndErrMsg();
    }   

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_CONSTITUENT_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE CONSTITUENT that do NOT default:
	    at_SUBORDINATES 
	    at_CONTENT_PORTIONS
	    at_IMAGING_ORDER
	    dp_DOC_APP_PROF_DEFAULTS
	    dp_CODING_ATTRIBUTES
	    dp_PRESENTATION_FEATURES

	Attributes with type SEQUENCE CONSTITUENT that DO default:
	    at_BALANCE (objects only - N.B. obj classes get filtered out)

	The masks are irrelevent.
    */

    INT_type rval;

    INT_type dummy;        

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    /* attributes with default NULL_SEQUNECE */
    case at_BALANCE:
	if( *destination == (POINTER_type) 0 ){
	    *destination = (POINTER_type) NULL_SEQUENCE;
	    *destination_mask = wanted_mask;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_CONSTITUENT_Deflt]: " );
	    dummy = TKERR_Format( " have storage and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_CONSTITUENT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " - can't get iso 8613 default for SEQUENCE CONSTITUENT default" );
	dummy = TKERR_EndErrMsg();
	break;
    }


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_INT_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /*
	The source mask is ignored.

	The source is a sequence type.
    
	The destination is a sequence type.
    */

    rval = SEQ_CopySequenceElements( (SEQUENCE_type) source,
	    (SEQUENCE_type) destination );

    if( rval != ERROR_INT ){
	*destination_mask = source_mask;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type SEQUENCE_INT_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSource;
    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    TheSource = (SEQUENCE_type) value;
    if( TheSource->sequence_tag != SEQUENCE_INT_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_INT_Alloc]: " );
	dummy = TKERR_Format( " value doesn't have SEQUENCE_INT_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_INT_tag, 
		TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type SEQUENCE_INT_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_INT_Set(AttrStructPtr, TheStorage)
	IN OUT ATTR_attribute_t AttrStructPtr;
	IN POINTER_type		TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_INT_field =
	(SEQUENCE_INT_type) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type SEQUENCE_INT_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (POINTER_type) GET_SEQUENCE_INT( AttrStructPtr );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type SEQUENCE_INT_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes that have type SEQUENCE INT_type:
	    at_CONTENT_ARCH_CLASS 
	    at_TYPE_OF_CODING 
	    dp_EXTERNAL_DOCUMENT_CLASS
	    dp_DOC_APP_PROFILE
	    cc_GRAPHIC_RENDITION

	The masks are irrelevent.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {

    case at_CONTENT_ARCH_CLASS:
	if( !( CONST_IsBasic( TheConstituent ) ||
		CONST_IsDVLBasic( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_INT_CkVal]: at_CONTENT_ARCH_CLASS must go on a basic component");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_TYPE_OF_CODING:
	/* basic components */
	if( !( CONST_IsContent( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_INT_CkVal]: at_TYPE_OF_CODING must go on a content portion");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    
    case dp_EXTERNAL_DOCUMENT_CLASS:
    case dp_DOC_APP_PROFILE:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_INT_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_GRAPHIC_RENDITION:
	if( !( CONST_IsBlock( TheConstituent) ||
	       CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[SEQUENCE_INT_CkVal]: cc_GRAPHIC_RENDITION must go on a block or a presentation style " );
	dummy = TKERR_EndErrMsg();		    
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_INT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have SEQUENCE_INT_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_INT_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_INT_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of INT_type's" );
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



INT_type SEQUENCE_INT_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes that have type SEQUENCE INT_type that do NOT default:
	    dp_EXTERNAL_DOCUMENT_CLASS
	    dp_DOC_APP_PROFILE
	    
	Attributes that have type SEQUENCE INT_type that DO default:
	    at_CONTENT_ARCH_CLASS 
	    at_TYPE_OF_CODING (UGH)
	    cc_GRAPHIC_RENDITION

	The masks are irrelevent.
    */

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    /* default with one INT_type with value (INT_type) 0 */
    case cc_GRAPHIC_RENDITION:
	if( *destination == (POINTER_type) 0 ){
	    TheSequence = MakeSequence( SEQUENCE_INT_tag, (INT_type) 1);
	    if( TheSequence == ERROR_SEQUENCE ){
		/* ASSERT: TKError got set */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    *destination = (POINTER_type) TheSequence;
	    TheSequence->sequence_value.ints[0] = (INT_type) 0;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_INT_Deflt]: " );
	    dummy = TKERR_Format( " have storage and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_CONTENT_ARCH_CLASS:
	if( *destination == (POINTER_type) 0 ){
	    * (SEQUENCE_type *) destination = at_CONTENT_ARCH_CLASS_form_cc;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_INT_Deflt]: " );
	    dummy = TKERR_Format( " have storage and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    /* unimplemented */
    case at_TYPE_OF_CODING:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_INT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " iso 8613 default unimplemented" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_INT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have type SEQUENCE INT" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    *destination_mask = wanted_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
