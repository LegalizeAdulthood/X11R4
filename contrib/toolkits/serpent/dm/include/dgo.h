/* $Header: /u/04c/mnt/integ/dm/include/RCS/dgo.h,v 1.6 89/02/15 18:07:42 little Exp $ */

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
$Log:	dgo.h,v $
 * Revision 1.6  89/02/15  18:07:42  little
 * add multiple include protection and copyright notice
 * 
 * Revision 1.5  89/01/04  13:30:52  little
 * add Ops_is_remove definition
 * 
 * Revision 1.4  88/11/11  16:20:41  little
 * add definition for Ops_is_new
 * 
 * Revision 1.3  88/10/04  17:41:38  little
 * modify some of the ops record component names
 * 
 * Revision 1.2  88/10/03  09:04:10  little
 * modified the named constants for ops record component names
 * 
*/


#ifndef dgo
#define dgo

/*
**	Type declarations to allow OPS83 types to be used in
**	C programs.  The declarations in this file are machine-dependent;
**	the declarations below are for OPS83 running on DEC VAX computers.
**
**	It is important to remember that OPS83 passes arguments by reference
**	and not value.  As a result, formal parameters in C routines are
**	generally declared type pointer to X rather than type X.
*/

typedef int	opsInteger;		/* OPS83 integers */
typedef int	opsSymbol;		/* OPS83 symbols */
typedef char	opsChar;		/* OPS83 characters */
typedef char	opsLogical;		/* OPS83 logicals */
typedef double	opsReal;		/* OPS83 reals */
typedef char	opsName[128];		/* OPS83 names */


/*				??this caouses problems, look into it
typedef int id_type;
*/

typedef opsSymbol opsControl_flag;
typedef char opsString;
typedef opsInteger dgo_wme_index;

#define opsFalse '\000'
#define opsTrue '\001'
#define opsSdd_id_component_name "sdd_id"
#define opsSdd_vc_id_component_name "vc_id"
#define opsSdd_creator_id_component_name "creator_sdd_id"
#define opsSdd_cf_name "cf"
#define opsSdd_cf_component_name "component_cf"
#define opsSdd_parent_id_name "parent_id"
#define opsSdd_cf_create "create"
#define opsSdd_cf_modify "modify"
#define opsSdd_cf_delete "remove"
#define opsNoWme -1

opsSymbol get_ops_symbol ();
void shell_ops ();
opsInteger get_wme_from_serpent_id ();


/* --------------------------------------------------- Ops_make_wme */
opsInteger Ops_make_wme (
/* &ELEMENT_NAME, &I, &PARENT */
	);
/*
&ELEMENT_NAME: name
&I:id
&PARENT:id
*/


opsSymbol get_ops_symbol ();
void run_Ops ();

opsLogical Ops_wstore_symbol ();
opsLogical Ops_wstore_symbol_array ();

opsLogical Ops_wstore_string ();
opsLogical Ops_wextract_string ();

opsLogical Ops_wstore_integer ();
opsLogical Ops_wextract_integer ();

opsLogical Ops_wstore_logical ();
opsLogical Ops_wstore_logical_array ();

opsLogical Ops_wstore_sdd_id ();
opsLogical Ops_is_new ();
opsLogical Ops_is_remove ();

void set_ops_debug ();

#endif
