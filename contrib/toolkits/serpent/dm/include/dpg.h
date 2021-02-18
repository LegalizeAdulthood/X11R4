/*
$Header: /u/04c/mnt/integ/dm/include/RCS/dpg.h,v 1.5 89/05/19 17:34:24 bmc Exp $
*/


/*
$Log:	dpg.h,v $
 * Revision 1.5  89/05/19  17:34:24  bmc
 * Changed comments to reflect new function definitions.
 * 
 * Revision 1.4  89/02/09  16:47:28  ejh
 * added copyright notice
 * 
 * Revision 1.3  88/10/24  17:17:00  little
 * dpg_propagate_modify has a new (third) parameter.  this is the component
 * name, if there is one otherwise it is "", of the component being modified.
 * 
 * Revision 1.2  88/10/04  17:42:27  little
 * dpg_propogate_create gets new parameter of parent
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

#ifndef dpg
#define dpg


/* --------------------------------------------  dpg_find_match */
int dpg_find_match (
/*	candidate, list_element */
	);
/*
char *candidate;
char *list_element;
*/


/* --------------------------------------------  dpg_propagate_reference */
void dpg_propagate_reference (
/*	object, component_position */
	);
/*
dob_data_object object;
int component_position - the data definition version (zero based );
*/


/* --------------------------------------------  dpg_propagate_modify */
void dpg_propagate_modify (
/*	object, component_position, component_name */
	);
/*
dob_data_object object;
int component_position - the data definition version (zero based );
string component_name - the name of the component being modified
*/


/* --------------------------------------------  dpg_propagate_create */
void dpg_propagate_create (
/*	id , parent */
	);
/*
dob_data_object object;
dob_data_object parent;
*/


/* --------------------------------------------  dpg_propagate_delete */
void dpg_propagate_delete (
/*	id */
	);
/*
dob_data_object object;
*/


#endif

