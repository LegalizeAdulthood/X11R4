	/* $Header: /u/04c/mnt/integ/saddle/include/RCS/a22.h,v 1.10 89/10/18 10:16:19 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  a22.h
|
|  Description:
|	A collection of global entities used all over SADDLE
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	many
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
$Log:	a22.h,v $
 * Revision 1.10  89/10/18  10:16:19  ejh
 * moved a22_convert_name to utilities (u22.h), along with the constants
 * UPPER and LOWER
 * 
 * Revision 1.9  89/08/30  14:56:43  bmc
 * No longer includes "<ctype.h>".  The macros/functions tolower() and
 * toupper() are not handled consistently across various UNIX versions,
 * so consistent versions are now provided in "u22.h" (included by this
 * header file).
 * 
 * Revision 1.8  89/08/29  13:57:50  bmc
 * Remove ctype-derivative definitions for Sun.  Add line to include <ctype.h>
 * unconditionally.
 * 
 * Revision 1.7  89/02/09  19:19:42  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/01/11  13:54:16  ejh
 * removed VERSION; added generic "magic number"
 * 
 * Revision 1.5  88/12/21  17:01:12  ejh
 * added code to handle toupper and tolower on the sun
 * 
 * Revision 1.4  88/12/21  13:12:26  ejh
 * new version number
 * 
 * Revision 1.3  88/12/16  15:29:38  ejh
 * updated VERSION
 * 
 * Revision 1.2  88/12/06  16:03:49  ejh
 * put ifndef around memoryPack
 * 
 * Revision 1.1  88/12/06  14:09:43  ejh
 * Initial revision
 * 
|	12 Nov 1988:	created; incorporates all original .h files (scons,
|			svars, sdebug)
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

#ifndef	A22_H
#define	A22_H

#ifndef	memoryPack
#define	memoryPack
#endif

#include	"global.h"
#include	"string.h"
#include	"listPack.h"
#include	<stdio.h>

#ifdef	a22_TOP
#define	a22_EXTERN
#else
#define	a22_EXTERN	extern
#endif

/*--------------------------------------------------------------------------*\
|  Constants:	DD, RECORD
|		BOOLEAN, INTEGER, REAL, STRING, BUFFER, ID
|		END_DD, END_RECORD
|
|		COMMENT, INLINE_COMMENT, BAD_LINE
| 
|  Description:
|	an enumeration of the major types of SADDLE, as well as some other
|	lexically significant words
| 
\* Definition:                                                              */

	typedef	enum	{

/*	major types */

	DD,
	RECORD,

/*	simple types */

	BOOLEAN,
	INTEGER,
	REAL,
	STRING,
	BUFFER,
	ID,

/*	used to signify the finding of the word END followed by whatever */

	END_DD,
	END_RECORD,

/*	other miscellaneous types */

	COMMENT,
	INLINE_COMMENT,
	BAD_LINE,

/*	and the execution string */

	EXEC_STRING,

/*	just a trick to make arrays which use these values as subscripts
	work right */

	NUMBER_OF_SADDLE_TYPES

	}	a22_saddle_type;

/*                                                                          *\
|
|  Components:
|	these are passed around to procedures to identify the current
|	context, instead of some character string
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constant:	a22_saddle_magic_id
| 
|  Description:
|	a string which describes the current version and identifies files as
|	having been generated by SADDLE
| 
\* Definition:                                                              */

#define	a22_saddle_magic_id	"SADDLE version 1.25"

/*                                                                          *\
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: a22_saddle_node
| 
|  Description:
|	structure holding the definition of each saddle statement
|	intermediate form
| 
\* Definition:                                                              */

	typedef	struct	{
		a22_saddle_type	stype;
		string		name;
		string		id_of;
		int		size;
		}	a22_saddle_node;

/*                                                                          *\
|
|  Components:
|	stype	type of statement
|	name	name associated with the statement: could be a type or
|		a shared data; could also be the text of a comment
|	id_of	if name is an id, shows what's it's an id of
|	size	size of the entity, if a type
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable: a22_saddle_list
| 
|  Description:
|	the list of SADDLE nodes
| 
\* Definition:                                                              */

	a22_EXTERN	LIST	a22_saddle_list;

/*                                                                          *\
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable: a22_continue
|
|  Description:
|       used by main to determine if yacc finished with no fatal errors
|
\* Definition:                                                              */

#ifndef a22_TOP

        extern  boolean a22_continue;

#else

        boolean a22_continue = TRUE;

#endif

/*                                                                          *\
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routines:	DEBUG1, DEBUG2, DEBUG3
| 
|  Description:
|	define the macros for various levels of debug for SADDLE
| 
\* Definition:                                                              */

	a22_EXTERN	sdd_debug;

#define	DEBUG1(statement) if (sdd_debug > 0) {statement; fflush (stderr);}

#define	DEBUG2(statement) if (sdd_debug > 1) {statement; fflush (stderr);}

#define	DEBUG3(statement) if (sdd_debug > 2) {statement; fflush (stderr);}

/*                                                                          *\
|
|  Components:
|	DEBUG1	used for errors
|	DEBUG2	used for routine entry/exit
|	DEBUG3	used for variable tracing
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variables: yyin, yylineno
|
|  Description:
|	needed by lex and yacc
|
\* Definition:                                                              */

	extern	FILE	*yyin;
	extern	int	yylineno;

/*                                                                          *\
|
|  Components:
|	yyin		the name of the file pointer used by lex
|	yylineno	the current line number in yacc
|
\*--------------------------------------------------------------------------*/

#endif
