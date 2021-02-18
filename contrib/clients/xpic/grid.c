/* $Header: grid.c,v 1.4 89/07/02 15:58:17 moraes Exp $ */
#include <stdio.h>
#include <sys/types.h>
#include <X11/Xos.h>

/* size in bytes of a bitmap */
#define BitmapSize(width, height) ((((width) + 7) * (height))/8)

/*
 *  Makes a bitmap of the right size for the given grid-point spacing, 5
 *  grid points to a cross
 */
char *makegrid(sep, size, width, height)
int sep;
int *size;
int *width;
int *height;
{
	char *data;
	register int i, j;
	extern char *calloc();
	
	if (sep < 4) {
		message("makegrid: Separation too small");
		return( (char *) NULL);
	}

	(*width) = (*height) = sep * 5;
	(*size) = BitmapSize(*width, *height);
	if ((data = (char *) calloc((unsigned) *size, (unsigned) 1)) == NULL) {
		message("makegrid: Not enough memory for grid");
		return( (char *) NULL);
	}

	/* For each vertical grid point */
	for(j = 0; j < (*height); j += sep) {
		/* For each horizontal grid point */
		for (i = 0; i < (*width); i += sep) {
			/* Generate the grid point */
			setbit(data, *width, i, j);
		}
	}

	/* Do the special stuff at the cross */
	*data |= 0x0007;				/* first 3 bits in the top line */

	setbit(data, *width, (*width) - 2, 0);	/* last 2 bits in the top line */
	setbit(data, *width, (*width) - 1, 0);

	setbit(data, *width, 0, 1);				/* first bit in second and third */
	setbit(data, *width, 0, 2);				/* lines */

	setbit(data, *width, 0, (*height) - 2);	/* first bit in the last but one */
	setbit(data, *width, 0, (*height) - 1);	/* and last lines */

	return(data);
}

setbit (raster, width, x, y)
char *raster;
register int x;
int y;
{
	register char *byte = raster + x/8 + y*((width+7)/8);
	x %= 8;
	*byte |= (1 << x);
}



#ifdef TEST
main(argc, argv)
int argc;
char **argv;
{
	FILE *file = stdout;
	char *fname;
	register int i;
	int spacing = 8;
	char *bitmap;
	int bitmap_size, bitmap_wide, bitmap_high;
	

	if (argc > 1) {
		fname = argv[1];
		if ((file = fopen(fname, "w")) == NULL) {
			error("Can't open file");
			/*NOTREACHED*/
		}
	}
	if (argc > 2)
		spacing = atoi(argv[2]);

	bitmap = makegrid(spacing, &bitmap_size, &bitmap_wide, &bitmap_high);

	(void) fprintf (file, "#define grid%d_width %d\n", spacing, bitmap_wide);
	(void) fprintf (file, "#define grid%d_height %d\n", spacing, bitmap_high);
	(void) fprintf (file, "static char grid%d_bits[] = {\n   0x%02x",
		spacing, (u_char) bitmap[0]);

	for (i=1;i<bitmap_size;i++) {
		(void) fprintf (file, ",");
		(void) fprintf (file, (i % 12) ? " " : "\n   ");
		(void) fprintf (file, "0x%02x", (u_char) bitmap[i]);
	}
	(void) fprintf (file, "};\n");
}

error(s)
char *s;
{
	(void) fprintf(stderr, "%s\n", s);
	exit(1);
}
#endif
