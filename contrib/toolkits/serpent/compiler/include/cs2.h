/*
$Header: /u/04c/mnt/integ/compiler/include/RCS/cs2.h,v 1.9 89/07/24 14:54:42 little Exp $
*/


/*
$Log:	cs2.h,v $
 * Revision 1.9  89/07/24  14:54:42  little
 * ops removal
 * 
 * Revision 1.8  89/04/14  11:24:09  little
 * add definition for cs2_ops_find2_stream.
 * 
 * Revision 1.7  89/03/15  17:30:28  little
 * add the find and remove ops files.
 * 
 * Revision 1.6  89/02/15  17:46:19  little
 * take out cs2_c_mod_stream, add cs2_ops_top_stream & cs2_c_snip_stream
 * stream variables.  add copyright notice
 * 
 * Revision 1.5  88/12/14  11:01:49  little
 * add definition for cf_ops_create
 * 
 * Revision 1.4  88/10/04  17:27:55  little
 * no change
 * 
 * Revision 1.3  88/10/04  17:25:14  little
 * changed cs2_initialize_output_files, cs2_initialize, and cs2_gen to
 * take a parameter which is the dialogue name
 * 
 * Revision 1.2  88/10/03  08:52:29  little
 * changed some of the named constants wrt ops record component names
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

#ifndef cs2
#define cs2

#include "serpent.h"

#define DM_BOX_NAME "DM_BOX"

#ifdef cs2_top
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN boolean c22_got_dm_box;

EXTERN FILE *cs2_c_pet_stream;
EXTERN FILE *cs2_c_att_stream;
EXTERN FILE *cs2_c_var_stream;
EXTERN FILE *cs2_c_meth_stream;
EXTERN FILE *cs2_c_cact_stream;
EXTERN FILE *cs2_c_dact_stream;
EXTERN FILE *cs2_c_proc_header0_stream;
EXTERN FILE *cs2_c_proc_header1_stream;
EXTERN FILE *cs2_c_snip_stream;

EXTERN FILE *cs2_symtab_stream;
EXTERN FILE *cs2_dm_init_stream;
EXTERN FILE *cs2_dm_exec_stream;

EXTERN int cs2_current_c_proc_number;

#undef EXTERN



/* -------------------------------------------     cs2_fflush */
void cs2_fflush (
/*	stream */
	);
/*
FILE *stream;
*/


/* --------------------------------------------     cs2_cc_gen */
void cs2_cc_gen (
/*	vc */
	);
/*
cus_sym vc;
*/


/* --------------------------------------------     cs2_vars_gen */
int cs2_vars_gen (
/*	vc */
	);
/*
cus_sym vc;
*/


/* --------------------------------------------     cs2_objs_gen */
int cs2_objs_gen (
/*	vc */
	);
/*
cus_sym vc;
*/


/* --------------------------------------------     cs2_vc_gen */
int cs2_vc_gen (
/*	vc */
	);
/*
cus_sym vc;
*/


/* ----------------------------------------     cs2_open_output_file */
FILE *cs2_open_output_file (
/*	file_name */
	);
/*
string file_name;
*/


/* --------------------------------------------     cs2_open_c_file */
FILE *cs2_open_c_file (
/*	name */
	);
/*
string name;
*/


/* ----------------------------------------     cs2_initialize_output_files */
void cs2_initialize_output_files (
/*	dialogue_name */
	);
/*
string dialogue_name;
*/


/* -------------------------------------------     cs2_cleanup_output_files */
void cs2_cleanup_output_files ();


/* -------------------------------------------     cs2_cleanup */
void cs2_cleanup ();


/* -------------------------------------------     cs2_initialize */
void cs2_initialize (
/*	dialogue_name */
	);
/*
string dialogue_name;
*/



/* --------------------------------------------     cs2_gen */
void cs2_gen (
/*	dialogue_name */
	);
/*
string dialogue_name;
*/


#endif

