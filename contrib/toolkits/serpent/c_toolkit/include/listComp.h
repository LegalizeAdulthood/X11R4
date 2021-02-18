/****************************************************************************

	C Programmer's Toolbox

	Module Name:	listComp

	Purpose:	Maps the functions of listPack onto alistPack
			and vice-versa.  This can be for debugging, but
			more likely to see which will make your code
			run faster.

	Use:		either  #define LIST_TO_ALIST
				#include <listComp.h>
				#include <alistPack.h>

			or	#define ALIST_TO_LIST
				#include <listComp.h>
				#include <listPack.h>
			
	Author:		Robert A Zimmermann, Carnegie-Mellon University
			Department of Electrical and Computer Engineering
	
	History:
	15 Aug 85	Created

*************************************************************************/

#ifndef DID_LIST_COMPATABILITY		/* prevent multiple inclusions */
#define DID_LIST_COMPATABILITY

#ifdef ALIST_TO_LIST

#define	ALIST	LIST
#define ASTACK	STACK
#define	AQUEUE	QUEUE

#define alist_empty( list )		list_empty( list )
#define first_in_alist( list )		first_in_list( list )
#define last_in_alist( list )		last_in_list( list )
#define	loop_through_alist( a, b, c )	loop_through_list( a, b, c )

#define	make_alist()			make_list()
#define	free_alist( list, flag )		free_list(  list,  flag )
#define	flush_alist( list, flag )	flush_list(  list,  flag )
#define	add_to_ahead( list, node )	add_to_head(  list,  node )
#define	add_to_atail( list, node )	add_to_tail(  list,  node )
#define	get_ahead_val( list )		get_head_val(  list )
#define	get_atail_val( list )		get_tail_val(  list )
#define	get_anode_value( list, node )	get_node_value(  list,  node )
#define	get_alist_head(	list )		get_list_head(  list )
#define	get_alist_tail(	list )		get_list_tail(  list )
#define	get_from_ahead(	list )		get_from_head(  list )
#define	get_from_atail(	list )		get_from_tail(  list )
#define	alist_length(	list )		list_length(  list )
#define	reverse_alist( list )		reverse_list(  list )
#define	sort_alist( list )		sort_list(  list )
#define	sort_alist_func( list, func )	sort_list_func( list,  func )
#define	copy_alist( list, flag )		copy_list(  list,  flag )
#define	append_alist( list1, list2 )	append_list( list1,  list2)
#define	in_alist( list, node )		in_list(  list,  node )
#define	get_alist_pos( list, pos )	get_list_pos(  list,  pos )
#define	get_alist_val( list, val )	get_list_val(  list,  val )
#define	delete_alist_pos( list, pos )	delete_list_pos( list,  pos)
#define	delete_alist_val( list, val )	delete_list_val( list,  val)
#define	delete_alist_node( list, node )	delete_list_node( list,  node)

#define	insert_alist_pos( list, node, pos)  insert_list_pos( list, node,  pos )
#define	insert_alist_val( list, node, val)  insert_list_val( list, node,  val )
#define	replace_alist( list, old, new )     replace_list(  list,  old,  new )

#define get_alist_next( list, curr )	get_list_next( list, curr )
#define get_alist_prev( list, curr )	get_list_prev( list, curr )
#define get_alist_next_func( list, curr, node, func ) \
	    get_list_next_func( list, curr, node, func )

#define	eval_alist( list, func )		eval_list( list,  func )
#define	search_alist( list, node, func )	search_list(  list,  node,  func )
#define	for_all_alist( list, node, func )	\
	    for_all_list(  list,  node,  func )
#define	for_all_2alist( list, node, node2, func  )	\
	    for_all_2list(  list,  node,  node2,  func )
#define	for_all_3alist( list, node, node2, node3, func  )	\
	    for_all_3list(  list,  node,  node2,  node3,  func )

#define make_astack()			(STACK) make_list()
#define free_astack( stack, flag )	free_list( stack, flag )
#define push_astack( stack, node )	add_to_tail( stack, node )
#define pop_astack( stack )		get_from_tail( stack )
#define top_of_astack( stack )		get_list_tail( stack )
#define height_of_astack( stack )	list_length( stack )
#define astack_empty( stack )		(height_of_stack( stack ) == 0)

#define make_aqueue()			(QUEUE) make_list()
#define free_aqueue( queue, flag )	free_list( queue, flag )
#define enaqueue( queue, node )		add_to_tail( queue, node )
#define deaqueue( queue )		get_from_head( queue )
#define head_of_aqueue( queue )		get_list_head( queue )
#define length_of_aqueue( queue )	list_length( queue )
#define aqueue_empty( queue )		(length_of_queue( queue ) == 0)
#endif

#ifdef LIST_TO_ALIST

#define	LIST	ALIST
#define STACK	ASTACK
#define	QUEUE	AQUEUE

#define list_empty( list )		alist_empty( list )
#define first_in_list( list )		first_in_alist( list )
#define last_in_list( list )		last_in_alist( list )
#define	loop_through_list( a, b, c )	loop_through_alist( a, b, c )

#define	make_list()			make_alist()
#define	free_list( list, flag )		free_alist(  list,  flag )
#define	flush_list( list, flag )	flush_alist(  list,  flag )
#define	add_to_head( list, node )	add_to_ahead(  list,  node )
#define	add_to_tail( list, node )	add_to_atail(  list,  node )
#define	get_head_val( list )		get_ahead_val(  list )
#define	get_tail_val( list )		get_atail_val(  list )
#define	get_node_value( list, node )	get_anode_value(  list,  node )
#define	get_list_head(	list )		get_alist_head(  list )
#define	get_list_tail(	list )		get_alist_tail(  list )
#define	get_from_head(	list )		get_from_ahead(  list )
#define	get_from_tail(	list )		get_from_atail(  list )
#define	list_length(	list )		alist_length(  list )
#define	reverse_list( list )		reverse_alist(  list )
#define	sort_list( list )		sort_alist(  list )
#define	sort_list_func( list, func )	sort_alist_func( list,  func )
#define	copy_list( list, flag )		copy_alist(  list,  flag )
#define	append_list( list1, list2 )	append_alist( list1,  list2)
#define	in_list( list, node )		in_alist(  list,  node )
#define	get_list_pos( list, pos )	get_alist_pos(  list,  pos )
#define	get_list_val( list, val )	get_alist_val(  list,  val )
#define	delete_list_pos( list, pos )	delete_alist_pos( list,  pos)
#define	delete_list_val( list, val )	delete_alist_val( list,  val)
#define	delete_list_node( list, node )	delete_alist_node( list,  node)

#define	insert_list_pos( list, node, pos)  insert_alist_pos( list, node,  pos )
#define	insert_list_val( list, node, val)  insert_alist_val( list, node,  val )
#define	replace_list( list, old, new )     replace_alist(  list,  old,  new )

#define get_list_next( list, curr )	get_alist_next( list, curr )
#define get_list_prev( list, curr )	get_alist_prev( list, curr )
#define get_list_next_func( list, curr, node, func ) \
	    get_alist_next_func( list, curr, node, func )

#define	eval_list( list, func )		eval_alist( list,  func )
#define	search_list( list, node, func )	search_alist(  list,  node,  func )
#define	for_all_list( list, node, func )	\
	    for_all_alist(  list,  node,  func )
#define	for_all_2list( list, node, node2, func  )	\
	    for_all_2alist(  list,  node,  node2,  func )
#define	for_all_3list( list, node, node2, node3, func  )	\
	    for_all_3alist(  list,  node,  node2,  node3,  func )

#define make_stack()			(STACK) make_alist()
#define free_stack( stack, flag )	free_alist( stack, flag )
#define push_stack( stack, node )	add_to_tail( stack, node )
#define pop_stack( stack )		get_from_tail( stack )
#define top_of_stack( stack )		get_alist_tail( stack )
#define height_of_stack( stack )	alist_length( stack )
#define stack_empty( stack )		(height_of_astack( stack ) == 0)

#define make_queue()			(QUEUE) make_alist()
#define free_queue( queue, flag )	free_alist( queue, flag )
#define enqueue( queue, node )		add_to_tail( queue, node )
#define dequeue( queue )		get_from_head( queue )
#define head_of_queue( queue )		get_alist_head( queue )
#define length_of_queue( queue )	list_length( queue )
#define queue_empty( queue )		(length_of_aqueue( queue ) == 0)
#endif

#endif
