static char rcsid [] =
	"$Header: /u/04c/mnt/integ/utilities/src/RCS/ipb2.c,v 1.8 89/06/15 20:17:11 ljb Exp $";

/*--------------------------------------------------------------------------*\
| 
|  Name:  Playback formatting (ipb2)
|
|  Description:  
|     The playback formatting reads and produces formatted output for
|     the journalling file
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     ipb2.c - contains the implementation for the playback formater
|     ipb.h - contains the external definition for the playback abstr.
|     ipb2test.c - test file.
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
$Log:	ipb2.c,v $
 * Revision 1.8  89/06/15  20:17:11  ljb
 * fix trailing null problem for component_name
 * 
 * Revision 1.7  89/06/06  15:04:33  little
 * add boolean type to format_component (ljb)
 * 
 * Revision 1.6  89/06/05  09:02:55  ljb
 * make work with "remove" and fix minor bugs
 * 
 * Revision 1.5  89/06/01  15:37:05  ejh
 * fixed buffer record/playback; turned status tests into io_test
 * 
 * Revision 1.4  89/02/10  14:48:51  ejh
 * added copyright notice
 * 
 * Revision 1.3  88/10/14  16:08:07  little
 * fix to work with dm sd
 * 
|     April 28, 1988 : Module created.
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

#include	"global.h"
#include	"string.h"
#include	"isc.h"
#include	"isd.h"
#include	"iid.h"
#include	"idd.h"
#include	"ipb.h"

#define DEBUG1(statement)  SERPENT_DEBUG1(ipb_debug, statement)
#define DEBUG2(statement)  SERPENT_DEBUG2(ipb_debug, statement)
#define DEBUG3(statement)  SERPENT_DEBUG3(ipb_debug, statement)

int	ipb_debug;
FILE	*fp;
HASH	mbx_table;
typedef	struct {
    string		mbx_name;
    idd_data_definition	data_definition;
} mbx_table_type;

/*--------------------------------------------------------------------------*\
|  Routine: ipb_mbx_hash
|
|  Description:
|	This routine calculates the hash function for a mailbox
\*--------------------------------------------------------------------------*/

int	ipb_mbx_hash (mbx_name)
string	mbx_name;

    {
    int		hash_value = 0;
    string	pointer;

    for (pointer = mbx_name; *pointer != '\0'; pointer ++)
	hash_value += *pointer;
    return ((((int) hash_value) & 077777) % HASH_TABLE_SIZE);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_mbx_cmp
|
|  Description:
|	This routine compares test value to see if have found actual value
\*--------------------------------------------------------------------------*/

boolean	ipb_mbx_cmp (test_mbx, mbx_table_struct)
string		test_mbx;
mbx_table_type	*mbx_table_struct;

    {
    return (!strcmp (test_mbx, mbx_table_struct->mbx_name));
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_init_mbx
|
|  Description:
|	This routine initializes the hash table for mailbox vs data def
\*--------------------------------------------------------------------------*/

void	ipb_init_mbx ()

    {
    mbx_table = make_hashtable (HASH_TABLE_SIZE, ipb_mbx_hash, ipb_mbx_cmp);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_cleanup_mbx
|
|  Description:
|	This routine cleans up the hash table for mailboxes
\*--------------------------------------------------------------------------*/


void	ipb_cleanup_mbx ()

    {
    free_hashtable (mbx_table, AND_NODES);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_get_data_def
|
|  Description:
|	This routine returns the data definition when passed a mbx
\*--------------------------------------------------------------------------*/

idd_data_definition	ipb_get_data_def (mbx)
string	mbx;

    {
    mbx_table_type	*mbx_elem;
/*
   use the get_from_hash routine to retrieve and then pull out
   the data defintion to return
*/
    mbx_elem = (mbx_table_type *) get_from_hashtable (mbx_table, mbx);
    return (mbx_elem->data_definition);
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_add_new_mbx
|
|  Description:
|	This routine adda a new mbx, data def pair to hash table
\*--------------------------------------------------------------------------*/

void	ipb_add_new_mbx (mbx, data_def)
string			mbx;
idd_data_definition	data_def;

    {
    mbx_table_type	*temp;
    NODE		ident;
/*  
   this routine creates a node with mailbox name, data definition in it
*/
    temp = (mbx_table_type *) make_node (sizeof (mbx_table_type));
    temp->data_definition = data_def;
    temp->mbx_name = strcpy (make_node (strlen (mbx) + 1), mbx);
    ident = add_to_hashtable (mbx_table, temp, mbx);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_format_component
|
|  Description:
|	The format element routine causes the component dependent portion of
|	the journaling file to be output to stdout
|
\*--------------------------------------------------------------------------*/

void	ipb_format_component (data_def, element_name)
idd_data_definition	data_def;
string			element_name;

    {
    int			len = -1,
			offset,
			position,
			int_temp;
    char		component_name [100],
			*string_temp;
    double		real_temp;
    idd_data_types	data_type;
    iid_id_type		id_temp;
    idd_buffer_type	*buffer_temp;
    string		buffer_info;

    while (len)
	{
	(void) fread (&len, sizeof (len), 1, fp);
	if (len)
	    {
	    (void) fread (component_name, len, 1, fp);
            component_name[len] = '\0';
	    fprintf (stdout, "      %s", component_name);
	    idd_get_component_lop (data_def, element_name, component_name,
		&len, &offset, &position);
	    data_type = idd_get_shared_data_type (data_def, element_name,
		component_name);

	    if (data_type == idd_integer)
		{
		(void) fread (&int_temp, len, 1, fp);
		fprintf (stdout, "   %d\n", int_temp);
		}
	    else if (data_type == idd_real)
		{
		(void) fread (&real_temp, len, 1, fp);
		fprintf (stdout, "   %f\n", real_temp);
		}
	    else if (data_type == idd_boolean)
		{
		(void) fread (&int_temp, len, 1, fp);
		fprintf (stdout, "   %d\n", int_temp);
		}
	    else if (data_type == idd_string)
		{
                string_temp = make_node(len+1);
		(void) fread (string_temp, len, 1, fp);
		fprintf (stdout, "   %s\n", string_temp);
                free_node(string_temp);
		}
	    else if (data_type == idd_id)
		{
		(void) fread (&id_temp, len, 1, fp);
		fprintf (stdout, "   %d\n", id_temp);
		}
	    else if (data_type == idd_buffer)
		{
		buffer_temp = (idd_buffer_type *) make_node (len);
		buffer_info = ((string) buffer_temp) +
		    sizeof (idd_buffer_type);
		(void) fread (buffer_temp, len, 1, fp);
		fprintf (stdout, "BUFFER, length = %d\n",buffer_temp->length);
		fprintf (stdout,
		    "\t\t0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",
		    (*buffer_info), (*(buffer_info + 1)),
		    (*(buffer_info + 2)), (*(buffer_info + 3)),
		    (*(buffer_info + 4)), (*(buffer_info + 5)),
		    (*(buffer_info + 6)), (*(buffer_info + 7)),
		    (*(buffer_info + 8)), (*(buffer_info + 9)));
		free (buffer_temp);
		}
	    }
	}
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_format_element
|
|  Description:
|	This routine causes the element dependent portion of the journaling
|	file to be output to stdout
|
\*--------------------------------------------------------------------------*/

void	ipb_format_element (data_def)
idd_data_definition	data_def;

    {
    int			len = -1;
    char		element_name [100];
    isd_change_type	change_type;
    iid_id_type		id;

    while (len)
	{
	(void) fread (&len, sizeof (len), 1, fp);
	if (len)
	    {
	    (void) fread (element_name, len, 1, fp);
	    fprintf (stdout, "   element name: %s", element_name);
	    (void) fread (&change_type, sizeof (change_type), 1, fp);
	    if (change_type == isd_create)
		fprintf (stdout, "  change_type: create");
	    else if (change_type == isd_modify)
		fprintf (stdout, "  change_type: modify");
	    else if (change_type == isd_remove)
		fprintf (stdout, "  change_type: remove");
	    else if (change_type == isd_get)
		fprintf (stdout, "  change_type: get   ");
	    (void) fread (&id, sizeof (id), 1, fp);
	    fprintf (stdout, "  id: %d\n", id);

	    if (change_type != isd_remove) ipb_format_component (data_def, element_name);
	    }
	}
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_format_transaction
|
|  Description:
|	This routine causes the transaction dependent portion of the
|	journaling file to be output to stdout
|
\*--------------------------------------------------------------------------*/

void	ipb_format_transaction ()

    {
    struct	timeval	tod;
    string		char_time;
    int			len;
    char		sender [100],
			receiver [100];
    mbx_table_type	*hash_entry;

    fprintf (stdout, "TRANSACTION:\n");
    (void) fread (&tod, sizeof (tod), 1, fp);
    char_time = ctime (&tod.tv_sec);
/*
   ctime returns a 26 char string of the form dow mmm dd hh:mm:ss yyyy\n\0;
   we insert a \0 between the ss and the yyyy.  This allows printing the
   microseconds before the year
*/
    char_time [19] = '\0';
    fprintf (stdout, "  %s:%d  %s", char_time, tod.tv_usec/1000,
	&char_time [20]);

    (void) fread (&len, sizeof (len), 1, fp);
    (void) fread (sender, len, 1, fp);
    sender [len] = '\0';

    fprintf (stdout, "   sender: %s\n", sender);

    (void) fread (&len, sizeof (len), 1, fp);
    (void) fread (receiver, len, 1, fp);
    receiver [len] = '\0';

    fprintf (stdout, "   receiver: %s\n", receiver);
/*
   locate data definition from mailbox name, only one of sender receiver
   should be in the hash table
*/
    hash_entry = (mbx_table_type *) get_from_hashtable (mbx_table, sender);
    if (!(hash_entry) || (!strcmp (hash_entry->mbx_name, "DM_BOX")))
	hash_entry = (mbx_table_type *) get_from_hashtable (mbx_table,
	    receiver);

    ipb_format_element (hash_entry->data_definition);
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_format_participant
|
|  Description:
|	This routine causes a new participant information to be written to
|	stdout, the ill file to be read in and the data_definition information
|	to be added to the participant hash table
|
\*--------------------------------------------------------------------------*/

void	ipb_format_participant	(message)
string	message;

    {
    int			len;
    char		ill_file_name [100],
			mbx_name [100];
    idd_data_definition	data_def;

    fprintf (stdout, "%s\n", message);

    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (ill_file_name, len, 1, fp));
    ill_file_name [len] = '\0';
    fprintf (stdout, "   ill file name: %s\n", ill_file_name);

    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (mbx_name, len, 1, fp));

    mbx_name [len] = '\0';
    fprintf (stdout, "   mailbox name: %s\n\n", mbx_name);

    if (strcmp (ill_file_name, ""))
	{
	data_def = idd_create_data_definition();
	isd_input_ill_file (data_def, ill_file_name);
	ipb_add_new_mbx (mbx_name, data_def);
	}
    return;
    }

/*--------------------------------------------------------------------------*\
|  Routine: ipb_format_playback
|
|  Description:
|	This routine causes the playback file recorded earlier to be output to
|	stdout
|
\*--------------------------------------------------------------------------*/

void	ipb_format_playback (file_name)
string	file_name;

    {
    char	dial_name [100],
		message [100],
		check_string [9];
    int		len;

    DEBUG2 (fprintf (stderr, "ipb_format_playback: call.\n"));
    DEBUG3 (fprintf (stderr, "ipb_format_playback: file name = %s.\n",
	file_name));

    set_status (ok);
/*
   Create/open file for playback.
*/
    fp = fopen (file_name, "r");
    check_null (fp, "ipb_format_playback: error opening specified file.\n",
	system_operation_failed);

    (void) fread (check_string, 8, 1, fp);
    check_string [8] = '\0';
    if (strcmp (check_string, "JOURNAL:"))
	{
	set_status (invalid_journal_file);
	fclose (fp);
	return;
	}
    fprintf (stdout, "FORMATTING JOURNAL FILE: %s\n\n", file_name);
    fprintf (stdout, "HEADER: \n");
/*
  read in dialogue name, message and ill file name
*/
    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (dial_name, len, 1, fp));
    dial_name [len] = '\0';
    fprintf (stdout, "   dialogue name: %s\n", dial_name);

    io_test (fread (&len, sizeof (len), 1, fp));
    io_test (fread (message, len, 1, fp));
    message [len] = '\0';
    fprintf (stdout, "   message: %s\n\n", message);

    ipb_init_mbx ();
    ipb_format_participant ("OWNER:");

    while (fread (check_string, 6, 1, fp))
	{
        check_string [6] = '\0';
	if (!strcmp (check_string, "PARTN:"))
	    ipb_format_participant ("PARTICIPANT:");
	else if (!strcmp (check_string, "TRANS:"))
	    ipb_format_transaction ();
	}

    DEBUG2 (fprintf (stderr, "ipb_format_playback: exit.\n"));

    return;
    }
