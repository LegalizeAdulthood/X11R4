/***********************************************************

$Header: StateSave.c,v 3.0 89/11/20 09:25:40 mikew Exp $

Copyright 1989 by Mi

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

#include <X11/Xlib.h>
#include "ICC.h"
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "xdefs.h"
#include "Top.h"
#include "PropUtils.h"
#include "ICCUtils.h"
#include "WinInfo.h"
#include "State.h"
#include "Atoms.h"

/*
 * Description: Save the current state of the window system.
 * 
 * Side Effects: Can cause the clients to update WM_COMMAND and/or die.
 *
 */

Status
StateSave(state)
    StatePtr	    state;	/* pointer to state */
{
    Status   	cc;
    char     	stateFile[256];
    FILE       *stateFP;
    char       *getenv();
    WinInfoPtr  wi;

    sprintf(stateFile, "%s/%s", getenv("HOME"), applData.stateFile);
    stateFP = fopen(stateFile, "w");
    if (stateFP == NULL) {
	perror("rewrite statefile");
	return (SM_FAILURE);
    }
    for (wi = state->topList; wi != NULL; wi = wi->next)
      if ((cc = WIPrint(stateFP, wi)) < 0)
	  return(cc);
    (void) fclose(stateFP);
    return (SM_SUCCESS);
}

/*
 * Description: Write the state to a file.
 * 
 */

Status
StatePrint(stateFP, state)
    FILE           *stateFP;	/* stream to send output to */
    StatePtr        state;	/* state to output */
{
    WinInfoPtr      wi;

    for (wi = state->topList; wi != NULL; wi = wi->next)
      (void) WIPrint(stateFP, wi);
    return (SM_SUCCESS);
}

/*
 * Description: Apply proper quoting to an argument. For instance, 
 *		an argument with a space in it should be quoted.
 * 
 */

Status
QuoteArg(arg, string, len)
    unsigned char	*arg;		/* argument to quoted  */
    char		*string; 	/* result of quoting */
    int			*len;		/* length of string */
{
    int			special = False;
    char		*cp;
    register int	charsLeft = *len;

    string += strlen(string);
    if (rindex(arg, ' ')) {
	*string++ = '\"';
	if (--charsLeft <= 0)
	    return(SM_FAILURE);
	special = True;
    }
    for (cp = (char *) arg; cp < (char *) arg + strlen(arg); ++cp) {
	if (*cp == '\"' || *cp == '\\') {
	    *string++ = '\\';
	    if (charsLeft-- <= 0)
		return(SM_FAILURE);
	}
	*string++ = *cp;
	if (charsLeft-- <= 0)
	    return(SM_FAILURE);
    }
    if (special == True) {
	*string++ = '\"';
	charsLeft--;
    }
    *string = 0;
    *len = charsLeft;
    return(SM_SUCCESS);
}

/*
 * Description: Apply proper quoting to arguments. For instance, arguments with
 * 		spaces should have quotes around them.
 * 
 */

static
Status
ArgvToString(argc, argv, string, len)
int		argc;		/* number of  arguments*/
unsigned char **argv;		/* arguments */
char	       *string;		/* result string */
int		len;		/* length of result string */
{
    int i;

    string[0] = 0;
    for (i = 0; i < argc; ++i) {
	if (i > 0) {
	    if (--len <= 0)
		break;
	    strcat(string, " ");
	}
	if (QuoteArg(argv[i], string, &len) < 0)
	    return(SM_FAILURE);
    }
    return(SM_SUCCESS);
}


/*
 * Description: Print the command neccessary to restart the client associated
 * 		with a specific top-level window.
 * 
 */

static 
Status 
WIPrint(stateFP, wi)
    FILE           *stateFP;	/* output stream */
    WinInfo        *wi;		/* window information structure */
{
    char	    	   command[500];
    unsigned char	  *host;
    static unsigned char   hostname[HOSTNAME_LEN] = "none";
    static unsigned char   localhost[] = "localhost";

    if (!strcmp(hostname, "none"))
	if (gethostname(hostname, sizeof(hostname)) < 0)
	    return(SM_FAILURE);

    if (wi->argv == NULL)
	return(SM_FAILURE);
    ArgvToString(wi->argc, wi->argv, command, sizeof(command));
    if (strcmp(hostname, wi->wmClientMachine))
	host = wi->wmClientMachine;
    else
	host = localhost;
    fprintf(stateFP, "%d %d %d %d %d %d %s %s %s %s\n", 
	    wi->win_gravity, wi->wmState.state, wi->width, wi->height, 
	    wi->x, wi->y,
	    host, wi->wmClass.res_class, wi->wmClass.res_name,
	    command);
    return (SM_SUCCESS);
}

static int
putString(fp, s, len)
FILE		*fp;
unsigned char	*s;
int 		len;
{
    if (putw(htonl(len), fp))
	return(SM_FAILURE);
    if (len > 0)
	if (fwrite(s, len, 1, fp) == 0)
	    return(SM_FAILURE);
    return(SM_SUCCESS);
}
    
/*
 * Description: Print the command neccessary to restart the client associated
 * 		with a specific top-level window.
 * 
 */

static
Status
WISave(stateFP, wi)
FILE           *stateFP;	/* output stream */
WinInfo        *wi;		/* output  */
{
    XTextProperty  textCommand;
    static unsigned char   hostname[HOSTNAME_LEN] = "none";
    static unsigned char   localhost[] = "localhost";

    if (!strcmp(hostname, "none"))
	if (gethostname(hostname, sizeof(hostname)) < 0)
	    return(SM_FAILURE);
    if (putw(htonl(wi->win_gravity), stateFP))
	return(SM_FAILURE);;
    if (putw(htonl(wi->wmState.state), stateFP))
	return(SM_FAILURE);
    if (putw(htonl(wi->width), stateFP))
	return(SM_FAILURE);
    if (putw(htonl(wi->height), stateFP))
	return(SM_FAILURE);
    if (putw(htonl(wi->x), stateFP))
	return(SM_FAILURE);
    if (putw(htonl(wi->y), stateFP))
	return(SM_FAILURE);
    if (strcmp(hostname, wi->wmClientMachine)) {
	if (putString(stateFP, wi->wmClientMachine, 
		      strlen(wi->wmClientMachine)) < 0)
	    return(SM_FAILURE);
    } else {
	if (putString(stateFP, localhost, strlen(localhost)) < 0)
	    return(SM_FAILURE);
    }
    if (putString(stateFP, (unsigned char *) wi->wmClass.res_class,
		  strlen(wi->wmClass.res_class)) < 0)
	return(SM_FAILURE);
    if (putString(stateFP, (unsigned char *) wi->wmClass.res_name,
		  strlen(wi->wmClass.res_name)) < 0)
	return(SM_FAILURE);
    if (XStringListToTextProperty(wi->argv, wi->argc, &textCommand) == 0)
	return(SM_FAILURE);
    if ((textCommand.format != 8) || (textCommand.encoding != XA_STRING))
	return(SM_FAILURE);
    if (putString(stateFP, textCommand.value, (int) textCommand.nitems) < 0)
	return(SM_FAILURE);
    return (SM_SUCCESS);
}


