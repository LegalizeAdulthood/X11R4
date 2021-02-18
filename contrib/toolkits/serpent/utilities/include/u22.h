/* $Header: /u/04c/mnt/integ/utilities/include/RCS/u22.h,v 1.4 89/10/18 10:13:46 ejh Exp $ */
/*--------------------------------------------------------------------------*\
|
|  Name:  u22.h
|
|  Description:  
|     Global declarations of general interest to Serpent source modules.
|     Specifically, this file contains useful macros and definitions that
|     are useful across the Serpent system.
|
|  Component of:
|     Utilities Layer.
|
|  Related files:
|     None.
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
|     Brian M. Clapper
|     bmc@sei.cmu.edu  
|
|  Change History:
|     30 Aug 1989 : Module created.
$Log:	u22.h,v $
 * Revision 1.4  89/10/18  10:13:46  ejh
 * created u22_convert_string (came from a22); defined constants UPPERCASE
 * and LOWERCASE associated with it
 * 
 * Revision 1.3  89/10/02  14:20:53  serpent
 * added include of global.h so we get the definition of string
 * 
 * Revision 1.2  89/09/27  23:02:01  bmc
 * Added u22_copy_string routine.
 * 
 * Revision 1.1  89/08/30  14:51:48  bmc
 * Initial revision
 * 
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

#ifndef _u22_h_
#define _u22_h_

#include <ctype.h>
#include "global.h"

/*--------------------------------------------------------------------------*\
|  Macros: Character Conversion
|
|  Description:
|     The standard tolower() and toupper() functions (defined in <ctype.h>)
|     are handled inconsistently across UNIX systems.  These macros provide
|     a portable front-end to the functions.
|
|     ToLower(a)	If the argument is an upper-case alphabetic, the
|                       macro returns its lower-case equivalent.  Otherwise,
|                       the argument is returned unchanged.
|     ToUpper(a)	If the argument is an lower-case alphabetic, the
|                       macro returns its lower-case equivalent.  Otherwise,
|                       the argument is returned unchanged.
|
|  Note: The inclusion of <ctype.h> preceding this comment is REQUIRED!
|
\* Definition:                                                              */

#define ToLower(a) 	( isupper ((a)) ? tolower ((a)) : (a) );
#define ToUpper(a) 	( islower ((a)) ? toupper ((a)) : (a) );

/*--------------------------------------------------------------------------*\
|  Constants:	UPPERCASE, LOWERCASE
| 
|  Description:
|	used to tell u22_convert_string which case to convert to
| 
\* Definition:                                                              */

#define	UPPERCASE	1
#define	LOWERCASE	2

/*                                                                          *\
|
|  Components:
|	UPPERCASE	used to tell convert_string to convert to upper case
|	LOWERCASE	used to tell convert_string to convert to lower case
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: u22_copy_string (or just copy_string)
| 
|  Description:  
|     Allocates storage for a string and copies the parameter into it.
| 
\* Syntax:                                                                  */

        string u22_copy_string
	    (
	/*  old_string : string */
	    );


/* 
| Macro Synonym
\*                                                                          */

#define	copy_string(old_string) \
	u22_copy_string(old_string)

/* 
|  Parameters:
|     old_string - The string to duplicate.
|
|  Returns:  
|     Exact copy of old_string in an allocated buffer, or NULL in case of
|     memory allocation error.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine:	u22_convert_string
| 
|  Description:
|	converts a name from upper case to lower, or vice versa
| 
\* Definition:                                                              */

	string	u22_convert_string (
	/*	the_name:	in	string,
		the_case:	in	int
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	the_name	the name to be converted
|	the_case	what case it is to be converted to
|
|  Returns:
|	the converted name
|
\*--------------------------------------------------------------------------*/

#endif _u22_h_
