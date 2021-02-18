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
=======		    Module Imgio  --  Read and Write Images		=======
=======				File imgio.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	This module reads and writes image files.  It creates
	an Img in order to read a file, but doesn't delete the
	Img when a file is written.

	All code here is part of the layer of image processing 
	functions which is independent of the underlying window
	system, graphics support layer, or operating system.

	Although read_image and write_image accept both color-mapped
	and RGB images, they actually include logic only for
	color mapped images.  They handle RGB images by invoking
	read_rgb and write_rgb, which are local routines.


  Public Routines:

	describe_img	Describe image
	read_img	Read image from file
	write_img	Write image to file


  Local Routines:

	read_rgb	Read RGB image from file
	write_rgb	Write RGB image to file


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	7-14-89		Paul Raveling
	Fixed bug:  Reading an RGB file gobbled the first 4 bytes
	of associated data in the header.

	3-14-89		Paul Raveling
	Created file, based partly on older utility definitions.
*/



#include <stdio.h>
#include <imglib/imglib.h>


extern	char	*malloc ();		/*  Allocate memory		*/
extern	char	*calloc ();		/*  Allocate memory differently	*/


/*2
===============================================================================
=======                                                                 =======
=======		     read_rgb --  Read RGB Image from Files		=======
=======                                                                 =======
===============================================================================

  Invocation:

	image = read_rgb ( filename, imgtype )


  Functions:

	Read an RGB image from four files.  See read_img for more info.


  Input:

	filename	char *
			Base name of files to read:  if filename is
			"clyde", the image files are clyde.a, clyde.r,
			clyde.g, and clyde.b.

	imgtype		int
			Type of image to read:  rgb_image or rgb_info.


  Output:

	image		img *
	<return value>	pointer to img structure for the new image
			null => the image couldn't be read:  either
			free memory is insufficient, an input error
			occurred, or a format error exists in the
			image file.
*/


static	Img	*read_rgb (filename, imgtype)

	char	*filename;		/*  Name of file to read	*/
	int	 imgtype;		/*  Type of image to read	*/

{
	Img	*image;		/*  Image to return			*/

	FILE	*attfile;	/*  Stream to read attribute file	*/
	FILE	*compfile;	/*  Stream to read R, G, or B component	*/

	int	 width;		/*  Width of image			*/
	int	 height;	/*  Height of image			*/
	int	 size;		/*  Image size in number of pixels	*/
	int	 status;	/*  Status code				*/
	int	 bc;		/*  Byte count [from fread]		*/
	int	 i;		/*  Universal index			*/

	int	 length;	/*  Length of associated data		*/
	int	 bleng;		/*  Length of block containing asssoc data  */
	Byte	*asdata;	/*  Pointer to associated data		*/
	Byte	*bp;		/*  Pointer to a Byte (of assoc data)	*/

	Byte	*compdata;	/*  RGB component data			*/
	Rgbpix	*pixdata;	/*  Data as Rgbpix pixels		*/
	Rgbpix	*pp;		/*  Pointer into pixdata		*/

	Byte	 whrtext[12];	/*  Text of width/height/reserved	*/
	int	 mi, sc;	/*  Used within rgbi4 macro		*/




/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to encode unsigned integer of up to 4 digits	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	rgbi4(ivar) { \
 ivar = 0; \
 mi = 0; \
 while (mi++ < 4) { sc = *bp++; if (sc != ' ') break; } \
 -- mi; \
 while (mi++ < 4) \
	{ \
	if ( (sc < '0') || (sc > '9') ) \
	   abortrgbrd("Invalid integer in image attribute file") \
	ivar *= 10; \
	ivar += sc - '0'; \
	sc = *bp++; \
	} \
 --bp; }




/*:::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort after an error	 */
/*:::::::::::::::::::::::::::::::::::::::*/

#define	abortrgbrd(msg) { \
 fprintf ( stderr, "read_img:  %s\n", msg ); \
 fprintf (stderr, "read_img:  Can't read image  %s\n", filename); \
 if (asdata != NULL) free(asdata); \
 if (compdata != NULL) free(compdata); \
 if (pixdata  != NULL) free(pixdata); \
 return NULL; }



/*--------------------------------------------------------------*/
/*	Preset all pointers to allocatable memory to NULL	*/
/*--------------------------------------------------------------*/

asdata		= NULL;
compdata	= NULL;
pixdata		= NULL;



/*---------------------------------*/
/*	Read image attributes	   */
/*---------------------------------*/

attfile = (FILE *) open_yafile ( filename, "r", ".a" );
if ( attfile == NULL )
   abortrgbrd("Can't locate attribute file")

bc = fread ( whrtext, 1, 12, attfile );	/*  Read 12 bytes	*/
if ( bc < 12 )
   abortrgbrd("Image attribute file too short")

bp = whrtext;
rgbi4(width)
rgbi4(height)

if ( width == 0 )
   abortrgbrd("Width = 0 => no image")

if ( height == 0 )
   abortrgbrd("Height = 0 => no image")



/*----------------------------------------*/
/*	Read associated data, if any	  */
/*----------------------------------------*/

asdata = (Byte *) malloc (512);		/*  Allocate a buffer		*/
if ( asdata == NULL )
   abortrgbrd("Insufficient memory")

bleng  = 512;				/*  Initial buffer length = 512	*/
length = 0;				/*  Currently contains 0 bytes	*/
bp     = asdata;

while (1)				/*  Buffer consecutive bytes	*/
      {					/*  up to end of file		*/
      status = getc (attfile);
      if ( status == EOF )
	 break;

      *bp++ = (Byte) status;		/*  Buffer next byte		*/
      ++ length;			/*  Increment length		*/
      if ( length >= bleng )		/*  If data fills buffer	*/
	 {				/*     Allocate bigger buffer	*/
         Byte	*newbuf;
	 Byte	*obp;

	 newbuf = (Byte *) malloc (bleng+512);
	 if ( newbuf == NULL )
	    abortrgbrd("Insufficient memory")
	 bleng += 512;			/*  Update buffer length	*/
	 obp = asdata;			/*  Copy old data to new buffer	*/
	 bp  = newbuf;
	 for ( i=0; i < length; ++i )
	     *bp++ = *obp;

	 free (asdata);			/*  Release old buffer		*/
	 asdata = newbuf;		/*  Substitute new buffer	*/
	 }
      }

if ( length == 0 )			/*  If no associated data exists */
   {					/*     free buffer's memory	*/
   free (asdata);
   asdata = NULL;
   }

close_yafile (attfile);			/*  Close attribute file.	*/


/*----------------------------------------------*/
/*	If image type is rgb_info		*/
/*	create and return info-only image	*/
/*----------------------------------------------*/

if ( imgtype == rgb_info )
   {
   image = create_img ( imgtype, width, height, length, asdata,
			NULL, NULL, NULL );
   if ( image == NULL )
      abortrgbrd("Couldn't create image")
   return image;
   }



/*-----------------------------------------*/
/*	Set up to read RGB components	   */
/*-----------------------------------------*/

size	   = width * height;		/*  Keep # of pixels	*/
compdata   = (Byte *) malloc (size);
pixdata    = (Rgbpix *) calloc (size, sizeof(Rgbpix));

if ( (compdata == NULL) || (pixdata == NULL) )
   abortrgbrd("Insufficient memory")



/*------------------------------*/
/*	Read red component	*/
/*------------------------------*/

compfile = (FILE *) open_yafile ( filename, "r", ".r" );
if ( compfile == NULL )
   abortrgbrd("Can't locate red pixel data file")

bc = fread ( compdata, 1, size, compfile );  /*  Read red pixels	*/
if ( bc < size )
   {
   fprintf ( stderr,
	"read_img:  Warning -- Red pixel file has only %d pixels;\n",
	bc );
   fprintf ( stderr, "           Missing values assumed to be 0\n" );
   for ( i=bc; i < size; ++i )
       compdata[i] = 0;
   }

for ( i=0; i<size; ++i )		/*  Pack component into pixels	*/
    {
    pixdata[i] = 0;			/*  Before 1st data, clear pixel */
    pixdata[i] |= compdata[i] << pixel_red_shift;
    }

close_yafile ( compfile );



/*------------------------------*/
/*	Read green component	*/
/*------------------------------*/

compfile = (FILE *) open_yafile ( filename, "r", ".g" );
if ( compfile == NULL )
   abortrgbrd("Can't locate green pixel data file")

bc = fread ( compdata, 1, size, compfile );  /*  Read green pixels	*/
if ( bc < size )
   {
   fprintf ( stderr,
	"read_img:  Warning -- Green pixel file has only %d pixels;\n",
	bc );
   fprintf ( stderr, "           Missing values assumed to be 0\n" );
   for ( i=bc; i < size; ++i )
       compdata[i] = 0;
   }

for ( i=0; i<size; ++i )		/*  Pack component into pixels	*/
    pixdata[i] |= compdata[i] << pixel_green_shift;

close_yafile ( compfile );



/*------------------------------*/
/*	Read blue component	*/
/*------------------------------*/

compfile = (FILE *) open_yafile ( filename, "r", ".b" );
if ( compfile == NULL )
   abortrgbrd("Can't locate blue pixel data file")

bc = fread ( compdata, 1, size, compfile );  /*  Read blue pixels	*/
if ( bc < size )
   {
   fprintf ( stderr,
	"read_img:  Warning -- Blue pixel file has only %d pixels;\n",
	bc );
   fprintf ( stderr, "           Missing values assumed to be 0\n" );
   for ( i=bc; i < size; ++i )
       compdata[i] = 0;
   }

for ( i=0; i<size; ++i )		/*  Pack component into pixels	*/
    pixdata[i] |= compdata[i] << pixel_blue_shift;

close_yafile ( compfile );




/*-----------------------------------*/
/*	Create and return image	     */
/*-----------------------------------*/

free (compdata);			/*  Release component data buffer  */

image = create_img ( imgtype, width, height, length, asdata,
			0, NULL, pixdata );
if ( image == NULL )
   abortrgbrd("Couldn't create image")

if ( asdata != NULL )			/*  Mark blocks allocated	*/
   image -> alloc |= asdata_mem;	/*    to the image		*/
if ( pixdata != NULL )
   image -> alloc |= pixdata_mem;

return image;				/*  Return the image		*/
}

/*2
===============================================================================
=======                                                                 =======
=======		     write_rgb --  Write RGB Image to Files		=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = write_rgb ( filename, image )


  Functions:

	Write an RGB image to four files.  See write_img for more info.


  Input:

	filename	char *
			Base for names of files to write.  Name can include
			references to home directories (~, ~username),
			environment variables ($whatnot) in addition
			to ordinary path info verbiage.

	image		Img *
			Pointer to image to write.


  Output:

	status		int
			Status:  1 => completed successfully,
				 0 => couldn't write one or more files

			Inability to write the file may be due to
			exhaustion of disk space.  It's been known
			to happen to folks with big bunches of images...

*/



static	int	write_rgb (filename, image)

	char	*filename;		/*  Pointer to filename	*/
	Img	*image;			/*  Pointer to image	*/

{
	FILE	*pixfile;		/*  Output FILE pointer		*/
	char	*indfname;		/*  Individual filename		*/
	char	*cp;			/*  Pointer to end of name	*/
	Byte	*bp;			/*  Pointer to a byte		*/
	Rgbpix	*pp;			/*  Pointer to an RGB pixel	*/
	int	 i;			/*  Universal index		*/
	int	 status;		/*  Status of a putc operation	*/
	Byte	 b;			/*  A byte			*/




/*:::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort after an error	 */
/*:::::::::::::::::::::::::::::::::::::::*/

#define	abortrgbwr(msg) { \
 fprintf ( stderr, "write_img:  %s\n", msg ); \
 if (pixfile != NULL) { \
	fprintf ( stderr, "write_img:  Can't write image file %s\n", \
	((Yafile *) pixfile) -> actual_name ); \
	close_yafile (pixfile); }\
 else \
	fprintf (stderr, "write_img:  Can't write image file %s\n",filename); \
 return 0; }




/*--------------------------------------------------*/
/*	Set up to generate different filenames	    */
/*--------------------------------------------------*/

pixfile = NULL;				/*  No file open yet		*/
				/*  Allocate space for full filename	*/
indfname = (char *) malloc ( strlen(filename) + 5 );
if ( indfname == NULL )
   abortrgbwr("Insufficient memory")

strcpy ( indfname, filename );	/*  Copy "base" name to working name	*/
cp = indfname;
while ( *cp++ != 0 )			/*  Find end of base name	*/
      ;
-- cp;



/*--------------------------------*/
/*	Write attribute file	  */
/*--------------------------------*/

*cp++ = '.';				/*  Attr filename is 'name.a'	*/
*cp++ = 'a';
*cp   = 0;

pixfile = (FILE *) open_yafile ( indfname, "w", NULL );
if ( pixfile == NULL )
   abortrgbwr("Can't open file -- Is path write protected or out of disk space?")

i = fprintf ( pixfile, "%4u%4u    ", image->width, image->height );
if ( i < 0 )
   abortrgbwr("Can't write to file -- Is disk out of space?")

if ( image->asdata_length != 0 )	/*  If associated data exists	*/
   {					/*  Append it to width & height	*/
   i  = image -> asdata_length;
   bp = image -> asdata;

   while ( i-- > 0 )
	 {
	 status = putc ( *bp++, pixfile );
	 if ( status == EOF )
	    abortrgbwr("Can't write to file -- Is disk out of space?")
	 }
   }

close_yafile (pixfile);

if ( image->imgtype == rgb_info )	/*  If image is info only	*/
   return 1;				/*     output's complete	*/




/*--------------------------------------*/
/*	Write red component file	*/
/*--------------------------------------*/

*--cp = 'r';				/*  Make extension '.r.Z'	*/
cp[1] = '.';
cp[2] = 'Z';
cp[3] = 0;

pixfile = (FILE *) open_yafile ( indfname, "w", NULL );
if ( pixfile == NULL )
   abortrgbwr("Can't open file -- Is path write protected or out of disk space?")

i  = image -> num_pixels;		/*  Write red value of each pixel  */
pp = (Rgbpix *) image -> pixdata;

while ( i-- > 0 )
      {
      b = (*pp++ & pixel_red) >> pixel_red_shift;
      status = putc ( b, pixfile );
      if ( status == EOF )
	 abortrgbwr("Can't write to file -- Is disk out of space?")
      }

close_yafile (pixfile);




/*--------------------------------------*/
/*	Write green component file	*/
/*--------------------------------------*/

*cp = 'g';				/*  Make extension '.g.Z'	*/

pixfile = (FILE *) open_yafile ( indfname, "w", NULL );
if ( pixfile == NULL )
   abortrgbwr("Can't open file -- Is path write protected or out of disk space?")

i  = image -> num_pixels;		/*  Write green value of each pixel  */
pp = (Rgbpix *) image -> pixdata;

while ( i-- > 0 )
      {
      b = (*pp++ & pixel_green) >> pixel_green_shift;
      status = putc ( b, pixfile );
      if ( status == EOF )
	 abortrgbwr("Can't write to file -- Is disk out of space?")
      }

close_yafile (pixfile);




/*--------------------------------------*/
/*	Write blue component file	*/
/*--------------------------------------*/

*cp = 'b';				/*  Make extension '.b.Z'	*/

pixfile = (FILE *) open_yafile ( indfname, "w", NULL );
if ( pixfile == NULL )
   abortrgbwr("Can't open file -- Is path write protected or out of disk space?")

i  = image -> num_pixels;		/*  Write blue value of each pixel  */
pp = (Rgbpix *) image -> pixdata;

while ( i-- > 0 )
      {
      b = (*pp++ & pixel_blue) >> pixel_blue_shift;
      status = putc ( b, pixfile );
      if ( status == EOF )
	 abortrgbwr("Can't write to file -- Is disk out of space?")
      }

close_yafile (pixfile);



return 1;				/*  Return status for success	*/
}


/*1
===============================================================================
=======                                                                 =======
=======			describe_img  --  Describe image		=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = describe_img ( image, outfile, format, heading )


  Functions:

	Describe image characteristics.

	This writes text describing the image's geometry, color map
	(if any), and associated data (if any).  Various pieces of
	info can be omitted or formatted differently under control
	of the "format" parameter.

	Defaulting file to NULL and format to 0 produces what we
	think a user would most likely want to see most of the time.

	Details of this are likely to change.


  Input:

	image		Img *
			Pointer to image to describe

	outfile		FILE *
			Pointer to stream to report image info on;
			NULL => use stdout

	format		int
			Mask for info to display:

			  0x01 => Heading text
			  0x02 => Image type
			  0x04 => Geometry & size
			  0x08 => Color map size
			  0x10 => Associated data length
			  0x20 => Associated data contents
			  0x40 => Color map contents

			  0x00 defaults to 0x3f

	heading		char *
			Heading text; NULL => "\n\tImage description:\n\n"


  Output:	None to caller
*/


void	 describe_img ( image, outfile, format, heading )

	Img	*image;		/*  Pointer to image to describe	*/
	FILE	*outfile;	/*  File to write description to	*/
	int	 format;	/*  Format to use (actually field mask)	*/
	char	*heading;	/*  Heading text			*/

{
/*------------------------------------------*/
/*	Translate defaulted parameters	    */
/*------------------------------------------*/

if ( outfile == NULL )			/*  Default file to stdout	*/
   outfile = stdout;

if ( format == 0 )			/*  Default format to 0x1f	*/
   format = 0x3f;

if ( heading == NULL )
   heading = "\n\tImage description:\n\n";



/*--------------------------------------*/
/*	Start chucking out info...	*/
/*--------------------------------------*/

if ( (format & 0x01) != 0 )
   fprintf ( outfile, "%s", heading );

if ( (format & 0x02) != 0 )
   switch ( image->imgtype )
	{
	case mapped_image:
	case mapped_info:
		fprintf ( outfile, "\tType:\t\t\tColor mapped\n" );
		break;
	case rgb_image:
	case rgb_info:
		fprintf ( outfile, "\tType:\t\t\tRGB (24 bits per pixel)\n" );
		break;
	}

if ( (format & 0x04) != 0 )
   fprintf ( outfile, "\tGeometry:\t\t%d x %d      (%d pixels)\n",
		image->width, image->height, image->num_pixels );

if ( (format & 0x08) != 0 )
   {
   if ( image->colormap != NULL )
      fprintf (outfile, "\tColor map size:\t\t%d colors\n", image->num_colors);
   else
      fprintf ( outfile, "\t(No color map)\n" );
   }


if ( (format & 0x30 != 0 ) && (image->asdata_length == 0) )
   fprintf ( outfile, "\t(No associated data)\n" );

if ( ((format & 0x10) != 0) && (image->asdata_length != 0) )
   {
   if ( (format & 0x20) != 0 )
      fprintf (outfile, "\tAssociated data,\t%d bytes:\n",
		 image->asdata_length);
   else
      fprintf (outfile, "\tAssociated data:\t%d bytes\n",
		 image->asdata_length);
   }

if ( ((format & 0x20) != 0) && (image->asdata_length != 0) )
   {
   int	 hex;		/*  Use either ascii or hex	*/
   Byte	*bp;
   Byte	 b;
   int	 i;

   bp  = image->asdata;
   i   = image->asdata_length;
   hex = 0;

   while ( i-- > 0 )
	 {
	 b = *bp++;
	 if ( (b >= 0x20) && (b <= 0x7e) )
	    continue;
	 if (   (b=='\0') || (b=='\n') || (b=='\t')
	     || (b=='\f') || (b=='\r') || (b == '\b') )
	    continue;
	 hex = 1;
	 break;
	 }

   bp  = image->asdata;
   i   = image->asdata_length;
   if ( (format & 0x10) == 0 )
      fprintf ( outfile, "\tAssociated data:\n" );
   fprintf ( outfile, "\t" );

   if ( hex )
      while ( i-- > 0 )
	 fprintf ( outfile, "%02X ", *bp++ );
   else
      {
      fprintf ( outfile, "\"" );

      while ( i-- > 0 )
	 {
	 b = *bp++;
	 if ( (b >= 0x20) && (b <= 0x7e) )
	    {
	    putc ( b, outfile );
	    continue;
	    }
	 switch ( b )
		{
		case '\0':	fprintf ( outfile, "\\0" ); break;
		case '\n':	fprintf ( outfile, "\\n" ); break;
		case '\r':	fprintf ( outfile, "\\r" ); break;
		case '\f':	fprintf ( outfile, "\\f" ); break;
/*		case '\t':	fprintf ( outfile, "\\t" ); break;	*/
		case '\t':	fprintf ( outfile, "\t" );  break;
		case '\b':	fprintf ( outfile, "\\b" ); break;
		}
	 }

      fprintf ( outfile, "\"" );
      }
   fprintf ( outfile, "\n" );
   }

if ( (format & 0x40) != 0 )
   {
   int	 i;

   fprintf ( outfile, "\tColor map:\n\n" );

   for  ( i=0; i < image->num_colors; ++i )
	fprintf ( outfile, "\t\t\t%02X%02X%02X  =  (%3u %3u %3u)\n",
		image->colormap[i].red,
		image->colormap[i].green,
		image->colormap[i].blue,
		image->colormap[i].red,
		image->colormap[i].green,
		image->colormap[i].blue );
   }

fprintf ( outfile, "\n" );
}


/*1
===============================================================================
=======                                                                 =======
=======		       read_img --  Read Image from File		=======
=======                                                                 =======
===============================================================================

  Invocation:

	image = read_img ( filename, imgtype )


  Functions:

	Read an image file.

	Under control of imgtype it's possible to read only
	image attributes, or image attributes + color map.

	For a color mapped image, read_img reads a single
	optionally compressed file.  If the given filename
	can't be opened, read_img checks for "filename.p[.Z]"
	Choice of ".p" is a bit arbitrary, but suggests some
	relation to X11 pixmaps.

	For an RGB image, read_img reads four files with
	names of the form:

	    filename.a[.Z]	Attributes of image
	    filename.r[.Z]	Red pixel components
	    filename.g[.Z]	Green pixel components
	    filename.b[.Z]	Blue pixel components

	Although RGB images can have color maps while they're
	in use, their files never have color maps.

	If called to read an image of type "any_image" or "any_info",
	read_image reads a color mapped image if one is available.
	If there is no mapped image, it reads an RGB image.

	"Attributes" are the Img fields indicating the image's
	width, height, number of colors, number of pixels, and
	associated data.


  Input:

	filename	char *
			Name of file to read.  Name can include
			references to home directories (~, ~username),
			environment variables ($whatnot) in addition
			to ordinary path info verbiage.

	imgtype		int
			Type of image, as defined in imglib.h:

			  mapped_image	Color mapped
			  mapped_info	Color mapped, Info only:  No pixel data
			  rgb_image	32-bit RGB, unmapped
			  rgb_info	RGB, Info only:  No pixel data

			  any_image	Either mapped or RGB image
			  any_info	Either mapped or RGB, no pixel data


  Output:

	image		Img *
	<return value>	Pointer to Img structure for the new image
			NULL => the image couldn't be read:  Either
			free memory is insufficient, an input error
			occurred, or a format error exists in the
			image file.
*/



Img	*read_img ( filename, imgtype )

	char	*filename;		/*  Name of file to read	*/
	int	 imgtype;		/*  Type of image to read	*/

{
	Img	*image;			/*  Ptr to Img structure	*/
	FILE	*pixfile;		/*  Input FILE pointer		*/

	char	*cp;			/*  Pointer to some character	*/
	Byte	*bp;			/*  Pointer to some byte	*/
	int	 sc;			/*  Status or char from getc	*/
	int	 mi;			/*  Index/counter for macros	*/
	int	 bc;			/*  Byte count [from fread]	*/
	int	 bs;			/*  Block size of a section	*/
	int	 version;		/*  File version number		*/

	char	 file_id[8];		/*  File identifier text	*/
	char	 prefix[10];		/*  Section prefix		*/
	char	*atblock;		/*  Attribute block		*/
	Byte	*cmblock;		/*  Color map block		*/
	Byte	*pdblock;		/*  Pixel data block		*/
	Byte	*asdata;		/*  Associated data		*/
	Col	*colmap;		/*  Color map			*/

	int	 width;			/*  Image width			*/
	int	 height;		/*  Height			*/
	int	 num_colors;		/*  Number of colors		*/
	int	 num_pixels;		/*  Number of pixels		*/
	int	 asdata_length;		/*  # bytes of associated data	*/
	int	 i;			/*  General purpose index	*/




/*::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to read a section prefix &	*/
/*	encode section length into bc		*/
/*::::::::::::::::::::::::::::::::::::::::::::::*/

#define	rsp(s1,s2,secmes) { \
 bc = fread ( prefix, 1, 10, pixfile ); \
 if ( bc < 10 ) \
    abortrd("Image file too short") \
 if ((prefix[0] != s1) || (prefix[1] != s2)) \
    abortrd(secmes) \
 cp = &prefix[2]; \
 i8(bc) }





/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to encode unsigned integer of up to 4 digits	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	i4(ivar) { \
 ivar = 0; \
 mi = 0; \
 while (mi++ < 4) { sc = *cp++; if (sc != ' ') break; } \
 -- mi; \
 while (mi++ < 4) \
	{ \
	if ( (sc < '0') || (sc > '9') ) \
	   abortrd("Invalid integer in image file header") \
	ivar *= 10; \
	ivar += sc - '0'; \
	sc = *cp++; \
	} --cp; }




/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to encode unsigned integer of up to 8 digits	*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	i8(ivar) { \
 ivar = 0; \
 mi = 0; \
 while (mi++ < 8) { sc = *cp++; if (sc != ' ') break; } \
 -- mi; \
 while (mi++ < 8) \
	{ \
	if ( (sc < '0') || (sc > '9') ) \
	   abortrd("Invalid integer in image file header") \
	ivar *= 10; \
	ivar += sc - '0'; \
	sc = *cp++; \
	} --cp; }



/*:::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort after an error	 */
/*:::::::::::::::::::::::::::::::::::::::*/

#define	abortrd(msg) { \
 fprintf ( stderr, "read_img:  %s\n", msg ); \
 if (pixfile != NULL) { \
	fprintf ( stderr, "read_img:  Can't read image file %s\n", \
	((Yafile *) pixfile) -> actual_name); \
	close_yafile (pixfile); }\
 else \
	fprintf (stderr, "read_img:  Can't read image file %s\n", filename); \
 if (atblock != NULL) free(atblock); \
 if (cmblock != NULL) free(cmblock); \
 if (pdblock != NULL) free(pdblock); \
 if (asdata  != NULL) free(asdata); \
 return NULL; }



/*-------------------------------------------------*/
/*	If image type is any_image or iny_info	   */
/*	identify which type is available	   */
/*-------------------------------------------------*/

pixfile = NULL;

if ((imgtype == any_image) || (imgtype == any_info))
pixfile = (FILE *) open_yafile ( filename, "r", ".p" );
if ( pixfile != NULL )
   {
   if ( imgtype == any_image )
      imgtype = mapped_image;
   else
      imgtype = mapped_info;
   }
else
   {
   if ( imgtype == any_image )
      imgtype = rgb_image;
   else
      imgtype = rgb_info;
   }


/*-------------------------------------------------*/
/*	If image type refers to an RGB image,	   */
/*	invoke a separate subprogram to read	   */
/*	four different files.			   */
/*						   */
/*	Remaining code in this function handles	   */
/*      only color mapped image files.		   */
/*-------------------------------------------------*/

if ( (imgtype == rgb_image) || (imgtype == rgb_info) )
   return read_rgb ( filename, imgtype );	/* ... Farm it out	*/



/*--------------------------*/
/*	Open image file	    */
/*--------------------------*/

atblock = NULL;			/*  Preset pointers to allocated	*/
cmblock = NULL;			/*  memory blocks to NULL		*/
pdblock = NULL;			/*  (not allocated yet)			*/
asdata  = NULL;


if ( pixfile == NULL )		/*  If file wasn't opened by type check	*/
   pixfile = (FILE *) open_yafile ( filename, "r", ".p" ); /* do it now	*/
if ( pixfile == NULL )
   abortrd("Can't locate file")




/*--------------------------------------*/
/*	Read File Identification	*/
/*--------------------------------------*/

bc = fread ( file_id, 1, 8, pixfile );		/*  Read 8 bytes	*/
if ( bc < 8 )
   abortrd("Image file too short")

if (   (file_id[0] != 'S')		/*  Verify file type	*/
    || (file_id[1] != 'C')		/*  as "SCMI"		*/
    || (file_id[2] != 'M')
    || (file_id[3] != 'I') )
   abortrd("File isn't an image (SCMI) file")

cp = &file_id[4];		/*  Encode file format version #	*/
i4(version)

if ( version <= 0 )			/*  Verify valid version #	*/
   abortrd("Invalid file format version number")
if ( version > CUR_SCMI_VERSION )	/*  Verify known version	*/
   abortrd("File format version too high to handle")



/*------------------------------*/
/*	Read attribute block	*/
/*------------------------------*/

					/*  Read section prefix		*/
rsp('A','T',"Expected but didn't find attribute (AT) section")
bs = bc;				/*  Keep blocksize		*/

atblock = malloc (bs);			/*  Allocate space for attr blk	*/
if ( atblock == NULL )
   abortrd("Insufficient memory")

bc = fread ( atblock, 1, bs, pixfile );	/*  Read attribute block	*/
if ( bc < bs )
   abortrd("Image file too short")

cp = atblock;
i4(width)				/*  Encode image width		*/
i4(height)				/*  Encode image height		*/
i4(num_colors)				/*  Encode number of colors	*/
num_pixels = width * height;		/*  Infer number of pixels	*/
asdata_length = bs - 12;		/*  Derive assoc data length	*/

if ( asdata_length > 0 )		/*  If associated data exists	*/
   {					/*     copy to own mem block	*/
   Byte	*src, *dst;


   asdata = (Byte *) malloc (bs-12);	/*  Allocate memory		*/
   if ( asdata == NULL )
      abortrd("Insufficient memory")

   src = (Byte *) (&atblock[12]);
   dst = asdata;			/*  Copy associated data	*/
   bc  = asdata_length;

   while ( bc-- > 0 )
	 *dst++ = *src++;
   }

free (atblock);				/*  Attribute blk no longer needed  */
atblock = NULL;
 


/*--------------------------*/
/*	Read color map	    */
/*--------------------------*/

					/*  Read section prefix		*/
rsp('C','M',"Expected but didn't find color map (CM) section")
bs = bc;				/*  Keep blocksize		*/

cmblock = (Byte *) malloc (bs);		/*  Allocate space for map blk	*/
if ( cmblock == NULL )
   abortrd("Insufficient memory")

bc = fread ( cmblock, 1, bs, pixfile );	/*  Read color map block	*/
if ( bc < bs )
   abortrd("Image file too short")

colmap = (Col *) calloc (num_colors, sizeof(Col));
if ( colmap == NULL )
   abortrd("Insufficient memory")

bp = cmblock;
for ( i=0; i < num_colors; ++i )	/*  Set up actual color map	*/
    {
    colmap[i].pixel = i;
    colmap[i].red   = *bp++;
    colmap[i].green = *bp++;
    colmap[i].blue  = *bp++;
    }

free (cmblock);				/*  Release input block		*/
cmblock = (Byte *) colmap;		/*  Substitute actual map	*/



/*---------------------------*/
/*	Read pixel data	     */
/*---------------------------*/

if ( imgtype == mapped_image )		/*  If looking for actual image	*/
   {					/*     (not just info about it)	*/

					/*  Read section prefix		*/
   rsp('P','D',"Expected but didn't find pixel data (PD) section")
   bs = bc;				/*  Keep blocksize		*/

   pdblock = (Byte *) malloc (bs);	/*  Allocate space for pixels	*/
   if ( pdblock == NULL )
      abortrd("Insufficient memory")

   bc = fread ( pdblock, 1, bs, pixfile );	/*  Read pixel block	*/
   if ( bc < bs )
      abortrd("Image file too short")
   }



/*-----------------------------------------------------------*/
/*	Close file, create Img structure, and return it	     */
/*-----------------------------------------------------------*/

close_yafile (pixfile);

image = create_img ( imgtype, width, height, asdata_length, asdata,
			num_colors, cmblock, pdblock );

if ( image == NULL )
   abortrd("Couldn't create image")

if ( asdata != NULL )			/*  Mark blocks allocated	*/
   image -> alloc |= asdata_mem;	/*    to the image		*/
if ( cmblock != NULL )
   image -> alloc |= colmap_mem;
if ( pdblock != NULL )
   image -> alloc |= pixdata_mem;

return image;				/*  Return the image		*/
}



/*1
===============================================================================
=======                                                                 =======
=======		      write_img  --  Write Image to File		=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = write_img ( filename, image )


  Functions:

	Write an image file or files:

	    --	If the image is color-mapped, write a single file.
		If the given filename does not contain a '.', name
		the output file "filename.p.Z".

	    --  If the image is 32-bit RGB, write four files:

		filename.a	Attributes
		filename.r.Z	Red   component of pixel values
		filename.g.Z	Green component of pixel values
		filename.b.Z	Blue  component of pixel values

	Any file whose name ends with ".Z" is written in compressed
	format by piping its data through compress.


  Input:

	filename	char *
			Name of file to write.  Name can include
			references to home directories (~, ~username),
			environment variables ($whatnot) in addition
			to ordinary path info verbiage.

	image		Img *
			Pointer to image to write


  Output:

	status		int
			Status:  1 => completed successfully,
				 0 => couldn't write file

			Inability to write the file may be due to
			exhaustion of disk space.  It's been known
			to happen to folks with big bunches of images...
*/



int	write_img ( filename, image )

	char	*filename;		/*  Pointer to filename	*/
	Img	*image;			/*  Pointer to image	*/


{
	FILE	*pixfile;		/*  Output FILE pointer		*/

	char	*cp;			/*  Pointer to characters	*/
	Byte	*bp;			/*  Pointer to Bytes		*/
	Byte	*bp2;			/*  Another Byte pointer	*/
	int	 bc;			/*  Byte count			*/
	int	 i;			/*  Index for <whatnot>		*/
	int	 maxpix;		/*  Maximum legal pixel value	*/
	int	 secsize;		/*  Size of a section		*/

	char	 file_id[9];		/*  File identifier text	*/
	char	 prefix[11];		/*  Section prefix		*/
	char	*atblock;		/*  Attribute block		*/
	Byte	*cmblock;		/*  Color map block		*/



/*:::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to write section prefix	   */
/*:::::::::::::::::::::::::::::::::::::::::*/

#define wsp(s1,s2,n) { \
 cp=prefix; *cp++=s1; *cp++=s2; d8(n); \
 bc = fwrite ( prefix, 1, 10, pixfile ); \
 if (bc < 10) \
    abortwr("Write to file failed") }



/*::::::::::::::::::::::::::::::::::::::*/
/*	Macro to decode an integer	*/
/*	into 4-digit ascii		*/
/*::::::::::::::::::::::::::::::::::::::*/

#define d4(n) { \
 if (n>9999) abortwr("Excessive integer value") \
 sprintf (cp, "%4u", n); \
 cp += 4; }


/*::::::::::::::::::::::::::::::::::::::*/
/*	Macro to decode an integer	*/
/*	into 8-digit ascii		*/
/*::::::::::::::::::::::::::::::::::::::*/

#define d8(n) { \
 if (n>99999999) abortwr("Excessive integer value") \
 sprintf (cp, "%8u", n); \
 cp += 8; }





/*:::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort after an image	 */
/*	validity check fails		 */
/*:::::::::::::::::::::::::::::::::::::::*/

#define	wrabort(msg) { \
 fprintf ( stderr, "write_img,  Invalid image:  %s\n", msg ); \
 fprintf ( stderr, "write_img:  Can't write image file %s\n", filename ); \
 return 0; }



/*:::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort after an error	 */
/*:::::::::::::::::::::::::::::::::::::::*/

#define	abortwr(msg) { \
 fprintf ( stderr, "write_img:  %s\n", msg ); \
 if (pixfile != NULL) { \
	fprintf ( stderr, "write_img:  Can't write image file %s\n", \
	((Yafile *) pixfile) -> actual_name ); \
	close_yafile (pixfile); }\
 else \
	fprintf (stderr, "write_img:  Can't write image file %s\n",filename); \
 return 0; }




/*----------------------------------------------*/
/*	Check image for validity to avoid	*/
/*	writing outrageous atrocities		*/
/*----------------------------------------------*/

if (   (image->imgtype == mapped_info) || (image->imgtype == rgb_info)
    || (image->pixdata == NULL) || (image->num_pixels == 0) )
   wrabort("Has no pixels")

if ( (image->imgtype != mapped_image) && (image->imgtype != rgb_image) )
   wrabort("Unknown type")

if ( image->width < 0 )
   wrabort("Width is negative")
if ( image->width > 10000 )
   wrabort("Width is > 10,000")

if ( image->height < 0 )
   wrabort("Height is negative")
if ( image->height > 10000 )
   wrabort("Height is > 10,000")

if ( (image->width * image->height) != image->num_pixels )
   wrabort("Invalid image -- Number of pixels != height*width")

if ( image->asdata_length < 0 )
   wrabort("Associated data length is negative")
if ( (image->asdata_length != 0) && (image->asdata == NULL) )
   wrabort("Associated data length is nonzero, but none exists")

if ( image->imgtype == mapped_image )
   {
   if ( image->num_colors < 2 )
      wrabort("Fewer than 2 colors")
   if ( image->num_colors > 256 )
      wrabort("More than 256 colors")
   if ( image->colormap == NULL )
      wrabort("Has no color map")

#if	1		/*  Change to #if 0 if this proves too slow	*/

   /*	Check all pixel values for validity	*/

   bp = image->pixdata;
   i  = image->num_pixels;
   maxpix = image->num_colors - 1;
   while (i-- > 0)
	 if ( *bp++ > maxpix )
	    wrabort("At least 1 pixel's value exceeds max color map index")
#endif
   }




/*-------------------------------------------------*/
/*	If image type refers to an RGB image,	   */
/*	invoke a separate subprogram to write	   */
/*	four different files.			   */
/*						   */
/*	Remaining code in this function handles	   */
/*      only color mapped image files.		   */
/*-------------------------------------------------*/

if ( (image->imgtype == rgb_image) || (image->imgtype == rgb_info) )
   return write_rgb ( filename, image );	/* ... Farm it out	*/

pixfile = NULL;			/*  Indicate file not open yet	*/



/*------------------------------*/
/*	Open (pixmap) file	*/
/*------------------------------*/

cp = filename;			/*  Look for '.' in filename	*/
i  = 0;
while ( *cp != 0 )
      if ( *cp++ == '.' )
	 {
	 ++ i;
	 break;
	 }

if ( i )			/*  If filename contained '.'		*/
				/*     Open this name  			*/
   pixfile = (FILE *) open_yafile ( filename, "w", NULL );
else				/*  Else...				*/
   {				/*	Try to open "filename.p.Z"	*/
   char	*defaulted_name;

   defaulted_name = malloc ( strlen(filename) + 5 );
   if ( defaulted_name == NULL )
      abortwr("Insufficient memory")

   strcpy (defaulted_name, filename);	/*  Generate "fileaname.p.Z"	*/
   strcat (defaulted_name, ".p.Z");

				/*     Open this name  			*/
   pixfile = (FILE *) open_yafile ( defaulted_name, "w", NULL );
   if ( pixfile == NULL )	/*  If that failed, try original name	*/
      pixfile = (FILE *) open_yafile ( filename, "w", NULL );
   free (defaulted_name);
   }

if ( pixfile == NULL )
   abortwr("Can't open file -- Is path write protected or out of disk space?")




/*--------------------------------------*/
/*	Write File Identification	*/
/*--------------------------------------*/

cp = file_id;				/*  Generate file type	*/
*cp++ = 'S';
*cp++ = 'C';
*cp++ = 'M';
*cp++ = 'I';
d4(CUR_SCMI_VERSION)		/*  Append file format version #	*/

bc = fwrite ( file_id, 1, 8, pixfile );
if ( bc < 8 )
   abortwr("Write operation failed -- Out of disk space?")




/*--------------------------------*/
/*	Write attribute block	  */
/*--------------------------------*/

secsize = 12+image->asdata_length;
wsp('A','T',secsize)		/*  Write section prefix	*/

atblock = malloc (secsize);	/*  Allocate block for attributes	*/
if ( atblock == NULL )		/*	in file's format	*/
   abortwr("Insufficient memory")

cp = atblock;
d4(image->width)		/*  Format width	*/
d4(image->height)		/*  Format height	*/
d4(image->num_colors)		/*  Format color count	*/
bp2 = (Byte *) cp;

if ( image->asdata_length > 0 )	/*  If associated data exists	*/
   {				/*	copy it.		*/
   bp = image->asdata;
   i  = image->asdata_length;
   while ( i-- > 0 )
	 *bp2++ = *bp++;
   }

bc = fwrite ( atblock, 1, secsize, pixfile );
free(atblock);
if ( bc < secsize )
   abortwr("Write operation failed -- Out of disk space?")




/*---------------------------*/
/*	Write color map	     */
/*---------------------------*/

secsize = 3 * image->num_colors;
wsp('C','M',secsize)		/*  Write section prefix	*/


cmblock = (Byte *) malloc (secsize);	/*  Allocate block for color map*/
if ( cmblock == NULL )		/*	in file's format	*/
   abortwr("Insufficient memory")

bp = cmblock;			/*  Collect RGB values from color map	*/
for ( i=0; i < image->num_colors; ++i )
    {
    *bp++ = image->colormap[i].red;
    *bp++ = image->colormap[i].green;
    *bp++ = image->colormap[i].blue;
    }

bc = fwrite ( cmblock, 1, secsize, pixfile );
free(cmblock);
if ( bc < secsize )
   abortwr("Write operation failed -- Out of disk space?")



/*------------------------------*/
/*	 Write pixel data	*/
/*------------------------------*/

wsp('P','D',image->num_pixels)		/*  Write section prefix	*/

bc = fwrite ( image->pixdata, 1, image->num_pixels, pixfile );
if ( bc < image->num_pixels )
   abortwr("Write operation failed -- Out of disk space?")




/*-------------------------------------------*/
/*	Close file and return status 1	     */
/*-------------------------------------------*/

close_yafile (pixfile);
return 1;
}
