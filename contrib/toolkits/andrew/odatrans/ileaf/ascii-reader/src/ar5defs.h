
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
/*	ar5defs.h                                 */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  06/88    BT	Original coding
 */

#ifndef _AR5DEFS_H
#define _AR5DEFS_H
/**--------------------------------------------------------------------------**/
/**	This file defines the public interface for the ASCII Reader (AR)     **/
/**	Fixed size structures are directly present.  Variable data like cmpn **/
/**	definitions, that may occur arbitrarily many times, have a pointer   **/
/**	to a list.							     **/
/**	The main data structure is declared after all other data structures. **/
/**	The function interface declarations are the very last part of 	     **/
/**	this file.       						     **/
/**	            							     **/
/**	Both the data structures and the data conventions are slightly	     **/
/**	different than in TPS, since here we need to do less.	     	     **/
/**	            							     **/
/**	conventions:							     **/
/**	a structure or typedef		something_t			     **/
/**	a pointer variable		somethingP			     **/
/**--------------------------------------------------------------------------**/
#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"	/** cdefs.h defines bool, shar, ushar **/
#include "sysstdio.h"
#else
#include <stdio.h>
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short rsu;
typedef long Rsu;
#endif
/**--------------------------------------------------------------------------**/
/** this is for dynamically-growing char/shar string services.  **/
#include "ar5csave.h"
/**--------------------------------------------------------------------------**/
#ifndef TRUE
#define TRUE	(1)
#define FALSE	(0)
#endif
#ifndef YES
#define YES	(1)
#define NO	(0)
#endif

/**--------------------------------------------------------------------------**/
#define DEF_RSUINCH	(900)

/**--------------------------------------------------------------------------**/
/** Constants of the implementation					     **/

#define AR_APAGE_ANY	    (-1)	/* A-page has no maximum limit. **/
#define AR_DIAGRAM_RSU      (16384)
#define AR_HYPHEN_ANY	    (0)
#define AR_HYPHEN_DEFAULT   (5)
#define AR_LINE_SCALING     (1000)
#define AR_MAX_NAME         (20+1)/** must be defined before {cmpn,frame}name**/
#define AR_MAX_ANLEVELS     (8)
#define AR_MAX_ANTAGSIZE    (15+1)
#define AR_MAX_ANVALUESIZE  (15+1)
#define AR_MAX_ATTRVALSIZE  (256+1)
#define AR_MAX_ANFIX        (15+1)
#define AR_MAX_COLORS       (256)
#define AR_MAX_CMPNNAME     (AR_MAX_NAME)
#define AR_MAX_FEATHERING_DFLT     (8)
#define AR_MAX_FONTNAME     (80+1)
#define AR_MAX_FRAMENAME    (AR_MAX_NAME)
#define AR_MAX_HYPHEN       (10)
#define AR_MAX_INDCNT	    (128)	/*  maximum indent line count */
#define AR_MAX_IXLEVELS     (6)		/*  maximum index levels */
#define AR_MAX_IXSIZE       (47+1)
#define AR_MAX_LANGUAGE	    (32+1)
#define	AR_MAX_LNSPC	    (10.0)	/*  maximum line spacing (in lines) */
#define AR_MAX_PATTERNS     (256)
#define AR_MAX_PGNUM_PREFIX (32+1)
#define AR_MAX_PRINTER_SIZE (128+1)
#define AR_MAX_QSTR	    (255+1)	/* max length of any string */
#define AR_MAX_REFTAGSIZE   (15+1)
#define AR_MAX_SHAR	    (0x3FFF)	/** max value of any shar. **/
#define AR_MAX_TABS         (30)
#define AR_MAX_TOCNAME      (31+1)
#define AR_MAX_WIDORPH	    (16)	/*  maximum widow/orphan number */
#define AR_MAX_WORD	    (25+1)	/* max length of token word */
#define AR_MIN_ANLEVELS     (1)
#define AR_MIN_LNSPC	    (1.0)	/*  minimum line spacing (in lines) */
#define AR_PAGE_DENOM	    (1024)/* autosize page-percent in ratio to this. **/
#define AR_PATTERN_SIZE	    (16)
#define AR_PGNUM_INHERIT    (-1)
#define AR_SCREEN_DPI       (75)

/**--------------------------------------------------------------------------**/
typedef short Measure_t;
#define tok_INCHES	(0)
#define tok_MM		(1+tok_INCHES)
#define tok_PICAS	(1+tok_MM)
#define tok_CICEROS	(1+tok_PICAS)
#define AR_MAX_MEASURE	(tok_CICEROS)
extern  char *Measure_names[]; 

/**--------------------------------------------------------------------------**/
typedef short Num_style_t;
#define tok_ARABIC	(0)
#define tok_LOWROMAN	(1+tok_ARABIC)
#define tok_UPROMAN	(1+tok_LOWROMAN)
#define tok_LOWALPHA	(1+tok_UPROMAN)
#define tok_UPALPHA	(1+tok_LOWALPHA)
#define AR_MAX_NUMSTYLE	(tok_UPALPHA)
extern  char *NumStyle_names[];

/**--------------------------------------------------------------------------**/
typedef short Direction_t;
#define d_UNKNOWN	(0)
#define d_TOP		(1+d_UNKNOWN)
#define d_BOTTOM	(1+d_TOP)
#define d_LEFT		(1+d_BOTTOM)
#define d_RIGHT		(1+d_LEFT)
#define d_CENTER	(1+d_RIGHT)
#define d_BOTH 		(1+d_CENTER)
#define d_INNER		(1+d_BOTH)
#define d_OUTER		(1+d_INNER)
#define d_PORTRAIT	(1+d_OUTER)
#define d_LANDSCAPE	(1+d_PORTRAIT)
#define d_AUTOMATIC 	(1+d_LANDSCAPE)
#define d_ANY		(1+d_AUTOMATIC)

/**--------------------------------------------------------------------------**/
/**	Composition/FormSpace values	**/
extern char *Composition_names[];
#define FSPC_OVR	(0)
#define FSPC_OPT	(1+FSPC_OVR)

/**--------------------------------------------------------------------------**/
/**	Component Alignment values	**/
#define CJST	(0)
#define CCEN	(1+CJST)
#define CLFT	(1+CCEN)
#define CRGT	(1+CLFT)

/**--------------------------------------------------------------------------**/
/**	Identifier: structures returned by ArNextObject() are self-identified**/
/**	Each one MUST have an Identifier_t as its first member.		     **/
/**--------------------------------------------------------------------------**/
#define Identify(thing) (thing? (*(Identifier_t *)thing) : (ID_Error))
typedef short	Identifier_t;
#define ID_Error	(-1)
#define ID_NullObj	(1+ID_Error)
#define ID_OpsDecl	(1+ID_NullObj)
#define ID_Document	(1+ID_OpsDecl)
#define ID_Font		(1+ID_Document)
#define ID_Color	(1+ID_Font)
#define ID_Pattern	(1+ID_Color)
#define ID_Page		(1+ID_Pattern)
#define ID_MasterFrame	(1+ID_Page)
#define ID_Frame	(1+ID_MasterFrame)
#define ID_ClassDef	(1+ID_Frame)
#define ID_MasterCmpn	(1+ID_ClassDef)
#define ID_Cmpn		(1+ID_MasterCmpn)
#define ID_MasterTable	(1+ID_Cmpn)
#define ID_Table	(1+ID_MasterTable)
#define ID_MasterRow	(1+ID_Table)
#define ID_Row		(1+ID_MasterRow)
#define ID_Cell		(1+ID_Row)
#define ID_HeadFoot	(1+ID_Cell)
#define ID_AutoStream	(1+ID_HeadFoot)
#define ID_FileInfo	(1+ID_AutoStream)
#define ID_State	(1+ID_FileInfo)
#define ID_Comment	(1+ID_State)
#define ID_UnknownDecl  (1+ID_Comment)
#define ID_UnknownProp  (1+ID_UnknownDecl)
#define ID_MAX		(ID_UnknownProp)	/** keep this up-to-date **/

/**--------------------------------------------------------------------------**/
/**	Null Object.						     	     **/
/**	A NullObj_t has an identifier, a pointer to the next one on a singly-**/
/**	linked list, and a spare pointer for use by clients.  The spare is   **/
/**	NEVER touched by AR functions, except to init it to NULL in newly-   **/
/**	allocated objects.  Each data structure that can be returned by      **/
/**	ArNextObject() and is part of a linked list conforms to a NullObj_t  **/
/**	in its first fields; this allows various data structures to be 	     **/
/**	linked and handled uniformly.   				     **/
/**	Other object returned by ArNextObject() conform at the first member, **/
/**	so that the Identify() macro can work.				     **/
/**--------------------------------------------------------------------------**/
/**	The macro DECLARE_NullObjFields is used to ensure a uniform 	     **/
/**	declaration of its fields in data structures that conform	     **/
/**	to a NullObj_t.  						     **/
/**--------------------------------------------------------------------------**/
#define DECLARE_NullObjFields \
    Identifier_t id; \
    struct nullobj_t *nextP; \
    struct nullobj_t *spareP

typedef struct nullobj_t
{
    DECLARE_NullObjFields;
} NullObj_t;

typedef NullObj_t opaque_t;

/**--------------------------------------------------------------------------**/
/**	'Unknown property' information.				     	     **/
/**	it is preserved as text, intact and uninterpreted.		     **/
/**--------------------------------------------------------------------------**/
typedef struct unknown_t
{
    DECLARE_NullObjFields;
    Ssave_t		text;
} Unknown_t;

/**--------------------------------------------------------------------------**/
/**	Comment information.					     	     **/
/**--------------------------------------------------------------------------**/
typedef struct Comment_t
{
    DECLARE_NullObjFields;
    Ssave_t		text;
} Comment_t;

/**--------------------------------------------------------------------------**/
/**	OPS declaration information.					     **/
/**--------------------------------------------------------------------------**/
#define OPS_STYLE_DEFAULT	(0)
#define OPS_STYLE_METRIC	(1+OPS_STYLE_DEFAULT)
typedef short Ops_version_t;
typedef struct ops_decl_t 
{
    Identifier_t	id;
    Unknown_t 	       *unkP;
    Ops_version_t	version;
    short		style;
} Ops_info_t;

/**--------------------------------------------------------------------------**/
/**	Document property information.					     **/
/**--------------------------------------------------------------------------**/
#define spot_color_sep_OFF	(0)
#define spot_color_sep_SCREENED	(1+spot_color_sep_OFF)
#define spot_color_sep_SOLID	(1+spot_color_sep_SCREENED)

typedef struct document_flags_t
{
    unsigned header_page		: 1;
    unsigned double_sided		: 1;
    unsigned manual_feed		: 1;
    unsigned print_rev			: 1;
    unsigned print_strikes		: 1;
    unsigned print_underlines		: 1;
    unsigned underline_at_descender	: 1;
    unsigned orient_inverted		: 1;
} Document_flags_t;

typedef struct document_info_t 
{
    Identifier_t	id;
    Unknown_t	       *unkP;
    Document_flags_t	flags;
    shar 		final_output_dev[AR_MAX_PRINTER_SIZE];
    shar 		def_printer     [AR_MAX_PRINTER_SIZE];
    Measure_t		measure_unit;
    Num_style_t		A_Pg_style;
    shar		A_Pg_prefix[AR_MAX_WORD];
    short		max_page;
    short		spot_color_sep;
} Document_info_t;

/**--------------------------------------------------------------------------**/
/**	Font information.						     **/
/**--------------------------------------------------------------------------**/
/**	Font attributes are directly connected with inheritance into in-line **/
/**	components.                                  		 	     **/
/**	The <!Fonts,...> section does not declare any attributes, just 	     **/
/**	font family, size, and face.					     **/
/**	The Font = ...., property of a cmpn master can set either inheritance**/
/**	or attributes, since the master properties may be invoked either as  **/
/**	a top-level component or in-line component.			     **/
/**	The Font = ...., property of a cmpn tag  can set attributes, if it   **/
/**	is a top-level component, or inheritance, if it is an in-line 	     **/
/**	component.  A font-token in the body of either component cannot set  **/
/**	inheritance but can set attributes.   BT			     **/

/**--------------------------------------------------------------------------**/
/**	Values for a font face.						     **/
typedef short		Font_face_t;
extern char *FontFace_names[];
#define face_UNKNOWN		(0)
#define face_PLAIN		(1+face_UNKNOWN)
#define face_BOLD		(1+face_PLAIN)
#define face_ITALIC		(1+face_BOLD)
#define face_BOLD_ITALIC	(1+face_ITALIC)

/**--------------------------------------------------------------------------**/
/** used for all two-bit fields below. **/
#define fn_INHERIT_NONE		(0)
#define fn_INHERIT_LESS		(1+fn_INHERIT_NONE)
#define fn_INHERIT_MORE		(1+fn_INHERIT_LESS)
#define fn_INHERIT_TOGGLE	(1+fn_INHERIT_LESS)/*MORE,TOGGLE have same val*/
#define fn_INHERIT_DIRECT	(1+fn_INHERIT_MORE)

/**--------------------------------------------------------------------------**/
#define cap_NORMAL	(0)
#define cap_ALL		(1+cap_NORMAL)
#define cap_LOWSMALL	(1+cap_ALL)
#define cap_ALLSMALL	(1+cap_LOWSMALL)

/**--------------------------------------------------------------------------**/
#define trk_OFF		(0)
#define trk_MINTIGHT	(1+trk_OFF)
#define trk_MEDTIGHT	(1+trk_MINTIGHT)
#define trk_MAXTIGHT	(1+trk_MEDTIGHT)
#define trk_UNUSED	(1+trk_MAXTIGHT)
#define trk_MINLOOSE	(1+trk_UNUSED)
#define trk_MEDLOOSE	(1+trk_MINLOOSE)
#define trk_MAXLOOSE	(1+trk_MEDLOOSE)

/**--------------------------------------------------------------------------**/
#define fn_FACE_BOLD_OP		(1)
#define fn_FACE_ITALIC_OP	(2)
#define fn_FACE_SAME	(fn_FACE_BOLD_OP|fn_FACE_ITALIC_OP)

/**--------------------------------------------------------------------------**/
#define lang_AM		(0)
#define lang_BR		(1+lang_AM)
#define lang_CA		(1+lang_BR)
#define lang_DA		(1+lang_CA)
#define lang_DU		(1+lang_DA)
#define lang_FI		(1+lang_DU)
#define lang_FR		(1+lang_FI)
#define lang_GE		(1+lang_FR)
#define lang_IT		(1+lang_GE)
#define lang_NL		(1+lang_IT)	/** The 'no language' case. **/
#define lang_NO		(1+lang_NL)
#define lang_PO		(1+lang_NO)
#define lang_SP		(1+lang_PO)
#define lang_SW		(1+lang_SP)
#define lang_DEFAULT	(lang_AM)
extern char *Language_names[];

/**--------------------------------------------------------------------------**/
/** the other colors vary and cant be given names.			     **/
#define clr_BLACK	(1)

/**--------------------------------------------------------------------------**/
/**	Fundamental info about a font.					     **/
typedef struct font_info_t 
{
    DECLARE_NullObjFields;
    short		font_token;
    shar		font_name[AR_MAX_WORD];
    short		font_size;
    Font_face_t		font_face;
} Font_info_t;

/**--------------------------------------------------------------------------**/
typedef struct font_inherit_t
{
    unsigned size	: 2;
    unsigned fam	: 2;
    unsigned lang	: 2;
    unsigned bold	: 2;
    unsigned ital	: 2;
    unsigned str	: 2;
    unsigned und	: 2;
    unsigned rev	: 2;
    unsigned sup	: 2;
    unsigned sub	: 2;
    unsigned kpr	: 2;
    unsigned trk	: 2;
    unsigned cap	: 2;
    unsigned clr	: 2;
    unsigned ovr	: 2;
} Font_inherit_t;

/**--------------------------------------------------------------------------**/
typedef struct font_attr_t 
{
    unsigned str	: 1;
    unsigned und	: 1;
    unsigned lang	: 4;
    unsigned rev	: 1;
    unsigned sub	: 1;
    unsigned sup	: 1;
    unsigned kpr	: 1;
    unsigned trk	: 3;
    unsigned dbl	: 1;
    unsigned ovr	: 1;
    unsigned clr	: 8;
    unsigned cap	: 2;
} Font_attr_t;

/**--------------------------------------------------------------------------**/
/** this is used in parsing font markup, e.g. <F387@U@S@Lbr@iD>		     **/
typedef struct font_value_t
{
    short		font_token;
    Font_inherit_t	font_inherit;
    Font_attr_t		font_attr;
} Font_value_t;

/**--------------------------------------------------------------------------**/
/**	Color descriptions.						     **/
/**--------------------------------------------------------------------------**/
/**	There is a limit (curently 256) on how many colors can be described  **/
/**	in one document.  However, most documents have many fewer than the   **/
/**	limit.  Hence we use a linked list, rather than an array.	     **/

typedef struct color_desc_t 
{
    float	r, g, b;
} Color_desc_t;

typedef struct color_t 
{
    shar		type;
    short		index;
    Color_desc_t	desc;
} Color_t;

typedef struct color_info_t 
{
    DECLARE_NullObjFields;
    Color_t		color;
} Color_info_t;

/**--------------------------------------------------------------------------**/
/**	Fill-Pattern descriptions.					     **/
/**--------------------------------------------------------------------------**/
/**	There is a limit (curently 256) on how many patterns can be described**/
/**	in one document.  However, most documents have many fewer than the   **/
/**	limit.  Hence we use a linked list, rather than an array.	     **/

typedef struct pattern_t 
{
    shar		type;
    short		index;
    unsigned short	pattern[AR_PATTERN_SIZE];
} Pattern_t;

typedef struct pattern_info_t 
{
    DECLARE_NullObjFields;
    Pattern_t			pattern;
} Pattern_info_t;

/**--------------------------------------------------------------------------**/
/**	Line-Pattern descriptions.					     **/
/**--------------------------------------------------------------------------**/
/**	We only define the default pattern, though we should define them all.**/
#define linepat_SOLID	(0)

/**--------------------------------------------------------------------------**/
/**	Page information is used in microdocuments as well as document-wide. **/
/**--------------------------------------------------------------------------**/
#define REV_BAR_DEFAULT	(0)
#define REV_BAR_LEFT	(1+REV_BAR_DEFAULT)
#define REV_BAR_RIGHT	(1+REV_BAR_LEFT)
#define PG_PARITY_DEFAULT	(0)
#define PG_PARITY_EVEN	(1+PG_PARITY_DEFAULT)
#define PG_PARITY_ODD	(1+PG_PARITY_EVEN)
#define TURNED_0	(0)
#define TURNED_90	(1+TURNED_0)
#define TURNED_180	(1+TURNED_90	)
#define TURNED_270	(1+TURNED_180	)

typedef struct page_flags_t
{
    unsigned bleed		: 1;
    unsigned hyphenation	: 1;
    unsigned bal_columns	: 1;
    unsigned vert_just		: 1;
    unsigned feathering		: 1;
    unsigned vert_just_pages	: 1;
    unsigned frozen_autonums	: 1;
    unsigned pgnum_absolute	: 1;
    unsigned rev_bar_place	: 2;
    unsigned page_parity	: 2;
    unsigned turned_pages	: 2;
} Page_flags_t;

#define MARGIN_NORMAL	(0)
#define MARGIN_LR	(1+MARGIN_NORMAL)
#define START_PG_ANY	(-1)

/**--------------------------------------------------------------------------**/
typedef struct page_info_t 
{
    Identifier_t id;
    Unknown_t	       *unkP;
    Page_flags_t flags;
    bool	micro_props_only;
    short	columns;
    Rsu		gutter;
    Rsu		height;
    Rsu		width;
    Rsu		top_margin;
    Rsu		bottom_margin;
    Rsu		left_margin;
    Rsu		right_margin;
    Rsu		inner_margin;
    Rsu		outer_margin;
    Direction_t	first_page;
    short	l_r_margins;
    short	start_page_num;
    shar	pgnum_prefix[AR_MAX_WORD];
    Num_style_t	pgnum_style;
    short	ladder_count;
    short	margin_stretch;
    short	margin_shrink;
    short	frame_margin_stretch;
    short	max_feathering;
    short	depth_pbreak;
    short	depth_no_pbreak;
} Page_info_t;

/**--------------------------------------------------------------------------**/
/**	Autonumber information.						     **/
/**--------------------------------------------------------------------------**/
typedef struct an_level_t 
{
    Num_style_t    an_symbol;
    shar 	   an_prefix[AR_MAX_ANFIX];
    shar 	   an_suffix[AR_MAX_ANFIX];
    short 	   start_value;
    bool 	   an_last_only;
    bool 	   an_show;
    bool 	   an_inherit;	/** BT NOTE NOTE 7/6/88 NOTE NOTE **/
} An_level_t;

typedef struct an_stream_info_t
{
    DECLARE_NullObjFields;
    /** Unknown_t		*unkP; BT can't parse props here anyway**/
    shar 			*an_name;
    short 			an_levels;
    An_level_t 			an_level_info[AR_MAX_ANLEVELS];
} An_stream_info_t;

typedef struct autonum_t
{
    bool 	first;
    bool	restart;
    short	level;
    shar	name [AR_MAX_ANTAGSIZE];
    shar	tag  [AR_MAX_ANTAGSIZE];
    shar	value[AR_MAX_ANVALUESIZE];
} Autonum_t;

/**--------------------------------------------------------------------------**/
/**	Index data structures. 						     **/
/**--------------------------------------------------------------------------**/
#define index_SCOPE_LINE	(0)
#define index_SCOPE_CMPN	(1+index_SCOPE_LINE)
#define index_SCOPE_COUNT	(1+index_SCOPE_CMPN)
#define index_SCOPE_NAMED_CMPN	(1+index_SCOPE_COUNT)
#define index_SCOPE_SEE_ALSO	(1+index_SCOPE_NAMED_CMPN)

typedef struct index_t
{
    short	scope;
    short	count;
    short	typeface;
    short	dictionary;		/** language. **/
    shar	see_string[AR_MAX_IXSIZE];
    shar	to_named  [AR_MAX_IXSIZE];
    shar	doc       [AR_MAX_IXSIZE];
    shar	*sort_strings[AR_MAX_IXLEVELS];
    shar	*headings    [AR_MAX_IXLEVELS];
} Index_t;

/**--------------------------------------------------------------------------**/
/**	Ref data structures.						     **/
/**--------------------------------------------------------------------------**/
#define ref_CUR_PAGE_PLAIN	(0)
#define ref_CUR_PAGE_BLANK	(1+ref_CUR_PAGE_PLAIN)
#define ref_ANUM_NUM		(1+ref_CUR_PAGE_BLANK)
#define ref_ANUM_PAGE		(1+ref_ANUM_NUM)
#define ref_ATTR_VALUE		(1+ref_ANUM_PAGE)
#define ref_CUR_FRAME		(1+ref_ATTR_VALUE)
#define ref_TOTAL_FRAME		(1+ref_CUR_FRAME)

typedef struct lookuppair_t
{
    struct lookuppair_t *nextP;
    shar*	nameP;
    shar*	stringP;
} LookupPair_t;

typedef struct ref_t
{
    long	  frame_start;
    shar	  tag      [AR_MAX_REFTAGSIZE];
    shar	  attribute[AR_MAX_REFTAGSIZE];
    shar	  result   [AR_MAX_REFTAGSIZE];
    shar	  value    [AR_MAX_REFTAGSIZE];
    short	  range;
    short 	  operation;
    short	  type;
    LookupPair_t *pairsP;
} Ref_t;

/**--------------------------------------------------------------------------**/
/**	Effectivity attributes.						     **/
/**--------------------------------------------------------------------------**/
extern char *Attr_range_names[];
#define attr_RANGE_PAGE		(0)
#define attr_RANGE_SHEET	(1+attr_RANGE_PAGE)
#define attr_RANGE_CMPN		(1+attr_RANGE_SHEET)
extern char *Attr_oper_names[];
#define attr_OPER_MAX		(0)
#define attr_OPER_MIN		(1+attr_OPER_MAX)

typedef struct attr_t
{
    struct attr_t *nextP;
    shar*	nameP;
    shar*	stringP;
} Attr_t;

/**--------------------------------------------------------------------------**/
/**	Micro-document information.					     **/
/**--------------------------------------------------------------------------**/
/** 	A micro-document has its own, limited, Page_info_t and a list of     **/
/**	NullObj_t contents. TPS 4.0 allows only Components in a microdoc;    **/
/**	we allow anything.						     **/
typedef struct microdoc_t 
{
    Page_info_t  micro_page;
    NullObj_t    *contentsP;
} Microdoc_t;

/**--------------------------------------------------------------------------**/
/**     Diagram Information                                                  **/
/**--------------------------------------------------------------------------**/

extern	short	Arimagecount;	/* Arimagecount is used to number images. */
				/* ArSaveDg sets it to zero at the top of */
				/* each frame.  If you read a frame       */
				/* "by hand", set Arimagecount to zero    */

/** Objects within a diagram. **/
#define dg_GROUP	'g'  /* 0 */
#define dg_POLYBOX	'B'  /* 1 */
#define dg_LINE		'v'  /* 2 */
#define dg_RELL		'e'  /* 3 */
#define dg_ARC		'a'  /* 4 */
#define dg_ARC_CC	'A'  /* 5 */
#define dg_CHART	'c'  /* 6 */
#define dg_EQN		'm'  /* 7 */
#define dg_MICRODOC	'T'  /* 8 */
#define dg_SPLINE_SEG	's'  /* 9 */
#define dg_POLY		'p'  /* 10 */
#define dg_TEXTSTR	't'  /* 11 */
#define dg_HLINE	'H'  /* 12 */
#define dg_VLINE	'?'  /* 13 */  /* 'V' conflicts with vector ? */
#define dg_IMAGE	'i'  /* 14 */
#define dg_VECTORS	'V'  /* 15 */  /*actually a vector-list plotter object*/
#define dg_BOX		'b'  /* 16 */
#define dg_SPLINE	'S'  /* 17 */
#define dg_EDIT_STATE	'E'  /* 18 */
#define dg_TRACE	'd'  /* 19 */
#define dg_OUTLINE	'O'  /* 20 */
#define dg_CONVERT	'o'  /* 21 */
#define dg_EPS		'n'  /* 22 */

typedef int *opaque_ptr_t;        /* an "any type" pointer */
typedef shar ArDgObj_type;        /* object type letter representations */

typedef struct ar_dg_object_t { /* the common part of all diagramming objects */
  struct ar_dg_object_t *next;
  ArDgObj_type          type;
  short                 version;
  short                 z_coord;
  long                  flags;
  opaque_ptr_t          info;   
} ArDgObject_t;

typedef struct ar_dg_head_t {     /* the highest level of a diagram */
  short          version;
  ArDgObject_t   *contents;
} ArDgHead_t;

#define CURRENT_DIAGRAM_VERS 6
#define ERR_UNKNOWN 0
#define ERR_VERSION 1
#define ERR_FORMAT  2
#define ERR_MEMORY  3
#define ERR_TYPE    4
#define MAX_NESTING 30
#undef ERROR
#define ERROR      -1
#define OK          0
#define MAX_S_LEN 2048
#define MAX_TEXT_S_LEN 256
#define OUTPUT_LINE_LENGTH 80
#define ID_LEN 3

/*
 * The following are the TSD's (Type Specific Data) for the
 * non-common portion of the ArDgObject_t. The "info" field of
 * the ArDgObject_t is a pointer to one of these babies.
 */

typedef struct poly_tsd_t {    /* polygon 'p', Vers 8 */
  short        fpat;
  short        fcolor;
  short        ftrans;
  ArDgObject_t *contents;
} PolyTSD_t;

typedef struct line_tsd_t {    /* line 'v', Vers 6 */
  float x1, y1;
  float x2, y2;
  short color;
  short trans;
  short width;
  short pattern;
} LineTSD_t;

typedef struct edit_state_tsd_t {    /* an edit object, 'E', vers 15 */
  /* this portion is known as an EDITSTATE struct in OPS */
    short	    fill;	    /* default fill pattern */
    short	    width;	    /* default border width */
    short	    gravity;	    /* if TRUE, align via gravity */
    float	    gravrad;	    /* current gravity radius */
    short	    detent;	    /* if TRUE, detent active */
    float	    angle;	    /* detent angle */
    short	    pattern;	    /* dash pattern */
    short	    zoom;	    /* if TRUE, zoom enabled */
    float	    zoomlev;	    /* zoom level */
    float	    xshift, yshift; /* graph relocation (deltas) */
    short	    scale;	    /* if TRUE, input scaling in effect */
    float	    scalefact;	    /* input scale factor */
    short           fcolor;         /* default fill color */
    short	    ftrans;         /* default fill transparency */
    short           ecolor;	    /* default edge color */
    short	    etrans;	    /* default edge transparency */
    float	    textangle;	    /* default text angle */
    short           tcolor;         /* default text color */
  /* this portion is known as a GRID struct in OPS */
    short           display;        /* if TRUE, display */
    short           align;          /* if TRUE, align to grid */
    short           ontop;          /* if TRUE, grid on top (else bottom) */
    float           minw;	     /* minor horizontal interval */
    float           minh;	     /* minor vertical interval */
    short           majw;	     /* major horizontal interval */
    short           majh;	     /* major vertical interval */
    short           type;	     /* (version 8) grid type */
    float           mini;	     /* (version 9) minor isometric interval */
    short           maji;	     /* (version 9) major isometric interval */
} EditStateTSD_t;

typedef struct ellipse_tsd_t {  /* ellipse 'e', Vers 8 */
  float ox, oy;
  float t1x, t1y;
  float t2x, t2y;
  short fcolor;
  short ftrans;
  short fpattern;
  short bcolor;
  short btrans;
  short bwidth;
  short bpattern;
} EllipseTSD_t;

typedef struct arc_tsd_t {  /* arc 'a', Vers 8 */
  short format;
  float btx, bty;
  float bx, by;
  float mx, my;
  float ex, ey;
  float tex, tey;
  float x, y;
  short fpattern;
  short fcolor;
  short ftrans;
  short bcolor;
  short btrans;
  short bwidth;
  short bpattern;
} ArcTSD_t;

typedef struct knot_t {  /* this struct is used by spline */
  struct knot_t *next;
  float x, y;
  short color;
  short trans;
  short width;
  short pattern;
} KNOT_t;

typedef struct spline_tsd_t {  /* spline 'S', Vers, 13 */
  float x, y;
  short fpattern;
  short fcolor;
  short ftrans;
  short number_of_knots;
  KNOT_t *knot_list;
  short type;
} SplineTSD_t;

typedef struct textstr_tsd_t {   /* text string 't', Vers 14*/
  float x, y;
  short anchor;
  short color;
  short trans;
  float angle;
  char *attrib;
  char *font;
  shar *text;
} TextstrTSD_t;

typedef struct hex_t {              /* this is used by image */
  long size;
  unsigned char *data;
} HEX_t;

typedef struct raster_t {            /* this is used by image */
  long  id;        /* image id */
  bool  reference; /* my addition, this raster is just a reference ? y/n */
  short xh, yh;    /* transformation */
  short xv, yv;
  short x1, y1;    /* cropping rectangle */
  short x2, y2;
  short map_size;  /* contrast map info */
  HEX_t *map;
  short screenx;   /* halftone screen info */
  short screeny;
  HEX_t *screen;
  short w;         /* raster dimensions */
  short h;
  short d;
  short format;    
  long  size;
  HEX_t *raster;    /* raster itself */
} RASTER_t; 

typedef struct break_t {              /* this is used by image */
  struct break_t *next;
  float x;
  float y;
} BREAK_t;

typedef struct image_tsd_t {          /* image 'i', vers 15 */
  float xo, yo;      /* transformation parallelogram */
  float xh, yh;
  float xv, yv;
  short cx1, cy1;    /* cropping rectangle */
  short cx2, cy2;
  short num_breaks;  /* contrast curve */
  BREAK_t *break_list;
  short color;       /* color & transparency */
  short trans;
  short submenu;     /* subeditor last used */
  float origw, origh;/* default dimensions */
  RASTER_t *orig;    /* origin & display rasters */
  RASTER_t *disp;
} ImageTSD_t;


typedef struct box_tsd_t {     /* box 'b' , vers ?? */
  float x1, y1;
  float x2, y2;
  short texture;
  short border_texture;
  short border_width;
  short border_pattern;
} BoxTSD_t;  

typedef struct eqn_tsd_t {   /* equation 'm', vers 8 */
  float x, y;
  char *font;
  shar *text;
} EqnTSD_t;

typedef struct micro_tsd_t {   /* microdoc 'T', vers 12 */
  float x, y;
  short bcolor;
  short btrans;
  short bpat;
  short lcolor;
  short ltrans;
  short width;
  short pattern;
  short flags;
  Microdoc_t *text;
} MicroTSD_t;

typedef struct spline_seg_tsd_t {   /* spline segment 's', vers ?? */
  float x1, y1;
  float x2, y2;
  short color;
  short trans;
  short width;
  short pattern;
} SplineSegTSD_t;

typedef struct hex_list_t {    /* used by Vector, ESP */
  struct hex_list_t *next;
  long              no_pairs;
  HEX_t             *data;
} HexList_t;

typedef struct vector_list_tsd_t {    /* vector list plotter object 'V', vers 9 */
  short color;
  short trans;
  float xo, yo;
  float xh, yh;
  float xv, yv;
  HexList_t *list;
} VectorListTSD_t;

typedef struct eps_tsd_t {   /* Encapsulatet PostScript object 'n', vers 5 */
  float xo, yo;
  float xh, yh;
  float xv, yv;
  float llx, lly;
  float urx, ury;
  ArDgObject_t *display_obj;
  HexList_t *list;
} EpsTSD_t;

typedef struct outline_tsd_t {  /* outline font 'O', vers 5 */
  long key;
  char *font;
  short character;
  char *encrypted_data;
} OutlineTSD_t;

typedef enum ct_record_id_e {    /* used by chart */
  v, t, f, g, h, j, m, b, w, l, lo, x, y, p, r, D, Dx, F, G, C, P, T, L, O, E, W
} CtRecordId_e;

#ifndef _CT_VALUE
#define _CT_VALUE
/* data in charts keeps double and display precision */
typedef struct {
	double val ;	/* value	  */
	short dig ;	/* display digits to right of decimal */
} CT_VALUE ;
#endif

typedef CT_VALUE CtData_t;

typedef struct ct_value_t {      /* used by chart */
  union {
    long  ival;
    CtData_t cval;
    float fval;	              	/* for fields not recording precision */
    shar *pval;
  } val;
} CtValue_t;

typedef struct ct_field_t {      /* used by chart */
  struct ct_field_t *next;
  CtValue_t         *value;
} CtField_t;

typedef struct ct_record_t {     /* used by chart */
  struct ct_record_t *next;
  CtRecordId_e id;
  CtField_t    *field;
} CtRecord_t;

typedef struct chart_tsd_t {     /* chart 'c', vers 5 */
  float x1, y1;
  float x2, y2;
  char *font;
  short version;
  CtRecord_t *record;
} ChartTSD_t;

/**--------------------------------------------------------------------------**/
/**	Header/Footer information.					     **/
/**--------------------------------------------------------------------------**/
typedef short HeadFoot_type;
extern char *HeaderFooter_names[];
#define hf_PLAIN_HDR	(0)
#define hf_PLAIN_FTR	(1+hf_PLAIN_HDR)
#define hf_LEFT_HDR	(1+hf_PLAIN_FTR)
#define hf_LEFT_FTR	(1+hf_LEFT_HDR)
#define hf_RIGHT_HDR	(1+hf_LEFT_FTR	)
#define hf_RIGHT_FTR	(1+hf_RIGHT_HDR	)
#define hf_FIRST_HDR	(1+hf_RIGHT_FTR	)
#define hf_FIRST_FTR	(1+hf_FIRST_HDR	)

typedef struct headerfooter_t
{
    DECLARE_NullObjFields;
    Unknown_t			*unkP;
    HeadFoot_type		type;
    short			turned;	/** TURNED_90, etc. **/
    ArDgHead_t			diagram;
} HeaderFooter_t;

/**--------------------------------------------------------------------------**/
/**	Table Rulings information. (used by frames and tables and cells)     **/
/**--------------------------------------------------------------------------**/
/*  Table Rulings structure
 *	Each "cell" of a table is either a Cell_t or a Frame_t.
 *	It has two rulings, one for the left edge of the frame,
 *	and one for the top. (The right and bottom rulings are handled by the
 *	next frame on the line, and the frame on the next line, respectively;
 *	frames at the right and bottom edges of the table are handled
 *	specially).
 *	Each ruling has individual width, color, pattern, and visibility,
 *	a field controls whether the rule is single or double (double rules
 *	are drawn as two single rules with a single width space in between,
 *	giving a total width 3 times that of a single rule).
 *	Table borders are similar, except a rule is defined for each of the
 *	four sides. 
 */

typedef struct table_rule_t 
{
    unsigned char width;    	/* width of drawn ruling (in pixels) */
    unsigned char pattern;  	/* dot/dash pattern */
    unsigned char color;    	/* line color */
    unsigned char double_rule;  /* double-line ruling */
    unsigned char visible;  	/* ruling does not show */
} Table_rule_t;

typedef struct table_border_t 
{
    Table_rule_t top;
    Table_rule_t bottom;
    Table_rule_t left;
    Table_rule_t right;
} Table_border_t;

typedef struct tl_ruling_t 	/** this one is for top&left combos, as in   **/
{				/** frame-as-cell markup.		     **/
    Table_rule_t top;
    Table_rule_t left;
} TL_ruling_t;

/**--------------------------------------------------------------------------**/
/**	Frame information.						     **/
/**--------------------------------------------------------------------------**/

/**	Frame autosize information.					     **/

/**	Frame width control.						     **/
extern char *FrameAutoWid_names[];
#define ASZNOX        (0)	     /* 'size contents to frame = no' */
#define ASZWFIX       (1+ASZNOX)     /* width is fixed */
#define ASZEXX        (1+ASZWFIX)    /* diagram width follows frame  */
#define ASZEXXCOL     (1+ASZEXX)     /* diagram width follows frame, to column*/
#define ASZINX        (1+ASZEXXCOL)  /* frame   width follows diagram */
#define ASZWPAG       (1+ASZINX)     /* width set to page-minus-margins */
#define ASZWCOL       (1+ASZWPAG)    /* width set to column */
#define ASZWPAGM      (1+ASZWCOL)    /* width set to page including margins */
#define ASZWPAGL      (1+ASZWPAGM)   /* width set to page plus left margin */
#define ASZWPAGR      (1+ASZWPAGL)   /* width set to page plus right margin */
#define ASZWPAGO      (1+ASZWPAGR)   /* width set to page plus outer margin */
#define ASZWPAGI      (1+ASZWPAGO)   /* width set to page plus inner margin */
#define ASZWLMARG     (1+ASZWPAGI)   /* width set to left page margin */
#define ASZWRMARG     (1+ASZWLMARG)  /* width set to right page margin */
#define ASZWOMARG     (1+ASZWRMARG)  /* width set to outer page margin */
#define ASZWIMARG     (1+ASZWOMARG)  /* width set to inner page margin */
#define ASZWGUTTER    (1+ASZWIMARG)  /* width set to column gutter */

/**	Frame height control.						     **/
extern char *FrameAutoHgt_names[];
#define ASZNOY        (0)	     /* 'size contents to frame = no' */
#define ASZHFIX       (1+ASZNOY)     /* height is fixed */
#define ASZEXY        (1+ASZHFIX)    /* diagram height follows frame */
#define ASZINY        (1+ASZEXY)     /* frame height follows diagram */
#define ASZHPAG       (1+ASZINY)     /* height set to page minus margins */
#define ASZHPAGM      (1+ASZHPAG)    /* height set to page incl. margins */
#define ASZHPAGT      (1+ASZHPAGM)   /* height set to page plus top margin */
#define ASZHPAGB      (1+ASZHPAGT)   /* height set to page plus bottom margin */
#define ASZHTMARG     (1+ASZHPAGB)   /* height set to top page margin */
#define ASZHBMARG     (1+ASZHTMARG)  /* height set to bottom page margin */

typedef struct autosize_t {
    char		frame_width;
    char		frame_height;
    Rsu			xmin,  xmax;
    Rsu			ymin,  ymax;
    unsigned short	xfrac, yfrac;
    short		xadj,  yadj;
} Autosize_t;


/**--------------------------------------------------------------------------**/
/**	Frame flag definitions.						     **/
/**--------------------------------------------------------------------------**/

/**--------------------------------------------------------------------------**/
/**	Location types.							     **/
#define	fr_FIXED	(0)
#define	fr_FLOAT	(1+fr_FIXED)

/**--------------------------------------------------------------------------**/
/**	Horizontal Alignments.						     **/
extern char *FrameHAlign_names[];
#define	fr_H_LEFT	(0)
#define	fr_H_CENTER	(1+fr_H_LEFT)
#define	fr_H_RIGHT	(1+fr_H_CENTER)
#define	fr_H_INNER	(1+fr_H_RIGHT)
#define	fr_H_OUTER	(1+fr_H_INNER)

/**--------------------------------------------------------------------------**/
/**	Vertical Alignments.						     **/
extern char *FrameVAlign_names[];
#define	fr_V_BASELN	(0)
#define	fr_V_BOTTOM	(1+fr_V_BASELN)
#define	fr_V_CENTER	(1+fr_V_BOTTOM)
#define	fr_V_TOP	(1+fr_V_CENTER)

/**--------------------------------------------------------------------------**/
/**	X Location Alignments.						     **/
extern char *FrameHRef_names[];
#define	fr_X_PAGENONE	(0)
#define	fr_X_PAGEBOTH	(1+fr_X_PAGENONE)
#define	fr_X_MARGLEFT	(1+fr_X_PAGEBOTH)
#define	fr_X_MARGRIGHT	(1+fr_X_MARGLEFT)
#define	fr_X_MARGINNER	(1+fr_X_MARGRIGHT)
#define	fr_X_MARGOUTER	(1+fr_X_MARGINNER)
#define	fr_X_COLUMN	(1+fr_X_MARGOUTER)
#define	fr_X_ANCHOR	(1+fr_X_COLUMN)
#define	fr_X_GUTTLEFT	(1+fr_X_ANCHOR)
#define	fr_X_GUTTRIGHT	(1+fr_X_GUTTLEFT)

/**--------------------------------------------------------------------------**/
/**	Y Location Alignments.						     **/
extern char *FrameVRef_names[];
#define	fr_Y_PAGENONE	(0)
#define	fr_Y_PAGEBOTH	(1+fr_Y_PAGENONE)
#define	fr_Y_MARGTOP	(1+fr_Y_PAGEBOTH)
#define	fr_Y_MARGBOTTOM	(1+fr_Y_MARGTOP)
#define	fr_Y_ANCHOR	(1+fr_Y_MARGBOTTOM)

/**--------------------------------------------------------------------------**/
/**	Z (depth) Alignments.						     **/
#define	fr_Z_NOTSET	(0)
#define	fr_Z_UNDERLAY	(1+fr_Z_NOTSET)
#define	fr_Z_OVERLAY	(1+fr_Z_UNDERLAY)

/**--------------------------------------------------------------------------**/
/**	Repeating information.						     **/
#define	fr_REP_NONE	(0)
#define	fr_REP_BEGIN	(1+fr_REP_NONE)
#define	fr_REP_END	(1+fr_REP_BEGIN)
#define	fr_REP_SAME_PAGE (1+fr_REP_END)		/** this may be redundant??? **/

/**--------------------------------------------------------------------------**/
/**	Placement information.						     **/
extern char *FramePlace_names[];
#define fr_FIXEDPLACE	(0)
#define fr_FOLLOW_ANCHOR (1+fr_FIXEDPLACE)
#define fr_FOLLOW_TEXT	(1+fr_FOLLOW_ANCHOR)
#define fr_BOTTOM	(1+fr_FOLLOW_TEXT)
#define fr_MIDDLE	(1+fr_BOTTOM)
#define fr_TOP		(1+fr_MIDDLE)
#define fr_UNDERLAY	(1+fr_TOP)
#define fr_OVERLAY	(1+fr_UNDERLAY)

/**--------------------------------------------------------------------------**/
typedef struct frame_flag_t 
{
    /**---8-bit boundaries------**/
    unsigned	fixed		: 1;
    unsigned	shared		: 1;
    unsigned	auto_edit	: 1;
    unsigned	cell		: 1;
    unsigned    placement	: 4;
    /**-------------------------**/
    unsigned	h_align		: 3;
    unsigned	v_align		: 2;
    unsigned	show_anchor	: 1;
    unsigned	proportional	: 1;
    unsigned	filler_unused	: 1;
    /**-------------------------**/
    unsigned	x_locate	: 4;
    unsigned	y_locate	: 4;
    /**-------------------------**/
    unsigned	z_locate	: 2;
    unsigned	repeat_info	: 2;
    unsigned	repeat_same_page: 1;
    unsigned	same_page	: 1;
    unsigned	hidden		: 1;
/** unsigned	numbered	: 1; **/
    unsigned	super_num	: 1;
    /**-------------------------**/
    unsigned	cont_height	: 1;
    unsigned	cont_width	: 1;
    /**-------------------------**/
} Frame_flag_t;

/**--------------------------------------------------------------------------**/
/**	Frame structure definition.					     **/
/**--------------------------------------------------------------------------**/
/**	A frame can be pointed to by a token in a component's body, or it    **/
/**	can be a full-fledged member of a table row.  A Master frame is on   **//**	the list of all frame masters.  When part of a component, the frame's**/
/**	nextP will be NULL.  when part of a row, or being a master, nextP    **/
/**	points to the next one on the list.				     **/

typedef struct frame_t
{
    DECLARE_NullObjFields;
    Unknown_t		*unkP;
    shar		name[AR_MAX_FRAMENAME];
    Frame_flag_t	flags;
    shar		*page_numP;
    Autosize_t		*autosizeP;
    Attr_t		*attrvalsP;
    Rsu			a;	/* adjustment, ZERO for float (negative dist
				   from line baseline to frame top) */
    Rsu			height;
    Rsu			width;
    ArDgHead_t		diagram;	/* diagram frame structure */
    TL_ruling_t		*rulingsP;	/* ruling descriptor (NULL if none) */
    short tint;			/* background texture */
    Rsu			xofs;	/* alignment offset in x */
    Rsu			yofs;	/* alignment offset in y */
    Autonum_t 		*anumP;	/* autonumber that acts as the anchor for this
				   efrm. */
    short		straddle; /* for frames in cells */
} Frame_t;

/**--------------------------------------------------------------------------**/
/** Table Cell Information.						     **/
/**	A table cell is like a frame, but it in fact contains only microdoc  **/
/**	contents.  If the actual table-cell contains any graphic objects,    **/
/**	it will be represented by a <Frame,...> command in a row, not by     **/
/**	a <Cell,...> command.						     **/
/**	A Cell has a limited subset of frame properties that are legal in    **/
/**	its markup.  We use a separate data structure to represent it,       **/
/**	rather than overload (or underload, but in any case, re-use) a       **/
/**	Frame_t.							     **/

typedef struct cell_t
{
    DECLARE_NullObjFields;
    Unknown_t		*unkP;
    Autosize_t		*autosizeP;
    Rsu			height;
    Rsu			width;
    bool		auto_edit;
    short		straddle;
    short		v_align;
    TL_ruling_t		rulings;
    NullObj_t		*cellbodyP;	/** a list of Cmpn_t's most likely.  **/
} Cell_t;

/**--------------------------------------------------------------------------**/
/** Table Row Information.						     **/
/**	A table is a sequence of rows, each containing the same number 	     **/
/**	of cells, making columns.					     **/
/**	A row may also have a page-break token in it, since any row can begin**/
/**	a page.  We put a whole buffer in place, for consistency and some    **/
/**	dim view of future expandability, e.g. frames within tables, tables  **/
/**	within tables.							     **/

typedef struct row_flags_t
{
    unsigned hidden		: 1;
    unsigned new_page		: 1;
    unsigned new_column		: 1;
    unsigned pbreak_after	: 1;
    unsigned is_header		: 1;
    unsigned is_footer		: 1;
    unsigned A_page		: 1;
} Row_flags_t;

typedef struct row_t
{
    DECLARE_NullObjFields;
    shar		*row_nameP;
    Unknown_t	        *unkP;
    NullObj_t		*contentsP;
    Row_flags_t		flags;
    Rsu			top_margin;
    Rsu			bottom_margin;
    Font_value_t	font;
    Attr_t		*attrvalsP;
    Ssave_t		text_content;	/** holds page break token, if any. **/
} Row_t;

typedef struct row_name_t
{
    struct row_name_t	*nextP;
    shar 		*row_nameP;
} Row_name_t;

typedef struct table_flags_t
{
    unsigned new_page		: 1;
    unsigned new_column		: 1;
    unsigned pbreak_within	: 1;
    unsigned pbreak_after	: 1;
    unsigned straddle		: 1;
    unsigned pbreak_rulings	: 1;
    unsigned A_page		: 1;
} Table_flags_t;

typedef struct table_column_t 
{
    struct table_column_t *nextP;
    short		   num_column;		/** which column am I?  **/
    Rsu 		   width;
    Table_rule_t	   top;
    Table_rule_t	   left;
} Table_column_t;

typedef struct table_t
{
    DECLARE_NullObjFields;
    shar		*table_nameP;
    Unknown_t	        *unkP;
    short		num_columns;
    Table_column_t	*column_infoP;
    Row_t		*master_rowsP;
    Row_name_t		*create_rowsP;
    Row_t		*content_rowsP;
    Table_flags_t	flags;
    Rsu			top_margin;
    Rsu			bottom_margin;
    Rsu			left_margin;
    Rsu			right_margin;
    short		widow_control;
    short		orphan_control;
    Table_border_t	border;
    Table_rule_t	header_rule;
    Table_rule_t	footer_rule;
} Table_t;

/**--------------------------------------------------------------------------**
/**	Component information.						     **//**--------------------------------------------------------------------------**/
/**	Profiling applies to top-level components, not in-line components    **/
/**	or tables.							     **/

typedef struct cmpn_profile_t 
{
    short	start_line;
    short	line_count;
    Rsu		place;
    Direction_t	side;
    struct cmpn_profile_t *nextP;
} Cmpn_profile_t;

/**--------------------------------------------------------------------------**/
/**	Tab definitions 						     **/

typedef unsigned char Tab_type_t;
#define tab_LEFT		(0)
#define tab_CENTER	 	(1+tab_LEFT)
#define tab_RIGHT		(1+tab_CENTER )
#define tab_DECIMAL		(1+tab_RIGHT)
#define tab_DECIMAL_COMMA	(1+tab_DECIMAL	)

typedef struct tabentry_t 
{
    Rsu	    	stop;
    Tab_type_t	type;
} Tabentry_t;

typedef struct tabline_t 
{
    short	    num_stops;
    Tabentry_t	    entries[AR_MAX_TABS];
} Tabline_t;

/**--------------------------------------------------------------------------**/
typedef short	fill_t;
#define fill_OFF	(0)
#define fill_BLANK	(1+fill_OFF)
#define fill_ON		(1+fill_BLANK	)
/**--------------------------------------------------------------------------**/
typedef short	kern_track_t;
typedef short	Content_type_t;
/**--------------------------------------------------------------------------**/
extern char *Content_names[];
#define tok_BUF_PRIVATE		(0)
#define tok_BUF_PREFIX		(1+tok_BUF_PRIVATE)
#define tok_BUF_SHARED		(1+tok_BUF_PREFIX)
#define tok_BUF_PREFIX_SHARED	(1+tok_BUF_SHARED)

/**--------------------------------------------------------------------------**/
/** The component itself. **/
/**--------------------------------------------------------------------------**/

typedef struct cmpn_t 
{
    DECLARE_NullObjFields;
    Unknown_t		*unkP;
    shar		cmpn_name[AR_MAX_CMPNNAME];
    Attr_t	    	*attrvalsP;		/* cmpn's attribute values */
    Content_type_t	content_type;
    Ssave_t		body;
    bool		hidden;
    bool		subcmpn; /** BT **/
    Font_value_t	font_value;
    Rsu		    	top_margin;
    Rsu		    	bottom_margin;
    Rsu		    	left_margin;
    Rsu		    	right_margin;
    Rsu		    	first_indent;
    Rsu	    		line_spacing;		/* value X 1000 */
    short	    	indent_count;		/* indent line count */
    Tabline_t	    	tabs;			/* tab line structure */
    Direction_t		alignment;
    bool		att_prior;
    bool		pbreak_within;
    bool		att_next;
    fill_t		fill;
    bool		new_page;
    bool		new_column;
    short		widow_ctrl;
    short		orphan_ctrl;
    bool		straddle;
    short		an_level;
    bool		kern_space;
    bool		word_spacing;
    float		word_sp_max;
    float		word_sp_min;
    float		word_sp_nom;
    bool		letter_spacing;
    float		letter_sp_max;
    short		composition;	/** BT **/
    short   		hyphenation;	/* Hyphenation threshold */
    Cmpn_profile_t	*profileP;
    shar		toc_name[AR_MAX_TOCNAME];
} Cmpn_t;

/**--------------------------------------------------------------------------**/
/**	Component Content Information					     **/
/**--------------------------------------------------------------------------**/
/** Definitions of the tokens in the Component content buffer.		     **/
/** Tokens in the range [0..16383] are simple shars that represent themselves**/
/** Tokens in the range [16384..32767] are markup tokens.		     **/
/** Other values are not defined.					     **/
/** Some tokens (e.g. <HR>) represent only themselves.  		     **/
/** Others (e.g. <Index, ...>) have extra information.  These have a pointer **/
/** to a secondary structure with the information.  Thus a big token has six **/
/** bytes in the buffer, and a shar has two.  (Assuming dense   	     **/
/** structures and 4-byte pointers.  The AR code is not sensitive to that    **/
/** assumption, and client programs should not be, either.  See BufEnter().  **/
/** There is *one* case of a token that is just two bytes long.  It exists   **/
/** *only* to pad the buffer to an alignment boundary required by the	     **/
/** underlying architecture.  Clients should use BufEnter() to insert tokens,**/
/** since it knows about the alignment requirements of each machine.  The    **/
/** padding token should always be ignored.				     **/

typedef short			TOK_t;	/** MUST fit in SsSaveS() call. **/
#define TOK			(0x4000)
#define IS_TOKEN(it)		((it)&TOK)
#define TOKEN_SET(it)		((it)|TOK)
#define TOKEN_RESET(it)		((it)&~TOK)
#define TOKEN_STEP		(sizeof(Token_t)/sizeof(shar))
#define IS_PADDING(it)		((it)==(tok_PaddingNOP|TOK))

#define tok_Invalid		(0)			/**  placeholder **/
#define tok_PaddingNOP		(1+tok_Invalid)		/**  no semantics **/
#define tok_Autonum		(1+tok_PaddingNOP)	/**  AUTO_NUM **/
#define tok_Cell		(1+tok_Autonum)		/**  AL_CELL **/
#define tok_CharSpecial		(1+tok_Cell)		/**  non-print-ascii **/
#define tok_Comment		(1+tok_CharSpecial)	/** COMMENT **/
#define tok_Cmpn		(1+tok_Comment)		/** IN-LINE CMPN **/
#define tok_Data		(1+tok_Cmpn)		/** FUTURE_EXPANSION **/
#define tok_EndFrame		(1+tok_Data)		/** END_FRAME **/
#define tok_EndInline		(1+tok_EndFrame)	/** END_SUB **/
#define tok_EndSub		(1+tok_EndInline)	/** END_SUB **/
#define tok_EndTable		(1+tok_EndSub)		/** END_TABLE **/
#define tok_EndText		(1+tok_EndTable)	/** END_TEXT **/
#define tok_FirstPageFooter	(1+tok_EndText)		/** FFOOTER **/
#define tok_FirstPageHeader	(1+tok_FirstPageFooter)	/** FHEADER **/
#define tok_Font		(1+tok_FirstPageHeader)	/** FN **/
#define tok_FJ			(1+tok_Font)		/** FLUSH_RIGHT **/
#define tok_Frame		(1+tok_FJ)		/** LD_FRAME **/
#define tok_HR			(1+tok_Frame)		/** HARD_RETURN **/
#define tok_Index		(1+tok_HR)		/** INDEX_MARKUP **/
#define tok_Language		(1+tok_Index)		/** FUTURE_EXPANSION **/
#define tok_LeftPageFooter	(1+tok_Language)	/** LFOOTER **/
#define tok_LeftPageHeader	(1+tok_LeftPageFooter)	/** LHEADER **/
#define tok_Mail		(1+tok_LeftPageHeader)	/** FUTURE_EXPANSION **/
#define tok_Master_Row		(1+tok_Mail)	 	/** AL_MASTER_ROW **/
#define tok_NoOp		(1+tok_Master_Row) 	/** No-Operation **/
#define tok_Note		(1+tok_NoOp)		/** FUTURE_EXPANSION **/
#define tok_PageFooter		(1+tok_Note)		/** FOOTER **/
#define tok_PageHeader		(1+tok_PageFooter)	/** HEADER **/
#define tok_Query		(1+tok_PageHeader)	/** FUTURE_EXPANSION **/
#define tok_Ref			(1+tok_Query)		/** REF_MARKUP **/
#define tok_Rev			(1+tok_Ref)		/** FUTURE_EXPANSION **/
#define tok_RightPageFooter	(1+tok_Rev)		/** RFOOTER **/
#define tok_RightPageHeader	(1+tok_RightPageFooter)	/** RHEADER **/
#define tok_Row			(1+tok_RightPageHeader)	/** AL_ROW **/
#define tok_SP			(1+tok_Row)		/** HARD_SPACE **/
#define tok_SR			(1+tok_SP)		/** SOFT_RETURN **/
#define tok_Tab			(1+tok_SR)		/** TAB **/
#define tok_TabDash		(1+tok_Tab)		/** TAB_DASH **/
#define tok_TabDot		(1+tok_TabDash)		/** TAB_DOT **/
#define tok_Table		(1+tok_TabDot)		/** AL_TABLE **/
#define tok_TabUnder		(1+tok_Table)		/** TAB_HYPH **/
#define tok_PageBreak		(1+tok_TabUnder)	/** LD_PAGEBR **/
#define tok_Unknown		(1+tok_PageBreak)	/** default   **/
#define TOK_MAX			(tok_Unknown)	    /** keep this up-to-date **/

/**--------------------------------------------------------------------------**/
typedef struct token_t
{
    TOK_t    token;
    union {
	long		 token_data;	/** used by Language, Rev,(?), Tab   **/
	Cmpn_t		*subcmpnP;
	Font_value_t	*fontvalP;
	Autonum_t	*anumP;
	Ssave_t		*textP;	            /** shared by Comment, PageBreak **/
	Frame_t		*frameP;
	Index_t 	*indexP;
	Ref_t		*refP;
	Table_t		*tableP;
	Comment_t	*commentP;
	Unknown_t	*unkP;
    } u;
} Token_t;

/**--------------------------------------------------------------------------**/
/**	This macro is called with a (Token_t *) and an (Ssave_t *)	     **/
/**	It does assume that sizeof(Token_t) is a multiple of		     **/
/**	sizeof(short), and the overall code assumes that the zero'th field   **/
/**	is located zero bytes from the start of the Token_t structure.	     **/
/**	It does not assume dense-packed structures.			     **/

#ifdef LONGALIGNED
#define BUF_ALIGN(whereP) SsAlign(whereP, sizeof(long), tok_PaddingNOP|TOK)
#else
#define BUF_ALIGN(whereP)
#endif

#define BufEnter(thingP, whereP) \
    /** make caller's buffer refer to this item. **/ \
    { /** note this is the LONGALIGNED way to do this. **/ \
      register short *sP; \
      register int i; \
	BUF_ALIGN(whereP); \
	for (sP=(short*)thingP, i=sizeof(*thingP)/sizeof(short); i--; ) {\
	    SsSaveS(*sP++, whereP); \
	} \
    }

/**--------------------------------------------------------------------------**/
/**	This is the whole document state **/

typedef struct il_state_t 
{
    Identifier_t	id;
    Ops_info_t		ops_info;
    Document_info_t	doc_info;
    Font_info_t		*fontP;		/** points to list of defn's **/
    Color_info_t	*colorP;
    Pattern_info_t	*patternP;
    Page_info_t		page_info;
    An_stream_info_t	*an_streamP;		/** points to list of Streams**/
    Cmpn_t		*class_defaultsP;	/** points to a single Cmpn_t**/
    Cmpn_t		*cmpnP;			/** points to list of Masters**/
    Frame_t		*frameP;		/** points to list of Masters**/
    Table_t		*tableP;		/** points to list of Masters**/
    HeaderFooter_t	*hdftP;			/** points to list of defn's **/
    NullObj_t		*bodyP;			/** points to list of objects**/
} Il_state_t;

/**--------------------------------------------------------------------------**/
/**	The per-file data structure.  Not changed by the client.	     **/

typedef struct file_info_t
{
    Identifier_t id;	/** redundant. **/
    FILE 	*ifile;
    Ssave_t	inqueue;
    shar	saved_char;
} File_info_t;

/**--------------------------------------------------------------------------**/
/**	Services to clients.						     **/
/**--------------------------------------------------------------------------**/
/**	The services defined here are layered on top of the simple parsing   **/
/**	of I.A.M.  They are useful to client programs, especially input text **/
/**	filters, but they may be used or ignored freely by any client.	     **/
/**--------------------------------------------------------------------------**/

/**--------------------------------------------------------------------------**/
/**	The ArInit() data structure.					     **/
/**	Allows the client to control some of the internal wrokings of AR     **/
/**	It is downwards-compatible to previous AR libraries, because new     **/
/**	fields are added strictly at the end.				     **/

#define ARINITFIELDS (2)
typedef struct ar_init_t {
    short entries;		/** for future growth.  **/
    Rsu   client_rsu;		/** basis of rsu's. **/
    bool  double_markup;	/** control of ArEmitChar() & '<' **/
} Ar_init_t;

/**--------------------------------------------------------------------------**/
/**	This is the data structure for character mapping.  It is mostly used **/
/**	for 'special' characters, i.e. byte values greater than 127, but it  **/
/**	can map any single byte to a multi-byte replacement sequence.  The   **/
/**	idea is that in the template file, the input byte value is specified **/
/**	in decimal or hex (it may not be representable as its own self), and **/
/**	the Interleaf replacement for it is also given.  The replacement is  **/
/**	often <change to symbol font><special character><change back>	     **/
/**	In real use a  Char_map_t is an array of 256 pointers to shars.      **/

typedef shar *Char_map_t;
/**--------------------------------------------------------------------------**/
/**	I N P U T   C H A R A C T E R   M A P				     **/
/**	A map is a linked list of patterns.				     **/
/**	A pattern is a sequence of unsigned long integers, with meaning	     **/
/**	coded within each integer.  The patterns are overloaded into	     **/
/**	an array of shars, and mananged by Ssave_t functions		     **/
/**									     **/
/**	The high order 16 bits indicate the type of the item, and the low    **/
/**	bits are the value of the item.					     **/
/**									     **/
/**	A plain character has type 0; therefore the longword can be used     **/
/**	directly, without any masking.					     **/
/**									     **/
/**	The end of the pattern is marked by a longword of all one's.	     **/
/**									     **/
/**	Pattern groups, and repeat counts, are not yet implemented.	     **/
/**	Therefore each pattern is compared one-to-one with the input stream. **/
/**									     **/
/**	It's not inherent in the representation, but patterns should be	     **/
/**	sorted so that the longest ones are first.  then 'foobar' is matched **/
/**	before 'foo', which is generally desirable.			     **/
/**									     **/
/**	If implemented, repeat counts will be a minimum and a maximum,	     **/
/**	in bits 15-8 and 7-0 of the value, min 0, max 255.		     **/
/**	Groups allow repeat counts to apply to more than one item.	     **/
/**	Repeat count greatly complicate the pattern match, since it requires **/
/**	a fancy automaton to recognize it.				     **/
/**									     **/
/**--------------------------------------------------------------------------**/
typedef shar           Map_Pattern_t;
#define	AR_PLAIN_CHAR	(0)			/** must be zero!. **/
#define	AR_ANY_SINGLE	(AR_PLAIN_CHAR+1)
#define	AR_UNORDERED_PAIR	(AR_ANY_SINGLE+1)
#define	AR_RANGE_START	(AR_UNORDERED_PAIR+1)
#define	AR_RANGE_LOW	(AR_RANGE_START+1)
#define	AR_RANGE_HIGH	(AR_RANGE_LOW+1)
#define	AR_RANGE_END	(AR_RANGE_HIGH+1)
#define	AR_GROUP_START	(AR_RANGE_END+1)
#define	AR_GROUP_END	(AR_GROUP_START+1)
#define	AR_REPEAT_COUNT	(AR_GROUP_END+1)
#define	AR_PATTERN_END	((Map_Pattern_t)((long)0xFFFF))

/**--------------------------------------------------------------------------**/
/**	This has a count because it cannot be nul-terminated.  Nul can be    **/
/**	part of the replacement.					     **/
typedef struct replacement_t {
    short count;
    shar *sP;
} Replacement_t;

/**--------------------------------------------------------------------------**/
typedef enum entry_e { r_TEXT, r_FUNCTION } Entry_e;

/**--------------------------------------------------------------------------**/
typedef struct in_char_map_t {
    struct in_char_map_t  *nextP;
    Map_Pattern_t             *patternP;
    Entry_e                type;
    Replacement_t          replacement;
    bool	  	   (*repfnP)();
} In_char_map_t;

/**--------------------------------------------------------------------------**/
typedef struct in_char_info_t {
    FILE *ifile;
    In_char_map_t *mapP;
    Ssave_t in_text;
    Ssave_t rep_text;
} In_char_info_t;

/**--------------------------------------------------------------------------**/
typedef struct fnlist_t 
{
    shar   *nameP;
    bool  (*funcP)();
} FnList_t;


/**--------------------------------------------------------------------------**/
/**	This is the function interface for clients of AR        **/
/**--------------------------------------------------------------------------**/

/**--------  I N I T I A L I Z A T I O N   F U N C T I O N S   ----------**/

extern void          ArInit(/* Ar_init_t *  */);
extern Il_state_t   *ArStateInit(/* Il_state_t *  */);
extern File_info_t  *ArFileInit(/* FILE * */);
extern Cmpn_t       *ArCmpnInit(/* Cmpn_t *  */);
extern Font_value_t  ArFontValueInit();

/**--------  M E M O R Y   A L L O C A T I O N   F U N C T I O N S  ---------**/

extern char  *ArSafeMalloc(/* long  */);
extern char  *ArSafeRealloc(/* char *, unsigned  */);

/**--------  O B J E C T   D E - A L L O C A T I O N   F U N C T I O N S  ---**/

extern bool   ArObjectFree(/* NullObj_t *   */);
extern bool   ArCmpnBodyFree(/* Ssave_t *   */);
extern bool   ArTokenFree(/* Token_t *   */);
extern bool   ArDiagramFree(/* ArDgHead_t *   */);

/**--------  H I G H - L E V E L   R E A D   F U N C T I O N S  ----------**/

extern NullObj_t     *ArNextObject(/* File_info_t *, NullObj_t *  */);
extern NullObj_t     *ArDocRead(/* File_info_t *, Il_state_t *  */);

/**--------  M A P P E D   C H A R   I N P U T   F U N C  T I O N S  --------**/ 
extern In_char_map_t  *ArGetInCharMap(/**   Il_state_t *    **/);
extern In_char_info_t *ArMapGetchInit(/**   FILE*, In_char_map_t*   **/);
extern shar  ArMapGetch  (/**           In_char_info_t*   **/);
extern shar  ArMapUnGetch(/**   shar,   In_char_info_t*   **/);

/**--------  S I M P L E I/O  F U N C T I O N S  ----------**/

extern shar   ArGetch();
extern shar   ArGetchNW();
extern bool   ArGetInt();
extern bool   ArGetFloat();
extern bool   ArGetWordShrC();
extern shar   ArUnGetch();
extern shar  *ArUnGetshr();
extern void   ArEmitChar(/* unsigned shar, Char_map_t *, FILE * */);
#define       ArCharIsMapped(c,mapP) ((mapP)!=NULL && ((mapP)[(c)]!=NULL))

/**--------  H I G H - L E V E L  W R I T E   F U N C T I O N S   ----------**/

extern bool ArObjectWrite         (/* NullObj_t *,     Il_state_t *, FILE * */);
extern bool ArDocWrite            (/* Il_state_t *,       FILE *            */);
extern bool ArDocDeclWrite        (/* Il_state_t *,       FILE *            */);
extern bool ArDocBodyWrite        (/* Il_state_t *,       FILE *            */);

/**--------  W R I T E   F U N C T I O N S   ----------**/

extern void ArCommentWrite        (/* Comment_t *,        FILE *            */);
extern void ArOpsInfoWrite	  (/* Ops_info_t *,       FILE *	    */);
extern void ArDocInfoWrite	  (/* Document_t *,       FILE *	    */);
extern void ArFontInfoWrite	  (/* Font_info_t *,      FILE *	    */);
extern void ArColorInfoWrite	  (/* Color_info_t *,     FILE *	    */);
extern void ArPatternInfoWrite	  (/* Pattern_info_t *,   FILE *	    */);
extern void ArPageInfoWrite	  (/* Page_info_t *,      FILE *	    */);
extern void ArAnStreamInfoWrite	  (/* An_stream_info_t *, FILE *	    */);
extern void ArFrameMastersWrite	  (/* Frame_t *,          FILE *	    */);
extern void ArFrameWrite	  (/* Frame_t *,          FILE *	    */);
extern void ArHeaderFooterWrite	  (/* HeaderFooter_t *,   FILE *	    */);
extern void ArTableMastersWrite   (/* Table_t *,          FILE * 	    */);
extern void ArTableWrite          (/* Table_t *,       Il_state_t *, FILE * */);
extern void ArClassDefaultsWrite  (/* Cmpn_t *,           FILE *	    */);
extern void ArCmpnMastersWrite	  (/* Cmpn_t *,           FILE *	    */);
extern bool ArCmpnWrite           (/* Cmpn_t *,        Il_state_t *, FILE * */);
extern void ArCmpnTabsWrite       (/* Tabline_t *,        FILE *            */);
extern void ArTokenWrite          (/* Token_t *,          FILE *            */);
extern void ArIndexWrite          (/* Index_t *,          FILE *            */);
extern void ArRefWrite            (/* Ref_t *,            FILE *            */);
extern void ArAutonumWrite        (/* Autonum_t *,        FILE *            */);
extern void ArMicrodocWrite       (/* Microdoc_t *,       FILE *            */);
extern void ArFileInfoWrite       (/* File_info_t *,      FILE *            */);

/**--------  P R I N T F	F U N C T I O N S   ----------**/
extern long ArFprintf		(/**	strP, fmtP, arg	**/);
extern long ArSprintf		(/**	strP, fmtP, arg	**/);

/**--------  S H A R / S H O R T - S T R I N G 	F U N C T I O N S   ---------**/
extern	int	 ArShrlen	(/**	sP		**/);
extern	int	 ArShrcmp	(/**	sP1, sP2	**/);
extern	int	 ArShrncmp	(/**	sP1, sP2, n	**/);
extern	short	*ArShrcpy	(/**	sP1, sP2	**/);
extern	short	*ArShrindex	(/**	sP, c		**/);
extern	short	*ArShrrindex	(/**	sP, sh		**/);
extern	short	*ArShrcat	(/**	sP1, sP2	**/);
extern	short	*ArShrncpy	(/**	sP1, sP2, n	**/);
extern	short	*ArShrncat	(/**	sP1, sP2, n	**/);
extern	short	*ArShrstrcat	(/**	sP, cP		**/);
extern	short	*ArShrstrcpy	(/**	sP1, sP2	**/);
extern	int	 ArShrstrcmp	(/**	sP1, sP2	**/);
extern	int	 ArShrstrncmp	(/**	sP1, sP2, n	**/);
extern	int	 ArStrtoshr	(/**	sP, cP		**/);
extern	long	 ArAtoi		(/**	cP		**/);
extern	char	*ArShrnstr	(/**	dsP, scP, n	**/);
extern	int	 ArAtoiS	(/**	sP		**/);

/**--------------------------------------------------------------------------**/
/** That's all, folks! **/
#endif
