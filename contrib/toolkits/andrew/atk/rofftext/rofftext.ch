/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.ch,v 2.6 89/04/04 13:27:32 mp1w Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_rofftext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.ch,v 2.6 89/04/04 13:27:32 mp1w Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* rofftext - translate troff/nroff to ATK text object
 *
 */


/* environment:
point size
font
fill, adjust, center
line spacing, line length
indent, temporary indent
input trap
tabs
underline
control character
nobreak control character
hyphenation
title length
number mode
margin character
*/



enum RegFmt {
    reg_Single=0,     /* 0,1,2...*/
    reg_Triple,     /* 000,001,002,...*/
    reg_LCRoman,    /* 0,i,ii,iii,...*/
    reg_UCRoman,    /* 0,I,II,III,...*/
    reg_LCaz,       /* 0,a,b,c,...*/
    reg_UCaz        /* 0,A,B,C,...*/
};

struct reg {
    enum RegFmt format;
    int value;
    int autoinc;
};
typedef struct reg *Reg;

enum trickle_type {
    trickle_File,
    trickle_String
};

struct trickle {
    struct _trickle *t;
};

struct _trickle {
    enum trickle_type type;
    union {
        FILE *f;
        char *s;
    }u;
    char *filename;
    int LineNumber;
    char *buf;
    boolean pop;
    struct _trickle *prev;
};
typedef struct trickle *Trickle;

struct MagicBuf {
    char *begin, *end, *ptr;
    int size;
    int blocksize;
};
typedef struct MagicBuf *BUF;
#define BUFSIZ 1024
#define ClearBuf(b) ((b)->ptr = (b)->begin)
#define Buf2Str(b) ((b)->begin)
BUF NewBuf();

struct InputContext {
    int argc;
    char **argv;
};
typedef struct InputContext *IC;


struct trap {
    int loc;
    char *macro;
};

#define MAXARGS 24
#define StrDup(string) ((char *)strcpy((char *)malloc(strlen(string)+1),string))
#define NewObj(type) ((type *)malloc(sizeof(type)))

/* reserve the use of 200 - 210 */

#define NULCHAR 0200
#define ESCAPE 0201

enum scale_type {
    scale_u = 0,
    scale_i = 1,
    scale_c = 2,
    scale_P = 3,
    scale_m = 4,
    scale_n = 5,
    scale_p = 6,
    scale_v = 7,
    scale_none = 8
};

#define INDENTLEVELS 5

struct roffEnviron{
    int pointSize;
    char *font; /* name of the font */
    char *prevFont;
    int fontStyle; /* style ID for font */
    int indent;     /* number of basic units to indent */
    int prevIndent;
    int tempIndent; /* number of basic units for temp. indent */
    int NextInputTrap;  /* next input line to spring input trap */
    char *InputTrapCmd; /* name of input trap cmd */
    char controlChar; /* '.' */
    char NBControlChar; /* '\'' */
};

/* for debugging (obviously) */

#ifdef DEBUGGING
extern int ROFFDEBUG;
#define DEBUG(arg) if (ROFFDEBUG != 0) fprintf arg
#else /* DEBUGGING */
#define DEBUG(arg)
#endif /* DEBUGGING */


/* diversions */

struct diversionLevel {
    int NextDiversionTrap; /* line which springs next diversion trap */
    char *DiversionTrapCmd; /* command on sprung trap */
    char *name; /* diversion name */
    int level; /* diversion level */
    boolean NoSpaceMode;
    int OutputDone;
};


struct stackelt {
    int level;
    long pos;
    struct environment *env;
    int ID;
};

struct tempelt {
    int level;
    long pos;
    struct style *style;
    int ID;
};


#define Newelt() ((struct stackelt *)malloc(sizeof(struct stackelt)))
#define STACKSIZE 48


class rofftext: text {
classprocedures:
    InitializeObject(struct rofftext *self) returns boolean;
    InitializeClass() returns boolean;
    FinalizeObject(struct rofftext *self);
    ReadRoffIntoText(struct text *t, FILE *fp, long pos, char **resources) returns long;
methods:
    SetText(struct text *t); /* must be used before any styles started */
macromethods:
    SetInputFiles(files) ((self)->inputfiles = files)
    SetInitialPos(n) ((self)->pos = n)
overrides:
    Read(FILE *file, long id) returns long;
    Write(FILE *file, long writeID, int level) returns long;
    ViewName() returns char *;
    SetAttributes(struct attributes *atts);
data:
    /* global state variables */

    int v_InCond;                   /* inside conditional input */
    boolean v_TrapBlown; /* TRUE after trap is sprung */
    boolean v_CopyMode;         /* inside macro definition */
    BUF v_SnarfOutput;           /* save output in a buffer */
    boolean v_ReadEscapes;       /* escape mechanism is turned on */
    boolean v_RawMode;          /* interpret only \{  and  \} */
    boolean v_LastIfResult;      /* for .el requests */
    boolean v_BOL;        /* the last char read was beginning of line */
    boolean v_NextBOL;    /* the next char read will be beginning of line */
    boolean v_MultiSpace;
    Trickle Input;                      /* input for put() to push a command! */
    char *Output;                       /* string used to snarf output */
    char EscChar;                       /* the current escape char */
    boolean RoffType;           /* TRUE for troff, FALSE for nroff */
    char *v_Trap;                       /* trap for beginning of page */
    int v_InputLineCount;           /* non-blank input line number */
    int v_InputLineNumber;          /* real input line number */
    int v_ErrorCount;               /* number of errors for sanity check */
    int v_TempIndentTrap;               /* line number which springs temp. indent */
    boolean v_BlankLine;                 /* current line is blank */
    struct roffEnviron *CurrentEnviron; /* current environment */
    struct glist *EnvironStack;
    struct roffEnviron *Environs[4]; /* environments -- last one is dummy */
    int v_DiversionLevel;
    struct diversionLevel *CurrentDiversion;
    struct glist *DiversionStack;
    boolean v_FillMode;

    unsigned char Berliz[256]; /* translation table */

    /* indenting */


    int IndentStyle[INDENTLEVELS]; /* style ID's for indentation */
    char *IndentName[INDENTLEVELS]; /* name of styles for indent */
    int TempIndentStyle[INDENTLEVELS];
    char *TempIndentName[INDENTLEVELS];
    int MinusTempIndentStyle[INDENTLEVELS];
    char *MinusTempIndentName[INDENTLEVELS];


    struct glist *ArgStack;
    struct hash *Registers;
    struct hash *Commands;
    struct hash *Macros;
    struct hash *Goofy; /* commands with special syntax */
    struct hash *SpecialChars; /* for \(XX */
    char current;
    char *Nullarg[10];
    IC CurrentContext;


    int ScaleFactor[8];
    /* Fonts */

    char *Fonts[5];/* = {"roman","italic","bold","special","typewriter"};*/
    /*int CurrentFont = 0;*/
    int CurrentFontSize;


    struct stackelt *stack;
    struct tempelt *tempstack;
    long pos;
    int styleID;

    char *macrofile;
    char **inputfiles;

    boolean HelpMode;
    boolean BeCompletelyBogus;

    struct glist *EndMacros;
    char *filename;
    boolean PrintWarnings;
    struct text *text;  /* text to use for actually inserting chars and etc */
};
