/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/atkodaapp.c,v 1.6 89/10/17 15:07:18 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/atkodaapp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/atkodaapp.c,v 1.6 89/10/17 15:07:18 mss Exp $";
#endif

/*
*
*	Modified from bx and be1 ezprint code.
*
*
*
*/
#include <class.h>
#include <atkodaapp.eh>
#include <odatk.h>
#include <odadef.h>

#include <im.ih>
#include <text.ih>
#include <textv.ih>
#include <textoda.ih>
#include <view.ih>
#include <dataobj.ih>
#include <filetype.ih>
#include <atkoda.ih>
#include <odalib.ih>
#include <odaatkapp.ih>

extern int errno;

char *progname;
struct atkoda * globalODAData;
static int doReadBack = 0;
static int doODADump = 0;
static BOOL_type printWarnings = BOOL_false;
static BOOL_type globalDebug = BOOL_false;
static BOOL_type rasterScaling = BOOL_false;
static BOOL_type doFudgeMargins = BOOL_false;
static int useGDB = 0;

void atkodaapp__ReadInitFile(self)
struct atkodaapp *self;
{
    /* Maybe we should have some preference processing here */
}

static INT_type NumCRsInParaBreak = 2;
static int useStdOut = 0;


long InitGlobal(gD,sourceName, outputName, ofile, mainDataObject, mainView)
struct atkoda * gD;
char * sourceName;
char * outputName;
FILE * ofile;
struct dataobject * mainDataObject;
struct view * mainView; {

    at_USER_VISIBLE_NAME_type	globalComment;
    char			* tempString;
    struct text_statevector	tempStateVector;
    CONSTITUENT			temp_ERROR_CONSTITUENT;		/* temp for vax 4.3 bug */
    DOCUMENT_type		temp_ERROR_DOCUMENT;		/* temp for vax 4.3 bug */

    gD->inputFileName = sourceName;
    gD->outputFileName = outputName;
    gD->outputFileDescriptor = ofile;
    gD->topLevelDocument = mainDataObject;
    gD->topLevelView = mainView;
    gD->NumCRsInParaBreak = NumCRsInParaBreak;

    if (useStdOut) {
	gD->ODAParams.error_od = fileno(stdout);
    }

    if (LCL_ERROR_INT == odalib_LOCAL_Init()){
	fprintf(stdout,"Cannot initialized the local interface\n");
	return -1;
    }

    temp_ERROR_CONSTITUENT = odalib_ERROR_CONSTITUENT();
    temp_ERROR_DOCUMENT = odalib_ERROR_DOCUMENT();

    if (ERROR_INT == odalib_InitToolKit(&gD->ODAParams)) {
	fprintf(stdout,"Cannot initialized the toolkit \n");
	return -1;
    }

    /* Since toolkit now exists, can get toolkit specific state from it */
    atkoda_InitializeODAState(gD);
    if ((printWarnings != BOOL_false) || (globalDebug != BOOL_false) ) {
	gD->ODAParams.print_warnings = BOOL_true;
    }

    if (temp_ERROR_DOCUMENT == (gD->outputDocument = odalib_MakeDocument())){
	fprintf(stdout,"Cannot create Document -- reason unknown\n");
	return -1;
    }

    if (temp_ERROR_CONSTITUENT == 
	 (gD->documentProfile = odalib_MakeDocumentProfile(gD->outputDocument))){
	fprintf(stdout,"Cannot create document profile\n");

    }

    if (temp_ERROR_CONSTITUENT == 
	 (gD->specificLogicalRoot = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_doc_logical_root, SPECIFIC_COMPONENT))) {
	fprintf(stdout,"Cannot create document logical root\n");
	return -1;
    }

    tempString = (char *) malloc(sizeof(char) * 200);
    if (!tempString) {
	printf("Could not allocate memory for creating comment on doc log root\n");
	return -1;
    }
    sprintf(tempString,"Input file %s; output file %s; date ***", sourceName, outputName);
    globalComment = odalib_Chars2String(tempString);
    free(tempString);
    if (odalib_ERROR_SEQUENCE() == globalComment) {
	fprintf(stdout,"Cannot create attribute for global comment\n");
    }
    else {
	if (ERROR_INT == odalib_SetAttr(gD->specificLogicalRoot, at_USER_VISIBLE_NAME, globalComment, ALL_PARMS)) {
	    fprintf(stdout,"Cannot place USER READABLE attribute on spec. logical root\n");
	}
	odalib_DeleteString(globalComment);
    }
    
    
    if (temp_ERROR_CONSTITUENT == 
	 (gD->genericLayoutRoot = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_doc_layout_root, GENERIC_COMPONENT))) {
	fprintf(stdout,"Cannot create document layout root\n");
	return -1;
    }

    gD->genericLogicalRoot = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_doc_logical_root, /* generic */ GENERIC_COMPONENT);
    odalib_SetAttr(gD->specificLogicalRoot, at_OBJECT_CLASS, (POINTER_type) gD->genericLogicalRoot, ALL_PARMS);

    atkoda_PushPassage(gD, gD->specificLogicalRoot, &tempStateVector, (struct style*) NULL);

    return 0;
}

INT_type FinishDocumentProfile(gD)
struct atkoda * gD; {
    INT_type		rval;
    INT_type		FontCount;
    dp_FONTS_LIST_type	tempFL;
    p_font_list_blk	curFontPtr;

    rval = SUCCESS_INT;	

    if (gD->fontList) {
	tempFL = (dp_FONTS_LIST_type) odalib_MakeSequence(SEQUENCE_FONT_PAIR_tag, (gD->fontList->ODAFontID));
	for(FontCount = gD->fontList->ODAFontID, curFontPtr = gD->fontList;
	    FontCount>0;
	    FontCount--, curFontPtr = curFontPtr->next) {
	    tempFL->sequence_value.font_pairs[FontCount-1].font_id = curFontPtr->ODAFontID;
	    tempFL->sequence_value.font_pairs[FontCount-1].font_reference = curFontPtr->ODAFontInfo;
	}
	if (gD->doDebugging) {
	    printf("About to set font list attribute\n");
	}
	odalib_SetAttr(gD->documentProfile, dp_FONTS_LIST, (POINTER_type) tempFL, Ignore_Parms);
	if (gD->doDebugging) {
	    printf("Finished setting font list attribute\n");
	}
	odalib_DeleteSequence(tempFL);
    }

    return rval;

}




void ConvertFile(sourceFile,sourceName,outputName)
FILE * sourceFile;
char * sourceName;
char * outputName; {
    FILE * ofile;	    /* FRB for output file (if not stdout) */
    FILE *fopen();

    int objectID;
    struct dataobject *mainDataObject;
    struct view *mainView;
    char *objectName,*viewName;
    enum view_DescriberErrs describeStatus;
    char * readBackName;
    struct odaatkapp * readBackAppl;


    if (useGDB) kill(getpid(),1);

    globalODAData = atkoda_New();
    globalODAData->doDebugging = globalDebug;
    globalODAData->ScaleRaster = rasterScaling;
    globalODAData->doFudgeMargins = doFudgeMargins;
    mainDataObject = (struct dataobject *) NULL;
    mainView = (struct view *) NULL;
    objectName = viewName = (char *) NULL;
    

    objectName = (char *) filetype_Lookup(sourceFile,sourceName, &objectID, NULL);
    if(objectName == NULL) {
	mainDataObject = (struct dataobject *) text_New();
    }
    else 
	mainDataObject = (struct dataobject *) class_NewObject(objectName);
    dataobject_Read(mainDataObject,sourceFile,objectID);
    viewName = dataobject_ViewName(mainDataObject);
    if(viewName == NULL) {
	mainView = (struct view *) textview_New();
    }
    else {
	mainView =(struct view *) class_NewObject(viewName);
    }
    view_SetDataObject(mainView,mainDataObject);
    /* create output file file */
    if(outputName){
	if((ofile = fopen(outputName,"w")) == NULL){
	    fprintf(stdout,"can't open %s for output\n",outputName);
	    exit(1);
	}
    }
    else {
	ofile = stdout;
	outputName = "stdout";
    }

    if (InitGlobal(globalODAData,sourceName, outputName, ofile, mainDataObject, mainView)){
	fprintf(stdout,"Cannot initialize global ODA data\n");
	return;
    }

    describeStatus = view_Describe(mainView,"oda",ofile,globalODAData);
    if (describeStatus != view_NoDescribeError){
	fprintf(stdout,"Cannot perform description %d\n",describeStatus);
    }

    /* Finish building the document profile */
    FinishDocumentProfile(globalODAData);

    /* Label the final document so we can read the debugging output */
    odalib_LabelDocument(globalODAData->outputDocument);

    fflush(stdout);
    fflush(ofile);

    if (doODADump) {
	if (ERROR_INT == 
	    odalib_WriteDocumentState(fileno(stdout), globalODAData->outputDocument)){
	    fprintf(stdout,"Cannot write out document state\n");
	}
	fflush(stdout);
    }

    if (globalODAData->doDebugging) {
	printf("About to write out ODIF\n");
    }
    if(ERROR_INT == odalib_WriteODIF(fileno(ofile), globalODAData->outputDocument, dp_INTERCHANGE_FMT_CLASS_a)) {
	fprintf(stderr, "Cannot write out ODIF (%d)\n",odalib_TKError());
    }
    odalib_LOCAL_WriteBytes(fileno(ofile), (POINTER_type) NULL, (INT_type) -1);

    /* fflush(ofile); */

    if (globalODAData->doDebugging) {
	printf("Finished writing out ODIF\n");
    }
    if (doReadBack) {
	readBackName = (char *) malloc(strlen(outputName)+strlen(".tran") + 1);
	strcpy(readBackName,outputName);
	strcat(readBackName,".tran");
	if (globalODAData->doDebugging) {
	    printf("About to create no odaatkapp object\n");
	}
	readBackAppl = odaatkapp_New();
	if (globalODAData->doDebugging) {
	    printf("Created odaatkap object\n");
	}
	if (readBackAppl) {
	    odaatkapp_DoInCoreDoc(readBackAppl, sourceName, readBackName, globalODAData->outputDocument, globalODAData->doDebugging);
	}
	else {
	    printf("Could not create read-back odaatkapp object \n");
	}
	free(readBackName);
	odaatkapp_Destroy(readBackAppl);
    }

    if (ERROR_INT == odalib_FinalToolKit()){
	fprintf(stdout,"Cannot finalize the toolkit\n");
    }
    if (LCL_ERROR_INT == odalib_LOCAL_Final()){
	fprintf(stdout,"Cannot finalize local\n");
    }

    fflush(ofile);
    if(ofile != stdout){
	fclose(ofile);
	ofile = stdout;
    }

    /* Only close AFTER delete when we did the locking */
    fclose(sourceFile);
    view_Destroy(mainView);
    dataobject_Destroy(mainDataObject);
}

boolean atkodaapp__ParseArgs (self,argc, argv)
struct atkodaapp *self;
    int argc;
    char **argv;
/* Since we are non-interactive, everything is done in this function so we
can handle the args as they come */
    {register int i;
    char *outputName;
    FILE *sourceFile,*fopen();



    /* initialize as if ez */
    ((struct application * )self)->name = "ez";
    super_ReadInitFile(self);
    ((struct application * )self)->name = "atkoda";

    outputName = NULL;
    sourceFile = NULL;
    progname = argv[0];
 
    for (i=1;i<argc;i++){
	if (argv[i][0] == '-'){
	    	switch(argv[i][1]){
		case 'O':
		case 'o':
    		        if(argv[i][2] != '\0')
				outputName = &argv[i][2];
			else if(++i  < argc){
				outputName = argv[i];
				}
			else {
				usage();
				}
			break;
		case 's':
			/* Read document from stdin */
			sourceFile = stdin;
			break;
		case 'n': /* turn off dumping of toolkit debugging output */
		        doODADump = 1;
		        break;
		case 'd':
		        /* turn on whatever debugging is appropriate */
		        globalDebug = 1;
		        break;
		case 'r':
		        /* automatically try to read it back in */
		        doReadBack = 1;
		        break;
	        case 'e':
		        /* have tool kit print warnings as well */
		        printWarnings = BOOL_true;
		        break;
		case 'g':
		        /* Drop into debugger */
		        useGDB = 1;
		        break;
		case 'x':
		        /* dont expand (scale) raster */
		        rasterScaling = !rasterScaling;
		        break;
		case 'w':
		        /* Use stdout for error messages instead of stderr */
		        useStdOut = 1;
		        break;
		case '1':
		        /* Assume one cr in paragraph break */
		        NumCRsInParaBreak = 1;
		        break;
		case '2':
		        /* Assume two crs in paragraph break */
		        NumCRsInParaBreak = 2;
		        break;
		case 'f':
		        /* Fudge Margins when preparing output */
		        doFudgeMargins = BOOL_true;
		        break;
		default:
			 printf("bad flag %s\n", argv[i]);
			break;
		}
	    }
	else {
	    /* See if we have a request from standard in first, and if so, process it before getting to the files */
	    if(sourceFile == stdin) {
		ConvertFile(sourceFile,/* source name*/ "stdin",outputName);
	    }
	    /* Nope, not from stdin, so go figure out what kind of file it is */
	    if((sourceFile = fopen(argv[i],"r") ) == NULL){
		fprintf(stdout,"Can't open %s\n",argv[i]);
		continue;
	    }
	    ConvertFile(sourceFile, argv[i], outputName);

	}
    }
    return TRUE;
}

atkodaapp__Run(self)
struct atkodaapp *self;
{   /* we are already done */
    exit(0);
}
boolean atkodaapp__InitializeObject(classID,self)
struct classheader *classID;
struct atkodaapp *self;
{

    atkodaapp_SetMajorVersion(self,2);
    atkodaapp_SetMinorVersion(self,1);

    return TRUE;

}

usage(){
	fprintf(stdout,"usage: %s  <-stdin> < -o OutputFileName> file\n",progname);
	exit(2);
	}
