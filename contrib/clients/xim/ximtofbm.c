/* ximtofbm.c
*  Copyright (c) 1989, Massachusetts Institute of Technology
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
*   I provide this software freely as a public service.  It is NOT
*   public domain nor a commercial product, and therefore is not subject
*   to an an implied warranty of merchantability or fitness for a
*   particular purpose.  I provide it as is, without warranty.
*
*   This software is furnished  only on the basis that any party who
*   receives it indemnifies and holds harmless the parties who furnish
*   it against any claims, demands, or liabilities connected with using
*   it, furnishing it to others, or providing it to a third party.
*
*   Philip R. Thompson (phils@athena.mit.edu)
***********************************************************************/
#ifndef lint
static char  ximtofbm_rcsid[] = 
    "$Header: /mit/phils/X/RCS/ximtofbm.c,v 1.9 89/11/09 17:29:12 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"
#include "fbm.h"
    
char  *progName;
int  debug_flag;

main(argc, argv)
int argc;
char **argv;
{
    register i;
    FILE *in_fp = stdin, *out_fp = stdout;
    int pclose(), fclose(), (*closefunc)() = fclose;
    XimImage xim;
    FBM  fbm;
    byte *red_p, *grn_p, *blu_p;
    byte *xbp, *xep, *fbp;
    int gif_flag = 0, ncolors = 0;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            char *ptr = rindex(argv[++i], '.');
            if (ptr && strcmp(ptr, ".Z") == 0) {
                char popen_cmd[80];
                (void)strcpy(popen_cmd, "zcat ");
                (void)strcat(popen_cmd, argv[i]);
                in_fp = popen(popen_cmd, "r");
                closefunc = pclose;
            } else
                in_fp = fopen(argv[i], "r");
            if (in_fp == NULL)
                XimError("Can't open input file: %s ", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_fp = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s\n", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-d", 2) == 0) {
            debug_flag = 1;
            continue;
        }
        if (strncmp(argv[i], "-G", 2) == 0) {
            gif_flag = 1;
            continue;
        }
        XimError("Usage: %s [-in file] [-out file] [-Gif]", progName);
    }

    ReadXim(in_fp, &xim);
    (void)(*closefunc)(in_fp);

    fbm.hdr.cols = xim.width;
    fbm.hdr.rows = xim.height;
    fbm.hdr.planes = xim.nchannels;
    fbm.hdr.bits = xim.bits_channel;
    fbm.hdr.physbits = 8;
    fbm.hdr.rowlen = 16 * ((xim.bytes_per_line + 15) / 16);
    fbm.hdr.plnlen = fbm.hdr.rowlen * fbm.hdr.rows;
    if (xim.nchannels == 1 && xim.bits_channel == 8) {
        if (gif_flag) {
            fbm.hdr.aspect = 1.2;
            for (i=1; i < xim.ncolors; i <<= 1)
                ;
            ncolors = i;
        } else {
            fbm.hdr.aspect = 1.0;
            ncolors = xim.ncolors;
        }
        fbm.hdr.clrlen = ncolors * 3;
    }
    else
        fbm.hdr.clrlen = 0;
    strncpy(fbm.hdr.title, xim.program, FBM_MAX_TITLE);
    strncpy(fbm.hdr.credits, xim.author, FBM_MAX_TITLE);
    if (!alloc_fbm(&fbm))
        XimError("can't alloc fbm data");

    xbp = xim.data;
    xep = xbp + xim.datasize;
    fbp = fbm.bm;
    while (xbp < xep) {
        bcopy((char *)xbp, (char *)fbp, xim.bytes_per_line);
        xbp += xim.bytes_per_line;
        fbp += fbm.hdr.rowlen;
    }
    if (xim.nchannels == 3) {
        xbp = xim.grn_data;
        xep = xbp + xim.datasize;
        fbp = &fbm.bm[fbm.hdr.plnlen];
        while (xbp < xep) {
            bcopy((char *)xbp, (char *)fbp, xim.bytes_per_line);
            xbp += xim.bytes_per_line;
            fbp += fbm.hdr.rowlen;
        }
        xbp = xim.blu_data;
        xep = xbp + xim.datasize;
        fbp = &fbm.bm[2 * fbm.hdr.plnlen];
        while (xbp < xep) {
            bcopy((char *)xbp, (char *)fbp, xim.bytes_per_line);
            xbp += xim.bytes_per_line;
            fbp += fbm.hdr.rowlen;
        }
    }
    if (xim.nchannels == 1 && xim.bits_channel == 8) {
        red_p = fbm.cm;
        grn_p = &fbm.cm[ncolors];
        blu_p = &fbm.cm[ncolors << 1];
        for (i=0; i < xim.ncolors; i++) {
            red_p[i] = xim.colors[i].red;
            grn_p[i] = xim.colors[i].grn;
            blu_p[i] = xim.colors[i].blu;
        }
    }
    write_fbm(&fbm, out_fp);
    (void)fclose(out_fp);
    exit(0);
}

/*** end ximtofbm.c ***/
