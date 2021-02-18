/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/textaux/RCS/contentv.c,v 1.12 89/10/26 17:28:52 tpn Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/textaux/RCS/contentv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/textaux/RCS/contentv.c,v 1.12 89/10/26 17:28:52 tpn Exp $";
#endif /* lint */

#include "class.h"
#include "ctype.h"
#include "bind.ih"
#include "view.ih"
#include "menulist.ih"
#include "keymap.ih"
#include "content.ih"
#include "message.ih"
#include "mark.ih"
#include <buffer.ih>
#include <im.ih>
#include <frame.ih>
#include <environ.ih>
#include <print.ih>
#include <complete.ih>
#include <textv.ih>
#include <text.ih>
#include <style.ih>
#include <stylesht.ih>
#include <fontdesc.ih>
#include <proctbl.ih>
#include <tindex.ih>

#include "contentv.eh"
struct menulist *contentvMenus;
static struct keymap *contentvKeyMap;
#define Data(self) ((struct content *)(((struct view *) self)->dataobject))
#define Text(v)	(struct text *) ((v)->header.view.dataobject)
#define Srctext(self) (Data(self)->srctext)
boolean contentv__InitializeObject(classID,self)
struct classheader *classID;
struct contentv *self;
{
    self->menus = menulist_DuplicateML(contentvMenus, self);
    self->srcview = NULL;
    return TRUE;
}
boolean contentv__FinializeObject(classID,self)
struct classheader *classID;
struct contentv *self;
{
     if(self->menus){
	menulist_Destroy(self->menus);
	self->menus = NULL;
    }
    return TRUE;
}
static void reinit(self,value)
struct contentv *self;
long value;
{
    struct content *ct;
    long  pos = contentv_GetDotPosition(self);

    ct = Data(self);
    content_reinit(ct);
    contentv_SetDotPosition(self,pos);
    contentv_FrameDot(self,pos);
}
static void enumerate(self,value)
struct contentv *self;
long value;
{
    struct content *ct;
    long len,tlen;
    ct = Data(self);
    if((len = contentv_GetDotLength(self)) > 0){
	char buf[128],iname[128];
	long stlen;
	long pos = contentv_GetDotPosition(self);
	tlen = content_GetLength(ct);
	len--;
	while((tlen > pos + len ) && (content_GetChar(ct,pos + len) != '\n')) len++ ;
	len++;
	if((stlen = content_StringToInts(ct,pos,NULL)) > 0 && stlen < 127)
	    content_CopySubString(ct,pos,stlen - 1,buf,FALSE);
	else {
	    buf[0] = '0';
	    buf[1] = '\0';
	}
	stlen = message_AskForString(self,60,"Starting number ",buf, iname, sizeof(iname));
	if (stlen < 0){
	    message_DisplayString(self, 0, "Punt!");
	    return;
	}
	if(strlen(iname) == 0)content_Enumerate(ct,pos,len,NULL);
	else {
	    strcat(iname,"\t");
	    content_Enumerate(ct,pos,len,iname);
	}
    }
    else {
	content_Enumerate(ct,-1,0,NULL);
    }
}
static int LocateInView(v1,v2,v3,dat)
struct view *v1,*v2,*v3;
long dat;
{
    struct mark *m = (struct mark *) dat;
    if(class_IsTypeByName(class_GetTypeName(v2),"textview")){
	struct textview *tv = (struct textview *)v2;
	textview_SetDotPosition(tv,mark_GetPos(m));
	textview_SetDotLength(tv,mark_GetLength(m));
	textview_FrameDot(tv,mark_GetPos(m));
    }
    return 0; /* go through all views */
}
static void locate(self,value)
struct contentv *self;
long value;
{
    struct content *ct;
    struct mark *loc;
    struct buffer *buf;
    ct = Data(self);
    loc = content_locate(ct,contentv_GetDotPosition(self));
    if(loc == NULL) return;

    if(self->srcview)
	LocateInView(NULL,(struct view *)self->srcview,NULL,(long) loc);
    else {
	buf = buffer_FindBufferByData((struct dataobject *)ct->srctext);
	if(buf)
	    buffer_EnumerateViews(buf,LocateInView,(long) loc);
    }
}
static void denumerate(self,value)
struct contentv *self;
long value;
{
    struct content *ct;
    long len,pos,tlen;
    ct = Data(self);
    if((len = contentv_GetDotLength(self)) > 0){
	pos = contentv_GetDotPosition(self);
	tlen = content_GetLength(ct);
	len--;
	while((tlen > pos + len ) && (content_GetChar(ct,pos + len) != '\n')) len++ ;
	len++;
	content_Denumerate(ct,pos,len);
    }
    else content_Denumerate(ct,-1,0);
}
struct contentv_cntr {
struct buffer *buf;
int tc,bc;
};
static int check(fr, rock)
    struct frame *fr;
    struct contentv_cntr *rock;
{
     rock->tc++;
     if(frame_GetBuffer(fr) == rock->buf) rock->bc++;
     return FALSE;
}
struct frame *getframe(vw)
struct view *vw;
{
    while (vw->parent != NULL){
	vw = vw->parent;
	if(class_IsTypeByName(class_GetTypeName(vw),"frame")){
	    return (struct frame *) vw;
	}
    }
    return NULL;
}
void destroy(self,value)
struct contentv *self;
long value;
{
    struct buffer *buffer;
    struct frame *fr;
    struct proctable_Entry *pr;
    int (*proc)();
    struct contentv_cntr cc;
    if((pr = proctable_Lookup("frame-delete-window")) != NULL && 
	proctable_Defined(pr) &&
	(buffer = buffer_FindBufferByData(Data(self))) != NULL &&
	(fr = getframe((struct view *) self)) != NULL){
	proc = proctable_GetFunction(pr) ;
	cc.tc = cc.bc = 0;
	cc.buf = buffer;
	frame_Enumerate(check, &cc);
	(*proc)(fr,0);
	if(cc.tc > 1 &&  cc.bc == 1) {
	    buffer_Destroy(buffer);
	    content_Destroy(Data(self)); 
	}
    }
}
static void contentv_MakeContents(self)
    register struct textview *self;
{
    contentv_MakeWindow(Text(self));
}
void contentv__GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos)
    struct contentv *self;
    long position;
    long numberOfClicks;
    enum view_MouseAction action;
    long startLeft;
    long startRight;
    long *leftPos;
    long *rightPos;
    {
	super_GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos);
	if(numberOfClicks == 1 && (action == view_LeftUp || action == view_RightUp))
	    locate(self,0);
    }
static boolean findframe(fr,buf)
struct frame *fr;
struct buffer *buf;
{
    if(frame_GetBuffer(fr) == buf) return TRUE;
    return FALSE;
}
static void contentv_doprint(self,type)
register struct contentv *self;
char *type;
{
    struct buffer *bu;
    struct frame *fr;
    struct text *txt;
    struct content *ct;
    struct proctable_Entry *pr;
    int (*proc)();

    ct = Data(self);
    txt = ct->srctext;
    if(txt == NULL || (bu = buffer_FindBufferByData((struct dataobject *) txt))== NULL) {
	message_DisplayString(self,0,"Can't find buffer for source");
	return;
    }
    if((fr = frame_Enumerate(findframe,(long) bu)) == NULL){
	message_DisplayString(self,0,"Can't find view for source");
	return;
    }
    if((pr = proctable_Lookup(type)) != NULL && proctable_Defined(pr) ){
	proc = proctable_GetFunction(pr) ;
	environ_Put("PrintContents","");
	(*proc)(fr,0);
	environ_Delete("PrintContents");
    }
    else {
	message_DisplayString(self,0,"Can't find proctable entry");
	return;
    }
}
void contentv_PreviewCmd(self)
    struct contentv *self;
{
    contentv_doprint(self,"frame-preview");	
}
void contentv_PrintCmd(self)
    struct contentv *self;
{
    contentv_doprint(self,"frame-print");
}

static struct bind_Description contentvBindings[]={
    {"contentv-reinit",NULL,0,"Contents~1,Update Contents~20", 0,0,reinit,"reinitialize the headings"},
    {"contentv-enumerate",NULL,0,"Contents~1,Enumerate~1",0,0,enumerate,"Enumerate the heading"},
    {"contentv-denumerate",NULL,0,"Contents~1,Denumerate~2",0,0,denumerate,"Denumerate the heading"},
    {"contentv-destroy",NULL,0,"Quit~99",0,0,destroy,"destroy the table of content"}, 
    {"contentv-delete-window",NULL,0,"Delete Window~89",0,0,destroy,"destroy the table of content"},
    {"tindex-print",NULL,0,"Index~2,Print Index~21",0,0,NULL,"print index","tindex"}, 
    {"tindex-preview",NULL,0,"Index~2,Preview Index~20",0,0,NULL,"preview index","tindex"}, 
    {"tindex-index-term",NULL,0,"Index~2,Index term~1",0,0,NULL,"index all instances of a term","tindex"}, 
    {"tindex-read-index-file",NULL,0,"Index~2,Read list~10",0,0,NULL,"read a list of index terms ","tindex"}, 
    {"tindex-write-index-file",NULL,0,"Index~2,Write list~11",0,0,NULL,"write a list of index terms ","tindex"}, 
    {"tindex-index-italic",NULL,0,"Index~2,Italic Index~30",0,0,NULL,"make the index entries italic","tindex"},
    {"tindex-index-plain",NULL,0,"Index~2,Plain Index~31",0,0,NULL,"make the index entries plain","tindex"},
    {"tindex-expose-inv-index",NULL,0,"Index~2,Expose Inv. Index~32",0,0,NULL,"Expose the invisible index entries","tindex"},
    {"tindex-hide-inv-index",NULL,0,"Index~2,Hide Inv. Index~33",0,0,NULL,"Hide the invisible index entries","tindex"},
    {"contentv-preview",		    NULL,0,	    "File~10,Preview~21",0,0, contentv_PreviewCmd, "Previews document."},
    {"contentv-print",		    NULL,0,	    "File~10,Print~22",0,0, contentv_PrintCmd, "Prints document."},
    NULL
};
void contentv__PostMenus(self, menulist)
struct contentv *self;
struct menulist *menulist;
{
    menulist_ClearChain(self->menus);
    menulist_ChainBeforeML(self->menus, menulist, menulist);
    super_PostMenus(self, self->menus);
}
boolean contentv__InitializeClass(classID)
    struct classheader *classID;
{
    struct classinfo *textviewtype = class_Load("textview");
    contentvMenus = menulist_New();
    contentvKeyMap =  keymap_New();
    bind_BindList(contentvBindings, contentvKeyMap , contentvMenus, &contentv_classinfo);

    proctable_DefineProc("contentv-make-window",contentv_MakeContents,textviewtype,NULL,"Make a table of contents window");

    return TRUE;
}
static void contentv__MakeWindow(classID,txt)
struct classinfo *classID;
struct text *txt;
{
    char buf[1024];
    struct content *ct;
    struct buffer *buffer;
    struct frame *fr;
    struct im *window;
    if((buffer = buffer_FindBufferByData((struct dataobject *)txt)) != NULL){
	sprintf(buf,"Contents_%s",buffer_GetName(buffer));
    }
    else sprintf(buf,"Table_of_Contents");
    ct = content_New();
    content_SetSourceText(ct,txt);
    buffer  = buffer_Create(buf,NULL,"contentv",ct);
    if(buffer == NULL) return ;
    if ((window = im_Create(NULL)) != NULL) {
	fr = frame_New();
	frame_SetCommandEnable(fr, TRUE);
	im_SetView(window, fr);
	frame_PostDefaultHandler(fr, "message", frame_WantHandler(fr, "message"));
	frame_SetBuffer(fr, buffer, TRUE);
	buffer_SetScratch(buffer,TRUE);
    }
}
