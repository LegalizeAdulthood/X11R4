
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr15.c,v 1.3 89/10/31 13:14:50 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr15.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr15.c,v $
 * Revision 1.3  89/10/31  13:14:50  jr
 * Lots of casting problems.
 * 
 * Revision 1.2  89/10/26  14:05:28  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:18:00  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/12  09:05:30  mss
 * Removed #
 * 
 * Revision 1.5  88/12/21  14:39:26  annm
 * fixed bug in check value main loop - missing TheField paramter in call to type specific check value routine 
 * 
 * Revision 1.4  88/11/22  22:52:04  jr
 * Have Font checking code just return success
 * all the time (it was failing on VMS)!!!!
 * 
 * Revision 1.3  88/11/02  16:55:47  annm
 * removed some suprious &'s that sun/vax compiliation found
 * 
 * Revision 1.2  88/11/02  12:54:35  annm
 * removed call to inst2dsclass in ckval for Sequence Ints
 * 
 * Revision 1.1  88/11/01  16:02:35  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr15.c,v $";
#endif

/* Put .c template comment here */

/*

    This file contains the ckval canonical procedures 
    for the SEQUENCE_FONT_PAIR_type aunion.h type.


*/

#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR15
#define TK_IMPL_CONST
#define TK_TKI_TKERR

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <seq.h>
#include <font.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR15
#undef TK_IMPL_CONST
#undef TK_TKI_TKERR


/* put #define's and declarations local to this file here */




INT_type SEQUENCE_FONT_PAIR_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheValue;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    goto LEAVE;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    if( !CONST_IsDocProf(TheConstituent) ){
	rval = ERROR_INT;
	TKError = BADATTR_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[SEQUENCE_FONT_PAIR_CkVal]: dp_FONTS_LIST must go on the document profile" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheValue = (SEQUENCE_FONT_PAIR_type) value;

    if( TheValue->sequence_tag != SEQUENCE_FONT_PAIR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	"[SEQUENCE_FONT_PAIR_CkVal]: bad sequence tag in font pair sequence - ");
	dummy = TKERR_FormatInt("got tag of %d",
				(INT_type) TheValue->sequence_tag);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheValue->length; i++ ){

	SlaveRval = ATTR_FontPairCkVal( &TheValue->sequence_value.font_pairs[i] );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
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



INT_type ATTR_FontPairCkVal(  TheFontPair )
    IN FONT_PAIR_type *TheFontPair;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    /* check the font id  as > 0 to match up with cc_CHARACTER_FONTS */
    if( TheFontPair->font_id <= (INT_type) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_FontPairCkVal]: font id of " );
	dummy = TKERR_FormatInt( "%d is <= 0", TheFontPair->font_id );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check the font reference */
    rval = ATTR_Ck_Fields( &FontReferenceFields[0],
	    &FontReferenceInfo, (POINTER_type) &TheFontPair->font_reference );


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Ck_Fields( fields, info, value )
    IN FIELDS_type *fields;
    IN FONT_BRANCH_INFO_type *info;
    IN POINTER_type value;
{

    INT_type rval; 

    INT_type i;

    POINTER_type ValueBase;

    FIELDS_type *ThisField;    

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    for( i = (INT_type) 0; i < info->table_size; i++ ){

	ValueBase = (*info->GetBase)( value, &fields[i] );

	ThisField = &fields[i];
	rval = (*ThisField->CkVal )( ValueBase, ThisField );
	if( rval == ERROR_INT ){
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



INT_type ATTR_CkVal_Dfnpre()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
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



INT_type ATTR_CkVal_Structured_Name(value, TheField)
    POINTER_type value;
    FIELDS_type *TheField;
{

    INT_type rval;

    STRUCTURED_NAME_type *TheValue;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheValue = (STRUCTURED_NAME_type *) value;

    switch( TheValue->structured_name_tag ){

    case STRUCTURED_NAME_ID_tag:
	SlaveRval = ATTR_CkVal_ISO9541_Id_Type( 
	    &TheValue->structured_name_value.id, TheField);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	break;

    case STRUCTURED_NAME_SEQ_tag:

	for( i = (INT_type) 0;
	     i < TheValue->structured_name_value.idseq->length;
	     i++ ){

	    SlaveRval = ATTR_CkVal_ISO9541_Id_Type( 
		&TheValue->structured_name_value.idseq->sequence_value.iso9541_ids[i], TheField );

	    if( SlaveRval == ERROR_INT ){
		rval = ERROR_INT;
		goto LEAVE;
	    }

	}

	break;

    case STRUCTURED_NAME_UNSPEC_tag:
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Structured_Name]: bad structured name tag" );
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



INT_type ATTR_CkVal_ISO9541_Id_Type( TheValue, TheField )
    IN ISO9541_ID_type *TheValue;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type MonsterValue;

    INT_type i;

    SEQUENCE_type TheSequence;

    BYTE_type *TheByte;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( TheValue->iso9541_id_tag ){

    case ID_INT_tag:

	/*
	    The standard says that the integer must be between 0 and 
	    10^79 -1.

	    Strategy is to check for < 0 first. Then if there
	    are enough bits per integer, calculate the value 
	    of 10^79 -1 and then do the > check. The calculation
	    is done because machines with too few bits can not 
	    represent 10^79 -1. The calculation of this monster
	    value is done iteratively to elminate the need for
	    a system dependent math package.

	    N.B. The value of 10^79-1 requires 241 bits because:

	    2^239 = 883,423,532,389,192,164,791,648,750,371,459,257,913,
		    741,948,437,809,479,060,803,100,646,309,888

	    and

	    2^240 = 1,766,847,064,778,384,329,583,297,500,742,918,515,
		    827,483,896,875,618,958,121,606,201,292,619,776

	    so the magnitude of 10^79 - 1 can be represented in 240 bits but 
	    one more is needed for the sign.
	*/

	if( TheValue->iso9541_id_value.integer < (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_ISO9541_Id_Type]: iso9541 id value (integer) is negative - " );
	    dummy = TKERR_FormatInt("got value of %d ", 
		    TheValue->iso9541_id_value.integer );
	    dummy = TKERR_Format( "for field " );
	    dummy = TKERR_Format( TheField->field_name );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	if( BITS_PER_INT >= (INT_type) 241 ){
	    
	    MonsterValue = (INT_type) 1;
	    for( i = (INT_type) 1;
		 i <= (INT_type) 79; i++ ){
		MonsterValue *= (INT_type) 10;
	    }
	    MonsterValue--;

	    if( TheValue->iso9541_id_value.integer > MonsterValue ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[ATTR_CkVal_ISO9541_Id_Type]: iso9541 id value (integer is too big - " );
		dummy = TKERR_FormatInt( "got value of %d",
			TheValue->iso9541_id_value.integer );
		dummy = TKERR_Format( "for field " );
		dummy = TKERR_Format( TheField->field_name );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	break;

    case ID_STR_tag:

	/*
	    The iso 9541 standard requires that:
	    
		the length of the string be between 1 and 72
		each character be one of:
		    Latin letters A to Z (or equivalently a to z),
		    digits 0 to 9
		or
		    hyphen/minus/neutral dash

	    The test proceeds my making sure the value has
		SEQUENCE_BYTE_tag. Then the length is checked.
		Then the characters in the byte sequence are checked.
		This check is written as if( !( what it should be ) ).

		N.B. The hyphen/minus are the same character according to ISO
		6937/2. QUESTION: what is 'neutral dash' - we could not find
		it listed in ISO 6937/2.
	*/

	TheSequence = TheValue->iso9541_id_value.octet_string;

	if( TheSequence->sequence_tag != SEQUENCE_BYTE_tag ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[TKERR_ATTR_CkVal_ISO9541_Id_Type]: octet string has bad tag - " );
	    dummy = TKERR_FormatInt( "got tag of %d",
		(INT_type) TheSequence->sequence_tag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	if( TheValue->iso9541_id_value.octet_string->length < (INT_type) 1 ||
	    TheValue->iso9541_id_value.octet_string->length > (INT_type) 72 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[TKERR_ATTR_CkVal_ISO9541_Id_Type]: octet string has bad length - " );
	    dummy = TKERR_FormatInt( "got length of %d",
		(INT_type) TheValue->iso9541_id_value.octet_string->length );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	TheByte = TheValue->iso9541_id_value.octet_string->sequence_value.bytes;
	for( i = (INT_type) 0 ; 
	     i < TheValue->iso9541_id_value.octet_string->length; i++ ){
	    
	    if( !( ( *TheByte >= (BYTE_type) 'A' && 
		  *TheByte <= (BYTE_type) 'Z' ) ||
		( *TheByte >= (BYTE_type) 'a' &&
		   *TheByte <= 'z' ) ||
		( *TheByte >= (BYTE_type) '0' &&
		  *TheByte <= '9' ) ||
		( *TheByte == (BYTE_type) '-' ) ) ){

		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[TKERR_ATTR_CkVal_ISO9541_Id_Type]: octet string has bad character of (hex) " );
		dummy = TKERR_FormatInt( "%x ", (INT_type) *TheByte );
		dummy = TKERR_FormatInt( "at character position (base 1) %d",
			(INT_type) (i + 1) );
			    /* add 1 to correct to base 1 */
		dummy = TKERR_Format( " - error in structured name for field " );
		dummy = TKERR_Format( TheField->field_name );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }

	    TheByte++;
	}

	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[ATTR_CkVal_ISO9541_Id_Type]: bad iso9541 id tag" );
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



INT_type ATTR_CkVal_Int_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    /*
	The following fields have int type:

	    FONT_REFERENCEY_type:
		propdata,
		posture,
		weight,
		propwdth,
		numchars,
		structur

	    MODAL_FONT_ATTR_type:
		esclass
    */

    INT_type rval;

    INT_type TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheValue = * (INT_type *) value;

    switch( TheField->universal_field_index ){

    case PROPDATA_UNIV:
	if( TheValue != ISO_PROPDATA_unspec &&
	    TheValue != ISO_PROPDATA_no &&
	    TheValue != ISO_PROPDATA_yes ){

	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Type]: bad value for propdata - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case POSTURE_UNIV:
	if( TheValue != ISO_POSTURE_unspec &&
	    TheValue != ISO_POSTURE_upright &&
	    TheValue != ISO_POSTURE_clockwise &&
	    TheValue != ISO_POSTURE_counter &&
	    TheValue != ISO_POSTURE_italic &&
	    TheValue != ISO_POSTURE_other ){

	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
	    "[ATTR_CkVal_Int_Type]: bad value for posture - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case WEIGHT_UNIV:
	if( TheValue != ISO_WEIGHT_unspec &&
	    TheValue != ISO_WEIGHT_ultra_light &&
	    TheValue != ISO_WEIGHT_extra_light &&
	    TheValue != ISO_WEIGHT_light &&
	    TheValue != ISO_WEIGHT_semi_light &&
	    TheValue != ISO_WEIGHT_medium &&
	    TheValue != ISO_WEIGHT_semi_bold &&
	    TheValue != ISO_WEIGHT_bold &&
	    TheValue != ISO_WEIGHT_extra_bold &&
	    TheValue != ISO_WEIGHT_ultra_bold ){

	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Type]: bad value for weigth - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case PROPWDTH_UNIV:
	if( TheValue != ISO_PROPWDTH_unspec &&
	    TheValue != ISO_PROPWDTH_ultra_condensed &&
	    TheValue != ISO_PROPWDTH_extra_condensed &&
	    TheValue != ISO_PROPWDTH_condensed &&
	    TheValue != ISO_PROPWDTH_semi_condensed &&
	    TheValue != ISO_PROPWDTH_medium &&
	    TheValue != ISO_PROPWDTH_semi_expanded &&
	    TheValue != ISO_PROPWDTH_expanded &&
	    TheValue != ISO_PROPWDTH_extra_expanded &&
	    TheValue != ISO_PROPWDTH_ultra_expanded ){
	    
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Type]: bad value for propwdth - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMCHARS_UNIV:
	/* value must be >= 0 but -1 is the undefined value */
	if( TheValue < (INT_type) -1 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Type]: bad value for numchars - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}
	break;

    case STRUCTUR_UNIV:
	if( TheValue != ISO_STRUCTUR_unspec &&
	    TheValue != ISO_STRUCTUR_solid &&
	    TheValue != ISO_STRUCTUR_outline &&
	    TheValue != ISO_STRUCTUR_inline &&
	    TheValue != ISO_STRUCTUR_shadow &&
	    TheValue != ISO_STRUCTUR_patterned ){
	    
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Type]: bad value for structur - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
	
    case ESCLASS_UNIV:
	if( TheValue != ISO_ESCLASS_unspec &&
	    TheValue != ISO_ESCLASS_fixed &&
	    TheValue != ISO_ESCLASS_variable ){
	    
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Type]: bad value for esclass - got value of " );
	    dummy = TKERR_FormatInt( "%d", TheValue );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_CkVal_Int_Type]: trying to check int type value but got bad universal field index - " );
	dummy = TKERR_Format(" field name is " );
	dummy = TKERR_Format( TheField->field_name );
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



INT_type ATTR_CkVal_Int_Pair_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    /*
	Fields with type INT_PAIR_type:

	    FONT_REFERENCE_type:
		dsnsize,
		minsize,
		maxsize
		mnfeatsz

	    MODAL_FONT_ATTR_type:
		escapdir,
		avgesc,
		avglsesc,
		avgcpesc,
		avgwtesc,
		mfx,
		mlx,
		mbx,
		mrx,
		minampl,
		maxampl,
		minl2lsp,
		minanasc,
		maxanasc

	    struct TABESC:
		tx,
		ty

	    SCORE_type:
		scorpos,
		scorthik

	    VARIANT_SCRIPT_type:
		vxoffset,
		vyoffset,
		vxscale,
		vyscale
    */

    INT_type rval;

    INT_PAIR_type *TheValue;

    INT_type temp_first;
    INT_type temp_second;
    INT_type temp;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheValue = (INT_PAIR_type *) value;

    switch( TheField->universal_field_index ){

    /*
	Int pairs represent real values - for these the real must 
	be >= 0. Given that second == 0 indicates the undefined value
	the test is done in a positive sense as:

	    if( second == 0 )
		ASSERT: any first is ok as value is undefined 
		return success
	    else if( (first > 0 && second > 0 ) ||
		     (first < 0 && second < 0 ) ||
		     ( first == 0 ) )
		    return success
	    else return error
    */
    case DSNSIZE_UNIV:
    case MINSIZE_UNIV:
    case MAXSIZE_UNIV:
    case MNFEATSZ_UNIV:
    case AVGESC_UNIV:
    case SCORTHIK_UNIV:
    case VXSCALE_UNIV:
    case VYSCALE_UNIV:
    case MINL2LSP_UNIV:
    case MINANASC_UNIV:
    case MAXANASC_UNIV:

	if( TheValue->second == (INT_type) 0 ){
	    goto LEAVE;
	}
	else if( (TheValue->first > (INT_type) 0 &&
		    TheValue->second > (INT_type) 0 ) ||
		 (TheValue->first < (INT_type) 0 &&
		    TheValue->second < (INT_type) 0 ) ||
		  TheValue->first == (INT_type) 0 ){
		goto LEAVE;
	}
	else {
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Pair_Type]: illegal real value for field  " );
	    dummy = TKERR_Format( TheField->field_name );
	    dummy = TKERR_Format( ": " );
	    dummy = TKERR_FormatInt( "first = %d ", TheValue->first );
	    dummy = TKERR_FormatInt( "second = %d", TheValue->second );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    /* the following can be any real */
    case AVGLCESC_UNIV:
    case AVGCPESC_UNIV:
    case AVGWTESC_UNIV:
    case TX_UNIV:
    case TY_UNIV:
    case MFX_UNIV:
    case MLX_UNIV:
    case MBX_UNIV:
    case MRX_UNIV:
    case MINAMPL_UNIV:
    case MAXAMPL_UNIV:
    case SCORPOS_UNIV:
    case VXOFFSET_UNIV:
    case VYOFFSET_UNIV:
	break;

    /*
	Must be a positive real and have value between 0 and 360
    */
    case ESCAPDIR_UNIV:
	if( TheValue->second == (INT_type) 0 ){
	    goto LEAVE;
	}
	else if( (TheValue->first > (INT_type) 0 &&
		    TheValue->second > (INT_type) 0 ) ||
		 (TheValue->first < (INT_type) 0 &&
		    TheValue->second < (INT_type) 0 ) ||
		  TheValue->first == (INT_type) 0 ){

		/*
		    calculate an int version of the real to check
		    for between 0 and 360. Given that the direction
		    of truncation when either operand is negative 
		    is undefined, temporary versions of first and
		    second, guaranteed to be positive are created before
		    doing the division. Given that the result will
		    be truncated down, a special case test is
		    needed for a result of exactly 360.
		*/

		if( TheValue->first < (INT_type) 0 ){
		    temp_first = ( - TheValue->first );
    		}
		else {
		    temp_first = TheValue->first;
		}
		if( TheValue->second < (INT_type) 0 ){
		    temp_second = ( - TheValue->second );
    		}
		else {
		    temp_second = TheValue->second;
		}
		temp = temp_first / temp_second;
		/* 
		    Given first test above the result must be positive 
		    so only check for > 360.
		*/
		if( temp > (INT_type) 360 ||
		    ( temp == (INT_type) 360 && 
			TheValue->first % TheValue->second != (INT_type) 0 ) ){
		    rval = ERROR_INT;
		    TKError = BADPARM_err;
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format(
			"[ATTR_CkVal_Int_Pair_Type]: escapdir value is > 360" );
		    dummy = TKERR_Format(" - n.b. test considered first/second" );
		    dummy = TKERR_FormatInt(" - got first = %d ",
			    TheValue->first );
		    dummy = TKERR_FormatInt( "and second = %d",
			    TheValue->second );
		    dummy = TKERR_EndErrMsg();
		    goto LEAVE;
		}
	}
	else {
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_CkVal_Int_Pair_Type]: illegal real value for escapdir: " );
	    dummy = TKERR_FormatInt( "first = %d ", TheValue->first );
	    dummy = TKERR_FormatInt( "second = %d", TheValue->second );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_CkVal_Int_Pair_Type]: bad universal index ('");
	dummy = TKERR_Format(TheField->field_name);
	dummy = TKERR_FormatInt("'): %d", TheField->universal_field_index);
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



INT_type ATTR_CkVal_Sequence_Int_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;
    
    SEQUENCE_INT_type TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheValue = * (SEQUENCE_INT_type *) value;

    if( TheValue == NULL_SEQUENCE ){
	goto LEAVE;
    }

    if( TheValue->sequence_tag != SEQUENCE_INT_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_CkVal_Sequence_Int_Type]: Dsclass value doesn't have sequence int tag" );
	dummy = TKERR_FormatInt( " - got tag = %d",
		(INT_type) TheValue->sequence_tag );
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



INT_type ATTR_CkVal_Writing_Modes_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &WritingModesFields[0],
		&WritingModesInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Writing_Modes_Type]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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



INT_type ATTR_CkVal_Modal_Font_Attr_Type( value, TheField )
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &ModalFontAttrFields[0],
		&ModalFontAttrInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Modal_Font_Attr_Type]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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



INT_type ATTR_CkVal_Tabesc( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &TabescFields[0],
		&TabescInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Tabesc]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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



INT_type ATTR_CkVal_Scores_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &ScoresFields[0],
		&ScoresInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Scores_Type]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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



INT_type ATTR_CkVal_Score_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &ScoreFields[0],
		&ScoreInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Score_Type]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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



INT_type ATTR_CkVal_Variant_Scripts_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &VariantScriptsFields[0],
		&VariantScriptsInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Variant_Scripts_Type]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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



INT_type ATTR_CkVal_Variant_Script_Type( value, TheField)
    IN POINTER_type value;
    IN FIELDS_type *TheField;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ATTR_Ck_Fields( &VariantScriptFields[0],
		&VariantScriptInfo, value );
    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_CkVal_Variant_Script_Type]: The above error occurred in the " );
	dummy = TKERR_Format( TheField->field_name );
	dummy = TKERR_Format( " field" );
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


