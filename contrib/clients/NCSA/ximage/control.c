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
#include <X11/AsciiText.h>
#include <X11/cursorfont.h>

#include "xstuff.h"
#include "buttons.h"
#include "gr_def.h"
#include "gr_var.h"

#define MAIN_CTRL_DIM_X
#define MAIN_CTRL_DIM_Y
#define MSGBUFFSIZE	4096

extern void Quit();
extern void ImageBox();
extern void CBNewImage();
extern void PaletteBox();
extern void GetAFile();
extern void CBDrawSDSBox();
extern void CBNewAnimate();
extern void CBTestNotebook();


static	Widget	msg_window;
static	char msgbuff[MSGBUFFSIZE];
static	msgWinCreated = FALSE;

Init_Ctrl_Window(parent)
Widget parent;
{
Widget  ctrl_window;
Widget	buttonbox;
int	x;


	ctrl_window = Make_Window(GetVersion(),
				parent,0,0,402,200); 
	
	for(x=0; x< MSGBUFFSIZE; x++)
		msgbuff[x]=' ';
	for(x=0; x < MSGBUFFSIZE; x+=10)
		msgbuff[x]='\n';

	msgbuff[MSGBUFFSIZE-1]='\0';

/*	msg_window = Make_Text(ctrl_window,1,22,400,80,
				msgbuff);
*/

	msg_window = (Widget) gr_MakeText("LogWindow",ctrl_window,VERTONLY,
                                        SELECTABLE,STRINGSOURCE,
                                        msgbuff,
                                        1, 22, 400, 80);
	msgWinCreated = TRUE;
PrintMesg("This software is a product of the National Center for \n");
PrintMesg("Supercomputing Applications at the University of Illinois\n");
	PrintMesg("Please send bug reports to davet@ncsa.uiuc.edu\n");
	PrintMesg(GetVersionDate());

        Set_Pointer(msg_window,XC_gumby);
	buttonbox = Make_ButtonBox(ctrl_window,1,105,400,40);
	b[CTRL_IMAGE]	= MakeButton("Image",buttonbox,1,1,
				CBNewImage,NULL,0,0);
	b[CTRL_PALETTE]	= MakeButton("Palette",buttonbox,45,1,
				PaletteBox,NULL,0,0);
	b[CTRL_ANIMATE] = MakeButton("Animate",buttonbox,101,1,
				CBNewAnimate,NULL,0,0);
#ifdef UNDER_DEVELOPMENT
	b[CTRL_NOTEBOOK]= MakeButton("Notebook",buttonbox,157,1,
				CBTestNotebook,NULL,0,0);
/*	(void) MakeButton("Test",buttonbox,250,1,CBDrawSDSBox,NULL,0,0);*/
#endif
	b[CTRL_QUIT]	= MakeButton("Quit",buttonbox,365,1,Quit,NULL,0,0);
}

PrintMesg(s)
char *s;
{
static int ptr = 0;
int i;
XtTextBlock text;

int slength = strlen(s);

	if (!msgWinCreated){
		printf("%s",s);
		return;
		}
	if((strlen(s) + ptr) > MSGBUFFSIZE) {
		/* reset message buffer */
		int x;
		ptr = 0;
		for(x=0; x< MSGBUFFSIZE; x++)
			msgbuff[x]=' ';
		for(x=0; x < MSGBUFFSIZE; x+=10)
			msgbuff[x]='\n';
		msgbuff[MSGBUFFSIZE-1]='\0';
		}
	strncpy((msgbuff+ptr),s,slength); /* copy mesg except for null */

	text.ptr = s;
	text.firstPos = 0;
	text.length = slength;
	text.format = FMT8BIT;
	i = XtTextReplace(msg_window,ptr,ptr+slength,&text);
	XtTextDisplay(msg_window);
	XtTextSetInsertionPoint(msg_window,ptr);
/*XtTextSetInsertionPoint(msg_window,XtTextGetInsertionPoint(msg_window));*/
	ptr = ptr + slength;
}
