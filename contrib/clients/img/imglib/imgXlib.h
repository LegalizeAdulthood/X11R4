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

/*
===============================================================================
=======                                                                 =======
=======           X11-Specific  Image  Library  Definitions	    	=======
=======                      File  imgXlib.h                            =======
=======                                                                 =======
===============================================================================


        This header file supplies image library definitions
	for public access by programs using imglib functions
	over the X Window System, Version 11.

	Anything that includes this header file should also include:

	  <imglib.h>		\*  Include sys-independent defs	*\

	  <X11/Xlib.h>
	  <X11/Xutil.h>		\*  Include X11 definitions		*\
	  <X11/Xos.h>

  File history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made gradually
	since creation of this header file.

	3-14-89		Paul Raveling
	Created file.

*/



/*-----------------------------------------------------------*/
/*---------	Basic X11 Interface Environment     ---------*/
/*-----------------------------------------------------------*/

typedef	struct	imglib_X11env
	{
	Display *display;	/*  Connected display description  */

	int	 screen;		/*  Screen number	*/
	int	 width;			/*  Width of screen	*/
	int	 height;		/*  Height of screen	*/
	int	 depth;			/*  Depth of screen	*/
	Visual	*defvisual;		/*  Default visual	*/
	Visual	*visual;		/*  Actual visual	*/
	int	 numvis;		/*  # visuals supported	*/
	XVisualInfo *visinfo;		/*  Visual info list	*/
	Window	 rootwin;		/*  Root window of scrn	*/
	Colormap cmap;			/*  Color map		*/
	GC	 gc;			/*  Graphics context	*/

/*	Standard options from command line and/or resources	*/

	Geom	 geometry;		/*  Geometry		*/
	int	 geommask;		/*  X11 geometry mask	*/
	int	 maxcolors;		/*  Max # colors to use	*/

	Byte	 force;			/*  Forced conversion:	*/
#define	  F_NOTHING	0		/*	Nothing (no conversion)	*/
#define	  F_GRAYSCALE	1		/*	Grayscale	*/
#define	  F_MONOCHROME	2		/*	Monochrome	*/

	Byte	 color_map;		/*  Color map:		*/
	Byte	 color_instances;	/*  Color instance:	*/
#define	  PRIVATE	0		/*	Private		*/
#define	  SHARED	1		/*	Shared		*/

#define	DEFAULT_COLOR_MAP	SHARED
#define	DEFAULT_COLOR_INSTANCES	PRIVATE

	} X11env;




/*:::::::::::::::::::::::::::::::::::::::::*/
/*	Sir:  System Image Resources	   */
/*:::::::::::::::::::::::::::::::::::::::::*/

typedef struct	imglib_Xres
	{
	X11env	*X11_env;		/*  X11 environment		*/ 
	int	 num_colors;		/*  Number of color cells	*/
	Word	 pixels[256];		/*  X11 pixels, 1 per color	*/
	Pixmap	 pix;			/*  X11 pixmap			*/
	Window	 win;			/*  X11 window			*/
	XEvent	 event;			/*  Last event on window	*/
	} Sir;



/*  -----------------------------------------------------------  */
/*  --------------------  Public Procedures  ------------------  */
/*  -----------------------------------------------------------  */

extern	int	 ck_colorcap ( /* X11_env */ );
		/*  Check read/write color capacity	Module imgXlib.c  */

extern	int	 create_sysres ( /* image, X11_env */ );
		/*  Create system-dependent resources	Module imgXlib.c  */

extern	void	 delete_sysres ( /* image */ );
		/*  Delete system-dependent resources	Module imgXlib.c  */

extern	int	display_img ( /* image, X11_env, event_mask */ );
		/*  Display image in new window		Module imgXlib.c  */

extern	Img	*get_img ( /* drawable, geometry, X11_env */ );
		/*  Get image from drawable		Module imgXlib.c  */

extern	XEvent	*maint_img ( /* image, X11_env */ );
		/*  Maintain displayed image		Module imgXlib.c  */

extern	int	 put_img ( /* image, drawable, geometry, X11_env */ );
		/*  Put image to drawable		Module imgXlib.c  */

extern	X11env	*start_X11 ( /* display_name */ );
		/*  Start up X interface		Module imgXlib.c  */

extern	void	 stop_X11 ( /* X11_env */ );
		/*  Stop (terminate) X interface	Module imgXlib.c  */

extern	void	 update_colors ( /* image */ );
		/*  Update X11 colormap			Module imgXlib.c  */
