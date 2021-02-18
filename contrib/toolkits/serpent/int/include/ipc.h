/*      $Header: /u/04c/mnt/integ/int/include/RCS/ipc.h,v 1.3 89/02/09 18:43:28 ejh Exp $ */
/*--------------------------------------------------------------------------*\
|
|  Name:  Process Communciation Abstraction (ipc)
|
|  Description:  
|     The process communication abstraction of the interface layer 
|     provides interprocess communcation within SERPENT and between
|     the application and the SERPENT system.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     ipc.c - contains the implementation for the process communcation 
|             abstraction.
|     ipc.h - contains the external definition for the process communcation
|             abstraction.
|     ipctest.c - test file.
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
|     $Log:	ipc.h,v $
 * Revision 1.3  89/02/09  18:43:28  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  14:40:57  rcs
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

#ifndef ipc

/*--------------------------------------------------------------------------*\
|  Includes: ipc
| 
|  Description:  
|     The following includes are required by the ipc component.
| 
\* Definition:                                                              */

#include <stdio.h>                        /* C Standard I/O Package         */
#include <errno.h>                        /* Standard Ultrix error numbers  */

#include <sys/types.h>                    /* Unix MsgOps                    */

#ifndef IPC_PRIVATE
#  include <sys/ipc.h>
#endif

#include <sys/msg.h>

#include "global.h"                       /* Global type defs and constants */
#include "isc.h"                          /* status code                    */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: ipc_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef ipc_main
  int ipc_debug = 1;
#else
  extern int ipc_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: ipc_mailbox
| 
|  Description:  
|     Mailbox handle.
| 
\* Definition:                                                              */
	typedef private ipc_mailbox;
/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_create
| 
|  Description:  
|     Creates a mailbox and returns a handle to that mailbox.
| 
\* Syntax:                                                                  */

	ipc_mailbox ipc_create();

/* 
|  Parameters:
|     none.
|
|  Returns:  
|     A handle to a newly created mailbox.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_init
| 
|  Description:  
|     Determines the mailbox handle associated with the mailbox name.
| 
\* Syntax:                                                                  */

	ipc_mailbox ipc_init(
	  /* mailbox : in out string */
	);

/* 
|  Parameters:
|     mailbox - Mailbox environment variable name.
|
|  Returns:  
|     The mailbox handle.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_send
| 
|  Description:  
|     The send transaction routine sends a message to a specified receiver
|     from a specified sender.
| 
\* Syntax:                                                                  */

	void ipc_send(
	  /* receiver : in ipc_mailbox */
	  /* sender   : in ipc_mailbox */
	  /* message  : in char *      */
	);

/*
|  Parameters:
|     receiver - Mailbox handle of receiving process.
|     sender - Mailbox handle of sending process.
|     message - Message.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_receive
| 
|  Description:  
|     Wait for the next message available for the receiver.
| 
\* Syntax:                                                                  */

	char *ipc_receive(
	  /* receiver : in  ipc_mailbox */
	  /* sender   : out ipc_mailbox */
	);

/* 
|  Parameters:
|     receiver - Mailbox handle of receiving process.
|     sender - Mailbox handle of sending process.
|
|  Returns:  
|     Sent message.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_receive_no_wait
| 
|  Description:  
|     Receive a message for the specified receiver.  If no message is
|     available status is set to not_available.
| 
\* Syntax:                                                                  */

	char *ipc_receive_no_wait(
	  /* receiver : in  ipc_mailbox */
	  /* sender   : out ipc_mailbox */
	);

/* 
|  Parameters:
|     receiver - Mailbox handle of receiving process.
|     sender - Mailbox handle of sending process.
|
|  Returns:  
|     Sent message.
|  
|  Statuses:  
|     ok, not_available, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_receive_from_sender
| 
|  Description:  
|     The receive from sender routine receives a message for a specified
|     receiver from a specified sender.  If no message is
|     available status is set to not_available.
| 
\* Syntax:                                                                  */

	char *ipc_receive_from_sender(
	  /* receiver : in ipc_mailbox */
	  /* sender   : in ipc_mailbox */
	);

/* 
|  Parameters:
|     receiver - Mailbox handle of receiving process.
|     sender - Mailbox handle of sending process.
|
|  Returns:  
|     Sent message.
|  
|  Statuses:  
|     ok, not_available, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_cleanup
| 
|  Description:  
|     Peforms necessary cleanup of the process communication abstraction.
| 
\* Syntax:                                                                  */

	void ipc_cleanup();

/* 
|  Parameters:
|     none.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_destroy
| 
|  Description:  
|     Destroys an existing mailbox and reallocates all system resources
|     associated with the mailbox.
| 
\* Syntax:                                                                  */

	void ipc_destroy(
	  /* mailbox: in out ipc_mailbox */
	);

/* 
|  Parameters:
|     mailbox - Mailbox to be destroyed.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/
#define ipc
#endif
