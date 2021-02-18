static char rcsid [] =
	"$Header: /u/04c/mnt/integ/int/src/RCS/ipb3.c,v 1.12 89/10/23 14:26:27 ljb Exp $";
/*--------------------------------------------------------------------------*\
| 
|  Name:  Playback abstraction (ipb3)
|
|  Description:  
|     The playback abstraction provides entries to playback 
|     transactions within the interface layer.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     ipb.c - contains the implementation for the record abstraction.
|     ipb2.c - contains the formatting of a journaling file
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
|     May 17, 1988 : Module created.
|     $Log:	ipb3.c,v $
 * Revision 1.12  89/10/23  14:26:27  ljb
 * make playback work when playing back non dm process
 * 
 * Revision 1.11  89/09/29  16:39:39  ljb
 * work on some bugs discovered when using playback command
 * current copy works for playing back dm but not saw
 * 
 * Revision 1.10  89/09/21  11:38:39  ljb
 * no change
 * 
 * Revision 1.9  89/09/21  09:00:52  ljb
 * no change
 * 
 * Revision 1.8  89/09/19  16:19:06  ljb
 * change get_next_transaction into binary
 * 
 * Revision 1.7  89/09/19  14:27:46  ljb
 * set and test status after ipb_map calls
 * 
 * Revision 1.6  89/06/02  10:42:37  trm
 * just a checkout/in test.  trm
 * 
 * Revision 1.5  89/06/01  15:22:12  ejh
 * fixed buffer record/playback; turned status tests into io_test
 * 
 * Revision 1.4  89/02/09  19:04:52  ejh
 * added copyright notice
 * 
 * Revision 1.3  88/11/02  15:40:54  rcs
 * *** empty log message ***
 * 
 * Revision 1.2  88/11/02  14:42:12  rcs
 * moved debug variable to ipb.h - rCs
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

#define	memoryPack

#include	<stdio.h>
#include	<errno.h>
#include	<sys/time.h>

#include	"hashPack.h"
#include	"listPack.h"

#include	"global.h"
#include	"string.h"
#include	"isc.h"
#include	"isd.h"
#include	"isd_priv.h"
#include	"iid.h"
#include	"ipb.h"

#define DEBUG1(statement)  SERPENT_DEBUG1(ipb_debug, statement)
#define DEBUG2(statement)  SERPENT_DEBUG2(ipb_debug, statement)
#define DEBUG3(statement)  SERPENT_DEBUG3(ipb_debug, statement)

#define MBX_HASH_TABLE_SIZE 23
#define ID_NOT_IN_TABLE -1

    struct	timezone	tzp;
    FILE			*fp;
    string			fred_buffer;
    isd_trans			transaction;
    iid_id_type			id;
    int				bitcount;
    HASH			id_hash_table,
				mbx_hash;

    element_change_record	*current_element;
    isd_change_type		change;
    LIST			changed_component_list;
    isd_process			cur_receiver;
    isd_process			cur_sender;

    extern	process_record	in_box;
    extern	isd_process	out_box;

    typedef	struct {
	iid_id_type	prev_id;
	iid_id_type	cur_id;
    } id_mapping;

    typedef	struct {
	char		mbx_name[32];
	char		ill_file_name [32];
	isd_process	process;
	ipb_corr_type	status;   
    } mbx_control_block;

    mbx_control_block	*host_struct;
    ipb_corr_type	default_status = uninvolved;   

/*--------------------------------------------------------------------------*\
|  Routine: ipb_mbx_cmp
|
|  Description:
|	This routine compares two mailbox names to see if same
\*--------------------------------------------------------------------------*/

boolean	ipb_mbx_cmp (test_mbx, mbx_struc)
string			test_mbx;
mbx_control_block	*mbx_struc;

    {
    return (!strcmp (test_mbx, mbx_struc->mbx_name));
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_mbx_hash
|
|  Description:
|	This routine will calculate the hash function for mail box names
\*--------------------------------------------------------------------------*/

int	ipb_mbx_hash (mbx_name)
string	mbx_name;

    {
    register	string	cp = mbx_name;
    register	int	i = 0;
    int			sh;

    while (*cp)
	i = i*2 + *cp++;
    sh = (i & 077777) % MBX_HASH_TABLE_SIZE;
    return (sh);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_init_mbx
|
|  Description:
|	This routine initializes the mbx hash table
\*--------------------------------------------------------------------------*/

void	ipb_init_mbx ()

    {
    mbx_hash = make_hashtable (MBX_HASH_TABLE_SIZE, ipb_mbx_hash,
	ipb_mbx_cmp);
    return;
    }

/*-------------------------------------------------------------------------*\
|  Routine: free_mbx_nodes
|
|  Description:
|	This routine frees the nodes associated with each element
\*--------------------------------------------------------------------------*/

void	free_mbx_nodes (dummy, node)
string			dummy;
mbx_control_block	*node;

    {
    free_node (node);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_free_mbx
|
|  Description:
|	This routine cleans up the mailbox table
\*--------------------------------------------------------------------------*/

void	ipb_free_mbx ()

    {
    for_all_hashtable (mbx_hash, NULL, free_mbx_nodes);
    free_hashtable (mbx_hash, NOT_NODES);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_mbx_entry
|
|  Description:
|	This routine retrieves a mbx entry and create a new one, if necessary
\*--------------------------------------------------------------------------*/

mbx_control_block	*ipb_get_mbx_entry (mbx)
string	mbx;

    {
    mbx_control_block	*mbx_pointer;

    mbx_pointer = (mbx_control_block *) get_from_hashtable (mbx_hash, mbx);
    if (mbx_pointer)
	return (mbx_pointer);
/*
  mbx not in table, initialize ill file to empty and process record
  to NULL
*/
    mbx_pointer = (mbx_control_block *) make_node (sizeof(mbx_control_block));
    strcpy (mbx_pointer->mbx_name, mbx);
    mbx_pointer->ill_file_name [0] = '\0';
    mbx_pointer->process = NULL;
    add_to_hashtable (mbx_hash, mbx_pointer, mbx);
    mbx_pointer->status = uninvolved;
    return (mbx_pointer);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_process
|
|  Description:
|	This routine retrieves the process record for a particular mbx
\*--------------------------------------------------------------------------*/

isd_process	ipb_get_process (mbx)
string	mbx;

    {
    mbx_control_block	*mbx_pointer;

    mbx_pointer = ipb_get_mbx_entry (mbx);
    return (mbx_pointer->process);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_set_host_ill_file
|
|  Description:
|	This routine does the initialization once the hosts ill file has
|       been located.  The initializatio is different when the host is
|       the dm from when it is a tap
\*--------------------------------------------------------------------------*/

void		ipb_set_host_ill_file (mbx)
string 	mbx;

{
/*
  if we have the dialogue manager as host then initialize and copy in_box
*/

   DEBUG2(fprintf(stderr,"ipb_set_host_ill_file call: mbx: %s\n", mbx);)

      if(!strcmp(mbx, DM_BOX))
        {
         isd_dm_init();
         /*  it doesn't matter whether there is a dm.ill since that
             doesn't show up in record*/
         isd_dm_init_sdd(NULL);
	     }

/*
  the host is not the dialogue manager.  Therefore must be a tap
  and can do normal initialization
*/
     else
       {
        isd_init(host_struct->mbx_name, host_struct->ill_file_name);
       }

   host_struct->process = (isd_process) &in_box;
   return;
	     }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_add_corresponent
|
|  Description:
|	This routine sets a particular mailbox to be a correspondent and do
|	the approriate communication initialization.  If it is called with
|	ALL_CORRESP then the default for mailboxes is set to correspondent
\*--------------------------------------------------------------------------*/

void	ipb_add_correspondent (mbx)
string	mbx;

    {
    mbx_control_block	*mbx_pointer;

    if (!strcmp (mbx, ALL_CORRESP))
	{
	default_status = correspondent;
	return;
	}
    mbx_pointer = ipb_get_mbx_entry (mbx);
    mbx_pointer->status = correspondent;
    if (!(mbx_pointer->process))
/*
  if we have the dialogue manager as host and ill file filled in
   then initialize process record
*/
	if (!strcmp (host_struct->mbx_name, DM_BOX) &&
	    strcmp (mbx_pointer->ill_file_name, ""))
	    mbx_pointer->process = isd_create_process_definition
		(mbx_pointer->mbx_name, mbx_pointer->ill_file_name);
/*
  the host is not the dialogue manager, therefore must be a tap and
  can pick up outbox if host has been initialized
*/
	else if (host_struct->process)
	    mbx_pointer->process = out_box;
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_set_ill_file
|
|  Description:
|	This routine sets the ill file entry of the mail box table
\*--------------------------------------------------------------------------*/

mbx_control_block	*ipb_set_ill_file (mbx, ill_file)
string	mbx,
	ill_file;

    {
    mbx_control_block	*mbx_pointer;

    mbx_pointer = ipb_get_mbx_entry (mbx);
/*
   copy in the ill file name.  The communication will be initialized
   through set_host or add_correspondent called either here, from the user
   or through status routine
*/
    strcpy (mbx_pointer->ill_file_name, ill_file);
    if (mbx_pointer->status == host)
	ipb_set_host_ill_file (mbx);
    else if (mbx_pointer->status == correspondent || default_status == correspondent)
	ipb_add_correspondent (mbx);
    return (mbx_pointer);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_playback_status
|
|  Description:
|	This routine returns the current status of the mailbox.  If
|	ALL_CORRESP is set and the particular mailbox has an uninvolved
|	status, then it is added to the correspondence list.
\*--------------------------------------------------------------------------*/

ipb_corr_type	ipb_playback_status (mbx)
string	mbx;

    {
    mbx_control_block	*mbx_pointer;

    mbx_pointer = ipb_get_mbx_entry (mbx);
    if (mbx_pointer->process)
	return (mbx_pointer->status);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_map_cmp
|
|  Description:
|	This routine compares two ids to see if same
\*--------------------------------------------------------------------------*/

boolean	ipb_map_cmp (test_id, id_struc)
iid_id_type	test_id;
id_mapping	*id_struc;

    {
    return (test_id == id_struc->prev_id);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_map_hash
|
|  Description:
|	This routine calculates the hash function for id
\*--------------------------------------------------------------------------*/

int	ipb_map_hash (test_id)
iid_id_type	test_id;

    {
    return ((((int) test_id) & 077777) % HASH_TABLE_SIZE);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_init_map
|
|  Description:
|	This routine initializes the mapping table for new_ids and old_ids
\*--------------------------------------------------------------------------*/

void	ipb_init_map ()

    {
    id_hash_table = make_hashtable (HASH_TABLE_SIZE, ipb_map_hash,
	ipb_map_cmp);
    return;
    }

/*-------------------------------------------------------------------------*\
|  Routine: free_map_nodes
|
|  Description:
|	This routine frees the nodes associated with each element
\*--------------------------------------------------------------------------*/

void	free_map_nodes (dummy, node)
string		dummy;
id_mapping	*node;

    {
    free_node (node);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_cleanup_map
|
|  Description:
|	This routine cleana up the mapping table for new_ids and old_ids
\*--------------------------------------------------------------------------*/

void	ipb_cleanup_map ()

    {
    for_all_hashtable (id_hash_table, NULL, free_map_nodes);
    free_hashtable (id_hash_table, NOT_NODES);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_mapped_id
|
|  Description:
|	This routine returns the new id when passed an old id
\*--------------------------------------------------------------------------*/

iid_id_type	ipb_get_mapped_id (old_id)
iid_id_type	old_id;

    {
    id_mapping	*id_elem;
/*
   use the get_from_hash routine to retrieve and then pull out
   the id to return
*/
    if (old_id == 0) return (0);
    if (!(id_elem = (id_mapping *) get_from_hashtable(id_hash_table, old_id)))
	return (ID_NOT_IN_TABLE);
    return (id_elem->cur_id);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_map_new_id
|
|  Description:
|	This routine adds a new id, old id pair to the mapping table
\*--------------------------------------------------------------------------*/

void	ipb_map_new_id (new_id, old_id)
iid_id_type	new_id,
		old_id;

    {
    id_mapping	*temp;
/*  
   this routine creates a node with old_id, new_id in it.
*/
    if (old_id == 0) return;
    temp = (id_mapping *) make_node (sizeof (id_mapping));
    temp->cur_id = new_id;
    temp->prev_id = old_id;
    add_to_hashtable (id_hash_table, temp, old_id);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_process_components
|
|  Description:
|	This routine processwa the component portion of the journaling file.
|	Each component is added to the current transaction if process flag is
|	true and is ignored if process flag is false.  The component list is
|	terminated by a NULL length for the component name
\*--------------------------------------------------------------------------*/

void	ipb_process_components (transact, new_id, elem_name, process_flag)
isd_trans		transact;
iid_id_type		new_id;
string			elem_name;
boolean			process_flag;


    {
    int			len,
			bufflen,
			offset,
			position;
    char		comp_name [32];
    string		buffer;
    iid_id_type		map_id;
    idd_data_definition	data_def;
/*
   this routine reads in all of the components on the journaling file, one
   at a time.  Each component is: length of component name, component name, 
   value.  the list of components is null terminated (length is null).
   if process flag is true then the ids are mapped and the component is 
   added to the transaction.  If false then the next component is read
*/
    DEBUG2 (fprintf (stderr, "ipb_process_components: call\n"));

    if (strcmp (((process_record *)cur_receiver)->mailbox_name,DM_BOX) != 0)
         data_def = ((process_record *)cur_receiver)->data_definition;
    else
         data_def = ((process_record *)cur_sender)->data_definition;

    io_test (fread (&len, sizeof (len), 1, fp));
    while (len)
	{
	io_test (fread (comp_name, len, 1, fp));
        comp_name[len] = '\0';
        DEBUG3(fprintf (stderr, "ipb_process_components: component name %s\n",comp_name));
	idd_get_component_lop (data_def, elem_name, comp_name, &bufflen, &offset,
	    &position);
	check_status ("ipb_process_components: from isd_get_component_lop");
	buffer = make_node (bufflen);
	io_test (fread (buffer, bufflen, 1, fp));
	if (process_flag)             /*map if id type*/
	    {
	    if (idd_get_shared_data_type (data_def, elem_name, comp_name) == idd_id)
		{
		map_id = ipb_get_mapped_id (*((int *)buffer));
                if (map_id == ID_NOT_IN_TABLE) {
                   map_id = iid_create_id();
                   check_status ("ipb_process_components: from isd_add_shared_data");
                   ipb_map_new_id (map_id,*((int *)buffer));
		}
		isd_put_shared_data (transact, new_id, elem_name, comp_name,
		    &map_id);
		}
	    else
		isd_put_shared_data (transact, new_id, elem_name, comp_name,
		    buffer);
	    check_status ("ipb_process_components: from isd_put_shared_data");
	    }
	free_node (buffer);
	io_test (fread (&len, sizeof (len), 1, fp));
	}
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_read_element_header
|
|  Description:
|	The read element routine assumes that the journaling file is
|	positioned at an element header and reads in the information written
|	on that header; it returns non zero if there is an element name and
|	zero if there are no more
\*--------------------------------------------------------------------------*/

boolean	ipb_read_element_header (element_name, old_id, chg_type)
string		element_name;
iid_id_type	*old_id;
isd_change_type	*chg_type;

    {
    boolean	more = FALSE;
    int		len;

    DEBUG2 (fprintf (stderr, "ipb_read_element_header: call\n"));

    set_status (ok);

    set_status (ok);
    io_test_return (fread (&len, sizeof (len), 1, fp), FALSE);
    if (len)
	{
	io_test_return (fread (element_name, len, 1, fp), FALSE);
        element_name[len] = '\0';
    DEBUG3(fprintf (stderr, "ipb_read_element_header: element name %s\n",element_name));
	io_test_return (fread (chg_type, sizeof (chg_type), 1, fp), FALSE);
	io_test_return (fread (old_id, sizeof (old_id), 1, fp), FALSE);
	more = TRUE;
	}
    DEBUG2 (fprintf (stderr, "ipb_read_element_header: return\n"));
    return (more);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_begin_playback
|
|  Description:
|	The playback routine causes the playback manager to open a journaling
|	file recorded earlier.  The owner of the journaling file is read in
|	and recorded in the mailbox hash table.
\*--------------------------------------------------------------------------*/

void	ipb_begin_playback (file_name, host_mbx, dial_name,message)
string	file_name,
	host_mbx,
	dial_name,
	message;

    {
    char	check_string [9],
		ill_file_name [32],
		mbx_name [32];
    int		len;

ipb_debug = 3;
    DEBUG2 (fprintf (stderr, "ipb_begin_playback: call.\n"));
    DEBUG3 (fprintf (stderr, "ipb_begin_playback: file name = %s.\n",
	file_name));
    DEBUG3 (fprintf (stderr, "ipb_begin_playback: host mbx = %s.\n",
	host_mbx));

    set_status (ok);
/*
   Create/open file for playback.
*/
    fp = fopen (file_name, "r");

    if (!fp) fprintf (stderr,"ipb_begin_playback: illegal file %s\n",file_name);
    check_null (fp, "serpent_init:  error opening file.\n",
	invalid_journal_file);

    (void) fread (check_string, 8, 1, fp);
    check_string [8] = '\0';
    if (strcmp (check_string, "JOURNAL:"))
	{
	set_status (invalid_journal_file);
	fclose (fp);
	return;
	}

    ipb_init_mbx ();

/*
  create host mailbox
*/
    host_struct = ipb_get_mbx_entry(host_mbx);
    host_struct->status = host;
/*
  read in dialogue name and message
*/
    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (dial_name, len, 1, fp));
    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (message, len, 1, fp));
/*
  now add owner of correspondence to mbx table
*/
    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (ill_file_name, len, 1, fp));
    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (mbx_name, len, 1, fp));

    ipb_set_ill_file (mbx_name, ill_file_name);

    ipb_init_map ();

    DEBUG2 (fprintf (stderr, "ipb_begin_playback: exit.\n"));

    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_next_transaction_header
|
|  Description:
|	This routine returns the header information from the next transaction.
|	It assumes that the journaling file is positioned to the header of the
|	transaction.
\*--------------------------------------------------------------------------*/

boolean	ipb_get_next_transaction_header (sender, receiver, tod)
string		sender,
		receiver;
struct	timeval	*tod;

    {
    int		len,
		count;
    char	tempstr [7],
		ill_file_name [32],
		mbx_name [32];
    boolean	trans_flag = off;

    DEBUG2 (fprintf (stderr, "ipb_get_next_trans_header: call\n"));
    DEBUG3 (fprintf (stderr, "ipb_get_next_trans_header: file pointer:", fp));

    set_status (ok);
/*
   this routine assumes that the journaling file is positioned either at
   the start of a transaction or at the information for the next participant
   the distinction is made by the first 6 chars which are either PARTN or
   TRANS
*/
    while (trans_flag == off)
	{
	if (fread (tempstr, 6, 1, fp) == 0)return (false);
	tempstr [6] = '\0';
	if (!strcmp (tempstr, "PARTN:"))
	    {
	    io_test (fread (&len, sizeof (len), 1, fp));
	    io_test (fread (ill_file_name, len, 1, fp));
	    io_test (fread (&len, sizeof (len), 1, fp));
	    io_test (fread (mbx_name, len, 1, fp));
	    ipb_set_ill_file (mbx_name, ill_file_name);
	    }
	else if (!strcmp (tempstr, "TRANS:"))
	    {
	    trans_flag = on;
/*
         now read time of day, sender mailbox name and receiver mailbox name
*/
	    io_test (fread (tod, sizeof (struct timeval), 1, fp));
	    io_test (fread (&len, sizeof (len), 1, fp));
	    io_test (fread (sender, len, 1, fp));
	    io_test (fread (&len, sizeof (len), 1, fp));
	    io_test (fread (receiver, len, 1, fp));
	    }
	}
    ipb_playback_status (receiver);	
    ipb_playback_status (sender);
    cur_receiver = ipb_get_process (receiver);
    cur_sender = ipb_get_process (sender);

    DEBUG2 (fprintf (stderr, "ipb_get_next_trans_header: exit\n"));

    return (true);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_playback_transaction
|
|  Description:
|	This routine plays back an entire transaction.  It reads the
|	transaction from the journaling file and prior to return, it commits
|	the transaction.
\*--------------------------------------------------------------------------*/

void	ipb_playback_transaction ()

    {
    isd_trans		transact;
    char		elem_name [32];
    isd_change_type	chg_type;
    iid_id_type		old_id,
			new_id;

    DEBUG2 (fprintf (stderr, "ipb_playback_transaction: call\n"));

    set_status (ok);

    transact = isd_start_dm_transaction (cur_receiver);
    check_status ("ipb_playback trans: from isd_start_dm_transaction");
    while (ipb_read_element_header (elem_name, &old_id, &chg_type))
	{
	check_status ("ipb_playback trans: from ipb_read_element_header");

    DEBUG3 (fprintf (stderr, "ipb_playback_transaction: change_type %d\n", chg_type));
/*
   if change type is remove, then we find the current id from the
   mapping table and remove the new id
*/
	if (chg_type == isd_remove)
	    {
	    new_id = ipb_get_mapped_id (old_id);
	    isd_remove_shared_data (transact, elem_name, new_id);
	    check_status ("ipb_playback trans: from isd_remove_shared_data");
	    }
/*
    if change type is create, then we see if id has aleady been mapped, if
    it has then we use existing mapping, if it has not then we map it
    and use new mapping.
*/
	else if (chg_type == isd_create)
	    {
	    new_id = ipb_get_mapped_id (old_id);
	    if (new_id == ID_NOT_IN_TABLE)
		{
		new_id = iid_create_id();
		check_status ("ipb_playback trans: from isd_add_shared_data");
		ipb_map_new_id (new_id, old_id);
		}
            isd_add_shared_data_with_id(transact, elem_name, NULL, NULL, new_id);
	    ipb_process_components (transact, new_id, elem_name, TRUE);
	    check_status ("ipb_playback trans: from ipb_process_components");
	    }
/*
    if we have a modify, then the id must already have been mapped and
    we can get the old value
*/
	else if (chg_type == isd_modify)
	    {
	    new_id = ipb_get_mapped_id (old_id);
	    ipb_process_components (transact, new_id, elem_name, TRUE);
	    check_status ("ipb_playback_trans: after ipb_process_components");
	    }
	}
    isd_commit_transaction (transact);
    check_status ("ipb_playback trans: from isd_commit_transaction");

    DEBUG2 (fprintf (stderr, "ipb_playback_transaction: exit\n"));

    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_map_transaction
|
|  Description:
|	This routine skips the information associated with the current
|	transaction on the journaling file.  When it does this, it maps the
|	ids from the journaling file with the actual ids from the current
|	processes
\*--------------------------------------------------------------------------*/

void	ipb_map_transaction (trans_id)
isd_trans	trans_id;

    {
    isd_change_type	chg_type;
    iid_id_type		old_id,
			new_id;
    char		elem_name [32];
    boolean		more_element;


    DEBUG2 (fprintf (stderr, "ipb_map_transaction: call\n"));

    set_status (ok);

/*
   elements are read in from the journaling file and from the actual
   transaction, if the elements represent a create, then the new and old
   ids are mapped.  Component information is skipped on this side.
*/

    new_id = isd_get_first_changed_element (trans_id);
    while (new_id != iid_null_id)
	{
	if (!ipb_read_element_header (elem_name, &old_id, &chg_type)){
             set_status (inconsistent_jour_file);
             return;
	}
	check_status ("ipb_map_trans: from ipb_read_element_header");
	if (chg_type == isd_create)
	    ipb_map_new_id (new_id, old_id);
	ipb_process_components (NULL, NULL, elem_name, FALSE);
	check_status ("ipb_map_trans: from ipb_process_components");
	new_id = isd_get_next_changed_element (trans_id);
	}
    more_element = ipb_read_element_header (elem_name, &old_id, &chg_type);
    check_status ("ipb_map_trans: from read_element");
/*
   we have run out of elements on the new list, if we haven't run out
   of elements on the old list there is an error
*/
    if (more_element)
	set_status (inconsistent_jour_file);

    DEBUG2 (fprintf (stderr, "ipb_map_transaction: return\n"));
    return;
   }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_skip_transaction
|
|  Description:
|	This routine skips the information associated with the current
|	transaction on the journaling file.  When it does this, it ignores all
|	information on the journaling file
\*--------------------------------------------------------------------------*/

void	ipb_skip_transaction ()

    {
    isd_change_type	chg_type;
    char		elem_name [32];
    iid_id_type		old_id;


    DEBUG2 (fprintf (stderr, "ipb_skip_transaction: call\n"));

    set_status (ok);

/*
   elements are read in from the journaling file and all information is
   ignored
*/
   while (ipb_read_element_header (elem_name, &old_id, &chg_type))
	{
	check_status ("ipb_skip_trans: from ipb_read_element_header");
/*
    the length of the name has either been read prior to entering the loop
    or at the end of the loop, in any case, we can assume it is a valid
    length
*/
	ipb_process_components (NULL, NULL, elem_name, FALSE);
	check_status ("ipb_skip_trans: from ipb_process_components");
	}
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_end_playback
|
|  Description:
|	This routine terminates the playback.  It closes the file and cleans
|	up the hash table
\*--------------------------------------------------------------------------*/

void	ipb_end_playback ()

    {
    set_status (ok);
    if (fclose (fp) == EOF)
	set_status (io_failure);
    ipb_cleanup_map ();
    ipb_free_mbx ();
    return;
    }
