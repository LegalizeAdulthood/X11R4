#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty_main.c 20.38 89/12/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Very active terminal emulator subwindow pty code.
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sgtty.h>
#include <ctype.h>

#include <xview/win_struct.h>
#include <xview/win_input.h>
#include <xview/cursor.h>
#include <xview/pixwin.h>
#include <xview/icon.h>
#include <xview/ttysw.h>
#include <xview/notice.h>
#include <xview/frame.h>

#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <pixrect/pixfont.h>
#include <xview/notify.h>
#include <xview/win_notify.h>
#include <xview_private/term_impl.h>
#include <xview/sel_svc.h>

extern int      errno;
extern void     ttysetselection();
extern Textsw_index textsw_insert();
Pkg_private void     ttysw_lighten_cursor();

/*
 * jcb	-- remove continual cursor repaint in shelltool windows also known to
 * ttyansi.c
 */

extern int      do_cursor_draw;
extern int      tty_new_cursor_row, tty_new_cursor_col;
Pkg_private void ttysw_show_walkmenu();


/* shorthand */
#define	iwbp	ttysw->ttysw_ibuf.cb_wbp
#define	irbp	ttysw->ttysw_ibuf.cb_rbp
#define	iebp	ttysw->ttysw_ibuf.cb_ebp
#define	ibuf	ttysw->ttysw_ibuf.cb_buf
#define	owbp	ttysw->ttysw_obuf.cb_wbp
#define	orbp	ttysw->ttysw_obuf.cb_rbp
#define	oebp	ttysw->ttysw_obuf.cb_ebp
#define	obuf	ttysw->ttysw_obuf.cb_buf

/* #ifdef TERMSW */
/*
 * The basic strategy for building a line-oriented command subwindow
 * (terminal emulator subwindow) on top of the text subwindow is as follows.
 * 
 * The idle state has no user input still to be processed, and no outstanding
 * active processes at the other end of the pty (except the shell).
 * 
 * When the user starts creating input events, they are passed through to the
 * textsw unless they fall in the class of "activating events" (which right
 * now consists of \n, \r, \033, ^C and ^Z).  In addition, the end of the
 * textsw's backing store is recorded when the first event is created.
 * 
 * When an activating event is entered, all of the characters in the textsw from
 * the recorded former end to the current end of the backing store are added
 * to the list of characters to be sent to the pty.  In addition, the current
 * end is set to be the insertion place for response from the pty.
 * 
 * If the user has started to enter a command, then in order to avoid messes on
 * the display, the first response from the pty will be suffixed with a \n
 * (unless it ends in a \n), and the pty will be marked as "owing" a \n.
 * 
 * In the meantime, if the user continues to create input events, they are
 * appended at the end of the textsw, after the response from the pty.  When
 * an activating event is entered, all of the markers, etc. are updated as
 * described above.
 * 
 * The most general situation is: Old stuff in the log file ^User editing here
 * More old stuff Completed commands enqueued for the pty Pty inserting here^
 * (Prompt)Partial user command
 */

/* #endif TERMSW */


/*
 * Main pty processing.
 */

/*
 * Write window input to pty
 */
ttysw_pty_output(ttysw, pty)
    register Ttysw_folio ttysw;
    int             pty;
{
    register int    cc;

    if (ttysw_getopt(ttysw, TTYOPT_TEXT)) {
	Termsw_folio    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw));

	if (termsw->pty_eot > -1) {
	    char           *eot_bp = ibuf + termsw->pty_eot;
	    char            nullc = '\0';

	    /* write everything up to pty_eot */
	    if (eot_bp > irbp) {
		cc = write(pty, irbp, eot_bp - irbp);
		if (cc > 0) {
		    irbp += cc;
		} else if (cc < 0) {
		    perror("TTYSW pty write failure");
		}
	    }
	    /* write the eot */
	    if (irbp == eot_bp) {
		cc = write(pty, &nullc, 0);
		if (cc < 0) {
		    perror("TTYSW pty write failure");
		} else {
		    termsw->pty_eot = -1;
		}
	    }
	}			/* termsw->pty_eot > -1 */
	/* only write the rest of the buffer if it doesn't have an eot in it */
	if (termsw->pty_eot > -1)
	    return;
    }
    if (iwbp > irbp) {
	cc = write(pty, irbp, iwbp - irbp);
	if (cc > 0) {
	    irbp += cc;
	    if (irbp == iwbp)
		irbp = iwbp = ibuf;
	} else if (cc < 0) {
	    perror("TTYSW pty write failure");
	}
    }
}

void
ttysw_process_STI(ttysw, cp, cc)
    register Ttysw_folio ttysw;
    register char  *cp;
    register int    cc;
{
    register short  post_id;
    register Textsw textsw;
    register Termsw_folio termsw;
    Textsw_index    pty_index;
    Textsw_index    cmd_start;

#ifdef DEBUG
    fprintf(stderr, "STI ");
    fwrite(cp, 1, cc, stderr);
    fprintf(stderr, "\n");
#endif				/* DEBUG */
    if (!TTY_IS_TERMSW(ttysw)) {
	return;
    }
    textsw = TEXTSW_FROM_TTY(ttysw);
    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    /* Assume app wants STI text echoed at cursor position */
    if (termsw->cooked_echo) {
	pty_index = textsw_find_mark(textsw, termsw->pty_mark);
	if (termsw->cmd_started) {
	    cmd_start = textsw_find_mark(textsw, termsw->user_mark);
	} else {
	    cmd_start = (Textsw_index) window_get(textsw, TEXTSW_LENGTH);
	}
	if (cmd_start > pty_index) {
	    if (termsw->append_only_log) {
		textsw_remove_mark(textsw, termsw->read_only_mark);
	    }
	    (void) textsw_delete(textsw, pty_index, cmd_start);
	    if (termsw->append_only_log) {
		termsw->read_only_mark =
		    textsw_add_mark(textsw, pty_index, TEXTSW_MARK_READ_ONLY);
	    }
	    termsw->pty_owes_newline = 0;
	}
    }
    /* Pretend STI text came in from textsw window fd */
    while (cc > 0) {
	post_id = (short) (*cp);
	win_post_id(textsw, post_id, NOTIFY_SAFE);
	cp++;
	cc--;
    }
    /* flush caches */
    (void) window_get(textsw, TEXTSW_LENGTH);
}

/*
 * Read pty's input (which is output from program)
 */
ttysw_pty_input(ttysw, pty)
    register Ttysw_folio ttysw;
    int             pty;
{
    static struct iovec iov[2];
    register int    cc;
    char            ucntl;
    register unsigned int_ucntl;
    register Termsw_folio termsw;

    /* readv avoids need to shift packet header out of owbp. */
    iov[0].iov_base = &ucntl;
    iov[0].iov_len = 1;
    iov[1].iov_base = owbp;
    iov[1].iov_len = oebp - owbp;

    cc = readv(pty, iov, 2);
    if (cc < 0 && errno == EWOULDBLOCK)
	cc = 0;
    else if (cc <= 0)
	cc = -1;
    if (cc > 0) {
	int_ucntl = (unsigned) ucntl;
	if (int_ucntl != 0 && ttysw_getopt(ttysw, TTYOPT_TEXT)) {
	    unsigned        tiocsti = TIOCSTI;
	    if (int_ucntl == (tiocsti & 0xff)) {
		ttysw_process_STI(ttysw, owbp, cc - 1);
	    }
	    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw));
	    (void) ioctl(ttysw->ttysw_tty, TIOCGETC, &termsw->tchars);
	    (void) ioctl(ttysw->ttysw_tty, TIOCGLTC, &termsw->ltchars);
	    (void) ttysw_getp(TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw));	/* jcb for nng */
	} else {
	    owbp += cc - 1;
	}
    }
}

/*
 * Send program output to terminal emulator.
 */
ttysw_consume_output(ttysw_view)
    Ttysw_view_handle ttysw_view;

{
    register Ttysw_folio ttysw = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    short           is_not_text;
    int             cc;

    /* cache the cursor removal and re-render once in this set -- jcb */
    if (is_not_text = !ttysw_getopt(ttysw, TTYOPT_TEXT)) {
	(void) ttysw_removeCursor();
	do_cursor_draw = FALSE;
    }
    while (owbp > orbp && !(ttysw->ttysw_flags & TTYSW_FL_FROZEN)) {
	if (is_not_text) {
	    if (ttysw->ttysw_primary.sel_made) {
		ttysel_deselect(&ttysw->ttysw_primary, SELN_PRIMARY);
	    }
	    if (ttysw->ttysw_secondary.sel_made) {
		ttysel_deselect(&ttysw->ttysw_secondary, SELN_SECONDARY);
	    }
	}
	cc = ttysw_output_it(ttysw_view, orbp, owbp - orbp);
	orbp += cc;
	if (orbp == owbp)
	    orbp = owbp = obuf;
    }

    if (is_not_text) {
	(void) ttysw_drawCursor(tty_new_cursor_row, tty_new_cursor_col);
	do_cursor_draw = TRUE;
    }
}

/*
 * Add the string to the input queue.
 */
ttysw_input_it(ttysw, addr, len)
    register Ttysw_folio ttysw;
    char           *addr;
    register int    len;
{
    Textsw          textsw;

    if (ttysw_getopt(ttysw, TTYOPT_TEXT)) {
	textsw = TEXTSW_FROM_TTY(ttysw);
	(void) textsw_insert(textsw, addr, (long int) len);
    } else {
	Ttysw_view_handle ttysw_view = TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw);
	if (iwbp + len >= iebp) {
	    /* Input buffer would overflow, so tell user and discard chars.  */
	    (void) notice_prompt(
				 xv_get(TTY_PUBLIC(ttysw), WIN_FRAME),
				 (Event *) 0,
				 NOTICE_MESSAGE_STRINGS,
			  "Too many characters to add to the input buffer.",
			     "Wait a few seconds after you click Continue,",
				 "then retype the missing characters.",
				 0,
				 NOTICE_BUTTON_YES, "Continue",
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
	    return (0);
	}
	bcopy(addr, iwbp, len);
	iwbp += len;
	ttysw->ttysw_lpp = 0;	/* reset page mode counter */
	if (ttysw->ttysw_flags & TTYSW_FL_FROZEN)
	    (void) ttysw_freeze(ttysw_view, 0);
	if (!(ttysw->ttysw_flags & TTYSW_FL_IN_PRIORITIZER)) {
	    (void) ttysw_reset_conditions(ttysw_view);
	}
    }
    return (len);
}

/* #ifndef TERMSW */
ttysw_handle_itimer(ttysw)
    register Ttysw_folio ttysw;
{
    if (ttysw->ttysw_primary.sel_made) {
	ttysel_deselect(&ttysw->ttysw_primary, SELN_PRIMARY);
    }
    if (ttysw->ttysw_secondary.sel_made) {
	ttysel_deselect(&ttysw->ttysw_secondary, SELN_SECONDARY);
    }
    (void) ttysw_pdisplayscreen(0);
}

/*
 * handle standard events.
 */
Xv_public int
ttysw_eventstd(ttysw_view_public, ie)
/* This could be a public ttysw view or termsw view */
    register Tty_view ttysw_view_public;
    register struct inputevent *ie;
{
    Frame           frame_public;
    register Ttysw_folio ttysw = TTY_PRIVATE_FROM_ANY_VIEW(ttysw_view_public);
    register Tty    tty_public = TTY_PUBLIC(ttysw);

    switch (event_action(ie)) {
      case KBD_USE:
      case KBD_DONE:
	frame_public = (Frame) xv_get(tty_public, WIN_OWNER);
	switch (event_action(ie)) {
	  case KBD_USE:
	    (void) ttysw_restore_cursor();
	    (void) frame_kbd_use(frame_public, tty_public);
	    return TTY_DONE;
	  case KBD_DONE:
	    ttysw_lighten_cursor();
	    (void) frame_kbd_done(frame_public, tty_public);
	    return TTY_DONE;
	}
      case WIN_REPAINT:
      case WIN_GRAPHICS_EXPOSE:
	if (TTY_IS_TERMSW(ttysw)) {
	    Termsw_view_handle termsw = TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw);

	    if (termsw->folio->cmd_started) {
		(void) ttysw_scan_for_completed_commands(TTY_VIEW_PRIVATE(ttysw_view_public), -1, 0);
	    }
	}
	(void) ttysw_display(ttysw);
	return (TTY_DONE);
      case WIN_RESIZE:
	(void) ttysw_resize(TTY_VIEW_PRIVATE_FROM_ANY_PUBLIC(ttysw_view_public));
	return (TTY_DONE);
      case ACTION_SELECT:
	return ttysw_process_point(ttysw, ie);
      case ACTION_ADJUST:
	return ttysw_process_adjust(ttysw, ie);

      case ACTION_MENU:{
            if (event_is_down(ie)) {
	        ttysw_show_walkmenu(ttysw_view_public, ie);
	        ttysw->ttysw_butdown = ACTION_MENU;
	    }

	    return (TTY_DONE);
	}

#ifdef notdef			/* BUG ALERT */
	/*
	 * 11 Sept 87: Alok found that if we do the exit processing, we turn
	 * off LOC_WINEXIT and thus defeat the auto-generation of KBD_DONE by
	 * xview_x_input_readevent.  Until we incorporate a fix in the lower input
	 * code we comment out this optimization.
	 */
      case LOC_WINEXIT:
	return ttysw_process_exit(ttysw, ie);
#endif
      case LOC_MOVEWHILEBUTDOWN:
	return ttysw_process_motion(ttysw, ie);
      default:
	return ttysw_process_keyboard(ttysw, ie);
    }
}

static int
ttysw_process_point(ttysw, ie)
    register Ttysw_folio ttysw;
    register struct inputevent *ie;
{
    if (win_inputposevent(ie)) {
	ttysw->ttysw_butdown = ACTION_SELECT;
	ttysel_make(ttysw, ie, 1);
    } else {
	if (ttysw->ttysw_butdown == ACTION_SELECT) {
	    (void) ttysel_adjust(ttysw, ie, FALSE, FALSE);
	    ttysetselection(ttysw);
	}
	/* ttysw->ttysw_butdown = 0; */
    }
    return TTY_DONE;
}

static int
ttysw_process_adjust(ttysw, ie)
    register Ttysw_folio ttysw;
    register struct inputevent *ie;
{
    if (win_inputposevent(ie)) {
	(void) ttysel_adjust(ttysw, ie, TRUE,
			     (ttysw->ttysw_butdown == ACTION_ADJUST));
	/* Very important for this to be set after the call to ttysel_adjust */
	ttysw->ttysw_butdown = ACTION_ADJUST;
    } else {
	if (ttysw->ttysw_butdown == ACTION_ADJUST) {
	    (void) ttysel_adjust(ttysw, ie, FALSE, FALSE);
	    ttysetselection(ttysw);
	}
	/* ttysw->ttysw_butdown = 0; */
    }
    return TTY_DONE;
}

static int
ttysw_process_motion(ttysw, ie)
    register Ttysw_folio ttysw;
    register struct inputevent *ie;
{
    if ((ttysw->ttysw_butdown == ACTION_SELECT) ||
	(ttysw->ttysw_butdown == ACTION_ADJUST))
	(void) ttysel_adjust(ttysw, ie, FALSE, FALSE);
    return TTY_DONE;
}

static int
ttysw_process_keyboard(ttysw, ie)
    Ttysw_folio     ttysw;
    struct inputevent *ie;

{
    register int    id = event_id(ie);

    if (event_action(ie) == ACTION_HELP)
	return (ttysw_domap(ttysw, ie));

    if ((id >= ASCII_FIRST && id <= META_LAST) && (win_inputposevent(ie))) {
	char            c = (char) id;

	/*
	 * State machine for handling logical caps lock, ``F1'' key.
	 * Capitalize characters except when an ESC goes by.  Then go into a
	 * state where characters are passed uncapitalized until an
	 * alphabetic character is passed.  We presume that all ESC sequences
	 * end with an alphabetic character.
	 * 
	 * Used to solve the function key problem where the final `z' is is
	 * being capitalized. (Bug id: 1005033)
	 */
	if (ttysw->ttysw_capslocked & TTYSW_CAPSLOCKED) {
	    if (ttysw->ttysw_capslocked & TTYSW_CAPSSAWESC) {
		if (isalpha(c))
		    ttysw->ttysw_capslocked &= ~TTYSW_CAPSSAWESC;
	    } else {
		if (islower(c))
		    c = toupper(c);
		else if (c == '\033')
		    ttysw->ttysw_capslocked |= TTYSW_CAPSSAWESC;
	    }
	}
	(void) ttysw_input_it(ttysw, &c, 1);
#ifdef TTY_ACQUIRE_CARET
	if (!ttysw->ttysw_caret.sel_made) {
	    ttysel_acquire(ttysw, SELN_CARET);
	}
#endif
	return TTY_DONE;
    }
    if (id > META_LAST) {
	return ttysw_domap(ttysw, ie);
    }
    return TTY_OK;
}

/* #endif TERMSW */

/*
 * After the character array image changes size, this routine must be called
 * so that pty knows about the new size.
 */
xv_tty_new_size(ttysw, cols, lines)
    Ttysw_folio     ttysw;
    int             cols, lines;

{
#ifdef sun
    /*
     * The ttysize structure and TIOCSSIZE and TIOCGSIZE ioctls are available
     * only on Suns.
     */

    struct ttysize  ts;
    struct sigvec vec, ovec;
  
    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    (void) sigvec(SIGTTOU, &vec, &ovec);

    ts.ts_lines = lines;
    ts.ts_cols = cols;
    if ((ioctl(ttysw->ttysw_tty, TIOCSSIZE, &ts)) == -1)
	perror("ttysw-TIOCSSIZE");

    (void) sigvec(SIGTTOU, &ovec, 0);
#else
    /*
     * Otherwise, we use the winsize struct  and TIOCSWINSZ ioctl.
     */
    struct winsize  ws;
    struct sigvec vec, ovec;

    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    (void) sigvec(SIGTTOU, &vec, &ovec);

    ws.ws_row = lines;
    ws.ws_col = cols;
    if ((ioctl(ttysw->ttysw_tty, TIOCSWINSZ, &ws)) == -1)
	perror("ttysw-TIOCSWINSZ");

    (void) sigvec(SIGTTOU, &ovec, 0);
#endif
}


/*
 * Freeze tty output.
 */
Pkg_private int
ttysw_freeze(ttysw_view, on)
    Ttysw_view_handle ttysw_view;
    int             on;
{
    register Ttysw_folio ttysw = ttysw_view->folio;
    register Tty_view ttysw_view_public = TTY_PUBLIC(ttysw_view);
    extern Xv_Cursor ttysw_cursor;
    extern Xv_Cursor ttysw_stop_cursor;

    if (!ttysw_cursor)
	ttysw_cursor = xv_get(ttysw_view_public, WIN_CURSOR);
    if (!(ttysw->ttysw_flags & TTYSW_FL_FROZEN) && on) {
	struct sgttyb   sgttyb;

	(void) ioctl(ttysw->ttysw_tty, TIOCGETP, (char *) &sgttyb);
	if ((sgttyb.sg_flags & (RAW | CBREAK)) == 0) {
	    xv_set(ttysw_view_public,
		   WIN_CURSOR, ttysw_stop_cursor,
		   0);
	    ttysw->ttysw_flags |= TTYSW_FL_FROZEN;
	} else
	    ttysw->ttysw_lpp = 0;
    } else if ((ttysw->ttysw_flags & TTYSW_FL_FROZEN) && !on) {
	xv_set(ttysw_view_public,
	       WIN_CURSOR, ttysw_cursor,
	       0);
	ttysw->ttysw_flags &= ~TTYSW_FL_FROZEN;
	ttysw->ttysw_lpp = 0;
    }
    return ((ttysw->ttysw_flags & TTYSW_FL_FROZEN) != 0);
}

/*
 * Set (or reset) the specified option number.
 */
ttysw_setopt(ttysw_folio_or_view, opt, on)
    Xv_opaque       ttysw_folio_or_view;
    int             opt, on;
{
    Tty             folio_or_view_public = TTY_PUBLIC((Ttysw_folio) ttysw_folio_or_view);
    Ttysw_view_handle ttysw_view;
    Ttysw_folio     ttysw_folio;
    int             result = 0;

    if (IS_TTY_VIEW(folio_or_view_public) ||
	IS_TERMSW_VIEW(folio_or_view_public)) {
	ttysw_view = (Ttysw_view_handle) ttysw_folio_or_view;
	ttysw_folio = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    } else {
	ttysw_folio = (Ttysw_folio) ttysw_folio_or_view;
	ttysw_view = TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw_folio);
    }

    switch (opt) {
      case TTYOPT_TEXT:	/* termsw */
	if (on)
	    result = ttysw_be_termsw(ttysw_view);
	else
	    result = ttysw_be_ttysw(ttysw_view);
    }
    if (result != -1) {
	if (on)
	    ttysw_folio->ttysw_opt |= 1 << opt;
	else
	    ttysw_folio->ttysw_opt &= ~(1 << opt);
    }
}

ttysw_getopt(ttysw, opt)
    Ttysw_folio     ttysw;
    int             opt;
{
    return ((ttysw->ttysw_opt & (1 << opt)) != 0);
}

ttysw_flush_input(ttysw)
    Ttysw_folio     ttysw;
{
    struct sigvec   vec, ovec;	/* Sys V compatibility */
    int flushf = 0;

    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    (void) sigvec(SIGTTOU, &vec, &ovec);

    /* Flush tty input buffer */
    if (ioctl(ttysw->ttysw_tty, TIOCFLUSH, &flushf))
	perror("TIOCFLUSH");

    (void) sigvec(SIGTTOU, &ovec, (struct sigvec *) 0);

    /* Flush ttysw input pending buffer */
    irbp = iwbp = ibuf;
}
