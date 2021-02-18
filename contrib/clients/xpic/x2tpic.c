/* $Header: x2tpic.c,v 1.3 89/07/02 15:59:58 moraes Exp $ */
#include <X11/Xos.h>
#include <stdio.h>
#include <ctype.h>
#include "defs.h"
#include "tune.h"
#include "xtp.h"

/*
 * This program reads in a file in xpic formats and outputs a file which, when
 * given as input to tpic, will produce a TeX format file with the xpic
 * picture.  The difference from x2pic is that strings are written in the form
 * "{\twelverm datastring}" instead of "\fR\s12datastring\fP".  Furthermore,
 * when a font is encountered, a string of the form "\font\eighteenrm cmr10
 * scaled1800" is written after the ".PE"; this will pass through tpic and
 * initialize the font when it gets to TeX.
 */

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


#define MAXFONTS 512
#define FONTDESCFILE "x2tpic"

#define STRNEQ(s1, s2, n)	((*s1)==(*s2) && strncmp(s1, s2, n) == 0)

extern char *malloc();
extern char *calloc();

#define xtransform(x)	((double) ((x) - orgX))
#define ytransform(y)	((double) (picHeight - (y)))

static int orgX, orgY, picWidth, picHeight, delX, delY;

static double width, height;
static double x, y;
static char *style_words[NSTYLES] = {
	"", "dotted", "dashed", "dashed 0.1 * scale", "dashed 0.15 * scale"
};
static int style;
static int linewidth = 0;

static char *progname;
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

static int nfonts = MAXFONTS;
struct fontinfo *availfonts;
int navailfonts;

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
		(void) fputs("warning: ellipses have no attributes in pic\n", stderr);
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
 *  print out a string. For TeX, we don't escape anything.
 */
static void PrintText(s)
char *s;
{
	register char *cp = s;

	for(; *cp != 0; cp++) {
		if (isascii(*cp) && isprint(*cp)) {
		 	(void) fputc(*cp, outFile);
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
	/* FIX FOR TeX */
	(void) fprintf(outFile, "\"{%s ", ChangeFont( font, size) );
	PrintText(s);
	(void) fprintf(outFile, " }\" %s at %g, %g\n", halign_words[halign], x, y);
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

	/* FIX FOR TeX */
	(void) fflush(outFile);
	rewind(outFile);
	PutTexFontPostScript(stdout);
	copystuff(outFile, stdout);
	rewind(outFile);

	return;

#undef INPEOF
#undef INPERR
}

copystuff(from, to)
FILE *from;
FILE *to;
{
	char s[BUFSIZ];
	
	while(fgets(s, sizeof(s), from) != NULL)
		fputs(s, to);

}

usage(s)
char *s;
{
	(void) fprintf(stderr, "Usage: %s [-s scale] [-f maxfonts] [filename]\n",
	 s);
	exit(-1);
}

char *
getword(cpp)
char **cpp;
{
	register char *cp = *cpp;
	char *s;

	while(*cp != '\0' && (*cp == ' ' || *cp == '\t' || *cp == '\n'))
		cp++;
	if (*cp == '\0')
		return(NULL);
	s = cp;
	while(*cp != '\0' && *cp != ' ' && *cp != '\t' && *cp != '\n')
		cp++;
	*cp++ = '\0';
	*cpp = cp;
	return(s);
}


/* Font tables are of the form
        xpic-font-name point-size tex-font-string real-tex-font how-to-scale
   (eg)
        Roman 12 twelverm cmr10 magstep1
        Helvetica 24 twfourhelv PS-Helvetica pointsize24
 */

static char *pointsize = "pointsize";
static char *curparse;

readfonttable(file)
char *file;
{
	FILE *fp;
	int size, texsize;
	char *s1, *s2, *s3, *s4, *s5, *s;
	extern char *strsave();
	char buf[MAXSTR*4];
	char fbuf[MAXSTR];
	int nlines = 0;
	struct fontinfo *font = &availfonts[navailfonts];
	
	if ((fp = fopen(file, "r")) == NULL)
		return;
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		nlines++;
		if ((curparse = index(buf, '#')) != NULL)
			*curparse = '\0';
		curparse = buf;
		if ((s1 = getword(&curparse)) == NULL)
			continue;
		if ((s2 = getword(&curparse)) == NULL || (size = atoi(s2)) <= 0)
			goto complain;
		if ((s3 = getword(&curparse)) == NULL || *s3 != '\\')
			goto complain;
		if ((s4 = getword(&curparse)) == NULL)
			goto complain;
		s5 = getword(&curparse);
		if (navailfonts == nfonts) {
			(void) fprintf(stderr, "Too many fonts - max %d. Use -f to raise it\n",
			 nfonts);
			exit(-1);
		}
		if (s5 == NULL) {
			s = strsave(s4);
		} else if (STRNEQ(s5, pointsize, 9)) {
			s5 += 9;
			if ((texsize = atoi(s5)) == 0)
				goto complain;
			(void) sprintf(fbuf, "%s at %dpt", s4, texsize);
			s = strsave(fbuf);
		} else {
			(void) sprintf(fbuf, "%s scaled %s", s4, s5);
			s = strsave(fbuf);
		}
#ifdef DEBUG
		(void) fprintf(stderr, "Adding mapping %s, %d, %s, %s\n", s1, size,
		 s3, s);
#endif
		font->style = strsave(s1);
		font->size = size;
		font->texname = strsave(s3);
		font->texfontinit = s;
		font->refcnt = 0;
		font++;
		navailfonts++;
		continue;
complain:
		(void) fprintf(stderr,
		 "Bad syntax on line %d, file %s somewhere at \"%s\"\n",
		 nlines, file, curparse);
	}
	(void) fclose(fp);
}


main(argc, argv)
int argc;
char **argv;
{
	double atof();
	int c;
	char fontfile[MAXSTR];
	char *hdir;
	char *getenv();
	char tempfilename[MAXSTR];
	
	progname = argv[0];
	inFile = stdin;
	(void) sprintf(tempfilename, "%s/x2tpic%d", DUMPDIR, getpid());
	if ((outFile = fopen(tempfilename, "w+")) == NULL) {
		(void) fprintf(stderr, "Can't open %s for update\n", tempfilename);
		exit(-1);
	}
#ifndef DEBUG
	(void) unlink(tempfilename); /* So file goes away when the program dies */
#endif
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
	availfonts = (struct fontinfo *) calloc((unsigned) nfonts,
	 sizeof(struct fontinfo));
	if (availfonts == NULL) {
		(void) fprintf(stderr, "Can't allocate font table of %d elements.\n",
		 nfonts);
		exit(-1);
	}
	navailfonts = 0;
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
			convert();
			optind++;
		}
	}
	exit(0);
}
