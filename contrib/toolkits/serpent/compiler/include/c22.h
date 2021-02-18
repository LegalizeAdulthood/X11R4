/*
$Header: /u/04c/mnt/integ/compiler/include/RCS/c22.h,v 1.16 89/10/23 20:05:06 ejh Exp $
*/

/*--------------------------------------------------------------------------*\
|
|  Name:	c22.h
|
|  Description:
|	This file is the external appearance of the SLANG processor.
|
|  Component of:
|	SLANG processor
|
|  Related files:
|	many yacc, lex, and c files
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Author:  
|	Erik J. Hardy
|	ejh@sei.cmu.edu
|
|  Change History:
|	05 Jul 1988:	module created
$Log:	c22.h,v $
 * Revision 1.16  89/10/23  20:05:06  ejh
 * removed c22_convert_name
 * 
 * Revision 1.15  89/08/30  14:54:51  bmc
 * No longer includes "<ctype.h>".  The macros/functions tolower() and
 * toupper() are not handled consistently across various UNIX versions,
 * so consistent versions are now provided in "u22.h" (included by this
 * header file).
 * 
 * Revision 1.14  89/08/29  17:46:34  bmc
 * Remove ctype-derivative definitions for Sun.  Add line to include <ctype.h>
 * unconditionally.
 * 
 * Revision 1.13  89/08/29  17:31:36  little
 * change god to eve
 * 
 * Revision 1.12  89/08/27  12:21:58  ejh
 * fixed error message for prologue syntax error (SPR 100)
 * 
 * Revision 1.11  89/07/24  14:54:13  little
 * ops removal
 * 
 * Revision 1.10  89/04/25  09:03:43  ljb
 * add error messages pertaining to user defined functions
 * 
 * Revision 1.9  89/02/09  15:57:52  ejh
 * added copyright notice
 * 
 * Revision 1.8  89/02/02  14:33:55  ejh
 * fixed redifinition of toupper, etc. on sun
 * 
 * Revision 1.7  88/12/20  15:44:14  ejh
 * added global current file name variable
 * 
 * Revision 1.6  88/12/19  13:58:01  little
 * retrofit some sun demanded fixes
 * 
 * Revision 1.5  88/10/04  17:27:13  little
 * no change
 * 
 * Revision 1.4  88/10/04  17:23:09  little
 * added log comment
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

#ifndef c22
#define c22

#include        "global.h"
#include        "string.h"
#include        <listPack.h>
#include        <stdio.h>
#include        "isd.h"
#include        "cus.h"
#include	"u22.h"

#ifdef c22_top
#define EXTERN
#else
#define EXTERN extern
#endif

#define c22_god_text "EVE"

EXTERN int c22_debug;
EXTERN int c22_slang_line_num;
EXTERN boolean c22_output_debug_code;
EXTERN string c22_pp_filename;

/*--------------------------------------------------------------------------*\
|  Routine: c22_compile
| 
|  Description:
|	This routine compiles a SLANG program.
|
\* Syntax:                                                                  */

	int	c22_compile	(
	/*	file_ptr:	in	FILE,
		debug_level:	in	int
		output_debug_code: in  boolean 
		print_trees : in  boolean 
		dialogue_name: in string
	*/
	);

/* 
|  Parameters:
|	file_ptr	an open file pointer to the SLANG program
|	debug_level	what debug level is requested by the user
|	output_debug_code 	indicates if debug code should go into the
|				generated output c
|	print_trees	indicates if the syntax tree should be output
|	dialogue_name	name of dialogue input file
|
|  Return value:
|	none
|  
|  Statuses:  
|	none
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Macros:    DEBUG1, DEBUG2, DEBUG3, & FATAL_ERROR
|
|  Description:
|       define the macros for various levels of debug for SLANG
|
\* Definitions:                                                              */

#define COMPILE_ERROR(statement)\
   {\
   fprintf (stderr,"COMPILE ERROR: at or near SLANG program line number %d\n",\
	c22_slang_line_num);\
   statement;\
   fprintf (stderr, "\n");\
   fflush (stderr);\
   c22_continue = FALSE;\
   }

#define FATAL_ERROR(statement)\
   {  long pid;\
      char command [256];\
   fprintf (stderr, "!!!!!     SLANG COMPILER FATAL ERROR\n");\
   fprintf (stderr, "\t\tat or near SLANG program line number %d\n",\
	c22_slang_line_num);\
   fprintf (stderr, "\tin compiler file %s, line %d\n", __FILE__, __LINE__);\
   statement;\
   fflush (stderr);\
   pid = getpid ();\
   sprintf (command, "gcore %d", pid);\
   system (command);\
   sprintf (command, "mv core.%d core", pid);\
   system (command);\
   exit (1);\
   }

#define DEBUG1(statement)\
   if (c22_debug > 0)\
      {\
        statement;\
	fprintf (stderr, "	file %s, line %d\n", __FILE__, __LINE__);\
        fflush (stderr);\
      }

#define DEBUG2(statement)\
   if (c22_debug > 1)\
      {\
        statement;\
	fprintf (stderr, "	file %s, line %d\n", __FILE__, __LINE__);\
        fflush (stderr);\
      }

#define DEBUG3(statement)\
   if (c22_debug > 2)\
      {\
        statement;\
	fprintf (stderr, "	file %s, line %d\n", __FILE__, __LINE__);\
        fflush (stderr);\
      }

/*                                                                          *\
|
|  Components:
|       DEBUG1  typically associated with error reporting
|       DEBUG2  typically associated with routine/function entry/exit tracing
|       DEBUG3  typically associated with variable tracing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constants: error types
| 
|  Description:
|	used to tell what kind of error occurred in SLANG
| 
\* Definition:                                                              */

/*	CAUTION: WHEN ADDING TO THIS LIST ADD STRINGS TO
	c22_errors_print_value */
typedef	enum c22_errors	{
   VC_EXISTS,
   VAR_EXISTS,
   OBJECT_EXISTS,
   ILLEGAL_CHAR,
   UNKNOWN_OBJECT_TYPE,
   UNKNOWN_ATTRIBUTE,
   SD_NOT_EXIST,
   NO_ILL_FILES,
   OPEN_FILE,
   ILL_SYNTAX,
   PARSING,
   YACC_ERROR,
   ILLEGAL_FUNCTION,
   FUNCTION_DEFINITION,
   FUNCTION_TYPE,

   LAST_ERROR	/* one more to get the right number */
   } c22_error_type;

static char *c22_errors_print_value [] = {
   "VC exists",
   "variable exists",
   "object exists",
   "illegal character",
   "unknown object type",
   "unknown attribute",
   "shared data does not exist",
   "no shared data description files",
   "open file",
   "shared data description syntax",
   "parsing",
   "YACC error",
   "error reading function definitions",
   "function already defined",
   "illegal type for function",

   "LAST_ERROR"
   };

/*--------------------------------------------------------------------------*\
|  Constant: NONAME
| 
|  Description:
|	null name string
| 
\* Definition:                                                              */

#define c22_NONAME	""

/*--------------------------------------------------------------------------*\
|  Variables: c22_lineno
| 
|  Description:
|	used to update yylineno to represent the line number of the whole
|	file, not just the part yacc does
| 
\* Definition:                                                              */

	EXTERN	int	c22_lineno;

/*--------------------------------------------------------------------------*\
|  Variables: various FILE variables
| 
|  Description:
|	used to define FILEs (file pointers) used in SLANG
| 
\* Definition:                                                              */

	EXTERN	FILE	*yyin;

/*--------------------------------------------------------------------------*\
|  Variables: various cus_sym variables
| 
|  Description:
|	used to define symbol table handles
| 
\* Definition:                                                              */

	EXTERN	cus_sym	car_and_symbol;
	EXTERN	cus_sym	car_attributes_symbol;
	EXTERN	cus_sym	car_creation_condition_symbol;
	EXTERN	cus_sym	car_else_symbol;
	EXTERN	cus_sym	car_false_symbol;
	EXTERN	cus_sym	car_if_symbol;
	EXTERN	cus_sym	car_method_symbol;
	EXTERN	cus_sym	car_null_symbol;
	EXTERN	cus_sym	car_self_symbol;
	EXTERN	cus_sym	car_objects_symbol;
	EXTERN	cus_sym	car_or_symbol;
	EXTERN	cus_sym	car_true_symbol;
	EXTERN	cus_sym	car_undefined_symbol;
	EXTERN	cus_sym	car_while_symbol;

	EXTERN	cus_sym	car_dot_symbol;
	EXTERN  cus_sym car_cc_dot_symbol;
	EXTERN	cus_sym	car_notequal_symbol;
	EXTERN	cus_sym	car_lessequal_symbol;
	EXTERN	cus_sym	car_lessthan_symbol;
	EXTERN	cus_sym	car_greaterequal_symbol;
	EXTERN	cus_sym	car_greaterthan_symbol;
	EXTERN	cus_sym	car_gets_symbol;
	EXTERN	cus_sym	car_equal_symbol;
	EXTERN	cus_sym	car_plus_symbol;
	EXTERN	cus_sym	car_binary_minus_symbol;
	EXTERN	cus_sym	car_unary_minus_symbol;
	EXTERN	cus_sym	car_not_symbol;
	EXTERN	cus_sym	car_slash_symbol;
	EXTERN	cus_sym	car_power_symbol;
	EXTERN	cus_sym	car_star_symbol;

	EXTERN	cus_sym	car_comment_symbol;
	EXTERN	cus_sym	car_function_call_symbol;

	EXTERN  cus_sym car_new_function;

	EXTERN  cus_sym car_god_vc;

/*--------------------------------------------------------------------------*\
|  Variables: various status variables
| 
|  Description:
|	used to determine the status of various pieces of SLANG
| 
\* Definition:                                                              */

	EXTERN boolean	c22_continue;

/*                                                                          *\
|
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: c22_error
| 
|  Description:
|	This routine is the error routine for SLANG.
|
\* Syntax:                                                                  */

	void	c22_error	(
	/*	the_error:	in	c22_error_type,
		the_string:	in	string,
		the_lineno:	in	int
	*/
	);

/* 
|  Parameters:
|	the_error	what error occurred
|	the_string	what entity the error occurred on
|
|  Returns:  
|	none
|  
|  Statuses:  
|	none
|  
\*--------------------------------------------------------------------------*/

#undef EXTERN
#endif
