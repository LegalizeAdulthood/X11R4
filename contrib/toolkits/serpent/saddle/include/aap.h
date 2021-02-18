	/* $Header: /u/04c/mnt/integ/saddle/include/RCS/aap.h,v 1.4 89/02/09 19:20:29 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  aap.h
|
|  Description:
|	The analysis abstraction of SADDLE
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	aap.c
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
$Log:	aap.h,v $
 * Revision 1.4  89/02/09  19:20:29  ejh
 * added copyright notice
 * 
 * Revision 1.3  89/01/11  14:49:10  ejh
 * added variable to handle execution string
 * 
 * Revision 1.2  88/12/06  16:05:19  ejh
 * put ifndef around memoryPack
 * 
 * Revision 1.1  88/12/06  14:10:20  ejh
 * Initial revision
 * 
|	16 Nov 1988:	created; incorporates all original analysis files
|	14 Dec 1988:	original version
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

#ifndef	AAP_H
#define	AAP_H

#ifndef	memoryPack
#define	memoryPack
#endif

#include	"listPack.h"

#include	"a22.h"

#ifdef	aap_TOP
#define	aap_EXTERN
#else
#define	aap_EXTERN	extern
#endif

/*--------------------------------------------------------------------------*\
|  Constants: errors
| 
|  Description:
|	used to tell current status of parsing
| 
\* Definition:                                                              */

#define	OK		 0
#define	FATAL_ERROR	-1
#define	NAME_ERROR	-2
#define	YACC_ERROR	-3
#define	EXEC_ERROR	-4

/*                                                                          *\
|
|  Components:
|	OK		what it sounds like
|	FATAL_ERROR	ditto
|	NAME_ERROR	to show that a name was overloaded
|	YACC_ERROR	used to fake out yacc when it calls yyerror
|	EXEC_ERROR	parameters withouth executable string
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variables: various LIST variables
| 
|  Description:
|	used to hold the values of various lists
|
\* Syntax:                                                                  */

	aap_EXTERN	LIST	aap_element_list,
				aap_component_list;

/*                                                                          *\
|
|  Components:
|	aap_element_list	holds the list of all element names found
|	aap_component_list	holds the list of all component names found
|				within a given record
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable: aap_tap_exec
| 
|  Description:
|	the executable string for the tap
|
\* Syntax:                                                                  */

	aap_EXTERN	string	aap_tap_exec;

/*                                                                          *\
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: aap_make_node
| 
|  Description:
|	creates a structure big enough to handle an a22_saddle_node and fills
|	in the name
|
\* Syntax:                                                                  */

	a22_saddle_node	*aap_make_node (
	/*	the_name:	in	string
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	the_name	the name of the node
|
|  Returns:
|	nothing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: aap_add_node
| 
|  Description:
|	adds a node to the list of SADDLE statements
|
\* Syntax:                                                                  */

	void	aap_add_node (
	/*	the_type:	in	a22_saddle_type,
		the_node:	in	*a22_saddle_node
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	the_type	the type of the node
|	the_node	the node itself
|
|  Returns:
|	nothing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: aap_check_name
| 
|  Description:
|	checks the name against those that have been used before
|
\* Syntax:                                                                  */

	void	aap_check_name	(
	/*	the_node:	in	*a22_saddle_node
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	the_node	the node to be checked
|
|  Returns:
|	nothing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: aap_error
|
|  Description:
|	reports errors
|
\* Syntax:                                                                  */

	void	aap_error	(
	/*	the_name:	in      string,
		the_error:	in      int,
		the_type:	in      a22_saddle_type
	*/
	);

/*                                                                          *\
|
|  Parameters:
|	the_name	the name that the error was found on, if on a name
|	the_error	the type of error
|	the_type	the type of the_name, if on a name
|
|  Returns:
|       nothing
|
\*--------------------------------------------------------------------------*/

#undef	yyerror
#define	yyerror(c)	aap_error(c,YACC_ERROR,BAD_LINE)

#endif
