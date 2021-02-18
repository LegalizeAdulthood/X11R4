/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

/*
 *  File:   ioetc.c
 *  Author: Andrew Werth
 *
 *  Routine which attempts to determine which type of raster
 *  is pointed at, and then read it accordingly.
 *
 */

#define TK_TKI_rast
#define TK_TKI_rio
#define TK_IMPL_rio
#define TK_GVARS_ioetc
#define TK_TKI_etc
#define TK_IMPL_etc
#define TK_GVARS_ioetc

#include <rgsite.h>
#include <raster.h>
#include <rasio.h>
#include <ioetc.h>

#undef TK_GVARS_ioetc
#undef TK_IMPL_etc
#undef TK_TKI_etc
#undef TK_IMPL_rio
#undef TK_TKI_rio
#undef TK_TKI_rast

BYTE_type   ETC_MagicBuffer[10];
INT_type    ETC_MagicSize;



/*\
 *  ETC_READRASTER:  Read a raster in a format which is to be
 *		     determined by inspecting the "magic number."
\*/

RASTER_type *ETC_ReadRaster(GetByte, parameter, fmt)
    IN	INT_type	(*GetByte)();
    IN	POINTER_type    parameter;
    OUT	VALUE_type	*fmt;
{
    RASTER_type	    *rval;
    INT_type	    ch;
    FORMAT_type	    format;
    INT_type	    i;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    ETC_MagicSize = 0;

    for (i = 0; i < 10; i++) {
	ch = (*GetByte)(parameter);
	if (ch == RG_ERROR_INT) {
	    rval = RG_ERROR_RASTER;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
	ETC_MagicBuffer[i] = (BYTE_type) ch;
    }
    ETC_MagicSize = i;

    switch (ETC_MagicBuffer[0]) {
	case 'P':
	    format.format_tag = FORMAT_PBM;
	    rval = PBM_ReadRaster(GetByte, parameter,
				    format.format_body.pbm);
	    break;
	case '\\':
	    format.format_tag = FORMAT_ANDREW;
	    rval = ANDY_ReadRaster(GetByte, parameter,
				    &format.format_body.andrew);
	    break;
	case '#':
	    format.format_tag = FORMAT_XBM;
	    rval = X_ReadRaster(GetByte, parameter,
				   &format.format_body.xbm);
	    break;
	case 'F':
	    format.format_tag = FORMAT_IFF;
	    rval = FBM_ReadRaster(GetByte, parameter, &format);
	    break;
	case '\0':
	    if (((ETC_MagicBuffer[1] & 0xf0) >> 4) == 0x01) {
		format.format_tag = FORMAT_GROUP3;
		format.format_body.g3.msb = LEFTBIT;
		rval = G3_ReadRaster(GetByte, parameter,
				    &format.format_body.g3);
		if (rval != RG_ERROR_RASTER) {
		    break;
		}
	    }
	    else if ((ETC_MagicBuffer[1] & 0x0f) == 0x08) {
		format.format_tag = FORMAT_GROUP3;
		format.format_body.g3.msb = RIGHTBIT;
		rval = G3_ReadRaster(GetByte, parameter,
				    &format.format_body.g3);
		if (rval != RG_ERROR_RASTER) {
		    break;
		}
	    }
	    ETC_MagicSize = 10;
	    format.format_tag = FORMAT_MACPAINT;
	    format.format_body.macpaint.macbin =
		(ETC_MagicBuffer[1] == '\0') ? 0 : 2;
	    rval = MACP_ReadRaster(GetByte, parameter,
				    &format.format_body.macpaint);
	    break;
	case 'G':
	    format.format_tag = FORMAT_GIF;
	    rval = FBM_ReadRaster(GetByte, parameter, &format);
	    break;
	case 0x59:
	    format.format_tag = FORMAT_SUN;
	    rval = FBM_ReadRaster(GetByte, parameter, &format);
	    break;
	default:
	    rval = RG_ERROR_RASTER;
	    RGError = RG_NOFORM_err;
	    goto LEAVE;
    }
    *fmt = format.format_tag;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}



/*\
 *  ETC_GETNUMBER:  Get a number from the input descriptor.
\*/

INT_type ETC_GetNumber(GetByte, Parameter)
    INT_type	    (*GetByte)();
    POINTER_type    Parameter;
{
    INT_type	rval;
    INT_type	byte;
    INT_type	Div10;
    INT_type	Mod10;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ETC_GetNumber");
    }
#endif

    rval = 0;
    Div10 = MAXIMUM_INT / 10;
    Mod10 = MAXIMUM_INT % 10;

    while ((byte = (*GetByte)(Parameter)) >= 
		       (INT_type) '0' && (byte <= '9')) {
	if (rval > Div10 || (rval == Div10 && (rval % 10) > Mod10)){
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
	rval = 10 * rval + (byte - '0');
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ETC_GetNumber");
    }
#endif

    return(rval);
}


/*\
 *  ETC_PUTNUMBER:  Puts a number to output.
\*/

INT_type ETC_PutNumber(PutByte, Parameter, number, spaces)
    INT_type	(*PutByte)();
    POINTER_type Parameter;
    INT_type	number;
    INT_type	spaces;
{
    INT_type	rval;
    INT_type	length;
    INT_type	i;
    CHAR_type	buffer[MAX_INT_LENGTH];

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ETC_PutNumber");
    }
#endif

    length = 0;
    i = number;

    if (i == 0) {
	(*PutByte)(Parameter,'0');
	goto END;
    }

    while (i > 0) {
	buffer[length++] = (i % 10) + '0';
	i = i / 10;
    }

    while (--length >= 0) {
	if ((*PutByte)
	    (Parameter,(BYTE_type)buffer[length]) ==RG_ERROR_INT) {

	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

END:
    if (spaces != 0) {
	if ((*PutByte)(Parameter, ' ') == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ETC_PutNumber");
    }
#endif

    return(rval);
}





/*  End of file .c  */
