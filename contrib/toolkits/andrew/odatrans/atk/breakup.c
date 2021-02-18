/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/breakup.c,v 1.2 89/09/08 17:25:33 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/breakup.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/breakup.c,v 1.2 89/09/08 17:25:33 mss Exp $";
#endif

/*
  $Log:	breakup.c,v $
 * Revision 1.2  89/09/08  17:25:33  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:20:52  mss
 * Initial revision
 * 
 * Revision 1.18  89/08/25  09:32:25  mss
 * Switched to using toolkit routines
 * 
 * Revision 1.17  89/05/20  20:38:28  mss
 * Changes how pres/lay attributes set
 * 
 * Revision 1.16  89/05/07  19:40:58  mss
 * Temporary hack to deal with flakey console problems
 * 
 * Revision 1.15  89/04/30  12:06:18  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.14  89/03/23  07:03:11  mss
 * Does some font line calculations from window system;
 * uses new component and DVL specifications
 * 
 * Revision 1.13  88/12/14  10:50:17  mss
 * Changed the use of NULL_CONSTITUENT constant to
 * odalib_NULL_CONSTITUENT() class procedure
 * 
 * Revision 1.12  88/11/29  14:41:17  mss
 * Provided better debugger tracking in paragraph break
 * analysis
 * 
 * Revision 1.11  88/11/18  13:27:43  mss
 * Changed parabreak processing so that
 * 1) the extra new line in a sequence of
 * new lines is appended to the end of the
 * preceeding paragraph instead of leading
 * the next paragraph and 2) attempt to make
 * sure that trailing new lines at the end of
 * a file are preserved (not sure if completely
 * right).
 * 
 * Revision 1.10  88/11/17  17:44:12  mss
 * Changed parabreak to allow single CRs at end fo file
 * to not mean break, but just mean extra CR;
 * Calls to mkinsetcomp to change presentation attrs
 * into layout attrs.
 * 
 * Revision 1.9  88/11/08  14:53:08  mss
 * Changed some flags; patched style analysis so
 * that outbound has reasonable margins, inbound
 * none
 * 
 * Revision 1.8  88/11/04  17:02:03  mss
 * Track paragraph state (including para breaks in
 * the midst of styles)
 * 


*/



/* Module used to breakup ATK style environments into NBS structures */

#include <class.h>

#include <odatk.h>
#include <odadef.h>
#include <odalib.ih>
#include <atkoda.ih>
#include <attsup.h>
#include <text.ih>
#include <textv.ih>
#include <txtstvec.h>
#include <style.ih>
#include <styles.h>
#include <environ.ih>
#include <envrment.ih>
#include <dict.ih>
#include <graphic.ih>
#include <fontdesc.ih>
#include <view.ih>
#include <viewref.ih>
#include <breakup.h>
#include <mkinsetcomp.h>

/* Forward declarations */

void FixupStyles(/*gD, oldPassage, oldStyle*/);
BOOL_type IsParaEnd(/*textdo, pos*/);
BOOL_type IsViewAtPos(/*textdo, pos*/);
BOOL_type IsViewComing(/*gD, textdo, pos*/);
CONSTITUENT MakeNewTextContent(/*gData, textData*/);
CONSTITUENT MakeParagraph(/*gD*/);
char *  FilterString(/*tempStr*/);
CONSTITUENT MakeFigure(/*gD*/);
BOOL_type IsSpecialOddCRTest(/*gD, textdo,pos,len*/);

INT_type ProcessStyleEntry(gD, curTextView, curEnv)
struct atkoda * gD;
struct textview * curTextView;
struct environment * curEnv; {

    CONSTITUENT	    newPassage;
    CONSTITUENT	    newPhrase;
    struct text_statevector tempStateVector;
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering ProcessStyleEntry with environment %X\n", curEnv);
    }
    rval = SUCCESS_INT;

    /* What is current state of formatting */
    text_InitStateVector(&tempStateVector);
    text_ApplyEnvironment(&tempStateVector,gD->globalStyle,curEnv);

    /* Figure out the differences */

    newPassage = MakePassage(gD, curEnv->data.style, SPECIFIC_COMPONENT);

    atkoda_PushPassage(gD, newPassage, &tempStateVector, curEnv->data.style);

    if (atkoda_OpenParagraph(gD)) {
	newPhrase = MakePhrase(gD, curEnv->data.style, SPECIFIC_COMPONENT);
	/* Override any attributes because of nested styles */
	CompensateState(gD, &tempStateVector, curEnv->data.style, newPhrase, (CONSTITUENT_type) NULL); 
	odalib_AppendSubordinate(atkoda_TopPhrase(gD),newPhrase);
	atkoda_PushPhrase(gD, newPhrase, &tempStateVector, curEnv->data.style);
    }
    else {
	newPhrase = odalib_NULL_CONSTITUENT();
    }

    /* Calculate compensating state information, both locally and for fonts */
    /* CompensateState(gD, &tempStateVector, curEnv->data.style, newPassage, newPhrase); */

    if (gD->doDebugging) {
        printf("Leaving ProcessStyleEntry\n");
    }

    return rval;
}

INT_type ProcessStyleExit(gD, curTextView, curEnv)
struct atkoda * gD;
struct textview * curTextView;
struct environment * curEnv; {

    CONSTITUENT	    oldPassage;
    CONSTITUENT	    newPhrase;
    struct text_statevector tempStateVector;
    BOOL_type	    oldHasChildren;
    struct style    * oldStyle;
    at_SUBORDINATES_type originalParaSubordinates, newParaSubordinate;
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering ProcessStyleExit\n");
    }

    rval = SUCCESS_INT;


    /* Finish with passage (and connect as appropriate) */
    oldPassage = atkoda_TopPassage(gD);
    oldHasChildren = atkoda_HasTopPassageChildren(gD);
    atkoda_TopPassageSV(gD,&tempStateVector);
    oldStyle = atkoda_TopPassageStyle(gD);
    atkoda_PopPassage(gD);
    if (oldHasChildren) {
	atkoda_AddTopPassageChild(gD,oldPassage);
	FixupStyles(gD, oldPassage, oldStyle);
	CompensateState(gD, &tempStateVector, oldStyle, oldPassage, (CONSTITUENT_type) NULL); 

    }
    else {
	odalib_DeleteConstituent(oldPassage);
    }

    /* Process any open paragraph */
    if (atkoda_OpenParagraph(gD)) {

	/* See if we are in nested phrases or if we are going up out of paragraph */
	if (atkoda_CurParagraph(gD) != atkoda_TopPhrase(gD)){
	    /* In nested phrases, so just pop phrase */
	    atkoda_PopPhrase(gD);
	}
	else {
	    /* Need to add an intermediate Phrase since we are moving up the entire contents of a paragraph beyond its highest passage point */


	    /* about to shift whole paragraph up a notch */
	    newPhrase = MakePhrase(gD, oldStyle, SPECIFIC_COMPONENT);
	    /* Get the subordinates from the current paragraph and splice in the new phrase */
	    (void) odalib_GetAttr(atkoda_CurParagraph(gD), at_SUBORDINATES, (POINTER_type) &originalParaSubordinates, Use_Local_Value, Ignore_Parms);
	    /* Make phrase's subordinates, the old paragraphs */
	    (void) odalib_SetAttr(newPhrase, at_SUBORDINATES, (POINTER_type) originalParaSubordinates, Ignore_Parms);
	    /* And make para's suboridnates only the phrase */
	    newParaSubordinate = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag,1);
	    newParaSubordinate->sequence_value.constituents[0] = newPhrase;
	    (void) odalib_SetAttr(atkoda_CurParagraph(gD), at_SUBORDINATES, (POINTER_type) newParaSubordinate, Ignore_Parms);

	    /* Cleanup by deleting extra sequences */
	    (void) odalib_DeleteSequence(originalParaSubordinates);
	    (void) odalib_DeleteSequence(newParaSubordinate);

	    /* Reset highest Passage containing part of paragraph */
	    gD->highestPassageForPara = atkoda_TopPassage(gD);

	    /* And check for any kind of special case attributes */
	    /* What is current state of formatting */
	    text_InitStateVector(&tempStateVector);
	    text_ApplyEnvironment(&tempStateVector,gD->globalStyle,curEnv);
	    FixupStyles(gD, newPhrase, oldStyle);
	    CompensateState(gD, &tempStateVector, oldStyle, newPhrase, (CONSTITUENT_type) NULL); 

	}
    }

    if (gD->doDebugging) {
        printf("Leaving ProcessStyleExit\n");
    }
    
    return rval;
}

INT_type CopySVIntoFontBlock(gD, fontBlk, SV)
struct atkoda * gD;
p_font_list_blk fontBlk;
struct text_statevector * SV;{
    INT_type	    rval;
    struct fontdesc * tempFontDesc;
    struct FontSummary * tempFontSummary;

    if (gD->doDebugging) {
	printf("Entering CopySVIntoFontBlock for font %s, size %d, Attributes %X\n", SV->CurFontFamily, SV->CurFontSize, SV->CurFontAttributes);
    }

    rval = SUCCESS_INT;

    fontBlk->CurFontFamily = SV->CurFontFamily;
    fontBlk->CurFontAttributes = SV->CurFontAttributes;
    fontBlk->CurScriptMovement = SV->CurScriptMovement;
    fontBlk->CurFontSize = SV->CurFontSize;
    fontBlk->SpecialFlags = SV->SpecialFlags;

    /* Calculate the size of the line to use */
#if 0
    tempFontDesc = fontdesc_Create(SV->CurFontFamily, SV->CurFontAttributes, SV->CurFontSize);
    tempFontSummary = fontdesc_FontSummary(tempFontDesc,im_GetDrawable(gD->windowSystemIM));
    if (gD->doDebugging) {
	printf("Got calculated line spacing from window system %d\n", tempFontSummary->maxHeight);
    }
    fontBlk->LineSpacing = tempFontSummary->maxHeight;
#endif
    fontBlk->LineSpacing = SV->CurFontSize + 2;

    return rval;

}

p_font_list_blk MakeNewFontFromSV(gD,SV)
struct atkoda * gD;
struct text_statevector * SV; {
    p_font_list_blk rval;

    rval = atkoda_MakeNewStyleBlock(gD, /* From SV */ BOOL_false);
    /* Copy over the state vector fields for later comparison */
    CopySVIntoFontBlock(gD, rval, SV);
    /* Fill in the ODA fields, one by one */
    FillInNewFamily(gD, rval, SV->CurFontFamily);
    /* Fill in the size field */
    FillInNewSize(gD, rval, SV->CurFontSize);
    /* And the face code */
    FillInNewFace(gD, rval, SV->CurFontAttributes);
    /* And various special flags */
    FillInNewFlags(gD, rval, SV->SpecialFlags);
    /* And subscript/supescripts */
    FillInNewSubSuper(gD, rval, SV->CurScriptMovement, SV->CurFontSize);

    return rval;
}

INT_type ProcessTextRun(gD, textdo, start, finish, curEnv)
struct atkoda * gD;
struct text * textdo;
long start;
long finish; 
struct environment * curEnv; {
    char * tempStr;
    char * charPtr;
    char * filteredStr;		/* String massaged with control codes */

    long i, initial_i;
    CONSTITUENT	    newBLO;	/* Hold the text*/
    CONSTITUENT	    newPara;	/* If creating a paragraph */
    INT_type rval;
    at_CONCATENATION_type	    doConcatenation;	/* Whether to concat. new run */
    BOOL_type			    firstTimeThrough;	/* For special casing empty paragraphs at beginning of doc */
    struct text_statevector	    tempStateVector;
    p_font_list_blk		    fontBlk;
    CONSTITUENT			    CP;			/* Content portion made for a BLO */
    long			    len;		/* Length of text object */

    if (gD->doDebugging) {
        printf("Entering ProcessTextRun from %d to %d\n", start, finish);
    }

    rval = SUCCESS_INT;

    /* Special case the beginning of a document */
    firstTimeThrough = (start == 0);
    len = text_GetLength(textdo);

    /* Calculate the font to use -- this should be done on an environment by environment basis, but for now, we'll do it here, though that's grossly inefficent (but obviously correct) */
    text_InitStateVector(&tempStateVector);
    text_ApplyEnvironment(&tempStateVector,gD->globalStyle,curEnv);
    fontBlk = atkoda_FindFontFromSV(gD,&tempStateVector);
    if (!fontBlk) {
	fontBlk = MakeNewFontFromSV(gD, &tempStateVector);
    }


    /* A first version -- just pull out the characters and see if there is an end of paragraph yet or not */

    tempStr = (char *) malloc(3 * sizeof(char) * (finish - start + 1));
    i=start;

    while (i<finish) {
	/* Are we starting a new paragraph? yes => skip over CR's */
	if (! atkoda_OpenParagraph(gD)) {
	    /* Skip over the extra CR's that marked paragraph boundary */
	    initial_i = i;
	    if (gD->doDebugging) {
		printf("About to filter preliminary cr's at position %d (which is %c) until %d\n", i,text_GetChar(textdo,i), finish);
	    }
	    if (! firstTimeThrough) {
		while ( (i<finish) && ('\n' == text_GetChar(textdo, i)) && (gD->NumCRsLeftinParaBreak >0 )) {
		    if (gD->doDebugging) {
			printf("Skipping character %d which is %c\n", i, text_GetChar(textdo,i));
		    }
		    gD->NumCRsLeftinParaBreak--;
		    i++;
		}
	    }
	    /* See if anything left to put into paragraph/phrase! -- must special case the possibility that the style surrounds an otherwise empty paragraph break. Sigh. */
	    /* Note: this is a very tricky condition test. The idea is to properly handle the following case:

		@style{a b c
		<paragraph break>
		} d e f

		where the style closes immediately after the paragraph break. The interpretation, for better or worse, is that the second paragrpah (d e f) should be interpretated as underneath the "style", since the style included the paragraph break, and hence the starting of a new paragraph. Therefore, def is concatenated onto an empty BLO so that the original style's formatting requirements are carried out. The complicated test has to do with the way the cr's at the beginning of a document are handled: the first time through, they are ignored so they can be handled as an "ordinary" paragraph break but after the first time through, their special designatin is removed, and another special test is made to make sure that no extra spurious paragraph gets created on account of the leading paragraphs. Ideally, we are trying to model the effect of having a closed style just before the paragraph break at the beginning of the document. */
	    if (gD->doDebugging) {
		printf("Checking for empty wrap: i = %d, finish = %d, initial_i = %d, start = %d\n", i, finish, initial_i, start);
	    }
	    if ( (i==finish)  &&
		  ((finish != len) || (gD->LastParaBreakGenerated == gD->LastParaBreakFound)) &&
		 ((initial_i != start) || 
		  ((!firstTimeThrough) && (initial_i == 0))) ) {
		/* Nothing to do */
		if (gD->doDebugging) {
		    printf("Doing break out of loop \n");
		}
		break;
	    }
	    firstTimeThrough = BOOL_false;
	    /* Check for a coming view? */
	    if ((IsViewComing(gD, textdo, i)) &&
		(gD->LastParaBreakFound == gD->LastParaBreakGenerated) ){
		/* all done, pick upcoming view */
		break;
	    }
	    /* Last chance to bail out -- we are going to punt the case where a style extended beyond paragraph break but not over any characters -- if after the paragraph break there is nothing left, we're punting */
	    if((i==finish) &&
	       ((finish != len) || (gD->LastParaBreakGenerated == gD->LastParaBreakFound))){
		break;
	    }

	    /* Open the paragraph for later use */
	    newPara = MakeParagraph(gD);
	    atkoda_SetParagraph(gD, newPara);
	    gD->LastParaBreakGenerated = gD->LastParaBreakFound;
	    /* Don't concatenate any text */
	    doConcatenation = at_CONCATENATION_non_concated;
	}
	else {
	    /* continuing paragraph, so concatenate new piece of text */
	    doConcatenation = at_CONCATENATION_concatenated;
	}

	/* Reset the character buffer pointer */
	charPtr = tempStr;

	/* About to get the real guts of the string -- until next para break or until end of text run */
	while ( (i<finish) && (! IsParaEnd(gD, textdo, i))) {
	    *charPtr++ = text_GetChar(textdo,i);
	    i++;
	}

	/* terminate the string */
	*charPtr++ = '\000';

	/* Filter the string for illegal, or reprocessed characters */
	filteredStr = FilterString(gD, tempStr);

	/* Make Basic/CP pieces */
	newBLO = MakeNewTextContent(gD, filteredStr, &CP);
	free(filteredStr);

	/* Set concatenation to match beginning status (as necessary) */
	if (doConcatenation != at_CONCATENATION_non_concated) {
	    odalib_SetLayoutAttr(newBLO, at_CONCATENATION, (POINTER_type) &doConcatenation, ALL_PARMS);
	}

	/* Attach text to appropriate place */
	atkoda_AppendBLOToPhrase(gD, newBLO, CP, fontBlk);

	/* Now decide how to change global state: if reached end of paragraph, then close paragraph and place paragraph onto passage. If not reached end of paragraph, then add text to current phrase. */
	if (IsParaEnd(gD, textdo,i)){
	    /* Found end of paragraph, so we will close the paragraph and place it into containing passage */
	    atkoda_AddCurParagraphToHighestPassage(gD);
	    atkoda_ClearPhraseStack(gD);
	}
    }

    free(tempStr);

    if (gD->doDebugging) {
        printf("Leaving ProcessStyleExit\n");
    }

    return rval;

}

INT_type ProcessInnerView(gD, curEnclosingView, curEnv)
struct atkoda* gD;
struct textview * curEnclosingView;
struct environment * curEnv; {
    /* code to write out view information */

    struct viewref    *vr ;
    struct view		    * embeddedView;
    enum view_DescriberErrs EmbeddedDescriptionStatus;
    INT_type		    rval;
    CONSTITUENT		    FigureCLO;

    if (gD->doDebugging) {
	printf("Entering ProcessInnerView\n");
	printf("Writing out nested inset %X, %X\n", curEnv, curEnv->data.viewref->dataObject);
    }

    rval = SUCCESS_INT;

    /* Set up the Figure CLO for later use */
    FigureCLO = MakeFigure(gD);
    atkoda_SetParagraph(gD, FigureCLO);

    /* Must recalculate styles as appropriate to correct for use of presentation attributes when layout attributes are needed */

    InsetCompensation(gD, curEnclosingView, curEnv, FigureCLO);

    vr = curEnv->data.viewref;
    if((embeddedView = (struct view *)dictionary_LookUp(curEnclosingView,(char *)vr) ) == NULL) {
	if(class_IsTypeByName(vr->viewType,"view") && (embeddedView  = (struct view *)class_NewObject(vr->viewType)) != NULL){
	    view_SetDataObject(embeddedView ,vr->dataObject);
	    dictionary_Insert(curEnclosingView,(char *)vr,(char *)embeddedView);
	}
    }
    else if(embeddedView == (struct view *) textview_UNKNOWNVIEW)
	embeddedView = NULL;

    if(embeddedView != NULL){
	EmbeddedDescriptionStatus = view_Describe(embeddedView, "oda",(FILE*) 0, gD);
    }

    /* And attach figure to containing passage */
    atkoda_AddCurParagraphToHighestPassage(gD);
    atkoda_ClearPhraseStack(gD);

    /* And we should have a <cr> to eat following the view, so look for it */
    gD->NumCRsLeftinParaBreak = 1;

    if (gD->doDebugging) {
	printf("Leaving ProcessInnerView\n");
    }

    return rval;

}

CONSTITUENT MakeNewTextContent(gData, textData, CP)
struct atkoda * gData;
char * textData;
CONSTITUENT *CP; /* returned content portion */
{
    CONSTITUENT returnBLO;
    register CONSTITUENT tempCP;
    SEQUENCE_CONSTITUENT_type tempSeq;
    SEQUENCE_BYTE_type tempCI;
    int i;


    if (gData->doDebugging) {
	printf("Entering MakeNewTextContent for data (%X) %s\n", textData, textData);
    }

    /* Make the Basic logical object and insert it into current CLO */
    returnBLO = odalib_MakeComponent(gData->outputDocument, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);

    /* Make the Content portion */
    *CP = tempCP = odalib_MakeContent(gData->outputDocument, CHARACTER_CONTENT);
    odalib_SetAttr(tempCP, at_TYPE_OF_CODING, (POINTER_type) odalib_at_TYPE_OF_CODING_cc(), ALL_PARMS);

    /* Attach CP to BLO -- we always use 1 CP per BLO */
    tempSeq = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag, 1);
    tempSeq->sequence_value.constituents[0] = tempCP;
    (void) odalib_SetAttr(returnBLO, at_CONTENT_PORTIONS, (POINTER_type) tempSeq, ALL_PARMS);

    /* And get rid of copy of sequence */
    (void) odalib_DeleteSequence(tempSeq);

    /* Attach string to CP. Note we omit the NULL at the end */
    tempCI = odalib_MakeSequence(SEQUENCE_BYTE_tag, strlen(textData));
    for (i=0;i<strlen(textData);i++) {
	tempCI->sequence_value.bytes[i] = textData[i];
    }
    (void) odalib_SetAttr(tempCP, at_CONTENT_INFORMATION, (POINTER_type) tempCI, ALL_PARMS);
    /* And get rid of copy of sequence */
    (void) odalib_DeleteSequence(tempCI);

    /* Mark the BLO as being a text object through its generic */
    (void) odalib_SetAttr(returnBLO, at_OBJECT_CLASS, (POINTER_type) gData->genericText, ALL_PARMS);
    (void) odalib_SetAttr(returnBLO, at_CONTENT_ARCH_CLASS, (POINTER_type) odalib_at_CONTENT_ARCH_CLASS_proc_cc(), ALL_PARMS);

    if (gData->doDebugging) {
	printf("Leaving MakeNewTextContent\n");
    }

    return returnBLO;
}

#define	PassagePrefix "Passage for style "

CONSTITUENT MakePassage(gD, curStyle, ObjectClass)
struct atkoda * gD;
struct style * curStyle; 
VALUE_type ObjectClass; {
    CONSTITUENT rval;
    char * commentString;
    p_styleSheetInfo passageStyleBlk;

    if (gD->doDebugging) {
        printf("Entering MakePassage\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_comp_logical_obj, ObjectClass);

    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Passage", DATA_BYTE_TYPE);

    commentString = (char *) malloc(strlen(curStyle->name) + strlen(PassagePrefix) + 1);
    if (commentString) {
        strcpy(commentString, PassagePrefix);
	strcat(commentString, curStyle->name);
        (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, commentString, DATA_CHAR_TYPE);
	free(commentString);
    }

    /* Look up the correct passage */
    if (ObjectClass == SPECIFIC_COMPONENT) {
	passageStyleBlk = LookupSSInfo(gD, curStyle);
	if (passageStyleBlk && (passageStyleBlk->passage)) {
	    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type)passageStyleBlk->passage , ALL_PARMS);
	}
	else {
	    printf("Could not find generic passage for style %s\n", curStyle->name);
	}
    }

    if (gD->doDebugging) {
        printf("Leaving MakePassage\n");
    }

    return rval;

}

#define	PhrasePrefix "Phrase for style "

CONSTITUENT MakePhrase(gD, curStyle, ObjectClass)
struct atkoda * gD;
struct style * curStyle; 
VALUE_type ObjectClass; {

    CONSTITUENT rval;
    char * commentString;
    p_styleSheetInfo phraseStyleBlk;

    if (gD->doDebugging) {
        printf("Entering MakePhrase\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_comp_logical_obj, ObjectClass);
    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Phrase", DATA_BYTE_TYPE);

    commentString = (char *) malloc(strlen(curStyle->name) + strlen(PhrasePrefix) + 1);
    if (commentString) {
        strcpy(commentString, PhrasePrefix);
	strcat(commentString, curStyle->name);
        (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, commentString, DATA_CHAR_TYPE);
	free(commentString);
    }

    /* Look up the correct phrase */
    if (ObjectClass == SPECIFIC_COMPONENT) {
	phraseStyleBlk = LookupSSInfo(gD, curStyle);
	if (phraseStyleBlk && (phraseStyleBlk->phrase)) {
	    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type)phraseStyleBlk->phrase, ALL_PARMS);
	}
	else {
	    printf("Could not find generic phrase for style %s\n", curStyle->name);
	}
    }

    if (gD->doDebugging) {
        printf("Leaving MakePhrase\n");
    }

    return rval;

}

CONSTITUENT MakeParagraph(gD)
struct atkoda * gD;
{
    CONSTITUENT rval;

    if (gD->doDebugging) {
        printf("Entering MakeParagraph\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_comp_logical_obj, SPECIFIC_COMPONENT);

    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Paragraph", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, "NBS Paragraph", DATA_CHAR_TYPE);
    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type) gD->genericParagraph, ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeParagraph\n");
    }


    return rval;

}

void FixupStyles(gD, oldPassage, oldStyle)
struct atkoda * gD;
CONSTITUENT oldPassage;
struct style * oldStyle; {

    if (gD->doDebugging) {
        printf("Entering FixupStyles\n");
    }


    if (gD->doDebugging) {
        printf("Entering FixupStyles\n");
    }


}


#define	UsingSingleCR	(gD->NumCRsInParaBreak == 1)

BOOL_type IsParaEnd(gD, textdo, pos)
struct atkoda * gD;
struct text * textdo;
long pos; {
    long len;
    char c1, c2;
    BOOL_type rval;

    if (gD->doDebugging) {
        printf("Entering IsParaEnd at pos %d with value %d, last break %d, numCRsLeft %d\n", pos, rval, gD->LastParaBreakFound, gD->NumCRsLeftinParaBreak);
    }

    len = text_GetLength(textdo);
    if (pos >= len) {
	rval = BOOL_true;
	goto LEAVE;
    }

    c1 = text_GetChar(textdo, pos);
    switch(c1) {
        case '\n':
	    if ( UsingSingleCR) {
		gD->NumCRsLeftinParaBreak = 1;
		rval = BOOL_true;
		goto LEAVE;
	    }
	    if ((pos+1) == len) {
		if (gD->LastParaBreakFound == (pos - 1)){
		    rval = BOOL_true;
		    gD->NumCRsLeftinParaBreak = 1;
		}
		else {
		    rval = BOOL_false;
		}
		goto LEAVE;
	    }
	    c2 = text_GetChar(textdo, pos+1);
	    switch (c2) {
		case '\n': /* We now have two CRs is a row, but is this a
			    special case where we have an odd number of CRs
			    and we want the first one to be part of the previous
			    paragraph? Let's check it out */
		           if (IsSpecialOddCRTest(gD,textdo,pos,len)) {
			       rval = BOOL_false;
			       goto LEAVE;
			   }
		           rval = BOOL_true;
			   /* See if we are looking at second cr of previous para break */
			   if ((pos-1) != gD->LastParaBreakFound){
			       gD->LastParaBreakFound = pos;
			       gD->NumCRsLeftinParaBreak = 2;
			   }
			   else {
			       gD->NumCRsLeftinParaBreak = 1;
			   }
			   break;
		case MagicViewChar:
			   rval = IsViewAtPos(gD, textdo, pos+1);
			   break;
		default:   rval = BOOL_false;
		           break;
	    }
	    goto LEAVE;

	case MagicViewChar: rval = IsViewAtPos(gD, textdo, pos);
			    break;
	default:	    rval = BOOL_false;
			    break;
    }

LEAVE:
    if (gD->doDebugging) {
        printf("Leaving IsParaEnd at pos %d with value %d, last break %d, numCRsLeft %d\n", pos, rval, gD->LastParaBreakFound, gD->NumCRsLeftinParaBreak);
    }

    return rval;

}

BOOL_type IsViewAtPos(gD, textdo, pos)
struct atkoda * gD;
struct text * textdo;
long pos; {
    struct environment * testenv;
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering IsViewAtPos at position %d\n", pos);
    }

    testenv = environment_GetInnerMost(textdo->rootEnvironment, pos);
    rval = (testenv->type != environment_Style);

    if (gD->doDebugging) {
        printf("Leaving IsViewAtPos with value %d\n", rval);
    }

    return rval;
}

BOOL_type IsSpecialOddCRTest(gD, textdo,pos,len)
struct atkoda * gD;
struct text * textdo;
long pos; 
long len; {

    BOOL_type	rval;
    CHAR_type	prevChar;
    INT_type	curPos;
    INT_type	numCRs;
    BOOL_type	isOdd;
    CHAR_type	lastChar;

    if (gD->doDebugging) {
        printf("Entering IsSpecialOddCRTest at position %d\n", pos);
    }

    rval = BOOL_false;
    numCRs = 0;

    /* We want to see if this is a trailing end of a paragraph rather than a paragraph break. To speed up things, let's make sure that the previous character was not a CR, beginning of document or view, since that would immediately imply that we are not in the special case */

    if (pos == 0) {
	rval = BOOL_false;
	goto LEAVE;
    }

    prevChar = text_GetChar(textdo,pos-1);
    if ( ('\n' == prevChar) ||
	 ( (MagicViewChar == prevChar) && IsViewAtPos(gD, textdo, pos-1))){
	rval = BOOL_false;
	goto LEAVE;
    }

    /* So, we know that we are at the beginning of a sequence of CRs. Now we need to know how many there are and whether they end with a EOF, a view or a chataracter */
    for(curPos = pos+1; (curPos < len) &&
                        ('\n' == (lastChar = text_GetChar(textdo, curPos))); curPos++);
    numCRs = curPos - pos;
    isOdd = (numCRs & 0x1);
    if (isOdd) {
	/* Odd number of CRs, so must either be EOF, or not a view */
	if (curPos == len) { /* end of file, so keep CR */
	    rval = BOOL_true;
	}
	else if (lastChar != MagicViewChar){ /* not a view start */
	    rval = BOOL_true;
	}
	else if (!IsViewAtPos(gD, textdo, curPos)){ /* not really a view at curPos, just the character that looks like it */
	    rval = BOOL_true;
	}
	else {	/* must have a view at CurPos, so leading odd CRs are parabreak */
	    rval = BOOL_false;
	}
    }
    else {  /* Have an even number of CRs - in theory, there *must* be an
	      odd number of CRs around a view, since the preprocessing
	      guarantees it. Hence an even number of CRs always represents
	      a sequence of paragraph breaks */
	rval = BOOL_false;
    }

LEAVE:

    if (gD->doDebugging) {
        printf("Leaving IsSpecialOddCRTest with value %d after %d CRs\n", rval, numCRs);
    }

    return rval;
}

BOOL_type IsViewComing(gD, textdo, pos)
struct atkoda * gD;
struct text * textdo;
long pos; {
    BOOL_type	    rval;
    INT_type	    len;
    CHAR_type	    c1;

    if (gD->doDebugging) {
        printf("Entering IsView at position %d\n", pos);
    }

    len = text_GetLength(textdo);

    rval = BOOL_false;

    if (pos >= len) {
	goto LEAVE;
    }
    
    c1 = text_GetChar(textdo, pos);
    if ((c1==MagicViewChar) && IsViewAtPos(gD,textdo,pos)){
	rval = BOOL_true;
    }
    else if ((c1=='\n') && ((pos+1) < len) && (MagicViewChar == text_GetChar(textdo,pos+1)) && IsViewAtPos(gD, textdo, pos+1)) {
	rval = BOOL_true;
    }

LEAVE:

    if (gD->doDebugging) {
        printf("Leaving ViewComing with value %d\n", rval);
    }

    return rval;

}


char *  FilterString(gD, tempStr)
struct atkoda * gD;
char * tempStr; {
    char * rval;
    INT_type	curLen;		/* Current length of destination string + null */
    INT_type	tabLen;		/* Length of STAB codes, w/o null */
    char	* srcPtr;	/* Where reading from */
    INT_type	dstIndex;	/* Where we are reading from */
    char	tabString[9];	/* For constructing tab indication */
    INT_type	i;		/* For copying over tab string */

    if (gD->doDebugging) {
	printf("Entering FilterString with length %d for string %s\n", strlen(tempStr), tempStr);
    }

    /* Filter string performs two transformations: it converts all LF (\n) into CR/LF pairs, and it changes all tabs into STABs */

    curLen = strlen(tempStr) + 1;
    rval = (char *) malloc(curLen);

    /* Cover over each character, checking for special cases */
    for(dstIndex = 0, srcPtr=tempStr; *srcPtr; srcPtr++){
	switch(*srcPtr){
		case '\n':
		      curLen++;
		      if(gD->doDebugging) {
			  printf("Adding LF to CR giving new length of %d\n", curLen);
		      }
		      rval = (char *) realloc(rval, curLen);
		      rval[dstIndex++] = '\015'; /* cr */
		      rval[dstIndex++] = '\012'; /* lf */
		      gD->numTabsSinceLastCR = 0;
		      break;
		case '\t':
		      if(gD->doDebugging) {
			  printf("Adding (s)tab\n");
		      }
		      gD->numTabsSinceLastCR++;
		      if (gD->numTabsSinceLastCR > 9999) {
			  printf("Too many tabs in line %d\n", gD->numTabsSinceLastCR);
			  gD->numTabsSinceLastCR = 1;
		      }
		      else if (gD->numTabsSinceLastCR <= 0) {
			  printf("Illegal tab value %d -- internal error\n", gD->numTabsSinceLastCR);
			  gD->numTabsSinceLastCR = 1;
		      }
		      sprintf(tabString, "\033[%d ^", gD->numTabsSinceLastCR);
		      tabLen = strlen(tabString);
		      curLen += (tabLen - 1); /* -1 because we are resuing the "tab" character */
		      rval = (char *) realloc(rval, curLen);
		      for (i=0;i<tabLen;i++){
			  rval[dstIndex++] = tabString[i];
		      }
		      break;
		default:
		    rval[dstIndex++] = *srcPtr;
	}

    }

    /* terminate the copy */
    rval[dstIndex++] = (char) 0;

    if (gD->doDebugging) {
	printf("Leaving FilterString with %s\n", rval);
    }

    return rval;
}

static CONSTITUENT MakeFigure(gD)
struct atkoda * gD;{
    CONSTITUENT rval;

    if (gD->doDebugging) {
        printf("Entering MakeFigure\n");
    }

    rval = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_comp_logical_obj, SPECIFIC_COMPONENT);

    (void) odalib_SetStringAttr(rval, at_APPLICATION_COMMENTS, "Figure", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(rval, at_USER_READABLE_COMMENTS, "ATK Embedded Object", DATA_CHAR_TYPE);

    /* Connect with generic  */
    odalib_SetAttr(rval, at_OBJECT_CLASS, (POINTER_type) gD->genericFigure, ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeFigure\n");
    }

    return rval;

}
