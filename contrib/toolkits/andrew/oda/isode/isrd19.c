
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd19.c,v 1.1 89/09/21 05:35:08 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd19.c,v $

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

$Log:	isrd19.c,v $
 * Revision 1.1  89/09/21  05:35:08  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:38:55  mss
 * Initial revision
 * 
 * Revision 1.7  89/09/12  09:35:04  mss
 * Removed #
 * 
 * Revision 1.6  89/08/28  16:51:38  jl3p
 * added copyright notice
 * 
 * Revision 1.5  89/08/24  10:12:37  jl3p
 * removed debug line to print PE after reading it in
 * 
 * Revision 1.4  89/08/16  09:55:23  mss
 * Arranged everything to use global odaiso.h
 * 
 * Revision 1.3  89/08/09  12:56:15  jl3p
 * no changes
 * 
 * 
 * Revision 1.2  89/08/03  21:41:27  mss
 * Changed some ODIF's to ISRD
 * 
 * Revision 1.1  89/08/03  15:45:05  mss
 * Initial revision
 * 
 * Revision 1.5  89/08/01  23:00:02  mss
 * changed ODIF reference to printing routines to ISPR
 * 
 * Revision 1.4  89/07/27  14:52:02  jl3p
 * changed AddToHash routine so that it hashes a permanent copy of the id
 * 
 * Revision 1.3  89/07/14  16:49:54  jl3p
 * fixed the following:
 *    ISODE_Hash function return value corrected,
 *    bucket list traversal in ISODE_LookUpId,
 *    and ISODE_GetHashStructure return value for the empty constituent.
 * 
 * Revision 1.2  89/07/10  14:42:38  annm
 * tweek in Get Sequence of Constituents
 * 
 * Revision 1.1  89/06/21  17:37:39  annm
 * Initial revision
 * 
 * Revision 1.1  89/06/21  16:58:43  annm
 * Initial revision
 * 
*/

#ifdef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd19.c,v $";
#endif

/* toolkit incluces */
#define TK_IMPL_ISRD
#define TK_GVARS_ISRD19
#define TK_IMPL_INIT
#define TK_TKI_TKERR
#define TK_IMPL_CONST
#define TK_TKI_MM

#include <values.h>
#include <apityp.h>
#include <init.h>
#include <acd.h>
#include <iter.h>
#include <isrd.h>
#include <tkerr.h>
#include <mm.h>

#undef TK_IMPL_ISRD
#undef TK_GVARS_ISRD19
#undef TK_IMPL_INIT
#undef TK_TKI_TKERR
#undef TK_IMPL_CONST
#undef TK_TKI_MM

/* UNIX includes */
#include <stdio.h>

/* isode includes */
#include <psap.h>



DOCUMENT_type isode_ReadODIF(InFile) 
    IN FILE		*InFile;
{

    DOCUMENT_type	rval;	/* The returned value */

    INT_type		dummy;

    INT_type		SlaveRval;

    PS			ps;

    PE			pe;

    struct isode2toolkit_t isode2toolkit;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* first make the doucment */
    isode2toolkit.document = MakeDocument();
    if( isode2toolkit.document == ERROR_DOCUMENT ){
	rval = ERROR_DOCUMENT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_ReadODIF]: can not make the document" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = isode2toolkit.document;

    ps = ps_alloc(std_open);
    if( ps == NULLPS ){
	rval = ERROR_DOCUMENT;
	TKError = INTERNAL_err;
	dummy = DeleteDocument( isode2toolkit.document );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_ReadODIF]: can not allocate the ps stream" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = std_setup( ps, InFile );
    if( SlaveRval != OK ){
	rval = ERROR_DOCUMENT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_ReadODIF]: can not perform the ps standard setup" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* initialize the hash system */
    SlaveRval = ISODE_HashInit();

    while( (pe = ps2pe(ps) ) != NULLPE ){
    
	SlaveRval = parse_ISRD1_Interchange__Data__Element(pe, 1, NULLIP,
			NULLVP, &isode2toolkit );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_DOCUMENT;
	    dummy = DeleteDocument( isode2toolkit.document );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[isode_ReadODIF]: can not parse a pe" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	pe_free( pe );
    
    }

    ps_free( ps );

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



/*
 useful routines for dealing with forward references
 and for resolving forward references.
*/

INT_type ISODE_Hash( id )
    IN STRING_type id;
{

    INT_type rval;

    INT_type dummy;

    INT_type i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    if( id->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_Hash]: id is not a sequence of char" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (INT_type) 0;

    for( i = (INT_type) 0; i < id->length; i++ ){

	rval += (INT_type) id->sequence_value.chars[i];
    
    }

    rval %= NHASH;
    
LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



INT_type ISODE_HashInit()
{

    INT_type rval;

    INT_type i;

    static struct hash_t gisteb;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    for( i = (INT_type) 0; i < NHASH; i++ ){

	buckets[i].next = EMPTY_BUCKET;

    }

    /* set up the empty bucket */
    ERROR_BUCKET = &gisteb;
        
LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



struct hash_t *ISODE_LookUpId( id )
    IN STRING_type id;
{

    struct hash_t *rval;

    INT_type dummy;

    INT_type bucket;

    struct hash_t *temp;

    INT_type SlaveRval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    if( id->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_BUCKET;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_LookUp]: id is not a sequence of char" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    bucket = ISODE_Hash( id );
    if( bucket == ERROR_INT ){
	rval = ERROR_BUCKET;
	goto LEAVE;
    }

    rval = ERROR_BUCKET;

    for( temp = buckets[bucket].next; temp != EMPTY_BUCKET;
	 temp = temp->next ){

	SlaveRval = ISODE_STRING_Equal( id, temp->id );

	if( SlaveRval == BOOL_true ){
	    rval = temp;
	    goto LEAVE;
	}

    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



BOOL_type ISODE_STRING_Equal( id1, id2 )
    STRING_type id1;
    STRING_type id2;
{

    BOOL_type rval;

    INT_type i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    if( id1->length != id2->length ){
	rval = BOOL_false;
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < id1->length; i++ ){
    
	if( id1->sequence_value.chars[i] !=
	    id2->sequence_value.chars[i] ){
	    break;
	}
	
    }

    if( i < id1->length ){
	rval = BOOL_false;
    }
    else {
	rval = BOOL_true;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



struct hash_t *ISODE_GetHashStructure( id, document )
    IN STRING_type id;
    IN DOCUMENT_type document;
{

    /*
	Given an id, look it up in the hash system.
	If found return, otherwise make an empty constituent,
	and add it to the hash system.

	This routine is useful when encountering references
	to a single constituent, e.g. a LAYOUT_STYLE attribute
	on a logical object - use this to get the constituent
	for the style.
	
    */    

    struct hash_t *rval;

    CONSTITUENT constituent;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = ISODE_LookUpId( id );
    if( rval != ERROR_BUCKET ){
	/* we found it !!! */
	goto LEAVE;
    }
    
    /* make an empty constituent */
    constituent = CONST_MakeConstituent( document );
    if( constituent == ERROR_CONSTITUENT ){
	rval = ERROR_BUCKET;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_GetHashStructure]: can not make an empty constituent" );
	dummy = TKERR_EndErrMsg();
    }

    SlaveRval = ISODE_AddToHash( constituent, id );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_BUCKET;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_GetHashStructure]: can not add the new constituent to the hash system" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = ISODE_LookUpId( id );

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}

INT_type ISODE_AddToHash( constituent, id )
    IN CONSTITUENT constituent;
    IN STRING_type id;
{

    INT_type rval;

    POINTER_type Ptr;

    struct hash_t   *NewBucket;

    INT_type HashBucket;

    INT_type dummy;


#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    Ptr = MM_Malloc( sizeof( struct hash_t ) );
    if( Ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_GetHashStructure]: can not get memory for the new hash structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }    

    NewBucket = (struct hash_t *) Ptr;

    NewBucket->constituent = constituent;
    NewBucket->id = CopySequence(id);

    /* now add the new bucket to the appropriate hash bucket */

    HashBucket = ISODE_Hash( id );    
    if( HashBucket == ERROR_INT ){
	rval = ERROR_INT;
	dummy = MM_Free( Ptr );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_GetHashStructure]: can not hash the id" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* add to the beginning of bucket */

    /* the new bucket being added points to the current first guy */
    NewBucket->next = buckets[HashBucket].next;

    /* the hash bucket points the the new guy */
    buckets[HashBucket].next = NewBucket;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}



INT_type ISODE_FillInConstituent( constituent, id )
    IN CONSTITUENT constituent;
    IN STRING_type id;
{

    /* 
	This routine is called after processing the body of
	a constiutent. The paradygm is that a constituent
	is always created when the constituent is encountered
	in the data stream. Attributes are attached to this constiutent.

	The problem arises that a reference to this constituent
	(actually its id) may have been seen earlier. If this
	is the case then the 'original constituent' (the one
	created when the reference was seen needs to be filled
	in with the attributes from the consituent created
	while reading the body in the data stream.

	If the constituent (actually the id) has never been
	seen then the constituent needs to be added to the 
	hash system.
    */

    INT_type rval;

    struct hash_t *Hash;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* see if the id is in the hash system */
    Hash = ISODE_LookUpId( id );

    if( Hash == ERROR_BUCKET ){

	/* must add to the hash system */
	SlaveRval = ISODE_AddToHash( constituent, id );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_FillInConstituent]: can not add new constituent to the hash system" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	/* it is in the successfully in the hash system */
    }
    else {

	/* it is in the system already - so fill in the existing constituent */
	SlaveRval = CONST_FillInConstituent( 
			/* the empty */ Hash->constituent,
			/* the full */ constituent );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_FillInConstituent]: can not fill in the empty constituent" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;

}



SEQUENCE_CONSTITUENT_type ISODE_MakeSequenceConstituent( ParentId, Kids, document )
    IN STRING_type ParentId;
    IN SEQUENCE_STRING_type Kids;
    IN DOCUMENT_type document;
{

    /*
	Given the parent's id and the kids, make a sequence of constituents
	to return.

	The returned sequence will be used to set either the 
	at_SUBORDINATES or at_CONTENT_PORTIONS attribute.

    */

    SEQUENCE_CONSTITUENT_type rval;

    INT_type dummy;

    INT_type i;

    STRING_type KidId;

    INT_type SlaveRval;

    struct hash_t *HashStruct;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = MakeSequence( SEQUENCE_CONSTITUENT_tag, (INT_type) 0 );
    if( rval == ERROR_SEQUENCE ){
	rval = ERROR_SEQUENCE;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ISODE_MakeSequenceConstituent]: can not make the empty sequence to start off" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < Kids->length; i++ ){

	KidId = CopySequence( ParentId );
	if( KidId == ERROR_SEQUENCE ){
	    dummy = DeleteSequence( rval );
	    rval = ERROR_SEQUENCE;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_MakeSequenceConstituent]: can not copy the parent's id" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SlaveRval = ExpandSequence( KidId, (INT_type) 1 );
	if( SlaveRval == ERROR_INT ){
	    dummy = DeleteSequence( rval );
	    dummy = DeleteSequence( KidId );
	    rval = ERROR_SEQUENCE;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_MakeSequenceConstituent]: can not expand the kid's id" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	KidId->sequence_value.chars[KidId->length - (INT_type) 1] = ' ';

	SlaveRval = AppendSequence( KidId, Kids->sequence_value.strings[i] );
	if( SlaveRval == ERROR_INT ){
	    dummy = DeleteSequence( rval );
	    dummy = DeleteSequence( KidId );
	    rval = ERROR_SEQUENCE;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_MakeSequenceConstituent]: can not append the kid's id" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	HashStruct = ISODE_GetHashStructure( KidId, document );

	SlaveRval = ExpandSequence( rval, (INT_type) 1 );
	if( SlaveRval == ERROR_INT ){
	    dummy = DeleteSequence( rval );
	    dummy = DeleteSequence( KidId );
	    rval = ERROR_SEQUENCE;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ISODE_MakeSequenceConstituent]: can not expand the constituent sequence" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	rval->sequence_value.constituents[rval->length-1] =
	    HashStruct->constituent;

	dummy = DeleteSequence( KidId );
	
    }
        
LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;


}
