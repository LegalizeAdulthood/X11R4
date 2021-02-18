
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr14.c,v 1.1 89/09/21 06:17:38 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr14.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr14.c,v $
 * Revision 1.1  89/09/21  06:17:38  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/12  09:05:07  mss
 * Removed #
 * 
 * Revision 1.3  88/12/13  19:54:31  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.2  88/11/01  16:01:52  annm
 * fix of font monster alloc/copy bug for null sequence value in the sequence of ints field
 * 
 * Revision 1.1  88/10/14  16:09:14  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: ";
#endif

/* Put .c template comment here */

/*

    This file contains the copy canonical procedures 
    for the SEQUENCE_FONT_PAIR_type aunion.h type.


*/

#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR14
#define TK_TKI_TKERR
#define TK_TKI_SEQ

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <seq.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR14
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ


/* put #define's and declarations local to this file here */



INT_type SEQUENCE_FONT_PAIR_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    SEQUENCE_FONT_PAIR_type TheSource;

    SEQUENCE_FONT_PAIR_type TheDestination;

    INT_type dummy;

    INT_type SlaveRval;

    INT_type i;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (SEQUENCE_FONT_PAIR_type) source;
    TheDestination = (SEQUENCE_FONT_PAIR_type) destination;

    if( TheSource->sequence_tag != SEQUENCE_FONT_PAIR_tag ||
	TheDestination->sequence_tag != SEQUENCE_FONT_PAIR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[SEQUENCE_FONT_PAIR_Copy]: bad sequence tag in source or destiantion" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSource->length; i++ ){

	SlaveRval = ATTR_FontPairCopy( &TheSource->sequence_value.font_pairs[i],
		&TheDestination->sequence_value.font_pairs[i] );
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



INT_type ATTR_FontPairCopy( Source, Destination )
    IN FONT_PAIR_type *Source;
    OUT FONT_PAIR_type *Destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    /* copy font id */
    Destination->font_id = Source->font_id;

    /* copy the font reference */
    rval = ATTR_Copy_Fields( &FontReferenceFields[0],
	    &FontReferenceInfo, (POINTER_type) &Source->font_reference,
	    (POINTER_type) &Destination->font_reference );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Fields( fields, info, src, dest )
    IN FIELDS_type *fields;
    IN FONT_BRANCH_INFO_type *info;
    IN POINTER_type src;
    IN POINTER_type dest;
{

    INT_type rval; 

    INT_type i;

    POINTER_type SrcBase;
    POINTER_type DestBase;

    FIELDS_type *ThisField;    

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    for( i = (INT_type) 0; i < info->table_size; i++ ){

	SrcBase = (*info->GetBase)( src, &fields[i] );
	DestBase = (*info->GetBase)( dest, &fields[i] );    

	ThisField = &fields[i];
	rval = (*ThisField->Copy )( SrcBase, DestBase );
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



/* Get base routines  for the font reference type/subtypes */

POINTER_type ATTR_GetFontReferenceBase( base, TheField )
POINTER_type base;
FIELDS_type *TheField;
{

    FONT_REFERENCE_type *FontRefBase;

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    FontRefBase = (FONT_REFERENCE_type *) base;

    switch( TheField->field_index ){

    case DFNPRE:
	rval = ERROR_POINTER;
	break;
    
    case FONTNAME:
	rval = (POINTER_type) &FontRefBase->fontname;
	break;
    case DIFNTNAM:
	rval = (POINTER_type) &FontRefBase->difntnam;	
	break;
    case PROPDATA:
	rval = (POINTER_type) &FontRefBase->propdata;    
	break;
    case TYPEFACE:
	rval = (POINTER_type) &FontRefBase->typeface;
	break;
    case FAMILY:
	rval = (POINTER_type) &FontRefBase->family;    
	break;
    case POSTURE:
	rval = (POINTER_type) &FontRefBase->posture;
	break;
    case WEIGHT:
	rval = (POINTER_type) &FontRefBase->weight;
	break;
    case PROPWDTH:
	rval = (POINTER_type) &FontRefBase->propwdth;
	break;
    case NUMCHARS:
	rval = (POINTER_type) &FontRefBase->numchars;
	break;
    case DSNSIZE:
	rval = (POINTER_type) &FontRefBase->dsnsize;
	break;
    case MINSIZE:
	rval = (POINTER_type) &FontRefBase->minsize;
	break;
    case MAXSIZE:
	rval = (POINTER_type) &FontRefBase->maxsize;
	break;
    case DSCLASS:
	rval = (POINTER_type) &FontRefBase->dsclass;
	break;
    case STRUCTUR:
	rval = (POINTER_type) &FontRefBase->structur;
	break;
    case MNFEATSZ:
	rval = (POINTER_type) &FontRefBase->mnfeatsz;
	break;
    case MODES:
	rval = (POINTER_type) &FontRefBase->modes;
	break;
    case MODE:
	rval = (POINTER_type) &FontRefBase->mode;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetFontReferenceBase]: bad field index" );
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



POINTER_type ATTR_GetWritingModesBase( base, TheField )
POINTER_type base;
FIELDS_type *TheField;
{

    WRITING_MODES_type *TheModes;

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheModes = (WRITING_MODES_type *) base;

    switch( TheField->field_index ){

    case DFNPRE:
	rval = ERROR_POINTER;
	break;
    
    case ISO_0:
	rval = (POINTER_type) &TheModes->iso_0;
	break;

    case ISO_180:
	rval = (POINTER_type) &TheModes->iso_180;
	break;

    case ISO_270:
	rval = (POINTER_type) &TheModes->iso_270;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetFontReferenceBase]: bad field index" );
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



POINTER_type ATTR_GetModalFontAttrBase( base, TheField)
POINTER_type base;
FIELDS_type *TheField;
{

    POINTER_type rval;

    MODAL_FONT_ATTR_type *TheModal;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheModal = (MODAL_FONT_ATTR_type *) base;

    switch( TheField->field_index ){

    case DFNPRE:
	rval = ERROR_POINTER;
	break;
    
    case ESCAPDIR:
	rval = (POINTER_type) &TheModal->escapdir;
	break;

    case ESCLASS:
	rval = (POINTER_type) &TheModal->esclass;
	break;

    case AVGESC:
	rval = (POINTER_type) &TheModal->avgesc;
	break;

    case AVGLCESC:
	rval = (POINTER_type) &TheModal->avglcesc;
	break;

    case AVGCPESC:
	rval = (POINTER_type) &TheModal->avgcpesc;
	break;

    case AVGWTESC:
	rval = (POINTER_type) &TheModal->avgwtesc;
	break;

    case TABESC_FIELD:
	rval = (POINTER_type) &TheModal->tabesc;
	break;

    case MFX:
	rval = (POINTER_type) &TheModal->mfx;
	break;

    case MLX:
	rval = (POINTER_type) &TheModal->mlx;
	break;

    case MBX:
	rval = (POINTER_type) &TheModal->mbx;
	break;

    case MRX:
	rval = (POINTER_type) &TheModal->mrx;
	break;

    case MINAMPL:
	rval = (POINTER_type) &TheModal->minampl;
	break;

    case MAXAMPL:
	rval = (POINTER_type) &TheModal->maxampl;
	break;

    case SCORES_FIELD:
	rval = (POINTER_type) &TheModal->scores;
	break;

    case VARSCRPT_FIELD:
	rval = (POINTER_type) &TheModal->varscrpt;
	break;

    case MINL2LSP:
	rval = (POINTER_type) &TheModal->minl2lsp;
	break;

    case MINANASC:
	rval = (POINTER_type) &TheModal->minanasc;
	break;

    case MAXANASC:
	rval = (POINTER_type) &TheModal->maxanasc;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetFontReferenceBase]: bad field index" );
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



POINTER_type ATTR_GetTabescBase( base, TheField)
POINTER_type base;
FIELDS_type *TheField;
{

    POINTER_type rval;

    struct tabesc_struct *TheTabesc;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheTabesc = (struct tabesc_struct *) base;

    switch( TheField->field_index ){

    case TX:
	rval = (POINTER_type) &TheTabesc->tx;
	break;

    case TY:
	rval = (POINTER_type) &TheTabesc->ty;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetTabescBase]: bad field index" );
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



POINTER_type ATTR_GetScoresBase( base, TheField )
POINTER_type base;
FIELDS_type *TheField;
{

    POINTER_type rval;

    SCORES_type *TheScores;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheScores = (SCORES_type *) base;

    switch( TheField->field_index ){

    case DFNPRE:
	rval = ERROR_POINTER;
	break;
    
    case RSCORE:
	rval = (POINTER_type) &TheScores->rscore;
	break;

    case LSCORE:
	rval = (POINTER_type) &TheScores->lscore;
	break;

    case TSCORE:
	rval = (POINTER_type) &TheScores->tscore;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetScoresBase]: bad field index" );
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

POINTER_type ATTR_GetScoreBase( base, TheField )
POINTER_type base;
FIELDS_type *TheField;
{

    POINTER_type rval;

    SCORE_type *TheScore;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheScore = (SCORE_type *) base;

    switch( TheField->field_index ){

    case SCORPOS:
	rval = (POINTER_type) &TheScore->scorpos;
	break;

    case SCORTHIK:
	rval = (POINTER_type) &TheScore->scorthik;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetScoresBase]: bad field index" );
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

POINTER_type ATTR_GetVariantScriptsBase( base, TheField )
POINTER_type base;
FIELDS_type *TheField;
{

    POINTER_type rval;

    VARIANT_SCRIPTS_type *TheScripts;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheScripts = (VARIANT_SCRIPTS_type *) base;

    switch( TheField->field_index ){

    case DFNPRE:
	rval = ERROR_POINTER;
	break;

    case LSCRIPT:
	rval = (POINTER_type) &TheScripts->lscript;
	break;

    case RSCRIPT:
	rval = (POINTER_type) &TheScripts->rscript;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetScoresBase]: bad field index" );
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

POINTER_type ATTR_GetVariantScriptBase( base, TheField )
POINTER_type base;
FIELDS_type *TheField;
{

    POINTER_type rval;

    VARIANT_SCRIPT_type *TheScript;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheScript = (VARIANT_SCRIPT_type *) base;

    switch( TheField->field_index ){

    case VXOFFSET:
	rval = (POINTER_type) &TheScript->vxoffset;
	break;

    case VYOFFSET:
	rval = (POINTER_type) &TheScript->vyoffset;
	break;

    case VXSCALE:
	rval = (POINTER_type) &TheScript->vxscale;
	break;

    case VYSCALE:
	rval = (POINTER_type) &TheScript->vyscale;
	break;

    default: 
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetScoresBase]: bad field index" );
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



/* low level output routines by type */

INT_type ATTR_Copy_Dfnpre( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
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



INT_type ATTR_Copy_Structured_Name( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    STRUCTURED_NAME_type *TheSrc;
    STRUCTURED_NAME_type *TheDest;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (STRUCTURED_NAME_type *) src;
    TheDest = (STRUCTURED_NAME_type *) dest;

    switch( TheSrc->structured_name_tag ){

    case STRUCTURED_NAME_ID_tag:
	SlaveRval = ATTR_Copy_ISO9541_Id_Type( 
	    &TheSrc->structured_name_value.id,
	    &TheDest->structured_name_value.id );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	break;

    case STRUCTURED_NAME_SEQ_tag:

	for( i = (INT_type) 0;
	     i < TheSrc->structured_name_value.idseq->length;
	     i++ ){

	    SlaveRval = ATTR_Copy_ISO9541_Id_Type( 
		&TheSrc->structured_name_value.idseq->sequence_value.iso9541_ids[i],
		&TheDest->structured_name_value.idseq->sequence_value.iso9541_ids[i] );

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
		"[ATTR_Copy_Structured_Name]: bad structured name tag" );
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



INT_type ATTR_Copy_ISO9541_Id_Type( TheSrc, TheDest )
    IN ISO9541_ID_type *TheSrc;
    OUT ISO9541_ID_type *TheDest;
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

    switch( TheSrc->iso9541_id_tag ){

    case ID_INT_tag:
	TheDest->iso9541_id_value.integer = TheSrc->iso9541_id_value.integer;
	break;

    case ID_STR_tag:
	SlaveRval = SEQ_CopySequenceElements( 
	    TheSrc->iso9541_id_value.octet_string,	
	    TheDest->iso9541_id_value.octet_string );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[ATTR_Copy_ISO9541_Id_Type]: bad iso9541 id tag" );
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



INT_type ATTR_Copy_Int_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    INT_type *Src;
    INT_type *Dest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Src = (INT_type *) src;
    Dest = (INT_type *) dest;

    *Dest = *Src;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Int_Pair_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    INT_PAIR_type *TheSrc;
    INT_PAIR_type *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (INT_PAIR_type *) src;
    TheDest = (INT_PAIR_type *) dest;

    TheDest->first = TheSrc->first;
    TheDest->second = TheSrc->second;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Sequence_Int_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    SEQUENCE_type TheSrc;
    SEQUENCE_type TheDest;

    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = * (SEQUENCE_type *) src;
    TheDest = *(SEQUENCE_type *) dest;

    if( TheSrc == NULL_SEQUENCE ){
	goto LEAVE;
    }

    SlaveRval = SEQ_CopySequenceElements( TheSrc, TheDest );
    if( SlaveRval == ERROR_INT ){
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



INT_type ATTR_Copy_Writing_Modes_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    WRITING_MODES_type *TheSrc;
    WRITING_MODES_type *TheDest;


#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (WRITING_MODES_type *) src;
    TheDest = (WRITING_MODES_type *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &WritingModesFields[0], &WritingModesInfo,
		src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Modal_Font_Attr_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    MODAL_FONT_ATTR_type *TheSrc;
    MODAL_FONT_ATTR_type *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (MODAL_FONT_ATTR_type *) src;
    TheDest = (MODAL_FONT_ATTR_type *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &ModalFontAttrFields[0], 
	&ModalFontAttrInfo, src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Tabesc( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    struct tabesc_struct *TheSrc;
    struct tabesc_struct *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (struct tabesc_struct *) src;
    TheDest = (struct tabesc_struct *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &TabescFields[0], &TabescInfo,
		src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Scores_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    SCORES_type *TheSrc;
    SCORES_type *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (SCORES_type *) src;
    TheDest = (SCORES_type *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &ScoresFields[0], 
	&ScoresInfo, src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Score_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    SCORE_type *TheSrc;
    SCORE_type *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (SCORE_type *) src;
    TheDest = (SCORE_type *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &ScoreFields[0], 
	&ScoreInfo, src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_Copy_Variant_Scripts_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    VARIANT_SCRIPTS_type *TheSrc;
    VARIANT_SCRIPTS_type *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (VARIANT_SCRIPTS_type *) src;
    TheDest = (VARIANT_SCRIPTS_type *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &VariantScriptsFields[0], 
	&VariantScriptsInfo, src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );


}



INT_type ATTR_Copy_Variant_Script_Type( src, dest )
    IN POINTER_type src;
    OUT POINTER_type dest;
{

    INT_type rval;

    VARIANT_SCRIPT_type *TheSrc; 
    VARIANT_SCRIPT_type *TheDest;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSrc = (VARIANT_SCRIPT_type *) src;
    TheDest = (VARIANT_SCRIPT_type *) dest;

    TheDest->specified = TheSrc->specified;

    if( TheSrc->specified == BOOL_false ){
	goto LEAVE;
    }

    rval = ATTR_Copy_Fields( &VariantScriptFields[0], 
	&VariantScriptInfo, src, dest );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}
