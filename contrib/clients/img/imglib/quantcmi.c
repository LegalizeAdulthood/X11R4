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
======			      Module Quantcmi				======
======			Quantize Color Mapped Images			======
======									======
==============================================================================

  Function:

	This module contains two subprograms to quantize a color-mapped
	image, so that it uses a smaller number of colors.


  Public subprograms:

	quant_cmi	Quantize color mapped image


  Private subprograms:

	prune		Prune subtree of color classification tree


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made
	within the last 3 weeks.

	3-25-89		Paul Raveling
	Created this module by modifying a copy of alcols.

*/


#define	MAXRGB		255	/*  Maximum RGB component value		*/

#define	MAXINT	2147483647	/*  Maximum integer, to suit hardware	*/

#define	MAXATS		10000	/*  Maximum adaptive tree size,
					total number of nodes		*/

#include <sys/types.h>
#include <stdio.h>
#include <imglib/imglib.h>

				/*  Specialized NULL pointer values:	*/

#define	BNULL (struct mem_block *) NULL
#define	NNULL (Ctnode *) NULL


static	Ctnode	*cube_root;	/*  Root of color cube tree		*/

static	int	 num_colreq;	/*  Number of colors required		*/

static	int	 coltdepth;	/*  Assigned color tree depth		*/



	/*----------------------------------------------*/
	/*  Memory suballocation data for color trees	*/
	/*	--  Shared with color quantization	*/
	/*----------------------------------------------*/

 struct	 mem_block			/*  Memory block from malloc	*/
	 {
	 struct mem_block  *next_block;		/*  Link to next block	*/
	 Ctnode		    node_space[1000];	/*  Room for 1000 nodes	*/
	 };

extern	struct mem_block  *img_block_queue;	/*  Queue of malloc'd blks  */
extern	struct mem_block  *img_new_block;	/*  Pointer to new block    */
extern	Ctnode	*img_next_node;			/*  Ptr to next free byte   */
extern	int	 img_num_free;		/*  Remaining free space, as
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



/*::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to find next node of a tree	*/
/*	using depth-first search and		*/
/*	tracking depth in tree			*/
/*::::::::::::::::::::::::::::::::::::::::::::::*/

#define	dfnext(node,depth) \
 { \
 if (node->off != NNULL ) \
    { \
    node = node->off; \
    ++ depth; \
    } \
 else \
    { \
    if (node->sibling != NNULL) \
	node = node->sibling; \
    else \
	{ \
	node = node->parent; \
	-- depth; \
        while (depth >= 0) \
		{ \
		if (node->sibling != NNULL) \
		   { \
		   node = node->sibling; \
		   break; \
		   } \
		node = node ->parent; \
		-- depth; \
		} \
	 } \
    } \
 }


/*::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to find next node of a tree	*/
/*	using depth-first search without	*/
/*	tracking depth in tree			*/
/*::::::::::::::::::::::::::::::::::::::::::::::*/

#define	dfnextnl(node) \
 { \
 if (node->off != NNULL ) \
    node = node->off; \
 else \
    { \
    if (node->sibling != NNULL) \
	node = node->sibling; \
    else \
	{ \
	node = node->parent; \
        while (next != NNULL) \
		{ \
		if (node->sibling != NNULL) \
		   { \
		   node = node->sibling; \
		   break; \
		   } \
		node = node ->parent; \
		} \
	 } \
    } \
 }


/*2
******************************************************************************
******									******
******		          prune  --  Prune subtree			******
******									******
******************************************************************************

  Invocation:

	prune ( node_ptr )


  Function:

	Delete the given node and all of its offspring.

	Steps in deleting an individual node are:

	1.  Merge color statistics into parent node
	2.  Delete all offspring.
	3.  If the node is first on sibling list
	       update parent's offspring pointer to next sibling
	4.  Delete the node from its sibling list.
	5.  Release the node's memory.

*/

static
prune ( node_ptr )

	Ctnode	*node_ptr;
{
register Ctnode	*next;
register Ctnode	*prev;
register Ctnode	*parent;



/*----------------------------------------------*/
/*	Prune all offspring of this node	*/
/*----------------------------------------------*/

while ( node_ptr -> off != NNULL )
      prune ( node_ptr -> off );


/*----------------------------------------------*/
/*	Merge color statistics into parent	*/
/*----------------------------------------------*/

parent = node_ptr -> parent;
parent -> numuniq  += node_ptr -> numuniq;
parent -> sumred   += node_ptr -> sumred;
parent -> sumgreen += node_ptr -> sumgreen;
parent -> sumblue  += node_ptr -> sumblue;


/*------------------------------------------------------*/
/*	Remove current node from its sibling list	*/
/*	or from its parent's offspring pointer		*/
/*------------------------------------------------------*/

next   = node_ptr -> sibling;
prev   = parent   -> off;

if ( prev == node_ptr )			/*  If node is head of sibling list  */
   parent -> off = next;		/*     change parent's offspring ptr */
else
   {
   /*
	Find node's predecessor on sibling list and queue around node.
	Note that the check above guarantees existence of a predecessor
	on the sibling list.
   */

   while ( prev -> sibling != node_ptr )
	 prev = prev -> sibling;

   prev -> sibling = next;
   }

}


/*1
******************************************************************************
******									******
******		    quant_cmi  --  Quantize Color Mapped Image		******
******									******
******************************************************************************

  Invocation:

	status = quant_cmi ( image, num_colors )


  Function:


	Quantize colors for an image.  This replaces the image's
	color map with a smaller map and midifies its pixel data
	to index the new map.

	The old color map is discarded and a new one is allocated.
	Pixel data is updated in place.

	The actual number of colors in the quantized map can be
	slightly less than the number requested.


  Input:

	image		Img *
			Pointer to image to quantize

	num_colors	int
			Maximum number of colors to be retained
			in the image


  Output:

	status		int
	<return value>	Completion status:
				0 => Failed
				1 => Succeeded
*/


int	quant_cmi ( image, num_colors )

	Img	*image;			/*  Ptr to Img structure	*/
	int	 num_colors;		/*  Max # of colors to retain	*/

{
	int	 i;		/*  Pixel index				*/
	int	 local_csrf[9];	/*  Level-specific (aggregate) csrf	*/
	int	 pt;		/*  Basic pruning threshold		*/
	int	 local_pt;	/*  Local threshold for current level	*/
	int	 next_pt;	/*  Next pruning theshold		*/
	int	 next_local_pt;	/*  Next theshold for current level	*/
	int	 cur_level;	/*  Current level in tree		*/

	Ctnode	*cur;		/*  Current node	*/
	Ctnode	*next;		/*  Next node		*/
	Ctnode	*prev;		/*  Previous node	*/
	int	 level;		/*  Level in tree	*/
	int	 li;		/*  Level index		*/

	Byte	 r;		/*  Pixel red   value	*/
	Byte	 g;		/*  Pixel green value	*/
	Byte	 b;		/*  Pixel blue value	*/
	Byte	 mp;	/*  Midpoint of a color range	*/

	Col	*cp;		/*  Color pointer	*/
	Byte	*pp;		/*  Pixel pointer	*/

	int	 colc;		/*  Color count		*/
	Col	*oldmap;	/*  Pointer to old color map	*/
	Col	*newmap;	/*  Pointer to new color map	*/



/*::::::::::::::::::::::::::::::::::::::*/
/*	Macro to clean up & abort	*/
/*::::::::::::::::::::::::::::::::::::::*/

#define	abort(msg) { \
 fprintf (stderr, "quantcmi:  %s\n", msg); \
 while ( img_block_queue != BNULL ) \
	{ \
	img_new_block = img_block_queue -> next_block; \
	free ( img_block_queue ); \
	img_block_queue = img_new_block; \
	} \
 return 0; }



/*--------------------------------------------------*/
/*	Initialize tree to describe color cube	    */
/*--------------------------------------------------*/

img_block_queue = BNULL;	/*  Init memory block queue as empty	*/
img_num_free    = 0;		/*  Indicate no free blocks		*/
coltdepth   = 8;		/*  Generate 8-deep tree		*/

get_node(cube_root);		/*  Allocate root node		*/
if ( cube_root == NNULL )
   abort("Insufficient memory to quantize image")
cube_root->maxred   = MAXRGB;
cube_root->maxgreen = MAXRGB;		/*  Set root color range	*/
cube_root->maxblue  = MAXRGB;

if ((image->state & cm_inited) == 0)	/*  If not already done,	*/
   init_colman (image);			/*     init color management	*/




/*--------------------------------------------------------------*/
/*	For each original color, locate or allocate a node	*/
/*	at each level of the color cube tree which contains	*/
/*	this color and update the count and sum values for	*/
/*	colors in that node.					*/
/*--------------------------------------------------------------*/

for ( i=0; i < image->num_colors; ++i )
    {					/*  For each color...		*/
    cp = &(image->colormap[i]);

    r = cp -> red;
    g = cp -> green;			/*  Get RGB values		*/
    b = cp -> blue;
    colc = cp -> freq;			/*  Get frequency		*/

    cur = cube_root;			/*  Start at root node		*/

    for ( li=0; li < coltdepth; ++ li )
	{	/*  For each level of color tree ...	*/

	level = li + 1;

	if ( cur->off == NNULL )	/*  If nothing on level		*/
					/*	Create new level	*/

	   {				/*	Create new level	*/
	   get_node(next)
	   if ( next == NNULL )
	      abort("Insufficient memory to quantize image")
	   next->parent = cur;		/*  Create 1st element		*/
	   cur->off     = next;

	   /*  Set colors of 1st element to contain pixel	*/

	   mp = cur->minred + (cur->maxred - cur->minred)/2;
	   if ( r <= mp )
	      {
	      next->minred = cur->minred;
	      next->maxred = mp;
	      }
	   else
	      {
	      next->minred = mp+1;
	      next->maxred = cur->maxred;
	      }

	   mp = cur->mingreen + (cur->maxgreen - cur->mingreen)/2;
	   if ( g <= mp )
	      {
	      next->mingreen = cur->mingreen;
	      next->maxgreen = mp;
	      }
	   else
	      {
	      next->mingreen = mp+1;
	      next->maxgreen = cur->maxgreen;
	      }

	   mp = cur->minblue + (cur->maxblue - cur->minblue)/2;
	   if ( b <= mp )
	      {
	      next->minblue = cur->minblue;
	      next->maxblue = mp;
	      }
	   else
	      {
	      next->minblue = mp+1;
	      next->maxblue = cur->maxblue;
	      }

	   }				/*	Create new level	*/

        cur = cur -> off;		/*  Point to 1st entry on level	*/

	while ( cur != NNULL )		/*  Search siblings at this level  */
	      {
	      prev = cur;
	      if (   (r >= cur->minred  ) && (r <= cur->maxred  )
		  && (g >= cur->mingreen) && (g <= cur->maxgreen)
		  && (b >= cur->minblue ) && (b <= cur->maxblue ) )
		 break;
	      cur = cur->sibling;
	      }

	if ( cur == NNULL )		/*  If no node for pixel's color,  */
	   {				/*	Create new sibling	*/
	   get_node(cur)
	   if ( cur == NNULL )
	      abort("Insufficient memory to quantize image")
	   prev->sibling = cur;
	   prev		 = prev->parent;
	   cur->parent   = prev;


	   /*  Set colors of new element to contain pixel	*/

	   mp = prev->minred + (prev->maxred - prev->minred)/2;
	   if ( r <= mp )
	      {
	      cur->minred = prev->minred;
	      cur->maxred = mp;
	      }
	   else
	      {
	      cur->minred = mp+1;
	      cur->maxred = prev->maxred;
	      }

	   mp = prev->mingreen + (prev->maxgreen - prev->mingreen)/2;
	   if ( g <= mp )
	      {
	      cur->mingreen = prev->mingreen;
	      cur->maxgreen = mp;
	      }
	   else
	      {
	      cur->mingreen = mp+1;
	      cur->maxgreen = prev->maxgreen;
	      }

	   mp = prev->minblue + (prev->maxblue - prev->minblue)/2;
	   if ( b <= mp )
	      {
	      cur->minblue = prev->minblue;
	      cur->maxblue = mp;
	      }
	   else
	      {
	      cur->minblue = mp+1;
	      cur->maxblue = prev->maxblue;
	      }

	   }				/*	Create new sibling	*/

	/*
		cur now points to an existing node
		representing the pixel's color.
	*/

        ++ cur->numcols;		/*  Count colors in node	*/

	if ( level == coltdepth )	/*  If this is a leaf...	*/
	   {
	   cur->numuniq  += colc;	/*	Increment unique color count */
	   cur->sumred   += r*colc;	/*	Sum rgb values for	*/
	   cur->sumgreen += g*colc;	/*	later derivation of	*/
	   cur->sumblue  += b*colc;	/*	mean cube color		*/
	   }

	}	/*  For each level of color tree ...	*/
    }				/*  For each pixel	*/

/*
	Set total number of color points (number of pixels)
	in root node.
*/

cube_root->numcols = image -> num_pixels;


/*----------------------------------------------------------------------*/
/*	Select colors from the color tree by reducing the tree		*/
/*----------------------------------------------------------------------*/

next_pt = 1;			/*  Initial pruning threshold = 1	*/
num_colreq = 0x7fffffff;

/*
	Build table of local structure retention factors
	for each level of the color tree.

	Factor for lowest level (level coltdepth) is 1.0,
	represented by 4 (numerator of 4/4).  Each higher level
	uses 1.25 * value for level immediately below.
*/

local_csrf[coltdepth] = 4;

for ( i = coltdepth-1; i>=0; --i )
    {
    local_csrf[i] = (local_csrf[i+1] * 5) >> 2;
    if ( local_csrf[i] == 0 )
       local_csrf[i] = 1;
    }



			/*  While tree would produce too many colors	*/
while ( num_colreq > num_colors )	/*  reduce tree iteratively	*/
{
cur        = cube_root;		/*  Traverse tree for each iteration	*/
cur_level  = 0;

pt	   = next_pt;		/*  Set new pruning threshold		*/
local_pt   = (pt * local_csrf[0]) >> 2;
				/*  Preset ...				*/
num_colreq = 0;			/*  Count of colors it represents	*/
next_pt    = MAXINT >> 2; 	/*  Preset next pruning thresh to max   */



while (cur != NNULL)
      {
      /*	Set local pruning thresholds for this level of tree	*/

      local_pt      = (pt << 2) / local_csrf[cur_level] + 1;
      next_local_pt = (next_pt << 2) / local_csrf[cur_level] + 1;


      /*  Decide whether to prune this node	*/

      if ( cur->numcols <= local_pt )
	 {
	 /*
		Locate next node at or above this level,
		then prune this node and any offspring it may have.
	 */

	 if ( cur->sibling != NNULL )	/*  If a sibling exists	*/
	    next = cur->sibling;	/*	it's next	*/
	 else				/*  Else move up and across	*/
	    {
	    next = cur->parent;
	    -- cur_level;
	    while ( next != NNULL)
		  {
		  if ( next->sibling != NNULL )
		     {
		     next = next->sibling;
		     break;
		     }
		  next = next -> parent;
		  -- cur_level;
		  }
	    }

	 if ( cur->off != NNULL )	/*  If node has offspring	*/
	    prune ( cur->off );		/*     merge their statistics	*/
	 if (   (cur->numuniq != 0)	/*  If node has unique colors	*/
	     && (cur->parent->numuniq == 0) ) /*   and parent doesn't,	*/
	    ++ num_colreq;		/*     count a required color	*/

	 prune ( cur );			/*  Prune current node		*/
	 cur = next;			/*  Advance to next node	*/
	 }

      else			/*  If this node shouldn't be pruned...	*/
	 {
	 if ( cur->numcols < next_local_pt )  /*  If node has few colors  */
	    {				/*  Reset next pruning threshold  */
	    next_local_pt = cur->numcols;
	    if ( cur_level == coltdepth )
	       next_pt = next_local_pt;
	    else
	       next_pt = (next_local_pt * local_csrf[cur_level])
		         / local_csrf[coltdepth] + 1;
	    }


	 if ( cur->numuniq != 0 )	/*  If node has unique colors	*/
	    ++ num_colreq;		/*     count a required color	*/

	 /*	Advance to next node, offspring allowed	  */

	 dfnext(cur,cur_level)
	 }

      }

}




/*----------------------------------*/
/*	Allocate new color map	    */
/*----------------------------------*/

/*
	First, scan tree to determine number of colors for new map.
	Any node with a nonzero unique color count will generate a color.
*/

cur  = cube_root;		/*  Start at root of tree		*/
colc = 0;			/*  Start at element 0 of color map	*/
cur_level = 0;

while ( cur != NNULL )
      {
      if ( cur->numuniq != 0 )		/*  If node has unique colors	*/
	 ++colc;			/*     increment color count	*/
      dfnext(cur, cur_level)		/*  Locate next node		*/
      }


oldmap = image -> colormap;		/*  Save pointer to old map	*/
newmap = (Col *) calloc ( colc, sizeof(Col) );	/*  Allocate new map	*/
if ( newmap == NULL )
   abort("Insufficient memory to quantize image")



/*--------------------------------------------------*/
/*	Set color definitions in new color map	    */
/*--------------------------------------------------*/

cur = cube_root;		/*  Start at root of tree		*/
i   = 0;			/*  Start at element 0 of color map	*/
cur_level = 0;

while ( cur != NNULL )
      {
      /*
	If current node's unique color count is nonzero,
	   then allocate a color for this node.
      */

      if ( cur->numuniq != 0 )		/*  If node has unique colors	*/
	 {				/*	define a color for it	*/
         cp = &newmap[i];
	 cp -> red   = cur->sumred   / cur->numuniq;
	 cp -> green = cur->sumgreen / cur->numuniq;
	 cp -> blue  = cur->sumblue  / cur->numuniq;
	 cp -> freq  = cur->numuniq;
	 cp -> pixel = i;
	 cur->numcols = i;		/*  Save color map index in numcols */
	 ++i;				/*  Incr count of colors allocated  */
	 }

      dfnext(cur,cur_level)		/*	Locate next node	*/
      }



/*-----------------------------------------------------------------*/
/*	Identify how to remap pixels, from old map to new map	   */
/*-----------------------------------------------------------------*/

for ( i=0; i < image->num_colors; ++i )
    {
    cp = &oldmap[i];
    r = cp -> red;
    g = cp -> green;
    b = cp -> blue;
    cur  = cube_root;			/*  Start at root node		*/
    prev = cur;

    /*  Find lowest cube in tree that contains this (old) color		*/

    while ( cur -> off != NNULL )	/*  While any offspring exists	*/
	  {
	  cur = cur -> off;		/*  Go 1 level deeper		*/

	  while ( (cur != NNULL) && (prev != cur) ) /*  Search siblings	*/
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

	  if ( prev != cur )		/*  Escape if pixel was		*/
	     break;			/*  not found at this depth	*/
	  }

    cp -> pixel = prev->numcols;	/*  Note new pixel index	*/
    }



/*----------------------------------------*/
/*	Remap pixels to use new map	  */
/*----------------------------------------*/

pp = image->pixdata;
i  = image->num_pixels;

while ( i-- > 0 )
      *pp++ = oldmap[*pp].pixel;




/*------------------------------------------------------*/
/*	Release storage used by the color cube tree	*/
/*------------------------------------------------------*/

while ( img_block_queue != BNULL )
      {
      img_new_block = img_block_queue -> next_block;
      free ( img_block_queue );
      img_block_queue = img_new_block;
      }



/*-------------------------------*/
/*	Finish housekeeping	 */
/*-------------------------------*/

if ( image->alloc & colmap_mem )	/*  If Img owns old map		*/
   free (oldmap);			/*     Release its memory	*/

image -> num_colors = colc;		/*  Save # colors allocated	*/
image -> colormap   = newmap;		/*  Save pointer to new map	*/
image -> alloc |= colmap_mem;		/*  Mark color map owned by Img	*/

(image->cmiq) -> map = newmap;		/*  Store new map's info in	*/
(image->cmiq) -> mapcolors = colc;	/*  entry at head of queue	*/
(image->cmiq) -> mapunique = colc;

return 1;				/*  Return "success" status	*/
}
