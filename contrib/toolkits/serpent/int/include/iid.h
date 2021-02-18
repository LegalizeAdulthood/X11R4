/*      $Header: /u/04c/mnt/integ/int/include/RCS/iid.h,v 1.4 89/10/05 17:19:54 rcs Exp $ */

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
|     $Log:	iid.h,v $
 * Revision 1.4  89/10/05  17:19:54  rcs
 * added routines to convert from strings to ids although
 * i don't think anyone will ever use them
 * 
 * Revision 1.3  89/02/09  18:41:05  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  14:11:26  rcs
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

#ifndef iid

/*--------------------------------------------------------------------------*\
|  Indludes:  iid
| 
|  Description:  
|     Necessary includes for iid abstraction.
| 
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "ism.h"                /* shared memory abstraction                */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: iid_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef iid_main
  int iid_debug = 1;
#else
  extern int iid_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: iid_counter
| 
|  Description:  
|     Counter type.
| 
\* Definition:                                                              */

	typedef long *iid_counter;
/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: iid_id_type
| 
|  Description:  
|     ID type.
| 
\* Definition:                                                              */

	typedef long iid_id_type;
/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constant: iid_null_id
| 
|  Description:  
|     Null, undefined or unavailable id value.
| 
\* Definition:                                                              */

#define iid_null_id (iid_id_type)-1

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_create_counter
| 
|  Description:  
|     Allocates space for the ID counter in shared memory and returns a
|     pointer to the counter.
| 
\* Syntax:                                                                  */

	iid_counter iid_create_counter();

/* 
|  Parameters:
|     none.
|
|  Returns:  
|     A handle to the ID counter in shared memory. 
|  
|  Statuses:  
|     ok, out_of_shared_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_init
| 
|  Description:  
|     Intializes each process which needs to allocate id's.
| 
\* Syntax:                                                                  */

	void iid_init();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_create_id
| 
|  Description:  
|     The send transaction routine sends a transaction handle to a mailbox.
|     This enables a receiveing process to access the transaction.
| 
\* Syntax:                                                                  */

	iid_id_type iid_create_id();

/*
|  Parameters:
|     mailbox - Mailbox to receive message.
|     transaction - Transaction handle.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_equal
| 
|  Description:  
|     The equal routine turns true if the specified id's are identical.
| 
\* Syntax:                                                                  */

	boolean iid_equal(
	  /* first  : in iid_id_type */
	  /* second : in iid_id_type */
	);

/* 
|  Parameters:
|     first - first id.
|     second - second id.
|
|  Returns:  
|     True if first id is equal to the second, false if not.
|  
|  Statuses:  
|     ok, invalid_id.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_after
| 
|  Description:  
|     The after routine turns true if the first id was created after the
|     second id.
| 
\* Syntax:                                                                  */

	boolean iid_after(
	  /* first  : in iid_id_type */
	  /* second : in iid_id_type */
	);

/* 
|  Parameters:
|     first - first id.
|     second - second id.
|
|  Returns:  
|     True if first id comes after the second, false if not.
|  
|  Statuses:  
|     ok, invalid_id.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_before
| 
|  Description:  
|     The before routine turns true if the first id was created before the
|     second id.
| 
\* Syntax:                                                                  */

	boolean iid_before(
	  /* first  : in iid_id_type */
	  /* second : in iid_id_type */
	);

/* 
|  Parameters:
|     first - first id.
|     second - second id.
|
|  Returns:  
|     True if first id comes before second, false if not.
|  
|  Statuses:  
|     ok, invalid_id.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_string_to_id
| 
|  Description:
|     The iid_string_to_id routine converts a string representation of
|     an id to an actual id.
| 
\* Syntax:                                                                  */

	iid_id_type iid_string_to_id(
	  /* string_id  : in string */
	);

/* 
|  Parameters:
|     string_id - string version of the id.
|
|  Returns:  
|     Real ID Value.
|  
|  Statuses:  
|     ok.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_cleanup
| 
|  Description:  
|     Peforms necessary cleanup of the id abstraction.
| 
\* Syntax:                                                                  */

	void iid_cleanup();

/* 
|  Parameters:
|     none.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: iid_destroy_counter
| 
|  Description:  
|     Destroys the counter in shared memory.
| 
\* Syntax:                                                                  */

	void iid_destroy_counter();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/
#define iid
#endif

