/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/datacat/RCS/datacata.c,v 2.12 89/10/27 11:07:04 tpn Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/datacat/RCS/datacata.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/datacat/RCS/datacata.c,v 2.12 89/10/27 11:07:04 tpn Exp $";
#endif /* lint */
#include <sys/param.h> /* For MAXPATHLEN. */

/*
*	datacat - A program for concatening ez documents
*
*
*
*
*/
#include <class.h>
#include <andrewos.h>
#include <datacata.eh>
#include <text.ih>
#include <dataobj.ih>
#include <environ.ih>
#include <keymap.ih>
/* output options */


extern int errno;

char *progname;
#define checkandprint(A) if(A) {datacatapp_PrintVersionNumber(self);A = FALSE;}

void datacatapp__ReadInitFile(self)
struct datacatapp *self;
{
}

boolean datacatapp__ParseArgs (self,argc, argv)
struct datacatapp *self;
    int argc;
    char **argv;
/* Since we are non-interactive, everything is done in this function so we
can handle the args as they come */
    {register int i;
    char *DocumentName ;
    long clen;
    FILE *ofile,*f,*fopen();
    struct text *tx;
    boolean pv = TRUE;
    boolean verbose = FALSE;
#ifdef LOCKANDDELETE
    int LockAndDelete = 0;
#endif /* LOCKANDDELETE */
    /* initialize as if ez */
    ((struct application * )self)->name = "ez";
    super_ReadInitFile(self);
    ((struct application * )self)->name = "datacat";
    DocumentName = NULL;
    ofile = stdout; f = NULL;
    progname = argv[0];
    tx = text_New();
    for (i=1;i<argc;i++){
	if (argv[i][0] == '-'){
	    switch(argv[i][1]){
		case 'q': 
		    pv = FALSE;
		    break;
		case 'v':
		    pv = TRUE;
		    verbose = TRUE;
		    break;
		case 'O':
		case 'o':
		    if(argv[i][2] != '\0')
			DocumentName = &argv[i][2];
		    else if(++i  < argc){
			DocumentName = argv[i];
		    }
		    else {
			checkandprint(pv);
			usage();
		    }
		    if((ofile = (fopen(DocumentName,"w"))) == NULL){
			checkandprint(pv);
			fprintf(stderr,"Can't open %s for output\n",DocumentName);
			exit(1);
		    }
		    break;

#ifdef LOCKANDDELETE
		case 'z':
		    LockAndDelete = 1;
		    break;
#endif /* LOCKANDDELETE */

		default:
		    checkandprint(pv);
		    if(text_GetLength(tx) == 0){
			fprintf(stderr,"bad flag %s\n", argv[i]);
			usage();
		    }
		    else 
			fprintf(stderr,"bad flag %s - ignoring\n", argv[i]);
		    break;
	    }
	}
	else {
#ifndef LOCKANDDELETE
	    if((f = fopen(argv[i],"r") ) == NULL){
		checkandprint(pv);
		fprintf(stderr,"Can't open %s\n",argv[i]);
		continue;
	    }
#else /* LOCKANDDELETE */
	    if((f = fopen(argv[i],(LockAndDelete ? osi_F_READLOCK : "r")) ) == NULL){
		checkandprint(pv);
		fprintf(stderr,"Can't open %s\n",argv[i]);
		continue;
	    }
	    if (LockAndDelete) {
		if (osi_ExclusiveLockNoBlock(fileno(f))){
		    checkandprint(pv);
		    fprintf(stderr, "Cannot lock %s (%d)\n", argv[i], errno);
		    fclose(f);
		    continue;
		}
	    }
#endif /* LOCKANDDELETE */
	    checkandprint(pv);
	    if(verbose){
		fprintf(stderr,"concatenating  %s\n",argv[i]);
		fflush(stderr);
	    }
	    clen = text_GetLength(tx);
	    text_InsertCharacters(tx,clen,"\n",1);
	    text_InsertFile(tx,f,argv[i], clen);
	    fclose(f);
	    clen = text_GetLength(tx);
	    if(text_GetChar(tx,clen - 2) == '\n')
		text_DeleteCharacters(tx,clen - 2,1);
#ifdef LOCKANDDELETE
	    if (LockAndDelete && unlink(argv[i])) {
		fprintf(stderr, "Cannot delete file %s\n", argv[i]);
	    }
#endif /* LOCKANDDELETE */
	}
    }
    text_Write(tx,ofile,1,0);
    return TRUE;
    }
datacatapp__Run(self)
struct datacatapp *self;
{   /* we are already done */
    exit(0);
}
void datacatapp__FinalizeObject(classID,self)
struct classheader *classID;
struct datacatapp *self;
{
}
boolean datacatapp__InitializeObject(classID,self)
struct classheader *classID;
struct datacatapp *self;
{
    datacatapp_SetMajorVersion(self,1);
    datacatapp_SetMinorVersion(self,0);
    datacatapp_SetPrintVersionFlag(self,FALSE);
    return TRUE;

}
usage(){
	fprintf(stderr,"usage: %s  <-quiet> <-verbose> < -o OutputFileName> file[s] \n",progname);
	fflush(stderr);
	exit(2);
	}
