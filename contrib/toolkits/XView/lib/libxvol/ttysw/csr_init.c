#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)csr_init.c 20.18 89/10/10";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Character screen initialization and cleanup routines.
 */

#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/pixwin.h>
#include <xview/win_struct.h>
#include <xview/win_input.h>
#include <xview/window.h>
#include <xview_private/charimage.h>
#include <xview_private/charscreen.h>

Xv_Window       csr_pixwin;

/*
 * Character screen initialization
 */
Pkg_private int
wininit(win, maximagewidth, maximageheight)
    Xv_object       win;
    int            *maximagewidth, *maximageheight;
{
    struct inputmask im;
    struct rect     rect;
    struct rect    *prect;
    Xv_Window       rootwindow;

    /*
     * Set input masks
     */
    (void) win_getinputmask(win, &im, 0);
    im.im_flags |= IM_ASCII;
    im.im_flags |= IM_META;
    im.im_flags |= IM_NEGEVENT;
    win_setinputcodebit(&im, KBD_USE);
    win_setinputcodebit(&im, KBD_DONE);
    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, MS_MIDDLE);
    win_setinputcodebit(&im, MS_RIGHT);
    win_unsetinputcodebit(&im, LOC_WINENTER);
    win_unsetinputcodebit(&im, LOC_WINEXIT);

    win_setinputcodebit(&im, LOC_DRAG);
    (void) win_setinputmask(win, &im, 0, 0);
    /*
     * pixwin should be set in tty_init
     */
    /*
     * Setup max image sizes.
     */
    rootwindow = (Xv_Window) xv_get(csr_pixwin, XV_ROOT);
    prect = (struct rect *) xv_get(rootwindow, WIN_RECT);
    rect = *prect;
    *maximagewidth = rect.r_width - chrleftmargin;
    if (*maximagewidth < 0)
	*maximagewidth = 0;
    *maximageheight = rect.r_height;
    /*
     * Determine sizes
     */
    (void) win_getsize(win, &rect);
    winwidthp = rect.r_width;
    winheightp = rect.r_height;

    return (1);
}

Pkg_private void
xv_new_tty_chr_font(font)
    Pixfont        *font;
{

    pixfont = font;		/* BUG ALERT: Remove all these globals! */
    chrwidth = font->pf_defaultsize.x;
    chrheight = font->pf_defaultsize.y;
    chrbase = -(font->pf_char['n'].pc_home.y);
}
