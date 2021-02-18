
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
 CMU-ITC-071) for full text & permitted uses.


	Include file defining branch table mapping

	attribute identifier values into aunion.h types

    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.



    WARNING: Scripts for generating this file
	    assume that attribute ids are generated
	    with the attribute id names in case
	    insensitive alphabetic order. If this
	    not the case then THIS TABLE IS UGH !!!
*/


#ifndef TK_INCLUDED_ATTR0
#define TK_INCLUDED_ATTR0


#undef TK_ATTR
#ifdef TK_TKI_ATTR
#define TK_ATTR
#else
#ifdef TK_IMPL_ATTR
#define TK_ATTR
#else
#define AP_ATTR
#endif
#endif


#ifdef EXTERN
#undef EXTERN

#endif
#ifdef TK_ATTR


#include <rnames.h>


/*
	The following defines give an indexing of
	aunion.h types
*/


#define at_BORDER_const	((INT_type)0)
#define at_DEFAULT_VALUE_LISTS_const	((INT_type)1)
#define at_DIMENSIONS_const	((INT_type)2)
#define at_MEDIUM_TYPE_const	((INT_type)3)
#define at_OFFSET_const	((INT_type)4)
#define at_POSITION_const	((INT_type)5)
#define at_SAME_LAYOUT_OBJECT_const	((INT_type)6)
#define at_SEPARATION_const	((INT_type)7)
#define cc_ITEMISATION_const	((INT_type)8)
#define CONSTITUENT_const	((INT_type)9)
#define CONSTRUCTION_EXPR_const	((INT_type)10)
#define dp_DOC_APP_PROF_DEFAULTS_const	((INT_type)11)
#define dp_LOCAL_FILE_REFERENCE_const	((INT_type)12)
#define dp_ODA_VERSION_const	((INT_type)13)
#define gg_COLOUR_REPS_const	((INT_type)14)
#define gg_EDGE_RENDITION_const	((INT_type)15)
#define gg_ENCODING_ANNOUNCER_const	((INT_type)16)
#define gg_FILLED_AREA_REND_const	((INT_type)17)
#define gg_LINE_RENDITION_const	((INT_type)18)
#define gg_MARKER_RENDITION_const	((INT_type)19)
#define gg_PICTURE_DIMENSIONS_const	((INT_type)20)
#define gg_REGION_INTEREST_SPEC_const	((INT_type)21)
#define gg_TEXT_RENDITION_const	((INT_type) 22)
#define gg_TRANSFORMATION_SPEC_const	((INT_type) 23)
#define gg_TRANSPARENCY_SPEC_const	((INT_type)24)
#define INT_const	((INT_type)25)
#define INT_PAIR_const	((INT_type)26)
#define LAYOUT_OBJECT_const	((INT_type)27)
#define rg_CLIPPING_const	((INT_type)28)
#define rg_IMAGE_DIMENSIONS_const	((INT_type)29)
#define rg_PEL_SPACING_const	((INT_type)30)
#define SEQUENCE_ASN1_OBJECT_ID_const	((INT_type)31)
#define SEQUENCE_at_MEDIUM_TYPE_const	((INT_type)32)
#define SEQUENCE_BINDING_PAIR_const	((INT_type)33)
#define SEQUENCE_BYTE_const	((INT_type)34)
#define SEQUENCE_CONSTITUENT_const	((INT_type)35)
#define SEQUENCE_COPYRIGHT_INFO_const	((INT_type)36)
#define SEQUENCE_FONT_PAIR_const	((INT_type)37)
#define SEQUENCE_INT_const	((INT_type)38)
#define SEQUENCE_INT_PAIR_const	((INT_type)39)
#define SEQUENCE_NAME_ORG_const	((INT_type)40)
#define SEQUENCE_RESOURCE_MAP_const	((INT_type)41)
#define SEQUENCE_REVISION_HISTORY_const	((INT_type)42)
#define SEQUENCE_STRING_const	((INT_type)43)
#define SEQUENCE_STRING_OR_ID_const	((INT_type)44)
#define SEQUENCE_TAB_STOP_const	((INT_type)45)
#define SEQUENCE_VALUE_const	((INT_type)46)
#define STRING_const	((INT_type)47)
#define STRING_EXPR_const	((INT_type)48)
#define STRING_OR_ID_const	((INT_type)49)
#define VALUE_const	((INT_type)50)


/*
	This branch table maps attribute ids into
	the aunion.h operation table named
	ATTR_AttrTypeTbl which appears below.
*/

/* real name for the attr index table */

#ifndef USE_LONG_NAMES
#define ATTR_IndexIntoTypes    gatiit

#endif


#ifdef TK_GVARS_ATTR16
#define EXTERN
#else
#define EXTERN extern
#endif




EXTERN INT_type ATTR_IndexIntoTypes[]

#ifdef TK_GVARS_ATTR16
= {

	0,
	STRING_const,
	SEQUENCE_BYTE_const,
	SEQUENCE_CONSTITUENT_const,
	SEQUENCE_BINDING_PAIR_const,
	VALUE_const,
	at_BORDER_const,
	VALUE_const,
	VALUE_const,
	SEQUENCE_INT_const,
	STRING_EXPR_const,
	STRING_const,
	STRING_const,
	SEQUENCE_BYTE_const,
	SEQUENCE_CONSTITUENT_const,
	VALUE_const,
	at_DEFAULT_VALUE_LISTS_const,
	at_DIMENSIONS_const,
	VALUE_const,
	CONSTRUCTION_EXPR_const,
	SEQUENCE_CONSTITUENT_const,
	LAYOUT_OBJECT_const,
	STRING_const,
	CONSTITUENT_const,
	VALUE_const,
	CONSTITUENT_const,
	STRING_const,
	CONSTITUENT_const,
	at_MEDIUM_TYPE_const,
	LAYOUT_OBJECT_const,
	CONSTITUENT_const,
	STRING_const,
	STRING_const,
	VALUE_const,
	at_OFFSET_const,
	INT_PAIR_const,
	SEQUENCE_STRING_const,
	at_POSITION_const,
	CONSTITUENT_const,
	STRING_const,
	VALUE_const,
	STRING_const,
	at_SAME_LAYOUT_OBJECT_const,
	at_SEPARATION_const,
	SEQUENCE_CONSTITUENT_const,
	CONSTITUENT_const,
	VALUE_const,
	SEQUENCE_INT_const,
	STRING_const,
	STRING_const,
	INT_const,
	VALUE_const,
	VALUE_const,
	SEQUENCE_INT_PAIR_const,
	VALUE_const,
	INT_const,
	SEQUENCE_BYTE_const,
	VALUE_const,
	INT_const,
	SEQUENCE_BYTE_const,
	SEQUENCE_INT_const,
	INT_const,
	INT_PAIR_const,
	cc_ITEMISATION_const,
	INT_PAIR_const,
	SEQUENCE_TAB_STOP_const,
	VALUE_const,
	INT_const,
	INT_const,
	VALUE_const,
	VALUE_const,
	INT_const,
	STRING_const,
	STRING_const,
	SEQUENCE_BYTE_const,
	SEQUENCE_BYTE_const,
	STRING_const,
	SEQUENCE_NAME_ORG_const,
	SEQUENCE_VALUE_const,
	SEQUENCE_CONSTITUENT_const,
	SEQUENCE_VALUE_const,
	SEQUENCE_BYTE_const,
	SEQUENCE_ASN1_OBJECT_ID_const,
	SEQUENCE_COPYRIGHT_INFO_const,
	STRING_const,
	SEQUENCE_NAME_ORG_const,
	dp_DOC_APP_PROF_DEFAULTS_const,
	SEQUENCE_INT_const,
	VALUE_const,
	STRING_const,
	STRING_OR_ID_const,
	INT_const,
	STRING_const,
	STRING_const,
	SEQUENCE_INT_const,
	SEQUENCE_VALUE_const,
	SEQUENCE_FONT_PAIR_const,
	VALUE_const,
	VALUE_const,
	VALUE_const,
	SEQUENCE_STRING_const,
	SEQUENCE_VALUE_const,
	VALUE_const,
	dp_LOCAL_FILE_REFERENCE_const,
	STRING_const,
	SEQUENCE_at_MEDIUM_TYPE_const,
	INT_const,
	INT_const,
	dp_ODA_VERSION_const,
	SEQUENCE_STRING_const,
	SEQUENCE_NAME_ORG_const,
	SEQUENCE_INT_PAIR_const,
	SEQUENCE_INT_PAIR_const,
	SEQUENCE_NAME_ORG_const,
	SEQUENCE_CONSTITUENT_const,
	VALUE_const,
	SEQUENCE_BYTE_const,
	SEQUENCE_VALUE_const,
	STRING_const,
	SEQUENCE_STRING_OR_ID_const,
	STRING_const,
	STRING_OR_ID_const,
	SEQUENCE_RESOURCE_MAP_const,
	SEQUENCE_REVISION_HISTORY_const,
	STRING_const,
	VALUE_const,
	VALUE_const,
	STRING_const,
	STRING_const,
	STRING_const,
	SEQUENCE_STRING_OR_ID_const,
	STRING_const,
	SEQUENCE_VALUE_const,
	SEQUENCE_ASN1_OBJECT_ID_const,
	INT_PAIR_const,
	SEQUENCE_STRING_const,
	gg_COLOUR_REPS_const,
	gg_EDGE_RENDITION_const,
	gg_ENCODING_ANNOUNCER_const,
	gg_FILLED_AREA_REND_const,
	gg_LINE_RENDITION_const,
	gg_MARKER_RENDITION_const,
	gg_PICTURE_DIMENSIONS_const,
	VALUE_const,
	gg_REGION_INTEREST_SPEC_const,
	gg_TEXT_RENDITION_const,
	gg_TRANSFORMATION_SPEC_const,
	gg_TRANSPARENCY_SPEC_const,
	rg_CLIPPING_const,
	VALUE_const,
	rg_IMAGE_DIMENSIONS_const,
	INT_PAIR_const,
	VALUE_const,
	INT_const,
	INT_const,
	INT_const,
	VALUE_const,
	rg_PEL_SPACING_const,
	VALUE_const,
	INT_PAIR_const
}
#endif
;
/* above endif closes ifdef on GVARS */
#undef EXTERN
/*
	This table contains the attribute id #define'd
	names in C string form and a C string version of
	the attribute names as they appear in the IS
*/


/* real name for the attr name table */

#ifndef USE_LONG_NAMES
#define ATTR_AttrNameTbl	gatant

#endif


#ifdef TK_GVARS_ATTR17
#define EXTERN
#else
#define EXTERN extern
#endif


EXTERN struct ATTR_AttrNameTbl_struct {

	CHAR_type *attrid_name;

	CHAR_type *oda_name;

} ATTR_AttrNameTbl[]

#ifdef TK_GVARS_ATTR17
= {

	{"Null Attr Name", "Null ODA Name"},
	{"at_ALT_REP", "Alternative Representation"},
	{"at_APPLICATION_COMMENTS", "Application Comments"},
	{"at_BALANCE", "Balance"},
	{"at_BINDINGS", "Bindings"},
	{"at_BLK_ALGNM", "Block Alignment"},
	{"at_BORDER", "Border"},
	{"at_COLOUR", "Colour"},
	{"at_CONCATENATION", "Concatenation"},
	{"at_CONTENT_ARCH_CLASS", "Content Architecture Class"},
	{"at_CONTENT_GENERATOR", "Content Generator"},
	{"at_CONTENT_ID_LAYOUT", "Content Identifier - Layout"},
	{"at_CONTENT_ID_LOGICAL", "Content Identifier - Logical"},
	{"at_CONTENT_INFORMATION", "Content Information"},
	{"at_CONTENT_PORTIONS", "Content Portions"},
	{"at_CONTENT_TYPE", "Content Type"},
	{"at_DEFAULT_VALUE_LISTS", "Default Value Lists"},
	{"at_DIMENSIONS", "Dimensions"},
	{"at_FILL_ORDER", "Fill Order"},
	{"at_GEN_FOR_SUBORDINATES", "Generator for Subordinates"},
	{"at_IMAGING_ORDER", "Imaging Order"},
	{"at_INDIVISIBILITY", "Indivisibility"},
	{"at_LAYOUT_CATEGORY", "Layout Category"},
	{"at_LAYOUT_OBJECT_CLASS", "Layout Object Class"},
	{"at_LAYOUT_PATH", "Layout Path"},
	{"at_LAYOUT_STYLE", "Layout Style"},
	{"at_LAYOUT_STYLE_IDENTIFIER", "Layout Style Identifier"},
	{"at_LOGICAL_SOURCE", "Logical Source"},
	{"at_MEDIUM_TYPE", "Medium Type"},
	{"at_NEW_LAYOUT_OBJECT", "New Layout Object"},
	{"at_OBJECT_CLASS", "Object Class"},
	{"at_OBJECT_CLASS_IDENTIFIER", "Object Class Identifier"},
	{"at_OBJECT_IDENTIFIER", "Object Identifier"},
	{"at_OBJECT_TYPE", "Object Type"},
	{"at_OFFSET", "Offset"},
	{"at_PAGE_POSITION", "Page Position"},
	{"at_PERMITTED_CATEGORIES", "Permitted Categories"},
	{"at_POSITION", "Position"},
	{"at_PRESENTATION_STYLE", "Presentation Style"},
	{"at_PRESENTATION_STYLE_ID", "Presentation Style Identifier"},
	{"at_PROTECTION", "Protection"},
	{"at_RESOURCE", "Resource"},
	{"at_SAME_LAYOUT_OBJECT", "Same Layout Object"},
	{"at_SEPARATION", "Separation"},
	{"at_SUBORDINATES", "Subordinates"},
	{"at_SYNCHRONIZATION", "Synchronization"},
	{"at_TRANSPARENCY", "Transparency"},
	{"at_TYPE_OF_CODING", "Type of Coding"},
	{"at_USER_READABLE_COMMENTS", "User-readable Comments"},
	{"at_USER_VISIBLE_NAME", "User-visible Name"},
	{"cc_1ST_LINE_OFFSET", "First Line Offset"},
	{"cc_ALIGNMENT", "Alignment"},
	{"cc_CHAR_ORIENTATION", "Character Orientation"},
	{"cc_CHARACTER_FONTS", "Character Fonts"},
	{"cc_CHARACTER_PATH", "Character Path"},
	{"cc_CHARACTER_SPACING", "Character Spacing"},
	{"cc_CODE_EXT_ANNOUNCERS", "Code Extension Announcers"},
	{"cc_FORMATTING_INDICATOR", "Formatting Indicator"},
	{"cc_GRAPHIC_CHAR_SUBREP", "Graphic Character Subrepertoire"},
	{"cc_GRAPHIC_CHARACTER_SETS", "Graphic Character Sets"},
	{"cc_GRAPHIC_RENDITION", "Graphic Rendition"},
	{"cc_INDENTATION", "Indentation"},
	{"cc_INITIAL_OFFSET", "Initial Offset"},
	{"cc_ITEMISATION", "Itemisation"},
	{"cc_KERNING_OFFSET", "Kerning Offset"},
	{"cc_LINE_LAYOUT_TABLE", "Line Layout Table"},
	{"cc_LINE_PROGRESSION", "Line Progression"},
	{"cc_LINE_SPACING", "Line Spacing"},
	{"cc_ORPHAN_SIZE", "Orphan Size"},
	{"cc_PAIRWISE_KERNING", "Pairwise Kerning"},
	{"cc_PROP_LINE_SPACING", "Proportional Line Spacing"},
	{"cc_WIDOW_SIZE", "Widow Size"},
	{"dp_ABSTRACT", "Abstract"},
	{"dp_ACCESS_RIGHTS", "Access Rights"},
	{"dp_ADDITIONAL_INFORMATION", "Additional Information"},
	{"dp_ALT_REP_CHARACTER_SETS", "Alternative Representation Character Sets"},
	{"dp_AUTHORIZATION", "Authorization"},
	{"dp_AUTHORS", "Authors"},
	{"dp_BLOCK_ALIGNMENTS", "Block Alignments"},
	{"dp_CODING_ATTRIBUTES", "Coding Attributes"},
	{"dp_COLOURS", "Colours"},
	{"dp_COMMENTS_CHARACTER_SETS", "Comments Character Sets"},
	{"dp_CONTENT_ARCH_CLASSES", "Content Architecture Classes"},
	{"dp_COPYRIGHT", "Copyright"},
	{"dp_CREATION_DATE_AND_TIME", "Creation Date and Time"},
	{"dp_DISTRIBUTION_LIST", "Distribution List"},
	{"dp_DOC_APP_PROF_DEFAULTS", "Document Application Profile Defaults"},
	{"dp_DOC_APP_PROFILE", "Document Application Profile"},
	{"dp_DOC_ARCH_CLASS", "Document Architecture Class"},
	{"dp_DOCUMENT_DATE_AND_TIME", "Document Date and Time"},
	{"dp_DOCUMENT_REFERENCE", "Document Reference"},
	{"dp_DOCUMENT_SIZE", "Document Size"},
	{"dp_DOCUMENT_TYPE", "Document Type"},
	{"dp_EXPIRY_DATE_AND_TIME", "Expiry Date and Time"},
	{"dp_EXTERNAL_DOCUMENT_CLASS", "External Document Class"},
	{"dp_FILL_ORDERS", "Fill Orders"},
	{"dp_FONTS_LIST", "Fonts List"},
	{"dp_GENERIC_LAY_STRUCT", "Generic Layout Structure"},
	{"dp_GENERIC_LOG_STRUCT", "Generic Logical Structure"},
	{"dp_INTERCHANGE_FMT_CLASS", "Interchange Format Class"},
	{"dp_LANGUAGES", "Languages"},
	{"dp_LAYOUT_PATHS", "Layout Paths"},
	{"dp_LAYOUT_STYLES", "Layout Styles"},
	{"dp_LOCAL_FILE_REFERENCE", "Local File Reference"},
	{"dp_LOCAL_FILING_DATE_TIME", "Local Filing Date and Time"},
	{"dp_MEDIUM_TYPES", "Medium Types"},
	{"dp_NUMBER_OBJECTS_PER_PAGE", "Number of Objects Per Page"},
	{"dp_NUMBER_OF_PAGES", "Number of Pages"},
	{"dp_ODA_VERSION", "ODA Version"},
	{"dp_ORGANIZATIONS", "Organizations"},
	{"dp_OWNERS", "Owners"},
	{"dp_PAGE_DIMENSIONS", "Page Dimensions"},
	{"dp_PAGE_POSITIONS", "Page Positions"},
	{"dp_PREPARERS", "Preparers"},
	{"dp_PRESENTATION_FEATURES", "Presentation Features"},
	{"dp_PRESENTATION_STYLES", "Presentation Styles"},
	{"dp_PROFILE_CHARACTER_SETS", "Profile Character Sets"},
	{"dp_PROTECTIONS", "Protections"},
	{"dp_PURGE_DATE_AND_TIME", "Purge Date and Time"},
	{"dp_REFS_TO_OTHER_DOCS", "References to Other Documents"},
	{"dp_RELEASE_DATE_AND_TIME", "Release Date and Time"},
	{"dp_RESOURCE_DOCUMENT", "Resource Document"},
	{"dp_RESOURCES", "Resources"},
	{"dp_REVISION_HISTORY", "Revision History"},
	{"dp_SECURITY_CLASSIFICATION", "Security Classification"},
	{"dp_SPECIFIC_LAY_STRUCT", "Specific Layout Structure"},
	{"dp_SPECIFIC_LOG_STRUCT", "Specific Logical Structure"},
	{"dp_START_DATE_AND_TIME", "Start Date and Time"},
	{"dp_STATUS", "Status"},
	{"dp_SUBJECT", "Subject"},
	{"dp_SUPERSEDED_DOCUMENTS", "Superseded Documents"},
	{"dp_TITLE", "Title"},
	{"dp_TRANSPARENCIES", "Transparencies"},
	{"dp_TYPES_OF_CODING", "Types of Coding"},
	{"dp_UNIT_SCALING", "Unit Scaling"},
	{"dp_USER_SPECIFIC_CODES", "User Specific Codes"},
	{"gg_COLOUR_REPS", "Colour Representations"},
	{"gg_EDGE_RENDITION", "Edge Rendition"},
	{"gg_ENCODING_ANNOUNCER", "Geometric Graphics Encoding Announcer"},
	{"gg_FILLED_AREA_REND", "Filled Area Rendition"},
	{"gg_LINE_RENDITION", "Line Rendition"},
	{"gg_MARKER_RENDITION", "Marker Rendition"},
	{"gg_PICTURE_DIMENSIONS", "Picture Dimensions"},
	{"gg_PICTURE_ORIENTATION", "Picture Orientation"},
	{"gg_REGION_INTEREST_SPEC", "Region of Interest Specification"},
	{"gg_TEXT_RENDITION", "Text Rendition"},
	{"gg_TRANSFORMATION_SPEC", "Transformation Specification"},
	{"gg_TRANSPARENCY_SPEC", "Transparency Specification"},
	{"rg_CLIPPING", "Clipping"},
	{"rg_COMPRESSION", "Compression"},
	{"rg_IMAGE_DIMENSIONS", "Image Dimensions"},
	{"rg_INITIAL_OFFSET", "Initial Offset"},
	{"rg_LINE_PROGRESSION", "Line Progression"},
	{"rg_NUMBER_DISCARDED_PELS", "Number of Discarded Pels"},
	{"rg_NUMBER_OF_LINES", "Number of Lines"},
	{"rg_NUMBER_OF_PELS_PER_LINE", "Number of Pels Per Line"},
	{"rg_PEL_PATH", "Pel Path"},
	{"rg_PEL_SPACING", "Pel Spacing"},
	{"rg_PEL_TRANS_DENSITY", "Pel Transmission Density"},
	{"rg_SPACING_RATIO", "Spacing Ratio"}
}
#endif
;
/* endif close ifdef on GVARS */


#undef EXTERN
/*
	Next come the forward declarations for all the
	canonical procedures for operation on aunion.h
	types.
*/


extern INT_type at_BORDER_WDbug();
extern INT_type at_BORDER_Copy();
extern INT_type at_BORDER_CkVal();
extern POINTER_type at_BORDER_Alloc();
extern INT_type at_BORDER_Free();
extern INT_type at_BORDER_Set();
extern POINTER_type at_BORDER_Get();
extern INT_type at_BORDER_Deflt();
extern INT_type at_DEFAULT_VALUE_LISTS_WDbug();
extern INT_type at_DEFAULT_VALUE_LISTS_Copy();
extern INT_type at_DEFAULT_VALUE_LISTS_CkVal();
extern POINTER_type at_DEFAULT_VALUE_LISTS_Alloc();
extern INT_type at_DEFAULT_VALUE_LISTS_Free();
extern INT_type at_DEFAULT_VALUE_LISTS_Set();
extern POINTER_type at_DEFAULT_VALUE_LISTS_Get();
extern INT_type at_DEFAULT_VALUE_LISTS_Deflt();
extern INT_type at_DIMENSIONS_WDbug();
extern INT_type at_DIMENSIONS_Copy();
extern INT_type at_DIMENSIONS_CkVal();
extern POINTER_type at_DIMENSIONS_Alloc();
extern INT_type at_DIMENSIONS_Free();
extern INT_type at_DIMENSIONS_Set();
extern POINTER_type at_DIMENSIONS_Get();
extern INT_type at_DIMENSIONS_Deflt();
extern INT_type at_MEDIUM_TYPE_WDbug();
extern INT_type at_MEDIUM_TYPE_Copy();
extern INT_type at_MEDIUM_TYPE_CkVal();
extern POINTER_type at_MEDIUM_TYPE_Alloc();
extern INT_type at_MEDIUM_TYPE_Free();
extern INT_type at_MEDIUM_TYPE_Set();
extern POINTER_type at_MEDIUM_TYPE_Get();
extern INT_type at_MEDIUM_TYPE_Deflt();
extern INT_type at_OFFSET_WDbug();
extern INT_type at_OFFSET_Copy();
extern INT_type at_OFFSET_CkVal();
extern POINTER_type at_OFFSET_Alloc();
extern INT_type at_OFFSET_Free();
extern INT_type at_OFFSET_Set();
extern POINTER_type at_OFFSET_Get();
extern INT_type at_OFFSET_Deflt();
extern INT_type at_POSITION_WDbug();
extern INT_type at_POSITION_Copy();
extern INT_type at_POSITION_CkVal();
extern POINTER_type at_POSITION_Alloc();
extern INT_type at_POSITION_Free();
extern INT_type at_POSITION_Set();
extern POINTER_type at_POSITION_Get();
extern INT_type at_POSITION_Deflt();
extern INT_type at_SAME_LAYOUT_OBJECT_WDbug();
extern INT_type at_SAME_LAYOUT_OBJECT_Copy();
extern INT_type at_SAME_LAYOUT_OBJECT_CkVal();
extern POINTER_type at_SAME_LAYOUT_OBJECT_Alloc();
extern INT_type at_SAME_LAYOUT_OBJECT_Free();
extern INT_type at_SAME_LAYOUT_OBJECT_Set();
extern POINTER_type at_SAME_LAYOUT_OBJECT_Get();
extern INT_type at_SAME_LAYOUT_OBJECT_Deflt();
extern INT_type at_SEPARATION_WDbug();
extern INT_type at_SEPARATION_Copy();
extern INT_type at_SEPARATION_CkVal();
extern POINTER_type at_SEPARATION_Alloc();
extern INT_type at_SEPARATION_Free();
extern INT_type at_SEPARATION_Set();
extern POINTER_type at_SEPARATION_Get();
extern INT_type at_SEPARATION_Deflt();
extern INT_type cc_ITEMISATION_WDbug();
extern INT_type cc_ITEMISATION_Copy();
extern INT_type cc_ITEMISATION_CkVal();
extern POINTER_type cc_ITEMISATION_Alloc();
extern INT_type cc_ITEMISATION_Free();
extern INT_type cc_ITEMISATION_Set();
extern POINTER_type cc_ITEMISATION_Get();
extern INT_type cc_ITEMISATION_Deflt();
extern INT_type CONSTITUENT_WDbug();
extern INT_type CONSTITUENT_Copy();
extern INT_type CONSTITUENT_CkVal();
extern POINTER_type CONSTITUENT_Alloc();
extern INT_type CONSTITUENT_Free();
extern INT_type CONSTITUENT_Set();
extern POINTER_type CONSTITUENT_Get();
extern INT_type CONSTITUENT_Deflt();
extern INT_type CONSTRUCTION_EXPR_WDbug();
extern INT_type CONSTRUCTION_EXPR_Copy();
extern INT_type CONSTRUCTION_EXPR_CkVal();
extern POINTER_type CONSTRUCTION_EXPR_Alloc();
extern INT_type CONSTRUCTION_EXPR_Free();
extern INT_type CONSTRUCTION_EXPR_Set();
extern POINTER_type CONSTRUCTION_EXPR_Get();
extern INT_type CONSTRUCTION_EXPR_Deflt();
extern INT_type dp_DOC_APP_PROF_DEFAULTS_WDbug();
extern INT_type dp_DOC_APP_PROF_DEFAULTS_Copy();
extern INT_type dp_DOC_APP_PROF_DEFAULTS_CkVal();
extern POINTER_type dp_DOC_APP_PROF_DEFAULTS_Alloc();
extern INT_type dp_DOC_APP_PROF_DEFAULTS_Free();
extern INT_type dp_DOC_APP_PROF_DEFAULTS_Set();
extern POINTER_type dp_DOC_APP_PROF_DEFAULTS_Get();
extern INT_type dp_DOC_APP_PROF_DEFAULTS_Deflt();
extern INT_type dp_LOCAL_FILE_REFERENCE_WDbug();
extern INT_type dp_LOCAL_FILE_REFERENCE_Copy();
extern INT_type dp_LOCAL_FILE_REFERENCE_CkVal();
extern POINTER_type dp_LOCAL_FILE_REFERENCE_Alloc();
extern INT_type dp_LOCAL_FILE_REFERENCE_Free();
extern INT_type dp_LOCAL_FILE_REFERENCE_Set();
extern POINTER_type dp_LOCAL_FILE_REFERENCE_Get();
extern INT_type dp_LOCAL_FILE_REFERENCE_Deflt();
extern INT_type dp_ODA_VERSION_WDbug();
extern INT_type dp_ODA_VERSION_Copy();
extern INT_type dp_ODA_VERSION_CkVal();
extern POINTER_type dp_ODA_VERSION_Alloc();
extern INT_type dp_ODA_VERSION_Free();
extern INT_type dp_ODA_VERSION_Set();
extern POINTER_type dp_ODA_VERSION_Get();
extern INT_type dp_ODA_VERSION_Deflt();
extern INT_type gg_COLOUR_REPS_WDbug();
extern INT_type gg_COLOUR_REPS_Copy();
extern INT_type gg_COLOUR_REPS_CkVal();
extern POINTER_type gg_COLOUR_REPS_Alloc();
extern INT_type gg_COLOUR_REPS_Free();
extern INT_type gg_COLOUR_REPS_Set();
extern POINTER_type gg_COLOUR_REPS_Get();
extern INT_type gg_COLOUR_REPS_Deflt();
extern INT_type gg_EDGE_RENDITION_WDbug();
extern INT_type gg_EDGE_RENDITION_Copy();
extern INT_type gg_EDGE_RENDITION_CkVal();
extern POINTER_type gg_EDGE_RENDITION_Alloc();
extern INT_type gg_EDGE_RENDITION_Free();
extern INT_type gg_EDGE_RENDITION_Set();
extern POINTER_type gg_EDGE_RENDITION_Get();
extern INT_type gg_EDGE_RENDITION_Deflt();
extern INT_type gg_ENCODING_ANNOUNCER_WDbug();
extern INT_type gg_ENCODING_ANNOUNCER_Copy();
extern INT_type gg_ENCODING_ANNOUNCER_CkVal();
extern POINTER_type gg_ENCODING_ANNOUNCER_Alloc();
extern INT_type gg_ENCODING_ANNOUNCER_Free();
extern INT_type gg_ENCODING_ANNOUNCER_Set();
extern POINTER_type gg_ENCODING_ANNOUNCER_Get();
extern INT_type gg_ENCODING_ANNOUNCER_Deflt();
extern INT_type gg_FILLED_AREA_REND_WDbug();
extern INT_type gg_FILLED_AREA_REND_Copy();
extern INT_type gg_FILLED_AREA_REND_CkVal();
extern POINTER_type gg_FILLED_AREA_REND_Alloc();
extern INT_type gg_FILLED_AREA_REND_Free();
extern INT_type gg_FILLED_AREA_REND_Set();
extern POINTER_type gg_FILLED_AREA_REND_Get();
extern INT_type gg_FILLED_AREA_REND_Deflt();
extern INT_type gg_LINE_RENDITION_WDbug();
extern INT_type gg_LINE_RENDITION_Copy();
extern INT_type gg_LINE_RENDITION_CkVal();
extern POINTER_type gg_LINE_RENDITION_Alloc();
extern INT_type gg_LINE_RENDITION_Free();
extern INT_type gg_LINE_RENDITION_Set();
extern POINTER_type gg_LINE_RENDITION_Get();
extern INT_type gg_LINE_RENDITION_Deflt();
extern INT_type gg_MARKER_RENDITION_WDbug();
extern INT_type gg_MARKER_RENDITION_Copy();
extern INT_type gg_MARKER_RENDITION_CkVal();
extern POINTER_type gg_MARKER_RENDITION_Alloc();
extern INT_type gg_MARKER_RENDITION_Free();
extern INT_type gg_MARKER_RENDITION_Set();
extern POINTER_type gg_MARKER_RENDITION_Get();
extern INT_type gg_MARKER_RENDITION_Deflt();
extern INT_type gg_PICTURE_DIMENSIONS_WDbug();
extern INT_type gg_PICTURE_DIMENSIONS_Copy();
extern INT_type gg_PICTURE_DIMENSIONS_CkVal();
extern POINTER_type gg_PICTURE_DIMENSIONS_Alloc();
extern INT_type gg_PICTURE_DIMENSIONS_Free();
extern INT_type gg_PICTURE_DIMENSIONS_Set();
extern POINTER_type gg_PICTURE_DIMENSIONS_Get();
extern INT_type gg_PICTURE_DIMENSIONS_Deflt();
extern INT_type gg_REGION_INTEREST_SPEC_WDbug();
extern INT_type gg_REGION_INTEREST_SPEC_Copy();
extern INT_type gg_REGION_INTEREST_SPEC_CkVal();
extern POINTER_type gg_REGION_INTEREST_SPEC_Alloc();
extern INT_type gg_REGION_INTEREST_SPEC_Free();
extern INT_type gg_REGION_INTEREST_SPEC_Set();
extern POINTER_type gg_REGION_INTEREST_SPEC_Get();
extern INT_type gg_REGION_INTEREST_SPEC_Deflt();
extern INT_type gg_TEXT_RENDITION_WDbug();
extern INT_type gg_TEXT_RENDITION_Copy();
extern INT_type gg_TEXT_RENDITION_CkVal();
extern POINTER_type gg_TEXT_RENDITION_Alloc();
extern INT_type gg_TEXT_RENDITION_Free();
extern INT_type gg_TEXT_RENDITION_Set();
extern POINTER_type gg_TEXT_RENDITION_Get();
extern INT_type gg_TEXT_RENDITION_Deflt();
extern INT_type gg_TRANSFORMATION_SPEC_WDbug();
extern INT_type gg_TRANSFORMATION_SPEC_Copy();
extern INT_type gg_TRANSFORMATION_SPEC_CkVal();
extern POINTER_type gg_TRANSFORMATION_SPEC_Alloc();
extern INT_type gg_TRANSFORMATION_SPEC_Free();
extern INT_type gg_TRANSFORMATION_SPEC_Set();
extern POINTER_type gg_TRANSFORMATION_SPEC_Get();
extern INT_type gg_TRANSFORMATION_SPEC_Deflt();
extern INT_type gg_TRANSPARENCY_SPEC_WDbug();
extern INT_type gg_TRANSPARENCY_SPEC_Copy();
extern INT_type gg_TRANSPARENCY_SPEC_CkVal();
extern POINTER_type gg_TRANSPARENCY_SPEC_Alloc();
extern INT_type gg_TRANSPARENCY_SPEC_Free();
extern INT_type gg_TRANSPARENCY_SPEC_Set();
extern POINTER_type gg_TRANSPARENCY_SPEC_Get();
extern INT_type gg_TRANSPARENCY_SPEC_Deflt();
extern INT_type INT_WDbug();
extern INT_type INT_Copy();
extern INT_type INT_CkVal();
extern POINTER_type INT_Alloc();
extern INT_type INT_Free();
extern INT_type INT_Set();
extern POINTER_type INT_Get();
extern INT_type INT_Deflt();
extern INT_type INT_PAIR_WDbug();
extern INT_type INT_PAIR_Copy();
extern INT_type INT_PAIR_CkVal();
extern POINTER_type INT_PAIR_Alloc();
extern INT_type INT_PAIR_Free();
extern INT_type INT_PAIR_Set();
extern POINTER_type INT_PAIR_Get();
extern INT_type INT_PAIR_Deflt();
extern INT_type LAYOUT_OBJECT_WDbug();
extern INT_type LAYOUT_OBJECT_Copy();
extern INT_type LAYOUT_OBJECT_CkVal();
extern POINTER_type LAYOUT_OBJECT_Alloc();
extern INT_type LAYOUT_OBJECT_Free();
extern INT_type LAYOUT_OBJECT_Set();
extern POINTER_type LAYOUT_OBJECT_Get();
extern INT_type LAYOUT_OBJECT_Deflt();
extern INT_type rg_CLIPPING_WDbug();
extern INT_type rg_CLIPPING_Copy();
extern INT_type rg_CLIPPING_CkVal();
extern POINTER_type rg_CLIPPING_Alloc();
extern INT_type rg_CLIPPING_Free();
extern INT_type rg_CLIPPING_Set();
extern POINTER_type rg_CLIPPING_Get();
extern INT_type rg_CLIPPING_Deflt();
extern INT_type rg_IMAGE_DIMENSIONS_WDbug();
extern INT_type rg_IMAGE_DIMENSIONS_Copy();
extern INT_type rg_IMAGE_DIMENSIONS_CkVal();
extern POINTER_type rg_IMAGE_DIMENSIONS_Alloc();
extern INT_type rg_IMAGE_DIMENSIONS_Free();
extern INT_type rg_IMAGE_DIMENSIONS_Set();
extern POINTER_type rg_IMAGE_DIMENSIONS_Get();
extern INT_type rg_IMAGE_DIMENSIONS_Deflt();
extern INT_type rg_PEL_SPACING_WDbug();
extern INT_type rg_PEL_SPACING_Copy();
extern INT_type rg_PEL_SPACING_CkVal();
extern POINTER_type rg_PEL_SPACING_Alloc();
extern INT_type rg_PEL_SPACING_Free();
extern INT_type rg_PEL_SPACING_Set();
extern POINTER_type rg_PEL_SPACING_Get();
extern INT_type rg_PEL_SPACING_Deflt();
extern INT_type SEQUENCE_ASN1_OBJECT_ID_WDbug();
extern INT_type SEQUENCE_ASN1_OBJECT_ID_Copy();
extern INT_type SEQUENCE_ASN1_OBJECT_ID_CkVal();
extern POINTER_type SEQUENCE_ASN1_OBJECT_ID_Alloc();
extern INT_type SEQUENCE_ASN1_OBJECT_ID_Free();
extern INT_type SEQUENCE_ASN1_OBJECT_ID_Set();
extern POINTER_type SEQUENCE_ASN1_OBJECT_ID_Get();
extern INT_type SEQUENCE_ASN1_OBJECT_ID_Deflt();
extern INT_type SEQUENCE_at_MEDIUM_TYPE_WDbug();
extern INT_type SEQUENCE_at_MEDIUM_TYPE_Copy();
extern INT_type SEQUENCE_at_MEDIUM_TYPE_CkVal();
extern POINTER_type SEQUENCE_at_MEDIUM_TYPE_Alloc();
extern INT_type SEQUENCE_at_MEDIUM_TYPE_Free();
extern INT_type SEQUENCE_at_MEDIUM_TYPE_Set();
extern POINTER_type SEQUENCE_at_MEDIUM_TYPE_Get();
extern INT_type SEQUENCE_at_MEDIUM_TYPE_Deflt();
extern INT_type SEQUENCE_BINDING_PAIR_WDbug();
extern INT_type SEQUENCE_BINDING_PAIR_Copy();
extern INT_type SEQUENCE_BINDING_PAIR_CkVal();
extern POINTER_type SEQUENCE_BINDING_PAIR_Alloc();
extern INT_type SEQUENCE_BINDING_PAIR_Free();
extern INT_type SEQUENCE_BINDING_PAIR_Set();
extern POINTER_type SEQUENCE_BINDING_PAIR_Get();
extern INT_type SEQUENCE_BINDING_PAIR_Deflt();
extern INT_type SEQUENCE_BYTE_WDbug();
extern INT_type SEQUENCE_BYTE_Copy();
extern INT_type SEQUENCE_BYTE_CkVal();
extern POINTER_type SEQUENCE_BYTE_Alloc();
extern INT_type SEQUENCE_BYTE_Free();
extern INT_type SEQUENCE_BYTE_Set();
extern POINTER_type SEQUENCE_BYTE_Get();
extern INT_type SEQUENCE_BYTE_Deflt();
extern INT_type SEQUENCE_CONSTITUENT_WDbug();
extern INT_type SEQUENCE_CONSTITUENT_Copy();
extern INT_type SEQUENCE_CONSTITUENT_CkVal();
extern POINTER_type SEQUENCE_CONSTITUENT_Alloc();
extern INT_type SEQUENCE_CONSTITUENT_Free();
extern INT_type SEQUENCE_CONSTITUENT_Set();
extern POINTER_type SEQUENCE_CONSTITUENT_Get();
extern INT_type SEQUENCE_CONSTITUENT_Deflt();
extern INT_type SEQUENCE_COPYRIGHT_INFO_WDbug();
extern INT_type SEQUENCE_COPYRIGHT_INFO_Copy();
extern INT_type SEQUENCE_COPYRIGHT_INFO_CkVal();
extern POINTER_type SEQUENCE_COPYRIGHT_INFO_Alloc();
extern INT_type SEQUENCE_COPYRIGHT_INFO_Free();
extern INT_type SEQUENCE_COPYRIGHT_INFO_Set();
extern POINTER_type SEQUENCE_COPYRIGHT_INFO_Get();
extern INT_type SEQUENCE_COPYRIGHT_INFO_Deflt();
extern INT_type SEQUENCE_FONT_PAIR_WDbug();
extern INT_type SEQUENCE_FONT_PAIR_Copy();
extern INT_type SEQUENCE_FONT_PAIR_CkVal();
extern POINTER_type SEQUENCE_FONT_PAIR_Alloc();
extern INT_type SEQUENCE_FONT_PAIR_Free();
extern INT_type SEQUENCE_FONT_PAIR_Set();
extern POINTER_type SEQUENCE_FONT_PAIR_Get();
extern INT_type SEQUENCE_FONT_PAIR_Deflt();
extern INT_type SEQUENCE_INT_WDbug();
extern INT_type SEQUENCE_INT_Copy();
extern INT_type SEQUENCE_INT_CkVal();
extern POINTER_type SEQUENCE_INT_Alloc();
extern INT_type SEQUENCE_INT_Free();
extern INT_type SEQUENCE_INT_Set();
extern POINTER_type SEQUENCE_INT_Get();
extern INT_type SEQUENCE_INT_Deflt();
extern INT_type SEQUENCE_INT_PAIR_WDbug();
extern INT_type SEQUENCE_INT_PAIR_Copy();
extern INT_type SEQUENCE_INT_PAIR_CkVal();
extern POINTER_type SEQUENCE_INT_PAIR_Alloc();
extern INT_type SEQUENCE_INT_PAIR_Free();
extern INT_type SEQUENCE_INT_PAIR_Set();
extern POINTER_type SEQUENCE_INT_PAIR_Get();
extern INT_type SEQUENCE_INT_PAIR_Deflt();
extern INT_type SEQUENCE_NAME_ORG_WDbug();
extern INT_type SEQUENCE_NAME_ORG_Copy();
extern INT_type SEQUENCE_NAME_ORG_CkVal();
extern POINTER_type SEQUENCE_NAME_ORG_Alloc();
extern INT_type SEQUENCE_NAME_ORG_Free();
extern INT_type SEQUENCE_NAME_ORG_Set();
extern POINTER_type SEQUENCE_NAME_ORG_Get();
extern INT_type SEQUENCE_NAME_ORG_Deflt();
extern INT_type SEQUENCE_RESOURCE_MAP_WDbug();
extern INT_type SEQUENCE_RESOURCE_MAP_Copy();
extern INT_type SEQUENCE_RESOURCE_MAP_CkVal();
extern POINTER_type SEQUENCE_RESOURCE_MAP_Alloc();
extern INT_type SEQUENCE_RESOURCE_MAP_Free();
extern INT_type SEQUENCE_RESOURCE_MAP_Set();
extern POINTER_type SEQUENCE_RESOURCE_MAP_Get();
extern INT_type SEQUENCE_RESOURCE_MAP_Deflt();
extern INT_type SEQUENCE_REVISION_HISTORY_WDbug();
extern INT_type SEQUENCE_REVISION_HISTORY_Copy();
extern INT_type SEQUENCE_REVISION_HISTORY_CkVal();
extern POINTER_type SEQUENCE_REVISION_HISTORY_Alloc();
extern INT_type SEQUENCE_REVISION_HISTORY_Free();
extern INT_type SEQUENCE_REVISION_HISTORY_Set();
extern POINTER_type SEQUENCE_REVISION_HISTORY_Get();
extern INT_type SEQUENCE_REVISION_HISTORY_Deflt();
extern INT_type SEQUENCE_STRING_WDbug();
extern INT_type SEQUENCE_STRING_Copy();
extern INT_type SEQUENCE_STRING_CkVal();
extern POINTER_type SEQUENCE_STRING_Alloc();
extern INT_type SEQUENCE_STRING_Free();
extern INT_type SEQUENCE_STRING_Set();
extern POINTER_type SEQUENCE_STRING_Get();
extern INT_type SEQUENCE_STRING_Deflt();
extern INT_type SEQUENCE_STRING_OR_ID_WDbug();
extern INT_type SEQUENCE_STRING_OR_ID_Copy();
extern INT_type SEQUENCE_STRING_OR_ID_CkVal();
extern POINTER_type SEQUENCE_STRING_OR_ID_Alloc();
extern INT_type SEQUENCE_STRING_OR_ID_Free();
extern INT_type SEQUENCE_STRING_OR_ID_Set();
extern POINTER_type SEQUENCE_STRING_OR_ID_Get();
extern INT_type SEQUENCE_STRING_OR_ID_Deflt();
extern INT_type SEQUENCE_TAB_STOP_WDbug();
extern INT_type SEQUENCE_TAB_STOP_Copy();
extern INT_type SEQUENCE_TAB_STOP_CkVal();
extern POINTER_type SEQUENCE_TAB_STOP_Alloc();
extern INT_type SEQUENCE_TAB_STOP_Free();
extern INT_type SEQUENCE_TAB_STOP_Set();
extern POINTER_type SEQUENCE_TAB_STOP_Get();
extern INT_type SEQUENCE_TAB_STOP_Deflt();
extern INT_type SEQUENCE_VALUE_WDbug();
extern INT_type SEQUENCE_VALUE_Copy();
extern INT_type SEQUENCE_VALUE_CkVal();
extern POINTER_type SEQUENCE_VALUE_Alloc();
extern INT_type SEQUENCE_VALUE_Free();
extern INT_type SEQUENCE_VALUE_Set();
extern POINTER_type SEQUENCE_VALUE_Get();
extern INT_type SEQUENCE_VALUE_Deflt();
extern INT_type STRING_WDbug();
extern INT_type STRING_Copy();
extern INT_type STRING_CkVal();
extern POINTER_type STRING_Alloc();
extern INT_type STRING_Free();
extern INT_type STRING_Set();
extern POINTER_type STRING_Get();
extern INT_type STRING_Deflt();
extern INT_type STRING_EXPR_WDbug();
extern INT_type STRING_EXPR_Copy();
extern INT_type STRING_EXPR_CkVal();
extern POINTER_type STRING_EXPR_Alloc();
extern INT_type STRING_EXPR_Free();
extern INT_type STRING_EXPR_Set();
extern POINTER_type STRING_EXPR_Get();
extern INT_type STRING_EXPR_Deflt();
extern INT_type STRING_OR_ID_WDbug();
extern INT_type STRING_OR_ID_Copy();
extern INT_type STRING_OR_ID_CkVal();
extern POINTER_type STRING_OR_ID_Alloc();
extern INT_type STRING_OR_ID_Free();
extern INT_type STRING_OR_ID_Set();
extern POINTER_type STRING_OR_ID_Get();
extern INT_type STRING_OR_ID_Deflt();
extern INT_type VALUE_WDbug();
extern INT_type VALUE_Copy();
extern INT_type VALUE_CkVal();
extern POINTER_type VALUE_Alloc();
extern INT_type VALUE_Free();
extern INT_type VALUE_Set();
extern POINTER_type VALUE_Get();
extern INT_type VALUE_Deflt();


/*
	This table contains the canonical procedures
	for operating on each of the aunion.h types
*/


/* real name for the attr type table */

#ifndef USE_LONG_NAMES
#define ATTR_AttrTypeTbl	gatatt

#endif


#ifdef TK_GVARS_ATTR18
#define EXTERN
#else
#define EXTERN extern
#endif


EXTERN struct ATTR_AttrTypeTbl_struct {

	INT_type (*write_debug)();

	INT_type (*copy)();

	INT_type (*check_set_attr_value)();

	POINTER_type (*allocate)();

	INT_type (*free)();

	INT_type (*set)();

	POINTER_type (*get)();

	INT_type (*iso_default)();

} ATTR_AttrTypeTbl[]

#ifdef TK_GVARS_ATTR18
= {

{ at_BORDER_WDbug,
	at_BORDER_Copy,
	at_BORDER_CkVal,
	at_BORDER_Alloc,
	at_BORDER_Free,
 	at_BORDER_Set,
	at_BORDER_Get,
	at_BORDER_Deflt },
{ at_DEFAULT_VALUE_LISTS_WDbug,
	at_DEFAULT_VALUE_LISTS_Copy,
	at_DEFAULT_VALUE_LISTS_CkVal,
	at_DEFAULT_VALUE_LISTS_Alloc,
	at_DEFAULT_VALUE_LISTS_Free,
 	at_DEFAULT_VALUE_LISTS_Set,
	at_DEFAULT_VALUE_LISTS_Get,
	at_DEFAULT_VALUE_LISTS_Deflt },
{ at_DIMENSIONS_WDbug,
	at_DIMENSIONS_Copy,
	at_DIMENSIONS_CkVal,
	at_DIMENSIONS_Alloc,
	at_DIMENSIONS_Free,
 	at_DIMENSIONS_Set,
	at_DIMENSIONS_Get,
	at_DIMENSIONS_Deflt },
{ at_MEDIUM_TYPE_WDbug,
	at_MEDIUM_TYPE_Copy,
	at_MEDIUM_TYPE_CkVal,
	at_MEDIUM_TYPE_Alloc,
	at_MEDIUM_TYPE_Free,
 	at_MEDIUM_TYPE_Set,
	at_MEDIUM_TYPE_Get,
	at_MEDIUM_TYPE_Deflt },
{ at_OFFSET_WDbug,
	at_OFFSET_Copy,
	at_OFFSET_CkVal,
	at_OFFSET_Alloc,
	at_OFFSET_Free,
 	at_OFFSET_Set,
	at_OFFSET_Get,
	at_OFFSET_Deflt },
{ at_POSITION_WDbug,
	at_POSITION_Copy,
	at_POSITION_CkVal,
	at_POSITION_Alloc,
	at_POSITION_Free,
 	at_POSITION_Set,
	at_POSITION_Get,
	at_POSITION_Deflt },
{ at_SAME_LAYOUT_OBJECT_WDbug,
	at_SAME_LAYOUT_OBJECT_Copy,
	at_SAME_LAYOUT_OBJECT_CkVal,
	at_SAME_LAYOUT_OBJECT_Alloc,
	at_SAME_LAYOUT_OBJECT_Free,
 	at_SAME_LAYOUT_OBJECT_Set,
	at_SAME_LAYOUT_OBJECT_Get,
	at_SAME_LAYOUT_OBJECT_Deflt },
{ at_SEPARATION_WDbug,
	at_SEPARATION_Copy,
	at_SEPARATION_CkVal,
	at_SEPARATION_Alloc,
	at_SEPARATION_Free,
 	at_SEPARATION_Set,
	at_SEPARATION_Get,
	at_SEPARATION_Deflt },
{ cc_ITEMISATION_WDbug,
	cc_ITEMISATION_Copy,
	cc_ITEMISATION_CkVal,
	cc_ITEMISATION_Alloc,
	cc_ITEMISATION_Free,
 	cc_ITEMISATION_Set,
	cc_ITEMISATION_Get,
	cc_ITEMISATION_Deflt },
{ CONSTITUENT_WDbug,
	CONSTITUENT_Copy,
	CONSTITUENT_CkVal,
	CONSTITUENT_Alloc,
	CONSTITUENT_Free,
 	CONSTITUENT_Set,
	CONSTITUENT_Get,
	CONSTITUENT_Deflt },
{ CONSTRUCTION_EXPR_WDbug,
	CONSTRUCTION_EXPR_Copy,
	CONSTRUCTION_EXPR_CkVal,
	CONSTRUCTION_EXPR_Alloc,
	CONSTRUCTION_EXPR_Free,
 	CONSTRUCTION_EXPR_Set,
	CONSTRUCTION_EXPR_Get,
	CONSTRUCTION_EXPR_Deflt },
{ dp_DOC_APP_PROF_DEFAULTS_WDbug,
	dp_DOC_APP_PROF_DEFAULTS_Copy,
	dp_DOC_APP_PROF_DEFAULTS_CkVal,
	dp_DOC_APP_PROF_DEFAULTS_Alloc,
	dp_DOC_APP_PROF_DEFAULTS_Free,
 	dp_DOC_APP_PROF_DEFAULTS_Set,
	dp_DOC_APP_PROF_DEFAULTS_Get,
	dp_DOC_APP_PROF_DEFAULTS_Deflt },
{ dp_LOCAL_FILE_REFERENCE_WDbug,
	dp_LOCAL_FILE_REFERENCE_Copy,
	dp_LOCAL_FILE_REFERENCE_CkVal,
	dp_LOCAL_FILE_REFERENCE_Alloc,
	dp_LOCAL_FILE_REFERENCE_Free,
 	dp_LOCAL_FILE_REFERENCE_Set,
	dp_LOCAL_FILE_REFERENCE_Get,
	dp_LOCAL_FILE_REFERENCE_Deflt },
{ dp_ODA_VERSION_WDbug,
	dp_ODA_VERSION_Copy,
	dp_ODA_VERSION_CkVal,
	dp_ODA_VERSION_Alloc,
	dp_ODA_VERSION_Free,
 	dp_ODA_VERSION_Set,
	dp_ODA_VERSION_Get,
	dp_ODA_VERSION_Deflt },
{ gg_COLOUR_REPS_WDbug,
	gg_COLOUR_REPS_Copy,
	gg_COLOUR_REPS_CkVal,
	gg_COLOUR_REPS_Alloc,
	gg_COLOUR_REPS_Free,
 	gg_COLOUR_REPS_Set,
	gg_COLOUR_REPS_Get,
	gg_COLOUR_REPS_Deflt },
{ gg_EDGE_RENDITION_WDbug,
	gg_EDGE_RENDITION_Copy,
	gg_EDGE_RENDITION_CkVal,
	gg_EDGE_RENDITION_Alloc,
	gg_EDGE_RENDITION_Free,
 	gg_EDGE_RENDITION_Set,
	gg_EDGE_RENDITION_Get,
	gg_EDGE_RENDITION_Deflt },
{ gg_ENCODING_ANNOUNCER_WDbug,
	gg_ENCODING_ANNOUNCER_Copy,
	gg_ENCODING_ANNOUNCER_CkVal,
	gg_ENCODING_ANNOUNCER_Alloc,
	gg_ENCODING_ANNOUNCER_Free,
 	gg_ENCODING_ANNOUNCER_Set,
	gg_ENCODING_ANNOUNCER_Get,
	gg_ENCODING_ANNOUNCER_Deflt },
{ gg_FILLED_AREA_REND_WDbug,
	gg_FILLED_AREA_REND_Copy,
	gg_FILLED_AREA_REND_CkVal,
	gg_FILLED_AREA_REND_Alloc,
	gg_FILLED_AREA_REND_Free,
 	gg_FILLED_AREA_REND_Set,
	gg_FILLED_AREA_REND_Get,
	gg_FILLED_AREA_REND_Deflt },
{ gg_LINE_RENDITION_WDbug,
	gg_LINE_RENDITION_Copy,
	gg_LINE_RENDITION_CkVal,
	gg_LINE_RENDITION_Alloc,
	gg_LINE_RENDITION_Free,
 	gg_LINE_RENDITION_Set,
	gg_LINE_RENDITION_Get,
	gg_LINE_RENDITION_Deflt },
{ gg_MARKER_RENDITION_WDbug,
	gg_MARKER_RENDITION_Copy,
	gg_MARKER_RENDITION_CkVal,
	gg_MARKER_RENDITION_Alloc,
	gg_MARKER_RENDITION_Free,
 	gg_MARKER_RENDITION_Set,
	gg_MARKER_RENDITION_Get,
	gg_MARKER_RENDITION_Deflt },
{ gg_PICTURE_DIMENSIONS_WDbug,
	gg_PICTURE_DIMENSIONS_Copy,
	gg_PICTURE_DIMENSIONS_CkVal,
	gg_PICTURE_DIMENSIONS_Alloc,
	gg_PICTURE_DIMENSIONS_Free,
 	gg_PICTURE_DIMENSIONS_Set,
	gg_PICTURE_DIMENSIONS_Get,
	gg_PICTURE_DIMENSIONS_Deflt },
{ gg_REGION_INTEREST_SPEC_WDbug,
	gg_REGION_INTEREST_SPEC_Copy,
	gg_REGION_INTEREST_SPEC_CkVal,
	gg_REGION_INTEREST_SPEC_Alloc,
	gg_REGION_INTEREST_SPEC_Free,
 	gg_REGION_INTEREST_SPEC_Set,
	gg_REGION_INTEREST_SPEC_Get,
	gg_REGION_INTEREST_SPEC_Deflt },
{ gg_TEXT_RENDITION_WDbug,
	gg_TEXT_RENDITION_Copy,
	gg_TEXT_RENDITION_CkVal,
	gg_TEXT_RENDITION_Alloc,
	gg_TEXT_RENDITION_Free,
 	gg_TEXT_RENDITION_Set,
	gg_TEXT_RENDITION_Get,
	gg_TEXT_RENDITION_Deflt },
{ gg_TRANSFORMATION_SPEC_WDbug,
	gg_TRANSFORMATION_SPEC_Copy,
	gg_TRANSFORMATION_SPEC_CkVal,
	gg_TRANSFORMATION_SPEC_Alloc,
	gg_TRANSFORMATION_SPEC_Free,
 	gg_TRANSFORMATION_SPEC_Set,
	gg_TRANSFORMATION_SPEC_Get,
	gg_TRANSFORMATION_SPEC_Deflt },
{ gg_TRANSPARENCY_SPEC_WDbug,
	gg_TRANSPARENCY_SPEC_Copy,
	gg_TRANSPARENCY_SPEC_CkVal,
	gg_TRANSPARENCY_SPEC_Alloc,
	gg_TRANSPARENCY_SPEC_Free,
 	gg_TRANSPARENCY_SPEC_Set,
	gg_TRANSPARENCY_SPEC_Get,
	gg_TRANSPARENCY_SPEC_Deflt },
{ INT_WDbug,
	INT_Copy,
	INT_CkVal,
	INT_Alloc,
	INT_Free,
 	INT_Set,
	INT_Get,
	INT_Deflt },
{ INT_PAIR_WDbug,
	INT_PAIR_Copy,
	INT_PAIR_CkVal,
	INT_PAIR_Alloc,
	INT_PAIR_Free,
 	INT_PAIR_Set,
	INT_PAIR_Get,
	INT_PAIR_Deflt },
{ LAYOUT_OBJECT_WDbug,
	LAYOUT_OBJECT_Copy,
	LAYOUT_OBJECT_CkVal,
	LAYOUT_OBJECT_Alloc,
	LAYOUT_OBJECT_Free,
 	LAYOUT_OBJECT_Set,
	LAYOUT_OBJECT_Get,
	LAYOUT_OBJECT_Deflt },
{ rg_CLIPPING_WDbug,
	rg_CLIPPING_Copy,
	rg_CLIPPING_CkVal,
	rg_CLIPPING_Alloc,
	rg_CLIPPING_Free,
 	rg_CLIPPING_Set,
	rg_CLIPPING_Get,
	rg_CLIPPING_Deflt },
{ rg_IMAGE_DIMENSIONS_WDbug,
	rg_IMAGE_DIMENSIONS_Copy,
	rg_IMAGE_DIMENSIONS_CkVal,
	rg_IMAGE_DIMENSIONS_Alloc,
	rg_IMAGE_DIMENSIONS_Free,
 	rg_IMAGE_DIMENSIONS_Set,
	rg_IMAGE_DIMENSIONS_Get,
	rg_IMAGE_DIMENSIONS_Deflt },
{ rg_PEL_SPACING_WDbug,
	rg_PEL_SPACING_Copy,
	rg_PEL_SPACING_CkVal,
	rg_PEL_SPACING_Alloc,
	rg_PEL_SPACING_Free,
 	rg_PEL_SPACING_Set,
	rg_PEL_SPACING_Get,
	rg_PEL_SPACING_Deflt },
{ SEQUENCE_ASN1_OBJECT_ID_WDbug,
	SEQUENCE_ASN1_OBJECT_ID_Copy,
	SEQUENCE_ASN1_OBJECT_ID_CkVal,
	SEQUENCE_ASN1_OBJECT_ID_Alloc,
	SEQUENCE_ASN1_OBJECT_ID_Free,
 	SEQUENCE_ASN1_OBJECT_ID_Set,
	SEQUENCE_ASN1_OBJECT_ID_Get,
	SEQUENCE_ASN1_OBJECT_ID_Deflt },
{ SEQUENCE_at_MEDIUM_TYPE_WDbug,
	SEQUENCE_at_MEDIUM_TYPE_Copy,
	SEQUENCE_at_MEDIUM_TYPE_CkVal,
	SEQUENCE_at_MEDIUM_TYPE_Alloc,
	SEQUENCE_at_MEDIUM_TYPE_Free,
 	SEQUENCE_at_MEDIUM_TYPE_Set,
	SEQUENCE_at_MEDIUM_TYPE_Get,
	SEQUENCE_at_MEDIUM_TYPE_Deflt },
{ SEQUENCE_BINDING_PAIR_WDbug,
	SEQUENCE_BINDING_PAIR_Copy,
	SEQUENCE_BINDING_PAIR_CkVal,
	SEQUENCE_BINDING_PAIR_Alloc,
	SEQUENCE_BINDING_PAIR_Free,
 	SEQUENCE_BINDING_PAIR_Set,
	SEQUENCE_BINDING_PAIR_Get,
	SEQUENCE_BINDING_PAIR_Deflt },
{ SEQUENCE_BYTE_WDbug,
	SEQUENCE_BYTE_Copy,
	SEQUENCE_BYTE_CkVal,
	SEQUENCE_BYTE_Alloc,
	SEQUENCE_BYTE_Free,
 	SEQUENCE_BYTE_Set,
	SEQUENCE_BYTE_Get,
	SEQUENCE_BYTE_Deflt },
{ SEQUENCE_CONSTITUENT_WDbug,
	SEQUENCE_CONSTITUENT_Copy,
	SEQUENCE_CONSTITUENT_CkVal,
	SEQUENCE_CONSTITUENT_Alloc,
	SEQUENCE_CONSTITUENT_Free,
 	SEQUENCE_CONSTITUENT_Set,
	SEQUENCE_CONSTITUENT_Get,
	SEQUENCE_CONSTITUENT_Deflt },
{ SEQUENCE_COPYRIGHT_INFO_WDbug,
	SEQUENCE_COPYRIGHT_INFO_Copy,
	SEQUENCE_COPYRIGHT_INFO_CkVal,
	SEQUENCE_COPYRIGHT_INFO_Alloc,
	SEQUENCE_COPYRIGHT_INFO_Free,
 	SEQUENCE_COPYRIGHT_INFO_Set,
	SEQUENCE_COPYRIGHT_INFO_Get,
	SEQUENCE_COPYRIGHT_INFO_Deflt },
{ SEQUENCE_FONT_PAIR_WDbug,
	SEQUENCE_FONT_PAIR_Copy,
	SEQUENCE_FONT_PAIR_CkVal,
	SEQUENCE_FONT_PAIR_Alloc,
	SEQUENCE_FONT_PAIR_Free,
 	SEQUENCE_FONT_PAIR_Set,
	SEQUENCE_FONT_PAIR_Get,
	SEQUENCE_FONT_PAIR_Deflt },
{ SEQUENCE_INT_WDbug,
	SEQUENCE_INT_Copy,
	SEQUENCE_INT_CkVal,
	SEQUENCE_INT_Alloc,
	SEQUENCE_INT_Free,
 	SEQUENCE_INT_Set,
	SEQUENCE_INT_Get,
	SEQUENCE_INT_Deflt },
{ SEQUENCE_INT_PAIR_WDbug,
	SEQUENCE_INT_PAIR_Copy,
	SEQUENCE_INT_PAIR_CkVal,
	SEQUENCE_INT_PAIR_Alloc,
	SEQUENCE_INT_PAIR_Free,
 	SEQUENCE_INT_PAIR_Set,
	SEQUENCE_INT_PAIR_Get,
	SEQUENCE_INT_PAIR_Deflt },
{ SEQUENCE_NAME_ORG_WDbug,
	SEQUENCE_NAME_ORG_Copy,
	SEQUENCE_NAME_ORG_CkVal,
	SEQUENCE_NAME_ORG_Alloc,
	SEQUENCE_NAME_ORG_Free,
 	SEQUENCE_NAME_ORG_Set,
	SEQUENCE_NAME_ORG_Get,
	SEQUENCE_NAME_ORG_Deflt },
{ SEQUENCE_RESOURCE_MAP_WDbug,
	SEQUENCE_RESOURCE_MAP_Copy,
	SEQUENCE_RESOURCE_MAP_CkVal,
	SEQUENCE_RESOURCE_MAP_Alloc,
	SEQUENCE_RESOURCE_MAP_Free,
 	SEQUENCE_RESOURCE_MAP_Set,
	SEQUENCE_RESOURCE_MAP_Get,
	SEQUENCE_RESOURCE_MAP_Deflt },
{ SEQUENCE_REVISION_HISTORY_WDbug,
	SEQUENCE_REVISION_HISTORY_Copy,
	SEQUENCE_REVISION_HISTORY_CkVal,
	SEQUENCE_REVISION_HISTORY_Alloc,
	SEQUENCE_REVISION_HISTORY_Free,
 	SEQUENCE_REVISION_HISTORY_Set,
	SEQUENCE_REVISION_HISTORY_Get,
	SEQUENCE_REVISION_HISTORY_Deflt },
{ SEQUENCE_STRING_WDbug,
	SEQUENCE_STRING_Copy,
	SEQUENCE_STRING_CkVal,
	SEQUENCE_STRING_Alloc,
	SEQUENCE_STRING_Free,
 	SEQUENCE_STRING_Set,
	SEQUENCE_STRING_Get,
	SEQUENCE_STRING_Deflt },
{ SEQUENCE_STRING_OR_ID_WDbug,
	SEQUENCE_STRING_OR_ID_Copy,
	SEQUENCE_STRING_OR_ID_CkVal,
	SEQUENCE_STRING_OR_ID_Alloc,
	SEQUENCE_STRING_OR_ID_Free,
 	SEQUENCE_STRING_OR_ID_Set,
	SEQUENCE_STRING_OR_ID_Get,
	SEQUENCE_STRING_OR_ID_Deflt },
{ SEQUENCE_TAB_STOP_WDbug,
	SEQUENCE_TAB_STOP_Copy,
	SEQUENCE_TAB_STOP_CkVal,
	SEQUENCE_TAB_STOP_Alloc,
	SEQUENCE_TAB_STOP_Free,
 	SEQUENCE_TAB_STOP_Set,
	SEQUENCE_TAB_STOP_Get,
	SEQUENCE_TAB_STOP_Deflt },
{ SEQUENCE_VALUE_WDbug,
	SEQUENCE_VALUE_Copy,
	SEQUENCE_VALUE_CkVal,
	SEQUENCE_VALUE_Alloc,
	SEQUENCE_VALUE_Free,
 	SEQUENCE_VALUE_Set,
	SEQUENCE_VALUE_Get,
	SEQUENCE_VALUE_Deflt },
{ STRING_WDbug,
	STRING_Copy,
	STRING_CkVal,
	STRING_Alloc,
	STRING_Free,
 	STRING_Set,
	STRING_Get,
	STRING_Deflt },
{ STRING_EXPR_WDbug,
	STRING_EXPR_Copy,
	STRING_EXPR_CkVal,
	STRING_EXPR_Alloc,
	STRING_EXPR_Free,
 	STRING_EXPR_Set,
	STRING_EXPR_Get,
	STRING_EXPR_Deflt },
{ STRING_OR_ID_WDbug,
	STRING_OR_ID_Copy,
	STRING_OR_ID_CkVal,
	STRING_OR_ID_Alloc,
	STRING_OR_ID_Free,
 	STRING_OR_ID_Set,
	STRING_OR_ID_Get,
	STRING_OR_ID_Deflt },
{ VALUE_WDbug,
	VALUE_Copy,
	VALUE_CkVal,
	VALUE_Alloc,
	VALUE_Free,
 	VALUE_Set,
	VALUE_Get,
	VALUE_Deflt }
}
#endif
;
/* this endif closes the ifdef on GVARS */
#undef EXTERN


#endif
#endif
 /* these endif's close the ifdef TK_ATTR
 and ifndef TK_INCLUDED at the top of the file */


#undef TK_ATTR
