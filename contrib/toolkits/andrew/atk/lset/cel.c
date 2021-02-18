/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/cel.c,v 2.12 89/08/05 15:26:53 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/cel.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/cel.c,v 2.12 89/08/05 15:26:53 tpn Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <class.h>
#include <cel.eh>
#include <dataobj.ih>
#include <dict.ih>
#include <proctbl.ih>
#include <envrment.ih>
#include <text.ih>
#include <ctype.h>
#define VALUE 10
static long viewID = 0;

static SetVisible(self)
struct cel *self;
{
    cel_SetVisible(self);
}
static SetInvisible(self)
struct cel *self;
{
    cel_SetInvisible(self);
}
boolean cel__InitializeObject(classID, self)
struct classheader *classID;
struct cel *self;
{
    self->refname = NULL;
    self->viewType = NULL;
    self->dataType = NULL;
    self->dataatm = NULL;
    self->viewatm = NULL;
    self->refatm = NULL;
    self->linkatm = NULL;
    self->linkname = NULL;
    self->viewID = viewID++;
    self->dataObject = NULL;
    self->desw = self->desh = 0;
    self->script = NULL;
    self->application = FALSE;
    self->script = NULL;
    self->readfromfile = 0;
    self->usedefaultview = 0;
    self->mode = cel_VISIBLE;
    return TRUE;
}

struct cel *cel__Create(classID, viewType, dataObject)
struct classheader *classID;
    char *viewType;
    struct dataobject *dataObject;
{
    struct cel *newvr;
    
    if ((newvr = cel_New()))  {
	if ((newvr->viewatm = atom_Intern(viewType))!= NULL) {
	    newvr->viewType = atom_Name(newvr->viewatm);
	    newvr->dataObject = dataObject;
	    return newvr;
	}
    }
    fprintf(stderr, "Could not allocate cel structure - exiting\n");
    exit(1);
}
char *cel__SetRefName(self,refname)
struct cel *self;
char *refname;
{
    if(refname){
	if((self->refatm = atom_Intern(refname)) != NULL)
	    self->refname = atom_Name(self->refatm);
	}
    return self->refname;
}
void cel__UnsetRefName(self)
struct cel *self;
{
    if(self->refname && *self->refname){
	free(self->refname);
	self->refname = NULL;
    }
}
struct dataobject *cel__GetObject(self)
struct cel *self;
{
    return (self->dataObject);
}
boolean cel__SetChildObject(self,newobject,viewName)
struct cel *self;
struct dataobject *newobject;
char *viewName;
{
    if(viewName == NULL || *viewName == '\0')
	cel_SetViewName(self,viewName,TRUE);
    else cel_SetViewName(self,viewName,FALSE);
    cel_SetObject(self,newobject);
    cel_SetRefName(self,self->dataType);
    return TRUE;
}
long cel__GetModified(self)
register struct cel *self;
{
    register long mod = super_GetModified(self);
    if(self->NoSave) return mod;
    if(self->dataObject)
	mod += dataobject_GetModified(self->dataObject);
    if(self->script)
	mod += text_GetModified(self->script);
    return mod;
}
boolean cel__SetObject(self,newobject)
struct cel *self;
struct dataobject *newobject;
{
    if(newobject){
	newobject->id = dataobject_UniqueID(newobject); 
	/* 	    Register the object with the dictionary */
	dictionary_Insert(NULL,(char *)newobject->id,(char *) newobject);
	self->dataObject = newobject;
	cel_SetObjectName(self,class_GetTypeName(newobject));
	if(self->usedefaultview)
	    cel_SetViewName(self,NULL,TRUE);
	if(strcmp(self->dataType,"value")== 0) 
	    self->application =  VALUE;
	return TRUE;
    }
    return FALSE;
}
boolean cel__SetObjectByName(self,dataname)
struct cel *self;
char *dataname;
{
    struct dataobject *newobject;
    if((dataname == NULL || *dataname == '\0')) return FALSE;
    if((newobject = (struct dataobject *) class_NewObject(dataname)) != NULL) {
	cel_SetObject(self,newobject);
	return TRUE;
    }
    return FALSE;
}
void cel__SetObjectName(self,dataname)
struct cel *self;
char *dataname;
{
    if(dataname && *dataname && 
	(self->dataatm = atom_Intern(dataname))!= NULL) {
	self->dataType = atom_Name(self->dataatm);
    }
}
    
void cel__SetViewName(self,viewname,usedefaultview)
struct cel *self;
char *viewname;
int usedefaultview;
{
    if(viewname && *viewname){
	self->usedefaultview = FALSE;
    }
    else {
	self->usedefaultview = usedefaultview;
	if(usedefaultview && self->dataObject)
	   viewname = dataobject_ViewName(self->dataObject);
    }
    if (viewname && *viewname && 
	 (self->viewatm = atom_Intern(viewname))!= NULL) {
	    self->viewType = atom_Name(self->viewatm);
    }
}
void cel__SetLinkName(self,linkname)
struct cel *self;
char *linkname;
{
    if (linkname && *linkname && 
	 (self->linkatm = atom_Intern(linkname))!= NULL) {
	    self->linkname = atom_Name(self->linkatm);
    }
    else{
	self->linkatm = NULL;
	self->linkname = NULL;
    }
}
void cel__SetApplication(self,app)
struct cel *self;
int app;
{
    if(self->dataType != NULL){
	if(app != cel_VALUE && strcmp(self->dataType,"value") == 0) {
	self->application = cel_VALUE;
	return ;
	}
    }
    self->application = app;
}

void cel__InsertObject (self,newobject, dataname,viewname,usedefaultview)
struct cel *self;
struct dataobject *newobject;
char *dataname;
char *viewname;
int usedefaultview;
{
    char buf[128];
    if(newobject != NULL){
	dataname = class_GetTypeName(newobject);
    }
    else if((dataname == NULL || *dataname == '\0')) /* no object */;
    else{
	if((newobject = (struct dataobject *) class_NewObject(dataname)) == NULL) {
	    if(self->application == VALUE){
		if((newobject = (struct dataobject *) class_NewObject("value")) == NULL) return;
		if(viewname == NULL || *viewname == '\0'){
		    viewname = buf;
		    sprintf(buf,"%sV",dataname);
		}
	    }
	    else return;
	}
    }
    if(newobject){
	newobject->id = dataobject_UniqueID(newobject); 
	/* 	    Register the object with the dictionary */
	dictionary_Insert(NULL,(char *)newobject->id,(char *) newobject);
	self->dataObject = newobject;
    }
  
 
    if(dataname && *dataname){
	 if ((self->dataatm = atom_Intern(dataname))!= NULL) {
	    self->dataType = atom_Name(self->dataatm);

	}
    }
    if(self->dataType && (strcmp(self->dataType,"value")== 0)) 
	self->application =  VALUE;
#ifdef DEBUG
printf("Initing v = %s, d = %s, r = %s\n",self->viewType,self->dataType,self->refname);
#endif /* DEBUG */
}
struct atom *getline(place)
char **place;
{
    char tmpbuf[512];
    char *c = tmpbuf;
    char *buf;
    char *index();
    buf = *place;
#ifdef DEBUG
printf("GETLINE GOT ---- %s XXXXXXX\n",*place);
#endif /* DEBUG */
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
    *place = buf;
    return atom_Intern(tmpbuf);
}
long cel__Read(self, file, id)
    struct cel *self;
    FILE *file;
    long id;
{
    long endcount = 1;
    boolean begindata;
    char *s;
    long c;
    long status;
    char objectname[200],*cp;
    long objectid;
    struct dataobject *newobject = NULL;
    char cbuf[2048];
    char *buf,*index();
    long textpending;
    long did,version = 0l;
    did = 0l;
    textpending = 0;
    buf = cbuf;
/* printf("In Cel Read\n"); */
    cel_SetID(self,cel_UniqueID(self));/* change id to unique number */
    while (endcount != 0)  {
        while ((c = getc(file)) != EOF && c != '\\')  {
	    if(endcount == 1){
		/* Place actual read code here */
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
		if((c = getc(file))!= '\n' || (strcmp(objectname,"zip") == 0)) ungetc(c,file);
                /* Call the New routine for the object */
		if( buf == cbuf && endcount == 1 && version == 0 && id == 0 &&
		   (strcmp(objectname,class_GetTypeName(self)) == 0) ){
		    /* reading the begindata for this object */
		    id = 1;
printf("Skipping Self\n");
		    continue;
		}
                if ((newobject = (struct dataobject *) class_NewObject(objectname)))  {
                    /* Register the object with the dictionary */
		    dictionary_Insert(NULL,(char *)objectid, (char *)newobject);
                    /* Call the read routine for the object */
                    status = dataobject_Read(newobject, file, objectid);
		    if (status != dataobject_NOREADERROR) {
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
/*    cp = index(cbuf,'\n'); cp++; */
    cp = cbuf;
    if (*cp == '\n') cp++;
    switch(version){
	case 2:
	    sscanf(cp,"%d %ld %d %d %d %d\n" ,&(self->application),
		   &did,&textpending,&(self->desw), &(self->desh),&(self->mode));
	    break;
	default:
	    sscanf(cp,"%d %ld %d %d %d\n" ,&(self->application),
		   &did,&textpending,&(self->desw), &(self->desh));
    }
    while (*cp != '\n') cp++;
    cp++;

    if((self->dataatm = getline(&cp)) != NULL)self->dataType = atom_Name(self->dataatm);
    else self->dataType = NULL;
    if((self->viewatm = getline(&cp)) != NULL) self->viewType =atom_Name(self->viewatm);
    else self->viewType = NULL;
    if((self->refatm = getline(&cp)) != NULL) self->refname =atom_Name(self->refatm);
    else self->refname = NULL;
    if((self->linkatm = getline(&cp)) != NULL) self->linkname =atom_Name(self->linkatm);
    else self->linkname = NULL;
    *buf = '\0';
    if(textpending){
	self->script = (struct text *) newobject;
	textpending = FALSE;
    }
    self->readfromfile = TRUE;
#ifdef DEBUG
   printf("<%s>\n",cbuf);
   printf("['%s' '%s' '%s' %ld]\n",self->dataType,self->viewType,self->refname,did); fflush(stdout); 
#endif /* DEBUG */
    if(did) self->dataObject = (struct dataobject *)dictionary_LookUp(NULL,(char *) did);
    else if((self->linkname == NULL  || *(self->linkname) == '\0') && self->dataType != NULL){
	if(self->viewType == NULL || *(self->viewType) == '\0')
	    self->usedefaultview = TRUE;
	cel_SetObjectByName(self,self->dataType);
    }
/*    registerobject(self); Assume arbiter will do this when view is linked.
      Unfortunately, this meens it will be unavailable until then .
      Maybe the arbiter should save a list of the objects and names */
#ifdef DEBUG
    printf("dobj = %d\n",self->dataObject);
#endif /* DEBUG */
    return dataobject_NOREADERROR;
}

long cel__WriteLink(self,file ,writeid,level)
struct cel *self;
FILE *file;
long writeid;
int level;
{
    long val;
    struct dataobject *dob;
    dob = self->dataObject;
    self->dataObject = NULL;
    val = cel_Write(self,file ,writeid,level);
    self->dataObject = dob;
    return val;
}
long cel__Write(self,file ,writeid,level)
struct cel *self;
FILE *file;
long writeid;
int level;
{
    long did;
    did = 0l;
    if (self->header.dataobject.writeID == writeid)  return cel_GetID(self);
    if(level == 0 && *(self->refname) == '\0' && self->script == NULL && 
	self->application == cel_APPLICATION && 
	self->dataObject != NULL && 
	strcmp(class_GetTypeName(self),"arbiter") == 0 &&
	class_IsTypeByName(class_GetTypeName(self->dataObject),"text") &&
	( text_GetExportEnvironments((struct text *) self->dataObject) == FALSE ||  
	environment_NumberOfChildren(((struct text *) self->dataObject)->rootEnvironment) <= 0))
	/* don't write out self over plain text */
	level--;
    if(level != -1){
	self->header.dataobject.writeID = writeid;
	fprintf(file,"\\begindata{%s,%ld}\n",class_GetTypeName(self),cel_GetID(self));
	fprintf(file,"\\V 2\n"); /* Version Number */
    }
#ifdef WOULDWORKBUT
    if(self->dataObject) did = dataobject_Write(self->dataObject,file,writeid,level+1);
#else /* WOULDWORKBUT */
    if(self->dataObject){dataobject_Write(self->dataObject,file,writeid,level+1); did = dataobject_UniqueID(self->dataObject);}
#endif /* WOULDWORKBUT */
    if(level != -1){
	if(self->linkname)
	    fprintf(file,"%d %ld %d %d %d %d \n>OBJ< %s\n>VIEW< %s\n>REF< %s\n>LINK< %s\n" ,self->application,
		    did,(self->script != NULL),self->desw, self->desh,self->mode,(self->dataType)?self->dataType:"", (self->viewType)?self->viewType:"",(self->refname)?self->refname:"",(self->linkname)?self->linkname:"");
	else 
	    fprintf(file,"%d %ld %d %d %d %d \n>OBJ< %s\n>VIEW< %s\n>REF< %s\n" ,self->application,
		    did,(self->script != NULL),self->desw, self->desh,self->mode,(self->dataType)?self->dataType:"", (self->viewType)?self->viewType:"",(self->refname)?self->refname:"");
	if(self->script){
	    text_Write(self->script,file,writeid,level+1);
	}
	fprintf(file,"\\enddata{%s,%ld}\n",class_GetTypeName(self),cel_GetID(self));
    }
    return cel_GetID(self);
}
void cel__SetVisibilityBit(self,mode)
struct cel *self;
int mode;
{
    if(mode != self->mode){
	self->mode = mode;
	cel_NotifyObservers(self,0);
    }
}
boolean cel__InitializeClass(classID)
struct classheader *classID;
{
    proctable_DefineProc("cel-set-visible", SetVisible,&cel_classinfo,NULL, "Make cel visible");
    proctable_DefineProc("cel-set-invisible", SetInvisible,&cel_classinfo,NULL, "Make cel invisible");
    return TRUE;
}

static searchatt(self,attname,len)
struct cel *self;
char *attname;
long *len;
{
    long tlen,i,attlen,j;
    attlen = strlen(attname);
    tlen = text_GetLength(self->script);
    for(i = 0; ((i = text_Index(self->script,i,'<',tlen - i)) != EOF);){
	i++;
	if((i + attlen < tlen) && (text_GetChar(self->script,i + attlen) == '>') && text_Strncmp(self->script,i,attname,strlen(attname)) == 0){
	    i += attlen;
	    j = text_Index(self->script,i,'(', tlen - i);
	    if(j != EOF) {
		j++;
		i = text_Index(self->script,j,')', tlen - j);
		if(i == EOF) return -1;
		*len = i - j;
		return j;
	    }
	}
    }
    return -1;
}
void cel__SetStringAtt(self,attname,attval)
struct cel *self;
char *attname,*attval;
{
    char buf[256];
    long i,len;
    if(self->script == NULL) self->script = text_New();
    if( (i = searchatt(self,attname,&len)) >= 0){
	if(attval == NULL) text_DeleteCharacters(self->script,i,len);
	else text_ReplaceCharacters(self->script,i,len,attval,strlen(attval));
    }
    else if(attval != NULL){
	sprintf(buf,"[string] <%s> (%s)\n",attname,attval);
	text_InsertCharacters(self->script,0,buf,strlen(buf));
    }
    cel_NotifyObservers(self,cel_NeedsRepost);
}
void cel__SetLongAtt(self,attname,val)
struct cel *self;
char *attname;
long val;
{
    char buf[256],attval[64];
    long i,len;
    if(val == cel_UNDEFINEDVALUE) *attval = '\0';
    else sprintf(attval,"%ld",val);
    if(self->script == NULL) self->script = text_New();
    if( (i = searchatt(self,attname,&len)) >= 0){
	text_ReplaceCharacters(self->script,i,len,attval,strlen(attval));
    }
    else if(val != cel_UNDEFINEDVALUE){
	sprintf(buf,"[long] <%s> (%s)\n",attname,attval);
	text_InsertCharacters(self->script,0,buf,strlen(buf));
    }
    cel_NotifyObservers(self,cel_NeedsRepost);
}
long cel__GetLongAtt(self,attname)
struct cel *self;
char *attname;
{
    long i,len;
    char buf[256],*c;
    if(self->script == NULL) return 0;
    if( (i = searchatt(self,attname,&len)) >= 0){
	if(len > 255) len = 255;
	if(len == 0) return cel_UNDEFINEDVALUE;
	for(c = buf; len; len--,i++) *c++ = text_GetChar(self->script,i);
	*c = '\0';
	return(atoi(buf));
    }
    return cel_UNDEFINEDVALUE;
}
char * cel__GetStringAtt(self,attname,buf,buflen)
struct cel *self;
char *attname,*buf;
long buflen;
{
    char *c;
    long i,len;
    if(self->script == NULL) return NULL;
    if( (i = searchatt(self,attname,&len)) >= 0){
	if(len >= buflen) len = buflen - 1;
	for(c = buf; len; len--,i++) *c++ = text_GetChar(self->script,i);
	*c = '\0';
	return(buf);
    }
    return NULL;
}
void cel__FinializeObject(classID, self)
struct classheader *classID;
struct cel *self;
{
}
