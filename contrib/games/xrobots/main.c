/*
 * main.c  --  xrobots
 *
 * Send comments, bug reports, etc. to brianw@Sun.COM.
 *
 * 
 * Copyright 1989 Brian Warkentine
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the author's name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE 
 * AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * The author's current employer has no connection with this software, as it
 * was written before being employed at Sun.  The following address is for 
 * contacting the author only and does not imply any responsibility on the 
 * behalf of Sun Microsystems.
 * 
 * Contact the author at:
 * 	Brian Warkentine  (brianw@Sun.COM)
 * 	Sun Microsystems
 * 	2550 Garcia Avenue
 * 	Mountain View, Ca 94043-1100
 *
 */

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Core.h>
#include <X11/Shell.h>

#ifdef R3
#include <X11/Label.h>
#include <X11/Command.h>
#include <X11/Box.h>
#else
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#endif


#include "xrobots.h"

/*----------------------------------------------------------------------*/

Widget top_shell, top_widget, playfield_widget, score_command, sonic_command;

Display *display;
Window playfield;

GC gc;
GC cleargc;

/*----------------------------------------------------------------------*/

static Arg arglisttop_shell[] = {
  {  XtNinput,  (XtArgVal)True },
};

static Arg arglistplayfield[] = {
  {  XtNheight, (XtArgVal) pos_to_coord(MAXY)  },
  {  XtNwidth,  (XtArgVal) pos_to_coord(MAXX)  },
  {  XtNborderWidth, (XtArgVal) 4 },
};

static Arg arglistteleport_command[] = {
  {  XtNlabel, (XtArgVal) "Teleport"  },
};

static Arg arglistwait_command[] = {
  {  XtNlabel, (XtArgVal) "Wait"  },
};

static Arg arglistsonic_command[] = {
  {  XtNlabel, (XtArgVal) "Sonic Screwdriver"  },
};

static Arg arglistquit_command[] = {
  {  XtNlabel, (XtArgVal) "Quit"  }, 
};

static Arg arglistnew_game[] = {
  {  XtNlabel, (XtArgVal) "New Game"  }, 
};

static Arg arglistscore_command[] = {
  {  XtNlabel, (XtArgVal) "Score:    0"  }, 
};

/*----------------------------------------------------------------------*/

static char translations_str[] = 
  "<Btn1Down>:	do_nothing()		\n\
   <Btn2Down>:	do_nothing()		\n\
   <Btn3Down>:	do_nothing()		\n\
   <Btn1Up>:    move()			\n\
   :<Key>u:     move(right, up)		\n\
   :<Key>l:     move(right)		\n\
   :<Key>n:     move(right, down)	\n\
   :<Key>y:     move(left, up)		\n\
   :<Key>h:     move(left)		\n\
   :<Key>b:     move(left, down)	\n\
   :<Key>k:     move(up)		\n\
   :<Key>j:     move(down)		\n\
   :<Key>U:     jump(right, up)		\n\
   :<Key>L:     jump(right)		\n\
   :<Key>N:     jump(right, down)	\n\
   :<Key>Y:     jump(left, up)		\n\
   :<Key>H:     jump(left)		\n\
   :<Key>B:     jump(left, down)	\n\
   :<Key>K:     jump(up)		\n\
   :<Key>J:     jump(down)		\n\
   <Key>.:      move(nowhere)		\n\
   <Key>\\ :    move(nowhere)		\n\
   <Btn2Up>:    go_here() 		\n\
   <Btn3Up>:    wait()	 		\n\
   <Key>s:      sonic()			\n\
   <Key>t:      teleport() 		\n\
   <Key>w:      wait()			\n\
   <Key>z:      new_game()		\n\
   Ctrl<Key>c:  quit() ";

static void quit_game();

static XtActionsRec actions[] = {
  {"wait",      (XtActionProc)wait_for_em},
  {"teleport",  (XtActionProc)teleport},
  {"sonic",     (XtActionProc)sonic_action},
  {"move",      (XtActionProc)move_action},
  {"go_here",	(XtActionProc)go_here_action},
  {"jump",	(XtActionProc)jump_action},
  {"quit",      (XtActionProc)quit_game},
  {"new_game",  (XtActionProc)new_game},
  {"do_nothing",(XtActionProc)do_nothing_action},
};

Pixel fg, bg;
Boolean spiffy,			/* limited graphics 			*/
	autoteleport,		/* teleport when sonic used, and no moves */
	autoteleportalways,	/* teleport when no available moves 	*/
	showjumps,		/* show no intermediate moves 		*/
	diewaiting;		/* die if you use wait unsafely 	*/
XtTranslations translations;

static XrmOptionDescRec options[] = {
	{"-spiffy",	"spiffy",	XrmoptionSepArg, NULL },
	{"-autoteleport","autoteleport",XrmoptionSepArg, NULL },
	{"-autoteleportalways","autoteleportalways",XrmoptionSepArg, NULL },
	{"-showmovement","showmovement",XrmoptionSepArg, NULL },
	{"-diewaiting","diewaiting",XrmoptionSepArg, NULL },
	{"-scorefile","scorefile",XrmoptionSepArg, NULL },
};

static XtResource application_resources[] = {
  {"foreground", "Foreground", XtRPixel, sizeof(Pixel),
                (Cardinal)&fg, XtRString, (caddr_t) "Black"},
  {"background", "Background", XtRPixel, sizeof(Pixel),
                (Cardinal)&bg, XtRString, (caddr_t) "White"},
  {"spiffy", "Spiffy", XtRBoolean, sizeof(Boolean),
                (Cardinal)&spiffy, XtRString, (caddr_t) "True"},
  {"autoteleport", "Autoteleport", XtRBoolean, sizeof(Boolean),
                (Cardinal)&autoteleport, XtRString, (caddr_t) "False"},
  {"autoteleportalways", "Autoteleportalways", XtRBoolean, sizeof(Boolean),
                (Cardinal)&autoteleportalways, XtRString, (caddr_t) "False"},
  {"showmovement", "Showmovement", XtRBoolean, sizeof(Boolean),
                (Cardinal)&showjumps, XtRString, (caddr_t) "True"},
  {"diewaiting", "Diewaiting", XtRBoolean, sizeof(Boolean),
                (Cardinal)&diewaiting, XtRString, (caddr_t) "False"},
  {"translations","Translations", XtRTranslationTable, sizeof(XtTranslations),
                (Cardinal)&translations, XtRString, (caddr_t)translations_str},
  {"scorefile","Scorefile", XtRString, sizeof(String),
                (Cardinal)&score_filename, XtRString, (caddr_t)SCORE_FILE},
};

/*----------------------------------------------------------------------*/

main(argc, argv)
  unsigned int argc;
  char **argv;
{
  Arg args[1];
  XGCValues gcv;
  Widget teleport_command,
	 wait_command,
	 quit_command,
	 new_game_command;

  srandom(getpid());

  top_shell = XtInitialize(argv[0], "xrobots", options, XtNumber(options), &argc, argv);
  XtSetValues(top_shell, arglisttop_shell, XtNumber(arglisttop_shell));

  XtAddActions(actions,XtNumber(actions));

  XtGetApplicationResources(top_shell, 0, application_resources, 
			XtNumber(application_resources), NULL, 0 );

  top_widget = XtCreateManagedWidget(
                                    "top_widget",
                                    boxWidgetClass,
                                    top_shell,
                                    0,0);

  playfield_widget = XtCreateManagedWidget(
                                    "playfield",
                                    widgetClass,
                                    top_widget,
                                    arglistplayfield,
                                    XtNumber(arglistplayfield));

  XtAugmentTranslations(playfield_widget,translations);

  teleport_command = XtCreateManagedWidget(
                                    "teleport_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistteleport_command,
                                    XtNumber(arglistteleport_command));

  XtAddCallback(teleport_command, XtNcallback, (XtCallbackProc)teleport, 0);

  wait_command =  XtCreateManagedWidget(
                                    "wait_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistwait_command,
                                    XtNumber(arglistwait_command));

  XtAddCallback(wait_command, XtNcallback, (XtCallbackProc)wait_for_em, 0);

  sonic_command= XtCreateManagedWidget(
                                    "sonic_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistsonic_command,
                                    XtNumber(arglistsonic_command));

  XtAddCallback(sonic_command, XtNcallback, (XtCallbackProc)sonic_action, 0);

  quit_command = XtCreateManagedWidget(
                                    "quit_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistquit_command,
                                    XtNumber(arglistquit_command));

  XtAddCallback(quit_command,     XtNcallback, (XtCallbackProc)quit_game, 0);

  new_game_command = XtCreateManagedWidget(
                                    "new_game_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistnew_game,
                                    XtNumber(arglistnew_game));

  XtAddCallback(new_game_command, XtNcallback, (XtCallbackProc)new_game, 0);

  score_command = XtCreateManagedWidget(
                                    "score_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistscore_command,
                                    XtNumber(arglistscore_command));

  XtAddCallback(score_command, XtNcallback, (XtCallbackProc)show_scores_callback, 0);


  create_high_score_popup(top_widget);

  XtRealizeWidget(top_shell);


  display   = XtDisplay(playfield_widget);
  playfield = XtWindow(playfield_widget);
  gcv.foreground = fg;
  gcv.background = bg;
  gcv.function = GXcopy;
  gc = XCreateGC(display, playfield, 
 		GCForeground | GCBackground | GCFunction, &gcv);
  gcv.foreground = bg;
  cleargc = XCreateGC(display, playfield,
 		 GCForeground | GCBackground | GCFunction, &gcv);

  XtAddEventHandler(playfield_widget, ExposureMask, 0, redisplay_level, 0);
  XtAddEventHandler(playfield_widget, PointerMotionMask, 0, pointer_moved, 0);

  init_pixmaps(top_shell);

  new_game();

  XtMainLoop();

}


static void
quit_game()
{
  free_pixmaps();
  XtDestroyWidget(top_shell);
  XFreeGC(display,gc);
  XFreeGC(display,cleargc);

  exit(0);
}


void
update_score(score)
  int score;
{
  char text[13];
  (void)sprintf(text,"Score: %4d",score);
  XtSetArg(arglistscore_command[0],XtNlabel,text);
  XtSetValues(score_command,arglistscore_command,1);
}



