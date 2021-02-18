static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/ush.c,v 1.3 89/02/13 12:33:33 ejh Exp $";

/*	$Log:	ush.c,v $
 * Revision 1.3  89/02/13  12:33:33  ejh
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

#include "ush.h"		/* shared memory hash utility		    */

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

static	SHM_LIST	hashtables_in_use = NULLshmlist;

static	check_hash_integrity( table, routine )
SHM_HASH	table;
char	*routine;
    {
    register int i;

    if (hashtables_in_use == NULLshmlist)
	fprintf(stderr, "'*s' called before hashtable was created\n", routine);
    else if (!usl_in_shm_list( hashtables_in_use, table ))
	fprintf(stderr, "'%s' called with unknown hashtable\n", routine);
#if DO_CHECK > 2
    else {
	for (i = table->size; --i >= 0 )
	    if (table->lists[i] != NULLshmlist)
		usl_in_shm_list( table->lists[i], NULL );
	}
#endif
    }

#else

#define	check_hash_integrity( table, routine )

#endif

SHM_HASH	_ush_make_shm_hashtable( size, hashfunc, matchfunc )
int	size;
PFI	hashfunc, matchfunc;
    {
    register int	i;
    register SHM_HASH	temp;

#if DO_CHECK > 0
    if (size < 0) return NULLshmhash;
#endif

    temp = (SHM_HASH) make_shm_node( sizeof( SHM_HASH_struct ) + ((size - 1) * sizeof( SHM_LIST )));

    temp->size			= size;
    temp->hashfunc		= hashfunc;
    temp->matchfunc		= matchfunc;
    ush_size_of_shm_hashtable( temp )	= 0;

    for ( i = size; --i >= 0; )	temp->lists[i] = NULLshmlist;

#if DO_CHECK > 1
    if (hashtables_in_use == NULLshmlist)
	hashtables_in_use = usl_make_shm_list();
    
    return (SHM_HASH) usl_add_to_tail( hashtables_in_use, temp );
#else

    return temp;
#endif
    }

void	_ush_change_shm_hash_functions( table, hashfunc, matchfunc )
SHM_HASH	table;
PFI	hashfunc, matchfunc;
    {

    check_hash_integrity( table, "ush_free_shm_hashtable" );

    table->hashfunc		= hashfunc;
    table->matchfunc		= matchfunc;

    }

void	_ush_free_shm_hashtable( table, flag )
SHM_HASH	table;
int	flag;
    {
    register int i;

    check_hash_integrity( table, "ush_free_shm_hashtable" );

#if DO_CHECK > 1
    usl_delete_shm_list_node( hashtables_in_use, table );
#endif

    for ( i = table->size; --i >= 0; )
	if (table->lists[i] != NULLshmlist) usl_free_shm_list( table->lists[i], flag );
    free_shm_node( table );
    }

SHM_NODE	_ush_add_to_shm_hashtable( table, node, ident, flag )
SHM_HASH	table;
SHM_NODE	node;
SHM_NODE	ident;
int	flag;
    {
    register int	index;
    register SHM_LIST	list;
    register SHM_NODE	ptr;

    check_hash_integrity( table, "ush_add_to_shm_hashtable" );

    index = (* table -> hashfunc)( ident );

#if DO_CHECK > 0
    if (index < 0 || index >= table->size) {
	fputs( "ush_add_to_shm_hashtable:  hashfunc value out of range\n", stderr );
	return NULL;
	}
#endif
    ush_size_of_shm_hashtable( table )++;
    
    if ((list = table->lists[ index ]) == NULLshmlist)
         list = table->lists[ index ] = usl_make_shm_list();
    else if (flag == SHM_HASH_UNIQUE) {
	usl_loop_through_shm_list( list, ptr, SHM_NODE )
	    if ((* table->matchfunc)( ident, ptr )) return NULL;
	}

    return usl_add_to_tail( list, node );
    }

SHM_NODE	_ush_get_from_shm_hashtable( table, ident )
SHM_HASH	table;
SHM_NODE	ident;
    {
    register int	index;
    register SHM_NODE	ptr;
    register SHM_LIST	list;

    check_hash_integrity( table, "ush_get_from_shm_hashtable" );

    index = (* table -> hashfunc)( ident );

#if DO_CHECK > 0
    if (index < 0 || index >= table->size) {
	fputs( "ush_get_from_shm_hashtable:  hashfunc value out of range\n", stderr );
	return NULL;
	}
#endif
    
    if ((list = table->lists[ index ]) != NULLshmlist)
	usl_loop_through_shm_list( list, ptr, SHM_NODE )
	    if ((* table->matchfunc)( ident, ptr ))
		return ptr;

    return NULL;
    }

SHM_NODE	_ush_delete_from_shm_hashtable( table, ident )
SHM_HASH	table;
SHM_NODE	ident;
    {
    register int	index;
    register SHM_NODE	ptr;
    register SHM_LIST	list;

    check_hash_integrity( table, "ush_delete_from_shm_hashtable" );

    index = (* table -> hashfunc)( ident );

#if DO_CHECK > 0
    if (index < 0 || index >= table->size) {
	fputs( "ush_delete_from_shm_hashtable:  hashfunc value out of range\n", stderr );
	return NULL;
	}
#endif
    
    if ((list = table->lists[ index ]) != NULLshmlist)
	usl_loop_through_shm_list( list, ptr, SHM_NODE )
	    if ((* table->matchfunc)( ident, ptr )) {
		ush_size_of_shm_hashtable( table )--;
		return usl_delete_shm_list_node( list, ptr );
		}

    return NULL;
    }

int	_ush_for_all_shm_hashtable( table, node, func ) 
SHM_HASH	table;
SHM_NODE	node;
PFI	func;
    {
    register int	i, sum = 0;
    register SHM_NODE	temp;
    register SHM_LIST	list;

    check_hash_integrity( table, "hashtable_length" );

    for (i = table->size; --i >= 0; )
	if ((list = table->lists[i]) != NULLshmlist)
	    usl_loop_through_shm_list( list, temp, SHM_NODE )
		sum += func( node, temp );
    
    return sum;
    }

int	_ush_for_all_shm_2hashtable( table, node, node2, func )
SHM_HASH	table;
SHM_NODE	node, node2;
PFI	func;
    {
    register int	i, sum = 0;
    register SHM_NODE	temp;
    register SHM_LIST	list;

    check_hash_integrity( table, "hashtable_length" );

    for (i = table->size; --i >= 0; )
	if ((list = table->lists[i]) != NULLshmlist)
	    usl_loop_through_shm_list( list, temp, SHM_NODE )
		sum += func( node, node2, temp );
    
    return sum;
    }

int	_ush_for_all_shm_3hashtable( table, node, node2, node3, func )
SHM_HASH	table;
SHM_NODE	node, node2, node3;
PFI	func;
    {
    register int	i, sum = 0;
    register SHM_NODE	temp;
    register SHM_LIST	list;

    check_hash_integrity( table, "hashtable_length" );

    for (i = table->size; --i >= 0; )
	if ((list = table->lists[i]) != NULLshmlist)
	    usl_loop_through_shm_list( list, temp, SHM_NODE )
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
    SHM_HASH	hash;
    int		i, j;
    struct foo	*str;

    for (i=0; i<NUM; i++) sprintf( names[i].name, format, i );

    printf( "making a hash table\n" );
    hash	= ush_make_shm_hashtable( HashNum, hash_name, match_segments );

    printf( "adding hash entries\n" );
    for (i=0; i<NUM; i++)
	ush_add_to_shm_hashtable( hash, &names[i], names[i].name );
    
    printf( "checking hash entries\n" );
    ush_for_all_shm_hashtable( hash, NULL, print_name );

    for (i=0; i<10; i++) {
	printf( "Retrieve entry #" );
	scanf( "%d", &j );
	sprintf( temp, format, j );
	if ((str = (struct foo *) ush_get_from_shm_hashtable( hash, temp )) == NULL)
	    printf( "Entry #%d not found\n", j );
	else
	    printf( "Entry #%d is '%s'\n", j, str->name );
	}

    for (i=0; i<10; i++) {
	printf( "Delete entry #" );
	scanf( "%d", &j );
	sprintf( temp, format, j );
	if ((str = (struct foo *) ush_delete_from_shm_hashtable( hash, temp )) == NULL)
	    printf( "Entry #%d not found\n", j );
	else
	    printf( "Entry #%d is '%s'\n", j, str->name );
	}

    printf( "deleting hash entries\n" );
    for (i=0; i<NUM; i++)
	ush_delete_from_shm_hashtable( hash, &names[i] );

    ush_free_shm_hashtable( hash, NOT_NODES );

    memory_stats( stdout, "after freeing table" );
    }
#endif
