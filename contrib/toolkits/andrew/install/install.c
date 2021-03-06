/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/install/RCS/install.c,v 1.3 89/11/20 15:04:37 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/install/RCS/install.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/install/RCS/install.c,v 1.3 89/11/20 15:04:37 cfe Exp $ ";
#endif

/* ALSO utimes and strip the file

Generic install command.  Options are:
	-s 			strip the file	(default for executable files with no extension)
	-ns			do not strip the file	(default for other files)
	-c			ignored for compatability
	-m <mode>	chmod to this value
	-o <user>		chown to this user
	-g <group>	chgrp to this group
*/

#define MAXFILES 200
#define BUFSIZE 32768
#include <andrewos.h>
#ifndef MAXPATHLEN
#include <sys/param.h>
#endif /* MAXPATHLEN */
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <a.out.h>

struct stat istat, ostat;

extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

char *ErrorString(aerrno)
    int aerrno; {
    static char tbuffer[100];
    if (aerrno < 0 || aerrno >= sys_nerr) {
	sprintf(tbuffer, "undefined error code %d", aerrno);
	return tbuffer;
    }
    return sys_errlist[aerrno];
}

stripName(aname)
    char *aname;
    {if (rindex(aname, '.') == 0) return 1;
    else return 0;
    }

atoo(astr)
    register char *astr;
    {register long value;
    register char tc;
    value = 0;
    while (tc = *astr++)
	{value <<= 3;
	value += tc-'0';
	}
    return value;
    }

static int quickStrip (afd, asize, fileName)
    int afd;
    long asize;
    char *fileName;
    {int n, bytesLeft;
    struct exec buf;
    struct exec *head;

#ifdef ZMAGIC
    n = lseek(afd, 0, 0);
    if (n < 0) {printf("Initial lseek failed while stripping file: %s\n", ErrorString(errno)); return -1;}
    n = read(afd, &buf, sizeof(buf));
    if (n < 0) {printf("Initial read failed while stripping: %s\n", ErrorString(errno)); return -1;}
    head = &buf;
    if (n >= sizeof(*head) && !N_BADMAG(*head))
	{/* This code lifted from strip.c. */
	bytesLeft = (long) head->a_text + head->a_data;
	head->a_syms = head->a_trsize = head->a_drsize = 0;
	if (head->a_magic == ZMAGIC)
	    bytesLeft += N_TXTOFF(*head) - sizeof(*head);
	/* also include size of header */
	bytesLeft += sizeof(*head);
	n = lseek(afd, 0, 0);
	if (n < 0) {printf("lseek failed while stripping file: %s\n", ErrorString(errno)); return -1;}
	n = write(afd, &buf, sizeof(buf));
	if (n < 0) {printf("write failed while stripping file: %s\n", ErrorString(errno)); return -1;}
	}
    else
	bytesLeft = 0;

     /* check if size of stripped file is same as existing file */
     if (bytesLeft != 0 && bytesLeft != asize)
	{if (ftruncate(afd, bytesLeft) < 0)
	    {printf("ftruncate failed after stripping file: %s\n", ErrorString(errno)); return -1;}
	}
#else /* ZMAGIC */
     {
	 char stripCommand[MAXPATHLEN + 7];
	 sprintf(stripCommand, "strip %s", fileName);
	 if (system(stripCommand) != 0)
	     return -1;
     }
#endif /* ZMAGIC */    
    return 0;
    }

main (argc, argv)
    int argc;
    char **argv;
    {int setOwner, setMode, setGroup, ifd, ofd;
    long mode, owner, group;
    struct passwd *tpw;
    char *fnames[MAXFILES], *newNames[MAXFILES];
    long rcode, code, newcode;
    char *dname;
    char pname[1024];
    char pnametmp[1024];
    char FirstChar;
    char IsFirst;
    int pnamelen;
    static char diskBuffer[BUFSIZE];	/* must be static to avoid compiler bugs for large stuff */
    char myHostName[100];
    struct timeval tvp[2];
    int isDir;
    int strip;
    int fptr;
    register char *tp;
    register long i;

    fptr = 0;
    rcode = 0;
    strip = -1;	/* don't know yet */
    owner = 0;
    setOwner = 0;
    setMode = 0;
    group = 0;
    setGroup = 0;

    for(i=1; i<argc; i++)
	{tp = argv[i];
	if (tp[0] == '-')
	    {/* a switch */
	    if (!strcmp(tp, "-m")) mode = atoo(argv[++i]), setMode=1;
	    else if (!strcmp(tp, "-s")) strip = 1;
	    else if (!strcmp(tp, "-ns")) strip = 0;
	    else if (!strcmp(tp, "-c")) /* nothing */;
	    else if (!strcmp(tp, "-o"))
		{/* look up the dude */
		tpw = getpwnam(argv[++i]);
		if (!tpw)
		    {printf("User %s not found in passwd database, ignored\n", argv[i]);
		    }
		else
		    {owner = tpw->pw_uid;
		    setOwner =1;
		    }
		}
	    else if (!strcmp(tp, "-g"))
		{/* look up the dude */
		tpw = getpwnam(argv[++i]);
		if (!tpw)
		    {printf("Group %s not found in passwd database, ignored\n", argv[i]);
		    }
		else
		    {group = tpw->pw_gid;
		    setGroup =1;
		    }
		}
		else printf("Bad switch %s\n", argv[i]);
	    }
	else
	    {/* a file name */
	    if (fptr >= MAXFILES)
		{printf("Too many files on command line, max is %d\n", MAXFILES);
		exit(1);
		}
	    fnames[fptr++] = argv[i];
	    }
	}

    /* we've parse the commands, now *do* them */

    /* otherwise we are doing a local install, so we do the work for each file here */
    /* the last name in the fname array is the dir in which to put all this stuff */

    if (fptr < 2)
	{printf("Not enough file names\n");
	exit(1);
	}

    dname = fnames[--fptr];
    if (stat(dname, &istat) < 0) {
	if (errno == ENOENT)
	    isDir = 0;		/* creating a new file */
	else {
	    printf("Can't stat destination ``%s'': %s\n", dname, ErrorString(errno));
	    exit(1);
	}
    } else {
	if ((istat.st_mode & S_IFMT) == S_IFDIR)  isDir = 1;
	else isDir = 0;
    }

    /* either can be n files and one dir, or one file and one target file */
    if (!isDir && fptr != 1) return printf("target for multiple files must be a dir\n");

    for (i=0;i<fptr;i++)
	{/* figure out name to put as entry name for file */
	tp = rindex(fnames[i], '/');
	if (tp) newNames[i] = tp+1;
	else newNames[i] = fnames[i];
	}
    for (i=0;i<fptr;i++)
	{/* copy newName[i] into directory dname */

	/* pname is target file in either case */
	if (isDir)
	    {strcpy(pname, dname);
	    strcat(pname, "/");
	    strcat(pname, newNames[i]);
	    }
	else strcpy(pname, dname);
	strcpy(pnametmp, pname);
	/* Make up a temporary name for a destination */
	pnamelen = strlen(pnametmp);
	gethostname(myHostName, sizeof(myHostName)-1);	/* lv room for null */
	if (pnamelen > 1020 - strlen(myHostName)) pnamelen = 1020 - strlen(myHostName);
	pnametmp[pnamelen] = '.';
	strcpy(&pnametmp[pnamelen+1], myHostName);
	if (strcmp(fnames[i], pnametmp) == 0) strcpy(&pnametmp[pnamelen], ".NeW");

	ifd = open(fnames[i], O_RDONLY, 0);
	if (ifd < 0)
	    {printf("Can't open source file ``%s'': %s\n", fnames[i], ErrorString(errno));
	    rcode = 1;
	    continue;
	    }
	if (fstat (ifd, &istat) < 0) {
	    printf("Cound not fstat input file ``%s'': %s; skipping it\n", fnames[i], ErrorString(errno));
	    close(ifd);
	    rcode = 1;
	    continue;
	}
	/* check to see if this file is hard to duplicate */
	ofd = open(pnametmp, O_RDWR | O_TRUNC | O_CREAT, 0666);
	if (ofd < 0)
	    {printf("Could not create output temp file ``%s'': %s\n", pnametmp, ErrorString(errno));
	    close(ifd);
	    rcode = 1;
	    continue;
	    }
	if (!setMode) mode = 0755;	/* this is the default for our rcs to work */
	/* here both files are open and ready to go */
	IsFirst = 1;
	FirstChar = '#';
	while (1)
	    {code = read(ifd, diskBuffer, BUFSIZE);
	    if (code == 0) break;
	    if (code < 0)
		{printf("READ ERROR %d: %s\n", errno, ErrorString(errno));
		break;
		}
	    if (IsFirst) {
		FirstChar = diskBuffer[0];
		IsFirst = 0;
	    }
	    errno = 0;
	    newcode = write(ofd, diskBuffer, code);
	    if (newcode != code)
		{printf("WRITE ERROR %d: %s\n", errno, ErrorString(errno));
		break;
		}
	    }
	if (code != 0)
	    {rcode = 1;	/* an error occurred copying the file */
	     printf("Warning: Error occurred writing output temp file %s; skipping it\n",
			 pnametmp);
	    close(ifd); unlink(pnametmp); close(ofd);
	    continue;	/* to the next file */
	    }
	/* strip the file? */
	if (strip == 1 ||
	    (strip == -1 && ((istat.st_mode & 0111) == 0111) && stripName(newNames[i]) && FirstChar != '#'))
		if (quickStrip(ofd,istat.st_size, pnametmp) < 0) {
		    printf("...strip failed for output temp file ``%s''; skipping it\n", pnametmp);
		    close(ifd); unlink(pnametmp); close(ofd);
		    rcode = 1;
		    continue;
		}

	/* do the chmod, etc calls before closing the file for max parallelism on store behind */
	close(ifd);

	if (fchmod(ofd, mode) < 0)
	    {printf("Couldn't chmod output temp file ``%s'': %s\n",
			pnametmp, ErrorString(errno));
	    unlink(pnametmp); close(ofd);
	    rcode = 1;
	    continue;
	    }

#ifndef hpux
	tvp[0].tv_sec = istat.st_atime;
	tvp[0].tv_usec = 0;
	tvp[1].tv_sec = istat.st_mtime;
	tvp[1].tv_usec = 0;
	if (utimes(pnametmp, tvp) < 0)
	    {printf("Couldn't utimes output temp file ``%s'': %s\n",
			pnametmp, ErrorString(errno));
	    unlink(pnametmp); close(ofd);
	    rcode = 1;
	    continue;
	    }
#endif /* hpux */

	code = close(ofd);
	if (code != 0)
	    {printf("Warning: Could not close output temp file %s (%s)\n",
			pnametmp, ErrorString(errno));
	    unlink(pnametmp);
	    rcode = 1;	/* an error occurred closing the output file */
	    continue;	/* to the next file */
	    }

	/* do this later so vice doesn't see chown of unstored file */
	if (setOwner || setGroup)
	    if (chown(pnametmp, (setOwner? owner : -1), (setGroup? group : -1)) < 0) {
		printf("Couldn't set %s for output temp file %s: %s\n",
			(setOwner? (setGroup? "owner and group" : "owner") : "group"),
			pnametmp, ErrorString(errno));
		unlink(pnametmp);
		rcode = 1;
		continue;
	    }
	if (rename(pnametmp, pname) < 0)
	    {printf("Couldn't rename temp file %s to be output file %s: %s\n",
			pnametmp, pname, ErrorString(errno));
	    unlink(pnametmp);
	    rcode = 1;
	    continue;
	    }
	}
    /* all done now */
    exit(rcode);
    }
