/* $Header: x2ps.c,v 1.8 89/07/02 15:59:43 moraes Exp $ */
#include <X11/Xos.h>
#include <stdio.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/param.h>
#include "defs.h"
#include "tune.h"
/*
 *  This simply reads in xpic format, and emits it (slightly massaged)
 *  with a tagged on PostScript prolog that does most of the work. The
 *  only real work done by this program is for splines which are
 *  converted to bezier curves by some gory math, and arrows which are
 *  computed here by essentially the same algorithm used by xpic
 */
#define MAXFONTS 127
#define FONTDESCFILE "x2ps"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#define PROLOGUE	"x2ps.pro"
#define TRAILER		"x2ps.tra"

/* PostScript (well, typesetting) constant. Should it be 72.27? */
#define PPI		72.0	/* 72 (big) points per inch */

#define xtransform(x)	((x) - orgX)
#define ytransform(y)	((orgY - (y)))

extern char *malloc();

static int orgX, orgY, picWidth, picHeight;

static int x, y;

static char *halign_words[] = {
	"ce", "lj", "rj"
};

static char *progname;
static int arrow_type = -1;
static int curStyle = -1;
static char *curFont = NULL;
static int curSize = -1;
static int curWidth = -1;
static int halign, valign;
static int showPage = TRUE;
static int flipcoordinates = FALSE;
static double hoffset = 0;
static double voffset = 0;
#ifdef DEBUG
static char *prologueFile = PROLOGUE;
static char *trailerFile = TRAILER;
#else DEBUG
static char *prologueFile = NULL;
static char *trailerFile = NULL;
#endif DEBUG

FILE *outFile;
FILE *inFile;

extern int optind;
extern char *optarg;

static void ChangeThickness(t)
{
	if (curWidth != t) {
		curWidth = t;
		if (curWidth == 0)
			(void) fprintf(outFile, "0.5 W\n");
		else
			(void) fprintf(outFile, "%d W\n", curWidth);
	}
}

	
static void ChangeStyle(style)
{
	if ((style < 0) || (style >= NSTYLES)) {
		(void) fprintf(stderr, "Unknown style - %d\n", style);
		style = 0;
	}
	if (style != curStyle) {
		(void) fprintf(outFile, "%d D\n", style);
		curStyle = style;
	}
}

static void ChangeFont(font)
char *font;
{
	extern char *HashSearch();
	char *psfont = HashSearch(font);
	if (psfont == NULL) {
		(void) fprintf(stderr, "Unavailable font %s\n", font);
		return;
	}
	if (psfont != curFont) {
		(void) fprintf(outFile, "/%s F\n", psfont);
		curFont = psfont;
	}
}

static void ScaleFont(size)
{
	if (size <= 0) {
		(void) fprintf(stderr, "Incorrect font size %d\n", size);
		size = 10;
	}
	if (size != curSize) {
		(void) fprintf(outFile, "%d S\n", size);
		curSize = size;
	}
}


static void MakeBox(x1, y1, x2, y2, attr)
{
	ChangeStyle(getlinestyle(attr));
	x = xtransform(x1);
	y = ytransform(y1);
	(void) fprintf(outFile, "%d %d ", x, y);
	x = xtransform(x2);
	y = ytransform(y2);
	(void) fprintf(outFile, "%d %d b\n", x, y);
}


static void MakeEllipse(xc, yc, xr, yr, attr)
{
	ChangeStyle(getlinestyle(attr));
	x = xtransform(xc);
	y = ytransform(yc);
	(void) fprintf(outFile, "%d %d %d %d e\n", xr, yr, x, y);
}


/*
 *  print out a string, escaping '(' and ')' with a \ - we don't escape \, to
 *  give people a mechanism for sending strange numbers to the lw
 */
static void PrintText(s)
char *s;
{
	register char *cp = s;

	for(; *cp != 0; cp++) {
		if (isascii(*cp) && isprint(*cp)) {
 			if (*cp != '(' && *cp != ')')
		 		(void) fputc(*cp, outFile);
			else
				(void) fprintf(outFile, "\\%c", *cp);
		} else {
			/* Ok - but we're only printing the lower byte! */
			(void) fprintf(outFile, "\\%o", *cp & 0xff);
		} 
	}
}
			
/*ARGSUSED*/
static void MakeText(s, len, font, size, attr, xc, yc)
char *s;
char *font;
{
	ChangeFont(font);
	ScaleFont(size);
	halign = gettext_halign(attr);
	valign = gettext_valign(attr);
	if ((valign < 0) || (valign >= 3)) {
		(void) fprintf(stderr, "Incorrect vert alignment %d\n", valign);
		valign = 0;
	}
	if ((halign < 0) || (halign >= 3)) {
		(void) fprintf(stderr, "Incorrect horiz alignment %d\n", halign);
		halign = 0;
	}
	x = xtransform(xc);
	y = ytransform(yc);
	(void) fprintf(outFile, "%d %d m\n(", x, y);
	PrintText(s);
	(void) fprintf(outFile, ") %s %d t\n", halign_words[halign], valign);
}
	

#define PI 			3.14159265358979323846264338327950288419716939937511
#define ARROWSIZE	10.
#define ARROWANGLE	0.46

#define round(x)	((int) ((x) + 0.5))

/*
 *  Draws an arrowhead on the end of the line going from x1, y1 to x2, y2.
 *  It calculates the direction, and draws two lines ARROWSIZE long at
 *  angles of ARROWANGLE to the line.
 */
static void DrawArrow (x1, y1, x2, y2) 
int x1, y1, x2, y2; 
{
	double dir, dx, dy, atan2(), cos(), sin();

	dx = (double) (x2 - x1);
	dy = (double) (y2 - y1);
	if (dx == 0. && dy == 0.) 
		return;
	dir = atan2(dy, dx) + PI;

	/* Arrows should be solid lines, say the critics */
	(void) fprintf(outFile, "0 D\n");
	x = x2 + round(ARROWSIZE * cos (dir - ARROWANGLE));
	y = y2 + round(ARROWSIZE * sin (dir - ARROWANGLE));
	(void) fprintf(outFile, "%d %d m\n", x, y);
	(void) fprintf(outFile, "%d %d l\n", x2, y2);
	x = x2 + round(ARROWSIZE * cos (dir + ARROWANGLE));
	y = y2 + round(ARROWSIZE * sin (dir + ARROWANGLE));
	(void) fprintf(outFile, "%d %d l\n", x, y);
	(void) fprintf(outFile, "%d D\n", curStyle);
	(void) fprintf(outFile, "%d %d m\n", x2, y2);
}

static int oldX, oldY;
static int lastX, lastY;
static BOOL firstpoint = FALSE;

/*ARGSUSED*/
static void StartLine(xc, yc, n, attr)
{
	int x, y;
	
	arrow_type = getlinearrow(attr);
	if ((arrow_type < 0) || (arrow_type >= 4)) {
		(void) fprintf(stderr, "Unknown arrow type %d\n", arrow_type);
		arrow_type = 0;
	}
	ChangeStyle(getlinestyle(attr));
	x = xtransform(xc);
	y = ytransform(yc);
	(void) fprintf(outFile, "%d %d m\n", x, y);
	firstpoint=TRUE;
	oldX = lastX = x;
	oldY = lastY = y;
}


static void NextLinePointAt(xc, yc)
{
	int x, y;
	
	x = xtransform(xc);
	y = ytransform(yc);
	if (firstpoint) {
		if (arrow_type & 0x1) DrawArrow(x, y, oldX, oldY);
		firstpoint = FALSE;
	}
	(void) fprintf(outFile, "%d %d l\n", x, y);
	oldX = lastX;
	oldY = lastY;
	lastX = x;
	lastY = y;
}


static void EndLine()
{
	if (arrow_type & 0x2) DrawArrow(oldX, oldY, lastX, lastY);
	(void) fprintf(outFile, "x\n");
}

/*
 *  This takes a set of points for a quadratic B-Spline (passing thru'
 *  all the points and lying within their convex hull, and produces a
 *  PostScript curve using Bezier segments. Mark Moraes, 1987.
 */
/*ARGSUSED*/
StartSpline(xc, yc, n, attr)
{
	arrow_type = getlinearrow(attr);
	if ((arrow_type < 0) || (arrow_type >= 4)) {
		(void) fprintf(stderr, "Unknown arrow type %d\n", arrow_type);
		arrow_type = 0;
	}
	ChangeStyle(getlinestyle(attr));
	x = xtransform(xc);
	y = ytransform(yc);
	(void) fprintf(outFile, "%d %d m\n", x, y);
	firstpoint=TRUE;
	oldX = lastX = x;
	oldY = lastY = y;
}


static void NextSplinePointAt(xc, yc, lastpoint)
{
	int x, y;
	double dX, dY, dX2, dY2;
	
	x = xtransform(xc);
	y = ytransform(yc);
	if (firstpoint) {
		if (arrow_type & 0x1) DrawArrow(x, y, oldX, oldY);
		firstpoint = FALSE;
	}
	dX = x - oldX;
	dY = y - oldY;
	if (lastpoint) {
		dX += x - lastX;
		dY += y - lastY;
	}
	dX2 = lastX - oldX;
	dY2 = lastY - oldY;
	(void) fprintf(outFile, "%g %g ", dX2 / 3., dY2 / 3.);
	(void) fprintf(outFile, "%g %g ", (2. * dX2 + dX) / 6.,
	 (2. * dY2 + dY)/ 6.);
	(void) fprintf(outFile, "%g %g s\n", dX / 2., dY / 2.);
	oldX = lastX;
	oldY = lastY;
	lastX = x;
	lastY = y;
}


void EndSpline()
{
	if (arrow_type & 0x2) DrawArrow(oldX, oldY, lastX, lastY);
	(void) fprintf(outFile, "x\n");
}

	 
/*
 *  Puts a PostScript prologue from the file 'prologue' to stdout
 *  preceded by a header comment according to Adobe's Structuring
 *  Conventions. 'filename' is the name of the xpic file, and the four
 *  values denote the bounding box of the figure
 */
WritePrologue(prologue, filename, llx, lly, urx, ury)
char *prologue;
char *filename;
double llx, lly, urx, ury;
{
	FILE *f;
	char hostname[MAXHOSTNAMELEN];
	struct passwd *pw;
	extern time_t time();
	time_t clock = time((time_t *) 0);
	char s[BUFSIZ];

	if ((f = fopen(prologue, "r")) == NULL) {
		(void) fprintf(stderr, "Cannot open prologue file %s\n", prologue);
		exit(-1);
	}

	/* The comments section - according to Appendix C in the Red Book */
	(void) fprintf(outFile, "%%!\n");
	(void) gethostname(hostname, sizeof(hostname));
	if ((pw = getpwuid(getuid())) == NULL) 
		(void) fprintf(stderr, "Cannot get passwd info!");
	else
		(void) fprintf(outFile, "%%%%Creator: %s@%s (%s)\n", pw->pw_name,
 		 hostname, pw->pw_gecos);
	if (filename != NULL)
		(void) fprintf(outFile, "%%%%Title: %s (xpic)\n", filename);
	(void) fprintf(outFile, "%%%%CreationDate: %s", ctime(&clock));
	/* Only one page pictures allowed */
	(void) fprintf(outFile, "%%%%Pages: 1\n");
#if 0
	/* !! Ouch. */
	(void) fprintf(outFile, "%%%%DocumentFonts: (atend)\n");
#endif
	(void) fprintf(outFile, "%%%%BoundingBox: %g %g %g %g\n", llx, lly,
	 urx, ury);
	(void) fprintf(outFile, "%%\t(in inches) at %g %g, width %g, height %g\n",
	 llx / PPI, lly / PPI, (urx - llx) / PPI, (ury - lly) / PPI);
	(void) fprintf(outFile, "%%%%EndComments\n");

	/* Now copy the prologue - stdio should be fast enough */
	while(fgets(s, sizeof(s), f) != NULL)
		fputs(s, outFile);

	(void) fclose(f);

	(void) fprintf(outFile, "%%%%EndProlog\n");
}

/*
 *  Put a PostScript trailer on stdout from the file 'trailer' as per
 *  the Structuring Conventions. 
 */
WriteTrailer(trailer, showpage)
char *trailer;
{
	FILE *f;
	char s[BUFSIZ];

	if ((f = fopen(trailer, "r")) == NULL) {
		(void) fprintf(stderr, "Cannot open trailer file %s\n", trailer);
		exit(-1);
	}

	(void) fprintf(outFile, "%%%%Trailer\n");
	if (showpage)
		(void) fprintf(outFile, "showpage\n");

	/* Now copy the trailer - stdio should be fast enough */
	while(fgets(s, sizeof(s), f) != NULL)
		fputs(s, outFile);

	(void) fclose(f);
}



/*
 *  Read in a file output by xpic, and emit the appropriate PostScript text
 */
static void convert(land, scale, infilename)
double scale;
char *infilename;
{
	int type;
	int xc, yc, xr, yr, len, attr;
	int size;
	int x1, y1, x2, y2;
	char *s;
	char font[MAXSTR];
	int c, i, n;
	int err, nf, gs;
	int num, thickness;
	int xoff, yoff;
	double xscale = scale;	/* Allow for grid spacing as well */

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
	/* Take advantage of the already mirrored coordinates if rotating */
	orgY = (land) ? y1 : y2;
	picWidth = x2 - x1;
	picHeight = y2 - y1;
	/* Convert x and y offsets to xpic units */
	xoff = hoffset * 10.0 * gs;
	yoff = voffset * 10.0 * gs;
	orgX -= xoff;
	orgY += yoff;
	/*
	 *  We need to tell psfig the correct bounding box in points (72
	 *  points = 1 inch) and xpic has gridspacing * 10 pixels per
	 *  inch, hence the xscale bit. We also have a possibly
	 *  user-specified scale to deal with. The actual scaling is
	 *  done in PostScript - we need to calculate this here only for
	 *  the bounding box.
	 */
	xscale *= PPI / (double) (gs * 10.0);
	if (land) {
		WritePrologue(prologueFile, infilename,
 		 (double) (hoffset * PPI), (double) (voffset * PPI),
		 (double) (xscale * picHeight), (double) (xscale * picWidth));
		(void) fprintf(outFile, "land\n");
	} else {
		WritePrologue(prologueFile, infilename,
 		 (double) (hoffset * PPI), (double) (voffset * PPI),
		 (double) (xscale * picWidth), (double) (xscale * picHeight));
	}
	if ((int) scale != 1)
		(void) fprintf(outFile, "%lg %lg scale\n", scale, scale);
	(void) fprintf(outFile, "%d ss\n", gs*10);
	if (flipcoordinates) {
		/*
		 * Usually, we move the picture so that the lower left corner
		 * of the bounding box is at the PostScript page origin (lower
		 * left corner). If the user specifies the flipcoordinates
		 * (-w) option, they want the picture as it appeared on the
		 * screen, so we get PostScript to perform the flip since it
		 * knows the real pagesize. So we translate the picture to teh
		 * origin in x2ps, and PostScript translates it back if this
		 * option is set.
		 */
		(void) fprintf(outFile, "getpagesize %d %d flip\n", x1, y2);
	}
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
			MakeEllipse(xc, yc, xr, yr, attr);
			break;
		case CIRCLE:
			nf = fscanf(inFile, " %d %d %d", &xc, &yc, &xr);
			if (nf != 3) {
				err = INPERR;
				break;
			}
			MakeEllipse(xc, yc, xr, xr, attr);
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
			MakeText(s, len, font, size, attr, xc, yc);
			free(s);
			break;
		case LINE:
			if (fscanf(inFile, " %d %d %d", &n, &xc, &yc) != 3) {
				err = INPERR;
				break;
			}
			StartLine(xc, yc, n, attr);
			for (i = 1; i < n; i++) {
				if (fscanf(inFile, " %d %d", &xc, &yc) != 2) {
					err = INPERR;
					break;
				}
				NextLinePointAt(xc, yc);
			}
			if (err != INPERR)
				EndLine();
			break;
		case SPLINE:
			if (fscanf(inFile, " %d %d %d", &n, &xc, &yc) != 3) {
				err = INPERR;
				break;
			}
			StartSpline(xc, yc, n, attr);
			for (i = 1; i < n; i++) {
				if (fscanf(inFile, " %d %d", &xc, &yc) != 2) {
					err = INPERR;
					break;
				}
				NextSplinePointAt(xc, yc, (i == n - 1));
			}
			if (err != INPERR)
				EndSpline();
			break;
		}
	} while (err == 0);
	WriteTrailer(trailerFile, showPage);
	if (err == INPERR)
		(void) fprintf(stderr, "Incorrect input format");

	return;

#undef INPEOF
#undef INPERR
}

usage(s)
char *s;
{
	(void) fprintf(stderr, "\
Usage: %s [-w] [-r] [-s scale] [-p prologuefile] [-t trailerfile] \n\
	[-h hoffset] [-v voffset] [-x] [-f maxfonts] [filename]\n", s);
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
	int land = FALSE;
	double scale = 1.0;
	double atof();
	int c;
	int nfonts = MAXFONTS;
	char fontfile[MAXSTR];
	char *hdir;
	char *getenv();
	
	progname = argv[0];
	inFile = stdin;
	outFile = stdout;
	while((c = getopt(argc, argv, "wrs:t:p:xf:h:v:")) != EOF) {
		switch (c) {
		case 'w':
			flipcoordinates = TRUE;
			break;
		case 'r':
			land = TRUE;
			break;
		case 's':
			scale = atof(optarg);
			break;
		case 'p':
			prologueFile = optarg;
			break;
		case 't':
			trailerFile = optarg;
			break;
		case 'x':
			/* Pronounce this 'Ech' to rhyme with TeX! */
			showPage = !showPage;
			break;
		case 'f':
			nfonts = atoi(optarg);
			break;
		case 'h':
			hoffset = atof(optarg);
			break;
		case 'v':
			voffset = atof(optarg);
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
	if (!prologueFile) {
		prologueFile = malloc(sizeof(PROLOGUE) + sizeof(LIBDIR));
		(void) sprintf(prologueFile, "%s/%s", LIBDIR, PROLOGUE);
	}

	if (!trailerFile) {
		trailerFile = malloc(sizeof(TRAILER) + sizeof(LIBDIR));
		(void) sprintf(trailerFile, "%s/%s", LIBDIR, TRAILER);
	}

	/* Must have some files */
	if (optind >= argc) {
		convert(land, scale, "stdin");
	} else {
		while (optind < argc) {
			if ((inFile = fopen(argv[optind], "r")) == NULL) {
				(void) fprintf(stderr, "Can't open %s for reading\n", 
				 argv[optind]);
				break;
			}
			outFile = stdout;
			convert(land, scale, argv[optind]);
			optind++;
		}
	}
}
