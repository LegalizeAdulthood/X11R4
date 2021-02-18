/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <vax_3/system.h>
        SYSTEM_H_FILE = vax_3/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = vax_3
        SYS_OS_ARCH = dec_vax

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
/* (currently none) */


/* Get site-specific inclusions */
#include <site.mcr>
