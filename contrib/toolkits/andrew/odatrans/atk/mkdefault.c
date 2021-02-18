/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkdefault.c,v 1.3 89/10/12 15:38:58 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkdefault.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkdefault.c,v 1.3 89/10/12 15:38:58 ghoti Exp $";
#endif

/*
  $Log:	mkdefault.c,v $
 * Revision 1.3  89/10/12  15:38:58  ghoti
 * BUG: 49 (#ifdef 0 -> #if 0)
 * 
 * Revision 1.2  89/09/08  17:28:03  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:21:42  mss
 * Initial revision
 * 
 * Revision 1.6  89/04/30  12:07:08  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.5  88/11/18  15:57:38  mss
 * Fixed problem where ATK uses negative margins
 * 
 * Revision 1.4  88/11/17  13:46:09  mss
 * Added generation of separation, line spacing,
 * first line offset and indentation (as appropriate)
 * 
 * Revision 1.3  88/11/08  14:53:21  mss
 * Changed some flags; patched style analysis so
 * that outbound has reasonable margins, inbound
 * none
 * 
 * Revision 1.2  88/10/31  21:31:27  mss
 * Added more style function (fonts/margins)
 * 
 * Revision 1.1  88/10/26  09:16:57  mss
 * Initial revision
 * 

*/

/* Procedures for calculating the default environment */

#include <stdio.h>
#include <class.h>
#include <odatk.h>
#include <odalib.ih>
#include <styles.h>
#include <attsup.h>
#include <odadef.h>

#include <atkoda.ih>
#include <breakup.h>
#include <text.ih>
#include <txtstvec.h>
#include <style.ih>
#include <fontdesc.ih>
#include <mkdefault.h>

INT_type MakeDefaultState(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;	/* "style" block of the global environment */
{

    /* Rearrange the parametes into a more palatable form, and go for it */

    INT_type	numChanges;

    if (gD->doDebugging) {
	printf("Entering MakeDefaultState\n");
    }

    numChanges = 0;

    if (gD->doDebugging) {
	printf("Font family %s\n", actualSV->CurFontFamily ? actualSV->CurFontFamily: "<none>");
	printf("Font size %d\n", actualSV->CurFontSize);
	printf("Left margin %d\n", actualSV->CurLeftMargin);
	printf("Right margin %d\n", actualSV->CurRightMargin);
	printf("Indentation %d\n", actualSV->CurIndentation);
	printf("Justification %d\n", actualSV->CurJustification);
    }


    numChanges = CreateAuxiliaryAttributes(gD, actualSV, styleBlk);

    if (gD->doDebugging) {
	printf("Leaving MakeDefaultState\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryAttributes(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV; 
p_styleSheetInfo styleBlk;
{

    INT_type			numChanges;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryAttributes\n");
    }

    numChanges = 0;

    /* Go through each possible style attribute and see if anything to do */

    /* First, have to take care of changes to offset */

    numChanges += CreateAuxiliaryMargins(gD, actualSV, styleBlk);

    /* Page: top margin,bottom margin, header, footer */
    numChanges += CreateAuxiliaryPage(gD, actualSV, styleBlk);

    /* Justification */
    numChanges += CreateAuxiliaryJust(gD, actualSV, styleBlk);

    /* New Line spacing */
    numChanges += CreateAuxiliaryLSpacing(gD, actualSV, styleBlk);

    /* New paragraph spread */
    numChanges += CreateAuxiliaryPSpread(gD, actualSV, styleBlk);

    /* Font calculations */
    numChanges += CreateAuxiliaryFont(gD, actualSV, styleBlk);

    /* Tab calculations */
    numChanges += CreateAuxiliaryTabs(gD, actualSV, styleBlk);

    /* Misc flags */
    numChanges += CreateAuxiliaryMisc(gD, actualSV, styleBlk);

    /* Counter calculations */
    numChanges += CreateAuxiliaryCounter(gD, actualSV, styleBlk);

    /* Information read in from elsewhere */
    numChanges += CreateAuxiliaryInformation(gD, actualSV, styleBlk);

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryAttributes\n");
    }

    return numChanges;

}

INT_type CreateAuxiliaryMargins(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV; 
p_styleSheetInfo styleBlk;
{
    INT_type		    numChanges;
    at_OFFSET_type	    * newSharedOffset;
    PARM_MASK_type	    newOffsetParms;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryMargins\n");
    }

    numChanges = 0;
    newSharedOffset = (at_OFFSET_type * ) NULL;
    newOffsetParms = (PARM_MASK_type) 0;

    /* Left margin and indentation (related) */
    numChanges += CreateAuxiliaryLMandIndent(gD, actualSV, styleBlk, &newSharedOffset, &newOffsetParms);

    /* Right margin */
    numChanges += CreateAuxiliaryRM(gD, actualSV, styleBlk, &newSharedOffset, &newOffsetParms);

    /* And make the changes */
    if (newSharedOffset) {
	odalib_SetAttr(styleBlk->layoutStyle, at_OFFSET, (POINTER_type) newSharedOffset, newOffsetParms);
	free(newSharedOffset);
    }

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryMargins\n");
    }

    return numChanges;


}

INT_type CreateAuxiliaryLMandIndent(gD, actualSV, styleBlk, newSharedOffset, newOffsetParms)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
at_OFFSET_type	    ** newSharedOffset;
PARM_MASK_type	    * newOffsetParms;

{

    INT_type		    numChanges;
    INT_type		    oda_compensation;
    INT_type		    NewIndentationPAttr;
    INT_type		    NewRHOffset;
    INT_type		    NewFLOffset;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryLMandIndent\n");
    }

    numChanges = 0;

    /* Get the values */
    oda_compensation = (actualSV->CurIndentation < 0) ? actualSV->CurIndentation : 0;
    NewRHOffset = actualSV->CurLeftMargin + oda_compensation;

    /* See if ATK doing something funny */
    if (NewRHOffset < 0) {
	if (gD->doDebugging) {
	    printf("Negaive left margin (%d) being rounded to 0\n", NewRHOffset);
	}
	NewRHOffset = 0;
    }

    NewIndentationPAttr = -oda_compensation;
    NewFLOffset = actualSV->CurIndentation;
    if (gD->doDebugging) {
	printf("CurLeftMargin %d, curIndentation %d\n", actualSV->CurLeftMargin, actualSV->CurIndentation);
	printf("RHOffset %d, 1stOffset %d, Indentation %d\n", NewRHOffset, NewFLOffset, NewIndentationPAttr);
    }

    /* Add a new right hand offset, properly scaled */
    numChanges++;
    if ((*newSharedOffset ) == NULL) {
	*newSharedOffset = (at_OFFSET_type *) malloc(sizeof(at_OFFSET_type));
    }
    (*newSharedOffset)->right_hand_offset = atkoda_CVDotstoSMU(gD, NewRHOffset);
    *newOffsetParms |= RIGHT_HAND_OFFSET_parm;


    /* Add a new indentation presentation attribute, properly scaled */
    numChanges++;
    odalib_SetIntAttr(styleBlk->presStyle, cc_INDENTATION, atkoda_CVDotstoSMU(gD, NewIndentationPAttr));

    /* Add a new first line offset, properly scaled */
    numChanges++;
    odalib_SetIntAttr(styleBlk->presStyle, cc_1ST_LINE_OFFSET, atkoda_CVDotstoSMU(gD,NewFLOffset));

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryLMandIndent\n");
    }

    return numChanges;

}

INT_type CreateAuxiliaryRM(gD, actualSV, styleBlk, newSharedOffset, newOffsetParms)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
at_OFFSET_type ** newSharedOffset;
PARM_MASK_type * newOffsetParms;
 {

    INT_type		    numChanges;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryRM\n");
    }

    /* Set the new lefthand offset attribute, properly scaled. */
    numChanges++;
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


    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryRM\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryPage(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryPage\n");
    }

    numChanges = 0;

#if 0
	 /* Along with state vector changes */
	 (styleSV->CurTopMargin != actualSV->CurTopMargin || styleSV->CurBottomMargin != actualSV->CurBottomMargin) ) {

	/* Reset the pages margins -- means a new generic page, layout category, and such */
	printf("Unexpected top and bottom margin changes in ATK document for style %s, style Top %d, actual Top %d, style Bottom %d, actual Bottom %d\n", curStyle->name, styleSV->CurTopMargin, actualSV->CurTopMargin, styleSV->CurBottomMargin, actualSV->CurBottomMargin);
#endif

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryPage\n");
    }

    return numChanges;

}

INT_type CreateAuxiliaryJust(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type			numChanges;
    cc_ALIGNMENT_type		whichAlignment;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryJust\n");
    }

    numChanges = 0;

    switch (actualSV->CurJustification) {
	case style_LeftJustified:
	    whichAlignment = cc_ALIGNMENT_start_aligned;
	    break;
	case style_RightJustified:
	    whichAlignment = cc_ALIGNMENT_end_aligned;
	    break;
	case style_Centered:
	    whichAlignment = cc_ALIGNMENT_centred;
	    break;
        case style_LeftAndRightJustified:
        case style_LeftThenRightJustified:
	    whichAlignment = cc_ALIGNMENT_justified;
	    break;
        case style_PreviousJustification:
	default:
	    printf("Illegal/Unknown ATK justification value %d\n", actualSV->CurJustification);
	    whichAlignment = cc_ALIGNMENT_start_aligned;
    }

    odalib_SetAttr(styleBlk->presStyle, cc_ALIGNMENT, (POINTER_type) &whichAlignment, Ignore_Parms);

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryJust\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryLSpacing(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		    numChanges;
    cc_LINE_SPACING_type    defaultLSpacing;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryLSpacing\n");
    }

    numChanges = 0;


    numChanges++;
    defaultLSpacing = atkoda_CVDotstoSMU(gD,actualSV->CurSpacing);
    odalib_SetIntAttr(styleBlk->presStyle, cc_LINE_SPACING, defaultLSpacing);

#if 0
	/* We have a relative value for interline spacing, set it accordingly with proper scaling*/
	/* Note: this could be tricky, since it means defining a new spacing and inserting it as a content information prefix */
	 (styleSV->CurSpacing != actualSV->CurSpacing)
#endif

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryLSpacing\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryPSpread(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;
    at_SEPARATION_type	defaultSeparation;
    PARM_MASK_type	sepMask;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryPSpread\n");
    }

    numChanges = 0;

    numChanges++;
    defaultSeparation.leading_edge = atkoda_CVDotstoSMU(gD,actualSV->CurSpread);
    defaultSeparation.trailing_edge = atkoda_CVDotstoSMU(gD,actualSV->CurSpread);
    sepMask = LEADING_EDGE_parm	| TRAILING_EDGE_parm;
    odalib_SetAttr(styleBlk->layoutStyle, at_SEPARATION, (POINTER_type) &defaultSeparation, sepMask);
#if 0
	 (styleSV->CurSpread != actualSV->CurSpread) ){
	/* Have a mismatch again, see which one does not match. This is a pain, since the state vector only contains a summary, and not above or below specific information */
#endif

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryPSpread\n");
    }

    return numChanges;
}

#define	FontFamilySize	100
INT_type CreateAuxiliaryFont(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;
    p_font_list_blk	fontBlk;
    cc_CHARACTER_FONTS_type defaultFont;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryFont\n");
    }

    numChanges = 0;

    /* Take care of the document profile parts */
    numChanges++;
    fontBlk = atkoda_FindFontFromSV(gD, actualSV);
    if (!fontBlk) {
	fontBlk = MakeNewFontFromSV(gD, actualSV);
    }

    /* And fill in the cc_character_fonts part */
    defaultFont = (cc_CHARACTER_FONTS_type) odalib_MakeSequence(SEQUENCE_INT_PAIR_tag, 1);
    defaultFont->sequence_value.int_pairs[0].first = atkoda_CVDotstoSMU(gD,actualSV->CurFontSize);
    defaultFont->sequence_value.int_pairs[0].second = fontBlk->ODAFontID;
    odalib_SetAttr(styleBlk->presStyle, cc_CHARACTER_FONTS, (POINTER_type) defaultFont, Ignore_Parms);
    odalib_DeleteSequence(defaultFont);

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryFont\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryMisc(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;

#if 0
    long AddMiscFlags;      /* Hinge, Pagebreak, Hidden, ... */
    long OutMiscFlags;
#endif

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryMisc\n");
    }
    numChanges = 0;
    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryMisc\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryCounter(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;
#if 0
    char *CounterName;
#endif

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryCounter\n");
    }

    numChanges = 0;

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryCounter\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryTabs(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;
    long NumTabChanges;

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryTabs\n");
    }

    numChanges = 0;
    NumTabChanges  = 0;

    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryTabs\n");
    }

    return numChanges;
}

INT_type CreateAuxiliaryInformation(gD, actualSV, styleBlk)
struct atkoda * gD;
struct text_statevector * actualSV;
p_styleSheetInfo styleBlk;
{

    INT_type		numChanges;
#if 0
    struct namespace *AdditionalAttributes;
#endif

    if (gD->doDebugging) {
	printf("Entering CreateAuxiliaryInformation\n");
    }
    numChanges = 0;


    if (gD->doDebugging) {
	printf("Leaving CreateAuxiliaryInformation\n");
    }

    return numChanges;
}
