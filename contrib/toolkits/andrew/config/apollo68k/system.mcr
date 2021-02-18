/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <apollo68k/system.h>
        SYSTEM_H_FILE = apollo68k/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = apollo68k
        SYS_OS_ARCH = apollo_68k

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        XSRCDIR = /afs/andrew/asa/src/mit/X.V11R3/


/* Get site-specific inclusions */
#include <site.mcr>
