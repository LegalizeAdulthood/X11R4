/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn12.c,v 1.4 89/11/01 15:51:38 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn12.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rasn12.c,v $
 * Revision 1.4  89/11/01  15:51:38  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.3  89/10/31  15:48:37  annm
 * fixing lint problems
 * 
 * Revision 1.2  89/10/25  12:47:48  jr
 * Cast constant parameters for PC.
 * 
 * Revision 1.1  89/09/21  06:36:12  mss
 * Initial revision
 * 
 * Revision 1.17  89/01/18  16:59:55  annm
 * chaged the cc_CHAR_IMAGE_ORIENTATION to cc_CHAR_ORIENTATION
 * 
 * Revision 1.16  88/11/17  15:04:15  mz0k
 * added RASN1_Separation
 * 
 * Revision 1.15  88/10/28  12:56:01  mz0k
 * fixed the tags for the FontType
 * 
 * Revision 1.14  88/10/24  14:11:59  mz0k
 * minors change for 'string'
 * 
 * Revision 1.13  88/10/21  13:44:33  mz0k
 * parm_mask coersed to PARM_MASK_type
 * 
 * Revision 1.12  88/10/18  14:27:47  mz0k
 * added RASN1_Offset
 * 
 * Revision 1.11  88/10/14  17:48:03  mz0k
 * pass in contentmade by reference in RASN1_Asn1ObjectId
 * 
 * Revision 1.10  88/10/13  22:12:17  mz0k
 * deleted RASN1_IntPair
 * 
 * Revision 1.9  88/10/10  14:27:02  mss
 * Fixed Identical so that no incrementing in midst of statement
 * 
 * Revision 1.8  88/10/06  18:24:38  mz0k
 * 1) got rid of the map[]'s;
 * 2) Added the erroe messages;
 * 3) fixed the count stuff.
 * 
 * Revision 1.6  88/09/30  15:58:51  mz0k
 * IO_ReadByte/Bytes --> RASN1_ReadByte/Bytes and deleted the Content_PortionsEnding
 * 
 * Revision 1.5  88/09/25  17:22:14  mz0k
 * mistaken use of MM_Malloc
 * 
 * Revision 1.4  88/09/22  14:54:55  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.3  88/09/13  14:51:12  mz0k
 * changed "" to <> & other stuffs
 * 
 * Revision 1.2  88/09/08  16:57:20  mz0k
 * First version that works.
 * 
 * Revision 1.1  88/09/07  14:48:59  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn12.c,v $";
#endif

#define TK_IMPL_RASN1
#define TK_GVARS_RASN12
#define TK_TKI_IO
#define TK_TKI_ATTR
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_IMPL_INIT
#define TK_TKI_STR
#define TK_TKI_TKERR
#define TK_TKI_PARMS
#define TK_IMPL_RODIF

#include <rasn1.h>
#include <init.h>
#include <mm.h>
#include <str.h>
#include <parms.h>
#include <rodif.h>

#undef TK_IMPL_RASN1
#undef TK_GVARS_RASN12
#undef TK_TKI_IO
#undef TK_TKI_ATTR
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_IMPL_INIT
#undef TK_TKI_STR
#undef TK_TKI_TKERR
#undef TK_TKI_PARMS

/* Forward declarations */



INT_type RASN1_Asn1ObjectId(id, count, attrid, contentmade, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN INT_type		    attrid;
    IN BOOL_type	    *contentmade;
    IN OUT CONSTITUENT	    constituent;
{   /* Remember that we assumed that the Asn1_Object_Id_type  is of definite length.
      */


	SEQUENCE_INT_type	ints;	/* The sequence for the object_identifier_id_type */
	INT_type		rval, loccount, val;
	VALUE_type		conttype;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
	
    *count -= rval;	
    loccount = rval;

    if ((rval == INDEFINITE_LENGTH) || (rval ==0))	 {
	rval = ERROR_INT;
	TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[RASN1_Asn1ObjectId]: The Asn1 object identifier type is assumed to be of definite length for now. Stop at 0x%x", bytecount );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
 
    ints = MakeSequence(SEQUENCE_INT_tag, (INT_type) 0); 
    if (ints == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* The number of integers N is length+1.  Assume that the first digit is always 2. 
      So the second is:  y= int-2*40 */
    rval = ExpandSequence(ints, (INT_type) 1);
    ints->sequence_value.ints[0] = (INT_type) 2;

    /* Find out the following integers for the sequence */
    while (loccount > 0) {
	/* Make up each integer by combining the bytes -- initialize the val first */
	val = 0;
	do {
	    rval = RASN1_ReadByte(id, &nextbyte);	
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    loccount--;

	    val = (val << (INT_type) 7) | ((INT_type) nextbyte & (~(~0x7F)));
	} while ((0x80 & nextbyte) !=  0 );

	rval = ExpandSequence(ints, (INT_type) 1);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	if (ints->length == 2) {
	    ints->sequence_value.ints[1] = val - 2*40;
	}
	else {
	    ints->sequence_value.ints[ints->length-1] = val;
	}
    }

    /* set the attribute */
    if (attrid == at_CONTENT_ARCH_CLASS) {
	rval = SetAttr(constituent, attrid, (POINTER_type) ints, NO_PARMS);
    }
    else if (*contentmade){  /* The case of at_TYPE_OF_CODING */
	rval = SetAttr(TheContent, at_TYPE_OF_CODING, 
		    (POINTER_type) ints, NO_PARMS);
    }
    else {  
	switch (LookUpObjectId(ints)) {
	        case CC_CODING_id:
		    conttype = CHARACTER_CONTENT;
		    break;
		case RG_T6_CODING_id:
		case RG_T4_1_CODING_id:
		case RG_T4_2_CODING_id:
		case RG_BITMAP_CODING_id:
		    conttype = RASTER_GRAPHICS_CONTENT;
		    break;
		case GG_CODING_id:
		    conttype = GEOMETRIC_GRAPHICS_CONTENT;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_Asn1ObjectId]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
	/* Make the content portion */
	TheContent = MakeContent(doc, conttype);
	if (TheContent == ERROR_CONSTITUENT) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	*contentmade = BOOL_true;
	/* Set the attribute of at_TYPE_OF_CODING */
	rval = SetAttr(TheContent, at_TYPE_OF_CODING, (POINTER_type) ints, NO_PARMS);
    }
		    
  

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


INT_type RASN1_TabStop(id, count, tab_stops)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	SEQUENCE_TAB_STOP_type	    tab_stops;
{   /* read the SET of attributes in the tabulation stop.
      */
	TAB_STOP_type		tab_stop;
	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	STRING_type		string;	
	VALUE_type		value;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    80 -- 0; tabulation-reference;
	    81 -- 1; tabulation-position;
	    82 -- 2; alignment;
	    83 -- 3; alignment-character-string.
	*/


	/* The valid value for the nextbyte could be  one of the above or :
	00 -- indecates a possible EOC; ( Empty set )
	others -- error.
	  */
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_TabStop]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_TabStop]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    tab_stop.tab_reference = string;
		    break;
		case 0x81:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    tab_stop.tab_position = rval;
		    break;
		case 0x82:
		    rval = RASN1_GetValue(id, &loccount, cc_CHAR_ORIENTATION, &value);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    tab_stop.alignment = value;
		    break;
		case 0x83:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    tab_stop.alignment_string = string;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_TabStop]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* Everything is all right,  let's append this tab_stop to the sequence */
    rval = ExpandSequence(tab_stops, (INT_type) 1);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    tab_stops->sequence_value.tab_stops[tab_stops->length-1] = tab_stop;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type RASN1_FontType(id, count, int_pairs)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	SEQUENCE_INT_PAIR_type	    int_pairs;
{   /* read the SET of attributes in the character fonts.
      */
	INT_PAIR_type		int_pair;
	INT_type		loccount;
	INT_type		rval;	
	BOOL_type		isdeflen;
	INT_type		dummy;
	BYTE_type		byte;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
     /*	get the length first */	    
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }

    while (0 == 0) {
	if ((loccount == 0x00) && (isdeflen)) {
	    rval = SUCCESS_INT;
	    break;
	}
	/* fetch the nextbyte and make sure it's one of:   a0, a1 or 00. */
	rval = RASN1_ReadByte(id, &nextbyte);   
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	if ((INT_type) nextbyte	== 0x00) {  /* possible EOC */
	    rval = RASN1_ReadByte(id, &nextbyte);	
	    if ((INT_type) nextbyte != 0x00) {
		rval = ERROR_INT;
		TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[RASN1_FontType]: Invalid byte 0x%x", (INT_type) nextbyte);
		dummy = TKERR_FormatInt(" at 0x%x", bytecount );
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
	    }
	    break;
	}
	else {  /* one of the two integers */
	    switch ((INT_type) nextbyte) {
		case 0x80:
		case 0x81:
		    byte = nextbyte;
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }	
		    if (byte == 0x80) {
			int_pair.first = rval;
			break;
		    }
		    else {
			int_pair.second = rval;
			break;
		    }
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FontType]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	    }
	}
    }

    /* now that we have got the two integers, append them to the sequence */
    rval = ExpandSequence(int_pairs, (INT_type) 1);
    if (rval == ERROR_INT ) {
	goto LEAVE;
    }
    int_pairs->sequence_value.int_pairs[int_pairs->length-1] = int_pair;
	
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



SEQUENCE_STRING_type RASN1_SaveSeqStrings(id, count)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
{   /* 1. Read the SEQUENCE_OF_STRING_type data and return  it.
      2. Applies to at_SUBORDINATES and at_CONTENT_PORTIONS and,  at_PERMITTED_CATEGORIES.. 
      3. In the case of at_CONTENT_PORTIONS,  we assume the order of appearance constituents
          is as defined ISO 8613-5,  namely, a content portion always appears after it's 
      referenced.
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on=BOOL_true;
	INT_type		rval;
	STRING_type		string;	
	SEQUENCE_STRING_type	rseq;	/* The returned value */
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    rseq = MakeSequence(SEQUENCE_STRING_tag, (INT_type) 0);
    if (rseq == ERROR_SEQUENCE) {
	goto LEAVE;
    }

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	rseq = ERROR_SEQUENCE;
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }
    
    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    break;
	}
    
    
	/* Fetch the nextbyte and see -- it can only be 12, 13 or 00.
	 */
	rval = RASN1_ReadByte(id, &nextbyte);   
	if (rval == ERROR_INT) {
	    rseq = ERROR_SEQUENCE;
	    goto LEAVE;
	}
	loccount--;

	if ((INT_type) nextbyte == 0x00) {
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rseq = ERROR_SEQUENCE;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SaveSeqStrings]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if ((INT_type) nextbyte != 0x00) {
			rseq = ERROR_SEQUENCE;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SaveSeqStrings]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
	}
	else if (((INT_type) nextbyte == 0x12) || ((INT_type) nextbyte == 0x13)) {
	    /* Each of these is a string  */
	   string = RASN1_GetString(id, &loccount);
	   if (string == ERROR_STRING) {
	       rseq = ERROR_SEQUENCE;
	       goto LEAVE;
	   }
	   /* Put this string in the sequence */
	   rval = ExpandSequence(rseq, (INT_type) 1);
	   if (rval == ERROR_INT) {
	       rseq = ERROR_SEQUENCE;
	       goto LEAVE;
	   }
	   rseq->sequence_value.strings[rseq->length-1] = string;
	 
	}
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rseq;
}

HASH_type   *lookup(id)
    IN STRING_type  id;
{   /* Look in the hash table and see if that constituent is there.
        If found, return the pointer to the node, otherwise return null pointer.
    */
    HASH_type	*hp;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    for (hp=hash[hashit(id)]; hp != (HASH_type *) 0; hp = hp->next) {
	if (Identical(hp->id, id)) {
	    break;
	}   /* found it */
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( hp );

}


    
INT_type addnode(id, constituent)
    IN STRING_type  id;
    IN CONSTITUENT  constituent;
{   /* Create a new node and put the constituent in the hash table.
    */
    HASH_type	    *hp;
    POINTER_type    pointer;
    INT_type	    index, rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;
    
    /* allocate the memory for a new node */
    pointer = MM_Malloc( (INT_type) sizeof(HASH_type));
    if (pointer == ERROR_POINTER) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    hp = (HASH_type *) pointer;

    hp->id = id;
    hp->constituent = constituent;
    index = hashit(id);
    hp->next = hash[index];

    hash[index] = hp;
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return(rval);

}


        
BOOL_type Identical(s1, s2)
    IN STRING_type  s1;
    IN STRING_type  s2;
{   /* Compare the VALUEs of the two strings.  Return BOOL_true if identical,  otherwise BOOL_false.
    */
	BOOL_type   rval=BOOL_true;
	INT_type    i=0;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if (s1->length != s2->length) {
	rval = BOOL_false;
	goto LEAVE;
    }
    
    for (i=0, rval=BOOL_true; i < s1->length; i++) {
	if (s1->sequence_value.chars[i] != s2->sequence_value.chars[i]){
	    rval = BOOL_false;
	    goto LEAVE;
	}
    }
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return(rval);

}


    
INT_type RASN1_CompLogicalAttributes(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* read the SET of attributes in the default value lists for composite logical attributes
    and set those attributes to the constituent.
      */
	INT_type		loccount=0;
	INT_type		rval;	
	BOOL_type		go_on=BOOL_true;
	BOOL_type		isdeflen;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
     /*	get the length first */	    
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }
    

    while (go_on) {
	if ((loccount == 0x00) && (isdeflen)) {
	    rval = SUCCESS_INT;
	    break;
	}
	/* fetch the nextbyte and make sure it's one of:   88, 8a or 00. */
	rval = RASN1_ReadByte(id, &nextbyte);   
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_CompLogicalAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x88:
		    rval = RASN1_SetValue(id, &loccount, at_PROTECTION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8a:
		    rval = RASN1_String2Constituent(id, &loccount, at_LAYOUT_STYLE, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_CompLogicalAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x", bytecount );
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



INT_type RASN1_String2Constituent(id, count, attrid, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    IN INT_type			    attrid; /* the attr to be set */
    OUT	CONSTITUENT		    *constituent;
{   /* Read an ASN1 string and set an attribute of api CONSTITUENT_type.
      */

	INT_type		rval;	
	STRING_type		string;
	HASH_type		*hp;
	CONSTITUENT		locconst;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
    string = RASN1_GetString(id, count);
    if (string == ERROR_STRING) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* See if the constituent has been created before */
    if ((hp=lookup(string)) != (HASH_type *) 0) {
	locconst = hp->constituent;
    }
    else if (attrid == at_OBJECT_CLASS) {

	/* Nah... it's never referenced before. In the case of object descriptors,  the at_OBJECT_CLASS must refer to a class constituent that is in the hash table already */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[RASN1_String2Constituent]: Invalid byte 0x%x", (INT_type) nextbyte);
	dummy = TKERR_FormatInt(" at 0x%x.  The at_OBJECT_CLASS in the specific structures must refer to a generic structure that is in the hash table already (according to the order defined in the standard.", bytecount );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
    else {  /* Make an empty constituent  to fool around */
	locconst = CONST_MakeConstituent(doc);
	if (locconst == ERROR_CONSTITUENT) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = addnode(string, locconst);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

    /* Set the attribute of CONSTITUENT_type */
    if (attrid == IDINSTRUCT) {
	/* The case of construction expression: set the locconst to the object_class_id in the factor. */
	*constituent = locconst;
    }
    else {
	rval = SetAttr(*constituent,	attrid, locconst, NO_PARMS);
    }   
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


 
INT_type RASN1_BasicLogicalAttributes(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* read the SET of attributes in the default value lists for basic logical attributes
    and set those attributes to the constituent.
      */
	INT_type		loccount;
	INT_type		rval;	
	BOOL_type		go_on=BOOL_true;
	BOOL_type		isdeflen;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    
     /*	get the length first */	    
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }

    while (go_on) {
	if ((loccount == 0x00) && (isdeflen)) {
	    rval = SUCCESS_INT;
	    break;
	}
	/* fetch the nextbyte and make sure it's 00 or one of the following:
	 a3 -- 0; presentation attributes;
	 88 -- 1; protection;
	 89 -- 2; presentation style;
	 8a -- 3; layout style.
	   */
	rval = RASN1_ReadByte(id, &nextbyte);  
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_BasicLogicalAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x.", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa3:
		    /* Notice this is a set of JUST constent-arch-class ??? */
		    rval = RASN1_DefaultPresAttributes(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x88:
		    rval = RASN1_SetValue(id, &loccount, at_PROTECTION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x89:
		    rval = RASN1_String2Constituent(id, &loccount, at_PRESENTATION_STYLE, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8a:

		    rval = RASN1_String2Constituent(id, &loccount, at_LAYOUT_STYLE, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BasicLogicalAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x.", bytecount );
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



INT_type RASN1_DefaultPresAttributes(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* Under the 'Presentation-Attributes',  there is ONLY the 'Content-Arch-Class'
      is defaultable.  Other attributes are to be default under 'Presentation Style'.
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on=BOOL_true, contentmade;
	INT_type		rval;
	INT_type		dummy;	

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }
    

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   	
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	/* The next byte can only be: 06 or 00. */

	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_DefaultPresAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x.", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);		
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x06:
		    contentmade = BOOL_true;
		    rval = RASN1_Asn1ObjectId(id, &loccount, at_CONTENT_ARCH_CLASS, &contentmade, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_DefaultPresAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x.", bytecount );
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



INT_type RASN1_ContentAttributes(id, count, twinid)
    IN INT_type			    id;
    IN INT_type			    *count;
    OUT	STRING_PAIR_type	    *twinid;
{   /* 
      Process the data steams about the content attributes including the two identifiers and
           the alternative representation..  
      Pass out the filled content portion .
      */
	INT_type		loccount;
	BOOL_type		isdeflen, contentmade=BOOL_false, go_on=BOOL_true;
	INT_type		rval;	
	STRING_type		string;
	BOOL_type		Has_Altrep=BOOL_false; /* Do we have the at_ALT_REP */
	STRING_type		altrep;	    /* vairable to save the at_ALT_REP */
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }
    
    (*twinid).tag[0]=(*twinid).tag[1] = 0;

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   	
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	/* The next byte one of the following:
	 40 -- 0; content identifier-layout;
	 84 -- 1; content identifier-logical;
	 ab -- 2; ext-cont-arch-coding-attributes,  we don't support this;
	 ------- the above has to be saved before the type of the content is known from one of the following:
	   80 -- 3; type of coding "t6", we don't support this;
	   86 -- 4; type of coding "object identifier";
	   a1 -- 5; character coding attributes;
	   a2 -- 6; raster graphic coding attributes;
	   a7 -- 7; geo-gr-coding-attributes;
	   83 -- 8; alternative representation.
	  */
	/* In the case of 00, it means a possible EOC of this set. */

	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    rval = RASN1_ReadByte(id, &nextbyte);		
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_ContentAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x.", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x40:
		    /* SAve the layout identifier */
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    (*twinid).tag[0]++;
		    (*twinid).id[0] = string;
		    break;
		case 0x84:
		    /* Save the logical identifier */
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    (*twinid).tag[1]++;
		    (*twinid).id[1] = string;
		    break;
		case 0xab:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_ContentAttributes]: Not inplemented yet at this time.  STop at 0x%x.", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		case 0x80:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_ContentAttributes]: Not inplemented yet at this time.  STop at 0x%x.", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		case 0x86:
		    /* Get the content type and make the content portion and set this attribute */
		    rval = RASN1_Asn1ObjectId(id, &loccount, at_TYPE_OF_CODING, &contentmade, NULL_CONSTITUENT);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#ifdef UNIMPLM
		case 0xa1:
		    if (!contentmade) {
			TheContent = MakeContent(doc, CHARACTER_CONTENT);
			if (TheContent == ERROR_CONSTITUENT) {
			    rval = ERROR_INT;
			    goto LEAVE;
			}
			contentmade = BOOL_true;
		    }
		    rval = RASN1_CharacterCodingAttrs(id, &loccount, TheContent);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#endif
		case 0xa2:
		    if (!contentmade) {
			TheContent = MakeContent(doc, RASTER_GRAPHICS_CONTENT);
			if (TheContent == ERROR_CONSTITUENT) {
			    rval = ERROR_INT;
			    goto LEAVE;
			}
			contentmade = BOOL_true;
		    }
		    rval = RASN1_RasterGrCodingAttrs(id, &loccount, TheContent);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#ifdef UNIMPLM
		case 0xa7:
		    if (!contentmade) {
			TheContent	= MakeContent(doc, GEOMETRIC_GRAPHICS_CONTENT);
			if (TheContent == ERROR_CONSTITUENT) {
			    rval = ERROR_INT;
			    goto LEAVE;
			}
			contentmade = BOOL_true;
		    }
		    rval = RASN1_GeoGrCodingAttrs(id, &loccount, TheContent);
		    if (rval ==	ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#endif 
		case 0x83:
		    Has_Altrep = BOOL_true;
		    /* Get the string of alternative representation into the local variable */
		    altrep = RASN1_GetString(id, &loccount);
		    if (altrep == ERROR_STRING) {
			rval =	ERROR_INT;
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_ContentAttributes]: Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x.", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* Now that the set of content attributes are all read,  remember we skipped over the identifier(s) and the alternative representation.  So SetAttr them. */
    if (Has_Altrep) {
	rval = SetAttr(TheContent, at_ALT_REP, (POINTER_type) altrep, NO_PARMS);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	Has_Altrep = BOOL_false;
    }

    if ((*twinid).tag[0] == 1) {
	rval = SetAttr(TheContent, at_CONTENT_ID_LAYOUT, 
		    (POINTER_type) (*twinid).id[0], NO_PARMS);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    if ((*twinid).tag[1] == 1) {
	rval = SetAttr(TheContent, at_CONTENT_ID_LOGICAL, 
		    (POINTER_type) (*twinid).id[1], NO_PARMS);
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type RASN1_Separation(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	PARM_MASK_type		parm_mask;
	at_SEPARATION_type	separation;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initializations */
    parm_mask = (PARM_MASK_type) 0;
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 80 -- leading;
	 81 -- trailing;
	 82 -- centre.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_Separation]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_Separation]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    separation.leading_edge = rval;
		    parm_mask |= LEADING_EDGE_parm;
		    break;
		case 0x81:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    separation.trailing_edge = rval;
		    parm_mask |= TRAILING_EDGE_parm;
		    break;
		case 0x82:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    separation.centre_separation = rval;
		    parm_mask |= CENTRE_SEPARATION_parm;
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_Separation]:  Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* Set the attribute */
    rval = SetAttr(constituent, at_SEPARATION, 
		(POINTER_type) &separation, parm_mask);
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

INT_type RASN1_Offset(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	PARM_MASK_type		parm_mask;
	at_OFFSET_type		offset;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initializations */
    parm_mask = (PARM_MASK_type) 0;
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 80 -- right-hand;
	 81 -- left-hand;
	 82 -- trailing;
	 83 -- leading.
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_Offset]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_Offset]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    offset.right_hand_offset = rval;
		    parm_mask |= RIGHT_HAND_OFFSET_parm;
		    break;
		case 0x81:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    offset.left_hand_offset = rval;
		    parm_mask |= LEFT_HAND_OFFSET_parm;
		    break;
		case 0x82:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    offset.trailing_offset = rval;
		    parm_mask |= TRAILING_OFFSET_parm;
		    break;
		case 0x83:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    offset.leading_offset = rval;
		    parm_mask |= LEADING_OFFSET_parm;
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_Offset]:  Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* Set the attribute */
    rval = SetAttr(constituent, at_OFFSET, (POINTER_type) &offset, parm_mask);
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

