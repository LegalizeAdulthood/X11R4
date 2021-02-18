/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Shell.h>

#include "xstuff.h"
#include "ximage.icon"

extern Widget toplevel;
Widget	b[50];			/* Buttons */

static	char whoWroteMe[]="\nNCSA XImage written by Dave Thompson\nContour&Shade routines written by Dave Whittington\n";

main(argc,argv)
int	argc;
char	*argv[];
{
        Arg             argList[2];

        Pixmap  iconPixmap;


	Initialize(argc,argv);

	XtRealizeWidget(toplevel);

	Set_Pointer(toplevel,XC_gumby);

        iconPixmap = XCreateBitmapFromData(XtDisplay(toplevel),
                XtWindow(toplevel), XIMAGE_ICON, XIMAGE_WIDTH, XIMAGE_HEIGHT);
        XtSetArg(argList[0],XtNiconPixmap, iconPixmap);
        XtSetValues(toplevel, argList, 1);

	XtMainLoop();
}

void Quit(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	ReleaseAllImages();
	ClosePaletteBox();
	ReleaseAllAnimations();
	exit(0);
}
