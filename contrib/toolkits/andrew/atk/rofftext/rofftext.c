/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.c,v 2.14 89/09/28 08:20:23 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.c,v 2.14 89/09/28 08:20:23 ghoti Exp $ ";
#endif /* lint */

/* rofftext
 *
 * a text object to interpret troff code
 */

#include <class.h>
#include <ctype.h>
#include <andrewos.h> /* sys/time.h strings.h */
#include <text.ih>
#include <style.ih>
#include <stylesht.ih>
#include <envrment.ih>
#include <dataobj.ih>
#include <namespc.ih>
#include <environ.ih>
#include <mark.ih>
#include <attribs.h>

#include <hash.ih>
#include <glist.ih>

#include <rofftext.eh>
/*#include "rofftext.h"*/
#include <roffcmds.h>
#include <roffstyl.h>
#include <roffutil.h>

static int SCALE[8] = 
{    1, /* u, basic unit */
   432, /* i, inch */
   170, /* c, centimeter */
    72, /* P, pica */
    60, /* m, Em */
    30, /* n, En */
     6, /* p, point */
    72 /* v, vertical line space */
};

static char *FONT[5]  = {"roman","italic","bold","special","typewriter"};

/* for when debugging is turned off */

#ifdef DEBUGGING
int ROFFDEBUG = 0;
#endif /* DEBUGGING */

/* Change styles to indent for a certain number of units
 *
 */

SetIndent(self,u)
struct rofftext *self;
int u;
{

    int indentcm = ((u+85)/170); /* convert units to centimeters, round to nearest */
    int i;

    DEBUG((stderr,"Indenting %d cm\n",indentcm));

    for(i=0;i<INDENTLEVELS;i++,indentcm = indentcm >> 1) {
        if (indentcm & 1) {
            DEBUG((stderr,"Opening indent %d\n",i));
            self->IndentStyle[i] = ChangeStyle(self,self->IndentStyle[i],self->IndentName[i]);
        }
        else {
            DEBUG((stderr,"Closing indent %d\n",i));
            self->IndentStyle[i] = ChangeStyle(self,self->IndentStyle[i],NULL);
        }
    }

}

/* set a temporary indent */

SetTempIndent(self,u)
struct rofftext *self;
int u;
{
    int indentcm;
    int i;
    int *style1, *style2;
    char **name;

    if (self->CurrentEnviron->indent > u) {
        DEBUG((stderr,"(negative indent)\n"));
        u = self->CurrentEnviron->indent - u;
        style1 = self->MinusTempIndentStyle;
        style2 = self->TempIndentStyle;
        name = self->MinusTempIndentName;
    }
    else {
        DEBUG((stderr,"(positive indent)\n"));
        u = u - self->CurrentEnviron->indent;
        style1 = self->TempIndentStyle;
        style2 = self->MinusTempIndentStyle;
        name = self->TempIndentName;
    }
    indentcm = ((u+85)/170); /* convert units to centimeters, round to nearest */
    DEBUG((stderr,"Temp Indenting %d cm\n",indentcm));

    for(i=0;i<INDENTLEVELS;i++,indentcm = indentcm >> 1) {
        if (indentcm & 1) {
            DEBUG((stderr,"Opening temp indent %d (%s)\n",i,name[i]));
            style1[i] = ChangeStyle(self,style1[i],name[i]);
        }
        else {
            DEBUG((stderr,"Closing temp indent %d\n",i));
            style1[i] = ChangeStyle(self,style1[i],NULL);
        }
        style2[i] = ChangeStyle(self,style2[i],NULL);
    }

}
/* set and check for beginning of line */

Is_BOL(self)
struct rofftext *self;
{
    return self->v_BOL;
}

/* next character read will be beginning of line */

Set_BOL(self)
struct rofftext *self;
{
    self->v_NextBOL = TRUE;
}

/* destroy an input context structure */

DestroyContext(c)
IC c;
{
    free(c);
}

/* evaluate an expression */
/*
EvalNumber(self,str)
struct rofftext *self;
char *str;
{
    int i;
    sscanf(str,"%d",&i);
    return i;
}
*/
/* open a trickle on a stream or string */

Trickle topen(self,filename,f,s)
struct rofftext *self;
char *filename;
FILE *f;
char *s;
{
    Trickle t = (Trickle)malloc(sizeof(struct trickle));
    struct _trickle *cur = (struct _trickle *)malloc(sizeof(struct _trickle));
    t->t = cur;

    if (filename != NULL) {
        if ((cur->u.f = fopen(filename,"r")) != NULL) {
            cur->type = trickle_File;
        }
        else {
            fprintf(stderr,"rofftext: could not open file %s\n",filename);
            cur->type = trickle_String;
            cur->u.s = cur->buf = "";
        }
        cur->filename = StrDup(filename);
        cur->LineNumber = 0;
        cur->prev = NULL;
        cur->pop = FALSE;
    }
    else if (f != NULL) {
        cur->type = trickle_File;
        cur->u.f = f;
        cur->filename = "*unknown*";
        cur->LineNumber = 0;
        cur->prev = NULL;
        cur->pop = FALSE;
    }
    else if (s != NULL) {
        cur->type = trickle_String;
        cur->u.s = cur->buf = StrDup(s);
        cur->filename = "(none)\n";
        cur->LineNumber = 0;
        cur->prev = NULL;
        cur->pop = FALSE;
    }
    else {
        cur->type = trickle_String;
        cur->u.s = cur->buf = "";
        cur->prev = NULL;
        cur->pop = FALSE;
    }
    return t;
}

tclose(self,t)
struct rofftext *self;
Trickle t;
{
}


/* Get a character from the trickle */

g(self,t)
struct rofftext *self;
Trickle t;
{
    register int c = 0;
    register struct _trickle *cur = t->t;

    switch (cur->type) {
        case trickle_File:
            c = getc(cur->u.f);
            if (c == '\n')
                cur->LineNumber++;
            break;
        case trickle_String:
            c = *cur->u.s++;
            if (c == '\0')
                c = EOF;
            break;
        default:
            DEBUG((stderr,"HELP!  Unknown trickle type in (%x)!!\n",cur));
            exit(1);
            break;
    }
    if ((c == EOF) && (cur->prev != NULL)) {
        t->t = cur->prev;
        if (cur->type == trickle_File)
            fclose(cur->u.f);
        else {
            if (cur->pop) {
                /* pop macro argument stack */
                /* pop args */
                DestroyContext(self->CurrentContext);
                self->CurrentContext = (IC)glist_Pop(self->ArgStack);
            }
            free(cur->buf);
        }
        free(cur);
        c = g(self,t);
    }
    
/* for debugging */
#ifdef DEBUG2
    putc(c,stderr);
#endif /* DEBUG2 */
    return c;
}

/* push a character on a trickle */
ung(self,c,t)
struct rofftext *self;
char c;
Trickle t;
{
    register struct _trickle *cur = t->t;
    switch (cur->type) {
        case trickle_File:
            ungetc(c,cur->u.f);
            if (c == '\n')
                cur->LineNumber--;
            break;
        case trickle_String:
            if (cur->u.s == cur->buf) {
                char *temp = (char *)malloc(strlen(cur->buf)+2);
                *temp = c;
                strcpy((temp+1),cur->u.s);
                free(cur->u.s);
                cur->u.s = temp;
            }
            else
                *(--cur->u.s)=c;
            break;
    }
}

/* push a file or string on a trickle 
 * f is TRUE if we should pop the macro stack on EOF or end-of-string
 */

tpush(self,t,filename,f,s,push,argc,argv)
struct rofftext *self;
Trickle t;
char *filename;
FILE *f;
char *s;
boolean push;
int argc;
char *argv[];
{
    struct _trickle *temp = (struct _trickle *)malloc(sizeof(struct _trickle));
    static char fn[512] = "/usr/man/";

    DEBUG((stderr,"filename: %s  t->t->filename:%s\n",filename,t->t->filename));
    if (filename != NULL) {

#if 0
	strcpy(fn, t->t->filename);
	if (s1 = rindex(fn,'/')) {
	    *s1 = '\0';
	    if (s2 = rindex(fn,'/')) {
		*(s2+1) = '\0';
		DEBUG((stderr,"root name:%s\n",fn));
		strcat(fn,filename);
		DEBUG((stderr,"new name:%s\n",fn));
		exit(1);
	    }
	}
#endif /* 0 */
	
        if ((temp->u.f = fopen(filename,"r")) != NULL) {
            temp->type = trickle_File;
        }
        else {
	    /*
	     * HORRIBLE HACK TO GET THIS TO WORK WITH MAN PAGES
	     */
	    strcpy(fn + sizeof("/usr/man/") - 1, filename);
	    DEBUG((stderr,"fn:%s\n",fn));
	    if ((temp->u.f = fopen(fn,"r")) != NULL) {
		temp->type = trickle_File;
	    }
	    else {
		fprintf(stderr,"rofftext: could not open file %s\n",filename);
		temp->type = trickle_String;
		temp->u.s = temp->buf = "";
	    }
        }
        temp->filename = StrDup(filename);
        temp->LineNumber = 0;
    }
    else if (f != NULL) {
        temp->type = trickle_File;
        temp->u.f = f;
        temp->filename = "input file";
        temp->LineNumber = 0;
    }
    else if (s != NULL) {
        temp->type = trickle_String;
        temp->buf = temp->u.s = StrDup(s);
        if (t->t) {
            temp->filename = t->t->filename;
            temp->LineNumber = t->t->LineNumber;
        }
        else {
            temp->filename = "*unknown*";
            temp->LineNumber = 0;
        }
    }
    else {
        DEBUG((stderr,"********tpush: called with null arguments\n"));
        temp->type = trickle_String;
        temp->buf = temp->u.s = "";
        temp->filename = "(NULL!)";
        temp->LineNumber = 0;
    }
    temp->prev = t->t;
    temp->pop = push;
    if (push) { /* push input context */
        int i;
        glist_Push(self->ArgStack,self->CurrentContext);
        self->CurrentContext = (IC)malloc(sizeof(struct InputContext));
        self->CurrentContext->argv = (char **)malloc(sizeof(char *)*10);
        self->CurrentContext->argc = argc;

        for(i=0;i<argc;i++) {
            self->CurrentContext->argv[i] = StrDup(argv[i]);
        }
        while(i<10)
            self->CurrentContext->argv[i++] = "";
    }

    t->t = temp;
}

/* munch to end of line */
munch(self,t)
struct rofftext *self;
Trickle t;
{
    register int c;

    while ((c = g(self,t)) != '\n' && c != EOF);
    ung(self,'\n',t);
}

/* read the next two input characters and output the corresponding special character */

special(self,t)
struct rofftext *self;
Trickle t;
{
    char temp[3],*result;
    temp[0] = g(self,t);
    temp[1] = g(self,t);
    temp[2] = '\0';
    result = hash_Lookup(self->SpecialChars,temp);
    if (result)
        tpush(self,t,NULL,NULL,result,FALSE,NULL,NULL);
    else {
        temp[0] = 'X';
        temp[1] = '\0';
        tpush(self,t,NULL,NULL,temp,FALSE,NULL,NULL);
    }
}

/* set font according to \\f request */

setfont(self,t)
struct rofftext *self;
Trickle t;
{
    char name[3],*font;
    getname(self,t,name);
    if (isdigit(name[0])) {
        int digit = name[0]-'1';
        if ((digit >= 0) && (digit <= 3))
            font = self->Fonts[digit];
        else {
            DEBUG((stderr,"********WARNING: bogus font (%s)********\n)",font));
            font = self->Fonts[0];
        }
    }
    else
        switch(name[0]) {
            case 'B': font = "bold";
                break;
            case 'I': font = "italic";
                break;
            case 'S': font = "special";
                break;
            case 'P': font = self->CurrentEnviron->prevFont;
                break;
            case 'L': font = "typewriter";
                break;
            default: font = "roman";
                break;

        }

    self->CurrentEnviron->prevFont = self->CurrentEnviron->font;
    self->CurrentEnviron->font = font;
    self->CurrentEnviron->fontStyle = ChangeStyle(self,self->CurrentEnviron->fontStyle,font);
}

/* get the width of a string */

getwidth(self,t)
struct rofftext *self;
Trickle t;
{
    register int c,delim = get(self,t); /*read until this */
    int length = 0;
    char temp[16];

    PushEnviron(self,3);
    while(((c = get(self,t)) != delim) && (c != EOF) && (c != '\n'))
        length++;
    PopEnviron(self);
    sprintf(temp,"%d",length);
    tpush(self,t,NULL,NULL,temp,FALSE,NULL,NULL);
}

/* munch requests for movement */

munchmove(self,t)
struct rofftext *self;
Trickle t;
{
    static BUF Buffer = NULL;
    register int c,delim = get(self,t);

    if (Buffer == NULL)
        Buffer = NewBuf();

    ClearBuf(Buffer);
    DEBUG((stderr,"{{munchmove->}}"));
    while(((c = get(self,t)) != delim) && (c != EOF) && (c != '\n'))
        Add2Buf(Buffer,c);
    DEBUG((stderr,"{{<-munchmove}}"));
    Add2Buf(Buffer,'\0');
    /* do movement here, if you want to */

}


/* returns a 1- or 2-character name, as in \nX or \n(XX */

getname(self,t,name)
struct rofftext *self;
Trickle t;
char *name;
{
    if ((name[0] = g(self,t)) == '(') {
        name[0] = g(self,t);
        name[1] = g(self,t);
        name[2] = '\0';
    }
    else
        name[1] = '\0';
}

/* get point size change request */

getsize(self,t)
struct rofftext *self;
Trickle t;
{
    register int c,d;

    c = g(self,t);
    if ((c == '+') || (c == '-')) {
        d = (c=='-')?(-1):(1);
        d *= (g(self,t)-'0');
        return 0;
    }
    
    d = (c-'0');
    if ((d==6)||(d==7)||(d==8)||(d==9)) {
        return d;
    }
    else {
        c = g(self,t);
        d = 10 * d + (c-'0');
        return d;
    }

}

/* Get the contents of a register, formatted appropriately */

char *getregister(self,t)
struct rofftext *self;
Trickle t;
{
    register int c;
    struct reg *r;
    int inc = 0;
    static char temp[32],*ptr,*fmtstring;


    ptr = temp;
    c = get(self,t);
    if (c == '+') {
        inc = 1;
        c = get(self,t);
    }
    if (c == '-') {
        inc = -1;
        c = get(self,t);
    }
    if (c == '(') {
        *ptr++ = get(self,t);
        *ptr++ = get(self,t);
    }
    else
        *ptr++ = c;
    *ptr = '\0';
    
    r = (Reg)hash_Lookup(self->Registers,temp);
    if (r == NULL) {
        DEBUG((stderr, "Register (%s) not found\n",temp));
        return "0";
    }
    DEBUG((stderr,"Accessing register %s\n",temp));
    switch(r->format) {
        case reg_Single:
            fmtstring = "%d";
            break;
        case reg_Triple:
            fmtstring = "%03d";
            break;
        case reg_LCRoman:
        case reg_UCRoman:
        case reg_LCaz:
        case reg_UCaz:
            fmtstring = "%d"; /* cop out for now */
            break;
    }
    if (inc)
        r->value += (r->autoinc * inc);
    sprintf(temp,fmtstring,r->value);
    return temp;
}


putregister(self,name,value,fmt,inc,relative)
struct rofftext *self;
char *name;
int value;
enum RegFmt fmt;
int inc;
boolean relative;
{
    struct reg *r = (Reg)hash_Lookup(self->Registers,name);
    if (r==NULL) {
        r = NewObj(struct reg);
        r->value = value;
        r->format = fmt;
        r->autoinc = inc;
        hash_Store(self->Registers,name,r);
    }
    else {
        r->value = relative?(r->value + value):value;
        r->format = fmt;
        r->autoinc = inc;
    }
}


/* storing and retrieving named strings */

char *getstring(self,name)
struct rofftext *self;
char *name;
{
    char *str;
    DEBUG((stderr,"Name = (%s)\n",name));
    if ((str = hash_Lookup(self->Macros,name)) == NULL) {
        DEBUG((stderr,"String was Null\n"));
        return "";
    }
    else {
        DEBUG((stderr,"String was (%s)\n",str));
        return str;
    }
}

putstring(self,name,value)
struct rofftext *self;
char *name,*value;
{
    char *v = StrDup(value), *existing;

    DEBUG((stderr,"Storing string (%s) (%s)\n",name,v));
    hash_Delete(self->Commands,name);
    existing = hash_Delete(self->Macros,name);
    if (existing)
        free(existing);
    hash_Store(self->Macros,name,v);
    hash_Store(self->Commands,name,DoMacro);
}

/* gets a (possibly quoted) argument.  Returns "\n" for end of line. 
 * takes an optional argument, for max number of characters allowed in argument
 * (0 for unlimited number)
 *
 */

getarg(self,t,buf,n,copymode)
struct rofftext *self;
Trickle t;
char *buf;
int n;
boolean copymode;
{
    register int c;
    register int count = 0;
    boolean tmp = self->v_CopyMode;

    /* read leading whitespace */
    do {
        c = get(self,t);
    } while((c==' ')||(c=='\t'));

    if (c == '\n') /* sigh */
        *buf++ = '\n';

    if (copymode)
        self->v_CopyMode = TRUE;

    /* read argument */
    if (c == '\"') { /* quoted argument */

        c = get(self,t);
        for(;;){
            if (n && (count++ > n))
                break;
            if (c == '\n')
                break;
            else if (c == '\"')      /* yuck! */
                if ((c = get(self,t)) != '\"')
                    break;
            *buf++ = c;
            c = get(self,t);

        }

    }
    else    /* arg not quoted */
        while((c != ' ') && (c != '\t') && (c != '\n') && (n==0 || count<n)) {
            if (c != NULCHAR) *buf++ = c;
            c = get(self,t);
            ++count;
        }
    if (copymode)
        self->v_CopyMode = tmp;
    *buf = '\0';
    ung(self,c,t);   /* push back space or newline */
}
        
/* Put a character on the output */

put(self,c)
struct rofftext *self;
unsigned char c;
{
    if (self->v_SnarfOutput!=NULL)
        Add2Buf(self->v_SnarfOutput,c);
    else {
        /* beginning of page trap */
        if (!self->v_TrapBlown && self->v_Trap) {
            procedure cmd;

            self->v_TrapBlown = TRUE;
            cmd = (procedure)(long)hash_Lookup(self->Commands,self->v_Trap);
            self->Nullarg[0] = self->v_Trap;
            if (cmd)
                (*cmd)(self,self->Input,0,1,self->Nullarg);
            self->Nullarg[0] = "";
        }
        if (self->HelpMode) {
            if (c == '\n')
                return;
            else {
                self->HelpMode = FALSE;
                /* put one line at the top */
                text_AddInCharacter(self->text,self->pos++,'\n');
            }
        }

        if (c != NULCHAR)
            text_AddInCharacter(self->text,self->pos++,self->Berliz[(int)c]);
    }
    /* 80 chars is completely arbitrary */
    if(self->CurrentDiversion->OutputDone++ > 0) { 
        self->CurrentDiversion->NoSpaceMode = FALSE;
    }
}


/* Do a "break" on the output
 *
 * Two in a row are ignored
 *
 */
DoBreak(self)
struct rofftext *self;
{
    if (self->CurrentDiversion->OutputDone) {
        /*        put(self,'\n');*/
        put(self,'\n');
        self->CurrentDiversion->OutputDone = 0;
        self->v_MultiSpace = FALSE;
        if (self->CurrentDiversion->NoSpaceMode)
            DEBUG((stderr, "Break: turning off no-space-mode - "));
        self->CurrentDiversion->NoSpaceMode = FALSE;
        DEBUG((stderr,"<<BREAK>>"));

        /* handle temporary indent */
        if (self->CurrentEnviron->tempIndent != self->CurrentEnviron->indent) {
            self->CurrentEnviron->tempIndent = self->CurrentEnviron->indent;
            SetTempIndent(self,self->CurrentEnviron->indent);
        }
    }
    DEBUG((stderr, "Did break\n"));
}

/* Get a character, translating quoted characters */

get(self,t)
struct rofftext *self;
Trickle t;
{
    register int c;
    register boolean translated = FALSE;
    char temp[4];
    self->v_BOL = self->v_NextBOL;

    c = g(self,t);
    if (self->v_ReadEscapes == FALSE) { /* don't use escapes at all */
        self->v_NextBOL = (c == '\n');
        return c;
    }

    if (c == self->EscChar) {
        c = g(self,t);

        /* conditionals get translated in raw mode, but not copy mode */
        if (!self->v_CopyMode) {
            translated = TRUE;
            switch(c) {
                case '{': /* begin conditional input */
                    self->v_InCond += 1;
                    c = get(self,t);
                    break;
                case '}': /* end conditional input */
                    self->v_InCond -= 1;
                    if (self->v_InCond<0) {
                        DEBUG((stderr,"**Conditional Nesting Error**\n"));
                        self->v_InCond = 0;
                    }
                    c = get(self,t);
                    break;
                default:
                    translated = FALSE;
                    break;
            }
        }

        /* these characters not translated in copy or raw mode */
        if (!translated && !self->v_CopyMode && !self->v_RawMode) {
            translated = TRUE;
            switch(c) {
                case 'e':   /* printable escape */
                    c = ESCAPE;
                    break;
                case '0':   /* digit space */
                    put(self,' ');
                    c = get(self,t);
                    break;
                case '|':   /* narrow space */
                    c = get(self,t);
                    break;
                case '^':   /* half-narrow space */
                    c = get(self,t);
                    break;
                case '&':   /* null character */ /* must fix to stop \&.xx from interp */
                    c = NULCHAR;
                    break;
                case '!':   /* transparent line ind. */
                    /* read rest of line and plaster on output */
                    break;
                case 'z':   /* zero width character */
                    c = get(self,t);
                    break;
                case '(':   /* special character */
                    special(self,t);
                    c = get(self,t);
                    break;
                case 'f':   /* switch to font */
                    if (self->v_DiversionLevel == 0) {
                        setfont(self,t);
                        c = get(self,t);
                    }
                    else {
                        ung(self,c,t);
                        c = self->EscChar;
                    }
                    break;
                case 's': /* change point size */
                    getsize(self,t);
                    c = get(self,t);
                    break;
                case 'w': /* width of string */
                    getwidth(self,t);
                    c = get(self,t);
                    break;
                case 'v':   /* vert. and horiz. movement */
                case 'h':
                    munchmove(self,t);
                    c = get(self,t);
                    break;
                case 'u':   /* ignore movement */
                case 'd':
                case 'r':
                    c = get(self,t);
                    break;
                case 'k':   /* mark horiz place in reg. */
                    g(self,t); /* throw away next char */
                    c = get(self,t);
                    break;
                case 'c':   /* continuation */
                    c = get(self,t);
                    break;
                default:
                    translated = FALSE;
                    break;
            }
        }

        /* These characters are translated in copy and normal mode */

        if (!translated && !self->v_RawMode) {
            translated = TRUE;
            if (c != self->EscChar) { /* always translate escape */
                switch(c) {
                    case '"':   /* comment */
                        munch(self,t);
                        c = get(self,t);
                        break;
                    case '*':   /* stored string */
                        getname(self,t,temp);
                        tpush(self,t,NULL,NULL,getstring(self,temp),FALSE,NULL,NULL);
                        c = get(self,t);
                        break;
                    case 'n':   /* register output */
                        tpush(self,t,NULL,NULL,getregister(self,t),FALSE,NULL,NULL);
                        c = get(self,t);
                        break;
                    case 'a':   /* non-interpreted leader (?)*/
                        c = get(self,t);
                        break;
                    case 't':   /* tab */
                        put(self,'\t');
                        c = get(self,t);
                        break;
                    case '\n': /* escaped newline */
                        c = get(self,t);
                        self->v_BOL = TRUE;
                        break;
                    case '$': /* argument */
                        temp[0] = get(self,t);
                        temp[1] = '\0';
                        tpush(self,t,NULL,NULL,self->CurrentContext->argv[atoi(temp)],FALSE,NULL,NULL);
                        c = get(self,t);
                        break;
                    default:    /* oops, it wasn't translatable */
                        translated = FALSE;
                        break;
                }
            }
        }

        /* if we couldn't translate it: */
        /* if in normal mode, just print the char */
        /* otherwise, put out the escape char + char */

        if (!translated && (self->v_RawMode || self->v_CopyMode)) {
                ung(self,c,t);
                c = self->EscChar;
        }
    }

    self->v_NextBOL = (c == '\n');
    return c;
}

#define CMD(name,fn) hash_Store(self->Commands,"name",fn)
#define GOOFY(name,fn) hash_Store(self->Goofy,"name",fn)

/* Put the default commands in the dictionary */

CreateDefaultCommands(self)
struct rofftext *self;
{
    CMD(ex,ex_cmd);
    CMD(rm,rm_cmd);
    CMD(rn,rm_cmd);
    CMD(br,br_cmd);
    CMD(nr,nr_cmd);
    CMD(af,af_cmd);
    CMD(rr,rr_cmd);
    CMD(ds,ds_cmd);
    CMD(so,so_cmd);
    CMD(de,de_cmd);
    CMD(di,di_cmd);
    CMD(c0,c0_cmd);
    CMD(c1,c1_cmd);
    CMD(PM,PM_cmd);
    CMD(PA,PA_cmd);
    CMD(wh,wh_cmd);
    CMD(sp,sp_cmd);
    CMD(it,it_cmd);
    CMD(ft,ft_cmd);
    CMD(in,in_cmd);
    CMD(ti,ti_cmd);
    CMD(fi,fi_cmd);
    CMD(nf,nf_cmd);
    CMD(ns,ns_cmd);
    CMD(rs,rs_cmd);
    CMD(ce,ce_cmd);
    CMD(sv,sv_cmd);
    CMD(ig,ig_cmd);
    CMD(ev,ev_cmd);

    /* these commands do nothing but cause a line break */
    CMD(bp,br_cmd); /* ?? */
    CMD(fl,br_cmd);

    GOOFY(if,if_cmd);
    GOOFY(ie,ie_cmd);
    GOOFY(el,el_cmd);
    GOOFY(ds,ds_cmd);
    GOOFY(tl,tl_cmd);
    GOOFY(tr,tr_cmd);
/*    GOOFY(tm,tm_cmd);*/
}




/* Initialize the pre-defined registers */

boolean rofftext__InitializeObject(classID,self)
struct classheader *classID;
struct rofftext *self;
{
    struct timeval tv;
    struct tm *tm;
    int i,mult;
    char temp[64];

    DEBUG((stderr, "ENTER Starting"));
    self->Registers = hash_New();

    DEBUG((stderr, "doing time"));
    gettimeofday(&tv,NULL);
    tm = localtime(&tv.tv_sec);
    putregister(self,"dw",(int)tm->tm_wday+1,reg_Single,0,0);
    putregister(self,"dy",(int)tm->tm_mday,reg_Single,0,0);
    putregister(self,"mo",(int)tm->tm_mon+1,reg_Single,0,0);
    putregister(self,"yr",(int)tm->tm_year,reg_Single,0,0);
    putregister(self,"%",(int)1,reg_Single,0,0);


    DEBUG((stderr, "init stuff"));
    /* initialize commands and macros */
    self->Commands = hash_New();
    self->Macros = hash_New();
    self->Goofy = hash_New();
    self->ArgStack = glist_New();
    self->SpecialChars = hash_New();
    InitChars(self);    /* in roffchrs.c */
    CreateDefaultCommands(self);

    /* initialize arguments */
    DEBUG((stderr, "context"));
    self->CurrentContext = (IC)malloc(sizeof(struct InputContext));
    self->CurrentContext->argv = (char **)malloc(sizeof(char *)*10);
    for(i=0;i<10;i++) {
        self->CurrentContext->argv[i] = "";
        self->Nullarg[i] = "";
    }

    self->CurrentContext->argc = 0;

    DEBUG((stderr, "indents"));
    /* initialize indents */
    for (i=0,mult=1;i<INDENTLEVELS;i++,mult*=2) {
        self->IndentStyle[i]= self->TempIndentStyle[i] = self->MinusTempIndentStyle[i] = 0;
        sprintf(temp,"indent%d",mult);
        self->IndentName[i] = StrDup(temp);
        sprintf(temp,"tempindent%d",mult);
        self->TempIndentName[i] = StrDup(temp);
        sprintf(temp,"tempindentneg%d",mult);
        self->MinusTempIndentName[i] = StrDup(temp);
    }

    DEBUG((stderr, "environment"));
    /* environment stack */
    CreateEnvirons(self);
    self->EnvironStack = glist_New();
    self->DiversionStack = glist_New();
    self->CurrentDiversion = CreateDiversion(self,NULL);

    DEBUG((stderr, "xlation table"));
    /* translation table */
    for(i=0;i<256;i++)
        self->Berliz[i] = i;
    self->EscChar = '\\';
    self->Berliz[ESCAPE] = self->EscChar;

    self->v_InCond = 0;
    self->v_TrapBlown = FALSE;
    self->v_CopyMode = FALSE;
    self->v_SnarfOutput = NULL;
    self->v_ReadEscapes = TRUE;
    self->v_RawMode = FALSE;
    self->v_LastIfResult = TRUE;
    self->v_BOL = TRUE;
    self->v_NextBOL = TRUE;
    self->v_MultiSpace = FALSE;
    self->RoffType = FALSE;
    self->v_Trap = NULL;
    self->v_InputLineCount = 0;
    self->v_InputLineNumber = 0;
    self->v_ErrorCount = 0;
    self->v_TempIndentTrap = 0;
    self->v_BlankLine = TRUE;
    self->v_DiversionLevel = 0;
    self->v_FillMode = TRUE;
    self->current = '\n';
    for(i=0;i<8;i++) {
        self->ScaleFactor[i] = SCALE[i];
    }
    for(i=0;i<5;i++) {
        self->Fonts[i] = FONT[i];
    }
    self->CurrentFontSize = 10;


    DEBUG((stderr, "template"));
    rofftext_ReadTemplate(self,"roff",FALSE);
    self->text = (struct text *)self;

    DEBUG((stderr, "stacks"));
    self->stack = (struct stackelt *)malloc(STACKSIZE*sizeof(struct stackelt));
    self->tempstack = (struct tempelt *)malloc(STACKSIZE*sizeof(struct tempelt));
    self->stack->env = environment_GetEnclosing(self->text->rootEnvironment, 0L);
    self->stack->pos = environment_Eval(self->stack->env);
    self->stack->level = -1;
    self->tempstack->level = -1;
    self->styleID = 1;
    self->pos = 0;

    self->macrofile = NULL;
    self->inputfiles = NULL;
    self->filename = NULL;
    self->HelpMode = FALSE;
    self->BeCompletelyBogus = FALSE;
    self->EndMacros = glist_New();
    self->PrintWarnings = FALSE;
    return TRUE;
}



/* expects a line on the input : " XX YY YY YY YY..."
 * where "XX" is a command,
 * "YY" are arguments.
 *
 */

DoCommand(self,t,name,br)
struct rofftext *self;
Trickle t;
char *name;
boolean br;
{
    int i=0,j;
    char temp[128];
    boolean copy = FALSE;
    procedure cmd = NULL;
    char *Arg[MAXARGS];

    DEBUG((stderr,"<Getting 1st argument,"));
    getarg(self,t,temp,2,copy); 
    Arg[i++] = StrDup(temp);
    DEBUG((stderr,"%s>\n",temp));

    if ((cmd = (procedure)(long)hash_Lookup(self->Goofy,temp)) == NULL) {

        /* if not special syntax command, get rest of arguments */

        if (hash_Lookup(self->Macros,temp) != NULL) {
            copy = TRUE; /* macro args read in copy mode */
        }
        /* have to look up command anyway */
        if ((cmd = (procedure)(long)hash_Lookup(self->Commands,temp)) == NULL) {
            copy = TRUE; /* unknown macros args read in copy mode */
        }

        for( getarg(self,t,temp,127,copy); (*temp != '\n') && (i < MAXARGS);     getarg(self,t,temp,127,copy)) {
            /* copy argument */
            Arg[i++] = StrDup(temp);
        }
        while(get(self,t) != '\n'); /* munch newline & extraneous args */
    }

    for(j=i;j<MAXARGS;j++)
        Arg[j]="";

    DEBUG((stderr,"Argc = %d, Name = (%s) ",i,Arg[0])); /* debugging */
    for (j=1;j<i;j++)
        DEBUG((stderr,"(%s) ",Arg[j]));
    DEBUG((stderr,"\n"));

    if (cmd != NULL)
        (*cmd)(self,t,br,i,Arg);

    DEBUG((stderr, "did cmd\n"));

    name[0] = Arg[0][0];
    name[1] = Arg[0][1];
    name[2] = '\0';

    for (j=0;j<i;j++) {
        free(Arg[j]);
    }
    DEBUG((stderr, "freed\n"));
}

/* scan the input for commands, do commands, crush text  until EOF or cmd */

Scan(self,t,cmd)
struct rofftext *self;
Trickle t;
char *cmd;
{
    register int c;
    char temp[3],name[3],*ptr;

    DEBUG((stderr, "Scan, cmd=%s\n", cmd));

    while ((c = get(self,t)) != EOF) {

        /* in copy mode, pass everything straight through */
        /* until you reach 'cmd' or .. */

        if (self->v_CopyMode) { 
            if (Is_BOL(self) && (c == '.' || c == '\'')) {
                getarg(self,t,temp,2,0);
                if (strcmp(temp,cmd)==0) {
                    tpush(self,t,NULL,NULL,temp,FALSE,NULL,NULL);
                    DoCommand(self,t,name,(c == '.'));
                    return;
                }
                else {
                    ptr = temp;
                    put(self,c);
                    while(*ptr != '\0')
                        put(self,*ptr++);
                }
            }
            else
                put(self,c);
        }

        /* not in copy mode -- look for commands, pass non-command text through */
        /* (do space-crushing here?) */

        else {
            switch (c) {
                case '.':
                case '\'':
                    if (Is_BOL(self)) {
                        DoCommand(self,t,name,(c == '.'));
                        if ((cmd != NULL) && (strcmp(cmd,name) == 0))
                            return;
                    }
                    else
                        put(self,c);
                    break;
                case '\n':
                    if (!self->v_BlankLine)
                        self->v_InputLineCount++;

                    /* handle input trap from .it */

                    if (self->CurrentEnviron->NextInputTrap
                         && self->CurrentEnviron->InputTrapCmd
                         && (self->v_InputLineCount >= self->CurrentEnviron->NextInputTrap)) {
                        procedure cmd2 = (procedure)(long)hash_Lookup(self->Commands, self->CurrentEnviron->InputTrapCmd);
                        self->Nullarg[0] = self->CurrentEnviron->InputTrapCmd;
                        if (cmd2)
                            (*cmd2)(self,t,0,1,self->Nullarg);
                        self->CurrentEnviron->NextInputTrap = 0;
                        self->Nullarg[0] = "";
                    }

                    self->v_BlankLine = TRUE;

                    /* handle no-fill mode here*/
                    if (self->v_FillMode) {

                        if (Is_BOL(self)) {
                            DoBreak(self);
                            if (!self->CurrentDiversion->NoSpaceMode) {
                                put(self,'\n');
                                self->CurrentDiversion->OutputDone = 0;
                                self->v_MultiSpace = FALSE;
                            }
                        }
                        else {
                            if (!self->v_MultiSpace)
                                put(self,' ');
                            self->v_MultiSpace = TRUE;
                        }
                    }
                    else
                        put(self,c);
                    break;
                case ' ':
                case '\t':
                    /* handle no-fill mode here*/
                    if (self->v_FillMode) {
                        if (!self->v_MultiSpace)
                            put(self,c);
                        self->v_MultiSpace = TRUE;
                    }
                    else
                        put(self,c);
                    break;
                default:
                    self->v_BlankLine = FALSE;
                    self->v_MultiSpace = FALSE;
                    put(self,c);
                    break;
            }
        }
    }
}

long rofftext__Read(self,file,id)
struct rofftext *self;
FILE *file;
long id;
{
    procedure cmd;
    char **begin,**ptr;

    if (file == NULL) {
        if ((self->filename != NULL) && (strcmp(self->filename,"-")==0))
            file = stdin;
        else
            fprintf(stderr,"rofftext: could not open input file\n");
    }

    self->Input = topen(self,NULL,file,NULL);



    /* push input files in reverse order, so they will be read in sequence */

    if (self->inputfiles && *self->inputfiles) {
        for(begin = ptr = self->inputfiles;(*ptr != NULL);ptr++);
        for(ptr--;(ptr >= begin);ptr--) {
            if (strcmp(*ptr,"-")==0)
                tpush(self,self->Input,NULL,stdin,NULL,FALSE,NULL,NULL);
            else
                tpush(self,self->Input,*ptr,NULL,NULL,FALSE,NULL,NULL);
        }
    }

    if (self->macrofile) {
        tpush(self,self->Input,self->macrofile,NULL,NULL,FALSE,NULL,NULL);
    }

    /* scan until end of file */
    Scan(self,self->Input,NULL);

    if (glist_Size(self->EndMacros)>0) {
        struct trap *t;
        while(t = (struct trap *)glist_Pop(self->EndMacros)) {
            cmd = (procedure)(long)hash_Lookup(self->Commands,t->macro);
            self->Nullarg[0] = t->macro;
            if (cmd)
                (*cmd)(self,self->Input,0,1,self->Nullarg);
        }
    }
    Set_BOL(self);
    /* scan some more to process end macros */

    Scan(self,self->Input,NULL);

    put(self,'\n');
    CloseAllStyles(self);
    return dataobject_NOREADERROR;
}

char *rofftext__ViewName(self)
struct rofftext *self;
{
    return "textview";
}


boolean rofftext__InitializeClass(classID)
struct classheader *classID;
{
#ifdef DEBUGGING
    if ((char *)getenv("ROFFDEBUG") != (char *) NULL)
	ROFFDEBUG = 1;
#endif /* DEBUGGING */
    return TRUE;
}

void rofftext__FinalizeObject(classID,self)
struct classheader *classID;
struct rofftext *self;
{}




void rofftext__SetAttributes(self,atts)
struct rofftext	*self;
struct attributes *atts;
{
    super_SetAttributes(self,atts);


    while(atts!=NULL){
        DEBUG((stderr,"Found attribute (%s)\n",atts->key));
        if (strncmp(atts->key,"filename",8) == 0) {
            self->filename = StrDup(atts->value.string);
        }
        else if(strncmp(atts->key,"rofftext-",9)==0){
            char    *key=atts->key+9;

            if (strcmp(key,"macrofile")==0) {
                self->macrofile = StrDup(atts->value.string);
            }
            else if (strcmp(key,"help-mode")==0) {
                self->HelpMode = atoi(atts->value.string);
            }
            else if (strcmp(key,"print-warnings")==0) {
                self->PrintWarnings = atoi(atts->value.string);
            }
            else if (strcmp(key,"be-bogus") == 0) {
                self->BeCompletelyBogus = atoi(atts->value.string);
            }
        }
        atts=atts->next;
    }
}


/***********************************************************************************/

/* nicked from text.c */

#define DATASTREAMVERSIONNUMBER 12

long rofftext__Write(self, file, writeID, level)
    struct rofftext *self;
    FILE *file;
    long writeID;
    int level;
{

    if (self->text->header.dataobject.writeID != writeID)  {
	self->text->header.dataobject.writeID = writeID;
        fprintf(file, "\\begindata{%s, %ld}\n", "text",
                 dataobject_UniqueID(&self->text->header.dataobject));
        fprintf(file, "\\textdsversion{%d}\n", DATASTREAMVERSIONNUMBER);

	if (self->text->styleSheet->templateName)
	    fprintf(file, "\\template{%s}\n", self->text->styleSheet->templateName);
	stylesheet_Write(self->text->styleSheet, file);
	rofftext_WriteSubString(self, 0, self->header.simpletext.length, file, TRUE);
        fprintf(file, "\\enddata{%s,%d}\n", "text", self->text->header.dataobject.id);

	fflush(file);
    }
    return self->header.dataobject.id;
}



long rofftext__ReadRoffIntoText(classID,t,fp,pos,resources)
struct classheader *classID;
struct text *t;
FILE *fp;
long pos;
char **resources;
{
    long cc;
    struct rofftext *r = rofftext_New();

    rofftext_SetInputFiles(r,resources);
    rofftext_SetInitialPos(r,pos);
    rofftext_SetText(r,t);
    cc = rofftext_Read(r,fp,0l);
    rofftext_Destroy(r);
    return cc;
}

void rofftext__SetText(self,t)
struct rofftext *self;
struct text *t;
{
    text_ReadTemplate(t,"roff",FALSE);
    self->stack->env = environment_GetEnclosing(t->rootEnvironment, 0L);
    self->stack->pos = environment_Eval(self->stack->env);
    self->text = t;
}

