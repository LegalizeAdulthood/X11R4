/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/atkoda.c,v 1.3 89/10/13 09:56:34 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/atkoda.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/atkoda.c,v 1.3 89/10/13 09:56:34 mss Exp $";
#endif

/*
  $Log:	atkoda.c,v $
 * Revision 1.3  89/10/13  09:56:34  mss
 * Replaced incorrect use of ERROR_DOCUMENT with call to
 * odalib_ERROR_DOCUMENT()
 * 
 * Revision 1.2  89/09/08  17:24:25  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:19:25  mss
 * Initial revision
 * 
 * Revision 1.22  89/08/25  09:29:14  mss
 * Using new tool kit routine for appending subordinate
 * 
 * Revision 1.21  89/07/21  09:47:42  mss
 * Added -e flag for printing warngins
 * 
 * Revision 1.20  89/05/20  20:37:14  mss
 * Remove routines for setting pres/lay styles
 * 
 * Revision 1.19  89/05/07  19:44:41  mss
 * Temporarily eliminated im connection to get around console problem
 * 
 * Revision 1.18  89/04/30  12:11:07  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.17  89/03/23  06:57:31  mss
 * Attaches itself to a window manager for font size calculatios
 * 
 * Revision 1.16  88/12/16  16:05:16  mss
 * Added new way to initialize toolkit specific values
 * (ordering problem between creating/initializing
 * toolkit and global translator data)
 * Also added some more debugging tracing
 * 
 * Revision 1.15  88/12/14  11:40:00  mss
 * Changed used of NULL_STRING constant to
 * odalib_NULL_STRING class procedure
 * 
 * Revision 1.14  88/12/14  10:49:13  mss
 * Changed the use of NULL_CONSTITUENT constant to
 * odalib_NULL_CONSTITUENT() class procedure
 * 
 * Revision 1.13  88/12/13  20:50:43  mss
 * Changed NULL_SEQUENCE to use odalib class
 * procedure instead
 * 
 * Revision 1.12  88/11/29  14:42:10  mss
 * Changed initial value of paragraph break state so that
 * initial <cr>s will be processed as appropriate.
 * 
 * Revision 1.11  88/11/17  17:30:44  mss
 * Added doFudgeMargins flag to selectively turn on and
 * off margin hacking
 * 
 * Revision 1.10  88/11/08  14:52:00  mss
 * Added global state for turning off raster scaling
 * 
 * Revision 1.9  88/11/04  17:01:54  mss
 * Added global state for tracking paragraph breaks
 * 


*/

#include <class.h>
#include <odatk.h>
#include <atkoda.eh>

#include <odalib.ih>
#include <text.ih>
#include <errormsg.h>
#include <txtstvec.h>
#include <style.ih>
#include <im.ih>

#define tracing 1

boolean atkoda__InitializeObject(classID,self)
struct classheader *classID;
struct atkoda *self;
{
    self->ODAParams.automatic_label = BOOL_true;
    self->ODAParams.error_od = fileno(stderr);
    self->ODAParams.format_errors = BOOL_true;
    self->ODAParams.output_filter = NULL_FILTER_ROUTINE;
    self->ODAParams.print_warnings = BOOL_false;

    self->inputFileName = NULL;
    self->outputFileName = NULL;
    self->outputFileDescriptor = stdout;
#if 0
    self->outputDocument = odalib_ERROR_DOCUMENT();
#endif
    self->topLevelDocument = NULL;
    self->topLevelView = NULL;
#if 0
    self->specificLogicalRoot = odalib_NULL_CONSTITUENT();
    self->genericLogicalRoot = odalib_NULL_CONSTITUENT();
    self->genericLayoutRoot = odalib_NULL_CONSTITUENT();
    self->documentProfile = odalib_NULL_CONSTITUENT();
    self->genericFigure = odalib_NULL_CONSTITUENT();
    self->genericParagraph = odalib_NULL_CONSTITUENT();
    self->genericText = odalib_NULL_CONSTITUENT();
    self->genericRaster = odalib_NULL_CONSTITUENT();
    self->genericTable = odalib_NULL_CONSTITUENT();
    self->genericEquation = odalib_NULL_CONSTITUENT();
    self->pSet = odalib_NULL_CONSTITUENT();
#endif
    self->styleList = NULL;
    self->passageSize = -1;
    self->passageTop = -1;
    self->passageStack = NULL;
    self->phraseSize = -1;
    self->phraseTop = -1;
    self->phraseStack = NULL;
#if 0
    self->firstBLO = odalib_NULL_CONSTITUENT();
    self->firstBLOsFonts = odalib_NULL_SEQUENCE();
#endif
    self->numTabsSinceLastCR = 0;
#if 0
    self->highestPassageForPara = odalib_NULL_CONSTITUENT();
#endif
    text_InitStateVector(&self->rootSV);
    self->globalStyle = (struct style *) NULL;
    self->fontList = NULL;
#if 0
    self->curTextLC = odalib_NULL_STRING();
    self->curFNoteLC = odalib_NULL_STRING();
#endif
    self->doDebugging = BOOL_false;
    self->SMU_Numerator = 1;
    self->SMU_Denominator = 1;
    self->NumCRsInParaBreak = 2;
    self->NumCRsLeftinParaBreak = 0;
    self->LastParaBreakFound = -2;
    self->LastParaBreakGenerated = -2;
    self->ScaleRaster = BOOL_true;
    self->doFudgeMargins = BOOL_false;
#if 0
    self->windowSystemIM = im_Create(NULL);
#endif
    return TRUE;

}

void atkoda__InitializeODAState(self)
struct atkoda * self;
{

    self->outputDocument = odalib_ERROR_DOCUMENT();
    self->specificLogicalRoot = odalib_NULL_CONSTITUENT();
    self->genericLogicalRoot = odalib_NULL_CONSTITUENT();
    self->genericLayoutRoot = odalib_NULL_CONSTITUENT();
    self->documentProfile = odalib_NULL_CONSTITUENT();
    self->genericFigure = odalib_NULL_CONSTITUENT();
    self->genericParagraph = odalib_NULL_CONSTITUENT();
    self->genericText = odalib_NULL_CONSTITUENT();
    self->genericRaster = odalib_NULL_CONSTITUENT();
    self->genericTable = odalib_NULL_CONSTITUENT();
    self->genericEquation = odalib_NULL_CONSTITUENT();
    self->pSet = odalib_NULL_CONSTITUENT();
    self->firstBLO = odalib_NULL_CONSTITUENT();
    self->firstBLOsFonts = odalib_NULL_SEQUENCE();
    self->highestPassageForPara = odalib_NULL_CONSTITUENT();
    self->curTextLC = odalib_NULL_STRING();
    self->curFNoteLC = odalib_NULL_STRING();

}

#define StackIncrements	10

void atkoda__PushPassage(gD, newPassage, newStateVector, newStyle)
struct atkoda * gD;
CONSTITUENT newPassage;
struct text_statevector * newStateVector;
struct style * newStyle; {
    /* Push a new passage onto the passage stack */

    if (gD->doDebugging) {
	printf("Entering PushPassage with style %X\n", newStyle);
    }

    /* Space available? */
    if (gD->passageTop == gD->passageSize) {
	/* Run out of stack, must increase it */
	gD->passageSize += StackIncrements;
	if (gD->passageStack == NULL) {
	    gD->passageStack = (p_passage_stk_blk) malloc(sizeof(passage_stk_blk) * gD->passageSize);
	}
	else {
	    gD->passageStack = (p_passage_stk_blk) realloc(gD->passageStack, sizeof(passage_stk_blk) * gD->passageSize);
	}
	if (gD->passageStack == NULL) {
	    gD->passageTop = gD->passageSize = -1;
	    printf("PushPassage: cannot allocate space for passage stack\n");
	    goto LEAVE;
	}
    }

    /* Fill in new entry */
    gD->passageTop++;
    gD->passageStack[gD->passageTop].passage = newPassage;
    gD->passageStack[gD->passageTop].passageSV = *newStateVector;
    gD->passageStack[gD->passageTop].passageStyle = newStyle;
    gD->passageStack[gD->passageTop].passageHasChildren = BOOL_false;

LEAVE:
    if (gD->doDebugging) {
	printf("Leaving PushPassage with style %X\n", newStyle);
    }

}

void atkoda__PushPhrase(gD, newPhrase, newStateVector, newStyle)
struct atkoda * gD;
CONSTITUENT newPhrase;
struct text_statevector * newStateVector;
struct style * newStyle; {
    /* Push a new Phrase onto the Phrase stack */

    if (gD->doDebugging) {
	printf("Entering PushPhrase with style %X\n", newStyle);
    }

    /* Space available? */
    if (gD->phraseTop == gD->phraseSize) {
	/* Run out of stack, must increase it */
	gD->phraseSize += StackIncrements;
	if (gD->phraseStack == NULL) {
	    gD->phraseStack = (p_phrase_stk_blk) malloc(sizeof(phrase_stk_blk) * gD->phraseSize);
	}
	else {
	    gD->phraseStack = (p_phrase_stk_blk) realloc(gD->phraseStack, sizeof(phrase_stk_blk) * gD->phraseSize);
	}
	if (gD->phraseStack == NULL) {
	    gD->phraseTop = gD->phraseSize = -1;
	    printf("PushPhrase: cannot allocate space for phrase stack\n");
	    return;
	}
    }

    /* Fill in new entry */
    gD->phraseTop++;
    gD->phraseStack[gD->phraseTop].phrase = newPhrase;
    gD->phraseStack[gD->phraseTop].phraseSV = *newStateVector;
    gD->phraseStack[gD->phraseTop].phraseStyle = newStyle;

    if (gD->doDebugging) {
	printf("Leaving PushPhrase\n");
    }

}

void atkoda__PopPassage(gD)
struct atkoda * gD; {
    if (gD->passageTop > -1) {
	gD->passageTop--;
    }
}

void atkoda__PopPhrase(gD)
struct atkoda * gD; {
    if (gD->phraseTop > -1) {
	gD->phraseTop--;
    }
}

CONSTITUENT atkoda__TopPassage(gD)
struct atkoda * gD; {
    if (gD->passageTop > -1) {
	return gD->passageStack[gD->passageTop].passage;
    }
    else {
	return odalib_ERROR_CONSTITUENT();
    }
    
}

BOOL_type atkoda__HasTopPassageChildren(gD)
struct atkoda * gD; {
    if (gD->passageTop > -1) {
	return gD->passageStack[gD->passageTop].passageHasChildren;
    }
    else {
	return BOOL_false;
    }
    
}

struct style* atkoda__TopPassageStyle(gD)
struct atkoda * gD; {
    if (gD->passageTop > -1) {
	return gD->passageStack[gD->passageTop].passageStyle;
    }
    else {
	return (struct style * ) NULL;
    }
    
}

void atkoda__TopPassageSV(gD, returnSV)
struct atkoda * gD; 
struct text_statevector * returnSV; {
    if (gD->passageTop > -1) {
	*returnSV =  gD->passageStack[gD->passageTop].passageSV;
    }
    else {
	/* */
    }
    
}

void atkoda__AddTopPassageChild(gD,newChild)
struct atkoda * gD;
CONSTITUENT newChild; {

    if (gD->doDebugging) {
	printf("Entering AddTopPassageChild with child %X\n", newChild);
    }

    if (gD->passageTop > -1) {
	gD->passageStack[gD->passageTop].passageHasChildren = BOOL_true;
	odalib_AppendSubordinate(gD->passageStack[gD->passageTop].passage, newChild);
    }

    if (gD->doDebugging) {
	printf("Leaving AddTopPassageChild\n");
    }

}

void atkoda__AddCurParagraphToHighestPassage(gD)
struct atkoda * gD;{
    INT_type	curPassage;

    if (gD->doDebugging) {
	printf("Entering AddCurParagraphToHighestPassage\n");
    }

    odalib_AppendSubordinate(gD->highestPassageForPara, atkoda_CurParagraph(gD));
    for(curPassage = gD->passageTop; curPassage > -1; curPassage--) {
	if (gD->highestPassageForPara == gD->passageStack[curPassage].passage) {
	    gD->passageStack[curPassage].passageHasChildren = BOOL_true;
	    break;
	}
    }

    if (gD->doDebugging) {
	printf("Leaving AddCurParagraphToHighestPassage\n");
    }

}

CONSTITUENT atkoda__TopPhrase(gD)
struct atkoda * gD; {
    if (gD->phraseTop > -1) {
	return gD->phraseStack[gD->phraseTop].phrase;
    }
    else {
	return odalib_ERROR_CONSTITUENT();
    }
    
}

void atkoda__ClearPhraseStack(gD)
struct atkoda * gD; {

    if (gD->doDebugging) {
	printf("Entering ClearPhraseStack\n");
    }

    gD->phraseTop = -1;
    /* Set the character fonts as needed */
    if (gD->firstBLOsFonts != odalib_NULL_SEQUENCE()) {
	odalib_SetPresAttr(gD->firstBLO, cc_CHARACTER_FONTS, (POINTER_type) gD->firstBLOsFonts, (PARM_MASK_type) 0);
	odalib_DeleteSequence(gD->firstBLOsFonts);
	gD->firstBLOsFonts = odalib_NULL_SEQUENCE();
    }
    gD->highestPassageForPara = odalib_ERROR_CONSTITUENT();
    gD->firstBLO = odalib_ERROR_CONSTITUENT();
    gD->numTabsSinceLastCR = 0;

    if (gD->doDebugging) {
	printf("Leaving ClearPhraseStack\n");
    }


}


void atkoda__SetParagraph(gD,newPara)
struct atkoda * gD;
CONSTITUENT newPara; {
    struct text_statevector tempStateVector;

    if (gD->doDebugging) {
	printf("Entering SetParagraph for paragraph %X\n", newPara);
    }

    atkoda_ClearPhraseStack(gD);
    atkoda_PushPhrase(gD, newPara, &tempStateVector, (struct style *) NULL);
    gD->firstBLO = odalib_ERROR_CONSTITUENT();
    gD->highestPassageForPara = atkoda_TopPassage(gD);
    gD->numTabsSinceLastCR = 0;

    if (gD->doDebugging) {
	printf("Leaving SetParagraph\n");
    }

}

INT_type FontIDLookUp(gD, fontBlk)
struct atkoda * gD;
p_font_list_blk fontBlk; {
    INT_type	rval;
    INT_type	MaxFontUsed;
    INT_type	i;

    if (gD->doDebugging) {
	printf("Entering FontIDLookup for fontid %d\n", fontBlk ? fontBlk->ODAFontID : -1 );
    }

    rval = 0;		/*Assume default font, if nothing else does */
    MaxFontUsed = gD->firstBLOsFonts->length;
    for(i=0;i<MaxFontUsed;i++){
	if (gD->firstBLOsFonts->sequence_value.int_pairs[i].second ==
	    fontBlk->ODAFontID) {
	    /* Already in the character fonts list, reuse it */
	    rval = i;
	    goto LEAVE;
	}
    }

    /* Not in list. See if room available */
    if (MaxFontUsed == 10) {
	printf("Too many fonts in a pargraph. Cannot create entry for %s\n", fontBlk->CurFontFamily);
    }
    else {
	odalib_ExpandSequence(gD->firstBLOsFonts,1);
	rval = MaxFontUsed;
	gD->firstBLOsFonts->sequence_value.int_pairs[MaxFontUsed].first = atkoda_CVDotstoSMU(gD,fontBlk->CurFontSize);
	gD->firstBLOsFonts->sequence_value.int_pairs[MaxFontUsed].second = fontBlk->ODAFontID;
    }

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving FontIDLookUp with %d\n", rval);
    }

    return rval;

}

static char SGRMagic[5]	= {'\033', '[', '1', 'X', 'm'};	/* replaced as necessary */

INT_type InsertFontSGR(gD, CP, CFOffset)
struct atkoda * gD;
CONSTITUENT CP;
INT_type CFOffset;
{
    INT_type		rval;
    SEQUENCE_BYTE_type	theBits;
    INT_type		originalLength;
    INT_type		i;

    if (gD->doDebugging) {
	printf("Entering InsertFontSGR for CFOffset %d\n", CFOffset);
    }

    rval = SUCCESS_INT;
    odalib_GetAttr(CP, at_CONTENT_INFORMATION, (POINTER_type) &theBits, /* don't inherit */ BOOL_false, (PARM_MASK_type *) 0);
    /* Add the magic sequence <esc> [ #  m  where # is 10 through 19 */
    SGRMagic[3] = '0' + CFOffset;
    originalLength = theBits->length;
    odalib_ExpandSequence(theBits, 5);
    for(i=originalLength-1;i>=0;i--){
	theBits->sequence_value.bytes[i+5] = theBits->sequence_value.bytes[i];
    }
    strncpy(theBits->sequence_value.bytes, SGRMagic, 5);
    odalib_SetAttr(CP, at_CONTENT_INFORMATION, (POINTER_type) theBits, NO_PARMS);
    odalib_DeleteSequence(theBits);

    if (gD->doDebugging) {
	printf("Leaving InsertFontSGR\n");
    }

    return rval;
}

void atkoda__AppendBLOToPhrase(gD, newBLO, CP, fontBlk)
struct atkoda * gD;
CONSTITUENT newBLO; 
CONSTITUENT CP;
p_font_list_blk fontBlk;
{

    INT_type	    CFOffset;

    if (gD->doDebugging) {
	printf("Entering AppendBLOToPhrase with newBLO %X and font %d\n", newBLO, fontBlk ? fontBlk->ODAFontID : -1);
    }

    if (gD->firstBLO == odalib_ERROR_CONSTITUENT()) {
	gD->firstBLO = newBLO;
	gD->firstBLOsFonts = (cc_CHARACTER_FONTS_type) odalib_MakeSequence(SEQUENCE_INT_PAIR_tag, 0);
    }
    if (fontBlk) {
	/* Get offset for this character font */
	CFOffset = FontIDLookUp(gD, fontBlk);
	/* Put in magic sequence, as necessary */
	if (CFOffset != 0) {
	    InsertFontSGR(gD, CP, CFOffset);
	}
    }

    odalib_AppendSubordinate(atkoda_TopPhrase(gD), newBLO);

    if (gD->doDebugging) {
	printf("Leaving AppendBLOToPhrase\n");
    }

}

BOOL_type atkoda__OpenParagraph(gD)
struct atkoda * gD;{
    return (gD->phraseTop > -1);
}

CONSTITUENT atkoda__CurParagraph(gD)
struct atkoda * gD;{
    if (atkoda_OpenParagraph(gD))
	return (gD->phraseStack[0].phrase);
    else
	return odalib_ERROR_CONSTITUENT();
}


/* This procedures takes a value in dots (standard ATK dots of 72 points/inch) and returns the amoutn of SMUs taken by those dots. */

INT_type atkoda__CVDotstoSMU(gD, DotValue)
struct atkoda * gD;
INT_type DotValue; {
    INT_type rval;

    rval = atkoda_CVDotsPerInchToSMU(gD, 72, DotValue);

    return rval;

}

/* This procedure takes a denisty of dots per inch (first parameter) and a number of dots and returns the length of those dots at that density in SMUs */

INT_type atkoda__CVDotsPerInchToSMU(gD, DotDensityPerInch, NumOfDots)
struct atkoda * gD;
INT_type DotDensityPerInch;
INT_type NumOfDots; {
    INT_type rval;


    rval = (1200 * gD->SMU_Denominator * NumOfDots) / (DotDensityPerInch * gD->SMU_Numerator);

    return rval;

}

/* Style structure manipulation and creation */

p_font_list_blk atkoda__MakeNewStyleBlock(gD, FromStyle)
struct atkoda * gD;
BOOL_type FromStyle;{
    p_font_list_blk rval;

    if (gD->doDebugging) {
	printf("Entering MakeNewStyleBlock\n");
    }

    rval = (p_font_list_blk) malloc(sizeof(font_list_blk));
    rval->next = gD->fontList;
    if (gD->fontList) {
	rval->ODAFontID = gD->fontList->ODAFontID + 1;
    }
    else {
	rval->ODAFontID = 1;
    }
    gD->fontList = rval;

    odalib_UnspecifyFont(&rval->ODAFontInfo);
    rval->BasedOnStyle = FromStyle;
    /* Information from FSV */
    rval->CurFontFamily = (char *) NULL;
    rval->CurFontAttributes = 0;
    rval->CurScriptMovement = 0;
    rval->CurFontSize = 0;
    rval->SpecialFlags = 0;
    /* Information from Style structure */
    rval->FontFamily = (char *) NULL;
    rval->FontSize.SizeBasis = style_PreviousFontSize;
    rval->FontSize.Operand = 0;
    rval->FontScript.ScriptBasis = style_PreviousScriptMovement;
    rval->FontScript.Operand = 0;
    rval->FontScript.SizeUnit = style_RawDots;
    rval->AddFontFaces = 0;      /* OR'ed FontFaces */
    rval->OutFontFaces = ~0;      /* NOT'ed OR'ed FontFaces */
    rval->AddMiscFlags = 0;      /* Hinge, Pagebreak, Hidden, ... */
    rval->OutMiscFlags = ~0;

    if (gD->doDebugging) {
	printf("Leaving MakeNewStyleBlock\n");
    }

    return rval;

}

BOOL_type SameSV(testFont, testSV)
p_font_list_blk testFont;
struct text_statevector *testSV; {
    BOOL_type rval;

    rval = ((!testFont->BasedOnStyle) && 
	    (testFont->CurFontSize == testSV->CurFontSize) &&
	    (testFont->SpecialFlags == testSV->SpecialFlags) &&
	    (testFont->CurScriptMovement == testSV->CurScriptMovement) &&
	    (testFont->CurFontAttributes == testSV->CurFontAttributes) &&
	    (  (testFont->CurFontFamily == testSV->CurFontFamily) ||
	       (testFont->CurFontFamily && testSV->CurFontFamily && 
		(0 == strcmp(testFont->CurFontFamily , testSV->CurFontFamily))) )
	     );

    return rval;
}

p_font_list_blk atkoda__FindFontFromSV(gD,SV) 
struct atkoda * gD;
struct text_statevector *SV; {
    p_font_list_blk rval;

    if (gD->doDebugging) {
	printf("Entering FindFontFromSV with SV %X\n", SV);
    }

    for (rval = gD->fontList; rval; rval=rval->next) {
	if (SameSV(rval, SV)) {
	    break;
	}
    }

    if (gD->doDebugging) {
	printf("Leaving FindFontFromSV with %X\n", rval);
    }

    return rval;

}

BOOL_type SameStyle(testFont, testStyle)
p_font_list_blk testFont;
struct style *testStyle; {
    BOOL_type rval;

    rval = ((testFont->BasedOnStyle) && 
	     (testFont->FontSize.SizeBasis == testStyle->FontSize.SizeBasis) &&
	     (testFont->FontSize.Operand == testStyle->FontSize.Operand) &&
	     (testFont->FontScript.ScriptBasis == testStyle->FontScript.ScriptBasis) &&
	     (testFont->FontScript.Operand == testStyle->FontScript.Operand) &&
	     (testFont->FontScript.SizeUnit == testStyle->FontScript.SizeUnit) &&
	     (testFont->AddFontFaces == testStyle->AddFontFaces) &&
	     (testFont->OutFontFaces == testStyle->OutFontFaces) &&
	     (testFont->AddMiscFlags == testStyle->AddMiscFlags) &&
	     (testFont->OutMiscFlags == testStyle->OutMiscFlags) &&
	     (  (testFont->FontFamily == testStyle->FontFamily) ||
	        (testFont->FontFamily && testStyle->FontFamily && 
		(0 == strcmp(testFont->FontFamily , testStyle->FontFamily))))
	     );

    return rval;
}

p_font_list_blk atkoda__FindFontFromStyle(gD, curStyle)
struct atkoda * gD;
struct style * curStyle; {

    p_font_list_blk rval;

    if (gD->doDebugging) {
	printf("Entering FindFontFromStyle for style %X\n", curStyle);
    }

    for (rval = gD->fontList; rval; rval=rval->next) {
	if (SameStyle(rval, curStyle)) {
	    break;
	}
    }

    if (gD->doDebugging) {
	printf("Leaving FindFontFromStyle with %X\n", rval);
    }

    return rval;
}

