/* $Header: main.c,v 1.6 89/07/02 15:58:30 moraes Exp $ */
#include <signal.h>

#include "xpic.h"
#include "windows.h"
#include "spline.h"
#include "gels.h"
#include "tune.h"
#include "newfonts.h"
#include "assert.h"

#include <X11/Shell.h>
#include <X11/Xutil.h>


#include "xpic.icon"

#ifndef lint
static char rcsid[] = "$Header: main.c,v 1.6 89/07/02 15:58:30 moraes Exp $";
#endif

#define DUMPTEMPLATE	"%s/xpic%d"
static char dumpfile[sizeof(DUMPDIR) + sizeof(DUMPTEMPLATE) + 17];

int screenResolution;
char *lprinter;
char *lprcommand;

main(argc, argv)
int argc;
char **argv;
{
	extern char *getmyhomedir();
	int Dying(), X_Error(), X_IOError();
	Widget X_start();
	Widget toplevel;

	nullfile = "No file";
	argfile = NULL;
	inputWidget = 0;
	GelCounter = 0;
	homeDir = getmyhomedir();
	if ((flatVerts = InitSpline(&flatSize)) == NULL)
		error("Not enough memory for the rubber-band spline buffer");
	
	if ((flatVerts2 = InitSpline(&flatSize2)) == NULL)
		error("Not enough memory for the redraw spline buffer");

	toplevel = X_start(argc, argv);
	picDpy = XtDisplay(toplevel);
	
	XSetErrorHandler(X_Error);
	XSetIOErrorHandler(X_IOError);
	
	CreateWindows(toplevel);
	
	if (argfile == NULL) {
		if ((MainCell = NewCell("Main", nullfile)) == NULL) {
			error("Main: Can't create Main cell");
			/*NOTREACHED*/
		}
	} else {
		if ((MainCell = ReadCell(nullfile, argfile)) == NULL) {
			free(argfile);
			if ((MainCell = NewCell("Main", nullfile)) == NULL) {
				error("Main: Can't create Main cell");
				/*NOTREACHED*/
			}
		}
	}

	LastCell = CurrentCell = MainCell;

	(void) sprintf(dumpfile, DUMPTEMPLATE, DUMPDIR, getpid());
	/* We try to save the current cell in a temp. file before dying */
	if (signal(SIGHUP, SIG_IGN)==SIG_DFL)
		(void) signal(SIGHUP, Dying);
	if (signal(SIGINT, SIG_IGN)==SIG_DFL)
		(void) signal(SIGINT, Dying);
	if (signal(SIGQUIT, SIG_IGN)==SIG_DFL)
		(void) signal(SIGQUIT, Dying);
	/* If abort generates this signal instead of IOT, remove this handler */
	if (signal(SIGEMT, SIG_IGN)==SIG_DFL)
		(void) signal(SIGEMT, Dying);
	if (signal(SIGBUS, SIG_IGN)==SIG_DFL)
		(void) signal(SIGBUS, Dying);
	if (signal(SIGSEGV, SIG_IGN)==SIG_DFL)
		(void) signal(SIGSEGV, Dying);
	if (signal(SIGTERM, SIG_IGN)==SIG_DFL)
		(void) signal(SIGTERM, Dying);

	XtMainLoop();
	/*NOTREACHED*/
}


/* x MUST be a constant string */
#define whine(x)	(void) write(2, x, sizeof(x))

static int swansong = 0;

/* We try to save the current cell in a temp. file before dying */
Dying(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
	char buf[BUFSIZ];

	if (swansong)	/* we're here again. better quit fast */
		_exit(-1);
	swansong++;
	(void) sprintf(buf, "xpic received signal %d, code %d\n", sig, code);
	(void) write(2, buf, strlen(buf));
	if ((outFile = fopen(dumpfile, "w")) == NULL) {
		(void) sprintf(buf, "Can't open %s for dump\nDying.\n", dumpfile);
	} else {
		/*
		 * Don't want it to try malloc! In a bad memory corruption error, (of
		 * which I hope there aren't any!), the WriteGel can go into
		 * never-never land, if the pointers in the gelList are corrupted.
		 * Worth a try though...
		 */
		setbuf(outFile, buf);
		WriteGel(CurrentCell->gelList);
		(void) fclose(outFile);
		(void) sprintf(buf, "Dumped the current cell to %s\nNow dying.\n",
		 dumpfile);
	}
	(void) write(2, buf, strlen(buf));
	if (scp == NULL || sig == SIGSEGV || sig == SIGBUS || sig == SIGEMT
	 || sig == SIGILL)
		abort();
	else
		exit(-1);
	/* NOTREACHED */
}

/* Hell just froze over. */
/*ARGSUSED*/
X_IOError (dpy)
Display *dpy;
{
	perror("X Windows IO Error");
	Dying(0, 0, (struct sigcontext *) NULL);
	/*NOTREACHED*/
}


/*
 *  They say this isn't fatal - but if it happens, it's a serious
 *  problem, so we die.
 */
int X_Error(dpy, event)
Display *dpy;
XErrorEvent *event;
{
	char buffer[BUFSIZ];
	char mesg[BUFSIZ];
	char number[32];
	char *mtype = "XlibMessage";

	whine("Unthinkable - an xpic bug - Please note down the\n");
	whine("following message and tell the person responsible\n");
 	whine("for xpic here\n");

	XGetErrorText(dpy, (int) event->error_code, buffer, BUFSIZ);
	XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
	(void) fprintf(stderr, "%s: %s\n  ", mesg, buffer);
	XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code %d", 
	mesg, BUFSIZ);
	(void) fprintf(stderr, mesg, event->request_code);
	(void) sprintf(number, "%d", event->request_code);
	XGetErrorDatabaseText(dpy, "XRequest", number, "", 	buffer, BUFSIZ);
	(void) fprintf(stderr, " %s", buffer);
	fputs("\n  ", stderr);
	XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Request Minor code", 
	mesg, BUFSIZ);
	(void) fprintf(stderr, mesg, event->minor_code);
	fputs("\n  ", stderr);
	XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
	mesg, BUFSIZ);
	(void) fprintf(stderr, mesg, event->resourceid);
	fputs("\n  ", stderr);
	XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Error Serial #%d", 
	mesg, BUFSIZ);
	(void) fprintf(stderr, mesg, event->serial);
	fputs("\n  ", stderr);
	XGetErrorDatabaseText(dpy, mtype, "CurrentSerial", "Current Serial #%d",
	mesg, BUFSIZ);
	(void) fprintf(stderr, mesg, dpy->request);
	fputs("\n  ", stderr);
	if (event->error_code == BadImplementation) return 0;
	exit(1);
	Dying(0, 0, (struct sigcontext *) NULL);
	/*NOTREACHED*/
}


/* Defaults */
static Boolean defaultFALSE = FALSE;
static Boolean defaultTRUE	 = TRUE;
static int defaultWidth = DEFAULTPAGEWIDTH;
static int defaultHeight = DEFAULTPAGEHEIGHT;
static int defaultSpacing = DEFAULTSPACING;
static char *iconGeometry;
static char *iconPixmap;
static int rotate;

/* Application Resources - no particular widget */
static XtResource application_resources[] = {
	{"name", "Name", XtRString, sizeof(char *),
		(Cardinal)&progname, XtRString, "xpic"},
#ifndef FIXED_TOOLKIT
	{"iconGeometry", "IconGeometry", XtRString, sizeof(char *),
		(Cardinal)&iconGeometry, XtRString, (caddr_t) NULL},
	{"iconPixmap", "IconPixmap", XtRString, sizeof(char *),
		(Cardinal)&iconPixmap, XtRString, (caddr_t) NULL},
#endif
	{"pageWidth", "PageWidth", XtRInt, sizeof(int),
		(Cardinal)&pageWidth, XtRInt, (caddr_t) &defaultWidth},
	{"pageHeight", "PageHeight", XtRInt, sizeof(int),
		(Cardinal)&pageHeight, XtRInt, (caddr_t) &defaultHeight},
	{"gridSpacing", "GridSpacing", XtRInt, sizeof(int),
		(Cardinal)&gridSpacing, XtRInt, (caddr_t) &defaultSpacing},
	{"gridOn", "GridOn", XtRBoolean, sizeof(Boolean),
		(Cardinal)&gridOn, XtRBoolean, (caddr_t) &defaultTRUE},
	{"gridColor", "GridColor", XtRPixel, sizeof(Pixel),
		(Cardinal)&gridColor, XtRString, (caddr_t) "Black"},
	{"cursorColor", "CursorColor", XtRPixel, sizeof(Pixel),
		(Cardinal)&cursorColor, XtRString, (caddr_t) "Black"},
	{"highlight", "Highlight", XtRPixel, sizeof(Pixel),
		(Cardinal)&highlight, XtRString, (caddr_t) "Black"},
	{"rotate", "Rotate", XtRBoolean, sizeof(Boolean),
		(Cardinal)&rotate, XtRBoolean, (caddr_t) &defaultFALSE},
	{"backup", "Backup", XtRBoolean, sizeof(Boolean),
		(Cardinal)&backupOnWrite, XtRBoolean, (caddr_t) &defaultTRUE},
	{"printer", "Printer", XtRString, sizeof(char *),
		(Cardinal)&lprinter, XtRString, (caddr_t) "PostScript"},
	{"lprcommand", "LprCommand", XtRString, sizeof(char *),
		(Cardinal)&lprcommand, XtRString, "x2ps -w | lpr -P%s"},
	{"font", "Font", XtRFontStruct, sizeof(char *),
		(Cardinal)&defaultFont, XtRString, DEFAULT_FONT},
};

/*
 *  Command line options table. The command line is parsed for these,
 *  and it sets/overrides the appropriate values in the resource
 *  database
 */
static XrmOptionDescRec optionDescList[] = {
#ifndef FIXED_TOOLKIT
{"-igeometry",	"*iconGeometry",	XrmoptionSepArg,	(caddr_t) NULL},
{"-ipixmap", 	"*iconPixmap",		XrmoptionSepArg, 	(caddr_t) NULL},
#endif
{"-pw",			"*pageWidth", 		XrmoptionSepArg, 	(caddr_t) NULL},
{"-ph",			"*pageHeight", 		XrmoptionSepArg, 	(caddr_t) NULL},
{"-gs",			"*gridSpacing",		XrmoptionSepArg,	(caddr_t) NULL},
{"-gc",			"*gridColor",		XrmoptionSepArg,	(caddr_t) NULL},
{"-cr",			"*cursorColor",		XrmoptionSepArg,	(caddr_t) NULL},
{"-ms",			"*cursorColor",		XrmoptionSepArg,	(caddr_t) NULL},
{"-hl",			"*highlight",		XrmoptionSepArg,	(caddr_t) NULL},
{"-grid",		"*gridOn",			XrmoptionNoArg,		(caddr_t) "off"},
{"+grid",		"*gridOn",			XrmoptionNoArg,		(caddr_t) "on"},
{"-rotate",		"*rotate",			XrmoptionNoArg,		(caddr_t) "on"},
{"+rotate",		"*rotate",			XrmoptionNoArg,		(caddr_t) "off"},
{"-backup",		"*backup",			XrmoptionNoArg,		(caddr_t) "off"},
{"+backup",		"*backup",			XrmoptionNoArg,		(caddr_t) "on"},
{"-P",			"*printer",			XrmoptionStickyArg, (caddr_t) NULL},
{"-font",		".font",			XrmoptionSepArg,	(caddr_t) NULL},
{"-fn",			".font",			XrmoptionSepArg,	(caddr_t) NULL},
};


static Arg ourTopLevelArgs[] = {
	{ XtNallowShellResize, (XtArgVal) TRUE },
	{ XtNinput, (XtArgVal) TRUE },
};

Widget X_start(argc, argv)
int argc;
char **argv;
{
	int	i;
	Widget toplevel;
	char *lp;
	extern char *getenv();

	/*
	 * We do some silly stuff to ensure the -P option overrides the
	 * environment variable PRINTER which overrides the application resource
	 * [pP]rinter which overrides the default "PostScript"
	 */
	lp = getenv("PRINTER");

	toplevel = XtInitialize("xpic", "XPic", 
	 optionDescList, XtNumber(optionDescList), (Cardinal *) &argc, argv);

	if (lprinter != NULL)
		lp = lprinter;
		
	XtGetApplicationResources(toplevel, (caddr_t) 0, application_resources,
	 XtNumber(application_resources), (ArgList) NULL, (Cardinal) 0 );

	if (lp != NULL)
		lprinter = lp;
		
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			/* !! Yechh! We don't do multiple files yet - one day... */
			argfile = XtMalloc((unsigned) (strlen(argv[i]) + 1));
			(void) strcpy(argfile, argv[i]);
		}
		else	
			usage(progname, argv[i]);
	}
	if (rotate) {
		int temp = pageHeight;
		pageHeight = pageWidth;
		pageWidth = temp;
	}

	screenResolution = 254 *
	 HeightOfScreen(XtScreen(toplevel)) / HeightMMOfScreen(XtScreen(toplevel));
	screenResolution /= 10;
#ifdef DEBUG
	printf("screen resolution = %d\n", screenResolution);
#endif
	/*
	 *  !! Something for reverse video since the toolkit doesn't
	 *  seem to do much.  Also check that the colors (foreground,
	 *  background, highlight, cursorColor, gridColor are correct
	 */

	/* Something with icon geometry */
	if (iconGeometry != NULL) {
		int scr, junk1, junk2, bitmask;
		Arg args[2];

		for(scr = 0;	/* yyuucchh */
		 XtScreen(toplevel) != ScreenOfDisplay(XtDisplay(toplevel),scr);
		 scr++);

		args[0].name = XtNiconX;
		args[1].name = XtNiconY;
		bitmask = XGeometry(XtDisplay(toplevel), scr, iconGeometry, "",
		 0, 0, 0, 0, 0, (int *) &args[0].value, (int *) &args[1].value,
		 &junk1, &junk2);
		if (bitmask & (XValue | YValue)) /* No way to communicate Negative */
			XtSetValues( toplevel, args, 2);
	}
	
	if (iconPixmap != NULL) {
		int icon_width, icon_height, xhot, yhot;
		Pixmap icon_pixmap;
		Arg args[1];

		if (XReadBitmapFile(XtDisplay(toplevel), 
		 RootWindowOfScreen(XtScreen(toplevel)), iconPixmap,
		 (unsigned *) &icon_width, (unsigned *) &icon_height,
		 &icon_pixmap, &xhot, &yhot)
		 == BitmapSuccess) {
			args[0].name = XtNiconPixmap;
			args[0].value = (XtArgVal) icon_pixmap;
			XtSetValues(toplevel, args, 1);
		} else
			(void) fprintf(stderr, "xpic: can't find pixmap %s\n", iconPixmap);
	} else {
		Arg args[1];
		Pixmap icon_pixmap = XCreateBitmapFromData(XtDisplay(toplevel),
		 RootWindowOfScreen(XtScreen(toplevel)), xpic_bits, 
		 xpic_width, xpic_height);

		args[0].name = XtNiconPixmap;
		args[0].value = (XtArgVal) icon_pixmap;
		XtSetValues(toplevel, args, 1);
	}
	XtSetValues(toplevel, ourTopLevelArgs, XtNumber(ourTopLevelArgs));
	
	return(toplevel);
}


usage(prog, err)
char *prog;
char *err;
{
	if (err)
		(void) fprintf(stderr, "%s: Bad option: \"%s\"\n", prog, err);
	(void) fprintf(stderr, "\
Usage:\n\
%s [standard Toolkit options] [xpic options] ..... file ... \n\
\n\
The list of xpic options is shown below - together with the corresponding\n\
resource name. The option overrides the resource if both are specified\n\
The defaults are also shown.\n\
\n\
-grid,   gridOn \"off\"   Turns grid off       (On)\n\
+grid,   gridOn \"on\"    Turns grid on        (On)\n\
-gs,     gridSpacing    Grid spacing in pixels (8)\n\
-gc,     gridColor      Grid color             (Black)\n\
-ph,     pageHeight     PageHeight in 10*in.   (110)\n\
-pw,     pageWidth      PageWidth in 10*in.    (85)\n\
-rotate, rotate \"on\"    swap page height and width\n\
+rotate, rotate \"off\"   normal page height and width\n\
-backup, backup \"off\"   No backup file on write(On)\n\
+backup, backup \"on\"    Make backup file on write(On)\n\
-cr,     cursorColor    Sets cursor color      (Black)\n\
-ms,     cursorColor    Sets cursor color      (Black)\n\
-hl,     highlight      Sets highlight color   (Black)\n\
-fn,     font           default font           (%s)\n\
-font,   font           default font           (%s)\n\
-P name  printer        Sets printer name      (PostScript)\n",
	 prog, DEFAULT_FONT, DEFAULT_FONT);
	exit(1);
}
