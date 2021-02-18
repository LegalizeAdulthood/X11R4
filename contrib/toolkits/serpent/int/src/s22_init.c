static char rcsid [] = "$Header: s22_init.c,v 1.3 88/04/12 17:09:32 serpent Ex\
p $";
/*--------------------------------------------------------------------------*\
|
|  Name:  Initialization and Cleanup (iic)
|
|  Description:  
|     Peforms necessary initialization and cleanup for the interface layer.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     iic.c - contains the implementation for the initialization and cleanup
|	      data abstraction.
|     iic.h - contains the external definition for the initialization and 
|	      cleanup data abstraction.
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
|     $Log:	s22_init.c,v $
 * Revision 1.9  89/10/23  20:15:32  ejh
 * declared fopen; changed the name of generated files
 * 
 * Revision 1.8  89/07/20  11:30:31  bmc
 * No longer gets user name by calling cuserid(3); now queries USER environment
 * variable instead.
 * 
 * Revision 1.7  89/06/30  14:09:57  bmc
 * Now writes Serpent environment file to a well-defined, unique name in
 * /tmp.  Combined with changes to 'serpent' script, this allows one to run a
 * dialogue in a directory that doesn't have write permission).
 * 
 * Revision 1.6  89/02/22  11:00:52  little
 * no changes
 * 
 * Revision 1.5  89/02/21  17:58:11  little
 * change some left over debug1 print to debug3
 * 
 * Revision 1.4  89/02/21  17:34:26  little
 * add exit (1) for all errors
 * 
 * Revision 1.3  89/02/15  18:16:04  little
 * add a parameter which is the dialogue name
 * 
 * Revision 1.2  88/11/02  15:16:10  rcs
 * removed extraneous debug output
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

#include <stdio.h>		/* C Standard I/O Package		    */

#include "global.h"		/* Global type defs and constants	    */
#include "isc.h"		/* status code				    */

#include "ism.h"		/* shared memory abstraction		    */
#include "iid.h"		/* id abstraction			    */
#include "ids.h"		/* data synchronization abstraction	    */
#include "ipc.h"		/* process communication abstraction	    */

#define DEBUG1(statement) SERPENT_DEBUG1(s22_init_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(s22_init_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(s22_init_debug, statement)

#define init_error -1

int s22_init_debug = 1;         /* debug variable                           */

/*--------------------------------------------------------------------------*\
|  Routine Name:  get_env_file_name
| 
|  Description:  
|     Returns the name of the file to which to write environment information.
|
|  Arguments:
|     dialog_name - The name of the dialog for which we are initializing.
|     buffer      - The buffer in which to build the file name.  WARNING:
|                   The buffer MUST be large enough for the resulting file
|                   name!  This routine does NOT check!
|
|  Return Value:
|     For convenience, this routine returns a pointer to the start of the
|     buffer.  In case of error, it returns a NULL.
\*--------------------------------------------------------------------------*/

static char *get_env_file_name (dialog_name, buffer)
char *dialog_name;
char *buffer;
{
# define USER_NAME_ENV_VAR "USER"

  char pid_buf [50];
  char *user_name;

  extern char *getenv ();
  extern int getppid ();

  /*
  ** Don't use cuserid() to get user name.  The calling shell will refer to
  ** environment variable USER to get the user name, and cuserid doesn't
  ** always return the same thing USER is set to (particularly after an
  ** su(1)).  Instead, get the user name by querying the USER environment
  ** variable.
  */
  if (((user_name = getenv (USER_NAME_ENV_VAR)) == NULL) ||
        (strlen (user_name) == 0) )
    {
    (void) fprintf (stderr,
       "s22_init: %s variable is empty or missing; can't get name of user\n",
       USER_NAME_ENV_VAR);
    buffer = NULL;
    }

  else
    {
    *buffer = '\0';
    (void) strcat (buffer, "/tmp/");
    (void) strcat (buffer, user_name);
    (void) sprintf (pid_buf, ".%d", getppid ());
    (void) strcat (buffer, pid_buf);
    }

  return (buffer);
}

/*--------------------------------------------------------------------------*\
|  Name:  Serpent Init
| 
|  Description:  
|     Peforms necessary initializations for the interface abstraction.
\*--------------------------------------------------------------------------*/
main(argc, argv, envp)
int argc;
char *argv [];
char *envp [];
{
  FILE *fopen (), *env_fp, *init_fp;
   
  isc_status_codes status;
  int rc;

  char mailbox_name[33];
  char dummy [257];
  char init_file_name [1024];
  char env_file_name [1024];
  char error_string [1128];

  ipc_mailbox mailbox;
  int semaphore_id, shared_memory_id;
  iid_counter counter;
  
  DEBUG2(fprintf(stderr, "s22_init:  called.\n"););

  if (argc != 2)
      {printf ("\n\nNAME\n");
      printf ("\ts22_init -  Initialize the Serpent interface\n");
      printf ("\nSYNTAX\n");
      printf ("\ts22_init dialogue_name\n\n\n");
      exit (1);
      }
  strcat (strcat (strcpy (init_file_name, "."), argv[1]), "i.d");
					
  DEBUG3 (fprintf (stderr, "dialogue_name = %s\n", argv[1]);
  	  fprintf (stderr, "init_file_name = %s\n", init_file_name););
/*
   Open file .dialogue_namei.d to read.
*/
  init_fp = fopen (init_file_name, "r");
  if (!init_fp)
    {fprintf (stderr, "s22_init:  error opening %s\n", init_file_name);
    exit (1);
    }
/*
   Create/open file the environment file for writing.
*/

  if (get_env_file_name (argv [1], env_file_name) == NULL)
    /* Error already reported. */
    exit (1);

  DEBUG3 (fprintf (stderr, "env_file_name = %s\n", env_file_name););

  env_fp = fopen (env_file_name, "w");
  if(!env_fp)
    {fprintf (stderr, "s22_init:  error opening %s\n", env_file_name);
    exit (1);
    }
/*
   Read mailbox names from  .dialogue_name.init file.
   Create mailboxes, assign to environment variables, and write to 
   .serpent_environment file.
*/
   while (fscanf (init_fp, "%s%s", mailbox_name, dummy) != EOF) {

    mailbox = ipc_create();
    if (get_status ())
      {fprintf (stderr, "s22_init: error %d during ipc_create.\n",
	get_status ());
      exit (1);
      }

    fprintf(env_fp, "setenv %s %d\n", mailbox_name, mailbox);
  }
/*
   Create semaphore id, assign to environment variable, and write to file.
*/
  semaphore_id = ids_create();
  if (get_status ())
     {fprintf (stderr, "s22_init:  error %d during ipc_create.\n",
       get_status ());
     exit (1);
     }

  fprintf(env_fp, "setenv SEM_ID %d\n", semaphore_id);
/*
   Create shared memory id, assign to environment variable, and write to file.
*/
  shared_memory_id = ism_create();
  if (get_status ())
      {fprintf (stderr, "s22_init:  error %d during ism_create.\n",
		get_status ());
     exit (1);
     }

  fprintf(env_fp, "setenv SHM_ID %d\n", shared_memory_id);
/*
   Create id counter, assign to environment variable, and write to file.
*/
  counter = iid_create_counter();
  if (get_status ())
     {fprintf (stderr, "s22_init:  error %d during iid_create_counter.\n",
	       get_status () );
     exit (1);
     }

  fprintf(env_fp, "setenv ID_HAND %d\n", counter);

  ism_cleanup();
  if (get_status ())
      {fprintf (stderr, "s22_init:  error %d during ism_cleanup.\n",
	get_status () );
      exit (1);
      }
/*
   Close opened files.
*/ 
  fclose(init_fp);
  fclose(env_fp);

  DEBUG2 (fprintf (stderr, "s22_init:  exiting.\n"););

  exit (0);
}
