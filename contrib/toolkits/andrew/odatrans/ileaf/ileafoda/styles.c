/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/

/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/styles.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/styles.c,v 1.3 89/09/08 17:21:20 mss Exp $";
#endif

/*
  $Log:	styles.c,v $
 * Revision 1.3  89/09/08  17:21:20  mss
 * /tmp/msg
 * 
 * Revision 1.2  89/09/08  14:57:41  mss
 * Calculate left margin
 * 
 * Revision 1.1  89/09/05  21:58:35  mss
 * Initial revision
 * 
 * Revision 1.4  89/07/14  12:56:38  jaap
 * deal with the page margin as by order from mss
 * 
 * Revision 1.3  89/07/12  12:40:26  jaap
 * margins, and justification in place (sort of)
 * 
 * Revision 1.2  89/07/10  12:21:17  jaap
 * First shot at doing styles
 * 
 * Revision 1.1  89/07/06  14:28:56  jaap
 * Initial revision
 * 
*/

/*
 * As usual shamelessly stolen from mss
 */

#include <ext.h>

static INT_type
CreatePagePiece(newConstituent, whichdoc, objectType, NBSString, commentStr)
	CONSTITUENT *newConstituent;
	DOCUMENT_type whichdoc;
	char *NBSString;
	char *commentStr;
{


	*newConstituent = MakeComponent(whichdoc, objectType, GENERIC_COMPONENT);
	if(ERROR_CONSTITUENT == *newConstituent) {
		fatal("Could not create new object (%d) for stylesheet (%s)\n", objectType,
							ErrorMsg(TKError));
		return ERROR_INT;
	}
	AddStringAttribute(*newConstituent, at_APPLICATION_COMMENTS, NBSString, DoAsBytes);
	AddStringAttribute(*newConstituent, at_USER_READABLE_COMMENTS, commentStr,
									DoAsString);
}

INT_type CreateEmptyODADocStyle(whichdoc)
DOCUMENT_type whichdoc;
{
	/* This procedure initializes the pageset structures for use by the style sheets */

	CONSTITUENT a_const, b_const;
	CONSTITUENT LayDoc, Body, Header, Footer, Page, PageSet;

	at_GEN_FOR_SUBORDINATES_type TopNode;

	LayDoc = MakeComponent(whichdoc, at_OBJECT_TYPE_doc_layout_root,
									GENERIC_COMPONENT);
	if (LayDoc == ERROR_CONSTITUENT) {
		fatal("Could not make generic layout document root (%s)",
							ErrorMsg(TKError));
	}

	PageSet = MakeComponent(whichdoc, at_OBJECT_TYPE_page_set, GENERIC_COMPONENT);
	if (PageSet == ERROR_CONSTITUENT) {
		fatal("Could not make generic pageset %s\n", ErrorMsg(TKError));
	}

	/* LayDoc */
	AddStringAttribute(LayDoc, at_APPLICATION_COMMENTS, "Laydoc", DoAsBytes);
	AddStringAttribute(LayDoc, at_USER_READABLE_COMMENTS, "Generic Layout Root",
									DoAsString);

	/* PageSet */
	AddStringAttribute(PageSet, at_APPLICATION_COMMENTS, "PageSet", DoAsBytes);
	AddStringAttribute(PageSet, at_USER_READABLE_COMMENTS, "Generic PageSet",
								DoAsString);
	/* Connect Pageset to root */
	TopNode.construction_tag = TERM_tag;
	TopNode.construction_value.term.term_tag = REQ_tag;
	TopNode.construction_value.term.factor.factor_tag = OBJECT_CLASS_ID_tag;
	TopNode.construction_value.term.factor.factor_value.object_class_id = PageSet;
	SafeSetAttr(LayDoc, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

	/* create header */
	if( CreatePagePiece( &Header, whichdoc, at_OBJECT_TYPE_frame, "Header",
			"Generic header frame for stylesheet") == ERROR_INT) {
		fatal("Couldn't create header frame for stylesheet");
	}
	/* and  header frameH*/
	if( CreatePagePiece( &a_const, whichdoc, at_OBJECT_TYPE_frame, "FrameH",
			"Generic frame for sourcing headers for stylesheet") == ERROR_INT){
		fatal("Couldn't create frameH for sourcing for stylesheet");
	}
	/* Connect FrameH to Header */
	TopNode.construction_tag = TERM_tag;
	TopNode.construction_value.term.term_tag = REQ_tag;
	TopNode.construction_value.term.factor.factor_tag = OBJECT_CLASS_ID_tag;
	TopNode.construction_value.term.factor.factor_value.object_class_id = a_const;
	SafeSetAttr(Header, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

	/* create Footer */
	if( CreatePagePiece( &Footer, whichdoc, at_OBJECT_TYPE_frame, "Footer",
			"Generic footer frame for stylesheet") == ERROR_INT) {
		fatal("Couldn't create header frame for stylesheet");
	}
	/* and  footer frameH*/
	if( CreatePagePiece( &a_const, whichdoc, at_OBJECT_TYPE_frame, "FrameH",
			"Generic frame for sourcing footer for stylesheet") == ERROR_INT){
		fatal("Couldn't create frameH for sourcing for stylesheet");
	}
	/* Connect FrameH to Footer */
	TopNode.construction_tag = TERM_tag;
	TopNode.construction_value.term.term_tag = REQ_tag;
	TopNode.construction_value.term.factor.factor_tag = OBJECT_CLASS_ID_tag;
	TopNode.construction_value.term.factor.factor_value.object_class_id = a_const;
	SafeSetAttr(Footer, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode, ALL_PARMS);

	/* create body */
	if( CreatePagePiece( &Body, whichdoc, at_OBJECT_TYPE_frame, "BodyArea",
			"Generic frame for bodyarea for stylesheet") == ERROR_INT) {
		fatal("Couldn't create header frame for stylesheet");
	}
	/* and  body frameC*/
	if( CreatePagePiece( &a_const, whichdoc, at_OBJECT_TYPE_frame, "FrameC",
			"Generic frame for body text area for stylesheet") == ERROR_INT){
		fatal("Couldn't create frameH for sourcing for stylesheet");
	}
	/* and  body frameF*/
	if( CreatePagePiece( &b_const, whichdoc, at_OBJECT_TYPE_frame, "FrameF",
			"Generic frame for foot note body area for stylesheet")
								== ERROR_INT){
		fatal("Couldn't create frameH for sourcing for stylesheet");
	}
	/* Connect FrameC & FrameF to body */
	TopNode.construction_tag = SEQUENCE_tag;
	TopNode.construction_value.construction =
				MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag, 2);
	if (TopNode.construction_value.construction == ERROR_SEQUENCE) {
		fatal("Couldn't allocate sequence term for page (%s)", ErrorMsg(TKError));
	}
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].term_tag = REQ_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].factor.factor_tag
				= OBJECT_CLASS_ID_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].factor.factor_value.object_class_id
				= a_const;

	TopNode.construction_value.construction->
		sequence_value.construction_terms[1].term_tag = REQ_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[1].factor.factor_tag
				= OBJECT_CLASS_ID_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[1].factor.factor_value.object_class_id
				= b_const;
	SafeSetAttr(Body, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode,
				ALL_PARMS);

	/* create a page */
	if( CreatePagePiece( &Page, whichdoc, at_OBJECT_TYPE_page, "Page",
			"Generic page for stylesheet") == ERROR_INT) {
		fatal("Couldn't create Generic Page for Stylesheet");
	}

	/* attach to page set */
	if(ContractSequence(TopNode.construction_value.construction, 1) == ERROR_INT) {
		fatal("Couldn't contract gen for subordinates (%s)", ErrorMsg(TKError));
	}
	TopNode.construction_tag = SEQUENCE_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].term_tag = OPT_REP_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].factor.factor_tag
				= OBJECT_CLASS_ID_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].factor.factor_value.object_class_id
				= Page;
	SafeSetAttr(PageSet, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode,
				ALL_PARMS);

	/* attach header, body & footer to page */
	TopNode.construction_tag = SEQUENCE_tag;
	if(ExpandSequence( TopNode.construction_value.construction, 2) == ERROR_INT) {
		fatal("Couldn't allocate sequence term for page (%s)", ErrorMsg(TKError));
	}
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].term_tag = REQ_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].factor.factor_tag
				= OBJECT_CLASS_ID_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[0].factor.factor_value.object_class_id
				= Header;

	TopNode.construction_value.construction->
		sequence_value.construction_terms[1].term_tag = REQ_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[1].factor.factor_tag
				= OBJECT_CLASS_ID_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[1].factor.factor_value.object_class_id
				= Footer;

	TopNode.construction_value.construction->
		sequence_value.construction_terms[2].term_tag = REQ_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[2].factor.factor_tag
				= OBJECT_CLASS_ID_tag;
	TopNode.construction_value.construction->
		sequence_value.construction_terms[2].factor.factor_value.object_class_id
				= Body;

	SafeSetAttr(Page, at_GEN_FOR_SUBORDINATES, (POINTER_type) &TopNode,
				ALL_PARMS);

	if(DeleteSequence(TopNode.construction_value.construction) == ERROR_INT) {
		fatal("Couldn't delete seuqnece for building page (%s)", ErrorMsg(TKError));
	}

	return SUCCESS_INT;
}

/*
 * make a style for a generic logical object (Passage)
 */

do_style(pas, comP, classP)
	CONSTITUENT pas;
	Cmpn_t *comP, *classP;
{
	extern DOCUMENT_type Doc;

	char tmp[BUFSIZ];
	char sname[AR_MAX_CMPNNAME];

	CONSTITUENT laystyle;
	CONSTITUENT presstyle;
	at_DEFAULT_VALUE_LISTS_type stylelist;
	CONSTITUENT fakeBLO;

	ArSprintf(sname, "%S", comP->cmpn_name);

	/* layout */
	laystyle = SafeMakeStyle(Doc, LAYOUT_STYLE);
	ArSprintf(tmp, "DVL Layout Style for stylesheet %s", sname);
	AddStringAttribute(laystyle, at_USER_READABLE_COMMENTS, tmp, DoAsString);
	AddStringAttribute(laystyle, at_USER_READABLE_COMMENTS, sname, DoAsString);

	/* presentation */
	presstyle = SafeMakeStyle(Doc, PRESENTATION_STYLE);
	ArSprintf(tmp, "DVL Presentation Style for stylesheet %s", sname);
	AddStringAttribute(presstyle, at_USER_READABLE_COMMENTS, tmp, DoAsString);
	AddStringAttribute(presstyle, at_USER_READABLE_COMMENTS, sname, DoAsString);

	/* fakeBLO */
	if(InitializeDefaultValueLists(Doc, &stylelist,
			BAS_LOGICAL_ATTRIBUTES_parm) == ERROR_INT) {
		fatal("Couldn't create fake BLO to hold styles for stylesheet %s (%s)",
				sname, ErrorMsg(TKError));
	}
	fakeBLO = stylelist.basic_logical_attributes;
	/* attach styles to BLO */
	SafeSetAttr(fakeBLO, at_PRESENTATION_STYLE, (POINTER_type)presstyle, ALL_PARMS);
	SafeSetAttr(fakeBLO, at_LAYOUT_STYLE, (POINTER_type)laystyle, ALL_PARMS);

	/* attach content marker*/
	SafeSetAttr(fakeBLO,at_CONTENT_ARCH_CLASS,
				(POINTER_type) at_CONTENT_ARCH_CLASS_proc_cc, ALL_PARMS);
	SafeSetAttr(pas, at_DEFAULT_VALUE_LISTS, (POINTER_type) &stylelist, ALL_PARMS);

	/* fill in styles */
	CalcStyles(presstyle, laystyle, comP, classP);
}

/*
 * make the style attributes from a interleaf object
 *
 * If it is a delta, check the change if there, and then apply
 */
CalcStyles( press, lays, objectP, ClassP)
	Cmpn_t *objectP, *ClassP;
	CONSTITUENT press, lays;
{

	if(ClassP == (Cmpn_t *) NULL ) {	/* genuine new thing */
		CalcJust(press, objectP->alignment);
		CalcRM(lays, objectP->right_margin);
		CalcLM(lays, press, objectP);
	} else {		/* we are a delta */
		if(objectP->alignment != ClassP->alignment)
			CalcJust(press, objectP->alignment);
		if(objectP->right_margin != ClassP->right_margin)
			CalcRM(lays, objectP->right_margin);
		if(objectP->left_margin != ClassP->left_margin
		   || objectP->first_indent != ClassP->first_indent)
			CalcDeltaLM( lays, press, objectP, ClassP);
	}
}

/*
 * Do justification
 */
CalcJust(ls, align)
	CONSTITUENT ls;
	Direction_t align;
{
	switch(align){
		case d_LEFT:
			SafeSetValueAttr(ls, cc_ALIGNMENT, cc_ALIGNMENT_start_aligned);
			break;
		case d_RIGHT:
			SafeSetValueAttr(ls, cc_ALIGNMENT, cc_ALIGNMENT_end_aligned);
			break;
		case d_CENTER:
			SafeSetValueAttr(ls, cc_ALIGNMENT, cc_ALIGNMENT_centred);
			break;
		case d_BOTH:	/* FALLTROUGH */
		default:
			SafeSetValueAttr(ls, cc_ALIGNMENT, cc_ALIGNMENT_justified);
	}
}

/*
 * Get current offset
 * As usual, shamelessly stolen from mss
 */
at_OFFSET_type
*GetCurOffset(LayoutStyle,returnMask)
	CONSTITUENT LayoutStyle;
	PARM_MASK_type *returnMask;
{
	at_OFFSET_type *rval;

	if (HAS_ATTR == HasAttr(LayoutStyle, at_OFFSET)) {
		GetAttr(LayoutStyle, at_OFFSET, (POINTER_type)&rval, NOINHERIT, returnMask);
		if (*returnMask == ALL_PARMS) {
			*returnMask = (PARM_MASK_type) (LEADING_OFFSET_parm
						| TRAILING_OFFSET_parm
						| LEFT_HAND_OFFSET_parm
						| RIGHT_HAND_OFFSET_parm);
		}
	} else {
		rval = (at_OFFSET_type *) malloc(sizeof(at_OFFSET_type));
		*returnMask = (PARM_MASK_type) 0;
	}

	return rval;
}

/*
 * Calculate the right margin
 *
 * for page right stuff see fakepage()
 */

CalcRM(layout, margin)
	CONSTITUENT layout;
	Rsu margin;
{
	extern Rsu pageright;
	at_OFFSET_type *offset;
	PARM_MASK_type mask;

	offset = GetCurOffset(layout, &mask);

	if(offset->left_hand_offset < 0) {
		warn("CalcRM: negative Left Hand Offset (%d), zeroed", offset->left_hand_offset);
		offset->left_hand_offset = 0;
	}

	offset->left_hand_offset = margin + pageright;
	mask |= LEFT_HAND_OFFSET_parm;
	SafeSetAttr(layout, at_OFFSET, (POINTER_type) offset, mask);
	free(offset);
}
/*
 * Calculate the left margin, and while we are at it, also deal with the
 * first line offset
 *
 * Note that we throw away the indent_count: Too painful to do right.
 *
 * for pageleft see fakepage()
 */

CalcLM(layout, pres, oP)
	CONSTITUENT layout, pres;
	Cmpn_t *oP;
{
	extern Rsu pageleft;
	INT_type RHoffset = oP->left_margin + pageleft;
	INT_type first = oP->first_indent;
	INT_type indent = 0;

	if( first) {
		if(first < 0) {
			/*
			 * Cannot go over the Right Hand side,
			 * fake it with a combination of Right Hand Offset
			 * and the indentation;
			 */
			indent = 0 - first;
			RHoffset += first;
		} else
			indent = 0;
	}

	if(first)
		SafeSetIntAttr(pres, cc_1ST_LINE_OFFSET, first);
	if(indent) {
		if(indent < 0) {
			warn("CalcLM: negative indentation (%d), zeroed", indent);
			indent = 0;
		}
		SafeSetIntAttr(pres, cc_INDENTATION, indent);
	}
	doRM(layout, RHoffset);
}

/*
 * Right margin, first line offset & indentation delta
 *
 */
CalcDeltaLM(layout, pres, oP, cP)
	CONSTITUENT layout, pres;
	Cmpn_t *oP, *cP;
{
	extern Rsu pageleft;
	at_OFFSET_type *offset;
	PARM_MASK_type mask;
	INT_type RHoffset = oP->left_margin + pageleft;
	INT_type first = oP->first_indent;
	INT_type indent;
	INT_type status;

	/*
	 * Did only the right margin (and not the first line indent) change?
	 */
	if(first == cP->first_indent) {
		if(first < 0) {	/* subsctract introduced offset for this case */
			RHoffset += first;
		}
		doRM(layout, RHoffset);
	} else {	/* might have to change all three ODA attributes */
		if(RHoffset == cP->left_margin
		    && first > cP->first_indent) {	/* only first indent increases */
			SafeSetIntAttr(pres, cc_1ST_LINE_OFFSET, first);
		} else {	/* juggle all */
			CalcLM(layout, pres, oP);
		}
	}
}

doRM(layout, amount)
	CONSTITUENT layout;
	INT_type amount;
{
	at_OFFSET_type *offset;
	PARM_MASK_type mask;

	offset = GetCurOffset(layout, &mask);
	if(amount < 0) {
		warn("CalcLM: negative Right Hand Offset (%d), zeroed", amount);
		amount = 0;
	}
	offset->right_hand_offset = amount;
	mask |= RIGHT_HAND_OFFSET_parm;
	SafeSetAttr(layout, at_OFFSET, (POINTER_type) offset, mask);
	free(offset);
}
