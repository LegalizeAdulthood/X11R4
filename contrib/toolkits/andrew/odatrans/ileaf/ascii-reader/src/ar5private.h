
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
/*	ar5private.h                             */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  06/88    BT	Original coding
 */

#ifndef _AR5PRIVATE_H
#define _AR5PRIVATE_H
/**--------------------------------------------------------------------------**/
/**	This file defines the private interface for the ASCII Reader (AR)     **/
/**--------------------------------------------------------------------------**/

#define ARversion 	    (627) /* version X.xx */

#ifdef NEVER

Version	   date		who	what
-------	   ------	---	-----------------------------------------------
6.27	   06sep89	mss	Copyrights, Imakefile and other modifications for
                                X tape distribution as part of CMU's interleaf to
                                oda translator
6.26       29aug89      kint    Assorted "cleanup" for gcc by MD. (See xops.ar6)
                                Fixed loss of table footer border info (ar5table.c)
                                Wrong # args to ArGetColorSep() (ar5read.c, ar5lex.c)
                                Fixed typo in dump of chart version # (ar5dumpDg.c)
                                Repaired h, lo, D, & DX chart fields (ar5defs.h,
				 ar5loadDg.h, ar5cvt.c)
                                Output units of frame horoz align data (ar5dump.c)
                                Fixed reading of profile data (ar5cmpn.c, ar5dump.c)
                                Fixed output of attribute references (ar5ref.c)
                                Reordered HeaderFooter_names[] to agree w/defines
                                 in ar5defs.h (ar5strings.c)
                                Fixed cell autosize handling (ar5frameread.c, 
                                 ar5dump.c, ar5init.c)
6.25       17aug89      kint    Fixed LONGALIGNED bug where tok_Padding shar was
                                 being assigned an incorrect value. (ar5defs.h)
6.24       26jul89      kint     Assorted bug fixes:
                                Outline fonts now have a dynamic buffer (Ar5loadDg.c)
                                Cleanup of init of Page_info_t flags (ar5init.c)
                                Cleanup init and comments ar5frameread.c
                                Fixed frame anchor visibility (ar5dump.c)
                                Fixed wordspace, kern, letterspace (ar5init.c)
                                Corrected kern font attribute output (ar5font.c)
                                Fixed output of profile data (ar5dump.c)
                                Fixed storage of color & pattern info (ar5docread.c)
                                Fixed output of cell fixed-widith/height
                                 info (ar5table.c)
                                Fixed "doc=" field of index data (ar5index.c)
                                Fixed ArAutosizeWrite() to handle a mixture of fixed
                                 and variable dimensions (ar5dump.c)
6.23       18jul89      kint    Rewrote ar/ar5_0/Makefile (to make dev_kit).
                                Created dev_kit subdirectory.
                                Fixed %[cC] bug in ar5printf.c AND arprinf.c (ar3_5)
                                Created a new ar5test.doc.
                                Corrected unit conversion bug involving picas,
                                 and ciceros.
                                Changed size of "stop" from short to Rsu in
                                 ArGettab().
                                Use temp pointer for  LL traversal in ArMicrodocWrite.
                                Cleaned up frame initialization stuff in ar5init.c().
6.22       01jul89      kint    Rewrote ar5printf to use varargs().       
6.22       06jun89      kint    Changed table margin read to accept neg left
                                 margins (ar5table.c)
                                Fixed handling of quoted component names in
                                 microdocs, quotes were being ignored (ar5cmpn.c)

6.21	   22may89	MS	Fix freeing of grouped objects in ar5freeDg.c
				Fix parsing of empty components in microdocs.

6.20	   10may89	MS	Malloc extra byte for null string terminator
					in ar5table.c.
				Parse Straddle property for frames in tables.
				Separate Auto Frame size and content size
					properties.
				Fix +n inches in auto sizing for frames.
				Parse quoted attributes.
				Parse empty table cells correctly (I think).
				Implement Encapsulated PostScript read/write.
				Fix read/write of referenced images.
				Remove leading zero in font @Z attribute.
#endif

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
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
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#include "ar5defs.h"
#include "ar5errs.h"
/**--------------------------------------------------------------------------**/
/** number of objects in an array.  only works where sizeof(athing) is known **/

#ifndef NUM_OF
#define NUM_OF(athing) (sizeof(athing)/sizeof(athing[0]))
#endif

/**--------------------------------------------------------------------------**/
#define NEW(type) ((type *)ArSafeMalloc(sizeof(type)))
/** convert integer inches (100 == 1 inch) to Rsu units.		     **/
#define I2R(f) ((Rsu)(((f) * DEF_RSUINCH) / 100.0))

/**--------------------------------------------------------------------------**/
/** significant lexical characters in Interleaf ASCII Markup.		     **/
#define AR_CMD_CHAR   ('<')	    /* start a markup command */
#define AR_CMD_TERM   ('>')	    /*   end a markup command */
#define AR_PROP_TERM  (',')	    /*   end a property statement */
#define AR_CMNT_CHAR  ('-')	    /* -- is comment -- */
#define AR_DECL_CHAR  ('!')	    /* start a declaration */
#define AR_HEX_CHAR   ('#')	    /* start a <#xx> special character */
#define AR_HYPH_CHAR  ('-')	    /* <-> for discretionary hyphen */
#define AR_PAGE_CHAR  ('|')	    /* <|,...> for page-break info */
#define AR_ATTR_CHAR  ('@')	    /* @name=val   for effectivity attributes */
#define AR_FONT_POP   ('/')	    /* for </F> command */

/**--------------------------------------------------------------------------**/
/*  NOTE:  When sorting keytab tables, ignore case and blanks! */

struct keytab 
{
    char* keyword;
    short keyval;
};
/**--------------------------------------------------------------------------**/
extern shar EmptyName_shar[];
/**--------------------------------------------------------------------------**/
extern bool ARdebug;
/**--------------------------------------------------------------------------**/
extern NullObj_t *arObjectPushBack();
extern NullObj_t *arObjectPreFetch();
/**--------------------------------------------------------------------------**/
/**	See the comment in ar5defs.h about NullObj_t to understand these.    **/
/**	Only AR internal functions use them; a client can do it also, but    **/
/**	I don;t provide that service, it's likely to be more confusing than  **/
/**	helpful.							     **/

#define INIT_NullObjFields(ID) ID, NULL, NULL

#define SET_NullObjFields(thingP, ID) \
	 ((NullObj_t*)thingP)->id     = (ID), \
	 ((NullObj_t*)thingP)->nextP  = NULL, \
	 ((NullObj_t*)thingP)->spareP = NULL

/**--------------------------------------------------------------------------**/
/** new guys. **/
extern NullObj_t *ArBuildUnknownDecl();
extern Unknown_t *ArBuildUnknownProp();
extern Unknown_t *ArBuildUnknownPropEq();
extern Unknown_t *ArBuildUnknownPropPrefixEq();
extern void ArUnknownWrite();
extern void ArUnknownFree();
/**--------------------------------------------------------------------------**/
#define MALLOC  ArSafeMalloc
#define REALLOC ArSafeRealloc
#define FREE    free
/**--------------------------------------------------------------------------**/
#define FPRINTF fprintf
/** convert Rsu's to inches, in floating point, by the client's basis.	     **/
#define R2I(r) ((double)( ((double)(r))/((double)AR_init_info.client_rsu) ) )
extern char AR_dump_yesstr[];
extern char  AR_dump_nostr[];
extern char  AR_dump_onstr[];
extern char AR_dump_offstr[];
#define YesNo(x) (((x)==YES)? AR_dump_yesstr  : AR_dump_nostr)
#define OnOff(x) (((x)==YES)? AR_dump_onstr   : AR_dump_offstr)
/**--------------------------------------------------------------------------**/
extern short ld_error;		/** may become obsolete **/
extern struct keytab *ArBinaryShr();
extern NullObj_t  *ArBuildObject();
extern NullObj_t  *ArBuildCmpnName();
extern NullObj_t  *ArBuildCmpnShar();
extern NullObj_t  *ArBuildCmpnCmd();
extern NullObj_t  *ArBuildFromCmd();
extern NullObj_t  *ArBuildTable();
extern Frame_t    *ArBuildFrame();
extern Autonum_t  *ArBuildAutonum();
extern Microdoc_t *ArBuildMicrodoc();
extern NullObj_t  *ArBuildHeaderFooter();
extern void        ArPageInfoInit();
extern void        ArPageInfoSetContext();
extern void        ArSkipPastTerm();
extern void        ArSkipPastComma();
extern bool        ArFontTokenShr();
extern bool        ArGetColorSep();
extern bool        ArGetPRsu();
extern bool        ArGetPZRsu();
extern bool        ArGetRsu();
extern bool        ArGetHex();
extern int         ArDoHex();
extern bool        ArFontGetValue();
extern bool        ArFontValueEqual();
extern void        ArCmpnSetDefault();
extern void        arParseUserAttr();
extern short       arNameSearch();
extern int	   ArIsCommandName();
extern bool        arBufGetComment();
extern bool        arBufGetPageBreak();
extern bool        arBufGetHR();
extern bool        arBufGetSP();
extern bool        arBufGetFJ();
extern bool        arBufGetRef();
extern bool        arBufGetIndex();
extern bool        arBufGetFontCmd();
extern bool        arBufGetFontValue();
extern bool        arBufGetTab();
extern bool        arBufGetAutonum();
extern bool        arBufGetFrame();
extern void        ArTextBodyWrite();
extern bool        ArCmpnFree();
extern bool        ArTableFree();
extern void	   ArAutonumFree();
extern void	   ArIndexFree();
extern int 	   ArAtoiS();
/*
 * Diagramming function declarations:
 */
extern ArDgHead_t *ArSaveDg();
extern void ArSkipDg();
extern void ArSkipObject();
extern ArDgObject_t *ArSaveObject(), *ArSaveGroupInfo();
extern LineTSD_t *ArSaveLineInfo();
extern EditStateTSD_t *ArSaveEditInfo();
extern PolyTSD_t *ArSavePolyInfo();
extern ArcTSD_t *ArSaveArcInfo();
extern SplineTSD_t *ArSaveSplineInfo();
extern EllipseTSD_t *ArSaveEllipseInfo();
extern TextstrTSD_t *ArSaveTextstrInfo();
extern ImageTSD_t *ArSaveImageInfo();
extern RASTER_t *ArSaveRaster();
extern HEX_t *ArSaveHex();
extern BoxTSD_t *ArSaveBoxInfo();
extern EqnTSD_t *ArSaveEqnInfo();
extern MicroTSD_t *ArSaveMicroInfo();
extern SplineSegTSD_t *ArSaveSplineSegInfo();
extern VectorListTSD_t *ArSaveVectorListInfo();
extern EpsTSD_t *ArSaveEPSInfo();
extern OutlineTSD_t *ArSaveOutlineInfo();
extern ChartTSD_t *ArSaveChartInfo();
extern CtRecord_t *get_ct_record();
extern CtValue_t *get_ct_value();

/**--------------------------------------------------------------------------**/
#define Isdigit(ch) ((ch) >= '0' && (ch) <= '9')

/** see ald_chtype[] for the uses of these.				     **/
/** note that values greater than (?) LTR must be quoted in cmpn names, etc. **/

#define CTL 0  /* not used */
#define OTH 1  /* not used */
#define DIG 2
#define LTR 3
#define WHT 4	/* blank, tab, CR, LF, FF */
#define SPC 5	/* , = < > @ " '               these terminate a string */
#define FST 6   /* ! # -                       first character after < */
#define QUO 7	/* | del		       must quote, but not SPC */
#define RES 8	/* { } ~ \ ^ `  	       reserved */

extern char ald_chtype[256];

/**--------------------------------------------------------------------------**/
#define NOCHAR	(0)	/** used in calling ArGetWordShr()		     **/
#define CHARCASEBIT	(0x20)	/**(letter|CHARCASEBIT)==>lowercase letter.**/
#define RSUMAX   	(0x7FFFFFFF)
/**--------------------------------------------------------------------------**/
#define Lmarkup    ('<')
#define Rmarkup    ('>')
#define Lbracket   ('[')
#define Rbracket   (']')
#define Lbrace     ('{')
#define Rbrace     ('}')
#define EQsign     ('=')
#define Bslash     ('\\')
#define COMMA ','

/**--------------------------------------------------------------------------**/
/** is page_info context whole doc or microdoc? used by ArBuildPageInfo	     **/
#define pg_MACROCONTEXT	(0)		
#define pg_MICROCONTEXT	(1+pg_MACROCONTEXT)	

/**--------------------------------------------------------------------------**/
#define NO_MATCH  -1	/* No "=" after name */
#define NO_EQUALS -2	/* No "=" after name */

#define EXACT 1
#define LOOSE 0
#define ALD_CHTYP_MAX	(255)
#ifndef LINES
#define LINES 0
#endif
/** #define UNDEF -100	/* magic number for "empty" **/
#define AR_QUOTED 2	/* returned by AlGetWord if it was quoted */

#define P_DUMMY  (0)	/* I made this up.   BT*/
#define P_TEXT  1	/* "type" args to AlParseCmpn & AlDoComponent */
#define P_CLASS 2
#define P_DEFAULT 3
#define P_ROW 4
#define P_MROW 5	/* master row */
#define P_DUP_MASTER 6	/* a duplicate class or master row */

#define LD_BOTTOM	1
#define LD_LEFT		2
#define LD_RIGHT	3
#define LD_TOP		4
#define LD_HEADER	5
#define LD_FOOTER	6

#define AL_TEXT_OK	0
#define AL_TEXT_QUOTED	1
#define AL_TEXT_ERROR	2

#define MAX_INCLUDES 4  /* max depth of file includes */
#define MAXFNLEN  80	/* max include file name length */

/* FN must be unique among all token classes */
#define FN          100

/**--------------------------------------------------------------------------**/
#define FPAGE		0   /* these correspond to order of cl_hf_pointers */
#define RPAGE		1
#define LPAGE		2
#define UPAGE		3

#define	HEADL		0
#define	HEADC		1
#define	HEADR		2
#define	FOOTL		3
#define	FOOTC		4
#define	FOOTR		5

/* These are the TPS conversion factors from unitdefs.h */
/*  Ar uses them only for their inverses (see below)    */  
#define INCHES_PER_POINT        (1.0 / 72.27)
#define INCHES_PER_PICA         (12.0 * INCHES_PER_POINT)
#define INCHES_PER_DIDOT        (1238.0 / 1157.0 * INCHES_PER_POINT)
#define INCHES_PER_CICERO       (12.0 * INCHES_PER_DIDOT)

/* These are the conversion factors actually used by ar */
#define LINE_PER_INCH		(6.0)  /** default; should never be used **/
#define CM_PER_INCH		(2.54)
#define MM_PER_INCH		(25.4)
#define POINTS_PER_INCH		(1 / INCHES_PER_POINT)
#define PICAS_PER_INCH		(1 / INCHES_PER_PICA)
#define DIDOT_PER_INCH		(1 / INCHES_PER_DIDOT)
#define CICERO_PER_INCH		(1 / INCHES_PER_CICERO)
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
extern Il_state_t   	default_il_state;
extern Page_info_t  	default_micro_page_info;
extern An_stream_info_t	default_an_stream;
extern Index_t   	default_index_entry;
extern Autosize_t   	default_fr_autosize;
extern Frame_t      	default_frame;
extern Autosize_t   	default_fr_cell_autosize;
extern Frame_t      	default_fr_cell;
extern Table_rule_t	default_table_rule;
extern Table_border_t	default_table_border;
extern TL_ruling_t	default_TL_ruling;
extern Cell_t		default_table_cell;
extern Row_t		default_table_row;
extern Table_column_t	default_table_column;
extern Table_t		default_table;
extern Cmpn_t       	default_cmpn;
extern Cmpn_t       	default_metric_cmpn;
extern Cmpn_t       	output_cmpn;
extern Cmpn_t      	*active_default_cmpnP;
extern Cmpn_t      	*AR_prev_cmpnP;
extern Cmpn_t       	AR_prev_cmpn;
extern NullObj_t   	*AR_prev_objectP;
extern Il_state_t  	*AR_il_stateP;
extern Ar_init_t    	AR_init_info;
/**--------------------------------------------------------------------------**/
extern Font_inherit_t font_inherit_all;
extern Font_inherit_t font_inherit_nothing;
extern Font_inherit_t al_font_inherit;
extern short al_font_number;
extern Font_attr_t    al_attrs;
extern Cmpn_t* al_cmpn;
extern int     al_cmpn_type;
extern Cmpn_t* al_first_cmpn;
extern Table_t* al_table;
extern rsu al_table_width;
#if 0	/** BT **/
extern TCOL* al_table_col;
#endif	/** BT **/
extern Cmpn_t* al_subcmpn;
extern short al_dfont;		/* global default font (formerly Doc.dfont) */
extern int al_cmd_term;
extern int al_cmd_char;
extern int al_hex_char;
extern int al_com_char;
extern int al_cmd_end;
extern int al_num_cmpn;
extern Cmpn_t* al_last_cmpn;
extern int al_newline_count;
extern int al_line_number;
extern int al_err_count;
extern int al_eol_flag;
extern int al_legal_char;
extern int al_bs_discarded;	    /* # of backspace characters discarded */
extern int al_us_discarded;	    /* # of underscore characters discarded */
extern bool al_declarations_finished; /* TRUE when cmpn or head/foot seen */
extern bool al_whole_document;
extern bool al_cancel;
extern bool al_micro;
extern bool al_micro_in_master;     /* set if micro-doc in a <!Class */
extern Page_info_t* al_micro_pgAttrs;   /* page attrs of micro doc */
extern bool al_filled;		    /* true if "filled" (false means put
							 HR each line) */
extern bool al_bfilled;		    /* fill = blank */
extern long al_gluepos;		    /* position of last glue in component */
extern short al_cur_font;	    /* "ecrit" font */
extern short al_cmpn_dfont;	    /* "ecrit" font */
extern long al_cmpn_size;
extern char al_token_word[];  /* most all tokens are read in here */
extern short al_token_shr[];	    /*   or here */
extern int al_opsa_version;
/** extern Cmpn_t  al_default_cmpn; /* master defaults are copied here BT **/
/** extern Rulings_t al_table_default_rulings; BT **/
extern int al_unused_classes;
extern bool al_selected;  /* true if this cmpn selected */
extern bool al_no_att_prev;
extern bool al_old_widorph;   /* old meanings for widow/orphan */
extern bool al_old_frames;    /* old style baseline */
extern bool al_swap_bot_lst;  /* "bottom" and "following text" reversed */
extern bool al_diag_ctrl;     /* diagramming can have control characters */
extern bool al_qs_hex;	      /* set if quoted string can contain <#xx> etc. */
extern bool al_markup;
extern bool al_rab_quote;     /* set if '>' is quoted in diagram text strings */
extern bool al_diag_in_frame; /* set if frames have Diagram = */
extern bool al_14_tabs;	      /* set if 14 tabs only */
extern bool al_warn_line_no;  /* true if warning msg should include line#*/
extern bool al_master_frm_seen;    /* set if master frame markup seen */
extern bool al_master_tbl_seen;    /* set if master table markup seen */
extern bool al_hyphenate;     /* set if hyphenation desired */
extern bool al_masters_only;  /* set if reading styles sheet */
extern bool al_old_orient;    /* set if paper orientation in pgattrs */

extern int al_incl_cnt;	      /* include filename stack */
extern char al_filename[MAX_INCLUDES+1][MAXFNLEN];	/* file name */

/**--------------------------------------------------------------------------**/
/**	these used to be in one file, with their keytab structure, but	     **/
/**	I couldn't maintain that any more.   BT				     **/

#ifndef COMMENT
#define COMMENT     1	/* keep these matched with case statement */
#endif
/*#define OPSCHART    2 /* gone */
/*#define DIAGRAM     3	/* gone, can be reused */
#define HARD_RETURN 4
#define FOOTER      5
#define LD_FRAME    6
#define HEADER      7
#define HARD_SPACE  8
#define TAB	    9
#define TAB_DASH    10
#define TAB_DOT     11
#define TAB_HYPH    12
#define LFOOTER	    13
#define LHEADER	    14
#define RFOOTER	    15
#define RHEADER	    16
#define FLUSH_RIGHT 17
#define END_FRAME   18
#define FFOOTER	    19
#define FHEADER	    20
#define LD_PAGEBR   21
#define FUTURE_EXPANSION   22
#define AUTO_NUM    23
#define END_SUB		(24)	/** I added this. TPS uses only END_TEXT. BT **/
#define END_TEXT	(1+END_SUB)
#define INDEX_MARKUP	(1+END_TEXT)
#define REF_MARKUP	(1+INDEX_MARKUP)
#define AL_TABLE	(1+REF_MARKUP)
#define AL_CELL		(1+AL_TABLE)
#define AL_ROW		(1+AL_CELL)
#define END_TABLE	(1+AL_ROW)
#define AL_MASTER_ROW	(1+END_TABLE)
#define AL_SOFT_RETURN	(1+AL_MASTER_ROW)
#define AL_NO_OP	(1+AL_SOFT_RETURN)

/**--------------------------------------------------------------------------**/
/**	These format strings are used in output of markup from AR.	     **/
/**	They cover all non-diagramming output.				     **/
/**--------------------------------------------------------------------------**/
extern char AR_DECL_START_FMT[];
extern char AR_DECL_END_FMT[];
extern char AR_OPS_DECL_FMT[];
extern char AR_CMD_PLAIN_FMT[];
extern char AR_CMD_START_FMT[];
extern char AR_CMD_START_LINE_FMT[];
extern char AR_CMD_END_FMT[];
extern char AR_CMPN_START_FMT[];
extern char AR_CMPN_START_LINE_FMT[];
extern char AR_FONT_START_FMT[];
extern char AR_FONT_END_FMT[];
extern char AR_OBJ_NAME_FMT[];
extern char AR_INDEX_HEAD_FMT[];
extern char AR_NAME_PAIR_FMT[];
extern char AR_AN_LEVELS_FMT[];
extern char AR_PLAIN_STRING_FMT[];
extern char AR_PROP_NAME_FMT[];
extern char AR_PROP_LEVEL_NAME_FMT[];
extern char AR_PROP_STRING_FMT[];
extern char AR_PROP_LEVEL_STRING_FMT[];
extern char AR_PROP_YESNO_FMT[];
extern char AR_PROP_LEVEL_YESNO_FMT[];
extern char AR_PROP_PREFIX_YESNO_FMT[];
extern char AR_PROP_DECIMAL_FMT[];
extern char AR_PROP_LEVEL_DECIMAL_FMT[];
extern char AR_PROP_COLUMN_DECIMAL_FMT[];
extern char AR_PROP_PREFIX_DECIMAL_FMT[];
extern char AR_PROP_PREFIX_SUFFIX_DEC_FMT[];
extern char AR_PROP_STRING_DECIMAL_FMT[];
extern char AR_PROP_INCH_FMT[];
extern char AR_PROP_COLUMN_INCH_FMT[];
extern char AR_PROP_PREFIX_INCH_FMT[];
extern char AR_PROP_LINES_FMT[];
extern char AR_PROP_3F_FMT[];
extern char AR_PLAIN_3F_FMT[];
extern char AR_TAB_3F_FMT[];
extern char AR_ATTR_VAL_FMT[];
extern char AR_FONT_DEF_FMT[];
extern char AR_PROP_KEYWORD_FMT[];
extern char AR_PROP_KEYWORD_EQ_FMT[];
extern char AR_OPS_VERSION_FMT[];
extern char AR_TYPE_INDEX_FMT[];
extern char AR_COLOR_DESC_FMT[];
extern char AR_LEFT_TAB_FMT[];
extern char AR_RIGHT_TAB_FMT[];
extern char AR_CENTER_TAB_FMT[];
extern char AR_DEC_TAB_FMT[];
extern char AR_DEC_COMMA_TAB_FMT[];
extern char AR_UNKNOWN_DECL_FMT[];
extern char AR_UNKNOWN_PROP_FMT[];
extern char AR_DECL_COMMENT_FMT[];
extern char AR_TEXT_CHAR_FMT[];
extern char AR_CHAR_SPECIAL_FMT[];
extern char AR_PATTERN_HEX_FMT[];
extern char AR_PLUS_OFFSET_INCH_FMT[];
extern char AR_PLUS_OFFSET_FMT[];
extern char AR_MINUS_OFFSET_INCH_FMT[];
extern char AR_MINUS_OFFSET_FMT[];
extern char AR_TIMES_OFFSET_FMT[];
extern char AR_PAGE_BREAK_FMT[];
extern char AR_COMMENT_DECL_FMT[];
extern char AR_TAB_PLAIN_FMT[];
extern char AR_TAB_LEADER_FMT[];
extern char AR_FONT_LANG_FMT[];
/**--------------------------------------------------------------------------**/
#define DECL_START_FMT			AR_DECL_START_FMT
#define DECL_END_FMT			AR_DECL_END_FMT
#define OPS_DECL_FMT			AR_OPS_DECL_FMT
#define CMD_PLAIN_FMT			AR_CMD_PLAIN_FMT
#define CMD_START_FMT			AR_CMD_START_FMT
#define CMD_START_LINE_FMT		AR_CMD_START_LINE_FMT
#define CMD_END_FMT			AR_CMD_END_FMT
#define CMPN_START_FMT			AR_CMPN_START_FMT
#define CMPN_START_LINE_FMT		AR_CMPN_START_LINE_FMT
#define FONT_START_FMT			AR_FONT_START_FMT
#define FONT_END_FMT			AR_FONT_END_FMT
#define OBJ_NAME_FMT			AR_OBJ_NAME_FMT
#define INDEX_HEAD_FMT			AR_INDEX_HEAD_FMT
#define NAME_PAIR_FMT			AR_NAME_PAIR_FMT
#define AN_LEVELS_FMT			AR_AN_LEVELS_FMT
#define PLAIN_STRING_FMT		AR_PLAIN_STRING_FMT
#define PROP_NAME_FMT			AR_PROP_NAME_FMT
#define PROP_LEVEL_NAME_FMT		AR_PROP_LEVEL_NAME_FMT
#define PROP_STRING_FMT			AR_PROP_STRING_FMT
#define PROP_LEVEL_STRING_FMT		AR_PROP_LEVEL_STRING_FMT
#define PROP_YESNO_FMT			AR_PROP_YESNO_FMT
#define PROP_LEVEL_YESNO_FMT		AR_PROP_LEVEL_YESNO_FMT
#define PROP_PREFIX_YESNO_FMT		AR_PROP_PREFIX_YESNO_FMT
#define PROP_DECIMAL_FMT		AR_PROP_DECIMAL_FMT
#define PROP_LEVEL_DECIMAL_FMT		AR_PROP_LEVEL_DECIMAL_FMT
#define PROP_COLUMN_DECIMAL_FMT		AR_PROP_COLUMN_DECIMAL_FMT
#define PROP_PREFIX_DECIMAL_FMT		AR_PROP_PREFIX_DECIMAL_FMT
#define PROP_PREFIX_SUFFIX_DEC_FMT	AR_PROP_PREFIX_SUFFIX_DEC_FMT
#define PROP_STRING_DECIMAL_FMT		AR_PROP_STRING_DECIMAL_FMT
#define PROP_INCH_FMT			AR_PROP_INCH_FMT
#define PROP_COLUMN_INCH_FMT		AR_PROP_COLUMN_INCH_FMT
#define PROP_PREFIX_INCH_FMT		AR_PROP_PREFIX_INCH_FMT
#define PROP_LINES_FMT			AR_PROP_LINES_FMT
#define PROP_3F_FMT			AR_PROP_3F_FMT
#define PLAIN_3F_FMT			AR_PLAIN_3F_FMT
#define TAB_3F_FMT			AR_TAB_3F_FMT
#define ATTR_VAL_FMT			AR_ATTR_VAL_FMT
#define FONT_DEF_FMT			AR_FONT_DEF_FMT
#define PROP_KEYWORD_FMT		AR_PROP_KEYWORD_FMT
#define PROP_KEYWORD_EQ_FMT		AR_PROP_KEYWORD_EQ_FMT
#define OPS_VERSION_FMT			AR_OPS_VERSION_FMT
#define TYPE_INDEX_FMT			AR_TYPE_INDEX_FMT
#define COLOR_DESC_FMT			AR_COLOR_DESC_FMT
#define LEFT_TAB_FMT			AR_LEFT_TAB_FMT
#define RIGHT_TAB_FMT			AR_RIGHT_TAB_FMT
#define CENTER_TAB_FMT			AR_CENTER_TAB_FMT
#define DEC_TAB_FMT			AR_DEC_TAB_FMT
#define DEC_COMMA_TAB_FMT		AR_DEC_COMMA_TAB_FMT
#define UNKNOWN_DECL_FMT		AR_UNKNOWN_DECL_FMT
#define UNKNOWN_PROP_FMT		AR_UNKNOWN_PROP_FMT
#define DECL_COMMENT_FMT		AR_DECL_COMMENT_FMT
#define TEXT_CHAR_FMT			AR_TEXT_CHAR_FMT
#define CHAR_SPECIAL_FMT		AR_CHAR_SPECIAL_FMT
#define PATTERN_HEX_FMT			AR_PATTERN_HEX_FMT
#define PLUS_OFFSET_INCH_FMT		AR_PLUS_OFFSET_INCH_FMT
#define PLUS_OFFSET_FMT			AR_PLUS_OFFSET_FMT
#define MINUS_OFFSET_INCH_FMT		AR_MINUS_OFFSET_INCH_FMT
#define MINUS_OFFSET_FMT		AR_MINUS_OFFSET_FMT
#define TIMES_OFFSET_FMT		AR_TIMES_OFFSET_FMT
#define PAGE_BREAK_FMT			AR_PAGE_BREAK_FMT
#define COMMENT_DECL_FMT		AR_COMMENT_DECL_FMT
#define TAB_PLAIN_FMT			AR_TAB_PLAIN_FMT
#define TAB_LEADER_FMT			AR_TAB_LEADER_FMT
#define FONT_LANG_FMT			AR_FONT_LANG_FMT
/**--------------------------------------------------------------------------**/
#endif
