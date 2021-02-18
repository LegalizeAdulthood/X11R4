
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq1.c,v 1.6 89/11/01 09:12:36 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	seq1.c,v $
 * Revision 1.6  89/11/01  09:12:36  mss
 * Changed call to ATTR_FreeNameOrg to pass ptr instead of struct
 * 
 * Revision 1.5  89/11/01  07:02:50  mss
 * Fixed parenthesation in sizeof call
 * 
 * Revision 1.4  89/10/31  22:39:52  mss
 * Fixed casting of sizeof so that type precedes use
 * 
 * Revision 1.3  89/10/31  14:30:15  jr
 * Lots of casting errors fixed.
 * 
 * Revision 1.2  89/10/24  15:38:12  jr
 * Add some type coercions
 * for PC.
 * 
 * Revision 1.1  89/09/21  06:39:12  mss
 * Initial revision
 * 
 * Revision 1.49  89/08/02  17:24:35  annm
 * GOK
 * 
 * Revision 1.48  89/05/13  16:46:42  mss
 * Trashed sequence header when deleted to help find misuses
 * 
 * Revision 1.47  89/05/04  22:20:57  mss
 * Fixed DeleteSequence so that it correctly handled 0
 * length sequences; changed DeleteSequenceDeeply to reuse
 * DeleteSequence
 * 
 * Revision 1.46  89/04/21  18:17:12  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.45  89/04/11  15:31:32  annm
 * copy sequence support for the atomic string expr's and binding pair sequences
 * 
 * Revision 1.44  89/03/16  15:30:10  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.43  89/02/24  18:47:07  annm
 * changed REVISOR to REVISER
 * 
 * Revision 1.42  89/02/09  15:08:41  annm
 * can now copy sequences for string or id's
 * 
 * Revision 1.41  89/02/01  18:01:56  annm
 * can now copy sequences of resource maps
 * 
 * Revision 1.40  89/01/31  19:06:22  annm
 * can now copy SEQUENCE_STRING's and SEQUENCE_COPYRIGHT_INFO's and SEQUENCE_NAME_ORG's
 * 
 * 
 * Revision 1.39  89/01/26  18:19:04  annm
 * can now copy sequences of asn1 object id types
 * 
 * Revision 1.38  89/01/26  12:38:24  annm
 * added attr20.[ch] and dependency of attr.h on attr20.h
 * 
 * Revision 1.37  89/01/24  12:18:03  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.36  89/01/11  10:10:35  mss
 * Got rid of mentions of SEQUENCE_FILING_tag
 * 
 * Revision 1.35  88/12/13  19:56:33  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.34  88/10/17  12:49:59  annm
 * fixed compile bugs in copy sequence elements 2
 * 
 * Revision 1.33  88/10/14  16:10:50  annm
 * can now copy sequences for font pairs and iso9541id's
 * 
 * Revision 1.32  88/09/25  15:36:40  mz0k
 * JR fixed CopySeq for the Construction_terms
 * 
 * Revision 1.31  88/09/20  17:14:08  jr
 * Forward decl SEQ_CopyTabStop
 * 
 * Revision 1.30  88/09/13  14:37:24  mz0k
 * Fixed the AppendSequence.
 * 
 * Revision 1.28  88/09/06  13:13:03  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.27  88/09/06  12:02:53  annm
 * no change
 * 
 * Revision 1.26  88/08/26  16:26:25  annm
 * can now copy sequences of int pairs
 * 
 * Revision 1.25  88/08/22  19:44:49  annm
 * added ability to copy sequences of tab stops
 * 
 * Revision 1.24  88/08/16  02:51:54  mss
 * Added SEQ_MakeSequence; changed MakeSequence to use it
 * 
 * Revision 1.23  88/08/03  13:23:49  jr
 * Remove fiorw defn (duhhhh)
 * 
 * Revision 1.22  88/08/03  13:19:35  jr
 * Fix up forward defn's.
 * 
 * Revision 1.21  88/07/26  13:32:17  jr
 * Add appendsequencew
 * 
 * Revision 1.20  88/07/15  18:23:45  annm
 * made CopySequenceElements callable from outside the SEQ module
 * for use in the copy of sequences in the aunion.h canonical copy procedure
 * 
 * Revision 1.19  88/07/11  12:06:52  annm
 * typo
 * 
 * Revision 1.18  88/07/11  11:44:39  annm
 * added the 3 new seq's to the second level subroutines (added to the
 * switch statements.)
 * 
 * Revision 1.17  88/07/05  16:27:13  jr
 * Remove refs to at_LAYOUT_TEXTURE
 * 
 * Revision 1.16  88/06/17  15:07:19  annm
 * wrote ContractSequence; buffed rough edges in ExpandSequence
 * (TKError setting on can't realloc); can not copy sequences of resource maps
 * 
 * Revision 1.15  88/06/08  16:58:21  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.14  88/06/08  14:37:28  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq1.c,v $";
#endif

/* Put .c template comment here */

#define TK_IMPL_SEQ
#define TK_IMPL_ATTR
#define TK_GVARS_SEQ1
#define TK_TKI_MM
#define TK_TKI_STR
#define TK_TKI_TKERR

#include <rnames.h> 

#include <seq.h>
#include <attr.h>
#include <mm.h>
#include <str.h>
#include <values.h>
#include <apityp.h>
#include <parms.h>
#include <tkerr.h>

/* Forward declarations */
static INT_type	SequenceElementSize();
static INT_type	SetSeqValueField();
static INT_type	SEQ_CopyTabStop();
static POINTER_type	GetSeqValueField();

INT_type AppendSequence(s1, s2)
    IN OUT SEQUENCE_type    s1;
    IN     SEQUENCE_type    s2;
{
    INT_type	rval;	    /* the returned value */
    INT_type	oldlen;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters for legality */
    if (s1==NULL_SEQUENCE || s1==ERROR_SEQUENCE ||
	    s2==NULL_SEQUENCE || s2==ERROR_SEQUENCE) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* Special check for s2 empty */
    if (s2->length == 0) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }


    /* Expand s1 by length of s2 */
    oldlen = s1 -> length;
    rval = ExpandSequence(s1, s2->length);
    if (rval != ERROR_INT) {
	rval = SEQ_CopySeqElts2(s2, (INT_type) 0, s1, oldlen);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type ContractSequence(sequence, amount)
    IN OUT SEQUENCE_type    sequence;
    IN     INT_type	    amount;
{
    INT_type	rval;	    /* the returned value */

    INT_type	    eltsize;    /* Size of each element */
    POINTER_type    value;
    POINTER_type    p;
    INT_type	    OldLength;
    INT_type	    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */

    if( sequence == ERROR_SEQUENCE || sequence == NULL_SEQUENCE ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    if (amount < 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* Special case: contraction of 0 */
    if (amount == 0) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Have to do an contraction, determine element size */
    eltsize = SequenceElementSize(sequence->sequence_tag);
    if (eltsize == ERROR_INT) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* Now get it reallocated */
    value = GetSeqValueField(sequence);	/* Size of each element */
    if (value == ERROR_POINTER) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    OldLength = sequence->length;
    sequence -> length -= amount;	/* subtract off increment */
    p = MM_Realloc(value, (sequence->length)*eltsize);
    if (p == ERROR_POINTER) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	goto LEAVE;
    }
    dummy = SetSeqValueField(sequence, p);
    if (dummy == ERROR_INT) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	goto LEAVE;
    }
    rval = OldLength;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

SEQUENCE_type CopySequence(sequence)
    IN OUT SEQUENCE_type    sequence;
{
    SEQUENCE_type	rval;	/* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Create a new sequence of the proper length */
    rval = MakeSequence(sequence->sequence_tag, sequence->length);
    if (rval == ERROR_SEQUENCE) {
	goto LEAVE;
    }

    /* Now copy elements */
    if (SEQ_CopySequenceElements(/*from*/ sequence, /*to*/ rval) == ERROR_INT) {
	/* Clean up new sequence ? */
	rval = ERROR_SEQUENCE;
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



INT_type DeleteSequence(sequence)
    IN  SEQUENCE_type    sequence;
{

    /*
	Strategy:
	    1) first check the sequence to ensure that it is not
		ERROR_SEQUENCE or (SEQUENCE_type) 0
	    2) tirvially succeed for NULL_SEQUENCE
	    3) free the body of the sequence (the sequence_value
		part)
	    4) free the sequence header (the part with the 
		length, tag etc in it).

	N.B. This does not delete the stuff contained in the sequence
	    value. DeleteSequenceDeeply will free the contained stuff.
    */

    INT_type	rval;	    /* the returned value */

    POINTER_type ptr;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* check the sequence value */
    if( sequence == ERROR_SEQUENCE || 
	sequence == (SEQUENCE_type) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DeleteSequence]: the sequence has value ERROR_SEQUENCE or (SEQUENCE_type) 0" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( sequence == NULL_SEQUENCE ){
	goto LEAVE;
    }

    /* See if any elements to recover */
    if (sequence->length > 0) {
        ptr = GetSeqValueField( sequence );

        rval = MM_Free( ptr );
        if( rval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[DeleteSequence]: can not free the body of the sequence" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
        }
    }

    /* Trash the header as much as possible so that attempts 
      use it will fail quickly */
    sequence->length = 99999; /* Some random large number so loops will fire */
    sequence->sequence_value.values = (VALUE_type *) 0; /* empty pointer */
    sequence->sequence_tag = -1; /* illegal sequence tag */

    /* now free the sequence structure */
    rval = MM_Free( (POINTER_type) sequence );
    if( rval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DeleteSequence]: can not delete the sequence structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}



INT_type DeleteSequenceDeeply(sequence)
    IN  SEQUENCE_type    sequence;
{

    /*
	Strategy:
	    1) first check the sequence to ensure that it is not
		ERROR_SEQUENCE or (SEQUENCE_type) 0
	    2) trivially succeed for NULL_SEQUENE
	    3) enter a loop to free the stuff pointed to from  the body 
		of the sequence
	    4) free the body of the sequence (the sequence_value
		part)
	    5) free the sequence header (the part with the 
		length, tag etc in it).

	N.B. This does not delete the stuff contained in the sequence
	    value. DeleteSequenceDeeply will free the contained stuff.
    */

    INT_type	rval;	    /* the returned value */

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* check the sequence value */
    if( sequence == ERROR_SEQUENCE ||
	sequence == (SEQUENCE_type) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[DeleteSequenceDeeply]: the sequence has value ERROR_SEQUENCE or (SEQUENCE_type) 0" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( sequence == NULL_SEQUENCE ){
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < sequence->length; i++ ){
    
	switch( sequence->sequence_tag ){

	case SEQUENCE_ASN1_OBJECT_ID_tag:
	    SlaveRval = ATTR_FreeAsn1ObjectId( 
		    sequence->sequence_value.asn1_object_ids[i] );
	    break;

	case SEQUENCE_ATOMIC_STR_EXPR_tag:
	    SlaveRval = ATTR_FreeAtomicStrExpr(
		    &sequence->sequence_value.atomic_string_exprs[i] );
	    break;

	case SEQUENCE_BINDING_PAIR_tag:
	    SlaveRval = ATTR_FreeBindingPair(
		    &sequence->sequence_value.binding_pairs[i] );
	    break;

	case SEQUENCE_BYTE_tag:
	    continue;

	case SEQUENCE_CHAR_tag:
	    continue;

	case SEQUENCE_CONSTITUENT_tag:
	    continue;

	case SEQUENCE_CONSTRUCTION_TERM_tag:
	    SlaveRval = CONSTRUCTION_TERM_Free(
		    &sequence->sequence_value.construction_terms[i] );
	    break;

	case SEQUENCE_COPYRIGHT_INFO_tag:
	    SlaveRval = ATTR_FreeCopyrightInfo(
		    &sequence->sequence_value.copyright_infos[i] );
	    break;

	case SEQUENCE_FONT_PAIR_tag:
	    rval = ERROR_INT;
	    TKError = UNIMPL_err;
	    goto LEAVE;

	case SEQUENCE_INT_tag:
	    continue;

	case SEQUENCE_INT_PAIR_tag:
	    continue;

	case SEQUENCE_ISO9541_ID_tag:
	    rval = ERROR_INT;
	    TKError = UNIMPL_err;
	    goto LEAVE;

	case SEQUENCE_at_MEDIUM_TYPE_tag:
	    continue;

	case SEQUENCE_NAME_ORG_tag:
	    SlaveRval = ATTR_FreeNameOrg(
			    &sequence->sequence_value.name_orgs[i]);
	    break;

	case SEQUENCE_PERSONAL_NAME_tag:
	    SlaveRval = ATTR_FreePersonalName(
			&sequence->sequence_value.personal_names[i] );
	    break;

	case SEQUENCE_RESOURCE_MAP_tag:
	    SlaveRval = ATTR_FreeResourceMap(
			&sequence->sequence_value.resource_maps[i] );
	    break;

	case SEQUENCE_REVISION_HISTORY_tag:
	    SlaveRval = ATTR_FreeRevisionHistory(
		    &sequence->sequence_value.revision_histories[i] );
	    break;

	case SEQUENCE_REVISER_tag:
	    SlaveRval = ATTR_FreeReviser(
			&sequence->sequence_value.revisers[i] );	
	    break;

	case SEQUENCE_STRING_tag:
	    SlaveRval = ATTR_FreeStrings(
			sequence->sequence_value.strings[i] );
	    break;

	case SEQUENCE_STRING_OR_ID_tag:
	    SlaveRval = ATTR_Free_String_or_id_value(
			&sequence->sequence_value.string_or_ids[i] );
	    break;

	case SEQUENCE_TAB_STOP_tag:
	    SlaveRval = ATTR_FreeTabStop(
			&sequence->sequence_value.tab_stops[i] );
	    break;

	case SEQUENCE_VALUE_tag:
	    continue;

	default:
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DeleteSequenceDeeply]:  bad sequence tag value seen - got " );
	    dummy = TKERR_FormatInt( "%d", (INT_type) sequence->sequence_tag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;

	}

	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DeleteSequenceDeeply]: the above error occurred when freeing element  " );
	    dummy = TKERR_FormatInt( "%d", i );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;

	}
    
    }

    /* Delete the rest of the sequence "shallowly" */
    rval = DeleteSequence( sequence );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}



INT_type ExpandSequence(sequence, amount)
    IN OUT  SEQUENCE_type   sequence;
    IN	    INT_type	    amount;
{
    INT_type	    rval;	/* The returned value */
    INT_type	    eltsize;    /* Size of each element */
    POINTER_type    value;
    POINTER_type    p;
    INT_type	    dummy;
    INT_type	    OldLength;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    if (amount < 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* Special case expansion of 0 */
    if (amount == 0) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Have to do an expansion, determine element size */
    eltsize = SequenceElementSize(sequence->sequence_tag);
    if (eltsize == ERROR_INT) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* Now get it reallocated */
    OldLength = sequence->length;
    if (OldLength != 0) {
	value = GetSeqValueField(sequence);	/* Size of each element */
	if (value == ERROR_POINTER) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
    }
    sequence -> length += amount;	/* Add in increment */
    if (OldLength != 0) {
	p = MM_Realloc(value, (sequence->length)*eltsize);
    } else {
	p = MM_Malloc((sequence->length)*eltsize);
    }
    if (p == ERROR_POINTER) {
	rval = ERROR_INT;
	TKError = NOMEM_err;
	goto LEAVE;
    }
    dummy = SetSeqValueField(sequence, p);
    if (dummy == ERROR_INT) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	goto LEAVE;
    }
    rval = OldLength;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

SEQUENCE_type MakeSequence(type, length)
    IN TAG_type		type;
    IN INT_type		length;
{
    SEQUENCE_type	    rval;	/* The returned value */
    REGISTER INT_type	    eltsize;    /* Size of elements */

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    eltsize = SequenceElementSize(type);
    if (length < 0 || eltsize == ERROR_INT) {
	rval = ERROR_SEQUENCE;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MakeSequence]: bad length or can not get element size" );
	dummy = TKERR_FormatInt(" length = %d", length );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = SEQ_MakeSequence(type, length, (POINTER_type) 0);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

SEQUENCE_type SEQ_MakeSequence(type, length, dataPtr)
    IN TAG_type		type;
    IN INT_type		length;
    IN POINTER_type	dataPtr;
{
    SEQUENCE_type	    rval;	/* The returned value */
    REGISTER INT_type	    eltsize;    /* Size of elements */
    REGISTER POINTER_type   p;		/* Result of MM_Malloc */

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    eltsize = SequenceElementSize(type);
    if (length < 0 || eltsize == ERROR_INT) {
	rval = ERROR_SEQUENCE;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQ_MakeSequence]: bad length or can not get element size" );
	dummy = TKERR_FormatInt(" length = %d", length );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Get the storage for the SEQUENCE_type object */
    p = MM_Malloc((INT_type) sizeof(struct SEQUENCE_struct));
    if (p == ERROR_POINTER) {
	rval = ERROR_SEQUENCE;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQ_MakeSequence]: can not alloc the sequence struct" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = (SEQUENCE_type) p;

    /* Fill in fields */
    rval -> sequence_tag = type;
    rval -> length = length;
    rval -> private_info = (POINTER_type) 0;

    /* Get storage for elements if length > 0 */
    if (length > 0) {
	if (dataPtr != ((POINTER_type) 0)) {
	    p = dataPtr;
	}
	else {
	    p = MM_Malloc(length*eltsize);
	}
	if (p == ERROR_POINTER) {
	    rval = ERROR_SEQUENCE;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQ_MakeSequence]: can not alloc the body" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if (SetSeqValueField(rval, p) == ERROR_INT) {
	    rval = ERROR_SEQUENCE;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQ_MakeSequence]: can not set the sequence value pointer" );
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

    return rval;
}

INT_type SEQ_Final()
{
    INT_type	rval;	    /* the returned value */

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

    return rval;
}

INT_type SEQ_Init()
{
    INT_type	rval;	    /* the returned value */

    static struct SEQUENCE_struct error_sequence;
    static struct SEQUENCE_struct null_sequence;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initialize ERROR_SEQUENCE */
    ERROR_SEQUENCE = &error_sequence;
    NULL_SEQUENCE = &null_sequence;

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Determine size of a sequence's elements */

static INT_type SequenceElementSize(tag)
    IN TAG_type tag;
{
    REGISTER INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (tag) {
	case SEQUENCE_ASN1_OBJECT_ID_tag:
		rval = (INT_type) sizeof(ASN1_OBJECT_ID_type);
		break;
	case SEQUENCE_ATOMIC_STR_EXPR_tag:
		rval = (INT_type) sizeof(ATOMIC_STRING_EXPR_type);
		break;
	case SEQUENCE_BINDING_PAIR_tag:
		rval = (INT_type) sizeof(BINDING_PAIR_type);
		break;
	case SEQUENCE_BYTE_tag:
		rval = (INT_type) sizeof(BYTE_type);
		break;
	case SEQUENCE_CHAR_tag:
		rval = (INT_type) sizeof(CHAR_type);
		break;
	case SEQUENCE_CONSTITUENT_tag:
		rval = (INT_type) sizeof(CONSTITUENT_type);
		break;
	case SEQUENCE_CONSTRUCTION_TERM_tag:
		rval = (INT_type) sizeof(CONSTRUCTION_TERM_type);
		break;
	case SEQUENCE_COPYRIGHT_INFO_tag:
		rval = (INT_type) sizeof(COPYRIGHT_INFO_type);
		break;
	case SEQUENCE_FONT_PAIR_tag:
		rval = (INT_type) sizeof(FONT_PAIR_type);
		break;
	case SEQUENCE_INT_tag:
		rval = (INT_type) sizeof(INT_type);
		break;
	case SEQUENCE_INT_PAIR_tag:
		rval = (INT_type) sizeof(INT_PAIR_type);
		break;
	case SEQUENCE_ISO9541_ID_tag:
		rval = (INT_type) sizeof(ISO9541_ID_type);
		break;
	case SEQUENCE_at_MEDIUM_TYPE_tag:
		rval = (INT_type) sizeof(at_MEDIUM_TYPE_type);
		break;
	case SEQUENCE_NAME_ORG_tag:
		rval = (INT_type) sizeof(NAME_ORG_type);
		break;
	case SEQUENCE_PERSONAL_NAME_tag:
		rval = (INT_type) sizeof(PERSONAL_NAME_type);
		break;
	case SEQUENCE_RESOURCE_MAP_tag:
		rval = (INT_type) sizeof(RESOURCE_MAP_type);
		break;
	case SEQUENCE_REVISION_HISTORY_tag:
		rval = (INT_type) sizeof(REVISION_HISTORY_type);
		break;
	case SEQUENCE_REVISER_tag:
		rval = (INT_type) sizeof(REVISER_type);
		break;
	case SEQUENCE_STRING_tag:
		rval = (INT_type) sizeof(STRING_type);
		break;
	case SEQUENCE_STRING_OR_ID_tag:
		rval = (INT_type) sizeof(STRING_OR_ID_type);
		break;
	case SEQUENCE_TAB_STOP_tag:
		rval = (INT_type) sizeof(TAB_STOP_type);
		break;
	case SEQUENCE_VALUE_tag:
		rval = (INT_type) sizeof(VALUE_type);
		break;
	default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Assign the appropriate union field of a SEQUENCE_type object */

static INT_type SetSeqValueField(seq, p)
    IN OUT SEQUENCE_type    seq;
    IN	   POINTER_type	    p;
{
    REGISTER INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;	/* Assume success */

    switch (seq->sequence_tag) {
	case SEQUENCE_ASN1_OBJECT_ID_tag:
		seq -> sequence_value.asn1_object_ids = (ASN1_OBJECT_ID_type *) p;
		break;
	case SEQUENCE_ATOMIC_STR_EXPR_tag:
		seq -> sequence_value.atomic_string_exprs = (ATOMIC_STRING_EXPR_type *) p;
		break;
	case SEQUENCE_BINDING_PAIR_tag:
		seq -> sequence_value.binding_pairs = (BINDING_PAIR_type *) p;
		break;
	case SEQUENCE_BYTE_tag:
		seq -> sequence_value.bytes = (BYTE_type *) p;
		break;
	case SEQUENCE_CHAR_tag:
		seq -> sequence_value.chars = (CHAR_type *) p;
		break;
	case SEQUENCE_CONSTITUENT_tag:
		seq -> sequence_value.constituents = (CONSTITUENT *) p;
		break;
	case SEQUENCE_CONSTRUCTION_TERM_tag:
		seq -> sequence_value.construction_terms = (CONSTRUCTION_TERM_type *) p;
		break;
	case SEQUENCE_COPYRIGHT_INFO_tag:
		seq->sequence_value.copyright_infos = (struct COPYRIGHT_INFO_struct *) p;
		break;
	case SEQUENCE_FONT_PAIR_tag:
		seq -> sequence_value.font_pairs = (FONT_PAIR_type *) p;
		break;
	case SEQUENCE_INT_tag:
		seq -> sequence_value.ints = (INT_type *) p;
		break;
	case SEQUENCE_INT_PAIR_tag:
		seq -> sequence_value.int_pairs = (INT_PAIR_type *) p;
		break;
	case SEQUENCE_ISO9541_ID_tag:
		seq->sequence_value.iso9541_ids = (struct ISO9541_ID_struct *) p;
		break;
	case SEQUENCE_at_MEDIUM_TYPE_tag:
		seq -> sequence_value.medium_types = (at_MEDIUM_TYPE_type *) p;
		break;
	case SEQUENCE_NAME_ORG_tag:
		seq -> sequence_value.name_orgs = (NAME_ORG_type *) p;
		break;
	case SEQUENCE_PERSONAL_NAME_tag:
		seq->sequence_value.personal_names = (PERSONAL_NAME_type *) p;
		break;
	case SEQUENCE_RESOURCE_MAP_tag:
		seq -> sequence_value.resource_maps = (RESOURCE_MAP_type *) p;
		break;
	case SEQUENCE_REVISION_HISTORY_tag:
		seq -> sequence_value.revision_histories = (REVISION_HISTORY_type *) p;
		break;
	case SEQUENCE_REVISER_tag:
		seq -> sequence_value.revisers = (REVISER_type *) p;
		break;
	case SEQUENCE_STRING_tag:
		seq -> sequence_value.strings = (STRING_type *) p;
		break;
	case SEQUENCE_STRING_OR_ID_tag:
		seq->sequence_value.string_or_ids = (struct STRING_OR_ID_struct *) p;
		break;
	case SEQUENCE_TAB_STOP_tag:
		seq -> sequence_value.tab_stops = (TAB_STOP_type *) p;
		break;
	case SEQUENCE_VALUE_tag:
		seq -> sequence_value.values = (VALUE_type *) p;
		break;
	default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/*
   Obtain a pointer to the appropriate union field of a SEQUENCE_type
   object.
*/

static POINTER_type GetSeqValueField(seq)
    IN OUT SEQUENCE_type    seq;
{
    REGISTER POINTER_type	    rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (seq->sequence_tag) {
	case SEQUENCE_ASN1_OBJECT_ID_tag:
		rval = (POINTER_type) seq -> sequence_value.asn1_object_ids;
		break;
	case SEQUENCE_ATOMIC_STR_EXPR_tag:
		rval = (POINTER_type) seq -> sequence_value.atomic_string_exprs;
		break;
	case SEQUENCE_BINDING_PAIR_tag:
		rval = (POINTER_type) seq -> sequence_value.binding_pairs;
		break;
	case SEQUENCE_BYTE_tag:
		rval = (POINTER_type) seq -> sequence_value.bytes;
		break;
	case SEQUENCE_CHAR_tag:
		rval = (POINTER_type) seq -> sequence_value.chars;
		break;
	case SEQUENCE_CONSTITUENT_tag:
		rval = (POINTER_type) seq -> sequence_value.constituents;
		break;
	case SEQUENCE_CONSTRUCTION_TERM_tag:
		rval = (POINTER_type) seq -> sequence_value.construction_terms;
		break;
	case SEQUENCE_COPYRIGHT_INFO_tag:
		rval = (POINTER_type) seq->sequence_value.copyright_infos;
		break;
	case SEQUENCE_FONT_PAIR_tag:
		rval = (POINTER_type) seq -> sequence_value.font_pairs;
		break;
	case SEQUENCE_INT_tag:
		rval = (POINTER_type) seq -> sequence_value.ints;
		break;
	case SEQUENCE_INT_PAIR_tag:
		rval = (POINTER_type) seq -> sequence_value.int_pairs;
		break;
	case SEQUENCE_ISO9541_ID_tag:
		rval = (POINTER_type) seq->sequence_value.iso9541_ids;
		break;
	case SEQUENCE_at_MEDIUM_TYPE_tag:
		rval = (POINTER_type) seq -> sequence_value.medium_types;
		break;
	case SEQUENCE_NAME_ORG_tag:
		rval = (POINTER_type) seq -> sequence_value.name_orgs;
		break;
	case SEQUENCE_PERSONAL_NAME_tag:
		rval = (POINTER_type) seq->sequence_value.personal_names;
		break;
	case SEQUENCE_RESOURCE_MAP_tag:
		rval = (POINTER_type) seq -> sequence_value.resource_maps;
		break;
	case SEQUENCE_REVISION_HISTORY_tag:
		rval = (POINTER_type) seq -> sequence_value.revision_histories;
		break;
	case SEQUENCE_REVISER_tag:
		rval = (POINTER_type) seq -> sequence_value.revisers;
		break;
	case SEQUENCE_STRING_tag:
		rval = (POINTER_type) seq -> sequence_value.strings;
		break;
	case SEQUENCE_STRING_OR_ID_tag:
		rval = (POINTER_type) seq->sequence_value.string_or_ids;
		break;
	case SEQUENCE_TAB_STOP_tag:
		rval = (POINTER_type) seq -> sequence_value.tab_stops;
		break;
	case SEQUENCE_VALUE_tag:
		rval = (POINTER_type) seq -> sequence_value.values;
		break;
	default:
		rval = ERROR_POINTER;
		TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQ_CopySequenceElements(from, to)
    IN	   SEQUENCE_type    from;
    IN OUT SEQUENCE_type    to;
{
    REGISTER INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SEQ_CopySeqElts2(from, (INT_type) 0, to, (INT_type) 0);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQ_CopySeqElts2(from, fromstart, to, tostart)
    IN	   SEQUENCE_type    from;
    IN	   INT_type	    fromstart;
    IN OUT SEQUENCE_type    to;
    IN	   INT_type	    tostart;
{
    REGISTER INT_type	rval;
    REGISTER INT_type	fromindex;
    REGISTER INT_type	toindex;
    REGISTER INT_type	left;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;	/* to match interface of canonical copy */

    POINTER_type    Ptr;	/* for use in allocation contained structures */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Check compatibility  -- elt type & length */
    if (from->sequence_tag!=to->sequence_tag ||
	    (from->length-fromstart)!=(to->length-tostart)) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	goto LEAVE;
    }

    /* Okay, copy each element */
    toindex = tostart;
    fromindex = fromstart;

    for (left=from->length-fromstart; left>0; left--, toindex++, fromindex++) {

	switch (from->sequence_tag) {

	    case SEQUENCE_ASN1_OBJECT_ID_tag:
		    /*
			See the comment at the beginning of 
			SEQUENCE_ASN1_OBJECT_ID_Copy in attr20.c.
		    */
		    rval = ATTR_AllocASN1ObjectId( 

			(ASN1_OBJECT_ID_type)
			from->sequence_value.asn1_object_ids[fromindex],

			(ASN1_OBJECT_ID_type *)
			&to->sequence_value.asn1_object_ids[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = SEQ_CopySequenceElements( 
			/* source */
			(SEQUENCE_type) from->sequence_value.asn1_object_ids[fromindex],

			/* destination */
			(SEQUENCE_type) to->sequence_value.asn1_object_ids[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_ATOMIC_STR_EXPR_tag:
		    rval = ATTR_AllocAtomicStrExpr(
			    /* original */
			    (ATOMIC_STRING_EXPR_type *) 
			    &from->sequence_value.atomic_string_exprs[fromindex],
			    /* new */
			    (ATOMIC_STRING_EXPR_type *)
			    &to->sequence_value.atomic_string_exprs[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_CopyAtomicStrExpr(
			    /* source */
			    (ATOMIC_STRING_EXPR_type *)
			    &from->sequence_value.atomic_string_exprs[fromindex],
			    /* destination */
			    (ATOMIC_STRING_EXPR_type *)
			    &to->sequence_value.atomic_string_exprs[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_BINDING_PAIR_tag:
		    rval = ATTR_AllocBindingPair(
			    /* original */
			    (BINDING_PAIR_type *)
			    &from->sequence_value.binding_pairs[fromindex],
			    /* new */
			    (BINDING_PAIR_type *)
			    &to->sequence_value.binding_pairs[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_CopyBindingPair(
			    /* source */
			    (BINDING_PAIR_type *)
			    &from->sequence_value.binding_pairs[fromindex],
			    /* destination */
			    (BINDING_PAIR_type *)
			    &to->sequence_value.binding_pairs[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_BYTE_tag:
		    to->sequence_value.bytes[toindex]
			    = from->sequence_value.bytes[fromindex];
		    break;

	    case SEQUENCE_CHAR_tag:
		    to->sequence_value.chars[toindex]
			    = from->sequence_value.chars[fromindex];
		    break;

	    case SEQUENCE_CONSTITUENT_tag:
		    to->sequence_value.constituents[toindex]
			    = from->sequence_value.constituents[fromindex];
		    break;

	    case SEQUENCE_CONSTRUCTION_TERM_tag:
		    rval = CONSTRUCTION_TERM_Alloc(
			    &from->sequence_value.construction_terms[fromindex],
			    &to->sequence_value.construction_terms[toindex]);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = CONSTRUCTION_TERM_Copy(
			    &from->sequence_value.construction_terms[fromindex],
			    &to->sequence_value.construction_terms[toindex]);
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_COPYRIGHT_INFO_tag:
		    /*
			See the comment at the beginning of 
			SEQUENCE_COPYRIGHT_INFO_Copy in attr21.c.
		    */
		    rval = ATTR_AllocCopyright( 

			(COPYRIGHT_INFO_type *)
			&from->sequence_value.copyright_infos[fromindex],

			(COPYRIGHT_INFO_type *)
			&to->sequence_value.copyright_infos[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_CopyCopyRight( 
			/* source */
			(COPYRIGHT_INFO_type *) &from->sequence_value.copyright_infos[fromindex],
			/* destination */
			(COPYRIGHT_INFO_type *) &to->sequence_value.copyright_infos[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_FONT_PAIR_tag:
		    rval = ATTR_AllocFontPairBody(
			    &from->sequence_value.font_pairs[fromindex],
			    &to->sequence_value.font_pairs[toindex]);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = ATTR_FontPairCopy(
			    &from->sequence_value.font_pairs[fromindex],
			    &to->sequence_value.font_pairs[toindex]);
		    if( rval = ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_INT_tag:
		    to->sequence_value.ints[toindex]
			    = from->sequence_value.ints[fromindex];
		    break;

	    case SEQUENCE_INT_PAIR_tag:
		    to->sequence_value.int_pairs[toindex].first =
			from->sequence_value.int_pairs[fromindex].first;
		    to->sequence_value.int_pairs[toindex].second =
			from->sequence_value.int_pairs[fromindex].second;
		    break;

	    case SEQUENCE_ISO9541_ID_tag:
		    rval = ATTR_AllocISO9541Id(
			&from->sequence_value.iso9541_ids[fromindex],
			&to->sequence_value.iso9541_ids[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_Copy_ISO9541_Id_Type(
			&from->sequence_value.iso9541_ids[fromindex],
			&to->sequence_value.iso9541_ids[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_at_MEDIUM_TYPE_tag:
		    dummy_mask = (PARM_MASK_type) 0;
		    rval = at_MEDIUM_TYPE_Copy( 
			(POINTER_type) &from->sequence_value.medium_types[fromindex],
			 /* source */ NOMINAL_PAGE_SIZE_parm | SIDE_OF_SHEET_parm,
			/* wanted */ NOMINAL_PAGE_SIZE_parm | SIDE_OF_SHEET_parm,
			/* dest. mask */ &dummy_mask,
			(POINTER_type) &to->sequence_value.medium_types[toindex] 
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_NAME_ORG_tag:
		    /*
			See the comment at the beginning of 
			SEQUENCE_NAME_ORG_Copy in attr21.c.
		    */
		    rval = ATTR_AllocNameOrg( 

			(NAME_ORG_type *)
			&from->sequence_value.name_orgs[fromindex],

			(NAME_ORG_type *)
			&to->sequence_value.name_orgs[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_CopyNameOrg( 
			/* source */
			(NAME_ORG_type *) &from->sequence_value.name_orgs[fromindex],
			/* destination */
			(NAME_ORG_type *) &to->sequence_value.name_orgs[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_PERSONAL_NAME_tag:
		    rval = ATTR_AllocPersonalName(
			    (PERSONAL_NAME_type *)
			    &from->sequence_value.personal_names[fromindex],
			    (PERSONAL_NAME_type *)
			    &to->sequence_value.personal_names[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_CopyPersonalName(
			    (PERSONAL_NAME_type *)
			    &from->sequence_value.personal_names[fromindex],
			    (PERSONAL_NAME_type *)
			    &to->sequence_value.personal_names[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_RESOURCE_MAP_tag:
		    /*
			See the comment at the beginning of 
			SEQUENCE_RESOURCE_MAP_Copy in attr21.c.
		    */
		    rval = ATTR_AllocResourceMap( 

			(RESOURCE_MAP_type *)
			&from->sequence_value.resource_maps[fromindex],

			(RESOURCE_MAP_type *)
			&to->sequence_value.resource_maps[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    rval = ATTR_CopyResourceMap( 
			/* source */
			(RESOURCE_MAP_type *) &from->sequence_value.resource_maps[fromindex],
			/* destination */
			(RESOURCE_MAP_type *) &to->sequence_value.resource_maps[toindex]
		    );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_REVISION_HISTORY_tag:
		    rval = ATTR_AllocRevisionHistory(
			    (REVISION_HISTORY_type *)
			    &from->sequence_value.revision_histories[fromindex],
			    (REVISION_HISTORY_type *)
			    &to->sequence_value.revision_histories[toindex] );
		    if( rval == ERROR_INT ){
		    goto LEAVE;
		    }
		    rval = ATTR_CopyRevisionHistory(
			    (REVISION_HISTORY_type *)
			    &from->sequence_value.revision_histories[fromindex],
			    (REVISION_HISTORY_type *)
			    &to->sequence_value.revision_histories[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_REVISER_tag:
		    rval = ATTR_AllocReviser(
			    (REVISER_type *)
			    &from->sequence_value.revisers[fromindex],
			    (REVISER_type *)
			    &to->sequence_value.revisers[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    } 
		    rval = ATTR_CopyReviser(
			    (REVISER_type *)
			    &from->sequence_value.revisers[fromindex],
			    (REVISER_type *)
			    &to->sequence_value.revisers[toindex] );
		    if( rval == ERROR_INT ){
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_STRING_tag:
		    
		    Ptr = STRING_Alloc( (POINTER_type) 
			from->sequence_value.strings[fromindex] );
		    if( Ptr == ERROR_POINTER ){
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    to->sequence_value.strings[toindex] =
			(struct SEQUENCE_struct *) Ptr;
		    SlaveRval = STRING_Copy( 
			/* source */ (POINTER_type) from->sequence_value.strings[fromindex],
			/* source mask */ ALL_PARMS,
			/* wanted mask */ ALL_PARMS,
			/* dest mask */ &dummy_mask,
			/* dest */ (POINTER_type) to->sequence_value.strings[toindex] );
		    if( SlaveRval == ERROR_INT ){
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_STRING_OR_ID_tag:

		    SlaveRval = ATTR_Alloc_String_or_id_value( 
			&from->sequence_value.string_or_ids[fromindex],
			&to->sequence_value.string_or_ids[toindex] );
		    if( SlaveRval == ERROR_INT ){
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    SlaveRval = STRING_OR_ID_Copy( 
			/* source */ (POINTER_type) &from->sequence_value.string_or_ids[fromindex],
			/* source mask */ ALL_PARMS,
			/* wanted mask */ ALL_PARMS,
			/* dest mask */ &dummy_mask,
			/* dest */ (POINTER_type) &to->sequence_value.string_or_ids[toindex] );
		    if( SlaveRval == ERROR_INT ){
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_TAB_STOP_tag:
		    SlaveRval =
	    SEQ_CopyTabStop( /* src */ &from->sequence_value.tab_stops[fromindex], 
			     /* dst */ &to->sequence_value.tab_stops[toindex] );
		    if( SlaveRval == ERROR_INT ){
			rval = ERROR_INT;
			TKError = INTERNAL_err;
			goto LEAVE;
		    }
		    break;

	    case SEQUENCE_VALUE_tag:
		    to->sequence_value.values[toindex]
			    = from->sequence_value.values[fromindex];
		    break;

	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
		goto LEAVE;
	}
    }

    /* If here, everything was ok */
    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type SEQ_CopyTabStop( Source, Destination )
    IN TAB_STOP_type *Source;
    OUT TAB_STOP_type *Destination;
{

    INT_type rval;

    STRING_type OldString;
    STRING_type NewString;

    INT_type i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* first copy over the tab reference string */
    OldString = Source->tab_reference;
    if( OldString->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	/* Should free strings here */
	goto LEAVE;
    }

    NewString = MakeString( OldString->length );
    if( NewString == ERROR_STRING ){
	rval = ERROR_INT;
	/* should free strings here */
	goto LEAVE;
    }

    for( i = 0; i < OldString->length; i++){
	NewString->sequence_value.chars[i] = OldString->sequence_value.chars[i];
    }

    Destination->tab_reference = NewString;

    /* next copy the tab position */
    Destination->tab_position = Source->tab_position;

    /* now check and copy (if value ok) the alignment */
    if( Source->alignment != cc_LINE_LAYOUT_TABLE_start_alnd &&
	Source->alignment != cc_LINE_LAYOUT_TABLE_end_alnd &&
	Source->alignment != cc_LINE_LAYOUT_TABLE_centred &&
	Source->alignment != cc_LINE_LAYOUT_TABLE_alnd_arnd ){
	rval = ERROR_INT;
	/* should free strings here */
	TKError = BADPARM_err;
	goto LEAVE;
    }

    Destination->alignment = Source->alignment;

    /*
	 copy over the alignment string if the alignment field is
	algn arnd
    */
    if( Source->alignment == cc_LINE_LAYOUT_TABLE_alnd_arnd ){

	OldString = Source->alignment_string;
	if( OldString->sequence_tag != SEQUENCE_CHAR_tag ){
	    rval = ERROR_INT;
	    /* should free strings here */
	    goto LEAVE;
	}

	NewString = MakeString( OldString->length );
	if( NewString == ERROR_STRING ){
	    rval = ERROR_INT;
	    /* should free strings here */
	    goto LEAVE;
	}
	for( i = 0; i < OldString->length; i++){
	    NewString->sequence_value.chars[i] =
		 OldString->sequence_value.chars[i];
	}
	Destination->alignment_string = NewString;
    }
    else {
	Destination->alignment_string = NULL_STRING;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

