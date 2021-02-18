/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/nstdmark.c,v 2.9 89/11/20 08:39:51 ghoti Exp $ */
/* $ACIS:nstdmark.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/nstdmark.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/nstdmark.c,v 2.9 89/11/20 08:39:51 ghoti Exp $";
#endif /* lint */

#include <class.h>
#include <nstdmark.eh>
#include <tree23.ih>

static boolean GlobalIsolation = FALSE;

/* These statics are used for generating the next change in the nested marks.
Set in GetInnerMost and used in GetNextChange.
 */
static struct nestedmark *lastSelf;	
static long lastPos;
static long ncrlength;

boolean nestedmark__InitializeObject(classID, self)
struct classheader *classID;
struct nestedmark *self;  {
    self->children = NULL;
    self->position = NULL;
    self->length = 999999999;
    self->parent = NULL;
    self->includeEnding = TRUE;
    self->includeBeginning = FALSE;

    return TRUE;
}

struct nestedmark *nestedmark__NewButSimilar(self)
struct nestedmark *self;
{
    struct nestedmark *sib=nestedmark_NewFromObject(self);
    sib->includeBeginning=self->includeBeginning;
    sib->includeEnding=self->includeEnding;
    return sib;
}

static void DoFreeTree(self)
struct nestedmark *self;
{
    nestedmark_FreeTree(self);
}

void nestedmark__FreeTree(self)
struct nestedmark *self;  {
    if (self->children)  {
	tree23int_Apply(self->children, DoFreeTree);
	tree23int_Free(self->children);
    }
    nestedmark_Destroy(self);
}
/* owns for communication with filter proc
 */
struct filterstruct {
    struct tree23int *fptree;
    struct nestedmark *fpparent;
};

static void FilterProc(self, data, t, which)
struct nestedmark *self;
struct filterstruct *data;
struct tree23int *t;
struct tree23int *which;  {
    self->position = t;
    if (which == data->fptree)
	self->parent = data->fpparent;
}

struct nestedmark *splitOffRight(self,rpos)
struct nestedmark *self;
int rpos;
{
    struct tree23int *node;
    struct nestedmark *right=nestedmark_NewButSimilar(self);

    if(self->children!=NULL)
	node=tree23int_GetLeftMostNode(self->children);
    else
	node=NULL;

    if(node!=NULL)
	right->children=tree23int_New();

    while(node!=NULL){
	struct nestedmark *child=((struct nestedmark *)node->data);
	struct tree23int *nextnode = tree23int_GetNextNode(self->children,node);
	int pos=tree23int_Eval(node),
	    len=child->length;

	if(pos+len>rpos)
	    if(pos >= rpos){
		tree23int_Delete(node);
		child->position= tree23int_Insert(right->children,pos-rpos,child);
		tree23int_Destroy(node);
		child->parent=right;
	    }else{
		struct nestedmark *halfchild=
		  splitOffRight(child,rpos-pos);
		child->length=rpos-pos;
		halfchild->length=len-child->length;
		halfchild->position= tree23int_Insert(right->children,0,halfchild);
		halfchild->parent=right;
	    }		
	node=nextnode;
    }

    right->length=self->length-rpos;
    self->length=rpos;

    return right;
}

struct nestedmark *nestedmark__Split(self,rpos)
struct nestedmark *self;
long rpos;
{
    struct nestedmark *right=splitOffRight(self,rpos), *parent=self->parent;

    if(parent!=NULL){
	int prpos=tree23int_Eval(self->position);
	right->position=
	  tree23int_Insert(parent->children,prpos+rpos,right);
	right->parent=parent;
    }

    return right;
}

struct nestedmark *nestedmark__Add(self, pos, length)
struct nestedmark *self;
long pos;
long length;
{
    register struct nestedmark *cp;
    register struct nestedmark *nm1;
    register struct nestedmark *nm2;
    long eorg;
    long rpos;
    struct tree23int *tt1;
    struct tree23int *tt2;
    struct filterstruct procdata;
    struct nestedmark *newnm;

    if (length <= 0) return FALSE;

    nm1 = nestedmark_GetInnerMost(self, pos);
    nm2 = nestedmark_GetInnerMost(self, pos+length-1);
    cp = nestedmark_GetCommonParent(nm1,nm2);
    eorg = nestedmark_Eval(cp);
    rpos = pos - eorg;	/* store position relative to start of enviro */

    if(nm1!=cp){
	int crpos;
	while(nm1->parent!=cp)
	    nm1=nm1->parent;
	crpos=tree23int_Eval(nm1->position);
	if(rpos>crpos)
	    (void)nestedmark_Split(nm1,rpos-crpos);
    }

    if(nm2!=cp){
	int crpos;
	while(nm2->parent!=cp)
	    nm2=nm2->parent;
	crpos=tree23int_Eval(nm2->position);
	if(rpos+length<crpos+nm2->length)
	    (void)nestedmark_Split(nm2,rpos+length-crpos);
    }

    newnm = nestedmark_NewFromObject(self);
    newnm->parent = cp;
    tt1 = tree23int_New();
    tt2 = tree23int_New();
    procdata.fptree=tt2;
    procdata.fpparent=newnm;
    if (cp->children == NULL)
	cp->children = tree23int_New();
    tree23int_Filter(cp->children,0,tt1,tt2,rpos,rpos+length,FilterProc, &procdata);
    tree23int_Free(cp->children);
    cp->children = tt1;

/*     optimization: if tt2 is empty, put a null in here and ttt_free(tt2)

 */    newnm->children = tt2;

/*     now tt2 is too high by rpos, so decrease it

 */    tree23int_Update(tt2,-rpos,-rpos);
    newnm->position = (struct tree23int *) tree23int_Insert(tt1,rpos,newnm);
    nestedmark_SetLength(newnm,length);

    return newnm;
}

void nestedmark__Delete(self)
struct nestedmark *self;  {
register struct nestedmark *pp;
    int relleft;
    struct filterstruct procdata;

    relleft = tree23int_Eval(self->position);
    pp = self->parent;
    if (pp == NULL) return;	/* failed */
    tree23int_Delete(self->position);	/* delete from parent's 23 tree */
    tree23int_Free(self->position);
    if (self->children != NULL)  {
	procdata.fptree = pp->children;		/* not null, since e is a child */
	procdata.fpparent = pp;
	tree23int_Merge(self->children,pp->children,relleft,FilterProc, &procdata);
	tree23int_Free(self->children);
    }
    nestedmark_Destroy(self);
}

void nestedmark__Update(self, pos, length)
struct nestedmark *self;
long pos;
long length;
{
    register struct nestedmark *up, *tp;
    long tpos, tsize;
    boolean includebeginning;
    boolean includeending;
    long rpos = 0;
    long len;
    boolean ib;
    int offset = 0;

    tp = nestedmark_GetEnclosing(self,pos);
    if (tp == NULL) tp = self;
    includebeginning = ( ! GlobalIsolation) && tp->includeBeginning;
    includeending = ( ! GlobalIsolation) && tp->includeEnding;

    /* fix up the lengths  */

    if (length > 0)  {
        while (tp != NULL)  {
            tpos = nestedmark_Eval(tp);
            ib = FALSE;
            if (tp->parent == NULL
                    || (pos > tpos && pos < tpos+tp->length) 
                    || (ib = (includebeginning && pos == tpos)) 
                    || (includeending && pos == tpos + tp->length))  {
                tp->length += length;
                if (tp->children != NULL)
                    tree23int_Update(tp->children,pos - tpos + offset,length);
            }
            if (tp->position != NULL)
                rpos = tp->position->bump;
            len = tp->length;
            tp = tp->parent;
            if (tp != NULL)  {
                offset = (ib) ? 1 : 0;
                includebeginning = ( ! GlobalIsolation) 
                        && (tp->includeBeginning || ((rpos == 0) ? includebeginning : FALSE));
                includeending = ( ! GlobalIsolation) 
                        && (tp->includeEnding 
                                || ((rpos + len == tp->length) ? includeending : FALSE));
            }
        }
    }
    else  {
        while (length < 0)  {
            up = nestedmark_GetInnerMost(self,pos);
	    tsize = -ncrlength;
	    if (tsize < length) tsize = length;
	    if (up == NULL) up = self;
	    while (up)  {
		tpos = nestedmark_Eval(up);
		if (pos >= tpos && pos < tpos+up->length)
		    if (pos-tsize > tpos + up->length)
			up->length = pos-tpos;
		    else up->length += tsize;
		if (up->length == 0)  {
		    tp = up;
                    up = up->parent;
                    if (up != NULL)
			nestedmark_Delete(tp);
		}
		else  {
		    if (up->children != NULL)
			tree23int_Update(up->children,pos-tpos,tsize);
		    up = up->parent;
		}
	    }
	    length -= tsize;
	}
    }
}

struct nestedmark *nestedmark__GetInnerMost(self, pos)
struct nestedmark *self;
long pos;  {
    register struct nestedmark *tp;
    long  eleft;

/*     first check if it is within our range
 */
    if (self == NULL) return self;
    if (self->parent == 0) self->length = 999999999;
    ncrlength = self->length-pos;
    if (self->children)  {
	tp = (struct nestedmark *) tree23int_FindL(self->children,0,pos);	/* look for the sub-environment */
	if (tp != NULL)  {
	    eleft = tree23int_Eval(tp->position);
	    if (pos >= eleft && pos < eleft+tp->length)  {
		tp = nestedmark_GetInnerMost(tp,pos-eleft);
		lastSelf = self;
		lastPos = pos;
		return tp;
	    }
	}
	tp = (struct nestedmark *) tree23int_FindR(self->children,0,pos);
	if (tp != NULL)
	    ncrlength = tree23int_Eval(tp->position)-pos;
    }
    lastSelf = self;
    lastPos = pos;
    return self;
}

struct nestedmark *nestedmark__GetEnclosing(self, pos)
struct nestedmark *self;
long pos;  {
    register struct nestedmark *tp;
    long  eleft;

/*     first check if it is within our range
 */
    if (self == NULL) return self;
    if (self->parent == 0) self->length = 999999999;
    ncrlength = self->length-pos;
    if (self->children != NULL)  {
	tp = (struct nestedmark *) tree23int_FindL(self->children,0,pos);	/* look for the sub-environment */
	if (tp != NULL)  {
	    eleft = tree23int_Eval(tp->position);
	    if ((pos > eleft && pos < eleft+tp->length) || (pos == eleft && tp->includeBeginning) || (pos == eleft + tp->length && tp->includeEnding))  {
		tp = nestedmark_GetEnclosing(tp,pos-eleft);
		lastSelf = self;
		lastPos = pos;
		return tp;
	    }
	}
	tp = (struct nestedmark *) tree23int_FindR(self->children,0,pos);
	if (tp != NULL)
	    ncrlength = tree23int_Eval(tp->position)-pos;
    }
    lastSelf = self;
    lastPos = pos;
    return self;
}

long nestedmark__Eval(self)
struct nestedmark *self;  {
    register int i;

    i=0;
    while (self != 0)
	{
	if (self->position) i += tree23int_Eval(self->position);
	else return i;
	self = self->parent;
	}
    return i;
}

struct nestedmark *nestedmark__GetCommonParent(self, nmark)
struct nestedmark *self;
struct nestedmark *nmark;  {
    register struct nestedmark *tp;
    register struct nestedmark *up;

    tp = self;
    while (tp != NULL)
	{
	up = nmark;
	while (up != NULL)  {
	    if (up == tp) return tp;
	    up = up->parent;
	}
	tp = tp->parent;
    }
    return NULL;	/* no common parent */
}

void nestedmark__SetLength(self, length)
struct nestedmark *self;
long length;  {
    self->length = length;
}

long nestedmark__GetNextChange(self, pos)
struct nestedmark *self;
long pos;  {
    if (self != lastSelf || pos != lastPos)
	nestedmark_GetInnerMost(self, pos);
    return ncrlength;
}

/* This routine returns the distance from self to namrk.  If it is positive then
nmark is above self.  If it is negative then self is above nmark.  
If it is 0 then they are the same node and if it is nestedmark_UNRELATED
then they are not directly related. */

long nestedmark__Distance(self, nmark)
struct nestedmark *self;
struct nestedmark *nmark;
{
    register int i;
    register struct nestedmark *tmark = self;
    
    for (i = 0, tmark = self; tmark && tmark != nmark; i++, tmark = tmark->parent);
    if (tmark) return i;
    for (i = 0, tmark = nmark; tmark && tmark != self; i++, tmark = tmark->parent);
    if (tmark) return -i;
    return nestedmark_UNRELATED;
}

void nestedmark__SetStyle(self, includebeginning, includeending)
    struct nestedmark *self;
    boolean includebeginning;
    boolean includeending;
{
    self->includeBeginning = includebeginning;
    self->includeEnding = includeending;
}

static boolean IncludesPosition(self, pos)
    struct nestedmark *self;
    long pos;
{
    long markpos;

    markpos = nestedmark_Eval(self);

    return (markpos <= pos && markpos + self->length > pos);
}

struct nestedmark *nestedmark__GetChild(self, pos)
    struct nestedmark *self;
    long pos;
{
    struct nestedmark *child;
    
    if (self->children == NULL)
	return NULL;

    child = (struct nestedmark *) tree23int_Enumerate(self->children, IncludesPosition, pos);

    return child;
}

struct prevchild {
    long pos;
    struct nestedmark *endMark;
    struct nestedmark *previousChild;
};

static boolean PreviousPosition(self, pc)
    struct nestedmark *self;
    struct prevchild *pc;
{
    long markPos ;
    
    if (pc->endMark != NULL)  {
	if (pc->endMark == self)
	    return TRUE;
	else  {
	    pc->previousChild = self;
	    return FALSE;
	}
    }
    else  {
	markPos = nestedmark_Eval(self);
	if (markPos + self->length <= pc->pos)  {
	    pc->previousChild = self;
	    return FALSE;
	}
	else
	    return TRUE;
    }
}

struct nestedmark *nestedmark__GetPreviousChild(self, nm, pos)
    struct nestedmark *self;
    struct nestedmark *nm;
    long pos;
{
    struct prevchild pc;
    
    if (self->children == NULL)
	return NULL;
    
    if (nm == NULL || nm->parent != self)
	nm = nestedmark_GetChild(self, pos);
    pc.endMark = nm;
    pc.pos = pos;
    pc.previousChild = NULL;

    tree23int_Enumerate(self->children, PreviousPosition, &pc);
    return pc.previousChild;
}

static boolean NextPosition(self, pc)
    struct nestedmark *self;
    struct prevchild *pc;
{
    if (pc->endMark != NULL)  {
	if (pc->endMark == self)  {
	    pc->previousChild = self;
	    return FALSE;
	}
	else 
	    return (pc->previousChild != NULL);
    }
    else  {
	return (nestedmark_Eval(self) > pc->pos);
    }
}

struct nestedmark *nestedmark__GetNextChild(self, nm, pos)
    struct nestedmark *self;
    struct nestedmark *nm;
    long pos;
{
    struct prevchild pc;
    
    if (self->children == NULL)
	return NULL;
    
    if (nm == NULL || nm->parent != self)
	nm = nestedmark_GetChild(self, pos);
    pc.endMark = nm;
    pc.pos = pos;
    pc.previousChild = NULL;

    return (struct nestedmark *) tree23int_Enumerate(self->children, NextPosition, &pc);
}

long nestedmark__NumberOfChildren(self)
    struct nestedmark *self;
{
    if (self->children != 0)
	return tree23int_NumberOfLeaves(self->children);
    return 0;
}

	boolean
nestedmark__SetGlobalIsolation(classID, dontextend)
	boolean dontextend;
	struct classheader *classID;
{
	boolean Old = GlobalIsolation;
	GlobalIsolation = dontextend;
	return Old;
}
