/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	font.c - All the stuff you will ever need to deal with
 *		troff fonts, versatec fonts for the screen and picking
 *		the best font to use when nothing else is available.  
 *		Tries to be intelligent about the best font to use and
 *		to cache all the information it figures out.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
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
#include	<sys/dir.h>

#ifndef SUNTOOLS
extern Display *dpy;
extern GC gc;
/*
 *  Maximum number of fonts to list - make it too big and XListFonts
 *  will fail. Make it large enough to list all fonts you have - 1k
 *  should be enough for most installations
 */
#define MAX_XFONTS 1024
/* MAXDEVICE is the maximum length of the name of the device - (eg) devsun */
#define MAXDEVICE 120
#define LINESIZE 128
#endif
extern int SUNRES;

extern char *malloc();

#ifdef SUNTOOLS
/* emalloc - call fatal() if it can't get memory */
char *
emalloc(n)
int n;
{
	char *cp = malloc((unsigned int) n);
	if (cp == NULL)
		fatal("Can't allocate memory\n");
	return(cp);
}
#else
extern char *XtMalloc();
#define emalloc(n)	XtMalloc((unsigned int) (n))
#endif

/* rfont () - allocate space and read section of font description
 *
 *	fp - font file
 * 	n - number of bytes to read
 *	file - file name
 *	exit - pointer to table section
 */
char *
rfont(fp, n, file)
int	fp;
int	n;
char	*file;
{
	char	*p;
	extern char *malloc();

	if((p = (char *) malloc((unsigned) n)) == NULL)
		fatal("Can't allocate space for reading %d bytes of typesetter information.\n", n);
	if (read(fp, p, n) != n)
		fatal("Can't read font information from %s.\n", file);
	return(p);
}
		     
/*
	Each typesetter has a DESC.out file that contains information about
	the capabilities of the device.  The InitFonts function reads this
	file and puts 	the information in a useable format for this program.

	The following information is present in the file:
	Device			A binary image of the Device structure
				(see dev.h)
	
	PointSizeTable		An array of shorts that map a point size
				number (used in the output file format) into
				a real paper point size.

	SpecialCharacterNumber	An array of indexes that can be used to map
				a special character number (- 128) into the
				special character name array that follows.

	SpecialCharacterName	Many null terminated strings representing
				the names of the special characters that are
				recognized by this device.

	Then for each of the Device.nfonts fonts the following information
	is present (index by i, the font number).
	FontInformation[i]:	Lots of miscellaneous information about the
				font.
	WidthTable[i]		A pointer to a table of characters indicating
				the width of each character in the font.  The
				width listed is that of point size 'unitwidth'
				(see the Device structure.)
	Kerning[i]		A pointer to a table of ascender/descender
				information
	CodeTable[i]		A pointer to a table giving the code sent to
				the typesetter to actually produce this
				character.
	AsciiTable[i]		A pointer to a table giving the mapping
				between the ASCII code for a character and
			        the character number in the device width
				tables. Note: The first 128-32 entries of this
				table give the mapping to the printable ascii
				characters.  Those indices above 128-32 are
				used for the special characters defined in the
				SpecialCharacterName[SpecialCharacterNumber]
				array.

To look up a character do the following....
If the character is a printable ascii simply subtract 32 from its character
code, and look up this number in the AsciiTable.  The number retuned by
the AsciiTable can be used to index into the WidthTable, Kerning and
CodeTables.

If the character is a special character then it is necessary to search
through the names listed in the SpecialCharacterName array until a match
is found.  Then add 128-32 to this number and then do the above procedure.
				
*/

struct Typesetter TypesetterList = {
	"*********", };

struct Typesetter *UserTypesetter = NULL, *OutputTypesetter = NULL,
		  *DefaultTypesetter = NULL;

static char DefaultFont[] = "R";

static char *FontName = NULL;
	
						/* The table below is used
						 * to keep the mapping between
						 * font positions as used by
						 * the user and the actual
						 * font name.
						 */
static char FontPosition[NFONTS][MaxFontName+1];

/* LoadDevice - This routine is called by the routine that parses device
 *	control requests in the user's input file.  At this point the
 *	user has specified the desired typesetter.....we first check
 *	check to see if we already know about this one.  If not then we
 *	call the InitFonts() routine to load the description file.
 *
 *	This routine is simply called with the name of the typesetter.
 *	All typesetters are stored in a linked list so we just skip down
 *	the list until we find a match.
 */
struct Typesetter *
LoadDevice(Device)
char	*Device;
{
	struct Typesetter *CurrentTypesetter, *LastTypesetter;
	extern char *malloc();
	char	temp[BUFSIZ];

	for (CurrentTypesetter = &TypesetterList;
	     CurrentTypesetter;
	     CurrentTypesetter = CurrentTypesetter->NextTypesetter){
		     if (STREQ(Device,CurrentTypesetter->Name)){
			     return(CurrentTypesetter);
		     }
		     LastTypesetter = CurrentTypesetter;
	     }
	CurrentTypesetter = (struct Typesetter *)
	  			malloc(sizeof(*CurrentTypesetter));
	if (!CurrentTypesetter) {
		fatal("Can't get space for typesetter description.\n");
	}

	LastTypesetter->NextTypesetter = CurrentTypesetter;
	CurrentTypesetter->NextTypesetter = NULL;
	strncpy(CurrentTypesetter->Name,Device,MAXNAME);

	if (InitFonts(CurrentTypesetter)) {
#ifdef SUNTOOLS
		sprintf(temp,"%s/dev%s/DESC.out",FONTDIR,Typesetter->Name);
		fatal("Can't open typesetter description %s.\n",temp);
#else /* !SUNTOOLS */
		InitDefaultFonts(CurrentTypesetter);
#endif /* !SUNTOOLS */
	}
	return(CurrentTypesetter);
}
		     
/*
 * InitFonts - Given a typesetter structure (that is empty except for the
 * 	name) go out and read the description file (DESC.out) and fill in
 *	all the entries.
 *
 *	This is called by the LoadDevice() routine when a typesetter,
 *	previously unknown to this program, is requested.
 */
InitFonts(Typesetter)
struct Typesetter *Typesetter;
{
	char	temp[BUFSIZ];
	int	i;
	int	fp;

	sprintf(temp,"%s/dev%s/DESC.out",FONTDIR,Typesetter->Name);
	if ((fp = open(temp, O_RDONLY)) <= 0) {
		return 1;
	}
	if (read(fp, (char *) &Typesetter->Device, sizeof(Typesetter->Device)) 
						!= sizeof(Typesetter->Device))
		fatal("Can't read header of %s.\n",temp);
	
	Typesetter->PointSizeTable = (short *) rfont(fp,
		sizeof(short) * (Typesetter->Device.nsizes + 1), temp);

	Typesetter->SpecialCharacterNumber = (short *) rfont(fp,
		sizeof(short) * Typesetter->Device.nchtab, temp);
	
	Typesetter->SpecialCharacterName = (char *) rfont(fp,
		Typesetter->Device.lchname, temp);
	
	Typesetter->WidthTable = (char **)emalloc(NFONTS*sizeof(char *));
	Typesetter->CodeTable = (char **)emalloc(NFONTS*sizeof(char *));
	Typesetter->AsciiTable = (char **)emalloc(NFONTS*sizeof(char *));

 	for (i=1;i<=Typesetter->Device.nfonts;i++){
		int	NumberWidths;
		
		Typesetter->FontInformation[i] = (struct Font *) rfont(fp,
			sizeof(struct Font), temp);
		NumberWidths = Typesetter->FontInformation[i]->nwfont & 0377;

		Typesetter->WidthTable[i] = rfont(fp, 3 * NumberWidths +
			Typesetter->Device.nchtab + 128 - 32, temp);
						/* Skip Kerning Table */

		Typesetter->CodeTable[i] = Typesetter->WidthTable[i] +
			2 * NumberWidths;

		Typesetter->AsciiTable[i] = Typesetter->WidthTable[i] +
			3 * NumberWidths;

		SetFontPositionBuffer(i,Typesetter->FontInformation[i]->namefont,
				Typesetter->FontInformation[i]->intname);
	}

	close(fp);

						/* Clear out the rest of the
						   font table. */
	for (i=Typesetter->Device.nfonts+1;i<NFONTS;i++){
		Typesetter->FontInformation[i] = (struct Font *) 0;
	}

						/* Set up the default font
						   position (0) */
	Typesetter->FontInformation[0] =
	    (struct Font *)emalloc(sizeof (struct Font));
	Typesetter->WidthTable[0] = (char *)emalloc(3 * 255 +
	    Typesetter->Device.nchtab + 128 - 32);
	Typesetter->FontInformation[0]->nwfont = 255;
	Typesetter->CodeTable[0] = Typesetter->WidthTable[0] + 2 * 255;
	Typesetter->AsciiTable[0] = Typesetter->WidthTable[0] + 3 * 255;
	Typesetter->FontBitHead.NextFont = NULL;
	Typesetter->FontBitHead.NextSize = NULL;
	*(Typesetter->FontBitHead.Name) = NULL;

#ifdef	FONTDEBUG
	PrintDevice(Typesetter);
#endif	FONTDEBUG
	return (0);
}

/* InitTypesetter () - This routine is called in response to the "x i"
 *	device control request in the user's input file.  This routine 
 *	needs to do only two things.  Both of these things could be done
 *	in the LoadDevice routine but we do them here just so we have
 *	something to do in this routine.
 *
 *	First, we use ReadFontDirectory() to figure out which fonts we 
 *	have bits we can use on the screen.
 *
 *	Second, we load the FontPosition table with the default font 
 *	position mapping information.
 */
InitTypesetter()
{
	int	i;

	if (!ReadFontDirectory(UserTypesetter)){
#ifdef	TYPEDEBUG	
		printf("Loading the %s DESC.out file\n",DEFAULT_TYPESETTER);
#endif	TYPEDEBUG		
		if (!DefaultTypesetter){
			DefaultTypesetter = LoadDevice(DEFAULT_TYPESETTER);
			if (!DefaultTypesetter){
				fatal("Can't read default typesetter DESC\n");
			}
		}
		OutputTypesetter = DefaultTypesetter;
#ifdef	TYPEDEBUG		
		printf("Attempting to load bits for %s.\n",
		       OutputTypesetter->Name);
#endif	TYPEDEBUG		
		if (!ReadFontDirectory(DefaultTypesetter)){
			fatal("Can't get font bits for default typesetter.\n");
		}
	} else {
		OutputTypesetter = UserTypesetter;
	}
	
	for (i=0; i <= UserTypesetter->Device.nfonts; i++){
		if (UserTypesetter->FontInformation[i]) {
	        	strncpy(FontPosition[i],
				UserTypesetter->FontInformation[i]->namefont,
				MaxFontName);
		} else {
			FontPosition[i][0] = 0;
		}
	}
	for (; i<NFONTS; i++){
		FontPosition[i][0] = 0;
	}
		
#ifdef	FONTDEBUG
	PrintFontDirectory(OutputTypesetter);
#endif	FONTDEBUG
}	

/* SetFontPosition - Given a font number and a font name (from the "x f"
 *	device request) if the font is not already loaded (in that position)
 *      get the font info and store the name in the FontPostion buffer
 */
SetFontPosition(FontNumber,FontName,InternalFontName)
int	FontNumber;
char	*FontName, *InternalFontName;
{
	char	temp[BUFSIZ];
	int	NumberWidths;
	FILE	*fp;
	struct	Typesetter	*Typesetter = OutputTypesetter;
	int	norig;
	

	if (FontNumber < 0 || FontNumber > NFONTS)
		fatal("illegal x font command %d %s", FontNumber, FontName);
#ifdef FONTDEBUG
	printf("x font %d %s - FontPosition[%d]= %s\n", FontNumber, FontName,
	    FontNumber, FontPosition[FontNumber]);
#endif /* FONTDEBUG */
	if (strncmp(FontPosition[FontNumber], FontName, MaxFontName) == 0)
		return;
	if (Typesetter->FontInformation[FontNumber] == 0) {
#ifdef FONTDEBUG
		printf("allocating space for %s/dev%s/%s.out\n", FONTDIR,
		    Typesetter->Name, FontName);
#endif /* FONTDEBUG */
		Typesetter->FontInformation[FontNumber] =
		    (struct Font *)emalloc(sizeof (struct Font));
		Typesetter->WidthTable[FontNumber] = (char *)emalloc(3 * 255 +
		    Typesetter->Device.nchtab + 128 - 32);
		Typesetter->FontInformation[FontNumber]->nwfont = 255;
		Typesetter->CodeTable[FontNumber] =
		    Typesetter->WidthTable[FontNumber] + 2 * 255;
		Typesetter->AsciiTable[FontNumber] =
		    Typesetter->WidthTable[FontNumber] + 3 * 255;
	}
	SetFontPositionBuffer(FontNumber,FontName,InternalFontName);

	/* open the font file */
	sprintf(temp,"%s/dev%s/%s.out",FONTDIR,Typesetter->Name,
		FontName);
	fp = fopen(temp,"r");
	if (!fp){
		warning("Can't open typesetter description %s.\n",temp);
#ifdef FONTDEBUG
		printf("Can't open typesetter description %s.\n",temp);
#endif /* FONTDEBUG */
		return;
	}
	norig = Typesetter->FontInformation[FontNumber]->nwfont & 0xff;

	fread((char *) Typesetter->FontInformation[FontNumber],
	      sizeof(struct Font), 1, fp);
	NumberWidths = Typesetter->FontInformation[FontNumber]->nwfont & 0377;
	if (NumberWidths > norig) {
#ifdef FONTDEBUG
		printf("allocating space for %s/dev%s/%s.out\n", FONTDIR,
		    Typesetter->Name, FontName);
#endif /* FONTDEBUG */
		Typesetter->WidthTable[FontNumber] = (char *)emalloc(3 * 255 +
		    Typesetter->Device.nchtab + 128 - 32);
		norig = 255;
	}
	fread(Typesetter->WidthTable[FontNumber], NumberWidths * 3 +
	    Typesetter->Device.nchtab + 128 - 32, 1, fp);
	(void) fclose(fp);

	Typesetter->CodeTable[FontNumber] =
	    Typesetter->WidthTable[FontNumber] + 2 * NumberWidths;
	Typesetter->AsciiTable[FontNumber] =
	    Typesetter->WidthTable[FontNumber] + 3 * NumberWidths;
	Typesetter->FontInformation[FontNumber]->nwfont = norig;
}

/* SetFontPositionBuffer - Given a font number and a font name, store the
 * name in the font position table
 */
/*ARGSUSED*/
SetFontPositionBuffer(FontNumber,FontName,InternalFontName)
int	FontNumber;
char	*FontName, *InternalFontName;
{
	int	i;
	char *cp;

	if (FontNumber >= 0 && FontNumber <= NFONTS){
	        strncpy(FontPosition[FontNumber], FontName, MaxFontName);
#ifndef SUNTOOLS
		for(i = 0, cp = FontPosition[FontNumber]; *cp && 
		 i < MaxFontName; i++, cp++)
		 	if (isascii(*cp) && islower(*cp))
				*cp = tolower(*cp);
#endif
	}
}

/* SetFontSize - Given a point size change the current font to this new
 *	font.  Most of the work needed is done by VirtualLoadFont.  But
 *	then VirtualLoadFont doesn't really do any work....it just caches
 *	the information until it is needed later.
 */
SetFontSize(n)
int	n;					/* Actual Point Size */
{
	size = n;
	ditsiz = (float)(SUNRES * n)/75.;
#ifdef	FONTDEBUG
	printf("SetFontSize(%d)\n",n);
	printf("ditsiz=%g\n",ditsiz);
#endif	/* FONTDEBUG */

	if (FontName == NULL) {
		if ((FontName = (char *) malloc(sizeof(DefaultFont))) == NULL)
			fatal("Can't allocate font name space\n");
		strcpy(FontName, DefaultFont);
	}
	(void) VirtualLoadFont(FontName,size);
}

/* SetFont - Given a font number this routine looks it up in the FontPosition
 *	mapping table.  All the real work is done by the VirtualLoadFont()
 *	routine.
 */
SetFont(n)
int	n;					/* Internal Font Number */
{
	if (n >= 0 &&
	    n < NFONTS){
		font = n;
		FontName = FontPosition[n];
		(void) VirtualLoadFont(FontName,size);
	}
}

#ifdef	FONTDEBUG

/* PrintDevice - Print out an ASCII representation of the DESC.out file
 *	so that the correct operation of the InitFonts routine can be
 *	verified.  This also will verify that the DESC.out file and the
 *	dev.h file used to compile this program are compatible.
 */
PrintDevice(Typesetter)
struct Typesetter *Typesetter;
{
	int	i;
	struct dev	*Device;
	short	*SpecialCharacterNumber;
	char	*SpecialCharacterName;

	Device = &Typesetter->Device;

	printf("# Debugging output for suntroff of main device.\n");

	printf("res %d\n",Device->res);
	printf("hor %d\n",Device->hor);
	printf("vert %d\n",Device->vert);
	printf("unitwidth %d\n",Device->unitwidth);
	printf("paperwidth %d\n",Device->paperwidth);
	printf("paperlength %d\n",Device->paperlength);

	printf("sizes");
	for (i=0;i<Device->nsizes;i++){
		printf(" %d",Typesetter->PointSizeTable[i]);
	}
	printf("\n");

	printf("fonts %d",Device->nfonts);
	fflush(stdout);
	for (i=1;i<=Device->nfonts;i++){
		if (Typesetter->FontInformation[i]){
			printf(" %s",Typesetter->FontInformation[i]->namefont);
		} else {
			printf("FontInformation[i=%d] is null.\n",i);
		}
	}
	printf("\n");

	printf("charset\n");
	
	SpecialCharacterName = Typesetter->SpecialCharacterName;
	SpecialCharacterNumber = Typesetter->SpecialCharacterNumber;

	for (i=0;i<Device->nchtab;i++){
		printf(" %2s",
		       &SpecialCharacterName[SpecialCharacterNumber[i]]);
		if (i%10 == 0)
		  	printf("\n");
	}
       	printf("\n");

#if	FONTDEBUG > 1
	for (i=0;i<=Device->nfonts;i++){
		PrintFont(Typesetter,i);
	}
#endif	FONTDEBUG > 1
}

/* PrintFont - Print out an ASCII representation for the information in
 *	a font description file.
 */
PrintFont(Typesetter,i)
struct Typesetter *Typesetter;
int	i;
{
	int	j;
	struct Font **FontInformation1;
	char	**AsciiTable, **CodeTable, **WidthTable;
	short	*SpecialCharacterNumber;
	char	*SpecialCharacterName;
	struct Font **FontInformation;

	FontInformation = Typesetter->FontInformation;
	AsciiTable = Typesetter->AsciiTable;
	CodeTable = Typesetter->CodeTable;
	WidthTable = Typesetter->WidthTable;
	SpecialCharacterNumber = Typesetter->SpecialCharacterNumber;
	SpecialCharacterName = Typesetter->SpecialCharacterName;

	if (!FontInformation[i]){
		printf("There is no font information for font %d.\n",i);
		return;
	}
	printf("\n#\n");
	printf("name %s\n",FontInformation[i]->namefont);
	printf("internalname %s\n",FontInformation[1]->intname);
	printf("ligatures ");
	if (FontInformation[i]->ligfont & LFF) printf(" ff");
	if (FontInformation[i]->ligfont & LFI) printf(" fi");
	if (FontInformation[i]->ligfont & LFL) printf(" fl");
	if (FontInformation[i]->ligfont & LFFI) printf(" ffi");
	if (FontInformation[i]->ligfont & LFFL) printf(" ffl");
	printf(" 0\ncharset\n");
#ifdef notdef
	for (j=0;j<FontInformation[i]->nwfont & 0xff;j++){
		int	k;

		for (k=0;k<Typesetter->Device.nchtab+128-32;k++){
			if ((AsciiTable[i][k] & 0xff) == j){
				break;
			}
		}
		if (k < Typesetter->Device.nchtab+128-32){
			if (k < 128-32){
				printf("%c\t",k+32);
			} else {
				printf("%s\t",
				       &SpecialCharacterName
				       [SpecialCharacterNumber[k-(128-32)]]);
			}
		} else {
			printf("Couldn't find %d\t",j);
		}
		printf("%d\t%d\t%d\n",
		       WidthTable[i][j]&0xff,0,CodeTable[i][j]&0xff);
	}
#else
	for (j=0;j<FontInformation[i]->nwfont & 0xff;j++){
		int	k;
		int	nw;

		nw = AsciiTable[i][j] & 0xff;
		/*printf("%d %d ",j, nw);*/
		if (j < 128-32){
			printf("%c\t",j+32);
		} else {
			printf("%s\t",
			       &SpecialCharacterName
			       [SpecialCharacterNumber[j-(128-32)]]);
		}
		printf("%d\t%d\t%d\n",
		       WidthTable[i][nw]&0xff,0,CodeTable[i][nw]&0xff);
	}
#endif
	fflush(stdout);
}
	  

#endif	FONTDEBUG

/* ReadFontDirectory - Given a Typesetter go out to the BITDIR directory
 *	and figure out which fonts exist.  This information is important
 *	because it will allow the VirtualLoadFont() routine to search through
 *	memory instead of probing the file system every time it wants to
 *	change fonts.  Reading the entire directory is much more efficient
 *	then trying to do random probes looking for the closest match to
 *	a font.
 */
/* 
 *  For X Windows, the workstation that your display is on, and the
 *  machine you're running xtroff on may be quite different - and might
 *  have access to completely different fonts as a result. So we use
 *  XListFonts()
 */
ReadFontDirectory(Typesetter)
struct Typesetter *Typesetter;
{
#ifdef SUNTOOLS
	DIR	*dfp;
	struct direct *DirectoryEntry;
	char	Directory[BUFSIZ];
	char *p;
	char	*FontName, *FontSize;
	char *FileName;
	char	*Device;
#endif

	if (Typesetter->FontBitHead.NextFont){
		return(1);
	}
	  
#ifdef SUNTOOLS
	Device = Typesetter->Name;
	sprintf(Directory,"%s/dev%s",BITDIR,Device);
	dfp = opendir(Directory);
	if (!dfp){
		return(0);
	}

	while(DirectoryEntry = readdir(dfp)){

		if (!DirectoryEntry->d_fileno){
			continue;
		}
		FontSize = FontName = DirectoryEntry->d_name;
		while (isalpha(*FontSize)){
			FontSize++;
		}
		if (!*FontSize || FontName == FontSize)
			continue;			/* No Size */
		*FontSize = '\0';

		p = ++FontSize;
		while (isdigit(*p)){
			p++;
		}
		if (p == FontSize || *p != '\0'){
			continue;
		}
		FileName = (char *)malloc(strlen(Directory)+strlen(FontName) 
					  +strlen(FontSize)+4);
		if (!FileName){
			fatal("Can't allocate file name space.\n");
		}
		sprintf(FileName,"%s/%s.%s",Directory,FontName,FontSize);
		InsertFont(Typesetter,FontName,atoi(FontSize),FileName);
	}
	closedir(dfp);
	return(1);
#else /* !SUNTOOLS */
	if (strcmp(Typesetter->Name,"X11") != 0)
		return(readdevfontfile(Typesetter));
	else {
		readx11fontfile(Typesetter);
		return(1);
	}
#endif /* !SUNTOOLS */
}

#ifndef SUNTOOLS
readdevfontfile(Typesetter)
struct Typesetter *Typesetter;
{
	char **fontNames;
	char fontfamily[MAXDEVICE+8];
	char devname[MAXDEVICE];
	char *dpi;
	char *FontName;
	char *FontSize;
	char *FileName;
	char *p;
	int devnamelen;
	int dotsperinch;
	int nf;

	sprintf(devname, "dev%s", Typesetter->Name);
	devnamelen = strlen(devname);
	sprintf(fontfamily, "%s.*.*", devname);
	fontNames = XListFonts(dpy, fontfamily, MAX_XFONTS, &nf);
	if (nf == 0)
		return(0);
	while(--nf >= 0) {
		FontSize = FontName = fontNames[nf] + devnamelen + 1;
		while (isalpha(*FontSize)){
			FontSize++;
		}
		if (!*FontSize || FontName == FontSize)
			continue;			/* No Size */
		*FontSize = '\0';

		p = ++FontSize;
		while (isdigit(*p)){
			p++;
		}
		if (*p == '\0') {
			dpi = "75";
			dotsperinch = atoi(dpi);
			FileName = (char *)emalloc(devnamelen + strlen(FontName) 
			 + strlen(FontSize) + strlen(dpi) + 4);
			sprintf(FileName, "%s.%s.%s", devname,
			    FontName, FontSize);
		} else {
			if (p == FontSize || *p != '.'){
				continue;
			}

			*p++ = '\0';
			dpi = p;
			while (isdigit(*p)) {
				p++;
			}

			if (p == dpi || *p != '\0') {
				continue;
			}
			dotsperinch = atoi(dpi);
			FileName = (char *)malloc( (unsigned) (
			 devnamelen + strlen(FontName) + strlen(FontSize)
			 + strlen(dpi) + 4));
			if (!FileName){
				fatal("Can't allocate file name space.\n");
			}
			(void) sprintf(FileName, "%s.%s.%s.%s", devname,
			 FontName, FontSize, dpi);

			for(p = FontName; *p; p++)
				if (isascii(*p) && islower(*p)) 
					*p = toupper(*p);

			InsertFont(Typesetter, FontName,
			 (dotsperinch * atoi(FontSize) + (SUNRES / 2))
			  / SUNRES, FileName);
		}
		for(p = FontName; *p; p++)
			if (isascii(*p) && islower(*p)) 
				*p = toupper(*p);

		InsertFont(Typesetter, FontName,
		 (dotsperinch * atoi(FontSize) + (SUNRES / 2))
		  / SUNRES, FileName);
	}
	XFreeFontNames(fontNames);
	return (1);
}

readx11fontfile(Typesetter)
struct Typesetter *Typesetter;
{
	char pattern[LINESIZE];
	char basicname[LINESIZE];
	char **fontnames;
	char *cp;
	char *p;
	int fontsize;
	int nf;
	int i;
	extern char *GetFontMapString();

	for (cp = GetFontMapString(); *cp ;) {
		for (p = cp; *p && *p != '\n'; p++)
			if (! isalpha(*p))
				break;
		strncpy(basicname, cp, p-cp);	/* [di]troff name */
		basicname[p-cp] = '\0';
		while (*p && isspace(*p))
			p++;
		for (cp = p; *p && *p != '\n'; p++);
		strncpy(pattern, cp, p-cp);	/* X font name */
		pattern[p-cp] = '\0';
		cp = *p ? p+1 : p;
			
#ifdef FONTDEBUG
		printf("%s:\t%s\n", basicname, pattern);
#endif
		fontnames = XListFonts(dpy, pattern, MAX_XFONTS, &nf);
		if (nf == 0) {
#ifdef DEBUG
			(void) fprintf(stderr, "Warning: No fonts found for %s\n",pattern);
#endif
			continue;
		}
		for (i = 0; i < nf; i++) {
			fontsize = ConvertFontNameToSize(*fontnames),
#ifdef FONTDEBUG
			printf("%s %d\t%s\n", basicname,
				ConvertFontNameToSize(*fontnames),
				*fontnames);
#endif
			InsertFont(Typesetter, basicname,
			    (75 * fontsize + (SUNRES / 2)) / SUNRES,
			    *fontnames);
			fontnames++;
		}
	}
}

static char *
SkipFontNameElement (n)
	char	*n;
{
	while (*n != '-')
		if (!*++n)
			return 0;
	return n+1;
}

# define SizePosition		8
# define EncodingPosition	13

static
ConvertFontNameToSize (n)
	char	*n;
{
	int	i, size;

	for (i = 0; i < SizePosition; i++) {
		n = SkipFontNameElement (n);
		if (!n)
			return -1;
	}
	size = atoi (n);
	return size/10;
}
#endif /* !SUNTOOLS */

/* PrintFontDirectory - Print out all the fonts found by the ReadFontDirectory
 *	routine.
 */
PrintFontDirectory(Typesetter)
struct Typesetter *Typesetter;
{
	struct FontBitStruct *FontPointer, *SizePointer;
	char	*Device;

	Device = Typesetter->Name;
	printf("Found the following fonts for %s.",Device);

	for (FontPointer = &Typesetter->FontBitHead;
	     FontPointer;
	     FontPointer = FontPointer->NextFont){
		     printf("\n%s\t",FontPointer->Name);
		     for (SizePointer = FontPointer;
			  SizePointer;
			  SizePointer = SizePointer -> NextSize){
				  printf("%d ",SizePointer->Size);
			  }
	     }
	printf("\n");
}

/* InsertFont - OK, we've found a file name in the BITDIR that looks like
 *	a proper font name, add it to the font list for the current 
 *	typesetter.  This routine just looks for the proper point in the
 *	two dimensional (name and size) linked list.  AddFont() does
 *	all the real work.
 */
InsertFont(Typesetter,Name,Size,FileName)
struct Typesetter *Typesetter;
char	*Name, *FileName;
int	Size;
{
	struct FontBitStruct *FontPointer, *SizePointer, *CurrentFont;
	struct FontBitStruct *AddFont();

	for (FontPointer = &Typesetter->FontBitHead;
	     FontPointer;
	     FontPointer = FontPointer->NextFont){
		if (STREQ(Name,FontPointer->Name)){
			for (SizePointer = FontPointer;
			     SizePointer;
			     SizePointer = SizePointer->NextSize){
				     if (SizePointer->Size == Size){
					     return;
				     }
				     CurrentFont = SizePointer;
			     }
			CurrentFont->NextSize = AddFont(Name,Size,FileName);
			return;
		}
		CurrentFont = FontPointer;
	}
	CurrentFont->NextFont = AddFont(Name,Size,FileName);
}

/* AddFont - Add a structure to the list of available fonts indicating that
 *	the font Name and Size are available in the indicated FileName.
 */
struct FontBitStruct *
AddFont(Name,Size,FileName)
char	*Name, *FileName;
int	Size;
{

	struct FontBitStruct *font;

	font = (struct FontBitStruct *)malloc(sizeof(*font));
	if (!font){
		fatal("Can't get storage for font cache.\n");
	}
	
	strncpy(font->Name,Name,MaxFontName);
	font->Size = Size;
	font->NextFont = font->NextSize = NULL;
	font->Bits = NULL;
	font->FileName = FileName;
	return(font);
}

/* VirtualLoadFont - Given a desired FontName and PointSize find the font
 *	that we have screen bits for that is as close as possible.  Close
 *	is defined as being the same name and the largest point size that
 *	is smaller than the desired.
 *
 *	If we don't have the correct font name then go ahead and use the
 *	"R" font.  We don't bother giving the user an error message, but
 *	we probably should.
 *
 *	Once we have the best font name, then look down the list until
 *	we can find the best point size match.  If we can't find an exact
 *	match then use the NextBestFont which is the largest point size
 *	that is smaller than the desired (we try to avoid overlapping the
 *	characters).  If we didn't find the exact match then we install
 *	our best approximation as the correct choice so the user only
 *	sees one point size substitution error message.
 */
struct FontBitStruct *
VirtualLoadFont(FontName,PointSize)
char 	*FontName;				/* Actual Font Name */
int	PointSize;				/* Actual Point Size */
{
	struct FontBitStruct *NewFont, *NextBestFont, *FindFont(), *LastFont;

	if (!FontName || FontName[0] == 0)
		fatal("Bad font found in VirtualLoadFont.\n");

	if (CurrentFont && CurrentFont->Size == PointSize &&
	 STREQ(CurrentFont->Name, FontName)){
		return CurrentFont;
	}

	NewFont = FindFont(FontName);
	if (!NewFont){
		NewFont = FindFont(DefaultFont);
	}
	if (!NewFont){
		fatal("Couldn't find either the %s or the R font.",FontName);
	}

	NextBestFont = NewFont;

#ifdef	FONTDEBUG
	printf("Loading font name %s, point size %d.\n",
	       FontName,PointSize);
	if (CurrentFont)
		printf("Current font is %s, point size %d.\n",
			CurrentFont->Name, CurrentFont->Size);
#endif

	while (NewFont){
		if (NewFont->Size == PointSize){
			CurrentFont = NewFont;
			return(CurrentFont);
		}

		if (NextBestFont->Size > PointSize &&
		    NewFont->Size < NextBestFont->Size){
		        NextBestFont = NewFont;

		}
		    
		if (NewFont->Size < PointSize &&
		    NextBestFont->Size < NewFont->Size){
			    NextBestFont = NewFont;
		}
		LastFont = NewFont;
		NewFont = NewFont->NextSize;
	}
	if (abs(PointSize - NextBestFont->Size) > 1){
		warning("Couldn't find point size %d of the %s font. Using point size %d instead",
		      PointSize,FontName, NextBestFont->Size);
	}
	/*
	 * Don't have the correct size....so fake an entry so the user only
	 * gets one error message.
	 */
	if (PointSize != NextBestFont->Size){
		if (!LastFont ->NextSize){
			NewFont = AddFont(FontName, PointSize,
						     NextBestFont->FileName);
			NewFont->Bits = NextBestFont->Bits;
			NextBestFont = LastFont->NextSize = NewFont;
		}
	}
	CurrentFont = NextBestFont;
	return CurrentFont;
}

/* Findfont - Given a font name, find it on the list of available output
 *	type styles.  This routine is called by the VirtualLoadFont routine.
 */
struct FontBitStruct *
FindFont(FontName)
char	*FontName;
{
	struct FontBitStruct *NewFont;

	if (!OutputTypesetter){
		fatal("Program error: No typesetter specified.\n");
	}
	for (NewFont= &OutputTypesetter->FontBitHead;
	     NewFont;
	     NewFont=NewFont->NextFont){
		if (STREQ(NewFont->Name,FontName)){
			return(NewFont);
		}
	}
	return(0);
}

/* LoadFontBits - OK, the user has already requested a point size and
 *	typeface.  The VirtualLoadFont() routine has figured out which
 *	of the available output fonts is best to use.
 *
 *	Now this routine actually goes out and loads the correct (Versatec)
 *	font file.  All the real work is done by the SunTools pf_open()
 *	routine.  Thanks, Sun.
 */

LoadFontBits(){
#ifdef SUNTOOLS
	struct pixfont *pf_open();

	if (CurrentFont->Bits){
		return;				/* Already Loaded */
	}
#ifdef	FONTDEBUG	
	printf("Loading the bits from %s.\n",CurrentFont->FileName);
#endif	FONTDEBUG	

	CurrentFont->Bits = pf_open(CurrentFont->FileName);

	if (!CurrentFont->Bits){
		fatal("Can't read fonts from %s.\n",
		      CurrentFont->FileName);
	}
#else SUNTOOLS
	static Font curxfont = 0;
	
	if (CurrentFont->Bits){
		if (curxfont != CurrentFont->Bits->fid) {
#ifdef FONTDEBUG
			printf("changing to font %s\n", CurrentFont->FileName);
#endif FONTDEBUG
			curxfont = CurrentFont->Bits->fid;
			XSetFont(dpy, gc, curxfont);
		}
		return;				/* Already Loaded */
	}

#ifdef FONTDEBUG
	printf("Loading the bits from %s.\n",CurrentFont->FileName);
#endif FONTDEBUG

	CurrentFont->Bits = XLoadQueryFont(dpy, CurrentFont->FileName);

	if (!CurrentFont->Bits){
		fatal("Can't read fonts from %s.\n",
		      CurrentFont->FileName);
	}
	curxfont = CurrentFont->Bits->fid;
	XSetFont(dpy, gc, curxfont);
#endif SUNTOOLS
}
