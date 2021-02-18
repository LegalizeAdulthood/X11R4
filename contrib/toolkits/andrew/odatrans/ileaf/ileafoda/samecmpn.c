/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
#include <ext.h>

/*
 * compare two interleaf components and return true or false
 *
 * (based on dumpCmpnProps)
 */

BOOL_type
AttrValscmpnn( avP, bvP)
Attr_t *avP, *bvP;
{
	for( ; avP != NULL; avP = avP->nextP, bvP = bvP->nextP) {
		if( ArShrcmp(avP->nameP, bvP->nameP) != 0)
			return BOOL_false;
		if( ArShrcmp(avP->stringP, bvP->stringP) != 0)
			return BOOL_false;
	}
		return BOOL_true;
}

BOOL_type
samecmpn( cmP, masterP)
Cmpn_t *cmP, *masterP;
{
	/* this might become usefull for undefined master class
	commented out for now
    if (masterP == NULL) {
	masterP = &output_cmpn;
    }
	*/
    if (cmP->attrvalsP || masterP->attrvalsP) {
	return(AttrValscmpnn( cmP->attrvalsP, masterP->attrvalsP));
    }
	/* ID_Cpmn deals with subcomponents, we don't handle these yet */
    if (cmP->id != ID_Cmpn && cmP->content_type != masterP->content_type) {
	return BOOL_false;
    }
    if (cmP->hidden != masterP->hidden) {
	return BOOL_false;
    }
    if (cmP->subcmpn != masterP->subcmpn) {
	return BOOL_false;
    }
    if (ArFontValueEqual(&cmP->font_value, &masterP->font_value) == NO) {
	return BOOL_false;
    }
    if (cmP->top_margin != masterP->top_margin) {
	return BOOL_false;
    }
    if (cmP->bottom_margin != masterP->bottom_margin) {
	return BOOL_false;
    }
    if (cmP->left_margin != masterP->left_margin) {
	return BOOL_false;
    }
    if (cmP->right_margin != masterP->right_margin) {
	return BOOL_false;
    }
    if (cmP->first_indent != masterP->first_indent) {
	return BOOL_false;
    }
    if (cmP->line_spacing != masterP->line_spacing) {
	return BOOL_false;
    }
    if (cmP->alignment != masterP->alignment) {
	return BOOL_false;
    }
    if (cmP->att_prior != masterP->att_prior) {
	return BOOL_false;
    }
    if (cmP->pbreak_within != masterP->pbreak_within) {
	return BOOL_false;
    }
    if (cmP->att_next != masterP->att_next) {
	return BOOL_false;
    }
    if (cmP->fill != masterP->fill) {
	return BOOL_false;
    }
    if (cmP->new_page != masterP->new_page) {
	return BOOL_false;
    }
    if (cmP->new_column != masterP->new_column) {
	return BOOL_false;
    }
    if (cmP->widow_ctrl != masterP->widow_ctrl) {
	return BOOL_false;
    }
    if (cmP->orphan_ctrl != masterP->orphan_ctrl) {
	return BOOL_false;
    }
    if (cmP->straddle != masterP->straddle) {
	return BOOL_false;
    }
    if (cmP->kern_space != masterP->kern_space) {
	return BOOL_false;
    }
    if (cmP->word_spacing != masterP->word_spacing) {
	return BOOL_false;
    }
    if (cmP->letter_spacing != masterP->letter_spacing) {
	return BOOL_false;
    }
    if (cmP->composition != masterP->composition) {
	return BOOL_false;
    }
    if (cmP->hyphenation != masterP->hyphenation) {
	return BOOL_false;
    }
    if (ArShrcmp(cmP->toc_name, masterP->toc_name) != 0) {
	return BOOL_false;
    }
    /** check for tab differences, emit all tabs if different. **/
    if (ArTabsEqual(&cmP->tabs, &masterP->tabs) == NO) {
	return BOOL_false;
    }
	return BOOL_true;
}
