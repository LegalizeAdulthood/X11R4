/*
 *	Module Name:		treePack
 *
 *	Purpose:		Toolbox module for binary Trees
 *
 *	Contents:
 *	(TREE)	make_bintree( func )
 *			returns a new, empty tree, with the given node
 *			comparison function
 *
 *	(TREE)	copy_bintree( tree, flag )
 *			returns a copy of the tree, and copies the nodes
 *			if flag == AND_NODES
 *
 *	(void)	free_bintree( tree, flag )
 *			frees the tree and all associated storage.
 *			Will also free the nodes if flag == AND_NODES.
 *
 *	(NODE)	add_to_bintree( tree, ident, node )
 *			adds the entry to the tree by applying the
 *			comparison function to the node
 *			returns the node.
 *
 *	(NODE)	add_unique_to_bintree( tree, ident, node )
 *			adds the entry to the tree (by applying the comparison
 *			function to the node) if it is not in the tree.
 *			returns the node if it is not in the tree, otherwise
 *			NULL.
 *
 *	(NODE)	get_from_bintree( tree, ident )
 *			returns the node associated with ident
 *
 *	(NODE)	delete_from_bintree( tree, ident )
 *			as get_from_bintree(), but removes the node as well
 *
 *	(int)	for_all_bintree( tree, node, func )
 *	(int)	for_all_2bintree( tree, node, node2, func )
 *	(int)	for_all_3bintree( tree, node, node2, node3, func )
 *			call func( <nodes>, tree_nodes ) for every node in the
 *			tree
 *
 *	History:
 *	22Apr86	raz	Created
 *	26Jun86	raz	Added copy_bintree(),
 *			Fixed delete_bintree_node().
 *	07May87	raz	Made match_func compilable on other cc's
 */

#include "treePack.h"			/* get our definitions */

#define	match_func( tree, node1, node2 )	\
	(* TREE_FUNC( tree ))( node1, node2 )

/*
 * Debug support
 */

#ifdef DEBUG				/* If debugging, turn on max checking */
#define DO_CHECK 3
#endif

#ifndef DO_CHECK
#define DO_CHECK 0			/* default error checking */
#endif

#if DO_CHECK > 0

#include <stdio.h>

#include "listPack.h"

static	LIST	bintrees_in_use = NULLlist;

static	check_tree_integrity( tree, routine )
TREE	tree;
char	*routine;
    {
    register int i;

    if (bintrees_in_use == NULLlist)
	fprintf(stderr, "'*s' called before bintree was created\n", routine);
    else if (!in_list( bintrees_in_use, tree ))
	fprintf(stderr, "'%s' called with unknown bintree\n", routine);
#if DO_CHECK > 1
				    /* add major checking here */
#endif
    }

#else

#define	check_tree_integrity( tree, routine )

#endif

#ifndef NULL
#define NULL	((char *) 0)
#endif

TREE	_make_bintree( treefunc )
NODE	treefunc;
    {
    register TREE	temp;

    temp = (TREE) make_node( sizeof( TREE_struct ) );

    TREE_ROOT( temp )	= NULLtree;
    TREE_LENGTH( temp )	= 0;
    TREE_NODE( temp )	= (NODE) treefunc;	/* store the func here */

#if DO_CHECK > 1
    if (bintrees_in_use == NULLlist)
	bintrees_in_use = make_list();
    
    return (TREE) add_to_tail( bintrees_in_use, temp );
#else

    return temp;
#endif
    }

TREE	_copy_bintree( tree, flag )
TREE	tree;
int	flag;
    {
    register TREE copy;

    check_tree_integrity( tree, "copy_bintree" );

    if (tree == NULLtree) return NULLtree;

    copy = (TREE) copy_node( tree );

    copy_subtree( &(TREE_ROOT( copy )), flag );

#if DO_CHECK > 1
    if (bintrees_in_use == NULLlist)
	bintrees_in_use = make_list();
    
    return (TREE) add_to_tail( bintrees_in_use, copy );
#else
    return copy;
#endif
    }


static	copy_subtree( root, flag )
TREE	*root;
int	flag;
    {
    if (*root == NULLtree)	return;

    *root = (TREE) copy_node( *root );
    if (flag == AND_NODES)
	TREE_NODE( *root ) = copy_node( TREE_NODE( *root ) );
    
    copy_subtree( &( TREE_LEFT ( *root )), flag );
    copy_subtree( &( TREE_RIGHT( *root )), flag );
    }

static	_flush_bintree( tree, flag )
TREE	tree;
int	flag;
    {
    if (tree == NULLtree) return;

    if ( TREE_LEFT( tree )  != NULLtree ) {
	_flush_bintree( TREE_LEFT( tree ), flag );
	if (flag) free_node( TREE_NODE( TREE_LEFT( tree ) ) );
	free_node( TREE_LEFT( tree ) );
	}

    if ( TREE_RIGHT( tree )  != NULLtree ) {
	_flush_bintree( TREE_RIGHT( tree ), flag );
	if (flag) free_node( TREE_NODE( TREE_RIGHT( tree ) ) );
	free_node( TREE_RIGHT( tree ) );
	}

    TREE_LENGTH( tree ) = 0;
    }

void	_free_bintree( tree, flag )
TREE	tree;
int	flag;
    {
    check_tree_integrity( tree, "free_bintree" );

#if DO_CHECK > 1
    delete_list_node( bintrees_in_use, tree );
#endif

    _flush_bintree( tree, flag );
    free_node( tree );
    }

TREE	*_find_bintree_point( tree, root, ident, flag )
TREE	tree, *root;
NODE	ident;
int	flag;
    {
    register int	save;
    register TREE	temp;

    if (*root == NULLtree)
	return root;
    
    if ((save = match_func( tree, ident, TREE_NODE( *root ))) > 0)
	return _find_bintree_point( tree, &( TREE_RIGHT(*root) ), ident, flag );

    return (save < 0 || flag != TREE_UNIQUE) ?
	_find_bintree_point( tree, &( TREE_LEFT(*root) ), ident, flag ) :
	(TREE *) 0;
    }

NODE	_add_to_bintree( tree, node, ident, flag )
TREE	tree;
NODE	ident, node;
int	flag;
    {
    register TREE	*root, temp;

    check_tree_integrity( tree, "add_to_bintree" );

    if (root = _find_bintree_point(tree, &( TREE_ROOT( tree )), ident, flag)) {
	*root = temp = (TREE) make_node( sizeof( TREE_struct ) );
	TREE_LEFT( temp )  = NULLtree;
	TREE_RIGHT( temp ) = NULLtree;
	TREE_LENGTH( tree )++;
	return (TREE_NODE( temp ) = node);
	}

    return NULL;
    }

NODE	_get_from_bintree( tree, root, ident )
TREE	*root, tree;
NODE	ident;
    {
    register int	save;
    register TREE	temp;

    if ((temp = *root) == NULLtree) return NULL;	/* not in the tree? */

    if ((save = match_func( tree, ident, TREE_NODE( temp ))) == 0)
	return TREE_NODE( temp );		/* return o.k. */

    return (save > 0) ?
	_get_from_bintree( tree, &(TREE_RIGHT( temp )), ident ) :
	_get_from_bintree( tree, &(TREE_LEFT ( temp )), ident );
    }

static	add_deleted_branch( root, branch )
TREE	root, branch;
    {
    if (TREE_RIGHT( root ) == NULLtree)
	TREE_RIGHT( root ) = branch;
    else
	add_deleted_branch( TREE_RIGHT( root ), branch );
    }


NODE	_delete_from_bintree( tree, root, ident )
TREE	*root, tree;
NODE	ident;
    {
    register int	save;
    register TREE	temp;

    if ((temp = *root) == NULLtree) return NULL;	/* not in the tree? */

    if ((save = match_func( tree, ident, TREE_NODE( temp ))) == 0) {

	if (TREE_LEFT( temp ) == NULLtree)
	    *root	= TREE_RIGHT( temp );

	else {
	    *root	= TREE_LEFT( temp );
	    if (TREE_RIGHT( temp ) != NULLtree)
		add_deleted_branch( *root, TREE_RIGHT( temp ) );
	    }

	TREE_LENGTH( tree )--;			/* decrement length */
	free_node( temp );			/* kill the node */
	return TREE_NODE( temp );		/* return o.k. */
	}

    return (save > 0) ?
	_delete_from_bintree( tree, &(TREE_RIGHT( temp )), ident ) :
	_delete_from_bintree( tree, &(TREE_LEFT ( temp )), ident );
    }

int	_for_all_bintree( tree, node, func )
TREE	tree;
NODE	node;
PFI	func;
    {
    register int total;

    if (tree == NULLtree) return 0;

    total  = _for_all_bintree( TREE_LEFT( tree ), node, func );
    total += func( node, TREE_NODE( tree ) );
    return total + _for_all_bintree( TREE_RIGHT( tree ), node, func );
    }

int	_for_all_2bintree( tree, node, node2, func )
TREE	tree;
NODE	node, node2;
PFI	func;
    {
    register int total;

    if (tree == NULLtree) return 0;

    total  = _for_all_bintree( TREE_LEFT( tree ), node, node2, func );
    total += func( node, node2, TREE_NODE( tree ) );
    return total + _for_all_bintree( TREE_RIGHT( tree ), node, node2, func );
    }

int	_for_all_3bintree( tree, node, node2, node3, func )
TREE	tree;
NODE	node, node2, node3;
PFI	func;
    {
    register int total;

    if (tree == NULLtree) return 0;

    total  = _for_all_bintree( TREE_LEFT( tree ), node, node2, node3, func );
    total += func( node, node2, node3, TREE_NODE( tree ) );
    return total + _for_all_bintree( TREE_RIGHT( tree ), node, node2, node3, func );
    }

#if DEBUG

#define	TIMES	10

int	sort_by_name( string, node )
char	*string, *node;
    {
    int save;

    printf( "Comparing %s and %s, returning %d\n", string, node,
			save = strcmp( string, node ) );
    return save;
    }

int	print_node( dummy, node )
char	*dummy, *node;
    {
    printf( "'%s'\n", node );
    }

main() {
    TREE tree, copy;
    int	 i;
    char buffer[TIMES][80], delete[ 80 ];

    tree = make_bintree( sort_by_name );

    printf( "Enter %d strings to be added to the tree:\n", TIMES );
    for (i=0; i<TIMES; i++) {
	printf( "%-1d>  ", i );
	scanf( "%s", buffer[i] );
	if (add_unique_to_bintree( tree, buffer[i], buffer[i] ) == NULL)
	    printf( "It was already in the tree\n" );

	printf( "Tree has %d elements:\n", size_of_bintree( tree ) );
	for_all_bintree( tree, NULL, print_node );
	}

    printf( "Copying the tree\n" );
    copy = copy_bintree( tree, NOT_NODES );
    printf( "Tree has %d elements:\n", size_of_bintree( copy ) );
    for_all_bintree( copy, NULL, print_node );
    
    printf( "Enter %d strings to be deleted from the tree:\n", TIMES );
    for (i=0; i<TIMES; i++) {
	printf( "%-1d>  ", i );
	scanf( "%s", delete );
	if (delete_from_bintree( tree, delete ) == NULL)
	    printf( "It was not in the tree\n" );

	printf( "Tree has %d elements:\n", size_of_bintree( tree ) );
	for_all_bintree( tree, NULL, print_node );
	}

    free_bintree( tree, NOT_NODES );
    free_bintree( copy, NOT_NODES );

    mstats( stdout, "Memory Usage after freeing trees\n" );
    }
#endif
