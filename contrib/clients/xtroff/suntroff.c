/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	Suntroff - Code to interface the rather generic user interface
 *		provided by windows.c and the low level troff parsing 
 *		and drawing routines.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
 *		Write characters as words
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
#ifdef SUNTOOLS
#include	<suntool/sunview.h>
#endif SUNTOOLS
#define	MAXPAGES	500		        /* Pages to remember */

static FILE *CurrentFilePointer;		/* Current input file. */
static FILE *RealBufferPointer;	    		/* Buffer file pointer */
static FILE *RealFilePointer;			/* Real File Pointer */

static long PagePlace[MAXPAGES];		/* Remembered ftell
						   positions */
static int MaxPage = 0;			/* Number of remembered positions */


					/* TYPESETTER ENVIRONMENT VARIABLES */
int	size = 1;			/* Current Font Size (internal
					   pstable index) */
float	ditsiz;				/* Current Font Scale (special troff
					   characters) */
int	font = 1;			/* Current Font (internal font
					   number */
int	linmod;				/* Line Style....unused. */

int	linethickness;			/* unused */

int	HorizontalPosition;		/* Horizontal Position (Device
					   Coordinates) */
int	VerticalPosition;		/* Vertical Position on the page
					   (Device Coordinates) */

char	DeviceName[11];			/* Output produced for this device */

int	DeviceResolution;		/* Output produced at this resolution*/

struct FontBitStruct *CurrentFont = 0;	/* Pointer to the current font
					   information. */
int	DisplayOutput = 1;		/* Don't display output (just parse)
					   when this flag is zero. */
int	LineNumber = 0;			/* Input file line number */

extern	int SUNRES;			/* Resolution of display */

#ifdef SUNTOOLS
char	*DefaultTitle = "SUNTROFF (3.0) ";
#else
char	*DefaultTitle = "XTROFF (4.8) ";
#endif

/*
 * We use this macro to pack two characters into an int, so we can test for
 * troff special characters efficiently. As long as we use PACK_TWO_CHARS for
 * both constants that we want to compare against, eg. PACK_TWO_CHARS('c',
 * 'i') for 'ci', as well as for variables, it should be portable. Idea from
 * Dave Cahlander <dac@cray.com>, portable implementation from Henry Spencer
 * <henry@utzoo.uucp>, Jeffrey Lee <jonah@db.toronto.edu>
 */
#define	PACK_TWO_CHARS(c1, c2)	(((c1)<<8)|(c2))

int	CurrentPage = 0;		/* Current Page in Input File */

int	LastPage = 0;			/* Last Page of input file */

ShowPage(PageNumber){
	int	i;

	if (!CurrentFilePointer){
		warning("No file open for input.");
		return(0);
	}

	if (PageNumber < 1)
		PageNumber = 1;

	if (LastPage && PageNumber > LastPage){
		warning("There are only %d pages in\nthis document.",
		      LastPage);
		return(CurrentPage);
	}

	if (PageNumber < MAXPAGES){
		if (PagePlace[PageNumber]){
			FileSeek(PagePlace[PageNumber]);
			CurrentPage = PageNumber;
		} else {
			for (i=PageNumber;i>0 && !PagePlace[i];i--)
				;
			FileSeek(PagePlace[i]);
			
			SetTitleBar("Skipping",i);
			DisplayOutput = 0;
			while (!feof(CurrentFilePointer) &&
			       ((CurrentPage = ParseInput()) != PageNumber))
				;
		}
		DisplayOutput = 1;
		ClearPage();
		SetTitleBar("Rasterizing",PageNumber);
		CurrentPage = ParseInput();
		RefreshPage();
	}

	if (LastPage && PageNumber > LastPage){
		warning("There are only %d pages in\nthis document.",
		      LastPage);
		SetTitleBar("Displaying",CurrentPage);
		return(CurrentPage);
	}

	SetTitleBar("Displaying",PageNumber);
	return(PageNumber);
}


ClearPagePositions(){
	int	i;

	for (i=0;i<MAXPAGES;i++){
		PagePlace[i] = 0;
	}
	MaxPage = 0;
}

#ifdef SUNTOOLS
static short IconImage[] = {
#include	"ditroff.icon"
}; 
DEFINE_ICON_FROM_IMAGE(DitroffIcon,IconImage);

InitializeApplication(Frame,Canvas)
Window Frame, Canvas;
{
	window_set(Frame,
		   FRAME_ICON,&DitroffIcon,
		   0);
	SetTitleBar("Initializing",0);
}
#endif SUNTOOLS
	

InitializeFile(RealFile, TempFile)
FILE	*RealFile, *TempFile;
{
	CurrentFilePointer = RealFilePointer = RealFile;
	RealBufferPointer = TempFile;
	FileSeek(0L);
	ClearPagePositions();
	CurrentPage = LastPage = 0;
}

HorizontalMove(delta)
int	delta;
{
	HorizontalPosition += delta;
}

HorizontalGoto(NewPosition)
int	NewPosition;
{
	HorizontalPosition = NewPosition;
}

VerticalMove(delta)
int	delta;
{
	VerticalPosition += delta;
}

VerticalGoto(NewPosition)
int	NewPosition;
{
	VerticalPosition = NewPosition;
}

#ifndef SUNTOOLS
/*
 *  An attempt is made to gather characters up into words.  This
 *  produces a much better display since the individual characters
 *  in a word are separated by the space allocated for the character
 *  at this font size.  On the average, the position desired and
 *  the actual screen position will match, since the screen characters
 *  have the spacing described by the troff description file DESC.out.
 *  However, if the font has incorrect space or an incorrect discription
 *  file is used, this scheme will not work.  An error term is calculated
 *  that indicates the difference between where the character actually
 *  will be placed and the position required by cononical troff position
 *  file.  When this difference exceeds 3 pixels (arbitrary) the
 *  assembled word is terminated and the next character is placed at
 *  the position designated by troff.
 *  Note that the troff position can be a fractional pixel while 
 *  the screen position must always be an integer.
 */
PutCharacterString()
{
	char	string[512];
	char	strch[512];
	char	strpos[512];
	int	i, n = 0;
	int	c, ch, x, w;
	float	xdelta, xerror;
	char	**CodeTable, **AsciiTable;
	
	CodeTable = OutputTypesetter->CodeTable;
	AsciiTable = OutputTypesetter->AsciiTable;

	ch = GetChar();
	if (ch < 32 || ch > 128 ||
	 (AsciiTable[font] != NULL && AsciiTable[font][ch-32] == 0)) {
		PutCharacter(ch);
		return;
	}
	x = HorizontalPosition;
	xerror = 0;
	LoadFontBits();

/*  character translation */
	
	strpos[n] = 0;
	strch[n] = ch;
	ch = CodeTable[font][AsciiTable[font][ch-32]&0xff];
	string[n++] = ch;

	while((c = GetChar()) != EOF) {
		switch(c) {

		case '\n':
			LineNumber++;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			HorizontalMove(w = (c-'0')*10 + GetChar()-'0');
			strpos[n] = w;
			xdelta = CurrentFont->Bits->per_char[ch -
			    CurrentFont->Bits->min_char_or_byte2].width -
			    (w * SUNRES) / (float)UserTypesetter->Device.res;
			xerror += xdelta;
#ifdef	FONTDEBUG
			printf("ch=%c d=%g e=%g\n", ch, xdelta, xerror);
#endif	/* FONTDEBUG */
			if (xdelta < -5) {
				c = 'c';
			} else {
				ch = GetChar();

/*  character translation */

				strch[n] = ch;
				ch = CodeTable[font][AsciiTable[font][ch-32]&0xff];
				string[n++] = ch;
				if (ch != 0)
					break;
				c = 'w';
			}

		default:
/*
 * check cumulative error
 * The error should be less than .5 space on the wide side or
 * about 5 spaces on the narrow side.  The canned numbers of
 * 5 pixels and 50 pixels may also be O.K.
 */
			if (xerror > 5 || xerror < -50) {
				HorizontalPosition = x;
				for (i = 0; i < n; i++) {
					HorizontalPosition += strpos[i];
					PutCharacter(strch[i]&0xff);
				}
			} else {
				string[n] = '\0';
				PutString(x, string);
			}
			UnGetChar(c);
			return;
		}
	}
}

PutString(x, string)
int	x;
char	*string;
{
	if (!DisplayOutput)
		return;
	DrawString(x, VerticalPosition, string);
}
#endif /* SUNTOOLS */

PutCharacter(c)
int	c;
{
	int	OldFont, i, cwidth;
	char	**AsciiTable, *SpecialCharacterName, **CodeTable;
	short	*SpecialCharacterNumber;
	struct Font **FontInformation;
	struct dev *Device;

	AsciiTable = OutputTypesetter->AsciiTable;
	SpecialCharacterNumber = OutputTypesetter->SpecialCharacterNumber;
	SpecialCharacterName = OutputTypesetter->SpecialCharacterName;
	CodeTable = OutputTypesetter->CodeTable;
	FontInformation = OutputTypesetter->FontInformation;
	Device = &OutputTypesetter->Device;


	if (!DisplayOutput){
		return;
	}

	if (c == ' ')				/* Spaces are easy */
		return;
	  
	c -= 32;
	if (c < 0 || c > 128-32+Device->nchtab){
		warning(
		      "Bad character (%d) passed to PutCharacter at line %d.\n"
		      ,c+32,LineNumber);
	}
	
	OldFont = font;

	if (AsciiTable[font][c] == 0){			/* If its not in the
							   table then look for
							   it in the other 
							   fonts. */
		for (i=0;i<=Device->nfonts;i++){
			if (!FontInformation)
				continue;
#ifdef	MAYBENOT			
			if (!FontInformation[i]->specfont)
				continue;
#endif	MAYBENOT			
			if (!AsciiTable[i])
				continue;
			if (AsciiTable[i][c])
				break;
		}
		if (i <= Device->nfonts){
			font = i;
		        (void) VirtualLoadFont(FontInformation[i]->namefont,
					       size);
		} else if (c < 128-32) {
			warning("Can't find (%c)%d in %s character table.\n",
			    c+32, c+32, OutputTypesetter->Name);
			return;
		} else {
			char *s = &SpecialCharacterName[SpecialCharacterNumber[c+32-128]];
			switch(PACK_TWO_CHARS(s[0], s[1])) {
#ifndef SUNTOOLS
			case PACK_TWO_CHARS('F', 'i'):
				PutString(HorizontalPosition, "ffi");
				return;

			case PACK_TWO_CHARS('F', 'l'):
				PutString(HorizontalPosition, "ffl");
				return;

			case PACK_TWO_CHARS('f', 'i'):
				PutString(HorizontalPosition, "fi");
				return;

			case PACK_TWO_CHARS('f', 'f'):
				PutString(HorizontalPosition, "ff");
				return;

			case PACK_TWO_CHARS('f', 'l'):
				PutString(HorizontalPosition, "fl");
				return;
#endif /* !SUNTOOLS */

			default:
				warning(
				      "Can't find (%s)%d in %s character table.\n",
				      &SpecialCharacterName[SpecialCharacterNumber[
							     c+32-128]],
				      c+32,
				      OutputTypesetter->Name);
				return;
			}
		}
	}

	LoadFontBits();

#ifndef NOADJUST
	/*
	 *  A hack to help centre the X11 font in the space of the laser
	 *  printer font so it looks much nicer. Taken from David
	 *  Blythe's xproof previewer for X10, at the University of
	 *  Toronto. It might work in Suntools as well - I haven't
	 *  tried. - moraes
	 */
	cwidth = UserTypesetter->WidthTable[font]
		[UserTypesetter->AsciiTable[font][c]&0xff]&0xff;
	cwidth = (cwidth * UserTypesetter->PointSizeTable[size - 1]
	 + UserTypesetter->Device.unitwidth/2)
	 / UserTypesetter->Device.unitwidth;
#else
	cwidth = 0;
#endif
	
	DrawCharacter(HorizontalPosition,VerticalPosition,
		      CodeTable[font][AsciiTable[font][c]&0xff], cwidth);
	SetFont(OldFont);
}

PutSpecialCharacter(CharacterName)
char	*CharacterName;
{
	int 	i, c;
	struct dev *Device;
	short	*SpecialCharacterNumber;
	char	*SpecialCharacterName;

	Device = &OutputTypesetter->Device;
	SpecialCharacterNumber = OutputTypesetter->SpecialCharacterNumber;
	SpecialCharacterName = OutputTypesetter->SpecialCharacterName;

	if (!DisplayOutput){
		return;
	}

#ifndef SUNTOOLS
	/* Draw Troff special graphics (non-character) */

	c = PACK_TWO_CHARS(CharacterName[0], CharacterName[1]);

	switch (c) {
/*
 * /bv{0 800 moveto 0 -1000 rls}def
 */
	case PACK_TWO_CHARS('b', 'v'):
		/*line(.25, -.8, .25, .2);*/
/* /bv{20 800 moveto 0 -1000 rls}def */
		line(.020, -.8, .020, .2);
		return;
/*
 * /br{0 750 moveto 0 -1000 rls}def
 */
	case PACK_TWO_CHARS('b', 'r'):
		line(0., -.75, 0., .25);
		return;
/*
 * /ru{500 0 rls}def
 */
	case PACK_TWO_CHARS('r', 'u'):
		line(0., 0., .5, 0.);
		return;
/*
 * /lf{0 800 moveto 0 -1000 rlineto s4 0 rls}def
 */
	case PACK_TWO_CHARS('l', 'f'):
		line(.25, -.8, .25, .2);
		line(.25, .2, .5, .2);
		return;
/*
 * /rf{0 800 moveto 0 -1000 rlineto s4 neg 0 rls}def
 */
	case PACK_TWO_CHARS('r', 'f'):
		line(.25, -.8, .25, .2);
		line(.25, .2, 0., .2);
		return;
/*
 * /lc{0 -200 moveto 0 1000 rlineto s4 0 rls}def
 */
	case PACK_TWO_CHARS('l', 'c'):
		line(.25, .2, .25, -.8);
		line(.25, -.8, .5, -.8);
		return;
/*
 * /rc{0 -200 moveto 0 1000 rlineto s4 neg 0 rls}def
 */
	case PACK_TWO_CHARS('r', 'c'):
		line(.25, .2, .25, -.8);
		line(.25, -.8, 0., -.8);
		return;
/*
 * /sq{80 0 rmoveto currentpoint dround newpath moveto
 *     640 0 rlineto 0 640 rlineto -640 0 rlineto closepath stroke}def
 */
	case PACK_TWO_CHARS('s', 'q'):
		line(.08, 0., .72, 0.);
		line(.72, 0., .72, -.64);
		line(.72, -.64, .08, -.64);
		line(.08, -.64, .08, 0.);
		return;
/*
 * /bx{80 0 rmoveto currentpoint dround newpath moveto
 *     640 0 rlineto 0 640 rlineto -640 0 rlineto closepath fill}def
 */
	case PACK_TWO_CHARS('b', 'x'):
		box(.08, -.64, .72, 0.);
		return;
/*
 * /ci{500 360 rmoveto currentpoint newpath 333 0 360 arc
 *     50 setlinewidth stroke}def
 */
	case PACK_TWO_CHARS('c', 'i'):
		circle(.5, -.36, .25, 0);
		return;
/*
 * /lt{0 -200 moveto 0 550 rlineto currx 800 2cx s4 add exch s4 a4p stroke}def
 */
	case PACK_TWO_CHARS('l', 't'):
		line(.25, .2, .25, -.55);
		arc(.5, -.55, .25, 180, -90, 0);
		return;
/*
 * /rt{0 -200 moveto 0 550 rlineto currx 800 2cx s4 sub exch s4 a4p stroke}def
 */
	case PACK_TWO_CHARS('r', 't'):
		line(.25, .2, .25, -.55);
		arc(0., -.55, .25, 0, 90, 0);
		return;
/*
 * /lb{0 800 moveto 0 -550 rlineto currx -200 2cx s4 add exch s4 a4p stroke}def
 */
	case PACK_TWO_CHARS('l', 'b'):
		line(.25, -.8, .25, -.05);
		arc(.5, -.05, .25, 180, 90, 0);
		return;
/*
 * /rb{0 800 moveto 0 -500 rlineto currx -200 2cx s4 sub exch s4 a4p stroke}def
 */
	case PACK_TWO_CHARS('r', 'b'):
		line(.25, -.8, .25, -.05);
		arc(0., -.05, .25, 0, -90, 0);
		return;
/*
 * /lk{0 800 moveto 0 300 -300 300 s4 arcto pop pop 1000 sub
 *     0 300 4 2 roll s4 a4p 0 -200 lineto stroke}def
 */
	case PACK_TWO_CHARS('l', 'k'):
		line(.25, -.8, .25, -.55);
		arc(0., -.55, .25, 0, -90, 0);
		arc(0., -.05, .25, 90, -90, 0);
		line(.25, -.05, .25, .2);
		return;
/*
 * /rk{0 800 moveto 0 300 s2 300 s4 arcto pop pop 1000 sub
 *     0 300 4 2 roll s4 a4p 0 -200 lineto stroke}def
 */
	case PACK_TWO_CHARS('r', 'k'):
		line(.25, -.8, .25, -.55);
		arc(.5, -.55, .25, 180, 90, 0);
		arc(.5, -.05, .25, 90, 90, 0);
		line(.25, -.05, .25, .2);
		return;
/*
 * /bu{200 250 rmoveto currentpoint newpath 200 0 360 arc closepath fill}def
 */
	case PACK_TWO_CHARS('b', 'u'):
		circle(.25, -.36, .25, 1);
		return;
/*
 * /ob{200 250 rmoveto currentpoint newpath 200 0 360 arc closepath stroke}def
 */
	case PACK_TWO_CHARS('o', 'b'):
		arc(.25, -.36, .25, 0, 360, 0);
		return;
/*
 * /vr{0 800 moveto 0 -770 rls}def
 */
	case PACK_TWO_CHARS('v', 'r'):
		line(.25, -.8, .25, -.03);
		return;
/*
 * /rn{0 840 moveto 500 0 rls}def
 */
	case PACK_TWO_CHARS('r', 'n'):
		line(.25, -.84, .75, -.84);
		return;
/*
 * /ul{0 -140 moveto 500 0 rls}def
 */
	case PACK_TWO_CHARS('u', 'l'):
		/* line(.25, .14, .75, .14); */
/* /ul{0 -250 moveto 500 0 rls}def */
		line(.0, .250, .500, .250);
		return;
/*
 * /fractm [.65 0 0 .6 0 0] def
 * /fraction
 *  {/fden exch def /fnum exch def gsave /cf currentfont def
 *   cf fractm makefont setfont 0 .3 dm 2 copy neg rmoveto
 *   fnum show rmoveto currentfont cf setfont(\244)show setfont fden show 
 *   grestore ditwid 0 rmoveto} def
 */
	case PACK_TWO_CHARS('1', '8'):
		draw_fraction('1', '8', size);
		return;
	case PACK_TWO_CHARS('3', '8'):
		draw_fraction('3', '8', size);
		return;
	case PACK_TWO_CHARS('5', '8'):
		draw_fraction('5', '8', size);
		return;
	case PACK_TWO_CHARS('7', '8'):
		draw_fraction('7', '8', size);
		return;
	case PACK_TWO_CHARS('1', '3'):
		draw_fraction('1', '3', size);
		return;
	case PACK_TWO_CHARS('2', '3'):
		draw_fraction('2', '3', size);
		return;
/*
 *  space codes
 */
	case PACK_TWO_CHARS('\\', '^'):
		return;
	case PACK_TWO_CHARS('\\', '|'):
		return;
	}
#endif /* SUNTOOLS */
	

	for (i=0;i<Device->nchtab;i++){
		if (STREQ(&SpecialCharacterName[SpecialCharacterNumber[i]],
			   CharacterName))
			break;
	}
	if (i < Device->nchtab){
		PutCharacter(i+128);
		return;
	} else {
		switch(c) {

#ifndef SUNTOOLS
		case PACK_TWO_CHARS('F', 'i'):
			PutString(HorizontalPosition, "ffi");
			return;

		case PACK_TWO_CHARS('F', 'l'):
			PutString(HorizontalPosition, "ffl");
			return;

		case PACK_TWO_CHARS('f', 'i'):
			PutString(HorizontalPosition, "fi");
			return;

		case PACK_TWO_CHARS('f', 'f'):
			PutString(HorizontalPosition, "ff");
			return;

		case PACK_TWO_CHARS('f', 'l'):
			PutString(HorizontalPosition, "fl");
			return;
#endif /* !SUNTOOLS */

		default:
			warning("Couldn't find special character %s in %s character list.\n",
		  	    CharacterName, OutputTypesetter->Name);


		}
	}
}

	
#ifdef SUNTOOLS
PrintDocument(ActualFileName,Printer)
char	*ActualFileName, *Printer;
{
	char	Command[BUFSIZ];	
	int	i, SavedPageNumber;

	SavedPageNumber = CurrentPage;		/* Save this, just in case */
	SaveTitleBar();
	if (!LastPage && RealBufferPointer != RealFilePointer){
		for (i=1; i < MAXPAGES; i++){
			if (PagePlace[i])
				CurrentPage = i;
		}
		FileSeek(PagePlace[CurrentPage]);
		DisplayOutput = 0;		/* Now flush the rest of input
						   */
		while (!LastPage || !feof(RealFilePointer)){
			SetTitleBar("Flushing", CurrentPage);
			CurrentPage = ParseInput();
		}
	}
	
	SetTitleBar("Printing Document", -1);
	fflush(RealBufferPointer);

	sprintf(Command,"%s%s %s",LPRCOMMAND,Printer,
		ActualFileName);
	system(Command);
	RestoreTitleBar();
	CurrentPage = SavedPageNumber;
}


PrintPage(PageNumber,Printer)
int	PageNumber;
char	*Printer;
{
	char	FileName[BUFSIZ], Command[BUFSIZ];
	FILE	*fp;
	extern char *mktemp();

	(void) strcpy(FileName,"/tmp/suntroff.XXXXXX");
	(void) mktemp(FileName);
	
	fp = fopen(FileName,"w");
	if (!fp){
		warning("Can't open %s for writing page image.\n",
		      FileName);
		return;
	}

	SaveTitleBar();
	SetTitleBar("Printing", PageNumber);
	OutputPage(0L, PagePlace[1], CurrentFilePointer, fp);
	OutputPage(PagePlace[PageNumber], PagePlace[PageNumber+1],
		   CurrentFilePointer,fp);
	fprintf(fp, "\n");
	fprintf(fp, "x trailer\n");
	fprintf(fp, "x stop\n");
	fclose(fp);
	
	(void) sprintf(Command,"%s%s %s", LPRCOMMAND, Printer, FileName);
	(void) system(Command);
        unlink(FileName);
	RestoreTitleBar();
}
#else
/*
 * X Windows versions -- these use popen() instead of system() to cut
 * down on temporary files, and make PrinterCommand more standard.
 */
/*ARGSUSED*/
PrintDocument(ActualFileName,Printer)
char	*ActualFileName, *Printer;
{
	int	i, SavedPageNumber;
	extern char *PrinterCommand;
	FILE	*fp;

	SavedPageNumber = CurrentPage;		/* Save this, just in case */
	SaveTitleBar();
	if (!LastPage && RealBufferPointer != RealFilePointer){
		for (i=1; i < MAXPAGES; i++){
			if (PagePlace[i])
				CurrentPage = i;
		}
		FileSeek(PagePlace[CurrentPage]);
		DisplayOutput = 0;		/* Now flush the rest of input
						   */
		while (!LastPage || !feof(RealFilePointer)){
			SetTitleBar("Flushing", CurrentPage);
			CurrentPage = ParseInput();
		}
	}
	
	SetTitleBar("Printing Document", -1);
	fflush(RealBufferPointer);

	fp = popen(PrinterCommand, "w");
	if (!fp) {
		warning("Can't run command \"%s\" for printing document.\n",
		      PrinterCommand);
		return;
	}
	OutputPage(0L, 0L, CurrentFilePointer, fp);
	if (pclose(fp) == -1) {
	    warning("\"%s\" failed\n", PrinterCommand);
	    return;
	}
	RestoreTitleBar();
	CurrentPage = SavedPageNumber;
}


/*ARGSUSED*/
PrintPage(PageNumber,Printer)
int	PageNumber;
char	*Printer;
{
	extern char *PrinterCommand;
	FILE	*fp = popen(PrinterCommand, "w");
	
	if (!fp) {
		warning("Can't run command \"%s\" for printing page.\n",
		      PrinterCommand);
		return;
	}

	SaveTitleBar();
	SetTitleBar("Printing", PageNumber);
	OutputPage(0L, PagePlace[1], CurrentFilePointer, fp);
	OutputPage(PagePlace[PageNumber], PagePlace[PageNumber+1],
		   CurrentFilePointer,fp);
	(void) fputs("\nx trailer\nx stop\n", fp);
	if (pclose(fp) == -1) {
	    warning("\"%s\" failed\n", PrinterCommand);
	    return;
	}
	RestoreTitleBar();
}
#endif


/*ARGSUSED*/
OutputPage(Start,End,Input,Output)
long	Start, End;
FILE	*Input, *Output;
{
	int	c;

	if (End != 0 && Start > End){
		fatal("PrintPage: starting offset (%d) is less than\nending offset (%d)\n",Start,End);
		return;
	}

	FileSeek(Start);

	do {
		c = GetChar();
		if (c != EOF){
			putc(c, Output);
		}
		Start ++;
	} while (c != EOF && (End == 0 || Start < End));
	
}
	

SearchFile(String,PageNumber,Direction)
int	PageNumber, Direction;
char	*String;
{
	PageNumber += Direction;		/* Skip Current Page */

	if (PageNumber <= 0 || (LastPage && PageNumber > PageNumber) ||
	    !String || String[0] == NULL){
		return(0);
	}

	if (PagePlace[PageNumber] == 0){
		warning("Can't find the current page while searching.");
		return(0);
	}

	FileSeek(PagePlace[PageNumber]);
	for (;PageNumber>0 ;PageNumber += Direction){
		if (LastPage && PageNumber > LastPage){
			return(0);
		}
		if (feof(CurrentFilePointer)){
			return(0);
		}
		if (Direction < 0){
			FileSeek(PagePlace[PageNumber]);
		}

		SetTitleBar("Searching",PageNumber);
		if (SearchPage(String)) {
			return(PageNumber);
		}
	}
	return(0);
}

SearchPage(String)
char	*String;
{
	char	*StringP = String;
	int	c;

	while ((c = GetChar()) != EOF){
		switch(c){
		case ' ':
		case 0:
		case '{':
		case '}':
		case '\n':
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			GetChar();
		case 'c':
			c = GetChar();
			if (c == *StringP){
				StringP++;
				if (*StringP == 0){
					return(1);
				}
			} else {
				StringP = String;
			}
			break;
		case 'C':
			GetChar();
			GetChar();
			StringP = String;
			break;
		case 'D':
		case 'x':
		case '#':
			do {
				c = GetChar();
			} while (c != '\n' && c != EOF);
			StringP = String;
			break;
		case 'w':
			if (*StringP == ' '){
				*StringP++;
				if (*StringP == 0){
					return(1);
				}
			} else {
				StringP = String;
			}
			break;
		case 'n':
			if (*StringP == ' '){
				*StringP++;
				if (*StringP == 0){
					return(1);
				} 
			} else {
				StringP = String;
			}
			GetNumber();
			GetNumber();
			break;
		case 's':
		case 'f':
		case 'H':
		case 'V':
		case 'h':
		case 'v':
			GetNumber();
			break;
		case 'p':
			(void) GetNumber();
			(void) RememberPagePosition();
			return(0);
		default:
			warning("Unknown input character %c(%d)\n",
			      c,c);
			break;
		}
	}
	return(0);
}

static	UnreadCharacter = 0;

/*
 * Pages are ordered by physical position in the file, because of the weird
 * numbers possible with troff pages
 */
RememberPagePosition()
{
	extern long ftell();
	int pageplace = ftell(RealBufferPointer);
	int mid = 0;
#ifdef SEEK
	char *unread = "";
#endif

	if (UnreadCharacter) {
		pageplace--;
		UnreadCharacter = 0;
#ifdef SEEK
		unread = " with unread character";
#endif /* SEEK */
	}

	if (pageplace > PagePlace[MaxPage]) { /* Usual case */
		PagePlace[++MaxPage] = pageplace;
		mid = MaxPage;
	} else {
		/* Binary search for the page - it must be in the table */
		int hi = MaxPage;
		int lo = 0;
		while (hi >= lo) {
			mid = lo + (hi - lo) / 2;
			if (PagePlace[mid] == pageplace)
				break;
			if (pageplace < PagePlace[mid])
				hi = mid - 1;
			else
				lo = mid + 1;
		}
		if (PagePlace[mid] != pageplace) 
			fatal("pageplace 0x%x wasn't in table\n");
	}
#ifdef	SEEK					
	printf("Remembering page %d at 0x%x%s.\n", mid, PagePlace[mid], 
	 unread);
#endif	/* SEEK	*/
	return(mid);
}

FileSeek(Position)
long	Position;
{
	UnreadCharacter = 0;
	CurrentFilePointer = RealBufferPointer;
	fseek(CurrentFilePointer,Position,0);
#ifdef	SEEK		
	printf("Seeking to %x of real buffer.\n", Position);
#endif	SEEK
}

GetChar(){
	int	i;
	
	if (UnreadCharacter){
		i = UnreadCharacter;
		UnreadCharacter = 0;
		return(i);
	}

	i = getc(CurrentFilePointer);
	if (CurrentFilePointer != RealBufferPointer){
		putc(i, RealBufferPointer);
	}			
	
	if (i == EOF){
		if (RealFilePointer != RealBufferPointer){
			if (CurrentFilePointer == RealBufferPointer){
				CurrentFilePointer = RealFilePointer;
				i = GetChar();
			}
		}
	}
	
	return(i);
}

UnGetChar(c)
int	c;
{
	if (UnreadCharacter){
		fatal("Can't UnGetChar more than one character.\n");
	}

	UnreadCharacter = c;
}

char *
GetLine(Buffer, Length)
char	*Buffer;
int	Length;
{
	int 	i = 0, c = 0;
	char	*p = Buffer;
	
	Length--;			    /* Save room for final NULL */
	while (i < Length && (c = GetChar()) != EOF && c != '\n'){
		if (p)
			*p++ = c;
	}
	if (c == '\n' && p){		    /* Retain the newline like fgets */
		*p++ = c;
	}
	if (c == '\n')
		UnGetChar(c);
	

	if (p)	
		*p = '\0';
	return(Buffer);
} 

char *
GetWord(Buffer, Length)
char	*Buffer;
int	Length;
{
	int 	i = 0, c;
	char	*p = Buffer;
	
	Length--;			    /* Save room for final NULL */
	
	while ((c = GetChar()) != EOF && isspace(c));
	if (c != EOF){
		UnGetChar(c);
	}

	while (i < Length && (c = GetChar()) != EOF && !isspace(c)){
		if (p)
			*p++ = c;
	}
	if (c != EOF)
		UnGetChar(c);
	
	if (p)
		*p = NULL;
	return(Buffer);
} 

GetNumber(){
	int	i = 0,  c;

	while ((c = GetChar()) != EOF && isspace(c));

	if (c != EOF){
		UnGetChar(c);
	}

	while ((c = GetChar()) != EOF && isdigit(c)){
		i = i*10 + c - '0';
	}

	if (c != EOF)
		UnGetChar(c);
	return (i);
}
	
	    
