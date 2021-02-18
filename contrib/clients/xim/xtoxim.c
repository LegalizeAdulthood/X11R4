/* xtoxim.c
*  Author: Philip Thompson (phils@athena.mit.edu)
*   - Converts XWDFiles to "xim" 8 bit format.
*   - Also translates parallax video to 24 bit format.
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
static char  xtoxim_rcsid[] =
    "$Header: /mit/phils/X/RCS/xtoxim.c,v 1.17 89/11/09 17:29:54 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include "XimHeader.h"

char *progName;
char *calloc(), *malloc(), *realloc(), *strncpy();
extern int debug_flag;

main(argc, argv)
int argc;
char **argv;
{
    register int  i;
    int plx_flag = 0;
    FILE *in_fp = stdin, *out_file = stdout, *fopen();
    XimImage *xim;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            if ((in_fp = fopen(argv[++i], "r")) == NULL)
                XimError("Can't read input file: %s.", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_file = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s\n", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-p", 2) == 0) {
            plx_flag = 1;
            continue;
        }
        if (strncmp(argv[i], "-d", 2) == 0) {
            debug_flag = 1;
            continue;
        }
        XimError("Usage: %s [-in file] [-out file] [-plx]", progName);
    }

    if (!(xim = (XimImage *)calloc(1, sizeof(XimImage))))
        XimError("can't calloc header");

    if (!XimReadXwdFile(in_fp, xim, plx_flag))
        XimError("can't read xwd file");

    (void)WriteXim(out_file, xim);
    exit(0);
}

/* End of xtoxim.c */
