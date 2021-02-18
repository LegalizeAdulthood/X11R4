static char rcsid [] =
	"$Header: /u/04c/mnt/integ/int/src/RCS/ipc.c,v 1.4 89/02/09 19:05:42 ejh Exp $";
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
|     June 10, 1988 : change receive_from_sender to wait for trans  ljb
|     $Log:	ipc.c,v $
 * Revision 1.4  89/02/09  19:05:42  ejh
 * added copyright notice
 * 
 * Revision 1.3  88/11/02  15:39:36  rcs
 * *** empty log message ***
 * 
 * Revision 1.2  88/11/02  14:45:41  rcs
 * moved debug variable to interface file - rCs
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

#define ipc_main
#include "ipc.h"                          /* process communication	    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(ipc_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(ipc_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(ipc_debug, statement)

#define IPC_WAIT 0

#define MESSAGE_KEY 5
#define MESSAGE_FLAG 0777
#define TRANSACTION_TYPE 1
#define NEXT_MESSAGE 0

/*-------------------- Type Definitions ------------------------------------*/

typedef struct  {
   long type;				/* message type			    */
   char *text;				/* message text (message) 	    */
} message_buffer;

/*-------------------- Retained Data ---------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ipc_create
| 
|  Description:  
|     Creates a mailbox and returns a handle to that mailbox.
\*--------------------------------------------------------------------------*/
ipc_mailbox ipc_create()
{                                         /* local type definitions	    */
int msg_queue_id;
boolean done = false;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ipc_create: call.\n");
  );
/* 
   Create a message queue.
*/
  while (!done) {			/* while not done		    */

    msg_queue_id = msgget(IPC_PRIVATE, MESSAGE_FLAG);
    rcheck_error(
      msg_queue_id, 
      "ipc_create: error during msgget", 
      system_operation_failed,
      NULL
    );
/*
   Reject zero or negative msg_queue_ids since these can not be used in 
   the mtype field of the msgp structure used in msgsnd.
*/
    if (msg_queue_id > 0) {		/* postive message id		    */
      done = true;
    }

  }					/* end while not done		    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_create: exiting.\n");
    fprintf(stderr, "ipc_create: mailbox = %x.\n", (ipc_mailbox)msg_queue_id);
  );
  return (ipc_mailbox)msg_queue_id;
}                                         /* end ipc_create                 */

/*--------------------------------------------------------------------------*\
|  Name: ipc_init
| 
|  Description:  
|     Gets the mailbox id corresponding to the mailbox name from the
|     environment variable of the same name.
\*--------------------------------------------------------------------------*/
ipc_mailbox ipc_init(mailbox)
string mailbox;
{                                         /* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ipc_init: call.\n");
    fprintf(stderr, "ipc_init: mailbox = %s.\n", mailbox);
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_init: exiting.\n");
    fprintf(
      stderr, 
      "ipc_init: mailbox = %x.\n", (ipc_mailbox)atoi(getenv(mailbox))
    );
  );
  return (ipc_mailbox)atoi(getenv(mailbox));
}                                         /* end ipc_init		    */

/*--------------------------------------------------------------------------*\
|  Name: ipc_send
| 
|  Description:  
|     Send a message to a given mailbox.
\*--------------------------------------------------------------------------*/
void ipc_send(receiver, sender, message)
ipc_mailbox receiver, sender;
char *message;
{                                         /* local type definitions         */
  message_buffer msg;
  int rc;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ipc_send: call.\n");
    fprintf(stderr, "ipc_send: receiver = %x.\n", receiver);
    fprintf(stderr, "ipc_send: sender = %x.\n", sender);
    fprintf(stderr, "ipc_send: message = %x.\n", message);
  );
/*
   Send message.
*/
  msg.type = (long)sender;
  msg.text = message;
  rc = msgsnd(receiver, &msg, sizeof(msg.text), IPC_NOWAIT);
  check_error(rc, "error in ipc_send", system_operation_failed);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_send: exiting.\n");
  );
  return;
}                                         /* end ipc_send       */

/*--------------------------------------------------------------------------*\
|  Name: ipc_receive
| 
|  Description:  
|     Wait for the next message available from a specified mailbox.
\*--------------------------------------------------------------------------*/
char *ipc_receive(receiver, sender)
ipc_mailbox receiver;
ipc_mailbox *sender;
{                                         /* local type definitions         */
  int rc;
  message_buffer message;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ipc_receive: call.\n");
    fprintf(stderr, "ipc_receive: receiver = %x.\n", receiver);
    fprintf(stderr, "ipc_receive: sender = %x.\n", *sender);
  );
/*
   Receive the message.
*/
  rc = msgrcv(
    receiver, 
    &message, 
    sizeof(message.text), 
    NEXT_MESSAGE,			/* take the next message available  */
    IPC_WAIT				/* wait for next message	    */
  );
  rcheck_error(
    rc, 
    "error in ipc_receive_trans", 
    system_operation_failed,
    NULL
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_receive: exiting.\n");
    fprintf(stderr, "ipc_receive: sender = %x.\n", message.type);
    fprintf(stderr, "ipc_receive: message.text = %x.\n", message.text);
  );
  *sender = (ipc_mailbox)message.type;
  return message.text;
}                                         /* end ipc_receive    */

/*--------------------------------------------------------------------------*\
|  Name: ipc_receive_no_wait
| 
|  Description:  
|     Receive a message from a specified mailbox.  If message not available
|     sets status to not_available and returns.
\*--------------------------------------------------------------------------*/
char *ipc_receive_no_wait(receiver, sender)
ipc_mailbox receiver;
ipc_mailbox *sender;
{                                         /* local type definitions         */
  int rc;
  message_buffer message;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ipc_receive_no_wait: call.\n");
    fprintf(stderr, "ipc_receive_no_wait: receiver = %x.\n", receiver);
    fprintf(stderr, "ipc_receive_no_wait: sender = %x.\n", *sender);
  );
/*
   Receive the message.
*/
  rc = msgrcv(
    receiver, 
    &message, 
    sizeof(message.text), 
    NEXT_MESSAGE,
    IPC_NOWAIT
  );
  if (rc == -1) {			/* if error			    */
    if (errno == ENOMSG) {		/* message not available	    */
      DEBUG3(
        perror("ipc_receive_no_wait: not available, ");
      );
      set_status(not_available);
      return(NULL);
    }					/* end if message not available	    */
    else {				/* other error			    */
      DEBUG1(
        perror("ipc_receive_no_wait:  error during msgrcv");
      );
      set_status(system_operation_failed);
      return(NULL);
    }					/* end else error		    */
  }					/* end if error
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_receive_no_wait: exiting.\n");
    fprintf(stderr, "ipc_receive_no_wait: sender = %x.\n", message.type);
    fprintf(
      stderr, 
      "ipc_receive_no_wait: message.text = %x.\n", message.text
    );
  );
  *sender = (ipc_mailbox)message.type;
  return message.text;
}                                         /* end ipc_receive_no_wait    */

/*--------------------------------------------------------------------------*\
|  Name: ipc_receive_from_sender
| 
|  Description:  
|     The receive from sender routine receives a message for a specified
|     receiver from a specified sender.  If no message is
|     available process is suspended.
\*--------------------------------------------------------------------------*/
char *ipc_receive_from_sender(receiver, sender)
ipc_mailbox receiver;
ipc_mailbox sender;
{                                         /* local type definitions         */
  int rc;
  message_buffer message;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "ipc_receive_from_sender: call.\n");
    fprintf(stderr, "ipc_receive_from_sender: receiver = %x.\n", receiver);
    fprintf(stderr, "ipc_receive_from_sender: sender = %x.\n", sender);
  );
/*
   Receive the message.
*/
  rc = msgrcv(
    receiver, 
    &message, 
    sizeof(message.text), 	/* size of message			    */
    sender,			/* receive from this particular sender      */
    IPC_WAIT			
  );
  if (rc == -1) {			/* if error			    */
    if (errno == ENOMSG) {		/* message not available	    */
      DEBUG1(
        perror("ipc_receive_from_sender:  error during msgrcv");
      );
      set_status(not_available);
      return(NULL);
    }					/* end if message not available	    */
    else {				/* other error			    */
      DEBUG1(
        perror("ipc_receive_from_sender:  error during msgrcv");
      );
      set_status(system_operation_failed);
      return(NULL);
    }					/* end else error		    */
  }					/* end if error
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_receive_from_sender: exiting.\n");
    fprintf(stderr, "ipc_receive_from_sender: sender = %x.\n", message.type);
    fprintf(
      stderr, 
      "ipc_receive_from_sender: message.text = %x.\n", message.text
    );
  );
  return message.text;
}                                         /* end ipc_receive_from_sender    */

/*--------------------------------------------------------------------------*\
|  Name: ipc_cleanup
| 
|  Description:  
|     Performs any necessary cleanup of the ipc component.
\*--------------------------------------------------------------------------*/
void ipc_cleanup()
{                                         /* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */

  DEBUG2(
    fprintf(stderr, "ipc_cleanup: call.\n");
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ipc_cleanup: exiting.\n");
  );
  return;
}                                         /* end ipc_cleanup 		    */

/*--------------------------------------------------------------------------*\
|  Name: ipc_destroy
| 
|  Description:  
|     Destroys an existing message queue.
\*--------------------------------------------------------------------------*/
void ipc_destroy(mailbox)
ipc_mailbox mailbox;
{                                         /* local type definitions         */
  int rc;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */

  DEBUG2(
    fprintf(stderr, "ipc_destroy: call.\n");
    fprintf(stderr, "ipc_destroy: mailbox = %x.\n", mailbox);
  );
/*
   Remove msg queue.
*/
  rc = msgctl(mailbox, IPC_RMID, NULL);
  check_error(rc, "error in ipc_destroy", system_operation_failed);
/*
   Return.
*/

  DEBUG2(
    fprintf(stderr, "ipc_destroy: exiting.\n");
  );
  return;
}                                         /* end ipc_init                   */

