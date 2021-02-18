static char rcsid [] =
	"$Header: /u/04c/mnt/integ/tools/src/RCS/reader.c,v 1.4 89/02/13 14:39:18 ejh Exp $";

/*--------------------------------------------------------------------------*\
| 
|  Name:  Technology Example.
|
|  Description:  
|     Provides test bed for interface functions.
|     
|     
$Log:	reader.c,v $
 * Revision 1.4  89/02/13  14:39:18  ejh
 * added copyright notice
 * 
 * Revision 1.3  89/02/09  17:34:51  little
 * no changes
 * 
 * Revision 1.2  88/10/10  19:32:17  little
 * fix to output buffer info
 * 
|
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

#include <malloc.h>
#include "serpent.h"			/* serpent externals		    */

/*-------------------- Constants -------------------------------------------*/


/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- KLUDGES          ------------------------------------*/
#define DEBUG1(statement) statement;

static char *change_type_names[] = {"create", "modify", "remove",
		"UNKNOWN CHANGE TYPE"};

extern int isd_debug;
extern int idd_debug;
extern int iid_debug;
extern int ipc_debug;
extern int ids_debug;
extern int ism_debug;
extern int isc_debug;
extern int ubs_debug;


/*-------------------- Retained Data ---------------------------------------*/

isc_status_codes status;
isd_trans transaction;
iid_id_type id;
char * fred_buffer;
 
string element_name;
isd_change_type change;
LIST changed_component_list;


                /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                                process_component_name
                   &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
void process_component_name (nada, component_name)
string nada, component_name;
{				/* local variables			    */
   int length, offset, position;
   idd_data_types data_type;
   int *int_ptr;
   float *float_ptr;
   char *buffer_info;

   isd_get_component_lop (element_name, component_name,
                        &length, &offset, &position);
   data_type = isd_get_shared_data_type (element_name,
                                         component_name);
   printf ("   %s: ", component_name);
   switch (data_type)
      {case idd_boolean:
         int_ptr = (int *) (fred_buffer + offset);
         printf ("%X\n", *int_ptr);
         break;
      case idd_integer:
         int_ptr = (int *) (fred_buffer + offset);
         printf ("%d\n", *int_ptr);
         break;
      case idd_real:
         float_ptr = (float *) (fred_buffer + offset);
         printf ("%f\n", *float_ptr);
         break;
      case idd_string:
         printf ("%s\n", (fred_buffer + offset));
         break;
      case idd_record:
         printf ("CANNOT PRINT RECORDS RIGHT NOW\n");
         break;
      case idd_id:
         int_ptr = (int *) (fred_buffer + offset);
         printf ("ID, VALUE = %d\n", *int_ptr);
         break;
      case idd_buffer:
         buffer_info = ((idd_buffer_type *) (fred_buffer + offset))-> body;
         printf ("BUFFER, length = %d\n",
                ((idd_buffer_type *) (fred_buffer + offset))-> length);
         printf ("\t\t0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",
		(*buffer_info), (*(buffer_info+1)), (*(buffer_info+2)),
		(*(buffer_info+3)), (*(buffer_info+4)), (*(buffer_info+5)),
		(*(buffer_info+6)), (*(buffer_info+7)), (*(buffer_info+8)),
		(*(buffer_info+9))
		);
         break;
      default:
         printf ("bad data type of %d in component %s in file %s, line %d\n",
                data_type, component_name, __FILE__, __LINE__);
         exit (1);
      }
   return;
}


char *get_change_type_name (the_change)
isd_change_type the_change;
{
   switch (the_change)
      {
      case isd_create:
        return (change_type_names[(int) isd_create]);
      case isd_modify:
        return (change_type_names[(int) isd_modify]);
      case isd_remove:
        return (change_type_names[(int) isd_remove]);
      default:
        return (change_type_names[3]);
      }
}


void process_element ()
{
   int length;


   element_name = isd_get_element_name (transaction, id);
   check_status ("pe: bad status from isd_get_element_name.\n");

   change = isd_get_change_type (transaction, id);
   check_status ("pe:  bad status from isd_get_change_type.\n");
   printf("ID = %d, change type = %s, element_name = %s\n", id,
	get_change_type_name (change), element_name);

   if (change != isd_remove)
      {length = isd_get_length (element_name);
      fred_buffer = malloc (length);
      isd_incorporate_changes (transaction, id, fred_buffer);
      check_status ("pe: bad status from isd_incorporate_channges.\n");
   
      changed_component_list =
			isd_create_changed_component_list (transaction, id);
      check_status ("pe:  bad status from isd_create_changed_comp_list.\n");
   
      for_all_list (changed_component_list, NULL, process_component_name);
   
      free (fred_buffer);
      fred_buffer = NULL;
      isd_destroy_changed_component_list (changed_component_list);
      check_status ("pe:  bad status from isd_destroy_changed_comp_list.\n");
      }
}			
  

                /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                                reader_initialize
                   &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
void reader_initialize (mailbox, ill_file)
char *mailbox, *ill_file;
{
   isc_status_codes status;

   serpent_init (mailbox, ill_file);
   if (status = get_status())
      {
         printf ("reader_initialize: bad status of %d from serpent_init\n",
                status);
         fflush(stdout);
         exit (1);
      }
   return;
}


                /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                                reader_cleanup
                   &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
void reader_cleanup ()
{
   isc_status_codes status;

   serpent_cleanup ();
   check_status ("reader_cleanup: bad status from serpent_cleanup.\n");
   return;
}

                /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                                main
                   &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{				/* local variables			    */

   if (argc != 3)
      {
      printf ("%s called with incorrect number of paramenters\n USAGE:\n",
	argv [0]);
      printf ("	%s 'mailbox name' 'ill file name'\n", argv [0]);
      exit (1);
      }
   printf("reader.main: called.\n");
   fflush(stdout);
   isd_debug = 0;
   idd_debug = 0;
   iid_debug = 0;
   ipc_debug = 0;
   ids_debug = 0;
   ism_debug = 0;
   isc_debug = 0;
   ubs_debug = 0;
   reader_initialize (argv[1], argv [2]);


fred_buffer = NULL;
while (1)
   {
    status = not_available;
    while (status == not_available)
       {
          transaction = isd_get_transaction ();
/*
          transaction = isd_get_transaction_no_wait ();
*/
          status = get_status ();
       }
   if (status != ok)
      {
         printf ("got a status from get_transaction of %d\n", status);
         printf ("BYE, BYE\n");
         exit (1);
      }
   printf ("got a transaction\n");
   fflush(stdout);

/*
    Get the id for the first element in the transaction and process the
    changed element.
*/
   id = isd_get_first_changed_element(transaction);
   check_status("reader:  bad status from isd_get_first_changed_element.\n");
/*
    Get the id of the remaining changed elements.
*/
   while (id != (iid_id_type)-1) {

     process_element ();

     id = isd_get_next_changed_element(transaction);
     check_status("reader:  bad status from isd_get_next_changed_element.\n");

   }				/* end while remaining changed elements     */
/*
    Purge transaction.
*/
   isd_purge_transaction(transaction);
   check_status("reader:  bad status from isd_purge.\n");
   }


/*
    Cleanup and return.
*/
   serpent_cleanup();
   check_status("reader:  bad status from isd_cleanup.\n");

   printf("reader.main: exiting.\n");
   fflush(stdout);

   return;
}
