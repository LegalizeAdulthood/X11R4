	/* $Header: /u/04c/mnt/integ/compiler/include/RCS/ca2.h,v 1.5 89/09/20 12:06:15 little Exp $ */

	/* $Log:	ca2.h,v $
 * Revision 1.5  89/09/20  12:06:15  little
 * redefined yyerror to use a local compiler routine so we can print out
 * more info for a parse error (such as the name of the original slang
 * file with the error)
 * 
 * Revision 1.4  89/04/25  09:05:07  ljb
 * add entry point which reads in user defined functions
 * 
 * Revision 1.3  89/02/09  16:03:36  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/12/20  15:48:21  ejh
 * put yacc's error redefinition here so it's available to both yacc and lex
 *  */

/*--------------------------------------------------------------------------*\
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

#ifndef ca2
#define ca2

#include "global.h"

boolean ca2_read_ill_entry (

/*	*file_ptr: in FILE */
	);


void ca2_read_prefix_info (
/*	file_ptr */
	);
/*
FILE    *file_ptr;  pointer to a currently open ill "file"
*/

boolean ca2_read_function_defs (
/*	file_ptr */
	);
/*
FILE    *file_ptr;  pointer to a currently open file where the function defs
                   can be found
*/


/* ---------------------------------------------  ca2_make_god_vc */
void ca2_make_god_vc (
	);


/* -------------------------------------------  ca2_init_sym_tab */
void ca2_init_sym_tab (
	);


/* --------------------------------------------  ca2_get_sd_sym */
cus_sym ca2_get_sd_sym (
/*	name */
	);
/*
string name;
*/

/*	to make it globally available within the analysis portion */

#undef  yyerror
#define yyerror(c)      c22_yyerror(c, yylineno)

#endif

