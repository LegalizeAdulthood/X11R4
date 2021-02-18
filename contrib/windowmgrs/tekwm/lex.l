%{

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: lex.l,v 5.8 89/11/22 15:15:51 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 * 001 -- Jordan Hubbard, Ardent Computer
 *  Handle comments differently.
 */

#include "y.tab.h"
#define lex_include 1    /* lex defines "input", conflicts with XWMHints */
#include "awm.h"

#undef	YYLMAX

#ifdef	LEXBUFSIZ
#define YYLMAX	LEXBUFSIZ
#else
#define YYLMAX	1024
#endif

char *dequote();
char *stash();

%}
W	[ \t]+
%%

#.*\n				{ return(COMMENT); }	/* Comments. */
\\\n				{ Lineno++; }		/* Continuation. */
{W}				{ ; }	/* Whitespace. */
[^ \"\t\n#=\^\>\!\|\(\)\[\]\{\}:;]+	{ yylval.sval=stash(yytext);return(STRING); }
\"[^\"]*\"			{ yylval.sval=stash(dequote(yytext));
				  return(STRING); }
\n				{ return(NL); }
.				{ return(*yytext); }

%%

/*
 * Remove the quotes enclosing a quoted string.
 */
char *
dequote(s)
char *s;
{
    char *p;

    ++s;
    p = rindex(s, '"');
    *p = '\0';
    return(s);
}

/*
 * Save string away in memory and return a pointer to it.  If
 * no space could be found, quit with a fatal error.
 */
char *stash(s)
char *s;
{
    char *ptr;

    if (!s || !strlen(s))
	return(NULL);
    ptr = malloc(strlen(s) + 1);
    if (ptr == NULL) {
        Error("out of space");
    }
    return(strcpy(ptr, s));
}
