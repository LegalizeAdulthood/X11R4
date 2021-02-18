	/* $Header: /u/04c/mnt/integ/compiler/include/RCS/csc.h,v 1.3 89/02/09 16:10:26 ejh Exp $ */


/*
$Log:	csc.h,v $
 * Revision 1.3  89/02/09  16:10:26  ejh
 * added copyright notice
 * 
 * Revision 1.2  89/01/09  16:47:14  little
 * add definition of csc_var_write
 * 
*/

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

#ifndef csc
#define csc




/* -------------------------------------------     csc_statements_write */
void csc_statements_write (
/*	stream, statements, current_statement, self */
	);
/*
FILE *stream;
LIST statements;
char *current_statement;
cus_sym self;
*/


void csc_if_write (
/*	stream, if_statement, self */
	);
/*
FILE *stream;
LIST if_statement;
cus_sym self;
*/



void csc_while_write (
/*	stream, while_statement, self */
	);
/*
FILE *stream;
LIST while_statement;
cus_sym self;
*/


void csc_fun_call_write (
/*	stream, fun_call_statement, self */
	);
/*
FILE *stream;
LIST fun_call_statement;
cus_sym self;
*/


void csc_gets_write (
/*	stream, gets_statement, self */
	);
/*
FILE *stream;
LIST gets_statement;
cus_sym self;
*/


void csc_comment_write (
/*	stream, comment_statement, self */
	);
/*
FILE *stream;
LIST comment_statement;
cus_sym self;
*/


void csc_statement_write (
/*	stream, statement, self */
	);
/*
FILE *stream;
LIST statement;
cus_sym self;
*/



void csc_snip_write (
/*	stream, snippet, self */
	);
/*
FILE *stream;
LIST snippet;
cus_sym self;
*/


void csc_meth_write (
/*	meth */
	);
/*
cus_sym meth;
*/


void csc_action_write (
/*	type */
/*	vc */
	);
/*
string type;
cus_sym vc;
*/


void csc_att_write (
/*	att */
	);
/*
cus_sym att;
*/


void csc_exp_write (
/*	stream, exp, self */
	);
/*
FILE *stream;
LIST exp;
cus_sym self;
*/


void csc_cc_write (
/*	vc */
	);
/*
cus_sym vc;
*/


void csc_gen_cal_write (
/*	proc_name */
	);
/*
string proc_name;
*/



/* -------------------------------------------     csc_generic_call_write */
void csc_generic_call_write (
/*	sym, proc_name */
	);
/*
cus_sym sym;
string proc_name;
*/


/* -------------------------------------------     csc_constant_write */
void csc_constant_write (
/*	stream, const, self */
	);
/*
FILE *stream;
cus_sym const;
cus_sym self;
*/


/* -------------------------------------------     csc_variable_write */
void csc_variable_write (
/*	stream, var, self */
	);

/*
FILE *stream;
cus_sym var;
cus_sym self;
*/

#endif

