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
=======           Generic  Image  Library  Definitions	               	=======
=======                     File  imglib.h                              =======
=======                                                                 =======
===============================================================================


        This header file supplies image library definitions
	for public access by programs using imglib functions.
	Definitions here are independent of the underlying
	window system, graphics layer, or operating system.

	*********************************************************
	****  WARNING:  These functions currently require    ****
	****  a machine with at least a 32-bit word length.  ****
	*********************************************************

	Programs using imglib under X11 should include both
	this header file and imgXlib.h.


  File history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-22-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.
	Changes not documented in this header have been made gradually
	since creation of this header file.

	3-14-89		Paul Raveling
	Created file, based partly on older utility definitions.

*/





/*  -----------------------------------------------------------  */
/*  -----------------------  Parameters  ----------------------  */
/*  -----------------------------------------------------------  */

#define	CUR_SCMI_VERSION  1	/*  Current version of SCMI file format	*/

#ifndef	NULL
#define	NULL	0		/*  NULL, if not already defined	*/
#endif


/*::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Orientation codes for printed images	  */
/*::::::::::::::::::::::::::::::::::::::::::::::::*/

#define	PIO_DEFAULT	0		/*  Default:  Choose best of:	*/
#define	PIO_PORTRAIT	1		/*   Portrait (unrotated)	*/
#define	PIO_LANDSCAPE	2		/*   Landscape (rotated 90-deg)	*/



/*  -----------------------------------------------------------  */
/*  -------------------  Constant Definitions -----------------  */
/*  -----------------------------------------------------------  */

/*::::::::::::::::::::::::::::::::::::::*/
/*	RGB (32-bit) Pixel Fields	*/
/*::::::::::::::::::::::::::::::::::::::*/

#define	  pixel_tag	0xff000000	/*  Tag			*/
#define	  pixel_red	0x00ff0000	/*  Red   component	*/
#define	  pixel_green	0x0000ff00	/*  Green component	*/
#define	  pixel_blue	0x000000ff	/*  Blue  component	*/

	/*
		Tag is data temporarily associated with pixels
		during particular processing functions
	*/

/*  Shift counts to align pixel components:	*/

#define	  pixel_tag_shift	24	/*  Tag			*/
#define	  pixel_red_shift	16	/*  Red   component	*/
#define	  pixel_green_shift	 8	/*  Green component	*/
#define	  pixel_blue_shift	 0	/*  Blue  component	*/




/*::::::::::::::::::::::::::::::::::::::*/
/*	Image (Img) Field Values	*/
/*::::::::::::::::::::::::::::::::::::::*/

/*	imgtype,	Type of image:				*/

#define	  mapped_image	0		/*  Color mapped	*/
#define	  mapped_info	1		/*  Info (no pixel data)*/
#define	  rgb_image	2		/*  32-bit RGB, no map	*/
#define	  rgb_info	3		/*  Info (no pixel data)*/
		/*  Values used only as parameters to read_img:	*/
#define	  any_image	4		/*  Any type, with data	*/
#define	  any_info	5		/*  Any type, no data	*/

/*	alloc,		Resources bound to image:			*/
#define	  asdata_mem	0x00000001	/*  Memory for associated data	*/
#define	  colmap_mem	0x00000002	/*  Memory for color map	*/
#define	  pixdata_mem	0x00000004	/*  Memory for pixel data	*/
#define	  sys_resources	0x00000008	/*  System [X11] resources	*/
#define	  app_resources	0xffff0000	/*  Application-resources,	*/
					/*	  local to application	*/

/*	state,		Image states:				*/
#define	  cm_inited	0x0001		/*  Color managment initialized	*/




/*  -----------------------------------------------------------  */
/*  --------------------     Simple Types    ------------------  */
/*  -----------------------------------------------------------  */

typedef	unsigned char	 Byte;		/*  Byte:  Unsigned char	*/
typedef	unsigned short	 Shord;		/*  Shord: Short word		*/
typedef	unsigned int	 Word;		/*  Word:  Unsigned integer	*/
typedef	Word		 Rgbpix;	/*  Pixel resides in word	*/
					/*  ** Word size >= 32 bits!!!	*/



/*  -----------------------------------------------------------  */
/*  -----------------------  Structures  ----------------------  */
/*  -----------------------------------------------------------  */

/*::::::::::::::::::::::::::::::::::*/
/*	Col:  Color Definition	    */
/*::::::::::::::::::::::::::::::::::*/

typedef struct imglib_color
	{
	Word	 freq;		/*  Frequency:  # pixels using color	*/
	Byte	 pixel;		/*  Color map index	*/
	Byte	 red;		/*  Red   intensity	*/
	Byte	 green;		/*  Green intensity	*/
	Byte	 blue;		/*  Blue  intensity	*/
	} Col;



/*::::::::::::::::::::::::::::::::::::*/
/*	Colmap:  Color Map Info	      */
/*::::::::::::::::::::::::::::::::::::*/

typedef struct imglib_mapinfo
	{
	struct imglib_mapinfo
		*next_map;	/*  Next map in image's map queue	*/
	Col	*map;		/*  Pointer to color map (Col array)	*/
	Byte	 mapcolors;	/*  Number of colors in map		*/
	Byte	 mapunique;	/*  Number of unique colors in map	*/
	} Mapinfo;




/*::::::::::::::::::::::::::::::::::::::*/
/*	Geom:  Geometry Definition	*/
/*::::::::::::::::::::::::::::::::::::::*/

typedef struct imglib_geom
	{
	int	 width;		/*  Width, pixels	*/
	int	 height;	/*  Height, pixels	*/
	int	 xoff;		/*  X offset		*/
	int	 yoff;		/*  Y offset		*/
	} Geom;




/*:::::::::::::::::::::::*/
/*	Img:  Image	 */
/*:::::::::::::::::::::::*/

typedef struct	imglib_image
	{
	int	 imgtype;	/*  Type of image		*/

	int	 width;		/*  Width  of image (# pixels)	*/
	int	 height;	/*  Height of image (# pixels)	*/

	int	 asdata_length;	/*  Length of associated data	*/
	Byte	*asdata;	/*  Pointer to comment text	*/
				/*	(NULL if none)		*/

	int	 num_colors;	/*  Number of colors used	*/
	Col	*colormap;	/*  Pointer to color map in use	*/
	Mapinfo	*cmiq;		/*  Head(color map info queue)	*/

	int	 num_pixels;	/*  Number of pixels in image	*/
	Byte	*pixdata;	/*  Pointer to pixel data	*/

	    /*  References to pixel data must be cast to (Rgbpix)
		when working with 32-bit RGB images		*/


	/*	Data meaningful only during processing		*/

	char	*sysres;	/*  System [X11] image resources;	*/
				/*  Cast type as required in references	*/

	int	 alloc;		/*  Resources allocated to image (flags)*/
	int	 state;		/*  State flags				*/

	Word	 aptdata[8];	/*  Hooks for application	*/
				/*    or toolkit data,		*/
	} Img;


/*------------------------------------------------------------------*/
/*	Color tree node, used by quantization and dithering	    */
/*------------------------------------------------------------------*/

typedef struct	imnode
	{
	struct imnode	*parent;	/*  Pointer to parent node	*/
	struct imnode	*off;		/*  Pointer to 1st offspring node */
	struct imnode	*sibling;	/*  Pointer to next sibling	*/
	Byte		 minred;	/*  Minimum red coordinate	*/
	Byte		 maxred;	/*  Maximum red coordinate	*/
	Byte		 mingreen;	/*  Minimum green coordinate	*/
	Byte		 maxgreen;	/*  Maximum green coordinate	*/
	Byte		 minblue;	/*  Minimum blue coordinate	*/
	Byte		 maxblue;	/*  Maximum blue coordinate	*/
	Byte		 wti;		/*  Weighted pseudo-intensity	*/
	Byte		 cmi;		/*  Color map index (if any)	*/
	Word		 numcols;	/*  Number of colors in cube	*/
	Word		 numuniq;	/*  # colors unique to node	*/
	Word		 sumred;	/*  Sum of red components	*/
	Word		 sumgreen;	/*  Sum of green components	*/
	Word		 sumblue;	/*  Sum of blue components	*/
	} Ctnode;



/*:::::::::::::::::::::::::::::::::::::::::::::::::*/
/*	Yafile:  Yet Another File Definition	   */
/*:::::::::::::::::::::::::::::::::::::::::::::::::*/
/*
	To be used with open_yafile, close yafile,
	and otherwise ordinary stream i/o
*/

typedef struct	imglib_file
	{
	FILE	 real_stream;		/*  Stream control blk	*/
	int	 compressed;		/*  Compressed flag:	*/
					/*	0 => Not compressed	*/
					/*	1 => Compressed		*/
	char	*actual_name;		/*  Actual filename	*/
	} Yafile;



/*  -----------------------------------------------------------  */
/*  --------------------  Public Procedures  ------------------  */
/*  -----------------------------------------------------------  */

extern	int	 change_img ( /* imgtype, image */ );
		/*  Change image			Module imgcredel */

extern	void	 close_yafile ( /* yafile */ );
		/*  Close yet another file		Module fileutil	*/

extern	void	 color_to_gray ( /* image */ );
		/*  Convert color image to grayscale	Module imgcvt	*/

extern	int	 color_to_mono ( /* image */ );
		/*  Convert color image to monochrome	Module imgcvt	*/

extern	Img	*copy_img ( /* imgtype, image */ );
		/*  Copy an image			Module imgcredel */

extern	Img	*create_img ( /* imgtype, width, height, asdlen, asdata,
		      num_colors, colormap, pixdata */ );
		/*  Create an image			Module imgcredel */

extern	void	 delete_img ( /* image */ );
		/*  Delete an image			Module imgcredel */

extern	void	 describe_img ( /* image, outfile, format, heading */ );
		/*  Describe an image			Module imgio	*/

extern	Byte	*expand_path ( /* csh_path */ );
		/*  Expand file pathname		Module fileutil	*/

extern	Ctnode	*img_rep_node ();
		/*  Replensish color tree node supply	Module imgcolor	*/

extern	int	 init_colman ( /* image */ );
		/*  Initialize for color management	Module imgcolor	*/

extern	Yafile 	*open_yafile ( /* filename, options, suffix */ );
		/*  Open yet another file		Module fileutil	*/

extern	void	print_img ( /* image, prfile, orientation, docname */ );
		/*  Print image				Module imagen	*/

extern	int	 quant_cmi ( /* image, num_colors */ );
		/*  Quantize color mapped image		Module quantcmi	*/

extern	int	 quant_rgb ( /* image, num_colors */ );
		/*  Quantize RGB image			Module quantrgb	*/

extern	Img	*read_img ( /* filename, imgtype */ );
		/*  Read image from file		Module imgio	*/

extern	int	 write_img ( /* filename, image */ );
		/*  Write image to file			Module imgio	*/


/*
	Routines in xrand -- random number generator
	(c) Copyright 1988 by A. Nowatzyk:  See xrand.c
*/

extern		 rnd_init ( /* seed */ );
		/*  Seed random number generation	Module xrand	*/

extern	long	 rnd_i ();
		/*  Return 32-bit signed integer	Module xrand	*/

extern	unsigned long	 rnd_u ();
		/*  Return 32-bit unsigned integer	Module xrand	*/

extern	long	 rnd_ri ( /* max */ );
		/*  Return integer in range [0,max-1]	Module xrand	*/

extern	double	 rnd_01d ();
		/*  Return d.p. float in range [0,1)	Module xrand	*/

extern	double	 rnd_ned ( /* lam */ );
		/*  Return neg exponentially distr'd	Module xrand	*/
		/*  d.p. float in range [0,infinity)			*/



