/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"ULTRIX_4.2bsd"
#define	sys_vax_22	1
#define	SYS_NAME	"vax_22"
#define	VAX_ENV

/* Here follow the overrides for this system. */
#undef	SY_B43
#define	SY_B43	1 /* This system is most like bsd 4.3 */


/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
