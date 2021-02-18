/*
$Header: /u/04c/mnt/integ/utilities/src/RCS/u22.c,v 1.3 89/10/18 10:15:26 ejh Exp $
*/

/*--------------------------------------------------------------------------*\
|
|  Name:  u22.c
|
|  Description:  
|     Global declarations of general interest to Serpent source modules.
|     Specifically, this file contains useful macros and definitions that
|     are useful across the Serpent system.
|
|  Component of:
|     Utilities Layer.
|
|  Related files:
|     None.
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
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Brian M. Clapper
|     bmc@sei.cmu.edu  
|

$Log:	u22.c,v $
 * Revision 1.3  89/10/18  10:15:26  ejh
 * created u22_convert_string (came from a22)
 * 
 * Revision 1.2  89/09/28  10:52:00  bmc
 * No important changes.  (Testing CM.)
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

#include <stdio.h>
#include <string.h>
#include "global.h"
#include "u22.h"

#define memoryPack
#include "memoryPack.h"

/*--------------------------------------------------------------------------*\
|  Routine: u22_copy_string (or just copy_string)
| 
|  Description:  
|     Allocates storage for a string and copies the parameter into it.
\*--------------------------------------------------------------------------*/

string u22_copy_string (old_string)
string old_string;
{
    char *new_string = NULL;


    if ( (old_string != NULL) &&
	 ((new_string = make_node (strlen (old_string) + 1)) != NULL) )
	(void) strcpy (new_string, old_string);

    return (new_string);
}

string	u22_convert_string (the_name, the_case)
string	the_name;
int	the_case;

    {
    int		i = 0;
    string	new_name;

/*	make a copy of the name and convert each character to new case */

    new_name = u22_copy_string (the_name);
    switch (the_case)
	{
	case UPPERCASE:
	    while (i < strlen (new_name))
		{
		new_name [i] = ToUpper (new_name [i]);
		i++;
		}
	    break;
	case LOWERCASE:
	    while (i < strlen (new_name))
		{
		new_name [i] = ToLower (new_name [i]);
		i++;
		}
	    break;
	}
    return (new_name);
    }
