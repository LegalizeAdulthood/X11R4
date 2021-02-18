/***********************************************************************
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
*   I provide this software freely as a public service.  It is NOT a
*   commercial product, and therefore is not subject to an an implied
*   warranty of merchantability or fitness for a particular purpose.  I
*   provide it as is, without warranty.
***********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char fbmtoxim_rcsid[] =
    "$Header: /mit/phils/X/RCS/xtoxim.c,v 1.17 89/11/09 17:29:54 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"
#include "fbm.h"
                          
char  *progName;
int debug_flag;

main(argc, argv)
int argc;
char **argv;
{
    int i;
    char *in_name = NULL, *calloc();
    FILE *in_fp = stdin, *out_fp = stdout, *fopen();
    XimImage *xim;
    FBM  fbm;
    byte *red_p, *grn_p, *blu_p;
    byte *xbp, *fbp, *fep;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            in_name = argv[++i];
            continue;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_fp = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s\n", argv[i]);
            continue;
        }
        XimError("Usage: %s [-in file][-out file]", progName);
    }

    read_bitmap(&fbm, in_name);
    (void)fclose(in_fp);

    if (!(xim = (XimImage *)calloc(1, sizeof(XimImage))))
        XimError("can't malloc header space.");
    xim->height = fbm.hdr.rows;
    xim->bits_channel = fbm.hdr.bits;
    xim->nchannels = fbm.hdr.planes;
    if (fbm.hdr.bits == 1)
        xim->width = xim->bytes_per_line = fbm.hdr.rowlen;
    else
        xim->width = xim->bytes_per_line = fbm.hdr.cols;
    xim->datasize = (unsigned)(xim->height * xim->bytes_per_line);
    xim->npics = 1;
    xim->ncolors = fbm.hdr.clrlen / 3;
    xim->program = calloc(17, 1);
    xim->author = calloc(49, 1);
    (void)strncpy(xim->program, fbm.hdr.title, 16);
    (void)strncpy(xim->author, fbm.hdr.credits, 48);
    if (xim->ncolors)
        xim->colors = (Color *)calloc((unsigned)xim->ncolors,
                sizeof(Color));
    if (!(xim->data = (byte *)malloc(xim->datasize)))
        XimError("can't malloc data space.");
    if (xim->nchannels == 3) {
        if (!(xim->grn_data = (byte *)malloc(xim->datasize)))
            XimError("can't malloc grn_data space.");
        if (!(xim->blu_data = (byte *)malloc(xim->datasize)))
            XimError("can't malloc blu_data space.");
    }
    fbp = fbm.bm;
    fep = fbm.bm + fbm.hdr.plnlen;
    xbp = xim->data;
    while (fbp < fep) {
        bcopy((char *)fbp, (char *)xbp, xim->bytes_per_line);
        fbp += fbm.hdr.rowlen;
        xbp += xim->bytes_per_line;
    }
    if (xim->nchannels == 3) {
        fbp = fbm.bm + fbm.hdr.plnlen;
        fep = fbm.bm + (2 * fbm.hdr.plnlen);
        xbp = xim->grn_data;
        while (fbp < fep) {
            bcopy((char *)fbp, (char *)xbp, xim->bytes_per_line);
            fbp += fbm.hdr.rowlen;
            xbp += xim->bytes_per_line;
        }
        fbp = fbm.bm + (2 * fbm.hdr.plnlen);
        fep = fbm.bm + (3 * fbm.hdr.plnlen);
        xbp = xim->blu_data;
        while (fbp < fep) {
            bcopy((char *)fbp, (char *)xbp, xim->bytes_per_line);
            fbp += fbm.hdr.rowlen;
            xbp += xim->bytes_per_line;
        }
    }
    if (xim->nchannels == 1 && xim->bits_channel == 1)
        PixmapToBitmap(xim);
    else if (xim->nchannels == 1 && xim->bits_channel != 1) {
        red_p = &fbm.cm[0];
        grn_p = &fbm.cm[xim->ncolors];
        blu_p = &fbm.cm[2 * xim->ncolors];
        for (i=0; i < xim->ncolors; i++) {
            xim->colors[i].red = red_p[i];
            xim->colors[i].grn = grn_p[i];
            xim->colors[i].blu = blu_p[i];
        }
        RefineColors(xim);
    }
    WriteXim(out_fp, xim);
    (void)fclose(out_fp);
    exit(0);
}

/*** end fbmtoxim.c ***/
