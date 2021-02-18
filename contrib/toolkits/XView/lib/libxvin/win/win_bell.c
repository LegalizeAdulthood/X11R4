#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)win_bell.c 20.14 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Win_bell.c: Implement the keyboard bell.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <pixrect/pixrect.h>
#include <xview/base.h>
#include <xview/pixwin.h>
#include <xview/defaults.h>
#include <xview/rect.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>

/* Bell cached defaults */
static Bool     win_do_audible_bell;
static Bool     win_do_visible_bell;
static int      win_bell_done_init;

/*
 * Ring bell and flash window
 */
win_bell(window, tv, pw)
    Xv_object       window;
    struct timeval  tv;
    register Xv_object pw;
{
    Xv_Drawable_info *info;
    Display        *display;
    Rect            r;

    DRAWABLE_INFO_MACRO(window, info);
    display = xv_display(info);
    /* Get defaults for bell if  first time */
    if (!win_bell_done_init) {
	win_do_audible_bell = (Bool) defaults_get_boolean(
			     "alarm.audible", "Alarm.Audible", (Bool) TRUE);
	win_do_visible_bell = (Bool) defaults_get_boolean(
			     "alarm.visible", "Alarm.Visible", (Bool) TRUE);
	win_bell_done_init = 1;
    }
    /* Flash pw */
    if (pw && win_do_visible_bell) {
	(void) win_getsize(window, &r);
	(void) pw_writebackground(pw, 0, 0,
				  r.r_width, r.r_height, PIX_NOT(PIX_DST));
    }
    /* Ring bell */
    /* BUG: is this right? */
    if (win_do_audible_bell)
	XBell(display, 100);
    /* Wait */
    (void) win_blocking_wait(tv);
    /* Turn off flash */
    if (pw && win_do_visible_bell) {
	(void) pw_writebackground(pw, 0, 0,
				  r.r_width, r.r_height, PIX_NOT(PIX_DST));
    }
}

win_blocking_wait(wait_tv)
    struct timeval  wait_tv;
{
    extern struct timeval ndet_tv_subt();	/* From notifier code */
    struct timeval  start_tv, now_tv, waited_tv;
    int             bits;

    /* Get starting time */
    (void) gettimeofday(&start_tv, (struct timezone *) 0);
    /* Wait */
    while (timerisset(&wait_tv)) {
	/* Wait for awhile in select */
	bits = 0;
	(void) select(0, &bits, &bits, &bits, &wait_tv);
	/* Get current time */
	(void) gettimeofday(&now_tv, (struct timezone *) 0);
	/* Compute how long waited */
	waited_tv = ndet_tv_subt(now_tv, start_tv);
	/* Subtract time waited from time left to wait */
	wait_tv = ndet_tv_subt(wait_tv, waited_tv);
    }
}
