/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug1.c,v 1.3 89/10/31 14:31:55 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	debug1.c,v $
 * Revision 1.3  89/10/31  14:31:55  annm
 * fixing lint problems
 * 
 * Revision 1.2  89/10/26  14:08:12  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:28:02  mss
 * Initial revision
 * 
 * Revision 1.45  89/07/19  21:59:43  mss
 * Follow empty constituents when present
 * 
 * Revision 1.44  89/03/24  15:17:57  mss
 * Changed debugging output to label new DVL and DAP
 * components
 * 
 * Revision 1.43  89/01/24  10:41:32  mss
 * Made forward decls consistent with actual
 * declations (w.r.t. static).
 * 
 * Revision 1.42  89/01/18  15:54:44  annm
 * fixed the cc_CHAR_IMAGE_ORIENTATION's to get rid of the _IMAGE
 * 
 * Revision 1.41  88/12/15  14:18:19  mss
 * Changed handling of NULL_CONSTITUENT in 
 * document header to match use in rest of
 * toolkit
 * 
 * Revision 1.40  88/12/13  19:56:15  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.39  88/11/17  15:03:52  jr
 * Remove stub for at_OFFSET
 * 
 * Revision 1.38  88/11/07  09:45:51  mss
 * Added counter to WriteSubgraphState so
 * that one could dump parts of tree
 * instead of only whole document
 * 
 * Revision 1.37  88/10/17  11:57:57  jr
 * Add support for INT's
 * 
 * Revision 1.36  88/09/07  17:36:44  jr
 * Add LAYOUT_OBJECT_type
 * 
 * Revision 1.35  88/09/06  14:53:00  annm
 * no change
 * 
 * Revision 1.34  88/09/06  12:01:58  annm
 * no change
 * 
 * Revision 1.33  88/08/31  20:09:41  jr
 * Format the internal error
 * 
 * Revision 1.32  88/08/29  13:52:41  jr
 * Update for new attr names.
 * 
 * Revision 1.31  88/08/26  17:07:55  jr
 * Oops .. a small typo
 * 
 * Revision 1.30  88/08/26  16:36:28  jr
 * Handle all values
 * 
 * Revision 1.29  88/08/09  13:45:20  jr
 * Walk generic structure now.
 * 
 * Revision 1.28  88/07/29  17:13:10  jr
 * Generator for subordinates working
 * 
 * Revision 1.27  88/07/29  12:01:42  jr
 * Some missing defns were added.
 * 
 * Revision 1.26  88/07/29  10:51:53  jr
 * Use new attr0 table
 * 
 * Revision 1.25  88/07/19  12:26:37  mss
 * Made debug_counter a sharable global for use with
 * other debug modules
 * 
 * Revision 1.24  88/07/08  16:22:34  mss
 * Added entries to print out attributes for User-readable comments
 * and Application Comments
 * 
 * Revision 1.23  88/07/05  16:05:34  jr
 * Remove ref to form_cca
 * 
 * Revision 1.22  88/06/14  17:01:30  jr
 * Remove bad Andrew hack
 * 
 * Revision 1.21  88/06/08  16:56:03  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.20  88/06/08  14:35:10  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] =
    "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug1.c,v $";
#endif

#define TK_IMPL_DEBUG
#define TK_GVARS_DEBUG1
#define TK_IMPL_ATTR
#define TK_IMPL_CONST
#define TK_IMPL_DOC
#define TK_TKI_IO
#define TK_TKI_STR
#define TK_TKI_TKERR

#include <values.h>
#include <acd.h>
#include <attr0.h>
#include <str.h>
#include <io.h>
#include <debug.h>
#include <tkerr.h>
#include <parms.h>

#undef TK_IMPL_DEBUG
#undef TK_GVARS_DEBUG1
#undef TK_IMPL_ATTR
#undef TK_IMPL_CONST
#undef TK_IMPL_DOC
#undef TK_TKI_IO
#undef TK_TKI_STR
#undef TK_TKI_TKERR

    /*
       This constant controls how many bytes in a SEQUENCE_BYTE_type are
       output during debugging.
    */
#define MAX_BYTES_OUT	((INT_type)250)

/* Forward declarations */
static INT_type	WriteUnreachableConstituents();
static INT_type	WriteAttributesState();
static INT_type	InternalSubgraphState();
static INT_type	WalkSpecificObject();
static INT_type	WriteDebugString();
static INT_type	WriteDebugByte();
static INT_type	WriteConstituentString();

static struct {
    VALUE_type	value;
    CHAR_type	*name;
} values[] = {

    { at_OBJECT_TYPE_doc_layout_root,	"document layout root" },
    { at_OBJECT_TYPE_page_set,		"page set" },
    { at_OBJECT_TYPE_page,		"composite or basic page" },
    { at_OBJECT_TYPE_frame,		"frame" },
    { at_OBJECT_TYPE_block,		"block" },
    { at_OBJECT_TYPE_doc_logical_root,	"document logical root" },
    { at_OBJECT_TYPE_comp_logical_obj,	"composite logical object" },
    { at_OBJECT_TYPE_bas_logical_obj,	"basic logical object" },
    { at_CONTENT_TYPE_form_rgca,
		    "formatted raster graphics content architecture" },
    { at_POSITION_right_hand_aligned,	"right-hand aligned" },
    { at_POSITION_centred,		"centred" },
    { at_POSITION_left_hand_aligned,	"left-hand aligned" },
    { at_POSITION_normal_order,		"normal order" },
    { at_POSITION_reverse_order,	"reverse order" },
    { at_BORDER_solid,			"solid" },
    { at_BORDER_dashed,			"dashed" },
    { at_BORDER_dot,			"dot" },
    { at_BORDER_dash_dot,		"dash-dot" },
    { at_BORDER_dash_dot_dot,		"dash-dot-dot" },
    { at_BORDER_invisible,		"invisible" },
    { at_LAYOUT_PATH_type_0,		"0 degrees" },
    { at_LAYOUT_PATH_type_90,		"90 degrees" },
    { at_LAYOUT_PATH_type_180,		"180 degrees" },
    { at_LAYOUT_PATH_type_270,		"270 degrees" },
    { at_TRANSPARENCY_transparent,	"transparent" },
    { at_TRANSPARENCY_opaque,		"opaque" },
    { at_COLOUR_colourless,		"colourless" },
    { at_COLOUR_white,			"white" },
    { at_MEDIUM_TYPE_recto,		"recto" },
    { at_MEDIUM_TYPE_verso,		"verso" },
    { at_MEDIUM_TYPE_unspecified,	"unspecified" },
    { at_PROTECTION_protected,		"protected" },
    { at_PROTECTION_unprotected,	"unprotected" },
    { at_BLK_ALGNM_right_hand_aligned,	"right-hand aligned" },
    { at_BLK_ALGNM_left_hand_aligned,	"left-hand aligned" },
    { at_BLK_ALGNM_centred,		"centred" },
    { at_BLK_ALGNM_null,		"null" },
    { at_CONCATENATION_concatenated,	"concatenated" },
    { at_CONCATENATION_non_concated,	"non-concatenated" },
    { at_FILL_ORDER_normal_order,	"normal order" },
    { at_FILL_ORDER_reverse_order,	"reverse order" },
    { dp_GENERIC_LAY_STRUCT_fact_set,	"factor set" },
    { dp_GENERIC_LAY_STRUCT_part_gen,	"partial generator set" },
    { dp_GENERIC_LAY_STRUCT_comp_gen,	"complete generator set" },
    { dp_SPECIFIC_LAY_STRUCT_present,	"present" },
    { dp_GENERIC_LOG_STRUCT_fact_set,	"factor set" },
    { dp_GENERIC_LOG_STRUCT_part_gen,	"partial generator set" },
    { dp_GENERIC_LOG_STRUCT_comp_gen,	"complete generator set" },
    { dp_SPECIFIC_LOG_STRUCT_present,	"present" },
    { dp_LAYOUT_STYLES_present,		"present" },
    { dp_PRESENTATION_STYLES_present,	"present" },
    { dp_INTERCHANGE_FMT_CLASS_a,	"A" },
    { dp_INTERCHANGE_FMT_CLASS_b,	"B" },
    { cc_ALIGNMENT_start_aligned,	"start-aligned" },
    { cc_ALIGNMENT_end_aligned,		"end-aligned" },
    { cc_ALIGNMENT_centred,		"centred" },
    { cc_ALIGNMENT_justified,		"justified" },
    { cc_CHAR_ORIENTATION_0,	"0 degrees" },
    { cc_CHAR_ORIENTATION_90,	"90 degrees" },
    { cc_CHAR_ORIENTATION_180,	"180 degrees" },
    { cc_CHAR_ORIENTATION_270,	"270 degrees" },
    { cc_CHARACTER_PATH_0,		"0 degrees" },
    { cc_CHARACTER_PATH_90,		"90 degrees" },
    { cc_CHARACTER_PATH_180,		"180 degrees" },
    { cc_CHARACTER_PATH_270,		"270 degrees" },
    { cc_ITEMISATION_no_itemisation,	"no itemisation" },
    { cc_ITEMISATION_start_aligned,	"start-aligned" },
    { cc_ITEMISATION_end_aligned,	"end-aligned" },
    { cc_LINE_LAYOUT_TABLE_start_alnd,	"start-aligned" },
    { cc_LINE_LAYOUT_TABLE_end_alnd,	"end-aligned" },
    { cc_LINE_LAYOUT_TABLE_centred,	"centred" },
    { cc_LINE_LAYOUT_TABLE_alnd_arnd,	"aligned-around" },
    { cc_LINE_PROGRESSION_90,		"90 degrees" },
    { cc_LINE_PROGRESSION_270,		"270 degrees" },
    { cc_PAIRWISE_KERNING_yes,		"yes" },
    { cc_PAIRWISE_KERNING_no,		"no" },
    { cc_FORMATTING_INDICATOR_yes,	"yes" },
    { cc_FORMATTING_INDICATOR_no,	"no" },
    { cc_PROP_LINE_SPACING_yes,		"yes" },
    { cc_PROP_LINE_SPACING_no,		"no" },
    { rg_LINE_PROGRESSION_90,		"90 degrees" },
    { rg_LINE_PROGRESSION_270,		"270 degrees" },
    { rg_PEL_PATH_0,			"0 degrees" },
    { rg_PEL_PATH_90,			"90 degrees" },
    { rg_PEL_PATH_180,			"180 degrees" },
    { rg_PEL_PATH_270,			"270 degrees" },
    { rg_PEL_TRANS_DENSITY_1,		"1 BMU" },
    { rg_PEL_TRANS_DENSITY_2,		"2 BMU" },
    { rg_PEL_TRANS_DENSITY_3,		"3 BMU" },
    { rg_PEL_TRANS_DENSITY_4,		"4 BMU" },
    { rg_PEL_TRANS_DENSITY_5,		"5 BMU" },
    { rg_PEL_TRANS_DENSITY_6,		"6 BMU" },
    { rg_IMAGE_DIMENSIONS_fixed,	"fixed" },
    { rg_IMAGE_DIMENSIONS_variable,	"variable" },
    { rg_COMPRESSION_compressed,	"compressed" },
    { rg_COMPRESSION_uncompressed,	"uncompressed" }

};

#define NVALUES	    ( (INT_type) (sizeof(values) / sizeof(values[0])))

CHAR_type *GetValueName(value)
    IN VALUE_type   value;
{
	     CHAR_type	*rval;
    REGISTER INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    for (i=0; i<NVALUES; i++) {
	if (values[i].value == value) {
	    rval = values[i].name;
	    goto LEAVE;
	}
    }

    rval = (CHAR_type *) 0;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WriteConstituentState(od, constituent)
    IN INT_type	    od;
    IN CONSTITUENT  constituent;
{
    INT_type	rval;	    /* The returned value */
    CHAR_type	*name;	    /* Name of constituent for output */

    INTERNAL_CONSTITUENT TheConstituent;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /* Update counter to indicate visited this time */
    TheConstituent -> debug_counter = last_debug_counter;

    /* Write constituent label */
    rval = IO_WriteChar(od, '\n');
    if (rval != ERROR_INT) {
	rval = DEBUG_WriteDebugConstituent(od, TheConstituent);
    }
    if (rval != ERROR_INT) {
	rval = IO_WriteChar(od, ' ');
    }

    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Write kind of constituent */
    if (CONST_IsDocProf(TheConstituent)) {
	name = "DOCUMENT PROFILE";
    } else if (CONST_IsLayoutStyle(TheConstituent)) {
	name = "LAYOUT STYLE";
    } else if (CONST_IsPresentStyle(TheConstituent)) {
	name = "PRESENTATION STYLE";
    } else if (CONST_IsCharContent(TheConstituent)) {
	name = "CONTENT PORTION (character)";
    } else if (CONST_IsRasterContent(TheConstituent)) {
	name = "CONTENT PORTION (raster graphics)";
    } else if (CONST_IsGeoContent(TheConstituent)) {
	name = "CONTENT PORTION (geometric graphics)";
    } else if (CONST_IsDVLPage(TheConstituent)) {
	name = "DEFAULT VALUE LIST (page)";
    } else if (CONST_IsDVLFrame(TheConstituent)) {
	name = "DEFAULT VALUE LIST (frame)";
    } else if (CONST_IsDVLBlock(TheConstituent)) {
	name = "DEFAULT VALUE LIST (block)";
    } else if (CONST_IsDVLComp(TheConstituent)) {
	name = "DEFAULT VALUE LIST (composite logical)";
    } else if (CONST_IsDVLBas(TheConstituent)) {
	name = "DEFAULT VALUE LIST (basic logical)";
    } else if (CONST_IsDAPDBlock(TheConstituent)) {
	name = "DOCUMENT APPLICATION PROFILE DEFAULTS (block)";
    } else if (CONST_IsDAPDBasLogical(TheConstituent)) {
	name = "DOCUMENT APPLICATION PROFILE DEFAULTS (baisc logical)";
    } else {	/* Must be a component */

	/* See if generic/specific */
	if (CONST_IsGeneric(TheConstituent)) {
	    rval = IO_Print(od, "GENERIC ");
	} else {
	    rval = IO_Print(od, "SPECIFIC ");
	}
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}

	/* See what kind */
	if (CONST_IsBasLog(TheConstituent)) {
	    name = "BASIC LOGICAL OBJECT";
	} else if (CONST_IsCompLog(TheConstituent)) {
	    name = "COMPOSITE LOGICAL OBJECT";
	} else if (CONST_IsDocLogRoot(TheConstituent)) {
	    name = "DOCUMENT LOGICAL ROOT";
	} else if (CONST_IsDocLayRoot(TheConstituent)) {
	    name = "DOCUMENT LAYOUT ROOT";
	} else if (CONST_IsPageSet(TheConstituent)) {
	    name = "PAGE SET";
	} else if (CONST_IsPage(TheConstituent)) {
	    name = "PAGE";
	} else if (CONST_IsFrame(TheConstituent)) {
	    name = "FRAME";
	} else if (CONST_IsBlock(TheConstituent)) {
	    name = "BLOCK";
	}
    }

    /* Write name & newline */
    rval = IO_PrintString(od, "%s\n", name);

    /* Have attributes written */
    if (rval != ERROR_INT) {
	rval = WriteAttributesState(od, TheConstituent);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WriteDocumentState(od, document)
    IN INT_type		od;
    IN DOCUMENT_type	document;
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	i;
    CONSTITUENT		profile;

    INTERNAL_DOCUMENT_type TheDocument;

    /*
       This next structure contains info about structures that may be written
       by a call to WriteSubgraphState.  BEWARE: order is significant!!!!!
    */
#define UNASSIGNED_CONSTITUENT ( (CONSTITUENT) 0)

    static struct {
	CHAR_type   *name;
	CONSTITUENT  root;
    } roots[] = {
	{ "Generic Logical Structure",	    UNASSIGNED_CONSTITUENT },
#define GEN_LOG_ROOT	((INT_type)0)
	{ "Specific Logical Structure",	    UNASSIGNED_CONSTITUENT },
#define SPE_LOG_ROOT	((INT_type)1)
	{ "Generic Layout Structure",	    UNASSIGNED_CONSTITUENT },
#define GEN_LAY_ROOT	((INT_type)2)
	{ "Specific Layout Structure",	    UNASSIGNED_CONSTITUENT }
#define SPE_LAY_ROOT	((INT_type)3)
    };

#define NROOTS	((INT_type)(sizeof(roots) / sizeof(roots[0])))

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheDocument = (INTERNAL_DOCUMENT_type) document;

    /* Generate new counter value */
    last_debug_counter++;

    /* Write start header info */
    rval = IO_PrintInt(od, "\nWriting Entire Document: #%d", TheDocument->id);
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\n=======================\n");
    }
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Just pass off work to other routines */
    rval = IO_Print(od, "\nDocument Profile:\n");
    if (rval != ERROR_INT) {
	profile = TheDocument->document_profile;
	if (profile == NULL_CONSTITUENT) {
	    rval = IO_Print(od, "<<no Document Profile>>\n");
	} else {
	    rval = WriteConstituentState(od, profile);
	}
    }
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Fill in root table */
    roots[GEN_LOG_ROOT].root = TheDocument->generic_logical_root;
    roots[SPE_LOG_ROOT].root = TheDocument->specific_logical_root;
    roots[GEN_LAY_ROOT].root = TheDocument->generic_layout_root;
    roots[SPE_LAY_ROOT].root = TheDocument->specific_layout_root;

    /* Loop & do it */
    for (i=0; i<NROOTS; i++) {
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	rval = IO_PrintString(od, "\n%s:\n", roots[i].name);
	if (rval != ERROR_INT) {
	    if (roots[i].root == NULL_CONSTITUENT) {
		rval = IO_PrintString(od, "<<no %s>>\n", roots[i].name);
	    } else {
		rval = InternalSubgraphState(od, roots[i].root);
	    }
	}
    }

    /* Do the styles */
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\nLayout Styles:\n");
	if (rval != ERROR_INT) {
	    if (TheDocument->layout_styles_list.forward == &TheDocument->layout_styles_list) {
		rval = IO_Print(od, "<<no Layout Styles>>\n");
	    } else {
		rval = WriteStylesState(od, document, BOOL_true);
	    }
	}
    }
    if (rval != ERROR_INT) {
	rval = IO_Print(od, "\nPresentation Styles:\n");
	if (rval != ERROR_INT) {
	    if (TheDocument->presentation_styles_list.forward ==
				    &TheDocument->presentation_styles_list) {
		rval = IO_Print(od, "<<no Presentation Styles>>\n");
	    } else {
		rval = WriteStylesState(od, document, BOOL_false);
	    }
	}
    }	    

    /* Need to pick up remaining constituents here */
    rval = WriteUnreachableConstituents(od, document, last_debug_counter);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteUnreachableConstituents(od, document, counter)
    IN INT_type		od;
    IN DOCUMENT_type	document;
    IN INT_type		counter;
{
    INT_type	rval;		/* The returned value */
    INTERNAL_CONSTITUENT	head;		/* Head of the constituent list */
    INTERNAL_CONSTITUENT TheConstituent;	/* The current constituent */
    BOOL_type	unreachable;
    INTERNAL_DOCUMENT_type TheDocument;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheDocument = (INTERNAL_DOCUMENT_type) document;

    rval = IO_Print(od, "\nUnreachable Constituents:\n");
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    head = &TheDocument -> constituent_list;
    unreachable = BOOL_false;
    for (TheConstituent=head->forward; TheConstituent!=head; TheConstituent=TheConstituent->forward) {
	if (TheConstituent->debug_counter != counter) {
	    unreachable = BOOL_true;
	    rval = WriteConstituentState(od, (CONSTITUENT) TheConstituent);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
    }

    /* Assume success, for now */
    rval = SUCCESS_INT;

    /* See if any were found */
    if (!unreachable) {
	rval = IO_Print(od, "<<no unreachable constituents>>\n");
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type WriteStylesState(od, document, islayout)
    IN INT_type		od;
    IN DOCUMENT_type	document;
    IN BOOL_type	islayout;
{
	     INT_type			rval;	/* The returned value */
    REGISTER INTERNAL_DOCUMENT_type	idocument;
    REGISTER INTERNAL_CONSTITUENT	style;
    REGISTER INTERNAL_CONSTITUENT	head;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    idocument = (INTERNAL_DOCUMENT_type) document;
    head = (islayout ? &idocument->layout_styles_list
		     : &idocument->presentation_styles_list);

    /* See if empty */
    if (head == head->forward) {
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    for (style=head->forward; style!=head; style=style->forward) {
	rval = WriteConstituentState(od, (CONSTITUENT) style);
	if (rval == ERROR_INT) {
	    goto LEAVE;
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

INT_type WriteSubgraphState(od, constituent)
    IN INT_type	    od;
    IN CONSTITUENT  constituent;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Generate new counter value */
    last_debug_counter++;

    /* Call internal routine to walk */
    rval = InternalSubgraphState(od, constituent);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type DEBUG_Final()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type DEBUG_Init()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Initialize debug counter */
    last_debug_counter = 1;

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteAttributesState(od, constituent)
    IN INT_type		    od;
    INTERNAL_CONSTITUENT    constituent;
{
	     INT_type		    rval;   /* The returned value */
    REGISTER INT_type		    i;	    /* Loop index */
    REGISTER INT_type		    index;  /* Attr type index */
	     ATTR_attribute_t	    attr;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* Loop & checkdeal with attributes according to the table */
    for (i=MIN_ATTR; rval!=ERROR_INT && i<MAX_ATTR; i++) {
	if (!CONST_UnsafeHasAttr(constituent, i)) {
	    continue;
	}
	attr = ATTR_FindAttr( (CONSTITUENT) constituent, i);
	if (attr == ERROR_ATTRIBUTE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = IO_PrintString(od, "    (\"%s\"", ATTR_AttrNameTbl[i].oda_name);
	if (rval != ERROR_INT) {
	    index = ATTR_IndexIntoTypes[i];
	    rval = (*ATTR_AttrTypeTbl[index].write_debug)(od, i, attr);
	}
	if (rval==ERROR_INT && TKError==UNIMPL_err) {
		rval = SUCCESS_INT;
	}
	if (rval != ERROR_INT) {
	    rval = IO_Print(od, ")\n");
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

static INT_type InternalSubgraphState(od, constituent)
    IN INT_type	    od;
    IN CONSTITUENT  constituent;
{
    INT_type		    rval;   /* The returned value */
    INTERNAL_CONSTITUENT    TheConstituent;
    INT_type		    dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /* Walk through children, if there are any */
    if (CONST_IsObjCl(TheConstituent)) {

	/* Generic object */
	rval = DEBUG_WalkGenericObject(od, TheConstituent);

    } else if (CONST_IsObjNc(TheConstituent)) {

	/* Specific object */
	rval = WalkSpecificObject(od, TheConstituent);
    } else if (TheConstituent->what_kind == EMPTY) {

	/* Specific object */
	TKERR_StartErrMsg();
	dummy =
            TKERR_Format("[InternalSubgraphState]: Blindly following EMPTY");
	TKERR_EndErrMsg();
	rval = WalkSpecificObject(od, TheConstituent);

    } else {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	TKERR_StartErrMsg();
	dummy =
	    TKERR_FormatInt("[InternalSubgraphState]: Impossible constituent (%d)",
			    (INT_type) TheConstituent->what_kind);
	TKERR_EndErrMsg();
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type WalkSpecificObject(od, constituent)
    IN INT_type		    od;
    IN INTERNAL_CONSTITUENT constituent;
{
    REGISTER INT_type			rval;	    /* The return value */
    REGISTER INT_type			i;	    /* Loop index */
	     ATTR_attribute_t		attr;	    /* Pointer to attribute */
	     SEQUENCE_CONSTITUENT_type	children;   /* Sequence of children */
    REGISTER BOOL_type			contents;
    REGISTER CONSTITUENT		child;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = WriteConstituentState(od, (CONSTITUENT) constituent);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    /* Set children to sequence of its children */
    children = NULL_SEQUENCE;
    if (CONST_IsBasLogNc(constituent)) {

	/* It's a Basic Logical Object, look for "Content Portions" */
	if (CONST_UnsafeHasAttr(constituent, at_CONTENT_PORTIONS)) {
	    attr = ATTR_FindAttr((CONSTITUENT) constituent, at_CONTENT_PORTIONS);
	    if (attr == ERROR_ATTRIBUTE) {
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    children = GET_SEQUENCE_CONSTITUENT(attr);
	    contents = BOOL_true;
	}

    /* It's not a Basic Logical Object, look for "Subordinates" */
    } else {
	if (CONST_UnsafeHasAttr(constituent, at_SUBORDINATES)) {
	    attr = ATTR_FindAttr((CONSTITUENT) constituent, at_SUBORDINATES);
	    if (attr == ERROR_ATTRIBUTE) {
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    children = GET_SEQUENCE_CONSTITUENT(attr);
	    contents = BOOL_false;
	}
    }

    /* Now, just walk through the kids */
    if (children != NULL_SEQUENCE) {
	for (i=0; rval!=ERROR_INT && i<children->length; i++) {
	    child = children->sequence_value.constituents[i];
	    if (contents) {
		rval = WriteConstituentState(od, child);
	    } else {
		rval = InternalSubgraphState(od, child);
	    }					 
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

/*
   Write out debugging info for an attribute that is a
    sequence of constituents
*/

INT_type SEQUENCE_CONSTITUENT_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    REGISTER INT_type		rval;
    SEQUENCE_CONSTITUENT_type	constituents;
    REGISTER INT_type		length;
    REGISTER INT_type		i;

    INTERNAL_CONSTITUENT	this;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;
    constituents = GET_SEQUENCE_CONSTITUENT(attr);
    length = constituents->length;
    for (i=0; rval!=ERROR_INT && i<length; i++) {
	this =
	    (INTERNAL_CONSTITUENT) constituents -> sequence_value.constituents[i];
	rval = IO_WriteChar(od, ' ');
	if (rval != ERROR_INT) {
	    rval = DEBUG_WriteDebugConstituent(od, this);
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

INT_type STRING_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    REGISTER INT_type	    rval;	/* Return value */
	     STRING_type    str;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    str = GET_STRING(attr);    

    /* Should check for user-specified output filter routine */

    rval = WriteDebugString(od, str->sequence_value.chars, str->length);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugString(od, chars, length)
    REGISTER INT_type	od;
	     CHAR_type	*chars;
    REGISTER INT_type	length;
{
	     INT_type	    rval;
    REGISTER INT_type	    i;	    /* Loop index */
    REGISTER CHAR_type	    *c;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = IO_Print(od, " \"");

    for (i=0, c=chars; rval!=ERROR_INT && i<length; i++, c++) {
	rval = WriteDebugByte(od, (BYTE_type) *c);
    }

    /* Write closing ", if all okay */
    if (rval != ERROR_INT) {
	rval = IO_WriteChar(od, '"');
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type VALUE_WDbug(od, attrid, attr)
    REGISTER INT_type		od;
	     INT_type		attrid;
	     ATTR_attribute_t	attr;
{
    INT_type	rval;
    VALUE_type	value;
    CHAR_type	*name;

#ifdef DEBUG
    if(TraceRoutine()){
	Entering();
    }
#endif

    value = GET_VALUE(attr);

    name = GetValueName(value);
    if (name == (CHAR_type *) 0) {
	rval = ERROR_INT;
	goto LEAVE;
    }
    rval = IO_PrintString(od, " '%s'", name);

LEAVE:
#ifdef DEBUG
    if(TraceRoutine()){
	Leaving();
    }
#endif

    return rval;
}

INT_type CONSTITUENT_WDbug(od, attrid, attr)
    REGISTER INT_type		od;
	     INT_type		attrid;
	     ATTR_attribute_t	attr;
{
    INT_type		    rval;
    INTERNAL_CONSTITUENT    value;

#ifdef DEBUG
    if(TraceRoutine()){
	Entering();
    }
#endif

    value = (INTERNAL_CONSTITUENT) GET_CONSTITUENT(attr);

    rval = IO_WriteChar(od, ' ');
    if (rval != ERROR_INT) {
	rval = DEBUG_WriteDebugConstituent(od, value);
    }

LEAVE:
#ifdef DEBUG
    if(TraceRoutine()){
	Leaving();
    }
#endif

    return rval;
}

INT_type DEBUG_WriteDebugConstituent(od, constituent)
    INT_type		    od;
    INTERNAL_CONSTITUENT    constituent;
{
    INT_type	rval;

#ifdef DEBUG
    if(TraceRoutine()){
	Entering();
    }
#endif

    rval = IO_PrintInt(od, "%d", constituent->document->id);
    if (rval != ERROR_INT) {
	rval = IO_PrintInt(od, "/%d:", constituent->id);
    }

LEAVE:
#ifdef DEBUG
    if(TraceRoutine()){
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQUENCE_STRING_WDbug(od, attrid, attr)
    REGISTER INT_type		od;
	     INT_type		attrid;
	     ATTR_attribute_t	attr;
{
	     INT_type		    rval;
    REGISTER INT_type		    length;
	     SEQUENCE_STRING_type   value;
    REGISTER INT_type		    i;
	     STRING_type	    *strings;

#ifdef DEBUG
    if(TraceRoutine()){
	Entering();
    }
#endif

    value = GET_SEQUENCE_STRING(attr);

    length = value -> length;
    strings = value->sequence_value.strings;
    rval = SUCCESS_INT;
    for (i=0; rval!=ERROR_INT && i<length; i++) {
	if (i != 0) {
	    rval = IO_WriteChar(od, ' ');
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
	rval = WriteDebugString(od, strings[i]->sequence_value.chars,
				    strings[i]->length);
    }

LEAVE:
#ifdef DEBUG
    if(TraceRoutine()){
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQUENCE_VALUE_WDbug(od, attrid, attr)
    REGISTER INT_type		od;
	     INT_type		attrid;
	     ATTR_attribute_t	attr;
{
	     INT_type		    rval;
    REGISTER INT_type		    length;
	     SEQUENCE_VALUE_type    value;
    REGISTER INT_type		    i;
	     VALUE_type		    *values;
	     CHAR_type		    *name;

#ifdef DEBUG
    if(TraceRoutine()){
	Entering();
    }
#endif

    value = GET_SEQUENCE_VALUE(attr);

    length = value -> length;
    values = value->sequence_value.values;
    rval = SUCCESS_INT;
    for (i=0; rval!=ERROR_INT && i<length; i++) {
	if (i != 0) {
	    rval = IO_WriteChar(od, ' ');
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }
	}
	name = GetValueName(values[i]);
	if (name == (CHAR_type *) 0) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = IO_PrintString(od, "'%s'", name);
    }

LEAVE:
#ifdef DEBUG
    if(TraceRoutine()){
	Leaving();
    }
#endif

    return rval;
}

INT_type SEQUENCE_BYTE_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    REGISTER INT_type		   rval;	/* Return value */
	     SEQUENCE_BYTE_type    str;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    str = GET_SEQUENCE_BYTE(attr);    

    /* Should check for user-specified output filter routine */

    rval = DEBUG_WriteBytes(od, str->sequence_value.bytes, str->length);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LAYOUT_OBJECT_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type		rval;
    LAYOUT_OBJECT_type	*value;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Extract the value */
    value = GET_LAYOUT_OBJECT(attr);

    /* See what kind it is */
    switch (value->layout_object_tag) {
	case LAYOUT_OBJ_DESC_tag:
		rval = IO_WriteChar(od, ' ');
		if (rval != ERROR_INT) {
		    rval = DEBUG_WriteDebugConstituent(od,
		(INTERNAL_CONSTITUENT) value->layout_object_value.constituent);
		}
		break;
	case LAYOUT_CATEGORY_tag:
		rval =
		    WriteDebugString(od,
		value->layout_object_value.layout_category->sequence_value.chars,
		value->layout_object_value.layout_category->length);
		break;
	case OBJECT_TYPE_PAGE_tag:
		rval = IO_Print(od, " 'object type page'");
		break;
	case NULL_tag:
		rval = IO_Print(od, " 'null'");
		break;
	default:
		TKError = INTERNAL_err;
		rval = TKERR_StartErrMsg();
		rval = TKERR_FormatInt(
       "[LAYOUT_OBJECT_WDbug]: Impossible value for layout_object_tag (%d)",
				       (INT_type) value->layout_object_tag);
		rval = TKERR_EndErrMsg();
		rval = ERROR_INT;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type DEBUG_WriteBytes(od, bytes, length)
    REGISTER INT_type	od;
	     BYTE_type	*bytes;
	     INT_type	length;
{
	     INT_type	rval;
    REGISTER BYTE_type	*c;
    REGISTER INT_type	i;
    REGISTER INT_type	maxlen;

#ifdef DEBUG
    if (TraceRoutine()){
	Entering();
    }
#endif

    maxlen = (length > MAX_BYTES_OUT ? MAX_BYTES_OUT : length);

    rval = IO_Print(od, " `");

    for (i=0, c=bytes; rval!=ERROR_INT && i<maxlen; i++, c++) {
	rval = WriteDebugByte(od, *c);
    }

    /* Write closing `, if all okay */
    if (rval != ERROR_INT) {
	rval = IO_WriteChar(od, '`');
    }

    /* Show if bytes left off */
    if (rval!=ERROR_INT && length>MAX_BYTES_OUT) {
	rval = IO_Print(od, " ...");
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type WriteDebugByte(od, byte)
    REGISTER INT_type	od;
	     BYTE_type	byte;
{
	     INT_type	rval;
    REGISTER CHAR_type	c;

#ifdef DEBUG
    if (TraceRoutine()){
	Entering();
    }
#endif

    c = (CHAR_type) byte;
    if (c < ' ' || c > '~') {
	/* WARNING:  ASCII only */
	rval = IO_PrintInt(od, "\\%d", (INT_type) ((byte&0300) >> 6));
	if (rval != ERROR_INT) {
	    rval = IO_PrintInt(od, "%d", (INT_type) ((byte&070) >> 3));
	}
	if (rval != ERROR_INT) {
	    rval = IO_PrintInt(od, "%d", (INT_type) (byte&07));
	}
    } else if (c == '\\' || c == '"') {
	rval = IO_WriteChar(od, '\\');
	if (rval != ERROR_INT) {
	    rval = IO_WriteChar(od, c);
	}
    } else {
	rval = IO_WriteChar(od, c);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type at_SEPARATION_WDbug(od, attrid, attr)
    IN INT_type		    od;
    IN INT_type		    attrid;
    IN ATTR_attribute_t	    attr;
{
    INT_type		rval;
    at_SEPARATION_type	*value;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Extract the value */
    value = GET_at_SEPARATION(attr);

    if ((attr->parm_mask & LEADING_EDGE_parm) != 0) {
	rval = IO_PrintInt(od, " (\"leading edge\" %d)", value->leading_edge);
    }
    if (rval!=ERROR_INT && (attr->parm_mask & TRAILING_EDGE_parm) != 0) {
	rval = IO_PrintInt(od, " (\"trailing edge\" %d)", value->trailing_edge);
    }
    if ((attr->parm_mask & CENTRE_SEPARATION_parm) != 0) {
	rval = IO_PrintInt(od, " (\"centre separation\" %d)",
			   value->centre_separation);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}
