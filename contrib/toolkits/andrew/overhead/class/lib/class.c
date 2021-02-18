/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	class.c - runtime support for class system
 */

char class_rcsid[] = "$Header: class.c,v 1.2 89/11/30 10:56:07 xguest Exp $";


#include <andrewos.h> /* sys/types.h sys/file.h */
#include <stdio.h>
#include <a.out.h>
#include <setjmp.h>
#include <sys/signal.h>
#include <sys/param.h>
#include <sys/dir.h>

#include <classind.h>
#include <mapping.h>

#include <class.h>  /* this contains all the structs, etc. for the class system */
#include <doload.h>
#include <errno.h>

/* external symbols that have no include files */


extern char *getenv();	    /* %%%% */
extern int getpid();
extern char etext;
extern int errno;



/*
 * additional defined constants
 */





#define INITIALLISTSIZE 30

#define NOINIT 0
#define NOLOAD 1
#define LOAD 2





/* 
 * local data structures
 */

struct pathentry {
    struct pathentry *next;
    char name[1];
};

struct classload  {
    char *className;
    char *classNameKey;
    char *baseaddr;	/* this isn't actually used, except by people in gdb */
    struct classinfo *info;
    struct classinfo *(*getinfo)();
};

class_DebugLevelType DebugLevel;



/* 
 * data
 */


long class_ErrorReturnValue=0; /* used by classentry.s */

enum class_Error_type class_Error = class_ErrNone;  /* initialize this to force it into data segment */

static struct sigvec handler;		/* pointer to signal handler, used to catch signals */



static struct classheader unknownID = {NULL, "unknown", NULL, NULL};

static struct classload *ClassList = NULL;
static int ClassListEntries = 0;
static int MaxClassListEntries = 0;
static struct pathentry *globalPath = NULL;





long ClassEntry0();	long ClassEntry1();	long ClassEntry2();	long ClassEntry3();	long ClassEntry4();
long ClassEntry5();	long ClassEntry6();	long ClassEntry7();	long ClassEntry8();	long ClassEntry9();
long ClassEntry10();	long ClassEntry11();	long ClassEntry12();	long ClassEntry13();	long ClassEntry14();
long ClassEntry15();	long ClassEntry16();	long ClassEntry17();	long ClassEntry18();	long ClassEntry19();
long ClassEntry20();	long ClassEntry21();	long ClassEntry22();	long ClassEntry23();	long ClassEntry24();
long ClassEntry25();	long ClassEntry26();	long ClassEntry27();	long ClassEntry28();	long ClassEntry29();
long ClassEntry30();	long ClassEntry31();	long ClassEntry32();	long ClassEntry33();	long ClassEntry34();
long ClassEntry35();	long ClassEntry36();	long ClassEntry37();	long ClassEntry38();	long ClassEntry39();
long ClassEntry40();	long ClassEntry41();	long ClassEntry42();	long ClassEntry43();	long ClassEntry44();
long ClassEntry45();	long ClassEntry46();	long ClassEntry47();	long ClassEntry48();	long ClassEntry49();
long ClassEntry50();	long ClassEntry51();	long ClassEntry52();	long ClassEntry53();	long ClassEntry54();
long ClassEntry55();	long ClassEntry56();	long ClassEntry57();	long ClassEntry58();	long ClassEntry59();
long ClassEntry60();	long ClassEntry61();	long ClassEntry62();	long ClassEntry63();	long ClassEntry64();
long ClassEntry65();	long ClassEntry66();	long ClassEntry67();	long ClassEntry68();	long ClassEntry69();
long ClassEntry70();	long ClassEntry71();	long ClassEntry72();	long ClassEntry73();	long ClassEntry74();
long ClassEntry75();	long ClassEntry76();	long ClassEntry77();	long ClassEntry78();	long ClassEntry79();
long ClassEntry80();	long ClassEntry81();	long ClassEntry82();	long ClassEntry83();	long ClassEntry84();
long ClassEntry85();	long ClassEntry86();	long ClassEntry87();	long ClassEntry88();	long ClassEntry89();
long ClassEntry90();	long ClassEntry91();	long ClassEntry92();	long ClassEntry93();	long ClassEntry94();
long ClassEntry95();	long ClassEntry96();	long ClassEntry97();	long ClassEntry98();	long ClassEntry99();
long ClassEntry100();	long ClassEntry101();	long ClassEntry102();	long ClassEntry103();	long ClassEntry104();
long ClassEntry105();	long ClassEntry106();	long ClassEntry107();	long ClassEntry108();	long ClassEntry109();
long ClassEntry110();	long ClassEntry111();	long ClassEntry112();	long ClassEntry113();	long ClassEntry114();
long ClassEntry115();	long ClassEntry116();	long ClassEntry117();	long ClassEntry118();	long ClassEntry119();
long ClassEntry120();	long ClassEntry121();	long ClassEntry122();	long ClassEntry123();	long ClassEntry124();
long ClassEntry125();	long ClassEntry126();	long ClassEntry127();	long ClassEntry128();	long ClassEntry129();
long ClassEntry130();	long ClassEntry131();	long ClassEntry132();	long ClassEntry133();	long ClassEntry134();
long ClassEntry135();	long ClassEntry136();	long ClassEntry137();	long ClassEntry138();	long ClassEntry139();
long ClassEntry140();	long ClassEntry141();	long ClassEntry142();	long ClassEntry143();	long ClassEntry144();
long ClassEntry145();	long ClassEntry146();	long ClassEntry147();	long ClassEntry148();	long ClassEntry149();

struct basicobject_methods class_RoutineStruct = {
NULL,
ClassEntry0,		ClassEntry1,		ClassEntry2,		ClassEntry3,		ClassEntry4,
ClassEntry5,		ClassEntry6,		ClassEntry7,		ClassEntry8,		ClassEntry9,
ClassEntry10,		ClassEntry11,		ClassEntry12,		ClassEntry13,		ClassEntry14,
ClassEntry15,		ClassEntry16,		ClassEntry17,		ClassEntry18,		ClassEntry19,
ClassEntry20,		ClassEntry21,		ClassEntry22,		ClassEntry23,		ClassEntry24,
ClassEntry25,		ClassEntry26,		ClassEntry27,		ClassEntry28,		ClassEntry29,
ClassEntry30,		ClassEntry31,		ClassEntry32,		ClassEntry33,		ClassEntry34,
ClassEntry35,		ClassEntry36,		ClassEntry37,		ClassEntry38,		ClassEntry39,
ClassEntry40,		ClassEntry41,		ClassEntry42,		ClassEntry43,		ClassEntry44,
ClassEntry45,		ClassEntry46,		ClassEntry47,		ClassEntry48,		ClassEntry49,
ClassEntry50,		ClassEntry51,		ClassEntry52,		ClassEntry53,		ClassEntry54,
ClassEntry55,		ClassEntry56,		ClassEntry57,		ClassEntry58,		ClassEntry59,
ClassEntry60,		ClassEntry61,		ClassEntry62,		ClassEntry63,		ClassEntry64,
ClassEntry65,		ClassEntry66,		ClassEntry67,		ClassEntry68,		ClassEntry69,
ClassEntry70,		ClassEntry71,		ClassEntry72,		ClassEntry73,		ClassEntry74,
ClassEntry75,		ClassEntry76,		ClassEntry77,		ClassEntry78,		ClassEntry79,
ClassEntry80,		ClassEntry81,		ClassEntry82,		ClassEntry83,		ClassEntry84,
ClassEntry85,		ClassEntry86,		ClassEntry87,		ClassEntry88,		ClassEntry89,
ClassEntry90,		ClassEntry91,		ClassEntry92,		ClassEntry93,		ClassEntry94,
ClassEntry95,		ClassEntry96,		ClassEntry97,		ClassEntry98,		ClassEntry99,
ClassEntry100,		ClassEntry101,		ClassEntry102,		ClassEntry103,		ClassEntry104,
ClassEntry105,		ClassEntry106,		ClassEntry107,		ClassEntry108,		ClassEntry109,
ClassEntry110,		ClassEntry111,		ClassEntry112,		ClassEntry113,		ClassEntry114,
ClassEntry115,		ClassEntry116,		ClassEntry117,		ClassEntry118,		ClassEntry119,
ClassEntry120,		ClassEntry121,		ClassEntry122,		ClassEntry123,		ClassEntry124,
ClassEntry125,		ClassEntry126,		ClassEntry127,		ClassEntry128,		ClassEntry129,
ClassEntry130,		ClassEntry131,		ClassEntry132,		ClassEntry133,		ClassEntry134,
ClassEntry135,		ClassEntry136,		ClassEntry137,		ClassEntry138,		ClassEntry139,
ClassEntry140,		ClassEntry141,		ClassEntry142,		ClassEntry143,		ClassEntry144,
ClassEntry145,		ClassEntry146,		ClassEntry147,		ClassEntry148,		ClassEntry149
};





/**
 ** 
 ** local (static) functions for finding entries in the ClassList in various ways.
 ** 
 **/



static int FindEntryByType(info)
struct classinfo *info;
{
    int i;

    for (i = 0; i < ClassListEntries; i++)  {
	if (ClassList[i].info == info) return i;
    }
    return -1;
}

static int FindEntryByName(name)
char *name;
{
    int i;

    for (i = 0; i < ClassListEntries; i++)  {
	if (strcmp(ClassList[i].className, name) == 0)
	    return i;
    }
    return -1;
}

static int FindEntry(name, version, load, header)
char *name;
unsigned long version;
int load;
struct classheader *header;
{
    int i;
    int returncode;

    /* give callers a clue */
    errno=0;
    class_Error=class_ErrNone;

    for (i = 0; i < ClassListEntries; i++)  {
	if (strcmp(ClassList[i].className, name) == 0 || strcmp(ClassList[i].classNameKey, name) == 0)  {
	    if (ClassList[i].info == NULL && load != NOINIT) {
		ClassList[i].info = (*ClassList[i].getinfo)(header,version);
		if(ClassList[i].info==NULL){ /* InitializeClass failed */
		    class_Error=class_ErrInit;
		    return -1;
		}

		if (ClassList[i].info->methods != NULL)
			ClassList[i].info->methods->info = ClassList[i].info;	/* -wjh */

		/* if needed, call *__InitializeClass() */
		if (ClassList[i].info->classinitfunc != NULL) {  /* yes, no *__InitializeClass() */
		    if ( (*(ClassList[i].info->classinitfunc))(ClassList[i].info->header) != TRUE) {  /* yes, OK */
			fprintf(stderr, "Error from %s__InitializeClass() !!\n", ClassList[i].info->name);
			return -1;
		    }
		}
		ClassList[i].info->textbase=NULL; /* statically loaded */
	    }
	    return i;
	}
    }

    if (load == LOAD)  {
	/* return the newly-loaded package's index */
	int fd;
	struct classinfo *info;	/* from the dynamic loaded code */
	char tname[MAXPATHLEN];
	char *base;
	struct classinfo *(*entrypoint)();
	long textlength;

	fd = pathopen(name, tname, doload_extension, version);
	if (fd < 0) return -1;

	entrypoint = ( struct classinfo * (*)() ) doload (fd, name, &base, &textlength, tname);
	close(fd);
	if (entrypoint == NULL){
	    class_Error=class_ErrLoad;
	    return -1;
	}

	/* call the loaded routine at its entry point */
	info = (*entrypoint)(header, version);

	if(info==NULL){
	    class_Error=class_ErrInit;
	    free(base);
	    return -1;
	}

	info->textbase=base;    /* text segment is first */
	info->textlength=textlength;

	/* register this class */
	returncode = class_EnterInfo(info, info->name, NULL, base, info->namekey);

	if (info->methods != NULL)
		info->methods->info = info;		/* wjh */

	/* if needed, call *__InitializeClass() */
	if (info->classinitfunc	== NULL) {  /* yes, no *__InitializeClass() */
	    return returncode;
	} else {
	    if ( (*(info->classinitfunc))(info->header)	== TRUE) {  /* yes, OK */
		return returncode;
	    } else {
		fprintf(stderr, "Error from %s__InitializeClass() !!\n", info->name);
		return -1;
	    }
	}
    }

    class_Error=class_ErrNotLoaded;
    return -1;
}




char *class_Lookup(header,cpindex) 
struct classheader *header;
int cpindex;
{
    int index;

    if ((index = FindEntry(header->name, header->versionnumber, LOAD, header)) != -1)  {
	header->classprocedures = ClassList[index].info->procs;
	return (char *) &(ClassList[index].info->procs->routines[0]);
    }
    else {
	fprintf(stderr, "Could not find the class methods for %s version 0x%x (%d)!\n", header->name, header->versionnumber, header->versionnumber);

	return NULL;
    }
}

int class_EnterInfo(info, name, proc, base, namekey)
struct classinfo *info;
char *name;
struct classinfo *(*proc)();
char *base;
char *namekey;
{
    int i;

    if (ClassList == NULL)  {
	MaxClassListEntries = INITIALLISTSIZE;
	ClassList = (struct classload *) malloc(MaxClassListEntries * sizeof(struct classload));
	ClassListEntries = 0;
    }
    else if (info != NULL && (i=FindEntryByType(info)) != -1) return i;
    else if (info == NULL && (i=FindEntryByName(name)) != -1) return i;
    else if (ClassListEntries == MaxClassListEntries)  {
	MaxClassListEntries += MaxClassListEntries >> 1;
	ClassList = (struct classload *) realloc(ClassList, MaxClassListEntries * sizeof(struct classload));
    }

    ClassList[ClassListEntries].info = info;
    ClassList[ClassListEntries].className = name;
    ClassList[ClassListEntries].classNameKey = namekey;
    ClassList[ClassListEntries].baseaddr = base;
    ClassList[ClassListEntries].getinfo = proc;

    i = ClassListEntries;
    ClassListEntries++;
    return i;
}

struct basicobject *class_NewObject(name)
char *name;
{
    int index;
    
    if ((index = FindEntry(name, class_VERSIONNOTKNOWN, LOAD, &unknownID)) != -1)  {
	return((struct basicobject *)(*(ClassList[index].info->procs->routines[0])) (&unknownID, ClassList[index].info->versionnumber));
    }
    return NULL;
}

struct classinfo *class_Load(name)
    char *name;
{
    int infoIndex;

    if ((infoIndex = FindEntry(name, class_VERSIONNOTKNOWN, LOAD, &unknownID)) != -1)
        return ClassList[infoIndex].info;
    else
        return NULL;
}

boolean class_IsLoaded(name)
char *name;
{
    return (FindEntry(name, class_VERSIONNOTKNOWN, NOLOAD, &unknownID) != -1);
}

boolean class_IsType(testobject, typeobject)
    struct basicobject *testobject, *typeobject;
{
    struct classinfo *testtype = testobject->methods->info;

    do {
        if (testtype == typeobject->methods->info)
            return TRUE;
/* Check for both NULL and "" superclassname because class generates "",
 * but should be fixed. */
        if (testtype->superclass == NULL && testtype->superclassname != NULL && *testtype->superclassname != '\0')
            if ((testtype->superclass = class_Load(testtype->superclassnamekey)) == NULL) {
                    fprintf(stderr, "Could not load class %s.\n", testtype->superclassname);
                    exit(1);
            }
        testtype = testtype->superclass;
    } while (testtype != NULL);

    return FALSE;
}

boolean class_IsTypeByName(testname, typename)
    char *testname;
    char *typename;
{
    struct classinfo *testinfo;
    long index = FindEntry(testname, class_VERSIONNOTKNOWN, LOAD, &unknownID);

    if (index == -1) return FALSE;
    testinfo = ClassList[index].info;
    do {
        if (strcmp(testinfo->name, typename) == 0)
            return TRUE;
/* Check for both NULL and "" superclassname because class generates "",
 * but should be fixed. */
        if (testinfo->superclass == NULL && testinfo->superclassname != NULL && *testinfo->superclassname != '\0')
            if ((testinfo->superclass = class_Load(testinfo->superclassnamekey)) == NULL) {
                    fprintf(stderr, "Could not load class %s.\n", testinfo->superclassname);
                    exit(1);
            }
        testinfo = testinfo->superclass;
    } while (testinfo != NULL);

    return FALSE;
}

void class_PrintName(self, file)
struct basicobject *self;
FILE *file;  {
    struct classinfo *info;

    if (file == NULL) file = stdout;
    for (info = self->methods->info; info; info = info->superclass)
	fprintf(file, "%s.", info->name);
    putc('\n', file);
}

/* support for the dynamic loading code */
static pathopen (aname, tname, ext, version)
char *aname;
char *tname;
char *ext;
unsigned long version;
{
char * ThisPath;
int fn;

    ThisPath = MapByName(aname, version);
    if (ThisPath == NULL) {
	ThisPath = MapByKey(aname, version);
    }
    if (ThisPath == NULL) {
	return -1;
    }

    strcpy (tname, ThisPath);
    free(ThisPath);	/* clean up */
    fn = open(tname,O_RDONLY,0);
    if (fn >= 0) {
	return fn;
    }

    return -1;
}

/* adds the given colon-separated pathlist onto the FRONT of globalPath */
void class_PrependClassPath(path)
char *path;
{
    char *p;

    if(path==NULL || *path=='\0')
	return;

    p=path+strlen(path);

    do{
	char *end=p;
	struct pathentry *pathel;

	while(p>path && *(p-1)!=':')
	    p--;

/* This includes space for the trailing NUL since a struct pathentry has
 * name declared to be 1 character long.
 */
	pathel=(struct pathentry *)malloc(sizeof(struct pathentry)+(p<end?end-p:1));

	pathel->next=globalPath;
	if(p<end) {
	    strncpy(pathel->name,p,end-p);
            pathel->name[end-p] = '\0';
        }
	else
	    strcpy(pathel->name,".");

	globalPath=pathel;

	p--;
    } while(p>=path);
}




/* set up the mapping tables */
void class_ProcessClassPath(path)
char *path;
{
    char *p;
    struct pathentry * ThisPath;
    int PathIndex;
    FILE * IndexFile;
    char * buffer[MAXPATHLEN];
    struct IndexEntry * ThisEntry;

    if (InitializeMappingPackage() != 0) {
	fprintf(stderr, "CLASS RUNTIME:  CAN NOT INITIALIZE MAPPING SUBSYSTEM!!!\n");
	exit(1);
    }

    if(path==NULL || *path=='\0') {
	fprintf(stderr, "CLASS RUNTIME:  NO CLASS SEARCH PATHS SPECIFIED!!!\n");
	exit(1);
    }

    p=path+strlen(path);

    do {
	char *end=p;
	struct pathentry *pathel;

	while(p>path && *(p-1)!=':')
	    p--;

/* This includes space for the trailing NUL since a struct pathentry has
 * name declared to be 1 character long.
 */
	pathel=(struct pathentry *)malloc(sizeof(struct pathentry)+(p<end?end-p:1));

	pathel->next=globalPath;
	if(p<end) {
	    strncpy(pathel->name,p,end-p);
            pathel->name[end-p] = '\0';
        }
	else
	    strcpy(pathel->name,".");

	globalPath=pathel;

	p--;
    } while (p >= path);


    PathIndex = -1;
    for (ThisPath = globalPath; ThisPath != NULL; ThisPath = ThisPath->next) {
	strcpy(buffer, ThisPath->name);
	strcat(buffer, "/");
	strcat(buffer, INDEXFILE);
	IndexFile = fopen(buffer, "r");	/* can this be opened? */
	if (IndexFile != NULL) {
	    PathIndex = EnterPathEntry(ThisPath->name);
	    if (PathIndex < 0) {
		fprintf(stderr, "CLASS RUNTIME:  ERROR SETTING SEARCH PATH!!!\n");
		exit(1);
	    }
	    while ((ThisEntry = ReadEntry(IndexFile)) != NULL) {
		if (EnterMapEntry(ThisEntry->Name, ThisEntry->Key, ThisEntry->Version, PathIndex, ThisEntry->Data) != 0) {
		    fprintf(stderr, "CLASS RUNTIME:  ERROR ENTERING MAPPING ENTRY!!!\n");
		    exit(1);
		}
		DestroyEntry(ThisEntry);
	    }
	    (void) fclose(IndexFile);
	} else {
	    DIR *dirp;
	    struct direct *dirent;
	    int len;
	    char sname[100];

	    if (errno != ENOENT) {
		fprintf(stderr, "CLASS RUNTIME: No index file found in '%s': error %d; some objects in this directory may be ignored.\n", ThisPath->name);
	    }
	    if ((dirp = opendir(ThisPath->name)) == NULL) {
		fprintf(stderr, "CLASS runtime warning:  CLASSPATH directory %s is not readable (error %d); ignoring it.\n", ThisPath->name, errno);
	    } else {
		PathIndex = EnterPathEntry(ThisPath->name);
		if (PathIndex < 0) {
		    fprintf(stderr, "CLASS RUNTIME:  ERROR SETTING SEARCH PATH!!!\n");
		    exit(1);
		}
		while ((dirent = readdir(dirp)) != NULL) {
		    len = strlen(dirent->d_name);
		    if (len < sizeof(sname) && len > 3 && !strcmp(dirent->d_name + len - 3, ".do")) {
			strcpy(sname, dirent->d_name);
			sname[len-3] = '\0';
			if (EnterMapEntry(sname, sname, class_VERSIONNOTKNOWN, PathIndex, dirent->d_name) != 0) {
			    fprintf(stderr, "CLASS RUNTIME:  ERROR ENTERING IMPLICIT MAPPING ENTRY!!!\n");
			    exit(1);
			}
		    }
		}
		closedir(dirp);
	    }
	}
    }


    if (PathIndex == -1) {
	fprintf(stderr, "CLASS RUNTIME:  NO CLASSPATH ELEMENTS WERE USABLE!!!\n");
	exit(1);
    }
}

/**
 **
 ** These routines manipulate the class runtime system.
 **
 **/



/* %%%% */
/* Omit type declaration; some sv_handler's are declared int, others void. */
static class_ErrorHandler();

/**
 ** Set up the initial conditions for the 
 ** class runtime system.  If the system is 
 ** already running free all the allocated space,
 ** close files, etc. before reinitializing system.
 **/
class_ErrorType class_Init(defaultPath)
char *defaultPath;
{
    char *envString;

    /* setup and parse path */
    envString = getenv("CLASSPATH");
    if(envString==NULL)
	class_ProcessClassPath(defaultPath);
    else
	class_ProcessClassPath(envString);

/* set up the signal catcher */


    /* initialization of signal catching code */
    handler.sv_handler = class_ErrorHandler;
    handler.sv_mask = 0;
    handler.sv_onstack = 0;

    sigvec(SIGUSR1, &handler, NULL);

    return class_ErrorNoError;	/* good return value */
}


/**
 ** Set and Get the debugging level and set the 
 **/
class_ErrorType class_SetDebugLevel(level)
class_DebugLevelType level;

{
    DebugLevel = level;	    /* %%%% */

    return class_ErrorNoError;
}


class_DebugLevelType class_GetDebugLevel()

{
    return DebugLevel;
}


/**
 ** Set and Get the search path for loading 
 ** objects.
 **/
class_ErrorType class_SetClassPath(path)
char *path;
{
    struct pathentry *pe=globalPath;

    while(pe!=NULL){
	struct pathentry *next=pe->next;
	free(pe);
	pe=next;
    }

    globalPath=NULL;

    class_PrependClassPath(path);

    return class_ErrorNoError;	/* good return value */
}


char *
class_GetClassPath()

{
    return NULL;
}




/**
 **
 **  These routines are used for debugging purposes.
 **
 **/


/**
 **  Print the headings for a class runtime dump.
 **/
void class_DumpClassEntry(file, index)
FILE *file;
int index;

{
struct classinfo *ThisEntry;

    ThisEntry = ClassList[index].info;

    fprintf(file, "\n");
    fprintf(file, "*** Information for entry %d:\n", index);
    fprintf(file, "\n");
    if (ThisEntry != NULL) {
	fprintf(file, "    class name:          %s\n", ThisEntry->name);
	fprintf(file, "    class name key:      %s\n", ThisEntry->namekey);
	fprintf(file, "    class info at:       0x%x\n", (unsigned long) ThisEntry);
	if (ThisEntry->superclassname != NULL) {
	    fprintf(file, "    superclass name:     %s\n", ThisEntry->superclassname);
	    fprintf(file, "    superclass name key: %s\n", ThisEntry->superclassnamekey);
	    fprintf(file, "    superclass info at:  0x%x\n", (unsigned long) ThisEntry->superclass);
	} else {
	    fprintf(file, "    This class has no superclass\n");
	}
	fprintf(file, "\n");
	fprintf(file, "    version:             0x%x (%d)\n", ThisEntry->versionnumber, ThisEntry->versionnumber);
	fprintf(file, "    methods table at:    0x%x\n", (unsigned long) ThisEntry->methods);
	fprintf(file, "    procedures table at: 0x%x\n", (unsigned long) ThisEntry->procs);
	fprintf(file, "    text base:           0x%x\n", (unsigned long) ThisEntry->textbase);
	fprintf(file, "    text length:         0x%x (%d)\n", ThisEntry->textlength, ThisEntry->textlength);
    } else {
	fprintf(file, "    ** This class is statically loaded and has not yet  **\n");
	fprintf(file, "    ** been used therefore it has not been initialized. **\n");
	fprintf(file, "\n");
	fprintf(file, "    class name:          %s\n", ClassList[index].className);
	fprintf(file, "    class name key:      %s\n", ClassList[index].classNameKey);
    }
    fprintf(file, "\n");
    fflush(file);
}


/**
 **  Dump the information about all the registered classes to file.
 **/
void class_DumpAllClassInfo(file)
FILE *file;

{
int i;	/* used to cycle through the list of classes */

    /* print a heading */
    fprintf(file, "\n");
    fprintf(file, "Dump of class runtime registered class information.\n");
    fprintf(file, "Entries are dumped in the order they were registered.\n");
    fprintf(file, "\n");


    /* print the information */
    if ((ClassList == NULL) || (ClassListEntries == 0)) {
	fprintf(file, "No classes have been registered yet.\n");
    } else if (MaxClassListEntries < ClassListEntries) {
	fprintf(file, "The class information has been corrupted!!\n");
    } else {
	for (i=0; i<ClassListEntries; i++) {
	    class_DumpClassEntry(file, i);
	}
    }

    /* print a message telling we're done */
    fprintf(file, "\n");
    fprintf(file, "End of class runtime registered classes information.\n");
    fprintf(file, "\n");
    fflush(file);
}




/**
 **  Dump the mapping table info that includes the current
 ** search path, the entries, then some stats about the 
 ** hash table that holds the entries.
 **/
void class_DumpMappingTableInfo(file)
FILE *file;

{
    /* print a heading */
    fprintf(file, "\n");
    fprintf(file, "Dump of class mapping information.\n");
    fprintf(file, "\n");

    DumpMappingInfo(file);
    DumpMappingStats(file);

    /* print a message telling we're done */
    fprintf(file, "\n");
    fprintf(file, "End of class mapping information.\n");
    fprintf(file, "\n");
    fflush(file);
}




/**
 **  Dump the information about this process
 **/
void class_DumpAllInfo(file)
FILE *file;

{
    /* print a heading */
    fprintf(file, "\n\n");
    fprintf(file, "Dump of class information for process %d.\n", getpid());
    fprintf(file, "\n");


    /* print the information */
    class_DumpAllClassInfo(file);
    class_DumpMappingTableInfo(file);

    /* print a message telling we're done */
    fprintf(file, "\n");
    fprintf(file, "End of class information for process %d.\n", getpid());
    fprintf(file, "\n");
    fflush(file);
}




/**
 ** error handler for the debuggin signal
 **/
static int class_ErrorHandler(sig, code, scp)
int sig;
int code;   /* always zero on most system, ignore */
struct sigcontex *scp;

{
FILE * DumpFile;

    DumpFile = fopen("/tmp/class.dmp", "w");
    if (DumpFile == NULL) {
	DumpFile = stderr;
    }
/*    if (debuglevel.....) { */
	if (sig == SIGUSR1) { /* is this the correct signal?*/
	    class_DumpAllInfo(DumpFile);
	}
/*    }; */
	if (DumpFile != stderr) {
	    (void) fclose(DumpFile);
	}
	return NULL;	/* who knows what these things are supposed to return */
}






/**
 **
 ** routines to help with profiling
 **
 **/

/**
 ** This returns the special symbol etext defined
 ** by most UNIX(tm) linkers (ld) to indicate the end
 ** of the text segment.
 **/
void *class_GetEText()

{
    return &etext;
}


/**
 ** This routine will return the base address
 ** for the text segment of a dynamically loaded object.
 ** The returned value should be NULL for statically 
 ** loaded objects.  This value of NULL is set up in
 ** the class_EnterInfo() routine.
 **/
void *class_GetTextBase(thisclass)
struct classinfo *thisclass;

{
    return (thisclass->textbase);
}


/**
 ** This routine will return the length of
 ** the text segment of a dynamically loaded object.
 ** The returned value should be 0 for statically 
 ** loaded objects.  This value of 0 is set up in
 ** the class_EnterInfo() routine.
 **/
unsigned long class_GetTextLength(thisclass)
struct classinfo *thisclass;

{
    return (thisclass->textlength);
}
