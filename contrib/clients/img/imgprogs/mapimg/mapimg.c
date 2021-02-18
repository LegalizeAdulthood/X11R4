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
======		Program mapimg:		Map RGB Image		======
======			      File mapimg.c			======
======								======
======================================================================

	See manual entry for info.


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Main change is adding this module header.

	Early '89	Paul Raveling
	Created module
*/



#include <stdio.h>

#include <imglib/imglib.h>	/*  Include sys-independent defs	*/


main(argc,argv)

	int	 argc;
	char	*argv[];

{
	Img	*imin;			/*  Input image		*/
	Img	*imou;			/*  Output image	*/
	int	 status;		/*  Returned status	*/
	int	 num_colors;		/*  Number of colors	*/



/*:::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort with a message	   */
/*:::::::::::::::::::::::::::::::::::::::::*/

#define	abort(msg) {fprintf(stderr,"mapimg:  %s\n",msg); exit(2);}



/*----------------------------------------------------------*/
/*	Read RGB image from file named in 1st argument	    */
/*----------------------------------------------------------*/

if ( argc < 3 )				/*  Be sure filename exists	*/
   abort("Argument missing  --  Use \"mapimg image number_of_colors\"")

num_colors = atoi(argv[2]);		/*  Get number of colors	*/
if ( (num_colors < 2) || (num_colors > 256) )
   {
   fprintf ( stderr, "mapimg:  Can't sensibly quantize to %d colors;\n",
			num_colors );
   abort("Simple Color-Mapped Images are limited to 2-256 colors")
   }

printf ( "\nReading image \"%s\"\n", argv[1] );
imin = read_img ( argv[1], any_image );		/*  Read RGB image	*/



/*---------------------------*/
/*	Quantize image       */
/*---------------------------*/

if ( imin->imgtype == rgb_image )
   {
   printf ( "\nConverting from RGB image to color mapped image,\n" );
   printf ( "\tusing at most %d colors\n", num_colors );
   if ( imin -> num_pixels > 75000 )
      printf ( "Stand by...  This is likely to be slow\n\n" );

   status = quant_rgb ( imin, num_colors );
   if ( status == 0 )
      abort("Couldn't quantize RGB image")

   imou = copy_img ( mapped_image, imin );
   if ( imou == NULL )
      abort("Couldn't copy quantized RGB image to color mapped image")
   }
else		/*  Starting with mapped image  */
   {
   printf ( "\nReducing color content of color mapped image,\n" );
   printf ( "\tusing at most %d colors\n", num_colors );

   status = quant_cmi ( imin, num_colors );
   if ( status == 0 )
      abort("Couldn't quantize color mapped image")
   imou = imin;
   }


/*------------------------------*/
/*	Write mapped image	*/
/*------------------------------*/

printf ( "\nWriting color mapped image \"%s\"\n", argv[1] );
describe_img ( imou, NULL, 0, NULL );

status = write_img ( argv[1], imou );
if ( status == 0 )
   abort("Couldn't write color mapped image")
else
   exit (0);
}
