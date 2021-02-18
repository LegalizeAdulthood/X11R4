/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


/* Global include file for all modules that implement atk <-> oda conversions */
#include <stdio.h>
#include <styles.h>
#include <style.ih>
#include <txtstvec.h>
#include <im.ih>

typedef struct passage_stk_blk_struct {
    CONSTITUENT	passage;
    struct text_statevector passageSV;
    struct style * passageStyle;
    BOOL_type	passageHasChildren;
} passage_stk_blk, * p_passage_stk_blk;

typedef struct phrase_stk_blk_struct {
    CONSTITUENT	phrase;
    struct text_statevector phraseSV;
    struct style * phraseStyle;
} phrase_stk_blk, * p_phrase_stk_blk;

typedef struct font_list_blk_struct {
    struct font_list_blk_struct	    *next;
    FONT_REFERENCE_type		    ODAFontInfo;
    INT_type			    ODAFontID;
    BOOL_type			    BasedOnStyle;
    /* Shared information from both FSV and Style */
    long LineSpacing;		    /* Line spacing based on display system */
    /* Information from FSV */
    char *CurFontFamily;
    long CurFontAttributes;
    long CurScriptMovement;
    long CurFontSize;
    long SpecialFlags;
    /* Information from Style structure */
    char *FontFamily;
    struct fontsizestyle FontSize;
    struct fontscriptstyle FontScript;
    long AddFontFaces;      /* OR'ed FontFaces */
    long OutFontFaces;      /* NOT'ed OR'ed FontFaces */
    long AddMiscFlags;      /* Hinge, Pagebreak, Hidden, ... */
    long OutMiscFlags;
} font_list_blk, *p_font_list_blk;

class atkoda {
methods:
    PushPassage(CONSTITUENT newPassage, struct text_statevector * newStateVector, struct style * newStyle);
    PopPassage();
    TopPassage() returns CONSTITUENT;
    TopPassageSV(struct text_statevector * returnSV);
    HasTopPassageChildren() returns BOOL_type;
    AddTopPassageChild(CONSTITUENT newChild);
    ClearPhraseStack();
    AppendBLOToPhrase(CONSTITUENT newBLO, CONSTITUENT CP, p_font_list_blk fontBlk);
    SetParagraph(CONSTITUENT newPara);
    AddCurParagraphToHighestPassage();
    TopPassageStyle() returns struct style *;
    PushPhrase(CONSTITUENT newPhrase, struct text_statevector * newStateVector, struct style * newStyle);
    PopPhrase();
    TopPhrase() returns CONSTITUENT;
    OpenParagraph() returns BOOL_type;
    CurParagraph() returns CONSTITUENT;
    CVDotstoSMU(INT_type DotValue) returns INT_type;
    CVDotsPerInchToSMU(INT_type DotDensityPerInch, INT_type NumOfDots) returns INT_type;
    MakeNewStyleBlock(BOOL_type FromStyle) returns p_font_list_blk;
    FindFontFromSV(struct text_statevector *SV) returns p_font_list_blk;
    FindFontFromStyle(struct style * curStyle) returns p_font_list_blk;
    InitializeODAState();
data:
    INIT_PARMS_type ODAParams;    
    char * inputFileName;
    char * outputFileName;
    FILE * outputFileDescriptor;
    DOCUMENT_type outputDocument;
    struct dataobject * topLevelDocument;
    struct view * topLevelView;
    CONSTITUENT specificLogicalRoot;
    CONSTITUENT genericLogicalRoot;
    CONSTITUENT genericLayoutRoot;
    CONSTITUENT documentProfile;
    CONSTITUENT		genericFigure;
    CONSTITUENT		genericParagraph;
    CONSTITUENT		genericText;
    CONSTITUENT		genericRaster;
    CONSTITUENT		genericTable;
    CONSTITUENT		genericEquation;
    CONSTITUENT		pSet;
    p_styleSheetInfo	styleList;
    INT_type		passageSize;
    INT_type		passageTop;
    p_passage_stk_blk	passageStack;
    INT_type		phraseSize;
    INT_type		phraseTop;
    p_phrase_stk_blk	phraseStack;
    CONSTITUENT		firstBLO;
    cc_CHARACTER_FONTS_type firstBLOsFonts;
    INT_type		numTabsSinceLastCR;
    CONSTITUENT		highestPassageForPara;
    struct style *	globalStyle;
    struct text_statevector rootSV;
    p_font_list_blk	fontList;
    at_LAYOUT_CATEGORY_type curTextLC;
    at_LAYOUT_CATEGORY_type curFNoteLC;
    BOOL_type		doDebugging;
    INT_type		SMU_Numerator;
    INT_type		SMU_Denominator;
    INT_type		NumCRsInParaBreak;
    INT_type		NumCRsLeftinParaBreak;
    INT_type		LastParaBreakFound;
    INT_type		LastParaBreakGenerated;
    BOOL_type		ScaleRaster;
    BOOL_type		doFudgeMargins;
    struct im		*windowSystemIM;
};
