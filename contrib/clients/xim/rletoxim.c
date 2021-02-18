/***********************************************************************
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
#if (!defined(lint) && !defined(SABER))
static char  rletoxim_rcsid[] =
    "$Header: /mit/phils/X/RCS/xtoxim.c,v 1.17 89/11/09 17:29:54 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"
#include "svfb_global.h"
                          
#define Bcopy(a,b,c)       bcopy((char*)a, (char*)b, (int)c)
char  *progName, *calloc();

main(argc, argv)
int argc;
char **argv;
{
    int i;
    FILE *in_fp = stdin, *out_fp = stdout, *fopen();
    XimImage *xim;
    double gamma = 1.0, atof();

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            if ((in_fp = fopen(argv[++i], "r")) == NULL)
                XimError("Can't read input file: %s.", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_fp = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-g", 2) == 0) {
            gamma = atof(argv[++i]);
            continue;
        }
        XimError("Usage: %s [-in <file>] [-out <file>] [-gamma <float>]",
            progName);
    }

    xim = (XimImage *)calloc(1, sizeof(XimImage));
    if (!XimReadRleFile(in_fp, xim, gamma))
        XimError("can't read rle file");

    (void)WriteXim(out_fp, xim);
    (void)fclose(out_fp);
    exit(0);
}


XimReadRleFile(in_fp, xim, gamma)
FILE *in_fp;
XimImage *xim;
double gamma;
{
    int i;
    rle_pixel  **rows, **cmap = (rle_pixel**)0;
    char  *string;

    sv_globals.svfb_fd = in_fp;
    if (rle_get_setup(&sv_globals) < 0) {
        XimWarn("XimReadRleFile: can't read rle setup information");
        return(0);
    }
    xim->height = sv_globals.sv_ymax - sv_globals.sv_ymin + 1;
    xim->width = xim->bytes_per_line =
            sv_globals.sv_xmax - sv_globals.sv_xmin + 1;
    xim->datasize = (unsigned)xim->bytes_per_line * xim->height;
    xim->nchannels = sv_globals.sv_ncolors;
    xim->bits_channel = 8;
    xim->ncolors = 1 << sv_globals.sv_cmaplen;;
    xim->npics = 1;
    if (sv_globals.sv_alpha && SV_SET_BIT(sv_globals, SV_ALPHA ))
        xim->alpha_flag = 1;

    if (rle_row_alloc(&sv_globals, &rows)) {
        XimWarn("XimReadRleFile: rle_row_alloc() failed.");
        return(0);
    }
    if (!(xim->data = (byte *)calloc(xim->datasize, 1))) {
        XimWarn("XimReadRleFile: can't calloc() color buffer.");
        return(0);
    }
    if (xim->nchannels == 3) {
        xim->grn_data = (byte *)calloc(xim->datasize, 1);
        xim->blu_data = (byte *)calloc(xim->datasize, 1);
        if (xim->grn_data == NULL || xim->grn_data == NULL) {
            XimWarn("XimReadRleFile: can't calloc() color buffers.");
            return(0);
        }
    }
    if (xim->alpha_flag)
        if (!(xim->other = (byte *)calloc(xim->datasize, 1))) {
            XimWarn("XimReadRleFile: can't calloc() alpha buffer.");
            return(0);
        }

    if (string = rle_getcom("Author", &sv_globals)) {
        xim->author = calloc((unsigned)strlen(string)+1, 1);
        (void)strcpy(xim->author, string);
    }
    if (string = rle_getcom("Program", &sv_globals)) {
        xim->program = calloc((unsigned)strlen(string)+1, 1);
        (void)strcpy(xim->program, string);
    }
    if (string = rle_getcom("Date", &sv_globals)) {
        xim->date = calloc((unsigned)strlen(string)+1, 1);
        (void)strcpy(xim->date, string);
    }
    if (sv_globals.sv_comments && sv_globals.sv_comments[0])
         fprintf(stderr,"%s\n", sv_globals.sv_comments[0]);

    if (xim->nchannels == 1 && xim->ncolors) {
        xim->colors = (Color *)calloc((unsigned)xim->ncolors,
                sizeof(Color));
        cmap = buildmap(&sv_globals, 3, gamma);
        for (i=0; i < xim->ncolors; i++) {
            xim->colors[i].red = (byte)cmap[0][i];
            xim->colors[i].grn = (byte)cmap[1][i];
            xim->colors[i].blu = (byte)cmap[2][i];
        }
    }
    i = xim->datasize - xim->bytes_per_line;
    for ( ; i >= 0; i -= xim->bytes_per_line) {
        rle_getrow(&sv_globals, rows);
        Bcopy(rows[SV_RED], &xim->data[i], xim->bytes_per_line);
        if (xim->nchannels == 3) {
            Bcopy(rows[SV_GREEN],&xim->grn_data[i],xim->bytes_per_line);
            Bcopy(rows[SV_BLUE], &xim->blu_data[i],xim->bytes_per_line);
        }
        if (xim->alpha_flag)
            Bcopy(rows[SV_ALPHA], &xim->other[i], xim->bytes_per_line);
    }
    (void)fclose(in_fp);
    if (xim->nchannels == 1 && xim->ncolors && xim->colors)
        RefineColors(xim);
/***
    if (xim->nchannels == 1 && cmap) {
        for (i=0; i < sv_globals.sv_ncmap; i++)
            free((char *)cmap[i]);
        free((char*)cmap);
    }
***/
    rle_row_free(&sv_globals, rows);
    return(1);
}

/*** end rle_to_xim.c ***/
