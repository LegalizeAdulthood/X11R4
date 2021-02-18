/*      $Header: /u/04c/mnt/integ/utilities/include/RCS/cud.h,v 1.2 89/02/10 14:28:27 ejh Exp $ */

/*	$Log:	cud.h,v $
 * Revision 1.2  89/02/10  14:28:27  ejh
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

#ifndef cud
#define cud

	
void cud_print_symbol (
/*		FILE *stream */
/*		cus_sym sym */
/*		char *indent */
	);


void cud_print_tree (
/*		FILE *stream */
/*		LIST the_tree */
/*		char *indent */
	);


void cud_print_variable (
/*		FILE *stream */
/*		cus_sym variable */
	);


void cud_print_method (
/*		FILE *stream */
/*		cus_sym method */
	);


void cud_print_attribute (
/*		FILE *stream */
/*		cus_sym attribute */
	);


void cud_print_object (
/*		FILE *stream */
/*		cus_sym object */
	);


void cud_print_vc (
/*		FILE *stream */
/*		cus_sym vc */
	);

#endif

