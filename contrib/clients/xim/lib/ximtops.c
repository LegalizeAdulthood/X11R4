/* ximtops.c
* This program will dump the contents of an X window into
* Adobe PostScript raster format.
*
*  Copyright (c) 1989, Massachusetts Institute of Technology
*                Philip R. Thompson  (phils@athena.mit.edu)
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
#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"

#define XPS_VERSION     "3.4"
#define POINTS_PER_INCH 72.0
#define PAGE_BORDER     0.25
#define PAGE_WIDTH      8.5
#define PAGE_HEIGHT     11.0

XimWritePostscriptFile(out_fp, xim, user_height)
FILE  *out_fp;
XimImage  *xim;
double user_height;
{
    register  i, pages;
    register byte  *data = (byte*)0;
    double  img_rotate, img_x_trans = 0.0, img_y_trans = 0.0;
    double  user_width = 0.0;
    char  *xps_version = XPS_VERSION;

    /* Determine image scaling, translation and rotational information.
    */
    FrameImage(xim->width, xim->height, &img_rotate,
            &img_x_trans, &img_y_trans, &user_width, &user_height);

    /* Begin Writing out PostScript prolog.  */
    fprintf(out_fp, "%%!\n");
    fprintf(out_fp, "%%%%Title: Xim translation to postscript\n");
    fprintf(out_fp, "%%%%DocumentFonts: \n");
    fprintf(out_fp, "%%%%Creator: %s <%s> and ximtops %s\n",
            xim->program, xim->author, xps_version);
    fprintf(out_fp, "%%%%CreationDate: %s\n", xim->date);
    fprintf(out_fp, "%%%%Pages: (atend)\n");
    fprintf(out_fp, "%%%%BoundingBox: \n");
    fprintf(out_fp, "%%%%EndComments\n");
    fprintf(out_fp, "%%%%BeginProlog\n");
    fprintf(out_fp, "/ncolors %d def\n/bits_pixel %d def\n",
            xim->ncolors, xim->nchannels * xim->bits_channel);
    fprintf(out_fp, "/image_width %d def\n", xim->width);
    fprintf(out_fp, "/image_height %d def\n", xim->height);
    fprintf(out_fp, "/image_depth %d def\n", 8);
    fprintf(out_fp, "/image_string image_width image_depth mul ");
    fprintf(out_fp, "8 idiv string def\n");
    fprintf(out_fp, "/image_xps {\n");
    fprintf(out_fp, "   image_width image_height image_depth\n");
    fprintf(out_fp, "   [\n");
    fprintf(out_fp, "      image_width 0 0\n");
    fprintf(out_fp, "      image_height neg 0 image_height\n");
    fprintf(out_fp, "   ]\n");
    fprintf(out_fp, "   {currentfile image_string readhexstring pop}");
    fprintf(out_fp, "\n   image\n");
    fprintf(out_fp, "} def\n");
    fprintf(out_fp, "%%%%EndProlog\n");
    fprintf(out_fp, "initgraphics\n");

    if ((xim->bits_channel == 1) && (xim->nchannels == 1))
        BitmapToPixmap(xim);   /* there's probably a better way */
    ColorToGrey(xim);
    /* Write out the image data.  */
    for (pages=0; pages < xim->npics; pages++) {
        data = xim->data + (xim->datasize * pages);
        fprintf(out_fp,"gsave\n");
        fprintf(out_fp,"  %lf rotate\n", img_rotate);
        fprintf(out_fp,"  %lf %lf translate\n",img_x_trans,img_y_trans);
        fprintf(out_fp,"  %lf %lf scale\n", user_width, user_height);
        fprintf(out_fp,"  image_xps");

        for (i=0; i < xim->datasize; i++) {
            if ((i % 32) == 0)
                fprintf(out_fp, "\n");
            fprintf(out_fp, "%02x", data[i]);
        }
        fprintf(out_fp, "\n");
        fprintf(out_fp, "grestore\n");
        fprintf(out_fp, "showpage\n");
        fprintf(stderr,"%d of %d image(s) written\n", pages+1,
                xim->npics);
    }
    fprintf(out_fp, "%%%%Trailer\n");
    fprintf(out_fp, "%%%%Pages: %d\n", pages);
    return(1);
}


/* Compute the image rotation, X and Y translation, and X and Y scaling
* in order to frame the image on the page.
*/
FrameImage(width, height, rotate, x_trans, y_trans, u_width, u_height)
int width, height;
double *rotate, *x_trans, *y_trans, *u_width, *u_height;
{
    double image_w, image_h;
    double delta_x, delta_y;
    double scale, max, min;

    /* Compute image rotation.
     */
    if (((double)width / (double)height) > 1.0)
        *rotate = -90.0;
    else
        *rotate = 0.0;
    
    /* Compute image scaling.  */
    image_w = (double)width / POINTS_PER_INCH;
    image_h = (double)height / POINTS_PER_INCH;
    if (*u_height != 0.0) {          /* user specified height */
        scale = *u_height / image_h;
        image_h *= scale;
        image_w *= scale;
    }
    max = image_w > image_h ? image_w : image_h;
    min = image_w > image_h ? image_h : image_w;
    /* check max page sizes */
    if (max > (PAGE_HEIGHT - (2.0 * PAGE_BORDER))) {
        scale = (PAGE_HEIGHT - (2.0 * PAGE_BORDER)) / max;
        image_h *= scale;
        image_w *= scale;
    }
    if (min > (PAGE_WIDTH - (2.0 * PAGE_BORDER))) {
        scale = (PAGE_WIDTH - (2.0 * PAGE_BORDER)) / min;
        image_w *= scale;
        image_h *= scale;
    }
        
    *u_width = image_w * POINTS_PER_INCH;
    *u_height = image_h * POINTS_PER_INCH;
    if (*rotate == 0.0) {
        delta_x = PAGE_WIDTH - (image_w + (2.0 * PAGE_BORDER));
        delta_y = PAGE_HEIGHT - (image_h + (2.0 * PAGE_BORDER));
        if (delta_x >= 0.0)
            *x_trans = ((delta_x/2.0 + PAGE_BORDER)* POINTS_PER_INCH);
        else
            *x_trans = PAGE_BORDER * POINTS_PER_INCH;
        if (delta_y >= 0.0)
            *y_trans = ((delta_y/2.0 + PAGE_BORDER)* POINTS_PER_INCH);
        else
            *y_trans = PAGE_BORDER * POINTS_PER_INCH;
    } else {
        delta_x = PAGE_HEIGHT - (image_w + (2.0 * PAGE_BORDER));
        delta_y = PAGE_WIDTH - (image_h + (2.0 * PAGE_BORDER));
        if (delta_x >= 0.0)
            *x_trans = ((delta_x/2.0 + PAGE_BORDER - PAGE_HEIGHT) *
                POINTS_PER_INCH);
        else
            *x_trans = (PAGE_BORDER - PAGE_HEIGHT) * POINTS_PER_INCH;
        if (delta_y >= 0.0)
            *y_trans = ((delta_y/2.0 + PAGE_BORDER)* POINTS_PER_INCH);
        else
            *y_trans = PAGE_BORDER * POINTS_PER_INCH;
    }
}

/* End of ximtops.c */
