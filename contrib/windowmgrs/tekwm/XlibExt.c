#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: XlibExt.c,v 1.16.1.1 89/11/22 15:27:05 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1989 by Tektronix, Inc., Beaverton,
 * Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Digital Equipment, Tektronix or
 * M.I.T.  not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Digital
 * Equipment, Tektronix and M.I.T.  make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * DIGITAL EQUIPMENT, TEKTRONIX, AND M.I.T. DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT,
 * TEKTRONIX, OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#include <pwd.h>
/* above for GetHomeDir */
#include <stdio.h>
#include <ctype.h>
#include <X11/Xos.h>
#include "Xlibint.h"
#include <Xresource.h>
#include "XICCC.h"

static char *GetHomeDir (dest)
	char *dest;
{
	int uid;
	extern char *getenv();
	extern int getuid();
	extern struct passwd *getpwuid(), *getpwnam();
	struct passwd *pw;
	register char *ptr;

	if((ptr = getenv("HOME")) != NULL) {
		(void) strcpy(dest, ptr);

	} else {
		if((ptr = getenv("USER")) != NULL) {
			pw = getpwnam(ptr);
		} else {
			uid = getuid();
			pw = getpwuid(uid);
		}
		if (pw) {
			(void) strcpy(dest, pw->pw_dir);
		} else {
		        *dest = '\0';
		}
	}
	return dest;
}

static XrmDatabase GetAppUserDefaults(classname)
	char *classname;
{
	XrmDatabase rdb;
	char	filenamebuf[BUFSIZ];
	char	*filename;

	if ((filename = getenv("XAPPLRESDIR")) == NULL)
	    (void) GetHomeDir (filenamebuf);
	else
	    (void) strcpy(filenamebuf, filename);

    /* Toolkit code was silly.  And it had a bug - it could not handle
       an XAPPLRESDIR that lacked a trailing slash */
	(void) strcat(filenamebuf, "/");	/* two slashes is OK */
	(void) strcat(filenamebuf, classname);
	
	rdb = XrmGetFileDatabase(filenamebuf);

	return rdb;
}

Bool _X_trace_resources = 0;
Bool _X_print_resources = 0;
/* extern Bool _X_trace_resources;was in XGetDflt.c*/

Bool _XGetBoolResource(xdb, name, class)
    XrmDatabase xdb;
    char *name;
    char *class;
{
    XrmString type;
    XrmValue result;
    int i;

    XrmGetResource(xdb, name, class, &type, &result);
    if (result.addr) {
        for (i = 0; i < strlen(result.addr); i++)
	    result.addr[i] = tolower(result.addr[i]);
        if (*result.addr == 'y' || !strcmp(result.addr, "on")
	    || !strcmp(result.addr, "true")
	    || !strcmp(result.addr, "enable"))
	    return(1);
    }
    return(0);
}

/* extern XrmDatabase XInitDefaults();    declare here so this file is more
				     portable - really goes in Xresource.h? */

XrmDatabase XInitDefaults (dpy, progname)
    Display *dpy;			/* display for defaults.... */
    char * progname;			/* file to open in app-defaults */
{
    XrmDatabase userdb = NULL;
    XrmDatabase xdb = NULL;
    char fname[BUFSIZ];                 /* longer than any conceivable size */
    char *getenv();
    char *xenv;
    char name[BUFSIZ];
    char class[BUFSIZ];

    XrmInitialize();

    /*
     * See lib/Xtk/Initialize.c
     *
     * First, get the program defaults from the app-defaults/progname file.
     * Next, get per-app per-user defaults (for application to store).
     * These do not use app class like tk does, because we don't have a
     * hardwired class passed into GetDflts.  This should change for GetRsrc.
     * Next, get the defaults from the server; if none, then load from
     * ~/.Xdefaults.  Next, if there is an XENVIRONMENT environment variable,
     * then load that file.
     */

#ifndef XAPPLOADDIR
#define XAPPLOADDIR  "/usr/lib/X11/app-defaults/"
#endif /* XAPPLOADDIR */
    (void) strcpy(fname, XAPPLOADDIR);
    (void) strcat(fname, progname);
    xdb = XrmGetFileDatabase (fname);

    userdb = GetAppUserDefaults(progname);
    if (userdb != NULL) XrmMergeDatabases(userdb, &xdb);

    if (XResourceManagerString(dpy) == NULL) {
	fname[0] = '\0';
	(void) GetHomeDir (fname);
	(void) strcat (fname, "/.Xdefaults");
        userdb = XrmGetFileDatabase (fname);
    } else {
	userdb = XrmGetStringDatabase(XResourceManagerString(dpy));
    }
    XrmMergeDatabases (userdb, &xdb);

    if ((xenv = getenv ("XENVIRONMENT")) == NULL) {
	int len;
	fname[0] = '\0';
	(void) GetHomeDir (fname);
	(void) strcat (fname, "/.Xdefaults-");

	len = strlen (fname);
	gethostname (fname+len, BUFSIZ-len);
	xenv = fname;
    }
    userdb = XrmGetFileDatabase (xenv);
    XrmMergeDatabases (userdb, &xdb);
    /* check for a resource which requests resource tracing for debugging */
    (void) strcpy(name, progname);
    (void) strcpy(class, progname);
    class[0] = toupper(class[0]);
    (void) strcat(name, ".traceResources");
    (void) strcat(class, ".TraceResources");
    _X_trace_resources = _XGetBoolResource(xdb, name, class);
    /* check for a resource which requests a resource list to customize */
    (void) strcpy(name, progname);
    (void) strcpy(class, progname);
    class[0] = toupper(class[0]);
    (void) strcat(name, ".printResources");
    (void) strcat(class, ".PrintResources");
    _X_print_resources = _XGetBoolResource(xdb, name, class);
    return (xdb);

#ifdef old
    if (fname[0] != '\0') userdb =  XrmGetFileDatabase(fname);
    xdb = XrmGetStringDatabase(dpy->xdefaults);
    XrmMergeDatabases(userdb, &xdb);
    return xdb;
#endif
}

char _X_app_file_name[BUFSIZ] = {NULL};
/* extern char _X_app_file_name[]; was in XGetDflt.c*/

char *XGetDefault(dpy, prog, name)
	Display *dpy;			/* display for defaults.... */
	register char *name;		/* name of option program wants */
	char *prog;			/* name of program for option	*/

{					/* to get, for example, "font"  */
	char temp[BUFSIZ];
	XrmString type;
	XrmValue result;
	char *progname;

	/*
	 * strip path off of program name
	 */
	progname = rindex (prog, '/');
	if (progname)
	    progname++;
	else
	    progname = prog;

	/*
	 * see if database has ever been initialized.  Lookups can be done
	 * without locks held.
	 * Database is shared with XGetResource.
	 */
	LockDisplay(dpy);
	if (dpy->db == NULL || strcmp(_X_app_file_name, progname)) {
	    /* load from app-defaults/<progname> file, re-loading
	       if progname ever changes.
	       We assume that it will be very rare to alternate between
	       application databases, so better to keep only one database 
	       in memory at a time. */
	    if (dpy->db != NULL) {		
		XrmDestroyDatabase(dpy->db);
		/* New in R4 - stolen from there in lib/X/Xrm.c */
	    }
	    dpy->db = XInitDefaults(dpy, progname);
	    strcpy(_X_app_file_name, progname);
	}
	UnlockDisplay(dpy);

	sprintf(temp, "%s.%s", progname, name);
	XrmGetResource(dpy->db, temp, "Program.Name", &type, &result);

	if(_X_trace_resources)
	    if(result.addr)
		fprintf(stderr, "%s: got value  \t'%s'\nfor resource name \t'%s'\n             class\t'%s'\n",
		    _X_app_file_name, result.addr, temp, "Program.Name");
	    else
		fprintf(stderr, "%s: got value  \t'NULL'\nfor resource name \t'%s'\n             class\t'%s'\n",
		    _X_app_file_name, temp, "Program.Name");
	if(_X_print_resources)
	    if(result.addr)
                fprintf(stderr, "!%s: %s\n", name, result.addr);
	    else
                fprintf(stderr, "!%s: <use program default>\n", name);
	return (result.addr);
}

char *XGetResource(dpy, default_program, program, program_class, 
		   resource, resource_class)
	Display *dpy;			/* display for defaults.... */
	char *default_program;		/* dflt name of program for resource */
	char *program;			/* name of program for resource	*/
	char *program_class;		/* class of program for resource */
	register char *resource;	/* name of resource program wants */
	register char *resource_class;	/* class of resource program wants */

{
	char prog_name[BUFSIZ];
	char name[BUFSIZ];
	char class[BUFSIZ];
	XrmString type;
	XrmValue result;
	char *i, *j;

    if((default_program == NULL) || (*default_program == NULL)) {
		if(program_class == NULL) {
			/* This is illegal, at least prevent crashing */
			default_program  = "";
		}
		else {
			default_program = program_class;
		}
    }
	/*
	 * see if database has ever been initialized.  Lookups can be done
	 * without locks held.
	 * Database is shared with XGetDefault.
	 */
	LockDisplay(dpy);
	if (dpy->db == NULL || strcmp(_X_app_file_name, default_program)) {
	    /* XInitDefaults now an external function in XGetDflt.c */
	    /* load from app-defaults/<default_program> file, re-loading
	       if default_program ever changes.
	       We assume that it will be very rare to alternate between
	       application databases, so better to keep only one database 
	       in memory at a time. */
	    if (dpy->db != NULL) {		
		XrmDestroyDatabase(dpy->db);
		/* New in R4 - stolen from there in lib/X/Xrm.c */
	    }
	    dpy->db = XInitDefaults(dpy, default_program);
	    (void) strcpy(_X_app_file_name, default_program);
	}
	UnlockDisplay(dpy);

        /* but read contents using <program> for the app name! */
	if ((program == NULL) || (*program == NULL)) {
	    (void) strcpy(prog_name, default_program);
	    /* convert to an instance name in case it was a class name */
	    prog_name[0] = tolower(prog_name[0]);
	    program = prog_name;
	}
	name[0] = '\0';
	if((program_class == NULL) || (*program_class == NULL)) {
		/* default to capitalized default_program */
		*class = toupper(*default_program);
		for (i = class, j = default_program + 1; *(j - 1) != NULL;) {
		    *++i = *j++;
	    }
	}
	else {
	    char *nextclassdot = program_class;
	    char *nextnamedot = program;
	    int firstnamelen;
	    char *tmp;

	    /* extend the program names to the number of program classes */
	    /* find first program name */
	    if(tmp = (index(program, '.')))
		firstnamelen = tmp - program;
	    else
		firstnamelen = strlen(program);

	    while(nextclassdot = index(nextclassdot, '.')) {
		nextclassdot++;  /* skip to next class name */
		if(tmp = index(nextnamedot, '.')) {
		    nextnamedot = tmp + 1;
		}
		else {
		    /* fewer names than classes - prepend one */
		    (void) strncat(name, program, firstnamelen);
		    (void) strcat(name, ".");
		}
	    }

	    sprintf(class, "%s.", program_class);
	    i = class + strlen(class);
	}
	sprintf(rindex(name, '\0'), "%s.%s", program, resource);
	/* now i points to null terminator */
        if(resource_class == NULL) {
	    /* default to capitalized resource name(s) */
	    *i = toupper(*resource);
	    for (j = resource + 1; *(j - 1) != NULL;) {
		if ((*++i = *j++) == '.')
		    *++i = toupper(*j++);
	    }
	    /* NULL resource is an invalid argument - gets nothing */
	}
	else {
	    (void) strcpy(i, resource_class);
	}

	XrmGetResource(dpy->db, name, class, &type, &result);

	if(_X_trace_resources)
	    if(result.addr)
                fprintf(stderr, "%s: got value  \t'%s'\nfor resource name \t'%s'\n             class\t'%s'\n",
			_X_app_file_name, result.addr, name, class);
	    else
                fprintf(stderr, "%s: got value  \t'NULL'\nfor resource name \t'%s'\n             class\t'%s'\n",
			_X_app_file_name, name, class);
	if(_X_print_resources)
	    if(result.addr)
                fprintf(stderr, "!%s: %s\n", name, result.addr);
	    else
                fprintf(stderr, "!%s: <use program default>\n", name);
	return (result.addr);
}

/* start defining these structures right! */
typedef struct {
	short deltax;
	short deltay;
	short border_width;
} XWMDecoration;

#define SIGNEDINT long
#define UNSIGNEDINT unsigned long
/* this structure may be extended, but do not change the order */
typedef struct {
    SIGNEDINT deltax;
    SIGNEDINT deltay;
    UNSIGNEDINT border_width;
  } xPropWMDecoration;
#define NumPropWMDecorationElements 3 /* number of elements in this structure */

XWMDecoration *XGetWMDecoration (dpy, w)
	Display *dpy;
	Window w;
{
    xPropWMDecoration *prop = NULL;
    register XWMDecoration *wmdecoration;
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    static Atom Wm_decoration;
    static uninitialized = True;
	
    if(uninitialized) {
		Wm_decoration = XInternAtom(dpy, "WM_DECORATION", False);
		uninitialized = False;
    }
	if (XGetWindowProperty(dpy, w, Wm_decoration, 
	    0L, (long)NumPropWMDecorationElements,
	    False, Wm_decoration, &actual_type, &actual_format,
            &nitems, &leftover, (unsigned char **)&prop)
            != Success) return (NULL);

	/* If the property is undefined on the window, return null pointer. */

        if ((actual_type != Wm_decoration) ||
	    (nitems < NumPropWMDecorationElements) || (actual_format != 32)) {
			if (prop != NULL) Xfree ((char *)prop);
            return(NULL);
		}
	/* static copies not allowed in library, due to reentrancy constraint*/
	wmdecoration = (XWMDecoration *) Xcalloc (1, (unsigned) sizeof(XWMDecoration));
	/* this structure defined right! */
	wmdecoration->deltax = prop->deltax;
	wmdecoration->deltay = prop->deltay;
	wmdecoration->border_width = prop->border_width;
	Xfree ((char *)prop);
	return(wmdecoration);
}

/* 
 * XSetWMDecoration sets the property 
 *	WM_DECORATION 	type: WM_DECORATION	format:32
 */

XSetWMDecoration (dpy, w, wmdecoration)
	Display *dpy;
	Window w;
	XWMDecoration *wmdecoration;
{
	xPropWMDecoration prop;
	static Atom Wm_decoration;
	static uninitialized = True;
	
	if(uninitialized) {
	    Wm_decoration = XInternAtom(dpy, "WM_DECORATION", False);
	    uninitialized = False;
	}
	prop.deltax = wmdecoration->deltax;
	prop.deltay = wmdecoration->deltay;
	prop.border_width = wmdecoration->border_width;
	XChangeProperty (dpy, w, Wm_decoration, Wm_decoration, 32,
	    PropModeReplace, (unsigned char *) &prop, NumPropWMDecorationElements);
}

/* Don't statically define this structure, instead, use this routine */
XWMDecoration *XAllocWMDecoration ()	
{
    return((XWMDecoration *) Xcalloc (1, (unsigned) sizeof(XWMDecoration)));
}

/* the following should be in Xlib, but Consortium politics being what they
   are... */

/*
 * This files defines crock C structures for calling XGetWindowProperty and
 * XChangeProperty.  All fields must be longs as the semantics of property
 * routines will handle conversion to and from actual 32 bit objects.  If your
 * compiler doesn't treat &structoflongs the same as &arrayoflongs[0], you
 * will have some work to do.
 */
#define SIGNEDINT long
#define RESOURCEID unsigned long

/* this structure may be extended, but do not change the order */
typedef struct {
    SIGNEDINT state;
    RESOURCEID icon;
  } xPropWMState;
#define NumPropWMStateElements 2 /* number of elements in this structure */

XSetWMState (dpy, w, wmstate)
	Display *dpy;
	Window w;
	XWMState *wmstate;
{
	xPropWMState prop;
	static Atom Wm_state;
	static uninitialized = True;
	
	if(uninitialized) {
	    Wm_state = XInternAtom(dpy, "WM_STATE", False);
	    uninitialized = False;
	}
	prop.state = wmstate->state;
	prop.icon = wmstate->icon;
	XChangeProperty (dpy, w, Wm_state, Wm_state, 32,
	    PropModeReplace, (unsigned char *) &prop, NumPropWMStateElements);
}


/* Don't statically define this structure, instead, use this routine */
XWMState *XAllocWMState ()
{
    return((XWMState *) Xcalloc (1, (unsigned) sizeof(XWMState)));
}

XWMState *XGetWMState (dpy, w)
	Display *dpy;
	Window w;
{
    xPropWMState *prop = NULL;
    register XWMState *wmstate;
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    static Atom Wm_state;
    static uninitialized = True;
	
    if(uninitialized) {
		Wm_state = XInternAtom(dpy, "WM_STATE", False);
		uninitialized = False;
    }
	if (XGetWindowProperty(dpy, w, Wm_state, 
	    0L, (long)NumPropWMStateElements,
	    False, Wm_state, &actual_type, &actual_format,
            &nitems, &leftover, (unsigned char **)&prop)
            != Success) return (NULL);

	/* If the property is undefined on the window, return null pointer. */

        if ((actual_type != Wm_state) ||
	    (nitems < NumPropWMStateElements) || (actual_format != 32)) {
			if (prop != NULL) Xfree ((char *)prop);
            return(NULL);
		}
	/* static copies not allowed in library, due to reentrancy constraint*/
	wmstate = (XWMState *) Xcalloc (1, (unsigned) sizeof(XWMState));
	/* this structure defined right! */
	wmstate->state = prop->state;
	wmstate->icon = prop->icon;
	Xfree ((char *)prop);
	return(wmstate);
}
