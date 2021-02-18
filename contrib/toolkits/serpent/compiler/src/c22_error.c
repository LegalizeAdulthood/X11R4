   static char rcsid [] = "$Header: /u/04c/mnt/integ/compiler/src/RCS/c22_error.c,v 1.6 89/10/23 19:56:30 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  c22_error.c
|
|  Description:
|     This routine is the error routine for SLANG.
|
|  Component of:
|     SLANG processor
|
|  Related files:
|     many yacc, lex, and c files
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
$Log:	c22_error.c,v $
 * Revision 1.6  89/10/23  19:56:30  ejh
 * declared fopen; defined memoryPack
 * 
 * Revision 1.5  89/09/20  11:14:02  little
 * add c22_file_print and c22_yyerror
 * change some of the printout in c22_error
 * 
 * Revision 1.4  89/02/24  13:45:04  ejh
 * added reference to current file being processed (for #includes)
 * 
 * Revision 1.3  89/02/17  17:44:48  serpent
 * stop further processing if parser picked up a fatal error
 * 
 * Revision 1.2  89/02/09  16:20:33  ejh
 * added copyright notice
 * 
|     Erik J. Hardy
|     ejh@sei.cmu.edu
|
|  Change History:
|     14 Mar 1988:	module created
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

#define	memoryPack

#include	"global.h"	/* global definitions */
#include	<stdio.h>	/* for I/O */
#include	<errno.h>	/* for error reporting */
#include	"string.h"	/* for handling strings */
#include	"listPack.h"	/* list handling stuff */

/*	SLANG specific includes */

#include	"cus.h"		/* SLANG symbol table stuff */
#include	"c22.h"		/* SLANG globals */

#define BUFF_SIZE 1024

/*--------------------------------------------------------------------------*\
|  Routine: c22_file_print
|
|  Description:
|     prints out requested lines from requested file
\*--------------------------------------------------------------------------*/
void c22_file_print (file_name, start, stop)
char *file_name;
int start;
int stop;
{  FILE *fopen (), *stream;
   int cur_line_num;
   char buff [BUFF_SIZE];

   stream = fopen (file_name, "r");
   if (stream)
      {if (start < 1)
         start = 1;
      cur_line_num = 0;
      while (cur_line_num < start)
         {fgets (buff, BUFF_SIZE, stream);
         if feof (stream)
            {cur_line_num = stop;
            break;
            }
          else
            cur_line_num++;
         }
      fprintf (stderr, "\n");
      while (cur_line_num < stop)
         {fprintf (stderr, "%d: %s", cur_line_num, buff);
         fgets (buff, BUFF_SIZE, stream);
         if feof (stream)
            cur_line_num = stop;
          else
            cur_line_num++;
         }
      fclose (stream);
      }
   return;
}					/* end c22_file_print */

/*--------------------------------------------------------------------------*\
|  Routine: yyerror
|
|  Description:
|     parser error routine called by yacc
\*--------------------------------------------------------------------------*/
void c22_yyerror (error_msg, line_num)
string error_msg;
int line_num;
{  int start, stop;

   c22_error (PARSING, error_msg, line_num);
   DEBUG3 (fprintf (stderr, "input file name = %s, line_num = %d\n",
	c22_pp_filename, line_num););
   if (c22_pp_filename)
      {start = line_num - 4;
      if (start < 1)
         start = 1;
      stop = start + 9;
      c22_file_print (c22_pp_filename, start, stop);
      }
   return;
}					/* end c22_yyerror */

/*--------------------------------------------------------------------------*\
|  Routine: c22_error
|
|  Description:
|     yet another parser error routine
\*--------------------------------------------------------------------------*/
void c22_error (the_error, the_string, the_lineno)
c22_error_type	the_error;
string		the_string;
int		the_lineno;

{
    fprintf (stderr, "ERROR:  %s\n\t%s\n",
	c22_errors_print_value [(int) the_error], the_string);
    if (c22_pp_filename)
      {fprintf (stderr, "\tat or near line # %d\n", the_lineno);
      fprintf (stderr, "\tin file %s\n", c22_pp_filename);
      }
/*	the assumption is that all c22_errors are fatal */
    c22_continue = FALSE;
    return;
}					/* end c22_error */
