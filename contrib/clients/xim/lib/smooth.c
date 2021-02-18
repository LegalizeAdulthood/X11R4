/**********************************************************************/
/*  file: $Source: /mit/phils/X/Lib/RCS/smooth.c,v $ */
/*                                                                    */
/*  author: Philip R. Thompson, ($Author: phils $)                  */
/*          $Date: 89/10/23 22:43:06 $      $State: Exp $           */
/*                                                                    */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char smooth_rcsid[] =
    "$Header: /mit/phils/X/Lib/RCS/smooth.c,v 1.2 89/10/23 22:43:06 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include "XimHeader.h"

RgbToColor(red, grn, blu, bufsize, colpix)
byte *red, *grn, *blu;
unsigned bufsize;
Color *colpix;
{
    register int i;
    register Color *pc;

    for (i = 0, pc = colpix; i < bufsize; i++, pc++) {
        pc->red = red[i];
        pc->grn = grn[i];
        pc->blu = blu[i];
    }
}


/* Functions: ScanColor5 ()
*    Scan the current pixmap to process each pixel as the
*    "target pixel" at the center of a 5x5 array of pixels.
*
*    At present, the scan only deals with target pixels which
*    have a full set of neighbors.  Pixels in the top, bottom,
*    left, and right pairs of rows and columns of pixels are omitted
*    from the scan.
*
*  Input:    No parameters in function invocation;
*    colpix  (Color *) Pointer to pixel data as Color array
*    pix     (Color *) Pointer to pixmap descriptor
*  Output:   None as return value;
*    colpix  (Color *) Pointer to pixel data as Color array
*  Notes:
*    Separate pointers address the target pixel and its neighbors.
*    Their names are:
*        -----------------------------------------
*        |  p00  |  p01  |  p02  |  p03  |  p04  |
*        |-------|-------|-------|-------|-------|
*        |  p10  |  p11  |  p12  |  p13  |  p14  |
*        |-------|-------|-------|-------|-------|
*        |  p20  |  p21  |   p   |  p23  |  p24  |
*        |-------|-------|-------|-------|-------|
*        |  p30  |  p31  |  p32  |  p33  |  p34  |
*        |-------|-------|-------|-------|-------|
*        |  p40  |  p41  |  p42  |  p43  |  p44  |
*        -----------------------------------------
*    Use of separate pointers is likely to involve some degree
*    of iteration when editing the processing algorithm, but
*    it eliminates it during execution.  Honest, this is
*    deliberate!
*    The name of the target pixel pointer in the destination
*    color array is:
*      tp    Target pixel in destination color array
*/

/* Determine RGB distances between pixels p & q    */
#define dcr(q)  dr=(p->red)-(q->red);if (dr<0) dr= -dr;
#define dcg(q)  dg=(p->grn)-(q->grn);if (dg<0) dg= -dg;
#define dcb(q)  db=(p->blu)-(q->blu);if (db<0) db= -db;
/*  Account for pixel q in weighted averages    */
#define av(q,w)   sumw+=w;\
          wsumr += w*(q->red), wsumg+=w*(q->grn), wsumb+=w*(q->blu);
/* Conditionally average pixel q with pixel p  */
#define ca(q,w)   dcr(q);dcg(q);dcb(q);if((dr+dg+db)<threshold){av(q,w)}

Smoothim(xim, threshold)
XimImage *xim;
int threshold;
{
    register  i;
    char *calloc(), *malloc();
    Color  *colpix = NULL;     /* Pixmap as a Color array  */
    Color  *newcpx = NULL;     /* New copy of pixel Color data */
    extern int debug_flag;

    colpix = (Color *)calloc((unsigned)xim->datasize, sizeof(Color));
    if (colpix == NULL) {
        XimWarn("Insufficient memory for copy of image\n");
        return(0);
    }
    RgbToColor(xim->data, xim->grn_data, xim->blu_data,
            xim->datasize, colpix);
    free((char *)xim->data);
    free((char *)xim->grn_data);
    free((char *)xim->blu_data);

    if (debug_flag)
        fprintf(stderr, "Processing image...");
    newcpx = (Color *)calloc((unsigned)xim->datasize, sizeof(Color));
    if (newcpx == NULL) {
        XimWarn("Insufficient memory for second copy of image\n");
        return(0);
    }
    ScanColor5(colpix, newcpx, xim->width, xim->height, threshold);
    free((char *)colpix);

    xim->data = (byte *)malloc(xim->datasize);
    xim->grn_data = (byte *)malloc(xim->datasize);
    xim->blu_data = (byte *)malloc(xim->datasize);
    if (!xim->data || !xim->grn_data || !xim->blu_data) {
        XimWarn("Can't malloc buffers\n");
        return(1);
    }
    for (i=0; i < xim->datasize; i++) {
        xim->data[i] = newcpx[i].red;
        xim->grn_data[i] = newcpx[i].grn;
        xim->blu_data[i] = newcpx[i].blu;
    }
    free((char *)newcpx);
    if (debug_flag)
        fprintf(stderr, "Done.\n");
    return(1);
}


ScanColor5(colpix, newcpx, width, height, threshold)
Color *colpix, *newcpx;
int width, height, threshold;
{
    register Color *p;         /* Pointer to target pixel */
    Color *p00, *p01, *p02, *p03, *p04; /* Pointers to neighbors: */
    Color *p10, *p11, *p12, *p13, *p14;
    Color *p20, *p21, *p23, *p24;
    Color *p30, *p31, *p32, *p33, *p34;
    Color *p40, *p41, *p42, *p43, *p44;
    Color *tp;             /* Pointer to dest. target */
    int iy;                /* Counter for # rows to scan  */
    int ny;                /* Total # rows to scan    */
    int ix;                /* Counter for # col's to scan */
    int nx;                /* Total # columns to scan */
    int sumw;              /* Sum of weights      */
    int wsumr;             /* Weighted sum of reds    */
    int wsumg;             /* Weighted sum of greens  */
    int wsumb;             /* Weighted sum of blues   */
    int dr;                /* Delta Red   between 2 pixels */
    int dg;                /* Delta Green between 2 pixels */
    int db;                /* Delta Blue  between 2 pixels */
    int ri;                /* Rounding increment      */

    p00 = colpix;
    p01 = &colpix[1];
    p02 = &colpix[2];   /* Set pointers to row 0   */
    p03 = &colpix[3];
    p04 = &colpix[4];
    p10 = &colpix[width];
    p11 = &colpix[width + 1];
    p12 = &colpix[width + 2];   /* Set pointers to row 1   */
    p13 = &colpix[width + 3];
    p14 = &colpix[width + 4];
    p20 = &colpix[2 * width];
    p21 = &colpix[2 * width + 1];
    p = &colpix[2 * width + 2]; /* Set pointers to row 2   */
    tp = &newcpx[2 * width + 2];    /* Set ptr to 1st dest. pixel  */
    p23 = &colpix[2 * width + 3];
    p24 = &colpix[2 * width + 4];
    p30 = &colpix[3 * width];
    p31 = &colpix[3 * width + 1];
    p32 = &colpix[3 * width + 2];   /* Set pointers to row 3   */
    p33 = &colpix[3 * width + 3];
    p34 = &colpix[3 * width + 4];
    p40 = &colpix[4 * width];
    p41 = &colpix[4 * width + 1];
    p42 = &colpix[4 * width + 2];   /* Set pointers to row 4   */
    p43 = &colpix[4 * width + 3];
    p44 = &colpix[4 * width + 4];
    bcopy((char *)colpix, (char *)newcpx, 
            (width * height * sizeof(Color)));

    /* Initialize pointers and counters  */
    nx = width - 4; /* Set to scan (width-4) columns,  */
    ny = height - 5;    /* (height-4) rows         */
    /*  Scan pixmap data    */
    for (iy = 0; iy < ny; ++iy) {   /* For each row of pixels... */
        for (ix = 0; ix < nx; ++ix) {   /* For each pixel in row... */
            /* Compute weighted average of target pixel
             * color components with neighboring pixels of
             * approximately the same color. Weights assume
             * that the importance of neighboring pixels is
             * inversely proportional to the square of
             * their distance from the target pixel. */
            sumw = 0;
            wsumr = 0;  /* Preset sum of weights and   */
            wsumg = 0;  /* weighted averages to 0  */
            wsumb = 0;

            /* Do it to it */
            ca(p00, 5)
                ca(p01, 8)
                ca(p02, 10) /* Top row in 5x5 cell */
                ca(p03, 8)
                ca(p04, 5)
                ca(p10, 8)
                ca(p11, 20)
                ca(p12, 40) /* Row between target & top */
                ca(p13, 20)
                ca(p14, 8)
                ca(p20, 10)
                ca(p21, 40)
                av(p, 80)   /* Target pixel row */
                ca(p23, 40)
                ca(p24, 10)
                ca(p30, 8)
                ca(p31, 20)
                ca(p32, 40) /* Row between target & bottom */
                ca(p33, 20)
                ca(p34, 8)
                ca(p40, 5)
                ca(p41, 8)
                ca(p42, 10) /* Bottom row in 5x5 cell  */
                ca(p43, 8)
                ca(p44, 5)
                ri = (sumw >> 1) - 1;   /* Derive rounding increment  */

            /* Destination copy of target pixel = weighted
             * RGB average derived above */
            tp->red = (wsumr + ri) / sumw;
            tp->grn = (wsumg + ri) / sumw;
            tp->blu = (wsumb + ri) / sumw;
            ++p00; ++p01; ++p02;  /* Advance to next pixel */
            ++p03; ++p04; ++p10;
            ++p11; ++p12; ++p13;
            ++p14; ++p20; ++p21;
            ++p; ++tp;
            ++p23; ++p24; ++p30;
            ++p31; ++p32; ++p33;
            ++p34; ++p40; ++p41;
            ++p42; ++p43; ++p44;
        }

        /* Skip pixels at beginning and end of row as target
         * pixels  */
        p00 += 4; p01 += 4; p02 += 4;
        p03 += 4; p04 += 4; p10 += 4;
        p11 += 4; p12 += 4; p13 += 4;
        p14 += 4; p20 += 4; p21 += 4;
        p += 4; tp += 4;
        p23 += 4; p24 += 4; p30 += 4;
        p31 += 4; p32 += 4; p33 += 4;
        p34 += 4; p40 += 4; p41 += 4;
        p42 += 4; p43 += 4; p44 += 4;
    }
}

/* end smooth.c */
