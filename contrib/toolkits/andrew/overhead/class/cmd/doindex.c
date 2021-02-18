
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	mkindex.c - make the class system index tables
 */

#include <andrewos.h> /* sys/types.h sys/file.h */
#include <stdio.h>
#include <a.out.h>
#include <setjmp.h>
#include <sys/signal.h>

#include <class.h>  /* this contains all the structs, etc. for the class system */
#include <doload.h>
#include <classind.h>


/*
 * some needed data structures.
 */

struct EntryStruct {
    char * Name;
    struct EntryStruct * Next;
};



/*
 * external symbols that have no include files
 */

extern int errno;


/*
 * global data
 */


static struct classheader unknownID = {NULL, "unknown", NULL, NULL};

struct EntryStruct * EntryList;
char * ProgramName;
int TimeOut;
int lockfd;


/*
 *
 * local routines
 *
 */


/**
 ** handle errors by printing the passed message to 
 ** stderr and exitting with a status of 1.
 **/
static void
Error(message, param)
char * message;
char * param;

{
    (void) fprintf(stderr, "%s: %s%s...program terminated!\n", ProgramName, message, param);
    (void) fflush(stderr);
    if (lockfd >= 0) {
	(void) unlink(INDEXLOCKFILE);	/* delete it */
    };
    (void) exit(1);
}


/**
 ** print out a usage message and exit.
 **/
static void
Usage()

{
    fprintf(stderr, "\n");
    fprintf(stderr, "%s -x -tseconds -ddirectory [files]\n", ProgramName);
    fprintf(stderr, "  -x     print this help message\n");
    fprintf(stderr, "  -t     maximum number of seconds to wait to lock index\n");
    fprintf(stderr, "  -d     directory in which to build index\n");
    fprintf(stderr, "  files  list of files to register in index\n");
    fprintf(stderr, "\n");
    Error("end of help message", "");
}


/**
 ** Add a file to process the list of entries.
 **/
static void
AddEntry(name)
char * name;

{
struct EntryStruct * ThisEntry;
struct EntryStruct * NewEntry;

    NewEntry = (struct EntryStruct *) malloc(sizeof(struct EntryStruct));
    if (NewEntry == NULL) {
	Error("can't allocate any more memory", "");
    }
    NewEntry->Name = (char *) malloc(strlen(name) + 1);
    if (NewEntry->Name == NULL) {
	free(NewEntry);
	Error("can't allocate any more memory", "");
    }
    strcpy(NewEntry->Name, name);
    NewEntry->Next = NULL;

    if (EntryList == NULL) {
	EntryList = NewEntry;
    } else {
	ThisEntry = EntryList;
	while (ThisEntry->Next != NULL) {
	    ThisEntry = ThisEntry->Next;
	}
	ThisEntry->Next = NewEntry;
    }
}


/**
 ** Delete an entry.
 **/
static void
DeleteEntry(entry)
struct EntryStruct * entry;

{
    free(entry->Name);
    free(entry);
}


/**
 ** Parse the passed arguments.
 ** 
 ** Try to be smart about this and allow the use to place a space between the 
 ** switch and it's argument.
 **/
static void
ParseArgs(argc, argv)
int argc;
char *argv[];

{
int i;

    TimeOut = 0;	    /* by default wait forever */
    ProgramName	= rindex(argv[0], '/');  /* set up program name for messages */
    if (ProgramName == NULL) {
	ProgramName = argv[0];	/* just use argv[0] */
    } else {
	ProgramName += 1;	/* jump over "/" */
    }
    EntryList =	NULL;	    /* clear this before starting */

    if (argc ==	1) {	    /* if empty command line print help */
	Usage();
    }

    for (i=1; i < argc; i++) {
	if (*argv[i] != '-') {
	    AddEntry(argv[i]);
	} else {
	    switch (*(argv[i]+1)) {
		case 'x':
		    Usage();
		    break;
		case 't':
		    if (*(argv[i]+2) != NULL) {
			(void) sscanf((argv[i]+2), "%d", &TimeOut);
		    } else {
			if ( (i+1) < argc) {
			    i += 1;
			    (void) sscanf(argv[i], "%d", &TimeOut);
			} else {
			    Error("no timeout value specified for -t switch", "");
			}
		    }
		    break;
		case 'd':
		    if (*(argv[i]+2) != NULL) {
			if (chdir(argv[i]+2) != 0) {
			    Error("can not change to directory ", (argv[i]+2));
			}
		    } else {
			if ( (i+1) < argc) {
			    i += 1;
			    if (chdir(argv[i]) != 0) {
				Error("can not change to directory ", (argv[i]+2));
			    }
			} else {
			    Error("no directory specified for -d switch", "");
			}
		    }
		    break;
		default:
		    Usage();
		    break;
	    }
	}
    }
}


/** 
 ** Update index file.
 **
 ** When we get here we must have exclusive access for
 ** the creation of the new index file.  This lock must
 ** be set by the caller.
 **/
int
UpdateIndex(entry)
struct IndexEntry * entry;

{
int wroteit;	    /* used to make sure the entry is written */
FILE *infd;
FILE *outfd;
struct IndexEntry * ThisEntry;

    fprintf(stdout, "%s: indexing %s ...", ProgramName, entry->Data);
    fflush(stdout);

    wroteit = 0;    /* haven't written it yet */

    infd = fopen(INDEXFILE, "r");

    outfd = fopen(INDEXNEWFILE, "w");
    if (outfd == NULL) {
	Error("Unable to open a new index file", "");
    }

    if (infd != NULL) {
	while ((ThisEntry = ReadEntry(infd)) != NULL) {
	    if (strcmp(ThisEntry->Data,	entry->Data) ==	0) {	/* match, replace it */
		WriteEntry(outfd, entry);	/* replace this entry */
		wroteit	= 1;			/* remember we wrote it */
	    } else {
		WriteEntry(outfd, ThisEntry);	/* put it back */
	    }
	    DestroyEntry(ThisEntry);
	}
    }

    if (wroteit	== 0) {	    /* didn't write it yet, do so now */
	WriteEntry(outfd, entry);
    }

    if (fclose(outfd) == EOF) {
	Error("Error closing index file", "");
    }
    if (infd != NULL)
	(void) fclose(infd);    /* ignore return value */

    if (rename(INDEXNEWFILE, INDEXFILE) != 0) {
	Error("Error creating new index file", "");
    }
    fprintf(stdout, "OK\n");
    fflush(stdout);

}


/**
 ** Process a particular entry.
 **
 ** When we get here we must have exclusive access for
 ** the creation of the new index file.  This lock must
 ** be set by the caller.
 **/
static void
ProcessEntry(entry)
char * entry;

{
int objfd;
struct classinfo *info;	/* from the dynamic loaded code */
struct classinfo *(*entrypoint)();
char *base;
long textlength;
char tname[256];

struct IndexEntry ThisEntry;


    objfd = open(entry, O_RDONLY, 0);
    if (objfd < 0) {
	Error("can't open file ", entry);
    }

    entrypoint = ( struct classinfo * (*)() ) doload(objfd, entry, &base, &textlength, tname);
    close(objfd);

    if (entrypoint == NULL){
	Error("NULL entrypoint from ", entry);
    }

    /* call the loaded routine at its entry point to get the classinfo structure */
    info = (*entrypoint)(&unknownID, class_VERSIONNOTKNOWN);

    if(info == NULL){
	Error("can not obtain classinfo structure for ", entry);
    }

    ThisEntry.Name = info->name;
    ThisEntry.Key = info->namekey;
    ThisEntry.Version = info->versionnumber;
    ThisEntry.Data = entry;

    UpdateIndex(&ThisEntry);

    free(base);
}



/** 
 ** main()
 **/

int
main(argc, argv)
int argc;
char *argv[];

{
struct EntryStruct * TempEntry;
int loopcount;


    ParseArgs(argc, argv);  /* this may chdir() */
    loopcount = 0;
    do {
	lockfd = open(INDEXLOCKFILE, O_CREAT | O_EXCL, 022);
	if (lockfd < 0) {
	    if ((TimeOut != 0) && (loopcount > TimeOut)) { /* timeout of 0 means wait forever */
		Error("timed out while trying to lock index files", "");
	    }
	    sleep(1);	/* don't hog the CPU */
	    loopcount++;
	}
    } while (lockfd < 0);   /* keep going till we get a valid file descriptor */


    while (EntryList != NULL) {
	ProcessEntry(EntryList->Name);
	TempEntry = EntryList;		/* save this to be free'd */
	EntryList = EntryList->Next;
	DeleteEntry(TempEntry);
    }

    (void) close(lockfd);	/* done with this so close it */
    (void) unlink(INDEXLOCKFILE);	/* and delete it */

    exit(0);			/* success */
}



