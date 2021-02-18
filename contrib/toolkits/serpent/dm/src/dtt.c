static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/dtt.c,v 1.5 89/11/02 16:18:42 little Exp $";
/*--------------------------------------------------------------------------*\
|
|  Name:  Transaction Table  (dtt)
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
|     June 10, 1988 : Module created.
$Log:	dtt.c,v $
 * Revision 1.5  89/11/02  16:18:42  little
 * no changes
 * 
 * Revision 1.4  89/02/09  18:24:11  ejh
 * added copyright notice
 * 
 * Revision 1.3  88/11/04  17:04:48  little
 * change DEBUG1 to print d22_slang_source
 * 
 * Revision 1.2  88/10/03  09:17:45  little
 * add dm shared data.  the dm has no entries in the dtt transaction
 * table.  so anytime somebody asks for an open transacton the
 * NULL returned siginifies that the dm is the target and so no transaction
 * need be added to, we are already in the dm so the changes are  "automatically
 * " made to the dm's shared data.
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

#define dtt_main
#include "dtt.h"                          /* process communication	    */
#include "d22_debug.h"

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) DM_RT_DEBUG1(dtt_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(dtt_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(dtt_debug, statement)

#define MAX_MAIL_HASH 17

/*-------------------- Type Definitions ------------------------------------*/

typedef struct {
  ipc_mailbox mailbox;
  isd_trans transaction;
} current_trans_record;

/*-------------------- Retained Data ---------------------------------------*/

HASH dtt_trans_table;		/* hash table of transaction ids	    */

/*--------------------------------------------------------------------------*\
|  Routine: dtt_match_mailbox
| 
|  Description:  
|     Routine to compare an mailbox with the mailbox in a current transaction
|     record. 
\*--------------------------------------------------------------------------*/
int dtt_match_mailbox(mailbox, current_trans)
ipc_mailbox mailbox;
current_trans_record *current_trans;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "dtt_match_mailbox: call.\n");
    fprintf(stderr, "dtt_match_mailbox: mailboxentifer = %d.\n", mailbox);
    fprintf(
      stderr, 
      "dtt_match_mailbox: mailboxentifer = %x.\n", 
      current_trans
    );
  );

  DEBUG2(
    fprintf(stderr, "dtt_match_mailbox:  exiting.\n");
  );
  return (current_trans->mailbox == mailbox);
} 					/* end dtt_match_mailbox	    */

/*--------------------------------------------------------------------------*\
|  Routine: dtt_hash_mailbox
| 
|  Description:  
|     Internal function which will convert an mailbox into indecies
|     in the array of hash lists.
\*--------------------------------------------------------------------------*/
int dtt_hash_mailbox(mailbox)
ipc_mailbox mailbox;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2 (fprintf(stderr, "dtt_hash_mailbox call.\n"););
/*
   Return a value in the right range.
*/
  DEBUG2 (fprintf(stderr, "dtt_hash_mailbox exiting.\n"););

  return ((((int) mailbox) & 077777) % MAX_MAIL_HASH);
} 					/* end dtt_hash_mailbox		    */

/*--------------------------------------------------------------------------*\
|  Routine: dtt_init
| 
|  Description:  
|     Performs necessary intialization of the object base manager.
\*--------------------------------------------------------------------------*/
void dtt_init()
{                                         /* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "dtt_init: call.\n");
  );
/* 
   Create the mailbox hash table.
*/
  dtt_trans_table = make_hashtable(
    MAX_MAIL_HASH, 
    dtt_hash_mailbox, 
    dtt_match_mailbox
  );
    DEBUG3 (fprintf (stderr, "dtt_trans_table: dtt_trans_table = %x\n",
		     dtt_trans_table);)
  check_null(
    dtt_trans_table,
    "dtt_init:  out of memory during make trans hashtable .",
    out_of_memory
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dtt_init: exiting.\n");
  );
  return;
}                                         /* end dtt_init		    */

/*--------------------------------------------------------------------------*\
|  Routine: dtt_transaction_tell
| 
|  Description:  
|     Updates the object base with a new transaction for a specific mailbox.
\*--------------------------------------------------------------------------*/
void dtt_transaction_tell(mailbox, transaction)
ipc_mailbox mailbox;
isd_trans transaction;
{                                         /* local type definitions	    */
   current_trans_record *transaction_record;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "dtt_transaction_tell: call.\n");
    fprintf(stderr, "dtt_transaction_tell: transaction = %x.\n", transaction);
    fprintf(stderr, "dtt_transaction_tell: mailbox = %d.\n", mailbox);
  );
/* 
   Examine hash for existing current transaction record.
*/
  transaction_record = (current_trans_record *)get_from_hashtable(
    dtt_trans_table, 
    mailbox
  );
/* 
   Modify if found.
*/
  if (transaction_record != NULL) {	/* existing transaction record	    */
    transaction_record->transaction = transaction;
  }					/* end if existing trans record	    */
/* 
   Else if not found, create a new current transaction record, set values,
   and add to hash.
*/
  else {				/* no existing trans record	    */

    transaction_record = (current_trans_record *)make_node(
      sizeof(current_trans_record)
    );
    DEBUG3 (fprintf (stderr, "dtt_transaction_tell: transaction_record = %x\n",
		     transaction_record);)
    check_null(
      transaction_record,
      "dtt_transaction_tell: out of memory during make_node of trans rec.\n",
      out_of_memory
    );

    transaction_record->mailbox = mailbox;
    transaction_record->transaction = transaction;

    add_to_hashtable(dtt_trans_table, transaction_record, mailbox);

  }					/* end else no existing trans rec   */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dtt_transaction_tell: exiting.\n");
  );
  return;
}                                         /* end dtt_transaction_tell */

/*--------------------------------------------------------------------------*\
|  Routine: dtt_transaction_ask
| 
|  Description:  
|     Gets the transaction associated with a mailbox from the object base.
\*--------------------------------------------------------------------------*/
isd_trans dtt_transaction_ask(mailbox)
ipc_mailbox mailbox;
{                                         /* local type definitions	    */
  current_trans_record * transaction_record;
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "dtt_transaction_ask: call.\n");
    fprintf(stderr, "dtt_transaction_ask: mailbox = %d.\n", mailbox);
  );
/* 
   Examine hash for existing current transaction record.
*/
  transaction_record = (current_trans_record *)get_from_hashtable(
    dtt_trans_table, 
    mailbox
  );
/* 
   Set status and return.
*/
  DEBUG2 ( fprintf(stderr, "dtt_transaction_ask: exiting.\n"););

  if (transaction_record == NULL) 	/* not found			    */
    return NULL;
  else 					/* found			    */
    return transaction_record->transaction;
}                                       /* end dtt_transaction_ask	    */

/*--------------------------------------------------------------------------*\
|  Routine: dtt_cleanup
| 
|  Description:  
|     Performs necessary cleanup of the object base manager.
\*--------------------------------------------------------------------------*/
void dtt_cleanup()
{                                         /* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);  		          /* begin                          */
  DEBUG2(
    fprintf(stderr, "dtt_cleanup: call.\n");
  );
/* 
   Destroy the mailbox hash table.
*/
    DEBUG3 (fprintf (stderr, "dtt_cleanup: dtt_trans_table = %x\n",
		     dtt_trans_table);)
/*
??	should actually free node when free hashtable, but it is not "working"
*/
  free_hashtable(dtt_trans_table, AND_NODES);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dtt_cleanup: exiting.\n");
  );
  return;
}                                         /* end dtt_cleanup		    */


