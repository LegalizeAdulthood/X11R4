
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
 CMU-ITC-071) for full text & permitted uses.


	Include file defining attribute union type

    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.

*/

#ifndef TK_INCLUDED_AUNION
#define TK_INCLUDED_AUNION

#include <lcsite.h>
#include <apityp.h>

typedef union {

    at_BORDER_type	*at_BORDER_field;
    at_DEFAULT_VALUE_LISTS_type	*at_DEFAULT_VALUE_LISTS_field;
    at_DIMENSIONS_type	*at_DIMENSIONS_field;
    at_MEDIUM_TYPE_type	*at_MEDIUM_TYPE_field;
    at_OFFSET_type	*at_OFFSET_field;
    at_POSITION_type	*at_POSITION_field;
    at_SAME_LAYOUT_OBJECT_type	*at_SAME_LAYOUT_OBJECT_field;
    at_SEPARATION_type	*at_SEPARATION_field;
    cc_ITEMISATION_type	*cc_ITEMISATION_field;
    struct	 CONSTITUENT_struct *CONSTITUENT_field;
    CONSTRUCTION_EXPR_type	*CONSTRUCTION_EXPR_field;
    dp_DOC_APP_PROF_DEFAULTS_type	*dp_DOC_APP_PROF_DEFAULTS_field;
    dp_LOCAL_FILE_REFERENCE_type	*dp_LOCAL_FILE_REFERENCE_field;
    dp_ODA_VERSION_type	*dp_ODA_VERSION_field;
    INT_PAIR_type	*INT_PAIR_field;
    INT_type	*INT_field;
    LAYOUT_OBJECT_type	*LAYOUT_OBJECT_field;
    rg_CLIPPING_type	*rg_CLIPPING_field;
    rg_IMAGE_DIMENSIONS_type	*rg_IMAGE_DIMENSIONS_field;
    rg_PEL_SPACING_type	*rg_PEL_SPACING_field;
    SEQUENCE_ASN1_OBJECT_ID_type	SEQUENCE_ASN1_OBJECT_ID_field;
    SEQUENCE_at_MEDIUM_TYPE_type	SEQUENCE_at_MEDIUM_TYPE_field;
    SEQUENCE_BINDING_PAIR_type	SEQUENCE_BINDING_PAIR_field;
    SEQUENCE_BYTE_type	SEQUENCE_BYTE_field;
    SEQUENCE_CONSTITUENT_type	SEQUENCE_CONSTITUENT_field;
    SEQUENCE_COPYRIGHT_INFO_type	SEQUENCE_COPYRIGHT_INFO_field;
    SEQUENCE_FONT_PAIR_type	SEQUENCE_FONT_PAIR_field;
    SEQUENCE_INT_PAIR_type	SEQUENCE_INT_PAIR_field;
    SEQUENCE_INT_type	SEQUENCE_INT_field;
    SEQUENCE_NAME_ORG_type	SEQUENCE_NAME_ORG_field;
    SEQUENCE_RESOURCE_MAP_type	SEQUENCE_RESOURCE_MAP_field;
    SEQUENCE_REVISION_HISTORY_type	SEQUENCE_REVISION_HISTORY_field;
    SEQUENCE_STRING_OR_ID_type	SEQUENCE_STRING_OR_ID_field;
    SEQUENCE_STRING_type	SEQUENCE_STRING_field;
    SEQUENCE_TAB_STOP_type	SEQUENCE_TAB_STOP_field;
    SEQUENCE_VALUE_type	SEQUENCE_VALUE_field;
    STRING_EXPR_type	*STRING_EXPR_field;
    STRING_OR_ID_type	*STRING_OR_ID_field;
    STRING_type	STRING_field;
    VALUE_type	*VALUE_field;

} ATTR_UNION_t;

#define GET_at_BORDER(attr)	((attr)->attribute_value.at_BORDER_field)
#define GET_at_DEFAULT_VALUE_LISTS(attr)	((attr)->attribute_value.at_DEFAULT_VALUE_LISTS_field)
#define GET_at_DIMENSIONS(attr)	((attr)->attribute_value.at_DIMENSIONS_field)
#define GET_at_MEDIUM_TYPE(attr)	((attr)->attribute_value.at_MEDIUM_TYPE_field)
#define GET_at_OFFSET(attr)	((attr)->attribute_value.at_OFFSET_field)
#define GET_at_POSITION(attr)	((attr)->attribute_value.at_POSITION_field)
#define GET_at_SAME_LAYOUT_OBJECT(attr)	((attr)->attribute_value.at_SAME_LAYOUT_OBJECT_field)
#define GET_at_SEPARATION(attr)	((attr)->attribute_value.at_SEPARATION_field)
#define GET_cc_ITEMISATION(attr)	((attr)->attribute_value.cc_ITEMISATION_field)
#define GET_CONSTITUENT(attr)	((attr)->attribute_value.CONSTITUENT_field)
#define GET_CONSTRUCTION_EXPR(attr)	((attr)->attribute_value.CONSTRUCTION_EXPR_field)
#define GET_dp_DOC_APP_PROF_DEFAULTS(attr)	((attr)->attribute_value.dp_DOC_APP_PROF_DEFAULTS_field)
#define GET_dp_LOCAL_FILE_REFERENCE(attr)	((attr)->attribute_value.dp_LOCAL_FILE_REFERENCE_field)
#define GET_dp_ODA_VERSION(attr)	((attr)->attribute_value.dp_ODA_VERSION_field)
#define GET_INT_PAIR(attr)	((attr)->attribute_value.INT_PAIR_field)
#define GET_INT(attr)	(*(attr)->attribute_value.INT_field)
#define GET_LAYOUT_OBJECT(attr)	((attr)->attribute_value.LAYOUT_OBJECT_field)
#define GET_rg_CLIPPING(attr)	((attr)->attribute_value.rg_CLIPPING_field)
#define GET_rg_IMAGE_DIMENSIONS(attr)	((attr)->attribute_value.rg_IMAGE_DIMENSIONS_field)
#define GET_rg_PEL_SPACING(attr)	((attr)->attribute_value.rg_PEL_SPACING_field)
#define GET_SEQUENCE_ASN1_OBJECT_ID(attr)	((attr)->attribute_value.SEQUENCE_ASN1_OBJECT_ID_field)
#define GET_SEQUENCE_at_MEDIUM_TYPE(attr)	((attr)->attribute_value.SEQUENCE_at_MEDIUM_TYPE_field)
#define GET_SEQUENCE_BINDING_PAIR(attr)	((attr)->attribute_value.SEQUENCE_BINDING_PAIR_field)
#define GET_SEQUENCE_BYTE(attr)	((attr)->attribute_value.SEQUENCE_BYTE_field)
#define GET_SEQUENCE_CONSTITUENT(attr)	((attr)->attribute_value.SEQUENCE_CONSTITUENT_field)
#define GET_SEQUENCE_COPYRIGHT_INFO(attr)	((attr)->attribute_value.SEQUENCE_COPYRIGHT_INFO_field)
#define GET_SEQUENCE_FONT_PAIR(attr)	((attr)->attribute_value.SEQUENCE_FONT_PAIR_field)
#define GET_SEQUENCE_INT_PAIR(attr)	((attr)->attribute_value.SEQUENCE_INT_PAIR_field)
#define GET_SEQUENCE_INT(attr)	((attr)->attribute_value.SEQUENCE_INT_field)
#define GET_SEQUENCE_NAME_ORG(attr)	((attr)->attribute_value.SEQUENCE_NAME_ORG_field)
#define GET_SEQUENCE_RESOURCE_MAP(attr)	((attr)->attribute_value.SEQUENCE_RESOURCE_MAP_field)
#define GET_SEQUENCE_REVISION_HISTORY(attr)	((attr)->attribute_value.SEQUENCE_REVISION_HISTORY_field)
#define GET_SEQUENCE_STRING_OR_ID(attr)	((attr)->attribute_value.SEQUENCE_STRING_OR_ID_field)
#define GET_SEQUENCE_STRING(attr)	((attr)->attribute_value.SEQUENCE_STRING_field)
#define GET_SEQUENCE_TAB_STOP(attr)	((attr)->attribute_value.SEQUENCE_TAB_STOP_field)
#define GET_SEQUENCE_VALUE(attr)	((attr)->attribute_value.SEQUENCE_VALUE_field)
#define GET_STRING_EXPR(attr)	((attr)->attribute_value.STRING_EXPR_field)
#define GET_STRING_OR_ID(attr)	((attr)->attribute_value.STRING_OR_ID_field)
#define GET_STRING(attr)	((attr)->attribute_value.STRING_field)
#define GET_VALUE(attr)	(*(attr)->attribute_value.VALUE_field)

#endif
