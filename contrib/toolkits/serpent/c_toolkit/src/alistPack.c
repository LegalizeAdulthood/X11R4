/****************************************************************************

	C Programmer's Toolbox

	Module Name:	alistPack

	Purpose:	Provides the data structures necessary to
			support a DECK, and the functions needed
			to give full alist access abilities.

			The functions provided are identical to those in
			listPack, but implement the alist as arrays of
			pointers rather than linked lists of nodes.

			The alist nodes contain the length of the alist,
			a pointer to an array of node pointers, a current
			index into that array, and the maximum number of
			entries the array can hold.

			Each alist will hold (max_nodes - 1) entries.
			That is done so that 2^n byte memory allocators
			(like memoryPack) will hold them efficiently

	Author:		Robert A Zimmermann, Carnegie-Mellon University
			Department of Electrical and Computer Engineering
			send bugs and inquiries to: raz@cmu-ee-faraday
	
	Use:		a) compile (with or without -g for the debugger) and
			install in any archive library, or
			b) compile with -DDEBUG to generate a test program to
			verify the code, or
			c) compile with -DDO_CHECK=<level> to perform run-time
			alist verification (and install in any archive library)
			The errors detected through <level> are:
			    <0> none at all
			    <1> alistPack routines being called with NULL ALISTs
			    <2> Routines being called with ALISTs which were
			        not made by make_alist.
			    <3> ALISTs whose length is wrong.
			d) compile -DNO_MALLOC to force alistPack not to use
			the malloc in memoryPack

	History:
	13 Aug 85	Created module out of listPack

	26 Nov 85	Added change_alist_val

	13 Dec 85	Second version, as per listPack

	27 Jun 86	Added functions top conform with documentation
				insert_alist_node( alist, alist2, node )
				insert_subalist_pos( alist, alist2, pos )
				insert_subalist_node( alist, alist2, node )
				alist_equal( alist1, alist2, func )
	
	21 Jul 86	Fixed bug in _add_alist_node

****************************************************************************

Functions for creating and deleting ALISTs:

ALIST make_alist()		Returns a ALIST.  This may be passed to any
				alistPack routine;

     free_alist( alist, flag )	Garbage collects the alist.  flag is either
				AND_NODES, or NOT_NODES which is whether to
				return the nodes to the free alist.

ALIST flush_alist(alist, flag )	same as 'free_alist( alist, flag );
					 alist = make_alist();'
				returns the ALIST;

ALIST copy_alist( alist, flag )	Creates, copies and returns a alist.  flag is
				either AND_NODES or NOT_NODES, which is whether
				to copy the nodes themselves or just the
				pointers.




Functions for adding elements to ALISTs:


NODE add_to_ahead(alist, node )	Adds node to the beginning of alist.  Returns
				node.

NODE add_to_atail(alist, node )	Adds node to the end of alist.  Returns
				node.

NODE insert_alist_node( alist, node, target )
				Adds node before the first occurrance of
				the target node.  If not present, node is added
				to the tail.
NODE insert_alist_pos( alist, node, pos )
				Adds node before position number 'pos'.  If
				out of range, node is added to head or tail.

NODE insert_alist_val( alist, node, val )
				Adds node in ascending order of 'val'.  If
				alist values are not in order, well...

int   change_alist_val( alist, node, new_val )
				Change the value of the node to new_val.
				Returns the old value.

ALIST append_alist( first, second )
				ALIST second is appended to ALIST first. The
				ALIST returned is 'first' so that:
				new = append_alist(copy_alist(a), copy_alist(b))
				will work as expected.
				Note: 'second' is destroyed by append_alist!
				Note: append_alist( a, a ) is ignored!

LIST insert_subalist_node( dest, src, node )
				src is inserted into dest before the first
				occurrance of node.  If not present, src is
				appended.

LIST insert_subalist_pos( dest, src, pos )
				src is inserted into dest before the the pos-th
				node.  If pos <= 1 src is pre-pended.  If
				pos > list_length, src is appended.

Function for retrieving and removing nodes from ALISTs:

NODE get_alist_head( alist )	returns (but does not remove) the node at the
				beginning of the alist.

NODE get_alist_tail( alist )	returns (but does not remove) the node at the
				end of the alist.

NODE get_from_ahead( alist )	Removes and returns the node at the beginning
				of the alist.

NODE get_from_atail( alist )	Removes and returns the node at the end of
				the alist.

NODE get_alist_pos( alist,pos )	Returns (but does not remove) the node at 'pos'.
				If pos is out of range, it returns NULL.

NODE get_alist_val(alist, val )	Returns (but does not remove) the first node
				whose value == val.  Otherwise returns NULL;

NODE delete_alist_pos( alist, pos )	get_alist_pos with node removal

NODE delete_alist_val( alist, val )	get_alist_val with node removal

NODE delete_alist_node( alist, node )	Returns and removes 'node' from alist.
					Returns NULL if 'node' was not there.

NODE get_alist_next( alist, node )	Returns the element in the alist
					after 'node'.  If node is NULL, the
					first element is returned.

NODE get_alist_prev( alist, node )	Returns the element in the alist
					before 'node'.  If node is NULL, the
					last element is returned.

Applying C functions to ALISTs:

int for_all_alist ( alist, node, func )
int for_all_2alist( alist, node, node2, func )
int for_all_3alist( alist, node, node2, node3, func )
				Applies (*func)( node, <alist node> ) to all
				of the nodes in the alist.  Returns the sum of
				the values returned by all calls to func.

int alist_equal( alist1, alist2, func )
				Compares the two alists for equality
				based on the function (tests nodes if func = 0)

ALIST eval_alist( alist, func )	Sets the value of all nodes in the alist to
				func( <alist node> ).  The alist is returned.

NODE search_alist( alist, node, func )
				Applies (*func)( node, <alist node> ) to all
				of the nodes in the alist.  Returns the first
				node on which func returns non-zero (i.e. TRUE).

NODE get_alist_next_func( alist, start_node, node, func )
				As search_alist(), but starts at start-node.

ALIST sort_alist( alist )	Sorts alist into ascending order of the value
				of each node.  These can be set with eval_alist
				or insert_alist_val.
				The alist is returned.

ALIST reverse_alist( alist )	Reverses the order of alist.  Also useful if
				you want sort_alist to sort in descending order:
				reverse_alist( sort_alist( alist ) )
				The alist is returned.

ALIST sort_alist_func( alist, func )
				Sorts the alist by applying function to pairs
				of nodes.  The boolean, func( node1, node2 ),
				returns TRUE if the nodes are ordered correctly
				(i.e. node1 precedes node2), FALSE otherwise.

Additions ALIST utility functions:

int alist_empty( alist )		Is the alist empty?

int alist_length( alist )		Returns the number of nodes in the alist.

int in_alist( alist, node )	Returns FALSE (i.e. 0) if the node is not in
				the alist.  Otherwise, the position is returned.

int last_in_alist( alist, node )
int first_in_alist( alist, node )	Boolean functions which indicate whether the
				specified node is first (last) in the alist.

int replace_alist( alist, oldNode, newNode )
				Returns TRUE if it could replace the first
				occurance of oldNode with newNode.

int get_anode_val( alist, node )	Returns the value of <alist node>.  Or 0.

int get_ahead_val( alist )	Returns the value of <alist head>.  Or 0.

int get_atail_val( alist )	Returns the value of <alist tail>.  Or 0.

**************************************************************************/

/*
 * External Definitions and debugging macros:
 */

#include <alistPack.h>			/* Definitions for procedures	*/
					/* and ALIST structures		*/


#ifdef DEBUG				/* When running with DEBUG	*/

#ifndef DO_CHECK			/* make sure that run-time ALIST*/
#define DO_CHECK 3			/* integrity checking is turned */
#endif					/* on				*/

#define TIMES 5				/* size of test alists		*/
#endif


#if DO_CHECK > 0			/* For integrity checking, we	*/
					/* need the standard I/O defs,	*/
typedef char * STRING;			/* our definition of a STRING	*/
#include <stdio.h>			/* and an internal ALIST to keep	*/
					/* track of the ALISTs which are	*/
static	ALIST	alists_in_use = NULL;	/* valid at any given point in	*/
					/* time, otherwise make the	*/
#else					/* debug checker an un-routine!	*/
#define check_alist_integrity( alist, routine )
#endif

/*	Macros to make the code cleaner:		*/

#define CURRENT( alist )		(alist -> current)

#define	ASIZE( alist )	(sizeof(AELEMENT) * ((alist)->max_nodes - 1))

		/* This macro assures uniform traversal through ALISTs */
#define LOOP_THROUGH_ALIST( alist, reg ) \
	    for( reg = ALISThead( alist ); reg <= ALISTtail( alist ); reg++ )


#ifndef NULL
#define NULL 0
#endif

/*	This routine performs serious run-time checking on a ALIST.	*/

#if DO_CHECK > 0
	check_alist_integrity( alist, routine )
ALIST	alist;
STRING	routine;
    {
    register int	temp, save;

    if (alist == NULLalist)
	fprintf( stderr, "'%s' called with NULL alist\n", routine );
    else if (alists_in_use == NULLalist)
	fprintf( stderr, "'%s' called before any alists made\n", routine );

#if DO_CHECK > 1
    else {
	LOOP_THROUGH_ALIST( alists_in_use, temp )
	    if (alist == (ALIST) ALISTnode( alists_in_use, temp)) break;

	if (alist != alists_in_use && temp > ALISTlength( alists_in_use))
	    fprintf( stderr, "'%s' called with unknown alist\n", routine );

#if DO_CHECK > 2
	if (ALISTlength(alist) > alist -> max_nodes || ALISTlength( alist ) < 0)
	    fprintf(stderr, "Length of alist is incorrect in '%s'\n", routine);
#endif
	}
#endif
    }
#endif

/*
 * Internal utility routines (create node, insert before node, delete node)
 */


NODE	_add_alist_node( alist, pos, node, val )	/* Inserts a new node*/
ALIST	alist;						/* into a ALIST.  It  */
NODE	node;						/* then returns a    */
int	val, pos;					/* pointer to the new*/
    {							/* ALIST record.     */
    register int temp;

    if (++(ALISTlength( alist ))  >= alist->max_nodes ) {
	if (alist -> max_nodes == 0)	alist -> max_nodes  = 2;
	else				alist -> max_nodes *= 2;
	alist -> node_array = (AELEMENT *)
			change_node_size( (NODE) alist->node_array,
					  ASIZE( alist ) );
	}

    if (pos > ALISTlength(alist))	pos = ALISTlength( alist );

    /* Move the elements of the array up to make room for the new element */
    for (temp = ALISTlength( alist ) - 1; temp >= pos; temp--)
	alist->node_array[ temp ] = alist->node_array[ temp-1 ];

    ALISTnode( alist, pos )	= node;
    ALISTval( alist, pos )	= val;
    return node;
    }


NODE	_remove_alist_node( pos, alist )		/* Removes a node    */
ALIST	alist;						/* from a ALIST.  It  */
int	pos;						/* then return the   */
    {							/* NODE at the node  */
    register NODE save;					/* which was deleted.*/
    register int  temp, last;

    if (pos < ALISThead( alist ) || pos > ALISTtail( alist)) return NULL;
		
    save = ALISTnode( alist, pos );
    last = --(ALISTlength( alist ));
    for (temp = pos; temp <= last; temp++ ) {
	ALISTnode( alist, temp )  = ALISTnode( alist, temp+1 );
	ALISTval( alist, temp ) = ALISTval( alist, temp+1 );
	}
    return save;
    }

/*
 * Internal utility routines (swap pointers between ALISTs)
 */

#define	swap( a, b ) {				\
	register AELEMENT * c;			\
	c = a -> node_array;			\
	a -> node_array = b -> node_array;	\
	b -> node_array = c;			\
	}

/*
 * List insertion routines:
 * insert_alist_val will add a node in ascending order with respect to
 *		the values already assigned to the nodes in the alist. 
 */


NODE	_insert_alist_val( alist, node, val )
ALIST	alist;
NODE	node;
int	val;
    {
    register int pos;

    check_alist_integrity( alist, "insert_alist_val" );

    if (alist_empty( alist ))
	return _add_alist_node( alist, 1, node, val );

    if (ALISTval( alist, ALISTtail( alist )) < val)
	return add_to_atail_val( alist, node, val );

    LOOP_THROUGH_ALIST( alist, pos )
	if (ALISTval( alist, pos ) >= val) break;

    return _add_alist_node( alist, pos, node, val );
    }

/*
 * List insertion routines:
 * insert_alist_val will add a node in ascending order with respect to
 *		the values already assigned to the nodes in the alist. 
 */


NODE	_insert_alist_node( alist, node, target )
ALIST	alist;
NODE	node, target;
    {
    register int pos;

    check_alist_integrity( alist, "insert_alist_val" );

    if (alist_empty( alist ) || first_in_alist( alist, target ))
	return _add_alist_node( alist, 1, node, DEFAULT_ALIST_VAL );

    LOOP_THROUGH_ALIST( alist, pos )
	if (ALISTnode( alist, pos ) == target) break;

    return _add_alist_node( alist, pos, node, DEFAULT_ALIST_VAL );
    }

/*
 * Random access node retrieval.
 * get_alist_val reveals the first node with the given value
 */



NODE	_get_alist_val( alist, val )
ALIST	alist;
int	val;
    {
    register int temp;

    check_alist_integrity( alist, "get_alist_val" );

    LOOP_THROUGH_ALIST( alist, temp )
	if (ALISTval( alist, temp ) == val) return ALISTnode( alist, temp );

    return NULL;
    }

/* random node removal routines:
 * delete_alist_val removes the entry whose value is 'val' from the alist.
 * delete_alist_node removes the names node from the alist
 *
 * Both return the node to the caller.
 */

NODE	_delete_alist_val( alist, val )
ALIST	alist;
int	val;
    {
    register int temp;

    check_alist_integrity( alist, "delete_alist_val" );

    LOOP_THROUGH_ALIST( alist, temp )
	if (ALISTval( alist, temp ) == val) return _remove_alist_node( temp, alist );
    return NULL;
    }



NODE	_delete_alist_node( alist, node )
ALIST	alist;
NODE	node;
    {
    register int temp;

    check_alist_integrity( alist, "delete_alist_node" );

    LOOP_THROUGH_ALIST( alist, temp )
	if (ALISTnode( alist, temp ) == node) return _remove_alist_node( temp, alist );
    return NULL;
    }

/*
 * alist creation:
 * make_alist (make_queue, make_stack) returns a virgin alist to the caller.
 */

#define	create_alist( temp ) {				\
    temp = (ALIST) make_node( sizeof (ALISTNODE) );	\
    ALISTlength( temp )	= temp -> max_nodes = 0;	\
    temp -> node_array	= (AELEMENT *) 0;		\
    }


ALIST	_make_alist()
    {
    register ALIST temp;

					/* allocate a new alist structure   */
    create_alist( temp );

#if DO_CHECK > 0			/* must maintain the list of alists */
    if (alists_in_use == NULLalist)	/* need to allocate the super-list  */
	create_alist( alists_in_use );
					/* save and return the alist	    */
    return (ALIST) add_to_atail_val( alists_in_use, temp, "make_alist" );
#else

    return temp;

#endif
    }

/*
 * flush_alist: clear a alist for re-use
 * free_alist (free_queue, free_stack) takes a alist and returns its memory
 *	to the garbage collector.
 *
 * ALISTs may not be accessed after calls to free_alist.  If free_alist is called
 * with AND_NODES, the nodes are also returned to the garbage collector, and
 * they, too, may not be accessed further.
 */
	
ALIST	_flush_alist( alist, free_nodes )
ALIST	alist;
int	free_nodes;
    {
    register int temp;

    if (alist == NULLalist)	return make_alist();

    check_alist_integrity( alist, "flush_alist" );

    if (free_nodes)
	LOOP_THROUGH_ALIST( alist, temp ) free_node( ALISTnode( alist, temp ) );

    free_node( (NODE) alist -> node_array );
    alist -> node_array = (AELEMENT *) NULL;
    alist -> max_nodes  = ALISTlength( alist ) = 0;

    return alist;
    }


	
void	_free_alist( alist, free_nodes )
ALIST	alist;
int	free_nodes;
    {
    free_node( (NODE) flush_alist( alist, free_nodes ) );

#if DO_CHECK > 0		/* Remove alist from the alist of alists */
    delete_alist_node( alists_in_use, alist );
#endif
    }

/*
 * Insert an alist into another at a specified position
 */

ALIST	_insert_subalist_pos( alist1, alist2, pos )
ALIST	alist1, alist2;
int	pos;
    {
    register int	from, to, count;

    check_alist_integrity( alist1, "insert_subalist_pos (first argument)" );
    check_alist_integrity( alist2, "insert_subalist_pos (second argument)" );

    if (alist1 == alist2)	return alist1;

    if (!(alist_empty( alist2 ))) {

	alist1 -> max_nodes  = 2 * ((alist1->max_nodes > alist2->max_nodes) ?
				     alist1->max_nodes : alist2->max_nodes);
	alist1 -> node_array = (AELEMENT *)
		    change_node_size( (NODE) alist1 -> node_array,
		    		      ASIZE( alist1 ) );

	/* find out if we've got to do copying to prevent */
	/* the alist from being overwritten */
	if ((count = (1 + ALISTlength( alist1 ) - pos)) > 0 ) {

	    to   = ALISTlength(alist1) + ALISTlength(alist2);
	    from = ALISTlength(alist1);
	    while (count-- > 0)
		alist1->node_array[ --to ] = alist1->node_array[ --from ];

	    to   = ALISTlength(alist2) + ((pos <= 1) ? 0 : (pos - 1));
	    }
	else
	    to   = ALISTlength(alist1) + ALISTlength(alist2);
	
	from = ALISTlength(alist2);
	while (from > 0)
	    alist1->node_array[ --to ] = alist1->node_array[ --from ];
	}

    free_alist( alist2, NOT_NODES );		/* alist2 is now invalid */

    return alist1;				/* return valid ALIST	 */
    }


ALIST	_insert_subalist_node( alist1, alist2, node )
ALIST	alist1, alist2;
NODE	node;
    {
    register int pos;

    return ((pos = in_alist( alist1, node )) > 0) ?
		insert_subalist_pos( alist1, alist2, pos ) :
		append_alist( alist1, alist2 );
    }

/*
 * Append ALISTs together
 */

ALIST	_append_alist( alist1, alist2 )
ALIST	alist1, alist2;
    {
    register int	length1, length2;

    check_alist_integrity( alist1, "append_alist (first argument)" );
    check_alist_integrity( alist2, "append_alist (second argument)" );

    if (alist1 == alist2)	return alist1;

    if (!(alist_empty( alist2 ))) {

	length2 = ALISTlength( alist2 );	/* save lengths and update */
	length1 = ALISTlength( alist1 ) += length2;

					    /* expand the array? */
	if ( length1 >= alist1->max_nodes) {
	    alist1 -> max_nodes  =
			    2 * ((alist1->max_nodes > alist2->max_nodes) ?
				  alist1->max_nodes : alist2->max_nodes);
	    alist1 -> node_array = (AELEMENT *)
			    change_node_size( (NODE) alist1 -> node_array,
							ASIZE( alist1 ) );
	    }

	while (length2 > 0)			/* copy in the new elements */
	    alist1->node_array[ --length1 ] = alist2->node_array[ --length2 ];
	}

    free_alist( alist2, NOT_NODES );		/* alist2 is now invalid */

    return alist1;				/* return valid ALIST	 */
    }

/*
 * List processing.  These routines take (as a parameter) a pointer to a
 *	'C' function which will then be called with each element in the ALIST.
 *	Also, they take one (two or three) pointers which will also be passed
 *	to the 'C' function.
 *
 * for_all_alist applies a function to the nodes of a alist, and returns the
 *	sum of the values returned by that function.
 * for_all_2alist applies a function to the nodes of a alist, and returns the
 *	sum of the values returned by that function.
 */


int	_for_all_alist( alist, node, func )
ALIST	alist;
NODE	node;
PFI	func;			/* pointer to function returning integer */
    {
    register int total = 0, temp;

    check_alist_integrity( alist, "for_all_alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	total += (*func)( node, ALISTnode( alist, temp ) );
    return total;
    }


int	_for_all_2alist( alist, node, node2, func )
ALIST	alist;
NODE	node, node2;
PFI	func;			/* pointer to function returning integer */
    {
    register int total = 0, temp;

    check_alist_integrity( alist, "for_all_2alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	total += (*func)( node, node2, ALISTnode( alist, temp ) );
    return total;
    }

/*
 * List processing.  These routines take (as a parameter) a pointer to a
 * 'C' function which will then be called with each element in the ALIST.
 *
 * for_all_3alist applies a function to the nodes of a alist, and returns the
 *	sum of the values returned by that function.
 */


int	_for_all_3alist( alist, node, node2, node3, func )
ALIST	alist;
NODE	node, node2, node3;
PFI	func;			/* pointer to function returning integer */
    {
    register int total = 0, temp;

    check_alist_integrity( alist, "for_all_3alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	total += (*func)( node, node2, node3, ALISTnode( alist, temp ) );
    return total;
    }

/*
 * search_alist applies a function to each node until the function returns
 *	TRUE.  It then returns a pointer to that node.
 */


NODE	_search_alist( alist, node, func )
ALIST	alist;
NODE	node;
PFI	func;			/* pointer to function returning integer */
    {
    register int temp;

    check_alist_integrity( alist, "search_alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	if ((*func)( node, ALISTnode( alist, temp ) )) return ALISTnode( alist, temp );

    return NULL;
    }


int	_alist_equal( alist1, alist2, func )
ALIST	alist1, alist2;
PFI	func;
    {
    register int pos1, pos2;

    check_alist_integrity( alist1, "alist_equal" );
    check_alist_integrity( alist2, "alist_equal" );

    if (ALISTlength(alist1) != ALISTlength(alist2))	return 0;

    pos2 = ALISThead(alist2);
    LOOP_THROUGH_ALIST( alist1, pos1 )
	if (ALISTnode( alist1, pos1 ) != ALISTnode( alist2, pos2 ) &&
	    ((func == ((PFI) 0)) ||
	     ! ((*func)( ALISTnode(alist1,pos1), ALISTnode(alist2,pos2) ))))
		return 0;

    return 1;
    }

/*
 * Set all of the value fields within a ALIST.  This is used for sorting ALISTs
 * and for saving time on long evaluation functions.
 * eval_alist sets all of the values of the nodes of a alist to a function
 *	of the node.
 */


ALIST	_eval_alist( alist, func )
ALIST	alist;
PFI	func;			/* pointer to function returning integer */
    {
    register int temp;

    check_alist_integrity( alist, "eval_alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	ALISTval( alist, temp ) = (*func)( ALISTnode( alist, temp ) );

    return alist;
    }

int	_change_alist_val( alist, node, new_val )
ALIST	alist;
NODE	node;
int	new_val;
    {
    register int temp, save;

    check_alist_integrity( alist, "eval_alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	if (ALISTnode( alist, temp ) == node ) {
	    save = ALISTval( alist, temp );
	    ALISTval( alist, temp ) = new_val;
	    return save;
	    }

    return DEFAULT_ALIST_VAL;
    }

/*
 * Sort and reverse ALISTs:
 * sort_alist places the nodes into ascending order of 'value'
 *	(value gets set by either insert_alist_val or eval_alist).
 * sort_alist_func sorts the alist by applying the predicate function
 * reverse_alist is intuitively obvious to the casual obvious.
 */

ALIST	_sort_alist_func( alist, func )
ALIST	alist;
PFI	func;
    {
    register int	ptr1, ptr2;
    register ALIST	temp;

    check_alist_integrity( alist, "sort_alist_func" );

    if (ALISTlength( alist ) <= 1)	return alist;

    temp = make_alist();
    LOOP_THROUGH_ALIST( alist, ptr1 ) {
	LOOP_THROUGH_ALIST( temp, ptr2 )
	    if ((*func)( ALISTnode( alist, ptr1 ), ALISTnode( temp, ptr2 ))) break;

	_add_alist_node( temp, ptr2, ALISTnode( alist, ptr1 ), ALISTval( alist, ptr1 ) );
	}

    swap( temp, alist );
    free_alist( temp, NOT_NODES );

    return alist;
    }

ALIST	_sort_alist( alist )
ALIST	alist;
    {
    register ALIST temp;
    register int   ptr;

    check_alist_integrity( alist, "sort_alist" );

    if (ALISTlength( alist ) <= 1)	return alist;

    temp = make_alist();
    LOOP_THROUGH_ALIST( alist, ptr )
	insert_alist_val( temp, ALISTnode( alist, ptr ), ALISTval( alist, ptr ) );
    swap( temp, alist );
    free_alist( temp, NOT_NODES );

    return alist;
    }


ALIST	_reverse_alist( alist )
ALIST	alist;
    {
    register AELEMENT *	temp;
    register int	p1, p2;

    check_alist_integrity( alist, "reverse_alist" );

    if (ALISTlength( alist ) <= 1)	return alist;		/* why bother? */

    temp = (AELEMENT *) make_node( ASIZE( alist ) );
    p2   = ALISTlength( alist );
    LOOP_THROUGH_ALIST( alist, p1 ) {
        temp[ --p2 ].node  = ALISTnode(alist,p1);
	temp[   p2 ].value = ALISTval(alist,p1);
	}

    free_node( (NODE) alist -> node_array );
    alist -> node_array = temp;

    return alist;
    }

/*
 * Search and replace alist elements.
 * replace_alist looks the first occurrance of a node and replaces it with
 *	another node.
 * copy_alist will also copy the nodes if the flag is set.
 */


ALIST	_copy_alist( alist, flag )
ALIST	alist;
int	flag;
    {
    register ALIST temp;
    register int   ptr;

    check_alist_integrity( alist, "copy_alist" );

    temp		= (ALIST)	copy_node( (NODE) alist );
    temp -> node_array	= (AELEMENT *)	copy_node( (NODE) alist ->
    						   node_array );
    if (flag)
	LOOP_THROUGH_ALIST( temp, ptr )
	    ALISTnode( temp, ptr ) = copy_node( ALISTnode( temp, ptr ) );

#if DO_CHECK > 0			/* must maintain the list of alists */
    return (ALIST) add_to_atail_val( alists_in_use, temp, "copy_alist" );
#else
    return temp;
#endif
    }



int	_replace_alist( alist, old, new )
ALIST	alist;
NODE	old, new;
    {
    register int temp;

    check_alist_integrity( alist, "replace_alist" );

    LOOP_THROUGH_ALIST( alist, temp )	if (old == ALISTnode( alist, temp ) ) break;

    if (temp >= ALISThead(alist) && temp <= ALISTlength(alist))	ALISTnode(alist, temp) = new;

    return (temp >= ALISThead(alist) && temp <= ALISTlength(alist));
    }

/*
 * Probe ALISTs:
 * in_alist return the position of a node in the alist (0 if it is not there).
 * get_anode_val returns the value of a node (0 if the node is not in the alist).
 */


int	_in_alist( alist, node )
ALIST	alist;
NODE	node;
    {
    register int  count;

    check_alist_integrity( alist, "in_alist" );

    LOOP_THROUGH_ALIST( alist, count )
	if (node == ALISTnode( alist, count) ) return count;

    return 0;
    }


int	_get_anode_val( alist, node )
ALIST	alist;
NODE	node;
    {
    register int temp;

    check_alist_integrity( alist, "get_anode_val" );

    LOOP_THROUGH_ALIST( alist, temp )
	if (ALISTnode( alist, temp ) == node) return ALISTval( alist, temp );

    return DEFAULT_ALIST_VAL;
    }

/*
 *	Relative postion functions
 */

NODE	_get_alist_next( alist, current )
ALIST	alist;
NODE	current;
    {
    register int temp;

    check_alist_integrity( alist, "get_alist_next" );

    if (current == NULL)
	ALISTcur( alist ) = ALISThead( alist );
    else if (ALISTnode(alist, ALISTcur( alist )) == current)
	ALISTcur( alist )++;
    else
	LOOP_THROUGH_ALIST( alist, temp )
	    if (ALISTnode( alist, temp ) == current) {
		ALISTcur( alist ) = temp + 1;
		break;
		}

    return (ALISTvalid(alist, ALISTcur(alist))) ? ALISTnode(alist, ALISTcur(alist)) : NULL;
    }


NODE	_get_alist_prev( alist, current )
ALIST	alist;
NODE	current;
    {
    register int temp;

    check_alist_integrity( alist, "get_alist_prev" );

    if (current == NULL)
	ALISTcur( alist ) = ALISTtail( alist ) - 1;
    else if (ALISTnode( alist, ALISTcur( alist ) ) == current)
	ALISTcur( alist )--;
    else
	LOOP_THROUGH_ALIST( alist, temp )
	    if (ALISTnode( alist, temp ) == current) {
		ALISTcur( alist ) = temp - 1;
		break;
		}

    return (ALISTvalid(alist, ALISTcur(alist))) ? ALISTnode(alist, ALISTcur(alist)) : NULL;
    }

NODE	_get_alist_next_func( alist, current, node, func )
ALIST	alist;
NODE	current, node;
PFI	func;
    {
    register int temp;

    check_alist_integrity( alist, "get_alist_next_func" );

    if (current == NULL)
	temp = ALISThead( alist );
    else if (ALISTnode( alist, ALISTcur( alist ) ) == current)
	temp = ALISTcur( alist );
    else
	LOOP_THROUGH_ALIST( alist, temp )
	    if (ALISTnode( alist, temp ) == current) break;

    while (temp <= ALISTlength( alist ))
	if ( (*func)(node, ALISTnode( alist, temp )) ) break;
    
    ALISTcur( alist ) = temp;
    return (ALISTvalid(alist, ALISTcur(alist))) ? ALISTnode(alist, ALISTcur(alist)) : NULL;
    }

/*
 * Run debugging function on the alist package.  This section is not compiled
 * unless debugging is desired.
 */

#if DEBUG

debug_print_alist( alist )
ALIST alist;
    {
    register int temp;

    printf("\nPrinting alist 0x%x, length = %d:\n", alist, ALISTlength(alist));

    check_alist_integrity( alist, "debug_print_alist" );

    LOOP_THROUGH_ALIST( alist, temp )
	printf( "\t(%d)\tNode %d, value %d\n",
		    temp, ALISTnode( alist, temp ), ALISTval( alist, temp) );
    printf ("Done\n");
    }

main() {
    register int i;
    register ALIST alist, alist2, alist3;

    printf( "\ntesting make_alist()\n" );
    alist = make_alist();

    printf( "\ntesting debug_print_alist()\n" );
    debug_print_alist( alist );

    printf( "\ntesting add_to_ahead()\n" );
    for (i=0; i<TIMES; i++) {
	add_to_ahead( alist, i );
	debug_print_alist( alist );
	}
    free_alist( alist, NOT_NODES );

    alist = make_alist();
    printf( "\ntesting add_to_atail()\n" );
    for (i=0; i<TIMES; i++) {
	add_to_atail( alist, i );
	debug_print_alist( alist );
	}
    free_alist( alist, NOT_NODES );

    alist = make_alist();
    printf( "\ntesting insert_alist_val()\n" );
    for (i=0; i<TIMES; i++) {
	insert_alist_val( alist, i, rand() );
	debug_print_alist( alist );
	}

    alist2 = make_alist();
    printf( "\ntesting insert_alist_pos()\n" );
    for (i=0; i<TIMES; i++) {
	insert_alist_pos( alist2, i, rand() );
	debug_print_alist( alist2 );
	}

    printf( "\ntesting append_alist() and copy_alist()\n" );

    alist3 =  append_alist( make_alist(), make_alist() );
    debug_print_alist( alist3 );
    free_alist( alist3, NOT_NODES );

    alist3 =  append_alist( make_alist(), copy_alist(alist, NOT_NODES ) );
    debug_print_alist( alist3 );
    free_alist( alist3, NOT_NODES );

    debug_print_alist( append_alist( alist, 	make_alist() ) );
    debug_print_alist( append_alist( alist, copy_alist( alist2, NOT_NODES ) ) );
    debug_print_alist( append_alist( alist2, alist ) );

    free_alist( alist2, NOT_NODES );

    memory_stats( stdout, "alistPack test program" );
    }
#endif
