#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Restart.c,v 5.32 89/11/22 15:12:42 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 * 001 -- Loretta Guarino Reid, DEC Ultrix Engineering Group,
    Western Software Lab. Convert to X11.
 * 002 -- Jordan Hubbard, U.C. Berkeley. Titlebar cleanup code.
 */

#include "awm.h"
#include "X11/Xutil.h"

extern XContext AwmContext;

/*
 * SessionCmd used to determine what, if any, session cmd has
 * been called.
 * Guarantee zero if no session cmd has yet been called.
 * Guarantee positive if RestartSession or EndSession called.
 * values may be NULL_SESSION, RESTART_SESSION, or END_SESSION
 */
static int SessionCmd = NULL_SESSION;


Boolean Restart(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
    XButtonEvent button_event;
    int percentage;
    int i;

    Entry("Restart")

    percentage = VOLUME_PERCENTAGE(Volume);
    XBell(dpy, percentage);
    XBell(dpy, percentage);
	Cleanup();
    /*
     * Gag, rely on operating system to close connection because we don't
     * to mess ourselves up in case the exec fails.
     */
    execvp(*Argv, Argv);
    for (i = 0; i < 4; i++) {
	XBell(dpy, percentage);
	percentage += 10;
	if (percentage > 100) percentage = 100;
    }
    XSync (dpy, FALSE);
    fprintf(stderr, "%s:  Restart failed!\n", *Argv);
#if defined(XMALLOCDEBUG) | defined(MALLOCDEBUG)
    TERMINATE();
#endif
    exit(1);
}


Boolean Quit(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
     Entry("Quit")

     Cleanup();
#if defined(XMALLOCDEBUG) | defined(MALLOCDEBUG)
    TERMINATE();
#endif
     exit(0);
}

Boolean QuitTerm()
{
    Entry("QuitTerm")
    Warning("Got a SIGTERM; aborting...\n");

    Quit();
}

Boolean QuitInt()
{
    Entry("QuitInt")
    Warning("Got a SIGINT; aborting...\n");

    Quit();
}

Boolean QuitQuit()
{
    Entry("QuitQuit")
    Warning("Got a SIGQUIT; aborting...\n");

    Quit();
}

Boolean QuitHup()
{
    Entry("QuitHup")
    Warning("Got a SIGHUP; aborting...\n");

    Quit();
}


/*
 * Immediately blow client away without warning
 */
Boolean KillClient(window, mask, button, x, y)
Window window;
int mask;
int x, y;
{
     AwmInfoPtr awi;

     Entry("KillClient")

     if (window == RootWindow(dpy, scr))
		Leave(FALSE)
     if(!(awi = GetAwmInfo(window)))
	 Leave(FALSE)
     /*
      * If it's a titled window or an icon (we don't want to nuke ourselves)
      * we should get the client for it. We assume the subsequent destroy
      * notify will tell us to reclaim our resources.
      */
     XKillClient(dpy, awi->client);
     Leave(TRUE)
}


typedef struct _killItem {
    Window window;
    struct _killItem *next;
    } killItem;
static killItem *killList = NULL;

/*
 * Blow client away after warning it of impending doom and waiting for response
 *
 * WARNING!! 
 * CleanupSession depends on the fact that the mask,button,x,y
 * arguments are never used.  If they do get used, CleanupSession
 * will have to correctly pass them.
 */

Boolean TerminateClient(window, mask, button, x, y)
Window window;
int mask;
int x, y;
{
     AwmInfoPtr awi;
     XWMHints *wm_hints;
     static uninitialized = True;
     static XClientMessageEvent Event;
     killItem *newItem;
    int count, i;
    Atom *wm_protocols;

    Entry("TerminateClient")

    if (window == RootWindow(dpy, scr))
	Leave(FALSE)

    if(!(awi = GetAwmInfo(window)))
	Leave(FALSE)

    /* fire the warning shot if requested... */
    if (XGetWMProtocols(dpy, awi->client, &wm_protocols, &count)) {
	/* could optimize this by caching wm_protocols here, if we
	 * assumed that clients never change them, which we can't
	 */
	if (uninitialized) {
	    Event.type = ClientMessage;
	    Event.message_type = WmProtocols;
	    Event.format = 32;	/* timestamp */
	    /* looking at _XEventToWire, these are the fields to set */
	    uninitialized = False;
        }
	for(i = 0; (i < count) && (wm_protocols[i] != WmPrepareToDie); i++);
	if(i < count) {   /* got a match of WM_PREPARE_TO_DIE */
            /* add window to deletion list (order irrelevant) */
	    newItem = (killItem *) malloc(sizeof(killItem));	
	    newItem->window = awi->client;	
	    newItem->next = killList;
	    killList = newItem;
	    /* and tell client to dispose of data (and update WM_COMMAND) */
	    Event.window = awi->client;
	    Event.data.l[0] = WmPrepareToDie;
	    Event.data.l[1] = (long) last_server_time; 
	    		 /* collected from all events */
	    XSendEvent(dpy, awi->client, False, NULL, &Event);
	    /* no need to propogate to ourself! */
	    Leave(TRUE)
	}
	/* if client did not ask for WM_PREPARE_TO_DIE, kill it now */
	for(i = 0; (i < count) && (wm_protocols[i] != WmClosedown); i++);
	if(i < count) {   /* got a match of WM_CLOSEDOWN */
            /*  tell client to close its connection */
	    Event.window = awi->client;
	    Event.data.l[0] = WmClosedown;
	    Event.data.l[1] = (long) last_server_time; 
	    		 /* collected from all events */
	    XSendEvent(dpy, awi->client, False, NULL, &Event);
	    Leave(TRUE)
	}
    }
    /* else kill off client now */
    XKillClient(dpy, awi->client);
    XSync(dpy,FALSE);
    Leave(TRUE)
}


/* given client window ID, kill the client if it is on the kill list */

KillClientNow(window)
Window window;
{
    killItem *killPtr = killList;
    killItem *tempkillPtr;
    killItem *prevKillItem = NULL;
    static uninitialized = True;
    static XClientMessageEvent Event;
    int count, i;
    Atom *wm_protocols;

    Entry("killClientNow");

    if (killPtr == NULL) {
	/* no windows on delete list, ignore */
        Leave_void
    }

    /* fprintf (stderr, "KillClientNow: Current killList:\n"); */
    do {
        /* fprintf (stderr, "%x, ",  killPtr->window); */
        killPtr = killPtr->next;		    
    } while (killPtr->next != NULL);
    /* fprintf (stderr, "\n\n"); */
    killPtr = killList;

    while ((killPtr->window != window) && (killPtr->next != NULL)) {
        prevKillItem = killPtr;
        killPtr = killPtr->next;		    
    }
    if (killPtr->window != window) {
	/* window was not on delete list, ignore */
        Leave_void
    }
    /* OK, now check for WM_CLOSEDOWN before killing client. */
    if (XGetWMProtocols(dpy, window, &wm_protocols, &count)) {
	if (uninitialized) {
	    Event.type = ClientMessage;
	    Event.message_type = WmProtocols;
	    Event.data.l[0] = WmClosedown;
	    Event.format = 32;	/* timestamp */
	    uninitialized = False;
        }
	for(i = 0; (i < count) && (wm_protocols[i] != WmClosedown); i++);
	if(i < count) {   /* got a match of WM_CLOSEDOWN */
            /*  tell client to close its connection */
	    Event.window = window;
	    Event.data.l[1] = (long) last_server_time; 
	    		 /* collected from all events */
	    XSendEvent(dpy, window, False, NULL, &Event);
	}
	else
	    XKillClient(dpy, window);
    }
    else
	XKillClient(dpy, window);

    /*
     * First a list of all non login windows is exhausted
     * then a second list of a single login window is processed
     * If this is that login window, Sync so we die before returning
     */
    if (window == LoginWindow) {
	XSync(dpy,FALSE);
    }

    /* Note - if client exits first, this throws us into the error handler */

    /* if client did not respond to WM_PREPARE_TO_DIE, and user
       executed a kill function multiple times, the window will be
       on the list multiple times.  Be sure we delete all of them! */
    do {
	if (killPtr->window == window) {
            if (prevKillItem)
                prevKillItem->next = killPtr->next;
            else
                killList = killPtr->next;
	    tempkillPtr = killPtr;
	    killPtr = killPtr->next;
            free(tempkillPtr);
	}
	else {
	    prevKillItem = killPtr;
	    killPtr = killPtr->next;		    
	}
    } while (killPtr != NULL);

    /* 
     * When list is empty
     * Check to see if session is to be terminated
     */
    if (!killList)
	 /* May return if no session cmd called or Login not terminated yet */
	TerminateSession();

    Leave_void
}


/*
 * Ask client to delete window if requested, else terminate client.
 */
Boolean CanonicalDelete(window, mask, button, x, y)
Window window;
int mask;
int x, y;
{
    AwmInfoPtr awi;
    XWMHints *wm_hints;
    static uninitialized = True;
    static XClientMessageEvent Event;
    int count, i;
    Atom *wm_protocols;

    Entry("CanonicalDelete")

    if (window == RootWindow(dpy, scr))
	Leave(FALSE)

    if(!(awi = GetAwmInfo(window)))
	Leave(FALSE)

    if(uninitialized) {
	Event.type = ClientMessage;
	Event.message_type = WmProtocols;
	Event.data.l[0] = WmDeleteWindow;
	Event.format = 32;	/* timestamp */
        /* looking at _XEventToWire, these are the fields to set */
	uninitialized = False;
    }

    if (XGetWMProtocols(dpy, awi->client, &wm_protocols, &count)) {
	/* could optimize this by caching wm_protocols here */
	for (i = 0; (i < count) && (wm_protocols[i] != Event.data.l[0]); i++);
        if(i < count) {   /* got a match */
	    /* send delete window message to client */
	    Event.window = awi->client;
	    Event.data.l[1] = (long) last_server_time; 
			     /* collected from all events */
	    XSendEvent(dpy, awi->client, False, NULL, &Event);
	    /* no need to propogate to ourself! */
	    Leave(TRUE)
	}
    }
    TerminateClient(window, mask, button, x, y);	
    Leave(TRUE)
}


/*
 * Put any necessary cleanup code here, it will be invoked when awm exits
 * or restarts. Should let SaveSets do their job and icons die naturally.
 * However, the server does not currently save window rank! Sigh...
 * Attempt to preserve original client window border width...
 * Don't bother cleaning up alloced memory.
 */
Cleanup()
{
     Window junk, *windows;
     unsigned int nwins;
     AwmInfoPtr awi;
     XWindowAttributes xwa;
     unsigned int i;

     Entry("Cleanup");
    
     /* querying the tree is slow - should just traverse awm infos.*/
     if (XQueryTree(dpy, DefaultRootWindow(dpy), &junk, &junk, &windows, &nwins)) {
	  for (i = 0; i < nwins; i++) {
	       awi = GetAwmInfo(windows[i]);
	       if (!awi)
		    continue;
	       if (IsIcon(windows[i], NULL)) {
		   if (awi->state & ST_ICON)
	         /* map the window, but leave WM_STATE iconic for wm restart */
		       XMapWindow(dpy, awi->client);
		   /* so we don't do it twice when we hit the icon. */
		   if (awi->frame) {
		       XSetWindowBorderWidth(dpy, awi->client, 
						  awi->border_width);
		       /* unreparented windows (override_redirect,
			  transient_for) do not have their border widths set */
		       XGetWindowAttributes(dpy, awi->frame, &xwa);
	    /* we must behave as if we were killed for a reliable result:
	       reparent client to be exactly where it is now (root-relative)
	       but leave it marked decorated for next restart.  NoDecorate
	       leaves client in frame, but moves it back to mapped position. */
		       if (awi->state & ST_DECORATED) {
			   if (awi->attrs & AT_TITLE)
			       xwa.y += titlePositionExcess;
			   if (awi->attrs & AT_BORDER) {
			       xwa.y += borderPositionExcess;
			       xwa.x += borderPositionExcess;
			   }
		       }
	               XReparentWindow(dpy, awi->client, RootWindow(dpy, scr),
			               xwa.x, xwa.y);
	               XRemoveFromSaveSet(dpy, awi->client);
		       /* could call NoDecorate(awi->client, TRUE) here */
	           }
	       }
	  }
	  /* could free awi's here, if they were a linked list */
	  if (windows)
	      XFree(windows);
          XSync (dpy, FALSE);   /* make sure last ops get seen before we die */
     }
     Leave_void
}


Boolean RestartSession(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
    Entry("RestartSession")

    SessionCmd = RESTART_SESSION;
    CleanupSession();
    /* wait for windows to send WM_COMMAND event */
}

Boolean EndSession(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
    Entry("EndSession")

    SessionCmd = END_SESSION;
    CleanupSession();
    /* wait for windows to send WM_COMMAND event */
}

/*
 * This is the final routine called to restart or end a session.
 * It is called from TerminateClientNow() after the kill list is
 * exhausted.  Both TerminateClientNow() and this routine depend
 * on the SessionCmd static variable to have been set by either
 * RestartSession() [RESTART_SESSION],
 * EndSession() [END_SESSION],
 * or even
 * TerminateSession() [TERMINATE_SESSION] via EndSession.
 */
void
TerminateSession()
{
    killItem *killPtr = killList;

    Entry("TerminateSession")

    /* fprintf (stderr, "TerminateSession: SessionCmd = %d, Current killList:\n", SessionCmd);  */
    while (killPtr) {
        /* fprintf (stderr, "%x, ",  killPtr->window); */
        killPtr = killPtr->next;		    
    }
    /* fprintf (stderr, "\n\n"); */

    switch(SessionCmd){
    case NULL_SESSION:
	Leave_void;
	break;

    case RESTART_SESSION:
	XSync(dpy,FALSE);	/* sync to be sure all windows dead */
	execlp(Session_File, Session_File, NULL);
	break;

    case END_SESSION:
	SessionCmd = TERMINATE_SESSION;
	/*
	 * first killList of clients emptied
	 * now kill login window
	 */
	if(LoginWindow){
	    TerminateClient(LoginWindow);
	} else {
	    Error("Ending Session without finding Login Window");
	}
	break;

    case TERMINATE_SESSION:
	/*
	 * If all went well in the above TerminateClient(LoginWindow)
	 * call, TerminateClientNow() should have killed the Login Window
	 * and this code would never be reached.  
	 * For it to be reached, the killList must empty once for all
	 * nonlogin windows, then possibly again for the login window,
	 * so it would be the second call from TerminateClientNow()
	 * that takes this path.
	 */
	Warning("Server did not shut down; Login Window may not have exited");
	XSync(dpy,FALSE);
#if defined(XMALLOCDEBUG) | defined(MALLOCDEBUG)
    TERMINATE();
#endif
	exit(1);
	break;	/* not reached */
    }

}



/*
 * Basically when we restart or end a session we want to kill all current
 * clients running.  We have to wait for those who want to die gracefully.
 * If RestartSession or EndSession has been called, NewSession will be called
 * from KillClientNow() after the last client on the kill list has been killed.
 * Should let SaveSets do their job and icons die naturally.
 * XXX Attempt to preserve original client window border width...
 */
CleanupSession()
{
    Window junk, *windows;
    int nwins;
    AwmInfoPtr awi;
    XWindowAttributes xwa;
    unsigned int i;
    Boolean LoginOrConsole;
    killItem *prevKillItem;
    killItem *killPtr = killList;

    Entry("CleanupSession");
    
    /* fprintf (stderr, "CleanupSession: Current killList:\n"); */
    while (killPtr) {
        /* fprintf (stderr, "%x, ",  killPtr->window); */
        killPtr = killPtr->next;		    
    }
    /* fprintf (stderr, "\n\n"); */

    /* in case we had an aborted restart or end session, clear the KillList */
    while (killList) {
        prevKillItem = killList;
	killList = killList->next;
	free(prevKillItem);
    }

    /*
     * XXX querying the tree is slow - since we are now registering all mapped
     * top-level windows, we could probably build a list of awi's instead.
     */
    if(XQueryTree(dpy, DefaultRootWindow(dpy), &junk, &junk, &windows, &nwins)){
	for (i = 0; i < nwins; i++) {
	    awi = GetAwmInfo(windows[i]);
	    /*
	     * assume that login and console are real top level windows
	     */
	    if(!awi){
		/*
		 * do nothing for unregistered windows - assumes
		 * override_redirect windows are registered.
		 */
		continue;
	    }

	    /*
	     * not interested in icon windows
	     */
	    if(IsIcon(windows[i], NULL)){
		continue;
	    }

	    /*
	     * From here on we know top level window is a frame window
	     * or a registered, but undecorated, client window.
	     * Both will have valid awi->client window.
	     * We are only interested in the awi->client window.
	     */

	    LoginOrConsole = FALSE;	/* initialize FALSE for each win */
	    if(IsClassResName(RES_NAME_LOGIN,awi->client)){
		LoginWindow = awi->client;
		LoginOrConsole = TRUE;
	    }
	    else if (IsClassResName(RES_NAME_CONSOLE,awi->client)){
		ConsoleWindow = awi->client;
		LoginOrConsole = TRUE;
	    }
	     
	    if( LoginOrConsole ){
		/*
		 * Copy the Cleanup() handling of an iconified 
		 * login window if we plan to restart the session.
		 * Will not bother to preserve rank like Cleanup()
		 * attempts to.
		 */
	        if(SessionCmd == RESTART_SESSION){
		    /*
		     * map the window, but leave WM_STATE iconic for wm
		     * restart.  This makes absolutely sure that the login
		     * window is mapped in case something goes wrong.
		     */
	            if( (awi->state & ST_ICON) ){
		        XMapWindow(dpy, awi->client);
		    }
		    /* so we don't do it twice when we hit the icon. */
		    XSetWindowBorderWidth(dpy, awi->client, awi->border_width);
		    /*
		     * This is actually redundant - we will call Cleanup when
		     * tekwm is restarted.
		     */
		    /* don't reparent or remove from save set */
		}
		/*
		 * We do not terminate the login window here
		 * If needed do that in TerminateSession
		 */
		continue;
	    } else {
		/*
		 * terminate as soon as possible for all non login clients
		 */
		TerminateClient(awi->client);
	    }
	}
        killPtr = killList;
        /* fprintf (stderr, "CleanupSession: New killList:\n"); */
        while (killPtr) {
            /* fprintf (stderr, "%x, ",  killPtr->window); */
            killPtr = killPtr->next;		    
        }
        /* fprintf (stderr, "\n\n"); */

	if (windows)
	    XFree(windows);
	XSync (dpy, FALSE);   /* make sure last ops get seen before we die */
        if (!killList){
	     /* 
	      * If the TerminateClient() didn't put any windows
	      * on the kill list, we must call TerminateSession()
	      * because KillClientNow() will never be called.
	      * If there were any windows on the list, we would
	      * want KillClientNow() to call TerminateSession()
	      * when the list is drained.
	      */
	    TerminateSession();
	}
    }
    Leave_void
}



/*
 * IsWmName() returns TRUE if the string passed matches the WM_NAME
 * of the window
 */
Boolean
IsWmName(match,window)
char	*match;
Window	window;
{
    char *name;

    Entry("IsWmName")

    if( XFetchName(dpy,window,&name) ){
	if( strncmp(match,name,strlen(match)) == 0 ){
	    Leave(TRUE);
	} 
    }
    Leave(FALSE);
}


Boolean
IsClassResName(match,window)
char	*match;
Window window;
{
    XClassHint *clh;

    Entry("IsClassResName")
     
    clh = XAllocClassHint();
    if (XGetClassHint(dpy, window, clh)) {
	if( strncmp(match,clh->res_name,strlen(match)) == 0 ){
	    XFree(clh->res_name);
	    XFree(clh->res_class);
	    XFree(clh);
	    Leave(TRUE);
	} 
	XFree(clh->res_name);
	XFree(clh->res_class);
	XFree(clh);
    }
    Leave(FALSE)
}


Boolean
IsClassResClass(match,window)
char	*match;
Window window;
{
    XClassHint *clh;
     
    Entry("IsClassResClass")
     
    clh = XAllocClassHint();
    if (XGetClassHint(dpy, window, clh)) {
	if( strncmp(match,clh->res_class,strlen(match)) == 0 ){
	    XFree(clh->res_name);
	    XFree(clh->res_class);
	    XFree(clh);
	    Leave(TRUE);
	} 
	XFree(clh->res_name);
	XFree(clh->res_class);
	XFree(clh);
    }
    Leave(FALSE)
}
