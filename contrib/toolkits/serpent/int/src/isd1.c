static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/isd1.c,v 1.19 89/10/13 10:51:00 rcs Exp $";

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
|     Apr. 13, 1988 : Module modified to have separate 
|                     init_process_record procedure and call journaling  ljb
|     Sep. 26, 1988 : Fixed bug in free_element_change_record when deleting
|		      buffers of zero length. - rCs
|     $Log:	isd1.c,v $
 * Revision 1.19  89/10/13  10:51:00  rcs
 * fixed memory leak
 * 
 * Revision 1.18  89/09/29  16:38:56  ljb
 * make new version of add_shared_data which takes id as input
 * 
 * Revision 1.17  89/09/29  14:48:22  rcs
 * added support for typed buffers
 * 
 * Revision 1.16  89/09/20  16:38:35  ljb
 * record transaction prior to notifying other process of it
 * this is because other process was changing hash table which
 * recording was recording
 * 
 * Revision 1.15  89/02/26  15:10:55  little
 * change SERPENT_ILL_PATH to be SERPENT_DATA_PATH
 * 
 * Revision 1.14  89/02/16  23:47:51  ejh
 * empty transactions are no longer recorded
 * 
 * Revision 1.13  89/02/15  14:48:56  rcs
 * modified delete buffer to not free memory for null body pointers,
 * as opposed to buffers of length zero
 * 
 * Revision 1.12  89/02/09  19:07:42  ejh
 * added copyright notice
 * 
 * Revision 1.11  89/01/26  16:31:05  ejh
 * added code to handle SERPENT_ILL_PATH to search for ill files
 * 
 * Revision 1.10  88/12/08  15:11:27  rcs
 * fixed problem with everything being undefined
 * 
 * Revision 1.9  88/12/06  17:48:54  rcs
 * fixed bug setting strings undefined
 * 
 * Revision 1.8  88/12/06  15:43:43  rcs
 * added isd_init_sd_element routine
 * 
 * Revision 1.7  88/11/02  16:50:54  rcs
 * *** empty log message ***
 * 
 * Revision 1.6  88/11/02  14:45:53  rcs
 * moved debug variable to interface file - rCs
 * 
 * Revision 1.5  88/10/13  15:06:29  rcs
 * added 1 to number of components for call to ubs_create_bitstring
 * 
 * 
 * Revision 1.4  88/10/03  08:09:11  little
 * add isd_dm_init_sdd routine and change isd_dm_init to only initialize
 * the interface and not create a process structure for the dm. now
 * the isd_dm_int_sdd creates the process structure and, if a dm sdd is
 * present, adds the sdd to the process structure
 * 
 * Revision 1.3  88/09/29  14:47:01  rcs
 * Sep. 29, 1988 : Fixed isd_put_shared data to handle 
 *                 buffers and scalars.
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

#define isd_main
#include "serpent.h"
#include "isd.h"                /* shared data abstraction                  */
#include "isd_priv.h"           /* shared data abstraction private types    */
#include "ufa.h"		/* file access utilities abstraction        */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(isd_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(isd_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(isd_debug, statement)

#define PROCESS_RECORD_HASH_SIZE  17  

/*-------------------- Retained Data ---------------------------------------*/

process_record in_box;		/* attached process mailbox & data def	    */
isd_process out_box;		/* dm process mailbox & attached	    */

HASH isd_process_table;		/* list of process records indexed by mbx   */
typedef struct {
  ipc_mailbox hash_mbx_number;
  process_record *hash_process_record;
} process_table_entry;

/*--------------------------------------------------------------------------*\
|  Routine: isd_addto_process_table
| 
|  Description:  
|     The add to process table routine adds a new entry to the process
|     table
\*-------------------------------------------------------------------------*/ 
void isd_addto_process_table (process_box)
process_record *process_box;
{
process_table_entry *temp;

   temp = (process_table_entry *)make_node(sizeof(process_table_entry));
   temp->hash_mbx_number = process_box->mailbox;
   temp->hash_process_record = process_box;
   add_to_hashtable (isd_process_table, temp, process_box->mailbox);
   return;
}

/*--------------------------------------------------------------------------*\
|  Routine: isd_getfrom_process_table
| 
|  Description:  
|     The get from process table routine retrieves an entry to the process
|     table or NULL
\*-------------------------------------------------------------------------*/ 
isd_process isd_getfrom_process_table (mbx)
ipc_mailbox mbx;
{
process_table_entry *process_entry;

   process_entry = (process_table_entry *)get_from_hashtable( 
           isd_process_table,
           mbx);
   if (process_entry == NULL) return NULL;
   return (isd_process)(process_entry->hash_process_record);
}

/*--------------------------------------------------------------------------*\
|  Routine: isd_deletefrom_process_table
| 
|  Description:  
|     The delete from process table routine deletes an entry from the process
|     table
\*-------------------------------------------------------------------------*/ 
void isd_deletefrom_process_table (process)
process_record *process;
{
   delete_from_hashtable (isd_process_table, process->mailbox);
   return;
}

/*--------------------------------------------------------------------------*\
|  Routine: isd_cleanup_process_table
| 
|  Description:  
|     The clean up process table routine deletes the process table
\*-------------------------------------------------------------------------*/ 
void isd_cleanup_process_table ()
{
   free_hashtable (isd_process_table, AND_NODES);
   return;
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_process_cmp
| 
|  Description:  
|     The init process compare routine returns true or false depending upon
|     whether the entry matches the comparision value
\*-------------------------------------------------------------------------*/ 
boolean isd_process_cmp (test_mailbox, process_entry)
ipc_mailbox test_mailbox;
process_table_entry *process_entry;
{
   return test_mailbox == process_entry->hash_mbx_number;
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_process_hash
| 
|  Description:  
|     The process hash routine returns the hash number for the specified arg
\*-------------------------------------------------------------------------*/ 
int isd_process_hash(mbx_number)
ipc_mailbox mbx_number;
{
   return ((((int) mbx_number) & 077777) % PROCESS_RECORD_HASH_SIZE);
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_init_process_table
| 
|  Description:  
|     The init process table routine initializes the hash table used
|     to maintain the mailbox number, process record pairs
\*-------------------------------------------------------------------------*/ 
void isd_init_process_table()
{
   isd_process_table = make_hashtable (PROCESS_RECORD_HASH_SIZE,
                                   isd_process_hash, isd_process_cmp);
   return;
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_init_once
| 
|  Description:  
|     The init once routine initializes all the stuff that only needs to be
|     initialized once.  This is called directly by the dialogue manager
|     and indirectly by the taps.
\*-------------------------------------------------------------------------*/ 
void isd_init_once()
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_init_once: call.\n");
  );
/*
   Initialize id counter.
*/
  iid_init();
  check_status("isd_init_once: status from idd_init.");
/*
   Initialize semaphore id.
*/
  ids_init();
  check_status("isd_init_once: status from ids_init.");
/*
   Initialize shared memory.
*/
  ism_init();
  check_status("isd_init_once: status from ism_init.");
/*
  initialize process table
*/
   isd_init_process_table();
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_init_once: exiting.\n");
  );
  return;
}                                       /* end isd_init_once		    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_init_process_record
| 
|  Description:  
|     The initialize process record routine fills in a structure which defines
|      all of the elements of a process record EXCEPT the pointer to the
|      data definition.  It is the responsibility of the calling routine
|      to fill in the data definition field
|     
|     This routine is called directly by from within the isd data abstraction
\*-------------------------------------------------------------------------*/
void isd_init_process_record(process, mailbox, ill_file)
process_record *process;
string mailbox;
string ill_file;
{
/*
   Validate parameters.
*/
  check_null(
    mailbox, 
    "isd_init_process_record: null parameter 'mailbox'.\n",
    null_mailbox_name
  );

  check_null(
    ill_file, 
    "isd_init_process_record: null parameter 'ill_file'.\n",
    null_ill_file_name
    );
/*
   set validation stamp.
*/
  process->validation_stamp = isc_process_record_vs;
/*
   Allocate storage for ill file and mailbox names and copy into process
   record.
*/
  process->ill_file_name = make_node(strlen(ill_file)+1);
  check_null(
    process->ill_file_name, 
    "isd_init_process_record: out_of_memory", 
    out_of_memory
  );
  strcpy(process->ill_file_name, ill_file);

  process->mailbox_name = make_node(strlen(mailbox)+1);
  check_null(
    process->mailbox_name, 
    "isd_init_process_record: out_of_memory", 
    out_of_memory
  );
  strcpy(process->mailbox_name, mailbox);
/*
   Initialize record_fp and recording fields.
*/
  process->record_fp = NULL;
  process->recording = false;
/*
   Create the mailbox.
*/
  process->mailbox = ipc_init(mailbox);
  check_status("isd_init_process_record: status from ipc_init.");
/*
  add to process table
*/
  isd_addto_process_table(process);
  return;
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_input_open_ill_file
| 
|  Description:  
|     The input open ill file routine takes as input an empty data definition
|     and a fill open to an ill fill, reads the ill file in and creates the
|     data definition from it.
\*-------------------------------------------------------------------------*/ 
void isd_input_open_ill_file(data_definition, fp)
idd_data_definition data_definition;
FILE *fp;
{
idd_element element;
char element_name[50], component_name[50];
int offset, length;
idd_data_types type;
/*
   While file not empty create the appropriate data definitions.
*/
  fscanf (fp, "%s" , element_name);
  while (strcmp (element_name, ">>") != 0){
    fscanf(fp, "%s%d%d%d", component_name, &offset ,&length, &type);
    DEBUG3(
      fprintf(
        stderr, 
        "%s %s %d %d %d\n", element_name, component_name, offset, length, type
      );
    );

    if (strcmp(element_name, component_name) == 0) {	/* element	    */
      element = idd_create_element(
        data_definition, 
        element_name,
        type,
        length
      );						/* end if element   */
      check_status(
        "isd_open_input_file: status from idd_create_element."
      );
    }
    else {			/* component				    */
      idd_create_component(
        element,
        component_name,
        type,
        offset,
        length
      );
      check_status(
        "isd_create_process_def: status from idd_create_component.");
    }				/* end else component			    */
    fscanf (fp, "%s" , element_name);
  }				/* end while file not empty		    */
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_input_ill_file
| 
|  Description:  
|     The input ill file routine takes as input an empty data definition
|     and an ill file name, reads the ill file in and creates the
|     data definition from it.
\*-------------------------------------------------------------------------*/ 
void isd_input_ill_file(data_definition, ill_file)
idd_data_definition data_definition;
string ill_file;
{
FILE *fp;
char check_string[255];
string	full_file_name,
	env_value,
	getenv ();
boolean	found_env = TRUE;

   DEBUG2 (fprintf (stderr, "entering isd_input_ill_file:\n");
	fprintf (stderr, "\tdata_definition = %x, ill_file = %s\n",
		 data_definition, ill_file););
/*
   Open the ill file.
*/

  env_value = strcpy (make_node (strlen (getenv ("SERPENT_DATA_PATH")) + 2),
	getenv ("SERPENT_DATA_PATH"));
  if (!strlen (env_value))
	{
	found_env = FALSE;
	env_value [0] = '.';
	env_value [1] = '\0';
	}
  full_file_name = ufa_full_file_name (env_value, ill_file);
/*
  check_null (full_file_name,
	("no %s in SERPENT_DATA_PATH or current directory", ill_file),
	null_ill_file_name);
*/
  if (!full_file_name)
	{
	DEBUG1 (if (found_env)
		    fprintf (stderr, "no %s in SERPENT_DATA_PATH\n", ill_file);
		else
		    fprintf (stderr, "no %s in current directory\n",
			ill_file));
	set_status (null_ill_file_name);
	return;
	}
  fp = fopen (full_file_name, "r");
  free_node (full_file_name);
  free_node (env_value);
  DEBUG3(
    fprintf(stderr, "isd_input_ill_file: fp = %x.\n", fp);
  );
  check_null(
    fp, 
    "isd_input_ill_file: could not open ill file.\n",
    null_ill_file_name);
/*
  skip to correct position on ill file
*/
  fscanf(fp, "%s", check_string);
  while ( strcmp(check_string,"|||") != 0)   fscanf(fp, "%s", check_string);

  isd_input_open_ill_file(data_definition, fp);

/*
   Close the ill file.
*/
  fclose(fp);
  return;
}
/*--------------------------------------------------------------------------*\
|  Routine: isd_create_process_definition
| 
|  Description:  
|     The create process definition routine creates a structure which defines
|      both the mailbox id and the shared data definition for a process. 
|     
|     This routine is called directly by the dialogue manager and indirectly
|     by the application.
\*-------------------------------------------------------------------------*/ 
isd_process isd_create_process_definition(mailbox, ill_file)
string mailbox;
string ill_file;
{                                       /* local variable definitions       */
process_record *process;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_create_process_definition: call.\n");
    fprintf(stderr, "mailbox = %s.\n", mailbox);
    fprintf(stderr, "ill file = %s.\n", ill_file);
  );
/*
   Validate parameters.
*/
  rcheck_null(
    mailbox, 
    "isd_create_process_definition: null parameter 'mailbox'.\n",
    null_mailbox_name,
    NULL
  );

  rcheck_null(
    ill_file, 
    "isd_create_process_definition: null parameter 'ill_file'.\n",
    null_ill_file_name,
    NULL
  );
/*
   Create a process record and set validation stamp.
*/
  process = (process_record *)make_node(sizeof(process_record));
  rcheck_null(
    process, 
    "isd_create_process_definition: out_of_memory", 
    out_of_memory,
    NULL
  );
/*
   Fill in the process record
*/
  isd_init_process_record(process, mailbox, ill_file);
  rcheck_status(
    "isd_create_process_definition: status from isd_init_process_record.",
    NULL
  );
/*
   Create the data definition.
*/
  process->data_definition = idd_create_data_definition();
  rcheck_status(
    "isd_create_process_definition: status from idd_create_data_def.",
     NULL
  );

  isd_input_ill_file(process->data_definition, ill_file);
  rcheck_status("isd_create_process_definition: status from isd_input_ill_file",
  NULL
  );

/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "process = %x.\n", process);
    fprintf(stderr, "isd_create_process_definition: exiting.\n"););
  return (isd_process)process;
}                                       /* end isd_create_process_def       */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_mailbox
| 
|  Description:  
|     The get mailbox routine returns the mailbox associated with a given
|     process.
\*-------------------------------------------------------------------------*/ 
ipc_mailbox isd_get_mailbox(process)
isd_process process;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_mailbox: call.\n");
    fprintf(stderr, "isd_get_mailbox: process = %x.\n", process);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(process, isc_process_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_process_handle);
    DEBUG1(
      isc_print_status("isd_get_mailbox:");
    );
    return NULL;
  }					/* end if invalid transaction hand  */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_mailbox: exiting.\n");
    fprintf(stderr, 
      "isd_get_mailbox: mailbox = %x.\n",
      ((process_record *)process)->mailbox
    );
  );
  return ((process_record *)process)->mailbox;
}                                       /* end isd_get_mailbox		    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_get_data_definition
| 
|  Description:  
|     The get data definition routine returns the data definition associated
|     with a given process.
\*-------------------------------------------------------------------------*/ 
idd_data_definition isd_get_data_definition(process)
isd_process process;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_get_data_definition: call.\n");
    fprintf(stderr, "process = %x.\n", process);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(process, isc_process_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_process_handle);
    DEBUG1(
      isc_print_status("isd_get_data_definition:");
    );
    return NULL;
  }					/* end if invalid transaction hand  */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_get_data_definition: exiting.\n");
    fprintf(stderr, 
      "isd_get_data_definition: data_definition = %x.\n",
      ((process_record *)process)->data_definition
    );
  );
  return ((process_record *)process)->data_definition;
}                                       /* end isd_get_data_definition	    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_destroy_process_definition
| 
|  Description:  
|     The destroy process definition frees all storage associated 
|     with a process definition record.
\*-------------------------------------------------------------------------*/ 
void isd_destroy_process_definition(process)
isd_process process;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_destroy_process_def: call.\n");
    fprintf(stderr, "isd_destroy_process_def: process = %x.\n", process);
  );
/*
   Validate parameter.
*/
  if (!isc_validate(process, isc_process_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_process_handle);
    DEBUG1(
      isc_print_status("isd_destroy_process_definition:");
    );
    return;
  }					/* end if invalid transaction hand  */

   isd_deletefrom_process_table ((process_record *) process);
/*
   Destroy the data definition.
*/
  idd_destroy_data_definition(((process_record *)process)->data_definition);
  check_status("isd_destroy_proc_def: status from idd_destry_data_def.");
/*
   Destroy the process record.
*/
  free_node(process);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_destroy_process_definition: exiting.\n");
  );
  return;
}                                       /* end isd_destroy_process_def      */
 
/*--------------------------------------------------------------------------*\
|  Routine: isd_init
| 
|  Description:  
|     The init routine performs necessary initialization of the interface
|     layer.  
\*-------------------------------------------------------------------------*/ 
void isd_init(mailbox, ill_file)
string mailbox;
string ill_file;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_init: call.\n");
    fprintf(stderr, "mailbox = %s.\n", mailbox);
    fprintf(stderr, "ill_file = %s.\n", ill_file);
  );
/*
   Validate parameters.
*/
  check_null(
    mailbox, 
    "isd_create_process_definition: null parameter 'mailbox'.\n",
    null_mailbox_name
  );

  check_null(
    ill_file, 
    "isd_create_process_definition: null parameter 'ill_file'.\n",
    null_ill_file_name
  );
/*
   Peform "one time" initialization code.
*/
  isd_init_once();
  check_status("ids_init: status from isd_init.");
/*
   Define an "out_box" for the tap which contains the mailbox of the 
   dialogue manager and the shared data definition for the tap.
*/
  out_box = isd_create_process_definition("DM_BOX", ill_file);

  DEBUG3(
    fprintf(stderr, 
      "isd_init: out_box.data_definition = %x.\n", 
      ((process_record *)out_box)->data_definition
    );

    idd_print_data_definition(((process_record *)out_box)->data_definition);

  );

  check_status("ids_init: status from isd_create_process_definition.");
/*
   Fill in the process record for in_box
*/
  isd_init_process_record (&in_box, mailbox, ill_file);
  check_status( "isd_init: status from isd_init_process_record.");

  in_box.data_definition = ((process_record *)out_box)->data_definition;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_init: exiting.\n");
  );
  return;
}                                       /* end isd_init			    */

 
/*--------------------------------------------------------------------------*\
|  Routine: isd_dm_init
| 
|  Description:  
|     The init routine performs necessary initialization of the interface
|     layer for the dialogue manager.
\*-------------------------------------------------------------------------*/ 
void isd_dm_init ()
{
				/* Initialization.  */
   set_status(ok);                       /* begin                            */
   DEBUG2 (fprintf(stderr, "isd_dm_init: call\n"););

				/* Peform "one time" initialization code.  */
   isd_init_once ();
   check_status ("isd_dm_init: status from isd_init_once.");
   DEBUG2 (fprintf(stderr, "isd_dm_init: exit\n"););
   return;
}


/*--------------------------------------------------------------------------*\
|  Routine: isd_dm_init_sdd
| 
|  Description:
|     This routine performs necessary initialization of the shared data
|       data description structure from an ill for the dialogue manager.
\*-------------------------------------------------------------------------*/ 
void isd_dm_init_sdd (ill_file)
string ill_file;        
{
				/* Initialization.  */
  set_status(ok);                       /* begin                            */
  DEBUG2 (fprintf(stderr, "isd_dm_init_sdd: call\n");
	fprintf (stderr, "\till_file = %s\n", ill_file););

   if (ill_file)		/* there was a dm ill file present, define the
				   in_box and the shared data definition for
				   the dialogue manager.  */
      {isd_init_process_record (&in_box, "DM_BOX", ill_file);
      check_status (
	"isd_dm_init_sdd: bad status from isd_init_process_record w/ill file");
				/* Create the data definition.  */
      in_box.data_definition = idd_create_data_definition ();
      check_status ("isd_dm_init_sdd: bad status from idd_create_data_def.");
				/* read the ill file and place in internal
				   structure */
      isd_input_ill_file (in_box.data_definition, ill_file);
      check_status ("isd_dm_init_sdd: bad status from isd_input_ill_file");
      }
    else			/* there is no a dm ill file, define the in_box
				   and a dummy shared data definition for the
				   dialogue manager.  */
      {isd_init_process_record (&in_box, "DM_BOX", "");
      check_status (
	"isd_dm_init_sdd: status from isd_init_process_record w/o ill file");
      in_box.data_definition = NULL;
      }

  DEBUG2 (fprintf(stderr, "isd_dm_init_sdd: exiting.\n"););

  return;
}                                       /* end isd_dm_init_sdd		    */


/*--------------------------------------------------------------------------*\
|  Routine: match_id
| 
|  Description:  
|     Routine to compare the identifiers with user elements.
|   
\*--------------------------------------------------------------------------*/
int match_id(id, element_change)
iid_id_type id;
element_change_record *element_change;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "match_id: call.\n");
    fprintf(stderr, "match_id: identifer = %d.\n", id);
    fprintf(stderr, "match_id: identifer = %x.\n", element_change);
  );

  DEBUG2(
    fprintf(stderr, "match_id:  exiting.\n");
  );
  return (element_change->id == id);
} 					/* end match_id			    */

/*--------------------------------------------------------------------------*\
|  Routine: hash_id
| 
|  Description:  
|     Internal function which will convert an id into indices
|     in the array of hash lists.
|   
\*--------------------------------------------------------------------------*/
int hash_id(id)
iid_id_type id;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "hash_id call.\n");
  );
/*
   Return a value in the right range.
*/
  DEBUG2(
    fprintf(stderr, "hash_id exiting.\n");
  );
  return((int)id % MAX_HASH);
} 					/* end hash_id			    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_start_dm_transaction
| 
|  Description:  
|     The start dm transaction routine is used to define the start of a series
|     of data modifications that must be performed as a group.
\*--------------------------------------------------------------------------*/
isd_trans isd_start_dm_transaction(receiver)
isd_process receiver;
{                                       /* local variable definitions       */
transaction_record *transaction;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_start_dm_transaction: call.\n");
  );
/*
   Validate parameter.
*/
  if (!isc_validate(receiver, isc_process_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_process_handle);
    DEBUG1(
      isc_print_status("isd_start_dm_transaction:");
    );
    return NULL;
  }					/* end if invalid transaction hand  */
/*
   Create a transaction record in shared memory and set validation stamp.
*/
  transaction = (transaction_record *)make_node(
    sizeof(transaction_record)
  );
  rcheck_null(
    transaction,
    "isd_start_dm_transaction: out of memory.\n",
    out_of_memory,
    NULL
  );
  transaction->validation_stamp = isc_transaction_record_vs;
/*
   Set up intended receiver.  The receiver record contians both the mailbox
   id and the handle to the associated data definition.
*/
  transaction->receiver = (process_record *)receiver;
/*
   Set up change table.
*/

  DEBUG2(
    fprintf(
      stderr, 
      "isd_start_dm_transaction: about to make change table.\n"
    );
  );

  transaction->change_table = ush_make_shm_hashtable(
    MAX_HASH, 
    hash_id,
    match_id
  );
  rcheck_null(
    transaction, 
    "isd_start_dm_transaction:  ush_make_shm_hashtable failed.\n",
    out_of_memory,
    NULL
  );

  DEBUG2(
    fprintf(stderr, 
      "isd_start_dm_transaction:  change table = %x.\n", 
      transaction->change_table
    );
  );

/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_start_dm_transaction: exiting.\n");
    fprintf(
      stderr, 
      "isd_start_dm_transaction: transaction = %x.\n", 
      transaction
    );
  );
  return (isd_trans)transaction;
}                                       /* end isd_start_dm_transaction     */

/*--------------------------------------------------------------------------*\
|  Routine: isd_start_transaction
| 
|  Description:  
|     The start transaction routine is used to define the start of a series
|     of data modifications that must be performed as a group.
\*--------------------------------------------------------------------------*/
isd_trans isd_start_transaction()
{                                       /* local variable definitions       */
isd_trans transaction;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_start_transaction: call.\n");
  );
/*
   Call start transaction to create transaction record etc.
*/
  transaction = isd_start_dm_transaction(out_box);
  rcheck_status(
    "isd_start_trans:  status from isd_start_dm_trans.",
    NULL
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_start_transaction: exiting.\n");
  );
  return transaction;
}                                       /* end isd_start_transaction	    */

/*--------------------------------------------------------------------------*\
|  Routine: create_element_change_record
| 
|  Description:  
|     Internal routine to create an element change record set the element
|     name field.
\*--------------------------------------------------------------------------*/
element_change_record *create_element_change_record(transaction, element_name)
isd_trans transaction;
string element_name;
{                                       /* local variable definitions       */
element_change_record *element_change;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "create_element_change_record: call.\n");
    fprintf(
      stderr, 
      "create_element_change_record: transaction = %x.\n", 
      transaction
    );
    fprintf(stderr, 
      "create_element_change_record: element name = %s.\n", 
      element_name
    );
  );
/*
   Validate parameter.
*/
  if (!isc_validate(transaction, isc_transaction_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_create_element_change_record:");
    );
    return NULL;
  }					/* end if invalid transaction hand  */
/*
   Allocate storage for change record in shared memory.
*/
  element_change = (element_change_record *)ism_get_shared_memory(
    sizeof(element_change_record)
  );
  rcheck_status(
    "create_element_change_record: status from ism_get_shm.",
    NULL
  );
/*
   Allocate shared memory storage for element name and copy in.
*/
  element_change->element_name = ism_get_shared_memory(
    strlen(element_name)+1
  );

  strcpy(element_change->element_name, element_name);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, 
      "create_element_change_record: element change = %x.\n", 
      element_change
    );
    fprintf(stderr, "create_element_change_record: exiting.\n");
  );
  return element_change;
}                                       /* end create_element_change_record */

/*--------------------------------------------------------------------------*\
|  Routine: set_undefined
| 
|  Description:  
|     The set_undefined routine sets a shared data component or scalar
|     element to undefined.
\*--------------------------------------------------------------------------*/
void set_undefined(data_type, data, offset)
idd_data_types data_type;
caddr_t data;
int offset;
{                                       /* local variable definitions       */
  int undef;
  double undef_real;
/*
   Initialize.
*/
  DEBUG2(
    fprintf(stderr, "set_undefined: call.\n");
    fprintf(
      stderr, 
      "set_undefined: data type = %.\n", 
      data_type
    );
    fprintf(
      stderr, 
      "set_undefined: data = %x.\n", 
      data
    );
    fprintf(stderr, 
      "set_undefined: offset = %d.\n", 
      offset
    );
  );
/*
   Set scalar element or component to UNDEFINED.
*/
  switch (data_type) {			/* switch on data type		    */

    case idd_boolean:
      undef = UNDEFINED_BOOLEAN;
      ism_copy_into_shared_memory(
        data + offset, 
        sizeof(UNDEFINED_BOOLEAN), 
        &undef
      );
      check_status("set_undefined: status from ism_copy_into_shm.");
      break;

    case idd_integer:
      undef = UNDEFINED_INTEGER;
      ism_copy_into_shared_memory(
        data + offset, 
        sizeof(UNDEFINED_INTEGER), 
        &undef
      );
      check_status("set_undefined: status from ism_copy_into_shm.");
      break;

    case idd_real:
      undef_real = UNDEFINED_REAL;
      ism_copy_into_shared_memory(
        data + offset, 
        sizeof(UNDEFINED_REAL), 
        &undef_real
      );
      check_status("set_undefined: status from ism_copy_into_shm.");
      break;

    case idd_id:
      undef = UNDEFINED_ID;
      ism_copy_into_shared_memory(
        data + offset, 
        sizeof(UNDEFINED_ID), 
        &undef
      );
      check_status("set_undefined: status from ism_copy_into_shm.");
      break;

    case idd_string:
      ism_copy_into_shared_memory(
        data + offset, 
        sizeof(UNDEFINED_STRING), 
        UNDEFINED_STRING
      );
      check_status("set_undefined: status from ism_copy_into_shm.");
      break;

    case idd_buffer:
      undef = UNDEFINED_BUFFER_LENGTH;
      ism_copy_into_shared_memory(
        data + offset, 
        sizeof(UNDEFINED_BUFFER_LENGTH), 
        &undef
      );
      check_status("set_undefined: status from ism_copy_into_shm.");
      break;

    default:
      DEBUG1(
        fprintf(stderr, "set_undefined: unknown component type.\n");
      );
      set_status(invalid_data_type);
      return;
      
  }					/* end switch on data type	    */
/*
   Return.
*/
  return;
}					/* end set_undefined		    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_init_sd_element
| 
|  Description:  
|     The isd_init_sd_element routine is used to set an existing shared
|     data element to UNDEFINED.
\*--------------------------------------------------------------------------*/
void isd_init_sd_element(data_definition, element_name, data) 
idd_data_definition data_definition;
string element_name;
caddr_t data;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  int length, offset, position, components;
  LIST component_list;
  string component_i_name;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "isd_init_sd_elem: call.\n");
    fprintf(
      stderr, 
      "isd_init_sd_elem: data definition = %x.\n", 
      data_definition
    );
    fprintf(
      stderr, 
      "isd_init_sd_elem: element name = %s.\n", 
      element_name
    );
    fprintf(stderr, "isd_init_sd_elem: data = %x.\n", data);
  );
/*
   Get the number of components in the shared data element.
*/
  components = idd_get_number_of_components(
    data_definition,
    element_name
  );
  check_status("isd_init_sd_elem: status from idd_get_#_components.");
/*
   Initialize the element to undefined.
*/
/*
   If element is a scalar set the offset to 0, get the data type of the 
   element and call set_undefined to set the value to undefined.
*/
  if (components == -1) {		/* element is a scalar		    */

    offset = 0;

    data_type = idd_get_shared_data_type(
      data_definition,
      element_name,
      NULL
    );
    check_status("isd_init_sd_elem: status from idd_get_shrd_data_type.");

    set_undefined(data_type, data, offset);
    check_status("isd_init_sd_elem: status from set_undefined.");

  }					/* end if element is a scalar	    */
/*
   Else if element is a recoord loop through list of components and
   assign each to UNDEFINED.
*/
  else {				/* element is a record		    */

    component_list = idd_create_component_list(
      data_definition,
      element_name
    );
    check_status("isd_init_sd_elem: status from idd_create_comp_list.");

    loop_through_list(component_list, component_i_name, string) {
/*
   Get the length, offset and position of the component.
*/
      idd_get_component_lop(
        data_definition,
        element_name,
        component_i_name,
        &length,
        &offset,
        &position
      );
      check_status("isd_init_sd_elem: status from idd_get_lngthNoff.");
/*
   Get component data type.
*/
      data_type = idd_get_shared_data_type(
        data_definition,
        element_name,
        component_i_name
      );
      check_status("isd_init_sd_elem: status from idd_get_shrd_data_type.");

      set_undefined(data_type, data, offset);
      check_status("isd_init_sd_elem: status from set_undefined.");
            
    }					/* end loop_through_list	    */

    idd_destroy_component_list(component_list);
    check_status("isd_init_sd_elem: status from idd_destroy_comp_list.");
  }					/* end if component specified	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_init_sd_elem: exiting.\n");
  );
  return;
}                                       /* end isd_init_sd_elem 	    */

/*--------------------------------------------------------------------------*\
|  Routine: copy_component
| 
|  Description:  
|     The copy component routine examines a component to determine if it
|     is a buffer.  If so, shared memory space is allocated of buffer 
|     length and user data copied into the buffer.
\*--------------------------------------------------------------------------*/
void copy_component(data_definition, element_change, data, component_name)
idd_data_definition data_definition;
caddr_t data;
element_change_record *element_change;
string component_name;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  int length, offset, position;
  idd_buffer_type *user_buffer;
  idd_buffer_type *shm_buffer;

  DEBUG2(
    fprintf(stderr, "copy_component: call.\n");
    fprintf(
      stderr, 
      "copy_component: data definition = %x.\n", 
      data_definition
    );
    fprintf(
      stderr, 
      "copy_component: element change = %x.\n", 
      element_change
    );
    fprintf(
      stderr, 
      "copy_component: data = %x.\n", 
      data
    );
    fprintf(stderr, 
      "copy_component: component name = %s.\n", 
      component_name
    );
  );
/*
   Get the length, offset and position of the component.
*/
  idd_get_component_lop(
    data_definition,
    element_change->element_name,
    component_name,
    &length,
    &offset,
    &position
  );
  check_status("copy_component: status from idd_get_lngthNoff.");
/*
   Set appropriate change bit in changed component bitstring.
*/
  ubs_set_bit(element_change->changed_components, position);
  check_status("copy_component: status from ubs_set_bit.");
/*
   Get component data type.
*/
  data_type = idd_get_shared_data_type(
    data_definition,
    element_change->element_name,
    component_name
  );
  check_status("copy_component: status from idd_get_shared_data_type.");
/*
   If buffer handle special case.
*/
  if (data_type == idd_buffer) {	/* buffer type			    */

/*
   Free any existing buffer body regardless of if there is new data or not.
*/
    shm_buffer = (idd_buffer_type *)((char *)element_change->data + offset);

    if (shm_buffer->body != NULL) {	/* if existing buffer body	    */
      ism_free_shared_memory(shm_buffer->body);
      check_status("copy_component: status from ism_free_shared_memory.");
    }					/* end if existing buffer body	    */
/*
   Copy buffer into shared memory if buffer length is non zero.
*/
    user_buffer = (idd_buffer_type *)((char *)data + offset);

    if (user_buffer->length != 0) {    /* length is non-zero	    */

      DEBUG3(
        fprintf(
          stderr,
          "user_buffer = %x.\n",
          user_buffer
        );
        fprintf(
          stderr,
          "user_buffer->length = %x.\n",
          user_buffer->length
        );
        fprintf(
          stderr,
          "user_buffer->type = %s.\n",
          idd_get_type_string(user_buffer->type)
        );
        fprintf(
          stderr,
          "user_buffer->body = %x.\n",
          user_buffer->body
        );
      );

      shm_buffer->length = user_buffer->length;
      shm_buffer->type = user_buffer->type;

      shm_buffer->body = ism_get_shared_memory(user_buffer->length);
      check_status("copy_component: status from ism_get_shared_memory.");

      ism_copy_into_shared_memory(
        shm_buffer->body, 
        user_buffer->length, 
        user_buffer->body
      );
      check_status("copy_component: status from ism_copy_into_shm.");

      DEBUG3(
        fprintf(
          stderr,
          "copy_component: buffer length = %d.\n",
          shm_buffer->length
        );
        fprintf(
          stderr,
          "shm_buffer->type = %s.\n",
          idd_get_type_string(shm_buffer->type)
        );
        fprintf(
           stderr,
          "copy_component: buffer body = %s.\n",
          shm_buffer->body
        );
      );

    }					/* end if buffer length not zero    */

  }					/* end if buffer type		    */
  else {				/* not buffer type		    */
/*
   Copy user data into shared memory.
*/
    ism_copy_into_shared_memory(
      (element_change->data)+offset, 
      length, 
      data + offset
    );
    check_status("copy_component: status from ism_copy_into_shm.");
  }					/* end else not buffer type	    */
/*
   Return.
*/
  return;
}					/* end copy component		    */

/*--------------------------------------------------------------------------*\
|  Routine: add_data_to_ecr
| 
|  Description:  
|     The add data to ecr routine adds data to the element change record.
|     If existing_ecr is false an ecr and associated structures are created.
\*--------------------------------------------------------------------------*/
void add_data_to_ecr(transaction, element_change, component_name, 
                     existing_ecr, data)
isd_trans transaction;
element_change_record *element_change;
string component_name;
boolean existing_ecr;
caddr_t data;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  idd_buffer_type *user_buffer;
  idd_buffer_type *shm_buffer;
  int length, offset, position, components;
  LIST component_list;
  string component_i_name;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "add_data_to_ecr: call.\n");
    fprintf(stderr, "add_data_to_ecr: transaction = %x.\n", transaction);
    fprintf(
      stderr, 
      "add_data_to_ecr: element change = %x.\n", 
      element_change
    );
    fprintf(
      stderr, 
      "add_data_to_ecr: component name = %s.\n", 
      component_name
    );
    fprintf(stderr, "add_data_to_ecr: existing ecr = %d.\n", existing_ecr);
    fprintf(stderr, "add_data_to_ecr: data = %x.\n", data);
  );
/* 
   Determine size of shared memory required to maintain an instance of
   the shared data element and allocate the memory if ecr doesn't already
   exist.
*/
  length = idd_get_length(
    ((transaction_record *) transaction)->receiver->data_definition, 
    element_change->element_name
  );
  check_status("add_data_to_ecr: status from idd_get_length.");

  if (!existing_ecr) {		/* ecr does not exist			    */
    element_change->data = ism_get_shared_memory(length);
    check_status("add_data_to_ecr: status from ism_get_shm.");
  }				/* end if ecr does not exist		    */
/*
   Get the number of components in the shared data element.
*/
  components = idd_get_number_of_components(
    ((transaction_record *)transaction)->receiver->data_definition,
    element_change->element_name
  );
  check_status("add_data_to_ecr: status from idd_get_#_components.");
/*
   If the ecr doesn't already exists, create the changed component bit string
   and initialize the element change record to UNDEFINED if change type of
   create and the entire element is not specified.
*/
  if (!existing_ecr) {          /* ecr does not exist                       */

    element_change->changed_components = ubs_create_bitstring(components+1);

    if ((component_name != NULL) ||
		       ((component_name == NULL) && (data == NULL))) {	
      isd_init_sd_element(
        ((transaction_record *)transaction)->receiver->data_definition, 
        element_change->element_name, 
        element_change->data
      );
    }					/* end if component specified	    */
  }				/* end if create type			    */
/*
   If no data specified return without doing anything.
*/
  if (data == NULL) {			/* not initialized		    */
    return;
  }
/*
   If element specified copy element data into shared memory otherwise
   copy component data into shared memory.
*/
  if (component_name == NULL) {		/* element specified		    */
/*
   Set appropriate change bit in changed component bitstring.
*/
    ubs_set_bitstring(element_change->changed_components, components);
    check_status("add_data_to_ecr: status from ubs_set_bit.");
/*
   If element is a scalar copy user data into scalar shared data instance.
*/
    if (components == -1) {	/* if element is a scalar		    */
/*
   Get component data type.
*/
      data_type = idd_get_shared_data_type(
        ((transaction_record *)transaction)->receiver->data_definition,
        element_change->element_name,
        NULL
      );
      check_status("add_data_to_ecr: status from idd_get_shared_data_type.");
/*
   If element is of buffer type create shared memory buffer and copy in 
   user data.
*/
      if (data_type == idd_buffer) {	/* buffer type			    */
/*
   Free any existing buffer body regardless of if there is new user data.
*/
        shm_buffer = (idd_buffer_type *)element_change->data;

        if (shm_buffer->body != NULL) {	/* if existing buffer body	    */
          ism_free_shared_memory(shm_buffer->body);
          check_status("add_data_to_ecr: status from ism_free_shared_mem.");
        }				/* end if existing buffer body	    */

        user_buffer = (idd_buffer_type *)data;

        if (user_buffer->length != 0) {    /* length is non-zero	    */

          shm_buffer->length = user_buffer->length;
          shm_buffer->type = user_buffer->type;

          shm_buffer->body = ism_get_shared_memory(user_buffer->length);
          check_status("add_data_to_ecr: status from ism_get_shared_memory.");

          ism_copy_into_shared_memory(
            shm_buffer->body, 
            user_buffer->length, 
            user_buffer->body
          );
          check_status("add_data_to_ecr: status from ism_copy_into_shm.");

          DEBUG3(
            fprintf(
              stderr,
              "add_data_to_ecr: buffer length = %d.\n",
              shm_buffer->length
            );
            fprintf(
              stderr,
              "shm_buffer->type = %s.\n",
              idd_get_type_string(shm_buffer->type)
            );
            fprintf(
               stderr,
              "add_data_to_ecr: buffer body = %s.\n",
              shm_buffer->body
            );
          );
	}			/* end if non-zero buffer length	    */

      }				/* end if buffer type			    */
/*
   Copy user data into shared memory.
*/
      else {			/* else if not buffer type		    */

        ism_copy_into_shared_memory(element_change->data, length, data);
        check_status("add_data_to_ecr: status from ism_copy_into_shm.");

      }				/* end else not buffer type		    */

    }				/* end if element is a scalar		    */
    else {			/* element is a record			    */
      component_list = idd_create_component_list(
        ((transaction_record *)transaction)->receiver->data_definition,
        element_change->element_name
      );
      check_status("add_data_to_ecr: status from idd_create_component_list.");

      for_all_3list(
        component_list,
        ((transaction_record *)transaction)->receiver->data_definition,
	element_change,
        data,
        copy_component
      );

      idd_destroy_component_list(component_list);
      check_status("add_data_to_ecr: status from idd_destroy_comp_list.");
    }					/* end if element is a record	    */

  }					/* end if element specified	    */
  else {				/* component specified		    */
/*
   Get the length, offset and position of the component.
*/
    idd_get_component_lop(
      ((transaction_record *)transaction)->receiver->data_definition,
      element_change->element_name,
      component_name,
      &length,
      &offset,
      &position
    );
    check_status("isd_add_data_to_ecr: status from idd_get_component_lop.");
/*
   Set appropriate change bit in changed component bitstring.
*/
    ubs_set_bit(element_change->changed_components, position);
    check_status("isd_add_shrd_data: status from ubs_set_bit.");
/*
   Get component data type.
*/
    data_type = idd_get_shared_data_type(
      ((transaction_record *)transaction)->receiver->data_definition,
      element_change->element_name,
      component_name
    );
    check_status("add_data_to_ecr: status from idd_get_shared_data_type.");
/*
   If buffer handle special case.
*/
    if (data_type == idd_buffer) {	/* buffer type			    */

      user_buffer = (idd_buffer_type *)data;

        DEBUG3(
          fprintf(
            stderr,
            "add_data_to_ecr: user_buffer->length = %d.\n",
            user_buffer->length
          );
        );

      if (user_buffer->length != 0) {    /* length is non-zero	    */

        shm_buffer = (idd_buffer_type *)
	  ((char *)element_change->data + offset);
        shm_buffer->length = user_buffer->length;
        shm_buffer->type = user_buffer->type;

        shm_buffer->body = ism_get_shared_memory(user_buffer->length);
        check_status("add_data_to_ecr: status from ism_get_shared_memory.");

        ism_copy_into_shared_memory(
          shm_buffer->body, 
          user_buffer->length, 
          user_buffer->body
        );
        check_status("add_data_to_ecr: status from ism_copy_into_shm.");

        DEBUG3(
          fprintf(
            stderr,
            "add_data_to_ecr: buffer length = %d.\n",
            shm_buffer->length
          );
          fprintf(
            stderr,
            "shm_buffer->type = %s.\n",
            idd_get_type_string(shm_buffer->type)
          );
          fprintf(
             stderr,
            "add_data_to_ecr: buffer body ptr = %x.\n",
            shm_buffer->body
          );
          fprintf(
             stderr,
            "add_data_to_ecr: buffer body = %s.\n",
            shm_buffer->body
          );
        );

      }					/* end if buffer length is nonzero  */

    }					/* end if buffer type		    */
    else {				/* not buffer type		    */
/*
   Copy user data into shared memory.
*/
      ism_copy_into_shared_memory(
        (element_change->data)+offset, 
        length, 
        data
      );
      check_status("add_data_to_ecr: status from ism_copy_into_shm.");
    }					/* end else not buffer type	    */
  }					/* end else component specified	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "add_data_to_ecr: exiting.\n");
  );
  return;
}                                       /* end add_data_to_ecr 	    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_add_shared_data_with_id
| 
|  Description:  
|     The add shared data with id routine creates an instance for the specified 
|     shared data element.  
|     The data may be either initialized or uninitialized. 
\*--------------------------------------------------------------------------*/
void  isd_add_shared_data_with_id(transaction, element_name, component_name, 
				data, id)
isd_trans transaction;
string element_name, component_name;
caddr_t data;
iid_id_type id;
{                                       /* local variable definitions       */
element_change_record *element_change;
/*
   Initialization.
*/
  DEBUG2(
    fprintf(stderr, "isd_add_shared_data: call.\n");
    fprintf(stderr, "isd_add_shared_data: transaction = %x.\n", transaction);
    fprintf(
      stderr, 
      "isd_add_shared_data: element name = %s.\n", 
      element_name
    );
    fprintf(
      stderr, 
      "isd_add_shared_data: component name = %s.\n", 
      component_name
    );
    fprintf(stderr, "isd_add_shared_data: data = %x.\n", data);
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_transaction_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_add_shared_data:");
    );
    return;
  }					/* end if invalid transaction hand  */

  if (element_name == NULL) {		/* null element string		    */
    set_status(null_element_name);
    DEBUG1(
      isc_print_status("isd_add_shared_data:");
    );
    return;
  }					/* end if null element string	    */
/*
   Create an element change record.
*/
  element_change = create_element_change_record(
    transaction, 
    element_name
  );
  check_status(
    "isd_add_shared_data:  status from create_element_change_record."
  );
/*
   Add data to element change record.
*/
  add_data_to_ecr(transaction, element_change, component_name, false, data);
  check_status(
    "isd_add_shared_data:  status from add_data_to_ecr."
  );
/*
   Set change type in element change record to create.
*/
  element_change->change_type = isd_create;

/*
  add id to element change record.
*/
  element_change->id = id;
/*
   Add change record to change table associated with transaction.
*/
  ush_add_to_shm_hashtable(
    ((transaction_record *)transaction)->change_table, 
    element_change, 
    id
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_add_shared_data: id = %d.\n", id);
    fprintf(stderr, "isd_add_shared_data: exiting.\n");
  );
  return;
}                                       /* end isd_add_shared_data 	    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_add_shared_data
| 
|  Description:  
|     The add shared data routine creates an instance for the specified 
|     shared data element and returns a unique ID.  
|     The data may be either initialized or uninitialized. 
\*--------------------------------------------------------------------------*/
iid_id_type isd_add_shared_data(transaction, element_name, component_name, 
				data)
isd_trans transaction;
string element_name, component_name;
caddr_t data;
{
iid_id_type id;

/*
   Create ID 
*/
  id = iid_create_id();
  rcheck_status(
    "isd_add_shared_data:  status from idd_create_id.",
    NULL
  );
  isd_add_shared_data_with_id(transaction, element_name, component_name,
                              data, id);

  return id;
}

/*--------------------------------------------------------------------------*\
|  Routine: isd_put_shared_data
| 
|  Description:  
|     The put shared data call is used to put information into shared data.
\*--------------------------------------------------------------------------*/
void isd_put_shared_data(transaction, id, element_name, component_name, data)
isd_trans transaction;
iid_id_type id;
string element_name, component_name;
caddr_t data;
{                                       /* local variable definitions       */
element_change_record *element_change;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_put_shared_data: call.\n");
    fprintf(stderr, "isd_put_shared_data: transaction = %x.\n", transaction);
    fprintf(stderr, "isd_put_shared_data: id = %d.\n", id);
    fprintf(stderr, "isd_put_shared_data: element name = %s.\n", element_name);
    fprintf(stderr, "isd_put_shared_data: component name = %s.\n", component_name);
    fprintf(stderr, "isd_put_shared_data: data = %x.\n", data);
  );
/*
   Validate required parameters.
*/
  if (!isc_validate(transaction, isc_transaction_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_put_shared_data:");
    );
    return;
  }					/* end if invalid transaction hand  */

  if (element_name == NULL) {		/* null element string		    */
    set_status(null_element_name);
    DEBUG1(
      isc_print_status("isd_put_shared_data:");
    );
    return;
  }					/* end if null element string	    */
/*
   Find the change record associated with the given id on the list of 
   changes associated with the transaction.
*/
  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((transaction_record *)transaction)->change_table,
    id
  );
/*
   If id is not found in transaction create a new element change record
   and add it to the transaction.
*/
  if (element_change == NULL) {		/* assume id not in transaction     */
/*
   Create an element change record.
*/
    element_change = create_element_change_record(
      transaction, 
      element_name
    );
    check_status("isd_put_shared_data:  status from crt_elem_chg_rcrd.");
/*
   Add data to element change record.
*/
    add_data_to_ecr(
      transaction, 
      element_change, 
      component_name, 
      false,
      data
    );
    check_status("isd_put_shared_data:  status from add_data_to_ecr.");
/*
   Set id to specified id.
*/
    element_change->id = id;
/*
   Set change type in element change record to modify.
*/
    element_change->change_type = isd_modify;
/*
   Add change record to change table associated with transaction.
*/
    ush_add_to_shm_hashtable(
      ((transaction_record *)transaction)->change_table, 
      element_change, 
      id
    );
  }					/* end if id not in transaction     */
/*
   Else modify existing (create) element change record with new data.
*/
  else {				/* element change record found	    */
/*
   Modify data in element change record.
*/
    add_data_to_ecr(
      transaction, 
      element_change, 
      component_name, 
      true, 
      data
    );
    check_status("isd_put_shared_data: status from add_data_to_ecr.");
  }					/* end else ecr found		    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_put_shared_data: exiting.\n");
  );
  return;
}                                       /* end isd_put_shared_data 	    */

/*--------------------------------------------------------------------------*\
|  Routine: delete_buffer
| 
|  Description:  
|     The delete buffer routine examines a component to determine if it
|     is a buffer.  If the body of the buffer is not null it is deleted.
\*--------------------------------------------------------------------------*/
void delete_buffer(data_definition, element_change, component_name)
idd_data_definition data_definition;
element_change_record *element_change;
string component_name;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  int length, offset, position;
  idd_buffer_type *shm_buffer;

  DEBUG2(
    fprintf(stderr, "delete_buffer: call.\n");
    fprintf(
      stderr, 
      "delete_buffer: data definition = %x.\n", 
      data_definition
    );
    fprintf(
      stderr, 
      "delete_buffer: element change = %x.\n", 
      element_change
    );
    fprintf(stderr, 
      "delete_buffer: component name = %s.\n", 
      component_name
    );
  );
/*
   Get the length, offset and position of the component.
*/
  idd_get_component_lop(
    data_definition,
    element_change->element_name,
    component_name,
    &length,
    &offset,
    &position
  );
  check_status("delete_buffer: status from idd_get_lngthNoff.");
/*
   Test appropriate change bit in changed component bitstring.
*/
  if (ubs_test_bit(element_change->changed_components, position)) {
/*
   Get component data type.
*/
    data_type = idd_get_shared_data_type(
      data_definition,
      element_change->element_name,
      component_name
    );
    check_status("delete_buffer: status from idd_get_shared_data_type.");
/*
   If component is of buffer type free buffer storage.
*/
    if (data_type == idd_buffer) {	/* buffer type			    */

      shm_buffer = (idd_buffer_type *)((char *)element_change->data + offset);

      DEBUG3(
        fprintf(stderr, 
          "delete_buffer: component name = %s.\n", 
          component_name
        );
        fprintf(
          stderr,
          "delete_buffer: buffer length = %d.\n",
          shm_buffer->length
        );
        fprintf(
          stderr,
          "shm_buffer->type = %s.\n",
          idd_get_type_string(shm_buffer->type)
        );
        fprintf(
           stderr,
          "delete_buffer: buffer ptr = %x.\n",
          shm_buffer->body
        );
        fprintf(
           stderr,
          "delete_buffer: buffer body = %s.\n",
          shm_buffer->body
        );
      );

      if (shm_buffer->body != NULL) {	/* body is not null		    */

        ism_free_shared_memory(shm_buffer->body);
        check_status("delete_buffer: status from ism_free_shared_memory.");

      }					/* end if non-zero length buffer    */

    }					/* end if buffer type		    */
  }					/* end if component changed	    */
/*
   Return.
*/
  return;
}					/* end delete_buffer		    */

/*--------------------------------------------------------------------------*\
|  Routine: free_element_change_record
| 
|  Description:  
|     Internal routine to free storage associated with an element change 
|     record.
\*--------------------------------------------------------------------------*/
void free_element_change_record(data_definition, element_change)
idd_data_definition data_definition;
element_change_record *element_change;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  LIST component_list;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "free_element_change_record: call.\n");
    fprintf(
      stderr, 
      "free_element_change_record: data definition = %x.\n",
      data_definition
    );
    fprintf(
      stderr, 
      "free_element_change_record: element_change = %x.\n",
      element_change
    );
  );
/*
   If the change type is not remove free all allocated storage.
*/
  if (element_change->change_type != isd_remove) { /* chg type = remove	    */

    DEBUG3(
      fprintf(
        stderr, 
        "free_element_change_record: about to free changed_comps.\n"
      );
    );
/*
   If element is a record create a list of changed components and 
   for each buffer component free associated buffer.
*/
    data_type = idd_get_shared_data_type(
      data_definition,
      element_change->element_name,
      NULL
    );
    check_status("free_elem_chg_rcrd: status from idd_get_shared_data_type.");

    if (data_type == idd_record) {	/* element is a record		    */

      component_list = idd_create_component_list(
        data_definition,
        element_change->element_name
      );
      check_status("f.e.c.r.: status from idd_create_component_list.");

      for_all_2list(
        component_list,
        data_definition,
	element_change,
        delete_buffer
      );

      idd_destroy_component_list(component_list);
      check_status("f.e.c.r.: status from idd_destroy_comp_list.");
    }					/* element is a record		    */
    else if (data_type == idd_buffer) {	/* element is a scalar and buffer   */
      ism_free_shared_memory(((idd_buffer_type *)element_change->data)->body);
    }					/* element is a scalar and buffer   */

    ubs_destroy_bitstring(element_change->changed_components);
    check_status("free_elem_chg_rec: status from ubs_dstry_bitstring.");

    DEBUG3(
      fprintf(stderr, "free_element_change_record: about to free data.\n");
    );

    ism_free_shared_memory(element_change->data); 
    check_status("free_element_change_rcrd: status from ism_free_shm.");
  }					/* end if change type is remove     */
/*
   Free storage associated with element name.
*/

  DEBUG3(
    fprintf(stderr, "free_elem_change_record: about to free element_name.\n");
  );

  ism_free_shared_memory(element_change->element_name);
  check_status("free_element_change_record: status from ism_free_shm.");
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "free_element_change_record: exiting.\n");
  );
  return;
}                                       /* end free_element_chnge_record    */

/*--------------------------------------------------------------------------*\
|  Routine: isd_remove
| 
|  Description:  
|     The remove routine is used to remove a specified shared data element.
\*--------------------------------------------------------------------------*/
void isd_remove_shared_data(transaction, element_name, id)
string element_name;
isd_trans transaction;
iid_id_type id;          
{                                       /* local variable definitions       */
element_change_record *element_change;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_remove_shared_data: call.\n");
    fprintf(
      stderr, 
      "isd_remove_shared_data: transaction = %x.\n", 
      transaction
    );
    fprintf(stderr, "isd_remove_shared_data: id = %d.\n", id);
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_transaction_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_remove:");
    );
    return;
  }					/* end if invalid transaction hand  */

  if (element_name == NULL) {		/* null element string		    */
    set_status(null_element_name);
    DEBUG1(
      isc_print_status("isd_remove:");
    );
    return;
  }					/* end if null element string	    */
/*
   Determine if a change record associated with the given id is on the list 
   of changes associated with the transaction.
*/
  element_change = (element_change_record *)ush_get_from_shm_hashtable(
    ((transaction_record *)transaction)->change_table,
    id
  );
  DEBUG3(
    fprintf(
      stderr, 
      "isd_remove_shared_data: element change = %d.\n", 
      element_change
    );
  );
/*
   If id is not found in transaction create a new element change record,
   set the change type to remove and add it to the transaction.
*/
  if (element_change == NULL) {		/* assume id not in transaction     */
/*
   Create an element change record.
*/
    element_change = create_element_change_record(
      transaction, 
      element_name
    );
    check_status("isd_add_shared_data:  status from crte_elem_chg_rec.");
/*
   Set change type to remove.
*/
    element_change->change_type = isd_remove;    
/*
   Set id to specified id.
*/
    element_change->id = id;    
/*
   Add change record to change table associated with transaction.
*/
    ush_add_to_shm_hashtable(
      ((transaction_record *)transaction)->change_table, 
      element_change, 
      id
    );
  }					/* end if id not in transaction     */
/*
   Else if (id is in transaction) change type is create then delete
   element change record.
*/
  else if (element_change->change_type == isd_create) {	/* create	    */
/*
   Delete the change element corresponding to the given id from the
   change table associated with the transaction.
*/
    element_change = (element_change_record *)ush_delete_from_shm_hashtable(
      ((transaction_record *)transaction)->change_table, 
      id
    );
    check_null(
      element_change, 
      "isd_put_shared_data:  Could not delete element from shm hashtable.\n",
      not_found
    );
/*
   Free all storage associated with element change record.
*/
    free_element_change_record(
      ((transaction_record *)transaction)->receiver->data_definition,
      element_change
    );
    check_status("isd_remove_shared_data: status from free_elem_chnge.");
  }				/* end if change type is create		    */
/* 
   Else if change type is modify change change type from modify to remove,
   delete the data assoicated with the change and nullify the pointer.
*/
  else if (element_change->change_type == isd_modify) { /* modify	    */

    element_change->change_type = isd_remove;    

    ubs_destroy_bitstring(element_change->changed_components);
    check_status("isd_remove_sd: status from ubs_dstry_bitstring.");

    ism_free_shared_memory(element_change->data);
    check_status("isd_remove_shared_data: status from ism_free_shm.");

    element_change->data = NULL;
  }				/* end if change type is create		    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_remove_shared_data: exiting.\n");
  );
  return;
}                                       /* end isd_remove_shared_data       */

/*--------------------------------------------------------------------------*\
|  Routine: isd_rollback_transaction
| 
|  Description:  
|     The rollback transaction routine is used to abort a given transaction 
|     and delete the associated transaction buffer.
\*--------------------------------------------------------------------------*/
void isd_rollback_transaction(transaction)
isd_trans transaction;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_rollback_transaction: call.\n");
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_transaction_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_rollback_transaction:");
    );
    return;
  }					/* end if invalid transaction hand  */
/*
   Free storage associated with all element change records in transaction.
*/
  ush_for_all_shm_hashtable(
    ((transaction_record *)transaction)->change_table, 
    ((transaction_record *)transaction)->receiver->data_definition,
    free_element_change_record
  );
/*
   Delete the change_table hash table structure including all attached nodes.
*/
  ush_free_shm_hashtable(
    ((transaction_record *)transaction)->change_table, 
    AND_NODES
  );
/*
   Free receiver record. 
*/
  free_node(((transaction_record *)transaction)->receiver);
/*
   Free transaction record. 
*/
  free_node(transaction);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_rollback_transaction: exiting.\n");
  );
  return;
}                                       /* end isd_rollback_transaction     */

/*--------------------------------------------------------------------------*\
|  Routine: isd_commit_transaction
| 
|  Description:  
|     The commit transaction routine is used to commit a transaction to the
|     data base and notify the reciever.
\*--------------------------------------------------------------------------*/
void isd_commit_transaction(transaction)
isd_trans transaction;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "isd_commit_transaction: call.\n");
    fprintf(
      stderr, 
      "isd_commit_transaction: transaction = %x.\n", 
      transaction
    );
  );
/*
   Validate parameters.
*/
  if (!isc_validate(transaction, isc_transaction_record_vs)) { 
					/* invalid transaction handle	    */
    set_status(invalid_transaction_handle);
    DEBUG1(
      isc_print_status("isd_commit_transaction:");
    );
    return;
  }					/* end if invalid transaction hand  */
/*
   Send the pointer to the changes to the reciever specified in the receiver
   record.
*/

  DEBUG3(
    fprintf(stderr, "isd_commit_transaction: about to send transaction.\n");
    fprintf(stderr, 
      "isd_commit_transaction: mailbox = %x.\n", 
      ((transaction_record *)transaction)->receiver->mailbox
    );
    fprintf(stderr, 
      "isd_commit_transaction: sender = %x.\n", 
      in_box.mailbox
    );
    fprintf(stderr, 
      "isd_commit_transaction: message = %x.\n", 
      (char *)((transaction_record *) transaction)->change_table
    );
  );
/*
   Commit transaction if the hashtable is not empty, and check to
   allow recording.
*/
  if (!hashtable_empty(((transaction_record *)transaction)->change_table)) {

    ipb_record_transaction (
      (char *)((transaction_record *) transaction)->change_table,
           in_box.mailbox,
      ((transaction_record *)transaction)->receiver->mailbox);

    check_status("isd_commit_transaction: status from ipb_record_trans.");

    ipc_send(
      ((transaction_record *)transaction)->receiver->mailbox,
      in_box.mailbox,					/* sender	    */
      (char *)((transaction_record *) transaction)->change_table
    );
    check_status("isd_commit_transaction: status from ipc_send.");
  }
/*
   If empty, destroy empty hash table.
*/
  else {                                        /* empty table		*/

    ush_free_shm_hashtable(
      ((transaction_record *)transaction)->change_table,
      AND_NODES
    );

  }					/* end else empty hash		*/

/*
   Free transaction record.
*/
  free_node(transaction);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "isd_commit_transaction: exiting.\n");
  );
  return;
}                                       /* end isd_commit_transaction       */
/*--------------------------------------------------------------------------*\
|  Routine: isd_get_dialogue_name
| 
|  Description:  
|     The get dialogue name routine returns the name of the current dialogue
\*--------------------------------------------------------------------------*/
char *isd_get_dialogue_name()
{
   return "";
}

