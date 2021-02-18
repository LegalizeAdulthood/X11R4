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
======			       Program  Primg				======
======				Print  Image				======
======									======
==============================================================================

  Function:

	Print an image file, a window from the screen, or the
	entire screen image.  See the manual entry for details.

	This is a successor to the X10.4 printpix utility.
	That program's logic for translating from color to
	gray scale and from gray scale to dithered/enlarged
	monochrome is now in imglib.

	Aliases for this program force particular options as follows:

		prscr	--  Print screen image, default orientation
		prland	--  Print window or screen in landscape orientation
		prport	--  Print window or screen in portrait orientation


  Subprograms:

	main		Main procedure


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.

	3-27-89		Paul Raveling
	Created this program from the front end side of
	the X10.4 printpix utility.  Back end side is handled
	by imglib routines.

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



/*======================================================*/
/*	Convert RGB image to mapped grayscale image	*/
/*	... Quick addition, sanitary version later	*/
/*======================================================*/

convert_to_gray_mapped (image)

	Img	*image;

{
register Byte	*bp;
register Rgbpix	*pp;
register int	 i;

color_to_gray(image);	/*  Pseudo-quantize to grayscale  */

/*  A touch of filth:  Convert image "in place" to mapped	*/

image -> imgtype = mapped_image;
bp = (Byte *)   image -> pixdata;
pp = (Rgbpix *) image -> pixdata;
i = image -> num_pixels;

while ( i-- > 0 )
      *bp++ = (*pp++ >> pixel_tag_shift) & 0xff;
}


/*1
===============================================================================
=======                                                                 =======
=======		  Main  --  Main program to process pixmap		=======
=======                                                                 =======
===============================================================================

  Invocation:

	main ( argc, argv )


  Functions:

	Print image:  See manual page(s).


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
				0 => Image printed successfully
				1 => Questionable syntax in command operand
				2 => Couldn't print at least one image

*/



int	 main (argc, argv)

	int	 argc;			/*  Command argument count	*/
	char	*argv[];		/*  Pointers to arguments	*/

{
	int	 status;		/*  Status to return		*/

	int	 orient;		/*  Orientation of printed image:  */
					/*	PIO_DEFAULT,		*/
					/*	PIO_PORTRAIT, or 	*/
					/*	PIO_LANDSCAPE		*/

	int	 argi;			/*  Index to command arguments	*/
	char	*prog;			/*  Pointer to program name	*/
					/*	within argv[0]		*/
	char	*cp;			/*  Ptr for scanning argv[0]	*/

	char	*outname;		/*  Filename from "-o file"	*/
	char	*outpath;		/*  Output path envirenment	*/
	FILE	*ou;			/*  Output stream		*/
	Img	*image;			/*  Image being printed		*/
	X11env	*X11_env;		/*  X11 interface environment	*/
	Window	 win;			/*  Window to print		*/
	char	*doc_name;		/*  Document name for
						 window or screen image	*/



/*-----------------------------------------------*/
/*	Locate program name at end of argv[0]	 */
/*-----------------------------------------------*/

prog = argv[0];
cp   = prog;

while ( *cp != 0 )			/*  Program name is 1st text	*/
      if ( *cp++ == '/' )		/*  after last '/' in argv[0]	*/
	 prog = cp;



/*------------------------------*/
/*      Set Orientation		*/
/*------------------------------*/

status  = 0;				/*  Presume all will work	*/
orient  = PIO_DEFAULT;			/*  Presume default orientation	*/
outname = NULL;				/*  Presume no "-o file"	*/

/*
	If invoked by "prportrait" or "prlandscape alias,
	default to the corresponding orientation
*/

if ( strcmp (prog, "prport") == 0 )
   orient = PIO_PORTRAIT;
if ( strcmp (prog, "prland") == 0 )
   orient = PIO_LANDSCAPE;


/*	Parse flags	    */

for ( argi = 1; argi < argc; ++ argi )	/*  Scan arguments	*/
    {
    char	*cp;

    cp = argv[argi];		/*  Quit at 1st not starting with '-'	*/
    if ( *cp++ != '-' )
       break;

    if ( *cp == '-' )		/*  Quit looking for '-' after '--'	*/
       {
       ++argi;
       break;
       }

    if ( *cp == 'p' )		/*  "-p[anything] = portrait mode	*/
       orient = PIO_PORTRAIT;

    else if ( *cp == 'l' )	/*  "-l[anything] = landscape mode	*/
       orient = PIO_LANDSCAPE;

    else if ( *cp == 'o' )	/*  "-o[utput_to] file"			*/
       {
       if ( ++argi >= argc )
	  {
	  fprintf (stderr, "%s:  Ignoring \"-o\" switch (filename omitted)\n",
			prog);
	  break;
	  }

       outname = argv[argi];
       }

    else			/*  Anything else:  Questionable syntax	*/
       {
       status = 1;
       fprintf (stderr, "%s:  Ignoring unknown switch \"%s\"\n",
			prog, argv[argi]);
       fprintf (stderr, "\tKnown switches are:\n\n");
       fprintf (stderr, "\t\t-l[andscape]\tPrint in landscape orientation\n");
       fprintf (stderr, "\t\t-p[ortrait] \tPrint in portrait orientation\n\n");
       }
    }


/*------------------------------*/
/*	Set up output path	*/
/*------------------------------*/

ou = stdout;				/*  Preset for stdout		*/
if ( outname != NULL )			/*  If "-o file" was specified	*/
   outpath = outname;			/*	use that file		*/
else					/*  Else			*/
   outpath = getenv ( "priout" );	/*  Check environment variable	*/

if ( outpath != NULL )
   {
   char	*cp;				/*  Parse priout env string	*/

#define	skipws while ((*cp == ' ') || (*cp == '\t')) ++cp;
#define	skipbs while ((*cp != ' ') && (*cp != '\t') && (*cp != 0)) ++cp;


   cp = outpath;
   skipws				/*  Skip leading white space	*/

   if ( *cp == '|' )			/*  '|' => pipe to program	*/
      {
      skipbs				/*  Skip to start of command	*/
      skipws
      ou = popen ( cp, "w" );		/*  Open pipe to command	*/
      if ( ou == NULL )
	 {
	 fprintf ( stderr, "%s:  Couldn't open pipe to %s to print image\n",
			prog, cp );
	 exit(2);
         }
      }
   else
      {					/*  Else write to file		*/
      if ( *cp == '>' )			/*  Allow '>', in case users	*/
	 {				/*	write shell code	*/
	 skipbs				/*	other than '| program'	*/
	 skipws				/*	to this environment	*/
	 }				/*	variable		*/

      if ( *cp == '\\' )		/*  Allow a VERY limited escape	*/
	 ++ cp;				/*  in case someone has a file	*/
					/*  named [-|>]*		*/
      ou = fopen ( expand_path(cp), "w" );  /*  Open file for output	*/
      if ( ou == NULL )
	 {
	 fprintf ( stderr, "%s:  Couldn't open file %s to print image\n",
			prog, cp );
	 exit(2);
         }
      }
   }


/*--------------------------------------------------*/
/*	If the command name at least one file,	    */
/*	print the named image files		    */
/*--------------------------------------------------*/

if ( argi < argc )
   {
   -- argi;

   while ( ++argi < argc )		/*  For each argument		*/
	 {				/*	Read each image file	*/
	 image = read_img ( argv[argi], any_image );
	 if ( image == NULL )
	    {
	    status = 2;
	    fprintf ( stderr, "%s:  Couldn't read image file %s\n",
			prog, argv[argi] );
	    continue;
	    }

	 if ( image -> imgtype == rgb_image )
	    convert_to_gray_mapped ( image );

	 print_img ( image, ou, orient, argv[argi] );
	 }

   exit (status);			/*  Return worst status		*/
   }



/*----------------------------------------------------------*/
/*	Print contents of a window or of entire screen	    */
/*----------------------------------------------------------*/

X11_env = start_X11 (argc, argv);	/*  Start X11 interface		*/
if ( X11_env == NULL )
   {
   fprintf ( stderr, "%s:  Couldn't start X11 interface\n", prog );
   exit(2);
   }

if ( strcmp(prog, "prscr") == 0 )	/*  If this is prscr invocation	*/
   win = X11_env -> rootwin;		/*     print root window	*/
else					/*  Else			*/
   {					/*     let user pick a window	*/
   int		 xstatus;
   Cursor	 cursor;		/*  Cursor for temp use		*/
   XEvent	 event;			/*  Event for marking window	*/
   XButtonEvent	*butev;			/*  Ptr for convenience		*/


   cursor = XCreateFontCursor ( X11_env->display, XC_circle );
   if (cursor == NULL)
      {
      fprintf ( stderr, "%s:  Can't create a cursor\n", prog );
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
			"%s:  Can't grab pointer (root window isn't viewable)\n", prog );
		stop_X11 ( X11_env );
		exit(2);

	case AlreadyGrabbed:
		fprintf (stderr,
			"%s:  Can't grab pointer (already grabbed)\n", prog );
		stop_X11 ( X11_env );
		exit(2);

	case GrabFrozen:
		fprintf (stderr,
			"%s:  Can't grab pointer (frozen by active grab)\n",
			prog );
		stop_X11 ( X11_env );
		exit (2);

	case GrabInvalidTime:
		fprintf (stderr,
			"%s:  Can't grab pointer (current time is invalid)\n",
			prog );
		stop_X11 ( X11_env );
		exit (2);

        default:			/*  Anything else:	*/
		break;			/*	Explicitly OK	*/
	}

   fprintf (stderr, "\n");
   fprintf (stderr, "\tPlease select the window to print\n");
   fprintf (stderr, "\tby clicking the mouse in that window.\n");
   fprintf (stderr, "\n");
   fprintf (stderr, "\tSelecting the screen background\n");
   fprintf (stderr, "\tprints the entire screen.\n" );
   fprintf (stderr, "\n");

   XNextEvent (X11_env->display, &event);/*  Get mouse buttoning event	*/
   butev = (XButtonEvent *) &event;

   if ( butev -> subwindow != 0 )
      win = butev -> subwindow;		/*  Extract window		*/
   else
      win = X11_env -> rootwin;

   XUngrabPointer ( X11_env->display, CurrentTime );
   }


fprintf ( stderr,
    "\n\tStand by...  Reformatting for the Imagen will take some time\n\n" );

if ( win == X11_env->rootwin )		/*  Set document name to either	*/
   doc_name = "Screen Image";		/*	"Screen Image" or	*/
else					/*	"Window Image"		*/
   doc_name = "Window Image";

image = get_img ( win, NULL, X11_env );	/*  Get window contents		*/
if ( image == NULL )
   {
   fprintf ( stderr, "%s:  Couldn't get image from X11\n", prog );
   stop_X11 ( X11_env );
   exit (2);
   }

if ( image -> imgtype == rgb_image )
   convert_to_gray_mapped ( image );

print_img ( image, ou, orient, doc_name );	/*  Print it.		*/

stop_X11 ( X11_env );			/*  Shut down X interface	*/
exit (status);				/*  Issue exit status		*/
}
