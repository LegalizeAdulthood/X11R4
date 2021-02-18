
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn15.c,v 1.1 89/09/21 06:43:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn15.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn15.c,v $
 * Revision 1.1  89/09/21  06:43:23  mss
 * Initial revision
 * 
 * Revision 1.7  89/03/27  13:25:07  mss
 * Fixed some more structure passing
 * 
 * Revision 1.6  89/03/27  12:33:29  mss
 * Eliminated several passed-by-value structures
 * (changed to pass by reference)
 * 
 * Revision 1.5  89/01/24  12:19:07  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.4  88/11/04  01:45:37  mz0k
 * Wrote the length octet for the 'Identifier' field 
 * of the Structured Name
 * 
 * Revision 1.3  88/11/01  14:48:24  mz0k
 * Added VariantScripts
 * 
 * Revision 1.2  88/10/26  19:08:08  mz0k
 * deleted the mistaken EOC for a CHOICE
 * 
 * Revision 1.1  88/10/21  16:31:23  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn15.c,v $";
#endif

#define TK_IMPL_WASN1
#define TK_GVARS_WASN15
#define TK_IMPL_ATTR
#define TK_TKI_CONST
#define TK_TKI_IO
#define TK_TKI_PARMS
#define TK_TKI_FONT
#define TK_TKI_TKERR

#include <wasn1.h>
#include <tags.h>
#include <parms.h>
#include <font.h>

#undef TK_IMPL_WASN1
#undef TK_GVARS_WASN15
#undef TK_IMPL_ATTR
#undef TK_TKI_CONST
#undef TK_TKI_IO
#undef TK_TKI_PARMS
#undef TK_TKI_FONT
#undef TK_TKI_TKERR

/* Forward definitions */
static INT_type WASN1_ModalFontAttrs();
static INT_type WASN1_VariantScripts();
static INT_type WASN1_VariantScript();


INT_type WASN1_Seq9541IdType(od, tag, sequence)
    IN INT_type	    od;
    IN INT_type	    tag;
    SEQUENCE_ISO9541_ID_type	sequence;
{    
	/* SEQUENCE OF CHOICE {
	 identifier		[0] IMPLICIT INTEGER;
	 character-sequence	[1] IMPLICIT OCTET STRING}
       */
    INT_type rval;
    REGISTER INT_type	tagfield, i;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header first */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    for (i=0; i<sequence->length; i++) {
	rval = WASN1_9541IdType(od, EXPLICIT_TAG, &sequence->sequence_value.iso9541_ids[i]);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    
    /* The EOC for the sequence */
    rval = WASN1_EOC(od);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


INT_type WASN1_9541IdType(od, tag, id)
    IN INT_type	    od;
    IN INT_type	    tag;
    ISO9541_ID_type	*id;
{    
	/*  CHOICE {
	 identifier		[0] IMPLICIT INTEGER;
	 character-sequence	[1] IMPLICIT OCTET STRING}
       */
    INT_type rval;
    REGISTER INT_type	tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    /* NOTICE: if IMPLICIT in the calling level, the passed-in tag writes over the called level tags. */
    tagfield = tag;

    switch (id->iso9541_id_tag) {
	case ID_INT_tag:
	    if (tag == EXPLICIT_TAG) {
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 0);
	    }
	    rval = WASN1_Integer(od, tagfield, id->iso9541_id_value.integer);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    break;
	case ID_STR_tag:
	    if (tag == EXPLICIT_TAG) {
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	    }
	    rval = WASN1_SeqBytesInStruct(od,  tagfield, id->iso9541_id_value.octet_string );
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    break;
	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[WASN1_9541IdType]: Invalid id->iso9541_id_tag: 0d%d", (INT_type) id->iso9541_id_tag);
	    dummy = TKERR_EndErrMsg();
#endif
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


INT_type WASN1_StructuredName(od, tag, name)
    IN INT_type	    od;
    IN INT_type	    tag;
    STRUCTURED_NAME_type    *name;
{    
	/* Assume that at entrance to this routine,  the parameter for fontname has been verified to be SPECIFIED.
       */
    INT_type rval;
    REGISTER INT_type	tagfield;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

 
    tagfield = tag;

    switch (name->structured_name_tag) {
	case STRUCTURED_NAME_ID_tag:
	    if (tag == EXPLICIT_TAG) {
		tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 1);
	    }

	    /* NOTICE: We assume that the definition for the 'Identifier' in the Structured-Name is a MISTAKE.  It should not be an IMPLICIT. */


	    /* Thus let's write the header for the structured-name first. */
	    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }  

	    /* Then we can write the Identifier itself. */
	    rval = WASN1_9541IdType(od, EXPLICIT_TAG, &name->structured_name_value.id);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    
	    /* The EOC for the structured name */
	    rval = WASN1_EOC(od);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    break;
	    case STRUCTURED_NAME_SEQ_tag:
		if (tag == EXPLICIT_TAG) {
		    tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
		}
		rval = WASN1_Seq9541IdType(od, tagfield, name->structured_name_value.idseq);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		break;
	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[WASN1_StructuredName]: Invaid  name->structured_name_tag: 0d%d", (INT_type) name->structured_name_tag);
		dummy = TKERR_EndErrMsg();
#endif
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


INT_type WASN1_SeqIntsInStruct(od, tag, seq_int)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN SEQUENCE_INT_type seq_int;
{
	/* Sequence_int in a structure. */
    
    INT_type rval;
    REGISTER INT_type	tagfield;
    REGISTER INT_type   i;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    tagfield = MakeTag(UNIVERSAL_CLASS, PRIMITIVE, INTEGER_TAG);
    for	(i=0; (i<seq_int->length) && (rval != ERROR_INT); i++) {
	rval = WASN1_Integer(od, tagfield, seq_int->sequence_value.ints[i]);
    }
    
    /* Close the set */
    if (rval !=	ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
    }

INT_type WASN1_Modes(od, tag, modes)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN WRITING_MODES_type   *modes;
{
	/* Sequence_int in a structure. */
    
    INT_type rval;
    REGISTER INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Again, there is no default name prefix */

    if (modes->iso_0.specified != BOOL_false) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_ModalFontAttrs(od, tagfield, &modes->iso_0);
    }
    
    if ((rval != ERROR_INT) && (modes->iso_180.specified != BOOL_false)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WASN1_ModalFontAttrs(od, tagfield, &modes->iso_180);
    }
    
    if ((rval != ERROR_INT) && (modes->iso_270.specified != BOOL_false)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_ModalFontAttrs(od, tagfield, &modes->iso_270);
    }

    /* Ignoring the non-iso-modes for now. */
    
    /* Close the set */
    if (rval !=	ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
    }

static INT_type WASN1_ModalFontAttrs(od, tag, modal)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN MODAL_FONT_ATTR_type *modal;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Skip over everything except the escapement-class */

    if (modal->esclass != ISO_ESCLASS_unspec) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, PRIMITIVE, (INT_type) 2);
	rval = WASN1_Integer(od, tagfield, modal->esclass);
    }
    
    if (modal->varscrpt.specified != BOOL_false) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 18);
	rval = WASN1_VariantScripts(od, tagfield, &modal->varscrpt);
    }

    /* Close the set */
    if (rval !=	ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
    }

static INT_type WASN1_VariantScripts(od, tag, scripts)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN VARIANT_SCRIPTS_type *scripts;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Forget about the default-name-prefix */
    
    if (scripts->lscript.specified != BOOL_false) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_VariantScript(od, tagfield, &scripts->lscript);
    }
    if ((rval != ERROR_INT) && (scripts->rscript.specified != BOOL_false)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WASN1_VariantScript(od, tagfield, &scripts->rscript);
    }
    
    /* The non-ISO-scripts is not implemented */

    /* Close the set */
    if (rval !=	ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
    }

static INT_type WASN1_VariantScript(od, tag, script)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN VARIANT_SCRIPT_type *script;
{
    
    INT_type rval;
    REGISTER INT_type	tagfield;

#ifdef DEBUG
    if (TraceRoutine() ) {
	Entering();
    }
#endif

    tagfield = tag;
    if (tag == EXPLICIT_TAG) {
	tagfield = MakeTag(UNIVERSAL_CLASS, CONSTRUCTED, SEQUENCE_TAG);
    }

    /* Write the sequence header */
    rval = WASN1_Header(od, tagfield, INDEFINITE_LENGTH);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    if (script->vxoffset.second != 0) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 0);
	rval = WASN1_Real(od, tagfield, &script->vxoffset);
    }
    if ((rval != ERROR_INT) && (script->vyoffset.second != 0)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 1);
	rval = WASN1_Real(od, tagfield, &script->vyoffset);
    }
    if ((rval != ERROR_INT) && (script->vxscale.second != 0)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 2);
	rval = WASN1_Real(od, tagfield, &script->vxscale);
    }
    if ((rval != ERROR_INT) && (script->vyscale.second != 0)) {
	tagfield = MakeTag(CONTEXT_SPECIFIC_CLASS, CONSTRUCTED, (INT_type) 3);
	rval = WASN1_Real(od, tagfield, &script->vyscale);
    }

    /* Close the set */
    if (rval !=	ERROR_INT) {
	rval = WASN1_EOC(od);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
    }
