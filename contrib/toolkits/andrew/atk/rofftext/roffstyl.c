/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: roffstyl.c,v 1.1 89/12/06 20:21:09 xguest Exp $ */
/* $ACIS: $ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/rofftext/RCS/roffstyl.c,v $ */

#ifndef lint
static char *rcsid = "$Header: roffstyl.c,v 1.1 89/12/06 20:21:09 xguest Exp $ ";
#endif /* lint */

/* styles */

#include <class.h>
#include <text.ih>
#include <style.ih>
#include <stylesht.ih>
#include <envrment.ih>
#include <hash.ih>
#include <rofftext.ih>


WriteText(self)
struct rofftext *self;
{
    rofftext_Write(self,stdout,(long)self,1);
}


/* change style with id='id' to new style 'st'
 * returns id of new style
 *
 */

ChangeStyle(self,id,st)
struct rofftext *self;
int id;
char *st;
{
    int l, newid;

    DEBUG((stderr,"ChangeStyle: <current = %d>, changing %d to (%s)\n",self->stack->ID,id,(st?st:"")));

    if (id == 0) {
        return (BeginStyle(self,st));
    }

    for (l=self->stack->level;(id != self->stack->ID)&&(self->stack->level >= 0);l--) {
        self->tempstack++;
        self->tempstack->pos = self->stack->pos;
        self->tempstack->style = self->stack->env->data.style;
        self->tempstack->level = (self->tempstack-1)->level+1;
        self->tempstack->ID = self->stack->ID;
        CloseStyle(self);
    }
    CloseStyle(self);
    newid = BeginStyle(self,st);
    for(l=self->tempstack->level;l>=0;l--) {
	union environmentcontents data;
	
        self->stack++;
        self->stack->pos = self->pos;
 	data.style = self->tempstack->style;
        self->stack->env = environment_InsertStyle((self->stack-1)->env, self->pos - (self->stack-1)->pos, data, TRUE);
        self->stack->level = (self->stack-1)->level+1;
        self->stack->ID = self->tempstack->ID;
        self->tempstack--;
    }
    return newid;
    
}

/* close innermost style */

CloseStyle(self)
struct rofftext *self;
{
    DEBUG((stderr,"Closing style %d (%d to %d, length %d)\n", self->stack->ID, self->stack->pos, self->pos, (self->pos - self->stack->pos)));

    if (self->stack->level < 0) {
        DEBUG((stderr,"rofftext: WARNING: tried to close bottom-level style\n"));
        return;
    }

    if (self->pos > self->stack->pos) {
        environment_SetLength(self->stack->env,self->pos - self->stack->pos);
        environment_SetStyle(self->stack->env,FALSE,FALSE);
    }
    else {
        DEBUG((stderr,"(Removing the 0-length style)\n"));
        environment_Delete(self->stack->env);
    }
    self->stack--;

}

/* begin a style.  Caller must hang onto returned ID to close style */

BeginStyle(self,st)
struct rofftext *self;
char *st;
{
    struct style *style;
    union environmentcontents data;
    
    if (st == NULL)
        return 0;

    style = stylesheet_Find(self->text->styleSheet,st);

    if (style == NULL) {
        DEBUG((stderr,"BeginStyle: opening non-existent style (%s)\n",st));
        return 0; /* null style */
    }
    self->stack++;
    self->stack->pos = self->pos;
    data.style = style;
    self->stack->env = environment_InsertStyle((self->stack-1)->env, self->pos - (self->stack-1)->pos, data, TRUE);
    self->stack->level = (self->stack-1)->level+1;
    self->stack->ID = self->styleID++;

    if (self->styleID == 0)   /* make sure we don't get a '0' style id */
        self->styleID = 1;

    DEBUG((stderr,"BeginStyle: opening style %d (%s)\n",self->stack->ID,st));
    return self->stack->ID;
}

/* ends a style and cleans up */

EndStyle(self,ID)
struct rofftext *self;
int ID;
{
    int l;

    DEBUG((stderr,"EndStyle: current %d, closing %d\n",self->stack->ID,ID));

    if (ID == 0)
        return; /* null style */

    for (l=self->stack->level;(ID != self->stack->ID)&&(self->stack->level >= 0);l--) {
        self->tempstack++;
        self->tempstack->pos = self->stack->pos;
        self->tempstack->style = self->stack->env->data.style;
        self->tempstack->level = (self->tempstack-1)->level+1;
        self->tempstack->ID = self->stack->ID;
        CloseStyle(self);
    }
    CloseStyle(self);
    for(l=self->tempstack->level;l>=0;l--) {
	union environmentcontents data;
	
        self->stack++;
        self->stack->pos = self->pos;
 	data.style = self->tempstack->style;
	self->stack->env = environment_InsertStyle((self->stack-1)->env, self->pos - (self->stack-1)->pos, data, TRUE);
        self->stack->level = (self->stack-1)->level+1;
        self->stack->ID = self->tempstack->ID;
        self->tempstack--;
    }

} 

CloseAllStyles(self)
struct rofftext *self;
{
    while(self->stack->level >= 0)
        CloseStyle(self);
}
