/*  $Header: /u/04c/mnt/integ/dm/include/RCS/drc.h,v 1.7 89/10/06 10:09:31 rcs Exp $  */
/*  $Log:	drc.h,v $
 * Revision 1.7  89/10/06  10:09:31  rcs
 * changes to support typeless buffesr
 * 
 * Revision 1.6  89/06/15  15:29:22  ljb
 * add conversion routines for buffer and boolean
 * 
 * Revision 1.5  89/02/09  16:51:09  ejh
 * added copyright notice
 * 
 * Revision 1.4  89/02/07  20:05:56  ejh
 * removed MAXSTR
 * 
 * Revision 1.3  88/10/11  16:51:22  ljb
 * add buffer to integer and vice versa
 * 
 * Revision 1.2  88/10/03  14:55:06  ljb
 * add routines strtobuff and bufftostr to allow conversion between buffers and strings
 *   */
/*--------------------------------------------------------------------------*\
|
|  Name:  Run Time Conversions Utility (drc)
|
|  Description:  
|     The run time conversions utility provides low level type conversions
|     that are either not avaialalbe or do not return the appropriate
|     parameters.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     drc.c - contains the implementation for the transactionmanager.
|     drc.h - contains the external definition for the transaction manager.
|
|  Project:
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     June 30, 1988 : Module created.
|     Oct 1, 1988: added strtobuff and bufftostr   ljb
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#ifndef drc
#define drc

/*--------------------------------------------------------------------------*\
|  Includes: isd
|
|  Description:
|     The following is a list of all the necessary includes for the 
|     run time operations utility.
|
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <sys/types.h>          /* System types                             */

#define memoryPack              /* use memoryPack rather than mallocPack    */
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "idd.h"		/* interface data definitition abstraction  */
#include "isd.h"		/* interface shared data abstraction	    */
#include "cus.h"		/* compiler symbol table management package */

#include "ubo.h"		/* buffer operations			    */
#include "dtt.h"		/* transaction table			    */
#include "dob.h"		/* object base				    */
#include "dro.h"

#include <math.h>
#include <ctype.h>
#include <string.h>

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: drc_debug
|
|  Description:
|     Debug variable for the run time conversions utility.
|
\* Definition:                                                              */

#ifdef drc_main
  int drc_debug = 3;
#else
  extern int drc_debug;
#endif

#define issign(ch) ((ch == '+') || (ch == '-'))
#define isexp(ch)  ((ch == 'E') || (ch == 'e'))
#define isperiod(ch) (ch == '.')
#define UNDEF 0	

/*--------------------------------------------------------------------------*\
|  Routine: isinteger
| 
|  Description:  
|     Check if a string is an integer string
| 
\* Syntax:                                                                  */
	int isinteger(
	  /* str: ascii string */
	);
/* 
|  Parameters:
|     value - ascii string
|
|  Returns:  
|	true if is ascii integer, false otherwise
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: isreal
| 
|  Description:  
|     Check if a string is a real string
| 
\* Syntax:                                                                  */
 	int isreal(
	  /* str: ascii string */
	);
/* 
|  Parameters:
|     value - ascii string
|
|  Returns:  
|     true if is an ascii real, else false = 0
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_get_atv
| 
|  Description:
|     The dro get appropriately typed value routine accepts an operand and
|     a conversion type and returns a value of the conversion type or sets
|     status to invalid_conversion.
\* Syntax :								   */

	caddr_t drc_get_atv(
	  /* operand : dro_atomic_data_item */
	  /* data_type : idd_data_types */
	);
/*
|  Parameters:
|     value - dro_drohandle, idd_data_types
|
|  Returns:  
|     value in new idd_data_types
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: drc_get_undefined
| 
|  Description:
|     Returns the "correct" undefined value.
|
\* Syntax :								   */

	caddr_t drc_get_undefined(
	  /* desired_type : idd_data_types */
	);
/*
|  Parameters:
|     desired_type - the type of undefined we're looking for.
|
|  Returns:  
|     The "correct" undefined value.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: rtoi
| 
|  Description:  
|     Converts a real value to an integer value.
| 
\* Syntax:                                                                  */

        caddr_t rtoi(
          /* value : in double */
        );

/* 
|  Parameters:
|     value - pointer to Double precision real.
|
|  Returns:  
|     integer
|  
|  Statuses:  
|     ok, failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: itor
| 
|  Description:  
|     Converts an integer value to a real value.
| 
\* Syntax:                                                                  */
	caddr_t itor(
	  /* num: integer */
	);
/* 
|  Parameters:
|     value - integer
|
|  Returns:  
|     pointer to real.
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: itoa
| 
|  Description:  
|     Converts an integer value to an ascii.
| 
\* Syntax:                                                                  */
	caddr_t itoa(
	  /* num: integer */
	);
/* 
|  Parameters:
|     value - integer
|
|  Returns:  
|     ascii string
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: rtoa
| 
|  Description:  
|     Converts a real value to an ascii.
| 
\* Syntax:                                                                  */
	caddr_t rtoa(
	  /* num : caddr_t, a pointer to real */
	);
/* 
|  Parameters:
|     value - pointer to real
|
|  Returns:  
|     ascii string.
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: booltostr
| 
|  Description:  
|     Converts a boolean  to an ascii "true" or "false"
| 
\* Syntax:                                                                  */
  	caddr_t booltostr(
	  /* bool : boolean */
	);
/* 
|  Parameters:
|     value - boolean
|
|  Returns:  
|     ascii string "true" or "false"
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: astoi
| 
|  Description:  
|     Converts an ascii to an integer value.
| 
\* Syntax:                                                                  */
	caddr_t astoi(
	  /* num : char *, ascii string */
	);

/* 
|  Parameters:
|     value - ASCII string.
|
|  Returns:  
|     integer
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: astor
| 
|  Description:  
|     Converts an ascii value to a real value.
| 
\* Syntax:                                                                  */
	caddr_t astor(
	  /* num : char *, ascii string */
	);
	
/* 
|  Parameters:
|     value - ASCII string.
|
|  Returns:  
|     pointer to real (double *)
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: error
| 
|  Description:  
|     Sets an illegal conversion error.
| 
\* Syntax:                                                                  */

        int error(
          /* value : in string */
        );

/* 
|  Parameters:
|     value - ASCII string.
|
|  Returns:  
|     -1.
|  
|  Statuses:  
|     failed_conversion.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: strtobuff
| 
|  Description:  
|     This routine takes as input a string and returns a buffer
|     with the same length and body
| 
\* Syntax:                                                                  */
	caddr_t strtobuff(
	  /* arg: char *  */
	);
/* 
|  Parameters:
|     arg - pointer to string
|
|  Returns:  
|     pointer to buffer
|  
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: inttobuff
| 
|  Description:  
|     This routine takes as input an integer and returns a buffer
|     with the body of the integer in string form
| 
\* Syntax:                                                                  */
	caddr_t inttobuff(
	  /* arg:   int*/
	);
/* 
|  Parameters:
|     arg - integer
|
|  Returns:  
|     pointer to buffer
|  
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: convert_from_buffer
| 
|  Description:  
|    Converts a buffer to anything.  Buffers of buffer type can not be
|    converted to anything.  Everything else is handled by a recursive
|    call to funcall.
| 
\* Syntax:                                                                  */

	caddr_t convert_from_buffer(
	  /* arg: idd_buffer_type 	*/
	);

/* 
|  Parameters:
|     arg - buffer.
|
|  Returns:  
|     The converted value.
|  
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: booltoint
|
|  Description:
|     Converts a boolean value to an integer value.
|
\* Syntax:                                                                  */

        caddr_t booltoint(
          /* value : as boolean */
        );

/*
|  Parameters:
|     value - pointer to boolean.
|
|  Returns:
|     integer
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: booltoreal
|
|  Description:
|     Converts a boolean value to a real value.
|
\* Syntax:                                                                  */

        caddr_t booltoreal(
          /* value : as boolean */
        );

/*
|  Parameters:
|     value - pointer to boolean.
|
|  Returns:
|     real
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: booltobuff
|
|  Description:
|     Converts a boolean value to a buffer value.
|
\* Syntax:                                                                  */

        caddr_t booltobuff(
          /* value : as boolean */
        );

/*
|  Parameters:
|     value - pointer to boolean.
|
|  Returns:
|     pointer to buffer
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: inttobool
| 
|  Description:  
|     This routine takes as input an integer and returns a boolean
| 
\* Syntax:                                                                  */
	caddr_t inttobool(
	  /* arg:   int*/
	);
/* 
|  Parameters:
|     arg - integer
|
|  Returns:  
|     integer
|  
|  Statuses:  
|     none, failed_conversion
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: realtobool
| 
|  Description:  
|     This routine takes as input a real and returns a boolean
| 
\* Syntax:                                                                  */
	caddr_t realtobool(
	  /* arg:   double value*/
	);
/* 
|  Parameters:
|     arg - pointer to double
|
|  Returns:  
|     integer
|  
|  Statuses:  
|     none, failed_conversion
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: strtobool
| 
|  Description:  
|     This routine takes as input a string and returns a boolean
| 
\* Syntax:                                                                  */
	caddr_t strtobool(
	  /* arg:   string*/
	);
/* 
|  Parameters:
|     arg - string
|
|  Returns:  
|     integer
|  
|  Statuses:  
|     none, failed_conversion
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idtobuff
| 
|  Description:  
|     This routine takes as input an id and returns a buffer
|     with the same body (binary representation)
| 
\* Syntax:                                                                  */
	caddr_t idtobuff(
	  /* arg: id *  */
	);
/* 
|  Parameters:
|     arg - id
|
|  Returns:  
|     pointer to buffer
|  
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: realtobuff
| 
|  Description:  
|     This routine takes as input a real and returns a buffer
|     with the same body (binary representation)
| 
\* Syntax:                                                                  */
	caddr_t realtobuff(
	  /* arg: double *  */
	);
/* 
|  Parameters:
|     arg - pointer to double
|
|  Returns:  
|     pointer to buffer
|  
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/
#endif
