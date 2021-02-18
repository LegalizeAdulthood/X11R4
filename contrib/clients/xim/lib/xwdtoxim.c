/* xtoxim.c
*  Author: Philip Thompson (phils@athena.mit.edu)
*   - Converts XWDFiles to the "xim" format.
*   - Also translates parallax video to 24 bit xim format.
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
#ifndef lint
static char  xtoxim_rcsid[] =
    "$Header: /mit/phils/X/Lib/RCS/xwdtoxim.c,v 1.1 89/11/09 17:43:49 phils Exp Locker: phils $";
#endif

/* Copyright 1987 Massachusetts Institute of Technology */

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XWDFile.h>
#include <sys/time.h>
#include <pwd.h>
#include <sys/types.h>
#include "XimHeader.h"

struct conv_table {
    long ry, gy, by;
};

char  *progName;
char *calloc(), *malloc(), *realloc(), *strncpy();
extern int debug_flag;


XimReadXwdFile(fp, xim, plx_flag)
FILE *fp;
XimImage *xim;
int plx_flag;
{
    register int  i, j = 0;
    unsigned bufsize;
    int name_size, bytes_line;
    int grey_flag = False;
    u_long  swaptest = 1;
    XWDFileHeader header;
    char  *login_ptr = NULL, *ctime(), *getlogin();
    struct passwd  *getpwnam(), *getpwuid(), *pwd = NULL;
    struct timeval  time_val;
    struct timezone  time_zone;
    XColor  *colors = (XColor *)0;

    /* Read in header information. */
    if (fread((char *)&header, sizeof(header), 1, fp) != 1) {
        XimWarn("XwdToXim: unable to read Xwd file header.");
        return(0);
    }
    if (*(char *)&swaptest)
        swaplong((char *)&header, sizeof(header));

    /* check to see if the dump file is in the proper format.  */
    if (header.file_version != XWD_FILE_VERSION) {
        XimWarn("XwdToXim: XWD file format version mismatch");
        return(0);
    }
    if (header.header_size < sizeof(header)) {
        XimWarn("XwdToXim: XWD header size is too small");
        return(0);
    }
/***
    if ((header.pixmap_format != ZPixmap)) {
        XimWarn("XwdToXim: can only do ZPixmap format");
        return(0);
    }
***/
    if (header.pixmap_depth > 8) {
        XimWarn("Image is too deep");
        return(0);
    }

    /* Calloc and read window name.  */
    name_size = ((int)header.header_size - sizeof(header));
    if (!(xim->program = calloc((unsigned)name_size+1, sizeof(char)))) {
        XimWarn("XwdToXim: Can't calloc window name storage.");
        return(0);
    }
    if (fread(xim->program, 1, name_size, fp) != name_size) {
        XimWarn("XwdToXim: Unable to read window name from file.");
        return(0);
    }

    xim->width = (int)header.pixmap_width;
    xim->nchannels = 1;
    xim->height = (int)header.pixmap_height;
    xim->bits_channel = header.bits_per_pixel / xim->nchannels;
    bytes_line = (int)header.bytes_per_line;
    if ((xim->bits_channel) == 1 && (xim->nchannels == 1))
        xim->bytes_per_line = ((xim->width + 7) / 8);
    else
        xim->bytes_per_line = xim->width;

    if (xim->ncolors = (int)header.ncolors) {
        colors = (XColor *)calloc((unsigned)xim->ncolors,
                sizeof(XColor));
        xim->colors = (Color *)calloc((unsigned)xim->ncolors,
                sizeof(Color));
        if (colors == (XColor *)0 || xim->colors == (Color *)0) {
            XimWarn("XwdToXim: Can't calloc() colors.");
            return(0);
        }
        if (fread((char *)colors, sizeof(XColor), xim->ncolors, fp)
                != xim->ncolors) {
            XimWarn("XwdToXim: Unable to read color map from file");
            return(0);
        }
        if (debug_flag)
            fprintf(stderr,"Read in %d colors\n", xim->ncolors);
        for (i=0; i < xim->ncolors; i++) {
            if (*(char *)&swaptest) {
                swaplong((char *)&colors[i].pixel, sizeof(long));
                swapshort((char *)&colors[i].red, 3*sizeof(short));
            }
            j = (int)colors[i].pixel;
            xim->colors[j].red = (byte)(colors[i].red / 256);
            xim->colors[j].grn = (byte)(colors[i].green / 256);
            xim->colors[j].blu = (byte)(colors[i].blue / 256);
            if (debug_flag)
                fprintf(stderr,"pixel[%u]: r %5u   g %5u   b %5u\n",
                    colors[i].pixel, colors[i].red, colors[i].green,
                    colors[i].blue);
        }
    }
    
    /* Read in the pixmap buffer.  */
    bufsize = Image_Size(&header);
    if ((xim->data = (byte *)calloc(bufsize, 1)) == NULL) {
        XimWarn("XwdToXim: can't calloc data buffer\n");
        return(0);
    }
    if (xim->width != bytes_line) {
        byte *marker = xim->data;
        for (i=0; i < xim->height; i++) {
            if (fread((char*)marker, 1, bytes_line, fp) != bytes_line) {
                XimWarn("XwdToXim: unable to read pixmap from file.");
                return(0);
            }
            marker += xim->bytes_per_line;
        }
    } else 
        if (fread((char *)xim->data, 1, (int)bufsize, fp) != bufsize) {
            XimWarn("XwdToXim: unable to read pixmap from dump file.");
            return(0);
        }

    if (plx_flag && xim->bits_channel == 8 && xim->nchannels == 1) {
#ifdef notdef
        xim->data += 44 * xim->width; /* hack for removing to top box */
        xim->height -= 44;          /* assuming height of 44 ONLY on */
#endif notdef
        xim->datasize = xim->width * xim->height;  /* Athena's xvideo */
        if (!grey_flag) {
            xim->grn_data = (byte *)malloc(xim->datasize);
            xim->blu_data = (byte *)malloc(xim->datasize);
            if (xim->grn_data == NULL || xim->blu_data == NULL) {
                XimWarn("XwdToXim: can't malloc() color buffers\n");
                return(0);
            }
        }
        yuv_decode(xim, grey_flag);
        xim->bits_channel = 8;
        if (grey_flag) {
            xim->nchannels = 1;
            xim->ncolors = 256;
            if (!(xim->colors = (Color *)realloc((char *)xim->colors,
                    xim->ncolors * sizeof(Color))))
                return(0);
            for (i=0; i < xim->ncolors; i++) {
                xim->colors[i].red = xim->colors[i].grn =
                xim->colors[i].blu = (byte)i;
            }
        } else {
            xim->nchannels = 3;
            if (xim->colors) {
                free((char *)xim->colors);
                xim->colors = (Color *)0;
            }
            xim->ncolors = 0;
        }
    } else if (xim->bits_channel == 1  && xim->nchannels  == 1) {
        xim->nchannels = 1;
        xim->datasize = xim->bytes_per_line * xim->height;
        if (!(xim->data = (byte *)realloc((char *)xim->data,
                xim->datasize))) {
            XimWarn("XwdToXim: can't realloc data buffer\n");
            return(0);
        }
        if (header.bitmap_bit_order == MSBFirst ||
                header.byte_order == MSBFirst)
            Flip_bytes(xim);
    } else {
        xim->datasize = xim->bytes_per_line * xim->height;
        if (!(xim->data = (byte *)realloc((char *)xim->data,
                xim->datasize))) {
            XimWarn("XwdToXim: can't realloc data buffer\n");
            return(0);
        }
        free((char *)colors);
        RefineColors(xim);
    }
    (void)gettimeofday(&time_val, &time_zone);
    xim->date = ctime(&time_val.tv_sec);
    xim->date[strlen(xim->date)-1] = '\0';
    if (!(login_ptr = getlogin()))
        pwd = getpwuid((int)getuid());
    else
        pwd = getpwnam(login_ptr);
    if (!(xim->author = calloc((unsigned)strlen(pwd->pw_gecos)+1, 1))) {
        XimWarn("XwdToXim: Can't calloc author name storage.");
        return(0);
    }
    bcopy(pwd->pw_gecos, xim->author, strlen(pwd->pw_gecos));
    xim->npics = 1;
    return(1);
}


yuv_decode(xim, grey_flag)
XimImage *xim;
int grey_flag;
{
    register int  i, j, k;
    register byte *red_buf, *grn_buf, *blu_buf;
    register int  red, green, blue, gray, idx;
    struct conv_table  *p, ctable[256];
    byte minmax_table[768], *normalize;

    setup_tables(ctable, minmax_table);
    normalize = &minmax_table[256];
    red_buf = xim->data;
    grn_buf = xim->grn_data;
    blu_buf = xim->blu_data;

    for (j=0, i=0; i < xim->datasize; i+=4) {
        idx = ((red_buf[i] & 0xc0)>>6) | ((red_buf[i+1] & 0xc0)>>4) |
                ((red_buf[i+2] & 0xc0)>>2) | ((red_buf[i+3] & 0xc0));
        p = &ctable[idx];
        for (k=0; k < 4; k++, j++) {
            gray = (red_buf[j] & 0x3e) << 2;
            red = normalize[gray + p->ry];
            green = normalize[gray + p->gy];
            blue = normalize[gray + p->by];
            if (grey_flag)
                red_buf[j] = (byte)((int)((float)red*0.299 +
                    (float)green*0.587 + (float)blue*0.114 + 0.5));
            else {
                red_buf[j] = (byte)red;
                grn_buf[j] = (byte)green;
                blu_buf[j] = (byte)blue;
            }
        }
    }
}
            

/*  Copyright (c) 1987 by
*   PARALLAX GRAPHICS, INCORPORATED, Santa Clara, California.
*   All rights reserved
*   Author:   Martin J. Levy.
*             Parallax Graphics
*   Date:     October 1987
*   Modified: P. Thompson, Nov. 1988.  Simplified code.
*/
#define max(a,b)    ((a) < (b) ? (b) : (a))
#define min(a,b)    ((a) > (b) ? (b) : (a))

setup_tables(ctbl, minmax_table)
struct conv_table  ctbl[256];
byte  minmax_table[768];
{
    register int i;
    register byte *minmax = &minmax_table[256];

    for (i=0; i < 256; i++) {
        ctbl[i].ry = (((((i & 0x03)<<2) | ((i & 0x0c)>>2)) & 0xf) - 8)
            << 13;
        ctbl[i].ry *= 179;

        ctbl[i].by = (((((i & 0x30)>>2) | ((i & 0xc0)>>6)) & 0xf) - 8)
            << 13;
        ctbl[i].by *= 228;

        ctbl[i].gy = -((ctbl[i].ry>>8) * 131) - ((ctbl[i].by>>8) * 49);

        ctbl[i].ry >>= 16;
        ctbl[i].gy >>= 16;
        ctbl[i].by >>= 16;

        if (debug_flag)
            fprintf(stderr,"pixel[%u]: r %4ld   g %4ld   b %4ld\n", i,
                ctbl[i].ry, ctbl[i].gy, ctbl[i].by);
    }
    /* pre computed min and max of all pixel values, normalizes 0-255 */
    for (i= -256; i < 512; i++)
        minmax[i] = min(255, max(0, i));
}

int Image_Size(image)
XWDFileHeader *image;
{
    if (image->pixmap_format != ZPixmap)
        return(image->bytes_per_line * image->pixmap_height *
            image->pixmap_depth);
    return(image->bytes_per_line * image->pixmap_height);
}


XimWriteXwdFile(out_fp, xim)
FILE *out_fp;
XimImage *xim;
{
    unsigned long swaptest = 1;
    XColor *colors = NULL;
    int win_name_size, header_size;
    int i;
    XWDFileHeader header;

    if (xim->nchannels == 3 && xim->bits_channel == 8) {
        XimWarn("XimWriteXwdFile: can't write 24 bit xwd files");
        return(0);
    }
    if (xim->alpha_flag && xim->other)
        XimWarn("XimWriteXwdFile: alpha channel not saved");
    if (!xim->program || !xim->program[0])
        xim->program = "ximtoxwd";
    win_name_size = strlen(xim->program) + sizeof(char);

    /* load the colors into the table */
    if (xim->ncolors) {
        if (!(colors = (XColor *)calloc((unsigned)xim->ncolors,
                sizeof(Color))))
            return(0);
        for (i=0; i < xim->ncolors; i++) {
            colors[i].pixel = (u_long)i;
            colors[i].red = (u_short)(xim->colors[i].red * 257);
            colors[i].green = (u_short)(xim->colors[i].grn * 257);
            colors[i].blue = (u_short)(xim->colors[i].blu * 257);
        }
    }

    /* Calculate header size. */
    header_size = sizeof(header) + win_name_size;
    bzero((char *)&header, sizeof(header));

    /* Write out header information. */
    header.header_size = (xwdval)header_size;
    header.file_version = (xwdval)XWD_FILE_VERSION;
    header.pixmap_width = (xwdval)xim->width;
    header.pixmap_height = (xwdval)xim->height;
    header.xoffset = (xwdval)0;
    header.byte_order = (xwdval)LSBFirst;
    header.bitmap_unit = (xwdval)8;
    header.bitmap_bit_order = (xwdval)LSBFirst;
    header.bitmap_pad = (xwdval)0;
    header.bits_per_pixel = (xwdval)xim->bits_channel * xim->nchannels;
    header.bytes_per_line = (xwdval)xim->bytes_per_line;
    if (xim->nchannels == 1 && xim->bits_channel == 1) {
        header.pixmap_format = (xwdval)XYBitmap;
        header.visual_class = (xwdval)StaticGray;
        header.pixmap_depth = (xwdval)1;
    } else {
        header.pixmap_format = (xwdval)ZPixmap;
        header.visual_class = (xwdval)PseudoColor;
        header.pixmap_depth = (xwdval)8;
    }
    header.red_mask = header.green_mask = header.blue_mask = (xwdval)0;
    header.bits_per_rgb = (xwdval)xim->bits_channel;
    header.colormap_entries = header.ncolors = xim->ncolors;
    header.window_width = (xwdval)xim->width;
    header.window_height = (xwdval)xim->height;
    header.window_x = header.window_y = 0;
    header.window_bdrwidth = (xwdval)0;

    if (*(char *)&swaptest) {
        swaplong((char *)&header, sizeof(header));
        for (i = 0; i < xim->ncolors; i++) {
            swaplong((char *)&colors[i].pixel, sizeof(long));
            swapshort((char *)&colors[i].red, 3 * sizeof(short));
        }
    }
    (void)fwrite((char *)&header, sizeof(header), 1, out_fp);
    (void)fwrite(xim->program, win_name_size, 1, out_fp);

    /* Write out the color maps, if any */
    if (xim->ncolors && colors) {
        (void)fwrite((char *)colors, sizeof(XColor), xim->ncolors,
                out_fp);
        free((char*)colors);
    }

    /* Write out the buffer. */
    (void)fwrite(xim->data, (int)xim->datasize, 1, out_fp);

    (void)fflush(out_fp);
    return(1);
}

/*** End of xtoxim.c ***/
