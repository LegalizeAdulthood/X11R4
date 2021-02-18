static char rcsid [] =
        "$Header: /u/04c/mnt/integ/tools/src/RCS/runit.c,v 1.2 89/10/23 20:10:49 ejh Exp $";

#define memoryPack

#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "listPack.h"
#include "global.h"
#include "u22.h"
#include "ufa.h"
#include "isc.h"

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
$Log:	runit.c,v $
 * Revision 1.2  89/10/23  20:10:49  ejh
 * defined memoryPack; switched over to u22_convert_string and
 * u22_copy_string; changed the names of generated files; declared fopen;
 * removed convert_name
 * 
 * Revision 1.1  89/10/08  22:38:26  serpent
 * Initial revision
 * 
 * Revision 1.11  89/10/06  14:55:31  rcs
 * changed debug from 3 to 1
 * 
 * Revision 1.10  89/09/29  16:44:12  ljb
 * make work with playback and append options for serpent command
 * 
 * Revision 1.9  89/08/29  17:44:31  little
 * fix the quiting mispeel
 * add the signal (SIGINT, quit_things) when getting a q
 * SIGINT from one of the children
 * 
 * Revision 1.8  89/06/27  10:40:21  trm
 * chamg
 * fixed the wait call so won't core dump on the vax.     trm
 * 
 * Revision 1.7  89/02/22  15:28:20  serpent
 * add a wait untill all kind die to kill_kids
 * 
 * Revision 1.6  89/02/21  17:46:11  little
 * no changes
 * 
 * Revision 1.5  89/02/21  17:43:21  little
 * no changes
 * 
 * Revision 1.4  89/02/21  17:31:28  little
 * explicitly check for null coming back from getenv.  old way worked
 * on vaxen but not on suns.
 * 
 * Revision 1.3  89/02/20  13:52:23  little
 * no changes
 * 
 * Revision 1.2  89/02/20  13:44:12  little
 * no changes
 * 
 * Revision 1.1  89/02/15  17:22:04  little
 * Initial revision
 * 
*/


#define DEBUG1(statement) SERPENT_DEBUG1(x22_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(x22_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(x22_debug, statement)

#define UPPER   1
#define LOWER   2

#define PATH_ENV_VAR_NAME "SERPENT_EXE_PATH"
#define IN_BUF_SIZE 2048

static LIST kids_list;
static int x22_debug = 1;
static char *command_name;


/* ----------------------------------------------------------    get_args */
static char ** get_args (command, parameters)
string command, parameters;
{  LIST build_list;
   int next_string, i, num_args, parm_string_length;
   char **ret_val, *arg_guy [];

   build_list = make_list ();
   parm_string_length = strlen (parameters);
   
   add_to_tail (build_list, command);
   num_args = 1;
   i = 0;
   next_string = 0;

   while (i < parm_string_length)
      {while (parameters [i] != ' ' && i < parm_string_length)
         i++;
      parameters [i] = '\0';
      num_args++;
      DEBUG3 (
        fprintf (stderr, "found arg = %s\n", parameters + next_string););
      add_to_tail (build_list, parameters + next_string);
      while (parameters [i] == ' ' && i < parm_string_length)
         i++;
      next_string = i + 1;
      }

   ret_val = (char **) make_node ((num_args + 1) * sizeof (char *));
   DEBUG3 (fprintf (stderr, "num_args = %d, make_node size = %d\n",
		    num_args, (num_args + 1) * sizeof (char *)););

   for (i = 0; i < num_args; i++)
      {ret_val [i] = get_list_pos (build_list, i + 1);
      DEBUG3 (fprintf (stderr, "arg [%d] = %s\n", i, ret_val [i]););
      }
   ret_val [i] = 0;
   return (ret_val);
}					/* end get_args */


/* --------------------------------------------------------    parse_args *\
|   this routine scans the parameters and detects the beginning and end
|   of the parameters for each valid option.  Currently the valid options are
|   a and p.  These options are indicated by the keyword run-a and run-p
|   preceeding the parameters
\*-----------------------------------------------------------------------*/
void parse_args(argc, argv, start_a, end_a, start_p, end_p)
int argc;
int *start_a;
int *end_a;
int *start_p;
int *end_p;
char *argv[];
{
  int i;

  *end_a = -1;
  *end_p = -1;
  *start_a = 0;
  *start_p = 0;

  i = 1;
  while (i < argc){
    if (strcmp (argv[i],"run-a") == 0) *start_a = i + 1;
    if (strcmp (argv[i],"run-p") == 0) *start_p = i + 1;
    i++;
  }
/*
  we have detected the start of any parameters for a and p, now we need
  to calculate the end
*/
 
    *end_a = *start_p - 2;
    *end_p = argc - 1;

    DEBUG3(
     fprintf(stderr, "runit parse_args: start_a, end_a, start_p, end_p %d %d %d %d\n", 
            *start_a, *end_a, *start_p, *end_p);
          )
  return;
}

/* ----------------------------------------------------------    fire_up */
static void fire_up (command, parameters, envp)
string command, parameters;
char *envp [];
{  int pid;
   char **args;

   DEBUG2 (fprintf (stderr, "firing up %s, parameters = %s\n",
    command, parameters););

   args = get_args (command, parameters);
   DEBUG3 (fprintf (stderr, "fire_up: envr = %s\n", envp [0]););
   DEBUG3 (fprintf (stderr, "fire_up: args[0] = %s\n", args[0]););
   pid = fork (); 
   if (pid == 0)
				/* child */
      {execve (command, args, envp);
      fprintf (stderr, "problem firing up %s, errno = %d\n", command, errno);
      exit (1);
      }
    else
				/* parent */
      add_to_tail (kids_list, pid);
   return;
}					/* end fire_up */

/* ----------------------------------------------------------    kill_kids */
static void kill_kids (sig)
int sig;
{  int kid;
   union wait status;

   loop_through_list (kids_list, kid, int) kill (kid, sig);
   while (wait ( &status ) != -1);
   return;
}					/* end kill_kids */

/* ----------------------------------------------------------    quit_things */
static void quit_things ()
{ 

   (void) kill_kids (SIGINT);
   exit (0);
}					/* end quit_things */

/* -------------------------------------------------    abnormal_terminate */
static void abnormal_terminate ()
{

   (void) kill_kids (SIGTERM);
   printf ("\nabnormal termination of %s\n", command_name);
   exit (1);
}					/* end abnormal_terminate */

/* ----------------------------------------------------------    main */
main(argc, argv, envp)
int argc;
char *argv[];
char *envp [];
{  FILE *fopen (), *exec_f;
   string env_test, env_var, full_command_path, parameters, search_paths;
   LIST env_var_list, full_command_path_list, parameters_list;
   char in_buf [IN_BUF_SIZE];
   char command [256];
   boolean allus_ok;
   int kid, cnt, i, arg;
   int start_a, end_a, start_p, end_p;
   char exec_fn [1024];
   union wait status;
   char *position;

   DEBUG2(fprintf(stderr, "runit: version 1.0 called.\n");
     for (i = 0; i < argc; i++) {
       fprintf(stderr, "runit: argv[%d] = %s.\n", i, argv[i]);
     });

   parse_args(argc, argv, &start_a, &end_a, &start_p, &end_p);

   command_name = argv [0];

   DEBUG3 (fprintf (stderr, "runit: getenv = %x\n", getenv("SHM_ID")););
   DEBUG3 (fprintf (stderr, "runit: envr = %s\n", envp[0]););
   DEBUG3 (fprintf (stderr, "exec_fn = %s\n", exec_fn););

/*
   Open file containing listing taps to execute.
*/
   strcat(strcat(strcpy(exec_fn, "."), argv[1]), "e.d");

   exec_f = fopen(exec_fn, "r");
   if (!exec_f)
      {fprintf(stderr, "exec data file named %s does not exist\n", exec_fn);
      exit(1);
      }

   if (!getenv (PATH_ENV_VAR_NAME)) {
      fprintf(stderr, "%s environment var not present\n", PATH_ENV_VAR_NAME);
      exit(1);
   }

   search_paths = u22_copy_string (getenv (PATH_ENV_VAR_NAME));

   if (!strlen (search_paths))
      {search_paths [0] = '.';
      search_paths [1] = '\0';
      }

   DEBUG3 (fprintf (stderr, "search_paths = %s\n", search_paths););

   env_var_list = make_list ();
   full_command_path_list = make_list ();
   parameters_list = make_list ();
   kids_list = make_list ();

   env_var = make_node (256);
   full_command_path = make_node (1024);
   parameters = make_node (1024);
/*
   Set up signals.
*/
   signal(SIGINT, quit_things);
   signal(SIGTERM, abnormal_terminate);
   signal(SIGHUP, abnormal_terminate);
   signal(SIGQUIT, abnormal_terminate);
/*
   Read taps from "e.d" file.
*/
   allus_ok = TRUE;

   while (fgets (in_buf, IN_BUF_SIZE, exec_f) != NULL) {

     DEBUG3(fprintf(stderr, "input line = %s\n", in_buf););
/*
   Get env_var and command from the input string and then copy 
   the rest of the line as parameters.
*/
     sscanf(in_buf, "%s%s", env_var, command);
     strcpy(parameters, in_buf + strlen(env_var) + strlen(command) + 2);
/*
   Making sure to kill the end of line character.
*/
     position = strchr(parameters, '\n');
     if (position != NULL)
       *position = '\0';
     else {
       DEBUG3(fprintf(stderr, "runit: no end of line character.\n"););
     }
/*
    we now check whether the current command is to be replaced by the
    playback command.  
*/
    if (end_p > start_p && strcmp(command,argv[start_p]) == 0 &&
       strcmp(command,argv[1]) != 0){
              /* we have playback replacement of tap*/
      strcpy(command,"playback");
      strcpy(parameters, argv[end_p]);
      strcat(parameters, " ");
      strcat(parameters, u22_convert_string (argv[start_p],UPPERCASE));
      strcat(parameters,"_BOX");
      strcpy(env_var, "\0");
    }

    if (end_p > start_p && strcmp(command,argv[1]) == 0 &&
       (strcmp(command,argv[start_p]) == 0 ||
          strcmp("dm", u22_convert_string (argv[start_p],LOWERCASE)) == 0)){
              /* we have playback replacement of dm*/
      strcpy(command,"playback");
      strcpy(parameters, argv[end_p]);
      strcat(parameters," DM_BOX");
      strcpy(env_var, "\0");
	  }

     DEBUG3(fprintf(stderr, "env_var = %s, command = %s, parameters = %s\n",
         env_var, command, parameters););

/*----------- Tap parameters -----------------------------------------------*/

/*
   Loop through all the parms and see if any of them match to this specific
   tap.
*/
     arg = start_a;
  
     while (arg <= end_a) {

       DEBUG3(fprintf(stderr, "tap parameters specified.\n");
         fprintf(stderr, "runit: tap = %s.\n",
           u22_convert_string (argv[arg], UPPERCASE)););
/*
   If all characters of the env_var except the _EXE match the upper case
   version of the name of the tap passed in from the the serpent
   command concat the parameters to the end of the argv
*/
       if (strncmp(env_var, u22_convert_string (argv[arg], UPPERCASE),
	strlen(env_var) - 4) == 0) {
/*
   Replace all the control-a characters with blanks.
*/
         while ((position = strchr(argv[arg+1], '')) != NULL)
           *position = ' ';

         strcat(parameters, argv[arg+1]);

         DEBUG3(fprintf(stderr,
	   "parameters after strcat = %s.\n", parameters););

       }			/* end if matched tap name		*/ 

       arg = arg + 2;		/* increment over both parameters	*/

     }				/* end while more parms			*/
/*
   Test to see if command is overwritten by environment variable.
*/
      if (getenv (env_var)) {
         env_test = u22_copy_string (getenv (env_var));
         DEBUG3 (fprintf (stderr,
	    "command = %s overridden by environment var named %s,",
	    command, env_var);
            fprintf (stderr, " with value of %s\n", env_test););
         strcpy (command, env_test);
      }

      if (full_command_path = ufa_full_file_name (search_paths, command)) {
         DEBUG3(fprintf (stderr,
	   "full_command_path = %s\n", full_command_path););
         add_to_tail (env_var_list, env_var);
         add_to_tail (full_command_path_list, full_command_path);
         add_to_tail (parameters_list, parameters);
         env_var = make_node (256);
         full_command_path = make_node (1024);
         parameters = make_node (1024);
       }
       else {
         fprintf (stderr, "cannot find command named %s.\n", command);
         allus_ok = FALSE;
       }
   }			/* end while more lines in "e.d" file */

   if (!allus_ok)
      {fprintf (stderr, "\n\tcould not find all commands, aborting\n\n");
      exit (1);
      }

   DEBUG3(fprintf(stderr,
	"length = %d\n", list_length (full_command_path_list)););

   for (cnt = list_length (full_command_path_list); cnt > 0; cnt--)
      {fire_up (get_list_pos (full_command_path_list, cnt),
         get_list_pos (parameters_list, cnt), envp);
      }

   DEBUG3 (loop_through_list (kids_list, kid, int) {
       fprintf (stderr, "kid = %d\n", kid);});
/*
   Wait here for taps to return.
*/
   while (wait ( &status ) != -1);

   exit(0);
}					/* end main */
