/***********************************************************************
*  File:   xconvert.c
*  Author: Philip Thompson
*  $Date: 89/11/02 16:18:58 $
*  $Revision: 1.2 $
*  Purpose: Xim functions for converting images.
*  Copyright (c) 1988  Philip R. Thompson (phils@athena.mit.edu)
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
*   I provide this software freely as a public service.  It is NOT a
*   commercial product, and therefore is not subject to an an implied
*   warranty of merchantability or fitness for a particular purpose.  I
*   provide it as is, without warranty.
*   This software is furnished  only on the basis that any party who
*   receives it indemnifies and holds harmless the parties who furnish
*   it against any claims, demands, or liabilities connected with using
*   it, furnishing it to others, or providing it to a third party.
*
*   Philip R. Thompson (phils@athena.mit.edu)
***********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char ximlib_rcs_id[] =
    "$Header: /mit/phils/X/Lib/RCS/ximconvert.c,v 1.2 89/11/02 16:18:58 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include "XimHeader.h"

extern  debug_flag;
char  *malloc(), *calloc(), *realloc();

/* This edge enhancing is taken from the ACM Transaction on Graphics
*  Vol. 6, No. 4, October 1987.  Dot diffusion is not implemented.
*/
Edge_enhance(xim, enhance)
XimImage *xim;
{
    if (!(_edge_enhance(xim->data, xim->width, xim->height, enhance)))
        return(0);
    if (xim->nchannels == 3) {
        if (!(_edge_enhance(xim->grn_data, xim->width, xim->height,
                enhance)))
            return(0);
        if (!(_edge_enhance(xim->blu_data, xim->width, xim->height,
                enhance)))
            return(0);
    }
    return(1);
}

_edge_enhance(buf, width, height, enhance)
register byte  *buf;
register width, height, enhance;
{
    register i, x, y, lbyt, hbyt, tmp;
    register byte *tbuf;
    register float phi;
    unsigned bufsize = (unsigned)width * height;

    if (debug_flag)
        fprintf(stderr,"enhancing... ");
    if ((tbuf = (byte *)malloc(bufsize)) == NULL) {
        XimWarn("Can't malloc() core for edge enhancementi.\n");
        return(0);
    }
    bcopy((char *)buf, (char *)tbuf, (int)bufsize);
    lbyt = width-1;
    hbyt = width+1;
    i = lbyt;
    if (enhance == 9) {             /* is much faster, default */
        for (y=2; y < height; y++) {
            i += 2;
            for (x=2; x < width; x++,i++) {
                tmp = (9 * tbuf[i]) - (tbuf[i-hbyt] + tbuf[i-width]+
                    tbuf[i-lbyt] + tbuf[i-1] + tbuf[i+1] +
                    tbuf[i+lbyt] + tbuf[i+width] + tbuf[i+hbyt]);
                if (tmp > 255)
                    buf[i] = 255;
                else if (tmp < 0)
                    buf[i] = 0;
                else
                    buf[i] = (byte)tmp;
            }
        }
    } else {                        /* allows greater control */
        phi = enhance / 10.0;
        for (y=2; y < height; y++) {
            i += 2;
            for (x=2; x < width; x++,i++) {
                tmp = (tbuf[i-hbyt] + tbuf[i-width] + tbuf[i-lbyt] +
                    tbuf[i-1] + tbuf[i] + tbuf[i+1] + tbuf[i+lbyt] +
                    tbuf[i+width] + tbuf[i+hbyt]) / 9.0;
                tmp = rnd((tbuf[i]-(phi*tmp)) / (1.0-phi));
                if (tmp > 255)
                    buf[i] = 255;
                else if (tmp < 0)
                    buf[i] = 0;
                else
                    buf[i] = (byte)tmp;
            }
        }
    }
    free((char *)tbuf);
    return(1);
}


/* Transformation from RGB to the Y (or luminence) factor from
*  YIQ encoding.
*/
unsigned GreyValue(red, green, blue)
unsigned short red, green, blue;
{
    return((unsigned)rnd((float)red * 0.30 + (float)green * 0.55 +
            (float)blue * 0.15));
}

ColorToGrey(xim)
XimImage *xim;
{
    int i;

    if (xim->bits_channel == 1 && xim->nchannels == 1) {
        fprintf(stderr, "ColorToGrey: can't convert bitmaps.\n");
        return(1);
    }
    if (xim->nchannels == 3) {      /* convert to 8-bit grey values */
        for (i=0; i < xim->datasize; i++)
            xim->data[i] = (byte)GreyValue(xim->data[i],
                    xim->grn_data[i], xim->blu_data[i]);
        xim->nchannels = 1;
        xim->bits_channel = 8;
    } else
        ColorPixmapToGrey(xim);
    return(1);
}


ColorPixmapToGrey(xim)
XimImage *xim;
{
    register int  i;
    byte  tval[256];

    bzero((char *)tval, 256);
    /* determine b/w intensity level and place in pixel */
    if (debug_flag)
        fprintf(stderr,"making grey pixmap ...\n");
    for (i=0; i < xim->ncolors; i++) {
        tval[i] = (byte)GreyValue(xim->colors[i].red,
            xim->colors[i].grn, xim->colors[i].blu);
        if (debug_flag)
            fprintf(stderr,
            "color[%3d]  r: %5u  g: %5u  b: %5u  new: %3u\n", i,
                xim->colors[i].red, xim->colors[i].grn,
                xim->colors[i].blu, tval[i]);
    }
    for (i=0; i < xim->datasize; i++)
        xim->data[i] = tval[xim->data[i]];
    xim->ncolors = 256;
    xim->nchannels = 1;
    xim->bits_channel = 8;
    if (!(xim->colors = (Color *)realloc((char *)xim->colors,
            (unsigned)xim->ncolors * sizeof(Color))))
        return(0);
    for (i=0; i < xim->ncolors; i++)
        xim->colors[i].red = xim->colors[i].grn = xim->colors[i].blu =i;
    return(1);
}


BitmapToPixmap(xim)
XimImage *xim;
{
    int i, j, k, l;
    unsigned size;
    byte  *pbuf, *pdata, *bdata;

    size = (unsigned)xim->width * xim->height;
    if ((pbuf = (byte *)malloc(size)) == NULL) {
        XimWarn("BitmapToPixmap: can't malloc pixmap buffer");
        return(0);
    }
    for (i=0; i < xim->height; i++) {
        bdata = xim->data + i * xim->bytes_per_line;
        pdata = pbuf + i * xim->width;
        for (j=0, l=0; j < xim->bytes_per_line && l < xim->width; j++)
            for (k=0; k < 8 && l < xim->width; k++, l++)
                if (bdata[j] & (0x01 << k))
                    pdata[l] = 0;
                else
                    pdata[l] = 1;
    }
    xim->ncolors = 2;
    if (xim->colors)
        xim->colors = (Color *)realloc((char *)xim->colors,
                (unsigned)xim->ncolors * sizeof(Color));
    else
        xim->colors = (Color *)calloc((unsigned)xim->ncolors,
                sizeof(Color));
    if (xim->colors == NULL) {
        XimWarn("BitmapToPixmap: can't malloc colors");
        return(0);
    }
    free((char *)xim->data);
    xim->data = pbuf;
    xim->bytes_per_line = xim->width;
    xim->datasize = size;
    xim->bits_channel = 8;
    xim->nchannels = 1;
    xim->colors[0].red = 0;
    xim->colors[0].grn = 0;
    xim->colors[0].blu = 0;
    xim->colors[1].red = 255;
    xim->colors[1].blu = 255;
    xim->colors[1].grn = 255;
    return(1);
}


PixmapToBitmap(xim)
XimImage *xim;
{
    register  i, j, bit;
    register byte  *mbuffer, *mpbuffer,  mvalue, *pix;

    xim->bytes_per_line = (xim->width + 7) / 8;
    xim->datasize = xim->height * xim->bytes_per_line;
    if (xim->ncolors > 2)
        XimWarn("PixmapToBitmap: too many colors");
    if (xim->colors) {
        free((char *)xim->colors);
        xim->colors = (Color*)0;
    }
    xim->ncolors = 0;
    xim->bits_channel = 1;
    xim->nchannels = 1;
    if ((mpbuffer = (byte *)calloc(xim->datasize, 1)) == NULL) {
        XimWarn("PixmapToBitmap: can't calloc() bitmap.");
        return(0);
    }
    mbuffer = mpbuffer;
    pix = xim->data;
    for (i=0; i < xim->height; i++)
        for (j=0; j < xim->width; ) {
            mvalue = 0x00;
            for (bit=0; (bit < 8) && (j < xim->width); bit++, j++)
                if (*pix++)
                    mvalue |= (0x01 << bit);   /* LSBFirst order */
            *mbuffer++ = mvalue;
        }
    free((char *)xim->data);
    xim->data = mpbuffer;
    return(1);
}


GreyToBitmap_threshold(xim, thresh)
XimImage *xim;
int thresh;
{
    register  i, j, bit;
    register byte  *mbuffer, *mpbuffer,  mvalue, *pix;

    if (thresh == -1)
        thresh = 127;
    xim->bytes_per_line = (xim->width + 7) / 8;
    xim->datasize = xim->height * xim->bytes_per_line;
    if (xim->colors) {
        free((char *)xim->colors);
        xim->colors = (Color*)0;
    }
    xim->ncolors = 0;
    xim->bits_channel = 1;
    xim->nchannels = 1;
    if ((mpbuffer = (byte *)calloc(xim->datasize, 1)) == NULL) {
        XimWarn("PixmapToBitmap: can't calloc() bitmap.");
        return(0);
    }
    mbuffer = mpbuffer;
    pix = xim->data;
    for (i=0; i < xim->height; i++)
        for (j=0; j < xim->width; ) {
            mvalue = 0x00;
            for (bit=0; (bit < 8) && (j < xim->width); bit++, j++)
                if (*pix++ > thresh)
                    mvalue |= (0x01 << bit);   /* LSBFirst order */
            *mbuffer++ = ~mvalue;
        }
    free((char *)xim->data);
    xim->data = mpbuffer;
    return(1);
}


OneToThree(xim)
XimImage *xim;
{
    register i;
    register byte val, *grn, *blu;

    if (xim->nchannels == 3 && xim->bits_channel == 8) {
        XimWarn("OneToThree: already in 24 bit format.");
        return(0);
    }
    grn = (byte *)malloc(xim->datasize);
    blu = (byte *)malloc(xim->datasize);
    if (grn == NULL || blu == NULL) {
        XimWarn("OneToThree: can't malloc grn or blu data");
        return(0);
    }
    for (i=0; i < xim->datasize; i++) {
        val = xim->data[i];
        xim->data[i] = xim->colors[val].red;
        grn[i] = xim->colors[val].grn;
        blu[i] = xim->colors[val].blu;
    }
    if (xim->ncolors)
        xim->ncolors = 0;
    if (xim->colors) {
        free((char *)xim->colors);
        xim->colors = NULL;
    }
    xim->grn_data = grn;
    xim->blu_data = blu;
    xim->nchannels = 3;
    return(1);
}


RefineColors(xim)
XimImage *xim;
{
    register i, j;
    int indices[256];
    byte temp[256][3];
    register byte *bp, *ep;
    char *realloc();

    if (!xim->colors || !xim->ncolors) {
        XimWarn("RefineColors: no colortable");
        return(0);
    }
    bzero((char *)indices, (256 * sizeof(int)));
    bp = xim->data;
    ep = xim->data + xim->datasize;
    do {
        indices[*bp]++;
    } while (++bp < ep);
    for (i=0, j=0; i < 256; i++) {
        if (indices[i] > 0) {
            indices[i] = j;
            temp[j][0] = (byte)xim->colors[i].red;
            temp[j][1] = (byte)xim->colors[i].grn;
            temp[j][2] = (byte)xim->colors[i].blu;
            j++;
        }
    }
    xim->ncolors = j;
    if (!(xim->colors = (Color *)realloc((char *)xim->colors,
            (unsigned)(xim->ncolors * sizeof(Color))))) {
        XimWarn("RefineColors: can't realloc() colors");
        return(0);
    }
    for (i=0; i < xim->ncolors; i++) {
        xim->colors[i].red = temp[i][0];
        xim->colors[i].grn = temp[i][1];
        xim->colors[i].blu = temp[i][2];
    }
    bp = xim->data;
    do {
        *bp = (byte)indices[*bp];
    } while (++bp < ep);
    return(1);
}


InvertColors(xim)
XimImage *xim;
{
    int i;
    register byte *bp, *ep;

    if (xim->nchannels == 1 && xim->bits_channel == 1) {
        bp = xim->data;
        ep = xim->data + xim->datasize;
        do {
            *bp = ~*bp;
        } while (++bp < ep);
    } else if (xim->nchannels == 1 && xim->bits_channel == 8) {
        for (i=0; i < xim->ncolors; i++) {
            xim->colors[i].red = ~xim->colors[i].red;
            xim->colors[i].grn = ~xim->colors[i].grn;
            xim->colors[i].blu = ~xim->colors[i].blu;
        }
    } else {
        bp = xim->data;
        ep = xim->data + xim->datasize;
        do {
            *bp = ~*bp;
        } while (++bp < ep);
        bp = xim->grn_data;
        ep = xim->grn_data + xim->datasize;
        do {
            *bp = ~*bp;
        } while (++bp < ep);
        bp = xim->blu_data;
        ep = xim->blu_data + xim->datasize;
        do {
            *bp = ~*bp;
        } while (++bp < ep);
    }
    return(1);
}


SetColors(xim, colors, ncolors)
XimImage *xim;
Color *colors;
int ncolors;
{
    int i;

    if (xim->colors) {
        free((char *)xim->colors);
        xim->colors = (Color *)0;
        xim->ncolors = 0;
    }
    if ((xim->colors = (Color *)calloc((unsigned)ncolors,
                    sizeof(Color))) == NULL) {
        XimWarn("SetColors: can't calloc() colors.");
        return(0);
    }
    xim->ncolors = ncolors;
    for (i=0; i < xim->ncolors; i++) {
        xim->colors[i].red = colors[i].red;
        xim->colors[i].grn = colors[i].grn;
        xim->colors[i].blu = colors[i].blu;
    }
    if (debug_flag)
        for (i=0; i < xim->ncolors; i++)
            fprintf(stderr, "%3d:  (%3u,%3u,%3u) [%u]\n", i,
            colors[i].red, colors[i].grn,colors[i].blu,colors[i].pixel);
    xim->bits_channel = 8;
    xim->nchannels = 1;
    return(1);
}

/*** end xconvert.c ***/
