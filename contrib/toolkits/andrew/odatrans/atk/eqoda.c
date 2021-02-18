/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/eqoda.c,v 1.3 89/09/19 21:23:17 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/eqoda.c,v $ */

#ifndef lint
static char *rcsidspreadodac = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/eqoda.c,v 1.3 89/09/19 21:23:17 mss Exp $";
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

#include <eq.ih>
#include <eqv.ih>

#include <odatk.h>
#include <odadef.h>
#include <attsup.h>
#include <atkoda.ih>
#include <odalib.ih>

#include <eqoda.eh>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Forward reference */

char * ReadEqData(/*gD, theView, &sizeInBytes*/);

INT_type FillInAttributes(gD, EqBLO, EqCP, theView, theDataObject)
struct atkoda		*gD;		/* Document/translation global state */
CONSTITUENT		EqBLO;	/* Basic logical object for Eq */
CONSTITUENT		EqCP;	/* Content Portion for Eq */
struct eqview		*theView;	/* View on the Eq object */
struct eq		*theDataObject;	/* Eq data object */
{

    INT_type		rval;
    char *		dataBytes;	/* bytes for the Eq */
    INT_type		sizeInBytes;	/* number of bytes in data */
    SEQUENCE_BYTE_type	EqContents;	/* Attribute for content information */

/*
  *  This is where the real work is done.
 *
 */


    if (gD->doDebugging) {
	printf("Entering FillInAttributes for Eq\n");
    }

    rval = SUCCESS_INT;


    /* Convert ATK Eq data into appropriate ODA attributes */


    /* for now we let Eqs be text */
    odalib_SetAttr(EqCP, at_TYPE_OF_CODING, (POINTER_type) odalib_at_TYPE_OF_CODING_cc(), Ignore_Parms);

    /* Attach data to CP */
    dataBytes = ReadEqData(gD, theView, &sizeInBytes);

    if (odalib_ERROR_SEQUENCE() ==
	 (EqContents = odalib_MakeSequence(SEQUENCE_BYTE_tag, sizeInBytes))) {
	printf("Could not create byte sequence to hold content portion for Eqs (%d)\n", odalib_TKError());
	return ERROR_INT;
    }

    strncpy(EqContents->sequence_value.bytes, dataBytes, sizeInBytes);

    if (ERROR_INT == odalib_SetAttr(EqCP, at_CONTENT_INFORMATION, (POINTER_type) EqContents, ALL_PARMS)) {
	printf("Could not set attribute for content information on EqCP (%d)\n", odalib_TKError());
	/* return ERROR_INT; */
    }

    /* And get rid of copy of sequence */
    if (ERROR_INT == odalib_DeleteSequence(EqContents)) {
	printf("Could not deallocate content information sequence (%d)\n", odalib_TKError());
	/* return ERROR_INT; */
    }

    /* And get rid of the data bytes */
    free(dataBytes);

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving FillInAttributes (for Eq) with return value %d\n", rval);
    }

    return rval;

}


#define fPathTemplate "/tmp/tempEqData-ODA.XXXXXX"
/* Note trailing space below! */
#define spreadFilter "spreadoda.sh "

char * ReadEqData(gD, viewer, numBytes)
struct atkoda * gD;
struct eqview * viewer;
long * numBytes; 		    /* number of bytes from read */
{

    char * returnedData;	    /* Space to hold returned string */
    FILE *tempFILE;		    /* file for fake Eq write */
    struct eq * EqData;	    /* Where to get necessary info */
    struct stat	statBuffer;	    /* Place to hold file size */
    char * fPath;		    /* Temporary file name to use */
    char * cmdStr;		    /* Command to execute to filter data */
    INT_type	bufStart;
    INT_type	eqLength;

    if (gD->doDebugging){
	printf("Entering ReadEqData");
    }

    /* Most of this is unnecessary */
    EqData = (struct eq *) viewer->header.view.dataobject;

    eqLength = eq_Size(EqData) + 1;
    eqLength = 1000; /* I wonder how big it really is */
    returnedData = (char *) malloc(eqLength + 1);
    bufStart = 0;
    *numBytes = eq_GetTokens(EqData, &bufStart, eq_Size(EqData), returnedData, (eqLength + 1));

    if (gD->doDebugging) {
	printf("Got %d bytes from reading, with a eqn size of %d\n", *numBytes, eqLength);
    }

#if 0
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

    if (!EqData) {
	printf("Eqoda: missing data object for spread %X\n", viewer);
	return NULL;
    }

    /* Write out the data */
    EqChunk.LeftCol = -1;
    EqChunk.RightCol = EqData -> cols - 1;
    EqChunk.TopRow = -1;
    EqChunk.BotRow = EqData -> rows - 1;

    Eq_WriteASCII(tableData, tempFILE, &tableChunk);

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
#endif
    if (gD->doDebugging){
	printf("Leaving ReadTableData");
    }


    return returnedData;

}


static CONSTITUENT MakeEq(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (gD->doDebugging) {
        printf("Entering MakeEq\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);

    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Equation", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, "Basic ATK Equation Object", DATA_CHAR_TYPE);

/* Done by reference to generic */
#ifdef RedundantArchClass
    odalib_SetAttr(rval, at_CONTENT_ARCH_CLASS, odalib_at_CONTENT_ARCH_CLASS_fp_rg(),ALL_PARMS);
#endif

    /* Connect to generic */
    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type) gD->genericEquation, ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeEq\n");
    }

    return rval;

}


static CONSTITUENT MakeEqCP(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (gD->doDebugging) {
        printf("Entering MakeEqCP\n");
    }

    /* Fake Eq content with character content */
    rval = odalib_MakeContent(gD->outputDocument, CHARACTER_CONTENT);

    if (gD->doDebugging) {
        printf("Leaving MakeEqCP\n");
    }

    return rval;

}


enum view_DescriberErrs eqoda__Describe(descObject,viewToDescribe,format,file,gD)
struct eqoda * descObject;
struct eqview * viewToDescribe;
char * format;
FILE * file;
struct atkoda * gD; {

    enum view_DescriberErrs EmbeddedDescriptionStatus;
    register struct eq *d;
    CONSTITUENT EqBLO, EqCP;
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
    EqBLO = MakeEq(gD);
    EqCP = MakeEqCP(gD);

    /* Connect them together */
    atkoda_AppendBLOToPhrase(gD, EqBLO, EqCP, NULL);
    tempSeq = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag, 1);
    tempSeq->sequence_value.constituents[0] = EqCP;
    odalib_SetAttr(EqBLO, at_CONTENT_PORTIONS, (POINTER_type) tempSeq, ALL_PARMS);
    odalib_DeleteSequence(tempSeq);

    /* Connect to appropriate generics  (done in object creation)*/

    /* And set the attributes */

    if(gD->doDebugging) printf("Getting data object\n");
    d = (struct eq *) viewToDescribe->header.view.dataobject;

    if(gD->doDebugging) printf("Filling in equation attributes\n");
    FillInAttributes(gD, EqBLO, EqCP, viewToDescribe, d);

    if (gD->doDebugging) {
	printf("About to return from spread (Eq) oda desciber\n");
    }


    return view_NoDescribeError;

}


