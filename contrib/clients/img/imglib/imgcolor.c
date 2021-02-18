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
=======		   Module Imgcolor  --  Manage Colors			=======
=======			      File imgcolor.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	Manage image colors.

	This includes commonly used functions related for reducing the
	chromatic content of an image.  It also includes less commonly
	used functions for supporting multiple color maps with a
	single set of pixel data.

	Beware:  code for color dithering is in an intermediate
	stage between something that worked and something that
	was fast.  It's not currently used but some variant of it
	will probably revive in the future.


  Public Routines:

	img_rep_node	Replenish free node supply (for color tree)
	init_colman	Initialize image for color management
	dither_rgb	Dither an RGB image


  Local Routines:

	expand_color	Expand a color definition
	nearest_color	Find nearest mapped color


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	8-21-89		Paul Raveling
	Fixed bug in init_colman:  One reference was to the wrong
	pixel pointer when scanning an RGB image.

	3-24-89		Paul Raveling
	Created module.
*/



#include <stdio.h>
#include <imglib/imglib.h>

#define	reg	 register


extern	char	*malloc ();		/*  Allocate memory		*/
extern	char	*calloc ();		/*  Allocate memory differently	*/


/*--------------------------------------------------------------*/
/*	Data related to dithering, defined in module imgcvt	*/
/*--------------------------------------------------------------*/
/*	Tables used to accelerate processing for dithering	*/
/*	... plus one for grayscale conversion			*/
/*	Shared with other modules for other dithering		*/
/*--------------------------------------------------------------*/

extern	Rgbpix	 img_rgbbw[2] /* = {0x00000000, 0x00ffffff} */;
				/*  RGB values for black and white	*/


/*
	Data below are initialized on 1st invocation
	of a dithering procedure
*/

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



	/*----------------------------------------------*/
	/*  Memory suballocation data for color trees	*/
	/*	--  Shared with color quantization	*/
	/*----------------------------------------------*/

 struct	 mem_block			/*  Memory block from malloc	*/
	 {
	 struct mem_block  *next_block;		/*  Link to next block	*/
	 Ctnode		    node_space[1000];	/*  Room for 1000 nodes	*/
	 };

 struct mem_block  *img_block_queue;	/*  Queue of malloc'd blocks	*/
 struct mem_block  *img_new_block;		/*  Pointer to new block	*/
	Ctnode	*img_next_node;		/*  Pointer to next free byte	*/
	int	 img_num_free;		/*  Remaining free space, as
						number of nodes		*/


/*::::::::::::::::::::::::::::::::::::::::::::::*/
/*      Macro to get a new node with all        */
/*      fields reset to 0                       */
/*::::::::::::::::::::::::::::::::::::::::::::::*/

#ifdef coltvol
#define get_node(np)    \
 { \
 if ( img_num_free-- > 0 )	np = img_next_node++; \
 else				np = img_rep_node (); \
 ++ coltvol; \
 }
#else
#define get_node(np)    \
 { \
 if ( img_num_free-- > 0 )	np = img_next_node++; \
 else				np = img_rep_node (); \
 }
#endif

/*1
******************************************************************************
******									******
******		   img_rep_node  --  Replenish Free Node Supply		******
******									******
******************************************************************************

  Invocation:

	new_node = img_rep_mode ()

	[Invoked only from get_node macro]


  Function:

	Allocate memory for a new supply of node structures
	and preset all fields to 0.



  Input:

	None from caller; img_rep_node uses shared data structures
	img_block_queue, img_new_block, img_next_node, and img_num_free

  Output:

	new_node	Ctnode	*
	<return value>	Pointer to a new color tree node

	Data structures named as input are updated.


   Notes:

	Blocks are normally allocated by the get_node macro --
	get_node(np) set node pointer np to addr of new node.
	Before use, caller must initialize memory block data as follows:

img_block_queue = BNULL;	\*  Init memory block queue as empty	*\
img_num_free    = 0;		\*  Indicate no free blocks		*\

	After the color tree is no longer needed, space should be freed
	with this code fragment:

while ( img_block_queue != BNULL )
      {
      img_new_block = img_block_queue -> next_block;
      free ( img_block_queue );
      img-block_queue = img_new_block;
      }
*/



Ctnode	*img_rep_node ()

{
reg	Ctnode	*cur;
reg	int	 i, n;
reg	Word	*wp;



/*------------------------------------------*/
/*	Allocate memory for a new node	    */
/*------------------------------------------*/

img_new_block = (struct mem_block *) malloc (sizeof(struct mem_block));
				/*  Link new block at head of block queue    */
if ( img_new_block == NULL )
   return NULL;

wp = (Word *) img_new_block;
n = sizeof (struct mem_block) / sizeof (Word);
for ( i=0; i < n; ++ i )		/*  Clear all node space to 0	*/
    *wp++ = 0;

img_new_block -> next_block = img_block_queue;
img_block_queue = img_new_block;
img_num_free = 999;		/*  Suballocate first node-sized piece	     */
cur = img_new_block -> node_space;	/*  while finishing block init	     */
img_next_node = &(img_new_block -> node_space[1]);

return cur;
}




/*2
===============================================================================
=======                                                                 =======
=======		nearest_color  --  Find nearest mapped color		=======
=======                                                                 =======
===============================================================================

  Invocation:

	near_node = nearest_color ( image, red, green, blue, root_node )


  Functions:

	Find node of a color description subtree that contains
	the mapped color nearest a given color.

	This is a recursive procedure invoked from color dithering
	routines.  It's the caller's responsibility to call it
	as infrequently as possible and for as small a subtree
	as possible.  This can be VERY slow.


  Input:

	image		Img	*
			Pointer to Img structure, for locating color map

	root_node	Ctnode	*
			Pointer to root of color subtree to search

	red		Byte
	green		Byte
	blue		Byte
			Component values of target color


  Output:

	near_node	Ctnode	*
	<return value>	Pointer to node of tree which refers to the
			color map entry whose color is nearest that
			of the target color
*/



static Ctnode	*nearest_color ( image, red, green, blue, root_node )

	Img	*image;			/*  Image			*/
	Ctnode	*root_node;		/*  Root of subtree to search	*/
	Byte	 red, green, blue;	/*  Target color		*/

{
reg	Ctnode	*cur;			/*  Current node		*/
	Ctnode	*nearest;		/*  Nearest node (working copy)	*/
	Ctnode	*nbelow;		/*  Nearest in lower subtree	*/
reg	Col	*mc;			/*  Pointer to mapped color	*/
reg	int	 r, g, b;		/*  Target color as integers	*/
	int	 dxr, dxg, dxb;		/*  Distance components		*/
	int	 dx;			/*  Current distance		*/
	int	 mindx;			/*  Minimum distance		*/



/*	Macro to compute distance**2 of mapped color from target	   */

#define	getdx(node) { \
 mc = &(image->colormap[node->cmi]); \
 dxr = r - (int) mc->red; \
 dxg = g - (int) mc->green; \
 dxb = b - (int) mc->blue; \
 dx = dxr*dxr + dxg*dxg + dxb*dxb; \
 }



/*--------------------------------------*/
/*	If root node has no offspring	*/
/*	just return root node		*/
/*--------------------------------------*/
/*
	This relies on tree structure:
	Each leaf must have an assigned color
*/

cur = root_node;
if ( cur->off == NULL )
   return cur;

r = red;
g = green;		/*  Keep target color in registers as integers	*/
b = blue;
nearest = NULL;
mindx   = 195076;

if ( cur -> numuniq != 0 )	/*  If root node has mapped color	*/
   {				/*	find actual distance to it	*/
   getdx(cur)
   mindx = dx;
   nearest = cur;
   }


/*--------------------------------------*/
/*	Find nearest mapped color	*/
/*	in subtree below		*/
/*--------------------------------------*/

cur = cur -> off;	/*  Point to 1st offspring of root	*/

while ( cur != NULL )
    {
    if ( cur->numuniq != 0 )	/*  If node has an assigned color	*/
	{			/*	check its distance from target	*/
	getdx(cur)
	if ( dx < mindx )
	   {
	   mindx = dx;
	   nearest = cur;
	   }
	}

    if ( cur -> off != NULL )	/*  If node has offspring	*/
	{			/*	find nearest lower node	*/
	nbelow = nearest_color ( image, red, green, blue, cur->off );
	getdx(nbelow)
	if ( dx < mindx )
	   {
	   mindx = dx;
	   nearest = nbelow;
	   }
	}

    cur = cur -> sibling;	/*  Check next sibling		*/
    }

return nearest;
}


/*2
===============================================================================
=======                                                                 =======
=======		expand_color  --  Expand a color definition		=======
=======                                                                 =======
===============================================================================

  Invocation:

	expand_color (parent, proto, red, green, blue);


  Functions:

	Expand a color definition in a color tree to include
	a new RGB value.

	--  If the given parent has no offspring, define the color
	    in a new node.

	--  If a node on the target level of the tree can have
	    its RGB range expanded to include the color, do so.
	    Expansion is not allowed if any vertex of the expanded
	    color box would lie within any sibling's node.

	--  Otherwise define the color in a new node.


  Input:

	parent		Ctnode	*
			Node which will be parent of expanded or new node

	proto		Ctnode	*
			Node containing prototype data for the new node,
			mainly needing numuniq != 0 and cmi.

	red		Byte
	green		Byte
	blue		Byte
			Component values of target color


  Output:

	None to caller.  Color tree is modified by change to an existing
	offspring of the given parent node or creation of a new offspring
	for it.
*/

static
void	expand_color (parent, proto, red, green, blue)

	Ctnode	*parent;		/*  Parent node		*/
	Ctnode	*proto;			/*  Prototype node	*/
	Byte	 red, green, blue;	/*  Color		*/

{
reg	Ctnode	*cur;			/*  Current node	*/
	Ctnode	*new;			/*  New node		*/
	int	 loc_case;		/*  Color location case	*/
reg	int	 dxr,dxg,dxb;		/*  Distance components	*/
	int	 dx, mindx;		/*  Distances (**2)	*/
reg	int	 r, g, b;		/*  RGB as integers	*/
	Byte	 minr, maxr;
	Byte	 ming, maxg;		/*  Expanded limits	*/
	Byte	 minb, maxb;




/*----------------------------------------------*/
/*	If parent has no offspring, add one	*/
/*----------------------------------------------*/

if ( parent -> off == NULL )
   {
   get_node(cur)			/*  Get new node	*/
   if ( cur == NULL ) return;
   *cur = *proto;			/*  Preset to prototype	*/
   cur -> parent  = parent;
   cur -> off	  = NULL;		/* Reset family linkage	*/
   cur -> sibling = NULL;
   cur -> minred   = cur -> maxred   = red;
   cur -> mingreen = cur -> maxgreen = green;	/*  Set colors	*/
   cur -> minblue  = cur -> maxblue  = blue;
   return;
   }


/*-------------------------------------------------*/
/*	Find nearest color box among siblings	   */
/*-------------------------------------------------*/

new = NULL;
mindx = 195076;
r = red;
g = green;
b = blue;
cur = parent -> off;

while ( cur != NULL )
    {
    loc_case = 0;
    if ( red   < cur -> minred   ) loc_case |= 0x01;
    if ( red   > cur -> maxred   ) loc_case |= 0x02;
    if ( green < cur -> mingreen ) loc_case |= 0x04;
    if ( green > cur -> maxgreen ) loc_case |= 0x08;
    if ( blue  < cur -> minblue  ) loc_case |= 0x10;
    if ( blue  > cur -> maxblue  ) loc_case |= 0x20;

#define	pdx(r2,g2,b2) { \
 dxr = r - (int) r2; dxr *= dxr; \
 dxg = g - (int) g2; dxg *= dxg; \
 dxb = b - (int) b2; dxb *= dxb; \
 dx = dxr + dxb + dxg; \
 break; }


    switch ( loc_case )
	{
	case 0x01:	dx = cur->minred - red;  	dx*=dx;	break;
	case 0x02:	dx = red - cur->maxred;  	dx*=dx;	break;
	case 0x04:	dx = cur->mingreen - green;	dx*=dx;	break;
	case 0x05:	pdx(cur->minred,cur->mingreen,b)
	case 0x06:	pdx(cur->maxred,cur->mingreen,b)
	case 0x08:	dx = green - cur->maxgreen;	dx*=dx;	break;
	case 0x09:	pdx(cur->minred,cur->maxgreen,b)
	case 0x0a:	pdx(cur->maxred,cur->maxgreen,b)
	case 0x10:	dx = cur->minblue - b;		dx*=dx; break;
	case 0x11:	pdx(cur->minred,g,cur->minblue)
	case 0x12:	pdx(cur->maxred,g,cur->minblue)
	case 0x14:	pdx(r,cur->mingreen,cur->minblue)
	case 0x15:	pdx(cur->minred,cur->mingreen,cur->minblue)
	case 0x16:	pdx(cur->maxred,cur->mingreen,cur->minblue)
	case 0x18:	pdx(r,cur->maxgreen,cur->minblue)
	case 0x19:	pdx(cur->minred,cur->maxgreen,cur->minblue)
	case 0x1a:	pdx(cur->maxred,cur->maxgreen,cur->minblue)
	case 0x20:	dx = b - cur->maxblue;		dx*=dx; break;
	case 0x21:	pdx(cur->minred,g,cur->maxblue)
	case 0x22:	pdx(cur->maxred,g,cur->maxblue)
	case 0x24:	pdx(r,cur->mingreen,cur->maxblue)
	case 0x25:	pdx(cur->minred,cur->mingreen,cur->maxblue)
	case 0x26:	pdx(cur->maxred,cur->mingreen,cur->maxblue)
	case 0x28:	pdx(r,cur->maxgreen,cur->maxblue)
	case 0x29:	pdx(cur->minred,cur->maxgreen,cur->maxblue)
	case 0x2a:	pdx(cur->maxred,cur->maxgreen,cur->maxblue)
	default:	return;		/*  Impossible cases	*/
	}

    if  ( dx < mindx )
	{
	mindx = dx;
	new = cur;
	}

    cur = cur -> sibling;		/*  Check next sibling	*/
    }


/*----------------------------------------------*/
/*	See if nearest sibling can expand	*/
/*	without overlapping other siblings	*/
/*----------------------------------------------*/

/*	Macro to shorten code below...	*/

#define	break_if_in_node(r,g,b) { \
 if (	   (r >= cur->minred  ) && (r <= cur->maxred  ) \
	&& (g >= cur->mingreen) && (g <= cur->maxgreen) \
	&& (b >= cur->minblue ) && (b <= cur->maxblue ) ) \
	break; \
 }


/*	Identify potential new box for nearest color	*/

if ( red   < new->minred   ) minr = r; else minr = new->minred;
if ( red   > new->maxred   ) maxr = r; else maxr = new->maxred;
if ( green < new->mingreen ) ming = g; else ming = new->mingreen;
if ( green > new->maxgreen ) maxr = g; else maxg = new->maxgreen;
if ( blue  < new->minblue  ) minb = b; else minb = new->minblue;
if ( blue  > new->maxblue  ) maxr = b; else maxb = new->maxblue;


/*	See if any vertex of any sibling color is in expanded box	*/

cur = parent -> off;
while ( cur != NULL )
    {
    break_if_in_node(minr,ming,minb)
    break_if_in_node(minr,ming,maxb)
    break_if_in_node(minr,maxg,minb)
    break_if_in_node(minr,maxg,maxb)
    break_if_in_node(maxr,ming,minb)
    break_if_in_node(maxr,ming,maxb)
    break_if_in_node(maxr,maxg,minb)
    break_if_in_node(maxr,maxg,maxb)

    cur = cur -> sibling;
    }


if ( cur == NULL )		/*  If no conflict occurred	*/
   {				/*	expand existing node	*/
   new -> minred   = minr;
   new -> maxred   = maxr;
   new -> mingreen = ming;
   new -> maxgreen = maxg;
   new -> minblue  = minb;
   new -> maxblue  = maxb;
   }
else				/*  Else (RGB volumes conflict	*/
   {				/*	supply new node below nearest	*/
   cur = parent -> off;
   while ( cur->sibling != NULL ) cur = cur->sibling;
   get_node(new)
   if ( new == NULL ) return;
   *new = *proto;
   cur -> sibling = new;
   new -> parent  = parent;
   new -> off	  = NULL;
   new -> sibling = NULL;
   new -> minred   = new -> maxred   = red;
   new -> mingreen = new -> maxgreen = green;	/*  Set colors	*/
   new -> minblue  = new -> maxblue  = blue;
   }

}




/*1
===============================================================================
=======                                                                 =======
=======	    init_colman  --  Initialize Image for Color Management	=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = init_colman ( image )


  Functions:

	Initialize color management data for a color-mapped image.
	The image type can be either mapped_image or rgb_image,
	but an RGB image must have a color map, which would otherwise
	be optional.  RGB pixel values must have color map indices
	in their tag fields.

	This routine must be called after the image is created
	and before most other color management routines are invoked
	on it.

	Specific data initialized are:

	--  A color info queue entry for the current color map.
	    If queue entries exist, init_colman updates the one
	    at the head of the queue.  If none exist, it creates
	    a new queue entry.

	--  Pixel frequency statistics in the current color map
	    are initialized by scanning the pixel data.  While
	    doing this, pixels are checked for invalid color map
	    indices.  If any exist, a summary message is sent to
	    stderr and the faulty pixel values are replaced by 0.


  Input:

	image		Img	*
			Pointer to Img structure for image to delete


  Output:

	status		int
	<return value>	0 if initialization succeeded;
			1 if init completed but invalid pixels were found
			2 if init failed due to insufficient memory
			  or incorrect image format
*/



int	 init_colman ( image )

	Img	*image;		/*  Ptr to image to initialize	*/

{
	int	 bad_pixels;	/*  Count of invalid pixels		*/
	Mapinfo	*ci;		/*  Ptr to color info queue entry	*/
	Byte	 pixel;		/*  Current pixel value			*/
	Byte	*pp;		/*  Pixel pointer for mapped image	*/
	Rgbpix	*rgbp;		/*  Pixel pointer for RGB image		*/
	int	 i;		/*  Universal index			*/




/*::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort with message	  */
/*::::::::::::::::::::::::::::::::::::::::*/

#define	abort_init(msg) {fprintf (stderr, \
 "init_colman:  %s, can't initialize color management\n", msg); return 2;}



/*-----------------------------------------------------------------*/
/*	Verify that image has both a color map and pixel data	   */
/*-----------------------------------------------------------------*/

if ( (image->imgtype != mapped_image) && (image->imgtype != rgb_image) )
   abort_init("Wrong image type")

if ( (image->colormap == NULL) || (image->num_colors == 0) )
   abort_init("No color map")

if ( (image->pixdata == NULL) || (image->num_pixels == 0) )
   abort_init("No pixel data")



/*----------------------------------------------*/
/*	Initialize Color Info Queue Entry	*/
/*----------------------------------------------*/

ci = image -> cmiq;
if ( ci == NULL )		/*  If no quene entry exists yet	*/
   {				/*     Allocate and enqueue one		*/
   ci = (Mapinfo *) malloc (sizeof(Mapinfo));
   if (ci == NULL)
      abort_init("Insufficient memory to create color info structure")

   image -> cmiq = ci;		/*  Define entire color info queue	*/
   ci -> next_map = NULL;	/*  to be the new entry			*/
   }

ci -> map	= image -> colormap;	/*  Set pointer to color map	*/
ci -> mapcolors = image -> num_colors;	/*  Set count of colors in map	*/
ci -> mapunique = 0;			/*  Preset count of uniques = 0	*/




/*-----------------------------------------------*/
/*	   Check pixels for validity and	 */
/*	   Generate color frequency stats	 */
/*-----------------------------------------------*/

bad_pixels = 0;				/*  Preset for no bad pixels	*/

for ( i=0; i < image->num_colors; ++i )	/*  Preset color frequencies	*/
    image->colormap[i].freq = 0;	/*	to 0			*/

if ( image->imgtype == mapped_image )	/*  Count pixels for each color	*/
   {
   pp = image->pixdata;			/*  ****  Mapped Image  ****	*/
   i  = image->num_pixels;
   while ( i-- > 0 )			/*  For each pixel...		*/
	 {
	 pixel = *pp++;
	 if ( pixel >= image->num_colors )  /*  If pixel is invalid	*/
	    {
	    ++ bad_pixels;		/*	  count it and		*/
	    *(pp-1) = 0;		/*	  replace it with 0	*/
	    pixel   = 0;
	    }
	 ++ image->colormap[pixel].freq;/*  Increment color count for	*/
	 }				/*  this pixel's color		*/
   }
else
   {
   rgbp = (Rgbpix *) image->pixdata;	/*  ****  RGB image  ****	*/
   i  = image->num_pixels;
   while ( i-- > 0 )			/*  For each pixel...		*/
	 {
	 pixel = (*rgbp++ & pixel_tag) >> pixel_tag_shift;
	 if (pixel >= image->num_colors)   /*  If pixel is invalid	*/
	    {
	    ++ bad_pixels;		/*	  count it and		*/
	    pixel   = 0;		/*	  replace it with 0	*/
	    *(rgbp-1) &= ~pixel_tag;
	    }
	 ++ image->colormap[pixel].freq;/*  Increment color count for	*/
	 }				/*  this pixel's color		*/
   }

image->state |= cm_inited;		/*  Set state flag to show	*/
					/*  "color mgmnt initialized"	*/

if ( bad_pixels != 0 )			/*  If any pixels were invalid	*/
   {					/*     report how many were bad	*/
   fprintf ( stderr, "init_colman:  %d invalid pixels replaced by 0\n" );
   return 1;
   }

return 0;
}

/*1
===============================================================================
=======                                                                 =======
=======	    	     dither_rgb  --  Dither an RGB image		=======
=======                                                                 =======
===============================================================================

  Invocation:

	status = dither_rgb ( image, tree )


  Functions:

	Dither an RGB image, coercing assigned pixel values to match
	colors in the image's color map.  Original RGB values remain
	unchanged, but color index tags (if any) in each pixel are
	set to an appropriate color index.

	Specifying NULL as the color tree pointer requires generating
	the tree, which may be time-consuming.  Quantization routines
	call dither_rgb as their final pixel assignment phase when
	dithering is desired.


  Input:

	image		Img	*
			Pointer to Img structure for image to delete

	tree		Ctnode	*
			Pointer to root node of color tree


  Output:

	status		int
	<return value>	Completion status:
				0 => Failed
				1 => Succeeded
*/



int	 dither_rgb ( image, tree )

	Img	*image;		/*  Ptr to image to dither	*/
	Ctnode	*tree;		/*  Ptr to root of color tree	*/

{
/*------------------------------------------*/
/*	Assign color indices to pixels	    */
/*------------------------------------------*/

reg	Byte	 r, g, b;		/*  Current color		*/

	/*  Pointers into scanline buffers:	*/
reg	Rgbpix	*cp;			/*  Ptr to current-line pixel	*/
reg	Rgbpix	*np;			/*  Ptr to next-line pixel	*/
	/*  Pointers into image data:	*/
reg	Rgbpix	*icp;			/*  Ptr to current-line pixel	*/
reg	Rgbpix	*inp;			/*  Ptr to next-line pixel	*/

	Rgbpix	*cslb;			/*  Current scanline buffer	*/
	Rgbpix	*nslb;			/*  Next scanline buffer	*/
	Rgbpix	*tslb;			/*  Temporary pointer		*/

	int	 localtree;		/*  0 if caller supplied tree,	*/
					/*  1 if tree allocated here	*/

	int	 cmi;			/*  Color map index		*/
	Col	*cme;			/*  Color map entry pointer	*/
	int	 rerr, gerr, berr;	/*  Error in one pixel		*/
	int	 x, y;			/*  X/Y coordinates		*/



/*------------------------------------------------------*/
/*	Initialize to propagate dithering error		*/
/*	in temporary scan line buffers			*/
/*------------------------------------------------------*/

if ( img_dither_tables_uninitialized )	/*  If not initialized yet,	*/
   img_init_dither_tables ();		/*     initialize dither tables	*/

localtree = 0;
if ( tree == NULL )
   {
   /* &&&&  tree = classify_colors ( image );	/*  Build color tree	*/
   if ( tree == NULL )
      {
      fprintf ( stderr,
	"dither_rgb:  Only usable through quantization at present\n" );
      return 0;
      }
   localtree = 1;
   }


/*
	Allocate scanline buffers 2 pixels wider than line width
	to simplify error propagation logic
*/

cslb = (Rgbpix *) calloc ( image -> width + 2, sizeof(Rgbpix) );
nslb = (Rgbpix *) calloc ( image -> width + 2, sizeof(Rgbpix) );
if ( (cslb == NULL) || (nslb == NULL) )
   {
   fprintf (stderr, "dither_rgb:  Insufficient memory\n");
   if ( cslb != NULL ) free (cslb);
   return 0;
   }


/*
	Copy top line of image into 'next' scanline buffer,
	which will become the first 'cur' line processed.
*/

icp = (Rgbpix *) image -> pixdata;
inp = icp;
np  = &nslb[1];
for ( x=0; x < image->width; ++x )
    *np++ = *inp++;



/*----------------------*/
/*	Scan image	*/
/*----------------------*/

/*	np & inp were initialized above	*/

for ( y=0; y < image->height; ++y )
    {
    /*  Switch next-line and current-line buffers	*/

    tslb = cslb;    cslb = nslb;    nslb = tslb;

    cp = &cslb[1];		/*  Point to current-scanline buffer	*/
    np = &nslb[1];		/*  Point to next-scanline buffer	*/

    if  ( y < (image->height-1) )/*  If image has another scanline	*/
	{			/*	copy it into next- buffer	*/
	for ( x=0; x < image->width; ++x )
	    *np++ = *inp++;
	np = &nslb[1];
	}


    for ( x=0; x < image->width; ++x )
	{			/*  Translate each pixel on current	*/

	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*	Get actual RGB color of current pixel		*/
	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

	r = (*cp & pixel_red  ) >> pixel_red_shift;	/*  Get pixel's	*/
	g = (*cp & pixel_green) >> pixel_green_shift;	/*  color	*/
	b = (*cp & pixel_blue ) >> pixel_blue_shift;


	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*  Find lowest cube in tree that contains this color	*/
	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

	{
	reg Ctnode	*cur;		/*  "Current" node pointer	*/
	reg Ctnode	*prev;		/*  "Previous" node pointer	*/

	cur  = tree;			/*  Start at root node		*/
	prev = cur;

	while ( cur -> off != NULL )	/*  While any offspring exists	*/
	  {
	  cur = cur -> off;		/*  Go 1 level deeper		*/

	  while ((cur != NULL) && (prev != cur)) /*  Search siblings	*/
	      {
	      if (   (r >= cur->minred  ) && (r <= cur->maxred  )
		  && (g >= cur->mingreen) && (g <= cur->maxgreen)
		  && (b >= cur->minblue ) && (b <= cur->maxblue ) )
		 {
		 prev = cur;		/*  Save ptr to matching node	*/
		 break;
		 }
	      cur = cur -> sibling;
	      }

	  if ( prev != cur )		/*  If pixel wasn't found	*/
	     {				/*  at this depth		*/
	     				/*	find nearest node	*/
	     cur = nearest_color ( image, r, g, b, prev );
	     expand_color (prev, cur, r, g, b);
	     prev = cur;
	     break;
	     }
	  }

	cmi = prev -> cmi;		/*  Get color's map index	*/
	cme = &(image->colormap[cmi]);	/*  Get color's map ptr		*/
	}


	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*	Set pixel tag to color map index for this color	*/
	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

	*icp   &= pixel_red | pixel_green | pixel_blue;
	*icp++ |= (cmi << pixel_tag_shift);


	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*	Set error (difference between actual RGB	*/
	/*	and RGB in associated colormap entry)		*/
	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

	rerr = ((int) r) - ((int) cme -> red);
	gerr = ((int) g) - ((int) cme -> green);
	berr = ((int) b) - ((int) cme -> blue);


	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*	Propagate error forward to neighboring pixels	*/
	/*::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	properr(pix,tab) { \
	r = (pix & pixel_red  ) >> pixel_red_shift; \
	g = (pix & pixel_green) >> pixel_green_shift; \
	b = (pix & pixel_blue ) >> pixel_blue_shift; \
	r = img_cliptab[((int) r) + tab[rerr+256]]; \
	g = img_cliptab[((int) g) + tab[gerr+256]]; \
	b = img_cliptab[((int) b) + tab[berr+256]]; \
	pix =	  (r << pixel_red_shift) \
		| (g << pixel_green_shift) \
		| (b << pixel_blue_shift); \
	}

	++ cp;
	properr(*cp,img_errw7)		/*  7/16 of error -> right	*/

	-- np;
	properr(*np,img_errw3)		/*  3/16 of error -> lower left	*/
	++ np;

	properr(*np,img_errw5)		/*  5/16 of error -> below	*/

	++ np;
	properr(*np,img_errw1)		/*  1/16 of error -> lower right*/


	/*  Logic above has advanced all pixel pointers:  icp, cp, & np	*/
	}
    }


/*----------------------------------------------*/
/*	Free temporary storage and return	*/
/*----------------------------------------------*/

free (cslb);
free (nslb);

return 1;
}