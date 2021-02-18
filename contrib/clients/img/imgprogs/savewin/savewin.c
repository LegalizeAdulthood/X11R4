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
==============================================================================
======									======
======			     Program  savewin				======
======				Save Window				======
======									======
==============================================================================

  Function:

	Save a window to an image file.  If the window is the root
	window, this saves the screen.  See the manual entry for details.


  Subprograms:

	main		Main procedure


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.

	3-29-89		Paul Raveling
	Created this program by modifying primg.  Hopefully this
	will shed some light on why primg skews one particular
	window.

*/



#include <stdio.h>
#include <pwd.h>

#include <imglib/imglib.h>	/*  Include sys-independent defs	*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>		/*  Include X11 definitions		*/
#include <X11/Xos.h>
#include <X11/cursorfont.h>

#include <imglib/imgXlib.h>	/*  Include sys-dependent defs for X11	*/

extern	char	*getenv ();


/*1
===============================================================================
=======                                                                 =======
=======		  Main  --  Main program to process pixmap		=======
=======                                                                 =======
===============================================================================

  Invocation:

	main ( argc, argv )


  Functions:

	Save window:  See manual page(s).


  Input:

	argc		int
			Command line argument count

	argv		char	*[]
			Array of pointers to argument strings
			Refer to program header for interpretation
			of arguments.

  Output:

	status		int
	<return value>	Exit code:
				0 => Window saved successfully
				1 => Questionable syntax in command operand
				2 => Couldn't write window image

*/



int	 main (argc, argv)

	int	 argc;			/*  Command argument count	*/
	char	*argv[];		/*  Pointers to arguments	*/

{
	int	 status;		/*  Status to return		*/

	Img	*image;			/*  Image of window		*/
	X11env	*X11_env;		/*  X11 interface environment	*/
	Window	 win;			/*  Window to print		*/




/*----------------------------------------------------------*/
/*	Save contents of a window or of entire screen	    */
/*----------------------------------------------------------*/

if ( argc < 2 )
   {
   fprintf ( stderr, "savewin:  No output filename given;\n" );
   fprintf (stderr, "          Invoke as \"savewin output_image_filename\"\n");
   exit (2);
   }

X11_env = start_X11 ( argc, argv );	/*  Start X11 interface		*/
if ( X11_env == NULL )
   {
   fprintf ( stderr, "%s:  Couldn't start X11 interface\n",
		argv[0] );
   exit(2);
   }

   {					/*     let user pick a window	*/
   int		 xstatus;
   Cursor	 cursor;		/*  Cursor for temp use		*/
   XEvent	 event;			/*  Event for marking window	*/
   XButtonEvent	*butev;			/*  Ptr for convenience		*/


   cursor = XCreateFontCursor ( X11_env->display, XC_circle );
   if (cursor == NULL)
      {
      fprintf ( stderr, "%s:  Can't create a cursor\n", argv[0] );
      stop_X11 ( X11_env );
      exit(2);
      }

   xstatus = XGrabPointer ( X11_env->display, X11_env->rootwin,
		True, ButtonPressMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, (Window) NULL,
		cursor, CurrentTime );
   switch ( xstatus )
	{
	case GrabNotViewable:
		fprintf (stderr,
			"%s:  Can't grab pointer (root window isn't viewable)\n", argv[0] );
		stop_X11 ( X11_env );
		exit(2);

	case AlreadyGrabbed:
		fprintf (stderr,
			"%s:  Can't grab pointer (already grabbed)\n", argv[0] );
		stop_X11 ( X11_env );
		exit(2);

	case GrabFrozen:
		fprintf (stderr,
			"%s:  Can't grab pointer (frozen by active grab)\n", argv[0] );
		stop_X11 ( X11_env );
		exit (2);

	case GrabInvalidTime:
		fprintf (stderr,
			"%s:  Can't grab pointer (current time is invalid)\n", argv[0] );
		stop_X11 ( X11_env );
		exit (2);

        default:			/*  Anything else:	*/
		break;			/*	Explicitly OK	*/
	}

   fprintf (stderr, "\n");
   fprintf (stderr, "\tPlease select the window to save\n");
   fprintf (stderr, "\tby clicking the mouse in that window.\n");
   fprintf (stderr, "\n");
   fprintf (stderr, "\tSelecting the screen background\n");
   fprintf (stderr, "\tsaves the entire screen.\n" );
   fprintf (stderr, "\n");

   XNextEvent (X11_env->display, &event);/*  Get mouse buttoning event	*/
   butev = (XButtonEvent *) &event;

   if ( butev -> subwindow != 0 )
      win = butev -> subwindow;		/*  Extract window		*/
   else
      win = X11_env -> rootwin;

   XUngrabPointer ( X11_env->display, CurrentTime );
   }



image = get_img ( win, NULL, X11_env );	/*  Get window contents		*/
if ( image == NULL )
   {
   fprintf ( stderr, "%s:  Couldn't get image from X11\n", argv[0] );
   stop_X11 ( X11_env );
   exit (2);
   }

write_img ( argv[1], image );

stop_X11 ( X11_env );			/*  Shut down X interface	*/
exit (0);				/*  Issue exit status		*/
}
