/**********************************************************************/
/* file:    quantizim.c                                               */
/* purpose: To quantize a 2**24 bit image down to 2**8 (256 or less)  */
/*          with an ImageHeader                                       */
/* author:  Philip Thompson, 1988                                     */
/* Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#ifndef lint
static char  quantizim_rcsid[] =
    "$Header: /mit/phils/X/RCS/quantizim.c,v 1.14 89/11/09 17:27:23 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"

#define OCTREE  1
#define DITHER  2
#define MEDIAN  3
                          
char *progName;
char *malloc(), *calloc();
extern debug_flag;

main(argc, argv)
int argc;
char **argv;
{
    register  i;
    XimImage xim;
    int ncolors = 256, enhance = 0, type = OCTREE;
    FILE *out_fp = stdout, *in_fp = stdin, *popen(), *fopen();
    int  pclose(), fclose(), (*closefunc)() = fclose;
    Color  *colors;
    extern int  COLTNODES, ditherFactor;
    int map_flag = 0, dither_flag = 1;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        char *ptr = NULL;
        if (strncmp(argv[i], "-i", 2) == 0) {
            ptr = rindex(argv[++i], '.');
            if (ptr && strcmp(ptr, ".Z") == 0) {   /* compressed file */
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
        if (strncmp(argv[i], "-co", 3) == 0) {
            ncolors = atoi(argv[++i]);
            continue;
        }
        if (strncmp(argv[i], "-di", 3) == 0) {
            type = DITHER;
            if ((ptr=rindex(argv[i],'=')) != NULL)
                ditherFactor = atoi(ptr+1);
            else
                ditherFactor = 6;
            dither_flag = !dither_flag; /* toggle median dithering */
            continue;
        }
        if (strncmp(argv[i], "-me", 3) == 0) {
            type = MEDIAN;
            continue;
        }
        if (strncmp(argv[i], "-cm", 3) == 0) {
            map_flag = 1;
            type = MEDIAN;
            continue;
        }
        if (strncmp(argv[i], "-no", 3) == 0) {
            COLTNODES = atoi(argv[++i]);
            continue;
        }
        if (strncmp(argv[i], "-en", 3) == 0) {
            if ((ptr=rindex(argv[i],'=')) != NULL)
                enhance = atoi(ptr+1);
            else
                enhance = 9;
            continue;
        }
        if (strncmp(argv[i], "-de", 3) == 0) {
            debug_flag = 1;
            continue;
        }
        Usage();
    }

    ReadXim(in_fp, &xim);
    (void)(*closefunc)(in_fp);
    if (xim.nchannels != 3 || xim.bits_channel != 8)
        XimError("Iimage must have 24 bits per pixel (3 channels).");

    if (enhance)
        Edge_enhance(&xim, enhance);
    if (!(colors = (Color *)calloc((unsigned)ncolors, sizeof(Color))))
            XimError("Can't calloc() colors.");

    if (type == OCTREE) {
        if (!OctreeQuantize(&xim, ncolors))
            XimError("error in OctreeQuantize");
    } else if (type == MEDIAN) {
        if (map_flag)
            ncolors = MakeDithermap(ditherFactor, colors, ncolors);
        ncolors = XimQuant(&xim, ncolors, colors, map_flag, dither_flag);
    } else {
        if (ditherFactor < 2 || ditherFactor > 6)
            XimError("Dither level must be between 2 and 6");
        xim.ncolors = MakeDithermap(ditherFactor, colors, ncolors);
        Rgb_dither(&xim);
        if (!(xim.colors = (Color *)calloc((unsigned)xim.ncolors, 
                    sizeof(Color))))
            XimError("Can't calloc() colors.");
        for (i=0; i < xim.ncolors; i++) {
            xim.colors[i].red = colors[i].red;
            xim.colors[i].grn = colors[i].grn;
            xim.colors[i].blu = colors[i].blu;
        }
        if (debug_flag)
            for (i=0; i < ncolors; i++)
                fprintf(stderr, "%-3d:  (%3u, %3u, %3u) [%u]\n", i,
                colors[i].red, colors[i].grn, colors[i].blu,
                colors[i].pixel);
    }
    xim.bits_channel = 8;
    xim.nchannels = 1;
    RefineColors(&xim);
    WriteXim(out_fp, &xim);
    (void)fclose(out_fp);
    fprintf(stderr,"ncolors %d  nchannels %d\n", xim.ncolors,
            xim.nchannels);
    exit(0);
}


Usage ()
{
    static char **cpp, *help_message[] = {
    "where options include:",
    "  -in file           file to read in - stdin default",
    "  -out file          file to write out - stdout default",
    "  -dither[=#]        use dithering algorithm [variable levels]",
    "  -octree            use octree algorithm - default",
    "  -median            use median cut algorithm",
    "  -colors #          maximum number of colors in quantizing",
    "  -no #              number of pixels per node (octree only)",
    "  -cmap              use same colormap as dither (median only)",
    "  -enhance[=1-9]     variable edge enhancement, (default = 9)",
    "  -debug             print various debuging information",
    "Options may be abbreviated to the shortest unambiguous prefix.",
    };

    fprintf (stderr, "usage:\n  %s [-options ...]\n\n", progName);
    for (cpp = help_message; *cpp; cpp++)
        fprintf (stderr, "%s\n", *cpp);
    exit(1);
}

/*** end quantixim.c ***/
