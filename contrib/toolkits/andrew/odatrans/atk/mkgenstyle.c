/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgenstyle.c,v 1.3 89/10/12 15:39:06 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgenstyle.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgenstyle.c,v 1.3 89/10/12 15:39:06 ghoti Exp $";
#endif

/*
  $Log:	mkgenstyle.c,v $
 * Revision 1.3  89/10/12  15:39:06  ghoti
 * BUG: 49 (#ifdef 0 -> #if 0)
 * 
 * Revision 1.2  89/09/08  17:28:48  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:21:36  mss
 * Initial revision
 * 
 * Revision 1.12  89/08/25  11:36:44  mss
 * Changed to use tool kit instead of support routines
 * (some simplications)
 * 
 * Revision 1.11  89/05/20  20:43:17  mss
 * Changed style attribute setting
 * 
 * Revision 1.10  89/04/30  12:07:31  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.9  89/03/23  07:06:36  mss
 * Uses same tracing technique as rest of translator
 * 
 * Revision 1.8  88/11/18  15:58:14  mss
 * Fixed problem where ATK uses negative margins
 * 
 * Revision 1.7  88/11/17  13:45:37  mss
 * Added generation of separation, line spacing,
 * first line offset and indentation (as appropriate)
 * 
 * Revision 1.6  88/11/02  16:57:01  mss
 * 
 * 
 * Revision 1.5  88/10/31  21:31:45  mss
 * Added more style function (fonts/margins)
 * 
 * Revision 1.4  88/10/25  15:37:19  mss
 * Added more style processing/default processing
 * 
 * Revision 1.3  88/10/21  15:04:59  mss
 * Added font name style (generic) generation
 * 
 * Revision 1.2  88/10/17  15:57:09  mss
 * Added more debug tracing;
 * Fixed GetCurOffset to return value
 * 
 * Revision 1.1  88/10/17  12:01:01  mss
 * Initial revision
 * 
 * Revision 1.1  88/09/22  11:16:12  mss
 * Initial revision
 * 

*/

/* Procedure for calculating the differences between state vectors, given a style specification */

#include <stdio.h>
#include <ctype.h>
#include <class.h>
#include <odatk.h>
#include <odalib.ih>
#include <styles.h>
#include <attsup.h>

#include <atkoda.ih>
#include <text.ih>
#include <txtstvec.h>
#include <style.ih>
#include <fontdesc.ih>
#include <mkgenstyle.h>
#include <odadef.h>

/* Forward declarations */
at_OFFSET_type * GetCurOffset(/*gD,LayoutStyle,returnMask*/);

INT_type CalcStyleAttributes(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrAttributes %X, %X\n", newStyleBlk, curStyle);
    }

    numChanges = 0;

    /* Go through each possible style attribute and see if anything to do */
    /* Left margin and indentation (related) */
    numChanges += CalcStyleAttrLMandIndent(gD, newStyleBlk, curStyle);

    /* Right margin */
    numChanges += CalcStyleAttrRM(gD, newStyleBlk, curStyle);

    /* Page: top margin,bottom margin, header, footer */
    numChanges += CalcStyleAttrPage(gD, newStyleBlk, curStyle);

    /* Justification */
    numChanges += CalcStyleAttrJust(gD, newStyleBlk, curStyle);

    /* New Line spacing */
    numChanges += CalcStyleAttrLSpacing(gD, newStyleBlk, curStyle);

    /* New paragraph spread */
    numChanges += CalcStyleAttrPSpread(gD, newStyleBlk, curStyle);

    /* Font calculations */
    numChanges += CalcStyleAttrFont(gD, newStyleBlk, curStyle);

    /* Tab calculations */
    numChanges += CalcStyleAttrTabs(gD, newStyleBlk, curStyle);

    /* Misc flags */
    numChanges += CalcStyleAttrMisc(gD, newStyleBlk, curStyle);

    /* Counter calculations */
    numChanges += CalcStyleAttrCounter(gD, newStyleBlk, curStyle);

    /* Information read in from elsewhere */
    numChanges += CalcStyleAttrInformation(gD, newStyleBlk, curStyle);

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrAttributes\n");
    }

    return numChanges;

}

INT_type CalcStyleAttrLMandIndent(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		    numChanges;
    enum style_MarginValue  NewLMarginBasis, NewIndentationBasis;
    long		    NewLMarginOperand, NewIndentationOperand;
    enum style_Unit	    NewLMarginUnit, NewIndentationUnit;
    INT_type		    oda_compensation;
    INT_type		    IndentationPAttr;
    INT_type		    RHOffset;
    INT_type		    FLOffset;
    struct text_statevector *styleSV;
    at_OFFSET_type	    * curOffset;
    PARM_MASK_type	    curOffsetMask;
    BOOL_type		    mustDoOffset, mustDoFLO, mustDoIndentation;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrLMandIndent\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;
    mustDoOffset = mustDoFLO = mustDoIndentation = BOOL_false;

    /* Get the old values */

    style_GetNewLeftMargin(curStyle, &NewLMarginBasis, &NewLMarginOperand, &NewLMarginUnit);
    style_GetNewIndentation(curStyle, &NewIndentationBasis, &NewIndentationOperand, &NewIndentationUnit);

    /* See if any changes specified that resulted in differing state vectors */
    if ( /* Any style changes? */
	 (NewLMarginBasis != style_LeftMargin || NewLMarginOperand != 0 || NewIndentationBasis != style_PreviousIndentation || NewIndentationOperand != 0)  ) {
	/* Adjust the local values appropriately -- since left margin and paragraph indentation are ganged together, must calculate what they would look like in terms of ODA terms and only add those that are different */
	/* First, old values */
	oda_compensation = (styleSV->CurIndentation < 0) ? styleSV->CurIndentation : 0;
	RHOffset = styleSV->CurLeftMargin + oda_compensation;
	IndentationPAttr = -oda_compensation;
	FLOffset = styleSV->CurIndentation;
	if (gD->doDebugging) {
	    printf("CurIndentation %d, CurLeftMargin %d\n", styleSV->CurIndentation, styleSV->CurLeftMargin);
	    printf("RHOffset %d, 1st line offset %d, indentation %d\n", RHOffset, FLOffset, IndentationPAttr);
	}

	/* Now, fixup any ATK values which are bogus */
	if (RHOffset < 0) {
	   if (gD->doDebugging) {
	       printf("Current Offset goes beyond left margin (negative) changing to 0\n");
	   }
	   RHOffset = 0;
	}


	/* Set only what is needed */
	if (NewLMarginBasis != style_LeftMargin || NewLMarginOperand != 0 ) {
	    if (gD->doDebugging) {
		printf("About change Left margin\n");
	    }
	    mustDoOffset = BOOL_true;
	}
	if (NewIndentationBasis != style_PreviousIndentation || NewIndentationOperand != 0) {
	    if (gD->doDebugging) {
		printf("About change first line offset\n");
	    }
	    mustDoFLO = BOOL_true;
	}
	if (mustDoIndentation && (styleSV->CurIndentation < 0)) {
	    if (gD->doDebugging) {
		printf("About change offset and indentation\n");
	    }
	    mustDoOffset = BOOL_true;
	    mustDoIndentation = BOOL_true;
	}

	/* Set the new lefthand offset attribute, properly scaled. */
	if (mustDoOffset) {
	    curOffset = GetCurOffset(gD,newStyleBlk->layoutStyle, &curOffsetMask);
	    curOffset->right_hand_offset = atkoda_CVDotstoSMU(gD, RHOffset);
	    curOffsetMask |= RIGHT_HAND_OFFSET_parm;
	    odalib_SetAttr(newStyleBlk->layoutStyle, at_OFFSET,(POINTER_type) curOffset, curOffsetMask);
	    free(curOffset);
	}
	/* And first line offset, and presentation -- for now, all three */
	if (mustDoFLO) {
	    odalib_SetIntAttr(newStyleBlk->presStyle, cc_1ST_LINE_OFFSET, atkoda_CVDotstoSMU(gD, FLOffset));
	}
	/* And the indentation */
	if (mustDoIndentation) {
	    odalib_SetIntAttr(newStyleBlk->presStyle, cc_INDENTATION, atkoda_CVDotstoSMU(gD, IndentationPAttr));
	}

    }

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrLMandIndent\n");
    }

    return numChanges;

}

INT_type CalcStyleAttrRM(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		    numChanges;
    enum style_MarginValue  NewRMarginBasis;
    long		    NewRMarginOperand;
    enum style_Unit	    NewRMarginUnit;
    struct text_statevector *styleSV;
    at_OFFSET_type	    * curOffset;
    PARM_MASK_type	    curOffsetMask;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrRM gD (%X), newStyleBlk (%X), curStyle (%X)\n",
	       gD, newStyleBlk, curStyle);
    }

    styleSV = &newStyleBlk->SV;

    style_GetNewRightMargin(curStyle, &NewRMarginBasis, &NewRMarginOperand, &NewRMarginUnit);
    /* See if any changes specified that resulted in differing state vectors */
    if ( /* Any style changes? */
	 (NewRMarginBasis != style_RightMargin || NewRMarginOperand != 0)  ) {

	/* Set the new lefthand offset attribute, properly scaled. */
	curOffset = GetCurOffset(gD,newStyleBlk->layoutStyle, &curOffsetMask);

	curOffset->left_hand_offset = atkoda_CVDotstoSMU(gD, newStyleBlk->SV.CurRightMargin);

	/* ATK allows for wierdness, so prevent it */
	if (curOffset->left_hand_offset < 0) {
	    if (gD->doDebugging) {
		printf("Negative right margin (%d) being set to 0\n",curOffset->left_hand_offset);
	    }
	    curOffset->left_hand_offset = 0;
	}

	curOffsetMask |= LEFT_HAND_OFFSET_parm;
	odalib_SetAttr(newStyleBlk->layoutStyle, at_OFFSET,(POINTER_type) curOffset, curOffsetMask);
	free(curOffset);
    }

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrRM\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrPage(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    enum style_MarginValue  NewBMarginBasis, NewTMarginBasis;
    long		    NewBMarginOperand, NewTMarginOperand;
    enum style_Unit	    NewBMarginUnit, NewTMarginUnit;
    struct text_statevector *styleSV;
    LAYOUT_OBJECT_type    LayoutObject;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrPage\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

    style_GetNewBottomMargin(curStyle, &NewBMarginBasis, &NewBMarginOperand, &NewBMarginUnit);
    style_GetNewTopMargin(curStyle, &NewTMarginBasis, &NewTMarginOperand, &NewTMarginUnit);
    /* See if any changes specified that resulted in differing state vectors */
    if ( /* Any style changes? */
	 (NewBMarginBasis != style_BottomMargin || NewBMarginOperand != 0 || NewTMarginBasis != style_TopMargin || NewTMarginOperand != 0)  ||
	 style_IsNewPageAdded(curStyle) || style_IsNewPageRemoved(curStyle)) {
	if (style_IsNewPageAdded(curStyle)){
	    LayoutObject.layout_object_tag = OBJECT_TYPE_PAGE_tag;
	    odalib_SetLayoutAttr(newStyleBlk->passage, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObject, ALL_PARMS);
	    odalib_SetLayoutAttr(newStyleBlk->phrase, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObject, ALL_PARMS);
	}
	else if (style_IsNewPageRemoved(curStyle)){
	    LayoutObject.layout_object_tag = NULL_tag;
	    odalib_SetLayoutAttr(newStyleBlk->passage, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObject, ALL_PARMS);
	    odalib_SetLayoutAttr(newStyleBlk->phrase, at_NEW_LAYOUT_OBJECT, (POINTER_type) &LayoutObject, ALL_PARMS);
	}

	/* Reset the pages margins -- means a new generic page, layout category, and such */
#if 0
	printf("Unexpected top and bottom margin changes in ATK document for style %s, style Top %d,  style Bottom %d\n", curStyle->name, styleSV->CurTopMargin,  styleSV->CurBottomMargin);
#endif
    }

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrPage\n");
    }

    return numChanges;

}

INT_type CalcStyleAttrJust(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type			numChanges;
    struct text_statevector *styleSV;
    enum style_Justification	newJustification;


    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrJust\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

    newJustification = style_GetJustification(curStyle);
    switch(newJustification) {
	    case style_PreviousJustification:
		/* Nothing to do, just ignore */
		break;
	    case style_LeftJustified:
		/* Make no justification */
		odalib_SetValueAttr(newStyleBlk->presStyle, cc_ALIGNMENT, cc_ALIGNMENT_start_aligned);
		break;
	    case style_RightJustified:
		odalib_SetValueAttr(newStyleBlk->presStyle, cc_ALIGNMENT, cc_ALIGNMENT_end_aligned);
		break;
	    case style_Centered:
		odalib_SetValueAttr(newStyleBlk->presStyle, cc_ALIGNMENT, cc_ALIGNMENT_centred);
		break;
	    case style_LeftAndRightJustified:
            case style_LeftThenRightJustified:
		odalib_SetValueAttr(newStyleBlk->presStyle, cc_ALIGNMENT, cc_ALIGNMENT_justified);
		break;
    }

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrJust\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrLSpacing(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    enum style_SpacingValue  specSpacingBasis;
    long		     specSpacingOperand;
    enum style_Unit	     specSpacingUnit;
    struct text_statevector *styleSV;
    long CurSpacing;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrLSpacing\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

    style_GetNewInterlineSpacing(curStyle, &specSpacingBasis, &specSpacingOperand, &specSpacingUnit);
    if ((specSpacingBasis != style_InterlineSpacing || specSpacingOperand != 0)) {
	numChanges++;
	/* We have a relative value for interline spacing, set it accordingly with proper scaling*/
	/* Note: this could be tricky, since it means defining a new spacing and inserting it as a content information prefix */
    }

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrLSpacing\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrPSpread(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    enum style_SpacingValue IPSpacing, AboveSpacing, BelowSpacing;
    long		    IPOperand, AboveOperand, BelowOperand;
    enum style_Unit	    IPUnit, AboveUnit, BelowUnit;
    struct text_statevector *styleSV;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrPSpread\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

    style_GetNewInterparagraphSpacing(curStyle, &IPSpacing, &IPOperand, &IPUnit);
    style_GetNewAbove(curStyle, &AboveSpacing, &AboveOperand, &AboveUnit);
    style_GetNewBelow(curStyle, &BelowSpacing, &BelowOperand, &BelowUnit);

    if ((AboveSpacing != style_AboveSpacing || AboveOperand != 0 || 
	 BelowSpacing != style_BelowSpacing || BelowOperand != 0 ||
	 IPSpacing != style_InterparagraphSpacing || IPOperand != 0 )){

    }

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrPSpread\n");
    }

    return numChanges;
}

INT_type CopyStyleIntoFontBlock(gD, fontBlk, curStyle)
struct atkoda * gD;
p_font_list_blk	fontBlk;
struct style * curStyle;
{
    INT_type rval;

    fontBlk->FontFamily = curStyle->FontFamily;
    fontBlk->FontSize = curStyle->FontSize;
    fontBlk->FontScript = curStyle->FontScript;
    fontBlk->AddFontFaces = curStyle->AddFontFaces;
    fontBlk->OutFontFaces = curStyle->OutFontFaces;
    fontBlk->AddMiscFlags = curStyle->AddMiscFlags;
    fontBlk->OutMiscFlags = curStyle->OutMiscFlags;

    rval = SUCCESS_INT;

    return rval;

}

#define	helveticaIndex	0
#define	timesRomanIndex	1
#define	courierIndex	2
#define	symbolIndex	3

/*
(1) Helvetica and Times are registered names of Linotype as font family names.

(2) Symbol and Courier are generic names for those font families.

(3) Symbol and Courier typefaces are copyrights of Adobe.

(4) Times Roman is a particular type face of the font family Times.

We generally use Helevetica, Times, Symbol and Courier in Andrew. Therefore, we will describe these fonts in the 9541 font descirptors as

ATK Name	Font Family	Font Design

Helvetica	Linotype/Helvetica	5.1.2 (Sans Serif, Gothic, Neo-grotesque)
Times		Linotype/Times	4.5.1 (Serif, Legibility, Rounded)
Courier	Adobe/Courier	4.6.5 (Serif, Square Serif, Typewriter)
Symbol		Adobe/Symbol	--
*/

static struct {
    char * AndyName;
    char * ISOName;
    VALUE_type DesignClass;	/*  0 => no class */
}  FontNameMappingTable [] =
{
    {"andysans", "Linotype/Helvetica", DSCLASS_SANS_gothic_neo },
    {"andy",  "Linotype/Times", DSCLASS_SERIFS_legi_rounded },
    {"andytype",  "Adobe/Courier", DSCLASS_SERIFS_square_type },
    {"andysymbol", "Adobe/Symbol", DSCLASS_ERROR }
};

#define NumAndyFonts (sizeof(FontNameMappingTable) / sizeof(FontNameMappingTable[0]))



INT_type FillInNewFamily(gD, fontBlk, styleFontFamily)
struct atkoda * gD;
p_font_list_blk	fontBlk;
char * styleFontFamily;
{
    char	* canonicalFamilyName;
    char	* srcPtr, *dstPtr;
    INT_type	i;
    INT_type	rval;
    SEQUENCE_ISO9541_ID_type	tempSeq;
    char	*tempName; /* for temp name */

    rval = SUCCESS_INT;

    if (gD->doDebugging) {
	printf("Entering FillInNewFamily for %s\n", styleFontFamily);
    }

    /* Convert name to all lower case */
    canonicalFamilyName = (char *) malloc(strlen(styleFontFamily)+1);
    for(srcPtr=styleFontFamily, dstPtr=canonicalFamilyName; *srcPtr; srcPtr++, dstPtr++){
	*dstPtr = (islower(*srcPtr) ?  *srcPtr : tolower(*srcPtr) );
    }

    /* Mark the end of the destination string */
    *dstPtr = 0;

    /* Check against specially renamed adobe fonts anyway */
    for(i=0;i<NumAndyFonts;i++){
	if (gD->doDebugging) {
	    printf("Comparing %s with %s(%d)\n", canonicalFamilyName,  FontNameMappingTable[i].AndyName, i);
	}
	if (0 == strcmp(canonicalFamilyName, FontNameMappingTable[i].AndyName)) {
	    break;
	}
    }
    /* Did we find a match? */
    if (i<NumAndyFonts) {
	/* Yes, a match, so use its attributes */
	odalib_SetFontFamily(&fontBlk->ODAFontInfo,
			     FontNameMappingTable[i].ISOName);
    }
    else {
	/* Nope, strange font, just reuse its name */
	printf("Unknown ATK font %s found\n", styleFontFamily);
	tempName = (char *) malloc(strlen(styleFontFamily) + strlen("CMUITC/") + 1 /* null */ );
	sprintf(tempName,"CMUITC/%s", styleFontFamily);
	odalib_SetFontFamily(&fontBlk->ODAFontInfo, tempName);
	free(tempName);
    }
    free(canonicalFamilyName);

    if (gD->doDebugging) {
	printf("Leaving FillInNewFamily\n");
    }

    return rval;
}

INT_type FillInNewSize(gD, fontBlk, CurFontSize)
struct atkoda * gD;
p_font_list_blk	fontBlk;
long CurFontSize; {

    INT_type rval;

    if (gD->doDebugging) {
	printf("Entering FillInNewSize %d\n", CurFontSize);
    }


    /* The design size of a font has to be given in mm, which is very strange, but we'll do it */
    /* Size(mm) = (size in pts) * (1 inch/72 pts) * (2.54 cm / 1 inch) (100 mm/10 cm) */
    /* Size (mm) = (size) * 254 / 720 = size * 127 / 360 */

    fontBlk->ODAFontInfo.dsnsize.first = CurFontSize * 127;
    fontBlk->ODAFontInfo.dsnsize.second = 360;

    if (gD->doDebugging) {
	printf("Leaving FillInNewSize %d\n", CurFontSize);
    }

    return rval;
}

INT_type FillInNewFace(gD, fontBlk, CurFontAttributes)
struct atkoda * gD;
p_font_list_blk	fontBlk;
long CurFontAttributes; 
{

    INT_type rval;
    INT_type	numRepeats;

    if (gD->doDebugging) {
	printf("Entering FillInNewFace %d\n", CurFontAttributes);
    }


    /* Weights: Bold */
    if (CurFontAttributes & (fontdesc_Bold | fontdesc_Thin | fontdesc_Black)){
	numRepeats = 0;
	if (CurFontAttributes & fontdesc_Thin) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.weight = ISO_WEIGHT_light;
	}
	if (CurFontAttributes & fontdesc_Black) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.weight = ISO_WEIGHT_ultra_bold;
	}
	if (CurFontAttributes & fontdesc_Bold) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.weight = ISO_WEIGHT_bold;
	}
	if (numRepeats != 1){
	    printf("More than one font weight specified %X\n", CurFontAttributes & (fontdesc_Bold | fontdesc_Thin | fontdesc_Black) );
	}
    }
    else {
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_medium;
    }

    /* Postures: Italic */
    if (CurFontAttributes & fontdesc_Italic){
	fontBlk->ODAFontInfo.posture = ISO_POSTURE_italic;
    }
    else {
	fontBlk->ODAFontInfo.posture = ISO_POSTURE_upright;
    }

    /* Structure: outline, shadow */
    if (CurFontAttributes & (fontdesc_Shadow | fontdesc_Outline)){
	numRepeats = 0;
	if (CurFontAttributes & fontdesc_Shadow) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.structur = ISO_STRUCTUR_shadow;
	}
	if (CurFontAttributes & fontdesc_Outline) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.structur = ISO_STRUCTUR_outline;
	}
	if (numRepeats != 1) {
	    printf("More than one font strutur specified %X\n", CurFontAttributes & (fontdesc_Shadow | fontdesc_Outline));
	}
    }

    /* Escapements */
    fontBlk->ODAFontInfo.modes.specified = BOOL_true;
    fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
    if (CurFontAttributes & fontdesc_Fixed){
	fontBlk->ODAFontInfo.modes.iso_0.esclass = ISO_ESCLASS_fixed;
    }
    else {
	fontBlk->ODAFontInfo.modes.iso_0.esclass = ISO_ESCLASS_variable;
    }

    /* Property width */
    if ( CurFontAttributes & 
         (fontdesc_Medium | fontdesc_Heavy | fontdesc_Condense )){
	numRepeats = 0;
	if (CurFontAttributes & fontdesc_Medium) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_medium;
	}
	if (CurFontAttributes & fontdesc_Heavy) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_ultra_condensed;
	}
	if (CurFontAttributes & fontdesc_Condense) {
	    numRepeats++;
	    fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_condensed;
	}
	if (numRepeats != 1) {
	    printf("More than one font propwdth specified %X\n", CurFontAttributes & 
         (fontdesc_Medium | fontdesc_Heavy | fontdesc_Condense ));
	}
    }
    else {
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_medium;
    }

    if (gD->doDebugging) {
	printf("Leaving FillInNewFace %d\n", CurFontAttributes);
    }

    return rval;
}

INT_type CalcFaceChanges(gD, fontBlk, curStyle, styleSV)
struct atkoda * gD;
p_font_list_blk	fontBlk;
struct style * curStyle;
struct text_statevector * styleSV;
{

    INT_type rval;
    INT_type	numRepeats;

    if (gD->doDebugging) {
	printf("Entering CalcFaceChanges\n");
    }

    /* Note: for all of these, a better translation would be to allow any one specification with any number of deletions (since the addition implicitly overrides the deletion, but since not everything can be translated exactly, a warning will come out instead. */

    /* Weights: Bold */
    numRepeats = 0;
    /* Added weights? */
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Bold)) {
	numRepeats++;
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_bold;
    }
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Thin)) {
	numRepeats++;
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_light;
    }
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Black)) {
	numRepeats++;
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_ultra_bold;
    }
    /* removed weights? */
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Bold)) {
	numRepeats++;
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_medium;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Thin)) {
	numRepeats++;
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_medium;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Black)) {
	numRepeats++;
	fontBlk->ODAFontInfo.weight = ISO_WEIGHT_medium;
    }
    /* Make sure only one thing specified */
    if (numRepeats > 1){
	printf("More than one font weight specified in style\n");
    }


    /* Postures: Italic */
    numRepeats = 0;
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Italic)){
	numRepeats++;
	fontBlk->ODAFontInfo.posture = ISO_POSTURE_italic;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Italic)){
	numRepeats++;
	fontBlk->ODAFontInfo.posture = ISO_POSTURE_upright;
    }
    if (numRepeats > 1) {
	printf("More than one font posture specified in style\n");
    }

    /* Structure: outline, shadow */
    numRepeats = 0;
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Shadow)) {
	numRepeats++;
	fontBlk->ODAFontInfo.structur = ISO_STRUCTUR_shadow;
    }
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Outline)) {
	numRepeats++;
	fontBlk->ODAFontInfo.structur = ISO_STRUCTUR_outline;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Shadow)) {
	numRepeats++;
	fontBlk->ODAFontInfo.structur = ISO_STRUCTUR_solid;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Outline)) {
	numRepeats++;
	fontBlk->ODAFontInfo.structur = ISO_STRUCTUR_solid;
    }

    if (numRepeats > 1) {
	printf("More than one font strutur specified\n");
    }

    /* Escapements */
    numRepeats = 0;
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Fixed)){
	numRepeats++;
	fontBlk->ODAFontInfo.modes.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.esclass = ISO_ESCLASS_fixed;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Fixed)){
	numRepeats++;
	fontBlk->ODAFontInfo.modes.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.esclass = ISO_ESCLASS_variable;
    }
    if (numRepeats > 1) {
	printf("More than one font escapement specified\n");
    }

    /* Property width */
    numRepeats = 0;
    /* Added widths*/
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Medium)) {
	numRepeats++;
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_medium;
    }
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Heavy)) {
	numRepeats++;
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_ultra_condensed;
    }
    if (style_IsAnyAddedFontFace(curStyle,fontdesc_Condense)) {
	numRepeats++;
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_condensed;
    }
    /* Removed widths */
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Medium)) {
	numRepeats++;
	/* This is strange -- how to unspecify a default? */
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_unspec;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Heavy)) {
	numRepeats++;
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_medium;
    }
    if (style_IsAnyRemovedFontFace(curStyle,fontdesc_Condense)) {
	numRepeats++;
	fontBlk->ODAFontInfo.propwdth = ISO_PROPWDTH_medium;
    }

    if (numRepeats > 1) {
	printf("More than one font propwdth specified\n");
    }


    if (gD->doDebugging) {
	printf("Leaving CalcFaceChanges\n");
    }

    return rval;
}


INT_type FillInNewFlags(gD, fontBlk, SpecialFlags)
struct atkoda * gD;
p_font_list_blk	fontBlk;
long SpecialFlags;
{

    INT_type rval;

    if (gD->doDebugging) {
	printf("Entering FillInNewFlags %d\n", SpecialFlags);
    }


    /* Scores */
    if (SpecialFlags & style_Underline){
	fontBlk->ODAFontInfo.modes.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.scores.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.scores.rscore.specified = BOOL_true;
	/* Note that according to page 3 of part 5 of 9541, unspecified measurements are in terms of body size ratios. Thus I will arbitrarily select the thickness to be 1/12 of the body size (1 point for a 12 point font) and the distance to be 1/6 of the body size (2 pts down for a 12 point font) */
	fontBlk->ODAFontInfo.modes.iso_0.scores.rscore.scorpos.first = 1;
	fontBlk->ODAFontInfo.modes.iso_0.scores.rscore.scorpos.second = 6;
	fontBlk->ODAFontInfo.modes.iso_0.scores.rscore.scorthik.first = 1;
	fontBlk->ODAFontInfo.modes.iso_0.scores.rscore.scorthik.second = 12;
    }
    if (SpecialFlags & style_OverBar) {
	fontBlk->ODAFontInfo.modes.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.scores.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.scores.lscore.specified = BOOL_true;
	/* Note that according to page 3 of part 5 of 9541, unspecified measurements are in terms of body size ratios. Thus I will arbitrarily select the thickness to be 1/12 of the body size (1 point for a 12 point font) and the distance to be 1/6 of the body size above the top of the font (2 pts above the top for a 12 point font) which means 14 points up (negative) */
	fontBlk->ODAFontInfo.modes.iso_0.scores.lscore.scorpos.first = -7;
	fontBlk->ODAFontInfo.modes.iso_0.scores.lscore.scorpos.second = 6;
	fontBlk->ODAFontInfo.modes.iso_0.scores.lscore.scorthik.first = 1;
	fontBlk->ODAFontInfo.modes.iso_0.scores.lscore.scorthik.second = 12;
    }

    if (gD->doDebugging) {
	printf("Leaving FillInNewFlags %d\n", SpecialFlags);
    }

    return rval;
}


INT_type FillInNewSubSuper(gD, fontBlk, ScriptMovement, CurFontSize)
struct atkoda * gD;
p_font_list_blk	fontBlk;
long ScriptMovement;
long CurFontSize;
{

    INT_type rval;

    if (gD->doDebugging) {
	printf("Entering FillInNewSubSuper %d\n", ScriptMovement);
    }

    if (ScriptMovement < 0){
	/* super script */
	fontBlk->ODAFontInfo.modes.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.lscript.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.lscript.vyoffset.first = -ScriptMovement;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.lscript.vyoffset.second = CurFontSize;
    }
    else if (ScriptMovement > 0) {
	/* subscript */
	fontBlk->ODAFontInfo.modes.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.rscript.specified = BOOL_true;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.rscript.vyoffset.first = -ScriptMovement;
	fontBlk->ODAFontInfo.modes.iso_0.varscrpt.rscript.vyoffset.second = CurFontSize;
    }

    if (gD->doDebugging) {
	printf("Leaving FillInNewSubSuper %d\n", ScriptMovement);
    }

    return rval;
}

#define	FontFamilySize	100
INT_type CalcStyleAttrFont(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    char		styleFontFamily[FontFamilySize];
    enum style_FontSize specFontBasis;
    long		specFontOperand;
    BOOL_type		UsingBold;
    struct text_statevector *styleSV;
    p_font_list_blk	fontBlk;
    cc_CHARACTER_FONTS_type newFont;
    enum style_ScriptMovement specScriptBasis;
    long		specScriptOperand;
    enum style_Unit	specScriptUnit;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrFont\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

    /* This is a tricky one. If anything in the style is specified, then it likely that the whole specification will have to be changed */

    /* For now, do name, size, bold and italic */
    style_GetFontFamily(curStyle, styleFontFamily, FontFamilySize);
    style_GetFontSize(curStyle, &specFontBasis, &specFontOperand);
    style_GetFontScript(curStyle, &specScriptBasis, &specScriptOperand, &specScriptUnit);

    /* See if anything changed in style */
    if (  /* family? */
	 (styleFontFamily[0] != '\0') || 
	  /* size? */
         (specFontBasis	!= style_PreviousFontSize) || (specFontOperand != 0) ||
	  /* attributes/faces? */
	  (style_GetAddedFontFaces(curStyle) != (long) fontdesc_Plain) || (style_GetRemovedFontFaces(curStyle) != (long) ~fontdesc_Plain) ||
	  /* underline/overbar? */
	  (style_IsUnderlineAdded(curStyle) || style_IsUnderlineRemoved(curStyle) ||
	   style_IsOverBarAdded(curStyle) || style_IsOverBarRemoved(curStyle) ) ||
	  /* sub/super scripts */
	  (specScriptBasis != style_PreviousScriptMovement) || (specScriptOperand != 0) 
	  ){
	/* process font changes -- have to build it from actual SV, check it against font list of generated fonts, add it to list in character fonts, and place it in front of the content information */
	/* Do it piece by piece. First, see if a font descriptor with all necessary information exists */
	fontBlk = atkoda_FindFontFromStyle(gD,curStyle);
	if(!fontBlk) {

	    /* Doesn't exist, so we must make one */
	    fontBlk = atkoda_MakeNewStyleBlock(gD, /* From style */ BOOL_true);

	    /* Copy over the style fields for later comparison */
	    CopyStyleIntoFontBlock(gD, fontBlk, curStyle);

	    /* Fill in the ODA fields, one by one */

	    /* First, any name change? */
	    if (styleFontFamily[0] != '\0') {
		numChanges++;
		if (gD->doDebugging) {
		    printf("New font name for style is (%X) %s\n", styleFontFamily, styleFontFamily);
		}
		FillInNewFamily(gD, fontBlk, styleFontFamily);
	    }

	    /* Any size change? */
	    if ((specFontBasis != style_PreviousFontSize) || (specFontOperand != 0) ) {
		numChanges++;
		FillInNewSize(gD, fontBlk, styleSV->CurFontSize);
	    }

	    /* Any face code (attribute) changes? */
	   if ((style_GetAddedFontFaces(curStyle) != (long) fontdesc_Plain) || (style_GetRemovedFontFaces(curStyle) != (long) ~fontdesc_Plain)) {
	       numChanges++;
	       CalcFaceChanges(gD, fontBlk, curStyle, styleSV);
	   }

	  /* underline/overbar? */
	  if (style_IsUnderlineAdded(curStyle) || style_IsUnderlineRemoved(curStyle) ||
	   style_IsOverBarAdded(curStyle) || style_IsOverBarRemoved(curStyle)) {
	      numChanges++;
	      FillInNewFlags(gD, fontBlk, styleSV->SpecialFlags);
	  }

	  /* sub/super scripts */
	  if ((specScriptBasis != style_PreviousScriptMovement) || (specScriptOperand != 0) ) {
	      numChanges++;
	      FillInNewSubSuper(gD, fontBlk, styleSV->CurScriptMovement, styleSV->CurFontSize);
	  }


	}

	/* All finished putting together the changes in the style, so now make a cc_character_fonts on the style to record it! */
	newFont = (cc_CHARACTER_FONTS_type) odalib_MakeSequence(SEQUENCE_INT_PAIR_tag, 1);
	newFont->sequence_value.int_pairs[0].first = atkoda_CVDotstoSMU(gD,styleSV->CurFontSize);
	newFont->sequence_value.int_pairs[0].second = fontBlk->ODAFontID;
	odalib_SetAttr(newStyleBlk->presStyle, cc_CHARACTER_FONTS, (POINTER_type) newFont, Ignore_Parms);
	odalib_DeleteSequence(newFont);
    }

#if 0
    char *FontFamily;
    struct fontsizestyle FontSize;
    struct fontscriptstyle FontScript;
    long AddFontFaces;      /* OR'ed FontFaces */
    long OutFontFaces;      /* NOT'ed OR'ed FontFaces */
#endif

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrFont\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrMisc(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    struct text_statevector *styleSV;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrMisc\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

#if 0
    long AddMiscFlags;      /* Hinge, Pagebreak, Hidden, ... */
    long OutMiscFlags;
#endif
    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrMisc\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrCounter(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    struct text_statevector *styleSV;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrCounter\n");
    }

#if 0
    char *CounterName;
#endif
    numChanges = 0;
    styleSV = &newStyleBlk->SV;

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrCounter\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrTabs(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
 {

    INT_type		numChanges;
    struct text_statevector *styleSV;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrTabs\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

#if 0
    long NumTabChanges;
#endif

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrTabs\n");
    }

    return numChanges;
}

INT_type CalcStyleAttrInformation(gD, newStyleBlk, curStyle)
struct atkoda * gD;
p_styleSheetInfo newStyleBlk;
struct style * curStyle;
{

    INT_type		numChanges;
    struct text_statevector *styleSV;

    if (gD->doDebugging) {
	printf("Entering CalcStyleAttrInformation\n");
    }

    numChanges = 0;
    styleSV = &newStyleBlk->SV;

#if 0
    struct namespace *AdditionalAttributes;
#endif

    if (gD->doDebugging) {
	printf("Leaving CalcStyleAttrInformation\n");
    }

    return numChanges;
}

/* This procedure is lifted from ATK's style.c file. It should be a
   supported interface */

static long CVDots(amt, unit)
register long amt;
enum style_Unit unit;
{
    switch (unit) {
        case style_RawDots:
            return amt;
        case style_Inches:
            return (amt * 72) >> 16;
        case style_CM:
            return (amt * 28) >> 16;
        case style_Points:
            return amt;
        case style_Ems:     /* amt * 10? */
            return 0;
        case style_Lines:   /* amt * 10? */
            return 0;
    }

    return 0;
}


at_OFFSET_type * GetCurOffset(gD,LayoutStyle,returnMask)
struct atkoda * gD;
CONSTITUENT LayoutStyle;
PARM_MASK_type * returnMask; {
    at_OFFSET_type * rval;

    if (gD->doDebugging) {
	printf("Entering GetCurOffset\n");
    }

    if (HAS_ATTR == odalib_HasAttr(LayoutStyle, at_OFFSET)) {
	odalib_GetAttr(LayoutStyle, at_OFFSET, (POINTER_type) &rval, Use_Local_Value, returnMask);
	if (*returnMask == ALL_PARMS) {
	    *returnMask = (PARM_MASK_type) (LEADING_OFFSET_parm | TRAILING_OFFSET_parm | LEFT_HAND_OFFSET_parm | RIGHT_HAND_OFFSET_parm);
	}
    }
    else {
	rval = (at_OFFSET_type *) malloc(sizeof(at_OFFSET_type));
	*returnMask = (PARM_MASK_type) 0;
    }

    if (gD->doDebugging) {
	printf("Leaving GetCurOffset\n");
    }

    return rval;

}

