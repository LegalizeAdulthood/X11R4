/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 *      PC Server - VICE Specific Functions
 *      Access to the VICE File System for IBM PC/XT/ATs
 *
 *      (C)Copyright IBM Corporation, 1986
 *      Program Property of IBM
 *
 *      Version 2.0 by Larry Raper
 *      Developed for the Information Technology Center at
 *      Carnegie-Mellon University
 *
 */

#include <andyenv.h>
#include <pcserver.h>
extern int PCS_debuglevel;
#define	TRUE	1
#define	FALSE	0

#ifdef AFS_ENV
#define VIRTUE
#include <rx/xdr.h>
#include <sys/ioctl.h>
#include <afs/vice.h>
#include <afs/venus.h>
#include <sys/stat.h>
#undef VIRTUE
#undef AFS_ENV
#include <afs/prs_fs.h>
#include <afs/afsint.h>

#define MAXSIZE 2000

GetQuotaInfo (pathname, Bytes_Per_AU, Total_AUs, Remaining_AUs)
char *pathname;
int Bytes_Per_AU, *Total_AUs, *Remaining_AUs;
{
    struct ViceIoctl blob;
    char space[MAXSIZE];
    VolumeStatus  *status;
    long code, temp;

    blob.out_size = MAXSIZE;
    blob.in_size = 0;
    blob.out = space;
    /*code = pioctl(pathname,_VICEIOCTL(4), &blob,TRUE);*/
    code = pioctl(pathname, VIOCGETVOLSTAT, &blob, 1);
    if (code == 0) {
	status = (VolumeStatus *) space;
	if (status->MaxQuota > 0) {
	    temp = (status->MaxQuota - status->BlocksInUse);
	    temp = (temp < status->PartBlocksAvail ?
		    temp*1024 : status->PartBlocksAvail*1024);
	    *Remaining_AUs = temp / Bytes_Per_AU;
	    *Total_AUs = ((long)(status->MaxQuota*1024)) / Bytes_Per_AU;
	}
	else {
	    *Remaining_AUs = ((long)(status->PartBlocksAvail*1024))
	      / Bytes_Per_AU;
	    *Total_AUs = ((long)(status->PartMaxBlocks*1024)) / Bytes_Per_AU;
	}
    }
    else {
	*Total_AUs = 10000;         /* Only dummy figures avail */
	*Remaining_AUs = 2000;
    }
}

ViceCleanup ()
{
#ifdef RUBBISH
    struct ViceIoctl blob;

    blob.in_size = 0;               /* Invalidate pag info */
    blob.out_size = 0;
    Beside being rubbish this call passes the wrong number
      of arguments
      pioctl("/cmu", VIOCUNPAG, &blob);
#endif /* RUBBISH */
}

int Vice ()
{
    return (TRUE);
}

#else /* AFS_ENV */

#ifdef AIX
#include <sys/stat.h>
#include <andrewos.h> /* sys/types.h */
#include <ustat.h>
#endif /* AIX */

GetQuotaInfo (pathname, Bytes_Per_AU, Total_AUs, Remaining_AUs)
char *pathname;
int Bytes_Per_AU, *Total_AUs, *Remaining_AUs;
{
#ifdef AIX
    struct stat devbuf;
    struct ustat buf;

    if ((stat (pathname, &devbuf) == 0) && (ustat (devbuf.st_dev, &buf) == 0)) {
	*Remaining_AUs = (int) ((buf.f_tfree * 512)/Bytes_Per_AU);
	*Total_AUs = (10000 > *Remaining_AUs ? 10000 : *Remaining_AUs+1000);
	return;
    }

#else /* AIX */
    *Total_AUs = 10000;             /* Only dummy figures avail */
    *Remaining_AUs = 2000;
#endif /* AIX */
}

ViceCleanup ()
{
}

int Vice ()
{
    return (FALSE);
}

#endif /* AFS_ENV */

Terminate (rc)
int rc;
{
    CleanUpOrphans ();
    ViceCleanup ();
    DBGMSG (1, ("Terminating - rc: %d", rc));
    exit (rc);
}
#ifdef NOT_IF_IAM_MAINTAINING_IT_YOU_DONT
/* Disgusting hack to avoid VICE dependencies in jr's gasp.c */

U_GetLocalTokens ()
{
}
#endif /* NOT_IF_IAM_MAINTAINING_IT_YOU_DONT */

