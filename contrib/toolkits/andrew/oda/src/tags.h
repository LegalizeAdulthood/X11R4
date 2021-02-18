
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tags.h,v 1.1 89/09/21 06:40:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tags.h,v $

		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report CMU-ITC-071)
 for full text & permitted uses.


 Include file defining all of the tag constants used in
 variant records (union structures)
 
$Log:	tags.h,v $
 * Revision 1.1  89/09/21  06:40:43  mss
 * Initial revision
 * 
 * Revision 1.3  88/09/12  17:43:33  jr
 * Add unspec tag for fotn stuff.
 * 
 * Revision 1.2  88/09/06  13:22:22  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.1  88/07/15  15:38:24  mss
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_TAGS
#define TK_INCLUDED_TAGS


/* Tags for CONSTRUCTION_FACTOR_type */

#define	OBJECT_CLASS_ID_tag	((TAG_type) 51)
#define	EXPR_tag		((TAG_type) 52)

/* Tags for CONSTRUCTION_TERM)type */

#define	REQ_tag			((TAG_type) 53)
#define	OPT_tag			((TAG_type) 54)
#define	REP_tag			((TAG_type) 55)
#define	OPT_REP_tag		((TAG_type) 56)

/* Tags for CURR_INSTANCE_PARMS_type */

/* 	OBJECT_CLASS_ID_tag (51) */
#define	OBJECT_TYPE_tag		((TAG_type) 58)

/* Tags for OBJECT_ID_EXPR_type */

#define	CURRENT_OBJECT_tag	((TAG_type) 59)
#define	CURRENT_INSTANCE_tag	((TAG_type) 60)
#define	SUPERIOR_OBJECT_tag	((TAG_type) 61)
#define	PRECEDING_OBJECT_tag	((TAG_type) 62)

/* Tags for OBJECT_REF_type */

#define	OBJECT_ID_tag		((TAG_type) 63)
#define	OBJECT_ID_EXPR_tag	((TAG_type) 64)

/* Tags for NUMERIC_FUNCTION_type */

#define	INCREMENT_tag		((TAG_type) 65)
#define	DECREMENT_tag		((TAG_type) 66)
#define	ORDINAL_tag		((TAG_type) 67)

/* Tags for NUMERIC_EXPR_type */

#define	NUMERIC_LITERAL_tag	((TAG_type) 68)
#define	REF_BINDING_VALUE_tag	((TAG_type) 69)
#define	NUMERIC_FUNCTION_tag	((TAG_type) 70)

/* Tags for STRING_FUNCTION_type */

#define	MAKE_STRING_tag		((TAG_type) 71)
#define	UPPER_ALPHA_tag		((TAG_type) 72)
#define	LOWER_ALPHA_tag		((TAG_type) 73)
#define	UPPER_ROMAN_tag		((TAG_type) 74)
#define	LOWER_ROMAN_tag		((TAG_type) 75)

/* Tags for ATOMIC_STRING_EXPR_type */

#define	STRING_LITERAL_tag	((TAG_type) 76)
/*	REF_BINDING_VALUE_tag (69) */
#define	STRING_FUNCTION_tag	((TAG_type) 78)

/* Tags for BINDING_VALUE_type */

#define	STRING_EXPR_tag		((TAG_type) 79)
#define	NUMERIC_EXPR_tag	((TAG_type) 80)
/*	OBJECT_ID_EXPR_tag  (64) */
#define	INTEGER_tag		((TAG_type) 82)
#define	STRING_tag		((TAG_type) 83)
#define	CONSTITUENT_tag		((TAG_type) 84)

/* Tags for CONSTRUCTION_EXPR_type */

#define	TERM_tag		((TAG_type) 85)
#define	SEQUENCE_tag		((TAG_type) 86)
#define	AGGREGATE_tag		((TAG_type) 87)
#define	CHOICE_tag		((TAG_type) 88)

/* Tags for DIMENSION_type */

#define	FIXED_DIMENSIONS_tag	((TAG_type) 89)
#define	VARIABLE_PAGE_HEIGHT_tag ((TAG_type)90)
#define	RULE_A_tag		((TAG_type) 91)
#define	RULE_B_tag		((TAG_type) 92)
#define	MAX_SIZE_tag		((TAG_type) 93)

/* Tags for ISO9541_ID_type */

#define	ID_INT_tag		((TAG_type) 94)
#define	ID_STR_tag		((TAG_type) 95)

/* Tags for STRUCTURE_NAME_type */

#define	STRUCTURED_NAME_ID_tag	((TAG_type) 96)
#define	STRUCTURED_NAME_SEQ_tag	((TAG_type) 97)
#define	STRUCTURED_NAME_UNSPEC_tag	((TAG_type) 98)

/* Tags for LAYOUT_OBJ_type */

#define	LAYOUT_OBJ_DESC_tag	((TAG_type) 99)
#define	LAYOUT_CATEGORY_tag	((TAG_type) 100)
#define	OBJECT_TYPE_PAGE_tag	((TAG_type) 101)
#define	NULL_tag		((TAG_type) 102)

/* Tags for STRING_OR_ID_type */

/*	STRING_tag (83) */
#define	ID_tag			((TAG_type) 103)

/* Tags for at_POSITION_type */

#define	FIXED_POSITION_tag	((TAG_type) 104)
#define	VARIABLE_POSITION_tag	((TAG_type) 105)

/* Tags for dp_AUTHORIZATION_type */

#define	NAME_tag		((TAG_type) 106)
#define	ORG_tag			((TAG_type) 107)

/* Tags for rg_IMAGE_DIMENSIONS_type */

#define	WIDTH_CONTROLLED_tag	((TAG_type) 108)
#define	HEIGHT_CONTROLLED_tag	((TAG_type) 109)
#define	AREA_CONTROLLED_tag	((TAG_type) 110)
#define	AUTOMATIC_tag		((TAG_type) 111)

#endif
