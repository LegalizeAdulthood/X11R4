%{

        static char rcsid [] = "$Header: /u/04c/mnt/integ/compiler/src/RCS/cal.l,v 1.12 89/10/23 19:59:13 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:	cal.l
|
|  Description:
|	This is the lexical analyzer for SLANG.
|
|  Component of:
|	SLANG processor
|
|  Related files:
|	many yacc, lex, and c files
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
$Log:	cal.l,v $
 * Revision 1.12  89/10/23  19:59:13  ejh
 * switched over to u22_copy_string and u22_convert_string
 * 
 * Revision 1.11  89/09/20  11:59:43  little
 * change YYLMAX to be dependent on a system variable so it will be more
 * portable.  change rule for # so we correctly get the file name field
 * 
 * Revision 1.10  89/07/31  14:32:32  ejh
 * added new reserved words: ADI, BOOLEAN, BUFFER, EXTERNALS, ID, INTEGER,
 * REAL, STRING, VOID
 * 
 * Revision 1.9  89/03/14  17:40:42  rcs
 * modified string constants to allow eols
 * 
 * Revision 1.8  89/02/22  16:23:41  rcs
 * fixed real literal problem
 * 
 * Revision 1.7  89/02/10  12:58:32  ejh
 * moved lex offset symbol to top of file to make it lex
 * 
 * Revision 1.6  89/02/09  16:24:54  ejh
 * added copyright notice
 * 
 * Revision 1.5  89/02/06  11:35:51  ejh
 * changed "y.tab.h" to "cay.h" so the new universal make works
 * 
 * Revision 1.4  89/01/04  14:42:50  ejh
 * fixed the extraction of file name and line number for include files
 * 
 * Revision 1.3  88/12/20  15:41:08  ejh
 * removed WHENMODIFIED; redefined real numbers as (double *); added '&' and
 * '|'; added ability to pick up line numbers and file names from included
 * files
 * 
 * Revision 1.2  88/12/08  14:16:03  ejh
 * on_create => on create, etc.; added log
 *
|	01 Mar 1988:	module created
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

#include	<ctype.h>
#include	"global.h"	/* global definitions */
#include	<stdio.h>	/* for I/O */
#include	"memoryPack.h"	/* to pick up make_node */
#include	"string.h"	/* string handling stuff */

/*	SLANG specific includes */

#include	"u22.h"		/* Serpent universals */
#include	"c22.h"		/* SLANG globals */
#include	"ca2.h"		/* SLANG analysis globals */
#include	"cay.h"		/* yacc token declarations */

	/* make the max input buffer size something reasonable */

#undef YYLMAX
#define YYLMAX BUFSIZ

char	lextext [YYLMAX];	/* line buffer; YYLMAX is plenty big enough */
char	*lextp = lextext;	/* pointer to above */
double  atof(); 
int	i, j;

#define	get_line() \
	(*lextp == '\0'? (fgets (lextext, YYLMAX - 1, yyin), \
		lextp = lextext, *lextp++): *lextp++)

/*	these are to redefine input and unput, so that we can read files,
	while still using lex's variables */

#undef	input
#define	input() \
	(((yytchar = yysptr > yysbuf?U (*--yysptr):get_line ()) == \
		'\n'?yylineno++:0), feof (yyin)?0:yytchar)

#undef	unput
#define	unput(c) \
	{yytchar = (c); if (yytchar == '\n') yylineno--; *yysptr++=yytchar;}

%}

%%

\"[^"]*\"	{

/*	note that no quotes or EOLs can be inside of a quoted string */

	DEBUG3 (fprintf (stderr, "found a QUOTE\n"));
	yytext [yyleng - 1] = '\0';
	yylval.xstring = u22_copy_string (yytext + 1);
	return (xQUOTE);
	}

"!*"[^("*!")]*"*!"	{

/*	this says: if you find a comment starter, read everything you find
	until you find the comment ender; then return only the text of the
	comment */

	DEBUG3 (fprintf (stderr, "found a COMMENT\n"));
	yytext [yyleng - 2] = '\0';
	yylval.xstring = u22_copy_string (yytext + 2);
	return (xCOMMENT);
	}

"/*"[.\n]*"*/"	{

/*	this is similar to the comment, but this is a throw away comment, i.e.
	it can show up anywhere in the file, 'cause we're not gonna save it */

	}

[aA][dD][iI]	{
	DEBUG3 (fprintf (stderr, "found the word ADI\n"));
	return (xADI);
	}

[aA][nN][dD]	{
	DEBUG3 (fprintf (stderr, "found the word AND\n"));
	return (xAND);
	}

[aA][tT][tT][rR][iI][bB][uU][tT][eE][sS]	{
	DEBUG3 (fprintf (stderr, "found the word ATTRIBUTES\n"));
	return (xATTRIBUTES);
	}

[bB][oO][oO][lL][eE][aA][nN]	{
	DEBUG3 (fprintf (stderr, "found the word BOOLEAN\n"));
	return (xBOOLEAN);
	}

[bB][uU][fF][fF][eE][rR]	{
	DEBUG3 (fprintf (stderr, "found the word BUFFER\n"));
	return (xBUFFER);
	}

[cC][rR][eE][aA][tT][iI][oO][nN][ \t]*[cC][oO][nN][dD][iI][tT][iI][oO][nN] {
	DEBUG3 (fprintf (stderr, "found the word CREATION_CONDITION\n"));
	return (xCREATIONCONDITION);
	}

[dD][oO]	{
	DEBUG3 (fprintf (stderr, "found the word DO\n"));
	return (xDO);
	}

[eE][lL][sS][eE]	{
	DEBUG3 (fprintf (stderr, "found the word ELSE\n"));
	return (xELSE);
	}

[eE][lL][sS][eE][ \t]*[iI][fF]	{
	DEBUG3 (fprintf (stderr, "found the word ELSE IF\n"));
	return (xELSEIF);
	}

[eE][nN][dD][ \t]*[iI][fF]	{
	DEBUG3 (fprintf (stderr, "found the word END IF\n"));
	return (xENDIF);
	}

[eE][nN][dD][ \t]*[vV][cC]	{
	DEBUG3 (fprintf (stderr, "found the word END VC\n"));
	return (xENDVC);
	}

[eE][nN][dD][ \t]*[wW][hH][iI][lL][eE]	{
	DEBUG3 (fprintf (stderr, "found the word END WHILE\n"));
	return (xENDWHILE);
	}

[eE][xX][tT][eE][rR][nN][aA][lL][sS]	{
	DEBUG3 (fprintf (stderr, "found the word EXTERNALS\n"));
	return (xEXTERNALS);
	}

[fF][aA][lL][sS][eE]	{
	DEBUG3 (fprintf (stderr, "found the word FALSE\n"));
	return (xFALSE);
	}

[iI][fF]	{
	DEBUG3 (fprintf (stderr, "found the word IF\n"));
	return (xIF);
	}

[iI][dD]	{
	DEBUG3 (fprintf (stderr, "found the word ID\n"));
	return (xID);
	}

[iI][nN][tT][eE][gG][eE][rR]	{
	DEBUG3 (fprintf (stderr, "found the word INTEGER\n"));
	return (xINTEGER);
	}

[mM][eE][tT][hH][oO][dD][sS]	{
	DEBUG3 (fprintf (stderr, "found the word METHODS\n"));
	return (xMETHODS);
	}

[nN][oO][tT]	{
	DEBUG3 (fprintf (stderr, "found the word NOT\n"));
	return (xNOT);
	}

[nN][uU][lL][lL]	{
	DEBUG3 (fprintf (stderr, "found the word NULL\n"));
	return (xNULL);
	}

[oO][bB][jJ][eE][cC][tT][sS]	{
	DEBUG3 (fprintf (stderr, "found the word OBJECTS\n"));
	return (xOBJECTS);
	}

[oO][nN][ \t]*[cC][rR][eE][aA][tT][eE]	{
	DEBUG3 (fprintf (stderr, "found the word ON_CREATE\n"));
	return (xONCREATE);
	}

[oO][nN][ \t]*[dD][eE][sS][tT][rR][oO][yY]	{
	DEBUG3 (fprintf (stderr, "found the word ON_DESTROY\n"));
	return (xONDESTROY);
	}

[oO][rR]	{
	DEBUG3 (fprintf (stderr, "found the word OR\n"));
	return (xOR);
	}

[rR][eE][aA][lL]	{
	DEBUG3 (fprintf (stderr, "found the word REAL\n"));
	return (xREAL);
	}

[sS][tT][rR][iI][nN][gG]	{
	DEBUG3 (fprintf (stderr, "found the word STRING\n"));
	return (xSTRING);
	}

[tT][hH][eE][nN]	{
	DEBUG3 (fprintf (stderr,"found the word THEN\n"));
	return (xTHEN);
	}

[tT][rR][uU][eE]	{
	DEBUG3 (fprintf (stderr, "found the word TRUE\n"));
	return (xTRUE);
	}

[uU][nN][dD][eE][fF][iI][nN][eE][dD]	{
	DEBUG3 (fprintf (stderr, "found the word UNDEFINED\n"));
	return (xUNDEFINED);
	}

[vV][aA][rR][iI][aA][bB][lL][eE][sS]	{
	DEBUG3 (fprintf (stderr, "found the word VARIABLES\n"));
	return (xVARIABLES);
	}

[vV][cC]	{
	DEBUG3 (fprintf (stderr, "found the word VC\n"));
	return (xVC);
	}

[vV][oO][iI][dD]	{
	DEBUG3 (fprintf (stderr, "found the word VOID\n"));
	return (xVOID);
	}

[wW][hH][iI][lL][eE]	{
	DEBUG3 (fprintf (stderr, "found the word WHILE\n"));
	return (xWHILE);
	}

[a-zA-Z]([a-zA-Z0-9_]*[a-zA-Z0-9]+)*	{

/*	now we look for names; any alphanumeric string (including underscores)
	which starts with a letter is valid text; if it's bigger than 32
	characters, truncate it; finally, convert it to lower case, as SLANG
	is case insensitive (as are SADDLE and ILL) */

	if (yyleng > 32)
		{
		yytext [32] = '\0';
		yyleng = 32;
		}
	yylval.xstring = u22_convert_string (yytext, LOWERCASE);
	DEBUG3 (fprintf (stderr, "found a NAME: %s\n", yytext));
	return (xNAME);
	}

[0-9]+"."[0-9]*	{

/*	now we look for real numbers; then convert from character to real */

	yylval.xreal = (double *) make_node (sizeof (double));
	*(yylval.xreal) = atof (yytext);
	DEBUG3 (fprintf (stderr, "found a REAL NUMBER: %s\n", yytext));
	return (xREALNUMBER);
	}

"."[0-9]+	{
	yylval.xreal = (double *) make_node (sizeof (double));
	*(yylval.xreal) = atof (yytext);
	DEBUG3 (fprintf (stderr, "found a REAL NUMBER: %s\n", yytext));
	return (xREALNUMBER);
	}

[0-9]+	{

/*	now we look for integer numbers; then we convert to integer */

	yylval.xint = atoi (yytext);
	DEBUG3 (fprintf (stderr, "found an INTEGER NUMBER: %s\n", yytext));
	return (xINTEGERNUMBER);
	}

"{"	{

/*	these are all the special characters */

	DEBUG3 (fprintf (stderr, "found a LEFT CURLY BRACKET\n"));
	return (xLEFTCURLY);
	}

"}"	{
	DEBUG3 (fprintf (stderr, "found a RIGHT CURLY BRACKET\n"));
	return (xRIGHTCURLY);
	}

"("	{
	DEBUG3 (fprintf (stderr, "found a LEFT PARENTHESIS\n"));
	return (xLEFTPAREN);
	}

")"	{
	DEBUG3 (fprintf (stderr, "found a RIGHT PARENTHESIS\n"));
	return (xRIGHTPAREN);
	}

"["	{
	DEBUG3 (fprintf (stderr, "found a LEFT SQUARE BRACKET\n"));
	return (xLEFTSQUARE);
	}

"]"	{
	DEBUG3 (fprintf (stderr, "found a RIGHT SQUARE BRACKET\n"));
	return (xRIGHTSQUARE);
	}

"."	{
	DEBUG3 (fprintf (stderr, "found a DOT\n"));
	return (xDOT);
	}

","	{
	DEBUG3 (fprintf (stderr, "found a COMMA\n"));
	return (xCOMMA);
	}

";"	{
	DEBUG3 (fprintf (stderr, "found a SEMICOLON\n"));
	return (xSEMICOLON);
	}

"<>"	{
	DEBUG3 (fprintf (stderr, "found a NOT EQUALS\n"));
	return (xNOTEQUAL);
	}

"<="	{
	DEBUG3 (fprintf (stderr, "found a LESS EQUALS\n"));
	return (xLESSEQUAL);
	}

"<"	{
	DEBUG3 (fprintf (stderr, "found a LESS THAN\n"));
	return (xLESSTHAN);
	}

">="	{
	DEBUG3 (fprintf (stderr, "found a GREATER EQUALS\n"));
	return (xGREATEREQUAL);
	}

">"	{
	DEBUG3 (fprintf (stderr, "found a GREATER THAN\n"));
	return (xGREATERTHAN);
	}

":="	{
	DEBUG3 (fprintf (stderr, "found a GETS\n"));
	return (xGETS);
	}

":"	{
	DEBUG3 (fprintf (stderr, "found a COLON\n"));
	return (xCOLON);
	}

"="	{
	DEBUG3 (fprintf (stderr, "found an EQUALS\n"));
	return (xEQUAL);
	}

"+"	{
	DEBUG3 (fprintf (stderr, "found a PLUS\n"));
	return (xPLUS);
	}

"-"	{
	DEBUG3 (fprintf (stderr, "found a MINUS\n"));
	return (xMINUS);
	}

"/"	{
	DEBUG3 (fprintf (stderr, "found a SLASH\n"));
	return (xSLASH);
	}

"**"	{
	DEBUG3 (fprintf (stderr, "found a POWER\n"));
	return (xPOWER);
	}

"*"	{
	DEBUG3 (fprintf (stderr, "found a STAR\n"));
	return (xSTAR);
	}

"&"	{
	DEBUG3 (fprintf (stderr, "found an AMPERSAND\n"));
	return (xAMPERSAND);
	}

"|"	{
	DEBUG3 (fprintf (stderr, "found a BAR\n"));
	return (xBAR);
	}

\n"#"[^\n]*	{

/*	this says: if you find a pound sign at the beginning of a line, read
	the entire line, then pick off the line number and file name */

	DEBUG3 (fprintf (stderr, "lex: found a pound sign\n"););

	i = 0;
	while (!isdigit (yytext [i]))
	    i++;
	yylineno = 0;
	while (isdigit (yytext [i]))
	    {
	    yylineno = 10 * yylineno + yytext [i] - '0';
	    i++;
	    }
	while (yytext [i] != '"')
	    i++;
	i++;
	DEBUG3 (fprintf (stderr, "lex: before mucking\n");
	    fprintf (stderr,
		"lex: length of rest = %d\n", strlen (yytext + i));
	    fprintf (stderr, "lex: rest = %s\n\n", yytext + i););
	j = i;
	while (yytext [j] != '"')
	    j++;
	yytext [j] = '\0';
	c22_pp_filename = u22_copy_string (yytext + i);
	DEBUG3
	    (fprintf (stderr, "lex: now reading from %s at line number %d\n",
		c22_pp_filename, yylineno););
	}

[ \t\n]	{

/*	skip spaces, tabs, and eols (yeah, that's a real space) */

	}

.	{

/*	this says: if you find any illegal characters (or sequence thereof),
	flush the rest of the line up to, but not including, the EOL and
	return an error */

	c22_error (ILLEGAL_CHAR, yytext, yylineno);
	}
