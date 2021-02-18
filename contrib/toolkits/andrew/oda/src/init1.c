
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init1.c,v 1.3 89/10/26 14:10:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	init1.c,v $
 * Revision 1.3  89/10/26  14:10:00  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/10/13  11:33:37  jr
 * Hacks for buggy VMS loader.
 * 
 * Revision 1.1  89/09/21  06:31:01  mss
 * Initial revision
 * 
 * Revision 1.33  89/09/05  14:40:26  mss
 * Removed call of local_init (done by translators as necessary)
 * 
 * Revision 1.32  89/03/27  12:34:17  mss
 * Added initialization/dependencies for ATTS module
 * 
 * Revision 1.31  88/09/06  13:12:01  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.30  88/09/06  12:02:19  annm
 * no change
 * 
 * Revision 1.29  88/08/31  15:28:22  jr
 * LookupObjectId -> LookUpObjectId
 * 
 * Revision 1.28  88/08/31  12:27:12  jr
 * Add LookupObjectId
 * 
 * Revision 1.27  88/08/19  16:09:53  jr
 * Rearrange forward decl's for
 * VMS (still not clear this is
 * the problem, but it can't hurt).
 * 
 * Revision 1.26  88/08/15  18:18:04  annm
 * no change
 * 
 * Revision 1.25  88/08/07  20:39:34  jr
 * Call DEBUG & LABEL init's
 * 
 * Revision 1.24  88/07/28  13:49:59  jr
 * Fix fencepost bug in init'ing of ASN.1
 * Object Id's
 * 
 * Revision 1.23  88/07/08  13:37:00  jr
 * Init ASN1 Object ID's
 * 
 * Revision 1.22  88/06/15  17:27:03  jr
 * Clean up Init routine
 * 
 * Revision 1.21  88/06/08  16:56:47  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.20  88/06/08  14:35:52  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init1.c,v $";
#endif

#define TK_IMPL_INIT
#define TK_GVARS_INIT1
#define TK_TKI_MM
#define TK_TKI_DOC
#define TK_TKI_CONST
#define TK_TKI_ATTR
#define TK_TKI_STR
#define TK_TKI_LOCAL
#define TK_TKI_TKERR
#define TK_TKI_SEQ
#define TK_TKI_DEBUG
#define TK_TKI_LABEL
#define TK_TKI_ATTS

#include <init.h>
#include <mm.h>
#include <acd.h>
#include <str.h>
#include <local.h>
#include <tkerr.h>
#include <seq.h>
#include <debug.h>
#include <label.h>
#include <atts.h>

#undef TK_IMPL_INIT
#undef TK_GVARS_INIT1
#undef TK_TKI_MM
#undef TK_TKI_DOC
#undef TK_TKI_CONST
#undef TK_TKI_ATTR
#undef TK_TKI_STR
#undef TK_TKI_LOCAL
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ
#undef TK_TKI_LABEL
#undef TK_TKI_DEBUG
#undef TK_TKI_ATTS

/* Forward decl's */

/*************
    This forward declaration is commented out because the VMS
    C compiler seemed to be having trouble dealing with the
    forward, link-time reference to InitObjectIds.

INT_type    InitObjectIds();
*************/

static struct {
    ASN1_OBJECT_ID_type	*variable;
    VALUE_type		kind;
    INT_type		asn1[5];
} object_ids[] = {

    { &at_CONTENT_ARCH_CLASS_form_cc,   FORM_CC_id,	{ 2, 8, 2, 6, 0 } },
    { &at_CONTENT_ARCH_CLASS_proc_cc,   PROC_CC_id,	{ 2, 8, 2, 6, 1 } },
    { &at_CONTENT_ARCH_CLASS_fp_cc,	FP_CC_id,	{ 2, 8, 2, 6, 2 } },
    { &at_CONTENT_ARCH_CLASS_form_rg,   FORM_RG_id,	{ 2, 8, 2, 7, 2 } },
    { &at_CONTENT_ARCH_CLASS_fp_rg,	FP_RG_id,	{ 2, 8, 2, 7, 0 } },
    { &at_CONTENT_ARCH_CLASS_form_gg,   FORM_GG_id,	{ 2, 8, 2, 8, 0 } },

    { &at_TYPE_OF_CODING_cc,		CC_CODING_id,   { 2, 8, 3, 6, 0 } },
    { &at_TYPE_OF_CODING_T6,		RG_T6_CODING_id,{ 2, 8, 3, 7, 0 } },
    { &at_TYPE_OF_CODING_T4_1,		RG_T4_1_CODING_id, { 2, 8, 3, 7, 1 } },
    { &at_TYPE_OF_CODING_T4_2,		RG_T4_2_CODING_id, { 2, 8, 3, 7, 2 } },
    { &at_TYPE_OF_CODING_bitmap,	RG_BITMAP_CODING_id, { 2, 8, 3, 7, 3 } },
    { &at_TYPE_OF_CODING_gg,		GG_CODING_id,	{ 2, 8, 3, 8, 0 } },
};

#define NIDS	((INT_type)(sizeof(object_ids) / sizeof(object_ids[0])))

INT_type FinalToolKit()
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

/*
   Initialize predefined ASN.1 OBJECT IDENTIFIER's.
*/

static INT_type InitObjectIds()
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	i;
    REGISTER INT_type	j;
    SEQUENCE_INT_type	id;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    for (i=0; i<NIDS; i++) {
	id = MakeSequence(SEQUENCE_INT_tag, (INT_type) 5);
	if (id == ERROR_SEQUENCE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	for (j=0; j<5; j++) {
	    id->sequence_value.ints[j] = object_ids[i].asn1[j];
	}
	(*object_ids[i].variable) = id;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

VALUE_type LookUpObjectId(id)
   IN ASN1_OBJECT_ID_type   id;
{
	     VALUE_type	rval;
    REGISTER INT_type		i;
    REGISTER INT_type		j;
    REGISTER INT_type		length;
    REGISTER INT_type		*this_id;
    REGISTER BOOL_type		match;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    for (i=0; i<NIDS; i++) {
	if (id->length == (*object_ids[i].variable)->length) {
	    length = id->length;
	    match = BOOL_true;
	    this_id = (*object_ids[i].variable)->sequence_value.ints;
	    for (j=0; j<length; j++) {
		if (id->sequence_value.ints[j] != this_id[j]) {
		    match = BOOL_false;
		    break;
		}
	    }
	    if (match) {
		rval = object_ids[i].kind;
		goto LEAVE;
	    }
	}
    }

    rval = UNKNOWN_id;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type InitToolKit(params)
    IN INIT_PARMS_type *params;
{
    INT_type	rval;	    /* The returned value */
    INT_type	i;
    static INT_type (*InitRouts[])() = {

	    MM_Init,	    CONST_Init,	STR_Init,
	    DOC_Init,	    ATTR_Init,	TKERR_Init,	SEQ_Init,
	    LABEL_Init,	    DEBUG_Init,	ATTS_Init,
	/* Misc initialization */
	    InitObjectIds
    };
#define NROUTS	((INT_type)(sizeof(InitRouts) / sizeof(InitRouts[0])))
	    
/*
   These routines are necessary due to a bug on VMS -- any module
   without a called routine will not be loaded correctly.
*/
    static INT_type (*DummyRouts[])() = {
	    ATTR16_Dummy,   ATTR17_Dummy,   ATTR18_Dummy
    };
#define NDUMMIES    ((INT_type)(sizeof(DummyRouts) / sizeof(DummyRouts[0])))

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /*
	ASSERT: if an error occurs that the appropriate module
	init routine will set up TKError.
	QUESTION: On error how much should be undone ?
    */

    /* Assume that all works */
    rval = SUCCESS_INT;

    /*
       Call the dummy routines -- these do nothing, but must appear to
       be celled for the VMS linker.
    */
    for (i=0; i<NDUMMIES; i++) {
	rval = (*DummyRouts[i])();
	if( rval == ERROR_INT ){
	    goto LEAVE;
	}
    }

    /* remember the init params pointer */
    InitParms = params;

    for (i=0; i<NROUTS; i++) {
	rval = (*InitRouts[i])();
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

    return rval;
}

INT_type INIT_Final()
{
    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

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

INT_type INIT_Init()
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
