#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty_init.c 20.36 89/09/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ttysw initialization, destruction and error procedures
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#include <sgtty.h>
#include <signal.h>
#include <stdio.h>
#include <utmp.h>
#include <pwd.h>
#include <ctype.h>
#include <fcntl.h>

#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>

#include <xview/win_input.h>
#include <xview_private/win_keymap.h>
#include <xview/defaults.h>
#include <xview/ttysw.h>
#include <xview/font.h>
#include <xview/cursor.h>
#include <xview/scrollbar.h>
#include <xview_private/term_impl.h>
#include <xview_private/charscreen.h>
#include <xview_private/draw_impl.h>

#define TTYSLOT_NOTFOUND(n)		((n)<=0)	/* BSD returns 0; S5
							 * returns -1 */

#if defined(WITH_3X_LIBC) || defined(vax)
/* 3.x - 4.0 libc transition code; old (pre-4.0) code must define the symbol */
#define jcsetpgrp(p)	setpgrp((p),(p))
#endif

extern char    *mktemp();
extern char    *strncpy();
extern char    *strcpy();
extern long     lseek();
char           *textsw_checkpoint_undo();

#ifndef TIOCUCNTL
#define	TIOCUCNTL	_IOW(t, 102, int)	/* pty: set/clr usr cntl mode */
#endif				/* TIOCUCNTL */
#ifndef TIOCTCNTL
#define TIOCTCNTL       _IOW(t, 32, int)	/* pty: set/clr intercept
						 * ioctl mode */
#endif				/* TIOCTCNTL */



char           *getenv();

extern PIXFONT *xv_pf_open();
Pkg_private void xv_new_tty_chr_font();

extern int      ttysel_use_seln_service;


struct ttysw_createoptions {
    int             becomeconsole;	/* be the console */
    char          **argv;	/* args to be used in exec */
    char           *args[4];	/* scratch array if need to build argv */
};

Xv_Cursor       ttysw_cursor;	/* default (text) cursor) */
Xv_Cursor       ttysw_stop_cursor;	/* stop sign cursor (i.e., CTRL-S) */

static Defaults_pairs bold_style[] = {
    "None", TTYSW_BOLD_NONE,
    "Offset_X", TTYSW_BOLD_OFFSET_X,
    "Offset_Y", TTYSW_BOLD_OFFSET_Y,
    "Offset_X_and_Y", TTYSW_BOLD_OFFSET_X | TTYSW_BOLD_OFFSET_Y,
    "Offset_XY", TTYSW_BOLD_OFFSET_XY,
    "Offset_X_and_XY", TTYSW_BOLD_OFFSET_X | TTYSW_BOLD_OFFSET_XY,
    "Offset_Y_and_XY", TTYSW_BOLD_OFFSET_Y | TTYSW_BOLD_OFFSET_XY,
    "Offset_X_and_Y_and_XY", TTYSW_BOLD_OFFSET_X |
    TTYSW_BOLD_OFFSET_Y |
    TTYSW_BOLD_OFFSET_XY,
    "Invert", TTYSW_BOLD_INVERT,
    NULL, -1
};

static Defaults_pairs inverse_and_underline_mode[] = {
    "Enable", TTYSW_ENABLE,
    "Disable", TTYSW_DISABLE,
    "Same_as_bold", TTYSW_SAME_AS_BOLD,
    NULL, -1
};

ttysw_lookup_boldstyle(str)
    char           *str;
{
    int             bstyle;
    if (str && isdigit(*str)) {
	bstyle = atoi(str);
	if (bstyle < TTYSW_BOLD_NONE || bstyle > TTYSW_BOLD_MAX)
	    bstyle = -1;
	return bstyle;
    } else {
	return defaults_lookup(str, bold_style);
    }
}

ttysw_print_bold_options()
{
    Defaults_pairs *pbold;

    (void) fprintf(stderr, "Options for boldface are %d to %d or:\n",
		   TTYSW_BOLD_NONE, TTYSW_BOLD_MAX);
    for (pbold = bold_style; pbold->name; pbold++) {
	(void) fprintf(stderr, "%s\n", pbold->name);
    }
}

/*
 * Ttysw initialization.
 */
Pkg_private     Xv_opaque
ttysw_init_view_internal(parent, tty_view_public)
    Tty             parent;
    Tty_view        tty_view_public;
{
    Ttysw_view_handle ttysw_view = (Ttysw_view_handle)
    calloc(1, sizeof(Ttysw_view_object));
    Xv_Drawable_info *info;

    /* Allocate private object and set links so TTY_PRIVATE/PUBLIC works.  */
    if (ttysw_view == 0)
	return ((Xv_opaque) NULL);

    ((Xv_tty_view *) tty_view_public)->private_data = (Xv_opaque) ttysw_view;
    ttysw_view->public_self = (Tty_view) tty_view_public;
    ttysw_view->folio = TTY_PRIVATE_FROM_ANY_PUBLIC(parent);
    ttysw_view->folio->current_view_public = tty_view_public;
    ttysw_view->folio->view = ttysw_view;

    /* Bug alert: what about XV_HELP_DATA */


    if (xv_tty_imageinit(ttysw_view->folio, tty_view_public) == 0) {
	free((char *) ttysw_view);
	return ((Xv_opaque) NULL);
    }
    /* create stop cursor but don't show it */
    DRAWABLE_INFO_MACRO(tty_view_public, info);	/* define info */
    ttysw_stop_cursor = xv_get(xv_server(info), XV_KEY_DATA, CURSOR_STOP_PTR);
    if (!ttysw_stop_cursor) {
	ttysw_stop_cursor = xv_create(tty_view_public, CURSOR,
				      CURSOR_SRC_CHAR, OLC_STOP_PTR,
				      CURSOR_MASK_CHAR, 0,
				      0);
	xv_set(xv_server(info),
	       XV_KEY_DATA, CURSOR_STOP_PTR, ttysw_stop_cursor,
	       0);
    }
    xv_set(tty_view_public,
	   WIN_RETAINED, xv_get(xv_screen(info), SCREEN_RETAIN_WINDOWS),
	   XV_HELP_DATA, "xview:ttysw",
	   0);

    return ((Xv_opaque) ttysw_view);

}

Pkg_private     Xv_opaque
ttysw_init_folio_internal(tty_public)
    Tty             tty_public;
{
    extern          ttysw_eventstd();
    Ttysw_folio     ttysw;
    Xv_opaque       font = NULL;
    int             is_client_pane;

    /* Allocate private object and set links so TTY_PRIVATE/PUBLIC works.  */
    ttysw = (Ttysw_folio) calloc(1, sizeof(Ttysw));
    if (ttysw == 0)
	return ((Xv_opaque) NULL);

    ((Xv_tty *) tty_public)->private_data = (Xv_opaque) ttysw;
    ttysw->public_self = (Tty) tty_public;

    ttysw->ttysw_eventop = ttysw_eventstd;
    /* Following call only affect appearance of ttysw, not termsw */
    (void) ttysw_setboldstyle(
			      defaults_lookup(
					      (char *) defaults_get_string("term.boldStyle", "Term.BoldStyle", "Invert"),
					      bold_style));
    (void) ttysw_set_inverse_mode(
				  defaults_lookup(
						  (char *) defaults_get_string("term.inverseStyle", "Term.InverseStyle", "Enable"),
					       inverse_and_underline_mode));
    (void) ttysw_set_underline_mode(
				    defaults_lookup(
						    (char *) defaults_get_string("term.underlineStyle", "Term.UnderlineStyle", "Enable"),
					       inverse_and_underline_mode));

    ttysw->ttysw_ibuf.cb_rbp = ttysw->ttysw_ibuf.cb_buf;
    ttysw->ttysw_ibuf.cb_wbp = ttysw->ttysw_ibuf.cb_buf;
    ttysw->ttysw_ibuf.cb_ebp =
	&ttysw->ttysw_ibuf.cb_buf[sizeof(ttysw->ttysw_ibuf.cb_buf)];
    ttysw->ttysw_obuf.cb_rbp = ttysw->ttysw_obuf.cb_buf;
    ttysw->ttysw_obuf.cb_wbp = ttysw->ttysw_obuf.cb_buf;
    ttysw->ttysw_obuf.cb_ebp =
	&ttysw->ttysw_obuf.cb_buf[sizeof(ttysw->ttysw_obuf.cb_buf)];
    ttysw->ttysw_kmtp = ttysw->ttysw_kmt;

    (void) ttysw_readrc(ttysw);

    xv_set(tty_public, XV_HELP_DATA, "xview:ttysw", 0);

    if (ttyinit(ttysw) == -1) {
	free((char *) ttysw);
	perror("ttysw_init_internal");
	return ((Xv_opaque) NULL);
    }
    (void) ttysw_ansiinit(ttysw);

    /* initialize selection service code */
    (void) ttysw_setopt((char *)ttysw, TTYOPT_SELSVC, ttysel_use_seln_service);
    if (ttysw_getopt(ttysw, TTYOPT_SELSVC)) {
	ttysel_init_client(ttysw);
    }
    (void) ttysw_mapsetim(ttysw);

    is_client_pane = (int) xv_get((Xv_object) tty_public, WIN_IS_CLIENT_PANE);
    if (is_client_pane) {
	Xv_opaque       parent_font;
	int             scale, size;
	char           *name = defaults_get_string("font.name", "Font.Name", (char *) 0);

	if (name && (strlen(name) > 0)) {
	    font = (Xv_opaque) xv_pf_open(name);
	} else
	    font = (Xv_opaque) 0;
	if (!font) {
	    parent_font = (Xv_opaque) xv_get(tty_public, WIN_FONT);
	    scale = (int) xv_get(parent_font, FONT_SCALE);
	    if (scale > 0) {
		font = (Xv_opaque) xv_find(tty_public, FONT,
				FONT_FAMILY, FONT_FAMILY_DEFAULT_FIXEDWIDTH,
		/* FONT_FAMILY,	FONT_FAMILY_SCREEN, */
		       FONT_SCALE, (scale > 0) ? scale : FONT_SCALE_DEFAULT,
					   0);
	    } else {
		size = (int) xv_get(parent_font, FONT_SIZE);
		font = (Xv_opaque) xv_find(tty_public, FONT,
				FONT_FAMILY, FONT_FAMILY_DEFAULT_FIXEDWIDTH,
		/* FONT_FAMILY,	FONT_FAMILY_SCREEN, */
			   FONT_SIZE, (size > 0) ? size : FONT_SIZE_DEFAULT,
					   0);
	    }
	}
    } else {
	Xv_opaque       parent_font = (Xv_opaque) xv_get(tty_public, WIN_FONT);
	int             scale = (int) xv_get(parent_font, FONT_SCALE);
	if (scale > 0) {
	    font = (Xv_opaque) xv_find(tty_public, FONT,
				FONT_FAMILY, FONT_FAMILY_DEFAULT_FIXEDWIDTH,
	    /* FONT_FAMILY,	FONT_FAMILY_SCREEN, */
		       FONT_SCALE, (scale > 0) ? scale : FONT_SCALE_DEFAULT,
				       0);
	} else {
	    int             size = (int) xv_get(parent_font, FONT_SIZE);
	    font = (Xv_opaque) xv_find(tty_public, FONT,
				FONT_FAMILY, FONT_FAMILY_DEFAULT_FIXEDWIDTH,
	    /* FONT_FAMILY,	FONT_FAMILY_SCREEN, */
			   FONT_SIZE, (size > 0) ? size : FONT_SIZE_DEFAULT,
				       0);
	}
    }
    if (!font)
	font = (Xv_opaque) xv_get(tty_public, WIN_FONT);

    xv_new_tty_chr_font((Pixfont *)font);

    return ((Xv_opaque) ttysw);
}

int
ttysw_add_FNDELAY(fd)
    int             fd;
{
    int             fdflags;

    if ((fdflags = fcntl(fd, F_GETFL, 0)) == -1)
	return (-1);
    fdflags |= FNDELAY;
    if (fcntl(fd, F_SETFL, fdflags) == -1)
	return (-1);
    return (0);
}


/* ARGSUSED */
int
ttysw_fork_it(ttysw0, argv, wfd)
    caddr_t         ttysw0;
    char          **argv;
    int             wfd;	/* No longer used. */
{
    struct ttysubwindow *ttysw = (struct ttysubwindow *) ttysw0;
    struct ttysw_createoptions opts;
    int             pidchild;
    unsigned        ttysw_error_sleep = 2;
    struct sigvec   vec, ovec;
    int pgrp;

    ttysw->ttysw_pidchild = fork();
    if (ttysw->ttysw_pidchild < 0)
	return (-1);
    if (ttysw->ttysw_pidchild) {
	if (ttysw_add_FNDELAY(ttysw->ttysw_pty)) {
	    perror("fcntl");
	    /* Press on, as child is already launched. */
	}
	/* Sys V compatibility: signal() -> sigvec() */
/*	vec.sv_handler = SIG_IGN;
	vec.sv_mask = vec.sv_onstack = 0;
	sigvec(SIGTSTP, &vec, 0);
*/
#ifdef DEBUG
	sleep(3);
#endif				/* DEBUG */
	return (ttysw->ttysw_pidchild);
    }
    /* Move Sys V & we don't care about SIGWINCH's */
    vec.sv_handler = SIG_DFL;
    vec.sv_mask = vec.sv_onstack = 0;
    sigvec(SIGWINCH, &vec, 0);

    /*
     * Change process group of child process (me at this point in code) so
     * its signal stuff doesn't affect the terminal emulator.
     */
    pidchild = pgrp = getpid();
    if (setpgrp(0, pidchild) < 0) {
	/* setpgrp call failed. Find out what my pgrp is and use that */
    	pgrp = getpgrp(0);
    }

    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    sigvec(SIGTTOU, &vec, &ovec);

    if ((ioctl(ttysw->ttysw_tty, TIOCSPGRP, &pgrp)) == -1) {
	sleep(ttysw_error_sleep);
	perror("ttysw-TIOCSPGRP");
    }

    sigvec(SIGTTOU, &ovec, 0);

    /*
     * Initialize file descriptors. Connections to servers are marked as
     * close-on-exec, and don't need any further cleanup.
     */
    (void) close(ttysw->ttysw_pty);
    (void) dup2(ttysw->ttysw_tty, 0);
    (void) dup2(ttysw->ttysw_tty, 1);
    (void) dup2(ttysw->ttysw_tty, 2);
    (void) close(ttysw->ttysw_tty);

    if (*argv == (char *) NULL || strcmp("-c", *argv) == 0) {
	/* Process arg list */
	int             argc;

	for (argc = 0; argv[argc]; argc++);
	ttysw_parseargs(&opts, &argc, argv);
	argv = opts.argv;
    }

    /* restore various signals to their defaults */
    signal (SIGINT, SIG_DFL);
    signal (SIGQUIT, SIG_DFL);
    signal (SIGTERM, SIG_DFL);

    execvp(*argv, argv);
    perror(*argv);
    /* Wait a few seconds so that message can be read on ttysw */
    sleep(ttysw_error_sleep);
    exit(1);
    /* NOTREACHED */
}

/*
 * Create options calls
 */
ttysw_parseargs(opts, argcptr, argv_base)
    struct ttysw_createoptions *opts;
    int            *argcptr;
    char          **argv_base;
{
    char           *shell;
    int             argc = *argcptr;
    char          **argv = argv_base;
    char          **args = argv;

    bzero((caddr_t) opts, sizeof(*opts));
    /* Get options */
    while (argc > 0) {
	if (strcmp(argv[0], "-C") == 0 ||
	    strcmp(argv[0], "CONSOLE") == 0) {
	    opts->becomeconsole = 1;
	    (void) xv_cmdline_scrunch(argcptr, argv_base, argv, 1);
	} else
	    argv++;
	argc--;
    }
    argv = args;
    opts->argv = opts->args;
    /* Determine what shell to run. */
    shell = getenv("SHELL");
    if (!shell || !*shell)
	shell = "/bin/sh";
    opts->args[0] = shell;
    /* Setup remainder of arguments */
    if (*argv == (char *) NULL) {
	opts->args[1] = (char *) NULL;
#ifndef someday			/* this should go away */
    } else if (strcmp("-c", *argv) == 0) {
	/*
	 * The '-c' flag tells the shell to run following arg
	 */
	opts->args[1] = argv[0];
	opts->args[2] = argv[1];
	(void) xv_cmdline_scrunch(argcptr, argv_base, argv, 2);
	opts->args[3] = (char *) NULL;
#endif
    } else {
	/*
	 * Run program not under shell
	 */
	opts->argv = argv;
    }
    return;
}

/*
 * Ttysw cleanup.
 */
Pkg_private void
ttysw_done(ttysw_folio_private)
    Ttysw_folio     ttysw_folio_private;
{

    if (ttysw_folio_private->ttysw_ttyslot)
	(void) updateutmp("", ttysw_folio_private->ttysw_ttyslot,
			  ttysw_folio_private->ttysw_tty);

    ttysel_destroy(ttysw_folio_private);

    if (ttysw_folio_private->ttysw_pty)
	close(ttysw_folio_private->ttysw_pty);
    if (ttysw_folio_private->ttysw_tty)
	close(ttysw_folio_private->ttysw_tty);

    free((char *) ttysw_folio_private);
}

/*
 * Do tty/pty setup
 */
static int
ttyinit(ttysw)
    Ttysw          *ttysw;
{
    int             tt, tmpfd, ptynum = 0;
    int             pty = 0, tty = 0;
    int             on = 1;
    char            ptylet = 'p', linebuf[20], *line = &linebuf[0];
    struct stat     stb;

    /*
     * find unopened pty
     */
needpty:
    while (ptylet <= 's') {
	(void) strcpy(line, "/dev/ptyXX");
	line[strlen("/dev/pty")] = ptylet;
	line[strlen("/dev/ptyp")] = '0';
	if (stat(line, &stb) < 0)
	    break;
	for (ptynum = 0; ptynum < 16; ptynum++) {
	    line[strlen("/dev/ptyp")] = "0123456789abcdef"[ptynum];
	    pty = open(line, O_RDWR);
	    if (pty > 0)
		goto gotpty;
	}
	ptylet++;
    }
    (void) fprintf(stderr, "All pty's in use\n");
    return (-1);
    /* NOTREACHED */
gotpty:
    line[strlen("/dev/")] = 't';
    tt = open("/dev/tty", O_RDWR);
    if (tt > 0) {
	(void) ioctl(tt, TIOCNOTTY, 0);
	(void) close(tt);
    }
    tty = open(line, O_RDWR);
    if (tty < 0) {
	ptynum++;
	(void) close(pty);
	goto needpty;
    }
#define	WE_TTYPARMS	"WINDOW_TTYPARMS"
    if (ttysw_restoreparms(tty))
	(void) unsetenv(WE_TTYPARMS);

    /*
     * Copy stdin.  Set stdin to tty so ttyslot in updateutmp will think this
     * is the control terminal.  Restore state. Note: ttyslot should have
     * companion ttyslotf(fd).
     */
    tmpfd = dup(0);
    (void) close(0);
    (void) dup(tty);
    ttysw->ttysw_ttyslot = updateutmp((char *) 0, 0, tty);
    (void) close(0);
    (void) dup(tmpfd);
    (void) close(tmpfd);
    ttysw->ttysw_tty = tty;
    ttysw->ttysw_pty = pty;
#ifdef sun
    if (ioctl(ttysw->ttysw_pty, TIOCTCNTL, &on) == -1) {
	perror("TTYSW - setting TIOCTCNTL to 1 failed");
    }
#else				/* sun */
    if (ioctl(ttysw->ttysw_pty, TIOCPKT, &on) < 0) {
	perror("TTYSW - setting TIOCPKT to 1 failed");
    }
#endif

    return (0);
}

/*
 * Make entry in /etc/utmp for ttyfd. Note: this is dubious usage of
 * /etc/utmp but many programs (e.g. sccs) look there when determining who is
 * logged in to the pty.
 */
int
updateutmp(username, ttyslotuse, ttyfd)
    char           *username;
    int             ttyslotuse, ttyfd;
{
    /*
     * Update /etc/utmp
     */
    struct utmp     utmp;
    struct passwd  *passwdent;
    extern struct passwd *getpwuid();
    int             f;
    char           *ttyn;
    extern char    *ttyname(), *rindex();

    if (!username) {
	/*
	 * Get login name
	 */
	if ((passwdent = getpwuid(getuid())) == (struct passwd *) NULL) {
	    (void) fprintf(stderr, "couldn't find user name\n");
	    return (0);
	}
	username = passwdent->pw_name;
    }
    utmp.ut_name[0] = '\0';	/* Set incase *username is 0 */
    (void) strncpy(utmp.ut_name, username, sizeof(utmp.ut_name));
    /*
     * Get line (tty) name
     */
    ttyn = ttyname(ttyfd);
    if (ttyn == (char *) NULL)
	ttyn = "/dev/tty??";
    (void) strncpy(utmp.ut_line, rindex(ttyn, '/') + 1, sizeof(utmp.ut_line));
    /*
     * Set host to be empty
     */
    (void) strncpy(utmp.ut_host, "", sizeof(utmp.ut_host));
    /*
     * Get start time
     */
    (void) time((time_t *) (&utmp.ut_time));
    /*
     * Put utmp in /etc/utmp
     */
    if (ttyslotuse == 0)
	ttyslotuse = ttyslot();

    if (TTYSLOT_NOTFOUND(ttyslotuse)) {
	(void) fprintf(stderr,
		 "Cannot find slot in /etc/ttys for updating /etc/utmp.\n");
	(void) fprintf(stderr, "Commands like \"who\" will not work.\n");
	(void) fprintf(stderr, "Add tty[qrs][0-f] to /etc/ttys file.\n");
	return (0);
    }
    if ((f = open("/etc/utmp", 1)) >= 0) {
	(void) lseek(f, (long) (ttyslotuse * sizeof(utmp)), 0);
	(void) write(f, (char *) &utmp, sizeof(utmp));
	(void) close(f);
    } else {
	(void) fprintf(stderr, "make sure that you can write /etc/utmp!\n");
	return (0);
    }
    return (ttyslotuse);
}
