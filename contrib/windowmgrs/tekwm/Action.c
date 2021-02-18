#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Action.c,v 1.8 89/11/22 15:02:22 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#include <signal.h>

extern int execute();	/* Menu.c */
char *malloc();

/*
 * MODIFICATION HISTORY
 *
 * 000 -- Glenn Widener, Tektronix, inc.
 * extracted DoAction routine from awm's Menu.c for easier maintenance.
 */

#ifndef lint
static char *sccsid = "@(#)Menu.c	3.8	1/24/86";
#endif

#include "awm.h"

Boolean DoAction(window, mask, button, x, y, menu, action)
Window window;				/* Event window. */
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
MenuInfo *menu;
ActionLine *action;
{
     Boolean *tmp;
     char *buff;

     Entry("DoAction")

     switch(action->type) {
     case IsText:
	  XStoreBytes(dpy, action->text, strlen(action->text));
	  break;

     case IsTextNL:
	  buff = malloc(strlen(action->text) + 2);
	  strcpy(buff, action->text);
	  strcat(buff, "\n");
	  XStoreBytes(dpy, buff, strlen(buff));
	  free(buff);
	  break;

     case IsShellCommand:
          /* subshell got signal; should assume it is intended for tekwm and
	     exit??? */
          if(execute(action->text) == -1) {
	     /* Cleanup();
		exit(0); */
	  }
	  break;

     default:
	  (void) sprintf(error_msg, "Unknown action type %d (%s) invoked", action->type, action->text);
	  Warning(error_msg);
	  break;
     }
}
Boolean DoWindowCommand(window, mask, button, x, y, menu, action)
Window window;				/* Event window. */
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
MenuInfo *menu;
ActionLine *action;
{
     Boolean *tmp;
     char *buff;

     Entry("DoWindowCommand")

     switch(action->type) {

     case IsWindowCommand:
	  buff = malloc(strlen(action->text) + 30);
	  /* allow extra room for exactly one sprintf conversion spec */
	  (void) sprintf(buff, action->text, window);
          /* subshell got signal; should assume it is intended for tekwm and
	     exit??? */
          if(execute(buff) == -1) {
	     /* Cleanup();
		exit(0); */
	  }
	  break;

     default:
	  (void) sprintf(error_msg, "Unknown action type %d (%s) invoked", action->type, action->text);
	  Warning(error_msg);
	  break;
     }
}

