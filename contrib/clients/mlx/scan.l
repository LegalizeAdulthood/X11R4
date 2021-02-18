%o 10000
%a 15000

%{

/*
 * Copyright 1988 Siemens
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Siemens not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Siemens makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Claus Gittinger, Siemens
 * 
 * written 16-17 Mar 1988 by claus gittinger
 *
 * atomizer for .bdf files
 *
 * modified 13-14 June 1988 by mike newton (newton@csvax.caltech.edu) to
 *   handle DEFAULT_CHAR, and blank lines better.  (see parse.y)
 */

#include "y.tab.h"
#include "defs.h"

int lineno = 1;
int stringMode = 0;

%}

DIGIT		[0-9]
HEXDIGIT	[0-9a-fA-F]
HEX		{HEXDIGIT}+
ID              [a-zA-Z][-a-zA-Z0-9.]*

%%

STARTFONT           return STARTFONT;
SIZE                return SIZE;
FONTBOUNDINGBOX     return FONTBOUNDINGBOX;
STARTPROPERTIES     return STARTPROPERTIES;
FONT_DESCENT        return FONTDESCENT;
FONT_ASCENT         return FONTASCENT;
DEFAULT_CHAR        return DEFAULTCHAR;                 /* -- MON */
ENDPROPERTIES       return ENDPROPERTIES;
CHARS               return CHARS;
ENCODING            return ENCODING;
SWIDTH              return SWIDTH;
DWIDTH              return DWIDTH;
BBX                 return BBX;
BITMAP              return BITMAP;
ENDCHAR             return ENDCHAR;
ENDFONT             return ENDFONT;
FONT" ".*           {
			yylval.sval = malloc(strlen(yytext+5)+1);
			strcpy(yylval.sval, yytext+5);
			return FONT;
		    }
COPYRIGHT" ".*      {
			yylval.sval = malloc(strlen(yytext+10)+1);
			strcpy(yylval.sval, yytext+10);
			return COPYRIGHT;
		    }
STARTCHAR" ".*      {
			yylval.sval = malloc(strlen(yytext+10)+1);
			strcpy(yylval.sval, yytext+10);
			return STARTCHAR;
		    }
COMMENT" ".*        {
			yylval.sval = malloc(strlen(yytext+8)+1);
			strcpy(yylval.sval, yytext+8);
			return COMMENT;
		    }

[0-9]+              {
			if (stringMode) {
			    yylval.sval = malloc(strlen(yytext)+1);
			    strcpy(yylval.sval, yytext);
			    return STRING;
			} else
			    sscanf(yytext, "%d", &yylval.ival);
			return NUMBER;
		    }

-[0-9]+             {
			sscanf(yytext, "%d", &yylval.ival);
			return NUMBER;
		    }

{HEX}               {
			if (stringMode) {
			    yylval.sval = malloc(strlen(yytext)+1);
			    strcpy(yylval.sval, yytext);
			    return STRING;
			}
			sscanf(yytext, "%x", &yylval.ival);
			return NUMBER;
		    }

{ID}                {
			yylval.sval = malloc(strlen(yytext)+1);
			strcpy(yylval.sval, yytext);
			return IDENTIFIER;
		    }

\.                  return DOT;

\".*\"              {
			yylval.sval = malloc(strlen(yytext)+1);
			strcpy(yylval.sval, yytext);
			return STRING;
		    }

\t                  ;

\n                  {
			lineno++;
			return NEWLINE;
		    }

.                   ;

%%

