/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffcmds.c,v 2.6 89/02/17 17:05:17 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffcmds.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffcmds.c,v 2.6 89/02/17 17:05:17 ghoti Exp $ ";
#endif /* lint */

/* rofftext
 *
 * Commands
 */

#include <class.h>
#include <hash.ih>
#include <glist.ih>
#include <andrewos.h> /* strings.h */
#include <ctype.h>
#include <roffstyl.h>
#include <rofftext.ih>
#include <rofftext.h>

/* define or re-define string
  * must parse its own arguments
 */

ds_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    register int c;
    static BUF b = NULL;
    char name[3];
    boolean tmp = self->v_CopyMode;

    if (b == NULL)
        b = NewBuf();

    getarg(self,t,name,2,0);
    if (*name == '\n')
        return;

    ClearBuf(b);

    self->v_CopyMode = TRUE;
    /* read whitespace */
    while((c = get(self,t)) == ' ' || c == '\t');

    if (c == '\"') /* strip leading quote */
        c = get(self,t);

    /* read string */
    while((c != '\n') && (c != EOF)) {
        Add2Buf(b,c);
        c = get(self,t);
    }
    Add2Buf(b,'\0');
    self->v_CopyMode = tmp;

    DEBUG((stderr,"Defining string (%s) as (%s)\n",name,Buf2Str(b)));
    putstring(self,name,Buf2Str(b));
}

/* rename or remove request, string, macro */
rm_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    char *value,*str = "removing",*v;
    boolean rename = FALSE;

    if (argv[0][1] == 'n') { /* this is rn, not rm */
        rename = TRUE;
        str = "renaming";
    }

    value = hash_Lookup(self->Macros,argv[1]);
    if (value) {
        DEBUG((stderr,"%s string (%s)",str,argv[1]));
        if (rename) {
            v=hash_Rename(self->Macros,argv[1],argv[2]);
            v += (int)hash_Rename(self->Commands,argv[1],argv[2]);
        }
        else {
            v = hash_Delete(self->Macros,argv[1]);
            v += (int)hash_Delete(self->Commands,argv[1]);
            free(value);
        }
        DEBUG((stderr,"%s\n",(v?"succeeded":"FAILED ***")));
        return;
    }
    value = hash_Lookup(self->Commands,argv[1]);
    if (value) {
        DEBUG((stderr,"%s string (%s)",str,argv[1]));
        if (rename)
            v=hash_Rename(self->Commands,argv[1],argv[2]);
        else {
            v=hash_Delete(self->Commands,argv[1]);
            free(value);
        }
        DEBUG((stderr,"%s\n",(v?"succeeded":"FAILED ***")));
        return;
    }
    DEBUG((stderr,"RENAME/DELETE: (%s) NOT FOUND\n",argv[1]));
}


/* define register */
nr_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int value,inc;
    boolean relative;

    EvalString(self,argv[2],&value,scale_u,NULL,&relative);
    if (argc >= 4) {
        EvalString(self,argv[3],&inc,scale_u,NULL,NULL);
    }
    else
        inc = 0;

    DEBUG((stderr,"Defining register (%s) as (%.0f)\n",argv[1],value));
    putregister(self,argv[1],value,0,inc,relative);
}

/* assign format */
af_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    struct reg *r = (Reg)hash_Lookup(self->Registers,argv[1]);
    enum RegFmt fmt;

    switch(*argv[2]) {
        case '0':
            fmt = reg_Triple;
            break;
        case 'i':
            fmt = reg_LCRoman;
            break;
        case 'I':
            fmt = reg_UCRoman;
            break;
        case 'a':
            fmt = reg_LCaz;
            break;
        case 'A':
            fmt = reg_UCaz;
            break;
        default:
            fmt = reg_Single;
    }
    if (r==NULL) {
        r = NewObj(struct reg);
        r->value = 0;
        r->format = fmt;
        r->autoinc = 0;
        hash_Store(self->Registers,argv[1],r);
    }
    else {
        r->format = fmt;
    }

}

/* remove register */
rr_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    
}


/* read from a file */
so_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    tpush(self,t,argv[1],NULL,NULL,FALSE,NULL,NULL);
}



/* line break */
br_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    if (br)
        DoBreak(self);
}

/* exit be2roff ??? */
ex_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
}


/* Do a macro */
DoMacro(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{

    char *macro = hash_Lookup(self->Macros,argv[0]);
    DEBUG((stderr,"v-----Calling Macro named (%s)-------v\n",argv[0]));
    tpush(self,t,NULL,NULL,macro,TRUE,argc,argv);
}



/* define or re-define macro */
de_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    char *macro,*existing,*name = StrDup(argv[1]);
    char *end = StrDup(argv[2]);
    static BUF b = NULL;

    if (b == NULL)
        b = NewBuf();

    ClearBuf(b);

    self->v_CopyMode = TRUE;
    self->v_SnarfOutput = b;

    Scan(self,t,((argc<3)?".":end));

    self->v_SnarfOutput = NULL;
    self->v_CopyMode = FALSE;
    Add2Buf(b,'\0');

    macro = StrDup(Buf2Str(b));

    hash_Delete(self->Commands,name);
    existing = hash_Delete(self->Macros,name);
    if (existing)
        free(existing);

    DEBUG((stderr,"--Defining Macro (%s)--\n",name));

    hash_Store(self->Macros,name,macro);
    hash_Store(self->Commands,name,DoMacro);
    free(name);
    free(end);
/*    DEBUG((stderr,"%s\n<----------------\n",macro));*/
}


/* divert output into a macro */

di_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    char *macro;
    static BUF b = NULL;
    char *existing;

    if (b == NULL)
        b = NewBuf();

    if (argc > 1) { 

        ClearBuf(b);

        DEBUG((stderr,"--Diverting to Macro (%s)------>\n",argv[1]));

/*        tmp = self->v_CopyMode;
        self->v_CopyMode = TRUE;*/
        self->v_SnarfOutput = b;
        PushDiversion(self);
        self->CurrentDiversion->name = StrDup(argv[1]);
    }
    else{
        if (self->v_DiversionLevel > 0) {
            self->v_SnarfOutput = NULL;
            /*        self->v_CopyMode = tmp;*/
            Add2Buf(b,'\0');

            macro = StrDup(Buf2Str(b));

            existing = (char *)hash_Delete(self->Macros,self->CurrentDiversion->name);
            if (existing)
                free(existing);
            hash_Delete(self->Commands,self->CurrentDiversion->name);

            hash_Store(self->Macros,self->CurrentDiversion->name,macro);
            hash_Store(self->Commands,self->CurrentDiversion->name,DoMacro);
            DEBUG((stderr,"\n<----------------divert (%s)\n",self->CurrentDiversion->name));
            PopDiversion(self);
        }
    }
}


/* debugging */
c0_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    self->v_CopyMode = 0;
}

c1_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    self->v_CopyMode = 1;
}


/* if-else  -- special syntax */

ie_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int c,delim,delimcount;
    char *str,*string2;
    boolean sense = FALSE,result;
    int dresult;
    static BUF b = NULL;

    if (b == NULL)
        b = NewBuf();

    /* read leading whitespace */
    do {
        c = get(self,t);
    } while((c==' ')||(c=='\t'));

    /* tell what to do by first char */

    if (c=='!') {
        sense = TRUE;
        do {
            c = get(self,t);
        } while((c==' ')||(c=='\t'));
    }
    if (isdigit(c)) { /* numeric compare */
        ClearBuf(b);
        while((c!= ' ') && (c != '\t') && (c != '\n')) {
            Add2Buf(b,c);
            c = get(self,t);
        }
        Add2Buf(b,'\0');
        str = Buf2Str(b);
        DEBUG((stderr,"Comparing numeric value (%s)\n",str));
        EvalString(self,str,&dresult,scale_u,NULL,NULL);
        result = (dresult > 0);
    }
    else {
        switch(c) {
            case 'o': result = TRUE;
                break;
            case 'e': result = FALSE;
                break;
            case 't': result = self->RoffType;
                break;
            case 'n': result = !self->RoffType;
                break;
            default: /* string compare */
                /* read first argument into string */ /* this is inadequate */
                ClearBuf(b);
                delimcount = 0;
                delim = c;
                c = get(self,t);
                if (c == delim)
                    delimcount++;

                while((c != '\n') && (delimcount < 2)) {
                    Add2Buf(b,c);
                    c = get(self,t);
                    if (c == delim)
                        delimcount++;
                }
                Add2Buf(b,'\0');
                str = Buf2Str(b);
                DEBUG((stderr,"====>if: delim '%c', string argument is (%s)\n",delim,str));


                string2 = index(str,delim);
                if (string2 == NULL)
                    string2 = "";
                else {
                    *string2++ = '\0';
                }
                DEBUG((stderr,"Comparing (%s) (%s)\n",str,string2));
                result = (strcmp(str,string2)==0);

                break;
        }
        c = get(self,t); /* get first char afterwards because number does */

    }
    ClearBuf(b);
    DEBUG((stderr,"if: sense %d, result %d\n",sense,result));
    /* if condition is false, munch to end of conditional input */
    if (result == sense) {
        self->v_RawMode = TRUE;
        DEBUG((stderr,"-->Munch->"));
        while(c != EOF && ((c != '\n') || (self->v_InCond > 0))) {
            c = get(self,t);
        }
        DEBUG((stderr,"<-Munch<--"));
        self->v_RawMode = FALSE;
    }
    else {
        /* read spaces */
        while((c ==' ') || (c == '\t'))
            c = get(self,t);
        ung(self,c,t);
        Set_BOL(self); /* next char is BOL */
    }

    self->v_LastIfResult = (sense != result);
}


/* if command -- special syntax */
if_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    boolean tmp = self->v_LastIfResult;
    ie_cmd(self,t,br,argc,argv);
    self->v_LastIfResult = tmp;
}

/* else command -- special syntax */
el_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int c;

    DEBUG((stderr,"else: previous result was %d\n",self->v_LastIfResult));
    if (self->v_LastIfResult) { /* munch to end of conditional input */
        self->v_RawMode = TRUE;
        DEBUG((stderr,"-->Munch->"));
        do {
            c = get(self,t);
        } while(c != EOF && ((c != '\n') || (self->v_InCond > 0)));
        DEBUG((stderr,"<-Munch<--"));
        self->v_RawMode = FALSE;
    }
    else {
        do {
            c = get(self,t);
        } while((c ==' ')||(c == '\t')); /* munch whitespace */
        ung(self,c,t);
        Set_BOL(self);
    }
    self->v_LastIfResult = TRUE;
}



/* print macro -- for debugging */
PM_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    char *m;
    printf("Macro named (%s) ----->\n",argv[1]);
    m = hash_Lookup(self->Macros,argv[1]);
    if (m)
        printf("%s",m);
    else
        printf("(Could not find %s)\n",argv[1]);
    printf("\n--------------End\n");
}


PA_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    printf("::::::::::::::::Printing all macros:::::::::::::::::\n");
    hash_Debug((self->Macros));
    printf("::::::::::::::::DONE::::::::::::::::\n");
}


/* title command -- special syntax */

tl_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    register int c,style;
    char *str,*string1=NULL,*string2=NULL,*string3=NULL,*end=NULL;
    int tmp = self->CurrentDiversion->OutputDone;
    static BUF b = NULL;

    if (b == NULL)
        b = NewBuf();
    ClearBuf(b);
    c = get(self,t);
    /* read leading whitespace */
    while((c==' ')||(c=='\t'))
        c = get(self,t);
    /* read stuff into string */
    while((c != EOF) && (c != '\n')) {
        Add2Buf(b,c);
        c = get(self,t);
    }
    Add2Buf(b,'\0');
    str = Buf2Str(b);

    style = BeginStyle(self,"majorheading");
    /* now get three parts */
    string1 = str+1;
    string2 = index(string1,*str);
    if (string2) {
        *string2++ = '\0';
        string3 = index(string2,*str);
        if (string3) {
            *string3++ = '\0';
            end = index(string3,*str);
            if (end)
                *end = '\0';
        }
    }
    if (string2) {
        while(*string1 != '\0') {
            if ((c = *string1++) != '%')
                put(self,c);
        }
        put(self,' '); put(self,' ');
        put(self,' '); put(self,' ');
    }
    if (string3) {
        while(*string2 != '\0') {
            if ((c = *string2++) != '%')
                put(self,c);
        }
        put(self,' '); put(self,' ');
        put(self,' '); put(self,' ');
    }
    if (end) {
        while(*string3 != '\0') {
            if ((c = *string3++) != '%')
                put(self,c);
        }
        put(self,'\n');
    }
    EndStyle(self,style);
    self->CurrentDiversion->OutputDone = tmp; /* don't screw up breaks */

}


SortTraps(trap1,trap2)
struct trap *trap1,*trap2;
{
    if (trap1->loc > trap2->loc)
        return 1;
    else if (trap1->loc < trap2->loc)
        return -1;
    else return 0;
}

/* set a trap -- beginning or end of page */
wh_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int result;
    boolean absolute,relative;

    /* check location for trap */
    EvalString(self,argv[1],&result,scale_u,&absolute,&relative);
    if (!relative && !absolute && (result == 0)) {
        DEBUG((stderr,"Setting beginning trap\n"));
        self->v_Trap = StrDup(argv[2]);
    }
    /* foo */
    if (relative && (result < 0)) {
        struct trap *trap = (struct trap *)malloc(sizeof(struct trap));
        trap->loc = result;
        trap->macro = StrDup(argv[2]);
        glist_InsertSorted(self->EndMacros,trap,SortTraps);
    }

}

/* space, the final frontier (ugh) */
sp_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    if (self->CurrentDiversion->NoSpaceMode) {
        DEBUG((stderr,"Space: no space mode is on...\n"));
        return;
    }
    else
        sv_cmd(self,t,br,argc,argv);
}

sv_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int result;
    int lines = 1;

    /* do a break */
    if (br)
        DoBreak(self);

    if (argc > 1) {
        EvalString(self,argv[1],&result,scale_v,NULL,NULL);
        if (result > 0) {
            lines = (int)((result / (self->ScaleFactor[(int)scale_v])) + (0.5));
        }
        else
            lines = 0;
    }
    DEBUG((stderr,"SPACE: spacing %d from a result of %g\n",lines,result));
    if (lines > 0) {
        while(lines-- > 0)
            put(self,'\n');
        self->CurrentDiversion->OutputDone = 0;
    }
}


it_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int result;

    if (argc<3) {
        if (self->CurrentEnviron->InputTrapCmd)
            free(self->CurrentEnviron->InputTrapCmd);
        self->CurrentEnviron->InputTrapCmd = NULL;
        self->CurrentEnviron->NextInputTrap = 0;
    }
    else {
        self->CurrentEnviron->InputTrapCmd = StrDup(argv[2]);
        EvalString(self,argv[1], &result, scale_u,NULL,NULL);
        self->CurrentEnviron->NextInputTrap = result + self->v_InputLineCount;
    }
}

/* set global font */

ft_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    char *font;
    if (argc<2)return;
    if (self->v_DiversionLevel>0) { /* yuck-o, handle fonts in diversions */
        int tmp = self->CurrentDiversion->OutputDone;
        put(self,'\\');
        put(self,'f');
        put(self,*argv[1]);
        self->CurrentDiversion->OutputDone = tmp;
        return;
    }
    font = argv[1];
    if (isdigit(font[0])) {
        int digit = font[0]-'1';
        if ((digit >= 0) && (digit <= 3))
            font = self->Fonts[digit];
        else {
            DEBUG((stderr,"********WARNING: bogus font (%s)********\n)",font));
            font = self->Fonts[0];
        }
    }
    else
        switch(font[0]) {
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

/* set global indent */

in_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    /* this is bogus for now */
    int result;
    boolean relative;
    int temp;

    if (br)
        DoBreak(self);

    temp = self->CurrentEnviron->indent;

    if (argc>1) {
        DEBUG((stderr,"Indenting (%s)\n",argv[1]));

        EvalString(self,argv[1],&result,scale_m,NULL,&relative);
        if (relative)
            self->CurrentEnviron->indent += result;
        else
            self->CurrentEnviron->indent = result;
    }
    else {
        self->CurrentEnviron->indent = self->CurrentEnviron->prevIndent;
    }
    self->CurrentEnviron->prevIndent = temp;
    self->CurrentEnviron->tempIndent = self->CurrentEnviron->indent;

    if (self->CurrentEnviron->indent != temp)
        SetIndent(self,self->CurrentEnviron->indent);
}

/* temporary indent */
ti_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    /* this is bogus for now */
    int result;
    boolean absolute,relative;

    if (br)
        DoBreak(self);

    if (argc>1) {
        DEBUG((stderr,"Temp. indenting (%s)\n",argv[1]));

        EvalString(self,argv[1],&result,scale_m,&absolute,&relative);
        if (relative)
            self->CurrentEnviron->tempIndent = self->CurrentEnviron->indent + result;
        else
            self->CurrentEnviron->tempIndent = result;
        if (self->CurrentEnviron->tempIndent < 0)
            self->CurrentEnviron->tempIndent = 0;
        if (self->CurrentEnviron->tempIndent != self->CurrentEnviron->indent)
            SetTempIndent(self,self->CurrentEnviron->tempIndent);
    }

}


/* no-fill mode */
nf_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    if (br)
        DoBreak(self);
    self->v_FillMode = FALSE;
}

/* fill mode */
fi_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    if (br)
        DoBreak(self);
    self->v_FillMode = TRUE;
}


/* set no-space mode */
ns_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    DEBUG((stderr,"===Turning on no-space mode===\n"));
    self->CurrentDiversion->NoSpaceMode = TRUE;
}


/* restore spacing */
rs_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    DEBUG((stderr,"=Resetting Space Mode=\n"));
    self->CurrentDiversion->NoSpaceMode = FALSE;
}


/* center text */
ce_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    if (br)
        DoBreak(self);
}


/* ignore input */
ig_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    char *end = StrDup(argv[1]);
    static BUF b = NULL;

    if (b == NULL)
        b = NewBuf();

    ClearBuf(b);

    self->v_CopyMode = TRUE;
    self->v_SnarfOutput = b;

    Scan(self,t,((argc<2)?".":end));

    self->v_SnarfOutput = NULL;
    self->v_CopyMode = FALSE;
    /* ignore completely! */
    free(end);
}


/* translate characters  - special syntax */
tr_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    register int c;
    unsigned char source,new;
    char temp[3],name[3];

    /* read leading spaces */

    self->v_RawMode = TRUE;
    do {
        c = get(self,t);
    } while ((c==' ')||(c == '\t'));

    /* now translate, checking for special chars */

    while( (c != '\n') && (c != EOF)) {
        if (c == '\\') { /* check for special */
            c = get(self,t);
            if (c == '(') {
                name[0] = get(self,t);
                name[1] = get(self,t);
                name[2] = '\0';
                DEBUG((stderr,"tr: found special char %s\n",name));
                /* change special char */
            }
            else {
                temp[0] = '\\';
                temp[1] = c;
                temp[2] = '\0';
                tpush(self,t,NULL,NULL,temp,FALSE,NULL,NULL);
                self->v_RawMode = FALSE;
                c = get(self,t);
                name[0] = '\0';
                DEBUG((stderr,"tr: found special char %s, which maps to %c\n",temp,c));
            }
        }
        else
            name[0] = '\0';
        source = c;
        self->v_RawMode = FALSE;

        if (c == '\n') /* in case a translated character was null */
            break;

        /* should really handle special chars here, too */
        new = c = get(self,t);
        if (new == '\n')
            new = ' ';
        if (name[0] == '\0') {
            DEBUG((stderr,"tr: translating %c to %c\n",source,new));
            self->Berliz[source] = new;
        }
        else {
            temp[0] = new;
            temp[1] = '\0';
            /* we won't worry about keeping track of mallocs here */
            hash_Store(self->SpecialChars,name,StrDup(temp));
            DEBUG((stderr,"tr: renaming %s to %s\n",name,temp));
        }
        self->v_RawMode = TRUE;
        c = get(self,t);
    }
    self->v_RawMode = FALSE;
}

/* switch environment */
ev_cmd(self,t,br,argc,argv)
struct rofftext *self;
Trickle t;
boolean br;
int argc;
char *argv[];
{
    int result;
    int i;

    if (argc > 1) {
        EvalString(self,argv[1],&result,scale_u,NULL,NULL);
        i = result;
        if ((i >= 0) && (i <= 2))
            PushEnviron(self,i);
    }
    else
        PopEnviron(self);
}
