/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readtable.c,v 1.3 89/09/19 21:24:10 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readtable.c,v $ */

#ifndef lint 
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readtable.c,v 1.3 89/09/19 21:24:10 mss Exp $";
#endif

/*
  $Log:	readtable.c,v $
 * Revision 1.3  89/09/19  21:24:10  mss
 * Made system v compatible
 * 
 * Revision 1.2  89/09/08  17:32:15  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:52  mss
 * Initial revision
 * 
 * Revision 1.3  89/04/30  12:12:04  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.2  88/11/04  17:06:57  mss
 * Added state vector argument
 * 
 * Revision 1.1  88/11/02  16:51:23  mss
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
#include <table.ih>
#include <spread.ih>

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
void GetTableData();

void ProcessTableCP(gD, curBasic, contents, contentType, contentMode, startPosition, curSV)
p_odainfo_type gD;
CONSTITUENT curBasic;
SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
INT_type contentType, contentMode;
long * startPosition;
struct text_statevector * curSV;
{
    CONSTITUENT	thisCP;			    /* this raster's content portion */

    struct table * newTableObj;
    struct viewref * dummyVR;
    int	i;				    /* general loop variable */

    SEQUENCE_BYTE_type			TableContents;


/*
 * top of procedure
 */

    if (gD->doDebugging) {
        printf("ProcessTableCP:  Entering\n");
    }


/* %%%
 * change this to be a for loop that loops through the contents creating a new ATK raster for each.
 */
    if (contents->length != 1) {
	printf("ProcessTableCP:  don't know how to deal with %d CPs\n", contents->length);
    }
    thisCP = contents->sequence_value.constituents[0];	/* get the first one */


/*
 * get all the attributes from the table portion of the ODA document 
 */

    if (odalib_GetAttr(thisCP, at_CONTENT_INFORMATION, (POINTER_type) &TableContents, BOOL_false, (PARM_MASK_type *) 0) == ERROR_INT) {
	printf("ProcessTableCP:  error getting at_CONTENT_INFORMATION attribute (%d)\n",odalib_TKError());
	printf("ProcessTableCP:  odalib_TKErrorMesg() = %s\n", odalib_TKErrorMesg());
    }

/*
 * now make a table and spread and insert the information.
 */

    if (gD->doDebugging) {
	printf("ProcessTableCP:  about to make table object and insert data\n");
    }

    *startPosition = text_GetLength(gD->topLevelDocument);

    dummyVR = text_InsertObject(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), "table", "spread");
    newTableObj = (struct table *) dummyVR->dataObject;

    GetTableData(gD, newTableObj, TableContents);

    odalib_DeleteSequence(TableContents);

    if (gD->doDebugging){
        printf("ProcessTableCP:  Leaving\n");
    }

}

#define fPathTemplate "/tmp/tempTableData-ODA.XXXXXX"
/* Note trailing space below! */
#define tableFilter "table.sh "

void GetTableData(gD, theTable, theContent)
p_odainfo_type gD;
struct table * theTable;
at_CONTENT_INFORMATION_type theContent;
{

    char * returnedData;	    /* Space to hold returned string */
    FILE *tempFILE;		    /* file for fake table write */
    struct table * tableData;	    /* Where to get necessary info */
    struct chunk tableChunk;	    /* how much of table to write out */
    char * fPath;		    /* Temporary file name to use */
    char * cmdStr;		    /* Command to execute to filter data */

    if (gD->doDebugging){
	printf("Entering GetTableData\n");
    }


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
	printf("GetTableData: could not create %s\n",fPath);
	return ;
    }

    /* Write out the bytes for later reading! */
    fwrite(theContent->sequence_value.bytes, sizeof(BYTE_type), theContent->length, tempFILE);

    /* Make sure everything is out */
    fflush(tempFILE);
    close(tempFILE);

#if 0
    /* Run it through whatever fiter is required */
    cmdStr = (char *) malloc(strlen(tableFilter) + strlen(fPath) + 1);
    if (cmdStr) {
	(void) strcpy(cmdStr,tableFilter);
        (void) strcat(cmdStr,fPath);
	system(cmdStr);
	free(cmdStr);
    }
    else {
	printf("GetTableData: could not run table data through filter\n");
    }
#endif

    /* Slurp it back into the data array */
    tempFILE = fopen(fPath,"r");
    if (!tempFILE) {
	/* Could not create the file, abort */
	printf("GetTableData: could not reopen temporary file after filtering %s\n",fPath);
	return;
    }


    /* Read it in */
    table_ReadASCII(theTable, tempFILE);

    /* and delete the temporary file */
    fclose(tempFILE);
    unlink(fPath);

    if (gD->doDebugging){
	printf("Leaving ReadTableData");
    }


    return ;

}

