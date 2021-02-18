/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/openpipe.c,v 2.6 89/02/10 09:14:24 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/openpipe.c,v $
*/
#include <stdio.h>
#include <andrewos.h>
#include <ms.h>
#include <mailconf.h>

extern char *getenv();
extern FILE *popen();

MS_OpenDebuggingPipescript(DoIt) 
int DoIt;  /* Turns it on or off */
{
    static FILE *ftmp = NULL;

    if (DoIt == 2) {
	char *TypescriptVector[2];
	char TypescriptCommand[100];

	sprintf(TypescriptCommand, AndrewDir("/bin/typescript"));
	TypescriptVector[0] = TypescriptCommand;
	TypescriptVector[1] = NULL;
	if (! osi_vfork()) {
	    int fd;

	    /* I am a child */
	    for (fd = getdtablesize(); fd > 2; --fd) close(fd);
	    execv(TypescriptVector[0], TypescriptVector);
	    NonfatalBizarreError("Could not exec typescript");
	    _exit(1);
	}
	return(0);
    }
    if (DoIt) {
	char PipescriptCommand[100];
	sprintf(PipescriptCommand, AndrewDir("/bin/pipescript -t 'MessageServer Debugging'"));
	if (ftmp) pclose(ftmp);
	ftmp = popen(PipescriptCommand, "w");
	if (ftmp == NULL) {
	    AMS_RETURN_ERRCODE(errno, EIN_POPEN, EVIA_OPENDEBUGGINGPIPESCRIPT);
	}
	if (dup2(fileno(ftmp), 1) == -1) {
	    AMS_RETURN_ERRCODE(errno, EIN_DUP2, EVIA_OPENDEBUGGINGPIPESCRIPT);
	}
    } else {
	if (ftmp) {  /* No need to do it if ftmp is null */
	    pclose(ftmp);
	    dup2(1, 2);
	    ftmp = NULL;
	}
    }
    return(0);
}
