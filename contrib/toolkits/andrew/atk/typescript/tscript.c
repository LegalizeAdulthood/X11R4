/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscript.c,v 2.28 89/11/21 15:26:55 jhh Exp $ */
/* $ACIS:tscript.c 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscript.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscript.c,v 2.28 89/11/21 15:26:55 jhh Exp $";
#endif /* lint */


#include <class.h>
#include <text.ih>
#include <mark.ih>
#include <keystate.ih>

#include <menulist.ih>
#include <proctbl.ih>
#include <keymap.ih>
#include <im.ih>
#include <message.ih>
#include <msghndlr.ih>
#include <typetext.ih>
#include <smpltext.ih>
#include <dataobj.ih>
#include <fontdesc.ih>
#include <envrment.ih>
#include <style.ih>
#include <stylesht.ih>
#include <environ.ih>
#include <print.ih>
#include <filetype.ih>
#include <tscript.eh>

#include <ctype.h>
#include <andrewos.h> /* sys/types.h sys/time.h */
#include <sys/stat.h>
#include <sys/ioctl.h>
#ifdef hpux
#include <sys/bsdtty.h>
#include <sys/ptyio.h>
#endif /* hpux */

#include <signal.h>

#if SY_AIX221
#include <sys/devinfo.h>
#include <sys/pty.h>
#include <sys/termio.h>
#include <sys/tty.h>
#else /* #if SY_AIX221 */
#include <sgtty.h>
#endif /* #if SY_AIX221 */


/* #define DONTCUTSTYLES */
#define MyEnvinfo(text,pos) environment_GetInnerMost(text->rootEnvironment, pos)
#define TEXT(A) ((struct text *)(A->header.view.dataobject))
#define TEXTOBJ(A) A->header.view.dataobject
static char *DefaultArgList[] = {
    0, 0
};
#define ISPIPESCRIPT(self) (self->pipescript)

static char **myarglist = NULL;
static boolean Pipescript = FALSE;
static struct style *staticBoldStyle = NULL;
static struct keymap *ssmap;
static struct menulist *typescriptMenus;
static FILE *df = NULL;
static FILE *odf = NULL;

static void Typescript_SelfInsertCmd();
static void MyCanOutHandler();	
/* The following are initialized from the proctable by our InitializeClass
 * routine. There may well be a better way of doing this. The routines
 * should certainly be void instead of int, but the proctable package
 * is screwed. */
static int 	(*textview_EndOfTextCmd)();
static int 	(*textview_SelfInsertCmd)();
static int    	(*textview_DigitCmd)();
static int 	(*textview_BeginningOfLineCmd)();
static int 	(*textview_EndOfLineCmd)();
static int 	(*textview_YankCmd)();
static int 	(*textview_BackwardsRotatePasteCmd)();
static int 	(*textview_RotatePasteCmd)();
static int 	(*textview_DeleteCmd)();
static int 	(*textview_RuboutCmd)();
static int	(*textview_LineToTop)();
static int 	(*typescript_CopyRegionCmd)(); 
static int 	(*typescript_ZapRegionCmd)(); 

extern char  *realloc();

static long maxSize;
static long extraRoom;
static char *cmd;
static int CmdSize;
static boolean FileMenu = FALSE;
#define SetCmdSize(A) if(A > CmdSize)  cmd = (char *) realloc(cmd,(CmdSize = 64 + A))

static
typescriptAddMenu (nbuf,proc)
char *nbuf;
struct proctable_Entry *proc;
{
    char *c,*bf,*cp,*retstr;
	retstr = NULL;
	bf  = (char *)malloc(strlen(nbuf) + 1);
	for(c = bf,cp = nbuf; *cp != '\0'; cp++ ,c++){
		if( *cp == ':'){
			*c = '\0';
			retstr = c + 1;
			}
		else *c = *cp;
		}
	*c = '\0';
	if(retstr && *retstr){
            menulist_AddToML(typescriptMenus,bf,proc,(long) retstr,0);
        }
	else free( bf);
}
    
static typescript_PreviewCmd(self)
    struct typescript *self;
{
    doprint(self,0);
}

static typescript_PrintCmd(self)
    struct typescript *self;
{
    doprint(self,1);
}
static doprint(self,porp)
struct typescript *self;
int porp;
{


    message_DisplayString(self, 0, "Processing request.");
    im_ForceUpdate();

    if (class_Load("print") == NULL) {
	message_DisplayString(self, 0, "Print aborted: could not load class \"print\".");
        return;
    }
    print_ProcessView((struct view *)self, porp, 1,"pipescript", "");
    message_DisplayString(self, 0, "Print request submitted; watch console for results.");
}
static typescript_SaveAs(self)
   struct typescript *self;
{
    char frs[256],mes[256];
    FILE *f;
    int failed = 0;
    if(message_AskForString(self,0,"File Name: ","",frs,256) == -1) return;
    filetype_CanonicalizeFilename(mes,frs,256);
    if((f = fopen(mes,"w")) == NULL) failed++;
    else {
	text_Write(TEXT(self),f,im_GetWriteID(),0); 
	if (vclose(fileno(f)) < 0) failed++;
    }
    if(failed)
    {
	sprintf(frs,"Can't write %s",mes);
	message_DisplayString(self,0,frs);
	return;
    }
    sprintf(frs,"wrote %s",mes);
    message_DisplayString(self, 0,frs);
}

static typescript_SetPrinterCmd(self)
struct typescript *self;
{

    struct msghandler *messageLine = (struct msghandler *) typescript_WantHandler(self, "message");
    char *currentPrinter, *defaultPrinter, answer[256], prompt[sizeof("Current printer is . Set printer to []: ") + 128];

    if (messageLine == NULL)
        return;
    currentPrinter = environ_Get("PRINTER");
    defaultPrinter = environ_GetProfile("print.spooldir");
    if (currentPrinter != NULL && defaultPrinter != NULL)
        sprintf(prompt, "Current printer is %.64s. Set printer to [%.64s]: ", currentPrinter, defaultPrinter);
    else if (defaultPrinter != NULL)
        sprintf(prompt, "Set printer to [%.64s]: ", defaultPrinter);
    else
        strcpy(prompt, "Set printer to: ");
    if (msghandler_AskForString(messageLine, 0, prompt, NULL, answer, sizeof(answer)) == -1)
        return;
    if (*answer != '\0') {
        environ_Put("PRINTER", answer);
        defaultPrinter = answer;
    }
    else
        environ_Delete("PRINTER");
    if (defaultPrinter != NULL) {
        sprintf(prompt, "Printer set to %.64s.", defaultPrinter);
        msghandler_DisplayString(messageLine, 0, prompt);
    }
    else
        msghandler_DisplayString(messageLine, 0, "Printer not set.");
}

static typescriptAddSearchMenu ()
{
    struct proctable_Entry *tempProc;
    if((tempProc = proctable_Lookup("textview-search")) != NULL){
        menulist_AddToML(typescriptMenus,"Search~20,Forward~10",tempProc,NULL,0);
    }

    if((tempProc = proctable_Lookup("textview-reverse-search")) != NULL){
        menulist_AddToML(typescriptMenus,"Search~20,Backward~11",tempProc,NULL,0);
    }
}

static typescriptAddFileMenu ()
{
    struct proctable_Entry *tempProc;
    struct classinfo *classInfo = &typescript_classinfo;

    tempProc=proctable_DefineProc("typescript-Save-As",	typescript_SaveAs,  classInfo, NULL, "Prompt for file name to save");
    menulist_AddToML(typescriptMenus,"File~30,Save As~10",tempProc,NULL,0);

    tempProc=proctable_DefineProc("typescript-Print", typescript_PrintCmd, classInfo, NULL, "Print typescript");
    menulist_AddToML(typescriptMenus,"File~30,Print~21",tempProc,NULL,0);

    tempProc=proctable_DefineProc("typescript-Preview", typescript_PreviewCmd, classInfo, NULL, "Preview typescript");
    menulist_AddToML(typescriptMenus,"File~30,Preview~22",tempProc,NULL,0);

    tempProc=proctable_DefineProc("typescript-SetPrinter", typescript_SetPrinterCmd, classInfo, NULL, "Set Printer");
    menulist_AddToML(typescriptMenus,"File~30,Set Printer~20",tempProc,NULL,0);
}

static AnounceDeath(self)
struct typescript *self;
{
    char buf[512];
    char *shell = environ_Get("SHELL");

    if(shell == NULL) shell = "/bin/csh";
    if(ISPIPESCRIPT(self)){
	sprintf(buf, "EOF on pipe");
	message_DisplayString(self, 15,buf);
    }
    else if(strcmp(self->progname,shell) == 0) {
	sprintf(buf,"This process died. Start new typescript to continue work.");
	message_DisplayString(self, 100,buf);
    }
    else {
	sprintf(buf,"The %s process died.",self->progname);
	message_DisplayString(self, 100,buf);
    }    
}
static struct environment *GetCommandEnv(self,pos,start,end)
register struct typescript *self;
register long pos;
long *start, *end;
{
        register struct environment *te;

	te = MyEnvinfo(TEXT(self), pos);
	if (te->data.style != staticBoldStyle && pos > 0){
	    /* cursor may be at end of environment */
	    	te = MyEnvinfo(TEXT(self), pos - 1);
	}
	if (te->data.style != staticBoldStyle){
	    return NULL;
	}
	/* here we are in the command's bold region */
	*start = environment_Eval(te);
	*end =  *start + te->header.nestedmark.length;
	return te;
}
static void typescript_RuboutCmd (self)
    register struct typescript *self;
{
    if ((self->readOnlyLen != -1)){
	if(self->readOnlyLen > 0) self->readOnlyLen--;
    }
    else if(typescript_GetDotPosition(self) + typescript_GetDotLength(self) > text_GetFence(TEXT(self)))
	textview_RuboutCmd((struct textview *)self);
}

struct typescript *typescript__Create(classID, arglist,diskf,filemenu)
struct classheader *classID;
char **arglist;
FILE *diskf;
boolean filemenu;
{
    struct typescript *self;
    struct typetext *tt;
    if(arglist == NULL) Pipescript = TRUE;
    else if( *arglist) myarglist = arglist;
    df = diskf;
    FileMenu = filemenu;
    tt = typetext_New();
    self = typescript_New();
    typescript_SetDataObject(self,(struct dataobject *)tt);
    return(self);
}

struct typescript *typescript__CreatePipescript(classID, indiskf, outdiskf, filemenu)
struct classheader *classID;
FILE *indiskf;
FILE *outdiskf;
boolean filemenu;
{
    struct typescript *self;

    self = typescript_Create(NULL, indiskf, filemenu);
    odf = outdiskf;

    return self;
}

static void
MaintainLastEnv (td)
struct typescript *td; {

    long len, spos;
    struct text *mydoc;
    struct environment *te;

    mydoc =  TEXT(td);
    spos = mark_GetPos(td->cmdStart);
    len = text_GetLength(mydoc);
    if (spos < len) {
	te = MyEnvinfo(mydoc, spos);
	if (te->data.style != staticBoldStyle){
	    te = environment_InsertStyle(mydoc->rootEnvironment, spos, staticBoldStyle, TRUE);
	}
        environment_SetStyle(te, FALSE , FALSE);
	environment_SetLength(te, len - spos);
   }

}
static void
TypescriptLeftCommand(tsv)
    register struct typescript *tsv; {
    register long pos;
    long start,end;
    pos = typescript_GetDotPosition(tsv);
    if( typescript_GetDotLength(tsv)== 0) {
	if(mark_GetPos(tsv->cmdStart) == pos) return;
	if(GetCommandEnv(tsv,pos,&start,&end)!= NULL){
	    if(start == pos) return;
	    typescript_SetDotPosition(tsv, start);
	    typescript_WantUpdate(tsv, tsv);
	    return;
	}
    }
    textview_BeginningOfLineCmd((struct textview *)tsv);
}
static void 
TypescriptEndOfLineCommand(tsv)
    register struct typescript *tsv; {
    register long pos;
    long start,end;
    pos = typescript_GetDotPosition(tsv);
    if( typescript_GetDotLength(tsv) == 0  && GetCommandEnv(tsv,pos,&start,&end)!= NULL){
	if(end== pos) return;
	typescript_SetDotPosition(tsv, end);
    	typescript_WantUpdate(tsv, tsv);
	}
    else 
	textview_EndOfLineCmd((struct textview *)tsv);
}
static void
TypescriptEOTCommand (tv)
struct typescript   *tv; {
    static struct timeval t = { 0, 0};
    int wfds = 1<<tv->SubChannel;
    if(tv->SubChannel < 0 ) return;
    select(32, 0, &wfds, 0, &t);
    if (!wfds) {
    	    message_DisplayString(tv, 0,"Process not ready for input");
	return;
    }	
    if (typescript_GetDotPosition(tv) >= text_GetLength(TEXT(tv)) ){
	write (tv->SubChannel, "", 0);
    }
    else{
	textview_DeleteCmd(tv);
    }
}

static void
TypescriptINTCommand (tv)
register struct typescript *tv; {
    SendSig (tv, SIGINT);
}
static void
TypescriptSTOPCommand (tv)
register struct typescript *tv; {
#if SY_AIX221
/* %%%%%%  must changed if AIX supports the STOP signal */
    SendSig (tv, 0);
#else /* if SY_AIX221 */
    SendSig (tv, SIGTSTP);
#endif /* if SY_AIX221 */
}
static void
TypescriptQUITCommand (tv)
register struct typescript *tv; {
    SendSig (tv, SIGQUIT);
}

static int
SendSig (tv, sig) register struct typescript *tv; {
    int pgrp = 0;
    if(tv->SubChannel < 0 ) return 0;
#if SY_AIX221
    pgrp = tv->pgrpid;	/* get saved process group */
#else
    ioctl (tv->SubChannel, TIOCGPGRP, &pgrp);
#endif
    if (pgrp == 0){
    	    message_DisplayString(tv, 0, "Can't send signal to subprocess");
	}
    else killpg (pgrp, sig);
}

static void TypescriptUnboundCommand(tv)
register struct typescript *tv; {
}

static void smashReadOnlyBuf(tv)
register struct typescript *tv; 
{   /* Clear out the buf when no longer needed since it probably contains a password */
    register char *c;
    register int i = READONLYMAX;
    for(c = tv->readOnlyBuf; i > 0; i-- )
	*c++ = 'X';
}


static void
TypescriptDoReturnCommand (tv,endpos)
register struct typescript *tv;
register long endpos;
{
    register struct text *d;
    int    maxpos, vfp, wfds;
    register   stpos, len;
    static struct timeval t = { 0, 0};
    if(tv->SubChannel < 0) {
	AnounceDeath(tv);
	return;
    }
    MaintainLastEnv(tv);
    wfds = 1 << tv->SubChannel;
    d = TEXT(tv);
    maxpos = text_GetLength(d);
    select(32, 0, &wfds, 0, &t);
    if (!wfds || tv->OutputWait) {
	if (!tv->OutputWait) {
	    /* add an output handler */
	    im_AddCanOutHandler(tv->SCFile, MyCanOutHandler, tv, 6);
	    tv->OutputWait = 1;
	}
	/* force newline addition to end of input during typeahead,
	 as it is forced in the normal case */
	text_InsertCharacters (d, maxpos, "\n", 1);
	mark_SetPos(tv->cmdStart,maxpos + 1);
	typescript_SetDotPosition(tv,maxpos + 1);

	return;
    }	

    /* compute end of stuff to send */
    if (maxpos > (vfp=mark_GetPos(tv->cmdStart))) {
	stpos = vfp;
	len = maxpos - stpos;
	if (len < 0)
	    len = 0;
    }
    else {
	stpos = maxpos;
	len = 0;
    }
    SetCmdSize(len + 1);

    if (tv->readOnlyLen > 0) {
        if (len > 0){
	    text_CopySubString(d, stpos, len , cmd, FALSE);
            write (tv->SubChannel, cmd, len);
	}
        tv->readOnlyBuf[tv->readOnlyLen] = '\n';
        write(tv->SubChannel, tv->readOnlyBuf, tv->readOnlyLen + 1);
        text_InsertCharacters (d, stpos + len++, "\n", 1);
	smashReadOnlyBuf(tv);
    }
    else {
        text_InsertCharacters (d, stpos + len++, "\n", 1);
	text_CopySubString(d, stpos, len, cmd, FALSE);
	if(len < 100)
            write (tv->SubChannel, cmd , len);
	else {
	    int tl;
	    char *cc;
	    for(cc = cmd,tl = len;tl >= 100; cc += 100, tl -= 100)
		write (tv->SubChannel, cc , 100);
	    if(tl > 0) 	write (tv->SubChannel, cc , tl);
	}
    }
    tv->readOnlyLen = -1;
    stpos += len;
    if(endpos < 0)
	typescript_SetDotPosition(tv, stpos);
    else typescript_SetDotPosition(tv, endpos);
    typescript_SetDotLength(tv, 0);
    tv->lastEnv = NULL;
    mark_SetPos(tv->cmdStart, stpos);
    mark_SetLength(tv->cmdStart, 0);
    text_SetFence(d, stpos);
    typescript_FrameDot(tv, stpos);
    text_NotifyObservers(d , 0);
    if (text_GetLength(d) > maxSize)
	typetext_AlwaysDeleteCharacters ((struct typetext *) d, 0, text_GetLength(d) - maxSize + extraRoom);
}
static void
TypescriptReturnCommand (tv)
register struct typescript *tv; {
    TypescriptDoReturnCommand (tv,-1);
}
static void TypescriptReturnAndPositionCommand(self, data)
    struct typescript *self;
    long data;
{
    TypescriptDoReturnCommand(self,text_GetFence(TEXT(self)));
    textview_LineToTop((struct textview *)self,data); 
} 

static 
typescript_HandleMenus(self,data)
struct typescript *self;
long data;
{
	char *s = (char *) data;
	while(*s){
	if(*s == '\n' || *s == '\r') 
		TypescriptReturnCommand(self);
	 else  Typescript_SelfInsertCmd(self,*s);
	  s++;
	}
}
static void
TypescriptZapCommand (tv)
    register struct typescript *tv;
{
    register struct text *d;
    int    maxpos;
    register   stpos;

    if (tv->readOnlyLen != -1) {
        tv->readOnlyLen = 0;
        return;
    }
    d = TEXT(tv);
    maxpos = text_GetLength(d);
    stpos = mark_GetPos(tv->cmdStart);
    if (maxpos > stpos) {
	text_DeleteCharacters(d, stpos, maxpos-stpos);
	typescript_SetDotPosition(tv, stpos);
	typescript_SetDotLength(tv, 0);
	tv->lastEnv = NULL;
    }
    text_NotifyObservers(d , 0); 
}
static void
GrabCommandHere (tv, where)
long where;
register struct typescript   *tv; {
    register int i;
    long start, size,len;
    struct environment *te;
    i = where;
    if (i<0) i=0;
    te = MyEnvinfo(TEXT(tv), i);
    if (te->data.style != staticBoldStyle) return;
    /* here we are in the command's bold region */
    start = environment_Eval(te);
    size = te->header.nestedmark.length;
    /* now compute the next place to start */
    SetCmdSize(size);
    /* now copy the bits out */
    text_CopySubString(TEXT(tv), start, size , cmd, FALSE);
    if((len = typescript_GetDotLength(tv)) > 0)
/* deletecharacters will check the fence for us */
	text_DeleteCharacters(TEXT(tv),typescript_GetDotPosition(tv),len);
    start = text_GetLength(TEXT(tv));
    text_InsertCharacters(TEXT(tv), start, cmd, size);
    typescript_SetDotPosition(tv, start);
    typescript_SetDotLength(tv, size);
    typescript_FrameDot(tv, start);
    text_NotifyObservers(TEXT(tv), 0);
}
static void
GrabLastCommand (tv)
    register struct typescript   *tv; {
    register int i;
    struct environment *te;
    struct text *sstxt = TEXT(tv);

    i = mark_GetPos(tv->cmdStart);
    /* i starts off somewhere to the right of the last command's bold region */
    for (;;)  {
	te = environment_GetPreviousChild(sstxt->rootEnvironment, tv->lastEnv, i);
	if (te != NULL)  {
	    tv->lastEnv = te;
	    if (te->header.nestedmark.length != 0)  {
		i = environment_Eval(te);
		GrabCommandHere(tv, i);
		return;
	    }
	}
	else
	    return;
    }
}

static void
GrabNextCommand (tv)
register struct typescript   *tv; {
    register long i;
    struct environment *te;
    struct text *sstxt = TEXT(tv);
    i = mark_GetPos(tv->cmdStart);
    /* i starts off somewhere to the left of the last command's bold region */
    for (;;)  {
	te = environment_GetNextChild(sstxt->rootEnvironment, tv->lastEnv, i);
	if (te != NULL )  {
	    i = environment_Eval(te);
	    if (i < mark_GetPos(tv->cmdStart))  {
		tv->lastEnv = te;
		if (te->header.nestedmark.length != 0)  {
		    GrabCommandHere(tv, i);
		    return;
		}
	    }
	    else
		return;
	}
	else
	    return;
    }
}

static void
GrabCurrentCommand(tv)
struct typescript *tv; {
    register int i;
    register struct text *d;
    struct environment *te;
    int  dlen;
    register long lineBegin, lineEnd;

    d = TEXT(tv);
    dlen = text_GetLength(d);
    if ((i=typescript_GetDotLength(tv)) == 0) {
	for(i = typescript_GetDotPosition(tv); i < dlen; i++) {
	    if (text_GetChar(d, i) == '\n') break;
	}
	lineEnd = i;
	i = typescript_GetDotPosition(tv);
	if (text_GetChar(d, i) == '\n') i--;	/* don't look at this end of line */
	for(;i>=0;i--) {
	    if (text_GetChar(d, i)=='\n') break;
	}
	lineBegin = i+1;

	for (i=lineBegin;i<lineEnd;i++) {
	    te = MyEnvinfo(d, i);
	    if (te->data.style == staticBoldStyle) {
		GrabCommandHere(tv, i); return;
	    }
	}
    }
    else {
	/* i still has the length */
	lineBegin = typescript_GetDotPosition(tv);
	lineEnd = lineBegin+i;
    }
    /* now send from lineBegin to lineEnd inclusive */
    SetCmdSize(lineEnd - lineBegin);
    text_CopySubString(d, lineBegin,lineEnd - lineBegin , cmd, FALSE);
    text_InsertCharacters(d, dlen, cmd, i = lineEnd-lineBegin);
    typescript_SetDotPosition(tv, dlen);
    typescript_SetDotLength(tv, i);
    typescript_FrameDot(tv, dlen);
    text_NotifyObservers(d, 0);
    typescript_PostMenus(tv , tv->menulist);
    /* MaintainLastEnv(tv); */
}

static void
ExecuteCurrentCommand(tv)
struct typescript *tv; {
    if(typescript_GetDotPosition(tv) < text_GetFence(TEXT(tv)))
	GrabCurrentCommand(tv);
    TypescriptReturnCommand (tv);
}
static void SetTitle(self,titleLine)
    struct typescript *self;
char *titleLine;
{

#define WMTITLELEN 40 /* Can you say "Magic hack?" */

    char titleBuffer[WMTITLELEN],*index();
    int len, maxLen = sizeof(titleBuffer) - 1;
    *titleBuffer = '\0';

    if (titleLine!= NULL){
	char *home=environ_Get("HOME");

	if(home!=NULL){
	    int hlen=strlen(home);
	    if(strncmp(titleLine,home,hlen)==0){
		strcpy(titleBuffer,"~");
		--maxLen;
		titleLine+=hlen;
	    }
	}		    
	len = strlen(titleLine);
	if (len > maxLen) {
	    char *partialName;

	    maxLen -= sizeof("---") - 1;
	    partialName = index(titleLine + (len - maxLen), '/');
	    if (partialName == NULL)
		partialName = titleLine + (len - maxLen);
	    else
		++partialName; /* Skip slash... */
	    strcpy(titleBuffer, "---");
	    strcat(titleBuffer, partialName);
	}
	else
	    strcat(titleBuffer, titleLine);
    }
  im_SetTitle(typescript_GetIM( self), titleBuffer); 
}
#define StartMagicChar 1 /* Ctrl A */
#define EndMagicChar 2  /* Ctrl B */
static char * ReadDirName(self,f,buf,bufsiz)
struct typescript *self;
FILE *f;
char *buf;
int *bufsiz;
{
	register char *cp;
	register c;
	register int i = *bufsiz;
	for(cp = buf;--i && f->_cnt>0; cp++){
	    if((c = getc(f))== EOF) {
		im_RemoveFileHandler(f);
		self->SubChannel = -1;
		AnounceDeath(self);
		break;
	    }
	    if((*cp = c) == '\n'){
		cp++;
		break;
	    }
	    if(*cp == EndMagicChar){
		*cp = '\0';
		if(im_ChangeDirectory(buf) == 0){
		    SetTitle(self,buf);
		}
		return(NULL);
	    }
	}
	*bufsiz = i;
	return(cp);
}

static void
ReadFromProcess (f, td)
    FILE *f;
    register struct typescript *td;
{

    char buf[4000];
    register char *bp = buf;
    register long dotpos, vfp;
    register  c = getc(f), i = 3999;
    int reframe = 0;
    char *input;
    int cpos;
    struct text *d = TEXT(td);

    mark_IncludeBeginning(td->cmdStart) = FALSE;
    dotpos = typescript_GetDotPosition(td);
    if (td->lastPosition == -1)
	td->lastPosition = dotpos;

     if (c!=EOF) {
	 if (odf != NULL)
	     putc(c, odf);

	if(c == StartMagicChar && !td->pipescript){
	    cpos = i;
	    if((input = ReadDirName(td,f,bp,&cpos)) != NULL){
		bp = input;
		i = cpos;
	    }
	}
	else  if(c != '\015') *bp++ = c;
        while (--i && f->_cnt>0) {
	    if((c = getc(f)) == EOF)  {
		break;
	    }
	    if (odf != NULL)  {
		putc(c, odf);
	    }
            *bp = c;
	    if(*bp == StartMagicChar && !td->pipescript){
		cpos = i;
		if ((input = ReadDirName(td,f,bp,&cpos)) != NULL){
		    bp = input;
		    i = cpos;
		    if(i == 0) break;
			 /* we could lose some output this way, but only if someone
			is cat-ing a binary or something else bogus.
			Even then , I doubt the reads will be this big. */
		}
	    }
	    else if(*bp != '\015') bp++;
        }
	if(bp != buf){
	    vfp = text_GetFence(d);
	    if(vfp <= dotpos){
		dotpos += bp - buf;
		reframe++;
	    }
	    text_InsertCharacters (d, vfp, buf, bp-buf);
	    text_SetFence(d, vfp = vfp+(bp-buf));
	    if (mark_GetPos(td->cmdStart) < vfp) {
		mark_SetPos(td->cmdStart, vfp);
		mark_SetLength(td->cmdStart, 0);
	    }
	    if (reframe){
		typescript_SetDotPosition(td,dotpos); 
		/* 	    typescript_FrameDot(td,dotpos);
		 */	}
	    text_NotifyObservers(d, 0);
	}
#if (!vax | vax2) /* everything but vax_11 */ /* this prevents vax from inserting chars in doc ! */
        if (td->readOnlyLen == -1) {
#if SY_AIX221
            struct termio ThisTerm;

            ioctl (fileno(f), TCGETA, &ThisTerm);
            if ( ! (ThisTerm.c_lflag & ECHO))  {
                td->readOnlyLen = 0;
	    }
#else /* #if SY_AIX221 */
            struct sgttyb sgttyb;

            ioctl (fileno(f), TIOCGETP, &sgttyb);
            if ( ! (sgttyb.sg_flags & ECHO))  {
                td->readOnlyLen = 0;
	    }
#endif /* #if SY_AIX221 */
	}
#endif /* (!vax | vax2)   */
     }
    if(c == EOF){
	if (odf != NULL)
	    fclose(odf);
        im_RemoveFileHandler(f);
	td->SubChannel = -1;
	AnounceDeath(td);
    }
    mark_IncludeBeginning(td->cmdStart) = TRUE;
}

static void
ClearTypescript (tv)
    struct typescript *tv;  {
        struct text *d = TEXT(tv);
        int p;

        for (p = mark_GetPos(tv->cmdStart); --p >= 0;)  {
            if (text_GetChar(d,p) == '\n')  {
                typetext_AlwaysDeleteCharacters((struct typetext *)d, 0, p + 1);
                text_NotifyObservers(d, 0);
                return;
            }
        }
    }
static void
NoEchoCommand(tv)
    struct typescript *tv;
{

    if (tv->readOnlyLen == -1)
        tv->readOnlyLen = 0;
}

boolean
typescript__InitializeObject (classID, tp)
struct classheader *classID;
struct typescript *tp;
     {
    int     pid;
    char  **arglist = NULL;
    int pgrp = getpgrp(0);
    int ptyChannel;
    int masterChannel;
#if SY_AIX221
    char ptyname[MAXPATHLEN];
#endif /* if SY_AIX221 */
#ifndef hpux
/* Disassociate this process from its controling tty. Must be done after opening the childs pty because that will become the controlling tty if we don't have one. */
    {
	int     fd = open ("/dev/tty", O_RDWR);
	if (fd >= 0) {
	    ioctl (fd, TIOCNOTTY, 0);
	    close (fd);
	}
#if SY_AIX221
	else {
	    ioctl(0, TIOCNOTTY,	0); /* might help when /dev/tty is hosed */
	    setpgrp();
	}
#else
        else
            setpgrp(0, 0);
#endif
    }
#endif /* hpux */

    tp->pipescript = FALSE;
    
    if(Pipescript) {
	tp->pipescript = TRUE;
	Pipescript = FALSE;
    }
    else if(myarglist != NULL) {
	arglist = myarglist;
	myarglist = NULL;
    }
   else {

        char *shell = environ_Get("SHELL");

        DefaultArgList[0] = (shell != NULL) ? shell : "/bin/csh";
        arglist = DefaultArgList;
    }
    tp->ChopLines = TRUE;
    tp->OutputWait = 0;
    typescript_SetBorder(tp, 5, 5);
    tp->readOnlyLen = -1;
    tp->lastPosition = -1;
#if SY_AIX221
    tp->pgrpid = 0;
#endif
    if(tp->pipescript){
	tp->SCFile = df;
	tp->SubChannel = -1;
	typescriptAddSearchMenu ();
	typescriptAddFileMenu();
	tp->keystate = keystate_Create(tp, ssmap);
	tp->menulist = menulist_DuplicateML(typescriptMenus, tp);
	maxSize = 100000;
	return TRUE;
    }
    
    tp->progname =  malloc (strlen(*arglist) + 1);
    strcpy(tp->progname,*arglist);
#if SY_AIX221
    if (! GetPtyandName(&masterChannel, &ptyChannel,ptyname,MAXPATHLEN))  {
#else /*if SY_AIX221 */
    if (! GetPty(&masterChannel, &ptyChannel))  {
#endif /*if SY_AIX221 */
	printf ("Can't connect subchannel\n");
	return FALSE;
    }

    tp->SubChannel = masterChannel;
#if SY_AIX221
    {
	int mode;

	mode = REMOTE;
	ioctl(tp->SubChannel, PTYSETM, &mode);
    }
#else /* #if SY_AIX221 */
    {
	int ON = 1;
	/* Convert TIOCREMOTE to 4.3 version, then try 4.2 version if
	   that doesn't work.  This is a HACK, and relies upon knowledge
	   of the encoding of ioctl's.  BUT it compiles correctly
	   with the ioctl.h from either 4.2 or 4.3.  What can you do?
	   Note that the ON flag is passed by reference in 4.3, by value in 4.2 */
#if defined(__STDC__) && !defined(__HIGHC__)
        if (ioctl (tp->SubChannel, _IOW('t', TIOCREMOTE&0xff, int), &ON) == -1)
            ioctl (tp->SubChannel, _IO('t', TIOCREMOTE&0xff), ON);
#else /* defined(__STDC__) && !defined(__HIGHC__) */
	if (ioctl (tp->SubChannel, _IOW(t, TIOCREMOTE&0xff, int), &ON) == -1)
	    ioctl (tp->SubChannel, _IO(t, TIOCREMOTE&0xff), ON);
#endif /* defined(__STDC__) && !defined(__HIGHC__) */

    ioctl (tp->SubChannel, FIOCLEX, 0);
    }
#endif /* #if SY_AIX221 */


    if ((pid = osi_vfork ()) < 0) {
	printf ("Fork failed\n");
	return FALSE;
    }
    if (pid == 0) {
	int     pid = getpid ();
#ifdef hpux
	setpgrp2(0, pid);
#else /* hpux */
#if SY_AIX221
	setpgrp();
	close(ptyChannel);
	/* reopen the terminal so it becomes the controlling terminal */
	if ((ptyChannel = open(ptyname, 2)) < 0) {
	    fprintf(stderr, "Could not open %s.\n", ptyname);
	    exit(1);
	}
#else /* if !SY_AIX221 */
	setpgrp (0, pid);
#endif /* if SY_AIX221 */
#endif /* hpux */
        dup2(ptyChannel, 0);
        dup2(ptyChannel, 1);
        dup2(ptyChannel, 2);

	/* Don't leave any unnecessary open file descriptors for child. */
	{
#ifndef hpux
	    extern int getdtablesize();
#endif /* hpux */
	    int numfds = getdtablesize();
	    int fd;

	    for (fd = 3; fd < numfds; fd++)
		close(fd);
	}
 
#if !SY_AIX221
	ioctl (0, TIOCSPGRP, &pid);
#endif /* if SY_AIX221 */
        environ_Put("TERM", "wm");
        environ_Delete("TERMCAP");
#if SY_AIX221
	{
	    struct termio ThisTerm;
	    ioctl (fileno(stdin), TCGETA, &ThisTerm);
	    ThisTerm.c_iflag &= ~ICRNL;
	    ThisTerm.c_oflag &=	~OPOST;	    /* turn all this stuff off */
	    ThisTerm.c_lflag |= ECHO;
	    ThisTerm.c_cflag |=	ISIG;	    /* enable signals */
	    ThisTerm.c_cflag &=	~ICANON;    /* turn off the editing */
	    ioctl (fileno(stdin), TCSETAW, &ThisTerm);
	}
#else /* #if SY_AIX221 */
	{	/* kernel doesn't reset pty's */
	    struct sgttyb sgttyb;
	    ioctl (0, TIOCGETP, &sgttyb);
	    sgttyb.sg_flags &= ~CRMOD;
#if (!vax | vax2) /* everything but vax_11 */
	    /*  causes echoing on the vax; needed on RT and sun 
		so the test in ReadFromProcess will work */
	    sgttyb.sg_flags |= ECHO;
#endif /* (!vax | vax2)  */
	    /* Run in raw mode to fix select problems. */
	    sgttyb.sg_flags |= RAW;
	    ioctl (0, TIOCSETP, &sgttyb);
	}
#endif /* #if SY_AIX221 */
	execvp (tp->progname, arglist);
	{   char buf[200];
	    sprintf(buf, "Couldn't exec %s\n", tp->progname);
	    write (1, buf, strlen(buf));
	    _exit (1);
	}
    }
#if SY_AIX221
    tp->pgrpid = pid;	/* save pid for sending quit and intr signals */
#else
#ifdef hpux
   setpgrp2(0, pgrp);
#else /* hpux */
    setpgrp(0, pgrp);
#endif /* hpux */
#endif /* #if !SY_AIX221 */

    tp->SCFile = fdopen(tp->SubChannel, "r");

    if(df){
	struct proctable_Entry *UserMenuProc;
	char nbuf[512];
        UserMenuProc = proctable_DefineProc("Read-User-Menus", typescript_HandleMenus, &typescript_classinfo, NULL, "Handle user supplied menus"); 
	   while (fgets (nbuf, sizeof nbuf, df)) {
		register pos = strlen(nbuf)-1;
		if (pos > 0)  {
		    if (nbuf[pos-1] == '\\')
		        nbuf[pos-1] = '\0';
		    else
		        nbuf[pos] = '\r';
		    typescriptAddMenu (nbuf,UserMenuProc);
		}
	    }
	fclose (df);
	df = NULL;
    }   
    typescriptAddtypescriptMenus();
    typescriptAddSearchMenu ();
    if(FileMenu){
	typescriptAddFileMenu();
	FileMenu = FALSE;
    }
    tp->menulist = menulist_DuplicateML(typescriptMenus, tp);
    tp->keystate = keystate_Create(tp, ssmap);
    return TRUE;
}

#define COBSIZE 100
/* Called when can send to pty.  Removes handler when no more data remains to be sent. */


static void MyCanOutHandler(afile, ad)
    FILE *afile;
    struct typescript *ad; {
    long start;
    register struct text *myd;
    register long i, end;
    register char *tp, tc;
    char buffer[COBSIZE];
    if(ad->SubChannel < 0 ) return;
    myd = TEXT(ad);
    start = text_GetFence(myd);
    end = text_GetLength(myd);
    if (start >= end) {
        if (ad->readOnlyLen > 0) {
            write(ad->SubChannel, ad->readOnlyBuf, ad->readOnlyLen);
	    smashReadOnlyBuf(ad);
            ad->readOnlyLen = -1;
        }
	ad->OutputWait = 0;
	im_RemoveCanOutHandler(ad->SCFile);
	return;
    }
    tp = buffer;
    for(i=start; i<end;) {
	tc = *tp++ = text_GetChar(myd, i);
	i++;		/* text_GetChar may become macro, so don't do this inside */
	if (tc == '\n') break;
	if (i >= start+COBSIZE-2) break;
    }
    text_SetFence(myd, i);
    write(ad->SubChannel, buffer, tp-buffer);
}

void
typescript__FinalizeObject(classID, ap)
struct classheader *classID;
struct typescript *ap; {
  /* dataobject_Destroy(TEXTOBJ(ap)); */ /* the doc will destroy it's own marks */
}

void typescript__ObservedChanged(ap,ov, value)
register struct typescript *ap;
struct observable *ov;
long value; {
    register long fencepos;

    /* after output has arrived, make sure view's dot is >= fence */
    if (ov == (struct observable *) TEXT(ap))  {
	if (value == observable_OBJECTDESTROYED)
	    /* the typescript can't do much without it's text */
	    typescript_Destroy(ap);
	else{
		MaintainLastEnv(ap);
     		fencepos = text_GetFence(TEXT(ap));
    		if ((fencepos <= typescript_GetDotPosition(ap)) && typescript_Visible(ap, ap->lastPosition) ) {
		    typescript_FrameDot(ap, fencepos);
   		 }
   		 typescript_WantUpdate(ap, ap);
	}
    }
}

void typescript__PostMenus(self, menulist)
    struct typescript *self;
    struct menulist *menulist;
{
	/* Ignore the textviews menus,
	but take advantage of the fact that it knows when to
	post (and retract) the selection menus */
    if (self->header.textview.hasInputFocus) {

        long mask;
        boolean readonly = typescript_GetDotPosition(self) < text_GetFence(TEXT(self));

        mask = ((typescript_GetDotLength(self) > 0) ? typescript_SelectionMenus : typescript_NoSelectionMenus) |
               (readonly ? 0 : typescript_AfterFenceMenus);

        if (menulist_SetMask(self->menulist, mask)) {
            super_PostMenus(self, self->menulist);
        }
    }
}

void typescript__ReceiveInputFocus(me)
    register struct typescript *me; {
    super_ReceiveInputFocus(me);
    me->keystate->next = NULL;
    me->header.textview.keystate->next = NULL; 
    keystate_AddBefore(me->keystate, me->header.textview.keystate); 
    typescript_PostKeyState(me , me->keystate);
    menulist_SetMask(me->menulist, textview_NoMenus);
}
static void 
typescript_handlereadonly(self ,c)
register struct typescript *self;
char c;
{   /* This will put characters in the read-only buffer without displaying them.
      Deals with the no-echo mode for entering passwords and the like. */
    if (isprint(c)) {
	if (self->readOnlyLen < (READONLYMAX - 1)) { /* Make sure to leave space for ending carriage return. */
	    self->readOnlyBuf[self->readOnlyLen++] = c;
	}
    }
}
static PositionDot(self)
register struct typescript *self;
{    
    register long dotpos,markpos;
    struct text *d = TEXT(self);

    if((dotpos = typescript_GetDotPosition(self)) < (markpos = text_GetFence(d))){
	while(dotpos < markpos) {
	    if (text_GetChar(d,dotpos) == '\n'){
		textview_EndOfTextCmd(self);
		return;
	    }
	    dotpos++;
	}
	typescript_SetDotPosition(self,dotpos); 
    }
    if(dotpos == markpos){
/*	modify environment style so environment doesn't get fragmented */
        struct environment *te =  MyEnvinfo(TEXT(self), dotpos);
        if (te->data.style == staticBoldStyle)
            environment_SetStyle(te, TRUE , FALSE);
    }
}
static void 
typescript_YankCmd (self)
register struct typescript *self;
{    
    PositionDot(self);
    textview_YankCmd(self);
}
static void Typescript_SelfInsertCmd(self, a)
    register struct typescript *self;
    register char a;
{
    PositionDot(self);

    if ((self->readOnlyLen != -1)){
	typescript_handlereadonly(self,a);
    }
    else {
   	textview_SelfInsertCmd(self, a);
    }
}
static void Typescript_DigitCmd(self, a)
    register struct typescript *self;
    char a;
{
    PositionDot(self);
 
    if ((self->readOnlyLen != -1)){
	typescript_handlereadonly(self,a);
    }
    else {
   	textview_DigitCmd(self, a);
    }
}

static 
typescript_BackwardsRotatePasteCmd(self)
    struct typescript *self;
{
    if(typescript_GetDotPosition(self) < text_GetFence(TEXT(self)))
	return;
    textview_BackwardsRotatePasteCmd(self);
}
static 
typescript_RotatePasteCmd(self)
    struct typescript *self;
{
    if(typescript_GetDotPosition(self) < text_GetFence(TEXT(self)))
	return;
    textview_RotatePasteCmd(self);
}

/* What to do when the textview hasn't defined something... */
int typescript_NoTextviewKey(self, key)
    struct typescript *self;
    long key;
{
    message_DisplayString(self, 0, "Could not execute command. Failure in looking up textview command.");
    return 0;
}

static typescriptAddtypescriptMenus()
{
    struct proctable_Entry *tempProc;
    struct classinfo *classInfo = &typescript_classinfo;

    tempProc = proctable_DefineProc("typescript-yank", typescript_YankCmd, classInfo, NULL, "Yank text back from kill-buffer.");
    keymap_BindToKey(ssmap, "\031", tempProc, 0);
    menulist_AddToML(typescriptMenus,"Paste~11",tempProc,NULL,typescript_AfterFenceMenus | typescript_NoSelectionMenus);

    tempProc=proctable_DefineProc("typescript-rotate-backward-paste", typescript_BackwardsRotatePasteCmd, classInfo, NULL, "Rotate kill-buffer backwards.");
    keymap_BindToKey(ssmap, "\033\031", tempProc, 0);

    tempProc = proctable_DefineProc("typescript-rotate-paste", typescript_RotatePasteCmd, classInfo, NULL, "Rotate kill-buffer.");
    keymap_BindToKey(ssmap, "\033y", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-Grab-Last-Cmd", GrabLastCommand, classInfo, NULL, "Grab Last Command");
    keymap_BindToKey(ssmap, "\033=", tempProc, 0);

    tempProc = proctable_DefineProc("typescript-Grab-Current-Cmd", GrabCurrentCommand, classInfo, NULL, "Grab Current Command");
    keymap_BindToKey(ssmap, "\033+", tempProc, 0);
    menulist_AddToML(typescriptMenus,"Move~30",tempProc,NULL,0);

    tempProc = proctable_DefineProc("typescript-Execute-Current-Cmd", ExecuteCurrentCommand, classInfo, NULL, "Execute Current Command");
    keymap_BindToKey(ssmap, "\033\015", tempProc, 0);
    menulist_AddToML(typescriptMenus,"Execute~31",tempProc,NULL,0);

    tempProc=proctable_DefineProc("typescript-Grab-Next-Cmd", GrabNextCommand, classInfo, NULL, "Grab Next Command");
    keymap_BindToKey(ssmap, "\033`", tempProc, 0);
}

boolean typescript__InitializeClass(classID)
    struct classheader *classID;
{
    struct proctable_Entry *tempProc, *si, *dig;
    struct classinfo *classInfo = &typescript_classinfo;
    register long i;
    char str[2];
    maxSize = environ_GetProfileInt("maxsize", 10000);
    extraRoom = maxSize / 10;

    CmdSize = 256;
    cmd = (char *)malloc(CmdSize);
    ssmap = keymap_New();
    typescriptMenus = menulist_New();

/* Initialize our pointers to textview command routines. */
    class_Load("textview"); /* Make sure the textview is loaded first. */
    if ((tempProc = proctable_Lookup("textview-line-to-top")) != NULL) {
        proctable_ForceLoaded(tempProc);
        textview_LineToTop = proctable_GetFunction(tempProc);
    }
    else
        textview_LineToTop = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-end-of-text")) != NULL) {
        textview_EndOfTextCmd = proctable_GetFunction(tempProc);
    }
    else
        textview_EndOfTextCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-self-insert")) != NULL)
        textview_SelfInsertCmd = proctable_GetFunction(tempProc);
    else
        textview_SelfInsertCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-digit")) != NULL)
        textview_DigitCmd = proctable_GetFunction(tempProc);
    else
        textview_DigitCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-beginning-of-line")) != NULL)
        textview_BeginningOfLineCmd = proctable_GetFunction(tempProc);
    else
        textview_BeginningOfLineCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-end-of-line")) != NULL)
        textview_EndOfLineCmd = proctable_GetFunction(tempProc);
    else
        textview_EndOfLineCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-yank")) != NULL)
        textview_YankCmd = proctable_GetFunction(tempProc);
    else
        textview_YankCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-rotate-backward-paste")) != NULL)
        textview_BackwardsRotatePasteCmd = proctable_GetFunction(tempProc);
    else
        textview_BackwardsRotatePasteCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-rotate-paste")) != NULL)
        textview_RotatePasteCmd = proctable_GetFunction(tempProc);
    else
        textview_RotatePasteCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-delete-next-character")) != NULL)
        textview_DeleteCmd = proctable_GetFunction(tempProc);
    else
        textview_DeleteCmd = typescript_NoTextviewKey;

    if ((tempProc = proctable_Lookup("textview-copy-region")) != NULL)
        typescript_CopyRegionCmd = proctable_GetFunction(tempProc);
    else
        typescript_CopyRegionCmd = typescript_NoTextviewKey;
    if ((tempProc = proctable_Lookup("textview-zap-region")) != NULL)
        typescript_ZapRegionCmd = proctable_GetFunction(tempProc);
    else
        typescript_ZapRegionCmd = typescript_NoTextviewKey;


    if ((tempProc = proctable_Lookup("textview-delete-previous-character")) != NULL)
        textview_RuboutCmd = proctable_GetFunction(tempProc);
    else
        textview_RuboutCmd = typescript_NoTextviewKey;
    tempProc=proctable_DefineProc("typescript-interupt-command", TypescriptINTCommand, classInfo, NULL, "Handle ^C");
    keymap_BindToKey(ssmap, "\003", tempProc, 0);
#if SY_AIX221
    keymap_BindToKey(ssmap, "\177", tempProc, 0);   /* bind DEL too! */
#endif /* #if SY_AIX221 */
    tempProc=proctable_DefineProc("typescript-beginning-of-line", TypescriptLeftCommand, classInfo, NULL, "Move to line beginning");
    keymap_BindToKey(ssmap, "\001", tempProc, 0);

    tempProc=proctable_DefineProc("typescript-end-of-line", TypescriptEndOfLineCommand, classInfo, NULL, "Move to line end");
    keymap_BindToKey(ssmap, "\005", tempProc, 0);

    tempProc=proctable_DefineProc("typescript-EOT-command", TypescriptEOTCommand, classInfo, NULL, "Handle ^D");
    keymap_BindToKey(ssmap, "\004", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-return-cmd", TypescriptReturnCommand, classInfo, NULL, "Handle enter key");
    keymap_BindToKey(ssmap, "\015", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-return-and-position", TypescriptReturnAndPositionCommand, classInfo, NULL, "Handle enter key and place command at top of the screen");
    keymap_BindToKey(ssmap, "\012", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-ZAP-command", TypescriptZapCommand, classInfo, NULL, "handles ^U line clearing");
    keymap_BindToKey(ssmap, "\025", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-Stop-cmd", TypescriptSTOPCommand, classInfo, NULL, "Handle ^Z Stop cmd");
    keymap_BindToKey(ssmap, "\032", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-unbound", TypescriptUnboundCommand, classInfo, NULL, "Does nothing.");
    keymap_BindToKey(ssmap, "\033~", tempProc, 0);

    tempProc = proctable_DefineProc("typescript-zap-region", typescript_ZapRegionCmd, classInfo, NULL, "Cut region to kill-buffer."); 
    keymap_BindToKey(ssmap, "\027", tempProc, 0);
    menulist_AddToML(typescriptMenus,"Cut~11",tempProc,NULL,typescript_AfterFenceMenus | typescript_SelectionMenus);

    tempProc = proctable_DefineProc("typescript-copy-region", typescript_CopyRegionCmd, classInfo, NULL, "Copy region to kill-buffer.");
    keymap_BindToKey(ssmap, "\033w", tempProc, 0);
    menulist_AddToML(typescriptMenus,"Copy~12",tempProc,NULL,typescript_SelectionMenus);

    tempProc=proctable_DefineProc("typescript-QUIT-command", TypescriptQUITCommand, classInfo, NULL, "handles quit cmd");
    keymap_BindToKey(ssmap, "\034", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-No-Echo-Cmd", NoEchoCommand, classInfo, NULL, "Turn Off Echoing");
    keymap_BindToKey(ssmap, "\030\014", tempProc, 0);
    tempProc=proctable_DefineProc("typescript-delete-previous-character", typescript_RuboutCmd, classInfo, NULL, "Delete the previous character.");
    keymap_BindToKey(ssmap, "\010", tempProc, 0); 
    keymap_BindToKey(ssmap, "\177", tempProc, 0);
 

    si=proctable_DefineProc("Typescript-self-insert", Typescript_SelfInsertCmd, classInfo, NULL, "Insert a character.");
    dig=proctable_DefineProc("Typescript-digit", Typescript_DigitCmd, classInfo, NULL, "Insert a character.");
    str[0] = ' ';
    str[1] = '\0';
    for (i = 32; i < 127; i++)  {
	if (i < 48 || i > 57)
	    keymap_BindToKey(ssmap, str, si, i);
	else
	    keymap_BindToKey(ssmap, str, dig, i);
	str[0]++;
    }

    tempProc = proctable_DefineProc("typescript-Clear", ClearTypescript, classInfo, NULL, "Clear Typescript");
    menulist_AddToML(typescriptMenus,"Clear~51",tempProc,NULL,0);
    return TRUE;
}

void typescript__Update(self)
    struct typescript *self;
{
    self->lastPosition = -1;
    super_Update(self);
}

void typescript__FullUpdate(self, type, left, top, width, height)
    struct typescript *self;
    enum view_UpdateType type;
    long left;
    long top;
    long width;
    long height;
{
    self->lastPosition = -1;
    super_FullUpdate(self, type, left, top, width, height);
}
void typescript__GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos)
    struct typescript *self;
    long position;
    long numberOfClicks;
    enum view_MouseAction action;
    long startLeft;
    long startRight;
    long *leftPos;
    long *rightPos;
{
	if(numberOfClicks%3 ) {
	    super_GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos);
	    return;
	}
	if(GetCommandEnv(self,position,leftPos,rightPos)== NULL)
	    super_GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos);
}

void typescript__SetDataObject(tp,obj)
struct typescript *tp;
struct dataobject *obj;
{
    struct style *defaultStyle;
    char bodyFont[100];
    long fontSize = 10;
    long fontStyle = fontdesc_Fixed;
    char *font;
    long stpos;
    struct typetext *shtext = (struct typetext *)obj;
    struct text *SsText = (struct text *) shtext;
    super_SetDataObject(tp,obj);
    if (!staticBoldStyle) {
	if ((staticBoldStyle = stylesheet_Find(SsText->styleSheet, "bold")) == NULL){
		staticBoldStyle = style_New();
		style_SetName(staticBoldStyle, "bold");
		stylesheet_Add(SsText->styleSheet, staticBoldStyle);
	}
	style_AddNewFontFace(staticBoldStyle, fontdesc_Bold);
    }

    if ((defaultStyle = typescript_GetDefaultStyle(tp)) == NULL)  {
	    defaultStyle = style_New();
	    style_SetName(defaultStyle, "default");
    }
    if ((font = environ_GetProfile("bodyfont")) == NULL || ! fontdesc_ExplodeFontName(font, bodyFont, sizeof(bodyFont), &fontStyle, &fontSize)) {
	strcpy(bodyFont, "AndyType");
	fontSize = 10;
	fontStyle = fontdesc_Fixed;
    }
    style_SetFontFamily(defaultStyle, bodyFont);
    style_SetFontSize(defaultStyle, style_ConstantFontSize, fontSize);
    style_ClearNewFontFaces(defaultStyle);
    style_AddNewFontFace(defaultStyle, fontStyle);
    style_SetJustification(defaultStyle, style_LeftJustified);
    style_SetNewLeftMargin(defaultStyle,style_LeftMargin,16384, style_Inches);
    style_SetNewIndentation(defaultStyle,style_LeftEdge,-16384, style_Inches);
    typescript_SetDefaultStyle(tp, defaultStyle);
    stpos = text_GetLength(SsText);
    tp->cmdStart = text_CreateMark(SsText,stpos,0);
    text_SetFence(SsText, stpos);
    mark_IncludeBeginning(tp->cmdStart) = TRUE;
    if(!shtext->hashandler){
	im_AddFileHandler (tp->SCFile, ReadFromProcess, tp, 6);
	shtext->hashandler = TRUE;
    }
    typescript_SetDotPosition(tp,stpos);
    im_ForceUpdate();
}
