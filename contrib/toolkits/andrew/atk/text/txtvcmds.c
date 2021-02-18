/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txtvcmds.c,v 2.32 89/11/02 10:11:39 tpn Exp $ */
/* $ACIS:txtvcmds.c 1.7$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txtvcmds.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txtvcmds.c,v 2.32 89/11/02 10:11:39 tpn Exp $";
#endif /* lint */

#include <class.h>
#define AUXMODULE 1
#include <textv.eh>

#include <text.ih>
#include <keymap.ih>
#include <proctbl.ih>
#include <menulist.ih>
#include <graphic.ih>
#include <im.ih>
#include <message.ih>
#include <search.ih>
#include <view.ih>
#include <filetype.ih>
#include <mark.ih>
#include <envrment.ih>
#include <stylesht.ih>
#include <style.ih>
#include <viewref.ih>
#include <environ.ih>
#include <bind.ih>
#include <dict.ih>
#include <dataobj.ih>
#include <complete.ih>
#include <txtvinfo.h>
#include <fnote.ih>

#include <andrewos.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>

static long lcKill;
static long lcYank;
static long lcMove;
static long lcDisplayEnvironment;

#define Text(v)	(struct text *) ((v)->header.view.dataobject)

#define TEXT_VIEWREFCHAR '\377'

#define textview_GetLines(self) ((self)->nLines)

/* Added safety for views --cm26 */

static boolean ConfirmViewDeletion(self, pos, len)
struct textview *self;
long pos, len;
{
    struct text *d;
    boolean hasViews;
    static char *yesOrNo[] = {"Yes", "No", NULL};
    long answer;

    d = Text(self);

    for (hasViews = FALSE; len--; pos++)
        if (text_GetChar(d, pos) == TEXT_VIEWREFCHAR &&
          environment_GetInnerMost(d->rootEnvironment, pos)->type ==
          environment_View) {
            hasViews = TRUE;
            break;
        }

    if (! hasViews)
        return TRUE;

    if (message_MultipleChoiceQuestion(self, 80,
         "Really delete inset(s)?", 1, &answer,
          yesOrNo, NULL) < 0 || answer != 0) {
        message_DisplayString(self, 0, "Cancelled.\n");
        return FALSE;
    }

    return TRUE;
}

/* Added friendly read-only behavior 04/27/89 --cm26 */

static boolean ConfirmReadOnly(self)
struct textview *self;
{
    if (text_GetReadOnly(Text(self))) {
        message_DisplayString(self, 0,
          "Document is read only.");
        return TRUE;
    } else
        return FALSE;
}

void textview_ForwardWordCmd();
void textview_BackwardWordCmd();
void textview_SelfInsertCmd();
void textview_BeginningOfLineCmd();

void textview_LineToTopCmd(self)
register struct textview *self;
{
    long pos;

    pos = textview_GetDotPosition(self) + textview_GetDotLength(self);
    pos = textview_MoveBack(self, pos, 0, textview_MoveByLines, 0, 0);

    textview_SetTopPosition(self, pos);
    textview_WantUpdate(self, self);
}

void textview_ForwardParaCmd(self)
register struct textview *self;
{
    register int j, ct, pos, dlen;
    register struct text *d;

    j = 0;
    ct = im_Argument(self->header.view.imPtr);
    d = Text(self);
    dlen = text_GetLength(d);
    pos =textview_GetDotPosition(self);
    while (j<ct) {
	while (pos<dlen) {
	    pos++;	/* always move at least one character */
	    if (text_GetChar(d,pos) == '\n') break;
	}
	textview_SetDotPosition(self, pos);
	if (pos < dlen) pos++;
	j++;
    }
    textview_WantUpdate(self, self);
}

void textview_BackwardParaCmd(self)
register struct textview *self;
{
    register int j, ct, pos;
    register struct text *d;

    j = 0;
    ct = im_Argument(self->header.view.imPtr);
    d=Text(self);
    pos=textview_GetDotPosition(self);
    while (j<ct)  {
	if (pos > 0) pos--;
	while (pos>0 && text_GetChar(d,pos-1) != '\n') pos--;
	j++;
    }
    textview_SetDotPosition(self,pos);
    textview_WantUpdate(self, self);
}


void textview_OpenLineCmd(self)
register struct textview *self;
{
    register int i, pos, ct;
    char tc;
    struct text *d;

    if (ConfirmReadOnly(self))
        return;
    textview_CollapseDot(self);
    d=Text(self);
    pos = textview_GetDotPosition(self);
    tc = '\012';
    i = 0;
    ct = im_Argument(self->header.view.imPtr);
    while (i < ct) {
	text_InsertCharacters(d,pos,&tc,1);
	i++;
    }
    text_NotifyObservers(d, 0);
}

/* This is no longer a command you can type;
 * it is now called from a menu item */

void textview__LookCmd(self, look)
register struct textview *self;
int look;
{
    register struct text *d;
    int s, len;
    struct style *styleptr;

    if (ConfirmReadOnly(self))
        return;
    d = Text(self);
    s = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);
    if (len == 0)
        return;     /* Nothing to do */
    styleptr = d->styleSheet->styles[look];
    if (text_AddStyle(d, s, len, styleptr) == NULL)
	message_DisplayString(self, 0,
            "Sorry; can't add style.");
    else
	text_NotifyObservers(d, 0);
}

void textview_PlainerCmd(self)
register struct textview *self;
{
    register struct text *d;
    register struct environment *env;
    int pos, len;

    if (ConfirmReadOnly(self))
        return;
    d = Text(self);
    pos = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);
    if (im_GetLastCmd(self->header.view.imPtr) == lcDisplayEnvironment && (env = self->displayEnvironment) != NULL && env != d->rootEnvironment)  {
	pos = environment_Eval(env);
	len = env->header.nestedmark.length;
	environment_Delete(env);
	text_SetModified(d);
	im_SetLastCmd(self->header.view.imPtr, lcDisplayEnvironment);
	self->displayEnvironment = NULL;
	message_DisplayString(self, 0, "");
    }
    else if (len == 0) {
	env = environment_GetInnerMost(d->rootEnvironment, pos);
	if (env == NULL  || env == d->rootEnvironment) return;
	pos = environment_Eval(env);
	len = env->header.nestedmark.length;
	environment_Delete(env);
	text_SetModified(d);
    }
    else {
	if (environment_Remove(d->rootEnvironment, pos, len, environment_Style, FALSE))
	    text_SetModified(d);
    }
    text_RegionModified(d, pos, len);
    text_NotifyObservers(d, 0);
}

void textview_PlainestCmd(self)
register struct textview *self;
{
    register struct text *d;
    int pos, len;

    if (ConfirmReadOnly(self))
        return;
    d = Text(self);
    pos = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);
    if (len == 0)
	return;
    else if (environment_Remove(d->rootEnvironment,
       pos, len, environment_Style, TRUE))
	text_SetModified(d);
    text_RegionModified(d, pos, len);
    text_NotifyObservers(d, 0);
}

static objecttest(self,name,desiredname)
register struct textview *self;
char *name,*desiredname;
{
    if(class_Load(name) == NULL){
        char foo[640];
        sprintf(foo,"Can't load %s",name);
         message_DisplayString(self, 0, foo);
        return(FALSE);
    }
    if(! class_IsTypeByName(name,desiredname)){
        char foo[640];
        sprintf(foo,"%s is not a %s",name,desiredname);
         message_DisplayString(self, 0, foo);
        return(FALSE);
    }
    return(TRUE);
}

void textview_InsertInsetCmd (self)
register struct textview *self;
{
    char iname[100];
    char viewname[200];
    long pf;
    boolean promptforname = im_ArgProvided(view_GetIM((&self->header.view)));
    im_ClearArg(self->header.view.imPtr);    
    viewname[0] = '\0';
    if (ConfirmReadOnly(self))
        return;
    if(text_GetObjectInsertionFlag(Text(self)) == FALSE){
	message_DisplayString(self, 0, "Object Insertion Not Allowed!");
	return;
    }
    pf = message_AskForString(self, 0, "Data object to insert here: ", 0, iname, sizeof(iname));
    if (pf < 0){
	message_DisplayString(self, 0, "Punt!");
	return;
    }
    if(strlen(iname)==0){
	message_DisplayString(self, 0, "No name specified");
	return;
    }
    if(objecttest(self,iname,"dataobject") == FALSE) return;
    if(promptforname){
        if( message_AskForString (self, 0, "View to place here ", 0, viewname, 200) < 0) return;
        if(objecttest(self,viewname,"view") == FALSE) return;
    }
    self->currentViewreference = text_InsertObject(Text(self), textview_GetDotPosition(self), iname,viewname);
    text_NotifyObservers(Text(self), 0);
}

/* Inserts a lookz view in front of the current paragraph. */

void textview_ExposeStyleEditor(self)
struct textview *self;
{
    register int pos;
    register struct text *d;

    if(objecttest(self, "lookzview", "view") == FALSE)
        return;
    if(objecttest(self, "lookz", "dataobject") == FALSE)
        return;
    if(text_GetObjectInsertionFlag(Text(self)) == FALSE){
	message_DisplayString(self, 0, "Object Insertion Not Allowed!");
	return;
    }
    d = Text(self);
    /* Find the beginning of the current paragraph. */
    pos = textview_GetDotPosition(self) + textview_GetDotLength(self);
    while (pos > 0 && text_GetChar(d, pos) != '\n')
        pos--;
    /* If we can insert it between two carriage returns (or at the beginning of the
         * document) do so, otherwise insert it at the start of this paragraph. */
    if (pos > 0 && text_GetChar(d, pos - 1) != '\n')
	pos += 1;
    self->currentViewreference =
          text_InsertObject(Text(self), pos, "lookz", "lookzview");
    textview_FrameDot(self, pos);
    text_NotifyObservers(d, 0);
}

void textview_GotoParagraphCmd(self)
register struct textview *self;
{
    char temp[100];
    int line, len, gf;
    register int i, pos;
    register struct text *d;

    d = Text(self);

    if (im_ArgProvided(self->header.view.imPtr))
        line = im_Argument(self->header.view.imPtr);
    else {
        gf = message_AskForString(self, 0, "What paragraph? ", 0, temp, 100);
        if (gf < 0)
            return;
        line = atoi(temp);
    }

    len = text_GetLength(d);
    pos = 0;

    i = 1;
    while (i < line) {
	if (pos >= len)
            break;
	if (text_GetChar(d, pos) == '\012')
            i++;
        pos++;
    }

    textview_SetDotPosition(self, pos);
    textview_SetDotLength(self, 0);
    textview_FrameDot(self, pos);
    textview_WantUpdate(self, self);
}

void textview_WhatParagraphCmd (v)
register struct textview *v;
{
    char temp[100];
    register int i, pos;
    register struct text *d;

    d=Text(v);
    pos = textview_GetDotPosition(v)-1;
    i=1;
    while (pos >= 0) {
        if (text_GetChar(d,pos) == '\012')
            i++;
        pos--;
    }
    sprintf(temp,"Paragraph %d.",i);
    message_DisplayString(v, 0, temp);
}

/* Serach command statics. */

char *lastPattern = NULL;
static char searchString[100] = "";
boolean forwardSearch = TRUE; /* TRUE if last search was forward. */

int textview_SearchCmd(self)
register struct textview *self;
{
    char thisString[100], *tp, messageBuf[120];
    int pos = 0, gf, ct;
    register int j;
    register struct text *d;

    d = Text(self);
    ct = im_Argument(self->header.view.imPtr);
    if (*searchString != '\0')
        sprintf(messageBuf, "Search for [%s] : ", searchString);
    else
        strcpy(messageBuf,  "Search for: ");
    gf = message_AskForString(self, 0, messageBuf, NULL, thisString, sizeof(thisString));
    if (gf < 0) return(-1);
    textview_SetDotPosition(self,
       textview_GetDotPosition(self)+textview_GetDotLength(self));
    if (thisString[0] != '\000') {
        tp = search_CompilePattern(thisString,&lastPattern);
	if (tp != 0) {
            message_DisplayString(self, 0, tp);
	    return(-2);
        }
        strcpy(searchString, thisString);
    }
    j = 0;
    while (j<ct) {
        pos = search_MatchPattern(d,
            textview_GetDotPosition(self),lastPattern);
	if (pos < 0) {
            textview_SetDotLength(self,0);
            message_DisplayString(self, 0, "Search failed");
            return(-3);
        }
        textview_SetDotPosition(self,pos);
        textview_SetDotLength(self, search_GetMatchLength());
        j++;
    }
    textview_FrameDot(self,pos);
    textview_WantUpdate(self, self);
    forwardSearch = TRUE;
    return(0);
}

int textview_RSearchCmd(self)
register struct textview *self;
{
    int ct, gf, orgpos, pos = 0;
    register int j;
    register struct text *d;
    char thisString[100], *tp, messageBuf[130];

    d = Text(self);
    orgpos = textview_GetDotPosition(self);
    ct = im_Argument(self->header.view.imPtr);
    if (*searchString != '\0')
        sprintf(messageBuf, "Reverse search for [%s] : ", searchString);
    else
        strcpy(messageBuf,  "Reverse search for: ");
    gf=message_AskForString(self, 0, messageBuf, NULL, thisString, sizeof(thisString));
    if (gf < 0)
        return -1;
    textview_SetDotLength(self,0);
    if (orgpos > 0) textview_SetDotPosition(self,orgpos-1);
    if (thisString[0] != '\000') {
	tp = search_CompilePattern(thisString,&lastPattern);
        if (tp != 0) {
            message_DisplayString(self, 0, tp);
            return -2;
        }
        strcpy(searchString, thisString);
    }
    j=0;
    while (j<ct) {
        pos = search_MatchPatternReverse(d,
           textview_GetDotPosition(self),lastPattern);
	if (pos < 0) {
            textview_SetDotPosition(self,orgpos);
            message_DisplayString(self, 0, "Search failed");
            return -3;
        }
        textview_SetDotPosition(self,pos);
        textview_SetDotLength(self, search_GetMatchLength());
        j++;
    }
    textview_FrameDot(self,pos);
    textview_WantUpdate(self, self);
    forwardSearch = FALSE;
    return 0;
}

void textview_SearchAgain(self)
struct textview *self;
{
    struct text *d = Text(self);

    if (lastPattern != NULL) {
        long pos = textview_GetDotPosition(self);

	if (forwardSearch) {
	    pos = textview_GetDotPosition(self) + textview_GetDotLength(self);
            textview_SetDotPosition(self, pos);
            textview_SetDotLength(self, 0);
            pos = search_MatchPattern(d, pos, lastPattern);
        }
        else {
            textview_SetDotLength(self, 0);
            if (pos > 0)
		textview_SetDotPosition(self, --pos);
            pos = search_MatchPatternReverse(d, pos, lastPattern);
        }
        if (pos < 0)
            message_DisplayString(self, 0, "Search failed.");
        else {
            textview_SetDotPosition(self,pos);
            textview_SetDotLength(self, search_GetMatchLength());
            textview_FrameDot(self,pos);
            textview_WantUpdate(self, self);
        }
    }
    else
        message_DisplayString(self, 0, "Must have searched at least once to search again.");
}

void textview_GlitchUpCmd(self)
register struct textview *self;
{
    register int n;
    register int pos;

    n = im_Argument(self->header.view.imPtr);
    pos = textview_GetTopPosition(self);
    pos = textview_MoveBack(self, pos, 0, textview_MoveByLines, 0, 0);
    pos = textview_MoveForward(self, pos, n, textview_MoveByLines, 0, 0);
    if (self->scroll == textview_ScrollBackward)
        self->scroll = textview_MultipleScroll;
    textview_SetTopPosition(self, pos);
    textview_WantUpdate(self, self);
}

void textview_GlitchDownCmd(self)
register struct textview *self;
{
    register int n,pos;
    int dist, lines;

    n = im_Argument(self->header.view.imPtr);
    pos = textview_GetTopPosition(self);
    pos = textview_MoveBack(self, pos, n, textview_MoveByLines, &dist, &lines);
    if (self->scroll == textview_ScrollForward)
	self->scroll = textview_MultipleScroll;
    else {
	if (self->scrollDist == -1) {
	    self->scrollDist = dist;
	    self->scrollLine = lines;
	}
	else {
	    self->scrollDist += dist;
	    if (self->scrollDist >= textview_GetLogicalHeight(self))
		self->scrollDist = -1;
	    else
		self->scrollLine += lines;
	}
    }
	
    textview_SetTopPosition(self, pos);
    textview_WantUpdate(self, self);
}

/* Number of lines below which we use one line overlaps instead of two line overlaps. */

#define SMALLWINDOW 4

void textview_NextScreenCmd(self)
struct textview *self;
{
    int argument = im_Argument(self->header.view.imPtr);
    int count;

    im_ClearArg(self->header.view.imPtr);
    for (count = 0; count < argument; ++count) {

        int numLines = textview_GetLines(self);
        long pos;
        long viewHeight = textview_GetLogicalHeight(self) - self->by;

        if (numLines == 0) /* Do nothing is no text on the screen. */
            return;

        if (numLines == 1)
            textview_GlitchUpCmd(self);
        else {
            /* If more than SMALLWINDOW lines on the screen. */
            if (numLines > SMALLWINDOW ||
                 (self->lines[numLines - 1].y + 2 * self->lines[numLines - 1].height) < viewHeight)
                numLines -= 2;
            else
                --numLines;
            pos = textview_GetTopPosition(self);
            pos = textview_MoveBack(self, pos, 0, textview_MoveByLines, 0, 0);		/* get line aligned */
            pos = textview_MoveForward(self, pos, numLines, textview_MoveByLines, 0, 0);	/* move forward */
            if (self->scroll == textview_ScrollBackward)
                self->scroll = textview_MultipleScroll;
            textview_SetTopPosition(self, pos);
        }
    }
    textview_WantUpdate(self, self);
}

void textview_PrevScreenCmd(self)
register struct textview *self;
{

    int argument = im_Argument(self->header.view.imPtr);
    int count;

    im_ClearArg(self->header.view.imPtr);

    for (count = 0; count < argument; ++count) {
        long numLines = textview_GetLines(self);
        long pos;
        long viewHeight = textview_GetLogicalHeight(self) - self->by;
        long dist;
        long lines;
        long overlapHeight;

        if (numLines == 1)
            textview_GlitchDownCmd(self);
        else {
            /* If more than SMALLWINDOW lines on the screen. */
            if (numLines > SMALLWINDOW || (numLines > 1) &&
                 (self->lines[numLines - 1].y + 2 * self->lines[numLines - 1].height) < viewHeight)
                overlapHeight = self->lines[1].y + self->lines[1].height;
            else if (numLines > 0)
                overlapHeight = self->lines[0].y + self->lines[0].height;
            else
                overlapHeight = 0;

            pos = textview_GetTopPosition(self);
            pos = textview_MoveBack(self, pos, viewHeight - overlapHeight, textview_MoveByPixels, &dist, &lines);

            if (self->scroll == textview_ScrollForward)
                self->scroll = textview_MultipleScroll;
            else if (self->scrollDist == -1)  {
                self->scrollDist = dist;
                self->scrollLine = lines;
            }
            else  {
                self->scrollDist = -1;
            }

            textview_SetTopPosition(self,pos);
        }
    }
    textview_WantUpdate(self, self);
}

#ifdef CONVERSIONERROR
void textview_PrintFile(self)
struct textview *self;
{
    register struct text *d = Text(self);
    register struct buffer *b = (struct buffer *) buffer_FindData(d);
    char *filename = b->fname;
    
    if (! filename || ! filename[0]) {
	filename = b->bname;
	if (! filename || ! filename[0])
	    filename = "unknown";
    }
    
    doctroff_PrintDoc(d, 1, 1, filename, "");
}

void textview_PreviewCmd(self)
register struct textview *self;
{
    register struct text *d = Text(self);
    register struct buffer *b = buffer_finddata(d);
    char *filename = b->fname;
    
    if (! filename || ! filename[0])  {
	filename = b->bname;
	if (! filename || ! filename[0])
	    filename = "unknown";
    }
    
    doctroff_PrintDoc(d, 0, 1, filename, "");
}

void textview_SetPrinterCmd(self)
struct textview *self;
{
    char p[200];
    char *currentprinter;
    char str[300];
    
    currentprinter = (char *) environ_Get("PRINTER");
    if (currentprinter == NULL)
        currentprinter = environ_GetProfile("print.spooldir");
    if (message_AskForString(self, 0,
            "Set Printer to: ", currentprinter, p, 200) < 0) {
	if (p[0] == '\0') {
	    environ_Delete("PRINTER");
	    strcpy(p, environ_GetProfile("print.spooldir"));
	}
	else
	    environ_Put("PRINTER", p);
    }
    else
	strcpy(p, currentprinter);
    sprintf(str, "Printer now set to %s", p); 
    message_DisplayString(self, 0, str);
}
#endif /* CONVERSIONERROR */

#define SHOWSIZE 250

void textview_ShowStylesCmd(self)
register struct textview *self;
{
    char tbuf[SHOWSIZE];
    register char *tp;
    struct environment *te;
    struct environment *de = NULL;
    struct text *d;
    int curLen, flag;
    long pos;

    curLen = 1;		/* the null at the end */
    if (im_GetLastCmd(self->header.view.imPtr) == lcDisplayEnvironment) {
	de = self->displayEnvironment;
	pos = self->displayEnvironmentPosition;
    }
    else {
	textview_CollapseDot(self);
	pos = textview_GetDotPosition(self);
    }

    strcpy(tbuf, "Styles: ");
    flag = 0;
    d = Text(self);
    te = d->rootEnvironment;	/* now its the root */
    if (te)
	te = environment_GetEnclosing(te, pos);	/* now its not */
    if (te) {
	if (te == d->rootEnvironment) {
	    strcat(tbuf, "None");
	}
	else {
	    while (te != d->rootEnvironment) {
		tp = te->data.style->name;
		
		if(tp == NULL) tp = "(Unnamed Style)";
		if (flag)
		    strcat(tbuf, " <- ");

		if (de == NULL)
		    de = te;
		else if (de == te)
		    de = NULL;

		curLen += 4+strlen(tp);
		if (curLen >= SHOWSIZE) break;
		if (de == te)
		    strcat(tbuf, "**");
		strcat(tbuf,tp);
		if (de == te)  {
		    strcat(tbuf, "**");
		    textview_SetDotPosition(self, environment_Eval(te));
		    textview_SetDotLength(self, environment_GetLength(te));
		}
		te = (struct environment *) te->header.nestedmark.parent;
		flag = 1;
	    }
	}
	if (de == NULL)  {
	    textview_SetDotPosition(self, pos);
	    textview_SetDotLength(self, 0);
	}
	message_DisplayString(self, 0, tbuf);
	im_SetLastCmd(self->header.view.imPtr, lcDisplayEnvironment);
	self->displayEnvironment = de;
	self->displayEnvironmentPosition = pos;
    }
    else message_DisplayString(self, 0, "Error");
    textview_WantUpdate(self, self);
}

void textview_QueryReplaceCmd(self)
struct textview *self;
{
    char replacement[100], casedString[100];
    char *searchError;
    int reply;
    boolean keepAsking = TRUE;
    boolean keepRunning = TRUE;
    long originalPos;
    long originalLength;
    struct text *d;
    int numFound = 0;
    int numReplaced = 0;
    static int expertReplace = -999999;
    long searchPos;
    int replacementLen;
    long fencePos;
    long pos;
    char promptBuf[120];
    char *prompt;
    boolean defaultExists;
    struct mark *area;

    if (ConfirmReadOnly(self))
        return;
    if (expertReplace == -999999)
        expertReplace = environ_GetProfileSwitch("ExpertMode", 0);

    keepAsking = 1;
    keepRunning = 1;
    d = Text(self);

    defaultExists = *searchString != '\0';
    if (defaultExists) {
        sprintf(promptBuf, "Replace [%s] : ", searchString);
        prompt = promptBuf;
    }
    else
        prompt = "Replace: ";
    if (message_AskForString(self, 0, prompt, NULL, searchString, sizeof(searchString)) < 0)
        return;

    if (message_AskForString(self, 0, "New string: ", NULL, replacement, sizeof(replacement)) < 0)
        return;
    replacementLen = strlen(replacement);

    if (!defaultExists || *searchString != '\0')
        if ((searchError =
          search_CompilePattern(searchString, &lastPattern)) != NULL) {
            message_DisplayString(self, 0, searchError);
            return;
        }

    originalPos = searchPos = textview_GetDotPosition(self);
    originalLength = textview_GetDotLength(self);

    if (originalLength != 0)
        area = text_CreateMark(d, originalPos, originalLength);
    else
        area = NULL;

    if (searchPos < (fencePos = text_GetFence(d)))
        searchPos = fencePos;

    while ((pos = search_MatchPattern(d, searchPos, lastPattern)) >= 0 && keepRunning && (area == NULL || pos < mark_GetPos(area) + mark_GetLength(area)))  {

        long matchLen = search_GetMatchLength(); /* length can change between matches on RE searches */

        ++numFound;

        textview_SetDotPosition(self, pos);
        textview_SetDotLength(self, matchLen);
        textview_FrameDot(self, pos);

        if (keepAsking) {
            textview_Update(self);
            reply = im_GetCharacter(textview_GetIM(self));
        }
        else
            reply = ' ';

        switch (reply) {
            int i;

            case 'q':
            case '\007':
            case EOF:		/* Interrupted im_GetChar */
                keepRunning = FALSE;
                break;
            case '-':
                if (isupper(text_GetChar(d, pos))) {
                    for (i = 1; i < matchLen; i++)
                        if (islower(text_GetChar(d, pos + i)))
                            break;
                    if (i >= matchLen) {
                        for (i = 0; i < replacementLen; i++)
                            if (islower(replacement[i]))
                                casedString[i] = toupper(replacement[i]);
                            else
                                casedString[i] = replacement[i];
                        casedString[i] = '\0';
                    }
                    else { /* Upcase first letter. */
                        for (i = 0; i < replacementLen; i++)
                            casedString[i] = replacement[i];
                        casedString[i] = '\0';
                        if (islower(replacement[0]))
                            casedString[0] = toupper(replacement[0]);
                        else
                            casedString[i] = replacement[0];
                    }
                }
                else
                    strncpy(casedString, replacement, replacementLen);
                text_ReplaceCharacters(d, pos, matchLen, casedString, replacementLen);
                searchPos = pos + replacementLen;
                ++numReplaced;
                break;
            case '!':
            case '.':
                if (reply == '!')
                    keepAsking = FALSE;
                else
                    keepRunning = FALSE;
                /* Fall through. */
            case ' ':
                text_ReplaceCharacters(d, pos, matchLen, replacement, replacementLen);
                searchPos = pos + replacementLen;
                ++numReplaced;
                break;
#if 0 /* This does not seem to work. */
            case 'e':
                im_DoMacro(textview_GetIM(self));
                while (im_Interact(0))
                    ;	/* let the macro execute */
                break;
#endif /* 0  */
            case 'n':
                searchPos = pos + matchLen;
                break;
            case 'r':
                if (expertReplace)  {
                    message_DisplayString(self, 0, "Recursive editing; ^C exits.");
                    im_KeyboardProcessor();
                    break;
                }
                /* Otherwise, fall through to the default */
            default:
                message_DisplayString(self, 0, expertReplace ?
                   "One of ' ', '.', 'n', 'r', '!', 'q' '-'" :
                   "One of ' ', '.', 'n', '!', 'q' '-'");
                im_ForceUpdate();
                break;
        }

        if (keepAsking)
            textview_Update(self);
    }

    if (numFound > 0)  {
        char messageString[100];

        if (area != NULL) {
            textview_SetDotPosition(self, mark_GetPos(area));
            textview_SetDotLength(self, mark_GetLength(area));
        } else {
            textview_SetDotPosition(self, originalPos);
            textview_SetDotLength(self, originalLength);
        }
	textview_FrameDot(self, originalPos);
        text_NotifyObservers(d, 0);
        sprintf(messageString, "Found %d occurrences; replaced %d.", numFound, numReplaced);
        message_DisplayString(self, 0, messageString);
    }
    else
        message_DisplayString(self, 0, "No occurrences found.");

    if (area != NULL) {
        text_RemoveMark(d, area);
        mark_Destroy(area);
    }

    return;
}

void textview_QuoteCmd(self)
register struct textview *self;
{
    register long i;
    long count;
    register struct text *d;
    char tc;
    long where;

    if (ConfirmReadOnly(self))
        return;
    count = im_Argument(self->header.view.imPtr);
    d = Text(self);
    tc = im_GetCharacter(self->header.view.imPtr);
    if (tc >= '0' && tc <= '7') {
        char c1 = im_GetCharacter(self->header.view.imPtr) - '0',
             c2 = im_GetCharacter(self->header.view.imPtr) - '0';
        tc = ((tc - '0' << 3) + c1 << 3) + c2;
    }
    where = textview_GetDotPosition(self);
    for(i=0;i<count;i++)
	text_InsertCharacters(d, where+i, &tc, 1);
    textview_SetDotPosition(self, where+count);
    text_NotifyObservers(d, 0);
}

void textview_InsertFile(self)
struct textview *self;
{
    char filename[MAXPATHLEN];
    FILE *inputFile;
    long initialPos;
    long pos;
    struct stat buf;
    
    if (ConfirmReadOnly(self))
        return;
    if (im_GetDirectory(filename) != NULL) /* Use CWD for now */
        strcat(filename, "/");
    if (completion_GetFilename(self, "Insert file: ", filename, filename, sizeof(filename), FALSE, TRUE) == -1 )
        return;
    stat(filename,&buf);
    if(buf.st_mode & S_IFDIR) {
	message_DisplayString(self, 0, "Can't insert a directory"); 
	return;
    }
    if ((inputFile = fopen(filename, "r")) == NULL) {
        message_DisplayString(self, 0, "Could not insert file."); /* Really should give a more informative error message. */
        return;
    }

    textview_CollapseDot(self);
    initialPos = pos = textview_GetDotPosition(self);
    text_InsertFile(Text(self), inputFile,filename, pos);

    fclose(inputFile);

    textview_SetDotPosition(self,initialPos);
    textview_SetDotLength(self, pos - initialPos);
    im_SetLastCmd(self->header.view.imPtr, lcYank);
    text_NotifyObservers(Text(self), self);
}

void textview_YankCmd(self)
register struct textview *self;
{
    long ct;
    long initialPos;
    long pos;
    struct text *d;
    FILE *pasteFile;

    if (ConfirmReadOnly(self))
        return;
    ct = im_Argument(self->header.view.imPtr);
    if (ct > 100) {
        message_DisplayString(self, 0, "Yank argument limit: 100");
        return;
    }

    d = Text(self);

    textview_CollapseDot(self);
    initialPos = pos = textview_GetDotPosition(self);

    while (ct--) {
	pasteFile = im_FromCutBuffer(textview_GetIM(self));
        pos += text_InsertFile(d, pasteFile, NULL, pos);
	im_CloseFromCutBuffer(textview_GetIM(self), pasteFile);
    }

    textview_SetDotPosition(self,initialPos);
    textview_SetDotLength(self, pos - initialPos);
    im_SetLastCmd(self->header.view.imPtr, lcYank);
    text_NotifyObservers(d, self);
}

static textview_DoRotatePaste(self, count)
register struct textview *self;
int count;
{
    register struct text *d = Text(self);

    im_ClearArg(self->header.view.imPtr);        /* Make it safe to call yank. */

    if (ConfirmReadOnly(self))
        return;
    text_DeleteCharacters(d,		/* Get rid of what is there now. */
	textview_GetDotPosition(self), textview_GetDotLength(self));
    if (im_GetLastCmd(self->header.view.imPtr) != lcYank)    /* If not following yank. */
        count--;    /* Make it get top thing off of ring, instead of one down on ring. */
    im_RotateCutBuffers(textview_GetIM(self), count);    /* Put the ring in the right place. */
    textview_YankCmd(self);    /* Snag it in off the ring. */
}


void textview_BackwardsRotatePasteCmd(self)
struct textview *self;
{
    textview_DoRotatePaste(self, - im_Argument(self->header.view.imPtr));
}

void textview_RotatePasteCmd(self)
struct textview *self;
{
    textview_DoRotatePaste(self, im_Argument(self->header.view.imPtr));
}

void textview_InsertNLCmd(self)
register struct textview *self;
{
    textview_SelfInsertCmd(self,'\012');
}

static int stringmatch(d,pos,c)
register struct text *d;
register long pos;
register char *c;
{
    /* Tests if the text begins with the given string */
    while(*c != '\0') {
        if(text_GetChar(d,pos) != *c) return FALSE;
        pos++; c++;
    }
    return TRUE;
}

static void DoCopyRegion(self, pos, len, appendFlag)
struct textview *self;
long pos, len;
boolean appendFlag;
{
    struct text *d;
    register long nextChange;
    FILE *cutFile;
    int UseDataStream;

    d = Text(self);
    environment_GetInnerMost(d->rootEnvironment, pos);
    nextChange = environment_GetNextChange(d->rootEnvironment, pos);

    cutFile = im_ToCutBuffer(textview_GetIM(self));
    if (UseDataStream = ((nextChange <= len|| stringmatch(d,pos,"\\begindata")) && text_GetExportEnvironments(d)))
	fprintf(cutFile, "\\begindata{%s, %d}\n",
		(d->CopyAsText)?"text": class_GetTypeName(d),
		/* d->header.dataobject.id */ 999999);
    d->header.dataobject.writeID = im_GetWriteID();
    text_WriteSubString(d, pos, len, cutFile, UseDataStream);
    
    if (UseDataStream)
	fprintf(cutFile, "\\enddata{%s,%d}\n", 
		(d->CopyAsText)?"text": class_GetTypeName(d), /* d->header.dataobject.id */ 999999);

    if (appendFlag)
	im_AppendToCutBuffer(textview_GetIM(self), cutFile);
    else
	im_CloseToCutBuffer(textview_GetIM(self), cutFile);
}

void textview_ZapRegionCmd(self)
register struct textview *self;
{
    long pos, len;

    if (ConfirmReadOnly(self))
        return;
    pos = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);

    DoCopyRegion(self, pos, len, FALSE);
    text_DeleteCharacters(Text(self), pos, len);
    textview_SetDotLength(self, 0);
    im_SetLastCmd(self->header.view.imPtr, lcKill);
    text_NotifyObservers(Text(self), 0);
}

/* In order to safely allow killing multiple lines of text, each possibly */
/* including ATK objects, the cutbuffer contents are pasted back and then */
/* a larger area is re-cut.  This isn't visible and wouldn't be so bad were */
/* it not for the extreme inefficiency of cutbuffer transfers. */

void textview_KillLineCmd(self)
register struct textview *self;
{
    register int count, pos, endpos, lastpos;
    register struct text *d;

    if (ConfirmReadOnly(self))
        return;
    d = Text(self);
    count = im_Argument(self->header.view.imPtr);
    textview_CollapseDot(self);
    endpos = pos = textview_GetDotPosition(self);
    lastpos = text_GetLength(d);

    while (count-- > 0 && endpos < lastpos) {
        if (text_GetChar(d, endpos) == '\012')
            endpos++;
        else
            while (text_GetChar(d, endpos) != '\012' && endpos < lastpos)
                endpos++;
    }

    if (im_GetLastCmd(self->header.view.imPtr) == lcKill) {
        FILE *pasteFile;
        pasteFile = im_FromCutBuffer(textview_GetIM(self));
        endpos += text_InsertFile(d, pasteFile, NULL, pos);
        im_CloseFromCutBuffer(textview_GetIM(self), pasteFile);
    }

    if (endpos > pos) {
        DoCopyRegion(self, pos, endpos - pos, FALSE);
        text_DeleteCharacters(Text(self), pos, endpos - pos);
        textview_SetDotLength(self, 0);
        im_SetLastCmd(self->header.view.imPtr, lcKill);
        text_NotifyObservers(Text(self), 0);
    }
}

/* This routine will, if immediately followed by a text cutting commnd, cause
 * the cut to append to the current buffer instead of placing it in a new
 * buffer. */

void textview_AppendNextCut(self)
struct textview *self;
{
    im_SetLastCmd(self->header.view.imPtr, lcKill);	/* mark us as a text killing command */
}

void textview_CopyRegionCmd (self)
register struct textview *self;
{
    DoCopyRegion(self,
      textview_GetDotPosition(self),
      textview_GetDotLength(self), FALSE);
}

void textview_GetToCol (self, col)
register struct textview *self;
register int col;
{
    register struct text *d;
    register int pos;

    d = Text(self);
    pos = textview_GetDotPosition (self);
    while (col > 0)  {
	if (col >= 8) {
	    col -= 8;
	    text_InsertCharacters (d,pos,"\011",1);
	}
	else  {
	    col--;
	    text_InsertCharacters(d,pos," ",1);
	}
	pos++;
    }
    textview_SetDotPosition (self,pos);
    text_NotifyObservers(d, 0);
}

void textview_StartOfParaCmd (self)
register struct textview *self;
{
    register struct text *d;
    register int pos;

    d = Text(self);
    pos = textview_GetDotPosition(self);
    while (--pos >= 0 && text_GetChar(d, pos) != '\n');
    ++pos;
    textview_SetDotPosition(self, pos);
    textview_SetDotLength(self, 0);
    view_WantUpdate((struct view *)self, self);
}

void textview_EndOfParaCmd (self)
register struct textview *self;
{
    register struct text *d;
    register int pos;
    register int dlen;

    d = Text(self);
    pos = textview_GetDotPosition(self);
    dlen = text_GetLength(d);
    while (pos < dlen && text_GetChar(d, pos) != '\n')
	++pos;
    textview_SetDotPosition(self, pos);
    textview_SetDotLength(self, 0);
    view_WantUpdate((struct view *) self, self);
}

void textview_MyLfCmd(self)
register struct textview *self;
{
    register int indentation;

    if (ConfirmReadOnly(self))
        return;
    textview_StartOfParaCmd(self);
    indentation = textview_GetSpace (self,textview_GetDotPosition(self));
    textview_EndOfParaCmd(self);
    textview_InsertNLCmd(self);
    textview_GetToCol(self,indentation);
}

void textview_DeleteCmd (self)
register struct textview *self;
{
    register struct text *d;
    register int pos, len;

    if (ConfirmReadOnly(self))
        return;
    d = Text(self);
    textview_CollapseDot(self);

    pos = textview_GetDotPosition(self);
    len = im_Argument(self->header.view.imPtr);

    if (ConfirmViewDeletion(self, pos, len)) {
        text_DeleteCharacters(d, pos, len);
        textview_FrameDot(self, pos);
        text_NotifyObservers(d, 0);
    }
}

void textview_KillWhiteSpaceCmd(self)
register struct textview *self;
{
    register struct text *d;
    register int p, tc, ep;

    /* First move back until no longer looking at whitespace */
    /* Then delete forward white space. */

    if (ConfirmReadOnly(self))
        return;
    d = Text(self);
    textview_CollapseDot(self);
    p = textview_GetDotPosition(self);
    while (p > 0)  {
	tc = 0xff & text_GetChar (d,p-1);
	if (tc == 9 || tc == 32) 
	    p--;
	else
	    break;
    }
    textview_SetDotPosition(self,p);
    for (ep = p;
       ((tc = text_GetChar (d,ep)) != EOF) && (tc =='\t' || tc == ' '); ep++)
	;
    text_DeleteCharacters(d, p, ep - p);
    text_NotifyObservers(d, 0);
}

int textview_GetSpace(self, pos)
struct textview *self;
register int pos;
{
    register int rval;
    register int tc;
    register struct text *d;
    register long len;
    
    d = Text(self);
    rval = 0;
    len = text_GetLength(d);
    while (pos < len)  {
	tc = 0xff & text_GetChar(d,pos);
	if (tc == 32) rval++;
	else if (tc == 9)
	    rval += 8;
	else
	    return rval;
	pos++;
    }
    return rval;
}

static void AdjustIndentation(self, amount)
struct textview *self;
{
    int indentation;

    if (ConfirmReadOnly(self))
        return;

    textview_StartOfParaCmd(self);
    indentation = textview_GetSpace(self,
             textview_GetDotPosition(self));
    textview_KillWhiteSpaceCmd(self);
    indentation += amount;
    textview_GetToCol(self, indentation);
    textview_EndOfParaCmd(self);
}

void textview_UnindentCmd(self)
struct textview *self;
{
    AdjustIndentation(self, -4);
}

void textview_IndentCmd(self)
struct textview *self;
{
    AdjustIndentation(self, 4);
}

void textview_SelectRegionCmd(self)
register struct textview *self;
{
    register int i;
    register int dot, mark;

    mark = mark_GetPos(self->atMarker);
    dot = textview_GetDotPosition(self);
    if (mark > dot) {
	i = dot;
	dot = mark;
	mark = i;
    }
    /* Now assume that mark <= dot */
    textview_SetDotLength(self,dot-mark);
    textview_SetDotPosition(self,mark);
    textview_WantUpdate(self, self);
}

void textview_ExchCmd(self)
register struct textview *self;
{
    register long p;
    register long len;

    p = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);
    textview_SetDotPosition(self, mark_GetPos(self->atMarker));
    textview_SetDotLength(self, mark_GetLength(self->atMarker));
    mark_SetPos(self->atMarker,p);
    mark_SetLength(self->atMarker, len);
    textview_FrameDot(self, textview_GetDotPosition(self));
    textview_WantUpdate(self, self);
}

void textview_CtrlAtCmd(self)
register struct textview *self;
{
    mark_SetPos(self->atMarker,textview_GetDotPosition(self));
    mark_SetLength(self->atMarker,textview_GetDotLength(self));
    message_DisplayString(self, 0, "Mark set.");
}

void textview_SelfInsertCmd(self, a)
register struct textview *self;
char a;
{
    register int ct, i, pos;
    register struct text *d;

    if (ConfirmReadOnly(self))
        return;

    d = Text(self);
    ct=im_Argument(self->header.view.imPtr);
    textview_CollapseDot(self);
    pos=textview_GetDotPosition(self);
    for (i = 0; i < ct; i++)
	text_InsertCharacters(d,pos++,&a,1);
    textview_SetDotPosition(self,pos);
    textview_FrameDot(self, pos);
    text_NotifyObservers(d, 0);
}

void textview_RuboutCmd (self)
register struct textview *self;
{
    register long endpos, len;
    register struct text *d;

    if (ConfirmReadOnly(self))
        return;

    textview_CollapseDot(self);
    d = Text(self);

    endpos = textview_GetDotPosition(self);
    len = im_Argument(self->header.view.imPtr);

    if (endpos == 0)
        return;

    if (endpos - len < 0)
	len = endpos;

    if (ConfirmViewDeletion(self, endpos - len, len)) {
        text_DeleteCharacters(d, endpos - len, len);
        textview_SetDotPosition(self, endpos - len);
        textview_FrameDot(self, endpos - len);
        text_NotifyObservers(d, 0);
    }
}

void textview_BackwardCmd(self)
register struct textview *self;
{
    register long pos;
    register long newPos;

    textview_CollapseDot(self);
    pos = textview_GetDotPosition(self);
    textview_SetDotPosition(self,newPos = (pos-im_Argument(self->header.view.imPtr)));
    textview_FrameDot(self, newPos);
    textview_WantUpdate(self, self);
}

void textview_ForwardCmd(self)
register struct textview *self;
{
    register long pos;
    register long newPos;

    textview_CollapseDot(self);
    pos = textview_GetDotPosition(self);
    textview_SetDotPosition(self, (newPos = pos + im_Argument(self->header.view.imPtr)));
    if (pos != textview_GetDotPosition(self)) {
        textview_FrameDot(self, newPos);
        textview_WantUpdate(self, self);
    }
}

void textview_PreviousLineCmd (self)
register struct textview *self;
{
    register int npos;
    int xpos;
    struct mark tm;	/* note this mark is not on the document's marker chain */
    long pos, dist, lines, currentline, startPos;

    startPos = textview_GetDotPosition(self);
    textview_CollapseDot(self);
    pos = textview_GetDotPosition(self);
    if (im_GetLastCmd(self->header.view.imPtr) == lcMove)
	xpos = self->movePosition;
    else
	self->movePosition = xpos = self->cexPos;
    currentline = textview_FindLineNumber(self, pos);
    npos = textview_MoveBack(self, pos, im_Argument(self->header.view.imPtr), textview_MoveByLines, &dist, &lines);
    if (lines > currentline)  {
	/* Have moved back off the screen */
	if (self->scroll == textview_ScrollForward)
	    self->scroll = textview_MultipleScroll;
	if (self->scrollDist == -1 )  {
	    if (currentline != -1)  {
		self->scrollDist = dist - (self->lines[currentline].y - self->lines[0].y);
		self->scrollLine = lines - currentline;
		textview_SetTopPosition(self, npos);
	    }
	    else  {
		long topPos;

		currentline = textview_FindLineNumber(self, startPos);
		if (currentline != -1 || (startPos <= (topPos = textview_GetTopPosition(self)) && pos >= topPos))
		    textview_FrameDot(self, npos);
	    }
	}
	else  {
	    self->scrollDist += dist;
	    self->scrollLine += lines;
	    textview_SetTopPosition(self, npos);
	}
    }
    else
	textview_FrameDot(self, npos);
    mark_SetPos(&tm, npos);
    mark_SetLength(&tm, 0);
    if (self->csxPos != -1)  {
	struct formattinginfo info;

	npos = textview_LineRedraw (self, textview_GetPosition, &tm, self->bx,
	    self->by, textview_GetLogicalWidth(self) - 2 * self->bx,
	    textview_GetLogicalHeight(self) - 2 * self->by, xpos, (int *) 0, NULL, &info);
    }
    textview_SetDotPosition(self, npos);
    im_SetLastCmd(self->header.view.imPtr, lcMove);
    textview_WantUpdate(self, self);
}

void textview_NextLineCmd (self)
register struct textview *self;
{
    register int npos;
    int xpos;
    struct mark tm;	/* note this mark is not on the document's marker chain */
    long pos, currentline, nlines, newline, startPos;
    static int linetomove;

    startPos = textview_GetDotPosition(self);
    textview_CollapseDot(self);
    pos = textview_GetDotPosition(self);

    if (im_GetLastCmd(self->header.view.imPtr) == lcMove)
	xpos = self->movePosition;
    else
	self->movePosition = xpos = self->cexPos;
    currentline = textview_FindLineNumber(self, pos);
    npos = textview_MoveBack(self,pos,0, textview_MoveByLines, NULL, NULL);
    npos = textview_MoveForward(self,npos, nlines = im_Argument(self->header.view.imPtr), textview_MoveByLines, NULL, NULL);
    if (currentline != -1)  {
	/* Current Position is on the screen */
	
	newline = textview_FindLineNumber(self, npos);
	if (newline == -1)  {
	    linetomove = nlines - (self->nLines - currentline - 1);
	    
	    if (linetomove < self->nLines && linetomove > 0)  {
		if (self->scroll == textview_ScrollBackward) self->scroll = textview_MultipleScroll;
		textview_SetTopPosition(self, mark_GetPos(self->lines[linetomove].data));
	    }
	    else
		textview_FrameDot(self, npos);
	}
    }
    else  {
	/* Current Position is off the screen */
	if (self->scroll == textview_ScrollForward)  {
	    linetomove += nlines;
	    if (linetomove < self->nLines)
		textview_SetTopPosition(self, mark_GetPos(self->lines[linetomove].data));
	    else
		textview_FrameDot(self, npos);
	}
	else  {
	    long topPos;

	    currentline = textview_FindLineNumber(self, startPos);
	    if (currentline != -1 || (startPos <= (topPos = textview_GetTopPosition(self)) && pos >= topPos))
		textview_FrameDot(self, npos);
	}
    }

    mark_SetPos(&tm, npos);
    mark_SetLength(&tm, 0);
    if (self->csxPos != -1)  {
	struct formattinginfo info;

	npos = textview_LineRedraw (self, textview_GetPosition, &tm, self->bx,
	    self->by, textview_GetLogicalWidth(self)-2*self->bx,
	    textview_GetLogicalHeight(self)-2*self->by, xpos, (int *) 0, NULL, &info);
    }
    textview_SetDotPosition(self,npos);
    im_SetLastCmd(self->header.view.imPtr, lcMove);
    textview_WantUpdate(self, self);
}

void textview_EndOfTextCmd(self)
register struct textview *self;
{
    register struct text *d;
    register int e;

    textview_CtrlAtCmd(self);
    d = Text(self);
    e = text_GetLength(d);
    textview_SetDotPosition(self,e);
    textview_SetDotLength(self,0);
    textview_FrameDot(self,e);
    textview_WantUpdate(self, self);
}

void textview_BeginningOfTextCmd(self)
register struct textview *self;
{
    textview_CtrlAtCmd(self);
    textview_SetDotPosition(self,0);
    textview_SetDotLength(self,0);
    textview_FrameDot(self,0);
    textview_WantUpdate(self, self);
}

void textview_EndOfLineCmd (self)
register struct textview *self;
{
    register int startpos, npos, dsize;
    register struct text *d;
    /* lie for now */

    d = Text(self);
    if ((dsize = textview_GetDotLength(self)) > 0)  {
	textview_SetDotPosition(self, dsize+textview_GetDotPosition(self));
	textview_SetDotLength(self, 0);
        textview_WantUpdate(self, self);
	return;
    }
    dsize = text_GetLength(d);
    npos = textview_MoveBack(self, startpos=textview_GetDotPosition(self), 0 , textview_MoveByLines, NULL, NULL);
    npos = textview_MoveForward(self, npos, 1, textview_MoveByLines, NULL, NULL);
    if (npos > 0)  {
	if (npos == dsize)  {
	    if (text_GetChar(d, npos-1) == '\n') npos--;
	    if (npos < startpos) npos = startpos;
	}
	else if (text_GetChar(d, npos-1) == ' ' )
	    while (npos > startpos && (text_GetChar(d, npos-1)) == ' ') npos--;
	else if (text_GetChar(d, npos-1) == '\n') npos--;
    }
    textview_SetDotPosition(self, npos);
    textview_SetDotLength(self, 0);
    textview_FrameDot(self, npos);
    textview_WantUpdate(self, self);
}

void textview_BeginningOfLineCmd(self)
register struct textview *self;
{
    register int pos;

    if (textview_GetDotLength(self) != 0) {
        textview_SetDotLength(self,0);
	pos = textview_GetDotPosition(self);
    }
    else {
        pos = textview_MoveBack(self, textview_GetDotPosition(self),0, textview_MoveByLines, 0, 0);
        textview_SetDotPosition(self,pos);
    }
    textview_FrameDot(self, pos);
    textview_WantUpdate(self, self);
}

/* Switches the two characters before the dot. */

void textview_TwiddleCmd (self)
register struct textview *self;
{
    long pos;
    register struct text *text;
    char char1, char2;
    struct environment *env1, *env2;
    struct viewref *vr1, *vr2;

    if (ConfirmReadOnly(self))
        return;

    textview_CollapseDot(self);
    text = Text(self);
    pos = textview_GetDotPosition(self);

    if (pos < 2 || text_GetReadOnly(text) == TRUE)
        return;

    char1 = text_GetChar(text, pos - 2);
    char2 = text_GetChar(text, pos - 1);

    text_ReplaceCharacters(text, pos - 2, 1, &char2, 1);
    text_ReplaceCharacters(text, pos - 1, 1, &char1, 1);

    /* Code to deal with VIEWREFCHARS  -cm26 */
    /* If a view must be moved, it's deleted then reinserted */
    /* (environment_Update does not seem to work) */
    /* The order of the following is kind of important */

    env1 = environment_GetInnerMost(text->rootEnvironment, pos - 2);
    if (env1 != NULL)
        if (env1->type != environment_View)
            env1 = NULL;
        else {
            vr1 = env1->data.viewref;
            env1->data.viewref = NULL;  /* Protect viewref from Delete */
            environment_Delete(env1);
        }

    env2 = environment_GetInnerMost(text->rootEnvironment, pos - 1);
    if (env2 != NULL)
        if (env2->type != environment_View)
            env2 = NULL;
        else {
            vr2 = env2->data.viewref;
            env2->data.viewref = NULL;  /* Protect viewref from Delete */
            environment_Delete(env2);
        }

    if (env1 != NULL)
        environment_WrapView(text->rootEnvironment,
            pos - 1, 1, vr1);

    if (env2 != NULL)
        environment_WrapView(text->rootEnvironment,
            pos - 2, 1, vr2);

    textview_FrameDot(self, pos);
    text_NotifyObservers(text, 0);
}

void textview_DeleteWordCmd (self)
register struct textview *self;
{
    struct text *d;
    register int count, pos, len;

    if (ConfirmReadOnly(self))
        return;

    textview_CollapseDot(self);
    d = Text(self);

    count = im_Argument(self->header.view.imPtr);
    im_ClearArg(self->header.view.imPtr);   /* Don't want Fwd Word to use it! */
    pos = textview_GetDotPosition(self);

    while (count--)
	textview_ForwardWordCmd(self);

    len = textview_GetDotPosition(self) - pos;

    if (ConfirmViewDeletion(self, pos, len)) {
        text_DeleteCharacters(d, pos, len);
        textview_FrameDot(self, pos);
        text_NotifyObservers(d, self);
    }
}

void textview_RuboutWordCmd (self)
register struct textview *self;
{
    struct text *d;
    register int count, endpos, len;

    if (ConfirmReadOnly(self))
        return;

    textview_CollapseDot(self);
    d = Text(self);

    count = im_Argument(self->header.view.imPtr);
    im_ClearArg(self->header.view.imPtr);   /* Don't want Bkwd Word to use it! */
    endpos = textview_GetDotPosition(self);

    while (count--)
	textview_BackwardWordCmd(self);

    len = endpos - textview_GetDotPosition(self);

    if (ConfirmViewDeletion(self, endpos - len, len)) {
        text_DeleteCharacters(d, endpos - len, len);
        textview_FrameDot(self, endpos - len);
        text_NotifyObservers(d, self);
    }
}

void textview_ForwardWordCmd (self)
register struct textview *self;
{
    register int j, count, pos, dlen;
    register struct text *d;

    textview_CollapseDot(self);
    j = 0;
    count = im_Argument(self->header.view.imPtr);
    d=Text(self);
    pos=textview_GetDotPosition(self);
    dlen= text_GetLength(d);
    while (j<count)  {
	while (pos<dlen && isalnum(text_GetChar(d, pos))==0) pos++;
	while (pos<dlen && isalnum(text_GetChar(d,pos))!=0) pos++;
	j++;
    }
    textview_SetDotPosition(self,pos);
    textview_FrameDot(self, pos);
    textview_WantUpdate(self, self);
}

void textview_BackwardWordCmd (self)
register struct textview *self;
{
    register int j, count, pos;
    register struct text *d;

    textview_CollapseDot(self);
    j = 0;
    count = im_Argument(self->header.view.imPtr);
    d=Text(self);
    pos=textview_GetDotPosition(self);
    while (j<count) {
	while (pos>0 && isalnum(text_GetChar(d,pos-1))==0)  pos--;
	while (pos>0 && isalnum(text_GetChar(d,pos-1))!=0) pos--;
	j++;
    }
    textview_SetDotPosition(self,pos);
    textview_FrameDot(self, pos);
    textview_WantUpdate(self, self);
}

static void ShowArg(self)
struct textview *self;
{
    char buf[100];
    struct im_ArgState *as = im_GetArgState(textview_GetIM(self));

    sprintf(buf, "Arg: %d", as->argument);
    message_DisplayString(self, 0, buf);
}


void textview_SetArgumentCmd (self)
struct textview *self;
{
    register struct im_ArgState *as;
    register long lcstate;
    long newArg;

    lcstate = im_GetLastCmd(self->header.view.imPtr);
    as = im_GetArgState(self->header.view.imPtr);
    as->argDigit = FALSE;
    newArg = 4;
    if (as->argProvided) {
        newArg = as->argument * 4;
        if (newArg > 65536)
            newArg = 65536;
    }
    im_ProvideArg(self->header.view.imPtr, newArg);
    im_SetLastCmd(self->header.view.imPtr, lcstate);	/* be transparent */

    ShowArg(self);
}

void textview_DigitCmd(self, c)
struct textview *self;
register char c;
{
    register struct im_ArgState *as;
    register long lcstate;

    as = im_GetArgState(self->header.view.imPtr);
    if (as->argProvided) {
        long newArg;
	lcstate = im_GetLastCmd(self->header.view.imPtr);
	if (! as->argDigit) {
	    as->argDigit = TRUE;
	    as->argument = 0;
	}
        newArg = as->argument * 10 + (c - '0');
        if (newArg > 65536)
            newArg = 65536;
        im_ProvideArg(self->header.view.imPtr, newArg);
	im_SetLastCmd(self->header.view.imPtr, lcstate);
        ShowArg(self);
    } else
        textview_SelfInsertCmd(self, c);
}

void textview_ChangeTemplate(self)
struct textview *self;
{
    char tname[150];
    register struct text *d;
    int gf;

    if (ConfirmReadOnly(self))
        return;

    d = Text(self);
    gf = message_AskForString(self, 0,
      "Add styles from template: ", 0, tname, 100);
    if (gf < 0)
        return;
    if (tname[0] == '\0')  {   /* no name specified */
	message_DisplayString(self, 0, "No name specified.");
	return;
    }
    if (text_ReadTemplate(d, tname, text_GetLength(d) == 0) < 0)
        message_DisplayString(self, 100, "Could not read template file.");
    else {
	text_RegionModified(d, 0, text_GetLength(d));
	text_NotifyObservers(d, 0);
        message_DisplayString(self, 0, "Done.");
    }
}

static void AdjustCase(self, upper, firstOnly)
struct textview *self;
boolean upper, firstOnly;
{
    long pos, len, count, i;
    boolean capitalize;

    if (ConfirmReadOnly(self))
        return;

    pos = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);
    count = len;

    if (len == 0)
        while (pos > 0 && isalnum(text_GetChar(Text(self), pos - 1)))
            pos--;

    capitalize = TRUE;

    while (pos < text_GetLength(Text(self))) {
        i = text_GetChar(Text(self), pos);
        if (len == 0) {
            if (! isalnum(i))
                break;
        } else {
            if (--count < 0)
                break;
        }
        if (upper) {
            if ((capitalize || ! firstOnly) && islower(i)) {
                char new = toupper(i);
                text_ReplaceCharacters(Text(self), pos, 1, &new, 1);
            }
	    else if(!capitalize && firstOnly && isupper(i)){
		char new = tolower(i);
		text_ReplaceCharacters(Text(self), pos, 1, &new, 1);
	    }
        } else if (isupper(i)) {
            char new = tolower(i);
            text_ReplaceCharacters(Text(self), pos, 1, &new, 1);
        }
        capitalize = isspace(i);
        pos++;
    }

    text_NotifyObservers(Text(self), 0);
}

void textview_UppercaseWord(self, key)
struct textview *self;
long key;
{
    AdjustCase(self, TRUE, FALSE);
}

void textview_LowercaseWord(self, key)
struct textview *self;
long key;
{
    AdjustCase(self, FALSE, FALSE);
}

void textview_CapitalizeWord(self, key)
struct textview *self;
long key;
{
    AdjustCase(self, TRUE, TRUE);
}

void textview_ToggleCase(self, key)
struct textview *self;
long key;
{
    long pos = textview_GetDotPosition(self);
    int i;
    char new;

    if (ConfirmReadOnly(self))
        return;

    i = text_GetChar(Text(self), pos);

    if (isupper(i))
        new = tolower(i);
    else if (islower(i))
        new = toupper(i);
    else
        new = i;

    text_ReplaceCharacters(Text(self), pos, 1, &new, 1);
    text_NotifyObservers(Text(self), 0);

    textview_SetDotPosition(self, pos + 1);
}

void textview_CursorToLine(self, line)
struct textview *self;
long line;
{
    if (line > 0 && line <= self->nLines) {
        textview_SetDotPosition(self, mark_GetPos(self->lines[line - 1].data));
        textview_SetDotLength(self, 0);
        textview_WantUpdate(self, self);
    }
}

/* Useful commands for our editor. */

void textview_CursorToTop(self, key)
struct textview *self;
long key;
{
    textview_CursorToLine(self, 1);
}

void textview_CursorToCenter(self, key)
struct textview *self;
long key;
{
    textview_CursorToLine(self, self->nLines / 2);
}

void textview_CursorToBottom(self, key)
struct textview *self;
long key;
{
    textview_CursorToLine(self, self->nLines);
}

void textview_GrabReference(self,key)
struct textview *self;
long key;
{
    long pos,len;
    struct viewref *vr;
    struct text *d = Text(self);
    pos = textview_GetDotPosition(self);
    len = textview_GetDotLength(self);
    if (len == 0) len = text_GetLength(d) - pos;
    if ((vr = text_FindViewreference(d, pos, len)) == NULL)
        message_DisplayString(self, 0, "No References Found");
    else
        d->currentViewreference = vr;
}

void textview_PlaceReference(self,key)
struct textview *self;
long key;
{
    long pos;
    char p[250];
    struct text *d = Text(self);
    boolean promptforname = im_ArgProvided(textview_GetIM(self));

    im_ClearArg(self->header.view.imPtr); 

    if (ConfirmReadOnly(self))
        return;
    if(text_GetObjectInsertionFlag(Text(self)) == FALSE){
	message_DisplayString(self, 0, "Object Insertion Not Allowed!");
	return;
    }

    *p = '\0';
    if(d->currentViewreference == NULL) {
        message_DisplayString(self, 0, "No References Found");
        return;
    }
    pos = textview_GetDotPosition(self) +  textview_GetDotLength(self);
    if(promptforname && message_AskForString (self, 0, "View to place here ", d->currentViewreference->viewType, p, 200) < 0) return;
    if (p[0] == '\0')  strcpy(p,d->currentViewreference->viewType);
    if(objecttest(self,p,"view") == FALSE) return;
    text_AddView(d,pos,p, d->currentViewreference->dataObject);
    text_NotifyObservers(d,0);
}

void textview_CheckSpelling(self)
struct textview *self;
{
    message_DisplayString(self, 0,
       "Sorry; \"Check Spelling\" is not implemented.");
}

void textview_ToggleReadOnly(self)
struct textview *self;
{
    boolean argp = im_ArgProvided(textview_GetIM(self)), arg;
    struct text *myText = Text(self);

    if (argp)
        arg = im_Argument(textview_GetIM(self));
    if ((argp && arg) || (!argp && text_GetReadOnly(myText))) /* In readonly mode. */
        text_SetReadOnly(myText, FALSE);
    else
        text_SetReadOnly(myText, TRUE);
    if (text_GetReadOnly(myText))
        message_DisplayString(self, 0, "Text is now read only.");
    else
        message_DisplayString(self, 0, "Text is now writable.");
    text_NotifyObservers(myText, 0); /* Handles updating of menus on read only transition. */
}

#define BADCURPOS -1

void textview_ToggleExposeStyles(self)
struct textview *self;
{
    self->exposeStyles = ! self->exposeStyles;
    self->force = TRUE;
    self->csxPos = BADCURPOS;		/* Indication that cursor is not visible */
    self->cexPos = BADCURPOS;
    textview_WantUpdate(self, self);
}

static void textview_InsertPageBreak (self)
    register struct textview *self;
{
    long pos;
    struct text *d;

    d = Text(self);
    if(text_GetObjectInsertionFlag(d) == FALSE){
	message_DisplayString(self, 0, "Object Insertion Not Allowed!");
	return;
    }
    pos = textview_GetDotPosition(self);
    if(text_GetChar(d,pos) != '\n'){
	text_InsertCharacters(d,pos,"\n",1);
    }
    if(text_GetChar(d,pos - 1) != '\n'){
	text_InsertCharacters(d,pos,"\n",1);
	pos++;
    }

    /* self->currentViewreference = */
    text_InsertObject(d, pos,"bp","bpv"); 
    text_NotifyObservers(d,0);
}
static void textview_NextPage (self)
    register struct textview *self;
{
    long pos,len;
    struct text *d;
    struct viewref *vr;
    d = Text(self);
    len = text_GetLength(d);
    pos = textview_GetDotPosition(self);
    while ( pos <= len && (pos = text_Index(d,pos,TEXT_VIEWREFCHAR,len - pos)) != EOF){
	if((vr = text_FindViewreference(d,pos,1)) != NULL && 
	   *(vr->viewType) == 'b' &&
	   strcmp(vr->viewType,"bpv") == 0){
	    textview_SetDotPosition(self,pos + 2);
	    textview_SetTopPosition(self,pos + 2);
	    break;
	}
	pos++;
    }
}
static long text_rindex(txt,pos,c)
register struct text *txt;
register long pos;
register char c;
{
    for(;pos > 0;pos--){
	if(text_GetChar(txt,pos) == c) return pos;
    }
    return EOF;
}
static void textview_LastPage (self)
    register struct textview *self;
{
    long pos,cnt;
    struct text *d;
    struct viewref *vr;
    d = Text(self);
    pos = textview_GetDotPosition(self);
    for ( cnt = 0;pos > 0 && (pos = text_rindex(d,pos,TEXT_VIEWREFCHAR)) != EOF;pos--){
	if((vr = text_FindViewreference(d,pos,1)) != NULL && 
	   *(vr->viewType) == 'b' &&
	   strcmp(vr->viewType,"bpv") == 0){
	    if(cnt++ == 0) continue;
	    textview_SetDotPosition(self,pos + 2);
	    textview_SetTopPosition(self,pos + 2);
	    return;
	}
    }
    textview_SetDotPosition(self,0);
    textview_SetTopPosition(self,0);
}
#if 0
static boolean lookforfootnote(self,text,pos,env)
struct textview *self;
struct text *text;
long pos;
struct environment *env;
{
    struct style *st;
    char *sn;
    if(env->type == environment_Style) {
	st = env->data.style;
	if(st != NULL && ((sn = style_GetName(st)) != NULL) &&
	   *sn == 'f' && strcmp(sn,"footnote") == 0) return TRUE;
    }
    return FALSE;
}
#endif
static void textview_InsertFootnote(self)
    register struct textview *self;
{
    long pos;
    struct fnote *fn;

    if(text_GetObjectInsertionFlag(Text(self)) == FALSE){
	message_DisplayString(self, 0, "Object Insertion Not Allowed!");
	return;
    }
    pos = textview_GetDotPosition(self);
#if 0
    if(text_EnumerateEnvironments(Text(self),pos,0,lookforfootnote,NULL) != NULL){
	message_DisplayString(self,0,"footnotes in footnotes not supported");
	return;
    }
#endif
    fn = fnote_New();
/*    self->currentViewreference = text_InsertObject(Text(self), pos,"fnote","fnotev"); */
    text_AddView(Text(self), pos,"fnotev",fn);
    fnote_addenv(fn,Text(self),pos);
    
    text_NotifyObservers(Text(self),0);
    textview_SetDotPosition(self,pos + 1);
}
static void textview_OpenFootnotes(self)
    register struct textview *self;
{
    fnote_OpenAll(Text(self));
}
static void textview_CloseFootnotes(self)
    register struct textview *self;
{
    fnote_CloseAll(Text(self));
}
static void textview_WriteFootnotes(self)
    register struct textview *self;
{
    FILE *f,*fopen();
    struct text *tmpt;
    f = fopen("/tmp/notes","w");
    tmpt = text_New();
    fnote_CopyAll(Text(self),tmpt,1,TRUE);
    text_Write(tmpt,f,0,0);
    fclose(f);
    text_Destroy(tmpt);
}



static struct bind_Description textviewBindings[]={
    {"textview-beginning-of-line", "\001",0,NULL,0,0,textview_BeginningOfLineCmd,"Move to beginning of current line."},
    {"textview-beginning-of-line", "\033H",0},
    {"textview-backward-character", "\002",0,NULL,0,0,textview_BackwardCmd,"Move backward one character."},
    {"textview-backward-character", "\033D",0},
    {"textview-delete-next-character", "\004",0,NULL,0,0,textview_DeleteCmd,"Delete the next character."},
    {"textview-end-of-line", "\005",0,NULL,0,0,textview_EndOfLineCmd,"Move to end of the current line."},
    {"textview-end-of-line", "\033F",0},
    {"textview-forward-character", "\006",0,NULL,0,0,textview_ForwardCmd,"Move forward one character."},
    {"textview-forward-character", "\033C",0},
    {"textview-delete-previous-character", "\010",0,NULL,0,0,textview_RuboutCmd,"Delete the previous character."},
    {"textview-delete-previous-character", "\177",0},
    {"textview-insert-newline", "\015",0,NULL,0,0,textview_InsertNLCmd,"Insert a newline character."},
    {"textview-next-line", "\016",0,NULL,0,0,textview_NextLineCmd,"Move to next line."},
    {"textview-next-line", "\033B",0},
    {"textview-previous-line", "\020",0,NULL,0,0,textview_PreviousLineCmd,"Move to previous line."},
    {"textview-previous-line", "\033A",0},
    {"textview-set-argument", "\025",0,NULL,0,0,textview_SetArgumentCmd,"Set argument count."},

    {"textview-indent", "\033i",0,NULL,0,0,textview_IndentCmd,"Indent current line."},

    {"textview-GrabReference", "\030w",0,NULL,0,0,textview_GrabReference,"Grab the next viewref"},

    {"textview-PlaceReference", "\030y",0,NULL,0,0,textview_PlaceReference,"Create a new viewref"},

    {"textview-unindent", "\033u",0,NULL,0,0,textview_UnindentCmd,"Un-indent current line."},

    {"textview-exch", "\030\030",0,NULL,0,0,textview_ExchCmd,"Exchange dot and mark."},

    {"textview-select-region", "\033\200",0,NULL,0,0,textview_SelectRegionCmd,"Select between dot and mark."},

    {"textview-copy-region", "\033w",0,"Copy~11",0,textview_SelectionMenus,textview_CopyRegionCmd,"Copy region to kill-buffer."},

    {"textview-mylf", "\012",0,NULL,0,0,textview_MyLfCmd,"Skip to next line and indent."},

    {"textview-zap-region", "\027",0,"Cut~10",0, textview_SelectionMenus | textview_NotReadOnlyMenus,textview_ZapRegionCmd,"Remove the text within the selection region and place it in a cutbuffer."},

    {"textview-append-next-cut", "\033\027",0,NULL,0,0,textview_AppendNextCut, "Make next cut command append to the cutbuffer as opposed to making a new buffer."},

    {"textview-yank", "\031",0,"Paste~10",0,textview_NotReadOnlyMenus | textview_NoSelectionMenus,textview_YankCmd,"Yank text back from kill-buffer."},

    {"textview-insert-file", "\030\t", 0,"File~10,Insert File~10",0,textview_NotReadOnlyMenus,textview_InsertFile,"Prompt for a filename and insert that file's contents into the document."},

    {"textview-rotate-backward-paste", "\033\031",0,NULL,0,0,textview_BackwardsRotatePasteCmd,"Rotate kill-buffer backwards."},

    {"textview-line-to-top", "\033!",0,NULL,0,0,textview_LineToTopCmd,"Move current line to top of screen."},

    {"textview-rotate-paste", "\033y",0,NULL,0,0,textview_RotatePasteCmd,"Rotate kill-buffer."},
    
    {"textview-forward-para", "\033]",0,NULL,0,0,textview_ForwardParaCmd,"Move to the next paragraph."},

    {"textview-backward-para", "\033[",0,NULL,0,0,textview_BackwardParaCmd,"Move to the next paragraph."},

    {"textview-open-line", "\017",0,NULL,0,0,textview_OpenLineCmd,"Insert blank line at current pos."},

    {"textview-plainer", "\030\020",0,"Plainer~40",0,textview_NotReadOnlyMenus,textview_PlainerCmd,"Remove innermost environment."},

    {"textview-plainest",NULL,0,"Plainest~41",0,textview_SelectionMenus | textview_NotReadOnlyMenus,textview_PlainestCmd,"Remove all enclosing styles."},

    {"textview-prev-screen", "\033v",0,NULL,0,0,textview_PrevScreenCmd,"Move forward to previous screen"},
    {"textview-prev-screen", "\033G",0},

    {"textview-next-screen", "\026",0,NULL,0,0,textview_NextScreenCmd,"Move forward to next screen"},
    {"textview-next-screen", "\033E",0},

    {"textview-glitch-down", "\021",0,NULL,0,0,textview_GlitchDownCmd,"Glitch screen down one line."},
    {"textview-glitch-down", "\033z",0},

    {"textview-glitch-up", "\032",0,NULL,0,0,textview_GlitchUpCmd,"Glitch screen up one line."},

    {"textview-twiddle-chars", "\024",0,NULL,0,0,textview_TwiddleCmd,"Exchange previous two chars."},

    {"textview-kill-line", "\013",0,NULL,0,0,textview_KillLineCmd,"Kill rest of line."},

    {"textview-search", "\023",0,"Search/Spell~1,Forward~10",0,0,(void (*)())textview_SearchCmd,"Search forward."},

    {"textview-reverse-search", "\022",0,"Search/Spell~1,Backward~11",0,0,(void (*)())textview_RSearchCmd,"Search backward."},

    {"textview-search-again",NULL,0,"Search/Spell~1,Search Again~12",0,0,textview_SearchAgain,"Repeat last search."},

    {"textview-insert-inset-here", "\033\t",0,NULL,0,0,textview_InsertInsetCmd,"Add inset at this location."},

    {"textview-what-paragraph", "\033N",0,NULL,0,0,textview_WhatParagraphCmd,"Print current paragraph number."},

    {"textview-show-styles", "\033s",0,NULL,0,0,textview_ShowStylesCmd,"Show styles at dot."},

    {"textview-goto-paragraph", "\033n",0,NULL,0,0,textview_GotoParagraphCmd,"Go to specific paragraph."},

    {"textview-query-replace", "\033q",0,"Search/Spell~1,Query Replace~20",0,textview_NotReadOnlyMenus,textview_QueryReplaceCmd,"Query replace."},

#ifdef IBM
    {"spell-check-document",NULL,0,"Search/Spell~1,Check Spelling~30", 0,textview_NotReadOnlyMenus,textview_CheckSpelling,"Checks spelling from the caret on.", "spell"},
#else /* IBM */
    {"spell-check-document",NULL,0,"Search/Spell~1,Check Spelling~30", 0,textview_NotReadOnlyMenus,NULL,"Checks spelling from the caret on.", "spell"},
#endif /* IBM */

    {"textview-quote", "\030\021",0,NULL,0,0,textview_QuoteCmd,"Uninterpreted insert of any char or octal code."},

    {"textview-kill-white-space", "\033k",0,NULL,0,0,textview_KillWhiteSpaceCmd,"Delete spaces and tabs around the current pos."},

    {"textview-ctrl-at", "\200",0,NULL,0,0,textview_CtrlAtCmd,"Set a mark."},

    {"textview-backward-word", "\033b",0,NULL,0,0,textview_BackwardWordCmd,"Move backward to beginning of word."},
    {"textview-delete-next-word", "\033d",0,NULL,0,0,textview_DeleteWordCmd,"Delete  the next word."},
    {"textview-forward-word", "\033f",0,NULL,0,0,textview_ForwardWordCmd,"Move forward to end of word."},
    {"textview-delete-previous-word", "\033h",0,NULL,0,0,textview_RuboutWordCmd,"Delete the previous word."},
    {"textview-delete-previous-word", "\033\b",0,NULL,0,0,textview_RuboutWordCmd,NULL},
    {"textview-delete-previous-word", "\033\177",0,NULL,0,0,textview_RuboutWordCmd,NULL},
    {"textview-end-of-text", "\033>",0,NULL,0,0,textview_EndOfTextCmd,"Move to end of text."},
    {"textview-beginning-of-text", "\033<",0,NULL,0,0,textview_BeginningOfTextCmd,"Move to beginning of text."},
    {"textview-toggle-character-case", "\036",0,NULL,0,0,textview_ToggleCase,"Toggle the case of the character at the dot."},
    {"textview-lowercase-word", "\033l",0,NULL,0,0,textview_LowercaseWord,"Convert word (or region) to lower case."},
    {"textview-uppercase-word",NULL,0,NULL,0,0,textview_UppercaseWord,"Convert word (or region) to upper case."},
    {"textview-capitalize-word",NULL,0,NULL,0,0,textview_CapitalizeWord,"Capitalize word (or all words within a region)."},
    {"textview-cursor-to-top", "\033,",0,NULL,0,0,textview_CursorToTop,"Moves cursor to the beggining of the line currently at the top of the screen."},
    {"textview-cursor-to-bottom", "\033.",0,NULL,0,0,textview_CursorToBottom,"Moves cursor to the beggining of the line currently at the bottom of the screen."},
    {"textview-cursor-to-center", "\033/",0,NULL,0,0,textview_CursorToCenter,"Moves cursor to the beggining of the line currently at the center of the screen."},

    {"textview-change-template",NULL,0,"File~10,Add Template~31",0,textview_NotReadOnlyMenus,textview_ChangeTemplate, NULL, "Change to named template."},

    {"textview-toggle-read-only", "\033~",0,NULL,0,0,textview_ToggleReadOnly,"Change read only status of text object."},
    {"textview-toggle-expose-styles", NULL,0,NULL,0,0,textview_ToggleExposeStyles,"Expose/hide style information"},
    {"textview-edit-styles", NULL, 0, NULL, 0, textview_NotReadOnlyMenus, textview_ExposeStyleEditor,"Expose style editor"},
    {"textview-insert-pagebreak", NULL,0,"Page~9,Insert Pagebreak~11",0,textview_NotReadOnlyMenus,textview_InsertPageBreak,"Add page break at this location."},
    {"textview-next-page", NULL,0,"Page~9,Next Page~12",0,0,textview_NextPage,"Frame text at next page break object"},
    {"textview-last-page", NULL,0,"Page~9,Previous Page~13",0,0,textview_LastPage,"Frame text at last page break object"},
    {"textview-insert-footnote", NULL,0,"Page~9,Insert Footnote~20",0,textview_NotReadOnlyMenus,textview_InsertFootnote,"Add footnote at this location."},
    {"contentv-make-window", NULL,0,"Page~9,Table of Contents~30",0,0,NULL,"Make a table of contents window","contentv"},
    {"textview-open-footnotes",NULL,0,"Page~9,Open Footnotes~22",0,0,textview_OpenFootnotes,"Open all footnotes"},
    {"textview-close-footnotes",NULL,0,"Page~9,Close Footnotes~23",0,0,textview_CloseFootnotes,"Close all footnotes"},
    {"textview-write-footnotes",NULL,0,NULL,0,0,textview_WriteFootnotes,"Write all footnotes"},
    NULL
};

struct keymap *textview_InitKeyMap(classInfo, normalMenus)
struct textview_classinfo *classInfo;
struct menulist **normalMenus;
{
    struct keymap *newKeymap = keymap_New();
    register long i;
    char str[2];
    struct proctable_Entry *si,*dig;
    
    if(normalMenus!=NULL)
	*normalMenus=menulist_New();

    lcKill = im_AllocLastCmd();
    lcYank = im_AllocLastCmd();
    lcMove = im_AllocLastCmd();
    lcDisplayEnvironment = im_AllocLastCmd();

    bind_BindList(textviewBindings, newKeymap, *normalMenus, classInfo);

    si=proctable_DefineProc("textview-self-insert", textview_SelfInsertCmd, classInfo, NULL, "Insert a character.");
    dig=proctable_DefineProc("textview-digit", textview_DigitCmd, classInfo, NULL, "Insert a character.");

    str[0] = ' ';
    str[1] = '\0';
    for (i = 32; i < 127; i++)  {
	if (i < '0' || i > '9')
	    keymap_BindToKey(newKeymap, str, si, i);
	else
	    keymap_BindToKey(newKeymap, str, dig, i);
	str[0]++;
    }
    keymap_BindToKey(newKeymap, "\t", si, '\t');

    return newKeymap;
}
