/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/lset.c,v 2.7 89/02/25 21:09:21 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/lset.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/lset.c,v 2.7 89/02/25 21:09:21 ghoti Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <class.h>
#include <lset.eh>
#include <dict.ih>
#include <dataobj.ih>
#include <rm.ih>
#include <atom.ih>
#include <atomlist.ih>
#include <text.ih>
#include <ctype.h>
/* #define DEBUG 1 */

#define VALUE 10
#define CEL 5
static struct atom *a_vp,*a_name,*a_atomlist;

static registerobject(self)
struct lset *self;
{
    struct atomlist *al;
    char buf[256];
    if(self->refname == NULL || *self->refname == '\0') return;
    sprintf(buf,"DOBJ-%s",self->refname);
    al = atomlist_StringToAtomlist(buf);
    if(self->dobj == NULL) {
	printf("BOGUS Posting of NULL for %s\n",buf);
    }
    else{ 
#ifdef DEBUG
	printf("Posting %d for %s (%s)\n",(long)self->dobj,buf,class_GetTypeName(self->dobj)); 
#endif /* DEBUG */
	rm_PostResource(al,(long)self->dobj,a_vp);
	lset_Put(self,a_name,a_atomlist,al);
    }
}
static struct dataobject *getregisteredobject(self)
struct lset *self;
{
    struct atomlist *al;
    char buf[256];
    long val;
    if(self->refname == NULL || *self->refname == '\0') return NULL;
    sprintf(buf,"DataObject-%s",self->refname);
    al = atomlist_StringToAtomlist(buf);
    if(rm_GetResource(al,al,a_vp,&val))
	return (struct dataobject *)val;
    return NULL;
}
char *lset__registername(self,name)
struct lset *self;
char *name;
{
    strcpy(self->refname,name);
    if(getregisteredobject(self) != NULL){
	*self->refname = '\0';
	return NULL;
    }
    return self->refname;
}

boolean lset__InitializeClass(ClassID)
struct classheader *ClassID;
{
    
    a_vp = atom_Intern("struct dataobject *");
    a_name = atom_Intern("name");
    a_atomlist = atom_Intern("atomlist");
    return TRUE;
}
void lset__InsertObject (self, name,viewname)
struct lset *self;
char *name;
char *viewname;
{
    struct dataobject *newobject;
    char buf[128];
    char *val = "value";
    if(name == NULL || *name == '\0') newobject = NULL;
    else if((newobject = getregisteredobject(self)) == NULL){
	if((self->application == VALUE) && !class_IsTypeByName(name,"value")) {
	    sprintf(buf,"%sV",name);
	    name = val;
	}
	if(name == val || (newobject = (struct dataobject *) class_NewObject(name)) == NULL) {
	    if(self->application == VALUE){
		if((newobject = (struct dataobject *) class_NewObject("value")) == NULL) return;
		if(viewname == NULL || *viewname == '\0'){
		    viewname = buf;
		}
	    }
	    else return;
	}
    }
    if(newobject){
	newobject->id = dataobject_UniqueID(newobject); 
	/* 	    Register the object with the dictionary */
	dictionary_Insert(NULL,(char *)newobject->id,(char *) newobject);
    }
    if(viewname == NULL || *viewname == '\0'){
	if(newobject == NULL) return;
	viewname = dataobject_ViewName(newobject);
    }
    self->dobj = newobject;
    if(viewname)strcpy(self->viewname,viewname);
    if(name)strcpy(self->dataname,name);
    registerobject(self);
}

long lset__GetModified(self)
struct lset *self;
{
    register long mod = super_GetModified(self);
    if(self->dobj)
	mod += dataobject_GetModified(self->dobj);
    if(self->left)
	mod += dataobject_GetModified(self->left);
    if(self->right)
	mod += dataobject_GetModified(self->right);
    return mod;
}
	
static char *getline(buf,c)
register char *buf,*c;
{
    char *index();
/* printf("Getting line from %s\n",buf); */
    if(buf == NULL || *buf == '\0'){
	*c = '\0';
	return NULL;
    }
    if(*buf == '>' && index(buf,'<') != NULL) {
	buf = index(buf,'<');
	buf++;
    }
    while(*buf == ' ') buf++;
    while((*c = *buf++) != '\n') {
/*	putchar(*c); */
	if(*c++ == '\0') return NULL;
    }
    *c = '\0';
    return (buf);
}

long lset__Read(self, file, id)
    struct lset *self;
    FILE *file;
    long id;
{
    long endcount = 1;
    boolean begindata;
    char *s;
    long c,version;
    long status;
    char objectname[200],*cp;
    long objectid;
    struct dataobject *newobject = NULL;
    char cbuf[2048];
    char *buf,*index();
    long textpending;
    long did,lid,rid;
    did = lid = rid = 0l;
    textpending = 0;
    buf = cbuf;

    lset_SetID(self,lset_UniqueID(self));/* change id to unique number */
    while (endcount != 0)  {
        while ((c = getc(file)) != EOF && c != '\\')  {
	    if(endcount == 1){
#ifdef DEBUG
putchar(c);
#endif /* DEBUG */
	    *buf++ = c;
	    }
        }
        if (c == EOF) return dataobject_NOREADERROR;
        if ((c = getc(file)) == EOF)
            return dataobject_PREMATUREEOF;
        if (c == 'b')  {
            begindata = TRUE;
            s = "egindata";
        }
        else if (c == 'e')  {
            begindata = FALSE;
            s = "nddata";
        }
        else  {
	    if(endcount == 1){
		/* Place handling of \x characters here */
		if(c == 'V') {
		    version = 0;
		    while ((c = getc(file)) != EOF && c != '\n')
			if(isdigit(c)) version = (version * 10) + (c - '0');
		    if (c == EOF) return dataobject_NOREADERROR;
		}
	    }
            continue;
        }
        while ((c = getc(file)) != EOF && c == *s) s++;
        if (c == '{' && *s == '\0')  {
            if (begindata) {
                s = objectname;
                while ((c = getc(file)) != EOF && c != ',')
                    *s++ = c;
                if (c == EOF) return dataobject_PREMATUREEOF;
                *s = '\0';
                objectid = 0;
                while ((c = getc(file)) != EOF && c != '}')
                    if(c >= '0' && c <= '9')objectid = objectid * 10 + c - '0';
                if (c == EOF) return dataobject_PREMATUREEOF;
		if(((c = getc(file))!= '\n') || (strcmp(objectname,"zip") == 0)) ungetc(c,file);
                /* Call the New routine for the object */
                if ((newobject = (struct dataobject *) class_NewObject(objectname)))  {
                    /* Register the object with the dictionary */
		    dictionary_Insert(NULL,(char *)objectid, (char *)newobject);
                    /* Call the read routine for the object */
                    status = dataobject_Read(newobject, file, objectid);
		    if (status != dataobject_NOREADERROR){
			printf("ERROR reading %s, %d\n",objectname,status);
			return status; 
		    }
		}
                else {
                    endcount += 1;
		    /* return dataobject_OBJECTCREATIONFAILED; */
		}

	    }
            else  {
                endcount -= 1;
                while ((c = getc(file)) != EOF && c != '}');
		if((c = getc(file))!= '\n') ungetc(c,file);
            }
        }
        else if(endcount == 1){
	    
        /* 	    Place Handling of characters following \  
           */	
	    *buf++ = c;
	}
    }
    sscanf(cbuf,"%d %d %d %ld %ld %ld %d\n" ,&(self->type),&(self->pct),&(self->application),
	 &did,&lid,&rid,&textpending);
    cp = index(cbuf,'\n'); cp++;
    cp = getline(cp,self->dataname);
    cp = getline(cp,self->viewname);
    cp = getline(cp,self->refname);
    *buf = '\0';
    if(textpending){
	self->pdoc = (struct text *) newobject;
	textpending = FALSE;
    }
#ifdef DEBUG
   printf("<%s>\n",cbuf);
   printf("[%d %d '%s' '%s' %ld %ld %ld]\n",self->type,self->pct,self->dataname,self->viewname,
	 did,lid,rid); fflush(stdout); 
#endif /* DEBUG */
    if(did) self->dobj = (struct dataobject *)dictionary_LookUp(NULL,(char *) did);
    if(lid) self->left = (struct dataobject *)dictionary_LookUp(NULL,(char *) lid);
    if(rid) self->right = (struct dataobject *)dictionary_LookUp(NULL,(char *) rid);
    registerobject(self);
#ifdef DEBUG
    printf("dobj = %d, left = %d, right = %d\n",self->dobj, self->left, self->right);
#endif /* DEBUG */
    return dataobject_NOREADERROR;
}

long lset__Write(self,file ,writeid,level)
struct lset *self;
FILE *file;
long writeid;
int level;
{
    long did,lid,rid;
    did = lid = rid = 0l;
    if (self->header.dataobject.writeID == writeid)  return lset_GetID(self);
    self->header.dataobject.writeID = writeid;

    fprintf(file,"\\begindata{lset,%ld}\n",lset_GetID(self));
    fprintf(file,"\\V 1\n"); /* Version Number */
    if(self->dobj){dataobject_Write(self->dobj,file,writeid,level+1); did = dataobject_UniqueID(self->dobj);}
    if(self->left){dataobject_Write(self->left,file,writeid,level+1);lid = dataobject_UniqueID(self->left);}
    if(self->right){ dataobject_Write(self->right,file,writeid,level+1);rid = dataobject_UniqueID(self->right);}
    fprintf(file,"%d %d %d %ld %ld %ld %d\n>OBJ< %s\n>VIEW< %s\n>REF< %s\n" ,self->type,self->pct,self->application,
	 did,lid,rid,(self->pdoc != NULL),self->dataname,self->viewname,self->refname);
    if(self->pdoc){
	text_Write(self->pdoc,file,writeid,level+1);
    }
    fprintf(file,"\\enddata{lset,%ld}\n",lset_GetID(self));
    return lset_GetID(self);
}

boolean lset__InitializeObject(classID, self)
struct classheader *classID;
struct lset *self;
{
*self->dataname = '\0';
*self->viewname = '\0';
*self->refname = '\0';
self->type = 0;
self->pct = 0;
self->revision = 0;
self->dobj = NULL;
self->left = NULL;
self->right = NULL;
self->application = FALSE;
lset_SetID(self,lset_UniqueID(self));
self->pdoc = NULL;
return TRUE;
}
