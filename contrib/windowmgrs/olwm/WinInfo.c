/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) WinInfo.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "menu.h"
#include "olwm.h"
#include "st.h"

extern void	*IntMalloc();

st_table	*wiHashTable;

static
wiCompare( w1, w2 )
	register char	*w1, *w2;
{
	return ((Window)w1) - ((Window)w2);
}

static int
wiHash( w1, modulus )
	register char	*w1;
	register int	modulus;
{
	return ((Window)w1) % modulus;
}

/*
 * initialize the hash tables
 * returns: True	- success
 *	    False	- failure
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
WIInit( dpy )
	Display	*dpy;
{
	wiHashTable = st_init_table(wiCompare, wiHash);
}

/*
 * create a storage area for associated window information
 * returns: allocated storage
 */
WinInfo *
WICreateInfo(win)
	Window	win;
{
	WinInfo	*new;

	new = (WinInfo *)IntMalloc( (unsigned)sizeof(WinInfo) );
	st_insert(wiHashTable, (int)win, (char *)new);

	/* zero out the new structure; memset 2nd arg is int (uses as char) */
	memset((char *)new, 0, sizeof(WinInfo));

	/* fill this in for caller */
	new->self = win;

	return new;
}

/*
 * delete storage for window information
 * returns: True	- window entry deleted
 *          False	- window entry not found
 */
int
WIDeleteInfo(win)
	Window	win;
{
	WinInfo	*oldInfo;
	Window	tmpWin = win;
	Window  *tmpWinPtr = &tmpWin;

	if (st_delete(wiHashTable, (char *)tmpWinPtr, (char *)&oldInfo))
	{
		/* lint will complain about this cast */
		free((char *)oldInfo);
		return True;
	}
	else
		return False;
}

/*
 * retrieve information associated with a window
 * returns: Pointer to WinInfo struct if window is found
 *	    NULL if not found
 */
WinInfo *
WIGetInfo(win)
	Window	win;
{
	WinInfo	*winInfo = (WinInfo *)0;

	st_lookup(wiHashTable, win, &winInfo);

	return winInfo;
}
