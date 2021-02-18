/* 
 *  CreateWindows() sets up all the windows in a widget hierarchy that
 *  consists of a form enclosing the picture window (Chris Peterson's
 *  Window widget), my Minibuf widget under it, and a column of Labels
 *  (StaticText) and ButtonBoxes (RCmanagers) to the right of it. If you
 *  want to try and figure out what this is, you had better have the Xt
 *  Intrinsics manual, and HP widgets handy. And plenty of aspirin...
 */
/*  Mark Moraes, CSRI, University of Toronto, July 26, 1988 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/PButton.h>
#include <Xw/RCManager.h>
#include <Xw/Form.h>
#include <Xw/SText.h>
#include "Window.h"
#include "Minibuf.h"

#include "xpic.h"
#include "windows.h"
#include "input.h"
#include "newfonts.h"
#include "draw.h"

#define MAXARGS 		32
/* 
 *  Arbitrary choice of width based on trial and error - if someone
 *  finds a better way, let me know. This is the width of the 'menu' of
 *  buttons along the right
 */
#define BUTTON_AREA_WIDTH	270

#define MAXWIDGETS		256

#define FORM_NAME		"form"
#define PIC_NAME		"picture"
#define MINIBUF_NAME	"minibuf"
#define DUMMYBUF_NAME	"dummybuf"
#define BUTTON_CURSOR	"top_left_arrow"
#define PIC_CURSOR		"tcross"
#define TEXT_CURSOR		"xterm"
#define WAIT_CURSOR		"watch"

Arg args[MAXARGS];
static Cardinal argcount;
#define setarg(name, value) (XtSetArg(args[argcount], name, value), argcount++)
    
/*
 *  In order to be efficient we should manage the whole hierarchy in 
 *  one go, the docs would have us believe. So we XtCreate() all 
 *  widgets, and store them in this array, using the addthingy() macro, 
 *  and before we realize the toplevel, we XtManage the whole 
 *  shebang.
 */
Widget *thingies;
static Cardinal num_kids = 0;
Widget LastParent;

#define addthingy(w, parent) \
{\
if (LastParent == parent) \
	thingies[num_kids++] = w;\
else \
{\
    XtManageChildren(thingies, num_kids);\
    num_kids = 0;\
    thingies[num_kids++] = w;\
}\
LastParent = parent; }

/*
 *  Selecting a button releases the button that is presently selected in
 *  a radio button box. So we don't want a release action
 */
static String booleanButtonTransTbl = 
	"<Btn1Down>: select()\n\
	 <EnterWindow>:	enter()\n\
	 <LeaveWindow>: leave()\n\
	 <FocusIn>: focusIn() \n\
	 <FocusOut>: focusOut() \n\
	 <KeyDown>s: select() \n\
	 <KeyDown>0xFF60: select() \n\
	 <KeyUp>0xFF60: unselect() \n\
	 <Key>0xFF52: traverseUp() \n\
	 <Key>0xFF54: traverseDown() \n\
	 <Key>0xFF51: traverseLeft() \n\
	 <Key>0xFF53: traverseRight() \n\
	 <Key>0xFF56: traverseNext() \n\
	 <Key>0xFF55: traversePrev() \n\
	 <Key>0xFF50: traverseHome()";

static String defaultButtonTransTbl = 
	"<Btn1Down>: select() unselect()\n\
	 <EnterWindow>:	enter()\n\
	 <LeaveWindow>: leave()\n\
	 <FocusIn>: focusIn() \n\
	 <FocusOut>: focusOut() \n\
	 <KeyDown>s: select() \n\
	 <KeyDown>0xFF60: select() \n\
	 <KeyUp>0xFF60: unselect() \n\
	 <Key>0xFF52: traverseUp() \n\
	 <Key>0xFF54: traverseDown() \n\
	 <Key>0xFF51: traverseLeft() \n\
	 <Key>0xFF53: traverseRight() \n\
	 <Key>0xFF56: traverseNext() \n\
	 <Key>0xFF55: traversePrev() \n\
	 <Key>0xFF50: traverseHome()";

static String updownTransTbl =
	"<Btn1Down>: increment() select() \n\
	 <Btn3Down>: decrement() select() \n\
	 <Btn1Up>: unselect() \n\
	 <Btn3Up>: unselect() \n\
	 <EnterWindow>: enter() \n\
	 <LeaveWindow>: leave() \n\
	 <FocusIn>: focusIn() \n\
	 <FocusOut>: focusOut() \n\
	 <KeyDown>s: select() \n\
	 <KeyDown>0xFF60: select() \n\
	 <KeyUp>0xFF60: unselect() \n\
	 <Key>0xFF52: traverseUp() \n\
	 <Key>0xFF54: traverseDown() \n\
	 <Key>0xFF51: traverseLeft() \n\
	 <Key>0xFF53: traverseRight() \n\
	 <Key>0xFF56: traverseNext() \n\
	 <Key>0xFF55: traversePrev() \n\
	 <Key>0xFF50: traverseHome()";

extern void Increment(), Decrement();
XtActionsRec actionTable[] = {
	{"increment", 	Increment},
	{"decrement", 	Decrement},
};

static 	XtTranslations booleanButtonTranslations;
static 	XtTranslations defaultButtonTranslations;
static 	XtTranslations updownTranslations;

/* 
 *  This array contains the names of the various menu items, and is
 *  structured in the form BoxTitle, BoxItemList, BoxTitle, BoxItemList,
 *  ......, 0 where each BoxItemList consists of BoxItemName,
 *  BoxItemName, ...., 0. The & indicates Command buttons and the +
 *  indicates the default value, which only makes sense for boolean
 *  buttons. The "*" is a special button - instead of making the
 *  procedure with it a handler, that procedure is called immediately,
 *  with the label as an argument - the procedure in question returns a
 *  ButtonInfo struct, and also initializes it's data appropriately.
 *  Typically, (well, always) I use this for an "up-down" button - this
 *  is a Command button where pressing the right key decrements, and the
 *  left key increments, and allows you to cycle through a list of
 *  choices. The initializing routine is meant to set this list of
 *  things to cycle through up - this can be read from a file.
 */

/*
 *  The method of initializing defaults is not too elegant, because the +
 *  merely means that the button should be highlit - it does not set the
 *  appropriate variables, which are initialized somewhere at the end of
 *  CreateWindows, and should be changed if the defaults are changed.
 *  Now if this was in a proper object oriented language......
 */
extern void picEventHandle();

extern void copy(), cut(), move(), adjust(), 
	rotate(), scale(), paste(),
	change(), editattrib(), element(), 
	linepattern(), linearrow(), linethickness(),
	textfont(), textsize(),
 	readcell(), savecell(), getcell(), putcell(),
	lprintcell(),
	redisplay(), quit(), change_buffer(),
	kill_buffer(), textvalign(), texthalign(),
	setgrid(), setsnap(), undo(), status();
#ifdef DEBUG
extern void printcell();
#endif

static ButtonInfo buttonnames[] = {
	"Elements", NULL, 
		"Box", element, "+Line", element, "Spline", element, 
		"Circle", element, "Ellipse", element, "Text", element,
		"Copy", copy, "Move", move, "Delete", cut, "Paste", paste,
		"Adjust", adjust, "ChangeAttribute", change,
#if 0
		/* !! One day... */
		"Scale", scale, "Rotate", rotate,
#endif
		"Get", getcell, "Put", putcell, 
	"Edit Attributes", NULL, 
		"Block", editattrib, "+Element", editattrib,
	"Line Patterns", NULL, 
		"+Solid", linepattern, "Dotted", linepattern, 
		"Short-Dashed", linepattern, "Long-Dashed", linepattern,
		"Dot-Dashed", linepattern,
	"Line Thickness", NULL,
		"+0", linethickness,
		"1", linethickness, "2", linethickness, "3", linethickness,
		"4", linethickness, "5", linethickness, "6", linethickness,
		"7", linethickness, "8", linethickness, "9", linethickness,
		"10", linethickness,		
	"Line Arrows", NULL,
		"+None", linearrow, "Start", linearrow, 
		"End", linearrow,	"Both", linearrow,
	"Text Font", NULL,
		"*", textfont,
	"Text Point Size", NULL, 
		"*", textsize,
	"Text Vertical", NULL,
		"Top", textvalign, "+Middle", textvalign, "Bottom", textvalign,
	"Text Horizontal", NULL,
		"+Centred", texthalign, "Left Just.", texthalign, 
		"Right Just.", texthalign,
	"Snap", NULL,
		"1", setsnap, "2", setsnap, 
		"4", setsnap, "+8", setsnap, "16", setsnap , "24", setsnap, 
		"32", setsnap, "40", setsnap,
	"&Files", NULL, 
		"Read", readcell, "Save", savecell, "Save As", savecell, 
		"Print", lprintcell,
	"&Miscellaneous", NULL,
		"Undo", undo, "Redisplay", redisplay, 
		"Grid", setgrid, "Status", status, "Exit", quit,
#ifdef DEBUG
		"PrintCell", printcell,
#endif
#if 0
	"&Buffers", NULL, 
		"Change Buffer", change_buffer,
		"Kill Buffer", kill_buffer,
#endif
	NULL, NULL
};

static Cursor framecursor, piccursor, waitcursor, textcursor;


void CreateWindows(toplevel)
Widget toplevel;
{
	Widget buttonbox, button;
	int i;
	Dimension height, border;
	int isCommand;
	char *label;
	int maxMenuWidth = BUTTON_AREA_WIDTH;
	char boxname[64];
	XrmValue fromVal, toVal;
	static XtCallbackRec callbacks[] = {
		{finished_input, NULL},
		{NULL, NULL},
	};
	XGCValues gcv;
	Pixel foreground, background;
	XColor foreback[2];
	char *grid_data;
	char *makegrid();
	int grid_width, grid_height, grid_size;
	/* 
	 *  The grids are stored as bitmaps - that's much faster than drawing
	 *  them point by point The grids are generated by the procedures
	 *  makegrid and invertgrid in grid.c from the value of the grid
	 *  point spacing.
	 */
	Pixmap gray_tile;
	void (*prehandler)();
	ButtonInfo posthandler;
	ButtonInfo updowninfo;
	XtTranslations translations;
	Widget typeout;
	extern Widget CreateTypeout();
	extern Pixmap XwCreateTile();

	thingies = (Widget *) XtCalloc(MAXWIDGETS, sizeof(Widget));
	
	XtAddActions(actionTable, XtNumber(actionTable));
	updownTranslations = XtParseTranslationTable(updownTransTbl);
	booleanButtonTranslations = XtParseTranslationTable(booleanButtonTransTbl);
	defaultButtonTranslations = XtParseTranslationTable(defaultButtonTransTbl);

	outerFrame = XtCreateWidget(FORM_NAME, XwformWidgetClass,
		toplevel, (ArgList) NULL, (Cardinal) 0);
	addthingy(outerFrame, toplevel);
	SetFocusRoot(outerFrame);
	fromVal.addr = BUTTON_CURSOR;
	fromVal.size = sizeof(BUTTON_CURSOR);
	XtConvert(outerFrame, XtRString, &fromVal, 
		XtRCursor, (XrmValuePtr) &toVal);
	framecursor = *(Cursor *) toVal.addr;

	picWinHeight = pageHeight * gridSpacing;
	picWinWidth = pageWidth * gridSpacing;
	minPicHeight = MINPAGEHEIGHT * gridSpacing;
	minPicWidth = MINPAGEWIDTH * gridSpacing;

	/*
	 *  First create the picwin - the XtNyAttachOffset is a lie; we'll
	 *  deal with it later when we get the correct height from the
	 *  minibuf widget. One day, I'll understand how to do this right
	 */
	argcount = 0;
	setarg(XtNwidth, (Dimension) picWinWidth);
	setarg(XtNheight, (Dimension) picWinHeight);
	setarg(XtNxRefName, FORM_NAME);
	setarg(XtNyRefName, FORM_NAME);
	setarg(XtNxResizable, True);
	setarg(XtNyResizable, True);
	setarg(XtNyAttachBottom, True);
	setarg(XtNyAttachOffset, 0);
	picWidget = XtCreateWidget(PIC_NAME, windowWidgetClass,
		outerFrame, args, argcount);
	addthingy(picWidget, outerFrame);
	fromVal.addr = PIC_CURSOR;
	fromVal.size = sizeof(PIC_CURSOR);
	XtConvert(outerFrame, XtRString, &fromVal,
		XtRCursor, (XrmValuePtr) &toVal);
	piccursor = *(Cursor *) toVal.addr;
	fromVal.addr = WAIT_CURSOR;
	fromVal.size = sizeof(WAIT_CURSOR);
	XtConvert(outerFrame, XtRString, &fromVal,
		XtRCursor, (XrmValuePtr) &toVal);
	waitcursor = *(Cursor *) toVal.addr;
	fromVal.addr = TEXT_CURSOR;
	fromVal.size = sizeof(TEXT_CURSOR);
	XtConvert(outerFrame, XtRString, &fromVal,
		XtRCursor, (XrmValuePtr) &toVal);
	textcursor = *(Cursor *) toVal.addr;

#ifdef TYPEOUT
	typeout = CreateTypeout(picWidget, 80, 40, 0, 0, (char *) NULL);
#endif

	/* Set event handler for picwin */
	/* One day, we'll use the translation manager here */
	XtAddEventHandler(picWidget, 
	 (EventMask) ExposureMask | ButtonPressMask | PointerMotionMask |
	 StructureNotifyMask, NULL, picEventHandle, (caddr_t) NULL);
	/* create the minibuf */
	argcount = 0;
	setarg(XtNwidth, (Dimension) picWinWidth);
	setarg(XtNxRefName, FORM_NAME);
	setarg(XtNyRefName, PIC_NAME);
	setarg(XtNyAddHeight, True);
	setarg(XtNxResizable, True);
	setarg(XtNyAttachBottom, True);
	setarg(XtNyAttachOffset, 0);
	setarg(XtNfinishedCallback, callbacks);
#ifdef TYPEOUT
	setarg(XtNtypeout, typeout);
	setarg(XtNtypeoutColumns, 80);
#endif
	inputWidget = XtCreateWidget(MINIBUF_NAME, minibufWidgetClass,
		outerFrame, args, argcount);
	addthingy(inputWidget, outerFrame);

	/* get the height and border width of the minibuf */
	argcount = 0;
	setarg(XtNheight, &height);
	setarg(XtNborderWidth, &border);
	XtGetValues(inputWidget, args, argcount);

	/* Shorten the picWidget to let the two coexist happily. */
	argcount = 0;
	setarg(XtNyAttachOffset, height + 2 * border);
	XtSetValues(picWidget, args, argcount);
	
	/*
	 *  Wanted to use RefWidget, but that causes Xt to dump core in a
	 *  strcpy deep in an XtCreate - so we use Refname which works well,
	 *  but is (I suspect) more time-consuming. Oh well - who cares...
	 */
	buttonbox = outerFrame;
	(void) strcpy(boxname, FORM_NAME);
	for (i = 0; buttonnames[i].name != NULL; i++) {
		if (buttonnames[i].handler == NULL) {
			/* If it starts with &, make it a command box */
			if (*buttonnames[i].name == '&') {
				label = &buttonnames[i].name[1];
				isCommand = TRUE;
			} else {
				label = buttonnames[i].name;
				isCommand = FALSE;
			}

			/* make label and attach to form */
			argcount = 0;
			setarg(XtNwidth, (Dimension) maxMenuWidth);
			setarg(XtNstring, label);
			setarg(XtNxRefName, PIC_NAME);
			setarg(XtNyRefName, boxname);
			setarg(XtNxAddWidth, True);
			setarg(XtNyAddHeight, True);
			setarg(XtNxAttachRight, True);
			setarg(XtNalignment, XwALIGN_CENTER);
			button = XtCreateWidget(label, 
				XwstatictextWidgetClass, outerFrame, args, argcount);
			addthingy(button, outerFrame);

			/*
			 *  make button box (RCmanager to HP Widgets) and attach to
			 *  label
			 */
			argcount = 0;
			/*
			 *  If it is a command button box, then buttons go up and
			 *  down normally. If radio button, then we only allow one
			 *  button to be down. Fortunately, the RCManager does this
			 *  for us - we just set up the translations for the button
			 *  so that Btn1Up is NOT 'release' - that's done by the
			 *  RCManager
			 */
			if (!isCommand)
				setarg(XtNmode, XwONE_OF_MANY);
			setarg(XtNwidth, (Dimension) maxMenuWidth);
			/* As many buttons in a row as possible */
			setarg(XtNlayoutType, XwMAXIMUM_UNALIGNED);
			setarg(XtNxRefName, PIC_NAME);
			setarg(XtNyRefName, label);
			setarg(XtNxAddWidth, True);
			setarg(XtNyAddHeight, True);
			/*
			 *  This maintains the box width, and since height is
			 *  unspecified, computes the minimum height. For
			 *  efficiency, it then gets left alone (we hope)
			 */
			setarg(XtNlayout, XwMAXIMIZE);
			setarg(XtNxResizable, False);
			(void) sprintf(boxname, "%s.box", label);
			buttonbox = XtCreateWidget(boxname,
			 XwrowColWidgetClass, outerFrame, args, argcount);
			addthingy(buttonbox, outerFrame);
		} else {
			/* make button a child of the current RCManager */
			argcount = 0;
#if 0
			setarg(XtNhighlightStyle, XwPATTERN_BORDER);
			setarg(XtNhighlightTile, XwFOREGROUND);
			setarg(XtNhighlightThickness, 0);
#endif
			prehandler = buttonnames[i].handler;
			posthandler.name = NULL;
			label = buttonnames[i].name;
			if (isCommand)
				translations = defaultButtonTranslations;
			else
				translations = booleanButtonTranslations;
			/* If the button name starts with a '+', it is ON by default */
			if (buttonnames[i].name[0] == '+') {
				label++;
				setarg(XtNset, True);
			} else if (buttonnames[i].name[0] == '*') {
				(* prehandler)(label, &updowninfo, &posthandler);
				label = updowninfo.name;
				prehandler = updowninfo.handler;
				translations = updownTranslations;
				isCommand = TRUE;	/* Just making sure! */
			}
			setarg(XtNlabel, label);
			setarg(XtNtranslations, translations);
			button = XtCreateWidget(label,
			 XwpushButtonWidgetClass, buttonbox, args, argcount);
			addthingy(button, buttonbox);
			XtAddCallback(button, XtNselect, prehandler, label);
			if (posthandler.name)
				(* posthandler.handler)(button);
		}
	}
	
	/* This had better be fast */
	if (num_kids)
	    XtManageChildren(thingies, num_kids);
	XtRealizeWidget(toplevel);
	XtFree((char *) thingies);

	picWin = XtWindow(picWidget);
	inputWin = XtWindow(inputWidget);

	argcount = 0;
	setarg(XtNforeground, &foreground);
	setarg(XtNbackground, &background);
	XtGetValues(picWidget, args, argcount);

	foreback[0].pixel = cursorColor;
	foreback[1].pixel = background;
	XQueryColors (picDpy, DefaultColormapOfScreen(XtScreen(outerFrame)),
	 foreback, 2);
	XRecolorCursor (picDpy, framecursor, foreback, foreback+1);
	XRecolorCursor (picDpy, piccursor, foreback, foreback+1);
	XDefineCursor(picDpy, XtWindow(outerFrame), framecursor);
	XDefineCursor(picDpy, picWin, piccursor);

	/*
	 *  make the GC stuff here - one for copy, one for invert. Remember
	 *  to change the both appropriately
	 */
	gcv.foreground = foreground;
	gcv.background = background;
	gcv.function = GXcopy;
	gcv.line_width = 0;
	gcv.line_style = LineSolid;
	gcNormal = XCreateGC(picDpy, picWin, GCForeground | GCBackground 
	 | GCLineWidth | GCLineStyle | GCFunction, &gcv);
	tmpGcNormal = XCreateGC(picDpy, picWin, GCForeground | GCBackground 
	 | GCLineWidth | GCLineStyle | GCFunction, &gcv);
	gcv.foreground = background;
	gcErase = XCreateGC(picDpy, picWin, GCForeground | GCBackground 
	 | GCLineWidth | GCLineStyle | GCFunction, &gcv);
	tmpGcErase = XCreateGC(picDpy, picWin, GCForeground | GCBackground 
	 | GCLineWidth | GCLineStyle | GCFunction, &gcv);
	gcv.foreground = foreground ^ background;
	gcv.function = GXxor;
	gcInvert = XCreateGC(picDpy, picWin, GCForeground | GCBackground
	 | GCLineWidth | GCLineStyle | GCFunction, &gcv);
	tmpGcInvert = XCreateGC(picDpy, picWin, GCForeground | GCBackground
	 | GCLineWidth | GCLineStyle | GCFunction, &gcv);
	
	/* This one's for drag boxes and outlines, bounding boxes, etc. */
	gcv.foreground = foreground ^ background;
	gcv.background = background;
	gcv.function = GXxor;
	gcBlock = XCreateGC(picDpy, picWin, GCForeground | GCBackground
	 | GCFunction, &gcv);

	/* Now create the tile for tiling the pic win */
	if ((grid_data = makegrid(gridSpacing, &grid_size, &grid_width, 
	 &grid_height)) == NULL) {
	 	error("CreateWindows: Can't make grid");
		/*NOTREACHED*/
	 }

	if ((gridTile = XCreatePixmapFromBitmapData(XtDisplay(outerFrame),
	  RootWindowOfScreen(XtScreen(outerFrame)), grid_data, 
	  (unsigned) grid_width, (unsigned) grid_height, gridColor, background, 
	  (unsigned) DefaultDepthOfScreen(XtScreen(outerFrame)))) == NULL) {
		error("CreateWindows: Can't make grid tile");
		/*NOTREACHED*/
	}

	blankTile = XwCreateTile(XtScreen(toplevel), foreground, background, 
	 XwBACKGROUND);
	 
	if(gridOn) {
		argcount = 0;
		setarg(XtNbackgroundPixmap, gridTile);
		XtSetValues(picWidget, args, argcount);
	}
	
	gray_tile = XwCreateTile(XtScreen(toplevel), foreground, background, 
	 Xw25_FOREGROUND);
	gcv.foreground = foreground ^ background;
	gcv.background = background;
	gcv.function = GXxor;
	gcv.fill_style = FillTiled;
	gcv.tile = gray_tile;

	gcGray = XCreateGC(picDpy, picWin, GCForeground | GCBackground
	 | GCFunction | GCFillStyle | GCTile, &gcv);

	picBox.ll.x = 0;
	picBox.ll.y = 0;
	picBox.ur.x = picWinWidth;
	picBox.ur.y = picWinHeight;

	mouseResolution = gridSpacing;

	if ((verts = (XPoint *) calloc((unsigned) INC_VERTS, sizeof(XPoint)))
	 == NULL) {
		error("CreateWindows: Can't allocate vertex buffer");
		/*NOTREACHED*/
	}
	nVerts = 0;
	maxVerts = INC_VERTS;

	/* Initialize the line styles - patterns from gnuplot */

	MAKEPATTERN(LineTypes[SOLID], "");					/* Solid */
	MAKEPATTERN(LineTypes[DOTTED], "\002\003");			/* Dotted */
	MAKEPATTERN(LineTypes[SDASH], "\005\003");			/* Short dashed */
	MAKEPATTERN(LineTypes[LDASH], "\010\004");			/* Long Dashed */
	MAKEPATTERN(LineTypes[DDASH], "\005\003\002\003");	/* Dot dashed */

	/* 
	 *  If these are changed, remember to change the leading + on the
	 *  menu item names in the buttonnnames struct. Not too nice to
	 *  hardwire these like this, but the handlers rely on these being
	 *  set up correctly.
	 */
	lineThickness = 0;
	drawingMode = START_MODE;
	editMode = ELEMENT;
	objectType = LINE;
	line_type = SOLID;
	line_arrow = NO_ARROW;
	textVertAlign = MIDLINE;
	textHorizAlign = CENTRE;
}


/*
 *  Generates a dummy ExposeWindow event
 */
void RedrawPicWin()
{
	XEvent ev;
	
	XClearWindow(picDpy, picWin);
	/* generate a dummy event to redraw the window */
	ev.type = Expose;
	ev.xexpose.window = picWin;
	ev.xexpose.width = picWinWidth;
	ev.xexpose.height = picWinHeight;
	picEventHandle(picWidget, (caddr_t) NULL, &ev);
}

void
SetWaitCursor()
{
	XDefineCursor(picDpy, picWin, waitcursor);
	XFlush(picDpy);
}

void
SetTextCursor()
{
	XDefineCursor(picDpy, picWin, textcursor);
	XFlush(picDpy);
}

void
SetWorkingCursor()
{
	XDefineCursor(picDpy, picWin, piccursor);
	XFlush(picDpy);
}
