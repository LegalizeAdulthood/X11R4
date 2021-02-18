/*LIBS: -lutil
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/mkcon.c,v 2.15 89/11/06 17:24:27 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/mkcon.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/mkcon.c,v 2.15 89/11/06 17:24:27 tpn Exp $ ";
#endif /* lint */

#include <class.h>
#include <mapping.h>
#include <stdio.h>
#include <andrewos.h> /* sys/file.h */
#include <sys/param.h>
extern char *AndrewDir();

#define AWKPROG AndrewDir("/lib/arbiters/conpros.awk") 
#define MAKEFILE  AndrewDir("/lib/arbiters/makefile.arb") 
#define START "* user code begins here for "
#define END "* user code ends here for "
#define STARTCHAR '/'
#define TRUE 1
#define FALSE 0
char *oldb,*newb;
struct stf {
    char *begin,*end,*key;
    struct stf *next,*bro;
};
static char Imakebase[] =  "\
DOBJS = %s.do\n\
IHFILES = %s.ih\n\
NormalObjectRule()\n\
NormalATKRule()\n\
InstallClassFiles($(DOBJS),$(IHFILES))\n\
\n\
DependTarget()\n\
";
static char shellbase[] = "\
if test -r Makefile \n\
then \n\
echo Using existing Makefile\n\
else \n\
genmake\n\
echo making dependencies\n\
make depend\n\
fi\n\
make\n\
";

void ws(s,f)
struct stf *s;
FILE *f;
{
    char *c;
    for(c = s->begin; c < s->end; c++)
	putc(*c,f);
}
justcomments(s)
struct stf *s;
{
    char *c;
    int incomment = FALSE;
    for(c = s->begin; c < s->end; c++){
	if(*c == '/' && *(c+1) == '*') incomment = TRUE;
	if(incomment == FALSE){
	    return FALSE;
	}
	if(*c == '\n' && c - 1 > s->begin && *(c - 1) == '/' && *(c - 2) == '*')
	    incomment = FALSE;
    }
    return TRUE;
}
writemerge(o,n,f)
struct stf *o,*n;
FILE *f;
{
    for(;n != NULL; n = n->next){
	if(n->bro) {
	    ws(n->bro,f);
	}
	else  {
	    ws(n,f);
	}
    }
    for(n = o; n != NULL; n = n->next){
	if(n->key && !n->bro && !justcomments(n)) break;
    }
    if(n != NULL){
	fprintf(f,"\n#ifdef UNUSED_USER_CODE\n");
	for(;o != NULL; o = o->next){
	    if(o->key && !o->bro && !justcomments(o)) ws(o,f);
	}
	fprintf(f,"\n#endif /* UNUSED_USER_CODE */\n");
    }
}
struct stf *newstf(old)
struct stf *old;
{
    struct stf *new;
    new =  (struct stf *) (malloc(sizeof (struct stf)));
    if(old) old->next = new;
    new->begin = new->end = new->key = NULL;
    new->bro = new->next = NULL;
    return new;
}
struct stf *makestf(buf)
char *buf;
{
    struct stf *start, *new;
    char *c;
    int sl = strlen(START);
    int el = strlen(END);
    new = start = newstf(NULL);
    new->begin = buf;
    for(c = buf;*c != '\0';c++){
	if(*c == STARTCHAR ){
	    if(strncmp(c+1,START,sl) == 0){
		if(new->begin != c){
		    new->end = c;
		    new= newstf(new);
		    new->begin = c;
		}
		new->key = c + sl;
	    }
	    else if(strncmp(c+1,END,el) == 0){
		while(*c != '\n') c++;
		new->end = c + 1;
		new= newstf(new);
		new->begin = c + 1;
	    }
	}
    }
    new->end = c;
    return start;
}				
char *getf(f,s,buf)
FILE *f;
char *s,*buf;
{
    static char ending[1024];
    register char *c,*cp;
    extern char *index();
    struct MapEntryStruct *mp;
    for(c = buf;fgets(c,2048,f) != NULL; ){
	if(*c == *s && strncmp(c,s,strlen(s)) == 0){
	    *c = '\0';
	    return buf; 
	}
	/* The following code translates the object names into the
	 file names for the include files */
	if(*c == '#' && (strncmp(c,"#include <",10)) == 0){
	    c += 10;
	    cp = index(c,'.');
	    if(cp != NULL){
		*cp = '\0';
		if((mp = RetrieveByName(c,class_VERSIONNOTKNOWN)) == NULL || mp->Key == NULL || mp->Name == mp->Key || strcmp(mp->Name,mp->Key) == 0) *cp = '.';
		else {
		    strcpy(ending,cp +1);
		    sprintf(c,"%s.%s",mp->Key,ending);
		}
	    }
	}	
	while(*c != '\0') c++;
    }
}
keycmp(s1,s2)
register char *s1,*s2;
{
    while(*s1++ == *s2){
	if(*s2++ == '\n') return TRUE;
    }
    return FALSE;
}
setbro(s1,s2)
struct stf *s1,*s2;
{
    register struct stf *ss2,*ss1;
    for(ss1 = s1 ; ss1 != NULL; ss1 = ss1->next){
	for(ss2 = s2; ss2 != NULL ; ss2 = ss2->next){
	    if(ss1->key != NULL && ss2->key != NULL &&
	       *(ss1->key) == *(ss2->key) && (keycmp(ss1->key , ss2->key) == TRUE)){
		ss1->bro = ss2;
		ss2->bro = ss1;
		break;
	    }
	}
    }
}

static FILE *tryopen(name,renamed)
char *name;
boolean *renamed;
{
    char buf[1024],*p;
    FILE *f;
    *renamed = FALSE;
    if((f =fopen(name,"r")) != NULL) return f;
    strcpy(buf,name);
    p = rindex(buf,'.');
    if(p != NULL && strcmp(p,".ch") == 0){
	strcpy(p,".H");
	if((f =fopen(buf,"r")) != NULL){
	    *renamed = TRUE;
	    return f;
	}
    }
    return NULL;
}
	    
process(oldf,name)
FILE *oldf;
char *name;
{
    static char oldb[64000], newb[64000];
    FILE *newf;
    boolean renamed;
    struct stf *olds,*news;
    getf(oldf,"! THIS IS THE END OF THIS FILE !!!\n",oldb);
    if((newf = tryopen(name,&renamed)) == NULL ){
	if((newf = fopen(name,"w")) != NULL){
	    printf("Creating %s\n",name);
	    fwrite(oldb,strlen(oldb),1,newf);
	    fclose(newf);
	    return TRUE;
	}
	fprintf(stderr,"Can't Write %s\n",name);
	return FALSE;
    }
    if(renamed){
	printf("Merging old .H file and new info to create %s\n",name);
    }
    else {
	sprintf(newb,"%s.old",name);
	rename(name,newb);	
	printf("Moving %s to %s.old\n",name,name);
    }
    getf(newf,"",newb);
    fclose(newf);
    if((newf = fopen(name,"w")) == NULL){
	fprintf(stderr,"Can't Write %s\n",name);
	return FALSE;
    }
    olds = makestf(oldb);
    news = makestf(newb);
    setbro(olds,news);
    if(!renamed) printf("Merging in new info to create %s\n",name);
    writemerge(news,olds,newf);
    fclose(newf);
    return TRUE;
}


usage(s)
char *s;
{
    fprintf(stderr,"usage: %s <-C ClassName> <-F FunctionName> <-T Title> <-S ShellScriptName> <filename>\n",s);
    exit(1);
}

static char makefile[MAXPATHLEN];
	
main(argc,argv)
char *argv[];
{
    register char *c;
    register int cc;
    char bb[256],cmd[5200],tmpfile[256];
    char *awkprog,*src,*program;
    char *cls,*func,*title,*sname;
    int success = TRUE;
    FILE *f;
    strcpy(makefile,MAKEFILE);
    class_Init(AndrewDir("/dlib/atk"));
    awkprog = AWKPROG;
    src = cls = title = sname = NULL;
    func = "go";
    *tmpfile = '\0';
    program = *argv;
    while(*++argv != NULL){
	if(**argv=='-'){
	    switch((*argv)[1]){
		case 'c':   /* Classname for controller class */
		case 'C':
		    cls = (argv[0][2] == '\0')? *++argv : (*argv) + 2;
		    break;
		case 'f':   /*  function in controllers class to call */
		case 'F':   /* (default is 'go' ) */
		    func =(argv[0][2] == '\0')? *++argv : (*argv) + 2;
		    break;
		case 't':   /* Title for top of screen */
		case 'T': 
		    title =(argv[0][2] == '\0')? *++argv : (*argv) + 2;
		    break;
		case 'a':   /* Name of alternate Awk program to run   */
		case 'A':   /* (debugging purposes only) */
			   
		    awkprog =(argv[0][2] == '\0')? *++argv : (*argv) + 2;
		    break;
		case 'o':    /* name of shellscript to create for running application */
		case 'O':  
		    sname = (argv[0][2] == '\0')? *++argv : (*argv) + 2;
		    break;
		default:
		    break;
	    }
	    if(argv == NULL) usage(program);
	}
	else src = *argv;
    }
    
    if(src){
	if(access(src,R_OK) == -1){
	    fprintf(stderr,"ERROR: Can't Open %s\n",src);
	    exit(1);
	}
	printf("Creating a controller for %s. Please wait...\n",src);
    }
    else {
	printf("Attempting to read file from standard input\n");
    }
    if(cls == NULL){
	if (src)
	    sprintf(cmd,"%s %s",awkprog,src);
	else 
	    sprintf(cmd,"%s",awkprog);
    }	
    else {
	FILE *ff;
	int i;
	for(i = 0; i < 100; i++){
	    sprintf(tmpfile,"/tmp/adew%d.tmp",i);
	    if(access(tmpfile,F_OK) == -1 && ((ff = fopen(tmpfile,"w")) != NULL))
		break;
	}
	if(ff == NULL) {
	    fprintf(stderr,"ERROR: Can't open file in /tmp");
	    exit(1);
	}
	fprintf(ff,"CLASSNAME= %s FUNCTIONAME= %s\n",cls,func);
	if(src == NULL) {
	    while((cc = getc(stdin)) != EOF){
		if(putc(cc,ff) == EOF){
		    fprintf(stderr,"ERROR: Error in writing %s",tmpfile);
		    exit(1);
		}
	    }
	    fclose(ff);
	    sprintf(cmd,"%s %s",awkprog,tmpfile);
	}
	else {
	    fclose(ff);
	    sprintf(cmd,"cat %s %s | %s",tmpfile,src,awkprog);
	}
    }
    *bb = '\0';
    printf("running %s\n",cmd);
    fflush(stdout);
    if((f = popen(cmd,"r")) != NULL){
	while(fgets(bb,256,f) != NULL){
	    if(*bb == '!'){
		fprintf(stderr,"%s",bb);
		pclose(f);
		if(*tmpfile) unlink(tmpfile);
		usage(program);
	    }
	    for(c = bb; *c != '\0'; c++) 
		if(*c == '\n') *c = '\0';
	    if(process(f,bb) == FALSE){
		success = FALSE;
		break;
	    }
	}
    }
    else success = FALSE;
    pclose(f);
    if(*tmpfile) unlink(tmpfile);
    if(success && strlen(bb) > 0){

	for(c = bb; *c != '\0'; c++){
	    if(*c == '.') {
		*c = '\0';
		break;
	    }
	}
	sprintf(cmd,"make%s",bb);
	if(access("Imakefile",R_OK) == -1 &&
	   ((f = fopen("Imakefile","w")) != NULL)){
	    printf("Creating Imakefile\n");
	    fprintf(f,Imakebase,bb,bb);
	    fclose(f);
	}
	sprintf(cmd,"make%s",bb);
	if(access(cmd,R_OK) == -1 &&
	   ((f = fopen(cmd,"w")) != NULL)){
	    printf("Creating %s shellscript\n",cmd);
	    fprintf(f,shellbase,bb);
	    fclose(f);
	    chmod(cmd,0755);
	}
	if(sname){
	    if( src != NULL && 
	       ((f = fopen(sname,"w")) != NULL)){
		printf("Creating %s shellscript\n",sname);
		fprintf(f,"runadew");
		if(cls){
		    fprintf(f," -C\"%s\" -F\"%s\"",cls,func);
		}
		if(title){
		    fprintf(f," -T\"%s\"",title);
		}
		fprintf(f," -S\"%s\" $*\n",src);
		fclose(f);
		chmod(sname,0755);
	    }
	    else {
		if(src == NULL) 
		    fprintf(stderr,"ERROR: Need source file name to create %s shellscript\n",sname);
		else fprintf(stderr,"ERROR: Can't open %s for output",sname);
	    }
	}
	printf("Done\n");
    } 
}

