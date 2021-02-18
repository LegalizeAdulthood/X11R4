static char rcsid [] =
	"$Header: /u/04c/mnt/new_integ/tools/src/RCS/format.c,v 1.1 89/10/09 11:09:27 serpent Exp $";

/*--------------------------------------------------------------------------*\
|   
|  Name:  journaling file dumper
|
|  Description:  
|     formats journaling file.
|     
|
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
|     Len Bass
|     ljb@sei.cmu.edu  
|
|  Change History:
|     April 8, 1988 : Module created.
$Log:	format.c,v $
 * Revision 1.1  89/10/09  11:09:27  serpent
 * Initial revision
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

/*-------------------- Includes --------------------------------------------*/

#include <stdio.h>
/*-------------------- Constants -------------------------------------------*/


/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- Retained Data ---------------------------------------*/

main(argc,argv)
int argc;
char **argv;
{				/* local variables			    */
/*
   Inititize.
*/

  ipb_format_playback(argv[1]);
/* 
   Cleanup and return.
*/
  fflush(stdout);
  return;
}
