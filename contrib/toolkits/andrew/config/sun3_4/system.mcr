/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <sun3_4/system.h>
        SYSTEM_H_FILE = sun3_4/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = sun3_4
        SYS_OS_ARCH = sun_68k

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
/* (nothing at present) */

/* Get site-specific inclusions */
#include <site.mcr>
