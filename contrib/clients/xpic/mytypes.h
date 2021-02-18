#ifndef __XPIC_TYPES_H__
#define __XPIC_TYPES_H__
/*
 *  Stores the graphic elements as a struct which contains a bounding box,
 *  an element type (tokens), a pointer to the data for the element, and
 *  an attribute word.  For such things as boxes the data pointer will be
 *  null.  For polylines and splines, it will consist of a linked list of
 *  points. For a conic (circle, ellipse), it will consist of centre, and
 *  the x and y radii.  For text, it consists of a length, and a string.
 *  For cells, it consists of a name, and a pointer to a graphic struct.
 *  The attributes fit into a long int. They are things like linestyle
 *  (solid, dotted, short dashed, long dashed, dot dashed), arrowtype
 *  (noarrow, arrow at the start, arrow at the end, arrow at both ends),
 *  and whether the box/conic is filled or not. Text fonts, text sizes are
 *  stored in the text struct itself
 */


typedef struct {
	int x, y;
} Point;

typedef struct {
	Point ll, ur;
} Box;

typedef struct _gel {
	Box b_box;
	int type;					/* The type of gel - see below */
	int attributes;				/*
								 *  The attributes for the gel -
								 *  linestyle, arrowstyle, fillstyle,
								 *  text alignment
								 */
	int number;					/*
								 *  The id number of the gel - so they
								 *  can be kept in sorted order - this
								 *  will be necessary when doing filled
								 *  shapes - making sure different
								 *  shapes get drawn in the right order
								 */
	int linewidth;				/* Line thickness */
	int int_flags;				/*
								 *  For internal use only - (eg) marking
								 *  during select
								 */
	caddr_t data;				/*
								 *  A gel dependent item - it points to
								 *  nothing for a box, which is
								 *  represented by the bounding box, to
								 *  a struct of type PointList for lines
								 *  and splines, to a TextString for
								 *  text, to a Conic for circles and
								 *  ellipses
								 */
	struct _gel *next;
} Gel;

typedef struct {
	int nVerts;
	XPoint *v;
} PointList;


typedef struct {
	Point centre;
	int xrad, yrad;
} Conic;

typedef struct {
	int pointsize;		/* At the current screen resolution */
	char *xfontname;	/* to use for XLoadQueryFont() */
	XFontStruct *font;	/* Actual font */
} FontSizes;

typedef struct {
	char *name;
	FontSizes *sizes;
	int nsizes;
	int maxsizes;
	int cursize;
} FontFamily;

typedef struct {
	char *name;
	void (* handler)();
} ButtonInfo;

typedef struct _text {
	int x, y;
	int length;
	char *str;
	FontFamily *font;
	int sizeindex;
	char *fontname;
	int fontsize;
} TextString;

/* A string buffer that reallocates itself - size is the actual alloced size */
typedef struct {
	char *buf;
	int size;
} Buf;

/* 
 *  Cells are the actual pictures - they are essentially one Gel linked
 *  list, a corresponding View, and a bit of info about the cell status.
 *  They form a linked list too, because one xpic may have multiple cells.
 */
typedef struct _cell {
	Gel *gelList;				/* All the cell's graphic elements */
	int undo;					/* 
								 *  For an undo operation, the top 'undo'
								 *  elements must be removed from gelList
								 *  and...
								 */
	Gel *undoList;				/*
								 *  .. the elements in 'undoList' must be
								 *  added to gelList
								 */
	char *name;					/* cell name */
	char *filename;				/* File associated with this cell */
	time_t mtime;				/* The last modified time of the cell file */
	int saved;					/* Is this thing saved? */
	int changes;				/* Number of changes since last checkpoint */
	struct _cell *next;
} Cell;

#endif /* __XPIC_TYPES_H__ */ /* Do not add anything after this line */
