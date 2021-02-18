/* $Header: /u/04c/mnt/integ/tools/src/RCS/snap.c,v 1.2 89/02/13 14:40:38 ejh Exp $ */

/* $Log:	snap.c,v $
 * Revision 1.2  89/02/13  14:40:38  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/10/03  13:45:43  ljb
 * Initial revision
 *  */

/*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XWDFile.h>
#include <X11/cursorfont.h>
#include <sys/uio.h>
#include <pwd.h>

/* -------------------------------------------------------------------*/

#define W_MAX 2400
#define H_MAX 3150

#define PAPER_WIDTH 85*30 /* 8.5 inches */
#define PAPER_LENGTH 11*300 /* 11 inches */

#define min(x,y) (((x)<(y))?(x):(y))

#define F_PORTRAIT 1
#define F_LANDSCAPE 2
#define F_REPORT 64
#define F_COMPACT 128
#define F_INVERT 256
#define LINELEN 72 /* number of CHARS per line of bitmap output */

#define COMMENTVERSION "PS-Adobe-1.0"

enum orientation {
    UNSPECIFIED = -1,
    PORTRAIT = 0,
    LANDSCAPE = 1,
    UPSIDE_DOWN = 2,
    LANDSCAPE_LEFT = 3,
};


/* mapping tables to map a byte in to the hex representation of its
 * bit-reversal
 */
char hex1[]="084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f\
084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f\
084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f\
084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f084c2a6e195d3b7f";

char hex2[]="000000000000000088888888888888884444444444444444cccccccccccccccc\
2222222222222222aaaaaaaaaaaaaaaa6666666666666666eeeeeeeeeeeeeeee\
111111111111111199999999999999995555555555555555dddddddddddddddd\
3333333333333333bbbbbbbbbbbbbbbb7777777777777777ffffffffffffffff";

char _reverse_byte[0x100] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

char *ps_epilog[] = {
	"",
	"showpage",
	"%%Trailer",
	0
};


Display *dpy;
char *obuf; /* buffer to contain entire rotated bit map */

/* ---------------------------------------------------------------------*\
| Routine: Select_Window;
|
| Description:
|	Allow user to select a window using the mouse and returns
	window id.
\*----------------------------------------------------------------------*/
Window Select_Window(dpy)
     Display *dpy;
{
  int status;
  Cursor cursor;
  XEvent event;
  Window target_win = None;
  int buttons = 0;
  int screen;

  /* Make the target cursor */
  cursor = XCreateFontCursor(dpy, XC_crosshair);
  screen = DefaultScreen(dpy);

  /* Grab the pointer using target cursor, letting it room all over */
  status = XGrabPointer(dpy, RootWindow(dpy, screen), False,
			ButtonPressMask|ButtonReleaseMask, GrabModeSync,
			GrabModeAsync, None, cursor, CurrentTime);
  if (status != GrabSuccess) {
	fprintf(stderr, "Can't grab the mouse.");
	exit(1);
  }

  /* Let the user select a window... */
  while ((target_win == None) || (buttons != 0)) {
    /* allow one more event */
    XAllowEvents(dpy, SyncPointer, CurrentTime);
    XWindowEvent(dpy, RootWindow(dpy, screen),
		 ButtonPressMask|ButtonReleaseMask, &event);
    switch (event.type) {
    case ButtonPress:
      if (target_win == None) {
	target_win = event.xbutton.subwindow; /* window selected */
	if (target_win == None)
	  target_win = RootWindow(dpy, screen);
      }
      buttons++;
      break;
    case ButtonRelease:
      if (buttons > 0) /* there may have been some down before we started */
	buttons--;
       break;
    }
  } 

  XUngrabPointer(dpy, CurrentTime);      /* Done with pointer */

  return(target_win);
}

/* ---------------------------------------------------------------------*\
| Routine: Get_XColors
|
| Description:
|	get xcolors of all pixels in image and return # of colors
\*----------------------------------------------------------------------*/
int Get_XColors(win_info, colors)
     XWindowAttributes *win_info;
     XColor **colors;
{
    int i, ncolors;

    if (!win_info->colormap)
	return(0);

    if (win_info->visual->class == TrueColor ||
	win_info->visual->class == DirectColor)
	return(0);    /* XXX punt for now */

    ncolors = win_info->visual->map_entries;
    if (!(*colors = (XColor *) malloc (sizeof(XColor) * ncolors))) {
	fprintf(stderr,"Out of memory!");
	exit(1);
    }

    for (i=0; i<ncolors; i++)
      (*colors)[i].pixel = i;

    XQueryColors(dpy, win_info->colormap, *colors, ncolors);
    
    return(ncolors);
}


#ifdef XPROLOG
/* for debugging, get the prolog from a file */
dump_prolog(flags) {
    char *fname=(flags & F_COMPACT) ? "prolog.compact" : "prolog";
    FILE *fi = fopen(fname,"r");
    char buf[1024];

    if (fi==NULL) {
	perror(fname);
	exit(1);
    }
    while (fgets(buf,1024,fi)) fputs(buf,stdout);
    fclose(fi);
}

#else XPROLOG
/* postscript "programs" to unpack and print the bitmaps being sent */

char *ps_prolog_compact[] = {
    "%%Pages: 1",
    "%%EndProlog",
    "%%Page: 1 1",
    "",
    "/bitgen",
    "	{",
    "		/nextpos 0 def",
    "		currentfile bufspace readhexstring pop % get a chunk of input",
    "		% interpret each byte of the input",
    "		{",
    "			flag { % if the previous byte was FF",
    "				/len exch def % this byte is a count",
    "				result",
    "				nextpos",
    "				FFstring 0 len getinterval % grap a chunk of FF's",
    "					putinterval % and stuff them into the result",
    "				/nextpos nextpos len add def",
    "				/flag false def",
    "			}{ % otherwise",
    "				dup 255 eq { % if this byte is FF",
    "					/flag true def % just set the flag",
    "					pop % and toss the FF",
    "				}{ % otherwise",
    "					% move this byte to the result",
    "					result nextpos",
    "						3 -1 roll % roll the current byte back to the top",
    "						put",
    "					/nextpos nextpos 1 add def",
    "				} ifelse",
    "			} ifelse",
    "		} forall",
    "		% trim unused space from end of result",
    "		result 0 nextpos getinterval",
    "	} def",
    "",
    "",
    "/bitdump % stk: width, height, iscale",
    "	% dump a bit image with lower left corner at current origin,",
    "	% scaling by iscale (iscale=1 means 1/300 inch per pixel)",
    "	{",
    "		% read arguments",
    "		/iscale exch def",
    "		/height exch def",
    "		/width exch def",
    "",
    "		% scale appropriately",
    "		width iscale mul height iscale mul scale",
    "",
    "		% data structures:",
    "",
    "		% allocate space for one line of input",
    "		/bufspace 36 string def",
    "",
    "		% string of FF's",
    "		/FFstring 256 string def",
    "		% for all i FFstring[i]=255",
    "		0 1 255 { FFstring exch 255 put } for",
    "",
    "		% 'escape' flag",
    "		/flag false def",
    "",
    "		% space for a chunk of generated bits",
    "		/result 1000 string def",
    "",
    "		% read and dump the image",
    "		width height 1 [width 0 0 height neg 0 height]",
    "			{ bitgen }",
    "			image",
    "	} def",
    0
};

char *ps_prolog[] = {
    "%%Pages: 1",
    "%%EndProlog",
    "%%Page: 1 1",
    "",
    "/bitdump % stk: width, height, iscale",
    "% dump a bit image with lower left corner at current origin,",
    "% scaling by iscale (iscale=1 means 1/300 inch per pixel)",
    "{",
    "	% read arguments",
    "	/iscale exch def",
    "	/height exch def",
    "	/width exch def",
    "",
    "	% scale appropriately",
    "	width iscale mul height iscale mul scale",
    "",
    "	% allocate space for one scanline of input",
    "	/picstr % picstr holds one scan line",
    "		width 7 add 8 idiv % width of image in bytes = ceiling(width/8)",
    "		string",
    "		def",
    "",
    "	% read and dump the image",
    "	width height 1 [width 0 0 height neg 0 height]",
    "	{ currentfile picstr readhexstring pop }",
    "	image",
    "} def",
    0
};

dump_prolog(flags) {
    char **p = (flags & F_COMPACT) ? ps_prolog_compact : ps_prolog;
    while (*p) printf("%s\n",*p++);
}
#endif XPROLOG

static int
points(n)
{
    /* scale n from pixels (1/300 inch) to points (1/72 inch) */
    n *= 72;
    return n/300;
}

static char *
escape(s)
char *s;
{
    /* make a version of s in which control characters are deleted and
     * special characters are escaped.
     */
    static char buf[200];
    char *p = buf;

    for (;*s;s++) {
	if (*s < ' ' || *s > 0176) continue;
	if (*s==')' || *s=='(' || *s == '\\') {
	    sprintf(p,"\\%03o",*s);
	    p += 4;
	}
	else *p++ = *s;
    }
    *p = 0;
    return buf;
}

/* ---------------------------------------------------------------------*\
| Routine: ps_setup
|
| Description:
|	Setting up header for postscript file.
\*----------------------------------------------------------------------*/
ps_setup(iw, ih, orientation, scale, left, top, 
	   flags, header, trailer)
int iw;
int ih;
enum orientation orientation;
int scale;
int left;
int top;
int flags;
char *header;
char *trailer;
{
    char    hostname[256];
    struct passwd  *pswd;
    long    clock;
    int lm, bm; /* left (bottom) margin (paper in portrait orientation) */

    printf ("%%!%s\n", COMMENTVERSION);
    pswd = getpwuid (getuid ());
    (void) gethostname (hostname, sizeof hostname);
    printf ("%%%%Creator: %s:%s (%s)\n", hostname,
            pswd->pw_name, pswd->pw_gecos);
    printf ("%%%%CreationDate: %s",
                (time (&clock), ctime (&clock)));
    printf ("%%%%EndComments\n");

    dump_prolog(flags);

    if (orientation==PORTRAIT) {
	lm = (left > 0)? left : ((PAPER_WIDTH - scale * iw) / 2);
	bm = (top > 0)? (PAPER_LENGTH - top - scale * ih)
		: ((PAPER_LENGTH - scale * ih) / 2);
	if (header || trailer) {
	    printf("gsave\n");
	    printf("/Times-Roman findfont 15 scalefont setfont\n");
	    /* origin at bottom left corner of image */
	    printf("%d %d translate\n",points(lm),points(bm));
	    if (header) {
		char *label = escape(header);
		printf("%d (%s) stringwidth pop sub 2 div %d moveto\n",
		    points(iw*scale), label, points(ih*scale) + 10);
		printf("(%s) show\n",label);
	    }
	    if (trailer) {
		char *label = escape(trailer);
		printf("%d (%s) stringwidth pop sub 2 div -20 moveto\n",
		    points(iw*scale), label);
		printf("(%s) show\n",label);
	    }
	    printf("grestore\n");
	}
	/* set resolution to device units (300/inch) */
	printf("72 300 div dup scale\n");
	/* move to lower left corner of image */
	printf("%d %d translate\n",lm,bm);
	/* dump the bitmap */
	printf("%d %d %d bitdump\n",iw,ih,scale);
    } else { /* orientation == LANDSCAPE */
	/* calculate margins */
	lm = (top > 0)? (PAPER_WIDTH - top - scale * ih)
		: ((PAPER_WIDTH - scale * ih) / 2);
	bm = (left > 0)? (PAPER_LENGTH - left - scale * iw)
		: ((PAPER_LENGTH - scale * iw) / 2);

	if (header || trailer) {
	    printf("gsave\n");
	    printf("/Times-Roman findfont 15 scalefont setfont\n");
	    /* origin at top left corner of image */
	    printf("%d %d translate\n",points(lm),points(bm + scale * iw));
	    /* rotate to print the titles */
	    printf("-90 rotate\n");
	    if (header) {
		char *label = escape(header);
		printf("%d (%s) stringwidth pop sub 2 div %d moveto\n",
		    points(iw*scale), label, points(ih*scale) + 10);
		printf("(%s) show\n",label);
	    }
	    if (trailer) {
		char *label = escape(trailer);
		printf("%d (%s) stringwidth pop sub 2 div -20 moveto\n",
		    points(iw*scale), label);
		printf("(%s) show\n",label);
	    }
	    printf("grestore\n");
	}
	/* set resolution to device units (300/inch) */
	printf("72 300 div dup scale\n");
	/* move to lower left corner of image */
	printf("%d %d translate\n",lm,bm);
	/* dump the bitmap */
	printf("%d %d %d bitdump\n",ih,iw,scale);
    }
}

/* ---------------------------------------------------------------------*\
| Routine: Select_Window
|
| Description:
|	generate tail of postcript file
\*----------------------------------------------------------------------*/
ps_finish()
{
	char **p = ps_epilog;

	while (*p) printf("%s\n",*p++);
}


/* ---------------------------------------------------------------------*\
| Routine: ps_output_bits
|
| Description:
|	gerating postcript image from window dump data for monochrome
|	display.
\*----------------------------------------------------------------------*/
ps_output_bits(iw, ih, flags, orientation, win, data)
int iw;
int ih;
int flags;
XWDFileHeader *win;
char *data;
enum orientation orientation;
{
    unsigned long swaptest = 1;
    int iwb = win->bytes_per_line;
    register int i;
    int n,bytes;
    unsigned char *buffer;
    register int ocount=0;
    extern char hex1[],hex2[];
    static char hex[] = "0123456789abcdef";

    buffer = (unsigned char *)malloc((unsigned)(iwb + 3));
    if (orientation == LANDSCAPE) {
	/* read in and rotate the entire image */
	/* The Postscript language has a rotate operator, but using it
	 * seem to make printing (at least on the Apple Laserwriter
	 * take about 10 times as long (40 minutes for a 1024x864 full-screen
	 * dump)!  Therefore, we rotate the image here.
	 */
	int ocol = ih;
	int owidth = (ih+31)/32; /* width of rotated image, in bytes */
	int oheight = (iw+31)/32; /* height of rotated image, in scanlines */
	register char *p, *q;
	owidth *= 4;
	oheight *= 32;

	/* Allocate buffer for the entire rotated image (output).
	 * Owidth and Oheight are rounded up to a multiple of 32 bits,
	 * to avoid special cases at the boundaries
	 */
	obuf = (char *)malloc((unsigned)(owidth*oheight));
	if (obuf==0) {
	    fprintf(stderr,"xpr: cannot allocate %d bytes\n",owidth*oheight);
	    exit(1);
	}
	bzero(obuf,owidth*oheight);

	for (i=0;i<ih;i++) {
/*	    fullread(0,(char *)buffer,iwb); */
	    bcopy(data, (char *)buffer, iwb);
	    data = (char *)((int)data + iwb);
	    if (win->bitmap_bit_order == MSBFirst)
		_swapbits(buffer, (long)iwb);
	    if (flags & F_INVERT)
		_invbits(buffer, (long)iwb);
	    if (!(*(char *) &swaptest))
		_swaplong((char *)buffer,(long)iwb);
	    ps_bitrot(buffer,iw,--ocol,owidth);
	}
	if (!(*(char *) &swaptest))
	    _swaplong(obuf,(long)(iw*owidth));
	q = &obuf[iw*owidth];
	bytes = (ih+7)/8;
	for (p=obuf;p<q;p+=owidth)
	    ocount = ps_putbuf((unsigned char *)p,bytes,ocount,flags&F_COMPACT);
    }
    else {
	for (i=0;i<ih;i++) {
/*	    fullread(0,(char *)buffer,iwb); */
	    bcopy(data, (char *)buffer, iwb);
	    data = (char *)((int)data + iwb);
	    if (win->bitmap_bit_order == MSBFirst)
		_swapbits(buffer, (long)iwb);
	    if (flags & F_INVERT)
		_invbits(buffer, (long)iwb);
	    ocount = ps_putbuf(buffer,(iw+7)/8,ocount,flags&F_COMPACT);
	}
    }
    if (flags & F_COMPACT) {
	if (ocount) {
	    /* pad to an integral number of lines */
	    while (ocount++ < LINELEN)
		/* for debugging, pad with a "random" value */
		putchar(hex[ocount&15]);
	    putchar('\n');
	}
    }
}

_invbits (b, n)
	register unsigned char *b;
	register long n;
{
	do {
		*b = ~*b;
		b++;
	    } while (--n > 0);
	
}

/* copied from lib/X/XPutImage.c */

_swapbits (b, n)
	register unsigned char *b;
	register long n;
{
	do {
		*b = _reverse_byte[*b];
		b++;
	    } while (--n > 0);
	
}

_swapshort (bp, n)
     register char *bp;
     register long n;
{
	register char c;
	register char *ep = bp + n;
	do {
		c = *bp;
		*bp = *(bp + 1);
		bp++;
		*bp = c;
		bp++;
	}
	while (bp < ep);
}

_swaplong (bp, n)
     register char *bp;
     register long n;
{
	register char c;
	register char *ep = bp + n;
	register char *sp;
	do {
	  	sp = bp + 3;
		c = *sp;
		*sp = *bp;
		*bp++ = c;
		sp = bp + 1;
		c = *sp;
		*sp = *bp;
		*bp++ = c;
		bp += 2;
	}
	while (bp < ep);
}

/* ---------------------------------------------------------------------*\
| Routine: ps_putbuf;
|
| Description:
| 	Dump some bytes in hex, with bits in each byte reversed
| 	Ocount is number of chacters that have been written to the current
| 	output line.  It's new value is returned as the result of the 
|	function.
| 	Ocount is ignored (and the return value is meaningless) if 
|	compact==0.
\*----------------------------------------------------------------------*/
int
ps_putbuf(s, n, ocount, compact)
register unsigned char *s;	/* buffer to dump */
register int n;			/* number of BITS to dump */
register int ocount;		/* position on output line for next char */
int compact;			/* if non-zero, do compaction (see below) */
{
    register int ffcount = 0;
    extern char hex1[],hex2[];
    static char hex[] = "0123456789abcdef";
#define PUT(c) { putchar(c); if (++ocount>=LINELEN) \
	{ putchar('\n'); ocount=0; }}

    if (compact) {
	/* The following loop puts out the bits of the image in hex,
	 * compressing runs of white space (represented by one bits)
	 * according the the following simple algorithm:  A run of n
	 * 'ff' bytes (i.e., bytes with value 255--all ones), where
	 * 1<=n<=255, is represented by a single 'ff' byte followed by a
	 * byte containing n.
	 * On a typical dump of a full screen pretty much covered by
	 * black-on-white text windows, this compression decreased the
	 * size of the file from 223 Kbytes to 63 Kbytes.
	 * Of course, another factor of two could be saved by sending
	 * the bytes 'as is' rather than in hex, using some sort of
	 * escape convention to avoid problems with control characters.
	 * Another popular encoding is to pack three bytes into 4 'sixels'
	 * as in the LN03, etc, but I'm too lazy to write the necessary
	 * PostScript code to unpack fancier representations.
	 */
	while (n--) {
	    if (*s == 0xff) {
		if (++ffcount == 255) {
		    PUT('f'); PUT('f');
		    PUT('f'); PUT('f');
		    ffcount = 0;
		}
	    }
	    else {
		if (ffcount) {
		    PUT('f'); PUT('f');
		    PUT(hex[ffcount >> 4]);
		    PUT(hex[ffcount & 0xf]);
		    ffcount = 0;
		}
		PUT(hex1[*s]);
		PUT(hex2[*s]);
	    }
	    s++;
	}
	if (ffcount) {
	    PUT('f'); PUT('f');
		PUT(hex[ffcount >> 4]);
	    PUT(hex[ffcount & 0xf]);
	    ffcount = 0;
	}
    }
    else { /* no compaction: just dump the image in hex (bits reversed) */
	while (n--) {
	    putchar(hex1[*s]);
	    putchar(hex2[*s++]);
	}
	putchar('\n');
    }
    return ocount;
}

/* ---------------------------------------------------------------------*\
| Routine: ps_bitrot
|
| Description:
|	
\*----------------------------------------------------------------------*/
ps_bitrot(s,n,col,owidth)
unsigned char *s;
register int n;
int col;
register int owidth;
/* s points to a chunk of memory and n is its width in bits.
 * The algorithm is, roughly,
 *    for (i=0;i<n;i++) {
 *        OR the ith bit of s into the ith row of the
 *        (col)th column of obuf
 *    }
 * Assume VAX bit and byte ordering for s:
 *	The ith bit of s is s[j]&(1<<k) where i=8*j+k.
 *	It can also be retrieved as t[j]&(1<<k), where t=(int*)s and i=32*j+k.
 * Also assume VAX bit and byte ordering for each row of obuf.
 * Ps_putbuf() takes care of converting to Motorola 68000 byte and bit
 * ordering.  The following code is very carefully tuned to yield a very
 * tight loop on the VAX, since it easily dominates the entire running
 * time of this program.  In particular, iwordp is declared last, since
 * there aren't enough registers, and iwordp is referenced only once
 * every 32 times through the loop.
 */
{
    register int mask = 1<<(col%32);
    register int iword; /* current input word (*iwordp) */
    register int b = 0; /* number of bits in iword left to examine */
    register char *opos = obuf + (col/32)*4;
	/* pointer to word of obuf to receive next output bit */
    register int *iwordp = (int *) s; /* pointer to next word of s */

    while (--n>=0) {
	if (--b < 0) {
	    iword = *iwordp++;
	    b = 31;
	}
	if (iword & 1) {
	    *(int *)opos |= mask;
	}
	opos += owidth;
	iword >>= 1;
    }
}
	
/* ---------------------------------------------------------------------*\
| Routine: wd_to_ps
|
| Description:
|	Fetch window, image and xcolor info and generate postcript output.
\*----------------------------------------------------------------------*/
wd_to_ps (window, dscale)
Window window;
int dscale;
{
  XWindowAttributes win_info;
  XImage *image;
  XColor *color;
  int ncolors;
  register int w_scale;
  register int h_scale;
  register int w_max;
  register int h_max;
  int flags = False;
  int scale;
  int width = -1;
  int height = -1;
  int top = -1;
  int left = -1;
  char *header1 = NULL;
  char *trailer = NULL;
  enum orientation orientation;
  XWDFileHeader header;

  XGetWindowAttributes(dpy, window, &win_info);
  image = XGetImage ( dpy, window,
                      -win_info.border_width, -win_info.border_width,
                      win_info.width + (win_info.border_width << 1),
                      win_info.height + (win_info.border_width << 1),
                      ~0, ZPixmap);

  ncolors = Get_XColors(&win_info, &color);

  if (dscale == 0) {
  if ((image->width < image->height || (flags & F_PORTRAIT)) && 
    !(flags & F_LANDSCAPE)) {
    orientation = PORTRAIT;
    w_max = (width > 0)? width : W_MAX;
    h_max = (height > 0)? height : H_MAX;
    w_scale = w_max / image->width;
    h_scale = h_max / image->height;
    scale = min(w_scale, h_scale);
  } else {
    orientation = LANDSCAPE;
    w_max = (width > 0)? width : H_MAX;
    h_max = (height > 0)? height : W_MAX;
    w_scale = w_max / image->width;
    h_scale = h_max / image->height;
    scale = min(w_scale, h_scale);
  }
  }
  else {
	scale = dscale;
	if (image->width < image->height)
	    orientation = PORTRAIT;
	else
	    orientation = LANDSCAPE;
  }

  header.pixmap_depth = (xwdval) image->depth;
  header.pixmap_width = (xwdval) image->width;
  header.pixmap_height = (xwdval) image->height;
  header.bitmap_bit_order = (xwdval) image->bitmap_bit_order;
  header.bits_per_pixel = (xwdval) image->bits_per_pixel;
  header.bytes_per_line = (xwdval) image->bytes_per_line;
  header.visual_class = (xwdval) win_info.visual->class;
  header.red_mask = (xwdval) win_info.visual->red_mask;
  header.green_mask = (xwdval) win_info.visual->green_mask;
  header.blue_mask = (xwdval) win_info.visual->blue_mask;
  header.bits_per_rgb = (xwdval) win_info.visual->bits_per_rgb;
  header.colormap_entries = (xwdval) win_info.visual->map_entries;
  header.ncolors = ncolors;

  ps_setup(image->width, image->height, orientation, scale, left, top,
        flags, header1, trailer);
  ps_output_bits(image->width, image->height, flags, orientation, 
		&header, image->data);
  ps_finish();

  if (ncolors > 0) free(color);
  XDestroyImage(image);
}

main (argc, argv)
int argc;
char *argv[];
{
  Window win;
  int scale = 0;

  dpy = XOpenDisplay(NULL);
  win = Select_Window(dpy);
  if (argc <= 1) scale = 1;
  else scale = atoi(argv[1]);
  
  wd_to_ps(win, scale);
  XCloseDisplay(dpy);
}
