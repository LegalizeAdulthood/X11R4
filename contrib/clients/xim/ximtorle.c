/* ximtorle.c
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
static char  ximtorle_rcsid[] = 
    "$Header: /mit/phils/X/RCS/ximtorle.c,v 1.6 89/11/09 17:29:39 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include <strings.h>
#include "svfb_global.h"
#include "XimHeader.h"
    
#define  Bcopy(a,b,c)   bcopy((char*)a, (char*)b, (int)c)

char  *progName, *calloc();

main(argc, argv)
int argc;
char **argv;
{
    register i;
    FILE *in_fp = stdin, *out_fp = stdout;
    int pclose(), fclose(), (*closefunc)() = fclose;
    XimImage *xim;

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
            if (in_fp == (FILE *)0)
                XimError("Can't open input file: %s ", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_fp = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s\n", argv[i]);
            continue;
        }
        XimError("Usage: %s [-in file][-out file]", progName);
    }
    xim = (XimImage *)calloc(1, sizeof(XimImage));
    if (!ReadXim(in_fp, xim))
        XimError("%s can't read header", progName);
    (void)(*closefunc)(in_fp);

    XimWriteRleFile(out_fp, xim);

    (void)fclose(out_fp);
    exit(0);
}


XimWriteRleFile(out_fp, xim)
FILE *out_fp;
XimImage *xim;
{
    register i;
    rle_pixel **rows;
    char comment[80], *AllocAndCopy();

    if (xim->nchannels == 1  && xim->bits_channel == 1)
        if (!BitmapToPixmap(xim)) {
            XimWarn("XimWriteRleFile: can't convert to pixmap");
            return(0);
        }
    /* Intialize and setup rle header */
    sv_globals.sv_xmin = 0;
    sv_globals.sv_ymin = 0;
    sv_globals.sv_ymax = xim->height - 1;
    sv_globals.sv_xmax = xim->width - 1; 
    sv_globals.svfb_fd = out_fp;
    sv_globals.sv_background = 0;
    sv_globals.sv_cmaplen = 0;
    sv_globals.sv_cmap = (rle_map *)0;
    if (xim->nchannels == 1 && xim->bits_channel == 8) {
        sv_globals.sv_ncolors = 1;
        sv_globals.sv_ncmap = 3;
        if (xim->ncolors && xim->colors) {
            rle_map *redp, *grnp, *blup;

            for (i=1; i < xim->ncolors; i <<= 1)
                sv_globals.sv_cmaplen++;
            i = 1 << sv_globals.sv_cmaplen;
            sv_globals.sv_cmap = (rle_map *)calloc((unsigned)(3 * i),
                    sizeof(rle_map));
            redp = sv_globals.sv_cmap;
            grnp = sv_globals.sv_cmap + i;
            blup = sv_globals.sv_cmap + (2 * i);
            for (i=0; i < xim->ncolors; i++) {
                redp[i] = (rle_map)(xim->colors[i].red * 257);
                grnp[i] = (rle_map)(xim->colors[i].grn * 257);
                blup[i] = (rle_map)(xim->colors[i].blu * 257);
            }
        }
    } else if (xim->nchannels == 3 && xim->bits_channel == 8) {
        sv_globals.sv_ncmap = 0;
        sv_globals.sv_ncolors = 3;
    }
    sv_globals.sv_comments = (char **)calloc(4, sizeof(char *));

    for (i=0; i < sv_globals.sv_ncolors; i++)
        SV_SET_BIT(sv_globals, i);
    if (xim->alpha_flag) {
        sv_globals.sv_alpha = 1;
        SV_SET_BIT(sv_globals, SV_ALPHA);
    }
    if (rle_row_alloc(&sv_globals, &rows)) {
        XimWarn("XimWriteRleFile: rle_row_alloc failed\n");
        return(0);
    }

    i = 0;
    /* Move the data */
    if (xim->author) {
        (void)sprintf(comment, "Author=%s", xim->author);
        sv_globals.sv_comments[i++] = AllocAndCopy(comment,
                strlen(comment));
    }
    if (xim->program) {
        (void)sprintf(comment, "Program=%s", xim->program);
        sv_globals.sv_comments[i++] = AllocAndCopy(comment,
                strlen(comment));
    }
    if (xim->date) {
        (void)sprintf(comment, "Date=%s", xim->date);
        sv_globals.sv_comments[i++] = AllocAndCopy(comment,
                strlen(comment));
    }
    sv_setup(RUN_DISPATCH, &sv_globals);
    i = xim->datasize - xim->width;
    for ( ; i >= 0; i -= xim->width) {
            Bcopy(&xim->data[i], rows[1], xim->bytes_per_line);
        if (xim->nchannels == 3) {
            Bcopy(&xim->grn_data[i],rows[2],xim->bytes_per_line);
            Bcopy(&xim->blu_data[i], rows[3],xim->bytes_per_line);
        }
        if (xim->alpha_flag)
            Bcopy(&xim->other[i], rows[0], xim->bytes_per_line);
        sv_putrow(&rows[1], xim->bytes_per_line, &sv_globals);
    }
    sv_puteof(&sv_globals);
    if (sv_globals.sv_cmap)
        free((char*)sv_globals.sv_cmap);
    rle_row_free(&sv_globals, rows);
    return(1);
}

/*** end ximrle.c ***/
