/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/troff.c,v 1.2 89/09/08 17:50:21 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/troff.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	troff.c,v $
 * Revision 1.2  89/09/08  17:50:21  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:34:53  mss
 * Initial revision
 * 
 * Revision 1.3  89/05/17  15:13:46  jaap
 * plugging memory leaks,
 * cosmetic changes
 * 
 * Revision 1.2  89/01/12  17:56:52  jaap
 * better handling of escape sequences in character content portion
 * As a side effect we now also know about STAB although it is not done properly to the
 * standard
 * 
 * Revision 1.1  89/01/05  18:44:13  jaap
 * Initial revision
 * 
 */

#ifndef lint
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/troff.c,v $";
#endif
/* static char sccsid[] = "%Z%	%M%	%I%	%E%"; */
#include <stdio.h>
#include <ext.h>

/*
 * Routines to generate troff in a ``neat'' way.
 */

#define	PREFER 60    /* ``prefererred'' output line length */

static int cc = 0;	    /* character count of output line */
static char lastchar;	    /* last char we've seen */
static char lastadjust = 'l';	/* last adjustmode */
static int curps = 10;		/* current pointsize */

/*
 * trinit - initialize troff to something.
 *
 * define an arbitrary top and bottom marging by defining macros.
 *
 */

/* 
 *top marging .25 inch, no space mode on
 */
static char *header = ".de NP\n'sp .25i\n.ns\n..\n.wh 0 NP\n";
/*
 * bottom marging .25 inch. generate new page
 */
static char *footer = ".de BT\n'bp\n..\n.wh -.25i BT\n";

/*
 * idea stolen from psfig, a box macro
 * we place the box, dependent on the current adlust mode
 * should actually be done by initial offset, or at_BLOCK_ALIGN
 * .j register contains adjust mode:
 *	    .j = 1  adjust both
 *	    .j == 0 ragged right (left flushed)
 *	    .j == 3 centered adjust 
 *	    .j == 5 ragged left (right adjust)
 */
static char *boxmac[] = {
".de BX",
".nr In \\\\n(.i",
".nr Fi \\\\n(.u",
".nr LL \\\\n(.l",
".nf",
".if \\\\n(.j==0 .nr x \\\\n(.i",    /* left flush */
".if \\\\n(.j==1 .nr x \\\\n(.i",    /* left flush */
".if \\\\n(.j==3 .nr x (\\\\n(.l-\\\\n(.iu-\\\\$1)/2+\\\\n(.i",   /* center box */
".if \\\\n(.j==5 .nr x (\\\\n(.l-\\\\$1",    /* right flush */
".in \\\\nxu", /* indent to calculated place */
"\\s10\\L'\\\\$2\\(br'\\l'\\\\$1\\(ul'\\L'-\\\\$2\\(br'\\l'-\\\\$1\\(ul'\\x'\\\\$2'\\s0",
".in \\\\n(Inu",    /* now clean up */
".if \\\\n(Fi .fi",
".rr In",
".rr Fi",
".rr LL",
".rr x",
"..",
0 };

/*
 * start a raster output, a completely original idea this time.
 */

static char *startraster[] = {
".de SR",
".nr Fo \\\\n(.f",
".nr In \\\\n(.i",
".nr Fi \\\\n(.u",
".nr LL \\\\n(.l",
".nr Ps \\\\n(.s",
".nr Vs \\\\n(.v",
".nf",
".if \\\\n(.j==0 .nr x \\\\n(.i",    /* left flush */
".if \\\\n(.j==1 .nr x \\\\n(.i",    /* left flush */
".if \\\\n(.j==3 .nr x (\\\\n(.l-\\\\n(.iu-\\\\$1)/2+\\\\n(.i",   /* center box */
".if \\\\n(.j==5 .nr x (\\\\n(.l-\\\\$1",    /* right flush */
".in \\\\nxu", /* indent to calculated place */
".ps 8",
".tr #.",
".ft R",
".cs R 1 36",	/* should actually get this from the pel-spacing values etc. */
".nr x \\\\w'#'u",
".vs \\\\nxu",	/* should also be pel-spacing etc. */
"..",
0};

/*
 * end a raster
 */
static char *endraster[] = {
".de ER",
".cs R",
".in \\\\n(Inu",    /* now clean up */
".if \\\\n(Fi .fi",
".vs \\\\n(Vsu",
".ps \\\\n(Ps",
".ft \\\\n(Fo",
".tr ..",
".rr Fo",
".rr Ps",
".rr Vs",
".rr In",
".rr Fi",
".rr LL",
".rr x",
"..",
0};

static char *autobox[] = {
"to be filled in",
0};

trinit()
{

	fprintf(stdout, "%s%s", header, footer);
	fprintf(stdout, ".ad l\n");
	fprintf(stdout, ".po .25i\n");
	fprintf(stdout, ".nh\n");
	trmac(boxmac);
	trmac(startraster);
	trmac(endraster);
	fprintf(stdout, ".br\n");
/*	fprintf(stdout, "\\&\\\"get troff started\n"); */
	cc = 0;
	lastchar = '\0';
}

/*
 * dump a troff macro
 */

trmac(p)
char **p;
{
	while( *p ) {
		fprintf(stdout, "%s\n", *p++);
	}
}
/*
 * trout should format nicely into troff input lines of 60 chars, hide lines starting
 * with a "." or a "'", etc.
 *
 */
trout(c)
char c;
{
	switch(c) {
		case ' ':
			if ( cc	== 0 ){	/* will cause a break */
				lastchar = ' ';
				return;
			}else if ( cc	> PREFER ) {	/* break line*/
				putc('\n' , stdout);
				cc = 0;
				lastchar = '\0';
				return;
			}else {	/*
				 * check whether we saw a punctuation char before
				 * troff treats these differently and we want to be
				 * consistent
				 */
				switch(lastchar) {
					case '.':
					case '!':
					case '?':
						/* fudge */
						if ((cc == 0) ||
						   (lastchar == ' ' && c != ' ')) {
							atkfudge();
						}
						putc('\n', stdout);
						cc = 0;
						lastchar = '\0';
						return;
				}
			}
			break;	/* will output the space */
		case '.':
		case '\'':
			if (cc == 0 ) { /* hide with zero-width char */
				fprintf(stdout, "\\&");
			}
			break;
		case '\\':
			/* fudge */
			if ((cc == 0) || (lastchar == ' ' && c != ' ')) {
				atkfudge();
			}
			fprintf(stdout, "\\e");
			lastchar = c;
			return;
		case '\n':	/* um kludge */
			trnewline();	/* diamond hardline break kludge */
			return;
		}
	/*
	 * since linespace doesn't work simulate atk behaviour
	 */
	if ((cc == 0) || (lastchar == ' ' && c != ' ')) {
		atkfudge();
	}
	lastchar = c;
	fprintf(stdout, "%c", c);
	cc++;
}

/*
 * the atk doesn't know about line spacing,so we can not do a .vs here.
 * To overcome this problem, we wrap a \x'' command around each word.
 *
 * This trick will also help for interleaf and other systems which don't deal with
 * a hard linespacing.
 */
atkfudge()
{
	int x = curps - 10;

	if( x > 0) {	/* extra spacing before line if pointsize != 10 */
		sprintf(msgsbuf, "\\x'-%dp'", x);
		fprintf(stdout, msgsbuf);
		cc += strlen(msgsbuf);
	}
}

/*
 * push a comment to troff
 *
 * Note that we actually should check whether or not we are inside white space
 */
trcomment(str)
char *str;
{
	if( cc > 0) {
		fprintf(stdout, "\\\" %s\n", str);
		cc = 0;
	} else
		fprintf(stdout, ".\\\" %s\n", str);
}

/*
 * trscale, scale BMU to inch, applying SMU's
 */
float
trscale(i)
int i;
{
	return (float) i * smu.num / (smu.denom * 1200) ;
}

/*
 * do a troff paragraph
 */
trpar(offset, indent, align, firstline, vs)
at_OFFSET_type *offset;
cc_ALIGNMENT_type align;
cc_1ST_LINE_OFFSET_type firstline;
cc_LINE_SPACING_type vs;
cc_INDENTATION_type indent;
{
	float findent, tmpindent, linelength;

	/*
	trcomment("trpar");
	 */
	/*
	 * offsets
	 */
	/*
	 * first set indentation
	 */
	findent = trscale(offset->right_hand_offset+indent);
	trflush();
	fprintf(stdout, ".in %.4fi\n", findent);
	/*
	 * right margin is defined by .ll ( assume
	 * paper width = 8.5 inch page offset  2* .25 inch)
	 */
	linelength = 9.0 - trscale(offset->left_hand_offset + indent) - 1.0;
	fprintf(stdout, ".ll %.4fi\n", linelength);
	/*
	 * firstline offset, only issue .ti when there is an offset.
	 */
	if( firstline != 0 ) {
		tmpindent = trscale(firstline);
		fprintf(stdout, ".ti %.4fi\n", tmpindent);
	}
	/*
	 * baseline spacing
	 */
	fprintf(stdout, ".vs %.4fi\n", trscale(vs));
	cc = 0;
	tralign(align);

}

/*
 * do alignment
 */

tralign(align)
cc_ALIGNMENT_type align;
{
	char adjust;
		/*
		 * adjustment
		 */
	switch(align) {
		case cc_ALIGNMENT_start_aligned:
			adjust = 'l';
			break;
		case cc_ALIGNMENT_end_aligned:
			adjust = 'r';
			break;
		case cc_ALIGNMENT_centred:
			adjust = 'c';
			break;
		case cc_ALIGNMENT_justified:
			adjust = 'b';
			break;
		default:
			sprintf(msgsbuf, "Unknown alignment");
			fprintf(stderr, "%s\n", msgsbuf);
			trcomment(msgsbuf);
	}
	if ( adjust != lastadjust) {
		trflush();
		fprintf(stdout, ".br	\\\"align\n");	/* always break to take effect */
		fprintf(stdout, ".ad %c\n", adjust);
		lastadjust = adjust;
		cc = 0;
	}
}

/*
 * process paragraph separation, we get separation and WIDOW_SIZE * LINE_SPACING
 */

trsep(sep, ne)
at_SEPARATION_type *sep;
int ne;
{
	static trail = 0;
	int total = 0;

		/*
		 * if the last thing was a hard newline, make sure we do the line spacing
		 */
	if (lastchar == '\n') {
		fprintf(stdout, ".sp	\\\" separation\n");
		lastchar = '\0';
	}
	total = trail > sep->leading_edge ? trail : sep->leading_edge;
	trflush();
	fprintf(stdout, ".ne %.4fi\n", trscale( total + ne));  /* does it fit? */
	if (total == 0) {
#if 0
		fprintf(stdout, ".br\n");   /*break previous line*/
#else
		/* yet another umich kludge */
		fprintf(stdout, ".sp	\\\" um kludge paragraph separation\n");
#endif
	} else
		fprintf(stdout, ".sp %.4fi\n", trscale(total));
	trail =	sep->trailing_edge; /* remember for next time */
	cc = 0;

}

/*
 * flush current output line
 */

trflush()
{
	if(cc > 0) {
		putc('\n', stdout);
		cc = 0;
		lastchar = '\0';
	}
}

/*
 * issue a hard newline
 */

trnewline()
{
	if (cc != 0) {
		trbreak("hard newline");
	} else
		fprintf(stdout, ".sp	\\\" hard newline\n");
	lastchar = '\n';
}

/*
 * issue a break (actually, part of hard newline sequence), with a possible
 * comment attached
 */
trbreak(c)
char *c;
{
	if (cc != 0)
		fputc('\n', stdout);
	fprintf(stdout, ".br	\\\" %s\n", *c ? c : "trbreak");
	cc = 0;
	lastchar = '\0';
}

/*
 * draw a box (eventually)
 */
trautobox(width, height)
long width, height;
{
	float x = trscale((int)width);
	float y = trscale((int)height);

	/* we don't try to be smart on the moment, like automatic scaling */
	trbreak("trbox");
	sprintf(msgsbuf, "box x: %.4fi y: %.4fi", x, y);
	trcomment(msgsbuf);
	fprintf(stdout, ".ne %.4fi\n", y);
	fprintf(stdout, ".BX %.4fi %.4fi\n", x, y);
/*	fprintf(stdout, ".sp %.fi\n", y); */
}

/*
 * print troff font info
 */
trfont(f)
struct FontStruct *f;
{
	int i;

	/* quick sanity check */
	if (f->PointSize == UNSPECIFIED_PS||f->TroffName == '\0') {
		sprintf(msgsbuf, "Illegal fontstruct, ft %s .ps %d mount %s .fp %d",
				f->TroffName, f->PointSize,
				f->DoMount == NO ? "no" : "yes",
				f->MountPosition);
		trcomment(msgsbuf);
		return;
	} else {	/* put out troff */
		if (cc == 0 || lastchar == ' ') {	/* easy */
			if(lastchar == ' ') {
				fprintf(stdout,"\n");
				cc = 0; lastchar = '\0';
			}
			if( f->DoMount == YES ) {
				fprintf(stdout, ".fp %d %s\n", f->MountPosition,
								f->TroffName);
			}
			fprintf(stdout, ".ft %s\n", f->TroffName);
			if ( f->PointSize != curps) {
				fprintf(stdout, ".ps %d\n", f->PointSize);
				curps = f->PointSize;
			}
		} else {	/* hard case */
			if( f->DoMount == YES ) {
				fprintf(stdout, "\n'fl\n.fp %d %s\n\\c", f->MountPosition,
								f->TroffName);
			}
			i = strlen(f->TroffName);
			if( i > 1) {
				fprintf(stdout,"\\f(%s", f->TroffName);
				cc += 5;
			} else {
				fprintf(stdout, "\\f%s", f->TroffName);
				cc += 3;
			}
			if ( f->PointSize != curps) {
				fprintf(stdout, "\\s%d", f->PointSize);
				curps = f->PointSize;
			}
		}
	}
}

trmount(f)
struct FontStruct *f;
{

	trflush();
	fprintf(stdout, ".fp %d %s\n", f->MountPosition, f->TroffName);
}

/*
 * flag start of raster
 */
trstartraster(width, height)
long width, height;
{
	float x = trscale((int)width);
	float y = trscale((int)height);

	/* we don't try to be smart on the moment, like automatic scaling */
	trbreak("raster");
	sprintf(msgsbuf, "raster x: %.4fi y: %.4fi", x, y);
	trcomment(msgsbuf);
	fprintf(stdout, ".ne %.4fi\n", y);
	fprintf(stdout, ".SR %.4fi %.4fi\n", x, y);

}

/*
 * dump raster to troff
 */
trraster( r, xbits, ybits)
char *r;
int xbits, ybits;
{
	int rowbytes = (xbits +7) / 8 ;
	register int x, y;
	register int white;
	extern char *t1[];
	char *p = r;

	for(y = 0; y < ybits; y++) {
		white = 0;
		for(x = 0; x < rowbytes; x++) {
			switch (*p) {
			case 0:	/* all white */
				white++;
				break;
			default:
				if (white) {
					while(white--) {
						fprintf(stdout, "        ");
					}
					white = 0;
				}
				fprintf(stdout, "%s", t1[*p]);
			}
			p++;
		}
		fprintf(stdout, "\n");
	}
	fprintf(stdout, ".ER\n");
}

/*
 * issue a tab for troff, ignoring the tab stop info on the moment
 */
trtab(i)
int i;
{
	fprintf(stdout, "\t");
}
