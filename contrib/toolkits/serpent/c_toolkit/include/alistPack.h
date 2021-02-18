/****************************************************************************

	C Programmer's Toolbox

	Module Name:	alistPack

	Purpose:	Provides the data structures necessary to
			support array lists

	Author:		Robert A Zimmermann, Carnegie-Mellon University
			Department of Electrical and Computer Engineering
	
	History:
	15 Aug 85	Created module from out of listPack

	13 Dec 85	Second version, as per listPack

	27 Jun 86	Updated to conform to the manual

	15 Jul 86	Retro-fitted loop_through_alist bug fix

	08 Dec 86	Added loop_through_alist_func

	04 May 87	Fixed insert_list_node() macro
			Added additional typecasts

*************************************************************************/

#ifndef	 NULLalist			/* check for previous inclusion	*/


#ifndef memoryPack 
#define memoryPack              /* use memoryPack rather than mallocPack    */
#endif

#include "memoryPack.h"			/* Memory Management routines */

#define	DEFAULT_ALIST_VAL	0	/* default node tag value */

#ifndef AND_NODES
#define AND_NODES		1	/* flags for free_alist and	*/
#define NOT_NODES		0	/* copy_alist			*/
typedef int (*PFI)();			/* pointer to func returning int */
#endif


/*	Declaration of type ALIST.  (made public for debugging)		*/

typedef struct {
	NODE		node;
	int		value;
	} AELEMENT;

typedef struct {
	AELEMENT *	node_array;	/* pointer to the array of elements */
	int		length,		/* number of elements currently */
			current,	/* the index of the current element */
			max_nodes;	/* size of the node_array */
	} ALISTNODE, *ALIST, *ASTACK, *AQUEUE;


#define	NULLalist	((ALIST) 0)

/************************************************************************
 *	These macros expand to replace functions which can be done	*
 *	more efficiently in-line.					*
 ************************************************************************/

#define ALISTindex(alist, pos)	((ALIST) (alist)) -> node_array[((int) (pos))-1]
#define ALISTnode(alist, pos)	(ALISTindex( alist, pos ).node)
#define ALISTval(alist, pos)	(ALISTindex( alist, pos ).value)
#define	ALISThead(alist)	1
#define	ALISTlength(alist)	(((ALIST) (alist)) -> length)
#define ALISTcur(alist)		(((ALIST) (alist)) -> current)
#define	ALISTtail(alist)	ALISTlength( alist )

#define	alist_length( alist )		ALISTlength( alist )

#define alist_empty( alist )		(alist_length( alist ) == 0)

#define ALISTvalid(alist, pos)	\
	((pos) >= ALISThead(alist) && (pos) <= ALISTtail(alist))

#define get_alist_tail( alist )	\
	(alist_empty( alist ) ? (NODE) 0 : ALISTnode(alist, ALISTtail(alist)))

#define get_atail_val( alist )	\
	(alist_empty( alist ) ? (NODE) 0 : ALISTval(alist, ALISTtail(alist)))

#define get_from_atail( alist )	\
	(_remove_alist_node( ALISTtail( alist ), (ALIST) (alist) ) )

#define add_to_atail_val( alist, node, val )	\
	(_add_alist_node( (ALIST) (alist), 1 + (ALISTtail( alist )), (NODE) (node), (int) (val) ))

#define add_to_atail( alist, node ) \
	(add_to_atail_val( alist, node, DEFAULT_ALIST_VAL ))

#define get_alist_head( alist )	\
	(alist_empty( alist ) ? (NODE) 0 : ALISTnode(alist, ALISThead(alist)))

#define get_ahead_val( alist )	\
	(alist_empty( alist ) ? (NODE) 0 : ALISTval(alist, ALISThead(alist)))

#define get_from_ahead( alist )	\
	(_remove_alist_node( ALISThead(alist), (ALIST) (alist) ))

#define add_to_ahead_val( alist, node, val )	\
	(_add_alist_node( (ALIST) (alist), ALISThead( alist ), (NODE) (node), (int) (val) ))

#define add_to_ahead( alist, node ) \
	(add_to_ahead_val( alist, node, DEFAULT_ALIST_VAL ))

#define get_alist_pos( alist, pos ) \
	(ALISTvalid( alist, pos ) ? ALISTnode( alist, pos ) : (NODE) 0 )

#define get_alist_pos_val( alist, pos ) \
	(ALISTvalid( alist, pos ) ? ALISTval( alist, pos ) : DEFAULT_ALIST_VAL )

#define delete_alist_pos( alist, pos )	(_remove_alist_node( (int) (pos), (ALIST) (alist) ))

#define insert_alist_pos( alist, node, pos )	\
	    (_add_alist_node( (ALIST) (alist), (int) (pos), (NODE) (node), DEFAULT_ALIST_VAL))

#define first_in_alist( alist, node )	\
		((NODE) get_alist_head( alist ) == (NODE) node)

#define last_in_alist( alist, node )	\
		((NODE) get_alist_tail( alist ) == (NODE) node)

/****************************************************************
 *	These macros construct a for() loop for C programs	*
 ****************************************************************/

#define	__get_anode( lptr, type )			\
	    ((ALISTvalid( lptr, ALISTcur(lptr) ) ) ?	\
		    (type) ALISTnode( (lptr), ALISTcur(lptr) ) : (type) 0)

#define loop_through_alist_func( alist, ptr, type, func )			\
	for ( ALISTcur(alist) = ALISThead(alist);				\
	     (ptr = __get_anode((alist), type)) != (type) 0;			\
	      ALISTcur(alist) = (ptr == __get_anode( alist, type )) ?		\
				    1 + ALISTcur(alist) :			\
				    (int) func( alist, (NODE) ptr ) )

#define loop_through_alist( alist, ptr, type )				\
		loop_through_alist_func( alist, ptr, type, _get_alist_next )

/************************************************************************
 *	External Routine Declarations.  Macros will redefine the	*
 *	calling sequences of all non-macro routines			*
 ************************************************************************/

extern	ALIST	_make_alist(), _eval_alist(), _reverse_alist(),
		_insert_subalist_node(), _insert_subalist_pos(),
		_sort_alist(), _copy_alist(), _append_alist(), _flush_alist();

extern	NODE	_get_alist_val(), _delete_alist_val(), _insert_alist_node(),
		_search_alist(), _insert_alist_val(), _delete_alist_node(),
		_get_alist_next(), _get_alist_prev(), _get_alist_next_func();

extern	int	_get_anode_val(), _for_all_alist (), _for_all_2alist(),
		_change_alist_value(), _alist_equal(),
		_for_all_3alist(), _in_alist(), _replace_alist();

extern	void	_free_alist();

/************************************************************************
 *		Macros for automatic type recasting.			*
 ************************************************************************/

#define	free_alist( alist, flag )	\
	    _free_alist( (ALIST) (alist), (int) (flag) )
#define	make_alist()			\
	    _make_alist()
#define	flush_alist( alist, flag )	\
	    _flush_alist((ALIST) (alist), (int) (flag) )
#define	reverse_alist( alist )		\
	    _reverse_alist( (ALIST) (alist) )
#define	sort_alist( alist )		\
	    _sort_alist( (ALIST) (alist) )
#define	sort_alist_func( alist, func )	\
	    _sort_alist_func((ALIST) (alist), (PFI) (func))
#define	copy_alist( alist, flag )	\
	    _copy_alist( (ALIST) (alist), (int) (flag) )
#define	insert_subalist_pos( alist1, alist2, pos )	\
	    _insert_subalist_pos((ALIST)(alist1),(ALIST)(alist2),(int)(pos))
#define	insert_subalist_node( alist1, alist2, node )	\
	    _insert_subalist_node((ALIST)(alist1),(ALIST)(alist2),(NODE)(node))
#define	in_alist( alist, node )		\
	    _in_alist((ALIST) (alist), (NODE) (node))
#define	append_alist( alist1, alist2 )	\
	    _append_alist((ALIST) (alist1), (ALIST) (alist2))
#define	in_alist( alist, node )		\
	    _in_alist((ALIST) (alist), (NODE) (node))
#define	get_alist_val( alist, val )	\
	    _get_alist_val((ALIST) (alist), (int) (val))
#define	delete_alist_val( alist, val )	\
	    _delete_alist_val((ALIST) (alist), (int) (val))
#define get_alist_prev( alist, curr )	\
	    _get_alist_prev((ALIST) (alist), (NODE) (curr))
#define get_alist_next( alist, curr )	\
	    _get_alist_next((ALIST) (alist), (NODE) (curr))
#define get_alist_next_func( alist, curr, node, func ) \
	    _get_alist_next_func((ALIST)(alist),(NODE)(curr),(NODE)(node),(PFI)(func))
#define	get_anode_val( alist, node ) \
	    _get_anode_val((ALIST) (alist), (NODE) (node))
#define	delete_alist_node( alist, node ) \
	    _delete_alist_node((ALIST) (alist), (NODE) (node))
#define	change_alist_val( alist, node, val ) \
	    _change_alist_val( (ALIST) (alist), (NODE) (node), (int) (val) )
#define	insert_alist_node( alist, node, target ) \
	    _insert_alist_node( (ALIST) (alist), (NODE) (node), (NODE) (target) )
#define	insert_alist_val( alist, node, val ) \
	    _insert_alist_val( (ALIST) (alist), (NODE) (node), (int) (val) )
#define	replace_alist( alist, old, new ) \
	    _replace_alist( (ALIST) (alist), (NODE) (old), (NODE) (new) )
#define	eval_alist( alist, func )		\
	    _eval_alist( (ALIST) (alist), (PFI) (func) )
#define	alist_equal( alist, alist2, func )	\
	    _alist_equal( (ALIST) (alist), (ALIST) (alist2), (PFI) (func) )
#define	search_alist( alist, node, func )	\
	    _search_alist( (ALIST) (alist), (NODE) (node), (PFI) (func) )
#define	for_all_alist( alist, node, func )	\
	    _for_all_alist( (ALIST) (alist), (NODE) (node), (PFI) (func) )
#define	for_all_2alist( alist, node, node2, func  )	\
	    _for_all_2alist((ALIST)(alist),(NODE)(node),(NODE)(node2),(PFI)(func))
#define	for_all_3alist( alist, node, node2, node3, func  )	\
	    _for_all_3alist((ALIST)(alist),(NODE)(node),(NODE)(node2),(NODE)(node3),(PFI)(func))

/*
 * ASTACK routines
 */

#define make_astack()			(ASTACK) make_alist()

#define free_astack( astack, flag )	free_alist( astack, flag )

#define push_astack( astack, node )	add_to_atail( astack, node )

#define pop_astack( astack )		get_from_atail( astack )

#define top_of_astack( astack )		get_alist_tail( astack )

#define height_of_astack( astack )	alist_length( astack )

#define astack_empty( astack )		(height_of_astack( astack ) == 0)

/*
 * AQUEUE routines
 */

#define make_aqueue()			(AQUEUE) make_alist()

#define free_aqueue( aqueue, flag )	free_alist( aqueue, flag )

#define enaqueue( aqueue, node )	add_to_atail( aqueue, node )

#define deaqueue( aqueue )		get_from_ahead( aqueue )

#define head_of_aqueue( aqueue )	get_alist_head( aqueue )

#define length_of_aqueue( aqueue )	alist_length( aqueue )

#define aqueue_empty( aqueue )		(length_of_aqueue( aqueue ) == 0)

#endif
