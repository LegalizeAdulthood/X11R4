/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"Apollo_4.3bsd"
#define	sys_apollo_domain_os	1
#define	SYS_NAME	"Apollo_Domain/OS"
#define	SUN_ENV


/* Here follow the overrides for this system. */
#undef	SY_B43
#define	SY_B43	1 /* This system is most like bsd 4.3 */


/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
