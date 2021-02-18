/* $Header: /u/04c/mnt/integ/int/include/RCS/ipb.h,v 1.7 89/10/23 14:24:48 ljb Exp $ */
/*--------------------------------------------------------------------------*\
|
|  Name:  Playback abstraction (ipb)
|
|  Description:  
|     The playback abstraction provides entries to record and playback
|     transactions within the interface layer.  The owner of a recording
|     is the process within which the call to record is made.  All of the
|     messages received or sent by the process through the interface are
|     recorded.  Filtering of conversations is done at playback time.  The
|     utility: format_journaling  will produce a formatted record of the
|     journaling file
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     ipb.c - contains the implementation for the playback abstraction.
|     ipb.h - contains the external definition for the playback abstr.
|     ipbtest.c - test file.
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
|     Len Bass
|     ljb@sei.cmu.edu  
|
|  Change History:
|     Feb. 23, 1988 : Module created.
|     $Log:	ipb.h,v $
 * Revision 1.7  89/10/23  14:24:48  ljb
 * add additional external call: get_mailbox 
 * 
 * Revision 1.6  89/09/19  15:55:33  ljb
 * set ipb_get_next_transaction_header to boolean
 * 
 * Revision 1.5  89/06/01  15:33:36  ejh
 * defined io_test macros
 * 
 * Revision 1.4  89/02/09  18:42:17  ejh
 * added copyright notice
 * 
 * Revision 1.3  88/11/02  15:45:30  rcs
 * *** empty log message ***
 * 
 * Revision 1.2  88/11/02  14:35:44  rcs
 * moved debug variable from c file - Rcs
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

#ifndef	ipb
#define	ipb
#define	ALL_CORRESP "all_correspondents"
#define	DM_BOX "DM_BOX"
#define	HASH_TABLE_SIZE	5000

/*--------------------------------------------------------------------------*\
|  Includes: ipb
| 
|  Description:  
|     The following is a list of all the necessary includes for the ipb
|     abstraction.
| 
\* Definition:                                                              */

#include "hashPack.h"		/* C Toolbox - hashPack                     */

#include "isc.h"		/* status codes                             */
#include "isd.h"		/* shared data abstraction                  */
#include "isd_priv.h"		/* private data types                       */

/* 
\*--------------------------------------------------------------------------*/

#define	io_test(thing)	check_null (thing, "", io_failure)
#define	io_test_return(thing,thing2) \
	rcheck_null (thing, "", io_failure, thing2)

/*--------------------------------------------------------------------------*\
|  Global Variable: ipb_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef ipb_main
  int ipb_debug = 1;
#else
  extern int ipb_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
| Type: ipb_corr_type
|
| Description:
|  The ipb_corr_type enumerates the possible statuses that a mailbox
|  which has occurred on a journaling file might have.
|
\* Definition:								*/

	typedef	enum	corr_type {
	  uninvolved			=0,
	  host				=1,
	  correspondent			=2
	} ipb_corr_type;

/*									    *\
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_start_recording
| 
|  Description:  
|     The start recording routine enables recording, opens a file 
|     and writes out a header consisting of:
|       1. identifier: "JOURNAL:"
|       2. length and name of dialogue being recorded
|       3. length and user supplied message
|       4. owner mailbox and ill file name.
| 
\* Syntax:                                                    */

        void ipb_start_recording(
          /* file_name   : in string            */
          /* message     : in string            */
        );

/* 
|  Parameters:
|     file_name - File to which to write journaling.
|     message - string to be placed as a portion of the preamble of file
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, io_failure, already_recording
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_record_transaction
|
|  Description:
|     record one transaction.  The record written consists of:
|       0. TRANS:
|       1. Time of Day
|       2. sender mailbox name
|       3. receiver mailbox name
|       4. for each change element in hash table
|          a. change element (if Null length then end of list)
|             element name and length of name
|             change type
|             current id
|          b. data being changed (NULL length terminates components changed)
|              component name and length of name
|              value 
|      if the transaction is between correspondents who have not been
|      recorded on the file, then the correspondent's mailbox and ill file
|      are recorded on the journaling file
|
\* Syntax:                                                                  */

        void ipb_record_transaction(
          /* change_table : in HASH*/
          /* sender : in ipc_mailbox  */
          /* receiver: in ipc_mailbox */
        );

/* 
|  Parameters:
|     change_table - pointer to hash table for element changes
|     sender - mailbox of sender
|     receiver - mailbox of receiver
|
|  Returns:  
|     Void.
|
|  
|  Statuses:  
|     ok, io_failure
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_stop_recording
| 
|  Description:  
|     The stop recording routine causes the journaling facility to terminate
|     recording for this invoking process
| 
\* Syntax:                                                                  */

        void ipb_stop_recording();

/* 
|  Parameters:
|     None
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, io_failure, invalid_process_record
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_begin_playback
| 
|  Description:  
|     The playback routine causes the playback manager to open a
|     journaling file recorded earlier.  The owner of the journaling file
|     is read in and recorded in the mailbox hash table.
|
\* Syntax:   

                                                               */

        void ipb_begin_playback(
          /* file_name   : in string */
          /* host_mbx    : in string */
          /* dial_name   : out string*/
          /* message     : out string*/
        );

/* 
|  Parameters:
|     file_name - File to which journaling was previously done
|     host_mbx - name of mail box which is to be simulated by playback
|     dial_name - name of dialogue which created journaling file
|     message - string given by creater of journaling file
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, end_of_file, invalid_journal_file.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_next_transaction_header
| 
|  Description:  
|     This routine returns the participants in the conversation of which
|     the next transaction is a portion.  It also returns the time of day
|     that the transaction was recorded.  If eof then it returns false.
| 
\* Syntax:                                                                  */

        boolean ipb_get_next_transaction_header(
          /* sender : out string*/
          /* receiver : out string*/
          /* tod : out struct timeval*/
        );

/* 
|  Parameters:
|     sender - mailbox name of sender process for next transaction
|     receiver - mailbox name of receiver process for next transaction
|     tod - time of day of next transaction     
|
|  Returns:  
|     true if another transaction, false if not
|  
|  Statuses:  
|     ok, end_of_file.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_playback_transaction
| 
|  Description:  
|     This routine reads the next transaction, maps the ids for all
|     creates and enters changed data into a transation.  It is
|     intended for use when simulating the send of a transaction of interest.
| 
\* Syntax:                                                                  */

        void ipb_playback_transaction();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, end_of_file
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_map_transaction
| 
|  Description:  
|     This routine reads the next transaction and maps the ids for all
|     creates but does not actually simulate the transaction.  It is
|     intended for use when simulating the receiver.
| 
\* Syntax:                                                                  */

        void ipb_map_transaction(
          /* trans_id : in isd_trans	*/
        );

/* 
|  Parameters:
|     trans_id - id of transaction which has been accepted outside
|                the playback abstraction
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, inconsistent_jour_file, end_of_file
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_skip_transaction
| 
|  Description:  
|     This routine skips the next transaction and ignores any information
|     on transaction.  It is intended for use when one of the correspondents 
|     is uninvolved in the playback
| 
\* Syntax:                                                                  */

        void ipb_skip_transaction(
        );

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, end_of_file
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_end_playback
| 
|  Description:  
|     This routine terminates playback, closes the journaling file,
|     and cleans up all of the interprocess activity.
| 
\* Syntax:                                                                  */

        void ipb_end_playback(
        );

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, io_failure.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_process
| 
|  Description:  
|     This routine will retrieve the process record for a particular mbx
|     It should appropriately be in the interface, but since it isn't -
|     at the moment ...
|
\* Syntax:						*/

       isd_process ipb_get_process(
         /*mbx : in string  */
       );
/*
|  Parameters:
|    mbx - string version mail box name
|
|  Returns:
|    process record pointer of mailbox (NULL if doesn't exist)
|
|  Statuses:
|    none returned
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_add_correspondent
| 
|  Description:  
|     This routine will set a particular mailbox to be a correspondent and 
|     do the approriate communication initialization
|
\* Syntax:							*/

      void ipb_add_correspondent(
       /* mbx  : in string	*/
      );

/*
|  Parameters:
|    mbx - mailbox name for correspondent to be added (ALL_CORRES if
|          all of the correspondents on the journaling file are to
|          be involved in simulation
|
|  Returns:
|    Void
|
|  Statuses:
|    none
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ipb_playback_status
| 
|  Description:  
|     This routine will determine whether a particular mail box
|     is the host, a correspondent or uninvolved with this simulation
|
\* Syntax:								    */

      ipb_corr_type ipb_playback_status(
       /* mbx  : in string	*/
      );

/*
|  Parameters:
|    mbx - mailbox name to determine status for
|
|  Returns:
|    enumeration type of status
|
|  Statuses:
|    none
\*--------------------------------------------------------------------------*/

#endif
