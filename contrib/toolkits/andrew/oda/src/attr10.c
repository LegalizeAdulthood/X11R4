
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr10.c,v 1.2 89/10/31 12:52:15 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr10.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr10.c,v $
 * Revision 1.2  89/10/31  12:52:15  jr
 * Lots of casting problems.
 * 
 * Revision 1.1  89/09/21  06:16:09  mss
 * Initial revision
 * 
 * Revision 1.22  89/09/12  09:02:20  mss
 * Removed #
 * 
 * Revision 1.21  89/08/02  17:22:50  annm
 * fixed bogus TKERR error msg in STRING_Alloc
 * 
 * Revision 1.20  89/07/19  22:30:19  mss
 * Changed some errors to warnings
 * 
 * Revision 1.19  89/04/21  18:13:47  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.18  89/04/11  15:37:21  annm
 * I can't remember what I did here
 * 
 * Revision 1.17  89/03/09  15:49:30  annm
 * changes for new DAPD stuff
 * 
 * Revision 1.16  89/02/23  20:34:09  annm
 * changes for new DVL stuff
 * 
 * Revision 1.15  89/01/19  14:40:45  annm
 * primping the Check value code - tightened up restrictions on 
 * presentation attributes going on content dependent DVL/DAPD components. 
 * ALso tightened up on presentation attribute placement to account 
 * for the shared versus logical versus layout presentation attributes.
 * 
 * Revision 1.14  89/01/17  16:54:39  annm
 * changes for DVL defaulting
 * 
 * Revision 1.13  89/01/03  17:01:47  annm
 * switch statement missing explicit case for rg_LINE_PROGRESSION - 
 * added this case as an UNIMPL_err.
 * 
 * Revision 1.12  88/12/14  15:03:43  mss
 * Rearranged code so that it would compile on a
 * 1 meg macintosh
 * 
 * Revision 1.11  88/12/13  19:52:44  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.10  88/11/18  13:40:26  annm
 * no change
 * no change
 * 
 * Revision 1.9  88/10/20  16:31:16  annm
 * no change
 * 
 * Revision 1.8  88/10/14  11:54:28  mss
 * Added semantic checking for rg_COMPRESSION
 * 
 * Revision 1.7  88/09/20  09:18:48  annm
 * fixed static char array dlc size to make sun happy
 * 
 * Revision 1.6  88/09/19  16:53:30  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.5  88/09/06  13:06:17  annm
 * changed ""'s in includes to <>
 * 
 * Revision 1.4  88/08/30  15:11:57  annm
 * no change
 * 
 * Revision 1.3  88/08/29  13:33:20  jr
 * rg_PEL_TRANSMISSION_DENSITY -> rg_PEL_TRANS_DENSITY
 * 
 * Revision 1.2  88/08/26  16:26:05  annm
 * 
 * 
 * Revision 1.1  88/08/23  14:01:54  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr10.c,v $";
#endif

/* Put .c template comment here */

/*

    This file contains the canonical procedures fro the following
    aunion.h types;

	STRING_type
	VALUE_type
	INT_PAIR_type
	SEQUENCE_TAB_STOP_type

    At present, only the Copy, Alloc, Set, Get and iso default
    canonical procedures are implemented. The others
    are still in stubs.c
*/

#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR10
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_STR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <rnames.h>
#include <acd.h>
#include <seq.h>
#include <mm.h>
#include <values.h>
#include <str.h>
#include <attrid.h>
#include <tkerr.h>
#include <attr0.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR10
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_STR
#undef TK_TKI_MM
#undef TK_TKI_TKERR


/* put #define's and declarations local to this file here */



INT_type STRING_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    INT_type rval;

    STRING_type Source;

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

    Source = (STRING_type) source;

    if( Source == NULL_STRING ){
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



POINTER_type STRING_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    STRING_type  TheSource;
    STRING_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    TheSource = (STRING_type) value;

    if( TheSource == NULL_STRING ){
	rval = value;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[STRING_Alloc]: source has bad tag of " );
	dummy = TKERR_FormatInt("%d", (INT_type) TheSource->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_CHAR_tag, 
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



INT_type STRING_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

    STRING_type TheString;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheString = (STRING_type) storage;

    if( TheString == NULL_STRING ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = DeleteString( (STRING_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type STRING_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.STRING_field =
	(STRING_type) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type STRING_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (POINTER_type) GET_STRING( AttrStructPtr );
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



INT_type STRING_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type STRING
	    at_OBJECT_IDENTIFIER
	    at_OBJECT_CLASS_IDENTIFIER
	    at_RESOURCES
	    at_LAYOUT_CATEGORY 
	    at_LAYOUT_STYLE_IDENTIFIER
	    at_PRESENTATION_STYLE_ID
	    at_CONTENT_ID_LOGICAL
	    at_CONTENT_ID_LAYOUT
	    at_ALT_REP
	    at_USER_READABLE_COMMENTS
	    at_USER_VISIBLE_NAME
	    dp_TITLE
	    dp_SUBJECT
	    dp_DOCUMENT_TYPE
	    dp_ABSTRACT
	    dp_DOCUMENT_DATE_AND_TIME
	    dp_CREATION_DATE_AND_TIME
	    dp_LOCAL_FILING_DATE_TIME
	    dp_EXPIRY_DATE_AND_TIME
	    dp_START_DATE_AND_TIME
	    dp_PURGE_DATE_AND_TIME
	    dp_RELEASE_DATE_AND_TIME
	    dp_STATUS
	    dp_AUTHORIZATION
	    dp_SECURITY_CLASSIFICATION
	    dp_ACCESS_RIGHTS

	The mask is irrelevent.
    */


    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    STRING_type TheValue;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_OBJECT_IDENTIFIER:
	if( !( CONST_IsSpecific( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a specific object" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_OBJECT_CLASS_IDENTIFIER:
    case at_RESOURCE:
	if( !( CONST_IsGeneric( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on an object class" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	
    
    case at_LAYOUT_CATEGORY:
	if( !( CONST_IsLayoutStyle( TheConstituent )  ||
		CONST_IsLogical( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style or a logical component" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_LAYOUT_STYLE_IDENTIFIER:
	if( !( CONST_IsLayoutStyle( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_PRESENTATION_STYLE_ID:
	if( !( CONST_IsPresentStyle( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_CONTENT_ID_LOGICAL:
    case at_CONTENT_ID_LAYOUT:
    case at_ALT_REP:
	if( !( CONST_IsContent( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a content portion" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_USER_READABLE_COMMENTS:
    case at_USER_VISIBLE_NAME:
	if( !( CONST_IsComponent( TheConstituent )  ||
	       CONST_IsStyle( TheConstituent) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a component or style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case dp_TITLE:
    case dp_SUBJECT:
    case dp_DOCUMENT_TYPE:
    case dp_ABSTRACT:
    case dp_DOCUMENT_DATE_AND_TIME:
    case dp_CREATION_DATE_AND_TIME:
    case dp_LOCAL_FILING_DATE_TIME:
    case dp_EXPIRY_DATE_AND_TIME:
    case dp_START_DATE_AND_TIME:
    case dp_PURGE_DATE_AND_TIME:
    case dp_RELEASE_DATE_AND_TIME:
    case dp_STATUS:
    case dp_AUTHORIZATION:
    case dp_SECURITY_CLASSIFICATION:
    case dp_ACCESS_RIGHTS:
	if( !( CONST_IsDocProf( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[STRING_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have STRING_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* now examie the actual value */

    TheValue = (STRING_type) value;

    if( attribute == at_LAYOUT_CATEGORY &&
	TheValue != NULL_STRING &&
	  TheValue->sequence_tag != SEQUENCE_CHAR_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a STRING_type or NULL_STRING" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else if( attribute != at_LAYOUT_CATEGORY &&
	    TheValue->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[STRING_CkVal]: Value's sequence tag is not SEQUENCE_CHAR_tag " );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    if( attribute == at_OBJECT_IDENTIFIER ||
	attribute == at_OBJECT_CLASS_IDENTIFIER ||
	attribute == at_LAYOUT_STYLE_IDENTIFIER ||
	attribute == at_PRESENTATION_STYLE_ID ||
	attribute == at_CONTENT_ID_LOGICAL || 
	attribute == at_CONTENT_ID_LAYOUT ){

	SlaveRval = ATTR_CheckId( TheValue, attribute );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
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



INT_type ATTR_CheckId( TheId, attribute )
    IN STRING_type  TheId;
    IN INT_type	    attribute;
{

    INT_type rval;

    INT_type i;

    INT_type j;

    static CHAR_type IdChars[12] = {
	" 0123456789"
    };

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    for( i = (INT_type) 0; i < TheId->length; i++ ){
	
	for( j = (INT_type) 0; j < (INT_type) 11; j++ ){
	
	    if( TheId->sequence_value.chars[i] == IdChars[j] ){
		break;
	    }

	}

	if( j == (INT_type) 11){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		    "[ATTR_CheckId]: Bad character in value for attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_EndErrMsg();
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



INT_type STRING_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type STRING that do NOT default:
	    at_OBJECT_IDENTIFIER
	    at_OBJECT_CLASS_IDENTIFIER
	    at_RESOURCES
	    at_LAYOUT_STYLE_IDENTIFIER
	    at_PRESENTATION_STYLE_ID
	    at_CONTENT_ID_LOGICAL
	    at_CONTENT_ID_LAYOUT
	    at_ALT_REP
	    dp_TITLE
	    dp_SUBJECT
	    dp_DOCUMENT_TYPE
	    dp_ABSTRACT
	    dp_DOCUMENT_DATE_AND_TIME
	    dp_CREATION_DATE_AND_TIME
	    dp_LOCAL_FILING_DATE_TIME
	    dp_EXPIRY_DATE_AND_TIME
	    dp_START_DATE_AND_TIME
	    dp_PURGE_DATE_AND_TIME
	    dp_RELEASE_DATE_AND_TIME
	    dp_STATUS
	    dp_AUTHORIZATION
	    ap_SECURITY_CLASSIFICATION
	    dp_ACCESS_RIGHTS

	Attributes with type STRING that DO default:
	    at_USER_READABLE_COMMENTS
	    at_USER_VISIBLE_NAME
	    at_LAYOUT_CATEGORY (objects only - obj classes and styles
		must be filtered out)

	The masks are irrelevent.
    */
				       
    INT_type rval;

    STRING_type TheString;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    /* default value of NULL_STRING */
    case at_LAYOUT_CATEGORY:
	if( *destination == (POINTER_type) 0 ){
	    *destination = (POINTER_type) NULL_STRING;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[STRING_Deflt]: have space and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    /* default value of zero length string */
    case at_USER_READABLE_COMMENTS:
    case at_USER_VISIBLE_NAME:
	if( *destination == (POINTER_type) 0 ){
	    TheString = MakeString( (INT_type) 0 );
	    if( TheString == ERROR_STRING ){
		/* ASSERT: TKError got set */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    *destination = (POINTER_type) TheString;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[STRING_Deflt]: Can not get memory for a STRING_type" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[STRING_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have STRING_type" );
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



INT_type VALUE_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    INT_type rval;

    /*
	The source mask is ignored for value.
	
	The source is the address of the VALUE_type value.

	The destination is the address to store in.
    */

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    * (VALUE_type *) destination = * (VALUE_type *) source;

    *destination_mask = source_mask;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type VALUE_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;
    
    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = MM_Malloc( (INT_type) sizeof( VALUE_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[VALUE_Alloc]: can not get memory for a VALUE_type" );
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



INT_type VALUE_Free(storage)
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



INT_type VALUE_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.VALUE_field = (VALUE_type *) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}



POINTER_type VALUE_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = (POINTER_type) AttrStructPtr->attribute_value.VALUE_field;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );
}




static INT_type VALUE_CkValConstituent(TheConstituent, attribute)
	INTERNAL_CONSTITUENT TheConstituent;
	INT_type attribute;
{

    /*
	Attributes with type VALUE_type:
	    at_CONTENT_TYPE
	    at_LAYOUT_PATH
	    at_TRANSPARENCY
	    at_COLOUR
	    at_PROTECTION
    	    at_BLK_ALGNM
	    at_CONCATENATION
	    at_FILL_ORDER
	    at_OBJECT_TYPE 
	    dp_GENERIC_LAY_STRUCT
	    dp_SPECIFIC_LAY_STRUCT
	    dp_GENERIC_LOG_STRUCT
	    dp_SPECIFIC_LOG_STRUCT
	    dp_LAYOUT_STYLES
	    dp_PRESENTATION_STYLES
	    dp_INTERCHANGE_FORMAT_CLASS
	    cc_ALIGNMENT
	    cc_CHAR_ORIENTATION
	    cc_CHARACTER_PATH
	    cc_LINE_PROGRESSION
	    cc_PAIR_WISE_KERNING
	    cc_FORMATTING_INDICATOR
            rg_COMPRESSION
	    rg_LINE_PROGRESSION
	    rg_PEL_PATH
	    rg_PEL_TRANS_DENSITY

	The mask is irrelevent.
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

    case at_CONTENT_TYPE:
	if( !( 
		CONST_IsBasic( TheConstituent ) &&
		CONST_IsLayout( TheConstituent) &&
		CONST_IsDVLBlock( TheConstituent )
	    ) 
	){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[Value_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a basic layout object" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_LAYOUT_PATH:
	if( !( CONST_IsFrame( TheConstituent ) ||
		CONST_IsDVLFrame( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a frame" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	
    
    case at_TRANSPARENCY:
    case at_COLOUR:
	if( !( CONST_IsPage(TheConstituent ) ||
		CONST_IsFrame(TheConstituent) ||
		CONST_IsBlock( TheConstituent )  ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLPage( TheConstituent ) ||
		CONST_IsDVLFrame( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a page, frame, block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_PROTECTION:
	if( !( CONST_IsLogical( TheConstituent )  ||
		CONST_IsDVLLogical( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a logical component" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_BLK_ALGNM:
    case at_CONCATENATION:
    case at_FILL_ORDER:
	if( !( CONST_IsLayoutStyle( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case at_OBJECT_TYPE:
	if( !( CONST_IsComponent( TheConstituent )  ||
		CONST_IsDVLBlock( TheConstituent) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a component" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case dp_GENERIC_LAY_STRUCT:
    case dp_SPECIFIC_LAY_STRUCT:
    case dp_GENERIC_LOG_STRUCT:
    case dp_SPECIFIC_LOG_STRUCT:
    case dp_LAYOUT_STYLES:
    case dp_PRESENTATION_STYLES:
    case dp_INTERCHANGE_FMT_CLASS:
	if( !( CONST_IsDocProf( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case cc_CHAR_ORIENTATION:
    case cc_CHARACTER_PATH:
    case cc_LINE_PROGRESSION:
    case cc_PAIRWISE_KERNING:
    case cc_ALIGNMENT:
	if( !( CONST_IsBlock( TheConstituent )  ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case rg_LINE_PROGRESSION:
    case rg_PEL_PATH:
    case rg_PEL_TRANS_DENSITY:
	if( !( CONST_IsBlock( TheConstituent )  ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case cc_FORMATTING_INDICATOR:
	if( !( CONST_IsBlock( TheConstituent )  ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;	

    case rg_COMPRESSION:
	if( !( CONST_IsRasterContent( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent )  ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
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
	dummy = TKERR_Format( "[VALUE_CkValConstituent]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have VALUE_type" );
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


static INT_type VALUE_CkValATValue(attribute, TheValue)
	INT_type attribute;
        VALUE_type TheValue;
{

    /*
	AT Attributes with type VALUE_type:
	    at_CONTENT_TYPE
	    at_LAYOUT_PATH
	    at_TRANSPARENCY
	    at_COLOUR
	    at_PROTECTION
    	    at_BLK_ALGNM
	    at_CONCATENATION
	    at_FILL_ORDER
	    at_OBJECT_TYPE 

	The mask is irrelevent.
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

    case at_CONTENT_TYPE:
	if( TheValue != at_CONTENT_TYPE_form_rgca ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		"is %d and must be at_CONTENT_TYPE_form_rgca", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_LAYOUT_PATH:
	if( TheValue != at_LAYOUT_PATH_type_0 &&
	    TheValue != at_LAYOUT_PATH_type_90 &&
	    TheValue != at_LAYOUT_PATH_type_180 &&
	    TheValue != at_LAYOUT_PATH_type_270 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_TRANSPARENCY:
	if( TheValue != at_TRANSPARENCY_transparent &&
	    TheValue != at_TRANSPARENCY_opaque ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_COLOUR:
	if( TheValue != at_COLOUR_colourless &&
	    TheValue != at_COLOUR_white ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_PROTECTION:
	if( TheValue != at_PROTECTION_protected &&
	    TheValue != at_PROTECTION_unprotected ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_BLK_ALGNM:
	if( TheValue != at_BLK_ALGNM_right_hand_aligned &&
	    TheValue != at_BLK_ALGNM_left_hand_aligned &&
	    TheValue != at_BLK_ALGNM_centred &&
	    TheValue != at_BLK_ALGNM_null ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_CONCATENATION:
	if( TheValue != at_CONCATENATION_concatenated &&
	    TheValue != at_CONCATENATION_non_concated ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_FILL_ORDER:
	if( TheValue != at_FILL_ORDER_normal_order &&
	    TheValue != at_FILL_ORDER_reverse_order ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case at_OBJECT_TYPE:
	if( TheValue != at_OBJECT_TYPE_doc_layout_root &&
	    TheValue != at_OBJECT_TYPE_page_set &&
	    TheValue != at_OBJECT_TYPE_page &&
	    TheValue != at_OBJECT_TYPE_frame &&
	    TheValue != at_OBJECT_TYPE_block &&
	    TheValue != at_OBJECT_TYPE_doc_logical_root &&
	    TheValue != at_OBJECT_TYPE_comp_logical_obj &&
	    TheValue != at_OBJECT_TYPE_bas_logical_obj ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValATValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;


    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[VALUE_CkValATValue: Value checking of attribute ");
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" is not implemented");
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


static INT_type VALUE_CkValCCValue(attribute, TheValue)
	INT_type attribute;
        VALUE_type TheValue;
{

    /*
	CC Attributes with type VALUE_type:
	    cc_ALIGNMENT
	    cc_CHAR_ORIENTATION
	    cc_CHARACTER_PATH
	    cc_LINE_PROGRESSION
	    cc_PAIR_WISE_KERNING
	    cc_FORMATTING_INDICATOR

	The mask is irrelevent.
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


    case cc_ALIGNMENT:
	if( TheValue != cc_ALIGNMENT_start_aligned &&
	    TheValue != cc_ALIGNMENT_end_aligned &&
	    TheValue != cc_ALIGNMENT_centred &&
	    TheValue != cc_ALIGNMENT_justified ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValCCValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", (INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case cc_CHAR_ORIENTATION:
	if( TheValue != cc_CHAR_ORIENTATION_0 &&
	    TheValue != cc_CHAR_ORIENTATION_90 &&
	    TheValue != cc_CHAR_ORIENTATION_180 &&
	    TheValue != cc_CHAR_ORIENTATION_270 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValCCValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case cc_CHARACTER_PATH:
	if( TheValue != cc_CHARACTER_PATH_0 &&
	    TheValue != cc_CHARACTER_PATH_90 &&
	    TheValue != cc_CHARACTER_PATH_180 &&
	    TheValue != cc_CHARACTER_PATH_270 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValCCValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case cc_LINE_PROGRESSION:
	if( TheValue != cc_LINE_PROGRESSION_90 &&
	    TheValue != cc_LINE_PROGRESSION_270 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValCCValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case cc_PAIRWISE_KERNING:
	if( TheValue != cc_PAIRWISE_KERNING_yes &&
	    TheValue != cc_PAIRWISE_KERNING_no ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValCCValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case cc_FORMATTING_INDICATOR:
	if( TheValue != cc_FORMATTING_INDICATOR_yes &&
	    TheValue != cc_FORMATTING_INDICATOR_no ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValCCValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[VALUE_CkValCCValue: Value checking of attribute ");
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" is not implemented");
	dummy = TKERR_EndWarnMsg();
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


static INT_type VALUE_CkValDPValue(attribute, TheValue)
	INT_type attribute;
        VALUE_type TheValue;
{

    /*
	DP Attributes with type VALUE_type:
	    dp_GENERIC_LAY_STRUCT
	    dp_SPECIFIC_LAY_STRUCT
	    dp_GENERIC_LOG_STRUCT
	    dp_SPECIFIC_LOG_STRUCT
	    dp_LAYOUT_STYLES
	    dp_PRESENTATION_STYLES
	    dp_INTERCHANGE_FORMAT_CLASS

	The mask is irrelevent.
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


    case dp_GENERIC_LAY_STRUCT:
	if( TheValue != dp_GENERIC_LAY_STRUCT_fact_set &&
	    TheValue != dp_GENERIC_LAY_STRUCT_part_gen &&
	    TheValue != dp_GENERIC_LAY_STRUCT_comp_gen ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case dp_SPECIFIC_LAY_STRUCT:
	if( TheValue != dp_SPECIFIC_LAY_STRUCT_present ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case dp_GENERIC_LOG_STRUCT:
	if( TheValue != dp_GENERIC_LOG_STRUCT_fact_set &&
	    TheValue != dp_GENERIC_LOG_STRUCT_part_gen &&
	    TheValue != dp_GENERIC_LOG_STRUCT_comp_gen ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		"is %d is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case dp_SPECIFIC_LOG_STRUCT:
	if( TheValue != dp_SPECIFIC_LOG_STRUCT_present ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case dp_LAYOUT_STYLES:
	if( TheValue != dp_LAYOUT_STYLES_present ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case dp_PRESENTATION_STYLES:
	if( TheValue != dp_PRESENTATION_STYLES_present ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		"is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case dp_INTERCHANGE_FMT_CLASS:
	if( TheValue != dp_INTERCHANGE_FMT_CLASS_a &&
	    TheValue != dp_INTERCHANGE_FMT_CLASS_b ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValDPValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[VALUE_CkValDPValue: Value checking of attribute ");
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" is not implemented");
	dummy = TKERR_EndWarnMsg();
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


static INT_type VALUE_CkValRGValue(attribute, TheValue)
	INT_type attribute;
        VALUE_type TheValue;
{

    /*
	RG Attributes with type VALUE_type:
            rg_COMPRESSION
	    rg_LINE_PROGRESSION
	    rg_PEL_PATH
	    rg_PEL_TRANS_DENSITY

	The mask is irrelevent.
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

    case rg_COMPRESSION:
	if( TheValue != rg_COMPRESSION_compressed &&
	    TheValue != rg_COMPRESSION_uncompressed ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValRGValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
				    " is %d which is illegal", 
				    (INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case rg_LINE_PROGRESSION:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[VALUE_CkValRGValue]: Checking for attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " is unimplemented" );
	dummy = TKERR_EndWarnMsg();
	break;    

    case rg_PEL_PATH:
	if( TheValue != rg_PEL_PATH_0 &&
	    TheValue != rg_PEL_PATH_90 &&
	    TheValue != rg_PEL_PATH_180 &&
	    TheValue != rg_PEL_PATH_270 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValRGValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case rg_PEL_TRANS_DENSITY:
	if( TheValue != rg_PEL_TRANS_DENSITY_1 &&
	    TheValue != rg_PEL_TRANS_DENSITY_2 &&
	    TheValue != rg_PEL_TRANS_DENSITY_3 &&
	    TheValue != rg_PEL_TRANS_DENSITY_4 &&
	    TheValue != rg_PEL_TRANS_DENSITY_5 &&
	    TheValue != rg_PEL_TRANS_DENSITY_6 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[VALUE_CkValRGValue]: Value of attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_FormatInt( 
		" is %d which is illegal", 
		(INT_type) TheValue );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[VALUE_CkValRGValue]: Value checking of attribute ");
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" is not implemented");
	dummy = TKERR_EndWarnMsg();
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



static INT_type VALUE_CkValValue(attribute, TheValue)
	INT_type attribute;
        VALUE_type TheValue;
{

    /*
	Attributes with type VALUE_type:
	    at_CONTENT_TYPE
	    at_LAYOUT_PATH
	    at_TRANSPARENCY
	    at_COLOUR
	    at_PROTECTION
    	    at_BLK_ALGNM
	    at_CONCATENATION
	    at_FILL_ORDER
	    at_OBJECT_TYPE 
	    dp_GENERIC_LAY_STRUCT
	    dp_SPECIFIC_LAY_STRUCT
	    dp_GENERIC_LOG_STRUCT
	    dp_SPECIFIC_LOG_STRUCT
	    dp_LAYOUT_STYLES
	    dp_PRESENTATION_STYLES
	    dp_INTERCHANGE_FORMAT_CLASS
	    cc_ALIGNMENT
	    cc_CHAR_ORIENTATION
	    cc_CHARACTER_PATH
	    cc_LINE_PROGRESSION
	    cc_PAIRWISE_KERNING
	    cc_FORMATTING_INDICATOR
            rg_COMPRESSION
	    rg_LINE_PROGRESSION
	    rg_PEL_PATH
	    rg_PEL_TRANS_DENSITY

	The mask is irrelevent.
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

    case at_CONTENT_TYPE:
    case at_LAYOUT_PATH:
    case at_TRANSPARENCY:
    case at_COLOUR:
    case at_PROTECTION:
    case at_BLK_ALGNM:
    case at_CONCATENATION:
    case at_FILL_ORDER:
    case at_OBJECT_TYPE:
	rval = VALUE_CkValATValue(attribute, TheValue);
	break;

    case dp_GENERIC_LAY_STRUCT:
    case dp_SPECIFIC_LAY_STRUCT:
    case dp_GENERIC_LOG_STRUCT:
    case dp_SPECIFIC_LOG_STRUCT:
    case dp_LAYOUT_STYLES:
    case dp_PRESENTATION_STYLES:
    case dp_INTERCHANGE_FMT_CLASS:
	rval = VALUE_CkValDPValue(attribute, TheValue);
	break;

    case cc_ALIGNMENT:
    case cc_CHAR_ORIENTATION:
    case cc_CHARACTER_PATH:
    case cc_LINE_PROGRESSION:
    case cc_PAIRWISE_KERNING:
    case cc_FORMATTING_INDICATOR:
	rval = VALUE_CkValCCValue(attribute, TheValue);
	break;

    case rg_COMPRESSION:
    case rg_LINE_PROGRESSION:
    case rg_PEL_PATH:
    case rg_PEL_TRANS_DENSITY:
	rval = VALUE_CkValRGValue(attribute, TheValue);
	break;

    default:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[VALUE_CkValValue: Value checking of attribute ");
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" is not implemented");
	dummy = TKERR_EndWarnMsg();
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type VALUE_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type VALUE_type:
	    at_CONTENT_TYPE
	    at_LAYOUT_PATH
	    at_TRANSPARENCY
	    at_COLOUR
	    at_PROTECTION
    	    at_BLK_ALGNM
	    at_CONCATENATION
	    at_FILL_ORDER
	    at_OBJECT_TYPE 
	    dp_GENERIC_LAY_STRUCT
	    dp_SPECIFIC_LAY_STRUCT
	    dp_GENERIC_LOG_STRUCT
	    dp_SPECIFIC_LOG_STRUCT
	    dp_LAYOUT_STYLES
	    dp_PRESENTATION_STYLES
	    dp_INTERCHANGE_FORMAT_CLASS
	    cc_ALIGNMENT
	    cc_CHAR_ORIENTATION
	    cc_CHARACTER_PATH
	    cc_LINE_PROGRESSION
	    cc_PAIR_WISE_KERNING
	    cc_FORMATTING_INDICATOR
            rg_COMPRESSION
	    rg_LINE_PROGRESSION
	    rg_PEL_PATH
	    rg_PEL_TRANS_DENSITY

	The mask is irrelevent.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    VALUE_type TheValue;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif
    rval = SUCCESS_INT;

    /* Verify that the attribute can appear on this constituent */

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    if (ERROR_INT == VALUE_CkValConstituent(TheConstituent, attribute)){
	/* Error message filled in already, just mark a return error code */
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* now examine the actual value */

    TheValue = * (VALUE_type *) value;

    if (ERROR_INT == VALUE_CkValValue(attribute, TheValue)){
	/* Error message filled in already, just mark a return error code */
	rval = ERROR_INT;
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



INT_type VALUE_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{
	
    /*
	Attributes with type VALUE_type that do NOT default:
	    at_OBJECT_TYPE (UGH)
	    dp_GENERIC_LAY_STRUCT
	    dp_SPECIFIC_LAY_STRUCT
	    dp_GENERIC_LOG_STRUCT
	    dp_SPECIFIC_LOG_STRUCT
	    dp_LAYOUT_STYLES
	    dp_PRESENTATION_STYLES
	    dp_INTERCHANGE_FORMAT_CLASS

    
	Attributes with type VALUE_type that DO default:
	    at_CONTENT_TYPE
	    at_LAYOUT_PATH
	    at_TRANSPARENCY
	    at_COLOUR
	    at_PROTECTION
    	    at_BLK_ALGNM
	    at_CONCATENATION
	    at_FILL_ORDER
	    cc_ALIGNMENT
	    cc_CHAR_ORIENTATION (UGH)
	    cc_CHARACTER_PATH
	    cc_LINE_PROGRESSION
	    cc_PAIR_WISE_KERNING
	    cc_FORMATTING_INDICATOR
 	       rg_COMPRESSION
	    rg_LINE_PROGRESSION
	    rg_PEL_PATH
	    rg_PEL_TRANS_DENSITY

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

    /* first get space */

    switch( attribute ){

    case rg_COMPRESSION:
    case rg_PEL_TRANS_DENSITY:
    case cc_PROP_LINE_SPACING:
    case cc_FORMATTING_INDICATOR:
    case cc_PAIRWISE_KERNING:
    case cc_CHARACTER_PATH:
    case cc_ALIGNMENT:
    case at_FILL_ORDER:
    case at_CONCATENATION:
    case at_BLK_ALGNM:
    case at_PROTECTION:
    case at_COLOUR:
    case at_TRANSPARENCY:
    case at_LAYOUT_PATH:
    case cc_LINE_PROGRESSION:
    case rg_LINE_PROGRESSION:
    case rg_PEL_PATH:
	if( *destination == (POINTER_type) 0 ){
	    *destination = MM_Malloc( (INT_type) sizeof( VALUE_type ) );
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( 
		    "[VALUE_Deflt]: have space and shouldn't" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[VALUE_Deflt]: Have memory and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_CONTENT_TYPE:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format("[VALUE_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " iso 8613 default value is not implemented" );
	dummy = TKERR_EndWarnMsg();
	goto LEAVE;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[VALUE_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have VALUE_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    switch( attribute ){

    /* default value is compressed */
    case rg_COMPRESSION:
	* (VALUE_type *) *destination = rg_COMPRESSION_compressed;
	break;

    /* default value is 6 */
    case rg_PEL_TRANS_DENSITY:
	* (VALUE_type *) *destination = (VALUE_type) 6;
	break;

    case rg_PEL_PATH:
	* (VALUE_type *) *destination = rg_PEL_PATH_0;
	break;
    
    /* default value is no */
    case cc_PROP_LINE_SPACING:
	* (VALUE_type *) *destination = cc_PROP_LINE_SPACING_no;
	break;

    /* default value is no */
    case cc_FORMATTING_INDICATOR:
	* (VALUE_type *) *destination = cc_FORMATTING_INDICATOR_no;
	break;

    /* default value is yes */
    case cc_PAIRWISE_KERNING:
	* (VALUE_type *) *destination = cc_PAIRWISE_KERNING_yes;
	break;
	
    /* default value is (VALUE_type) zero */
    case cc_CHARACTER_PATH:
	* (VALUE_type *) *destination = (VALUE_type) 0;
	break;
	
    /* default value is start aligned */
    case cc_ALIGNMENT:
	* (VALUE_type *) *destination = cc_ALIGNMENT_start_aligned;
	break;
    
    /* default value is normal order */
    case at_FILL_ORDER:
	* (VALUE_type *) *destination = at_FILL_ORDER_normal_order;
	break;
    
    /* default value is non-concatenated */
    case at_CONCATENATION:
	* (VALUE_type *) *destination = at_CONCATENATION_non_concated;
	break;

    /* default value is right hand aligned */
    case at_BLK_ALGNM:
	* (VALUE_type *) *destination = at_BLK_ALGNM_right_hand_aligned;
	break;

    /* default value is unprotected */
    case at_PROTECTION:
	* (VALUE_type *) *destination = at_PROTECTION_unprotected;
	break;

    /* default value is colourless */
    case at_COLOUR:
	* (VALUE_type *) *destination = at_COLOUR_colourless;
	break;
    
    /* default value is transparent */
    case at_TRANSPARENCY:
	* (VALUE_type *) *destination = at_TRANSPARENCY_transparent;
	break;

    /* default if (VALUE_type) 270 degrees */
    case at_LAYOUT_PATH:
    case cc_LINE_PROGRESSION:
    case rg_LINE_PROGRESSION:
	* (VALUE_type *) *destination = (VALUE_type) 270;
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[VALUE_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have VALUE_type" );
	dummy = TKERR_EndErrMsg();
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



INT_type INT_PAIR_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN OUT POINTER_type destination;
{

    INT_type rval;

    INT_PAIR_type *TheSource;
    INT_PAIR_type *TheDestination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (INT_PAIR_type *) source;
    TheDestination = (INT_PAIR_type *) destination;

    TheDestination->first = TheSource->first;
    TheDestination->second = TheSource->second;

    *destination_mask = source_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type INT_PAIR_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( INT_PAIR_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[INT_PAIR_Alloc]: Can not get memory for an int pair type" );
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




INT_type INT_PAIR_Free(storage)
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



INT_type INT_PAIR_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.INT_PAIR_field = 
	(INT_PAIR_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type INT_PAIR_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_INT_PAIR( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type INT_PAIR_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type INT_PAIR_type:
	    at_PAGE_POSITION
	    dp_UNIT_SCALING
	    cc_KERNING_OFFSET
	    cc_INITIAL_OFFSET
	    rg_INITIAL_OFFSET
	    rg_SPACING_RATIO

	The masks are irrelevent.
    */


    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_PAIR_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_PAGE_POSITION:
	if( !( CONST_IsPage( TheConstituent ) ||
		CONST_IsDVLPage( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a page" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case dp_UNIT_SCALING:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_KERNING_OFFSET:
	if( !( CONST_IsBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a basic or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case rg_SPACING_RATIO:
	if( !( CONST_IsPresentStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a basic or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_INITIAL_OFFSET:
	if( !( CONST_IsBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a basic or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case rg_INITIAL_OFFSET:
	if( !( CONST_IsBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a basic or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
	
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[INT_PAIR_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have INT_PAIR_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheValue = (INT_PAIR_type *) value;

    switch( attribute ){

    case dp_UNIT_SCALING:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " - can not check value" );
	dummy = TKERR_EndWarnMsg();
	goto LEAVE;

    case cc_KERNING_OFFSET:
    case cc_INITIAL_OFFSET:
	if( TheValue->first < (INT_type) 0 ||
	    TheValue->second < (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " both ints in the int pair must be non negative" );
	    dummy = TKERR_FormatInt( " - got first = %d ", TheValue->first );

	    dummy = TKERR_FormatInt( "and second = %d\n", TheValue->second );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case rg_INITIAL_OFFSET:
	/* any ints are legal */
	break;

    case rg_SPACING_RATIO:
	if( TheValue->first <= (INT_type) 0 ||
	    TheValue->second <= (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[INT_PAIR_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " both ints in the int pair must be non negative" );
	    dummy = TKERR_FormatInt( " - got first = %d ", TheValue->first );
	    dummy = TKERR_FormatInt( "and second = %d\n", TheValue->second );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
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



INT_type INT_PAIR_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN OUT POINTER_type *destination;
{

    INT_type rval;

    INT_PAIR_type *TheIntPair;

    INT_type dummy;

    /*
	Attributes with type INT_PAIR_type that do NOT default:
	    dp_UNIT_SCALING

	Attributes with type INT_PAIR_type that DO default:
	    cc_KERNING_OFFSET
	    cc_INITIAL_OFFSET
	    rg_INITIAL_OFFSET
	    rg_SPACING_RATIO

	The masks are irrelevent.
    */


#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    /* first get space */
    switch( attribute ){
    
    case cc_KERNING_OFFSET:
    case rg_SPACING_RATIO:
	if( *destination == (POINTER_type) 0 ){
	    *destination = MM_Malloc( (INT_type) sizeof( INT_PAIR_type ) );
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[INT_PAIR_Deflt]: have space and shouldn't " );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
    	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[INT_PAIR_Deflt]: have space and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case cc_INITIAL_OFFSET:
    case rg_INITIAL_OFFSET:
	rval = ERROR_INT;
	TKError = UNIMPL_err;
    	dummy = TKERR_StartWarnMsg();
	dummy = TKERR_Format( "[INT_PAIR_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " iso 8613 default is not implemented" );
	dummy = TKERR_EndWarnMsg();
	goto LEAVE;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
    	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[INT_PAIR_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have INT_PAIR_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* now load up the default value */

    TheIntPair = (INT_PAIR_type *) *destination;

    switch( attribute ){
    
    case cc_KERNING_OFFSET:
	TheIntPair->first = (INT_type) 0;
	TheIntPair->second = (INT_type) 0;
	break;

    case rg_SPACING_RATIO:
	TheIntPair->first = (INT_type) 1;
	TheIntPair->second = (INT_type) 1;
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



INT_type SEQUENCE_TAB_STOP_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    SEQUENCE_type Source;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

        /*
	The source mask is ignored.

	The source is a sequence type.
    
	The destination is a sequence type.
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
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_TAB_STOP_Alloc(value)
	IN POINTER_type value;
{

    /*
	Do a shallow allocate as the TAB STOP copy routine
	will use SEQ_CopySequenceElements that does an
	allocate as it copies.
    */

    POINTER_type rval;

    SEQUENCE_TAB_STOP_type TheSequence;
    SEQUENCE_TAB_STOP_type TheSource;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSource = (SEQUENCE_TAB_STOP_type) value;

    if( TheSource == NULL_SEQUENCE ){
	rval = value;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_TAB_STOP_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[SEQUENCE_TAB_STOP_Alloc]: Source is not a sequence of tab stops" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_TAB_STOP_tag, 
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	/* ASSERT: TKError got set */
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_TAB_STOP_Free(storage)
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

    rval = DeleteSequenceDeeply( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_FreeTabStop( TheTabStop )
    IN TAB_STOP_type *TheTabStop;
{

    INT_type rval;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SlaveRval = STRING_Free( (POINTER_type) TheTabStop->tab_reference );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeTabStop]: can not free the tab reference" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = STRING_Free( (POINTER_type) TheTabStop->alignment_string );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeTabStop]: can not free the alignment string" );
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



INT_type SEQUENCE_TAB_STOP_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_TAB_STOP_field =
	(SEQUENCE_TAB_STOP_type) TheStorage;


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_TAB_STOP_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_TAB_STOP( AttrStructPtr );
  

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_TAB_STOP_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE_TAB_STOP_type:
	    cc_LINE_LAYOUT_TABLE

	The masks are irrelevent.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheValue;

    INT_type dummy;

    INT_type i;
    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case cc_LINE_LAYOUT_TABLE:
	if( !( CONST_IsBlock( TheConstituent ) || 
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[SEQUENCE_TAB_STOP_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_TAB_STOP_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have sequence tab stop type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (SEQUENCE_type) value;

    if( attribute == cc_LINE_LAYOUT_TABLE &&
	TheValue != NULL_SEQUENCE &&
	TheValue->sequence_tag != SEQUENCE_TAB_STOP_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_TAB_STOP_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of tab stops or NULL_SEQUENCE" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else if( attribute == cc_LINE_LAYOUT_TABLE &&
    	     TheValue == NULL_SEQUENCE ){
	     rval = SUCCESS_INT;
	     goto LEAVE;    
    }

    switch( attribute ){
    
    case cc_LINE_LAYOUT_TABLE:

	for( i = 0; i < TheValue->length; i ++ ){
	    SlaveRval = ATTR_CheckTabStop( 
			&TheValue->sequence_value.tab_stops[i]);
	    if( SlaveRval == ERROR_INT ){
		/* ASSERT: TKError got set */
		rval = ERROR_INT;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt(
		    "[SEQUENCE_TAB_STOP_CkVal]: Error above was in tab stop %d", i );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	
	    }
	}
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



INT_type ATTR_CheckTabStop( TheTabStop )
    IN TAB_STOP_type *TheTabStop;
{

    INT_type rval;

    INT_type dummy;

    INT_type i;
    INT_type j;

    static CHAR_type Digits[11] = {
	"0123456789"
    };

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( TheTabStop->tab_reference->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[ATTR_CheckTabStop]: Tab stop value whose tab reference is not a sequence of chars" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( TheTabStop->tab_reference->length < (INT_type) 1 ||
	TheTabStop->tab_reference->length > (INT_type) 4 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( 
	    "[ATTR_CheckTabStop]: Tab stop value whose tab reference string have bad length of %d", TheTabStop->tab_reference->length );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheTabStop->tab_reference->length; i++ ){
	
	for( j = (INT_type) 0; j < (INT_type) 10; j ++ ){

	    if( TheTabStop->tab_reference->sequence_value.chars[i] ==
		Digits[j] ){
		break;
	    }

	}

	if( j == 10 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
	    "[ATTR_CheckTabStop]: Tab stop value whose tab reference string is bad" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

    }

    if( TheTabStop->tab_position < (INT_type) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( 
	    "[ATTR_CheckTabStop]: Tab stop value whose tab position isM negative:  %d", TheTabStop->tab_position );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( TheTabStop->alignment != cc_LINE_LAYOUT_TABLE_start_alnd &&
	TheTabStop->alignment != cc_LINE_LAYOUT_TABLE_end_alnd &&
	TheTabStop->alignment != cc_LINE_LAYOUT_TABLE_centred &&
	TheTabStop->alignment != cc_LINE_LAYOUT_TABLE_alnd_arnd ){
    	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( 
	    "[ATTR_CheckTabStop]: Tab stop value whose alignment is illegal: %d", 
	    (INT_type) TheTabStop->alignment );
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


INT_type SEQUENCE_TAB_STOP_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE_TAB_STOP_type that do NOT default:


	Attributes with type SEQUENCE_TAB_STOP_type that DO default:
	    cc_LINE_LAYOUT_TABLE

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

    case cc_LINE_LAYOUT_TABLE:
	if( *destination == (POINTER_type) 0 ){
	    *destination = (POINTER_type) NULL_SEQUENCE;
	    *destination_mask = wanted_mask;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_TAB_STOP_Deflt]: have space and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
	
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[SEQUENCE_TAB_STOP_Deflt]: Attribute ");
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
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
