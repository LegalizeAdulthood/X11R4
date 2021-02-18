
extern char *XtNewString();
letter            [a-zA-Z]
any               [a-zA-Z0-9.\-+_]
digit             [0-9]
%%
                  yyin = Newsrcfp;
^[ \t]*"\n"       ;
"\n"              return(EOL);
"-"               return(DASH);
","               return(COMMA);
[:!]              {yylval.character = yytext[0]; return(SEPARATOR); };  
{digit}+          {yylval.integer = atoi(yytext); return(NUMBER); };
{letter}{any}+    {yylval.string = XtNewString(yytext); return(NAME); };
^{any}+           {yylval.string = XtNewString(yytext); return(NAME); };
[ \t]             ;
^"options ".*"\n" {
                    optionsLine = XtNewString(yytext);
                    optionsLine[utStrlen(optionsLine) - 1] = '\0';
                  };
%%
/*
 * $Header: /users/ricks/xrn-11/R4/RCS/newsrc.l,v 1.5 89/11/17 12:51:42 ricks Rel $
 */

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

yywrap()
{
    return(1);
}

/*ARGSUSED*/
yyerror(s)
char *s;
{
    /* lint/kludge */
#ifdef lint
    (void) yyinput();
    (void) yyoutput(0);
    (void) yyunput(0);
#endif /* lint */
}

