#include <stdio.h>
#include <varargs.h>

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
/*
 * Yechh!! The only way I know of to extract the size of the text widget in
 * rows is to call the sink's maxLines procedure. So we need the Private
 * headers TextEditP, and therefore, XwP and IntrinsicP. There must be a
 * cleaner way
 */
#include <Xw/TextEdit.h>
#include <Xw/TextEditP.h>

#ifdef TYPEOUT
/*
 *  Typeout routines - The typeout widget is created and passed to the
 *  minibuf as an arg. Minibuf typeout is only enabled if the typeout
 *  widget is passed in. The typeout widget is a asciiString of some
 *  specific size, with a string of size rows*cols, enclosed in a popup
 *  shell. TOstart pops it up, sets a grab on it, and starts printing
 *  stuff to the string. When the string is full, (i.e. ready to
 *  scroll),or the typeout is done, it calls redisplay, and waits for a
 *  mouse click or keypress. So the typeout widget has a translation
 *  table of "<Key>: done()\n <BtnUp>: done()\n". done() actually does
 *  the paging, or exits.
 */

#define DEFAULT_FONT	"9x15"
#define DEFAULT_ROWS	24
#define DEFAULT_COLS	80

static Widget shell = NULL;
static Widget typeout = NULL;
static char *outString = NULL;
static int len = 0;
static char *cp = NULL;
static int notOver;
static Dimension internalMargin = 2;
static int nLines = 0;
static int nRows = 0;
static int nCols = 0;

static Arg text_args[16];
static char defaultTranslations[] = 
	"<Key>:		done()\n";

static void Done();
static XtActionsRec actionsList [] = {
	{"done", 	Done},
};

#define setarg(name, value)	(XtSetArg(text_args[i], name, value), i++)
#define fontwidth(x)	((x)->max_bounds.rbearing - (x)->min_bounds.lbearing)
#define fontheight(x)	((x)->max_bounds.ascent + (x)->max_bounds.descent)

/*
 * !! This is a lousy kludge - for starteds, it should use the normal widget
 * hierarchy mechanism to get the font, we should then find out the font,
 * compute the size and request a resize. Also, we should probbaly use a
 * TransientShell - except that the window managers don't do that right, mostly
 */
Widget CreateTypeout(parent, cols, rows, x, y, font)
Widget parent;
char *font;
{
	XFontStruct *finfo;
	int i, pixelwidth, pixelheight;
	XtTranslations defTranslations;
	
	if (shell) 
		return typeout;
	shell = XtCreateWidget("typeoutshell", overrideShellWidgetClass,
	 parent, (ArgList) NULL, (Cardinal) 0);
	if (!font) {
		finfo = XLoadQueryFont(XtDisplay(shell), DEFAULT_FONT);
	} else {
		finfo = XLoadQueryFont(XtDisplay(shell), font);
		if (finfo == NULL)
			finfo = XLoadQueryFont(XtDisplay(shell), DEFAULT_FONT);
	}
	if (finfo == NULL) {
		(void) fprintf(stderr, "Can't load default font \"%s\" - quitting\n",
		 DEFAULT_FONT);
		exit(-1);
	}
	if (cols == 0)
		cols = DEFAULT_COLS;
	if (rows == 0)
		rows = DEFAULT_ROWS;
	nCols = cols;
	pixelwidth = cols * fontwidth(finfo) + 2 * internalMargin;
	pixelheight = rows * fontheight(finfo) + 2 * internalMargin;
	XtAddActions(actionsList, XtNumber(actionsList));
	defTranslations = XtParseTranslationTable(defaultTranslations);
	len = cols *rows + 1;
	outString = XtMalloc((unsigned) len);
	bzero(outString, len);
	i = 0;
	setarg(XtNwidth, (Dimension) pixelwidth);
	setarg(XtNheight, (Dimension) pixelheight);
	setarg(XtNfont, finfo);
	setarg(XtNx, (Position) x);
	setarg(XtNy, (Position) y);
	setarg(XtNtranslations, defTranslations);
	setarg(XtNstring, outString);
	setarg(XtNlength, 0);
	setarg(XtNmaximumSize, len);
	setarg(XtNeditType, XwtextEdit);
	setarg(XtNleftMargin, internalMargin);
	setarg(XtNrightMargin, internalMargin);
	setarg(XtNtopMargin, internalMargin);
	setarg(XtNbottomMargin, internalMargin);
	typeout = XtCreateManagedWidget("typeout", XwtexteditWidgetClass,
	 shell, text_args, (Cardinal) i);
	return typeout;
}

/*ARGSUSED*/
static void Done(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	notOver = FALSE;
}

more(s)
char *s;
{
	XEvent ev;

	if (s) {
		(void) strcpy(cp, s);
		cp += strlen(s);
	}
	XwTextInsert((XwTextEditWidget) typeout, (unsigned char *) outString);
	notOver = TRUE;
	while(notOver) {
		XtNextEvent(&ev);
		XtDispatchEvent(&ev);
	}
}

reset()
{
	cp = outString;
	/*
	 * Extract number of rows in textEdit widget - there must be a better
	 * way!
	 */
	nRows = (*(((XwTextEditWidget)typeout)->text.sink->maxLines))(typeout);
	nLines = 1;
	bzero(outString, len);
}

xputs(s)
char *s;
{
	while(*s) {
		*cp++ = *s;
		if (*s == '\n') {
			nLines++;
			if (nLines == nRows) {
				more("Press any key to continue");
				reset();
				XwTextClearBuffer((XwTextEditWidget) typeout);
			}
		}
		s++;
	}
}
		
/* TOstart(s) char *s;
 *	Starts the typeout, and prints 's' as a title. Typeout is some 
 *	sort of overlay 'window' or something, for temporary output, 
 *	which can popup, and vanish after the user has read it. 
 */
TOstart(s)
char *s;
{
	reset();
	XwTextClearBuffer((XwTextEditWidget) typeout);
	XtPopup(shell, XtGrabNonexclusive);
	XtAddGrab(typeout, False, False);
	AcquireFocus(typeout);
	/* title */
	xputs(s);
	xputs("\n----------\n");
}


/* Typeout(fmt, args) char *fmt; va_dcl args;
 *	Is like printf() - prints args according to format 'fmt'. 
 *	Is a <varargs> routine. If fmt is NULL, it prints a newline.
 */
/*VARARGS1*/
Typeout(va_alist)
va_dcl
{
	va_list args;
	char *fmt;
	char buf[128];

	va_start(args);
	fmt = va_arg(args, char *);
	if (!fmt) {
		xputs("\n");
	} else {
		(void) vsprintf(buf, fmt, args);
		xputs(buf);
	}
	va_end(args);
}


/* TOstop()
 *	End of typeout - this of waits for a keypress.
 *	It should then clean up the typeout and return. It only returns.
 */
TOstop()
{
	more("---Press any key or click a button to quit----");
	ReleaseFocus(typeout);
	XtRemoveGrab(typeout);
	XtPopdown(shell);
	return;
}
#else TYPEOUT
/*ARGSUSED*/
Widget CreateTypeout(parent, cols, rows, x, y, font)
Widget parent;
char *font;
{
	return(NULL);
}

/*ARGSUSED*/
TOstart(s)
{
}

/*VARARGS0*/
/*ARGSUSED*/
Typeout(va_alist)
va_dcl
{
}

TOstop()
{
}
#endif TYPEOUT
/* Don't add anything after the #endif TYPEOUT */
