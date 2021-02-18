/* $Header: /u/04c/mnt/integ/dm/include/RCS/d22.h,v 1.12 89/11/02 16:25:45 little Exp $ */


/*
$Log:	d22.h,v $
 * Revision 1.12  89/11/02  16:25:45  little
 * put the debug macros un NDEBUG control
 * 
 * Revision 1.11  89/10/02  13:31:04  rcs
 * removed stupid include of serpent.h
 * 
 * Revision 1.10  89/09/20  12:08:18  little
 * change runtime error printout to better highlight the real error with a
 * shit load of *s
 * 
 * Revision 1.9  89/08/29  17:43:26  little
 * change definition of d22_slang_source so the addition rt error print
 * works with the new way the compiler is generating the string
 * >> .
 * change god to eve
 * 
 * Revision 1.8  89/06/09  13:39:49  little
 * add definition of d22_monitor
 * 
 * Revision 1.7  89/05/26  10:53:18  little
 * changes definition of d22_sne from a list to be a pointer to a dgr_sne.
 * 
 * Revision 1.6  89/02/09  16:38:53  ejh
 * added copyright notice
 * 
 * Revision 1.5  89/01/04  15:40:56  little
 * add printing of d22_slang_source to FATAL_ERROR
 * 
 * Revision 1.4  88/11/04  16:59:24  little
 * change DEBUG1 to also print global d22_slang_source which will contain
 * vc name, object, att name, var name, etc.
 * 
 * Revision 1.3  88/10/27  18:47:44  little
 * add definition for d22_temp_free_list
 * 
 * Revision 1.2  88/10/12  15:58:03  little
 * add d22_recording_on flag
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

#ifndef d22
#define d22

#include "listPack.h"
#include "cus.h"

/*
	?? need to coordinate with the definition of god text in c22.h
*/
#define d22_god_text "EVE"


/*
	?? need to coordinate this number within dm and across to compiler
*/
#define MAX_FILE_NAME_LENGTH 257


#ifdef d22_top
#define EXTERN
caddr_t d22_slang_source;
#else
#define EXTERN extern
extern caddr_t d22_slang_source;
#endif




EXTERN int d22_debug;
EXTERN LIST d22_firing_line;
EXTERN dgr_sne *d22_sne;
EXTERN int d22_slang_line_num;
EXTERN cus_sym d22_god_vc;
EXTERN boolean d22_xactions_open;
EXTERN boolean d22_recording_on;
EXTERN caddr_t d22_temp_free_list;
EXTERN boolean d22_monitor;

#define FATAL_ERROR(statement)\
   {  long pid;\
      char command [256];\
   fprintf (stderr, "\n\n***********************************************\n"); \
   fprintf (stderr, "!!!!!     DM FATAL ERROR, at or near SLANG line %d\n",\
   	d22_slang_line_num);\
   fprintf (stderr, "\t  in %s\n", d22_slang_source);\
   fprintf (stderr, "	file %s, line %d\n", __FILE__, __LINE__);\
   statement;\
   fprintf (stderr, "\n***********************************************\n\n"); \
   fflush (stderr);\
   pid = getpid ();\
   sprintf (command, "gcore %d", pid);\
   system (command);\
   sprintf (command, "mv core.%d core", pid);\
   system (command);\
   exit (1);\
   }


#ifndef NDEBUG
#define DEBUG1(statement)\
   if (d22_debug > 0)\
      {\
        statement;\
	fprintf (stderr, "\tfile %s, line %d, ", __FILE__, __LINE__);\
        fprintf (stderr, "at or near SLANG line %d\n", d22_slang_line_num);\
        fprintf (stderr, "\t  in %s\n", d22_slang_source);\
        fflush (stderr);\
      }

#define DEBUG2(statement)\
   if (d22_debug > 1)\
      {\
        statement;\
	fprintf (stderr, "	file %s, line %d\n", __FILE__, __LINE__);\
        fflush (stderr);\
      }

#define DEBUG3(statement)\
   if (d22_debug > 2)\
      {\
        statement;\
	fprintf (stderr, "	file %s, line %d\n", __FILE__, __LINE__);\
        fflush (stderr);\
      }
#else
#define DEBUG1(statement)
#define DEBUG2(statement)
#define DEBUG3(statement)
#endif

#endif	/* ifndef d22 */


