/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"HPUX_300"
#define	sys_hpux_300	1
#define	SYS_NAME	"hp300"
#define	HP300_ENV


/* Here follow the overrides for this system. */
#undef	SY_U52
#define	SY_U52	1   /* HP300 is most like SysVR2. */

#undef	RESOLVER_ENV

#define	BUILDANDREWINSTALL_ENV	1


/* Now follow the site-specific customizations. */
#include <site.h>


#endif	/* SYSTEM_H */
