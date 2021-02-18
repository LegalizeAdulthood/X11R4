/*      $Header: /u/04c/mnt/integ/int/include/RCS/ids.h,v 1.3 89/02/09 18:39:51 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  Data Synchronization (ids)
|
|  Description:  
|     The data syncrhonization provides data synchronization 
|     using monitors.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     ids.c - contains the implementation for the data synch abstraction.
|     ids.h - contains the external definition for the data synch abstraction.
|     idstest.c - test file.
|
|  Project:
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Owner:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     Oct. 19, 1987 : Module created.
|     $Log:	ids.h,v $
 * Revision 1.3  89/02/09  18:39:51  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  14:21:12  rcs
 * moved debug variable from c file - rCs
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

#ifndef ids

/*--------------------------------------------------------------------------*\
|  Includes: ids
| 
|  Description:  
|      Necessary includes for the ids data abstraction.
| 
\* Definition:                                                              */


#include <stdio.h>                        /* C Standard I/O Package         */
#include <errno.h>                        /* Standard Ultrix error numbers  */

#include <sys/types.h>                    /* Unix Semaphores                */

#ifndef IPC_PRIVATE
#  include <sys/ipc.h>
#endif

#include <sys/sem.h>

#include "global.h"                       /* Global type defs and constants */
#include "isc.h"                          /* status code                    */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: ids_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef ids_main
  int ids_debug = 1;
#else
  extern int ids_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_create
| 
|  Description:  
|      Create a semaphore set and return the id.     
| 
\* Syntax:                                                                  */

	int ids_create();

/* 
|  Parameters:
|     none.
|
|  Returns:  
|     semaphore id.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_init
| 
|  Description:  
|     The ids_init routine provides necessary initialization for the data
|     synchronization abstraction.
| 
\* Syntax:                                                                  */

	void ids_init(
	  /* semaphore_id : in int */
	);

/* 
|  Parameters:
|     semaphore_id - created semaphore id.
|
|  Returns:  
|     semaphore id.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_start_read
| 
|  Description:  
|     Start of read transaction.
| 
\* Syntax:                                                                  */

	void ids_start_read();

/* 
|  Parameters:
|     none.
|
|  Returns:  void.
|  
|  Statuses:  ok, system_operation_failed
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_end_read
| 
|  Description:  
|     End of read transaction.
| 
\* Syntax:                                                                  */

	void ids_end_read();

/* 
|  Parameters:
|     none.
|
|  Returns:  void.
|  
|  Statuses:  ok, system_operation_failed
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_start_write
| 
|  Description:  
|     Start of write transaction.
| 
\* Syntax:                                                                  */

	void ids_start_write();

/* 
|  Parameters:
|     none.
|
|  Returns:  void.
|  
|  Statuses:  ok, system_operation_failed
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_end_write
| 
|  Description:  
|     End of write transaction.
| 
\* Syntax:                                                                  */

	void ids_end_write();

/* 
|  Parameters:
|     none.
|
|  Returns:  void.
|  
|  Statuses:  ok, system_operation_failed
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ids_cleanup
| 
|  Description:  
|     Perform necessary cleanup for the data synch abstraction.
| 
\* Syntax:                                                                  */

	void ids_cleanup();

/* 
|  Parameters:
|     none.
|
|  Returns:  void.
|  
|  Statuses:  ok, system_operation_failed
|  
\*--------------------------------------------------------------------------*/
#endif
#define ids
