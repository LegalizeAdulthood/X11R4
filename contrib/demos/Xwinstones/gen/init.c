/* Copyright International Business Machines, 1989
 *
 *
 *
 *  IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
 *  PARTICULAR USE.  IN NO EVENT SHALL
 *  IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 *  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 *  SOFTWARE
 *
 *
 *
*/
#include "stones.h"

int  fg, bg;

int
init(ops)
optionsPtr ops;
{
XFontStruct *fontstruct;
Window window;
Display *display;
XGCValues gcv;
XSetWindowAttributes xswa;
XColor installedcolor[1], databasecolor[1];

display = ops->display;

if ((fontstruct = XLoadQueryFont(display, ops->font))==(0))
{
	printf("%s: Cannot find font %s\n", prog, ops->font);
	return(-1);
}

switch (ops->dVisual->class) 
   {
   case StaticColor:
   case PseudoColor:
   case TrueColor:
   case DirectColor:
	GETCOLOR(bg, ops->bgtext, BlackPixel);
	GETCOLOR(fg, ops->fgtext, WhitePixel);
	break;
   case StaticGray:
   case GrayScale:
   default:
	bg= BlackPixel(display,ops->dScreen);
	fg= WhitePixel(display,ops->dScreen);
	break;
   } /* end switch */

xswa.event_mask = MapNotify | ExposureMask | StructureNotifyMask;
xswa.background_pixel = bg;
xswa.border_pixel = fg;
xswa.override_redirect = True;    /* override redirection thru window manager*/
xswa.override_redirect = False;   /* lets window manger control placement */
window = XCreateWindow(display, DefaultRootWindow(display),
	ops->winX, ops->winY, ops->winW, ops->winH, ops->winBW,
	DefaultDepth(display, ops->dScreen),
	InputOutput, DefaultVisual(display, ops->dScreen),
	CWOverrideRedirect | CWEventMask | CWBackPixel | CWBorderPixel, &xswa);

XChangeProperty(display, window, XA_WM_NAME, XA_STRING, 8,
	PropModeReplace, prog, strlen(prog) );
XStoreName(display,window,prog);
XSetIconName(display,window,prog);
XSelectInput(display, window, MapNotify | ExposureMask | StructureNotifyMask);

gcv.font = fontstruct->fid;

GETGC(ops->alu, fg, bg, ops->fggc);
GETGC(ops->alu, bg, fg, ops->bggc);

XWarpPointer( ops->display, None, RootWindow( ops->display, ops->dScreen),
	0, 0, 0, 0, ops->cursX, ops->cursY);
XMapRaised(display,window);
firstExposeExpected = 1 ;
XSync(display, 0);

ops->window = window;
return(1);
}
