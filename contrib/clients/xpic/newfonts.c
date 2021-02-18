#ifndef lint
static char *rcsid = "$Header: newfonts.c,v 1.6 89/07/02 15:58:37 moraes Exp $";
#endif
/*
 *  This reads in a font map, which describes what xpic font names (the
 *  ones shown on the buttons) map onto which X font names. It
 *  XListFonts() the server to get that information, and uses fonts of
 *  different scales by recomputing their effective point size on the
 *  screen given the screen resolution. It prepares tables of these and
 *  will step through the appropriate table for the +/- buttons.
 */
#include <ctype.h>
#include "xpic.h"
#include "windows.h"
#include "tune.h"
#include "assert.h"
#include "newfonts.h"
#include "xfname.h"

XFontStruct *defaultFont;

static FontFamily *availFonts;
static int nAvailFonts;
static int maxAvailFonts;

static char *defaultFontName = NULL;
static int defaultFontSize = 0;

#define MAX_XFONTS 		1024
#define LINESIZE		128
#define MAXFILENAME		256
/* We allocate the availFonts array in chunks of FAMILY_CHUNK */
#define FAMILY_CHUNK	16	/* !! */
/* We allocate the availFonts[i].sizes array in chunks of SIZE_CHUNK */
#define SIZE_CHUNK		32	/* !! */

void readfontfile(filename)
char *filename;
{	
	FILE *fp;
	char line[LINESIZE];
	char pattern[LINESIZE];
	char fullname[MAXSTR];
	char **fontnames;
	char **cp;
	int nf;
	int i;
	FontFamily *fptr, *findfont();
	void addfont();

#define parse_error() \
	(void) fprintf(stderr, "xpic: Bad line in fontdesc file %s\n\t%s\n", \
	 filename, line) \

	if ((fp = fopen(filename, "r")) == NULL) 
		return;
	while(fgets(line, LINESIZE, fp) != NULL) {
		if (sscanf(line, " %s", fullname) != 1) {
			parse_error();
			continue;
		} else if (strcmp(fullname, "default") == 0) {
			if (sscanf(line, " %*s %s %d", fullname, &defaultFontSize) != 2) {
			 	parse_error();
			} else {
				defaultFontName = strcpy(XtMalloc((unsigned)
				 (strlen(fullname) + 1)), fullname);
			}
			continue;
		}
		/* else */
		i = sscanf(line, " %s %[^\n]\n", fullname, pattern);
		if (i == 2) {
			/* must be an X11R[34] pattern */
			fontnames = XListFonts(picDpy, pattern, MAX_XFONTS, &nf);
			if (nf == 0) {
#ifdef DEBUG
				(void) fprintf(stderr, "Warning: No fonts found for %s\n",
				 prefix);
#endif
				continue;
			}
			fptr = findfont(fullname, FALSE);
			cp = fontnames;
#ifdef DEBUG
			fprintf(stderr, "%s %s\n", fullname, pattern);
#endif
			while(--nf >= 0) {
				char **fn;

				fn = getfontname(*cp);
#ifdef DEBUG
				fprintf(stderr, "	%s %s %s\n", *cp, fn[PIXEL_SIZE], fn[RESOLUTION_Y]);
#endif
				if (fn[PIXEL_SIZE] == NULL || fn[RESOLUTION_X] == NULL)
				    continue;
				addfont(fptr, strsave(*cp),
				 atoi(fn[PIXEL_SIZE]), atoi(fn[RESOLUTION_Y]));
				cp++;
			}
			XFreeFontNames(fontnames);
			fontnames = NULL;
			continue;
		}
	 	parse_error();
	}
}


/*
 *  Finds the entry for font family - makes a new one if necessary.
 *  Straight linear search for now.
 */
FontFamily *findfont(fullname, return_null)
char *fullname;
{
	register FontFamily *fptr;
	register int i;
	char *name = XtMalloc((unsigned) (strlen(fullname) + 1));

	(void) strcpy(name, fullname);
	for (i = 0, fptr = availFonts; i < nAvailFonts; i++, fptr++)
		if(STREQ(name, fptr->name))
			break;
	if (i == nAvailFonts && !return_null) {
		/* Make sure there's enough space */
		if (nAvailFonts == maxAvailFonts) {
			/* No - we need to grow the table */
			maxAvailFonts += FAMILY_CHUNK;
			availFonts = (FontFamily *) XtRealloc((char *) availFonts,
			 (unsigned) (maxAvailFonts * sizeof(FontFamily)));
#ifdef DEBUG
			printf("realloced font families' table to %d\n", maxAvailFonts);
#endif
			for (i = nAvailFonts; i < maxAvailFonts; i++) {
				availFonts[i].nsizes = 0;
				availFonts[i].maxsizes = SIZE_CHUNK;
				availFonts[i].sizes = (FontSizes *)XtCalloc(sizeof(FontSizes), 
				 SIZE_CHUNK);
				availFonts[i].name = NULL;
				availFonts[i].cursize = 0;
			}
			/*
			 *  Make sure fptr points to realloc'ed table to same place
			 *  it pointed to in old table.
			 */
			fptr = availFonts + nAvailFonts;
		}
		/* Isn't there yet, so add it */
		fptr->name = name;
		nAvailFonts++;
#ifdef DEBUG
		printf("Added font family %s\n", name);
#endif
	} else if (i == nAvailFonts && return_null) {
		return(NULL);
	}
	return(fptr);
}


static void addfont(fptr, fontname, fontsize, dpi)
FontFamily *fptr;
char *fontname;
int fontsize, dpi;
{
	int realsize = (8. * dpi * fontsize)/(75. * gridSpacing) + .5;
#if 0
	/*
	 * This is the exact font calculation - the above fudge gives better
	 * results since the fonts are 75 dpi and the xpic grid is 80 dpi. So
	 * we just fake it to pretend both are the same
	 */
	int realsize = (dpi * fontsize)/(10. * gridSpacing) + .5;
#endif
	register int i;
	register FontSizes *sptr;
	register FontSizes *sp;

	/*
	 *  Find the place for inserting the fontname by pointsize - the
	 *  table is ordered by pointsize
	 */
	for(i = 0; i < fptr->nsizes && fptr->sizes[i].pointsize < realsize; i++)
	 	;
	if (i < fptr->nsizes && fptr->sizes[i].pointsize == realsize) {
		/* Duplicate - ignore */
#ifdef DEBUG
		printf("duplicate - %s and %s are both %d in size in family %s\n", 
		 fptr->sizes[i].xfontname, fontname, realsize, fptr->name);
#endif DEBUG		 
		return;
	} 
	/* Reallocate if we're overrunning */
	if (fptr->nsizes == fptr->maxsizes) {
		fptr->maxsizes += SIZE_CHUNK;
		fptr->sizes = (FontSizes *) XtRealloc((char *) fptr->sizes,
		 (unsigned) (fptr->maxsizes * sizeof(FontSizes)));
#ifdef DEBUG
		printf("reallocing sizes for family %s to %d\n", fptr->name, 
		 fptr->maxsizes);
#endif
	}
	sptr = &fptr->sizes[i];
	/* Now insert the FontSize info */
	if (sptr->pointsize == 0) {
		/* add at the end */
		sptr->pointsize = realsize;
		sptr->xfontname = fontname;
		sptr->font = NULL;
} else {
		sp = fptr->sizes + fptr->nsizes;
		bcopy((char *)sptr, (char *)(sptr+1), (sp - sptr)*sizeof(FontSizes));
		sptr->pointsize = realsize;
		sptr->xfontname = fontname;
		sptr->font = NULL;
	}
	fptr->nsizes++;
#ifdef DEBUG
	printf("added %s - pointsize %d to family %s\n", fontname, realsize,
	 fptr->name);
#endif
}

/*
 *  Changes the font, and opens the new font if it hasn't been used yet.
 *  On error, if a font can't be opened, it doesn't change the font.
 *  Internally, the font changed because the button was
 *  updated, the real font size and name will be stored with the Gel
 */
XFontStruct *ChangeFont(fontsize, pad)
FontSizes *fontsize;
int *pad;
{
	XFontStruct *textfont;

	*pad = 0;

	if (fontsize->font == NULL) {
		if ((fontsize->font = XLoadQueryFont(picDpy, fontsize->xfontname)) 
		 == NULL) {
		 	(void) sprintf(errstring, "Can't find font %s",
			 fontsize->xfontname);
			message(errstring);
			/* So we don't go thru this again... */
			fontsize->font = defaultFont;
		}
	}
	textfont = fontsize->font;

	/*
	 *  Compute the new width of the space character - It is assumed that
	 *  the space character does not exist in the fonts we are using, i.e.
	 *  it is zero width. This is true of vfonts, and of some of the X
	 *  proportional width fonts. The space width is taken to be the Troff
	 *  width of 12/36 ems.
	 */
	*pad = XTextWidth(textfont, " ", 1);
	if (*pad == 0)
		*pad = XTextWidth(textfont, "m", 1) / 3;

	return(textfont);
}

/* finds the index of the closest pointsize in FontFamily fptr. */
int findsize(fptr, realsize)
FontFamily *fptr;
int realsize;
{
	register int i;
	
	/* find pointsize */
	for(i = 0; i < fptr->nsizes && fptr->sizes[i].pointsize < realsize; i++)
	 	;
	if (i >= fptr->nsizes) 
		/* largest size available */
		i = fptr->nsizes - 1;
	return(i);
		
}


FontFamily *InitFonts(default_fontname, default_fontsize, n_availfonts)
char **default_fontname;
int *default_fontsize;
int *n_availfonts;
{
	register int i;
	char filename[MAXFILENAME];
	
	maxAvailFonts = FAMILY_CHUNK;
	nAvailFonts = 0;
	availFonts = (FontFamily *) XtCalloc(sizeof(FontFamily), FAMILY_CHUNK);
	for (i = 0; i < FAMILY_CHUNK; i++) {
		availFonts[i].nsizes = 0;
		availFonts[i].maxsizes = SIZE_CHUNK;
		availFonts[i].sizes = (FontSizes *) XtCalloc(sizeof(FontSizes), 
		 SIZE_CHUNK);
		availFonts[i].name = NULL;
	}

	(void) sprintf(filename, "%s/fontdesc/xpic", LIBDIR);
	readfontfile(filename);
	(void) sprintf(filename, "%s/.xpic", homeDir);
	readfontfile(filename);
	*default_fontname = defaultFontName;
	*default_fontsize = defaultFontSize;
	*n_availfonts = nAvailFonts;
	return(availFonts);
}

#ifdef TEST
main(argc, argv)
char **argv;
{
	Widget toplevel = XtInitialize("testfont", "TestFont", NULL, 0, 
	 &argc, argv);

	picDpy = XtDisplay(toplevel);

	InitFonts();
}

Dying()
{
	abort();
}
#endif
