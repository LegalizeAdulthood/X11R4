/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"ibm032_aix221"
#define	sys_rt_aix221	1
#define	SYS_NAME	"rt_aix221"
#ifndef	IBM032_ENV
#define	IBM032_ENV
#endif /* IBM032_ENV */


/* Here follow the overrides for this system. */
#undef	SY_AIX221
#define	SY_AIX221	1 /* This system is most like AIX 2.2.1 */

/* This system needs the customized ``install'' program. */
#define	BUILDANDREWINSTALL_ENV	1

/* Things that aren't yet handled */
#undef RESOLVER_ENV
#undef LEVEL_ENV


/* No ditroff here (yet) */
#undef DITROFF_ENV


/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
