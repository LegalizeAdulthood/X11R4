#include <stdio.h>
#include <signal.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/WorkSpace.h>



/*  Callback lists for the hotspot  */

extern void KeyCallback();
extern void SelectCallback();
extern void ReleaseCallback();
extern void ResizeCallback();
extern void ExposeCallback();

static XtCallbackRec keyCallback[] =
{
   { KeyCallback,    (caddr_t) NULL },
   { NULL,           (caddr_t) NULL },
};

static XtCallbackRec selectCallback[] =
{
   { SelectCallback, (caddr_t) NULL },
   { NULL,           (caddr_t) NULL },
};

static XtCallbackRec releaseCallback[] =
{
   { ReleaseCallback, (caddr_t) NULL },
   { NULL,            (caddr_t) NULL },
};

static XtCallbackRec resizeCallback[] =
{
   { ResizeCallback, (caddr_t) NULL },
   { NULL,           (caddr_t) NULL },
};

static XtCallbackRec exposeCallback[] =
{
   { ExposeCallback, (caddr_t) NULL },
   { NULL,           (caddr_t) NULL },
};




void main (argc, argv)
unsigned int argc;
char **argv;

{
    Widget toplevel, hotspot;
    Arg args[10];
   extern void myQuit();

   signal (SIGINT, myQuit);

    toplevel = XtInitialize (argv[0], "HOTSPOT1", NULL, 0, &argc, argv);

    XtSetArg (args[0], XtNselect, (XtArgVal) selectCallback);
    XtSetArg (args[1], XtNrelease, (XtArgVal) releaseCallback);
    XtSetArg (args[2], XtNexpose, (XtArgVal) exposeCallback);
    XtSetArg (args[3], XtNresize, (XtArgVal) resizeCallback);
    XtSetArg (args[4], XtNkeyDown, (XtArgVal) keyCallback);

    hotspot = XtCreateWidget ("hotspot",  XwworkSpaceWidgetClass, 
                              toplevel, (ArgList) args, 5);
    XtManageChild (hotspot);
    XtRealizeWidget (toplevel);
    XtMainLoop ();
}


void KeyCallback (hw, closure, call_data)
XwWorkSpaceWidget hw;
caddr_t         closure;
caddr_t         call_data;

{
   printf ("Key Down Callback Called\n");
}

void SelectCallback (hw, closure, call_data)
XwWorkSpaceWidget hw;
caddr_t         closure;
caddr_t         call_data;

{
   printf ("Select Callback Called\n");
}


void ReleaseCallback (hw, closure, call_data)
XwWorkSpaceWidget hw;
caddr_t         closure;
caddr_t         call_data;

{
   printf ("Release Callback Called\n");
}



void ResizeCallback (hw, closure, call_data)
XwWorkSpaceWidget hw;
caddr_t         closure;
caddr_t         call_data;

{
   printf ("Resize Callback Called\n");
}


void ExposeCallback (hw, closure, call_data)
XwWorkSpaceWidget hw;
caddr_t         closure;
caddr_t         call_data;

{
   printf ("Expose Callback Called\n");
}



void myQuit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}
