/****************************************************************
 * fbquant.c: FBM Library 0.93 (Beta test) 03-May-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * fbquant: Convert an RGB color image to mapped color format (color
 *      quantization step).  Floyd-Steinberg dithering is used
 *      to reduce color banding.  The quantization used is a
 *      modification of Heckbert's median cut.
 *
 * HISTORY
 * 03-May-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *  Clear FBM pointers before allocate
 *
 * 07-Apr-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *  Add provision for using colormap from another image.
 *
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *  Beta release (version 0.93) mlm@cs.cmu.edu
 *
 * 26-Feb-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *  Changes for small color maps.  Fixed bug with unsigned
 *  arithmetic that ruined dithering for images with small
 *  colormaps.  Added error limiting in the Floyd-Steinberg
 *  code to prevent color "shadowing" that occurs with small
 *  numbers of colors.  Also change to use colors 0..n-1 instead
 *  of reserving colors 0 and n-1 for Sun foreground/background
 *  colors.
 *
 * 11-Nov-88  Michael Mauldin (mlm) at Carnegie Mellon University
 *  Created.
 *
 * References: Uses a variant of Heckbert's adaptive partitioning
 *         algorithm.  See Computer Graphics, v16n3 July 1982
 ****************************************************************/
/*********************************************************************/
/*  file:   $Source: /mit/phils/X/Lib/RCS/fbquant.c,v $        */
/*                                                                    */
/*  author: Philip R. Thompson, ($Author: phils $)                    */
/*          $Date: 89/11/02 16:08:33 $      $State: Exp $             */
/*                                                                    */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson                                  */
/*                Computer Resource Laboratory (CRL)                  */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
 static char fbquant_rcsid[] =
         "$Header: /mit/phils/X/Lib/RCS/fbquant.c,v 1.7 89/11/02 16:08:33 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include "XimHeader.h"

#define RD 0
#define GR 1
#define BL 2
#define REDMASK 0076000
#define REDSHFT 10
#define GRNMASK 0001740
#define GRNSHFT 5
#define BLUMASK 0000037
#define BLUSHFT 0
#define CUBITS  5
#define CUBIGN  3
#define CUBSID  32
#define CUBSIZ  32768
#define MAXSHRT 32767
#define MAXERR  32

#define GETR(X) (((X) & REDMASK) >> REDSHFT)
#define GETG(X) (((X) & GRNMASK) >> GRNSHFT)
#define GETB(X)  ((X) & BLUMASK)

#define CLRINDEX(R,G,B)   (((R) << REDSHFT) & REDMASK | \
     ((G) << GRNSHFT) & GRNMASK | ((B)  & BLUMASK))

#define CLRINDEX8(R,G,B)   (((R) << 7) & REDMASK | \
     ((G) << 2) & GRNMASK | ((B) >> 3)  & BLUMASK)

#define GETR8(X)  (((X) & REDMASK) >> (REDSHFT-CUBIGN))
#define GETG8(X)  (((X) & GRNMASK) >> (GRNSHFT-CUBIGN))
#define GETB8(X)  (((X) & BLUMASK) << CUBIGN)
#define SWAP(A,B)  (i = A , A = B, B = i)

typedef struct PIXEL {
    short cnt, color;
} PIXEL;

extern debug_flag;
int verbose = 0;
char *malloc(), *calloc();


XimQuant(xim, ncolors, colors, use_map, dither_flag)
XimImage *xim;
Color *colors;
int ncolors, use_map, dither_flag;
{
    int i, hist[CUBSIZ];       /* Color cube 32x32x32 for histogram */
    int free_map = 0;
    Color *cmap;

    if (colors)
        cmap = colors;
    else {
        if (!(cmap = (Color *)calloc((unsigned)ncolors,
                sizeof(Color)))) {
            XimWarn("XimQuant: can't calloc() color table");
            return(0);
        }
        free_map = 1;
    }
    /* Build colormap by sampling and mcut if not specified */
    if (!use_map && cmap) {
        sample_image(xim, hist);
        build_colormap(hist, cmap, ncolors);
    }
    /* Use Floyd-Steinberg error diffusion to quantize using the
    *  new cmap
    */
    ncolors = clr_quantize(xim, cmap, ncolors, dither_flag);

    xim->colors = (Color *)calloc(ncolors, sizeof(Color));
    xim->ncolors = ncolors;
    for (i=0; i < xim->ncolors; i++)
        xim->colors[i] = cmap[i];
    if (free_map)
        free((char *)cmap);
    return(ncolors);
}


/* sample_image
*/
sample_image(image, hist)
XimImage *image;
int hist[];
{
    register int i, j;
    register byte *rp, *gp, *bp;

    /* Clear the histogram */
    bzero((char *)hist, CUBSIZ * sizeof(int));
    /* Now count */
    rp = image->data;
    gp = image->grn_data; 
    bp = image->blu_data;
    for (j=0; j < image->height; j++)
        for (i = 0; i < image->width; i++)
            hist[CLRINDEX8(*rp++, *gp++, *bp++)]++;
}


/****************************************************************
 * build_colormap:
 ****************************************************************/
build_colormap(hist, cmap, ncolors)
int hist[];
Color *cmap;
int ncolors;
{
    register int i, j;
    PIXEL box[CUBSIZ];
    register PIXEL *b;
    int used = 0, cmp_int();

    /* Build the first box, encompassing all pixels */
    for (b = box, i = 0; i < CUBSIZ; i++) {
        b->color = i;
        j = hist[i];
        b->cnt = (j > MAXSHRT) ? MAXSHRT : j;
        b++;
    }
    /* Move all non-zero count pixels to the front of the list */
    for (i = 0, used = 0; i < CUBSIZ; i++) {
        if (box[i].cnt > 0)
            box[used++] = box[i];
    }
    /* Special case if we didnt need all colors */
    if (used <= ncolors) {
        /* Copy the colors actually found */
        for (i = 0; i < used; i++) {
            cmap[i].red = GETR8(box[i].color);
            cmap[i].grn = GETG8(box[i].color);
            cmap[i].blu = GETB8(box[i].color);
        }
        /* Set the rest to WHITE */
        for (; i < ncolors; i++) {
            cmap[i].red = 255;
            cmap[i].grn = 255;
            cmap[i].blu = 255;
        }
    } else      /* Build sets all colors */
        split_box(box, used, 0, ncolors, cmap);

    /* Now sort 0..n-1 according to increasing intensity */
    qsort((char *)cmap, ncolors, sizeof(Color), cmp_int);

    /* Set the output indices */
    for (i=0; i < ncolors; i++)
        cmap[i].pixel = i;
}


/* split_box: Basic recursive part of Heckberts adaptive partitioning
*        algorithm.
*/
split_box(box, boxlen, clr, numclr, cmap)
PIXEL *box;
int boxlen, clr, numclr;
Color *cmap;
{
    int maxv[3], minv[3], numv[3];
    int pcnt[3][CUBSID];
    int sbox, snum, split, half, maxdif, dif;
    register PIXEL *top, *bot;
    int topw, botw;
    int red, grn, blu;
    int cmp_red(), cmp_grn(), cmp_blu();
    register int i, c;

    /* If numclr exceeds boxlen, we are in trouble */
    if (numclr > boxlen) {
        fprintf(stderr, "boxlen %d is less numclr %d, panic!\n than",
            boxlen, numclr);
        (void)fflush(stderr);
        abort();
    }
    /* Base case: only one color, assign the average for this cell */
    if (numclr <= 1) {
        red = box_avg_red(box, boxlen);
        grn = box_avg_grn(box, boxlen);
        blu = box_avg_blu(box, boxlen);

        /* Map x to x+4, because the histogram maps values to
         * multiples of 8 */
        cmap[clr].red = red + 4;
        cmap[clr].grn = grn + 4;
        cmap[clr].blu = blu + 4;
        if (debug_flag) {
            fprintf(stderr, "   assigning color %d  <%u,%u,%u>  (%d)\n",
                clr, cmap[clr].red, cmap[clr].grn, cmap[clr].blu,
                box_weight(box, boxlen));
        }
        return;
    }
    /* Gather statistics about the boxes contents */
    minv[RD] = minv[GR] = minv[BL] = CUBSID;
    maxv[RD] = maxv[GR] = maxv[BL] = 0;
    numv[RD] = numv[GR] = numv[BL] = 0;
    for (i = 0; i < CUBSID; i++)
        pcnt[RD][i] = pcnt[GR][i] = pcnt[BL][i] = 0;

    for (i = 0; i < boxlen; i++) {
        c = box[i].color;
        red = GETR(c);
        grn = GETG(c);
        blu = GETB(c);
        if (red < minv[RD])
            minv[RD] = red;
        if (red > maxv[RD])
            maxv[RD] = red;
        if (grn < minv[GR])
            minv[GR] = grn;
        if (grn > maxv[GR])
            maxv[GR] = grn;
        if (blu < minv[BL])
            minv[BL] = blu;
        if (blu > maxv[BL])
            maxv[BL] = blu;
        if (++pcnt[RD][red] == 1)
            numv[RD]++;
        if (++pcnt[GR][grn] == 1)
            numv[GR]++;
        if (++pcnt[BL][blu] == 1)
            numv[BL]++;
    }
    /* Special case, boxlen = numclr, just assign each box one
     * color
     */
    if (boxlen == numclr) {
        for (i = 0; i < boxlen; i++)
            split_box(box + i, 1, clr + i, 1, cmap);
        return;
    }
    /* Pick a dimension to split */
    split = -1;
    maxdif = -1;

    if ((dif = (maxv[RD] - minv[RD])) > maxdif) {
        maxdif = dif;
        split = RD;
    }
    if ((dif = (maxv[GR] - minv[GR])) > maxdif) {
        maxdif = dif;
        split = GR;
    }
    if ((dif = (maxv[BL] - minv[BL])) > maxdif) {
        maxdif = dif;
        split = BL;
    }
    /* Sort along the chosen dimension */
    switch (split) {
    case RD:
        qsort((char *)box, boxlen, sizeof(*box), cmp_red);
        break;
    case GR:
        qsort((char *)box, boxlen, sizeof(*box), cmp_grn);
        break;
    case BL:
        qsort((char *)box, boxlen, sizeof(*box), cmp_blu);
        break;
    default:
        fprintf(stderr, "panic in split_box, split = -1\n");
        (void)fflush(stderr);
        (void)fflush(stdout);
        abort();
    }
    /* Split at the median, but make sure there are at least
     * numclr/2 different colors on each side of the split, to
     * avoid wasting colors.
     * 
     * Note: need to keep in mind that when the box is large, dont
     * split too close to one edge.
     */
    half = numclr / 2;
    top = box;
    bot = box + (boxlen - 1);
    topw = top->cnt;
    botw = bot->cnt;

    /* Set top and bot to point to min/max feasible splits */
    while ((top - box) + 1 < half) {
        top++;
        topw += top->cnt;
    }
    while ((boxlen - (bot - box)) < half) {
        bot--;
        botw += bot->cnt;
    }
    /* Move top and bottom towards each other 1/8 of remaining
     * distance
     */
    c = (bot - top) / 8;
    for (i = 0; i < c; i++) {
        top++;
        topw += top->cnt;
    }
    for (i = 0; i < c; i++) {
        bot--;
        botw += bot->cnt;
    }
    /* Now search for median */
    while (top < bot) {
        if (topw < botw) {
            top++;
            topw += top->cnt;
        } else {
            bot--;
            botw += bot->cnt;
        }
    }
    /* Decide which half gets the midpoint */
    if (topw > botw) {  /* Median wants to go with top */
        sbox = (top - box) + 1;
        snum = numclr - half;
    } else {    /* Median wants to go with bottom */
        sbox = (top - box);
        snum = half;
    }
    /* Handle boundary conditions with number of colors vs box size */
    if (sbox == 0)
        sbox++;
    else if (sbox == boxlen)
        sbox--;
    while (snum > sbox)
        snum--;
    while (numclr - snum > boxlen - sbox)
        snum++;
# ifndef OPTIMIZE
    /* Check for boundary condition errors */
    if (snum <= 0 || snum >= numclr) {
        fprintf(stderr, "panic, using zero colors for box\n");
        (void)fflush(stderr);
        abort();
    }
    if (boxlen - sbox < numclr - snum) {
        fprintf(stderr, "panic, about to used %d boxes for %d colors\n",
            boxlen - sbox, numclr - snum);
        (void)fflush(stderr);
        abort();
    }
    if (sbox < snum) {
        fprintf(stderr, "panic, about to used %d boxes for %d colors\n",
            sbox, snum);
        (void)fflush(stderr);
        abort();
    }
# endif
    if (debug_flag) {
        int count = numclr, depth = 8;
        while (count > 0) {
            depth--;
            count /= 2;
        }
        for (i = 0; i < depth; i++)
            fprintf(stderr, "  ");
        fprintf(stderr,
            "box [%d..%d|%d] r(%d,%d,%d) g(%d,%d,%d) b(%d,%d,%d) =>",
            0, boxlen - 1, numclr, minv[RD], maxv[RD], numv[RD],
            minv[GR], maxv[GR], numv[GR], minv[BL], maxv[BL], numv[BL]);
        fprintf(stderr, " %c [%d..%d|%d] [%d..%d|%d]\n",
            "RGB"[split], 0, sbox - 1, snum, sbox, boxlen - 1,
            numclr - snum);
    }
    /* Now recurse and split each sub-box */
    split_box(box, sbox, clr, snum, cmap);
    split_box(box + sbox, boxlen -sbox, clr + snum, numclr -snum, cmap);
}


/*   box_weight: Determine the total count of a box
*/
box_weight(box, boxlen)
register PIXEL *box;
register int boxlen;
{
    register int sum = 0, i;

    for (i = 0; i < boxlen; i++)
        sum += box[i].cnt;
    return(sum);
}


/*  box_avg_red: Determine the average red value [0..255] of a box
*/
box_avg_red(box, boxlen)
register PIXEL *box;
register int boxlen;
{
    register int sum = 0, n = 0, i;

    for (i = 0; i < boxlen; i++) {
        sum += GETR8(box[i].color);
        n++;
    }
    return(sum / n);
}

/****************************************************************
 * box_avg_grn: Determine the average grn value [0..255] of a box
 ****************************************************************/
box_avg_grn(box, boxlen)
register PIXEL *box;
register int boxlen;
{
    register int sum = 0, n = 0, i;

    for (i = 0; i < boxlen; i++) {
        sum += GETG8(box[i].color);
        n++;
    }
    return(sum / n);
}

/****************************************************************
 * box_avg_blu: Determine the average blu value [0..255] of a box
 ****************************************************************/

box_avg_blu(box, boxlen)
register PIXEL *box;
register int boxlen;
{
    register int sum = 0, n = 0, i;

    for (i = 0; i < boxlen; i++) {
        sum += GETB8(box[i].color);
        n++;
    }
    return(sum / n);
}


/** sort by increasing red ( then green and blue )
 */
cmp_red(a, b)
PIXEL *a, *b;
{
    register r;

    if (r = GETR(a->color) - GETR(b->color))
        return(r);
    if (r = GETG(a->color) - GETG(b->color))
        return(r);
    if (r = GETB(a->color) - GETB(b->color))
        return(r);
    return(0);
}

/* sort by increasing green ( then blue and red )
 */
cmp_grn(a, b)
PIXEL *a, *b;
{
    register r;

    if (r = GETG(a->color) - GETG(b->color))
        return(r);
    if (r = GETB(a->color) - GETB(b->color))
        return(r);
    if (r = GETR(a->color) - GETR(b->color))
        return(r);
    return (0);
}

/* sort by increasing blue ( then red and green )
 */
cmp_blu(a, b)
PIXEL *a, *b;
{
    register r;

    if (r = GETB(a->color) - GETB(b->color))
        return(r);
    if (r = GETR(a->color) - GETR(b->color))
        return(r);
    if (r = GETG(a->color) - GETG(b->color))
        return(r);
    return(0);
}


/* clr_quantize: Do Floyd Steinberg quantizing on the image
 */
clr_quantize(xim, cmap, ncolors, dither_flag)
XimImage *xim;
Color *cmap;
int ncolors, dither_flag;
{
    int **cerr, **lerr, **terr;
    int rowlen = xim->bytes_per_line;
    register int i, j;
    register unsigned  offset;
    int rderr, grerr, blerr, clr;
    int rd, gr, bl;
    byte *rp, *gp, *bp, *obm, *output;
    Color  saved[256];

    bcopy((char *)cmap,  (char *)saved, ncolors * sizeof(Color)); 
    /* Precompute necessary nearest neighbor query info.  Note that
     * we wait until after copying the colormap, since init
     * reorders it
     */
    init_nearest(cmap, ncolors);

    if (!(output = (byte *)malloc(xim->datasize))) {
        XimWarn("clr_quantize: can't malloc output");
        return(0);
    }
    if (!dither_flag) {   /* Just use the nearest color everywhere */
        rp = xim->data;
        gp = xim->grn_data;
        bp = xim->blu_data;
        obm = output;
        for (j=0; j < xim->height; j++)
            for (i=0; i < xim->width; i++) {
                rd = (int)*rp++;
                gr = (int)*gp++;
                bl = (int)*bp++;
                *obm++ = cmap[nearest(rd, gr, bl, cmap, ncolors)].pixel;
            }
        free((char *)xim->data);
        xim->data = output;
        free((char *)xim->grn_data);
        xim->grn_data = (byte *)0;
        free((char *)xim->blu_data);
        xim->blu_data = (byte *)0;
        xim->bits_channel = 8;
        xim->nchannels = 1;
        bcopy((char *)saved,  (char *)cmap, ncolors * sizeof(Color)); 
        return(ncolors);
    }
    /* Do halftoning */
    cerr = (int **)calloc(3, sizeof(int *));
    lerr = (int **)calloc(3, sizeof(int *));
    cerr[RD] = (int *)malloc((unsigned)xim->width * sizeof(int));
    cerr[GR] = (int *)malloc((unsigned)xim->width * sizeof(int));
    cerr[BL] = (int *)malloc((unsigned)xim->width * sizeof(int));
    lerr[RD] = (int *)malloc((unsigned)xim->width * sizeof(int));
    lerr[GR] = (int *)malloc((unsigned)xim->width * sizeof(int));
    lerr[BL] = (int *)malloc((unsigned)xim->width * sizeof(int));
    if (!cerr || !cerr[RD] || !lerr || !lerr[BL]) {
        free((char *)output);
        XimWarn("clr_quantize: can't malloc error distribution");
        return(0);
    }
    /* Top border */
    rp = xim->data;
    gp = xim->grn_data;
    bp = xim->blu_data;
    obm = output;
    for (i=0; i < xim->width; i++)
        obm[i] = cmap[nearest((int)rp[i], (int)gp[i], (int)bp[i], cmap,
                ncolors)].pixel;

    /* Left border */
    rp = xim->data;
    gp = xim->grn_data;
    bp = xim->blu_data;
    obm = output;
    for (j = 0; j < xim->height; j++) {
        offset = j * rowlen;
        obm[offset] = cmap[nearest((int)rp[offset], (int)gp[offset],
            (int)bp[offset], cmap, ncolors)].pixel;
    }
    /* Right border */
    offset = xim->width - 1;
    rp = xim->data + offset;
    gp =  xim->grn_data + offset;
    bp = xim->blu_data + offset;
    obm = output + offset;
    for (j = 0; j < xim->height; j++) {
        offset = j * rowlen;
        obm[offset] = cmap[nearest((int)rp[offset], (int)gp[offset],
                (int)bp[offset], cmap, ncolors)].pixel;
    }
    /* Clear error vectors */
    for (i=0; i < xim->width; i++) {
        cerr[RD][i] = cerr[GR][i] = cerr[BL][i] = 0;
        lerr[RD][i] = lerr[GR][i] = lerr[BL][i] = 0;
    }
    /* Major loop for Floyd-Steinberg */
    for (j=1; j < xim->height; j++) {
        offset = j * xim->width + 1;
        rp = xim->data + offset;
        gp = xim->grn_data + offset;
        bp = xim->blu_data + offset;
        obm = output + offset;
        for (i=1; i < (xim->width - 1); i++) {
            rd = (int)*rp++;
            gr = (int)*gp++;
            bl = (int)*bp++;
            /* Sum up errors using Floyd-Steinberg weights */
            rderr = cerr[RD][i - 1] + 7 * lerr[RD][i - 1] +
                    5 * lerr[RD][i] + 3 * lerr[RD][i + 1];
            grerr = cerr[GR][i - 1] + 7 * lerr[GR][i - 1] +
                    5 * lerr[GR][i] + 3 * lerr[GR][i + 1];
            blerr = cerr[BL][i - 1] + 7 * lerr[BL][i - 1] +
                    5 * lerr[BL][i] + 3 * lerr[BL][i + 1];
            rderr >>= 4;    /* Divide by 16 */
            grerr >>= 4;
            blerr >>= 4;
            rd += rderr; /* Chose nearest color to adjusted RGB value */
            gr += grerr;
            bl += blerr;
            clr = nearest(rd, gr, bl, cmap, ncolors);
            *obm++ = cmap[clr].pixel;

            /* Compute accumulated error for this pixel */
            rd -= (int)cmap[clr].red;
            gr -= (int)cmap[clr].grn;
            bl -= (int)cmap[clr].blu;

            /* Limit error (avoids color shadows) */
            if (rd < -MAXERR || rd > MAXERR)
                rd = (rd * 3) >> 2;
            if (gr < -MAXERR || gr > MAXERR)
                gr = (gr * 3) >> 2;
            if (bl < -MAXERR || bl > MAXERR)
                bl = (bl * 3) >> 2;

            /* Store errors in error vectors */
            cerr[RD][i] = rd;
            cerr[GR][i] = gr;
            cerr[BL][i] = bl;
        }
        /* Swap error vectors */
        terr = lerr;
        lerr = cerr;
        cerr = terr;
    }
    free((char *)lerr[RD]);
    free((char *)lerr[GR]);
    free((char *)lerr[BL]);
    free((char *)cerr[RD]);
    free((char *)cerr[GR]);
    free((char *)cerr[BL]);
    free((char *)lerr);
    free((char *)cerr);
    free((char *)xim->data);
    xim->data = (byte *)0;
    free((char *)xim->grn_data);
    xim->grn_data = (byte *)0;
    free((char *)xim->blu_data);
    xim->blu_data = (byte *)0;
    xim->data = output;
    xim->bits_channel = 8;
    xim->nchannels = 1;
    bcopy((char *)saved,  (char *)cmap, ncolors * sizeof(Color)); 
    return(ncolors);
}


/****************************************************************
 * nearest: Choose nearest color
 ****************************************************************/
short cache[CUBSIZ];

init_nearest(cmap, ncolors)
Color *cmap;
int ncolors;
{
    register int i;
    int cmp_cmap();

    /* Initialize the cache */
    for (i=0; i < CUBSIZ; i++)
        cache[i] = -1;
    /* Sort the colormap by decreasing red, then green, then blue */
    qsort((char *)cmap, ncolors, sizeof(Color), cmp_cmap);

    if (debug_flag) {
        fprintf(stderr, "\nFinal colormap:\n\n");
        for (i = 0; i < ncolors; i++)
            fprintf(stderr, "%3d:  (%3u,%3u,%3u) [%u]\n", i,
            cmap[i].red, cmap[i].grn, cmap[i].blu, cmap[i].pixel);
    }
}

/* Fast square macro, uses local variable to avoid mulitple eval of X */
#define SQR(X)      (sqrtmp = (X), sqrtmp*sqrtmp)

/* Fast distance macro */
#define CDIST(CPTR, CR, CG, CB)\
    (sumtmp =  SQR (((int)((CPTR)->red)) - CR),\
    sumtmp += SQR (((int)((CPTR)->grn)) - CG),\
    sumtmp += SQR (((int)((CPTR)->blu)) - CB), sumtmp)
#define restrict(X)     ((X) < 0 ? 0 : (X) > 255 ? 255 : (X))

nearest(rd, gr, bl, cmap, ncolors)
int rd, gr, bl;
Color *cmap;
int ncolors;
{
    register int clr, sqrtmp, sumtmp;
    register Color *a, *b, *c = NULL, *best, *ctail;
    int cindx, bestd, dist;

    rd = restrict(rd);
    gr = restrict(gr);
    bl = restrict(bl);

    /* Find array index in cache */
    cindx = CLRINDEX8(rd, gr, bl);

    /* Check cache for color value */
    if ((clr = cache[cindx]) >= 0)
        return(clr);
    /* Search through colormap for nearest neighbor: 1) Find
     * nearest red value by binary search 2) Search up and down,
     * keeping best point 3) Terminate search when red difference
     * is greater than best pt */

    /* Binary search for nearest red value */
    ctail = &cmap[ncolors];
    for (a = cmap, b = ctail - 1; a < b;) {
        c = a + (b - a) / 2;    /* Find midpoint */

        if (debug_flag && verbose)
            fprintf(stderr,
                "Searching for %d, a=%d (%d), b=%d (%d), c=%d (%d)\n",
                rd, a - cmap, a->red, b - cmap, b->red, c - cmap, c->red);

        if (c->red == rd)
            break;
        else if (c->red < rd)
            a = ++c;
        else
            b = c;
    }
    /* c now points to closest red value, search up and down for
     * closer Euclidean distance, and stop search when the red
     * distance alone exceeds the bext found. */

    /* Set best and bestd to best red value */
    best = c;
    bestd = CDIST(c, rd, gr, bl);
    if (debug_flag && verbose)
        fprintf(stderr,
            "Found c = %d (%d)  dist = %d\n", c - cmap, c->red, bestd);

    /* a and b are search pointers up and down */
    a = c - 1;
    b = c + 1;

    while (bestd > 0 && (a >= cmap || b < ctail)) {
        if (debug_flag && verbose)
            fprintf(stderr,
                "  search: bestd %d, best %d|%d, a %d, b %d\n",
                bestd, best - cmap, best->pixel, a - cmap, b - cmap);
        if (a >= cmap) {
            if ((dist = CDIST(a, rd, gr, bl)) < bestd) {
                best = a;
                bestd = dist;
            }
            if (SQR((int) a->red - rd) >= bestd)
                a = cmap - 1;
            else
                a--;
        }
        if (b < ctail) {
            if ((dist = CDIST(b, rd, gr, bl)) < bestd) {
                best = b;
                bestd = dist;
            }
            if (SQR((int) b->red - rd) >= bestd)
                b = ctail;
            else
                b++;
        }
    }
    if (best < cmap || best >= ctail) {
        perror("returning invalid color\n");
        abort();
    }
    clr = (best - cmap);
    if (debug_flag)
        fprintf(stderr, "Nearest(%3d,%3d,%3d) => (%3d,%3d,%3d) [%d]\n",
           rd, gr, bl, best->red, best->grn, best->blu, best->pixel);
    return ((cache[cindx] = clr));
}


/****************************************************************
 * sort colormap by decreasing red ( then green and blue )
 ****************************************************************/
cmp_cmap(a, b)
Color *a, *b;
{
    register int r;

    if (r = (a->red - b->red))
        return(r);
    if (r = (a->grn - b->grn))
        return(r);
    if (r = (a->blu - b->blu))
        return(r);
    return (0);
}


/****************************************************************
 * sort colormap by increasing intensity (Use NTSC weights)
 ****************************************************************/
#define RW 299
#define GW 587
#define BW 114

cmp_int(a, b)
register Color *a, *b;
{
    register int ai, bi;

    ai = RW * a->red + GW * a->grn + BW * a->blu;
    bi = RW * b->red + GW * b->grn + BW * b->blu;
    return (ai - bi);
}

/*** end fbquant.c ***/
