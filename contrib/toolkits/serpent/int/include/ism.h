/*      $Header: /u/04c/mnt/integ/int/include/RCS/ism.h,v 1.3 89/02/09 18:49:25 ejh Exp $ */

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
|    Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|    Author:  
|	Robert C. Seacord
|	rcs@sei.cmu.edu  
|
|    Change History:
|	Oct. 19, 1987 : Module created.
|       $Log:	ism.h,v $
 * Revision 1.3  89/02/09  18:49:25  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  14:51:45  rcs
 * moved debug variable from c file
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

#ifndef ism

/*--------------------------------------------------------------------------*\
|  Includes: ism
|
|  Description:
|     Required includes for ism abstraction.
|
\* Definition:                                                              */

#include <stdio.h>                        /* C Standard I/O Package         */
#include <errno.h>                        /* Standard Ultrix error numbers  */

#include <sys/types.h>                    /* Unix Shared Memory Ops         */

#ifndef IPC_PRIVATE
#  include <sys/ipc.h>
#endif

#include <sys/shm.h>

#include "global.h"                       /* Global type defs and constants */
#include "isc.h"                          /* status code                    */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: ism_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef ism_main
  int ism_debug = 1;
#else
  extern int ism_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constant: external names
|
|  Description:
|     The following constants define external names which can be used
|     to invoke shared memory functions.  These names are consistent with
|     those provided in the memoryPack utility.
|
\* Definition:                                                              */

#define SHM_NODE ism_node
#define make_shm_node ism_get_shared_memory
#define free_shm_node ism_free_shared_memory
#define copy_shm_node ism_copy_shared_memory

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constant: Null Shared Memory Node
|
|  Description:
|     Define NULL pointer.
|
\* Definition:                                                              */

#define NULL_SHM_NODE ((SHM_NODE) 0)

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: ism_node
|
|  Description:
|     Handle to a node.
|
\* Definition:                                                              */

        typedef private ism_node;

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_create
| 
|  Description:  
|     Allocates 1M of storage for shared memory starting from 10M - 11M in
|     process memory.
|
\* Syntax:                                                                  */

       int ism_create();

/*
|  Parameters:
|     none.
|
|  Returns:
|     Shared memory ID.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_init
| 
|  Description:  
|     The ism_init routine performs necessary initialization of the shared
|     memory interface.
|
\* Syntax:                                                                  */

       void ism_init();

/*
|  Parameters:
|     none.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_get_shared_memory
| 
|  Description:  
|     Creates a shared memory node and returns a handle to that node.
|
\* Syntax:                                                                  */

      ism_node ism_get_shared_memory(
	/* nbytes : in Size */
      );

/*
|  Parameters:
|     nbytes - Size of memory to allocate in bytes.
|
|  Returns:
|     Pointer to allocated storage.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_copy_shared_memory
| 
|  Description:  
|     Returns a pointer to a shared memory node which is identical to 
|     the original node.
|
\* Syntax:                                                                  */

      ism_node ism_copy_shared_memory(
	/* node : in ism_node */
      );

/*
|  Parameters:
|     node - Shared memory node to copy.
|
|  Returns:
|     Pointer to allocated storage.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_copy_into_shared_memory
|
|  Description:
|     Copies "n" bytes from "user_data" to "shared_memory".
|
\* Syntax:                                                                  */

	void ism_copy_into_shared_memory(
	  /* shared_memory : in out ism_node */
	  /* n             : in     Size     */
	  /* user_data     : in     char *   */
	);

/*
|  Parameters:
|     shared_memory - Pointer to shared memory node.
|     n - Number of bytes to copy.
|     user_data - Pointer to user data.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, invalid_handle.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_copy_from_shared_memory
|
|  Description:
|     Copies "n" bytes from "shared_memory" to "user_data".
|
\* Syntax:                                                                  */

	void ism_copy_from_shared_memory(
	  /* shared_memory : in     ism_node */
	  /* n             : in     Size     */
	  /* user_data     : in out char *   */
	);

/*
|  Parameters:
|     shared_memory - Pointer to shared memory node.
|     n - Number of bytes to copy.
|     user_data - Pointer to user data.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, invalid_handle.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_free_shared_memory
| 
|  Description:  
|     Deallocates an existing shared memory node.
|
\* Syntax:                                                                  */

      void ism_free_shared_memory(
        /* shared_memory : in out ism_node */
      );

/*
|  Parameters:
|     shared_memory - shared memory node to be freed.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_cleanup
| 
|  Description:  
|     Detaches process from shared memory.
|
\* Syntax:                                                                  */

      void ism_cleanup();

/*
|  Parameters:
|     none.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ism_destroy
| 
|  Description:  
|     Removes the shared memory identifier from the system and destroys
|     the shared memory segment and associated data structures.
|
\* Syntax:                                                                  */

      void ism_destroy();

/*
|  Parameters:
|     none.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, system_operation_failed.
|
\*--------------------------------------------------------------------------*/
#endif
#define ism
