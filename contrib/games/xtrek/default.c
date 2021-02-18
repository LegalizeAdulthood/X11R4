/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#define  BUFSIZ  2048

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
	 */
        /* this code modified not to initialize the server's db via .Xdefaults
         *  so it doesn't do funky things in the server based environment.
	 *
	 * only change from original MIT code.
         */
	/*
	LockDisplay(dpy);
	*/
	if (dpy->db == NULL)
		return (caddr_t) NULL;
	/*
	UnlockDisplay(dpy);
	*/
	sprintf(temp, "%s.%s", progname, name);
	XrmGetResource(dpy->db, temp, "Program.Name", &type, &result);

	return (result.addr);
}
