/*--------------------------------------------------------------------------*\
|
|  Name:  aay.y
|
|  Description:
|	This is the grammar for SADDLE
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	aal.l
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
$Log:	aay.y,v $
 * Revision 1.4  89/10/18  10:35:55  ejh
 * was doing far too many make_nodes and strcpy's
 * 
 * Revision 1.3  89/02/09  19:25:17  ejh
 * added copyright notice
 * 
 * Revision 1.2  89/01/11  15:21:21  ejh
 * new handling of EXECSTRING
 * 
 * Revision 1.1  88/12/06  14:27:47  ejh
 * Initial revision
 * 
|	12 Nov 1988:	new version
|	14 Dec 1987:	original version
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

%{

	static char rcsid [] = "$Header: /u/04c/mnt/integ/saddle/src/RCS/aay.y,v 1.4 89/10/18 10:35:55 ejh Exp $";

#define	memoryPack

#include	"serpent.h"
#include	"string.h"
#include	"listPack.h"
#include	<stdio.h>

#include	"a22.h"
#include	"aap.h"

/*	needed by lex for detection of EOF */

int	yywrap ()

    {
    return (1);
    }

/*	some local goodies */

int		the_size;
LIST		name_list,
		comment_list;
a22_saddle_type	the_type;
a22_saddle_node	*name_pointer,
		*inline_comment;

%}

/*--------------------------------------------------------------------------*\
|
|  Type:  union
|
|  Description:
|	union of types for yacc
|
\*  Definition:                                                             */

	%union	{
		int		xint;
		string		xstring;
		a22_saddle_type	xsaddle_type;
		a22_saddle_node	*xsaddle_node;
		}

/*  Definition:                                                             *\
|
|  Components:
|	xint		for returning an integer
|	xstring		for returning a string
|	xsaddle_type	for returning an a22_saddle_type
|	xsaddle_node	for returning an a22_saddle_node
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|
|  Constants:  all of the tokens and the rules that need to be declared
|
|  Description:
|	these are all of the tokens and some of the rules used by yacc for the
|	SADDLE processor
|
\*  Definition:                                                             */

	%token	<xint>		xEND xSHARED xDATA xRECORD xNUMBER xBOOLEAN
	%token	<xint>		xINTEGER xREAL xSTRING xBUFFER xID xOF
	%token	<xint>		xLEFTPAREN xRIGHTPAREN xCOLON xSEMICOLON
	%token	<xint>		xCOMMA xLEFTBRACKET xRIGHTBRACKET xLEFTCURLY
	%token	<xint>		xRIGHTCURLY xEXECSTRING

	%token	<xstring>	xTEXT xINLINECOMMENT xCOMMENT

	%type	<xstring>	id_choice

	%type	<xsaddle_node>	name

/*                                                                          *\
|
\*--------------------------------------------------------------------------*/

	%start	saddle_spec

%%

saddle_spec:	xEXECSTRING
		{
		name_list = make_list ();
		comment_list = make_list ();
		aap_element_list = make_list ();
		aap_component_list = make_list ();
		aap_add_node (EXEC_STRING, aap_make_node (aap_tap_exec));
		}
		any_comment data_definition

data_definition:	name xCOLON xSHARED xDATA
		{
		DEBUG3 (fprintf (stderr, "aay: dd begin\n"));
		aap_add_node (DD, $1);
		}
		dd_body dd_end eos
		{
		if (inline_comment != NULL)
		    aap_add_node (INLINE_COMMENT, inline_comment);
		}
		opt_comment

any_comment:	xINLINECOMMENT
		{
		aap_add_node (INLINE_COMMENT, aap_make_node ($1));
		}
		opt_comment

	|	opt_comment

	;

dd_body:	record_type

	|	base_type

	|	dd_body record_type

	|	dd_body base_type

	;

record_type:	name xCOLON xRECORD
		{
		DEBUG3 (fprintf (stderr, "aay: record type\n"));
		aap_add_node (RECORD, $1);
		}
		record_body record_end eos
		{
		if (inline_comment != NULL)
		    aap_add_node (INLINE_COMMENT, inline_comment);
		}
		opt_comment

record_body:	base_type

	|	record_body base_type

	;

record_end:	xEND xRECORD
		{
		DEBUG3 (fprintf (stderr, "aay: record end\n"));
		aap_add_node (END_RECORD, aap_make_node (""));
		}

base_type:	the_base_type eos
		{
		while ((name_pointer = (a22_saddle_node *) get_from_head
		    (name_list)) != NULL)
		    {
		    name_pointer -> size = the_size;
		    aap_add_node (the_type, name_pointer);
		    if (inline_comment != NULL)
			aap_add_node (INLINE_COMMENT, inline_comment);
		    }
		}
		opt_comment

the_base_type:	name xCOLON type_choice
		{
		(void) add_to_head (name_list, $1);
		}

	|	name xCOMMA the_base_type
		{
		(void) add_to_head (name_list, $1);
		}

	;

type_choice:	xBOOLEAN
		{
		DEBUG3 (fprintf (stderr, "aay: boolean\n"));
		the_size = sizeof (boolean);
		the_type = BOOLEAN;
		}

	|	xINTEGER
		{
		DEBUG3 (fprintf (stderr, "aay: integer\n"));
		the_size = sizeof (int);
		the_type = INTEGER;
		}

	|	xREAL
		{
		DEBUG3 (fprintf (stderr, "aay: real\n"));
		the_size = sizeof (double);
		the_type = REAL;
		}

	|	xSTRING string_size
		{
		DEBUG3 (fprintf (stderr, "aay: string\n"));
		the_type = STRING;
		}

	|	xBUFFER
		{
		DEBUG3 (fprintf (stderr, "aay: buffer\n"));
		the_type = BUFFER;
		}

	|	xID id_choice
		{
		DEBUG3 (fprintf (stderr, "aay: id\n"));
		if ($2 != NULL)
		    {
		    name_pointer = NULL;
		    while ((name_pointer = (a22_saddle_node *) get_list_next
			(name_list, name_pointer)) != NULL)
			name_pointer -> id_of = $2;
		    }
		the_size = sizeof (id_type);
		the_type = ID;
		}

	;

string_size:	/* empty string */
		{
		the_size = 2;
		}

	|	xLEFTPAREN xNUMBER xRIGHTPAREN
		{
		the_size = $2 + 1;
		}

	|	xLEFTBRACKET xNUMBER xRIGHTBRACKET
		{
		the_size = $2 + 1;
		}

	|	xLEFTCURLY xNUMBER xRIGHTCURLY
		{
		the_size = $2 + 1;
		}

	;

id_choice:	/* empty string */
		{
		$$ = NULL;
		}

	|	xTEXT
		{}

	|	xOF xTEXT
		{
		$$ = $2;
		}

	;

name:		xTEXT
		{
		$$ = aap_make_node ($1);
		}

eos:		xSEMICOLON inline_comment

inline_comment:	/* empty string */
		{
		inline_comment = NULL;
		}

	|	xINLINECOMMENT
		{
		DEBUG3 (fprintf (stderr, "aay: inline comment\n"));
		inline_comment = aap_make_node ($1);
		}

	;

opt_comment:	/* empty string */

	|	comment
		{
		while ((name_pointer = (a22_saddle_node *) get_from_head
		    (comment_list)) != NULL)
		    aap_add_node (COMMENT, name_pointer);
		}

	;

comment:	xCOMMENT
		{
		DEBUG3 (fprintf (stderr, "aay: comment\n"));
		(void) add_to_tail (comment_list, aap_make_node ($1));
		}

	|	comment xCOMMENT
		{
		DEBUG3 (fprintf (stderr, "aay: comment\n"));
		(void) add_to_tail (comment_list, aap_make_node ($2));
		}

	;

dd_end:		xEND xSHARED xDATA
		{
		DEBUG3 (fprintf (stderr, "aay: dd end\n"));
		aap_add_node (END_DD, aap_make_node (""));
		}

%%
