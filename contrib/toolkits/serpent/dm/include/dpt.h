	/* $Header: /u/04c/mnt/integ/dm/include/RCS/dpt.h,v 1.4 89/10/02 13:29:56 rcs Exp $ */

/*	$Log:	dpt.h,v $
 * Revision 1.4  89/10/02  13:29:56  rcs
 * removed stupid include of serpent.h
 * 
 * Revision 1.3  89/05/19  17:15:02  bmc
 * Changed comments and function definitions to be consistent with
 * changes to 'dob.h'.
 * 
 * Revision 1.2  89/02/09  16:48:38  ejh
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

#ifndef dpt
#define dpt


/* -------------------------------------------     dpt_vc_exists */
dob_data_object dpt_vc_exists (
/*	template_name, partial_vce */
	);
/*
string template_name;
LIST partial_vce;
*/


/* -------------------------------------------     dpt_rite_rule */
boolean dpt_rite_rule (
/*	parent_vc, creating_shared_data */
	);
/*
dob_data_object parent_vc;
dob_data_object creating_shared_data;
*/


#endif

