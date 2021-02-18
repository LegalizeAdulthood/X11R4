/*      $Header: /u/04c/mnt/integ/int/include/RCS/usl.h,v 1.2 89/02/13 12:16:45 ejh Exp $ */

/*	$Log:	usl.h,v $
 * Revision 1.2  89/02/13  12:16:45  ejh
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

#ifndef	 usl			/* check for previous inclusion	*/

#ifndef	AND_NODES
#define AND_NODES		1	/* flags for free_shm_list, */
#define NOT_NODES		0	/* copy_shm_list, and flush_shm_list */
typedef int (*PFI)();			/* pointer to func returning int */
#endif

/************************************************************************
 *		Type Definitions and exported constants			*
 ************************************************************************/

#include "ism.h"			/* shared memory abstraction */

#define	NULLshmlist	((SHM_LIST) 0)	/* Typecast NULL pointer */

#define SHM_LIST_DEFAULT_VALUE	0	/* default element tag value */

typedef struct shmListNode {
  struct shmListNode *next, *prev; 	/* list head & tail respectively */
  SHM_NODE node;			/* the ptr (??) to the data */
  int value;			/* for sorting and insertion */
} SHM_LISTNODE, *SHM_LIST;

/************************************************************************
 *	Define SHM_LIST functions that can be done as macros		*
 *----------------------------------------------------------------------*
 * These macros will allow the compiler to in-line expand some of the	*
 * simpler listPack functions.  That will reduce code size and		*
 * execution time.  It will also prohibit run-time checking of list	*
 * parameters.								*
 ************************************************************************/

#define	SHM_LISThead( list )	(((SHM_LIST) (list)) -> next)
#define	SHM_LISTtail( list )	(((SHM_LIST) (list)) -> prev)
#define	SHM_LISTvalue( list )	(((SHM_LIST) (list)) -> value)
#define	SHM_LISTnode( list )	(((SHM_LIST) (list)) -> node)
#define	SHM_LISTcurrent( list )	(((SHM_LIST) ((list) -> node)) -> node)

#define	usl_shm_list_length( list )	(int) (SHM_LISTvalue( list ))

#define usl_shm_list_empty( list )	(usl_shm_list_length( list ) == 0)

#define usl_get_shm_list_head( list )		\
	    (usl_shm_list_empty( list ) ? NULL_SHM_NODE : SHM_LISTnode( SHM_LISThead( list ) ) )

#define	usl_get_from_head( list )		\
	    _usl_shm_list_remove_node( SHM_LISThead( list ), list )

#define usl_get_head_val( list )	\
    (usl_shm_list_empty( list ) ? SHM_LIST_DEFAULT_VALUE : SHM_LISTvalue(SHM_LISThead(list)))

#define usl_first_in_shm_list( list, node )	\
	    (usl_get_shm_list_head( list ) == ((SHM_NODE) node) )

#define usl_get_shm_list_tail( list )		\
	    (usl_shm_list_empty( list ) ? NULL_SHM_NODE : SHM_LISTnode( SHM_LISTtail( list ) ) )

#define	usl_get_from_tail( list )		\
	    _usl_shm_list_remove_node( SHM_LISTtail( list ), list )

#define usl_get_tail_val( list )	\
    (usl_shm_list_empty( list ) ? SHM_LIST_DEFAULT_VALUE : SHM_LISTvalue(SHM_LISTtail(list)))

#define usl_last_in_shm_list( list, node )	\
	    (usl_get_shm_list_tail( list ) == ((SHM_NODE) node) )

#define	usl_add_to_head_val( list, node, val )			\
(usl_shm_list_empty( list ) ? 						\
    _usl_shm_list_add_node( list, &SHM_LISTtail(list), &SHM_LISThead(list),	\
    		    (SHM_NODE) node, val )	:			\
    _usl_shm_list_add_node( list, &SHM_LISThead(list)->prev,		\
    		    &SHM_LISThead(list), (SHM_NODE) node, val ))

#define	usl_add_to_head( list, node )	\
	usl_add_to_head_val( list, node, SHM_LIST_DEFAULT_VALUE )

#define	usl_add_to_tail_val( list, node, val )			\
(usl_shm_list_empty( list ) ? 						\
    _usl_shm_list_add_node( list, &SHM_LISTtail(list), &SHM_LISThead(list),	\
    		    (SHM_NODE) node, val ) : 			\
    _usl_shm_list_add_node( list, &SHM_LISTtail(list),			\
    		    &SHM_LISTtail(list)->next, (SHM_NODE) node, val ))

#define	usl_add_to_tail( list, node )	\
	usl_add_to_tail_val( list, node, SHM_LIST_DEFAULT_VALUE )

/****************************************************************
 *	These macros construct a for() loop for C programs	*
 ****************************************************************/


#define usl_loop_through_shm_list_func( list, ptr, type, recovery_func )	\
	for ( ptr = (type) (list), SHM_LISTnode(list) = (SHM_NODE) (list);	\
	     (ptr = ((SHM_LISTnode(list) != NULL_SHM_NODE &&			\
		      ptr == (type) SHM_LISTcurrent(list) &&		\
		      SHM_LISThead((list)->node) != NULLshmlist ) ?		\
     (type) SHM_LISTnode(SHM_LISTnode(list) = (SHM_NODE) SHM_LISThead(SHM_LISTnode(list)))	\
     : (type) recovery_func( list, ptr ))) != (type) 0; )


#define usl_loop_through_shm_list( list, ptr, type )				\
	for ( ptr = (type) (list), SHM_LISTnode(list) = (SHM_NODE) (list);	\
	     (ptr = ((SHM_LISTnode(list) != NULL_SHM_NODE &&			\
		      ptr == (type) SHM_LISTcurrent(list) &&		\
		      SHM_LISThead((list)->node) != NULLshmlist ) ?		\
     (type) SHM_LISTnode(SHM_LISTnode(list) = (SHM_NODE) SHM_LISThead(SHM_LISTnode(list)))	\
		     : (type) 0 )) != (type) 0; )

/************************************************************************
 *		External Routine Declarations.				*
 *	Macros will redefine the calling sequences of all routines	*
 ************************************************************************/

extern	SHM_LIST	_usl_eval_shm_list(), _usl_reverse_shm_list(), _usl_make_shm_list(),
		_usl_sort_shm_list(), _usl_copy_shm_list(), _usl_append_shm_list(), _usl_flush_shm_list(),
		_usl_insert_shm_sublist_list_pos(), _usl_insert_shm_sublist_list_node();

extern	SHM_NODE	_usl_insert_shm_list_pos(), _usl_insert_shm_list_node(), _usl_insert_shm_list_val(),
		_usl_get_shm_list_pos(), _usl_get_shm_list_val(), _usl_delete_shm_list_pos(),
		_usl_delete_shm_list_val(), _usl_delete_shm_list_node(),
		_usl_get_shm_list_next(), _usl_get_shm_list_prev(), _usl_get_shm_list_next_func(),
		_usl_shm_list_add_node(), _usl_shm_list_remove_node(), _usl_search_shm_list();

extern	int	_usl_get_node_val(), _usl_change_shm_list_val(), _usl_get_shm_list_pos_val(),
		_usl_for_all_shm_list(), _usl_for_all_2list(), _usl_shm_list_equal(),
		_usl_for_all_shm_3list(), _usl_in_shm_list(), _usl_replace_shm_list(), free_shm_list();

/************************************************************************
 *		Macros for automatic type recasting.			*
 ************************************************************************/

#define	usl_make_shm_list()			_usl_make_shm_list()
#define	usl_reverse_shm_list( list )		_usl_reverse_shm_list( (SHM_LIST) (list) )
#define	usl_sort_shm_list( list )		_usl_sort_shm_list( (SHM_LIST) (list) )

#define	usl_free_shm_list( list, flag )		\
	    _usl_free_shm_list( (SHM_LIST) (list), (int) (flag) )
#define	usl_flush_shm_list( list, flag )	\
	    _usl_flush_shm_list( (SHM_LIST) (list), (int) (flag) )
#define	usl_get_node_val( list, node )	\
	    _usl_get_node_val((SHM_LIST) (list), (SHM_NODE)(node))
#define	usl_sort_shm_list_func( list, func )	\
	    _usl_sort_shm_list_func((SHM_LIST) (list), (PFI) (func))
#define	usl_copy_shm_list( list, flag )	\
	    _usl_copy_shm_list( (SHM_LIST) (list), (int) (flag) )
#define	usl_append_shm_list( list1, list2 )	\
	    _usl_append_shm_list((SHM_LIST) (list1), (SHM_LIST) (list2))
#define	usl_in_shm_list( list, node )	\
	    _usl_in_shm_list( (SHM_LIST) (list), (SHM_NODE) (node) )
#define	usl_get_shm_list_pos( list, pos )	\
	    _usl_get_shm_list_pos( (SHM_LIST) (list), (int) (pos) )
#define	usl_get_shm_list_val( list, val )	\
	    _usl_get_shm_list_val( (SHM_LIST) (list), (int) (val) )
#define	usl_delete_shm_list_pos( list, pos )	\
	    _usl_delete_shm_list_pos((SHM_LIST) (list), (int) (pos))
#define	usl_delete_shm_list_val( list, val )	\
	    _usl_delete_shm_list_val((SHM_LIST) (list), (int) (val))
#define usl_get_shm_list_prev( list, curr )	\
	    _usl_get_shm_list_prev((SHM_LIST) (list), (SHM_NODE) (curr))
#define usl_get_shm_list_next( list, curr )	\
	    _usl_get_shm_list_next((SHM_LIST) (list), (SHM_NODE) (curr))
#define usl_get_shm_list_next_func( list, curr, node, func ) \
	    _usl_get_shm_list_next_func((SHM_LIST)(list),(SHM_NODE)(curr),(SHM_NODE)(node),(PFI)(func))
#define	usl_delete_shm_list_node( list, node ) \
	    _usl_delete_shm_list_node((SHM_LIST) (list), (SHM_NODE) (node))
#define	usl_insert_shm_list_pos( list, node, pos) \
	    _usl_insert_shm_list_pos( (SHM_LIST) (list), (SHM_NODE) (node), (int) (pos) )
#define	usl_get_shm_list_pos_val( list, pos ) \
	    _usl_get_shm_list_pos_val( (SHM_LIST) (list), (int) (pos) )
#define	usl_change_shm_list_val( list, node, val ) \
            _usl_change_shm_list_val( (SHM_LIST) (list), (SHM_NODE) (node), (int) (val) )
#define	usl_insert_shm_list_val( list, node, val ) \
	    _usl_insert_shm_list_val( (SHM_LIST) (list), (SHM_NODE) (node), (int) (val) )
#define	usl_insert_shm_list_node( list, node, target ) \
	    _usl_insert_shm_list_node( (SHM_LIST) list, (SHM_NODE) (node), (SHM_NODE) (target) )
#define usl_insert_shm_sublist_list_node( list1, list2, node )	\
	    _usl_insert_shm_sublist_list_node((SHM_LIST) (list1), (SHM_LIST) (list2), (SHM_NODE) (node))
#define usl_insert_shm_sublist_list_pos( list1, list2, pos )	\
	    _usl_insert_shm_sublist_list_pos((SHM_LIST) (list1), (SHM_LIST) (list2), (int) (pos))
#define	usl_replace_shm_list( list, old, new ) \
	    _usl_replace_shm_list( (SHM_LIST) (list), (SHM_NODE) (old), (SHM_NODE) (new) )
#define	usl_shm_list_equal( list1, list2, func )	\
	    _usl_shm_list_equal( (SHM_LIST) (list1), (SHM_LIST) (list2), (PFI) (func) )
#define	usl_eval_shm_list( list, func )	\
	    _usl_eval_shm_list( (SHM_LIST) (list), (PFI) (func) )
#define	usl_search_shm_list( list, node, func )	\
	    _usl_search_shm_list( (SHM_LIST) (list), (SHM_NODE) (node), (PFI) (func) )
#define	usl_for_all_shm_list( list, node, func )	\
	    _usl_for_all_shm_list( (SHM_LIST) (list), (SHM_NODE) (node), (PFI) (func) )
#define	usl_for_all_2list( list, node, node2, func  )	\
	    _usl_for_all_2list((SHM_LIST)(list),(SHM_NODE)(node),(SHM_NODE)(node2),(PFI)(func))
#define	ush_for_all_shm_3list( list, node, node2, node3, func  )	\
	    _usl_for_all_shm_3list((SHM_LIST)(list),(SHM_NODE)(node),(SHM_NODE)(node2),(SHM_NODE)(node3),(PFI)(func))

#define usl
#endif
