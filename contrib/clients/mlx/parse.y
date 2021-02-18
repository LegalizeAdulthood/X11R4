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

%}

%union {
	int ival;
	char *sval;
}

%token NEWLINE DOT
%token STARTFONT SIZE FONTBOUNDINGBOX STARTPROPERTIES
%token FONTDESCENT FONTASCENT COPYRIGHT ENDPROPERTIES
%token DEFAULTCHAR                                      /* -- MON */
%token CHARS ENCODING SWIDTH DWIDTH BBX BITMAP
%token ENDCHAR ENDFONT
%token <sval> FONT COPYRIGHT STARTCHAR COMMENT STRING IDENTIFIER
%token <ival> NUMBER

%%

file:           definitions characters ENDFONT NEWLINE
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
		    }
		;

definitions:    startfont fontdefs 
		;

startfont:      STARTFONT NUMBER DOT NUMBER NEWLINE
		    {
			font.rev = $2;
			font.subrev = $4;
			if (font.rev != 2 || font.subrev != 1)
			    fprintf(stderr, "WARNING: font is not rev 2.1\n");
		    }
		;

fontdefs:	fontdef
		| fontdefs fontdef
		;

fontdef:	comment
		| font
		| size
		| boundingbox
		| properties
		;

comment:        COMMENT NEWLINE
		    {
			font.comment = $1;
		    }
		|
		;

font:           FONT NEWLINE
		    {
			font.fontname = $1;
		    }
		;

size:           SIZE NUMBER NUMBER NUMBER NEWLINE
		    {
			font.sizes[0] = $2;
			font.sizes[1] = $3;
			font.sizes[2] = $4;
		    }
		;

boundingbox:    FONTBOUNDINGBOX NUMBER NUMBER NUMBER NUMBER NEWLINE
		    {
			font.boundingbox.w = $2;
			font.boundingbox.h = $3;
			font.boundingbox.x = $4;
			font.boundingbox.y = $5;
		    }
		;

properties:     startprop props endprop
		;

startprop:      STARTPROPERTIES NUMBER NEWLINE
		    {
			font.nprop = $2;
			npropCounted = 0;
		    }
		;

props:		prop
		| props prop
		;

prop:		ascent
		| descent
		| copyright
		| defaultchar            /* -- MON */
		;

/* all of this -- MON */
defaultchar:    DEFAULTCHAR NUMBER NEWLINE
		    {
			font.defaultCharValid = 1;
			font.defaultChar = $2;
			npropCounted++;
		    }
		;

descent:        FONTDESCENT NUMBER NEWLINE
		    {
			font.descentValid = 1;
			font.descent = $2;
			npropCounted++;
		    }
		;

ascent:         FONTASCENT NUMBER NEWLINE
		    {
			font.ascentValid = 1;
			font.ascent = $2;
			npropCounted++;
		    }
		;

copyright:      COPYRIGHT NEWLINE
		    {
			font.copyrightValid = 1;
			font.copyright = $1;
			npropCounted++;
		    }
		;

endprop:        ENDPROPERTIES NEWLINE
		    {
			if (font.nprop != npropCounted)
			    fprintf(stderr, "Warning: STARTPROPERTIES\n");
		    }
		;

characters:     chars chardefs
		;

chars:          CHARS NUMBER NEWLINE
		    {
			font.nchars = $2;
			font.characters = (struct character *)
						malloc(sizeof(struct character) * $2);
			charp = font.characters;
		    }
		;

chardefs:	/* empty */
		| chardefs character
		;

/* Hacked the next 7 lines -- MON */
character:	rcharacter
		| ncharacter
		;

ncharacter:	NEWLINE

rcharacter:     STARTCHAR NEWLINE charprops bitmap ENDCHAR NEWLINE
		    {
			charp->charId = $1;
			charp++;
		    }
		;

charprops:	charprop
		| charprops charprop
		;

charprop:	encoding
		| swidth
		| dwidth
		| bbx
		;

encoding:       ENCODING NUMBER NEWLINE
		    {
			charp->encoding = $2;
		    }
		;

swidth:         SWIDTH NUMBER NEWLINE
		    {
			charp->swidth[0] = $2;
			charp->swidth[1] = 0;
		    }
	        | SWIDTH NUMBER NUMBER NEWLINE
		    {
			charp->swidth[0] = $2;
			charp->swidth[1] = $3;
		    }
		;

dwidth:         DWIDTH NUMBER NEWLINE
		    {
			charp->dwidth[0] = $2;
			charp->dwidth[1] = 0;
		    }
		| DWIDTH NUMBER NUMBER NEWLINE
		    {
			charp->dwidth[0] = $2;
			charp->dwidth[1] = $3;
		    }
		;

bbx:            BBX NUMBER NUMBER NUMBER NUMBER NEWLINE
		    {
			charp->bbx.w = $2;
			charp->bbx.h = $3;
			charp->bbx.x = $4;
			charp->bbx.y = $5;
		    }
		;

bitmap:         bitmapstart NEWLINE numbers stringmodeoff
		    {
			int i;

			charp->nrows = nrows;
			charp->rows = (char **)malloc(sizeof(char *) * nrows);
			for (i=0; i<nrows; i++) 
			    charp->rows[i] = bits[i];
		    }
		;

bitmapstart:    BITMAP stringmodeon
		    {
			nrows = 0;
		    }
		;

numbers:        ; /* empty */
		| numbers STRING NEWLINE
		    {
			bits[nrows++] = $2;
		    }
		;

stringmodeon:   /* empty */
		    {
			/* this is a kludge - sorry */
			stringMode = 1;
		    }
		;

stringmodeoff:  /* empty */
		    {
			stringMode = 0;
		    }
		;

%%

yyerror() {
	fprintf(stderr, "parse error in line %d\n", lineno);
	exit(1);
}
