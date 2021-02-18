/* 
 * xnetload.c: multiple xload, using rwho statistics
 * $Date: 89/10/04 10:04:41 $ $Revision: 1.5 $
 *
 * Copyright 1989 University of Waterloo
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of UW not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  UW makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * UW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL UW
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Mike Berkley, jmberkley@watnext.waterloo.edu, 1989
 *
 */

/*
 * xnetload uses the Load widget load proc to get the local
 * machine load.  This means that xnetload needs to be setgid
 * kmem.  If you do not want this, then define NOLOCAL when compiling.
 */

#include <stdio.h>
#include <netdb.h>
#include <ctype.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Form.h>
#include <X11/Load.h>
#include "patchlevel.h"

void GetStatus();
extern int getload();         /* gets rwho load */
extern char *malloc();
extern char *strtok();
static Widget makestatuswidget(), makeremotestatus(), makesubform();
static void modifyloadcall();
static void SetRemUpdate();

#define DOWNMSG      " DOWN"
#define DEFREMUPDATE (60)	/* remote update defaults to 60 seconds */
#define BIGNUM (99999)		/* ~infinite number of load widgets */

/* arguments to Form widget */
static Arg Formargs[] = {
    { XtNfromVert, (XtArgVal) NULL },       /* Chain from .value wid */
    { XtNdefaultDistance, (XtArgVal) 1 },   /* default spacing */
    { XtNresizable, (XtArgVal) TRUE },      /* allow resize */
};

/* Arguments to load windows */
static Arg Statusargs[] = {
    { XtNfromHoriz, (XtArgVal) NULL },      /* Chain from .value wid */
    { XtNresizable, (XtArgVal) TRUE },      /* allow resize */
    { XtNborderWidth, (XtArgVal) 0 },       /* 0 border for load wid  */
    { XtNlabel, (XtArgVal) NULL },          /* Load title */
};

/* Application specific resources - turn off local load display */
static String Configuration;
static Boolean Localload;
static XtResource app_resources[] = {
    { "localload", "LocalLoad", XtRBoolean, sizeof(Boolean),
	(Cardinal) &Localload, XtRString,
/* if NOLOCAL is defined, then no local is displayed, by default */
#ifdef NOLOCAL
"off"
#else
"on"
#endif
},
  /* configuration of loads - horizontal, vertical or mXn/mxn */
  { "configuration", "Configuration", XtRString, sizeof(String),
      (Cardinal) &Configuration, XtRString, "horizontal" },
};

/* command line options for local load display and configuration */
/* If compiled NOLOCAL, then off is default, but on still allowed */
static XrmOptionDescRec app_options[] = {
    { "-local", "localload", XrmoptionNoArg, "on" },
    { "-nolocal", "localload", XrmoptionNoArg, "off" },
    { "-configuration", "configuration", XrmoptionSepArg, "vertical"},
};

main(argc,argv)
int argc;
char *argv[];
{
    int currarg;             /* arg counter to walk through hosts */
    int maincount,subcount,count;
    String maindirection,subdirection;
    static Widget toplevel, mainform, subform, statuswin, head;

    /* The toolkit tries to open your display if you use -help */
    /* so let's do it the other way */
    if((argc > 1) && !strncmp(argv[1],"-help",5)) {
        usage();
        exit(0);
    }

    /* Initialize the top level */
    toplevel = XtInitialize(*argv[0], "XNetload",
                            app_options, XtNumber(app_options), &argc, argv);

    /* Set remote load update to default to DEFREMUPDATE seconds */
    SetRemUpdate(toplevel);

    /* Get local vs nolocal, configuration */
    XtGetApplicationResources( toplevel, (caddr_t)NULL,
			      app_resources, XtNumber(app_resources),
			      NULL, (Cardinal) 0);
      
    /* Parse configuration */
    if(strncmp(Configuration,"horiz",5) == 0) {
	subcount = BIGNUM;	/* ~ inf num of widgets horizontally */
	maincount = 1;
    }
    else if(strncmp(Configuration,"vert",4) == 0) {
	maincount = BIGNUM;	/* ~ inf num of widgets horizontally */
	subcount = 1;
    }
    else {
	/* format is 4x5 for 4 columns, 5 rows */
	strtok(Configuration,"xX");
	if(Configuration[0] == '*')
	  subcount = BIGNUM;
	else if(! isdigit(Configuration[0])) {
	    usage();
	    exit(-1);
	}
	else if((subcount = atoi(Configuration)) == 0)
	  subcount = 1;
	
	Configuration = strtok((char *)NULL,"xX");
	if(Configuration[0] == '*')
	  maincount = BIGNUM;
	else if(! isdigit(Configuration[0])) {
	    usage();
	    exit(-1);
	}
	else if((maincount = atoi(Configuration)) == 0)
	  maincount = 1;
    }

    if(subcount >= maincount) {
	maindirection = XtNfromVert;
	subdirection = XtNfromHoriz;
    }
    /* need to swap so that bigger count is the sub count */
    /* This lowers the number of form widgets */
    else {
	maindirection = XtNfromHoriz;
	subdirection = XtNfromVert;
	/* swap counts */
	count = subcount;subcount = maincount;maincount = count;
    }
    /* adjust subcount so that we don't always get one row if we use Nx* */
    if(subcount == BIGNUM) {
	subcount = argc/maincount;
	if(maincount * subcount != argc)
	  subcount++;
    }

    /* Create a Form widget to contain all of the Loads */
    mainform =
      XtCreateManagedWidget("outerform",
                            formWidgetClass,
                            toplevel,
                            Formargs,
                            XtNumber(Formargs));

    /* Make the first sub-form widget, to contain one row/col */
    subform = makesubform(mainform,NULL,maindirection);

    /*
     * load widget reads kmem for local load.  Define NOLOCAL if you
     * don't want this.
     */
    count = 0;
    if(Localload) {
        /* Get and set label name */
        String labelname = malloc(BUFSIZ*sizeof(char));
        gethostname(labelname,BUFSIZ);
	strtok(labelname,".");

        /* Create the local load widget */
        statuswin =
	  makestatuswidget(labelname,subform,(Widget)NULL,
			   "local",subdirection);
	count++;		/* So local added into subform count */
    }

    /* Remote Loads */
    /* currarg initialized above */
    for(currarg = 1;(currarg < argc)&&(maincount > 0);maincount--) {
	for(;(currarg < argc)&&(count < subcount);count++,currarg++) {
	    if(count > 0) {
		/* Chain to first load */
		statuswin = makeremotestatus(argv[currarg],
					     subform,
					     statuswin,
					     subdirection);
	    }
	    else {
		/* chain to left/top edge of subform */
		statuswin = makeremotestatus(argv[currarg],
					     subform,
					     NULL,
					     subdirection);
	    }
	}
	count = 0;
	/* make another subform if there are more machines */
	if((currarg < argc) && (maincount > 1))
	  subform = makesubform(mainform,subform,maindirection);
    }

    /* if there are no loads to display, then should stop   */
    /* Note, statuswin is static, guaranteed to start at 0, */
    /* so it's non-zero only if a load widget was created   */
    if(!statuswin) {
        fprintf(stderr,"xnetload: No machines to display\n");
        usage();
        exit(0);
    }

    /* draw the widgets, and look for new events */
    XtRealizeWidget(toplevel);
    XtMainLoop();
}

/*
 * Make a sub form widget
 */
static Widget
makesubform(formwin,lastwin,direction)
Widget formwin,lastwin;
String direction;
{
    /* set up chain to previous widget */
    XtSetArg(Formargs[0],direction, (XtArgVal)lastwin);

    /* Create the local form */
    return(XtCreateManagedWidget("subform",
				 formWidgetClass,
				 formwin,
				 Formargs,
				 XtNumber(Formargs)));
}

/*
 * Make a status widget
 */
static Widget
makestatuswidget(label,formwin,lastwin,name,direction)
char *label;
Widget formwin,lastwin;
String name,direction;
{
    XtSetArg(Statusargs[XtNumber(Statusargs)-1],
	     XtNlabel,(XtArgVal)label);
    /* set up chain to previous Load widget */
    XtSetArg(Statusargs[0],direction,(XtArgVal)lastwin);
    
    /* Create the load widget */
    return(XtCreateManagedWidget(name,
				 loadWidgetClass,
				 formwin,
				 Statusargs,
				 XtNumber(Statusargs)));
}

/*
 * Make a remote status widget
 */
static Widget
makeremotestatus(hostname,formwin,lastwin,direction)
String hostname;
Widget formwin,lastwin;
String direction;
{
    String labelname;		/* label, could be "machine DOWN" */
    struct hostent *host;	/* for gethostbyname() */
    Widget remotestatus;

    /* translate command arg to a hostname */
    host = gethostbyname(hostname);
    /* if host == NULL, then hostname is junk for some reason */
    if(!host) 
      return(NULL);

    /* get rid of domain's in fully specified host names */
    strtok(host->h_name,".");
    /* allocate memory for name and downmsg for label */
    if(!(labelname=malloc((strlen(host->h_name)
			   +strlen(DOWNMSG)+1)*sizeof(char))
	 )) {
	perror("xnetload: Ran out of memory for labelname");
	exit(1);
    }
    /* set label name */
    strcpy(labelname,host->h_name);
    remotestatus = makestatuswidget(labelname,formwin,lastwin,
				    "remote",direction);

    modifyloadcall(remotestatus,host->h_name);
    return(remotestatus);
}
    

/*
 * replace standard load widget status call with one that looks at rwho
 */
static void
modifyloadcall(statuswin,name)
Widget statuswin;
String name;
{
    String hostname;

    /* allocate memory for hostname */
    if(!(hostname=
	 malloc((strlen(name)+1)*sizeof(char)))) {
	perror("xnetload: Ran out of memory for name");
	exit(1);
    }
    /* Set up hostname, and load callback (GetStatus()) */
    strcpy(hostname,name);
    XtRemoveAllCallbacks(statuswin, XtNgetLoadProc);
    XtAddCallback(statuswin,XtNgetLoadProc,
		  GetStatus,(caddr_t)hostname);
}

/*
 * You're wondering why I'm doing this, right?  Well, the default update
 * for the Load widget is 5 seconds (I think), but if rwhod updates the
 * files every 60 seconds, then the load displays will look weird.  So, I
 * don't force the user to have a 60 second update, but I at least make
 * this the default.
 */
static void
SetRemUpdate(w)
Widget w;
{
    XrmDatabase rdb;
    char resourceline[50];

    /* Make a database with new default */
    sprintf(resourceline,"%s: %d\n","xnetload*remote*update",DEFREMUPDATE);
    rdb = XrmGetStringDatabase(resourceline);

    /* merge existing Display database into new one */
    /* so that user choices are preserved */
    XrmMergeDatabases(XtDisplay(w)->db,&rdb);

    /* Set Display database to new one */
    XtDisplay(w)->db = rdb;
}

/* GetStatus() global vars */
static char *Label;
static Arg Labelarg[1];
/*
 * Load callback function
 *    - calls getload to determine current load, then updates labels
 */
void
GetStatus(w,closure,call_data)
Widget w;                        /* load widget */
caddr_t closure;                 /* name of host, char*  */
caddr_t call_data;               /* load, float*  */
{
    int load;                    /* current load for this machine */

    /* get current label for widget */
    XtSetArg(Labelarg[0],XtNlabel,(XtArgVal)&Label);
    XtGetValues(w,Labelarg,(Cardinal)1);

    /* if load is less than zero, then machine is down or something */
    if((load = getload(closure)) < 0) {
        *(double *)call_data = 0.0;
        /* update label with DOWN message */
        if(!matchstr(Label,DOWNMSG)) {
	    updatelabel(w,(char *)closure,DOWNMSG);
	}
    }
    /* otherwise we found a good load, so use it */
    else {
        *(double *)call_data = (double)load/100.0;
        /* hate to do this everytime, but we have to check */
        if(matchstr(Label,DOWNMSG)) {
	    updatelabel(w,(char *)closure,"");
	}
        return;
    }
}

/*
 * look for pat in str - exact match, no wild cards here
 */
static int
matchstr(str,pat)
register char *str, *pat;
{
    while(str = index(str,*pat))
      if(! strcmp(str++,pat))
        return(TRUE);
    return(FALSE);
}

/*
 * replace label with str and msg
 */
static int
updatelabel(w,str,msg)
Widget w;
register char *str, *msg;
{
    XEvent Sendevent;

    sprintf(Label,"%s%s",str,msg);
    XtSetArg(Labelarg[0],XtNlabel, (XtArgVal)Label);
    XtSetValues(w,Labelarg, (Cardinal)1);

    /* Load widget doesn't redraw when label is changed.  Force it! */
    XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,TRUE);
}

static int
usage()
{
    fprintf(stderr,"usage:\n xnetload [-configuration mXn] [-nolocal] [machine] ...\n");
}
