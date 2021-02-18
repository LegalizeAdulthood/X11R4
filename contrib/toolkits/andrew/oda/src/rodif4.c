
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif4.c,v 1.4 89/11/01 15:57:02 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif4.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif4.c,v $
 * Revision 1.4  89/11/01  15:57:02  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.3  89/11/01  11:38:32  jr
 * Remove some cast's of nextbyte.
 * 
 * Revision 1.2  89/10/31  14:10:12  jr
 * Lots of casting errors fixed.
 * 
 * Revision 1.1  89/09/21  06:38:37  mss
 * Initial revision
 * 
 * Revision 1.7  89/09/12  09:15:48  mss
 * Removed #
 * 
 * Revision 1.6  89/01/24  12:17:48  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.5  88/10/31  16:35:35  mz0k
 * This time the Variants stuff works.
 * 
 * Revision 1.3  88/10/27  15:14:32  mz0k
 * Set the Specified tag for the modes stuff
 * 
 * Revision 1.2  88/10/26  19:07:19  mz0k
 * added details about writing_modes
 * 
 * Revision 1.1  88/10/21  16:59:29  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif4.c,v $";
#endif

/* Put .c template comment here */


#define TK_IMPL_RODIF
#define TK_IMPL_RASN1
#define TK_GVARS_RODIF4
#define TK_TKI_DOC
#define TK_TKI_ATTR
#define TK_IMPL_CONST
#define TK_TKI_IO
#define TK_TKI_PARMS
#define TK_TKI_TAGS
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_TKI_STR
#define TK_TKI_TKERR
#define TK_TKI_FONT

#include <font.h>
#include <rodif.h>

#undef TK_IMPL_RODIF
#undef TK_IMPL_RASN1
#undef TK_GVARS_RODIF4
#undef TK_TKI_DOC
#undef TK_TKI_ATTR
#undef TK_IMPL_CONST
#undef TK_TKI_IO
#undef TK_TKI_PARMS
#undef TK_TKI_TAGS
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_TKI_STR
#undef TK_TKI_TKERR
#undef TK_TKI_FONT

/* Forward declarations */
static INT_type RODIF_DocumentCharacteristics();
static INT_type RODIF_AddDocCharacteristics();
static INT_type RODIF_FontsList();
static INT_type RODIF_FontList();
static INT_type RODIF_FontReference();
static INT_type RODIF_WritingModes();
static INT_type RODIF_ModalFontAttrs();
static INT_type RODIF_VariantScripts();
static INT_type RODIF_VariantScript();

INT_type RODIF_DocumentProfile(id)
    IN INT_type	    id;
{ 
    /* 
       Read the Document Profile part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
            */

	INT_type		count;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	CONSTITUENT		profile;
	INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, &count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { count = rval;  }
    
    /* Make a presentation style first */
    profile = MakeDocumentProfile(doc);
    if (profile == ERROR_CONSTITUENT)  {
	rval = ERROR_INT;
	goto LEAVE;
    }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (count == 0)) {
	    break;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	count--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can onlu be one of the following:
	 80 -- generic layout structure;
	 81 -- specific layout structure;
	 84 -- genreic logical structure;
	 85 -- specific logical structure;
	 86 -- presentation styles;
	 87 -- layout styles;
	 a9 -- external document class;
	 aa -- resource document;
	 ab -- resources;
	 13 -- resource name;
	 41 -- object class identifier;
	 a2 -- document characteristics;
	 a3 -- document management attributes;
	 Also,	00 -- a	possible    EOC.
	 */

	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length .*/
		    if (isdeflen)  {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_DocumentProfile]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_DocumentProfile]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		    /* Let's forget about most of the stuffs here except for the fonts_list */
		case 0xa2:
		    rval = RODIF_DocumentCharacteristics(id, &count, profile);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_DocumentProfile]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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



static INT_type RODIF_DocumentCharacteristics(id, count, profile)
    IN INT_type	    id;
    IN INT_type	    *count;
    IN OUT CONSTITUENT profile;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can only be one of the following:
	 (we skip over everything to the additional-doc-characteristics)
	 a9 -- additional-doc-characteristics;
	 Also,  00 -- a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_DocumentCharacteristics]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_DocumentCharacteristics]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa9:
		    rval = RODIF_AddDocCharacteristics(id, &loccount, profile);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_DocumentCharacteristics]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_AddDocCharacteristics(id, count, profile)
    IN INT_type	    id;
    IN INT_type	    *count;
    IN OUT CONSTITUENT profile;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	INT_PAIR_type		int_pair_pr;
	PARM_MASK_type		parm_mask;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can only be one of the following:
	 a3 -- unit scaling;
	 a2 -- fonts-list;
	 Also,  00 -- a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_AddDocCharacteristics]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_AddDocCharacteristics]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa3:
		    rval = RASN1_GetIntPair(id, &loccount, &int_pair_pr, dp_UNIT_SCALING, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(profile, dp_UNIT_SCALING, (POINTER_type) &int_pair_pr, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa2:
		    rval = RODIF_FontsList(id, &loccount, profile);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_DocumentCharacteristics]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_FontsList(id, count, profile)
    IN INT_type	    id;
    IN INT_type	    *count;
    IN OUT CONSTITUENT profile;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	SEQUENCE_FONT_PAIR_type	font_pairs;
	FONT_PAIR_type		font_pair;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }

    /* Make a sequence for the SEQUENCE_FONT_PAIR_type */
    font_pairs = MakeSequence(SEQUENCE_FONT_PAIR_tag, (INT_type) 0);
    if (font_pairs == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can only be one of the following:
	 31 -- SET { 
	    font-identifier;
	    font-reference
	 }
	 
	 Also,  00 -- a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_FontsList]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_FontsList]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x31:
		    rval = RODIF_FontList(id, &loccount, &font_pair);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }

		    rval = ExpandSequence(font_pairs, (INT_type) 1);
		    if (rval == ERROR_INT) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    font_pairs->sequence_value.font_pairs[font_pairs->length-1] = font_pair;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_FontsList]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* We've got the sequence of font_pairs,  set the attribute */
    rval = SetAttr(profile, dp_FONTS_LIST, (POINTER_type) font_pairs, ALL_PARMS);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


	
static INT_type RODIF_FontList(id, count, font_pair)
    IN INT_type	    id;
    IN INT_type	    *count;
    OUT	FONT_PAIR_type	*font_pair;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can only be one of the following:
	 02 -- font identifier;
	 30 -- font reference;
	 
	 Also,  00 -- a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_FontList]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_FontList]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x02:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    font_pair->font_id = rval;
		    break;
		case 0x30:
		    rval = RODIF_FontReference(id, &loccount, &(font_pair->font_reference));
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_FontList]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_FontReference(id, count, reference)
    IN INT_type	    id;
    IN INT_type	    *count;
    OUT	FONT_REFERENCE_type *reference;
{ 
    /* This returns a brand bew font reference,  with all fields ubspecified at the beginning. 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	INT_PAIR_type		int_pair;
	PARM_MASK_type		parm_mask;
	SEQUENCE_INT_type	seqints;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    go_on = BOOL_true;

    /* Initialize the font reference */
    rval = UnspecifyFont(reference);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can only be one of the following:
	 (Skip over something to the fields we need most!!! )
	 a5, or 85 -- family name;
	 86 -- posture;
	 87 -- weight;
	 88 -- proportionate width;
	 ac -- design size;
	 af -- design classification;
	 90 -- structure;
	 b2 -- writing modes;

	 Also 00 -- possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_FontReference]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_FontReference]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa5:
		    rval = RASN1_StructuredName(id, &loccount, &(reference->family), BOOL_true, (INT_type) 1);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    } 
		    break;
		case 0x85:
		    rval = RASN1_StructuredName(id, &loccount, &(reference->family), BOOL_true, (INT_type) 2);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    } 
		    break;
		case 0x86:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    reference->posture = rval;
		    break;
		case 0x87:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    reference->weight = rval;
		    break;
		case 0x88:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    reference->propwdth = rval;
		    break;
		case 0xac:
		    rval = RASN1_GetIntPair(id,	&loccount, &int_pair, dp_FONTS_LIST, &parm_mask);  
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    reference->dsnsize = int_pair;
		    break;
		case 0xaf:
		    seqints = MakeSequence(SEQUENCE_INT_tag, (INT_type) 0);
		    if (seqints == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = RODIF_GetSequenceInt(id, &loccount, seqints);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    reference->dsclass = seqints;
		    break;
		case 0x90:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    reference->structur = rval;
		    break;
		case 0xb2:
		    rval = RODIF_WritingModes(id, &loccount, &(reference->modes), BOOL_true);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    reference->modes.specified = BOOL_true;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_FontReference]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_WritingModes(id, count, modes, isimplicit)
    IN INT_type	    id;
    IN INT_type	    *count;
    OUT	WRITING_MODES_type  *modes;
    IN BOOL_type    isimplicit;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    if (!isimplicit) {
	/* Read the UNIVERSAL sequence tag itself,  presumably 0x30 */
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;
    }

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The nextbyte can only be one of the following:
	 a0  --  default name prefix;
	     a1 -- iso-0;
	     a2 -- iso-180;
	     a3 -- iso-270;
	     a4 -- non-iso-modes;
	     Also 00 -- possible EOC.
	       */


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_WritingModes]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_WritingModes]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa1:
		    rval = RODIF_ModalFontAttrs(id, &loccount, &(modes->iso_0), BOOL_true);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    } 
		    modes->iso_0.specified = BOOL_true;
		    break;
		case 0xa2:
		    rval = RODIF_ModalFontAttrs(id, &loccount, &(modes->iso_180), BOOL_true);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    } 
		    modes->iso_180.specified = BOOL_true;
		    break;
		case 0xa3:
		    rval = RODIF_ModalFontAttrs(id, &loccount, &(modes->iso_270), BOOL_true);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    } 
		    modes->iso_270.specified = BOOL_true;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_WritingModes]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_ModalFontAttrs(id, count, font, isimplicit)
    IN INT_type	    id;
    IN INT_type	    *count;
    OUT	MODAL_FONT_ATTR_type	*font;
    IN BOOL_type    isimplicit;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    if (!isimplicit) {
	/* Read the UNIVERSAL sequence tag itself,  presumably 0x30 */
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;
    }

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The nextbyte can only be one of the following:
	 (skip over someting to the one we need most.)
	 82 -- escapement class;
	 b2 -- variant scripts;
	 Also 00 -- possible EOC.
	   */


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_ModalfontAttrs]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_ModalFontAttrs]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x82:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    font->esclass = rval;
		    break;
		case 0xb2:
		    rval = RODIF_VariantScripts(id, &loccount, &(font->varscrpt), BOOL_true);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_ModalfontAttrs]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_VariantScripts(id, count, scripts, isimplicit)
    IN INT_type	    id;
    IN INT_type	    *count;
    OUT	VARIANT_SCRIPTS_type	*scripts;
    IN BOOL_type    isimplicit;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    scripts->specified = BOOL_true;

    if (!isimplicit) {
	/* Read the UNIVERSAL sequence tag itself,  presumably 0x30 */
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;
    }

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The nextbyte can only be one of the following:
	 a0 -- default name prefix;
	 a1 -- leftward;
	 a2 -- eightward;
	 a3 -- non-iso-scripts;
	 Also 00 -- possible EOC.
	   */


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_VariantScripts]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_VariantScripts]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_VariantScripts]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x.  Umplemented yet.", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		case 0xa1:
		    rval = RODIF_VariantScript(id, &loccount, &(scripts->lscript), BOOL_true);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa2:
		    rval = RODIF_VariantScript(id, &loccount, &(scripts->rscript), BOOL_true);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa3:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_VariantScripts]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x.  Umplemented yet.", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_VariantScripts]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
static INT_type RODIF_VariantScript(id, count, script, isimplicit)
    IN INT_type	    id;
    IN INT_type	    *count;
    OUT	VARIANT_SCRIPT_type	*script;
    IN BOOL_type    isimplicit;
{ 
    /* 
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	INT_PAIR_type		int_pair;
	PARM_MASK_type		parm_mask;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    script->specified = BOOL_true;

    if (!isimplicit) {
	/* Read the UNIVERSAL sequence tag itself,  presumably 0x30 */
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;
    }

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The nextbyte can only be one of the following:
	 a0 -- x-offset;
	 a1 -- y-offset;
	 a2 -- x-scale;
	 a3 -- y-scale;
	 Also 00 -- possible EOC.
	   */


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_VariantScript]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_VariantScript]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    rval = RASN1_GetIntPair(id,	&loccount, &int_pair, dp_FONTS_LIST, &parm_mask);  
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    script->vxoffset = int_pair;
		    break;
		case 0xa1:
		    rval = RASN1_GetIntPair(id,	&loccount, &int_pair, dp_FONTS_LIST, &parm_mask);  
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    script->vyoffset = int_pair;
		    break;
		case 0xa2:
		    rval = RASN1_GetIntPair(id,	&loccount, &int_pair, dp_FONTS_LIST, &parm_mask);  
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    script->vxscale = int_pair;
		    break;
		case 0xa3:
		    rval = RASN1_GetIntPair(id,	&loccount, &int_pair, dp_FONTS_LIST, &parm_mask);  
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    script->vyscale = int_pair;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_VariantScript]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
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


	
