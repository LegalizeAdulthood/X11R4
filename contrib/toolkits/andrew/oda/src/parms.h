
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
 CMU-ITC-071) for full text & permitted uses.


	Include file defining attribute union type

    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.

*/

#ifndef TK_INCLUDED_PARMS
#define TK_INCLUDED_PARMS

#define NO_PARMS	((PARM_MASK_type) (1<<0))
#define ALL_PARMS	((PARM_MASK_type) (1<<1))

    /* at_POSITION */
#define HORIZONTAL_POSITION_parm	((PARM_MASK_type)1<<2)
#define VERTICAL_POSITION_parm	((PARM_MASK_type)1<<3)
#define POS_LEADING_OFFSET_parm	((PARM_MASK_type)1<<4)
#define POS_TRAILING_OFFSET_parm	((PARM_MASK_type)1<<5)
#define POS_RIGHT_HAND_OFFSET_parm	((PARM_MASK_type)1<<6)
#define POS_LEFT_HAND_OFFSET_parm	((PARM_MASK_type)1<<7)
#define POS_LEADING_EDGE_parm	((PARM_MASK_type)1<<8)
#define POS_TRAILING_EDGE_parm	((PARM_MASK_type)1<<9)
#define POS_CENTRE_SEPARATION_parm	((PARM_MASK_type)1<<10)
#define POS_ALIGNMENT_parm	((PARM_MASK_type)1<<11)
#define POS_FILL_ORDER_parm	((PARM_MASK_type)1<<12)

    /* at_DIMENSIONS */
#define HORIZONTAL_DIMENSION_parm	((PARM_MASK_type)1<<2)
#define VERTICAL_DIMENSION_parm	((PARM_MASK_type)1<<3)

    /* at_BORDER */
#define LEFT_HAND_EDGE_parm	((PARM_MASK_type)1<<2)
#define RIGHT_HAND_EDGE_parm	((PARM_MASK_type)1<<3)
#define BORDER_TRAILING_EDGE_parm	((PARM_MASK_type)1<<4)
#define BORDER_LEADING_EDGE_parm	((PARM_MASK_type)1<<5)

    /* at_MEDIUM_TYPE */
#define NOMINAL_PAGE_SIZE_parm	((PARM_MASK_type)1<<2)
#define SIDE_OF_SHEET_parm	((PARM_MASK_type)1<<3)

    /* at_OFFSET */
#define LEADING_OFFSET_parm	((PARM_MASK_type)1<<2)
#define TRAILING_OFFSET_parm	((PARM_MASK_type)1<<3)
#define LEFT_HAND_OFFSET_parm	((PARM_MASK_type)1<<4)
#define RIGHT_HAND_OFFSET_parm	((PARM_MASK_type)1<<5)

    /* at_SEPARATION */
#define LEADING_EDGE_parm	((PARM_MASK_type)1<<2)
#define TRAILING_EDGE_parm	((PARM_MASK_type)1<<3)
#define CENTRE_SEPARATION_parm	((PARM_MASK_type)1<<4)

    /* rg_CLIPPING */
#define COORD1_parm	((PARM_MASK_type)1<<2)
#define COORD2_parm	((PARM_MASK_type)1<<3)

#endif
