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
=======	       Module Imagen  --  Imagen Reformatting Functions		=======
=======			     File imagen.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	This module includes a set of functions to reformat an
	image to dithered monochrome at a larger scale and print
	the image on an Imagen printer.

	These are minimally modified versions of routines from
	the old X10.4 printpix utility.  We hope to replace
	the relative crudity of most in the foreseeable future.


  Public Routines:

	print_img	Print image


  Local Routines:

	add_byte	Buffer 8 bits of data for Imagen
	write_patches	Write array of patches for Imagen
	write_imp_init  Write initialization for Imagen
	write_imp_end	Terminate output for Imagen
	dithergm	Dither grayscale to monochrome
	dither2x2	Generate 2x2 dithered output
	dither3x3	Generate 3x3 dithered output
	dither4x4	Generate 4x4 dithered output


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	3-27-89		Paul Raveling
	Created module from a major part of printpix.c.  Some
	of printpix's code was adopted from a different program
	for printing rasters to Imagens.  Unfortunately I no longer
	remember what that program was.
*/



#include <stdio.h>
#include <pwd.h>
#include <imglib/imglib.h>


extern	char	*malloc ();		/*  Allocate memory		*/
extern	char	*calloc ();		/*  Allocate memory differently	*/
extern	unsigned short	 getuid();
extern	struct	passwd	*getpwuid ();





/*	Data for Imagen formatting	*/


#define     MAG_POWER           0	/* can be 0, 1, or 2 */
#define     DOTS_PER_INCH       300	/* For Imagen 8/300 */

#define	    W1  8*DOTS_PER_INCH / 3	/*  1st image width threshold	*/
#define	    W2	8*DOTS_PER_INCH / 4	/*  2nd image width threshold	*/

#define     OPAQUE              3
#define     OPERATION_TYPE      OPAQUE

#define     LOBYTE(number) ((char)(number & 0x00FF))
#define     HIBYTE(number16) ((char)((number16 >> 8) & 0x00FF))

#define     PATCH_SIZE          32
#define     PATCH_BYTES         4



typedef Byte patch[ PATCH_SIZE ][ PATCH_BYTES ] ;

static	int	h_patches ;		/* Number of Impress patches across  */
static	int	v_patches ;		/* Number of Impress patches down    */

static	Byte	*newpix;		/*  New rendition of pixel data	*/
static	int	 newwidth;		/*  New rendition width		*/
static	int	 newheight;		/*  New rendition height	*/
static	int	 newsize;		/*  New rendition size in pixels*/

static	Img	*img;			/*  Image to print		     */
static	FILE	*ou;			/*  File for output		     */
static	int	 orient;		/*  Orientation code		     */
static	char	*doc_name;		/*  Filename, or other document name */
static	char	 owner[100];		/*  Owner's (user's) name	     */

static	patch	*patches;		/*  Data used only within add_byte   */
static	short	 patch_num;		/*  but initialized once for	     */
static	short	 patch_row;		/*  each image by print_img	     */
static	short	 patch_col;


#define	BLACK	1		/*  Imagen black	*/
#define	WHITE	0		/*  Imagen white	*/



/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro sufficient to convert color map data:	   */
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	gray(r,g,b)	(77*r + 150*g + 29*b) >> 8

/*	"Perfect" conversion would be .299*r + .587*g + .114*b	*/



/*--------------------------------------------------------------*/
/*	Data related to dithering, defined in module imgcvt	*/
/*--------------------------------------------------------------*/
/*	Tables used to accelerate processing for dithering	*/
/*	... plus one for grayscale conversion			*/
/*	Shared with other modules for other dithering		*/
/*--------------------------------------------------------------*/

extern	Byte	 img_dither_tables_uninitialized;
extern	void	 img_init_dither_tables ();

extern	Rgbpix	 img_grayrgb[256];	/*  Gray value to RGB pixel	*/
					/*	Domain = [0,255]	*/
					/*	Range  = [0,255] for each
						RGB component, R=G=B	*/

extern	short	 img_errtab[256];	/*  Error in current pixel	*/
					/*	Domain = [0,255]	*/
					/*	Range  = [-127,+127]	*/

	/*   Tables below are slightly larger than required	*/
	/*   to suit their actual domains; this just makes	*/
	/*   things easier to think about as powers of 2	*/

	/*	Error tables are as large as possible in case	*/
	/*	of aberrant RGB values in color dithering	*/


extern	Byte	 img_cliptab[768];	/*  Clipped pixel values	*/
					/*	Domain = [-255,+512]	*/
					/*	Range  = [0,255]	*/

extern	int	 img_errw1[512];	/*  1/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/

extern	int	 img_errw3[512];	/*  3/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/

extern	int	 img_errw5[512];	/*  5/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/

extern	int	 img_errw7[512];	/*  7/16 of error + 256		*/
					/*	Domain = [-255,+255]	*/


/*1
===============================================================================
=======                                                                 =======
=======			print_img  --  Print Image			=======
=======                                                                 =======
===============================================================================

  Invocation:

	print_img ( image, prfile, orientation, docname )


  Functions:

	Print the given image on an Imagen printer.  Convert
	color to grayscale and dither to monochrome.


  Input:

	image		Img *
			Pointer to image to print

	prfile		FILE *
			Pointer to output file; file must already
			be open and will be left open when print_img
			returns.  Callers should prefer to supply
			a pipe to lpr or an equivalent program, but
			writing to disk also works.  It just eats
			a lot of space.

	orientation	int
			Orientation code, defined in <imglib.h>:

			  PIO_DEFAULT:	    Default:  Choose best of:
			  PIO_PORTRAIT:		Portrait (unrotated)
			  PIO_LANDSCAPE:	Landscape (rotated 90-deg)

	docname		char *
			Pointer to document name (name of file containing
			image or some other string, such as "Screen Dump")


  Output:	No direct output:

		Memory is buffered into a local "patches" array, then
		written to the designated output file.
*/



void	print_img ( image, prfile, orientation, docname )

	Img	*image;			/*  Pointer to image to print	*/
	FILE	*prfile;		/*  File to write [to printer]	*/
	int	 orientation;		/*  Orientation on page		*/
	char	*docname;		/*  Document name		*/

{
	Byte	*p;			/*  Pointer to a [pixel] byte	*/
	int	 i;			/*  Universal index		*/



/*---------------------------------------------------------------*/
/*	Save a few parameters and initialize a few data		 */
/*---------------------------------------------------------------*/

img	 = image;			/*  Image to print		*/
ou	 = prfile;			/*  Output file pointer		*/
orient	 = orientation;			/*  Image orientation		*/
doc_name = docname;			/*  Document name		*/

patches   = NULL;
patch_num = 0;				/*  Data used within add_byte	*/
patch_row = 0;
patch_col = 0;



/*---------------------------------------*/
/*	Choose orientation and scale	 */
/*---------------------------------------*/

if ( orient == PIO_DEFAULT )
   {
   /*	Choose (seemingly) best orientation	*/

   int	maxdim;
   int	whichmax;

   if ( image -> height > image -> width )
      {
      maxdim = image -> height;
      whichmax = 0;
      }
   else
      {
      maxdim = image -> width;
      whichmax = 1;
      }

   if ( maxdim > 1024 )
      {
      if ( whichmax == 0 )
         orient = PIO_PORTRAIT;
      else
	 orient = PIO_LANDSCAPE;
      }
   else
      {
      if ( maxdim <= W1 )
	 {
	 if ( whichmax == 0 )
	    orient = PIO_PORTRAIT;
	 else
	    orient = PIO_LANDSCAPE;
	 }
      else
	 {
	 if ( whichmax == 0 )
	    orient = PIO_LANDSCAPE;
	 else
	    orient = PIO_PORTRAIT;
	 }
      }
   }



/*------------------------------*/
/*	Dither the Data		*/
/*------------------------------*/

if ( orient == PIO_PORTRAIT )
   {
   if ( image->width > W2 )
      dither2x2 (orient);

   if ( (image->width > W1) && (image->width <= W2) )
      dither3x3 (orient);

   if ( image->width <= W1 )
      dither4x4 (orient);
   }
else
   {
   if ( image->height > W2 )
      dither2x2 (orient);

   if ( (image->height > W1) && (image->height <= W2) )
      dither3x3 (orient);

   if ( image->height <= W1 )
      dither4x4 (orient);
   }




/*--------------------------------------*/
/*	Write dithered data to file	*/
/*--------------------------------------*/

h_patches = newwidth  / 32;
v_patches = newheight / 32;

write_imp_init() ;		/*  Write header for Imagen		*/

p = newpix;			/*  Write image to Imagen raster	*/
i = 0;

while ( i < newsize )
      {
      Byte	 cc;

      cc = *p++;
      cc = (cc << 1) + *p++;
      cc = (cc << 1) + *p++;
      cc = (cc << 1) + *p++;
      cc = (cc << 1) + *p++;
      cc = (cc << 1) + *p++;
      cc = (cc << 1) + *p++;
      cc = (cc << 1) + *p++;

      i += 8;
      add_byte ( cc );
      }

write_imp_end() ;		/*  Terminate file	*/
fflush(ou);			/*  Flush last data out	*/


/*------------------------------------------------------*/
/*	Release allocated memory blocks and return	*/
/*------------------------------------------------------*/

free (patches);			/*  Free data used here	*/
free (newpix);

}



/*2
===============================================================================
=======                                                                 =======
=======		  Add_byte:  Buffer 8-bits of data for Imagen		=======
=======                                                                 =======
===============================================================================

  Invocation:

	add_byte ( one_byte )


  Functions:

	Buffer 8 bits of data for the Imagen, placing the byte
	in the appropriate patch.

	Add_byte is called sequentially for the entire image to
	scan the raster in the most natural order:  Across each
	raster line, top to bottom.

	This routine came from the public domain sunimp program,
	which appears to have come from a public domain macimp program.


  Input:

	one_byte	Byte
			8 bits of bitmap data
			(leftmost bit is MSB, rightmost bit is LSB)


  Output:	No direct output:

		Memory is buffered into the "patches" array, then
		written to file "ou" when the patches are full.

*/



static	add_byte( one_byte )
	Byte	 one_byte ;
{
/*
	Data used only here,
	defined in module header,
	intialized in print_img:

     static patch *patches = NULL ; 
     static short patch_num = 0 ; 
     static short patch_row = 0 ;
     static short patch_col = 0 ;
*/

     if( patches == NULL )		/* First time only */
        if( ( patches=(patch *)calloc(h_patches,sizeof(patch)) ) == NULL )
	  {
	  fprintf (stderr,"Insufficient memory to convert to Imagen raster\n");
	  exit (1);
	  }


     patches[ patch_num ][ patch_row ][ patch_col ] = one_byte ;

     if ( ++ patch_col >= PATCH_BYTES )
     {
          patch_col = 0 ;
          if ( ++ patch_num >= h_patches )
          {
               patch_num = 0 ; 
               if ( ++ patch_row >= PATCH_SIZE )
               {
                    patch_row = 0 ;
                    write_patches ( patches ) ;
               }
          }
     }
}


/*2
===============================================================================
=======                                                                 =======
=======		Write_patches:  Write array of patches for Imagen	=======
=======                                                                 =======
===============================================================================

  Invocation:

	write_patches ( patches )


  Functions:

	Write an array of patches for the Imagen.  Each patch
	is a 32-pixel square raster, packed at 8 pixels per byte.

	This routine came from the public domain sunimp program,
	which appears to have come from a public domain macimp program.


  Input:

	patches		patch	*
			Pointer to array of patches to write


  Output:	No direct output:

		Data from "patches" array is written to file "ou".

*/




static	write_patches ( patches )
     patch * patches ;
{
     short patchnum, patchrow, patchcol ;
     
     for ( patchnum = 0 ; patchnum < h_patches ; patchnum ++ )
          for ( patchrow = 0 ; patchrow < PATCH_SIZE ; patchrow ++ )
               for ( patchcol = 0 ; patchcol < PATCH_BYTES ; patchcol ++ )
                    putc( patches[ patchnum ][ patchrow ][ patchcol ], ou ) ;
}


/*2
===============================================================================
=======                                                                 =======
=======		Write_imp_init:  Write initialization for Imagen	=======
=======                                                                 =======
===============================================================================

  Invocation:

	write_imp_init ()


  Functions:

	Write an header containing information to identify the printed
	file and setting up for raster output.

	This routine came from the public domain sunimp program,
	which appears to have come from a public domain macimp program.
	It has been modified slightly.


  Input:	No direct input:

		write_imp_init uses system calls to get the current
		time and the user's name.


  Output:	No direct output:

		Various data are written to file "ou".

*/



static	write_imp_init()
{
	struct	passwd	*pp;
     char *ctime(), cimptime[26];
     long time(), imptime;

     imptime = time(0);			/* get time in internal form */
     strcpy (cimptime, ctime(&imptime));/* put time in string form */
     cimptime[24] = '\0';		/* nullify \n in time string */
     pp = getpwuid ( getuid() );

     strcpy ( owner, pp->pw_name );
     if ( (*owner >= 'a') && (*owner <= 'z') )
        *owner += 'A' - 'a';

     fprintf(ou, "@Document(Language ImPress, Name \"%s\"",
		doc_name ) ;
     if ( pp != NULL )
        fprintf(ou, ", Jobheader On, Owner \"%s\"", owner );
     else
	fprintf(ou, ", Jobheader On, Owner \"Unknown\"" );
     fprintf(ou, ", Spooldate \"%s\")",cimptime);
     /* SET_ABS_H */
          putc( (char) 135, ou ) ; 
          putc( HIBYTE( (short) DOTS_PER_INCH/4 ), ou ) ;
          putc( LOBYTE( (short) DOTS_PER_INCH/4 ), ou ) ;

     /* SET_ABS_V */
          putc( (char) 137, ou ) ; 
          putc( HIBYTE( (short) DOTS_PER_INCH ), ou ) ;
          putc( LOBYTE( (short) DOTS_PER_INCH ), ou ) ;

     /* SET_MAGNIFICATION */
          putc( (char) 236, ou ) ;
	  putc( (char) MAG_POWER, ou ) ;

     /* BITMAP */
          putc( (char) 235, ou ) ;
	  putc( (char) OPERATION_TYPE, ou ) ;
          putc( (char) h_patches, ou ) ;
	  putc( (char) v_patches, ou ) ;
}


/*2
===============================================================================
=======                                                                 =======
=======		   Write_imp_end:  Terminate output for Imagen		=======
=======                                                                 =======
===============================================================================

  Invocation:

	write_imp_end ()


  Functions:

	Write a simple trailer to terminate the page and the file
	for the Imagen.

	This routine came from the public domain sunimp program,
	which appears to have come from a public domain macimp program.


  Input:	None


  Output:	No direct output:

		Various data are written to file "ou".

*/


static	write_imp_end()
{
     /* ENDPAGE */ 
        putc( (char) 219, ou ) ;

     /* EOF */ 
        putc( (char) 255, ou ) ;
}


/*2
===============================================================================
=======                                                                 =======
=======		  dithergm  --  Dither gray scale to monochrome		=======
=======                                                                 =======
===============================================================================

  Invocation:

	dithergm (pix, newwidth, height)


  Functions:

	Dither the resized image to monochrome.


  Input:
	
	pix		Byte *
			Pointer to new pixel data, rescaled, possibly
			rotated, converted from color to gray scale,
			then dithered to monochrome

	width		int
			Width of image represented by newpix

	height		int
			Height of image represented by newpix



  Output:	No direct output; data in module header::

	*pix		Byte[]
			Data converted from gray scale to dithered
			monochrome

*/



static	dithergm (pix, width, height)

	Byte	*pix;		/*  Pixel data to dither	*/
	int	 width;		/*  Width of data to dither	*/
	int	 height;	/*  Height of data to dither	*/

{
	Byte	*cp;		/*  Current row pointer		*/
	Byte	*np;		/*  Next row pointer		*/
	int	 x;		/*  Current X			*/
	int	 y;		/*  Current Y			*/
	int	 err;		/*  Current error		*/
	int	 wm1;		/*  width - 1			*/
	int	 hm1;		/*  height - 1			*/
	int	 i;		/*  Table index			*/




/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to convert pixel to BLACK or WHITE & save error	 */
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#if	BLACK			/*  If black is 1, white is 0	*/
#define cvpix { \
	err = img_errtab[*cp]; \
	*cp = (*cp >> 7) ^ 1; \
	}
#else				/*  If black is 0, white is 1	*/
#define cvpix { \
	err = img_errtab[*cp]; \
	*cp >>= 7; \
	}
#endif


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macros to propagate part of error to another pixel	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	adjust(pixel,weight) \
 pixel = img_cliptab[((int) pixel) + ((err*weight) >> 4) + 256

#define	adjust1(pixel) \
 pixel = img_cliptab[pixel + img_errw1[err+256]];

#define	adjust3(pixel) \
 pixel = img_cliptab[pixel + img_errw3[err+256]];

#define	adjust5(pixel) \
 pixel = img_cliptab[pixel + img_errw5[err+256]];

#define	adjust7(pixel) \
 pixel = img_cliptab[pixel + img_errw7[err+256]];


/*----------------------*/
/*	Scan Image	*/
/*----------------------*/

if ( img_dither_tables_uninitialized )
   img_init_dither_tables ();

wm1 = width  - 1;
hm1 = height - 1;
cp  = pix;
np  = &pix[width];

for ( y=0; y < hm1; ++y )
    {
    cvpix
    adjust7(*(cp+1))
    adjust5(*np)
    adjust1(*(np+1))
    ++ cp;  ++ np;

    for ( x=1; x < wm1; ++x )
	{
	cvpix
	adjust7(*(cp+1))
	adjust3(*(np-1))
	adjust5(*np)
	adjust1(*(np+1))
	++ cp; ++ np;
	}

    cvpix
    adjust3(*(np-1))
    adjust5(*np)
    ++ cp; ++ np;
    }

for ( x=0; x < wm1; ++x )
    {
    cvpix
    adjust7(*(cp+1))
    ++ cp; ++ np;
    }

cvpix
}


/*2
===============================================================================
=======                                                                 =======
=======		   dither2x2:  Generate 2x2 dithered output		=======
=======                                                                 =======
===============================================================================

  Invocation:

	dither2x2 (orient)


  Functions:

	1.  Convert color pixel values to gray scale.
	2.  Enlarge the image by doubling pixels.
	3.  If image is in landscape mode, rotate it 90 degrees.
	4.  Dither the grayscale pixels to monochrome.

	The first three functions use a single pass over the data.
	Dithering calls dither () to make a pass over the enlarged data.


  Input:

	orient		Output image orientation:
			PIO_PORTRAIT (unrotated) or PIO_LANDSCAPE (rotated)

	Not supplied directly:

	img		Img  *
			Pointer to original image


  Output:	No direct output; data in module header::

	newpix		Byte *
			Pointer to new pixel data, rescaled, possibly
			rotated, converted from color to gray scale,
			then dithered to monochrome

	newwidth	int
			Width of image represented by newpix

	newheight	int
			Height of image represented by newpix

	newsize		int
			Size of image represented by newpix in pixels
			(or Bytes)

*/



static	dither2x2 (orient)

	int	 orient;

{
	int	 nw;		/*  New width		*/
	int	 nh;		/*  New height		*/
	int	 nnp;		/*  New # pixels	*/

	int	 ow;		/*  Old width,  possibly rotated	*/
	int	 oh;		/*  Old height, possibly rotated	*/

	int	 i, j;
	Byte	 gsmap[256];	/*  Gray scale map	*/
	Byte	 gc;		/*  Gray scale color	*/

	Byte	*s;		/*  Source pixel ptr	*/
	Byte	*du;		/*  Upper dest. ptr	*/
	Byte	*dl;		/*  Lower dest. ptr	*/

/*	Data used only for rotating to landscape mode:	   */

	int	 k;		/*  Row index		*/
	Byte	*col;		/*  Pixel column index	*/




/*---------------------------------*/
/*	Set up gray scale map	   */
/*---------------------------------*/

for ( i = 0; i < img->num_colors; ++ i )
    gsmap[i] =  gray (  img->colormap[i].red,
			img->colormap[i].green,
			img->colormap[i].blue);




/*--------------------------------------------------------------*/
/*	Translate from old pixel indices to gray scale data	*/
/*	while enlarging and perhaps rotating			*/
/*--------------------------------------------------------------*/

if ( orient == PIO_PORTRAIT )
   {
   nw = 2 * img->width;
   nh = 2 * img->height;
   ow = img->width;
   oh = img->height;
   }
else
   {
   nw = 2 * img->height;
   nh = 2 * img->width;
   ow = img->height;
   oh = img->width;
   }

if ( (nw & 31) != 0 )
   nw = nw + 32 - (nw & 31);

if ( (nh & 31) != 0 )
   nh =  nh + 32 - (nh & 31);

nnp = nw * nh;

newpix = (Byte *) malloc (nnp);
if ( newpix == NULL )
   {
   fprintf ( stderr, "Insufficient memory for second copy of image\n" );
   exit (1);
   }

if ( orient == PIO_PORTRAIT )
   s = img->pixdata;
else
   {
   s = img->pixdata + img->width - 1;
   k = 0;
   col = s;
   }
du = newpix;
dl = &newpix[nw];
j  = 0;					/*  0 pixels from current line	*/

for ( i=0; i < img->num_pixels; ++i )
    {
    if ( orient == PIO_PORTRAIT )
       gc = gsmap[*s++];
    else
       {
       gc = gsmap[*s];
       s += img->width;
       if ( ++k >= img->height )
	  {
	  k = 0;
	  -- col;
	  s = col;
	  }
       }

    *du++ = gc;
    *du++ = gc;
    *dl++ = gc;
    *dl++ = gc;

    if ( ++j >= ow )			/*  Advance dest. pointers	*/
       {				/*	at end of each row	*/
       j *= 2;
       while ( j++ < nw )		/*  Pad line if necessary	*/
	     {
	     *du++ = BLACK;
	     *dl++ = BLACK;
	     }
       j = 0;
       du += nw;
       dl += nw;
       }
    }


/*	Pad bottom if necessary		*/

for ( i = 2*oh; i < nh; ++i )
    for ( j=0; j < nw; ++j )
	*du++ = BLACK;



newwidth  = nw;
newheight = nh;				/*  Save new image parameters	*/
newsize   = nnp;

dithergm(newpix, newwidth, newheight);	/*  Dither to monochrome	*/
}


/*2
===============================================================================
=======                                                                 =======
=======		   dither3x3:  Generate 3x3 dithered output		=======
=======                                                                 =======
===============================================================================

  Invocation:

	dither3x3 (orient)


  Functions:

	1.  Convert color pixel values to gray scale.
	2.  Enlarge the image by doubling pixels.
	3.  If image is in landscape mode, rotate it 90 degrees.
	4.  Dither the grayscale pixels to monochrome.

	The first three functions use a single pass over the data.
	Dithering calls dither () to make a pass over the enlarged data.


  Input:

	orient		Output image orientation:
			PIO_PORTRAIT (unrotated) or PIO_LANDSCAPE (rotated)

	Not supplied directly:

	img		Img  *
			Pointer to original image


  Output:	No direct output:

	newpix		Byte *
			Pointer to new pixel data, rescaled, possibly
			rotated, converted from color to gray scale,
			then dithered to monochrome

	newwidth	int
			Width of image represented by newpix

	newheight	int
			Height of image represented by newpix

	newsize		int
			Size of image represented by newpix in pixels
			(or Bytes)

*/



static	dither3x3 (orient)

	int	 orient;

{
	int	 nw;		/*  New width		*/
	int	 nh;		/*  New height		*/
	int	 nnp;		/*  New # pixels	*/

	int	 ow;		/*  Old width,  possibly rotated	*/
	int	 oh;		/*  Old height, possibly rotated	*/

	int	 i, j;
	Byte	 gsmap[256];	/*  Gray scale map	*/
	Byte	 gc;		/*  Gray scale color	*/

	Byte	*s;		/*  Source pixel ptr	*/
	Byte	*du;		/*  Upper dest. ptr	*/
	Byte	*dm;		/*  Middle dest. ptr	*/
	Byte	*dl;		/*  Lower dest. ptr	*/


/*	Data used only for rotating to landscape mode:	   */

	int	 k;		/*  Row index		*/
	Byte	*col;		/*  Pixel column index	*/








/*---------------------------------*/
/*	Set up gray scale map	   */
/*---------------------------------*/

for ( i = 0; i < img->num_colors; ++ i )
    gsmap[i] =  gray (  img->colormap[i].red,
			img->colormap[i].green,
			img->colormap[i].blue);




/*--------------------------------------------------------------*/
/*	Translate from old pixel indices to gray scale data	*/
/*	while enlarging and perhaps rotating			*/
/*--------------------------------------------------------------*/

if ( orient == PIO_PORTRAIT )
   {
   nw = 3 * img->width;
   nh = 3 * img->height;
   ow = img->width;
   oh = img->height;
   }
else
   {
   nw = 3 * img->height;
   nh = 3 * img->width;
   ow = img->height;
   oh = img->width;
   }

if ( (nw & 31) != 0 )
   nw = nw + 32 - (nw & 31);

if ( (nh & 31) != 0 )
   nh =  nh + 32 - (nh & 31);

nnp = nw * nh;

newpix = (Byte *) malloc (nnp);
if ( newpix == NULL )
   {
   fprintf ( stderr, "Insufficient memory for second copy of image\n" );
   exit (1);
   }


if ( orient == PIO_PORTRAIT )
   s = img->pixdata;
else
   {
   s = img->pixdata + img->width - 1;
   k = 0;
   col = s;
   }
du = newpix;
dm = &newpix[nw];
dl = &newpix[2*nw];
j  = 0;					/*  0 pixels from current line	*/

for ( i=0; i < img->num_pixels; ++i )
    {
    if ( orient == PIO_PORTRAIT )
       gc = gsmap[*s++];
    else
       {
       gc = gsmap[*s];
       s += img->width;
       if ( ++k >= img->height )
	  {
	  k = 0;
	  -- col;
	  s = col;
	  }
       }

    *du++ = gc;
    *du++ = gc;
    *du++ = gc;

    *dm++ = gc;
    *dm++ = gc;
    *dm++ = gc;

    *dl++ = gc;
    *dl++ = gc;
    *dl++ = gc;

    if ( ++j >= ow )		/*  Advance dest. pointers	*/
       {				/*	at end of each row	*/
       j *= 3;
       while ( j++ < nw )		/*  Pad line if necessary	*/
	     {
	     *du++ = BLACK;
	     *dm++ = BLACK;
	     *dl++ = BLACK;
	     }
       j = 0;
       du += 2*nw;
       dm += 2*nw;
       dl += 2*nw;
       }
    }


/*	Pad bottom if necessary		*/

for ( i = 3*oh; i < nh; ++i )
    for ( j=0; j < nw; ++j )
	*du++ = BLACK;


newwidth  = nw;
newheight = nh;				/*  Save new image parameters	*/
newsize   = nnp;

dithergm(newpix, newwidth, newheight);	/*  Dither to monochrome	*/
}



/*2
===============================================================================
=======                                                                 =======
=======		   dither4x4:  Generate 4x4 dithered output		=======
=======                                                                 =======
===============================================================================

  Invocation:

	dither4x4 (orient)


  Functions:

	1.  Convert color pixel values to gray scale.
	2.  Enlarge the image by doubling pixels.
	3.  If image is in landscape mode, rotate it 90 degrees.
	4.  Dither the grayscale pixels to monochrome.

	The first three functions use a single pass over the data.
	Dithering calls dither () to make a pass over the enlarged data.


  Input:

	orient		Output image orientation:
			PIO_PORTRAIT (unrotated) or PIO_LANDSCAPE (rotated)

	Not supplied directly:

	img		Img  *
			Pointer to original image


  Output:	No direct output:

	newpix		Byte *
			Pointer to new pixel data, rescaled, possibly
			rotated, converted from color to gray scale,
			then dithered to monochrome

	newwidth	int
			Width of image represented by newpix

	newheight	int
			Height of image represented by newpix

	newsize		int
			Size of image represented by newpix in pixels
			(or Bytes)

*/



static	dither4x4 (orient)

	int	 orient;

{
	int	 nw;		/*  New width		*/
	int	 nh;		/*  New height		*/
	int	 nnp;		/*  New # pixels	*/

	int	 ow;		/*  Old width,  possibly rotated	*/
	int	 oh;		/*  Old height, possibly rotated	*/

	int	 i, j;
	Byte	 gsmap[256];	/*  Gray scale map	*/
	Byte	 gc;		/*  Gray scale color	*/

	Byte	*s;		/*  Source pixel ptr	*/
	Byte	*du;		/*  Upper dest. ptr	*/
	Byte	*dum;		/*  Upper middle d_ptr	*/
	Byte	*dlm;		/*  Lower middle d_ptr	*/
	Byte	*dl;		/*  Lower dest. ptr	*/



/*	Data used only for rotating to landscape mode:	   */

	int	 k;		/*  Row index		*/
	Byte	*col;		/*  Pixel column index	*/





/*---------------------------------*/
/*	Set up gray scale map	   */
/*---------------------------------*/

for ( i = 0; i < img->num_colors; ++ i )
    gsmap[i] =  gray (  img->colormap[i].red,
			img->colormap[i].green,
			img->colormap[i].blue);




/*--------------------------------------------------------------*/
/*	Translate from old pixel indices to gray scale data	*/
/*	while enlarging and perhaps rotating			*/
/*--------------------------------------------------------------*/

if ( orient == PIO_PORTRAIT )
   {
   nw = 4 * img->width;
   nh = 4 * img->height;
   ow = img->width;
   oh = img->height;
   }
else
   {
   nw = 4 * img->height;
   nh = 4 * img->width;
   ow = img->height;
   oh = img->width;
   }

if ( (nw & 31) != 0 )
   nw = nw + 32 - (nw & 31);

if ( (nh & 31) != 0 )
   nh =  nh + 32 - (nh & 31);

nnp = nw * nh;

newpix = (Byte *) malloc (nnp);
if ( newpix == NULL )
   {
   fprintf ( stderr, "Insufficient memory for second copy of image\n" );
   exit (1);
   }


if ( orient == PIO_PORTRAIT )
   s = img->pixdata;
else
   {
   s = img->pixdata + img->width - 1;
   k = 0;
   col = s;
   }
du = newpix;
dum = &newpix[nw];
dlm = &newpix[2*nw];
dl  = &newpix[3*nw];
j  = 0;					/*  0 pixels from current line	*/

for ( i=0; i < img->num_pixels; ++i )
    {
    if ( orient == PIO_PORTRAIT )
       gc = gsmap[*s++];
    else
       {
       gc = gsmap[*s];
       s += img->width;
       if ( ++k >= img->height )
	  {
	  k = 0;
	  -- col;
	  s = col;
	  }
       }

    *du++  = gc;
    *du++  = gc;
    *du++  = gc;
    *du++  = gc;

    *dum++ = gc;
    *dum++ = gc;
    *dum++ = gc;
    *dum++ = gc;

    *dlm++ = gc;
    *dlm++ = gc;
    *dlm++ = gc;
    *dlm++ = gc;

    *dl++  = gc;
    *dl++  = gc;
    *dl++  = gc;
    *dl++  = gc;

    if ( ++j >= ow )			/*  Advance dest. pointers	*/
       {				/*	at end of each row	*/
       j *= 4;
       while ( j++ < nw )		/*  Pad line if necessary	*/
	     {
	     *du++  = BLACK;
	     *dum++ = BLACK;
	     *dlm++ = BLACK;
	     *dl++  = BLACK;
	     }
       j = 0;
       du  += 3*nw;
       dum += 3*nw;
       dlm += 3*nw;
       dl  += 3*nw;
       }
    }


/*	Pad bottom if necessary		*/

for ( i = 4*oh; i < nh; ++i )
    for ( j=0; j < nw; ++j )
	*du++ = BLACK;



newwidth  = nw;
newheight = nh;				/*  Save new image parameters	*/
newsize   = nnp;

dithergm(newpix, newwidth, newheight);	/*  Dither to monochrome	*/
}
