/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/table/RCS/tabio.c,v 1.9 89/10/13 10:35:39 jhh Exp $ */
/* $ACIS:tabio.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/table/RCS/tabio.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/table/RCS/tabio.c,v 1.9 89/10/13 10:35:39 jhh Exp $";
#endif /* lint */

/* tabio.c - input/output for table */

extern char * malloc();
extern char * realloc();

#include <class.h>

#include <dataobj.ih>

#include <ctype.h>

#define AUXMODULE
#include <table.eh>

/* read and write ASCII representation */

/*  * format for ASCII representation
 *
 * Cells are separated by tabs
 * Rows are separated by newlines
 *
 * Each cell has the following elements in order:
 *
 *
 *  1.  Color of line to left of cell
 *
 *    = if glued to cell at left (in which case nothing below appears)
 *    | if edge to left of cell is visible
 *    Either of these is followed by a space.  This is important because
 *    it distinguishes this from interspersed horizontal color lines (see below).
 *
 *
 *  2.  Cell format and precision (if not default values)
 *
 *   {<format><precision> <L>}
 *
 *
 *  3.  Special case for currency-formatted cells:
 *
 *   "$" if dollar format
 *
 *
 *  4.  Contents of cell: one of:
 *
 *     null cell:  zero or more blanks
 *       (exactly one blank is recommended, so that null cells at the end of the row are unambiguous)
 *     expressions to be calculated: =expression (= is optional if unambiguous without it)
 *     direct character strings: "string or 'string or ^string (' is optional if string starts with a letter)
 *     imbedded objects: \datatype  (in message buffer - data stream in file)
 *
 *
 * If horizontal lines are visible they are specified by additional
 * lines interspersed with the rows, containing:
 *
 *    - if the horizontal line between cells is visible
 *    = if the cells are glued together
 *    tab after each cell
 *
 * If row and/or column data is included it appears at the beginning:
 *
 *   \row t1 t2 ... tn
 *   \col t1 t2 ... tn
 *
 *   where ti is the thickness of row i.
 */

/* write thickness data */

static void WriteThickness(f, tag, slice, first, last, def)
FILE *f;
char *tag;
struct slice * slice;
int first, last;
{
    int i;
    int alldef;

    alldef = 1;
    for (i = first; i < last; i++)
	if (slice[i].thickness != def) {
	    alldef = 0;
	    break;
	}
    if (alldef == 0) {
	fprintf(f, "\\%s", tag);
	for (i = first; i <= last; i++)
	    fprintf(f, " %d", slice[i].thickness);
	putc('\n', f);
    }
}

/* write color information */

static void WriteAboveColor(T, f, r, first, last)
register struct table * T;
FILE *f;
int r;
int first, last;
{
    int c, clast;

    for (c = first, clast = -1; c <= last; c++)
	if (table_ColorAbove(T, r, c) == BLACK
	    || table_IsJoinedAbove(T, r, c))
	    clast = c;
    for (c = first; c <= clast; c++) {
	if (table_ColorAbove(T, r, c) == BLACK)
	    putc('-', f);
	else if (table_IsJoinedAbove(T, r, c))
	    putc('=', f);
	putc((c < clast) ? '\t' : '\n', f);
    }
}

/* write string */

static char *WriteString (x, cp)
register char *x;
register char *cp;
{
    for (; *x; x++) {
	if (*x == '\n')
	    *cp++ = '\\', *cp++ = 'n';
	else if (*x == '\t')
	    *cp++ = '\\', *cp++ = 't';
	else if (*x == '\\')
	    *cp++ = '\\', *cp++ = *x;
	else
	    *cp++ = *x;
    }
    return cp;
}

/* write contents of one cell */

void WriteCell (T, f, cell, buff, level)
register struct table * T;
FILE *f;
struct cell * cell;
char *buff;
int level;
{
    char *cp = buff;

    if (debug) {
	int n, r, c;
	n = cell - T->cells;
	r = n / table_NumberOfColumns(T) + 1;
	c = n % table_NumberOfColumns(T) + 1;
	printf("WriteCell(,, [%d,%d],, %d)\n", r, c, level);
    }

    /* output format and precision and lock */

    if ((cell->format != CURRENCYFORMAT
	&& cell->format != GENERALFORMAT)
	|| cell->precision != DefaultPrecision
	|| cell->lock) {
	*cp++ = '{';
        *cp++ = cell->format;
	if (cell->precision != DefaultPrecision) {
	    sprintf(cp, "%d", cell->precision);
	    while (*cp) cp++;
	}
	if (cell->lock)
	    *cp++ = ' ', *cp++ = 'L';
	*cp++ = '}';
	*cp++ = ' ';
    }

    /* special case for % and $ */

    if (cell->format == CURRENCYFORMAT)
	*cp++ = cell->format;

    /* output formula or text */

    switch (cell->celltype) {
	case table_EmptyCell:
	    *cp++ = ' ';
	    break;
	case table_TextCell:
	    cp = WriteString(cell->interior.TextCell.textstring, cp);
	    if (debug)
		printf("text string is %s\n", buff);
	    break;
	case table_ValCell:
	    if (!isdigit(*cell->interior.ValCell.formula))
		*cp++ = '=';
	    cp = WriteString(cell->interior.ValCell.formula, cp);
	    break;
	case table_ImbeddedObject:
	    if (!(cell->interior.ImbeddedObject.data)) {
		char *tp = "MISSING DATA OBJECT!";
		while (*tp)
		    *cp++ = *tp++;
	    }
	    else if (f) {
		if (debug)
		    printf("Writing out %s\n", class_GetTypeName(cell->interior.ImbeddedObject.data));
		dataobject_Write (cell->interior.ImbeddedObject.data, f, super_GetWriteID(T), level + 1);
	    }
	    else {
		char *tp = class_GetTypeName(cell->interior.ImbeddedObject.data);
		*cp++ = '\\';
		while (*tp)
		    *cp++ = *tp++;
	    }
	    break;
    }
    *cp = '\0';
}

/* write subrectangle */

void WriteASCII (T, f, chunk, level)
register struct table * T;
FILE *f;
Chunk chunk;
int level;
{
    int r, c;
    int fr = max(0, chunk->TopRow);
    int lr = chunk->BotRow;
    int fc = max(0, chunk->LeftCol);
    int lc = chunk->RightCol;

    if (debug)
	printf("WriteASCII(,,[%d..%d, %d..%d], %d)\n", fr+1, lr+1, fc+1, lc+1, level);
    if (chunk->LeftCol < 0)
	WriteThickness(f, "rows", T->row, fr, lr, TABLE_DEFAULT_ROW_THICKNESS);
    if (chunk->TopRow < 0)
	WriteThickness(f, "cols", T->col, fc, lc, TABLE_DEFAULT_COLUMN_THICKNESS);

    for (r = fr; r <= lr; r++ ) {

	WriteAboveColor(T, f, r, fc, lc);

	for (c = fc; c <= lc; c++) {
	    char buff[257];

	    if (table_IsJoinedToLeft(T, r, c)) {
		putc('=', f);
		putc(' ', f);
	    }
	    else if (table_ColorToLeft(T, r, c) == BLACK) {
		putc('|', f);
		putc(' ', f);
	    }

	    if (!table_IsJoinedToAnother(T, r, c)) {
		WriteCell(T, f, table_GetCell(T, r, c), buff, level);
		fprintf(f, "%s", buff);
	    }

	    if (c < lc) putc('\t', f);
	}
	if (table_ColorToLeft(T, r, lc + 1) == BLACK) {
	    putc('\t', f);
	    putc('|', f);
	    putc(' ', f);
	}
	putc('\n', f);
    }

    WriteAboveColor(T, f, lr + 1, fc, lc);
}

/* refill buffer if necessary */

/* if f is NULL, we are simply scanning a line buffer with cp; buff and cl are meaningless */
/* in any case, the buffer is terminated with a null */

#define advance(f, buff, cp, cl) {cp++; if (*cp == '\0') cp = ReadLine(f, buff, cl);}

/* read an input line */

/* by reading starting at buff+1 we leave one character worth of putback */

static char * ReadLine(f, buff, cl)
FILE *f;
char buff[];
char *cl;
{
    char *cp;
    int inc = 0;

    cp = buff+1;
    if (f != NULL) {
	for (; (inc = getc(f)) > 0 && cp < cl-1; ) {
	    *cp++ = inc;
	    if (inc == '\n') break;
	}
    }
    *cp = '\0';
    if (debug)
	printf("Input line:  \'%s\' inc=%d\n", buff+1, inc);
    return buff+1;
}

/* read thickness vector */

static int ReadSlice(f, buff, cp, cl, sp)
FILE *f;
char buff[];
char *cp;
char *cl;
struct slice **sp;
{
    int i, t;

    *sp = (struct slice *) malloc (sizeof (struct slice));
    if (*sp == NULL) {
	printf("Out of memory reading table (ReadSlice)\n");
	return -1;
    }
    for (i = 0; *cp != '\0' && *cp != '\n'; i++) {
	while (*cp == ' ') advance(f, buff, cp, cl);
        if (*cp < '0' || *cp > '9')
            return 0;
	*sp = (struct slice *) realloc( (char *) *sp, (i + 1) * sizeof (struct slice));
	if (*sp == NULL) {
	    printf("Out of memory reading table (ReadSlice)\n");
	    return -1;
	}
        for (t = 0; *cp >= '0' && *cp <= '9'; ) {
	    t = t * 10 + (*cp - '0');
	    advance(f, buff, cp, cl);
	}
        (*sp)[i].thickness = t;
	while (*cp == ' ') advance(f, buff, cp, cl);
    }
    return i;
}

/* read color information */

static int ReadLeftColor(T, f, buff, cp, cl, r, c)
struct table *T;
FILE *f;
char buff[];
char *cp;
char *cl;
int r, c;
{
    int color;
    char lastc;
    char *oldcp = cp;

    color = GHOST;

    if (*cp == '=' || *cp == '|') {
	lastc = *cp;
	advance(f, buff, cp, cl);
	cp--;
	*cp = lastc;
	if (*cp == '=' && *(cp+1) == ' ') {
	    color = JOINED;
	    advance(f, buff, cp, cl);
	}
	else if (*cp == '|' && *(cp+1) == ' ') {
	    color = BLACK;
	    advance(f, buff, cp, cl);
	}
	if (*cp == ' ') advance(f, buff, cp, cl);
    }

    table_SetLeftColor(T, r, c, color);
    return cp-oldcp;
}

/* read horizontal colors and return true if there were some */

static int ReadAboveColor(T, f, buff, cp, cl, r)
register struct table * T;
FILE *f;
char buff[];
char *cp;
char *cl;
int r;
{
    int c;
    int cmax;
    char lastc;
    char *cpstart = cp;

    /* this code only looks at the first bufferful, which could be a problem (unlikely) */

    for (c = 0, cmax = -1; *cp != '\0' && *cp != '\n'; ) {
	if (*cp == '-' || *cp == '=') {
	    cmax = c;
	    lastc = *cp;
	    while (*cp == lastc) cp++;
	}
	if (*cp == '\t') cp++;
	else if (*cp != '\0' && *cp != '\n')
	    return 0;
    }
    if (cmax < 0)
	return 0;
    
    if (c >= table_NumberOfColumns(T))
	table_ChangeSize (T, table_NumberOfRows(T), c + 1);
    if (r > table_NumberOfRows(T))
	table_ChangeSize (T, r, table_NumberOfColumns(T));

    for (c = 0, cp = cpstart; *cp != '\0' && *cp != '\n'; ) {
	if (*cp == '-' || *cp == '=') {
	    if (c >= table_NumberOfColumns(T))
		table_ChangeSize (T, table_NumberOfRows(T), c + 1);
	    table_SetAboveColor(T, r, c, (*cp == '-') ? BLACK : JOINED);
	    lastc = *cp;
	    while (*cp == lastc) advance(f, buff, cp, cl);
	}
	if (*cp == '\t') {
	    advance(f, buff, cp, cl);
	    c++;
	}
    }
    return 1;
}

/* read string */

static char *ReadString(cp, result)
char *cp;
char **result;
{
    register char *x, *y;
    register int k;

    for (x = cp, k = 0; *x != '\0' && *x != '\n' && *x != '\t'; x++, k++)
	if (*x == '\\')
	    x++;
    y = *result = malloc(k + 1);
    if (y == NULL) {
	printf("Out of space reading table (ReadString)\n");
	return x;
    }
    for (x = cp; k-- > 0; ) {
	if (*x == '\\') {
	    switch (*++x) {
	    case 'n':
		*y++ = '\n';
		break;
	    case 't':
		*y++ = '\t';
		break;
	    default:
		*y++ = *x++;
	    }
	} else
	    *y++ = *x++;
    }
    *y = '\0';
    return x;
}

/* read a cell, return pointer past what was read */

int ReadCell (T, f, buff, cp, cl, cell)
register struct table *T;
FILE *f;
char *buff;
char *cp;
char *cl;
struct cell *cell;
{
    char *bp = cp;

    DestroyCell(T, cell);

    /* get format, precision, and lock */

    while (*cp == ' ') advance(f, buff, cp, cl);
    if (*cp == '{') {
	advance(f, buff, cp, cl);
	if (*cp != ' ') {
	    cell->format = GENERALFORMAT;
	    cell->precision = DefaultPrecision;
	    if (*cp != '}' && (*cp < '0' || *cp > '9'))
		    cell->format = *cp++;
	    if (*cp >= '0' && *cp <= '9') {
		int precision;
		for (precision = 0; *cp >= '0' && *cp <= '9'; cp++)
		    precision = (precision * 10) + (*cp - '0');
		cell->precision = precision;
	    }
	}
	while (*cp == ' ') advance(f, buff, cp, cl);
	if (*cp == 'L')
	    cell->lock = 1;
	while (*cp != '\0' && *cp != '\n' && *cp != '}' && *cp != '\t')
	    advance(f, buff, cp, cl);
	if (*cp == '}') advance(f, buff, cp, cl);
    }

    /* special case for $ */

    while (*cp == ' ') advance(f, buff, cp, cl);
    if (*cp == '$') {
	cell->format = CURRENCYFORMAT;
	advance(f, buff, cp, cl);
    }

    /* get string */

    if (*cp == '\'' || *cp == '^' || *cp == '\"' || isalpha(*cp)) {
	cell->celltype = table_TextCell;
	cp = ReadString(cp, &(cell->interior.TextCell.textstring));
	if (cell->interior.TextCell.textstring == NULL)
	    cell->celltype = table_EmptyCell;
    }

    /* get imbedded object from buffer */

    else if (*cp == '\\' && f == NULL) {
	/* leave it null */
	while (*cp != '\0' && *cp != '\n' && *cp != '\t')
	    cp++;
    }

    /* get imbedded object */

    else if (strncmp(cp, "\\begindata{", 11) == 0) {
	char dataname[256];
	register char *np;
	long uniqueID;

	for (cp += 11, np = dataname; np < dataname + 255 && *cp != '\0' && *cp != ','; ) {
	    *np++ = *cp;
	    advance(f, buff, cp, cl);
	}
	*np = '\0';
	if (*cp == ',') advance(f, buff, cp, cl);
	while (*cp == ' ') advance(f, buff, cp, cl);
	uniqueID = 0;
	while (*cp >= '0' && *cp <= '9') {
	    uniqueID = uniqueID * 10 + (*cp - '0');
	    advance(f, buff, cp, cl);
	}
	while (*cp == ' ') advance(f, buff, cp, cl);
	if (*cp == '}') {
	    advance(f, buff, cp, cl);
	    while (*cp == ' ') advance(f, buff, cp, cl);
	}
	if (*cp != '\0' && *cp != '\n')
	    printf("Wrong begindata format: %s\n", buff);
	else {
	    struct dataobject *newobject = (struct dataobject *) class_NewObject(dataname);
	    if (!newobject)
		printf("Could not create %s object.\n", dataname);
	    else {
		cell->celltype = table_ImbeddedObject;
		cell->interior.ImbeddedObject.data = newobject;
		cell->interior.ImbeddedObject.views = NULL;
		newobject->id = uniqueID;
		if (f) {
		    dataobject_Read(newobject, f, uniqueID);
		    if (debug)
			printf("Additional ");
		    cp = ReadLine(f, buff, cl);
		}
	    }
	}
    }

    /* get formula */

    else if (*cp != '\0' && *cp != '\n' && *cp != '\t') {
	if (*cp == '=')
	    advance(f, buff, cp, cl);
	cell->celltype = table_ValCell;
	cp = ReadString(cp, &(cell->interior.ValCell.formula));
	cell->lastcalc = 0;
	if (cell->interior.ValCell.formula == NULL)
	    cell->celltype = table_EmptyCell;
    }

    /* skip trailing spaces */
    while (*cp == ' ') advance(f, buff, cp, cl);

    return cp - bp;
}

/* read subrectangle */

struct table * ReadASCII (T, f)
register struct table * T;
FILE *f;
{
    char buff[1000];
    char *cp;
    char *cl;
    int r, c, n;
    struct slice * s;

    /* need 1x1 for ChangeSize to allocate the edge array */

    table_ChangeSize (T, 0, 0);
    table_ChangeSize (T, 1, 1);

    T->everythingChanged = T->cellChanged = ++(T->timeStamper);

    /* read first line of input */

    cl = buff + sizeof buff;
    cp = ReadLine(f, buff, cl);
    
    if (strncmp(cp, "\\begindata{table,", 17) == 0) {
	if (debug)
	printf("*** Skipped begindata - if you see this, bug Andy Palay ***\n");
	cp = ReadLine(f, buff, cl);
    }
    if (*cp == '\n') {
	if (debug)
	    printf("Skipped empty input line at beginning of table!\n");
	cp = ReadLine(f, buff, cl);
    }

    /* read thickness information */

    if (strncmp(cp, "\\rows", 5) == 0) {
	if (debug)
	    printf("Got rows\n");
	n = ReadSlice(f, buff, cp+5, cl, &s);
	if (n > 0) {
	    if (n > table_NumberOfRows(T))
		table_ChangeSize (T, n, table_NumberOfColumns(T));
	    for (r = 0; r < n; r++)
		T->row[r].thickness = s[r].thickness;
	}
	if (s)
	    free (s);
	s = NULL;
	cp = ReadLine(f, buff, cl);
    }

    if (strncmp(cp, "\\cols", 5) == 0) {
	if (debug)
	    printf("Got cols\n");
	n = ReadSlice(f, buff, cp+5, cl, &s);
	if (n > 0) {
	    if (n > table_NumberOfColumns(T))
		table_ChangeSize (T, table_NumberOfRows(T), n);
	    for (c = 0; c < n; c++)
		T->col[c].thickness = s[c].thickness;
	}
	if (s)
	    free (s);
	s = NULL;
	cp = ReadLine(f, buff, cl);
    }

    for (r = -1; *cp != '\0';  ) {

	if (ReadAboveColor(T, f, buff, cp, cl, r + 1))
	    cp = ReadLine(f, buff, cl);
	if (*cp == '\0') {
	    if (debug)
		printf("End of input\n");
	    break;
	}

	if (strncmp(cp, "\\enddata{", 8) == 0) 
	    break;

	if (++r >= table_NumberOfRows(T))
	    table_ChangeSize (T, r + 1, table_NumberOfColumns(T));

	if (debug)
	    printf("*** row %d *** for \'%s\'\n", r + 1, cp);

	for (c = -1;;) {

	    cp +=  (n = ReadLeftColor(T, f, buff, cp, cl, r, c+1));
	    if ((*cp == '\0' || *cp == '\n') && n > 0)
		break;

	    if (++c >= table_NumberOfColumns(T))
		table_ChangeSize (T, table_NumberOfRows(T), c + 1);
	    
	    cp += ReadCell(T, f, buff, cp, cl, table_GetCell(T, r, c));

	    if (*cp != '\t')
		break;
	    advance(f, buff, cp, cl);
	}

	cp = ReadLine(f, buff, cl);
    }

    return T;
}
