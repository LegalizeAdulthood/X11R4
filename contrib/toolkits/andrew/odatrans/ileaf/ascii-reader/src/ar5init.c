
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	ar5init.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original coding
 */

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"
#include "ar5private.h"

/**--------------------------------------------------------------------------**/
bool ARdebug = NO;	    /** used globally.    for development only.   BT **/
/**--------------------------------------------------------------------------**/
/** for historical reasons, I2R takes an integer argument measuring hundreths**/
/** of an inch, while RSUMM takes a float directly expressing a number of    **/
/** millimeters.  BT 							     **/
/**									     **/
/** #define I2R(f)   ((Rsu)(((f) * DEF_RSUINCH) / 100.0)) BT **/
#define RSUMM(x) ((Rsu)(((x) * DEF_RSUINCH) / MM_PER_INCH ))

Il_state_t *AR_il_stateP = NULL;
Cmpn_t  AR_prev_cmpn;
Cmpn_t *AR_prev_cmpnP = NULL;
NullObj_t *AR_prev_objectP = NULL;

Cmpn_t default_cmpn = 
{
    INIT_NullObjFields(ID_Cmpn),	/**	id, nextP, spareP	**/
    NULL,				/**	unkP	**/
    {'p','a','r','a',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  /**cmpn_name**/
    NULL,				/**	attrvalsP	**/
    tok_BUF_PRIVATE,			/**	content_type	**/
    {NULL, NULL, NULL, NULL}, 		/**	contents	**/
    NO,					/**	hidden 	**/
    NO,					/**	subcmpn	**/
    {
	0,					/**	font_token	**/
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/**	font_inherit	**/
	{ 0,0,0,0,0,0,YES,0,0,0,clr_BLACK,0 }   /**	font_attr	**/
    },
    0,					/**    rsu top_margin;	**/
    I2R(14),				/**    rsu bottom_margin;	**/
    0,					/**    rsu left_margin;	**/
    0,					/**    rsu right_margin;	**/
    0,					/**    rsu first_indent;	**/
    I2R(131),				/**    rsu line_spacing;	**/
    0,					/**	indent_count	**/
    {					/**    Tabline_t tabs;	**/
      AR_MAX_TABS,
      {
	{ I2R(000),  tab_LEFT },{ I2R( 75),  tab_LEFT },{ I2R(150),  tab_LEFT },
	{ I2R(225),  tab_LEFT },{ I2R(300),  tab_LEFT },{ I2R(375),  tab_LEFT },
	{ I2R(450),  tab_LEFT },{ I2R(525),  tab_LEFT },{ I2R(600),  tab_LEFT },
	{ I2R(675),  tab_LEFT },{ I2R(750),  tab_LEFT },{ I2R(825),  tab_LEFT },
	{ I2R(900),  tab_LEFT },{ I2R(975),  tab_LEFT },{ I2R(1025), tab_LEFT },
	{ I2R(1125), tab_LEFT },{ I2R(1200), tab_LEFT },{ I2R(1275), tab_LEFT },
	{ I2R(1350), tab_LEFT },{ I2R(1425), tab_LEFT },{ I2R(1500), tab_LEFT },
	{ I2R(1575), tab_LEFT },{ I2R(1650), tab_LEFT },{ I2R(1725), tab_LEFT },
	{ I2R(1800), tab_LEFT },{ I2R(1875), tab_LEFT },{ I2R(1950), tab_LEFT },
	{ I2R(2025), tab_LEFT },{ I2R(2100), tab_LEFT },{ I2R(2175), tab_LEFT }
      }
    },
    d_BOTH,		/**    Direction_t alignment;	**/
    NO,			/**    bool att_next;    	**/
    YES,		/**    bool pbreak_within;	**/
    NO,			/**    bool att_prior;   	**/
    fill_ON,		/**    fill_e fill;	**/
    NO, 		/**	new_page	**/
    NO,			/**	new_column	**/
    2,			/**    short widow_ctrl;	**/
    2,			/**    short orphan_ctrl;	**/
    NO,			/**    bool straddle;	**/
    0,			/**	an_level	**/
    NO, 		/**	kern_space	**/
#if 0	/** BT **/
    0,			/**	kern_track	**/
#endif	/** BT **/
    NO, 		/**	word_spacing	**/
    1.56, 		/**	word_sp_max	**/
    0.56, 		/**	word_sp_min	**/
    1.0, 		/**	word_sp_nom	**/
    NO, 		/**	letter_spacing	**/
    0.25, 		/**	letter_sp_max	**/
    FSPC_OVR,		/**	composition	**/
    AR_HYPHEN_DEFAULT,	/**    short hyphenation;	**/
    NULL, 		/**	profileP	**/
    			/**    shar toc_doc_name[AR_ANTYPSIZ];	**/
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

Cmpn_t default_metric_cmpn = 
{
    INIT_NullObjFields(ID_Cmpn),	/**	id, nextP, spareP	**/
    NULL,				/**	unkP	**/
    {'p','a','r','a',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  /**cmpn_name**/
    NULL,				/**	attrvalsP	**/
    tok_BUF_PRIVATE,			/**	content_type	**/
    {NULL, NULL, NULL, NULL}, 		/**	contents	**/
    NO,					/**	hidden 	**/
    NO,					/**	subcmpn	**/
    {
	0,					/**	font_token	**/
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/**	font_inherit	**/
	{ 0,0,0,0,0,0,YES,0,0,0,clr_BLACK,0 }   /**	font_attr	**/
    },
    0,					/**    rsu top_margin;	**/
    RSUMM(3.5),				/**    rsu bottom_margin;	**/
    0,					/**    rsu left_margin;	**/
    0,					/**    rsu right_margin;	**/
    0,					/**    rsu first_indent;	**/
    I2R(131),				/**    rsu line_spacing;	**/
    0,					/**	indent_count	**/
    {					/**    Tabline_t tabs;	**/
      AR_MAX_TABS,
      {
	{ I2R(000),  tab_LEFT },{ I2R( 75),  tab_LEFT },{ I2R(150),  tab_LEFT },
	{ I2R(225),  tab_LEFT },{ I2R(300),  tab_LEFT },{ I2R(375),  tab_LEFT },
	{ I2R(450),  tab_LEFT },{ I2R(525),  tab_LEFT },{ I2R(600),  tab_LEFT },
	{ I2R(675),  tab_LEFT },{ I2R(750),  tab_LEFT },{ I2R(825),  tab_LEFT },
	{ I2R(900),  tab_LEFT },{ I2R(975),  tab_LEFT },{ I2R(1025), tab_LEFT },
	{ I2R(1125), tab_LEFT },{ I2R(1200), tab_LEFT },{ I2R(1275), tab_LEFT },
	{ I2R(1350), tab_LEFT },{ I2R(1425), tab_LEFT },{ I2R(1500), tab_LEFT },
	{ I2R(1575), tab_LEFT },{ I2R(1650), tab_LEFT },{ I2R(1725), tab_LEFT },
	{ I2R(1800), tab_LEFT },{ I2R(1875), tab_LEFT },{ I2R(1950), tab_LEFT },
	{ I2R(2025), tab_LEFT },{ I2R(2100), tab_LEFT },{ I2R(2175), tab_LEFT }
      }
    },
    d_BOTH,		/**    Direction_t alignment;	**/
    NO,			/**    bool att_next;    	**/
    YES,		/**    bool pbreak_within;	**/
    NO,			/**    bool att_prior;   	**/
    fill_ON,		/**    fill_e fill;	**/
    NO, 		/**	new_page	**/
    NO,			/**	new_column	**/
    2,			/**    short widow_ctrl;	**/
    2,			/**    short orphan_ctrl;	**/
    NO,			/**    bool straddle;	**/
    0,			/**	an_level	**/
    NO, 		/**	kern_space	**/
#if 0	/** BT **/
    0,			/**	kern_track	**/
#endif	/** BT **/
    NO, 		/**	word_spacing	**/
    1.56, 		/**	word_sp_max	**/
    0.56, 		/**	word_sp_min	**/
    1.0, 		/**	word_sp_nom	**/
    NO, 		/**	letter_spacing	**/
    0.25, 		/**	letter_sp_max	**/
    FSPC_OVR,		/**	composition	**/
    AR_HYPHEN_DEFAULT,	/**    short hyphenation;	**/
    NULL, 		/**	profileP	**/
    			/**    shar toc_doc_name[AR_ANTYPSIZ];	**/
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

/** selects metric/non-metric **/
Cmpn_t *active_default_cmpnP = &default_cmpn;

/**--------------------------------------------------------------------------**/
Cmpn_t output_cmpn = 		          /** to minimize output properties. **/
{
    INIT_NullObjFields(ID_Cmpn),	/**	id, nextP, spareP	**/
    NULL,				/**	unkP	**/
    {'p','a','r','a',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  /**cmpn_name**/
    NULL,				/**	attrvalsP	**/
    tok_BUF_PRIVATE,			/**	content_type	**/
    {NULL, NULL, NULL, NULL}, 		/**	contents	**/
    NO,					/**	hidden 	**/
    NO,					/**	subcmpn	**/
    {
	0,					/**	font_token	**/
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/**	font_inherit	**/
	{ 0,0,0,0,0,0,YES,0,0,0,clr_BLACK,0 }   /**	font_attr	**/
    },
    0,					/**    rsu top_margin;	**/
    I2R(14),				/**    rsu bottom_margin;	**/
    0,					/**    rsu left_margin;	**/
    0,					/**    rsu right_margin;	**/
    0,					/**    rsu first_indent;	**/
    I2R(131),				/**    rsu line_spacing;	**/
    0,					/**	indent_count	**/
    {					/**    Tabline_t tabs;	**/
      AR_MAX_TABS,
      {
	{ I2R(000),  tab_LEFT },{ I2R( 75),  tab_LEFT },{ I2R(150),  tab_LEFT },
	{ I2R(225),  tab_LEFT },{ I2R(300),  tab_LEFT },{ I2R(375),  tab_LEFT },
	{ I2R(450),  tab_LEFT },{ I2R(525),  tab_LEFT },{ I2R(600),  tab_LEFT },
	{ I2R(675),  tab_LEFT },{ I2R(750),  tab_LEFT },{ I2R(825),  tab_LEFT },
	{ I2R(900),  tab_LEFT },{ I2R(975),  tab_LEFT },{ I2R(1025), tab_LEFT },
	{ I2R(1125), tab_LEFT },{ I2R(1200), tab_LEFT },{ I2R(1275), tab_LEFT },
	{ I2R(1350), tab_LEFT },{ I2R(1425), tab_LEFT },{ I2R(1500), tab_LEFT },
	{ I2R(1575), tab_LEFT },{ I2R(1650), tab_LEFT },{ I2R(1725), tab_LEFT },
	{ I2R(1800), tab_LEFT },{ I2R(1875), tab_LEFT },{ I2R(1950), tab_LEFT },
	{ I2R(2025), tab_LEFT },{ I2R(2100), tab_LEFT },{ I2R(2175), tab_LEFT }
      }
    },
    d_BOTH,		/**    Direction_t alignment;	**/
    NO,			/**    bool att_next;    	**/
    YES,		/**    bool pbreak_within;	**/
    NO,			/**    bool att_prior;   	**/
    fill_ON,		/**    fill_e fill;	**/
    NO, 		/**	new_page	**/
    NO,			/**	new_column	**/
    2,			/**    short widow_ctrl;	**/
    2,			/**    short orphan_ctrl;	**/
    NO,			/**    bool straddle;	**/
    0,			/**	an_level	**/
    NO, 		/**	kern_space	**/
#if 0	/** BT **/
    0,			/**	kern_track	**/
#endif	/** BT **/
    NO, 		/**	word_spacing	**/
    1.56, 		/**	word_sp_max	**/
    0.56, 		/**	word_sp_min	**/
    1.0, 		/**	word_sp_nom	**/
    NO, 		/**	letter_spacing	**/
    0.25, 		/**	letter_sp_max	**/
    FSPC_OVR,		/**	composition	**/
    AR_HYPHEN_DEFAULT,	/**    short hyphenation;	**/
    NULL, 		/**	profileP	**/
    			/**    shar toc_doc_name[AR_ANTYPSIZ];	**/
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


/**--------------------------------------------------------------------------**/
/**	Table defaults.							     **/
/**--------------------------------------------------------------------------**/
Table_rule_t default_table_rule =
{
    1,			/** width				**/
    linepat_SOLID,	/** pattern (dashed, solid, etc.)	**/
    clr_BLACK,		/** color				**/
    NO,			/** double_rule				**/
    YES			/** visible				**/
};

Table_border_t default_table_border =
{
    { 1, linepat_SOLID,	clr_BLACK, NO, YES },
    { 1, linepat_SOLID,	clr_BLACK, NO, YES },
    { 1, linepat_SOLID,	clr_BLACK, NO, YES },
    { 1, linepat_SOLID,	clr_BLACK, NO, YES }
};

TL_ruling_t default_TL_ruling =
{
    { 1, linepat_SOLID,	clr_BLACK, NO, YES },
    { 1, linepat_SOLID,	clr_BLACK, NO, YES }
};

Cell_t default_table_cell =
{
    INIT_NullObjFields(ID_Cell),		/** id, nextP, spareP	**/
    NULL,					/** unkP	**/
    NULL,					/** autosizeP	**/
    0,						/** height	**/
    0,						/** width	**/
    YES,					/** auto_edit	**/
    1,						/** straddle	**/
    fr_V_TOP,					/** v_align	**/
    {						/** rulings	**/
	{ 1, linepat_SOLID, clr_BLACK, NO, YES },	/** top		**/
	{ 1, linepat_SOLID, clr_BLACK, NO, YES }	/** left	**/
    },
    NULL,					/** cellbodyP	**/
};

Row_t default_table_row =
{
    INIT_NullObjFields(ID_Row),			/** id, nextP, spareP	**/
    NULL,					/** row_nameP		**/
    NULL,					/** unkP		**/
    NULL,					/** contentsP		**/
    { 						/** flags		**/
	NO,						/** hidden	**/
	NO,						/** new_page	**/
	NO,						/** new_column	**/
	YES,						/** pbreak_after**/
	NO,						/** is_header	**/
	NO,						/** is_footer	**/
	NO,						/** A_page	**/
    },
    0,						/** top_margin		**/
    0,						/** bottom_margin	**/
    { 						/** font		**/
	0,						/** font_token	**/
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },		/** font_inherit**/
	{ 0,0,0,0,0,0,YES,0,0,0,clr_BLACK,0 }   	/** font_attr	**/
    },
    NULL,					/** attrvalsP		**/
    {						/** text_content	**/
	NULL, NULL, NULL, NULL
    }
};

#define DEF_RULING	{ 1, linepat_SOLID, clr_BLACK, NO, YES }

Table_column_t default_table_column =
{
    NULL,		/** nextP	**/
    1,			/** num_column	**/
    DEF_RSUINCH,	/** width	**/
    DEF_RULING,		/** top		**/
    DEF_RULING,		/** left	**/
};

Table_t default_table =
{
    INIT_NullObjFields(ID_Table),			/** id, nextP, spareP**/
    NULL,						/** table_nameP	     **/
    NULL,						/** unkP	     **/
    0,							/** num_columns	     **/
    NULL,						/** column_infoP     **/
    NULL,						/** master_rowsP     **/
    NULL,						/** create_rowsP     **/
    NULL,						/** content_rowsP    **/
    {							/** flags	     **/
	NO,						  /** new_page	     **/
	NO,						  /** new_column     **/
	YES,						  /** pbreak_within  **/
	YES,						  /** pbreak_after   **/
	NO,						  /** straddle	     **/
	YES,						  /** pbreak_rulings **/
	NO,						  /** A_page	     **/
    },
    I2R(  8),						/** top_margin	     **/
    I2R(  8),						/** bottom_margin    **/
    I2R(  0),						/** left_margin	     **/
    I2R(  0),						/** right_margin     **/
    2,							/** widow_control    **/
    2,							/** orphan_control   **/
    {							/** border	     **/
	DEF_RULING,					  /** top	     **/
	DEF_RULING,					  /** left	     **/
	DEF_RULING,					  /** bottom	     **/
	DEF_RULING					  /** right	     **/
    },
    DEF_RULING,						/** header_rule	     **/
    DEF_RULING,						/** footer_rule	     **/
};

/**--------------------------------------------------------------------------**/
/**	Document-state defaults.					     **/
/**--------------------------------------------------------------------------**/
Il_state_t default_il_state =
{
    ID_State,	/** id **/
    {		/** ops_info **/
	ID_OpsDecl,
	NULL,	/** unkP **/
	ARversion,
	OPS_STYLE_DEFAULT
    },
    {		/** doc_info **/
	ID_Document,
	NULL,		/** unknown property list, unkP **/
	{		/** document flags. **/
	    YES,	/** header_page **/	
	    NO,		/** double_sided **/	
	    NO,		/** manual_feed **/	
	    YES,	/** print_rev **/	
	    YES,	/** print_strikes **/	
	    YES,	/** print_underlines **/	
	    NO,		/** underline_at_descender **/	
	    NO		/** orient_inverted **/	
	},
	/** final_output_dev **/	
	{ 'c', 'x', 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	},
	/** def_printer **/	
	{       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	},
	tok_INCHES,		/** measure_unit **/	
	tok_ARABIC,		/** A_Pg_style **/	
				/** A_Pg_prefix **/	
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
	AR_APAGE_ANY		/** max_page **/	
    },
    NULL,	/** font_defnP **/
    NULL,	/** colorP **/
    NULL,	/** patternP **/
    {		/** page_info **/
	ID_Page,
	NULL,	/** unkP **/
	{   /** flags **/
	    NO,			/** bleed **/	
	    YES,		/** hyphenation **/	
	    YES,		/** bal_columns **/	
	    YES,		/** vert_just **/	
	    YES,		/** feathering **/	
	    YES,		/** vert_just_pages **/	
	    NO,			/** frozen_autonums **/
	    YES,                /** pgnum absolute **/
	    REV_BAR_LEFT,	/** rev_bar_place **/
	    PG_PARITY_DEFAULT,	/** page_parity **/
	    TURNED_0,		/** turned_pages **/
	},
	NO,		    /** micro_props_only	**/
	1,		    /** columns			**/
	I2R( 33),	    /**	gutter			**/
	I2R(1100),	    /**	height			**/
	I2R(850),	    /**	width			**/
	I2R(100),	    /**	top_margin		**/
	I2R(110),	    /**	bottom_margin		**/
	I2R(140),	    /**	left_margin		**/
	I2R(140),	    /**	right_margin		**/
	I2R(100),	    /**	inner_margin		**/
	I2R(100),	    /**	outer_margin		**/
	d_ANY,		    /**	first_page		**/
	MARGIN_NORMAL,	    /** margins			**/
	START_PG_ANY,       /**	start_page_num		**/
			    /** pgnum_prefix 		**/
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	tok_ARABIC,	    /**	pgnum_style		**/
	0,		    /** ladder_count		**/
	200,	            /**	margin_stretch		**/
	50,	    	    /**	margin_shrink		**/
	10,	    	    /**	frame_margin_stretch	**/
	8,	    	    /**	max_feathering		**/
	95,	    	    /**	depth_pbreak		**/
	90	    	    /**	depth_no_pbreak		**/
    },
    NULL,	/** an_streamP **/
    NULL,	/** class_defaultsP **/
    NULL,	/** cmpnP **/
    NULL,	/** frameP **/
    NULL,	/** tableP **/
    NULL	/** hdftP **/
};
 
Page_info_t default_metric_page_info = 
{
	ID_Page,
	NULL,	/** unkP **/
	{   /** flags **/
	    NO,			/** bleed **/	
	    YES,		/** hyphenation **/	
	    YES,		/** bal_columns **/	
	    YES,		/** vert_just **/	
	    YES,		/** feathering **/	
	    YES,		/** vert_just_pages **/
	    NO,			/** frozen_autonums **/	
	    YES,                /** pgnum_absolute **/
	    REV_BAR_LEFT,	/** rev_bar_place **/
	    PG_PARITY_DEFAULT,	/** page_parity **/
	    TURNED_0,		/** turned_pages **/
	},
	NO,		    /** micro_props_only	**/
	1,		    /** columns			**/
	I2R( 33),	    /**	gutter			**/
	I2R(1100),	    /**	height			**/
	I2R(850),	    /**	width			**/
	I2R(100),	    /**	top_margin		**/
	I2R(110),	    /**	bottom_margin		**/
	I2R(140),	    /**	left_margin		**/
	I2R(140),	    /**	right_margin		**/
	I2R(100),	    /**	inner_margin		**/
	I2R(100),	    /**	outer_margin		**/
	d_ANY,		    /**	first_page		**/
	MARGIN_NORMAL,	    /** margins			**/
	START_PG_ANY,       /**	start_page_num		**/
			    /** pgnum_prefix 		**/
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	tok_ARABIC,	    /**	pgnum_style		**/
	0,		    /** ladder_count		**/
	200,	            /**	margin_stretch		**/
	50,	    	    /**	margin_shrink		**/
	10,	    	    /**	frame_margin_stretch	**/
	8,	    	    /**	max_feathering		**/
	95,	    	    /**	depth_pbreak		**/
	90	    	    /**	depth_no_pbreak		**/
};

Page_info_t default_micro_page_info = 
{
	ID_Page,
	NULL,	/** unkP **/
	{   /** flags **/
	    NO,			/** bleed **/	
	    YES,		/** hyphenation **/	
	    YES,		/** bal_columns **/	
	    YES,		/** vert_just **/	
	    YES,		/** feathering **/	
	    YES,		/** vert_just_pages **/	
	    NO,			/** frozen_autonums **/
	    YES,                /** pgnum absolute **/
	    REV_BAR_LEFT,	/** rev_bar_place **/
	    PG_PARITY_DEFAULT,	/** page_parity **/
	    TURNED_0,		/** turned_pages **/
	},
	YES,		    /** micro_props_only	**/
	1,		    /** columns			**/
	I2R( 33),	    /**	gutter			**/
	I2R(1100),	    /**	height			**/
	I2R(850),	    /**	width			**/
	I2R(  3),	    /**	top_margin		**/
	I2R(  3),	    /**	bottom_margin		**/
	I2R(000),	    /**	left_margin		**/
	I2R(000),	    /**	right_margin		**/
	I2R(000),	    /**	inner_margin		**/
	I2R(000),	    /**	outer_margin		**/
	d_ANY,		    /**	first_page		**/
	MARGIN_NORMAL,	    /** margins			**/
	START_PG_ANY,       /**	start_page_num		**/
			    /** pgnum_prefix 		**/
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	tok_ARABIC,	    /**	pgnum_style		**/
	0,		    /** ladder_count		**/
	200,	            /**	margin_stretch		**/
	50,	    	    /**	margin_shrink		**/
	10,	    	    /**	frame_margin_stretch	**/
	8,	    	    /**	max_feathering		**/
	95,	    	    /**	depth_pbreak		**/
	90	    	    /**	depth_no_pbreak		**/
};
Page_info_t *active_page_infoP = &default_il_state.page_info;

/**--------------------------------------------------------------------------**/
Index_t default_index_entry = 
{
    index_SCOPE_LINE,
    0,			/** count **/
    face_PLAIN,		/** typeface **/
    lang_DEFAULT,	/** dictionary **/
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /** see_string **/
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /** to_named **/
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /** doc	**/
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    { NULL, NULL, NULL, NULL, NULL, NULL },	/** sort_strings **/
    { NULL, NULL, NULL, NULL, NULL, NULL },	/** headings     **/
};

/**--------------------------------------------------------------------------**/
An_stream_info_t default_an_stream = 
{ 
    INIT_NullObjFields(ID_AutoStream),		/** id, nextP, spareP	**/
    NULL,					/** an_name (shar *) **/
    1,						/** an_levels	**/
    {
      { tok_ARABIC,			        /* an_symbol */
	{0},					/* an_prefix */
	{'.', 0},		         	/* an_suffix */
	1,		        		/* start_value */
	FALSE,					/* an_last_only */
	TRUE,					/* an_show */
	FALSE },				/* an_inherit */
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE },
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE },
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE },
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE },
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE },
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE },
      { tok_ARABIC, {0}, {'.', 0}, 1,	FALSE, TRUE, FALSE }
    }
};

/**--------------------------------------------------------------------------**/
Frame_t default_frame = 
{
    INIT_NullObjFields(ID_Frame),		/** id, nextP, spareP	**/
    NULL,	/** unkP	**/
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	/** name	**/
    {   /** flags **/
	fr_FIXED, 		/** fixed	**/
	NO,			/** shared	**/
	NO,			/** auto_edit	**/
	NO,			/** cell	**/
	fr_FOLLOW_ANCHOR,	/** placement (I made these up  BT)	**/
	fr_H_LEFT,		/** halign	**/
	fr_V_TOP,		/** valign	**/
	YES,			/** show_anchor	**/
	NO,			/** proportional	**/
	NO,			/** filler_unused	**/
	fr_X_PAGENONE,		/** x_locate	**/
	fr_Y_PAGENONE,		/** y_locate	**/
	fr_Z_NOTSET,		/** z_locate	**/
	fr_REP_NONE,		/** repeat_info	**/
	NO,			/** repeat-same_page	**/
	NO,			/** same_page	**/
	NO,			/** hidden	**/
	NO,			/** super_num	**/
	NO,			/* size contents to heigth */
	NO			/* size contents to width */
    },
    NULL,		/** page_numP	**/
    NULL,		/** autosizeP	**/
    NULL,		/** attrvalsP	**/
    0,                  /** a           **/
    DEF_RSUINCH,	/** height	**/
    2*DEF_RSUINCH,	/** width	**/
    {0,NULL},		/** diagram	**/
    NULL,		/** rulingsP	**/
    0,			/** tint	**/
    0,			/** xofs	**/
    0,			/** yofs	**/
    NULL,		/** anumP	**/
    1			/* straddle */
};

Autosize_t default_fr_autosize =
{
    ASZWFIX,			/** frame_width	**/
    ASZHFIX,			/** frame_heigh	**/
    0,				/** xmin	**/
    32767,			/** xmax	**/
    0,				/** ymin	**/
    32767,			/** ymax	**/
    AR_PAGE_DENOM,		/** xfrac	**/
    AR_PAGE_DENOM,		/** yfrac	**/
    0,				/** xadj	**/
    0				/** yadj	**/
};

Autosize_t default_fr_cell_autosize =
{
    ASZEXX,			/** frame_width	**/
    ASZINY,			/** frame_heigh	**/
    0,				/** xmin	**/
    32767,			/** xmax	**/
    0,				/** ymin	**/
    32767,			/** ymax	**/
    AR_PAGE_DENOM,		/** xfrac	**/
    AR_PAGE_DENOM,		/** yfrac	**/
    0,				/** xadj	**/
    0				/** yadj	**/
};

Frame_t default_fr_cell = 
{
    INIT_NullObjFields(ID_Frame),		/** id, nextP, spareP	**/
    NULL,	/** unkP	**/
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	/** name	**/
    {   /** flags **/
	fr_FIXED, 		/** fixed	**/
	NO,			/** shared	**/
	YES,			/** auto_edit	**/
	YES,			/** cell	**/
	fr_FOLLOW_ANCHOR,	/** placement (I made these up  BT)	**/
	fr_H_LEFT,		/** halign	**/
	fr_V_TOP,		/** valign	**/
	NO,			/** show_anchor	**/
	NO,			/** proportional	**/
	NO,			/** filler_unused	**/
	fr_X_PAGENONE,		/** x_locate	**/
	fr_Y_PAGENONE,		/** y_locate	**/
	fr_Z_NOTSET,		/** z_locate	**/
	fr_REP_NONE,		/** repeat_info	**/
	NO,			/** repeat-same_page	**/
	NO,			/** same_page	**/
	NO,			/** hidden	**/
	NO,			/** super_num	**/
	NO,			/* size contents to heigth */
	YES			/* size contents to width */
    },
    NULL,		/** page_numP	**/
    NULL,		/** autosizeP	**/
    NULL,		/** attrvalsP	**/
    0,                  /** a           **/
    DEF_RSUINCH,	/** height	**/
    2*DEF_RSUINCH,	/** width	**/
    {0, NULL},		/** diagram	**/
    NULL,		/** rulingsP	**/
    0,			/** tint	**/
    0,			/** xofs	**/
    0,			/** yofs	**/
    NULL,		/** anumP	**/
    0,                  /** straddle    **/
};

/**--------------------------------------------------------------------------**/
Ar_init_t AR_init_info = 
{
    2,				/** two entries. **/
    DEF_RSUINCH,		/** 900 rsu's per inch. **/
    YES				/** double '<<' in ArEmitChar() on output. **/
};

/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/

File_info_t *
ArFileInit(ifile)
  register FILE *ifile;
{
  register File_info_t *fiP    = NULL;
  static Ssave_t empty_inqueue = { NULL, NULL, NULL, NULL };

    if (ifile == NULL) {
	/* ArFprintf(stderr, "ArFileInit: NULL file pointer: get help.\n"); */
	return(NULL);
    }
    else if (getc(ifile) == AR_CMD_CHAR && getc(ifile) == AR_DECL_CHAR) {
	fiP          = NEW(File_info_t);
	fiP->id      = ID_FileInfo;
	fiP->ifile   = ifile;
	fiP->inqueue = empty_inqueue;
	ArUnGetch(AR_DECL_CHAR, fiP);
	ArUnGetch(AR_CMD_CHAR, fiP);
    }
    else {
	/* ArFprintf(stderr,  */
	    /* "ArFileInit: file not in I.A.M. format: get help.\n"); */
	return(NULL);
    }
    return( fiP );
}

/**--------------------------------------------------------------------------**/
void ArInit( initP )
  Ar_init_t *initP;
{
# define SETRSU(f)     ((double)(f)        * AR_init_info.client_rsu)
# define SETRSUMM(f) ((((double)(f))/25.4) * AR_init_info.client_rsu)
  extern float al_unit_factor[];
  static bool ARinitted = NO;

    if (ARinitted == NO) {
	  ARinitted = YES;
	  if (initP == NULL || initP->entries < 0) {
	      fprintf(stderr, "Bad parameter to ArInit(); get help.\n");
	      exit( -1 );
	  }
	  /** load up the AR control info. **/
	  switch(initP->entries) {
	  /** these cases all fall through to the bottom. **/
	  case 2:  AR_init_info.double_markup = initP->double_markup;
	  case 1:  AR_init_info.client_rsu    = initP->client_rsu;
	  case 0:
	  default:	break;
	  }
  	  if (AR_init_info.client_rsu > 0) {
	    /**fprintf(stderr,
		"RSU's per inch == %d\n",AR_init_info.client_rsu);
	    **/
	  }
  	  else {
	      AR_init_info.client_rsu = DEF_RSUINCH;
  	      fprintf(stderr, 
  	      "ArInit warning: bad rsu measure (%d). Using %d instead.\n",
  			      initP->client_rsu, DEF_RSUINCH);
  	  }
	  {
	    default_cmpn.tabs.num_stops = AR_MAX_TABS;
	    default_cmpn.tabs.entries[ 0].stop = SETRSU( 0.00);
	    default_cmpn.tabs.entries[ 1].stop = SETRSU( 0.75);
	    default_cmpn.tabs.entries[ 2].stop = SETRSU( 1.50);
	    default_cmpn.tabs.entries[ 3].stop = SETRSU( 2.25);
	    default_cmpn.tabs.entries[ 4].stop = SETRSU( 3.00);
	    default_cmpn.tabs.entries[ 5].stop = SETRSU( 3.75);
	    default_cmpn.tabs.entries[ 6].stop = SETRSU( 4.50);
	    default_cmpn.tabs.entries[ 7].stop = SETRSU( 5.25);
	    default_cmpn.tabs.entries[ 8].stop = SETRSU( 6.00);
	    default_cmpn.tabs.entries[ 9].stop = SETRSU( 6.75);
	    default_cmpn.tabs.entries[10].stop = SETRSU( 7.50);
	    default_cmpn.tabs.entries[11].stop = SETRSU( 8.25);
	    default_cmpn.tabs.entries[12].stop = SETRSU( 9.00);
	    default_cmpn.tabs.entries[13].stop = SETRSU( 9.75);
	    default_cmpn.tabs.entries[14].stop = SETRSU(10.25);
	    default_cmpn.tabs.entries[15].stop = SETRSU(11.25);
	    default_cmpn.tabs.entries[16].stop = SETRSU(12.00);
	    default_cmpn.tabs.entries[17].stop = SETRSU(12.75);
	    default_cmpn.tabs.entries[18].stop = SETRSU(13.50);
	    default_cmpn.tabs.entries[19].stop = SETRSU(14.25);
	    default_cmpn.tabs.entries[20].stop = SETRSU(15.00);
	    default_cmpn.tabs.entries[21].stop = SETRSU(15.75);
	    default_cmpn.tabs.entries[22].stop = SETRSU(16.50);
	    default_cmpn.tabs.entries[23].stop = SETRSU(17.25);
	    default_cmpn.tabs.entries[24].stop = SETRSU(18.00);
	    default_cmpn.tabs.entries[25].stop = SETRSU(18.75);
	    default_cmpn.tabs.entries[26].stop = SETRSU(19.50);
	    default_cmpn.tabs.entries[27].stop = SETRSU(20.25);
	    default_cmpn.tabs.entries[28].stop = SETRSU(21.00);
	    default_cmpn.tabs.entries[29].stop = SETRSU(21.75);
	}
	default_cmpn.bottom_margin = SETRSU(0.14);
	default_cmpn.line_spacing  = (rsu)(1.310 * AR_LINE_SCALING);
	/** now, default_cmpn is correct.  update the other global Cmpn_t's **/
	output_cmpn = default_cmpn;
	default_metric_cmpn = default_cmpn;
	default_metric_cmpn.bottom_margin = 
		RSUMM(3.5) * (AR_init_info.client_rsu/(double)DEF_RSUINCH);
	{
	    default_micro_page_info.gutter           = SETRSU( 0.33);
	    default_micro_page_info.height           = SETRSU(11.00);
	    default_micro_page_info.width            = SETRSU( 8.50);
	    default_micro_page_info.top_margin       = SETRSU( 0.03);
	    default_micro_page_info.bottom_margin    = SETRSU( 0.03);
	    default_micro_page_info.left_margin      = SETRSU( 0.00);
	    default_micro_page_info.right_margin     = SETRSU( 0.00);
	    default_micro_page_info.inner_margin     = SETRSU( 0.00);
	    default_micro_page_info.outer_margin     = SETRSU( 0.00);

	    default_il_state.page_info.gutter        = SETRSU( 0.33);
	    default_il_state.page_info.height        = SETRSU(11.00);
	    default_il_state.page_info.width         = SETRSU( 8.50);
	    default_il_state.page_info.top_margin    = SETRSU( 1.00);
	    default_il_state.page_info.bottom_margin = SETRSU( 1.10);
	    default_il_state.page_info.left_margin   = SETRSU( 1.40);
	    default_il_state.page_info.right_margin  = SETRSU( 1.40);
	    default_il_state.page_info.inner_margin  = SETRSU( 1.00);
	    default_il_state.page_info.outer_margin  = SETRSU( 1.00);

	    default_metric_page_info.gutter          = SETRSUMM(   8.0);
	    default_metric_page_info.height          = SETRSUMM( 297.0);
	    default_metric_page_info.width           = SETRSUMM( 210.0);
	    default_metric_page_info.top_margin      = SETRSUMM(  25.0);
	    default_metric_page_info.bottom_margin   = SETRSUMM(  28.0);
	    default_metric_page_info.left_margin     = SETRSUMM(  35.0);
	    default_metric_page_info.right_margin    = SETRSUMM(  35.0);
	    default_metric_page_info.inner_margin    = SETRSUMM(  25.0);
	    default_metric_page_info.outer_margin    = SETRSUMM(  25.0);
	}
	{
	    default_frame.height      = SETRSU(1.0);
	    default_frame.width       = SETRSU(1.0);

	    default_fr_cell.height = SETRSU(1.0);
	    default_fr_cell.width  = SETRSU(1.0);

	    default_table_row.top_margin    = SETRSU(0.03);
	    default_table_row.bottom_margin = SETRSU(0.03);

	    default_table.top_margin        = SETRSU(0.08);
	    default_table.bottom_margin     = SETRSU(0.08);

	    default_table_column.width	    = SETRSU(1.00);
	}
	{
	    al_unit_factor[0] = AR_init_info.client_rsu/LINE_PER_INCH;
	    al_unit_factor[1] = AR_init_info.client_rsu;
	    al_unit_factor[2] = AR_init_info.client_rsu/CM_PER_INCH;
	    al_unit_factor[3] = AR_init_info.client_rsu/MM_PER_INCH;
	    al_unit_factor[4] = AR_init_info.client_rsu/POINTS_PER_INCH;
	    al_unit_factor[5] = AR_init_info.client_rsu/PICAS_PER_INCH;
	    al_unit_factor[6] = AR_init_info.client_rsu/DIDOT_PER_INCH;
	    al_unit_factor[7] = AR_init_info.client_rsu/CICERO_PER_INCH;
	}
    }
}

/**--------------------------------------------------------------------------**/
Il_state_t *ArStateInit( stateP )
  register Il_state_t *stateP;
{
    if (stateP) {
	*stateP = default_il_state;
    }
    return( stateP );
}

/**--------------------------------------------------------------------------**/
Font_value_t ArFontValueInit( )
{
  static Font_value_t it = 
  {
	0,					/**	font_token	**/
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/**	font_inherit	**/
	{ 0,0,0,0,0,0,YES,0,0,0,clr_BLACK,0 }   /**	font_attr	**/
  };
    return( it );
}

/**--------------------------------------------------------------------------**/
Cmpn_t *ArCmpnInit( cmP )
  register Cmpn_t *cmP;
{
    if (cmP) { 
	*cmP = *active_default_cmpnP; 
	cmP->attrvalsP = NULL;		/** this is right.  BT **/
    }
    return( cmP );
}

/**--------------------------------------------------------------------------**/
void ArCmpnSetDefaults( cmP )
  register Cmpn_t *cmP;
{
    if (cmP) { 
	*active_default_cmpnP = *cmP; 
    }
}

/**--------------------------------------------------------------------------**/
void
ArPageInfoInit(pgP)
  Page_info_t *pgP;
{
    if (pgP) {
	*pgP = *active_page_infoP;
    }
}

/**--------------------------------------------------------------------------**/
void 
ArPageInfoSetContext( which )
  int which;
{
    if (which == pg_MICROCONTEXT) {
	active_page_infoP = &default_micro_page_info;
    }
    else if (which == pg_MACROCONTEXT) {
	active_page_infoP = &default_il_state.page_info;
    }
    else {
	ArFprintf(stderr, 
	    "Internal error in ArPageInfoSetContext(): get help.\n");
	exit( -1 );
    }
}

/**--------------------------------------------------------------------------**/
void ArDefaultsSetAntiMetric( ilP )
  register Il_state_t *ilP;
{
    if (ilP) { 
	ilP->doc_info.measure_unit = tok_INCHES;
	ilP->page_info = default_il_state.page_info;
	active_default_cmpnP = &default_cmpn;
    }
}

/**--------------------------------------------------------------------------**/
void ArDefaultsSetMetric( ilP )
  register Il_state_t *ilP;
{
    if (ilP) { 
	ilP->doc_info.measure_unit = tok_MM;
	ilP->page_info = default_metric_page_info;
    }
    active_default_cmpnP = &default_metric_cmpn;
}

/**--------------------------------------------------------------------------**/
