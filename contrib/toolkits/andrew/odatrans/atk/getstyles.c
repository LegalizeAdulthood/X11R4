/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getstyles.c,v 1.3 89/09/11 13:55:52 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getstyles.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getstyles.c,v 1.3 89/09/11 13:55:52 mss Exp $";
#endif

/*
  $Log:	getstyles.c,v $
 * Revision 1.3  89/09/11  13:55:52  mss
 * Got rid of extraneous inclusion of text.ih
 * 
 * Revision 1.2  89/09/08  17:27:38  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:22  mss
 * Initial revision
 * 
 * Revision 1.18  89/07/21  10:58:50  mss
 * Switched to using iterator from own method
 * 
 * Revision 1.17  89/04/30  12:06:55  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.16  89/03/23  07:05:57  mss
 * Uses new component and dvl specifications
 * 
 * Revision 1.15  88/12/14  10:50:29  mss
 * Changed the use of NULL_CONSTITUENT constant to
 * odalib_NULL_CONSTITUENT() class procedure
 * 
 * Revision 1.14  88/12/13  20:53:01  mss
 * Changed NULL_SEQUENCE to use odalib class
 * procedure instead
 * 
 * Revision 1.13  88/11/29  14:36:55  mss
 * Fixed (hacked) style analysis on generic basic logical objects
 * so that the object is used as the "fakeblo" in lookups
 * 
 * Revision 1.12  88/11/17  17:41:26  mss
 * Added state for seeing if style was ever applied
 * (in case style changing was required)
 * 
 * Revision 1.11  88/11/10  11:57:14  mss
 * Fixed some typos in error msgs;
 * Prevented null pointer access in debugging
 * output when checking for style names
 * 
 * Revision 1.10  88/11/08  14:53:32  mss
 * Prevent reuse of global name
 * 
 * Revision 1.9  88/11/04  17:04:55  mss
 * *** empty log message ***
 * 
 * Revision 1.8  88/11/02  16:56:13  mss
 * Revised margin calculations
 * 
 * Revision 1.7  88/10/31  21:30:57  mss
 * Added more style function (fonts/margins)
 * 
 * Revision 1.6  88/10/18  14:04:23  mss
 * Added SMU conversion routine
 * 
 * Revision 1.5  88/10/17  11:58:21  mss
 * Integrate into other code
 * 
 * Revision 1.4  88/10/10  18:51:47  mss
 * 
 * 
 * Revision 1.3  88/10/01  17:10:51  mss
 * Added more style analysis
 * 
 * Revision 1.2  88/09/22  10:59:27  mss
 * Code rearranged a little
 * 
 * Revision 1.1  88/09/07  11:09:21  mss
 * Initial revision
 * 


*/

/* This modules walks over the generic part of a document and extracts all the style information that it can find, specifically:

  presentation style for BLO;
  layout style for BLO;
  generic page

    For each style found, it tries to create an appropriate ATK style that corresponds to it. For this purpose, this module also tries to create a global style/FSV that corresponds with the global environment of the document */

#include <odatk.h>
#include <class.h>
#include <odadef.h>
#include <attsup.h>
#include <text.ih>
#include <textv.ih>
#include <txtstvec.h>
#include <style.ih>
#include <stylesht.ih>
#include <fontdesc.ih>
#include <odalib.ih>
#include <odaatk.h>
#include <getstyles.h>
#include <getdefault.h>


p_style_elt style_list = NULL;

/* This procedure takes a value in SMUs and a density to convert to, and returns a number which is the same size converted from SMUs and into the desired denisty. For example, given 300 SMUs, and a request of 200 dots/inch, the return value is how many dots, at 200 dots/inch, corresponds to 300 SMUs (in this case 50, since 50/200 is 0.25 inch, and 300/1200 is 0.25 inch) */
INT_type GetDenseDotsFromSMUs(gD, SizeInSMUs, DotDensityPerInch)
p_odainfo_type gD;
INT_type SizeInSMUs;
INT_type DotDensityPerInch; {
    INT_type rval;

    rval = (DotDensityPerInch * gD->SMU_Numerator * SizeInSMUs) / (1200 * gD->SMU_Denominator);

    return rval;
}



INT_type SetJustification(ATKStyle, alignmentValue)
struct style * ATKStyle;
cc_ALIGNMENT_type alignmentValue; {
    INT_type rval;

    rval = SUCCESS_INT;

    switch(alignmentValue) {
	case cc_ALIGNMENT_start_aligned:
	    style_SetJustification(ATKStyle, style_LeftJustified);
	    break;
	case cc_ALIGNMENT_end_aligned:
	    style_SetJustification(ATKStyle, style_RightJustified);
	    break;
	case cc_ALIGNMENT_centred:
	    style_SetJustification(ATKStyle, style_Centered);
	    break;
	case cc_ALIGNMENT_justified:
	    style_SetJustification(ATKStyle, style_LeftAndRightJustified);
	    break;
	default:
	    style_SetJustification(ATKStyle, style_PreviousJustification);
	    printf("Unknown alignment value %d, using Previous Justfiication  (atk default)\n", alignmentValue);
    }

    return rval;
}


static INT_type GetJustification(gD,testConstituent, ATKStyle)
p_odainfo_type gD;
CONSTITUENT testConstituent;
struct style * ATKStyle;
{
    cc_ALIGNMENT_type	curAlignment;
    INT_type		rval;
    
    if (gD->doDebugging) {
	printf("Entering GetJustification\n");
    }

    rval = SUCCESS_INT;

    if (SUCCESS_INT == GetOnObjOrStyle(testConstituent, cc_ALIGNMENT, (POINTER_type) &curAlignment, Ignore_Parms)) {
	if (gD->doDebugging) {
	    printf("Found an alignment %d\n", curAlignment);
	}
	rval = SetJustification(ATKStyle, curAlignment);
    }
    else {
	if (gD->doDebugging){
	    printf("Using default alignment\n");
	}
	style_SetJustification(ATKStyle, style_PreviousJustification);
    }

    if (gD->doDebugging) {
	printf("Leaving GetJustification\n");
    }

    return rval;
}

static INT_type GetHorizontalMargins(gD,testConstituent, ATKStyle)
p_odainfo_type gD;
CONSTITUENT testConstituent;
struct style * ATKStyle;
{
    at_OFFSET_type	*curODAOffset;
    PARM_MASK_type	offsetParms;
    cc_INDENTATION_type	curODAIndentation;
    cc_1ST_LINE_OFFSET_type curFLOffset;
    INT_type		rval;
    INT_type		GotIndentation, GotOffset, GotFLOffset;
    long		newRHMargin, newLHMargin;
    
    if (gD->doDebugging) {
	printf("Entering GetHorizontalMargins\n");
    }

    rval = SUCCESS_INT;

    GotIndentation = GetOnObjOrStyle(testConstituent, cc_INDENTATION, (POINTER_type) &curODAIndentation, Ignore_Parms);
    GotOffset = GetOnObjOrStyle(testConstituent, at_OFFSET, (POINTER_type) &curODAOffset, &offsetParms);
    /* Make it canonical */
    if (offsetParms == ALL_PARMS) {
	offsetParms = LEADING_OFFSET_parm | TRAILING_OFFSET_parm | LEFT_HAND_OFFSET_parm | RIGHT_HAND_OFFSET_parm;
    }
    GotFLOffset = GetOnObjOrStyle(testConstituent, cc_1ST_LINE_OFFSET, (POINTER_type) &curFLOffset, Ignore_Parms);
    if ((GotIndentation == SUCCESS_INT) || (GotOffset == SUCCESS_INT) || (GotFLOffset == SUCCESS_INT)) {
	if (gD->doDebugging) {
	    printf("Found an indentation, 1st line offset or offset\n");
	}
	if (GotOffset == SUCCESS_INT) {
	    /* See which margins have changed */
	    if (offsetParms & LEFT_HAND_OFFSET_parm) {
		/* new right margin */
		newRHMargin = GetDenseDotsFromSMUs(gD,curODAOffset->left_hand_offset, 72);
		if (gD->doDebugging) {
		    printf("Found an RM %d\n",newRHMargin);
		}
		if (newRHMargin != gD->globalSV.CurRightMargin) {
		    style_SetNewRightMargin(ATKStyle, style_RightMargin, (long) newRHMargin - gD->globalSV.CurRightMargin, style_Points);
		}
		else {
		    style_SetNewRightMargin(ATKStyle, style_ConstantMargin, (long) newRHMargin, style_Points);
		}
	    }
	    if (offsetParms & RIGHT_HAND_OFFSET_parm) {
		/* new left margin */
		newLHMargin = GetDenseDotsFromSMUs(gD,curODAOffset->right_hand_offset, 72);
		if (gD->doDebugging) {
		    printf("Found a LM %d\n",newLHMargin);
		}
		if (newLHMargin != gD->globalSV.CurLeftMargin) {
		    style_SetNewLeftMargin(ATKStyle, style_LeftMargin, (long) newLHMargin - gD->globalSV.CurLeftMargin, style_Points);
		}
		else {
		    style_SetNewLeftMargin(ATKStyle, style_ConstantMargin, (long) newLHMargin, style_Points);
		}

	    }
	    free(curODAOffset);
	}

    }
    else {
	if (gD->doDebugging){
	    printf("Using default indentation and offsets\n");
	}
/*	style_SetJustification(ATKStyle, style_PreviousJustification); */
    }

    if (gD->doDebugging) {
	printf("Leaving GetHorizontalMargins\n");
    }

    return rval;
}




INT_type GetSingleBasicStyle(gD,node, refPStyle, refLStyle, refNLO)
p_odainfo_type gD;
CONSTITUENT node;
CONSTITUENT * refPStyle, * refLStyle;
at_NEW_LAYOUT_OBJECT_type ** refNLO;
{
     INT_type rval;

    if (gD->doDebugging) {
        printf("Entering GetSingleBasicStyle\n");
    }

    rval = SUCCESS_INT;
    /* Well, here is a BLO, are there styles? */
    if (CheckAndGetAttr(node, at_PRESENTATION_STYLE, (POINTER_type) refPStyle, Ignore_Parms) != SUCCESS_INT){
	*refPStyle = odalib_NULL_CONSTITUENT();
    }
    if (CheckAndGetAttr(node, at_LAYOUT_STYLE, (POINTER_type) refLStyle, Ignore_Parms) != SUCCESS_INT){
	*refLStyle = odalib_NULL_CONSTITUENT();
    }
    if (CheckAndGetAttr(node, at_NEW_LAYOUT_OBJECT, (POINTER_type) refNLO, Ignore_Parms) != SUCCESS_INT) {
	*refNLO = (at_NEW_LAYOUT_OBJECT_type *) NULL;
    }


LEAVE:
    if (gD->doDebugging) {
        printf("Leaving GetSingleBasicStyle\n");
    }


    return rval;
}



INT_type GetSingleCompositeStyle(gD, node, refPStyle, refLStyle, refNLO, fakeBLO)
p_odainfo_type gD;
CONSTITUENT node;
CONSTITUENT * refPStyle, * refLStyle;
at_NEW_LAYOUT_OBJECT_type ** refNLO;
CONSTITUENT * fakeBLO; 	    /* From DVL */
{
    INT_type status;
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering GetSingleCompositeStyle\n");
    }

    rval = SUCCESS_INT;
    /* Do we have either a DVL/PStyle, a DVL/LStyle or NLO? */
    if (SUCCESS_INT == GetBLOFromDVL(node, fakeBLO)) {
	/* Well, there was a BLO, are there styles? */
	if (CheckAndGetAttr(*fakeBLO, at_PRESENTATION_STYLE, (POINTER_type) refPStyle, Ignore_Parms) != SUCCESS_INT){
	    *refPStyle = odalib_NULL_CONSTITUENT();
	}
	if (CheckAndGetAttr(*fakeBLO, at_LAYOUT_STYLE, (POINTER_type) refLStyle, Ignore_Parms) != SUCCESS_INT){
	    *refLStyle = odalib_NULL_CONSTITUENT();
	}
	if (CheckAndGetAttr(node, at_NEW_LAYOUT_OBJECT, (POINTER_type) refNLO, Ignore_Parms) != SUCCESS_INT) {
	    *refNLO = (at_NEW_LAYOUT_OBJECT_type *) NULL;
	}
    }

LEAVE:
    if (gD->doDebugging) {
        printf("Leaving GetSingleCompositeStyle\n");
    }
    return rval;
}


INT_type CheckDupStyleName(gD, node, retName, curName)
p_odainfo_type gD;
CONSTITUENT node;
at_USER_VISIBLE_NAME_type * retName, *curName;
{
    if (gD->doDebugging) {
        printf("Entering CheckDupStyleName\n");
    }

    if (SUCCESS_INT != CheckAndGetAttr(node, at_USER_VISIBLE_NAME, (POINTER_type) retName, Ignore_Parms)){
	*retName = odalib_NULL_SEQUENCE();
    }
    /* We have a name, see if it is first one defined */
    else if (*curName == odalib_NULL_SEQUENCE()) {
	*curName = *retName;
    }
    /* Not first defined, does it match previous? */
    else if ( ((*retName)->length != (*curName)->length) ||  (strncmp((*retName)->sequence_value.chars, (*curName)->sequence_value.chars, (*retName)->length)!=0) ){
	    printf("Mismatched style names, ignoring second %s1, %s2\n", (*curName)->sequence_value.chars, (*retName)->sequence_value.chars);
      }

    if (gD->doDebugging) {
        printf("Leaving CheckDupStyleName\n");
    }
    return SUCCESS_INT;
}


INT_type GetStyleName(gD, node, PStyle, LStyle, retName)
p_odainfo_type gD;
CONSTITUENT node, PStyle, LStyle; 
char ** retName; {
    at_USER_VISIBLE_NAME_type nodeName, PStyleName, LStyleName;
    at_USER_VISIBLE_NAME_type testName;
    static int nextStyleCounter = 1;
    char tempStr[40];

    if (gD->doDebugging) {
        printf("Entering GetStyleName\n");
    }

    *retName = (char *) NULL;
    testName = odalib_NULL_SEQUENCE();

    CheckDupStyleName(gD, PStyle, &PStyleName, &testName);
    CheckDupStyleName(gD, LStyle, &LStyleName, &testName);
    CheckDupStyleName(gD, node, &nodeName, &testName);

    /* If any exist, make a copy for later use, otherwise invent a name */
    if (testName == odalib_NULL_SEQUENCE()) {
	/* Make a new one */
	nextStyleCounter++;
	sprintf(tempStr,"translator-style-%d",nextStyleCounter);
	*retName = (char *) malloc(strlen(tempStr) + 1);
	strcpy(*retName, tempStr);
    }
    else {
	strcpy(tempStr,""); /* in case I want a prefix */
	*retName = (char *) malloc(testName->length + 1 + strlen(tempStr));
	strcat(*retName,tempStr);
	strncat(*retName,testName->sequence_value.chars,testName->length);
    }

    /* Now throw away any attribute we are hanging on to */
    if (PStyleName != odalib_NULL_SEQUENCE()) {
	if (ERROR_INT == odalib_DeleteSequence(PStyleName)) {
	    printf("Could not delete presentation style name %s\n", ErrorMsg(odalib_TKError()));
	}
    }
    if (LStyleName != odalib_NULL_SEQUENCE()) {
	if (ERROR_INT == odalib_DeleteSequence(LStyleName)) {
	    printf("Could not delete layout style name %s\n", ErrorMsg(odalib_TKError()));
	}
    }
    if (nodeName != odalib_NULL_SEQUENCE()) {
	if (ERROR_INT == odalib_DeleteSequence(nodeName)) {
	    printf("Could not delete node style name %s\n", ErrorMsg(odalib_TKError()));
	}
    }
    
    if (gD->doDebugging) {
        printf("Leaving GetStyleName (%s)\n", *retName);
    }
    return SUCCESS_INT;

}


typedef struct styleMap_struct {
    struct styleMap_struct * next;	/* next block in chain */
    CONSTITUENT		    node;	/* node associated with style */
    p_style_elt		    styleDef;	/* Full information about style */
} styleMap, * p_styleMap;

p_styleMap globalMapPtr = (p_styleMap) NULL;


struct style * FindATKStyleFromConst(gD, node)
p_odainfo_type gD;
CONSTITUENT node;
{
    p_styleMap testNode;

    if (gD->doDebugging) {
	printf("Searching for style for %x\n", node);
    }
    for(testNode=globalMapPtr; testNode && testNode->node != node; testNode = testNode->next){
	if (gD->doDebugging) {
	    printf("Looking at style %s\n", testNode->styleDef->styleName ? testNode->styleDef->styleName : "<no style name>" );
	}
    }
    if (testNode) {
	if (gD->doDebugging) {
	    printf("About to return %s\n", testNode->styleDef->styleName  ? testNode->styleDef->styleName : "<no style name>");
	}
	return testNode->styleDef->ATKStyle;
    }
    else {
	if (gD->doDebugging) {
	    printf("Could not find style\n");
	}
	return (struct style * ) NULL;
    }
}


AddToStyleList(gD,node,realStyleElt)
p_odainfo_type gD;
CONSTITUENT node;
/* struct style * ATKstyle; */
p_style_elt realStyleElt;
{
    p_styleMap tempBlk;

    if (gD->doDebugging) {
	printf("Adding style %x for node %x\n", realStyleElt, node);
    }
    tempBlk = (p_styleMap) malloc(sizeof(styleMap));
    if (tempBlk	== NULL) {
	printf("Could not allocate space in AddtoStyleList\n");
	  return;
    }
    tempBlk->next = globalMapPtr;
    tempBlk->node = node;
    tempBlk->styleDef = realStyleElt;

    globalMapPtr = tempBlk;
}

BOOL_type AreSameStrings(str1, str2)
STRING_type str1;
STRING_type str2; {
    BOOL_type rval;
    INT_type i;

    if (str1->length != str2->length) {
	rval = BOOL_false;
    }
    else {
	rval = BOOL_true;
	for(i=0;i<str1->length;i++){
	    if (str1->sequence_value.chars[i] != str2->sequence_value.chars[i]) {
		rval = BOOL_false;
		break;
	    }
	}
    }

    return rval;
}


BOOL_type AreNLOSame(NLO1,NLO2)
at_NEW_LAYOUT_OBJECT_type * NLO1;
at_NEW_LAYOUT_OBJECT_type * NLO2;{
    BOOL_type rval;

    if ( NLO1 == NLO2) {
	rval = BOOL_true;
    }
    else if ( NLO1 == (at_NEW_LAYOUT_OBJECT_type *) NULL) {
	rval = BOOL_false;
    }
    else if ( NLO2 == (at_NEW_LAYOUT_OBJECT_type *) NULL) {
	rval = BOOL_false;
    }
    else if (NLO1->layout_object_tag != NLO2->layout_object_tag) {
	rval = BOOL_false;
    }
    else {
	switch(NLO1->layout_object_tag){
	    case OBJECT_TYPE_PAGE_tag:
	    case NULL_tag:
		    rval = BOOL_true;
		    break;
	    case LAYOUT_OBJ_DESC_tag:
		    rval = (NLO1->layout_object_value.constituent == NLO2->layout_object_value.constituent);
		    break;
	    case LAYOUT_CATEGORY_tag:
		    rval = AreSameStrings(NLO1->layout_object_value.layout_category, NLO2->layout_object_value.layout_category);
		    break;
		
	}
    }

    return rval;
}


p_style_elt FindStyleElt(gD, PStyle, LStyle, NLO, gPage)
p_odainfo_type gD;
CONSTITUENT PStyle;
CONSTITUENT LStyle;
at_NEW_LAYOUT_OBJECT_type * NLO;
CONSTITUENT gPage;  /* currently unused */
{
    /* This routine checks to see if it can find the style already present
	in the list of known styles */
    p_style_elt testBlk;

    for(testBlk = style_list; 
	 testBlk && ((testBlk->presentationStyle != PStyle) ||
	            (testBlk->layoutStyle != LStyle) ||
	            (!AreNLOSame(testBlk->NLO, NLO))); testBlk=testBlk->next);
    return testBlk;
}


p_style_elt MakeNewStyleElt(gD, PStyle, LStyle, NLO, gPage)
p_odainfo_type gD;
CONSTITUENT PStyle;
CONSTITUENT LStyle;
CONSTITUENT gPage;
at_NEW_LAYOUT_OBJECT_type * NLO; {
    p_style_elt rval;

    if (gD->doDebugging) {
	printf("Entering MakeNewStyleElt %X, %X, %X, %X\n", gD, PStyle, LStyle, NLO);
    }

    rval = (p_style_elt) malloc (sizeof(style_elt));
    if (rval == NULL) {
	printf("Could not allocated style element in MakeNewStyleElt\n");
	goto LEAVE;
    }
    rval->next = style_list;
    style_list = rval;
    rval->ATKStyle = (struct style *) NULL; /* Filled in later */
    rval->styleName = (char * )	NULL;	    /* Filled in later */
    rval->presentationStyle = PStyle;
    rval->layoutStyle = LStyle;
    rval->NLO = NLO;
    rval->genericPage = gPage;
    rval->styleEverApplied = BOOL_false;

LEAVE:

    return rval;

}


char * CheckForUniqueStyleName(gD, curStyleName) 
p_odainfo_type gD;
char * curStyleName; {
    /* Routine checks to see if the name is unique, and if not, make it unique */
    char * rval ;
    p_style_elt testBlk;

    if (gD->doDebugging) {
	printf("Entering CheckForUniqueStyleName %X for %s\n", gD, curStyleName);
    }

    rval = curStyleName;

    /* Look for a match */
    for(testBlk = style_list; 
	 testBlk && ((testBlk->styleName == (char *) NULL) ||  (strcmp(curStyleName,testBlk->styleName)!=0)); testBlk=testBlk->next);
    /* Matching string or default global style */
    if (testBlk || (0==strcmp(curStyleName,"global"))) {
	/* Found a style with the same name, albeit different styles */
	/* Try to create a new one (style prime) from the old -- this should be realloc*/
	rval = (char *) malloc(strlen(curStyleName)+1+1);
	strcpy(rval,curStyleName);
	strcat(rval,"'");
	/* And recheck */
	if(rval) {
	    rval = CheckForUniqueStyleName(gD, rval);
	}
	else {
	    printf("Could not create a new style name for %s\n", curStyleName);
	}
    }

LEAVE:
    if (gD->doDebugging) {
	printf("Leaving CheckForUniqueStyleName with %s\n", rval);
    }

    return rval;
    
}


void GetATKStyleInfo(gD, node, fakeBLO, PStyle, LStyle, NLO, gPage, ATKStyle)
p_odainfo_type gD;
CONSTITUENT node;
CONSTITUENT fakeBLO;
CONSTITUENT PStyle;
CONSTITUENT LStyle;
at_NEW_LAYOUT_OBJECT_type * NLO;
CONSTITUENT gPage;
struct style * ATKStyle;
{
    if (gD->doDebugging) {
	printf("Entering GetATKStyleInfo\n");
    }

    /* First, get justification */
    GetJustification(gD,fakeBLO, ATKStyle);

    /* Next, get L/R margin/ Indentation information */
    GetHorizontalMargins(gD, fakeBLO, ATKStyle);

    /* Next, get all sorts of font stuff -- this is the same as doing SV calculations for it */
    GetStyleFontInfo(gD, fakeBLO, ATKStyle);

    if (gD->doDebugging) {
	printf("Entering GetATKStyleInfo\n");
    }

}

static INT_type GetStyleFontInfo(gD,BLO, ATKStyle)
p_odainfo_type gD;
CONSTITUENT BLO;
struct style * ATKStyle;
{
    INT_type			rval;
    cc_CHARACTER_FONTS_type	curCF;
    cc_GRAPHIC_RENDITION_type	curGR;
    INT_type			i;	    /* loop index */
    INT_type			CFIndex;    /* which cf to use */
    p_FontAsSV_type		styleFont;	/* which font to examine */


    if (gD->doDebugging) {
        printf("Entering GetStyleFontInfo\n");
    }

    rval = SUCCESS_INT;

    if ((SUCCESS_INT == GetOnObjOrStyle(BLO, cc_CHARACTER_FONTS, (POINTER_type) &curCF, Ignore_Parms)) &&
	 (curCF->length > 0) ) {
	/* Found a character fonts definition, so we have a font */
	CFIndex = 0;
	if (SUCCESS_INT == GetOnObjOrStyle(BLO, cc_GRAPHIC_RENDITION, (POINTER_type) &curGR, Ignore_Parms)) {
	    /* Found a GR, otherwise we assume index 0 */
	    for (i=0;i<curGR->length;i++) {
		/* Note: wired in knowledge about what GR means */
		if ((curGR->sequence_value.ints[i] >= 10) &&
		    (curGR->sequence_value.ints[i] <= 19)) {
		    CFIndex = curGR->sequence_value.ints[i] - 10;
		}
	    }
	    odalib_DeleteSequence(curGR);

	}

	/* Make sure that whatever GR we have, it is in bounds */
	/* See if we have that many invoked fonts */
	if (CFIndex > curCF->length-1) {
	    printf("Style font %d beyond end of possible font list %d\n",
		   CFIndex, curCF->length-1);
	    CFIndex = 0;
	}

	/* Get the font information in absolute terms first */
	styleFont = FindSVforFont(gD, curCF->sequence_value.int_pairs[CFIndex].second);
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

	    /* See if a relative or absolute size (heuristic) */
	    if (styleFont->newFontSize == gD->globalSV.CurFontSize) {
		/* It's the same, so an absolute value */
		style_SetFontSize(ATKStyle, style_ConstantFontSize, styleFont->newFontSize);
	    }
	    else {
		/* No,its different, so assume it's a relative value */
		style_SetFontSize(ATKStyle, style_PreviousFontSize, styleFont->newFontSize - gD->globalSV.CurFontSize);
	    }
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

	odalib_DeleteSequence(curCF);

    }

LEAVE:

    if (gD->doDebugging) {
        printf("Leaving GetStyleFontInfo\n");
    }

    return rval;
}


p_style_elt GetStyleElt(gD, node, fakeBLO, PStyle, LStyle, NLO, gPage)
p_odainfo_type gD;
CONSTITUENT node;
CONSTITUENT fakeBLO;
CONSTITUENT PStyle;
CONSTITUENT LStyle;
at_NEW_LAYOUT_OBJECT_type * NLO;
CONSTITUENT gPage;
{
    p_style_elt rval;
    char * styleName;
    char * menuName;
    struct style * newStyle;

    if (gD->doDebugging) {
	printf("Entering GetStyleElt %X, %X, %X, %X, %X\n", gD, PStyle, LStyle, NLO, gPage);
    }

    rval = FindStyleElt(gD, PStyle, LStyle, NLO, gPage);
    if (rval == (p_style_elt) NULL) {
	/* Don't have it yet, so go make it */
	rval = MakeNewStyleElt(gD, PStyle, LStyle, NLO, gPage);

	/* We have a style, go start getting stuff out of it, like its name! */
	GetStyleName(gD, node, PStyle, LStyle, &styleName );
	styleName = CheckForUniqueStyleName(gD, styleName);

	/* Add it to the style system of the ATK document */
	newStyle = style_New();
	style_SetName(newStyle, styleName);

	stylesheet_Add(gD->topLevelDocument->styleSheet, newStyle);
	menuName = (char *) malloc(strlen(styleName) + 1 + strlen("Document,"));
	strcpy(menuName,"Document,");
	strcat(menuName,styleName);
	if (menuName != (char *) NULL) {
	    style_SetMenuName(newStyle, menuName);
	    free(menuName);
	}

	/* And get rid of string */
	free(styleName);

	/* Get as much state as desired */
	GetATKStyleInfo(gD, node, fakeBLO, PStyle, LStyle, NLO, gPage, newStyle);


	/* Record style info in style element block
	   (and fix the SV to take into account the style) */
	rval->ATKStyle = newStyle;
	rval->styleName = style_GetName(newStyle);
	text_InitStateVector(&rval->SV);
	text_ApplyEnvironment(&rval->SV, gD->globalStyle, (struct environment *) NULL);
	text_ApplyEnvironment(&rval->SV, newStyle, (struct environment *) NULL);
    }

    if (gD->doDebugging) {
	printf("Leaving GetStyleElt with %X\n", rval);
    }

    return rval;
}



INT_type GetSingleGenericStyle(gD,node) 
p_odainfo_type gD;
CONSTITUENT node;
{
    INT_type status;
    at_NEW_LAYOUT_OBJECT_type *NLO;
    at_PRESENTATION_STYLE_type PStyle;
    at_LAYOUT_STYLE_type LStyle;
    at_OBJECT_TYPE_type ObjType;
    INT_type rval;
    CONSTITUENT fakeBLO;
    p_style_elt	curStyleElt;		    /* Unique style block */

    if (gD->doDebugging){
        printf("Entering GetSingleGenericStyle from %X\n", node);
    }

    rval = SUCCESS_INT;

    if (ERROR_INT == SafeGetAttr(node, at_OBJECT_TYPE, (POINTER_type) &ObjType, Use_Local_Value, Ignore_Parms)) {
	printf("Could not get object type during style analysis %s\n", ErrorMsg(odalib_TKError()));
	rval = ERROR_INT;
	goto LEAVE;
    }

    NLO = (at_NEW_LAYOUT_OBJECT_type *) NULL;
    PStyle = odalib_NULL_CONSTITUENT();
    LStyle = odalib_NULL_CONSTITUENT();

    switch (ObjType) {
        case at_OBJECT_TYPE_doc_logical_root:
	    /* root handled specially as global environment */
	    rval = SUCCESS_INT;
	    goto LEAVE;
	case at_OBJECT_TYPE_comp_logical_obj:
	    rval = GetSingleCompositeStyle(gD,node,&PStyle, &LStyle, &NLO, &fakeBLO);
	    break;
        case at_OBJECT_TYPE_bas_logical_obj:
	    rval = GetSingleBasicStyle(gD,node, &PStyle, &LStyle, &NLO);
	    fakeBLO = node; /* so we can look where we should */
	    break;
	default:
	    printf("Unexpected object type %d in getsinglegenericstyle\n", ObjType);
	    rval = ERROR_INT;
	    goto LEAVE;
    }

    /* Did we wind up with a presentation style, layout style, or NLO? */
    if ((PStyle != odalib_NULL_CONSTITUENT()) || (LStyle != odalib_NULL_CONSTITUENT()) || (NLO != (at_NEW_LAYOUT_OBJECT_type *) NULL)) {
	/* See if we already have a style from this trio */
	curStyleElt = GetStyleElt(gD, node, fakeBLO, PStyle, LStyle, NLO, /* gPage */ odalib_NULL_CONSTITUENT());

	/* regardless of found or created,
	   And add it to the list of generic <-> styles */
	AddToStyleList(gD,node,curStyleElt);
    }


LEAVE:

    if (gD->doDebugging){
        printf("Leaving GetSingleGenericStyle from %X\n", node);
    }

    return rval;

}



INT_type GetGenericStyles(gD)
p_odainfo_type gD;
{

    ITERATOR_type   getStyleIterator;
    CONSTITUENT	    nextGenericNode;

    getStyleIterator = odalib_MakeSubgraphIterator(gD->genericLogicalRoot, PARENTS_FIRST, DEPTH_FIRST);
#if 0
    if (getStyleIterator == odalib_ERROR_ITERATOR()) {
	goto LEAVE;
    }
#endif

    while ((odalib_NULL_CONSTITUENT() != (nextGenericNode = odalib_NextConstituent(getStyleIterator)))
	    && (odalib_ERROR_CONSTITUENT() != nextGenericNode)) {
		GetSingleGenericStyle(gD,nextGenericNode);
	    }

    odalib_DeleteIterator(getStyleIterator);


LEAVE:

    return SUCCESS_INT;
}

