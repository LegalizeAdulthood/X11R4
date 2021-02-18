/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
#include <ext.h>
/*
 * Walk around the body of the Interleaf object
 *
 * Create the first basic Logic Object non-concatenated, to hold the
 *	Chacarfer Fonts and the default font, and Character rendition
 *	For each font we see in the input, create a concatenated object
 *	with proper character rendition
 * 
 */
CharContent(GenPar, SpecPar, infoP, classP)
	CONSTITUENT GenPar, SpecPar;	/* The Generic & Specific Parents */
	Cmpn_t *infoP, *classP;
{
	shar *sP, *tP;
	long i, count = 0;
	INT_type first = 0;
	shar c;
	Token_t *tokenP;

	CONSTITUENT FBLO, tmpBLO;
	extern CONSTITUENT nonBLO(), conBLO();

	if((i = SsAmount(&infoP->body)) <= 0L) {
		warn("Empty Interleaf component");
		return;
	}
	/*
	 *  devide stuff into BLO's, and separate content portions
	 */
	tP = sP = infoP->body.outP;

	/*
	 * create the nonconcatenated BLO
	 */
	FBLO = nonBLO(GenPar, &classP->font_value);
	while(i > 0) {
		if((c = *sP) == '\0') {
			break;	/* end of fun */
		} else {
			if(IS_TOKEN(c)) {
				tokenP = (Token_t *)sP;
				switch(TOKEN_RESET(tokenP->token)) {
					/*
					 * We are only interested in a small font token
					 */
				case tok_Font:	/** FN **/
					if(first == 0) {
						attCP(FBLO, tP, count);
						first++;
					} else {
						attCP(tmpBLO, tP, count);
						AppendSubordinate(SpecPar, tmpBLO);
					}
					if(tokenP->u.fontvalP->font_token == 0) {
						tmpBLO = conBLO(GenPar, &classP->font_value);
					} else {	
						tmpBLO = conBLO(GenPar, tokenP->u.fontvalP);
					}
					tP = sP += sizeof(Token_t)/sizeof(shar);
					i -= sizeof(Token_t)/sizeof(shar);
					count = sizeof(Token_t)/sizeof(shar);
					break;
				default:
					sP += sizeof(Token_t)/sizeof(shar);
					i -= sizeof(Token_t)/sizeof(shar);
					count += sizeof(Token_t)/sizeof(shar);
				}
			} else {
				sP++; i++; count++;
			}
		}
	}
	if( first == 0) {	/* there were no font changes */
		attCP(FBLO, tP, count);
		first++;
	} else {
		attCP(tmpBLO, tP, count);
		AppendSubordinate(SpecPar, tmpBLO);
	}

	/*
	 * connect these pieces, here is the place to attach character fonts
	 */
	AppendSubordinate(SpecPar, FBLO);
	/*
	 * attach content information
	 * quite inefficient, since the string is copied again, but
	 * we worry about it an other time
	if(AppendCharContent(BLO, CP, p) == ERROR_INT) {
		fatal("Couldn't append char content");
	}
 get rid of copy
	free(p);
*/
}

CONSTITUENT
BLO(GenPar, fontvalP, concat)
	CONSTITUENT GenPar;
	Font_value_t *fontvalP;
	at_CONCATENATION_type concat;
{
	CONSTITUENT rval, GenTextBLO, LayStyle;

	char tmp[BUFSIZ];

	rval = MakeComponent(Doc, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);
	AddStringAttribute(rval, at_APPLICATION_COMMENTS, "Text", DoAsBytes);
	ArSprintf(tmp, "Specific Text for font F<%d>", fontvalP->font_token);
	AddStringAttribute(rval, at_USER_READABLE_COMMENTS, tmp, DoAsString);

	if( concat == at_CONCATENATION_concatenated) {
		LayStyle = SafeMakeStyle(Doc, LAYOUT_STYLE);
		ArSprintf(tmp, "Layout style for F<%d>", fontvalP->font_token);
		AddStringAttribute(LayStyle, at_USER_READABLE_COMMENTS, tmp, DoAsString);
		SafeSetValueAttr(LayStyle, at_CONCATENATION, concat);
		SafeSetAttr(rval, at_LAYOUT_STYLE, (POINTER_type)LayStyle, ALL_PARMS);
	}


	GenTextBLO = MakeComponent(Doc, at_OBJECT_TYPE_bas_logical_obj,
						GENERIC_COMPONENT);
	AddStringAttribute(GenTextBLO, at_APPLICATION_COMMENTS, "Text",
									DoAsBytes);
	ArSprintf(tmp, "Generic Paragraph for font F<%d>", fontvalP->font_token);
	AddStringAttribute(GenTextBLO, at_USER_READABLE_COMMENTS, tmp, DoAsString);
	AppendGenForSub(GenPar, GenTextBLO);

	/*
	 * connect specific text to generic
	 */
	SafeSetAttr(rval, at_OBJECT_CLASS, GenTextBLO, ALL_PARMS);
	return rval;
}

/*
 * makes the generic and specific constituent,
 * returns the specific, sets non-concatenation
 */
CONSTITUENT
nonBLO(GenPar, fontvalP)
	CONSTITUENT GenPar;
	Font_value_t *fontvalP;
{
	CONSTITUENT rval;

	/*
	 * this would be the place to initialize the Character fontlist
	 * to nothing, set the default font, Also set the SGR
	 */

	/*
	 * make specific & generic
	 */
	rval = BLO(GenPar, fontvalP, at_CONCATENATION_non_concated);

	return rval;
}
CONSTITUENT
conBLO(GenPar, fontvalP)
	CONSTITUENT GenPar;
	Font_value_t *fontvalP;
{
	CONSTITUENT rval;

	/*
	 * Here on wants to set the SGR
	 */

	/*
	 * make specific & generic
	 */
	rval = BLO(GenPar, fontvalP, at_CONCATENATION_concatenated);

	return rval;
}

/*
 * attach contents to BLO
 */
attCP(BLO, sP, amount)
	CONSTITUENT BLO;
	shar *sP;
	long amount;
{
	CONSTITUENT CP;

	char *p, *q;
	shar c;
	long i = amount;

	if(amount == 0) {	/* nothing to do, create empty contents */
		char dummy = '\0';
		if((CP = MakeContent(Doc, CHARACTER_CONTENT)) == ERROR_CONSTITUENT) {
			fatal("Couldn't make character content (%s)", ErrorMsg(TKError));
		}

		/*
		 * attach ``content'' information
		 */
		if(AppendCharContent(BLO, CP, &dummy) == ERROR_INT) {
				fatal("Couldn't append char content");
		}
		return;
	}

	/* at least amount of bytes */
	if((p = q = (char *)malloc(amount)) == NULL) {
		fatal("Not enough memory for content");
	}
	/*
	 * copy the stuff
	 */
	while(i > 0) {
		if((c = *sP) == '\0') {
			break;	/* end of fun */
		} else {
			if(IS_TOKEN(c)) {	/*strip out */
				sP += sizeof(Token_t)/sizeof(shar);
				i -= sizeof(Token_t)/sizeof(shar);
			} else {
				*q++ = c;
				sP++; i--;
			}
		}
	}
	*q = '\0';	/* terminate */

	if((CP = MakeContent(Doc, CHARACTER_CONTENT)) == ERROR_CONSTITUENT) {
		fatal("Couldn't make character content (%s)", ErrorMsg(TKError));
	}
	
	/*
	 * attach content information
	 * quite inefficient, since the string is copied again, but
	 * we worry about it an other time
	 */
	if(AppendCharContent(BLO, CP, p) == ERROR_INT) {
		fatal("Couldn't append char content");
	}
	/* get rid of copy */
	free(p);
}
