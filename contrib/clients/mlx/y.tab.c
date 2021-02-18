
# line 2 "parse.y"
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
 * q & d hacked grammar for .bdf files.
 *
 * modified 13-14 June 1988 by mike newton (newton@csvax.caltech.edu) to
 *   handle DEFAULT_CHAR, and blank lines better.  (see parse.y)
 */
#include <stdio.h>
#include "defs.h"

extern int lineno;		/* counts lines in .bdf input file */
extern int stringMode;		/* kludge in scan.l */
static struct character *charp;	/* current character */
static char *bits[1000];	/* that good for long characters ... */
static int nrows;
static int npropCounted;


# line 29 "parse.y"
typedef union  {
	int ival;
	char *sval;
} YYSTYPE;
# define NEWLINE 257
# define DOT 258
# define STARTFONT 259
# define SIZE 260
# define FONTBOUNDINGBOX 261
# define STARTPROPERTIES 262
# define FONTDESCENT 263
# define FONTASCENT 264
# define COPYRIGHT 265
# define ENDPROPERTIES 266
# define DEFAULTCHAR 267
# define CHARS 268
# define ENCODING 269
# define SWIDTH 270
# define DWIDTH 271
# define BBX 272
# define BITMAP 273
# define ENDCHAR 274
# define ENDFONT 275
# define FONT 276
# define STARTCHAR 277
# define COMMENT 278
# define STRING 279
# define IDENTIFIER 280
# define NUMBER 281
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 292 "parse.y"


yyerror() {
	fprintf(stderr, "parse error in line %d\n", lineno);
	exit(1);
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 55
# define YYLAST 147
short yyact[]={

  95,  93,  17,  18,  20, 105, 102,  97,  87,  86,
  85,  84,  79,  62,  61,  59,  57,  55,  16,  54,
  15,  50,  49,  47,  96,  94,  40,  29,  28,  24,
  21, 104,  74,  75,  76,  77,  83,  74,  75,  76,
  77,   7,  22,  46,  89,  37,  36,  38,  53,  39,
  37,  36,  38,   4,  39,  41, 107, 106, 101, 100,
  98,  92,  90,  88,  78,  67,  66,  65,  64,  63,
  60,  58,  56,  48,  42,  27,  26,  69,  31,   9,
  91, 103,  99,  82,  73,  72,  71,  70,  25,  80,
  68,  45,  44,  43,  23,   6,  35,  34,  33,  32,
  51,  30,  19,  14,  13,  12,  11,  10,   8,  52,
   3,   5,   2,   1,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  81 };
short yypact[]={

-206,-1000,-227,-258,-251,-233,-1000,-252,-258,-1000,
-1000,-1000,-1000,-1000,-1000,-181,-182,-253,-254,-213,
-255,-203,-183,-234,-184,-1000,-1000,-1000,-259,-260,
-218,-1000,-1000,-1000,-1000,-1000,-262,-264,-185,-265,
-186,-266,-1000,-1000,-1000,-1000,-187,-1000,-1000,-267,
-268,-1000,-1000,-188,-189,-190,-1000,-191,-1000,-192,
-232,-193,-269,-1000,-1000,-1000,-1000,-1000,-237,-1000,
-1000,-1000,-1000,-1000,-270,-271,-272,-273,-1000,-194,
-230,-1000,-195,-1000,-196,-256,-257,-274,-1000,-197,
-1000,-1000,-1000,-1000,-198,-1000,-199,-275,-1000,-248,
-1000,-1000,-276,-1000,-200,-201,-1000,-1000 };
short yypgo[]={

   0, 113, 112, 111, 110, 108,  79, 107, 106, 105,
 104, 103, 102, 101, 100,  78,  99,  98,  97,  96,
  95,  94,  93,  92,  91,  90,  89,  77,  87,  86,
  85,  84,  83,  82,  81,  80 };
short yyr1[]={

   0,   1,   2,   4,   5,   5,   6,   6,   6,   6,
   6,   7,   7,   8,   9,  10,  11,  12,  13,  13,
  15,  15,  15,  15,  19,  17,  16,  18,  14,   3,
  20,  21,  21,  22,  22,  24,  23,  25,  25,  27,
  27,  27,  27,  28,  29,  29,  30,  30,  31,  26,
  32,  33,  33,  35,  34 };
short yyr2[]={

   0,   4,   2,   5,   1,   2,   1,   1,   1,   1,
   1,   2,   0,   2,   5,   6,   3,   3,   1,   2,
   1,   1,   1,   1,   3,   3,   3,   2,   2,   2,
   3,   0,   2,   1,   1,   1,   6,   1,   2,   1,
   1,   1,   1,   3,   3,   4,   3,   4,   6,   4,
   2,   0,   3,   0,   0 };
short yychk[]={

-1000,  -1,  -2,  -4, 259,  -3, -20, 268,  -5,  -6,
  -7,  -8,  -9, -10, -11, 278, 276, 260, 261, -12,
 262, 281, 275, -21, 281,  -6, 257, 257, 281, 281,
 -13, -15, -16, -17, -18, -19, 264, 263, 265, 267,
 281, 258, 257, -22, -23, -24, 277, 257, 257, 281,
 281, -14, -15, 266, 281, 281, 257, 281, 257, 281,
 257, 281, 281, 257, 257, 257, 257, 257, -25, -27,
 -28, -29, -30, -31, 269, 270, 271, 272, 257, 281,
 -26, -27, -32, 273, 281, 281, 281, 281, 257, 274,
 257, -35, 257, 257, 281, 257, 281, 281, 257, -33,
 257, 257, 281, -34, 279, 281, 257, 257 };
short yydef[]={

   0,  -2,   0,  12,   0,   0,  31,   0,   2,   4,
   6,   7,   8,   9,  10,   0,   0,   0,   0,   0,
   0,   0,   0,  29,   0,   5,  11,  13,   0,   0,
   0,  18,  20,  21,  22,  23,   0,   0,   0,   0,
   0,   0,   1,  32,  33,  34,   0,  35,  30,   0,
   0,  16,  19,   0,   0,   0,  27,   0,  17,   0,
   0,   0,   0,  28,  26,  25,  24,   3,   0,  37,
  39,  40,  41,  42,   0,   0,   0,   0,  14,   0,
   0,  38,   0,  53,   0,   0,   0,   0,  15,   0,
  51,  50,  43,  44,   0,  46,   0,   0,  36,  54,
  45,  47,   0,  49,   0,   0,  52,  48 };
#ifndef lint
static	char yaccpar_sccsid[] = "@(#)yaccpar 1.1 86/07/08 SMI"; /* from UCB 4.1 83/02/11 */
#endif

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 1:
# line 46 "parse.y"
{
			struct character *charp;
			int i;

			/*
			 * find max box for window size
			 */
			font.maxbbx.w = font.maxbbx.h = 0;
			for (i=0, charp=font.characters; i<font.nchars; i++, charp++) {
			    if (charp->bbx.w > font.maxbbx.w)
				font.maxbbx.w = charp->bbx.w;
			    if (charp->bbx.h > font.maxbbx.h)
				font.maxbbx.h = charp->bbx.h;
			}
		    } break;
case 3:
# line 67 "parse.y"
{
			font.rev = yypvt[-3].ival;
			font.subrev = yypvt[-1].ival;
			if (font.rev != 2 || font.subrev != 1)
			    fprintf(stderr, "WARNING: font is not rev 2.1\n");
		    } break;
case 11:
# line 87 "parse.y"
{
			font.comment = yypvt[-1].sval;
		    } break;
case 13:
# line 94 "parse.y"
{
			font.fontname = yypvt[-1].sval;
		    } break;
case 14:
# line 100 "parse.y"
{
			font.sizes[0] = yypvt[-3].ival;
			font.sizes[1] = yypvt[-2].ival;
			font.sizes[2] = yypvt[-1].ival;
		    } break;
case 15:
# line 108 "parse.y"
{
			font.boundingbox.w = yypvt[-4].ival;
			font.boundingbox.h = yypvt[-3].ival;
			font.boundingbox.x = yypvt[-2].ival;
			font.boundingbox.y = yypvt[-1].ival;
		    } break;
case 17:
# line 120 "parse.y"
{
			font.nprop = yypvt[-1].ival;
			npropCounted = 0;
		    } break;
case 24:
# line 138 "parse.y"
{
			font.defaultCharValid = 1;
			font.defaultChar = yypvt[-1].ival;
			npropCounted++;
		    } break;
case 25:
# line 146 "parse.y"
{
			font.descentValid = 1;
			font.descent = yypvt[-1].ival;
			npropCounted++;
		    } break;
case 26:
# line 154 "parse.y"
{
			font.ascentValid = 1;
			font.ascent = yypvt[-1].ival;
			npropCounted++;
		    } break;
case 27:
# line 162 "parse.y"
{
			font.copyrightValid = 1;
			font.copyright = yypvt[-1].sval;
			npropCounted++;
		    } break;
case 28:
# line 170 "parse.y"
{
			if (font.nprop != npropCounted)
			    fprintf(stderr, "Warning: STARTPROPERTIES\n");
		    } break;
case 30:
# line 180 "parse.y"
{
			font.nchars = yypvt[-1].ival;
			font.characters = (struct character *)
						malloc(sizeof(struct character) * yypvt[-1].ival);
			charp = font.characters;
		    } break;
case 36:
# line 200 "parse.y"
{
			charp->charId = yypvt[-5].sval;
			charp++;
		    } break;
case 43:
# line 217 "parse.y"
{
			charp->encoding = yypvt[-1].ival;
		    } break;
case 44:
# line 223 "parse.y"
{
			charp->swidth[0] = yypvt[-1].ival;
			charp->swidth[1] = 0;
		    } break;
case 45:
# line 228 "parse.y"
{
			charp->swidth[0] = yypvt[-2].ival;
			charp->swidth[1] = yypvt[-1].ival;
		    } break;
case 46:
# line 235 "parse.y"
{
			charp->dwidth[0] = yypvt[-1].ival;
			charp->dwidth[1] = 0;
		    } break;
case 47:
# line 240 "parse.y"
{
			charp->dwidth[0] = yypvt[-2].ival;
			charp->dwidth[1] = yypvt[-1].ival;
		    } break;
case 48:
# line 247 "parse.y"
{
			charp->bbx.w = yypvt[-4].ival;
			charp->bbx.h = yypvt[-3].ival;
			charp->bbx.x = yypvt[-2].ival;
			charp->bbx.y = yypvt[-1].ival;
		    } break;
case 49:
# line 256 "parse.y"
{
			int i;

			charp->nrows = nrows;
			charp->rows = (char **)malloc(sizeof(char *) * nrows);
			for (i=0; i<nrows; i++) 
			    charp->rows[i] = bits[i];
		    } break;
case 50:
# line 267 "parse.y"
{
			nrows = 0;
		    } break;
case 52:
# line 274 "parse.y"
{
			bits[nrows++] = yypvt[-1].sval;
		    } break;
case 53:
# line 280 "parse.y"
{
			/* this is a kludge - sorry */
			stringMode = 1;
		    } break;
case 54:
# line 287 "parse.y"
{
			stringMode = 0;
		    } break;
		}
		goto yystack;  /* stack new state and value */

	}
