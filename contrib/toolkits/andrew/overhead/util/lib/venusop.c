/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*

	venusop.c -- routines that are specific to Vice file system, doing Venus operations

	VenusFlush(pathname): take a pathname and do an ``fs flush'' op.

	VenusFetch(pathname): take a pathname and pre-fetch it.

	VenusCancelStore(fid): prevent the close(fid) from storing the file.

	VenusFlushCallback(pathname): flush the callback, but not the data, for the named file.

	All routines return the result of the pioctl/ioctl and leave error codes in errno.
*/

#include <andyenv.h>
#include <errno.h>
#include <stdio.h>
#include <andrewos.h>		/* sys/types.h */
#include <sys/ioctl.h>
#ifdef AFS_ENV
#include <afs/venus.h>
#endif /* AFS_ENV */

int VenusFlush(pname)
char *pname;
{
#ifdef AFS_ENV
    if (ViceIsRunning()) {
	struct ViceIoctl dummy;
	dummy.in_size = 0;
	dummy.out_size = 0;
	return (pioctl(pname, VIOCFLUSH, &dummy, 1));
    } else
#endif /* AFS_ENV */
	return 0;
}

int VenusFlushCallback(pname)
char *pname;
{
#ifdef AFS_ENV
    if (ViceIsRunning()) {
	struct ViceIoctl dummy;
	dummy.in_size = 0;
	dummy.out_size = 0;
	return(pioctl(pname, VIOCFLUSHCB, &dummy, 1));
    } else
#endif /* AFS_ENV */
	return 0;
}

int VenusFetch(pname)
char *pname;
{
#ifdef AFS_ENV
    if (ViceIsRunning()) {
	struct ViceIoctl dummy;
	dummy.in_size = 0;
	dummy.out_size = 0;
	return(pioctl(pname, VIOCPREFETCH, &dummy, 1));
    } else
#endif /* AFS_ENV */
	return 0;
}

int VenusCancelStore(fid)
int fid;
{
#ifdef AFS_ENV
    if (ViceIsRunning()) {
	struct ViceIoctl dummy;
	dummy.in_size = 0;
	dummy.out_size = 0;
	return(ioctl(fid, VIOCABORT, &dummy));
    } else
#endif /* AFS_ENV */
	return 0;
}
