/* file xtalk.c

	Written by : Yaser Doleh
	Kent State University
	email doleh@kent.edu
	March 15, 1980
*/
	
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Cardinals.h>
#include <X11/AsciiText.h>
#include <X11/Box.h>
#include <X11/Command.h>
#include <X11/Label.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <stdio.h>

#define	MAXSIZE	4096
#define WIDTH	500
#define	HEIGHT	200

XtAppContext	xtalk;
Display		*dpy1, *dpy2;
Widget	toplevel1, toplevel2, top1, top2, box1, box2, send1, send2, rec1, rec2;
char	sbuf1[MAXSIZE], sbuf2[MAXSIZE], rbuf1[MAXSIZE], rbuf2[MAXSIZE];

static XrmOptionDescRec options[] = {
    { "-geometry",	"xtalk-originator.geometry", XrmoptionSepArg, NULL }
};

static void xtalk_exit()
{
	exit(0);
}

static void Beep(w, event, params, nparams)
Widget w;
XEvent *event;
String *params;
Cardinal *nparams;
{
	XBell(XtDisplay(w), 0);
}
 
main(argc,argv)
int	argc;
char	*argv[];
{
	Arg	args[10];
	int	num_args;
	static XtCallbackRec	callback[]={
				{ xtalk_exit, NULL },
				{ NULL, NULL },
			};

	XtTranslations	trans;
	static char trans_string[] = "<Key>: beep()\n";
	
	static XtActionsRec actionsList[] = {
		{ "beep", Beep },
	};
	char title[128];
	char *userName = (char*)getenv("USER");
	
	XtToolkitInitialize();
	xtalk = XtCreateApplicationContext();
	dpy1 = XtOpenDisplay(xtalk, NULL, NULL, "XTalk",
			     options, XtNumber(options), &argc, argv);
	if (dpy1 == NULL) {
	    fprintf( stderr, "%s: couldn't open local display\n", argv[0] );
	    exit( 0 );
	}
	if (argc == 2) {
	    dpy2 = XtOpenDisplay(xtalk, argv[1], NULL, "XTalk",
				 options, XtNumber(options), &argc, argv);
	    if (dpy2 == NULL) {
		fprintf( stderr, "%s: couldn't open remote display %s\n",
			 argv[0], argv[1] );
		exit( 0 );
	    }
	}
	else {
	    fprintf( stderr, "Usage: %s other-display [toolkitoptions]\n",
		     argv[0] );
	    exit( 0 );
	}

	toplevel1 = XtAppCreateShell("xtalk", "XTalk", applicationShellWidgetClass,
			dpy1, NULL, 0);

	toplevel2 = XtAppCreateShell("xtalk", "XTalk", applicationShellWidgetClass,
			dpy2, NULL, 0);

	sprintf( title, "xtalk to %.32s", DisplayString(dpy2) );
	XtSetArg( args[0], XtNtitle, title );
	top1 = XtCreatePopupShell("xtalk-originator", topLevelShellWidgetClass,
				  toplevel1, args, ONE);

	sprintf( title, "xtalk from %.32s at %.32s",
		 (userName != NULL) ? userName : "unknown user",
		 DisplayString(dpy1)
		);
	top2 = XtCreatePopupShell("xtalk-recipient", topLevelShellWidgetClass,
				  toplevel2, args, ONE);

	box1 = XtCreateManagedWidget("box1", boxWidgetClass, top1, NULL, 0);
	box2 = XtCreateManagedWidget("box2", boxWidgetClass, top2, NULL, 0);
	XtSetArg( args[0], XtNlabel, DisplayString(dpy2) );
	XtCreateManagedWidget("user1", labelWidgetClass, box1, args, ONE);
	sprintf( title, "%.32s@%.32s",
		 (userName != NULL) ? userName : "unknown user",
		 DisplayString(dpy1)
		);
	XtSetArg( args[0], XtNlabel, title );
	XtCreateManagedWidget("user2", labelWidgetClass, box2, args, ONE);
	XtSetArg( args[0], XtNlabel, " Place pointer in top window and type ");
	XtCreateManagedWidget("label1", labelWidgetClass, box1, args, 1);
	XtCreateManagedWidget("label2", labelWidgetClass, box2, args, 1);
	
	XtSetArg( args[0], XtNlabel, " EXIT ");
	XtSetArg( args[1], XtNcallback, callback);
	XtCreateManagedWidget("exit1", commandWidgetClass, box1, args, 2);
	XtCreateManagedWidget("exit2", commandWidgetClass, box2, args, 2);
	
	XtSetArg( args[0], XtNstring, sbuf1); 
	XtSetArg( args[1], XtNwidth, WIDTH);
	XtSetArg( args[2], XtNheight, HEIGHT); 
	XtSetArg( args[3], XtNeditType, XawtextEdit);  
	XtSetArg( args[4], XtNlength, MAXSIZE);
	send1 = XtCreateManagedWidget("send1", asciiStringWidgetClass, 
				box1, args, FIVE);
	
	XtSetArg( args[0], XtNstring, rbuf1);
	rec1 = XtCreateManagedWidget("rec1", asciiStringWidgetClass, 
				box1, args, FIVE);
	
	XtSetArg( args[0], XtNstring, sbuf2);
	send2 = XtCreateManagedWidget("send2", asciiStringWidgetClass, 
				box2, args, FIVE);
	
	XtSetArg( args[0], XtNstring, rbuf2);
	rec2 = XtCreateManagedWidget("rec2", asciiStringWidgetClass, 
				box2, args, FIVE);
			
	{
	    /* work around bug in XmuCvtStringToCursor */
	    Cursor cursor1, cursor2;
	    Arg newArgs[1];
	    Boolean doneSend = False, doneRcv = False;
	    XtSetArg(newArgs[0], XtNcursor, XCreateFontCursor(dpy2, XC_xterm));
	    XtSetArg( args[0], XtNcursor, &cursor1 );
	    XtGetValues( send1, args, ONE );
	    XtSetArg( args[0], XtNcursor, &cursor2 );
	    XtGetValues( send2, args, ONE );
	    if (cursor1 == cursor2) {
		XtSetValues( send2, newArgs, ONE );
		doneSend = True;
	    }
	    XtGetValues( rec2, args, ONE );
	    if (cursor1 == cursor2) {
		XtSetValues( rec2, newArgs, ONE );
		doneRcv = True;
	    }
	    if (!doneSend || !doneRcv) {
		XtGetValues( rec1, args, ONE );
		if (cursor1 != cursor2) {
		    XtSetArg( args[0], XtNcursor, &cursor1 );
		    if (!doneSend) {
			XtGetValues( send2, args, ONE );
			if (cursor1 == cursor2)
			    XtSetValues( send2, newArgs, ONE );
		    }
		    if (!doneRcv) {
			XtGetValues( rec2, args, ONE );
			if (cursor1 == cursor2)
			    XtSetValues( rec2, newArgs, ONE );
		    }
		}
	    }
	}
	XtRealizeWidget(top1);
	XtRealizeWidget(top2);
	
	XtAppAddActions(xtalk, actionsList, XtNumber(actionsList));
	trans = XtParseTranslationTable(trans_string);
	XtOverrideTranslations(rec1, trans);
	XtOverrideTranslations(rec2, trans);

	XtPopup(top1, XtGrabNone);
	XtPopup(top2, XtGrabNone);

	while(1) {
		XEvent	event;
		XawTextBlock	text;
		
		XtAppNextEvent(xtalk, &event);
		XtDispatchEvent(&event);
		if (event.type == KeyPress
		    || event.type == KeyRelease
		    || event.type == ButtonPress
		    || event.type == ButtonRelease)
		    {
		    text.firstPos = 0;
		    text.ptr = sbuf1;
		    text.length = strlen(sbuf1);
		    XawTextReplace(rec2, 0, text.length, &text);
		    text.firstPos = 0;
		    text.ptr = sbuf2;
		    text.length = strlen(sbuf2);
		    XawTextReplace(rec1, 0, text.length, &text);
		}
	}
	
	
	
}
