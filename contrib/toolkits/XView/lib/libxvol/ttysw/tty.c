#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty.c 20.36 89/09/12";
#endif
#endif

/*****************************************************************/
/* tty.c                           */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/*****************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/sun.h>
#include <xview/frame.h>
#include <xview/notify.h>
#include <xview/tty.h>
#include <xview/ttysw.h>
#include <xview/textsw.h>
#include <xview/termsw.h>
#include <xview/defaults.h>
#include <xview_private/term_impl.h>
#include <xview/scrollbar.h>
#include <xview_private/charscreen.h>
#define	_NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef	_NOTIFY_MIN_SYMBOLS

#define HELP_INFO(s) XV_HELP_DATA, s,

extern char    *getenv();
extern caddr_t  textsw_checkpoint_undo();
extern Attr_avlist attr_find();

static int      tty_quit_on_death(), tty_handle_death();

Pkg_private Xv_Window csr_pixwin;
Pkg_private Notify_value ttysw_event();
Pkg_private void ttysw_interpose();

Pkg_private Menu ttysw_walkmenu();
Pkg_private int ttytlsw_escape();
Pkg_private int ttytlsw_string();
Pkg_private Ttysw *ttysw_init_internal();
Pkg_private Xv_opaque ttysw_init_folio_internal();
Pkg_private Xv_opaque ttysw_init_view_internal();
Pkg_private void xv_new_tty_chr_font();

Pkg_private int tty_folio_init();
Pkg_private Xv_opaque ttysw_folio_set();
Pkg_private Xv_opaque ttysw_folio_get();
Pkg_private int ttysw_folio_destroy();

Pkg_private int tty_view_init();
Pkg_private Xv_opaque ttysw_view_set();
Pkg_private Xv_opaque ttysw_view_get();
Pkg_private int ttysw_view_destroy();
extern int      ttysw_textsw_changed();



/*****************************************************************************/
/* Ttysw init routines for folio and  view	                             */
/*****************************************************************************/
/*ARGSUSED*/
Pkg_private int
tty_folio_init(parent, tty_public, avlist)
    Xv_Window       parent;
    Tty             tty_public;
    Tty_attribute   avlist[];
{
    Ttysw_folio     ttysw;	/* Private object data */

    ttysw = (Ttysw_folio) (ttysw_init_folio_internal(tty_public));
    if (!ttysw)
	return (XV_ERROR);

    ttysw->hdrstate = HS_BEGIN;
    ttysw->ttysw_stringop = ttytlsw_string;
    ttysw->ttysw_escapeop = ttytlsw_escape;
    (void) xv_set(tty_public,
		  WIN_MENU, ttysw_walkmenu(tty_public),
		  0);
    ttysw_interpose(ttysw);
    return (XV_OK);
}

/*ARGSUSED*/
Pkg_private int
tty_view_init(parent, tty_view_public, avlist)
    Xv_Window       parent;	/* Tty public folio */
    Tty_view        tty_view_public;
    Tty_attribute   avlist[];
{
    Ttysw_view_handle ttysw_view;	/* Private object data */

    /*
     * BUG ALERT!  Re-arrange code to pass this pixwin into the appropriate
     * layer instead of just smashing it set from here!
     */
    csr_pixwin = tty_view_public;


    ttysw_view = (Ttysw_view_handle) (ttysw_init_view_internal(parent, tty_view_public));

    if (!ttysw_view)
	return (XV_ERROR);


    /* ttysw_walkmenu() can only be called after public self linked to */
    (void) xv_set(tty_view_public,
		  WIN_NOTIFY_SAFE_EVENT_PROC, ttysw_event,
		  WIN_NOTIFY_IMMEDIATE_EVENT_PROC, ttysw_event,
		  0);

    /* ttysw_interpose(ttysw_view); */

    /* Draw cursor on the screen and retained portion */
    (void) ttysw_drawCursor(0, 0);
    return (XV_OK);
}




/***************************************************************************
ttysw_set_internal
*****************************************************************************/
static          Xv_opaque
ttysw_set_internal(tty_public, avlist)
    Tty             tty_public;
    Tty_attribute   avlist[];
{
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(tty_public);
    register Tty_attribute *attrs;
    static int      quit_tool;
    int             pid = -1, bold_style = -1, argv_set = 0;
    char          **argv = 0;
    int             do_fork = FALSE;
    char           *buf;
    int            *buf_used;
    int             buf_len;

    for (attrs = avlist; *attrs; attrs = tty_attr_next(attrs)) {
	switch (attrs[0]) {

	  case TTY_ARGV:
	    do_fork = TRUE;
	    argv_set = 1;
	    argv = (char **) attrs[1];
	    break;

	  case TTY_CONSOLE:
	    if (attrs[1]) {
#ifdef sun			/* Vaxen do not support the TIOCCONS ioctl */
		if ((ioctl(ttysw->ttysw_tty, TIOCCONS, 0)) == -1)
		    perror("ttysw-TIOCCONS");
#endif
	    };
	    break;

	  case TTY_INPUT:
	    buf = (char *) attrs[1];
	    buf_len = (int) attrs[2];
	    buf_used = (int *) attrs[3];
	    *buf_used = ttysw_input_it(ttysw, buf, buf_len);
	    break;

	  case TTY_PAGE_MODE:
	    (void) ttysw_setopt(TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw), TTYOPT_PAGEMODE, (int)
				(attrs[1]));
	    break;

	  case TTY_QUIT_ON_CHILD_DEATH:
	    quit_tool = (int) attrs[1];
	    break;

	  case TTY_BOLDSTYLE:
	    (void) ttysw_setboldstyle((int) attrs[1]);
	    break;

	  case TTY_BOLDSTYLE_NAME:
	    bold_style = ttysw_lookup_boldstyle((char *) attrs[1]);
	    if (bold_style == -1)
		(void) ttysw_print_bold_options();
	    else
		(void) ttysw_setboldstyle(bold_style);
	    break;

	  case TTY_INVERSE_MODE:
	    (void) ttysw_set_inverse_mode((int) attrs[1]);
	    break;

	  case TTY_PID:
	    do_fork = TRUE;
	    /* TEXTSW_INFINITY ==> no child process, 0 ==> we want one */
	    /* BUG ALERT: need validity check on (int)attrs[1]. */
	    ttysw->ttysw_pidchild = (int) attrs[1];
	    break;

	  case TTY_UNDERLINE_MODE:
	    (void) ttysw_set_underline_mode((int) attrs[1]);
	    break;

	  case WIN_FONT:
	    {

		if (attrs[1]) {
		    /*
		     * Cursor for the original font has been drawn, so take
		     * down
		     */
		    ttysw_removeCursor();
		    (void) xv_new_tty_chr_font((Pixfont *)attrs[1]);
		    /* after changing font size, cursor needs to be re-drawn */
		    (void) ttysw_drawCursor(0, 0);
		}
		break;
	    }

	  case XV_END_CREATE:
	    /*
	     * xv_create(0, TTY, 0) should fork a default shell, but
	     * xv_create(0, TTY, TTY_ARGV, TTY_ARGV_DO_NOT_FORK, 0) should
	     * not fork anything (ttysw_pidchild will == TEXTSW_INFINITY >
	     * 0).
	     */
	    if (!do_fork && ttysw->ttysw_pidchild <= 0)
		do_fork = TRUE;
	    if (ttysw->view)
		ttysw_resize(ttysw->view);

	  default:
	    (void) xv_check_bad_attr(TTY, attrs[0]);
	    break;
	}
    }

    /*
     * WARNING. For certain sequences of calls, the following code loses
     * track of the process id of the current child, and could be tricked
     * into having multiple children executing at once.
     */
    if ((int) argv == TTY_ARGV_DO_NOT_FORK) {
	ttysw->ttysw_pidchild = TEXTSW_INFINITY;
    } else {
	if (argv_set && ttysw->ttysw_pidchild == TEXTSW_INFINITY) {
	    ttysw->ttysw_pidchild = 0;
	}
	if (ttysw->ttysw_pidchild <= 0 && do_fork) {
	    pid = ttysw_fork_it((char *) (ttysw), argv ? argv : (char **) &argv,
				0);
	    if (pid > 0) {
		(void) notify_set_wait3_func((Notify_client) ttysw,
				(Notify_func) (quit_tool ? tty_quit_on_death
					       : tty_handle_death),
					     pid);
	    }
	}
    }

    return (XV_OK);
}

Pkg_private     Xv_opaque
ttysw_folio_set(ttysw_folio_public, avlist)
    Tty             ttysw_folio_public;
    Tty_attribute   avlist[];
{
    return (ttysw_set_internal(ttysw_folio_public, avlist));

}

Pkg_private     Xv_opaque
ttysw_view_set(ttysw_view_public, avlist)
    Tty_view        ttysw_view_public;
    Tty_attribute   avlist[];
{
    return (ttysw_set_internal(ttysw_view_public, avlist));

}




/*****************************************************************************/
/* ttysw_get_internal        				                     */
/*****************************************************************************/
/*VARARGS*/ /*ARGSUSED*/
static          Xv_opaque
ttysw_get_internal(tty_public, status, attr, args)
    Tty             tty_public;
    int            *status;
    Tty_attribute   attr;
    va_list         args;
{
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(tty_public);

    switch (attr) {
      case OPENWIN_VIEW_CLASS:
	return ((Xv_opaque) TTY_VIEW);

      case TTY_PAGE_MODE:
	return (Xv_opaque) ttysw_getopt((char *) (ttysw), TTYOPT_PAGEMODE);

      case TTY_QUIT_ON_CHILD_DEATH:
	return (Xv_opaque) 0;

      case TTY_PID:
	return (Xv_opaque) ttysw->ttysw_pidchild;

      case TTY_PTY_FD:
	return (Xv_opaque) ttysw->ttysw_pty;

      case TTY_TTY_FD:
	return (Xv_opaque) ttysw->ttysw_tty;

      case WIN_TYPE:		/* SunView1.X compatibility */
	return (Xv_opaque) TTY_TYPE;

      default:
	if (xv_check_bad_attr(TTY, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return ((Xv_opaque) 0);
    }
}


Pkg_private     Xv_opaque
ttysw_folio_get(ttysw_folio_public, status, attr, args)
    Tty             ttysw_folio_public;
    int            *status;
    Tty_attribute   attr;
    va_list         args;
{
    return (ttysw_get_internal(ttysw_folio_public, status, attr, args));

}

Pkg_private     Xv_opaque
ttysw_view_get(ttysw_view_public, status, attr, args)
    Tty_view        ttysw_view_public;
    int            *status;
    Tty_attribute   attr;
    va_list         args;
{
    return (ttysw_get_internal(ttysw_view_public, status, attr, args));

}


/* ARGSUSED */
static
tty_quit_on_death(client, pid, status, rusage)
    caddr_t         client;
    int             pid;
    union wait     *status;
    struct rusage  *rusage;
{
    Ttysw_folio     ttysw = (Ttysw_folio) client;
    Tty             tty_public = TTY_PUBLIC(ttysw);
    Xv_object       frame;

    if (!(WIFSTOPPED(*status))) {
	if (status->w_termsig || status->w_retcode || status->w_coredump) {
	    (void) fprintf(stderr,
		       "A %s window has exited because its child exited.\n",
			   TTY_IS_TERMSW(ttysw) ? "command" : "tty");
	    (void) fprintf(stderr, "Its child's process id was %d and it", pid);
	    if (status->w_termsig) {
		(void) fprintf(stderr, " died due to signal %d",
			       status->w_termsig);
	    } else if (status->w_retcode) {
		(void) fprintf(stderr, " exited with return code %d",
			       status->w_retcode);
	    }
	    if (status->w_coredump) {
		(void) fprintf(stderr, " and left a core dump.\n");
	    } else {
		(void) fprintf(stderr, ".\n");
	    }
	}
	frame = xv_get(tty_public, WIN_FRAME);
	(void) xv_set(frame, FRAME_NO_CONFIRM, TRUE, 0);
	xv_destroy(frame);

    }
}

/* ARGSUSED */
static
tty_handle_death(tty_folio_private, pid, status, rusage)
    Ttysw_folio     tty_folio_private;
    int             pid;
    union wait     *status;
    struct rusage  *rusage;
{
    if (!(WIFSTOPPED(*status))) {
	tty_folio_private->ttysw_pidchild = 0;
    }
}


Pkg_private int
ttysw_view_destroy(ttysw_view_public, status)
    Tty_view        ttysw_view_public;
    Destroy_status  status;
{
    Ttysw_view_handle ttysw_view_private =
    TTY_VIEW_PRIVATE_FROM_ANY_VIEW(ttysw_view_public);


    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF)) {
	free((char *) ttysw_view_private);
    }
    return (XV_OK);
}

Pkg_private int
ttysw_folio_destroy(ttysw_folio_public, status)
    Ttysw_folio     ttysw_folio_public;
    Destroy_status  status;
{
    return (ttysw_destroy(ttysw_folio_public, status));
}
