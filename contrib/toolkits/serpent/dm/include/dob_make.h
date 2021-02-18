	/* $Header: /u/04c/mnt/integ/dm/include/RCS/dob_make.h,v 1.4 89/02/09 16:45:50 ejh Exp $ */

/*
$Log:	dob_make.h,v $
 * Revision 1.4  89/02/09  16:45:50  ejh
 * added copyright notice
 * 
 * Revision 1.3  89/01/24  17:00:28  rcs
 * added dob_make_id_constant_adi header
 * 
 * Revision 1.2  88/12/13  09:47:43  little
 * add declaration for dob_make_undefined_adi
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

#ifndef dob_make
#define dob_make
#include "dro.h"

/* --------------------------------------------------- dob_make_constant_adi */
dro_atomic_data_item dob_make_string_constant_adi (
/*	value */
	);
/*
string value;
*/


/* ------------------------------------------ dob_make_boolean_constant_adi */
dro_atomic_data_item dob_make_boolean_constant_adi (
/*	value */
	);
/*
int value;
*/


/* --------------------------------------------------- dob_make_integer_adi */
dro_atomic_data_item dob_make_integer_constant_adi (
/*	value */
	);
/*
int value;
*/



/* --------------------------------------------------- dob_make_id_adi */
dro_atomic_data_item dob_make_id_constant_adi (
/*	value */
	);
/*
iid_id_type value;
*/


/* --------------------------------------------------- dob_make_real_adi */
dro_atomic_data_item dob_make_real_constant_adi (
/*	value */
	);
/*
double value;
*/


/* --------------------------------------------------- dob_make_temp_adi */
dro_atomic_data_item dob_make_temp_adi (
	);


/* --------------------------------------------------- dob_make_undefined_adi */
dro_atomic_data_item dob_make_undefined_adi (
	);


/* --------------------------------------------------- dob_make_adi */
dro_atomic_data_item dob_make_adi (
/*	the_id, the_name */
	);
/*
iid_id_type the_id;
string the_name;
*/


#endif

