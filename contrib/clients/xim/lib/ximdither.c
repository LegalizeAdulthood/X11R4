/***********************************************************************
*  File:   dither.c
*  Author: Philip Thompson
*  $Date: 89/11/02 16:19:07 $
*  $Revision: 1.2 $
*  Purpose: Library of floyd-steingerg dithering routines.
*           Classical Floyd-Steinberg halftoning with 4 pixel error
*           distribution.
*
*  Copyright (c) 1988, 1989  Philip R. Thompson (phils@athena.mit.edu)
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
static char dither_rcs_id[] =
    "$Header: /mit/phils/X/Lib/RCS/ximdither.c,v 1.2 89/11/02 16:19:07 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include "XimHeader.h"

#define WHITE 255
#define THRESH 127
#define BLACK 0
#define INITERR(X,Y)    (X - (Y ? WHITE : BLACK) + (THRESH - X)/2)

int GreyToBitmap_fs(), GreyToBitmap_dither();
int (*bw)() = GreyToBitmap_fs;
int dm4[4][4] = {
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 }
};
int dm8[8][8] = {
   {  0, 32,  8, 40,  2, 34, 10, 42 },
   { 48, 16, 56, 24, 50, 18, 58, 26 },
   { 12, 44,  4, 36, 14, 46,  6, 38 },
   { 60, 28, 52, 20, 62, 30, 54, 22 },
   {  3, 35, 11, 43,  1, 33,  9, 41 },
   { 51, 19, 59, 27, 49, 17, 57, 25 },
   { 15, 47,  7, 39, 13, 45,  5, 37 },
   { 63, 31, 55, 23, 61, 29, 53, 21 }
};
int  dm16[16][16];
int  modN[256], divN[256], ditherFactor = 8;
int  levels, levelsq;
char  *calloc();
extern  debug_flag;


/* Grey to monochrome conversion. Performed in place - overwrites
* the source pixmap and transforms it into a bitmap in the process.
*/
GreyToBitmap_fs(xim)
XimImage *xim;
{
    register byte *pmp, *mbuffer, *bbuffer, mvalue, *first;
    register int i, j, gray, error, bit, width, height;
    int *lerr, *cerr, *terr, *error1, *error2;

    if (debug_flag)
        fprintf(stderr, "GreyToBitmap_fs ...");
    width = xim->width;
    height = xim->height;
    xim->bytes_per_line = (xim->width + 7) / 8;
    xim->datasize = xim->bytes_per_line * xim->height;
    xim->bits_channel = 1;
    xim->nchannels = 1;

    /* Allocate space for error arrays */
    error1 = (int *)calloc((unsigned)(width+2), sizeof(int));
    error2 = (int *)calloc((unsigned)(width+2), sizeof(int));
    bbuffer = mbuffer = (byte *)calloc(xim->datasize,  sizeof(byte));
    if ((error1 == NULL) || (error2 == NULL) || (mbuffer == NULL)) {
        XimWarn("calloc() in GreyToBitmap conversion");
        return(0);
    }
    
    cerr = &error1[1];
    lerr = &error2[1];
    for (error=0, i=0; i < width; ) {        /* The top border */
        mvalue = 0x00;
        for (j=0; (j < 8) && (i < width); j++, i++) {
            gray = (int)xim->data[i] + error;
            bit = gray > THRESH ? WHITE : BLACK;
            if (bit)
                    mvalue |= (0x01 << j);    /* for Vax */
            error = gray - bit;
            lerr[i] = (THRESH - (int)bit) / 2;
        }
        *mbuffer++ = ~mvalue;
    }
    /* Process the rest.            1 5 3
    *  Error distribution:          7 x
    */
    for (j=1; j < height; j++) {
        pmp = &xim->data[j*width];
        first = mbuffer;
        for (i=0; i < width; ) {
            mvalue = 0x00;
            for (bit=0; (bit < 8) && (i < width); bit++, i++) {
                gray = (int)pmp[i] + (int)(lerr[i-1] + (5 * lerr[i]) +
                        (3 * lerr[i+1]) + (7 * cerr[i-1])) / 16;
                if (gray > THRESH) {
                      /*  mvalue |= (0x80 >> bit);     for RT, Sun  */
                        mvalue |= (0x01 << bit);    /* for Vax */
                    cerr[i] = gray - WHITE;
                } else
                    cerr[i] = gray;
            }
            *mbuffer++ = ~mvalue;
        }
        cerr[-1] = INITERR((int)pmp[-1], (*first & 0x01));
        /* Swap error buffers */
        terr = error1;
        error1 = error2;
        error2 = terr;
        cerr = &error1[1];
        lerr = &error2[1];
    }
    free((char *)error1);
    free((char *)error2);
    free((char *)xim->colors);
    xim->colors = NULL;
    xim->ncolors = 1;
    free((char *)xim->data);
    xim->data = bbuffer;
    if (debug_flag)
        fprintf(stderr, "Done\n");
    return(1);
}


/* b/w 8x8 matrix dithering function
*/
GreyToBitmap_dither(xim)
XimImage *xim;
{
    byte *buffer, *bbuffer, *mbuffer, mvalue;
    register i, j, k;

    if (debug_flag)
        fprintf(stderr, "GreyToBitmap_fs ...");
    buffer = xim->data;
    xim->bytes_per_line = (xim->width + 7) / 8;
    xim->datasize = xim->bytes_per_line * xim->height;
    xim->bits_channel = 1;
    xim->nchannels = 1;
    bbuffer = mbuffer = (byte *)calloc(xim->datasize,  sizeof(byte));
    if (bbuffer == NULL) {
        XimWarn("GreyToBitmap_dither: can't calloc() buffer");
        return(0);
    }
    for (i=0; i < xim->height; i++) {
        for (j=0; j < xim->width; ) {
            mvalue = 0x00;
            for (k=0; (k < 8) && (j < xim->width); k++, j++)
                if (*buffer++ > dm8[i % 8][j % 8])
                    mvalue |= (0x01 << k);
            *mbuffer++ = ~mvalue;
        }
    }
    free((char *)xim->colors);
    xim->colors = NULL;
    xim->ncolors = 1;
    free((char *)buffer);
    xim->data = bbuffer;
    if (debug_flag)
        fprintf(stderr, "Done\n");
    return(1);
}


/* Normalize dither matrix to 255 maximum value.
*/
NormalizeDM(dimension)
int dimension;
{
    int i, j;
    float normalValue;

    normalValue = 256.0 / (float)(dimension * dimension);
    for (i=0; i < dimension; i++) {
        for (j=0; j < dimension; j++) {
            dm8[i][j] = rnd(dm8[i][j] * normalValue);
            if (debug_flag) {
                fprintf(stderr,"%8d", dm8[i]);
                if  (((i+1) % dimension) == 0)
                    fprintf(stderr,"\n");
            }
        }
    }
    return(1);
}


/*****************************************
*  Color Dithering 
*****************************************/

/* Map rg&b channels to 8 bits through dithering.  This color dithering
* code has been adapted and greatly simplified from the original work by
*   author: Spencer W. Thomas
*           Computer Science Dept. (cs.utah.edu)
*           University of Utah
* Copyright (c) 1986, University of Utah
*/
Rgb_dither(xim)
XimImage *xim;
{
    register i, x, y, col=0, row=0, width, height;
    register byte *red, *grn, *blu;

    red = xim->data;
    grn = xim->grn_data;
    blu = xim->blu_data;
    width = xim->width;
    height = xim->height;
    for (i=0, y=0; y < height; y++, row=y%16) {
        for (x=0; x < width; x++, col=x%16, i++) {
            red[i] = (byte)(
                (modN[red[i]] > dm16[col][row] ? divN[red[i]]+1 :
                divN[red[i]]) +
                (modN[grn[i]] > dm16[col][row] ? divN[grn[i]]+1 :
                divN[grn[i]]) * levels +
                (modN[blu[i]] > dm16[col][row] ? divN[blu[i]]+1 :
                divN[blu[i]]) * levelsq);
        }
    }
    free((char *)xim->grn_data);
    xim->grn_data = NULL;
    free((char *)xim->blu_data);
    xim->blu_data = NULL;
    xim->nchannels = 1;
    xim->bits_channel = 8;
    return(1);
}


byte DitherPixel(red, grn, blu, row, col)
byte red, grn, blu;
int row, col;
{
    register value;

    row %= 16;
    col %= 16;
    value = (modN[red] > dm16[col][row] ? divN[red] + 1 : divN[red]) +
        (modN[grn] > dm16[col][row] ? divN[grn]+1 : divN[grn]) *levels +
        (modN[blu] > dm16[col][row] ? divN[blu]+1 : divN[blu]) *levelsq;
    return((byte)value);
}


Pixmap_dither(xim)
XimImage  *xim;
{
    register x, y, col=0, row=0, width, height;
    register byte red, grn, blu, *loc, *buf;

    buf = xim->data;
    width = xim->width;
    height = xim->height;
    if (debug_flag)
        fprintf(stderr,"Pixmap_dither: levels = %d\n", levels);
    for (loc = buf, y=0; y < height; y++, row=y%16) {
        for (x=0; x < width; x++, col=x%16, loc++) {
            red = xim->colors[*loc].red;
            grn = xim->colors[*loc].grn;
            blu = xim->colors[*loc].blu;
            *loc = (byte)(
                (modN[red] > dm16[col][row] ? divN[red]+1 : divN[red]) +
                (modN[grn] > dm16[col][row] ? divN[grn]+1 : divN[grn])
                * levels +
                (modN[blu] > dm16[col][row] ? divN[blu]+1 : divN[blu])
                * levelsq);
        }
    }
    return(1);
}


DitherToDisplay(xim, enhance, mono_flag, cdither_flag)
XimImage  *xim;
int enhance, mono_flag, cdither_flag;
{
    if (xim->bits_channel == 1 && xim->nchannels == 1)
        return(1);
    if (mono_flag && cdither_flag)
        cdither_flag = 0;   /* mono_flag wins */
    if (mono_flag) {               /* make a bitmap image  */
        ColorToGrey(xim);
        if (bw == GreyToBitmap_dither)
            NormalizeDM(ditherFactor);
        if (enhance)
            Edge_enhance(xim, enhance);
        /* Translate pixmap into a bitmap for monochrome display. */
        (void)(*bw)(xim);
    } else {                      /* use color image */
        if (xim->nchannels >= 3) {
            if (enhance)
                Edge_enhance(xim, enhance);
            Rgb_dither(xim);
        } else if (cdither_flag)
            Pixmap_dither(xim);
    }
    if (xim->npics <= 1 && xim->nchannels == 3) {
        if (xim->grn_data) {
            free((char *)xim->grn_data);
            xim->grn_data = NULL;
        }
        if (xim->blu_data) {
            free((char *)xim->blu_data);
            xim->blu_data = NULL;
        }
    }
    return(1);
}


/* Create a color map and dithering matrix for the specified
*  intensity levels.
*/
int MakeDithermap(factor, colors, maxcolors)
int  factor, maxcolors;
Color colors[];
{
    float N, magicfact;
    register i, j, k, l, ncolors;
    extern int dm4[4][4], dm16[16][16], divN[256], modN[256];
    extern int levels, levelsq, ditherFactor;

    for (i = factor; i > 1; i--)
        if ((i*i*i) <= maxcolors)
            break;
    ditherFactor = levels = i;
    levelsq = levels * levels;      /* squared */
    ncolors = levelsq * levels;     /* and cubed */
    if (levels < 2) {
        XimWarn("Make_Xdithermap: can't work with less than 2 levels");
        return(0);
    }
    N = 255.0 / (levels-1.0);       /* Get size of each step */

    /* Set up the color map entries.  */
    for (i=0; i < ncolors; i++) {
        colors[i].pixel = (byte)i;
        colors[i].red = (byte)rnd((i%levels) * N);
        colors[i].grn = (byte)rnd(((i/levels)%levels) * N);
        colors[i].blu = (byte)rnd(((i/levelsq)%levels) * N);
    }
    for (i=0; i < 256; i++) {
        divN[i] = (int)(i / N);
        modN[i] = i - (int)(N * divN[i]);
    }
    magicfact = (N - 2.0) / 16.0;
    for (i=0; i < 4; i++)
        for (j=0; j < 4; j++)
            for (k=0; k < 4; k++)
                for (l=0; l < 4; l++)
                    dm16[4*k+i][4*l+j] = rnd(dm4[i][j] * magicfact +
                                        (dm4[k][l]/16.0) * magicfact);
    if (debug_flag) {
        fprintf(stderr,"Levels = %d  Ncolors = %d\n", levels, ncolors);
        for (i=0; i < 16; i++) {
            for (j=0; j < 16; j++)
                fprintf(stderr,"%4d", dm16[i][j]);
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
    }
    return(ncolors);
}

/* end dither.c */
