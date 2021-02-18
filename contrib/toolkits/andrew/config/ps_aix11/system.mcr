/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <ps_aix11/system.h>
        SYSTEM_H_FILE = ps_aix11/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = ps_aix11
        SYS_OS_ARCH = aix_i386

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        XSRCDIR = /
        XUTILDIR = /usr/amos/bin/X11
        INCLUDES =  -I${BASEDIR}/include/atk -I${BASEDIR}/include -I${XBASEDIR}/usr/include

        CC = cc -DBSD_INCLUDES -DBSD_REMAP_SIGNAL_TO_SIGVEC
#define ConstructMFLAGS 1
        RANLIB = echo ranlib is not needed on this system


/* Get site-specific inclusions */
#include <site.mcr>
