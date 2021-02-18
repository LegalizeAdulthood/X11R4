static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/isd2.c,v 1.10 89/09/29 15:37:21 rcs Exp $";

/*--------------------------------------------------------------------------*\
| 
|  Name:  Shared data abstraction (isd)
|
|  Description:  
|     The shared data abstraction provides a means of communicating data
|     between the various processes of the SERPENT system. 
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     isd.c - contains the implementation for the shared data abstraction.
|     isd.h - contains the external definition for the shared data abstr.
|     isdtest.c - test file.
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
|     Nov. 19, 1987 : Module created.
|     Apr. 12, 1988 : Added code to insure change type of either isd_change
|                     or isd_modify on call to isd_incorporate_changes, rCs.
|     Apr. 27, 1988 : Added isd_get_shared_data - rCs.
|     May 11, 1988  : Clean saber compile and added call to ipb_record_
|                     transaction  ljb
|     Sep. 20, 1988 : Fixed bug with incorporating changes for zero
|                     length buffers. - rCs
|     Sep. 22, 1988 : Added component_not_changed status code to 
|		      isd_get_shared_data. - rCs
|     $Log:	isd2.c,v $
 * Revision 1.10  89/09/29  15:37:21  rcs
 * added support for typed buffers
 * 
 * Revision 1.9  89/02/09  19:08:40  ejh
 * added copyright notice
 * 
 * Revision 1.8  89/01/30  14:42:06  rcs
 * no change
 * 
 * Revision 1.7  88/12/05  17:21:39  rcs
 * add_shared_data now initializes data to undefined
 * 
 * Revision 1.6  88/11/02  14:48:13  rcs
 * moved debug variable to interface file
 * 
 * Revision 1.5  88/09/29  14:48:25  rcs
 * *** empty log message ***
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

#include "isd.h"                /* shared data abstraction                  */
#include "isd_priv.h"           /* shared data abstraction private types    */
#include "isd1.h"               /* access to local routine in isd1	    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(isd_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(isd_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(isd_debug, statement)

/*-------------------- Externals -------------------------------------------*/

extern process_record in_box;	/* attached process mailbox & data def	    */
extern isd_process out_box;	/* dm process mailbox & attached            */

/*--------------------------------------------------------------------------*\
|  Routine: build_list
| 
|  Description:  
|     The build list routine is an internal procedure which simply 
|     provides a procedure name instead of a define for use in calling
|     the usl_add_to_tail function.
\*--------------------------------------------------------------------------*/
SHM_NODE build_list(list, element)
SHM_LIST list;
SHM_NODE element;
{                                       /* local variable definitions       */
  SHM_NODE node;

  node = usl_add_to_tail(list, element);
  return node;
}                                       /* end build_list		    */

/*--------------------------------------------------------------------------*\
|  Routine: build_rcvd_tran_struct
| 
|  Description:  
|     The build received transaction structure routine builds the necessary
|     data structures for a newly received transaction.
\*--------------------------------------------------------------------------*/
isd_trans build_rcvd_tran_struct(change_table, sender)
SHM_HASH change_table;
ipc_mailbox sender;
{                                       /* local variable definitions       */
rcvd_tran *transaction;
SHM_LIST change_list;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "build_rcvd_tran_struct: call.\n");
    fprintf(
      stderr, 
      "build_rcvd_tran_struct: change table = %x.\n", 
      change_table
    );
    fprintf(stderr, "build_rcvd_tran_struct: sender = %x.\n", sender);
  );
/*
   Create a received transaction record in local process memory, set 
   validation stamp and sender field.
*/
  transaction = (rcvd_tran *)make_node(
    sizeof(rcvd_tran)
  );
  rcheck_status(
    "isd_start_transaction: status from ism_get_shm.",
    NULL
  );

  transaction->validation_stamp = isc_rcvd_tran_vs;

  transaction->sender = sender;
/*
   Redefine the hash and match functions for the hash table since
   these pointers will not come through shared memory intact.
*/
  ush_change_shm_hash_functions(change_table, hash_id, match_id);
/* 
   Set default data definition field to in box data definition.  This will
   be the correct data definition for TAPS and NULL for the dialogue manager.
*/
  transaction->data_definition = in_box.data_definition;
/*
   Create a list in shared memory.
*/

  DEBUG3(
    fprintf(stderr, "build_rcvd_tran_struct: about to make change_list.\n");
  );

  change_list = usl_make_shm_list();
  rcheck_null(
    change_list,
    "build_rcvd_tran_struct:  ush_make_shm_list failed.",
    out_of_memory,
    NULL
  );
/*
   Convert list structure of hash elements by applying build list function
   to each element in the change table.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "build_rcvd_tran_struct: about to create list structure.\n"
    );
  );

  ush_for_all_shm_hashtable(
    change_table, 
    change_list,  
    build_list
  );
/*
   Return received transaction record.
*/
  transaction->change_list = change_list;
  transaction->change_table = change_table;
  transaction->current_change = NULL;

/*
  record transaction on journaling file
*/
  ipb_record_transaction(
     change_table, sender, in_box.mailbox);

  DEBUG2(
    fprintf(stderr, "build_rcvd_tran_struct: exiting.\n");
    fprintf(stderr, 
      "build_rcvd_tran_struct: transaction = %x.\n", 
      (isd_trans)transaction
    );
  );

  return (isd_trans)transaction;
}                                       /* end build_rcvd_tran_struct       */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_transaction
| 
|  Description:  
|     The get transaction routine is used to get the id for the next 
|     completed transaction.
\*--------------------------------------------------------------------------*/
isd_trans isd_get_transaction()
{                                       /* local variable definitions       */
isd_trans transaction;
SHM_HASH change_table;
ipc_mailbox sender;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_transaction: call.\n");
  );
/*
   Receive transaction.
*/
  DEBUG3(
    fprintf(stderr, "isd_get_transaction: about to call ipc_receive.\n");
    fprintf(stderr, "isd_get_transaction: in box = %x.\n");
  );

  change_table = (SHM_HASH)ipc_receive(
    in_box.mailbox, 
    &sender
  );
  rcheck_status(
    "isd_get_transaction: status from ipc_receive.",
    NULL
  );  

  DEBUG3(
    fprintf(
      stderr, 
      "isd_get_transaction: change_table = %x.\n", 
      change_table
    );
  );
/*
   Build received transaction structure.
*/
  transaction = build_rcvd_tran_struct(change_table, sender);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_transaction: exiting.\n");
    fprintf(stderr, 
      "isd_get_transaction: transaction = %x.\n", 
      transaction
    );
  );
  return transaction;
}                                       /* end isd_get_transaction          */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_transaction_no_wait
| 
|  Description:  
|     The get transaction routine is used to get the id for the next 
|     completed transaction.
\*--------------------------------------------------------------------------*/
isd_trans isd_get_transaction_no_wait()
{                                       /* local variable definitions       */
isd_trans transaction;
SHM_HASH change_table;
ipc_mailbox sender;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_transaction_no_wait: call.\n");
  );
/*
   Receive transaction.
*/
  DEBUG3(
    fprintf(stderr, "isd_get_transaction_no_wait: about to call ipc_receive.\n");
    fprintf(stderr, "isd_get_transaction_no_wait: in box = %x.\n");
  );

  change_table = (SHM_HASH)ipc_receive_no_wait(
    in_box.mailbox, 
    &sender
  );
  if (get_status() == not_available) {
    return NULL;
  }
  else if (get_status()) {
    DEBUG1(
      isc_print_status("isd_get_trans_no_wait: status from ipc_receive.");
    );
    return NULL;
  }

  DEBUG3(
    fprintf(
      stderr, 
      "isd_get_transaction_no_wait: change_table = %x.\n", 
      change_table
    );
  );
/*
   Build received transaction structure.
*/
  transaction = build_rcvd_tran_struct(change_table, sender);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_transaction_no_wait: exiting.\n");
    fprintf(stderr, 
      "isd_get_transaction_no_wait: transaction = %x.\n", 
      transaction
    );
  );
  return transaction;
}                                       /* end isd_get_transaction_no_wait  */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_transaction_from_sender
| 
|  Description:  
|     The get transaction from sender routine is used to get the id for the 
|     next completed transaction from a specific sender.  If no message is
|     available the calling process is suspended until available.
\*--------------------------------------------------------------------------*/
isd_trans isd_get_transaction_from_sender(sender)
ipc_mailbox sender;
{                                       /* local variable definitions       */
isd_trans transaction;
SHM_HASH change_table;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_transaction_from_sender: call.\n");
  );
/*
   Receive transaction.
*/
  DEBUG3(
    fprintf(stderr, "isd_get_trans_from_sender: about to call ipc_rcv_from_snder.\n");
    fprintf(stderr, "isd_get_trans_from_sender: in.box = %x.\n");
  );

  change_table = (SHM_HASH)ipc_receive_from_sender(
    in_box.mailbox, 
    sender
  );
  rcheck_status(
    "isd_get_transaction_from_sender: status from ipc_receive.",
    NULL
  );  

  DEBUG3(
    fprintf(stderr, "isd_get_trans_from_sender: change_table = %x.\n", change_table);
  );
/*
   Build received transaction structure.
*/
  transaction = build_rcvd_tran_struct(change_table, sender);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_transaction_from_sender: exiting.\n");
    fprintf(stderr, 
      "isd_get_transaction_from_sender: transaction = %x.\n", 
      transaction
    );
  );
  return transaction;
}                                       /* end isd_get_trans_from_sender    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_sender
| 
|  Description:  
|     The get sender routine is used to return the sender for a specified
|     transaction.
|
\*--------------------------------------------------------------------------*/
ipc_mailbox isd_get_sender(transaction)
isd_trans transaction;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_sender: call.\n");
    fprintf(stderr, "isd_get_sender: transaction = %x.\n", transaction);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_get_sender:");
    );
    return NULL;
  }                                     /* end if invalid transaction hand  */
/*
   Return transaction sender.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_sender: exiting.\n");
    fprintf(stderr, 
      "isd_get_sender: sender = %x.\n", 
      ((rcvd_tran *)transaction)->sender
    );
  );
  return ((rcvd_tran *)transaction)->sender;
}                                       /* end isd_get_sender          */

/*--------------------------------------------------------------------------*\
|  Routine: isd_specify_data_definition
| 
|  Description:  
|     The specify data definition allows the dialogue manager to specify
|     the data definition for the TAP from which the transaction was
|     received.
\*--------------------------------------------------------------------------*/
void isd_specify_data_definition(transaction, process)
isd_trans transaction;
isd_process process;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_specify_data_definition: call.\n");
    fprintf(stderr, "isd_specify_data_definition: transaction = %x.\n", transaction);
    fprintf(stderr, "isd_specify_data_definition: process = %x.\n", process);
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_specify_data_definition:");
    );
    return;
  }                                     /* end if invalid transaction hand  */

  if (!isc_validate(process, isc_process_record_vs)) {
                                        /* invalid process handle       */
    set_status(invalid_process_handle);
    DEBUG1(
      isc_print_status("isd_specify_data_definition:");
    );
    return;
  }                                     /* end if invalid process hand  */
/*
   Put data definition in received transaction record.
*/
  ((rcvd_tran *)transaction)->data_definition = 
		((process_record *)process)->data_definition;

  DEBUG2(
    fprintf(stderr, "isd_specify_data_definition: exiting.\n");
  );
  return;
}                                       /* end isd_specify_data_definition  */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_first_changed_element
| 
|  Description:  
|     The get first changed element routine is used to get the id of the 
|     first changed element on a transaction list.
\*--------------------------------------------------------------------------*/
iid_id_type isd_get_first_changed_element(transaction)
isd_trans transaction;
{                                       /* local variable definitions       */
iid_id_type id;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_first_changed_element: call.\n");
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_get_first_changed_element:");
    );
    return iid_null_id;
  }                                     /* end if invalid transaction hand  */
/*
   Get the list head of the current change list.
*/
  ((rcvd_tran *)transaction)->current_change = 
	    (element_change_record *)get_list_head(
			((rcvd_tran *)transaction)->change_list
  );
/*
   If next element found set id to changed element id, otherwise set id to -1.
*/
  if (((rcvd_tran *)transaction)->current_change != NULL) { /* found	    */
    id = ((rcvd_tran *)transaction)->current_change->id;
  }							/* end if found     */
  else {			/* next element not found		    */
    id = iid_null_id;
  }				/* end else next element not found	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_first_changed_element: id = %d.\n", id);
    fprintf(stderr, "isd_get_first_changed_element: exiting.\n");
  );
  return id;
}                                       /* end isd_get_first_changed_elem   */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_next_changed_element
| 
|  Description:  
|     The get next changed element routine is used to get the id of the 
|     next changed element on a transaction list.
\*--------------------------------------------------------------------------*/
iid_id_type isd_get_next_changed_element(transaction)
isd_trans transaction;
{                                       /* local variable definitions       */
iid_id_type id;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_next_changed_element: call.\n");
    fprintf(
      stderr, 
      "isd_get_next_changed_element: transaction = %x.\n", 
      transaction
    );
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_get_next_changed_element:");
    );
    return iid_null_id;
  }                                     /* end if invalid transaction hand  */
/*
   Get the next element in the change list.
*/
   ((rcvd_tran *)transaction)->current_change = 
   				(element_change_record *)get_list_next(
    ((rcvd_tran *)transaction)->change_list,
    ((rcvd_tran *)transaction)->current_change
  );
/*
   If next element found set id to changed element id, otherwise set id to -1.
*/
  if (((rcvd_tran *)transaction)->current_change != NULL) { /* found	    */
    id = ((rcvd_tran *)transaction)->current_change->id;
  }							/* end if found     */
  else {			/* next element not found		    */
    id = iid_null_id;
  }				/* end else next element not found	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_next_changed_element: id = %d.\n", id);
    fprintf(stderr, "isd_get_next_changed_element: exiting.\n");
  );
  return id;
}                                       /* end isd_get_next_changed_elem   */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_element_name
| 
|  Description:  
|     The get element name accepts an instance id as a parameter and returns 
|     the associated element name.
\*--------------------------------------------------------------------------*/
string isd_get_element_name(transaction, id)
isd_trans transaction;
iid_id_type id;
{                                       /* local variable definitions       */
element_change_record *element_change;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_element_name: call.\n");
    fprintf(stderr, "isd_get_element_name: transaction = %x.\n", transaction);
    fprintf(stderr, "isd_get_element_name: id = %d.\n", id);
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_get_element_name:");
    );
    return NULL;
  }                                     /* end if invalid transaction hand  */
/*
   Get the element name from the hash table in shared memory.
*/
  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table,
    id
  );
  rcheck_null(
    element_change, 
    "isd_get_element_name:  could not find element in change table.\n",
    not_found,
    NULL
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_element_name: exiting.\n");
    fprintf(stderr, 
      "isd_get_element_name: element name = %s.\n", 
      element_change->element_name
    );
  );
  return element_change->element_name;
}                                       /* end isd_get_element_name         */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_change_type
| 
|  Description:  
|     The get change type routine accepts an instance id as a parameter and 
|     returns the associated change type
\*--------------------------------------------------------------------------*/
isd_change_type isd_get_change_type(transaction, id)
isd_trans transaction;
iid_id_type id;
{                                       /* local variable definitions       */
element_change_record *element_change;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_change_type: call.\n");
    fprintf(stderr, "isd_get_change_type: transaction = %x.\n", transaction);
    fprintf(stderr, "isd_get_change_type: id = %d.\n", id);
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_get_change_type:");
    );
    return isd_no_change;
  }                                     /* end if invalid transaction hand  */
/*
   Get the element name from the hash table in shared memory.
*/
  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table,
    id
  );
  rcheck_null(
    element_change, 
    "isd_get_change_type:  could not find element in change table.\n",
    not_found,
    isd_no_change
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_change_type: exiting.\n");
  );
  return element_change->change_type;
}                                       /* end isd_get_change_type          */

/*--------------------------------------------------------------------------*\
|  Routine: isd_create_changed_component_list
| 
|  Description:  
|     The create changed component name list routine accepts an instance id as
|     a parameter and returns the associated list of changed component names.
\*--------------------------------------------------------------------------*/
LIST isd_create_changed_component_list(transaction, id)
isd_trans transaction;
iid_id_type id;
{                                       /* local variable definitions       */
element_change_record *element_change;
LIST changed_components;
string component_name;
int components, i;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_create_changed_component_list: call.\n");
    fprintf(stderr, 
      "isd_create_changed_component_list: transaction = %x.\n", 
      transaction
    );
    fprintf(stderr, "isd_create_changed_component_list: id = %d.\n", id);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_create_changed_component_list:");
    );
    return NULL;
  }                                     /* end if invalid transaction hand  */
/*
   Get the element name from the hash table in shared memory.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_create_chg_comp_list: about to get elem from chg table.\n"
    );
  );

  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table,
    id
  );
  rcheck_null(
    element_change, 
    "isd_create_chngd_comp_list:  could not find element in change table.\n",
    not_found,
    NULL
  );
/*
   Create a list in process memory to return the list of component names.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_create_chg_comp_list: about to make chng comp list.\n"
    );
  );   

  changed_components = make_list();
  rcheck_null(
    changed_components,
    "isd_create_changed_component_list:  make_list failed.\n",
    out_of_memory,
    NULL
  );
/*
   Determine the number of components in the shared data element.
*/

  DEBUG3(
    fprintf(stderr, "isd_create_chg_comp_list: about to call idd_get_#_comps.\n");
  );   

  components = idd_get_number_of_components(
    ((rcvd_tran *)transaction)->data_definition,
    element_change->element_name
  );
  rcheck_status(
    "isd_create_changed_comp_list: status from idd_get_#_comps.",
    NULL
  );

  DEBUG3(
    fprintf(stderr, "isd_create_chg_comp_list: # of components = %d.\n", components);
  );

/*
  Determine, for each component in the data defintion, if that component
  has been changed (its changed bit is set) and if so add component
  name to the list of changed components.
*/
  for (i = 0; i <= components; i++) {	/* for each component, 0 - i	    */
    if (ubs_test_bit(element_change->changed_components, i)) {
					/* component changed		    */
      component_name = idd_get_component_name(
        ((rcvd_tran *)transaction)->data_definition,
        element_change->element_name,
        i
      );
      rcheck_status(
        "isd_create_changed_comp_list: status from idd_get_comp_name.",
        NULL
      );

      add_to_tail(changed_components, component_name);
    }					/* end if component changed	    */
  }					/* end for each component	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_create_changed_component_list: exiting.\n");
    fprintf(stderr, 
      "isd_create_changed_component_list: changed_components = %x.\n",
      changed_components
    );
  );
  return changed_components;
}                                       /* end isd_get_changed_comp_list    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_destroy_changed_component_list
| 
|  Description:  
|     The destroy changed component list routine releases all storage
|     associated with a changed component list.
\*--------------------------------------------------------------------------*/
void isd_destroy_changed_component_list(changed_component_list)
LIST changed_component_list;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_destroy_changed_component_list: call.\n");
    fprintf(stderr, 
      "isd_destroy_chngd_comp_list: changed_component_list = %x.\n", 
      changed_component_list);
  );
/*
   Destroy the list not including the nodes (which are actually pointers
   to data in the data definition.
*/
  free_list(changed_component_list, NOT_NODES);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_destroy_changed_component_list: exiting.\n");
  );
  return;
}                                       /* end isd_destroy_chngd_comp_list  */

/*--------------------------------------------------------------------------*\
|  Routine: isd_shm2pm_buffcpy
| 
|  Description:  
|     Copies contents of shm_buff to pm_buff.  The buffer structure is
|     assumed to exist but storage for the buffer body will be allocated
|     in process memory.
\*--------------------------------------------------------------------------*/
void isd_shm2pm_buffcpy(pm_buff, shm_buff)
idd_buffer_type *pm_buff;
idd_buffer_type *shm_buff;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_shm2pm_buffcpy: call.\n");
    fprintf(stderr, "isd_shm2pm_buffcpy: pm_buff = %x.\n", pm_buff);
    fprintf(stderr, "isd_shm2pm_buffcpy: shm_buff = %x.\n", shm_buff);
  );

  DEBUG3(
    fprintf(
      stderr,
      "isd_shm2pm_buffcpy: shm buffer length = %d.\n",
      shm_buff->length
    );
    fprintf(
      stderr,
      "isd_shm2pm_buffcpy: shm_buff->type = %s.\n",
      idd_get_type_string(shm_buff->type)
    );
    fprintf(
      stderr,
      "isd_shm2pm_buffcpy: shm buffer body = %s.\n",
      shm_buff->body
    );
    fprintf(
      stderr,
      "isd_shm2pm_buffcpy: user buffer address = %x.\n", 
      pm_buff
    );
  );
/*
   If buffer length is non-zero, allocate process memory for new buffer body
   and copy from shared memory.
*/
  pm_buff->length = shm_buff->length;
  pm_buff->type = shm_buff->type;

  if (pm_buff->length > 0) {		/* positive buffer length	   */
    pm_buff->body = make_node(pm_buff->length);
    check_null(
      pm_buff->body,
      "isd_shm2pm_buffcpy: out of memory.\n",
      out_of_memory
    );

    ism_copy_from_shared_memory(
      shm_buff->body, 
      shm_buff->length, 
      pm_buff->body
    );
    check_status(
      "isd_shm2pm_buffcpy: status from ism_copy_from_shm."
    );
  } 				/* end if no buffer body		    */
/*
 Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_shm2pm_buffcpy: exiting.\n");
  );
  return;
}                                       /* end isd_shm2pm_buffcpy	    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_incorporate_change
| 
|  Description:  
|     The incorporate changes routine is used to incorporate changes into
|     the original data.
\*--------------------------------------------------------------------------*/
void isd_incorporate_changes(transaction, id, data)
isd_trans transaction;
iid_id_type id;
caddr_t data;
{                                       /* local variable definitions       */
  element_change_record *element_change;
  string component_name;
  int i, components, length, offset, position;
  idd_buffer_type *user_buffer;
  idd_buffer_type *shm_buffer;
  idd_data_types data_type;
  isd_change_type change_type;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_incorporate_changes: call.\n");
    fprintf(
      stderr, 
      "isd_incorporate_changes: transaction = %x.\n", 
      transaction
    );
    fprintf(stderr, "isd_incorporate_changes: id = %d.\n", id);
    fprintf(stderr, "isd_incorporate_changes: data = %x.\n", data);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_incorporate_changes:");
    );
    return;
  }                                     /* end if invalid transaction hand  */
/*
   Make sure element change type is either create or modify.  
   Otherwise it is impossible to incorporate changes since there are no 
   changes so set status to remove_change_type and return.
*/
  change_type = isd_get_change_type(transaction,id);
  if ((change_type != isd_create) && (change_type != isd_modify)) {
    set_status(invalid_change_type);
    DEBUG1(
      isc_print_status("isd_incorporate_changes:");
    );
    return;
  }
/*
   Get the element name from the hash table in shared memory.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_incorporate_changes: about to get elem from chg table.\n"
    );
  );

  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table,
    id
  );
  check_null(
    element_change, 
    "isd_incorporate_changes:  could not find element in change table.\n",
    not_found
  );
/*
   Determine the number of components in the shared data element.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_incorporate_changes: about to call idd_get_#_comps.\n"
    );
  );   

  components = idd_get_number_of_components(
    ((rcvd_tran *)transaction)->data_definition,
    element_change->element_name
  );
  check_status("isd_incorporate_changes: status from idd_get_#_comps.");

  DEBUG3(
    fprintf(
      stderr, 
      "isd_incorporate_changes: # of components = %d.\n", 
       components
    );
  );
  if (components == -1) {		/* element is a scalar		    */

    length = idd_get_length(
      ((rcvd_tran *)transaction)->data_definition,
      element_change->element_name
    );
    check_status("isd_incorporate_changes: status from idd_get_length.");
/*
   Get data type.
*/
    data_type = idd_get_shared_data_type(
      ((rcvd_tran *)transaction)->data_definition,
      element_change->element_name,
      NULL
    );
    check_status(
      "isd_incorporate_changes: status from idd_get_shared_data_type."
    );

    DEBUG3(
      fprintf(
        stderr, 
        "isd_incorporate_changes: data type = %d.\n", 
         data_type
      );
    );
/*
   If buffer handle special case.
*/
    if (data_type == idd_buffer) {    /* buffer type                      */

      user_buffer = (idd_buffer_type *)data;
      shm_buffer = (idd_buffer_type *)element_change->data;

      DEBUG3(
        fprintf(
          stderr,
          "isd_incorporate_changes: buffer length = %d.\n",
          shm_buffer->length
        );
        fprintf(
          stderr,
          "isd_shm2pm_buffcpy: user_buffer->type = %s.\n",
          idd_get_type_string(user_buffer->type)
        );
        fprintf(
          stderr,
          "isd_incorporate_changes: buffer body = %s.\n",
          shm_buffer->body
        );
      );
/*
   Free user buffer if it exists.
*/
      if (user_buffer->body != NULL) {	/* buffer exists?		    */
        free_node(user_buffer->body);
      }					/* end if buffer exists		    */
/*
   Allocate process memory for new buffer and copy from shared memory.
*/
      user_buffer->length = shm_buffer->length;
      user_buffer->type = shm_buffer->type;

      user_buffer->body = make_node(user_buffer->length);
      check_null(
        user_buffer->body,
        "isd_incorporate_changes: out of memory.\n",
        out_of_memory
      );

      ism_copy_from_shared_memory(
        shm_buffer->body, 
        shm_buffer->length, 
        user_buffer->body
      );
      check_status("isd_incorporate_changes: status from ism_copy_from_shm.");

    }				/* end if buffer type                       */
    else {			/* not a buffer				    */
      ism_copy_from_shared_memory(
        element_change->data,
        length,
        data
      );
      check_status("isd_inc_changes: status from ism_copy_from_shm.");
    }				/* end else not a buffer		    */
  }					/* end if element is a scalar	    */
  else {				/* element is a record		    */
/*
   Transfer data from transaction data to user specified location for
   each component with a set changed component bit.
*/
    for (i = 0; i <= components; i++) {	/* for each component, 0 - i	    */
      if (ubs_test_bit(element_change->changed_components, i)) {
					/* component changed		    */
      
        component_name = idd_get_component_name(
          ((rcvd_tran *)transaction)->data_definition,
          element_change->element_name,
          i
        );
        check_status("isd_inc_changes: status from idd_get_comp_name."); 

        DEBUG3(
          fprintf(stderr, 
            "isd_incorporate_changes:  component_name = %s.\n", 
            component_name
          );
        ); 

        idd_get_component_lop(
          ((rcvd_tran *)transaction)->data_definition,
          element_change->element_name,
          component_name,
          &length,
          &offset,
          &position
        );
        check_status("isd_inc_changes: status from idd_get_comp_lop.");

        DEBUG3(
          fprintf(stderr, "isd_incorporate_changes: length = %d.\n", length); 
          fprintf(stderr, "isd_incorporate_changes: offset = %d.\n", offset); 
          fprintf(
            stderr, 
            "isd_incorporate_changes:  position = %d.\n", 
            position
          ); 
        ); 
/*
   Get data type.
*/
        data_type = idd_get_shared_data_type(
          ((rcvd_tran *)transaction)->data_definition,
          element_change->element_name,
          component_name
        );
        check_status(
          "isd_incorporate_changes: status from idd_get_shared_data_type."
        );

        DEBUG3(
          fprintf(
            stderr, 
            "isd_incorporate_changes: data type = %d.\n", 
             data_type
          );
        );
/*
   If buffer handle special case.
*/
        if (data_type == idd_buffer) {    /* buffer type		    */

          user_buffer = (idd_buffer_type *)((char *)data + offset);
          shm_buffer = (idd_buffer_type *)
				((char *)element_change->data + offset);
          DEBUG3(
            fprintf(
              stderr,
              "isd_incorporate_changes: shm buffer length = %d.\n",
              shm_buffer->length
            );
            fprintf(
              stderr,
              "isd_incorporate_changes: user_buffer->type = %s.\n",
              idd_get_type_string(user_buffer->type)
            );
            fprintf(
              stderr,
              "isd_incorporate_changes: shm buffer body = %s.\n",
              shm_buffer->body
             );
            fprintf(
              stderr,
              "isd_incorporate_changes: user buffer address = %x.\n",
              user_buffer
             );
          );
/*
   Free user buffer if it exists.
*/
          if (user_buffer->body != NULL) {	/* buffer exists?	    */
            free_node(user_buffer->body);
          }					/* end if buffer exists	    */
/*
   Allocate process memory for new buffer and copy from shared memory.
*/
          user_buffer->length = shm_buffer->length;
          user_buffer->type = shm_buffer->type;

          user_buffer->body = make_node(user_buffer->length);
          check_null(
            user_buffer->body,
            "isd_incorporate_changes: out of memory.\n",
            out_of_memory
          );

          ism_copy_from_shared_memory(
            shm_buffer->body, 
            shm_buffer->length, 
            user_buffer->body
          );
          check_status(
            "isd_incorporate_changes: status from ism_copy_from_shm."
          );

        }			/* end if buffer type                       */
        else {			/* else not buffer type			    */

          DEBUG3(
            switch (data_type) {		/* switch on data type	    */
              case idd_boolean:
              case idd_integer:
              case idd_id:
                fprintf(
                  stderr,
                  "isd_incorporate_changes: value = %d.\n",
                  *((int *)(element_change->data + offset))
                );
                break;
              case idd_real:
                fprintf(
                  stderr,
                  "isd_incorporate_changes: value = %f.\n",
                  *((double *)(element_change->data + offset))
                );
                break;
              case idd_string:
                fprintf(
                  stderr,
                  "isd_incorporate_changes: value = %s.\n",
                  element_change->data + offset
                );
              case idd_buffer:
              case idd_record:
                fprintf(
                  stderr,
                  "isd_incorporate_changes: buffer or record type.\n"
                );
                break;
              default:
                fprintf(
                  stderr,
                  "isd_incorporate_changes: unknown type.\n"
                );
	    }					/* end switch		   */
          );

          ism_copy_from_shared_memory(
            (element_change->data) + offset,
            length,
            data + offset
          );
          check_status("isd_inc_changes: status from ism_copy_from_shm.");
        }			/* end else buffer type                     */

      }					/* end if component changed	    */
    }					/* end for each component	    */
  }					/* end if element is a record	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_incorporate_changes: exiting.\n");
  );
  return;
}                                       /* end isd_incorporate_changes      */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_shared_data
| 
|  Description:  
|     The get data routine allocates process memory, copies shared 
|     data into process memory and returns a pointer to the data.
|
|     Warning:  Record components may not have been specified and therefore
|               do not contain valid data.
\*--------------------------------------------------------------------------*/
caddr_t isd_get_shared_data(transaction, id, component_name)
isd_trans transaction;
iid_id_type id;
string component_name;
{                                       /* local variable definitions       */
  element_change_record *element_change;

  int i, components, length, offset, position;
  idd_data_types data_type;
  isd_change_type change_type;
  caddr_t data;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_shared_data: call.\n");
    fprintf(
      stderr, 
      "isd_get_shared_data: transaction = %x.\n", 
      transaction
    );
    fprintf(stderr, "isd_get_shared_data: id = %d.\n", id);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_get_shared_data:");
    );
    return NULL;
  }                                     /* end if invalid transaction hand  */
/*
   Make sure element change type is either create or modify.  
   Otherwise it is impossible to incorporate changes since there are no 
   changes so set status to remove_change_type and return.
*/
  change_type = isd_get_change_type(transaction, id);
  if ((change_type != isd_create) && (change_type != isd_modify)) {
    set_status(invalid_change_type);
    DEBUG1(
      isc_print_status("isd_get_shared_data:");
    );
    return NULL;
  }
/*
   Get the element name from the hash table in shared memory.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_get_shared_data: about to get elem from chg table.\n"
    );
  );

  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table,
    id
  );
  rcheck_null(
    element_change, 
    "isd_get_shared_data:  could not find element in change table.\n",
    not_found,
    NULL
  );
/*
   If component name not specified get the entire element, otherwise
   only get the specified component.
*/
  if (component_name == NULL) {		/* element			    */
/*
   Determine the number of components in the shared data element.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_get_shared_data: about to call idd_get_#_comps.\n"
    );
  );   

  components = idd_get_number_of_components(
    ((rcvd_tran *)transaction)->data_definition,
    element_change->element_name
  );
  rcheck_status("isd_get_shared_data: status from idd_get_#_comps.",NULL);

  DEBUG3(
    fprintf(
      stderr, 
      "isd_get_shared_data: # of components = %d.\n", 
       components
    );
  );
/*
   Get the length of the element and allocate storage.
*/
    length = idd_get_length(
      ((rcvd_tran *)transaction)->data_definition,
      element_change->element_name
    );
    rcheck_status("isd_get_shared_data: status from idd_get_length.", NULL);

    data = make_node(length);
    rcheck_null(
      data,
      "isd_get_shared_data:  out_of_memory during make_node.\n",
      out_of_memory,
      NULL
    );
/*
   Determine if element is a scalar or a record and handle accordingly.
*/
  if (components == -1) {		/* element is a scalar		    */
/*
   Get data type.
*/
    data_type = idd_get_shared_data_type(
      ((rcvd_tran *)transaction)->data_definition,
      element_change->element_name,
      NULL
    );
    rcheck_status(
      "isd_get_shared_data: status from idd_get_shared_data_type.",
      NULL
    );
/*
   If buffer handle special case.
*/
    if (data_type == idd_buffer) {    /* buffer type                      */

      ((idd_buffer_type *)data)->body = NULL;

      isd_shm2pm_buffcpy(
        (idd_buffer_type *)data, 
        (idd_buffer_type *)element_change->data
      );
      rcheck_status(
        "isd_get_shared_data: bad status from isd_shm2pm_buffcpy.",
        NULL
      );

    }				/* end if buffer type                       */
    else {			/* not a buffer				    */
      ism_copy_from_shared_memory(
        element_change->data,
        length,
        data
      );
      rcheck_status("isd_inc_changes: status from ism_copy_from_shm.",
        NULL);
    }				/* end else not a buffer		    */
  }					/* end if element is a scalar	    */
  else {				/* element is a record		    */
/*
   Transfer data from transaction data to user specified location for
   each component with a set changed component bit.
*/
    for (i = 0; i <= components; i++) {	/* for each component, 0 - i	    */
      if (ubs_test_bit(element_change->changed_components, i)) {
					/* component changed		    */
      
        component_name = idd_get_component_name(
          ((rcvd_tran *)transaction)->data_definition,
          element_change->element_name,
          i
        );
        rcheck_status("isd_inc_changes: status from idd_get_comp_name.",
                        NULL); 

        DEBUG3(
          fprintf(stderr, 
            "isd_get_shared_data:  component_name = %s.\n", 
            component_name
          );
        ); 

        idd_get_component_lop(
          ((rcvd_tran *)transaction)->data_definition,
          element_change->element_name,
          component_name,
          &length,
          &offset,
          &position
        );
        rcheck_status("isd_inc_changes: status from idd_get_comp_lop.",
        NULL);

        DEBUG3(
          fprintf(stderr, "isd_get_shared_data: length = %d.\n", length); 
          fprintf(stderr, "isd_get_shared_data: offset = %d.\n", offset); 
          fprintf(
            stderr, 
            "isd_get_shared_data:  position = %d.\n", 
            position
          ); 
        ); 
/*
   Get data type.
*/
        data_type = idd_get_shared_data_type(
          ((rcvd_tran *)transaction)->data_definition,
          element_change->element_name,
          component_name
        );
        rcheck_status(
          "isd_get_shared_data: status from idd_get_shared_data_type.",
          NULL
        );
/*
   If buffer handle special case.
*/
        if (data_type == idd_buffer) {    /* buffer type		    */

         ((idd_buffer_type *)((char *)data + offset))->body = NULL;

          isd_shm2pm_buffcpy(
            (idd_buffer_type *)((char *)data + offset),
            (idd_buffer_type *)((char *)element_change->data + offset)
          );
          rcheck_status(
            "isd_get_shared_data: bad status from isd_shm2pm_buffcpy.",
            NULL
          );

        }			/* end if buffer type                       */
        else {			/* else not buffer type			    */
          ism_copy_from_shared_memory(
            (element_change->data) + offset,
            length,
            data + offset
          );
          rcheck_status("isd_inc_changes: status from ism_copy_from_shm.",
          NULL);
        }			/* end else buffer type                     */

      }					/* end if component changed	    */
    }					/* end for each component	    */
  }					/* end if element is a record	    */
  }					/* end if element		    */
/*
   Component name specified.
*/
  else {				/* component			    */
/*
   Get the length, offset and position of the component.
*/
    idd_get_component_lop(
      ((rcvd_tran *)transaction)->data_definition,
      element_change->element_name,
      component_name,
      &length,
      &offset,
      &position
    );
    rcheck_status(
      "isd_get_shared_data: bad status from idd_get_component_lop.",
      NULL
    );
/*
   If the component has not been changed, set error code and exit.
*/
    if (!ubs_test_bit(element_change->changed_components, position)) {
                                        /* component changed                */
       set_status(component_not_changed);
       DEBUG1(
         isc_print_status("isd_get_shared_data");
         fflush(stderr);
       );

       return(NULL);
    }					/* end if component changed	    */
/*
   Allocate process memory for component.
*/
    data = make_node(length);
    rcheck_null(
      data,
      "isd_get_shared_data:  out_of_memory during make_node.\n",
      out_of_memory,
      NULL
    );
/*
   Get data type.
*/
    data_type = idd_get_shared_data_type(
      ((rcvd_tran *)transaction)->data_definition,
      element_change->element_name,
      component_name
    );
    rcheck_status(
      "isd_get_shared_data: status from idd_get_shared_data_type.",
      NULL
    );
/*
   If buffer handle special case.
*/
   if (data_type == idd_buffer) {    /* buffer type		    */

     ((idd_buffer_type *)(char *)data)->body = NULL;

     isd_shm2pm_buffcpy(
       (idd_buffer_type *)((char *)data),
       (idd_buffer_type *)((char *)element_change->data + offset)
     );
     rcheck_status(
       "isd_get_shared_data: bad status from isd_shm2pm_buffcpy.",
       NULL
     );

   }				/* end if buffer type                       */
   else {			/* else not buffer type			    */
     ism_copy_from_shared_memory(
       (element_change->data) + offset,
       length,
       data
     );
     rcheck_status(
       "isd_inc_changes: status from ism_copy_from_shm.",
       NULL
     );
   }				/* end else buffer type                     */

 }				/* end if component			    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_shared_data: exiting.\n");
    fprintf(stderr, "isd_get_shared_data: data = %x.\n", data);
  );
  return data;
}                                       /* end isd_get_shared_data     	    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_shared_data_type
|
|  Description:
|     The get shared data type routine is used to get the type associated with
|     a shared data element.
\*--------------------------------------------------------------------------*/
idd_data_types isd_get_shared_data_type(element_name, component_name)
string element_name;
string component_name;
{                                       /* local variable definitions       */
  idd_data_types data_type;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_shared_data_type: call.\n");
    fprintf(
      stderr, 
      "isd_get_shared_data_type: element name = %s.\n", 
      element_name
    );
    fprintf(
      stderr, 
      "isd_get_shared_data_type: component name = %s.\n", 
      component_name
    );
  );
/*
   Validate parameter.
*/
  if (element_name == NULL) {           /* null element string              */
    set_status(null_element_name);
    DEBUG1(
      isc_print_status("isd_get_shared_data_type:");
    );
    return idd_no_data_type;
  }                                     /* end if null element string       */
/*
   Add in box data definition and call idd get shared data type.
*/
  data_type = idd_get_shared_data_type(
    in_box.data_definition, 
    element_name, 
    component_name
  );
  rcheck_status(
    "isd_get_sd_type: bad status from idd_get_sd_data_type",
    idd_no_data_type
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_shared_data_type: data type = %d.\n", data_type);
    fprintf(stderr, "isd_get_shared_data_type: exiting.\n");
  );
  return data_type;
}                                       /* end isd_get_shared_data_type     */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_length
|
|  Description:
|     The get length routine returns the length of a specified shared data
|     element.
\*--------------------------------------------------------------------------*/
int isd_get_length(element_name)
string element_name;
{                                       /* local variable definitions       */
  int length;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_length: call.\n");
    fprintf(stderr, "isd_get_length: element_name = %s.\n", element_name);
  );
/*
   Validate parameter.
*/
  if (element_name == NULL) {           /* null element string              */
    set_status(null_element_name);
    DEBUG1(
      isc_print_status("isd_get_length:");
    );
    return -1;
  }                                     /* end if null element string       */
/*
   Add in_box data definition and call idd get length.
*/
  length = idd_get_length(in_box.data_definition, element_name);
  rcheck_status("isd_get_length: bad status from idd_get_length",
               NULL);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_length: length = %d.\n", length);
    fprintf(stderr, "isd_get_length: exiting.\n");
  );
  return length;
}                                       /* end isd_get_length		    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_component_lop
|
|  Description:
|     The get component lop routine returns the length, offset and position
|     of a component within a shared data element.  Component positions are
|     numbered from 0 to n-1.
\*--------------------------------------------------------------------------*/
void isd_get_component_lop(element_name, component_name, length, offset,
			                                             position)
string element_name;
string component_name;
int *length;
int *offset;
int *position;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_component_lop: call.\n");
  );
/*
   Validate parameter.
*/
  if (element_name == NULL) {           /* null element string              */
    set_status(null_element_name);
    DEBUG1(
      isc_print_status("isd_get_component_lop:");
    );
    return;
  }                                     /* end if null element string       */

  if (component_name == NULL) {		/* null component string	    */
    set_status(null_component_name);
    DEBUG1(
      isc_print_status("isd_get_component_lop:");
    );
    return;
  }                                     /* end if null component string     */
/*
   Add in box data definition and call idd_get_component_lop.
*/
  idd_get_component_lop(
    in_box.data_definition,
    element_name, 
    component_name,
    length,
    offset,
    position
  );
  check_status("isd_get_component_lop: bad status from idd_get_comp_lop");
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_component_lop: length = %d.\n", *length);
    fprintf(stderr, "isd_get_component_lop: offset = %d.\n", *offset);
    fprintf(stderr, "isd_get_component_lop: position = %d.\n", *position);
    fprintf(stderr, "isd_get_component_lop: exiting.\n");
  );
  return;
}                                       /* end isd_get_component	    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_purge_transaction
| 
|  Description:  
|     The purge transaction routine is used to delete a committed transaction
|     buffer.  Transaction buffers maintain a history of changes to the shared
|     data.  Transaction buffers should be purged once there is no longer any
|     need for this information.
\*--------------------------------------------------------------------------*/
void isd_purge_transaction(transaction)
isd_trans transaction;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_purge_transaction: call.\n");
    fprintf(
      stderr, 
      "isd_purge_transaction: transaction = %x.\n", 
      transaction
    );
  );
/*
   Validate parameter.
*/
  if (!isc_validate(transaction, isc_rcvd_tran_vs)) {
                                        /* invalid transaction handle       */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_purge_transaction:");
    );
    return;
  }                                     /* end if invalid transaction hand  */
/*
   Destroy change list structure from around the actual data nodes.
*/

  DEBUG3(
    fprintf(stderr, "isd_purge_transaction: about to free change list.\n");
  );

  usl_free_shm_list(((rcvd_tran *)transaction)->change_list, NOT_NODES);
/*
   Free storage associated with all element change records in transaction 
   hash table.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "isd_purge_trans: about to free elements in change table.\n"
    );
  );

  ush_for_all_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table, 
    ((rcvd_tran *)transaction)->data_definition,
    free_element_change_record
  );
/*
   Destroy change hashtable structure and all nodes.
*/

  DEBUG3(
    fprintf(stderr, "isd_purge_transaction: about to free change table.\n");
  );

  ush_free_shm_hashtable(
    ((rcvd_tran *)transaction)->change_table, 
    AND_NODES
  );
/*
   Delete received transaction record.
*/

  DEBUG3(
    fprintf(stderr, "isd_purge_transaction: about to free transaction.\n");
  );

  free_node(transaction);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_purge_transaction: exiting.\n");
  );
  return;
}                                       /* end isd_purge_transaction       */

/*--------------------------------------------------------------------------*\
|  Routine: isd_dm_cleanup
| 
|  Description:  
|     The cleanup once routine performs necessary clean of the interface 
|     layer for the dialogue manager.
\*--------------------------------------------------------------------------*/
void isd_dm_cleanup()
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_dm_cleanup: call.\n");
  );
/*
   Clean up shared memory.
*/
  ism_cleanup();
  check_status("isd_dm_cleanup: status from ipc_cleanup.");
/*
   Clean up semaphore id.
*/
  ids_cleanup();
  check_status("isd_dm_cleanup: status from ids_cleanup.");
/*
   Clean up id counter.
*/
  iid_cleanup();
  check_status("isd_dm_cleanup: status from iid_cleanup.");
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_dm_cleanup: exiting.\n");
  );
  return;
}                                       /* end isd_dm_cleanup		    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_cleanup
| 
|  Description:  
|     The cleanup routine performs necessary clean of the interface 
|     layer for the application.
\*--------------------------------------------------------------------------*/
void isd_cleanup()
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_cleanup: call.\n");
  );
/*
   Destroy out_box data definition.
*/
  isd_destroy_process_definition(out_box);
  check_status("isd_cleanup: status from isd_destroy_process_def.");
/*
   Cleanup everything else.
*/
  isd_dm_cleanup();
  check_status("isd_cleanup: status from isd_dm_cleanup.\n");
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_cleanup: exiting.\n");
  );
  return;
}                                       /* end isd_cleanup		    */

