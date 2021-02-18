/*LIBS: -lutil
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/filter.c,v 2.6 89/02/17 16:07:39 ghoti Exp $ */
/* $ACIS:filter.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/filter.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/filter.c,v 2.6 89/02/17 16:07:39 ghoti Exp $";
#endif /* lint */

/* filter the textview selection region through a command
 */

#include <class.h>
#include <andrewos.h>
#include <filter.eh>

#include <im.ih>
#include <proctbl.ih>
#include <textv.ih>
#include <text.ih>
#include <message.ih>

#define IN 1
#define OUT 2
#define FORMAT 4

struct filterdata{
    FILE *infp,*outfp;
/*
    struct region source,sink;
*/
    struct textview *v;
    struct text *t;
    int pos,len;
    short method;
};

static void commandFinished(pid,fd,status)
int pid;
struct filterdata *fd;
union wait *status;
{
    char buf[40],*em,*statustostr();

    strcpy(buf,"Filtering...process ");
    em=statustostr(status,buf+20,20);

    if(em==NULL){ /* sucessful completion */
	if(fd->method&OUT){
	    int oldlen;
	    text_DeleteCharacters(fd->t,fd->pos,fd->len);
	    oldlen=text_GetLength(fd->t);
	    rewind(fd->outfp);
            if (fd->method & FORMAT)
                text_InsertFile(fd->t,fd->outfp,NULL,fd->pos);
            else
                text_ReadSubString(fd->t,fd->pos,fd->outfp,FALSE);
	    textview_SetDotLength(fd->v,text_GetLength(fd->t)-oldlen);
	}
	message_DisplayString(fd->v,0,"Filtering...done.");
    }else
	message_DisplayString(fd->v,1,buf);

    text_NotifyObservers(fd->t,0);

    fclose(fd->infp);
    fclose(fd->outfp);

    free((char *)fd);
}

static void filter(tv,command,method)
struct textview *tv;
char *command;
short method;
{
    static int count=0,pid;
    char *argvbuf[100],**argv,**strtoargv();
    char buf[100];
    struct filterdata *fd=(struct filterdata *)malloc(sizeof(struct filterdata));

/*    if(im_ArgProvided(textview_GetIM(tv))){
	struct buffer *b=buffer_CreateTemp();
	sprintf(buf,"To buffer: [%s] ",buffer_GetName(b));
	message_AskForString(tv,0,buf,NULL,buf,sizeof(buf));
	
    }*/

    if(fd==NULL){
	message_DisplayString(tv,1,"Malloc failed.");
	return;
    }

    fd->v=tv;
    fd->t=(struct text *)tv->header.view.dataobject;
    fd->pos=textview_GetDotPosition(tv);
    fd->len=textview_GetDotLength(tv);
    fd->method=method;

    sprintf(buf,"/tmp/filter.%d.%d",getpid(),count++);

    if(method&IN)
	fd->infp=fopen(buf,"w+");
    else
	fd->infp=fopen("/dev/null","r");
    if(fd->infp==NULL){
	char mbuf[200];
	sprintf("Can't open %s.",buf);
	message_DisplayString(tv,1,mbuf);
	return;
    }
    if(method&IN)
	unlink(buf);

    if(method&OUT)
	fd->outfp=fopen(buf,"w+");
    else
	fd->outfp=fopen("/dev/null","w");
    if(fd->outfp==NULL){
	char mbuf[200];
	sprintf("Can't open %s.",buf);
	message_DisplayString(tv,1,mbuf);
	fclose(fd->infp);
	return;
    }
    if(method&OUT)
	unlink(buf);

    if(method&IN){
	text_WriteSubString(fd->t,fd->pos,fd->len,fd->infp,FALSE);
	fflush(fd->infp);
	rewind(fd->infp);
    }

    argv=strtoargv(command,argvbuf,100);

    switch(pid = osi_vfork()){
	case 0:
	    close(0);
	    close(1);
	    dup(fileno(fd->infp));
	    dup(fileno(fd->outfp));
#if 0
	    {
		char **p;
		write(2,"Execing ",8);
		for(p=argv;*p!=NULL;p++){
		    write(2,*p,strlen(*p));
		    write(2," ",1);
		}
		write(2,"\n",1);
	    }
#endif /* 0 */
	    execvp(*argv,argv);
	    exit(-1);
	    break;
	case -1:
	    message_DisplayString(tv,1,"Fork failed.");
	    return;
	default:
	    im_AddZombieHandler(pid,commandFinished,(long)fd);
	    message_DisplayString(tv,0,"Filtering...");
    }
}

static void filterRegion(tv)
struct textview *tv;
{
    char cbuf[500];
    if(message_AskForString(tv,0,"Command: ",NULL,cbuf,sizeof(cbuf))!=0)
	return;
    filter(tv,cbuf,IN+OUT);
}

static void filterRegionFmt(tv)
struct textview *tv;
{
    char cbuf[500];
    if (message_AskForString(tv,0,"Command: ",NULL,cbuf,sizeof(cbuf))!=0)
        return;
    filter(tv,cbuf,IN+OUT+FORMAT);
}

static void filterRegionThruCommand(tv,command)
struct textview *tv;
char *command;
{
    char cbuf[500];
    strcpy(cbuf,command); /* since filter trashes its input string */
    filter(tv,cbuf,IN+OUT);
}

static void filterRegionThruCmdFmt(tv,command)
struct textview *tv;
char *command;
{
    char cbuf[500];
    strcpy(cbuf,command); /* since filter trashes its input string */
    filter(tv,cbuf,IN+OUT+FORMAT);
}

static void sinkRegionThruCommand(tv,command)
struct textview *tv;
char *command;
{
    char cbuf[500];
    strcpy(cbuf,command); /* since filter trashes its input string */
    filter(tv,cbuf,IN);
}

static void sinkRegion(tv)
struct textview *tv;
{
    char cbuf[500];
    if(message_AskForString(tv,0,"Command: ",NULL,cbuf,sizeof(cbuf))!=0)
	return;
    filter(tv,cbuf,IN);
}

static void sinkRegionThruCmdFmt(tv,command)
struct textview *tv;
char *command;
{
    char cbuf[500];
    strcpy(cbuf,command); /* since filter trashes its input string */
    filter(tv,cbuf,IN+FORMAT);
}

static void sinkRegionFmt(tv)
struct textview *tv;
{
    char cbuf[500];
    if(message_AskForString(tv,0,"Command: ",NULL,cbuf,sizeof(cbuf))!=0)
	return;
    filter(tv,cbuf,IN+FORMAT);
}

boolean filter__InitializeClass(classID)
struct classheader *classID;
{
    struct classinfo *tvi=class_Load("textview");

    if(tvi==NULL)
	return FALSE;

    proctable_DefineProc("filter-filter-region",
			  filterRegion,tvi,NULL,"Prompts for a command and executes it with the selection region as standard input, replacing the region with the output of the command.");
    proctable_DefineProc("filter-sink-region",
			  sinkRegion,tvi,NULL,"Prompts for a command and provides the selected region as standard input to it; the command's output is discarded.");
    proctable_DefineProc("filter-filter-region-thru-command",
			  filterRegionThruCommand,tvi,NULL,"Executes a command with the selection region as standard input, replacing it with the output of the command.");
    proctable_DefineProc("filter-sink-region-thru-command",
			  sinkRegionThruCommand,tvi,NULL,"Provides the selected region as standard input to the specified command; the command's output is discarded.");
    proctable_DefineProc("filter-filter-region-formatted",
			  filterRegionFmt,tvi,NULL,"Prompts for a command and executes it with the selection region as standard input, replacing the region with the output of the command. ATK datastream is read and written.");
    proctable_DefineProc("filter-sink-region-formatted",
			  sinkRegionFmt,tvi,NULL,"Prompts for a command and provides the selected region as standard input to it; the command's output is discarded.  ATK datastream is written.");
    proctable_DefineProc("filter-filter-region-thru-command-formatted",
			  filterRegionThruCmdFmt,tvi,NULL,"Executes a command with the selection region as standard input, replacing it with the output of the command.  ATK datastream is read and written.");
    proctable_DefineProc("filter-sink-region-thru-command-formatted",
			  sinkRegionThruCmdFmt,tvi,NULL,"Provides the selected region as standard input to the specified command; the command's output is discarded.  ATK datastream is written.");

    return TRUE;
}
