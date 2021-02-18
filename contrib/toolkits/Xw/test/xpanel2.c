
/*
 * To compile this program type:
 *		cc -Wc,-Nd2000 -Wc,-Ns2000 -o xpanel xpanel.c -lXw -lXt -lX11
 */

#include <signal.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/TitleBar.h>
#include <Xw/PButton.h>
#include <Xw/RCManager.h>
#include <Xw/SText.h>
#include <Xw/Panel.h>

void LoadPanel();
void LoadTitleBar();

Widget	pnl;
Widget	tbar, pdn, wsp;
Widget	toplevel;

#define	MAXKIDS 20

Widget	tbarKids[MAXKIDS];
int	numTbarKids = 0;

Widget	pdnKids[MAXKIDS];
int	numPdnKids = 0;

Widget	wspKids[MAXKIDS];
int	numWspKids = 0;

void myQuit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}

void main (argc,argv)
	unsigned int	argc;
	char	*argv[];
{
	char *title_str;
	Arg	myargs[10];

    signal (SIGINT, myQuit);

	toplevel = XtInitialize(argv[0],"Xtest",NULL,0,&argc,argv);

	title_str = NULL;
	LoadPanel(title_str);

	LoadTitleBar();

	XtRealizeWidget(toplevel);

	XtMainLoop();
}

void LoadPanel()
{
	Arg	myargs[10];

	XtSetArg(myargs[0],XtNwidth,175);
	XtSetArg(myargs[1],XtNheight,175);
	pnl = XtCreateManagedWidget("Panel",
		XwpanelWidgetClass, toplevel, (ArgList) myargs, 0);

	XtSetArg(myargs[0],XtNwidgetType,XwTITLE);
	tbar = XtCreateManagedWidget("TitleBar",
		XwtitlebarWidgetClass, pnl, (ArgList) myargs, 1);

	XtSetArg(myargs[0],XtNwidgetType,XwPULLDOWN);
	XtSetArg(myargs[1],XtNlayoutType,XwMAXIMUM_COLUMNS);
	XtSetArg(myargs[2],XtNsingleRow,TRUE);
	pdn = XtCreateManagedWidget("PullDown",
		XwrowColWidgetClass, pnl, (ArgList) myargs, 3);

	XtSetArg(myargs[0], XtNlayout, XwMINIMIZE);
	XtSetArg(myargs[1], XtNwidgetType, XwWORK_SPACE);
	wsp = XtCreateManagedWidget("WorkSpace",XwrowColWidgetClass,
		pnl, (ArgList) myargs, 2);
	
}

static Widget quit_btn;
static Widget add_to_pdown_btn, add_to_wsp_btn;
static Widget del_from_pdn_btn, del_from_wsp_btn;

void QuitCB();
void AddPulldownCB();
void AddWorkSpaceCB();
void DelPulldownCB();
void DelWorkSpaceCB();

void LoadTitleBar()
{
	Arg	myargs[10];

	XtSetArg(myargs[0], XtNlabel, "Quit");
	XtSetArg(myargs[1], XtNregion, XwALIGN_LEFT);
	quit_btn = XtCreateManagedWidget("Q",
		XwpushButtonWidgetClass,tbar,myargs,2);
	XtAddCallback(quit_btn, XtNselect, QuitCB, NULL);
	
	XtSetArg(myargs[0], XtNlabel, "P+");
	XtSetArg(myargs[1], XtNregion, XwALIGN_LEFT);
	add_to_pdown_btn = XtCreateManagedWidget("P+",
		XwpushButtonWidgetClass,tbar,myargs,2);
	XtAddCallback(add_to_pdown_btn, XtNselect, AddPulldownCB, NULL);

	XtSetArg(myargs[0], XtNlabel, "W+");
	XtSetArg(myargs[1], XtNregion, XwALIGN_LEFT);
	add_to_wsp_btn = XtCreateManagedWidget("W+",
		XwpushButtonWidgetClass,tbar,myargs,2);
	XtAddCallback(add_to_wsp_btn, XtNselect, AddWorkSpaceCB, NULL);

	XtSetArg(myargs[0], XtNlabel, "P-");
	XtSetArg(myargs[1], XtNregion, XwALIGN_LEFT);
	del_from_pdn_btn = XtCreateManagedWidget("P-",
		XwpushButtonWidgetClass,tbar,myargs,2);
	XtAddCallback(del_from_pdn_btn, XtNselect, DelPulldownCB, NULL);

	XtSetArg(myargs[0], XtNlabel, "W-");
	XtSetArg(myargs[1], XtNregion, XwALIGN_LEFT);
	del_from_wsp_btn = XtCreateManagedWidget("W-",
		XwpushButtonWidgetClass,tbar,myargs,2);
	XtAddCallback(del_from_wsp_btn, XtNselect, DelWorkSpaceCB, NULL);

	numTbarKids = 5;
}

void QuitCB()
{
	printf("Leaving Panel Test Application\n");
	exit(0);
}

void AddPulldownCB()
{
	char kid_label[30];

	if (numPdnKids == MAXKIDS)
	{
		printf("Pulldown full\n");
		return;
	}

	sprintf(kid_label,"P %d",numPdnKids);
	pdnKids[numPdnKids] = XtCreateManagedWidget(kid_label,
		XwpushButtonWidgetClass,pdn,NULL,0);
	
	numPdnKids++;
}

void AddWorkSpaceCB()
{
	char kid_label[30];

	if (numWspKids == MAXKIDS)
	{
		printf("WorkSpace full\n");
		return;
	}

	sprintf(kid_label,"W %d",numWspKids);
	wspKids[numWspKids] = XtCreateManagedWidget(kid_label,
		XwpushButtonWidgetClass,wsp,NULL,0);
	
	numWspKids++;
}

void DelPulldownCB()
{

	if (numPdnKids == 0)
	{
		printf("Pulldown empty\n");
		return;
	}

	numPdnKids--;

	XtDestroyWidget(pdnKids[numPdnKids]);
}

void DelWorkSpaceCB()
{

	if (numWspKids == 0)
	{
		printf("WorkSpace empty\n");
		return;
	}

	numWspKids--;

	XtDestroyWidget(wspKids[numWspKids]);
}
