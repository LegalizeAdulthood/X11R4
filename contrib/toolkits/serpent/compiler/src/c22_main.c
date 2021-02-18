static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/c22_main.c,v 1.52 89/10/23 19:55:21 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:	c22_main.c
|
|  Description:
|	This is the main program for SLANG.
|
|  Component of:
|	SLANG processor
|
|  Related files:
|	many yacc, lex, and c files
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Author:  
|	Erik J. Hardy
|	ejh@sei.cmu.edu
|
|  Change History:
|	01 Mar 1988:	module created
$Log:	c22_main.c,v $
 * Revision 1.52  89/10/23  19:55:21  ejh
 * took out ops references; declared fopen; switched over to u22_copy_string;
 * new version number
 * 
 * Revision 1.51  89/09/20  11:15:36  little
 * new version number and a new way of using c22_pp_filename which is used
 * for printing file name containing parse errors
 * 
 * Revision 1.50  89/09/01  11:03:52  little
 * bump version number
 * 
 * Revision 1.49  89/08/29  17:29:22  little
 * new vwersio
 * new version number
 * 
 * Revision 1.48  89/07/31  14:31:07  ejh
 * new version number
 * 
 * Revision 1.47  89/07/24  14:51:58  little
 * new version number
 * 
 * Revision 1.46  89/06/14  10:55:40  little
 * change version number
 * 
 * Revision 1.45  89/05/26  10:48:39  little
 * change version number
 * /
 * 
 * Revision 1.44  89/05/19  10:42:51  little
 * change version number
 * 
 * Revision 1.43  89/04/29  09:49:52  little
 * change version number
 * 
 * Revision 1.42  89/04/27  11:18:20  little
 * *** empty log message ***
 * 
 * Revision 1.41  89/04/14  11:27:49  little
 * new version number
 * 
 * Revision 1.40  89/04/12  18:59:39  little
 * new vewrsion number
 * 
 * Revision 1.39  89/04/07  16:13:25  little
 * change versino number
 * 
 * Revision 1.38  89/03/15  17:28:50  little
 * change version number
 * 
 * Revision 1.37  89/03/08  12:02:50  little
 * new version number
 * 
 * Revision 1.36  89/03/07  15:52:04  rcs
 * new version number
 * 
 * Revision 1.35  89/02/28  18:28:23  little
 * new version number
 * 
 * Revision 1.34  89/02/27  17:01:51  little
 * change version number
 * 
 * Revision 1.33  89/02/15  17:34:30  little
 * make the dialogue name a parameter in a lot of places
 * 
 * Revision 1.32  89/02/03  15:58:08  little
 * new version number
 * 
 * Revision 1.31  89/02/01  16:19:34  little
 * new vewrsion number
 * 
 * Revision 1.30  89/01/30  15:14:22  little
 * change version number
 * 
 * Revision 1.29  89/01/26  17:31:39  little
 * new version number
 * 
 * Revision 1.28  89/01/18  14:24:00  ejh
 * new version number
 * 
 * Revision 1.27  89/01/17  16:53:51  little
 * change version number
 * 
 * Revision 1.26  89/01/12  13:26:38  little
 * no changes
 * 
 * Revision 1.25  89/01/12  11:32:20  little
 * no changes
 * 
 * Revision 1.24  89/01/11  19:00:18  little
 * change version number
 * 
 * Revision 1.23  89/01/11  14:04:43  little
 * new version number
 * 
 * Revision 1.22  89/01/09  16:27:46  little
 * change version number
 * 
 * Revision 1.21  89/01/06  16:53:28  little
 * no change
 * 
 * Revision 1.20  89/01/06  14:36:28  little
 * no changes
 * 
 * Revision 1.19  89/01/06  14:30:19  little
 * no changes
 * 
 * Revision 1.18  89/01/06  14:27:56  little
 * no changes
 * 
 * Revision 1.17  89/01/06  14:24:39  little
 * no changes
 * 
 * Revision 1.16  89/01/06  14:17:34  little
 * no changes
 * 
 * Revision 1.15  89/01/04  14:41:05  ejh
 * new version number
 * 
 * Revision 1.14  88/12/21  16:57:20  ejh
 * new version number
 * 
 * Revision 1.13  88/12/20  15:34:29  ejh
 * new version number
 * 
 * Revision 1.12  88/12/15  13:56:55  little
 * change version number
 * 
 * Revision 1.11  88/12/14  10:39:37  little
 * change version number
 * 
 * Revision 1.10  88/12/04  14:02:14  little
 * change version number
 * 
 * Revision 1.9  88/11/03  18:48:58  little
 * change version number
 * 
 * Revision 1.8  88/10/31  15:02:02  little
 * change version number
 * 
 * Revision 1.7  88/10/24  17:01:34  little
 * change version number
 * 
 * Revision 1.6  88/10/11  11:40:52  little
 * change version number
 * 
 * Revision 1.5  88/10/04  17:32:01  little
 * change version number
 * 
 * Revision 1.4  88/10/03  08:57:03  little
 * change the version number
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

#define	c22_top
#define	memoryPack

#include	<stdio.h>	/* for I/O */
#include	<errno.h>	/* for error reporting */
#include	"string.h"	/* for handling strings */
#include	<memoryPack.h>	/* to pick up make_node */

/*	SLANG specific includes */

#include	"c22.h"	/* SLANG globals */
#include	"u22.h"

#define c22_VERSION     2.14

/*--------------------------------------------------------------------------*\
|  Routine: c22_print_usage
|
|  Description:
|     output the help.man infor for the compiler
\*--------------------------------------------------------------------------*/
static void c22_print_usage()
{
   printf ("NAME\n\tslang_compiler - compile a slang program\n\n");
   printf ("SYNTAX\n\tslang_compiler [options] pre_processed_file dialogue_name\n\n");
   printf ("DESCRIPTION\n");
   printf ("\tThe slang_compiler command compiles a Slang program into\n");
   printf ("\tC code.\n");
   printf ("\tThe pre_processed_file is usually the result of passing a\n");
   printf ("\t'raw' Slang file through the C preprocessor (/lib/cpp). The\n");
   printf ("\tdialogue name parameter is used to generated the output\n");
   printf ("\tfile names. It is usually the name of the file input to the\n");
   printf ("\tpreprocessor, w/o the extension. All errors go to either\n");
   printf ("\tstdout or stderr and may be redirected at will\n\n");
   printf ("OPTIONS\n");
   printf ("\t-dn\tsets debug level to n. useful values are 0,1,2,3\n");
   printf ("\t\tdefault is 1\n\n");
   printf ("\t-c\tput debugging statements in output C code\n");
   printf ("\t\tdefault is to NOT put the debugging statements into the code\n\n");
   printf ("\t-t\toutput syntax trees to stdout\n");
   printf ("\t\tdefault is to NOT output syntax trees\n\n");
   printf ("EXAMPLE\n\tslang_compiler -c -d2 pre-processed_file spider\n");
   printf ("\n\n");
   return;
}					/* end c22_print_usage */

/*--------------------------------------------------------------------------*\
|  Routine: main
|
|  Description:
|     slang compiler main program
\*--------------------------------------------------------------------------*/
main	(argc, argv)
int argc;
string argv [];

{  string file_name;
   FILE	*fopen (), *file_ptr;
   int debug_level, output_debug_code, print_trees;
   char *p;
	/* tell the world we're here */
   fprintf (stdout, "Welcome to SLANG, Version %.2f\n\n", c22_VERSION);

   if (argc < 3)
      {c22_print_usage ();
      exit (1);
      }
	/* process the parameters */
   argv++;
   argc--;

   output_debug_code = 0;
   debug_level = 0;
   print_trees = 0;
   if (argc > 0)
      {p = *argv;
      if (*p == '-')
         {while ((argc > 0) && (*p++ == '-'))
            {switch (*p)
               {case 'd':
                   debug_level = *++p - '0';
                   break;
                case 'c':
                   output_debug_code = 1;
                   break;
                case 't':
                   print_trees = 1;
                   break;
                default:
                   printf ("slang_compiler: bad flag -%c\n\n", *p);
                   c22_print_usage ();
                   exit (1);
               }
            argc--;
            argv++;
            p = *argv;
            }
         }
      }

   DEBUG3 (fprintf (stderr,
	"debug level = %d, output debug code = %d, print trees = %d\n",
	debug_level, output_debug_code, print_trees););

   if (argc < 1)
      {fprintf (stderr, "input file name not provided.\n");
      exit (1);
      }
	/* open the file */
   file_name = u22_copy_string (*argv);
   c22_pp_filename = NULL;
   file_ptr = fopen (file_name, "r");
   if (file_ptr == NULL)
      {c22_error (OPEN_FILE, file_name);
      exit (1);
      }
   argv++;
   argc--;

   if (argc < 1)
      {fprintf (stderr, "dialogue name not provided.\n");
      exit (1);
      }
   fprintf (stdout, "\n\tCompiling dialogue named %s\n\n", *argv);
	/* compile! */
   exit (c22_compile (file_ptr, debug_level, output_debug_code, print_trees,
      *argv) ? 0 : 1);

}					/* end main */
