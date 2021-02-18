/* readump.c - read an XWD file */

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XWDFile.h>


char   *calloc ();

/*  */

/* Reads in XWDFileHeader, window name, and possibly raster
	only supports XYFormat and one plane for now... */


int	XReadDumpFile (filename, xwd, name, raster)
char   *filename;
XWDFileHeader *xwd;
char  **name;
short **raster;
{
    int     i;
    FILE   *fp;

    if ((fp = fopen (filename, "r")) == NULL)
	return 0;

    i = XReadDumpFP (fp, xwd, name, raster);

    fclose (fp);

    return i;
}

/*  */

int	XReadDumpFP (fp, xwd, name, raster)
FILE   *fp;
XWDFileHeader *xwd;
char  **name;
char	**raster;
{
    int	    i;
    char   *cp,
	   *np;
    XColor	*colors;

    if (fread ((char *) xwd, sizeof *xwd, 1, fp) != 1)
	return (-1);

    if (xwd -> file_version != XWD_FILE_VERSION
	    && (xwd -> file_version != 7 || xwd -> pixmap_depth != 1))
	return (-2);

    if ((i = xwd -> header_size - sizeof *xwd) < 0)
	i = -i;
    if (name == NULL)
	name = &np;
    if ((*name = calloc ((unsigned) i, sizeof **name)) == NULL)
	return (-3);
    if (fread (*name, sizeof **name, i, fp) != i) {
	free (*name);
	return (-4);
    }

    if (raster == NULL)
	goto done;

    if (xwd -> pixmap_format != XYPixmap
	    || xwd -> pixmap_depth != 1
	    || xwd -> ncolors != 2) {
	free (*name);
	return (-5);
    }

    i = xwd->ncolors * sizeof(XColor);
    colors = (XColor *)malloc((unsigned)(i));
    
    if( colors == NULL )
    {
	free (*name);
	return (-6);
    }
    if (fread ((char *)colors, sizeof (char), i, fp) != i) {
	free (colors);
	free (*name);
	return (-7);
    }
    
    i = xwd->bytes_per_line * xwd->pixmap_height;
    cp = (char *)malloc(i);
    
    if( cp == NULL )
    {
	free(*name);
	free(colors);
	return(-8);
    }
    if( fread(cp, sizeof(char), i, fp) != i )
    {
	free(*name);
	free(colors);
	free(cp);
	return(-9);
    }
    *raster = cp;

done: ;
    if (name == &np)
	free (*name);
    return (1);
}

