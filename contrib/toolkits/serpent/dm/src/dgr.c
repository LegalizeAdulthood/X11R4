static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/dgr.c,v 1.8 89/08/02 15:34:57 bmc Exp $";


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
$Log:	dgr.c,v $
 * Revision 1.8  89/08/02  15:34:57  bmc
 * Fixed bug in firing line sort algorithm in function
 * dgr_process_sne_list().  Changed dgr_print_sne() to
 * display more information.
 * 
 * Revision 1.7  89/07/24  14:57:08  little
 * ops removal
 * 
 * Revision 1.6  89/05/26  10:59:00  little
 * change things to work witht he new sne structure (dpg_sne)
 * 
 * Revision 1.5  89/04/24  13:03:27  trm
 * no change. just checkin' checkin.
 * 
 * Revision 1.4  89/02/15  17:52:55  little
 * take out dgr_run_snippet routine: it is now written in the "snip"
 * c file by the compiler.  add copyright notice.
 * 
 * Revision 1.3  88/10/31  15:20:21  little
 * add some error print
 * 
 * Revision 1.2  88/10/26  11:09:31  little
 * check and see if an object exists before "running a snippet on it" id
 * dgr_run_snippet
 * 
*/

#include <stdio.h>
#include <strings.h>
#include "serpent.h"                    /* serpent externals                */
#include "isc.h"
#include "iid.h"
#include "dgr.h"
#include "d22.h"
#include "cus.h"
#include "dob.h"

/*
?? only routine needing to be recompiled for every dialogue is dgr_run_snippet
break that routine out into a seperate file and put rest into libdm.a
*/

/*--------------------------------------------------------------------------*\
|  Routine: dgr_print_sne_list
|
|  Description:
|     prints out a sne list
\*--------------------------------------------------------------------------*/
void dgr_print_sne_list (stream, sne_list, indent)
FILE *stream;
LIST sne_list;
string indent;
{  dgr_sne *sne;

   if (list_length (sne_list) > 0)
      {fprintf (stream, "%s (\n", indent);
      loop_through_list (sne_list, sne, dgr_sne *)
         {dgr_print_sne (stream, sne, indent);
         }
      fprintf (stream, "%s )\n", indent);
      }
   return;
}					/* end dgr_print_sne_list */


/*--------------------------------------------------------------------------*\
|  Routine: dgr_print_sne
|
|  Description:
|      prints a sne
\*--------------------------------------------------------------------------*/
void dgr_print_sne (stream, sne, indent)
FILE *stream;
dgr_sne *sne;
string indent;
{
   cus_sym ste_1;
   cus_sym ste_2;
   char *s_1;
   char *s_2;

   if (sne != NULL)
      {
      s_1 = NULL;
      s_2 = NULL;

      if ( (sne -> parm_1 != dob_null_object) &&
	   (sne -> parm_1 != NULL) &&
      	   (ste_1 = dob_get_symbol_entry (sne -> parm_1)) )
	s_1 = cus_sym_representation (ste_1);

      if ( (sne -> parm_2 != dob_null_object) &&
	   (sne -> parm_2 != NULL) &&
      	   (ste_2 = dob_get_symbol_entry (sne -> parm_2)) )
	s_2 = cus_sym_representation (ste_2);

      fprintf (stream,
	"%s {Routine = %d, SELF = %x (%s), Shared Data = %x (%s)}\n",
	  indent, sne-> routine,
	  sne -> parm_1, s_1 == NULL ? "No symbol table entry" : s_1,
	  sne -> parm_2, s_2 == NULL ? "No symbol table entry" : s_2);
      fflush (stream);
      }
   return;
}


/*--------------------------------------------------------------------------*\
|  Routine: dgr_process_sne_list
|
|  Description:
|     take a sne list (usually the firing line) and "run" down it
|	calling the indicated routines, taking care to not call the
|	same "routine-parent-sd combination" twice
\*--------------------------------------------------------------------------*/
void dgr_process_sne_list (sne_list)
LIST sne_list;
{  dgr_sne *sne;
   LIST local_sne_list;
   caddr_t p1, last_p1, p2, last_p2;
   dgr_snippet routine, last_routine;

   set_status (ok);
			/* first make a copy of the sne list, and null
			   it out.  snippets executed might cause elements
			   to be put on the sne list passed */
   local_sne_list = copy_list (sne_list, NOT_NODES);
   flush_list (sne_list, NOT_NODES);

			/* now we sort the list by the
			   the snippet to be called, which is the first
			   element of each of the sne's on the line
			*/
   sort_list_func (local_sne_list, dgr_line_sort_func);

   last_routine = -2;
   last_p1 = (caddr_t) NULL;
   last_p2 = (caddr_t) NULL;

   loop_through_list (local_sne_list, sne, dgr_sne *)
			/* get each sne & process it */
      {routine = sne-> routine; 
      p1 = sne-> parm_1;
      p2 = sne-> parm_2;
      if (routine != last_routine)
			/* if this is a "new" snippet, 
			   list, fire snippet and record it
			*/
         {last_routine = routine;
         last_p1 = p1;
         last_p2 = p2;
         DEBUG3 (fprintf (stderr, "dgr_process_sne_list: routine");
                 dgr_print_sne (stderr, sne, ""););
         dgr_run_snippet (sne);
         }
			/* we are still with the same snippet, check to see
			   if the snippet, parent "pair" have already been
			   fired.  if so do nothing, if not fire it, and
			   record parent
			*/
       else if (last_p1 != p1)
         {DEBUG3 (fprintf (stderr, "dgr_process_sne_list: first parameter");
                 dgr_print_sne (stderr, sne, ""););
         dgr_run_snippet (sne);
         last_p1 = p1;
         last_p2 = p2;
         }
			/* we are still with the same snippet, check to see
			   if the snippet, parent, sd "triple"have already been
			   fired.  if so do nothing, if not fire it, and
			   record sd
			*/
       else if (last_p2 != p2)
         {DEBUG3 (fprintf (stderr, "dgr_process_sne_list: second parameter");
                 dgr_print_sne (stderr, sne, ""););
         dgr_run_snippet (sne);
         last_p2 = p2;
         }
      }

			/* reset the firing line empty */
   flush_list (local_sne_list, NOT_NODES);

   DEBUG2 (fprintf (stderr, "exiting dgr_process_sne_list\n"););
   return;
}				/* end dgr_process_sne_list */


/*--------------------------------------------------------------------------*\
|  Routine: dgr_line_sort_func
|
|  Description:
|     sne list sort function
\*--------------------------------------------------------------------------*/
int dgr_line_sort_func (item_a, item_b)
dgr_sne *item_a, *item_b;
{

   if (item_a-> routine > item_b-> routine)
      return (FALSE);
   if (item_a-> routine == item_b-> routine)
      if (item_a-> parm_2 > item_b-> parm_2)
         return (FALSE);
   if (item_a-> routine == item_b-> routine)
      if (item_a-> parm_1 == item_b-> parm_1)
         if (item_a-> parm_2 > item_b-> parm_2)
         return (FALSE);
   return (TRUE);
}				/* end dgr_line_sort_func */


/*--------------------------------------------------------------------------*\
|  Routine: dgr_dummy_snippet
|
|  Description:
|     this is the name of the dummy snippet routine which the compiler
|	put as the last entry in the "generic" routine array
\*--------------------------------------------------------------------------*/
void dgr_dummy_snippet (dummy)
iid_id_type dummy;
{
   FATAL_ERROR (fprintf (stderr, "dgr_dummy_snippet called, OOPS\n"););
   return;
}


