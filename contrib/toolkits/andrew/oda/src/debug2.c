
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug2.c,v 1.5 89/11/01 15:49:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug2.c,v $


		(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	debug2.c,v $
 * Revision 1.5  89/11/01  15:49:43  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.4  89/10/31  14:43:27  annm
 * fixing lint problems
 * 
 * Revision 1.3  89/10/31  12:29:57  jr
 * Remove spurious test of rval.
 * 
 * Revision 1.2  89/10/26  14:08:35  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:28:21  mss
 * Initial revision
 * 
 * Revision 1.18  89/03/22  15:51:31  annm
 * added routine to print out at_DEFAULT_VALUE_LISTS
 * 
 * Revision 1.17  89/01/24  10:42:14  mss
 * Made forward decls consistent with actual
 * declations (w.r.t. static).
 * 
 * Revision 1.16  88/10/26  12:20:53  jr
 * Add debugging support for SCORES
 * 
 * Revision 1.14  88/10/25  16:59:35  jr
 * OFFSET & SEQUENCE_INT_PAIR_type
 * 
 * Revision 1.13  88/10/21  14:36:27  jr
 * Font stuff done (for now)?
 * 
 * Revision 1.12  88/10/21  13:32:05  jr
 * Some support for fonts (more to come)
 * 
 * Revision 1.11  88/10/19  12:07:00  mss
 * Changed preprocessor statement to use symbol
 * instead of 0; Fixed passing of sequence to
 * io_writenum
 * 
 * Revision 1.10  88/10/17  17:35:26  jr
 * Handle remaining raster
 * attributes (at least the ones
 * we need).
 * 
 * Revision 1.9  88/10/17  11:58:35  jr
 * Add font
 * 
 * Revision 1.8  88/09/07  19:37:13  jr
 * Make INT_PAIR_type debugging output
 * work.
 * 
 * Revision 1.7  88/09/07  19:29:34  jr
 * Make ASN.1 object id's work.
 * 
 * Revision 1.6  88/09/06  13:11:21  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.5  88/08/09  13:45:54  jr
 * Walk generic structure now.
 * 
 * Revision 1.4  88/08/03  12:52:22  jr
 * Fix up a forward defn
 * 
 * Revision 1.3  88/08/03  12:23:42  jr
 * Remove #ifdef 0
 * 
 * Revision 1.2  88/07/29  17:13:39  jr
 * Generator for subordinates working
 * 
 * Revision 1.1  88/07/19  12:27:06  mss
 * Initial revision
 * 
 
*/

#ifndef LINT
static char RCSID[] =
    "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug2.c,v $";
#endif

#define TK_IMPL_DEBUG
#define TK_GVARS_DEBUG2
#define TK_IMPL_ATTR
#define TK_IMPL_CONST
#define TK_IMPL_DOC
#define TK_TKI_IO
#define TK_TKI_STR

#include <values.h>
#include <acd.h>
#include <attr0.h>
#include <str.h>
#include <io.h>
#include <debug.h>
#include <init.h>
#include <parms.h>
#include <font.h>

#undef TK_IMPL_DEBUG
#undef TK_GVARS_DEBUG2
#undef TK_IMPL_CONST
#undef TK_IMPL_DOC

/* Forward declarations */
static INT_type WriteConstructionExpr();
static INT_type WriteConstructionFactor();
static INT_type WriteConstructionTerm();
static INT_type WriteConstructionFactor();
static INT_type WriteSeqConstructionTerm();
static INT_type WalkConstructionExpr();
static INT_type WalkConstructionTerm();
static INT_type WalkSeqConstructionTerm();
static INT_type WalkConstructionFactor();
static INT_type WriteDebugFont();
static INT_type WriteDebugStructName();
static INT_type WriteDebugISO9541IdArray();
static INT_type WriteDebugISO9541Id();
static INT_type WriteDebugReal();
static INT_type WriteDebugDsclass();
static INT_type WriteDebugModes();
static INT_type WriteDebugModalFontAttr();
static INT_type WriteFontEnum();
static INT_type WriteDebugVarScripts();
static INT_type WriteDebugVarScript();
static INT_type WriteDebugScores();
static INT_type WriteDebugScore();

/* This constituent has already been written out */

INT_type DEBUG_WalkGenericObject(od, node)
    IN INT_type		    od;
    IN INTERNAL_CONSTITUENT node;
{
	     INT_type		rval;
    REGISTER ATTR_attribute_t	attr;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* See if we've already been here */
    if (node->debug_counter == last_debug_counter) {
	goto LEAVE;
    }

    /* Write it out */
    rval = WriteConstituentState(od, (CONSTITUENT) node);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* See what kind of constituent */
    if (CONST_IsBasic(node)) {
	/* We don't implement this, for now */
	goto LEAVE;
    }

    /* See if if it has "Generator for Subordinates */
    if (!CONST_UnsafeHasAttr(node, at_GEN_FOR_SUBORDINATES)) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* Supposed to have "Generator for Subordinates" -- make sure */
    attr = ATTR_FindAttr( (CONSTITUENT) node, at_GEN_FOR_SUBORDINATES);
    if (attr == ERROR_ATTRIBUTE) {
	rval = ERROR_INT;
	goto LEAVE;
    }

    rval = WalkConstructionExpr(od, GET_CONSTRUCTION_EXPR(attr));

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type CONSTRUCTION_EXPR_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type		    rval;
    CONSTRUCTION_EXPR_type  *value;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    value = GET_CONSTRUCTION_EXPR(attr);

    rval = IO_WriteChar(od, ' ');
    if (rval != ERROR_INT) {
	rval = WriteConstructionExpr(od, value);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteConstructionExpr(od, expr)
    INT_type		    od;
    CONSTRUCTION_EXPR_type  *expr;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (expr->construction_tag) {

	case TERM_tag:
	    rval = WriteConstructionTerm(od, &expr->construction_value.term);
	    break;

	case SEQUENCE_tag:
	    rval = WriteSeqConstructionTerm(od, "SEQ",
				expr->construction_value.construction);
	    break;

	case AGGREGATE_tag:
	    rval = WriteSeqConstructionTerm(od, "AGG",
				expr->construction_value.construction);
	    break;

	case CHOICE_tag:
	    rval = WriteSeqConstructionTerm(od, "CHO",
				expr->construction_value.construction);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteSeqConstructionTerm(od, name, seq)
    INT_type			    od;
    CHAR_type			    *name;
    SEQUENCE_CONSTRUCTION_TERM_type seq;
{
	     INT_type		    rval;
    REGISTER CONSTRUCTION_TERM_type *terms;
    REGISTER INT_type		    length;
    REGISTER INT_type		    i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = IO_PrintString(od, "%s[", name);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    length = seq -> length;
    terms = seq -> sequence_value.construction_terms;
    rval = SUCCESS_INT;
    for (i=0; i<length && rval!=ERROR_INT; i++) {
	if (i != 0) {
	    rval = IO_WriteChar(od, ' ');
	}
	if (rval != ERROR_INT) {
	    rval = WriteConstructionTerm(od, &terms[i]);
	}
    }

    if (rval != ERROR_INT) {
	rval = IO_WriteChar(od, ']');
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteConstructionTerm(od, term)
    IN INT_type			od;
    IN CONSTRUCTION_TERM_type	*term;
{
    INT_type	rval;
    CHAR_type	*name;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (term->term_tag) {
	case REQ_tag:	    name = (CHAR_type *) 0; break;
	case OPT_tag:	    name = "OPT";	    break;
	case REP_tag:	    name = "REP";	    break;
	case OPT_REP_tag:   name = "OPT REP";	    break;
	default:	    rval = ERROR_INT;
			    TKError = INTERNAL_err;
			    goto LEAVE;
    }

    if (name != (CHAR_type *) 0) {
	rval = IO_PrintString(od, "%s ", name);
    }
    if (rval != ERROR_INT) {
	rval = WriteConstructionFactor(od, &term->factor);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteConstructionFactor(od, factor)
    IN INT_type			od;
    IN CONSTRUCTION_FACTOR_type	*factor;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (factor->factor_tag) {
	case OBJECT_CLASS_ID_tag:
	    rval =
		DEBUG_WriteDebugConstituent(od,
			(INTERNAL_CONSTITUENT) factor->factor_value.object_class_id);
	    break;

	case EXPR_tag:
	    rval = WriteConstructionExpr(od, factor->factor_value.expr);
	    break;

	default:	    rval = ERROR_INT;
			    TKError = INTERNAL_err;
			    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WalkConstructionExpr(od, expr)
    IN INT_type			od;
    IN CONSTRUCTION_EXPR_type	*expr;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (expr->construction_tag) {

	case TERM_tag:
	    rval = WalkConstructionTerm(od, &expr->construction_value.term);
	    break;

	case SEQUENCE_tag:
	case AGGREGATE_tag:
	case CHOICE_tag:
	    rval = WalkSeqConstructionTerm(od,
					   expr->construction_value.construction);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WalkConstructionTerm(od, term)
    IN INT_type			od;
    IN CONSTRUCTION_TERM_type	*term;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = WalkConstructionFactor(od, &term->factor);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WalkConstructionFactor(od, factor)
    IN INT_type			od;
    IN CONSTRUCTION_FACTOR_type	*factor;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    switch (factor->factor_tag) {

	case OBJECT_CLASS_ID_tag:
	    rval = DEBUG_WalkGenericObject(od,
			(INTERNAL_CONSTITUENT) factor->factor_value.object_class_id);
	    break; 

	case EXPR_tag:
	    rval = WalkConstructionExpr(od, factor->factor_value.expr);
	    break;

	default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WalkSeqConstructionTerm(od, seq)
    INT_type			    od;
    SEQUENCE_CONSTRUCTION_TERM_type seq;
{
	     INT_type		    rval;
    REGISTER CONSTRUCTION_TERM_type *terms;
    REGISTER INT_type		    length;
    REGISTER INT_type		    i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    length = seq -> length;
    terms = seq -> sequence_value.construction_terms;
    rval = SUCCESS_INT;
    for (i=0; i<length && rval!=ERROR_INT; i++) {
	rval = WalkConstructionTerm(od, &terms[i]);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static struct {
    VALUE_type	id;
    CHAR_type	*name;
} object_ids[] = {

    { FORM_CC_id,   "formatted character content architecture" },
    { PROC_CC_id,   "processable character content architecture" },
    { FP_CC_id,	    "formatted processable character content architecture" },
    { FORM_RG_id,   "formatted raster graphics content architecture" },
    { FP_RG_id,	    "formatted processable raster graphics content architecture" },
    { FORM_GG_id,   "formatted geometric graphics content architecture" },
    { CC_CODING_id, "character content encoding" },
    { RG_T6_CODING_id,
		    "T6 encoding" },
    { RG_T4_1_CODING_id,
		    "T4 one dimensional encoding" },
    { RG_T4_2_CODING_id,
		    "T4 two dimensional encoding" },
    { RG_BITMAP_CODING_id,
		    "bitmap encoding" },
    { GG_CODING_id, "geometric graphics encoding" },
    { UNKNOWN_id,   "unknown" }
};
#define NIDS	( (INT_type)(sizeof(object_ids) / sizeof(object_ids[0])))

INT_type SEQUENCE_INT_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
	     INT_type		rval;
    REGISTER INT_type		i;
    REGISTER INT_type		length;
    REGISTER INT_type		*ints;
	     SEQUENCE_INT_type	value;
	     VALUE_type		id;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_SEQUENCE_INT(attr);

    /* Write out as integers */
    length = value->length;
    ints = value->sequence_value.ints;
    for (i=0; i<length; i++) {
	IO_PrintInt(od, " %d", ints[i]);
    }

    /*
       If attribute is "Content Architecture Class" or "Type
       of Coding", write out string name.
    */
    if (attrid==at_CONTENT_ARCH_CLASS || attrid==at_TYPE_OF_CODING) {
	id = LookUpObjectId(value);
	if (id == ERROR_id) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	for (i=0; i<NIDS; i++) {
	    if (object_ids[i].id == id) {
		IO_PrintString(od, "\n        ['%s']", object_ids[i].name);
		break;
	    }
	}
    }
    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type INT_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type	rval;
    INT_type	value;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_INT(attr);

    rval = IO_PrintInt(od, " %d", value);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
INT_type rg_CLIPPING_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type		rval;
    rg_CLIPPING_type	*value;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_rg_CLIPPING(attr);

    rval = IO_Print(od, " < ");
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    if ((attr->parm_mask & COORD1_parm) != 0) {
	rval = IO_PrintInt(od, "<%d, ", value->coord1.first);
	if (rval != ERROR_INT) {
	    rval = IO_PrintInt(od, "%d>", value->coord1.second);
	}
    } else {
	rval = SUCCESS_INT;
    }

    if (rval != ERROR_INT) {
	rval = IO_Print(od, ", ");
    }

    if ((attr->parm_mask & COORD2_parm) != 0) {
	if (rval != ERROR_INT) {
	    rval = IO_PrintInt(od, "<%d, ", value->coord2.first);
	}
	if (rval != ERROR_INT) {
	    rval = IO_PrintInt(od, "%d>", value->coord2.second);
	}
    }

    if (rval != ERROR_INT) {
	rval = IO_Print(od, " >");
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type rg_PEL_SPACING_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type		rval;
    rg_PEL_SPACING_type	*value;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_rg_PEL_SPACING(attr);

    if (value->null) {
	rval = IO_Print(od, " 'null'");
	goto LEAVE;
    }

    rval = IO_PrintInt(od, " (\"length\" %d)", value->length);
    if (rval != ERROR_INT) {
	rval = IO_PrintInt(od, " (\"pel spaces\" %d)", value->pel_spaces);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type rg_IMAGE_DIMENSIONS_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type			rval;
    rg_IMAGE_DIMENSIONS_type	*value;
    REGISTER CHAR_type		*name;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_rg_IMAGE_DIMENSIONS(attr);

    switch (value->image_dimensions_tag) {

	case WIDTH_CONTROLLED_tag:
	    rval = IO_PrintInt(od, " (\"minimum width\" %d)",
			value->image_dimensions_value.width_controlled.first);
	    if (rval != ERROR_INT) {
		rval = IO_PrintInt(od, " (\"preferred width\" %d)",
			value->image_dimensions_value.width_controlled.second);
	    }
	    break;

	case HEIGHT_CONTROLLED_tag:
	    rval = IO_PrintInt(od, " (\"minimum height\" %d)",
			value->image_dimensions_value.height_controlled.first);
	    if (rval != ERROR_INT) {
		rval = IO_PrintInt(od, " (\"preferred height\" %d)",
			value->image_dimensions_value.height_controlled.second);
	    }
	    break; 

	case AREA_CONTROLLED_tag:
	    rval = IO_PrintInt(od, " (\"minimum width\" %d)",
		value->image_dimensions_value.area_controlled.minimum_width);
	    if (rval != ERROR_INT) {
		rval = IO_PrintInt(od, " (\"preferred width\" %d)",
		  value->image_dimensions_value.area_controlled.preferred_width);
	    }
	    if (rval != ERROR_INT) {
		rval = IO_PrintInt(od, " (\"minimum height\" %d)",
		  value->image_dimensions_value.area_controlled.minimum_height);
	    }
	    if (rval != ERROR_INT) {
		rval = IO_PrintInt(od, " (\"preferred height\" %d)",
		  value->image_dimensions_value.area_controlled.preferred_height);
	    }
	    if (rval != ERROR_INT) {
		rval = IO_Print(od, " (\"aspect ratio flag\" ");
	    }
	    if (rval != ERROR_INT) {
		name =
 GetValueName(value->image_dimensions_value.area_controlled.aspect_ratio_flag);
		if (name == (CHAR_type *) 0) {
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		rval = IO_PrintString(od, "'%s')", name);
	    }

	case AUTOMATIC_tag:
	    rval = IO_Print(od, " \"automatic\"");
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type INT_PAIR_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type	    rval;
    INT_PAIR_type   *value;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_INT_PAIR(attr);

    rval = IO_PrintInt(od, " <%d, ", value->first);
    if (rval != ERROR_INT) {
	rval = IO_PrintInt(od, "%d>", value->second);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQUENCE_FONT_PAIR_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
	     INT_type			rval;
	     SEQUENCE_FONT_PAIR_type	value;
    REGISTER INT_type			length;
    REGISTER INT_type			i;
    REGISTER FONT_PAIR_type		*font_pairs;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Extract value */
    value = GET_SEQUENCE_FONT_PAIR(attr);

    /* Extract length & pointer to font pairs */
    length = value -> length;
    font_pairs = value -> sequence_value.font_pairs;

    rval = SUCCESS_INT;
    for (i=0; i<length && rval!=ERROR_INT; i++) {
	rval = IO_PrintInt(od, "\n      <%d,", font_pairs[i].font_id);
	if (rval != ERROR_INT) {
	    rval = WriteDebugFont(od, &font_pairs[i].font_reference);
	}
	if (rval != ERROR_INT) {
	    rval = IO_Print(od, "\n      >");
	}
    }
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\n    ");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

/*  Tables for mapping some font field values to string names */

typedef struct {
    INT_type	value;
    CHAR_type	*name;
} EnumTable_t;

static EnumTable_t StructurTable[] = {
    { ISO_STRUCTUR_unspec,	(CHAR_type *) 0 },
    { ISO_STRUCTUR_solid,	"solid" },
    { ISO_STRUCTUR_outline,	"outline" },
    { ISO_STRUCTUR_inline,	"inline" },
    { ISO_STRUCTUR_shadow,	"shadow" },
    { ISO_STRUCTUR_patterned,	"patterned" }
};
#define NSTRUCTUR   ((INT_type)(sizeof(StructurTable) / sizeof(StructurTable[0])))

static EnumTable_t PostureTable[] = {
    { ISO_POSTURE_unspec,	(CHAR_type *) 0 },
    { ISO_POSTURE_upright,	"upright" },
    { ISO_POSTURE_clockwise,	"clockwise" },
    { ISO_POSTURE_counter,	"counter-clockwise" },
    { ISO_POSTURE_italic,	"italic" },
    { ISO_POSTURE_other,	"other" }
};
#define NPOSTURE   ((INT_type)(sizeof(PostureTable) / sizeof(PostureTable[0])))

static EnumTable_t WeightTable[] = {
    { ISO_WEIGHT_unspec,	(CHAR_type *) 0 },
    { ISO_WEIGHT_ultra_light,	"ultra light" },
    { ISO_WEIGHT_extra_light,	"extra light" },
    { ISO_WEIGHT_light,		"light-clockwise" },
    { ISO_WEIGHT_semi_light,	"semi light" },
    { ISO_WEIGHT_medium,	"medium" },
    { ISO_WEIGHT_semi_bold,	"semi bold" },
    { ISO_WEIGHT_bold,		"bold" },
    { ISO_WEIGHT_extra_bold,	"extra bold" },
    { ISO_WEIGHT_ultra_bold,	"ultra bold" }
};
#define NWEIGHT   ((INT_type)(sizeof(WeightTable) / sizeof(WeightTable[0])))

static EnumTable_t PropwdthTable[] = {
    { ISO_PROPWDTH_unspec,	(CHAR_type *) 0 },
    { ISO_PROPWDTH_ultra_condensed,
				"ultra condensed" },
    { ISO_PROPWDTH_extra_condensed,
				"extra condensed" },
    { ISO_PROPWDTH_condensed,	"condensed" },
    { ISO_PROPWDTH_semi_condensed,
				"semi condensed" },
    { ISO_PROPWDTH_medium,	"medium" },
    { ISO_PROPWDTH_semi_expanded,
				"semi expanded" },
    { ISO_PROPWDTH_expanded,	"expanded" },
    { ISO_PROPWDTH_extra_expanded,
				"extra expanded" },
    { ISO_PROPWDTH_ultra_expanded,
				"ultra expanded" }
};
#define NPROPWDTH   ((INT_type)(sizeof(PropwdthTable) / sizeof(PropwdthTable[0])))

static EnumTable_t EsclassTable[] = {
    { ISO_ESCLASS_unspec,	(CHAR_type *) 0 },
    { ISO_ESCLASS_fixed,	"fixed pitch" },
    { ISO_ESCLASS_variable,	"variable pitch" }
};
#define NESCLASS   ((INT_type)(sizeof(EsclassTable) / sizeof(EsclassTable[0])))

static INT_type WriteDebugFont(od, font)
    IN INT_type		    od;
    IN FONT_REFERENCE_type  *font;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Just do each field */
    rval = WriteDebugStructName(od, "FONTNAME", &font->fontname);
    if (rval != ERROR_INT) {
	rval = WriteDebugStructName(od, "DIFNTNAM", &font->difntnam);
    }
    if (rval != ERROR_INT) {
	rval = WriteFontEnum(od, "STRUCTUR",
			     StructurTable, NSTRUCTUR, font->structur);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugStructName(od, "TYPEFACE", &font->typeface);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugStructName(od, "FAMILY", &font->family);
    }
    if (rval != ERROR_INT) {
	rval = WriteFontEnum(od, "POSTURE",
			     PostureTable, NPOSTURE, font->posture);
    }
    if (rval != ERROR_INT) {
	rval = WriteFontEnum(od, "WEIGHT", WeightTable, NWEIGHT, font->weight);
    }
    if (rval != ERROR_INT) {
	rval = WriteFontEnum(od, "PROPWDTH",
			     PropwdthTable, NPROPWDTH, font->propwdth);
    }
    if (rval!=ERROR_INT && font->numchars!=-1) {
	rval = IO_PrintInt(od, "\n         (\"NUMCHARS\" %d)", font->numchars);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "DSNSIZE", &font->dsnsize);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MINSIZE", &font->minsize);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MAXSIZE", &font->maxsize);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugDsclass(od, font->dsclass);
    }
    /* ISO/STRUCTUR */
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MNFEATSZ", &font->mnfeatsz);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugModes(od, &font->modes);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugStructName(od, "MODE", &font->mode);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugStructName(od, name, sname)
    IN INT_type		    od;
    IN CHAR_type	    *name;
    IN STRUCTURED_NAME_type *sname;
{
    INT_type	rval;
    ISO9541_ID_type	id[1];
    ISO9541_ID_type	*ids;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    switch (sname->structured_name_tag) {

	case STRUCTURED_NAME_UNSPEC_tag:
		rval = SUCCESS_INT;
		goto LEAVE;

	case STRUCTURED_NAME_ID_tag:
		id[0] = sname->structured_name_value.id;
		rval = WriteDebugISO9541IdArray(od, name, id, (INT_type) 1);
		break;

	case STRUCTURED_NAME_SEQ_tag:
		ids = sname->structured_name_value.idseq->sequence_value.iso9541_ids;
		rval = WriteDebugISO9541IdArray(od, name, ids,
				    sname->structured_name_value.idseq->length);
		break;

	default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugISO9541IdArray(od, name, array, length)
    IN INT_type		od;
    IN CHAR_type	*name;
    IN ISO9541_ID_type	*array;
    IN INT_type		length;
{
	     INT_type	rval;
    REGISTER INT_type	i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = IO_PrintString(od, "\n         (\"%s\"", name);
    for (i=0; rval!= ERROR_INT && i<length; i++) {
	rval = WriteDebugISO9541Id(od, &array[i]);
    }
    if (rval != ERROR_INT) {
	rval = IO_Print(od, ")");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugISO9541Id(od, id)
    IN INT_type		od;
    IN ISO9541_ID_type	*id;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    switch (id->iso9541_id_tag) {
	case ID_INT_tag:
		rval = IO_PrintInt(od, " %d", id->iso9541_id_value.integer);
		break;
	case ID_STR_tag:
		rval = DEBUG_WriteBytes(od,
			id->iso9541_id_value.octet_string->sequence_value.bytes,
			id->iso9541_id_value.octet_string->length);
		break;
	default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugReal(od, name, real)
    IN INT_type		od;
    IN CHAR_type	*name;
    IN INT_PAIR_type	*real;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if unspecified */
    if (real->second == 0) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = IO_PrintString(od, "\n         (\"%s\" ", name);
    if (rval != ERROR_INT) {
	rval = IO_PrintInt(od, "%d/", real->first);
    }
    if (rval != ERROR_INT) {
	rval = IO_PrintInt(od, "%d)", real->second);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugDsclass(od, seq)
    IN INT_type		    od;
    IN SEQUENCE_INT_type    seq;
{
	     INT_type	rval;
    REGISTER INT_type	i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    if (seq == NULL_SEQUENCE) {
        rval = SUCCESS_INT;
	goto LEAVE;
    }
    rval = IO_Print(od, "\n         (\"DSCLASS\"");
    for (i=0; rval!=ERROR_INT && i<seq->length; i++) {
	rval = IO_PrintInt(od, " %d", seq->sequence_value.ints[i]);
    }
    /* Should see if it's something recognized here */
    if (rval != ERROR_INT) {
	rval = IO_Print(od, ")");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugModes(od, modes)
    IN INT_type		    od;
    IN WRITING_MODES_type   *modes;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if specified */
    if (!modes->specified) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = WriteDebugModalFontAttr(od, (CHAR_type *) "0", &modes->iso_0);
    if (rval != ERROR_INT) {
	rval = WriteDebugModalFontAttr(od, (CHAR_type *) "180", &modes->iso_180);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugModalFontAttr(od, (CHAR_type *) "270", &modes->iso_270);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugModalFontAttr(od, name, attr)
    IN INT_type		    od;
    IN CHAR_type	    *name;
    IN MODAL_FONT_ATTR_type *attr;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if specified */
    if (!attr->specified) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = IO_PrintString(od, "\n       (\"%s\"", name);
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "ESCAPDIR", &attr->escapdir);
    }
    if (rval != ERROR_INT) {
	rval = WriteFontEnum(od, "ESCLASS",
			     EsclassTable, NESCLASS, attr->esclass);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "AVGCESC", &attr->avgesc);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "AVGLCESC", &attr->avglcesc);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "AVGCPESC", &attr->avgcpesc);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "AVGWTESC", &attr->avgwtesc);
    }
    if (rval!=ERROR_INT && attr->tabesc.specified) {
	rval = WriteDebugReal(od, "TX", &attr->tabesc.tx);
	if (rval != ERROR_INT) {
	    rval = WriteDebugReal(od, "TY", &attr->tabesc.ty);
	}
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MFX", &attr->mfx);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MLX", &attr->mlx);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MBX", &attr->mbx);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MRX", &attr->mrx);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MINAMPL", &attr->minampl);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "MAXAMPL", &attr->maxampl);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugScores(od, &attr->scores);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugVarScripts(od, &attr->varscrpt);
    }
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\n       )");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteFontEnum(od, name, table, nelems, value)
    IN INT_type	    od;
    IN CHAR_type    *name;
    IN EnumTable_t  table[];
    IN INT_type	    nelems;
    IN INT_type	    value;
{
	     INT_type	rval;
    REGISTER INT_type	i;


#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /*  Look for value */
    for (i=0; i<=nelems; i++) {
	if (table[i].value == value) {
	    if (table[i].name == (CHAR_type *) 0) {
		/* Unspecified value */
		rval = SUCCESS_INT;
		goto LEAVE;
	    }
	    rval = IO_PrintString(od, "\n         (\"%s\" ", name);
	    if (rval != ERROR_INT) {
		rval = IO_PrintString(od, "%s)", table[i].name);
		goto LEAVE;
	    }
	}
    }

    /* If here, then no match was found */
    rval = IO_PrintString(od, "\n         (\"%s\" illegal value:", name);
    if (rval != ERROR_INT) {
	rval = IO_PrintInt(od, "%d)", value);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugVarScripts(od, var)
    IN INT_type		    od;
    IN VARIANT_SCRIPTS_type *var;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if specified */
    if (!var->specified) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* It's specified, just output each field */
    rval = WriteDebugVarScript(od, &var->lscript, "LSCRIPT");
    if (rval != ERROR_INT) {
	rval = WriteDebugVarScript(od, &var->rscript, "RSCRIPT");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugVarScript(od, var, name)
    IN INT_type		    od;
    IN VARIANT_SCRIPT_type  *var;
    IN CHAR_type	    *name;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if specified */
    if (!var->specified) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = IO_PrintString(od, "\n        (\"%s\"", name);

    /* It's specified, just output each field */
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "VXOFFSET", &var->vxoffset);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "VYOFFSET", &var->vyoffset);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "VXSCALE", &var->vxscale);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "VYSCALE", &var->vyscale);
    }
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\n        )");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugScores(od, scores)
    IN INT_type	    od;
    IN SCORES_type  *scores;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if specified */
    if (!scores->specified) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    /* It's specified, just output each field */
    rval = WriteDebugScore(od, &scores->rscore, "RSCORE");
    if (rval != ERROR_INT) {
	rval = WriteDebugScore(od, &scores->lscore, "LSCORE");
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugScore(od, &scores->tscore, "TSCORE");
    }
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\n        )");
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugScore(od, score, name)
    IN INT_type	    od;
    IN SCORE_type   *score;
    IN CHAR_type    *name;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* See if specified */
    if (!score->specified) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = IO_PrintString(od, "\n        (\"%s\"", name);

    /* It's specified, just output each field */
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "SCORPOS", &score->scorpos);
    }
    if (rval != ERROR_INT) {
	rval = WriteDebugReal(od, "SCORTHIK", &score->scorthik);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQUENCE_INT_PAIR_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
	     INT_type		    rval;
	     SEQUENCE_INT_PAIR_type value;
    REGISTER INT_type		    i;
    REGISTER INT_type		    length;
    REGISTER INT_PAIR_type	    *pairs;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_SEQUENCE_INT_PAIR(attr);

    length = value -> length;
    pairs = value -> sequence_value.int_pairs;
    rval = SUCCESS_INT;
    for (i=0; i<length && rval!=ERROR_INT; i++) {
	rval = IO_PrintInt(od, " <%d, ", pairs[i].first);
	if (rval != ERROR_INT) {
	    rval = IO_PrintInt(od, "%d>", pairs[i].second);
	}
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type at_OFFSET_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type	    rval;
    at_OFFSET_type  *value;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Extract value */
    value = GET_at_OFFSET(attr);

    if ((attr->parm_mask & LEADING_OFFSET_parm) != 0) {
	rval = IO_PrintInt(od, " (\"leading offset\" %d)", value->leading_offset);
    }
    if ((attr->parm_mask & TRAILING_OFFSET_parm)!=0 && rval!=ERROR_INT) {
	rval = IO_PrintInt(od, " (\"trailing offset\" %d)",
			   value->trailing_offset);
    }
    if ((attr->parm_mask & LEFT_HAND_OFFSET_parm)!=0 && rval!=ERROR_INT) {
	rval = IO_PrintInt(od, " (\"left-hand offset\" %d)",
			   value->left_hand_offset);
    }
    if ((attr->parm_mask & RIGHT_HAND_OFFSET_parm)!=0 && rval!=ERROR_INT) {
	rval = IO_PrintInt(od, " (\"right-hand offset\" %d)",
			   value->right_hand_offset);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type at_DEFAULT_VALUE_LISTS_WDbug(od, attrid, attr)
    IN INT_type		od;
    IN INT_type		attrid;
    IN ATTR_attribute_t	attr;
{

    REGISTER INT_type rval;

    at_DEFAULT_VALUE_LISTS_type *TheDVL;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheDVL = GET_at_DEFAULT_VALUE_LISTS(attr);

    if( TheDVL->page_attributes != NULL_CONSTITUENT ){
	rval = IO_Print( od, "\n\tThe DVL page attributes: " );
	if( rval == ERROR_INT ){   
	    goto LEAVE;
	}
	rval = DEBUG_WriteDebugConstituent( od, 
		    (INTERNAL_CONSTITUENT) TheDVL->page_attributes );
	if( rval == ERROR_INT ){
	    goto LEAVE;
	}
    }

    if( TheDVL->frame_attributes != NULL_CONSTITUENT ){
	rval = IO_Print( od, "\n\tThe DVL frame attributes: " );
	if( rval == ERROR_INT ){   
	    goto LEAVE;
	}
	rval = DEBUG_WriteDebugConstituent( od,
					   (INTERNAL_CONSTITUENT) TheDVL->frame_attributes );
	if( rval == ERROR_INT ){
	    goto LEAVE;
	}
    }

    if( TheDVL->block_attributes != NULL_CONSTITUENT ){
	rval = IO_Print( od, "\n\tThe DVL block attributes: " );
	if( rval == ERROR_INT ){   
	    goto LEAVE;
	}
	rval = DEBUG_WriteDebugConstituent( od, 
		    (INTERNAL_CONSTITUENT) TheDVL->block_attributes );
	if( rval == ERROR_INT ){
	    goto LEAVE;
	}
    }

    if( TheDVL->composite_logical_attributes != NULL_CONSTITUENT ){
	rval = IO_Print( od, "\n\tThe DVL composite logical attributes: " );
	if( rval == ERROR_INT ){   
	    goto LEAVE;
	}
	rval = DEBUG_WriteDebugConstituent( od, 
		    (INTERNAL_CONSTITUENT) TheDVL->composite_logical_attributes );
	if( rval == ERROR_INT ){
	    goto LEAVE;
	}
    }

    if( TheDVL->basic_logical_attributes != NULL_CONSTITUENT ){
	rval = IO_Print( od, "\n\tThe DVL basic logical attributes: " );
	if( rval == ERROR_INT ){   
	    goto LEAVE;
	}
	rval = DEBUG_WriteDebugConstituent( od, 
		(INTERNAL_CONSTITUENT) TheDVL->basic_logical_attributes );
	if( rval == ERROR_INT ){
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
