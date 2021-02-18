/*	@(#)rasterfile.h 1.3 84/01/06 SMI	*/

/*
 * Description of header for files containing raster images
 */
struct rasterfile {
	int	ras_magic;		/* magic number */
	int	ras_width;		/* width (pixels) of image */
	int	ras_height;		/* height (pixels) of image */
	int	ras_depth;		/* depth (1, 8, or 24 bits) of pixel */
	int	ras_encoding;		/* raw (0), run-length (1) */
	int	ras_type;		/* should be set to 0 for now */
	int	ras_maptype;		/* type of colormap */
					/* 1 = red[ras_maplength/3],green[...*/
	int	ras_maplength;		/* length (bytes) of following map */
	/* color map follows for ras_maplength bytes, followed by image */
};
#define	RAS_MAGIC	0x59a66a95

/*
 * NOTE:  Each line of the image is rounded out to a multiple
 *        of 16-bits.  This corresponds to the rounding convention
 *        used by the memory pixrect package (/usr/include/pixrect/memvar.h)
 *        of the SunWindows system.
 */

