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
=======		    Module imgXlib  --  X11 image interface		=======
=======				File imgXlib.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	This module is the X11 interface for imglib functions.

	Unlike other imglib functions, the routines in this module
	have calling sequences which depend on an X11 environment.


  X resources used:

	ColorMap:	Private or Shared
	ColorInstance:	Private or Shared

	In both cases 'p', 'P', 's', or 'S' as the initial character
	of the resource or XDefault value is sufficient.


	ColorMap = Private forces allocation and installation
			of a private colormap for the window
			displaying an image.

	ColorMap = Shared uses the default colormap for the
			X11 environment's display.


	ColorInstance = Private uses XAllocColorCells to allocate colormap
			cells exclusively for the image being displayed.

	ColorInstance = Shared uses XAllocColor to make the window
			displaying an image share existing read-only
			colors.


  Public Routines:

	ck_colorcap	Check read/write color capacity
	create_sysres	Create system-dependent resources
	delete_sysres	Delete system-dependent resources
	display_img	Display image in new window
	get_img		Get image	(Copy from drawable)
	maint_img	Maintain Displayed Image
	put_img		Put image	(Copy to drawable)
	start_X11	Start X11 Interface
	stop_X11	Stop X11 Interface
	update_colors	Update X11 color map


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

	3-21-89		Paul Raveling
	Created module.
*/



#include <stdio.h>
#include <imglib/imglib.h>	/*  Include sys-independent defs	*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>		/*  Include X11 definitions		*/
#include <X11/Xos.h>

#include <imglib/imgXlib.h>	/*  Include sys-dependent defs for X11	*/



extern	char	*malloc ();		/*  Allocate memory		*/
extern	char	*calloc ();		/*  Allocate memory differently	*/



/*1
===============================================================================
=======                                                                 =======
=======		ck_colorcap  --  Check Read/Write Color Capacity	=======
=======                                                                 =======
===============================================================================

  Invocation:

	num_colors = ck_colorcap ( X11_env )


  Functions:

	Check the number of read/write colors available in the given
	X11 display environment.  If the X11 environment includes
	a smaller "maxcolors" override, use the override.

	Since there's no X11 function to do this directly,  ck_colorcap
	uses iterative attempts to allocate read/write cells.  It
	uses a binary search to minimize the overhead involved.


  Input:

	X11_env		X11env *
			Pointer to X11 interface environment
			(e.g., as created by start_X11)


  Output:

	num_colors	int
	<return value>	Number of read/write color cells available.
*/



int	 ck_colorcap ( X11_env )

	X11env	*X11_env;		/*  X11 interface environment	*/

{
	int	 num_colors;		/*  Number of colors		*/
	int	 min_num;		/*  Minimum # possible		*/
	int	 max_num;		/*  Maximum # possible		*/

	Word	 pixels[256];		/*  XAllocColorCells needs this	*/
	Word	 plane_masks[8];	/*  XAllocColorCells needs this	*/
	int	 status;		/*  Status returned by XAlloc...*/

#ifndef	FALSE
#define	FALSE	0			/*  Boolean FALSE		*/
#endif



/*----------------------------------------------*/
/*	Just start (grumble) searching...	*/
/*----------------------------------------------*/

min_num    = 0;				/*  Min possible = 0 colors	*/
max_num	   = 255;			/*  Max possible = 255 colors	*/
if ( (X11_env->visual)->map_entries < max_num )	/*  or # colors in map	*/
   max_num = (X11_env->visual)->map_entries;

num_colors = (min_num + max_num + 1) / 2;  /*  1st guess is midrange	*/

while ( min_num < max_num )		/*  While possible min < max	*/
      {
      if ( num_colors != 0 )
         status = XAllocColorCells ( X11_env->display, X11_env->cmap,
			FALSE, plane_masks, 0, pixels, num_colors );
      else
	 status = 1;		/*  You can ALWAYS allocate 0 colors	*/

      if ( status )			/*  If request succeeded	*/
	 {				/*     Free alloc'd cells	*/
	 XFreeColors ( X11_env->display, X11_env->cmap,
			pixels, num_colors, 0 );
	 min_num = num_colors;		/*  	Move min_num up		*/
	 }
      else				/*  Else (request failed)	*/
	 max_num = num_colors - 1;	/*	Move max_num down	*/

      /*	Next trial count is middle of new range, rounded	*/

      num_colors = (min_num + max_num + 1) / 2;
      }

if ( X11_env -> maxcolors < num_colors )
   num_colors = X11_env -> maxcolors;

return num_colors;
}

		

/*1
===============================================================================
=======                                                                 =======
=======	     create_sysres  --  Create System-Dependent Resources	=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = create_sysres ( image, X11_env )


  Functions:

	Create system-dependent resources required to display or
	otherwise manipulate an image.  This also binds the image
	to a particular X11 environment.

	Normally this would be called by a system-dependent function
	which needs such resources but finds they don't exist yet
	for this image.


  Input:

	image		Img *
			Pointer to image being initialized.

	X11_env		X11env *
			Pointer to X11 environment for image


  Output:

	status		int
	<return value>	Success/fail status:
			    0 => Couldn't create some resource
			    1 => Resources were created
*/



int	 create_sysres ( image, X11_env )

	Img	*image;			/*  Pointer to image		*/
	X11env	*X11_env;		/*  Pointer to X11 environment	*/

{
	Sir	*sir;		/*  Ptr to system image resources	*/


/*----------------------------------------------------------------*/
/*	Allocate and initialize System Image Resources block	  */
/*----------------------------------------------------------------*/

sir = (Sir *) malloc (sizeof(Sir));
if ( sir == NULL )
   {
   fprintf (stderr,"create_sysres:  Insufficent memory for image resources\n");
   return 0;
   }

image->sysres = (char *) sir;		/*  Link sys resources to image	  */
image->alloc |= sys_resources;		/*  Mark "sys resources alloc'd"  */

sir -> X11_env    = X11_env;		/*  Save environment pointer	*/
sir -> num_colors = 0;			/*  0 X11 pixels allocated	*/
					/*  Leave pixels uninitialized	*/
sir -> pix = NULL;			/*  No pixmap yet		*/
sir -> win = NULL;			/*  No window yet		*/

return 1;
}
		

/*1
===============================================================================
=======                                                                 =======
=======	      delete_sysres  --  Delete System-Dependent Resources	=======
=======                                                                 =======
===============================================================================

  Invocation:

	delete_sysres ( image )


  Functions:

	Free whatever system-dependent resources are associated with
	the given image.


  Input:

	image		Img *
			Pointer to image being cleaned up


  Output:

	None.  Any system resources (colors, pixmaps, windows,...)
	associated with this image are freed, and associated
	blocks of memory are released.
*/



void	 delete_sysres ( image )

	Img	*image;			/*  Pointer to image		*/

{
	Sir	*sir;		/*  Ptr to system image resources	*/
	Display	*disp;		/*  Ptr to associated display		*/



/*----------------------------------------------*/
/*	Release any owned X11 resources		*/
/*	and the System Interface Resources	*/
/*	info block				*/
/*----------------------------------------------*/

if ( (image->alloc & sys_resources) == 0 )	/*  If no resources	*/
   return;					/*     do nothing	*/

sir  = (Sir *) image->sysres;		/*  Point to resource block	*/
disp = (sir->X11_env)->display;		/*  Point to display		*/

if ( sir->win != NULL )			/*  If a window exists		*/
   XDestroyWindow ( disp, sir->win );	/*	free it			*/

if ( sir->pix != NULL )			/*  If a pixmap exists		*/
   XDestroyWindow ( disp, sir->win );	/*	free it			*/

if ( sir->num_colors != 0 )		/*  If colors are owned		*/
   XFreeColors ( disp,			/*	free them		*/
		(sir->X11_env) -> cmap,
		sir -> pixels,
		sir -> num_colors,
		0 );

image -> alloc &= ~sys_resources;	/*  Indicate no sys resources	*/
free ( sir );				/*  associated with image and	*/
image -> sysres = NULL;			/*  free Sir			*/
}


/*1
===============================================================================
=======                                                                 =======
=======		   display_img --  Display Image in New Window		=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = display_img ( image, display_name, event_mask )


  Functions:

	Create a new pixmap and window, and display an image in it.


  Input:

	image		Img *
			Image to display


	X11_env		X11env *
			Pointer to X11 environment for image

	event_mask	Word
			X11 event mask for event for events of interest
			on image's window.  This function forces
			ExposureMask on in the event mask.


  Output:

	status		int
	<return value>	Status:  0 => failed, 1 => succeeded
*/



int	display_img ( image, X11_env, event_mask )

	Img	*image;			/*  Image to display		*/
	X11env	*X11_env;		/*  Pointer to X11 environment	*/
	Word	 event_mask;		/*  Event mask for img's window	*/

{
	Geom	 wingeom;		/*  Window's geometry		*/
	Geom	 pixgeom;		/*  Pixmap's geometry		*/
	Sir	*sir;			/*  System interface resources	*/

	Window	 win;			/*  Window for image		*/
	Pixmap	 pix;			/*  Pixmap for image		*/
	int	 status;		/*  Returned status		*/

    XSizeHints	 bull;			/*  X malarky			*/




/*------------------------------------------------*/
/*	Initialize resources and geometries	  */
/*------------------------------------------------*/

if ( image->alloc & sys_resources )	/*  If image has sys resources	*/
   delete_sysres (image);		/*     free old ones		*/

status = create_sysres (image, X11_env);/*  Create sys resources	*/
if ( status == 0 )
   {
   fprintf ( stderr, "display_img:  Couldn't allocate X resources\n" );
   return 0;
   }
sir = (Sir *) image -> sysres;		/*  Keep pointer to sys resources  */

wingeom = X11_env -> geometry;
if ( (X11_env -> geommask & WidthValue) == 0 )
   wingeom.width  = image->width;
if ( (X11_env -> geommask & HeightValue) == 0 )
   wingeom.height = image->height;
if ( X11_env -> geommask & XNegative )
   wingeom.xoff = X11_env -> width  - wingeom.xoff - wingeom.width;
if ( X11_env -> geommask & YNegative )
   wingeom.yoff = X11_env -> height - wingeom.yoff - wingeom.height;

pixgeom.xoff   = 0;
pixgeom.yoff   = 0;			/*  Set pixmap geometry		*/
pixgeom.width  = image->width;
pixgeom.height = image->height;


/*--------------------------------------*/
/*	Create resources for image	*/
/*--------------------------------------*/

sir -> win = XCreateSimpleWindow ( X11_env->display, X11_env->rootwin,
	wingeom.xoff, wingeom.yoff, wingeom.width, wingeom.height, 2,
	BlackPixel(X11_env->display,X11_env->screen),
	BlackPixel(X11_env->display,X11_env->screen) );
if ( sir->win == NULL )
   {
   fprintf ( stderr, "display_img:  Couldn't create X window\n" );
   return 0;
   }

sir -> pix = XCreatePixmap ( X11_env->display, sir->win,
	 image->width, image->height, X11_env->depth );
if ( sir->pix == NULL )
   {
   fprintf ( stderr, "display_img:  Couldn't create X pixmap\n" );
   return 0;
   }

put_img ( image, sir->pix, &pixgeom, X11_env );	/*  Put image to pixmap	*/

/*	Override the bleepin' window manager by setting hints	*/

if ( ((X11_env -> geommask) & (XValue | YValue)) != 0 )
   bull.flags = USPosition | USSize;
else
   bull.flags = USSize;
bull.x      = wingeom.xoff;
bull.y      = wingeom.yoff;
bull.width  = wingeom.width;
bull.height = wingeom.height;
XSetNormalHints ( X11_env->display, sir->win, &bull );

XMapWindow ( X11_env->display, sir->win );	/*  Map the window	*/

XSelectInput ( X11_env->display, sir->win, event_mask | ExposureMask );
return 1;

}


/*1
===============================================================================
=======                                                                 =======
=======		     get_img --  Get Image from Drawable		=======
=======                                                                 =======
===============================================================================

  Invocation:

	image = get_img ( drawable, geometry, X11_env )


  Functions:

	Copy a region of a drawable to a new image.

	Restrictions:

	    1.  If the drawable is a pixmap rather than a window,
		the color map could be wrong.  We assume that a
		pixmap uses the default color map for the given
		X11 environment.  If the caller knows for sure what
		color map is appropriate, it can supply an X11_env
		structure with the appropriate cmap setting.

	    2.  This currently doesn't work if the default color map
		uses more than 256 colors (i.e., is more than 8 bits
		deep), or if XGetImage reports:

		    --  Image depth > than 8
		    --  Pixel size other than 8 bits
		    --  RGB mask values other than 0


  Input:

	drawable	Drawable
			Window or pixmap to get image from:
			X11 Drawable (XID)

	geometry	Geom *
			Pointer to imglib Geom structure representing
			geometry of region to get from drawable;
			NULL => get entire contents of drawable

	X11_env		X11env *
			Pointer to X11 interface environment
			(e.g., as created by start_X11)


  Output:

	image		Img *
	<return value>	Pointer to new image;
			NULL => couldn't get X11 image, or couldn't
			create a new imglib Img (probably insufficent
			memory)
*/


Img	*get_img ( drawable, geometry, X11_env )

	Drawable drawable;		/*  Drawable to get image from	*/
	Geom	*geometry;		/*  Geometry of region to get	*/
	X11env	*X11_env;		/*  X11 interface environment	*/

{
	Img	*image;			/*  Ptr to Img structure	*/
	XImage	*X11_image;		/*  Image as X11 image		*/
	XColor	 X11_map[256];		/*  X11 color map		*/
	int	 X11_numcolors;		/*  Size of X11 color map	*/
	Geom	 default_geometry;	/*  Default geometry		*/
	Geom	*region;		/*  Geometry of region to get	*/
	int	 datasize;		/*  # of pixels:  width*height	*/
	int	 num_colors;		/*  Size of image's color map	*/
	Col	 colormap[256];		/*  Prototype color map		*/
	Col	*cp;			/*  Pointer to a color (Col)	*/
	XColor	*Xcp;			/*  Pointer to an X11 color (XColor) */
	Byte	*psp;			/*  Pixel source pointer	*/
	Byte	*pdp;			/*  Pixel destination pointer	*/
	int	 npi;			/*  Next pixel index to assign	*/
	int	 i;			/*  Universal index		*/
	int	 bol;			/*  Byte counter within a line	*/
	int	 bpl;			/*  Bytes of data per line	*/
 XWindowAttributes  window_atts;	/*  Window attributes		*/
	int	 status;		/*  Status returned by function	*/



/*::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort with message	*/
/*::::::::::::::::::::::::::::::::::::::*/

#define abort_gi(msg) {\
 fprintf (stderr,"get_img:  %s\n",msg); \
 if (X11_image != NULL) XDestroyImage(X11_image); \
 return NULL; }




/*-----------------------------------------------------------*/
/*	If geometry is omitted, get drawable's geometry	     */
/*-----------------------------------------------------------*/

X11_image = NULL;			/*  Preset to show no image yet	*/
if ( geometry != NULL )			/*  If caller gave a geometry	*/
   region = geometry;			/*     use it			*/
else					/*  else			*/
   {					/*     Get drawable's geometry	*/
   int		 w, h, x, y, bw, d;
   Window	 root;
   int		 status;

   status = XGetGeometry (X11_env->display, drawable, &root,
			  &x, &y, &w, &h, &bw, &d);
   default_geometry.width  = w;
   default_geometry.height = h;
   default_geometry.xoff   = 0;
   default_geometry.yoff   = 0;
   region = &default_geometry;
   }

/*	Derive pixel data size in bytes from region's width*height	*/

datasize = region->width * region->height;



/*------------------------------------------------------*/
/*	Get image and default color map from X11	*/
/*------------------------------------------------------*/

X11_image = XGetImage (X11_env->display, drawable,
		region->xoff, region->yoff, region->width, region->height,
		AllPlanes, ZPixmap );
if ( X11_image == NULL )
   abort_gi("Couldn't get X11 image")

if ( X11_image->depth == 24 )
   {			/*  Looks like TrueColor or DirectColor	*/
   if (   ((X11_env->visual) -> red_mask   != pixel_red)
       || ((X11_env->visual) -> green_mask != pixel_green)
       || ((X11_env->visual) -> blue_mask  != pixel_blue) )
      abort_gi("Can't handle 24-bit-deep image due to unusual pixel masks")

   image = create_img ( rgb_image, region->width, region->height,
			0, NULL, 0, NULL,  X11_image->data );
   if ( image == NULL )
      abort_gi("Couldn't create image")

   image -> alloc |= pixdata_mem;	/*  Give pixel data to image	*/
   free (X11_image);
   return image;
   }

if ( X11_image->depth > 8 )
   abort_gi("Can't handle images with depth > 8")

if ( X11_image->bits_per_pixel != 8 )
   abort_gi("Can't handle images with other than 8 bits per pixel")


status = XGetWindowAttributes ( X11_env->display, drawable, &window_atts );
if ( status == 0 )		/*  If drawable isn't a window	*/
   {				/*	use default color map	*/
   X11_numcolors = (X11_env->visual)->map_entries;
   if ( X11_numcolors > 256 )
      abort_gi("Can't handle visuals with more than 256 colors")

   for ( i=0; i < X11_numcolors; ++i )
       X11_map[i].pixel = i;

   XQueryColors ( X11_env->display, X11_env->cmap, X11_map, X11_numcolors );
   }
else				/*  else			*/
   {				/*	use window's color map	*/
   X11_numcolors = (window_atts.visual)->map_entries;
   if ( X11_numcolors > 256 )
      abort_gi("Can't handle visuals with more than 256 colors")

   for ( i=0; i < X11_numcolors; ++i )
       X11_map[i].pixel = i;

   XQueryColors (X11_env->display, window_atts.colormap, X11_map, X11_numcolors);
   }




/*------------------------------------------------------*/
/*	Translate pixels to imglib color indices	*/
/*	& collect info for the image's color map	*/
/*------------------------------------------------------*/

/*
	Scan image to build image color map and reassign pixel values.
	The imglib image will use consecutive pixel indices from
	0 through n-1, where n is the number of colors the image uses
	and isn't known yet.

	This is a bit weird because some X11 images have more bytes
	per line than pixels per line, even in 8-bit Z format.
*/

for ( i=0; i < X11_numcolors; ++i )	/*  Preset pixel frequencies	*/
    colormap[i].freq = 0;		/*	to 0			*/

npi = 0;				/*  Next pixel index to assign = 0  */
psp = (Byte *) X11_image -> data;	/*  Point to start of pixel data    */
pdp = psp;				/*  For both source & destination   */
i   = datasize;				/*  Count pixels to process	*/
bol = 0;				/*  Count bytes on each line	*/
bpl = X11_image -> bytes_per_line;	/*  in case data width > image width */

while ( i-- > 0 )			/*  For each pixel...		*/
      {
      cp = &colormap[*psp];		/*  Point to imglib map entry	*/

      if ( cp -> freq == 0 )		/*  If 1st appearance of pixel	*/
	 {				/*     init color map entry	*/
	 Xcp = &X11_map[*psp];		/*    and X11 map entry		*/
	 cp -> pixel = npi++;		/*  Set an imglib pixel value	*/
	 cp -> red   = Xcp->red   >> 8;
	 cp -> green = Xcp->green >> 8;	/*  Set color values		*/
	 cp -> blue  = Xcp->blue  >> 8;
	 }

      ++ cp -> freq;		/*  Count the pixel			*/
      *pdp++ = cp -> pixel;	/*  Map pixel to imglib color map index	*/
      ++ psp;			/*  Advance to next source pixel	*/
      if ( ++bol >= region->width )	/*  If a line's been processed	*/
	 {			/*     skip to start of next line	*/
	 while ( bol++ < bpl )
	       ++ psp;
	 bol = 0;
	 }
      }

num_colors = npi;		/*  # colors used = "next" pixel index	*/




/*------------------------------------------------------*/
/*	Create imglib image & generate its color map	*/
/*------------------------------------------------------*/

image = create_img ( mapped_image, region->width, region->height,
			0, NULL, num_colors, NULL,  X11_image->data );
if ( image == NULL )
   abort_gi("Couldn't create image")

image -> alloc |= pixdata_mem;		/*  Give pixel data to image	*/


/*	Condense color map to exclude X11 colors not used by image	*/

npi = 0;			/*  Next pixel index = 0		*/
cp  = image -> colormap;	/*  Point to map alloc'd by create_img	*/

for ( i=0; i < X11_numcolors; ++i )	/*  Copy only colors used	*/
    if ( colormap[i].freq != 0 )	/*  in the image		*/
       image->colormap[colormap[i].pixel] = colormap[i];


/*
	Free the X11 XImage,  but don't XDestroyImage it:
	XDestroyImage frees its data,  but we've adopted its pixel data
	for the imglib image.
*/

free (X11_image);

return image;			/*  Return the image	*/
}



/*1
===============================================================================
=======                                                                 =======
=======		     maint_img  --  Maintain Displayed Image		=======
=======                                                                 =======
===============================================================================

  Invocation:

	X11_event = maint_img ( image, X11_env )


  Functions:

	Maintain a displayed image.

	This means waiting for an X event and handling refreshing.
	The caller is responsible for having selected event input
	appropriately, and for including ExposreMask in the event
	mask given to X.

	Return to the caller to let him handle any X event other
	than an XExposeEvent.  Handle exposures within this routine.


  Input:

	image		Img *
			Pointer to image being displayed.

	X11_env		X11env *
			Pointer to X11 interface environment
			(e.g., as created by start_X11)

  Output:

	X11_event	XEvent *
	<return value>	Pointer to last event reported by X11
			that's of interest to the caller.
*/


XEvent *maint_img ( image, X11_env )

	Img	*image;		/*  Image to maintain		*/
	X11env	*X11_env;	/*  X11 environment		*/

{
	XExposeEvent *expose;	/*  Pointer for expose events	*/
	Sir	*sir;		/*  Pointer to sys resources	*/



/*--------------------------------------------------------------*/
/*	Copy pixmap to window when an exposure event occurs	*/
/*--------------------------------------------------------------*/

sir = (Sir *) image -> sysres;			/*  Point to X resources  */
expose = (XExposeEvent *) &sir -> event;	/*  Point to event data	  */

while (1)				/*  Process next event	*/
	{
	XNextEvent ( X11_env->display, expose );

	switch ( (sir->event).type )
	    {
	    case Expose:	/*  Expose:  Copy area from pixmap	*/

		XCopyArea ( X11_env->display, sir->pix, sir->win, X11_env->gc,
			expose->x, expose->y, expose->width, expose->height,
			expose->x, expose->y );
		break;

	    case NoExpose:	/*  A couple noise events...		*/
	    case GraphicsExpose:

		continue;

	    default:		/*  Anything else:  Give it to caller	*/

		return &sir->event;
	    }
	}
}


/*1
===============================================================================
=======                                                                 =======
=======		       put_img --  Put Image to Drawable		=======
=======                                                                 =======
===============================================================================

  Invocation:

	put_img ( image, drawable, geometry, X11_env )


  Functions:

	Copy an image to a region of a drawable.


	WARNING:  This may modify the original image:

	If "force monochrome" or "force grayscale" is in effect,
	or if the target display environment has a restrictive
	visual (e.g., monochrome or grayscale display), or if
	the image contains more colors than are available,...

	The ORIGINAL image may be quantized, converted to grayscale,
	dithered, or otherwise disturbed.


  Input:

	image		Img *
			Pointer to image to put (i.e., image to draw)

	drawable	Drawable
			Window or pixmap to put image into:
			X11 Drawable (XID)

	geometry	Geom *
			geometry of region to write in drawable;
			NULL => image_width x image_height + 0 + 0

	X11_env		X11env *
			Pointer to X11 interface environment
			(e.g., as created by start_X11)


  Output:

	status		int
	<return value>	Status:  0 => Couldn't put image, 1 => did.
*/



int	 put_img ( image, drawable, geometry, X11_env )

	Img	*image;			/*  Ptr to Img structure	*/
	Drawable drawable;		/*  Drawable to get image from	*/
	Geom	*geometry;		/*  Geometry of region to write	*/
	X11env	*X11_env;		/*  X11 interface environment	*/

{
	Geom	 dgeom;			/*  Default geometry		*/
	Geom	*ageom;			/*  Actual geometry		*/
	Sir	*imx;			/*  Ptr to image's X resources	*/
	XImage	*im;			/*  Ptr to XImage		*/
	int	 status;		/*  Status returned from X11	*/
	Word	 plane_masks[8];	/*  XAllocColorCells needs this	*/
	int	 i;			/*  Universal index		*/
	int	 ncola;			/*  Number r/w colors available	*/
	XColor	 c;			/*  An X11 color definition	*/
	Col	*cp;			/*  Pointer to an img color	*/
	Byte	*xpix;			/*  Pointer to X pixels		*/
	Byte	*pp;			/*  Img pixel pointer		*/
	Byte	*xp;			/*  X11 pixel pointer		*/
	int	 install_new_cmap;	/*  Install-color-map flag	*/



/*--------------------------------------*/
/*	Verify that image has data	*/
/*--------------------------------------*/

if (   (image->imgtype == mapped_info)
    || (image->imgtype == rgb_info) )
   {
   fprintf ( stderr, "put_img:  Can't transmit info-only image (no data) to X11\n" );
   return 0;
   }


/*------------------------------------------------*/
/*	If geometry was omitted, default it	  */
/*------------------------------------------------*/

if ( geometry != NULL )
   ageom = geometry;			/*  Use caller's geometry	*/
else
   {					/*  Default to image size,	*/
   dgeom.width  = image -> width;	/*  align top left corner	*/
   dgeom.height = image -> height;	/*  of image with top left	*/
   dgeom.xoff   = 0;			/*  corner of drawable		*/
   dgeom.yoff   = 0;
   ageom = &dgeom;
   }



/*------------------------------------------*/
/*	Warp image to match visual or	    */
/*	forced characteristics		    */
/*------------------------------------------*/

if (   (X11_env -> force == F_GRAYSCALE)
    || ( (   ((X11_env->visual) -> class == StaticGray)
	  || ((X11_env->visual) -> class == GrayScale))
	&& ((X11_env->visual) -> map_entries > 2) ) )
   color_to_gray (image);

if (   (X11_env -> force == F_MONOCHROME)
    || (X11_env -> maxcolors == 2)
    || ((X11_env->visual) -> map_entries == 2) )
   {
   status = color_to_mono (image);
   if ( status == 0 )
      return 0;
   }


/*	If image is mapped & display is RGB, change image to RGB form	*/

if (   ((X11_env->visual) -> class == DirectColor)
    && (image -> imgtype == mapped_image) )
   {
   status = change_img ( rgb_image, image );
   if ( status == 0 )
      return 0;
   }

/*	If image is RGB & display isn't, change image to mapped form	*/

if (   ((X11_env->visual) -> class != DirectColor)
    && (image -> imgtype != mapped_image) )
   {
   quant_rgb ( image, X11_env -> maxcolors );
   status = change_img ( mapped_image, image );
   if ( status == 0 )
      return 0;
   }


if ( image->sysres == NULL )		/*  If image has no sys resources  */
   {
   status = create_sysres (image, X11_env);	/*  Create sys resources   */
   if ( status == 0 )
      {
      fprintf (stderr, "put_img:  Couldn't allocate X resources\n");
      return 0;
      }
   }

imx   = (Sir *) image -> sysres;	/*  Point to image's X resources   */
install_new_cmap = 0;			/*  Presume to use old color map   */


if ( image -> imgtype == rgb_image )	/*------------------------------*/
   {					/*  Zowie, 24-bit RGB!!!	*/
					/*------------------------------*/
   im = XCreateImage ( X11_env->display,  X11_env->visual, X11_env->depth,
	ZPixmap, 0, image->pixdata, ageom->width, ageom->height, 32, 0 );

   if ( im == NULL )
      {
      fprintf (stderr, "put_img:  Couldn't create X11 XImage\n" );
      return 0;
      }

   XPutImage ( X11_env->display, drawable, X11_env->gc, im,
	0, 0, ageom->xoff, ageom->yoff, ageom->width, ageom->height );

   return 1;
   }


/*---------------------------------*/
/*	Deal with color mapping	   */
/*---------------------------------*/

if ( imx -> num_colors == 0 )		/*  If necessary,		*/
   {					/*	Allocate X colors	*/

   if   ( X11_env -> color_map == PRIVATE )  /*  For private color map	*/
	{
	X11_env -> cmap = XCreateColormap	/*  Create new one	*/
	  ( X11_env->display, X11_env->rootwin, X11_env->visual, AllocNone );
	install_new_cmap = 1;		/*  Ask for later installation	*/
	}

   if ( X11_env -> color_instances == PRIVATE )  /*  For private colors	*/
      {					/*	Allocate map cells	*/
      ncola = ck_colorcap ( X11_env );	/*  Find # colors available	*/
      if ( ncola < image->num_colors )	/*  If too few...		*/
	 quant_cmi ( image, ncola );	/*     Quantize to max # of	*/
					/*     available colors		*/

      imx -> num_colors = image->num_colors;  /* Save count of X colors	*/
					/*  Allocate this many colors:	*/
      status = XAllocColorCells ( X11_env->display, X11_env->cmap,
		FALSE, plane_masks, 0, imx->pixels, image->num_colors );
      if ( status == 0 )
	 {
	 fprintf (stderr,"put_img:  Couldn't allocate enough colors from X\n");
	 return 0;
	 }
      }
   else					/*  If using shared colors	*/
      {
      for ( i=0; i < image->num_colors; ++i )	/*  For each image color  */
	  {					/*  Get nearest X color	*/
	  cp = &(image->colormap[i]);
	  c.red   = cp->red   << 8;
	  c.green = cp->green << 8;
	  c.blue  = cp->blue  << 8;
	  c.flags = DoRed | DoGreen | DoBlue;
	  status = XAllocColor (X11_env->display, X11_env->cmap, &c);
	  if ( status == 0 )		/*  If we run out of colors  */
	     {
	     XFreeColors ( X11_env->display, X11_env->cmap,
			imx->pixels, i, 0 );	/*  Free alloc'd colors	*/
	     quant_cmi ( image, i-1 );	/*  Quantize to fewer colors	*/
	     i = -1;			/*  Restart allocation		*/
	     continue;
	     }
	  imx->pixels[i] = c.pixel;
	  }
      }
   }



/*------------------------------------------------*/
/*	Set color definitions for this image	  */
/*------------------------------------------------*/

if ( X11_env -> color_instances == PRIVATE )
   for ( i=0; i < image->num_colors; ++i )	/*  Store private colors  */
       {
       cp = &(image->colormap[i]);
       c.red   = cp->red   << 8;
       c.green = cp->green << 8;
       c.blue  = cp->blue  << 8;
       c.pixel = imx->pixels[i];
       c.flags = DoRed | DoGreen | DoBlue;
       XStoreColor (X11_env->display, X11_env->cmap, &c);
       }



/*-----------------------------------------*/
/*	Map pixel values to X11 pixels	   */
/*-----------------------------------------*/

xpix = (Byte *) malloc (image->num_pixels);	/*  Allocate memory for	*/
if ( xpix == NULL )			/*	a copy of pixel data	*/
   {
   fprintf (stderr, "put_img:  Insufficient memory\n" );
   return 0;
   }

pp = image->pixdata;			/*  Access img pixels with pp,	*/
xp = xpix;				/*	   X11 pixels with xp	*/
i  = image->num_pixels;

while ( i-- > 0 )			/*  For each pixel		*/
      *xp++ = imx->pixels[*pp++];	/*      remap to X11 pixel	*/



/*--------------------------*/
/*	Create XImage	    */
/*--------------------------*/

im = XCreateImage ( X11_env->display,  X11_env->visual, X11_env->depth,
	ZPixmap, 0, xpix, ageom->width, ageom->height, 8, 0 );

if ( im == NULL )
   {
   fprintf (stderr, "put_img:  Couldn't create X11 XImage\n" );
   free (xpix);
   return 0;
   }

XPutImage ( X11_env->display, drawable, X11_env->gc, im,
	0, 0, ageom->xoff, ageom->yoff, ageom->width, ageom->height );

free (xpix);				/*  Release copied pixel data	*/

if ( install_new_cmap )			/*  For private color map	*/
   {					/*	 Muck around a lot	*/
   XSetWindowAttributes	 winat;		/*  Crud for changing color map	*/

   winat.colormap = X11_env->cmap;
   XChangeWindowAttributes		/*  Change window to use this map  */
	( X11_env->display, imx->win, CWColormap, &winat );
   XInstallColormap ( X11_env->display, X11_env->cmap );
   }

return 1;
}



/*1
===============================================================================
=======                                                                 =======
=======		     start_X11  --  Start up X Interface		=======
=======                                                                 =======
===============================================================================

  Invocation:

	X11_env = start_X11 ( argc, argv )


  Function:

	Start_X11 opens a connection to an X11 display server and
	saves information for a default X11 interface environment.

	The caller should terminate use of this environment by
	calling stop_X11.


  Input:

	argc		int
			Number of arguments on command line used
			to invoke program.  Minimum argc = 1.

	argv		char *[]
			Array of pointers to command line arguments.
			argv[0] must contain the program name.


  Output:

	X11_env		X11env *
	<return value>	Pointer to X11 interface environment
			(e.g., as created by start_X11)
			NULL => Couldn't open named display

*/


X11env	*start_X11 ( argc, argv )

	int	 argc;			/*  Number of command arguments	*/
	char	*argv[];		/*  Pointers to arguments	*/

{
	/*  Pointers to command line arguments	*/

	char	*program;		/*  program name		*/
	char	*display_name;		/*  -display	host:d.s	*/
	char	*geometry;		/*  -geometry	WxH+X+Y		*/
	char	*maxcolors;		/*  -maxcolors	n		*/
	char	*force;			/*  -force	nothing/grayscale/
							monochrome	*/
	char	*colormap;		/*  -colormap	private/shared	*/
	char	*colorinstances;	/*  -colorinstances	"	*/


	/*	Other local data	*/

	X11env	*X11_env;		/*  X11 interface environment	*/
	char	*default_string;	/*  Default for color selection	*/
	char	 c;			/*  1st character of default	*/
	long	 vm;			/*  Visual info mask		*/
	XVisualInfo  vt;		/*  Visual info template	*/
	int	 nv;			/*  Number of visuals		*/
	int	 i;			/*  Index to arguments		*/



/*:::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort after an error	 */
/*:::::::::::::::::::::::::::::::::::::::*/

#define	abort_sx(msg) { \
 fprintf(stderr,"%s\n",msg); \
 if (X11_env != NULL) free(X11_env); \
 return NULL; }



/*------------------------------------------------------*/
/*	Allocate interface environment structure,	*/
/*	connect to display server, and set options	*/
/*	from command line switches and resources	*/
/*------------------------------------------------------*/

program		= argv[0];	/*  1st arg is program name		*/
display_name	= NULL;		/*  Preset pointers to other possible	*/
geometry	= NULL;		/*	command args to NULL		*/
maxcolors	= NULL;
force		= NULL;
colormap	= NULL;
colorinstances	= NULL;

for ( i=1; i < argc; ++i )	/*  Scan cmd args for known switches	*/
    {
    if  ( *argv[i] != '-' )
	continue;
    if	( strcmp(argv[i], "--") == 0 )
	break;
    if  ( strcmp(argv[i], "-display") == 0 )
	display_name	= argv[++i];
    if  ( strcmp(argv[i], "-geometry") == 0 )
	geometry	= argv[++i];
    if  ( strcmp(argv[i], "-maxcolors") == 0 )
	maxcolors	= argv[++i];
    if  ( strcmp(argv[i], "-force") == 0 )
	force		= argv[++i];
    if  ( strcmp(argv[i], "-colormap") == 0 )
	colormap	= argv[++i];
    if  ( strcmp(argv[i], "-colorinstances") == 0 )
	colorinstances	= argv[++i];
    }


X11_env = (X11env *) malloc (sizeof(X11env));
if ( X11_env == NULL )
   abort_sx("start_X11:  Insufficient memory")

X11_env -> display = XOpenDisplay ( display_name );
if ( X11_env->display == NULL )
   abort_sx("Unable to connect to display server")


if ( geometry == NULL )
   geometry =
	XGetDefault ( X11_env -> display, program, "geometry" );
if ( maxcolors == NULL )
   maxcolors =
	XGetDefault ( X11_env -> display, program, "maxcolors" );
if ( force == NULL )
   force =
	XGetDefault ( X11_env -> display, program, "force" );
if ( colormap == NULL )
   colormap =
	XGetDefault ( X11_env -> display, program, "colormap" );
if ( colorinstances == NULL )
   colorinstances =
	XGetDefault ( X11_env -> display, program, "colorinstances" );


X11_env -> geommask = 0;
(X11_env -> geometry).width  = 0;
(X11_env -> geometry).height = 0;
(X11_env -> geometry).xoff   = 40;
(X11_env -> geometry).yoff   = 40;

if ( geometry != NULL )
   X11_env -> geommask = XParseGeometry ( geometry,
	&(X11_env->geometry).xoff,  &(X11_env->geometry).yoff,
	&(X11_env->geometry).width, &(X11_env->geometry).height );

X11_env -> maxcolors = 256;

if ( maxcolors != NULL )
   {
   X11_env -> maxcolors = atoi(maxcolors);
   if ((X11_env -> maxcolors < 2) || (X11_env -> maxcolors > 256))
      {
      fprintf ( stderr,
	"\nWarning:  maxcolors = %s is invalid,\n", maxcolors );
      fprintf ( stderr, "\tShould be in range 2-256; defaulting to 256\n" );
      X11_env -> maxcolors = 256;
      }
   }


X11_env -> force	  = F_NOTHING;

if ( force != NULL )
   {
   if ( strcmp (force, "nothing") == 0 )
      X11_env -> force = F_NOTHING;
   else if ( strcmp (force, "grayscale") == 0 )
      X11_env -> force = F_GRAYSCALE;
   else if ( strcmp (force, "monochrome") == 0 )
      X11_env -> force = F_MONOCHROME;
   else
      {
      fprintf ( stderr,
	"\nWarning:  force = %s not recognized,\n", force );
      fprintf ( stderr,
	 "\texpected \"nothing\", \"grayscale\", or \"monochrome\" --\n" );
      fprintf ( stderr, "\tDefaulting to \"force nothing\"\n\n" );
      }
   }


X11_env -> color_map	  = DEFAULT_COLOR_MAP;

if ( colormap != NULL )
   {
   if ( strcmp (colormap, "private") == 0 )
      X11_env -> color_map = PRIVATE;
   else if ( strcmp (colormap, "shared") == 0 )
      X11_env -> color_map = SHARED;
   else
      {
      fprintf ( stderr,
	"\nWarning:  colormap = %s not recognized,\n", colormap );
      fprintf ( stderr, "\texpected \"private\" or \"shared\" --\n" );
#if DEFAULT_COLOR_MAP-SHARED
      fprintf ( stderr, "\tDefaulting to \"colormap private\"\n\n" );
#else
      fprintf ( stderr, "\tDefaulting to \"colormap shared\"\n\n" );
#endif
      }
   }


X11_env -> color_instances = DEFAULT_COLOR_INSTANCES;

if ( colorinstances != NULL )
   {
   if ( strcmp (colorinstances, "private") == 0 )
      X11_env -> color_instances = PRIVATE;
   else if ( strcmp (colorinstances, "shared") == 0 )
      X11_env -> color_instances = SHARED;
   else
      {
      fprintf ( stderr,
	"\nWarning:  colorinstances = %s not recognized,\n", colorinstances );
      fprintf ( stderr, "\texpected \"private\" or \"shared\" --\n" );
#if DEFAULT_COLOR_INSTANCES-SHARED
      fprintf ( stderr, "\tDefaulting to \"colorinstances private\"\n\n" );
#else
      fprintf ( stderr, "\tDefaulting to \"colorinstances shared\"\n\n" );
#endif
      }
   }



/*----------------------------------------------*/
/*	Set default interface environment	*/
/*----------------------------------------------*/

X11_env -> screen  = DefaultScreen	( X11_env->display );
X11_env -> width   = DisplayWidth	( X11_env->display, X11_env->screen );
X11_env -> height  = DisplayHeight	( X11_env->display, X11_env->screen );
X11_env -> depth   = DisplayPlanes	( X11_env->display, X11_env->screen );
X11_env -> defvisual = DefaultVisual	( X11_env->display, X11_env->screen );
X11_env -> visual  = X11_env -> defvisual;
vm = VisualScreenMask;
vt.screen = X11_env -> screen;
X11_env -> visinfo = XGetVisualInfo	( X11_env->display, vm, &vt, &nv );
X11_env -> numvis  = nv;
X11_env -> rootwin = RootWindow		( X11_env->display, X11_env->screen );
X11_env -> cmap    = DefaultColormap	( X11_env->display, X11_env->screen );
X11_env -> gc      = XCreateGC	(X11_env->display, X11_env->rootwin, 0, NULL);


/*
	Latent extension:  Choose one of multiple visuals
	when more than one is available.

	Not implemented yet because we don't have a system
	that supports this yet.
*/

/*	Verify that visual being used is one that imglib supports	*/

switch ((X11_env->visual) -> class)
    {
    case StaticGray:
	if ( X11_env -> color_map != SHARED )
	   {
	   fprintf ( stderr,
		"\nWarning:  Overriding colormap option to \"shared\"\n" );
	   fprintf ( stderr, "\tto allow using StaticGray visual\n\n" );
	   X11_env -> color_map = SHARED;
	   }
	if ( X11_env -> color_instances != SHARED )
	   {
	   fprintf ( stderr,
	"\nWarning:  Overriding colorinstances option to \"shared\"\n" );
	   fprintf ( stderr, "\tto allow using StaticGray visual\n\n" );
	   X11_env -> color_instances = SHARED;
	   }
	break;

    case GrayScale:
	break;

    case StaticColor:
	if ( X11_env -> color_map != SHARED )
	   {
	   fprintf ( stderr,
		"\nWarning:  Overriding colormap option to \"shared\"\n" );
	   fprintf ( stderr, "\tto allow using StaticColor visual\n\n" );
	   X11_env -> color_map = SHARED;
	   }
	if ( X11_env -> color_instances != SHARED )
	   {
	   fprintf ( stderr,
	"\nWarning:  Overriding colorinstances option to \"shared\"\n" );
	   fprintf ( stderr, "\tto allow using StaticColor visual\n\n" );
	   X11_env -> color_instances = SHARED;
	   }
	break;

    case PseudoColor:
	break;

    case TrueColor:
	if ( X11_env -> color_map != SHARED )
	   {
	   fprintf ( stderr,
		"\nWarning:  Overriding colormap option to \"shared\"\n" );
	   fprintf ( stderr, "\tto allow using TrueColor visual\n\n" );
	   X11_env -> color_map = SHARED;
	   }
	if ( X11_env -> color_instances != SHARED )
	   {
	   fprintf ( stderr,
	"\nWarning:  Overriding colorinstances option to \"shared\"\n" );
	   fprintf ( stderr, "\tto allow using TrueColor visual\n\n" );
	   X11_env -> color_instances = SHARED;
	   }
	break;

    case DirectColor:
	if ( (X11_env->visual) -> bits_per_rgb != 8 )
	   {
	   fprintf ( stderr,
	"\nWarning:  DirectColor visual uses %d bits per RGB component;\n",
		(X11_env->visual) -> bits_per_rgb );
	   fprintf ( stderr,
	"\tImglib supports only 8, Something will probably fail...\n\n" );
	   }
	if (   ((X11_env->visual) -> red_mask   != pixel_red)
	    || ((X11_env->visual) -> green_mask != pixel_green)
	    || ((X11_env->visual) -> blue_mask  != pixel_blue) )
	   {
	   fprintf ( stderr,
  "\nWarning:  Imglib doesn't support DirectColor visuals with RGB masks\n" );
	   fprintf ( stderr,
  "\tOther than %08X, %08X, and %08X, respectively\n",
		pixel_red, pixel_green, pixel_blue );
	   fprintf ( stderr, "\tSomething will probably fail...\n\n" );
	   }
	break;
    }

return X11_env;
}



/*1
===============================================================================
=======                                                                 =======
=======		    stop_X11  --  Stop (Terminate) X Interface		=======
=======                                                                 =======
===============================================================================

  Invocation:

	stop_X11 ( X11_env )


  Function:

	Start_X11 closes a connection to an X11 display server and
	frees the default X11 interface environment information
	that was saved by start_X11.


  Input:


	X11_env		X11env *
	<return value>	Pointer to X11 interface environment
			(e.g., as created by start_X11)


  Output:	None

*/


void	 stop_X11 ( X11_env )

	X11env	*X11_env;		/*  X11 interface environment	*/

{
if ( X11_env -> color_map == PRIVATE )	/*  Uninstall any private color map  */
   XUninstallColormap ( X11_env -> display, X11_env -> cmap );

XFree ( X11_env -> visinfo );		/*  Free visual info		     */

XCloseDisplay ( X11_env -> display );	/*  Close display server connection  */
free ( X11_env );			/*  Release environment's memory     */
}


/*1
===============================================================================
=======                                                                 =======
=======		  update_colors  --  Update X11 Color Map		=======
=======                                                                 =======
===============================================================================

  Invocation:

	update_colors ( image )


  Functions:

	Update X11 color map to match new contents of image color map.
	At present this is implemented only for X11 environments
	using private colors.

	This does not allow changing the size of an existing color map.


  Input:

	image		Img *
			Pointer to image to put (i.e., image to draw)


  Output:

	None to caller; X11 color map is updated,
*/



void	update_colors ( image )

	Img	*image;			/*  Ptr to Img structure	*/

{
	Sir	*imx;			/*  Ptr to image's X resources	*/
	X11env	*X11_env;		/*  X11 environment		*/
	Col	*cp;			/*  Pointer to a color (Col)	*/
	XColor	 c;			/*  An X11 color definition	*/
	int	 i;			/*  Color map index		*/
	int	 status;		/*  Status returned by function	*/



imx   = (Sir *) image -> sysres;	/*  Point to image's X resources*/
X11_env = imx -> X11_env;		/*  Point to X11 environment	*/


if ( X11_env -> color_instances == PRIVATE ) 
   for ( i=0; i < image->num_colors; ++i )	/*  Store private colors */
       {
       cp = &(image->colormap[i]);
       c.red   = cp->red   << 8;
       c.green = cp->green << 8;
       c.blue  = cp->blue  << 8;
       c.pixel = imx->pixels[i];
       c.flags = DoRed | DoGreen | DoBlue;
       XStoreColor (X11_env->display, X11_env->cmap, &c);
       }
else
   for ( i=0; i < image->num_colors; ++i )	/*  Get shared colors	*/
       {
       XFreeColors ( X11_env->display, X11_env->cmap,
			imx->pixels, i, 0 );	/*  Free alloc'd colors	*/
       cp = &(image->colormap[i]);
       c.red   = cp->red   << 8;
       c.green = cp->green << 8;
       c.blue  = cp->blue  << 8;
       c.flags = DoRed | DoGreen | DoBlue;
       status = XAllocColor (X11_env->display, X11_env->cmap, &c);
       if ( status != 0 )
          imx->pixels[i] = c.pixel;
       else				/*  If out of shared colors	*/
	  imx->pixels[i] = 0;		/*	default to pixel 0	*/
       }

}
