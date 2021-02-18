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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Box.h>
#include <Xw/BBoard.h>

#include "xstuff.h"



Widget toplevel;	/* The top dude of everything ...really everything..*/
Widget toplevelform;	/* The top composite widget, parent of all the shells*/

extern void ETDirReturn();
XtActionsRec XIactionTable[] = {{"ETDirReturn",ETDirReturn}};


Initialize(argc,argv)
int argc;
char *argv[];
{
Arg		arglist[10];
Cardinal	i=0;
char		buff[128];
char		buff2[256];
Display		*dpy;
int		screen;
Visual		*myVis;


	toplevel = XtInitialize(argv[0],"XImageTool",NULL,0,&argc,argv);
	toplevelform = XtCreateManagedWidget(argv[0],XwbulletinWidgetClass,
					toplevel,arglist,i);

	XtAddActions(XIactionTable,XtNumber(XIactionTable));

	dpy    = XtDisplay(toplevel);
	screen = DefaultScreen(dpy);

	if (! InitImage(dpy))
		exit(0);
	if (! InitPalette(dpy))
		exit(0);
	if (! InitPalEntry(dpy))
		exit(0);
	if (! InitGetAFile(dpy))
		exit(0);
	if (! InitData(dpy))
		exit(0);
	if (! InitAnimation(dpy))
		exit(0);

	{
	char *garbage;
	garbage = (char *) XIMalloc(1025);
	XIFree(garbage);
	}

	Init_Ctrl_Window(toplevelform);

	/* Print the Display Info*/

  if (8 == DisplayPlanes(dpy,screen)) /* perfectionism at it's wosrt*/
    sprintf(buff,"Display is an %d plane %%s with %d entries in Colormap\n",
			DisplayPlanes(dpy,screen),XDisplayCells(dpy,screen));
  else
    sprintf(buff,"Display is a %d plane %%s with %d entries in Colormap\n",
			DisplayPlanes(dpy,screen),XDisplayCells(dpy,screen));

	myVis = GetVisual(dpy);
	switch (myVis->class) {
		case StaticGray:
			sprintf(buff2,buff,"StaticGray");
			break;
		case GrayScale:
			sprintf(buff2,buff,"GrayScale");
			break;
		case StaticColor:
			sprintf(buff2,buff,"StaticColor");
			break;
		case PseudoColor:
			sprintf(buff2,buff,"PseudoColor");
			break;
		case TrueColor:
			sprintf(buff2,buff,"TrueColor");
			break;
		case DirectColor:
			sprintf(buff2,buff,"DirectColor");
			break;
		}
	PrintMesg(buff2);
	PrintMesg("Done initializing NCSA X Image.\n");

} /* Initialize() */

