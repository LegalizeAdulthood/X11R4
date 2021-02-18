/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	Troff_parse.c - Parse the device independent troff file.  All
 *		the real work of drawing the bits is done by other
 *		files.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
 *		Combine characters into words
 *			David Cahlander (Cray Research, Inc.)
 *
 *	Legalese -  This command was developed as an independent project
 *		to satisfy a need of the author.  This program may contain
 *		bugs and the user is cautioned to independently verify that
 *		the program is suitable for the user's intended purpose.
 *		The program is made available on an ``as is'' basis with
 *		all faults and without any implied or expressed warranties
 *		or support from either the author, Malcolm Slaney, or
 *		Schlumberger Palo Alto Research Laboratory.
 *
 *		I am putting this program in the public domain.  You are
 *		free to use it as you wish.  In return I ask two things.
 *		First, that you do not remove the names of the authors
 *		from this work.  Secondly, if you make changes or
 *		improvements to this program that you pass these back to
 *		the author so that everybody can benefit from the
 *		improvements.
 *
 *					Malcolm Slaney  (December 1986)
 *					Schlumberger Palo Alto Research
 *					3340 Hillview Avenue
 *					Palo Alto, CA 94304
 *					(415) 496-4669
 *					spar!malcolm@decwrl.dec.com
 *					malcolm@ecn.purdue.edu
 *					malcolm@spar.slb.com (Someday)
 */

#include	"suntroff.h"

static int StopSeen = 0;

extern char *GetWord(), *GetLine();

ParseInput()
{
	int	n, k, c;
	char	Buffer[BUFSIZ];

	StopSeen = 0;

	for (;;) {
		if ((c = GetChar()) != EOF) {
			switch (c) {
			case '\n':	
				LineNumber++;
				break;
			case ' ':	/* when input is text */
			case 0:		/* occasional noise creeps in */
				break;
			case '{':	/* push down current environment */
				push_env();
				break;
			case '}':
				pop_env();
				break;
					/* two motion digits plus a
					   character */
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				HorizontalMove((c-'0')*10 + GetChar()-'0');
				PutCharacter(GetChar());
				break;
			case 'c':	/* single ascii character */
#ifndef SUNTOOLS
				PutCharacterString();
#else
				PutCharacter(GetChar());
#endif
				break;
			case 'C':
				GetWord(Buffer, BUFSIZ);
				PutSpecialCharacter(Buffer);
				break;
#ifdef	BEZERKELYISM
			case 't':	/* straight text */
			{
				char	*s, c;

				if (GetLine(Buffer,BUFSIZ) == NULL){
					fatal("unexpected end of input");
				}

				s = Buffer;

				while ((c = *s++) != '\n') {
					if (c == '\\') {
						switch (c = *s++) {
						case '\\':
						case 'e':
						    PutCharacter('\\');
						    break;
						case '(':
						{
						    char str[3];

						    str[0] = *s++;
						    str[1] = *s++;
						    str[2] = '\0';
						    PutSpecialCharacter(str);
						    break;
						}
						}
					} else {
					    PutCharacter(c);
					}
					/*hmot((int)hscale);*/
				}
				break;
			}
#endif	BEZERKELYISM				
			case 'D':	/* draw function */
				GetLine(Buffer, BUFSIZ);
				ParseDrawFunction(Buffer);
				break;
			case 's':
					/* ignore fractional sizes */
				n = GetNumber();
				SetFontSize(n);
				break;
			case 'f':
				n = GetNumber();
				SetFont(n);
				break;
#ifdef	BEZERKELYISM
			case 'i':
				n = GetNumber();
				setstip(n);	/* ignore for now */
				break;
#endif	BEZERKELYISM
			case 'H':	/* absolute horizontal motion */
				k = GetNumber();
				HorizontalGoto(k);
				break;
			case 'h':	/* relative horizontal motion */
				k = GetNumber();
				HorizontalMove(k);
				break;
			case 'w':	/* word space */
#ifndef BEZERKELYISM
				c = GetChar();
				if (c < '0' || c > '9') {
					UnGetChar(c);
					break;
				}
				HorizontalMove((c-'0')*10 + GetChar()-'0');
#ifndef SUNTOOLS
				PutCharacterString();
#endif
#endif
				break;
			case 'V':
				n = GetNumber();
				VerticalGoto(n);
				break;
			case 'v':
				n = GetNumber();
				VerticalMove(n);
				break;
			case 'P':	/* new spread */
				break;
			case 'p':	/* new page */
				(void) GetNumber();
				return(RememberPagePosition());
			case 'n':	/* end of line */
				GetNumber();
				GetNumber();
				HorizontalGoto(0);
				break;
			case '#':	/* comment */
				GetLine((char *) NULL, 0);
				break;
			case 'x':	/* device control */
				ParseDeviceControl();
				break;
			default:
				warning("Unknown input character %c(%d)\n",
				      c, c);
			}
		} else {
			extern int LastPage, CurrentPage;
			if (!LastPage && !StopSeen){
				warning("File is incomplete!\nEnd of file reached before finding\nthe end of the document.\nHave read %d pages.",CurrentPage);
			}
			return(CurrentPage);
		}
	}
}

struct	state {
	int	size;
	int	font;
	int	style;
	int 	thick;
	int	h_pos;
	int	v_pos;
};

struct	state 	StateStack[MAXSTATE];
struct	state	*statep = StateStack;
	  
push_env()					/* begin a new block */
{
	statep->size = size;
	statep->font = font;
	statep->style = linmod;
	statep->thick = linethickness;
	statep->h_pos = HorizontalPosition;
	statep->v_pos = VerticalPosition;
	if (statep+1 >= StateStack+MAXSTATE)
		warning( "{ nested too deep");
	else
		statep++;
}

pop_env()					/* pop to previous state */
{
	if (--statep < StateStack)
		warning("extra }");
	else {
		size = statep->size;
		font = statep->font;
	    	HorizontalPosition = statep->h_pos;
		VerticalPosition = statep->v_pos;
		linmod = statep->style;
		linethickness = statep->thick;
		SetFont(font);
		SetFontSize(size);
	}
}

ParseDrawFunction(buf)
char	*buf;
{
	int	n, m, n1, m1;
	int bordered; /* until we do polygons right */

	switch (buf[0]) {
	case 'l':				/* draw a line */
		sscanf(buf+1, "%d %d", &n, &m);
		DrawLine(n, m);
		break;
	case 'c':				/* circle */
		sscanf(buf+1, "%d", &n);
		DrawCircle(n);
		break;
	case 'e':				/* ellipse */
		sscanf(buf+1, "%d %d", &m, &n);
		DrawEllipse(m, n);
		break;
	case 'a':				/* arc */
		sscanf(buf+1, "%d %d %d %d", &n, &m, &n1, &m1);
		DrawArc(n, m, n1, m1);
		break;
	case '~':				/* wiggly line */
		DrawSpline(buf+1,1);
		break;
#ifdef	BEZERKELYISM
	case 'q':				/* versatec polygon - ignore */
		while (buf[strlen(buf) - 1] != '\n'){
			if (GetLine(buf, sizeof(buf)) == NULL){
				fatal("unexpected end of input");
			}
		}
		break;
	case 'P':				/* unbordered */
		bordered = 0;
	case 'p':				/* polygon */
		sscanf(buf+1, "%d", &n);
		n = 1;
		while(buf[n++] == ' ');
		while(isdigit(buf[n])) n++;
		DrawSpline(buf+n, -1);
		bordered = 1;
		break;
	case 'g':				/* gremlin spline */
		DrawSpline(buf+1, 0);
		break;
	case 't':				/* line thickness */
		sscanf(buf+1, "%d", &n);
		drawthick(n); 
		break;
	case 's':	/* line style */
		sscanf(buf+1, "%d", &n);
		drawstyle(n);
		break;
#endif	BEZERKELEYISM
	default:
		warning("unknown drawing function %s", buf);
		break;
	}
} 

ParseDeviceControl()				/* Parse the x commands */
{
        char str[20], str1[50];
	int c, n;
	extern int LastPage, CurrentPage;

	GetWord(str, 20);
	switch (str[0]) {			/* crude for now */
	case 'T':				/* output device */
		GetWord(DeviceName, 10);
		break;
	case 'i':				/* initialize */
		UserTypesetter = LoadDevice(DeviceName);
		CurrentPage = 1;
		SetPrinter(DeviceName);
		InitTypesetter();
		break;
	case 't':				/* trailer */
		break;
	case 'p':				/* pause -- can restart */
		break;
	case 's':				/* stop */
		StopSeen = 1;
		LastPage = CurrentPage;
		return;
	case 'r':				/* resolution when prepared */
		DeviceResolution = GetNumber();
		break;
	case 'f':				/* font used */
		n = GetNumber();
		(void) GetWord(str, 20);
		(void) GetLine(str1, 50);
		SetFontPosition(n, str, str1);
		break;
	case 'H':				/* char height */
		break;
	case 'S':				/* slant */
		break;
	}
	while ((c = GetChar()) != '\n')		/* skip rest of input line */
		if (c == EOF)
			return;
	return;
}
