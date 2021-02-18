/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readtext.c,v 1.2 89/09/08 17:32:36 mss Exp $ */
/* $Source: */

#ifndef lint 
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/readtext.c,v 1.2 89/09/08 17:32:36 mss Exp $";
#endif

/*
  $Log:	readtext.c,v $
 * Revision 1.2  89/09/08  17:32:36  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:41  mss
 * Initial revision
 * 
 * Revision 1.5  89/04/30  12:12:09  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.4  89/03/23  07:10:07  mss
 * Various hacks to properly read UM and MDC text streams (escapes)
 * 
 * Revision 1.3  88/12/01  10:05:18  mss
 * Added special hacks to get fonts for UM/Diamond translator
 * 
 * Revision 1.2  88/11/08  14:57:33  mss
 * Changed processing of in-line codes for SGR, CR/LF and
 * tabs
 * 
 * Revision 1.1  88/11/04  17:07:31  mss
 * Initial revision
 * 

*/

#include <class.h>

#include <view.ih>
#include <dataobj.ih>
#include <txtstvec.h>
#include <text.ih>
#include <textv.ih>
#include <envrment.ih>
#include <style.ih>
#include <stylesht.ih>
#include <viewref.ih>

#include <odatk.h>
#include <odalib.ih>
#include <odaatk.h>
#include <odadef.h>
#include <readraster.h>
#include <errormsg.h>
#include <fontdesc.ih>
#include <getdefault.h>

#define emsg() ErrorMsg(odalib_TKError())




typedef struct sblk_struct {
    struct sblk_struct * next;
    struct style * atks;
    INT_type font_id;
} sblk_type;

static sblk_type * slist = (sblk_type *) NULL;

static INT_type UMFontInfo(gD,ATKStyle, FLIndex)
p_odainfo_type gD;
struct style * ATKStyle;
long FLIndex;
{
    INT_type			rval;
    cc_CHARACTER_FONTS_type	curCF;
    cc_GRAPHIC_RENDITION_type	curGR;
    INT_type			i;	    /* loop index */
    INT_type			CFIndex;    /* which cf to use */
    p_FontAsSV_type		styleFont;	/* which font to examine */


    if (gD->doDebugging) {
        printf("Entering UMFontInfo\n");
    }

    rval = SUCCESS_INT;

	/* Get the font information in absolute terms first */
	styleFont = FindSVforFont(gD,FLIndex);
	/* Have the font description, so extract the information bit by bit */
	if (styleFont->nameDefined) {
	    if (gD->doDebugging) {
		printf("Name defined: %s\n", styleFont->newFamilyName);
	    }
	    style_SetFontFamily(ATKStyle, styleFont->newFamilyName);
	}

	if (styleFont->sizeDefined) {
	    if (gD->doDebugging) {
		printf("Size defined: %d\n", styleFont->newFontSize);
	    }
	    style_SetFontSize(ATKStyle, style_ConstantFontSize, styleFont->newFontSize);
	}

	if (styleFont->facesDefined) {
	    if (gD->doDebugging) {
		printf("Faces defined: %X\n", styleFont->newFaces);
	    }
	    style_ClearNewFontFaces(ATKStyle);
	    style_ClearOldFontFaces(ATKStyle);
	    style_AddNewFontFace(ATKStyle, styleFont->newFaces);
	}
	if (styleFont->flagsDefined) {
	    if (gD->doDebugging) {
		printf("flags defined: %X\n", styleFont->newFlags);
	    }
	    style_AddFlag(ATKStyle, styleFont->newFlags);
	}
	if (styleFont->scriptsDefined) {
	    if (gD->doDebugging) {
		printf("Scripts defined: %d\n", styleFont->newScript);
	    }
	    /* See if a relative or absolute script movement (heuristic) */
	    if (styleFont->newScript == gD->globalSV.CurScriptMovement) {
		/* It's the same, so an absolute value */
		style_SetFontScript(ATKStyle, style_ConstantScriptMovement, styleFont->newScript, style_Points);
	    }
	    else {
		/* No,its different, so assume it's a relative value */
		style_SetFontScript(ATKStyle, style_PreviousScriptMovement, styleFont->newScript - gD->globalSV.CurScriptMovement, style_Points);
	    }

	}



LEAVE:

    if (gD->doDebugging) {
        printf("Leaving GetUMFontInfo\n");
    }

    return rval;
}

INT_type FillInStyleArray(gD)
p_odainfo_type		gD;
{
    CONSTITUENT DocProf;
    dp_FONTS_LIST_type	fl;
    INT_type i;
    struct style * tempstyle;
    sblk_type * tempsblk;
    char michstylename[40];
    FONT_REFERENCE_type * fr;

    if (gD->doDebugging) {
	printf("Entering FillInStyleArray \n");
    }

    for(i=0;i<gD->fonts->length;i++){
	tempsblk = (sblk_type *) malloc (sizeof(sblk_type));
	tempsblk->next = slist;
	slist = tempsblk;
	tempsblk->atks = style_New();
	tempsblk->font_id = gD->fonts->sequence_value.font_pairs[i].font_id;
	fr = &gD->fonts->sequence_value.font_pairs[i].font_reference;
	sprintf(michstylename,"MichiganStyle%d",i);
	style_SetName(tempsblk->atks, michstylename);
	stylesheet_Add(gD->topLevelDocument->styleSheet, tempsblk->atks);
	UMFontInfo(gD, tempsblk->atks, tempsblk->font_id);

    }

    
    if (gD->doDebugging) {
	printf("Leaving FillInStyleArray\n");
    }

    return SUCCESS_INT;
}


struct style * GetUMStyle(gD, FLIndex)
p_odainfo_type		gD;
INT_type FLIndex;
{
    struct style * rval;
    sblk_type * temp;

    if (gD->doDebugging) {
	printf("Entering GetUMStyle for %d\n", FLIndex);
    }

    if (slist == NULL) {
	FillInStyleArray(gD);
    }

    rval = slist-> atks;

    temp = slist;
    for(temp=slist;temp && temp->font_id != FLIndex; temp = temp->next) {
    }

    if (!temp) {
	rval = slist->atks;
    }
    else {
	rval = temp->atks;
    }

    if (gD->doDebugging) {
	printf("Leaving GetUMStyle with %X\n", rval);
    }

    return rval;

}

static INT_type ApplyFontStyle(gD, SGROffset, startPos, len)
p_odainfo_type		gD;
INT_type		SGROffset;
long startPos, len; {

    INT_type rval;
    struct style * ATKStyle;
    INT_type		FLIndex;
    struct environment * newEnv;

    if (gD->doDebugging) {
	printf("Entering ApplyFontStyle for SGROffset %d from %d for %d\n", SGROffset, startPos, len);
    }

    if (SGROffset > gD->initialBLOCFonts->length) {
	printf("SGROffset greater than cc_fonts_length %d\n", gD->initialBLOCFonts->length);
	goto LEAVE;
    }
    FLIndex = gD->initialBLOCFonts->sequence_value.int_pairs[SGROffset].second;
    ATKStyle = GetUMStyle(gD, FLIndex);
    newEnv = text_AlwaysAddStyle(gD->topLevelDocument,startPos,len, ATKStyle);
    /* end the style */
    newEnv->header.nestedmark.includeEnding = 0;
    newEnv->header.nestedmark.includeBeginning = 1;

    rval = SUCCESS_INT;

    
LEAVE:

    if (gD->doDebugging) {
	printf("Leaving ApplyFontStyle\n");
    }

    return rval;
}


INT_type SpecialUMHack(gD, curTextData, curBasic, initialLen)
p_odainfo_type		gD;
SEQUENCE_BYTE_type	curTextData;
CONSTITUENT		curBasic;
long			initialLen;
{
    INT_type rval;
    INT_type	SGROffset;
    INT_type	FLIndex;
    struct style * ATKStyle;
    struct environment * newEnv;

    if (gD->doDebugging) {
	printf("Entering SpecialUMHack basic %X, fonts %X\n", curBasic, gD->initialBLOCFonts);
    }

    rval = SUCCESS_INT;

    if (/*not  initial BLO? */
	 (curBasic != gD->initialBLO) &&
	 /* SGR at front */
	 (curTextData->length >= 5) &&
	 (curTextData->sequence_value.bytes[0] == '\033') &&
	 (curTextData->sequence_value.bytes[1] == '[') &&
	 (curTextData->sequence_value.bytes[2] ==  '1') &&
	 (curTextData->sequence_value.bytes[4] == 'm')) {
	/* We have a SGR, so process it some way */
	SGROffset = curTextData->sequence_value.bytes[3] - '0';
	if (gD->doDebugging){
	    printf("Using %d SGR offset\n", SGROffset);
	}
	ApplyFontStyle(gD, SGROffset, initialLen, text_GetLength(gD->topLevelDocument)-initialLen);
#if 0
	if (SGROffset > gD->initialBLOCFonts->length) {
	    printf("SGROffset greater than cc_fonts_length %d\n", gD->initialBLOCFonts->length);
	    goto LEAVE;
	}
	FLIndex = gD->initialBLOCFonts->sequence_value.int_pairs[SGROffset].second;
	ATKStyle = GetUMStyle(gD, FLIndex);
	newEnv = text_AlwaysAddStyle(gD->topLevelDocument, initialLen, text_GetLength(gD->topLevelDocument)-initialLen, ATKStyle);
	/* end the style */
	newEnv->header.nestedmark.includeEnding = 0;
	newEnv->header.nestedmark.includeBeginning = 1;
#endif

    }
    else {
	if (gD->doDebugging) {
	    printf("Assuming style OK on empty or first paragraph\n");
	}
    }

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving SpecialUMHack\n");
    }


    return rval;
}

INT_type FakeUpMDCStyle(gD, startByte, endByte, SGROffset)
p_odainfo_type		gD;
long startByte, endByte;
INT_type    SGROffset;
{
    INT_type rval;

    rval = SUCCESS_INT;


LEAVE:

    return rval;

}


char * FilterODABytes(gD, curTextData, returnedSize)
p_odainfo_type		gD;
SEQUENCE_BYTE_type	curTextData;
long *			returnedSize;
{
    char	* rval;	    /* Bytes coming back */
    INT_type	srcIndex;   /* For stepping through the source bytes */
    INT_type	srcSize;    /* Number of bytes in source sequence */
    INT_type	dstIndex;   /* For stepping through result bytes */
    INT_type	dstSize;    /* Number of allocated bytes for dst string */
    INT_type	downCount;  /* Number of bytes to throw away as unused */
    INT_type	srcRecovery;	/* Place right after escape to try in case of error in control codes */
    char	testChar;	/* current under examination in control code processing */
    INT_type	startByte, endByte; /* counters for styles */
    char	prevChar;	/* COntains cc_graphic code */

    if (gD->doDebugging) {
        printf("Entering FilterODABytes\n");
    }

    /* Start with same amount of storage */
    dstSize = curTextData->length;
    rval = (char *) malloc(dstSize);
    downCount = 0;
    startByte = 0;

    srcSize = curTextData->length;
    for(srcIndex=0, dstIndex=0;srcIndex<srcSize;srcIndex++){
	switch(curTextData->sequence_value.bytes[srcIndex]){
	    case '\015':    /* cr, just throw away, should be a LF following */
		downCount++;
		break;
	    case '\033':    /* An escape, here we go */
		/* See if we have a CSI */
		downCount++;	/* throw away the escape */
		srcIndex++;	/* move to next character */
		srcRecovery = srcIndex;	/* right after escape, if needed to recover from error later */
		if (srcIndex >= srcSize){
		    /* see if we ran out of source early */
		    printf("Escape sequence prematurely terminated\n");
		    goto LEAVE;
		}
		if (curTextData->sequence_value.bytes[srcIndex] != '[') {
		    printf("Escape sequence not CSI: <esc>%c\n",curTextData->sequence_value.bytes[srcIndex]);
		    /* Just keep all of the bytes since we don't know what it is */
		    srcIndex = srcRecovery;	    /* Reuse the byte in actual text */
		    break;
		}
		downCount++;	    /* throw away the [ */
		/* Now look for a qualifying terminating character, here ^ or m */
		while((++srcIndex < srcSize)){
		    testChar = curTextData->sequence_value.bytes[srcIndex];
		    if (testChar == '^') {
			/* We have a tab -- replace with \t */
			rval[dstIndex++] = (char) '\t';
			break;
		    }
		    else if (testChar == 'm') {
			/* We have a SGR -- we assume styles handled this */
			downCount++;    /* throw away character */
			/* We got it, so now do it */
			FakeUpMDCStyle(gD, startByte, endByte, prevChar-'0');
			startByte = endByte;
			break;
		    }
		    else {
			/* Intermediate characters, just toss */
			prevChar = testChar;
			downCount++;
		    }
		}
		/* see if we ran out of characters before finding a terminating character */
		if (srcIndex == srcSize) {
		    printf("Escape sequence never terminated\n");
		    /* Try to recover enough for later examination */
		    srcIndex = srcRecovery;
		    downCount =	0;
		}
		break;
	    default:	    /* otherwise just keep it */
		rval[dstIndex++] = curTextData->sequence_value.bytes[srcIndex];
		endByte++;
		break;
	}
    }

LEAVE:

    *returnedSize = dstSize - downCount;
    rval = (char *) realloc(rval, dstSize - downCount);

    if (gD->doDebugging) {
        printf("Leaving FilterODABytes\n");
    }

    return rval;

}


void ProcessSingleCharCP(gD, curContent, contentMode, curSV,curBasic)
p_odainfo_type gD;
CONSTITUENT curContent;
INT_type contentMode; 
struct text_statevector * curSV;
CONSTITUENT curBasic;
{

    SEQUENCE_BYTE_type	curTextData;
    char *		atkTextData;
    long		atkTextLen;
    long		initialLen;

    if (gD->doDebugging) {
        printf("Entering ProcessSingleCharCP\n");
    }

    /* Look for content information */
    if (ERROR_INT == SafeGetAttr(curContent, at_CONTENT_INFORMATION, (POINTER_type) &curTextData, Use_Local_Value, Ignore_Parms)) {
	printf("Missing content information(not checking for other places) (%s)\n", emsg());
	return;
    }

    /* Throw the bytes into the data object -- miminal checking, should really be parsed */
    /* Exceedingly quick and dirty checking to make sure that no SGR is in front */
    /* {'\033', '[', '1', 'X', 'm'} */


    initialLen = text_GetLength(gD->topLevelDocument);

    atkTextData = FilterODABytes(gD, curTextData, &atkTextLen);
    text_AlwaysInsertCharacters(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), atkTextData, atkTextLen);
    if (atkTextLen > 0) {
	    gD->LastInsertWasFigure = BOOL_false;
    }
#if 0
    if ((curTextData->length >= 5) &&
	 (curTextData->sequence_value.bytes[0] == '\033') &&
	 (curTextData->sequence_value.bytes[1] == '[') &&
	 (curTextData->sequence_value.bytes[2] ==  '1') &&
	 (curTextData->sequence_value.bytes[4] == 'm')) {
	text_AlwaysInsertCharacters(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), curTextData->sequence_value.bytes+5, (long) curTextData->length-5);
	if (curTextData->length > 5) {
	    gD->LastInsertWasFigure = BOOL_false;
	}

    }
    else {
	text_AlwaysInsertCharacters(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), curTextData->sequence_value.bytes, (long) curTextData->length);
	if (curTextData->length > 0) {
	    gD->LastInsertWasFigure = BOOL_false;
	}
    }
#endif

    SpecialUMHack(gD, curTextData, curBasic, initialLen);

    free(atkTextData);
    if (ERROR_INT == odalib_DeleteSequence(curTextData)) {
	printf("Error deleting sequence for text content information (%s)\n", emsg());
    }

    if (gD->doDebugging) {
        printf("Leaving ProcessSingleCharCP\n");
    }

}


void ProcessCharCP(gD, curBasic, contents, contentType, contentMode, startPosition, curSV)
p_odainfo_type gD;
CONSTITUENT curBasic;
SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
INT_type contentType, contentMode;
long * startPosition;
struct text_statevector * curSV;
{
    INT_type	i;
    VALUE_type curCatValue;

    if (gD->doDebugging) {
        printf("Entering ProcessCharCP\n");
    }

    /* Check to see if this is body text or if some other place */

    /* Check to see if this is a paragraph (unconcatanted) beginning */
    if (ERROR_INT == odalib_GetValueAttr(curBasic, at_CONCATENATION, (POINTER_type) &curCatValue, Use_Inherited)) {
	printf("Could not get value for concatenation attribute and assuming non-conconcatented (%s)\n", emsg());
	curCatValue = at_CONCATENATION_non_concated;
    }

    *startPosition = text_GetLength(gD->topLevelDocument);
    if (curCatValue == at_CONCATENATION_non_concated) {
	/* Starting a new paragraph -- see if closing off old one */
	if (gD->firstParagraphInDocument) {
	    gD->firstParagraphInDocument = BOOL_false;
	}
	else {	/* Finish previous paragraph (if necessary) */
	    if (!gD->LastInsertWasFigure) {
		text_AlwaysInsertCharacters(gD->topLevelDocument, text_GetLength(gD->topLevelDocument), "\n\n", (long) 2);
	    }
	    gD->LastInsertWasFigure = BOOL_false; /* A paragraph break is consider inserting text, so that if an empty paragraph follows, we will start to include paragraph breaks */
	}
	*startPosition = text_GetLength(gD->topLevelDocument);
	/* Cleanup left over state from old initial BLO */
	if (gD->initialBLOCFonts != NULL) {
	    if (ERROR_INT == odalib_DeleteSequence(gD->initialBLOCFonts)) {
		printf("Could not delete copy of character fonts attribute (%s)\n", emsg());
	    }
	}


	/* record new initial BLO (for various lists) */
	gD->initialBLO = curBasic;
	if (ERROR_INT == odalib_GetAttr(curBasic, cc_CHARACTER_FONTS, (POINTER_type) &gD->initialBLOCFonts, Use_Inherited, Ignore_Parms)) {
	    printf("Could not find value for character fonts attribute, assuming no character fonts chosen (%s)\n", emsg());
	} 

    }
    /* Go get all of the bits (assuming ascii for now) */

    for (i=0; i< contents->length; i++ ){
	ProcessSingleCharCP(gD, contents->sequence_value.constituents[i], contentMode, curSV, curBasic);
    }

    if (gD->doDebugging){
        printf("Leaving ProcessCharCP\n");
    }

}

