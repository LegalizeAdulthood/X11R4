/*************************************************************************

	C Programmers' Toolbox
	Copyright (c) 1989 by Carnegie Mellon University, Pittsburgh, PA.

	Module Name:	memoryPack
	Purpose:	Memory management routines

	Author:		Robert A Zimmermann, Carnegie -Mellon University
			Department of Electrical and Computer Engineering

	History:	Notes:

	23 Feb 84	Created module out of routines from the layout
			system lUtils.

	 7 May 84	Added definition for copy_node

	25 Jun 84	Added definition for node_size

	 9 Jul 84	Changed the definition of free_node

	 7 Jan 84	Added definition of change_node_size
	
	15 Aug 85	Changed code module to perform its own allocation.
			This file then becomes function definitions.
	
	26 Jun 86	Changed to match the manual

 ************************************************************************* */

/* $Header: /u/04c/mnt/integ/c_toolkit/include/RCS/memoryPack.h,v 1.2 89/07/13 14:57:48 bmc Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Serpent Change History
|
$Log:	memoryPack.h,v $
 * Revision 1.2  89/07/13  14:57:48  bmc
 * Added external definition for u22_process_name (defined in mallocPack.c).
 * 
\*--------------------------------------------------------------------------*/


/*
 * Data structures
 */

#ifndef	DID_MEMORY_PACK			/* test for previous inclusion	*/

typedef char * NODE;

#define NULLnode	((NODE) 0)	/* Typecast NULL pointer */

#define	DID_MEMORY_PACK

#ifndef	memoryPack || MEMORYPACK
#ifndef	make_node
#define	make_node( n )		malloc1( n )
#define	copy_node( p )		copy_alloc( p )
#define	free_node( p )		free1( (char *) (p) )
#define dup_node( p )		dup_alloc( p )
#define node_refcount( p )	alloc_refcount( p )
#define change_node_size(p,x)	((p == NULL) ? malloc1(x) : realloc1( p, x ))
#define	memory_stats( fp, str )	mstats( fp, str )
#endif
#else

extern	NODE	make_node(), copy_node(), change_node_size();
extern		free_node(), node_size(), node_refcount();

#endif

extern	char	*u22_process_name; /* identifies current process */

extern	NODE	malloc1 ( /* size */ );

extern	NODE	calloc ( /* number, size */ );

extern	NODE	copy_alloc ( /* node */ );

extern		free1 ( /* node */ );

extern	NODE	realloc1( /* node, new_size */ );

extern	int	alloc_size ( /* node */ );

extern	int	dup_alloc( /* node */ );

extern	int	alloc_refcount( /* node */ );

#endif
