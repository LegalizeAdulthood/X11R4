/*
 *              Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * Define file for : Standard GKS C-language binding
 */

#ifndef   GKS_H
#define   GKS_H

#include  <stdio.h>     /*  for file i/o */

/* Gint - integer */
typedef int Gint;       /* this datatype must be defined by the implementation as
                           a type suitable for containing integer values at least 16 bits long */
/* Glong - longer integer */
typedef int   Glong;

/* Gintlist - integer list */
typedef struct {
        Gint            number;         /* number of integer in list */
        Gint            *integers;      /* array of integers */
} Gintlist;

/* Gchar - chatacter */
typedef char Gchar;   /* this datatype must be defined by the implementation
                         as a type suitable for containing characters of at least 8 bits */

/* Gfloat - floating point number */
typedef float Gfloat;    /* this datatype must be defined by the implementation
                            as a type suitable for containing float values at least 32 bits long */

/* Gredraw - workstation redraw state */
typedef enum {
        GRD_GKS,                /* redraw due to GKS */
        GRD_X                   /* redraw due to X */
} Gredraw;

/* Gacf - attribute control flag */
typedef enum {
        GCURRENT,
        GSPECIFIED
} Gacf;

/* Gasf - aspect source flag */
typedef enum {
        GBUNDLED,
        GINDIVIDUAL
} Gasf;

/* Gasf - aspect source flags */
typedef struct {
        Gasf            ln_type;        /* line type */
        Gasf            ln_width;       /* line width */
        Gasf            ln_colour;      /* line colour */
        Gasf            mk_type;        /* marker type */
        Gasf            mk_size;        /* marker size */
        Gasf            mk_colour;      /* marker colour */
        Gasf            tx_fp;          /* text font and precision */
        Gasf            tx_exp;         /* text expansion */
        Gasf            tx_space;       /* text character spacing */
        Gasf            tx_colour;      /* text colour */
        Gasf            fl_inter;       /* fill area interior style */
        Gasf            fl_style;       /* filll area style index */
        Gasf            fl_colour;      /* fill area colour */
}Gasfs;

/* Gattr - attributes used */
typedef enum {
        GPOLYLINE,
        GPOLYMARKER,
        GTEXT,
        GFILLAREA
} Gattrs;

/* Gcstat - choice status */
typedef enum {
        GC_OK,
        GC_NOCHOICE,
        GC_NONE
} Gcstat;

/* Gchoice - choice data */
typedef struct {
        Gcstat          status;         /* choice status */
        Gint            choice;         /* choice number */
} Gchoice;

/* Gprflag - prompt flag */
typedef enum {
        GPROFF,
        GPRON
} Gprflag;

/* Gchoicerec - choice data record */
typedef union {
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet1;
        struct {
                Gint    number;         /* number of alternatives */
                Gprflag *enable;        /* array of prompts */
                Gchar   *data;          /* device/implementation dependent data */
        } pet2;
        struct {
                Gint    number;         /* number of choice strings */
                Gchar   **strings;      /* array of strings */
                Gchar   *data;          /* device/implementation dependent data */
        } pet3;
        struct {
                Gint    number;         /* number of choice strings */
                Gchar   **strings;      /* array of strings */
                Gchar   *data;          /* device/implementation dependent data */
        } pet4;
        struct {
                Gint    seg;            /* segment name */
                Gint    number;         /* number of alternatives */
                Gint    *pickids;       /* array of pick identifiers */
                Gchar   *data;          /* device/implementation dependent data */
        } pet5;
        /* ....
           more for implementation defined PETs */
} Gchoicerec;

/* Gimode - input mode */
typedef enum {
        GREQUEST,
        GSAMPLE,
        GEVENT
} Gimode;

/* Gesw - echo switch */
typedef enum {
        GECHO,
        GNOECHO
} Gesw;

/* Glimit - coordinate limits */
typedef struct {
        Gfloat          xmin;           /* x minimum limit */
        Gfloat          xmax;           /* x maximum limit */
        Gfloat          ymin;           /* y minimum limit */
        Gfloat          ymax;           /* y maximum limit */
} Glimit;

/* Gchoicest - choice state */
typedef struct {
        Gimode          mode;           /* mode */
        Gesw            esw;            /* echo switch */
        Gchoice         choice;         /* choice data */
        Gint            pet;            /* prompt and echo type */
        Glimit          e_area;         /* echo area */
        Gchoicerec      record;         /* choice data record */
} Gchoicest;

/* Gclip - clipping indicator */
typedef enum {
        GCLIP,
        GNOCLIP
} Gclip;

/* Gcliprect - clipping rectangle */
typedef struct {
        Gclip           ind;            /* clipping indicator */
        Glimit          rec;            /* clipping rectangle */
} Gcliprec;

/* Gclrflag - clear control flag */
typedef enum {
        GCONDITIONALLY,
        GALWAYS
} Gclrflag;

/* Gcoavail - colour availablity */
typedef enum {
        GCOLOUR,
        GMONOCHROME
} Gcoavail;

/* Gcobunbl - colour bundle */
typedef struct {
        Gfloat          red;            /* red intensity */
        Gfloat          green;          /* green intensity */
        Gfloat          blue;           /* blue intensity */
} Gcobundl;

/* Gcofac - colour facilities */
typedef struct {
        Gint            colours;                /* number of colours */
        Gcoavail        coavail;        /* colour availability */
        Gint            predefined;     /* number of predefined bundles */
} Gcofac;

/* Gcovaild - colour values valid */
typedef enum {
        GABSENT,
        GPRESENT
} Gcovalid;

/* Gcsw - coordinate switch */
typedef enum {
        GWC,            /* world coordinates */
        GNDC            /* normalized device coordinates */
} Gcsw;

/* Gdefchoice - default choice data */
typedef struct {
        Gint            choices;        /* maximum number of choices */
        Gintlist        pets;           /* list of prompt and echo types */
        Glimit          e_area;         /* default echo area */
        Gchoicerec      record;         /* default choice data record */
} Gdefchoice;

/* Gdefmode - deferral mode */
typedef enum {
        GASAP,          /* as soon as possible */
        GBNIG,          /* before next interaction globally */
        GBNIL,          /* before next interaction locally */
        GASTI           /* at some time in */
} Gdefmode;

/* Girgmode - implicit regeneration mode */
typedef enum {
        GSUPPRESSED,
        GALLOWED
} Girgmode;

/* Gdefer - deferral state */
typedef struct {
        Gdefmode        defmode;        /* defferral mode */
        Girgmode        irgmode;        /* implicit regeneration mode */
} Gdefer;

/* Gpoint - coordinate point */
typedef struct {
        Gfloat          x;              /* x coordinate */
        Gfloat          y;              /* y coordinate */
} Gpoint;

/* Gipoint - integer point */
typedef struct {
        Gint            x;              /* x coordinate */
        Gint            y;              /* y coordinate */
} Gipoint;

/* Glnbundl - polyline bundle */
typedef struct {
        Gint            type;           /* line type */
        Gfloat          width;          /* linewidth scale factor */
        Gint            colour;         /* polyline colour index */
} Glnbundl;

/* Glnattr - polyline attributes */
typedef struct {
        Gasf            type;           /* linetype ASF */
        Gasf            width;          /* linewidth ASF */
        Gasf            colour;         /* linecolour ASF */
        Gint            line;           /* line index */
        Glnbundl        bundl;          /* line bundle */
} Glnattr;

/* Gpfcf - polyline/fill area flag */
typedef enum {
        GPF_POLYLINE,
        GPF_FILLAREA
} Gpfcf;

/* Gflinter - fill area interior style */
typedef enum {
        GHOLLOW,
        GSOLID,
        GPATTERN,
        GHATCH
} Gflinter;

/* Gflbundl - fill area bundle */
typedef struct {
        Gflinter        inter;          /* fill area interior style */
        Gint            style;          /* fill area style index */
        Gint            colour;         /* fill area colour index */
} Gflbundl;

/* Gflattr - fill area attributes */
typedef struct {
        Gasf            inter;          /* fill area interior style ASF */
        Gasf            style;          /* fill area style index ASF */
        Gasf            colour;         /* fill area colour ASF */
        Gint            fill;           /* fill area index */
        Gflbundl        bundl;          /* fill area bundle */
} Gflattr;

/* Glocrec - locator data record */
typedef union {
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet1;
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet2;
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet3;
        struct {
                Gacf    acf;            /* attribute control flag */
                Glnattr ln;             /* polyline attributes  */
                Gchar   *data;          /* device/implementation dependent data */
        } pet4;
        struct {
                Gpfcf   pfcf;           /* polyline/fill area control flag */
                Gacf    acf;            /* attribute control flag */
                union {
                        Glnattr ln;     /* polyline attributes */
                        Gflattr fl;     /* fill area attributes */
                } attr;
                Gchar   *data;          /* device/implementation dependent data */
        } pet5;
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet6;
        /* ... */                       /* more for implementatoin defined PETs */
} Glocrec;

/* Gdefloc - default locator data */
typedef struct {
        Gpoint          position;       /* initial position */
        Gintlist        pets;           /* list of prompted echo types */
        Glimit          e_area;         /* default echo area */
        Glocrec         record;         /* default locator data record */
} Gdefloc;

/* Gpickrec - pick data record */
typedef union {
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet1;
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet2;
        struct {
                Gchar   *data;          /* device/implementation dependent data */
        } pet3;
        /* ... */                       /* more for implementation defined PETs */
} Gpickrec;

/* Gdefpick - default pick data */
typedef struct {
        Gintlist        pets;           /* list of prompt and echo types */
        Glimit          e_area;         /* default echo area */
        Gpickrec        record;         /* default pick data record */
} Gdefpick;

/* Gstringrec - string data record */
typedef union {
        struct {
                Gint    bufsiz;         /* buffer size */
                Gint    position;       /* initial cursor position */
                Gchar   *data;          /* device/implementation dependent data */
        } pet1;
        /* ... */               /* more for implementation defined PETs */
} Gstringrec;

/* Gdefstring - default string data */
typedef struct {
        Gint            bufsiz;         /* initial buffer size */
        Gintlist        pets;           /* list of prompt and echo types */
        Glimit          e_area;         /* default echo area */
        Gstringrec      record;         /* default string data record */
} Gdefstring;

/* Gmkbundl - polymarker bundle */
typedef struct {
        Gint            type;           /* marker type */
        Gfloat          size;           /* marker size scale factor */
        Gint            colour;         /* polymarker colour index */
} Gmkbundl;

/* Gmkattr - polymarker attributes */
typedef struct {
        Gasf            type;           /* marker type ASF */
        Gasf            size;           /* marker size ASF */
        Gasf            colour;         /* marker colour ASF */
        Gint            mark;           /* marker index */
        Gmkbundl        bundl;          /* marker bundle */
} Gmkattr;

/* Gstrokerec - stroke data record */
typedef union {
        struct {
                Gint    bufsiz;         /* input buffer size */
                Gint    editpos;        /* editing position */
                Gpoint  interval;       /* x,y interval */
                Gfloat  time;           /* time interval */
                Gchar   *data;          /* device/implementation dependent data */
        } pet1;
        struct {
                Gint    bufsiz;         /* input buffer size */
                Gint    editpos;        /* editing position */
                Gpoint  interval;       /* x,y interval */
                Gfloat  time;           /* time interval */
                Gchar   *data;          /* device/implementation dependent data */
        } pet2;
        struct {
                Gint    bufsiz;         /* input buffer size */
                Gint    editpos;        /* editing position */
                Gpoint  interval;       /* x,y interval */
                Gfloat  time;           /* time interval */
                Gacf    acf;            /* attribute control falg */
                Gmkattr mk;             /* marker attributes */
                Gchar   *data;          /* device/implementation dependent data */
        } pet3;
        struct {
                Gint    bufsiz;         /* input buffer size */
                Gint    editpos;        /* editing position */
                Gpoint  interval;       /* x,y interval */
                Gfloat  time;           /* time interval */
                Gacf    acf;            /* attribute control falg */
                Glnattr ln;             /* line attributes */
                Gchar   *data;          /* device/implementation dependent data */
        } pet4;
        /* ... */               /* more for implementation defined PETs */
} Gstrokerec;

/* Gdefstroke - default stroke data */
typedef struct {
        Gint            bufsiz;         /* buffer size */
        Gintlist        pets;           /* list of prompt and echo types */
        Glimit          e_area;         /* adfault echo area */
        Gstrokerec      record;         /* default stroke data record */
} Gdefstroke;

/* Gvalrec - valuator data record */
typedef union {
        struct {
                Gfloat  low;            /* low range limit */
                Gfloat  high;           /* high range limit */
                Gchar   *data;          /* device/implementation dependent data */
        } pet1;
        struct {
                Gfloat  low;            /* low range limit */
                Gfloat  high;           /* high range limit */
                Gchar   *data;          /* device/implementation dependent data */
        } pet2;
        struct {
                Gfloat  low;            /* low range limit */
                Gfloat  high;           /* high range limit */
                Gchar   *data;          /* device/implementation dependent data */
        } pet3;
        /* ... */               /* more for implementation defined PETs */
} Gvalrec;

/* Gdefval - default valuator data */
typedef struct {
        Gfloat          value;          /*initial value */
        Gintlist        pets;           /* list of prompt and echo types */
        Glimit          e_area;         /* adfault echo area */
        Gvalrec         record;         /* default stroke data record */
} Gdefval;

/* Gdevunits - device coordinate units */
typedef enum {
        GDC_METRES,
        GDC_OTHER
} Gdevunits;

/* Gdspsize - display size */
typedef struct {
        Gdevunits       units;          /* device coordinate units */
        Gpoint          device;         /* device coordinate unit size */
        Gipoint         raster;         /* raster unit size */
} Gdspsize;

/* Gdspsurf - display surface */
typedef enum {
        GEMPTY,
        GNOTEMPTY
} Gdspsurf;

/* Gescin - escape input data record */
typedef union {
        struct {
                Gint    dummy;    /* escape dependent data record */
        } escid1; /* name of the specific escape function */
        struct {
                Gint    dummy;    /* escape dependent data record */
        } escid2; /* name of the specifice escape function */
        /* .... */ /* more for implementation defined PETS */
} Gescin;

/* Gescout - escape output data record */
typedef union {
        struct {
                Gint    dummy;  /* escape dependent data record */
        } escid1; /* name of the specific escape function */
        struct {
                Gint    dummy;  /* escape dependent data record */
        } escid2; /* name of the specifice escape function */
        /* .... */ /* more for implementation defined PETS */
} Gescout;

/* Giclass - input class */
typedef enum {
        GNCLASS,
        GLOCATOR,
        GISTROKE,
        GVALUATOR,
        GCHOICE,
        GPICK,
        GISTRING
} Giclass;

/* Gcvent - event */
typedef struct {
        Gint            ws;             /* workstation */
        Gint            dev;            /* device number */
        Giclass         class;          /* event class */
} Gevent;

/* Gextent - text extent */
typedef struct {
        Gpoint          concat;         /* concatenation point */
        Gpoint          ll;             /* lower left corner */
        Gpoint          lr;             /* lower right corner */
        Gpoint          ul;             /* upper left corner */
        Gpoint          ur;             /* upper right corner */
} Gextent;

/* Gfile - FILE objects type */
typedef FILE Gfile;

/* Gflfac - fill area facilities */
typedef struct {
        Gintlist        interiors;      /* list of available interior styles */
        Gintlist        hatches;        /* list of available styles */
        Gint            predefined;     /* number of predefined bundles */
} Gflfac;

/* Ggdpfac - GDP facilities */
typedef struct {
        Gint            number;         /* number of GDPs */
        Gattrs          *attrs;         /* list of attributes used */
} Ggdpfac;

/* Ggdplist - GDP list */
typedef struct {
        Gint            number;         /* number of GDPs */
        Gint            (*functions)(); /* array of points to GDP functions */
        Gint            *indices;       /* array of official registered GDP indices */
} Ggdplist;

/* Ggdprec - GDP data record */
typedef union {
        struct {
                Gint    dummy;  /*  GDP dependent data record */
        } gdpid1;  /* name of the specific GDP function */
        struct {
                Gint    dummy;  /*  GDP dependent data record */
        } gdpid2;  /* name of the specific GDP function */
        /* .... */ /*  more for implementation defined PETs */
} Ggdprec;

/* Ggksmit - GKS metafile item */
typedef struct {
        Gint            type;           /* item type */
        Gint            length;         /* item length */
} Ggksmit;

/* Gtxprec - text precision */
typedef enum {
        GSTRING,
        GCHAR,
        GSTROKE
} Gtxprec;

/* Gtxfp - text font and precision */
typedef struct {
        Gint            font;           /* text font */
        Gtxprec         prec;           /* text precision */
} Gtxfp;

/* Gindattr - individual attributes */
typedef struct {
        Gint            lntype;         /* current line type */
        Gfloat          lnwidth;        /* current linewidth scale factor */
        Gint            lncolour;       /* current polyline colour index */
        Gint            mktype;         /* current marker type */
        Gfloat          mksize;         /* current marker size scale factor */
        Gint            mkcolour;       /* current polymarker colour */
        Gtxfp           fp;             /* current font and precision structure */
        Gfloat          chexp;          /* current character expansion factor */
        Gfloat          chspace;        /* current character spacing */
        Gint            txcolour;       /* current text colour index */
        Gflinter        flstyle;        /* current fill area interior style */
        Gint            flindex;        /* current fill area style index */
        Gint            flcolour;       /* current fill area colour index */
        Gasfs           asflist;        /* aspect source flags */
} Gindattr;

/* Ginqtype - inquiry type */
typedef enum {
        GSET,
        GREALIZED
} Ginqtype, Gqtype;     /* two names due to an error in the C Binding */

/* Gistat - input status */
typedef enum {
        GOK,
        GNONE
} Gistat;

/* Glastev - last event */
typedef enum {
        GLAST,
        GNOTLAST
} Glastev;

/* Glevel - level of GKS */
typedef enum {
        GLMA = -3,
        GLMB,
        GLMC,
        GL0A,
        GL0B,
        GL0C,
        GL1A,
        GL1B,
        GL1C,
        GL2A,
        GL2B,
        GL2C
} Glevel;

/* Glnfac - polyline facilities */
typedef struct {
        Gintlist        types;          /* list of available line types */
        Gint            widths;         /* number of line widths  AUG */
        Gfloat          nom;            /* nominal width */
        Gfloat          min;            /* minimum width */
        Gfloat          max;            /* maximum width */
        Gint            predefined;     /* number of predefined bundles */
} Glnfac;

/* Glntype - line type */
/* The following are this implementation specific line style */
#define GLN_LDASH       -3      /* Long dash (-- -- -- --) */
#define GLN_DDOTDASH    -2      /* Double-Dot-Dash (.._.._.._) */
#define GLN_SDASH       -1      /* short dash (-  -  -  -) */
/* The folowing are standard defined */
#define GLN_SOLID       1
#define GLN_DASH        2       /* medium dash (- - - - -) */
#define GLN_DOT         3
#define GLN_DOTDASH     4

/* Gloc - locator data */
typedef struct {
        Gint            transform;      /* normalization transformation number */
        Gpoint          position;       /* locator position */
} Gloc;

/* Glocst - locator state */
typedef struct {
        Gimode          mode;           /* mode */
        Gesw            esw;            /* echo switch */
        Gloc            loc;            /* locator data */
        Gint            pet;            /* prompt and echo type */
        Glimit          e_area;         /* echo area */
        Glocrec         record;         /* locator data record */
} Glocst;

/* Gmkfac - polymarker  facilities */
typedef struct {
        Gintlist        types;          /* list of available marker  types */
        Gint            sizes;          /* number of marker size   AUG */
        Gfloat          nom;            /* nominal size */
        Gfloat          min;            /* minimum size */
        Gfloat          max;            /* maximum size */
        Gint            predefined;     /* number of predefined bundles */
} Gmkfac;

/* Gmktype - marker type */
#define GMK_POINT       1
#define GMK_PLUS        2
#define GMK_STAR        3
#define GMK_O           4
#define GMK_X           5

/* Gmodtype - dynamic modification type */
typedef enum {
        GIRG,
        GIMM
} Gmodtype;

/* Gmodseg - dynamic modification of segment attributes */
typedef struct {
        Gmodtype        transform;      /* transformation */
        Gmodtype        appear;         /* appearing (turning visible) */
        Gmodtype        disappear;      /* disappearing (turning invisible) */
        Gmodtype        highlight;      /* highlighting */
        Gmodtype        priority;       /* priority */
        Gmodtype        addition;       /* addition of primitive to segment */
        Gmodtype        deletion;       /* deletion of segment */
} Gmodseg;

/* Gmodws - dynamic modification of workstation attributes */
typedef struct {
        Gmodtype        line;           /* polyline */
        Gmodtype        mark;           /* polymarker */
        Gmodtype        text;           /* text */
        Gmodtype        fill;           /* fill area */
        Gmodtype        pat;            /* pattern */
        Gmodtype        colour;         /* colour */
        Gmodtype        wstran;         /* workstation transformation */
} Gmodws;

/* Gnframe - new frame action at update */
/* Gstore - workstation storage of non-segment primitives state */
typedef enum {
        GNO,
        GYES
} Gnframe, Gstore;

/*  Gnumdev - number of input device */
typedef struct {
        Gint            locator;        /* locators */
        Gint            stroke;         /* stroke */
        Gint            valuator;       /* valuator */
        Gint            choice;         /* choice */
        Gint            pick;           /* pick */
        Gint            string;         /* string */
} Gnumdev;

/* Gos - GKS operating state */
typedef enum {
        GGKCL,          /* GKS closed */
        GGKOP,          /* GKS open */
        GWSOP,          /* workstation open */
        GWSAC,          /* workstation active */
        GSGOP           /* segment open */
} Gos;

/* Gpstat - pick status */
typedef enum {
        GP_OK,
        GP_NOPICK,
        GP_NONE
} Gpstat;

/* Gpick - pick data */
typedef struct {
        Gpstat          status;         /* pick status */
        Gint            seg;            /* pick segment */
        Gint            pickid;         /* pick identifier */
} Gpick;

/* Gpickst - pick state */
typedef struct {
        Gimode          mode;           /* mode */
        Gesw            esw;            /* echo switch */
        Gpick           pick;           /* pick data */
        Gint            pet;            /* prompt and echo type */
        Glimit          e_area;         /* echo area */
        Gpickrec        record;         /* pick data record */
} Gpickst;

/* Gtxpath - text path */
typedef enum {
        GTP_RIGHT,
        GTP_LEFT,
        GTP_UP,
        GTP_DOWN
} Gtxpath;

/* Gtxhor - text alignment horizontal component */
typedef enum {
        GTH_NORMAL,
        GTH_LEFT,
        GTH_CENTRE,
        GTH_RIGHT
} Gtxhor;

/* Gtxver - text alignment vertical component */
typedef enum {
        GTV_NORMAL,
        GTV_TOP,
        GTV_CAP,
        GTV_HALF,
        GTV_BASE,
        GTV_BOTTOM
} Gtxver;

/* Gtxalign - text alignment */
typedef struct {
        Gtxhor          hor;            /* horizontal component */
        Gtxver          ver;            /* vertival component */
} Gtxalign;

/* Gpriattr - primary attributes */
typedef struct {
        Gint            plnindex;       /* current polyline index */
        Gint            pmkindex;       /* current polymarker index */
        Gint            txindex;        /* current text index */
        Gfloat          height;         /* current character height */
        Gpoint          up;             /* current character up vector */
        Gfloat          chwidth;        /* current character width */
        Gpoint          base;           /* current character base vector */
        Gtxpath         path;           /* current text path */
        Gtxalign        align;          /* current text alignment */
        Gint            flindex;        /* current fill area index */
        Gpoint          widthvec;       /* current pattern width vector */
        Gpoint          heightvec;      /* current pattern heigh vector */
        Gpoint          prp;            /* current pattern reference point */
} Gpriattr;


/* Gptbundl - pattern bundle */
typedef struct {
        Gipoint         size;           /* pattern array size */
        Gint            *array;         /* pattern array */
} Gptbundl;

/* Gpxarray - pixel array */
typedef struct {
        Gcovalid        covalid;        /* colour values valid */
        Gint            *array;         /* pixel array */
} Gpxarray;

/* Gqloc - request locator */
typedef struct {
        Gistat          status;         /* input status */
        Gloc            loc;            /* locator data */
} Gqloc;

/* Gqstring - request string */
typedef struct {
        Gistat          status;         /* input status */
        Gchar           *string;        /* string data */
} Gqstring;

/* Gstroke - stroke data */
typedef struct {
        Gint            transform;      /* normalization transformation number */
        Gint            n_points;       /* number of points in stroke */
        Gpoint          *points;        /* array of points in stroke */
} Gstroke;

/* Gqstroke - request stroke */
typedef struct {
        Gistat          status;         /* input status */
        Gstroke         stroke;         /* stroke data */
} Gqstroke;

/* Gqueue - queue information */
typedef struct {
        Giclass         class;          /* event class */
        Gint            devno;          /* logical input device number */
} Gqueue;

/* Gqval - request valuator */
typedef struct {
        Gistat          status;         /* input status */
        Gfloat          val;            /* valuator data */
} Gqval;

/* Grect - coordinate of rectangle */
typedef struct {
        Gpoint          ll;             /* lower left-hand corner */
        Gpoint          ur;             /* upper right-hand corner */
} Grect;

/* Gregen - regeneratoin flag */
typedef enum {
        GPERFORM,
        GPOSTPONE
} Gregen;

/* Gsegdet - segment detectability */
typedef enum {
        GUNDETECTABLE,
        GDETECTABLE
} Gsegdet;

/* Gseghi - segment highlighting */
typedef enum {
        GNORMAL,
        GHIGHLIGHTED
} Gseghi;

/* Gsegvis - segment visibility */
typedef enum {
        GVISIBLE,
        GINVISIBLE
} Gsegvis;

/* Gsegattr - segment attribute */
typedef struct {
        Gint            seg;            /* segment name */
        Gfloat          segtran[2][3];  /* segment transformation matrix */
        Gsegvis         vis;            /* visibility of segment */
        Gseghi          hilight;        /* highlighting of the segment */
        Gfloat          pri;            /* priority of the segment */
        Gsegdet         det;            /* detectability of the segment */
} Gsegattr;

/* Gsimultev - simultaneous events */
typedef enum {
        GNOMORE,
        GMORE
} Gsimultev;

/* Gstringst - string state */
typedef struct {
        Gimode          mode;           /* mode */
        Gesw            esw;            /* echo switch */
        Gchar           *string;        /* string data */
        Gint            pet;            /* prompt and echo type */
        Glimit          e_area;         /* echo area */
        Gstringrec      record;         /* string data record */
} Gstringst;

/* Gstrlist - string list */
typedef struct {
        Gint            number;         /* number of string in list */
        Gchar           **strings;      /* array of pointers to strings */
} Gstrlist;

/* Gstrokest - stroke state */
typedef struct {
        Gimode          mode;           /* mode */
        Gesw            esw;            /* echo switch */
        Gstroke         stroke;         /* stroke data */
        Gint            pet;            /* prompt and echo type */
        Glimit          e_area;         /* echo area */
        Gstrokerec      record;         /* stroke data record */
} Gstrokest;

/* Gtxbundl - text bundle */
typedef struct {
        Gtxfp           fp;             /* font and precision */
        Gfloat          ch_exp;         /* character expansion */
        Gfloat          space;          /* character spacing */
        Gint            colour;         /* text colour */
} Gtxbundl;

/* Gtxattr - text attributes */
typedef struct {
        Gasf            fp;             /* text font and precision ASF */
        Gasf            tx_exp;         /* text character expansion ASF */
        Gasf            space;          /* text character spacing ASF */
        Gasf            colour;         /* text colour ASF */
        Gint            text;           /* text index */
        Gtxbundl        bundl;          /* text bundle */
} Gtxattr;

/* Gtxfac - text facility */
typedef struct {
        Gint            fps;            /* number of fonts and precisions */
        Gtxfp           *fp_list;       /* list of fonts and precisions */
        Gint            heights;        /* number of character heights */
        Gfloat          min_ht;         /* minimum height */
        Gfloat          max_ht;         /* maximum height */
        Gint            expansions;     /* number of character expansion factors */
        Gfloat          min_ex;         /* minimum expansion factor */
        Gfloat          max_ex;         /* maximum expansion factor */
        Gint            predefined;     /* number of predefined bundles */
} Gtxfac;

/* Gvalst - valuator state */
typedef struct {
        Gimode          mode;           /* mode */
        Gesw            esw;            /* echo switch */
        Gfloat          val;            /* valuator data */
        Gint            pet;            /* prompt and echo type */
        Glimit          e_area;         /* echo area */
        Gvalrec         record;         /* valuator data record */
} Gvalst;


/* Gvpri - viewport input priority */
typedef enum {
        GHIGHER,
        GLOWER
} Gvpri;

/* Gwscat - workstation category */
typedef enum {
        GOUTPUT,
        GINPUT,
        GOUTIN,
        GWISS,
        GMO,
        GMI
} Gwscat;

/* Gwsclass - workstation classification */
typedef enum {
        GVECTOR,
        GRASTER,
        GOTHER
} Gwsclass;

/* Gwsct - workstation connection and type */
typedef struct {
        Gchar   *conn;          /* workstation connection */
        Gchar   *type;          /* workstation type */
} Gwsct;

/* Gwsdus - workstation deferral and update state */
typedef struct {
        Gdefmode        defmode;        /* deferral mode */
        Gdspsurf        dspsurf;        /* display surface */
        Girgmode        irgmode;        /* implicit regeneratoin mode */
        Gnframe         nframe;         /* new frame action update */
} Gwsdus;

/* Gwsmax - workstation maximum number */
typedef struct {
        Gint            open;           /* number of open workstations */
        Gint            active;         /* number of active workstations */
        Gint            assoc;          /* number of associated workstations */
} Gwsmax;

/* Gwsstate - workstation state */
typedef enum {
        GINACTIVE,
        GACTIVE
} Gwsstate;

/* Gwstable - length of workstation tables */
typedef struct {
        Gint            line;           /* polyline tables */
        Gint            mark;           /* polymarker tables */
        Gint            text;           /* text tables */
        Gint            fill;           /* fill area tables */
        Gint            pat;            /* pattern tables */
        Gint            colour;         /* colour tables */
} Gwstables;

/* Gwstus - workstation transformation update state */
typedef enum {
        GNOTPENDING,
        GPENDING
} Gwstus;

/* Gwstran - workstation transformation */
typedef struct {
        Glimit          w;              /* window */
        Glimit          v;              /* viewport */
} Gwstran;

/* Gwsti - workstation transformaton information */
typedef struct {
        Gwstus          wstus;          /* workstation transformation update state */
        Gwstran         request;        /* request transformatoin */
        Gwstran         current;        /* current transformatoin */
} Gwsti;

/* Error handling function prototypes */
Gint gerrorhand(Gint errnum, Gint funcname, Gfile *perrfile);

/* GKS description table */
Gint ginqavailwstypes(Gstrlist *wstypes);
Gint ginqlevelgks(Glevel *lev);
Gint ginqmaxntrannum(Gint *maxtran);
Gint ginqwsmaxnum(Gwsmax *maxws);

/* GKS state list function prototypes */
Gint ginqactivews(Gintlist *wsids);
Gint ginqclip(Gcliprec *clipping);
Gint ginqcurntrannum(Gint *tran);
Gint ginqcurpickid(Gint *pickid);
Gint ginqindivattr(Gindattr *indivattr);
Gint ginqinputoverflow(Gqueue *overflow);
Gint ginqmoreevents(Gsimultev *events);
Gint ginqntran(Gint num, Gwstran *tran);
Gint ginqntrannum(Gintlist *tranlist);
Gint ginqopenws(Gintlist *wsids);
Gint ginqopst(Gos *state);
Gint ginqprimattr(Gpriattr *primattr);
Gint gselntran(Gint trans);
Gint gsetasf(Gasfs *asf);
Gint gsetcharexpan(Gfloat expansion);
Gint gsetcharheight(Gfloat height);
Gint gsetcharspace(Gfloat spacing);
Gint gsetcharup(Gpoint *up_vector);
Gint gsetclip(Gclip ind);
Gint gsetfillcolourind(Gint colour);
Gint gsetfillcolourind(Gint idx);
Gint gsetfillind(Gint idx);
Gint gsetfillintstyle(Gflinter style);
Gint gsetfillstyleind(Gint idx);
Gint gsetlinecolourind(Gint idx);
Gint gsetlineind(Gint idx);
Gint gsetlinetype(Gint type);
Gint gsetlinewidth(Gfloat width);
Gint gsetmarkercolourind(Gint idx);
Gint gsetmarkerind(Gint idx);
Gint gsetmarkersize(Gfloat size);
Gint gsetmarkertype(Gint type);
Gint gsetpatrefpt(Gpoint *ref);
Gint gsetpatsize(Gpoint *siz);
Gint gsettextalign(Gtxalign *txalign);
Gint gsettextcolourind(Gint idx);
Gint gsettextfontprec(Gtxfp *txfp);
Gint gsettextind(Gint idx);
Gint gsettextpath(Gtxpath path);
Gint gsetviewport(Gint trans, Glimit *viewport);
Gint gsetviewportinputpri(Gint trans, Gint ref_trans, Gvpri priority);
Gint gsetwindow(Gint trans, Glimit *window);

/* Workstation description table prototypes */
Gint ginqavailgdp(Gchar *ws_type, Ggdplist *gdps);
Gint ginqcolourfacil(Gchar *ws_type, Gcofac *fac);
Gint ginqdefchoice(Gchar *ws_type, Gint dev, Gdefchoice *data);
Gint ginqdefdeferst(Gchar *ws_type, Gdefer *def);
Gint ginqdefloc(Gchar *ws_type, Gint dev, Gdefloc *data);
Gint ginqdefpick(Gchar *ws_type, Gint dev, Gdefpick *data);
Gint ginqdefstring(Gchar *ws_type, Gint dev, Gdefstring *data);
Gint ginqdefstroke(Gchar *ws_type, Gint dev, Gdefstroke *data);
Gint ginqdefval(Gchar *ws_type, Gint dev, Gdefval *data);
Gint ginqdisplayspacesize(Gchar *ws_type, Gdspsize *dspsz);
Gint ginqfillfacil(Gchar *ws_type, Gflfac *fac);
Gint ginqgdp(Gchar *ws_type, Gint function, Ggdpfac *fac);
Gint ginqlinefacil(Gchar *ws_type, Glnfac *fac);
Gint ginqmarkerfacil(Gchar *ws_type, Gmkfac *fac);
Gint ginqmaxwssttables(Gchar *ws_type, Gwstables *tables);
Gint ginqmodsegattr(Gchar *ws_type, Gmodseg *dyn);
Gint ginqmodwsattr(Gchar *ws_type, Gmodws *dyn);
Gint ginqnumavailinput(Gchar *ws_type, Gnumdev *num);
Gint ginqnumsegpri(Gchar *ws_type, Gint *numpri);
Gint ginqpatfacil(Gchar *ws_type, Gint *fac);
Gint ginqpredcolourrep(Gchar *ws_type, Gint index, Gcobundl *rep);
Gint ginqpredfillrep(Gchar *ws_type, Gint idx, Gflbundl *rep);
Gint ginqpredlinerep(Gchar *ws_type, Gint idx, Glnbundl *rep);
Gint ginqpredmarkerrep(Gchar *ws_type, Gint idx, Gmkbundl *rep);
Gint ginqpredpatrep(Gchar *ws_type, Gint idx, Gptbundl *rep);
Gint ginqpredtextrep(Gchar *ws_type, Gint idx, Gtxbundl *rep);
Gint ginqtextfacil(Gchar *ws_type, Gtxfac *fac);
Gint ginqwscategory(Gchar *ws_type, Gwscat *cat);
Gint ginqwsclass(Gchar *ws_type, Gwsclass *class);
Gint ginqwsconntype(Gint ws_id, Gwsct *ct);
Gint ginqwsst(Gint ws_id, Gwsstate *state);

/* Workstation state list */
Gint ginqchoicest(Gint ws_id, Gint dev, Gchoicest *state);
Gint ginqcolourindices(Gint ws_id, Gintlist *indices);
Gint ginqcolourrep(Gint ws_id, Gint idx, Gqtype type, Gcobundl *rep);
Gint ginqfillindices(Gint ws_id, Gintlist *idxlist);
Gint ginqfillrep(Gint ws_id, Gint idx, Gqtype type, Gflbundl *rep);
Gint ginqlineindices(Gint ws_id, Gintlist *idxlist);
Gint ginqlinerep(Gint ws_id, Gint idx, Gqtype type, Glnbundl *rep);
Gint ginqlocst(Gint ws_id, Gint dev, Gqtype type, Glocst *state);
Gint ginqmarkerindices(Gint ws_id, Gintlist *idxlist);
Gint ginqmarkerrep(Gint ws_id, Gint idx, Gqtype type, Gmkbundl *rep);
Gint ginqpatindices(Gint ws_id, Gintlist *idxlist);
Gint ginqpatrep(Gint ws_id, Gint idx, Gqtype type, Gptbundl *rep);
Gint ginqpickst(Gint ws_id, Gint dev, Gqtype type, Gpickst *state);
Gint ginqpixelarraydim(Gint ws_id, Grect *rect, Gipoint *dim);
Gint ginqpixelarray(Gint ws_id, Gpoint *point, Gipoint *dimen, Gpxarray *pxarr);
Gint ginqpixel(Gint ws_id, Gpoint *ppoint, Gint *pix);
Gint ginqsegnamesws(Gint ws_id, Gintlist *segs);
Gint ginqstringst(Gint ws_id, Gint dev, Gstringst *state);
Gint ginqstrokest(Gint ws_id, Gint dev, Gqtype type, Gstrokest *state);
Gint ginqtextextent(Gint ws_id, Gpoint position, Gchar *string, Gextent *extent);
Gint ginqtextindices(Gint ws_id, Gintlist *idxlist);
Gint ginqtextrep(Gint ws_id, Gint idx, Gqtype type, Gtxbundl *rep);
Gint ginqvalst(Gint ws_id, Gint dev, Gvalst *state);
Gint ginqwsconntype(Gint ws_id, Gwsct *ct);
Gint ginqwsdeferupdatest(Gint ws_id, Gwsdus *du);
Gint ginqwsst(Gint ws_id, Gwsstate *state);
Gint ginqwstran(Gint ws_id, Gwsti *wstran);
Gint gsetcolourrep(Gint ws_id, Gint idx, Gcobundl *rep);
Gint gsetdeferst(Gint ws_id, Gdefmode deferral_mode, Girgmode regen_mode);
Gint gsetfillrep(Gint ws_id, Gint idx, Gflbundl *rep);
Gint gsetlinerep(Gint ws_id, Gint idx, Glnbundl *rep);
Gint gsetmarkerrep(Gint ws_id, Gint idx, Gmkbundl *rep);
Gint gsetpatrep(Gint ws_id, Gint idx, Gptbundl *rep);
Gint gsettextrep(Gint ws_id, Gint idx, Gtxbundl *bundle);
Gint gsetwsviewport(Gint ws_id, Glimit *viewport);
Gint gsetwswindow(Gint ws_id, Glimit *window);

/* GKS control function prototypes */
Gint gactivatews(Gint ws_id);
Gint gclearws(Gint ws_id, Gclrflag control_flag);
Gint gclosegks(void);
Gint gclosews(Gint ws_id);
Gint gdeactivatews(Gint ws_id);
Gint gflushevents(Gint ws_id, Giclass class, Gint dev);
Gint gmessage(Gint ws_id, Gchar *string);
Gint gopengks(Gfile *err_file, Glong memory);
Gint gopenws(Gint ws_id, Gchar *connection, Gchar *ws_type);
Gint gredrawsegws(Gint ws_id);
Gint gupdatews(Gint ws_id, Gregen regenflag);

/* output primitive function prototypes */
Gint gcellarray(Grect *rect, Gipoint *dim, Gint row, Gint *colour);
Gint gfillarea(Gint num_pts, Gpoint *pts);
Gint ginqtextextent(Gint ws_id, Gpoint position, Gchar *string, Gextent *extent);
Gint gpolyline(Gint num_pts, Gpoint *pts);
Gint gpolymarker(Gint num_pts, Gpoint *pts);
Gint gtext(Gpoint *at, Gchar *string);

/* input function prototypes */
Gint gawaitevent(Gfloat timeout, Gevent *event);
Gint ggetchoice(Gchoice *response);
Gint ggetloc(Gloc *response);
Gint ggetpick(Gpick *response);
Gint ggetstring(Gchar *response);
Gint ggetstroke(Gstroke *response);
Gint ggetval(Gfloat *response);
Gint ginqinputoverflow(Gqueue *overflow);
Gint ginqmoreevents(Gsimultev *events);

/* CHOICE input function prototypes */
Gint ginitchoice(Gint ws_id, Gint dev, Gchoice *init, Gint pet, Glimit *area, Gchoicerec *record);
Gint ginqchoicest(Gint ws_id, Gint dev, Gchoicest *state);
Gint greqchoice(Gint ws_id, Gint dev, Gchoice *response);
Gint gsamplechoice(Gint ws_id, Gint dev, Gchoice *response);
Gint gsetchoicemode(Gint ws_id, Gint dev, Gimode mode, Gesw echo);

/* LOCATOR input function prototypes */
Gint ginitloc(Gint ws_id, Gint dev, Gloc *init, Gint pet, Glimit *area, Glocrec *record);
Gint ginqlocst(Gint ws_id, Gint dev, Gqtype type, Glocst *state);
Gint greqloc(Gint ws_id, Gint dev, Gqloc *response);
Gint gsampleloc(Gint ws_id, Gint dev, Gloc *response);
Gint gsetlocmode(Gint ws_id, Gint dev, Gimode mode, Gesw echo);

/* PICK input function prototypes */
Gint ginitpick(Gint ws_id, Gint dev, Gpick *init, Gint pet, Glimit *area, Gpickrec *record);
Gint ginqpickst(Gint ws_id, Gint dev, Gqtype type, Gpickst *state);
Gint greqpick(Gint ws_id, Gint dev, Gpick *response);
Gint gsamplepick(Gint ws_id, Gint dev, Gpick *response);
Gint gsetpickmode(Gint ws_id, Gint dev, Gimode mode, Gesw echo);

/* STRING input function prototypes */
Gint ginitstring(Gint ws_id, Gint dev, Gchar *init, Gint pet, Glimit *area, Gstringrec *record);
Gint ginqstringst(Gint ws_id, Gint dev, Gstringst *state);
Gint greqstring(Gint ws_id, Gint dev, Gqstring *response);
Gint gsamplestring(Gint ws_id, Gint dev, Gchar *response);
Gint gsetstringmode(Gint ws_id, Gint dev, Gimode mode, Gesw echo);

/* STROKE input device function prototypes */
Gint ginitstroke(Gint ws_id, Gint dev, Gstroke *init, Gint pet, Glimit *area, Gstrokerec *record);
Gint ginqstrokest(Gint ws_id, Gint dev, Gqtype type, Gstrokest *state);
Gint greqstroke(Gint ws_id, Gint dev, Gqstroke *response);
Gint gsamplestroke(Gint ws_id, Gint dev, Gstroke *response);
Gint gsetstrokemode(Gint ws_id, Gint dev, Gimode mode, Gesw echo);

/* metafile function prototypes */
Gint ggetgksm(Gint ws_id, Ggksmit *result);
Gint ginterpret(Ggksmit *recInfo, Gchar *data);
Gint greadgksm(Gint ws_id, Gint length, Gchar *record);
Gint gwritegksm(Gint ws_id, Gint type, Gint length, Gchar *data);

/* segment function prototypes */
Gint gassocsegws(Gint ws_id, Gint seg_id);
Gint gcloseseg(void);
Gint gcopysegws(Gint ws_id, Gint seg_id);
Gint gcreateseg(Gint name);
Gint gdelseg(Gint name);
Gint gdelsegws(Gint ws_id, Gint name);
Gint ginqassocws(Gint seg, Gintlist *asswk);
Gint ginqcurpickid(Gint *pickid);
Gint ginqnameopenseg(Gint *seg);
Gint ginqsegattr(Gsegattr *segattr);
Gint ginqsegnames(Gintlist *segs);
Gint ginsertseg(Gint seg_id, Gfloat segtran[2][3]);
Gint gredrawsegws(Gint ws_id);
Gint grenameseg(Gint old, Gint new);
Gint gsetpickid(Gint pick_id);
Gint gsetsegattr(Gint name, Gsegattr *segattr);

/* transformation function prototypes */
Gint gaccumtran(Gfloat segtran[2][3], Gpoint *ppoint, Gpoint *pshift, Gfloat angle, Gpoint *pscale, Gcsw coord, Gfloat result[2][3]);
Gint gevaltran(Gpoint *ppoint, Gpoint *pshift, Gfloat angle, Gpoint *pscale, Gcsw coord, Gfloat result[2][3]);

/* Enumerated types for error checking */

typedef enum { errnull,
errgopengks,
errgclosegks,
errgopenws,
errgclosews,
errgactivatews,
errgdeactivatews,
errgclearws,
errgredrawsegws,
errgupdatews,
errgsetdeferst,
errgmessage,
errgescredrawnotify,
errgescsetcolourmask,
errgescsetdcsize,
errgescstoreprimi,
errgescinqxattr,
errgpolyline,
errgpolymarker,
errgtext,
errgfillarea,
errgcellarray,
errggdp,
errgsetlineind,
errgsetlinetype,
errgsetlinewidth,
errgsetlinecolourind,
errgsetmarkerind,
errgsetmarkertype,
errgsetmarkersize,
errgsetmarkercolourind,
errgsettextind,
errgsettextfontprec,
errgsetcharexpan,
errgsetcharspace,
errgsettextcolourind,
errgsetcharheight,
errgsetcharup,
errgsettextpath,
errgsettextalign,
errgsetfillind,
errgsetfillintstyle,
errgsetfillstyleind,
errgsetfillcolourind,
errgsetpatsize,
errgsetpatrefpt,
errgsetasf,
errgsetpickid,
errgsetlinerep,
errgsetmarkerrep,
errgsetpatrep,
errgsetfillrep,
errgsettextrep,
errgsetcolourrep,
errgsetwindow,
errgsetviewport,
errgsetviewportinputpri,
errgselntran,
errgsetclip,
errgsetwswindow,
errgsetwsviewport,
errgcreateseg,
errgcloseseg,
errgrenameseg,
errgdelseg,
errgdelsegws,
errgassocsegws,
errgcopysegws,
errginsertseg,
errgsetsegtran,
errgsetvis,
errgsethighlight,
errgsetsegpri,
errgsetdet,
errgsetsegattr,
errginitloc,
errginitstroke,
errginitval,
errginitchoice,
errginitpick,
errginitstring,
errgsetlocmode,
errgsetstrokemode,
errgsetvalmode,
errgsetchoicemode,
errgsetpickmode,
errgsetstringmode,
errgreqloc,
errgreqstroke,
errgreqval,
errgreqchoice,
errgreqpick,
errgreqstring,
errgsampleloc,
errgsamplestroke,
errgsampleval,
errgsamplechoice,
errgsamplepick,
errgsamplestring,
errgawaitevent,
errgflushevents,
errggetloc,
errggetstroke,
errggetval,
errggetchoice,
errggetpick,
errggetstring,
errgwritegksm,
errggettypegksm,
errgreadgksm,
errggetgksm,
errginterpret,
errgevaltran,
errgaccumtran,
errgemergencyclosegks,
errgerrorhand,
errgerrorlog,
errginqactivews,
errginqasf,
errginqassocws,
errginqavailgdp,
errginqavailwstypes,
errginqchoicest,
errginqclip,
errginqcolourfacil,
errginqcolourindices,
errginqcolourrep,
errginqcurntrannum,
errginqdefchoice,
errginqdefdeferst,
errginqdefloc,
errginqdefpick,
errginqdefstring,
errginqdefstroke,
errginqdefval,
errginqdisplayspacesize,
errginqdisplaysize,
errginqfillfacil,
errginqfillindices,
errginqfillrep,
errginqgdp,
errginqindivattr,
errginqinputoverflow,
errginqlevelgks,
errginqlinefacil,
errginqlineindices,
errginqlinerep,
errginqlocst,
errginqmarkerfacil,
errginqmarkerindices,
errginqmarkerrep,
errginqmaxntrannum,
errginqmaxwssttables,
errginqmodsegattr,
errginqmodwsattr,
errginqmoreevents,
errginqnameopenseg,
errginqntrannum,
errginqntran,
errginqnumavailinput,
errginqnumsegpri,
errginqopenws,
errginqopst,
errginqpatfacil,
errginqpatindices,
errginqpatrep,
errginqcurpickid,
errginqpickst,
errginqpixel,
errginqpixelarray,
errginqpixelarraydim,
errginqpredcolourrep,
errginqpredfillrep,
errginqpredlinerep,
errginqpredmarkerrep,
errginqpredpatrep,
errginqpredtextrep,
errginqprimattr,
errginqsegattr,
errginqsegnames,
errginqsegnamesws,
errginqstringst,
errginqstrokest,
errginqtextextent,
errginqtextfacil,
errginqtextindices,
errginqtextrep,
errginqvalst,
errginqwscategory,
errginqwsclass,
errginqwsconntype,
errginqwsdeferupdatest,
errginqwsmaxnum,
errginqwsst,
errginqwstran
} Gerrmap;

typedef enum {
errXgksChoUpdatePrompt = 1000,
errXgksEnqueueEvent,
errXgksLocUpdatePrompt,
errXgksStrUpdatePrompt,
errXgksPicUpdatePrompt,
errXgksInsertMesgPrimi,
errXgksDuplicatePrimi,
errXgksSegPrimiTran,
errXgksStkUpdatePrompt,
errXgksValUpdatePrompt,
errXgksDistFillarea,
errXgksInitWssFillArea,
errXgksDistCellarray,
errXgksExecData,
errxPolyMarker,
errxFillArea,
errxInqPixelarray
} Gfuncerrmap;


/*
 * British spellings of GKS terms.
 *
 *      metre           meter
 *      centre          center
 *      colour          colour
 *      initialise      initialize      (not used in the code)
 */

#define GDC_METERS      GDC_METRES
#define GTH_CENTER      GTH_CENTRE
#define GCOLOR          GCOLOUR

#define ginqcolorfacil          ginqcolourfacil
#define ginqcolorrep            ginqcolourrep
#define ginqcolorindices        ginqcolourindices
#define ginqpredcolorrep        ginqpredcolourrep
#define gsetcolorrep            gsetcolourrep
#define gsetfillcolorind        gsetfillcolourind
#define gsetlinecolorind        gsetlinecolourind
#define gsetmarkercolorind      gsetmarkercolourind
#define gsettextcolorind        gsettextcolourind
#define gescsetcolormask        gescsetcolourmask

#define ln_color        ln_colour
#define mk_color        mk_colour
#define tx_color        tx_colour
#define fl_color        fl_colour
#define colors          colours
#define color           colour
#define lncolor         lncolour
#define mkcolor         mkcolour
#define txcolor         txcolour
#define flcolor         flcolour

#endif
