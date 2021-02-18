        /* $Header: /u/04c/mnt/integ/compiler/include/RCS/cso.h,v 1.2 89/02/09 16:13:50 ejh Exp $ */

/*	$Log:	cso.h,v $
 * Revision 1.2  89/02/09  16:13:50  ejh
 * added copyright notice
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

#ifndef cso
#define cso


/* -------------------------------------------     cso_def_write */
void cso_def_write (
/*	stream, name, num_components */
	);
/*
FILE *stream;
string name;
int num_components;
*/


/* -------------------------------------------     cso_cc_write */
void cso_cc_write (
/*	vc */
	);
/*
cus_sym vc;
*/


/* -------------------------------------------     cso_vc_write */
void cso_vc_write (
/*	vc */
	);
/*
cus_sym vc;
*/


/* -------------------------------------------     cso_var_write */
void cso_var_write (
/*	var */
	);
/*
cus_sym var;
*/


/* -------------------------------------------     cso_sdd_element_write */
void cso_sdd_element_write (
/* 	stream, element */
	);
/*
FILE *stream;
char *element;
*/


/* -------------------------------------------     cso_find_rule_write */
void cso_find_rule_write (
/*	stream, name */
	);
/*
FILE *stream;
string name;
*/


/* -------------------------------------------     cso_remove_rule_write */
void cso_remove_rule_write (
/*	stream, name */
	);
/*
FILE *stream;
string name;
*/


/* -------------------------------------------     cso_debug_print_write */
void cso_debug_print_write (
/*	stream, name, num_components */
	);
/*
FILE *stream;
string name;
int num_components;
*/


/* ----------------------------------------     cso_ill_files_write */
void cso_ill_files_write (
	);

#endif

