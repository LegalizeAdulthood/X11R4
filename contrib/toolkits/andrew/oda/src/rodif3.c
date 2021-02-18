
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif3.c,v 1.11 89/11/01 15:55:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif3.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif3.c,v $
 * Revision 1.11  89/11/01  15:55:00  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.10  89/11/01  11:38:03  jr
 * Remove some cast's of nextbyte.
 * 
 * Revision 1.9  89/10/31  22:47:29  mss
 * Removed extra parameter declaration of RODIF_DefaultValieLitsLayout
 * 
 * Revision 1.8  89/10/31  22:20:29  mss
 * *** empty log message ***
 * 
 * Revision 1.7  89/10/31  22:18:32  mss
 * Fixed comment in log which contained a slash-star star-slash
 * in it (confusing
 * the compiler since the log is itself in one big comment)
 * 
 * Revision 1.6  89/10/31  15:35:19  jaap
 * RODIF_DefaultValueListsLayout was declared as:
 * static INT_type RODIF_DefaultValueListsLayout(
 *      id, count, isclass, constituent)
 * But it isclass was never used inside the procedure body.
 * If was also only called with three argunents.
 * So I removed isclass from the procedure declaration
 * 
 * Revision 1.5  89/10/31  15:31:36  jaap
 * lint shouldn't complain about 2nd arg of RASN1_Length
 * 
 * Revision 1.4  89/10/31  15:28:36  jaap
 * Lint should shut up about 3rd arg from SetAttr
 * 
 * Revision 1.3  89/10/31  12:32:19  jr
 * Change count -> &count
 * 
 * Revision 1.2  89/10/25  12:47:26  jr
 * Cast constant parameters for PC.
 * 
 * Revision 1.1  89/09/21  06:38:17  mss
 * Initial revision
 * 
 * Revision 1.20  89/09/12  09:15:22  mss
 * Removed #
 * 
 * Revision 1.19  89/03/22  15:52:50  annm
 * changes for the new dvl's
 * 
 * Revision 1.18  89/01/04  09:47:18  mss
 * Added forward declarations; Cleaned up some error
 * message handling; fixed bug in reading new page value
 * for new layout object; made some local procedures static
 * 
 * Revision 1.17  88/11/17  17:39:21  mz0k
 * added at_BLK_ALGNM
 * 
 * Revision 1.16  88/11/17  15:02:30  mz0k
 * added at_SEPARATION
 * 
 * Revision 1.15  88/11/01  14:52:47  mz0k
 * Taking care of the case that there is no identifier for a structure
 * 
 * Revision 1.14  88/10/26  19:05:45  mz0k
 * make an empty sequence before calling RODIF_GetSequenceInts(Bytes)
 * 
 * Revision 1.13  88/10/21  12:29:32  mz0k
 * moved RODIF_DocumentProfile to rodif4.c
 * 
 * Revision 1.12  88/10/18  14:33:14  mz0k
 * added the call to RASN1_Offset
 * 
 * Revision 1.11  88/10/06  16:25:03  mz0k
 * 1) %d --> %x;
 * 2) fixed the count stuff;
 * 3) IO_ReadByte --> RASN1_ReadByte (added the bytecount for the current position in the stream).
 * 
 * Revision 1.10  88/10/03  09:56:28  mz0k
 * changed INDEF_LENGTH (0X80) into INDEFINITE_LENGTH (-2)
 * 
 * Revision 1.9  88/09/30  16:08:46  mz0k
 * added the TKERR's and deleted the map[]'s
 * 
 * Revision 1.8  88/09/29  14:13:09  mz0k
 * the case of NULL_tag for the NEW_LAYOUT_OBJECT
 * 
 * Revision 1.7  88/09/28  17:35:29  mz0k
 * Changed the recursive structures of the program
 * 
 * Revision 1.6  88/09/28  13:50:47  mz0k
 * made the constitutents in the default-value-lists always specific
 * 
 * Revision 1.5  88/09/27  18:25:34  mz0k
 * added the part for setting the content portions to the real constituent in the hash table
 * 
 * Revision 1.4  88/09/22  23:16:53  mz0k
 * added the LAYOUT_OBJECT_CLASS attribute
 * 
 * Revision 1.3  88/09/22  14:55:57  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.2  88/09/13  15:00:37  mz0k
 * changed "" to <> & forward referenced constituents
 * 
 * Revision 1.1  88/09/07  14:51:17  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif3.c,v $";
#endif

/* Put .c template comment here */


#define TK_IMPL_RODIF
#define TK_IMPL_RASN1
#define TK_GVARS_RODIF3
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
#undef TK_GVARS_RODIF3
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

/* Forward declarations */

/* global variables */
static STRING_type  identifier;
static struct {
    BOOL_type		    hassub;
    CONSTRUCTION_EXPR_type  expr;
} GenForSub;	/*Variable to remember the generator for subordinates */
		
static struct {
    BOOL_type			 hassub;    /* Is there an  at_CONTENT_PORTIONS? */
    SEQUENCE_CONSTITUENT_type    constituents;  /* the seqeunce to remember the  or the content portions forward referenced. */
}  Contents;
static SEQUENCE_CONSTITUENT_type    contids;	/* for the last_ids of the forward-referenced content portions */

/* Procedures */

static INT_type    RODIF_LayoutObjClassBody();
static INT_type RODIF_LayoutDirectives(/*id, count, constituent*/);
static INT_type RODIF_DefaultValueListsLayout(/*id, count, constituent*/);



INT_type RODIF_PresentationStyle(id)
    IN INT_type	    id;
{ /* 
      a) Read the Presentation Style part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
      b) Read the next byte and see what to do next.
      */

	INT_type		count;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	STRING_type		string, identifier;
	CONSTITUENT		presentation;
	HASH_type		*hp;
	BOOL_type		hasid;
	INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif
    go_on = BOOL_true;
    hasid = BOOL_false;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, &count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { count = rval;  }
    
    /* Make a presentation style first */
    presentation = MakeStyle(doc, PRESENTATION_STYLE);
    if (presentation ==	ERROR_CONSTITUENT)  {
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
    
	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    45 -- 0; style-identifier;
	    80 -- 1; user readable comments;
	    81 -- 2; user visible name;
	    82 -- 3; transparency;
	    a3 -- 4; presentation attributes;
	    84 -- 5; colour;
	    a5 -- 6; border.

	      Also, if we come up to a 0x00,  it means a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC.  Case without an style-id is invalid.*/
		    if ((isdeflen) || (!hasid)) {
			rval = ERROR_INT;
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_PresentationStyle]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x. See if you have presentation style identifier.", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_PresentationStyle]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x45:
		    string = RASN1_GetString(id, &count);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    /* Pass out the identifier */
		    identifier = string;

		    rval = SetAttr(presentation, at_PRESENTATION_STYLE_ID, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    hasid = BOOL_true;
		    break;
		case 0x80:
		    string = RASN1_GetString(id, &count);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(presentation,at_USER_READABLE_COMMENTS, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    string = RASN1_GetString(id, &count);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(presentation,	at_USER_VISIBLE_NAME, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x82:
		    rval = RASN1_SetValue(id, &count, at_TRANSPARENCY, presentation);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa3:
		    rval = RODIF_PresentationAttributes(id, &count, presentation);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x84:
		    rval = RASN1_SetValue(id, &count, at_COLOUR, presentation);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		/* forget about the border for now */
#ifdef UNIMPL
		case 0xa5:
		    rval = RASN1_Border();
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
#endif
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_PresentationStyle]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
	}
    }
    
    /* Now that a valid presentation style constituent has been done, put it in the hash table */
    
    if ((hp=lookup(identifier))	!= (HASH_type *) 0  ) {
	/* Ah...it's been created before, just fill it. */
	rval = CONST_FillInConstituent(hp->constituent, presentation);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    else {
	/* Not exists before, add a node to the table for it. */
	rval = addnode(identifier, presentation);
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

    return( rval );

}



INT_type RODIF_LayoutObjectClass(id)
    IN INT_type	    id;
{  /* 
      a) Read the Genreic Layout part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.

      */

    INT_type		rval;	    /* the returned value for various operations */
    BOOL_type		isdeflen;
    INT_type		count=0;	    /* the counter to keep track of definite length */
    CONSTITUENT		constituent, realconstituent;
    HASH_type		*hp;
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First thing is to read the length octet(s) of the layout-object-class constituent  */
    rval = RASN1_Length(id, &count);	
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

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    break;
	    }
	    else {
		rval = ERROR_INT;
		TKError	= ODIF_err;	    /* invald stream */

		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[RODIF_LayoutObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
		dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		dummy = TKERR_EndErrMsg();

		goto LEAVE;
	    }
	}
	else if (nextbyte == 0x02) {
		/* Return a constituent with at_OBJECT_TYPE set */
		rval = RASN1_ObjectType(id, &count, &constituent,  BOOL_false, BOOL_true);
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
			    rval = RODIF_LayoutObjClassBody(id, &count, constituent);
			    if (rval == ERROR_INT) {
				goto LEAVE;
			    }
		}
		else {
		    /* There's only the at_OBJECT_TYPE,  this is invalid in ODA. */
		    rval = ERROR_INT;
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LayoutObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x. The layout object class doesn't have any attributes in the body.", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		}
	}
	else {
	    /* There is nothing in the sequence,  this is an error */
	    rval = ERROR_INT;
	    TKError = ODIF_err;

	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[RODIF_LayoutObjectClass]: Invalid byte:0x%x", (INT_type) nextbyte);
	    dummy = TKERR_FormatInt( " at 0x%x. This is an empty layout object class.", bytecount );
	    dummy = TKERR_EndErrMsg();

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
	rval = SetAttr(realconstituent,	at_GEN_FOR_SUBORDINATES, (POINTER_type) &(GenForSub.expr), NO_PARMS);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	GenForSub.hassub = BOOL_false;
    }

    /* Do the same thing to the forward-referenced content portions */
    if (Contents.hassub) {
	Contents.constituents =    RODIF_SequenceConstituent(id, &count, at_CONTENT_PORTIONS, identifier, contids);
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



static INT_type RODIF_LayoutObjClassBody(id, count, constituent )
    IN INT_type			    id;
    IN OUT INT_type		    *count;
    OUT	CONSTITUENT		    constituent;
{   /* 
      a) Read the Generic Layout Body  part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
          b) It's in this routine that the global "identifier"  for object class is set.
      */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	STRING_type 		string;
	SEQUENCE_STRING_type	seqstr;
	BOOL_type		hasid;
	INT_type	dummy;
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
    count -= rval;

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
	    a3 or ba --3; position;
	    a4 -- 4; dimemsions;
	    85 -- 5; transparency;
	    a6 -- 6; presentation attributes;
	    a7 -- 7; default value lists;
	    88 -- 8; user readable comments;
	    a9 -- 9; bindings;
	    aa -- 10; content generator;
	    8b -- 11; layout-path;
	    ad -- 12; permitted categories;
	    8e -- 13; user visible name;
	    af -- 14; page-position;
	    b0 -- 15; medium type;
	    91 -- 16; presentation style;
	    92 -- 17; logical source;
	    b5 -- 18; balance;
	    96 -- 19; colour;
	    b7 -- 20; border;
	    98 -- 21; resource-name;
	    99 -- 22; application comments.
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

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutObjClassBody]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x. Check if you have the layout object class identifier.", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutObjClassBody]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

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

		    rval = SetAttr(constituent,	at_OBJECT_CLASS_IDENTIFIER, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    hasid = BOOL_true;
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
		    rval = RODIF_DefaultValueListsLayout(id, (INT_type *) &loccount,  constituent);
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
		case 0xad:
		    seqstr = RASN1_SaveSeqStrings(id, &loccount);
		    if (seqstr == ERROR_SEQUENCE) { 
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, at_PERMITTED_CATEGORIES, (POINTER_type) seqstr, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		    
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
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LayoutObjClassBody]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
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

    return( rval );

}


	
static INT_type RODIF_DefaultValueListsLayout(id, count, constituent)
    IN INT_type		    id;
    OUT INT_type	    *count;
    OUT	CONSTITUENT	    constituent;
{   /* read the SET type data.
      */
	INT_type		loccount=0;
	BOOL_type		isdeflen, go_on=BOOL_true;
	INT_type		rval;  
	CONSTITUENT		locconst;
	at_DEFAULT_VALUE_LISTS_type TheDVL;
	VALUE_type		comptype;
	INT_type	dummy;

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
    
    
	/* Fetch the nextbyte and see -- it can only be a5, a6,
	 or 00 indicating a possible EOC.. */
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
			dummy = TKERR_FormatInt( "[RODIF_DefaultValueListsLayout]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_DefaultValueListsLayout]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa2:
		case 0xa3:
		case 0xa4:
		    if (nextbyte == 0xa2) {
			comptype = at_OBJECT_TYPE_page;
		    }
		    else if (nextbyte == 0xa3) {
			comptype = at_OBJECT_TYPE_frame;
		    }
		    else {
			comptype = at_OBJECT_TYPE_block;
		    }
		    
		    locconst = MakeComponent(doc, comptype, DVL_COMPONENT);
		    if (locconst == ERROR_CONSTITUENT) {
			rval = ERROR_INT;
			goto LEAVE;
		    }


		    if (nextbyte == 0xa2) {
			TheDVL.page_attributes = locconst;
			rval = RASN1_PageAttributes(id, &loccount, locconst);
		    }
		    else if (nextbyte == 0xa3) {
			TheDVL.frame_attributes = locconst;
			rval = RASN1_FrameAttributes(id, &loccount, locconst);
		    }
		    else {
			TheDVL.block_attributes = locconst;
			rval = RASN1_BlockAttributes(id, &loccount, locconst);
		    }

		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;	/* invalid stream */

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_DefaultValueListsLayout]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

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



INT_type RODIF_LayoutStyle(id)
    IN INT_type	    id;
{ /* 
      a) Read the Layout Style part of the ODIF stream and create the 
      corresponding internal data structures in the document of 'doc'.
      b) Read the next byte and see what to do next.
      */

	INT_type		count;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	STRING_type		string, identifier;
	CONSTITUENT		layout;
	HASH_type		*hp;
	BOOL_type		hasid;
	INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    hasid  = BOOL_false;
    go_on = BOOL_true;

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, (INT_type *) &count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
  
    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { count = rval;  }
    
    /* Make a layout style first */
    layout = MakeStyle(doc, LAYOUT_STYLE);
    if (layout == ERROR_CONSTITUENT)  {
	rval = ERROR_INT;
	goto LEAVE;
    }

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (count == 0)) {
	    break;
	}
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   count--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    45 -- 0; style-identifier;
	    80 -- 1; user readable comments;
	    81 -- 2; user visible name;
	       a4 -- 3; layout directives.

	      Also, if we come up to a 0x00,  it means a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC.  Case without an style-id is invalid.*/
		    if ((isdeflen) || (!hasid)) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutStyle]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  See if you have the layout style identifier.", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutStyle]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x45:
		    string = RASN1_GetString(id, &count);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }

		    /* the identifier to pass out */
		    identifier = string;

		    rval = SetAttr(layout, at_LAYOUT_STYLE_IDENTIFIER, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    hasid = BOOL_true;
		    break;
		case 0x80:
		    string = RASN1_GetString(id, &count);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(layout,at_USER_READABLE_COMMENTS, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    string = RASN1_GetString(id, &count);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(layout, at_USER_VISIBLE_NAME, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa4:
		    rval = RODIF_LayoutDirectives(id, &count, layout);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LayoutStyle]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
	}
    } 

    /* Now that a valid layout style constituent has been done, put it in the hash table */
    
    if ((hp=lookup(identifier))	!= (HASH_type *) 0  ) {
	/* Ah...it's been created before, just fill it. */
	rval = CONST_FillInConstituent(hp->constituent, layout);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }
    else {
	/* Not exists before, add a node to the table for it. */
	rval = addnode(identifier, layout);
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

    return( rval );

}


	
static INT_type RODIF_LayoutDirectives(id, count, constituent)
	IN INT_type	    id;
	IN INT_type	    *count;
	IN CONSTITUENT	    constituent;
{ /* 
      a) Read the Layout Directives part of the ODIF stream and create the 
      corresponding internal data structures.

      */

	INT_type		loccount;
	BOOL_type		isdeflen, go_on=BOOL_true;
	INT_type		rval;	
	STRING_type		string;
	LAYOUT_OBJECT_type	LayoutObj;  /* for the indivisibility,  new-layout-object and the same-layout-object */
	VALUE_type		value;	    /* Fake value for page tags in Layout Object Class */
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
    if (isdeflen)	 { loccount = rval;  }
    

    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    break;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The map for each of the attributes to avoid duplicates.  The attributes,
	    in the order, are: 
	    80, 81, 82 or 8f -- 0; indivisibility,  one of:
	                                layour object class identifier;
	                                layout category name;
	                                layout object type;
	                                null;
	    a3 -- 1; separation;
	    a4 -- 2; offset;
	    85 -- 3; fill-order;
	    86 -- 4; concatenation;
	    87, 88, 89 or 90 -- 5; new layout object,  (same as that of indivisibility);
	    aa -- 6; same layout object;
	    8b -- 7; object class identifier;
	    8c -- 8; layout category name;
	    8d, b1 or 92 -- 9; synchronisation;
	    8e -- 10;  block alignment.

	      Also, if we come up to a 0x00,  it means a possible EOC.
	*/


	switch (nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false; 
		    break;
		case 0x80:
		    LayoutObj.layout_object_tag = LAYOUT_OBJ_DESC_tag;
		    rval = RASN1_String2Constituent(id, &loccount, IDINSTRUCT, &LayoutObj.layout_object_value.constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, at_INDIVISIBILITY, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    LayoutObj.layout_object_tag = LAYOUT_CATEGORY_tag;
		    LayoutObj.layout_object_value.layout_category = string;
		    
		    rval = SetAttr(constituent, at_INDIVISIBILITY, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0x82:
		    LayoutObj.layout_object_tag	= OBJECT_TYPE_PAGE_tag;
		    
		    rval = RASN1_GetValue(id, &loccount, at_OBJECT_TYPE, &value);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    if (value != at_OBJECT_TYPE_page) {
			TKError = INTERNAL_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  In the case of an OBJECT_TYPE_PAGE_tag,  the value should be an at_OBJECT_TYPE_page.", bytecount );
			dummy = TKERR_EndErrMsg();

			rval = ERROR_INT;
			goto LEAVE;
		    }
		    
		    rval = SetAttr(constituent, at_INDIVISIBILITY, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0xa3:
		    rval = RASN1_Separation(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa4:
		    rval = RASN1_Offset(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x86:
		    rval = RASN1_SetValue(id, &loccount, at_CONCATENATION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8e:
		    rval = RASN1_SetValue(id, &loccount, at_BLK_ALGNM, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8f:

		    LayoutObj.layout_object_tag	= NULL_tag;
		    /*  The length should be 0x00 */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = INTERNAL_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  In the case of an NULL_tag,  the value should be 0x00.", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }

		    
		    rval = SetAttr(constituent, at_INDIVISIBILITY, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x87:
		    LayoutObj.layout_object_tag = LAYOUT_OBJ_DESC_tag;
		    rval = RASN1_String2Constituent(id, &loccount, IDINSTRUCT, &LayoutObj.layout_object_value.constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }

		    rval = SetAttr(constituent, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0x88:
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    LayoutObj.layout_object_tag = LAYOUT_CATEGORY_tag;
		    LayoutObj.layout_object_value.layout_category = string;
		    
		    rval = SetAttr(constituent, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0x89:
		    LayoutObj.layout_object_tag	= OBJECT_TYPE_PAGE_tag;
		    
		    rval = RASN1_GetValue(id, &loccount, at_OBJECT_TYPE, &value);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    if (value != at_OBJECT_TYPE_page) {
			TKError = INTERNAL_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  In the case of an OBJECT_TYPE_PAGE_tag,  the value should be an at_OBJECT_TYPE_page.", bytecount );
			dummy = TKERR_EndErrMsg();

			rval = ERROR_INT;
			goto LEAVE;
		    }
		    
		    rval = SetAttr(constituent, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT){
			goto LEAVE;
		    }
		    break;
		case 0x90:
		    LayoutObj.layout_object_tag	= NULL_tag;
		    /*  The length should be 0x00 */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    if (nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = INTERNAL_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x.  In the case of an NULL_tag,  the value should be 0X00", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }


		    rval = SetAttr(constituent, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObj, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8b:
		    rval = RASN1_String2Constituent(id, &loccount, at_LAYOUT_OBJECT_CLASS, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8c:
		    
		    string = RASN1_GetString(id, &loccount);
		    if (string == ERROR_STRING) {
			rval = ERROR_INT;
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, at_LAYOUT_CATEGORY, (POINTER_type) string, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RODIF_LayoutDirectives]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
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

    return( rval );

}



INT_type RODIF_LayoutObject(id)
    IN INT_type	    id;
{
	INT_type	rval;
	INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[RODIF_LayoutObject]: Not implemented at this time.");
    dummy = TKERR_EndErrMsg();


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



