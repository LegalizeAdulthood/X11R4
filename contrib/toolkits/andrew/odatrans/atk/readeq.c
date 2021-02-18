/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readeq.c,v 1.3 89/09/19 21:23:45 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readeq.c,v $ */

#ifndef lint 
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readeq.c,v 1.3 89/09/19 21:23:45 mss Exp $";
#endif

/*
  $Log:	readeq.c,v $
 * Revision 1.3  89/09/19  21:23:45  mss
 * Made system v compatible
 * 
 * Revision 1.2  89/09/08  17:31:29  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:58  mss
 * Initial revision
 * 
 * Revision 1.3  89/04/30  12:11:51  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.2  88/11/04  17:06:24  mss
 * Use proper routine for reading equation data
 * 
 * Revision 1.1  88/11/02  16:52:47  mss
 * Initial revision
 * 

*/

#include <class.h>
#include <rect.h>
#include <view.ih>
#include <dataobj.ih>
#include <txtstvec.h>
#include <text.ih>
#include <viewref.ih>
#include <eq.ih>
#include <eqv.ih>

#include <odatk.h>
#include <odalib.ih>
#include <odaatk.h>
#include <odadef.h>
#include <readtable.h>
#include <errormsg.h>

/*
 * this is the current state of implementation.  To add new capabilities start by looking for comments with "%%%" in them.

 */

/* forward references  */
void GetEqData();

void ProcessEqCP(gD, curBasic, contents, contentType, contentMode, startPosition, curSV)
p_odainfo_type gD;
CONSTITUENT curBasic;
SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
INT_type contentType, contentMode;
long * startPosition;
struct text_statevector * curSV;
{
    CONSTITUENT	thisCP;			    /* this content portion */

    struct eq * newEqObj;
    struct viewref * dummyVR;
    int	i;				    /* general loop variable */

    SEQUENCE_BYTE_type			EqContents;


/*
 * top of procedure
 */

    if (gD->doDebugging) {
        printf("ProcessEqCP:  Entering\n");
    }


/* %%%
 * change this to be a for loop that loops through the contents creating a new ATK object for each.
 */
    if (contents->length != 1) {
	printf("ProcessEqCP:  don't know how to deal with %d CPs\n", contents->length);
    }
    thisCP = contents->sequence_value.constituents[0];	/* get the first one */


/*
 * get all the attributes from the eq portion of the ODA document 
 */

    if (odalib_GetAttr(thisCP, at_CONTENT_INFORMATION, (POINTER_type) &EqContents, BOOL_false, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessEqCP:  error getting at_CONTENT_INFORMATION attribute (%d)\n",odalib_TKError());
	printf("ProcessEqCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }

/*
 * now make a eq and insert the information.
 */

    if (gD->doDebugging) {
	printf("ProcessEqCP:  about to make eq object and insert data\n");
    }

    *startPosition = text_GetLength(gD->topLevelDocument);

    dummyVR = text_InsertObject(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), "eq", (char *) NULL);
    newEqObj = (struct eq *) dummyVR->dataObject;

    GetEqData(gD, newEqObj, EqContents);

    odalib_DeleteSequence(EqContents);

    if (gD->doDebugging){
        printf("ProcessEqCP:  Leaving\n");
    }

}

#define fPathTemplate "/tmp/tempEqData-ODA.XXXXXX"
/* Note trailing space below! */
#define eqFilter "eq.sh "

void GetEqData(gD, theEq, theContent)
p_odainfo_type gD;
struct eq * theEq;
at_CONTENT_INFORMATION_type theContent;
{

    char * returnedData;	    /* Space to hold returned string */
    FILE *tempFILE;		    /* file for fake table write */
    struct table * tableData;	    /* Where to get necessary info */
    char * fPath;		    /* Temporary file name to use */
    char * cmdStr;		    /* Command to execute to filter data */
    char * tempData;		    /* null teminated token stream */

    if (gD->doDebugging){
	printf("Entering GetEqData\n");
    }

    /* All of this is necessary if filtering is required -- for now just snarf the bits */
    /* First, create an appropriate string */
    tempData = (char *) malloc(theContent->length + 1);
    strncpy(tempData, theContent->sequence_value.bytes, theContent->length);
    /* and terminate the string */
    tempData[theContent->length] = (BYTE_type) 0;
    if (gD->doDebugging) {
	printf("About to insert the following text into equations: \"%s\"\n", tempData);
    }

    eq_Erase(theEq);
    eq_InsertTokens(theEq, (long) 0, tempData);
 
#if 0

    fPath = (char *) malloc(strlen(fPathTemplate + 1));
    if (!fPath) {
	/* couldn't make space for new path name! */
	printf("spreadda: could not allocate space for new file name %s\n", fPathTemplate);
	return;
    }

    (void) strcpy(fPath,fPathTemplate);
    mktemp(fPath);

    tempFILE = fopen(fPath,"w+");
    if (!tempFILE) {
	/* Could not create the file, abort */
	printf("GetEqData: could not create %s\n",fPath);
	return ;
    }

    /* Write out the bytes for later reading! */
    fwrite(theContent->sequence_value.bytes, sizeof(BYTE_type), theContent->length, tempFILE);

    /* Make sure everything is out */
    fflush(tempFILE);
    close(tempFILE);


    /* Run it through whatever fiter is required */
    cmdStr = (char *) malloc(strlen(tableFilter) + strlen(fPath) + 1);
    if (cmdStr) {
	(void) strcpy(cmdStr,tableFilter);
        (void) strcat(cmdStr,fPath);
	system(cmdStr);
	free(cmdStr);
    }
    else {
	printf("GetEqData: could not run table data through filter\n");
    }


    /* Slurp it back into the data array */
    tempFILE = fopen(fPath,"r");
    if (!tempFILE) {
	/* Could not create the file, abort */
	printf("GetEqData: could not reopen temporary file after filtering %s\n",fPath);
	return;
    }


    /* Read it in */
    table_ReadASCII(theTable, tempFILE);

    /* and delete the temporary file */
    fclose(tempFILE);
    unlink(fPath);
#endif

    if (gD->doDebugging){
	printf("Leaving ReadEqData\n");
    }


    return ;

}

