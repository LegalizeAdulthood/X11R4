static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/din_main.c,v 1.27 89/10/23 19:45:26 ejh Exp $";

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

/*
$Log:	din_main.c,v $
 * Revision 1.27  89/10/23  19:45:26  ejh
 * changed names of generated files; declared fopen
 * 
 * Revision 1.26  89/08/30  16:37:37  little
 * change default source code location message
 * 
 * Revision 1.25  89/08/29  17:40:23  little
 * change version number and change god to eve
 * 
 * Revision 1.24  89/08/02  15:32:50  bmc
 * Added din_print_firing_line() function.  din_minor_cycle() now prints
 * out firing line if external variable din_debug is true.
 * 
 * Revision 1.23  89/07/20  16:44:57  bmc
 * Changes for OPS/83 removal.  Bumped version number up as well.
 * 
 * Revision 1.22  89/06/14  10:57:01  little
 * change version number
 * change symbol tab init call to tell sym tab to disable redefined processing
 * 
 * Revision 1.21  89/06/09  13:38:08  little
 * add d22_monitor var initialize and check to ensure size of int = size of char *
 * 
 * Revision 1.20  89/05/26  10:57:32  little
 * change version number
 * 
 * Revision 1.19  89/05/22  11:09:44  bmc
 * Now explicitly include 'uer.h'.  Related to changes to 'dob.c', 'dob.h'
 * and 'uer.h'.
 * 
 * Revision 1.18  89/05/22  08:56:10  little
 * add welcome and finished initialization messages and version # stuff
 * 
 * Revision 1.17  89/05/19  17:28:57  bmc
 * Changed to accomodate new definitions in 'dob.h'.
 * 
 * Revision 1.16  89/02/21  23:52:49  rcs
 * din_terminate exits with (1) instead of (-1)
 * 
 * Revision 1.15  89/02/15  17:55:17  little
 * change the names of the dialogue init and symbol table data files
 * to include the name of the dialogue
 * 
 * Revision 1.14  89/02/01  21:59:49  rcs
 * modified din_run_time_error to not delete the object base until
 *   after calling the print routine
 * 
 * Revision 1.13  89/01/09  14:51:35  little
 * no changes
 * 
 * Revision 1.12  88/12/19  18:04:20  rcs
 * added din_run_time_error 
 * 
 * Revision 1.11  88/12/14  11:05:59  little
 * start and commit transactions before and after god is made.  this allows
 * god to have vars, objs, creation/destruction actions.
 * 
 * Revision 1.10  88/11/04  17:03:23  little
 * add initialization of d22_slang_source
 * 
 * Revision 1.9  88/11/03  10:22:45  little
 * change cntl-c to NOT do a core dump and cntl-\ TO DO a core dump
 * 
 * Revision 1.8  88/11/02  13:32:48  little
 * change din_minor_cycle to correctly continue to look at the firing
 * line and exhaust it before running ops.  also, make running ops
 * under control of a parameter.
 * 
 * Revision 1.7  88/10/31  16:40:56  little
 * change the error processing for when there are too many minor cycles in
 * a major cycle.  now it just prints an error, and resets the firing_line
 * to empty.  it used to exit.
 * 
 * Revision 1.6  88/10/27  19:08:56  little
 * add initialization of d22_temp_free_list
 * 
 * Revision 1.5  88/10/12  16:00:32  little
 * add recording capability
 * 
 * Revision 1.4  88/10/03  09:12:28  little
 * add dm shared data
 * 
*/

#include <stdio.h>
#include <strings.h>
#include <signal.h>
#include <signal.h>
#include "serpent.h"			/* serpent externals		    */
#include "iid.h"
#include "cus.h"
#include "din.h"
#include "dob.h"
#include "dgo.h"
#include "dgr.h"
#include "d22.h"
#include "uer.h"			/* error reporting facility	    */


/*
??? compiler and run time sharing of symbolic constants
        the following symbolic constants should be in an include, which
        is used by both the compiler and here
*/

#define VERSION_NUMBER "1.48"

#define INIT_INFO_FILE "i.d"
#define SYMBOL_TABLE_DATA_FILE "t.d"
#define MON_FILE "m.d"
#define MAX_MIN_PER_MAJ 10


#define make_full_name(ext) \
   strcat (strcat (strcpy (a_string, "."), din_get_dm_name ()), ext)


extern int d22_slang_line_num;
extern boolean din_debug;

/* -------------------------------------------------   din_print_firing_line */
void din_print_firing_line ()
{
    fprintf (stderr, "d22_firing_line =\n");
    fflush (stderr);
    dgr_print_sne_list (stderr, d22_firing_line, "   ");
    fflush (stderr);
}

/* ----------------------------------------------------   din_minor_cycle */
void din_minor_cycle (num_maj)
int num_maj;
{  int num_min;

   while (list_length (d22_firing_line) > 0)
      {num_min = 1;
      while (list_length (d22_firing_line) > 0)
         {if (num_min > MAX_MIN_PER_MAJ)
            {fprintf (stderr,
                "!!! too many minor cycles in major cycle number %d !!!\n",
                num_maj);
            free_list (d22_firing_line, NOT_NODES);
            d22_firing_line = make_list ();
            }
           else
            {DEBUG3 (fprintf (stderr, "num_min = %d\n", num_min);
               dgr_print_sne_list (stderr, d22_firing_line, "   ");
               fprintf (stderr, "length of firing line = %d\n\n",
   		list_length (d22_firing_line)););

	    if (din_debug)
	      din_print_firing_line ();

            dgr_process_sne_list (d22_firing_line);
            num_min ++;
            }
         }
      }
return;
}					/* end din_minor_cycle */


/* ----------------------------------------------------   din_main  */
void din_main ()
{  int num_maj;

				/* first run a single major loop to catch
				   any initial vcs which do not depend on
				   shared data */
   num_maj = 1;
   din_start_transactions ();
   din_minor_cycle (num_maj, TRUE);
   din_commit_transactions ();
				/* now go into infinite major cycle loop */
   num_maj = 2;
   while (1)
      {din_start_transactions ();
      din_process_xaction (TRUE);  /* do not care about return value */
      din_minor_cycle (num_maj, TRUE);
      din_commit_transactions ();
      num_maj ++;
      }
   return;
}					/* end din_main */


/* ----------------------------------------------   din_start_transactions */
void din_start_transactions ()
{  LIST tap_list;
   isd_process tap;

   if (d22_xactions_open)
      {fprintf (stderr, "transactions already started\n");
      return;
      }
   d22_xactions_open  = TRUE;
   tap_list = (LIST) cus_get_prop (d22_god_vc, taps_prop);
   loop_through_list (tap_list, tap, isd_process)
      {dtt_transaction_tell (isd_get_mailbox (tap),
			     isd_start_dm_transaction (tap));
/*
?? print mailbox name for failed start transaction
*/
      if (get_status ())
         FATAL_ERROR (
		isc_print_status ("DM cannot start transaction,status = ");)
      }
   return;
}					/* end din_start_transactions */


/* ----------------------------------------------   din_commit_transactions */
void din_commit_transactions ()
{  LIST tap_list;
   isd_process tap;

   if (!d22_xactions_open)
      {fprintf (stderr, "transactions were not started\n");
      return;
      }
   d22_xactions_open  = FALSE;
   tap_list = (LIST) cus_get_prop (d22_god_vc, taps_prop);
   loop_through_list (tap_list, tap, isd_process)
      {commit_transaction (dtt_transaction_ask (isd_get_mailbox (tap)));
/*
?? print mailbox name for failed commit transaction
*/
      if (get_status ())
         FATAL_ERROR (
		isc_print_status ("DM cannot commit transaction, status = ");)
      }
   return;
}					/* end din_commit_transactions */


/* -------------------------------------------   din_rollback_transactions */
void din_rollback_transactions ()
{  LIST tap_list;
   isd_process tap;

   if (!d22_xactions_open)
      {fprintf (stderr, "transactions were not started\n");
      return;
      }
   d22_xactions_open  = FALSE;
   tap_list = (LIST) cus_get_prop (d22_god_vc, taps_prop);
   loop_through_list (tap_list, tap, isd_process)
      {rollback_transaction (dtt_transaction_ask (isd_get_mailbox (tap)));
/*
?? print mailbox name for failed rollback transaction
*/
      if (get_status ())
         FATAL_ERROR (
	     isc_print_status ("DM cannot rollback transaction, status = ");)
      }
   return;
}					/* end din_rollback_transactions */


/* -------------------------------------------   din_process_xaction */
boolean din_process_xaction (wait)
boolean wait;
{  isd_trans xaction;

   if (wait)
      xaction = isd_get_transaction ();
    else
      xaction = isd_get_transaction_no_wait ();
   if (get_status ())
      {if (!wait)
         {fprintf (stderr, "NO TRANSACTION AVAILABLE\n");
         return (FALSE);
         }
       else
         FATAL_ERROR (isc_print_status (
	   "DM got an error getting a transaction, status = ");)
      }
   if (!wait)
      fprintf (stderr, "GOT A TRANSACTION\n");
   dob_transaction_update (xaction);
   if (get_status ())
	FATAL_ERROR (isc_print_status
		("DM screwed up processing a transaction, status = ");)
   return (TRUE);
}					/* end din_process_xaction */


/* -------------------------------------------   din_abnormal_termination */
void din_abnormal_termination ()
{
   din_cleanup ();
   FATAL_ERROR (fprintf (stderr, "DM interrupted\n");)
}					/* end din_abnormal_termination */


/* -------------------------------------------   din_quit */
void din_quit ()
{
   din_cleanup ();
   fprintf (stdout, "\n\nSerpent Dialogue Manager exiting.\n");
   exit (0);
}					/* end din_quit */


/* -------------------------------------------   din_run_time_error */
void din_run_time_error ()
{
  long pid;
  char command [256];
/*
   Clean up dialogue manager and print end user error message.
*/
  uer_print_error();
  din_cleanup();

  DEBUG1(
    fprintf (
      stderr, 
      "!!!!!     DM FATAL ERROR, at or near SLANG line %d\n",
      d22_slang_line_num
    );
    fflush (stderr);
    pid = getpid ();
    sprintf (command, "gcore %d", pid);
    system (command);
    sprintf (command, "mv core.%d core", pid);
    system (command);
  );

  fprintf(stdout, "\n\nSerpent dialogue manager exiting.\n");
  exit(1);
}					/* end din_run_time_error */


/* -------------------------------------------   din_init */
void din_init ()
{  dob_data_object god_object;
   char a_string [1024];
   static char default_location [] = "<UNKNOWN LOCATION>";

   fprintf (stdout, "Welcome to Serpent Dialogue Manager, Version %s\n",
	    VERSION_NUMBER);
   if (sizeof (int) != sizeof (char *))
      FATAL_ERROR (fprintf (stderr, "\nsizeof (int) != sizeof (char *)\n"););
 
   d22_slang_source = default_location;
   d22_slang_line_num = -1;

   signal (SIGINT, din_quit);
   signal (SIGTERM, din_abnormal_termination);
   signal (SIGHUP, din_abnormal_termination);
   signal (SIGQUIT, din_abnormal_termination);
   signal(SIGUSR1, din_run_time_error);

   d22_recording_on = FALSE;
   d22_monitor = FALSE;

				/* create a smbol table and notify sym tab
				   routine that redefine processing is not
				   to be performed
				*/
   cus_create_sym_tab (FALSE);

   isd_dm_init ();
   check_status ("din_init: bad status from isd_dm_init");

   dtt_init ();
   if (get_status ())
      FATAL_ERROR (isc_print_status
		("DM cannot initialize transaction table, status = ");)
   dob_init ();
   if (get_status ())
      FATAL_ERROR (
	isc_print_status ("DM cannot initialize object base, status = ");)
   din_get_st_data (make_full_name (SYMBOL_TABLE_DATA_FILE));
   d22_xactions_open  = FALSE;
   d22_god_vc = cus_look_up (d22_god_text);
   if (!d22_god_vc)
      FATAL_ERROR (fprintf (stderr, "DM initialization cannot find EVE\n"))
   d22_firing_line = make_list ();
   d22_sne = (dgr_sne *) NULL;
   d22_temp_free_list = NULL;

   din_process_tap_info (make_full_name (INIT_INFO_FILE));

   DEBUG3 (fprintf (stderr, "EVE's id = %d, status = %d\n",
	cus_get_prop (d22_god_vc, id_prop), get_status ()););

   din_start_transactions ();
   god_object = dob_vc_create (dob_null_object,
			       (int) cus_get_prop (d22_god_vc, id_prop),
			       make_list (),
			       dob_null_object);
   din_commit_transactions ();

   fprintf (stdout, "Serpent Dialogue Manager initialization complete\n");

   return;
}					/* end din_init */


/* -------------------------------------------   din_cleanup */
void din_cleanup ()
{  char a_string [1024];
   FILE *fopen (), *mon_fp;

   if (d22_monitor)
      {mon_fp = fopen (make_full_name (MON_FILE), "w");
      cus_print_prop_nums (mon_fp, "");
      fclose (mon_fp);
      }
   if (d22_recording_on)
      din_stop_recording ();
   din_terminate_interface ();
   dob_cleanup ();
   dtt_cleanup ();
   return;
}					/* end din_cleanup */


/* -------------------------------------------   main routine */

main (argc, argv)
int argc;
char *argv [];
{
				/*
				   Call the din_shell.  Its parameter is a
				   boolean indicating whether or not to loop
				   forever.  Not looping forever implies calling
				   the dm shell.  If there are any command line
				   arguments, we want to call the dm shell.
				*/

   din_shell_c (/* loop_forever = */ --argc == 0);
   exit (0);
}
