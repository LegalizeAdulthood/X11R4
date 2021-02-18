/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/textoda.c,v 1.3 89/09/13 09:41:59 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/textoda.c,v $ */

#ifndef lint
static char *rcsidtextodac = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/textoda.c,v 1.3 89/09/13 09:41:59 mss Exp $";
#endif

 /* Bugs 
	Does not handle hyphenation properly - will have to be fixed when scanning the profile routines are put into camlib.

	Fixed
	uses \s<SIZE> 
	handles need new line right
	changes \ to \\
	sub and superscripts
	Passthru not handled (should set needNewLine )
 */
 
#define debugflag 0
#define styleflag 0

#include <class.h>

#include <text.ih>
#include <fontdesc.ih>
#include <environ.ih>
#include <dict.ih>
#include <viewref.ih>
#include <view.ih>
#include <textv.ih>
#include <txtstvec.h>
#include <envrment.ih>
#include <style.ih>
#include <stylesht.ih>

#include <odatk.h>
#include <atkoda.ih>
#include <odalib.ih>
#include <styles.h>
#include <mkgeneric.h>
#include <breakup.h>
#include <textoda.eh>


/* temp */

/* forward declarations */
INT_type CreateODAGenericStructrue(/*gD*/);
BOOL_type CreateOneStyle(/* curStyle, gD*/);
void CheckCRs(/*gD, d, pos*/);

#define MaxStack 100
typedef struct {
	long cStackTop;		/* Points to last used element */
	CONSTITUENT constituentStack[MaxStack];
	struct environment *(environStack[MaxStack]);
	struct text_statevector sv[MaxStack];
    } CLOStack_type;



FILE * troffFile;

static int needNewLine;		/* true put out a new line if giveing command */
static long dFont;		/* desired index of name of the font */
static long dFace;		/* desired faces of the font */

static struct {
    char *fontname;
    char *fontcodes[9];
	/* fontcodes are in this order: 
			plain, italic, bold, bolditalic, 
			fixed-plain, fixed-italic, fixed-bold, fixed-bolditalic,
			shadow
		all shadowface is bold for now */ 
} fonttable[] = {
    {"timesroman", {"R", "I", "B", "BI", "CR", "CI", "CB", "CD", "B"}},
    {"helvetica", {"HR", "HI", "HB", "HD", "CR", "CI", "CB", "CD", "B"}},
    {"andy", {"R", "I", "B", "BI", "CR", "CI", "CB", "CD", "B"}},
    {"andysans", {"HR", "HI", "HB", "HD", "CR", "CI", "CB", "CD", "B"}},
    {"andytype", {"CR", "CI", "CB", "CD", "CR", "CI", "CB", "CD", "CR"}},
    {"gacha", {"CR", "CI", "CB", "CD", "CR", "CI", "CB", "CD", "CR"}},
    {0, {"R", "I", "B", "BI", "CR", "CI", "CB", "CD", "B"}}    /* default for unknown family */
}; 

static char ChangeMsg[1024];	/* This is a hack place to put information about state changes */

#define ModBits 4
#define FaceCodeBits 4


static changejustification(old, new,stackp, gData)
    enum style_Justification old, new;
    CLOStack_type * stackp;
    struct atkoda * gData;
{
    if (old != new)  {
	if (styleflag){
	 printf("Justification needs to be changed from ");
	 strcat(ChangeMsg,"New justification ");
	 switch(old) {
	    case style_Centered:
		printf("centered to ");
/*	        fputs(".ad c\n", troffFile);*/
	        break;
	    case style_LeftJustified:
		printf("left justified to ");
		break;
	    case style_LeftThenRightJustified:
		printf("left then right justified to ");
	        break;
	    case style_RightJustified:
		printf("right justified to ");
	        break;
	    case style_LeftAndRightJustified:
		printf("left and right justified to ");
	        break;
	    default:
		printf("textoda_changejustification: Unknown old justification code %d\n",old);
	        break;
	 }
	}
	if (needNewLine)  {
/*	    putc('\n', troffFile);*/
	    needNewLine = 0;
	}
/*	fputs(".br\n",troffFile);*/
	switch (new)  {
	    case style_Centered:
		if (styleflag) {
		    printf("centered\n");
		    strcat(ChangeMsg,"centered;");
		}
/*	        fputs(".ad c\n", troffFile);*/
	        break;
	    case style_LeftJustified:
		if (styleflag) {
		    printf("left justified\n");
		    strcat(ChangeMsg,"left justified;");
		}
		break;
	    case style_LeftThenRightJustified:
		if (styleflag) {
		    printf("left then right justified\n");
		    strcat(ChangeMsg,"left then right justified;");
		}
/*	        fputs(".ad l\n", troffFile);*/
	        break;
	    case style_RightJustified:
		if (styleflag) {
		    printf("right justified\n");
		    strcat(ChangeMsg,"right justfied;");
		}
/*	        fputs(".ad r\n", troffFile);*/
	        break;
	    case style_LeftAndRightJustified:
		if (styleflag){
		    printf("left and right justified\n");
		    strcat(ChangeMsg,"left and right justified;");
		}
/*	        fputs(".ad b\n", troffFile);*/
	        break;
	    default:
		printf("textoda_changejustification: Unknown new justification code %d\n",new);
	        break;
	}
    }
}

static changeTabs(sv, nsv, stackp, gData)
    CLOStack_type * stackp;
    struct atkoda * gData;
    struct text_statevector *sv, *nsv;
{
	static int firsttime = 1;

	if (styleflag && firsttime){
	    printf("Tab processing code not yet provided\n");
	    firsttime = 0;
	}
	/* See if there are any differences in the tabs stops specified */
	
}

/* Writes the document out in troff format into a file  */


long WalkTextTree(curTextObj,curTextView, gD)
    struct text *curTextObj;
    struct textview * curTextView;
    struct atkoda * gD;

{   /* Write version 11 */

	long pos, len;
    struct environment *rootenv;
    struct environment *startenv;
    struct environment *endenv;
    struct environment *curenv;
    struct environment *newenv;
    struct environment *parentenv;
    struct environment *tempenv;
    long end;
    long i;
    long elen;
    int levels;
    long envpos;

    if (gD->doDebugging) printf("Entering WalkTextTree\n");

   /* printf("Skipping over initilaation of default environ\n");*/

    pos = 0;
    len = text_GetLength(curTextObj);
    startenv = environment_GetInnerMost(curTextObj->rootEnvironment, pos);
    endenv = environment_GetInnerMost(curTextObj->rootEnvironment, pos + len - 1);
    rootenv = environment_GetCommonParent(startenv, endenv);
    
    for (envpos = environment_Eval(rootenv); pos == envpos && pos + len == envpos + rootenv->header.nestedmark.length; rootenv = (struct environment *) rootenv->header.nestedmark.parent, envpos = environment_Eval(rootenv));
    
    curenv = rootenv;
    while (curenv != startenv)  {
	curenv = environment_GetChild(curenv, pos);
	if (curenv->type == environment_Style) {
	    /*printf("Add CLO for %s/%X\n", curenv->data.style->name,curenv);*/
	    /* PushNewCLO(&curState, curenv, rock); */
	    (void) ProcessStyleEntry(gD,curTextView, curenv);
	}
	else if (curenv->type == environment_View) {
	    printf("Add CLO for nested object %X\n",curenv);
	}
    }
    i = pos;
    end = pos + len;
    while (i < end)  {
	newenv = environment_GetInnerMost(curTextObj->rootEnvironment, i);
	elen = environment_GetNextChange(curTextObj->rootEnvironment, i);
	if (elen + i > end)
	    elen = end - i;
	if (curenv != newenv)  {
	    parentenv = environment_GetCommonParent(curenv, newenv);
	    levels = environment_Distance(curenv, parentenv);
	    tempenv = curenv;
	    while (levels > 0)  {
		if (gD->doDebugging){
		    printf("end of CLO for %X\n", tempenv);
		}
		if (tempenv->type == environment_Style) {
		    /* PopCLO(&curState, rock); */
		    ProcessStyleExit(gD, curTextView, tempenv);
		}
		tempenv = (struct environment *) environment_GetParent(tempenv);
		/*putc('}', file);*/
		levels--;
	    }
	    
	    curenv = parentenv;
	    
	    if (curenv != newenv)  {
		struct environment *stack[100];
		struct environment *te;
		int level = 0;

		for (te = newenv; te != parentenv; te = (struct environment *) te->header.nestedmark.parent)
		    stack[level++] = te;
		while (level > 0)  {
		    curenv = stack[--level];
		    if (curenv->type == environment_Style) {
			if (gD->doDebugging) {
			    printf("Processing a style entry\n");
			}
			ProcessStyleEntry(gD, curTextView, curenv);
		    }
		    else if (curenv->type == environment_View)  {
			/* Pushing a CLO for embedded object */
			/*PushNewCLO(&curState, curenv, rock);*/
			break;
		    }
		}
	    }
	}
	if (curenv->type == environment_View)  {
	    /* code to write out view information */
	    ProcessInnerView(gD,curTextView, curenv);
	    i += curenv->header.nestedmark.length;
	    elen = 0;
	}

	elen += i;
	ProcessTextRun(gD,curTextObj,i,elen, curenv);
	i = elen;
    }
    
    levels = environment_Distance(curenv, rootenv);
    tempenv = curenv;
    while (levels-- > 0) {
	if (gD->doDebugging) printf("closing environment %X\n", tempenv);
	if (tempenv->type == environment_Style) {
	    /* PopCLO(&curState, rock); */
	    ProcessStyleExit(gD, curTextView, tempenv);
	}
	tempenv = (struct environment *) environment_GetParent(tempenv);
    }
    return 0;
}

enum view_DescriberErrs textoda__Describe(descObject,viewToDescribe,format,file,gD)
struct textoda * descObject;
struct textview * viewToDescribe;
char * format;
FILE * file;
struct atkoda * gD; {

    register struct text *d;
    struct environment *cenv;

    if (gD->doDebugging){
	printf("Entering textoda\n");
    }

    d = (struct text *) viewToDescribe->header.view.dataobject;
    if (gD->doDebugging) {
       printf("Current global style is %X\n", gD->globalStyle);
    }
    if(gD->globalStyle == NULL){
	if( initializestyle(gD) == -1) {
	    printf("Could not initialize global style/environment\n");
	    return;
	}
    }

    if (gD->doDebugging){
	printf("About to get root environment\n");
    }

    cenv = d->rootEnvironment;		/* initial environment */
    if (gD->doDebugging) {
	printf("RootEnv = %X, Data Document = %X\n", cenv, d);
    }

    if (debugflag) {
	printf("Initializing state vector\n");
    }
    text_InitStateVector(&gD->rootSV);
    text_ApplyEnvironment(&gD->rootSV,gD->globalStyle,cenv);

    CleanTextObject(gD, d);

    CreateODAGenericStructure(gD);

    if (WalkTextTree(d, viewToDescribe, gD)) {
	printf("Error walking tree.\n");
	return view_NoDescriptionAvailable;
    }
    return view_NoDescribeError;


}

static CleanTextObject(gD, d)
struct atkoda * gD;
struct text * d; {
    /* This procedure makes sure that all insets have an odd number of cr's before and after them */

    long i;

    if (gD->doDebugging) {
	printf("Entering CleanTextObject\n");
    }

    /* Note: we use GetLength because length can change */
    for (i=0; i<text_GetLength(d); i++) {
	if ((MagicViewChar == text_GetChar(d,i)) && IsViewAtPos(gD,d,i)) {
	    CheckCRs(gD,d,i);
	}
    }

    if (gD->doDebugging) {
	printf("Leaving CleanTextObject\n");
    }


}
#define MagicViewChar '\377'

void CheckCRs(gD, d, pos)
struct atkoda * gD;
struct text * d;
long pos;{
    /* Quick check to see if odd number of crs come before view */
    long    NumofCRs;
    long    testPos;

    if (gD->doDebugging) {
	printf("Entering CheckCRs for view at pos %d\n", pos);
    }


    /* First check for CRs after view */
    NumofCRs = 0;
    for(testPos=pos+1;testPos < text_GetLength(d); testPos++){
	if ('\n' == text_GetChar(d,testPos)) {
	    NumofCRs++;
	}
	else {
	    break;
	}
    }
    if (gD->doDebugging) {
	printf("Found %d crs after view\n", NumofCRs);
    }

    if ((NumofCRs % 2) != 1){
	text_AlwaysInsertCharacters(d, pos+1, "\n", (long) 1);
	if (gD->doDebugging){
	    printf("Inserting a <cr> at pos %d after view\n", pos+1);
	}
    }


    NumofCRs = 0;
    for(testPos=pos-1;testPos>=0; testPos--){
	if ('\n' == text_GetChar(d,testPos)) {
	    NumofCRs++;
	}
	else {
	    break;
	}
    }

    if (gD->doDebugging) {
	printf("Found %d crs before view\n", NumofCRs);
    }

    if ((NumofCRs % 2) != 1){
	text_AlwaysInsertCharacters(d, pos, "\n", (long) 1);
	if (gD->doDebugging){
	    printf("Inserting a <cr> at pos %d before view\n", pos);
	}
    }

    if (gD->doDebugging) {
	printf("Leaving CheckCRs\n");
    }


}


static initializestyle(gD)
struct atkoda * gD;{
    long		    fontSize = 12;
    char		    bodyFont[100];
    char		    *font;
    long		    fontStyle = fontdesc_Plain;
    enum style_MarginValue  marginBasis;
    long		    marginOperand;
    enum style_Unit	    marginUnit;
    boolean		    justify;
    long		    lineSpacingOperand;
    long		    paraSpreadOperand;

    if (gD->doDebugging) {
	printf("Entering initializestyle %x\n", gD);
    }
    
    gD->globalStyle = style_New();
    style_SetName(gD->globalStyle, "global");

    if (environ_ProfileEntryExists("print.bodyfont", FALSE))
	font = environ_GetProfile("print.bodyfont");
    else
	font = environ_GetProfile("bodyfont");

    if (font == NULL || ! fontdesc_ExplodeFontName(font, bodyFont, (long)sizeof(bodyFont), &fontStyle, &fontSize)) {
	strcpy(bodyFont, "Andy");
    }
    justify = environ_GetProfileSwitch("justified", TRUE);
    style_SetFontFamily(gD->globalStyle, bodyFont);
    style_SetFontSize(gD->globalStyle, style_ConstantFontSize,(long) fontSize);
    style_AddNewFontFace(gD->globalStyle, fontStyle);
    if (! justify)
        style_SetJustification(gD->globalStyle, style_LeftJustified);

    /* Wire in some reasonable margin values for exhanging/printing 1 inch on all sides */

    marginBasis = style_ConstantMargin;
    marginOperand = 1 << 16;
    marginUnit = style_Inches;
    if (gD->doFudgeMargins) {
	style_SetNewLeftMargin(gD->globalStyle, marginBasis, marginOperand, marginUnit);
	style_SetNewRightMargin(gD->globalStyle, marginBasis, marginOperand, marginUnit);
    }
    else {
	style_SetNewLeftMargin(gD->globalStyle, style_LeftMargin, 0, style_Points);
	style_SetNewRightMargin(gD->globalStyle, style_RightMargin, 0, style_Points);
	style_SetNewIndentation(gD->globalStyle, style_PreviousIndentation, 0, style_Points);
/*
	style_SetNewLeftMargin(gD->globalStyle, marginBasis, 0, marginUnit);
	style_SetNewRightMargin(gD->globalStyle, marginBasis, 0, marginUnit);
*/
    }
    style_SetNewTopMargin(gD->globalStyle, marginBasis, marginOperand, marginUnit);
    style_SetNewBottomMargin(gD->globalStyle, marginBasis, marginOperand, marginUnit);

    /* And fill in some reasonable values for line and paragraph spacing */
    lineSpacingOperand = /* one line */ 12;  /* baseline to baseline */
    paraSpreadOperand =	/* one line */ 12;  /* one line between paragraphs */
    style_SetNewInterlineSpacing(gD->globalStyle, style_ConstantSpacing, lineSpacingOperand, style_Points);
    style_SetNewInterparagraphSpacing(gD->globalStyle, style_ConstantSpacing, paraSpreadOperand, style_Points);

    if (gD->doDebugging) {
	printf("Leaving initializestyle %x\n", gD);
    }

    return 0;
}
