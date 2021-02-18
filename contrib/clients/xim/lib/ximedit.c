/**********************************************************************/
/*  file:   $Source: /mit/phils/X/Lib/RCS/ximedit.c,v $        */
/*                                                                    */
/*  author: Philip R. Thompson, ($Author: phils $)                    */
/*          $Date: 89/11/02 16:19:15 $      $State: Exp $             */
/*                                                                    */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char ximedit_rcsid[] =
        "$Header: /mit/phils/X/Lib/RCS/ximedit.c,v 1.3 89/11/02 16:19:15 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include "XimHeader.h"

extern int debug_flag;
char *malloc();

XimRotate90(xim)
XimImage *xim;
{
    int i, j, k, nchannels;
    int oj, ok, outwidth, outheight;
    byte *data[4], *buf;

    data[0] = xim->data;
    data[1] = xim->grn_data;
    data[2] = xim->blu_data;
    data[3] = xim->other;
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    outheight = xim->width;
    outwidth = xim->height;
    for (i=0; i < nchannels; i++) {
        if (data[i]) {
            if ((buf = (byte *)malloc(xim->datasize)) == (byte*)0) {
                XimWarn("XimRotate90: can't malloc buffer");
                return(0);
            }
            for (j=0; j < xim->height; j++) {
                for (k=0; k < xim->bytes_per_line; k++) {
                    oj = xim->width - (k + 1);
                    ok = j;
                    buf[oj * outwidth + ok] =
                            data[i][j * xim->bytes_per_line + k];
                }
            }
            free((char*)data[i]);
            data[i] = buf;
        }
    }
    xim->width = xim->bytes_per_line = outwidth;
    xim->height = outheight;
    xim->data = data[0];
    xim->grn_data = data[1];
    xim->blu_data = data[2];
    xim->other = data[3];
    return(1);
}


XimRotate270(xim)
XimImage *xim;
{
    int i, j, k, nchannels;
    int oj, ok, outwidth, outheight;
    byte *data[4], *buf;

    data[0] = xim->data;
    data[1] = xim->grn_data;
    data[2] = xim->blu_data;
    data[3] = xim->other;
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    outheight = xim->width;
    outwidth = xim->height;
    for (i=0; i < nchannels; i++) {
        if (data[i] == (byte *)0)
            continue;
        if ((buf = (byte *)malloc(xim->datasize)) == (byte*)0) {
            XimWarn("XimRotate270: can't malloc buffer");
            return(0);
        }
        for (j=0; j < xim->height; j++) {
            for (k=0; k < xim->bytes_per_line; k++) {
                oj = k;
                ok = xim->height - (j + 1);
                buf[oj * outwidth + ok] =
                        data[i][j * xim->bytes_per_line + k];
            }
        }
        free((char*)data[i]);
        data[i] = buf;
    }
    xim->width = xim->bytes_per_line = outwidth;
    xim->height = outheight;
    xim->data = data[0];
    xim->grn_data = data[1];
    xim->blu_data = data[2];
    xim->other = data[3];
    return(1);
}


XimFlipHorizontal(xim)
XimImage *xim;
{
    int i, j, k, nchannels;
    byte *data[4], *buf, *from, *to;

    if (debug_flag)
        fprintf(stderr, "XimFlipHorizontal() called\n");
    data[0] = xim->data;
    data[1] = xim->grn_data;
    data[2] = xim->blu_data;
    data[3] = xim->other;
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    for (i=0; i < nchannels; i++) {
        if (data[i]) {
            if ((buf = (byte *)malloc(xim->datasize)) == (byte*)0) {
                XimWarn("XimFlipHorizontal: can't malloc buffer");
                return(0);
            }
            to = buf; 
            from = data[i] + xim->bytes_per_line;
            for (j=0; j < xim->height; j++) {
                for (k=0; k < xim->bytes_per_line; k++) {
                    from--;
                    *to = *from;
                    to++;
                }
                from += (2 * xim->bytes_per_line);
            }
            free((char *)data[i]);
            data[i] = buf;
        }
    }
    xim->data = data[0];
    xim->grn_data = data[1];
    xim->blu_data = data[2];
    xim->other = data[3];
    return(1);
}


XimFlipVertical(xim)
XimImage *xim;
{
    int i, j, nchannels;
    byte *data[4], *buf, *from, *to;

    if (debug_flag)
        fprintf(stderr, "XimFlipVertical() called\n");

    data[0] = xim->data;
    data[1] = xim->grn_data;
    data[2] = xim->blu_data;
    data[3] = xim->other;
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    for (i=0; i < nchannels; i++) {
        if (data[i] == (byte*)0)
            continue;
        if ((buf = (byte *)malloc(xim->datasize)) == (byte*)0) {
            XimWarn("XimFlipVertical: can't malloc buffer");
            return(0);
        }
        to = buf + xim->datasize;
        from = data[i];
        for (j=0; j < xim->height; j++) {
            to -= xim->bytes_per_line;
            bcopy((char *)from, (char *)to, xim->bytes_per_line);
            from += xim->bytes_per_line;
        }
        free((char*)data[i]);
        data[i] = buf;
    }
    xim->data = data[0];
    xim->grn_data = data[1];
    xim->blu_data = data[2];
    xim->other = data[3];
    return(1);
}



/* Lifted from xgif.c
*/
XimFastScale(xim, xs, ys)
XimImage *xim;
float xs, ys;
{
    int i, nchannels;
    int ix, iy, ex, ey, eWide, eHigh;
    byte *buf, *ilptr, *ipptr, *elptr, *epptr, *data[4];

    if (debug_flag)
        fprintf(stderr, "XimFastScale() called\n");
    data[0] = xim->data;
    data[1] = xim->grn_data;
    data[2] = xim->blu_data;
    data[3] = xim->other;
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    eWide = (int)(xs * (float)xim->width + 0.5);
    eHigh = (int)(ys * (float)xim->height + 0.5);
    for (i=0; i < nchannels; i++) {
        if (data[i] == (byte *)0)
            continue;
        if (!(buf = (byte *)malloc((unsigned)(eWide * eHigh)))) {
            XimWarn("XimFastScale: can't malloc buffer");
            return(0);
        }
        elptr = epptr = buf;
        for (ey=0; ey < eHigh; ey++, elptr += eWide) {
            iy = (xim->height * ey) / eHigh;
            epptr = elptr;
            ilptr = data[i] + (iy * xim->bytes_per_line);
            for (ex=0; ex < eWide; ex++, epptr++) {
                ix = (xim->bytes_per_line * ex) / eWide;
                ipptr = ilptr + ix;
                *epptr = *ipptr;
            }
        }
        free((char*)data[i]);
        data[i] = buf;
    }
    xim->data = data[0];
    xim->grn_data = data[1];
    xim->blu_data = data[2];
    xim->other = data[3];
    xim->width = xim->bytes_per_line = eWide;
    xim->height = eHigh;
    xim->datasize = (unsigned)(eWide * eHigh);
    return(1);
}


XimCrop(xim, Xorig, Yorig, Xend, Yend)
XimImage *xim;
int Xorig, Yorig, Xend, Yend;
{
    int i, j, width, height, nchannels;
    unsigned size;
    byte *data[4], *buf, *offset, *to;

    if (debug_flag)
        fprintf(stderr, "XimCrop: x1 %d  y1 %d   x2 %d  y2 %d\n",
            Xorig, Yorig, Xend, Yend);
    width = (Xend - Xorig) + 1;
    height = (Yend - Yorig) + 1;
    if ((width < 3) || (height < 3)) {
        XimWarn("XimCrop: Crop dimensions must be greater than 2.");
        return(0);
    }
    if ((width > xim->width)  || (height  > xim->height)) {
        XimWarn("XimCrop: Crop dimensions are greater than image.");
        return(0);
    }
    data[0] = xim->data;
    data[1] = xim->grn_data;
    data[2] = xim->blu_data;
    data[3] = xim->other;
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    size = (unsigned)(width * height);

    for (i=0; i < nchannels; i++) {
        if (data[i] == (byte *)0)
            continue;
        if ((buf = (byte *)malloc(size)) == (byte *)0) {
            XimWarn("XimCrop: can't malloc buffer");
            return(0);
        }
        offset = data[i] + (xim->bytes_per_line * Yorig) + Xorig;
        to = buf;
        for (j=0; j < height; j++) {
            bcopy((char *)offset, (char *)to, width);
            offset += xim->bytes_per_line;
            to += width;
        }
        free((char*)data[i]);
        data[i] = buf;
    }
    xim->data = data[0];
    xim->grn_data = data[1];
    xim->blu_data = data[2];
    xim->other = data[3];
    xim->width = xim->bytes_per_line = width;
    xim->height = height;
    xim->datasize = size;
    return(1);
}

/*** end ximedit.c ***/
