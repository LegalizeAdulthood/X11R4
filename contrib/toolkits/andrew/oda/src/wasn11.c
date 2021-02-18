
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn11.c,v 1.4 89/11/01 15:59:45 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn11.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn11.c,v $
 * Revision 1.4  89/11/01  15:59:45  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.3  89/10/31  14:32:48  jr
 * Casting errors fixed.
 * 
 * Revision 1.1  89/09/21  06:41:55  mss
 * Initial revision
 * 
 * Revision 1.14  89/01/24  12:18:29  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.13  89/01/18  16:03:44  annm
 * chaged cc_CHAR_IMAGE_ORIENTATION's to cc_CHAR_ORIENTATION's
 * 
 * Revision 1.12  88/11/02  12:46:01  mz0k
 * Allowed 0 in Nbytes
 * 
 * Revision 1.11  88/11/01  14:45:49  mz0k
 * Added TKError messages
 * 
 * Revision 1.10  88/10/31  13:14:19  mz0k
 * According to the uno-redundancy requirement of asn.1,
 * also with regards to different machines,  write out an 
 * integer.
 * 
 * Revision 1.9  88/09/30  15:30:19  mss
 * Casted all uses of BITS_PER_INT so
 * that naked integer can be defined for it
 * 
 * Revision 1.8  88/09/27  18:28:41  mz0k
 * corrected the use of Nbytes
 * 
 * Revision 1.7  88/09/06  13:14:37  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.6  88/08/29  14:35:03  jr
 * Fix up attr names.
 * 
 * Revision 1.5  88/08/10  11:12:06  mz0k
 * changed cc_Itemisation
 * 
 * Revision 1.4  88/08/08  10:22:03  mz0k
 * Took care that an integer be less than the MAXINT.
 * 
 * Revision 1.3  88/08/04  14:16:08  mz0k
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn11.c,v $";
#endif

#define TK_IMPL_WASN1
#define TK_GVARS_WASN11
#define TK_TKI_IO
#define TK_IMPL_ATTR
#define TK_TKI_CONST
#define TK_TKI_TKERR

#include <wasn1.h>

#undef TK_IMPL_WASN1
#undef TK_GVARS_WASN11
#undef TK_TKI_IO
#undef TK_IMPL_ATTR
#undef TK_TKI_CONST
#undef TK_TKI_TKERR

/* This file contains those lower level ASN1 routines */



/* Forward declarations */
static INT_type IO_INT_Value();
static INT_type PositiveInteger();


INT_type WASN1_EOC(od)
    IN INT_type	    od;
{
    INT_type	rval;
    BYTE_type	octet[2];

#ifdef DEBUG
    if( Traceroutine() ) {
	Entering();
    }
#endif

    octet[0]=octet[1]=((BYTE_type) 0);
    rval = IO_WriteBytes(od, octet, (INT_type) 2);

#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_Length(od, length)
    IN INT_type	od;
    IN INT_type	length;
{   /*
      Write the length octet
      */

    INT_type rval;
    BYTE_type	octet[1];
    INT_type		dummy;

#ifdef DEBUG
    if( Traceroutine() ) {
	Entering();
    }
#endif

    /* See if indefinite form or short form */
    if (length == INDEFINITE_LENGTH || length <= SHORT_LENGTH) { 
	if (length == INDEFINITE_LENGTH) { 
	        octet[0] = INDEFINITE_LENGTH_OCTET; /* 1000,0000 */
	} 
	else if (length < 0){
	    /* Make sure not negative */
	    TKError = BADPARM_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[WASN1_Length]: The length shouldn't be negative (: 0d%d)", length);
	    dummy = TKERR_EndErrMsg();
#endif
	    rval = ERROR_INT;
	    goto LEAVE;
	} 
	else{
	    octet[0] = ((BYTE_type) length);
	}
	rval = IO_WriteByte(od, octet[0]);
	    goto LEAVE;
    }
  
    /* Now ... must use long form --- see how many bytes are needed first */
    octet[0] = (1 << 7) | MakeByte(Nbytes(length, (INT_type) 8)); 
    rval = IO_WriteByte(od, octet[0]);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    /* Write out the length (long) value */
    rval = IO_INT_Value(od, length);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type IO_INT_Value(od, val)
    IN INT_type	od;
    IN INT_type val;
{   /* 
      Write out the content octets for a long integer 
      */

	 INT_type rval;
REGISTER INT_type nbytes;


#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif  

    /* Initialize rval to a legal value */
    rval = SUCCESS_INT;
    
    /* Now . . . long integer , see how many bytes are needed & write the bytes */
    nbytes = Nbytes(val, (INT_type) 8);
    for (; rval != ERROR_INT && nbytes>0; nbytes--) {
	rval = IO_WriteByte(od, (BYTE_type) ((val >> ((nbytes-1)*8)) & 0xFF));
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif
    return rval;
}

#ifndef ONES_COMPLEMENT

/*
   Output a negative integer in ASN.1 format -- this is for
   2's complement representations.
*/

INT_type TwosComplement(od, val)
    IN INT_type	    od;
    IN INT_type	    val;
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	nbytes;
    REGISTER INT_type	leftover;
    REGISTER BYTE_type	lastbyte;
    REGISTER BYTE_type	nextbyte;
    INT_type		dummy;
	

    static BYTE_type fill[] = {

	/* 0 */ 0x0,	/* unused */
	/* 1 */ 0xFE,	/* 1111 1110 */
	/* 2 */ 0xFC,	/* 1111 1100 */
	/* 3 */ 0xF8,	/* 1111 1000 */
	/* 4 */ 0xF0,	/* 1111 0000 */
	/* 5 */ 0xE0,	/* 1110 0000 */
	/* 6 */ 0xC0,	/* 1100 0000 */
	/* 7 */ 0x80	/* 1000 0000 */
    };

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /*
       Start looking at high-order bytes to determine how many bytes will
       be necessary for representation.

       1st step is to deal with a partial byte (i.e., not all 8 bits).
    */
    leftover = ((INT_type)BITS_PER_INT) % 8;
    if (leftover != 0) {
	lastbyte = (val >> (((INT_type)BITS_PER_INT)/8)) & ~fill[leftover];
	if (lastbyte == 0) {
	    /* This can't happen -- it means the sign bit was off */
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy =
TKERR_FormatInt("[TwosComplement]: The sign bit of a negative integer (%d) is off",
		val);
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
	/* Sign extend this byte */
	lastbyte |= fill[leftover];
    } else {
	/* No partial byte,  it's like with an extra byte of all 1's */
	lastbyte = (BYTE_type) 0xFF;
    }

   
    /* Write out the octets according to the ASN.1 */
    for (nbytes=((INT_type)BITS_PER_INT)/8; nbytes>=1; nbytes--) {
	nextbyte = val >> ((nbytes-1) * 8);
	if ((lastbyte & 0xFF) == 0xFF) {
	    if ((nextbyte & 0x80) == 0) {
		/* The case of 'ff 00'.  Save the last byte, i.e., oxff,  and break out */
		break;
	    }
	    lastbyte = nextbyte;
	}
	else if ((lastbyte & 0x80) == 0) {
	    /* This is an error -- the sign bit is off */
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[TwosComplement]: The sign bit of a negative integer (%d) is off", val);
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
	else { 
	    /* Save the last byte with a sign bit of '1',  and break out */
	    break;
	}
    }

    /* For bytes end with 'ff', namely, -1,  simply write an 0xff. */
    if ((lastbyte == 0xFF) && (nbytes == 0)) {
	/* Write the length octet first */
	rval = WASN1_Length(od, (INT_type) 1);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* The content itself */
	rval = IO_WriteByte(od, (BYTE_type) 0xFF);
	goto LEAVE;
    }
	
    /* In cases other than a '-1', write the length octet first */
    rval = WASN1_Length(od, (INT_type) (nbytes+1));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write the 'lastbyte' */
    rval = IO_WriteByte(od, lastbyte);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write the rest octets */
    for (; nbytes>=1; nbytes--) {
	nextbyte = val >> ((nbytes-1)*8);
	rval = IO_WriteByte(od, nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
    }

#endif

#ifdef ONES_COMPLEMENT

INT_type OnesComplement(od, val)
    IN INT_type	    od;
    IN INT_type	    val;
{
    INT_type	rval;	    /* The returned value */
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
#ifndef DONT_FORMAT_ERRORS
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[OnesComplement]: Ones complement is not implemented.");
    dummy = TKERR_EndErrMsg();
#endif

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

#endif

static INT_type PositiveInteger(od, val)
    IN INT_type	    od;
    IN INT_type	    val;
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	nbytes;
    REGISTER INT_type	leftover;
    REGISTER BYTE_type	lastbyte;
    REGISTER BYTE_type	nextbyte;
    INT_type		dummy;
	

    static BYTE_type fill[] = {

	/* 0 */ 0x0,	/* unused */
	/* 1 */	0x1,	/* 0000 0001 */
	/* 2 */	0x3,	/* 0000 0011 */
	/* 3 */	0x7,	/* 0000 0111 */
	/* 4 */	0xf,	/* 0000 1111 */
	/* 5 */	0x1f,	/* 0001 1111 */
	/* 6 */	0x3f,	/* 0011 1111 */
	/* 7 */	0x7f,	/* 0111 1111 */
    };

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /*
       1st step is to deal with a partial byte (i.e., not all 8 bits).
    */
    leftover = ((INT_type)BITS_PER_INT) % 8;
    if (leftover != 0) {
	lastbyte = (val >> (((INT_type)BITS_PER_INT)/8));
	/* Sign extend this byte */
	lastbyte &= fill[leftover];
    } else {
	/* No partial byte,  it's like with an extra byte of all 0's */
	lastbyte = (BYTE_type) 0x0;
    }

   
    /* Write out the octets according to the ASN.1 */
    for (nbytes=((INT_type)BITS_PER_INT)/8; nbytes>=1; nbytes--) {
	nextbyte = val >> ((nbytes-1) * 8);
	if ((lastbyte | 0x0) == 0x0) {
	    if ((nextbyte & 0x80) == 0x80) {
		/* The case of '00 ff'.  Save the last byte, i.e., ox00,  and break out */
		break;
	    }
	    lastbyte = nextbyte;
	}
	else if ((~lastbyte & 0x80) == 0) {
	    /* The sign bit is wrong */
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[PositiveInteger]: The sign bit of a positive integer (%d) is wrong.", val);
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
	}
	else { 
	    /* This is the first byte to write,  save it in 'lastbyte' and  break out */
	    break;
	}
    }

    /* For 0x0 specifically */
    if ((lastbyte == 0x0) & (nbytes == 0)) {
	/* Write the length octet first */
	rval = WASN1_Length(od, (INT_type) 1);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	/* The content itself */
	rval = IO_WriteByte(od, (BYTE_type) 0x0);
	goto LEAVE;
    }
	
    /* In cases other than a '0', write the length octet first */
    rval = WASN1_Length(od, (INT_type) (nbytes+1));
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Then write the 'lastbyte' saved */
    rval = IO_WriteByte(od, lastbyte);
    if (rval == ERROR_INT ) {
	goto LEAVE;
    }

    /* Write the rest octets */
    for (; nbytes>=1; nbytes--) {
	nextbyte = val >> ((nbytes-1)*8);
	rval = IO_WriteByte(od, nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return (rval);
    }

INT_type Nbytes(value, bits_per_byte)
    IN INT_type	value;
    IN INT_type	    bits_per_byte;
{
	/* Find how many bytes needed for a non-negative integer:
	    a) 7 bits per byte
            b) 8 bits per byte
        */
	    INT_type	rval;
REGISTER    INT_type	last;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    if (value < 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[Nbytes]: the parameter (%d) should be a  non-negative integer.", value);
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
    
    /* the case of 0 */
    if (value == 0) {
	rval = 1;
	goto LEAVE;
    }

    /* See how many significant bits */
    for (last=((INT_type)BITS_PER_INT); last>0; last--) {
	if ((value & (1 <<(last-1))) != 0) {
	    break;
	}
    }
    if (last == (INT_type) BITS_PER_INT) {
	/* Can't happen */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[Nbytes]: The sign bit of a non-negative integer (%d) is wrong.", value);
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
  
    rval=(last-1)/bits_per_byte+1;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_Integer(od, tag, val)
    IN INT_type	    od;
    IN INT_type	    tag;
    IN INT_type	    val;
{   /* 
      Write the id, length and content octets for an integer 
     */
    

	     INT_type	rval;	    /* The returned value */

	     
#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Write the id octet for an integer */
    
    rval = IO_WriteByte(od, MakeByte(tag));
    if (rval == ERROR_INT ) {
	goto LEAVE;
    }

    /* The length and the value */
    /* Special case 0 */
    if (val == 0) {
	rval = WASN1_Length(od, (INT_type) 1);
	if (rval != ERROR_INT) {
	    rval = IO_WriteByte(od, (BYTE_type) 0);
	}
	goto LEAVE;
    }

    /* If negative, do elsewhere */
    if (val < 0) {
	rval = NegativeInteger(od, val); 
	goto LEAVE;
    }

    /* Now... positive,  see how many bytes are needed -- write the length & then the bytes */
    rval = PositiveInteger(od, val);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type WASN1_ValueType(od, object, tag, attrid)
    IN INT_type	    od;
    IN CONSTITUENT  object;
    IN INT_type	    tag;
    IN INT_type	    attrid;
{
    INT_type		rval;	    /* The returned value */
    ATTR_attribute_t	attr;
    VALUE_type		value;
    INT_type		val;
    INT_type		dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Get the "value type" attribute & look at it */
    attr = ATTR_FindAttr(object, attrid);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    value = GET_VALUE(attr);
    switch (value) {
	
	/* at_OBJCET_TYPE( layout) */
	case at_OBJECT_TYPE_doc_layout_root:
	    val = 0;
	    break;
	case at_OBJECT_TYPE_page_set:
	    val = 1;
	    break;
	case at_OBJECT_TYPE_page:
	    val = 2;
	    break;
	case at_OBJECT_TYPE_frame:
	    val = 3;
	    break;
	case at_OBJECT_TYPE_block:
	    val = 4;
	    break;
	
	/* at_OBJECT_TYPE (logical) */
	case at_OBJECT_TYPE_doc_logical_root:
	    val = 0;
	    break; 
	case at_OBJECT_TYPE_comp_logical_obj:
	    val = 1;
	    break;
	case at_OBJECT_TYPE_bas_logical_obj:
	    val = 2;
	    break;    

	/* at_CONTENT_TYPE */
	case at_CONTENT_TYPE_form_rgca:
	    val = 0;
	    break;    
	/* at_POSITION */
	case at_POSITION_right_hand_aligned:
	    val = 0;
	    break;  
	case at_POSITION_centred:
	    val = 1;
	    break; 
	case at_POSITION_left_hand_aligned:
	    val = 2;
	    break;
	case at_POSITION_normal_order:
	    val = 3;
	    break;
	case at_POSITION_reverse_order:
	    val = 4;
	    break;
  
	/* at_BORDER */
	case at_BORDER_solid:
	    val = 0;
	    break;   
	case at_BORDER_dashed:
	    val = 1;
	    break;  
	 case at_BORDER_dot:
	    val = 2;
	    break;    
	case at_BORDER_dash_dot:
	    val = 3;
	    break;   
	case at_BORDER_dash_dot_dot:
	    val = 4;
	    break;  
	case at_BORDER_invisible:
	    val = 5;
	    break;
	    
	/* at_LAYOUT_PATH */
	case at_LAYOUT_PATH_type_0:
	    val = 0;
	    break;   
	case at_LAYOUT_PATH_type_90:
	    val = 1;
	    break;  
	case at_LAYOUT_PATH_type_180:
	    val = 2;
	    break;    
	case at_LAYOUT_PATH_type_270:
	    val = 3;
	    break;

	    
	 /* at_TRANSPARENCY */
	case at_TRANSPARENCY_transparent:
	    val = 0;
	    break;   
	case at_TRANSPARENCY_opaque:
	    val = 1;
	    break;  

   
	/* at_COLOUR */
	case at_COLOUR_colourless:
	    val = 0;
	    break;   
	case at_COLOUR_white:
	    val = 1;
	    break;  


	/* at_MEDIUM_TYPE */
	case at_MEDIUM_TYPE_recto:
	    val = 0;
	    break;   
	case at_MEDIUM_TYPE_verso:
	    val = 1;
	    break;  
	case at_MEDIUM_TYPE_unspecified:
	    val = 2;
	    break;       

	/* at_PROTECTION */
	case at_PROTECTION_protected:
	    val = 0;
	    break;   
	case at_PROTECTION_unprotected:
	    val = 1;
	    break;      

	/* at_BLK_ALGNM */
	case at_BLK_ALGNM_right_hand_aligned:
	    val = 0;
	    break;   
	case at_BLK_ALGNM_left_hand_aligned:
	    val = 1;
	    break;  
	case at_BLK_ALGNM_centred:
	    val = 2;
	    break;  
#ifdef UNKNOWNASN1  
	case at_BLK_ALGNM_null:
	    val = 3;
	    break; 
#endif 

	/* at_CONCATENATION */
	case at_CONCATENATION_concatenated:
	    val = 0;
	    break;   
	case at_CONCATENATION_non_concated:
	    val = 1;
	    break;      

	/* at_FILL_ORDER */
	case at_FILL_ORDER_reverse_order:
	    val = 0;
	    break;   
	case at_FILL_ORDER_normal_order:
	    val = 1;
	    break; 

	/* dp_GENERIC_LAY_STRUCT */
	case dp_GENERIC_LAY_STRUCT_fact_set:
	    val = 0;
	    break;   
	case dp_GENERIC_LAY_STRUCT_part_gen:
	    val = 1;
	    break;  
	case dp_GENERIC_LAY_STRUCT_comp_gen:
	    val = 2;
	    break;      

	/* dp_SPECIFIC_LAY_STRUCT */
	case dp_SPECIFIC_LAY_STRUCT_present:
	    val = 0;
	    break;     

	/* dp_GENERIC_LOG_STRUCT */
	case dp_GENERIC_LOG_STRUCT_fact_set:
	    val = 0;
	    break;   
	case dp_GENERIC_LOG_STRUCT_part_gen:
	    val = 1;
	    break;  
	case dp_GENERIC_LOG_STRUCT_comp_gen:
	    val = 2;
	    break;     

	/* dp_SPECIFIC_LOG_STRUCT */
	case dp_SPECIFIC_LOG_STRUCT_present:
	    val = 0;
	    break; 

	/* dp_LAYOUT_STYLES */
	case dp_LAYOUT_STYLES_present:
	    val = 0;
	    break;

	/* dp_PRESENTATION_STYLES */
	case dp_PRESENTATION_STYLES_present:
	    val = 0;
	    break;
	/* dp_INTERCHANGE_FMT_CLASS */
	case dp_INTERCHANGE_FMT_CLASS_a:
	    val = 0;
	    break;  
	case dp_INTERCHANGE_FMT_CLASS_b:
	    val = 1;
	    break; 
	    
	/* cc_ALIGNMENT  */
	case cc_ALIGNMENT_start_aligned:
	    val = 0;
	    break;   
	case cc_ALIGNMENT_end_aligned:
	    val = 1;
	    break;  
	case cc_ALIGNMENT_centred:
	    val = 2;
	    break;    
	case cc_ALIGNMENT_justified:
	    val = 3;
	    break;     

	/* cc_CHAR_ORIENTATION  */
	case cc_CHAR_ORIENTATION_0:
	    val = 0;
	    break;   
	case cc_CHAR_ORIENTATION_90:
	    val = 1;
	    break;  
	case cc_CHAR_ORIENTATION_180:
	    val = 2;
	    break;    
	case cc_CHAR_ORIENTATION_270:
	    val = 3;
	    break;     

	/* cc_CHARACTER_PATH  */
	case cc_CHARACTER_PATH_0:
	    val = 0;
	    break;   
	case cc_CHARACTER_PATH_90:
	    val = 1;
	    break;  
	case cc_CHARACTER_PATH_180:
	    val = 2;
	    break;    
	case cc_CHARACTER_PATH_270:
	    val = 3;
	    break;       

      
	/* cc_Itemisation -- is used in struct, thus to be found in WASN1_Itemisation */

	/* cc_LINE_LAYOUT_TABLE  */
	case cc_LINE_LAYOUT_TABLE_start_alnd:
	    val = 0;
	    break;   
	case cc_LINE_LAYOUT_TABLE_end_alnd:
	    val = 1;
	    break;  
	case cc_LINE_LAYOUT_TABLE_centred:
	    val = 2;
	    break;    
	case cc_LINE_LAYOUT_TABLE_alnd_arnd:
	    val = 3;
	    break;     

        /* cc_LINE_PROGRESSION  */
	case cc_LINE_PROGRESSION_90:
	    val = 1;
	    break;   
	case cc_LINE_PROGRESSION_270:
	    val = 3;
	    break; 

	

	/* cc_FORMATTING_INDICATOR  */
	case cc_FORMATTING_INDICATOR_yes:
	    val = 0;
	    break;   
	case cc_FORMATTING_INDICATOR_no:
	    val = 1;
	    break;

   
	/* cc_PROP_LINE_SAPCING  */
	case cc_PROP_LINE_SPACING_yes:
	    val = 0;
	    break;   
	case cc_PROP_LINE_SPACING_no:
	    val = 1;
	    break;   

    
	/* cc_PAIRWISE_KERNING  */
	case cc_PAIRWISE_KERNING_yes:
	    val = 0;
	    break;   
	case cc_PAIRWISE_KERNING_no:
	    val = 1;
	    break;   

	/* rg_IMAGE_DIMENSIONS  */
	case rg_IMAGE_DIMENSIONS_fixed:
	    val = 0;
	    break;   
	case rg_IMAGE_DIMENSIONS_variable:
	    val = 1;
	    break;   

        /* rg_PEL_PATH  */
	case rg_PEL_PATH_0:
	    val = 0;
	    break;   
	case rg_PEL_PATH_90:
	    val = 1;
	    break;  
	case rg_PEL_PATH_180:
	    val = 2;
	    break;    
	case rg_PEL_PATH_270:
	    val = 3;
	    break;     

        /* rg_LINE_PROGRESSION  */
	case rg_LINE_PROGRESSION_90:
	    val = 1;
	    break;   
	case rg_LINE_PROGRESSION_270:
	    val = 3;
	    break;   

	/* rg_PEL_TRANS_DENSITY */
	case rg_PEL_TRANS_DENSITY_1:
	    val = 0;
	    break;   
	case rg_PEL_TRANS_DENSITY_2:
	    val = 1;
	    break;  
	case rg_PEL_TRANS_DENSITY_3:
	    val = 2;
	    break;    
	case rg_PEL_TRANS_DENSITY_4:
	    val = 3;
	    break;   
	case rg_PEL_TRANS_DENSITY_5:
	    val = 4;
	    break;  
	case rg_PEL_TRANS_DENSITY_6:
	    val = 5;
	    break;  

	/* rg_COMPRESSION  */
	case rg_COMPRESSION_compressed:
	    val = 1;
	    break;   
	case rg_COMPRESSION_uncompressed:
	    val = 3;
	    break;   

	
    
	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[WASN1_ValueType]: Invalid value (%d).", (INT_type) value);
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
    }
    
    rval = WASN1_Integer(od, tag, val);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}
