/*    $Header: /u/04c/mnt/integ/int/src/RCS/ipb.c,v 1.13 89/09/19 15:41:05 ljb Exp $       */
/*    $Log:	ipb.c,v $
 * Revision 1.13  89/09/19  15:41:05  ljb
 * reverse order of ipc_send calls and journaling calls
 * other process was modifying changes prior to journaling
 * 
 * Revision 1.12  89/07/18  13:17:49  bmc
 * Now correctly checks for null pointer after opening recording file.
 * 
 * Revision 1.11  89/06/05  09:03:38  ljb
 * make work with "remove" and fix minor bugs
 * 
 * Revision 1.7  89/06/01  15:21:02  ejh
 * fixed buffer recording; turned status tests into io_test
 * 
 * Revision 1.6  89/02/09  19:03:56  ejh
 * added copyright notice
 * 
 * Revision 1.5  88/11/02  21:43:28  rcs
 * changed debug1 to debug2 for call, exit and parameters
 * 
 * Revision 1.4  88/11/02  14:32:54  rcs
 * moved debug variable to include file - rCs
 * 
 * Revision 1.3  88/10/13  18:17:01  little
 * fix record to "work" in the presence of dm sd
 * 
 * Revision 1.2  88/10/12  14:35:39  ljb
 * fix bug which recorded wrong names and data
 *           */
/*--------------------------------------------------------------------------*\
| 
|  Name:  Playback abstraction (ipb)
|
|  Description:  
|     The playback abstraction provides entries to record and playback 
|     transactions within the interface layer.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     ipb.c - contains the implementation for the record abstraction.
|     ipb.h - contains the external definition for the playback abstr.
|     ipb2.c - contains implementation of the formatting program for the
|              recorded file
|     ipb3.c - contains implementation of the playback abstr
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

#define	ipb_main
#define	memoryPack

#include	<stdio.h>
#include	<errno.h>
#include	<sys/time.h>

#include	"hashPack.h"
#include	"listPack.h"
#include	"string.h"

#include	"ubs.h"
#include	"isc.h"
#include	"isd.h"
#include	"isd_priv.h"
#include	"iid.h"
#include	"idd.h"
#include	"ipc.h"
#include	"ipb.h"

#define	DEBUG1(statement)  SERPENT_DEBUG1(ipb_debug, statement)
#define	DEBUG2(statement)  SERPENT_DEBUG2(ipb_debug, statement)
#define	DEBUG3(statement)  SERPENT_DEBUG3(ipb_debug, statement)

struct	timezone	tzp;
FILE			*fp;
string			fred_buffer;
isd_trans		transaction;
iid_id_type		id;
HASH			id_hash;
process_record		*cur_process;
element_change_record	*current_element;
isd_change_type		change;
LIST			changed_component_list;

extern	process_record	in_box;

/*--------------------------------------------------------------------------*\
|  Routine: ipb_start_recording
| 
|  Description:  
|     The start recording routine enables recording.
\*--------------------------------------------------------------------------*/

void	ipb_start_recording (file_name, message)
string	file_name,
	message;

    {
    string	dial_name;
    int		len;

    DEBUG2 (fprintf (stderr, "ipb_start_recording: call.\n"));
    DEBUG3 (fprintf (stderr, "ipb_start_recording: file name = %s.\n",
	file_name));

    set_status (ok);
    check_null (isc_validate (&in_box, isc_process_record_vs),
	"ipb_start_recording: invalid_process_record\n",
	invalid_process_handle);
    if (in_box.recording)
	{
	set_status (already_recording);
	DEBUG1 (fprintf(stderr, "ipb_start_recording: already_recording\n"));
	return;
	}

    in_box.recording = on;
    in_box.record_fp = fopen (file_name, "w");
    check_null (in_box.record_fp, "ipb_start_recording: open failure\n",
	io_failure);

/* 
  write out file header.  The header consists of the string "JOURNAL:"
  followed by the dialogue name, the message (from the parameter list)
*/

    io_test (fwrite ("JOURNAL:", 8, 1, in_box.record_fp));
    dial_name = isd_get_dialogue_name ();
    len = strlen (dial_name) + 1;           /* allow for terminator*/
    io_test (fwrite (&len, sizeof(len), 1, in_box.record_fp));
    io_test (fwrite (dial_name, len, 1, in_box.record_fp));
    len = strlen (message) + 1;             /*allow for terminator*/
    io_test (fwrite (&len, sizeof (len), 1, in_box.record_fp));
    io_test (fwrite (message, len, 1, in_box.record_fp));

/*
   write out the owner record.  This consists of owner mailbox name, ill
   file name, mailbox name
*/

    len = strlen (in_box.ill_file_name) + 1;
    io_test (fwrite (&len, sizeof (len), 1, in_box.record_fp));
    io_test (fwrite (in_box.ill_file_name, len, 1, in_box.record_fp));
    len = strlen (in_box.mailbox_name) + 1;
    io_test (fwrite (&len, sizeof(len), 1, in_box.record_fp));
    io_test (fwrite (in_box.mailbox_name, len, 1, in_box.record_fp));

    DEBUG3 (fprintf (stderr, "ipb_start_recording: file pointer %d\n",
	in_box.record_fp));
    DEBUG2 (fprintf (stderr, "ipb_start_recording: exit\n"));

    return;
    }	/* end ipb_start_recording */

/*--------------------------------------------------------------------------*\
|  Routine: ipb_write_component
|
|  Description:
|    The write change routine writes the changes for one component to the file
|
\*--------------------------------------------------------------------------*/

void	ipb_write_component (changes,component_name)
ubs_bitstring	changes;
string		component_name;

    {
    int			comp_length,
			offset,
			position,
			name_len;
    idd_data_types	data_type;
    idd_buffer_type	*the_buffer;
    string		temp_string;

/*  
    this is a local routine called with an component name of an element
    change record which is to be recorded to the file.  The output of this
    routine is:  component name, value
*/

    DEBUG2 (fprintf (stderr, "ipb_write_component: call\n"));

    idd_get_component_lop (cur_process->data_definition,
	current_element->element_name, component_name, &comp_length, &offset,
	&position);

    if (ubs_test_bit (changes, position))
	{
	
	DEBUG3 (fprintf (stderr, "ipb_write_component: current element %x\n",
	    current_element);
	    fprintf (stderr, "ipb_write_component: element name %s\n",
		current_element->element_name);
	    fprintf (stderr, "ipb_write_component: component name %s\n",
		component_name);
	    fprintf (stderr, "ipb_write_component: component length %d\n",
		comp_length));

	name_len = strlen (component_name) + 1;      /*allow for terminator*/
	io_test (fwrite (&name_len, sizeof (name_len), 1, fp));
	io_test (fwrite (component_name, name_len, 1, fp));
	data_type = idd_get_shared_data_type (cur_process->data_definition,
	    current_element->element_name, component_name);
	if (data_type != idd_buffer)
	    {
	    io_test (fwrite ((current_element->data) + offset, comp_length, 1,
		fp));
	    }
	else
	    {
	    the_buffer = (idd_buffer_type *) (current_element->data + offset);
	    name_len = the_buffer -> length;
	    temp_string = make_node (name_len + 1);
	    (void) strncpy (temp_string, the_buffer -> body, name_len);
	    temp_string [name_len] = '\0';
	    io_test (fwrite (&name_len, sizeof (int), 1, fp));
	    io_test (fwrite (temp_string, name_len, 1, fp));
	    }
	}

    DEBUG2 (fprintf (stderr, "ipb_write_component: exit\n"));

    return;
    }	/* end ipb_write_component */

/*--------------------------------------------------------------------------*\
|  Routine: ipb_write_element
|
|  Description:
|    The write element writes the identification for one changed element
|    to the file
|
\*--------------------------------------------------------------------------*/

void	ipb_write_element (nada,change_element)
string			nada;
element_change_record	*change_element;

    {
    int		name_length,
		len;
    LIST	component_names;

/*
   this is a local routine which is called to write out the next element in
   the change list.  it writes out: element name, change type, id prior
   calls the component writing routine to record the components and the
   values and writes out a trailer.
*/

    DEBUG2 (fprintf(stderr, "ipb_write_element: call.\n"));
    current_element = change_element;
    name_length = strlen (current_element->element_name) + 1;  

/*
      allow for terminator
*/

    io_test (fwrite (&name_length, sizeof (name_length), 1, fp));
    io_test (fwrite (current_element->element_name, name_length, 1, fp));
    io_test (fwrite (&current_element->change_type,
	sizeof (current_element->change_type), 1, fp));
    io_test (fwrite (&current_element->id, sizeof (current_element->id), 1,
	fp));

DEBUG3(fprintf(stderr,"ipb_write_element: element %s, change type %d, id %d\n",
			current_element->element_name,
			current_element->change_type,
			current_element->id);)	

/* if we are deleting an element then there is nothing more to do here*/

   if (current_element->change_type == isd_remove) return;

/*
   we have written the element specific information, next create 
   component list and pass it and bitstring to component writing
   routine
*/

    component_names = idd_create_component_list
	(cur_process->data_definition, current_element->element_name);
    if (component_names)
	for_all_list (component_names, current_element->changed_components, 
	    ipb_write_component);
    check_status ("ipb_write_element: for_all_list");
    if (component_names)
	idd_destroy_component_list(component_names);
    len = NULL;
    io_test (fwrite (&len, sizeof (len), 1, fp));

    DEBUG2 (fprintf (stderr, "ipb_write_element: exit\n"));

    return;
    }	/* end ipb_write_element */

/*--------------------------------------------------------------------------*\
|  Routine: ipb_add_corr
| 
|  Description:  
|     The add correspondent routine adds a new participant to the
|     journaling file
\*--------------------------------------------------------------------------*/

void	ipb_add_corr (process_box)
process_record	*process_box;

    {
    int	len;

    DEBUG2 (fprintf (stderr, "ipb_add_corr: call.\n"));
    DEBUG3 (fprintf (stderr, "ipb_add_corr: process record %x.\n",
	process_box));

    io_test (fwrite ("PARTN:", 6, 1, in_box.record_fp));
    len = strlen (process_box->ill_file_name) + 1;
    io_test (fwrite (&len, sizeof (len), 1, in_box.record_fp));
    io_test (fwrite (process_box->ill_file_name, len, 1, in_box.record_fp));
    len = strlen (process_box->mailbox_name) + 1;
    io_test (fwrite (&len, sizeof (len), 1, in_box.record_fp));
    io_test (fwrite (process_box->mailbox_name, len, 1, in_box.record_fp));

    DEBUG2 (fprintf (stderr, "ipb_add_corr: exit.\n"));

    return;
    }	/* end ipb_add_corr */

/*--------------------------------------------------------------------------*\
|  Routine: ipb_record_transaction
| 
|  Description:  
|     The record transaction routine writes a transaction to record_fp.
\*--------------------------------------------------------------------------*/

void	ipb_record_transaction (change_table, sender_mbx, receiver_mbx)
HASH		change_table;
ipc_mailbox	sender_mbx,
		receiver_mbx;

    {
    int			len;
    struct	timeval	tod;
    process_record	*sender,
			*receiver;

    DEBUG2 (fprintf (stderr, "ipb_record_transaction: call.\n"));
    DEBUG3 (fprintf (stderr, "ipb_record_transaction: change_table %x.\n",
	change_table);
	fprintf (stderr, "ipb_record_transaction: sender %d.\n", sender_mbx);
	fprintf (stderr, "ipb_record_transaction: receiver %d.\n",
	    receiver_mbx));

    set_status (ok);
/*
   First find out if this transaction is to be recorded and determine
   the file pointer
*/
    if (in_box.recording == false)
	return;
    fp = in_box.record_fp;
/*
   then find out if the change table is empty
*/
    if (!change_table)
	return;
/*  
   now retrieve sender and receiver process records and make sure all are
   partipicants in the recording file
*/
    sender = (process_record *) isd_getfrom_process_table (sender_mbx);
    if (sender->recording == false)
	{
	ipb_add_corr (sender);
	sender->recording = true;
	}
    receiver = (process_record *) isd_getfrom_process_table (receiver_mbx);
    if (receiver->recording == false)
	{
	ipb_add_corr (receiver);
	receiver->recording = true;
	}
    cur_process = sender;
    if (!strcmp (sender->mailbox_name, "DM_BOX"))
	cur_process = receiver;
/*
   Each transaction has time of day, sender and receiver
*/
    DEBUG3 (fprintf (stderr, "ipb_record_transaction: fp. %d\n",fp));

    io_test (fwrite ("TRANS:", 6, 1, fp));
    gettimeofday (&tod, &tzp);
    io_test (fwrite (&tod, sizeof (tod), 1, fp));

    DEBUG3 (fprintf (stderr, "ipb_record_transaction: after tod\n"));

    len = strlen (sender->mailbox_name) + 1;          /*allow for terminator*/
    io_test (fwrite (&len, sizeof (len), 1, fp));
    io_test (fwrite (sender->mailbox_name, len, 1, fp));

    DEBUG3 (fprintf (stderr, "ipb_record_transaction: after sender name\n"));

    len = strlen (receiver->mailbox_name) + 1;      /* allow for terminator*/
    io_test (fwrite (&len, sizeof (len), 1, fp));
    io_test (fwrite (receiver->mailbox_name, len, 1, fp));

    DEBUG3 (fprintf (stderr,"ipb_record_transaction: after receiver name\n"));

/*
   now write out the elements and the changes
*/
    DEBUG3(fprintf(stderr,"\n");)
    for_all_hashtable (change_table, NULL, ipb_write_element);
    check_status ("ipb_record_transaction: for_all_hashtable");
    len = NULL;
    io_test (fwrite (&len, sizeof (len), 1, fp));      /*end of element list*/

    DEBUG2 (fprintf(stderr, "ipb_write_transaction: exit.\n"));

    return;
    }	/* end ipb_write_transaction */

/*--------------------------------------------------------------------------*\
|  Routine: ipb_stop_recording
| 
|  Description:  
|     The stop recording routine causes the transaction manager to stop
|     recording the operations associated with a given process.
\*--------------------------------------------------------------------------*/

void	ipb_stop_recording ()

    {

    DEBUG2 (fprintf (stderr, "ipb_stop_recording: call.\n"););

    set_status (ok);
/*
   Close recording file.
*/
    if (fclose (in_box.record_fp) == EOF)
	set_status (io_failure);
/*
   Set recording to "off".
*/
    in_box.recording = false;

    DEBUG2 (fprintf (stderr, "ipb_stop_recording: exit.\n"));

    return;
    }	/* end ipb_stop_recording */
