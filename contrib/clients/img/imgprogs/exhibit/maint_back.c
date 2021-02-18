/*

    Copyright (c) 1989 University of Southern California

	Paul Raveling
	USC Information Sciences Institute
	4676 Admiralty Way
	Marina del Rey, California 90292


	The authors grant without fee permission to use, copy, modify,
	distribute, and sell this software for any purpose, provided:

	--  The above copyright notice appears in all copies.

	--  This disclaimer appears in all source code copies.

	--  The names of ISI and USC are not used in advertising or
	    publicity  pertaining  to distribution  of the software
	    without prior specific written permission from ISI.

	ISI provides this software freely as a public service.  It is
	NOT a commercial product,  and therefore is not subject to an
	an implied  warranty  of  merchantability  or  fitness  for a
	particular purpose.  ISI provides it as is, without warranty.

	This software  is furnished  only on the basis that any party
	who receives it indemnifies  and holds harmless  the original
	authors and their sponsors  against any claims,  demands,  or
	liabilities  connected  with  using  it,   furnishing  it  to
	others, or providing it to a third party.


	The author  of this software is  Paul Raveling.   Development
	used the author's after-work hours and ISI's facilities.

	Thanks are due to ISI for allowing  such extracurricular work
	and  to Hewlett Packard,   whose  University  Grants  program
	provided  the 9000/370  workstation  and  supporting software
	which was the platform for this work.
*/

/*0
===============================================================================
=======                                                                 =======
=======		   Module maint_back  --  Maintain Background		=======
=======			     File maint_back.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	Maint_back maintains a black background while exhibit
	shows one image at a time over it.  If the keyboard is
	locked, or if this is screen 0, it grabs keyboard an
	mouse input.


  Public Routines:

	maint_back	Maintain background

  Local Routines:

	None


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.

	Summer '89	Paul Raveling
	Created module as a hack.
*/



#include <stdio.h>
#include <signal.h>
#include <pwd.h>

#include <imglib/imglib.h>	/*  Include sys-independent defs	*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>		/*  Include X11 definitions		*/
#include <X11/Xos.h>

#include <imglib/imgXlib.h>	/*  Include sys-dependent defs for X11	*/


#ifndef	hpux
#define	volatile	""
#endif


extern	char	*crypt();



/*	Background usage code values:	*/

#define	   UNLOCKED	1		/*	Black, unlocked		*/
#define	   LOCKED	2		/*	Black, locked		*/


extern	Window	ourparent;	/*  Either root or background window	*/


maint_back (back, ctl)

	int	 back;		/*  Background usage		*/
	int	 ctl;		/*  Control process' pid	*/

{
	X11env	*X11_env;
volatile XEvent	 event;
	Window	 bgwin;		/*  Background window		*/
	Pixmap	 nullpix;	/*  Empty bitmap		*/
	Cursor	 nullcur;	/*  "Invisible" cursor		*/
	int	 backproc;	/*  Background process' pid	*/
volatile XSizeHints bull;	/*  X malarky			*/
	XColor	 black;

volatile int	 ss_timeout;	/*  Saved screen saver parameters	*/
volatile int	 ss_interval;
volatile int	 ss_prefer_blanking;
volatile int	 ss_allow_exposures;

	int	 kbdgrabbed;	/*  Keyboard grab status	*/
	int	 ptrgrabbed;	/*  Pointer  grab status	*/

	char	 userpw[20];	/*  Encrypted user password	*/
	char	 rootpw[20];	/*  Encrypted root password	*/
	char	 kbinp[20];	/*  Keyboard input		*/
  struct passwd	*pw;		/*  Password info		*/
	char	*kbip;		/*  Keyboard input pointer	*/
	int	 kbbc;		/*  Keyboard byte count		*/

static	char	*argv[] = {"exhibit"};	/*  Dummy argument vector	*/



/*--------------------------------------------------------------*/
/*	First, cover the screen with a solid black window	*/
/*--------------------------------------------------------------*/

X11_env = start_X11 ( 1, argv );	/*  Start X11 interface		*/
if ( X11_env == NULL )
   {
   fprintf ( stderr, "Couldn't start X11 interface for background window\n" );
   return;
   }

bgwin = XCreateSimpleWindow ( X11_env->display, X11_env->rootwin,
	0, 0, X11_env->width, X11_env->height, 0,
	BlackPixel(X11_env->display,X11_env->screen),
	BlackPixel(X11_env->display,X11_env->screen) );

if ( bgwin == NULL )
   {
   fprintf ( stderr, "Creation of background window failed\n" );
   return;
   }
ourparent = bgwin;			/*  Make this the parent of	*/
					/*	all images		*/


/*	Fake out the window manager's policy.  (Who said policy-free?)	*/

bull.flags = USPosition | USSize;	/*  Tell it the user wants	*/
bull.x      = 0;			/*  this geometry		*/
bull.y      = 0;
bull.width  = X11_env -> width;
bull.height = X11_env -> height;
XSetNormalHints ( X11_env->display, bgwin, &bull );



/*----------------------------------------------------------------------*/
/*	Create separate process to watch input on background window	*/
/*----------------------------------------------------------------------*/

backproc = fork ();			/*  Create process for		*/
if ( backproc != 0 )			/*  background window		*/
   return;

/*	Remaining code is the background window maintenance process	*/



XMapWindow ( X11_env->display, bgwin );	/*  Map the window	*/

XSelectInput ( X11_env->display, bgwin,
		ButtonPressMask | KeyPressMask /* | ExposureMask */ );
	/*
	"Handle" exposures by letting display server
	fill the background color in exposed regions.
	*/

/*	Save screen saver values, then disable it	*/

XGetScreenSaver ( X11_env->display, &ss_timeout, &ss_interval,
			&ss_prefer_blanking, &ss_allow_exposures );

XSetScreenSaver ( X11_env->display, 0, 0, PreferBlanking, AllowExposures );


pw = getpwuid(getuid());		/*  Save encrypted user password  */
strcpy ( userpw, pw->pw_passwd );

pw = getpwuid(0);			/*  Save encrypted root password  */
strcpy ( rootpw, pw->pw_passwd );


black.pixel = BlackPixel(X11_env->display,X11_env->screen);
black.red   = 0;
black.green = 0;
black.blue  = 0;
black.flags = DoRed | DoGreen | DoBlue;

nullpix = XCreateBitmapFromData ( X11_env->display, bgwin, "\0", 1, 1 );
nullcur = XCreatePixmapCursor
		( X11_env->display, nullpix, nullpix, &black, &black, 0, 0 );

if ((back == LOCKED) || (X11_env->display == 0))
   {
   kbdgrabbed = XGrabKeyboard ( X11_env->display, bgwin, False,
		GrabModeAsync, GrabModeAsync, CurrentTime);
   ptrgrabbed = XGrabPointer  ( X11_env->display, bgwin, False,
		ButtonPressMask, GrabModeAsync, GrabModeAsync,
		None, nullcur, CurrentTime );
#if 0	/*  Currently have old version of libXhp11.a  */
#ifdef	hpux
   XHPDisableReset( X11_env -> display );  /*  Disable ctl-shift-reset	*/
#endif
#endif
   }

/*----------------------*/
/*	Get input	*/
/*----------------------*/

kbip = kbinp;			/*  Start with empty kbd input buffer	*/
kbbc = 0;

while (1)
      {
volatile char		 keybuf[20];	/* &*@$#*&@$ keyboard paraphernalia */
volatile KeySym		 keysym;
volatile XComposeStatus	 compstat;
      int	 n;
      char	*cp;


      XNextEvent ( X11_env->display, &event );

      if ((event.type == ButtonPress) && (back == UNLOCKED))
	 break;

      if ( event.type == KeyPress )
	 {
	 n = XLookupString ( &event, keybuf, 20, &keysym, &compstat );
	 if ( n > 0 )
	    {
	    if ( back == UNLOCKED )		/*  If unlocked,	*/
	       {				/*	just look for	*/
	       if ( *keybuf == 0x03 )		/*	control-C	*/
		  break;
	       else
		  continue;
	       }

	    /*	back == LOCKED	*/

	    if	((*keybuf == '\n') || (*keybuf == '\r'))
		{
		*kbip = 0;
		if (strcmp ( crypt(kbinp, userpw), userpw ) == 0)
		   break;
		if (strcmp ( crypt(kbinp, rootpw), rootpw ) == 0)
		   break;

		kbip = kbinp;
		kbbc = 0;
		continue;
		}

	    if	((*keybuf == 0x08) || (*keybuf == 0x7f))
		{
		if ( kbbc > 0 )
		   {
		   -- kbip;
		   -- kbbc;
		   }
		continue;
		}

	    cp = keybuf;
	    while ((kbbc < 19) && (n > 0))
		  {
		  *kbip++ = *cp++;
		  ++ kbbc;
		  -- n;
		  }
	    }
	 }
      }



/*--------------------------------------------------------------*/
/*	Restore screensaver and terminate the whole shebang	*/
/*--------------------------------------------------------------*/

XSetScreenSaver ( X11_env->display, &ss_timeout, &ss_interval,
			&ss_prefer_blanking, &ss_allow_exposures );

if ( kbdgrabbed == GrabSuccess )
   XUngrabKeyboard ( X11_env->display, CurrentTime );
if ( ptrgrabbed == GrabSuccess )
   XUngrabPointer ( X11_env->display, CurrentTime );

kill ( ctl, SIGHUP );			/*  Delete control process	*/
exit (0);
}
