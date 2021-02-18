	/* $Header: /u/04c/mnt/integ/utilities/include/RCS/ufa.h,v 1.2 89/02/10 14:35:43 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  ufa.h
|
|  Description:
|	file access utilities abstraction for serpent
|
|  Component of:
|	utilities
|
|  Related files:
|	ufa.c
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
$Log:	ufa.h,v $
 * Revision 1.2  89/02/10  14:35:43  ejh
 * added copyright notice
 * 
 * Revision 1.1  89/01/26  16:05:34  ejh
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

#ifndef	UFA_H
#define	UFA_H

#include	"string.h"

#ifdef	ufa_TOP
#define	ufa_EXTERN
#else
#define	ufa_EXTERN	extern
#endif

/*--------------------------------------------------------------------------*\
|  Routine:	ufa_full_file_name
| 
|  Description:
|	takes as input the contents of a path variable and a file name, and
|	returns the first full path name of that file within that path
|	variable
| 
\* Definition:                                                              */

	string	ufa_full_file_name	(
	/*	path:	in	string,
		file:	in	string
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	path	the contents of the path variable
|	file	the file name to be searched for
|
|  Returns:
|	the full path name, if found; if not, NULL
|
\*--------------------------------------------------------------------------*/

#endif
