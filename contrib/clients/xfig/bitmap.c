/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "object.h"
#include "paintop.h"

extern F_compound	objects;

extern int		errno;
extern int		DEBUG;
extern int		pointmarker_shown;
extern int		compoundbox_shown;

extern char		*sys_errlist[];
extern int		sys_nerr, errno;

write_bitmap(file_name)
char	*file_name;
{
	FILE			*fp;
	struct stat		 file_status;
	char			 string[180];

	if (*file_name == 0) {
	    put_msg("No file");
	    return;
	    }
	if (stat(file_name, &file_status) == 0) { /* file exists */
	    if (file_status.st_mode & S_IFDIR) {
		put_msg("\"%s\" is a directory", file_name);
		return;
		}
	    if (file_status.st_mode & S_IWRITE) { /* writing is permitted */
		if (file_status.st_uid != geteuid()) {
		    put_msg("\"%s\" permission is denied", file_name);
		    return;
		    }
		else {
		    sprintf(string, "\"%s\" File exists. Please click the LEFT button to COMFIRM overwrite. To cancel, click the MIDDLE or RIGHT button.", file_name);
		    if (wmgr_confirm(canvas_swfd, string) != -1) {
			put_msg("No write.");
			return;
			}
		    }
		}
	    else {
		put_msg("\"%s\" File is read only", file_name);
		return;
		}
	    }
	else if (errno != ENOENT) return;  /* file does exist but stat fails */

	if ((fp = fopen(file_name, "w")) == NULL) {
	    blink_msg();
	    put_msg("Couldn't open file %s, %s", file_name, sys_errlist[errno]);
	    return;
	    }
	else {
	    create_n_write_bitmap(fp);
	    }
	}

create_n_write_bitmap(fp)
FILE	*fp;
{
	int	 box, marker, xmin, ymin, xmax, ymax;
	PIXRECT	bitmap, pw_pixrect, pw_prretained;
	F_text	*t;

	/* Assume that there is at least one object */
	compound_bound(&objects, &xmin, &ymin, &xmax, &ymax);

	if (DEBUG) {
	    draw_rectbox(xmin, ymin, xmax, ymax, INV_PAINT);
	    }
#ifndef	X11
	bitmap = mem_create(xmax-xmin+1, ymax-ymin+1, 1);
	pw_pixrect = canvas_pixwin->pw_pixrect;
	pw_prretained = canvas_pixwin->pw_prretained;
	canvas_pixwin->pw_pixrect = canvas_pixwin->pw_prretained = bitmap;
	translate_compound(&objects, -xmin, -ymin);
	marker = pointmarker_shown;
	pointmarker_shown = 0;
	box = compoundbox_shown;
	compoundbox_shown = 0;
	pw_batch_on(canvas_pixwin);
	redisplay_arcobject(objects.arcs);
	redisplay_compoundobject(objects.compounds);
	redisplay_ellipseobject(objects.ellipses);
	redisplay_lineobject(objects.lines);
	redisplay_splineobject(objects.splines);
	canvas_pixwin->pw_pixrect = pw_pixrect;
	canvas_pixwin->pw_prretained = pw_prretained;
	pw_batch_off(canvas_pixwin);
	pointmarker_shown = marker;
	compoundbox_shown = box;
	write_pixrect(fp, bitmap);
	for (t = objects.texts; t != NULL; t = t->next) write_text(fp, t);
	translate_compound(&objects, xmin, ymin);
	fclose(fp);
	pr_destroy(bitmap);
#else
	put_msg("Sorry, write_bitmap not implemented for X11 yet");
#endif	X11
	}

#ifndef	X11
write_pixrect(fp, bitmap)
FILE		*fp;
PIXRECT		bitmap;
{
	int		i, j, width, height, shorts_per_row ;
	u_short		*ptr;

	width = bitmap->pr_size.x;
	height = bitmap->pr_size.y;
	shorts_per_row = (int) ((width + 15) / 16);

	put_msg("Writing . . .");

	fprintf(fp, "/* Format_version=1, Width=%d, Height=%d, ",
		width, height);
	fprintf(fp, "Depth=1, Valid_bits_per_item=16\n */\n");

	ptr = (u_short*)((MPR_DATA *) bitmap->pr_data)->md_image;

	for (i = 0; i < height; i++) {
	    fprintf(fp, "\t");
	    fprintf(fp, "0x%04X", *ptr++);
	    for (j = 1; j < shorts_per_row; j++) fprintf(fp, ",0x%04X", *ptr++);
	    if (height-i != 1) fprintf(fp, ","); /* if i is not the last row */
	    fprintf(fp, "\n");
	    }

	put_msg("Done");
	}
#endif	X11
