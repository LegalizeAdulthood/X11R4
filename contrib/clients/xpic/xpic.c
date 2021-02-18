/* $Header: xpic.c,v 1.6 89/07/02 16:00:06 moraes Exp $ */
/*
 *  This file contains lots of basic routines which manipulate the data
 *  and create/free/read/write various structures
 */

#include <strings.h>
#include <sys/types.h>
#include <sys/param.h>
#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN	PATH_MAX
# else
   /* If you haven't got either MAXPATHLEN or PATH_MAX defined, ouch! */
#  define MAXPATHLEN    256
# endif
#endif
#include <sys/stat.h>
#include <stdio.h>
#include <values.h>

#include "xpic.h"
#include "windows.h"
#include "input.h"
#include "gels.h"
#include "tune.h"
#include "newfonts.h"
#include "assert.h"

#define round(x)	((int) ((x) + 0.5))
#define xfree(x)	if (x) free(x); else

/* 
 *  basic bounding box manipulation routines (may write them later as
 *  macros if I think the speed improvement warrants it.) Note that
 *  'contains' means 'p' lying within or on the bounding box 'clip', hence
 *  'intersects' means 'b' intersecting or touching 'clip'.
 */

BOOL containsXY(x, y, clip)
register int x, y;
register Box *clip;
{
	return((x >= clip->ll.x) && (x <= clip->ur.x) &&
	 (y >= clip->ll.y )&& (y <= clip->ur.y));
}

BOOL contains(p, clip)
register Point *p;
register Box *clip;
{
	return((p->x >= clip->ll.x) && (p->x <= clip->ur.x) &&
	 (p->y >= clip->ll.y )&& (p->y <= clip->ur.y));
}

/* 
 *  If two boxes intersect, then the area of intersection must be
 *  positive, i.e. ur.x of the intersection > ll.x AND ur.y > ll.y where
 *  ur.x of the intersection is the minimum of the ur.x of the two
 *  boxes, and the ll.x is the maximum of the ll.x of the two boxes
 *  (similarly for y). (Assuming ll.x < ur.x, ll.y < ur.y ALWAYS for all
 *  boxes.)
 */
BOOL intersects(b, clip)
register Box *b;
register Box *clip;
{
	return ( (MAX(b->ll.x, clip->ll.x) <= MIN(b->ur.x, clip->ur.x)) &&
		 (MAX(b->ll.y, clip->ll.y) <= MIN(b->ur.y, clip->ur.y)) );
}


/* Returns TRUE if box 'b' lies ENTIRELY within 'clip' */
BOOL within(b, clip)
register Box *b;
register Box *clip;
{
	return(contains(&(b->ll), clip) && contains(&(b->ur), clip));
}


/*
 *  Frees the vertex buffer of a pointlist, and the pointlist itself
 */
void FreePtList(pt)
register PointList *pt;
{
	if (!pt)
		return;
	xfree((char *) pt->v);
	free((char *) pt);
}


/*
 *  Takes a XPoint array, mallocs space for N points, and copies the first
 *  N points from the given XPoint array to the PointList
 */
PointList *NewPtList(vertices, n)
register XPoint *vertices;
int n;
{
	PointList *thisPtList;
	
	if ((thisPtList = (PointList *) malloc(sizeof(PointList))) == NULL) {
		message("NewPointList: Can't get memory for new pointlist element");
		return( (PointList *) NULL);
	}

	if ((thisPtList->v = (XPoint *) calloc((unsigned) n, sizeof(XPoint)))
	 == NULL) {
		message("NewPointList: Can't get memory for new vertex list");
		free((char *) thisPtList);
		return( (PointList *) NULL);
	}

	thisPtList->nVerts = n;

	(void) bcopy((char *) vertices, (char *) (thisPtList->v),
	 (int) (n * sizeof(XPoint)));
	 
	return(thisPtList);
}


/* Reads in a pointlist from the current inFile */
PointList *ReadPtList(scale, type)
double scale;
int type;
{
	PointList *thisPtList;
	register XPoint *tmp;
	register int i, n;
	int x, y;
	
	if ((thisPtList = (PointList *) malloc(sizeof(PointList))) == NULL) {
		message("Can't get memory for new pointlist");
		return( (PointList *) NULL);
	}

	if (fscanf(inFile, " %d", &(thisPtList->nVerts)) != 1) {
		free((char *) thisPtList);
		return( (PointList *) NULL);
	}

	if (type == SPLINE)
		 thisPtList->nVerts += 2;

	n = thisPtList->nVerts;

	if ((thisPtList->v = (XPoint *) calloc((unsigned) n, sizeof(XPoint)))
	 == NULL) {
		message("Can't get memory for new vertex list");
		free((char *) thisPtList);
		return( (PointList *) NULL);
	}

	tmp = thisPtList->v;
	if (type == SPLINE) {
		tmp++;
		n -= 2;
	}

	for (i = 0; i < n; i++, tmp++) {
		if (fscanf(inFile, " %d %d", &x, &y) != 2) {
			FreePtList(thisPtList);
			return((PointList *) NULL);
		}
		tmp->x = round(x * scale);
		tmp->y = round(y * scale);
	}
	return(thisPtList);
}


/* writes out a pointlist to the current outFile */
void WritePtList(pt, type)
PointList *pt;
int type;
{
	register int i, j, n;
	register XPoint *vert;
	
	vert = pt->v;
	n = pt->nVerts;
	if (type == SPLINE) {
		vert++;
		n -= 2;
	}

	(void) fprintf(outFile, "%d\n", n);

	for (i = 0, j = 1; i < n; i++, vert++, j++) {
		(void) fprintf(outFile, " %d %d", vert->x, vert->y);
		if (j == 4) {
			j = 0;
			(void) fputc('\n', outFile);
		}
	}
	if (j != 1)
		(void) fputc('\n', outFile);
}


/*
 *  makes a Gel
 */
Gel *NewGel()
{
	Gel *thisGel;
	
	if ((thisGel = (Gel *) malloc(sizeof(Gel))) == NULL) {
		message("NewGel: Can't get memory for new element");
		return( (Gel *) NULL);
	}

	/* Null the pointers, and other fields */
	bzero((char *) thisGel, sizeof(Gel));
	
	thisGel->number = GelCounter++;
	
	return (thisGel);
}


/*
 *  Traverse the linked list, freeing each gel - for some items like line,
 *  may need to traverse sub-lists i.e ptlist
 */
void FreeGel(g)
register Gel *g;
{
	register Gel *tmp = g;

	while (tmp != NULL) {
		switch (tmp->type) {
		case LINE:
		case SPLINE:
			FreePtList( (PointList *) tmp->data);
			break;
		case TEXT:
			{
				TextString *tmp_text = (TextString *) tmp->data;
				if (tmp_text) xfree(tmp_text->str);
				xfree( (char *) tmp_text);
			}
			break;
		case CIRCLE:
		case ELLIPSE:
			xfree(tmp->data);
			break;
		default:
			break;
		}
		tmp = tmp->next;
		free ((char *) g);
		g = tmp;
	}
}


/*
 *  Pushes a list of Gels (g) onto another list of gels (stack). It
 *  returns the number of gels pushed onto the stack.
 */
int PushGel(stack, g)
register Gel *g, **stack;
{
	register Gel *tmp;
	register i = 0;
	
	while(g != NULL) {
		tmp = g->next;
 		g->next = *stack;
		*stack = g;
		g = tmp;
		i++;
	}
	return(i);
}


/*
 *  Pushes the list of gels g onto the stack, AFTER popping off N
 *  elements. The N elements are then pushed back on. This is used by
 *  aborts, when a gel needs to be pushed back onto the stack, without
 *  disturbing the top undo elements. Note that the N returned MAY NOT
 *  be used for undo purposes, since it is not from the top of the
 *  stack. Note also that this does not completely solve the problem,
 *  because if you select one of the top undo elements, and then try an
 *  undo, things get confused, In short - don't undo too long after you
 *  do!
 */
int PushUnderUndo(stack, g, n)
register Gel **stack;
register Gel *g;
register int n;
{
	Gel *tmp;

	tmp = PopGel(stack, n);
	n = PushGel(stack, g);
	(void) PushGel(stack, tmp);
	return(n);
}

	
/*
 *  This pops N gels off the stack and returns a pointer to the list
 */
Gel *PopGel(stack, n)
register Gel **stack;
int n;
{
	register Gel *g, *tmp;
	register int i;

	for(i = 0, g = NULL; i < n && *stack != NULL; i++) {
		tmp = (*stack)->next;
		(*stack)->next = g;
		g = *stack;
		*stack = tmp;
	}
	return(g);
}


/*
 *  Counts the number of Gels in a gellist
 */
int CountGel(gel)
register Gel *gel;
{
	register int i = 0;
	
	for (; gel != NULL; gel = gel->next, i++)
		;
	return(i);
}
/* 
 *  This returns the bounding box of a gel list. Since the bounding box is
 *  returned in a static struct, it must not be modified or changed.
 */
Box *GetBBox(g)
register Gel *g;
{
	static Box b;

	b.ur.x = b.ur.y = 0;
	b.ll.x = b.ll.y = MAXINT;
	while (g != NULL) {
		bigger_box(b, g->b_box);
		g = g->next;
	}
	return(&b);
}
		

/* 
 *  Just traverse the linked list, writing out all elements in the Gel
 *  list to outFile
 */
void WriteGel(g)
register Gel *g;
{
	Conic *conic;
	TextString *text;
	Box *b;

#ifdef MAGIC
	/* Write out the magic string, using the appropriate invocation */
	(void) fprintf(outFile, "#! %s\n", PROGRAMNAME);
#endif MAGIC
	/* First write out Gel bounding box */
	CalcBBox(g, MAXINT);
	b = GetBBox(g);
	(void) fprintf(outFile, "%d %d %d %d %d\n", b->ll.x, b->ll.y,
	 b->ur.x, b->ur.y, gridSpacing);

	/* Now write out the Gel list */
	for (;g != NULL; g = g->next) {
		(void) fprintf(outFile, "%d %d %d %d %d %d %x %d\n", g->type,
		 g->number, g->b_box.ll.x, g->b_box.ll.y,
		 g->b_box.ur.x, g->b_box.ur.y, g->attributes, g->linewidth);
		switch (g->type) {
		case BOX:
			break;
		case ELLIPSE:
			conic = (Conic *) g->data;
			(void) fprintf(outFile, "%d %d %d %d\n", conic->centre.x, 
			 conic->centre.y, conic->xrad, conic->yrad);
			break;
		case CIRCLE:
			conic = (Conic *) g->data;
			(void) fprintf(outFile, "%d %d %d\n", conic->centre.x, 
			 conic->centre.y, conic->xrad);
			break;
		case TEXT:
			text = (TextString *) g->data;
			(void) fprintf(outFile, "%d %d %d %s %d\n%s\n", text->x, text->y,
			 text->length, text->fontname, text->fontsize, text->str);
			break;
		case LINE:
		case SPLINE:
			WritePtList((PointList *) g->data, g->type);
			break;
		}
	}
}


char *
strsave(s)
char *s;
{
	char *s1 = XtMalloc((unsigned) (strlen(s) + 1));

	if (s1)
		(void) strcpy(s1, s);
	return(s1);
}


/*
 *  Read in and make a linked list of Gel items from inFile
 */
Gel *ReadGel()
{
	Gel *g;
	int type;
	int xc, yc, xr, yr, len, attr;
	int size;
	char font[MAXSTR];
	int x1, y1, x2, y2, gs;
	char *s;
	int c;
	int err, nf;
	PointList *ptlist;
	double scale;
	int num, thickness;
	FontFamily *fptr;
	char *fontname;

#define NOMEM 1
#define INPERR 2
#define INPEOF 3

	err = 0;
	g = NULL;
	ASSERT(allock(), "test 1");
#ifdef MAGIC
	/* Check for the magic header that the new xpic puts out */
	if ((c = getc(inFile)) == EOF) {
		message("Incorrect input format");
		return(NULL);
	}
	ASSERT(allock(), "test 2");
	(void) ungetc(c, inFile);
	if (c == '#') {
		/* Magic header - ignore */
		(void) fscanf(inFile, "%*[^\n]");
	}
#endif MAGIC
	/* Read in (and ignore) the gel bounding box */
	if (fscanf(inFile, " %d %d %d %d %d", &x1, &y1, &x2, &y2, &gs) != 5) {
		message("Incorrect input format");
		return(NULL);
	}
	
	if (gs == 0) {
		message("Incorrect input.");
		return(NULL);
	}

	scale = ((double) gridSpacing) / gs;
	
	/* Read in the actual list */
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
		x1 = round(x1 * scale);
		x2 = round(x2 * scale);
		y1 = round(y1 * scale);
		y2 = round(y2 * scale);
		switch (type) {
		case BOX:
			AddBoxGel(&g, x1, y1,x2, y2, attr, thickness);
			break;
		case ELLIPSE:
			nf = fscanf(inFile, " %d %d %d %d", &xc, &yc, &xr, &yr) ;
			if (nf != 4) {
				err = INPERR;
				break;
			}
			xc = round(xc * scale);
			yc = round(yc * scale);
			xr = round(xr * scale);
			yr = round(yr * scale);
			AddConicGel(&g, type, xc, yc, xr, yr, attr, x1, y1, x2, y2, thickness);
			break;
		case CIRCLE:
			nf = fscanf(inFile, " %d %d %d", &xc, &yc, &xr);
			if (nf != 3) {
				err = INPERR;
				break;
			}
			xc = round(xc * scale);
			yc = round(yc * scale);
			xr = round(xr * scale);
			AddConicGel(&g, type, xc, yc, xr, xr, attr, x1, y1, x2, y2, thickness);
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
			xc = round(xc * scale);
			yc = round(yc * scale);
			/* Go to the next line */
			while ((c = fgetc(inFile)) != '\n' && c != EOF)
				;
			if (c == EOF) {
				err = INPERR;
				break;
			}
			s = XtMalloc((unsigned) (len + 2));
			if (fgets(s, len + 1, inFile) == NULL) {
				free(s);
				err = INPERR;
				break;
			}
			s[len] = '\0';
			if ((fptr = findfont(font, TRUE)) == NULL) {
				fontname = strsave(font);
				fptr = defaultFontFamily;
			} else {
				fontname = fptr->name;
			}
			AddTextGel(&g, s, len, fptr, findsize(fptr, size), fontname, size, 
			 attr, xc, yc, x1, y1, x2, y2);
			break;
		case LINE:
		case SPLINE:
			if ((ptlist = ReadPtList(scale, type)) == NULL) {
				err = NOMEM;
				break;
			}
			AddLineGel(&g, type, ptlist, attr, x1, y1, x2, y2, thickness);
			ptlist = NULL;
			break;
		}
	} while (err == 0);
	if (err == NOMEM)
		message("No more memory for elements");
	else if (err == INPERR)
		message("Incorrect input format");

	CalcBBox(g, MAXINT);
	return(g);
#undef NOMEM
#undef INPERR
#undef INPEOF
}


/* 
 *  Creates a new cell, with appropriate filename and name, and giving it
 *  all the default values. It does not open the file, nor does it read
 *  anything in - the cell is empty
 */
Cell *NewCell(cellName, fileName)
char *cellName;
char *fileName;
{
	register Cell *thisCell;
	
	if ((thisCell = (Cell *) malloc(sizeof(Cell))) == NULL) {
		message("NewCell: Can't create cell");
		return( (Cell *)NULL);
	}

	thisCell->name = cellName;
	thisCell->filename = fileName;
	thisCell->gelList = NULL;
	thisCell->saved = NEWFILE;
	thisCell->undo = 0;
	thisCell->undoList = NULL;
	thisCell->next = NULL;
	thisCell->mtime = 0;

	return(thisCell);
}


/* Frees and destroys a cell */
void FreeCell(cell)
Cell *cell;
{
	if (!cell) 
		return;
	xfree(cell->name);
	xfree(cell->filename);
	FreeGel(cell->gelList);
	FreeGel(cell->undoList);
	free((char *) cell);
}


/*
 *  Prints a cell out to lpr
 */
LPrintCell(cell)
Cell *cell;
{
	static char *cmdbuf = NULL;
	char *buf;
	extern char *lprcommand;
	extern char *lprinter;
	extern void SetWorkingCursor(), SetWaitCursor();
	
	if (cmdbuf == NULL) {
		/* format default command */
		cmdbuf = XtMalloc((unsigned)
		 (strlen(lprcommand) + strlen(lprinter) + 1));
		(void) sprintf(cmdbuf, lprcommand, lprinter);
	}
	/* offer user a chance to change default printing command */
	if ((buf = get_input("Print command % ", cmdbuf, FALSE)) == NULL)
		return;
	/* free default command and make new command from user the default */
	XtFree(cmdbuf);
	cmdbuf = buf;
	/* open the pipe - Then write out the cell Gels. */
	if ((outFile = popen(buf, "w")) == NULL) {
		message("Can't open pipe for print");
	} else {
		SetWaitCursor();
		WriteGel(cell->gelList);
		(void) sprintf(errstring, "Print Complete");
		message(errstring);
		(void) pclose(outFile);
		SetWorkingCursor();
		outFile = NULL;
	}
}


/*
 *  Writes a cell out - closely emulates the Jove save about making
 *  backups, checking if the file has been written to, etc
 */
int
WriteCell(cell, backup)
Cell *cell;
{
	char *fname;
	char buf[MAXPATHLEN];
	struct stat stbuf;
	int retval = 0;
	extern void SetWorkingCursor(), SetWaitCursor();
#ifdef MAGIC
	int fperms;
#endif MAGIC
	
	
	if (!(cell->saved & MODIFIED)) {
		message("No changes need to be written.");
		return 1;
	}

	/* If no file name, ask for one - default is buffer name */
	if (!cell->filename || STREQ(cell->filename, nullfile)) {
		if ((fname = get_input("Save file name ? ", cell->name, TRUE)) == NULL)
			return 0;
		cell->filename = fname;
		cell->saved |= NEWFILE;
	}

	/*
	 *  Check that it's safe to write the file, and make a backup if
	 *  necessary
	 */
	if (!chk_mtime(cell, buf, backup))
		return 0;

	/* 
	 *  open the file - Then write out the cell Gels. Should sort the
	 *  cell gels before doing this
	 */
	if ((outFile = fopen(buf, "w")) == NULL) {
		(void) sprintf(errstring, "Can't open %s for save", buf);
		message(errstring);
		retval = 0;
	} else {
		SetWaitCursor();
		WriteGel(cell->gelList);
		SetWorkingCursor();
		if (fclose(outFile) == 0) {
			cell->saved = SAVED;
			(void) sprintf(errstring, "Wrote %s", buf);
			retval = 1;
		} else {
			(void) sprintf(errstring, "Error writing %s - cell not saved",
			 buf);
			retval = 0;
		}
		message(errstring);
		outFile = NULL;
		if (stat(buf, &stbuf) == -1) {
			(void) sprintf(errstring, "Cannot stat %s", buf);
			message(errstring);
		} else {
			cell->mtime = stbuf.st_mtime;
#ifdef MAGIC
			/*
			 * Extract permissions - we set it to owner execuatble, and make
			 * it group/other executable only if it is group/other readable.
			 * Not sure if it needs to be that elaborate...
			 */
			fperms = stbuf.st_mode & 0777;
			fperms |= 0100;		/* Set owner executable */
			if (fperms & 0040)
				fperms |= 0010;	/* Set group executable */
			if (fperms & 0004)
				fperms |= 0001;	/* Set user executable */
			if (chmod(buf, fperms) == -1) {
				(void) sprintf(errstring, "Cannot chmod %s", buf);
				message(errstring);
			}
#endif MAGIC
		}
	}
	return(retval);
}


/* 
 *  Read Cell routine - when this is done for multiple buffers, it should
 *  check first to see if another buffer already has the file. It takes a
 *  filename as input - if this is NULL, then it asks the user for a
 *  filename.
 */
Cell *ReadCell(prompt, fname)
char *prompt;
char *fname;
{
	char *name, *StripName();
	Cell *cell;
	char buf[MAXPATHLEN];
	struct stat	stbuf;
	extern void SetWorkingCursor(), SetWaitCursor();
	
	/* Get filename */
	if (fname == NULL)
		if ((fname = get_input(prompt, (char *) NULL, TRUE)) == NULL)
			return((Cell *) NULL);
	/*
	 *  create the cell
	 */
	if ((name = StripName(fname)) == NULL) {
		message("No memory for cell name");
		return((Cell *) NULL);
	}

	if ((cell = NewCell(name, fname)) == NULL) {
		message("No memory for new cell");
		free(name);
		free(fname);
		return((Cell *) NULL);
	}
	PathParse(fname, buf);
	/* if a file exists, then open it and read the Gels */
	if ((inFile = fopen(buf, "r")) != NULL) {
		SetWaitCursor();
		cell->gelList = ReadGel();
#ifdef DEBUG
		if (cell->gelList != NULL) {
			(void) sprintf(errstring, "Read file %s", buf);
			message(errstring);
		}
#endif
		if (fstat(fileno(inFile), &stbuf) == -1) {
			(void) sprintf(errstring, "Can't fstat %s", buf);
			message(errstring);
		} else {
			cell->mtime = stbuf.st_mtime;
		}
		(void) fclose(inFile);
		SetWorkingCursor();
		cell->saved = SAVED;
		inFile = NULL;
	} 
#ifdef DEBUG
	else {
		(void) sprintf(errstring, "New file %s", buf);
		message(errstring);
	}
#endif

	return(cell);
}


/*
 *  expands and ~ in the cell filename and returns that in fname, stats
 *  the file, checks with the user if the file has been modified since
 *  it was last saved/read by the program, or if we'll be overwriting a
 *  file, and returns TRUE if things can proceed, FALSE if things can't!
 *  If backup is true, then it makes a backup copy of the file. All in
 *  one stat! This code has been adapted from similar code in JOVE.
 */

int
chk_mtime(cell, fname, backup)
Cell	*cell;
char	*fname;
int		backup;
{
	struct stat	stbuf;
	int exists;
	static char	*badchars = "!$^&*()`{}\"'\\|<>? ";
	register char *cp = cell->filename;
	register int c;

	while (c = *cp++)
		if (c < ' ' || c == '\177' || index(badchars, c)) {
			(void) sprintf(errstring,
			 "'%c': bad character in filename \"%s\".", 
			 c, cell->filename);
			message(errstring);
			return(FALSE);
		}
	PathParse(cell->filename, fname);
	stbuf.st_mode = 0;
	exists = stat(fname, &stbuf);
	if ((!exists) && (cell->saved & NEWFILE)) {
		/* We're about to overwrite something */
		(void) sprintf(errstring,
		 "\"%s\" already exists; overwrite it (y/n)? ", 
		 cell->filename);
		switch (confirm(errstring, "n")) {
			case ABORT:
			case NO:
				return(FALSE);
			case YES:
				break;
		}
	}
	if ((cell->mtime != 0) &&				/* if we care ... */
	 (!exists) &&								/* and it exists */
	 (stbuf.st_mtime != cell->mtime)) {	/* and there's trouble. */
	 	(void) sprintf(errstring,
		 "\"%s\" on disk is not what you last read/saved. Overwrite (y/n)? ",
		 cell->filename);
		switch(confirm(errstring, "n")) {
			case ABORT:
			case NO:
				return(FALSE);
			case YES:
				break;
		}
	}
	if ((!exists) && backup)
		/* Create backup file with same mode as input file */
		if (file_backup(fname, stbuf.st_mode) == -1) {
			message("Couldn't write backup file - save failed");
			return(FALSE);
		}
		
	return(TRUE);
}
