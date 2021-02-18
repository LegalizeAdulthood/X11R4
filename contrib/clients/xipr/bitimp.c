/* bitimp.c - generic bitmap to imPress program */

/* Based (loosely) on Dan Winkler's <winkler@harvard> version for
   UNIX, based on Ed Pattermann's (at Stanford) version for TOPS20.

   Also based on Walter Underwood's <wunder@ford-wdl1> program called
   sunimp, which was based on Winkler's macimp.

   Converted to X11 by Dana Chee (dana@bellcore.com)
*/


#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "newimPcodes.h"


#define	NOTOK		(-1)
#define	OK		0


#define	NULLCP		((char *) 0)

#define	istr(s)		((s) && *(s))

/*  */

#define	WHITE		 0	/* should be in newimPcodes.h */
#define	SHADE		 3
#define	OR		 7
#define	BLACK		15


#define	RESOLUTION	300


#define	GenH		512
#define	GenV		512
#define	GenS		8

#define	MagMin		0
#define	MagM		1
#define	MagMax		2


#define	BitRd1()	Rd1 (BitIn)
#define	BitRd4()	Rd4 (BitIn)


#define	Rd1(f)		getc (f)	/* nothing fancy here... */


#define	ImWrt1(s)	Wrt1 (ImOut, s)
#define	ImWrt2(s)	Wrt2 (ImOut, s)
#define	ImWrtS(adr,len)	WrtS (ImOut, adr, len, verbose ? VbOut : NULL)

#define	ImWrtC(s)	{ \
	    static char buffer[] = s; \
	    ImWrtS (buffer, sizeof buffer - 1); \
	}
#define	ImWrtD(s)	ImWrtS (s, strlen (s))


#define	Wrt1(f,c)	putc (c & 0xff, f)

/*  */

typedef int (*IFP) ();

#ifdef	is68k
int	ISwork ();
#endif
int     MACwork (), SUNwork (), XWDwork (), GENwork ();

struct symbol {
    char   *sy_name;
    IFP	    sy_fnx;
}               symbols[] = {
		    "genimp", GENwork,
#ifdef	is68k
		    "isimp",  ISwork,
#endif
                    "macimp", MACwork,
                    "sunimp", SUNwork,
                    "xwdimp", XWDwork,
		    NULL,     XWDwork
};

struct symbol  *sy;

/*  */

int	debug = 0;
int	gensplit = GenS;
int	genheight = GenV;
int	genwidth = GenH;
int	landscape = 0;
int	magnification = MagM;
int	operation = SHADE;
int	pageno = 0;
int	PixPerIn = RESOLUTION;
int	PixPerInX = RESOLUTION;
int	PixPerInY = RESOLUTION;
int	print = 1;
int	remove = 0;
int	reverse = 0;
int	StdOutput = 0;
int	temporary = 0;
int	verbose = 0;

char   *myname = "bitimp";
char   *banner = NULL;
char   *copies = NULL;
char   *document = NULL;
char   *filename = NULL;
char   *header = "on";
char   *host = NULL;
char   *infile = NULL;
char   *jobname = NULL;
char   *outfile = NULL;
char   *printer = NULL;
char   *user = NULL;


FILE   *BitIn;
FILE   *ImOut;
FILE   *VbOut = stderr;

int	sigser ();


char   *calloc (), *mktemp ();

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    arginit (argv);
    siginit ();
    
    BitJob ();
    imPjob ();

    (*sy -> sy_fnx) ();

    BitFin ();
    imPfin ();
    
    exit (0);
}

/*    ARGS */

arginit (vec)
char  **vec;
{
    char   *cp;
    struct symbol  *sp;

    if (myname = rindex (*vec, '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = *vec;

    StdOutput = !isatty (fileno (stdout));

    for (sy = symbols; sy -> sy_name; sy++)
	if (strcmp (sy -> sy_name, myname) == 0)
	    break;

    for (vec++; *vec; vec++)
	if (*(cp = *vec) == '-')
	    switch (*++cp) {
		case 'a': 
		    if (!*++cp)
			cp = *++vec;
		    for (sp = symbols; sp -> sy_name; sp++)
			if (strncmp (sp -> sy_name, cp, strlen (cp)) == 0)
			    break;
		    if (sp -> sy_name)
			sy = sp;
		    break;

		case 'b': 
		    banner = *++cp ? cp : *++vec;
		    break;

		case 'c': 
		    copies = *++cp ? cp : *++vec;
		    break;

		case 'd': 
		    debug = 1;
		    break;

		case 'f': 
		    filename = *++cp ? cp : *++vec;
		    break;

		case 'g': 
		    switch (*++cp) {
			case 's':
			    gensplit = atoi (*++cp ? cp : *++vec);
			    if (gensplit != GenS || gensplit != 0)
				adios (NULLCP, "bad value to -gs");
			    break;

			case 'h': 
			case 'x': 
			    genwidth = atoi (*++cp ? cp : *++vec);
			    break;

			case 'v': 
			case 'y': 
			    genheight = atoi (*++cp ? cp : *++vec);
			    break;

			default: 
			    genheight = genwidth = atoi (*++cp ? cp : *++vec);
			    break;
		    }
		    break;

		case 'h': 
		    header = "off";
		    break;

		case 'i': 
		    outfile = *++cp ? cp : *++vec;
		    print = StdOutput = 0;
		    break;

		case 'l':
		    landscape++;
		    break;

		case 'p': 
		    print = 1;
		    StdOutput = 0;
		    break;

		case 'r': 
		    remove = 1;
		    break;

		case 's': 
		    StdOutput = 1;
		    print = 0;
		    break;

		case 't': 
		    reverse = !reverse;
		    break;

		case 'v': 
		    verbose = 1;
		    break;

		case 'D': 
		    document = ++cp;
		    break;

		case 'H': 
		    host = ++cp;
		    break;

		case 'J': 
		    jobname = ++cp;
		    break;

		case 'M': 
		    magnification = atoi (++cp);
		    if (magnification < MagMin || magnification > MagMax)
			adios (NULLCP, "bad value to -M");
		    break;

		case 'P': 
		    printer = *++cp ? cp : *++vec;
		    break;

		case 'R': 
		    PixPerIn = atoi (++cp);
		    break;

		case 'U': 
		    user = ++cp;
		    break;

		default: 
		    adios (NULLCP, "unknown switch \"%s\"", *vec);
	    }
	else
	    if (infile)
		adios (NULLCP, "too many input files (starting with %s)", *vec);
	    else
		infile = *vec;

    if (!sy -> sy_name) {
	for (sp = symbols; sp -> sy_name; sp++)
	    if (sp -> sy_fnx == sy -> sy_fnx)
		break;
	if (!sp -> sy_name)
	    adios (NULLCP, "chaos at symbols[%d]", sy - symbols);
	sy = sp;
    }

    if (verbose)
	fprintf (VbOut, "Mode=%s Magnification=%d PixPerIn=%d\n",
		sy -> sy_name, magnification, PixPerIn);
}

/*    Bit */

BitJob () {
    if (!istr (infile) || !strcmp (infile, "-")) {
	if (!istr (banner))
	    banner = "(stdin)";
	BitIn = stdin;
	if (verbose)
	    fprintf (VbOut, "Input file is (stdin)\n");
    }
    else
	if ((BitIn = fopen (infile, "r")) == NULL)
	    adios (infile, "unable to read");
	else
	    if (verbose)
		fprintf (VbOut, "Input file is %s\n", infile);
}

BitFin () {
    struct stat st;

    if (remove
	    && BitIn != stdin
	    && stat (infile, &st) != NOTOK
	    && (st.st_mode & S_IFMT) == S_IFREG)
	(void) unlink (infile);
}

/*    imP */

imPjob () {
    static char tmpfile[BUFSIZ];

    if (StdOutput)
	outfile = "-";
    else
	if (!istr (outfile)) {
	    outfile = mktemp (sprintf (tmpfile, "/tmp/%sXXXXXX", myname));
	    temporary = 1;
	}
    if (!strcmp (outfile, "-")) {
	ImOut = stdout;
	if (verbose)
	    fprintf (VbOut, "Output file is (stdout)\n");
    }
    else
	if ((ImOut = fopen (outfile, "w")) == NULL)
	    adios (outfile, "unable to write");
	else
	    if (verbose)
		fprintf (VbOut, "Output file is %s%s\n",
		    outfile, print ? " (spool)" : "");

    ImWrtC ("@document(language imPress, jobheader ");
    ImWrtD (header);
    ImWrtC (", name \"");
    if (istr (jobname))
	ImWrtD (jobname);
    else
	if (istr (filename))
	    ImWrtD (filename);
	else
	    if (istr (infile))
		ImWrtD (infile);
	    else
		if (istr (banner))
		    ImWrtD (banner);
		else
		    ImWrtC ("(stdin)");
    ImWrtC ("\"");

/*  */

    if (istr (document)) {
	ImWrtC (", ");
	ImWrtD (document);
    }
    if (istr (user)) {
	ImWrtC (", owner ");
	ImWrtD (user);
    }
    if (istr (host)) {
	ImWrtC (" at ");
	ImWrtD (host);
	ImWrtC (", site ");
	ImWrtD (host);
    }
    if (istr (copies)) {
	ImWrtC (", copies ");
	ImWrtD (copies);
	ImWrtC (", pagecollation");
    }
    if (reverse)
	ImWrtC (", pagereversal");
    ImWrtC (")");
    if (verbose)
	fprintf (VbOut, "\n");
}

/*  */

imPfin () {
    int     vecp;
    char   *vec[10];

    if (verbose && pageno > 0)
	fprintf (VbOut, "\n");

    ImWrt1 (imP_EOF);

    if (ImOut != stdout && fclose (ImOut))
	adios (outfile, "output error");

    if (print && ImOut != stdout) {
	vecp = 0;
	vec[vecp++] = "lpr";
	vec[vecp++] = "-v";
	if (istr (printer))
	    vec[vecp++] = "-P", vec[vecp++] = printer;
	if (temporary)
	    vec[vecp++] = "-r";
	vec[vecp++] = outfile;
	vec[vecp] = NULL;
	execvp ("lpr", vec);
	adios ("lpr", "unable to exec");
    }
}

/*    imPress */

#define	PSIZE		32
#define	PBYTES		4

typedef char patch[PSIZE][PBYTES];

int	patchH;
int	patchV;
int	patchP;

patch  *patches;


imPbegin (pixperinx, pixperiny, width, height)
int	pixperinx,
	pixperiny,
	width,
	height;
{
    PixPerInX = pixperinx;
    PixPerInY = pixperiny;
    if (verbose)
	fprintf (VbOut, "PixPerInX=%d PixPerInY=%d height=%d width=%d\n",
		PixPerInX, PixPerInY, height, width);

    if (width % PSIZE)
	adios (NULLCP, "bitmap width %d not a multiple of %d", width, PSIZE);

    patchH = (width + (PSIZE - 1)) / PSIZE;
    patchV = (height + (PSIZE - 1)) / PSIZE;
    patchP = (patchH * PBYTES) * (patchV * PSIZE) - (patchH * PBYTES) * height;
    if (verbose)
	fprintf (VbOut, "patchH=%d patchV=%d patchP=%d\n",
		patchH, patchV, patchP);

    imPbop ();
    
    if ((patches = (patch *) calloc ((unsigned) patchH, sizeof (patch)))
	    == NULL)
	adios (NULLCP, "insufficient memory for patch array");
}

/*  */

imPbop () {
    if (landscape)
	ImWrt1 (imP_SET_HV_SYSTEM), ImWrt1 (0105);

    ImWrt1 (imP_SET_ABS_H), ImWrt2 (PixPerInX);

    ImWrt1 (imP_SET_ABS_V), ImWrt2 (PixPerInY);

    ImWrt1 (imP_SET_MAGN), ImWrt1 (magnification);

    ImWrt1 (imP_BITMAP), ImWrt1 (operation), ImWrt1 (patchH), ImWrt1 (patchV);
}


#define	imPeop	imPend

imPend () {
    register int    i;

    for (i = 0; i < patchP; i++)
	imPmap ((unsigned char) 0x00);

    ImWrt1 (imP_ENDPAGE);

    pageno++;
    if (verbose)
        fprintf (VbOut, "%s[%d]%s", pageno % 10 != 1 ? " " : "", pageno,
	    pageno % 10 == 0 ? "\n" : "");
}

/*  */

imPmap (c)
unsigned char    c;
{
    register int    i,
                    j,
                    k;
    static int  n,
                row,
                col;

    patches[n][row][col] = c;

    if (++col >= PBYTES) {
	col = 0;
	if (++n >= patchH) {
	    n = 0;
	    if (++row >= PSIZE) {
		row = 0;
		for (i = 0; i < patchH; i++)
		    for (j = 0; j < PSIZE; j++)
			for (k = 0; k < PBYTES; k++)
			    ImWrt1 (patches[i][j][k]);
	    }
	}
    }
}

/*    GEN */

GENwork () {
    int     c,
            i,
            j;

    imPbegin (PixPerIn, PixPerIn, genwidth, genheight);

    if (patchP)
	gensplit = 0;
    if (gensplit) {
	i = 0;
	j = (genwidth * genheight) / gensplit;
    }

    while ((c = BitRd1 ()) != EOF) {
	imPmap ((unsigned char) c);
	if (gensplit && ++i == j)
	    i = 0, imPeop (), imPbop ();
    }

    if (gensplit || i)
	imPend ();
}

/*    IS68K */

#ifdef	is68k
#include <bitmap.h>


ISwork () {
    register int    c,
                    i,
                    j,
		    k,
		    slop;
    struct bitmap   bm;

    if (fread ((char *) &bm, sizeof bm, 1, BitIn) != 1)
	adios (NULLCP, "eof reading bitmap header");
    if (bm.b_magic != BMAGIC)
	advise (NULLCP, "bad IS68K magic number 0x%x, expecting 0x%x",
	    bm.b_magic, BMAGIC);

    slop = (i = bm.b_width % PSIZE) ? PSIZE - i : 0;

    imPbegin (bm.b_hres, bm.b_vres, bm.b_width + slop, bm.b_height);

    k = ((bm.b_width - 1) / 8) + 1;
    if (slop)
	slop = (bm.b_width + slop) / 8 - k;

    for (i = bm.b_height; i > 0; i--) {
	for (j = k; j > 0; j--)
	    if ((c = BitRd1 ()) != EOF)
		imPmap ((unsigned char) (c ^ 0xff));
	    else
		adios (NULLCP, "eof reading raster");

	if (k % 2)			/* IS pads to short boundaries */
	    (void) BitRd1 ();

	for (j = slop; j > 0; j--)	/* Imagen pads to PSIZE boundaries */
	    imPmap ((unsigned char) 0x00);
    }

    imPend ();
}
#endif

/*    MAC */

#define	MacHdrSize	512

#define	MacH		576
#define	MacV		720


MACwork () {
    register int    c,
                    i,
                    n;

    for (i = 0; i < MacHdrSize; i++)
	(void) BitRd1 ();

    imPbegin (PixPerIn, PixPerIn, MacH, MacV);

    while ((c = BitRd1 ()) != EOF)
	if (c > 127)
	    for (i = 0, n = 256 - c + 1, c = BitRd1 (); i < n; i++)
		imPmap ((unsigned char) c);
	else
	    for (i = 0, n = c + 1; i < n; i++)
		imPmap ((unsigned char) BitRd1 ());

    imPend ();
}

/*    SUN */

#include "rasterfile.h"


SUNwork () {
    register int    c,
                    i;
    struct rasterfile   rf;

    rf.ras_magic = BitRd4 ();
    if (feof (BitIn))
	exit (0);
    if (rf.ras_magic != RAS_MAGIC)
	advise (NULLCP, "bad SUN magic number 0x%x, expecting",
	    rf.ras_magic, RAS_MAGIC);
    rf.ras_width = BitRd4 ();
    rf.ras_height = BitRd4 ();
    if ((rf.ras_depth = BitRd4 ()) != 1)
	adios (NULLCP, "pixel depth too deep (%d)", rf.ras_depth);
    if ((rf.ras_encoding = BitRd4 ()) != 0)
	adios (NULLCP, "bitmap not encoded raw (%d)", rf.ras_encoding);
    if ((rf.ras_type = BitRd4 ()) != 0)
	advise (NULLCP, "bitmap type non-zero (%d)", rf.ras_type);
    rf.ras_maptype = BitRd4 ();
    rf.ras_maplength = BitRd4 ();
    for (i = 0; i < rf.ras_maplength; i++)
	(void) BitRd1 ();

    imPbegin (PixPerIn / 4, PixPerIn, rf.ras_width, rf.ras_height);

    while ((c = BitRd1 ()) != EOF)
	imPmap ((unsigned char) c);

    imPend ();
}

/*    XWD */

#include <X11/Xlib.h>
#include <X11/XWDFile.h>


XWDwork () {
    int	    i,
	    j,
	    k,
    	    l,
	    slop;
    unsigned char tail;
    unsigned nbytes;
    register char *cp;
    char  *raster;
    XWDFileHeader xwd;

    if ((i = XReadDumpFP (BitIn, &xwd, (char **) 0, &raster)) <= 0)
	adios (NULLCP, "window dump file is in bad format");

    k = xwd.bytes_per_line;		/* # of stored bytes per line */
    l = (xwd.pixmap_width + 7) / 8;	/* # of useful bytes per line */
    
    slop = (i = k % PBYTES) ? PBYTES - i : 0;	/* how many more bytes needed*/

    imPbegin (PixPerIn, PixPerIn, (k + slop)*8, xwd.pixmap_height);

    tail = (i = xwd.pixmap_width % 8) ? 8 - i : 0;
    tail = (unsigned)0xff << tail;
    
    cp = raster;
    for (i = xwd.pixmap_height; i > 0; i--) {
	for (j = l; j > 1; j--)
	    imPmap ((unsigned char) (*cp++));

	imPmap((unsigned char) (*cp++ & tail)); /* clip the last byte */

	for (j = (k - l); j > 0; j--)	/* put out extra bytes */
		imPmap((unsigned char) (*cp++ & 0x00));
	
	for (j = slop; j > 0; j--)	/* Imagen pads to PBYTES boundaries */
	    imPmap ((unsigned char) 0x00);
    }

    imPend ();

    free ((char *) raster);
}

/*    I/O */

int	Rd2 (stream)
FILE   *stream;
{
    register int    value;

    value = Rd1 (stream) << 8;
    value |= Rd1 (stream) & 0xff;
    return value;
}


int	Rd4 (stream)
FILE   *stream;
{
    register int    value;

    value = Rd2 (stream) << 16;
    value |= Rd2 (stream) & 0xffff;
    return value;
}

/*  */

WrtS (stream, s, n, aux)
FILE   *stream,
       *aux;
char   *s;
int	n;
{
    if (n <= 0)
	return;

    if (aux)
	(void) fwrite (s, 1, n, aux);
    if (fwrite (s, 1, n, stream) != n)
	adios (NULLCP, "output error");
}


Wrt2 (stream, c)
FILE   *stream;
int	c;
{
    Wrt1 (stream, (c >> 8) & 0xff);
    Wrt1 (stream, c & 0xff);
}

/*    SIGNALS */

siginit () {
    if (signal (SIGHUP, SIG_IGN) != SIG_IGN)
	(void) signal (SIGHUP, sigser);
    if (signal (SIGQUIT, SIG_IGN) != SIG_IGN)
	(void) signal (SIGQUIT, sigser);
    if (signal (SIGINT, SIG_IGN) != SIG_IGN)
	(void) signal (SIGINT, sigser);
}


/* ARGSUSED */

sigser (sig, code, sc)
int	sig;
long	code;
struct sigcontext *sc;
{
    exit (1);
}

/*    ERROR HANDLING */

/* VARARGS2 */

advise (what, fmt, a, b, c, d)
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d;
{
    (void) fflush (stdout);

    fprintf (stderr, "%s: ", myname);
    fprintf (stderr, fmt, a, b, c, d);
    if (what)
	(void) putc (' ', stderr), perror (what);
    else
	(void) putc ('\n', stderr);
    (void) fflush (stderr);
}


/* VARARGS2 */

adios (what, fmt, a, b, c, d)
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d;
{
    advise (what, fmt, a, b, c, d);
    exit (1);
}

