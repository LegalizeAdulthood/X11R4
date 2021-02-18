#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XWDFile.h>
#include <X11/cursorfont.h>
#include <sys/uio.h>
#include <pwd.h>

extern Display *dpy;
extern XColor *color;
extern XWindowAttributes win_info;

extern XImage *transform();
extern int debug;
extern int reverse;
