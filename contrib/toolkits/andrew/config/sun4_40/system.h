/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"SUN4_4.3bsd"
#define	sys_sun4_40	1
#define	SYS_NAME	"sun4_40"
#define	SUN_ENV

/* Here follow the overrides for this system. */
#undef	SY_B43
#define	SY_B43	1 /* This system is most like bsd 4.3 */

#define	NDBM_ENV    1
#define	GETDOMAIN_ENV	1


/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
