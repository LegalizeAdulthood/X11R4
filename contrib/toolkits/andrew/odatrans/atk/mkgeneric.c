/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgeneric.c,v 1.2 89/09/08 17:28:24 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgeneric.c,v $ */


/*
  $Log:	mkgeneric.c,v $
 * Revision 1.2  89/09/08  17:28:24  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:21:24  mss
 * Initial revision
 * 
 * Revision 1.10  89/08/25  10:16:59  mss
 * Changed some local att support routines into toolkit calls
 * 
 * Revision 1.9  89/04/30  12:07:21  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.8  89/03/23  06:59:58  mss
 * Changed to use new component and DVL specifications
 * 
 * Revision 1.7  88/11/02  16:56:35  mss
 * FIxed bugs with labeling generic tables and equations
 * 
 * Revision 1.6  88/10/31  21:31:36  mss
 * Added more style function (fonts/margins)
 * 
 * Revision 1.5  88/10/25  15:35:27  mss
 * Added more style processing/default processing
 * 
 * Revision 1.4  88/10/17  11:58:53  mss
 * Added right margin generation
 * 
 * Revision 1.3  88/10/10  18:52:59  mss
 * revised tracing
 * 
 * Revision 1.2  88/09/22  11:10:15  mss
 * Code rearranged a little
 * 
 * Revision 1.1  88/09/09  16:44:42  mss
 * Initial revision
 * 


*/

#include <class.h>

#include <odatk.h>
#include <odadef.h>
#include <odalib.ih>
#include <attsup.h>
#include <atkoda.ih>
#include <txtstvec.h>
#include <styles.h>
#include <text.ih>
#include <stylesht.ih>
#include <mkgeneric.h>
#include <mkstylediff.h>
#include <mkgenstyle.h>

#define tracing 1

/* Forward declaration */
BOOL_type CreateOneStyle(/*curStyle, gD*/);
void MakePhraseSubordinates(/*gD,  PhraseSub*/);
void MakePassageSubordinates(/*gD, PassageSub*/);
void ConnectPassages(/*gD, PassageSub*/);
void ConnectPhrases(/*gD, PhraseSub*/);
void ConnectFigure(/*gD*/);
void DeletePassageSubordinates(/*gD, PassageSub*/);
void DeletePhraseSubordinates(/*gD, PhraseSub*/);

static INT_type styleCount;

INT_type CreateODAGenericStructure(gD)
struct atkoda * gD; {
    INT_type	rval;
    at_GEN_FOR_SUBORDINATES_type    PassageSub;
    at_GEN_FOR_SUBORDINATES_type    PhraseSub;


    if (gD->doDebugging) {
        printf("Entering CreateODAGenericStructure %X\n",gD);
    }

    rval = SUCCESS_INT;

    /* Initialize the style structures */
    (void) CreateEmptyODADocStyle(gD,gD->outputDocument);

    /* Put together the global (default) environment */
    (void) MakeGlobalODAStructure(gD);

    /* Process each ATK style (and keep a count) */
    styleCount = 0;
    (void) stylesheet_EnumerateStyles(((struct text *)(gD->topLevelDocument))->styleSheet, CreateOneStyle, (long) gD);

    /* Create the one-of-a-kind structures */
    (void) MakeGenericParagraph(gD);
    (void) MakeGenericFigure(gD);
    (void) MakeGenericText(gD);
    (void) MakeGenericRaster(gD);
    (void) MakeGenericTable(gD);
    (void) MakeGenericEq(gD);

    /* Retrieve all passages and phrases for an attribute */
    MakePhraseSubordinates(gD, &PhraseSub);
    MakePassageSubordinates(gD, &PassageSub);

    /* Connect together everything */
    odalib_SetAttr(gD->genericLogicalRoot, at_GEN_FOR_SUBORDINATES, (POINTER_type) &PassageSub, Ignore_Parms);
    ConnectPassages(gD, &PassageSub);
    ConnectPhrases(gD, &PhraseSub);
    ConnectFigure(gD);
    odalib_SetAttr(gD->genericParagraph, at_GEN_FOR_SUBORDINATES, (POINTER_type) &PhraseSub, Ignore_Parms);

    /* Throw away temporaries */
    DeletePassageSubordinates(gD, &PassageSub);
    DeletePhraseSubordinates(gD, &PhraseSub);

    if (gD->doDebugging) {
        printf("Leaving CreateODAGenericStructure\n");
    }

    return rval;

}

/* Take the global SV and figure out all of the global defaults */
INT_type MakeGlobalODAStructure(gD)
struct atkoda * gD; {
    p_styleSheetInfo dummyStyleBlk;
    INT_type rval;

    if (gD->doDebugging) {
	printf("Entering MakeGlobalODAStructure\n");
    }

    /* First, set up an empty collection of all needed stuff page shoudl be true */
    dummyStyleBlk = CreateEmptyODAStyleSheet(gD, gD->outputDocument, NULL, /* LStyle */ BOOL_true, /* P Style */ BOOL_true, /* Page */ BOOL_true);

    /* And fill in the information */
    rval = MakeDefaultState(gD, &gD->rootSV, dummyStyleBlk);

    /* And plug it in where appropriate */
    odalib_SetAttr(gD->specificLogicalRoot, at_DEFAULT_VALUE_LISTS, (POINTER_type) &dummyStyleBlk->styleList, Ignore_Parms);
#if 0
    odalib_SetAttr(gD->documentProfile, dp_DOC_APP_PROF_DEFAULTS, (POINTER_type) dummyStyleBlk->styleList, Ignore_Parms);
#endif

LEAVE:

    if (gD->doDebugging) {
	printf("Leaving MakeGlobalODAStructure\n");
    }

    return rval;
}


static BOOL_type CreateOneStyle(curStyle, gD)
struct style *curStyle;
struct atkoda * gD; {
      long NumStyleChanges;
      p_styleSheetInfo newStyleBlk;

      if (gD->doDebugging) {
        printf("Entering CreateOneStyle %X, %X\n", curStyle, gD);
      }
      styleCount++;
      newStyleBlk = CreateEmptyODAStyleSheet(gD, gD->outputDocument, curStyle, /* LStyle */ BOOL_true, /* P Style */ BOOL_true, /* Page */ BOOL_false);
      NumStyleChanges = CalcStyleAttributes(gD, newStyleBlk, curStyle);

      if (gD->doDebugging) {
        printf("Leaving CreateOneStyle after making %d changes\n", NumStyleChanges);
      }

      return BOOL_false;    /* Not done enumerating them */
}

void MakePassageSubordinates(gD, PassageSub)
struct atkoda * gD;
at_GEN_FOR_SUBORDINATES_type *PassageSub;{
    p_styleSheetInfo	curStyleBlk;
    INT_type		i;

    if (gD->doDebugging) {
        printf("Entering MakePassageSubordinates\n");
    }

    PassageSub->construction_tag = TERM_tag;
    PassageSub->construction_value.term.term_tag = OPT_REP_tag;
    PassageSub->construction_value.term.factor.factor_tag = EXPR_tag;
    PassageSub->construction_value.term.factor.factor_value.expr = (CONSTRUCTION_EXPR_type *) malloc(sizeof(CONSTRUCTION_EXPR_type));
    if (PassageSub->construction_value.term.factor.factor_value.expr == ((CONSTRUCTION_EXPR_type *)NULL)) {
	printf("Could not allocate space for construction expression for passages's subordinates\n");
	return;
    }
    PassageSub->construction_value.term.factor.factor_value.expr->construction_tag = CHOICE_tag;
    PassageSub->construction_value.term.factor.factor_value.expr->construction_value.construction = (SEQUENCE_CONSTRUCTION_TERM_type) odalib_MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, styleCount+2); /* All passages + paragraph + figure */

    for(i=0, curStyleBlk = gD->styleList; curStyleBlk; curStyleBlk = curStyleBlk->next) {
	/* Only do real styles */
	if (curStyleBlk->ATKStyle) {
	    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].term_tag = OPT_REP_tag;
	    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].factor.factor_tag = OBJECT_CLASS_ID_tag;
	    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].factor.factor_value.object_class_id = curStyleBlk->passage;
	    i++;
	}
    }
    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount].term_tag = OPT_REP_tag;
    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount].factor.factor_tag = OBJECT_CLASS_ID_tag;
    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount].factor.factor_value.object_class_id = gD->genericParagraph;
    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount+1].term_tag = OPT_REP_tag;
    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount+1].factor.factor_tag = OBJECT_CLASS_ID_tag;
    PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount+1].factor.factor_value.object_class_id = gD->genericFigure;

    if (gD->doDebugging) {
        printf("Leaving MakePassageSubordinates\n");
    }

}

void MakePhraseSubordinates(gD, PhraseSub)
struct atkoda * gD;
at_GEN_FOR_SUBORDINATES_type *PhraseSub; {
    p_styleSheetInfo	curStyleBlk;
    INT_type		i;

    if (gD->doDebugging) {
        printf("Entering MakePhraseSubordinates\n");
    }

    PhraseSub->construction_tag = TERM_tag;
    PhraseSub->construction_value.term.term_tag = OPT_REP_tag;
    PhraseSub->construction_value.term.factor.factor_tag = EXPR_tag;
    PhraseSub->construction_value.term.factor.factor_value.expr = (CONSTRUCTION_EXPR_type *) malloc(sizeof(CONSTRUCTION_EXPR_type));
    if (PhraseSub->construction_value.term.factor.factor_value.expr == ((CONSTRUCTION_EXPR_type *)NULL)) {
	printf("Could not allocate space for construction expression for Phrases's subordinates\n");
	return;
    }
    PhraseSub->construction_value.term.factor.factor_value.expr->construction_tag = CHOICE_tag;
    PhraseSub->construction_value.term.factor.factor_value.expr->construction_value.construction = (SEQUENCE_CONSTRUCTION_TERM_type) odalib_MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, styleCount+1); /* All Phrases + text */

    for(i=0, curStyleBlk = gD->styleList; curStyleBlk; curStyleBlk = curStyleBlk->next) {
	/* Only do real styles */
	if(curStyleBlk->ATKStyle) {
	    PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].term_tag = OPT_REP_tag;
	    PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].factor.factor_tag = OBJECT_CLASS_ID_tag;
	    PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].factor.factor_value.object_class_id = curStyleBlk->phrase;
	    i++;
	}
    }
    PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount].term_tag = OPT_REP_tag;
    PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount].factor.factor_tag = OBJECT_CLASS_ID_tag;
    PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[styleCount].factor.factor_value.object_class_id = gD->genericText;

    if (gD->doDebugging) {
        printf("Leaving MakePhraseSubordinates\n");
    }

}

void DeletePhraseSubordinates(gD, PhraseSub)
struct atkoda * gD;
at_GEN_FOR_SUBORDINATES_type *PhraseSub; {

    odalib_DeleteSequence(PhraseSub->construction_value.term.factor.factor_value.expr-> construction_value.construction);
    free(PhraseSub->construction_value.term.factor.factor_value.expr);

}

void DeletePassageSubordinates(gD, PassageSub)
struct atkoda * gD;
at_GEN_FOR_SUBORDINATES_type *PassageSub; {

    odalib_DeleteSequence(PassageSub->construction_value.term.factor.factor_value.expr-> construction_value.construction);
    free(PassageSub->construction_value.term.factor.factor_value.expr);

}

void ConnectPassages(gD, PassageSub)
struct atkoda * gD;
at_GEN_FOR_SUBORDINATES_type * PassageSub; {
    p_styleSheetInfo	curStyleBlk;
    
    if(gD->doDebugging) {
	printf("Entering ConnectPassages\n");
    }

    for(curStyleBlk = gD->styleList; curStyleBlk != (p_styleSheetInfo)NULL; curStyleBlk = curStyleBlk->next) {
	/* Real style or just dummy? */
	if (curStyleBlk->ATKStyle) {
	    odalib_SetAttr(curStyleBlk->passage, at_GEN_FOR_SUBORDINATES, (POINTER_type) PassageSub, Ignore_Parms);
	}
    }

    if(gD->doDebugging) {
	printf("Leaving ConnectPassages\n");
    }



}

void ConnectPhrases(gD, PhraseSub)
struct atkoda * gD;
at_GEN_FOR_SUBORDINATES_type * PhraseSub; {
    p_styleSheetInfo	curStyleBlk;

    if(gD->doDebugging) {
	printf("Entering ConnectPhrases\n");
    }


    for(curStyleBlk = gD->styleList; curStyleBlk != (p_styleSheetInfo)NULL; curStyleBlk = curStyleBlk->next) {
	/* Real style or just dummy? */
	if (curStyleBlk->ATKStyle){
	    odalib_SetAttr(curStyleBlk->phrase, at_GEN_FOR_SUBORDINATES, (POINTER_type) PhraseSub, Ignore_Parms);
	}
    }


    if(gD->doDebugging) {
	printf("Leaving ConnectPhrases\n");
    }


}

void ConnectFigure(gD)
struct atkoda * gD; {
    at_GEN_FOR_SUBORDINATES_type figureSub;
    INT_type		    i;


    if(gD->doDebugging) {
	printf("Entering ConnectFigure\n");
    }


    figureSub.construction_tag = TERM_tag;
    figureSub.construction_value.term.term_tag = OPT_REP_tag;
    figureSub.construction_value.term.factor.factor_tag = EXPR_tag;
    figureSub.construction_value.term.factor.factor_value.expr = (CONSTRUCTION_EXPR_type *) malloc(sizeof(CONSTRUCTION_EXPR_type));
    if (figureSub.construction_value.term.factor.factor_value.expr == ((CONSTRUCTION_EXPR_type *)NULL)) {
	printf("Could not allocate space for construction expression for Figure's subordinates\n");
	return;
    }
    figureSub.construction_value.term.factor.factor_value.expr->construction_tag = CHOICE_tag;
    figureSub.construction_value.term.factor.factor_value.expr->construction_value.construction = (SEQUENCE_CONSTRUCTION_TERM_type) odalib_MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, 3); /* All Phrases + text */

    for(i=0; i<3; i++) {
	figureSub.construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].term_tag = OPT_REP_tag;
	figureSub.construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[i].factor.factor_tag = OBJECT_CLASS_ID_tag;
    }
    figureSub.construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[0].factor.factor_value.object_class_id = gD->genericRaster;
    figureSub.construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[1].factor.factor_value.object_class_id = gD->genericTable;
    figureSub.construction_value.term.factor.factor_value.expr-> construction_value.construction-> sequence_value.construction_terms[2].factor.factor_value.object_class_id = gD->genericEquation;

    odalib_SetAttr(gD->genericFigure, at_GEN_FOR_SUBORDINATES, (POINTER_type) &figureSub, ALL_PARMS);

    odalib_DeleteSequence(figureSub.construction_value.term.factor.factor_value.expr-> construction_value.construction);
    free(figureSub.construction_value.term.factor.factor_value.expr);
  
    if(gD->doDebugging) {
	printf("Leaving ConnectFigure\n");
    }

}



INT_type MakeGenericParagraph(gD)
struct atkoda * gD; {
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering MakeGenericParagraph\n");
    }

    rval = SUCCESS_INT;
    gD->genericParagraph = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_comp_logical_obj, GENERIC_COMPONENT);

    (void) odalib_SetStringAttr(gD->genericParagraph, at_APPLICATION_COMMENTS, "Paragraph", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(gD->genericParagraph, at_USER_READABLE_COMMENTS, "Generic paragraph for document", DATA_CHAR_TYPE);

    if (gD->doDebugging) {
        printf("Leaving MakeGenericParagraph\n");
    }

    return rval;
}

INT_type MakeGenericFigure(gD)
struct atkoda * gD; {
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering MakeGenericFigure\n");
    }

    rval = SUCCESS_INT;
    gD->genericFigure = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_comp_logical_obj, GENERIC_COMPONENT);

    (void) odalib_SetStringAttr(gD->genericFigure, at_APPLICATION_COMMENTS, "Figure", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(gD->genericFigure, at_USER_READABLE_COMMENTS, "Generic figure for document", DATA_CHAR_TYPE);

    if (gD->doDebugging) {
        printf("Leaving MakeGenericFigure\n");
    }

    return rval;
}

INT_type MakeGenericText(gD)
struct atkoda * gD; {
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering MakeGenericText\n");
    }

    rval = SUCCESS_INT;
    gD->genericText = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, GENERIC_COMPONENT);

    (void) odalib_SetStringAttr(gD->genericText, at_APPLICATION_COMMENTS, "Text", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(gD->genericText, at_USER_READABLE_COMMENTS, "Generic text for document", DATA_CHAR_TYPE);
    odalib_SetAttr(gD->genericText, at_CONTENT_ARCH_CLASS, (POINTER_type) odalib_at_CONTENT_ARCH_CLASS_proc_cc(), ALL_PARMS);
    if (gD->doDebugging) {
        printf("Leaving MakeGenericText\n");
    }

    return rval;
}

INT_type MakeGenericRaster(gD)
struct atkoda * gD; {
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering MakeGenericRaster\n");
    }

    rval = SUCCESS_INT;
    gD->genericRaster = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, GENERIC_COMPONENT);

    (void) odalib_SetStringAttr(gD->genericRaster, at_APPLICATION_COMMENTS, "Raster", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(gD->genericRaster, at_USER_READABLE_COMMENTS, "Generic Raster for document", DATA_CHAR_TYPE);
    odalib_SetAttr(gD->genericRaster, at_CONTENT_ARCH_CLASS, (POINTER_type) odalib_at_CONTENT_ARCH_CLASS_fp_rg(), ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeGenericRaster\n");
    }

    return rval;
}

INT_type MakeGenericTable(gD)
struct atkoda * gD; {
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering MakeGenericTable\n");
    }

    rval = SUCCESS_INT;
    gD->genericTable = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, GENERIC_COMPONENT);

    (void) odalib_SetStringAttr(gD->genericTable, at_APPLICATION_COMMENTS, "Table", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(gD->genericTable, at_USER_READABLE_COMMENTS, "Generic Table for document", DATA_CHAR_TYPE);
    odalib_SetAttr(gD->genericTable, at_CONTENT_ARCH_CLASS, (POINTER_type) odalib_at_CONTENT_ARCH_CLASS_proc_cc(), ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeGenericTable\n");
    }

    return rval;
}

INT_type MakeGenericEq(gD)
struct atkoda * gD; {
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering MakeGenericEq\n");
    }

    rval = SUCCESS_INT;
    gD->genericEquation = odalib_MakeComponent(gD->outputDocument, at_OBJECT_TYPE_bas_logical_obj, GENERIC_COMPONENT);

    (void) odalib_SetStringAttr(gD->genericEquation, at_APPLICATION_COMMENTS, "Equation", DATA_BYTE_TYPE);
    (void) odalib_SetStringAttr(gD->genericEquation, at_USER_READABLE_COMMENTS, "Generic Equation for document", DATA_CHAR_TYPE);
    odalib_SetAttr(gD->genericEquation, at_CONTENT_ARCH_CLASS, (POINTER_type) odalib_at_CONTENT_ARCH_CLASS_proc_cc(), ALL_PARMS);

    if (gD->doDebugging) {
        printf("Leaving MakeGenericEquation\n");
    }

    return rval;
}

