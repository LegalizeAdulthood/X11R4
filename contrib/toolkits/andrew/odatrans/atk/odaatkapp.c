/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/odaatkapp.c,v 1.4 89/09/08 18:39:55 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/odaatkapp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/odaatkapp.c,v 1.4 89/09/08 18:39:55 mss Exp $";
#endif

/*
  $Log:	odaatkapp.c,v $
 * Revision 1.4  89/09/08  18:39:55  mss
 * Added localal finalizeation
 * 
 * Revision 1.3  89/09/08  17:30:14  mss
 * /tmp/msg
 * 
 * Revision 1.2  89/09/05  15:05:32  mss
 * Added explicit local initialization
 * 
 * Revision 1.1  89/08/30  16:14:02  mss
 * Initial revision
 * 
 * Revision 1.26  89/08/30  16:10:55  mss
 * Removed oasis references
 * 
 * Revision 1.25  89/08/26  00:39:06  mss
 * Got rid of some unnecessary incldes
 * 
 * Revision 1.24  89/07/21  10:59:32  mss
 * added warning switches (-e)
 * 
 * Revision 1.23  89/05/20  20:44:22  mss
 * Got rid of tester code
 * 
 * Revision 1.22  89/05/04  22:22:04  mss
 * Added version numbers (but still does not work)
 * 
 * Revision 1.21  89/04/30  12:11:32  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.20  89/01/10  13:52:41  mss
 * Changed call to FindDocumentRoot to match new interface
 * 
 * Revision 1.19  88/12/14  10:50:46  mss
 * Changed the use of NULL_CONSTITUENT constant to
 * odalib_NULL_CONSTITUENT() class procedure
 * 
 * Revision 1.18  88/12/13  20:53:58  mss
 * Changed NULL_SEQUENCE to use odalib class
 * procedure instead
 * 
 * Revision 1.17  88/11/29  14:34:37  mss
 * Hacked up rasters so that the appearance of an offset in a specific
 * raster will force centering
 * 
 * Revision 1.16  88/11/18  16:03:59  mss
 * Start making calls on the routines
 * for manpulating style application and
 * compensation
 * 
 * Revision 1.15  88/11/17  17:31:03  mss
 * Added doFudgeMargins flag to selectively turn on and
 * off margin hacking
 * 
 * Revision 1.14  88/11/17  11:43:02  mss
 * Changed default setting of raster scaling switch to be
 * "no scaling" in both directions; also changed scaling
 * suppression messages to appear only when debugging
 * is turned on.
 * 
 * Revision 1.13  88/11/08  15:17:50  mss
 * Added global state for turning off raster scaling
 * 
 * Revision 1.12  88/11/08  14:56:45  mss
 * Changed global style analysis to try to best guess
 * how to handle global margins. Added flags for raster
 * scaling.
 * 
 * Revision 1.11  88/11/04  17:05:46  mss
 * Fixed null pointer bug in debugging output for fonts list
 * 
 * Revision 1.10  88/11/02  16:59:47  mss
 * Added extra <cr> around insets to perserve style information
 * 
 * Revision 1.9  88/10/31  21:32:51  mss
 * More state for global SV and fonts
 * 
 * Revision 1.8  88/10/18  14:05:12  mss
 * Added beginnings of profile calculations
 * 
 * Revision 1.7  88/10/11  21:59:50  mss
 * Changed name of ckcontenttype back
 * 
 * Revision 1.6  88/10/10  18:53:53  mss
 * revised tracing
 * 
 * Revision 1.5  88/10/01  17:11:08  mss
 * Updated to new atk
 * 
 * Revision 1.4  88/09/23  11:24:27  mss
 * Reversed statement/declarations error
 * 
 * Revision 1.3  88/09/22  11:10:54  mss
 * Global state used, added round tripping
 * 
 * Revision 1.2  88/09/07  11:05:10  mss
 * Substantial changes -- initial processing of styles
 * 
 * Revision 1.1  88/08/05  17:04:23  mss
 * Initial revision
 *  

*/

/*
*
*	Modified from bx and be1 ezprint code.
*
*
*
*/
#include <class.h>
#include <odaatkapp.eh>

#include <im.ih>
#include <text.ih>
#include <textv.ih>
#include <textoda.ih>
#include <view.ih>
#include <txtstvec.h>
#include <dataobj.ih>
#include <style.ih>
#include <stylesht.ih>
#include <filetype.ih>
#include <nstdmark.ih>
#include <envrment.ih>

#include <odatk.h>
#include <odalib.ih>
#include <odaatk.h>
#include <ckcontent.h>
#include <odadef.h>
#include <readtext.h>
#include <readraster.h>
#include <readtable.h>
#include <readeq.h>
#include <getcompsv.h>

#include <errormsg.h>
#include <getstyles.h>

/* missing in RT version */
extern char * strrchr();
extern char * ErrorMsg();

#define emsg() ErrorMsg(odalib_TKError())

#define tracing 1


extern int errno;

static int debugflag = 1;
static int useGDB = 0;
static BOOL_type printWarnings = BOOL_false;
static BOOL_type doFudgeMargins = BOOL_false;
static odainfo_type globalODAData;

char *progname;







void InitStyleTable(gD)
p_odainfo_type gD;
{

    if (gD->doDebugging) {
        printf("Entering InitStyleTable\n");
    }
    /* Empty for now, will be filled in later */

    GetGlobalStyle(gD);
    GetGenericStyles(gD);

    if (gD->doDebugging) {
        printf("Leaving InitStyleTable\n");
    }

}


INT_type MaybeGetAttr(curConstituent, attribute, attributevalue, inherited, parmmask)
CONSTITUENT curConstituent;
INT_type attribute;
POINTER_type attributevalue;
BOOL_type inherited;
PARM_MASK_type *parmmask; {

    INT_type	    rval;

    if (globalODAData.doDebugging) {
        printf("Entering MaybeGetAttr\n");
    }

    if (NO_ATTR == odalib_HasAttr(curConstituent, attribute)) {
	rval = ERROR_INT;
    }
    else {
	rval = SafeGetAttr(curConstituent, attribute, attributevalue, inherited, parmmask);
    }

    if (globalODAData.doDebugging) {
        printf("Leaving MaybeGetAttr\n");
    }

    return rval;
}

INT_type GetProfileInfo(gD)
p_odainfo_type gD;
{
    INT_type		    rval;
    dp_UNIT_SCALING_type    *curScaling;
    if (gD->doDebugging) {
        printf("Entering GetProfileInfo\n");
    }

    rval = SUCCESS_INT;

    gD->documentProfile = odalib_FindDocumentProfile(gD->odaDocument);
    if (gD->documentProfile == odalib_ERROR_CONSTITUENT()) {
	printf("Missing document profile, creating a dummy one\n");
	gD->documentProfile = odalib_MakeDocumentProfile(gD->odaDocument);
	if (odalib_ERROR_CONSTITUENT() == gD->documentProfile) {
	    printf("Could not create a dummy document profile \n");
	    goto LEAVE;
	}
    }

    if (HAS_ATTR == odalib_HasAttr(gD->documentProfile, dp_UNIT_SCALING)) {
	odalib_GetAttr(gD->documentProfile, dp_UNIT_SCALING, &curScaling, Use_Local_Value, Ignore_Parms);
	gD->SMU_Numerator = curScaling->first;
	gD->SMU_Denominator = curScaling->second;
	free(curScaling);
    }
    else {
	gD->SMU_Numerator = 1;
	gD->SMU_Denominator = 1;
    }

    if (HAS_ATTR == odalib_HasAttr(gD->documentProfile, dp_FONTS_LIST)) {
	if(gD->doDebugging) {
	    printf("dp fonts list attribute exist -- about to get it\n");
	}
	odalib_GetAttr(gD->documentProfile, dp_FONTS_LIST, &gD->fonts, Use_Local_Value, Ignore_Parms);
	if(gD->doDebugging) {
	    printf("Got fonts list attribute from document profile with %d fonts\n",
		   gD->fonts->length);
	}
    }
    else {
	if(gD->doDebugging) {
	    printf("Could not get fonts list attribute from document profile\n");
	}
	gD->fonts = odalib_NULL_SEQUENCE();
    }

    if (gD->doDebugging) {
        printf("Leaving GetProfileInfo\n");
    }

LEAVE:
    return rval;

}

static struct style  * dummyCenterStyle = NULL;

struct style * DummyCtr(gD)
p_odainfo_type gD;

{
    if (!dummyCenterStyle) {
	dummyCenterStyle  = style_New();
	style_SetName(dummyCenterStyle, "Imposed-Center");
	stylesheet_Add(gD->topLevelDocument->styleSheet, dummyCenterStyle);
	style_SetMenuName(dummyCenterStyle,"Document,Imposed-Center");
	style_SetJustification(dummyCenterStyle, style_Centered);
    }
    return dummyCenterStyle;

}

void WalkSpecificLogical(gD, specConstituent, startPosition, curSV)
p_odainfo_type gD;
CONSTITUENT specConstituent;
long * startPosition;	    /* Position where this content starts */
struct text_statevector * curSV;   /* Current value of environment that contains this subtree */
{

    VALUE_type			curConType;	/* Type of the specific Constituent */
    SEQUENCE_CONSTITUENT_type	subordinates; /* subordinates of the composite */
    SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
    INT_type			i;		/* counter */
    INT_type			contentType, contentMode;   /* What kind of content in specific */
    CONSTITUENT			underlyingGeneric;	/* generic for this specific */
    INT_type			contentStart, contentEnd;   /* what characters have been added */
    struct style *		styleToUse;		    /* ATK style for this object */
    struct style		* compensatingStyle;	    /* Style needed for compensation*/
    CONSTITUENT			dummyLS;
    struct style		*hackStyle;		    /* just for demo */
    long			childStartPosition;	    /* Where child started inserting chars */
    struct text_statevector	styleOnlySV;		    /* State vector used here */
    struct text_statevector	allSV;			    /* State vector from styles and local modifications */
    struct text_statevector	*compensatedSV;		    /* Final SV */

    if (gD->doDebugging) {
        printf("Entering WalkSpecificLogical %X, %X, %d\n", gD, specConstituent, *startPosition);
    }

    if (ERROR_INT == odalib_GetAttr(specConstituent, at_OBJECT_TYPE, (POINTER_type) &curConType, Use_Local_Value, Ignore_Parms) ){
	printf("Missing object type on constituent(%s)\n", emsg());
	goto LEAVE;
    }

    contentStart =  text_GetLength(gD->topLevelDocument);
    if (gD->doDebugging) {
        printf("Current textlength (initial content start) %d\n", contentStart);
    }

    /* Find out if a style is attached */
    if (SUCCESS_INT != CheckAndGetAttr(specConstituent, at_OBJECT_CLASS, (POINTER_type) &underlyingGeneric, Ignore_Parms)) {
	printf("Specific logical object missing generic\n");
	styleToUse = (struct style *) NULL;
    }
    else {
	styleToUse = FindATKStyleFromConst(gD, underlyingGeneric);
	if (gD->doDebugging) {
	    printf("Found a style to use %X\n", styleToUse);
	}
    }

    hackStyle = (struct style *) NULL;

    /* Check for any kind of compensations because of specific overrides of the generic : note this current wrong and needs to be filled in properly */
    compensatedSV = GetCompensatedSV(gD, specConstituent, curSV, styleToUse, &compensatingStyle);

    switch(curConType) {
	    case at_OBJECT_TYPE_doc_logical_root:
	    case at_OBJECT_TYPE_comp_logical_obj:
		/* Look for subordinates */
		if (HAS_ATTR == odalib_HasAttr(specConstituent, at_SUBORDINATES)) {
		    odalib_GetAttr(specConstituent, at_SUBORDINATES, (POINTER_type)&subordinates, Use_Local_Value, Ignore_Parms);
		}
		else {
		    if (gD->doDebugging) {
			printf("Unusual tree with a composite that has no subordinates\n");
		    }
		    subordinates = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag,0);
		}
		childStartPosition = -31;
		for (i=0; i< subordinates->length; i++ ){
		    WalkSpecificLogical(gD, subordinates->sequence_value.constituents[i], &childStartPosition, compensatedSV);
		    if (i==0) {
			contentStart = childStartPosition;
		    }
		}
		*startPosition = contentStart;
		if (ERROR_INT == odalib_DeleteSequence(subordinates)) {
		    printf("Cannot delete sequence for subordinates (%s)\n", emsg());
		}
		break;

	    case at_OBJECT_TYPE_bas_logical_obj:
		/* Look for content portions, so pull them off and process */
		if (ERROR_INT == SafeGetAttr(specConstituent, at_CONTENT_PORTIONS, (POINTER_type) &contents, Use_Local_Value, Ignore_Parms)){
		    printf("Missing content portion on basic logical object\n");
		    return;
		}
		GetContentType(gD, specConstituent, &contentType, &contentMode);
		/* Everything but text requires a special paragraph break around it -- so add cr's to separate the inset */
		if (contentType != CharContent) {
		    text_AlwaysInsertCharacters(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), "\n", (long) 1);
		    gD->LastInsertWasFigure = BOOL_true;
		}
		/* Figure out which architecture class and handle appropriately */
		switch(contentType) {
		    case CharContent:
			ProcessCharCP(gD, specConstituent, contents, contentType, contentMode, startPosition, compensatedSV);
			contentStart = *startPosition;
			break;
		    case RasterContent:
			/* Check for style? */
			ProcessRasterCP(gD, specConstituent, contents, contentType, contentMode, startPosition, compensatedSV);
		        contentStart = *startPosition;
			break;
		    case GeoContent:
			printf("Neither rasters nor geometric graphics processed yet\n");
			break;
		    case TableContent:
			ProcessTableCP(gD, specConstituent, contents, contentType, contentMode, startPosition, compensatedSV);
			break;
		    case EquationContent:
			ProcessEqCP(gD, specConstituent, contents, contentType, contentMode, startPosition, compensatedSV);
			break;
		    default:
			printf("Unknown content type %d\n", contentType);
			break;
		}
		/* Everything but text requires a special paragraph break around it -- so add cr's to separate the inset */
		if (contentType != CharContent) {
		    text_AlwaysInsertCharacters(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), "\n", (long) 1);
		    hackStyle = (struct style *) NULL;
		    if ((HAS_ATTR == odalib_HasAttr(specConstituent, at_LAYOUT_STYLE))){
			odalib_GetAttr(specConstituent, at_LAYOUT_STYLE, &dummyLS, Use_Local_Value, (PARM_MASK_type *) 0);
			if ((HAS_ATTR == odalib_HasAttr(dummyLS, at_OFFSET))) {
			    /* Hack, assume its just a centering style */
			    hackStyle = DummyCtr(gD);
			}
		    }

		}

		if (ERROR_INT == odalib_DeleteSequence(contents)) {
		    printf("Cannot delete sequence for content portions (%s)\n", emsg());
		}
		break;

	    default:
		printf("Unknown object type %d\n", curConType);
		break;
    }

    contentEnd =  text_GetLength(gD->topLevelDocument);

    if (hackStyle) {
	DoStyleApplication(gD, contentStart, contentEnd-contentStart, hackStyle);
    }

    if (styleToUse != (struct style *) NULL) {
	/* We had a style, so apply it now */
	if (gD->doDebugging) {
	    printf("Adding style from %d to %d for %d characters\n", contentStart, contentEnd, contentEnd-contentStart);
	}
	if (contentStart != contentEnd) {
	    if (compensatingStyle != NULL) {
		DoStyleApplication(gD, contentStart, contentEnd-contentStart, compensatingStyle);
	    }
	    DoStyleApplication(gD, contentStart, contentEnd-contentStart, styleToUse);
	}
        else {
	    if (gD->doDebugging) {
	        printf("Supressing style application over no content\n");
	    }
	}
    }

LEAVE:

    if (gD->doDebugging) {
        printf("Leaving WalkSpecificLogical begin %d, end %d\n", contentStart, contentEnd);
    }


}

void TranslateODA(gD)
p_odainfo_type gD;
{
    long startChar;
    enum style_MarginValue  marginBasis;    /* For patching global styles */
    enum style_Unit	    marginUnit;

    if (gD->doDebugging) {
        printf("Entering TranslateODA %X\n", gD);
    }

    /* Pull out information from document */
    gD->specificLogicalRoot = odalib_FindDocumentRoot(gD->odaDocument, SPECIFIC_DOC_LOGICAL_ROOT);
    if (odalib_ERROR_CONSTITUENT() == gD->specificLogicalRoot) {
	printf("Cannot find specific document root (%s)\n", emsg());
	return;
    }
    gD->genericLogicalRoot = odalib_FindDocumentRoot(gD->odaDocument, GENERIC_DOC_LOGICAL_ROOT);
    if (odalib_ERROR_CONSTITUENT() == gD->genericLogicalRoot) {
	printf("Cannot find specific document root (%s)\n", emsg());
	return;
    }

    /* Get necessary information from document profile */
    GetProfileInfo(gD);

    /* Initialize various association tables -- note side effect of extra space in front */
    InitStyleTable(gD);

    /* Perform tree walk over specific logical to get information */
    startChar = -29;
    WalkSpecificLogical(gD, gD->specificLogicalRoot, &startChar, &gD->globalSV);
    FlushStyleList(gD);

    /* Patch global style to compensate for bizarre Andrew redisplay semantics */
    /*marginBasis = style_PreviousMargin; */
    if (doFudgeMargins) {
	marginUnit = style_Inches;
	style_SetNewLeftMargin(gD->globalStyle, style_LeftMargin, 0, style_Points);
	style_SetNewRightMargin(gD->globalStyle, style_RightMargin, 0, style_Points);
	style_SetNewIndentation(gD->globalStyle, style_PreviousIndentation, 0, style_Points);
    }
    /* And add global style once around entire object to make sure that it is used! */
    text_AlwaysAddStyle(gD->topLevelDocument, 0, text_GetLength(gD->topLevelDocument), gD->globalStyle);


    if (gD->doDebugging) {
        printf("Leaving TranslateODA\n");
    }

}

void odaatkapp__ReadInitFile(self)
struct odaatkapp *self;
{
    /* Maybe we should have some preference processing here */
}

long InitFileNames(gD,sourceName, outputName, ofile)
p_odainfo_type gD;
char * sourceName;
char * outputName;
FILE * ofile;
{

    if (useGDB) kill(getpid(),1);

    gD->inputFileName = sourceName;
    gD->outputFileName = outputName;
    gD->outputFileDescriptor = ofile;
    if ( (printWarnings != BOOL_false) || gD->doDebugging) {
	globalODAData.ODAParams.print_warnings = BOOL_true;
    }
    else
	globalODAData.ODAParams.print_warnings = BOOL_false;

    if (LCL_ERROR_INT == odalib_LOCAL_Init()){
	fprintf(stdout,"Cannot initialized the local interface\n");
	return -1;
    }

    if (ERROR_INT == odalib_InitToolKit(&gD->ODAParams)) {
	fprintf(stdout,"Cannot initialized the toolkit \n");
	return -1;
    }

    return 0;
}

static BOOL_type scaleRasters = BOOL_false;

void InitGlobalODAData(gD)
p_odainfo_type gD; {
    gD->initialBLO = odalib_NULL_CONSTITUENT();
    gD->initialBLOCFonts = (cc_CHARACTER_FONTS_type) NULL;
    gD->firstParagraphInDocument = BOOL_true;
    gD->ODAParams.automatic_label = BOOL_true;
    gD->ODAParams.error_od = fileno(stderr);
    gD->ODAParams.format_errors = BOOL_true;
    gD->ODAParams.output_filter = NULL_FILTER_ROUTINE;
    gD->topLevelDocument = text_New();
    gD->topLevelView = (struct view*) textview_New();
    textview_SetDataObject((struct textview *)(gD->topLevelView), gD->topLevelDocument);
    gD->LastInsertWasFigure = BOOL_false;
    gD->ScaleRasters = scaleRasters;
}


void ConvertFile(sourceFile,sourceName,outputName,useTester, dumpODADebug)
FILE * sourceFile;
char * sourceName;
char * outputName; 
int useTester;
int dumpODADebug;
{
    FILE * ofile;	    /* FRB for output file (if not stdout) */
    FILE *fopen();

    char * dotPtr;
    int i;
/*    struct view *mainView;
    char *objectName,*viewName; */


    InitGlobalODAData(&globalODAData);

    /* Figure out where the odif should go */
    if ( (sourceFile == stdin) && (outputName == NULL)) {
	/* If no input name and no output name, use stdout */
	outputName = "stdout";
	ofile = stdout;
    }
    else if (outputName == NULL) {

	/* make enough space for worst case name */

	outputName = (char *) malloc(strlen(sourceName) + /* for null */ 1 + /* for extension */ strlen(".d"));
	(void) strcpy(outputName, sourceName);

	/* If still no output name, create one from input name, either by appending .d or by replacing extension with .d */
	/* see if extension is .d, if so ignore. Note: this next test is sleazy, since I assume that there will be something after the dot (if nothing else, the null terminating the string), so I can be sure not to go too far */
	dotPtr = strrchr(sourceFile,'.');
	if ((dotPtr == NULL) || ((*(dotPtr + 1)) == 'd')) {
	    /* no dot present, ".d" present, so just add (another) ".d" */
	    (void) strcat(outputName, ".d");
	}
	else {
	    /* Has an extension, so replace it */
	       *(++dotPtr) = 'd';
	       *(++dotPtr) = 0;
	    }
    }

    /* Open the output file as necessary */
    if (ofile != stdout) {
	if((ofile = fopen(outputName,"w")) == NULL){
	    fprintf(stdout,"can't open %s for output\n",outputName);
	    exit(1);
	}
    }

    /* Fire up the toolkit */
    if (InitFileNames(&globalODAData,sourceName, outputName, ofile)){
	fprintf(stdout,"Cannot initialize global ODA data\n");
	return;
    }

/*    kill(getpid(), 1); */

    /* Read in the document */
	if((sourceFile = fopen(sourceName,"r") ) == NULL){
	    fprintf(stdout,"Can't open %s\n",sourceName);
	    return;
	}
	if (globalODAData.doDebugging) {
	    fprintf(stdout, "about to read odif\n");
	}
	globalODAData.odaDocument = odalib_ReadODIF(fileno(sourceFile));
	if (globalODAData.doDebugging) {
	    fprintf(stdout, "finished readin odif\n");
	}
	fflush(stdout);
	fclose(sourceFile);
	if (globalODAData.odaDocument == odalib_ERROR_DOCUMENT()) {
	    fprintf(stdout,"Could not read ODIF file %s because %s\n", sourceName, emsg());
	    return;
	}
    

    if (dumpODADebug){
	if (ERROR_INT == odalib_WriteDocumentState(fileno(stdout),globalODAData.odaDocument)) {
	    printf("\nError providing debugging output for ODA document: %s\n", emsg());
	}
    }

    /* perform the translation */
    TranslateODA(&globalODAData);

    /* Write out the translated version */

    if (globalODAData.doDebugging) {
	printf("About to write out file\n");
    }
    text_Write(globalODAData.topLevelDocument, ofile, text_GetWriteID(globalODAData.topLevelDocument) + 1, 0);
    if (globalODAData.doDebugging) {
	printf("Finihsed writing file\n");
    }

    /* shut down the toolkit */

    fflush(stdout);
    fflush(ofile);

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


    if (globalODAData.doDebugging) {
	printf("About to destroy object\n");
    }
    text_Destroy(globalODAData.topLevelDocument);
    if (globalODAData.doDebugging) {
	printf("Destoryed text object\n");
    }

}

boolean odaatkapp__ParseArgs (self,argc, argv)
struct odaatkapp *self;
    int argc;
    char **argv;
{
/* Since we are non-interactive, everything is done in this function so we
can handle the args as they come */

    register int i;
    char *outputName;
    FILE *sourceFile,*fopen();
    int useTester;
    int dumpODADebug;

    /* initialize as if ez */
    ((struct application * )self)->name = "ez";
    super_ReadInitFile(self);
    ((struct application * )self)->name = "odaatk";

    outputName = NULL;
    sourceFile = NULL;
    progname = argv[0];
    useTester = FALSE;
    dumpODADebug = FALSE;
 
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
		case 'd':
		        /* turn on whatever debugging is appropriate */
		        globalODAData.doDebugging = BOOL_true;
		        break;
		case 'e':
			/* turn on warning messages */
			printWarnings = BOOL_true;
			break;
		case 'g':
		        /* Turn gdb break */
		        useGDB = 1;
		        break;
		case 't':
		        /* turn on tester usage */
		        useTester = TRUE;
		        break;
		case 'w':
		        /* dump out debugging output after reading file */
		        dumpODADebug = TRUE;
		        break;
		case 'x':
		        /* turn off scaling of rasters */
		        scaleRasters = !scaleRasters;
		        break;
		case 'f':
		        /* fudge the margins */
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
		ConvertFile(sourceFile,/* source name*/ "stdin",outputName, useTester, dumpODADebug);
	    }
	    else {
		sourceFile = NULL;
		ConvertFile(sourceFile, argv[i], outputName, useTester, dumpODADebug);
	    }
	}
    }
    return TRUE;
}

boolean odaatkapp__DoInCoreDoc(self, inputName, outputName, InputDoc, doDebugging)
struct odaatkapp * self;
char * inputName;
char * outputName;
DOCUMENT_type InputDoc;
BOOL_type doDebugging; {
    /* This procedure is used as a fake way to read back in a translated document */
    FILE * ofile;	    /* FRB for output file (if not stdout) */
    FILE *fopen();
    

    
    if (doDebugging) {
        printf("Entering DoInCoreDoc\n");
    }

    globalODAData.doDebugging = doDebugging;

    InitGlobalODAData(&globalODAData);

    ((struct application * )self)->name = "ez";
    super_ReadInitFile(self);
    ((struct application * )self)->name = "odaatk";
    if (debugflag) {
        printf("Finished reading init file\n");
    }

    progname = (char *)malloc(strlen("readback")+1);
    strcpy(progname,"readback");

    globalODAData.inputFileName = inputName;
    globalODAData.outputFileName = outputName;
    globalODAData.odaDocument = InputDoc;
    if (! outputName) {
	globalODAData.outputFileDescriptor = stdout;
    }
    else {
	if((ofile = fopen(outputName,"w")) == NULL){
	    fprintf(stdout,"can't open %s for output\n",outputName);
	    exit(1);
	}
	globalODAData.outputFileDescriptor = ofile;
    }


    /* None of this should matter, since the toolkit is already initialized elsewhere */
    globalODAData.ODAParams.automatic_label = BOOL_true;
    globalODAData.ODAParams.error_od = fileno(stderr);
    globalODAData.ODAParams.format_errors = BOOL_true;
    if ( (printWarnings != BOOL_false) || doDebugging) {
	globalODAData.ODAParams.print_warnings = BOOL_true;
    }
    else
	globalODAData.ODAParams.print_warnings = BOOL_false;
    globalODAData.ODAParams.output_filter = NULL_FILTER_ROUTINE;
    globalODAData.ODAParams.output_filter = NULL;

    /* perform the translation */
    TranslateODA(&globalODAData);

    /* Write out the translated version */
    printf("About to write out file\n");
    text_Write(globalODAData.topLevelDocument, ofile, text_GetWriteID(globalODAData.topLevelDocument) + 1, 0);
    printf("Finished writing file\n");

    /* shut down the toolkit */

    fflush(stdout);
    fflush(ofile);

    if(ofile != stdout){
	fclose(ofile);
	ofile = stdout;
    }

    printf("About to destory text object\n");
    text_Destroy(globalODAData.topLevelDocument);
    printf("Finished destroying text object\n");

    return TRUE;

}

odaatkapp__Run(self)
struct odaatkapp *self;
{   /* we are already done */
    exit(0);
}
boolean odaatkapp__InitializeObject(classID,self)
struct classheader *classID;
struct odaatkapp *self;
{
    odaatkapp_SetMajorVersion(self,2);
    odaatkapp_SetMinorVersion(self,1);
    return TRUE;

}

usage(){
	fprintf(stdout,"usage: %s  <-stdin> < -o OutputFileName> file\n",progname);
	exit(2);
	}
