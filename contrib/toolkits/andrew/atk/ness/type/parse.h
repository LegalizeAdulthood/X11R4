/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/type/RCS/parse.h,v 1.1 89/08/22 15:29:44 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/type/RCS/parse.h,v $ */

#ifndef _parse_h_
#define _parse_h_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *parse_h_rcsid = "$Header:";
#endif

/* 
	parse.h

	 header file for class file parser

*/

/*
 * $Log:	parse.h,v $
 * Revision 1.1  89/08/22  15:29:44  wjh
 * Initial revision
 * 
 */


union yystype {
	struct toksym *tok;
	struct type *type;
	char *str;
	long num;
	operation op;
	};

#define YYSTYPE union yystype

int yyparse();

YYSTYPE yylval;

struct lexdescription *parse_lexdesc();

#endif   /*  _parse_h_  */
