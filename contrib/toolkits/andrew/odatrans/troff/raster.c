/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/raster.c,v 1.2 89/09/08 17:50:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/raster.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	raster.c,v $
 * Revision 1.2  89/09/08  17:50:00  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:35:05  mss
 * Initial revision
 * 
 * Revision 1.2  89/05/17  15:13:40  jaap
 * plugging memory leaks,
 * cosmetic changes
 * 
 * Revision 1.1  89/01/05  18:43:59  jaap
 * Initial revision
 * 
 */

#ifndef lint
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/raster.c,v $";
#endif
/* static char sccsid[] = "%Z%	%M%	%I%	%E%"; */
#include <ext.h>


/*
 * for initial offset type
 */
#define	hor first
#define ver second
#define PELSIZ 16	/* hard wired in for now */

/*
 * process a raster
 *
 * Don't do anything special on the moment, just figure out how big it is and
 * then draw a border in the end
 */

procraster(basic, contents, type, mode)
CONSTITUENT basic;
SEQUENCE_CONSTITUENT_type contents;
INT_type type, mode;
{

	rg_PEL_TRANS_DENSITY_type *dens;
#if 0
	rg_INITIAL_OFFSET_type *offset;
#endif
	rg_IMAGE_DIMENSIONS_type *r;
	rg_PEL_SPACING_type *pelspace;
	rg_SPACING_RATIO_type *ratio;
	rg_NUMBER_OF_PELS_PER_LINE_type pelperlines;
	rg_NUMBER_OF_LINES_type lines;
	rg_CLIPPING_type *clip;

	at_CONTENT_INFORMATION_type rasterCP;
	CONSTITUENT thisCP;

	long x, y;

	if (contents->length != 1) {
		sprintf(msgsbuf, "Cannot deal with multiple CP's %d", contents->length);
	}
	thisCP = contents->sequence_value.constituents[0];  /* get the first one */;

	/*
	 * Find which kind we are processing
	 *
	 * apparently the automatic is the only one we can expect for now
	 *
	 */
	if (GetAttr(basic, rg_IMAGE_DIMENSIONS, (POINTER_type) &r, INHERIT, IGNORE)
	    ==ERROR_INT) {
		sprintf(msgsbuf, "Cannot get image dimensions %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	switch(r->image_dimensions_tag) {
		case AUTOMATIC_tag:
			trcomment("raster image dimension automatic");
			break;
		case WIDTH_CONTROLLED_tag:
			trcomment("Uh? width controlled raster");
			break;
		case AREA_CONTROLLED_tag:
			trcomment("Uh? area controlled raster");
			break;
		case HEIGHT_CONTROLLED_tag:
			trcomment("Uh? height controlled raster");
			break;
		default:
			sprintf(msgsbuf, "unimplemented raster image dimension: %d",
					(INT_type) r->image_dimensions_tag);
			trcomment(msgsbuf);
	}
#if 0
	/* not implemented */
	/*
	 * initial offset
	 */
	if (GetAttr(basic, rg_INITIAL_OFFSET, (POINTER_type) &offset, INHERIT, IGNORE)
	     == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get initial offset %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
#endif
	if (GetAttr(basic, rg_PEL_SPACING, (POINTER_type) &pelspace, INHERIT, IGNORE)
	    == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get pel spacing %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if (GetAttr(basic, rg_SPACING_RATIO, (POINTER_type) &ratio, INHERIT, IGNORE)
	    == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get spacing ratio %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if (GetIntAttr(thisCP, rg_NUMBER_OF_LINES, (POINTER_type) &lines, INHERIT)
	    == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get numbers of lines %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if (GetIntAttr(thisCP, rg_NUMBER_OF_PELS_PER_LINE, (POINTER_type) &pelperlines,
	    INHERIT) == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get numbers of pels per line %s",
					ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if (GetAttr(basic, rg_PEL_TRANS_DENSITY, (POINTER_type) &dens, INHERIT, IGNORE)
	     == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get pel transmission density %s",
					ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if (GetAttr(basic, rg_CLIPPING, (POINTER_type)&clip, INHERIT, IGNORE)
	    ==ERROR_INT){
		sprintf(msgsbuf, "Cannot get clipping %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if (GetAttr(thisCP, at_CONTENT_INFORMATION, (POINTER_type) &rasterCP, NOINHERIT,
	    IGNORE) == ERROR_INT) {
		sprintf(msgsbuf, "Cannot get raster content information %s",
							ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	/*
	 * sanity check on clipping
	 */
	if ((clip->coord1.first < 0) || (clip->coord1.first > pelperlines) ||
	    (clip->coord2.first < 0) || (clip->coord2.first > pelperlines) ||
	    (clip->coord1.second < 0) || (clip->coord1.second > lines) ||
	    (clip->coord2.second < 0) || (clip->coord2.second > lines)) {
		sprintf(msgsbuf, "illegal clipping attribute, setting to default \n");
		trcomment(msgsbuf);
		sprintf(msgsbuf, "bad values are ((%d, %d), (%d, %d)) Lines=%d, Rows=%d\n",
			clip->coord1.first, clip->coord1.second, clip->coord2.first,
			clip->coord2.second, lines, pelperlines);
		trcomment(msgsbuf);
		clip->coord1.first = 0;
		clip->coord1.second = 0;
		clip->coord2.first = pelperlines - 1;
		clip->coord2.second = lines - 1;
		sprintf(msgsbuf, "values are now ((%d, %d), (%d, %d))\n",
			clip->coord1.first, clip->coord1.second,
			clip->coord2.first, clip->coord2.second);
		trcomment(msgsbuf);
	}
	/*
	 * now find the size of the raster
	 according to paul, assume pel transmission density 6 bmu
	 dens = rg_PEL_TRANS_DENSITY_6;
	 */
	
	switch(pelspace->null) {
		case BOOL_true:
			trcomment("Fixed dimension raster");
			break;
		case BOOL_false:
			trcomment("Scalable dimension raster");
			x = clip->coord2.hor - clip->coord1.hor;
			y = clip->coord2.ver - clip->coord1.ver;
			x = x * pelspace->length + PELSIZ; /* notice width of pel */
			x /= pelspace->pel_spaces;
			y = y * pelspace->length + PELSIZ;
			y *= ratio->first;
			y /= pelspace->pel_spaces;
			y /= ratio->second;
			smu.num = 200; smu.denom = 72;
/*
not yet			trautobox(x, y);
*/
			trstartraster(x,y);
			trraster(&rasterCP->sequence_value.bytes[0], pelperlines, lines); 
			smu.num = smu.denom = 1;
			break;
		default:
			trcomment("Cannot figure out size of raster");
			sprintf(msgsbuf, "%d %d %d", pelspace->null, pelspace->length,
								pelspace->pel_spaces);
			trcomment(msgsbuf);
	}

	/*
	 * delete memory occupied by the beasties
	 */
	free(r);
	free(pelspace);
	free(ratio);
	free(clip);
	/* fprintf(stderr,"done with raster, deleting\n"); */
	if (ERROR_INT == DeleteSequence(rasterCP)) {
		sprintf(msgsbuf, "Error deleting raster content info: (%s)\n",
								ErrorMsg(TKError));
	}

	trcomment("Raster Implementation in Progress");
}
