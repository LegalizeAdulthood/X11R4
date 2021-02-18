#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_parse.c 20.24 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Do standard parse on argv to defaults.
 */

#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#else
#include <stdio.h>
#endif
#include <xview/defaults.h>
#include <xview/pkg.h>
#include <xview/xv_error.h>
#include <X11/Xlib.h>


Xv_private int  disable_screen_saver;
Xv_private int  lockstep_with_server;
Xv_private int  list_fonts;

#ifdef _XV_DEBUG
Xv_private int  server_gather_stats;
int             sview_gprof_start;
#endif

typedef struct {
    char           *short_name, *long_name;
    char           *def_name[2];/* defaults name(s) */
    char            num_args;
}               Cmd_line_flag;

/*
 * WARNING: The enumeration Flag_name, the array cmd_line_flags, and the
 * procedure xv_usage must all be updated together whenever a new option is
 * added, and old option is deleted, or the order of the existing options is
 * permuted.
 */
typedef enum {
    FLAG_SCALE,
    FLAG_FONT,
    FLAG_WIDTH,
    FLAG_HEIGHT,
    FLAG_SIZE,
    FLAG_POSITION,
    FLAG_ICON_POSITION,
    FLAG_LABEL,
    FLAG_ICONIC,
    FLAG_FOREGROUND_COLOR,
    FLAG_BACKGROUND_COLOR,
    FLAG_SET_DEFAULT_COLOR,
    FLAG_ICON_IMAGE,
    FLAG_ICON_LABEL,
    FLAG_ICON_FONT,
    FLAG_DEFAULTS_ENTRY,
    FLAG_HELP,
    FLAG_SAVER,
    FLAG_SYNC,
    FLAG_SERVER,
    FLAG_DISABLE_RETAINED,
    FLAG_DISABLE_XIO_ERROR_HANDLER,
#ifdef _XV_DEBUG
    FLAG_LIST_FONTS,
    FLAG_DEBUG,
    FLAG_STATS,
    FLAG_GPROF_START,
#endif
}               Flag_name;

static Cmd_line_flag *find_cmd_flag();

static Cmd_line_flag cmd_line_flags[] = {
    "-Wx", "-scale", "window.scale", 0, 1,
    "-Wt", "-font", "font.name", 0, 1,
    "-Ww", "-width", "window.columns", 0, 1,
    "-Wh", "-height", "window.rows", 0, 1,
    "-Ws", "-size", "window.width",
    "window.height", 2,
    "-Wp", "-position", "window.x", "window.y", 2,
    "-WP", "-icon_position", "icon.x",
    "icon.y", 2,
    "-Wl", "-label", "window.header", 0, 1,
    "-Wi", "-iconic", "window.iconic", 0, 0,
    "-Wf", "-foreground_color", "window.color.foreground", 0, 3,
    "-Wb", "-background_color", "window.color.background", 0, 3,
    "-Wg", "-set_default_color",
    "window.inheritcolor", 0, 0,
    "-WI", "-icon_image", "icon.pixmap", 0, 1,
    "-WL", "-icon_label", "icon.footer", 0, 1,
    "-WT", "-icon_font", "icon.font.name", 0, 1,
    "-Wd", "-default", 0, 0, 2,
    "-WH", "-help", 0, 0, 0,
    "", "-disable_screen_saver", 0, 0, 1,
    "", "-sync", 0, 0, 1,
    "-Wr", "-display", "server.name", 0, 1,
    "-Wdr", "-disable_retained", "window.mono.disableRetained", 0, 0,
    "-Wdxio", "-disable_xio_error_handler", 0, 0, 0,
#ifdef _XV_DEBUG
    "", "-list_fonts", 0, 0, 0,
    "", "-Xv_debug", 0, 0, 1,
    "", "-stats", 0, 0, 0,
    "", "-gprof_start", 0, 0, 0,
#endif
    0, 0, 0, 0, 0
};

Xv_private void
xv_cmdline_scrunch(argc_ptr, argv, remove, n)
    int            *argc_ptr;
    char          **argv, **remove;
    int             n;
/*
 * Takes remove to remove+n-1 out of argv, which is assumed to be NULL
 * terminated, although no use is made of that assumption. The original argv
 * is required from the caller to avoid having to scan the list looking for
 * its end.
 */
{
#ifdef _XV_DEBUG
    if (*argc_ptr < n) {
	(void) fprintf(stderr,
		       "xv_cmdline_scrunch: argc (%d) < count (%d)\n",
		       *argc_ptr, n);
	return;
    }
    if (argv[*argc_ptr]) {
	(void) fprintf(stderr,
		       "xv_cmdline_scrunch: argv[argc(%d)] (%d:%s) not NULL\n",
		       *argc_ptr, argv[*argc_ptr], argv[*argc_ptr]);
	return;
    }
#endif
    *argc_ptr = *argc_ptr - n;
    bcopy((char *) (remove + n), (char *) (remove),
	  sizeof(*remove) * (*argc_ptr - (remove - argv) + 1));
}

Xv_public int
xv_parse_cmdline(app_name, argc_ptr, argv_base, scrunch)
    char           *app_name;
    int            *argc_ptr;
    char          **argv_base;
    int             scrunch;
/*
 * Parse the command line, looking for sv flags.  Abort if a partial flag is
 * encountered, but just ignore unrecognized flags. If scrunch, remove
 * recognized flags (and their arguments) from the command line (argv) and
 * adjust the command count (argc_ptr).
 */
{
    register char **argv = argv_base;
    register int    argc = *argc_ptr;
    int             n;

    while (argc > 0) {
	switch ((n = xv_parse_one(app_name, argc, argv))) {
	  case 0:		/* Unrecognized flag: ignore it */
	    argc--;
	    argv++;
	    break;
	  case -1:
#ifdef _XV_DEBUG
	    /* Always print debugging flags, when #define'd. */
	    xv_generic_debug_help(stderr);
#endif
	    return (-1);
	  default:
	    if (scrunch) {
		xv_cmdline_scrunch(argc_ptr, argv_base, argv, n);
	    } else
		argv += n;
	    argc -= n;
	}
    }
    return (0);
}

int
xv_parse_one(app_name, argc, argv)
    char           *app_name;
    register int    argc;
    register char **argv;
{
    int             plus;
    int             bad_arg = 0;
    register Cmd_line_flag *slot;
    Flag_name       flag_name;

    if (argc < 1 || **argv != '-')
	return (0);

    slot = find_cmd_flag(argv[0]);

    if (!slot)
	return 0;

    if (argc <= slot->num_args) {
	char            dummy[128];

	(void) sprintf(dummy, "%s: missing argument after %s", app_name,
		       argv[0]);
	xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, dummy,
		 0);

	return (-1);
    }
    flag_name = (Flag_name) (slot - cmd_line_flags);
    switch (flag_name) {
      case FLAG_WIDTH:
      case FLAG_HEIGHT:
	if ((plus = atoi(argv[1])) < 0) {
	    bad_arg = 1;
	    goto NegArg;
	}
	defaults_set_integer(slot->def_name[0], plus);
	(void) setenv("XV_CLIENT_PANE_SIZE", "TRUE");
	break;

      case FLAG_SIZE:
	if ((plus = atoi(argv[1])) < 0) {
	    bad_arg = 1;
	    goto NegArg;
	}
	defaults_set_integer(slot->def_name[0], plus);
	if ((plus = atoi(argv[2])) < 0) {
	    bad_arg = 2;
	    goto NegArg;
	}
	defaults_set_integer(slot->def_name[1], plus);
	(void) setenv("XV_CLIENT_PANE_SIZE", "TRUE");
	break;

      case FLAG_POSITION:
      case FLAG_ICON_POSITION:
	defaults_set_integer(slot->def_name[0], atoi(argv[1]));
	defaults_set_integer(slot->def_name[1], atoi(argv[2]));
	if (flag_name == FLAG_POSITION)
	    (void) setenv("XV_CLIENT_PANE_POSITION", "TRUE");
	else {
	    (void) setenv("XV_CLIENT_ICON_X", argv[1]);
	    (void) setenv("XV_CLIENT_ICON_Y", argv[2]);
	}
	break;

      case FLAG_LABEL:
	(void) setenv("XV_CLIENT_LABEL", argv[1]);
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_ICON_LABEL:
	(void) setenv("XV_CLIENT_ICON_LABEL", argv[1]);
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_ICON_IMAGE:
	(void) setenv("XV_CLIENT_ICON_IMAGE", argv[1]);
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_ICON_FONT:
	(void) setenv("XV_CLIENT_ICON_FONT", argv[1]);
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_FONT:
	defaults_set_string(slot->def_name[0], argv[1]);
	(void) setenv("XV_CLIENT_PANE_FONT", argv[1]);
	break;

      case FLAG_SCALE:
	defaults_set_string(slot->def_name[0], argv[1]);
	(void) setenv("XV_SCALE", argv[1]);
	break;

      case FLAG_ICONIC:
	(void) setenv("XV_CLIENT_ICONIC", "TRUE");
	defaults_set_integer(slot->def_name[0], TRUE);
	break;

      case FLAG_SET_DEFAULT_COLOR:
	(void) setenv("XV_CLIENT_SET_DEFAULT_COLOR", "TRUE");
	/* boolean value -- if specified then TRUE */
	defaults_set_integer(slot->def_name[0], TRUE);
	break;

      case FLAG_DISABLE_RETAINED:
	/* boolean value -- if specified then TRUE */
	(void) setenv("XV_CLIENT_DISABLE_RETAIN", "TRUE");
	defaults_set_integer(slot->def_name[0], TRUE);
	break;

      case FLAG_DISABLE_XIO_ERROR_HANDLER:
	/* boolean value -- if specified then  */
	(void) XSetIOErrorHandler((int (*) ()) NULL);
	break;

      case FLAG_FOREGROUND_COLOR:
      case FLAG_BACKGROUND_COLOR:{
	    int             i, rgb[3];
	    char            chars[100];

	    /* convert to rgb ints then to a single string */
	    for (i = 0; i <= 2; i++) {
		/* if bad number or neg. then use 0 */
		if ((sscanf(argv[i + 1], "%d", &(rgb[i])) != 1) ||
		    (rgb[i] < 0))
		    rgb[i] = 0;
	    }
	    (void) sprintf(chars, "%d %d %d", rgb[0], rgb[1], rgb[2]);
	    if (flag_name == FLAG_FOREGROUND_COLOR)
	        (void) setenv("XV_CLIENT_FOREGROUND_COLOR", chars);
	    else
	        (void) setenv("XV_CLIENT_BACKGROUND_COLOR", chars);
	    defaults_set_string(slot->def_name[0], chars);
	    break;
	}

      case FLAG_DEFAULTS_ENTRY:
	defaults_set_string(argv[1], argv[2]);
	break;

      case FLAG_HELP:
	return (-1);

      case FLAG_SAVER:
	disable_screen_saver = atoi(argv[1]);
	break;

      case FLAG_SYNC:
	lockstep_with_server = atoi(argv[1]);
	break;

      case FLAG_SERVER:
	(void) setenv("XV_CLIENT_DISPLAY", argv[1]);
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

#ifdef _XV_DEBUG
      case FLAG_LIST_FONTS:
	list_fonts = TRUE;
	break;

      case FLAG_DEBUG:
	plus = atoi(argv[1]);
SetDebug:
	if (xv_set_debug_flag(plus, TRUE)) {
	    char            dummy[128];

	    (void) sprintf(dummy,
			   "xv_set_debug_flag; '%d' is out of bounds",
			   plus);
	    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING, dummy,
		     0);
	}
	break;
      case FLAG_STATS:
	server_gather_stats = TRUE;
	break;
      case FLAG_GPROF_START:
	sview_gprof_start = TRUE;
	break;
#endif

      default:
	return (0);

    }

    return (slot->num_args + 1);

/* BadFont:
    {
	char            dummy[128];

	(void) sprintf(dummy, "%s: bad font file (%s)", app_name, argv[1]);
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, dummy,
		 0);
	return (-1);
    } */

NegArg:
    {
	char            dummy[128];

	(void) sprintf(dummy, "%s: can't have negative argument %s after %s",
		       app_name, argv[bad_arg], argv[0]);
	xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, dummy,
		 0);
	return (-1);
    }

/* NoMsgError:
    return (-1); */
} 

static Cmd_line_flag *
find_cmd_flag(key)
    register char  *key;
{
    register Cmd_line_flag *slot = cmd_line_flags;

    for (slot = cmd_line_flags; slot->short_name; slot++)
	if ((strcmp(key, slot->short_name) == 0) ||
	    (strcmp(key, slot->long_name) == 0))
	    return slot;
    return 0;
}

#ifdef _XV_DEBUG
static int
xv_generic_debug_help(fd)
    int             fd;
{
    (void) fprintf(fd, "Generic debugging flags are:\n\
	(-disable_screen_saver)	bool\n\
	(-sync)			bool	(run synchronous with server)\n\
	(-list_fonts)\n\
	(-stats)\n\
	(-Xv_debug)		unsigned\n");
}

#endif

void
xv_usage(name)
    char           *name;
{
    (void) fprintf(stderr, "usage of %s generic window arguments:\n\
FLAG\t(LONG FLAG)\t\tARGS\t\tNOTES\n\
-Ww\t(-width)\t\tcolumns\n\
-Wh\t(-height)\t\tlines\n\
-Ws\t(-size)\t\t\tx y\n\
-Wp\t(-position)\t\tx y\n\
-WP\t(-icon_position)\tx y\n\
-Wl\t(-label)\t\t\"string\"\n\
-Wi\t(-iconic)\t\n\
-Wt\t(-font)\t\t\tfilename\n\
-Wx\t(-scale)\t\tsmall|medium|large|extra_large\n\
-Wf\t(-foreground_color)\tred green blue\t0-255 (no color-full color)\n\
-Wb\t(-background_color)\tred green blue\t0-255 (no color-full color)\n\
-Wg\t(-set_default_color)\t\t\t(apply color to subwindows too)\n\
-WI\t(-icon_image)\t\tfilename\n\
-WL\t(-icon_label)\t\t\"string\"\n\
-WT\t(-icon_font)\t\tfilename\n\
-Wr\t(-display)\t\t\"server_name:screen\"\n\
-Wdr\t(-disable_retained)\t\n\
-Wdxio\t(-disable_xio_error_handler)\t\n\
\t(-disable_screen_saver)\t0|1\n\
\t(-sync)\t\t\t0|1\n\
-WH\t(-help)\t\n",
		   name ? name : "");

    exit(97);
}

