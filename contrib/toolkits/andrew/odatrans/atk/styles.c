/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/styles.c,v 1.3 89/10/12 15:35:24 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/styles.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/styles.c,v 1.3 89/10/12 15:35:24 ghoti Exp $";
#endif

/*
  $Log:	styles.c,v $
 * Revision 1.3  89/10/12  15:35:24  ghoti
 * BUG: 48
 * changed #ifdef 0 to #if 0
 * 
 * Revision 1.2  89/09/08  17:33:21  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:21:17  mss
 * Initial revision
 * 
 * Revision 1.12  89/08/25  11:44:24  mss
 * Changed calls from attribute support to tool kit
 * 
 * Revision 1.11  89/04/30  12:12:24  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.10  89/03/27  17:05:42  mss
 * Shortened parameter name
 * 
 * Revision 1.9  89/03/24  15:12:05  mss
 * Get rid of user readable comments on DVL components
 * 
 * Revision 1.8  89/03/23  06:55:57  mss
 * Updated to use new component and dvl specifications
 * 
 * Revision 1.7  88/11/10  11:53:30  mss
 * Fixed bug where debugging output was
 * accidently commented out
 * 
 * Revision 1.6  88/10/25  15:37:52  mss
 * Added more style processing/default processing
 * 
 * Revision 1.5  88/10/10  18:53:17  mss
 * revised tracing
 * 
 * Revision 1.4  88/10/01  17:11:44  mss
 * Updated to new atk
 * Added more style analysis
 * 
 * Revision 1.3  88/09/22  11:11:17  mss
 * Added global state; rearranged some code
 * 
 * Revision 1.2  88/09/09  16:43:07  mss
 * Added style creation for atk styles
 * 
 * Revision 1.1  88/09/07  11:19:18  mss
 * Initial revision
 * 
 * Revision 1.2  88/08/24  10:40:20  mss
 * Uses attsup instead of extern dcls
 * 
 * Revision 1.1  88/08/10  09:54:04  mss
 * Initial revision
 * 

*/

#include <stdio.h>
#include <class.h>
#include <odatk.h>
#include <odalib.ih>
#include <styles.h>
#include <attsup.h>

#include <atkoda.ih>
#include <breakup.h>
#include <text.ih>
#include <txtstvec.h>


p_docPageSet pageSetList;
/* p_styleSheetInfo styleSheetList; */

at_NEW_LAYOUT_OBJECT_type nullNewLayoutObject;
CONSTITUENT nullNLOLayoutStyle;			/* Style containing only null New Layout Object */

/* This module is used for manipulating abstract stylesheet information between a native system and ODA */

#define	MAXSTRING   128
static char tempStr[MAXSTRING];			    /* used for building temp strings */


static p_styleSheetInfo CreateDVLPieces(gD, rval, whichdoc, ssname, makeLStyle, makePStyle)
struct atkoda * gD;
p_styleSheetInfo rval;
DOCUMENT_type whichdoc;
char * ssname;
BOOL_type makeLStyle;
BOOL_type makePStyle; {

    if (gD->doDebugging) {
	printf("Entering CreateDVLPieces\n");
    }

    /* Get basic pieces  - layout style */
    rval->layoutStyle = odalib_MakeStyle(whichdoc, LAYOUT_STYLE);
    if (rval->layoutStyle == odalib_ERROR_CONSTITUENT()) {
	printf("Could not create layout style for stylesheet %s\n", ErrorMsg(odalib_TKError()));
    }
    else {
	sprintf(tempStr, "DVL Layout Style for stylesheet %s", ssname);
	if (strlen(tempStr) > (MAXSTRING-1)) {
	    printf("Just smashed storage with string too long in CreateDVLPieces\n");
	    return NULL;
	}
	(void) odalib_SetStringAttr(rval->layoutStyle, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);
	(void) odalib_SetStringAttr(rval->layoutStyle, at_USER_VISIBLE_NAME, ssname, DATA_CHAR_TYPE);
    }

    /* Get presentation style */
    rval->presStyle = odalib_MakeStyle(whichdoc, PRESENTATION_STYLE);
    if (rval->presStyle == odalib_ERROR_CONSTITUENT()) {
	printf("Could not create presentation style for stylesheet %s\n", ErrorMsg(odalib_TKError()));
    }
    else {
	sprintf(tempStr, "DVL Presentation Style for stylesheet %s", ssname);
	if (strlen(tempStr) > (MAXSTRING-1)) {
	    printf("Just smashed storage with string too long in CreateDVLPieces\n");
	    return NULL;
	}
	(void) odalib_SetStringAttr(rval->presStyle, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);
	(void) odalib_SetStringAttr(rval->presStyle, at_USER_VISIBLE_NAME, ssname, DATA_CHAR_TYPE);
    }

    /* Make fake BLO to hold styles */
    if (odalib_InitializeDefaultValueLists(whichdoc, &rval->styleList, BAS_LOGICAL_ATTRIBUTES_parm) == ERROR_INT) {
	printf("Cannot create fake BLO to hold styles for stylesheet %s\n", ssname);
    }
    else {
	rval->fakeBLO = rval->styleList.basic_logical_attributes;
	/* Comment the block */
	sprintf(tempStr, "Fake BLO to hold styles for stylesheet %s", ssname);
	if (strlen(tempStr) > (MAXSTRING-1)) {
	    printf("Just smashed storage with string too long in CreateDVLPieces\n");
	    return NULL;
	}
#if 0
	/* Can't default comments! */
	(void) odalib_SetStringAttr(rval->fakeBLO, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);
#endif
	/* Attach styles to BLO */
	SafeSetAttr(rval->fakeBLO, at_PRESENTATION_STYLE, (POINTER_type) rval->presStyle, ALL_PARMS);
	SafeSetAttr(rval->fakeBLO, at_LAYOUT_STYLE, (POINTER_type) rval->layoutStyle, ALL_PARMS);

	/* Attach a content marker for the fakeBLO */
	SafeSetAttr(rval->fakeBLO, at_CONTENT_ARCH_CLASS, (POINTER_type)  odalib_at_CONTENT_ARCH_CLASS_proc_cc(),  ALL_PARMS);
    }

    if (gD->doDebugging) {
	printf("Leaving CreateDVLPieces\n");
    }


    return rval;
}


static p_styleSheetInfo CreateCategoryInfo(gD, rval, whichdoc, ssname)
struct atkoda * gD;
p_styleSheetInfo rval;
DOCUMENT_type whichdoc;
char * ssname ;
{
    static INT_type nextPageType = 0;		    /* Counter for layout category */

    if (gD->doDebugging) {
	printf("Entering CreateCategoryInfo\n");
    }


    /* Make new layout category identification */
    nextPageType++;
    sprintf(tempStr,"text-%s-%d", ssname, nextPageType);
    if (strlen(tempStr) > (MAXSTRING-1)) {
	printf("Smashed memory in CreatePagePieces\n");
	return NULL;
    }
    if (gD->doDebugging) {
	printf("About to malloc %d\n", strlen(tempStr) + 1);
    }
    rval->textCategory = (char *) malloc(strlen(tempStr) + 1);
    if (rval->textCategory == NULL) {
	printf("Could not allocate space for layout category name %s\n");
    }
    else {
	(void) strcpy(rval->textCategory, tempStr);
    }

    /* And for the footnotes */
    sprintf(tempStr,"fnote-%s-%d", ssname, nextPageType);
    if (strlen(tempStr) > (MAXSTRING-1)) {
	printf("Smashed memory in CreatePagePieces\n");
	return NULL;
    }
    rval->fnoteCategory = (char *) malloc(strlen(tempStr) + 1);
    if (rval->fnoteCategory == NULL) {
	printf("Could not allocate space for layout category name %s\n");
    }
    else {
	(void) strcpy(rval->fnoteCategory, tempStr);
    }

    /* And create the styles for attaching to specific BLOs as necessary */
    rval->textLayCatStyle = odalib_MakeStyle(whichdoc, LAYOUT_STYLE);
    if (odalib_ERROR_CONSTITUENT() == rval->textLayCatStyle) {
	printf("Could not make layout style for text layout category for %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
    }
    else {
	sprintf(tempStr,"Layout style for BLO text layout category for stylesheet %s", ssname);
	if (strlen(tempStr) > (MAXSTRING-1)) {
	    printf("Smashed memory in CreatePagePieces\n");
	    return NULL;
	}
	(void) odalib_SetStringAttr(rval->textLayCatStyle, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);

	/* attach the layout category to the style */
	(void) odalib_SetStringAttr(rval->textLayCatStyle, at_LAYOUT_CATEGORY, rval->textCategory, DATA_CHAR_TYPE);
    }

    rval->fnoteLayCatStyle = odalib_MakeStyle(whichdoc, LAYOUT_STYLE);
    if (odalib_ERROR_CONSTITUENT() == rval->fnoteLayCatStyle) {
	printf("Could not make layout style for foot note layout category for %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
    }
    else {
	sprintf(tempStr,"Layout style for BLO fnote layout category for stylesheet %s", ssname);
	if (strlen(tempStr) > (MAXSTRING-1)) {
	    printf("Smashed memory in CreatePagePieces\n");
	    return NULL;
	}
	(void) odalib_SetStringAttr(rval->fnoteLayCatStyle, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);

	/* attach the layout category to the style */
	(void) odalib_SetStringAttr(rval->fnoteLayCatStyle, at_LAYOUT_CATEGORY, rval->fnoteCategory, DATA_CHAR_TYPE);
    }

    /* And create a style for CLO's to hold the new layout object info */
    rval->newLayObject.layout_object_tag = LAYOUT_OBJ_DESC_tag;
    rval->newLayObject.layout_object_value.constituent = rval->curPage;

    rval->newLayObjStyle = odalib_MakeStyle(whichdoc, LAYOUT_STYLE);
    if (odalib_ERROR_CONSTITUENT() == rval->newLayObjStyle) {
	printf("Could not create new layout object style for stylesheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
    }
    else {
	/* Have style, fill in information */
	sprintf(tempStr,"Layout style for new layout object on CLO for stylesheet %s", ssname);
	if (strlen(tempStr) > (MAXSTRING-1)) {
	    printf("Smashed memory in CreatePagePieces\n");
	    return NULL;
	}
	(void) odalib_SetStringAttr(rval->newLayObjStyle, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);
	SafeSetAttr(rval->newLayObjStyle, at_NEW_LAYOUT_OBJECT, (POINTER_type) &rval->newLayObject, ALL_PARMS);
    }

    /* All done with layout category information */
    if (gD->doDebugging) {
	printf("Leaving CreateCategoryInfo\n");
    }

    return rval;
} 

static INT_type CreatePagePiece(gD, newConstituent, whichdoc, ssname, objectType, NBSString, commentStr)
struct atkoda * gD;
CONSTITUENT * newConstituent;
DOCUMENT_type whichdoc;
char * ssname ; 
char * NBSString;
char * commentStr;{

    if(gD->doDebugging) {
	printf("Entering CreatePagePiece\n");
    }

      *newConstituent = odalib_MakeComponent(whichdoc, objectType, GENERIC_COMPONENT);
      if (odalib_ERROR_CONSTITUENT() == *newConstituent) {
	  printf("Could not create new object (%d) for stylesheet %s (%s)\n", objectType, ssname, ErrorMsg(odalib_TKError()));
	  return ERROR_INT;
      }
      (void) odalib_SetStringAttr(*newConstituent, at_APPLICATION_COMMENTS, NBSString, DATA_BYTE_TYPE);
      sprintf(tempStr, commentStr, ssname);
      if (strlen(tempStr) > (MAXSTRING-1)) {
	  printf("Smashed memory in CreatePagePiece\n");
	  return ERROR_INT;
      }
      (void) odalib_SetStringAttr(*newConstituent, at_USER_READABLE_COMMENTS, tempStr, DATA_CHAR_TYPE);


    if(gD->doDebugging) {
	printf("Leaving CreatePagePiece\n");
    }

      return SUCCESS_INT;
}

static INT_type TagCategory(gD, frame, ssname, categoryStr)
struct atkoda * gD;
CONSTITUENT frame;
char * ssname;
char * categoryStr; {
     at_PERMITTED_CATEGORIES_type PerCatSeq;	/* tagging body areas */

    if (gD->doDebugging) {
	printf("Entering TagCategory\n");
    }

    /* Have to tag the frame with appropriate category */
     PerCatSeq = odalib_MakeSequence(SEQUENCE_STRING_tag, 1);
     if (odalib_ERROR_SEQUENCE() == PerCatSeq) {
	 printf("Could not allocate space for permitted categories name %s for frame of stylesheet %s (%s)\n", categoryStr, ssname, ErrorMsg(odalib_TKError()));
	 return ERROR_INT;
     }
     PerCatSeq->sequence_value.strings[0] = odalib_Chars2String(categoryStr);
     if (odalib_ERROR_STRING() == PerCatSeq->sequence_value.strings[0]){
	 printf("Could not allocate space for permitted catgeory %s of stylesheet %s (%s)\n", categoryStr, ssname, ErrorMsg(odalib_TKError()));
	 return ERROR_INT;
     }
     SafeSetAttr(frame, at_PERMITTED_CATEGORIES, (POINTER_type) PerCatSeq, ALL_PARMS);
     if (ERROR_INT == odalib_DeleteSequence(PerCatSeq->sequence_value.strings[0])) {
	 printf("Could not delete space for permitted category name %s of stylesheet %s (%s)\n", categoryStr, ssname, ErrorMsg(odalib_TKError()));
	 /* return ERROR_INT; */
     }
     if (ERROR_INT == odalib_DeleteSequence(PerCatSeq)) {
	 printf("Could not delete space for permitted category attribute %s for stylesheet %s (%s)\n", categoryStr, ssname, ErrorMsg(odalib_TKError()));
	 /* return ERROR_INT; */
     }

    if (gD->doDebugging) {
	printf("Leaving TagCategory\n");
    }

     return SUCCESS_INT;
}

static p_styleSheetInfo CreatePagePieces(gD, ssInfo, whichdoc, ssname)
struct atkoda * gD;
p_styleSheetInfo ssInfo;
DOCUMENT_type whichdoc;
char * ssname ;
{
    /* Creates a generic page for a new stylesheet */


    /* Intermediate pieces created, but otherwise unreferenced */
      CONSTITUENT   FrameCText;		/* Text frame inside of body */
      CONSTITUENT   FrameFFNote;	/* Footnote frame inside of body */
      at_GEN_FOR_SUBORDINATES_type TopNode;   /* temporary construction expression to link together frames */

    if(gD->doDebugging) {
	printf("Entering CreatePagePieces\n");
    }

    /* First create an appropriate page */
    if (ERROR_INT == CreatePagePiece(gD, &ssInfo->curPage, whichdoc, ssname, at_OBJECT_TYPE_page, "Page", "Generic page for stylesheet %s")) {
	return NULL;
    }

    /* Fill in layout category information for referencing page */
    ssInfo = CreateCategoryInfo(gD, ssInfo, whichdoc, ssname);
    if (NULL == ssInfo) {
	ssInfo = NULL;
	goto LEAVE;
    }

    /* And the header frame */
    if (ERROR_INT == CreatePagePiece(gD, &ssInfo->headerFrame, whichdoc, ssname, at_OBJECT_TYPE_frame, "Header", "Generic frame for header for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }
    /* And its internal "source" frame */
    if (ERROR_INT == CreatePagePiece(gD, &ssInfo->hdrSourceFrame, whichdoc, ssname, at_OBJECT_TYPE_frame, "FrameH", "Generic frame for sourcing headers for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }

    /* And the body frame */
    if (ERROR_INT == CreatePagePiece(gD, &ssInfo->bodyFrame, whichdoc, ssname, at_OBJECT_TYPE_frame, "BodyArea", "Generic frame for body area for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }
    /* And the local internal frames for text and footnotes */
    if (ERROR_INT == CreatePagePiece(gD, &FrameCText, whichdoc, ssname, at_OBJECT_TYPE_frame, "FrameC", "Generic frame for body text area for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }
    TagCategory(gD, FrameCText, ssname, ssInfo->textCategory);
    if (ERROR_INT == CreatePagePiece(gD, &FrameFFNote, whichdoc, ssname, at_OBJECT_TYPE_frame, "FrameF", "Generic frame for foot note body area for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }

    TagCategory(gD, FrameFFNote, ssname, ssInfo->fnoteCategory);

    /* And the footer */
    if (ERROR_INT == CreatePagePiece(gD, &ssInfo->footerFrame, whichdoc, ssname, at_OBJECT_TYPE_frame, "Footer", "Generic frame for footer for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }
    /* And its internal "source" frame */
    if (ERROR_INT == CreatePagePiece(gD, &ssInfo->ftrSourceFrame, whichdoc, ssname, at_OBJECT_TYPE_frame, "FrameH", "Generic frame for sourcing footers for stylesheet %s")) {
	ssInfo = NULL;
	goto LEAVE;
    }

    /* Now connect together all of these frames */

      /* Connect HDR, Body, FTR to Page Note: we will reuse the sequence */
      TopNode.construction_tag = SEQUENCE_tag;
      TopNode.construction_value.construction = odalib_MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, 3);
      if (odalib_ERROR_SEQUENCE() == TopNode.construction_value.construction) {
	  printf("Could not allocate sequence terms for hdr/body/ftr for stylesheet %s (%s)\n",ssname, ErrorMsg(odalib_TKError()));
	  ssInfo = NULL;
	  goto LEAVE;
      }
      TopNode.construction_value.construction-> sequence_value.construction_terms[0].term_tag = REQ_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[0].factor.factor_tag = OBJECT_CLASS_ID_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[0].factor.factor_value.object_class_id = ssInfo->headerFrame;
      TopNode.construction_value.construction-> sequence_value.construction_terms[1].term_tag = REQ_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[1].factor.factor_tag = OBJECT_CLASS_ID_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[1].factor.factor_value.object_class_id = ssInfo->bodyFrame;
      TopNode.construction_value.construction-> sequence_value.construction_terms[2].term_tag = REQ_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[2].factor.factor_tag = OBJECT_CLASS_ID_tag;
      TopNode.construction_value.construction->sequence_value.construction_terms[2].factor.factor_value.object_class_id = ssInfo->footerFrame;
      SafeSetAttr(ssInfo->curPage, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

      /* Place text and footnote sections onto body Note: we reuse sequence from above */
      TopNode.construction_tag = SEQUENCE_tag;
      if (ERROR_INT == odalib_ContractSequence(TopNode.construction_value.construction, 1)) {
	  printf("Could not contract sequence for building page's text/footnote parts for style sheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
	  ssInfo = NULL;
	  goto LEAVE;
      }
      TopNode.construction_value.construction-> sequence_value.construction_terms[0].term_tag = REQ_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[0].factor.factor_tag = OBJECT_CLASS_ID_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[0].factor.factor_value.object_class_id = FrameCText;
      TopNode.construction_value.construction-> sequence_value.construction_terms[1].term_tag = REQ_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[1].factor.factor_tag = OBJECT_CLASS_ID_tag;
      TopNode.construction_value.construction-> sequence_value.construction_terms[1].factor.factor_value.object_class_id = FrameFFNote;
      SafeSetAttr(ssInfo->bodyFrame, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

      if (ERROR_INT == odalib_DeleteSequence(TopNode.construction_value.construction)) {
	  printf("Could not delete sequence used for building body frames for stylesheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
	  /* return NULL; */
      }

      /* Place a FrameH into header and footer (reuse all fields except final object id) */
      TopNode.construction_tag = TERM_tag;
      TopNode.construction_value.term.term_tag = REQ_tag;
      TopNode.construction_value.term.factor.factor_tag = OBJECT_CLASS_ID_tag;
      TopNode.construction_value.term.factor.factor_value.object_class_id = ssInfo->hdrSourceFrame;
      SafeSetAttr(ssInfo->headerFrame, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

      TopNode.construction_value.term.factor.factor_value.object_class_id = ssInfo->ftrSourceFrame;
      SafeSetAttr(ssInfo->footerFrame, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

LEAVE:
    if(gD->doDebugging) {
	printf("Leaving CreatePagePieces (rval=%x)\n", ssInfo);
    }

      
      return ssInfo;
}

static p_styleSheetInfo AttachPageToPageSet(gD, rval, whichdoc, ssname)
struct atkoda * gD;
p_styleSheetInfo rval;
DOCUMENT_type whichdoc;
char * ssname; {
    p_docPageSet curPageBlk;	    /* Index into page/doc table */
    CONSTITUENT	pSet;		    /* Page set for this document */
    at_GEN_FOR_SUBORDINATES_type * TopPart;
    INT_type		    LastIndex;
    INT_type		    status;


    if (gD->doDebugging) {
	printf("Entering AttachPageToPageSet\n");
    }

    /* Find the pageset for this document */
    for(curPageBlk=pageSetList; curPageBlk && (curPageBlk->doc != whichdoc); curPageBlk = curPageBlk->next);
    if (curPageBlk == NULL) {
	/* No block with this doc/pageset, so an error has occured */
	printf("Missing pageset block for document %d/stylesheet %s\n", whichdoc, ssname);
	rval = NULL;
	goto LEAVE;
    }

    pSet = curPageBlk->curPageSet;
    if (pSet == NULL) {
	/* no page set for this document,so an error */
	printf("Missing pageset for document %d/stylesheet %s\n", whichdoc, ssname);
	rval = NULL;
	goto LEAVE;
    }

    /* Add the page to the page set */
    /* Any pages added yet? If so, reuse, else create */

    status = odalib_HasAttr(pSet, at_GEN_FOR_SUBORDINATES);
    switch(status){
	case ERROR_INT: 
	    printf("Error return on hasattr for stylesheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
	    rval = NULL;
	    goto LEAVE;
	    /* break; */
	case HAS_ATTR:
	    if (ERROR_INT == odalib_GetAttr(pSet, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopPart, BOOL_false, ( (PARM_MASK_type *) 0))) {
		printf("Could not get generator for subordinates from pageset for stylesheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
		return NULL;
	    }
	    if (ERROR_INT == odalib_ExpandSequence(TopPart->construction_value.construction, 1)) {
		printf("Could not expand sequence for gen for sub for pageset for stylesheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
		return NULL;
	    }
	    LastIndex = TopPart->construction_value.construction->length-1;
	    break;
	case NO_ATTR:
	    TopPart = (at_GEN_FOR_SUBORDINATES_type *) malloc(sizeof(at_GEN_FOR_SUBORDINATES_type));
	    TopPart->construction_tag = SEQUENCE_tag;
	    TopPart->construction_value.construction = odalib_MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, 1);
	    if (odalib_ERROR_SEQUENCE() == TopPart->construction_value.construction) {
		printf("Could not allocate sequence terms for pages in pageset for stylesheet %s (%s)\n", ssname, ErrorMsg(odalib_TKError()));
		return NULL;
	    }
	    LastIndex = 0;
	    break;
	default:
	    printf("Unknown return values from HasAttr %d\n",status);
	    break;
    }

    /* Add new page to sequence and set the attribute */
    TopPart->construction_value.construction-> sequence_value.construction_terms[LastIndex].term_tag = OPT_REP_tag;
    TopPart->construction_value.construction-> sequence_value.construction_terms[LastIndex].factor.factor_tag = OBJECT_CLASS_ID_tag;
    TopPart->construction_value.construction-> sequence_value.construction_terms[LastIndex].factor.factor_value.object_class_id = rval->curPage;
    SafeSetAttr(pSet, at_GEN_FOR_SUBORDINATES, (POINTER_type) TopPart, ALL_PARMS);

    /* Free up local copy of everything */
    if (ERROR_INT == odalib_DeleteSequence(TopPart->construction_value.construction)) {
	printf("Could not deallocate gen for subordinate sequence\n (%s)", ErrorMsg(odalib_TKError()));
	/* return ERROR_INT; */
    }
    if (ERROR_INT == odalib_LOCAL_Free(TopPart)) {
	printf("Could not delete generator for suboridnates construction expression (%s)\n", ErrorMsg(odalib_TKError()));
	/* return ERROR_INT; */
    }

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving AttachPageToPageSet\n");
    }

    return rval;

}

void CreateATKPieces(gD, ssBlk, ATKStyle, makePage)
struct atkoda * gD;
p_styleSheetInfo ssBlk; 
struct style * ATKStyle; 
BOOL_type makePage; {
    CONSTITUENT genericPassage;
    CONSTITUENT genericPhrase;

    if (gD->doDebugging) {
        printf("Entering CreateATKPieces %X %X %X\n", gD, ssBlk, ATKStyle);
    }

    /* Save style for later use */
    ssBlk->ATKStyle = ATKStyle;

    /* Calculate the equivalent FSV for this style */
    text_InitStateVector(&ssBlk->SV);
    text_ApplyEnvironment(&ssBlk->SV, gD->globalStyle, (struct environment *) NULL);
    text_ApplyEnvironment(&ssBlk->SV, ssBlk->ATKStyle, (struct environment *) NULL);

    /* Make the actual pieces of complete generic structure */
    genericPassage = MakePassage(gD, ssBlk->ATKStyle, GENERIC_COMPONENT);
    genericPhrase = MakePhrase(gD, ssBlk->ATKStyle, GENERIC_COMPONENT);

    ssBlk->passage = genericPassage;
    ssBlk->phrase = genericPhrase;

    /* Attach the style info (which should have been calculated by now) */
    /* In this first pass, every style has all possible information */

    if (ssBlk->ATKStyle->name) {
	(void) odalib_SetStringAttr(genericPassage, at_USER_VISIBLE_NAME, ssBlk->ATKStyle->name, DATA_CHAR_TYPE);
	(void) odalib_SetStringAttr(genericPhrase, at_USER_VISIBLE_NAME, ssBlk->ATKStyle->name, DATA_CHAR_TYPE);
    }
    odalib_SetAttr(genericPassage, at_DEFAULT_VALUE_LISTS, (POINTER_type) &ssBlk->styleList, ALL_PARMS);
    odalib_SetAttr(genericPhrase, at_DEFAULT_VALUE_LISTS, (POINTER_type) &ssBlk->styleList, ALL_PARMS);
    if (makePage) {
	odalib_SetAttr(genericPassage, at_LAYOUT_STYLE, (POINTER_type) ssBlk->newLayObjStyle, ALL_PARMS);
	odalib_SetAttr(genericPhrase, at_LAYOUT_STYLE, (POINTER_type) ssBlk->newLayObjStyle, ALL_PARMS);
    }

    if (gD->doDebugging) {
        printf("Leaving CreateATKPieces\n");
    }

}


p_styleSheetInfo CreateEmptyODAStyleSheet(gD, whichdoc, ATKStyle, makeLStyle, makePStyle, makePage)
struct atkoda * gD;
struct style * ATKStyle;
DOCUMENT_type whichdoc; 
BOOL_type makePStyle;
BOOL_type makeLStyle;
BOOL_type makePage; {

    p_styleSheetInfo  rval;			    /* returned style sheet */
    char * ssname;
    static char undefinedName[] = "Undefined";

    if (gD->doDebugging) {
	printf("Entering CreateEmptyODAStyleSheet %X, %X, %X, %d, %d, %d\n", gD, whichdoc,ATKStyle, makeLStyle, makePStyle, makePage);
    }

    /* Create new block and link it */
    rval = (p_styleSheetInfo) malloc(sizeof(styleSheetInfo));
    if (rval == NULL) {
	printf("Could not malloc space for style sheet information block\n");
	return rval;
    }

    rval->next = gD->styleList;
    gD->styleList = rval;

    if ((ATKStyle) && ATKStyle->name) {
	ssname = ATKStyle->name;
    }
    else {
	ssname = undefinedName;
    }

    /* Fill in values for various pieces - first name */
    rval->name = (char *) malloc(strlen(ssname) +1);
    if (rval->name == NULL) {
	printf("Could not get enough space to hold stylesheet name%s\n", ssname);
    }
    else {
	(void) strcpy(rval->name, ssname);
    }

    /* And remember which document this came from */
    rval->curDoc = whichdoc;

    /* Create styles and dvl for attaching to CLOs */
    rval = CreateDVLPieces(gD, rval, whichdoc, ssname, makeLStyle, makePStyle);

    if (makePage) {
	/* Create new page with appropriate structure */
	rval = CreatePagePieces(gD,rval, whichdoc, ssname);

	/* Attach Page to pageset */
	rval = AttachPageToPageSet(gD, rval, whichdoc, ssname);
    }

    /* Do ATK specific stuff */
    if (ATKStyle) {
	CreateATKPieces(gD, rval, ATKStyle, makePage);
    }
    else {
	rval->ATKStyle = NULL;
    }

    if (gD->doDebugging) {
	printf("Leaving CreateEmptyODAStyleSheet\n");
    }

    return rval;
    
}

INT_type CreateEmptyODADocStyle(gD,whichdoc)
struct atkoda * gD;
DOCUMENT_type whichdoc;
{
    /* This procedure initializes the pageset structures for use by the style sheets */

    CONSTITUENT	pageSet;
    CONSTITUENT LayDoc;
    p_docPageSet tempPageSetBlk;
    at_GEN_FOR_SUBORDINATES_type TopNode;

    if (gD->doDebugging) {
	printf("Entering CreateEmptyODADocStyle %X, %X\n", gD, whichdoc);
    }

    LayDoc = gD->genericLayoutRoot;
#if 0
    LayDoc = odalib_MakeComponent(whichdoc, at_OBJECT_TYPE_doc_layout_root, GENERIC_COMPONENT);
    if (odalib_ERROR_CONSTITUENT() == LayDoc) {
	printf("Could not make generic layout document root %s\n", ErrorMsg(odalib_TKError()));
	return ERROR_INT;
    }
#endif
    pageSet = odalib_MakeComponent(whichdoc, at_OBJECT_TYPE_page_set, GENERIC_COMPONENT);
    gD->pSet = pageSet;
    if (odalib_ERROR_CONSTITUENT() == pageSet) {
	printf("Could not make generic pageset %s\n", ErrorMsg(odalib_TKError()));
	return ERROR_INT;
    }

    /* LayDoc */
    (void) odalib_SetStringAttr(LayDoc, at_APPLICATION_COMMENTS, "Laydoc", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(LayDoc, at_USER_READABLE_COMMENTS, "Generic Layout Root", DATA_CHAR_TYPE);

    /* PageSet */
    (void) odalib_SetStringAttr(pageSet, at_APPLICATION_COMMENTS, "PageSet", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(pageSet, at_USER_READABLE_COMMENTS, "Generic PageSet", DATA_CHAR_TYPE);

    tempPageSetBlk = (p_docPageSet) malloc(sizeof(docPageSet));
    tempPageSetBlk->next = pageSetList;
    pageSetList = tempPageSetBlk;
    tempPageSetBlk->doc = whichdoc;
    tempPageSetBlk->curPageSet = pageSet;

    /* Connect Pageset to root */
    TopNode.construction_tag = TERM_tag;
    TopNode.construction_value.term.term_tag = REQ_tag;
    TopNode.construction_value.term.factor.factor_tag = OBJECT_CLASS_ID_tag;
    TopNode.construction_value.term.factor.factor_value.object_class_id = pageSet;
    SafeSetAttr(LayDoc, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

    /* Make the empty (null) new layout object */
    nullNewLayoutObject.layout_object_tag = NULL_tag;
    /* And create a style to hold the null layout object */
    nullNLOLayoutStyle = odalib_MakeStyle(whichdoc, LAYOUT_STYLE);
    if (odalib_ERROR_CONSTITUENT() == nullNLOLayoutStyle) {
	printf("Could not make layout style to hold null new layout object %s\n", ErrorMsg(odalib_TKError()));
    }
    else {
	SafeSetAttr(nullNLOLayoutStyle, at_NEW_LAYOUT_OBJECT, (POINTER_type) &nullNewLayoutObject, ALL_PARMS);
    }

    if (gD->doDebugging) {
	printf("Leaving CreateEmptyODADocStyle\n");
    }
    return SUCCESS_INT;
}


p_styleSheetInfo LookupSSInfo(gD, whichStyle)
struct atkoda * gD;
struct style * whichStyle; {
    p_styleSheetInfo rval;			    /* returned style sheet */

    if (gD->doDebugging) {
        printf("Entering LookupSSInfo with %X\n", whichStyle);
    }

    for(rval=gD->styleList; (rval != (p_styleSheetInfo) NULL) && (rval->ATKStyle != whichStyle); rval = rval->next);

    if (gD->doDebugging) {
        printf("Leaving LookupSSInfo with %X\n", rval);
    }

    return rval;

}

