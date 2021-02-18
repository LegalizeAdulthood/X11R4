/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char rcsid[]="$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/getpty.c,v 1.4 89/11/16 15:06:38 tpn Exp $";

/*
This module contains the routine getpty.  It returns file descriptors
for a pty. It takes two pointers to ints and returns the file decriptors.
It returns TRUE if it succeeds.
*/

#include <andrewos.h>

#include <sys/param.h>
#ifdef hpux
#include <sys/bsdtty.h>
#include <sys/ptyio.h>
#endif /* hpux */

#ifdef AIX
#include <sys/ioctl.h>
#include <sys/devinfo.h>
#include <sys/pty.h>
#include <sys/tty.h>
#endif /* AIX */


#ifdef hpux
#define MASTER_PTY_PREFIX "/dev/ptym/pty"
#define SLAVE_PTY_PREFIX "/dev/pty/tty"
#define MAX_PTYS 64
#endif /* HPUX */

#ifdef AIX
#define MASTER_PTY_PREFIX "/dev/ptc"
#define SLAVE_PTY_PREFIX "/dev/pts"
#define MAX_PTYS 256
#endif /* AIX */

#ifndef MASTER_PTY_PREFIX
#define MASTER_PTY_PREFIX "/dev/pty"
#define SLAVE_PTY_PREFIX "/dev/tty"
#define MAX_PTYS 64
#endif

static char *GetPtyNumberString(num)
int num;
{
static char ptyNum[10];

#ifdef AIX
    sprintf(ptyNum, "%d", num);
#else /* AIX */
    ptyNum[0] = 'p' + (num/16);
    ptyNum[1] = "0123456789abcdef"[num%16];
    ptyNum[2] = '\0';
#endif /* AIX */
    return ptyNum;
}
int GetPtyandName(masterFD, slaveFD,name,len)
int *masterFD;
int *slaveFD;
char *name;
int len;
{
    int PtyNumber = 0;
    char ptcname[MAXPATHLEN];
    char ptyname[MAXPATHLEN];
    char *ptyNum;
    int master;
    int slave;

    while (PtyNumber++ < MAX_PTYS) {
	ptyNum = GetPtyNumberString(PtyNumber);
	strcpy(ptcname, MASTER_PTY_PREFIX);
	strcat(ptcname, ptyNum);
	if ((master = open (ptcname, 2)) >= 0) {
#ifdef AIX
	{
	    int value;

	    if ((value = ioctl(master, PTYSTATUS, 0)) == -1 || (value & 0xffff) != 0 || (value >> 16) != 1)  {
		close(master);
		continue;
	    }
	}
#endif /* AIX */
	    strcpy(ptyname, SLAVE_PTY_PREFIX);
	    strcat(ptyname, ptyNum);
	    if(name != NULL) strncpy(name,ptyname,len);
	    if ((slave = open(ptyname, 2)) >= 0)  {
		if (masterFD != NULL)
		    *masterFD = master;
		if (slaveFD != NULL)
		    *slaveFD = slave;
		return 1;
	    }
	    else
		close(master);
	}
    }

    return 0;
}
int GetPty(masterFD, slaveFD)
int *masterFD;
int *slaveFD;
{
    return GetPtyandName(masterFD, slaveFD,NULL,0);
}
