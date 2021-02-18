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
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Viewport.h>
#include <X11/List.h>

#include <Xw/Xw.h>
#include <Xw/ScrollBar.h>
#include <Xw/List.h>
#include <Xw/Valuator.h>
#include <Xw/SWindow.h>

#include "xstuff.h"
#include "xstuff2.h"

Widget NewMakeVPort(shellName,parent,scrollType,vscrlEv,hscrlEv,grain,
				client_data,
				x,y,width,height)
char		*shellName;
Widget		parent;
A_Scroll_t	scrollType;
XtCallbackProc	vscrlEv,hscrlEv;
caddr_t		client_data;
int		grain;
int		x,y;
int		width,height;
{
Widget		shellWind;
Arg		argList[15];
Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	XtSetArg(argList[i],XtNgranularity,grain); i++;
	XtSetArg(argList[i],XtNinitialDelay,1000); i++;
	XtSetArg(argList[i],XtNrepeatRate,500); i++;
	switch (scrollType)
	{
		case NOSCROLL:
			XtSetArg(argList[i],XtNforceVerticalSB,FALSE); i++;
			XtSetArg(argList[i],XtNforceHorizontalSB,FALSE); i++;
			break;
		case VERTONLY:
			XtSetArg(argList[i],XtNforceVerticalSB,TRUE); i++;
			XtSetArg(argList[i],XtNforceHorizontalSB,FALSE); i++;
			break;
		case HORIZONLY:
			XtSetArg(argList[i],XtNforceVerticalSB,FALSE); i++;
			XtSetArg(argList[i],XtNforceHorizontalSB,TRUE); i++;
			break;
		case VERTHORIZ:
			XtSetArg(argList[i],XtNforceVerticalSB,TRUE); i++;
			XtSetArg(argList[i],XtNforceHorizontalSB,TRUE); i++;
			break;
	}

	shellWind = XtCreateManagedWidget(shellName,
				XwswindowWidgetClass,parent,argList,i);
	if (vscrlEv)
		XtAddCallback(shellWind,"vScrollEvent",vscrlEv,client_data);
	if (hscrlEv)
		XtAddCallback(shellWind,"hScrollEvent",hscrlEv,client_data);
	return(shellWind);
}


Widget NewMakeListSimple(shellName,vportWind,ncols,callBackProc,
			client_data,strings,x,y,width,height)
char		*shellName;
Widget		*vportWind;
int			ncols;
XtCallbackProc	callBackProc;
caddr_t		client_data;
char		*strings[];
int			x,y;
int			width,height;
{
	Widget		listWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNlist,strings);	i++;
	XtSetArg(argList[i],XtNborderWidth,0); i++;
	XtSetArg(argList[i],XtNdefaultColumns,ncols);	i++;
	XtSetArg(argList[i],XtNforceColumns,True);	i++;
	listWind = XtCreateManagedWidget(shellName,
			listWidgetClass, *vportWind, argList, i);
						
	if (callBackProc)
		XtAddCallback(listWind,"callback",callBackProc,client_data);

	return(listWind);
}

Widget NewMakeList(shellName,parent,vportWind,scrollType,ncols,grain,
			vscrlEv,hscrlEv,callBackProc,client_data,
			strings,x,y,width,height)
char		*shellName;
Widget		parent;
Widget		*vportWind; /* returned */
A_Scroll_t	scrollType;
char		*strings[];
int		ncols,grain;
XtCallbackProc	callBackProc,vscrlEv,hscrlEv;
caddr_t		client_data;
int		x,y;
int		width,height;
{
Widget		listWind;

	*vportWind = NewMakeVPort("XTDvport",parent,scrollType,
				 vscrlEv,hscrlEv,grain,client_data,
				 x,y,width,height);

	listWind = NewMakeListSimple(shellName,vportWind,
				 ncols,callBackProc,
				 client_data,strings,1,1,width,height);

	return(listWind);
}


Widget NewMakeListStatic(shellName,parent,vportWind,ncols,callBackProc,
			client_data,strings,x,y,width,height)
char		*shellName;
Widget		parent;
Widget		*vportWind;
char		*strings[];
int		ncols;
XtCallbackProc	callBackProc;
caddr_t		client_data;
int		x,y;
int		width,height;
{
Widget		listWind;

	*vportWind = Make_Board(shellName,parent,x,y,width,height);

	listWind = NewMakeListSimple(shellName,vportWind,
				 ncols,callBackProc,
				 client_data,strings,1,1,width,height);

	return(listWind);
}
