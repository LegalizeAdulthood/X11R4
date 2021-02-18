/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkinsetcomp.c,v 1.2 89/09/08 17:29:13 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkinsetcomp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkinsetcomp.c,v 1.2 89/09/08 17:29:13 mss Exp $";
#endif

/*
  $Log:	mkinsetcomp.c,v $
 * Revision 1.2  89/09/08  17:29:13  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:21:48  mss
 * Initial revision
 * 
 * Revision 1.7  89/05/20  20:45:26  mss
 * Changed style attribute setting
 * 
 * Revision 1.6  89/04/30  12:07:43  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.5  89/03/27  21:14:29  mss
 * Shortened parameter name
 * 
 * Revision 1.4  89/03/23  07:07:04  mss
 * Uses new component and dvl specifications
 * 
 * Revision 1.3  88/11/30  09:51:34  mss
 * Reinstated block alignment on rasters
 * (in redundant, incorrect fashion)
 * 
 * Revision 1.2  88/11/29  15:00:02  mss
 * Added initial versions of some of the inset compensation code
 * 
 * Revision 1.1  88/11/17  17:31:49  mss
 * Initial revision
 * 



*/



/* Module used to compensate non-text insets for outbound ODA */

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
#include <view.ih>
#include <viewref.ih>
#include <mkinsetcomp.h>


/* Forward declarations */
INT_type CompensateInsetMargins(/*gD, curEnclosingView, curEnv, FigureCLO*/);
INT_type CompensateInsetAlignment(/*gD, curEnclosingView, curEnv, FigureCLO*/);
INT_type CompensateInsetSeparation(/*gD, curEnclosingView, curEnv, FigureCLO*/);
INT_type DoAttachment(/*gD, FigureCLO, dummyLayoutStyle*/);

INT_type InsetCompensation(gD, curEnclosingView, curEnv, FigureCLO)
struct atkoda* gD;
struct textview * curEnclosingView;
struct environment * curEnv;
CONSTITUENT FigureCLO;
{
    INT_type	rval;
    struct text_statevector	tempStateVector;
    CONSTITUENT			dummyLayoutStyle;
    long			numAttrs;

    if (gD->doDebugging) {
	printf("Entering InsetCompensation\n");
    }

    rval = SUCCESS_INT;
    dummyLayoutStyle = odalib_MakeStyle(gD->outputDocument, LAYOUT_STYLE);
    numAttrs = 0;

    /* Have to provide a non-text alignment */
    text_InitStateVector(&tempStateVector);
    text_ApplyEnvironment(&tempStateVector,gD->globalStyle,curEnv);

    /* Compensate for varying left margins that have negative indentations */
    numAttrs += CompensateInsetMargins(gD, curEnclosingView, curEnv, FigureCLO, &tempStateVector, dummyLayoutStyle);

    /* Compensate for alignments */
    numAttrs += CompensateInsetAlignment(gD, curEnclosingView, curEnv, FigureCLO, &tempStateVector, dummyLayoutStyle);

    /* Compensate for paragraph separations */
    numAttrs += CompensateInsetSeparation(gD, curEnclosingView, curEnv, FigureCLO, &tempStateVector, dummyLayoutStyle);

    if (numAttrs != 0) {
	DoAttachment(gD, FigureCLO, dummyLayoutStyle);
    }
    else {
	odalib_DeleteConstituent(dummyLayoutStyle);
    }

LEAVE:
    if (gD->doDebugging) {
	printf("Leaving InsetCompensation\n");
    }

    return rval;
}

INT_type CompensateInsetMargins(gD, curEnclosingView, curEnv, FigureCLO, curSV, dummyLayoutStyle)
struct atkoda* gD;
struct textview * curEnclosingView;
struct environment * curEnv;
CONSTITUENT FigureCLO;
struct text_statevector * curSV;
CONSTITUENT		    dummyLayoutStyle;
{
    INT_type	rval;


    if (gD->doDebugging) {
	printf("Entering CompensateInsetMargins\n");
    }

    rval = 0;

    /* Compensation needed only if we have a negative paragraph indentation  (overhang) */

LEAVE:
    if (gD->doDebugging) {
	printf("Leaving CompensateInsetMargins\n");
    }

    return rval;
}

INT_type CompensateInsetAlignment(gD, curEnclosingView, curEnv, FigureCLO, curSV, dummyLayoutStyle)
struct atkoda* gD;
struct textview * curEnclosingView;
struct environment * curEnv;
CONSTITUENT FigureCLO;
struct text_statevector * curSV;
CONSTITUENT		    dummyLayoutStyle;
{
    INT_type		rval;
    at_BLK_ALGNM_type	newAlignment;

    if (gD->doDebugging) {
	printf("Entering CompensateInsetAlignment\n");
    }

    rval = 0;

    switch (curSV->CurJustification) {
	case style_LeftJustified:
        case style_LeftAndRightJustified:
        case style_LeftThenRightJustified:
	    /* This is normal, so nothing need be set */
	    newAlignment = at_BLK_ALGNM_right_hand_aligned;
	    break;
	case style_RightJustified:
	    rval++;
	    newAlignment = at_BLK_ALGNM_left_hand_aligned;
	    break;
	case style_Centered:
	    rval++;
	    newAlignment = at_BLK_ALGNM_centred;
	    break;
        case style_PreviousJustification:
	default:
	    printf("Illegal/Unknown ATK justification value %d\n", curSV->CurJustification);
	    newAlignment = at_BLK_ALGNM_right_hand_aligned;
    }
    if (rval) {
	odalib_SetAttr(dummyLayoutStyle, at_BLK_ALGNM, (POINTER_type) &newAlignment, ALL_PARMS);
	odalib_SetLayoutAttr(FigureCLO, at_BLK_ALGNM, (POINTER_type) &newAlignment, ALL_PARMS);
    }


LEAVE:
    if (gD->doDebugging) {
	printf("Leaving CompensateInsetAlignment\n");
    }

    return rval;
}


INT_type CompensateInsetSeparation(gD, curEnclosingView, curEnv, FigureCLO, curSV, dummyLayoutStyle)
struct atkoda* gD;
struct textview * curEnclosingView;
struct environment * curEnv;
CONSTITUENT FigureCLO;
struct text_statevector * curSV;
CONSTITUENT		    dummyLayoutStyle;
{
    INT_type		    rval;
    struct text		    * curTextObj;
    at_SEPARATION_type	    curSeparation;
    PARM_MASK_type	    curSeparationMask;
    long		    insetPosition;

    if (gD->doDebugging) {
	printf("Entering CompensateInsetSeparation\n");
    }

    rval = 0;
    goto LEAVE;

    curTextObj = (struct text *) curEnclosingView->header.view.dataobject;
    insetPosition = environment_Eval(curEnv);
    if (gD->doDebugging) {
	printf("Compensating separation for inset at position %d\n", insetPosition);
    }

    /* See if we are <cr> inset <cr>. If so, change separation on object to 0, as well as on preceding and following object */

    /* Is view preceeded by only a CR? */
    curSeparationMask = (PARM_MASK_type) 0;
    
    if ( (insetPosition > 2) /* have room for something to be in front of inset */ &&
	 (text_GetChar(curTextObj, insetPosition-2) != '\n') ) {
	/* We have <text><cr><inset> => so no separation */
	rval++;
	curSeparationMask |= TRAILING_EDGE_parm;
	curSeparation.trailing_edge = 0;
	/* Remember to set it on previous BLO as well */
    }

    if ( (insetPosition < (text_GetLength(curTextObj)-2)) /* need enough room after inset */ &&
	 (text_GetChar(curTextObj, insetPosition+2) != '\n')) {
	/* We have <inset><cr><text> => so no separation */
	rval++;
	curSeparationMask |= LEADING_EDGE_parm;
	curSeparation.leading_edge = 0;
	/* Remember to set on next blo as well */
    }

    if (curSeparationMask != (PARM_MASK_type) 0){
	odalib_SetAttr(dummyLayoutStyle, at_SEPARATION, (POINTER_type) &curSeparation, curSeparationMask);
    }


LEAVE:
    if (gD->doDebugging) {
	printf("Leaving CompensateInsetSeparation\n");
    }

    return rval;
}


INT_type DoAttachment(gD, FigureCLO, dummyLayoutStyle)
struct atkoda		    * gD;
CONSTITUENT		    FigureCLO;
CONSTITUENT		    dummyLayoutStyle;
{
    CONSTITUENT			 fakeBLO;
    at_DEFAULT_VALUE_LISTS_type	 tempDVL;
    INT_type			 rval;

    if (gD->doDebugging) {
	printf("Entering DoAttachment\n");
    }

    rval = SUCCESS_INT;

    if (odalib_InitializeDefaultValueLists(gD->outputDocument, &tempDVL, BAS_LOGICAL_ATTRIBUTES_parm) == ERROR_INT) {
	printf("DoAttachment: Cannot initialize DVL for figure\n");
	rval = ERROR_INT;
	goto LEAVE;
    }

    odalib_SetAttr(tempDVL.basic_logical_attributes, at_LAYOUT_STYLE, dummyLayoutStyle, ALL_PARMS);
    odalib_SetAttr(FigureCLO, at_DEFAULT_VALUE_LISTS, (POINTER_type) &tempDVL, ALL_PARMS);


LEAVE:
    if (gD->doDebugging) {
	printf("Leaving DoAttachement\n");
    }

    return rval;

}

