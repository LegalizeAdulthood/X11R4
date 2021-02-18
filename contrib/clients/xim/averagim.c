/* averagim.c
* This program averages several xim files together. Primarily for
* reducing digitization noise.
*
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
*/
#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"

char  *progName, *calloc(), *malloc();
int  debug_flag = 0;

main(argc, argv)
int argc;
char **argv;
{
    register  i, j, k, l;
    int  fpcount = 0, total;
    FILE  *in_fp[10], *out_fp = stdout;
    int pclose(), fclose(), (*closefunc)() = fclose;
    XimImage xims[10], ximout;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            char *ptr = rindex(argv[++i], '.');
            if (ptr && strcmp(ptr, ".Z") == 0) {
                char popen_cmd[80];
                (void)strcpy(popen_cmd, "zcat ");
                (void)strcat(popen_cmd, argv[i]);
                in_fp[fpcount] = popen(popen_cmd, "r");
                closefunc = pclose;
            } else
                in_fp[fpcount] = fopen(argv[i], "r");
            if (in_fp[fpcount++] == NULL)
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

    ReadXimHeader(in_fp[0], &xims[0]);
    bcopy((char *)&xims[0], (char *)&ximout, sizeof(XimImage));
    ximout.author = NULL;
    ximout.program = NULL;
    ximout.date = NULL;
    ximout.data = (byte *)malloc(ximout.datasize * 3);
    ximout.grn_data = ximout.data + ximout.datasize;
    ximout.blu_data = ximout.data + (2 * ximout.datasize);
    if (ximout.data == NULL || ximout.blu_data == NULL)
        XimError("can't malloc() ximout data space");

    for (i=1; i < fpcount; i++) {
        ReadXimHeader(in_fp[i], &xims[i]);
        if (xims[i].nchannels < 3)
            XimError("File must be 24 bit format");
        if (ximout.width != xims[i].width)
            XimError("Files must have the same width");
        if (ximout.height != xims[i].height)
            XimError("Files must have the same height");
    }
    fprintf(stderr, "fpcount %d\n", fpcount);

    for (i=0; i < fpcount; i++) {
        if ((xims[i].data = (byte *)malloc(xims[i].datasize)) == NULL)
            XimError("can't malloc data");
        ReadImageChannel(in_fp[i], xims[i].data, &xims[i].datasize, 0);
    }
    for (i=0, k=0; i < ximout.height; i++) {
        for (j=0; j < ximout.width; j++, k++) {
            total = 0;
            for (l=0; l < fpcount; l++)
                total += xims[l].data[k];
            ximout.data[k] = (byte)rnd(total / (float)fpcount);
        }
    }

    for (i=0; i < fpcount; i++)
       ReadImageChannel(in_fp[i], xims[i].data, &xims[i].datasize, 0);
    for (i=0, k=0; i < ximout.height; i++) {
        for (j=0; j < ximout.width; j++, k++) {
            total = 0;
            for (l=0; l < fpcount; l++)
                total += xims[l].data[k];
            ximout.grn_data[k] = (byte)rnd(total / (float)fpcount);
        }
    }

    for (i=0; i < fpcount; i++)
       ReadImageChannel(in_fp[i], xims[i].data, &xims[i].datasize, 0);
    for (i=0, k=0; i < ximout.height; i++) {
        for (j=0; j < ximout.width; j++, k++) {
            total = 0;
            for (l=0; l < fpcount; l++)
                total += xims[l].data[k];
            ximout.blu_data[k] = (byte)rnd(total / (float)fpcount);
        }
    }
    for (i=0; i < fpcount; i++)
        (void)(*closefunc)(in_fp[i]);
    WriteXim(out_fp, &ximout);
    (void)fclose(out_fp);
    exit(0);
}
