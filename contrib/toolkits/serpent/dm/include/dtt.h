	/* $Header: /u/04c/mnt/integ/dm/include/RCS/dtt.h,v 1.2 89/02/09 16:55:42 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  Transaction Table (dtt)
|
|  Description:  
|     The transaction table provides a mapping between the mailbox
|     associated with a particular process and the currently opened
|     transaction for that process.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     dtt.c - contains the implementation for the transactionmanager.
|     dtt.h - contains the external definition for the transaction manager.
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
$Log:	dtt.h,v $
 * Revision 1.2  89/02/09  16:55:42  ejh
 * added copyright notice
 * 
|     June 10, 1988 : Module created.
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

#ifndef dtt
#define dtt

/*--------------------------------------------------------------------------*\
|  Includes: isd
|
|  Description:
|     The following is a list of all the necessary includes for the isd
|     abstraction.
|
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <sys/types.h>          /* System types                             */

#define memoryPack              /* use memoryPack rather than mallocPack    */
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "isd.h"		/* interface shared data abstraction	    */
#include "cus.h"		/* compiler symbol table management package */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: dtt_debug
|
|  Description:
|     Debug variable for the transaction table.
|
\* Definition:                                                              */

#ifdef dtt_main
  int dtt_debug = 3;
#else
  extern int dtt_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dtt_init
| 
|  Description:  
|     Performs necessary intialization of the transaction table.
| 
\* Syntax:                                                                  */

        void dtt_init();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dtt_transaction_tell
| 
|  Description:  
|      Updates the transaction table with a new transaction for a specific 
|      mailbox.
| 
\* Syntax:                                                                  */

        void dtt_transaction_tell(
          /* mailbox : in ipc_mailbox  */
          /* transaction : in isd_trans  */
        );

/* 
|  Parameters:
|     mailbox - Mailbox for which current transaction has changed.
|     transaction - New current transaction.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_transaction_handle, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dtt_transaction_ask
| 
|  Description:  
|     Gets the transaction associated with a mailbox from the transaction 
|     table.
| 
\* Syntax:                                                                  */

        isd_trans dtt_transaction_ask(
          /* mailbox : in ipc_mailbox  */
        );

/* 
|  Parameters:
|     mailbox - Mailbox for which to return the current transaction.
|
|  Returns:  
|     The current transaction for the specified mailbox.
|  
|  Statuses:  
|     ok, not_found.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dtt_cleanup
| 
|  Description:  
|      Performs necessary cleanup of the transaction table.
|     
\* Syntax:                                                                  */

        void dtt_cleanup();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/
#endif
