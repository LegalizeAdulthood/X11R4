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
 *  File:   rasio.c
 *  Author: Andrew Werth
 *
 *  Raster input/output functions (read/write).
 *
 */

#define TK_TKI_rast
#define TK_TKI_init
#define TK_TKI_rio
#define TK_IMPL_rio
#define TK_GVARS_rasio

#include <rgsite.h>
#include <rginit.h>
#include <raster.h>
#include <rasio.h>

#undef TK_GVARS_rasio
#undef TK_IMPL_rio
#undef TK_TKI_rio
#undef TK_TKI_init
#undef TK_TKI_rast

#ifndef USE_LONG_NAMES

#define DoReadRaster	    dordra
#define DoWriteRaster	    dowrra
#define GetByteFromId	    gtbyid
#define GetByteFromMem	    gtbyme
#define PutByteToMem	    ptbyme
#define PutByteToOd	    ptbyod

#define rast_BaseBuff	    rababf
#define rast_CurPos	    racrpo
#define rast_StartPos	    rastpo
#define rast_WriteBlocks    rawrbl
#define rast_WriteSize	    rawrsz

#endif

static RASTER_type *DoReadRaster();
static INT_type PutByteToMem();
static INT_type GetByteFromId();
static INT_type GetByteFromMem();
static INT_type PutByteToOd();

static INT_type	    rast_WriteSize;
static INT_type	    rast_WriteBlocks;
static BYTE_type    *rast_BaseBuff;
static INT_type	    rast_StartPos;
static INT_type	    rast_CurPos;

extern CHAR_type    ETC_MagicBuffer[];
extern INT_type	    ETC_MagicSize;
extern INIT_RGPARMS_type *INIT_RGParms;

BOOL_type	    RIO_FromMemory = BOOL_false;


/*\
 *  READRASTER:  Read a raster from an input descriptor, in a 
 *	         certain graphics format.
\*/

RASTER_type *ReadRaster(id, format)
    IN	INT_type    id;
    IN	FORMAT_type *format;
{
    RASTER_type	    *rval;
    INT_type	    (*function)();

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    function = GetByteFromId;

    RIO_FromMemory = BOOL_false;
    rval = DoReadRaster(function, (POINTER_type) &id, format);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  READMEMRASTER:  Read a raster in from a spot in memory,
 *		    in a certain graphics format.
\*/

RASTER_type *ReadMemRaster(memory, format)
    POINTER_type    memory;
    FORMAT_type	    *format;
{
    RASTER_type	    *rval;
    INT_type	    (*function)();

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    function = GetByteFromMem;

    RIO_FromMemory = BOOL_true;
    rval = DoReadRaster(function, memory, format);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  DOREADRASTER:  The actual function which reads from the input
 *		   stream (either a descriptor or a chunk of memory)
 *		   and converts it into the internal raster format.
\*/

static RASTER_type *DoReadRaster(GetByte, parameter, format)
    INT_type	    (*GetByte)();
    POINTER_type    parameter;
    FORMAT_type	    *format;
{
    RASTER_type	    *rval;
    VALUE_type	    fmt;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    /*
     * Bytes are read using this function call:
     *    byte = (*GetByte)(parameter);
     *
     * This will automatically get a byte from either an input
     * descriptor or from memory.
     *
     */

    switch (format->format_tag) {
	case FORMAT_DETERMINE:
	    rval = ETC_ReadRaster(GetByte, parameter, &fmt);
	    format->format_tag = fmt;
	    break;			
	case FORMAT_PBM:
	    rval = PBM_ReadRaster(GetByte, parameter, 
				    format->format_body.pbm);
	    break;
	case FORMAT_GROUP3:
	    rval = G3_ReadRaster(GetByte, parameter,
				    &format->format_body.g3);
	    break;
	case FORMAT_GROUP4:
	    rval = G4_ReadRaster(GetByte, parameter,
				    &format->format_body.g4);
	    break;
	case FORMAT_MACPAINT:
	    rval = MACP_ReadRaster(GetByte, parameter,
				    &format->format_body.macpaint);
	    break;
	case FORMAT_ANDREW:
	    rval = ANDY_ReadRaster(GetByte, parameter,
				    &format->format_body.andrew);
	    break;
	case FORMAT_ODABM:
	    rval = OBM_ReadRaster(GetByte, parameter,
				   &format->format_body.oda_bitmap);
	    break;
	case FORMAT_XBM:
	    rval = X_ReadRaster(GetByte, parameter,
				   &format->format_body.xbm);
	    break;
	case FORMAT_SUN:
	case FORMAT_PCX:
	case FORMAT_GIF:
	case FORMAT_IFF:
	    rval = FBM_ReadRaster(GetByte, parameter, format);
	    break;
	default:
	    rval = RG_ERROR_RASTER;
	    RGError = RG_BADPARM_err;
	    goto LEAVE;
    }
    if (rval != RG_ERROR_RASTER) {
        rval->format = (POINTER_type) format;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  GETBYTEFROMID:  Routine which gets a byte from the input
 *		    descriptor pointed to by parameter.
\*/

static INT_type GetByteFromId(parameter)
    POINTER_type    parameter;
{
    BYTE_type	buf;
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (ETC_MagicSize > 0) {
	rval = ETC_MagicBuffer[10 - ETC_MagicSize--];
    }
    else {
	if (LOCAL_ReadBytes(*((INT_type *)parameter),&buf,1)<= 0) {
	    /* copy over error codes */
	    RGError = LCLError;
	    rval = RG_EOF;
	    goto LEAVE;
	}
	rval = (INT_type) buf;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

/*\
 *  GETBYTEFROMMEM:  Routine which gets a byte from a byte-stream
 *		     in memory, where the current location is
 *		     pointed to by parameter.
\*/

static INT_type GetByteFromMem(parameter)
    POINTER_type    *parameter;
{
    BYTE_type	    rval;
    POINTER_type    memory;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (ETC_MagicSize > 0) {
	rval = ETC_MagicBuffer[--ETC_MagicSize];
    }
    else {
	memory = (*parameter);

	rval = *memory;
	(*parameter) ++;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

/***************  OUTPUT ROUTINES  ****************/

/*\
 *  WRITERASTER:  Write a raster to an output descriptor.
\*/

INT_type WriteRaster(od, raster, format)
    INT_type	    od;
    RASTER_type	    *raster;
    FORMAT_type	    *format;
{
    INT_type	    rval;
    INT_type	    (*function)();

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("RIO_WriteRaster");
    }
#endif

    function = PutByteToOd;

    rval = DoWriteRaster(function,(POINTER_type)&od, format,raster);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("RIO_WriteRaster");
    }
#endif

    return(rval);
}


/*\
 *  WRITEMEMRASTER:  Write a raster to memory.
\*/

BYTE_type *WriteMemRaster(raster, format, start, size)
    RASTER_type	    *raster;
    FORMAT_type	    *format;
    INT_type	    start;
    INT_type	    *size;
{
    BYTE_type	    *rval;
    BYTE_type	    *rvalptr;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rast_StartPos = start;
    rast_CurPos	  = 0;

    if (size == (INT_type *) 0 || *size < 0) {
	rval = (BYTE_type *) RG_ERROR_POINTER;
	RGError = RG_BADPARM_err;
	goto LEAVE;
    }
    if (*size == 0) {
	rval = (BYTE_type *) MEM_Malloc(MEM_BUF_SIZE);
	rvalptr = rval;
	rast_WriteSize = -MEM_BUF_SIZE;
	rast_WriteBlocks = MEM_BUF_SIZE;
	rast_BaseBuff = rval;	
    }
    else {
	if ((rval = (BYTE_type *)MEM_Malloc(*size))==
		    (BYTE_type *)RG_ERROR_POINTER){
	    goto LEAVE;	    
	}
	rvalptr = rval;
	rast_WriteSize = *size;
    }

    if (DoWriteRaster(PutByteToMem, &rvalptr, format, raster)
					== RG_ERROR_INT) {
	if (RGError == -1) {
	    *size = (INT_type) (rvalptr - rval);
	    goto LEAVE;	    
	}
	rval = (BYTE_type *)RG_ERROR_POINTER;
	goto LEAVE;
    }
    *size = (INT_type) (rvalptr - rval);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}



/*\
 *  DOWRITERASTER:  Calls the correct readraster routine.
\*/

static INT_type DoWriteRaster(PutByte, parameter, format, raster)
    BYTE_type	    (*PutByte)();
    POINTER_type    parameter;
    FORMAT_type	    *format;
    RASTER_type	    *raster;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    /*
     * Bytes are written using this function call:
     *    (*PutByte)(parameter, byte);
     *
     * This will automatically send a byte to either an input
     * descriptor or from memory.
     *
     */

    switch (format->format_tag) {
	case FORMAT_DETERMINE:
	    if (raster->format == (POINTER_type) 0) {
		format->format_tag = INIT_RGParms->default_format;
		rval = DoWriteRaster(PutByte, parameter,
					    format, raster);
		goto LEAVE;
	    }
	    rval = DoWriteRaster(PutByte, parameter, 
			    (FORMAT_type *) raster->format, raster);
	    break;
	case FORMAT_MACPAINT:
	    rval = MACP_WriteRaster(PutByte, parameter,
			&format->format_body.macpaint,raster);
	    break;
	case FORMAT_PBM:
	    rval = PBM_WriteRaster(PutByte, parameter, 
				    format->format_body.pbm,raster);
	    break;
	case FORMAT_GROUP3:
	    rval = G3_WriteRaster(PutByte, parameter,
				    &format->format_body.g3,raster);
	    break;
	case FORMAT_GROUP4:
	    rval = G4_WriteRaster(PutByte, parameter,
				    &format->format_body.g4,raster);
	    break;
	case FORMAT_ODABM:
	    rval = OBM_WriteRaster(PutByte, parameter,
			&format->format_body.oda_bitmap,raster);
	    break;
	case FORMAT_XBM:
	    rval = X_WriteRaster(PutByte, parameter,
			&format->format_body.xbm,raster);
	    break;
	case FORMAT_ANDREW:
	    rval = ANDY_WriteRaster(PutByte, parameter,
			&format->format_body.andrew,raster);
	    break;
	case FORMAT_POSTSCRIPT:
	    rval = PS_WriteRaster(PutByte, parameter,
			&format->format_body.ps,raster);
	    break;
	case FORMAT_IFF:
	case FORMAT_GIF:
	case FORMAT_PCX:
	case FORMAT_SUN:
	    rval = FBM_WriteRaster(PutByte, parameter, format);
	    break;
	default:
	    rval = RG_ERROR_INT;
	    RGError = RG_BADPARM_err;
	    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

/*\
 *  PUTBYTETOOD:  Puts a byte ch to the output descriptor pointed
 *		  to by parameter.
\*/

static INT_type PutByteToOd(parameter, ch)
    POINTER_type    parameter;
    BYTE_type	    ch;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutByteToOd");
    }
#endif

    rval = LOCAL_WriteBytes(*((INT_type *) parameter), &ch, 1);
    if (rval == LCL_ERROR_INT) {
	/* copy over error code */
	rval = RG_ERROR_INT;
	RGError = LCLError;
    }
/*  rval = write(*((INT_type *) parameter), &ch, 1);  */

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutByteToOd");
    }
#endif

    return(rval);
}

/*\
 *  PUTBYTETOMEM:  Puts a byte ch to the memory address pointed to
 *		   by parameter.
\*/

static INT_type PutByteToMem(parameter, ch)
    POINTER_type    *parameter;
    BYTE_type	    ch;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    /* Check if current position is such that the byte should
     * actually get written to memory.  If not, just skip the
     * memory write.
     */

    if (rast_CurPos++ < rast_StartPos) {
	rval = RG_SUCCESS_INT;
	goto LEAVE;
    }

    /* Check to see if all the desired bytes have been written.
     * If the entire raster is being written, then the tool kit
     * reads in blocks of MEM_BUF_SIZE bytes at a time, reallocating
     * the buffer whenever a new block is required.
     */

    if (rast_WriteSize > 0) {
	*((BYTE_type *) *parameter) = ch;
	(*parameter) ++;
	rval = RG_SUCCESS_INT;
	rast_WriteSize--;
    }
    else if (rast_WriteSize < 0) {
	*((BYTE_type *) *parameter) = ch;
	(*parameter) ++;
	rval = RG_SUCCESS_INT;
	rast_WriteSize++;
	if (rast_WriteSize == 0) {  /* Time to realloc the block */
	    rast_WriteBlocks += MEM_BUF_SIZE;
	    rast_BaseBuff = (BYTE_type *) MEM_Realloc
				 (rast_BaseBuff, rast_WriteBlocks);
	    rast_WriteSize = -MEM_BUF_SIZE;
	}
    }
    else { 

	/* This is NOT an error, RGError == -1 means the write is
	 * complete.
	 */
	rval = RG_ERROR_INT;
	RGError = -1;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}



/*  End of file .c  */
