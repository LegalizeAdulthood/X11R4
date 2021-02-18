static char rcsid [] =
        "$Header: /u/04c/mnt/integ/c_toolkit/src/RCS/memoryPack.c,v 1.4 89/10/05 13:32:01 bmc Exp $";

/*
$Log:	memoryPack.c,v $
 * Revision 1.4  89/10/05  13:32:01  bmc
 * Test of check-in facility; no changes.
 * 
 * Revision 1.3  88/10/11  17:48:33  little
 * guard the test_free_struct call in free_node with a DEBUG
 * 
 * Revision 1.2  88/10/07  15:26:53  little
 * put in code (controlled by DEBUG flag) to check the validity of the
 * make&free node memory structures.  add log comment.
 * 
*/



/*************************************************************************

	C Programmers' Toolbox

	Copyright (c) 1989 by Carnegie-Mellon University, Pittsburgh, PA.

	Module Name:	memoryPack

	Purpose:	Memory management routines for non-unix o/s

	Author:		Robert A Zimmermann

	Notes:		Compile with the DO_CHECK option set to n or greater:
			0 - Do no run time checking or error reporting
			1 - Report errors
			2 - Generate statistics

	History:	

	25Jun87	raz	Rewrote to mirror mallocPack

 *************************************************************************/

/************************************************************************
 *		Set up defaults, error handlers, etc.			*
 ************************************************************************/

#ifdef	DEBUG
#define DO_CHECK	3
#endif

#ifndef DO_CHECK		/* Set default error checking	*/
#define DO_CHECK	2
#endif


#ifdef BELL
#define	bcopy( from, to, nbytes )	memcpy( to, from, nbytes )
#endif

#if	DO_CHECK > 0
#include <stdio.h>
static char *botch    = "memoryPack:  assertion failed: %s\n";
static char *free_err = "%s:  attempt to access a (probably) freed node 0x%x\n";

#define ASSERT(p)   if (!(p)) {					\
			fprintf( stderr, botch, " p " );	\
			abort();				\
			}

#if DO_CHECK > 1
#define	MAGIC			0x1c	/* random value for tagging our nodes */
#define ASSERT_MAGIC( mp )	ASSERT(mp->head.our_block == MAGIC)

#define	BAD_POINTER(func,addr)				\
	if (mp->head.our_block != MAGIC) {		\
	    fprintf( stderr, free_err, func, addr );	\
	    return 0;					\
	    }

#else
#define ASSERT_MAGIC( mp )
#endif

#else
#define ASSERT(p)
#define ASSERT_MAGIC( mp )
#endif


#ifndef NULL			/* pointer to nowhere */
#define	NULL 0
#endif

/************************************************************************
 *		Define the parameters for memory allocation		*
 *----------------------------------------------------------------------*
 *	SIZES:	The number of valid memory sizes to be allocated.	*
 *		Each size will require an entry in the 'nodes' table,	*
 *		and they must be put in the table in ascending order.	*
 *----------------------------------------------------------------------*
 *	BLOCK_SIZE:	The size of the pages which we will request	*
 *		from malloc1().  Each page, when received, will be	*
 *		subdivided into equal sized chunks of the required size.*
 *		The first will be returned to the user, the rest saved	*
 *		for future use.						*
 *		This must be a multiple of 1k (1024, 2048, etc.)	*
 *----------------------------------------------------------------------*
 *	BIGGEST_BLOCK:	The largest node that make_node() may get by	*
 *		subdividing a BLOCK_SIZE byte page.  Any request for	*
 *		nodes larger than this will be dealt with in multiples	*
 *		of BLOCK_SIZE chunks, so it should generally be the	*
 *		same as BLOCK_SIZE.					*
 ************************************************************************/

#define	SIZES		9			/* number of block lists */
#define	LAST_LIST	(SIZES-1)		/* this is the last small one */
#define	BIG_LIST	LAST_LIST		/* this is the over-sized one */

#define	BLOCK_SIZE	0x400			/* minimum for system request */
#define BIGGEST_BLOCK	BLOCK_SIZE		/* maximum 'small' block */
#define	BLOCK_BITS	(BLOCK_SIZE - 1)	/* low bits below 1k */


#define	is_small( ptr )	((((int) ptr) & BLOCK_BITS) != 0)


#define	MAX_REFCOUNT	255	/* beyond this, the block is un-freeable */

/************************************************************************
 *		Define the data structures and types			*
 ************************************************************************/


typedef char * NODE;		/* rather than importing the header */

NODE malloc ();


typedef	struct {
	unsigned short	which_size;	/* which free-list is it from? */
	unsigned char	our_block,	/* to tag blocks we created */
			refcount;	/* advanced memory support */
	} header;


typedef union memory_struct {
    header			head;		/* our information bytes */
    union	memory_struct	*next_node;	/* the free-list pointer */
    } *memory;


typedef struct long_memory_struct {
    header			head;		/* our information bytes */
    NODE			node;		/* where the block is */
    struct long_memory_struct	*next_node;	/* next long block */
    } *long_memory;

/************************************************************************
 *	This is the array which will hold the list of free nodes	*
 ************************************************************************/


static struct node_list {
	memory		next_node;	/* free-list pointer */
	unsigned int	num_in_use,	/* total nuber available */
			num_free,	/* number currently available */
			bytes_wasted,	/* total diff btw asked and given */
			nmalloc,	/* times malloc was used on this size */
			size;		/* sizeof(header) is included */
	} nodes[ SIZES ] = {
		    NULL, 0, 0, 0, 0, 16,	/* these are big node links */
		    NULL, 0, 0, 0, 0, 20,	/* LISTnode structures */
		    NULL, 0, 0, 0, 0, 32,
		    NULL, 0, 0, 0, 0, 64,
		    NULL, 0, 0, 0, 0, 128,
		    NULL, 0, 0, 0, 0, 256,
		    NULL, 0, 0, 0, 0, 512,
		    NULL, 0, 0, 0, 0, BIGGEST_BLOCK,
		    NULL, 0, 0, 0, 0, 0,	/* for big nodes */
		    };

#define long_nodes	((long_memory) nodes[ BIG_LIST ].next_node )

#if DO_CHECK > 1
/************************************************************************
 * 		memory_stats - print out statistics about memoryPack	*
 ************************************************************************/

#define	another_node( ptr )	ptr -> num_in_use++;	ptr -> num_free++;

	memory_stats(fp,s)
FILE	*fp;
char	*s;
    {
    register long_memory p;
    register int i;

    fprintf( fp, "Memory allocation statistics for %s\n", s);

    for (i=0; i< LAST_LIST ; i++) {
	if (i > 0 && nodes[i].size <= nodes[i-1].size)
	    fprintf( fp, "warning: list %d (size %d) smaller that list %d (size %d)\n",
			    i, nodes[i].size, i-1, nodes[i-1].size );
	if ((nodes[i].size % sizeof(NODE)) != 0)
	    fprintf( fp, "warning: list %d (size %d) not pointer aligned\n",
			    i, nodes[i].size );
	}

    fprintf( fp, "Sizes up to:\tin use:\tfree:\tmake'd:\tAvg Bytes wasted:" );
    for (i=0; i < LAST_LIST; i++) {
	fprintf( fp, "\n\t%d\t%d \t%d \t%d",
	    nodes[i].size, nodes[i].num_in_use - nodes[i].num_free,
	    nodes[i].num_free, nodes[i].nmalloc );
	if ( nodes[i].nmalloc )
	    fprintf( fp, " \t\t%d", nodes[i].bytes_wasted / nodes[i].nmalloc );
	}

    fprintf( fp, "\n<big nodes>\t%d \t%d \t%d\n",
	nodes[BIG_LIST].num_in_use - nodes[BIG_LIST].num_free,
	nodes[BIG_LIST].num_free, nodes[BIG_LIST].nmalloc );

    for (p = long_nodes; p != NULL; p = p->next_node)
	fprintf( fp, "\t0x%x, refcount(%d), size(%dK)\n",
			p -> node, p -> head.refcount, p -> head.which_size );
    }
#else
memory_stats() {}
#define	another_node( ptr )
#endif


void test_free_struct ()
{  int i;
   struct node_list *ptr;
   memory p;
   long_memory lp;

   for (ptr = nodes, i = 0; i < SIZES - 1; ptr++, i++)
      {p = ptr -> next_node;
      while (p)
         p = p -> next_node;
      }
   ptr = &nodes [8];
   if (ptr)
      {lp = (long_memory) ptr -> next_node;
      while (lp)
         lp = lp -> next_node;
      }

   return;
}


NODE		make_node( nbytes )    /* get a block */
unsigned	nbytes;
    {
#ifdef DEBUG
	int		nb, size, index;
#else
	register int		nb, size, index;
#endif

    if (nbytes == 0)	{
#ifdef DEBUG
    test_free_struct ();
#endif
    return NULL; }

#ifdef DEBUG
    test_free_struct ();
#endif

	    /* If the size needed exceeds the biggest standard sized block... */
    if ((nb  = nbytes + sizeof( header )) > BIGGEST_BLOCK) {
#ifdef DEBUG
	long_memory	lp, *lp2;
#else
	register long_memory	lp, *lp2;
#endif

	nb   = nbytes;
	size = (nb & BLOCK_BITS);			/* align to 1k */
	if (size > 0) nb += 1024 - size;		/* for efficiency */
	index = nb >> 10;				/* trash the low bits */

	for (lp = long_nodes; lp != NULL; lp = lp -> next_node)
	    if (lp -> head.refcount == 0 && lp -> head.which_size >= index)
		break;
	
	if (lp == NULL) {

	    another_node((&nodes[ BIG_LIST ]));		/* count the nodes */
	    lp = (long_memory) make_node((unsigned)
				sizeof( struct long_memory_struct ) );
	    lp -> head.which_size = index;
	    if (( lp -> node = (NODE) malloc1( nb )) == NULL) {
#ifdef DEBUG
    		test_free_struct ();
#endif
		return NULL;}

	    /* insert pointer into list of long nodes */
	    for ( lp2 = (long_memory *) &(nodes[BIG_LIST].next_node); ;
						    lp2 = &((*lp2)->next_node) )
		if ( *lp2 == NULL || (*lp2)->head.which_size >= index ) {
		    lp -> next_node	= *lp2;
		    (*lp2)		= lp;
		    break;
		    }
	    }

#if DO_CHECK > 1
	nodes[ BIG_LIST ].num_free--;
	nodes[ BIG_LIST ].nmalloc++;
#endif
	lp  -> head.refcount		= 1;
#ifdef DEBUG
    test_free_struct ();
#endif
	return lp -> node;
	}
    
    else {
#ifdef DEBUG
	memory			p;
	struct node_list	*ptr;
#else
	register memory			p;
	register struct node_list	*ptr;
#endif

	for (index = 0, ptr = nodes; (index < SIZES) && (ptr -> size < nb);
	    ptr++, index++)	/* null loop statement */ ;	/* find index */

	if (ptr -> next_node == NULL) {		/* get more memory */
						/* minimum request is BLOCK_SIZE */
	    size = (ptr -> size > BLOCK_SIZE) ? ptr -> size : BLOCK_SIZE;

	    if ((ptr -> next_node = p = (memory) malloc1( size )) == NULL) {
#ifdef DEBUG
    			test_free_struct ();
#endif
			return NULL;}

	    for ( ;; ) {    		/* set up linked list of the nodes */
		another_node( ptr );	/* keep track of number of nodes */
		if ((size -= ptr->size) < ptr->size) {
		    p -> next_node = NULL;
		    break;
		    }
		p = p -> next_node = (memory) (((int) p) + ptr->size );
		}
	    }

	p = ptr -> next_node;
	ptr -> next_node	= p->next_node;	/* update free-list */
	p   -> head.which_size	= index;	/* save the list index */
	p   -> head.refcount	= 1;		/* block is in use */
#if DO_CHECK > 1
	p   -> head.our_block  = MAGIC;		/* tag it for later */
	ptr -> num_free--;
	ptr -> nmalloc++;
	ptr -> bytes_wasted += ptr -> size - nb;
#endif

#ifdef DEBUG
    test_free_struct ();
#endif
	return (NODE) (((int) p) + sizeof( header ));
	}
    }

/************************************************************************
 *	dup_node - Duplicate a node.  Actually this just increments	*
 *			the node's reference count			*
 ************************************************************************/

		dup_node( ap )
register NODE	ap;			/* pointer to the user's block */
    {
    register memory		mp;
    register long_memory	lp;

    if (ap == NULL) return;		/* ignore this call */

    if (is_small( ap )) {		/* small sized node */

	mp = (memory) (ap - sizeof(header));	/* point back to header */

	ASSERT_MAGIC( mp );			/* make sure it's one of ours */

	if (mp->head.refcount < MAX_REFCOUNT)	/* so we don't overflow char */
	    ++mp->head.refcount;		/* bump the reference count */
	}
    else {
	for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)
	    if (lp -> node == ap) break;
	
	ASSERT( lp != NULL );
	if (lp->head.refcount < MAX_REFCOUNT)	/* so we don't overflow char */
	    ++lp->head.refcount;		/* bump the reference count */
	}
    }

int		node_refcount( ap )
register NODE	ap;			/* pointer to the user's block */
    {
    register memory		mp;
    register long_memory	lp;

    if (ap == NULL) return 0;		/* ignore this call */

    if (is_small( ap )) {		/* small sized node */

	mp = (memory) (ap - sizeof(header));	/* point back to header */

	BAD_POINTER( "alloc_refcount", ap );	/* do additional checking */

	return mp->head.refcount;
	}

    for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)
	if (lp -> node == ap) break;

    ASSERT( lp != NULL );

    return lp->head.refcount;
    }

		free_node( ap )
register NODE	ap;			/* pointer to the user's block */
    {
#ifdef DEBUG
    memory		mp;
    long_memory	lp;
    struct node_list	*ptr;
#else
    register long_memory	lp;
    register memory		mp;
    register struct node_list	*ptr;
#endif


    if (ap == NULL) return;		/* ignore this call */

#ifdef DEBUG
    test_free_struct ();
#endif

    if (is_small( ap )) {		/* small sized node */

	mp = (memory) (ap - sizeof(header));	/* point back to header */

	BAD_POINTER( "free_node", ap );		/* do additional checking */

	if (mp->head.refcount == MAX_REFCOUNT) return;	/* in use */

	if (--mp->head.refcount > 0) return;		/* still in use */

	ASSERT(mp->head.which_size < BIG_LIST);		/* a valid list index */

	ptr = &( nodes[ mp->head.which_size ] );	/* get list structure */
	mp  -> next_node = ptr -> next_node;
	ptr -> next_node = mp;
#if DO_CHECK > 1
	ptr -> num_free++;
#endif
	return 0;
	}

    for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)
	if (lp -> node == ap) break;

    ASSERT( lp != NULL );

    if (lp->head.refcount == MAX_REFCOUNT) return;	/* in use */

#if DO_CHECK > 1
    if (lp->head.refcount == 0) {
	    fprintf( stderr, free_err, "free_node", ap );
	    return 0;
	    }
#endif

    if (--lp->head.refcount != 0) return;		/* is it free? */

#if DO_CHECK > 1
    nodes[ BIG_LIST ].num_free++;
#endif
    return;
    }

#define	size_of_node( val, ap )						\
    if (is_small( ap )) {			/* small sized node */	\
	register memory mp;			/* temp. register */	\
	mp = (memory) (ap - sizeof(header));	/* point to header */	\
	ASSERT_MAGIC( mp );			/* it's one of ours? */	\
	ASSERT(mp->head.which_size < BIG_LIST);	/* valid list index? */	\
	val = nodes[ mp->head.which_size ].size - sizeof( header );	\
	}								\
    else {					/* big-list node */	\
	register long_memory lp;		/* temp. register */	\
	for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)	\
	    if (lp -> node == ap) break;	/* found the record? */	\
	ASSERT( lp != NULL );			/* check to be sure */	\
	val = ((lp -> head.which_size) << 10);	/* load the value */	\
	}

	 unsigned	node_size( ap )
register NODE		ap;
    {
    register unsigned temp;

    if (ap == NULL) return 0;

    size_of_node( temp, ap );

    return temp;
    }

	 NODE		change_node_size( ap, nbytes)
register NODE		ap;
register unsigned	nbytes;
    {
    register NODE		res;
    register struct node_list	*ptr;
    register unsigned int	size;

    if (ap == NULL)			return make_node(nbytes);

    if (nbytes == 0)			return (NODE) free_node(ap);

    size_of_node( size, ap );		/* how big is it? */

    if ((res = make_node(nbytes)) != NULL)
	bcopy( ap, res, (nbytes > size) ? size : nbytes );

    free_node( ap );

    return res;
    }

	 NODE	copy_node( ap )
register NODE	ap;
    {
    register NODE		res;
    register unsigned int	onb;

    if (ap == NULL) return NULL;

    size_of_node( onb, ap );

    if ((res = make_node(onb)) != NULL)
	bcopy( ap, res, (int) onb );

    return res;
    }

#ifdef DEBUG_MAIN

#define	BLOCKS	1500

main() {
    NODE my_nodes[ BLOCKS ];
    int  i;

    for (i=0; i<BLOCKS; i++)
	my_nodes[i] = (NODE) make_node( i );
    memory_stats( stdout, "After initial allocation" );

    for (i=0; i<BLOCKS; i+=2)
	free_node( my_nodes[i] );
    memory_stats( stdout, "After partial freeing" );

    for (i=0; i<BLOCKS; i+=2)
	my_nodes[i] = (NODE) make_node( i );
    memory_stats( stdout, "After second allocation (should be same as first)" );
    
    for (i=0; i<BLOCKS; i++)
	free_node( my_nodes[i] );
    memory_stats( stdout, "After final free (everything free except 16's)" );
    }
#endif
