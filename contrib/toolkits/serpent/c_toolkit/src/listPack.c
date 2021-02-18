/****************************************************************************

	C Programmer's Toolbox

	Module Name:	listPack

	Purpose:	Provides the data structures necessary to
			support a DECK, and the functions needed
			to give full list access abilities.

	Author:		Robert A Zimmermann, Carnegie-Mellon University
			Department of Electrical and Computer Engineering
			(Inspired by Kevin Nolish, and Chris K. Lata)
			send bugs and inquiries to: raz@cmu-ee-faraday
	
	Use:		a) compile (with or without -g for the debugger) and
			install in any archive library, or
			b) compile with -DDEBUG to generate a test program to
			verify the code, or
			c) compile with -DDO_CHECK=<level> to perform run-time
			list verification (and install in any archive library)
			The errors detected through <level> are:
			    <0> none at all
			    <1> listPack routines being called with NULL LISTs
			    <2> Routines being called with LISTs which were
			        not made by make_list.
			    <3> LISTs whose pointer links are inconsistent.
			    <3> LISTs whose length is wrong.

	History:
	27 Feb 84	Created module

	 2 Apr 84	Did code clean-up moving all list addition through
			routine 'add_node'
			Added flag to copy_list for copying nodes.
			Added main routine for debugging

	 7 May 84	Replaced 'for' loops through LISTs with a macro

	11 May 84	Added macros for LISThead, LISTtail, and LISTlength
			LISTlength in order to make to code cleaner.

	 2 Jul 84	Added append_list

	 9 Jul 84	Made the module lint-free

	12 Jul 84	Added check_list_integrity

	16 Jul 84	Added lists_in_use to integrity checking

	18 Jul 84	Added for_all_2list, for_all_3list

	23 Jul 84	Made eval_list return its list

	01 Aug 84	Added more checking to check_list_integrity

	05 Aug 84	Added delete_list_val

	26 Nov 84	Added flush_list

	 7 Feb 84	Added a few more macros for cleanliness
			Added sort_list_func
	
	18 Jun 85	Added get_list_next, get_list_prev, loop_through_list,
			get_list_next_func, first_in_list, last_in_list
	
	06 Aug 85	Added levels of checking to DO_CHECK

	14 Aug 85	Fixed a few subtle bugs nobody caught

	26 Nov 85	Added change_list_val

	09 Dec 85	Mods to accomodate the new header file

	13 Dec 85	Fixed bug in insert_list_pos

	26 Jun 86	Added routines to conform to manual:
			    insert_list_node( list, node, target )
			    insert_sublist_node( dest, src, node )
			    insert_sublist_pos( dest, src, pos )
			    list_equal( list1, list2, func )

****************************************************************************

Functions for creating and deleting LISTs:

LIST make_list()		Returns a LIST.  This may be passed to any
				listPack routine;

     free_list( list, flag )	Garbage collects the list.  flag is either
				AND_NODES, or NOT_NODES which is whether to
				return the nodes to the free list.

LIST flush_list( list, flag )	same as 'free_list( list, flag );
					 list = make_list();'
				returns the LIST;

LIST copy_list( list, flag )	Creates, copies and returns a list.  flag is
				either AND_NODES or NOT_NODES, which is whether
				to copy the nodes themselves or just the
				pointers.




Functions for adding elements to LISTs:


NODE add_to_head( list, node )	Adds node to the beginning of list.  Returns
				node.

NODE add_to_head_val( list, node )	Adds node with tag value to the
				beginning of list.  Returns node.

NODE add_to_tail( list, node )	Adds node to the end of list.  Returns node.

NODE add_to_head_val( list, node )	Adds node with tag value to the
				end of list.  Returns node.

NODE insert_list_pos( list, node, pos )
				Adds node before position number 'pos'.  If
				out of range, node is added to head or tail.

NODE insert_list_node( list, node, target )
				Adds node before the first occurrance of
				the target node.  If not present, node is added
				to the tail.

NODE insert_list_val( list, node, val )
				Adds node in ascending order of 'val'.  If
				list values are not in order, well...

LIST append_list( first, second )
				LIST second is appended to LIST first. The
				LIST returned is 'first' so that:
				new = append_list( copy_list(a), copy_list(b) )
				will work as expected.
				Note: 'second' is destroyed by append_list!
				Note: append_list( a, a ) is ignored!

LIST insert_sublist_node( dest, src, node )
				src is inserted into dest before the first
				occurrance of node.  If not present, src is
				appended.

LIST insert_sublist_pos( dest, src, pos )
				src is inserted into dest before the the pos-th
				node.  If pos <= 1 src is pre-pended.  If
				pos > list_length, src is appended.

Function for retrieving and removing nodes from LISTs:

NODE get_list_head( list )	returns (but does not remove) the node at the
				beginning of the list.

NODE get_list_tail( list )	returns (but does not remove) the node at the
				end of the list.

NODE get_from_head( list )	Removes and returns the node at the beginning
				of the list.

NODE get_from_tail( list )	Removes and returns the node at the end of
				the list.

NODE get_list_pos( list, pos )	Returns (but does not remove) the node at 'pos'.
				If pos is out of range, it returns NULL.

NODE get_list_val( list, val )	Returns (but does not remove) the first node
				whose value == val.  Otherwise returns NULL;

int  get_list_pos_val( list, pos, val )
				Returns the value of the node at pos.

int  change_list_val( list, node, new_val )
				Sets the value of the specified node
				to new_val.  Returns the old value.

NODE delete_list_pos( list, pos )	get_list_pos with node removal

NODE delete_list_val( list, val )	get_list_val with node removal

NODE delete_list_node( list, node )	Returns and removes 'node' from list.
					Returns NULL if 'node' was not there.

NODE get_list_next( list, node )	Returns the element in the list
					after 'node'.  If node is NULL, the
					first element is returned.

NODE get_list_prev( list, node )	Returns the element in the list
					before 'node'.  If node is NULL, the
					last element is returned.

Applying C functions to LISTs:

int for_all_list ( list, node, func )
int for_all_2list( list, node, node2, func )
int for_all_3list( list, node, node2, node3, func )
				Applies (*func)( node, <list node> ) to all
				of the nodes in the list.  Returns the sum of
				the values returned by all calls to func.

int list_equal( list1, list2, func )
				Apply the function to both lists until it
				returns FALSE or the end of the lists is
				reached.  Just compare nodes if func == NULL.

LIST eval_list( list, func )	Sets the value of all nodes in the list to
				func( <list node> ).  The list is returned.

NODE search_list( list, node, func )
				Applies (*func)( node, <list node> ) to all
				of the nodes in the list.  Returns the first
				node on which func returns non-zero (i.e. TRUE).

NODE get_list_next_func( list, start_node, node, func )
				As search_list( ... ), but starts at start-node.

LIST sort_list( list )		Sorts list into ascending order of the value
				of each node.  These can be set with eval_list,
				change_list_val, or insert_list_val.
				The list is returned.

LIST reverse_list( list )	Reverses the order of list.  Also useful if
				you want sort_list to sort in descending order:
				reverse_list( sort_list( list ) )
				The list is returned.

LIST sort_list_func( list, func )
				Sorts the list by applying function to pairs
				of nodes.  The boolean, func( node1, node2 ),
				returns TRUE if the nodes are ordered correctly
				(i.e. node1 precedes node2), FALSE otherwise.

Additions LIST utility functions:

int list_empty( list )		Is the list empty?

int list_length( list )		Returns the number of nodes in the list.

int in_list( list, node )	Returns FALSE (i.e. 0) if the node is not in
				the list.  Otherwise, the position is returned.

int last_in_list( list, node )
int first_in_list( list, node )	Boolean functions which indicate whether the
				specified node is first (last) in the list.

int replace_list( list, oldNode, newNode )
				Returns TRUE if it could replace the first
				occurance of oldNode with newNode.

int get_node_val( list, node )	Returns the value of <list node>.  Or 0.

int get_head_val( list )	Returns the value of <list head>.  Or 0.

int get_tail_val( list )	Returns the value of <list tail>.  Or 0.

**************************************************************************/

/*
 * External Definitions and debugging macros:
 */


#include "listPack.h"			/* Definitions for procedures	*/
					/* and LIST structures		*/

#ifdef DEBUG				/* When running with DEBUG	*/

#ifndef DO_CHECK			/* make sure that run-time LIST	*/
#define DO_CHECK 3			/* integrity checking is turned */
#endif					/* on				*/

#define TIMES 5				/* size of test lists		*/
#endif

#ifndef DO_CHECK
#define DO_CHECK 0
#endif

#if DO_CHECK > 0			/* For integrity checking, we	*/
					/* need the standard I/O defs,	*/
typedef char * STRING;			/* our definition of a STRING	*/
#include <stdio.h>			/* and an internal LIST to keep	*/
					/* track of the LISTs which are	*/
static	LIST lists_in_use = NULLlist;	/* valid at any given point in	*/
					/* time, otherwise make the	*/
#else					/* debug checker an un-routine!	*/
#define check_list_integrity( list, routine )
#endif

/*	Macros to make the code cleaner:		*/

#define	LISTlength( list )	(list -> value)

#define CURRENT( list )		(LISTnode( list ))
#define current_pos( list )	((LIST) CURRENT( list ))
#define current_node( list )	(LISTnode(current_pos( list )))


					/* This macro assures uniform	*/
					/* traversal through LISTs	*/
#define LOOP_THROUGH_LIST( list, reg ) \
    for( reg = LISThead( list ); reg != NULLlist; reg = reg -> next )



#define	add_node_at_tail( list, node, val ) \
    _list_add_node( list, &LISTtail(list), &LISTtail(list)->next, node, val )


#define	add_node_before_ptr( list, temp, node, val ) \
    _list_add_node( list, &temp->prev, &temp->prev->next, node, val )


#define	add_node_at_head( list, node, val ) \
    _list_add_node( list, &LISThead(list)->prev, &LISThead(list), node, val )


#define	add_first_node( list, node, val ) \
    _list_add_node( list, &LISTtail(list), &LISThead(list), node, val )

/*
 * If checking, we must clean up a temp list before freeing it,
 * otherwise, just free off the header node.
 */
#if DO_CHECK > 0
#define destroy_list( list )	LISThead( list ) = NULLlist;	\
				LISTtail( list ) = NULLlist;	\
				LISTlength( list ) = 0;		\
				free_list( list, NOT_NODES );
#else
#define destroy_list( list )	free_node( list );
#endif

/*	This routine performs serious run-time checking on a LIST.	*/

#if DO_CHECK > 0
	check_list_integrity( list, routine )
LIST	list;
STRING	routine;
    {
    register LIST	temp, save = NULLlist;
    register int	length = 0;

    if (list == NULLlist)
	fprintf( stderr, "'%s' called with NULL list\n", routine );
    else if (lists_in_use == NULLlist)
	fprintf( stderr, "'%s' called before any lists made\n", routine );

#if DO_CHECK > 1
    else {
	LOOP_THROUGH_LIST( lists_in_use, temp )
	    if (list == (LIST) LISTnode( temp ) ) break;

	if (list != lists_in_use && temp == NULLlist)
	    fprintf( stderr, "'%s' called with unknown list\n", routine );

#if DO_CHECK > 2
	LOOP_THROUGH_LIST( list, temp ) {
	    if (temp->prev != save)
		fprintf( stderr, "Error in prev pointer: %d != %d in '%s'\n",
				    temp->prev, save, routine );
	    if (temp->prev == NULLlist && LISThead(list) != temp)
		fprintf( stderr, "Head of list doesn't match node %d in '%s'\n",
				    temp, routine );
	    if (temp->next == NULLlist && LISTtail(list) != temp)
		fprintf( stderr, "Tail of list doesn't match node %d in '%s'\n",
				    temp, routine );
	    if (temp->next != NULLlist) if (temp->next->prev != temp )
		fprintf ( stderr, "Error in next pointer: %d != %d in '%s'\n",
				    temp->next, temp->next->prev, routine );
	    save = temp;
	    length++;
	    }
	if (length != LISTlength(list))
	    fprintf( stderr, "Length of list is incorrect in '%s'\n", routine );
#endif
	}
#endif
    }
#endif 
/*
 * Internal utility routines (insert node, remove node)
 * NODE _list_add_node() Inserts a new node into a LIST.  It then returns a
 * pointer to the new LIST record.
 * _list_remove_node() Removes a node from a LIST.  It then returns the
 * NODE at the node which was deleted.
 */

NODE	_list_add_node( list, prev, next, node, val )
LIST	list, *prev, *next;
NODE	node;
int	val;
    {
    register LIST temp;

    check_list_integrity( list, "_list_add_node" );

    temp = (LIST) make_node( sizeof (LISTNODE) );
    temp->prev  = *prev;
    temp->next  = *next;
    temp->node  = node;
    temp->value = val;
    *prev = temp;
    *next = temp;
    LISTlength( list )++;
    return node;
    }


NODE	_list_remove_node( ptr, list )
LIST	ptr, list;
    {
    if (ptr == NULLlist) return NULLnode;

    LISTlength( list )--;
    if (ptr->prev == NULLlist)	LISThead( list )	= ptr->next;
    else			ptr->prev->next = ptr->next;
    if (ptr->next == NULLlist)	LISTtail( list )	= ptr->prev;
    else			ptr->next->prev = ptr->prev;
    free_node( (NODE) ptr );
    return LISTnode( ptr );
    }

/*
 * Internal utility routines (swap pointers between LISTs)
 */

#define	swap_ptrs( p1, p2, type )	\
    	    tptr = (LIST) p1;		\
	    p1   = p2;			\
	    p2   = (type) tptr;		\


static	swap_lists( temp, list )
LIST	temp, list;
    {
    register LIST tptr;

    swap_ptrs( LISThead( list ), LISThead( temp ), LIST );
    swap_ptrs( LISTtail( list ), LISTtail( temp ), LIST );
    swap_ptrs( LISTlength( list ), LISTlength( temp ), int );
    }

/*
 * List insertion routines:
 * insert_list_node will add a node to a list before a target node.
 * If the target node is not present, the node will be added to the tail
 */

NODE	_insert_list_node( list, node, target )
LIST	list;
NODE	node, target;
    {
    register LIST ptr;

    check_list_integrity( list, "insert_list_node" );

    if (LISTlength(list) == 0)
	return add_first_node( list, node, LIST_DEFAULT_VALUE );

    if (first_in_list( list, target ))
	return add_node_at_head( list, node, LIST_DEFAULT_VALUE );

    LOOP_THROUGH_LIST( list, ptr )
	if (LISTnode( ptr ) == target)
	    return add_node_before_ptr( list, ptr, node, LIST_DEFAULT_VALUE );

    return add_node_at_tail( list, node, LIST_DEFAULT_VALUE );
    }

/*
 * List insertion routines:
 * insert_list_pos will add a node to a list 'pos' nodes from the head.
 * If pos <= 1 it will go onto the head; pos >= length will go onto the tail
 */

NODE	_insert_list_pos( list, node, pos )
LIST	list;
NODE	node;
int	pos;
    {
    register LIST ptr;

    check_list_integrity( list, "insert_list_pos" );

    if (LISTlength(list) == 0)
	return add_first_node( list, node, LIST_DEFAULT_VALUE );

    if (pos <= 1)
	return add_node_at_head( list, node, LIST_DEFAULT_VALUE );

    if (LISTlength(list) >= pos)
	LOOP_THROUGH_LIST( list, ptr ) if (pos-- <= 1)
	    return add_node_before_ptr( list, ptr, node, LIST_DEFAULT_VALUE );

    return add_node_at_tail( list, node, LIST_DEFAULT_VALUE );
    }

/*
 * List insertion routines:
 * insert_list_val will add a node in ascending order with respect to
 *		the values already assigned to the nodes in the list. 
 */


NODE	_insert_list_val( list, node, val )
LIST	list;
NODE	node;
int	val;
    {
    register LIST ptr;

    check_list_integrity( list, "insert_list_val" );

    if (LISTlength(list) == 0)
	return add_first_node(list, node, val);

    if (LISTvalue(LISThead(list)) >= val)
	return add_node_at_head( list, node, val );

    if (LISTvalue(LISTtail(list)) <= val)
	return add_node_at_tail( list, node, val );

    LOOP_THROUGH_LIST( list, ptr )
	if (LISTvalue( ptr ) > val) break;

    return add_node_before_ptr( list, ptr, node, val );
    }

/*
 * Random access node retrieval.
 * get_list_pos reveals the pos-th entry in the list.
 * get_list_val reveals the first node with the given value
 */


NODE	_get_list_pos( list, pos )
LIST	list;
int	pos;
    {
    register LIST temp;

    check_list_integrity( list, "get_list_pos" );

    if (pos > 0 && pos <= LISTlength( list ))
	LOOP_THROUGH_LIST( list, temp )	if (--pos == 0) return LISTnode(temp);

    return NULLnode;
    }


NODE	_get_list_val( list, val )
LIST	list;
int	val;
    {
    register LIST temp;

    check_list_integrity( list, "get_list_val" );

    LOOP_THROUGH_LIST( list, temp )
	if (LISTvalue(temp) == val) return LISTnode(temp);

    return NULLnode;
    }
/* random node removal routines:
 * delete_list_val removes the entry whose value is 'val' from the list.
 * delete_list_pos removes the pos-th entry from the list
 * delete_list_node removes the names node from the list
 *
 * All three return the node to the caller.
 */

NODE	_delete_list_val( list, val )
LIST	list;
int	val;
    {
    register LIST temp;

    check_list_integrity( list, "delete_list_val" );

    LOOP_THROUGH_LIST( list, temp )
	if (LISTvalue( temp ) == val) return _list_remove_node( temp, list );
    return NULLnode;
    }


NODE	_delete_list_pos( list, pos )
LIST	list;
int	pos;
    {
    register LIST temp;

    check_list_integrity( list, "delete_list_pos" );

    if (pos > 0 && pos <= LISTlength(list))
	LOOP_THROUGH_LIST( list, temp )
	    if (--pos == 0) return _list_remove_node( temp, list );
    return NULLnode;
    }


NODE	_delete_list_node( list, node )
LIST	list;
NODE	node;
    {
    register LIST temp;

    check_list_integrity( list, "delete_list_node" );

    LOOP_THROUGH_LIST( list, temp )
	if (LISTnode( temp ) == node) return _list_remove_node( temp, list );
    return NULLnode;
    }

/*
 * list creation:
 * make_list (make_queue, make_stack) returns a virgin list to the caller.
 */

#define create_list( temp )				\
	temp = (LIST) make_node( sizeof( LISTNODE ) );	\
	LISThead( temp ) = NULLlist;			\
	LISTtail( temp ) = NULLlist;			\
	CURRENT( temp )  = NULLnode;			\
	LISTlength( temp ) = 0;


LIST	_make_list()
    {
    register LIST temp;
		
    create_list( temp );		/* allocate a new list structure   */

#if DO_CHECK > 0			/* must maintain list of lists	   */
    if (lists_in_use == NULLlist) {	/* need to allocate list of lists  */
	create_list( lists_in_use );
	}
					/* save and return the list	   */
    return (LIST) add_to_head( lists_in_use, temp );
#else

    return temp;

#endif
    }

/*
 * flush_list: clear a list for re-use
 * LISTs may not be accessed after calls to free_list.  If free_list is called
 * with AND_NODES, the nodes are also returned to the garbage collector, and
 * they, too, may not be accessed further.
 */
	
LIST	_flush_list( list, free_nodes )
LIST	list;
int	free_nodes;
    {
    register LIST temp;		/* NOTE:  free_node does NOT destroy the data */
				/* in the node, so it is safe to free a node, */
    				/* and then access it immediately thereafter. */

    if (list == NULLlist) return make_list();

    check_list_integrity( list, "flush_list" );

    LOOP_THROUGH_LIST( list, temp ) {
	if (free_nodes == AND_NODES) free_node( LISTnode( temp ) );
	free_node( (NODE) temp );
	}

    LISThead( list ) = LISTtail( list ) = NULLlist;
    LISTlength( list ) = 0;

    return list;
    }


	
void	_free_list( list, free_nodes )
LIST	list;
int	free_nodes;
    {
    free_node( (NODE) flush_list( list, free_nodes ) );

#if DO_CHECK > 0		/* Remove list from the list of lists	*/
    delete_list_node( lists_in_use, list );
#endif
    }

/*
 * Append LISTs together
 */

LIST	_append_list( list1, list2 )
LIST	list1, list2;
    {
    check_list_integrity( list1, "append_list (first argument)" );
    check_list_integrity( list2, "append_list (second argument)" );

    if (list1 == list2)	return list1;		/* ignore this call	*/

    if ( LISTlength(list2) != 0 )		/* empty? free it later */

	if ( LISTlength(list1) == 0 ) {		/* If list1 was empty	*/
	    LISThead(list1) = LISThead(list2);	/* move head pointer	*/
	    LISTtail(list1) = LISTtail(list2);	/* move tail pointer	*/
	    }

	else {					/* no? relink the tail	*/
	    LISTtail(list1)->next	= LISThead(list2);
	    LISTtail(list1)->next->prev = LISTtail(list1);
	    LISTtail(list1)		= LISTtail(list2);
	    }

    LISTlength(list1) += LISTlength(list2);	/* Set new list length	*/

    destroy_list( list2 );			/* invalid, so trash it	*/

    return list1;				/* return valid LIST	*/
    }

/*
 * Insert a sublist into a LIST at a position
 */

LIST	_insert_sublist_pos( list1, list2, pos )
LIST	list1, list2;
int	pos;
    {
    register LIST temp;

    check_list_integrity( list1, "insert_sublist_pos (first argument)" );
    check_list_integrity( list2, "insert_sublist_pos (second argument)" );

    if (list1 == list2)	return list1;		/* ignore this call	*/

    /* If there is nothing in list2, all we do is free it at the end	*/
    if ( LISTlength(list2) != 0 )

	if ( LISTlength(list1) == 0 ) {		/* If list1 was empty	*/
	    LISThead(list1) = LISThead(list2);	/* move head pointer	*/
	    LISTtail(list1) = LISTtail(list2);	/* move tail pointer	*/
	    }

	else if (pos <= 1) {			/* prepend list2	*/
	    LISThead(list1)->prev = LISTtail(list2);
	    LISThead(list1)	  = LISThead(list2);
	    }

	else if (pos > LISTlength(list1)) {	/* append list2		*/
	    LISTtail(list1)->next = LISThead(list2);
	    LISTtail(list1)	  = LISTtail(list2);
	    }

	else 					/* ok. find the position */
	    LOOP_THROUGH_LIST( list1, temp )
		if (--pos == 1) {		/* insert here		*/
		    temp->next->prev	= LISTtail( list2 );
		    temp->next		= LISThead( list2 );
		    break;
		    }

    LISTlength(list1) += LISTlength(list2);	/* Set new list length	*/

    destroy_list( list2 );			/* invalid, so trash it	*/

    return list1;				/* return valid LIST	*/
    }

/*
 * Insert a sublist into a LIST after a target node
 */

LIST	_insert_sublist_node( list1, list2, node )
LIST	list1, list2;
NODE	node;
    {
    register LIST temp;

    check_list_integrity( list1, "insert_sublist_node (first argument)" );
    check_list_integrity( list2, "insert_sublist_node (second argument)" );

    if (list1 == list2)	return list1;		/* ignore this call	*/

    /* If there is nothing in list2, all we do is free it at the end	*/
    if ( LISTlength(list2) != 0 )

	if ( LISTlength(list1) == 0 ) {		/* If list1 was empty	*/
	    LISThead(list1) = LISThead(list2);	/* move head pointer	*/
	    LISTtail(list1) = LISTtail(list2);	/* move tail pointer	*/
	    }

	else if (first_in_list(list1,node)) {	/* prepend list2	*/
	    LISThead(list1)->prev = LISTtail(list2);
	    LISThead(list1)	  = LISThead(list2);
	    }

	else {					/* ok. find the target	*/
	    LOOP_THROUGH_LIST( list1, temp )
		if (LISTnode(temp) == node) {	/* insert here		*/
		    temp->next->prev = LISTtail( list2 );
		    temp->next	     = LISThead( list2 );
		    break;
		    }

	    if (temp == NULLlist) {		/* not in list:  append	*/
		LISTtail(list1)->next	= LISThead(list2);	/* link head */
		LISTtail(list1)		= LISTtail(list2);	/* move tail */
		}
	    }

    LISTlength(list1) += LISTlength(list2);	/* Set new list length	*/

    destroy_list( list2 );			/* invalid, so trash it	*/

    return list1;				/* return valid LIST	*/
    }

/*
 * List processing.  These routines take (as a parameter) a pointer to a
 *	'C' function which will then be called with each element in the LIST.
 *	Also, they take one (two or three) pointers which will also be passed
 *	to the 'C' function.
 *
 * for_all_list applies a function to the nodes of a list, and returns the
 *	sum of the values returned by that function.
 * for_all_2list applies a function to the nodes of a list, and returns the
 *	sum of the values returned by that function.
 */


int	_for_all_list( list, node, func )
LIST	list;
NODE	node;
PFI	func;			/* pointer to function returning integer */
    {
    register int total;
    register LIST temp;

    check_list_integrity( list, "for_all_list" );

    total = 0;
    LOOP_THROUGH_LIST( list, temp ) total += (*func)( node, LISTnode( temp ) );
    return total;
    }


int	_for_all_2list( list, node, node2, func )
LIST	list;
NODE	node, node2;
PFI	func;			/* pointer to function returning integer */
    {
    register int total;
    register LIST temp;

    check_list_integrity( list, "for_all_2list" );

    total = 0;
    LOOP_THROUGH_LIST( list, temp )
	total += (*func)( node, node2, LISTnode( temp ) );
    return total;
    }

/*
 * List processing.  These routines take (as a parameter) a pointer to a
 * 'C' function which will then be called with each element in the LIST.
 *
 * for_all_3list applies a function to the nodes of a list, and returns the
 *	sum of the values returned by that function.
 */


int	_for_all_3list( list, node, node2, node3, func )
LIST	list;
NODE	node, node2, node3;
PFI	func;			/* pointer to function returning integer */
    {
    register int total;
    register LIST temp;

    check_list_integrity( list, "for_all_3list" );

    total = 0;
    LOOP_THROUGH_LIST( list, temp )
	total += (*func)( node, node2, node3, LISTnode( temp ) );
    return total;
    }

/* search_list applies a function to each node until the function returns
 *	TRUE.  It then returns a pointer to that node.
 * list_equal tests to see if two lists are equivalent
 */


NODE	_search_list( list, node, func )
LIST	list;
NODE	node;
PFI	func;			/* pointer to function returning integer */
    {
    register LIST temp;

    check_list_integrity( list, "search_list" );

    LOOP_THROUGH_LIST( list, temp )
	if ((*func)( node, LISTnode( temp ) )) return LISTnode( temp );

    return NULLnode;
    }


int	_list_equal( list1, list2, func )
LIST	list1, list2;
PFI	func;
    {
    register LIST ptr1, ptr2;

    check_list_integrity( list1, "list_equal (first arg)" );
    check_list_integrity( list2, "list_equal (second arg)" );

    if (LISTlength( list1 ) != LISTlength( list2 )) return 0;

    ptr2 = LISThead( list2 );
    LOOP_THROUGH_LIST( list1, ptr1 ) {
	if (LISTnode(ptr1) != LISTnode(ptr2) &&
	    (func == ((PFI) 0) || (*func)(LISTnode(ptr1), LISTnode(ptr2)) == 0))
		return 0;
	ptr2 = ptr2->next;
	}
    
    return 1;
    }

/*
 * Set all of the value fields within a LIST.  This is used for sorting LISTs
 * and for saving time on long evaluation functions.
 * eval_list sets all of the values of the nodes of a list to a function
 *	of the node.
 * change_list_val sets the value of a single node
 * get_list_pos_val returns the value of the pos-th node
 */


LIST	_eval_list( list, func )
LIST	list;
PFI	func;			/* pointer to function returning integer */
    {
    register LIST temp;

    check_list_integrity( list, "eval_list" );

    LOOP_THROUGH_LIST( list, temp ) LISTvalue(temp) = (*func)( LISTnode(temp) );

    return list;
    }


int	_get_list_pos_val( list, pos )
LIST	list;
int	pos;
    {
    register LIST temp;

    check_list_integrity( list, "get_list_pos_val" );

    if (pos >= 1 && pos <= list_length( list ))
	LOOP_THROUGH_LIST( list, temp )
	    if (--pos == 0) return LISTvalue( temp );

    return LIST_DEFAULT_VALUE;
    }


int  	_change_list_val( list, node, new_val )
LIST	list;
NODE	node;
int	new_val;
    {
    register int  save;
    register LIST temp;

    check_list_integrity( list, "change_list_val" );

    LOOP_THROUGH_LIST( list, temp )
	if (LISTnode( temp ) == node ) {
	    save = LISTvalue( temp );
	    LISTvalue( temp ) = new_val;
	    return save;
	    }
    return LIST_DEFAULT_VALUE;
    }

/*
 * Sort and reverse LISTs:
 * sort_list places the nodes into ascending order of 'value'
 *	(value gets set by either insert_list_val or eval_list).
 * sort_list_func sorts the list by applying the predicate function
 * reverse_list is intuitively obvious to the casual obvious.
 */

LIST	_sort_list_func( list, func )
LIST	list;
PFI	func;
    {
    register LIST ptr1, ptr2, temp;

    check_list_integrity( list, "sort_list_func" );

    if (LISTlength( list ) <= 1)	return list;

    temp = make_list();
    add_first_node( temp, LISTnode(LISThead(list)), LISTvalue(LISThead(list)) );
    get_from_head( list );

    LOOP_THROUGH_LIST( list, ptr1 ) {
	LOOP_THROUGH_LIST( temp, ptr2 )
	    if ((*func)( LISTnode(ptr1), LISTnode(ptr2) )) break;

	if (ptr2 == NULLlist)
	    add_node_at_tail( temp, LISTnode(ptr1), LISTvalue(ptr1) );
	else if (ptr2 -> prev == NULLlist)
	    add_node_at_head( temp, LISTnode(ptr1), LISTvalue(ptr1) );
	else
	    add_node_before_ptr( temp, ptr2, LISTnode(ptr1), LISTvalue(ptr1) );
	}

    swap_lists( temp, list );
    free_list( temp, NOT_NODES );

    return list;
    }

LIST	_sort_list( list )
LIST	list;
    {
    register LIST temp, ptr;

    check_list_integrity( list, "sort_list" );

    if (LISTlength( list ) <= 1)	return list;

    temp = make_list();
    LOOP_THROUGH_LIST( list, ptr )
	insert_list_val( temp, LISTnode(ptr), LISTvalue(ptr) );
    swap_lists( temp, list );
    free_list( temp, NOT_NODES );

    return list;
    }


LIST	_reverse_list( list )
LIST	list;
    {
    register LIST temp, p;

    check_list_integrity( list, "reverse_list" );

    if (LISTlength(list) <= 1)	return list;		/* why bother? */

    temp = make_list();
    LOOP_THROUGH_LIST( list, p )
	add_to_head_val( temp, LISTnode(p), LISTvalue(p) );

    swap_lists( temp, list );				/* change pointers */
    free_list( temp, NOT_NODES );			/* trash old list */

    return list;
    }

/*
 * Search and replace list elements.
 * replace_list looks the first occurrance of a node and replaces it with
 *	another node.
 * copy_list will also copy the nodes if the flag is set.
 */


LIST	_copy_list( list, flag )
LIST	list;
int	flag;
    {
    register LIST temp, p;

    check_list_integrity( list, "copy_list" );

    temp = make_list();
    LOOP_THROUGH_LIST( list, p )
	add_to_tail_val( temp, (flag) ? copy_node(LISTnode(p)) : LISTnode(p),
						LISTvalue(p) );

    return temp;
    }



int	_replace_list( list, old, new )
LIST	list;
NODE	old, new;
    {
    register LIST temp;

    check_list_integrity( list, "replace_list" );

    LOOP_THROUGH_LIST( list, temp )	if (old == LISTnode(temp)) break;

    if (temp != NULLlist)		LISTnode(temp) = new;

    return ( temp != NULLlist );
    }

/*
 * Probe LISTs:
 * in_list return the position of a node in the list (0 if it is not there).
 * get_node_val returns the value of a node (0 if the node is not in the list).
 */


int	_in_list( list, node )
LIST	list;
NODE	node;
    {
    register LIST temp;
    register int  count;

    check_list_integrity( list, "in_list" );

    count = 1;
    LOOP_THROUGH_LIST( list, temp )
	if (node == LISTnode(temp)) return count;
	else count++;
    return 0;
    }


int	_get_node_val( list, node )
LIST	list;
NODE	node;
    {
    register LIST temp;

    check_list_integrity( list, "get_node_val" );

    LOOP_THROUGH_LIST( list, temp )
	if (LISTnode(temp) == node) return LISTvalue(temp);

    return LIST_DEFAULT_VALUE;
    }

/*
 *	Relative postion functions
 */

NODE	_get_list_next( list, current )
LIST	list;
NODE	current;
    {
    register LIST temp;

    check_list_integrity( list, "get_list_next" );

    if (current == NULLnode)
	CURRENT( list ) = (NODE) LISThead( list );
    else if (CURRENT( list ) && current_node( list ) == current)
	CURRENT( list ) = (NODE) (current_pos( list ) -> next);
    else {
	CURRENT( list ) = NULLnode;
	LOOP_THROUGH_LIST( list, temp )
	    if (LISTnode( temp ) == current) {
		CURRENT( list ) = (NODE) (temp -> next);
		break;
		}
	}

    return (CURRENT( list ) == NULLnode) ? NULLnode : current_node( list );
    }


NODE	_get_list_prev( list, current )
LIST	list;
NODE	current;
    {
    register LIST temp;

    check_list_integrity( list, "get_list_prev" );

    if (current == NULLnode)
	CURRENT( list ) = (NODE) LISTtail( list );
    else if (CURRENT( list ) && current_node( list ) == current)
	CURRENT( list ) = (NODE) (current_pos( list ) -> prev);
    else
	LOOP_THROUGH_LIST( list, temp )
	    if (LISTnode( temp ) == current) {
		CURRENT( list ) = (NODE) (temp -> prev);
		break;
		}

    return (CURRENT( list ) == NULLnode) ? NULLnode : current_node( list );
    }

NODE	_get_list_next_func( list, current, node, func )
LIST	list;
NODE	current, node;
PFI	func;
    {
    register LIST temp;

    check_list_integrity( list, "get_list_next_func" );

    if (current == NULLnode)
	temp = list;
    else if (CURRENT( list ) && current_node( list ) == current)
	temp = current_pos( list );
    else
	LOOP_THROUGH_LIST( list, temp )
	    if (LISTnode( temp ) == current) break;

    LOOP_THROUGH_LIST( temp, temp )
	if ((*func)(node, LISTnode( temp ) )) break;
    
    CURRENT( list ) = (NODE) temp;
    return (CURRENT( list ) == NULLnode) ? NULLnode : current_node( list );
    }


NODE	__get_list( list, ptr )
LIST	list;
NODE	ptr;
    {
    register LIST temp;

    check_list_integrity( "__get_list", list );
    LOOP_THROUGH_LIST( list, temp )
	if (LISTnode( temp ) == ptr)	return (NODE) temp;

    return NULLnode;
    }

/*
 * Run debugging function on the list package.  This section is not compiled
 * unless debugging is desired.
 */

#if DEBUG

debug_print_list( list )
LIST list;
    {
    register LIST temp;

    printf ("\nPrinting list %d, length = %d:\n", list, LISTlength(list) );

    check_list_integrity( list, "debug_print_list" );

    LOOP_THROUGH_LIST( list, temp )
	printf( "\t(%d)\tNode %d, value %d, next %d, prev %d\n",
		    temp, LISTnode(temp), LISTvalue(temp),
		    temp->next, temp->prev );
    printf ("Done\n");
    }

main() {
    register int i;
    register LIST list, list2;

    printf( "\ntesting make_list()\n" );
    list = make_list();

    printf( "\ntesting debug_print_list()\n" );
    debug_print_list( list );

    printf( "\ntesting add_to_head()\n" );
    for (i=0; i<TIMES; i++) {
	add_to_head( list, i );
	debug_print_list( list );
	}
    free_list( list, NOT_NODES );

    list = make_list();
    printf( "\ntesting add_to_tail()\n" );
    for (i=0; i<TIMES; i++) {
	add_to_tail( list, i );
	debug_print_list( list );
	}
    free_list( list, NOT_NODES );

    list = make_list();
    printf( "\ntesting insert_list_val()\n" );
    for (i=0; i<TIMES; i++) {
	insert_list_val( list, i, rand() );
	debug_print_list( list );
	}

    list2 = make_list();
    printf( "\ntesting insert_list_pos()\n" );
    for (i=0; i<TIMES; i++) {
	insert_list_pos( list2, i, rand() );
	debug_print_list( list2 );
	}
    insert_list_pos( list2, i/2, i/2 );
    debug_print_list( list2 );

    printf( "\ntesting append_list() and copy_list()\n" );
    debug_print_list( append_list( make_list(), make_list() ) );
    debug_print_list( append_list( make_list(), copy_list(list, NOT_NODES ) ) );
    debug_print_list( append_list( list, 	make_list() ) );
    debug_print_list( append_list( list, copy_list( list2, NOT_NODES ) ) );
    debug_print_list( append_list( list2, list ) );

    memory_stats( stdout, "listPack test program" );
    }
#endif
