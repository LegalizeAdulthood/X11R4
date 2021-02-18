#ifndef __XPIC_GLOBALS_H__
#define __XPIC_GLOBALS_H__
/*
 * This program has far too much global state for its own good. Oh well!
 */
int drawingMode;			/* Stores the current drawing mode:START_MODE etc*/
int objectType;				/* stores the current object type - LINE, etc. */
int editType;				/*
							 *  stores the current editing operation -
							 *  Copy, delete, move, etc.
							 */
int editMode;				/*
							 *  Stores the current editing mode - BLOCK,
							 *  ELEMENT
							 */

int line_type;
int line_arrow;

struct {
	char *pattern;
	int len;
} LineTypes[NSTYLES];

int fill_type;
Pixmap FillTypes[NFILLSTYLES];

/* Stuff to store text parameters */
int textVertAlign, textHorizAlign;

/*  The next few globals contain info about the current font */
XFontStruct *textFont;			/* Pointer to the current font XFontStruct */
int textSize;					/* The point size of the current font */
int spacePad;					/* The space pad required for the font */
FontFamily *fontType;			/* The name of a font */
Cell *CurrentCell;				/* The current cell being viewed */
Cell *LastCell;
Cell *MainCell;					/* head of the linked list of cells */
Gel *KillBuffer;				/* gel list of cut objects, saved for paste */
int KillX, KillY;				/* mouse position when objects were cut */
Box picBox;
char *nullfile;
char *argfile;
char errstring[128];
char *homeDir;
FILE *inFile, *outFile;
BOOL backupOnWrite;
int mouseResolution;
int GelCounter;
int lineThickness;
char *progname;

/*
 * X Windows globals. The clean thing to do would be to put these in a struct
 * and make the thing a pic widget. One day...
 */

/* The page width and height, in tenths of an inch */
int pageWidth, pageHeight;

Widget outerFrame;		/* The main window frame */
Widget picWidget;
Window picWin;			/* The actual drawing window */
/* We shouldn't need these in X11 */
int picWinWidth;
int picWinHeight;
int minPicWidth, minPicHeight;
Widget inputWidget;
Window inputWin;
Display *picDpy;

/*
 *  These are the 'current' GC values - the current element will use
 *  these values
 */
GC gcNormal, gcInvert, gcErase, gcGray;
/* Used for boxes, outlines, drags etc */
GC gcBlock;
/* 
 *  The contents of tmpGcNormal and Invert can be changed at any time by
 *  any routine. They're meant to be fast change GCs. Note that all the
 *  xxx_adj() routines use it, and make the HORRIBLE assumptions that
 *  no-one will change these during an ADJUST (START_MODE to END_MODE
 *  back to START_MODE) operation. Otherwise, we either need lots of GCs
 *  or we need lots of changes to the GCs when rubber banding.
 */
GC tmpGcNormal, tmpGcInvert, tmpGcErase;
XGCValues gcVal;

Pixmap gridTile;
Pixmap blankTile;
int gridSpacing;
BOOL gridOn;

Pixel cursorColor, gridColor, highlight;
/*
 *  verts is the XPoint buffer for the points input by the user for lines
 *  and splines
 */
XPoint *verts;			/* Used to store lines and splnes temporarily */
int nVerts;
int maxVerts;

/*
 *  Stores the 'flattened' output of the BSpline routines. Flattening is
 *  the process by which the polygon defining the spline is converted into
 *  the lots of little line segments forming the curve. 
 */
XPoint *flatVerts;
int flatSize;
int nFlat;
XPoint *flatVerts2;
int flatSize2;

#endif /* __XPIC_GLOBALS_H__ */ /* Do not add anything after this line */
