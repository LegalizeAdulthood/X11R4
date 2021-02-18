
/*
    $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn11.c,v 1.3 89/10/31 15:53:07 annm Exp $
    $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn11.c,v $


		(c) Copyright 1988 Carnegie Mellon University

	 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
		 CMU-ITC-071) for full text & permitted uses.


$Log:	rasn11.c,v $
 * Revision 1.3  89/10/31  15:53:07  annm
 * fixing lint problems
 * 
 * Revision 1.2  89/10/26  14:12:20  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:35:49  mss
 * Initial revision
 * 
 * Revision 1.13  89/03/22  15:55:58  annm
 * changes for the new dvl stuff
 * 
 * Revision 1.12  89/03/21  13:15:59  jr
 * SOme changes & cleanup.
 * 
 * Revision 1.11  89/01/18  16:02:57  annm
 * chaged cc_CHAR_IMAGE_ORIENTATION's to cc_CHAR_ORIENTATION's
 * 
 * Revision 1.10  88/10/31  13:05:12  mz0k
 * According to different machines,  distinguish 1'sand 2's complement
 *  when reading an integer
 * 
 * Revision 1.9  88/10/06  16:10:25  mz0k
 * 1) Added error messages;
 * 2) got rid of the overlap part of GetValue and SetValue;
 * 3) String and sequence_byte types can now be indefinite length;
 * 4) fixed the count stuffs;
 * 5) Cast the BITS_PER_INT to INT_type.
 * 
 * Revision 1.8  88/10/03  09:51:54  mz0k
 * changed INDEF_LENGTH (0X80) into INDEFINITE_LENGTH (-2)
 * 
 * Revision 1.7  88/09/30  15:50:58  mz0k
 * changed IO_ReadByte/IO_ReadBytes into RASN1_ReadByte/RASN1_ReadBytes
 * abd added the counter for the current position in the data stream.
 * 
 * Revision 1.6  88/09/27  18:16:53  mz0k
 * fixed the RASN1_Length for the long length situation
 * 
 * Revision 1.5  88/09/22  14:54:35  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.4  88/09/14  17:55:50  jr
 * Fix up MM_Malloc use problem.
 * 
 * Revision 1.3  88/09/13  14:49:55  mz0k
 * changed "" to <> & other stuffs
 * 
 * Revision 1.2  88/09/08  16:56:37  mz0k
 * First version that works.
 * 
 * Revision 1.1  88/09/07  14:48:52  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn11.c,v $";
#endif

#define TK_IMPL_RASN1
#define TK_GVARS_RASN11
#define TK_TKI_IO
#define TK_IMPL_ATTR
#define TK_TKI_CONST
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_TKI_STR
#define TK_TKI_TKERR

#include <mm.h>
#include <rasn1.h>
#include <str.h>

#undef TK_IMPL_RASN1
#undef TK_GVARS_RASN11
#undef TK_TKI_IO
#undef TK_IMPL_ATTR
#undef TK_TKI_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_TKI_STR
#undef TK_TKI_TKERR

/* This file contains those lower level RASN1 routines */

INT_type RASN1_ReadByte(id, byte)
    IN INT_type		    id;
    IN BYTE_type	    *byte;
{   /* To read a byte and increment the count by one */
    INT_type    rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = IO_ReadByte(id, byte);
    bytecount++;
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type RASN1_ReadBytes(id, bytes, length)
    IN INT_type		    id;
    IN BYTE_type	    *bytes;
    IN INT_type		    length;
{   /* To read a byte and increment the count by one */
	INT_type    rval;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = IO_ReadBytes(id, bytes, length);	
    if (rval != ERROR_INT) bytecount += length;;
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif
    return rval;
}

INT_type RASN1_Length(id, count)
    IN INT_type		    id;
    IN OUT INT_type	    *count;
{   /* 
     Read the length octet, and return:
      a) The definite length;
      b) The indefinite length.
      */

    INT_type		rval;	    /* the returned value  */
    BYTE_type		bytes[BITS_PER_INT/8+1];
    INT_type		nbytes, i;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = RASN1_ReadByte(id, bytes);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    if ((INT_type) bytes[0] < 0x80) {
	/* short length (<128) */
	rval = (INT_type) bytes[0];
    }
    else if (bytes[0] == INDEFINITE_LENGTH_OCTET) {
	/* indefinite length (0x80) */
	rval = INDEFINITE_LENGTH;
    }
    else { /* long length, recover the integer by combinding the bytes */
	nbytes = (INT_type) (bytes[0] & 0x7F);
	if (nbytes >	((INT_type)BITS_PER_INT)/8) {
	    /* Take this to be a mistake */
	    rval = ERROR_INT;
	    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_Length]: Length of data bigger that the MAXINT,  this is not implemented right now.  Stop at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}

	rval = RASN1_ReadBytes(id, bytes, nbytes);	
	if (rval == ERROR_INT)	{
	    goto LEAVE;
	}
	*count -= nbytes;

	/* clear the rval */
	rval = (INT_type) 0;
	for (i=0; i<nbytes; i++) {
	    /* Make sure the leading bits are zeros and assume that the trailing bits are
	     filled with zeros. */
	    rval |= (((INT_type) bytes[i]) & (~(~0xFF))) << (nbytes-i-1)*8;
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



#ifndef ONES_COMPLEMENT

INT_type Read2sComplement(id, count)
    IN INT_type		    id;
    OUT INT_type	    *count;
{   /*  Get the bytes and construct them into an integer.  Notice that they are of definite length.
      Return the integer.
      */


    INT_type		rval, val;	 
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
    loccount = rval;

    if ((rval > ((INT_type)BITS_PER_INT)/8) || (rval == INDEFINITE_LENGTH)) {
	rval = ERROR_INT;
	TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_Read2sComplement]: Length of integer bigger that the MAXINT,  this is not implemented right now.  Stop at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
    
    if (loccount == 1) {
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	if ((nextbyte & 0x80) == 0) {
	    rval = (INT_type) nextbyte;
	}
	else {
	    rval = (INT_type) (~0xff | nextbyte);
	}
    }
    else {
	/* Read a byte first and see if a negative */
	rval = RASN1_ReadByte(id, &nextbyte);	
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	loccount--;

	/* Make up each integer by combining the bytes -- initialize the val first */
	if ((nextbyte & 0x80) == 0) {
	    val = 0;
	}
	else {
	    val = ~((INT_type) 0);
	}

	/* The first byte */
	val = (val << (INT_type) 8) | ((INT_type) nextbyte & (~(~0xFF)));
	/* The rest */
	while (loccount > 0) {
	    rval = RASN1_ReadByte(id, &nextbyte);	
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	    loccount--;
	    val = (val << (INT_type) 8) | ((INT_type) nextbyte & (~(~0xFF)));
	} 
	rval = val;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}

#endif

#ifdef ONES_COMPLEMENT

INT_type Read1sComplement(id, count)
    IN INT_type		    id;
    OUT INT_type	    *count;
{   /*  Get the bytes and construct them into an integer.  Notice that they are of definite length.
      Return the integer.
      */


    INT_type		rval, val;	 
    INT_type		loccount;
    INT_type		dummy;
    
#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_FormatInt( "[Read1sComplement]: We don't use 1's complement.  Stop at 0x%x", bytecount );
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

#endif

INT_type RASN1_ObjectType(id, count, constpt, islogical, isclass)
    IN INT_type		    id;
    OUT INT_type	    *count;
    OUT	CONSTITUENT	    *constpt;
    IN BOOL_type	    islogical;
    IN BOOL_type	    isclass;
{   /*  Get the object type first, then make a component.
      */
    VALUE_type	comptype;
    INT_type	rval;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the value of the at_OBJECT_TYPE first */
    /* Assuming that the length is always 1 byte */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    if (rval != 1) {
	rval = ERROR_INT;
	TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[RASN1_ObjectType]: at_OBJECT_TYPE should be a value of one byte long. Stop at 0x%x", bytecount );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }

    rval = RASN1_ReadByte(id, &nextbyte);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
 
    if (islogical) {
	switch ((INT_type) nextbyte) {
	    case 0x00:
		comptype = at_OBJECT_TYPE_doc_logical_root;
		break;
	    case 0x01:
		comptype = at_OBJECT_TYPE_comp_logical_obj;
		break;
	    case 0x02:
		comptype = at_OBJECT_TYPE_bas_logical_obj;
		break;
	    default:
		rval = ERROR_INT;
		TKError = ODIF_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( "[RASN1_ObjectType]: Invalid byte 0x%x", (INT_type) nextbyte);
		dummy = TKERR_FormatInt("at 0x%x", bytecount );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	}
    }
    else {
	switch ((INT_type) nextbyte) {
	    case 0x00:
		comptype = at_OBJECT_TYPE_doc_layout_root;
		break;
	    case 0x01:
		comptype = at_OBJECT_TYPE_page_set;
		break;
	    case 0x02:
		comptype = at_OBJECT_TYPE_page;
		break;
	    case 0x03:
		comptype = at_OBJECT_TYPE_frame;
		break;
	    case 0x04:
		comptype = at_OBJECT_TYPE_block;
		break;
	    default:
		rval = ERROR_INT;
		TKError = ODIF_err;
		dummy = TKERR_StartErrMsg();
		dummy =
		    TKERR_FormatInt("[RASN1_ObjectType]: Invalid byte 0x%x",
				    (INT_type) nextbyte);
		dummy = TKERR_FormatInt("at 0x%x", bytecount);
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	}
    }

    /* Make the component */
    *constpt = MakeComponent(doc, comptype,
			     isclass ? GENERIC_COMPONENT : SPECIFIC_COMPONENT);
    if (*constpt == ERROR_CONSTITUENT) {
	rval = ERROR_INT;
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

STRING_type RASN1_GetString(id, count)
    IN INT_type		    id;
    IN OUT INT_type	    *count;
{   /*  Get the pointer to the string..
      */
    INT_type		rval, length;
    POINTER_type	pval;
    BYTE_type		*value;	
    STRING_type		rstring;    /* The returned string */
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Get the length first */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	rstring	= ERROR_STRING;
	goto LEAVE;
    }
    *count -= rval;
    

    if (rval == INDEFINITE_LENGTH) {
	/* Make an empty string first. */
	rstring = MakeSequence(SEQUENCE_CHAR_tag, (INT_type) 0);
	if (rstring == ERROR_SEQUENCE) {
	    goto LEAVE;
	}
	
	while (BOOL_true) {

	    /* Fetch the nextbyte and see if the end.	     
	     */
	    rval = RASN1_ReadByte(id, &nextbyte); 
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }

	    if ((INT_type) nextbyte == 0x00) {
		/* Exit 2: indefinite length ... comes to the EOC,  Read another byte. */
		rval = RASN1_ReadByte(id, &nextbyte);		
		if ((INT_type) nextbyte != 0x00) {
		    rstring = ERROR_STRING;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_GetString]: Invalid byte:0x%x", (INT_type) nextbyte );
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		}
		break;
	    }
	    else {
		rval = ExpandSequence(rstring, (INT_type) 1);
		if (rval == ERROR_INT) {
		    rstring = ERROR_STRING;
		    goto LEAVE;
		}
		rstring->sequence_value.chars[rstring->length-1] = (CHAR_type) nextbyte;
	    }
	} /* while */
    } /* the indefinite length */
    else {

	length = rval;

	/* Get the pointer to the string */
	pval = MM_Malloc(length);
	if (pval == ERROR_POINTER) {
	    rstring = ERROR_STRING;
	    goto LEAVE;
	}
	value = (BYTE_type *) pval;

	rval = RASN1_ReadBytes(id,  value, length);
	if (rval ==	ERROR_INT) {
	    rstring = ERROR_STRING;
	    goto LEAVE;
	}
    }
    /* Get the STRING_type data */
    rstring = SEQ_MakeSequence(SEQUENCE_CHAR_tag, length,  (POINTER_type) value);

    
LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif
return (rstring);

}

INT_type RASN1_SetValue(id, count, attrid, constituent)
    IN INT_type		    id;
    IN OUT INT_type	    *count;
    IN INT_type		    attrid;
    OUT	CONSTITUENT	    constituent;
{
    /*
	Get the value of a VALUE_type (those not in a struct),
	and set that attribute.
    */

    INT_type	rval;
    VALUE_type	value;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = RASN1_Length(id, count);	
    /* all the integers written for the VALUE_type is supposed to be less than 256 */
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    if (rval != 1) {
	rval = ERROR_INT;
	TKError = UNIMPL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[RASN1_SetValue]: VALUE_type should be of one byte long. Stop at 0x%x", bytecount );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = RASN1_ReadByte(id, &nextbyte);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Based on the attribute and the int value, return the VALUE_type value */
    switch (attrid) {
	   
		case at_CONTENT_TYPE:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_CONTENT_TYPE_form_rgca;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			    dummy = TKERR_StartErrMsg();
			    dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			    dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			    dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_LAYOUT_PATH:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_LAYOUT_PATH_type_0;
			    break;
			case 0x01:
			    value = at_LAYOUT_PATH_type_90;
			    break;
			case 0x02:
			    value = at_LAYOUT_PATH_type_180;
			    break;
			case 0x03:
			    value = at_LAYOUT_PATH_type_270;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_TRANSPARENCY:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_TRANSPARENCY_transparent;
			    break;
			case 0x01:
			    value = at_TRANSPARENCY_opaque;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_COLOUR:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_COLOUR_colourless;
			    break;
			case 0x01:
			    value = at_COLOUR_white;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_PROTECTION:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_PROTECTION_protected;
			    break;
			case 0x01:
			    value = at_PROTECTION_unprotected;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_BLK_ALGNM:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_BLK_ALGNM_right_hand_aligned;
			    break;
			case 0x01:
			    value = at_BLK_ALGNM_left_hand_aligned;
			    break;
			case 0x02:
			    value = at_BLK_ALGNM_centred;
			    break;
#ifdef UNKNOWNASN1
			case 0x03:
			    value = at_BLK_ALGNM_null;
			    break;
#endif
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_CONCATENATION:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_CONCATENATION_concatenated;
			    break;
			case 0x01:
			    value = at_CONCATENATION_non_concated;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_FILL_ORDER:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = at_FILL_ORDER_reverse_order;
			    break;
			case 0x01:
			    value = at_FILL_ORDER_normal_order;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_GENERIC_LAY_STRUCT:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_GENERIC_LAY_STRUCT_fact_set;
			    break;
			case 0x01:
			    value = dp_GENERIC_LAY_STRUCT_part_gen;
			    break;
			case 0x02:
			    value = dp_GENERIC_LAY_STRUCT_comp_gen;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_SPECIFIC_LAY_STRUCT:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_SPECIFIC_LAY_STRUCT_present;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_GENERIC_LOG_STRUCT:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_GENERIC_LOG_STRUCT_fact_set;
			    break;
			case 0x01:
			    value = dp_GENERIC_LOG_STRUCT_part_gen;
			    break;
			case 0x02:
			    value = dp_GENERIC_LOG_STRUCT_comp_gen;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_SPECIFIC_LOG_STRUCT:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_SPECIFIC_LOG_STRUCT_present;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_LAYOUT_STYLES:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_LAYOUT_STYLES_present;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_PRESENTATION_STYLES:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_PRESENTATION_STYLES_present;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case dp_INTERCHANGE_FMT_CLASS:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = dp_INTERCHANGE_FMT_CLASS_a;
			    break;
			case 0x01:
			    value = dp_INTERCHANGE_FMT_CLASS_b;
			    break;
			default:
			    rval = ERROR_INT;
			    goto LEAVE;
		    }
		    break;
		case cc_ALIGNMENT:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = cc_ALIGNMENT_start_aligned;
			    break;
			case 0x01:
			    value = cc_ALIGNMENT_end_aligned;
			    break;
			case 0x02:
			    value = cc_ALIGNMENT_centred;
			    break;
			case 0x03:
			    value = cc_ALIGNMENT_justified;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case cc_CHAR_ORIENTATION:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = cc_CHAR_ORIENTATION_0;
			    break;
			case 0x01:
			    value = cc_CHAR_ORIENTATION_90;
			    break;
			case 0x02:
			    value = cc_CHAR_ORIENTATION_180;
			    break;
			case 0x03:
			    value = cc_CHAR_ORIENTATION_270;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case cc_FORMATTING_INDICATOR:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = cc_FORMATTING_INDICATOR_yes;
			    break;
			case 0x01:
			    value = cc_FORMATTING_INDICATOR_no;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case cc_PROP_LINE_SPACING:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = cc_PROP_LINE_SPACING_yes;
			    break;
			case 0x01:
			    value = cc_PROP_LINE_SPACING_no;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case cc_PAIRWISE_KERNING:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = cc_PAIRWISE_KERNING_yes;
			    break;
			case 0x01:
			    value = cc_PAIRWISE_KERNING_no;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case rg_PEL_PATH:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = rg_PEL_PATH_0;
			    break;
			case 0x01:
			    value = rg_PEL_PATH_90;
			    break;
			case 0x02:
			    value = rg_PEL_PATH_180;
			    break;
			case 0x03:
			    value = rg_PEL_PATH_270;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case rg_LINE_PROGRESSION:
		    switch ((INT_type) nextbyte) {
			case 0x01:
			    value = rg_LINE_PROGRESSION_90;
			    break;
			case 0x03:
			    value = rg_LINE_PROGRESSION_270;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case rg_PEL_TRANS_DENSITY:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = rg_PEL_TRANS_DENSITY_1;
			    break;
			case 0x01:
			    value = rg_PEL_TRANS_DENSITY_2;
			    break;
			case 0x02:
			    value = rg_PEL_TRANS_DENSITY_3;
			    break;
			case 0x03:
			    value = rg_PEL_TRANS_DENSITY_4;
			    break;
			case 0x04:
			    value = rg_PEL_TRANS_DENSITY_5;
			    break;
			case 0x05:
			    value = rg_PEL_TRANS_DENSITY_6;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case rg_COMPRESSION:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    value = rg_COMPRESSION_uncompressed;
			    break;
			case 0x01:
			    value = rg_COMPRESSION_compressed;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_SetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
		    goto LEAVE;
    }

    /* set the attribute */
    rval = SetAttr(constituent, attrid, (POINTER_type) &value, NO_PARMS);
    if (rval == ERROR_INT) {
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

INT_type RASN1_GetValue(id, count, attrid, value)
    IN INT_type		    id;
    IN OUT INT_type	    *count;
    IN INT_type		    attrid;
    IN VALUE_type	    *value;
{   /*  Get the value of a VALUE_type, to be used in a structure.  The values here and those
      in the RASN1_SetValue should consist of all the VALUE_type values in the api.
      */

    INT_type	rval;
    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = RASN1_Length(id, count);	
    /*
	Remember that the VALUE_type values can only be integers
	in the range [0, 20]
    */
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    *count -= rval;

    if (rval != 1) {
	rval = ERROR_INT;
	TKError = ODIF_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	TKERR_FormatInt("[RASN_GetValue]: VALUE_type not 1 byte. Stop at %d",
			bytecount);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = RASN1_ReadByte(id, &nextbyte);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    
    /* Based on the attribute and the int value, return the VALUE_type value */
    switch (attrid) {
	   
		case at_POSITION:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    *value = at_POSITION_right_hand_aligned;
			    break;
			case 0x01:
			    *value = at_POSITION_centred;
			    break;
			case 0x02:
			    *value = at_POSITION_left_hand_aligned;
			    break;
			case 0x03:
			    *value = at_POSITION_normal_order;
			    break;
			case 0x04:
			    *value = at_POSITION_reverse_order;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_GetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_BORDER:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    *value = at_BORDER_solid;
			    break;
			case 0x01:
			    *value = at_BORDER_dashed;
			    break;
			case 0x02:
			    *value = at_BORDER_dot;
			    break;
			case 0x03:
			    *value = at_BORDER_dash_dot;
			    break;
			case 0x04:
			    *value = at_BORDER_dash_dot_dot;
			    break;
			case 0x05:
			    *value = at_BORDER_invisible;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt("[RASN1_GetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt("at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case at_MEDIUM_TYPE:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    *value = at_MEDIUM_TYPE_recto;
			    break;
			case 0x01:
			    *value = at_MEDIUM_TYPE_verso;
			    break;
			case 0x02:
			    *value = at_MEDIUM_TYPE_unspecified;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt("[RASN1_GetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			dummy = TKERR_FormatInt("at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case cc_ITEMISATION:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    *value = cc_ITEMISATION_no_itemisation;
			    break;
			case 0x01:
			    *value = cc_ITEMISATION_start_aligned;
			    break;
			case 0x02:
			    *value = cc_ITEMISATION_end_aligned;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			    dummy = TKERR_StartErrMsg();
			    dummy = TKERR_FormatInt( "[RASN1_GetValue]: Invalid byte 0x%x", (INT_type) nextbyte); 
			    dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			    dummy = TKERR_EndErrMsg();
#endif
			    goto LEAVE;
		    }
		    break;
		case cc_LINE_LAYOUT_TABLE:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    *value = cc_LINE_LAYOUT_TABLE_start_alnd;
			    break;
			case 0x01:
			    *value = cc_LINE_LAYOUT_TABLE_end_alnd;
			    break;
			case 0x02:
			    *value = cc_LINE_LAYOUT_TABLE_centred;
			    break;
			case 0x03:
			    *value = cc_LINE_LAYOUT_TABLE_alnd_arnd;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			    dummy = TKERR_StartErrMsg();
			    dummy =
			TKERR_FormatInt("[RASN1_GetValue]: Invalid byte 0x%x",
					(INT_type) nextbyte); 
			    dummy = TKERR_FormatInt("at 0x%x", bytecount );
			    dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		case rg_IMAGE_DIMENSIONS:
		    switch ((INT_type) nextbyte) {
			case 0x00:
			    *value = rg_IMAGE_DIMENSIONS_fixed;
			    break;
			case 0x01:
			    *value = rg_IMAGE_DIMENSIONS_variable;
			    break;
			default:
			    rval = ERROR_INT;
			    TKError = ODIF_err;
			    dummy = TKERR_StartErrMsg();
			    dummy =
		    TKERR_FormatInt("[RASN1_GetValue]: Invalid byte 0x%x",
				    (INT_type) nextbyte); 
			    dummy = TKERR_FormatInt( "at 0x%x", bytecount );
			    dummy = TKERR_EndErrMsg();
			    goto LEAVE;
		    }
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
		    dummy = TKERR_StartErrMsg();
		    dummy =
			TKERR_FormatInt("[RASN1_GetValue]: Invalid byte 0x%x",
					(INT_type) nextbyte); 
		    dummy = TKERR_FormatInt( "at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
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
