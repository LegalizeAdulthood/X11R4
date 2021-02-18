/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkstylediff.c,v 1.3 89/10/12 15:39:18 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkstylediff.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkstylediff.c,v 1.3 89/10/12 15:39:18 ghoti Exp $";
#endif

/*
  $Log:	mkstylediff.c,v $
 * Revision 1.3  89/10/12  15:39:18  ghoti
 * BUG: 49 (#ifdef 0 -> #if 0)
 * 
 * Revision 1.2  89/09/08  17:29:39  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:21:29  mss
 * Initial revision
 * 
 * Revision 1.12  89/05/20  20:44:01  mss
 * Changed style attribute setting
 * 
 * Revision 1.11  89/04/30  12:07:48  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.10  89/03/27  21:15:28  mss
 * Shortened parameter name
 * 
 * Revision 1.9  89/03/23  07:07:57  mss
 * Uses new component and dvl specifications;
 * little cleanup on debugging output
 * 
 * Revision 1.8  88/11/18  16:00:45  mss
 * Fixed problem where ATK uses negative margins
 * 
 * Revision 1.7  88/11/01  15:59:48  mss
 * Added acrhg class to fake BLO for use in DVL when
 * overriding style information
 * 
 * Revision 1.6  88/10/31  21:31:59  mss
 * Added more style function (fonts/margins)
 * 
 * Revision 1.5  88/10/25  15:37:42  mss
 * Added more style processing/default processing
 * 
 * Revision 1.4  88/10/21  15:05:38  mss
 * Fixed bug of using styles instead of default value lists
 * 
 * Revision 1.3  88/10/17  15:56:54  mss
 * Fixed order of decl problem
 * 
 * Revision 1.2  88/10/17  11:59:12  mss
 * 
 * 
 * Revision 1.1  88/09/22  11:16:12  mss
 * Initial revision
 * 

*/

/* Procedure for calculating the differences between state vectors, given a style specification */

#include <stdio.h>
#include <class.h>
#include <odatk.h>
#include <odalib.ih>
#include <styles.h>
#include <attsup.h>
#include <odadef.h>

#include <atkoda.ih>
#include <text.ih>
#include <txtstvec.h>
#include <style.ih>
#include <fontdesc.ih>
#include <mkstylediff.h>

INT_type CompensateState(gD, actualSV, curStyle, newPassage, newPhrase)
struct atkoda * gD;
struct text_statevector * actualSV;
struct style * curStyle; 
CONSTITUENT newPassage;
CONSTITUENT newPhrase; {

    /* Need to see what differences are apparent among the possible things to change, and add the attributes to the composite as appropriate. Note: neither the font information nor the page information can be saved here, but has to persist to the basic logical object level. The question becomes, when processing a BLO, does one use the passage or phrase version? I believe the answer is, whichever one the new BLO is being attached to */

    INT_type	numChanges;
    struct text_statevector * styleSV;
    p_styleSheetInfo curStyleBlk;

    if (gD->doDebugging) {
	printf("Entering CompensateState\n");
    }

    numChanges = 0;
    curStyleBlk = LookupSSInfo(gD, curStyle);
    styleSV = &curStyleBlk->SV;

    numChanges = CalcAuxiliaryAttributes(gD, newPassage, newPhrase, curStyle, styleSV, actualSV);

    if (gD->doDebugging) {
	printf("Leaving CompensateState\n");
    }

    return numChanges;
}


INT_type CalcAuxiliaryAttributes(gD, curPassage, curPhrase, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT curPassage, curPhrase;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type			numChanges;
    at_DEFAULT_VALUE_LISTS_type overrideList;
    CONSTITUENT			fakeBLO;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryAttributes\n");
    }

    numChanges = 0;

    /* Create the fake BLO for holding the overriding values */
    if (odalib_InitializeDefaultValueLists(gD->outputDocument, &overrideList, BAS_LOGICAL_ATTRIBUTES_parm) == ERROR_INT) {
	printf("CalcAuxiliaryAttributes: Cannot initialize DVL for overriding generic values\n");
	goto LEAVE;
    }
    fakeBLO = overrideList.basic_logical_attributes;

    /* Mark fake blo as a text object, Sigh. This gets around a bug in inheritance */
    (void) odalib_SetAttr(fakeBLO, at_CONTENT_ARCH_CLASS, (POINTER_type) odalib_at_CONTENT_ARCH_CLASS_proc_cc(), ALL_PARMS);

    /* Go through each possible style attribute and see if anything to do */

    /* First, have to take care of changes to offset */
    numChanges += CalcAuxiliaryMargins(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Page: top margin,bottom margin, header, footer */
    numChanges += CalcAuxiliaryPage(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Justification */
    numChanges += CalcAuxiliaryJust(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* New Line spacing */
    numChanges += CalcAuxiliaryLSpacing(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* New paragraph spread */
    numChanges += CalcAuxiliaryPSpread(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Font calculations */
    numChanges += CalcAuxiliaryFont(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Tab calculations */
    numChanges += CalcAuxiliaryTabs(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Misc flags */
    numChanges += CalcAuxiliaryMisc(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Counter calculations */
    numChanges += CalcAuxiliaryCounter(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* Information read in from elsewhere */
    numChanges += CalcAuxiliaryInformation(gD, fakeBLO, curStyle, styleSV, actualSV);

    /* See if any changes were made */
    if (numChanges != 0) {
	/* Yes, so make the default value list as appropriate */
	if (curPassage) {
	    odalib_SetAttr(curPassage, at_DEFAULT_VALUE_LISTS, (POINTER_type) &overrideList, Ignore_Parms);
	}
	if (curPhrase) {
	    odalib_SetAttr(curPhrase, at_DEFAULT_VALUE_LISTS, (POINTER_type) &overrideList, Ignore_Parms);
	}
    }
    else {
	odalib_DeleteConstituent(fakeBLO);
    }

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryAttributes with %d changes\n", numChanges);
    }

    return numChanges;

}

INT_type CalcAuxiliaryMargins(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {
    INT_type		    numChanges;
    at_OFFSET_type	    * newSharedOffset;
    PARM_MASK_type	    newOffsetParms;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryMargins\n");
    }

    numChanges = 0;
    newSharedOffset = (at_OFFSET_type * ) NULL;
    newOffsetParms = (PARM_MASK_type) 0;

    /* Left margin and indentation (related) */
    numChanges += CalcAuxiliaryLMandIndent(gD, fakeBLO, curStyle, styleSV, actualSV, &newSharedOffset, &newOffsetParms);

    /* Right margin */
    numChanges += CalcAuxiliaryRM(gD, fakeBLO, curStyle, styleSV, actualSV, &newSharedOffset, &newOffsetParms);

    /* And make the changes */
    if (newSharedOffset) {
	odalib_SetLayoutAttr(fakeBLO, at_OFFSET, (POINTER_type) newSharedOffset, newOffsetParms);
	free(newSharedOffset);
    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryMargins\n");
    }

    return numChanges;


}

INT_type CalcAuxiliaryLMandIndent(gD, fakeBLO, curStyle, styleSV, actualSV, newSharedOffset, newOffsetParms)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; 
at_OFFSET_type ** newSharedOffset;
PARM_MASK_type * newOffsetParms;
{

    INT_type		    numChanges;
    enum style_MarginValue  NewLMarginBasis, NewIndentationBasis;
    long		    NewLMarginOperand, NewIndentationOperand;
    enum style_Unit	    NewLMarginUnit, NewIndentationUnit;
    INT_type		    oda_compensation;
    INT_type		    OldIndentationPAttr, NewIndentationPAttr;
    INT_type		    OldRHOffset, NewRHOffset;
    INT_type		    OldFLOffset, NewFLOffset;
    INT_type		    scaledFLOffset, scaledIndentationPAttr;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryLMandIndent\n");
    }

    numChanges = 0;

    /* Get the old values */

    style_GetNewLeftMargin(curStyle, &NewLMarginBasis, &NewLMarginOperand, &NewLMarginUnit);
    style_GetNewIndentation(curStyle, &NewIndentationBasis, &NewIndentationOperand, &NewIndentationUnit);

    /* See if any changes specified that resulted in differing state vectors */
    if ( /* Any style changes? */
	 (NewLMarginBasis != style_LeftMargin || NewLMarginOperand != 0 || NewIndentationBasis != style_PreviousIndentation || NewIndentationOperand != 0) &&
	 /* Along with state vector changes */
	 (styleSV->CurLeftMargin != actualSV->CurLeftMargin || styleSV->CurIndentation != actualSV->CurIndentation) ) {
	/* Adjust the local values appropriately -- since left margin and paragraph indentation are ganged together, must calculate what they would look like in terms of ODA terms and only add those that are different */

	/* First, old values */
	oda_compensation = (styleSV->CurIndentation < 0) ? styleSV->CurIndentation : 0;
	OldRHOffset = styleSV->CurLeftMargin + oda_compensation;
	OldIndentationPAttr = -oda_compensation;
	OldFLOffset = styleSV->CurIndentation;

	/* And the new values */
	oda_compensation = (actualSV->CurIndentation < 0) ? actualSV->CurIndentation : 0;
	NewRHOffset = actualSV->CurLeftMargin + oda_compensation;
	NewIndentationPAttr = -oda_compensation;
	NewFLOffset = actualSV->CurIndentation;

	/* And see which have changed */
	if (OldRHOffset != NewRHOffset) {
	    numChanges++;
	    /* Add a new right hand offset, properly scaled */
	    if ((*newSharedOffset ) == NULL) {
		*newSharedOffset = (at_OFFSET_type *) malloc(sizeof(at_OFFSET_type));
	    }

	    /* Now, fixup any ATK values which are bogus */
	    if (NewRHOffset < 0) {
		if (gD->doDebugging) {
		    printf("Current Offset goes beyond left margin (negative) changing to 0\n");
		}
		NewRHOffset = 0;
	    }

	    (*newSharedOffset)->right_hand_offset = atkoda_CVDotstoSMU(gD, NewRHOffset);
	    *newOffsetParms |= RIGHT_HAND_OFFSET_parm;
	}

	if (OldIndentationPAttr != NewIndentationPAttr) {
	    numChanges++;
	    /* Add a new indentation presentation attribute, properly scaled */
	    scaledIndentationPAttr = atkoda_CVDotstoSMU(gD, NewIndentationPAttr);
	    odalib_SetPresAttr(fakeBLO, cc_INDENTATION, (POINTER_type) &scaledIndentationPAttr, ALL_PARMS);
	}


	if (OldFLOffset != NewFLOffset) {
	    numChanges++;
	    /* Add a new first line offset, properly scaled */
	    scaledFLOffset = atkoda_CVDotstoSMU(gD,NewFLOffset);
	    odalib_SetPresAttr(fakeBLO, cc_1ST_LINE_OFFSET, (POINTER_type) &scaledFLOffset, ALL_PARMS);
	}

    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryLMandIndent\n");
    }

    return numChanges;

}

INT_type CalcAuxiliaryRM(gD, fakeBLO, curStyle, styleSV, actualSV, newSharedOffset, newOffsetParms)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV;
at_OFFSET_type ** newSharedOffset;
PARM_MASK_type * newOffsetParms;
 {

    INT_type		    numChanges;
    enum style_MarginValue  NewRMarginBasis;
    long		    NewRMarginOperand;
    enum style_Unit	    NewRMarginUnit;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryRM\n");
    }

    style_GetNewRightMargin(curStyle, &NewRMarginBasis, &NewRMarginOperand, &NewRMarginUnit);
    /* See if any changes specified that resulted in differing state vectors */
    if ( /* Any style changes? */
	 (NewRMarginBasis != style_RightMargin || NewRMarginOperand != 0) &&
	 /* Along with state vector changes */
	 (styleSV->CurRightMargin != actualSV->CurRightMargin) ) {

	/* Set the new lefthand offset attribute, properly scaled. */
	if ((*newSharedOffset ) == NULL) {
	    *newSharedOffset = (at_OFFSET_type *) malloc(sizeof(at_OFFSET_type));
	}
	(*newSharedOffset)->left_hand_offset = atkoda_CVDotstoSMU(gD, actualSV->CurRightMargin);

	/* Check for ATK weirdness */
	if ((*newSharedOffset)->left_hand_offset < 0) {
	    if (gD->doDebugging) {
		printf("Negative right margin (%d) being rounded to 0\n",(*newSharedOffset)->left_hand_offset );
	    }
	    (*newSharedOffset)->left_hand_offset = 0;
	}

	*newOffsetParms |= LEFT_HAND_OFFSET_parm;
	numChanges++;
    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryRM\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryPage(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
    enum style_MarginValue  NewBMarginBasis, NewTMarginBasis;
    long		    NewBMarginOperand, NewTMarginOperand;
    enum style_Unit	    NewBMarginUnit, NewTMarginUnit;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryPage\n");
    }

    numChanges = 0;

    style_GetNewBottomMargin(curStyle, &NewBMarginBasis, &NewBMarginOperand, &NewBMarginUnit);
    style_GetNewTopMargin(curStyle, &NewTMarginBasis, &NewTMarginOperand, &NewTMarginUnit);
    /* See if any changes specified that resulted in differing state vectors */
    if ( /* Any style changes? */
	 (NewBMarginBasis != style_BottomMargin || NewBMarginOperand != 0 || NewTMarginBasis != style_TopMargin || NewTMarginOperand != 0) &&
	 /* Along with state vector changes */
	 (styleSV->CurTopMargin != actualSV->CurTopMargin || styleSV->CurBottomMargin != actualSV->CurBottomMargin) ) {

	/* Reset the pages margins -- means a new generic page, layout category, and such */
	printf("Unexpected top and bottom margin changes in ATK document for style %s, style Top %d, actual Top %d, style Bottom %d, actual Bottom %d\n", curStyle->name, styleSV->CurTopMargin, actualSV->CurTopMargin, styleSV->CurBottomMargin, actualSV->CurBottomMargin);
    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryPage\n");
    }

    return numChanges;

}

INT_type CalcAuxiliaryJust(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type			numChanges;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryJust\n");
    }

    numChanges = 0;

    if (style_GetJustification(curStyle) != style_PreviousJustification) {
	/* A change was requested -- in theory it is impossible for the FSV's to disagree since there is no such thing as a relative justification in Andrew, but we will check anyway */
	if (styleSV->CurJustification != actualSV->CurJustification) {
	    printf("Internal error: justification mistmatch. Style: %d, StyleSV %d, ActualSV %d\n", style_GetJustification(curStyle), styleSV->CurJustification, actualSV->CurJustification);
	}
    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryJust\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryLSpacing(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
    enum style_SpacingValue  specSpacingBasis;
    long		     specSpacingOperand;
    enum style_Unit	     specSpacingUnit;
    long CurSpacing;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryLSpacing\n");
    }

    numChanges = 0;


    style_GetNewInterlineSpacing(curStyle, &specSpacingBasis, &specSpacingOperand, &specSpacingUnit);
    if ((specSpacingBasis != style_InterlineSpacing || specSpacingOperand != 0) &&
	 (styleSV->CurSpacing != actualSV->CurSpacing)) {
	numChanges++;
	/* We have a relative value for interline spacing, set it accordingly with proper scaling*/
	/* Note: this could be tricky, since it means defining a new spacing and inserting it as a content information prefix */
    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryLSpacing\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryPSpread(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
    enum style_SpacingValue IPSpacing, AboveSpacing, BelowSpacing;
    long		    IPOperand, AboveOperand, BelowOperand;
    enum style_Unit	    IPUnit, AboveUnit, BelowUnit;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryPSpread\n");
    }

    numChanges = 0;

    style_GetNewInterparagraphSpacing(curStyle, &IPSpacing, &IPOperand, &IPUnit);
    style_GetNewAbove(curStyle, &AboveSpacing, &AboveOperand, &AboveUnit);
    style_GetNewBelow(curStyle, &BelowSpacing, &BelowOperand, &BelowUnit);

    if ((AboveSpacing != style_AboveSpacing || AboveOperand != 0 || 
	 BelowSpacing != style_BelowSpacing || BelowOperand != 0 ||
	 IPSpacing != style_InterparagraphSpacing || IPOperand != 0 ) &&
	 (styleSV->CurSpread != actualSV->CurSpread) ){
	/* Have a mismatch again, see which one does not match. This is a pain, since the state vector only contains a summary, and not above or below specific information */
    }

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryPSpread\n");
    }

    return numChanges;
}

#define	FontFamilySize	100
INT_type CalcAuxiliaryFont(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
    char		styleFontFamily[FontFamilySize];
    enum style_FontSize specFontBasis;
    long		specFontOperand;
    BOOL_type		UsingBold;

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryFont\n");
    }
    numChanges = 0;

    /* This is a tricky one. If anything in the style is specified, then it likely that the whole specification will have to be changed */

    /* This is unnecessary -- fonts always specified on each BLO, so this can be skipped */
    /* For now, do name, size, bold and italic */
    style_GetFontFamily(curStyle, styleFontFamily, FontFamilySize);
    style_GetFontSize(curStyle, &specFontBasis, &specFontOperand);
    /* See if anything changed in style */
    if ( (styleFontFamily[0] != '\0' || (specFontBasis	!= style_PreviousFontSize) || (specFontOperand != 0) || (style_IsAnyAddedFontFace(curStyle, (fontdesc_Italic | fontdesc_Bold))) || (style_IsAnyRemovedFontFace(curStyle,(fontdesc_Italic | fontdesc_Bold)))) &&
	  /* anything in fsv changed? */
	 ( 0 != strcmp(styleSV->CurFontFamily,actualSV->CurFontFamily) || styleSV->CurFontSize != actualSV->CurFontSize || styleSV->CurFontAttributes != actualSV->CurFontAttributes) ){
	/* process font changes -- have to build it from actual SV, check it against font list of generated fonts, add it to list in character fonts, and place it in front of the content information */
    }

#if 0
    char *FontFamily;
    struct fontsizestyle FontSize;
    struct fontscriptstyle FontScript;
    long AddFontFaces;      /* OR'ed FontFaces */
    long OutFontFaces;      /* NOT'ed OR'ed FontFaces */
#endif

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryFont\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryMisc(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;

#if 0
    long AddMiscFlags;      /* Hinge, Pagebreak, Hidden, ... */
    long OutMiscFlags;
#endif

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryMisc\n");
    }
    numChanges = 0;
    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryMisc\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryCounter(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
#if 0
    char *CounterName;
#endif

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryCounter\n");
    }

    numChanges = 0;

    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryCounter\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryTabs(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
#if 0
    long NumTabChanges;
#endif

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryTabs\n");
    }
    numChanges = 0;


    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryTabs\n");
    }

    return numChanges;
}

INT_type CalcAuxiliaryInformation(gD, fakeBLO, curStyle, styleSV, actualSV)
struct atkoda * gD;
CONSTITUENT fakeBLO;
struct style * curStyle;
struct text_statevector * styleSV;
struct text_statevector * actualSV; {

    INT_type		numChanges;
#if 0
    struct namespace *AdditionalAttributes;
#endif

    if (gD->doDebugging) {
	printf("Entering CalcAuxiliaryInformation\n");
    }
    numChanges = 0;


    if (gD->doDebugging) {
	printf("Leaving CalcAuxiliaryInformation\n");
    }

    return numChanges;
}
