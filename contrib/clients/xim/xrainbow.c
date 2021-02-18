/* xrainbow.c
*  All purpose function testing wrapper / debugger.
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
static char  xrainbow_rcsid[] = 
    "$Header: /mit/phils/X/RCS/xrainbow.c,v 1.3 89/11/09 17:29:47 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include "XimHeader.h"

#define  GAMMA  2.0   /* looks good on an ibm/rt */
#define  STEPS  256
#define  MAX    255.999999

char  *progName, *malloc(), *calloc();
int  debug_flag;

main(argc, argv)
int argc;
char **argv;
{
    register i;
    int width = 256, height = 256;
    FILE *out_fp = stdout;
    int pclose(), fclose();
    XimImage xim;
    double gamma = GAMMA, atof();
    double hue, sat, val;
    double red, grn, blu;
    byte *ptr;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_fp = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s\n", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-d", 2) == 0) {
            debug_flag = 1;
            continue;
        }
        if (strncmp(argv[i], "-g", 2) == 0) {
            gamma = atof(argv[++i]);
            continue;
        }
        if (strncmp(argv[i], "-w", 2) == 0) {
            width = atoi(argv[++i]);
            continue;
        }
        if (strncmp(argv[i], "-h", 2) == 0) {
            height = atoi(argv[++i]);
            continue;
        }
        XimError("Usage: %s [-out file][-widht #][-height #][-gamma #]",
            progName);
    }

    xim.width = xim.bytes_per_line = width;
    xim.height = height;
    xim.datasize = xim.height * xim.bytes_per_line;
    xim.nchannels = 1;
    xim.bits_channel = 8;
    xim.ncolors = STEPS;
    xim.colors = (Color *)calloc((unsigned)xim.ncolors, sizeof(Color));
    xim.npics = 1;

    sat = val = 1.0;
    red = grn = blu = 0.0;
    for (i=0; i < STEPS; i++) {
        hue = (double)i / (double)STEPS;
        rainbow(hue, sat, val, &red, &grn, &blu);
        xim.colors[i].red = (byte)floor(MAX * pow(red, 1.0 / gamma));
        xim.colors[i].grn = (byte)floor(MAX * pow(grn, 1.0 / gamma));
        xim.colors[i].blu = (byte)floor(MAX * pow(blu, 1.0 / gamma));
        fprintf(stderr, "%3d: red %3u  grn %3u  blu %3u\n", i,
            xim.colors[i].red, xim.colors[i].grn, xim.colors[i].blu);
    }
    xim.data = (byte *)malloc(xim.datasize);
    for (i=0; i < xim.bytes_per_line; i++)
        xim.data[i] = (byte)(STEPS * ((float)i / xim.bytes_per_line));
    ptr = xim.data + xim.bytes_per_line;
    for (i=0; i < xim.height; i++) {
        bcopy((char*)xim.data, (char*)ptr, xim.bytes_per_line);
        ptr += xim.bytes_per_line;
    }
    WriteXim(out_fp, &xim);
    (void)fclose(out_fp);
    exit(0);
}



/*  rainbow(h, s, v, r, g, b)
*   double h, s, v, *r, *g, *b;
*
*  This routine computes colors suitable for use in color level plots.
*  Typically s=v=1 and h varies from 0 (red) to 1 (blue) in
*  equally spaced steps.  (h=.5 gives green; 1<h<1.5 gives magenta.)
*  To convert for frame buffer, use   R = floor(255.999*pow(*r,1/gamma))
*  etc.
*  To get tables calibrated for other devices or to report complaints,
*  contact  Eric Grosse   research!ehg    201-582-5828.
*/

double huettab[] = {
    0.0000, 0.0062, 0.0130, 0.0202, 0.0280, 0.0365, 0.0457, 0.0559,
    0.0671, 0.0796, 0.0936, 0.1095, 0.1275, 0.1482, 0.1806, 0.2113,
    0.2393, 0.2652, 0.2892, 0.3119, 0.3333, 0.3556, 0.3815, 0.4129,
    0.4526, 0.5060, 0.5296, 0.5501, 0.5679, 0.5834, 0.5970, 0.6088,
    0.6191, 0.6281, 0.6361, 0.6430, 0.6490, 0.6544, 0.6590, 0.6631,
    0.6667, 0.6713, 0.6763, 0.6815, 0.6873, 0.6937, 0.7009, 0.7092,
    0.7190, 0.7308, 0.7452, 0.7631, 0.7856, 0.8142, 0.8621, 0.9029,
    0.9344, 0.9580, 0.9755, 0.9889, 1.0000
};

#ifdef notdef
main()
{
    int i;
    double hue, sat, val;
    double red, grn, blu;

    hue = sat = val = 1.0;
    red = grn = blu = 0.0;
    for (i=0; i <= STEPS; i++) {
        hue = (double)i / (double)STEPS;
        rainbow(hue, sat, val, &red, &grn, &blu);
        red = floor(MAX * pow(red, 1.0 / GAMMA));
        grn = floor(MAX * pow(grn, 1.0 / GAMMA));
        blu = floor(MAX * pow(blu, 1.0 / GAMMA));
        printf("%d: red %3u  grn %3u  blu %3u", i, red, grn, blu);
        printf("\t red %5.3f  grn %5.3f  blu %5.3f\n", red, grn, blu);
    }
}
#endif


/* computed from the FMC-1 color difference formula
*  Barco monitor, max(r,g,b)=1, n=61 magenta,  2 Jan 1986
*/
rainbow(h, s, v, r, g, b)
double h, s, v, *r, *g, *b;
{
    int i;
    double modf(), trash;

    h = 60 * modf((h / 1.5), &trash);
    i = floor(h);
    h = huettab[i] + (huettab[i + 1] - huettab[i]) * (h - i);
    dhsv2rgb(h, s, v, r, g, b);
}


/* here, h=.667 gives blue, h=0 or 1 gives red.
*  see Alvy Ray Smith, Color Gamut Transform Pairs, SIGGRAPH '78
*/
dhsv2rgb(h, s, v, r, g, b)         /* ...hexcone model... */
double h, s, v, *r, *g, *b;        /* all variables in range [0,1[ */
{
    int i;
    double f, m, n, k;
    double modf(), trash;

    h = 6 * modf(h, &trash);
    i = floor(h);
    f = h - i;
    m = (1 - s);
    n = (1 - s * f);
    k = (1 - (s * (1 - f)));
    switch (i) {
    case 0:
        *r = 1;
        *g = k;
        *b = m;
        break;
    case 1:
        *r = n;
        *g = 1;
        *b = m;
        break;
    case 2:
        *r = m;
        *g = 1;
        *b = k;
        break;
    case 3:
        *r = m;
        *g = n;
        *b = 1;
        break;
    case 4:
        *r = k;
        *g = m;
        *b = 1;
        break;
    case 5:
        *r = 1;
        *g = m;
        *b = n;
        break;
    default:
        fprintf(stderr, "bad i: %f %d", h, i);
        exit(1);
    }
    f = *r;
    if (f < *g)
        f = *g;
    if (f < *b)
        f = *b;
    f = v / f;
    *r *= f;
    *g *= f;
    *b *= f;
}
/*** end rainbow.c ***/
/*** end ximtest.c ***/
