/* xdefs.h */

extern int XLookupString();
extern int XSetFunction();
extern int XParseGeometry();
extern int XAllocColor();
extern int XBell();
extern int XChangeGC();
extern int XClearArea();
extern int XClearWindow();
extern int XCloseDisplay();
extern int XConnectionNumber();
extern int XCopyPlane();
extern int XDefineCursor();
extern int XDestroyWindow();
extern int XDisplayCells();
extern int XDrawImageString();
extern int XDrawLine();
extern int XDrawRectangle();
extern int XDrawString();
extern int XFillRectangle();
extern int XFillRectangles();
extern int XFlush();
extern int XFree();
extern int XFreeFontPath();
extern int XFreeGC();
extern int XFreePixmap();
extern int XGetWindowAttributes();
extern int XMapWindow();
extern int XNextEvent();
extern int XParseColor();
extern int XPending();
extern int XPixSet();
extern int XPutImage();
extern int XQueryPointer();
extern int XSelectInput();
extern int XSetBackground();
extern int XSetFontPath();
extern int XSetForeground();
extern int XSetNormalHints();
extern int XSetStandardProperties();
extern int XSetWMHints();
extern int XSetWindowBackground();
extern int XSetWindowBackgroundPixmap();
extern int XSetWindowBorder();
extern int XSetWindowBorderPixmap();
extern int XStoreName();
extern int XStoreName();
extern int XSync();
extern int XTextWidth();
extern int XUnmapWindow();
extern int XCopyArea();
extern int XSetClipMask();
extern int XSetClipOrigin();

/* 
 * X function declarations.
 */
Display *XOpenDisplay();

char *XFetchBytes();
char *XFetchBuffer();
char *XGetAtomName();
char *XGetDefault();
char *XDisplayName();
char *XKeysymToString();

int (*XSynchronize())();
int (*XSetAfterFunction())();
Atom XInternAtom();
Colormap XCopyColormapAndFree(), XCreateColormap();
Cursor XCreatePixmapCursor(), XCreateGlyphCursor(), XCreateFontCursor();
Font XLoadFont();
GC XCreateGC();
GContext XGContextFromGC();
Pixmap XCreatePixmap();
Pixmap XCreateBitmapFromData(), XCreatePixmapFromBitmapData();
Window XCreateSimpleWindow(), XGetSelectionOwner();
Window XCreateWindow(); 
Colormap *XListInstalledColormaps();
char **XListFonts(), **XListFontsWithInfo(), **XGetFontPath();
char **XListExtensions();
Atom *XListProperties();
XImage *XCreateImage(), *XGetImage(), *XGetSubImage();
XHostAddress *XListHosts();
KeySym XKeycodeToKeysym(), XLookupKeysym(), *XGetKeyboardMapping();
KeySym XStringToKeysym();
long XMaxRequestSize();
char *XResourceManagerString();
unsigned long XDisplayMotionBufferSize();
VisualID XVisualIDFromVisual();

/* routines for dealing with extensions */
XExtCodes *XInitExtension();
int (*XESetCreateGC())(), (*XESetCopyGC())(), (*XESetFlushGC())(),
    (*XESetFreeGC())(), (*XESetCreateFont())(), (*XESetFreeFont())(), 
    (*XESetCloseDisplay())(),
    (*XESetError())(), (*XESetErrorString())();
Bool (*XESetWireToEvent())();
Status (*XESetEventToWire())();
XExtCodes *XAddExtension();
XExtData *XFindOnExtensionList();
XExtData **XEHeadOfExtensionList();

/* these are routines for which there are also macros */
Window XRootWindow(), XDefaultRootWindow(), XRootWindowOfScreen();
Visual *XDefaultVisual(), *XDefaultVisualOfScreen();
GC XDefaultGC(), XDefaultGCOfScreen();
unsigned long XBlackPixel(), XWhitePixel(), XAllPlanes();
unsigned long XBlackPixelOfScreen(), XWhitePixelOfScreen();
unsigned long XNextRequest(), XLastKnownRequestProcessed();
char *XServerVendor(), *XDisplayString();
Colormap XDefaultColormap(), XDefaultColormapOfScreen();
Display *XDisplayOfScreen();
Screen *XScreenOfDisplay(), *XDefaultScreenOfDisplay();
long XEventMaskOfScreen();
