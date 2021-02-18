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
=======		   Module Imgcredel  --  Create/Delete Images		=======
=======			      File imgcredel.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	Create and delete images.  This includes the ability to
	create a new image by copying an existing image.

	All code here is part of the layer of image processing 
	functions which is independent of the underlying window
	system, graphics support layer, or operating system.


  Public Routines:

	create_img	Create image
	delete_img	Delete image
	copy_img	Copy image
	change_img	Change image type


  Local Routines:

	None


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	8-28-89		Paul Raveling
	Fixed bug in copy_img:  When going from mapped to RGB image,
	pixel data was converted to 0's due to necessary but omitted
	parentheses (operator precedence slipup).

	8-21-89		Paul Raveling
	Fixed potential bugs in create_img & copy_img, which hadn't
	initialized an image pointer before checking it in certain
	abort situations.

	3-16-89		Paul Raveling
	Created file, based partly on older utility definitions.
*/



#include <stdio.h>
#include <imglib/imglib.h>


extern	char	*malloc ();		/*  Allocate memory		*/
extern	char	*calloc ();		/*  Allocate memory differently	*/

#define	reg	register


/*1
===============================================================================
=======                                                                 =======
=======			  create_img  --  Create  Image			=======
=======                                                                 =======
===============================================================================

  Invocation:

	image = create_img ( imgtype, width, height, asdlen, asdata,
			     num_colors, colormap, pixdata )


  Functions:

	Create an image by allocating an Img structure and filling
	in fields with the given parameters.  The created image
	can be refer to either color-mapped 8-bit pixels or
	32-bit RGB (unmapped) pixels.  It can also omit pixel data  
	to supply only information concerning an image.

	32-bit pixels actually contain 24 bits of color, 8 bits each
	for red, green and blue components, and an 8-bit tag for use
	during various processing operations.

	RGB images are allowed to have color maps.  If present,
	such a map is for convenience in color-related processing
	functions; it is not used in displaying the image unless
	the system-dependent display function is such a processing
	function.

	If a pointer to any related block of data (associated data,
	color map, or pixel data) is NULL but the corresponding size
	or count parameter is nonzero, create_img allocates a
	block of memory of the required size.  Such blocks are
	"owned" by the image, and will be freed when delete_img
	deletes it.

	Blocks (associated data, color map, or pixel data) with
	non-NULL addresses and nonzero sizes are property of the
	caller; delete_img will NOT free them when it deletes
	the image.  If the caller wishes to transfer ownership of
	these blocks to the image, the calling routine must set
	the appropriate "alloc" bits in the returned Img -- See
	Img definition in imglib.h.

	Size of the pixel data is inferred to be width*height pixels.

	If color management functions will be used on the new image,
	it is necessary for the caller to invoke init_colman after
	ceate_img returns.


  Input:

	imgtype		int
			Type of image, as defined in imglib.h:

			  mapped_img	Color mapped
			  mapped_info	Color mapped, Info only:  No pixel data
			  rgb_img	32-bit RGB, unmapped
			  rgb_info	RGB, Info only:  No pixel data

	width		int
			Width of image in pixels

	height		int
			Height of image in pixels

	asdlen		int
			Length of associated data in bytes

	asdata		Byte *
			Pointer to associated data

	num_colors	int
			Number of colors in color map

	colormap	Col *
			Pointer to color map  (array of num_colors Col's)

	pixdata		Byte *
			Pointer to pixel data (array of width*height
			indices to color map)


  Output:

	image		Img *
	[return value]	Pointer to Img structure for new image;
			NULL => insufficient memory for image

*/


Img	*create_img ( imgtype, width, height, asdlen, asdata,
		      num_colors, colormap, pixdata )

	int	 imgtype;	/*  Image type			*/
	int	 width;		/*  Image width			*/
	int	 height;	/*  Image height		*/
	int	 asdlen;	/*  Length of associated data	*/
	Byte	*asdata;	/*  Ptr to associated data	*/
	int	 num_colors;	/*  Number of colors in map	*/
	Col	*colormap;	/*  Ptr to color map		*/
	Byte	*pixdata;	/*  Ptr to pixel data		*/

{
	Img	*image;		/*  Ptr to image structure	*/
	int	 i;		/*  Universal index		*/
	int	 num_pixels;	/*  Number of pixels		*/



/*	Macro to issue message and abort:	*/

#define	cri_abort(msg) { \
 fprintf(stderr,"create_img:  %s\n",msg); \
 if (image != NULL ) delete_img(image); \
 return NULL;}



/*----------------------------------------*/
/*	Check validity of parameters	  */
/*----------------------------------------*/

image = NULL;			/*  Preset to show no image yet	*/

if ( (imgtype < mapped_image) || (imgtype > rgb_info) )
   cri_abort("Image type is invalid")

if (   ((imgtype == mapped_info) || (imgtype == rgb_info))
    && (pixdata != NULL) )
   fprintf (stderr,
	"create_img:  Warning, pixel data given for info-only image\n" );


/*
	Other possibile errors are left unchecked:  This subprogram
	may be used to create a template which can be modified
	or completed in subsequent processing.
*/



/*----------------------------------------------------------*/
/*	Allocate image structure, fill in fixed fields	    */
/*----------------------------------------------------------*/

image = (Img *) malloc (sizeof(Img));
if ( image == NULL )
   cri_abort("Insufficient memory to create image")

image -> alloc  = 0;		/*  Nothing allocated to image yet	*/
image -> state  = 0;		/*  All state flags start at 0		*/
for ( i=0; i<8; ++i )		/*  No application or toolkit data yet	*/
    image -> aptdata[i] = NULL;

image -> imgtype    = imgtype;		/*  Set image type		*/
image -> width      = width;		/*  Set image width		*/
image -> height     = height;		/*  Set image height		*/
image -> num_pixels = width * height;	/*  Size = width * height	*/
num_pixels = image-> num_pixels;	/*  ,..keep for convience later	*/

image -> asdata     = NULL;
image -> colormap   = NULL;		/*  Preset pointers to ownable	*/
image -> pixdata    = NULL;		/*  data structures to NULL	*/
image -> sysres     = NULL;
image -> cmiq	    = NULL;



/*----------------------------------*/
/*	Set up associated data	    */
/*----------------------------------*/

image -> asdata_length = asdlen;	/*  Set size of associated data	*/
image -> asdata = asdata;		/*  Set ptr to associated data	*/
if ((asdlen != 0) && (asdata == NULL))	/*  If size != 0 but ptr is,	*/
   {					/*     allocate memory block	*/
   image -> asdata = (Byte *) malloc (asdlen);
   if ( image -> asdata == NULL )	/*  For insufficient memory,	*/
      cri_abort("Insufficient memory to create image")

   image -> alloc |= asdata_mem;	/*  Mark asdata owned by image	*/
   for ( i=0; i < asdlen; ++i )		/*  Preset asdata to 0's	*/
       image->asdata[i] = 0;
   }



/*----------------------------*/
/*	Set up color map      */
/*----------------------------*/

image -> num_colors = num_colors;	/*  Set # of mapped colors	*/
image -> colormap   = colormap;		/*  Set ptr to color map	*/
					/*  If appropriate...		*/
if (   (num_colors != 0) && (colormap == NULL)
    && ((imgtype == mapped_image) || (imgtype == mapped_info)) )
   {					/*     Allocate memory block	*/
   image -> colormap = (Col *) calloc (num_colors, sizeof(Col));
   if ( image -> colormap == NULL )	/*  For insufficient memory,	*/
      cri_abort("Insufficient memory to create image")

   image -> alloc |= colmap_mem;	/*  Mark map owned by image	*/
   for ( i=0; i < num_colors; ++i )	/*  Preset Colors to black	*/
       {
       image -> colormap[i].freq  = 0;	/*  ..Frequency       = 0	*/
       image -> colormap[i].pixel = i;	/*  ..Pixel value = map index	*/
       image -> colormap[i].red   = 0;	/*  ..Red   component = 0	*/
       image -> colormap[i].green = 0;	/*  ..Green component = 0	*/
       image -> colormap[i].blue  = 0;	/*  ..Blue  component = 0	*/
       }
  }



/*-----------------------------*/
/*	Set up pixel data      */
/*-----------------------------*/

image -> num_pixels = num_pixels;	/*  Set number of pixels	*/
image -> pixdata    = pixdata;		/*  Set ptr to pixel data	*/
					/*  If appropriate...		*/
if (   (num_pixels != 0) && (pixdata == NULL)
    && ((imgtype == mapped_image) || (imgtype == rgb_image)) )
   {					/*     Allocate memory block	*/
   if ( imgtype == mapped_image )
      image -> pixdata = (Byte *) malloc (num_pixels);
   else
      image -> pixdata =
		(Byte *) calloc (num_pixels, sizeof(Rgbpix));

   if ( image -> pixdata == NULL )	/*  For insufficient memory,	*/
      cri_abort("Insufficient memory to create image")

   image -> alloc |= pixdata_mem;	/*  Mark pixel data owned by img    */
					/*  Leave pixel data uninitialized  */
   }


return image;				/*  Return pointer to Img structure */
}



/*1
===============================================================================
=======                                                                 =======
=======			  delete_img  --  Delete Image			=======
=======                                                                 =======
===============================================================================

  Invocation:

	delete_img ( image )


  Functions:

	Delete an image.  Release the Img structure's memory and
	that of all subordinate structures "owned" by the image.



  Input:

	image		Img	*
			Pointer to Img structure for image to delete


  Output:	None
*/



void	 delete_img ( image )

	Img	*image;		/*  Ptr to image to delete	*/

{
	Mapinfo	*mi, *mn;	/*  Ptrs to color map info entries	*/



/*--------------------------------------------------*/
/*	Free memory for all "owned" structures,	    */
/*	then free Img structure			    */
/*--------------------------------------------------*/

if ( image -> alloc & asdata_mem )
   free (image -> asdata);
if ( image -> alloc & colmap_mem )
   free ( image -> colormap );
if ( image -> alloc & pixdata_mem )
   free ( image -> pixdata );
if ( image -> alloc & sys_resources )
   delete_sysres ( image );

mi = image -> cmiq;		/*  Free everything on color map queue	*/
while ( mi != NULL )
      {
      mn = mi -> next_map;
      free (mi);
      mi = mn;
      }

free (image);			/*  Free image structure itself		*/
}


/*1
===============================================================================
=======                                                                 =======
=======			  copy_img  --  Copy  Image			=======
=======                                                                 =======
===============================================================================

  Invocation:

	newimage = copy_img ( imgtype, oldimage )


  Functions:

	Create an image and copy an existing image to it.
	All data related to the old image is replicated in
	newly allocated memory for the new image, EXCEPT
	system-dependent resources.

	This can produce a new image of a different type than
	the old image.  If the image types are abbreviated to:

		cm	Color mapped image
		cmi	Color mapped image info (no pixel data)
		rgb	RGB unmapped image
		rgbi	RGB unmapped image info (no pixel data)

	Possible special actions are:

		Old	New	Special actions
		---	---	---------------

		cm	rgb	Color map is retained.  Pixel tags
				are set to the original pixel values
				(i.e., indices to color map).

		rgb	cm	Color map must be present in RGB image.
				Pixel tags must be indices to color map
				(i.e., new pixel values).

		cm	cmi \__/Pixel data is not copied --
		rgb	rgbi/  \New image's pixdata pointer is NULL.

		cm	rgbi	Color map is retained.

		rgb	cmi	Color map must be present in RGB image.

		cmi	cm \
		cmi	rgb \_/ New image has no pixel data,
		rgbi	cm  / \ even though its type indicates it does
		rgbi	rgb/


  Input:

	imgtype		int
			Type of new image, as defined in imglib.h:

			  mapped_img	Color mapped
			  mapped_info	Color mapped, Info only:  No pixel data
			  rgb_img	32-bit RGB, unmapped
			  rgb_info	RGB, Info only:  No pixel data

	oldimage	Img *
			Pointer to Img structure for old (source) image


  Output:

	newimage	Img *
	[return value]	Pointer to Img structure for new (destination) image;
			NULL => insufficient memory for image,
			or attempted to copy RGB image without
			a color map to a color-mapped image.
*/



Img	*copy_img ( imgtype, oldimage )

	int	 imgtype;	/*  New image type		*/
	Img	*oldimage;	/*  Ptr to old image structure	*/

{
	Img	*newimage;	/*  Ptr to new image structure	*/
	int	 i;		/*  Universal index		*/
	Byte	*srcb, *dstb;	/*  Ptrs for copying Bytes	*/
	Col	*srcc, *dstc;	/*  Ptrs for copying Colors	*/
	Rgbpix	*srcp, *dstp;	/*  Ptrs for copying RGB pixels	*/
	Byte	 r, g, b, t;	/*  RGB pixel components	*/


/*	Macro to issue message and abort:	*/

#define	cpi_abort(msg) { \
 fprintf(stderr,"copy_img:  %s\n",msg); \
 if (newimage != NULL ) delete_img(newimage); \
 return NULL;}



/*-------------------------------------------------*/
/*	Create new Img structure and force	   */
/*	allocation of whatever memory blocks	   */
/*	new image will need			   */
/*-------------------------------------------------*/

newimage = NULL;		/*  Preset to show no new image yet	*/

if (   ((imgtype == mapped_image) || (imgtype == mapped_info) )
    && ((oldimage->imgtype == rgb_image) || (oldimage->imgtype == rgb_info))
    && ((oldimage->colormap == NULL) || (oldimage -> num_colors == 0)) )
   cpi_abort("Color map is needed to copy from RGB to mapped image")


if ( (imgtype == mapped_image) || (imgtype == rgb_image) )
   newimage = create_img ( imgtype, oldimage->width, oldimage->height,
			oldimage->asdata_length, NULL,
			oldimage->num_colors, NULL,
			NULL );
else				/*  Info only:  Create no pixel data	*/
   {
   newimage = create_img ( imgtype, oldimage->width, oldimage->height,
			oldimage->asdata_length, NULL,
			oldimage->num_colors, NULL,
			1 );
   newimage->pixdata = NULL;
   }

if ( newimage == NULL )		/*  Return NULL if create_img failed	*/
   return NULL;



/*-----------------------------------------*/
/*	Copy associated data, if any	   */
/*-----------------------------------------*/

if (   (newimage->asdata_length != 0)	/* ... Using lots of caution	*/
    && (newimage->asdata != NULL)
    && (oldimage->asdata != NULL) )
   {
   i    = newimage->asdata_length;
   srcb = oldimage->asdata;
   dstb = newimage->asdata;

   while ( i-- > 0 )
	 *dstb++ = *srcb++;
   }



/*----------------------------------*/
/*	Copy color map, if any	    */
/*----------------------------------*/

if (   (newimage->num_colors != 0)	/* ... Using lots of caution	*/
    && (newimage->colormap != NULL)
    && (oldimage->colormap != NULL) )
   {
   i    = newimage->num_colors;
   srcc = oldimage->colormap;
   dstc = newimage->colormap;

   while ( i-- > 0 )
	 *dstc++ = *srcc++;
   }



/*----------------------------------*/
/*	Copy pixel data, if any	    */
/*----------------------------------*/

/*
	This code doesn't presently check for invalid pixel values
	(e.g., pixel value >= num_colors).
*/

if (   (newimage->num_pixels == 0)	/* ... Using lots of caution	*/
    || (newimage->pixdata == NULL)
    || (oldimage->pixdata == NULL) )
   return newimage;

i = newimage->num_pixels;		/*  Preset pixel counter for copies  */

switch ( oldimage->imgtype )
    {
    case mapped_image:
	switch ( newimage->imgtype )
	    {				/*--------------------------------*/
	    case mapped_image:		/*  mapped_image -> mapped_image  */
					/*--------------------------------*/

		srcb = oldimage->pixdata;
		dstb = newimage->pixdata;

		while ( i-- > 0 )
		      *dstb++ = *srcb++;

		break;

					/*------------------------------*/
	    case rgb_image:		/*  mapped_image -> rgb_image	*/
					/*------------------------------*/

		srcb = oldimage->pixdata;
		dstp = (Rgbpix *) newimage->pixdata;

		while ( i-- > 0 )
		      {
		      r = oldimage->colormap[*srcb].red;
		      g = oldimage->colormap[*srcb].green;
		      b = oldimage->colormap[*srcb].blue;
		      t = *srcb++;
		      *dstp++ =   (t << pixel_tag_shift)
				+ (r << pixel_red_shift)
				+ (g << pixel_green_shift)
				+ (b << pixel_blue_shift);
		      }
		break;

	    default:	break;
	    }
	break;

    case rgb_image:
	switch ( newimage->imgtype )
	    {				/*------------------------------*/
	    case mapped_image:		/*  rgb_image -> mapped_image	*/
					/*------------------------------*/

		srcp = (Rgbpix *) oldimage->pixdata;
		dstb = newimage->pixdata;

		while ( i-- > 0 )
		      *dstb++ = (*srcp++ & pixel_tag) >> pixel_tag_shift;
		break;

					/*--------------------------*/
	    case rgb_image:		/*  rgb_image -> rgb_image  */
					/*--------------------------*/

		srcp = (Rgbpix *) oldimage->pixdata;
		dstp = (Rgbpix *) newimage->pixdata;

		while ( i-- > 0 )
		      *dstp++ = *srcp++;
		break;

	    default:	break;
	    }
	break;

    default:	break;
    }

return newimage;
}


/*1
===============================================================================
=======                                                                 =======
=======			  change_img  --  Change  Image			=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = change_img ( imgtype, image )


  Functions:

	Change an existing image from one type to another.
	This normally is used to change image data between
	color mapped and RGB formats.

	CAUTION:  change_img does not touch system-dependent
	data associated with the image.  Do this to an image
	that's currently displayed and you may be shocked.

	If the image types are abbreviated to:

		cm	Color mapped image
		cmi	Color mapped image info (no pixel data)
		rgb	RGB unmapped image
		rgbi	RGB unmapped image info (no pixel data)

	Possible special actions are:

		Old	New	Special actions
		---	---	---------------

		cm	rgb	Color map is retained.  Pixel tags
				are set to the original pixel values
				(i.e., indices to color map).

		rgb	cm	Color map must be present in RGB image.
				Pixel tags must be indices to color map
				(i.e., new pixel values).

		cm	cmi \
		cm	rgbi \_/ Pixel data
		rgb	cmi  / \ is discarded
		rgb	rgbi/


		cm	rgbi	Color map is retained.

		rgb	cmi	Color map must be present in RGB image.

		cmi	cm \
		cmi	rgb \_/ New image has no pixel data,
		rgbi	cm  / \ even though its type indicates it does
		rgbi	rgb/


  Input:

	imgtype		int
			Type of new image, as defined in imglib.h:

			  mapped_img	Color mapped
			  mapped_info	Color mapped, Info only:  No pixel data
			  rgb_img	32-bit RGB, unmapped
			  rgb_info	RGB, Info only:  No pixel data

	image		Img *
			Pointer to Img structure for image


  Output:

	status		int
	[return value]	Status code:
			0 => insufficient memory for image,
			or attempted to change RGB image without
			a color map to a color-mapped image.
*/



int	 change_img ( imgtype, image )

	int	 imgtype;	/*  New image type		*/
	Img	*image;	/*  Ptr to old image structure	*/

{
reg	int	 i;		/*  Universal index		*/
	Byte	*srcb, *dstb;	/*  Ptrs for copying Bytes	*/
	Col	*srcc, *dstc;	/*  Ptrs for copying Colors	*/
	Rgbpix	*srcp, *dstp;	/*  Ptrs for copying RGB pixels	*/
	Byte	*old_mapped;	/*  Ptr to old color mapped data */
	Rgbpix	*old_rgb;	/*  Ptr to old RGB data		*/
reg	Byte	 r, g, b, t;	/*  RGB pixel components	*/


/*	Macro to issue message and abort:	*/

#define	chi_abort(msg) { \
 fprintf(stderr,"change_img:  %s\n",msg); \
 return 0;}



/*-------------------------------------------------*/
/*	Create new Img structure and force	   */
/*	allocation of whatever memory blocks	   */
/*	new image will need			   */
/*-------------------------------------------------*/

if (   ((imgtype == mapped_image) || (imgtype == mapped_info) )
    && ((image->imgtype == rgb_image) || (image->imgtype == rgb_info))
    && ((image->colormap == NULL) || (image -> num_colors == 0)) )
   chi_abort("Color map is needed to change from RGB to mapped image")



/*----------------------------------*/
/*	Copy pixel data, if any	    */
/*----------------------------------*/

i = image->num_pixels;			/*  Preset pixel counter for copies  */

switch ( image->imgtype )
    {
    case mapped_image:
	switch ( imgtype )
	    {				/*--------------------------------*/
	    case mapped_image:		/*  mapped_image -> mapped_image  */
					/*--------------------------------*/
		/*  Pretty silly */
		break;

					/*------------------------------*/
	    case rgb_image:		/*  mapped_image -> rgb_image	*/
					/*------------------------------*/

		old_mapped = image->pixdata;
		srcb = image->pixdata;
		image->pixdata = (Byte *) calloc
				 ( image->num_pixels, sizeof(Rgbpix) );
		if ( image->pixdata == NULL )
		   {
		   image->pixdata = old_mapped;
		   chi_abort("Insufficient memory to copy data")
		   }
		dstp = (Rgbpix *) image->pixdata;

		while ( i-- > 0 )
		      {
		      r = image->colormap[*srcb].red;
		      g = image->colormap[*srcb].green;
		      b = image->colormap[*srcb].blue;
		      t = *srcb++;
		      *dstp++ =   (t << pixel_tag_shift)
				+ (r << pixel_red_shift)
				+ (g << pixel_green_shift)
				+ (b << pixel_blue_shift);
		      }

		image -> imgtype = imgtype;
		if ( image->alloc & pixdata_mem )
		   free (old_mapped);
		image->alloc |= pixdata_mem;
		break;


					/*------------------------------*/
	    case mapped_info:		/*  mapped_image -> mapped_info	*/
	    case rgb_info:		/*  mapped_image -> rgb_info	*/
					/*------------------------------*/

		image -> imgtype = imgtype;
		if ( image->alloc & pixdata_mem )
		   free (image->pixdata);
		image->alloc &= ~pixdata_mem;
		image->pixdata = NULL;

	    default:	break;
	    }
	break;

    case rgb_image:
	switch ( imgtype )
	    {				/*------------------------------*/
	    case mapped_image:		/*  rgb_image -> mapped_image	*/
					/*------------------------------*/

		old_rgb = (Rgbpix *) image->pixdata;
		srcp = (Rgbpix *) image->pixdata;
		image->pixdata = (Byte *) malloc ( image->num_pixels );
		if ( image->pixdata == NULL )
		   {
		   image->pixdata = (Byte *) old_rgb;
		   chi_abort("Insufficient memory to copy data")
		   }
		dstb = image->pixdata;

		while ( i-- > 0 )
		      *dstb++ = (*srcp++ & pixel_tag) >> pixel_tag_shift;

		image -> imgtype = imgtype;
		if ( image->alloc & pixdata_mem )
		   free (old_rgb);
		image->alloc |= pixdata_mem;
		break;

					/*--------------------------*/
	    case rgb_image:		/*  rgb_image -> rgb_image  */
					/*--------------------------*/
		/*  Pretty silly */
		break;


					/*------------------------------*/
	    case mapped_info:		/*  rgb_image -> mapped_info	*/
	    case rgb_info:		/*  rgb_image -> rgb_info	*/
					/*------------------------------*/

		image -> imgtype = imgtype;
		if ( image->alloc & pixdata_mem )
		   free (image->pixdata);
		image->alloc |= pixdata_mem;
		image->pixdata = NULL;
		break;
	    }
	break;

    default:			/*  Forget about info -> noninfo	*/
	break;
    }

return 1;
}
