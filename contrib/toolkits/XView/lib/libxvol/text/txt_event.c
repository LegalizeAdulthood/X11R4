#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_event.c 20.29 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * New style, notifier-based, event and timer support by text subwindows.
 */

#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <errno.h>
#include <xview_private/win_keymap.h>

/* the state of this flag signifies if the scrollbar update	 */
/* action will be synchronous or asynchronous in nature		 */
/* this flag is known to and checked in txt_scroll.c		 */
int             textsw_timer_active = FALSE;

extern int      errno;
extern Notify_error win_post_event();
extern Ev_status ev_set();
extern void     ev_blink_caret();
extern int      termsw_menu_active();
extern int      termsw_menu_inactive();

#define	INVERT_CARET_IF_IDLE(textsw)\
	if (!TXTSW_IS_BUSY(textsw)) textsw_invert_caret(textsw)
static void     textsw_reset_timer();

pkg_private     Textsw_folio
textsw_folio_for_view(view)
    Textsw_view_handle view;
{
    ASSUME(view->magic == TEXTSW_VIEW_MAGIC);
    ASSUME(view->folio->magic == TEXTSW_MAGIC);
    return (view->folio);
}

/*
 *	there are a couple of crashes that appear to
 *	end up in this routine. therefore a blanket is 
 *	being placed around it to protect minimally 
 *	from bad pointer problems. If anything goes
 *	wrong a NULL pointer is passed back, which
 *	will bubble logic errors back out to the 
 *	place where (we hope) they originate. 6/20/89
 */
pkg_private     Textsw_view_handle
textsw_view_abs_to_rep(abstract)
    Textsw          abstract;
{
    Textsw_view_handle view;

    if( abstract == NULL )
	return NULL;

    view = VIEW_PRIVATE(abstract);

    if( view == NULL )
	return NULL;

    if (view->magic != TEXTSW_VIEW_MAGIC) {
	Textsw_folio    folio = TEXTSW_PRIVATE(abstract);

   	if( folio == NULL )
		return NULL;

	view = folio->first_view;
    }
    return (view);
}

pkg_private     Textsw
textsw_view_rep_to_abs(rep)
    Textsw_view_handle rep;
{
    ASSUME((rep == 0) || (rep->magic == TEXTSW_VIEW_MAGIC));
    return (VIEW_PUBLIC(rep));
}

/* ARGSUSED */
pkg_private     Notify_value
textsw_event(view_public, event, arg, type)
    Textsw_view     view_public;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type type;	/* Currently unused */
{
    Textsw_view_handle view = VIEW_PRIVATE(view_public);
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    register int    process_status;
    Notify_value    result = NOTIFY_DONE;
    Xv_object       frame;
    static short    read_status;

    folio->state |= TXTSW_DOING_EVENT;
    switch (event_id(event)) {
      case ACTION_RESCALE:
	/* don't need to do anything since frame will refont us */
	break;
      case KBD_USE:
	textsw_take_down_caret(folio);
	textsw_add_timer(folio, &folio->timer);
	folio->state |= TXTSW_HAS_FOCUS;
	(void) ev_set(view->e_view, EV_CHAIN_CARET_IS_GHOST, FALSE, 0);
	if (frame = xv_get(FOLIO_REP_TO_ABS(folio), WIN_FRAME)) {
	    frame_kbd_use(frame, FOLIO_REP_TO_ABS(folio));
	}
	goto Default;
      case KBD_DONE:
	folio->state &= ~TXTSW_HAS_FOCUS;
	textsw_take_down_caret(folio);	/* resets timer */
	(void) ev_set(view->e_view, EV_CHAIN_CARET_IS_GHOST, TRUE, 0);
	if (frame = xv_get(FOLIO_REP_TO_ABS(folio), WIN_FRAME)) {
	    frame_kbd_done(frame, FOLIO_REP_TO_ABS(folio));
	}
	goto Default;
      case WIN_MAP_NOTIFY:
	view->state |= TXTSW_VIEW_IS_MAPPED;
	goto Default;
      case WIN_UNMAP_NOTIFY:
	view->state &= ~TXTSW_VIEW_IS_MAPPED;
	goto Default;

      default:{
    Default:
	    process_status = textsw_process_event(view_public, event, arg);
	    /*
	     * BUG ALERT: there is no longer a notion of "the first event in
	     * this group", after which the old textsw_input_func called
	     * textsw_flush_caches(view, TFC_STD) to force pending deletes to
	     * be applied "as soon as appropriate".  In this code, the
	     * textsw_reset_timer will "eventually" cause the pending deletes
	     * and delayed insertion to happen, but can be "held off" by
	     * continual user actions.
	     */

	    if (view->state & TXTSW_VIEW_IS_MAPPED)
		textsw_reset_timer(folio);
	    if (process_status & TEXTSW_PE_READ_ONLY) {
		if (!read_status) {
		    textsw_read_only_msg(folio, event_x(event), event_y(event));
		    read_status = 1;
		}
	    } else if (process_status == 0) {
		result = NOTIFY_IGNORED;
		if (read_status)
		    read_status = 0;
		goto Return;
	    }
	    break;
	}
    }
Return:
    folio->state &= ~TXTSW_DOING_EVENT;
    return notify_next_event_func(view_public, (Notify_event) event, arg, type);
}

/* ARGSUSED */
pkg_private     Notify_value
textsw_timer_expired(folio, which)
    register Textsw_folio folio;
    int             which;	/* Currently unused */
{
    static int      timer;

    if ((folio->to_insert_next_free != folio->to_insert) &&
	((folio->func_state & TXTSW_FUNC_FILTER) == 0)) {
	textsw_flush_caches(folio->first_view, TFC_STD);
    }
    if (timer++ == 3) {		/* every 6 that the timer is set blink */
	timer = 0;

	/*
	 * prevent the caret from being rendered if there is a menu being
	 * handled
	 */
	if (termsw_menu_inactive())
	    INVERT_CARET_IF_IDLE(folio);

    }
    /* jcb -- update the scrollbars if needed */
    textsw_real_update_scrollbars(folio);

    if (!TXTSW_IS_READ_ONLY(folio)) {
	if (((folio->state & TXTSW_HAS_FOCUS) &&
	     (folio->caret_state & TXTSW_CARET_FLASHING)) ||
	    !(folio->caret_state & TXTSW_CARET_ON))
	    /* if caret is flashing and have kbd focus, or if caret is off */
	    textsw_add_timer(folio, &folio->timer);
	else
	    textsw_timer_active = FALSE;
    } else
	textsw_timer_active = FALSE;

    return (NOTIFY_DONE);
}

pkg_private void
textsw_add_timer(folio, timeout)
    register Textsw_folio folio;
    register struct timeval *timeout;
{
    struct itimerval itimer;

    itimer = NOTIFY_NO_ITIMER;
    itimer.it_value = *timeout;
    if (NOTIFY_FUNC_NULL ==
	notify_set_itimer_func((Notify_client) folio,
			       textsw_timer_expired, ITIMER_REAL,
			       &itimer, (struct itimerval *) 0)) {
	notify_perror("textsw_add_timer");
	textsw_timer_active = FALSE;
    } else {
	if (itimer.it_value.tv_usec != 0)
	    textsw_timer_active = FALSE;	/* asynchronous update mode
						 * if TRUE */
	else
	    textsw_timer_active = FALSE;
    }
}

pkg_private void
textsw_remove_timer(folio)
    register Textsw_folio folio;

{

    textsw_add_timer(folio, &NOTIFY_NO_ITIMER.it_value);
    textsw_timer_active = FALSE;

    if (folio->caret_state & TXTSW_CARET_ON) {
	textsw_invert_caret(folio);
    }
}

/*
 * Pull any existing timer (there need not be one) and add a fresh timer.
 * This results in "extending" the timeout on an existing timer.
 */
static void
textsw_reset_timer(folio)
    register Textsw_folio folio;
{

    textsw_add_timer(folio, &NOTIFY_NO_ITIMER.it_value);
    textsw_add_timer(folio, &folio->timer);

}

pkg_private void
textsw_take_down_caret(textsw)
    register Textsw_folio textsw;
{
    if (textsw->caret_state & TXTSW_CARET_ON) {
	textsw_invert_caret(textsw);
    }
    /* if caret is not flashing or we don't have kbd focus */
    if (!(textsw->caret_state & TXTSW_CARET_FLASHING) ||
	!(textsw->state & TXTSW_HAS_FOCUS)) {
	/* if timer is not already set, set the timer */
	if (notify_get_itimer_func((Notify_client) textsw, ITIMER_REAL)
	    == NOTIFY_FUNC_NULL)
	    textsw_add_timer(textsw, &textsw->timer);
    }
}

pkg_private void
textsw_invert_caret(textsw)
    register Textsw_folio textsw;
{

    if (TXTSW_IS_READ_ONLY(textsw) &&
	((textsw->caret_state & TXTSW_CARET_ON) == 0))
	return;
    ev_blink_caret(textsw->views, !(textsw->caret_state & TXTSW_CARET_ON));
    textsw->caret_state ^= TXTSW_CARET_ON;
}
