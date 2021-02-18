/***********************************************************

$Header: StateRestore.c,v 3.0 89/11/20 09:25:38 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


#include "SM.h"
#include <pwd.h>

#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "ICC.h"
#include "WinInfo.h"
#include "Top.h"
#include "Home.h"
#include "State.h"

static char    *GetName();

/* 
 * number of characters in a machine word when expressed as an ascii string
 */
#define MAX_CHARS_PER_NUMBER	10
#define MAX_CHARS_PER_STRING	512

/*
 * Description: Return 1 word in host byte order
 * 
 *
 */

static
Status
getWord(fp)
FILE *fp;			/* stream file pointer */
{
    char	temp[MAX_CHARS_PER_NUMBER + 1]; /* NULL terminated string */
    char       *p;
    int		c;

    for (p = temp; p < temp + MAX_CHARS_PER_NUMBER; ++p) {
	c = getc(fp);
	if (c == ' ')
	    break;
	if (c == EOF)
	    return(SM_FAILURE);
	*p = c;
    }
    *p = 0;
    return(atoi(temp));
}

/*
 * Name:	getString
 * 
 * Description: Read a string from a file
 * 
 * Side Effects: None.
 *
*/

static
Status
getString(fp, result)
FILE	       *fp;	/* input stream pointer */
unsigned char **result  ;
{
    unsigned char	temp[MAX_CHARS_PER_STRING + 1];
    unsigned char      *p;
    int			c;
    Status		cc;
    Boolean		quoteFlag = False;

    for (p = temp; p < temp + MAX_CHARS_PER_STRING; ) {
	c = getc(fp);
	switch (c) {
	case '\"':
	    if (quoteFlag)
		quoteFlag = False;
	    else
		quoteFlag = True;
	    continue;		/* process next char */
	case '\\':
	    c = getc(fp);	/* get next char and store it */
	    if (c == EOF) {
		cc = 2;
		goto done;
	    }
	    break;
	case ' ':
	    if (quoteFlag)
		break;
	    cc = 0;
	    goto done;		/* end of string */
	case '\n':
	    cc = 1;
	    goto done;		/* end of string */
	case EOF:
	    cc = 2;
	    goto done;		/* end of string */
	}
	*p++ = c;
    }
 done:
    *p = 0;
    *result = (unsigned char *) XtMalloc(p - temp + 1);
    strcpy(*result, temp);
    return(cc);
}

/*
 * Description: Read in a WinInfo from a saved state file.
 * 
 * Outputs:	int - -1 = error, 0 = ok, 1 = EOF
 * 
 */

int
WIRead(wi, fp)
    WinInfo	*wi;	/* window info structure */
    FILE	*fp;	/* input stream pointer */
{
    Status		cc;

    if ((wi->win_gravity = getWord(fp)) < 0)
	return(1);
    if ((wi->wmState.state = getWord(fp)) < 0)
	return(SM_FAILURE);
    if ((wi->width = getWord(fp)) < 0)
	return(SM_FAILURE);
    if ((wi->height = getWord(fp)) < 0)
	return(SM_FAILURE);
    if ((wi->x = getWord(fp)) < 0)
	return(SM_FAILURE);
    if ((wi->y = getWord(fp)) < 0)
	return(SM_FAILURE);
    if (getString(fp, &wi->wmClientMachine) != 0)
	return(SM_FAILURE);
    if (getString(fp, &wi->wmClass.res_class) != 0)
	return(SM_FAILURE);
    if (getString(fp, &wi->wmClass.res_name) != 0)
	return(SM_FAILURE);
    wi->argv = NULL;
    for (wi->argc = 1; ; ++wi->argc) {
	wi->argv = (unsigned char **) 
	    XtRealloc(wi->argv, (wi->argc) * sizeof(*wi->argv));
	cc = getString(fp, &wi->argv[wi->argc - 1]);
	if (cc == 2)
	    return(1);
	if (cc == 1)
	    break;
    }
    return(SM_SUCCESS);
}

/*
 * Description: Read the state from a saved state file.
 * 
 */

static
Status
StateRead(state)
    State	*state;	/* pointer to the current state */
{
    Status 	cc;
    FILE	*commandFile;
    char        *commandFileName;
    WinInfo	*wi;

    /*
     * if GetName can't file a .SMState file then just start an xterm
     * console.
     */
    if ((commandFileName = GetName()) == NULL) {
	if (system(applData.defaultCommand) == -1) {
	    Warning("RestoreState", "can't start console xterm\n");
	    return (SM_FAILURE);
	}
	return (SM_SUCCESS);
    }
    commandFile = fopen(commandFileName, "r");
    if (commandFile == NULL) {
	Warning("RestoreState", "can't open command file\n");
	return(SM_FAILURE);
    }
    for (;;) {
	wi = wiAlloc();
	if (wi == NULL)
	    return(SM_FAILURE);
	cc = WIRead(wi, commandFile);
	if (cc != 0)
	    if (wiFree(wi) < 0)
		return(SM_FAILURE);
	if (cc < 0)
	    return(cc);
	else if (cc == 1)
	    break;
	if ((cc = StateAddWinInfo(state, wi)) < 0)
	    return(cc);
	if ((cc = StateAddToHostList(state, wi)) < 0)
	    return(cc);
    }
    fclose(commandFile);
    return(SM_SUCCESS);
}

/*
 * Description: Safely concatenate two strings.
 *
 */

Status
safestrcat(target, source, len)
    char	*target;
    char	*source;
    int		len;
{
    int		targetLen;
    int		sourceLen;

    targetLen = strlen(target);
    sourceLen = strlen(source);
    if ((sourceLen + targetLen) >= len)
	return(SM_FAILURE);
    strcat(target, source);
    return(SM_SUCCESS);
}

/*
 * Description: Print the command neccessary to restart the client associated
 * 		with a specific top-level window.
 * 
 */

/*ARGSUSED*/
static
Status
WIGenerateCommand(wi, command, len)
    WinInfo        *wi;		/* window information structure */
    char 	   *command;	/* command line */
{
    Status		cc;
    int             	i;
    static char	    	temp[TEMP_LEN];

    command[0] = 0;
    if (wi->argv == NULL)
	return(SM_SUCCESS);
    if ((cc = safestrcat(command, (char *) wi->argv[0], len)) < 0)
	return(cc);
    if ((cc = safestrcat(command, " ", len)) < 0)
	return(cc);
    /* Check if WM_COMMAND has -geometry, so we can override it. */
    for (i = 1; i < wi->argc; ++i)
	if (!strcmp(wi->argv[i], applData.geometry))
	    break;
/*
 * No geometry was specified so put it at the beginning of the argument
 * list. This is questionable.
 */
    if (i == wi->argc) {
	sprintf(temp, "%s %dx%d+%d+%d ", applData.geometry,
		wi->width, wi->height, wi->x, wi->y);
	if ((cc = safestrcat(command, temp, len)) < 0)
	    return(cc);
    }
    if (wi->wmState.state == IconicState) {
	for (i = 1; i < wi->argc; ++i)
	    if (!strcmp(wi->argv[i], applData.iconic))
		break;
	if (i == wi->argc) {
	    if ((cc = safestrcat(command, applData.iconic, len)) < 0)
		return(cc);
	    if ((cc = safestrcat(command, " ", len)) < 0)
		return(cc);
	}
    }
    for (i = 1; i < wi->argc; ++i) {
/* Replace the -geometry with the current one.  This is questionable. */
	if (!strcmp(wi->argv[i], applData.geometry)) {
	    i++;
	    sprintf(temp, "%s %dx%d+%d+%d ", applData.geometry,
		    wi->width, wi->height, wi->x, wi->y);
	    if ((cc = safestrcat(command, temp, len)) < 0) 
		return(cc);
	    continue;
	}
/* Get rid of Iconic if it isn't iconic */
	if (wi->wmState.state != IconicState && !strcmp(wi->argv[i], 
							applData.iconic))
	    continue;
	if ((cc = QuoteArg(wi->argv[i], command, &len)) < 0)
	    return(cc);
	if ((cc = safestrcat(command, " ", len)) < 0)
	    return(cc);
    }
    if ((cc = safestrcat(command, "< /dev/null >& /dev/null &\n", len)) < 0)
	return(cc);
    return (SM_SUCCESS);
}

/*
 * Description: Set my own parameters based on a WinInfo with my name on 
 *		it.
 *
 */

SetMyOwnParameters(wi)
WinInfoPtr	wi;
{
    Arg		args[10];
    Cardinal	n;
    char	geom[30];

    n = 0;
    sprintf(geom, "%dx%d+%d+%d", wi->width, wi->height, wi->x, wi->y);
    XtSetArg(args[n], XtNgeometry, geom); n++;
    XtSetValues(topShell, args, n);
#ifdef NDEF
    XtSetArg(args[n], XtNx, wi->x); n++;
    XtSetArg(args[n], XtNy, wi->y); n++;
    XtSetArg(args[n], XtNwidth, wi->width); n++;
    XtSetArg(args[n], XtNheight, wi->height); n++;
#endif
    n = 0;
    if (wi->wmState.state == IconicState) {
	XtSetArg(args[n], XtNiconic, True); n++;
    } else {
	XtSetArg(args[n], XtNiconic, False); n++;
    }
    XtSetValues(topShell, args, n);
    return(0);
}
    
/*
 * Description: Start up all the programs specified by a state.
 * 
 * Side Effects: Starts up a bunch of programs.
 *
 */

Status
StateExecute(state)
    State	   *state;	/* state to execute */
{
    FILE	   *shellInput;
    char	    command[500];
    WinInfoPtr	    *winInfo;
    HostInfoPtr	    hostInfo;
    char	    setDisplay[100];
    char	   *display;
    char	   *nonHostPart;
    static unsigned char   hostname[HOSTNAME_LEN] = "none";
    static char	    localhost[] = "localhost";
    
    if (!strcmp(hostname, "none"))
	if (gethostname(hostname, sizeof(hostname)) < 0)
	    return(SM_FAILURE);

    for (hostInfo = state->hostInfo; hostInfo != NULL; 
	 hostInfo = hostInfo->next) {
	if (strcmp(hostInfo->hostName, localhost) == 0) {
	    strcpy(command, "/bin/csh");
	} else {
	    sprintf(command, "/usr/ucb/rsh %s /bin/csh -fi", 
		    hostInfo->hostName);
#ifdef NDEF
	    strcpy(command, "/usr/ucb/rsh ");
	    strcat(command, hostInfo->hostName);
#endif
	}
	shellInput = popen(command, "w");
	if (shellInput == NULL) {
	    Warning("RestoreState", "can't create pipe to shell\n");
	    return(SM_FAILURE);
	}
	display = XDisplayString(XtDisplay(topShell));
	if (strlen(display) < 1)
	    return(SM_FAILURE);
	nonHostPart = rindex(display, ':');
	if (display[0] == ':' || strncmp(display, "unix:", 5) == 0)
	    sprintf(setDisplay, "setenv DISPLAY %s%s\n", hostname, 
		    nonHostPart);
	else
	    sprintf(setDisplay, "setenv DISPLAY %s\n", display);
	fwrite(setDisplay, strlen(setDisplay), 1, shellInput);
	fflush(shellInput);
	for (winInfo = hostInfo->winInfos;
	     winInfo < hostInfo->winInfos + hostInfo->numWinInfos; ++winInfo) {
	    if (strcmp((*winInfo)->wmClass.res_class, "SM") == 0)
		SetMyOwnParameters(*winInfo);
	    else {
		WIGenerateCommand(*winInfo, command, sizeof(command));
		fwrite(command, strlen(command), 1, shellInput);
		fflush(shellInput);
	    }
	}
#ifdef NDEF
	pclose(shellInput);
#endif
    }
    return (SM_SUCCESS);
}

/*
 * Description: Restore the saved state
 * 
 * Side Effects: Starts up a bunch of programs.
 *
 */

Status
StateRestore()
{
    Status	cc;
    State	state;
    
    StateInit(&state);
    if ((cc = StateRead(&state)) < 0)
	return(cc);
    if ((cc = StateExecute(&state)) < 0)
	return(cc);
    StateCleanup(&state);
    return (SM_SUCCESS);
}


/*
 * Description: Finds the name of the SMState file to execute. It looks 
 *		first in the user's home directory, and then in a 
 *		system directory.
 * 
 * Outputs:	char * - the name of the file
 * 
 */

static
char    *
GetName()
{
    struct passwd  *pw;
    int             endpwent();
    struct passwd  *getpwuid();
    char           *home;
    static char     path[256];

    if ((home = getenv("HOME")) == NULL) {
	if ((pw = getpwuid(getuid())) == NULL) {
	    PWarning("GetName");
	    return (NULL);
	}
	home = pw->pw_dir;
	endpwent();
    }
    (void) sprintf(path, "%s/%s", home, applData.stateFile);

    if (access(path, F_OK) == 0)
	return (path);

    (void) sprintf(path, "%s/%s", SM_DIR, applData.defaultState);

    if (access(path, F_OK) == 0)
	return (path);

    return (NULL);
}


