/* $Header: Minibuf.c,v 1.2 89/07/06 18:13:58 xwindows Exp $ */
/*
 *  This file should be edited with 4-column tabs!  (:set ts=4 sw=4 in
 *  vi, set internal-tabstop 4 in Jove, and use edit-tab-stops in Emacs
 *  to get them
 */

/* 
 *  This set of routines create and manipulate a simple one-line input
 *  window. They offer a reasonable subset of EMACS like editing
 *  capabilities, Like EMACS, the command-key to action bindings can be
 *  altered. It does no data validation. The minibuffer can be created
 *  using normal Xt calls, with args, and parsing the resources. Other
 *  calls allow strings to be displayed or input or both. (prompted
 *  input). There is no restriction on the length of the input line,
 *  which will horizontally scroll in the window, EMACS-style, by half
 *  the "screen" width.  It can be used in dialog boxes, or as in xpic,
 *  for an interaction line.
 */
/* 
 *  Since this is the first (and only) widget I've written, there are
 *  probably lots of things done wrong. Feel free to fix 'em. This was
 *  written because the Text widgets available are more powerful than I
 *  need, but aren't capable of performing the simple function that this
 *  performs - display a prompt, a default chunk of text, and permit
 *  editing of the default. MOST important, it must invoke some sort of
 *  callback when the user hits RETURN or ^G/^C. As a minor point, I
 *  hate the silly underline ^ cursor that the text widget allows. Give
 *  me a solid blob anyday. It would be nice if someone could hack this
 *  capability into the Text widget - after a bit of digging in its
 *  code, I decided that preserving my sanity (or what's left of it) was
 *  important, and chose to write this - it was much simpler.
 */
/* Author: Mark Moraes (moraes@csri.toronto.edu)
   History: First written for X10 under the X10 tookit, for xpic in
   			August 1987.
   			This version was ported to X11, and then tuned and cleaned
			up - X11 is so much slower on Suns that the inefficient 
			redisplay strategy used in the X10 version was unfeasible
			June 1988.
 */
/* To do:
	Get it to understand the subtleties of reverse video 
 */

#include <strings.h>
#include <ctype.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/keysymdef.h>
#include <X11/CoreP.h>
#include <X11/Core.h>
#include "MinibufP.h"
#include "Minibuf.h"

#include "assert.h"

/* To keep references to data less than six miles long */
#define mbuf(x)			buf->minibuf.x
#define mcore(x)		buf->core.x
#define fontwidth(x)	((x)->max_bounds.width)
#define fontheight(x)	((x)->max_bounds.ascent + (x)->max_bounds.descent)
#define fontbaseline(x)	((x)->max_bounds.ascent)
#define CheckMode()		if (mbuf(inputMode) == FALSE) return; else ;


/*
 *  This should be larger than the expected filesize - if a XtRealloc
 *  occurs in insert_s(), add_mess(), bad things will happen because the
 *  f_complete routines don't expect the string to change while they're
 *  working - fixing this is work that I don't feel upto right now - one
 *  day....
 */
#define BUF_BLOCK 		128

#define MIN_COLS 		5
#define DEFAULTFONT		"8x13"
#define ABORT_MSG		"[Aborted]"
#define DELAY			300000		/*
									 *  Microseconds of delay for cursor
									 *  flash in SetMark
									 */

/* The default bindings */
static char defaultTranslations[] = 
	"Ctrl<Key>F:		forward-character()\n\
	<Key>0xff53:		forward-character()\n\
	Ctrl<Key>B:			backward-character()\n\
	<Key>0xff51:		backward-character()\n\
	Ctrl<Key>A:			beginning-of-line()\n\
	Ctrl<Key>E:			end-of-line()\n\
	Ctrl<Key>U:			universal-argument()\n\
	Ctrl<Key>D:			delete-next-character()\n\
	Ctrl<Key>H:			delete-previous-character()\n\
	<Key>0xff7f:		delete-previous-character()\n\
	<Key>0xffff:		delete-previous-character()\n\
	<Key>0xff08:		delete-previous-character()\n\
	Ctrl<Key>X:			exchange-point-and-mark()\n\
	Ctrl<Key>W:			kill-region()\n\
	Ctrl<Key>K:			kill-to-end-of-line()\n\
	Meta<Key>D:			kill-to-beginning-of-line()\n\
	Ctrl<Key>Y:			yank-killed-text()\n\
	Ctrl<Key>J:			newline()\n\
	<Key>0xff0a:		newline()\n\
	Ctrl<Key>M:			newline()\n\
	<Key>0xff0d:		newline()\n\
	Ctrl<Key>G:			abort()\n\
	Ctrl<Key>C:			abort()\n\
	<Btn1Down>:			set-cursor-to-mouse()\n\
	<Btn2Down>:			get-x-buffer()\n\
	<Btn3Down>:			set-mark-to-mouse()\n\
	Ctrl<Key>0x20:		make-this-the-mark()\n\
	<Key>0x20:			complete-filename()\n\
	Ctrl<Key>I:			complete-filename()\n\
	<Key>0xff09:		complete-filename()\n\
	Shift<Key>/:		list-files()\n\
	<Key>/:				insert-char()\n\
	<Key>?:				list-files()\n\
	<Key>:				insert-char()\n\
";


static void CursorForward(), CursorBack(), BeginningOfBuf(), EndOfBuf(),
 UnivArgument(), DeleteCharForward(), DeleteCharBack(), KillToEnd(),
 KillToBeginning(), YankKilledStuff(), MakeMark(), ExchangeMarkAndPoint(),
 CutMarkToPoint(), FinishedInput(), QuitInput(), SetCursor(), SetMark(),
 GetXBuffer(), CharInsert(), CompleteFilename(), ListFiles();

/* Actions Table */
static XtActionsRec actionsList [] = {
/* motion bindings */
	{"forward-character", 			CursorForward},
	{"backward-character", 			CursorBack},
	{"beginning-of-line", 			BeginningOfBuf},
	{"end-of-line", 				EndOfBuf},
	{"universal-argument",			UnivArgument},
/* delete bindings */
	{"delete-next-character", 		DeleteCharForward},
	{"delete-previous-character",	DeleteCharBack},
/* kill bindings */
	{"kill-to-end-of-line", 		KillToEnd},
	{"kill-to-beginning-of-line", 	KillToBeginning},
/* yank bindings */
	{"yank-killed-text", 			YankKilledStuff},
	{"make-this-the-mark",			MakeMark},
	{"exchange-point-and-mark",		ExchangeMarkAndPoint},
	{"kill-region",					CutMarkToPoint},
/* new line stuff */
	{"newline", 					FinishedInput},
	{"abort",						QuitInput},
/* Selection stuff */
	{"set-cursor-to-mouse",			SetCursor},
	{"set-mark-to-mouse",			SetMark},
	{"get-x-buffer",				GetXBuffer},
/* filename completion and listing of files */
	{"complete-filename",			CompleteFilename},
	{"list-files", 					ListFiles},
/* Insert character */
	{"insert-char",					CharInsert},
};

static int zero = 0;

#define offset(field) XtOffset(MinibufWidget, field)
static XtResource resources[] = {
	{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	 offset(minibuf.foreground), XtRString, "Black"},
	{XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	 offset(minibuf.finfo),XtRString, DEFAULTFONT},
	{XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
	 offset(minibuf.cursor), XtRString, "xterm"},
	{XtNfinishedCallback, XtCCallback, XtRCallback, sizeof(XtProc),
	 offset(minibuf.finishedCallback), XtRCallback, (caddr_t)NULL},
#ifdef TYPEOUT
	{XtNtypeoutColumns, XtCTypeoutColumns, XtRInt, sizeof(int),
	 offset(minibuf.typeoutColumns), XtRInt, (caddr_t)&zero},
	{XtNtypeout, XtCtypeout, XtRPointer, sizeof(Widget),
	 offset(minibuf.typeout), XtRPointer, (caddr_t)NULL},
#endif
};

static void Initialize(), Realize(), Destroy(), Redisplay(), Resize();

/*
 *  ...ClassData must be initialized at compile time.  Must initialize
 *  all substructures.  (Actually, last two here need not be initialized
 *  since not used.)
 */
MinibufClassRec minibufClassRec = {
	{
		(WidgetClass) &widgetClassRec,	/* superclass			*/	
		"Minibuf",						/* class_name			*/
		sizeof(MinibufRec),				/* size					*/
		NULL,							/* class_initialize		*/
		NULL,							/* class_part_initialize*/
		FALSE,							/* class_inited		  	*/
		Initialize,						/* initialize			*/
		NULL,							/* initialize_hook		*/
		Realize,						/* realize		 		*/
		actionsList,					/* actions		  		*/
		XtNumber(actionsList),			/* num_actions		  	*/
		resources,						/* resources		  	*/
		XtNumber(resources),			/* resource_count	  	*/
		NULLQUARK,						/* xrm_class		  	*/
		TRUE,							/* compress_motion	  	*/
		TRUE,							/* compress_exposure	*/
		TRUE,							/* compress_enterleave  */
		FALSE,							/* visible_interest	  	*/
		Destroy,						/* destroy		  		*/
		Resize,							/* resize		  		*/
		Redisplay,						/* expose		  		*/
		NULL,							/* set_values		  	*/
		NULL,							/* set_values_hook	  	*/
		XtInheritSetValuesAlmost,		/* set_values_almost	*/
		NULL,							/* get_values_hook	  	*/
		NULL,							/* accept_focus		  	*/
		XtVersion,						/* version		  		*/
		NULL,							/* callback_private	  	*/
		defaultTranslations,			/* tm_table		  		*/
		NULL,							/* query_geometry	  	*/
#ifdef XtCAccelerators
		XtInheritDisplayAccelerator,	/* display_accelerator	*/
		NULL							/* extension		*/
#endif /* XtCAccelerators */
	},  /* CoreClass fields initialization */
	{
		0,								/* field not used    	*/
	},  /* MinibufClass fields initialization */
};

/* for public consumption */
WidgetClass minibufWidgetClass = (WidgetClass)&minibufClassRec;

/*ARGSUSED*/
static void Initialize(request, new)
Widget request, new;
{
	register MinibufWidget buf = (MinibufWidget) new;
	XGCValues gcv;

	mbuf(string) = mbuf(killBuffer) = NULL;
	mbuf(size) = mbuf(startPos) = mbuf(cursorPos) = mbuf(mark)
	 = mbuf(killBufferLen) = mbuf(killBufferSize) = mbuf(len) 
	 = mbuf(cols) = mbuf(promptLen) = 0;
	mbuf(arg) = 1;
	mbuf(cursorX) = 0;
	mbuf(inputMode) = FALSE;
	gcv.foreground = mbuf(foreground);
	gcv.background = mcore(background_pixel);
	gcv.font = mbuf(finfo)->fid;
	mbuf(normal_gc) = XtGetGC(new, GCForeground | GCBackground | GCFont,
	 &gcv);
	gcv.foreground = mbuf(foreground) ^ mcore(background_pixel);
	gcv.function = GXxor;
	mbuf(invert_gc) = XtGetGC(new, GCForeground | GCBackground | GCFont |
	 GCFunction, &gcv);

	/* Compute these correctly */
	if (mcore(height) == 0)
		mcore(height) = fontheight(mbuf(finfo));

	if ((mbuf(cols) = mcore(width) / fontwidth(mbuf(finfo))) < MIN_COLS) {
		/* Set it to MIN_COLS */
		mcore(width) = MIN_COLS * fontwidth(mbuf(finfo));
	}
}

/* Wish the primitive widget had a cursor .. */
static void Realize(w, valueMask, attributes)
register Widget w;
Mask *valueMask;
XSetWindowAttributes *attributes;
{
	if ((attributes->cursor = ((MinibufWidget)w)->minibuf.cursor) != None)
	 *valueMask |= CWCursor;

	XtCreateWindow( w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
	 *valueMask, attributes );
}

static void Destroy(w)
Widget w;
{
	register MinibufWidget buf = (MinibufWidget) w;

	if (mbuf(string) != NULL)
		free(mbuf(string));
	if (mbuf(killBuffer) != NULL)
		free(mbuf(killBuffer));
	XtDestroyGC(mbuf(normal_gc));
	XtDestroyGC(mbuf(invert_gc));
}


/*ARGSUSED*/
static void Redisplay(w, event, region)
Widget w;
XEvent *event;
Region region;
{
	register MinibufWidget buf = (MinibufWidget) w;
	static void DisplayBuf(), DisplayCursor();

	if (!XtIsRealized(w))
		return;
	DisplayBuf(buf, TRUE);
	if (mbuf(cursorOn) == TRUE)
		DisplayCursor(buf);
}


static void Resize(w)
Widget w;
{
	register MinibufWidget buf = (MinibufWidget) w;
	static void DisplayBuf(), DisplayCursor();

	/*
	 *  We should probably do some geometry management stuff here, if
	 *  the number of cols is less than MIN_COLS
	 */
	mbuf(cols) = mcore(width) / fontwidth(mbuf(finfo));
	if (!XtIsRealized(w))
		return;
	DisplayBuf(buf, TRUE);
	if (mbuf(cursorOn) == TRUE)
		DisplayCursor(buf);
}

/* 
 *  Displays the buffer. When called with redraw = TRUE, it clears the
 *  entire buffer and redraws it. If redraw = FALSE, then it clears from
 *  the cursor position to the end of the buffer, and then redraws just
 *  that section. This allows for fast update when a single character is
 *  typed, or deleted, usually at the end of the line. If redraw is
 *  FALSE, teh cursor must be turned off before this is called -
 *  otherwise it may or may not vanish, causing problems. Typically, we
 *  hide the cursor, then update the buffer string and data, and then
 *  call this procedure. Note that for delete-char-forward, char-insert,
 *  and StrInsert, this gets called AFTER the mbuf(string) is updated
 *  but before the mbuf(cursorPos) is updated, which in delete-char-back
 *  and cut-point-to-mark, it is called afetr the cursorPos is updated
 *  as well, so that the changed region is redrawn.
 */
static void DisplayBuf(buf, redraw)
register MinibufWidget buf;
int redraw;
{
	register int nChars, isLonger;
	int start;
	register Widget w = (Widget) buf;

	if (redraw) {
		start = mbuf(startPos);
		XClearWindow(XtDisplay(w), XtWindow(w));
	} else {
		start = mbuf(cursorPos);
		mbuf(cursorX) = (mbuf(cursorPos) - mbuf(startPos)) * 
		 fontwidth(mbuf(finfo));
		XClearArea(XtDisplay(w), XtWindow(w), mbuf(cursorX), 0, 
		 mcore(width) - mbuf(cursorX), (unsigned) fontheight(mbuf(finfo)), 
		 False);
	}
	nChars = (mbuf(startPos) + mbuf(cols));
	isLonger = (nChars < mbuf(len));
	if (isLonger)
		nChars -= start + 1;
	else
		nChars = mbuf(len) - start;

	XDrawImageString(XtDisplay(w), XtWindow(w), mbuf(normal_gc), 
	 redraw? 0 : mbuf(cursorX), fontbaseline(mbuf(finfo)), 
	 mbuf(string) + start, nChars);

	if (isLonger) 
		XDrawImageString(XtDisplay(w), XtWindow(w), mbuf(normal_gc), 
		 (mbuf(cols) - 1) * fontwidth(mbuf(finfo)), 
		 fontbaseline(mbuf(finfo)), "!", 1); 
}


/* 
 *  Display the buffer cursor. It does the devious computations for
 *  Horiz scrolling of the minibuffer, back and forward, and does a
 *  'hard' redisplay (i.e. with a clear) of the window if a horiz.
 *  scroll is done. 
 */
static void DisplayCursor(buf)
register MinibufWidget buf;
{
	register int col;
	register Widget w = (Widget) buf;

	if (mbuf(cursorPos) > mbuf(len))
		mbuf(cursorPos) = mbuf(len);
	if (mbuf(cursorPos) < mbuf(promptLen))
		mbuf(cursorPos) = mbuf(promptLen);
		
	col = (mbuf(cursorPos) - mbuf(startPos));

	if ((col >= mbuf(cols)) || (col < 0)) {
		if (mbuf(cursorPos) < mbuf(cols) - 1) {
			/*
			 *  The cursor will be seen if the start
			 *  position is 0, which looks nicer than half
			 *  the prompt showing
			 */
			mbuf(startPos) = 0;
			col = mbuf(cursorPos);
		} else {
			/* 
			 *  Horiz scroll the buffer to mbuf(cols)/2
			 *  before cursorPos. this acounts for both
			 *  forward and backward scrolling.
			 */
			col = (mbuf(cols)) / 2 - 1;
			mbuf(startPos) = mbuf(cursorPos) - col;
			if (mbuf(startPos) < 0) {
				mbuf(startPos) = 0;
				col = (mbuf(cursorPos) - mbuf(startPos));
			}
		}
		DisplayBuf(buf, TRUE);
	}
	mbuf(cursorX) = col * fontwidth(mbuf(finfo));
	XFillRectangle(XtDisplay(w), XtWindow(w), mbuf(invert_gc), 
	 mbuf(cursorX), 0, (unsigned) fontwidth(mbuf(finfo)),
	 (unsigned) fontheight(mbuf(finfo)));
	mbuf(cursorOn) = TRUE;
}



/*
 *  Hide the cursor
 */
static void ConcealCursor(buf)
register MinibufWidget buf;
{
	register Widget w = (Widget) buf;
	if (mbuf(cursorOn) == TRUE) {
		XFillRectangle(XtDisplay(w), XtWindow(w), mbuf(invert_gc), 
		 mbuf(cursorX), 0, (unsigned) fontwidth(mbuf(finfo)),
		 (unsigned) fontheight(mbuf(finfo)));
		mbuf(cursorOn) = FALSE;
	}
}



/* 
 *  Inserts len characters of string s into the buffer at the current
 *  cursor position. Note that it does not re-display the cursor, while
 *  CharInsert does.
 */
/* The universal argument has no effect on this one */
static void StrInsert(s, len, buf)
register char *s;
register int len;
register MinibufWidget buf;
{
	register int newLen = mbuf(len) + len;
	static void QuitInput();

	while (newLen > mbuf(size)) {
		mbuf(size) += BUF_BLOCK;
		if (mbuf(string) == NULL)
			mbuf(string) = XtMalloc((unsigned) mbuf(size));
		else 
			mbuf(string) = XtRealloc(mbuf(string), (unsigned) mbuf(size));
		if ( mbuf(string) == NULL) {
			mbuf(size) = 0;
			mbuf(len) = 0;
			mbuf(inputMode) = FALSE;
			QuitInput((Widget) buf, (XEvent *) NULL, (String *) NULL, 
			 (Cardinal *) NULL);
			return;
		}
	}
	bcopy(mbuf(string) + mbuf(cursorPos), mbuf(string) + mbuf(cursorPos) + len, 
	 mbuf(len) - mbuf(cursorPos));
	bcopy(s, mbuf(string) + mbuf(cursorPos), len);
	mbuf(len) += len;
	DisplayBuf(buf, FALSE);
	mbuf(cursorPos) += len;
	mbuf(arg) = 1;
}



/*
 *  Inserts c arg times into buffer string at current position and moves
 *  cursor forward
 */
/*ARGSUSED*/
static void CharInsert(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int i = mbuf(cursorPos);
	register int j = mbuf(arg);
	static void QuitInput();
	KeySym keysym;
	XComposeStatus compose;
	int status;
	char c;
	
	CheckMode();
	status = XLookupString(&event->xkey, &c, 1, &keysym, &compose);
	if (!status || !isascii(c) || !isprint(c))
		return;
	ConcealCursor(buf);
	while (mbuf(size) < mbuf(len) + mbuf(arg)) { 	/* Need more space */
		mbuf(size) += BUF_BLOCK;
		if (mbuf(string) == NULL)
			mbuf(string) = XtMalloc((unsigned) mbuf(size));
		else 
			mbuf(string) = XtRealloc(mbuf(string), (unsigned) mbuf(size));
		if (mbuf(string) == NULL) {
			mbuf(size) = 0;
			mbuf(len) = 0;
			mbuf(inputMode) = FALSE;
			QuitInput(w, event, params, numparams);
			return;
		}
	}

	bcopy(mbuf(string) + i, mbuf(string) + i + mbuf(arg),
	 mbuf(len) - i);
	mbuf(len) += mbuf(arg);
	for (; j > 0; j--,i++) {
		mbuf(string)[i] = c;
	}
	DisplayBuf(buf, FALSE);
	mbuf(cursorPos) += mbuf(arg);
	mbuf(arg) = 1;
	DisplayCursor(buf);
}


/*ARGSUSED*/
static void CursorBack(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int i;
	
	CheckMode();
	if ((i = (mbuf(cursorPos) - mbuf(arg))) < mbuf(promptLen))
		i = mbuf(promptLen);

	ConcealCursor(buf);
	mbuf(cursorPos) = i;
	mbuf(arg) = 1;
	DisplayCursor(buf);
}


/*ARGSUSED*/
static void CursorForward(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int i;
	
	CheckMode();
	if ((i = (mbuf(cursorPos) + mbuf(arg))) > mbuf(len))
		i = mbuf(len);

	ConcealCursor(buf);
	mbuf(cursorPos) = i;
	mbuf(arg) = 1;
	DisplayCursor(buf);
}


/*ARGSUSED*/
static void SetCursor(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	Window foo1, foo2;
	int foo3, foo4;
	unsigned int foo5;
	int x, y;
	register int mCol;

	CheckMode();
	(void) XQueryPointer(XtDisplay(w), XtWindow(w),
	 &foo1, &foo2, &foo3, &foo4, &x, &y, &foo5);
	mCol = x / fontwidth(mbuf(finfo));
	ConcealCursor(buf);
	mbuf(cursorPos) = mbuf(startPos) + mCol;
	mbuf(arg) = 1;
	DisplayCursor(buf);
}

/* 
 *  Tries to flash the cursor to the mark, and then back to the cursor
 *  position. This will only be useful when bound to the mouse buttons.
 */
/*ARGSUSED*/
static void SetMark(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int tmp;
	Window foo1, foo2;
	int foo3, foo4;
	unsigned int foo5;
	int x, y;
	register int mCol;
	int len;

	CheckMode();
	tmp = mbuf(cursorPos);
	(void) XQueryPointer(XtDisplay(w), XtWindow(w),
	 &foo1, &foo2, &foo3, &foo4, &x, &y, &foo5);
	mCol = x / fontwidth(mbuf(finfo));
	ConcealCursor(buf);
	mbuf(cursorPos) = mbuf(mark) = mbuf(startPos) + mCol;
	DisplayCursor(buf);
	XFlush(XtDisplay(w));
	usleep(DELAY);
	ConcealCursor(buf);
	mbuf(cursorPos) = tmp;
	if (mbuf(cursorPos) > mbuf(mark)) {
		len = mbuf(cursorPos) - mbuf(mark);
		tmp = mbuf(mark);
	} else {
		len = mbuf(mark) - mbuf(cursorPos);
		tmp = mbuf(cursorPos);
	}
	XSetSelectionOwner(XtDisplay(w), XA_PRIMARY, None, CurrentTime);
	XStoreBytes(XtDisplay(w), mbuf(string) + tmp, len);
	DisplayCursor(buf);
	mbuf(arg) = 1;
}



/* The universal argument has no effect on this one */
/*ARGSUSED*/
static void BeginningOfBuf(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	CheckMode();
	ConcealCursor(buf);
	mbuf(cursorPos) = mbuf(promptLen);
	DisplayCursor(buf);
}



/* The universal argument has no effect on this one */
/*ARGSUSED*/
static void EndOfBuf(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;

	CheckMode();
	ConcealCursor(buf);
	mbuf(cursorPos) = mbuf(len);
	DisplayCursor(buf);
}



/*ARGSUSED*/
static void DeleteCharForward(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int i;

	CheckMode();
	if ((i = (mbuf(cursorPos) + mbuf(arg))) > mbuf(len))
		i = mbuf(len);

	ConcealCursor(buf);
	bcopy(mbuf(string) + i, mbuf(string) + mbuf(cursorPos), mbuf(len) - i);
	mbuf(len) -= i - mbuf(cursorPos);
	mbuf(arg) = 1;
	DisplayBuf(buf, FALSE);
	DisplayCursor(buf);
}


/*ARGSUSED*/
static void DeleteCharBack(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int i;

	CheckMode();
	if ((i = (mbuf(cursorPos) - mbuf(arg))) < mbuf(promptLen))
		i = mbuf(promptLen);

	ConcealCursor(buf);
	bcopy(mbuf(string) + mbuf(cursorPos), mbuf(string) + i, mbuf(len) - i);
	mbuf(len) += i - mbuf(cursorPos);
	mbuf(cursorPos) = i;
	mbuf(arg) = 1;
	DisplayBuf(buf, FALSE);
	DisplayCursor(buf);
}




/* The universal argument has no effect on this one */
/* Like Setmark, this flashes the cursor */
/*ARGSUSED*/
static void MakeMark(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;

	CheckMode();
	mbuf(mark) = mbuf(cursorPos);
	mbuf(arg) = 1;
}



/*ARGSUSED*/
static void ExchangeMarkAndPoint(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int tmp = mbuf(cursorPos);

	CheckMode();
	if (mbuf(mark) < mbuf(promptLen))
		mbuf(mark) = mbuf(promptLen);

	if (mbuf(mark) > mbuf(len))
		mbuf(mark) = mbuf(len);
		
	ConcealCursor(buf);
	mbuf(cursorPos) = mbuf(mark);
	mbuf(mark) = tmp;
	mbuf(arg) = 1;
	DisplayCursor(buf);
	
}



/* The universal argument has no effect on this one */
/*ARGSUSED*/
static void CutMarkToPoint(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int lower, upper;
	static void QuitInput();

	CheckMode();

	if (mbuf(mark) < mbuf(promptLen))
		mbuf(mark) = mbuf(promptLen);
		
	if (mbuf(mark) > mbuf(len))
		mbuf(mark) = mbuf(len);

	if (mbuf(mark) < mbuf(cursorPos)) {
 		lower = mbuf(mark);
		upper = mbuf(cursorPos);
	} else {
		lower = mbuf(cursorPos);
		upper = mbuf(mark);
	}

	mbuf(killBufferLen) = upper - lower;
	while (mbuf(killBufferLen) > mbuf(killBufferSize)) {
	 	mbuf(killBufferSize) += BUF_BLOCK;
		if (mbuf(killBuffer) == NULL)
			mbuf(killBuffer) = XtMalloc((unsigned) mbuf(size));
		else 
			mbuf(killBuffer) = XtRealloc(mbuf(killBuffer),
			 (unsigned) mbuf(size));
	 	if (mbuf(killBuffer) == NULL) {
		 	mbuf(killBufferSize) = 0;
			mbuf(killBufferLen) = 0;
			mbuf(inputMode) = FALSE;
			QuitInput(w, event, params, numparams);
			return;
		}
	}

	ConcealCursor(buf);
	bcopy(mbuf(string) + lower, mbuf(killBuffer), mbuf(killBufferLen));
	bcopy(mbuf(string) + upper, mbuf(string) + lower, mbuf(len) - upper);
	mbuf(cursorPos) = mbuf(mark) = lower;
	mbuf(len) -= mbuf(killBufferLen);
	mbuf(arg) = 1;
	DisplayBuf(buf, FALSE);
	DisplayCursor(buf);
}



/* The universal argument has no effect on this one */
/*ARGSUSED*/
static void KillToEnd(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int tmp_mark;

	CheckMode();
	tmp_mark = mbuf(mark);
	mbuf(mark) = mbuf(len);
	CutMarkToPoint(w, event, params, numparams);
	mbuf(mark) = tmp_mark;
}



/* The universal argument has no effect on this one */
/*ARGSUSED*/
static void KillToBeginning(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register int tmp_mark;
	
	CheckMode();
	tmp_mark = mbuf(mark);
	mbuf(mark) = mbuf(promptLen);
	CutMarkToPoint(w, event, params, numparams);
	mbuf(mark) = tmp_mark;
}



/*ARGSUSED*/
static void UnivArgument(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;

	CheckMode();
	mbuf(arg) *= 4;
}


/*
 *  The universal argument has no effect on this one - maybe it should,
 *  I'm not sure. So I'll take the easy way out
 */
/*ARGSUSED*/
static void YankKilledStuff(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;

	CheckMode();
	ConcealCursor(buf);
	mbuf(mark) = mbuf(cursorPos);
	StrInsert(mbuf(killBuffer), mbuf(killBufferLen), buf);
	DisplayCursor(buf);
}


/*ARGSUSED*/
static void GetXBuffer(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register char *s;
	int nBytes;
	
	CheckMode();
	ConcealCursor(buf);
	s = XFetchBytes(XtDisplay(w), &nBytes);
	StrInsert(s, nBytes, buf);
	DisplayCursor(buf);
}



/*ARGSUSED*/
static void FinishedInput(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	register char *s;
	register int len = mbuf(len) - mbuf(promptLen);

	CheckMode();
	ConcealCursor(buf);
	XFlush(XtDisplay(w));
	mbuf(len) = 0;
	mbuf(cursorPos) = mbuf(startPos) = 0;
	mbuf(inputMode) = FALSE;
	if ((s = XtMalloc((unsigned) (len + 1))) != NULL) {
		bcopy(mbuf(string) + mbuf(promptLen), s, len);
		s[len] = '\0';
	}
	XtCallCallbacks(w, XtNfinishedCallback, s);
}


/*ARGSUSED*/
static void QuitInput(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;
	char *savestr;
	int savelen;

	CheckMode();
	ConcealCursor(buf);
	mbuf(len) = 0;
	mbuf(cursorPos) = mbuf(startPos) = 0;
	mbuf(inputMode) = FALSE;

	savestr = mbuf(string);
	savelen = mbuf(len);
	mbuf(string) = ABORT_MSG;
	mbuf(len) = strlen(ABORT_MSG);
	DisplayBuf(buf, TRUE);
	XFlush(XtDisplay(w));
	mbuf(string) = savestr;
	mbuf(len) = savelen;
	
	/*
	 *  We don't redisplay the cursor - we've gone out of input mode, but
	 *  we invoke the callback which tells the user about this
	 */
	XtCallCallbacks(w, XtNfinishedCallback, (caddr_t) NULL);
}

static MinibufWidget curbuf = NULL;

/*ARGSUSED*/
static void CompleteFilename(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;

	CheckMode();
	if (!mbuf(completion)) {
		CharInsert(w, event, params, numparams);
		return;
	}
	ConcealCursor(buf);
	mbuf(string)[mbuf(len)] = '\0';
	mbuf(cursorPos) = mbuf(len);
	curbuf = buf;
	(void) f_complete(mbuf(string) + mbuf(promptLen), 
	 mbuf(cursorPos) - mbuf(promptLen), 0, ' ');
	DisplayCursor(buf);
	
}

void rbell()
{
	register MinibufWidget buf = curbuf;
	
	XBell(XtDisplay(mcore(self)), 0);
}

/*
 *  'at' is somewhere in mbuf(string), and we replace from 'at' to the
 *  end of the line with the first 'len' chars of string 's' - 'curpos'
 *  is updated.
 */
void insert_s(at, s, len, curpos)
char *at;
char *s;
int *curpos;
{
	register MinibufWidget buf = curbuf;
	
	mbuf(len) = mbuf(cursorPos) = at - mbuf(string);
	*at = '\0';
	StrInsert(s, len, buf);
	*curpos = mbuf(cursorPos) - mbuf(promptLen);
	mbuf(string)[mbuf(len)] = '\0';
}

/* add_mess(s) char *s;
 *	inserts 's' at the end of the buffer, then waits a respectable
 *	interval, deletes 's', and returns
 */
void add_mess(s)
char *s;
{
	register MinibufWidget buf = curbuf;
	int savecursor = mbuf(cursorPos);
	
	StrInsert(s, strlen(s), buf);
	DisplayCursor(buf);
	ConcealCursor(buf);
	rbell();
	usleep(DELAY);
	mbuf(len) = mbuf(cursorPos) = savecursor;
	mbuf(string)[savecursor] = '\0';
	DisplayBuf(buf, FALSE);
}

/*ARGSUSED*/
static void ListFiles(w, event, params, numparams)
Widget w;
XEvent *event;
String *params;
Cardinal *numparams;
{
	register MinibufWidget buf = (MinibufWidget) w;

	CheckMode();
	if (!mbuf(completion)) {
		CharInsert(w, event, params, numparams);
		return;
	}
	/* 
	 *  We haven't implemented a proper window typeout yet - if I
	 *  can manage it with the Text widget, but right now, it
	 *  redraws twice, and refuses to go away unless you take the
	 *  mouse into that window and click - yech!
	 */
	curbuf = (MinibufWidget) w;
#ifdef TYPEOUT
	if (mbuf(typeout)) {
		(void) f_complete(mbuf(string) + mbuf(promptLen), 
		 mbuf(cursorPos) - mbuf(promptLen), mbuf(typeoutColumns), '?');
	} else
#endif TYPEOUT
		rbell();
}

/* 
 *  Public routines - these should probably be done with XtSetValues()
 *  but that means more work... Sigh!
 */
void MinibufGetInput(w, prompt, defaultInput, complete)
Widget w;
register char *prompt;
register char *defaultInput;
{
	register MinibufWidget buf = (MinibufWidget) w;
	if (buf == NULL) {
		return;
	}
	
	mbuf(cursorPos) = mbuf(len) = 0;
	if (prompt != NULL) {
		StrInsert(prompt, strlen(prompt), buf);
	}
	
	mbuf(promptLen) = mbuf(cursorPos);

	if (defaultInput != NULL) {
		StrInsert(defaultInput, strlen(defaultInput), buf);
		mbuf(cursorPos) = mbuf(promptLen);
	}
	mbuf(inputMode) = TRUE;
	mbuf(completion) = complete;
	DisplayCursor(buf);
	/*
	 *  Focus events on the widget exclusively, not spring loaded - we
	 *  remove the grab in the callback
	 */
}
	

/*
 *  This displays msg in the buffer - since it uses the string passed in
 *  directly, and does not affect the buffer string (which is saved and
 *  restored), it should result in no more mallocs.
 */
void MinibufDisplayMessage(w, msg, ringbell)
Widget w;
register char *msg;
int ringbell;	/* Do we want bell to sound */
{
	register MinibufWidget buf = (MinibufWidget) w;
	char *savestr;
	int savelen;

	if (buf == NULL || msg == NULL) {
		return;
	}
	
	mbuf(inputMode) = FALSE;
	mbuf(len) = mbuf(cursorPos) = mbuf(startPos) = 0;
	savestr = mbuf(string);
	savelen = mbuf(len);
	mbuf(string) = msg;
	mbuf(len) = strlen(msg);
	
	DisplayBuf(buf, TRUE);
	XFlush(XtDisplay(w));
	if (ringbell)		/* Maybe a visible bell for fun! One day...*/
		XBell(XtDisplay(w), 0);
	mbuf(string) = savestr;
	mbuf(len) = savelen;
}

