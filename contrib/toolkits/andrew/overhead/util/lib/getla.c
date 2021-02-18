/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/getla.c,v 2.8 89/04/06 11:25:17 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/getla.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <andyenv.h>
#include <system.h>
#include <fdplumb.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <nlist.h>

static struct nlist Nl[] =
{
	{ "_avenrun" },
#define	X_AVENRUN	0
	{ 0 },
};

static int kmem = -1, fpastate = -1, fpacount = 0;

double getla(index)
int index;
{
#ifdef sun
    long avenrun[3];	/* For any kind of sun */
#else /* sun */
    double avenrun[3];
#endif /* sun */

    if (kmem < 0) {
	kmem = open("/dev/kmem", 0);
	if (kmem < 0) return -1.0;
#ifdef FIOCLEX
	(void) ioctl(kmem, FIOCLEX, 0);
#endif /* FIOCLEX */
	nlist("/vmunix", Nl);
	if (Nl[0].n_type == 0) {
	    close(kmem);
	    kmem = -1;
	    return -1.0;
	}
    }
    if (fpastate < 0 || (fpastate > 0 && fpacount <= 0)) {
	fpastate = (fpacheck() == 0 ? 1 : 0);
	fpacount = 100;	/* Check it every 100 calls */
    }
    if (fpacount >= 0) --fpacount;
    if (fpastate <= 0) return -1.0;	/* Default if the FPA is bad. */

    if (lseek(kmem, (long) Nl[X_AVENRUN].n_value, 0) == -1 ||
	 read(kmem, avenrun, sizeof(avenrun)) < sizeof(avenrun)) return -1.0;
#ifdef SUN_ENV
    return (double) avenrun[index] / FSCALE;
#else /* SUN_ENV */
    return avenrun[index];
#endif /* SUN_ENV */
}

getla_ShutDown()
{
    if (kmem >= 0) {
	close(kmem);
	kmem = -1;
    }
}
