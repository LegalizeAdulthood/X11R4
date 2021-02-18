
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr19.c,v 1.1 89/09/21 05:43:13 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr19.c,v $

 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *

$Log:	iswr19.c,v $
 * Revision 1.1  89/09/21  05:43:13  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:47:37  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/12  09:35:20  mss
 * Removed #
 * 
 * Revision 1.5  89/08/28  16:48:17  jl3p
 * added copyright notice
 * 
 * Revision 1.4  89/08/16  09:55:38  mss
 * Arranged everything to use global odaiso.h
 * 
 * Revision 1.3  89/08/11  11:21:26  jl3p
 * fixed ISODE write routines for content portions
 * so that multiple pointers to content portions would not
 * be used more than once.
 * 
 * Revision 1.2  89/08/03  21:41:51  mss
 * Changed some ODIF's to ISWR
 * 
 * Revision 1.1  89/08/03  15:40:01  mss
 * Initial revision
 * 
 * Revision 1.3  89/06/28  15:35:35  jl3p
 * Fixed iterator problem
 * 
 * Revision 1.2  89/06/26  15:06:38  jl3p
 * fixed PStream I/O problem
 * 
 * Revision 1.1  89/06/21  16:52:07  annm
 * Initial revision
 * 
*/

#ifdef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr19.c,v $";
#endif

/* UNIX includes */
#include <stdio.h>

/* isode includes */
#include <psap.h>

/* toolkit incluces */
#define TK_IMPL_ISWR
#define TK_GVARS_ISWR19
#define TK_IMPL_INIT
#define TK_TKI_TKERR

#include <values.h>
#include <apityp.h>
#include <init.h>
#include <acd.h>
#include <iter.h>
#include <iswr.h>
#include <tkerr.h>

#undef TK_IMPL_ISWR
#undef TK_GVARS_ISWR19
#undef TK_IMPL_INIT
#undef TK_TKI_TKERR



INT_type isode_WriteODIF(OutFile, document, format) 
    IN FILE		*OutFile;
    IN DOCUMENT_type	document;
    IN VALUE_type	format;
{

    INT_type		rval;	/* The returned value */

    INT_type		dummy;

    INT_type		SlaveRval;

    /* info passed to the isode stuff */
    struct isode2toolkit_t isode2toolkit;

    PS ps;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Interchange format class B unimplemented */
    if (format == dp_INTERCHANGE_FMT_CLASS_b) {
	rval = ERROR_INT;
	TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[isode_WriteODIF]: The interchange class B is not  implemented" );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }

    /* Make a sequence for the specific content portions first */
    isode2toolkit.Contents = MakeSequence(SEQUENCE_CONSTITUENT_tag,  (INT_type) 0);
    if( isode2toolkit.Contents == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[isode_WriteODIF]: can not make the seuqence for contents" );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }

    /* see if we need to label the doucment */
    if( InitParms->automatic_label == BOOL_true ){
	SlaveRval = LabelDocument( document );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = DeleteSequence( isode2toolkit.Contents );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[isode_WriteODIF]: can not label the document" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    /* allocate the output stream */
    isode2toolkit.ps = ps_alloc( std_open );
    if( isode2toolkit.ps == NULLPS ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not allocate the output stream" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = std_setup( isode2toolkit.ps, OutFile );
    if( SlaveRval != OK ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not perform the ps standard setup" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* write out the doucment */

    SlaveRval = ISODE_WriteDocProfile( document, &isode2toolkit );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the document profile" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }
    
    /* write out the generic layout structure */
    isode2toolkit.what_kind_constituent = GENERIC_LAYOUT;
    SlaveRval = ISODE_WriteStructure( document, &isode2toolkit, 
		    GENERIC_DOC_LAYOUT_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the generic layout structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* write out the specific layout structure */
    isode2toolkit.what_kind_constituent = SPECIFIC_LAYOUT;
    SlaveRval = ISODE_WriteStructure( document, &isode2toolkit, 
		    SPECIFIC_DOC_LAYOUT_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the specific layout structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* write out the text  */
    SlaveRval = ISODE_WriteText( document, &isode2toolkit );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the text" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }
    
    /* write out the generic logical structure */
    isode2toolkit.what_kind_constituent = GENERIC_LOGICAL;
    SlaveRval = ISODE_WriteStructure( document, &isode2toolkit, 
		    GENERIC_DOC_LOGICAL_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the generic logical structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* write out the generic logical structure */
    isode2toolkit.what_kind_constituent = SPECIFIC_LOGICAL;
    SlaveRval = ISODE_WriteStructure( document, &isode2toolkit, 
		    SPECIFIC_DOC_LOGICAL_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the  specific logical structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* write out the presentation styles */
    isode2toolkit.what_kind_constituent = PRESENTATION;
    SlaveRval = ISODE_WriteStyles( document, &isode2toolkit, 
		    PRESENTATION_STYLE );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the  presentation styles" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* write out the presentation styles */
    isode2toolkit.what_kind_constituent = LAYOUT;
    SlaveRval = ISODE_WriteStyles( document, &isode2toolkit, 
		    LAYOUT_STYLE );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;   
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_WriteODIF]: can not write the  layout styles" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



INT_type ISODE_WriteDocProfile( document, isode2toolkit )
    IN DOCUMENT_type document;
    IN struct isode2toolkit_t *isode2toolkit;
{

    INT_type rval;

    CONSTITUENT constituent;

    INT_type dummy;

    INT_type SlaveRval;

    PE pe;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    constituent = FindDocumentProfile( document );
    if( constituent == ERROR_CONSTITUENT && TKError != NOPROF_err ){
	rval = ERROR_INT;
	dummy = DeleteSequence( isode2toolkit->Contents );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WriteDocProfile]: can not find the doc profile" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( constituent == ERROR_CONSTITUENT && TKError == NOPROF_err ){
	goto LEAVE;
    }

    isode2toolkit->constituent = constituent;
    isode2toolkit->what_kind_constituent = DOC_PROFILE;

    SlaveRval = build_ISWR1_Interchange__Data__Element(&pe, 1, NULL, NULL, isode2toolkit );
    if( SlaveRval == NOTOK ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = DeleteSequence( isode2toolkit->Contents );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WriteDocProfile]: isode can not write the document profile" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    /* write out the document profile pe */
    SlaveRval = pe2ps( isode2toolkit->ps, pe );
    if( SlaveRval == NOTOK ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	pe_free( pe );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WRiteDocProfile]: can not write the document profile pe" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    pe_free( pe );


LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}



INT_type ISODE_WriteStructure( document, isode2toolkit, which_one )
    IN DOCUMENT_type	    document;
    IN struct isode2toolkit_t *isode2toolkit;
    IN VALUE_type	    which_one;
{

    INT_type rval;

    PE pe;

    CONSTITUENT constituent;

    INT_type dummy;

    INT_type SlaveRval;

    ITERATOR_type iterator;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    constituent = FindDocumentRoot( document, which_one );
    if( constituent == ERROR_CONSTITUENT && TKError != NOROOT_err ){
	rval = ERROR_INT;
	dummy = DeleteSequence( isode2toolkit->Contents );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WriteStructure]: can not find the root" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( constituent == ERROR_CONSTITUENT && TKError == NOROOT_err ){
	goto LEAVE;
    }

    isode2toolkit->constituent = constituent;

    iterator = MakeSubgraphIterator( constituent, PARENTS_FIRST, BREADTH_FIRST );
    if( iterator == ERROR_ITERATOR ){
	rval = ERROR_INT;
	dummy = DeleteSequence( isode2toolkit->Contents );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WriteStructure]: can not make iterator" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( constituent = NextConstituent( iterator ); 
	   constituent != NULL_CONSTITUENT && constituent != ERROR_CONSTITUENT;
	   constituent = NextConstituent( iterator ) ){

	isode2toolkit->constituent = constituent;
	SlaveRval = build_ISWR1_Interchange__Data__Element(&pe, 1, NULL, NULL, isode2toolkit );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = DeleteSequence( isode2toolkit->Contents );
	    dummy = DeleteIterator( iterator );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_WriteStructure]: isode can not write the structure" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}

	/* write out the pe */
	SlaveRval = pe2ps( isode2toolkit->ps, pe );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    pe_free( pe );
	    dummy = DeleteSequence( isode2toolkit->Contents );
	    dummy = DeleteIterator( iterator );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_WriteStructure]: can not write the pe" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	pe_free( pe );

    }

    if( constituent == ERROR_CONSTITUENT ){
	rval = ERROR_INT;
	dummy = DeleteSequence( isode2toolkit->Contents );
	dummy = DeleteIterator( iterator );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WriteStructure]: nextconstituent returned error" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    dummy = DeleteIterator( iterator );

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}



INT_type ISODE_WriteStyles( document, isode2toolkit, which_one )
    IN DOCUMENT_type	    document;
    IN struct isode2toolkit_t *isode2toolkit;
    IN VALUE_type	    which_one;
{

    INT_type rval;

    PE pe;

    ITERATOR_type iterator;

    CONSTITUENT constituent;

    INT_type dummy;

    INT_type SlaveRval;
    
#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    iterator = MakeStylesIterator( document, which_one );
    if( iterator == ERROR_ITERATOR ){
	/* assume that there are no styles */
	goto LEAVE;
    }

    for( constituent = NextConstituent( iterator );
	 constituent != NULL_CONSTITUENT && constituent != ERROR_CONSTITUENT;
	 constituent = NextConstituent( iterator ) ){

	isode2toolkit->constituent = constituent;
	SlaveRval = build_ISWR1_Interchange__Data__Element(&pe, 1, NULL, NULL, isode2toolkit );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = DeleteSequence( isode2toolkit->Contents );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_WriteStyles]: isode can not write the style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}

	/* write out the pe */
	SlaveRval = pe2ps( isode2toolkit->ps, pe );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    pe_free( pe );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_WriteStyles]: can not write a  style pe" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	pe_free( pe );
	
    }

    if( constituent == ERROR_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = DeleteSequence( isode2toolkit->Contents );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_WriteStyles]: can not get the next style" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    dummy = DeleteIterator( iterator );

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}



INT_type ISODE_WriteText( document, isode2toolkit )
    IN DOCUMENT_type	    document;
    IN struct isode2toolkit_t	*isode2toolkit;
{

    /*
	At this point the Content sequence in isode2toolkit
	contains the content portions for the layout structure.
	First round up the content for the logical structure.
	Then isode each content portion.
    */

    INT_type rval;

    PE pe;

    INT_type dummy;

    INT_type SlaveRval;

    INT_type i;
    
#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    SlaveRval = ISODE_FindContent( document, isode2toolkit, 
		    GENERIC_DOC_LAYOUT_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_FindWriteText]: can not find generic logical content" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }


    SlaveRval = ISODE_FindContent( document, isode2toolkit, 
		    SPECIFIC_DOC_LAYOUT_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_FindWriteText]: can not find specific logical content" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = ISODE_FindContent( document, isode2toolkit, 
		    GENERIC_DOC_LOGICAL_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_FindWriteText]: can not find generic logical content" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }


    SlaveRval = ISODE_FindContent( document, isode2toolkit, 
		    SPECIFIC_DOC_LOGICAL_ROOT );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_FindWriteText]: can not find specific logical content" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* now isode the content portions */
    isode2toolkit->what_kind_constituent = TEXT;
    for( i = (INT_type) 0; i < isode2toolkit->Contents->length; i++ ){

	isode2toolkit->constituent = isode2toolkit->Contents->sequence_value.constituents[i];
	SlaveRval = build_ISWR1_Interchange__Data__Element(&pe, 1, NULL, NULL, isode2toolkit );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = DeleteSequence( isode2toolkit->Contents );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_WriteText]: isode can not write text" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}

	SlaveRval = pe2ps( isode2toolkit->ps, pe );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    pe_free( pe );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_FindWriteText]: can not write a text unit pe" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	pe_free( pe );
    
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}



INT_type ISODE_FindContent( document, isode2toolkit, what_kind )
    IN DOCUMENT_type	    document;
    IN struct isode2toolkit_t	*isode2toolkit;
    IN VALUE_type	    what_kind;
{

    INT_type rval;

    CONSTITUENT constituent;

    INT_type dummy;

    INT_type SlaveRval;

    INT_type i;

    ITERATOR_type iterator;

    SEQUENCE_CONSTITUENT_type content;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    constituent = FindDocumentRoot( document, what_kind );
    if( constituent == ERROR_CONSTITUENT && TKError != NOROOT_err ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_FindContent]: can not find root" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( constituent == ERROR_CONSTITUENT && TKError == NOROOT_err ){
	goto LEAVE;
    }

    iterator = MakeSubgraphIterator( constituent, PARENTS_FIRST, DEPTH_FIRST );
    if( iterator == ERROR_ITERATOR ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_FindContent]: can not make iterator" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    for( constituent = NextConstituent( iterator );
	 constituent != ERROR_CONSTITUENT && constituent != NULL_CONSTITUENT;
	 constituent = NextConstituent( iterator ) ){

	SlaveRval = GetAttr( constituent, at_CONTENT_PORTIONS, &content,
			BOOL_false, (PARM_MASK_type) 0 );
	if( SlaveRval == ERROR_INT ){
	    continue;
	}

	for( i = (INT_type) 0;
	     i < content->length;
	     i++ ) {

	    SlaveRval =
		ISODE_AddContent(isode2toolkit->Contents,
				 content->sequence_value.constituents[i]);
	    if ( SlaveRval == ERROR_INT ) {
		rval = ERROR_INT;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[ISODE_FindContent]: Can not add to Content sequence");
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }

	} 

        dummy = DeleteSequence( content );
    }

    dummy = DeleteIterator( iterator );

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}



INT_type ISODE_AddContent( sequence, content )
    IN OUT SEQUENCE_CONSTITUENT_type	sequence;
    IN CONSTITUENT_type			content;
{

    INT_type rval;

    INT_type i;

    CONSTITUENT_type c;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    for (i=0; i<sequence->length; i++) {

	c = sequence->sequence_value.constituents[i];
	if ( ISODE_ContentsEqual(c,content) ) {
	    /* Already in list -- do not add again */
	    goto LEAVE;
	}

    }

    SlaveRval = ExpandSequence( sequence, (INT_type) 1);
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_AddContent]: can not expand Content sequence" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    sequence->sequence_value.constituents[sequence->length-1] = content;

LEAVE:

#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}


BOOL_type ISODE_ContentsEqual(c1,c2)
    IN CONSTITUENT_type c1;
    IN CONSTITUENT_type c2;
{

    BOOL_type rval;

    STRING_type logical_id1;
    STRING_type logical_id2;

    STRING_type layout_id1;
    STRING_type layout_id2;

    BOOL_type logical_ids_match;
    BOOL_type layout_ids_match;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* check if layout content id's match */

    if ( GetAttr( c1,
		  at_CONTENT_ID_LAYOUT,
		  (POINTER_type) &layout_id1,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
		) == ERROR_INT ) {

	/* No layout id to match with */
	layout_ids_match = BOOL_false;

    } else if ( GetAttr( c2,
		         at_CONTENT_ID_LAYOUT,
			 (POINTER_type) &layout_id2,
			 /* default */ BOOL_false,
			 /* mask */ (PARM_MASK_type *) 0
		       ) == ERROR_INT ) {

	/* No layout id to match with */
	layout_ids_match = BOOL_false;

    } else {

	layout_ids_match = ISODE_MatchIDs(layout_id1,layout_id2);

    }

    /* check if logical content ids match */

    if ( GetAttr( c1,
		  at_CONTENT_ID_LOGICAL,
		  (POINTER_type) &logical_id1,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
		) == ERROR_INT ) {

	/* No logical id to match with */
	logical_ids_match = BOOL_false;

    } else if ( GetAttr( c2,
			 at_CONTENT_ID_LOGICAL,
			 (POINTER_type) &logical_id2,
			 /* default */ BOOL_false,
			 /* mask */ (PARM_MASK_type *) 0
		       ) == ERROR_INT ) {

	/* No logical id to match with */
	logical_ids_match = BOOL_false;

    } else {

	logical_ids_match = ISODE_MatchIDs(logical_id1,logical_id2);

    }

    /* return BOOL_true if either the logical or layout id's match */

    rval = layout_ids_match || logical_ids_match;

LEAVE:

#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}


BOOL_type ISODE_MatchIDs(s1,s2)
    IN STRING_type s1;
    IN STRING_type s2;
{

    INT_type rval;

    INT_type i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* check lengths first */

    if ( s1->length != s2->length ) {
	rval = BOOL_false;
	goto LEAVE;
    }

    /* now check each character in string */

    for ( i=0; i<s1->length; i++ ) {

	if ( s1->sequence_value.chars[i] != s2->sequence_value.chars[i] ) {

	    /* character mismatch */
	    rval = BOOL_false;
	    goto LEAVE;

	}

    }

    /* All characters match, so return BOOL_true */

    rval = BOOL_true;

LEAVE:

#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}
