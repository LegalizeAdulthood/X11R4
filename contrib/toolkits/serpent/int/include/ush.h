/*      $Header: /u/04c/mnt/integ/int/include/RCS/ush.h,v 1.2 89/02/13 12:13:41 ejh Exp $ */

/*	$Log:	ush.h,v $
 * Revision 1.2  89/02/13  12:13:41  ejh
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

#ifndef NULLshmhash

#include "usl.h"		/* shared memory list utility		    */

typedef	struct {
    int		size,		/* number of sub-lists */
		number;		/* current count of elements */
    PFI		hashfunc,	/* convert name to hash-index */
		matchfunc;	/* match name to hashed node */
    SHM_LIST	lists[1];	/* dummy array */
} SHM_HASH_struct, *SHM_HASH;

#define	NULLshmhash	((SHM_HASH) 0)

#define	SHM_HASH_NOTUNIQUE	0
#define	SHM_HASH_UNIQUE	1

/*
 *	Macros and externals
 */

#define	ush_make_shm_hashtable( size, hashfunc, matchfunc )	\
	_ush_make_shm_hashtable( (int) (size), (PFI) (hashfunc), (PFI) (matchfunc) )

#define	ush_change_shm_hash_functions( size, hashfunc, matchfunc )	\
	_ush_change_shm_hash_functions( (int) (size), (PFI) (hashfunc), (PFI) (matchfunc) )

#define ush_free_shm_hashtable( table, flag )				\
	_ush_free_shm_hashtable( (SHM_HASH) (table), (int) (flag) )

#define ush_add_to_shm_hashtable( table, node, ident )	\
	_ush_add_to_shm_hashtable( (SHM_HASH) (table), (SHM_NODE) (node), (SHM_NODE) (ident), SHM_HASH_NOTUNIQUE )

#define ush_add_unique_to_shm_hashtable( table, node, ident )	\
	_ush_add_to_shm_hashtable( (SHM_HASH) (table), (SHM_NODE) (node), (SHM_NODE) (ident), SHM_HASH_UNIQUE )

#define ush_get_from_shm_hashtable( table, ident )		\
	_ush_get_from_shm_hashtable( (SHM_HASH) (table),  (SHM_NODE) (ident) )

#define ush_delete_from_shm_hashtable( table, ident )		\
	_ush_delete_from_shm_hashtable( (SHM_HASH) (table),  (SHM_NODE) (ident) )

#define ush_for_all_shm_hashtable( table, node, func )		\
	_ush_for_all_shm_hashtable( (SHM_HASH) (table), (SHM_NODE) (node), (PFI) (func) )

#define ush_for_all_shm_2hashtable( table, node, node2, func )	\
	_ush_for_allshm__2hashtable( (SHM_HASH) (table), (SHM_NODE) (node), (SHM_NODE) (node2), (PFI) (func) )

#define ush_for_all_shm_3hashtable( table, node, node2, node3, func )	\
	_ush_for_all_shm_3hashtable( (SHM_HASH) (table), (SHM_NODE) (node), (SHM_NODE) (node2), (SHM_NODE) (node3), (PFI) (func) )

#define ush_shm_hashtable_empty( table )	(ush_size_of_shm_hashtable( table ) == 0)

#define ush_size_of_shm_hashtable( table )	(((SHM_HASH) (table)) -> number )

extern	SHM_HASH	_ush_make_shm_hashtable();

extern void	_ush_free_shm_hashtable(),
		_ush_change_shm_hash_functions();

extern int	_ush_for_all_shm_hashtable(),
		_ush_for_all_shm_2hashtable(),
		_ush_for_all_shm_3hashtable();

extern SHM_NODE	_ush_add_to_shm_hashtable(),
		_ush_delete_from_shm_hashtable(),
		_ush_get_from_shm_hashtable();

#endif
