static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/iid.c,v 1.4 89/10/05 17:22:00 rcs Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  ID Abstraction (iid)
|
|  Description:  
|     The ID abstraction generates system unique ids for use in identifying
|     instances of shared data elements.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     iid.c - contains the implementation for the process communcation 
|             abstraction.
|     iid.h - contains the external definition for the process communcation
|             abstraction.
|     iidtest.c - test file.
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
|     $Log:	iid.c,v $
 * Revision 1.4  89/10/05  17:22:00  rcs
 * added routines to convert from strings to ids although
 * i don't think anyone will ever use them
 * 
 * Revision 1.3  89/02/09  19:02:18  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  14:08:33  rcs
 * moved debug variable to include file - rCs
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

#define iid_main
#include "iid.h"                /* id abstraction                           */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(iid_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(iid_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(iid_debug, statement)

/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- Retained Data ---------------------------------------*/

iid_counter counter;

/*--------------------------------------------------------------------------*\
|  Name: iid_create_counter
| 
|  Description:  
|     Allocates space for the ID counter in shared memory and returns a
|     pointer to the counter.
\*--------------------------------------------------------------------------*/
iid_counter iid_create_counter()
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_create_counter: call.\n");
  );
/*
   Allocate shared memory for counter.
*/
  counter = (iid_counter)ism_get_shared_memory(sizeof(long));
  rcheck_status("iid_create_counter: status %d from ism_get_shm.\n", NULL);
/*
   Initialize counter to one.
*/
  *counter = 1;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_create_counter: counter = %d.\n", counter);
    fprintf(stderr, "iid_create_counter: exiting.\n");
  );
  return counter;
}                                       /* end iid_create_counter	    */

/*--------------------------------------------------------------------------*\
|  Name: iid_init
| 
|  Description:  
|     Intializes each process which needs to allocate id's.
\*--------------------------------------------------------------------------*/
void iid_init()
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_init: call.\n");
  );
/*
   Get pointer to counter in shared data area.
*/
  counter = (iid_counter)atoi(getenv("ID_HAND"));    
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_init: exiting.\n");
  );
  return;
}                                       /* end iid_init			    */

/*--------------------------------------------------------------------------*\
|  Name: iid_create_id
| 
|  Description:  
|     Creates a unique id.
\*--------------------------------------------------------------------------*/
iid_id_type iid_create_id()
{					/* local variable definitions       */
iid_id_type id;
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_create_id: call.\n");
  );
/*
   Write synchronize create id operation.
*/
  ids_start_write();
  rcheck_status("ism_create_id:", iid_null_id);
/*
   Get the current id and then increment the counter.
*/
  id = *counter;
  (*counter)++;
  if (*counter == 0) {			/* overflow condition		    */
    set_status(overflow);
    DEBUG1(
      isc_print_status("iid_create_id:");
    );
    return(iid_null_id);
  }					/* end if overflow condition	    */
/*
   End write synchronization for create id.
*/
  ids_end_write();
  rcheck_status("ism_create_id:", iid_null_id);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_create_id: id = %d.\n", id);
    fprintf(stderr, "iid_create_id: exiting.\n");
  );
  return id;
}                                       /* end iid_create_id		    */

/*--------------------------------------------------------------------------*\
|  Name: iid_equal
| 
|  Description:  
|     The equal routine returns true if the ids are equal.
\*--------------------------------------------------------------------------*/
boolean iid_equal(first, second)
iid_id_type first, second;
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_equal: call.\n");
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_equal: iid_equal = %d.\n", first == second);
    fprintf(stderr, "iid_equal: exiting.\n");
  );
  return (first == second);
}                                       /* end iid_equal		    */

/*--------------------------------------------------------------------------*\
|  Name: iid_after
| 
|  Description:  
|     The after routine turns true if the first id was created after the
|     second id.
\*--------------------------------------------------------------------------*/
boolean iid_after(first, second)
iid_id_type first, second;
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_after: call.\n");
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_after: iid_after = %d.\n", first > second);
    fprintf(stderr, "iid_after: exiting.\n");
  );
  return (first > second);
}                                       /* end iid_after		    */

/*--------------------------------------------------------------------------*\
|  Name: iid_before
| 
|  Description:  
|     The before routine turns true if the first id was created before the
|     second id.
\*--------------------------------------------------------------------------*/
boolean iid_before(first, second)
iid_id_type first, second;
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_before: call.\n");
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_before: iid_before = %d.\n", first < second);
    fprintf(stderr, "iid_before: exiting.\n");
  );
  return (first < second);
}                                       /* end iid_before		    */

/*--------------------------------------------------------------------------*\
|  Name: iid_string_to_id
| 
|  Description:  
|     The iid_string_to_id routine converts a string representation of
|     an id to an actual id.
\*--------------------------------------------------------------------------*/
iid_id_type iid_string_to_id(string_id)
string string_id;
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_string_to_id: call.\n");
    fprintf(stderr, "iid_string_to_id: string_id = %s.\n", string_id);
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_string_to_id: value = %d.\n", atoi(string_id));
    fprintf(stderr, "iid_string_to_id: exiting.\n");
  );
  return (atoi(string_id));
}                                       /* end iid_string_to_id		    */

/*--------------------------------------------------------------------------*\
|  Name: iid_cleanup
| 
|  Description:  
|     Peforms necessary cleanup of the id abstraction.
\*--------------------------------------------------------------------------*/
void iid_cleanup()
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_cleanup: call.\n");
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_cleanup: exiting.\n");
  );
  return;
}                                       /* end iid_cleanup		    */

/*--------------------------------------------------------------------------*\
|  Name: iid_destroy_counter
| 
|  Description:  
|     Destroys the counter in shared memory.
\*--------------------------------------------------------------------------*/
void iid_destroy_counter()
{					/* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "iid_destroy_counter: call.\n");
  );
/*
   Free the storage allocated for the id counter in shared memory.
*/
  ism_free_shared_memory(counter);
  check_status("iid_create_counter: status %d from ism_free_shm.\n");
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "iid_destroy_counter: exiting.\n");
  );
  return;
}                                       /* end iid_destroy_counter	    */

