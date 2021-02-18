/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carlo Lisa
 *	   MIT Project Athena
 *	   carlo@athena.mit.edu
 *
 *
 *      This is the file xless.h for the Xless, a file browsing utility
 *      built upon Xlib and the XToolkit.
 *      It Contains: Lots of good defintions.
 *
 *	$Header: /mit/tax/Xless/RCS/xless.h,v 1.4 89/10/11 20:34:28 carlo Exp $
 *
 */

/* Std system and C include files */

#include <stdio.h>
#include <X11/Xos.h>
#include <sys/dir.h>
#include <sys/stat.h>

/* X include files */

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>

/* X toolkit Include files */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

#ifdef ATHENA
#  include <X11/AsciiText.h>
#  include <X11/Text.h>
#  include <X11/Command.h>
#  include <X11/Viewport.h>
#  include <X11/Box.h>
#  include <X11/Label.h>
#  include <X11/Paned.h>
#  include <X11/Form.h>
#else
#  include <X11/Xaw/AsciiText.h>
#  include <X11/Xaw/Text.h>
#  include <X11/Xaw/Command.h>
#  include <X11/Xaw/Viewport.h>
#  include <X11/Xaw/Box.h>
#  include <X11/Xaw/Label.h>
#  include <X11/Xaw/Paned.h>
#  include <X11/Xaw/Form.h>
#endif

/* Usefull defines */

#define MEMBUF		512000
#define TEXTWIDTH       500
#define TEXTHEIGHT      500
#define BOXWIDTH        500
#define BOXHEIGHT       50
#define BUTTONWIDTH     100
#define BUTTONHEIGHT    18
#define DIALOGWIDTH     350
#define DIALOGHEIGHT    150
#define MAX_INPUT	256
#define DEF_INPUT_W	200
#define INPUT_HEIGHT	6

/*
 * The environment variable containing the favorite editor.
 */

#define EDITORVAR "EDITOR"

/*
 * The default editor; make sure the file exist.
 * (Used if the environment variable EDITOR is not set)
 */

#define DEFEDITOR "/usr/ucb/vi"

/*
 * This is the default helpfile and is site specific,
 * make sure you change this.
 */

#ifndef HELPFILE
#define HELPFILE "/usr/lib/X11/xless.help" /* The default helpfile */
#endif

/* Default fonts. */
#define STANDARDCUR "left_ptr"

#ifdef ATHENA
#define STANDARDFONT "fixed"
#define TEXTFONT "-adobe-helvetica-medium-r-normal--12-120-*-*-p-67-iso8859-1"
#define LABELFONT "-adobe-times-bold-i-normal--24-240-*-*-p-128-iso8859-1"
#define BUTTONFONT "-adobe-times-bold-r-normal--14-140-*-*-p-77-iso8859-1"
#else
#define STANDARDFONT "8x13"
#define TEXTFONT "8x13"
#define LABELFONT "8x13bold"
#define BUTTONFONT "8x13bold"
#endif ATHENA

/* Typedefs */

typedef struct _SearchData {	/* The structure with the information for */
	Widget popup;		/* search facilities.			  */
	Widget father;
	Widget new_popup;
	Widget change_pop;
	Widget text;
	Widget lab;
	char *memory;
	char *selection;
	char *new_selection;
	char *new_sel_file;
	char *file;
} SearchData;

typedef struct _CallData {
	Widget shl;
	char *mem;
} CallData;

typedef struct _XlessFonts {
  XFontStruct *standard,	/* The font used if no fonts specied */
	*text,			/* Font used for Xless text */
	*label,			/* Font used for labels in dialog boxes */
	*button;		/* Font used for commandbuttons */
} XlessFonts;

typedef struct _XlessCursors {
  Cursor top,			/* The top cursor, default for Xless */
	dialog;			/* Cursor for dialog boxes */
} XlessCursors;

/* Resource manager sets these */

typedef struct _XlessResources {	
  XlessFonts fonts;			/* The fonts used for Xless */
  XlessCursors cursors;			/* The cursors used for Xless */
  char *help_file;			/* The help file name */
} XlessResources;

/* Externals */

extern char *getenv();
