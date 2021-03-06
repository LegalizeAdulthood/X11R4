/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpa.c,v 2.28 89/10/17 15:58:01 ajp Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpa.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpa.c,v 2.28 89/10/17 15:58:01 ajp Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*---------------------------------------------------------------------------*/
/*									     */
/*		          	ATK Help Program			     */
/*									     */
/*	History:							     */
/*		original be2 version: Mike Kazar, c. 1985		     */
/*									     */
/*		complete ATK rewrite: Marc Pawliger, 2/89		     */
/*									     */
/*	See README for programmer details				     */
/*									     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*	MODULE: helpa.c							     */
/*		Subclass of application, front-end for help.  Parses switches*/
/*		and creates a new help object.  Sets up a resident service   */
/*		on a socket to listen for new help connections.	             */
/*---------------------------------------------------------------------------*/

#include <class.h>

#include <andyenv.h>
#include <andrewos.h> /* sys/types.h sys/time.h strings.h */

#include <app.ih>
#include <environ.ih>
#include <frame.ih>
#include <help.ih>
#include <im.ih>
#include <message.ih>
#include <panel.ih>
#include <view.ih>

#include <stdio.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <netinet/in.h>

#include "config.h"
#include "helpsys.h"
#include <helpa.eh>

/*---------------------------------------------------------------------------*/
/*			CONDITIONAL DEBUGGING				     */
/*---------------------------------------------------------------------------*/

#ifdef DEBUGGING
/*
 * debugging statements can be included by compiling add modules with
 * -DDEBUGGING.  Debugging output is toggled by existance/nonexistance
 * of the environment variable HELPAPPDEBUG.
 */
int HELPAPPDEBUG = 0;
#undef DEBUG
#define DEBUG(arg)  if (HELPAPPDEBUG != 0) { printf arg; fflush(stdout); }
#else
#define DEBUG(arg)
#endif

/*---------------------------------------------------------------------------*/
/*				GLOBALS					     */
/*---------------------------------------------------------------------------*/

/* for setting up the help service */
static struct servent *ts;
static struct hostent *th;
static short helpPort;
static struct sockaddr_in myaddr;
static int helpSocket = -1;

static char *helpKey="";	/* the topic */
static int moreMode=FALSE;	/* use the termcap-based interface? */
static int listMode=FALSE;	/* just list files? */
static int print=FALSE;		/* in termcap-based mode, prompt for printing? */
static int newWindow=FALSE;	/* force a new help window? */
static int noDefault=FALSE;	/* use a default file or not? */
static char *indexName=NULL;	/* alternative index file? */
static char *aliasName=NULL;	/* additional index file? */
/* list of addition search directories */
static struct helpDir *searchDirs=(struct helpDir *)NULL;
static char *error = "Sorry; no help available on '%s'.";

struct help *helpobj;		/* global help object for ncproc use */

/*
 * usage statement
 */
static void show_usage(self)
struct helpapp *self;
{
#ifdef DEBUGGING
    fprintf(stderr,
	"Usage: %s [-dmnhlo] [-i index dir] [-a alias file] [-s dir] [topic]\n",
	    helpapp_GetName(self));
    fprintf(stderr,"\t-d: do not fork (debug mode)\n");
#else /* !DEBUGGING */
    fprintf(stderr,
	"Usage: %s [-mnhlo] [-i index dir] [-a alias file] [-s dir] [topic]\n",
	    helpapp_GetName(self));
#endif /* DEBUGGING */
    fprintf(stderr,
"	-h: show this usage statement\n\
	-m: 'more' mode (for use with terminals)\n\
	-o: in 'more' mode, prompt for printing each file\n\
	-n: use a new help window, don't re-use old window\n\
	-l: just list the help files available for this topic\n\
	-e: don't show default help file\n\
	-i directory: specify different index directory\n\
	-a alias file: specify different help.aliases file\n\
	-s directory: search this directory, too\n\
	topic: the subject on which you want help.\n");
}
    
    
/*
 * start up and start accepting connections and commands from other helps
 */
static void ncproc ()
{
    register int ns;
    struct sockaddr_in helpaddr;
    long addrlen = 0;
    char buf[MAXPATHLEN+1];
    char errbuf[HNSIZE + 50];
    long len;

    addrlen = sizeof(helpaddr);
    ns = accept(helpSocket, &helpaddr, &addrlen);
    if (ns >= 0) {
	while(1) {
	    addrlen = read(ns, &len, sizeof(long));
	    if (addrlen <= 0) {
		DEBUG(("ha: no more to read\n"));
		close(ns);
		return;
	    }
	    len = ntohl(len);
	    DEBUG(("ha: read len: %d\n", len));
	    addrlen = read(ns, buf, len);
	    DEBUG(("ha: read str: %s\n", buf));
	    switch(buf[0]) {
	    case 's':		/* add a search dir */
		help_AddSearchDir(buf+1);
		break;
	    case 'i':		/* change the index */
		help_SetIndex(buf+1);
		break;
	    case 'a':		/* add an alias file */
		help_SetAliasesFile(buf+1);
		break;
	    case 'h':		/* get help on a topic */
		im_ExposeWindow(view_GetIM(help_GetInstance()));
		sprintf(errbuf, error, buf+1);
		help_HelpappGetHelpOn(buf+1, help_NEW, help_HIST_NAME, errbuf);
	    }
	}
    }
}

/*
 * add a path to those to be searched
 */
static AddPath(astr)
register char *astr;
{
    char tname[MAXPATHLEN];
    register char *np;

    DEBUG(("ha: IN addpath\n"));
    while(1) {
	if (strlen(astr) == 0) break;
	np = index(astr, ':');
	if (np) {
	    strncpy(tname, astr, np-astr);
	    tname[np-astr] = 0;
	    astr = np+1;
	    help_AddSearchDir(tname);
	}
	else {
	    /* no colon, try the whole name */
	    help_AddSearchDir(astr);
	    break;
	}
    }
    DEBUG(("ha: OUT addpath\n"));
}


/*
 * send_pack: send a command packet to an existing help instance
 */
static int send_pack(c, s, sock)
char c;				/* the command char */
char *s;			/* the string to send */
int sock;			/* the socket to send to */
{
    long len;
    char buf[MAXPATHLEN + 1 + sizeof(long)];

    /* length = string + NULL + sizeof(length) + command char */
    len = strlen(s) + sizeof(long) + 2;
    *(long *)buf = htonl(len);
    sprintf(buf+sizeof(long), "%c%s", c, s);
    DEBUG(("ha: sent: %s\n", buf+sizeof(long)));
    return write(sock, buf, len);
}


/*
 * contact other help servers (if any) before doing any unecessary processing.
 *
 * If another server is found, we pass in -i -a and -s switches and finally the
 * new requested help topic.  The protocal is simple: a packet length, then a
 * character representing the command (i, a, s and h), and then the text of the
 * thing to be added.
 */

#ifdef MAXHOSTNAMELEN
#undef MAXHOSTNAMELEN
#endif /* MAXHOSTNAMELEN */
#define MAXHOSTNAMELEN 64	/* some people just don't */

static void unique_help()
{
    register int i;
    char *wmHost;
    int runningLocally;
    static FILE *tfile;

    /* lookup the help port, default to the 'right' thing */

    if ((ts = getservbyname("help", 0)) != (struct servent *)NULL)
	helpPort = ts->s_port;
    else
	helpPort = htons(HELPSOCK);

    wmHost = (char *) environ_Get("WMHOST");
    if (wmHost == NULL) {
	wmHost = (char *)malloc(MAXHOSTNAMELEN * sizeof(char));
	gethostname(wmHost, MAXHOSTNAMELEN);
	DEBUG(("ha: wmhost: %s\n",wmHost));
	runningLocally = 1;
    } else
	runningLocally = 0;

    if (!newWindow) {
	/* see if we can find a help server already */
	th = gethostbyname(wmHost);
	if (th != NULL) {
	    bcopy(th->h_addr, &myaddr.sin_addr.s_addr, sizeof(long));
	    myaddr.sin_family = AF_INET;
	    myaddr.sin_port = helpPort;
	    helpSocket = socket(AF_INET, SOCK_STREAM, 0);
	    i = connect(helpSocket, &myaddr, sizeof(myaddr));
	    if (i >= 0) {
		struct helpDir *thd, *nhd;

		for (thd = searchDirs; thd; thd = nhd) {
		    send_pack('s', thd->dirName, helpSocket);
		    nhd = thd->next;
		    free(thd->dirName);
		    free(thd);
		}
		if (indexName != NULL)
		    send_pack('i', indexName, helpSocket);
		if (aliasName != NULL)
		    send_pack('a', aliasName, helpSocket);
		send_pack('h', helpKey, helpSocket);
		printf("Sent request to existing help window.\n");
		close(helpSocket);
		exit(0);
	    }
	} else
	    printf("No 'localhost' found in host table; creating new window.\n");
    }

    /*
     *setup the help server, but not if we're running on someone else's machine
     */
    if (runningLocally) {
	helpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (helpSocket >= 0) {
	    int on;
	    
	    on = 1;
	    setsockopt(helpSocket, SOL_SOCKET, SO_REUSEADDR, on, sizeof(on) );
	    myaddr.sin_family = AF_INET;
	    myaddr.sin_port = helpPort;
	    myaddr.sin_addr.s_addr = 0;		/* me, me! */
	    i = bind(helpSocket, &myaddr, sizeof(myaddr));
	    if (i >= 0) {
		i = listen(helpSocket, 4);
		if (i == 0) {
		    /*
		     * we've got the socket correctly setup, now listen to it
		     */
		    tfile = fdopen(helpSocket, "r");
		    if (tfile) {
			im_AddFileHandler(tfile, ncproc, 0, 0);
		    }
		}
	    }
	}
    }
}


/*
 * parse command line arguments
 */
boolean helpapp__ParseArgs(self,argc,argv)
struct helpapp *self;
int argc;
char **argv;
{
    struct helpDir *thd=(struct helpDir *)NULL;

    DEBUG(("ha: IN parse args\n"));

    if(!super_ParseArgs(self,argc,argv))
	return FALSE;

#define GETARGSTR(var)\
{\
    if((*argv)[2]!='\0')\
        var= ((*argv)[2]=='=' ? &(*argv)[3] : &(*argv)[2]);\
    else if(argv[1]==NULL){\
	fprintf(stderr,"%s: %s switch requires an argument.\n",\
		helpapp_GetName(self),*argv);\
        return FALSE;\
    }else\
    	var= *++argv;\
}

    while(*++argv!=NULL)
	if(**argv=='-')
	    switch((*argv)[1]){
		    char *temp;
		case 'i':
		    if (indexName != NULL) {
			fprintf(stderr,"%s: only one index allowed\n",
				helpapp_GetName(self),*argv);
			exit(-1);
		    }
		    GETARGSTR(indexName);
		    /* also set no default */
		case 'e':
		    noDefault=TRUE;
		    break;
		case 'a':
		    GETARGSTR(aliasName);
		    break;
		case 'l':
		    listMode=TRUE;
		case 'm':
		    moreMode=TRUE;
		    break;
		case 's':
		    GETARGSTR(temp);
		    thd = (struct helpDir *)malloc(sizeof(struct helpDir));
		    thd->next = searchDirs;
		    searchDirs = thd;
		    thd->dirName = malloc(strlen(temp) + 1);
		    strcpy(thd->dirName, temp);
		    break;
		case 'n':
		    newWindow=TRUE;
		    break;
		case 'o':
		    print=TRUE;
		    break;
		case 'h':
		case 'x':
		    show_usage(self);
		    exit(0);
		    break;
		    /*NOTREACHED*/
		default:
		    fprintf(stderr,"%s: unrecognized switch: %s\n",
			    helpapp_GetName(self), *argv);
		    show_usage(self);
		    return FALSE;
	    } else {
		helpKey = *argv;
		DEBUG(("ha: key: %s\n",helpKey));
	}

    return TRUE;
}

boolean helpapp__Start(self)
struct helpapp *self;
{
    char *tp;
    struct helpDir *thd, *nhd;
    static struct im *im;
    static struct frame *frame;

    DEBUG(("ha: IN start\n"));
    if(!super_Start(self))
	return FALSE;

    /* try to determine if we're not under a window manager */
    if (((tp = environ_Get("WMHOST")) == NULL || *tp == '\0') &&
	((tp = environ_Get("DISPLAY")) == NULL || *tp == '\0') &&
	((tp = environ_Get("TERM")) == NULL || strcmp(tp, "wm") != 0))  {
	moreMode = TRUE;
	helpobj = help_New();
    }

    if (!moreMode) {
        unique_help();
    }

    /* allow us to 'see' the frame proctable, so we can use bind frame procs */
    class_Load("framecmds");
    
    if (!moreMode) {

	im = im_Create(NULL);		/* default window */
	if (!im) {
	    fprintf(stderr,"%s: failed to create new window; exiting.\n",
		    helpapp_GetName(self));
	    exit(1);
	}

	helpobj = help_New();
	
	frame = frame_New();

	if (!helpobj || !frame) {
	    fprintf(stderr,"%s: Could not initialize help properly; exiting.\n",
		    helpapp_GetName(self));
	    exit(1);
	}
	/* frame for frame_SetView must have associated im */
	frame_SetView(frame, help_GetApplicationLayer(helpobj));
	im_SetView(im, frame);

	/* add in a message handler */
	frame_PostDefaultHandler(frame, "message",
				 frame_WantHandler(frame, "message"));
    }

    for (thd = searchDirs; thd; thd = nhd) {
	help_AddSearchDir(thd->dirName);
	nhd = thd->next;
	free(thd->dirName);
	free(thd);
    }

    DEBUG(("key: '%s' nodef: %d\n", helpKey, noDefault));
    if ((!helpKey || !(*helpKey)) && !noDefault) {
	helpKey = (moreMode) ? NONWMDEFAULTFILE : WMDEFAULTFILE;
	DEBUG(("ha: nodef: %s\n",helpKey));
    }

    if ((tp = environ_GetProfile("SearchPath")) != NULL)
	/* add all the elements of this path to the help list */
	AddPath(tp);

    /* specify alternative index to use for debugging */
    if (indexName)
	help_SetIndex(indexName);

    /* specify additional help.aliases file */
    if (aliasName) {
	DEBUG(("ha: setting alias: %s\n",aliasName));
	help_SetAliasesFile(aliasName);
    }

    if (moreMode) {
	help_GetHelpOnTerminal(helpKey, listMode, print);
	exit(0);
    }

    DEBUG(("ha: OUT start\n"));
    return TRUE;
}

int helpapp__Run(self)
struct helpapp *self;
{
    char tbuffer[200];
    int code;

    DEBUG(("ha: IN run\n"));

    DEBUG(("ha: window made\n"));
    
    sprintf(tbuffer, error, helpKey);
    code = help_HelpappGetHelpOn(helpKey, help_NEW, help_HIST_NAME, tbuffer);
    if (code == 0) {
	/* couldn't get help on that item, and it wasn't because of a server,
	   so show the default file, which we 'know' exists */
	sprintf(tbuffer, error, WMDEFAULTFILE);
	help_HelpappGetHelpOn(WMDEFAULTFILE, help_NEW, help_HIST_NOADD, tbuffer);
    }
    help_WantInputFocus(helpobj, helpobj);
    super_Run(self);

    DEBUG(("ha: OUT run\n"));
    return(0);
}

boolean helpapp__InitializeObject(classID,self)
struct classheader *classID;
struct helpapp *self;
{
    helpapp_SetMajorVersion(self, MAJOR_VERSION);
    helpapp_SetMinorVersion(self, MINOR_VERSION);
#ifdef DEBUGGING
    if ((char *)getenv("HELPAPPDEBUG") != (char *) NULL)
	HELPAPPDEBUG = 1;
#endif /* DEBUGGING */
    return TRUE;
}
