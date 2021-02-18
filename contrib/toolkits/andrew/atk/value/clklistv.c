/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/clklistv.c,v 1.9 89/07/05 17:37:15 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/clklistv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/clklistv.c,v 1.9 89/07/05 17:37:15 tpn Exp $ ";
#endif /* lint */
#include <andrewos.h>
#include <class.h>
#include <lpair.ih>
#include <text.ih>
#include <cltextv.ih>
#include <observe.ih>
#include <value.ih>
#include <valuev.ih>

#include <clklistv.eh>


static void handleclicks(self,cv,position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos,which,type)
struct clicklistV *self;
struct cltextview *cv;
long *position;
long *numberOfClicks;
enum view_MouseAction *action;
long *startLeft;
long *startRight;
long *leftPos;
long *rightPos;
long which,type;
{   /* deal with clicks */
    struct value *val;
    if(type == cltextview_PREPROCESS){
	if(*numberOfClicks > 1 && 
	   value_GetValue((val = clicklistV_Value(self))) != *numberOfClicks)
	    value_SetValue(val,*numberOfClicks);
	return;
    }
    if(*action == view_LeftUp){
	char *cp,*index();
	int start,end,len,tlen;
	val = clicklistV_Value(self);
	if(*numberOfClicks == 1){
	    for(start = *leftPos - 1; start > 0 ; start--){
		if(text_GetChar(self->text,start) == '\n'){
		    start++;
		    break;
		}
	    }
            if(start < 0) start = 0;
	    tlen = text_GetLength(self->text);
	    if((end = text_Index(self->text,start,'\n',tlen - start)) == EOF)
		end = tlen;
	    len = end - start;
	    if(  len > 0){	
		if(len >= self->csize){
		    self->choice = realloc(self->choice,len + 1);
		    self->csize = len + 1;
		}
		text_CopySubString(self->text,start, len,self->choice,FALSE);
		if((cp = index(self->choice,'\n')) != NULL) *cp = '\0';
		self->choicechanged = TRUE;
		if(value_GetValue(val) != 1){
		    value_SetNotify(val,FALSE);
		    value_SetValue(val,1);
		    value_SetNotify(val,TRUE);
		}
		value_SetString(val,self->choice);
	    }
	}
    }
}
#ifdef USEGETCOUNT
static getcount(str,size)
char **str;
long size;
{
    register char *c;
    register long cnt = 0;
    for(c = *str; size > 0; size--){
	while(*c) cnt += (long) *c++;
    }
    return cnt;
}
#endif /* USEGETCOUNT */
static SetArray(self,str,size)
struct clicklistV *self;
char **str;
long size;
{
    struct text *txt;
    long i,end,sl,textchanged;
    txt = self->text;
    textchanged = 0;
    end = text_Index(txt,0,'\n',text_GetLength(txt));
    for(i = 0; size > 0; str++,size--){
	sl = strlen(*str);
/*	printf("str = %s, i = %d (%c), end = %d\n",*str,i,text_GetChar(txt,i),end);*/
	if(end == EOF){
	    text_AlwaysInsertCharacters(txt,text_GetLength(txt),*str,sl);
	    text_AlwaysInsertCharacters(txt,text_GetLength(txt),"\n",1);
	    textchanged++;
	}
	else{
	    if(sl == (end - i) && text_Strncmp(txt,i,*str,sl) == 0){
		/* string is correct */
		i = end;
	    }
	    else {
		text_AlwaysReplaceCharacters(txt,i,end - i,*str,sl);
		i += sl;
		textchanged++;
	    }
	    i++;
	    end = text_Index(txt,i,'\n',text_GetLength(txt) - i);
	}
    }
    if(end != EOF){
	sl = text_GetLength(txt);
	if(sl > i){
	    text_AlwaysDeleteCharacters(txt,i, sl - i);
	    textchanged++;
	}
    }
    if(textchanged) {
	text_SetFence(txt,text_GetLength(txt));
	text_NotifyObservers(txt,0);
	cltextview_CollapseDot(self->cltextview);
    }
}
void clicklistV__ObservedChanged(self,changed,value)
struct clicklistV *self;
struct observable *changed;
long value;
{
    struct value *val ;
    char **arr;
    long size;
    val = clicklistV_Value(self);
    if(value == observable_OBJECTDESTROYED){
	if(changed == (struct observable *)self->cltextview)
	    self->cltextview = NULL;
	if( changed == (struct observable *) self->text)
	    self->text = NULL;
	return;
    }
    if( changed == (struct observable *) self->text){
    }
    else {
	if(val != (struct value *)self->header.view.dataobject){
	    /* ERROR */
	    fflush(stdout);
	    val = (struct value *)self->header.view.dataobject;
	}
	if(self->choicechanged){
	    self->choicechanged = FALSE;
	    cltextview_SetDotLength(self->cltextview,0);
	}
	else{
	    arr = value_GetStringArray(val);
	    size = value_GetArraySize(val);
#ifdef USEGETCOUNT
	    {
		long count;
		count = getcount(arr,size);
		if(self->count != count){
		    self->count = count;
		    SetArray(self,arr,size);
		}
	    }
#else /* USEGETCOUNT */
	    SetArray(self,arr,size);
#endif /* USEGETCOUNT */
	}
	super_ObservedChanged(self,changed,value);
    }
}

struct view *clicklistV__GetApplicationLayer(self)
struct clicklistV *self;
{
    struct lpair *lp;
    struct cltextview *ev;
    long w,h;
    if((self->text = text_New()) == NULL) return (struct view *)self;
    if((ev = cltextview_New()) == NULL) return (struct view *)self;
    cltextview_SetDataObject(ev,self->text);
    h = 40;
    if(((struct view *)self)->parent != NULL){
	/* can't call desired size on unlinked text */
	cltextview_LinkTree(ev,self);
	cltextview_DesiredSize(ev,500,500,0,&w,&h);
	cltextview_UnlinkTree(ev);
    }
    lp = lpair_New();
    lpair_VTFixed(lp,self,cltextview_GetApplicationLayer(ev),h,TRUE);
    lpair_SetLPState(lp,lpair_TOPFIXED,lpair_HORIZONTAL,lpair_NOCHANGE);
    self->cltextview = ev;
    cltextview_AddClickObserver(ev,self,handleclicks,0);
    cltextview_AddObserver(ev,self);
    return (struct view *)lp;
}
boolean clicklistV__InitializeObject(classID,self)
struct classheader *classID;
struct clicklistV *self;
{
    char *malloc();
    self->text = NULL;
    self->cltextview = NULL;
    self->csize = 128;
    self->choice = malloc(128);
    self->choicechanged = FALSE;
    clicklistV_SetUseAlt(self,FALSE);
    return TRUE;
}
boolean clicklistV__FinalizeObject(classID,self)
struct classheader *classID;
struct clicklistV *self;
{
    if(self->text)
	text_RemoveObserver(self->text,self);
    free(self->choice);
    return TRUE;
}
void clicklistV__WantInputFocus(self)
struct clicklistV *self;
{
    if(self->cltextview) 
	cltextview_WantInputFocus(self->cltextview,self->cltextview);
}
