/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn13.c,v 1.2 89/10/31 14:12:53 jaap Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn13.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rasn13.c,v $
 * Revision 1.2  89/10/31  14:12:53  jaap
 * Cast 3rd arg of SetAttr to POINTER_type;
 * Cast 2nd arg of RASN1_GetIngterger to INT_type *
 * 
 * Revision 1.1  89/09/21  06:36:31  mss
 * Initial revision
 * 
 * Revision 1.10  89/01/18  17:44:45  annm
 * got rid of the parm_mask for cc_INITIAL_OFFSET and cc_KERNING_OFFSET
 * 
 * Revision 1.9  89/01/04  09:13:52  mss
 * Added forward declarations for static procedures;
 * cleaned up some error string code
 * 
 * Revision 1.8  88/10/21  13:45:38  mz0k
 * Fixed the parm_mask for rg_CLIPPING.
 * 
 * Revision 1.7  88/10/18  15:41:26  mz0k
 * firstint = BOOL_false;
 * 
 * Revision 1.5  88/10/13  22:14:56  mz0k
 * 1) added RASN1_GetIntPair;
 * 2) added the raster attributes and raster coding attributes.
 * 
 * Revision 1.4  88/10/07  13:45:12  mz0k
 * include rodif.h
 * 
 * Revision 1.3  88/10/06  16:16:27  mz0k
 * 1) added the error messages;
 * 2) fixed the count stuffs
 * 
 * Revision 1.2  88/10/03  09:53:52  mz0k
 * changed INDEF_LENGTH (0X80) into INDEFINITE_LENGTH (-2)
 * 
 * Revision 1.1  88/09/28  11:49:54  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn13.c,v $";
#endif

#define TK_IMPL_RASN1
#define TK_GVARS_RASN13
#define TK_TKI_IO
#define TK_TKI_ATTR
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_IMPL_INIT
#define TK_TKI_STR
#define TK_TKI_TKERR
#define TK_IMPL_RODIF

#include <rasn1.h>
#include <rodif.h>

#undef TK_IMPL_RASN1
#undef TK_GVARS_RASN13
#undef TK_TKI_IO
#undef TK_TKI_ATTR
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_IMPL_INIT
#undef TK_TKI_STR
#undef TK_TKI_TKERR
#undef TK_IMPL_RODIF

/* Forward declarations */
static INT_type RASN1_RgClipping(/*id, count, constituent*/);
static INT_type RASN1_RgPelSpacing(/*id, count, constituent*/);
static INT_type RASN1_RgSetSpacing(/*id, count, spacing*/);
static INT_type RASN1_RgImageDimensions(/*id, count, constituent*/);
static INT_type RASN1_RgSetAreaControlled(/*id, count, area*/);


INT_type RASN1_PageAttributes(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      Set the Page Attributes to the constituent in the default value lists of layout structure.
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	*count -= rval;
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
	rval = RASN1_ReadByte(id, &nextbyte);   loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC; 
	 a1 -- dimensions;
	 82 -- transparency;
	 a3 -- presentation-attributes;
	 a5 -- page-position;
	 a6 -- medium-type;
	 89 -- presentation-style;
	 8b -- colour;
	 others -- error.
	  */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa1:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Not implemented at this time. Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0x82:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Not implemented at this time. Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xa3:
		    rval = RODIF_PresentationAttributes(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa5:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Not implemented at this time. Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xa6:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Not implemented at this time. Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0x89:
		    rval = RASN1_String2Constituent(id, &loccount, at_PRESENTATION_STYLE, &constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x8b:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Not implemented at this time. Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		default:
		    rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_PageAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
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


INT_type RASN1_FrameAttributes(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      Set the Frame Attributes to the constituent in the default value lists of layout structure.
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	SEQUENCE_STRING_type	seqstr;
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    

    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	*count -= rval;
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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC; 
	 a0 -- position;
	 a1 -- dimensions;
	 82 -- transparency;
	 84 -- one-of-four-angles;
	 a7 -- permitted categories;
	 8b -- colour;
	 ac -- border;
	 others -- error.
	  */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xa1:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0x82:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0x84:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xa7:
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
		case 0x8b:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xac:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_FrameAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
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


INT_type RASN1_BlockAttributes(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      Set the Block Attributes to the constituent in the default value lists of layout structure.
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 a0 -- position;
	 a1 -- dimensions;
	 82 -- transparency;
	 a3 -- presenation attributes;
	 89 -- presentation style;
	 8b -- colour;
	 ac -- border;
	 others -- error.
	  */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xa1:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0x82:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xa3:
		    rval = RODIF_PresentationAttributes(id, &loccount, constituent);
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
		case 0x8b:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		case 0xac:
		    rval = ERROR_INT;
		    TKError = UNIMPL_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Not implemented at this time.  Stop at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_BlockAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
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




INT_type RASN1_RasterGrCodingAttrs(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 80 -- number of pels per line;
	 81 -- number of line;
	 82 -- compression;
	 84 -- number of discarded pels.
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RasterGrCodingAttrs]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RasterGrCodingAttrs]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, rg_NUMBER_OF_PELS_PER_LINE, (POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, rg_NUMBER_OF_LINES, (POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x82:
		    rval = RASN1_SetValue(id, &loccount, rg_COMPRESSION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x83:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, rg_NUMBER_DISCARDED_PELS, (POINTER_type) &rval, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RasterGrCodingAttrs]:  Invalid byte 0x%x", (INT_type) nextbyte);
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


INT_type RASN1_RasterAttributes(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	INT_type		dummy;
	INT_PAIR_type		int_pair_pr;
	PARM_MASK_type		parm_mask;

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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 80 -- pel path;
	 81 -- line progression;
	 82 -- pel transmission density;
	 a3 -- initial offset;
	 a4 -- clipping;
	 a5 :a0 -- pel spacing --- spacing;
	      81 -- pel spacing --- null;
	 a6 -- spacing ratio;
	 a7 -- image dimensions.
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RasterAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RasterAttributes]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		    rval = RASN1_SetValue(id, &loccount, rg_PEL_PATH, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    rval = RASN1_SetValue(id, &loccount, rg_LINE_PROGRESSION, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x82:
		    rval = RASN1_SetValue(id, &loccount, rg_PEL_TRANS_DENSITY, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa3:
		    rval = RASN1_GetIntPair(id, &loccount, &int_pair_pr, rg_INITIAL_OFFSET, &parm_mask); 
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, rg_INITIAL_OFFSET, (POINTER_type) &int_pair_pr, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa4:
		    rval = RASN1_RgClipping(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa5:
		    rval = RASN1_RgPelSpacing(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa6:
		    rval = RASN1_GetIntPair(id, &loccount, &int_pair_pr, rg_SPACING_RATIO, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    rval = SetAttr(constituent, rg_SPACING_RATIO, (POINTER_type) &int_pair_pr, NO_PARMS);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0xa7:
		    rval = RASN1_RgImageDimensions(id, &loccount, constituent);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RasterGrCodingAttrs]:  Invalid byte 0x%x", (INT_type) nextbyte);
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

static INT_type RASN1_RgClipping(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	INT_type		dummy;
	rg_CLIPPING_type	clipping;
	PARM_MASK_type		parm_mask, parm_mask1;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* Initialize the parm_mask */
    parm_mask1 = (PARM_MASK_type) 0;

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
	 a0 -- first coordinate pair;
	 a1 -- second coordinate pair.
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgClipping]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgClipping]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    rval = RASN1_GetIntPair(id, &loccount, &(clipping.coord1), rg_CLIPPING, &parm_mask); /* the parm_mask acts as a dummy here */
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    parm_mask1 = parm_mask1 | COORD1_parm;
		    break;
		case 0xa1:
		    rval = RASN1_GetIntPair(id, &loccount, &(clipping.coord2), rg_CLIPPING, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    parm_mask1 = parm_mask1 | COORD2_parm;
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RgClipping]:  Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
	}
    }

    /* set the attribute of rg_CLIPPING */
    rval = SetAttr(constituent, rg_CLIPPING, (POINTER_type) &clipping, parm_mask1);
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

INT_type RASN1_GetIntPair(id, count, int_pair_pr, attrid, parm_mask)
    IN INT_type		    id;
    OUT INT_type	    *count;
    OUT INT_PAIR_type	    *int_pair_pr;
    IN INT_type		    attrid;
    OUT	PARM_MASK_type	    *parm_mask;
{   /*  Read the sequence of two integers.  Notice the structure for reading SEQUENCE_type,  since the elements are OPTIONAL, it's exactly like that of a SET_type..
      */


    INT_type		rval;	    /* the returned value for various operations */
    BOOL_type		isdeflen,  first_int, go_on;
    INT_type		loccount;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initialization */
    *parm_mask = (PARM_MASK_type) 0;
    first_int = BOOL_true;
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

	/* The pair of ints could be those of a set of a sequence with tagfield of (80, 80), or(80, 81), or simply universal class (02, 02).  Therefor the only valid value for the nextbyte is:
	    02 or 80 or 81 -- the first of the two.  Notice that both of them are OPTIONAL.
	    */

	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_GetIntPair]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_GetIntPair]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x02:
		case 0x80:
		case 0x81:
		    if (first_int) { /* the first integer */
			rval = RASN1_GetInteger(id, (INT_type *) &loccount);
			if (rval == ERROR_INT) {
			    goto LEAVE;
			}
			/* plug the first integer to the structure */
			int_pair_pr->first = rval;
			first_int = BOOL_false;

			/* take care of the parm masks for some attributes */
#if 0
/* JONATHAN - please get rid of this - Ann */
			if (attrid == cc_INITIAL_OFFSET) {
			    *parm_mask |= HORIZONTAL_COORDINATE_parm;
			}
			else if (attrid == cc_KERNING_OFFSET) {
			    *parm_mask |= START_EDGE_OFFSET_parm;
			}
#endif
		    }
		    else {
			/* The second integer -- the next byte should be 80 or 81 */
			rval = RASN1_GetInteger(id, (INT_type *) &loccount);
			if (rval == ERROR_INT) {
			    goto LEAVE;
			}
			/* plug the second integer to the structure */
			int_pair_pr->second = rval;

			/* take care of the parm masks for some attributes */
#if 0
/* JONATHAN - please get rid of this - Ann */
			if (attrid == cc_INITIAL_OFFSET) {
			    *parm_mask |= VERTICAL_COORDINATE_parm;
			}
			else if (attrid == cc_KERNING_OFFSET) {
			    *parm_mask |= END_EDGE_OFFSET_parm;
			}
#endif
		    }
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_GetIntPair]:  Invalid byte 0x%x", (INT_type) nextbyte);
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



static INT_type RASN1_RgPelSpacing(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy;
	rg_PEL_SPACING_type	spacing;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initializations */
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
	 a0 -- spacing;
	 81 -- null.
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgPelSpacing]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgPelSpacing]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    spacing.null = BOOL_false;
		    /* set the two integers for the 'length' and the 'pel_spaces' of the spacing */
		    rval = RASN1_RgSetSpacing(id, &loccount, &spacing);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		case 0x81:
		    spacing.null = BOOL_true;
		    /* read a byte (presumably it's 0x00-- the length of a NULL. */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RgPelSpacing]:  Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
	}
    }

    /* set the attribute of rg_PEL_SPACING */
    rval = SetAttr(constituent, rg_PEL_SPACING, (POINTER_type) &spacing, NO_PARMS);
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

static INT_type RASN1_RgSetSpacing(id, count, spacing)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    OUT	rg_PEL_SPACING_type *spacing;
{   /* 
      Set the value of 'length' and 'pel_spaces' for 'spacing'.  Remember this is a sequence.
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on, firstint;
	INT_type		rval;	
	INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initializations */
    go_on = BOOL_true;
    firstint = BOOL_true;

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
	 02 -- x-coordinate or y-coordinate; 
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetSpacing]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetSpacing]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x02:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }

		    if (firstint) {
			spacing->length = rval;
			firstint = BOOL_false;
		    }
		    else {
			spacing->pel_spaces = rval;
		    }
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RgSetSpacing]:  Invalid byte 0x%x", (INT_type) nextbyte);
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

static INT_type RASN1_RgImageDimensions(id, count, constituent)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    IN OUT CONSTITUENT	    constituent;
{   /* 
      Set the image dimensions.
      */


	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	INT_type		dummy;
	rg_IMAGE_DIMENSIONS_type    dimensions;
	PARM_MASK_type		parm_mask;

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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 a0 -- width-controlled;
	 a1 -- height-controlled;
	 a2 -- area-controlled;
	 83 -- null.
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgImageDimensions]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgImageDimensions]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0xa0:
		    dimensions.image_dimensions_tag = WIDTH_CONTROLLED_tag;
		    rval = RASN1_GetIntPair(id, &loccount, &(dimensions.image_dimensions_value.width_controlled), rg_IMAGE_DIMENSIONS, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }		    
		    break;
		case 0xa1:
		    dimensions.image_dimensions_tag = HEIGHT_CONTROLLED_tag;
		    rval = RASN1_GetIntPair(id, &loccount, &(dimensions.image_dimensions_value.height_controlled), rg_IMAGE_DIMENSIONS, &parm_mask);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }		    
		    break;
		case 0xa2:
		    dimensions.image_dimensions_tag = AREA_CONTROLLED_tag;
		    rval = RASN1_RgSetAreaControlled(id, &loccount, &(dimensions.image_dimensions_value.area_controlled));
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }		    
		    break;
		case 0x83:
		    dimensions.image_dimensions_tag = AUTOMATIC_tag;
		    /* presumable the nextbyte is 0x00 -- the length of NULL */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }		    
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RgImageDimensions]:  Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();

		    goto LEAVE;
	}
    }

    /* set the attribute of rg_DIMENSIONS */
    rval = SetAttr(constituent, rg_IMAGE_DIMENSIONS, (POINTER_type) &dimensions, NO_PARMS);
    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

static INT_type RASN1_RgSetAreaControlled(id, count, area)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    OUT IMAGE_SIZE_type	*area;
{   /* this is to set the values for the sequence of 'area-controled'.
    */
	INT_type		loccount;
	BOOL_type		isdeflen, go_on= BOOL_true;
	INT_type		rval;	
	INT_type		dummy;
	VALUE_type		value;

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

	/* The valid value for the nextbyte could be  one of :
	    00 -- indecates a possible EOC;
	 02 -- minimum-width;
	   others -- error.
	   */


	
	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    loccount--;
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x02:
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }	
		    area->minimum_width = rval;

		    /* the next integer */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    loccount--;
		    if ((INT_type) nextbyte != 0x02) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }	
		    area->preferred_width = rval;

		    /* the next integer */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    loccount--;
		    if ((INT_type) nextbyte != 0x02) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }	
		    area->minimum_height = rval;

		    /* the next integer */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    loccount--;
		    if ((INT_type) nextbyte != 0x02) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_GetInteger(id, (INT_type *) &loccount);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }	
		    area->preferred_height = rval;

		    /* the next value */
		    rval = RASN1_ReadByte(id, &nextbyte);
		    loccount--;
		    if ((INT_type) nextbyte != 0x02) {
			rval = ERROR_INT;
			TKError = ODIF_err;

			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();

			goto LEAVE;
		    }
		    rval = RASN1_GetValue(id, &loccount, rg_IMAGE_DIMENSIONS, &value);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    area->aspect_ratio_flag = value;
		    break;
		default:
		    rval = ERROR_INT;	
		    TKError = ODIF_err;

		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_RgSetAreaControlled]:  Invalid byte 0x%x", (INT_type) nextbyte);
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

