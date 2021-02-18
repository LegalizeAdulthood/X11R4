/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/spreadoda.c,v 1.3 89/09/19 21:23:33 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/spreadoda.c,v $ */

#ifndef lint
static char *rcsidspreadodac = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/spreadoda.c,v 1.3 89/09/19 21:23:33 mss Exp $";
#endif


#define debugflag 0
#include <class.h>

#include <rect.h>
#include <fontdesc.ih>
#include <environ.ih>
#include <dict.ih>
#include <viewref.ih>
#include <view.ih>
#include <envrment.ih>
#include <style.ih>

#include <table.ih>
#include <spread.ih>

#include <odatk.h>
#include <odadef.h>
#include <attsup.h>
#include <atkoda.ih>
#include <odalib.ih>

#include <spreadoda.eh>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Forward reference */

char * ReadTableData(/*gD, theView, &sizeInBytes*/);

INT_type FillInAttributes(gD, TableBLO, TableCP, theView, theDataObject)
struct atkoda		*gD;		/* Document/translation global state */
CONSTITUENT		TableBLO;	/* Basic logical object for table */
CONSTITUENT		TableCP;	/* Content Portion for table */
struct spread		*theView;	/* View on the table object */
struct table		*theDataObject;	/* table data object */
{

    INT_type		rval;
    char *		dataBytes;	/* bytes for the table */
    INT_type		sizeInBytes;	/* number of bytes in data */
    SEQUENCE_BYTE_type	TableContents;	/* Attribute for content information */

/*
  *  This is where the real work is done.
 *
 */


    if (gD->doDebugging) {
	printf("Entering FillInAttributes for Spread/table\n");
    }

    rval = SUCCESS_INT;


    /* Convert ATK spread/table data into appropriate ODA attributes */


    /* for now we let tables be text */
    odalib_SetAttr(TableCP, at_TYPE_OF_CODING, (POINTER_type) odalib_at_TYPE_OF_CODING_cc(), Ignore_Parms);

    /* Attach data to CP */
    dataBytes = ReadTableData(gD, theView, &sizeInBytes);

    if (odalib_ERROR_SEQUENCE() ==
	 (TableContents = odalib_MakeSequence(SEQUENCE_BYTE_tag, sizeInBytes))) {
	printf("Could not create byte sequence to hold content portion for tables (%d)\n", odalib_TKError());
	return ERROR_INT;
    }

    strncpy(TableContents->sequence_value.bytes, dataBytes, sizeInBytes);

    if (ERROR_INT == odalib_SetAttr(TableCP, at_CONTENT_INFORMATION, (POINTER_type) TableContents, ALL_PARMS)) {
	printf("Could not set attribute for content information on TableCP (%d)\n", odalib_TKError());
	/* return ERROR_INT; */
    }

    /* And get rid of copy of sequence */
    if (ERROR_INT == odalib_DeleteSequence(TableContents)) {
	printf("Could not deallocate content information sequence (%d)\n", odalib_TKError());
	/* return ERROR_INT; */
    }

    /* And get rid of the data bytes */
    free(dataBytes);

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving FillInAttributes (for spread/table) with return value %d\n", rval);
    }

    return rval;

}


#define fPathTemplate "/tmp/tempTableData-ODA.XXXXXX"
/* Note trailing space below! */
#define spreadFilter "spreadoda.sh "

char * ReadTableData(gD, viewer, numBytes)
struct atkoda * gD;
struct spread * viewer;
long * numBytes; 		    /* number of bytes from read */
{

    char * returnedData;	    /* Space to hold returned string */
    FILE *tempFILE;		    /* file for fake table write */
    struct table * tableData;	    /* Where to get necessary info */
    struct stat	statBuffer;	    /* Place to hold file size */
    struct chunk tableChunk;	    /* how much of table to write out */
    char * fPath;		    /* Temporary file name to use */
    char * cmdStr;		    /* Command to execute to filter data */

    if (gD->doDebugging){
	printf("Entering ReadTableData");
    }


    returnedData = NULL;
    *numBytes = 0;

    fPath = (char *) malloc(strlen(fPathTemplate + 1));
    if (!fPath) {
	/* couldn't make space for new path name! */
	printf("spreadda: could not allocate space for new file name %s\n", fPathTemplate);
	return NULL;
    }

    (void) strcpy(fPath,fPathTemplate);
    mktemp(fPath);

    tempFILE = fopen(fPath,"w+");
    if (!tempFILE) {
	/* Could not create the file, abort */
	printf("taboleoda: could not create %s\n",fPath);
	return NULL;
    }

    tableData = (struct table *) viewer->header.view.dataobject;
    if (!tableData) {
	printf("tableoda: missing data object for spread %X\n", viewer);
	return NULL;
    }

    /* Write out the data */
    tableChunk.LeftCol = -1;
    tableChunk.RightCol = tableData -> cols - 1;
    tableChunk.TopRow = -1;
    tableChunk.BotRow = tableData -> rows - 1;

    table_WriteASCII(tableData, tempFILE, &tableChunk);

    /* Make sure everything is out */
    fflush(tempFILE);
    close(tempFILE);

#if 0
    /* Run it through whatever fiter is required */
    cmdStr = (char *) malloc(strlen(spreadFilter) + strlen(fPath) + 1);
    if (cmdStr) {
	(void) strcpy(cmdStr,spreadFilter);
        (void) strcat(cmdStr,fPath);
	system(cmdStr);
	free(cmdStr);
    }
    else {
	printf("spreadoda: could not run table data through filter\n");
    }
#endif

    /* Slurp it back into the data array */
    tempFILE = fopen(fPath,"r");
    if (!tempFILE) {
	/* Could not create the file, abort */
	printf("spreadoda: could not reopen temporary file after filtering %s\n",fPath);
	return NULL;
    }

    /* Make space for the data */
    fstat(fileno(tempFILE),&statBuffer);
    returnedData = (char *) malloc(statBuffer.st_size);
    if(returnedData) {
	/* Go get it! */
	*numBytes = fread(returnedData, /* bytes*/ 1, statBuffer.st_size, tempFILE);
	if (*numBytes != statBuffer.st_size) {
	    /* missed some bytes somewhere! */
	    printf("tableoda: read %d bytes from file, expected %d\n", *numBytes, statBuffer.st_size);
	}
    }

    /* and delete the temporary file */
    fclose(tempFILE);
    unlink(fPath);

    if (gD->doDebugging){
	printf("Leaving ReadTableData");
    }


    return returnedData;

}


static CONSTITUENT MakeTable(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (gD->doDebugging) {
        printf("Entering MakeTable\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);

    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Table", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, "Basic ATK Table Object", DATA_CHAR_TYPE);

/* Done by reference to generic */
#ifdef RedundantArchClass
    odalib_SetAttr(rval, at_CONTENT_ARCH_CLASS, odalib_at_CONTENT_ARCH_CLASS_fp_rg(),ALL_PARMS);
#endif

    /* Connect to generic */
    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type) gD->genericTable, ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeTable\n");
    }

    return rval;

}


static CONSTITUENT MakeTableCP(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (gD->doDebugging) {
        printf("Entering MakeTableCP\n");
    }

    /* Fake table content with character content */
    rval = odalib_MakeContent(gD->outputDocument, CHARACTER_CONTENT);

    if (gD->doDebugging) {
        printf("Leaving MakeTableCP\n");
    }

    return rval;

}


enum view_DescriberErrs spreadoda__Describe(descObject,viewToDescribe,format,file,gD)
struct spreadoda * descObject;
struct spread * viewToDescribe;
char * format;
FILE * file;
struct atkoda * gD; {

    enum view_DescriberErrs EmbeddedDescriptionStatus;
    register struct table *d;
    CONSTITUENT TableBLO, TableCP;
    SEQUENCE_CONSTITUENT_type tempSeq;

    /* Raster processing is very simple:
              1. make a raster (basic logical object)
              2. attach it to the figure
              3. make a content portion
              4. attach it to the raster
              5. Fill in the raster attributes as appropriate */

    if(gD->doDebugging) {
	printf("Entering spreadodaDescribe\n");
    }
 
    /* Make the pieces */
    TableBLO = MakeTable(gD);
    TableCP = MakeTableCP(gD);

    /* Connect them together */
    atkoda_AppendBLOToPhrase(gD, TableBLO, TableCP, NULL);
    tempSeq = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag, 1);
    tempSeq->sequence_value.constituents[0] = TableCP;
    odalib_SetAttr(TableBLO, at_CONTENT_PORTIONS, (POINTER_type) tempSeq, ALL_PARMS);
    odalib_DeleteSequence(tempSeq);

    /* Connect to appropriate generics  (done in object creation)*/

    /* And set the attributes */

    if(gD->doDebugging) printf("Getting data object\n");
    d = (struct table *) viewToDescribe->header.view.dataobject;

    if(gD->doDebugging) printf("Filling in raster attributes\n");
    FillInAttributes(gD, TableBLO, TableCP, viewToDescribe, d);

    if (gD->doDebugging) {
	printf("About to return from spread (table) oda desciber\n");
    }


    return view_NoDescribeError;

}


