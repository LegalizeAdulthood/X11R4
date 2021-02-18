
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr8.c,v 1.1 89/09/21 06:24:17 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr8.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr8.c,v $
 * Revision 1.1  89/09/21  06:24:17  mss
 * Initial revision
 * 
 * Revision 1.15  89/09/12  09:11:15  mss
 * Removed #
 * 
 * Revision 1.14  89/03/09  15:53:28  annm
 * new DAPD defaulting
 * 
 * Revision 1.13  89/02/28  15:38:16  annm
 * minor tweek
 * 
 * Revision 1.12  89/02/23  20:39:22  annm
 * new DVL stuff
 * 
 * Revision 1.11  89/01/19  12:07:19  mss
 * Moved ATTR_FindContentArchClass from EXTERN in .h file
 * to static in .c file
 * 
 * Revision 1.10  89/01/17  16:56:50  annm
 * changes for DVL defaulting
 * 
 * Revision 1.9  88/09/30  15:40:58  annm
 * fixing VMS bugs
 * 
 * Revision 1.8  88/09/21  15:47:56  annm
 * added tkerr stuff
 * 
 * Revision 1.7  88/09/17  16:12:01  mss
 * Got rid of most lines > 71 characters
 * 
 * Revision 1.6  88/09/12  17:03:52  annm
 * buffing rough edges relating to finding content architecture type
 * 
 * Revision 1.5  88/09/06  13:10:36  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/09/01  14:20:50  annm
 * doc app prof defaults now calls the locate constituent routine in
  * file attr7.c to inherit from the constituent found on a doc ap
  * prof defaults list. N.B. can not call inherit from constituent due
  * to mutual recursion between these two routines in the case of
  * related params attribute.
 * 
 * Revision 1.3  88/08/30  15:11:40  annm
 * 
 * 
 * Revision 1.2  88/08/16  18:12:56  annm
 * changes to permit inheritance from default value lists
 * 
 * Revision 1.1  88/08/15  18:06:59  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr8.c,v $";
#endif



#define	TK_IMPL_ATTR
#define TK_IMPL_CONST
#define TK_IMPL_DOC
#define TK_GVARS_ATTR8
#define TK_TKI_TKERR

#include <acd.h>
#include <values.h>
#include <tkerr.h>
#include <attr0.h>

#include <parms.h>

#undef TK_IMPL_ATTR
#undef TK_IMPL_CONST
#undef TK_IMPL_DOC
#undef TK_GVARS_ATTR8
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */



INT_type ATTR_GetDocProfDefaults( constituent, attribute, value,
		    mask, wanted_mask, destination_mask, destination )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    OUT POINTER_type *destination;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INTERNAL_DOCUMENT_type TheDocument;

    CONSTITUENT TheDocumentProfile;

    INT_type SlaveRval;

    dp_DOC_APP_PROF_DEFAULTS_type *TheDefaults;

    ATTR_attribute_t AttrStructPtr;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    /* First see if the constituent's document has the 
	document profile defaults attribute */
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    TheDocument = TheConstituent->document;
    TheDocumentProfile = TheDocument->document_profile;
    if( TheDocumentProfile == NULL_CONSTITUENT ){
	rval = NOT_DONE;
	goto LEAVE;
    }

    SlaveRval = HasAttr(TheDocumentProfile,dp_DOC_APP_PROF_DEFAULTS);
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
    AttrStructPtr =
      ATTR_FindAttr( TheDocumentProfile, dp_DOC_APP_PROF_DEFAULTS );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_GetDocProfDefaults]: constituent has attribute  " ); 
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" but can not find the attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* now get the value */
    TheDefaults = GET_dp_DOC_APP_PROF_DEFAULTS( AttrStructPtr );

    SlaveRval = ATTR_RummageThruDAPD( constituent, attribute, value, mask,
		    wanted_mask, destination_mask, destination, TheDefaults );

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
    if( SlaveRval == TIME_TO_STOP ){
	/* ASSERT: partial value accumulated - caller (GetDefaults in attr7.c )
	    will continue to get values from the iso8613 default */
	rval = TIME_TO_STOP;
	goto LEAVE;
    }
    else {
	/* SlaveRval must = NOT_DONE */
	rval = NOT_DONE;
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



INT_type ATTR_RummageThruDAPD( constituent, attribute, value, mask,
		    wanted_mask, destination_mask, destination, TheDefaults )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    OUT POINTER_type *destination;
    IN dp_DOC_APP_PROF_DEFAULTS_type *TheDefaults;
{

    INT_type rval;

    INT_type SlaveRval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch( attribute ){

    case at_CONTENT_ARCH_CLASS:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) TheDefaults->content_arch_class,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_CONTENT_TYPE:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->content_type,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_DIMENSIONS:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->dimensions,
		    /* source mask */ TheDefaults->dimensions_parm_mask,
		    /* wanted mask */ wanted_mask,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	break;

    case at_TRANSPARENCY:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->transparency,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_COLOUR:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->colour,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_BORDER:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->border,
		    /* source mask */ TheDefaults->border_parm_mask,
		    /* wanted mask */ wanted_mask,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	break;

    case at_LAYOUT_PATH:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->layout_path,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_PAGE_POSITION:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->page_position,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_MEDIUM_TYPE:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->medium_type,
		    /* source mask */ TheDefaults->medium_type_parm_mask,
		    /* wanted mask */ wanted_mask,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	break;

    case at_BLK_ALGNM:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) &TheDefaults->block_alignment,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    case at_TYPE_OF_CODING:
	SlaveRval = ATTR_LoadDestination( attribute, 
		    /* source */ (POINTER_type) TheDefaults->type_of_coding,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* destination mask */ destination_mask,
		    /* destination */ destination );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	}
	else {
	    rval = ALL_DONE;
	}
	break;

    default:
	/* might be one of the consituents */
	rval = ATTR_CheckDAPDConstituents( constituent, attribute, value, mask,
		wanted_mask, destination_mask, destination, TheDefaults );
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



INT_type ATTR_CheckDAPDConstituents( constituent, attribute, value, mask,
		wanted_mask, destination_mask, destination, TheDefaults )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    OUT POINTER_type value;
    OUT PARM_MASK_type *mask;
    IN PARM_MASK_type wanted_mask;
    IN OUT PARM_MASK_type *destination_mask;
    OUT POINTER_type *destination;
    IN dp_DOC_APP_PROF_DEFAULTS_type *TheDefaults;
{

    INT_type rval;

    BOOL_type answer;

    INT_type SlaveRval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    answer = ATTR_IsCCPresentation( attribute );
    if( answer == BOOL_true && 
	TheDefaults->cc_presentation != NULL_CONSTITUENT ){
	SlaveRval = ATTR_LocateConstAttr( TheDefaults->cc_presentation, 
		    attribute, wanted_mask,
		    destination_mask, destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	goto LEAVE;
    }

    answer = ATTR_IsCCCoding( attribute );
    if( answer == BOOL_true &&
	TheDefaults->cc_coding != NULL_CONSTITUENT ){
	SlaveRval = ATTR_LocateConstAttr( TheDefaults->cc_coding, 
		    attribute, wanted_mask,
		    destination_mask, destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	goto LEAVE;
    }

    answer = ATTR_IsRGPresentation( attribute );
    if( answer == BOOL_true &&
	TheDefaults->rg_presentation != NULL_CONSTITUENT ){
	SlaveRval = ATTR_LocateConstAttr( TheDefaults->rg_presentation, 
		    attribute, wanted_mask,
		    destination_mask, destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	goto LEAVE;
    }

    answer = ATTR_IsRGCoding( attribute );
    if( answer == BOOL_true &&
	TheDefaults->rg_coding != NULL_CONSTITUENT ){
	SlaveRval = ATTR_LocateConstAttr( TheDefaults->rg_coding, 
		    attribute, wanted_mask,
		    destination_mask, destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	goto LEAVE;
    }

    answer = ATTR_IsGGPresentation( attribute );
    if( answer == BOOL_true &&
	TheDefaults->gg_presentation != NULL_CONSTITUENT ){
	SlaveRval = ATTR_LocateConstAttr( TheDefaults->gg_presentation, 
		    attribute, wanted_mask,
		    destination_mask, destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	goto LEAVE;
    }

    answer = ATTR_IsGGCoding( attribute );
    if( answer == BOOL_true &&
	TheDefaults->gg_coding != NULL_CONSTITUENT ){
	SlaveRval = ATTR_LocateConstAttr( TheDefaults->gg_coding, 
		    attribute, wanted_mask,
		    destination_mask, destination );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	rval = ATTR_AreWeDone( attribute, wanted_mask, destination_mask );
	goto LEAVE;
    }

    /* if get here then the attribute was not a presentation or coding attr */
    rval = NOT_DONE;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



BOOL_type ATTR_IsCCPresentation( attribute )
    IN INT_type attribute;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch( attribute ){

    case cc_ALIGNMENT:
    case cc_CHARACTER_FONTS:
    case cc_CHAR_ORIENTATION:
    case cc_CHARACTER_PATH:
    case cc_CHARACTER_SPACING:
    case cc_CODE_EXT_ANNOUNCERS:
    case cc_1ST_LINE_OFFSET:
    case cc_GRAPHIC_CHARACTER_SETS:
    case cc_GRAPHIC_CHAR_SUBREP:
    case cc_GRAPHIC_RENDITION:
    case cc_ITEMISATION:
    case cc_KERNING_OFFSET:
    case cc_LINE_LAYOUT_TABLE:
    case cc_LINE_PROGRESSION:
    case cc_LINE_SPACING:
    case cc_PAIRWISE_KERNING:
    case cc_FORMATTING_INDICATOR:
    case cc_INITIAL_OFFSET:
    case cc_INDENTATION:
    case cc_ORPHAN_SIZE:
    case cc_PROP_LINE_SPACING:
    case cc_WIDOW_SIZE:
	rval = BOOL_true;
	break;

    default:
	rval = BOOL_false;
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



BOOL_type ATTR_IsCCCoding( attribute )
    IN INT_type attribute;
{

    /*
	There are no CC specific coding attributes.
    */

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = BOOL_false;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}



BOOL_type ATTR_IsRGPresentation( attribute )
    IN INT_type attribute;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch( attribute ){

    case rg_CLIPPING:
    case rg_LINE_PROGRESSION:
    case rg_PEL_PATH:
    case rg_INITIAL_OFFSET:
    case rg_PEL_TRANS_DENSITY:
    case rg_IMAGE_DIMENSIONS:
    case rg_PEL_SPACING:
    case rg_SPACING_RATIO:
	rval = BOOL_true;
	break;
	
    default:
	rval = BOOL_false;
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



BOOL_type ATTR_IsRGCoding( attribute )
    IN INT_type attribute;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch( attribute ){

    case rg_COMPRESSION:
    case rg_NUMBER_OF_LINES:
    case rg_NUMBER_OF_PELS_PER_LINE:
    case rg_NUMBER_DISCARDED_PELS:
	rval = BOOL_true;
	break;

    default:
	rval = BOOL_false;
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



BOOL_type ATTR_IsGGPresentation( attribute )
    IN INT_type attribute;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    switch( attribute ){

    case gg_ENCODING_ANNOUNCER:
    case gg_LINE_RENDITION:
    case gg_MARKER_RENDITION:
    case gg_TEXT_RENDITION:
    case gg_FILLED_AREA_REND:
    case gg_EDGE_RENDITION:
    case gg_COLOUR_REPS:
    case gg_TRANSPARENCY_SPEC:
    case gg_TRANSFORMATION_SPEC:
    case gg_REGION_INTEREST_SPEC:
    case gg_PICTURE_ORIENTATION:
    case gg_PICTURE_DIMENSIONS:
	rval = BOOL_true;
	break;
    
    default:
	rval = BOOL_false;
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



BOOL_type ATTR_IsGGCoding( attribute )
    IN INT_type attribute;
{

    /*
	There no coding attributes specific to the geometrics graphics
	content architecture.
    */

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ) {
	Entering();
    }
#endif

    rval = BOOL_false;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ) {
	Leaving();
    }
#endif

    return( rval );

}

