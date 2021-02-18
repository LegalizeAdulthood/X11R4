/***********************************************************************
*  File:   xlib.c
*  Author: Philip Thompson
*  $Date: 89/11/01 10:14:23 $
*  $Revision: 1.14 $
*  Purpose: General xim libray of utililities
*  Copyright (c) 1988  Philip R. Thompson
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
static char ximio_rcs_id[] =
    "$Header: /mit/phils/X/Lib/RCS/xim_io.c,v 1.14 89/11/01 10:14:23 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"

char  *malloc(), *calloc(), *realloc();
int  debug_flag;


ReadXim(in_fp, xim)
FILE *in_fp;
XimImage *xim;
{
    if (!ReadXimHeader(in_fp, xim)) {
        XimWarn("ReadXim: can't read header");
        return(0);
    }
    if (!ReadXimImage(in_fp, xim)) {
        XimWarn("ReadXim: can't read data");
        return(0);
    }
    return(1);
}


ReadXimHeader(in_fp, header)
FILE *in_fp;
XimImage  *header;
{
    int  i;
    XimAsciiHeader  a_head;

    if (debug_flag)
       fprintf(stderr, "ReadXimHeader: ...\n");
    bzero((char *)header, sizeof(XimImage));
    /* Read header and verify image file formats */
    if (fread((char *)&a_head, sizeof(ImageHeader), 1, in_fp) != 1) {
        XimWarn("ReadXimHeader: Unable to read file header");
        return(0);
    }
    if (atoi(a_head.header_size) != sizeof(ImageHeader)) {
        XimWarn("ReadXimHeader: Header size mismatch");
        return(0);
    }
    if (atoi(a_head.file_version) != IMAGE_VERSION) {
        XimWarn("ReadXimHeader: Incorrect Image_file Version.");
        return(0);
    }
    header->width = atoi(a_head.image_width);
    header->height = atoi(a_head.image_height);
    header->ncolors = atoi(a_head.num_colors);
    header->nchannels = atoi(a_head.num_channels);
    header->bytes_per_line = atoi(a_head.bytes_per_line);
/*    header->npics = atoi(a_head.num_pictures);
*/
    header->bits_channel = atoi(a_head.bits_per_channel);
    header->alpha_flag = atoi(a_head.alpha_channel);
    if (strlen(a_head.author)) {
        if (!(header->author = calloc((unsigned)strlen(a_head.author)+1,
                1))) {
            XimWarn("ReadXimHeader: can't calloc author string");
            return(0);
        }
    header->width = atoi(a_head.image_width);
        strncpy(header->author, a_head.author, strlen(a_head.author));
    }
    if (strlen(a_head.date)) {
        if (!(header->date =calloc((unsigned)strlen(a_head.date)+1,1))){
            XimWarn("ReadXimHeader: can't calloc date string");
            return(0);
        }
    header->width = atoi(a_head.image_width);
        strncpy(header->date, a_head.date, strlen(a_head.date));
    }
    if (strlen(a_head.program)) {
        if (!(header->program = calloc(
                    (unsigned)strlen(a_head.program) + 1, 1))) {
            XimWarn("ReadXimHeader: can't calloc program string");
            return(0);
        }
    header->width = atoi(a_head.image_width);
        strncpy(header->program, a_head.program,strlen(a_head.program));
    }
    /* Do double checking for bakwards compatibility */
    if (header->npics == 0)
        header->npics = 1;
    if (header->bits_channel == 0)
        header->bits_channel = 8;
    else if (header->bits_channel == 24) {
        header->nchannels = 3;
        header->bits_channel = 8;
    }
    if ((int)header->bytes_per_line == 0)
        if (header->bits_channel == 1 && header->nchannels == 1)
            header->bytes_per_line = (header->width + 7) / 8;
        else
            header->bytes_per_line = header->width;
    header->datasize =(unsigned)header->bytes_per_line * header->height;
    if (header->nchannels == 3 && header->bits_channel == 8)
        header->ncolors = 0;
    else if (header->nchannels == 1 && header->bits_channel == 8) {
        header->colors = (Color *)calloc((unsigned)header->ncolors,
                sizeof(Color));
        if (header->colors == NULL) {
            XimWarn("ReadXimHeader: can't calloc colors");
            return(0);
        }
        for (i=0; i < header->ncolors; i++) {
            header->colors[i].red = a_head.c_map[i][0];
            header->colors[i].grn = a_head.c_map[i][1];
            header->colors[i].blu = a_head.c_map[i][2];
        }
    }
    if (debug_flag) {
        if (header->author)
            (void)fprintf(stderr,"Author: %s\n", header->author);
        if (header->date)
            (void)fprintf(stderr,"Date: %s\n", header->date);
       fprintf(stderr, "ReadXimHeader: Done\n");
    }
    return(1);
}


ReadXimImage(in_fp, xim)
FILE *in_fp;
XimImage  *xim;
{
    if (debug_flag)
       fprintf(stderr, "ReadXimImage: ...");
    if (xim->data) {
        free((char *)xim->data);
        xim->data = (byte *)0;
    }
    if (xim->grn_data) {
        free((char *)xim->grn_data);
        xim->grn_data = (byte *)0;
    }
    if (xim->blu_data) {
        free((char *)xim->blu_data);
        xim->blu_data = (byte *)0;
    }
    if (xim->other) {
        free((char *)xim->other);
        xim->other = (byte *)0;
    }
    xim->npics = 0;
    if (!(xim->data = (byte *)calloc(xim->datasize, 1))) {
        XimWarn("ReadXimImage: can't malloc pixmap data");
        return(0);
    }
    if (!ReadImageChannel(in_fp, xim->data, &xim->datasize, 0)) {
        fprintf(stderr, "ReadXimImage: end of the images.");
        return(0);
    }
    if (xim->nchannels == 3) {
        xim->grn_data = (byte *)malloc(xim->datasize);
        xim->blu_data = (byte *)malloc(xim->datasize);
        if (xim->grn_data == NULL || xim->blu_data == NULL) {
            XimWarn("ReadXimImage: can't malloc rgb channel data");
            free((char *)xim->data);
            if (xim->grn_data)  free((char *)xim->grn_data);
            if (xim->blu_data)  free((char *)xim->blu_data);
            xim->data = xim->grn_data = xim->blu_data = (byte*)0;
            return(0);
        }
        if (!ReadImageChannel(in_fp, xim->grn_data, &xim->datasize, 0))
            return(0);
        if (!ReadImageChannel(in_fp, xim->blu_data, &xim->datasize, 0))
            return(0);
    }
    if (xim->alpha_flag) {
        if ((xim->other = (byte *)malloc(xim->datasize)) == NULL) {
            XimWarn("ReadXimImage: can't malloc alpha data");
            return(0);
        }
        if (!ReadImageChannel(in_fp, xim->other, &xim->datasize, 0))
            return(0);
    }
    xim->npics = 1;
    if (debug_flag)
       fprintf(stderr, "Done\n");
    return(1);
}


WriteXim(out_fp, xim)
FILE *out_fp;
XimImage *xim;
{
    if (!WriteXimHeader(out_fp, xim)) {
        XimWarn("WriteXim: error writing header");
        return(0);
    }
    if (!WriteXimImage(out_fp, xim)) {
        XimWarn("WriteXim: error writing data");
        return(0);
    }
    return(1);
}

WriteXimHeader(out_fp, xim)
FILE *out_fp;
XimImage *xim;
{
    int  i;
    XimAsciiHeader  a_head;
    char *sprintf();

    if (xim->npics == 0)
        xim->npics = 1;
    if (xim->bits_channel == 0)    /* backwards compatibility */
        xim->bits_channel = 8;
    if (xim->nchannels > 1 && xim->colors == NULL)
        xim->ncolors = 0;
    if (xim->bytes_per_line == 0)
        xim->bytes_per_line = (xim->bits_channel == 1 &&
            xim->nchannels == 1) ? (xim->width + 7)/8 : xim->width;
    bzero((char *)&a_head, sizeof(XimAsciiHeader));
    (void)sprintf(a_head.file_version, "%d", IMAGE_VERSION);
    (void)sprintf(a_head.header_size, "%d", sizeof(XimAsciiHeader));
    (void)sprintf(a_head.image_width, "%d", xim->width);
    (void)sprintf(a_head.image_height, "%d", xim->height);
    (void)sprintf(a_head.num_colors, "%d", xim->ncolors);
    (void)sprintf(a_head.num_channels, "%d", xim->nchannels);
    (void)sprintf(a_head.num_pictures, "%d", xim->npics);
    (void)sprintf(a_head.bits_per_channel, "%d", xim->bits_channel);
    (void)sprintf(a_head.bytes_per_line, "%d", xim->bytes_per_line);
    (void)sprintf(a_head.alpha_channel, "%d", xim->alpha_flag);
    if (xim->author)
       (void)strncpy(a_head.author,xim->author,sizeof(a_head.author)-1);
    if (xim->date)
        (void)strncpy(a_head.date, xim->date, sizeof(a_head.date)-1);
    if (xim->program)
        (void)strncpy(a_head.program, xim->program,
                sizeof(a_head.program)-1);
    if (xim->comments && xim->comments[0]) /* not yet supported */
        (void)strncpy(a_head.comment, xim->comments[0],
                sizeof(a_head.comment)-1);
    if (xim->nchannels == 1 && xim->bits_channel == 8 && xim->colors)
        for (i=0; i < xim->ncolors; i++) {
            a_head.c_map[i][0] = xim->colors[i].red;
            a_head.c_map[i][1] = xim->colors[i].grn;
            a_head.c_map[i][2] = xim->colors[i].blu;
        }
    if (!WriteBytes(out_fp, (byte *)&a_head, sizeof(XimAsciiHeader)))
        return(0);
    (void)fflush(out_fp);
    return(1);
}


WriteXimImage(out_fp, xim)
FILE  *out_fp;
XimImage  *xim;
{
    int i, j, nchannels;
    byte *data[4];

    /* This hasn't been tested on multi 24 bit images */
    nchannels = xim->nchannels + (xim->alpha_flag ? 1 : 0);
    for (i=0; i < xim->npics; i++) {
        data[0] = xim->data + (i * xim->datasize);
        data[1] = xim->grn_data + (i * xim->datasize);
        data[2] = xim->blu_data + (i * xim->datasize);
        data[3] = xim->other + (i * xim->datasize);
        for (j=0; j < nchannels; j++) {
            if (data[j] == (byte*)0)
                continue;
            if (!WriteBytes(out_fp, data[j], xim->datasize))
                return(0);
        }
    }
    (void)fflush(out_fp);
    return(1);
}


WriteBytes(fp, buf, nbytes)
FILE *fp;
byte *buf;
unsigned nbytes;
{
    if (fwrite((char *)buf, 1, nbytes, fp) != nbytes) {
        XimWarn("WriteBytes: error writing output.");
        return(0);
    }
    return(1);
}


ReadImageChannel(infp, buf, bufsize, encoded)
FILE *infp;
byte  *buf;
unsigned  *bufsize;
int  encoded;
{
    register int  i, runlen, nbytes;
    register unsigned  j;
    register byte *line;
    long  marker;

    if (debug_flag)
        fprintf(stderr,"ReadImageChannel: ... ");
    if (!encoded)
        j = fread((char *)buf, 1, (int)*bufsize, infp);
    else {
        if ((line=(byte *)malloc((unsigned)BUFSIZ)) == NULL) {
            XimWarn("ReadImageChannel: can't malloc() fread string.");
            return(0);
        }
        /* Unrunlength encode data */
        marker = ftell(infp);
        j = 0;
        while (((nbytes=fread((char *)line, 1, BUFSIZ, infp)) > 0) &&
            (j < *bufsize)) {
            for (i=0; (i < nbytes) && (j < *bufsize); i++) {
                runlen = (int)line[i]+1;
                i++;
                while (runlen--)
                    buf[j++] = line[i];
            }
            marker += i;
        }
        /* return to the begining of the next image's bufffer */
        if (fseek(infp, marker, 0) == -1) {
            XimWarn("Can't fseek to location in image buffer.");
            return(0);
        }
        free((char *)line);
    }
    if (j != *bufsize) {
        fprintf(stderr,"%cUnable to complete channel: %u / %u (%d%%)\n",
            7, j, *bufsize, (int)(j*100.0 / *bufsize));
        *bufsize = j;
    }
    return(1);
}

/*** end xim_io.c ***/
