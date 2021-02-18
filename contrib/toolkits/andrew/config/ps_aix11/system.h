/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"ps2_aix11"
#define	sys_ps_aix11	1
#define	SYS_NAME	"ps_aix11"
#ifndef	i386_ENV
#define	i386_ENV
#endif /* i386_ENV */


/* Here follow the overrides for this system. */
#undef	SY_AIX11
#define	SY_AIX11	1 /* This system is most like AIX 1.1 */

#undef RESOLVER_ENV

#undef LEVEL_ENV
#define LEVEL_ENV 1

#undef MK_BLD_BLKS
#undef DITROFF_ENV


/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
