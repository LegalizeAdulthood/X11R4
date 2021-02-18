/***********************************************************
Copyright 1989 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: showpcf.c,v 3.6 89/09/11 15:15:16 erik Exp $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h> 

#include <X11/X.h>
#include "misc.h"
#include "fontos.h"
#include "font.h"
#include "fontlib.h"
#include "fosfile.h"

#include "pcf.h"

char 	*program;
int	dumpTableOfContents=	0;

void
usage()
{
    fprintf (stderr, "usage:  %s [-options ...] snffile ...\n\n", program);
    fprintf (stderr, "where options include:\n");
    fprintf (stderr,
	"    -v            print character bearings and sizes\n");
    fprintf (stderr,
	"    -g            print character glyph bitmaps\n");
    fprintf (stderr,
	"    -h            print header information\n");
    fprintf (stderr,
	"    -toc          print table of contents only\n");
    fprintf (stderr, "\n");
    exit (1);
}

static void
TryFile(pFile, params, name, dumpLevel)
fosFilePtr	 pFile;
BuildParamsPtr	 params;
char		*name;
DumpLevel	 dumpLevel;
{
extern TableDesc	*pcfReadTableOfContents();
EncodedFontPtr		font=	NullEncodedFont;

    if (dumpTableOfContents) {
	int		 nEntries;
	TableDesc	*pToc;
	pToc= pcfReadTableOfContents(pFile,&nEntries);
	pcfPrintTOC(nEntries,pToc);
	return;
    }
    pcfReadFont(pFile,&font,NullCharSet,FONT_EVERYTHING,params);
    if (font == NullEncodedFont)
	fprintf (stderr, "%s: format failure for '%s'.\n", program, name);
    else
	DumpFont (font, params, dumpLevel);
    fosCloseFile(pFile);
}

main(argc, argv)
int	argc;
char	**argv;
{
int     	fontcnt = 0;
BuildParamsRec	params;
DumpLevel	dumpLevel = dumpFontInfo;
fosFilePtr	pFile;

    InitAtoms();

    params=	*fosNaturalParams;

    program = *argv;
    for (argc--, argv++; argc; argc--, argv++) {
	if (argv[0][0] == '-') {
	    switch (argv[0][1]) {
		case 'g': 
		    dumpLevel = dumpPictures;
		    break;
		case 'v': 
		    dumpLevel = dumpCharInfo;
		    break;
		case 'h': 
		    dumpLevel = dumpFontInfo;
		    break;
		case 't':
		    if (!strcmp(argv[0],"-toc")) 
			dumpTableOfContents= 1;
		    else
			usage ();
		    break;
		case '\0':		/* do stdin */
		    TryFile(fosStdin, &params, "-", dumpLevel);
		default: 
		    usage ();
		    break;
	    }
	 } else {
	    fontcnt++;
	    pFile = fosOpenFile(argv[0], "r");
	    if (pFile == NULL) {
		fprintf(stderr, "%s: ", program);
		perror(argv[0]);
	    } else {
		TryFile(pFile, &params, argv[0], dumpLevel);
		fosCloseFile(pFile,TRUE);
		pFile= NULL;
	    }
	}
    }
    if (!fontcnt)
	usage ();
    exit (0);
}

