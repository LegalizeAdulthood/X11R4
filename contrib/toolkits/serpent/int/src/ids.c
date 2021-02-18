static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/ids.c,v 1.6 89/02/09 19:01:30 ejh Exp $";

/*--------------------------------------------------------------------------*
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
|     $Log:	ids.c,v $
 * Revision 1.6  89/02/09  19:01:30  ejh
 * added copyright notice
 * 
 * Revision 1.5  89/01/23  11:32:39  rcs
 * added code to support architectural dependecies on short and int
 * 
 * Revision 1.4  88/11/02  17:41:23  rcs
 * changed short back to int since this did not work on microvax
 * 
 * Revision 1.3  88/11/02  14:23:18  rcs
 * moved debug variable to include file - rCs
 * 
 * Revision 1.2  88/10/29  18:33:49  rcs
 * replace int with short for sun compatability - rCs
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

#define ids_main
#include "ids.h"                          /* data synch			    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(ids_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(ids_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(ids_debug, statement)

#define SEMAPHORE_KEY IPC_PRIVATE
#define NUMBER_OF_SEMAPHORES 3
#define SEMAPHORE_FLAG 0777

#define RD_INIT_ELEMENTS 3
#define RD_DONE_ELEMENTS 1
#define WRT_REQ_ELEMENTS 1
#define WRT_INIT_ELEMENTS 3
#define WRT_DONE_ELEMENTS 1

/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- Retained Data ---------------------------------------*/

/* 
   68000 architecture requires a 'short' and vax requires an 'int'.
*/
#ifdef mc68000
short
#endif

#ifdef vax
int
#endif

    rd_init[RD_INIT_ELEMENTS*3] = { 	/* read initialization semop	    */
      1, 0, 0,
      2, 0, 0,
      0, 1, SEM_UNDO
    },

    rd_done[RD_DONE_ELEMENTS*3] = { 	/* read done semop		    */
      0, -1, SEM_UNDO
    },

    wrt_req[WRT_REQ_ELEMENTS*3] = {	/* write request semop		    */
      2, 1, 0
    },

    wrt_init[WRT_INIT_ELEMENTS*3] = { 	/* write initialization semop	    */
      0,  0, 0,
      1,  1, SEM_UNDO,
      2, -1, SEM_UNDO
    },

    wrt_done[WRT_DONE_ELEMENTS*3] = { 	/* write done semop		    */
      1, -1, SEM_UNDO
    };

int semaphore_id, rc;

/*--------------------------------------------------------------------------*\
|  Name: ids_create
| 
|  Description:  
|     Create a semaphore set and return the id.
\*--------------------------------------------------------------------------*/
int ids_create()
{                                         /* local type definitions         */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_create:  called.\n");
  );
/*
   Create semaphore.
*/
  semaphore_id = semget(SEMAPHORE_KEY, NUMBER_OF_SEMAPHORES, SEMAPHORE_FLAG);
  check_error(semaphore_id,"ids_create error",system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_create:  semaphore id = %d.\n", semaphore_id);
    fprintf(stderr, "ids_create:  exiting.\n");
  );
  return semaphore_id;
}                                         /* end ids_create                 */

/*--------------------------------------------------------------------------*\
|  Name: ids_init
| 
|  Description:  
|     Initializes semaphore id for ids instances of child processes.
\*--------------------------------------------------------------------------*/
void ids_init()
{                                         /* local type definitions         */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_init called.\n");
  );
/*
   Initialize semaphore id from environment variable.
*/
  semaphore_id = atoi(getenv("SEM_ID"));
  DEBUG3(
    fprintf(stderr, "ids_init:  semaphore_id = %d.\n", semaphore_id);
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_init exiting.\n");
  );
  return;
}                                         /* end ids_init                   */

/*--------------------------------------------------------------------------*\
|  Name: ids_start_read
| 
|  Description:  
|     Start of read transaction.
\*--------------------------------------------------------------------------*/
void ids_start_read()
{                                         /* local type definitions         */
  int status;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_start_read call.\n");
  );
/*
   Perform read init sempahore operation.
*/
  DEBUG3(
    fprintf(stderr, "ids_start_read:  semaphore_id = %d.\n", semaphore_id);
  );
  status = semop(semaphore_id, rd_init, RD_INIT_ELEMENTS);
  check_error(status,"ids_start_read error",system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_start_read exiting.\n");
  );
  return;
}                                         /* end ids_start_read             */

/*--------------------------------------------------------------------------*\
|  Name: ids_end_read
| 
|  Description:  
|     End of read transaction.
\*--------------------------------------------------------------------------*/
void ids_end_read()
{                             		/* local type definitions         */
  int status;
/*
   Initialization.
*/
  set_status(ok); 			/* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_end_read call.\n");
  );
/*
   Perform read done semaphore operation.
*/
  status = semop(semaphore_id, rd_done, RD_DONE_ELEMENTS);
  check_error(status,"ids_end_read error",system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_end_read exiting.\n");
  );
  return;
}                                         /* end ids_end_read               */

/*--------------------------------------------------------------------------*\
|  Name: ids_start_write
| 
|  Description:  
|     Start of write transaction.
\*--------------------------------------------------------------------------*/
void ids_start_write()
{                                         /* local type definitions         */
  int status;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_start_write call.\n");
  );
/*
   Perform write request semaphore operation.
*/
  status = semop(semaphore_id, wrt_req, WRT_REQ_ELEMENTS);
  check_error(status,"ids_start_write error",system_operation_failed);
/*
   Perform write init semaphore operation.
*/
  status = semop(semaphore_id, wrt_init, WRT_INIT_ELEMENTS);
  check_error(status,"ids_write_init error",system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_start_write exiting.\n");
  );
  return;
}                                         /* end ids_start_write            */

/*--------------------------------------------------------------------------*\
|  Name: ids_end_write
| 
|  Description:  
|     End of write transaction.
\*--------------------------------------------------------------------------*/
void ids_end_write()
{                                         /* local type definitions         */
  int status;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_end_write call.\n");
  );
/*
   Perform write done semaphore operation.
*/
  status = semop(semaphore_id, wrt_done, WRT_DONE_ELEMENTS);
  check_error(status,"ids_end_write error",system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_end_write exiting.\n");
  );
  return;
}                                         /* end ids_end_write              */

/*--------------------------------------------------------------------------*\
|  Name: ids_cleanup
| 
|  Description:  
|     Perform necessary initialization for the data synch abstraction.
\*--------------------------------------------------------------------------*/
void ids_cleanup()
{                                         /* local type definitions         */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_cleanup call.\n");
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_cleanup exiting.\n");
  );
  return;
}                                         /* end ids_init                   */

/*--------------------------------------------------------------------------*\
|  Name: ids_destroy
| 
|  Description:  
|    Destroys semaphore id set.
\*--------------------------------------------------------------------------*/
void ids_destroy()
{                                         /* local type definitions         */
  int status;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ids_destroy call.\n");
  );
/*
   Remove semaphore id.
*/
  status = semctl(semaphore_id, 0, IPC_RMID, 0);
  check_error(status,"ids_destroy error",system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ids_destroy exiting.\n");
  );
  return;
}                                         /* end ids_init                   */









