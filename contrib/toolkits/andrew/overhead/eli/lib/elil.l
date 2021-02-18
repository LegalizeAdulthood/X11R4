/* See end for RCS and copyright info */

%{
#include <eli.h>
#include <eliy.h>

#undef YYLMAX
#define YYLMAX (2000)  /* Arbitrary bound on yytext size */
%}

WS	    [ \t]
NL	    \n
DIGIT	    [0-9]
MAGIC	    [()'`,"; \t\n]
NONMAGIC    [^()'`,"; \t\n]

%%

\(			    return (LPAREN);
\)			    return (RPAREN);
(\+|-)?{DIGIT}+/{MAGIC}	    {
				if (EliProcessInfo.yparsebuflen < yyleng+1)
				    EliProcessInfo.yparsebuf = (char *) realloc(EliProcessInfo.yparsebuf,
					    EliProcessInfo.yparsebuflen = yyleng+1);
				strcpy( EliProcessInfo.yparsebuf, yytext );
				return (INTEGER);
			    }
'			    return (SQUOTE);
\"([^"\\]|{NL}|\\.)*\"	    {
				if (EliProcessInfo.yparsebuflen < yyleng + 1)
				    EliProcessInfo.yparsebuf = (char *) realloc(EliProcessInfo.yparsebuf,
					    EliProcessInfo.yparsebuflen = yyleng + 1);
				strcpy( EliProcessInfo.yparsebuf, yytext );
				return (STRING);
			    }
{NONMAGIC}+		    {
				if (EliProcessInfo.yparsebuflen < yyleng + 1)
				    EliProcessInfo.yparsebuf = (char *) realloc(EliProcessInfo.yparsebuf,
					    EliProcessInfo.yparsebuflen = yyleng + 1);
				strcpy( EliProcessInfo.yparsebuf, yytext ); 
				EliUpCaseStr( EliProcessInfo.yparsebuf );
				return (SYMBOL);
			    }
;.*			    ;	    /* Ignore comments */
({WS}|{NL})+		    ;	    /* Discard whitespace */

%%

/***********************************************************
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#undef input()

int  input()		/* Replaces the lex macro */
{
    int             result;

    switch (EliProcessInfo.u_source) {	/* Where is the input coming from? */
	case e_source_stdin:
	    result = ((yytchar = yysptr > yysbuf ?
		    U(*--yysptr) : getc(yyin)) == 10 ?
		    (yylineno++, yytchar) : yytchar) == EOF ?
		    0 : yytchar;
	    /*
	     * This ugly expression is what comes with lex
	     */
	    break;
	case e_source_string:
	    result = (int) *(EliProcessInfo.u_sourcestring++);	/* No need to test for
							 * end-of-string here
							 * because if it's the
							 * end of string, zero
							 * will get returned
							 * anyway. */
	    break;
	case e_source_file:
	    if (EOF == (result = fgetc(EliProcessInfo.u_inputfp)))
		result = 0;
	    break;
	default:
	    result = 0;
	    break;
    }
    return (result);
}


#undef unput(c)

unput(c)		/* Replaces the lex macro */
    int  c;
{
    switch (EliProcessInfo.u_source)
    {
	case e_source_stdin:
	    yytchar = c;
	    if (yytchar == '\n')
		yylineno--;
	    *yysptr++ = yytchar;
	    break;
	case e_source_string:
	    *(--EliProcessInfo.u_sourcestring) = (char) c;
	    break;
	case e_source_file:
	    ungetc(c, EliProcessInfo.u_inputfp);
	    break;
    }
}

/*
 *  $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lib/RCS/elil.l,v 2.4 89/07/25 11:50:37 bobg Exp $
 *
 *  $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lib/RCS/elil.l,v $
 */
