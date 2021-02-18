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
=======		  Module Imgcvt  --  Image Conversion Functions		=======
=======				File imgcvt.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	This module includes a set of image conversion functions.
	These are generally simple conversions that fall short
	of hard-core full-bore image processing.


  Public Routines:

	color_to_gray	Convert color image to gray scale
	color_to_mono	Convert color image to dithered monochrome
	dither		Reduce color content by dithering
	img_init_dither_tables
			Initialize dithering tables

			img_init_dither_tables is shared with other
			imglib modules but isn't advertised as something
			useful to imglib users.


  Local Routines:

	rgb_to_mono	Convert RGB image to dithered monochrome


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	3-17-89		Paul Raveling
	Created module.
*/


/*

  Notes:

	The notes below are extracted from a program by Ed Falk.
	They're signed as notes by Lou Steinberg, and they describe
	the Floyd-Steinberg dither algorithm used here, as well
	as in modules color and imagen.

	------------------

 * [1] Floyd-Steinberg dither:
 *
 *
 *  I should point out that the actual fractions we used were, assuming
 *  you are at X, moving left to right:
 *
 *		    X     7/16
 *	     3/16   5/16  1/16    
 *
 *  Note that the error goes to four neighbors, not three.  I think this
 *  will probably do better (at least for black and white) than the
 *  3/8-3/8-1/4 distribution, at the cost of greater processing.  I have
 *  seen the 3/8-3/8-1/4 distribution described as "our" algorithm before,
 *  but I have no idea who the credit really belongs to.

 *  Also, I should add that if you do zig-zag scanning (see my immediately
 *  previous message), it is sufficient (but not quite as good) to send
 *  half the error one pixel ahead (e.g. to the right on lines you scan
 *  left to right), and half one pixel straight down.  Again, this is for
 *  black and white;  I've not tried it with color.
 *  -- 
 *					    Lou Steinberg

*/



#include <stdio.h>
#include <imglib/imglib.h>


extern	char	*malloc ();		/*  Allocate memory		*/
extern	char	*calloc ();		/*  Allocate memory differently	*/


#define	reg	 register



/*--------------------------------------------------------------*/
/*	Tables used to accelerate processing for dithering	*/
/*	... plus one for grayscale conversion			*/
/*	Shared with other modules for other dithering		*/
/*--------------------------------------------------------------*/

	Rgbpix	 img_rgbbw[2] = {0x00000000, 0x00ffffff};
				/*  RGB values for black and white	*/


/*
	Data below are initialized on 1st invocation
	of a dithering procedure
*/

	Byte	 img_dither_tables_uninitialized = 1;

	Rgbpix	 img_grayrgb[256];	/*  Gray value to RGB pixel	*/
					/*	Domain = [0,255]	*/
					/*	Range  = [0,255] for each
						RGB component, R=G=B	*/

	short	 img_errtab[256];	/*  Error in current pixel	*/
					/*	Domain = [0,255]	*/
					/*	Range  = [-127,+127]	*/

	/*   Tables below are slightly larger than required	*/
	/*   to suit their actual domains; this just makes	*/
	/*   things easier to think about as powers of 2	*/

	/*	Error tables are as large as possible in case	*/
	/*	of aberrant RGB values in color dithering	*/


	Byte	 img_cliptab[768];	/*  Clipped pixel values	*/
					/*	Domain = [-255,+512]	*/
					/*	Range  = [0,255]	*/

	int	 img_errw1[512];	/*  1/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/

	int	 img_errw3[512];	/*  3/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/

	int	 img_errw5[512];	/*  5/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/

	int	 img_errw7[512];	/*  7/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/



/*2
===============================================================================
=======                                                                 =======
=======	    img_init_dither_tables  --  Initialize Dithering Tables	=======
=======                                                                 =======
===============================================================================

  Invocation:

	img_init_dither_tables ()


  Functions:

	Initialize statically allocated dithering tables.
	These are used mainly to accelerate dithering by doing
	table lookups instead of more awkward processing.


  Input:	None


  Output:

	Dither tables defined at the start of this module
	are initialized.
*/



void	img_init_dither_tables ()
{
reg	int	 i;


for ( i=0; i < 256; ++i )		/*  Gray index -> RGB pixel table  */
    img_grayrgb[i] =   (i << pixel_red_shift)
		     | (i << pixel_green_shift)
		     | (i << pixel_blue_shift);

for ( i=0; i < 128; ++i )		/*  Error table, 1st half	*/
    img_errtab[i] = i;

for ( i=128; i < 256; ++i )		/*  Error table, 2nd half	*/
    img_errtab[i] = i- 255;

for ( i=0; i < 256; ++i )		/*  Pixel clipping table, 	*/
    img_cliptab[i] = 0;			/*	values clipped to 0	*/

for ( i=256; i < 512; ++i )		/*  Pixel clipping table, 	*/
    img_cliptab[i] = i - 256;		/*	values not clipped	*/

for ( i=512; i < 768; ++i )		/*  Pixel clipping table, 	*/
    img_cliptab[i] = 255;		/*	values clipped to 255	*/

for ( i=0; i < 512; ++i )		/*  1/16 of error + 256		*/
    img_errw1[i] = ((i-256) / 16) + 256;

for ( i=0; i < 512; ++i )		/*  3/16 of error + 256		*/
    img_errw3[i] = ((3*(i-256)) / 16) + 256;

for ( i=0; i < 512; ++i )		/*  5/16 of error + 256		*/
    img_errw5[i] = ((5*(i-256)) / 16) + 256;

for ( i=0; i < 512; ++i )		/*  7/16 of error + 256		*/
    img_errw7[i] = ((7*(i-256)) / 16) + 256;


img_dither_tables_uninitialized = 0;	/*  Mark "tables initialized"	*/
}


/*1
===============================================================================
=======                                                                 =======
=======	      color_to_gray --  Convert Color Image to Grayscale	=======
=======                                                                 =======
===============================================================================

  Invocation:

	color_to_gray (image)


  Functions:

	Convert colors in an image to gray scale.  Conversion
	is based on:

	    gray = .299*red + .587*green + .114*blue

	but uses the following approximation for the sake of speed:

	    gray = (77*red + 150*green + 29*blue) >> 8

	This is accurate enough to suit human perception, and
	it's adequately fast as long as the C compiler decomposes
	multiplication by constants into shifts, adds, and subtracts.

	If the image is an RGB image without a colormap, color_to_gray
	supplies a 256-entry grayscale map and sets the map index
	in all pixels.


  Input:

	image		Img *
			Pointer to image to convert


  Output:

	No direct output;  Image is modified to change colors to gray scale
*/



void	color_to_gray (image)

	Img	*image;			/*  Ptr to image to convert	*/

{
reg	int	 i;		/*  Index for color map		*/
reg	unsigned g;		/*  Gray value			*/





/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro sufficient to convert color map data:	   */
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	gray(r,g,b)	((77*r + 150*g + 29*b) >> 8)

/*	Perfect conversion would be .299*r + .587*g + .114*b	*/



if ( img_dither_tables_uninitialized )	/*  If not initialized yet,	*/
   img_init_dither_tables ();		/*     initialize dither tables	*/


/*----------------------------------------*/
/*	If the image has a color map	  */
/*	   convert it			  */
/*----------------------------------------*/

if ( image->colormap != NULL )
   for ( i=0; i < image->num_colors; ++i )	/*  For each color...	*/
       {
       reg Col	*c;			/*  Color map entry pointer	*/

       c = &image->colormap[i];			/*  Point to map entry	*/
       g = gray(c->red,c->green,c->blue);	/*  Derive gray value	*/
       c->red   = g;
       c->green = g;				/*  Replace RGB values	*/
       c->blue  = g;				/*  with gray value	*/
       }

if ( image->imgtype == mapped_image )
   return;

if ( image -> colormap != NULL )
   {
   /*---------------------------------------------------*/
   /*	RGB image with a colormap:  Convert pixels	*/
   /*---------------------------------------------------*/

   reg Rgbpix	*p;		/*  Pixel pointer for RGB image	*/
   reg unsigned	 red, grn, blu;	/*  Pixel RGB components	*/


   p = (Rgbpix *) image->pixdata;		/*  Point to pixel data	*/
   i = image->num_pixels;			/*  Set pixel counter	*/
   while ( i-- > 0 )				/*  For each pixel...	*/
	 {
	 red = (*p & pixel_red  ) >> pixel_red_shift;	/*  Extract	*/
	 grn = (*p & pixel_green) >> pixel_green_shift;	/*  colors	*/
	 blu = (*p & pixel_blue ) >> pixel_blue_shift;
	 *p &= pixel_tag;			/*  Clear RGB in pixel	*/
	 *p++ |= img_grayrgb[gray(red,grn,blu)];/*  Derive gray, store	*/
						/*	into RGB fields	*/
	 }
   }
else
   {
   /*------------------------------------------------------------*/
   /*	RGB image with a colormap:  Supply map & convert pixels	 */
   /*------------------------------------------------------------*/

   reg Rgbpix	*p;		/*  Pixel pointer for RGB image	*/
   reg unsigned	 gv;		/*  Grayscale value		*/
   reg unsigned	 red, grn, blu;	/*  Pixel RGB components	*/


   image->colormap = (Col *) calloc (256, sizeof(Col));
   image->num_colors = 256;
   for  ( i=0; i < 256; ++i )
	{
	image->colormap[i].freq  = 0;
	image->colormap[i].pixel = i;
	image->colormap[i].red   = i;
	image->colormap[i].green = i;
	image->colormap[i].blue  = i;
	}

   /*  Convert pixels	*/

   p = (Rgbpix *) image->pixdata;		/*  Point to pixel data	*/
   i = image->num_pixels;			/*  Set pixel counter	*/
   while ( i-- > 0 )				/*  For each pixel...	*/
	 {
	 red = (*p & pixel_red  ) >> pixel_red_shift;	/*  Extract	*/
	 grn = (*p & pixel_green) >> pixel_green_shift;	/*  colors	*/
	 blu = (*p & pixel_blue ) >> pixel_blue_shift;
	 gv  = gray(red,grn,blu);		/*  Derive gray value	*/
	 *p++ = (gv << pixel_tag_shift) | img_grayrgb[gv]; /*  Store as	*/
	 ++ image -> colormap[gv].freq;		/*  tag + r + g + b	*/
	 }
   }
}

/*2
===============================================================================
=======                                                                 =======
=======	   rgb_to_mono  --  Convert RGB Image to Dithered Monochrome	=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = rgb_to_mono (image)


  Functions:


	Convert colors in an RGB color image to gray scale and dither
	the grayscale values to black and white.

	This is a private routine called by color_to_mono to handle
	this form of image.


  Input:

	image		Img *
			Pointer to image to convert


  Output:

	status		int
	<return value>	Success/failure status:
			    0 => Failure:  Image isn't color mapped,
					   or is info-only (no pixels!)
			    1 => Success
*/



int	rgb_to_mono (image)

	Img	*image;		/*  Pointer to image to convert	*/

{
reg	Rgbpix	*cp;		/*  Current row pointer, mapped image	*/
reg	Rgbpix	*np;		/*  Next row pointer,    mapped image	*/
	Rgbpix	*rgbpixdata;	/*  image->pixdata, not needing a cast	*/
	int	 x;		/*  Current X			*/
	int	 y;		/*  Current Y			*/
reg	int	 err;		/*  Current error		*/
	int	 width;		/*  Local copy of image width	*/
	int	 height;	/*  Local copy of image height	*/
	int	 wm1;		/*  width - 1			*/
	int	 hm1;		/*  height - 1			*/
reg	int	 i;		/*  Table index			*/
	Byte	 gr;		/*  Temporary gray scale value	*/
	Mapinfo	*mi, *mn;	/*  Ptrs to color map info entries	*/



/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macros to convert pixel to black or white & save error	 */
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*
	rcvpix assumes the current pixel has already been converted from
	an RGB value to a grayscale value by an error propagation macro
*/

#define rcvpix { \
	err = img_errtab[*cp]; \
	*cp = img_rgbbw[*cp >> 7]; \
	}



/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macros to propagate part of error to another pixel	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*
	This also converts the destination pixel from a color map index
	to a grayscale value.  It uses this macro defined earlier:
*/
/*	#define	gray(r,g,b)	(77*r + 150*g + 29*b) >> 8	*/


#define	radjust(pixel,weight) { \
 gr = gray(((pixel&pixel_red)>>pixel_red_shift), \
	((pixel&pixel_green)>>pixel_green_shift), \
	((pixel&pixel_blue)>>pixel_blue_shift)); \
 pixel = img_grayrgb[img_cliptab[((int) gr) + ((err*weight) >> 4) + 256]]; }


#define	rcvgray(pixel) \
 pixel = gray(((pixel&pixel_red)>>pixel_red_shift), \
	((pixel&pixel_green)>>pixel_green_shift), \
	((pixel&pixel_blue)>>pixel_blue_shift));


#define	radjust1(pixel) \
 pixel = img_grayrgb[img_cliptab[pixel + img_errw1[err+256]]];

#define	rcadjust1(pixel) { \
 gr = gray(((pixel&pixel_red)>>pixel_red_shift), \
	((pixel&pixel_green)>>pixel_green_shift), \
	((pixel&pixel_blue)>>pixel_blue_shift)); \
 pixel = img_cliptab[gr + img_errw1[err+256]]; \
 }


#define	radjust3(pixel) \
pixel = img_cliptab[pixel + img_errw3[err+256]];


#define	radjust5(pixel) \
 pixel = img_cliptab[pixel + img_errw5[err+256]];


#define	radjust7(pixel) \
 pixel = img_cliptab[pixel + img_errw7[err+256]];




/*---------------------------------*/
/*	Convert an RGB image	   */
/*---------------------------------*/

rgbpixdata = (Rgbpix *) image -> pixdata;
width  = image -> width;
height = image -> height;
wm1    = width  - 1;
hm1    = height - 1;

/*
	Convert top row and left column from RGB to grayscale values;
	All other color-to-gray conversions happen when propagating error
	to the pixel below and to the right of each target pixel.
*/

cp  = rgbpixdata;				/*  Convert top row	*/
for ( x=0; x <= wm1; ++x )
    {
    rcvgray (*cp)
    ++ cp;
    }

cp  = &rgbpixdata[width];			/*  Convert left column	*/
for ( y=1; y <= hm1; ++y )
    {
    rcvgray (*cp)
    cp += width;
    }


/*
	Scan image, converting to (0,0,0) for black, (255,255,255) for white,
	converting pixel below and to right from color index to grayscale,
	and propagating weighted error forward to neighboring pixels.
*/

cp  = rgbpixdata;
np  = &rgbpixdata[width];

for ( y=0; y < hm1; ++y )
    {
    rcvpix			/*  Convert leftmost pixel on scan line	*/
    radjust7(*(cp+1))
    radjust5(*np)
    rcadjust1(*(np+1))
    ++ cp;  ++ np;

    for ( x=1; x < wm1; ++x )	/*  Convert pixels up to last on line	*/
	{
	rcvpix
	radjust7(*(cp+1))
	radjust3(*(np-1))
	radjust5(*np)
	rcadjust1(*(np+1))
	++ cp; ++ np;
	}

    rcvpix			/*  Convert last pixel on scan line	*/
    radjust3(*(np-1))
    radjust5(*np)
    ++ cp; ++ np;
    }


for ( x=0; x < wm1; ++x )	/*	Convert last row of image	*/
    {
    rcvpix
    radjust7(*(cp+1))
    ++ cp; ++ np;
    }

rcvpix



/*----------------------------------------------------------------------*/
/*	Replace original color map, if any, with a monochrome map	*/
/*----------------------------------------------------------------------*/

if ( image -> colormap == NULL )	/*  If there's no color map	*/
   return 1;				/*     just return.		*/

if ( image -> alloc & colmap_mem )	/*  If image owns old color map	*/
   free ( image -> colormap );		/*     free it			*/

mi = image -> cmiq;		/*  Free everything on color map queue	*/
while ( mi != NULL )
      {
      mn = mi -> next_map;
      free (mi);
      mi = mn;
      }

image -> cmiq = NULL;			/*  Show no color map queue	*/
image -> num_colors = 2;		/*  Indicate 2 colors in map	*/
image -> colormap = (Col *) calloc ( 2, sizeof(Col) );
if ( image -> colormap == NULL )
   {
   fprintf ( stderr, "color_to_mono:  Insufficient memory\n" );
   return 0;
   }

image -> colormap[0].red   = 0;
image -> colormap[0].green = 0;
image -> colormap[0].blue  = 0;		/*  Set map entry for black	*/
image -> colormap[0].pixel = 0;
image -> colormap[0].freq  = 0;

image -> colormap[1].red   = 255;
image -> colormap[1].green = 255;
image -> colormap[1].blue  = 255;	/*  Set map entry for white	*/
image -> colormap[1].pixel = 0;
image -> colormap[1].freq  = 0;

return 1;
}

/*1
===============================================================================
=======                                                                 =======
=======	 color_to_mono  --  Convert Color Image to Dithered Monochrome	=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = color_to_mono (image)


  Functions:


	Convert  a color image to gray scale and dither the grayscale
	values to black and white.

	If the image is color-mapped, this replaces the color map
	with a 2-entry map.  Pixel values become 0 for black,
	1 for white.


  Input:

	image		Img *
			Pointer to image to convert


  Output:

	status		int
	<return value>	Success/failure status:
			    0 => Failure:  Image isn't color mapped,
					   or is info-only (no pixels!)
			    1 => Success
*/



int	color_to_mono (image)

	Img	*image;		/*  Pointer to image to convert	*/

{
	Byte	*cp;		/*  Current row pointer, mapped image	*/
	Byte	*np;		/*  Next row pointer,    mapped image	*/
	int	 x;		/*  Current X			*/
	int	 y;		/*  Current Y			*/
	int	 err;		/*  Current error		*/
	int	 width;		/*  Local copy of image width	*/
	int	 height;	/*  Local copy of image height	*/
	int	 wm1;		/*  width - 1			*/
	int	 hm1;		/*  height - 1			*/
	int	 i;		/*  Table index			*/
	Byte	 gr;		/*  Temporary gray scale value	*/
	Mapinfo	*mi, *mn;	/*  Ptrs to color map info entries	*/




/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macros to convert pixel to black or white & save error	 */
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*
	mcvpix assumes the current pixel has already been converted from
	a pixmap index to a grayscale value by an error propagation macro
*/

#define mcvpix { \
	err = img_errtab[*cp]; \
	*cp >>= 7; \
	}



/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macros to propagate part of error to another pixel	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*
	This also converts the destination pixel from a color map index
	to a grayscale value.  It uses this macro defined earlier:
*/
/*	#define	gray(r,g,b)	(77*r + 150*g + 29*b) >> 8	*/


#define	madjust(pixel,weight) \
 gr = gray(image->colormap[pixel].red, image->colormap[pixel].green, \
		image->colormap[pixel].blue); \
 pixel = img_cliptab[((int) gr) + ((err*weight) >> 4) + 256];


#define	mcvgray(pixel) \
 pixel = gray(image->colormap[pixel].red, image->colormap[pixel].green, \
		image->colormap[pixel].blue);


#define	madjust1(pixel) \
 pixel = img_cliptab[pixel + img_errw1[err+256]];

#define	mcadjust1(pixel) { \
 gr = gray(image->colormap[pixel].red, image->colormap[pixel].green, \
		image->colormap[pixel].blue); \
 pixel = img_cliptab[gr + img_errw1[err+256]]; }


#define	madjust3(pixel) \
 pixel = img_cliptab[pixel + img_errw3[err+256]];


#define	madjust5(pixel) \
 pixel = img_cliptab[pixel + img_errw5[err+256]];


#define	madjust7(pixel) \
 pixel = img_cliptab[pixel + img_errw7[err+256]];



/*-----------------------------------------*/
/*	Convert a color mapped image	   */
/*	or farm out an RGB image	   */
/*-----------------------------------------*/

if ( img_dither_tables_uninitialized )	/*  If not initialized yet,	*/
   img_init_dither_tables ();		/*     initialize dither tables	*/

if ( image->imgtype == rgb_image )
   return rgb_to_mono (image);

if ( image->imgtype != mapped_image )	/*  Complain if this is an	*/
   {					/*	info-only-image		*/
   fprintf ( stderr,
	"color_to_mono:  Image is info-only, there's no data to convert!\n" );
   return 0;
   }

width  = image -> width;
height = image -> height;
wm1    = width  - 1;
hm1    = height - 1;

/*
	Convert top row and left column from pixel indices to grayscale;
	All other color-to-gray conversions happen when propagating error
	to the pixel below and to the right of each target pixel.
*/

cp  = image -> pixdata;			/*  Convert top row	*/
for ( x=0; x <= wm1; ++x )
    {
    mcvgray (*cp)
    ++ cp;
    }

cp  = &(image -> pixdata[width]);	/*  Convert left column	*/
for ( y=1; y <= hm1; ++y )
    {
    mcvgray (*cp)
    cp += width;
    }


/*
	Scan image, converting to 0 for black, 1 for white,
	converting pixel below and to right from color index to grayscale,
	and propagating weighted error forward to neighboring pixels.
*/

cp  = image -> pixdata;
np  = &(image -> pixdata[width]);

for ( y=0; y < hm1; ++y )
    {
    mcvpix			/*  Convert leftmost pixel on scan line	*/
    madjust7(*(cp+1))
    madjust5(*np)
    mcadjust1(*(np+1))
    ++ cp;  ++ np;

    for ( x=1; x < wm1; ++x )	/*  Convert pixels up to last on line	*/
	{
	mcvpix
	madjust7(*(cp+1))
	madjust3(*(np-1))
	madjust5(*np)
	mcadjust1(*(np+1))
	++ cp; ++ np;
	}

    mcvpix			/*  Convert last pixel on scan line	*/
    madjust3(*(np-1))
    madjust5(*np)
    ++ cp; ++ np;
    }


for ( x=0; x < wm1; ++x )	/*	Convert last row of image	*/
    {
    mcvpix
    madjust7(*(cp+1))
    ++ cp; ++ np;
    }

mcvpix



/*--------------------------------------------------------------*/
/*	Replace original color map with a monochrome map	*/
/*--------------------------------------------------------------*/

if ( image -> alloc & colmap_mem )	/*  If image owns old color map	*/
   free ( image -> colormap );		/*     free it			*/

mi = image -> cmiq;		/*  Free everything on color map queue	*/
while ( mi != NULL )
      {
      mn = mi -> next_map;
      free (mi);
      mi = mn;
      }

image -> cmiq = NULL;			/*  Show no color map queue	*/
image -> num_colors = 2;		/*  Indicate 2 colors in map	*/
image -> colormap = (Col *) calloc ( 2, sizeof(Col) );
if ( image -> colormap == NULL )
   {
   fprintf ( stderr, "color_to_mono:  Insufficient memory\n" );
   return 0;
   }

image -> colormap[0].red   = 0;
image -> colormap[0].green = 0;
image -> colormap[0].blue  = 0;		/*  Set map entry for black	*/
image -> colormap[0].pixel = 0;
image -> colormap[0].freq  = 0;

image -> colormap[1].red   = 255;
image -> colormap[1].green = 255;
image -> colormap[1].blue  = 255;	/*  Set map entry for white	*/
image -> colormap[1].pixel = 0;
image -> colormap[1].freq  = 0;

return 1;
}
