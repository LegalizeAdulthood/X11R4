/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: frame.c,v 1.2 89/11/30 12:11:19 xguest Exp $ */
/* $ACIS:frame.c 1.6$ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/frame/RCS/frame.c,v $ */

#ifndef lint
static char *rcsid = "$Header: frame.c,v 1.2 89/11/30 12:11:19 xguest Exp $";
#endif /* lint */

/* frame.c
 * Provides the toplevel view on a buffer and message line.
  */

#ifndef MAX
#define MAX(A,B) ((A > B) ? A: B)
#endif /* MAX */
#include <andrewos.h>
#include <class.h>
#include <im.ih>

#include <view.ih>
#include <dataobj.ih>
#include <buffer.ih>
#include <style.ih>
#include <environ.ih>
#include <fontdesc.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <framev.ih>
#include <framecmd.ih>
#include <framemsg.ih>
#include <cursor.ih>
#include <graphic.ih>
#include <frame.eh>
#include <proctbl.ih>
#include <message.ih>
#include <ctype.h>
extern char *index();

static struct frame *allFrames = NULL; /* Maintains a list of all frames for enumeration purposes. */
static struct keymap *frameKeymap = NULL;
static struct menulist *frameMenus = NULL;

#define DEFAULTHEIGHT 20

#define ACCEPT 1
#define CANCEL 2
#define COMPLETE 3
#define HELP 4

struct pendingupdates {
    struct view *v;
    struct pendingupdates *next;
};

#define frame_FONTSIZE 12
#define frame_FONTPAD 10 /* bogus */
#define OFFSET 5
#define BUTTONON 1
#define BUTTONOFF 3

#define frame_SEPARATION 2
#define frame_TWOSEPARATIONS (2*frame_SEPARATION) /* 4 */
#define frame_HALFPADDING (frame_TWOSEPARATIONS+1) /* 5 */
#define frame_TOTALPADDING ((2*frame_HALFPADDING)+2) /* 10 */
#define frame_DOUBLEPADDING (2*frame_TOTALPADDING) /* 20 */
#define MINSIDEBYSIDEWIDTH 400
static int frame_GlobalStackCount = 0; /* a hack to prevent ugly recursion */
static struct keymap *mykm;
static struct proctable_Entry *returnconsidered, *cancel, *confirmAnswer, *gotkey;


static int CalculateLineHeight(self)
    struct frame *self;
{

    struct style *defaultStyle;
    struct fontdesc *defaultFont;
    struct FontSummary *fontSummary;
    char fontFamily[256];
    long refBasis, refOperand, fontSize;

    if ((defaultStyle = frameview_GetDefaultStyle(self->messageView)) == NULL)
        return DEFAULTHEIGHT;
    style_GetFontFamily(defaultStyle, fontFamily, sizeof (fontFamily));
    style_GetFontSize(defaultStyle, &refBasis, &fontSize);
    refOperand = style_GetAddedFontFaces(defaultStyle);
    defaultFont = fontdesc_Create(fontFamily, refOperand, fontSize);
    if ((fontSummary = fontdesc_FontSummary(defaultFont, frame_GetDrawable(self))) == NULL)
        return DEFAULTHEIGHT;
    return fontSummary->maxHeight + 4; /* Two for top and bottom border. */
}

boolean frame__InitializeObject(classID, self)
    struct classheader *classID;
    struct frame *self;
{

    self->buffer = NULL;
    self->targetView = NULL;
    self->childView = NULL;
    self->messageLine = framemessage_Create(self);
    self->messageView = self->messageLine->messageView;
    frame_SetNth(self, 1, self->messageView);
    self->dialogLine = framemessage_Create(self);
    self->dialogView = self->dialogLine->messageView;
    framemessage_SetCompanion(self->messageLine,(struct msghandler *)self->dialogLine);
    self->title = NULL;
    self->keystate = NULL;
    self->menulist = NULL;
    self->helpBuffer[0] = '\0';
    self->commandEnable = FALSE;
    self->lineHeight = 0;
    self->next = allFrames;
    allFrames = self;

    self->AwaitingFocus = 0;
    self->DialogBuffer = NULL;
    self->DialogBufferView = NULL;
    self->DialogTargetView = NULL;
 
    /* Create menulist/keystate */
    self->octcursor = cursor_Create((struct view *) self);
    self->arrowcursor = cursor_Create((struct view *) self);
    cursor_SetStandard(self->octcursor, Cursor_Octagon);
    cursor_SetStandard(self->arrowcursor, Cursor_Arrow);
    self->mymenus = menulist_Create(self);
    self->myfontdesc = fontdesc_Create("andysans", fontdesc_Bold, frame_FONTSIZE);
    self->mykeystate = keystate_Create(self, mykm);
    self->HeightsOfAnswer = NULL;
    self->uplist = NULL;
    self->mesrec.height = 0;
    self->UpdateRequested = FALSE;
    TidyUp(self);
    self->object = NULL;
    self->dataModified = FALSE;
    return TRUE;
}

void frame__FinalizeObject(classID, self)
    struct classheader *classID;
    struct frame *self;
{

    struct frame *traverse, **previous;

#if 0
    if (self->buffer != NULL) {
        if (self->childView != NULL) {
	    frame_SetNth(self, 0, NULL); /* Remove the view from the lpair so we can deallocate it. */
            buffer_RemoveView(self->buffer, self->childView);
	}
        buffer_RemoveObserver(self->buffer, self);
    }
#else
    frame_SetBuffer(self, NULL, FALSE);
#endif
    frame_SetNth(self, 1, NULL);    /* Remove the view from the	lpair */	
    framemessage_Destroy(self->messageLine);
    framemessage_Destroy(self->dialogLine);
    cursor_Destroy(self->octcursor);	
    cursor_Destroy(self->arrowcursor);	

    if (self->title != NULL)
        free(self->title);

    previous = &allFrames;
    for (traverse = allFrames; traverse != NULL && traverse != self; traverse = traverse->next)
        previous = &traverse->next;
    if (traverse != NULL) /* Bad error if this is false. */
        *previous = traverse->next;

}

static void SetTitle();

void frame__FullUpdate(self, type, left, top, width, height)
    struct frame *self;
    enum view_UpdateType type;
    long left, top, width, height;
{

    if (self->lineHeight == 0) {
        self->lineHeight = CalculateLineHeight(self);
        frame_VFixed(self, self->childView, self->messageView, self->lineHeight, TRUE);
    }

    SetTitle(self);

    super_FullUpdate(self, type, left, top, width, height);
    if (self->IsAsking) {
	ComputeSize(self);
	DoUpdate(self);
    }
}
static handleNewData(self)
struct frame *self;
{
    struct view *inputFocus, *targetView,*oldchild;
    oldchild = self->childView;
    view_UnlinkTree(oldchild);
    view_UnlinkTree(self->targetView);
    if (self->object)
        dataobject_RemoveObserver(self->object, self);
    self->object = buffer_GetData(self->buffer);
    dataobject_AddObserver(self->object, self);
    self->childView = buffer_GetView(self->buffer, &inputFocus, &targetView, NULL);
    self->targetView = targetView;
    frame_VFixed(self, self->childView, self->messageView, self->lineHeight, TRUE);
 /*   buffer_RemoveView(self->buffer,oldchild); */

}

void frame__Update(self)
    struct frame *self;
{
    struct pendingupdates *pu;

    self->UpdateRequested = FALSE;
    if(self->buffer != NULL && self->object != buffer_GetData(self->buffer)){
	handleNewData(self);
    }
    SetTitle(self);
    if (self->IsAsking && self->uplist) {
	RestoreBits(self);
/* 	CannotRestoreBits(self);    Will work but is too slow to be worth it */ 
    }
    while (self->uplist) {
	view_Update(self->uplist->v);
	pu = self->uplist->next;
	free(self->uplist);
	self->uplist = pu;
    }
    if (self->IsAsking) {
	DoUpdate(self);
    }
    else super_Update(self);

}

void frame__ObservedChanged(self, changed, value)
    struct frame *self;
    struct observable *changed;
    long value;
{
    if (self->buffer != NULL && changed == (struct observable *) self->buffer && self->object != buffer_GetData(self->buffer)) {
        if (self->object)
            dataobject_RemoveObserver(self->object, self);
        self->object = buffer_GetData(self->buffer);
        dataobject_AddObserver(self->object, self);
    }
    else if (changed == (struct observable *) self->object) {
        if (value == observable_OBJECTDESTROYED) {
            self->object = NULL;
        }
        else if (self->dataModified || buffer_GetScratch(self->buffer)) {
            return;
        }
    }

    if (!self->UpdateRequested)
        frame_WantUpdate(self, self);
}

void frame__SetBuffer(self, buffer, setInputFocus)
    struct frame *self;
    struct buffer *buffer;
    boolean setInputFocus;
{

    struct view *inputFocus, *targetView;

    if (buffer != self->buffer) {
        if (self->buffer != NULL) {
            if (self->childView != NULL) {
                frame_SetNth(self, 0, NULL); /* Remove the view from the lpair so we can deallocate it. */
                buffer_RemoveView(self->buffer, self->childView); /* Deallocate the view... */
            }
            if (self->object)
                dataobject_RemoveObserver(self->object, self);
            buffer_RemoveObserver(self->buffer, self);
        }
        self->buffer = buffer;
#if 1
        if (buffer != NULL) {
#endif
            buffer_AddObserver(buffer, self);
            self->object = buffer_GetData(self->buffer);
            dataobject_AddObserver(self->object, self);
            self->childView = buffer_GetView(buffer, &inputFocus, &targetView, NULL);
            self->targetView = targetView;
            frame_VFixed(self, self->childView, self->messageView, self->lineHeight, TRUE);
            if (setInputFocus)
                if (inputFocus == NULL)
                    view_WantInputFocus(targetView, targetView);
                else
                    view_WantInputFocus(inputFocus, inputFocus);
#if 1
        }
#endif
    }
}

void frame__SetView(self, view)
    struct frame *self;
    struct view *view;
{

    if (self->buffer != NULL) {
        if (self->childView != NULL) {
            frame_SetNth(self, 0, NULL); /* Remove the view from the lpair so we can deallocate it. */
            buffer_RemoveView(self->buffer, self->childView); /* Deallocate the view... */
        }
        buffer_RemoveObserver(self->buffer, self);
        if (self->object)
            dataobject_RemoveObserver(self->object, self);
        self->object = NULL;
        self->buffer = NULL;
    }
    self->childView = self->targetView = view;
    if (view != NULL) {
        frame_VFixed(self, self->childView, self->messageView, self->lineHeight, TRUE);
        view_WantInputFocus(view, view);
    }
}

/* Create a frame suitable for use as a buffer window. */
struct frame *frame__Create(classID, buffer)
    struct classheader *classID;
    register struct buffer *buffer;
{

    register struct frame *tempFrame = frame_New();

    frame_SetBuffer(tempFrame, buffer, TRUE);
    frame_SetCommandEnable(tempFrame, TRUE);
    return tempFrame;
}

/* Iterates over all frames.
 */
struct frame *frame__Enumerate(classID, mapFunction, functionData)
    struct classheader *classID;
    boolean (*mapFunction)();
    long functionData;
{

    struct frame *traverse, *next;

    for (traverse = allFrames; traverse != NULL; traverse = next) {
        next = traverse->next; /* So mapFunction is allowed to delete the frame. */
        if ((*mapFunction)(traverse, functionData))
            return traverse;
    }
    return NULL;
}

struct basicobject *frame__WantHandler(self, handlerName)
struct frame *self;
char *handlerName;
{
    if (strcmp(handlerName, "message") == 0)
	return (struct basicobject *) self->messageLine;
    else if (self->header.view.parent == NULL) return NULL;
    else
	return view_WantHandler(self->header.view.parent, handlerName);
}

void frame__PostKeyState(self, keystate)
struct frame *self;
struct keystate *keystate;
{
    if(self->IsAsking){
	if (keystate == NULL || keystate->object != (struct basicobject *) self->dialogView) {
	    self->mykeystate->next = NULL;
	    keystate_Reset(self->mykeystate);
	    keystate_AddAfter(self->mykeystate, keystate);
	}
	if (keystate == NULL) {
	    view_PostKeyState(self->header.view.parent, self->mykeystate);
	}
	else{
	    if (keystate == self->mykeystate->next) {
		self->mykeystate->next = NULL;
	    }
	    view_PostKeyState(self->header.view.parent, keystate);
	}
    }
    else  if (self->commandEnable) {
	if (keystate == NULL || keystate->object != (struct basicobject *) self->messageView) {
            self->keystate->next = NULL;
            keystate_Reset(self->keystate);
            keystate_AddAfter(self->keystate, keystate);
        }
        if (keystate == NULL) {
            view_PostKeyState(self->header.view.parent, self->keystate);
        }
        else
            view_PostKeyState(self->header.view.parent, keystate);
    }
    else
        view_PostKeyState(self->header.view.parent, keystate);
}


void frame__PostMenus(self, menulist)
struct frame *self;
struct menulist *menulist;
{

    if(self->IsAsking ){
	if((menulist != NULL) && self->hasDialogMessage && menulist->object == (struct basicobject *) self->dialogView){
	    menulist_ClearChain(self->mymenus);
	    menulist_ChainBeforeML(self->mymenus, menulist, menulist);
	    view_PostMenus(self->header.view.parent,self->mymenus);
	}
	else {
	    view_PostMenus(self->header.view.parent, self->mymenus);
	}
    }
    else if (self->commandEnable && ((menulist == NULL) || (menulist->object != (struct basicobject *) self->messageView))) {
	menulist_ClearChain(self->menulist);
	menulist_ChainBeforeML(self->menulist, menulist, menulist);
	view_PostMenus(self->header.view.parent, self->menulist);
    }
    else
	view_PostMenus(self->header.view.parent, menulist);
}

void frame__SetCommandEnable(self, enable)
    struct frame *self;
    boolean enable;
{

    if ((self->commandEnable = enable) == TRUE) {
        if (frameKeymap == NULL) {
 
            extern struct keymap *frame_InitKeymap();

            frameKeymap = framecmds_InitKeymap(&frameMenus);
        }

        if (self->keystate == NULL)
            self->keystate = keystate_Create(self, frameKeymap);
        if (self->menulist == NULL)
            self->menulist = menulist_DuplicateML(frameMenus, self);
    }
}

struct buffer *frame__GetHelpBuffer(self)
    struct frame *self;
{

    int i;
    struct buffer *buffer;

    for (i = 1; i < 100 && ((buffer = buffer_FindBufferByName(self->helpBuffer)) == NULL); i++) {

        char bufferName[30];

        sprintf(bufferName, "Help-Buffer-%d", i);
        if (((buffer = buffer_FindBufferByName(bufferName)) == NULL) || !buffer_Visible(buffer)) {
            if (buffer == NULL)
                buffer = buffer_Create(bufferName, NULL, "text", NULL);
            strncpy(self->helpBuffer, bufferName, sizeof(self->helpBuffer));
        }
    }
    buffer_SetScratch(buffer, TRUE);
    return buffer;
}
struct buffer *frame__SetHelpBuffer(self, buf, setInputFocus)
struct frame *self;
struct buffer *buf;
boolean setInputFocus;
{   /* Returns the last buffer IF it will need to be restored */
    struct view *inputFocus, *targetView;
    int NeedUpdate = 0;
    if(!self->IsAsking) {
	struct buffer *LastBuffer = frame_GetBuffer(self);
	frame_SetBuffer(self, buf, setInputFocus);
	return LastBuffer;
    }
    /* Put the buffer in the dialog box */
    if(self->DialogBuffer == NULL) NeedUpdate++;
    if (buf != self->DialogBuffer) {
	if (self->DialogBuffer != NULL) {
	    if (self->DialogBufferView != NULL)
		buffer_RemoveView(self->DialogBuffer, self->DialogBufferView);
	    buffer_RemoveObserver(self->DialogBuffer, self);
	}
	buffer_AddObserver(buf, self);
	self->DialogBuffer = buf;
	self->DialogBufferView = buffer_GetView(buf, &inputFocus, &targetView,"helptextview");
	self->DialogTargetView = targetView;
    }
    if(NeedUpdate){
	ComputeSize(self);
	frame_WantUpdate(self,self);
    }
    return NULL;
}

void frame__SetTitle(self, title)
    struct frame *self;
    char *title;
{
    if (self->title != NULL)
        free(self->title);

    if (title != NULL) {
        self->title = (char *) malloc(strlen(title) + 1);
        strcpy(self->title, title);
    }
    else
        self->title = NULL;

    if (! self->UpdateRequested)
        frame_WantUpdate(self, self);
}

/*
 * This code sets the title bar such that the end of a filename is visible.
 * Seldom is there so much code to accomplish so little.
 */
static void SetTitle(self)
    struct frame *self;
{

#define WMTITLELEN 40 /* Can you say "Magic hack?" */

    if (self->title == NULL) {
        if (frame_GetBuffer(self) == NULL)
            return;
        else {

            char titleBuffer[WMTITLELEN], *titleLine;
            int len, maxLen = sizeof(titleBuffer) - 1;

	    *titleBuffer = '\0';

	    if (self->buffer != NULL){
		if(buffer_GetScratch(self->buffer))
		    self->dataModified = FALSE;
		else if (self->dataModified = (buffer_GetWriteVersion(self->buffer) < dataobject_GetModified(buffer_GetData(self->buffer))))
		    --maxLen; /* Make room for '*' */
	    }
	    titleLine=buffer_GetFilename(frame_GetBuffer(self));
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
	    }else{
		titleLine=buffer_GetName(frame_GetBuffer(self));
		if (titleLine==NULL) {
		    im_SetTitle(frame_GetIM(self), "");
		    return;
		}else{
		    strcpy(titleBuffer, "Buffer: ");
		    maxLen-=sizeof("Buffer: ")-1;
		    strncat(titleBuffer,titleLine,maxLen);
		}
	    }

	    if (self->dataModified)
		strcat(titleBuffer, "*");

	    im_SetTitle(frame_GetIM(self), titleBuffer);
	}
    }
    else
	im_SetTitle(frame_GetIM(self), self->title);
}

/* The following is code to support the dialog box */

static void ConsiderReturning(self, Choice)
struct frame *self;
int Choice;
{
    if (self->StackPos != frame_GlobalStackCount) {
	framemessage_DisplayString(self->messageLine, 0, "Please answer the other dialog box first.");
	return;
    }
    if (Choice <= 0 || Choice > self->NumAnswerChoices) {
	framemessage_DisplayString(self->messageLine, 0, "That is not an answer; please try again.");
	return;
    }
    if(self->hasDialogMessage){
	switch(Choice){
	    case ACCEPT:
		frameview_Return(self->dialogView,(long)'\n');
		break;
	    case CANCEL:
		framemessage_CancelQuestion(self->dialogLine);
		break;
	    case COMPLETE:
		frameview_Complete(self->dialogView,(long)' ');
		return;
	    case HELP:
		frameview_Help(self->dialogView,(long)'?');
		return;
	}
    }
    self->IsAsking = 0;
    self->WildestAnswer = Choice;
}

struct view *
frame__Hit(self, action, x, y, nclicks)
struct frame *self;
enum view_MouseAction action;
long x, y, nclicks;
{
    int i;
    struct rectangle r;
    if(self->hasDialogMessage){
	if( self->dialogView && InRectangle(&self->mesrec, x, y))
	    return frameview_Hit(self->dialogView, action, view_EnclosedXToLocalX((struct view *)(self->dialogView), x), view_EnclosedYToLocalY((struct view *)(self->dialogView), y), nclicks);
	else if (self->DialogBufferView && InRectangle(&self->bufferrec, x, y))
	    return view_Hit(self->DialogBufferView, action, view_EnclosedXToLocalX((struct view *)(self->DialogBufferView), x), view_EnclosedYToLocalY((struct view *)(self->DialogBufferView), y), nclicks);
    }
    if (!self->IsAsking
	 || (!self->IsBlocking && !InRectangle(&self->AnswerBox, x, y))) {
	return super_Hit(self, action, x, y, nclicks);
    }

    if (action != view_LeftUp && action != view_RightUp) {
	return((struct view *) self);
    }
    /* Choose the answer here */
    for (i=1; i<=self->NumAnswerChoices; ++i) {
	r = self->HeightsOfAnswer[i];
	r.width = self->buttonmaxwid + frame_TOTALPADDING - frame_TWOSEPARATIONS;
	if (InRectangle(&r, x, y)) {
	    /* found right answer */
	    ConsiderReturning(self, i);
	    return((struct view *) self);
	}
    }
    if(!self->hasDialogMessage && InRectangle(&self->AnswerBox, x, y)) 
	ConsiderReturning(self, self->DefaultWildestAnswer);
    return((struct view *) self);
}
static 
InRectangle(r, x, y)
struct rectangle *r;
long x, y;
{
    if ((x < r->left) || (x > (r->left + r->width)) || (y < r->top) || (y > (r->top + r->height))) {
	return(0);
    }
    return(1);
}
static drawshadow(self,r)
struct frame *self;
struct rectangle *r;
{
    frame_FillRectSize(self,r->left + OFFSET,r->top + r->height,r->width,OFFSET,frame_GrayPattern(self,8,16));
    frame_FillRectSize(self,r->left + r->width,r->top + OFFSET,OFFSET,r->height - OFFSET,frame_GrayPattern(self,8,16));
}
static drawButton(self,rr,str,pushed)
struct frame *self;
struct rectangle *rr;
char *str;
boolean pushed;
{

    struct rectangle r;
    r = *rr;
    r.left +=frame_SEPARATION; r.top +=frame_SEPARATION;
    r.width = self->buttonmaxwid + frame_TOTALPADDING - frame_TWOSEPARATIONS;
    r.height -= frame_TWOSEPARATIONS;
    frame_FillRectSize(self,r.left,r.top,r.width + BUTTONOFF,r.height + BUTTONOFF,frame_WhitePattern(self));
#ifdef RAISEDBUTTONS
    offset = (pushed) ? BUTTONON: BUTTONOFF;
    if(pushed) {
	r.left += BUTTONOFF - BUTTONON; r.top += BUTTONOFF - BUTTONON;
    }
    frame_FillTrapezoid(self,r.left,r.top + r.height, r.width,
			 r.left	+ offset, r.top	+ r.height + offset, r.width,
			 frame_GrayPattern(self,8,16));
    frame_FillTrapezoid(self,r.left + r.width ,r.top , 0,
			 r.left + r.width,  r.top + offset,offset,
			 frame_GrayPattern(self,12,16));
    frame_FillTrapezoid(self,r.left + r.width ,r.top + r.height , offset,
			 r.left + r.width +offset , r.top + r.height + offset,0,
			 frame_GrayPattern(self,12,16));
    frame_FillRectSize(self,r.left + r.width ,r.top + offset,
			offset,r.height - offset,frame_GrayPattern(self,12,16));
    frame_MoveTo(self,r.left + r.width,r.top );
    frame_DrawLine(self,offset,offset);frame_DrawLine(self,0,r.height);
    frame_DrawLine(self,-r.width,0);frame_DrawLine(self,-offset,-offset);
    frame_MoveTo(self,r.left + r.width,r.top + r.height);
    frame_DrawLine(self,offset,offset);
#endif /* RAISEDBUTTONS */
    frame_DrawRect(self, &r);
    if(str){
	frame_MoveTo(self, r.left + ((r.width - rr->width) / 2), r.top + frame_SEPARATION + 1);
	frame_DrawString(self, str, graphic_ATLEFT | graphic_ATTOP);
    }
    if(pushed){
	r.top++; r.left++;r.width--;r.height--;r.width--;r.height--;
	r.top++; r.left++;r.width--;r.height--;
	frame_SetTransferMode(self, graphic_INVERT);
	frame_FillRect(self, &r,frame_BlackPattern(self));
	frame_SetTransferMode(self, graphic_COPY);
    }
}

static 
DoUpdate(self)
struct frame *self;
{
    struct rectangle *r;
    int i;

    SaveBits(self);
    frame_SetFont(self, self->myfontdesc);
    frame_SetTransferMode(self, graphic_COPY);
    if(self->DialogBuffer){
	frame_FillRect(self, &self->bufferrec, frame_WhitePattern(self));
	frame_FillRect(self, &self->AnswerBox, frame_WhitePattern(self));
	    /* Also clear area between boxes */
	if(self->AnswerBox.top == self->bufferrec.top) 	
	    frame_FillRectSize(self,self->AnswerBox.left + self->AnswerBox.width ,  self->AnswerBox.top,10, self->AnswerBox.height + OFFSET , frame_WhitePattern(self));
	else {
	    frame_FillRectSize(self,self->AnswerBox.left,  self->AnswerBox.height +self->AnswerBox.top, self->AnswerBox.width  + OFFSET ,10, frame_WhitePattern(self));
	}

    }
    else 	
	frame_FillRect(self,&self->AnswerBox, frame_WhitePattern(self));

    if(self->DialogBuffer)   { 
	drawshadow(self,&self->bufferrec);
	frame_DrawRect(self,&self->bufferrec);
    }
    drawshadow(self,&self->AnswerBox);
    frame_DrawRect(self,&self->AnswerBox);
    for (i=1; i<= self->NumAnswerChoices; ++i) {
	drawButton(self,&self->HeightsOfAnswer[i],self->MultipleAnswers[i],(i == self->DefaultWildestAnswer));
    }
    if(self->hasDialogMessage){
	r = &self->mesrec;
	frame_DrawRectSize(self, r->left - 1, r->top -1, r->width + 2, r->height + 2);
    }
    r = &self->HeightsOfAnswer[0];
    frame_MoveTo(self, r->left + frame_SEPARATION + 3, r->top + frame_SEPARATION + 3);
    frame_DrawString(self, self->MultipleAnswers[0], graphic_ATLEFT | graphic_ATTOP);
    frame_PostCursor(self, &self->AnswerBox, self->octcursor);
    if(self->hasDialogMessage){
	frameview_InsertView(self->dialogView, self, &self->mesrec);
	frameview_FullUpdate(self->dialogView, view_FullRedraw , 0, 0, 0, 0);
	frame_PostCursor(self, &self->mesrec, self->arrowcursor);
	if(self->DialogBuffer){
	    struct rectangle nr;
	    nr = self->bufferrec;
	    nr.top++;nr.left++; nr.width += -2; nr.height += -2;
	    view_InsertView(self->DialogBufferView, self, &nr);
	    view_FullUpdate(self->DialogBufferView, view_FullRedraw , 0, 0, 0, 0);
	}
    }
}

static 
SaveBits(self)
struct frame *self;
{
    /* A no-op for now */
}
static int ButtonInteract(self, AnswerList, DefaultWildestAnswer, WildestAnswer, flags)
struct frame *self;
char **AnswerList;
long DefaultWildestAnswer, *WildestAnswer;
int flags;
{
    int i;
    struct view *focus;

    if (self->IsAsking || self->AwaitingFocus) return(-1);
    while(im_Interact(0)); /*Clear out any pending updates */
    for (i = 0; AnswerList[i] && *AnswerList[i]; ++i) {
 	;
    }
    self->NumAnswerChoices = --i;
    if (i <= 0) {
	return(-1);
    }
    self->MultipleAnswers = AnswerList;
    self->DefaultWildestAnswer = DefaultWildestAnswer;
    self->StackPos = ++frame_GlobalStackCount;
    ComputeSize(self);
    self->IsAsking = 1;
    focus = im_GetInputFocus(frame_GetIM(self));
    if (focus && focus != (struct view *)self) view_LoseInputFocus(focus);
    PrepareMenus(self);
    if(! self->UpdateRequested) frame_WantUpdate(self, self);
    while (self->IsAsking) {
	im_Interact(1);
    }
    --frame_GlobalStackCount;
    retractCursors(self);
    if(!RestoreBits(self))
	CannotRestoreBits(self);
    if ((self->WildestAnswer <=0) || (self->WildestAnswer > self->NumAnswerChoices)) {
	*WildestAnswer = -1;
    } else {
	*WildestAnswer = self->WildestAnswer;
    }
    TidyUp(self);
    if (focus && focus != (struct view *)self ) view_ReceiveInputFocus(focus);
    return((*WildestAnswer == -1) ? -1 : 0);
}
static 
PrepareMenus(self)
struct frame *self;
{
    int i, plen;
    char Priority[10], QBuf[600], BigBuf[1200]; /* No one in his right mind will ever exceed these */

    /* if usemine, force out my keystate & menulist.  Otherwise restore the
    last non-mine version. */
    if (self->IsAsking) {
	menulist_ClearML(self->mymenus);
#ifdef PROBABLYHOPLESS /* wm bug on long menu items */
	strcpy(QBuf, self->MultipleAnswers[0]);
	PurifyString(QBuf);
	strcat(QBuf, ",");
#else /* PROBABLYHOPLESS  */
	strcpy(QBuf, "Dialog Box,");
#endif /* PROBABLYHOPLESS  */
	plen = strlen(QBuf);
	for (i=1; i<= self->NumAnswerChoices; ++i) {
	    strcpy(BigBuf, QBuf);
	    strcat(BigBuf, self->MultipleAnswers[i]);
	    PurifyString(&BigBuf[plen]);
	    if (strlen(BigBuf) > 80) { /* wm bug on long menu items */
		BigBuf[78] = '\0';
		strcat(BigBuf, "...");
	    }
	    sprintf(Priority, "~%d", i);
	    strcat(BigBuf, Priority);
	    menulist_AddToML(self->mymenus, BigBuf, returnconsidered, i,0);
	}
	menulist_AddToML(self->mymenus, "Quit", 0, 0,0);
	frame_PostMenus(self, self->mymenus); 
	frame_PostKeyState(self, self->mykeystate);
    }
}

/* This next routine tries to make sure a string won't blow up in a menu */
static 
PurifyString(s) 
char *s;
{
    if (!s) return;
    while (*s) {
	switch(*s) {
	    case ',':
		*s++ = '.';
		break;
	    case ':':
	    case '~':
		*s++ = '-';
		break;
	    default:
		++s;
	}
    }
}
static 
RestoreBits(self)
struct frame *self;
{
    /* for now, a no-op */
    return FALSE;
}

/* this next routine should go away once SaveBits and RestoreBits work. */
static 
CannotRestoreBits(self)
struct frame *self;
{
    struct rectangle r;

    frame_GetLogicalBounds(self, &r);
    super_FullUpdate(self, view_FullRedraw, r.left, r.top, r.width, r.height);
}

void frame__Advice(self, pp)
struct frame *self;
enum message_Preference pp;
{
    if(pp == message_NoBlock) self->IsBlocking = 0;
    else self->PositionalPreference = pp;
}
static 
ComputeSize(self)
struct frame *self;
{
    int i, xw, yw, curht, totht, totwid,realwid, curleft, maxheight, boxheight,maxwid = 0;

    maxheight = 0;
    if (!self->HeightsOfAnswer) {
	self->HeightsOfAnswer = (struct rectangle *) malloc((2+self->NumAnswerChoices) * sizeof(struct rectangle));
    }
    self->buttonmaxwid = 0;
    realwid = frame_GetLogicalWidth(self);
    curht = frame_HALFPADDING;
    if(self->DialogBuffer && realwid > MINSIDEBYSIDEWIDTH){
	totwid = realwid / 2;
    }
    else totwid = realwid;
    /* get max button sizes */
    for(i = 1; i <= self->NumAnswerChoices; i++) {

        struct FontSummary *fontSummary;

	fontdesc_StringSize(self->myfontdesc, frame_GetDrawable(self), self->MultipleAnswers[i], &xw, &yw);
	if ((i > 0) && (xw > self->buttonmaxwid))
            self->buttonmaxwid = xw;

/* This code should really be factored out of this loop, but I don't have
 * time to test that now.
 */
        if ((fontSummary = fontdesc_FontSummary(self->myfontdesc,
                                                frame_GetDrawable(self))) == NULL) {
            if (yw < (frame_FONTSIZE+frame_FONTPAD))
                yw = frame_FONTSIZE+frame_FONTPAD;
        }
        else
            yw = fontSummary->maxHeight + frame_FONTPAD;

	if(maxheight < yw) maxheight = yw;
	self->HeightsOfAnswer[i].height = yw;
	self->HeightsOfAnswer[i].width = xw;
    }
    curleft = (totwid - self->buttonmaxwid - frame_TOTALPADDING) / 2;

    /* calculate the header placement */
    fontdesc_StringSize(self->myfontdesc, frame_GetDrawable(self), self->MultipleAnswers[0], &xw, &yw);
    if (yw < (frame_FONTSIZE+frame_FONTPAD)) yw = frame_FONTSIZE+frame_FONTPAD; /* bogus fontdesc bug */
    self->HeightsOfAnswer[0].left = (totwid - xw - frame_TOTALPADDING) / 2;
    self->HeightsOfAnswer[0].width = xw+frame_TOTALPADDING;
    if (xw > maxwid) maxwid = xw;
    self->HeightsOfAnswer[0].top = curht;
    self->HeightsOfAnswer[0].height = yw;
    curht += frame_TWOSEPARATIONS + yw;

    if(self->hasDialogMessage){
	/* calculate the dialog box placement */
	if(self->mesrec.height == 0)
	    self->mesrec.height = self->lineHeight;
	xw = totwid - frame_TOTALPADDING - frame_TOTALPADDING - frame_TOTALPADDING - frame_TOTALPADDING;
	self->mesrec.left = (totwid - xw - frame_TOTALPADDING) / 2;
	self->mesrec.width = xw+frame_TOTALPADDING;
	self->mesrec.top = curht;
	curht += frame_TWOSEPARATIONS + self->mesrec.height;
    }

    /* calculate button placement */

    for (i=1; i<= self->NumAnswerChoices; ++i) {
	self->HeightsOfAnswer[i].left = curleft;
	self->HeightsOfAnswer[i].top = curht;
	curht += frame_TWOSEPARATIONS + maxheight;
    }

    if(self->hasDialogMessage && (self->buttonmaxwid + frame_TOTALPADDING) * 2 < totwid){
	/* double up buttons */
	curht = self->HeightsOfAnswer[1].top;
	for(i = 1; i< self->NumAnswerChoices; i += 2) {
	    self->HeightsOfAnswer[i].left += -10 -(self->buttonmaxwid / 2);
	    self->HeightsOfAnswer[i+1].left += 10 + (self->buttonmaxwid / 2);
	    self->HeightsOfAnswer[i + 1].top = curht;
	    self->HeightsOfAnswer[i].top = curht;
	    self->HeightsOfAnswer[i].height = maxheight;
	    self->HeightsOfAnswer[i + 1].height = maxheight;
	    curht += frame_TWOSEPARATIONS + maxheight;
	}
	if(i == self->NumAnswerChoices){
	    self->HeightsOfAnswer[i].top = curht;
	    curht += frame_TWOSEPARATIONS + self->HeightsOfAnswer[i].height;
	}
	maxwid = MAX((self->buttonmaxwid + frame_TOTALPADDING)* 2 ,maxwid);
    }
    else maxwid = MAX((self->buttonmaxwid + frame_TOTALPADDING) ,maxwid);
    if(self->hasDialogMessage)
	maxwid =  self->mesrec.width - frame_TOTALPADDING;

    /* Decide where in the view rect to put the box rect */
    self->AnswerBox.height = curht + frame_TOTALPADDING;
    totht = frame_GetLogicalHeight(self);
    boxheight = (self->DialogBuffer && realwid <= MINSIDEBYSIDEWIDTH)?   (self->AnswerBox.height + OFFSET)* 2 : self->AnswerBox.height ;
    if (boxheight > (totht + frame_TOTALPADDING)) {
	self->AnswerBox.top = 0;
    } else if (self->PositionalPreference == message_OnTop) {
	self->AnswerBox.top = frame_HALFPADDING;
    } else if (self->PositionalPreference  == message_OnBottom) {
	self->AnswerBox.top = totht - boxheight - frame_HALFPADDING;
    } else {
	self->AnswerBox.top = (totht - boxheight) / 2;
    }
    self->AnswerBox.width = maxwid + frame_DOUBLEPADDING + frame_HALFPADDING;
    self->AnswerBox.left = (totwid - maxwid - frame_DOUBLEPADDING - frame_HALFPADDING) / 2;
    /* Now go back and fix up all the offsets */
    curht = self->AnswerBox.top;
    if (curht) {
	for (i=0; i<=self->NumAnswerChoices; ++i) {
	    self->HeightsOfAnswer[i].top += curht;
	}
	self->mesrec.top += curht;
    }

    if(self->DialogBuffer){
	if(totwid < realwid){
	    self->bufferrec.top = self->AnswerBox.top ;	
	    self->bufferrec.left = self->AnswerBox.left + self->AnswerBox.width  + OFFSET + OFFSET;
	    self->bufferrec.width = realwid - self->AnswerBox.width - (OFFSET * 6);
	    self->bufferrec.height = self->AnswerBox.height ;
	}
	else {
	    self->bufferrec.top = self->AnswerBox.top + self->AnswerBox.height + OFFSET + OFFSET;	
	    self->bufferrec.left = self->AnswerBox.left;
	    self->bufferrec.width = self->AnswerBox.width ;
	    self->bufferrec.height = self->AnswerBox.height ;

	}
    }
}


static void GotKey(self, c)
struct frame *self;
char c;
{
    int curpt, startpt=self->DefaultWildestAnswer;
    char c1;

    if (startpt <= 0 || startpt > self->NumAnswerChoices) {
	startpt = 1;
    }
    curpt = startpt;
    c1 = isupper(c) ? tolower(c) : toupper(c);
    while (1) {
	if (++curpt > self->NumAnswerChoices) curpt = 1;
	if ((*self->MultipleAnswers[curpt] == c)
	|| (*self->MultipleAnswers[curpt] == c1)) {
	    drawButton(self,&self->HeightsOfAnswer[self->DefaultWildestAnswer], self->MultipleAnswers[self->DefaultWildestAnswer],FALSE);
	    self->DefaultWildestAnswer = curpt;
	    drawButton(self,&self->HeightsOfAnswer[self->DefaultWildestAnswer], self->MultipleAnswers[self->DefaultWildestAnswer],TRUE);
	    return;
	}
	if (curpt == startpt) {
	    framemessage_DisplayString(self->messageLine, 0, "That keystroke does not match any of the choices.");
	    return;
	}
    }
}

static void ConfirmDefaultAnswer(self)
struct frame *self;
{
    ConsiderReturning(self, self->DefaultWildestAnswer);
}

static void Cancel(self)
struct frame *self;
{
    /* ^G key binding */
    if (self->StackPos != frame_GlobalStackCount) {
	framemessage_DisplayString(self->messageLine, 0, "Please answer the other dialog box first.");
	return;
    }
#ifdef MUSTANSWER
    if (self->IsBlocking) {
	framemessage_DisplayString(self->messageLine, 0, "This is a question that MUST be answered.");
    } else {
	framemessage_DisplayString(self->messageLine, 0, "Question Cancelled.");
	self->IsAsking = 0;
	self->WildestAnswer = -1;
    }	
#else /* MUSTANSWER */
    framemessage_DisplayString(self->messageLine, 0, "Question Cancelled.");
    self->IsAsking = 0;
    self->WildestAnswer = -1;
#endif /* MUSTANSWER */
    if(self->hasDialogMessage){
	framemessage_CancelQuestion(self->dialogLine);
    }

}
static retractCursors(self)
struct frame *self;
{
    if(cursor_IsPosted(self->octcursor))
	frame_RetractCursor(self, self->octcursor);
    if(cursor_IsPosted(self->arrowcursor))
	frame_RetractCursor(self, self->arrowcursor);
    frame_PostMenus(self,NULL);
}
static 
TidyUp(self)
struct frame *self;
{
    struct pendingupdates *pu;

    self->IsAsking =  self->NumAnswerChoices = 0;  
    self->IsBlocking = TRUE;
    self->DefaultWildestAnswer = self->StackPos = -1;
    self->PositionalPreference = message_Center;
    self->MultipleAnswers = NULL;
    while (self->uplist) {
	pu = self->uplist->next;
	free(self->uplist);
	self->uplist = pu;
    }
    if (self->HeightsOfAnswer) {
	free(self->HeightsOfAnswer);
	self->HeightsOfAnswer = NULL;
    }
    if(self->hasDialogMessage && self->dialogView) frameview_UnlinkTree(self->dialogView);

    if (self->DialogBuffer != NULL) {
	if (self->DialogBufferView != NULL)
	    buffer_RemoveView(self->DialogBuffer, self->DialogBufferView);
	buffer_RemoveObserver(self->DialogBuffer, self);
    }
    self->DialogBuffer = NULL;
    self->DialogBufferView = NULL;
    self->DialogTargetView = NULL;
    self->hasDialogMessage = 0;
}
static isDialogChild(self, v)
struct frame *self;
struct view *v;
{
    while(v->parent != NULL && v->parent != (struct view *) self) 
	v = v->parent;
    return(v == self->DialogBufferView || v == (struct view *)self->dialogView);
}
void frame__WantUpdate(self, v)
struct frame *self;
struct view *v;
{
    if (self->IsAsking) {
	struct pendingupdates *pu;
	if(isDialogChild(self,v)) {
	    super_WantUpdate(self,v);
	    return;
	}
	if (v != (struct view *) self) {
	    pu = (struct pendingupdates *) malloc(sizeof (struct pendingupdates));
	    pu->next = self->uplist;
	    pu->v = v;
	    self->uplist = pu;
	}
	super_WantUpdate(self, self);
	self->UpdateRequested = TRUE;
    } else {
	super_WantUpdate(self, v);
    }
}
#define ERROR -1
int frame__DisplayString(self, priority, string)
struct frame *self;
int priority;
char *string;
{
    char *mychoices[3];
    long result,defaultChoice;
    defaultChoice = 1;
    mychoices[0] = string;
    mychoices[1] = "continue";
    mychoices[2] = NULL;
    if(!ButtonInteract(self,mychoices,defaultChoice,&result,priority > frame_MUSTANSWER)){
	return(0);
    }
    return ERROR;
}
static struct view *PrepareForStringInput(self,prompt,bufferSize,CompButtons)
struct frame *self;
char *prompt;
int bufferSize; /* Is actual sizeof buffer including NUL. */
boolean CompButtons;
{
    static char *ans[4];
    struct view *focus;
    self->AwaitingFocus = 1;
    focus = im_GetInputFocus(frame_GetIM(self));
    if (focus) frame_WantInputFocus(self,self); /* we take the focus ourself so that when the dialogview asks for it, no-one else has to redraw */
    while(im_Interact(0)); /* let the current focus get his update before the box is drawn */
    self->AwaitingFocus = 0;
    self->MultipleAnswers = ans;
    self->MultipleAnswers[0] = prompt;
    self->MultipleAnswers[ACCEPT] = " accept ";
    self->MultipleAnswers[CANCEL] = " cancel ";
    if(CompButtons) {
	self->MultipleAnswers[COMPLETE] = "complete";
	self->MultipleAnswers[HELP] = "  help  ";
	self->MultipleAnswers[5] = NULL;
	self->NumAnswerChoices = 4 ;
    }
    else {
	self->MultipleAnswers[3] = NULL;
	self->NumAnswerChoices = 2 ;
    }
    self->IsAsking = 1;
    self->hasDialogMessage = TRUE;

    self->DefaultWildestAnswer = 0;
    self->WildestAnswer = -1;
    self->StackPos = ++frame_GlobalStackCount;
    ComputeSize(self);
    PrepareMenus(self);
    if(!self->UpdateRequested)frame_WantUpdate(self, self);
    while(im_Interact(0)); 
    return focus;
}
int frame__AskForString(self, priority, prompt, defaultString, buffer, bufferSize)
struct frame *self;
int priority;
char *prompt, *defaultString, *buffer;
int bufferSize; /* Is actual sizeof buffer including NUL. */
{
    int i;
    struct view *focus;
    if (self->IsAsking || self->AwaitingFocus) return(-1);
    focus = PrepareForStringInput(self,prompt,bufferSize,FALSE);
    i = framemessage_AskForString(self->dialogLine, 0 ,"", defaultString, buffer, bufferSize);
    while(im_Interact(0));
    --frame_GlobalStackCount;
    retractCursors(self);
    if(!RestoreBits(self))
	CannotRestoreBits(self);
    TidyUp(self);
    if (focus) view_WantInputFocus(focus,focus);
    return i;
}
int frame__AskForStringCompleted(self, priority, prompt, defaultString, buffer, bufferSize, keystate, completionProc, helpProc, completionData, flags)
struct frame *self;
int priority;
char *prompt, *defaultString, *buffer;
int bufferSize; /* Is actual sizeof buffer including NUL. */
struct keystate *keystate;
enum message_CompletionCode (*completionProc)(/* char *string, long rock, char *buffer, int buffersize */);
int (*helpProc)(/* char *partialKeyword, long rock, int (*helpTextFunction)(), long helpTextRockchar */);
long completionData;
int flags;
{  
    int i;
    struct view *focus;
    if (self->IsAsking || self->AwaitingFocus) return(-1);
    focus = PrepareForStringInput(self,prompt,bufferSize,TRUE);
    i = framemessage_AskForStringCompleted(self->dialogLine, 0 ,"", defaultString, buffer, bufferSize, keystate, completionProc, helpProc, completionData, flags);
    while(im_Interact(0));
    --frame_GlobalStackCount;
    retractCursors(self);
    if(!RestoreBits(self))
	CannotRestoreBits(self);
    TidyUp(self);
    if (focus)view_WantInputFocus(focus,focus);
    return i;
}
int frame__MultipleChoiceQuestion(self, priority, prompt, defaultChoice, result, choices, abbrevKeys)
struct frame *self;
int priority;
char *prompt;
long defaultChoice;
long *result;
char **choices;
char *abbrevKeys;
{
    char *mychoices[100];
    int i;
    mychoices[0] = prompt;
    defaultChoice++;
    i = 0;
    do { 
	mychoices[i+1] = choices[i];
	if(choices[i] == '\0') break;
    } while (choices[i++]);
    if(!ButtonInteract(self,mychoices,defaultChoice,result,priority > frame_MUSTANSWER)){
	(*result)--;
	return(0);
    }
    return ERROR;
}

boolean frame__InitializeClass(classID)
    struct classheader *classID;
{
    char c[2], *s;
    mykm = keymap_New();
    returnconsidered = proctable_DefineProc("frame-consider-coming",  ConsiderReturning, &frame_classinfo, NULL, "consider selecting an answer");
    cancel = proctable_DefineProc("frame-cancel",  Cancel, &frame_classinfo, NULL, "Try to cancel dialog box");
    confirmAnswer = proctable_DefineProc("frame-confirm",  ConfirmDefaultAnswer, &frame_classinfo, NULL, "Select the default answer");
    gotkey = proctable_DefineProc("frame-got-key",  GotKey, &frame_classinfo, NULL, "change the default answer");
    c[1] = '\0';
    s = c;
    for (*s = 'a'; *s <= 'z'; ++*s) {
	keymap_BindToKey(mykm, s, gotkey, *s);
    }
    for (*s = 'A'; *s <= 'Z'; ++*s) {
	keymap_BindToKey(mykm, s, gotkey, *s);
    }
    for (*s = '0'; *s <= '9'; ++*s) {
	keymap_BindToKey(mykm, s, gotkey, *s);
    }
    keymap_BindToKey(mykm, "\007", cancel, 0);
    keymap_BindToKey(mykm, "\015", confirmAnswer, 0);
    return TRUE;
}
