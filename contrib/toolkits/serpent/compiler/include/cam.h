/* $Header: /u/04c/mnt/integ/compiler/include/RCS/cam.h,v 1.3 89/02/09 16:05:22 ejh Exp $ */


/*
$Log:	cam.h,v $
 * Revision 1.3  89/02/09  16:05:22  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/10/31  15:16:46  little
 * add definition for cam_fix_atts and the stupid log info
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

#ifndef cam
#define cam
#include "listPack.h"
#include "serpent.h"

void cam_fix_cc (
/*	vc : in cus_sym */
	);


LIST cam_fix_cc_tree (
/*	*/
	);


cus_sym cam_fix_var (
/*	*/
	);


LIST cam_fix_dot_tree (
/*	*/
	);


int cam_find_var (
/*	*/
	);


void cam_fix_methods (
/*	obj : in cus_sym*/
	);


/* --------------------------------------------     cam_fix_cc_used_vars   */
void cam_fix_cc_used_vars (
/*	vc */
	);
/*
cus_sym vc;
*/


/* --------------------------------------------     cam_fix_atts */
void cam_fix_atts (
/*	obj */
	);
/*
cus_sym obj;
*/


#endif

