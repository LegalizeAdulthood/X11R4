static char rcsid [] =
        "$Header: /u/04c/mnt/integ/utilities/src/RCS/ubo.c,v 1.9 89/10/05 14:37:47 rcs Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  Buffer Ops Utility  (ubo)
|
|  Description:  
|     The buffer ops utility provides basic buffer operations on buffers.
|
|  Component of:
|     Utilities Layer.
|
|  Related files:
|     ubo.c - contains the implementation for the transactionmanager.
|     ubo.h - contains the external definition for the transaction manager.
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
|  Project Leader:
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     June 10, 1988 : Module created.

$Log:	ubo.c,v $
 * Revision 1.9  89/10/05  14:37:47  rcs
 * made mods to support typed buffesr
 * modified ubo_create_buffer to initialize fields
 * 
 * Revision 1.8  89/09/27  16:27:20  bmc
 * Added ubo_create_buffer().
 * 
 * Revision 1.7  89/02/10  14:58:32  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/01/30  14:30:07  rcs
 * fixed bug in buffcmp in checking for undefined
 * modified buffcpy to support 0 length buffers
 * 
 * Revision 1.5  89/01/17  17:02:40  rcs
 * fixed problems detected by Saber-C
 * 
 * Revision 1.4  88/12/19  17:31:27  rcs
 * fixed buffcpy and buffcmp to handle undefined buffers
 * 
 * Revision 1.3  88/10/07  16:53:49  little
 * not modified.
 * 
 * Revision 1.2  88/10/07  15:52:47  little
 * fix ubo_buffcpy to copy correctly. add comment & log comments.
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

#define ubo_main
#include "ubo.h"                          /* process communication	    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(ubo_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(ubo_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(ubo_debug, statement)

/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- Retained Data ---------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ubo_buffcmp
| 
|  Description:  
|     Compares two existing buffers and returns true if equal or false if 
|     not equal.
\*--------------------------------------------------------------------------*/
boolean ubo_buffcmp(buff1, buff2)
idd_buffer_type *buff1;
idd_buffer_type *buff2;
{ 					/* local type definitions	    */
  int i;

  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "ubo_buffcmp: call.\n");
    fprintf(stderr, "ubo_buffcmp: buff1 = %x.\n", buff1);
    fprintf(stderr, "ubo_buffcmp: buff1 = %x.\n", buff2);
  );
/*
   If lengths are different return false.
*/
  if (buff1->length != buff2->length) {		/* different lengths	    */
    return false;
  }						/* end if different lengths */
/*
   If type are different return false.
*/
  if (buff1->type != buff2->type) {		/* different types	    */
    return false;
  }						/* end if different types */
/*
   If both lengths are undefined return true.  
*/
  if ((buff1->length == UNDEFINED_BUFFER_LENGTH) && 
      (buff2->length == UNDEFINED_BUFFER_LENGTH)) {	/* both undefined   */
    return true;
  }					/* end if both buffers undefind	    */
/*
   Else if one buffer is undefined return false.
*/
  else if ((buff1->length == UNDEFINED_BUFFER_LENGTH) ||
        (buff2->length == UNDEFINED_BUFFER_LENGTH)) {	/* one undefined    */
    return false;
  }
/*
   Compare each of the n bytes for equality.
*/
  for (i = 0; i < buff1->length; i++) {	 /* for each byte in buffer body    */

    if ((char)(*(buff1->body + i)) != (char)(*(buff2->body + i))) {   
      return false;
    }					/* end if buffers not equal	    */

  }					/* end for each byte in buffer	    */
/*
   Return true.
*/ 
  DEBUG2(
    fprintf(stderr, "ubo_buffcmp:  exiting.\n");
  );
  return true;
} 					/* end ubo_buffcmp		    */

/*--------------------------------------------------------------------------*\
|  Routine: ubo_buffcpy
| 
|  Description:  
|     Copies the contents of buff2 to buff1.  The buffer structure is
|     assumed to exist but storage for the buffer body will be malloced
|     and any existing pointer overwritten.
\*--------------------------------------------------------------------------*/
void ubo_buffcpy(buff1, buff2)
idd_buffer_type *buff1;
idd_buffer_type *buff2;
{  					/* begin 			    */
/*
   Initialize.
*/
  set_status(ok);			

  DEBUG2(
    fprintf(stderr, "ubo_buffcpy: call.\n");
    fprintf(stderr, "ubo_buffcpy: buff1 = %x.\n", buff1);
    fprintf(stderr, "ubo_buffcpy: buff1->length = %d.\n", buff1->length);
    fprintf(
      stderr, 
      "ubo_buffcpy: buff1->type = %s.\n", 
      idd_get_type_string(buff1->type)
    );
    fprintf(stderr, "ubo_buffcpy: buff1->body = %x.\n", buff1->body);
    fprintf(stderr, "ubo_buffcpy: buff2 = %x.\n", buff2);
    fprintf(stderr, "ubo_buffcpy: buff2->length = %d.\n", buff2->length);
    fprintf(
      stderr, 
      "ubo_buffcpy: buff2->type = %s.\n", 
      idd_get_type_string(buff2->type)
    );
    fprintf(stderr, "ubo_buffcpy: buff2->body = %x.\n", buff2->body);
  );
/* 
   Assign buff2 length to buff2 length.  
*/
  buff1->length = buff2->length;
  buff1->type = buff2->type;
  buff1->body = NULL;
/* 
   If buffer defined and of non-zero length allocate storage for buff1 body.
*/
  if ((buff1->length != UNDEFINED_BUFFER_LENGTH) && 	/* buff defined	    */
      (buff1->length != 0)) {			/* and of non-zero length   */
/*
   Allocate storage for buffer.
*/
    buff1->body = make_node(buff2->length);
    check_null(
      buff1->body,
      "ubo_buffcpy: bad status from make_node of buff1 body.\n",
      out_of_memory
    );
/* 
   CAUTION : memcpy only works for non-overlapping regions 
*/
    (void)memcpy(buff1-> body, buff2-> body, buff1->length);
  }					/* end if buffer defined	    */

  DEBUG2(
    fprintf(stderr, "ubo_buffcpy:  exiting.\n");
  );

  return;
} 					/* end ubo_buffcpy		    */

/*--------------------------------------------------------------------------*\
|  Routine: ubo_create_buffer
| 
|  Description:  
|     Creates storage for a buffer of undefined length.
\*--------------------------------------------------------------------------*/
idd_buffer_type *ubo_create_buffer ()
{
    idd_buffer_type *the_buffer;


    set_status (ok);

    the_buffer = (idd_buffer_type *) make_node (sizeof (idd_buffer_type));
    rcheck_null
	(
	the_buffer,
	"ubo_create_buffer: can't allocate memory for buffer structure.",
	out_of_memory,
	(idd_buffer_type *) NULL
	);

    the_buffer->length = UNDEFINED_BUFFER_LENGTH;
    the_buffer->type = idd_undefined;
    the_buffer->body = NULL;

    return (the_buffer);
}					/* end ubo_create_buffer 	    */

/*--------------------------------------------------------------------------*\
|  Routine: ubo_destroy_buffer
| 
|  Description:  
|     Destroys an existing buffer.
\*--------------------------------------------------------------------------*/
void ubo_destroy_buffer(buff)
idd_buffer_type *buff;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "ubo_destroy_buffer: call.\n");
    fprintf(stderr, "ubo_destroy_buffer: buff = %x.\n", buff);
  );
/*
   Free storage for buffer body if it exists.
*/
  if (buff->body != NULL) {
    free_node(buff->body);
  }
/*
   Free storage for buffer.
*/
  free_node(buff);
/*
   Return.
*/ 
  DEBUG2(
    fprintf(stderr, "ubo_destroy_buffer:  exiting.\n");
  );
  return;
} 					/* end ubo_destroy_buffer	    */
