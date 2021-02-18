/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
#include <ext.h>

/*
 * where we start to work
 */

CONSTITUENT GenLogRoot;
CONSTITUENT SpecLogRoot;
CONSTITUENT GenPara;

Rsu pageleft, pageright;

/*
 * list to fold generic logical constituents
 */

typedef struct imap_t {
	shar name[AR_MAX_CMPNNAME];	/* the interleaf name */
	CONSTITUENT GenConst;		/* Constituent to connect to */
	struct imap_t *nextP;
} Imap_t;

Imap_t *ImapbaseP;

Imap_t *
imapalloc() {
	Imap_t *aP;

	aP = (Imap_t *) calloc (1, sizeof(Imap_t));
	if ( aP == NULL )
		fatal("imapalloc: out of memory");
	else
		return aP;
}

doit()
{
	CreateEmptyODADocStyle(Doc);
	mklogic();
}

/*
 * make the logical structure
 */

mklogic()
{
	Cmpn_t *bP = (Cmpn_t *) my_il_state.bodyP;

	fakepage(&my_il_state.page_info);


	/* first make the logical root */
	GenLogRoot = MakeComponent(Doc, at_OBJECT_TYPE_doc_logical_root,
							GENERIC_COMPONENT);
	if(GenLogRoot == ERROR_CONSTITUENT)
		fatal("Couldn't make Generic Logical Root %s", ErrorMsg(TKError));
	AddStringAttribute(GenLogRoot, at_APPLICATION_COMMENTS, "LogDoc", DoAsBytes);
	

	/* and now for generic */
	SpecLogRoot = MakeComponent(Doc, at_OBJECT_TYPE_doc_logical_root,
							SPECIFIC_COMPONENT);
	if(SpecLogRoot == ERROR_CONSTITUENT)
		fatal("Couldn't make Generic Logical Root %s", ErrorMsg(TKError));
	AddStringAttribute(SpecLogRoot, at_APPLICATION_COMMENTS, "LogDoc", DoAsBytes);
	

	/* Connect them */
	SafeSetAttr(SpecLogRoot, at_OBJECT_CLASS, GenLogRoot, ALL_PARMS);

	/*
	 * more generic stuff
	 */
	GenPara = MakeComponent(Doc, at_OBJECT_TYPE_comp_logical_obj,
							GENERIC_COMPONENT);
	AddStringAttribute(GenPara, at_APPLICATION_COMMENTS, "Paragraph",
									DoAsBytes);
	AddStringAttribute(GenPara, at_USER_READABLE_COMMENTS, "NBBS Paragraph",
									DoAsString);
/*
	GenTextBLO = MakeComponent(Doc, at_OBJECT_TYPE_bas_logical_obj,
						GENERIC_COMPONENT);
	AddStringAttribute(GenTextBLO, at_APPLICATION_COMMENTS, "Text",
									DoAsBytes);
	AddStringAttribute(GenTextBLO, at_USER_READABLE_COMMENTS,
						"Generic Paragraph", DoAsString);

	 * connect these pieces

	AppendGenForSub(GenPara, GenTextBLO);
not anymore, we need to deal with phrases now
*/

	/*
	 * now walk the document
	 *
	 * we are very optimistic here, we only expect simple components in
	 * the Interleaf for now. Not things like bullets, lists etc.
	 * let's walk the list
	 */
	while(bP != NULL) {
		il_oda(bP);
		bP =  (Cmpn_t *) (bP->nextP);
	}
}

/*
 * map interleaf objects into ODA objects
 * and build into logical structure
 */

il_oda(oP)
	Cmpn_t *oP;
{
	Cmpn_t *cP = my_il_state.cmpnP;
	Imap_t *iP = ImapbaseP;
	Imap_t *saveiP;

	CONSTITUENT GenPass, SpecPass, Gentmp, Spectmp;

	char tmp[BUFSIZ];

	/*
	 * first find the class definition in Interleaf
	 */
	while (cP != NULL) {
		if( ArShrcmp(oP->cmpn_name, cP->cmpn_name) == 0)
			break;
		cP =  (Cmpn_t *) (cP->nextP);
	}
	if (cP == NULL) {	/* in line stuff, ignore for now */
		ArSprintf(tmp, "%S", oP->cmpn_name);
		warn("Cannot find Class for %s", tmp);
		return;
	}
	while( iP != NULL) {
		if( ArShrcmp(oP->cmpn_name, iP->name ) == 0 ) {	/* found it */
			GenPass = iP->GenConst;
			break;
		}
		saveiP = iP;
		iP = iP->nextP;
	}
	if ( iP == NULL) {	/* new object, create a new logical passage */
		iP = imapalloc();
		if (ImapbaseP == NULL)
			ImapbaseP = iP;
		else
			saveiP->nextP = iP;
		ArShrcpy(iP->name, cP->cmpn_name);
		GenPass = MakeComponent(Doc, at_OBJECT_TYPE_comp_logical_obj,
							GENERIC_COMPONENT);
		iP->GenConst = GenPass;
		AddStringAttribute(GenPass,at_APPLICATION_COMMENTS, "Passage", DoAsBytes);
		ArSprintf(tmp, "Generic Passage %#S", iP->name);
		AddStringAttribute(GenPass, at_USER_READABLE_COMMENTS,
				tmp , DoAsString);
		/*
		 * connect to root
		 */
		AppendGenForSub(GenLogRoot, GenPass);
		/*
		 * Connect to generic paragraph and text
		 */
		AppendGenForSub(GenPass, GenPara);
		/*
		 * add style to this beast, use the Class
		 */
		do_style(GenPass, cP, (Cmpn_t *)NULL);
	}

	/*
	 * Now create specific structure (Passage) for this beast
	 */
	SpecPass = MakeComponent(Doc, at_OBJECT_TYPE_comp_logical_obj, SPECIFIC_COMPONENT);
	AddStringAttribute(SpecPass, at_APPLICATION_COMMENTS, "Passage", DoAsBytes);
	ArSprintf(tmp, "Specific Passage w/o styles %#S", iP->name);
	AddStringAttribute(SpecPass, at_USER_READABLE_COMMENTS, tmp, DoAsString);
	AppendSubordinate(SpecLogRoot, SpecPass);
	/*
	 * connect
	 */
	SafeSetAttr(SpecPass, at_OBJECT_CLASS, GenPass, ALL_PARMS);

	/*
	 * now do some real work
	 */
	 if(samecmpn(oP, cP) != BOOL_true) {
		/*
		 * different then original definition, insert the delta,
		 *
		 * first the generic
		 */
		Gentmp = MakeComponent(Doc, at_OBJECT_TYPE_comp_logical_obj,
						GENERIC_COMPONENT);
		AddStringAttribute(Gentmp, at_APPLICATION_COMMENTS, "Passage", DoAsBytes);
		ArSprintf(tmp, "Generic Passage w/o styles %#S delta", iP->name);
		AddStringAttribute(Gentmp, at_USER_READABLE_COMMENTS, tmp, DoAsString);
		/*
		 * connect to previous passage
		 */
		AppendGenForSub(GenPass, Gentmp);
		/*
		 * connect generic paragrpah to it
		 */
		AppendGenForSub(Gentmp, GenPara);
		/*
		 * Now specific
		 */
		Spectmp = MakeComponent(Doc, at_OBJECT_TYPE_comp_logical_obj,
						SPECIFIC_COMPONENT);
		AddStringAttribute(Spectmp, at_APPLICATION_COMMENTS, "Passage", DoAsBytes);
		ArSprintf(tmp, "Specific Passage w/o styles %#S delta", iP->name);
		AddStringAttribute(Spectmp, at_USER_READABLE_COMMENTS, tmp, DoAsString);
		AppendSubordinate(SpecPass, Spectmp);
		/*
		 * connect
		 */
		SafeSetAttr(Spectmp, at_OBJECT_CLASS, Gentmp, ALL_PARMS);
		/*
		 * to be used later
		 */
		GenPass = Gentmp;
		SpecPass = Spectmp;
		do_style(GenPass,oP, cP);
	}
	/*
	 * paragraph
	 */
	Spectmp = MakeComponent(Doc, at_OBJECT_TYPE_comp_logical_obj, SPECIFIC_COMPONENT);
	AddStringAttribute(Spectmp, at_APPLICATION_COMMENTS, "Paragraph",
									DoAsBytes);
	ArSprintf(tmp, "Specific Paragraph %#S", iP->name);
	AddStringAttribute(Spectmp, at_USER_READABLE_COMMENTS, tmp, DoAsString);
	AppendSubordinate(SpecPass, Spectmp);
	/*
	 * connect paragraph to generic
	 */
	SafeSetAttr(Spectmp, at_OBJECT_CLASS, GenPara, ALL_PARMS);
	/*
	 * Text

	SpecPass = Spectmp;
	Spectmp = MakeComponent(Doc, at_OBJECT_TYPE_bas_logical_obj, SPECIFIC_COMPONENT);
	AddStringAttribute(Spectmp, at_APPLICATION_COMMENTS, "Text", DoAsBytes);
	ArSprintf(tmp, "Specific Text %#S", iP->name);
	AddStringAttribute(Spectmp, at_USER_READABLE_COMMENTS, tmp, DoAsString);
	AppendSubordinate(SpecPass, Spectmp);
not anymore, we need to deal with phrases now
*/
	/*
	 * connect specific text to generic
	SafeSetAttr(Spectmp, at_OBJECT_CLASS, GenTextBLO, ALL_PARMS);
not anymore, we need to deal with phrases now
*/
	/*
	 * deal with content
	 */
	CharContent(GenPara,  SpecPass, oP, cP);

}

/*
 * fake the page layout of Interleaf.
 * This should actually be a layout style on the page page frame or something like that,
 * but the other translators don't bother with pages at all.
 * Heck, we they even don't look at the page size.
 * So in the good old CMU/ITC tradition we do a half harted job: We cheat! Down with
 * proper scientific methods, we are only in it for the money and to get egos ``as big
 * as the state Alaska''.
 *
 * Find the margins of the page object and add it to any margin we set
 * This also implies that for going into Interleaf we have to set the page margins
 * to zero.
 *
 */
fakepage( pP )
	Page_info_t *pP;
{
	pageleft = pP->left_margin;
	pageright = pP->right_margin;
}
