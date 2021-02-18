/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getdefault.c,v 1.3 89/09/11 13:55:44 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getdefault.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getdefault.c,v 1.3 89/09/11 13:55:44 mss Exp $";
#endif

/*
  $Log:	getdefault.c,v $
 * Revision 1.3  89/09/11  13:55:44  mss
 * Got rid of extraneous inclusion of text.ih
 * 
 * Revision 1.2  89/09/08  17:27:12  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:29  mss
 * Initial revision
 * 
 * Revision 1.9  89/08/25  10:15:59  mss
 * Changed some local att support routines into toolkit calls
 * 
 * Revision 1.8  89/04/30  12:06:46  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.7  89/03/23  07:05:01  mss
 * Uses new component and dvl specifications
 * 
 * Revision 1.6  88/12/13  20:52:02  mss
 * Changed NULL_SEQUENCE to use odalib class
 * procedure instead
 * 
 * Revision 1.5  88/11/17  13:42:54  mss
 * Fixed retrieval of cc_CHARACTER_FONTS to handle a
 * NULL_SEQUENCE (default value)
 * 
 * Revision 1.4  88/11/07  10:15:35  mss
 * Forced global style to have a menu entry
 * 
 * Revision 1.3  88/11/04  17:03:02  mss
 * Fixed pointer bug when comparing two strings
 * 
 * Revision 1.2  88/11/01  16:00:20  mss
 * Fixed getting of escapement class (making sure
 * it really is defined before being used)
 * 
 * Revision 1.1  88/10/31  21:35:33  mss
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



INT_type GetGlobalStyle(gD)
p_odainfo_type gD;
{
    CONSTITUENT	tempBLO, tempCLO;
    SEQUENCE_CONSTITUENT_type	subordinates;

    /* Creates the global style based on the root value in the specific */

    if (gD->doDebugging) {
        printf("Entering GetGlobalStyle\n");
    }

    gD->globalStyle = style_New();
    style_SetName(gD->globalStyle, "global");
    style_SetMenuName(gD->globalStyle, "Document,Global Defaults");
    
    /* Since we want to use the the values as they would appear by a basic/composite object as it is being laid out, we need to attach some dummy objects to the logical root so that we can use the inheritance rules without simulating them by hand. Note we are fast and loose about error checking here */

    tempBLO = odalib_MakeComponent(gD->odaDocument, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);
    odalib_SetAttr(tempBLO,  at_CONTENT_ARCH_CLASS, (POINTER_type)  odalib_at_CONTENT_ARCH_CLASS_proc_cc(),  ALL_PARMS);

    tempCLO = odalib_MakeComponent(gD->odaDocument, at_OBJECT_TYPE_comp_logical_obj, SPECIFIC_COMPONENT);

    (void) odalib_AppendSubordinate(gD->specificLogicalRoot,tempBLO);
    (void) odalib_AppendSubordinate(gD->specificLogicalRoot,tempCLO);

    /* Get initial justification (alignment) */
    GetDefaultJustification(gD, tempBLO);

    /* Get Paragraph Indentation */
    GetDefaultHorizonalMargins(gD, tempBLO);

    /* Get Interparagraph spacing (above, below) */

    /* Get Interline spacing */

    /* Get font information */
    GetDefaultFontInfo(gD, tempBLO);

    /* Get font size */

    /* Get font face */

    /* Get baseline offset */

    /* Get top margin */

    /* Get bottom margin */

    /* Get tab definitions */

    /*  install the global style */
    stylesheet_Add(gD->topLevelDocument->styleSheet, gD->globalStyle);
    text_SetGlobalStyle(gD->topLevelDocument, gD->globalStyle);
    
    /* Remove the fake objects from root */
    if (ERROR_INT != SafeGetAttr(gD->specificLogicalRoot, at_SUBORDINATES, (POINTER_type) &subordinates, Use_Local_Value, Ignore_Parms)) {
	(void) odalib_ContractSequence(subordinates,2);
	(void) odalib_SetAttr(gD->specificLogicalRoot, at_SUBORDINATES, (POINTER_type) subordinates, Ignore_Parms);
	(void) odalib_DeleteSequence(subordinates);
	(void) odalib_DeleteConstituent(tempBLO);
	(void) odalib_DeleteConstituent(tempCLO);
    }
    else {
	printf("Could not get fake subordinates to remove from style\n");
    }


    /* Finished calculating and installing the global style, so
      figure out what the global state vector looks like */
    text_InitStateVector(&gD->globalSV);
    text_ApplyEnvironment(&gD->globalSV, gD->globalStyle, (struct environment *) NULL);

    if (gD->doDebugging) {
        printf("Leaving GetGlobalStyle\n");
    }


    return SUCCESS_INT;
}



INT_type GetDefaultJustification(gD, BLO)
p_odainfo_type gD;
CONSTITUENT BLO; {
    INT_type		rval;
    cc_ALIGNMENT_type	curAlignment;

    if (gD->doDebugging) {
        printf("Entering GetDefaultJustification\n");
    }

    rval = SUCCESS_INT;

    if (ERROR_INT == odalib_GetValueAttr(BLO, cc_ALIGNMENT, (POINTER_type) &curAlignment, Use_Inherited)) {
	printf("Could not get value for global alignment %s\n", ErrorMsg(odalib_TKError()));
	curAlignment = cc_ALIGNMENT_start_aligned;
    }
    SetJustification(gD->globalStyle, curAlignment);

    if (gD->doDebugging) {
        printf("Leaving GetDefaultJustification\n");
    }

    return rval;
}

INT_type GetDefaultHorizonalMargins(gD, BLO)
p_odainfo_type gD;
CONSTITUENT BLO; {
    INT_type		    rval;
    cc_INDENTATION_type	    curIndentation; /* ODA text indentation */
    cc_1ST_LINE_OFFSET_type curFLOffset;    /* ODA 1st line offset */
    at_OFFSET_type	    *curOffset;	    /* ODA block offsets */
    long		    LM,RM;	    /* ATK Margins */
    long		    PIndentation;   /* ATK Para. Indentation */


    if (gD->doDebugging) {
        printf("Entering GetDefaultHorizontalMargins\n");
    }

    rval = SUCCESS_INT;

    if (ERROR_INT == odalib_GetIntAttr(BLO, cc_INDENTATION, (POINTER_type) &curIndentation, Use_Inherited)) {
	printf("Could not get value for global indentation %s\n", ErrorMsg(odalib_TKError()));
	curIndentation = 0;
    }
    else {
	if (gD->doDebugging) {
	    printf("Got cc_INDENTATION %d\n", curIndentation);
	}
    }

    if (ERROR_INT == odalib_GetIntAttr(BLO, cc_1ST_LINE_OFFSET, (POINTER_type) &curFLOffset, Use_Inherited)) {
	printf("Could not get value for first line offset %s\n", ErrorMsg(odalib_TKError()));
	curFLOffset = 0;
    }
    else {
	if (gD->doDebugging) {
	    printf("Got 1st_line_offset %d\n", curFLOffset);
	}
    }

    if (ERROR_INT == odalib_GetAttr(BLO, at_OFFSET, (POINTER_type) &curOffset, Use_Inherited, Ignore_Parms)) {
	printf("Could not get value for offset %s\n", ErrorMsg(odalib_TKError()));
	curOffset = (at_OFFSET_type *) malloc(sizeof(at_OFFSET_type));
	curOffset->left_hand_offset = 0;
	curOffset->right_hand_offset = 0;
    }
    else {
	if (gD->doDebugging) {
	    printf("Got offset %x, %d, %d\n", curOffset, curOffset->left_hand_offset, curOffset->right_hand_offset);
	}
    }


    LM = (long) GetDenseDotsFromSMUs(gD,
	    curOffset->right_hand_offset + curIndentation, 72);
    RM = (long) GetDenseDotsFromSMUs(gD,curOffset->left_hand_offset, 72);
    PIndentation = (long) GetDenseDotsFromSMUs(gD, curFLOffset, 72);

    if (gD->doDebugging) {
        printf("Entering About to set ATK margins\n");
    }

    style_SetNewLeftMargin(gD->globalStyle, style_ConstantMargin, LM, style_Points);
    style_SetNewRightMargin(gD->globalStyle, style_ConstantMargin, RM, style_Points);
    style_SetNewIndentation(gD->globalStyle, style_ConstantMargin, PIndentation, style_Points);

    free(curOffset);

    if (gD->doDebugging) {
        printf("Leaving GetDefaultHorizontalMargins\n");
    }

    return rval;
}

INT_type GetDefaultFontInfo(gD, BLO)
p_odainfo_type gD;
CONSTITUENT BLO; {
    INT_type			rval;
    cc_CHARACTER_FONTS_type	curCF;
    cc_GRAPHIC_RENDITION_type	curGR;
    INT_type			i;	    /* loop index */
    INT_type			CFIndex;    /* which cf to use */
    p_FontAsSV_type		defaultFont;	/* which font to examine */


    if (gD->doDebugging) {
        printf("Entering GetDefaultFontInfo\n");
    }

    rval = SUCCESS_INT;

    if (ERROR_INT == odalib_GetAttr(BLO, cc_CHARACTER_FONTS, (POINTER_type) &curCF, Use_Inherited, Ignore_Parms)) {
	printf("Could not get value for character fonts %s\n", ErrorMsg(odalib_TKError()));
        curCF = (cc_CHARACTER_FONTS_type) odalib_MakeSequence(SEQUENCE_INT_PAIR_tag,0);
    }

    /* Anything available? If not, make it appear empty */
    if (curCF == (cc_CHARACTER_FONTS_type) odalib_NULL_SEQUENCE()) {
        curCF = (cc_CHARACTER_FONTS_type) odalib_MakeSequence(SEQUENCE_INT_PAIR_tag,0);
    }

    if (ERROR_INT == odalib_GetAttr(BLO, cc_GRAPHIC_RENDITION, (POINTER_type) &curGR, Use_Inherited, Ignore_Parms)) {
	printf("Could not get value for graphic rendition %s\n", ErrorMsg(odalib_TKError()));
        curGR = (cc_GRAPHIC_RENDITION_type) odalib_MakeSequence(SEQUENCE_INT_tag,0);
    }

    /* See which character font to invoke */
    CFIndex = 0;	/* Assume primary font */
    for (i=0;i<curGR->length;i++) {
	/* Note: wired in knowledge about what GR means */
        if ((curGR->sequence_value.ints[i] >= 10) &&
	    (curGR->sequence_value.ints[i] <= 19)) {
	    CFIndex = curGR->sequence_value.ints[i] - 10;
	    }
    }

    /* See if we have that many invoked fonts */
    if (curCF->length != 0) {
	/* We have some fonts, so go for it */
	if (CFIndex > curCF->length-1) {
            printf("Default font %d beyond end of possible font list %d\n",
		    CFIndex, curCF->length-1);
	    CFIndex = 0;
        }

	defaultFont = FindSVforFont(gD, curCF->sequence_value.int_pairs[CFIndex].second);

	/* Have the font description, so extract the information bit by bit */
	if (defaultFont->nameDefined) {
	    style_SetFontFamily(gD->globalStyle, defaultFont->newFamilyName);
	}
	if (defaultFont->sizeDefined) {
	    style_SetFontSize(gD->globalStyle, style_ConstantFontSize, defaultFont->newFontSize);
	}
	if (defaultFont->facesDefined) {
	    style_ClearNewFontFaces(gD->globalStyle);
	    style_ClearOldFontFaces(gD->globalStyle);
	    style_AddNewFontFace(gD->globalStyle, defaultFont->newFaces);
	}
	if (defaultFont->flagsDefined) {
	    style_AddFlag(gD->globalStyle, defaultFont->newFlags);
	}
	if (defaultFont->scriptsDefined) {
	    style_SetFontScript(gD->globalStyle, style_ConstantScriptMovement, defaultFont->newScript, style_Points);
	}
    }


    odalib_DeleteSequence(curCF);
    odalib_DeleteSequence(curGR);

LEAVE:

    if (gD->doDebugging) {
        printf("Leaving GetDefaultFontInfo\n");
    }

    return rval;
}



static struct {
    char * AndyName;
    char * ODAName;
    VALUE_type DesignClass;	/*  0 => no class */
}  ODAFontNameMappingTable [] =
{
    {"AndySans",    "Linotype/Helvetica",   DSCLASS_SANS_gothic_neo },
    {"Andy",	    "Linotype/Times",	    DSCLASS_SERIFS_legi_rounded },
    {"AndyType",    "Adobe/Courier",	    DSCLASS_SERIFS_square_type },
    {"AndySymbol",  "Adobe/Symbol",	    0 },
    {"AndySans",    "Helvetica",	    DSCLASS_SANS_gothic_neo },
    {"Andy",	    "Times",		    DSCLASS_SERIFS_legi_rounded },
    {"AndyType",    "Courier",		    DSCLASS_SERIFS_square_type },
    {"AndySymbol",  "Symbol",		    0 }
};

#define NumODAFonts (sizeof(ODAFontNameMappingTable) / sizeof(ODAFontNameMappingTable[0]))


INT_type GetDefaultFontName(gD, defaultFont)
p_odainfo_type	    gD;
p_FontAsSV_type	    defaultFont;{
    INT_type	rval;
    char	*stringName;
    char	*onlyFontName;
    char	*orgName;
    char	*andyName;
    BOOL_type	foundName;
    VALUE_type	codedClass;
    INT_type	i;

    if (gD->doDebugging) {
        printf("Entering GetDefaultFontName\n");
    }

    rval = SUCCESS_INT;

    GetFontNameAsString(gD, defaultFont, &stringName, &orgName, &onlyFontName);

    foundName = BOOL_false;

    /* We will employ several heueristics:
	-look for exact match on name
	-look for match w/o organization name
	-look to see if special ITC organization font
	-check the escape for "fix-width-ness"
	-check the design class for serifs
    */

    /* Look for exact match with family */
    if (stringName) {
	/* Be creative: maybe an exact match, maybe only font name matches with a different organization, maybe only the font family w/ no irganization given */
	for (i=0;i<NumODAFonts;i++){
	    if ((0==strcmp(stringName, ODAFontNameMappingTable[i].ODAName))||
		(onlyFontName && (0==strcmp(onlyFontName, ODAFontNameMappingTable[i].ODAName)))||
		(orgName && (0==strcmp(orgName, ODAFontNameMappingTable[i].ODAName)))
		) {
		/* Have a winner */
		andyName = ODAFontNameMappingTable[i].AndyName;
		foundName = BOOL_true;
		break;
	    }
	}
    }

    /* See it's a special ITC font, if so, just take it on faith, identified by CMUITC organization */
    if (orgName && (0==strcmp(orgName,"CMUITC"))) {
	/* Done -- use the font name on faith */
	if (onlyFontName && (strlen(onlyFontName)>0)){
	    andyName = onlyFontName;
	}
	else {
	    andyName = "andy";
	}
    }

#if 0
    /* removed since ATK should do fixed escapement as a font substitution */
    if (!foundName) {
        /* try using escapement class */
	if ((defaultFont->theFont->modes.specified == BOOL_true) &&
	    (defaultFont->theFont->modes.iso_0.specified == BOOL_true) &&
	    (defaultFont->theFont->modes.iso_0.esclass == ISO_ESCLASS_fixed)){
	    andyName = "andytype";
	    foundName = BOOL_true;
	}
    }
#endif

    if (!foundName) {
	/* try using design class */
	if ((defaultFont->theFont->dsclass != odalib_NULL_SEQUENCE()) &&
	    (defaultFont->theFont->dsclass->length >0)) {
	    /* Well, something is defined, so try to use it */
	    codedClass = odalib_Ints2Dsclass(defaultFont->theFont->dsclass);
	    if ((codedClass != DSCLASS_ERROR) && (codedClass != DSCLASS_NOMATCH)) {
		for (i=0;i<NumODAFonts;i++){
		    if (codedClass == ODAFontNameMappingTable[i].DesignClass) {
			/* Have a winner */
			andyName = ODAFontNameMappingTable[i].AndyName;
			foundName = BOOL_true;
			break;
		    }
		}
	    }
	    /* Well, nothing from the coded class, so make a best guess based on gross design classification */
	    if (!foundName) {
		switch(defaultFont->theFont->dsclass->sequence_value.ints[0]){
			case 1: /* Uncials */
			case 2: /* Inscriptionals */
			case 3: /* Blackletters */
			    /* Note: if we were tense, we could look at 3rd entry to check for serifs or not */
			    andyName = "andy";
			    break;
			case 4: /* serifs */
			    andyName = "andy";	/* real serif font */
			    break;
			case 5: /* sans serif */
			    andyName = "andysans";  /* real sans serif font */
			    break;
			case 6: /* scripts */
			case 7: /* ornamentals */
			    andyName = "andy";	    /* tempted to use icon12 */
			    break;
			default:
			    andyName = "andy";
			    break;
		}
	    }
	    foundName = BOOL_true;
	}
    }

    /* Now save up the results */
    defaultFont->nameDefined = foundName;
    if (foundName) {
	defaultFont->newFamilyName = (char *) malloc(sizeof(andyName)+1);
	strcpy(defaultFont->newFamilyName, andyName);
    }
	

    if (stringName) free(stringName);
    if (orgName) free(orgName);
    if (onlyFontName) free(onlyFontName);

LEAVE:

    if (gD->doDebugging) {
        printf("Leaving GetDefaultFontName %s\n", 
		defaultFont->nameDefined ? defaultFont->newFamilyName : "<undefined>" );
    }


    return rval;
}


INT_type GetDefaultFontSize(gD, defaultFont)
p_odainfo_type	    gD;
p_FontAsSV_type	    defaultFont;{
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering GetDefaultFontSize (%d/%d)\n", defaultFont->theFont->dsnsize.first, defaultFont->theFont->dsnsize.second);
    }

    rval = SUCCESS_INT;

    /* See if a size is defined */
    if (defaultFont->theFont->dsnsize.second != 0) {
	/* We have a measurement in mms, convert it to points */
	/* pts = (mm) * (1 inch/25.4 mm) * (72 pts/ 1 inch) * (10/10) */
	/* pts = (mm) 720 / 254 = (mm) 360 / 127 */
	defaultFont->sizeDefined = BOOL_true;
	defaultFont->newFontSize = 
	  (defaultFont->theFont->dsnsize.first * 360) /
	  (defaultFont->theFont->dsnsize.second * 127);
    }
LEAVE:
    if (gD->doDebugging) {
        printf("Leaving GetDefaultFontSize with %d, %d\n", defaultFont->sizeDefined,  defaultFont->newFontSize);
    }


    return rval;
}


INT_type GetDefaultFontFace(gD, defaultFont)
p_odainfo_type	    gD;
p_FontAsSV_type	    defaultFont;{
    INT_type rval;


    if (gD->doDebugging) {
        printf("Entering GetDefaultFontFace\n");
    }


    rval = SUCCESS_INT;

    /* This is a quick and dirty attempt to the most useful stuff out */

    /* Check for escapements */
    if ((defaultFont->theFont->modes.specified == BOOL_true) &&
	 (defaultFont->theFont->modes.iso_0.specified == BOOL_true) &&
	 (defaultFont->theFont->modes.iso_0.esclass != ISO_ESCLASS_unspec)) {
	switch (defaultFont->theFont->modes.iso_0.esclass) {
		case ISO_ESCLASS_fixed:
		    defaultFont->facesDefined = BOOL_true;
		    defaultFont->newFaces |= fontdesc_Fixed;
		    break;
		case ISO_ESCLASS_variable:
		    /* Do nothing in SV, turn off in style */
		    break;
		default:
		    printf("Unknown value for escapement class %d\n", defaultFont->theFont->modes.iso_0.esclass);
		    break;
	}
    }


    /* Check for bold and italic */
    if(defaultFont->theFont->posture != ISO_POSTURE_unspec){
	defaultFont->facesDefined = BOOL_true;
	switch(defaultFont->theFont->posture) {
		case ISO_POSTURE_italic:
		case ISO_POSTURE_clockwise:
		    defaultFont->newFaces |= fontdesc_Italic;
		    break;
		case ISO_POSTURE_upright:
		case ISO_POSTURE_other:
		    /* here do nothing, in style, turn off something */
		    break;
		default:
		    printf("Unknown posture value %d\n", defaultFont->theFont->posture);
	}
    }

    if(defaultFont->theFont->weight != ISO_WEIGHT_unspec){
	defaultFont->facesDefined = BOOL_true;
	switch(defaultFont->theFont->weight) {
		case ISO_WEIGHT_ultra_light:
		case ISO_WEIGHT_extra_light:
		case ISO_WEIGHT_light:
		case ISO_WEIGHT_semi_light:
		case ISO_WEIGHT_medium:
		    /* here do nothing, in style, turn off something */
		    break;
		case ISO_WEIGHT_semi_bold:
		case ISO_WEIGHT_bold:
		case ISO_WEIGHT_extra_bold:
		case ISO_WEIGHT_ultra_bold:
		    defaultFont->newFaces |= fontdesc_Bold;
		    break;
		default:
		    printf("Unknown weight value %d\n", defaultFont->theFont->weight);
		    break;
	}
    }

    /* Check for shadow and outline */
    if(defaultFont->theFont->structur != ISO_STRUCTUR_unspec){
	defaultFont->facesDefined = BOOL_true;
	switch(defaultFont->theFont->structur) {
		case ISO_STRUCTUR_shadow:
		    defaultFont->newFaces |= fontdesc_Shadow;
		    break;
		case ISO_STRUCTUR_outline:
		    defaultFont->newFaces |= fontdesc_Outline;
		    break;
		case ISO_STRUCTUR_solid:
		case ISO_STRUCTUR_inline:
		case ISO_STRUCTUR_patterned:
		    /* here do nothing, on style, turn off everything */
		    break;
		default:
		    printf("Unknown structur value %d\n", defaultFont->theFont->structur);
		    break;
	}
    }

    /* All done for now */


LEAVE:

    if (gD->doDebugging) {
        printf("Leaving GetDefaultFontFace\n");
    }

    return rval;
}


INT_type GetDefaultFontFlag(gD, defaultFont)
p_odainfo_type	    gD;
p_FontAsSV_type	    defaultFont;{
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering GetDefaultFontFlag\n");
    }

    rval = SUCCESS_INT;



LEAVE:

    if (gD->doDebugging) {
        printf("Leaving GetDefaultFontFlag\n");
    }

    return rval;
}


INT_type GetDefaultScripts (gD, defaultFont)
p_odainfo_type	    gD;
p_FontAsSV_type	    defaultFont;{
    INT_type rval;

    if (gD->doDebugging) {
        printf("Entering GetDefaultScripts\n");
    }

    rval = SUCCESS_INT;

    /* Scripts are encoded using  varscripts */
    if ((defaultFont->theFont->modes.specified == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.specified == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.varscrpt.specified  == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.varscrpt.lscript.specified == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.varscrpt.lscript.vyoffset.second != 0)) {
	defaultFont->scriptsDefined = BOOL_true;
	defaultFont->newScript = (gD->globalSV.CurFontSize * (-defaultFont->theFont->modes.iso_0.varscrpt.lscript.vyoffset.first)) / defaultFont->theFont->modes.iso_0.varscrpt.lscript.vyoffset.second;

	/* We have a super script */
    }
    else if ((defaultFont->theFont->modes.specified == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.specified == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.varscrpt.specified  == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.varscrpt.rscript.specified == BOOL_true) &&
	(defaultFont->theFont->modes.iso_0.varscrpt.rscript.vyoffset.second != 0)) {
	/* We have a sub script */
	defaultFont->scriptsDefined = BOOL_true;
	defaultFont->newScript = (gD->globalSV.CurFontSize * (-defaultFont->theFont->modes.iso_0.varscrpt.rscript.vyoffset.first)) / defaultFont->theFont->modes.iso_0.varscrpt.rscript.vyoffset.second;
    }

LEAVE:

    if (gD->doDebugging) {
        printf("Entering GetDefaultScripts\n");
    }

    return rval;
}

FONT_REFERENCE_type * FindFontRef(gD, wantedFontID)
p_odainfo_type	    gD;
INT_type	    wantedFontID; {

    FONT_REFERENCE_type	    *rval;
    INT_type		    i;

    if (gD->doDebugging) {
        printf("Entering FindFontRef\n");
    }

    rval = (FONT_REFERENCE_type *) NULL;
    if (gD->fonts != odalib_NULL_SEQUENCE()){
        for(i=0;i<gD->fonts->length;i++){
	    if (gD->fonts->sequence_value.font_pairs[i].font_id == wantedFontID){
		rval = &gD->fonts->sequence_value.font_pairs[i].font_reference;
		break;
	    }
	}
    }

    if (rval == (FONT_REFERENCE_type *) NULL) {
	printf("Could not find font descriptor %d\n", wantedFontID);
	rval = (FONT_REFERENCE_type *) malloc(sizeof(FONT_REFERENCE_type));
	odalib_UnspecifyFont(rval);
    }

    if (gD->doDebugging) {
        printf("Leaving FindFontRef\n");
    }

    return rval;
}


p_FontAsSV_type MappedSVFontList = (p_FontAsSV_type) NULL;

p_FontAsSV_type FindSVforFont(gD, fontID)
p_odainfo_type	    gD;
INT_type	    fontID; {

    p_FontAsSV_type	rval;


    if (gD->doDebugging) {
        printf("Entering FindSVforFont\n");
    }

    /* First, check to see if we have already processed this font */
    for(rval=MappedSVFontList; rval != NULL; rval = rval->next) {
	if (rval->font_id == fontID) {
	    break;
	}
    }

    if (rval == (p_FontAsSV_type) NULL) {
	/* Not found, must create a new block */
	rval = (p_FontAsSV_type) malloc(sizeof(FontAsSV_type));
	if (rval == NULL) {
	    printf("Could not allocate space for a FontSV mapping block\n");
	}
	InitFontSVBlock(gD, rval, fontID);

	/* Decipher each possible piece */
	GetDefaultFontName(gD, rval);
	GetDefaultScripts (gD, rval);
	GetDefaultFontSize(gD, rval);
	GetDefaultFontFace(gD, rval);
	GetDefaultFontFlag(gD, rval);
	GetDefaultScripts (gD, rval);

    }

    if (gD->doDebugging) {
        printf("Leaving FindSVforFont\n");
    }

    return rval;
}


INT_type InitFontSVBlock(gD, tempBlk, newFontID)
p_odainfo_type	    gD;
p_FontAsSV_type	    tempBlk; 
INT_type	    newFontID;{

    INT_type	    rval;

    if (gD->doDebugging) {
        printf("Entering InitFontSVBlock\n");
    }

    rval = SUCCESS_INT;

    tempBlk->next = MappedSVFontList;
    MappedSVFontList = tempBlk;
    tempBlk->font_id = newFontID;
    tempBlk->theFont = FindFontRef(gD, newFontID);
    tempBlk->nameDefined = BOOL_false;
    tempBlk->sizeDefined = BOOL_false;
    tempBlk->facesDefined = BOOL_false;
    tempBlk->flagsDefined = BOOL_false;
    tempBlk->scriptsDefined = BOOL_false;
    tempBlk->newFamilyName = (char *) NULL;
    tempBlk->newFontSize = 0;
    tempBlk->newFaces = 0;
    tempBlk->newFlags = 0;
    tempBlk->newScript = 0;

    if (gD->doDebugging) {
        printf("Leaving InitFontSVBlock\n");
    }

    return rval;
}


INT_type GetFontNameAsString(gD, defaultFont, stringName, orgName, onlyFontName)
p_odainfo_type	    gD;
p_FontAsSV_type	    defaultFont;
char		    **stringName;
char		    **orgName;
char		    **onlyFontName; {

    INT_type			rval;
    FONT_REFERENCE_type		*tempFR;
    SEQUENCE_ISO9541_ID_type	tempName;

    if (gD->doDebugging) {
        printf("Entering GetFontNameAsString\n");
    }

    rval = SUCCESS_INT;
    tempFR = defaultFont->theFont;

    /* Define the first component as the organization name, the second as the font name, and the string name as everythign separated by slashes */

    *stringName = (char *) NULL;
    *orgName = (char *) NULL;
    *onlyFontName = (char *) NULL;

    switch(tempFR->family.structured_name_tag) {
	    case STRUCTURED_NAME_UNSPEC_tag:
		break;
	    case STRUCTURED_NAME_ID_tag:
		printf("IDs in structured names not supported\n");
		break;
	    case STRUCTURED_NAME_SEQ_tag:
		/* Pull	it out, piece by piece */
		tempName = tempFR->family.structured_name_value.idseq;
		/* Allocate space for org */
		CopyOneID(gD, tempName, /* first component */ (INT_type) 0, orgName);
		CopyOneID(gD, tempName, /* second component */ (INT_type) 1, onlyFontName);
		CopyAllIDs(gD, tempName, stringName);
		break;
	    default:
		printf("Unknown structured name tag %d\n", tempFR->family.structured_name_tag);
		break;
    }

    if (gD->doDebugging) {
        printf("Leaving GetFontNameAsString \"%s\"\n", *stringName ? *stringName : "<no font name>" );
    }

    return rval;

}

INT_type CopyOneID(gD, name, component, target)
p_odainfo_type gD;
SEQUENCE_ISO9541_ID_type    name;
INT_type		    component;
CHAR_type		    **target; {

    INT_type		rval;

    if (gD->doDebugging) {
        printf("Entering CopyOneID (component %d of name length %d)\n", component, name->length);
    }

    rval = SUCCESS_INT;

    /* See if it is in range */
    if ((component >= name->length) || (component < 0)){
	/* Cannot provide this component, there are not enough slots */
	*target	= (char	*) malloc(1);	/* provide a legal but empty string */
	**target = (CHAR_type) NULL;
    }
    else {
	switch(name->sequence_value.iso9541_ids[component].iso9541_id_tag){
		case ID_INT_tag:
		     /* make sure have enough room /3 is num of 8s, and three extra: one for rounding error, 1 for (illegal) minus sign and one for null */
		    *target = (char *) malloc((BITS_PER_INT / 3) + 3);
		    sprintf(*target, "%d", name->sequence_value.iso9541_ids[component].iso9541_id_value.integer);
		    break;
		case ID_STR_tag:
		    /* allocated enough space (with trailing null) */
		    *target = (char *) malloc(name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> length + 1);
		    /* coy over the bytes */
		    if (gD->doDebugging) {
			printf("Adding %d bytes for name %.*s\n",
			       name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> length,
			       name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> length,
			       name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> sequence_value.bytes); 
		    }
		    strncpy(*target, 
			    name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> sequence_value.bytes,
			    name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> length);
		    /* and terminate with a null */
		    *((*target) + (name-> sequence_value.iso9541_ids[component].iso9541_id_value.octet_string-> length)) = (CHAR_type) 0;
		    break;
		default:
		    printf("Unknown iso9541_id_tag type %d\n", name->sequence_value.iso9541_ids[component].iso9541_id_tag);
		    break;
	}
    }

    if (gD->doDebugging) {
        printf("Leaving CopyOneID \"%s\"\n", *target);
    }

    return rval;

}

INT_type CopyAllIDs(gD, name, target)
p_odainfo_type gD;
SEQUENCE_ISO9541_ID_type    name;
CHAR_type		    **target; {

    INT_type		    rval;
    CHAR_type		    *tempTarget;
    INT_type		    curTargetLen, oldTargetLen;	    /* Current number of chars */
    INT_type		    i; /* index into component list */

    if (gD->doDebugging) {
        printf("Entering CopyAllIDs\n");
    }

    rval = SUCCESS_INT;

    /* Collect them one by one and concatenat them together with slashes
      It's not nice to do so much copying, but at least I can get it coded quickly */
    /* Start off with an empty string */
    *target = (char *) malloc(1);
    **target = (CHAR_type) 0;
    curTargetLen = 0;
    for (i=0; i< name->length;i++) {
	/* Get the ith component */
	CopyOneID(gD, name, i, &tempTarget);
	/* Append it to list of already held stuff */
	oldTargetLen = curTargetLen;
	curTargetLen += strlen(tempTarget);
	*target = (char *) realloc(*target, curTargetLen + 1 /* for the null */);
	strcpy((*target)+oldTargetLen, tempTarget);
	/* and toss intermediate result */
	free(tempTarget);
	/* See if a separating "/" is needed */
	if (i != (name->length-1)) {
	    /* More coming, so add a slash (overwritng the old null) */
	    curTargetLen++;
	    *target = (char *) realloc(*target, curTargetLen+1 /* for the null */);
	    *(*target + curTargetLen - 1) = (CHAR_type) '/';
	    *(*target + curTargetLen) = (CHAR_type) 0;
	}
    }

    if (gD->doDebugging) {
        printf("Leaving CopyAllIDs \"%s\"\n", *target);
    }
    
    return rval;

}
