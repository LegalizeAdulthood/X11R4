	static char rcsid [] = "$Header: /u/04c/mnt/integ/utilities/src/RCS/ufa.c,v 1.2 89/02/10 14:59:40 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  ufa.c
|
|  Description:
|	file access utilities for serpent
|
|  Component of:
|	utilities
|
|  Related files:
|	ufa.h
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
|	erik@sei.cmu.edu
|
|  Change History:
$Log:	ufa.c,v $
 * Revision 1.2  89/02/10  14:59:40  ejh
 * added copyright notice
 * 
 * Revision 1.1  89/01/26  16:07:43  ejh
 * Initial revision
 * 
|	20 Jan 89	created
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

#define	ufa_TOP

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<stdio.h>
#include	"global.h"
#include	"string.h"
#include	"ufa.h"

string	ufa_full_file_name (path, file)
string	path, file;

    {
    int		i = 0,
		start = 0,
		path_length = strlen (path),
		file_length = strlen (file);
    string	temp, local_path;
    struct	stat	buf;

    local_path = strcpy (make_node (path_length + 1), path);
    while (i < path_length)
	{
	while (local_path [i] != ' ' && i < path_length)
	    i++;
	local_path [i] = '\0';
	temp = strcpy (make_node (i - start + file_length + 2),
	    local_path + start);
	(void) strcat (temp, "/");
	(void) strcat (temp, file);
	if (!stat (temp, &buf))
	    if (!(buf.st_mode & S_IFDIR))
		return (temp);
	free_node (temp);
	while (local_path [i] == ' ' && i < path_length)
	    i++;
	start = i + 1;
	}
    return ((string) NULL);
    }
