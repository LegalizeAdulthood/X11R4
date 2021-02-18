static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/ca2_ill.c,v 1.10 89/09/20 11:57:46 little Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  ca2_ill.c
|
|  Description:
|	This routine reads an ill file.
|       and a function file
|
|  Component of:
|	SLANG processor
|
|  Related files:
|	many yacc and lex files
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
$Log:	ca2_ill.c,v $
 * Revision 1.10  89/09/20  11:57:46  little
 * change the way the ill prologue information is read so we can get the
 * file name fromthe # lines produced by the c preprocessor.
 * 
 * Revision 1.9  89/08/27  12:25:00  ejh
 * added handling of missing '|||'; fixed error reporting for errors in
 * prologue (SPR 100)
 * 
 * Revision 1.8  89/07/31  14:31:42  ejh
 * removed the code to do function defs (moved it to cal.l and cay.y)
 * 
 * Revision 1.7  89/04/28  14:44:36  ljb
 * make type for functions which do not return an argument be "void"
 * 
 * Revision 1.6  89/04/28  14:32:25  ljb
 * user defined functions have been debugged
 * 
 * Revision 1.5  89/04/25  09:12:21  ljb
 * add code to read in user defined functions 
 * (this code hasn't been tested yet, I am checking this in so
 * that another bug can be fixed
 * 
 * Revision 1.4  89/02/16  23:40:34  ejh
 * added copyright notice
 * 
 * Revision 1.3  89/01/18  14:22:40  ejh
 * changed reader to read the new ill file preamble and set up the properties
 * 
 * Revision 1.2  88/11/03  18:44:18  little
 * have ca2_get_sd_sym use the new routine idd_validate_element instead of the
 * hokey way of using idd_get_length
 * 
|	09 May 1988:	module created
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

#define BARS            "|||"
#define LEFT_ILL        "<<"
#define POUND_SIGN      '#'
#define	memoryPack

#include	"global.h"	/* global definitions */
#include	<stdio.h>	/* for I/O */
#include	<errno.h>	/* for error reporting */
#include	"string.h"	/* for handling strings */
#include	"memoryPack.h"	/* to pick up make_node */
#include	"isd.h"		/* all of the idd stuff */
#include	"isc.h"

   /*	SLANG specific includes */

#include	"c22.h"		/* SLANG globals */
#include	"ca2.h"

#define BUFF_SIZE 1024
#define get_next_line(msg) \
   if (!fgets (buff, BUFF_SIZE, file_ptr)) \
					/* need another error */ \
      {c22_error (ILL_SYNTAX, msg, yylineno); \
      return; \
      }


static char ca2_file_name [BUFF_SIZE];
extern int yylineno;


/* --------------------------------------------------  ca2_read_prefix_info */
void ca2_read_prefix_info (file_ptr)
FILE	*file_ptr;	/* pointer to a currently open ill file */

{  char buff [BUFF_SIZE];
   char *next, *replace;

				/* read the ill "files"; if we find one, read
				   the prolog into the structure, read in the
				   data and get the handle of the data
				   definition and then put all the information
				   away for future use
				*/
   get_next_line ("0: cannot read file prefix");
   while (strncmp (buff, BARS, strlen (BARS)))
      {next = buff;
      if (!strncmp (buff, LEFT_ILL, strlen (LEFT_ILL)))
         {if (!ca2_read_ill_entry (file_ptr))
            {c22_error (ILL_SYNTAX, c22_NONAME, yylineno);
            return;
            }
         get_next_line ("1: cannot read file prefix");
         continue;
	 }
       else 
         {if (buff [0] == POUND_SIGN)
            {DEBUG3 (fprintf (stderr, "ca2_read_prefix_info found a \#\n"););
            DEBUG3 (fprintf (stderr,
     "ca2_read_prefix_info: before - yylineno = %d, c22_pp_filename = %s\n",
                yylineno, c22_pp_filename););
            next = buff + 1;
            sscanf (next, "%d%s", &yylineno, ca2_file_name);
            c22_pp_filename = ca2_file_name + 1;
            replace = (char *) strchr (c22_pp_filename, '"');
            *replace = '\0';
            DEBUG3 (fprintf (stderr,
      "ca2_read_prefix_info: after - yylineno = %d, c22_pp_filename = %s\n",
                yylineno, c22_pp_filename););
            }
         get_next_line ("2: cannot read file prefix");
         yylineno++;
         }
      }
   yylineno++;
   return;
}					/* end ca2_read_prefix_info */


/* --------------------------------------------  ca2_fix_ill_string */
void ca2_fix_ill_string (the_string)
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
}


/* --------------------------------------------  ca2_read_ill_entry */
boolean	ca2_read_ill_entry	(
   file_ptr
   )

   FILE	*file_ptr;	/* pointer to a currently open ill file */

   {

   char	temp [80];

   int	new;

   string	generic_magic_id,
		specific_magic_id,
		tap_id,
		ill_file_name,
		mailbox_name,
		execution_name,
		exec_command,
		handle;

   cus_sym	the_sym;

   DEBUG2 (fprintf (stderr, "entering ca2_read_ill_entry\n"));

/*	skip the first line (comment), then read the preamble */


/*
	?? need to detect duplicate mailbox & ill file names
*/

   generic_magic_id = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (generic_magic_id);
   DEBUG3 (fprintf (stderr, "generic magic id = %s\n", generic_magic_id));
   specific_magic_id = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (specific_magic_id);
   DEBUG3 (fprintf (stderr, "specific magic id = %s\n", specific_magic_id));
   tap_id = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (tap_id);
   DEBUG3 (fprintf (stderr, "tap id = %s\n", tap_id));
   ill_file_name = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (ill_file_name);
   DEBUG3 (fprintf (stderr, "ill file name = %s\n", ill_file_name));
   mailbox_name = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (mailbox_name);
   DEBUG3 (fprintf (stderr, "mailbox name = %s\n", mailbox_name));
   execution_name = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (execution_name);
   DEBUG3 (fprintf (stderr, "execution name = %s\n", execution_name));
   exec_command = strcpy (make_node (80), fgets (temp, 80, file_ptr));
   ca2_fix_ill_string (exec_command);
   DEBUG3 (fprintf (stderr, "exec command = %s\n", exec_command));

/*	skip the next line ('|||') */

   (void) fgets (temp, 80, file_ptr);

/*	now we're positioned at the ill records, so read them in, but first
   get a handle for them */

   handle = idd_create_data_definition ();
   isd_input_open_ill_file (handle, file_ptr);

/*	store everything in the symbol table */

/*	??
we need to wory about "uniquifying" the names of these, the
short ill names are unique anmoung themselves, but not necessarily
across other things in the no context context
*/
   the_sym = cus_add_sym (tap_id, ill_sym, &new, NO_CONTEXT);
   cus_put_prop (the_sym, generic_magic_id_prop, generic_magic_id);
   cus_put_prop (the_sym, specific_magic_id_prop, specific_magic_id);
   cus_put_prop (the_sym, ill_file_name_prop, ill_file_name);
   cus_put_prop (the_sym, mailbox_name_prop, mailbox_name);
   cus_put_prop (the_sym, execution_name_prop, execution_name);
   cus_put_prop (the_sym, exec_command_prop, exec_command);
   cus_put_prop (the_sym, idd_handle_prop, handle);

/*	add the symbol to the ill name list */

   (void)add_to_tail (cus_get_prop (car_god_vc, ill_files_prop), the_sym);

   DEBUG2 (fprintf (stderr, "leaving ca2_read_ill_entry\n\n"));

/*	return status from the isd routine */

   return (TRUE);

   }	


/* --------------------------------------------  ca2_get_sd_sym */
cus_sym ca2_get_sd_sym (name)
string name;
{  LIST ill_file_list;
   cus_sym look;
   int the_length;
   boolean found_it;

   ill_file_list = (LIST) cus_get_prop (car_god_vc, ill_files_prop);
   look = (cus_sym) get_list_next (ill_file_list, NULL);
   found_it = FALSE;
   while (look != NULL && !found_it)
     {if (!(found_it = idd_validate_element ((idd_data_definition)
				     cus_get_prop (look, idd_handle_prop),
				     name)))
        look = (cus_sym) get_list_next (ill_file_list, look);
     }
   if (look == NULL || !found_it)
     return (NULL);
   return (look);
}
