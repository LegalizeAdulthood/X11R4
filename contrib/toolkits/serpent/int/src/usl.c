static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/usl.c,v 1.4 89/02/15 15:24:44 ejh Exp $";

/*	$Log:	usl.c,v $
 * Revision 1.4  89/02/15  15:24:44  ejh
 * line 345 was missing a '/' in front of the '*'
 * 
 * Revision 1.3  89/02/13  12:38:35  ejh
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

 /*
 * External Definitions and debugging macros:
 */


#include "usl.h"			/* shared memory list utility       */
					/* and SHM_LIST structures	    */

#ifdef DEBUG				/* When running with DEBUG	    */

#ifndef DO_CHECK			/* make sure that run-time SHM_LIST */
#define DO_CHECK 3			/* integrity checking is turned     */
#endif					/* on				    */

#define TIMES 5				/* size of test lists		    */
#endif

#ifndef DO_CHECK
#define DO_CHECK 0
#endif

#if DO_CHECK > 0			/* For integrity checking, we	*/
					/* need the standard I/O defs,	*/
typedef char * STRING;			/* our definition of a STRING	*/
#include <stdio.h>			/* and an internal SHM_LIST to keep */
					/* track of the SHM_LISTs which are */
static	SHM_LIST lists_in_use = NULLshmlist; /* valid at any given point in    */
					/* time, otherwise make the	*/
#else					/* debug checker an un-routine!	*/
#define check_list_integrity( list, routine )
#endif


/*	Macros to make the code cleaner:		*/

#define	SHM_LISTlength( list )	(list -> value)

#define CURRENT( list )		(SHM_LISTnode( list ))
#define current_pos( list )	((SHM_LIST) CURRENT( list ))
#define current_node( list )	(SHM_LISTnode(current_pos( list )))


					/* This macro assures uniform	*/
					/* traversal through SHM_LISTs	*/
#define LOOP_THROUGH_SHM_LIST( list, reg ) \
    for( reg = SHM_LISThead( list ); reg != NULLshmlist; reg = reg -> next )



#define	add_node_at_tail( list, node, val ) \
    _usl_shm_list_add_node( list, &SHM_LISTtail(list), &SHM_LISTtail(list)->next, node, val )


#define	add_node_before_ptr( list, temp, node, val ) \
    _usl_shm_list_add_node( list, &temp->prev, &temp->prev->next, node, val )


#define	add_node_at_head( list, node, val ) \
    _usl_shm_list_add_node( list, &SHM_LISThead(list)->prev, &SHM_LISThead(list), node, val )


#define	add_first_node( list, node, val ) \
    _usl_shm_list_add_node( list, &SHM_LISTtail(list), &SHM_LISThead(list), node, val )

/*
 * If checking, we must clean up a temp list before freeing it,
 * otherwise, just free off the header node.
 */
#if DO_CHECK > 0
#define destroy_list( list )	SHM_LISThead( list ) = NULLshmlist;	\
				SHM_LISTtail( list ) = NULLshmlist;	\
				SHM_LISTlength( list ) = 0;		\
				usl_free_shm_list( list, NOT_NODES );
#else
#define destroy_list( list )	free_shm_node( list );
#endif

/*	This routine performs serious run-time checking on a SHM_LIST.	*/

#if DO_CHECK > 0
	check_list_integrity( list, routine )
SHM_LIST	list;
STRING	routine;
    {
    register SHM_LIST	temp, save = NULLshmlist;
    register int	length = 0;

    if (list == NULLshmlist)
	fprintf( stderr, "'%s' called with NULL list\n", routine );
    else if (lists_in_use == NULLshmlist)
	fprintf( stderr, "'%s' called before any lists made\n", routine );

#if DO_CHECK > 1
    else {
	LOOP_THROUGH_SHM_LIST( lists_in_use, temp )
	    if (list == (SHM_LIST) SHM_LISTnode( temp ) ) break;

	if (list != lists_in_use && temp == NULLshmlist)
	    fprintf( stderr, "'%s' called with unknown list\n", routine );

#if DO_CHECK > 2
	LOOP_THROUGH_SHM_LIST( list, temp ) {
	    if (temp->prev != save)
		fprintf( stderr, "Error in prev pointer: %d != %d in '%s'\n",
				    temp->prev, save, routine );
	    if (temp->prev == NULLshmlist && SHM_LISThead(list) != temp)
		fprintf( stderr, "Head of list doesn't match node %d in '%s'\n",
				    temp, routine );
	    if (temp->next == NULLshmlist && SHM_LISTtail(list) != temp)
		fprintf( stderr, "Tail of list doesn't match node %d in '%s'\n",
				    temp, routine );
	    if (temp->next != NULLshmlist) if (temp->next->prev != temp )
		fprintf ( stderr, "Error in next pointer: %d != %d in '%s'\n",
				    temp->next, temp->next->prev, routine );
	    save = temp;
	    length++;
	    }
	if (length != SHM_LISTlength(list))
	    fprintf( stderr, "Length of list is incorrect in '%s'\n", routine );
#endif
	}
#endif
    }
#endif
/*
 * Internal utility routines (insert node, remove node)
 * SHM_NODE _usl_shm_list_add_node() Inserts a new node into a SHM_LIST.  It then 
 * returns a pointer to the new SHM_LIST record.
 * _usl_shm_list_remove_node() Removes a node from a SHM_LIST.  It then returns the
 * SHM_NODE at the node which was deleted.
 */

SHM_NODE _usl_shm_list_add_node( list, prev, next, node, val )
SHM_LIST list, *prev, *next;
SHM_NODE node;
int val;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "_usl_shm_list_add_node" );

    temp = (SHM_LIST) make_shm_node( sizeof (SHM_LISTNODE) );
    temp->prev  = *prev;
    temp->next  = *next;
    temp->node  = node;
    temp->value = val;
    *prev = temp;
    *next = temp;
    SHM_LISTlength( list )++;
    return node;
    }


SHM_NODE	_usl_shm_list_remove_node( ptr, list )
SHM_LIST	ptr, list;
    {
    if (ptr == NULLshmlist) return NULL_SHM_NODE;

    SHM_LISTlength( list )--;
    if (ptr->prev == NULLshmlist)	SHM_LISThead( list )	= ptr->next;
    else			ptr->prev->next = ptr->next;
    if (ptr->next == NULLshmlist)	SHM_LISTtail( list )	= ptr->prev;
    else			ptr->next->prev = ptr->prev;
    free_shm_node( (SHM_NODE) ptr );
    return SHM_LISTnode( ptr );
    }

/*
 * Internal utility routines (swap pointers between SHM_LISTs)
 */

#define	swap_ptrs( p1, p2, type )	\
    	    tptr = (SHM_LIST) p1;		\
	    p1   = p2;			\
	    p2   = (type) tptr;		\


static	swap_lists( temp, list )
SHM_LIST	temp, list;
    {
    register SHM_LIST tptr;

    swap_ptrs( SHM_LISThead( list ), SHM_LISThead( temp ), SHM_LIST );
    swap_ptrs( SHM_LISTtail( list ), SHM_LISTtail( temp ), SHM_LIST );
    swap_ptrs( SHM_LISTlength( list ), SHM_LISTlength( temp ), int );
    }

/*
 * List insertion routines:
 * usl_insert_shm_list_node will add a node to a list before a target node.
 * If the target node is not present, the node will be added to the tail
 */

SHM_NODE	_usl_insert_shm_list_node( list, node, target )
SHM_LIST	list;
SHM_NODE	node, target;
    {
    register SHM_LIST ptr;

    check_list_integrity( list, "usl_insert_shm_list_node" );

    if (SHM_LISTlength(list) == 0)
	return add_first_node( list, node, SHM_LIST_DEFAULT_VALUE );

    if (usl_first_in_shm_list( list, target ))
	return add_node_at_head( list, node, SHM_LIST_DEFAULT_VALUE );

    LOOP_THROUGH_SHM_LIST( list, ptr )
	if (SHM_LISTnode( ptr ) == target)
	    return add_node_before_ptr( list, ptr, node, SHM_LIST_DEFAULT_VALUE );

    return add_node_at_tail( list, node, SHM_LIST_DEFAULT_VALUE );
    }

/*
 * List insertion routines:
 * usl_insert_shm_list_pos adds a node to a list 'pos' nodes from the head.
 * If pos <= 1 it will go onto the head; pos >= length will go onto the tail
 */

SHM_NODE	_usl_insert_shm_list_pos( list, node, pos )
SHM_LIST	list;
SHM_NODE	node;
int	pos;
    {
    register SHM_LIST ptr;

    check_list_integrity( list, "usl_insert_shm_list_pos" );

    if (SHM_LISTlength(list) == 0)
	return add_first_node( list, node, SHM_LIST_DEFAULT_VALUE );

    if (pos <= 1)
	return add_node_at_head( list, node, SHM_LIST_DEFAULT_VALUE );

    if (SHM_LISTlength(list) >= pos)
	LOOP_THROUGH_SHM_LIST( list, ptr ) if (pos-- <= 1)
	    return add_node_before_ptr( list, ptr, node, SHM_LIST_DEFAULT_VALUE );

    return add_node_at_tail( list, node, SHM_LIST_DEFAULT_VALUE );
    }

/*
 * List insertion routines:
 * usl_insert_shm_list_val will add a node in ascending order with respect to
 *		the values already assigned to the nodes in the list. 
 */


SHM_NODE	_usl_insert_shm_list_val( list, node, val )
SHM_LIST	list;
SHM_NODE	node;
int	val;
    {
    register SHM_LIST ptr;

    check_list_integrity( list, "usl_insert_shm_list_val" );

    if (SHM_LISTlength(list) == 0)
	return add_first_node(list, node, val);

    if (SHM_LISTvalue(SHM_LISThead(list)) >= val)
	return add_node_at_head( list, node, val );

    if (SHM_LISTvalue(SHM_LISTtail(list)) <= val)
	return add_node_at_tail( list, node, val );

    LOOP_THROUGH_SHM_LIST( list, ptr )
	if (SHM_LISTvalue( ptr ) > val) break;

    return add_node_before_ptr( list, ptr, node, val );
    }

/*
 * Random access node retrieval.
 * usl_get_shm_list_pos reveals the pos-th entry in the list.
 * usl_get_shm_list_val reveals the first node with the given value
 */


SHM_NODE	_usl_get_shm_list_pos( list, pos )
SHM_LIST	list;
int	pos;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_shm_list_pos" );

    if (pos > 0 && pos <= SHM_LISTlength( list ))
	LOOP_THROUGH_SHM_LIST( list, temp )	if (--pos == 0) return SHM_LISTnode(temp);

    return NULL_SHM_NODE;
    }


SHM_NODE	_usl_get_shm_list_val( list, val )
SHM_LIST	list;
int	val;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_shm_list_val" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	if (SHM_LISTvalue(temp) == val) return SHM_LISTnode(temp);

    return NULL_SHM_NODE;
    }

 /* usl_delete_shm_list_val removes the entry whose value is 'val' from the list.
 * usl_delete_shm_list_pos removes the pos-th entry from the list
 * usl_delete_shm_list_node removes the names node from the list
 *
 * All three return the node to the caller.
 */

SHM_NODE	_usl_delete_shm_list_val( list, val )
SHM_LIST	list;
int	val;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_delete_shm_list_val" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	if (SHM_LISTvalue( temp ) == val) return _usl_shm_list_remove_node( temp, list );
    return NULL_SHM_NODE;
    }


SHM_NODE	_usl_delete_shm_list_pos( list, pos )
SHM_LIST	list;
int	pos;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_delete_shm_list_pos" );

    if (pos > 0 && pos <= SHM_LISTlength(list))
	LOOP_THROUGH_SHM_LIST( list, temp )
	    if (--pos == 0) return _usl_shm_list_remove_node( temp, list );
    return NULL_SHM_NODE;
    }


SHM_NODE	_usl_delete_shm_list_node( list, node )
SHM_LIST	list;
SHM_NODE	node;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_delete_shm_list_node" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	if (SHM_LISTnode( temp ) == node) return _usl_shm_list_remove_node( temp, list );
    return NULL_SHM_NODE;
    }

/*
 * list creation:
 * usl_make_shm_list (make_queue, make_stack) returns a virgin list to the caller.
 */

#define create_list( temp )				\
	temp = (SHM_LIST) make_shm_node( sizeof( SHM_LISTNODE ) );	\
	SHM_LISThead( temp ) = NULLshmlist;			\
	SHM_LISTtail( temp ) = NULLshmlist;			\
	CURRENT( temp )  = NULL_SHM_NODE;			\
	SHM_LISTlength( temp ) = 0;


SHM_LIST	_usl_make_shm_list()
    {
    register SHM_LIST temp;
		
    create_list( temp );		/* allocate a new list structure   */

#if DO_CHECK > 0			/* must maintain list of lists	   */
    if (lists_in_use == NULLshmlist) {	/* need to allocate list of lists  */
	create_list( lists_in_use );
	}
					/* save and return the list	   */
    return (SHM_LIST) usl_add_to_head( lists_in_use, temp );
#else

    return temp;

#endif
    }

/*
 * usl_flush_shm_list: clear a list for re-use
 * SHM_LISTs may not be accessed after calls to usl_free_shm_list.  If usl_free_shm_list is called
 * with AND_NODES, the nodes are also returned to the garbage collector, and
 * they, too, may not be accessed further.
 */
	
SHM_LIST	_usl_flush_shm_list( list, free_shm_nodes )
SHM_LIST	list;
int	free_shm_nodes;
    {
    register SHM_LIST temp;		/* NOTE:  free_shm_node does NOT destroy the data */
				/* in the node, so it is safe to free a node, */
    				/* and then access it immediately thereafter. */

    if (list == NULLshmlist) return usl_make_shm_list();

    check_list_integrity( list, "usl_flush_shm_list" );

    LOOP_THROUGH_SHM_LIST( list, temp ) {
	if (free_shm_nodes == AND_NODES) free_shm_node( SHM_LISTnode( temp ) );
	free_shm_node( (SHM_NODE) temp );
	}

    SHM_LISThead( list ) = SHM_LISTtail( list ) = NULLshmlist;
    SHM_LISTlength( list ) = 0;

    return list;
    }


	
void	_usl_free_shm_list( list, free_shm_nodes )
SHM_LIST	list;
int	free_shm_nodes;
    {
    free_shm_node( (SHM_NODE) usl_flush_shm_list( list, free_shm_nodes ) );

#if DO_CHECK > 0		/* Remove list from the list of lists	*/
    usl_delete_shm_list_node( lists_in_use, list );
#endif
    }

/*
 * Append SHM_LISTs together
 */

SHM_LIST	_usl_append_shm_list( list1, list2 )
SHM_LIST	list1, list2;
    {
    check_list_integrity( list1, "usl_append_shm_list (first argument)" );
    check_list_integrity( list2, "usl_append_shm_list (second argument)" );

    if (list1 == list2)	return list1;		/* ignore this call	*/

    if ( SHM_LISTlength(list2) != 0 )		/* empty? free it later */

	if ( SHM_LISTlength(list1) == 0 ) {		/* If list1 was empty	*/
	    SHM_LISThead(list1) = SHM_LISThead(list2);	/* move head pointer	*/
	    SHM_LISTtail(list1) = SHM_LISTtail(list2);	/* move tail pointer	*/
	    }

	else {					/* no? relink the tail	*/
	    SHM_LISTtail(list1)->next	= SHM_LISThead(list2);
	    SHM_LISTtail(list1)->next->prev = SHM_LISTtail(list1);
	    SHM_LISTtail(list1)		= SHM_LISTtail(list2);
	    }

    SHM_LISTlength(list1) += SHM_LISTlength(list2);	/* Set new list length	*/

    destroy_list( list2 );			/* invalid, so trash it	*/

    return list1;				/* return valid SHM_LIST	*/
    }

/*
 * Insert a sublist into a SHM_LIST at a position
 */

SHM_LIST	_usl_insert_shm_sublist_pos( list1, list2, pos )
SHM_LIST	list1, list2;
int	pos;
    {
    register SHM_LIST temp;

    check_list_integrity( list1, "usl_insert_shm_sublist_pos (first argument)" );
    check_list_integrity( list2, "usl_insert_shm_sublist_pos (second argument)" );

    if (list1 == list2)	return list1;		/* ignore this call	*/

    /* If there is nothing in list2, all we do is free it at the end	*/
    if ( SHM_LISTlength(list2) != 0 )

	if ( SHM_LISTlength(list1) == 0 ) {		/* If list1 was empty	*/
	    SHM_LISThead(list1) = SHM_LISThead(list2);	/* move head pointer	*/
	    SHM_LISTtail(list1) = SHM_LISTtail(list2);	/* move tail pointer	*/
	    }

	else if (pos <= 1) {			/* prepend list2	*/
	    SHM_LISThead(list1)->prev = SHM_LISTtail(list2);
	    SHM_LISThead(list1)	  = SHM_LISThead(list2);
	    }

	else if (pos > SHM_LISTlength(list1)) {	/* append list2		*/
	    SHM_LISTtail(list1)->next = SHM_LISThead(list2);
	    SHM_LISTtail(list1)	  = SHM_LISTtail(list2);
	    }

	else 					/* ok. find the position */
	    LOOP_THROUGH_SHM_LIST( list1, temp )
		if (--pos == 1) {		/* insert here		*/
		    temp->next->prev	= SHM_LISTtail( list2 );
		    temp->next		= SHM_LISThead( list2 );
		    break;
		    }

    SHM_LISTlength(list1) += SHM_LISTlength(list2);	/* Set new list length	*/

    destroy_list( list2 );			/* invalid, so trash it	*/

    return list1;				/* return valid SHM_LIST	*/
    }

/*
 * Insert a sublist into a SHM_LIST after a target node
 */

SHM_LIST	_usl_insert_shm_sublist_node( list1, list2, node )
SHM_LIST	list1, list2;
SHM_NODE	node;
    {
    register SHM_LIST temp;

    check_list_integrity( list1, "usl_insert_shm_sublist_node (first argument)" );
    check_list_integrity( list2, "usl_insert_shm_sublist_node (second argument)" );

    if (list1 == list2)	return list1;		/* ignore this call	*/

    /* If there is nothing in list2, all we do is free it at the end	*/
    if ( SHM_LISTlength(list2) != 0 )

	if ( SHM_LISTlength(list1) == 0 ) {		/* If list1 was empty	*/
	    SHM_LISThead(list1) = SHM_LISThead(list2);	/* move head pointer	*/
	    SHM_LISTtail(list1) = SHM_LISTtail(list2);	/* move tail pointer	*/
	    }


	else if (usl_first_in_shm_list(list1,node)) {	/* prepend list2	*/
	    SHM_LISThead(list1)->prev = SHM_LISTtail(list2);
	    SHM_LISThead(list1)	  = SHM_LISThead(list2);
	    }

	else {					/* ok. find the target	*/
	    LOOP_THROUGH_SHM_LIST( list1, temp )
		if (SHM_LISTnode(temp) == node) {	/* insert here		*/
		    temp->next->prev = SHM_LISTtail( list2 );
		    temp->next	     = SHM_LISThead( list2 );
		    break;
		    }

	    if (temp == NULLshmlist) {		/* not in list:  append	*/
		SHM_LISTtail(list1)->next	= SHM_LISThead(list2);	/* link head */
		SHM_LISTtail(list1)		= SHM_LISTtail(list2);	/* move tail */
		}
	    }

    SHM_LISTlength(list1) += SHM_LISTlength(list2);	/* Set new list length	*/

    destroy_list( list2 );			/* invalid, so trash it	*/

    return list1;				/* return valid SHM_LIST	*/
    }

/*
 * List processing.  These routines take (as a parameter) a pointer to a
 *	'C' function which will then be called with each element in the SHM_LIST.
 *	Also, they take one (two or three) pointers which will also be passed
 *	to the 'C' function.
 *
 * usl_for_all_shm_list applies a function to the nodes of a list, and returns the
 *	sum of the values returned by that function.
 * usl_for_all_shm_2list applies a function to the nodes of a list, and returns the
 *	sum of the values returned by that function.
 */


int	_usl_for_all_shm_list( list, node, func )
SHM_LIST	list;
SHM_NODE	node;
PFI	func;			/* pointer to function returning integer */
    {
    register int total;
    register SHM_LIST temp;

    check_list_integrity( list, "usl_for_all_shm_list" );

    total = 0;
    LOOP_THROUGH_SHM_LIST( list, temp ) total += (*func)( node, SHM_LISTnode( temp ) );
    return total;
    }


int	_usl_for_all_shm_2list( list, node, node2, func )
SHM_LIST	list;
SHM_NODE	node, node2;
PFI	func;			/* pointer to function returning integer */
    {
    register int total;
    register SHM_LIST temp;

    check_list_integrity( list, "usl_for_all_shm_2list" );

    total = 0;
    LOOP_THROUGH_SHM_LIST( list, temp )
	total += (*func)( node, node2, SHM_LISTnode( temp ) );
    return total;
    }

/*
 * List processing.  These routines take (as a parameter) a pointer to a
 * 'C' function which will then be called with each element in the SHM_LIST.
 *
 * usl_for_all_shm_3list applies a function to the nodes of a list, and returns the
 *	sum of the values returned by that function.
 */


int	_usl_for_all_shm_3list( list, node, node2, node3, func )
SHM_LIST	list;
SHM_NODE	node, node2, node3;
PFI	func;			/* pointer to function returning integer */
    {
    register int total;
    register SHM_LIST temp;

    check_list_integrity( list, "usl_for_all_shm_3list" );

    total = 0;
    LOOP_THROUGH_SHM_LIST( list, temp )
	total += (*func)( node, node2, node3, SHM_LISTnode( temp ) );
    return total;
    }

/* usl_search_shm_list applies a function to each node until the function returns
 *	TRUE.  It then returns a pointer to that node.
 * usl_shm_list_equal tests to see if two lists are equivalent
 */


SHM_NODE	_usl_search_shm_list( list, node, func )
SHM_LIST	list;
SHM_NODE	node;
PFI	func;			/* pointer to function returning integer */
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_search_shm_list" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	if ((*func)( node, SHM_LISTnode( temp ) )) return SHM_LISTnode( temp );

    return NULL_SHM_NODE;
    }


int	_usl_shm_list_equal( list1, list2, func )
SHM_LIST	list1, list2;
PFI	func;
    {
    register SHM_LIST ptr1, ptr2;

    check_list_integrity( list1, "usl_shm_list_equal (first arg)" );
    check_list_integrity( list2, "usl_shm_list_equal (second arg)" );

    if (SHM_LISTlength( list1 ) != SHM_LISTlength( list2 )) return 0;

    ptr2 = SHM_LISThead( list2 );
    LOOP_THROUGH_SHM_LIST( list1, ptr1 ) {
	if (SHM_LISTnode(ptr1) != SHM_LISTnode(ptr1) &&
	    (func == ((PFI) 0) || (*func)(SHM_LISTnode(ptr1), SHM_LISTnode(ptr1)) == 0))
		return 0;
	ptr2 = ptr2->next;
	}
    
    return 1;
    }

/*
 * Set all of the value fields within a SHM_LIST.  This is used for sorting SHM_LISTs
 * and for saving time on long evaluation functions.
 * usl_eval_shm_list sets all of the values of the nodes of a list to a function
 *	of the node.
 * usl_change_shm_list_val sets the value of a single node
 * usl_get_shm_list_pos_val returns the value of the pos-th node
 */


SHM_LIST	_usl_eval_shm_list( list, func )
SHM_LIST	list;
PFI	func;			/* pointer to function returning integer */
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_eval_shm_list" );

    LOOP_THROUGH_SHM_LIST( list, temp ) SHM_LISTvalue(temp) = (*func)( SHM_LISTnode(temp) );

    return list;
    }


int	_usl_get_shm_list_pos_val( list, pos )
SHM_LIST	list;
int	pos;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_shm_list_pos_val" );

    if (pos >= 1 && pos <= usl_shm_list_length( list ))
	LOOP_THROUGH_SHM_LIST( list, temp )
	    if (--pos == 0) return SHM_LISTvalue( temp );

    return SHM_LIST_DEFAULT_VALUE;
    }

int  	_usl_change_shm_list_val( list, node, new_val )
SHM_LIST	list;
SHM_NODE	node;
int	new_val;
    {
    register int  save;
    register SHM_LIST temp;

    check_list_integrity( list, "usl_change_shm_list_val" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	if (SHM_LISTnode( temp ) == node ) {
	    save = SHM_LISTvalue( temp );
	    SHM_LISTvalue( temp ) = new_val;
	    return save;
	    }
    return SHM_LIST_DEFAULT_VALUE;
    }

/*
 * Sort and reverse SHM_LISTs:
 * usl_sort_shm_list places the nodes into ascending order of 'value'
 *	(value gets set by either usl_insert_shm_list_val or usl_eval_shm_list).
 * usl_sort_shm_list_func sorts the list by applying the predicate function
 * usl_reverse_shm_list is intuitively obvious to the casual obvious.
 */

SHM_LIST	_usl_sort_shm_list_func( list, func )
SHM_LIST	list;
PFI	func;
    {
    register SHM_LIST ptr1, ptr2, temp;

    check_list_integrity( list, "usl_sort_shm_list_func" );

    if (SHM_LISTlength( list ) <= 1)	return list;

    temp = usl_make_shm_list();
    add_first_node( temp, SHM_LISTnode(SHM_LISThead(list)), SHM_LISTvalue(SHM_LISThead(list)) );
    usl_get_from_head( list );

    LOOP_THROUGH_SHM_LIST( list, ptr1 ) {
	LOOP_THROUGH_SHM_LIST( temp, ptr2 )
	    if ((*func)( SHM_LISTnode(ptr1), SHM_LISTnode(ptr2) )) break;

	if (ptr2 == NULLshmlist)
	    add_node_at_tail( temp, SHM_LISTnode(ptr1), SHM_LISTvalue(ptr1) );
	else if (ptr2 -> prev == NULLshmlist)
	    add_node_at_head( temp, SHM_LISTnode(ptr1), SHM_LISTvalue(ptr1) );
	else
	    add_node_before_ptr( temp, ptr2, SHM_LISTnode(ptr1), SHM_LISTvalue(ptr1) );
	}

    swap_lists( temp, list );
    usl_free_shm_list( temp, NOT_NODES );

    return list;
    }

SHM_LIST	_usl_sort_shm_list( list )
SHM_LIST	list;
    {
    register SHM_LIST temp, ptr;

    check_list_integrity( list, "usl_sort_shm_list" );

    if (SHM_LISTlength( list ) <= 1)	return list;

    temp = usl_make_shm_list();
    LOOP_THROUGH_SHM_LIST( list, ptr )
	usl_insert_shm_list_val( temp, SHM_LISTnode(ptr), SHM_LISTvalue(ptr) );
    swap_lists( temp, list );
    usl_free_shm_list( temp, NOT_NODES );

    return list;
    }


SHM_LIST	_usl_reverse_shm_list( list )
SHM_LIST	list;
    {
    register SHM_LIST temp, p;

    check_list_integrity( list, "usl_reverse_shm_list" );

    if (SHM_LISTlength(list) <= 1)	return list;		/* why bother? */

    temp = usl_make_shm_list();
    LOOP_THROUGH_SHM_LIST( list, p )
	usl_add_to_head_val( temp, SHM_LISTnode(p), SHM_LISTvalue(p) );

    swap_lists( temp, list );				/* change pointers */
    usl_free_shm_list( temp, NOT_NODES );			/* trash old list */

    return list;
    }

/*
 * Search and replace list elements.
 * usl_replace_shm_list looks the first occurrance of a node and replaces it with
 *	another node.
 * usl_copy_shm_list will also copy the nodes if the flag is set.
 */


SHM_LIST	_usl_copy_shm_list( list, flag )
SHM_LIST	list;
int	flag;
    {
    register SHM_LIST temp, p;

    check_list_integrity( list, "usl_copy_shm_list" );

    temp = usl_make_shm_list();
    LOOP_THROUGH_SHM_LIST( list, p )
	usl_add_to_tail_val( temp, (flag) ? copy_shm_node(SHM_LISTnode(p)) : SHM_LISTnode(p),
						SHM_LISTvalue(p) );

    return temp;
    }



int	_usl_replace_shm_list( list, old, new )
SHM_LIST	list;
SHM_NODE	old, new;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_replace_shm_list" );

    LOOP_THROUGH_SHM_LIST( list, temp )	if (old == SHM_LISTnode(temp)) break;

    if (temp != NULLshmlist)		SHM_LISTnode(temp) = new;

    return ( temp != NULLshmlist );
    }

/*
 * Probe SHM_LISTs:
 * usl_in_shm_list return the position of a node in the list (0 if it is not there).
 * usl_get_node_val returns the value of a node (0 if the node is not in the list).
 */


int	_usl_in_shm_list( list, node )
SHM_LIST	list;
SHM_NODE	node;
    {
    register SHM_LIST temp;
    register int  count;

    check_list_integrity( list, "usl_in_shm_list" );

    count = 1;
    LOOP_THROUGH_SHM_LIST( list, temp )
	if (node == SHM_LISTnode(temp)) return count;
	else count++;
    return 0;
    }


int	_usl_get_node_val( list, node )
SHM_LIST	list;
SHM_NODE	node;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_node_val" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	if (SHM_LISTnode(temp) == node) return SHM_LISTvalue(temp);

    return SHM_LIST_DEFAULT_VALUE;
    }

/*
 *	Relative postion functions
 */

SHM_NODE	_usl_get_shm_list_next( list, current )
SHM_LIST	list;
SHM_NODE	current;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_shm_list_next" );

    if (current == NULL_SHM_NODE)
	CURRENT( list ) = (SHM_NODE) SHM_LISThead( list );
    else if (CURRENT( list ) && current_node( list ) == current)
	CURRENT( list ) = (SHM_NODE) (current_pos( list ) -> next);
    else {
	CURRENT( list ) = NULL_SHM_NODE;
	LOOP_THROUGH_SHM_LIST( list, temp )
	    if (SHM_LISTnode( temp ) == current) {
		CURRENT( list ) = (SHM_NODE) (temp -> next);
		break;
		}
	}

    return (CURRENT( list ) == NULL_SHM_NODE) ? NULL_SHM_NODE : current_node( list );
    }


SHM_NODE	_usl_get_shm_list_prev( list, current )
SHM_LIST	list;
SHM_NODE	current;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_shm_list_prev" );

    if (current == NULL_SHM_NODE)
	CURRENT( list ) = (SHM_NODE) SHM_LISTtail( list );
    else if (CURRENT( list ) && current_node( list ) == current)
	CURRENT( list ) = (SHM_NODE) (current_pos( list ) -> prev);
    else
	LOOP_THROUGH_SHM_LIST( list, temp )
	    if (SHM_LISTnode( temp ) == current) {
		CURRENT( list ) = (SHM_NODE) (temp -> prev);
		break;
		}

    return (CURRENT( list ) == NULL_SHM_NODE) ? NULL_SHM_NODE : current_node( list );
    }

SHM_NODE	_usl_get_shm_list_next_func( list, current, node, func )
SHM_LIST	list;
SHM_NODE	current, node;
PFI	func;
    {
    register SHM_LIST temp;

    check_list_integrity( list, "usl_get_shm_list_next_func" );

    if (current == NULL_SHM_NODE)
	temp = list;
    else if (CURRENT( list ) && current_node( list ) == current)
	temp = current_pos( list );
    else
	LOOP_THROUGH_SHM_LIST( list, temp )
	    if (SHM_LISTnode( temp ) == current) break;

    LOOP_THROUGH_SHM_LIST( temp, temp )
	if ((*func)(node, SHM_LISTnode( temp ) )) break;
    
    CURRENT( list ) = (SHM_NODE) temp;
    return (CURRENT( list ) == NULL_SHM_NODE) ? NULL_SHM_NODE : current_node( list );
    }


SHM_NODE	__get_shm_list( list, ptr )
SHM_LIST	list;
SHM_NODE	ptr;
    {
    register SHM_LIST temp;

    check_list_integrity( "__get_shm_list", list );
    LOOP_THROUGH_SHM_LIST( list, temp )
	if (SHM_LISTnode( temp ) == ptr)	return (SHM_NODE) temp;

    return NULL_SHM_NODE;
    }

/*
 * Run debugging function on the list package.  This section is not compiled
 * unless debugging is desired.
 */

#if DEBUG

debug_print_list( list )
SHM_LIST list;
    {
    register SHM_LIST temp;

    printf ("\nPrinting list %d, length = %d:\n", list, SHM_LISTlength(list) );

    check_list_integrity( list, "debug_print_list" );

    LOOP_THROUGH_SHM_LIST( list, temp )
	printf( "\t(%d)\tNode %d, value %d, next %d, prev %d\n",
		    temp, SHM_LISTnode(temp), SHM_LISTvalue(temp),
		    temp->next, temp->prev );
    printf ("Done\n");
    }

main() {
    register int i;
    register SHM_LIST list, list2;

    printf( "\ntesting usl_make_shm_list()\n" );
    list = usl_make_shm_list();

    printf( "\ntesting debug_print_list()\n" );
    debug_print_list( list );

    printf( "\ntesting usl_add_to_head()\n" );
    for (i=0; i<TIMES; i++) {
	usl_add_to_head( list, i );
	debug_print_list( list );
	}
    usl_free_shm_list( list, NOT_NODES );

    list = usl_make_shm_list();
    printf( "\ntesting usl_add_to_tail()\n" );
    for (i=0; i<TIMES; i++) {
	usl_add_to_tail( list, i );
	debug_print_list( list );
	}
    usl_free_shm_list( list, NOT_NODES );

    list = usl_make_shm_list();
    printf( "\ntesting usl_insert_shm_list_val()\n" );
    for (i=0; i<TIMES; i++) {
	usl_insert_shm_list_val( list, i, rand() );
	debug_print_list( list );
	}

    list2 = usl_make_shm_list();
    printf( "\ntesting usl_insert_shm_list_pos()\n" );
    for (i=0; i<TIMES; i++) {
	usl_insert_shm_list_pos( list2, i, rand() );
	debug_print_list( list2 );
	}
    usl_insert_shm_list_pos( list2, i/2, i/2 );
    debug_print_list( list2 );

    printf( "\ntesting usl_append_shm_list() and usl_copy_shm_list()\n" );
    debug_print_list( usl_append_shm_list( usl_make_shm_list(), usl_make_shm_list() ) );
    debug_print_list( usl_append_shm_list( usl_make_shm_list(), usl_copy_shm_list(list, NOT_NODES ) ) );
    debug_print_list( usl_append_shm_list( list, 	usl_make_shm_list() ) );
    debug_print_list( usl_append_shm_list( list, usl_copy_shm_list( list2, NOT_NODES ) ) );
    debug_print_list( usl_append_shm_list( list2, list ) );

    memory_stats( stdout, "listPack test program" );
    }
#endif
