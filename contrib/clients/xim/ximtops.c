/* ximtops.c
* This program will dump the contents of an X window into
* Adobe PostScript raster format.
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
static char xxim_rcs_id[] =
    "$Header: /mit/phils/X/RCS/xxim.c,v 1.4 89/11/09 17:30:02 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include <strings.h>
#include <sys/time.h>
#include <pwd.h>
#include "XimHeader.h"

#define XPS_VERSION "3.2"
#define POINTS_PER_INCH 72.0
#define PAGE_BORDER 0.25
#define PAGE_WIDTH  8.5
#define PAGE_HEIGHT 11.0

char  *progName, *calloc(), *malloc();
int  debug_flag = 0;

main(argc, argv)
int argc;
char **argv;
{
    register  i;
    double user_height = 0.0, atof();
    FILE  *out_fp = stdout, *in_fp = stdin, *popen(), *fopen();
    int pclose(), fclose(), (*closefunc)() = fclose;
    XimImage *xim;

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
        if (strncmp(argv[i], "-h", 2) == 0) {
            user_height = atof(argv[++i]);
            continue;
        }
        XimError("Usage: %s [-in file] [-out file] [-height inches]",
                progName);
    }

    xim = (XimImage *)calloc(1, sizeof(XimImage));
    if (!ReadXim(in_fp, xim))
        XimError("can't read image");

    (void)XimWritePostscriptFile(out_fp, xim, user_height);

    /* Close the files.  */
    (void)(*closefunc)(in_fp);
    (void)fclose(out_fp);
    exit(0);
} /* end main */

/* End of ximtops.c */
