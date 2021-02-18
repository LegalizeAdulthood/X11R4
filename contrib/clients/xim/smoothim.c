/*
*    Copyright (c) 1988  University of Southern California
*    USC Information Sciences Institute
*    4676 Admiralty Way
*    Marina del Rey, California 90292
*
*  Function:
*    This is a somewhat experimental program to enhance pixmaps
*    which have some degree of noise in their data.  Today's
*    algorithm does weighted averaging of pixels in a 5x5 cell
*    around each target pixel, but only for those pixels within
*    some given RGB range of the target.
*
*    thresh is the RGB distance threshold, defaulted to 50.
*    Neighboring pixels are averaged only if their RGB displacement
*    from the target pixel is less than thresh.
*
*  Module history:
*    3-19-88     Paul Raveling Created enhance from a copy of propix.
*    1-31-89     Philip Thompson - greatly stripped and modified.
*                Copyright (c) 1989, M.I.T. 
*                Computer Resource Laboratory
*                Dept. of Architecture and Planning
*/
/**********************************************************************/
/*  file: $Source: /mit/phils/X/Lib/RCS/fant.c,v $ */
/*  author: Philip R. Thompson, ($Author: phils $)                    */
/*  $Date: 89/10/17 01:38:31 $      $State: Exp $                     */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char  smoothim_rcsid[] =
    "$Header: /mit/phils/X/RCS/xtoxim.c,v 1.17 89/11/09 17:29:54 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <strings.h>
#include "XimHeader.h"

int  T = 50;              /* Distance threshold for pixel averaging */
char *progName, *malloc(), *calloc();
int  debug_flag = 0;

main (argc, argv)
int  argc;
char **argv;
{
    register  i;
    FILE  *out_fp = stdout, *in_fp = stdin, *popen(), *fopen();
    int pclose(), fclose(), (*closefunc)() = fclose;
    XimImage  xim;                 /* picture header */

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            char *ptr = rindex(argv[++i], '.');
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
        if (strncmp(argv[i], "-t", 2) == 0) {
            T = atoi(argv[++i]);
            continue;
        }
        fprintf(stderr,"Usage: %s [-in file][-out file][-thresh #]",
            progName);
    }
    ReadXim(in_fp, &xim);
    (void)(*closefunc)(in_fp);

    if (xim.nchannels < 3 && xim.bits_channel < 8)
        XimError("Image must be in 24 bit (3 channel) format.");
    Smoothim(&xim, T);

    /*  Write the file */
    WriteXim(out_fp, &xim);
    (void)fclose(out_fp);
}

/* end smooth.c */
