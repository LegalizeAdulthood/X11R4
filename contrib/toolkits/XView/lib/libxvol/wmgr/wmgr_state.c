#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)wmgr_state.c 20.13 89/10/10";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Window mgr open/close and top/bottom.
 */

#include <stdio.h>
#include <sys/file.h>
#include <signal.h>
#include <pixrect/pixrect_hs.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/base.h>
#include <xview/pkg.h>
#include <xview/window_hs.h>
#include <xview/win_input.h>
#include <xview/cursor.h>
#include <xview/fullscreen.h>
#include <xview/wmgr.h>


void
wmgr_changelevel(window, parent, top)
    register Xv_object window;
    int             parent;
    int             top;
{
    int             topchildnumber, bottomchildnumber;

    (void) win_lockdata(window);
    /*
     * Don't try to optimize by not doing anything if already at desired
     * level.  Doing so messes up the fixup list because callers of this
     * routine do partial repair which incorrectly removes some stuff from
     * the damage list if a win_remove/win_insert pair hasn't been done.
     */
    /*
     * Remove from tree
     */
    (void) win_remove(window);
    /*
     * Set new links
     */
    if (top) {
	topchildnumber = win_getlink(parent, WL_TOPCHILD);
	(void) win_setlink(window, WL_COVERED, topchildnumber);
	(void) win_setlink(window, WL_COVERING, WIN_NULLLINK);
    } else {
	bottomchildnumber = win_getlink(parent, WL_BOTTOMCHILD);
	(void) win_setlink(window, WL_COVERING, bottomchildnumber);
	(void) win_setlink(window, WL_COVERED, WIN_NULLLINK);
    }
    /*
     * Insert into tree
     */
    (void) win_insert(window);
    (void) win_unlockdata(window);
    return;
}
