/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/thiscell.c,v 2.7 89/05/08 08:06:54 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/thiscell.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/thiscell.c,v 2.7 89/05/08 08:06:54 ghoti Exp $";
#endif /* lint */

#include <andyenv.h>
#include <stdio.h>
#include <ctype.h>
#include <andrewos.h>		/* strings.h */
#include <util.h>
#include <svcconf.h>
#include <errno.h>

#ifdef AFS_ENV
#include <rx/xdr.h>
#include <sys/ioctl.h>
#include <afs/vice.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif /* AFS_ENV */

#define	MAXSIZE		200

extern char *malloc();
extern int errno;

/*
  * GetCurrentWSCell
  *
  * Description:
  *	Return the string name of the cell to which the current workstation belongs.
  *
  * Arguments:
  *	Buf	: Pointer to character buffer in which we'll place the name of
  *		    the workstation's cell.
  *	size	: The max size available in the buffer.
  *
  * Returns:
  *	Any of the standard system error return codes.  Expected values are:
  *		0	    if everything went well.
  *		EINVAL	    if the AFS basename used was wrong.
  *		ENOENT   ditto
  *		EDOM	    if there wasn't enough room in the user's buffer.
  *
  * Side Effects:
  *	Nothing interesting.
  */

int GetCurrentWSCell(Buf, size) 
char *Buf;
int   size;
{
    char *CellName;
    long code;			/* Result of pioctl() call */
#ifdef AFS_ENV
    struct ViceIoctl blob;		/* I/O control area for Venus communication */
    char vcommbuff[MAXSIZE];	/* Communication buffer */
#endif /* AFS_ENV */

    CheckServiceConfiguration();
#ifdef AFS_ENV
    if (AMS_ViceIsRunning) {
	/*
	 * Prepare the pioctl I/O block.
	 */
	blob.in_size  = 0;
	blob.in       = 0;
	blob.out_size = MAXSIZE;
	blob.out      = vcommbuff;

	code = pioctl(ViceFile, _VICEIOCTL(31), &blob, 1);
	CellName = vcommbuff;
    } else
#endif /* AFS_ENV */
    {
	CellName = ThisDomain;
	code = 0;
    }

    if (code == 0) {
	/*
	 * Copy out the cell name into the supplied buffer if there's room.
*/
	if (size > strlen(CellName))
	    strcpy(Buf, CellName);
	else {
	    Buf[0] = '\0';    /*Put null string in buffer */
	    code = EDOM;
	}
    } else if (code < 0) code = errno;
    return(code);
}

/*
  * GetCurrentUserPrimaryCell
  *
  * Description:
  *	Return the string name of the cell in which the caller's primary tokens are valid.
  *
  * Arguments:
  *	Buf	: Pointer to character buffer in which we'll place the name of
  *		    the user's primary cell.  May be set to the null string if the
  *		    user doesn't have any valid primary tokens.
  *	size	: The max size available in the buffer.
  *
  * Returns:
  *	Any of the standard system error return codes.  Expected values are:
  *		0	    if everything went well.
  *		EINVAL	    if the AFS basename used was wrong.
  *		ENOENT   ditto
  *		EDOM	    if there wasn't enough room in the user's buffer.
  *
  * Side Effects:
  *	Nothing interesting.
  */

int GetCurrentUserPrimaryCell(Buf, size)
char *Buf;
int   size;
{
    char *CellName;
    long code;			/* Result of pioctl() call */
#ifdef AFS_ENV
    struct ViceIoctl blob;		/* I/O control area for Venus communication */
    char vcommbuff[MAXSIZE];	/* Communication buffer */
#endif /* AFS_ENV */

    CheckServiceConfiguration();
#ifdef AFS_ENV
    if (AMS_ViceIsRunning) {
	/* Prepare the pioctl I/O block. */
	blob.in_size  = 0;
	blob.in       = 0;
	blob.out_size = MAXSIZE;
	blob.out      = vcommbuff;

	code = pioctl(ViceFile, _VICEIOCTL(33), &blob, 1);
	CellName = vcommbuff;
    } else
#endif /* AFS_ENV */
    {
	CellName = ThisDomain;
	code = 0;
    }

    if (code == 0) {
	/*
	 * Copy out the primary cell name into the supplied buffer if there's room.
      */
	if (size > strlen(CellName))
	    strcpy(Buf, CellName);
	else {
	    Buf[0] = '\0';    /*Put null string in buffer */
	    code = EDOM;
	}
    } else if (code < 0) code = errno;
    return(code);
}

/*
  * GetCellFromFileName
  *
  * Description:
  *	Given a file name, return the string name of the cell in which that file resides.
  *
  * Arguments:
  *	FileName    : Name of the file to locate.
  *	Buf	    : Pointer to character buffer in which we'll place the name of
  *			the cell hosting the above file.
  *	size	    : The max size available in the buffer.
  *
  * Returns:
  *	Any of the standard system error return codes.  Expected values are:
  *		0	    if everything went well.
  *		EINVAL	    if the AFS basename used was wrong.
  *		ENOENT   ditto, or if the file couldn't be found in AFS.
  *		EDOM	    if there wasn't enough room in the user's buffer.
  *
  * Side Effects:
  *	Nothing interesting.
  */

int GetCellFromFileName(FileName, Buf, size)
char *FileName;
char *Buf;
int   size;
{
    char *CellName;
    long code;			/* Result of pioctl() call */
#ifdef AFS_ENV
    struct ViceIoctl blob;		/* I/O control area for Venus communication */
    char vcommbuff[MAXSIZE];	/* Communication buffer */
#endif /* AFS_ENV */

    CheckServiceConfiguration();
#ifdef AFS_ENV
    if (AMS_ViceIsRunning) {
	/* Prepare the pioctl I/O block. */
	blob.in_size  = strlen(FileName) + 1;
	blob.in       = FileName;
	blob.out_size = MAXSIZE;
	blob.out      = vcommbuff;

	code = pioctl(FileName, _VICEIOCTL(30), &blob, 1);
	CellName = vcommbuff;
    } else
#endif /* AFS_ENV */
    {
	CellName = ThisDomain;
	code = 0;
    }

    if (code == 0) {
	/*
	 * Copy out the file's host cell name into the supplied buffer if there's room.
	 */
	if (size > strlen(CellName))
	    strcpy(Buf, CellName);
	else {
	    Buf[0] = '\0';  /*Put null string in buffer */
	    code = EDOM;
	}
    } else if (code < 0) code = errno;
    return(code);
}

#ifdef TESTINGONLYTESTING
main()
{
    char Foo[5000];
    char FileName[128];
    int retcode, Ix;
    static char *FNs[] = {"/cmu/itc/nsb", "/cmu/rw/afs/psy.cmu.edu", "/cmu/nosuchthing", "/tmp"};
#define numFNs	(sizeof(FNs) / sizeof(FNs[0]))

    fprintf(stderr, "\nTesting the pioctl support for Cellular AMS.\n\n");

    fprintf(stderr, "Getting this workstation's home cell...\n");
    retcode = GetCurrentWSCell(Foo, sizeof(Foo));
    if (retcode == 0)
	fprintf(stderr, "\tHome cell is '%s'\n", Foo);
    else
	fprintf(stderr, "GetCurrentWSCell: %s\n", UnixError(retcode));

    fprintf(stderr, "Getting my primary cell...\n");
    retcode = GetCurrentUserPrimaryCell(Foo, sizeof(Foo));
    if (retcode == 0) {
	if (strlen(Foo) > 0)
	    fprintf(stderr, "\tPrimary authentication in cell '%s'\n", Foo);
	else
	    fprintf(stderr, "\t** You don't hold any primary tokens **\n");
    }
    else
	fprintf(stderr, "GetCurrentUserPrimaryCell: %s\n", UnixError(retcode));

    fprintf(stderr, "Finding out where files live...\n");

    for (Ix = 0; Ix < numFNs; ++Ix) {
	retcode = GetCellFromFileName(FNs[Ix], Foo, sizeof(Foo));
	if (retcode == 0)
	    fprintf(stderr, "\t%s:\t'%s'\n", FNs[Ix], Foo);
	else
	    fprintf(stderr, "GetCellFromFileName(``%s''): %s\n",
		    FNs[Ix], UnixError(retcode));
    }
}
#endif /* TESTINGONLYTESTING */
