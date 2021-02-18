/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/maint/RCS/mkindex.c,v 2.20 89/10/21 13:03:39 pgc Exp $ */
/* $ACIS:mkindex.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/maint/RCS/mkindex.c,v $ */
 
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/maint/RCS/mkindex.c,v 2.20 89/10/21 13:03:39 pgc Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
 
#define FSSIZE		32	/* max number of files */
#define BUCKETS		11	/* number of hash buckets */
#define MANSUBS "12345678nolp"	/* array of possible subdirectories of MANDIR, ie man1, mann */

 
#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/stat.h>
 
#include <andrewos.h> /* sys/types.h sys/file.h */
#include <index.h>

extern char *index();
static char *prog;		/* our name */
 
static int verbose = 0;
 
/*
 * lowercases's a string.
 */
static char *LowerCase(astring)
register char *astring;
{
    register char *tp = astring;

    while (tp && *tp != NULL)
	if (isupper(*tp)) {
	    *tp = tolower(*tp);
	    tp++;
	} else
	    tp++;
    return astring;
}


static BuildIndex(aindex, arealName, alinkName)
char *arealName, *alinkName;
register struct Index *aindex;
{
    register DIR *td;
    register struct direct *tde;
    register char *tp, *tfp;
    char pathBuffer[MAXPATHLEN];
    char keyBuffer[MAXPATHLEN / 4];
    char mandir = 0;
    static char ManName[] = "man";
    struct stat tstat;
    register long code;
    
    td = opendir(arealName);
    if (!td) {
        fprintf(stderr, "%s: can't open %s; skipping.\n", prog, arealName);
        return 2;
    } else {
	if (verbose)
	    printf("%s: indexing %s as %s.\n", prog, arealName, alinkName);
    }

    tfp = rindex(arealName, '/');
    mandir = (strcmp( (tfp) ? tfp+1 : arealName, ManName) == 0) ? 1 : 0;

    /* make a base path */
    strcpy(pathBuffer, alinkName);
    tfp = pathBuffer + strlen(pathBuffer);
    *tfp++ = '/';		/* tfp now points after the trailing / */

    while(tde=readdir(td)) {
        if (strcmp(tde->d_name, ".") == 0 || strcmp(tde->d_name, "..") == 0)
	    continue;
        strcpy(tfp, tde->d_name); /* complete the path */

	strcpy(keyBuffer, tde->d_name);
        tp = index(keyBuffer, '.');
        if (tp) *tp = '\0';           /* remove extension */

	/* check for "man" subdir */
        code = stat(pathBuffer, &tstat);
        if ((code == 0 && (tstat.st_mode & S_IFMT) == S_IFDIR) && (mandir != 0) && strncmp(tde->d_name, ManName, sizeof(ManName)-1) == 0) {
	    char rpath[MAXPATHLEN];

	    strcpy(rpath, arealName);
	    strcat(rpath, "/");
	    strcat(rpath, tde->d_name);

	    BuildIndex(aindex, rpath, pathBuffer);
	} else {
	    if (tp && index(MANSUBS, *(tp+1))) { /* it's a man page */
#if 0
		struct recordSet *ts;
#endif /* 0 */
		/* add w/o extension */
		index_AddPrimary(aindex, LowerCase(keyBuffer), pathBuffer);
#if 0
		/* I wanted to check if the index so far contained an entry for the
		   name sans extension, and the add_primary without the extension, so
		   this entry would be added to the list of files available through the
		   keyword of the name sans extension, but GetAnySet seems to have some
		   detrimental side effects... */
		ts = index_GetAnySet(aindex, keyBuffer);
		if (ts->count != 0) { /* add with no extension, since already there like that */
		    index_AddPrimary(aindex, LowerCase(keyBuffer), pathBuffer);
		    recordset_Free(ts);
		}
#endif /* 0 */

		*tp = '.';	/* put back extension */
		/* add with extension */
		index_AddPrimary(aindex, LowerCase(keyBuffer), pathBuffer);
	    } else {
		if ((code == 0 && (tstat.st_mode & S_IFMT) != S_IFDIR)) {
		    /* don't add directories */
		    index_AddPrimary(aindex, LowerCase(keyBuffer), pathBuffer);
		}
	    }
	}
    }
    closedir(td);
    return 0;
}


void show_usage()
{
    fprintf(stderr,"usage: %s [-v] input-file destination-index-dir\n", prog);
    fprintf(stderr,"\tinput file contains real-dir referencing-name pairs\n");
    fprintf(stderr,"\t-v: verbose mode\n");
}


main(argc, argv)
int argc;
register char **argv;
{
    struct Index *newIndex;
    long code, lineNo[FSSIZE];
    FILE *inputFile[FSSIZE], *tfile;
    char *inputName, *destinationName, *tmp;
    char opcode[MAXPATHLEN], path1[MAXPATHLEN], path2[MAXPATHLEN];
    int fsPtr;
 
    inputName = (char *) NULL;
    destinationName = (char *) NULL;
    tmp = rindex(*argv, '/');
    if (tmp)
	prog = tmp+1;
    else
	prog = *argv;

    while(*++argv!=NULL)
	if(**argv=='-')
	    switch((*argv)[1]){
		case 'v':
		    verbose = 1;
		    break;
		case 'x':
		case 'h':
		    show_usage();
		    exit(0);
		    break;
		case 'f':
		    fprintf(stderr,"%s: -f switch is now obsolete, continuing anyway\n", prog);
		default:
		    fprintf(stderr,"%s: unrecognized switch: %s\n", prog, *argv);
		    show_usage();
		    if ((*argv)[1] != 'f')
			exit(1);
		    break;
	    } else {
		if (inputName == NULL)
		    inputName = *argv;
		else if (destinationName == NULL)
		    destinationName = *argv;
		else {
		    show_usage();
		    exit(1);
		}
	    }

    if (destinationName == NULL || inputName == NULL) {
        show_usage();
	exit(1);
    }

    inputFile[0] = fopen(inputName, "r");
    fsPtr = 0;      /* current input file */

    if (!inputFile[0]) {
        fprintf(stderr,"%s: input file %s not found.\n", prog, inputName);
        exit(1);
    }
    
    /* now we create a destination index */
    if (verbose)
	printf("%s: refreshing index.\n", prog);

    index_Create(destinationName, BUCKETS);
    
    newIndex = index_Open(destinationName);
    
    /* now read each dir, and add the appropriate primary elements to newIndex */
    if (!newIndex) {
        fprintf(stderr, "%s: could not create index %s\n", prog, destinationName);
        exit(1);
    }

/*    lp = lineNo + fsPtr;
    *lp = 0;
*/
    lineNo[fsPtr] = 0;		/* evidently Sun4.0 doesn't grok this --mrp */
    while (1) {
        lineNo[fsPtr]++;
        code = fscanf(inputFile[fsPtr], "%s", opcode);
        if (code <= 0) {
            fclose(inputFile[fsPtr]);
            if (fsPtr <= 0) break;
            if (verbose)
	        printf("%s: include file end.\n", prog);
            fsPtr--;
            continue;
        }
        if (opcode[0] == '#') {	/* comment */
	    int tc;

	    while (1) {
		tc = getc(inputFile[fsPtr]);
		if (tc <= 0 || tc == '\n')
		    break;
	    }
            continue;
        }

	/* dir to index */
        if (!strcmp(opcode, "dir")) {
            code = fscanf(inputFile[fsPtr], "%s %s", path1, path2);
            if (code != 2) {
                fprintf(stderr, "%s: input line %d: wrong number of parameters (%d should be 2) to dir operation\n", prog, code, lineNo[fsPtr]);
		exit(1);
            } else 
		BuildIndex(newIndex, path1, path2);
        }

	/* alias to add */
        else if (!strcmp(opcode, "key")) {
            code = fscanf(inputFile[fsPtr], "%s %s", path1, path2);
            if (code != 2) {
                fprintf(stderr, "%s: line %d: wrong number of parameters (%d should be 2) to key operation\n", prog, code, lineNo[fsPtr]);
		exit(1);
            } else {
		if (verbose)
		    printf("%s: mapping key '%s' to file %s\n", prog, LowerCase(path1), path2);
		index_AddPrimary(newIndex, LowerCase(path1), path2);
	    }
        }

	/* include file */
        else if (!strcmp(opcode, "include")) {
            code = fscanf(inputFile[fsPtr], "%s", path1);
            if (code != 1) {
                fprintf(stderr, "%s: line %d: syntax error in include command\n", prog, lineNo[fsPtr]);
                exit(1);
            }
            tfile = fopen(path1, "r");
            if (!tfile) {
                fprintf(stderr, "%s: line %d: include file %s not found.\n", prog, lineNo[fsPtr], path1);
                exit(1);
            }
            if (verbose)
		printf("%s: including subfile %s.\n", prog, path1);
            inputFile[++fsPtr] = tfile;
            lineNo[fsPtr] = 0;
        }
        else {
            printf("%s: line %d -- unknown opcode in input file '%s'\n", prog, lineNo[fsPtr], opcode);
            exit(1);
        }
    }
    if (verbose)
	printf("%s: storing index files.\n", prog);
    index_Close(newIndex);
    return(0);
}
