/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <vax_22/system.h>
        SYSTEM_H_FILE = vax_22/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = vax_22
        SYS_OS_ARCH = dec_vax

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        XUTILDIR = ${BASEDIR}/bin/X11


/* Get site-specific inclusions */
#include <site.mcr>
