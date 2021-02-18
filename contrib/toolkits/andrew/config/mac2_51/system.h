/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"MAC2_mach"
#define	sys_mac2_51	1
#define	SYS_NAME	"mac2_51"
#define	MACH_ENV


/* Here follow the overrides for this system. */
/* for MACH systems */
#undef	SY_MCH
#define	SY_MCH	1


/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
