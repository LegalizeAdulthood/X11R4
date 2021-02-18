/*      $Header: /u/04c/mnt/integ/int/include/RCS/isd.h,v 1.6 89/09/29 16:40:44 ljb Exp $ */

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
|     May. 11, 1988 : references to typed data puts and gets removed  ljb
|     June 10, 1988 : isd_get_transaction_from_sender now waits for trans ljb
$Log:	isd.h,v $
 * Revision 1.6  89/09/29  16:40:44  ljb
 * add new routine which adds shared data with id 
 * 
 * Revision 1.5  89/02/09  18:44:59  ejh
 * added copyright notice
 * 
 * Revision 1.4  88/12/06  15:44:42  rcs
 * added isd_init_sd_element routine
 * 
 * Revision 1.3  88/11/02  14:48:49  rcs
 * moved debug variable from c file
 * 
 * Revision 1.2  88/10/03  08:07:21  little
 * add definition for isd_dm_init_sdd
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

#ifndef isd
#define isd

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

#include <sys/types.h>		/* System types				    */
#ifndef memoryPack
#define memoryPack              /* use memoryPack rather than mallocPack    */
#endif
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "ush.h"                /* shared memory hash utility               */
#include "ubs.h"                /* bitstring utility                        */

#include "ism.h"                /* shared memory abstraction                */
#include "iid.h"                /* id abstraction                           */
#include "ids.h"                /* data synchronization abstraction         */
#include "ipc.h"                /* process communication abstraction        */

#include "idd.h"                /* data definition abstraction              */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: isd_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef isd_main
  int isd_debug = 1;
#else
  extern int isd_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: isd_trans
| 
|  Description:  
|     The isd_trans abstract type is used to specify a given transaction.
| 
\* Definition:                                                              */

	typedef private isd_trans;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: isd_process
| 
|  Description:  
|     The process abstract type is used to define a process.
| 
\* Definition:                                                              */

	typedef private isd_process;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: isd_change_type
| 
|  Description:  
|     The isd change type defines the action to be taken with the changed
|     element record.
| 
\* Definition:                                                              */

	typedef enum change_type {
	  isd_no_change 	= -1,
	  isd_create		=  0,
	  isd_modify		=  1,
	  isd_remove		=  2,
	  isd_get		=  3
	} isd_change_type;

/* 
|  Components:
|      isd_no_change - Not changed or invalid change.
|      isd_create - Newly created shared data instance.
|      isd_modify - Modified existing shared data instance.
|      isd_remove - Remove existing shared data instance.
|      isd_get - Get value for existing shared data instance.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_create_process_definition
| 
|  Description:  
|     The create process definition routine creates a process structure to be 
|     used when starting transactions.
| 
\* Syntax:                                                                  */
                                                                
	isd_process isd_create_process_definition(
	  /* mailbox in : string */
	  /* pdd     in : string */
	);

/* 
|  Parameters:
|      mailbox - Receiver mailbox.
|      pdd - Receiver processed data definition file.
|
|  Returns:  
|      Handle of process record.
|  
|  Statuses:  
|     ok, out_of_memory, null_mailbox_name, null_ill_file_name, 
|     system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_destroy_process_definition
| 
|  Description:  
|     The destroy process definition routine is used to free all storage
|     associated with a process definition.
| 
\* Syntax:                                                                  */
                                                                
	void isd_destroy_process_definition(
	  /* process in : isd_process */
	);

/* 
|  Parameters:
|      process - Process definition to be destroyed.
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|      ok, invalid_process_handle.
|  
\*--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
|  Routine: isd_getfrom_process_table
|
|  Description:
|     isd maintains a table of mailbox number versus process record handle
|     The get from process table routine takes as input a mailbox number
|     and returns NULL (if the mailbox is not in the table) or the 
|     associated process record handle
\* Syntax:                                                                  */

         isd_process isd_getfrom_process_table (
           /*mbx in: ipc_mailbox*/
         );
/*  
|  Parameters: 
|      mbx - mailbox number
|
|  Returns:
|      process record associated with mailbox
|      NULL if no such record
|
|  Statuses:
|      no status change from this routine
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_mailbox
|
|  Description:
|     The get mailbox routine returns the mailbox associated with a given
|     process.
| 
\* Syntax:                                                                  */

	ipc_mailbox isd_get_mailbox(
	  /* process in :isd_process */
        );

/* 
|  Parameters:
|      process - Process record. 
|
|  Returns:  
|      Mailbox associated with process.
|  
|  Statuses:  
|     ok, invalid_process_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_data_definition
|
|  Description:
|     The get data definition routine returns the data definition associated 
|     with a given process.
| 
\* Syntax:                                                                  */

	idd_data_definition isd_get_data_definition(
	  /* process in : isd_process */
        );

/* 
|  Parameters:
|      process - Process record. 
|
|  Returns:  
|      Data definition associated with process.
|  
|  Statuses:  
|     ok, invalid_process_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_init
| 
|  Description:  
|     The init routine performs necessary initialization of the interface
|     layer for an application or technology (TAP).
| 
\* Syntax:                                                                  */
                                                                
	void isd_init(
	  /* mailbox in : string */
	  /* ill_filein : string */
	);

/* 
|  Parameters:
|      mailbox - Mailbox to be installed as "in box".
|      ill_file -  Processed data definition file.
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|     ok, out_of_memory, null_mailbox_name, null_ill_file_name, 
|     system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_input_open_ill_file
| 
|  Description:  
|     The input open ill file takes as input a file pointer of a file
|     open to the data definition section of an ill file and
|     reads in the ill file and creates the data definition.  It
|     is used internally by isd and externally by the slang compiler.
|     ill file is terminated by line of >>
| 
\* Syntax:                                                                  */
       
     void isd_input_open_ill_file(
          /*data_definition in/out: idd_data_definition */
          /*fp in: FILE */
     );

/* 
|  Parameters:
|      data_definition - handle to empty data definition table to be
|                        filled in by routine
|      fp - file pointer to open ill file
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|      none
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: isd_dm_init
| 
|  Description:  
|     The dm init routine performs necessary initialization of the interface
|     layer for the dialogue manager.
| 
\* Syntax:                                                                  */
                                                                
	void isd_dm_init ();

/* 
|  Parameters:
|      None.
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|     ok, out_of_memory, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: isd_dm_init_sdd
| 
|  Description:  
|     This routine performs necessary initialization of the shared data
|	data description structure/ill for the dialogue manager.
| 
\* Syntax:                                                                  */
                                                                
	void isd_dm_init (
		/* ill_file in : string */
	);

/* 
|  Parameters:
|      ill_file - name of ill file from which to creat a data definition, if
|		  null do not create a data definition
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|     ok, out_of_memory, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_start_dm_transaction
| 
|  Description:  
|     The start dm transaction routine is used to start a transaction 
|     intended for a specific receiver.
| 
\* Syntax:                                                                  */
                                                                
	isd_trans isd_start_dm_transaction(
	  /* receiver : in isd_process  */
	);

/* 
|  Parameters:
|     receiver - Process to receive transaction.
|
|  Returns:  
|      A unique transaction id.
|  
|  Statuses:  
|     ok, out_of_memory, overflow.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_start_transaction
| 
|  Description:  
|     The start transaction routine is used to define the start of a series
|     of data modifications.
|
\* Syntax:                                                                  */
                                                                
	isd_trans isd_start_transaction();

/* 
|  Parameters:
|      None.
|
|  Returns:  
|      A unique transaction id.
|  
|  Statuses:  
|     ok, out_of_memory, overflow.
|   
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_init_sd_element
|
|  Description:
|     The isd_init_sd_element routine is used to set an existing shared
|     data element to UNDEFINED.
|
\* Syntax:                                                                  */

	void isd_init_sd_element(
	  /* data_definition: in idd_data_definition	*/
	  /* element_name: in string			*/
	  /* data: in out caddr_t			*/
	);

/* 
|  Parameters:
|     data_definition - Data definition that contains the definition for
|       the specified element name.
|     element_name - Name of the shared data element. 
|     data - Pointer to the shared data element.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_add_shared_data
| 
|  Description:  
|     The add shared data routine creates an instance for the specified 
|     shared data element and returns a unique ID.  The shared data 
|     instanced may or may not be initialized.
| 
\* Syntax:                                                                  */

        iid_id_type isd_add_shared_data(
          /* transaction     : in isd_trans */
          /* element_name    : in string    */
          /* component_name  : in string    */
          /* data            : in caddr_t   */
        );

/* 
|  Parameters:
|     transaction - The transaction for which this operation is defined.
|     element_name - The name of the shared data element.
|     component_name - The name of a specific component to be initialized
|	with the data or null if the data corresponds to the entire element.
|     data - Shared data or null pointer if non-initialized.
|
|  Returns:  
|     A unique shared data id.
|  
|  Statuses:  
|     ok, out_of_memory, null_element_name, overflow.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_add_shared_data_with_id
| 
|  Description:  
|     The add shared data routine creates an instance for the specified 
|     shared data element.  The shared data 
|     instanced may or may not be initialized.
| 
\* Syntax:                                                                  */

        void isd_add_shared_data_with_id(
          /* transaction     : in isd_trans */
          /* element_name    : in string    */
          /* component_name  : in string    */
          /* data            : in caddr_t   */
          /* id		     : in iid_id_type*/
        );

/* 
|  Parameters:
|     transaction - The transaction for which this operation is defined.
|     element_name - The name of the shared data element.
|     component_name - The name of a specific component to be initialized
|	with the data or null if the data corresponds to the entire element.
|     data - Shared data or null pointer if non-initialized.
|     id - the id which is to be used for this data
|
|  Returns:  
|     nothing
|  
|  Statuses:  
|     ok, out_of_memory, null_element_name, overflow.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_put_shared_data
| 
|  Description:  
|     The put shared data call is used to put information into shared data.
| 
\* Syntax:                                                                  */

        void isd_put_shared_data(
          /* transaction    : in isd_trans */
          /* id             : in iid_id_type */
          /* element_name   : in string */
          /* component_name : in string */
          /* data           : in caddr_t * */
        );

/* 
|  Parameters:
|     transaction - The transaction to which the shared data should be put.
|     id - Shared data ID.
|     element_name - The name of the shared data element.
|     component_name - The name of the shared data component.
|     data - Shared data.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, undefined_shared_data_type, null_element_name, invalid_id.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: isd_remove_shared_data
| 
|  Description:  
|     The remove shared data routine is used to remove a specifed shared 
|     data element.
| 
\* Syntax:                                                                  */

        void isd_remove_shared_data(
          /* transaction  : in isd_trans */
          /* element_name : in string */
          /* id           : in iid_id_type */
        );

/* 
|  Parameters:
|     transaction - Transaction from which to remove the shared data element.
|     element_name - Name of element to be removed.
|     id - Existing shared data ID.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, out_of_memory, null_element_name, invalid_id.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_rollback_transaction
| 
|  Description:  
|     The rollback transaction routine is used to abort a given transaction 
|     and delete the associated transaction buffer.
| 
\* Syntax:                                                                  */

        void isd_rollback_transaction(
           /* transaction : in isd_trans */
        );

/* 
|  Parameters:
|     transaction - Existing transaction ID.
|
|  Returns:  
|     A handle to a newly created element.
|  
|  Statuses:  
|     ok, invalid_transaction_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_commit_transaction
| 
|  Description:  
|     The commit transaction routine is used to commit a transaction to the
|     data base and notify the list of intended receivers.
| 
\* Syntax:                                                                  */

        void isd_commit_transaction(
           /* transaction: in isd_trans */
        );

/* 
|  Parameters:
|     transaction - Existing transaction ID.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, out_of_memory, invalid_transaction_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_transaction
| 
|  Description:  
|     The get transaction routine is used to get the id for the next
|     completed transaction.
| 
\* Syntax:                                                                  */

        isd_trans isd_get_transaction();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     The transaction ID for a completed transaction.
|  
|  Statuses:  
|     ok, system_operation_failed.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_transaction_no_wait
| 
|  Description:  
|     The get transaction routine is used to get the id for the next
|     completed transaction.
| 
\* Syntax:                                                                  */

        isd_trans isd_get_transaction_no_wait();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     The transaction ID for a completed transaction.
|  
|  Statuses:  
|     ok, system_operation_failed, not_available.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_transaction_from_sender
| 
|  Description:  
|     The get transaction from sender routine is used to get the id for the
|     next completed transaction from a specific sender.  The current process
|     is suspended until a transaction arrives from the sender
| 
\* Syntax:                                                                  */

        isd_trans isd_get_transaction_from_sender(
	  /* sender : in ipc_mailbox */
	);

/* 
|  Parameters:
|     sender - Sender from which to receive transaction.
|
|  Returns:  
|     The transaction ID for a completed transaction.
|  
|  Statuses:  
|     ok, system_operation_failed
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_sender
| 
|  Description:  
|     The get sender routine returns the sender for the specified 
|     transaction.
| 
\* Syntax:                                                                  */

        ipc_mailbox isd_get_sender(
	  /* transaction : in isd_trans */
	);

/* 
|  Parameters:
|     transaction - Transaction for which to return receiver.
|
|  Returns:  
|     Mailbox id of the process that sent the transaction.
|  
|  Statuses:  
|     ok, invalid_transaction_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_specify_data_definition
| 
|  Description:  
|     The specify data definition allows the dialogue manager to specify
|     the data definition for the TAP from which the transaction was
|     received.
| 
\* Syntax:                                                                  */

        void isd_specify_data_definition(
	  /* transaction : in out isd_trans */
	  /* process     : in isd_process */
	);

/* 
|  Parameters:
|     transaction - Transaction for to specify process information.
|     process - Process record containing data definition for process.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_transaction_handle, invalid_process_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_first_changed_element
| 
|  Description:  
|     The get first changed element routine is used to get the id of the 
|     first changed element on a transaction list.
| 
\* Syntax:                                                                  */

        iid_id_type isd_get_first_changed_element(
          /* isd_trans : in transaction */
        );

/* 
|  Parameters:
|     transaction - Existing transaction ID.
|
|  Returns:  
|     The handle of the first changed element.
|  
|  Statuses:  
|     ok, invalid_transaction_handle, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_next_changed_element
| 
|  Description:  
|     The get next changed element routine is used to get the id of the 
|     next changed element on a transaction list or return iid_null_id
|     if transaction list is empty.
| 
\* Syntax:                                                                  */

        iid_id_type isd_get_next_changed_element(
          /* isd_trans : in transaction */
        );

/* 
|  Parameters:
|     transaction - Existing transaction ID.
|
|  Returns:  
|     The id of the next changed element or iid_null_id if none
|     available.
|  
|  Statuses:  
|     ok, invalid_transaction_handle, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_element_name
| 
|  Description:  
|     The get element name accepts an instance id as a parameter and returns 
|     the associated element name.
| 
\* Syntax:                                                                  */

	string isd_get_element_name(
          /* id : in iid_id_type */
	);

/* 
|  Parameters:
|     id - Existing shared data ID.
|
|  Returns:  
|     Element name associated with the shared data instance ID.
|  
|  Statuses:  
|     ok, invalid_id.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_change_type
| 
|  Description:  
|     The get change type routine accepts an instance id as a parameter and 
|     returns the associated change type
| 
\* Syntax:                                                                  */

	isd_change_type isd_get_change_type(
          /* id : in iid_id_type */
	);

/* 
|  Parameters:
|     id - Existing shared data ID.
|
|  Returns:  
|     Element name associated with the shared data instance ID.
|  
|  Statuses:  
|     ok, invalid_change_type, invalid_transaction_handle, invalid_id.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_create_changed_component_list
| 
|  Description:  
|     The create changed component name list routine accepts an instance id as
|     a parameter and returns the associated list of changed component names.
| 
\* Syntax:                                                                  */

        LIST isd_create_changed_component_list(
          /* id : in iid_id_type */
        );

/* 
|  Parameters:
|     id - Existing data instance id.
|
|  Returns:  
|     The list of changed component names associated with a data instance
|     or NULL if none.
|  
|  Statuses:  
|     ok, invalid_id, out_of_memory, element_not_a_record.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_destroy_changed_component_list
| 
|  Description:  
|     The destroy changed component list routine releases all storage
|     associated with a changed component list.
| 
\* Syntax:                                                                  */

        void isd_destroy_changed_component_list(
          /* changed_component_list : in out LIST */
        );

/* 
|  Parameters:
|     changed_component_list - List to be destroyed.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_incorporate_changes
| 
|  Description:  
|     The incorporate changes routine is used to incorporate changes into
|     the original data.
| 
\* Syntax:                                                                  */

        void isd_incorporate_changes(
          /* id   : in iid_id_type */
          /* data : in caddr_t     */
        );

/* 
|  Parameters:
|     id - Existing shared data ID.
|     data - Pointer to data with which to incorporate changes.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_id.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_shared_data
| 
|  Description:  
|     The get shared data routine allocates process memory, copies shared
|     data into process memory and returns a pointer to the data.
|
|     Warning:  Record components may not have been specified and therefore
|               do not contain valid data.
|
|     Note:     For Serpent release 1.0, get shared data only retrieves 
|		whole element records.
|
\* Syntax:                                                                  */

        caddr_t isd_get_shared_data(
          /* transaction	: in isd_trans */
          /* id			: in iid_id_type */
          /* component_name	: in string */
        );

/* 
|  Parameters:
|     transaction - Transaction in which to find shared data id.
|     id - Existing shared data id.
|     component_name - Name of component for which to retrieve data or
|		       entire element if NULL.
|
|  Returns:  
|     A pointer to changed data.
|  
|  Statuses:  
|     ok, invalid_id, out_of_memory, incomplete_record.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_shared_data_type
|
|  Description:
|     The get shared data type routine is used to get the type associated with
|     a shared data element.
|
\* Syntax:                                                                  */

        idd_data_types isd_get_shared_data_type(
          /* element_name    : in string */
          /* component_name  : in string */
        );

/*
|  Parameters:
|     element_name - The name of the shared data element.
|     component_name - The name of the shared data component or NULL.
|
|
|  Returns:
|     The type of the shared data element or record component.
|
|  Statuses:
|     ok, null_element_name.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_length
|
|  Description:
|     The get length routine returns the length of a specified shared data
|     element.
|
\* Syntax:                                                                  */

        int isd_get_length(
          /* element_name : in string */
        );
/*
|  Parameters:
|     element_name - Name of shared data element to search for.
|
|  Returns:
|     The length of the shared data element in bytes or 0 if not found.
|
|  Statuses:
|     ok, not_found.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_component_lop
|
|  Description:
|     The get component lop routine returns the length, offset and position
|     of a component within a shared data element.  Component positions are
|     numbered from 0 to n-1.
|
\* Syntax:                                                                  */

        void isd_get_component_lop(
          /* element_name    : in string */
          /* component_name  : in string */
          /* length          : out int */
          /* offset          : out int */
          /* position        : out int */
        );

/*
|  Parameters:
|     element_name - Name of the shared data element containing the component.
|     component_name - Component for which to return length and offset.
|     length - The length of the component (in bytes).
|     offset - The offset of the component within the element (in bytes).
|     position - The position of the component within the shared data record.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok, null_element_name, not_found.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_purge_transaction
| 
|  Description:  
|     The purge transaction routine is used to purge a received transaction
|     once the contents of the transaction have been examined and acted upon.
| 
\* Syntax:                                                                  */

        void isd_purge_transaction(
          /* transaction : in isd_trans */
        );

/* 
|  Parameters:
|     transaction - Existing transaction ID.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_id, illegal_receiver.
|     
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_dm_cleanup
| 
|  Description:  
|     The cleanup routine performs necessary clean up of the interface
|     layer for the dialogue manager.
| 
\* Syntax:								    */
                                                                
	void isd_dm_cleanup();

/* 
|  Parameters:
|      None.
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: isd_cleanup
| 
|  Description:  
|     The cleanup routine performs necessary clean up of the interface
|     layer for the application.  
| 
\* Syntax:								    */
                                                                
	void isd_cleanup();

/* 
|  Parameters:
|      None.
|
|  Returns:  
|      Void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*\
|  Routine: isd_get_dialogue_name
| 
|  Description:  
|     This routine returns the name of the current dialogue
| 
\* Syntax:								    */
                                                                
	char *isd_get_dialogue_name();

/* 
|  Parameters:
|      None.
|
|  Returns:  
|      pointer to current dialogue name
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/
#endif
