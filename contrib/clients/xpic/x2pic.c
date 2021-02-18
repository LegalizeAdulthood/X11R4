/* $Header: x2pic.c,v 1.6 89/07/02 15:59:33 moraes Exp $ */
#include <X11/Xos.h>
#include <stdio.h>
#include <ctype.h>
#include "defs.h"
#include "tune.h"

/*
 *  Define this is your pic does not reset the scale between consecutive
 *  pictures, resulting in successive xpic pictures in a document
 *  shrinking more and more - or apply pic.fix to your pic source, if
 *  you have it - or complain to your pic vendor.
 */
/* define BROKENPIC */

/* To do:
	2. Allow some sort of scaling and positioning arguments.
 */

#define MAXFONTS 127
#define FONTDESCFILE "x2pic"

extern char *malloc();

#define xtransform(x)	((double) ((x) - orgX))
#define ytransform(y)	((double) (picHeight - (y)))

static char *progname;
static int orgX, orgY, picWidth, picHeight, delX, delY;

static double width, height;
static double x, y;
static char *style_words[NSTYLES] = {
	"", "dotted", "dashed", "dashed 0.1 * scale", "dashed 0.15 * scale"
};
static int style;
static int linewidth = 0;

static char *halign_words[] = {
	"", "ljust", "rjust"
};
static int halign;
static int valign;

static char *arrows[] = {
	"", "<-", "->", "<->"
};
static int arrow_type;
static char *name;
static double picScale = 1.0;

FILE *outFile;
FILE *inFile;

extern int optind;
extern char *optarg;

static void ChangeThickness(t)
{
	if (linewidth != t) {
		linewidth = t;
		if (linewidth == 0)
			(void) fprintf(outFile, ".ps 10\n");
		else
			(void) fprintf(outFile, ".ps %d\n", linewidth * 20);
	}
}

static void MakeBox(x1, y1, x2, y2, attr)
{
	style = getlinestyle(attr);
	if ((style < 0) || (style >= NSTYLES)) {
		(void) fprintf(stderr, "Unknown style - %d\n", style);
		style = 0;
	}
	width = x2 - x1;
	height = y2 - y1;
	x = xtransform(((double) (x1 + x2)) / 2.0);
	y = ytransform(((double) (y1 + y2)) / 2.0);
	(void) fprintf(outFile, "box %s wid %g ht %g at %g, %g\n", 
	 style_words[style], width, height, x, y);
}


/* Ellipses and circles have no attributes in pic - not yet, anyway. */
/*ARGSUSED*/
static void MakeEllipse(xc, yc, xr, yr, attr, x1, y1, x2, y2)
{
	if (attr != 0) {
		fputs("warning: ellipses have no attributes in pic\n", stderr);
	}
	width = 2.0 * xr;
	height = 2.0 * yr;
	x = xtransform((double) xc);
	y = ytransform((double) yc);
	(void) fprintf(outFile, "ellipse wid %g ht %g at %g, %g\n", 
	 width, height, x, y);
}

/*ARGSUSED*/
static void MakeCircle(xc, yc, r, attr, x1, y1, x2, y2)
{
	if (attr != 0) {
		(void) fprintf(stderr, "warning: circles have no attributes in pic\n");
	}
	x = xtransform((double) xc);
	y = ytransform((double) yc);
	(void) fprintf(outFile, "circle radius %d at %g, %g\n", 
	 r, x, y);
}


/*
 *  print out a string, escaping '"' with a \ - we don't escape \, to
 *  give people a mechanism for sending strange numbers to the lw
 */
static void PrintText(s)
char *s;
{
	register char *cp = s;

	for(; *cp != 0; cp++) {
		if (isascii(*cp) && isprint(*cp)) {
 			if (*cp != '"')
		 		(void) fputc(*cp, outFile);
			else
				(void) fprintf(outFile, "\\%c", *cp);
		} else {
			/* We don't want to print these! */
			(void) fprintf(stderr, "Ignoring weird character \\%o in \"%s\"", 
				*cp & 0xff, s);
		} 
	}
}
			


/*ARGSUSED*/
static void MakeText(s, len, font, size, attr, xc, yc, x1, y1, x2, y2)
char *s;
char *font;
{
	char *troff_font;
	char *lastfont = "P";
	extern char *HashSearch();
	
	if ((troff_font = HashSearch(font)) == NULL) {
		(void) fprintf(stderr, "Unavailable font %s\n", font);
		font = "";
		lastfont = "";
	}
	if (size < 0) {
		(void) fprintf(stderr, "Incorrect font size %d\n", size);
		size = 10;
	}
	valign = gettext_valign(attr);
	if ((valign < 0) || (valign >= 3)) {
		(void) fprintf(stderr, "Incorrect vert alignment %d\n", valign);
		valign = 0;
	}
	halign = gettext_halign(attr);
	if ((halign < 0) || (halign >= 3)) {
		(void) fprintf(stderr, "Incorrect horiz. alignment %d\n", halign);
		halign = 0;
	}
	size = (int) (picScale * size);
	x = xtransform(xc);
	switch (valign) {
	case 0: /* MIDLINE */
		y = ytransform( yc );
		break;
	case 1: /* TOPLINE */
		y = ytransform( yc + ((double) (y2 - y1)) / 2.0 );
		break;
	case 2: /* BOTLINE */
		y = ytransform( yc - ((double) (y2 - y1)) / 2.0 );
		break;
	}
	(void) fprintf(outFile, "\"%s\\s%d", troff_font, size);
	PrintText(s);
	(void) fprintf(outFile, "\\s0\\f%s\" %s at %g, %g\n", lastfont, 
	 halign_words[halign], x, y);
}
	

/*ARGSUSED*/
static void StartLine(xc, yc, n, attr, type)
{
	arrow_type = getlinearrow(attr);
	if ((arrow_type < 0) || (arrow_type >= 4)) {
		(void) fprintf(stderr, "Unknown arrow type %d\n", arrow_type);
		arrow_type = 0;
	}
	style = getlinestyle(attr);
	if ((style < 0) || (style >= NSTYLES)) {
		(void) fprintf(stderr, "Unknown style - %d\n", style);
		style = 0;
	}
	x = xtransform(xc);
	y = ytransform(yc);
	name = "line";
	if (type != LINE) {
		name = "spline";
		if (style != 0) {
			(void) fprintf(stderr,
			 "warning: splines have no attributes in pic\n");
			style = 0;
		}
	}
	(void) fprintf(outFile, "%s %s %s from %g, %g ",
	 name, arrows[arrow_type], style_words[style], x, y);
}


static void NextAt(xc, yc)
{
	x = xtransform(xc);
	y = ytransform(yc);
	(void) fprintf(outFile, "\\\n\tto %g, %g ", x, y);
}


static void EndLine()
{
	(void) fprintf(outFile, "\n");
}

/*
 *  Read in a file output by xpic, and emit the appropriate pic text
 */
static void convert()
{
	int type;
	int xc, yc, xr, yr, len, attr;
	int size;
	int x1, y1, x2, y2;
	char *s;
	int c, i, n;
	int err, nf, gs;
	int num, thickness;
	char font[MAXSTR];


#define INPERR 2
#define INPEOF 3

	err = 0;
#ifdef MAGIC
	/* Check for the magic header that the new xpic puts out */
	if ((c = fgetc(inFile)) == EOF) {
		(void) fprintf(stderr, "Incorrect input format");
		return;
	}
	(void) ungetc(c, inFile);
	if (c == '#') {
		/* Magic header - ignore */
		(void) fscanf(inFile, "%*[^\n]");
	}
#endif MAGIC
	/* Read in (and ignore) the gel bounding box */
	(void) fscanf(inFile, " %d %d %d %d %d", &x1, &y1, &x2, &y2, &gs);
	/* Set up constants for scaling, translation etc. */
	orgX = x1;
	orgY = y1;
	delX = x2 - x1;
	delY = y2 - y1;
	picWidth = x2;
	picHeight = y2;
	(void) fprintf(outFile, ".PS\nscale = %lg\n",
	 (double) (gs * 10 / picScale));
	/* Read in the actual picture */
	do {
		if ((nf = fscanf(inFile, " %d", &type)) != 1) {
			err = INPEOF;
			break;
		}
		nf = fscanf(inFile, " %d %d %d %d %d %x %d", &num, &x1, &y1, 
		 &x2, &y2, &attr, &thickness);
		if (nf != 7) {
			err = INPERR;
			break;
		}
		ChangeThickness(thickness);
		switch (type) {
		case BOX:
			MakeBox(x1, y1,x2, y2, attr);
			break;
		case ELLIPSE:
			nf = fscanf(inFile, " %d %d %d %d", &xc, &yc, &xr, &yr) ;
			if (nf != 4) {
				err = INPERR;
				break;
			}
			MakeEllipse(xc, yc, xr, yr, attr, x1, y1, x2, y2);
			break;
		case CIRCLE:
			nf = fscanf(inFile, " %d %d %d", &xc, &yc, &xr);
			if (nf != 3) {
				err = INPERR;
				break;
			}
			MakeCircle(xc, yc, xr, attr, x1, y1, x2, y2);
			break;
		case TEXT:
			nf = fscanf(inFile, " %d %d %d %s %d", &xc, &yc, &len, font, 
			 &size);
			if (nf != 5) {
				err = INPERR;
				break;
			}
			/*
			 *  For backward compatibility with the bad old days. The
			 *  old convention of storing font information was really
			 *  ugly - a font number from 0-3, (corresponding to Roman,
			 *  Bolld, Italic, Special) and a size from 0-9
			 *  (corresponding to point sizes 6 - 24)
			 */
			if (font[1] == '\0') {
				int oldfontconvention = TRUE;
				
				switch (font[0]) {
				case '0':
					(void) strcpy(font, "Roman");
					break;
				case '1':
					(void) strcpy(font, "Bold");
					break;
				case '2':
					(void) strcpy(font, "Italic");
					break;
				case '3':
					(void) strcpy(font, "Special");
					break;
				default:
					/* Must a new font with a one letter name. Eeep! */
					oldfontconvention = FALSE;
				}
				if (oldfontconvention)
					/* Convert to pointsize */
					size = size * 2 + 6;
			}
			/* Go to the next line */
			while ((c = fgetc(inFile)) != '\n' && c != EOF)
				;
			if (c == EOF) {
				err = INPERR;
				break;
			}
			if (( s = malloc((unsigned) (len + 2))) == NULL) {
				(void) fprintf(stderr, "No more memory for text string");
				break;
			}
			if (fgets(s, len + 1, inFile) == NULL) {
				free(s);
				err = INPERR;
				break;
			}
			s[len] = '\0';
			MakeText(s, len, font, size, attr, xc, yc, x1, y1, x2, y2);
			free(s);
			break;
		case LINE:
		case SPLINE:
			if (fscanf(inFile, " %d %d %d", &n, &xc, &yc) != 3) {
				err = INPERR;
				break;
			}
			StartLine(xc, yc, n, attr, type);
			for (i = 1; i < n; i++) {
				if (fscanf(inFile, " %d %d", &xc, &yc) != 2) {
					err = INPERR;
					break;
				}
				NextAt(xc, yc);
			}
			if (err != INPERR)
				EndLine();
			break;
		}
	} while (err == 0);
	(void) fprintf(outFile, ".PE\n");
#ifdef BROKENPIC
	(void) fprintf(outFile, ".PS\nscale = 0\n.PE\n");
#endif
	if (err == INPERR)
		(void) fprintf(stderr, "Incorrect input format");

	return;

#undef INPEOF
#undef INPERR
}


usage(s)
char *s;
{
	(void) fprintf(stderr, "Usage: %s [-s scale] [-f maxfonts] [filename]\n",
	 s);
	exit(-1);
}

/* Font tables are of the form
	xpic-font-name	PostScript-font-name
 */
readfonttable(file)
char *file;
{
	FILE *fp;
	char xfontname[MAXSTR], psfontname[MAXSTR];
	char *s1, *s2;
	extern char *strsave();
	
	if ((fp = fopen(file, "r")) == NULL)
		return;
	while(fscanf(fp, " %s %s", xfontname, psfontname) == 2) {
		if ((s1 = strsave(xfontname)) && (s2 = strsave(psfontname)))
			HashInsert(s1, s2);
		else {
			(void) fprintf(stderr, "Out of memory\n");
			exit(-1);
		}
	}
	(void) fclose(fp);
}


main(argc, argv)
int argc;
char **argv;
{
	double atof();
	int c;
	int nfonts = MAXFONTS;
	char fontfile[MAXSTR];
	char *hdir;
	char *getenv();
	
	progname = argv[0];
	inFile = stdin;
	outFile = stdout;
	while((c = getopt(argc, argv, "s:f:")) != EOF) {
		switch (c) {
		case 's':
			picScale = atof(optarg);
			break;
		case 'f':
			nfonts = atoi(optarg);
			break;
		case '?':
			usage(progname);
			break;
		}
	}

	HashInit(nfonts);
	(void) sprintf(fontfile, "%s/fontdesc/%s", LIBDIR, FONTDESCFILE);
	readfonttable(fontfile);
	if (hdir = getenv("HOME")) {
		(void) sprintf(fontfile, "%s/.%s", hdir, FONTDESCFILE);
		readfonttable(fontfile);
	}
	/* Must have some files */
	if (optind >= argc) {
		convert();
	} else {
		while (optind < argc) {
			if ((inFile = fopen(argv[optind], "r")) == NULL) {
				(void) fprintf(stderr, "Can't open %s for reading\n", 
				 argv[optind]);
				break;
			}
			outFile = stdout;
			convert();
			optind++;
		}
	}
}
