	/* $Header: /u/04c/mnt/integ/saddle/include/RCS/acg.h,v 1.4 89/02/09 19:21:29 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  acg.h
|
|  Description:
|	The code generator abstraction of SADDLE
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	acg.c
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
$Log:	acg.h,v $
 * Revision 1.4  89/02/09  19:21:29  ejh
 * added copyright notice
 * 
 * Revision 1.3  89/01/12  00:57:09  ejh
 * added magic_number to calling sequence of all code generators
 * 
 * Revision 1.2  89/01/11  14:50:58  ejh
 * removed full name and added ill short name to calling sequences of all
 * code generators
 * 
 * Revision 1.1  88/12/06  14:11:27  ejh
 * Initial revision
 * 
|	22 Nov 1988:	created
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

#ifndef	ACG_H
#define	ACG_H

#ifdef	acg_TOP
#define	acg_EXTERN
#else
#define	acg_EXTERN	extern
#endif

/*--------------------------------------------------------------------------*\
|  Routine: acg_c_code
| 
|  Description:
|	back end c code generator
|
\* Syntax:                                                                  */

	void	acg_c_code	(
	/*	file_part:	in	string,
		mailbox_name:	in	string,
		short_name:	in	string,
		ill_short_name:	in	string,
		magic_number:	in	string
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	file_part	the name of the source file without an .ext
|	mailbox_name	the name of the mailbox for this shared data
|	short_name	the name of just the file, without .ext or leader
|	ill_short_name	short_name & .ill
|	magic_number	unique id of this run
|
|  Returns:
|	nothing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: acg_ada_code
| 
|  Description:
|	back end Ada code generator
|
\* Syntax:                                                                  */

	void	acg_ada_code	(
	/*	file_part:	in	string,
		mailbox_name:	in	string,
		short_name:	in	string,
		ill_short_name:	in	string,
		magic_number:	in	string
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	file_part	the name of the source file without an .ext
|	mailbox_name	the name of the mailbox for this shared data
|	short_name	the name of just the file, without .ext or leader
|	ill_short_name	short_name & .ill
|	magic_number	unique id of this run
|
|  Returns:
|	nothing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: acg_ill_code
| 
|  Description:
|	back end ill code generator
|
\* Syntax:                                                                  */

	void	acg_ill_code	(
	/*	file_part:	in	string,
		mailbox_name:	in	string,
		short_name:	in	string,
		ill_short_name:	in	string,
		magic_number:	in	string
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	file_part	the name of the source file without an .ext
|	mailbox_name	the name of the mailbox for this shared data
|	short_name	the short name of the ill file
|	ill_short_name	short_name & .ill
|	magic_number	unique id of this run
|
|  Returns:
|	nothing
|
\*--------------------------------------------------------------------------*/

#endif
