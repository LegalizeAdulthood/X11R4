
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif1.c,v 1.5 89/11/01 15:53:32 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif1.c,v $
 * Revision 1.5  89/11/01  15:53:32  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.4  89/11/01  11:36:53  jr
 * Remove some cast's of nextbyte.
 * 
 * Revision 1.3  89/10/31  14:49:39  jaap
 * Keep lint happy
 * 
 * 
 * Revision 1.2  89/10/25  12:46:31  jr
 * Cast constant parameters for PC.
 * 
 * Revision 1.1  89/09/21  06:37:29  mss
 * Initial revision
 * 
 * Revision 1.32  89/09/12  09:14:54  mss
 * Removed #
 * 
 * Revision 1.31  89/04/11  15:56:49  annm
 * no change
 * 
 * Revision 1.30  89/01/24  12:17:30  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.29  89/01/18  16:04:01  annm
 * chaged cc_CHAR_IMAGE_ORIENTATION's to cc_CHAR_ORIENTATION's
 * 
 * Revision 1.28  88/11/01  14:50:28  mz0k
 * Taking care of the case that there is no identifier for a structure
 * 
 * Revision 1.27  88/10/31  13:08:57  mz0k
 * added cc_1ST_LINE_OFFSET and cc_INDENTATION
 * 
 * Revision 1.26  88/10/27  15:11:15  mz0k
 * Moved the RASN1_GetInteger in the GetSequenceInts to the right place
 * 
 * Revision 1.25  88/10/26  19:01:05  mz0k
 * RODIF_SequenceBytes --> RODIF_GetSequenceBytes
 * Make an empty sequence before calling RODIF_GetSequenceInts
 * 
 * Revision 1.24  88/10/18  17:57:29  mz0k
 * added cc_KERNING_OFFSET
 * 
 * Revision 1.23  88/10/14  17:49:50  mz0k
 * pass in the contentmade by reference in the RASN1_Asn1ObjectId
 * added the parm_masks for the cc_INITIAL_OFFSET
 * 
 * Revision 1.22  88/10/14  11:50:18  mz0k
 * call to IntPair --> GetIntPair;
 * RODIF_RasterAttributes --> RASN1_RasterAttributes.
 * 
 * Revision 1.21  88/10/06  16:17:52  mz0k
 * 1) fixed the count stuff;
 * 2) allowed the sequence_byte type to be indefinite length;
 * 3) changed the error format from %d to %x.
 * 
 * Revision 1.20  88/10/03  09:54:42  mz0k
 * changed INDEF_LENGTH (0X80) into INDEFINITE_LENGTH (-2)
 * 
 * Revision 1.19  88/09/30  16:05:06  mz0k
 * 1) Added the TKERR_Mes;
 * 2) deketed the map[]'s
 * 
 * Revision 1.18  88/09/28  17:34:20  mz0k
 * Changed the recursive structures of the program
 * 
 * Revision 1.17  88/09/28  13:49:25  mz0k
 * made the constitutents in the default-value-lists always specific
 * 
 * Revision 1.16  88/09/27  18:19:34  mz0k
 * *** empty log message ***
 * 
 * Revision 1.15  88/09/22  14:55:19  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.14  88/09/13  15:21:31  mz0k
 * changed the " to <>
 * 
 * Revision 1.13  88/09/08  16:57:41  mz0k
 * First version that works.
 * 
 * Revision 1.11  88/07/25  14:04:29  annm
 * fixed typo in undef of TK_GVARS_RODIF1
 * 
 * Revision 1.10  88/06/08  16:58:08  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.9  88/06/08  14:37:16  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif1.c,v $";
#endif

/* Put .c template comment here */


#define TK_IMPL_RODIF
#define TK_IMPL_RASN1
#define TK_GVARS_RODIF1
#define TK_TKI_DOC
#define TK_TKI_ATTR
#define TK_IMPL_CONST
#define TK_IMPL_IO
#define TK_TKI_PARMS
#define TK_TKI_TAGS
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_TKI_STR
#define TK_TKI_TKERR

#include <rodif.h>
#include <rasn1.h>

#undef TK_IMPL_RODIF
#undef TK_IMPL_RASN1
#undef TK_GVARS_RODIF1
#undef TK_TKI_DOC
#undef TK_TKI_ATTR
#undef TK_IMPL_CONST
#undef TK_IMPL_IO
#undef TK_TKI_PARMS
#undef TK_TKI_TAGS
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_TKI_STR
#undef TK_TKI_TKERR

/* Forward declarations */

static INT_type	RODIF_CharacterAttributes();
static INT_type	RODIF_LogObjDescriptorBody();

static STRING_type  identifier;	/* the identifier of an object */
static struct {
    BOOL_type			 hassub;    /* Is there at_SUBORDINATES or at_CONTENT_PORTIONS? */
    SEQUENCE_CONSTITUENT_type    constituents;  /* the seqeunce to remember the subordinates or the content portions forward referenced. */
} Subordinates, Contents;

static SEQUENCE_STRING_type	subids; /* local variable to save the last_ids for subordinates */
static SEQUENCE_STRING_type	contids; /* variable for the last_ids of for the forward-referenced content portions */


INT_type RODIF_Final()
{

    INT_type	rval;	    /* the returned value */
    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[RODIF_Final]: Not Implemented" );
    dummy = TKERR_EndErrMsg();
#endif

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


INT_type RODIF_Init()
{

    INT_type	rval;	    /* the returned value */
    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[RODIF_Init]: Not Implemented" );
    dummy = TKERR_EndErrMsg();
#endif


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

INT_type hashit(id)
    IN STRING_type	id;
{

    INT_type	rval;	    /* the returned value */
    INT_type	i, sum=0;
    

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    for (i=0; i<=id->length-1; i++) {
	sum += id->sequence_value.chars[i];
    }
    rval = sum % HASHSIZE;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

DOCUMENT_type ReadODIF(id)
    IN INT_type	id;
{
    INT_type		rval;	    /* the returned value for various operations */
    REGISTER INT_type	i;
    INT_type		dummy;

    /* The routines for rodif 
	RODIF_DocumentProfile,
	RODIF_GenericLayout,
	RODIF_GenericLogical,
	RODIF_GenericContent, !
	RODIF_PresentationStyle,
	RODIF_LayoutStyle,
	RODIF_SpecificLayout,
	RODIF_SpecificLogical, !
	RODIF_SpecificContent !
	*/

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* Create a document to to be operated */
    doc = MakeDocument();
    if (doc == ERROR_DOCUMENT) {
	goto LEAVE;
    }

    /*Initialise the hash table for all constituents */
    for (i=0; i<=HASHSIZE-1; i++) hash[i] = (HASH_type *) 0;

    /* Initialize the counter for the current position in the data streams. */
    bytecount = (INT_type) -1;

    while (BOOL_true) {

	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    doc = ERROR_DOCUMENT;
	    goto LEAVE;
	}

	/* Hah... we got to the end of the stream safely! */
	if (rval == 0) {
	    goto LEAVE;
	}
    
	/* The possible values of the 'nextbyte', in the order of appearance in format A,  are:
	 a0, a1, a5, a3, a7, a8, a2, a6 and again, a3.
	 These objects may or may not be present in a document,  thus the program finds its
	 way in one of the following routines and finishes at the end of the data stream.
	 */

	switch (nextbyte) {
	    case 0xa0:
		rval = RODIF_DocumentProfile(id);
		break;
	    case 0xa1:
		rval = RODIF_LayoutObjectClass(id);
		break;
	    case 0xa5:
		rval = RODIF_LogicalObjectClass(id);
		break;
	    case 0xa3:
		rval = RODIF_ContentPortions(id);
		break;
	    case 0xa7:
		rval = RODIF_PresentationStyle(id);
		break;
	    case 0xa8:
		rval = RODIF_LayoutStyle(id);
		break;
	    case 0xa2:
		rval = RODIF_LayoutObject(id);
		break;
	    case 0xa6:
		rval = RODIF_LogicalObject(id);
		break;
	    default:
		doc = ERROR_DOCUMENT;
		TKError = ODIF_err;	
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[ReadODIF]: Invalid byte:0x%x", (INT_type) nextbyte);
		dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
	}

	if (rval == ERROR_INT) {
	    doc = ERROR_DOCUMENT;
	    goto LEAVE;
	}
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( doc );

}



INT_type RODIF_LogicalObject(id)
    IN INT_type		    id;
{   /* 
      a) Read the Sepcific Logical part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
      b) This is an example of a SEQUENCE data.

      */

    INT_type		rval;	    /* the returned value for various operations */
    BOOL_type		isdeflen;
    INT_type		count=0;	    /* the counter to keep track of definite length */
    CONSTITUENT		constituent;
    HASH_type		*hp;
static CONSTITUENT	realconstituent;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First thing is to read the length octet(s) of the logical-object-descriptor constituent  */
    rval = RASN1_Length(id, &count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
  
    isdeflen = ((rval == INDEFINITE_LENGTH)? BOOL_false : BOOL_true);
    if (isdeflen)	    count = rval;
    
    while (BOOL_true) {
	/* EXIT1: Is it the end of a definite length sequence? */
	if ((isdeflen) && (count == 0)) {
	    rval = SUCCESS_INT;
	    break;
	}

	/* Otherwise,  read another byte */
	rval = RASN1_ReadByte(id, &nextbyte);	
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;

	/* The valid value for the nextbyte could be either of:
	    00 -- the EOC  of the indefinite length sequence;
	    02 -- there is at_OBJECT_TYPE;
	 */
	if (nextbyte == 0x00) {
	    /* EXIT2: This can only happen in the case of indefinite length sequence. */
	    if (!isdeflen) {
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS	
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LogicalObject]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    break;
	    }
	    else {
		rval = ERROR_INT;
		TKError	= ODIF_err;	    /* invald stream */	
#ifndef DONT_FORMAT_ERRORS
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[RODIF_LogicalObject]: Invalid byte:0x%x", (INT_type) nextbyte);
		dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
	    }
	}
	else if (nextbyte == 0x02) {
		/* Return a constituent with at_OBJECT_TYPE set */
		rval = RASN1_ObjectType(id, &count, &constituent,  BOOL_true, BOOL_false);
		if (rval ==ERROR_INT) {
		    goto LEAVE;
		}
	
		/* Prefetch the next byte,  it should be :
		 31 -- the object descriptor body;
		 */
		rval = RASN1_ReadByte(id, &nextbyte);   
		count--;
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}

		if (nextbyte == 0x31) {
			    rval = RODIF_LogObjDescriptorBody(id, &count, constituent);
			    if (rval == ERROR_INT) {
				goto LEAVE;
			    }

		}
		else {
		    /* There's only the at_OBJECT_TYPE,  this is invalid in ODA. */
		    rval = ERROR_INT;
		    TKError = ODIF_err; /* Invalid stream */	
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LogicalObject]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		}   /* if (0x31) */
	} /* if 0x02 */
	else {
	    /* There is nothing in the sequence,  this is an error */
	    rval = ERROR_INT;
	    TKError = ODIF_err;	
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[RODIF_LogicalObject]: Invalid byte:0x%x", (INT_type) nextbyte);
	    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
    }
	  

    /* Now that a valid specific logical object is done,  plug this constituent into the hash table */ 
    if ((hp=lookup(identifier))	!= (HASH_type *) 0  ) {
	/* Ah...it's been created before, just fill it. */
	rval = CONST_FillInConstituent(hp->constituent, constituent);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	realconstituent = hp->constituent;
    }
    else {
	/* Not exists before, add a node to the table for it. */
	rval = addnode(identifier, constituent);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	realconstituent = constituent;
    }

    /* Now we got the "Real" constituent in the hash table,  SetAttr at_SUBORDINATES to it, if it has one .*/
    if (Subordinates.hassub) {
	/* Read each of the strings in	the subids and construct a  constituent out of it. */
	Subordinates.constituents  =	RODIF_SequenceConstituent(id, &count, at_SUBORDINATES,identifier, subids); 
	if (Subordinates.constituents == ERROR_SEQUENCE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = SetAttr(realconstituent, at_SUBORDINATES,(POINTER_type) (Subordinates.constituents), NO_PARMS);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* Disable the 'Hassub' */
	Subordinates.hassub = BOOL_false;
    }
    /* Do the same thing to the forward-referenced content portions */
    if (Contents.hassub) {
	Contents.constituents =	RODIF_SequenceConstituent(id, &count,	at_CONTENT_PORTIONS, identifier, contids);
	if (Contents.constituents == ERROR_SEQUENCE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = SetAttr(realconstituent, at_CONTENT_PORTIONS,(POINTER_type) (Contents.constituents), NO_PARMS);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* Disable the 'Hassub' */
	Contents.hassub = BOOL_false;
    }
    
   

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



static INT_type RODIF_LogObjDescriptorBody(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* 
      a) Read the Sepcific Logical part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
          b) It's in this routine that the global "identifier" is set.
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	STRING_type		string;
	INT_type		dummy;
	BOOL_type		hasid;
	SEQUENCE_BYTE_type	bytes;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    go_on = BOOL_true;
    hasid = BOOL_false;

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
    
	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    41 -- 0; object-identifier;
	    a0 -- 1; subordinates;
	    a1 -- 2; content portions;
	    82 -- 3; object-class;
	    a6 -- 4; presentation attributes;
	    a7 -- 5; default value lists;
	    88 -- 6; user readable comments;
	    a9 -- 7; bindings;
	    aa -- 8; content generator;
	    8e -- 9; user visible name;
	    91 -- 10; presentation style;
	    93 -- 11; layout style.
	    94 -- 12; protection;
	    99 -- 13; application comments.
	*/


	/* The valid value for the nextbyte could be  one of the above or :
	00 -- indecates a possible EOC; ( Empty set )
	others -- error.
	  */
	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC.  Case without an obj-id is invalid.*/
		    if ((isdeflen) || (!hasid)) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS	
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LogObjDescritorBody]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  See if you have the logical object identifier.", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */	
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LogObjDescritorBody]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x41:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    /* the identifier to pass out */
		    identifier = string;

		    rval = SetAttr(constituent,	at_OBJECT_IDENTIFIER,(POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    hasid = BOOL_true;
		    break;
		case 0xa0:
		    /* Yes,  we do have the at_SUBORDINATES */
		    Subordinates.hassub = BOOL_true;
		    /* Notice:  
		     At this time, we might not have read the object-identifier yet.  So we just save this sequence-of-numeric-string for the time.
			 */
		    subids = RASN1_SaveSeqStrings(id, &loccount);
		    if (subids == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    break;
		case 0xa1:

		    Contents.hassub = BOOL_true;
		    contids = RASN1_SaveSeqStrings(id, &loccount);
		    if (contids == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    break;
		case 0x82:
		    rval = RASN1_String2Constituent(id, &loccount, at_OBJECT_CLASS,  &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa6:
		    rval = RODIF_PresentationAttributes(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa7:
		    rval = RODIF_DefaultValueListsLogical(id, &loccount,  constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x88:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(constituent,at_USER_READABLE_COMMENTS,(POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		/* again who knows the binding pairs and the content generator? */
		case 0x8e:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(constituent,	at_USER_VISIBLE_NAME,(POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x91:
		    rval = RASN1_String2Constituent(id, &loccount, at_PRESENTATION_STYLE, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x93:
		    rval = RASN1_String2Constituent(id, &loccount, at_LAYOUT_STYLE, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x94:
		    /* Set the at_PROTECTION  */
		    rval = RASN1_SetValue(id, &loccount, at_PROTECTION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x99:
		    /* Make an empty sequence first. */
		    bytes = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) 0);
		    if (bytes == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    rval = RODIF_GetSequenceByte(id, &loccount, bytes);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, at_APPLICATION_COMMENTS,(POINTER_type) bytes, NO_PARMS);
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;	
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LogObjDescritorBody]: Invalid byte:0x%x", (INT_type) nextbyte);
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



INT_type RODIF_PresentationAttributes(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* 
      */
	INT_type		loccount=0;
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
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    42 -- 0; content type;
	    06 -- 1; content archtecture class;
	    a0 -- 2; character attributes;
	    a1 -- 3; raster graphic attributes;
	    a2 -- 4; geometric graphic attributes;
	       a6 -- 5;ext-cont-arch-attributes.    --- not implemented yet.
	*/


	/* The valid value for the nextbyte could be  one of the above or :
	00 -- indecates a possible EOC; ( Empty set )
	others -- error.
	  */
	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS	
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_PresentationAttributes]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);		
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */	
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_PresentationAttributes]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x42:
		    /* Set the at_CONTENT_TYPE  */
		    rval = RASN1_SetValue(id, &loccount, at_CONTENT_TYPE, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }

		    break;
		case 0x06:
		    contentmade = BOOL_true;
		    rval = RASN1_Asn1ObjectId(id, &loccount, at_CONTENT_ARCH_CLASS, &contentmade, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa0:
		    rval = RODIF_CharacterAttributes(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa1:
		    rval = RASN1_RasterAttributes(id, (INT_type *) &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#ifdef UNIMPLEM
		case 0xa2:
		    rval = RODIF_GeometricAttributes(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa6:
		    rval = RODIF_ExtContArchPresAttributes(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#endif
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;	
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_PresentationAttributes]: Invalid byte:0x%x", (INT_type) nextbyte);
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



static INT_type RODIF_CharacterAttributes(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* read the SET of attributes in the character attributes.
      */INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;
	INT_type		dummy;
	INT_PAIR_type		int_pair_pr;
	PARM_MASK_type		parm_mask;
	SEQUENCE_INT_type	seqints;
	SEQUENCE_BYTE_type	bytes;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    *count -= rval;
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
  
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
    
	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    80 -- 0; character-path;
	    81 -- 1; line-regression;
	    82 -- 2; character-orientation;
	    a3 -- 3; initial-offset;
	    86 -- 4; character-spacing;
	    87 -- 5; line-spacing;
	    88 -- 6; alignment;
	    a9 -- 7; layout-table;
	    aa -- 8; graphic-rendition;
	    8b -- 9; formatting-indicator;
	    ac -- 10; character-fonts;
	    8d -- 11; graphic-char-subrepertoire;
	    ae -- 12; itemisation;
	    8f -- 13; widow-size;
	    90 -- 14; orphan-size;
	    91 -- 15; graphic-character-sets;
	    93 --16; indentation;
	    b4 -- 17; kerning-offset;
	    95 -- 18; proportional-line-spacing;
	    96 -- 19; pairwise-kerning;
	    97 -- 20; first-line-offset;
	    98 -- 21; code-extention-announcers.
	*/


	/* The valid value for the nextbyte could be  one of the above or :
	00 -- indecates a possible EOC; ( Empty set )
	others -- error.
	  */
	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */	
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_CharacterAttributes]: Invalid byte:0x%x", (INT_type) nextbyte);
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
			dummy = TKERR_FormatInt( "[RODIF_CharacterAttributes]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		    /* Set the cc_CHARACTER_PATH  */
		    rval = RASN1_SetValue(id, &loccount, cc_CHARACTER_PATH, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    /* Set the cc_LINE_PROGRESSION  */
		    rval = RASN1_SetValue(id, &loccount, cc_LINE_PROGRESSION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x82:
		    /* Set the cc_CHAR_ORIENTATION  */
		    rval = RASN1_SetValue(id, &loccount, cc_CHAR_ORIENTATION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa3:
		    
		    rval = RASN1_GetIntPair(id, &loccount, &int_pair_pr, cc_INITIAL_OFFSET, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_INITIAL_OFFSET,(POINTER_type) &int_pair_pr, parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x86:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_CHARACTER_SPACING,(POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x87:
		    rval = RASN1_GetInteger(id, &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_LINE_SPACING,(POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x88:
		    /* Set the cc_ALIGNMENT  */
		    rval = RASN1_SetValue(id, &loccount, cc_ALIGNMENT, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa9:
		    
		    rval = RODIF_SeqTableStop(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xaa:
		    seqints = MakeSequence(SEQUENCE_INT_tag, (INT_type) 0);
		    if (seqints == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = RODIF_GetSequenceInt(id, &loccount, seqints);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_GRAPHIC_RENDITION,(POINTER_type) seqints, NO_PARMS);
		    break;
		    /* A lot of things missing here??? */
		case 0xac:
		    rval = RODIF_SequenceIntPair(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x91:
		    /* Make an empty sequence first. */
		    bytes = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) 0);
		    if (bytes == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    rval = RODIF_GetSequenceByte(id, &loccount, bytes);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent,  cc_GRAPHIC_CHARACTER_SETS,(POINTER_type) bytes, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x93:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_INDENTATION,(POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xb4:
		    rval = RASN1_GetIntPair(id, &loccount, &int_pair_pr, cc_KERNING_OFFSET, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_KERNING_OFFSET,(POINTER_type) &int_pair_pr, parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x97:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, cc_1ST_LINE_OFFSET,(POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x98:
		    /* Make an empty sequence first. */
		    bytes = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) 0);
		    if (bytes == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    rval = RODIF_GetSequenceByte(id, &loccount, bytes);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr( constituent, cc_CODE_EXT_ANNOUNCERS,(POINTER_type) bytes, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_CharacterAttributes]: Invalid byte:0x%x", (INT_type) nextbyte);
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



INT_type RODIF_SeqTableStop(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* read the SET_OF SET type data.
      */
	INT_type		loccount=0;
	BOOL_type		isdeflen, go_on=BOOL_true;
	INT_type		rval;	
	SEQUENCE_TAB_STOP_type	tab_stops;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    *count -= rval;
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }

    /* Make a sequence first */
    tab_stops = MakeSequence(SEQUENCE_TAB_STOP_tag, (INT_type) 0);
    if (tab_stops == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    break;
	}
    
    
	/* Fetch the nextbyte and see -- it can only be 31 as a UNIVERSAL_SET
	 or 00 indicating a possible EOC.. */
	rval = RASN1_ReadByte(id, &nextbyte);   	
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	if (nextbyte == 0x00) {
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_SeqTableStop]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);		
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_SeqTableStop]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
	}
	else if (nextbyte == 0x31) {
	    rval = RASN1_TabStop(id, &loccount, tab_stops);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_SeqTableStop]: Invalid byte:0x%x ", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
    }

    /* Everything is all right... set the attribute */
    rval = SetAttr(constituent, cc_LINE_LAYOUT_TABLE,(POINTER_type) tab_stops, NO_PARMS);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type RODIF_GetSequenceInt(id, count, seqints)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	SEQUENCE_INT_type	    seqints;
{   /* read the SET_OF INTEGERs type data.
      */
	INT_type		loccount=0;
	BOOL_type		isdeflen, go_on=BOOL_true;
	INT_type		rval;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    *count -= rval;
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }

    
    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    break;
	}
    
    
	/* Fetch the nextbyte and see -- it can only be 02 as a UNIVERSAL_INT,
	 or 00 indicating a possible EOC.. */
	rval = RASN1_ReadByte(id, &nextbyte);   	
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	if (nextbyte == 0x00) {
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_GetSequenceInt]: Invalid byte:0x%x ", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);		
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_GetSequenceInt]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
	}
	else if (nextbyte == 0x02) {
	    rval = ExpandSequence(seqints, (INT_type) 1);
	    if (rval == ERROR_INT ) {
		goto LEAVE;
	    }
	    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    seqints->sequence_value.ints[seqints->length-1] = rval;
	}
	else {
	    rval = ERROR_INT;
	    TKError	= ODIF_err;	    /* invalid stream */
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[RODIF_GetSequenceInt]: Invalid byte:0x%x", (INT_type) nextbyte);
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



INT_type RODIF_GetSequenceByte(id, count, bytes)
    IN INT_type		    id;
    IN OUT INT_type	    *count;
    OUT	SEQUENCE_BYTE_type  bytes;
{   /*  Get the pointer to the sequence of bytes.
      */
    INT_type		rval, length;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the length first  */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    if (rval == INDEFINITE_LENGTH) {
	while (BOOL_true) {

	    /* Fetch the nextbyte and see if the end.	     
		*/
	    rval = RASN1_ReadByte(id, &nextbyte); 
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }

	    if (nextbyte == 0x00) {
		/* Exit 2: indefinite length ... comes to the EOC,  Read another byte. */
		rval = RASN1_ReadByte(id, &nextbyte);		
		if (nextbyte != 0x00) {
		    /* Take this to be an error.  The data should not consist of any 0x00's itself. */
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_GetSequenceByte]: Invalid byte:0x%x", (INT_type) nextbyte );
		    dummy = TKERR_FormatInt( " at 0x%x.  It shouldn't be 0x00.", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		}
		break;
	    }
	    else {
		rval = ExpandSequence(bytes, (INT_type) 1);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}
		bytes->sequence_value.bytes[bytes->length-1] = (BYTE_type) nextbyte;
	    }
	} /* while */
    } /* the indefinite length */
    else {
	length = rval;

	/* Expand the sequence for that length */
	rval = ExpandSequence(bytes, length);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	rval = RASN1_ReadBytes(id, bytes->sequence_value.bytes, length);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }


    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif
return (rval);

}

INT_type RODIF_SequenceIntPair(id, count, constituent)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* read the SET of  INT_PAIRs type data (cc_CHARACTER_FONTS).
      */
	INT_type		loccount=0;
	BOOL_type		isdeflen, go_on=BOOL_true;
	INT_type		rval;
	SEQUENCE_INT_PAIR_type	int_pairs;
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

    /* Make a sequence first */
    int_pairs = MakeSequence(SEQUENCE_INT_PAIR_tag,  (INT_type) 0);
    if (int_pairs == ERROR_SEQUENCE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    break;
	}
    
    
	/* Fetch the nextbyte and see -- it can only be one of the following,
	 or 00 indicating a possible EOC:
	 a0, a1, a2, a3, a4, a5, a6, a7, a8, a9.
	 */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	if (nextbyte == 0x00) {
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_SequenceIntPair]: Invalid byte:0x%x", (INT_type) nextbyte);
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
			dummy = TKERR_FormatInt( "[RODIF_SequenceIntPair]: Invalid byte:0x%x", (INT_type) nextbyte );
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
	}
	else if (nextbyte>=0xa0 && (INT_type) nextbyte<=0xa9) {
	   rval = RASN1_FontType(id, &loccount, int_pairs);
	   if (rval == ERROR_INT) {
	       goto LEAVE;
	   }
	}
	else {
	    rval = ERROR_INT;
	    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[RODIF_SequenceIntPair]: Invalid byte:0x%x", (INT_type) nextbyte);
	    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
    }

    /* Everything is all right... set the attribute */
    rval = SetAttr(constituent, cc_CHARACTER_FONTS,(POINTER_type) int_pairs, NO_PARMS);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



SEQUENCE_CONSTITUENT_type RODIF_SequenceConstituent(id, count,attrid, identifier,lastids)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    IN INT_type			    attrid;
    IN STRING_type		    identifier;
    IN SEQUENCE_STRING_type	    lastids;
{   /* 1. Read the SEQUENCE_OF_STRING_ type data 'lastids' and pass out a SEQUENCE_OF_CONSTITUENTS with the global variable seq..
      2. Applies to at_SUBORDINATES and at_CONTENT_PORTIONS. 
      3. In the case of at_CONTENT_PORTIONS,  we assume the order of appearance constituents
          is as defined ISO 8613-5,  namely, a content portion always appears after it's 
      referenced.
      */
	
	SEQUENCE_CONSTITUENT_type   seq;    /* the returned value */
	INT_type		i;
	INT_type		rval;
	STRING_type		subid;	
	HASH_type		*hp;
	CONSTITUENT		locconst;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

  
    /* Make a sequence first */
    seq = MakeSequence(SEQUENCE_CONSTITUENT_tag, (INT_type) 0);
    if (seq == ERROR_SEQUENCE) {
	goto LEAVE;
    }
    
    for (i=0; i<=lastids->length-1; i++) {
	/* first get the identifier of the subordinate */
	subid = CopySequence(identifier);
	if (subid == ERROR_SEQUENCE) {
	    seq = ERROR_SEQUENCE;
	    goto LEAVE;
	}
	rval = ExpandSequence(subid, (INT_type) 1);
	if (rval == ERROR_INT) {
	    seq = ERROR_SEQUENCE;
	    goto LEAVE;
	}
	subid->sequence_value.chars[subid->length-1] = ' ';
	rval = AppendSequence(subid, (lastids->sequence_value.strings[i]));
	if (rval == ERROR_INT) {
	    seq = ERROR_SEQUENCE;
	    goto LEAVE;
	}
    

	   /* Now that we've got the identifier of one of the forward referenced constituents,
	  see if that constituent has already been created in the hash table.  Notice that a content
	 portion would never have been created at this time. */
	   if ((attrid == at_SUBORDINATES) && ((hp=lookup(subid)) != (HASH_type *) 0)) {
		   /* Hah... it's been created already--- forget it */
		   locconst = hp->constituent;
	   }
	   else {
	       /* What if it's never created before? Make it and plug it to the table. */
	       locconst = CONST_MakeConstituent( doc); 
	       if (locconst == ERROR_CONSTITUENT) {
		   seq = ERROR_SEQUENCE;
		   rval = ERROR_INT;
	       }
	       rval = addnode(subid, locconst);
	       if (rval == ERROR_INT) {
		   seq = ERROR_SEQUENCE;
		   goto LEAVE;
	       }
	   }

	   /* Append it to the sequence */
	   rval = ExpandSequence(seq, (INT_type) 1);
	   if (rval == ERROR_INT) {
	       seq = ERROR_SEQUENCE;
	       goto LEAVE;
	   }
	   seq->sequence_value.constituents[ seq->length-1 ] = locconst;   
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return seq;
}



