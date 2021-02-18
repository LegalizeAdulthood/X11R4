/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/table/RCS/table.ch,v 1.6 89/03/10 17:38:20 jhh Exp $ */
/* $ACIS:table.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/table/RCS/table.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtable_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/table/RCS/table.ch,v 1.6 89/03/10 17:38:20 jhh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* table.ch - table data object definition and interface */

#define table_VERSION 1
#define SafeFromNull(s) (((s) != NULL) ? (s) : "(null)")

/* information about an entire row or column */

struct slice {
   short thickness;
};
#define TABLE_DEFAULT_ROW_THICKNESS 0
#define TABLE_DEFAULT_COLUMN_THICKNESS 99

/* a dimension determines whether we are working
    horizontally or vertically at the moment                  */

enum dimension { ROWS, COLS };
typedef enum dimension Dimension;

/* "colors" of cell boundary lines */

typedef char Color;
#define SUPPRESSED 0	/* suppressed when not input focus */
#define JOINED 1	/* joined cells */
#define GHOST 2		/* dotted line, not printed */
#define BLACK 3		/* black line */

/* a chunk is a subrectangle of the spreadsheet */

struct chunk {
    short LeftCol;		/* first column involved */
    short RightCol;		/* last column involved */
    short TopRow;		/* first row involved */
    short BotRow;		/* last row involved */
};
typedef struct chunk * Chunk;

struct extended_double_struct {
    union {
	double standard;
	char *bogus;
    } extended_value;
    enum {extended_STANDARD, extended_BOGUS} type;
};

typedef struct extended_double_struct extended_double;

#define ExtendedType(x) ((x)->type)
#define StandardValue(x) ((x)->extended_value.standard)
#define IsStandard(x) ((x)->type == extended_STANDARD)
#define IsBogus(x) ((x)->type == extended_BOGUS)
#define ExtractBogus(x) ((x)->extended_value.bogus)

/* list of views (of a dataobject) */

struct viewlist {
    struct viewlist *next;	/* next view on list */
    struct view	*child;		/* child view - viewing this data object */
    struct view	*parent;	/* parent of the child */
};

/* fundamental cell content types */

enum CellTypes {table_EmptyCell, table_TextCell, table_ValCell, table_ImbeddedObject };

/* a cell is an element of a spreadsheet */

struct cell {
    char format;		/* format letter */
    char precision;		/* precision parameter of format */
    char lock;			/* cell is locked */
    int	lastcalc;		/* time stamp of last re-evaluation */
    enum CellTypes celltype;
    union {			/* cell type dependent information */
	int EmptyCell;
	struct {
	    char *textstring;	    /* string preceeded by justification char */
	} TextCell;
	struct {
	    extended_double value;  /* numeric value or nan("message") */
	    char *formula;	    /* symbolic formula */
	} ValCell;
	struct {
	    struct dataobject *data; /* embedded object */
	    struct viewlist *views;  /* views of embedded object */
	} ImbeddedObject;
    } interior;
};

/* format codes */

#define GENERALFORMAT 'G'
#define CURRENCYFORMAT '$'
#define PERCENTFORMAT '\045'	/* class preprocessor mangles the real thing */
#define EXPFORMAT 'E'
#define FIXEDFORMAT 'F'
#define HORIZONTALBARFORMAT 'H'
#define VERTICALBARFORMAT 'V'
#define DDMMMYYYYFORMAT 'Q'
#define MMMYYYYFORMAT 'R'
#define DDMMMFORMAT 'S'

#define adjustbyone(n) (n <= 0 ? 0 : n + 1)
#define max(n, m) (n < m ? m : n)

#define getDataObject(T) ((T) -> header.dataobject)


/* globals for entire package */

struct table * table_List;	/* root of list of all tables */

char debug;			/* nonzero if debugging */
int DefaultPrecision;		/* precision when none provided */

/* actual interface definition */

class table: dataobject[dataobj] {

overrides:
  ViewName() returns char *;
  Read (FILE * f, long id) returns long;
  Write (FILE * f, long writeid, int level) returns long;

methods:
  ToggleDebug ();
  FindName (char * name) returns struct thisobject *;
  SetName (char * name) returns struct thisobject *;
  ChangeSize (int nrows, int ncols);
  ExtractData (Chunk chunk) returns struct thisobject *;
  InsertData (struct thisobject * T, Chunk chunk);
  WriteASCII (FILE * f, Chunk chunk);
  ReadASCII (FILE * f) returns struct thisobject *;
  FormatCell (struct cell * cell, char * buff);
  ParseCell (struct cell * cell, char * buff);
  ChangeThickness (Dimension dim, int i, int thickness);
  FindBoundary (Chunk chunk);
  SetInterior (Chunk chunk, Color color);
  SetBoundary (Chunk chunk, Color color);
  ReEval (int r, int c);
  SetFormat (char ch, Chunk chunk);
  SetPrecision (int precision, Chunk chunk);
  Imbed (char *name, Chunk chunk);
  Lock (char ch, Chunk chunk);
  FormatDate (double fdate, char *buf, char format);

macromethods:
  Name() SafeFromNull(self->tablename)
  NumberOfRows() self->rows
  NumberOfColumns() self->cols
  RowHeight(r) self->row[r].thickness
  ColumnWidth(c) self->col[c].thickness
  GetCell(r,c) (self->cells+((r)*self->cols+(c)))
  ColorAbove(r,c) *(self->aboveedge+((r)*(self->cols+1)+(c)))
  ColorToLeft(r,c) *(self->leftedge+((r)*(self->cols+1)+(c)))
  SetAboveColor(r, c, color) *(self->aboveedge+((r)*(self->cols+1)+(c))) = color
  SetLeftColor(r, c, color) *(self->leftedge+((r)*(self->cols+1)+(c))) = color
  IsJoinedAbove(r, c) (table_ColorAbove(self, r, c) == JOINED)
  IsJoinedToLeft(r,c) (table_ColorToLeft(self, r, c) == JOINED)
  IsJoinedToAnother(r,c) (table_IsJoinedAbove(self, r, c) || table_IsJoinedToLeft(self, r, c))
  StampEverything() self->everythingChanged = ++(self->timeStamper)
  CurrentTimestamp() (self->timeStamper)
  EverythingTimestamp() (self->everythingChanged)
  CellsTimestamp() (self->cellChanged)
  EdgesTimestamp() (self->edgeChanged)
  WriteTimestamp() (self->fileWritten)

classprocedures:
  InitializeClass() returns boolean;
  InitializeObject (struct table * T) returns boolean;	/* initialize contents */
  FinalizeObject (struct table * T);	/* clean up contents */

data:
    char *tablename;			/* tag for write routine */
    struct table * NextTable;		/* next table in memory */
    short rows, cols;			/* numbers of rows and columns */
    struct slice *row, *col;		/* row and column information */
    struct cell *cells;			/* rectangular array of cells */
    char *leftedge;			/* edge array (1 extra for end) */
    char *aboveedge;			/* edge array (1 extra for end) */
    int everythingChanged;		/* timestamp when whole grid changed */
    int edgeChanged;			/* timestamp when edges changed */
    int cellChanged;			/* timestamp when cell contents changed */
    int fileWritten;			/* timestamp when file read/written */
    int timeStamper;			/* current timestamp */
};

/* end of table.ch */
