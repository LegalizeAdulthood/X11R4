static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/s22_clean.c,v 1.2 89/10/23 20:16:44 ejh Exp $";
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
|     $Log:	s22_clean.c,v $
 * Revision 1.2  89/10/23  20:16:44  ejh
 * declared fopen; changed the names of generated files
 * 
 * Revision 1.1  89/10/08  00:34:02  serpent
 * Initial revision
 * 
 * Revision 1.5  89/02/22  11:02:41  little
 * change some extreanous debug1 to debug3
 * 
 * Revision 1.4  89/02/21  17:33:51  little
 * add exit (1) for all errors
 * 
 * Revision 1.3  89/02/15  18:15:23  little
 * add a parameter which is the dialogue name
 * 
 * Revision 1.2  88/11/02  15:17:13  rcs
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
#include "isd.h"		/* shared data abstraction	 	    */

#define DEBUG1(statement) SERPENT_DEBUG1(s22_cleanup_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(s22_cleanup_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(s22_cleanup_debug, statement)

#define cleanup_error -1

int s22_cleanup_debug = 1;         /* debug variable                        */

/*--------------------------------------------------------------------------*\
|  Name:  Serpent Cleanup
| 
|  Description:  
|     Peforms necessary cleanup for the Serpent system.
\*--------------------------------------------------------------------------*/
main(argc, argv, envp)
int argc;
char *argv [];
char *envp [];
{
  FILE *fopen (), *env_fp, *init_fp;
  char file_name [1024];
  char error_string [1128];
   
  isc_status_codes status;
  int rc;

  char mailbox_name[33];
  char dummy [257];

  ipc_mailbox mailbox;
  int semaphore_id, shared_memory_id;

  DEBUG2(fprintf(stderr, "s22_cleanup:  called.\n"););
/*
   terminate the interface in order to determine system resource ids.
*/
  if (argc != 2)
      {printf ("\n\nNAME\n");
      printf ("\ts22_cleanup - Terminate and cleanup the Serpent interface\n");
      printf ("\nSYNTAX\n");
      printf ("\ts22_cleanup dialogue_name\n\n\n");
      exit (1);
      }
  strcat (strcat (strcpy (file_name, "."), argv[1]), "i.d", "r");
  DEBUG3 (fprintf (stderr, "dialogue_name = %s\n", argv[1]);
  	  fprintf (stderr, "file_name = %s\n", file_name););

  isd_init_once();
  if (get_status ())
     {fprintf (stderr, "s22_cleanup: bad status from isd_init_once.\n");
     exit (1);
     }

/*
   Open file to read.
*/
  init_fp = fopen (file_name, "r");
  if (!init_fp)
      {fprintf (stderr, "s22_cleanup:  error opening %s\n", file_name);
      exit (1);
      }
/*
   Read mailbox names from file.  Get mailbox ids
   from environment variables and destroy each mailbox.
*/
  while (fscanf (init_fp, "%s%s", mailbox_name, dummy) != EOF) {

    mailbox = (ipc_mailbox)atoi(getenv(mailbox_name));

    ipc_destroy(mailbox);
    if (get_status ())
       {fprintf (stderr, "s22_cleanup: bad status from ipc_destroy.\n");
       exit (1);
       }
  }
/*
   Destroy ID counter in shared memory.
*/
  iid_destroy_counter();
  if (get_status ())
     {fprintf (stderr, "s22_cleanup: bad status from iid_destroy_counter.\n");
     exit (1);
     }
/*
   Destroy shared memory ID.
*/
  ism_destroy();
  if (get_status ())
     {fprintf (stderr, "s22_cleanup: bad status from ism_destroy.\n");
     exit (1);
     }
/*
   Destroy semaphore ID.
*/
  ids_destroy();
  if (get_status ())
     {fprintf (stderr, "s22_cleanup: bad status from ids_destroy.\n");
     exit (1);
     }
/*
   Close opened file.
*/ 
  fclose(init_fp);

  DEBUG2(fprintf(stderr, "s22_cleanup:  exiting.\n"););
  exit (0);
}
