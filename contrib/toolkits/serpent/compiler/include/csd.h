	/* $Header: /u/04c/mnt/integ/compiler/include/RCS/csd.h,v 1.3 89/07/24 14:55:18 little Exp $ */

/*	$Log:	csd.h,v $
 * Revision 1.3  89/07/24  14:55:18  little
 * ops removal
 * 
 * Revision 1.2  89/02/09  16:12:24  ejh
 * added copyright noticde
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

#ifndef csd
#define csd

/* -------------------------------------------     csd_var_write */
void csd_var_write (
/*	var */
	);
/*
cus_sym var;
*/


/* -------------------------------------------     csd_att_write */
void csd_att_write (
/*	att */
	);
/*
cus_sym att;
*/


/* -------------------------------------------     csd_meth_write */
void csd_meth_write (
/*	obj */
	);
/*
cus_sym obj;
*/


/* -------------------------------------------     csd_vc_write */
void csd_vc_write (
/*	vc */
	);
/*
cus_sym vc;
*/


/* -------------------------------------------     csd_obj_write */
void csd_obj_write (
/*	obj */
	);
/*
cus_sym obj;
*/


/* -------------------------------------------     csd_cc_write */
void csd_cc_write (
/*	vc */
	);
/*
cus_sym vc;
*/


/* -------------------------------------------     csd_action_write */
void csd_action_write (
/*	action, vc */
	);
/*
string action;
cus_sym vc;
*/


/* -------------------------------------------     csd_dm_init_write */
void csd_dm_rt_write (
	);


#endif

