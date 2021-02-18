/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"ibm032_4.3bsd"
#define	sys_rt_r3	1
#define	SYS_NAME	"rt_r3"
#ifndef	IBM032_ENV
#define	IBM032_ENV
#endif /* IBM032_ENV */


/* Here follow the overrides for this system. */
#undef	SY_B43
#define	SY_B43	1 /* This system is most like bsd 4.3 */

#define	NDBM_ENV    1

/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
