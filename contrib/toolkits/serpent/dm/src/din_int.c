static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/din_int.c,v 1.10 89/10/23 19:47:47 ejh Exp $";

/*
$Log:	din_int.c,v $
 * Revision 1.10  89/10/23  19:47:47  ejh
 * declared fopen
 * 
 * Revision 1.9  89/08/29  17:39:45  little
 * change god to eve
 * 
 * Revision 1.8  89/05/25  17:35:47  little
 * rearrange include order becasue of changes to d22_sne int d22.h
 * 
 * Revision 1.7  89/02/09  17:34:02  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/01/06  17:16:03  little
 * no change
 * 
 * Revision 1.5  89/01/06  17:14:55  little
 * no change
 * 
 * Revision 1.4  89/01/06  17:01:45  little
 * no change
 * 
 * Revision 1.3  89/01/06  16:58:36  little
 * no change
 * 
 * Revision 1.2  88/10/03  09:12:01  little
 * add dm shared data
 * 
*/

/*--------------------------------------------------------------------------*\
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

#include <stdio.h>
#include <strings.h>
#include "serpent.h"			/* serpent externals		    */
#include "din.h"
#include "dgr.h"
#include "d22.h"


/*
??  dm box name and no dm ill file defines need to be in an h for compiler & dm
*/
#define DM_BOX_NAME "DM_BOX"
#define NON_EXISTANT_DM_ILL_FILE_NAME "HELLO_JACK"


/* ------------------------------------------------    din_process_tap_info */
void din_process_tap_info (file_name)
string file_name;
{  FILE *fopen (), *dm_init_file;
   char mailbox_name [MAX_FILE_NAME_LENGTH];
   char ill_file_name [MAX_FILE_NAME_LENGTH];
   LIST tap_list;
   isd_process tap;

   dm_init_file = fopen (file_name, "r");
   if (dm_init_file == NULL)
      FATAL_ERROR (fprintf (stderr, "CANNOT FIND %s FILE, BAILING OUT\n,",
		file_name))
   tap_list = make_list ();
   while (fscanf (dm_init_file, "%s%s", mailbox_name, ill_file_name) != EOF)
      {DEBUG3 (fprintf (stderr,
	"din_process_tap_info: mailbox_name = %s, ill_file_name = %s\n",
	mailbox_name, ill_file_name););

			/* we need to do special processing for the DM */
      if (!strcmp (mailbox_name, DM_BOX_NAME))
         {if (!strcmp (ill_file_name, NON_EXISTANT_DM_ILL_FILE_NAME))
            isd_dm_init_sdd (NULL);
           else
            isd_dm_init_sdd (ill_file_name);
         if (get_status ())
            FATAL_ERROR (
		isc_print_status ("din_init: bad status from isd_dm_init of");)
         }
        else
         {tap = isd_create_process_definition (mailbox_name, ill_file_name);
         if (get_status())
            FATAL_ERROR (isc_print_status
				(
"din_process_tap_info: problems with isd_create_process_definition status = "
				);)
         add_to_tail (tap_list, tap);
         }
      }

   if (list_length (tap_list) < 1)
      FATAL_ERROR (fprintf (stderr, "dm found no taps\n"))
   DEBUG3 (fprintf (stderr, "din_process_tap_info: tap_list length = %d\n",
	list_length (tap_list)););
   DEBUG3 (fprintf (stderr, "din_process_tap_info: 1st tap = %x\n",
	get_list_head (tap_list)););
   cus_put_prop (d22_god_vc, taps_prop, tap_list);

   fclose (dm_init_file);
   return;
}			/* end din_process_tap_info */


/* -------------------------------------------    din_terminate_interface */
void din_terminate_interface ()
{  LIST tap_list;
   isd_process tap;
   cus_sym god_vc;

   DEBUG2 (fprintf (stderr, "entering din_terminate_interface\n"))

   god_vc = cus_look_up (d22_god_text);
   if (!god_vc)
      FATAL_ERROR (fprintf (stderr, "terminate_interface cannot find EVE\n"))
   tap_list = (LIST) cus_get_prop (god_vc, taps_prop);
   DEBUG3 (fprintf (stderr, "din_terminate_interface: tap_list length = %d\n",
	list_length (tap_list)););
   loop_through_list (tap_list, tap, isd_process)
      {isd_destroy_process_definition (tap);
      if (get_status())
         FATAL_ERROR (isc_print_status
	  	(
"din_term_inter: problems with isd_destroy_process_definition status = "
		);)
      }
   isd_dm_cleanup ();
   if (get_status())
      FATAL_ERROR (isc_print_status
		("din_term_inter: problems with isd_cleanup status = ");)
   return;
   }					/* din_terminate_interface */


