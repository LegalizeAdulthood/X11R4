	/* $Header: /u/04c/mnt/integ/dm/include/RCS/dgr.h,v 1.3 89/05/26 10:55:16 little Exp $ */

/*	$Log:	dgr.h,v $
 * Revision 1.3  89/05/26  10:55:16  little
 * add definition for a dgr_sne.  a sne used to just be a two element list.
 * we went with this structure ini anticipation of removing ops and using the
 * sne mechanism for pets.
 * 
 * Revision 1.2  89/02/09  16:42:23  ejh
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

#ifndef dgr
#define dgr

#define NO_GENERIC_ROUTINE -1

typedef int dgr_snippet;

typedef struct {dgr_snippet routine;
		caddr_t parm_1;
		caddr_t parm_2;
	       } dgr_sne;

/* ----------------------------------------  dgr_print_sne_list */
void dgr_print_sne_list (
/*	stream, sne_list, indent */
	);
/*
FILE stream;
LIST sne_list;
string indent;
*/


/* ----------------------------------------  dgr_print_sne */
void dgr_print_sne (
/*	stream, sne, indent */
	);
/*
FILE stream;
LIST sne;
string indent;
*/


/* ----------------------------------------  dgr_process_sne_list */
void dgr_process_sne_list (
/*	sne_list */
	);
/*
LIST sne_list
*/


/* ----------------------------------------  dgr_line_sort_func */
int dgr_line_sort_func (
/*	item_0, item_1 */
	);
/*
char *item_0, *item_1;
*/


/* ----------------------------------------  dgr_run_snippet */
void dgr_run_snippet (
/*	routine, id */
	);
/*
int routine, id;
*/


/* ----------------------------------------  dgr_dummy_snippet */
void dgr_dummy_snippet (
/*	dummy */
	);
/*
iid_id_type dummy;
*/

#endif

