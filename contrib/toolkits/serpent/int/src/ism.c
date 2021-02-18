static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/ism.c,v 1.7 89/10/13 10:51:52 rcs Exp $";

/*--------------------------------------------------------------------------*\
|
|    Name:  Shared Memory Abstraction (ism)
|
|    Description:  
|	The shared memory abstraction provides for allocation and 
|	deallocation of shared memory.  
|	
|       Assign to each area an index "n". This is currently proportional to
|	the log 2 of size of the area rounded down to the nearest integer.
|	Then all free areas of storage whose length have the same index n are
|	organized into a chain with other free areas of index n (the "bucket"
|	chain). A request for allocation of storage first searches the list of
|	free memory.  The search starts at the bucket chain of index equal to
|	that of the storage request, continuing to higher index bucket chains
|	if the first attempt fails.
|	If the search fails then new memory is allocated.  Only the amount of
|	new memory needed is allocated.  Any old free memory left after an
|	allocation is returned to the free list.
|
|	All memory areas (free or busy) handled by malloc are also chained
|	sequentially by increasing address (the adjacency chain).  When memory
|	is freed it is merged with adjacent free areas if any.  If a free area
|	of memory ends at the end of memory (i.e. at the break), and if the
|	variable "endfree" is non-zero, then the break is contracted, freeing
|	the memory back to the system.
|
|	Notes:
|		ov_length field includes sizeof(struct overhead)
|		adjacency chain includes all memory, allocated plus free.
|
|
|    Component of:
|       Interface Layer.
|
|    Related files:
|	ism.c - contains the implementation for the shared memory.
|	ism.h - contains the external definition for shared memory.
|	ismtest.c - test file.
|
|    Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|    Version: 1.0
|
|    Project Leader:
|	Len Bass
|	ljb@sei.cmu.edu
|
|    Author:  
|	Robert C. Seacord
|	rcs@sei.cmu.edu  
|
|    Change History:
|	Oct. 19, 1987 : Module created.
|       $Log:	ism.c,v $
 * Revision 1.7  89/10/13  10:51:52  rcs
 * added ism_print_alloc_mem routine
 * 
 * Revision 1.6  89/08/29  10:54:24  bmc
 * Conditionally compiled code no longer defaults to VAX architecture if
 * '#ifdef mc68000' test fails.  Instead, explicit tests are included for
 * each supported architecture.  Unsupported architectures will cause
 * compilation errors.  (SPR #109)
 * 
 * Revision 1.5  89/03/07  16:19:40  little
 * fix up so shared mem is attached at 5MEG on the sun, due to the
 * sbrk "problem"
 * 
 * Revision 1.4  89/02/15  18:14:26  little
 * add some denug print and copyright notice
 * 
 * Revision 1.3  88/11/02  14:50:41  rcs
 * moved debug variable to interface file
 * 
|
\*--------------------------------------------------------------------------*\
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
/*-------------------- Includes --------------------------------------------*/

#define ism_main
#include "ism.h"			/* shared memory 		    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(ism_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(ism_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(ism_debug, statement)

#ifdef mc68000
#define SHM_SIZE 	0x00020000
#define SHM_START_ADDR 	0x00500000
#endif mc68000

#ifdef vax
#define SHM_SIZE 	0x00020000
#define SHM_START_ADDR 	0x00A00000
#endif vax

#define SHM_END_ADDR 	SHM_START_ADDR+SHM_SIZE
#define SHM_FLAG 0777

/*--------------------------------------------------------------------------*\
|  The following items may need to be configured for a particular machine.
\*--------------------------------------------------------------------------*/
/* 
   Alignment requirement for machine (in bytes). 
*/

#define NALIGN	4

/* 
   Size of an integer large enough to hold a character pointer.
*/

typedef	long Size;

/*--------------------------------------------------------------------------*\
|  End machine dependent definitions.
\*--------------------------------------------------------------------------*/

#define	MAGIC_FREE 0x548a934c
#define	MAGIC_BUSY 0xc139569a

#define NBUCKETS 18

typedef struct qelem {
  struct qelem *q_forw;
  struct qelem *q_back;
} qelem;

typedef struct {
  qelem	adjacency;			/* adjacency chain pointers	    */
  qelem	bucket;				/* bucket chain pointers 	    */
  long	magic;
  Size	length;
} overhead;

/*
  The following macros depend on the order of the elements in overhead
*/
#define TOADJ(p)   ((qelem *)(p))
#define FROMADJ(p) ((overhead *)(p))
#define FROMBUK(p) ((overhead *)( (char *)p - sizeof(qelem)))
#define TOBUK(p)   ((qelem *)( (char *)p + sizeof(qelem)))

void (*mlabort)() = {0};

#define ASSERT(p,q)	if (!(p)) mllcerr(q)

/*-------------------- Type Definitions ------------------------------------*/

/*--------------------------------------------------------------------------*\
|  The following structures are used to define how shared memory is 
|  allocated.  They must therefore also reside in shared memory since 
|  multiple process will be allocating and deallocating from it and
|  this structure must be maintained across processes.
\*--------------------------------------------------------------------------*/
/*
   Shared memory description structure.
*/
typedef struct {
  char *unused_start_addr;			/* start of unused memory   */
  qelem adjhead;			  	/* head of adjacency chain  */
  qelem buckets[NBUCKETS];	 		/* Head of bucket chains    */
} shm;

/*-------------------- Retained Data ---------------------------------------*/
/* 
   Sizes of buckets currently proportional to log 2() 
*/
Size mlsizes[] = {0, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
                  65536, 131072, 262144, 524288, 1048576, 2097152, 4194304};

int shared_memory_id;				/* shared memory seg id	    */

shm *shm_ptr = (shm *)SHM_START_ADDR;	/* pointer to start of shared mem   */

/*--------------------------------------------------------------------------*\
|  Name: ism_create
| 
|  Description:
|     Allocates 1M of storage for shared memory starting from 10M - 11M in
|     process memory.
\*--------------------------------------------------------------------------*/
int ism_create()
{					/* local type definitions	    */
  int rc;				/* return code from system calls    */
  int i;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_create:  call.\n");
  );
/*
   Get shared memory segment.
*/
  shared_memory_id = shmget(IPC_PRIVATE, SHM_SIZE, SHM_FLAG);

  DEBUG3 (fprintf (stderr, "ism_create: shared_memory_id = %d\n",
	shared_memory_id););

  rcheck_error(
    shared_memory_id,
    "ism_create: error during shm_get", 
    system_operation_failed,
    error_val
  );
/*
   Attach process to shared memory in order to initialize.
*/
  rc = shmat(shared_memory_id, SHM_START_ADDR, SHM_FLAG);

  DEBUG3 (fprintf (stderr, "ism_create: rc from shmat = %d\n", rc););

  rcheck_error(
    rc,
    "ism_create:  error during shmat",
    system_operation_failed,
    error_val
  );
/*
   Initialize adjacency chain pointers.
*/
  (shm_ptr->adjhead).q_forw = (qelem *)&(shm_ptr->adjhead);
  (shm_ptr->adjhead).q_back = (qelem *)&(shm_ptr->adjhead);
/*
   Initialize bucket pointers.
*/
  for(i = 0; i < NBUCKETS; i++) {
    (shm_ptr->buckets[i]).q_forw = (qelem *)&(shm_ptr->buckets[i]);
    (shm_ptr->buckets[i]).q_back = (qelem *)&(shm_ptr->buckets[i]);
  }
/*
   Start of unused memory.
*/
  shm_ptr->unused_start_addr = (char *)(SHM_START_ADDR + sizeof(shm));
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_create: shared memory id = %d.\n", shared_memory_id);
    fprintf(stderr, "ism_create:  exiting.\n");
  );
  return shared_memory_id;
}                                         /* end ism_create                 */

/*--------------------------------------------------------------------------*\
|  Name: ism_init
| 
|  Description:  
|     Used by child processes to initialize shared memory id from system 
|     variable and attach process to shared memory.
\*--------------------------------------------------------------------------*/
void ism_init()
{                                         /* local type definitions	    */
  int rc;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_init:  call.\n");
  );
/*
   Initialize shared memory id from system variable.
*/
  DEBUG3 (fprintf (stderr, "ism_init: genenv = %x\n", getenv("SHM_ID")););
  DEBUG3 (fprintf (stderr, "ism_init: id = %d\n", getpid ()););

  DEBUG3 (fprintf (stderr, "ism_init: id = %d\n", getpid ()););

  shared_memory_id = atoi(getenv("SHM_ID"));
  DEBUG3 (fprintf (stderr, "ism_init: shared_memory_id = %d\n",
	  shared_memory_id););
/*
   Attach process to shared memory.
*/
  rc = shmat(shared_memory_id, SHM_START_ADDR, SHM_FLAG);
  check_error(
    rc,
    "ism_init:  error during shmat",
    system_operation_failed
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_init:  exiting.\n");
  );
  return;
}                                         /* end ism_init                   */

/*--------------------------------------------------------------------------*\
|  Name:  mlindx
| 
|  Description:  
|     Internal routine which selects the smallest sized bucket from which 
|     memory can be allocated.
\*--------------------------------------------------------------------------*/
Size mlindx(n)
register Size n;
{
  register Size *p, *q, *r;

  p = &mlsizes[0];
  r = &mlsizes[NBUCKETS];
/* 
   Binary search. 
*/
  while ((q = (p + (r-p)/2)) > p) {
    if (n < *q) {
      r = q;
    }  
    else {
      p = q;
    }
  }
  return(q - &mlsizes[0]);
}

/*--------------------------------------------------------------------------*\
|  Name:  millcerr
| 
|  Description:  
|     Internal routine that performs scme sort of error handling.
\*--------------------------------------------------------------------------*/
void mllcerr(p)
char *p;
{
  register char *q;

  q = p;
  while (*q++);				/* find end of string		    */
  (void)write(2,p,q-p-1);
  if (mlabort)
    (*mlabort)();
    else
      abort();
  return;
}

/*--------------------------------------------------------------------------*\
|  Name:  insque
| 
|  Description:  
|     Internal routine to insert "item" after  "queu".
|
|  The vax has wondrous instructions for inserting and removing items into
|  doubly linked queues.  On the vax the assembler output of the C compiler is
|  massaged by an sed script to turn these function calls into invocations of
|  the insque and remque machine instructions.
\*--------------------------------------------------------------------------*/
void insque(item,queu)
register qelem *item, *queu;
{
  register qelem *pueu;

  pueu = queu->q_forw;
  item->q_forw = pueu;
  item->q_back = queu;
  queu->q_forw = item;
  pueu->q_back = item;
}

/*--------------------------------------------------------------------------*\
|  Name:  remque
| 
|  Description:  
|     Internal routine to removes an item.
\*--------------------------------------------------------------------------*/
void remque(item)
register qelem *item;
{
  register qelem *queu, *pueu;

  pueu = item->q_forw;
  queu = item->q_back;
  queu->q_forw = pueu;
  pueu->q_back = queu;
}

/*--------------------------------------------------------------------------*\
|  Name: ism_get_shared_memory
| 
|  Description:  
|     Creates a shared memory node and returns a handle to that node.
\*--------------------------------------------------------------------------*/
ism_node ism_get_shared_memory(nbytes)
Size nbytes;
{ 					/* local type definitions	    */
  register overhead *p, *q;
  register qelem *bucket;
  register Size surplus;
  Size mlindx();
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(
    fprintf(stderr, "ism_get_shared_memory: call.\n");
    fprintf(stderr, "ism_get_shared_memory: nbytes = %d.\n", nbytes);
  );
/*
   Write synchronize get shared memory operation.
*/
  ids_start_write();
  rcheck_status("ism_get_shared_memory:", NULL);
/*
   Determine overall size of storage to allocate.
*/
  nbytes = ((nbytes + (NALIGN-1)) & ~(NALIGN-1)) + sizeof(overhead);
/*
   Attempt to allocate memory out of free memory.
*/
  for (bucket = &(shm_ptr->buckets[mlindx(nbytes)]); 
    bucket < &(shm_ptr->buckets[NBUCKETS]); 
    bucket++
  ) { 			/* for each sized bucket starting from smallest	    */
    register qelem *b = shm_ptr->buckets;
    for(b = bucket->q_forw; b != bucket; b = b->q_forw) {
      p = FROMBUK(b);
      ASSERT(p->magic == MAGIC_FREE,
		"\nmalloc: Entry not marked FREE found on Free List!\n");
      if (p->length >= nbytes) {
        remque(b);
	surplus = p->length - nbytes;
	goto foundit;
      }					/* end if 			    */
    }					/* end for 			    */
  }					/* end for each size bucket	    */

/*  
   Attempt to allocate memory out of unused memory.
*/
  {
    register Size i;

    p = (overhead *)(shm_ptr->unused_start_addr);

    i = ((Size)p)&(NALIGN-1);
    if (i != 0) {
      p = (overhead *)((char *)p + NALIGN - i);
    }
/*
   If allocated memory does not exceed the end of shared data increase
   the starting address of unused memory, otherwise return null.
*/
    if (((char *)p + nbytes) < (char *)SHM_END_ADDR) {
      shm_ptr->unused_start_addr = (char *)p + nbytes;
    }
    else {

      DEBUG1(
        fprintf(
          stderr, "ism_get_shm:  request exceeds shared memory limit.\n"
        );
        fprintf(stderr, "ism_get_shm:  nbytes = %d.\n", nbytes);
        fprintf(
          stderr, 
          "ism_get_shm: (char *)p+nbytes = %d.\n", 
          (char*)p+nbytes
        );
        fprintf(stderr, "ism_get_shm: SHM_END_ADDR = %d.\n", SHM_END_ADDR);
      );

      set_status(out_of_memory);
      return(NULL);
    }
    p->length = nbytes;
    surplus = 0;
/*
   Add to end of adjacency chain.
*/
    DEBUG2(
      ASSERT(
        (FROMADJ(shm_ptr->adjhead.q_back)) < p, 
        "\nmalloc: Entry in adjacency chain with addr below Chain head!\n"
      );
    );

    insque(TOADJ(p),shm_ptr->adjhead.q_back);
  }

foundit:

      DEBUG3(
        fprintf(stderr, "ism_get_shm:  At foundit.\n");
      );

/* 
   Mark surplus memory free. 
*/
  if (surplus > sizeof(overhead)) {

      DEBUG3(
        fprintf(stderr, "ism_get_shm:  Surplus > sizeof(overhead).\n");
      );

/* 
    Move surplus from p to q.
*/
    q = (overhead *)((char *)p + nbytes);

    q->length = surplus;
    p->length = nbytes;
    q->magic = MAGIC_FREE;
/* 
   Add surplus into adjacency chain.
*/
    DEBUG3(
      fprintf(stderr, "ism_get_shm:  Add surplus into adjacency chain.\n");
    );

    insque(TOADJ(q),TOADJ(p));
/* 
   Add surplus into bucket chain.
*/
    DEBUG3(
      fprintf(stderr, "ism_get_shm:  Add surplus into bucket chain.\n");
    );

    insque(TOBUK(q),&(shm_ptr->buckets[mlindx(surplus)]));
  }
  p->magic = MAGIC_BUSY;
/*
   End write synchronize for get shared memory operation.
*/
  ids_end_write();
  rcheck_status("ism_get_shared_memory:", NULL);
/*
   Make sure that the specified memory location is between the
   boundaries for shared data.
*/
  if (((char *)p < (char *)SHM_START_ADDR)
     || ((char *)p > (char *)SHM_END_ADDR)) {   /* error                    */
    set_status(shm_out_of_bounds);
    return(NULL);
  }						/* end if error condition   */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, 
      "ism_get_shared_memory: node = %x.\n", 
      (char*)p + sizeof(overhead)
    );
    fprintf(stderr, "ism_get_shared_memory:  exiting.\n");
  );
  return((char*)p + sizeof(overhead));
}					/* end ism_get_shared_memory	    */

/*--------------------------------------------------------------------------*\
|  Name: ism_copy_shared_memory
| 
|  Description:  
|     Returns a pointer to a shared memory node which is identical to the
|     original node.
\*--------------------------------------------------------------------------*/
ism_node ism_copy_shared_memory(node)
ism_node node;
{                                         /* local type definitions	    */
  int i;
  Size n;
  ism_node shm_node;
  register char *mem = (char *)node;
  register overhead *p;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_copy_shared_memory:  call.\n");
    fprintf(stderr, "ism_copy_shared_memory:  node = %x.\n", node);
  );
/*
   Write synchronize get shared memory operation.
*/
  ids_start_write();
  rcheck_status("ism_copy_shared_memory:", NULL);
/*
   Return if shared memory pointer is null.
*/
  if (mem == NULL) {			/* null memory pointer		    */
    DEBUG2(
      fprintf(stderr, "ism_copy_shared_memory:  null shared memory ptr.\n");
    );
    set_status(null_memory_ptr);
    return;
  }
  p = (overhead *)(mem - sizeof(overhead));
/*
   Create a shared memory node of the same size as "node".
*/
  n = p->length;
  shm_node = ism_get_shared_memory(n);
  rcheck_status("ism_copy_shared_memory: %d from ism_get_shm.\n", NULL);
/*
   Copy n bytes from user_data to shared memory.
*/
  for (i = 0; i < n; i++) {
    *shm_node++ = *node++;
  }
/*
   End write synchronize for copy shared memory operation.
*/
  ids_end_write();
  rcheck_status("ism_copy_shared_memory:", NULL);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_copy_shared_memory:  node = %x.\n", shm_node);
    fprintf(stderr, "ism_copy_shared_memory:  exiting.\n");
  );
  return shm_node;
}                                         /* end ism_copy_shm	    */

/*--------------------------------------------------------------------------*\
|  Name: ism_copy_into_shared_memory
| 
|  Description:  
|     Copies "n" bytes from "user_data" to "shared_memory".
\*--------------------------------------------------------------------------*/
void ism_copy_into_shared_memory(shared_memory, n, user_data)
ism_node shared_memory;
Size n;
char *user_data;
{                                         /* local type definitions	    */
  int i;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_copy_into_shared_memory: call.\n");
    fprintf(
      stderr, 
      "ism_copy_into_shared_memory: shared memory = %x.\n",
      shared_memory
    );
    fprintf(stderr, "ism_copy_into_shared_memory: n = %d.\n", n);
    fprintf(
      stderr, 
      "ism_copy_into_shared_memory: user data = %x.\n",
      user_data
    );
  );
/*
   Copy n bytes from user_data to shared memory.
*/
  for (i = 0; i < n; i++) {
    *shared_memory++ = *user_data++;
  }
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_copy_into_shared_memory:  exiting.\n");
  );
  return;
}                                         /* end ism_copy_into_shm	    */

/*--------------------------------------------------------------------------*\
|  Name: ism_copy_from_shared_memory
| 
|  Description:  
|     Copies "n" bytes from "shared_memory" to "user_data".
\*--------------------------------------------------------------------------*/
void ism_copy_from_shared_memory(shared_memory, n, user_data)
ism_node shared_memory;
Size n;
char *user_data;
{                                         /* local type definitions	    */
  int i;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_copy_from_shared_memory:  call.\n");
    fprintf(
      stderr, 
      "ism_copy_from_shared_memory: shared memory = %x.\n",
      shared_memory
    );
    fprintf(stderr, "ism_copy_from_shared_memory: n = %d.\n", n);
    fprintf(
      stderr, 
      "ism_copy_from_shared_memory: user data = %x.\n",
      user_data
    );
  );
/*
   Copy n bytes from shared memory to user data.
*/
  for (i = 0; i < n; i++) {
    *user_data++ = *shared_memory++;
  }
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_copy_from_shared_memory:  exiting.\n");
  );
  return;
}                                         /* end ism_copy_from_shm	    */

/*--------------------------------------------------------------------------*\
|  Name: ism_free_shared_memory
| 
|  Description:  
|     Deallocates an existing shared memory node.
\*--------------------------------------------------------------------------*/
void ism_free_shared_memory(shared_memory)
ism_node shared_memory;
{                                         /* local type definitions         */
  register char *mem = (char *)shared_memory;
  register overhead *p, *q;
  void mlfree_end();
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_free_shared_memory: call.\n");
    fprintf(
      stderr, 
      "ism_free_shared_memory: shared_memory = %x.\n", 
      shared_memory
    );
  );
/*
   Write synchronize free shared memory operation.
*/
  ids_start_write();
  check_status("ism_get_shared_memory:");
/*
   Return if shared memory pointer is null.
*/
  if (mem == NULL) {			/* null memory pointer		    */
    DEBUG2(
      fprintf(stderr, "ism_free_shared_memory:  null shared memory ptr.\n");
    );
    set_status(null_memory_ptr);
    return;
  }
  p = (overhead *)(mem - sizeof(overhead));
/* 
   Not advised but allowed.
*/
  if (p->magic == MAGIC_FREE) {	/* memory already freed		    */
    DEBUG2(
      fprintf(stderr, "ism_free_shared_memory:  memory already free.\n");
    );
    set_status(invalid_request);
    return;
  }
/*
   Else if memory is neither busy nor free then it wasn't allocated with
   this malloc.
*/
  else if (p->magic != MAGIC_BUSY) {
    mllcerr("attempt to free memory not allocated with malloc!\n");
  }
/* 
   Try to merge with previous free area.
*/
  q = FROMADJ((TOADJ(p))->q_back);

  if (q != FROMADJ(&(shm_ptr->adjhead))) {
    ASSERT(q < p,
           "\nfree: While trying to merge a free area with a lower adjacent free area,\n addresses were found out of order!\n");
/* 
   If lower segment can be merged. 
*/
    if (q->magic == MAGIC_FREE  && (char *)q + q->length == (char *)p) {
/* 
   Remove lower address area from bucket chain.
*/
      remque(TOBUK(q));
/* 
   Remove upper address area from adjacency chain. 
*/
      remque(TOADJ(p));

      q->length += p->length;
      p->magic = NULL;	/* decommission */
      p = q;
    }
  }
/* 
   Try to merge with next higher free area. 
*/
  q = FROMADJ((TOADJ(p))->q_forw);

  if (q != FROMADJ((&shm_ptr->adjhead))) {
/* 
   Upper segment can be merged. 
*/
    DEBUG2(
      ASSERT(q > p, "\nfree: Addr out of order during merge of free area.\n");
    );
    if (q->magic == MAGIC_FREE && (char *)p + p->length == (char *)q) {
/* 
   Remove upper from bucket chain 
*/
      remque(TOBUK(q)); 
/* 
   Remove upper from adjacency chain. 
*/
      remque(TOADJ(q));

      p->length += q->length;
      q->magic = NULL;		/* decommission 		    */
    }
  }
  p->magic = MAGIC_FREE;
/* 
   Place in bucket chain. 
*/
  insque(TOBUK(p),&(shm_ptr->buckets[mlindx(p->length)]));
/*
   If free storage is at the end of memory convert to unused.
*/
  mlfree_end();
/*
   End write synchronization for free shared memory operation.
*/
  ids_end_write();
  check_status("ism_get_shared_memory:");
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_free_shared_memory:  exiting.\n");
  );
  return;
}                                         /* end ism_free_shared_memory	    */

/*--------------------------------------------------------------------------*\
|  Name: mlfree_end
| 
|  Description:  
|     Internal routine to convert free storage at the end of memory to  
|     unused.
\*--------------------------------------------------------------------------*/
void mlfree_end()
{
  register overhead *p;

  p = FROMADJ(shm_ptr->adjhead.q_back);
/* 
   If area is free & at end of memory return it to unused.
*/
  if (p->magic == MAGIC_FREE &&
               (char*)p + p->length == (char *)(shm_ptr->unused_start_addr)) {
    p->magic = NULL;			/* decommission just in case	    */
/* 
   Remove from end of adjacency chain.
*/
    remque(TOADJ(p));
/* 
   Remove from bucket chain. 
*/
    remque(TOBUK(p));
/* 
   Set start of unused to address of freed memory.
*/
    shm_ptr->unused_start_addr = (char *)p;
  }
/*
   Return.
*/
  return;
}                                         /* end ism_free_shared_memory	    */

/*--------------------------------------------------------------------------*\
|  Name: ism_cleanup
| 
|  Description:  
|     Detaches process from shared memory.
\*--------------------------------------------------------------------------*/
void ism_cleanup()
{                                         /* local type definitions	    */
  int rc;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_cleanup:  call.\n");
  );
/*
   Detach process from shared memory.
*/
  rc = shmdt(SHM_START_ADDR);
  check_error(
    rc,
    "ism_cleanup:  error during shmdt",
    system_operation_failed
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_cleanup:  exiting.\n");
  );
  return;
}                                         /* end ism_cleanup 		    */

/*--------------------------------------------------------------------------*\
|  Name: ism_print_allocated_memory
| 
|  Description:  
|     Creates a shared memory node and returns a handle to that node.
\*--------------------------------------------------------------------------*/
void ism_print_allocated_memory()
{ 					/* local type definitions	    */
  register qelem *p, *q;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(
    fprintf(stderr, "ism_print_allocated_memory: call.\n");
  );
/*
   Go down adjacency chain and allocated memory node.
*/
  p = (shm_ptr->adjhead).q_forw;

  DEBUG3(
    fprintf(
      stderr, 
      "ism_print_allocated_memory: (shm_ptr->adjhead).q_forw = %x.\n", 
      (shm_ptr->adjhead).q_forw
    );
    fprintf(
      stderr, 
      "ism_print_allocated_memory: (qelem *)&(shm_ptr->adjhead) = %x.\n", 
      (qelem *)&(shm_ptr->adjhead)
    );
  );

  while (p != (qelem *)&(shm_ptr->adjhead)) {

    fprintf(
      stderr,
      "ism_print_allocated_memory: node = %x",      
      (char *)p + sizeof(overhead)
    );

    if (((overhead *)p)->magic == MAGIC_BUSY) {
      fprintf(stderr, " is in use.\n");
    }
    else {
      fprintf(stderr, " is free.\n");
    }

    fflush(stderr);

    p = p->q_forw;

  DEBUG3(
    fprintf(
      stderr, 
      "ism_print_allocated_memory: p->q_forw = %x.\n", 
      p->q_forw
    );
  );

  }
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_print_allocated_memory:  exiting.\n");
  );
  return;
}				/* end ism_print_allocated_memory	    */

/*--------------------------------------------------------------------------*\
|  Name: ism_destroy
| 
|  Description:  
|     Removes the shared memory identifier from the system and destroys
|     the shared memory segment and associated data structures.
\*--------------------------------------------------------------------------*/
void ism_destroy()
{                                         /* local type definitions	    */
  int rc;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ism_destroy:  call.\n");
  );
/*
   If DEBUG2, print out all the shared memory which is still 
   allocated.
*/
  DEBUG2(
    ism_print_allocated_memory();
  );
/*
   Destroy shared memory structure.
*/
  rc = shmctl(shared_memory_id, IPC_RMID, NULL);
  check_error(
    rc, 
    "ism_destroy:  error during shmctl", 
    system_operation_failed
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ism_destroy:  exiting.\n");
  );
  return;
}                                         /* end ism_destroy 		    */
