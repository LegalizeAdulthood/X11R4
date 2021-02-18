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
 * produce a new .bdf file 
 *
 */

#include "defs.h"
#include <stdio.h>
#include <errno.h>

extern int errno;

output(font, filename)
struct font *font;
char *filename;
{
	FILE *out;
	int nchars;
	struct character *charp;

	out = fopen(filename, "w");
	if (out == NULL) {
	    fprintf(stderr, "cannot create %s\n", filename);
	    exit(1);
	}
	fprintf(out, "STARTFONT %d.%d\n", font->rev, font->subrev);
	fprintf(out, "COMMENT %s\n", font->comment);
	fprintf(out, "FONT %s\n", font->fontname);
	fprintf(out, "SIZE %d %d %d\n",
				font->sizes[0], font->sizes[1],
				font->sizes[2]);
	fprintf(out, "FONTBOUNDINGBOX %d %d %d %d\n",
				font->boundingbox.w, font->boundingbox.h,
				font->boundingbox.x, font->boundingbox.y);
	fprintf(out, "STARTPROPERTIES %d\n", font->nprop);
	if (font->defaultCharValid) /* -- MON */
	    fprintf(out, "DEFAULT_CHAR %d\n", font->defaultChar);
	if (font->descentValid)
	    fprintf(out, "FONT_DESCENT %d\n", font->descent);
	if (font->ascentValid)
	    fprintf(out, "FONT_ASCENT %d\n", font->ascent);
	if (font->copyrightValid)
	    fprintf(out, "COPYRIGHT %s\n", font->copyright);
	fprintf(out, "ENDPROPERTIES\n");
	fprintf(out, "CHARS %d\n", font->nchars);

	nchars = font->nchars;
	charp = font->characters;
	while (nchars--)
	    outputchar(out, charp++);

	fprintf(out, "ENDFONT\n");
        fclose(out);
}

outputchar(out, charp)
FILE *out;
struct character *charp;
{
	int i;

	fprintf(out, "STARTCHAR %s\n", charp->charId);
	fprintf(out, "ENCODING %d\n", charp->encoding);
	fprintf(out, "SWIDTH %d %d\n", charp->swidth[0], charp->swidth[1]);
	fprintf(out, "DWIDTH %d %d\n", charp->dwidth[0], charp->dwidth[1]);
	fprintf(out, "BBX %d %d %d %d\n",
			    charp->bbx.w, charp->bbx.h,
			    charp->bbx.x, charp->bbx.y);
	fprintf(out, "BITMAP\n");
	for (i=0; i<charp->nrows; i++) 
	    fprintf(out, "%s\n", charp->rows[i]);
	fprintf(out, "ENDCHAR\n");
}
