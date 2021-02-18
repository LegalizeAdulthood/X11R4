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

/* $XConsortium: bdftosnf.c,v 1.25 88/10/10 19:37:37 karlton Exp $ */

#include <stdio.h>
#include <X11/X.h>
#include "misc.h"

#include "fontstr.h"
#include "fontlib.h"
#include "fosfile.h"
#include "bdf.h"

char *myname = NULL;
char *currentFile = NULL;
int linenum = 0;

/*VARARGS*/
void
warning(msg, p1, p2, p3, p4)
    char *msg;
{
    fosInformation("%s: %s: ", myname, currentFile);
    fosInformation(msg, p1, p2, p3, p4);
    if (linenum)
	fosInformation(" at line %d\n", linenum);
    else
	fosInformation("\n");
}

/*
 * fatal error. never returns.
 */
/*VARARGS*/
void
fatal(msg, p1, p2, p3, p4)
    char *msg;
{
    warning(msg, p1, p2, p3, p4);
    exit(1);
}

main(argc, argv)
int	 argc;
char	*argv[];
{
fosFilePtr	pFile= NULL,pOutFile= NULL;
EncodedFontPtr	font= NullEncodedFont;
BuildParamsRec	params;
Mask		tables= FONT_EVERYTHING;
Mask		unread;
Bool		writeToStdout=	TRUE;
char		*outputFile=	NULL;

    params=	*fosNaturalParams;

    myname = argv[0];
    argc--, argv++;
    while (argc--) {
	if (argv[0][0] == '-') {
	    switch (argv[0][1]) {
	    case 'o':
		if (!writeToStdout) 
		    goto usage;
		if (argv[0][2]!=NUL)	outputFile=	&argv[0][2];
		writeToStdout=	FALSE;
		break;

	    case 'p':
		switch (argv[0][2]) {
		case '1':
		case '2':
		case '4':
		case '8':
		    if (argv[0][3] != '\0')
			goto usage;
		    params.glyphPad = argv[0][2] - '0';
		    break;
		default:
		    goto usage;
		}
		break;

	    case 'u':
		switch (argv[0][2]) {
		case '1':
		case '2':
		case '4':
		case '8':
		    if (argv[0][3] != '\0')
			goto usage;
		    params.scanUnit = argv[0][2] - '0';
		    break;
		default:
		    goto usage;
		}
		break;

	    case 'm':
		if (argv[0][2] != '\0')
		    goto usage;
		params.bitOrder = MSBFirst;
		break;

	    case 'l':
		if (argv[0][2] != '\0')
		    goto usage;
		params.bitOrder = LSBFirst;
		break;

	    case 'M':
		if (argv[0][2] != '\0')
		    goto usage;
		params.byteOrder = MSBFirst;
		break;

	    case 'L':
		if (argv[0][2] != '\0')
		    goto usage;
		params.byteOrder = LSBFirst;
		break;

	    case 'w':
		if (argv[0][2] != '\0')
		    goto usage;
		params.badbitsWarn = 1;
		break;

	    case 'W':
		if (argv[0][2] != '\0')
		    goto usage;
		params.ignoredCharWarn = 1;
		break;

	    case 't':	/* attempt to make terminal fonts if possible */
		if (argv[0][2] != '\0')
		    goto usage;
		params.makeTEfonts = 1;
		break;

	    case 'i':	/* inhibit ink metric computation */
		if (argv[0][2] != '\0')
		    goto usage;
		params.inhibitInk = 1;
		break;

	    default:
		goto usage;
	    }
	} else {
	    if (currentFile)
	    {
	usage:
		fosInformation(
	"usage: %s [-p#] [-u#] [-m] [-l] [-M] [-L] [-w] [-W] [-t] [-i] [bdf file]\n",
			myname);
		fosInformation("       where # for -p is 1, 2, 4, or 8\n");
		fosInformation("       and   # for -s is 1, 2, or 4\n");
		exit(1);
	    }
	    currentFile = argv[0];
	}
	argv++;
    }
    InitAtoms();

    if (currentFile) {
	if ((pFile= fosOpenFile(currentFile, "r")) == NULL) {
	    fatal("could not open \"%s\" to read\n",currentFile);
	}
    } else {
	pFile=	fosStdin;
	currentFile = "(stdin)";
    }

    if (writeToStdout) {
	pOutFile=	fosStdout;
    }
    else {
	char	outName[1000];
	int	len;

	if	(outputFile)	strcpy(outName,outputFile);
	else if	(currentFile)	strcpy(outName,currentFile);
	else			strcpy(outName,"stdin");
	len=	strlen(outName);
	if ((len>=4)&&(!strncmp(outName+len-4,".bdf"))) {
	    outName[len-4]=	NUL;
	}
	strcat(outName,".pcf");
	pOutFile=	fosOpenFile(outName,"w");
	if (pOutFile==NULL) {
	    fatal("could not open \"%s\" to write\n",outName);
	}
    }

    unread= bdfReadFont(pFile, &font, NullCharSet, FONT_EVERYTHING, &params);
    if (unread) {
	fosWarning("unread tables after bdfReadFont (0x%x)\n",unread);
    }
    if (font == NullEncodedFont)
	exit(1);
    fosCloseFile(pFile,TRUE);

#ifdef notdef
    DumpFont(font, &params, dumpPictures);
#endif

    if (!font->pCS->inkMetrics)
	tables&=	(~FONT_INK_METRICS);

    pcfWriteFont(pOutFile, font, tables, &params);
    fosCloseFile(pOutFile,FALSE);
    exit(0);
}

