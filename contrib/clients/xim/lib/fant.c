/* This software is copyrighted as noted below.  It may be freely
 * copied, modified, and redistributed, provided that the copyright
 * notice is preserved on all copies.
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is".  Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software
 * product without supplying the source, or without informing the
 * end-user that the source is available for no extra charge.
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of
 * modification, and the reason for such modification.
 *
 * fant.c - Perform spacial transforms on images. (should be "remap")
 * Author:  John W. Peterson
 *      Computer Science Dept.
 *      University of Utah
 * Date:    Wed Jun 25 1986
 * Copyright (c) 1986 John W. Peterson
 *
 * This program performs spatial transforms on images.  For full
 * details, consult the paper:
 *      Fant, Karl M. "A Nonaliasing, Real-Time, Spatial Transform
 *            Technique", IEEE CG&A, January 1986, p. 71
 */
/*  Copyright (c) 1989, Massachusetts Institute of Technology
*                Philip R. Thompson
*                Computer Resource Laboratory (CRL)
*                Dept. of Architecture and Planning
*                M.I.T., Rm 9-526
*                Cambridge, MA  02139
*   This  software and its documentation may be used, copied, modified,
*   and distributed for any purpose without fee, provided:
*       --  The above copyright notice appears in all copies.
*       --  This disclaimer appears in all source code copies.
*       --  The names of M.I.T. and the CRL are not used in advertising
*           or publicity pertaining to distribution of the software
*           without prior specific written permission from me or CRL.
*   I provide this software freely as a public service, but not as
*   public domain.  It is not a commercial product, and therefore is
*   not subject to an an implied warranty of merchantability or fitness
*   for a particular purpose.  I provide it as is, without warranty.
*/
/**********************************************************************/
/*  file: $Source: /mit/phils/X/Lib/RCS/fant.c,v $ */
/*                                                                    */
/*  author: Philip R. Thompson, ($Author: phils $)                    */
/*          $Date: 89/10/17 01:38:31 $      $State: Exp $             */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char filemenu_rcsid[] =
    "$Header: /mit/phils/X/Lib/RCS/fant.c,v 1.1 89/10/17 01:38:31 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include <math.h>
#include "XimHeader.h"

#define H_PASS  0
#define V_PASS  1

/* Conversion macros */
#define FRAC(x)  ((x) - ((int)(x)))
#define ROUND(x) ((int)((x) + 0.5))
#define DEG(x)   ((x) * 0.01745329252)
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#ifdef DEBUG
#define INDEX(ptr,x,y)  (*arr_index(ptr,x,y))
#define FINDEX(ptr,x,y) ((float)(*arr_index(ptr,x,y)))
#else
#define INDEX(ptr,x,y)  (*(ptr + out_width*(y)+(x)))
#define FINDEX(ptr,x,y) ((float)(*(ptr + out_width*(y)+(x))))
#endif DEBUG

typedef struct Point2D {
    float x, y;
} Point2D;

/* Each channel is stored in it's own raster (out_height * out_width
 * array of pixels).  This allows each channel to be transformed
 * separately.  We need two copies of each set of channels so we can
 * flip between them for the two passes of the algorithm.
 */
char *calloc();
byte *in_rast = NULL, *out_rast = NULL, *arr_index();
int const_ind;                  /* Constant index */
int pass;                       /* Which pass we're on (h or v) */
int verboseflag = 0;            /* Be chatty (Fant can be slow) */
int originflag;     /* Center picture on given orig instead of center */
int vpassonlyflag;        /* If true, we only need the vertical pass */
int out_width, out_height, out_size, outlinewidth;
int out_xmin, out_xmax, out_ymin, out_ymax;
int in_xmin, in_xmax, in_ymin, in_ymax;
extern int debug_flag;


FantProcess(xim, xscale, yscale, angle, Xorigin, Yorigin)
XimImage *xim;
float xscale, yscale, angle;
int Xorigin, Yorigin;
{
    int i;
    Point2D p[4];

    if ((Xorigin >= 0 && Xorigin < xim->width)  &&
            (Yorigin >= 0 && Yorigin < xim->height))
        originflag = 1;
    else
        originflag = 0;
    vpassonlyflag = 0;
    if (fabs((double)angle) > 45.0)
        fprintf(stderr,
        "Warning: angle larger than 45 degrees, image will blur.\n");
    if ((xscale == 1.0) && (angle == 0.0)) {
        vpassonlyflag = 1;
        if (verboseflag)
            fprintf(stderr, "fant: Only performing vertical pass\n");
    }

    /* To define the output rectangle, we start with a set of
     * points defined by the original image, and then rotate and
     * scale them as desired.
     */
    p[0].x = p[3].x = in_xmin = 0;
    p[0].y = p[1].y = in_ymin = 0;
    p[1].x = p[2].x = in_xmax = xim->width - 1;
    p[2].y = p[3].y = in_ymax = xim->height - 1;

    xform_points(p, xscale, yscale, angle);

    /* Make sure the output image is large enough */
    out_xmin = (int)MAX(0.0, MIN(p[0].x, p[3].x));
    out_ymin = (int)MAX(0.0, MIN(p[0].y, p[1].y));
    out_xmax = (int)MAX(ROUND(p[1].x), ROUND(p[2].x));
    out_xmax = (int)MAX(ROUND(p[1].x), ROUND(p[2].x));
    out_ymax = (int)MAX(ROUND(p[2].y), ROUND(p[3].y));

    /* Need to grab the largest dimensions so the buffers will hold
     * the picture.  The arrays for storing the pictures extend from
     * 0 to out_xmax in width and from out_ymin to ymax in height.
     */
    out_width = MAX(out_xmax, in_xmax) + 1;
    out_height = MAX(out_ymax - out_ymin, in_ymax - in_ymin) + 1;
    out_size = out_width * out_height;
    outlinewidth = out_xmax - out_xmin + 1;
    if (debug_flag)
        fprintf(stderr,"out_width %d  out_height %d  out_size %d\n",
        out_width, out_height, out_size);

    /* Since the array begins at xx_ymin, the four output corner
     * points must be translated to this coordinate system. */
    for (i=0; i < 4; i++) {
        p[i].y -= MIN(in_ymin, out_ymin);
        p[i].x -= MIN(in_xmin, out_xmin);
    }

    out_rast = (byte *)calloc((unsigned)out_size, 1);
    in_rast = (byte *)calloc((unsigned)out_size, 1);
    if (out_rast == NULL || in_rast == NULL) {
        fprintf(stderr, "fant: Not enough memory for rasters\n");
        return(0);
    }

    xim->datasize = outlinewidth * (out_ymax - out_ymin + 1);
    /* Transform each channel */
    if (xim->data) {
        GetRaster(in_rast, xim->data, xim->width);
        free((char *)xim->data);
        bzero((char *)out_rast, out_size);
        xform_image(p);
        if (!(xim->data = (byte *)calloc(xim->datasize, 1))) {
            XimWarn("Fant: can't calloc data");
            return(0);
        }
        PutRaster(out_rast, xim->data, outlinewidth);
    }
    if (xim->grn_data && xim->nchannels > 1) {
        GetRaster(in_rast, xim->grn_data, xim->width);
        free((char *)xim->grn_data);
        bzero((char *)out_rast, out_size);
        xform_image(p);
        if (!(xim->grn_data = (byte *)calloc(xim->datasize, 1))) {
            XimWarn("Fant: can't calloc grn_data");
            return(0);
        }
        PutRaster(out_rast, xim->grn_data, outlinewidth);
    }
    if (xim->blu_data && xim->nchannels > 1) {
        GetRaster(in_rast, xim->blu_data, xim->width);
        free((char *)xim->blu_data);
        bzero((char *)out_rast, out_size);
        xform_image(p);
        if (!(xim->blu_data = (byte *)calloc(xim->datasize, 1))) {
            XimWarn("Fant: can't calloc blu_data");
            return(0);
        }
        PutRaster(out_rast, xim->blu_data, outlinewidth);
    }
    if (xim->other && xim->alpha_flag) {
        GetRaster(in_rast, xim->other, xim->width);
        free((char *)xim->other);
        bzero((char *)out_rast, out_size);
        xform_image(p);
        if ((xim->other = (byte *)calloc(xim->datasize, 1)) == NULL) {
            XimWarn("Fant: can't calloc alpha data");
            return(0);
        }
        PutRaster(out_rast, xim->other, outlinewidth);
    }
    xim->width = xim->bytes_per_line = outlinewidth;
    xim->height = out_ymax - out_ymin + 1;
    free((char *)in_rast);
    free((char *)out_rast);
    return(1);
}


/* This transforms one channel of the image.
 * The result image is based on the points p, using linear
 * interpolation per scanline.
 */
xform_image(p)
Point2D *p;
{
    byte *tmprast;
    int ystart, yend;
    float real_outpos, sizefac, delta;
    float xinlen, yinlen;

    if (verboseflag)
        fprintf(stderr, "transforming channel ...");

    pass = V_PASS;           /* Vertical pass */
    xinlen = (float)(in_xmax - in_xmin + 1);
    yinlen = (float)(in_ymax - in_ymin + 1);
    real_outpos = p[0].y;
    sizefac = (p[3].y - p[0].y) / (yinlen - 1.0);
    delta = (p[1].y - p[0].y) / xinlen;
    for (const_ind = in_xmin; const_ind <= in_xmax; const_ind++) {
        interp_row(sizefac, 0, real_outpos, (in_ymax - in_ymin + 1),
                (out_height - 1));
        real_outpos += delta;
    }

    if (vpassonlyflag)
        return;
    pass = H_PASS;          /* Horizontal pass */

    /* Flip buffers */
    tmprast = in_rast;
    in_rast = out_rast;
    out_rast = tmprast;
    bzero((char *)out_rast, out_size);
    real_outpos = (((p[1].y - p[3].y) * (p[0].x - p[3].x)) /
            (p[0].y - p[3].y)) + p[3].x;
    sizefac = (p[1].x - real_outpos) / (xinlen - 1.0);
    delta = (p[3].x - real_outpos) / (p[3].y - (p[1].y + 1.0));

    /* If we're moving backwards, start at p1 (needs more thought...) */
    if (delta < 0)
        real_outpos = p[0].x;
    ystart = (int)MIN(p[1].y, p[0].y);
    yend = (int)MAX(p[3].y, p[2].y);
    if (ystart < 0) {   /* Ensure start isn't negative */
        real_outpos += delta * fabs((double)ystart);
        ystart = 0;
    }
    for (const_ind = ystart; const_ind < yend; const_ind++) {
        interp_row(sizefac, in_xmin, real_outpos, in_xmax, out_xmax);
        real_outpos += delta;
    }
    if (verboseflag)
        fprintf(stderr, "Done\n");

}


/* Interpolate a row or column of pixels.  This is a straightforward
*  (floating point) implementation of the paper in the algorithm.
*  Sizefac is the amount the row is scaled by, ras_strt is the
*  position to start in the input raster, and real_outpos is the place
*  (in floating point coordinates) to output pixels.  The algorithm
*  loops until it's read pixels up to inmax or output up to outmax.
*/
interp_row(sizefac, ras_strt, real_outpos, inmax, outmax)
float sizefac;
int ras_strt;
float real_outpos;
int inmax, outmax;
{
    int inptr, outptr;
    float inoff, inseg, outseg, accum, insfac, pv, getpxl();

    insfac = 1.0 / sizefac;
    if (real_outpos > 0.0) {
        inseg = 1.0;
        outseg = insfac * (1.0 - FRAC(real_outpos));
        outptr = (int)real_outpos;
        inptr = ras_strt;
    } else {    /* Must clip */
        inoff = -real_outpos * insfac;
        inseg = 1.0 - FRAC(inoff);
        outseg = insfac;
        inptr = ras_strt + ((int)inoff);
        outptr = 0;
    }
    accum = 0.0;
    while ((inptr < inmax) && (outptr < outmax)) {
        pv = getpxl(inptr, inseg);
        if (outseg > inseg) {
            accum += pv * inseg;
            outseg -= inseg;
            inseg = 1.0;
            inptr++;
        } else {
            accum += pv * outseg;
            inseg -= outseg;
            outseg = insfac;
            putpxl(outptr, accum * sizefac);
            outptr++;
            accum = 0.0;
        }
    }
    /* If we ran out of input, output partially completed pixel */
    if (outptr <= outmax)
        putpxl(outptr, accum * sizefac);
}


/* Transform the points p according to xscale, yscale and angle.
 * Rotation is done first, this allows the separate scaling factors to
 * be used to adjust aspect ratios.  Note the image quality of the
 * resulting transform degrades sharply if the angle is > 45 degrees.
 */
xform_points(p, xscale, yscale, angle, Xorigin, Yorigin)
Point2D *p;
float xscale, yscale, angle;
int Xorigin, Yorigin;
{
    int i;
    float s, c, xoff, yoff;
    float xmin, ymin;

    if (!originflag) {
        xoff = ((float)in_xmax - in_xmin) / 2.0 + in_xmin;
        yoff = ((float)in_ymax - in_ymin) / 2.0 + in_ymin;
    } else {
        xoff = Xorigin;
        yoff = Yorigin;
    }
    c = (float)cos(DEG((double)angle));
    s = (float)sin(DEG((double)angle));
    if (verboseflag)
        fprintf(stderr, "Output rectangle:\n");
    for (i=0; i < 4; i++) {
        p[i].x -= xoff;     /* translate to origin */
        p[i].y -= yoff;
        p[i].x *= xscale;   /* Scale... */
        p[i].y *= yscale;
        p[i].x = (p[i].x * c) + (p[i].y * s);   /* Rotate... */
        p[i].y = (p[i].x * -s) + (p[i].y * c);
        p[i].x += (xoff * xscale);   /* translate back from origin */
        p[i].y += (yoff * yscale);
    }
    xmin = MIN(p[0].x, p[3].x);      /* maike minor adjustments */
    ymin = MIN(p[0].y, p[1].y);      /* to centering */
    for (i=0; i < 4; i++) {
        p[i].y -= ymin;
        p[i].x -= xmin;
        if (verboseflag)
            fprintf(stderr, "  %4.1f\t%4.1f\n", p[i].x, p[i].y);
    }
}


/* Grab inseg's worth of pixels from the current input raster.  If inseg
 * is less than one, we perform linear interpolation (Fant's "expansion
 * smoothing") between adjacent pixels.
 * Pixel indexing is determined by two globals; which
 * determines which raster we read from, and pass, which tells if we're
 * indexing rows or columns.
 */
float getpxl(index, inseg)
int index;
float inseg;
{
    float result;

    if (pass == V_PASS) {
        if (index < in_ymax)
            result = inseg * FINDEX(in_rast, const_ind, index)
                + (1.0 - inseg) * FINDEX(in_rast, const_ind, index + 1);
        else
            result = inseg * FINDEX(in_rast, const_ind, index);
    } else {
        if (index < in_xmax)
            result = inseg * FINDEX(in_rast, index, const_ind)
                + (1.0 - inseg) * FINDEX(in_rast, index + 1, const_ind);
        else
            result = inseg * FINDEX(in_rast, index, const_ind);
    }
    return(result);
}


/* Write a pixel into the current output channel. */
putpxl(index, pxlval)
int index;
float pxlval;
{
    int pix = ROUND(pxlval);

#ifdef DEBUG
    if (pix > 255) {
        fprintf(stderr, "pixel overflow: %d at %d %d\n", pix,
            ((pass == V_PASS) ? const_ind : index),
            ((pass == V_PASS) ? index : const_ind));
        pix = 255;
    }
#endif DEBUG
    if (pass == V_PASS)
        INDEX(out_rast, const_ind, index) = (byte)pix;
    else
        INDEX(out_rast, index, const_ind) = (byte)pix;
}


GetRaster(to, from, width)
byte *to, *from;
int width;
{
    int i;

    bzero((char *)to, out_size);
    for (i = in_ymin; i <= in_ymax; i++) {
        bcopy((char *)from, (char *)to, width);
        from += width;
        to += out_width;
    }
}


PutRaster(from, to, width)
byte *from, *to;
int width;
{
    int i;

    /* If the output image is smaller than the input, we must offset
    *  into the pixel array by the difference between the two.
    */
    if (in_ymin < out_ymin)
        from += (out_width * (out_ymin - in_ymin));
    for (i = out_ymin; i <= out_ymax; i++) {
        bcopy((char *)from, (char *)to, width);
        from += out_width;
        to += width;
    }
}


#ifdef DEBUG
/* Dump out a raster (used for debugging). */
dumpraster(ras_ptr)
byte *ras_ptr;
{
    int j, i;

    for (i = in_ymin; i <= in_ymax; i++) {
        for (j = in_xmin; j <= in_xmax; j++) {
            fprintf(stderr,"%2x", ras_ptr[i*out_width+j]);
            fprintf(stderr, " ");
        }
        fprintf(stderr, "\n");
    }
}

byte *arr_index(ptr, x, y)
byte *ptr;
int x, y;
{
    if (x >= out_width) {
        fprintf(stderr, "X=%d >= out_width=%d\n", x, out_width);
        abort();
    }
    if (x < 0) {
        fprintf(stderr, "X=%d < 0\n", x);
        abort();
    }
    if (y >= out_height) {
        fprintf(stderr, "Y=%d >= out_height=%d\n", y, out_height);
        abort();
    }
    if (y < 0) {
        fprintf(stderr, "Y=%d < 0\n", y);
        abort();
    }
    return(ptr + (out_width * y + x));
}
#endif DEBUG

/*** end fant.c ***/
