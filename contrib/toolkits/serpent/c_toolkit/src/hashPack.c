/*
 *	Module Name:		hashPack
 *
 *	Purpose:		Toolbox module for hash tables
 *
 *	Contents:
 *	(HASH)	make_hashtable( size, hashfunc, matchfunc )
 *			returns a new, empty table of the specified
 *			size, using the specified functions
 *
 *	(void)	free_hashtable( table, flag )
 *			frees the table and all associated storage.
 *			Will also free the nodes if flag == AND_NODES.
 *
 *	(NODE)	add_to_hashtable( table, node, ident )
 *			adds the entry to the table by applying the
 *			hashing function to the ident
 *			returns the node.
 *
 *	(NODE)	add_unique_to_hashtable( table, node, ident )
 *			adds the entry to the table (by applying the hashing
 *			function to the ident) if it is not in the table.
 *			returns the node if it is not in the table, otherwise
 *			NULL.
 *
 *	(NODE)	get_from_hashtable( table, ident )
 *			returns the node associated with ident if it is
 *			in the table, otherwise NULL
 *
 *	(NODE)	delete_from_hashtable( table, ident )
 *			as get_from_hashtable(), but removes the node as well
 *
 *	History:
 *	22Apr86	raz	Created
 *	26Jun86 raz	Made some of the macros agree with the manual
 *	07May87	raz	Made function references compilable
 */


#include "hashPack.h"			/* get our definitions */

/*
 * Debug support
 */

#ifdef DEBUG
#define DO_CHECK 2
#endif

#if DO_CHECK > 0
#include <stdio.h>
#endif

#ifndef NULL
#define NULL	((char *) 0)
#endif

#if DO_CHECK > 1

static	LIST	hashtables_in_use = NULLlist;

static	check_hash_integrity( table, routine )
HASH	table;
char	*routine;
    {
    register int i;

    if (hashtables_in_use == NULLlist)
	fprintf(stderr, "'*s' called before hashtable was created\n", routine);
    else if (!in_list( hashtables_in_use, table ))
	fprintf(stderr, "'%s' called with unknown hashtable\n", routine);
#if DO_CHECK > 2
    else {
	for (i = table->size; --i >= 0 )
	    if (table->lists[i] != NULLlist)
		in_list( table->lists[i], NULL );
	}
#endif
    }

#else

#define	check_hash_integrity( table, routine )

#endif

HASH	_make_hashtable( size, hashfunc, matchfunc )
int	size;
PFI	hashfunc, matchfunc;
    {
    register int	i;
    register HASH	temp;

#if DO_CHECK > 0
    if (size < 0) return NULLhash;
#endif

    temp = (HASH) make_node( sizeof( HASH_struct ) + ((size - 1) * sizeof( LIST )));

    temp->size			= size;
    temp->hashfunc		= hashfunc;
    temp->matchfunc		= matchfunc;
    size_of_hashtable( temp )	= 0;

    for ( i = size; --i >= 0; )	temp->lists[i] = NULLlist;

#if DO_CHECK > 1
    if (hashtables_in_use == NULLlist)
	hashtables_in_use = make_list();
    
    return (HASH) add_to_tail( hashtables_in_use, temp );
#else

    return temp;
#endif
    }

void	_free_hashtable( table, flag )
HASH	table;
int	flag;
    {
    register int i;

    check_hash_integrity( table, "free_hashtable" );

#if DO_CHECK > 1
    delete_list_node( hashtables_in_use, table );
#endif

    for ( i = table->size; --i >= 0; )
	if (table->lists[i] != NULLlist) free_list( table->lists[i], flag );
    free_node( table );
    }

NODE	_add_to_hashtable( table, node, ident, flag )
HASH	table;
NODE	node;
NODE	ident;
int	flag;
    {
    register int	index;
    register LIST	list;
    register NODE	ptr;

    check_hash_integrity( table, "add_to_hashtable" );

    index = (* table -> hashfunc)( ident );

#if DO_CHECK > 0
    if (index < 0 || index >= table->size) {
	fputs( "add_to_hashtable:  hashfunc value out of range\n", stderr );
	return NULL;
	}
#endif
    size_of_hashtable( table )++;
    
    if ((list = table->lists[ index ]) == NULLlist)
         list = table->lists[ index ] = make_list();
    else if (flag == HASH_UNIQUE) {
	loop_through_list( list, ptr, NODE )
	    if ((* table->matchfunc)( ident, ptr )) return NULL;
	}

    return add_to_tail( list, node );
    }

NODE	_get_from_hashtable( table, ident )
HASH	table;
NODE	ident;
    {
    register int	index;
    register NODE	ptr;
    register LIST	list;

    check_hash_integrity( table, "get_from_hashtable" );

    index = (* table -> hashfunc)( ident );

#if DO_CHECK > 0
    if (index < 0 || index >= table->size) {
	fputs( "get_from_hashtable:  hashfunc value out of range\n", stderr );
	return NULL;
	}
#endif
    
    if ((list = table->lists[ index ]) != NULLlist)
	loop_through_list( list, ptr, NODE )
	    if ((* table->matchfunc)( ident, ptr ))
		return ptr;

    return NULL;
    }

NODE	_delete_from_hashtable( table, ident )
HASH	table;
NODE	ident;
    {
    register int	index;
    register NODE	ptr;
    register LIST	list;

    check_hash_integrity( table, "delete_from_hashtable" );

    index = (* table -> hashfunc)( ident );

#if DO_CHECK > 0
    if (index < 0 || index >= table->size) {
	fputs( "delete_from_hashtable:  hashfunc value out of range\n", stderr );
	return NULL;
	}
#endif
    
    if ((list = table->lists[ index ]) != NULLlist)
	loop_through_list( list, ptr, NODE )
	    if ((* table->matchfunc)( ident, ptr )) {
		size_of_hashtable( table )--;
		return delete_list_node( list, ptr );
		}

    return NULL;
    }

int	_for_all_hashtable( table, node, func )
HASH	table;
NODE	node;
PFI	func;
    {
    register int	i, sum = 0;
    register NODE	temp;
    register LIST	list;

    check_hash_integrity( table, "hashtable_length" );

    for (i = table->size; --i >= 0; )
	if ((list = table->lists[i]) != NULLlist)
	    loop_through_list( list, temp, NODE )
		sum += func( node, temp );
    
    return sum;
    }

int	_for_all_2hashtable( table, node, node2, func )
HASH	table;
NODE	node, node2;
PFI	func;
    {
    register int	i, sum = 0;
    register NODE	temp;
    register LIST	list;

    check_hash_integrity( table, "hashtable_length" );

    for (i = table->size; --i >= 0; )
	if ((list = table->lists[i]) != NULLlist)
	    loop_through_list( list, temp, NODE )
		sum += func( node, node2, temp );
    
    return sum;
    }

int	_for_all_3hashtable( table, node, node2, node3, func )
HASH	table;
NODE	node, node2, node3;
PFI	func;
    {
    register int	i, sum = 0;
    register NODE	temp;
    register LIST	list;

    check_hash_integrity( table, "hashtable_length" );

    for (i = table->size; --i >= 0; )
	if ((list = table->lists[i]) != NULLlist)
	    loop_through_list( list, temp, NODE )
		sum += func( node, node2, node3, temp );
    
    return sum;
    }

#ifdef DEBUG

#define HashNum	256			/* maximum hash entry */
#define	NUM	1000

struct	foo {
	int	dummy;
	char	*name[80];
	} names[ NUM ];


static	 int	hash_name( str )
register char	*str;
    {
    register int i;

    for (i = 0; *str != '\0'; i += *str++ );

    return (i & 0xff);
    }


static		match_segments( str, str2 )
register char	*str;
struct	 foo	*str2;
    {
    return strcmp( str, str2->name ) == 0;
    }


static		print_name( dummy, str )
char		*dummy;
struct foo	*str;
    {
    printf( "String '%s'\n", str->name );
    }

main() {
    char	temp[ 80 ],
		*format = "hash entry #%d";
    HASH	hash;
    int		i, j;
    struct foo	*str;

    for (i=0; i<NUM; i++) sprintf( names[i].name, format, i );

    printf( "making a hash table\n" );
    hash	= make_hashtable( HashNum, hash_name, match_segments );

    printf( "adding hash entries\n" );
    for (i=0; i<NUM; i++)
	add_to_hashtable( hash, &names[i], names[i].name );
    
    printf( "checking hash entries\n" );
    for_all_hashtable( hash, NULL, print_name );

    for (i=0; i<10; i++) {
	printf( "Retrieve entry #" );
	scanf( "%d", &j );
	sprintf( temp, format, j );
	if ((str = (struct foo *) get_from_hashtable( hash, temp )) == NULL)
	    printf( "Entry #%d not found\n", j );
	else
	    printf( "Entry #%d is '%s'\n", j, str->name );
	}

    for (i=0; i<10; i++) {
	printf( "Delete entry #" );
	scanf( "%d", &j );
	sprintf( temp, format, j );
	if ((str = (struct foo *) delete_from_hashtable( hash, temp )) == NULL)
	    printf( "Entry #%d not found\n", j );
	else
	    printf( "Entry #%d is '%s'\n", j, str->name );
	}

    printf( "deleting hash entries\n" );
    for (i=0; i<NUM; i++)
	delete_from_hashtable( hash, &names[i] );

    free_hashtable( hash, NOT_NODES );

    memory_stats( stdout, "after freeing table" );
    }
#endif
