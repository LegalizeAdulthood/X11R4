static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/din_shell.c,v 1.15 89/10/23 19:46:21 ejh Exp $";

#include <stdio.h>
#include <strings.h>
#include "serpent.h"			/* serpent externals		    */
#include "din.h"
#include "dgo.h"
#include "dgr.h"
#define d22_top
#include "d22.h"
#include "ipb.h"


/*
$Log:	din_shell.c,v $
 * Revision 1.15  89/10/23  19:46:21  ejh
 * declared fopen; got rid of ops stuff
 * 
 * Revision 1.14  89/08/02  15:33:52  bmc
 * Added din_debug variable, for use within din files.  Added 'A'
 * command to shell to allow automatic printing of firing line (see
 * din_main.c).
 * 
 * Revision 1.13  89/07/20  16:46:21  bmc
 * Commented out a log of commands for OPS/83 removal.
 * 
 * Revision 1.12  89/06/09  13:39:11  little
 * add new command to set d22_monitor
 * 
 * Revision 1.11  89/05/25  17:17:11  little
 * rearrange include order becasue of changes to d22_sne int d22.h
 * 
 * Revision 1.10  89/04/12  19:07:25  little
 * move the actual interactive shell code to a seperate routine
 * 
 * Revision 1.9  89/02/09  17:35:32  ejh
 * added copyright notice
 * 
 * Revision 1.8  89/01/04  15:36:58  little
 * add initialization and shell command setting of the c22_debug flag
 * 
 * Revision 1.7  88/11/02  13:35:48  little
 * change the P command to use the din_minor_cycle routine instead of "rolling
 * its own" and change the initial ops debug level to zero.
 * 
 * Revision 1.6  88/10/31  17:43:12  little
 * make the P shell command just print out an error message when there is
 * an "infinite" loop in the firing line and then clear out the firing
 * line.  it used to do a fatal error.
 * 
 * Revision 1.5  88/10/27  20:53:17  little
 * chage P shell command to run snippets until firing line is empty.
 * change r shell command to print firing line when done w/ops
 * 
 * Revision 1.4  88/10/13  18:10:01  little
 * put some debug print which was added last mode under the correct debug
 * control
 * 
 * Revision 1.3  88/10/12  15:59:44  little
 * add recording capability
 * 
 * Revision 1.2  88/10/11  17:42:40  little
 * set ubo_debug to 1
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

/* --------------------     KLUDGES    ----------------------- */

static char *scanf_strings[] = {
	"%d",	/* boolean */
	"%d",	/* integer */
	"%f",	/* real */
	"%s",	/* string */
	"%Q",	/* record */
	"%d",	/* id */
	"%Q"	/* buffer */
	};
extern int isd_debug;
extern int idd_debug;
extern int iid_debug;
extern int ipc_debug;
extern int ids_debug;
extern int ism_debug;
extern int isc_debug;
extern int ubs_debug;
extern int ubo_debug;

extern int d22_debug;
extern int dob_debug;
extern int dra_debug;
extern int drc_debug;
extern int dro_debug;
extern int dtt_debug;

boolean din_debug; /* owned by this bugger */

/*
???  maybe to put these in an h file, sahred between dm & compiler
*/
int c22_debug;
int c22_slang_line_num;

/*
??? MAX_CYCLES should be in an h, it is also used in din_main.c
*/
#define MAX_CYCLES 10

/* --------------------     end KLUDGES    ----------------------- */


/* -----------------------------  module globals  ------------------- */
static char din_recording_file_name [256];

/* ------------------------------------------------------ din_get_id */
/* 
    this routine will find the ops wme # from a serpent id requested from
    the operator
*/

/* -------------------------------------------  din_get_shared_data_type */
/*
   this routine returns the type of the shared data component passed in
*/
idd_data_types din_get_shared_data_type (element, component)
string element;
string component;

{  LIST tap_list;
   isd_process tap;
   idd_data_definition dd;
   idd_data_types ret_val;

   ret_val = (idd_data_types) -1;
   tap_list = (LIST) cus_get_prop (d22_god_vc, taps_prop);
   loop_through_list (tap_list, tap, isd_process)
      {dd = isd_get_data_definition (tap);
      ret_val = idd_get_shared_data_type (dd, element, component);
      if (ret_val != (idd_data_types) -1)
         return (ret_val);
      }
   return (ret_val);
}					/* end din_get_shared_data_type */


/* -------------------------------------------  din_get_component_lop */
/*
   this routine returns the length, offset and position (via out parameters)
   of the shared data component passed in.  the returned value indicates
   that the component was actually in the element
*/
boolean din_get_component_lop (element, component, length, offset, position)
string element;
string component;
int *length, *offset, *position;
{  LIST tap_list;
   isd_process tap;
   idd_data_definition dd;

   tap_list = (LIST) cus_get_prop (d22_god_vc, taps_prop);
   loop_through_list (tap_list, tap, isd_process)
      {dd = isd_get_data_definition (tap);
      idd_get_component_lop (dd, element, component,
                        length, offset, position);
      if (!get_status ())
         return (TRUE);
      }
   return (FALSE);
}						/* end din_get_component_lop */

 
/* --------------------------------------------------  din_modify_element */
/*
   THIS ROUTINE IS OBSOLETE, DUE TO THE FACT THAT SHARED DATA ELEMENT
   DATA VALUES ARE NOT KEPT IN OPS ANYMORE.

   this routine modifies a specific component value within an ops wme which
   represents a sahred data element
*/

/* --------------------------------------------- din_fix_string */
/*
   this routine takes a string and replaces the first lf/cr with null character
   it is useful for "fixing up string read in via (f)gets
*/
static void din_fix_string (the_string)
char *the_string;
{
   for (;
        ((*the_string != '\n') &&
         (*the_string != '\0') &&
         (*the_string != '\f'));
        the_string++) ;
   if (*the_string != '\0')
      *the_string = '\0';
   return;
}						/* end din_fix_string */


/* --------------------------------------------- din_start_recording */
/*
   this function starts transaction recording on the file requested from
   the operator
*/
void din_start_recording ()
{  char comment_string [256];
   FILE *fopen (), *stream;

   if (d22_recording_on)
      {printf ("Recording is already happening onto file %s\n",
	      din_recording_file_name);
      return;
      }
GET_FILE_NAME:
   printf ("name of recording file    ");
   fflush (stdout);
   fgets (din_recording_file_name, 256, stdin);
   din_fix_string (din_recording_file_name);
   stream = fopen (din_recording_file_name, "r");
   if (stream)
      {printf ("file exists\n");
      fclose (stream);
      goto GET_FILE_NAME;
      }
   stream = fopen (din_recording_file_name, "w");
   if (!stream)
      {printf ("cannot write to file\n");
      goto GET_FILE_NAME;
      }
    else
      fclose (stream);
   printf ("comment string for recording     ");
   fgets (comment_string, 256, stdin);
   din_fix_string (comment_string);
   DEBUG3 (fprintf (stderr, "din_recording_file_name = %s\n",
		din_recording_file_name);
      fprintf (stderr, "comment_string is %s\n", comment_string););
   ipb_start_recording (din_recording_file_name, comment_string);
   d22_recording_on = TRUE;
   return;
}					/* end din_start_recording */


/* --------------------------------------------- din_stop_recording */
/*
   this function turns off transaction recording
*/
void din_stop_recording ()
{
   if (!d22_recording_on)
      {printf ("Recording is not on\n");
      return;
      }
   ipb_stop_recording ();
   d22_recording_on = FALSE;
   return;
}						/* end din_stop_recording */


/* --------------------------------------------- din_print_debug_level */
/*
   this function prints out the current dm debug level
*/
void din_print_debug_level ()
{
   printf ("current debug level = %d\n", d22_debug);
   return;
}						/* end din_print_debug_level */


/* --------------------------------------- din_set_interface_debug_level */
/*
   this routine sets the various interface debug variables to the level
   requested from the operator
*/
void din_set_interface_debug_level ()
{
   int d_lev;

   printf ("input desired interface debug level    ");
   fflush (stdout);
   scanf ("%d", &d_lev);
   isd_debug = d_lev;
   idd_debug = d_lev;
   iid_debug = d_lev;
   ipc_debug = d_lev;
   ids_debug = d_lev;
   ism_debug = d_lev;
   isc_debug = d_lev;
   ubs_debug = d_lev;
   ubo_debug = d_lev;
   return;
}					/* end din_set_interface_debug_level */


/* ------------------------------------------------- din_set_debug_level */
/*
   this routine set the various dm debug variables to the level requested from
   the operator
*/
void din_set_debug_level ()
{
   int d_lev;

   printf ("input desired dm shell debug level    ");
   fflush (stdout);
   scanf ("%d", &d_lev);
   d22_debug = d_lev;
   dob_debug = d_lev;
   dra_debug = d_lev;
   drc_debug = d_lev;
   dro_debug = d_lev;
   dtt_debug = d_lev;
   c22_debug = d_lev;
   return;
}						/* end din_set_debug_level */


/* ---------------------------------------------------  din_turn_on_monitor */
/*
   this routine seets the internal dm monitor flag on
*/
static void din_turn_on_monitor ()
{
   d22_monitor = TRUE;
   return;
}						/* end din_turn_on_monitor */


/* ------------------------------------------------------  din_do_shell */
/*
   this routine is the acutal dm interactive shell
*/
void din_do_shell ()
{  int doit = 1;
   char command = 'a';
   char junk [256];
   int num_cycles;

   printf ("C>> ");
   while (doit)
      {
   	scanf (" %c", &command);
   	switch (command)
   	   {case '\n':
   		break;
   	    case 'I':
   		din_set_interface_debug_level ();
   		break;
   	    case 'D':
   		din_set_debug_level ();
   		break;
   	    case 'L':
   		din_print_debug_level ();
   		break;
   	    case 'G':
   		din_process_xaction (FALSE);
   		break;
   	   case  'F':
   		din_main ();
   		break;
   	   case  'Q':
   		doit = 0;
   		break;
   	   case  'S':
   		din_start_transactions ();
   		break;
   	   case  'C':
   		din_commit_transactions ();
   		break;
   	   case  'R':
   		din_rollback_transactions ();
   		break;
   	   case  'p':
		din_print_firing_line (); /* defined in din_main.c */
   		break;
	   case 'A':
		din_debug = ! din_debug;
		printf ("automatic firing line printing is %s\n",
		   din_debug ? "on" : "off");
		break;
   	   case  'P':
			  /* 
			     -1 as first parameter indicates we are calling
			     din_minor_cycle from the shell instead of the
			     "forever" main loop code in din_main
			  */
                din_minor_cycle (-1);
   		break;
           case 'T':
				/* flush input */
                fgets (junk, 256, stdin);
                din_start_recording ();
                break;
           case 't':
                din_stop_recording ();
                break;
	   case 'M':
	        din_turn_on_monitor ();
                break;
   	   case  '?':
   		printf ("F	run the DM 'forvever'\n");
                printf ("\n");
                printf ("T	start transaction recording\n");
                printf ("t	stop transaction recording\n");
                printf ("\n");
   		printf ("L	print current dm debug level\n");
   		printf ("D	set dm debug level\n");
   		printf ("I	set interface debug level\n");
                printf ("\n");
   		printf ("G	get transaction (no wait)\n");
   		printf ("S	start transactions\n");
   		printf ("C	commit transactions\n");
   		printf ("R	rollback transactions\n");
   		printf ("p	print firing line contents\n");
		printf 
		("A	toggle printing firing line just as it fires.\n");
   		printf ("P	process firing line\n");
                printf ("\n");
                printf
		    ("M	turn on monitoring (it cannot be turned off!)\n");
                printf ("\n");
   		printf ("Q	quit\n");
   		printf ("?	display  this information\n");
                printf ("\n");
   		break;
   	   default:
   		printf ("\ndo not grok %c command.  try ?\n",
   		   command);
   		break;
   	   }
   	if (command != '\n')
   		printf ("C>> ");
      }
}						/* end din_do_shell */


/* ------------------------------------------------------  din_shell_c */
/*
   this routine starts the dm 
*/
void din_shell_c (loop_forever)
boolean loop_forever;
{  int temp_debug_lev;

   d22_debug = 1;
   temp_debug_lev = 0;

   isd_debug = 1;
   idd_debug = 1;
   iid_debug = 1;
   ipc_debug = 1;
   ids_debug = 1;
   ism_debug = 1;
   isc_debug = 1;
   ubs_debug = 1;
   ubo_debug = 1;

   c22_debug = 1;

   dob_debug = 1;
   dra_debug = 1;
   drc_debug = 1;
   dro_debug = 1;
   dtt_debug = 1;

   din_debug = false;

   din_init ();
   if (loop_forever)
      din_main ();
   din_do_shell ();
   din_cleanup ();
   printf ("Serpent Dialogue Manager exiting.\n");
   return;
}						/* end din_shell_c */


