/*
 * Copyright 1988 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of N.C.D. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  N.C.D. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Olaf Brandt, NCD X Consortium
 *
 *
 * Copyright 1988 Siemens
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Siemens not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Siemens makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Claus Gittinger, Siemens
 *
 * please feel free to add any bells and neats to this program -
 * 
 * written 16-17 Mar 1988 by claus gittinger
 *
 * Email: ...!decvax!unido!athen!claus
 *
 * a little font editor by claus
 * usage: xfed [-fg color] [-bg color] 
 *             [-bd color] [-bw number] [-nogrid] [-psize number] fontname.bdf
 */

#include "defs.h"
#include "X11/Xos.h"
#include "X11/cursorfont.h"
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include <stdio.h>
#include <errno.h>

#include "icon.bit"
#include "prev.bit"
#include "next.bit"
#include "save.bit"
#include "quit.bit"
#include "bar3.bit"
#include "down3.bit"
#include "right3.bit"

Display *dpy;
int screen;
Window mainWin, infoWin, nameWin, charinfoWin, directWin, editWin;
Window rightWin, downWin, actualWin, inverseWin;
Pixmap saveButton, nextButton, prevButton, quitButton;
Pixmap rightPtr, downPtr, directBar;
Cursor right_cursor, left_cursor, cross_cursor, crosshair_cursor;
GC gc_fg, gc_bg;
XFontStruct *xfont;
XSizeHints xsh;
int ex1, ey1, ex2, ey2, einc;
long fgPixel, bgPixel, bdPixel;
char *fontfilename = NULL;
struct character *charp;
int charIndex = 0;
int firstSave = 1;
int changed = 0;
int variable_width = 0;
int readOnly = 0;
char info_buffer[200];
char name_buffer[200];
char char_buffer[200];

int borderWidth = 2;
int pixsize = 0;
int psize = 0;
int padding = 3;
int ascent;
int grid=1;
int main_width, main_height;
int max_button_w, max_text_w, edit_y, edit_height, edit_width;
int inverse_y, direct_y;
int info_length, name_length, char_length;
int downp_col, rightp_row, direct_bar;
int actual_width, actual_height;
int info_height, info_width, info_x;

#define SPACE		5
#define direct_height	bar_height
#define downw_height	10
#define rightw_width	10
#define min(x,y)        (x<y ? x : y)
#define max(x,y)        (x>y ? x : y)
#define hex(n)          ((n>9 ? n+'a'-10 : n+'0'))
#define dec(c)          ((c>'9' ? 10+(c | 32)-'a' : c-'0'))
#define SetBit		Button1
#define ComBit		Button2
#define ClearBit	Button3
#define SetBitMask	Button1Mask
#define ComBitMask	Button2Mask
#define ClearBitMask	Button3Mask
#define	UP		0
#define DOWN		1
#define	insert_horiz	'i'
#define	delete_horiz	'd'
#define insert_vert	'I'
#define delete_vert	'D'

void prevCharacter(), nextCharacter(), save(), quit();

struct Button {
	char *bits;
	int width;
	int height;
	int state;
	void (*action)();
	Window win;
	Pixmap pix;
} button[] = {
	{prev_bits, prev_width, prev_height, UP, prevCharacter},
	{next_bits, next_width, next_height, UP, nextCharacter},
	{save_bits, save_width, save_height, UP, save},
	{quit_bits, quit_width, quit_height, UP, quit}
};
#define num_of_buttons	4

struct attribs {
    char *argname;
    char *resname;
    char *value;
    char *desc;
} attributes[] = {
	{ "-display", "Display", NULL,
	  "\t-display host:dpy\t\tX server to use\n" },
#define attr_display 0
	{ "-geometry", "Geometry", NULL,
	  "\t-geometry geom\t\t\tgeometry for window\n" },
#define attr_geometry 1
	{ "-psize", "PixelSize", NULL,
	  "\t-psize\t\t\t\tpixel size in pixels\n" },
#define attr_psize 2
	{ "-nogrid", "NoGrid", NULL,
	  "\t-nogrid\t\t\t\tdon't use a grid\n" },
#define attr_nogrid 3
	{ "-bw", "BorderWidth", NULL,
	  "\t-bw number\t\t\tborder width in pixels\n" },
#define attr_borderwidth 4
	{ "-fn", "BodyFont", NULL,
	  "\t-fn font\t\t\tfont used for text\n" },
#define attr_bodyfont 5
	{ "-fg", "Foreground", NULL,
	  "\t-fg color\t\t\tforeground color\n" },
#define attr_foreground 6
	{ "-bg", "Background", NULL,
	  "\t-bg color\t\t\tbackground color\n" },
#define attr_background 7
	{ "-bd", "BorderColor", NULL,
	  "\t-bd color\t\t\tborder color\n" },
#define attr_bordercolor 8
	{ NULL, NULL, NULL, NULL }};
char *no_answers[] = {"n", "no", "off", "none", "false", NULL};
char *list_o_fonts[] = {NULL, "fixed", "vtsingle", "variable", NULL};
char *progname;

main(argc, argv)
char *argv[];
int argc;
{
    XEvent event;
    setupWindow(argc, argv);
    charp = font.characters; charIndex = 0;
    downp_col = charp->bbx.w;
    rightp_row = charp->bbx.h;
    direct_bar=charIndex * (info_width - bar_width) /
    font.nchars + (bar_width >> 1);
    while(1) {
	XNextEvent(dpy, &event);
	eventHandler(&event);
    }
}

usage(argc, argv) 
char *argv[];
{
    struct attribs *attr;
    fprintf (stderr, "usage:\t%s [-options ...] filename\n\n", progname);
    fprintf (stderr, "where options include:\n");
    for (attr = attributes; attr->argname; attr++)
	fprintf (stderr, attr->desc);
    exit(1);
}

char *makelower(word)
register char *word;
{
    register char *alphabits;
    for (alphabits=word; *alphabits; alphabits++)
	if (*alphabits >= 'A' && *alphabits <= 'Z') *alphabits += 'a' - 'A';
    return(word);
}

memberp(word, list)
register char *word;
register char **list;
{
    makelower(word);
    for (; *list; list++)
	if (!strcmp(word, *list)) return(1);
    return(0);
}

instr(arg, argname)
char *arg, *argname;
{
    while(*arg && *argname)
	if (((*arg++) | 'a'-'A') != *argname++) return(1);
    return(0);
}
	
setupWindow(argc, argv) 
char *argv[];
{
    char *geometry = NULL;
    char *option;
    XGCValues xgcv;
    Window root;
    int x, y, w, h, i;
    int geom_result;
    struct attribs *attr;
    unsigned int d;
    Visual visual;
    XCharStruct overall;
    XColor fg_color, bg_color, bd_color, exact;

    progname = argv[0];
    for (i=1; i<argc; i++) {
	char *arg = argv[i];
	if (*arg == '-') {
	    for (attr=attributes; attr->argname; attr++) {
		if (!instr(arg, attr->argname)) {
		    if (!instr(arg, attributes[attr_nogrid].argname)) {
			attr->value = "True";
			break;
		    }
		    if (++i >= argc) usage();
		    attr->value = argv[i];
		    break;
		}
	    }
	    if (!attr->argname) usage();
	}
	else if (*arg == '=')
	    geometry = arg;
	else if (!fontfilename)
	    fontfilename = argv[i];
    }
    if (!fontfilename) {
	fprintf (stderr, "%s:\tno file name specified\n", progname);
	usage();
    }
    if (!freopen(fontfilename, "r", stdin)) {
	fprintf(stderr, "%s:\tcannot open file: %s\n",progname, fontfilename);
	exit(1);
    }
    if (!freopen(fontfilename, "r+", stdin)) {
	fprintf(stderr, "%s:\twarning file '%s' is read only\n"
		,progname, fontfilename);
	readOnly = 1;
    }
    freopen(fontfilename, "r", stdin);
    yyparse();
    if (!(dpy = XOpenDisplay(option = attributes[attr_display].value))) {
	fprintf (stderr, "%s:\tunable to open display '%s'\n", progname,
		XDisplayName(option));
	exit(1);
    }
    for (attr = attributes; attr->argname; attr++) {
	if (!attr->value)
	    attr->value = XGetDefault(dpy, progname, attr->resname);
	if (!attr->value)
	    attr->value = XGetDefault(dpy, progname, makelower(attr->resname));
    }
    if ((option = attributes[attr_nogrid].value) &&
	!memberp (option, no_answers))
	grid = 0;
    if (option = attributes[attr_psize].value)
        psize = max(atoi(option), 1);
    if (option = attributes[attr_borderwidth].value)
        borderWidth = max(atoi(option), 0);
    if ((option = attributes[attr_geometry].value) && !geometry)
	geometry = option;

    screen = DefaultScreen(dpy);

    i = ((*list_o_fonts = attributes[attr_bodyfont].value) ? 0 : 1);
    for (; option = list_o_fonts[i]; i++) {
	if (xfont = XLoadQueryFont(dpy, option)) break;
	fprintf(stderr, "%s:\tunable to open font '%s'\n",
	    progname, option);
    }
    if (!xfont) {
	XCloseDisplay(dpy);
	exit(1);
    }
    if (xfont->min_bounds.width != xfont->max_bounds.width)
	variable_width = 1;

    cross_cursor = XCreateFontCursor(dpy, XC_cross);
    crosshair_cursor = XCreateFontCursor(dpy, XC_crosshair);
    right_cursor = XCreateFontCursor(dpy, XC_right_ptr);
    left_cursor = XCreateFontCursor(dpy, XC_left_ptr);

    fgPixel = bdPixel = BlackPixel(dpy, screen);
    bgPixel = WhitePixel(dpy, screen);
    {
	Colormap cmap = DefaultColormap(dpy, screen);
	char *fgColor = attributes[attr_foreground].value;
	char *bgColor = attributes[attr_background].value;
	char *bdColor = attributes[attr_bordercolor].value;
	XColor fgdef, bgdef, bddef, exact;

	if (fgColor && XParseColor(dpy, cmap, fgColor, &fgdef))
	    if (XAllocColor(dpy, cmap, &fgdef))
	        fgPixel = fgdef.pixel;
	if (bgColor && XParseColor(dpy, cmap, bgColor, &bgdef))
	    if (XAllocColor(dpy, cmap, &bgdef))
	        bgPixel = bgdef.pixel;
	if (bdColor && XParseColor(dpy, cmap, bdColor, &bddef))
	    if (XAllocColor(dpy, cmap, &bddef))
	        bdPixel = bddef.pixel;
    }
    {
    XSetWindowAttributes xswa;
    int display_width = DisplayWidth(dpy, screen);
    int display_height = DisplayHeight(dpy, screen);

    xswa.background_pixel = bgPixel;
    xswa.border_pixel = bdPixel;
    xswa.cursor = crosshair_cursor;
    xswa.event_mask = StructureNotifyMask;

    mainWin = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0, 1, 1,
		borderWidth, CopyFromParent, CopyFromParent, CopyFromParent,
	       	CWEventMask | CWCursor | CWBorderPixel | CWBackPixel, &xswa);

    xgcv.foreground = fgPixel;
    xgcv.background = bgPixel;
    xgcv.font = xfont->fid;
    gc_fg = XCreateGC(dpy, mainWin, GCForeground|GCBackground|GCFont, &xgcv);
    xgcv.foreground = bgPixel;
    xgcv.background = fgPixel;
    gc_bg = XCreateGC(dpy, mainWin, GCForeground|GCBackground|GCFont, &xgcv);

    layout1();

    xsh.min_width = 6*SPACE + max_button_w + font.maxbbx.w * (grid + 1) + grid;
    xsh.min_height = max(edit_y + font.maxbbx.h + grid + 3 * SPACE,
	inverse_y + actual_height + SPACE);
    xsh.flags = PMinSize;
    if (geometry) {
	geom_result = XParseGeometry(geometry, &xsh.x, &xsh.y,
	    &xsh.width, &xsh.height);
	if ((geom_result & WidthValue) && (geom_result & HeightValue)) {
	    if (xsh.width < xsh.min_width) xsh.width = xsh.min_width;
	    if (xsh.height < xsh.min_height) xsh.height=xsh.min_height;
	    xsh.flags |= USSize;
	}
	if ((geom_result & XValue) && (geom_result & YValue))
	    xsh.flags |= USPosition;
    }
    if (!(xsh.flags & USSize)) {
	xsh.width = info_x + info_width + SPACE;
	xsh.height = max(edit_y + edit_height, inverse_y+actual_height) +SPACE;
	xsh.flags |= PSize;
    }
    if (!(xsh.flags & USPosition)) {
	xsh.x = min(200, display_width - xsh.width - 2 * borderWidth);
	xsh.y = min(200, display_height - xsh.height - 2 * borderWidth);
	xsh.flags |= PPosition;
    }
    if (geom_result & XNegative)
	xsh.x += display_width - xsh.width - 2 * borderWidth;
    if (geom_result & YNegative)
	xsh.y += display_height - xsh.height - 2 * borderWidth;
    XResizeWindow (dpy, mainWin, xsh.width, xsh.height);
    XSetStandardProperties(dpy, mainWin, "Font Editor", progname,
	XCreateBitmapFromData(dpy, mainWin, icon_bits, icon_width, icon_height),
	argv, argc, &xsh);
    }
    XMapWindow(dpy, mainWin);
}

eventHandler(eventp)
XEvent *eventp;
{
    XEvent event = *eventp;
    Window eventWin;
    char firstKey();
    int last_row, last_col;
    int this_row, this_col;
    int x, y, w, h, i;

    eventWin = event.xany.window;
    switch (event.xany.type) {
	case ConfigureNotify:
	    if (eventWin != mainWin) break;
	    if (event.xconfigure.width == main_width &&
		event.xconfigure.height == main_height) break;
	    layout2(event.xconfigure.width, event.xconfigure.height);
	    XMapSubwindows(dpy, editWin);
	    XMapSubwindows(dpy, mainWin);
	    break;

	case Expose:
	    if (eventWin == editWin)
		redrawEditWindow();
	    else if (eventWin == downWin)
		redrawDownWindow();
	    else if (eventWin == rightWin)
		redrawRightWindow();
	    else if (eventWin == directWin)
		redrawDirectWindow(direct_bar);
	    else if (eventWin == charinfoWin)
		redrawCharinfoWindow(charp);
	    else if (eventWin == actualWin)
		redrawActualWindow(actualWin, gc_bg, gc_fg);
	    else if (eventWin == inverseWin)
		redrawActualWindow(inverseWin, gc_fg, gc_bg);
	    else if (eventWin == infoWin)
		redrawInfoWindow();
	    else if (eventWin == nameWin)
		redrawNameWindow();
	    else 
		for (i=num_of_buttons; i--;)
		    if (eventWin == button[i].win) {
			XCopyPlane(dpy, button[i].pix, eventWin, gc_fg, 0, 0,
			    button[i].width, button[i].height, 0, 0, 1);
			break;
		    }
		break;

	case ButtonPress:
	    if (eventWin == downWin)
		grabDown(event.xbutton.x);
	    else if (eventWin == rightWin)
		grabRight(event.xbutton.y);
	    else if (eventWin == directWin)
		grabBar(event.xbutton.x);
	    else if (eventWin == editWin) {
		if (whatSquare(event.xbutton.x, event.xbutton.y,
		    &last_row, &last_col) == 0)
		bitOp(event.xbutton.button, last_row, last_col);
		break;
	    } 
	    else
		for (i=num_of_buttons; i--;)
		    if (eventWin == button[i].win && button[i].state == UP) {
			button[i].state = DOWN;
			XCopyPlane(dpy,button[i].pix, eventWin, gc_bg, 0, 0,
			    button[i].width, button[i].height, 0, 0, 1);
			break;
		    }
	    break;

	case ButtonRelease:
	case LeaveNotify:
	    for (i=num_of_buttons; i--;)
		if (eventWin == button[i].win && button[i].state == DOWN) {
		    button[i].state = UP;
		    XCopyPlane(dpy,button[i].pix, eventWin, gc_fg, 0, 0,
			button[i].width, button[i].height, 0, 0, 1);
		    if (event.xany.type == ButtonRelease)
			button[i].action();
		    break;
		}
	   break;

	case MotionNotify:
	    if ((eventWin == editWin) && (!whatSquare (event.xmotion.x,
		event.xmotion.y, &this_row, &this_col)) &&
      		((this_row != last_row) || (this_col != last_col))) {
		bitOp(event.xmotion.state, this_row, this_col);
      		last_row = this_row;
      		last_col = this_col;
	    }
	break;

	case KeyPress:
	    if (eventWin == directWin)
		gotoChar(firstKey(event), -1);
	    else if (eventWin == editWin)
		editInsDel(firstKey(event), event);
	    break;

	default:
	    break;
    }
}

dialogEventHandler(event)
XEvent *event;
{
    if (event->type == Expose || event->type == ConfigureNotify)
	eventHandler(event);
}

/*
 * convert pointer x/y to row/col and check for pixel in bounding box
 * return -1 if illegal.
 */
whatSquare(x, y, prow, pcol)
int *prow, *pcol;
{
	int row, col;

	if (x < ex1 || x >= ex1 + downp_col * einc ||
            y < ey1 || y >= ey1 + rightp_row * einc) return(-1);
	col = (x - ex1) / einc;
	row = (y - ey1) / einc;
	*prow = row;
	*pcol = col;
	return 0;
}

/*
 * set/clear/complement a pixel in current character - row/col must be checked
 * elsewhere
 */
bitOp(what, row, col)
short unsigned int row, col;
{
	char *rowbits;
	short unsigned int bits, x, y, colq, colr, mask, bits_unchanged;

	changed = 1;
	colq = col >> 2;
	mask = 1 << (3 - (col & 3));
	bits_unchanged = bits = dec(charp->rows[row][colq]);

	switch (what) {
	    case SetBit:
	    case SetBitMask:
		bits |= mask;
		break;
	    case ComBit:
	    case ComBitMask:
		bits ^= mask;
		break;
	    case ClearBit:
	    case ClearBitMask:
		bits &= ~mask;
		break;
	}
	if (bits == bits_unchanged) return;
	charp->rows[row][colq] = hex(bits);
	x = ex1 + col*einc +1;
        y = ey1 + row*einc +1;
	if (bits & mask) {
	    XFillRectangle(dpy, editWin, gc_fg, x, y, pixsize, pixsize);
	    XDrawPoint(dpy, actualWin, gc_fg, col+3, row+3);
	    XDrawPoint(dpy, inverseWin, gc_bg, col+3, row+3);
	}
	else {
	    XFillRectangle(dpy, editWin, gc_bg, x, y, pixsize, pixsize);
	    XDrawPoint(dpy, actualWin, gc_bg, col+3, row+3);
	    XDrawPoint(dpy, inverseWin, gc_fg, col+3, row+3);
	}
}

void
nextCharacter() {
    if ((charIndex+1) < font.nchars) {
        charp++;
        charIndex++;
    }
    else
	for (; charIndex; charIndex--)
	    charp--;
    displayCharacter();
}

void
prevCharacter() {
    if (charIndex > 0) {
        charp--;
        charIndex--;
    }
    else
	for (; charIndex < font.nchars-1; charIndex++)
	    charp++;
    displayCharacter();
}

redrawEditWindow() {
	char *rowbits, c;
	short unsigned int nrows, row, col, ix, iy, len, bit, x, y;

	XClearWindow(dpy, editWin);
	if (grid) {
	    XSetForeground(dpy, gc_fg, bdPixel);
	    for (y=ey1; y <= ey2; y += einc)
	        XDrawLine(dpy, editWin, gc_fg, ex1, y, ex2, y);
	    for (x=ex1; x <= ex2; x += einc)
	        XDrawLine(dpy, editWin, gc_fg, x, ey1, x, ey2);
	    XSetForeground(dpy, gc_fg, fgPixel);
        }

	nrows = charp->nrows;
	for (iy=0, row=ey1; iy<nrows; iy++, row += einc) {
	    rowbits = charp->rows[iy];
	    len = strlen(rowbits);
	    for (ix=0, col=ex1; ix<len; ix++) {
		if ((x = dec(rowbits[ix])) == 0) {
		    col += (einc << 2);
		    continue;
		}
		for (bit=8; bit; bit >>= 1, col += einc)
		    if (x & bit)
			XFillRectangle(dpy, editWin, gc_fg, col+1, row+1,
					pixsize, pixsize);
	    }
	}
}

redrawDownWindow()
{
        XClearWindow(dpy, downWin);
	XCopyPlane(dpy, downPtr, downWin, gc_fg, down_width, 0,
                   down_width, downw_height, downp_col * einc + 7, 0, 1);
}

redrawRightWindow()
{
        XClearWindow(dpy, rightWin);
	XCopyPlane(dpy, rightPtr, rightWin, gc_fg, 0, right_height,
                   rightw_width, right_height, 0, rightp_row * einc + 7, 1);
}
redrawInfoWindow() {
	XDrawImageString(dpy, infoWin, gc_fg, padding, ascent+padding,
			  info_buffer, info_length);
}
redrawNameWindow() {
	XDrawImageString(dpy, nameWin, gc_fg, padding, ascent+padding,
			  name_buffer, name_length);
}
redrawCharinfoWindow(charp)
struct character *charp;
{
	static int new_length;
	sprintf(char_buffer, " Character: '%s'   Code: %x", 
		charp->charId, charp->encoding);
	if ((new_length = strlen(char_buffer)) != char_length || variable_width)
	    XClearWindow(dpy, charinfoWin);
	XDrawImageString(dpy, charinfoWin, gc_fg, padding, ascent+padding,
			  char_buffer, char_length = new_length);
}
redrawDirectWindow(x)
int x;
{
        XClearWindow(dpy, directWin);
	XCopyPlane(dpy, directBar, directWin, gc_fg, bar_width, 0,
                   bar_width, bar_height, x - (bar_width >> 1), 0, 1);
}

redrawActualWindow(win, gc_fg, gc_bg)
Window win;
GC gc_fg, gc_bg;
{
	char *rowbits, c;
	short unsigned int bit, nrows, ix, row, col, len;
	short unsigned int register x;

	XFillRectangle(dpy, win, gc_fg, 3, 3, actual_width-6, actual_height-6);

	nrows = charp->nrows;
	for (row=0; row<nrows; row++) {
	    rowbits = charp->rows[row];
	    len = strlen(rowbits);
	    for (ix=0, col=0; ix<len; ix++) {
		if ((c = rowbits[ix]) == '0') {
		    col += 4;
		    continue;
		}
		x = dec(c);
		for (bit=8; bit; bit >>= 1, col++)
		    if (x & bit)
			XDrawPoint(dpy, win, gc_bg, col+3, row+3);
	    }
	}
}

int clearmask[] = { 15, 8, 12, 14 };

changeBBX(newx, newy)
int newx, newy;
{
    unsigned int i, x, y, row, col, ncols, oldx, oldy;
    char *rowbits, **newrows;
    char *tmp;
    
    oldx = charp->bbx.w * einc + ex1;
    oldy = charp->bbx.h * einc + ey1;

    if (newx >= 0) {
        downp_col = (newx + (einc >> 1)  - ex1) / einc;
        redrawDownWindow();
        if (downp_col == charp->bbx.w || downp_col < 1) return;
        ncols = (downp_col - 1) >> 2;
        if (charp->bbx.w > downp_col) {
            for (row = 0; row < charp->nrows; row++) {
                rowbits = (char *) malloc(ncols + 3);
                strcpy(rowbits, charp->rows[row]);
                x = dec(rowbits[ncols]);
                x &= clearmask[downp_col & 3];
                rowbits[ncols] = hex(x);
                rowbits[ncols +1] = '0';
                rowbits[ncols +2] = '\0';
                free(charp->rows[row]);
                charp->rows[row] = rowbits;
            } 
            for (x = ex1 + downp_col * einc ; x < oldx; x += einc)
                 for (y = ey1; y < oldy; y += einc)
                      XFillRectangle(dpy, editWin, gc_bg,
                                     x+1, y+1, pixsize, pixsize);
            redrawActualWindow(actualWin, gc_bg, gc_fg);
            redrawActualWindow(inverseWin, gc_fg, gc_bg);
        }
        else {
            for (row = 0; row < charp->nrows; row++) {
                tmp = (char *) malloc(ncols + 3);
                strcpy(tmp, charp->rows[row]);
                while (strlen(tmp) <= ncols+1)
                    strcat (tmp, "0");
                free(charp->rows[row]);
                charp->rows[row] = tmp;
            }
        }
        charp->bbx.w = charp->dwidth[0] = downp_col;
    }
    else {
        rightp_row = (newy + einc / 2 - ey1) / einc;
        redrawRightWindow();
        if (rightp_row == charp->bbx.h || rightp_row < 1) return;
        ncols = (downp_col - 1) >> 2;
        tmp = (char *) malloc(ncols + 2);
        for (i = 0; i <= ncols+1; i++)
            tmp[i] = '0';
        tmp[i] = '\0';
        if (rightp_row < charp->bbx.h) {
            for (row = 0; row < charp->nrows; row++) {
                if (row >= rightp_row)
                    strcpy(charp->rows[row], tmp);
            }
            for (x = ex1; x < oldx; x += einc)
                 for (y = ey1 + rightp_row * einc; y < oldy; y += einc)
                      XFillRectangle(dpy, editWin, gc_bg,
                                     x+1, y+1, pixsize, pixsize);
            redrawActualWindow(actualWin, gc_bg, gc_fg);
            redrawActualWindow(inverseWin, gc_fg, gc_bg);
        }
        else {
            newrows = (char **)malloc(sizeof(char *) * rightp_row);
            for (i=0; i < charp->bbx.h; i++){
                newrows[i] = (char *) malloc(ncols + 2);
                strcpy(newrows[i],charp->rows[i]);
            }
            for (i; i < rightp_row; i++){
                newrows[i] = (char *) malloc(ncols + 2);
                strcpy(newrows[i],tmp);
            }
            free(charp->rows);
            charp->rows = newrows;
        }
        charp->nrows = charp->bbx.h = rightp_row;
    }
    changed = 1;
}

char
firstKey(event)
XEvent event;
{
    char keys[1];

    return(XLookupString(&event.xkey,keys,1,NULL,NULL) == 1 ? *keys : NULL);
}

gotoChar(c,index)
char c;
{
    struct character *newp;
    char *id;
    int i;

    if (index >= 0) {
	charp = font.characters + (charIndex = index);
	displayCharacter();
	return;
    }
    if (!c) return;
    newp = font.characters;
    for (i=0; i < font.nchars-1; i++, newp++) {
        id = newp->charId;
        if (id[0] == c && id[1] == '\0') {
            if (charp == newp) return;
            charp = newp;
            charIndex = i;
            displayCharacter();
            return;
        }
    }
    return;
}

displayCharacter()
{
    direct_bar=charIndex * (info_width - bar_width) /
		font.nchars + (bar_width >> 1);
    downp_col = charp->bbx.w;
    rightp_row = charp->bbx.h;
    redrawCharinfoWindow(charp);
    redrawDirectWindow(direct_bar);
    redrawDownWindow();
    redrawRightWindow();
    redrawEditWindow();
    redrawActualWindow(actualWin, gc_bg, gc_fg);
    redrawActualWindow(inverseWin, gc_fg, gc_bg);
    return;
}

int movemask[] = {15, 7, 3, 1};
int keepmask[] = {8, 12, 14, 15};
int delmask[] = {0, 8, 12, 14};

editInsDel(c, event)
char c;
XEvent event;
{
    char *tmp, *rowbits;
    int carry, oldcarry, ecol, dcol, event_row, event_col, i, j, x;
    int kmask, mmask;
 
    if (whatSquare (event.xkey.x, event.xkey.y,
		    &event_row, &event_col) == -1)
	return;
    if (c == insert_horiz) {
        if (event_row == rightp_row - 1) return;
        for (i=rightp_row-1; i>event_row; i--)
            strcpy(charp->rows[i], charp->rows[i-1]);
    }
    else if (c == delete_horiz) {
        for (i=event_row; i<rightp_row-1; i++) {
            strcpy(charp->rows[i], charp->rows[i+1]);
        }
        tmp = (char *) malloc((downp_col-1)/4 + 2);
        for (i = 0; i <= (downp_col-1)/4+1; i++)
            tmp[i] = '0';
        tmp[i] = '\0';
        strcpy (charp->rows[rightp_row-1],tmp);
    }
    else if (c == insert_vert) {
        if (event_col == downp_col - 1) return;
        ecol = (event_col)/4;
        dcol = (downp_col-1)/4;
        for (i=0; i<rightp_row; i++) {
            rowbits = charp->rows[i];
	    x = dec(rowbits[dcol]);
            x &= keepmask[(downp_col-2) & 3];
	    rowbits[dcol] = hex(x);
            for (j=dcol; j>ecol; j--) {
                x = dec(rowbits[j]);
                x = x/2 + (dec(rowbits[j-1]) & 1 ? 8 : 0);
                rowbits[j] = hex(x);
            }
            x = dec(rowbits[j]);
            kmask = keepmask[event_col & 3];
            mmask = movemask[event_col & 3];
            x = (x & kmask) | ((x & mmask) / 2);
            rowbits[j] = hex(x);
        }
    }
    else if (c == delete_vert) {
        ecol = (event_col)/4;
        dcol = (downp_col-1)/4;
        for (i=0; i<rightp_row; i++) {
            rowbits = charp->rows[i];
            x = dec(rowbits[ecol]);
            kmask = delmask[event_col & 3];
            mmask = movemask[event_col & 3] / 2;
            x = (x & kmask ) | ((x & mmask) * 2);
            if (strlen(rowbits) > 1)
                x += (dec(rowbits[ecol+1]) & 8 ? 1 : 0);
            rowbits[ecol] = hex(x);
            for (j=ecol+1; j<=dcol; j++) {
                x = dec(rowbits[j]);
                x = (x * 2) & 15;
                if (j < strlen(rowbits)-1)
                    x += (dec(rowbits[j+1]) & 8 ? 1 : 0);
                rowbits[j] = hex(x);
            }
        }
    }
    else return;
    changed = 1;
    displayCharacter();
    return;
}    

grabBar(new)
int new;
{
	short int status, old, low, high, half, index, newx;
	Cursor cursor;
	XEvent event;
	struct character *charp;

	half = bar_width / 2;
	low = half;
	high = info_width - half - 1;
	old = direct_bar;

	status = XGrabPointer(dpy, directWin, True, KeyPressMask |
				ButtonReleaseMask | PointerMotionMask,
				GrabModeSync, GrabModeAsync, None,
				right_cursor, CurrentTime);
	if (status != GrabSuccess) {
	    fprintf(stderr, "%s:\tcan't grab the pointer", progname);
	    return;
	}
	while (1) {
	    XAllowEvents(dpy, AsyncPointer, CurrentTime);
	    XNextEvent(dpy, &event);
	    new = min (max (low, event.xbutton.x), high);
	    index = (double)new / (info_width - half) * font.nchars;
	    index = min(max(0, index), font.nchars);
            charp = font.characters + index;
	    if (event.type == ButtonRelease) {
		XUngrabPointer(dpy, CurrentTime);
		gotoChar('\0',index);
		return;
	    }
	    else if (event.type == MotionNotify) {
	        if (abs(old - new) > bar_width) {
	            XClearArea(dpy, directWin, old - half, 0,
			       bar_width, bar_height, False);
	            XCopyPlane(dpy, directBar, directWin, gc_fg, bar_width, 0,
                               bar_width, bar_height, new - half, 0, 1);
	        }
	        else {
	            XCopyPlane(dpy, directBar, directWin, gc_fg, 0, 0,
                       bar3_width, bar3_height, new - bar_width - half, 0, 1);
	        }
	        old = new;
		if (!variable_width)
		    redrawCharinfoWindow(charp);
	    }
	}
}

grabDown(new)
int new;
{
    int status, old, low, high, half;
    Cursor cursor;
    XEvent event;

	half = down_width >> 1;
	low = ex1 + einc;
	high = ex1 + font.maxbbx.w * einc;
	old = ex1 + downp_col * einc;

	status = XGrabPointer(dpy, downWin, False,
				ButtonReleaseMask | PointerMotionMask,
				GrabModeSync, GrabModeAsync, None,
				right_cursor, CurrentTime);
	if (status != GrabSuccess) {
	    fprintf(stderr, "%s:\tcan't grab the pointer", progname);
	    return;
	}
	
	while (1) {
	    XAllowEvents(dpy, AsyncPointer, CurrentTime);
	    XNextEvent(dpy, &event);
            new = min(max(low, event.xbutton.x), high);
	    if (event.type == ButtonRelease) {
                changeBBX(new ,-1);
		XUngrabPointer(dpy, CurrentTime);
		XFlush(dpy);
		return;
	    }
	    else if (event.type == MotionNotify) {
	        if (abs(old  - new) > down_width) {
	            XClearArea(dpy, downWin, old - half, 0,
			       down_width, 2 * SPACE, False);
	            XCopyPlane(dpy, downPtr, downWin, gc_fg, down_width, 0,
                               down_width, 2 * SPACE, new - half, 0, 1);
	        }
	        else {
	            XCopyPlane(dpy, downPtr, downWin, gc_fg, 0, 0,
				down3_width,down3_height,
				(new - down_width) - half, 0, 1);
	        }
	        old = new;
	    }
	}
}

grabRight(new)
int new;
{
    int status, old, low, high, half;
    Cursor cursor;
    XEvent event;

	old = ey1 + rightp_row * einc;
	half = right_height >> 1;
	low = ey1 + einc;
	high = ey1 + font.maxbbx.h * einc;
	status = XGrabPointer(dpy, rightWin, False,
				ButtonReleaseMask | PointerMotionMask,
				GrabModeSync, GrabModeAsync, None,
				left_cursor, CurrentTime);
	if (status != GrabSuccess) {
	    fprintf(stderr, "%s:\tcan't grab the pointer", progname);
	    return;
	}
	
	while (1) {
	    XAllowEvents(dpy, AsyncPointer, CurrentTime);
	    XNextEvent(dpy, &event);
            new = min(max(low, event.xbutton.y), high);
	    if (event.type == ButtonRelease) {
                changeBBX(-1, new);
		XUngrabPointer(dpy, CurrentTime);
		XFlush(dpy);
		return;
	    }
	    else if (event.type == MotionNotify) {
	        if (abs(old - new) > right_height) {
	            XClearArea(dpy, rightWin, 0, old - half,
			       2 * SPACE, right_height, False);
	            XCopyPlane(dpy, rightPtr, rightWin, gc_fg, 0, right_height,
                               2 * SPACE, right_height, 0, new - half, 1);
	        }
	        else
	            XCopyPlane(dpy, rightPtr, rightWin, gc_fg, 0, 0,
				right3_width, right3_height,
				0, (new - right_height) - half, 1);
	        old = new;
	    }
	}
}

void
save()
{
    char cmdbuff[132];
    if (readOnly) {
       fprintf(stderr, "%s:\tfile '%s' is read only\n", progname, fontfilename);
       return;
    }
    if (firstSave) {
        firstSave = 0;
        sprintf(cmdbuff, "mv -f %s %s~", fontfilename, fontfilename);
        system(cmdbuff);
    }
    output(&font, fontfilename);
    changed = 0;
}

void
quit()
{
    if (changed && !readOnly) {
	int result;
	static char *strings[3] = {"Yes", "No", "Cancel"};
	result = dialog(mainWin, xfont, "Save changes before quitting?",
	    "", strings, 3, dialogEventHandler);
	switch (result) {
	    case 0:
		save();
		exit(0);
	    case 1:
		exit(0);
	    default:
		return;
	}
    }
    exit(0);
}

layout1()
{
#define CWMASK CWBackPixel | CWBorderPixel | CWCursor | CWWinGravity |\
	CWEventMask 
#define CFP3 CopyFromParent, CopyFromParent, CopyFromParent
    int dir, descent, xpos, ypos, i;
    XCharStruct overall;
    XSetWindowAttributes xswa;
    int info_y, name_y, charinfo_y;
    sprintf(info_buffer," File: '%s' ",fontfilename);
    sprintf(name_buffer," Name: '%s'  %d Characters ",
	font.fontname,font.nchars);
    info_length = strlen(info_buffer);
    name_length = strlen(name_buffer);
    if (info_length > name_length)
    XTextExtents(xfont, info_buffer, info_length, &dir, &ascent, &descent,
		&overall);
    else
    XTextExtents(xfont, name_buffer, name_length, &dir, &ascent, &descent,
		&overall);

    info_width = overall.rbearing - overall.lbearing + padding * 2;
    info_height = ascent + descent + padding * 2;
    max_text_w = info_width;
    info_x = SPACE;
    info_y = SPACE;
    name_y = info_y + info_height + SPACE;
    charinfo_y = name_y + info_height + SPACE;
    direct_y = charinfo_y + info_height + SPACE;
    
    max_button_w = font.maxbbx.w + 6;
    for (i=num_of_buttons; i--;)
	max_button_w = max(max_button_w, button[i].width);

    pixsize = ((info_width - 4*SPACE - grid - max_button_w) /
	font.maxbbx.w) - grid;
    if (psize)
	pixsize = psize;
    einc = pixsize + grid;
    ex1 = ey1 = 2 * SPACE;
    ex2 = ex1 + font.maxbbx.w * einc;
    ey2 = ey1 + font.maxbbx.h * einc;
    edit_width = einc * font.maxbbx.w + grid + 3 * SPACE; 
    edit_height = einc * font.maxbbx.h + grid + 3 * SPACE;
    xpos = info_x + edit_width + SPACE; 
    ypos = direct_y + direct_height + SPACE;
    info_width = max(info_width, edit_width + 3*SPACE + max_button_w);

    xswa.win_gravity = UnmapGravity;
    xswa.event_mask = ExposureMask;
    xswa.background_pixel = bgPixel;
    xswa.border_pixel = bdPixel;
    xswa.cursor = crosshair_cursor;
    infoWin = XCreateWindow(dpy, mainWin, info_x, info_y, info_width,
	info_height, 1, CFP3, CWMASK, &xswa);

    nameWin = XCreateWindow(dpy, mainWin, info_x, name_y, info_width,
	info_height, 1, CFP3, CWMASK, &xswa);

    charinfoWin = XCreateWindow(dpy, mainWin, info_x, charinfo_y,
	info_width, info_height, 1, CFP3, CWMASK, &xswa);

    xswa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask;
    xswa.cursor = cross_cursor;
    directWin = XCreateWindow(dpy, mainWin, info_x, direct_y,
	info_width, direct_height, 1, CFP3, CWMASK, &xswa);
    directBar = XCreateBitmapFromData(dpy, directWin, bar3_bits, bar3_width,
					bar3_height);

    xswa.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
	LeaveWindowMask;
    for (i=0; i<num_of_buttons; i++) {
	register struct Button *but = &button[i];
	but->win = XCreateWindow(dpy, mainWin, xpos, ypos, but->width,
		but->height, 1, CFP3, CWMASK, &xswa);
	but->pix = XCreateBitmapFromData(dpy, but->win, but->bits,
		but->width, but->height);
	ypos += but->height + SPACE;
    }
    xswa.cursor = crosshair_cursor;
    xswa.event_mask = ExposureMask;
    actual_width = font.maxbbx.w + 6;
    actual_height = font.maxbbx.h + 6;
    actualWin = XCreateWindow (dpy, mainWin, xpos, ypos, actual_width,
	actual_height, 1, CFP3, CWMASK, &xswa);
    if (actual_width * 2 + SPACE > max_button_w)
	ypos += actual_height + SPACE;
    else
	xpos += actual_width + SPACE;
    inverseWin = XCreateWindow (dpy, mainWin, xpos, ypos, actual_width,
	actual_height, 1, CFP3, CWMASK, &xswa);
    inverse_y = ypos;

    xswa.cursor = cross_cursor;
    xswa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask |
	Button1MotionMask | Button2MotionMask | Button3MotionMask;
    edit_y = direct_y + direct_height + SPACE;
    editWin = XCreateWindow(dpy, mainWin, info_x, edit_y, edit_width,
	edit_height, 0, CFP3, CWMASK, &xswa);

    downWin = XCreateWindow(dpy, editWin, 0, 0, edit_width, 2 * SPACE,
	0, CFP3, CWMASK, &xswa);
    downPtr = XCreateBitmapFromData(dpy, directWin,
	down3_bits, down3_width, down3_height);

    rightWin = XCreateWindow(dpy, editWin, 0, 0, 2 * SPACE, edit_height,
	0, CFP3, CWMASK, &xswa);
    rightPtr = XCreateBitmapFromData(dpy, rightWin,
	right3_bits, right3_width, right3_height);

}

layout2(w, h)
int w, h;
{
    int i, xpos, ypos, horiz_size, vert_size, newsize;
    XWindowChanges xwc;
    w = max(xsh.min_width, (main_width = w));
    h = max(xsh.min_height, (main_height = h));
    horiz_size = ((w - 6 * SPACE - grid - max_button_w) / font.maxbbx.w) - grid;
    vert_size = ((h - 5 * SPACE - direct_y - direct_height - grid) /
	 font.maxbbx.h) - grid;
    newsize = min(horiz_size, vert_size);
    if (psize) {
	newsize = psize;
        psize = 0;
    }
    if (newsize != pixsize) {
	einc = newsize + grid;
	edit_width = einc * font.maxbbx.w + grid + 3 * SPACE; 
    }
    info_width = edit_width + SPACE + max_button_w;
    if (info_width < max_text_w)
	info_width = min(max_text_w, w - 2*SPACE);
    XResizeWindow(dpy, infoWin, info_width, info_height);
    XResizeWindow(dpy, nameWin, info_width, info_height);
    XResizeWindow(dpy, charinfoWin, info_width, info_height);
    XResizeWindow(dpy, directWin, info_width, direct_height);
    if (newsize == pixsize) return;
    ex1 = ey1 = 2 * SPACE;
    ex2 = ex1 + font.maxbbx.w * einc;
    ey2 = ey1 + font.maxbbx.h * einc;
    edit_height = einc * font.maxbbx.h + grid + 3 * SPACE;
    pixsize = newsize;
    XResizeWindow(dpy, editWin, edit_width, edit_height);
    XResizeWindow(dpy, downWin, edit_width, downw_height);
    XResizeWindow(dpy, rightWin, rightw_width, edit_height);
    direct_bar=charIndex * (info_width - bar_width) /
		font.nchars + (bar_width >> 1);
    xpos = info_x + edit_width + SPACE;
    ypos = direct_y + direct_height + SPACE;
    for (i=0; i<num_of_buttons; ypos += button[i].height + SPACE, i++) 
	XMoveWindow(dpy, button[i].win, xpos, ypos);
    XMoveWindow(dpy, actualWin, xpos, ypos);
    if (actual_width * 2 + SPACE > max_button_w)
	ypos += actual_height + SPACE;
    else
	xpos += actual_width + SPACE;
    XMoveWindow(dpy, inverseWin, xpos, ypos);
}

