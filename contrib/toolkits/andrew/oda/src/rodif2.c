
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif2.c,v 1.4 89/11/01 11:37:30 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif2.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif2.c,v $
 * Revision 1.4  89/11/01  11:37:30  jr
 * Remove some cast's of nextbyte.
 * 
 * Revision 1.3  89/10/31  14:56:00  jaap
 * keep lint happy
 * 
 * Revision 1.2  89/10/25  12:47:02  jr
 * Cast constant parameters for PC.
 * 
 * Revision 1.1  89/09/21  06:37:50  mss
 * Initial revision
 * 
 * Revision 1.18  89/03/22  15:52:15  annm
 * changes for the new dvl's
 * 
 * Revision 1.17  89/03/21  13:16:48  jr
 * Minor cleanup & some changes
 * for modified routines.
 * 
 * Revision 1.16  88/11/02  20:21:34  mz0k
 * Allowed PRIMITIVE cases for the Construction-Expression
 * and the Construction-Term
 * 
 * Revision 1.15  88/11/01  14:52:14  mz0k
 * Taking care of the case that there is no identifier for a structure
 * 
 * Revision 1.14  88/10/26  19:04:38  mz0k
 * make an empty sequence before calling RODIF_GetSequenceInts(Bytes)
 * 
 * Revision 1.13  88/10/06  18:26:49  mz0k
 * TK_GVARS_RODIF1 --> TK_GVARS_RODIF2
 * 
 * Revision 1.12  88/10/06  16:21:52  mz0k
 * 1) changed the %d to %x;
 * 2) fixed the count stuff;
 * 3) got rid of the map[]'s;
 * 4) IO_ReadByte --> RASN1_REadByte.
 * 
 * Revision 1.11  88/10/03  09:55:37  mz0k
 * changed INDEF_LENGTH (0X80) into INDEFINITE_LENGTH (-2)
 * 
 * Revision 1.10  88/09/30  16:07:03  mz0k
 * 
 * added the TKERR's
 * 
 * Revision 1.9  88/09/28  17:35:13  mz0k
 * Changed the recursive structures of the program
 * 
 * Revision 1.8  88/09/28  13:50:20  mz0k
 * made the constitutents in the default-value-lists always specific
 * 
 * Revision 1.7  88/09/27  18:21:52  mz0k
 * fixed the RODIF_ConstrExpression and the like 
 * 
 * Revision 1.6  88/09/23  16:39:52  mz0k
 * minors change
 * 
 * Revision 1.5  88/09/23  14:48:12  mz0k
 * fixed the Construction_expression stuff
 * 
 * Revision 1.4  88/09/22  14:55:37  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.3  88/09/13  15:00:13  mz0k
 * changed "" to <> & forward referenced constituents
 * 
 * Revision 1.2  88/09/08  16:57:58  mz0k
 * First version that works.
 * 
 * Revision 1.1  88/09/07  14:50:36  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif2.c,v $";
#endif

/* Put .c template comment here */


#define TK_IMPL_RODIF
#define TK_IMPL_RASN1
#define TK_GVARS_RODIF2
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

#include <rodif.h>

#undef TK_IMPL_RODIF
#undef TK_IMPL_RASN1
#undef TK_GVARS_RODIF2
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

static STRING_type  identifier;

/* Variable to remember the generator for subordinates */
static struct {
    BOOL_type		    hassub;
    CONSTRUCTION_EXPR_type  expr;
} GenForSub;

/* Seqeunce to remember the  or the content portions forward referenced */
static struct {
    BOOL_type			 hassub;
	    /* Is there an  at_CONTENT_PORTIONS? */
    SEQUENCE_CONSTITUENT_type    constituents;

}  Contents;

/* For the last_ids of the forward-referenced content portions */
static SEQUENCE_CONSTITUENT_type contids;

/* Forward declarations */
static INT_type RODIF_LogicalObjClassBody();

INT_type RODIF_DefaultValueListsLogical(id, count, constituent)
    IN INT_type		    id;
    OUT INT_type	    *count;
    OUT	CONSTITUENT	    constituent;
{   /* Read the SET type data. */

    INT_type		loccount=0;
    BOOL_type		isdeflen, go_on;
    INT_type		rval;
    CONSTITUENT		locconst;
    at_DEFAULT_VALUE_LISTS_type TheDVL;
    VALUE_type		attrid;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    isdeflen = (rval!=INDEFINITE_LENGTH ? BOOL_true : BOOL_false);
    if (isdeflen)	 {
	loccount = rval;
    }

    TheDVL.page_attributes = NULL_CONSTITUENT;
    TheDVL.frame_attributes = NULL_CONSTITUENT;
    TheDVL.block_attributes = NULL_CONSTITUENT;
    TheDVL.composite_logical_attributes = NULL_CONSTITUENT;
    TheDVL.basic_logical_attributes = NULL_CONSTITUENT;

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    break;
	}

	/*
	    Fetch the nextbyte and see -- it can only be a5, a6,
	    or 00 indicating a possible EOC..
	*/
	rval = RASN1_ReadByte(id, &nextbyte); 
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	
	switch (nextbyte) {
	    case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_DefaultValueListsLogical]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt("[RODIF_DefaultValueListsLogical]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt(" at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa5:
		case 0xa6:
		    if (nextbyte == 0xa5)
			attrid = at_OBJECT_TYPE_comp_logical_obj;
		    else
			attrid = at_OBJECT_TYPE_bas_logical_obj;
		    locconst = MakeComponent(doc, attrid, DVL_COMPONENT);
		    if (locconst == ERROR_CONSTITUENT) {
			rval = ERROR_INT;
			goto LEAVE;
		    }	
		    if (nextbyte == 0xa5) {
			TheDVL.composite_logical_attributes = locconst;
			rval = RASN1_CompLogicalAttributes(id, &loccount, locconst);
		    }
		    else {
			TheDVL.basic_logical_attributes = locconst;
			rval = RASN1_BasicLogicalAttributes(id, &loccount, locconst);
		    }
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_DefaultValueListsLogical]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* Everything is all right... set the attribute */
    rval = SetAttr(constituent, at_DEFAULT_VALUE_LISTS, 
		(POINTER_type) &TheDVL, NO_PARMS);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type RODIF_ContentPortions(id)
    IN INT_type		    id;
{   /* 
      a) Read the Content Portions part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.

      */

    BOOL_type		isdeflen;
    INT_type		count;	    /* the counter to keep track of definite length */
    SEQUENCE_BYTE_type	infos;
    STRING_PAIR_type	twinid;
    BOOL_type		contattrvisited;
    BOOL_type		go_on= BOOL_true;
    INT_type		rval, i;	
    HASH_type		*hp1, *hp2;
    INT_type		dummy;
    SEQUENCE_BYTE_type	bytes;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    contattrvisited = BOOL_false;

    /* First thing is to read the length octet(s) of the Content Portion constituent  */
    rval = RASN1_Length(id, &count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	    count = rval;
    
    while (go_on) {
	/* Have we got to the end of a definite length set? */
	if ((isdeflen) && (count == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}

	/* Otherwise, fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);	
	count--;
	if (rval == ERROR_INT) {
	    break;
	}

	/* The nextbyte caould be one of:
	 00 -- the EOC of an indefinite length set;
	31 -- the set of content-portion-attributes;
	04 -- (first make sure it follows the content-portion-attributes) content information;
	*/
	switch (nextbyte) {
		case 0x00:
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_ContentPortions]: Invalid byte:0x%x", (INT_type) nextbyte);
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
			dummy = TKERR_FormatInt( "[RODIF_ContentPortions]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
		    }
		    go_on = BOOL_false;
		    break;
		case 0x31:
		    /* make the content portion TheContent and set the content attributes */
		    rval = RASN1_ContentAttributes(id, &count, &twinid);
		    if (rval ==ERROR_INT) {
			goto LEAVE;
		    }
		    contattrvisited = BOOL_true;
		    break;
		case 0x04:
		    if (!contattrvisited) {
			rval = ERROR_INT;
			TKError = ODIF_err; /* the infor should follow the content attributes */
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_Format("[RODIF_ContentPortions]: Missing ContentAttributes.");
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    
		    /* We have the content information present,  do it. */
		    /* Make an empty sequence first. */
		    bytes = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) 0);
		    if (bytes == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    rval = RODIF_GetSequenceByte(id, &count, bytes);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(TheContent, at_CONTENT_INFORMATION, (POINTER_type) bytes, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err; 
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_ContentPortions]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }

    /* A content portion is done.  Check if we have duplicate content portions, if yes,  collapse
	them and plug the result one in the hash table. */
 
    if ((twinid.tag[0] == 1) && (twinid.tag[1] == 1)){
	/* Both the logical and layout content identifier are present */
	if (((hp1=lookup(twinid.id[0])) == (HASH_type *) 0) || ((hp2=lookup(twinid.id[1])) == (HASH_type *) 0)) {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[RODIF_ContentPortions]: In this case that both of the logical and layout identifiers exist for the content portion,  we miss at least on one of the corresponding empty content portions made when forward referenced.");
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}

	rval = CONST_CollaspeContentPortions(TheContent, hp1->constituent, hp2->constituent);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    else if ((twinid.tag[0] == 0) && (twinid.tag[1] == 0)) {
	/* Neither of the identifiers is present */
	rval = ERROR_INT;
	TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[RODIF_ContentPortions]: Invalid byte:0x%x", (INT_type) nextbyte);
	dummy = TKERR_FormatInt( " at 0x%x.  There is no identifier.", bytecount );
	dummy = TKERR_EndErrMsg();
#endif
    }
    else {
	for (i=0; i<=1; i++) {
	    if (twinid.tag[i] == 1) {
		/* One of the two  identifiers is in the content portion */
		if ((hp1=(lookup(twinid.id[i]))) == (HASH_type *) 0) {
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[RODIF_ContentPortions]: The content portion is declaimed to be referenced before but not made.");
		    dummy = TKERR_EndErrMsg();
#endif
		    rval = ERROR_INT;
		    goto LEAVE;
		}
    
	
		rval = CONST_FillInConstituent(hp1->constituent, TheContent);
		if (rval ==  ERROR_INT) {
		    goto LEAVE;
		}
	    }
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



INT_type RODIF_LogicalObjectClass(id)
    IN INT_type		    id;
{   /* 
      a) Read the Genreic Logical part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.

      */

    INT_type		rval;	    /* the returned value for various operations */
    BOOL_type		isdeflen;
    INT_type		count=0;	    /* the counter to keep track of definite length */
    CONSTITUENT		constituent, realconstituent;
    HASH_type		*hp;
    BYTE_type		bytes[2];
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First thing is to read the length octet(s) of the logical-object-descriptor constituent  */
    rval = RASN1_Length(id, (INT_type *) count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	    count = rval;
   
    while (BOOL_true) {
	/* EXIT1: Is it the end of a definite length sequence? */
	if ((isdeflen) && (count == 0)) {
	    rval = SUCCESS_INT;
	    break;
	}

	/* Otherwise,  read another byte */  
	rval = RASN1_ReadByte(id, &nextbyte);	
	count--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The valid value for the nextbyte is either of:
	    00 -- the EOC of an indefinite length setquence
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
		    dummy = TKERR_FormatInt( "[RODIF_LogicalObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
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
		dummy = TKERR_FormatInt( "[RODIF_LogicalObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
		dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		dummy = TKERR_EndErrMsg();
#endif
		goto LEAVE;
	    }
	}
	else 
	if (nextbyte == 0x02) {
		/* Return a constituent with at_OBJECT_TYPE set */
		rval = RASN1_ObjectType(id, &count, &constituent,  BOOL_true, BOOL_true);
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
			    rval = RODIF_LogicalObjClassBody(id, &count, constituent);
			    if (rval == ERROR_INT) {
				goto LEAVE;
			    }
		}
		else {
		    /* There's only the at_OBJECT_TYPE,  this is invalid in ODA. */
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LogicalObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x. The Logical Object Class Body is missing. ", bytecount);
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		}
	}
	else {
	    /* There is nothing in the sequence,  this is an error */
	    rval = ERROR_INT;
	    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[RODIF_LogicalObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
	    dummy = TKERR_FormatInt( " at 0x%x. This is an empty logical object class? ", bytecount);
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
    }  


	/* Now that a valid generic logical descriptor is done,  plug this constituent into the hash table */
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

	/* Now we got the "Real" constituent in the hash table,  setatt at_GEN_FOR_SUBORDINATES to it, if it has one .*/
	if (GenForSub.hassub) {
	    rval = SetAttr(realconstituent, at_GEN_FOR_SUBORDINATES, (POINTER_type) &(GenForSub.expr),	    NO_PARMS);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    GenForSub.hassub = BOOL_false;
	}

	/* Do the same thing to the forward-referenced content portions */
	if (Contents.hassub) {
	    Contents.constituents = RODIF_SequenceConstituent(id, &count, at_CONTENT_PORTIONS, identifier, contids);
	    if (Contents.constituents == ERROR_SEQUENCE) {
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    rval = SetAttr(realconstituent, at_CONTENT_PORTIONS, (POINTER_type) (Contents.constituents), NO_PARMS);
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



static INT_type RODIF_LogicalObjClassBody(id, count, constituent )
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* 
      a) Read the Generic Logical part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
          b) It's in this routine that the global "identifier"  for object class is set.
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on, hasid;
	INT_type		rval;	
	STRING_type		string;
	INT_type		dummy;
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
	    41 -- 0; object-class-identifier;
	    a0 -- 1; generator for subordinates;
	    a1 -- 2; content portions;
	    a6 -- 4; presentation attributes;
	    a7 -- 5; default value lists;
	    88 -- 6; user readable comments;
	    a9 -- 7; bindings;
	    aa -- 8; content generator;
	    8e -- 9; user visible name;
	    91 -- 10; presentation style;
	    93 -- 11; layout style.
	    94 -- 12; protection;
	       98 -- 3; resource
	    99 -- 13; application comments.
	*/


	/* The valid value for the nextbyte could be  one of the above or :
	00 -- indecates a possible EOC; ( Empty set )
	others -- error.
	  */
	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if ((isdeflen) || (!hasid)) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LogicalObjClassBody]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  See if you have the logical object class identifier.", bytecount);
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
			dummy = TKERR_FormatInt( "[RODIF_LogicalObjClassBody]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount);
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x41:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			goto LEAVE;
		    }

		    /* the identifier to pass out */
		    identifier = string;
		    hasid = BOOL_true;

		    rval = SetAttr(constituent,	at_OBJECT_CLASS_IDENTIFIER, (POINTER_type) string,NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0xa0:
		    /* Yes, we do have a at_GEN_FOR_SUBORDINATES */
		    GenForSub.hassub = BOOL_true;

		    /* Walk the graph and save the identifiers for the subordinates in the GenForSub */
		    rval = RODIF_GenForSubordinates(id, &loccount, &(GenForSub.expr)); 
		    if (rval == ERROR_INT) {
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
		case 0x98:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			goto LEAVE;
		    }

		    rval = SetAttr(constituent,	at_RESOURCE, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT){
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
		    rval = RODIF_DefaultValueListsLogical(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x88:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent,at_USER_READABLE_COMMENTS, (POINTER_type) string, NO_PARMS);
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
		    rval = SetAttr(constituent,	at_USER_VISIBLE_NAME, (POINTER_type) string, NO_PARMS);
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
		    rval = SetAttr(constituent, at_APPLICATION_COMMENTS, (POINTER_type) bytes, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LogicalObjClassBody]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
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



INT_type RODIF_GenForSubordinates(id, count, expr)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTRUCTION_EXPR_type	    *expr;
{   /*
      Pass out the construction expression read,  with the  variable Expr.
      */

	INT_type		rval;
	INT_type		loccount;
	BOOL_type		isdeflen, go_on=BOOL_true;
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
			dummy = TKERR_FormatInt( "[RODIF_GenForSubordinates]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount);
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_GenForSubordinates]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount);
			dummy = TKERR_EndErrMsg();
#endif
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    go_on = BOOL_false;
	}
	else {	
	    /*  -- it can only be a0, a1, a2,  a3 or  83. */
	    rval = RODIF_ConstrExpression(id, &loccount, nextbyte, expr);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
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



INT_type RODIF_ConstrExpression(id, count,byte, expr)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    IN BYTE_type		    byte;
    IN CONSTRUCTION_EXPR_type	    *expr;

{   /*
      See which of the four construction expressions it is and pass out the expr, as well as the seq in the case of a sequence of terms.
      */

	INT_type		rval;
	BOOL_type		isdeflen;
	BOOL_type		go_on=BOOL_true;
	INT_type		loccount;
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
    

	switch ((INT_type) byte)	{
		case 0xa3:
		case 0x83:
		    expr->construction_tag	= TERM_tag;
		    while (go_on) {
			/* read the nextbyte and see which of the four it is */
			rval = RASN1_ReadByte(id, &nextbyte);
			if (rval == ERROR_INT) {
			    goto LEAVE;
			}
			switch (nextbyte) {
			    case 0x00:
				 /* Exit 2: indefinite length ... comes to the EOC. */
				if (isdeflen) {
				    rval = ERROR_INT;
				    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
				    dummy = TKERR_StartErrMsg();
				    dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
				    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
				    dummy = TKERR_EndErrMsg();
#endif
				    goto LEAVE;
				}
				rval = RASN1_ReadByte(id, &nextbyte);	
				if (nextbyte != 0x00) {
				    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
				    dummy = TKERR_StartErrMsg();
				    dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
				    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
				    dummy = TKERR_EndErrMsg();
#endif
				    rval = ERROR_INT;
				    goto LEAVE;
				}
				go_on = BOOL_false;
				break;
			    /* the four choices of construction terms */
			    case 0xa0:
			    case 0x80:
			    case 0xa1:
			    case 0x81:
			    case 0xa2:
			    case 0x82:
			    case 0xa3:
			    case 0x83:
				
				rval = RODIF_ConstrTerm(id, &loccount, nextbyte, &(expr->construction_value.term)); /* the term filled at return */
				if (rval == ERROR_INT) {
				    goto LEAVE;
				}
				break;
			    default:
				rval = ERROR_INT;
				TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
				dummy = TKERR_StartErrMsg();
				dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
				dummy = TKERR_FormatInt( " at 0x%x", bytecount);
				dummy = TKERR_EndErrMsg();
#endif
				goto LEAVE;
			}
		    }
		    break;
		case 0xa0:
		case 0xa1:
		case 0xa2:
		    if ((INT_type) byte == 0xa0) {
			expr->construction_tag = SEQUENCE_tag;
		    }
		    else if ((INT_type) byte == 0xa1) {
		    expr->construction_tag = AGGREGATE_tag;
		    }
		    else {
			expr->construction_tag = CHOICE_tag;
		    }


		    expr->construction_value.construction = MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, (INT_type) 0);
		    if (expr->construction_value.construction == ERROR_SEQUENCE) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    /* Let's loop around the sequence */
		    while (go_on) {
			/* First decide which of the four choices is present */
			rval = RASN1_ReadByte(id, &nextbyte);	
			loccount--;
			if (rval == ERROR_INT) {
			    goto LEAVE;
			}
			switch (nextbyte) {
			    case 0x00:
				 /* Exit 2: indefinite length ... comes to the EOC. */
				if (isdeflen) {
				    rval = ERROR_INT;
				    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
				    dummy = TKERR_StartErrMsg();
				    dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
				    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
				    dummy = TKERR_EndErrMsg();
#endif
				    goto LEAVE;
				}
				rval = RASN1_ReadByte(id, &nextbyte);	
				if (nextbyte != 0x00) {
				    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
				    dummy = TKERR_StartErrMsg();
				    dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
				    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
				    dummy = TKERR_EndErrMsg();
#endif
				    rval = ERROR_INT;
				    goto LEAVE;
				}
				go_on = BOOL_false;
				break;
			    /* the four choices of construction terms */
			    case 0xa0:
			    case 0x80:
			    case 0xa1:
			    case 0x81:
			    case 0xa2:
			    case 0x82:
			    case 0xa3:
			    case 0x83:
				
				
				rval = ExpandSequence(expr->construction_value.construction, (INT_type) 1);
				if (rval ==	ERROR_INT) {
				    goto LEAVE;
				}

				rval = RODIF_ConstrTerm(id, &loccount, nextbyte, &((expr->construction_value.construction)->sequence_value.construction_terms[(expr->construction_value.construction->length-1)]));
				if (rval == ERROR_INT) {
				    goto LEAVE;
				}
				break;
			    default:
				rval = ERROR_INT;
				TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
				dummy = TKERR_StartErrMsg();
				dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
				dummy = TKERR_FormatInt( " at 0x%x", bytecount);
				dummy = TKERR_EndErrMsg();
#endif
				goto LEAVE;
			}
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_ConstrExpression]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
		    dummy = TKERR_EndErrMsg();
#endif
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



INT_type RODIF_ConstrTerm(id, count, byte,term)
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    IN BYTE_type		    byte;
    OUT	CONSTRUCTION_TERM_type	    *term;
{   /*
      Pass out the construction term read,  with the global variable GenForSub.
      */

	INT_type		rval;
	INT_type		loccount;
	BOOL_type		isdeflen, go_on=BOOL_true;
	POINTER_type		pval;
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

    /* set the term tag first */
    switch ((INT_type) byte)	{
	       
		case 0xa0:
		case 0x80:
		    term->term_tag = REQ_tag;
		    break;
		case 0xa1:
		case 0x81:
		    term->term_tag = OPT_tag;
		    break;
		case 0xa2:
		case 0x82:
		    term->term_tag = REP_tag;
		    break;
		case 0xa3:
		case 0x83:
		    term->term_tag = OPT_REP_tag;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_ConstrTerm]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount);
		    dummy = TKERR_EndErrMsg();
		    goto LEAVE;
    }

    /* Do the Construction Factor here, too. */

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}

	/* read a byte and see if the end */
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	switch (nextbyte) {
		case 0x00:
		    /* this is the end of the construction term */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_ConstrTerm]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount);
			dummy = TKERR_EndErrMsg();
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_ConstrTerm]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount);
			dummy = TKERR_EndErrMsg();
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x41:
		    (term->factor).factor_tag = OBJECT_CLASS_ID_tag;
		    rval = RASN1_String2Constituent(id, &loccount, IDINSTRUCT, &((term->factor).factor_value.object_class_id));
		    if (rval ==ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa0:
		case 0xa1:
		case 0xa2:
		case 0xa3:
		case 0x83:
		    (term->factor).factor_tag = EXPR_tag;
		    /* Let's allocate the space for the damn expr */
		    pval = MM_Malloc((INT_type)sizeof(CONSTRUCTION_EXPR_type));
		    if (pval == ERROR_POINTER) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    (term->factor).factor_value.expr = (CONSTRUCTION_EXPR_type *) pval;
		    
		    rval = RODIF_ConstrExpression(id, &loccount,  nextbyte, (term->factor).factor_value.expr);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt("[RODIF_ConstrTerm]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt(" at 0x%x", bytecount);
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
