/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/procstuf.c,v 2.5 89/02/10 23:26:56 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/procstuf.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

/* convenient stuff for dealing with subprocesses */

#include <andrewos.h>		/* sys/time.h index rindex */
#ifndef hp9000s300
#include <sys/wait.h>
#endif /* hp9000s300 */
#include <ctype.h>
extern char *index(),*rindex(),*getenv();

#define NULL 0
#define FALSE 0
#define TRUE 1

/* trash second arg */
char *argvtostr(argv,buf,len)
char **argv;
char *buf;
int len;
{
    if(argv!=NULL && *argv){
	char **p=argv;

	if(strcmp(p[0],"/bin/sh")==0 && strcmp(p[1],"-c")==0 && strcmp(p[2],"exec ")==0)
	    *(p+=3)+=5; /* skip over the "sh -c exec " */

	if(strlen(*p)+1>len)
	    return NULL;
	strcpy(buf,*p++);

	while(*p!=NULL){
	    if(strlen(*p)+2>len)
		return NULL;
	    strcat(buf," ");
	    strcat(buf,*p++);
	}
    }else
	return NULL;

    return buf;
}

contains(str,set)
char *str,*set;
{
    char *p,*q;

    for(p=str; *p!='\0'; p++)
	for(q=set; *q!='\0'; q++)
	    if(*p==*q)
		return TRUE;

    return FALSE;
}

#define CSHCHARS "$;'\"`~<>*?|()"
#define SHCHARS "$;'\"`<>*?|()"

/* trashes first two args */
char **strtoargv(str,argv,len)
char *str;
char **argv;
int len;
{
    char **p=argv;
    char *s;
    char *shell=getenv("SHELL");
    short csh=FALSE;

    if(shell!=NULL){
	char *leaf=rindex(shell,'/');
	if(leaf==NULL)
	    leaf=shell;
	else
	    leaf++;
	csh=(strcmp(leaf,"csh")==0);
	if(!contains(str,(csh ? CSHCHARS : SHCHARS)))
	    shell=NULL;
    }
	    
    if(shell!=NULL){
	if(len<5)
	    return NULL;
	*p++=shell;
	if(csh)
	    *p++="-f";
	*p++="-c";
	*p++=str;
	/* put 'exec ' in */
	for(s=str+strlen(str); s>=str; s--)
	    *(s+5)= *s;
	bcopy("exec ",str,5);
	*p=NULL;
	return argv;
    }

    /* no special shell characters */
    while(*str && --len>0){
	while(*str && isspace(*str))
	    *str++='\0';
	if(*str){
	    *p++=str;
	    do
		str++;
	    while(*str && !isspace(*str));
	}
    }
    *p=NULL;

    return argv;
}

/* trash second arg */
char *statustostr(status,buf,len)
#ifdef hp9000s300
int *status;
#else /* hp9000s300 */
union wait *status;
#endif /* hp9000s300 */
char *buf;
int len;
{
#ifdef hp9000s300
    sprintf(buf,"status = %d.",*status);
#else /* hp9000s300 */
    if(WIFSTOPPED(*status))
	strcpy(buf,"stopped.");
    else if(status->w_termsig!=0){
	sprintf(buf,"killed by signal %d",status->w_termsig);
	if(status->w_coredump)
	    strcat(buf," (core dumped).");
	else
	    strcat(buf,".");
    }else if(status->w_retcode==0){
	strcpy(buf,"exited.");
	return NULL; /* successful */
    }else
	sprintf(buf,"exited with exit status of %d.",status->w_retcode);
#endif /* hp9000s300 */
    return buf;
}
