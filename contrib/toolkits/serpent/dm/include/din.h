/* $Header: /u/04c/mnt/integ/dm/include/RCS/din.h,v 1.5 89/07/24 14:57:47 little Exp $ */


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

/*
$Log:	din.h,v $
 * Revision 1.5  89/07/24  14:57:47  little
 * ops removal
 * 
 * Revision 1.4  89/02/15  18:08:32  little
 * add definition for din_get_dm_name and copyright notice
 * 
 * Revision 1.3  88/11/02  13:31:09  little
 * add the second parameter to the definition of din_minor_cycle
 * 
 * Revision 1.2  88/10/03  09:05:57  little
 * remode definition for din_init_interface routine
 * 
*/

#ifndef din
#define din

#include "cus.h"


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	ALL ESCAPE SEQUENCE STRINGS MUST BE OF THE SAME LENGTH AND
	OF COURSE THEY HAVE TO BE UNIQUE

   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
#define SYMBOL_ESCAPE "#S"
#define PROPERTY_ESCAPE "#P"
#define OPEN_LIST_ESCAPE "#("
#define CLOSE_LIST_ESCAPE "#)"
#define INTEGER_ESCAPE "#I"
#define BEGIN_SYMBOL_ENTRY_ESCAPE "#B"
#define END_SYMBOL_ENTRY_ESCAPE "#E"
#define STRING_ESCAPE "#G"


/* the following is used to qualify sd element templates when they are put into
   the "static" sym tab at dm initialize time
*/
#define SD_QUALIFIER_STRING "THIS_BE_SD_"


/*  -------------------------------------------------  din_get_st_data */
void din_get_st_data (
/*	stream */
	);
/*
FILE *stream;
*/


/*  -------------------------------------------------  din_get_st_entry */
boolean din_get_st_entry (
/*	stream */
	);
/*
FILE *stream;
*/


/*  -------------------------------------------------  din_get_dm_name */
char *din_get_dm_name (
	);


/*  -------------------------------------------------  din_get_element */
char *din_get_element (
/*	stream */
	);
/*
FILE *stream;
*/


/*  -------------------------------------------------  din_get_symbol */
cus_sym din_get_symbol (
/*	stream */
	);
/*
FILE *stream;
*/


/*  -------------------------------------------------  din_read_symbol */
cus_sym din_read_symbol (
/*	stream */
	);
/*
FILE *stream;
*/


/*  -------------------------------------------------  din_get_prop */
boolean din_get_prop (
/*	 stream, symbol */
	);
/*
FILE *stream;
cus_sym symbol;
*/


/*  -------------------------------------------------  din_read_integer */
char *din_read_integer (
/*	stream */
	);
/*
FILE *stream;
*/


/*  -------------------------------------------------  din_read_list */
char *din_read_list (
/*	stream */
	);
/*
FILE *stream;
*/


/* -------------------------------------------     cds_string_write */
void cds_string_write (
/*	stream, the_string */
	);
/*
FILE *stream;
string the_string;
*/


/*  -------------------------------------------------  din_read_string */
char *din_read_string (
/*	stream */
	);
/*
FILE *stream;
*/


/* ------------------------------------------------    din_process_tap_info */
void din_process_tap_info (
/*	file_name */
	);
/*
string file_name;
*/


/* -------------------------------------------    din_terminate_interface */
void din_terminate_interface (
	);


/* ----------------------------------------------------   din_main  */
void din_main (
	);


/* ----------------------------------------------   din_start_transactions */
void din_start_transactions (
	);


/* ----------------------------------------------   din_commit_transactions */
void din_commit_transactions (
	);


/* -------------------------------------------   din_rollback_transactions */
void din_rollback_transactions (
	);


/* -------------------------------------------   din_process_xaction */
boolean din_process_xaction (
/*	wait */
	);
/*
boolean wait;
*/


/* -------------------------------------------   din_abnormal_termination */
void din_abnormal_termination (
	);


/* -------------------------------------------   din_init */
void din_init (
	);


/* -------------------------------------------   din_cleanup */
void din_cleanup (
	);


/* ----------------------------------------------------   din_minor_cycle */
void din_minor_cycle (
	/* num_maj, call_ops */
	);

/*
int num_maj;
boolean call_ops;
*/


#endif

