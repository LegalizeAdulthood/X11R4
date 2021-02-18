
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
 CMU-ITC-071) for full text & permitted uses.


	Include file defining distinguished values

    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.

*/

#ifndef TK_INCLUDED_VALUES
#define TK_INCLUDED_VALUES

    /* at_OBJECT_TYPE */
#define at_OBJECT_TYPE_doc_layout_root	((VALUE_type) 1)
#define at_OBJECT_TYPE_page_set	((VALUE_type) 2)
#define at_OBJECT_TYPE_page	((VALUE_type) 3)
#define at_OBJECT_TYPE_frame	((VALUE_type) 4)
#define at_OBJECT_TYPE_block	((VALUE_type) 5)
#define at_OBJECT_TYPE_doc_logical_root	((VALUE_type) 6)
#define at_OBJECT_TYPE_comp_logical_obj	((VALUE_type) 7)
#define at_OBJECT_TYPE_bas_logical_obj	((VALUE_type) 8)

    /* at_CONTENT_TYPE */
#define at_CONTENT_TYPE_form_rgca	((VALUE_type) 9)

    /* at_POSITION */
#define at_POSITION_right_hand_aligned	((VALUE_type) 10)
#define at_POSITION_centred	((VALUE_type) 11)
#define at_POSITION_left_hand_aligned	((VALUE_type) 12)
#define at_POSITION_normal_order	((VALUE_type) 13)
#define at_POSITION_reverse_order	((VALUE_type) 14)

    /* at_BORDER */
#define at_BORDER_solid	((VALUE_type) 15)
#define at_BORDER_dashed	((VALUE_type) 16)
#define at_BORDER_dot	((VALUE_type) 17)
#define at_BORDER_dash_dot	((VALUE_type) 18)
#define at_BORDER_dash_dot_dot	((VALUE_type) 19)
#define at_BORDER_invisible	((VALUE_type) 20)

    /* at_LAYOUT_PATH */
#define at_LAYOUT_PATH_type_0	((VALUE_type) 21)
#define at_LAYOUT_PATH_type_90	((VALUE_type) 22)
#define at_LAYOUT_PATH_type_180	((VALUE_type) 23)
#define at_LAYOUT_PATH_type_270	((VALUE_type) 24)

    /* at_TRANSPARENCY */
#define at_TRANSPARENCY_transparent	((VALUE_type) 25)
#define at_TRANSPARENCY_opaque	((VALUE_type) 26)

    /* at_COLOUR */
#define at_COLOUR_colourless	((VALUE_type) 27)
#define at_COLOUR_white	((VALUE_type) 28)

    /* at_MEDIUM_TYPE */
#define at_MEDIUM_TYPE_recto	((VALUE_type) 29)
#define at_MEDIUM_TYPE_verso	((VALUE_type) 30)
#define at_MEDIUM_TYPE_unspecified	((VALUE_type) 31)

    /* at_PROTECTION */
#define at_PROTECTION_protected	((VALUE_type) 32)
#define at_PROTECTION_unprotected	((VALUE_type) 33)

    /* at_BLK_ALGNM */
#define at_BLK_ALGNM_right_hand_aligned	((VALUE_type) 34)
#define at_BLK_ALGNM_left_hand_aligned	((VALUE_type) 35)
#define at_BLK_ALGNM_centred	((VALUE_type) 36)
#define at_BLK_ALGNM_null	((VALUE_type) 37)

    /* at_CONCATENATION */
#define at_CONCATENATION_concatenated	((VALUE_type) 38)
#define at_CONCATENATION_non_concated	((VALUE_type) 39)

    /* at_FILL_ORDER */
#define at_FILL_ORDER_normal_order	((VALUE_type) 40)
#define at_FILL_ORDER_reverse_order	((VALUE_type) 41)

    /* dp_GENERIC_LAY_STRUCT */
#define dp_GENERIC_LAY_STRUCT_fact_set	((VALUE_type) 42)
#define dp_GENERIC_LAY_STRUCT_part_gen	((VALUE_type) 43)
#define dp_GENERIC_LAY_STRUCT_comp_gen	((VALUE_type) 44)

    /* dp_SPECIFIC_LAY_STRUCT */
#define dp_SPECIFIC_LAY_STRUCT_present	((VALUE_type) 45)

    /* dp_GENERIC_LOG_STRUCT */
#define dp_GENERIC_LOG_STRUCT_fact_set	((VALUE_type) 46)
#define dp_GENERIC_LOG_STRUCT_part_gen	((VALUE_type) 47)
#define dp_GENERIC_LOG_STRUCT_comp_gen	((VALUE_type) 48)

    /* dp_SPECIFIC_LOG_STRUCT */
#define dp_SPECIFIC_LOG_STRUCT_present	((VALUE_type) 49)

    /* dp_LAYOUT_STYLES */
#define dp_LAYOUT_STYLES_present	((VALUE_type) 50)

    /* dp_PRESENTATION_STYLES */
#define dp_PRESENTATION_STYLES_present	((VALUE_type) 51)

    /* dp_DOC_ARCH_CLASS */
#define dp_DOC_ARCH_CLASS_formatted	((VALUE_type) 52)
#define dp_DOC_ARCH_CLASS_processable	((VALUE_type) 53)
#define dp_DOC_ARCH_CLASS_fp	((VALUE_type) 54)

    /* dp_INTERCHANGE_FMT_CLASS */
#define dp_INTERCHANGE_FMT_CLASS_a	((VALUE_type) 55)
#define dp_INTERCHANGE_FMT_CLASS_b	((VALUE_type) 56)

    /* cc_ALIGNMENT */
#define cc_ALIGNMENT_start_aligned	((VALUE_type) 57)
#define cc_ALIGNMENT_end_aligned	((VALUE_type) 58)
#define cc_ALIGNMENT_centred	((VALUE_type) 59)
#define cc_ALIGNMENT_justified	((VALUE_type) 60)

    /* cc_CHAR_ORIENTATION */
#define cc_CHAR_ORIENTATION_0	((VALUE_type) 61)
#define cc_CHAR_ORIENTATION_90	((VALUE_type) 62)
#define cc_CHAR_ORIENTATION_180	((VALUE_type) 63)
#define cc_CHAR_ORIENTATION_270	((VALUE_type) 64)

    /* cc_CHARACTER_PATH */
#define cc_CHARACTER_PATH_0	((VALUE_type) 65)
#define cc_CHARACTER_PATH_90	((VALUE_type) 66)
#define cc_CHARACTER_PATH_180	((VALUE_type) 67)
#define cc_CHARACTER_PATH_270	((VALUE_type) 68)

    /* cc_ITEMISATION */
#define cc_ITEMISATION_no_itemisation	((VALUE_type) 69)
#define cc_ITEMISATION_start_aligned	((VALUE_type) 70)
#define cc_ITEMISATION_end_aligned	((VALUE_type) 71)

    /* cc_LINE_LAYOUT_TABLE */
#define cc_LINE_LAYOUT_TABLE_start_alnd	((VALUE_type) 72)
#define cc_LINE_LAYOUT_TABLE_end_alnd	((VALUE_type) 73)
#define cc_LINE_LAYOUT_TABLE_centred	((VALUE_type) 74)
#define cc_LINE_LAYOUT_TABLE_alnd_arnd	((VALUE_type) 75)

    /* cc_LINE_PROGRESSION */
#define cc_LINE_PROGRESSION_90	((VALUE_type) 76)
#define cc_LINE_PROGRESSION_270	((VALUE_type) 77)

    /* cc_PAIRWISE_KERNING */
#define cc_PAIRWISE_KERNING_yes	((VALUE_type) 78)
#define cc_PAIRWISE_KERNING_no	((VALUE_type) 79)

    /* cc_FORMATTING_INDICATOR */
#define cc_FORMATTING_INDICATOR_yes	((VALUE_type) 80)
#define cc_FORMATTING_INDICATOR_no	((VALUE_type) 81)

    /* cc_PROP_LINE_SPACING */
#define cc_PROP_LINE_SPACING_yes	((VALUE_type) 82)
#define cc_PROP_LINE_SPACING_no	((VALUE_type) 83)

    /* rg_LINE_PROGRESSION */
#define rg_LINE_PROGRESSION_90	((VALUE_type) 84)
#define rg_LINE_PROGRESSION_270	((VALUE_type) 85)

    /* rg_PEL_PATH */
#define rg_PEL_PATH_0	((VALUE_type) 86)
#define rg_PEL_PATH_90	((VALUE_type) 87)
#define rg_PEL_PATH_180	((VALUE_type) 88)
#define rg_PEL_PATH_270	((VALUE_type) 89)

    /* rg_PEL_TRANS_DENSITY */
#define rg_PEL_TRANS_DENSITY_1	((VALUE_type) 90)
#define rg_PEL_TRANS_DENSITY_2	((VALUE_type) 91)
#define rg_PEL_TRANS_DENSITY_3	((VALUE_type) 92)
#define rg_PEL_TRANS_DENSITY_4	((VALUE_type) 93)
#define rg_PEL_TRANS_DENSITY_5	((VALUE_type) 94)
#define rg_PEL_TRANS_DENSITY_6	((VALUE_type) 95)

    /* rg_IMAGE_DIMENSIONS */
#define rg_IMAGE_DIMENSIONS_fixed	((VALUE_type) 96)
#define rg_IMAGE_DIMENSIONS_variable	((VALUE_type) 97)

    /* rg_COMPRESSION */
#define rg_COMPRESSION_compressed	((VALUE_type) 98)
#define rg_COMPRESSION_uncompressed	((VALUE_type) 99)

#endif
