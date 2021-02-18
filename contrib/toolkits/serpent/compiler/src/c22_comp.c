static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/c22_comp.c,v 1.2 89/10/23 19:57:58 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:	c22_comp.c
|
|  Description:
|	This is the entry point for SLANG compilation.
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
$Log:	c22_comp.c,v $
 * Revision 1.2  89/10/23  19:57:58  ejh
 * cosmetics
 * 
 * Revision 1.1  89/10/08  00:43:26  serpent
 * Initial revision
 * 
 * Revision 1.7  89/09/20  11:13:12  little
 * change the initial value of c22_pp_filename
 * 
 * 
 * Revision 1.6  89/08/27  12:23:34  ejh
 * added handling of missing '|||'; fixed error reporting for errors in
 * prologue (SPR 100)
 * 
 * Revision 1.5  89/04/25  09:19:42  ljb
 * call new routine which reads both function defs and ill files
 * 
 * Revision 1.4  89/02/15  17:44:16  little
 * no changes
 * 
 * Revision 1.3  89/02/15  17:29:30  little
 * new version number
 * 
 * Revision 1.2  88/10/04  17:29:58  little
 * add dialogue_name parameter to a couple of functions
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

#define	memoryPack

#include	"global.h"	/* globals */
#include	<stdio.h>	/* for I/O */
#include	<listPack.h>	/* list handling stuff */

/*	SLANG specific includes */

#include	"c22.h"		/* SLANG debug stuff */
#include	"ca2.h"
#include	"cus.h"		/* SLANG symbol table stuff */
#include	"cud.h"		/* SLANG symbol table print routine */


/* ---------------------------------------------------   c22_compile */
int c22_compile (file_ptr, debug_level, output_debug_code, print_trees,
	dialogue_name)
FILE *file_ptr;
int debug_level;
boolean output_debug_code;
boolean print_trees;
string dialogue_name;

    {
    char	temp [80];

/* set up some global variables */

    c22_debug = debug_level;
    c22_output_debug_code = output_debug_code;
    c22_continue = TRUE;
    c22_slang_line_num = 0;

/* create the symbol table and all the universal symbols */

    ca2_init_sym_tab ();
    ca2_make_god_vc ();
    ca2_read_prefix_info (file_ptr);

/* if c22_error generated a fatal error, make sure we fail */

    if (!c22_continue)
	return (FALSE);

/*	if we didn't read any ill "files", syntax error */

    if (get_list_head (cus_get_prop (car_god_vc, ill_files_prop)) == NULL)
	{
	c22_error (NO_ILL_FILES, c22_NONAME, NULL);
	return (c22_continue);
	}

/* if everything's ok, set the file pointer to the beginning of the next
   line (which is the first line of the actual SLANG code), then parse
   the rest through yacc/lex */

    (void) fgets (temp, 80, file_ptr);
    yyin = file_ptr;
    yyparse ();
    c22_pp_filename = NULL;

    if (c22_continue)
	{
	if (print_trees)
	    cud_print_vc (stdout, cus_look_up (c22_god_text));
	cs2_gen (dialogue_name);
	}

    fclose (file_ptr);
    return (c22_continue);
    }
