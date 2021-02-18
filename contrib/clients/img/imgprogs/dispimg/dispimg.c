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
======================================================================
======								======
======		Program dispimg:	Display Image		======
======			      File dispimg.c			======
======								======
======================================================================

	See manual entry for info.


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes are deletion of [M]onochrome switch parsing, now
	handled by imglib in a more sanitary way, and finally
	adding this module header.

	Early '89	Paul Raveling
	Created module
*/




#include <stdio.h>

#include <imglib/imglib.h>	/*  Include sys-independent defs	*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>		/*  Include X11 definitions		*/
#include <X11/Xos.h>

#include <imglib/imgXlib.h>	/*  Include sys-dependent defs for X11	*/


main(argc,argv)

	int	 argc;
	char	*argv[];

{
	Img	*im;
	int	 status;
	X11env	*X11_env;
	XEvent	*event;




/*:::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort with a message	   */
/*:::::::::::::::::::::::::::::::::::::::::*/

#define	abort(msg) {fprintf(stderr,"dispimg:  %s\n",msg); exit(2);}



/*------------------------------------------------------*/
/*	Read image from file named in 1st argument	*/
/*------------------------------------------------------*/

if ( argc < 2 )				/*  Be sure filename exists	*/
   abort("No image filename on command line")

im = read_img ( argv[1], any_image );	/*  Read the image	*/
if ( im == NULL )
   abort("Couldn't read image")



/*------------------------------------------------------*/
/*	Start X interface, Display image in window	*/
/*------------------------------------------------------*/


X11_env = start_X11 ( argc, argv );	/*  Start X11 interface		*/
if ( X11_env == NULL )
   abort("Couldn't start X interface")

display_img ( im, X11_env, ButtonPressMask | KeyPressMask );

while (1)
   {
   char	 	 keybuf[20];	/* &*@$#*&@$ keyboard paraphernalia	*/
   int		 n;
   KeySym	 keysym;
   XComposeStatus compstat;


   event = maint_img ( im, X11_env );		/*  Maintain image	*/

   switch ( event->type )
	{
	case ButtonPress:

		/*  For now, exit on any mouse button	*/

		exit(0);

	case KeyPress:

		/*  Exit on control-C, ignore anything else	*/

	        n = XLookupString ( event, keybuf, 20, &keysym, &compstat );
		if ( (n > 0) && (*keybuf == 0x03) )
		   exit (0);
		break;

	default:
		fprintf ( stderr, "Unexpected event type, %d\n",
				event->type );
		break;
	}
   }

}
