
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report CMU-ITC-071)
 for full text & permitted uses.


    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.

*/

#ifndef TK_INCLUDED_APITYP
#define TK_INCLUDED_APITYP

#include <lcsite.h>
#include <const.h>

typedef struct {

    TAG_type factor_tag;   /* OBJECT_CLASS_ID_tag, EXPR_tag */

    union {

        CONSTITUENT			    object_class_id;
        struct CONSTRUCTION_EXPR_struct    *expr;
    } factor_value;

} CONSTRUCTION_FACTOR_type;

typedef struct {

    TAG_type		    term_tag;
        /* REQ_tag, OPT_tag, REP_tag, OPT_REP_tag */

    CONSTRUCTION_FACTOR_type    factor;
} CONSTRUCTION_TERM_type;

typedef struct {
    INT_type   first, second;
} INT_PAIR_type;

typedef struct {
    INT_PAIR_type   nominal_page_size;
    VALUE_type    side_of_sheet;
} at_MEDIUM_TYPE_type;

typedef struct SEQUENCE_struct {

    POINTER_type    private_info;

    TAG_type			      sequence_tag;

    INT_type    length;

    union {
        struct SEQUENCE_struct    **asn1_object_ids;
        struct ATOMIC_STRING_EXPR_struct    *atomic_string_exprs;
        struct BINDING_PAIR_struct    *binding_pairs;
        BYTE_type    *bytes;
        CHAR_type    *chars;
        CONSTITUENT    *constituents;
        CONSTRUCTION_TERM_type    *construction_terms;
        struct COPYRIGHT_INFO_struct    *copyright_infos;
        struct FONT_PAIR_struct    *font_pairs;
        INT_type    *ints;
        INT_PAIR_type    *int_pairs;
        struct ISO9541_ID_struct    *iso9541_ids;
        at_MEDIUM_TYPE_type    *medium_types;
        struct NAME_ORG_struct    *name_orgs;
        struct PERSONAL_NAME_struct    *personal_names;
        struct RESOURCE_MAP_struct    *resource_maps;
        struct REVISION_HISTORY_struct    *revision_histories;
        struct REVISER_struct    *revisers;
        struct SEQUENCE_struct    **strings;
        struct STRING_OR_ID_struct    *string_or_ids;
        struct TAB_STOP_struct    *tab_stops;
        VALUE_type    *values;
    } sequence_value;

} *SEQUENCE_type;

typedef SEQUENCE_type SEQUENCE_INT_type;

typedef SEQUENCE_INT_type ASN1_OBJECT_ID_type;

typedef SEQUENCE_type STRING_type;

typedef struct {

    TAG_type parm1_tag;
        /* OBJECT_CLASS_ID_tag, OBJECT_TYPE_tag */

    union {
        CONSTITUENT object_class_id;
        VALUE_type    object_type;
    } parm1;

    struct OBJECT_REF_struct *parm2;
} CURR_INSTANCE_PARMS_type;

typedef struct OBJECT_ID_EXPR_struct {

    TAG_type object_selection_function_tag;
        /*
            CURRENT_OBJECT_tag, CURRENT_INSTANCE_tag,
            SUPERIOR_OBJECT_tag, PRECEDING_OBJECT_tag
        */

    union {
        CURR_INSTANCE_PARMS_type		curr_inst_parms;
        struct OBJECT_ID_EXPR_struct    *sup_or_pre_parms;
    } parameters;

} OBJECT_ID_EXPR_type;

typedef struct OBJECT_REF_struct {

    TAG_type obj_ref_tag;
        /* OBJECT_ID_tag, OBJECT_ID_EXPR_tag */

    union {
        CONSTITUENT		    object_id;
        OBJECT_ID_EXPR_type    object_id_expr;
            /* Section @Ref(Object-Id-Expressions) */
    } object_ref_value;

} OBJECT_REF_type;

typedef struct {
    OBJECT_REF_type object_ref;
    INT_type    binding_identifier;
} REF_BINDING_VALUE_type;

typedef struct {

    TAG_type function_tag;
        /* INCREMENT_tag, DECREMENT_tag, ORDINAL_tag */

    union {
        struct NUMERIC_EXPR_struct	    *incr_decr_parameter;
        OBJECT_REF_type    ord_parameter;
    } numeric_function_value;

} NUMERIC_FUNCTION_type;

typedef struct NUMERIC_EXPR_struct {

    TAG_type numeric_expr_tag;
        /*
            NUMERIC_LITERAL_tag, REF_BINDING_VALUE_tag,
            NUMERIC_FUNCTION_tag
        */

    union {
        INT_type			numeric_literal;
        REF_BINDING_VALUE_type    binding_value;
        NUMERIC_FUNCTION_type    function;
    } numeric_expr_value;

} NUMERIC_EXPR_type;

typedef struct {

    TAG_type		function_tag;
        /*
            MAKE_STRING_tag, UPPER_ALPHA_tag, LOWER_ALPHA_tag,
            UPPER_ROMAN_tag, LOWER_ROMAN_tag
        */

    NUMERIC_EXPR_type    arg;

} STRING_FUNCTION_type;

typedef struct ATOMIC_STRING_EXPR_struct {

    TAG_type atomic_string_expr_tag;
        /*
            STRING_LITERAL_tag, REF_BINDING_VALUE_tag,
            STRING_FUNCTION_tag
        */

    union {
        STRING_type		    string;
        REF_BINDING_VALUE_type    value;
        STRING_FUNCTION_type    function;
    } atomic_string_value;

} ATOMIC_STRING_EXPR_type;

typedef SEQUENCE_type SEQUENCE_ATOMIC_STR_EXPR_type;

typedef SEQUENCE_ATOMIC_STR_EXPR_type STRING_EXPR_type;

typedef struct {
    TAG_type    binding_pair_tag;
        /*
            STRING_EXPR_tag, NUMERIC_EXPR_tag,
            OBJECT_ID_EXPR_tag, INTEGER_tag,
            STRING_tag, CONSTITUENT_tag
        */
    union {
        STRING_EXPR_type	    string_expr;
        NUMERIC_EXPR_type    numeric_expr;
        OBJECT_ID_EXPR_type    object_id_expr;
        INT_type    integer;
        STRING_type    string;
        CONSTITUENT    constituent;
    } binding_pair_value;
} BINDING_VALUE_type;

typedef struct BINDING_PAIR_struct {
    STRING_type	    binding_identifier;
    BINDING_VALUE_type    value;
} BINDING_PAIR_type;

typedef struct {
    BOOL_type  is_null;
    INT_type    border_line_width;
    VALUE_type    border_line_type;
    INT_type    border_freespace_width;
} BORDER_INFO_type;

typedef SEQUENCE_type SEQUENCE_CONSTRUCTION_TERM_type;

typedef struct CONSTRUCTION_EXPR_struct {

    TAG_type construction_tag;
        /* TERM_tag, SEQUENCE_tag, AGGREGATE_tag, CHOICE_tag */

    union {

        CONSTRUCTION_TERM_type	    term;
        SEQUENCE_CONSTRUCTION_TERM_type    construction;
            /* Applies to SEQUENCE_tag, AGGREGATE_tag & CHOICE_tag */
    } construction_value;

} CONSTRUCTION_EXPR_type;

typedef SEQUENCE_type SEQUENCE_STRING_type;

typedef struct COPYRIGHT_INFO_struct {
    SEQUENCE_STRING_type	copyright_information;
    SEQUENCE_STRING_type    copyright_dates;
} COPYRIGHT_INFO_type;

typedef struct {
    INT_type   minimum_dimension;
    INT_type    maximum_dimension;
} RULE_type;

typedef struct {
    TAG_type	dimensions_tag;
        /*
            FIXED_DIMENSIONS_tag, VARIABLE_PAGE_HEIGHT_tag,
            RULE_A_tag, RULE_B_tag, MAX_SIZE_tag
        */
    union {
        INT_type    fixed_dimension;
        INT_type    variable_page_height;
        RULE_type    rule;
    } dimensions_body;
} DIMENSION_type;

typedef struct {
    INT_type	horizontal_position;
    INT_type    vertical_position;
} FIXED_POSITION_type;

typedef SEQUENCE_type SEQUENCE_ISO9541_ID_type;

typedef SEQUENCE_type SEQUENCE_BYTE_type;

typedef struct ISO9541_ID_struct {
    TAG_type    iso9541_id_tag;
    /* ID_INT_tag, ID_STR_tag */
    union {
        INT_type    integer;
        SEQUENCE_BYTE_type    octet_string;
    } iso9541_id_value;
} ISO9541_ID_type;

typedef struct {
    TAG_type    structured_name_tag;
        /*
            STRUCTURED_NAME_ID_tag,
            STRUCTURED_NAME_SEQ_tag,
            STRUCTURED_NAME_UNSPEC_tag
        */
    union {
        ISO9541_ID_type    id;
        SEQUENCE_ISO9541_ID_type    idseq;
    } structured_name_value;
} STRUCTURED_NAME_type;

typedef struct {
    BOOL_type    specified;
    INT_PAIR_type    scorpos;
    INT_PAIR_type    scorthik;
} SCORE_type;

typedef struct {
    BOOL_type    specified;
    SEQUENCE_ISO9541_ID_type    dfnpre;	    /* Default name prefix */
    SCORE_type    rscore;
    SCORE_type    lscore;
    SCORE_type    tscore;
/* non-ISO-scores is not implemented */
} SCORES_type;

typedef struct {
    BOOL_type    specified;
    INT_PAIR_type    vxoffset;
    INT_PAIR_type    vyoffset;
    INT_PAIR_type    vxscale;
    INT_PAIR_type    vyscale;
} VARIANT_SCRIPT_type;

typedef struct {
    BOOL_type    specified;
    SEQUENCE_ISO9541_ID_type    dfnpre;	    /* Default name prefix */
    VARIANT_SCRIPT_type    lscript;
    VARIANT_SCRIPT_type    rscript;
/* non-ISO-scripts is not unimplemented */
} VARIANT_SCRIPTS_type;

typedef struct {
    BOOL_type    specified;
    SEQUENCE_ISO9541_ID_type    dfnpre;	    /* Default name prefix */
    INT_PAIR_type    escapdir;
    INT_type    esclass;
    INT_PAIR_type    avgesc;
    INT_PAIR_type    avglcesc;
    INT_PAIR_type    avgcpesc;
    INT_PAIR_type    avgwtesc;
    struct tabesc_struct {
        BOOL_type    specified;
        INT_PAIR_type    tx;
        INT_PAIR_type    ty;
    } tabesc;
    INT_PAIR_type    mfx;
    INT_PAIR_type    mlx;
    INT_PAIR_type    mbx;
    INT_PAIR_type    mrx;
/* ISO/CLPESCAD is unimplemented */
    INT_PAIR_type    minampl;
    INT_PAIR_type    maxampl;
/* ISO/TRACKS & ISO/TRACK are not implemented */
    SCORES_type    scores;
    VARIANT_SCRIPTS_type    varscrpt;
    INT_PAIR_type    minl2lsp;
    INT_PAIR_type    minanasc;
    INT_PAIR_type    maxanasc;
/* ISO/MODCHATT is not implemented */
/* The non-ISO-attributes is not implemented */
} MODAL_FONT_ATTR_type;

typedef struct {
    BOOL_type    specified;
    SEQUENCE_ISO9541_ID_type    dfnpre;	    /* Default name prefix */
    MODAL_FONT_ATTR_type    iso_0;
    MODAL_FONT_ATTR_type    iso_180;
    MODAL_FONT_ATTR_type    iso_270;
/* non-ISO-modes not implemented */
} WRITING_MODES_type;

typedef struct {
    SEQUENCE_ISO9541_ID_type	  dfnpre;    /* Default name prefix */
    STRUCTURED_NAME_type    fontname;
    STRUCTURED_NAME_type    difntnam;
    INT_type    propdata;
    STRUCTURED_NAME_type    typeface;
    STRUCTURED_NAME_type    family;
    INT_type    posture;
    INT_type    weight;
    INT_type    propwdth;
/* ISO/CHARCOMP (& its sub-fields) is unimplemented */
    INT_type    numchars;
/* The field ISO/LIGATURS is unimplemented */
    INT_PAIR_type    dsnsize;
    INT_PAIR_type    minsize;
    INT_PAIR_type    maxsize;
    SEQUENCE_INT_type    dsclass;
    INT_type    structur;
    INT_PAIR_type    mnfeatsz;
    WRITING_MODES_type    modes;
    STRUCTURED_NAME_type    mode;
/* ISO/TECHS is unimplemented */
} FONT_REFERENCE_type;

typedef struct FONT_PAIR_struct {
    INT_type		font_id;
    FONT_REFERENCE_type    font_reference;
} FONT_PAIR_type;

typedef struct {
    INT_type	    minimum_width;
    INT_type    preferred_width;
    INT_type    minimum_height;
    INT_type    preferred_height;
    VALUE_type    aspect_ratio_flag;
} IMAGE_SIZE_type;

typedef struct {
    BOOL_type		automatic_label;
    INT_type    error_od;
    BOOL_type    format_errors;
    BOOL_type    print_warnings;
    INT_type    (*output_filter)();
} INIT_PARMS_type;

typedef struct {
    TAG_type		layout_object_tag;
        /*
            LAYOUT_OBJ_DESC_tag, LAYOUT_CATEGORY_tag,
            OBJECT_TYPE_PAGE_tag, NULL_tag
        */
    union {
        STRING_type    layout_category;
        CONSTITUENT    constituent;
    } layout_object_value;
} LAYOUT_OBJECT_type;

typedef struct PERSONAL_NAME_struct {
    SEQUENCE_BYTE_type	surname;
    SEQUENCE_BYTE_type    givenname;
    SEQUENCE_BYTE_type    initials;
    SEQUENCE_BYTE_type    title;
} PERSONAL_NAME_type;

typedef struct NAME_ORG_struct {
    BOOL_type			name_is_present;
    PERSONAL_NAME_type    name;
    STRING_type    organization;
} NAME_ORG_type;

typedef struct RESOURCE_MAP_struct {
    STRING_type	name;
    CONSTITUENT    id;
} RESOURCE_MAP_type;

typedef SEQUENCE_type SEQUENCE_PERSONAL_NAME_type;

typedef struct REVISER_struct {
    SEQUENCE_PERSONAL_NAME_type	names;
    SEQUENCE_BYTE_type    position;
    SEQUENCE_BYTE_type    organization;
} REVISER_type;

typedef SEQUENCE_type SEQUENCE_REVISER_type;

typedef struct STRING_OR_ID_struct {
    TAG_type		string_or_id_tag;
        /* STRING_tag, ID_tag */
    union {
        STRING_type	    string;
        ASN1_OBJECT_ID_type    id;
    } string_or_id_value;
} STRING_OR_ID_type;

typedef struct REVISION_HISTORY_struct {
    STRING_type		revision_date_and_time;
    STRING_type    version_number;
    SEQUENCE_REVISER_type    revisers;
    STRING_OR_ID_type    version_reference;
    SEQUENCE_BYTE_type    user_comments;
} REVISION_HISTORY_type;

typedef SEQUENCE_type SEQUENCE_ASN1_OBJECT_ID_type;

typedef SEQUENCE_type SEQUENCE_BINDING_PAIR_type;

typedef SEQUENCE_type SEQUENCE_CHAR_type;

typedef SEQUENCE_type SEQUENCE_CONSTITUENT_type;

typedef SEQUENCE_type SEQUENCE_COPYRIGHT_INFO_type;

typedef SEQUENCE_type SEQUENCE_FONT_PAIR_type;

typedef SEQUENCE_type SEQUENCE_INT_PAIR_type;

typedef SEQUENCE_type SEQUENCE_NAME_ORG_type;

typedef SEQUENCE_type SEQUENCE_RESOURCE_MAP_type;

typedef SEQUENCE_type SEQUENCE_REVISION_HISTORY_type;

typedef SEQUENCE_type SEQUENCE_STRING_OR_ID_type;

typedef SEQUENCE_type SEQUENCE_TAB_STOP_type;

typedef SEQUENCE_type SEQUENCE_VALUE_type;

typedef SEQUENCE_type SEQUENCE_at_MEDIUM_TYPE_type;

typedef struct TAB_STOP_struct {
    STRING_type   tab_reference;
    INT_type    tab_position;
    VALUE_type    alignment;
    STRING_type    alignment_string;
} TAB_STOP_type;

typedef struct {
    struct {
        INT_type    leading_offset;
        INT_type    trailing_offset;
        INT_type    left_hand_offset;
        INT_type    right_hand_offset;
    } offset;
    struct {
        INT_type    leading_edge;
        INT_type    trailing_edge;
        INT_type    centre_separation;
    } separation;
    VALUE_type    alignment;
    VALUE_type    fill_order;
} VARIABLE_POSITION_type;

typedef STRING_type at_ALT_REP_type;

typedef SEQUENCE_BYTE_type at_APPLICATION_COMMENTS_type;

typedef SEQUENCE_CONSTITUENT_type at_BALANCE_type;

typedef SEQUENCE_BINDING_PAIR_type at_BINDINGS_type;

typedef VALUE_type at_BLK_ALGNM_type;

typedef struct {
    BORDER_INFO_type    left_hand_edge;
    BORDER_INFO_type    right_hand_edge;
    BORDER_INFO_type    trailing_edge;
    BORDER_INFO_type    leading_edge;
} at_BORDER_type;

typedef VALUE_type at_COLOUR_type;

typedef VALUE_type at_CONCATENATION_type;

typedef ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_type;

typedef STRING_EXPR_type at_CONTENT_GENERATOR_type;

typedef STRING_type at_CONTENT_ID_LAYOUT_type;

typedef STRING_type at_CONTENT_ID_LOGICAL_type;

typedef SEQUENCE_BYTE_type at_CONTENT_INFORMATION_type;

typedef SEQUENCE_CONSTITUENT_type at_CONTENT_PORTIONS_type;

typedef VALUE_type at_CONTENT_TYPE_type;

typedef struct {
    CONSTITUENT		page_attributes;
    CONSTITUENT    frame_attributes;
    CONSTITUENT    block_attributes;
    CONSTITUENT    composite_logical_attributes;
    CONSTITUENT    basic_logical_attributes;
} at_DEFAULT_VALUE_LISTS_type;

typedef struct {
    BOOL_type		horizontal_dimension_is_null;
    DIMENSION_type    horizontal_dimension;
    BOOL_type    vertical_dimension_is_null;
    DIMENSION_type    vertical_dimension;
} at_DIMENSIONS_type;

typedef VALUE_type at_FILL_ORDER_type;

typedef CONSTRUCTION_EXPR_type at_GEN_FOR_SUBORDINATES_type;

typedef SEQUENCE_CONSTITUENT_type at_IMAGING_ORDER_type;

typedef LAYOUT_OBJECT_type at_INDIVISIBILITY_type;

typedef STRING_type at_LAYOUT_CATEGORY_type;

typedef CONSTITUENT at_LAYOUT_OBJECT_CLASS_type;

typedef VALUE_type at_LAYOUT_PATH_type;

typedef STRING_type at_LAYOUT_STYLE_IDENTIFIER_type;

typedef CONSTITUENT at_LAYOUT_STYLE_type;

typedef CONSTITUENT at_LOGICAL_SOURCE_type;

typedef LAYOUT_OBJECT_type at_NEW_LAYOUT_OBJECT_type;

typedef STRING_type at_OBJECT_CLASS_IDENTIFIER_type;

typedef CONSTITUENT at_OBJECT_CLASS_type;

typedef STRING_type at_OBJECT_IDENTIFIER_type;

typedef VALUE_type at_OBJECT_TYPE_type;

typedef struct {
    INT_type    leading_offset;
    INT_type    trailing_offset;
    INT_type    left_hand_offset;
    INT_type    right_hand_offset;
} at_OFFSET_type;

typedef INT_PAIR_type at_PAGE_POSITION_type;

typedef SEQUENCE_STRING_type at_PERMITTED_CATEGORIES_type;

typedef struct {
    TAG_type		    position_tag;
        /*
            FIXED_POSITION_tag, VARIABLE_POSITION_tag
        */
    union {
        FIXED_POSITION_type    fixed_position;
        VARIABLE_POSITION_type    variable_position;
    } position_body;
} at_POSITION_type;

typedef STRING_type at_PRESENTATION_STYLE_ID_type;

typedef CONSTITUENT at_PRESENTATION_STYLE_type;

typedef VALUE_type at_PROTECTION_type;

typedef STRING_type at_RESOURCE_type;

typedef struct {
    CONSTITUENT			parm1;
    LAYOUT_OBJECT_type    parm2;
} at_SAME_LAYOUT_OBJECT_type;

typedef struct {
    INT_type    leading_edge;
    INT_type    trailing_edge;
    INT_type    centre_separation;
} at_SEPARATION_type;

typedef SEQUENCE_CONSTITUENT_type at_SUBORDINATES_type;

typedef CONSTITUENT at_SYNCHRONIZATION_type;

typedef VALUE_type at_TRANSPARENCY_type;

typedef ASN1_OBJECT_ID_type at_TYPE_OF_CODING_type;

typedef STRING_type at_USER_READABLE_COMMENTS_type;

typedef STRING_type at_USER_VISIBLE_NAME_type;

typedef INT_type cc_1ST_LINE_OFFSET_type;

typedef VALUE_type cc_ALIGNMENT_type;

typedef SEQUENCE_INT_PAIR_type cc_CHARACTER_FONTS_type;

typedef VALUE_type cc_CHARACTER_PATH_type;

typedef INT_type cc_CHARACTER_SPACING_type;

typedef VALUE_type cc_CHAR_ORIENTATION_type;

typedef SEQUENCE_BYTE_type cc_CODE_EXT_ANNOUNCERS_type;

typedef VALUE_type cc_FORMATTING_INDICATOR_type;

typedef SEQUENCE_BYTE_type cc_GRAPHIC_CHARACTER_SETS_type;

typedef INT_type cc_GRAPHIC_CHAR_SUBREP_type;

typedef SEQUENCE_INT_type cc_GRAPHIC_RENDITION_type;

typedef INT_type cc_INDENTATION_type;

typedef INT_PAIR_type cc_INITIAL_OFFSET_type;

typedef struct {
    VALUE_type	identifier_alignment;
    INT_type    identifier_start_offset;
    INT_type    identifier_end_offset;
} cc_ITEMISATION_type;

typedef INT_PAIR_type cc_KERNING_OFFSET_type;

typedef SEQUENCE_TAB_STOP_type cc_LINE_LAYOUT_TABLE_type;

typedef VALUE_type cc_LINE_PROGRESSION_type;

typedef INT_type cc_LINE_SPACING_type;

typedef INT_type cc_ORPHAN_SIZE_type;

typedef VALUE_type cc_PAIRWISE_KERNING_type;

typedef VALUE_type cc_PROP_LINE_SPACING_type;

typedef INT_type cc_WIDOW_SIZE_type;

typedef STRING_type dp_ABSTRACT_type;

typedef SEQUENCE_STRING_type dp_ACCESS_RIGHTS_type;

typedef SEQUENCE_BYTE_type dp_ADDITIONAL_INFORMATION_type;

typedef SEQUENCE_BYTE_type dp_ALT_REP_CHARACTER_SETS_type;

typedef struct {
    TAG_type		authorization_tag;
        /* NAME_tag, ORG_tag */

    union {
        PERSONAL_NAME_type    name;
        STRING_type    organization;
    } authorization_value;

} dp_AUTHORIZATION_type;

typedef SEQUENCE_NAME_ORG_type dp_AUTHORS_type;

typedef SEQUENCE_VALUE_type dp_BLOCK_ALIGNMENTS_type;

typedef SEQUENCE_VALUE_type dp_BORDERS_type;

typedef SEQUENCE_CONSTITUENT_type dp_CODING_ATTRIBUTES_type;

typedef SEQUENCE_VALUE_type dp_COLOURS_type;

typedef SEQUENCE_BYTE_type dp_COMMENTS_CHARACTER_SETS_type;

typedef SEQUENCE_ASN1_OBJECT_ID_type dp_CONTENT_ARCH_CLASSES_type;

typedef SEQUENCE_COPYRIGHT_INFO_type dp_COPYRIGHT_type;

typedef STRING_type dp_CREATION_DATE_AND_TIME_type;

typedef SEQUENCE_NAME_ORG_type dp_DISTRIBUTION_LIST_type;

typedef STRING_type dp_DOCUMENT_DATE_AND_TIME_type;

typedef STRING_OR_ID_type dp_DOCUMENT_REFERENCE_type;

typedef INT_type dp_DOCUMENT_SIZE_type;

typedef STRING_type dp_DOCUMENT_TYPE_type;

typedef ASN1_OBJECT_ID_type dp_DOC_APP_PROFILE_type;

typedef struct {
    ASN1_OBJECT_ID_type	    content_arch_class;
    BOOL_type    content_type_specified;
    VALUE_type    content_type;
    BOOL_type    dimensions_specified;
    PARM_MASK_type    dimensions_parm_mask;
    at_DIMENSIONS_type    dimensions;
    BOOL_type    transparency_specified;
    VALUE_type    transparency;
    BOOL_type    colour_specified;
    VALUE_type    colour;
    BOOL_type    border_specified;
    PARM_MASK_type    border_parm_mask;
    at_BORDER_type    border;
    BOOL_type    layout_path_specified;
    VALUE_type    layout_path;
    BOOL_type    page_position_specified;
    INT_PAIR_type    page_position;
    BOOL_type    medium_type_specified;
    PARM_MASK_type    medium_type_parm_mask;
    at_MEDIUM_TYPE_type    medium_type;
    BOOL_type    block_alignment_specified;
    VALUE_type    block_alignment;
    ASN1_OBJECT_ID_type    type_of_coding;
    CONSTITUENT    cc_presentation;
    CONSTITUENT    cc_coding;
    CONSTITUENT    rg_presentation;
    CONSTITUENT    rg_coding;
    CONSTITUENT    gg_presentation;
    CONSTITUENT    gg_coding;
} dp_DOC_APP_PROF_DEFAULTS_type;

typedef VALUE_type dp_DOC_ARCH_CLASS_type;

typedef STRING_type dp_EXPIRY_DATE_AND_TIME_type;

typedef STRING_OR_ID_type dp_EXTERNAL_DOCUMENT_CLASS_type;

typedef SEQUENCE_VALUE_type dp_FILL_ORDERS_type;

typedef SEQUENCE_FONT_PAIR_type dp_FONTS_LIST_type;

typedef VALUE_type dp_GENERIC_LAY_STRUCT_type;

typedef VALUE_type dp_GENERIC_LOG_STRUCT_type;

typedef VALUE_type dp_INTERCHANGE_FMT_CLASS_type;

typedef SEQUENCE_STRING_type dp_LANGUAGES_type; 

typedef SEQUENCE_VALUE_type dp_LAYOUT_PATHS_type;

typedef VALUE_type dp_LAYOUT_STYLES_type;

typedef struct {
    STRING_type	filename;
    STRING_type    location;
    STRING_type    user_comments;
} dp_LOCAL_FILE_REFERENCE_type;

typedef STRING_type dp_LOCAL_FILING_DATE_TIME_type;

typedef SEQUENCE_at_MEDIUM_TYPE_type dp_MEDIUM_TYPES_type;

typedef INT_type dp_NUMBER_OBJECTS_PER_PAGE_type;

typedef INT_type dp_NUMBER_OF_PAGES_type;

typedef struct {
    STRING_type	standard;
    STRING_type    date;
} dp_ODA_VERSION_type;

typedef SEQUENCE_STRING_type dp_ORGANIZATIONS_type;

typedef SEQUENCE_NAME_ORG_type dp_OWNERS_type;

typedef SEQUENCE_INT_PAIR_type dp_PAGE_DIMENSIONS_type;

typedef SEQUENCE_INT_PAIR_type dp_PAGE_POSITIONS_type;

typedef SEQUENCE_NAME_ORG_type dp_PREPARERS_type;

typedef SEQUENCE_CONSTITUENT_type dp_PRESENTATION_FEATURES_type;

typedef VALUE_type dp_PRESENTATION_STYLES_type;

typedef SEQUENCE_BYTE_type dp_PROFILE_CHARACTER_SETS_type;

typedef SEQUENCE_VALUE_type dp_PROTECTIONS_type;

typedef STRING_type dp_PURGE_DATE_AND_TIME_type;

typedef SEQUENCE_STRING_OR_ID_type dp_REFS_TO_OTHER_DOCS_type;

typedef STRING_type dp_RELEASE_DATE_AND_TIME_type;

typedef SEQUENCE_RESOURCE_MAP_type dp_RESOURCES_type;

typedef STRING_OR_ID_type dp_RESOURCE_DOCUMENT_type;

typedef SEQUENCE_REVISION_HISTORY_type dp_REVISION_HISTORY_type;

typedef STRING_type dp_SECURITY_CLASSIFICATION_type;

typedef VALUE_type dp_SPECIFIC_LAY_STRUCT_type;

typedef VALUE_type dp_SPECIFIC_LOG_STRUCT_type;

typedef STRING_type dp_START_DATE_AND_TIME_type;

typedef STRING_type dp_STATUS_type; 

typedef STRING_type dp_SUBJECT_type;

typedef SEQUENCE_STRING_OR_ID_type dp_SUPERSEDED_DOCUMENTS_type;

typedef STRING_type dp_TITLE_type;

typedef SEQUENCE_VALUE_type dp_TRANSPARENCIES_type;

typedef SEQUENCE_ASN1_OBJECT_ID_type dp_TYPES_OF_CODING_type;

typedef INT_PAIR_type dp_UNIT_SCALING_type;

typedef SEQUENCE_STRING_type dp_USER_SPECIFIC_CODES_type;

typedef struct {
    INT_PAIR_type coord1, coord2;
} rg_CLIPPING_type;

typedef VALUE_type rg_COMPRESSION_type;

typedef struct {
    TAG_type  image_dimensions_tag;
        /*
            WIDTH_CONTROLLED_tag, HEIGHT_CONTROLLED_tag,
            AREA_CONTROLLED_tag, AUTOMATIC_tag
        */
    union {
        INT_PAIR_type	width_controlled;
        INT_PAIR_type    height_controlled;
        IMAGE_SIZE_type    area_controlled;
    } image_dimensions_value;
} rg_IMAGE_DIMENSIONS_type;

typedef INT_PAIR_type rg_INITIAL_OFFSET_type;

typedef VALUE_type rg_LINE_PROGRESSION_type;

typedef INT_type rg_NUMBER_DISCARDED_PELS_type;

typedef INT_type rg_NUMBER_OF_LINES_type;

typedef INT_type rg_NUMBER_OF_PELS_PER_LINE_type;

typedef VALUE_type rg_PEL_PATH_type;

typedef struct {
    BOOL_type	    null;
    INT_type    length;
    INT_type    pel_spaces;
} rg_PEL_SPACING_type;

typedef VALUE_type rg_PEL_TRANS_DENSITY_type;

typedef INT_PAIR_type rg_SPACING_RATIO_type;

#endif
