/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <pmax_3/system.h>
        SYSTEM_H_FILE = pmax_3/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = pmax_3
        SYS_OS_ARCH = dec_mips

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        XSRCDIR = /afs/andrew/asa/src/mit/X.V11R3/
/* MIPS' compiler seems to have the standard set of PCC bugs dealing with 
 *     void...
 * The -G 0 is to prevent dynamically loadable modules from having global
 *     area sections.
 * The -Wl,D,1000000 switch is to move the data area down so that
 *     dynamically loaded routines are addressible within the 28 bit jump
 *     offset limit of the MIPS architecture.
 */
        CC = cc -Dvoid=int -G 0 "-Wl,-D,1000000"


/* Get site-specific inclusions */
#include <site.mcr>
