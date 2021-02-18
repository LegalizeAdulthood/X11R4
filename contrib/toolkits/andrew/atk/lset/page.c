/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/page.c,v 1.5 89/09/21 11:57:50 ajp Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/page.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/page.c,v 1.5 89/09/21 11:57:50 ajp Exp $ ";
#endif /* lint */

/* This code is taken , in part, from the switcher inset of N. Borenstein's
Andrew Toolkit Book . It has been modified and used with the permission
of the author */

#include <andrewos.h>
#include <page.eh>
#include <dataobj.ih>
#include <cel.ih>

#define page_BYDATAOBJECT -10
extern char *index();

boolean page__InitializeObject(c, self)
struct classheader *c;
struct page *self;
{
    self->FirstSwitchee = NULL;
    self->NowPlaying = NULL;
    self->PostMenusFlag = TRUE;
    return(TRUE);
}

void page__FinalizeObject(c, self)
struct classheader *c;
struct page *self;
{
    struct page_switchee *sw;
    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
	dataobject_Destroy(sw->d);
	free(sw->label);
	free(sw->viewname);
    }
}
static struct page_switchee *FindSwitchee(self, d,which)
register struct page *self;
register struct dataobject *d;
register long which;
{
    register struct page_switchee *sw = NULL;
    register struct page_switchee *Now = self->NowPlaying;
    switch( which){
	case page_CURRENT:
	    sw = self->FirstSwitchee;
	    break;
	case page_AFTERCURRENT:
	    if(Now != NULL){
		if((sw = Now->next) == NULL)
		    sw = self->FirstSwitchee;
	    }
	    break;
	case page_BEFORECURRENT :
	    if(Now != NULL){
		if(Now == self->FirstSwitchee)
		    Now = NULL;
		for (sw = self->FirstSwitchee; sw; sw = sw->next) {
		    if (sw->next == Now) break;
		}
	    }
	    break;
	case page_ATEND :
	    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
		if (sw->next == NULL) break;
	    }
	    break;
	case page_ATBEGINING :
	    sw = self->FirstSwitchee;
	    break;
	case page_BYDATAOBJECT:
	    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
		if (sw->d == d) break;
	    }
	    break;
	default:
	    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
		if (--which <= 0) break;
	    }
	    break;
    }
    return sw;
}
static boolean SetSwitchee(self,sw)
struct page *self;
struct page_switchee *sw;
{
    if(sw == self->NowPlaying) return(FALSE); /* no change */
    self->NowPlaying = sw;
    page_NotifyObservers(self,observable_OBJECTCHANGED);
    return(TRUE);
}

long page__Write(self,fp ,writeid,level)
struct page *self;
FILE *fp;
long writeid;
int level;
{
    struct page_switchee *sw;
    if (page_GetWriteID(self) != writeid) {
	page_SetWriteID(self, writeid);

	fprintf(fp, "\\begindata{page,%ld}\n",
		page_GetID(self));
	for (sw = self->FirstSwitchee; sw; sw=sw->next) {
	    if (sw == self->NowPlaying) {
		fprintf(fp, "*%s\n", sw->label);
	    } else {
		fprintf(fp, "%s\n", sw->label);
	    }
	    dataobject_Write(sw->d, fp, writeid, level+1);
	    fprintf(fp, "\\view{%s}\n", sw->viewname);
	}
	fprintf(fp,"\\enddata{page,%ld}\n",
		page_GetID(self));
    }
    return page_GetID(self);
}

long page__Read(self, fp, id)
struct page *self;
FILE *fp;
long id;
{
    char LineBuf[250], Label[250], *s, *obidstr, *thisname;
    int status, obid;
    struct dataobject *newob = NULL;

    while (TRUE) {
	if (fgets(Label, sizeof(Label)-1, fp) == NULL) {
	    return(dataobject_PREMATUREEOF);
	}
	if (!strncmp(Label, "\\enddata{page", 13)) {
	    break;
	}
	if (fgets(LineBuf, sizeof(LineBuf)-1, fp) == NULL) {
	    return(dataobject_PREMATUREEOF);
	}
	if (strncmp(LineBuf, "\\begindata{", 11)) {
	    return(dataobject_BADFORMAT);
	}
	thisname = &LineBuf[11];
	obidstr = index(thisname, ',');
	if (!obidstr) return(dataobject_BADFORMAT);
	*obidstr++ = '\0';
	s = index(obidstr, '}');
	if (!s) return(dataobject_BADFORMAT);
	*s = '\0';
	obid = atoi(obidstr);
	if ((newob = (struct dataobject *)
	     class_NewObject(thisname)))  {
	    status = dataobject_Read(newob, fp, obid);
	    if (status != dataobject_NOREADERROR) {
		return status;
	    }
	} else {
	    return(dataobject_OBJECTCREATIONFAILED);
	}
	if (fgets(LineBuf, sizeof(LineBuf)-1, fp) == NULL) {
	    return(dataobject_PREMATUREEOF);
	}
	if (strncmp(LineBuf, "\\view{", 6)) {
	    return(dataobject_BADFORMAT);
	}
	thisname = &LineBuf[6];
	s = index(thisname, '}');
	if (s) *s = '\0';
	s = index(Label, '\n');
	if (s) *s = '\0';
	if (Label[0] == '*') {
	    if (!page_AddObject(self, newob,
				    Label+1, thisname,page_ATEND)) {
		return(dataobject_OBJECTCREATIONFAILED);
	    }
	    page_SetNowPlaying(self, newob);
	} else if (!page_AddObject(self, newob,
				       Label, thisname,page_ATEND)) {
		return(dataobject_OBJECTCREATIONFAILED);
	}
    }
    if(self->NowPlaying == NULL && self->FirstSwitchee != NULL)
	SetSwitchee(self,self->FirstSwitchee);
    return dataobject_NOREADERROR;

}


boolean page__AddObject(self, d, label, viewname,which)
struct page *self;
struct dataobject *d;
char *label, *viewname;
{
    struct page_switchee *sw, *swtmp;

    sw = (struct page_switchee *) malloc(sizeof(struct page_switchee));
    if (sw == NULL) return(FALSE);
    if (label == NULL) label = "Next object";
    if (viewname == NULL) viewname = dataobject_ViewName(d);
    if (viewname == NULL) viewname = "view";
    sw->d = d;
    sw->label = malloc(1+strlen(label));
    if (sw->label == NULL) return(FALSE);
    strcpy(sw->label, label);
    sw->viewname = malloc(1+strlen(viewname));
    if (sw->viewname == NULL) return(FALSE);
    strcpy(sw->viewname, viewname);
    sw->next = NULL;

    /* find right place to put it 
      swtmp is set to the existing object that should be made to point to the new object 
      or NULL if the new object should be first */
    switch(which){
	case page_CURRENT:
	    /* inserting as the current object is the same as 
	     inserting after the current object , except that the current object will be updated */
	case page_AFTERCURRENT:
	    swtmp = self->NowPlaying;
	    break;	
	case page_BEFORECURRENT:
	case page_ATEND:
	    swtmp = FindSwitchee(self, NULL ,which);
	    break;
	case page_ATBEGINING:
	case 0:
	case 1:
	    /* insert as first object */
	    swtmp = NULL;
	    break;
	default:
	    /* insert as numbered object */
	    swtmp = FindSwitchee(self, NULL ,--which);
	    break;
    }
    if(swtmp){
	/* insert after swtmp */
	sw->next = swtmp->next;
	swtmp->next = sw;
    }
    else {
	/* insert as first object */
	sw->next = self->FirstSwitchee;
	self->FirstSwitchee = sw;
	}
    if (self->NowPlaying == NULL || which == page_CURRENT) {
	SetSwitchee(self,sw);
    }
    return(TRUE);
}

boolean page__DeleteObject(self, d)
struct page *self;
struct dataobject *d;
{
    struct page_switchee *sw, *prevsw;

    for(prevsw = NULL, sw = self->FirstSwitchee;
	 sw != NULL;
	  sw = sw->next) {
	if (sw->d == d) {
	    if (prevsw != NULL) {
		prevsw->next = sw->next;
	    } else {
		self->FirstSwitchee = sw->next;
	    }
/*	    dataobject_Destroy(sw->d); */
	    free(sw->label);
	    free(sw->viewname);
	    if(self->NowPlaying == sw){
		self->NowPlaying = self->FirstSwitchee;
		page_NotifyObservers(self,0);
	    }
	    return(TRUE);
	}
	prevsw = sw;
    }
    return(FALSE);
}
static char * page__GetSwitcheeName(self,sw)
struct page *self;
struct page_switchee *sw;
{

    if(class_IsTypeByName(class_GetTypeName(sw->d),"cel")){
	return cel_GetRefName((struct cel *) sw->d);
    }
    return(sw->label);
}
static char * page__GetNowPlayingName(self)
struct page *self;
{
    struct page_switchee *sw;

    sw = self->NowPlaying;
    if(class_IsTypeByName(class_GetTypeName(sw->d),"cel")){
	return cel_GetRefName((struct cel *) sw->d);
    }
    return(sw->label);
}

boolean page__SetNowPlaying(self, d)
struct page *self;
struct dataobject *d;
{
    struct page_switchee *sw;

    sw = FindSwitchee(self,d,page_BYDATAOBJECT);
    if(sw == NULL) return(FALSE);
    SetSwitchee(self,sw);
    return(TRUE);

}
boolean page__SetNowPlayingByName(self, name)
struct page *self;
char *name;
{
    struct page_switchee *sw;

    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
	if (strcmp(name,page_GetSwitcheeName(self,sw)) == 0) {
	    SetSwitchee(self,sw);
	    return(TRUE);
	}
    }
    return(FALSE);
}
boolean page__SetNowPlayingByPosition(self, number)
struct page *self;
long number;
{
    struct page_switchee *sw;
    sw = FindSwitchee(self,NULL,number);
    if(sw == NULL) return(FALSE);
    SetSwitchee(self,sw);
    return(TRUE);
}
struct dataobject *page__GetObjectAtPosition(self, number)
struct page *self;
long number;
{
    struct page_switchee *sw;
    sw = FindSwitchee(self,NULL,number);
    if(sw == NULL) return NULL;
    return sw->d;
}
struct dataobject *page__GetObjectByName(self, name)
struct page *self;
char *name;
{
    struct page_switchee *sw;
    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
	if (strcmp(name,page_GetSwitcheeName(self,sw)) == 0) {
	    return sw->d;
	}
    }
    return NULL;
}
char  *page__GetNameOfObject(self, d)
struct page *self;
struct dataobject *d;
{
    struct page_switchee *sw;
    sw = FindSwitchee(self,d,page_BYDATAOBJECT);
    if(sw == NULL) return NULL;
    return page_GetSwitcheeName(self,sw);
}
long page__GetPositionOfObject(self, d)
struct page *self;
struct dataobject *d;
{
    struct page_switchee *sw;
    long count = 0L;
    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
	count++;
	if(sw->d == d) return count;
    }
    return 0l;
}
long page__GetObjectCount(self)
struct page *self;
{
    struct page_switchee *sw;
    long count = 0L;
    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
	count++;
    }
    return count;
}
char *page__ViewName(self)
struct page *self;
{
    return("pagev"); 
}
long page__GetModified(self)
struct page *self;
{
#if 0
    long maxSoFar,x;
    struct page_switchee *sw;

    maxSoFar = super_GetModified(self);
    if (self->executingGetModified)
	return 0;
    self->executingGetModified = TRUE;

    for (sw = self->FirstSwitchee; sw; sw = sw->next) {
	if(sw->d){
	    x = dataobject_GetModified(sw->d);
	    if (x > maxSoFar)
		maxSoFar = x;
	}
    }
    self->executingGetModified = FALSE;

    return maxSoFar;
#else 
    return super_GetModified(self);
#endif
}
