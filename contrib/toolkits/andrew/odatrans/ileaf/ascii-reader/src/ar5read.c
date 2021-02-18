
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
/*	ar5read.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original AR version, derived from alMain.c
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
/** local routines and stubs: 						     **/
/**--------------------------------------------------------------------------**/
extern NullObj_t *ArBuildComment();
extern NullObj_t *ArBuildDocumentInfo();
extern NullObj_t *ArBuildFontDefs();
extern NullObj_t *ArBuildOPSDecl();
extern NullObj_t *ArBuildClass();
extern NullObj_t *ArBuildClassDef();
extern NullObj_t *ArBuildPageInfo();
extern NullObj_t *ArBuildAnStreamInfo();
extern NullObj_t *ArBuildColorInfo();
extern NullObj_t *ArBuildPatternInfo();
extern NullObj_t *ArBuildHeaderFooter();
/**--------------------------------------------------------------------------**/
extern NullObj_t *ArBuildCmpn();
extern NullObj_t *ArBuildCmpnName();
extern NullObj_t *ArBuildCmpnShar();
extern NullObj_t *ArBuildCmpnCmd();
Tabline_t ArDefaultTabline() { };
int ArSkipFrame() { return(0); }
int ArSkipDiagram() { return(0); }
/**--------------------------------------------------------------------------**/
NullObj_t *
arObjectPushBack(thingP)
  register NullObj_t *thingP;
{
    thingP->nextP = AR_prev_objectP;
    AR_prev_objectP = thingP;
    return( thingP );
}
/**--------------------------------------------------------------------------**/
NullObj_t *
arObjectPreFetch()
{
  register NullObj_t *resultP;

    if ((resultP = AR_prev_objectP) != NULL) {
	AR_prev_objectP = AR_prev_objectP->nextP;
    }
    return( resultP );

}
/**--------------------------------------------------------------------------**/
NullObj_t *ArSkipBTObject(fiP, sP)
  File_info_t *fiP;
  shar *sP;
{ 
  static shar skip_msg[] = 
      {'S','k','i','p','p','i','n','g',' ','t','h','i','s',':',' ', 0 };

    ArUnGetshr(sP, fiP);
    ArUnGetshr(skip_msg, fiP);
    return(ArBuildComment(fiP));
}

/**--------------------------------------------------------------------------**/
#define OPSVERSION 1
#define OPSSTYLE   2
struct keytab opsdecl_tokens[] = {  /* this list must remain sorted */
    {"Style", OPSSTYLE},
    {"Version", OPSVERSION}
};
#define NUMODS (sizeof(opsdecl_tokens) / sizeof(struct keytab))
/**--------------------------------------------------------------------------**/
#ifndef COMMENT
#define COMMENT (1)
#endif
#define CLASS       42  /* keep these matched with case statement */
#define CLASS_DEF   43
#define DOCUMENT    44
#define EQUATE      45
#define FONTS	    46
#define INCLUDE     47
#define INCLUDE_DECL 48
#define OPSDECL     49
#define PAGE_DEF    50
#define STATE       51
#define AUTOSTREAM  52
#define LD_MASTER_FRM	53
#define LD_MASTER_TBL	54
#define CLR_PALETTE 55
#define PAT_PALETTE 56

struct keytab declarative_names[] = {       /* this list must remain sorted! */
    {"Autonumber Stream",AUTOSTREAM},
    {"Class",CLASS},
    {"Class Defaults",CLASS_DEF},
    {"Color Definitions",CLR_PALETTE},
    {"Colors",CLR_PALETTE},
    {"Comment",COMMENT},
    {"Document",DOCUMENT},
    {"Equate",EQUATE},		    /* not used */
    {"Font Definitions",FONTS},
    {"Fonts",FONTS},
    {"Include",INCLUDE},
    {"Include Declarations",INCLUDE_DECL},
    {"Master Frame",LD_MASTER_FRM},
    {"Master Table",LD_MASTER_TBL},
    {"OPS",OPSDECL},
    {"Page",PAGE_DEF},
    {"Pattern Definitions",PAT_PALETTE},
    {"Patterns",PAT_PALETTE},
    {"State",STATE}
};


#define DOCFONT		1
#define HEADER_PAGE	2
#define HYPHEN		3
#define UNUSED_CLASSES	4   
#define PRINTER_TYPE	5
#define TARGET_PRINTER	6
#define LADDER		7
#define AL_LANGUAGE	8
#define AL_DOUBLE_SIDED	9
#define AL_MANUAL_FEED	10
#define AL_PRINT_REV	11
#define AL_PRINT_STR	12
#define AL_PRINT_UND	13
#define AL_UNIT		14
#define AL_APAGE_STYLE	15
#define AL_APAGE_NUM	16
#define AL_UNDER_POS	17
#define PAPER_ORIENT	18
#define PAPER_ORIENT_OLD	19
#define AL_APAGE_PREFIX	20
#define AL_COLOR_PROC	21

struct keytab al_doc_tokens[] = {  /* this list must remain sorted */
    {"A-Page # Prefix", AL_APAGE_PREFIX},
    {"A-Page # Style", AL_APAGE_STYLE},
    {"Consecutive Hyphens", LADDER},	    /* moved to <!Page> */
    {"Default Printer", TARGET_PRINTER},
    {"Double Sided", AL_DOUBLE_SIDED},
    {"Final Output Device", PRINTER_TYPE},  /* old */
    {"Font", DOCFONT},			    /* old */
    {"Header Page", HEADER_PAGE},
    {"Hyphenation", HYPHEN},		    /* moved to <!Page> */
    {"Keep Unused Classes", UNUSED_CLASSES},
    {"Language", AL_LANGUAGE},		    /* 2.5 foreign language hack */
    {"Manual Sheet Feed", AL_MANUAL_FEED},
    {"Maximum Page #", AL_APAGE_NUM},
    {"Measurement Unit", AL_UNIT},
    {"Orientation Inverted",PAPER_ORIENT},
    {"Paper Orientation",PAPER_ORIENT_OLD}, /* old */
    {"Printer Type", PRINTER_TYPE},	    /* old --note order */
    {"Print Rev Bars", AL_PRINT_REV},
    {"Print Strikes", AL_PRINT_STR},
    {"Print Underlines", AL_PRINT_UND},
    {"Spot Color Separation", AL_COLOR_PROC},
    {"Target Printer", TARGET_PRINTER},	    /* old */
    {"Underline at Descender", AL_UNDER_POS},
};
#define NUMDOCS (sizeof(al_doc_tokens) / sizeof(struct keytab))

/* These page props are allowed in microdocs. */
#define BMARGIN	    1
#define HEIGHT	    2
#define LMARGIN	    3
#define RMARGIN	    4
#define TMARGIN	    5
#define WIDTH	    6
#define FEATHER	    7
#define LD_GUTTER   8
#define LD_COLUMNS  9
#define LD_LADDER   10
#define VJUST	    11
#define LD_PAGE_HYPHEN	12
#define LD_REV_BAR_PLACE 13
#define FRAME_MAR_STR 14
#define VJUST_PAGES 15
#define MAR_SHR	    16
#define MAR_STR	    17
#define MAX_FEATHER 18
#define		MAX_MICRO	18	/* last allowable prop in micro */

/* These page props are NOT allowed in microdocs. */
#define	ORIENT	    	20
#define STARTPAGE   	21
#define LD_1ST_PAGE 	22
#define PN_TYPE	    	23
#define BLEED	    	24
#define BAL_COL	    	25
#define DEPTH_PB    	26
#define DEPTH_NPB   	27
#define PN_PREFIX   	28
#define IMARGIN	    	29
#define OMARGIN	    	30
#define LR_MARG	    	31
#define FREEZE_NUMS 	32
#define TURNED_PAGES	33
#define BEGWITH		34

struct keytab al_page_tokens[] = {  /* this list must remain sorted */
    {"Balance Columns", BAL_COL},
    {"Begin With", BEGWITH},
    {"Bleed", BLEED},
    {"Bottom Margin",BMARGIN},
    {"Columns", LD_COLUMNS},
    {"Consecutive Hyphens", LD_LADDER},
    {"Depth At Page Break", DEPTH_PB},
    {"Depth No Page Break", DEPTH_NPB},
    {"Feathering", FEATHER},
    {"First Page", LD_1ST_PAGE},	/* LEFT/RIGHT */
    {"Frame Margin Stretch", FRAME_MAR_STR},
    {"Frozen", FREEZE_NUMS},
    {"Frozen Number Streams", FREEZE_NUMS},
    {"Gutter", LD_GUTTER},
    {"Height",HEIGHT},
    {"Hyphenation", LD_PAGE_HYPHEN},
    {"Inner Margin",IMARGIN},
    {"Ladder", LD_LADDER},
    {"Left Margin",LMARGIN},
    {"Margins",LR_MARG},
    {"Margin Shrink", MAR_SHR},
    {"Margin Stretch", MAR_STR},
    {"Max. Feathering", MAX_FEATHER},
    {"Odd Page", LD_1ST_PAGE},	/* LEFT/RIGHT */
    {"Orientation",ORIENT},
    {"Outer Margin",OMARGIN},
    {"Page # Prefix", PN_PREFIX},
    {"Page # Style", PN_TYPE},
    {"Page Number Type", PN_TYPE},
    {"Revision Bar Placement", LD_REV_BAR_PLACE},
    {"Right Margin",RMARGIN},
    {"Starting Page", STARTPAGE},
    {"Starting Page #", STARTPAGE},  
    {"Top Margin",TMARGIN},
    {"Turned Pages", TURNED_PAGES},
    {"Vert. Just.",VJUST},
    {"Vert. Just. Pages", VJUST_PAGES},
    {"Vertical Justify",VJUST},
    {"Vertical Justify Pages", VJUST_PAGES},
    {"Width",WIDTH}
};
#define NUMPTS (sizeof(al_page_tokens) / sizeof(struct keytab))

#define PN_ARABIC   0
#define PN_LC_ROMAN 1
#define PN_UC_ROMAN 2
#define PN_LC_ALPHA 3
#define PN_UC_ALPHA 4
struct keytab al_pagenum_tokens[] = {  /* this list must remain sorted */
    {"Arabic", tok_ARABIC},
    {"Lowercase Alpha", tok_LOWALPHA},
    {"Lowercase Roman", tok_LOWROMAN},
    {"Roman",           tok_LOWROMAN},
    {"Uppercase Alpha", tok_UPALPHA},
    {"Uppercase Roman", tok_UPROMAN},
};
#define NUMPNTS (sizeof(al_pagenum_tokens) / sizeof(struct keytab))

#define AL_ENGLISH  1
#define AL_FRENCH   2
#define AL_GERMAN   3
#define AL_KBRITISH 4

struct keytab al_language_tokens[] = {  /* this list must remain sorted */
    {"English", AL_ENGLISH},
    {"French", AL_FRENCH},
    {"German", AL_GERMAN},
    {"Kodak British", AL_KBRITISH}
};
#define NUMLANGS (sizeof(al_language_tokens) / sizeof(struct keytab))

struct keytab al_weight_names[] = {  /* This one does not happen to need to be
				   * sorted! */
    {"ITALIC", face_ITALIC}, 
    {"BOLD", face_BOLD},
    {"ITALICS", face_ITALIC},
    {"ROMAN", face_PLAIN},
    {"NORMAL", face_PLAIN}
};
#define NWNAMES (sizeof(al_weight_names) / sizeof(struct keytab))

#define AN_SYMTYPE 1
#define AN_PREFIX 2
#define AN_SUFFIX 3
#define AN_START 4
#define AN_TRAIL 5
#define AN_SHOW 6
#define AN_INHERIT 7

struct keytab al_anum_tokens[] = { /* this list must remain sorted */
    {"Inherit", AN_INHERIT},
    {"Last Only", AN_TRAIL},
    {"Prefix", AN_PREFIX},
    {"Show", AN_SHOW},
    {"Starting Value", AN_START},
    {"Suffix", AN_SUFFIX},
    {"Symbol Type", AN_SYMTYPE},
    {"Trail", AN_TRAIL}
};

#define NUMATS (sizeof(al_anum_tokens) / sizeof(struct keytab))
	    
#define CENTERTEXT  		30
#define FIRSTPAGE   		31
#define HFFONT	    		32
#define LEFTTEXT    		33
#define RIGHTTEXT   		34
#define HFFRAME	    		45
#define ROTATION		64

struct keytab headfoot_tokens[] = {  /* this list must remain sorted */
    {"Centered Text",CENTERTEXT},
    {"Center Text",CENTERTEXT},	    /* note order */
    {"Diagram",HFFRAME},	    /* for consistency with <Frame, ...> */
    {"First Page",FIRSTPAGE},
    {"Font",HFFONT},
    {"Frame",HFFRAME},
    {"Left Text",LEFTTEXT},
    {"Right Text",RIGHTTEXT},
    {"Rotation", ROTATION}
};
#define NUMHFTS (sizeof(headfoot_tokens) / sizeof(struct keytab))

/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
#if 0	/** BT **/
 *
 * Possible improvements:
 *  deallocate font table
 *  when a parameter is undefined, only mention it the first occurrence
 *  use new message window
 *  a default "include" file for un_marked files
 *  if include fails, try alternate directory (as in styles)
 *  give warning if !Page comes after any references (classes..) 
 *  give warning if <!Fonts comes after <FI> etc.
 *  ArGetFont should check if font has been defined and substitute something

 *  If FONT = F0, use font from document definition (??)
 *  If a class is redefined, let the new one override.
 *  If a font number is redefined, let the new one override.
 *	likewise with <FI> etc.
 *  Fix tab setting in component defaults
 *  warn when class name is similar to command
 */
/*
 *      Philosophical notes:
 *
 *  A general rule is that we attempt to allow an ASCII
 *  document to be readable by as many previous version
 *  of the software as possible. If some markup is added
 *  that will occur only a few times per document. Say
 *  a field in <!Document is added) then old versions will
 *  already be OK: they will come out with one, or a few
 *  error messages. But suppose a really common guy is added
 *  (such as "Straddle = on" in component, or <Index,...>): then we
 *  want to avoid the situation where a zillion error messages
 *  come out. In these cases, we try to make the parser in the
 *  previous version know how to gracefully skip over the stuff.
 *
 *  Note: at one time, it was a goal that ASCII documents could be
 *  appended and loaded, but this was abandoned. This may explain
 *  a few oddities.
 *
 *  More philosophy (adopted when rewritten to load directly to
 *  memory): we try to keep things so that we have a largely
 *  consistant document at all times (except for formatting/pagination).
 *  Thus we set up default printers and fonts right away, and then
 *  change them as necessary. Likewise, components always have gapb
 *  and gape set correctly, etc.
 */

#endif	/** BT **/

/** I added this here.  BT **/
short ld_error = NO;

/**--------------------------------------------------------------------------**/
/* values for this table are defined in ar5private.h */

char ald_chtype[256] = {
/*       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F  */
/* 00*/ CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,WHT,WHT,CTL,WHT,WHT,CTL,CTL,
/* 10*/ CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,CTL,
/* 20*/ WHT,FST,SPC,FST,OTH,OTH,OTH,SPC,OTH,OTH,OTH,OTH,SPC,FST,OTH,OTH,
/* 30*/ DIG,DIG,DIG,DIG,DIG,DIG,DIG,DIG,DIG,DIG,OTH,OTH,SPC,SPC,SPC,OTH,
/* 40*/ SPC,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,
/* 50*/ LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,OTH,RES,OTH,RES,OTH,
/* 60*/ RES,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,
/* 70*/ LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,LTR,RES,QUO,RES,RES,QUO,
/* 80*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* 90*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* a0*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* b0*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* c0*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* d0*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* e0*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,
/* f0*/ QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO,QUO
};

/**--------------------------------------------------------------------------**/
/*  these globals generally relate to the component under construction: */
Cmpn_t* al_cmpn;		    /* the current component */
int al_cmpn_type;	    /* type of the current component (P_TEXT etc.) */
Cmpn_t* al_first_cmpn;	    /* the first component */
Cmpn_t* al_subcmpn;	    /* the current component or subcomponent. If in the
			       midst of loading a subcmpn, al_subcmpn points 
			       to the most deeply nested subcmpn. If in the 
			       midst of a cmpn load without a subcmpn, then 
			       al_subcmpn should be equal to al_cmpn.*/
Cmpn_t* al_last_cmpn;	    /* the last cmpn linked into the doc */
long al_cmpn_size;	    /* # of tokens (shorts) in ultimate cmpn */
long al_gluepos;	    /* position of last glue or HR in component */
bool al_filled;		    /* true if "filled" (false means put HR each line)*/
bool al_bfilled;	    /* fill = blank */
bool al_selected;	    /* true if this cmpn selected */
int al_newline_count;	    /* counts contiguous newlines, reset by
			     * ArDoCmpn */

Table_t* al_table;	    /* the current table (possibly a master table) */
rsu al_table_width;	    /* width of table row so far */
#if 0	/** BT **/
TCOL* al_table_col;	    /* pointer to current table col entry */
#endif	/** BT **/

short al_dfont;		    /* global default font (formerly Doc.dfont) */
			    /* must be short */
bool al_whole_document;
bool al_masters_only;	    /* set if reading styles sheet */
bool al_micro_in_master;    /* set if micro-doc in a <!Class */

bool al_cancel;		    /* set if load canceled */
bool al_micro;		    /* true during micro doc */
Page_info_t* al_micro_pgAttrs;  /* page attrs of micro doc */

bool al_end_text;	    /* set when <End Text> seen */
int al_deferred_command = 0;	/* If we hit a command in a microdoc (cell)
				 * that terminates it, we save it here */

bool al_warn_line_no;	    /* true if warning msg should include line#*/
int al_num_cmpn;
bool al_declarations_finished;	/* TRUE when cmpn, class or head/foot seen */
int al_include_decl_level;  /* counts depth of <!Include Declaration */
int al_line_number ;	    /* within the document */
int al_eol_flag ;	    /* used to get all CR/LF combinations */
int al_err_count;	    /* # errors seen in document */
int al_disc_hyphen;	    /* # <-> seen in document */

int al_legal_char = 0x20;   /* minimum legal char */
int al_cmd_char = '<';	    /* this may be set to UNDEF! */
int al_cmd_term = '>';
int al_decl_char = '!';
int al_hex_char = '#';
int al_hyph_char = '-';	    /* <-> for discretionary hyphen */
int al_cmd_end  = '/';
int al_com_char = '-';	    /* -- is comment -- */

int al_opsa_version;	    /* (times 100) from <!OPS, Version = > */
bool al_old_widorph;	    /* old meanings for widow/orphan */
bool al_version1;	    /* set if version 1 eol conventions */
bool al_old_frames;	    /* set if old style frames baseline */
bool al_no_att_prev;	    /* set if "attach previous" not allowed */
bool al_swap_bot_lst;	    /* set if "bottom" and "following text"
			     * are reversed */
bool al_diag_ctrl;	    /* diagramming can have control characters */
bool al_qs_hex;		    /* set if quoted string can contain <#xx>
			     * etc. */
bool al_rab_quote;	    /* set if '>' is quoted in diagram text strings */
bool al_diag_in_frame;	    /* set if frames have Diagram = */
bool al_14_tabs;	    /* set if 14 tabs only */
bool al_markup;		    /* set if markup expected */
bool al_master_frm_seen;    /* set if master frame markup seen */
bool al_master_tbl_seen;    /* set if master table markup seen */
bool al_hyphenate;	    /* set if hyphenation desired */
bool al_old_orient;	    /* set if paper orient in page props */

int al_unused_classes;	    /* Unused classes:	(currently unused)
				    * 1 means throw out
				    * 0 means create master */
int al_bs_discarded;	    /* # of backspace characters discarded */
int al_us_discarded;	    /* # of underscore characters discarded */

/**--------------------------------------------------------------------------**/
void 
ArGlobalsReset()
{
    al_micro = FALSE;
    al_deferred_command = 0;
    al_cancel = FALSE;
    al_unused_classes = 0;	/* not currently used */
    al_err_count = 0;
    al_disc_hyphen = 0;		/* count usage of <-> */
    al_bs_discarded = 0;
    al_us_discarded = 0;
    al_include_decl_level = 0;
    al_warn_line_no = TRUE;
    al_legal_char = 0x20;	/* minimum legal char */
    al_cmd_char = '<';
    al_declarations_finished = FALSE; /* TRUE when cmpn or head/foot seen */
    al_last_cmpn = NULL;
    al_cmpn = NULL;
    al_first_cmpn = NULL;
    al_table = NULL;
    al_subcmpn = NULL;
    al_selected = FALSE;
    al_num_cmpn = 0;
    al_diag_in_frame = FALSE;
    al_micro_in_master = FALSE;
    al_master_frm_seen = FALSE; /* set if master frame markup seen */
    al_master_tbl_seen = FALSE; /* set if master table markup seen */
    al_opsa_version = 610;	/* default for VersionControl */
				/* When changing version number, also change
				 * code in ArDoOPSDecl() and the error message:
		 "Warning: This software version expects ASCII Format Ver. 5.X"
			         * in alMain.m */
}

/**--------------------------------------------------------------------------**/
/**	This could be faster if we had the sizeof() each identified type     **/
/** 	available, perhaps in the Identifier_t itself.			     **/
/**	But it ain't broke, so don't fix it now.			     **/

#ifdef UNUSED

static NullObj_t *
ArObjectCopy( destP, srcP, free_src )
  register NullObj_t *destP, *srcP;
  bool free_src;
{
  static char err_msg[] =
   "ArObjectCopy(%#x, %#x): unknown identifier type %#x.  Get help. Exiting.\n";

    if (destP && srcP) {
	switch(*(Identifier_t *)srcP) {
	case ID_NullObj:
	    *(NullObj_t *)destP = *(NullObj_t *)srcP;
	    break;
	case ID_OpsDecl:
	    *(Ops_info_t *)destP = *(Ops_info_t *)srcP;
	    break;
	case ID_Document:
	    *(Document_info_t *)destP = *(Document_info_t *)srcP;
	    break;
	case ID_Font:
	    *(Font_info_t *)destP = *(Font_info_t *)srcP;
	    break;
	case ID_Color:
	    *(Color_info_t *)destP = *(Color_info_t *)srcP;
	    break;
	case ID_Pattern:
	    *(Pattern_info_t *)destP = *(Pattern_info_t *)srcP;
	    break;
	case ID_Page:
	    *(Page_info_t *)destP = *(Page_info_t *)srcP;
	    break;
	case ID_MasterFrame:
	case ID_Frame:
	    *(Frame_t *)destP = *(Frame_t *)srcP;
	    break;
	case ID_ClassDef:
	case ID_MasterCmpn:
	case ID_Cmpn:
	    *(Cmpn_t *)destP = *(Cmpn_t *)srcP;
	    break;
	case ID_Table:
	    *(Table_t *)destP = *(Table_t *)srcP;
	    break;
	case ID_HeadFoot:
	    *(HeaderFooter_t *)destP = *(HeaderFooter_t *)srcP;
	    break;
	case ID_AutoStream:
	    *(An_stream_info_t *)destP = *(An_stream_info_t *)srcP;
	    break;
	case ID_FileInfo:
	    *(File_info_t *)destP = *(File_info_t *)srcP;
	    break;
	case ID_State:
	    *(Il_state_t *)destP = *(Il_state_t *)srcP;
	    break;
	case ID_Comment:
	    *(Comment_t *)destP = *(Comment_t *)srcP;
	    break;
	default:
	    ArFprintf(stderr, err_msg, destP, srcP, *(Identifier_t *)srcP);
	    break;

	}
	if (free_src != NO) {
	    free(srcP);
	}
    }
    return( destP );
}

#endif /* UNUSED */

/**--------------------------------------------------------------------------**/
/**	All data structures that ArNextObject() can return MUST have an	     **/
/**	Identifier_t as the first element.  It is used to discriminate among **/
/**	the various types of objects that are returned.  The actual return   **/
/**	value is NULL or a pointer to an object so identified;  it is 	     **/
/**	allocated from dynamic memory.					     **/

NullObj_t *
ArNextObject(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{
    register int c, c2;
    NullObj_t *my_resultP = NULL;
    static int first_time = YES;
    static char *err_msg =
      "ArNextObject: ld_error == %d on entry. Get help. Exiting.";
    static char *err_msg2 =
      "ArNextObject() fatal error: not an I.A.M. file! Exiting.\n";
    
    AR_il_stateP = ilP;		/** this global is needed to fill in default **/
				/** values like cmpn props in microdocs.  BT **/

    if ((my_resultP = arObjectPreFetch()) != NULL) {
	/** handle special case! **/
	/** drop into code below the 'else' **/
    }
    else {
	al_end_text = FALSE;
	al_newline_count = 0;
	if (ld_error) {
	    ArFprintf(stderr, err_msg);
	    exit(-1);
	}
	if (first_time == YES) {
	    first_time = NO;
	    if (ArGetch(fiP) == AR_CMD_CHAR && ArGetch(fiP) == AR_DECL_CHAR) {
		/** it's cool.  put the chars back and proceed. **/
		ArUnGetch(AR_DECL_CHAR, fiP);
		ArUnGetch(AR_CMD_CHAR,  fiP);
	    }
	    else {
		/** it's not an IAM file.  terminate with extreme prejudice. **/
		ArFprintf(stderr, err_msg2);
		exit( -1 );
	    }
	}
	if ((c = ArGetchNW(fiP)) >= 0 ) {
	    if (c == al_cmd_char) {
		if((c2 = ArGetch(fiP)) == al_cmd_char) {
		/** it's a '<<' char starting a component. **/
		    my_resultP = 
			(NullObj_t *)ArBuildCmpnShar(fiP,c2,AR_prev_cmpnP,ilP);
		    /** since it is top-level, remember it for later use. **/
		    AR_prev_cmpnP = (Cmpn_t *)my_resultP;
		} 
		else {
		    /** it's a declaration, or table, or component. **/
		    my_resultP = ArBuildObject(fiP, c2, ilP);
		    if (Identify(my_resultP) == ID_Cmpn) {
			/** since it is top-level, remember it for later use.**/
			AR_prev_cmpnP = (Cmpn_t *)my_resultP;
		    }
		}
	    } 
	    else {
		/** it's a plain-text char starting a component. **/
		my_resultP = 
		    (NullObj_t *)ArBuildCmpnShar(fiP, c, AR_prev_cmpnP, ilP);
		/** since it is top-level, rmember it for later use. **/
		AR_prev_cmpnP = (Cmpn_t *)my_resultP;
	    }
	}
    }
    return( (NullObj_t *)my_resultP );
}
/**--------------------------------------------------------------------------**/
/** Build a declaration, table or component. **/

NullObj_t *ArBuildObject(fiP, c, ilP)
  File_info_t *fiP;
  shar c;
  Il_state_t * ilP;
{
  int temp;
  int decl = FALSE;
  int get_ret;
  Cmpn_t *my_cmP;
  static char *err_msg = 
    "ArBuildObject: saw <-> cmd; bad input format. Get help. Returning NULL.\n";

  pop_include:
    temp = c;
    if(temp <= ALD_CHTYP_MAX && ald_chtype[temp] == FST) {
	switch(temp) {
	case AR_DECL_CHAR:
	    decl = TRUE;
	    temp = NOCHAR;
	    break;
	case AR_HEX_CHAR:
	    if(al_include_decl_level > 0) {
		ArPopRead(fiP);    /* end of declarations   WRONG!   BT*/
		c = ArGetchNW(fiP);
		goto pop_include;  /** build an object from the prev. file **/
	    }
	    else {
	       /** it's a component with a hexified char as its first **/
	       /** character. Build a component and return it.        **/
	       my_cmP=
		   (Cmpn_t*)ArBuildCmpnShar(fiP,ArDoHex(fiP),AR_prev_cmpnP,ilP);
	       return( (NullObj_t *)my_cmP );
	    }
	    break;
	case AR_HYPH_CHAR:
	    ArFprintf(stderr, err_msg);
	    return( NULL );
	    break;
	default:
	    ArFprintf(stderr, "ArBuildObject internal error: get help.\n");
	    exit( -1 );
	    break;
	}
    }
    if((get_ret = ArGetWordShrC(fiP,temp,al_token_shr,AR_MAX_WORD-1,COMMA))==0){
	if(al_include_decl_level > 0) {
	    ArPopRead(fiP);    /* end of declarations */
	    c = ArGetchNW(fiP);
	    goto pop_include;  /** build an object from the prev. file **/
	}
	else {
	    /** failure to read a word in the current file.  **/
	    ArFprintf(stderr, 
		"ArBuildObject: ArGetWordShrC() failed. Returning NULL.\n");
	    return( NULL );
	}
    }
    if(!decl) {
	/** it must be a header/footer, component or table. **/
	if(al_include_decl_level > 0) {
	    ArPopRead(fiP);    /* end of declarations */
	    c = ArGetchNW(fiP);
	    goto pop_include;  /** build an object from the prev. file **/
	}
	temp = ArIsCommandName(fiP, al_token_shr, LOOSE);
	if(get_ret == AR_QUOTED || temp < 0) { 
	    /* quoted token means it is a component */
	    /* temp < 0 means not a command name, therefore a component. */
	    my_cmP = 
	       (Cmpn_t*)ArBuildCmpnName(fiP,al_token_shr,YES,AR_prev_cmpnP,ilP);
	    /** if it is top-level, remember it for later use. **/
	    return( (NullObj_t *)my_cmP );
	}
	else {
	    return( ArBuildFromCmd(fiP, temp, al_token_shr, ilP) );
	}
    } 
    else {
	switch(ArTokenShr(fiP, al_token_shr, 
			  declarative_names, 
			  NUM_OF(declarative_names), 
			  LOOSE)) {
	case COMMENT   :    return( ArBuildComment(fiP) ); 
			    break;
	case CLASS     :    return( ArBuildClass(fiP, ilP) ); 
			    break;
	case CLASS_DEF :    return( ArBuildClassDef(fiP, ilP) ); 
			    break;
	case DOCUMENT  :    return( ArBuildDocumentInfo(fiP) ); 
			    break;
	case EQUATE    : 
			    break;
	case FONTS     :    return( ArBuildFontDefs(fiP) ); 
			    break;
	case INCLUDE   : /** AlInclude(); **/
			    ArFprintf(stderr, 
				"Can't handle <!Include>. Exiting.\n");
			    exit(-1);
			    break;
	case INCLUDE_DECL: /** AlIncludeDecl();  **/
			    ArFprintf(stderr, 
				"Can't handle <!Include Decl>. Exiting.\n");
			    exit(-1);
			    break;
	case OPSDECL   :    return( ArBuildOPSDecl(fiP, ilP) ); 
			    break;
	case PAGE_DEF  :    return( ArBuildPageInfo(fiP) ); 
			    break;
	case STATE     :    return( NULL );
			    break;
	case LD_MASTER_FRM:    return( (NullObj_t *)ArBuildFrame(fiP, YES, NO) ); 
			    break;
	case LD_MASTER_TBL:    return( ArBuildTable(fiP, ilP, YES) ); 
			    break;
	case AUTOSTREAM:    return( ArBuildAnStreamInfo(fiP) ); 
			    break;
	case CLR_PALETTE:    return( ArBuildColorInfo(fiP) ); 
			    break;
	case PAT_PALETTE:    return( ArBuildPatternInfo(fiP) ); 
			    break;
	default :	
		/** AlTellError2(ERR_NO_SUCH_DECL, al_token_shr, 0); BT **/
		    ArFprintf(stderr, 
		      "Unknown Declaration: %Q. Skipping it.\n", al_token_shr);
		    ArSkipPastTerm(fiP);
		    return( NULL );
	}
    }
    return( NULL );
}

/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildComment(fiP)
  File_info_t *fiP;
{
  Comment_t *coP = NEW(Comment_t);
  register shar c;

    SET_NullObjFields(coP, ID_Comment);
    coP->text = SsInitEmpty();
    while ((c = ArGetch(fiP)) != Rmarkup) {
	SsSaveS(c, &coP->text);
    }
    SsSaveS('\0', &coP->text);
    /** ArFprintf(stderr, "ArBuildComment() ==> %Q\n", coP->text.baseP); BT **/
    return( (NullObj_t *)coP );
}


/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildOPSDecl(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{
    int c;
    float temp_float;
    Ops_info_t *opsP = NEW(Ops_info_t);
    
    *opsP = default_il_state.ops_info;

    while((c = ArGetchNW(fiP)) != al_cmd_term && c != EOF) {
	ArUnGetch(c, fiP);
	switch(ArGetTokenEq(fiP, opsdecl_tokens, NUMODS)) {
	case NO_EQUALS:
	    ArFprintf(stderr, "Syntax error in <!Ops>.  Check it.\n");
	    ArSkipPastTerm(fiP);
	    free(opsP);
	    return(NULL);
	    break;
	case OPSVERSION:
	    ArGetFloat(fiP, &temp_float);
	    opsP->version = al_opsa_version = temp_float * 100. + 0.5;
	    if(al_opsa_version >= 700) {
		/** Mn1Printf(Menmid, alMainMsgBadVersionNum); BT **/
		/** delay(10); BT **/
	    }
	    ArVersionControl(TRUE);
	    ArSkipPastComma(fiP);
	    break;
	case OPSSTYLE:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	    if(ArStrncmpShr(al_token_shr, "Metric", 6, LOOSE) == 0) {
		opsP->style = OPS_STYLE_METRIC;
		ArDefaultsSetMetric(ilP);
	    }
	    else {
		ArTellError2(ERR_NO_SUCH_CVAL, al_token_shr, 0);
	    }
	    ArSkipPastComma(fiP);
	    break;
	default:
	    opsP->unkP = 
		(Unknown_t *)ArBuildUnknownPropEq(fiP,al_token_shr,opsP->unkP);
	    break;
	}
    }
    /** ArFprintf(stderr, "Adding OpsDecl.\n"); BT **/
    return( (NullObj_t *)opsP );
}

ArVersionControl(saw_version)
bool saw_version;   /* tells if we saw Version= */
{
    bool save_14_tabs;
    register int ver = al_opsa_version;
    
#if 1	/** BT **/
    al_version1 = ver < 200;
    al_old_frames = ver < 220;
    al_no_att_prev = ver >= 220;
    al_old_widorph = ver < 230;
    al_swap_bot_lst = ver < 300;
    al_diag_ctrl = ver < 300;
    al_qs_hex = ver >= 300;
    al_rab_quote = ver >= 400;
    save_14_tabs = al_14_tabs;
    al_14_tabs = ver < 510;
    if(al_14_tabs != save_14_tabs) {
#if 0	/** BT **/
	extern Tabline_t ArDefaultTabline(); 
	default_cmpn.tabs        = ArDefaultTabline(); /* depends on version! */
	default_metric_cmpn.tabs = ArDefaultTabline(); /* depends on version! */
#endif	/** BT **/
	ArFprintf(stderr, "Can't set 14 tabs! exiting.  Get help.\n");
	exit( -1 );
    }
    /* we dont set this if we are assuming default version */
    if(saw_version) al_diag_in_frame = ver >= 510;
    al_old_orient = ver < 610;
#endif	/** BT **/
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildDocumentInfo(fiP)
  register File_info_t *fiP;
{
  shar c;
  int token;
  short tempshort;
  int   tempint;
  Document_info_t *diP = NEW(Document_info_t);
    
    *diP = default_il_state.doc_info;
    while((c = ArGetchNW(fiP)) != al_cmd_term && c != EOF) {
	ArUnGetch(c, fiP);
	token = ArGetTokenEq(fiP, al_doc_tokens, NUMDOCS);
	switch(token) {
	case NO_EQUALS:
	    ArFprintf(stderr, "Syntax error in <!Document>.  Check it.\n");
	    ArSkipPastTerm(fiP);
	    free(diP);
	    return(NULL);
	    break;
	case AL_LANGUAGE:
	case DOCFONT:
	case HYPHEN:
	case LADDER:
	case PAPER_ORIENT_OLD:
	    ArFprintf(stderr, "Obsolete property in <!Document>.  check it.\n");
	    ArSkipPastComma(fiP);
	    break;
	case PRINTER_TYPE:
	    if (ArGetWordShrC(fiP, NOCHAR, diP->final_output_dev, 
				NUM_OF(diP->final_output_dev), COMMA) == 0) {
		ArSkipPastComma(fiP);
	    }
	    break;
	case TARGET_PRINTER:
	    if (ArGetWordShrC(fiP, NOCHAR, diP->def_printer, 
				NUM_OF(diP->def_printer), COMMA) == 0) {
		ArSkipPastComma(fiP);
	    }
	    break;
	case HEADER_PAGE:
	    tempshort = FALSE;
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.header_page = YES;
	    else          diP->flags.header_page = NO;
	    break;
	case UNUSED_CLASSES:	/* currently unused */
	    tempshort = 0;
	    if(ArGetYesNoShort(fiP, &tempshort)) {
		if(tempshort) al_unused_classes = 0;
		else al_unused_classes = 1;
	    }
	    break;
	case AL_UNIT:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	    tempshort = arNameSearch(al_token_shr,Measure_names,AR_MAX_MEASURE);
	    if (tempshort >= 0) { diP->measure_unit = tempshort; }
	    else                { ArSkipPastComma(fiP); }
	    break;
	case AL_DOUBLE_SIDED:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.double_sided = YES;
	    else          diP->flags.double_sided = NO;
	    break;
	case AL_MANUAL_FEED:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.manual_feed = YES;
	    else          diP->flags.manual_feed = NO;
	    break;
	case AL_PRINT_REV:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.print_rev = YES;
	    else          diP->flags.print_rev = NO;
	    break;
	case AL_PRINT_STR:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.print_strikes = YES;
	    else          diP->flags.print_strikes = NO;
	    break;
	case AL_PRINT_UND:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.print_underlines = YES;
	    else          diP->flags.print_underlines = NO;
	    break;
	case AL_UNDER_POS:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.underline_at_descender = YES;
	    else          diP->flags.underline_at_descender = NO;
	    break;
	case AL_COLOR_PROC:
	    ArGetColorSep(fiP, &diP->spot_color_sep);
	    break;
	case AL_APAGE_STYLE:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	    tempshort = arNameSearch(al_token_shr,
				     NumStyle_names,
				     AR_MAX_NUMSTYLE);
	    if (tempshort >= 0) { diP->A_Pg_style = tempshort; }
	    else                { ArSkipPastComma(fiP); }
	    break;
	case AL_APAGE_PREFIX:
	    if (ArGetWordShrC(fiP, NOCHAR, diP->A_Pg_prefix, 
			  NUM_OF(diP->A_Pg_prefix), COMMA) == 0) {
		ArSkipPastComma(fiP);
	    }
	    break;
	case AL_APAGE_NUM:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint > 0) {
		diP->max_page = tempint;
	    }
	    break;
	case PAPER_ORIENT:
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) diP->flags.orient_inverted = YES;
	    else          diP->flags.orient_inverted = NO;
	    break;
	default:
	    diP->unkP = 
		(Unknown_t *)ArBuildUnknownPropEq(fiP, al_token_shr, diP->unkP);
	    break;
	}
    }
    /** ArFprintf(stderr, "Adding DocInfo.\n"); BT **/
    return( (NullObj_t *)diP );
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildFontDefs(fiP)
  register File_info_t *fiP;
{
  register Font_info_t *fdP, *startP, *tmpP;
  int tempint;
  short font_size, font_face, font_token;
  shar buf[AR_MAX_FONTNAME];
  extern void arParseFontName();
  shar c;
#ifdef UNUSED
  static Font_info_t my_font_init= {
     INIT_NullObjFields(ID_Font),
     0, 
     {'T','h','a','m','e','s',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
     10, 
     face_PLAIN
  };
#endif

    fdP = startP = tmpP = NULL;

    while ((c = ArGetchNW(fiP)) != EOF && c != AR_CMD_TERM) {
	font_token = font_size = font_face = 0;
	if (c != 'F') {
	    ArSkipPastComma(fiP);
	    continue;
	}
	if (ArGetInt(fiP, &tempint, NO, NO) == NO) {
	    ArSkipPastComma(fiP);
	    continue;
	}
	/** got font token. **/
	font_token = tempint;

	if ((c = ArGetchNW(fiP)) != '=') {
	    ArSkipPastComma(fiP);
	    continue;
	}
	arParseFontName(fiP, buf, &font_size, &font_face);

	/** now we've got this entry. **/
	fdP = NEW(Font_info_t);
	SET_NullObjFields(fdP, ID_Font);
	fdP->font_token = font_token;
	ArShrcpy(fdP->font_name, buf);
	fdP->font_size = (font_size==0)? 10 : font_size;
	fdP->font_face = (font_face==face_UNKNOWN)? face_PLAIN : font_face;
	/** now link it onto the list. **/
	if (startP == NULL) {
	    startP = fdP;
	}
	else {
	    /** find the end. **/
	    for (tmpP = startP; 
		tmpP->nextP != NULL; 
		tmpP = (Font_info_t *)tmpP->nextP) { 
		    /** find the end of the list. **/
	    }
	    tmpP->nextP = (NullObj_t *)fdP;/** join latest one onto the list.**/
	}
    }
    /** ArFprintf(stderr, "Adding FontDefs.\n"); BT **/
    return( (NullObj_t *)startP );
}
/**--------------------------------------------------------------------------**/


/** this is adapted from alFont.c   BT **/
void
arParseFontName(fiP, nameP, sizeP, faceP)
  File_info_t *fiP;
  shar *nameP;
  short *sizeP;
  short *faceP;
{
    register int i;
    register int c;
    register int n;
    register short* s;
    int lweight;
    char* wgtptr;
    short font_name[AR_MAX_FONTNAME + 1];

    /* we dont use AlGetWord because we need to keep blanks */
    for(i = AR_MAX_FONTNAME, s = font_name; --i > 0;) {
	if((c = ArGetch(fiP)) == EOF) break;
	if(c <= ALD_CHTYP_MAX) {
	    if((n = ald_chtype[c]) == SPC) {
		if(c != ',') ArUnGetch(c, fiP);
		break;
	    }
	    if(n == WHT) {
		if(s == font_name) continue;   /* omit leading blanks etc*/
		c = ' ';
	    }
	}
	*s++ = c;
    }
    *s = 0;

    /* first we extract the weight (italic bold etc.) */
    for(i = 0; i < NWNAMES; i++) {
	wgtptr = al_weight_names[i].keyword;
	lweight = strlen(wgtptr);
	for(s = font_name; (c = *s) != 0; s++) {
	    /* next line necessary so that ArShrcpy below chops out right thing*/
	    if(c == ' ') continue;
	    if(ArStrncmpShr(s, wgtptr, lweight, LOOSE) == 0) {
		/* this will lose if someone put blanks inside the name... */
		ArShrcpy(s, s + lweight); /* chop out the weight from font name */
		if (*faceP == 0) {
		    *faceP = al_weight_names[i].keyval;
		}
		else {
		    /** any combination must be this. **/
		    *faceP = face_BOLD_ITALIC;
		    break;
		}
		/* keep looping to find "bold italic" etc. */
		/* goto found_weight; */
	    }
	}
    }
    /* Next we extract the size: look for a number in the string, unless
     * part of a family name (e.g. math-1234) */
    /* found_weight: */
    for(s = font_name; (c = *s) != 0; s++) {
	if(Isdigit(c)) {
	    if(s > font_name && *(s-1) != ' ') continue; /* touches name */
	    ArCvtIntShr(s, sizeP, FALSE);
	    break;
	}
    }
#if 0	/** BT **/
    register short* ss;

    for(ss = s; Isdigit(*ss); ss++) ;
    ArShrcpy(s, ss);  /* remove the digits from the string */

    /* now whats left ought to be a family name */
  try_again:
    for(i=0, n = FnFamNum(); i < n; ++i) {
	if(ArStrncmpShr(font_name, FnFamPupName(i), FONTNAME_MAX, LOOSE) == 0){
	    if (EC_PRINT_FGROUP->famdex[i] != NULL) {
	      fdptr->family = i;
	      return(TRUE);
	    }
	}
    }
#else
    /** drop size digits from the family name. **/
    *s = '\0';	
    /** drop trailing spaces, if any, from the family name. **/
    for (--s; *s == ' ' && s > font_name; --s) {
	*s = '\0';	
    }
#endif	/** BT **/

    /* Interleaf font used to be called "letter" */
    if(ArStrncmpShr(font_name, "letter", AR_MAX_FONTNAME, LOOSE) == 0) {
	ArStrtoshr(font_name, "Interleaf");
	/** goto try_again; BT **/
    }

    /* check for old math font names */
    if(ArStrncmpShr(font_name, "Math. ext.", AR_MAX_FONTNAME, LOOSE) == 0) {
	ArStrtoshr(font_name, "Math Ext.");
	/** goto try_again; BT **/
    }
    if(ArStrncmpShr(font_name, "Math. A", AR_MAX_FONTNAME, LOOSE) == 0) {
	ArStrtoshr(font_name, "Math A");
	/** goto try_again; BT **/
    }
    if(ArStrncmpShr(font_name, "Math. B", AR_MAX_FONTNAME, LOOSE) == 0) {
	ArStrtoshr(font_name, "Math B");
	/** goto try_again; BT **/
    }
    ArShrcpy(nameP, font_name);	/** BT **/
}     

/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildColorInfo(fiP)
  register File_info_t *fiP;
{
  register Color_info_t *clrP, *startP, *tmpP;
  int tempint;
  shar type;
  short index;
  float r,g,b;
  shar c;
  static char *err_msg = 
    "Error reading color definitions; check the input document.\n";

    clrP = startP = tmpP = NULL;

    while ((c = ArGetchNW(fiP)) != EOF && c != AR_CMD_TERM) {
	type = index = 0;
	r = g = b = 0.0;
	if ((c | CHARCASEBIT) != 'c') {
	    ArSkipPastTerm(fiP);
	    break;
	}
	/** got color type. **/
	type = c;

	if (ArGetInt(fiP, &tempint, NO, NO) == NO) {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	}
	/** got color index. **/
	index = tempint;

	if ((c = ArGetchNW(fiP)) != '=') {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	}
	if (ArGetFloat(fiP, &r)  == YES   &&
	    ArGetchNW(fiP)       == COMMA &&
	    ArGetFloat(fiP, &g)  == YES   &&
	    ArGetchNW(fiP)       == COMMA &&
	    ArGetFloat(fiP, &b)  == YES) {
		/** got an r-g-b triplet sucessfully. **/
	}
	if ((c = ArGetchNW(fiP)) == COMMA) {
	    /** it's cool. **/
	}
	else if (c == AR_CMD_TERM) {
		ArUnGetch(c, fiP);	/** to terminate loop, above. **/
	}
	else {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	}

	/** now we've got this entry. **/
	clrP = NEW(Color_info_t);
	SET_NullObjFields(clrP, ID_Color);
	clrP->color.type   = type;
	clrP->color.index  = index;
	clrP->color.desc.r = r;
	clrP->color.desc.g = g;
	clrP->color.desc.b = b;

	/** now link it onto the list. **/
	if (startP == NULL) {
	    startP = clrP;
	}
	else {
	    /** find the end. **/
	    for (tmpP = startP; 
		tmpP->nextP != NULL; 
		tmpP = (Color_info_t *)tmpP->nextP) { 
		    /** find the end of the list. **/
	    }
	    tmpP->nextP = (NullObj_t *)clrP;/**join latest one onto the list.**/
	}
    }
    /** ArFprintf(stderr, "Adding ColorInfo.\n"); BT **/
    return( (NullObj_t *)startP );
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildPatternInfo(fiP)
  register File_info_t *fiP;
{
  register Pattern_info_t *patP, *startP, *tmpP;
  shar type;
  short index;
  short patbuf[AR_PATTERN_SIZE];
  shar c;
  static char *err_msg = 
      "Error reading pattern definitions; check the input document.\n";
  long templong;
  int  tempint;

    patP = startP = tmpP = NULL;

    while ((c = ArGetchNW(fiP)) != EOF && c != AR_CMD_TERM) {
	type = index = 0;
	if ((c | CHARCASEBIT) != 'p') {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	}
	/** got Pattern type. **/
	type = c;

	if (ArGetInt(fiP, &tempint, NO, NO) == NO) {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	}
	/** got Pattern index. **/
	index = tempint;

	if ((c = ArGetchNW(fiP)) != '=') {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	}
	c = ArGetchNW(fiP);
	ArUnGetch(c, fiP);
	for ( tempint = 0; tempint < AR_PATTERN_SIZE; tempint++) {
	   if (ArGetHex(fiP, &templong, sizeof(short)*2) != NO) {
	       patbuf[tempint] = templong;
	   }
	   else {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	   }
	}
	if ((c = ArGetchNW(fiP)) == COMMA) {
	    /** it's cool. **/
	}
	else if (c == AR_CMD_TERM) {
		ArUnGetch(c, fiP);	/** to terminate loop, above. **/
	}
	else {
	    ArFprintf(stderr, err_msg);
	    ArSkipPastTerm(fiP);
	    break;
	    }

	/** now we've got this entry. **/
	patP = NEW(Pattern_info_t);
	SET_NullObjFields(patP, ID_Pattern);
	patP->pattern.type   = type;
	patP->pattern.index  = index;
	for (tempint = 0; tempint < AR_PATTERN_SIZE; tempint++) {
	    patP->pattern.pattern[tempint] = patbuf[tempint];
	}

	/** now link it onto the list. **/
	if (startP == NULL) {
	    startP = patP;
	}
	else {
	    /** find the end. **/
	    for (tmpP = startP; 
		tmpP->nextP != NULL; 
		tmpP = (Pattern_info_t *)tmpP->nextP) { 
		    /** find the end of the list. **/
	    }
	    tmpP->nextP = (NullObj_t *)patP;/**join latest one onto the list.**/
	}
    }
    /** ArFprintf(stderr, "Adding PatternInfo.\n"); BT **/
    return( (NullObj_t *)startP );
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildPageInfo(fiP)
  register File_info_t *fiP;
{
  shar c;
  int token;
  int tempint;
  short tempshort;
  Page_info_t *pgP = NEW(Page_info_t);
    
    ArPageInfoInit(pgP);

    while((c = ArGetchNW(fiP)) != al_cmd_term && c != EOF) {
	ArUnGetch(c, fiP);
	token = ArGetTokenEq(fiP, al_page_tokens, NUMPTS);
	if(token > MAX_MICRO) {
	    pgP->micro_props_only = NO;	/** important!  BT **/
	}
        switch(token) {
	case NO_EQUALS:
	    ArFprintf(stderr, "Syntax error in <!Page>.  Check it.\n");
	    ArSkipPastTerm(fiP);
	    free(pgP);
	    return(NULL);
	    break;
	case BMARGIN:
	    ArGetPZRsu(fiP, &pgP->bottom_margin);
	    break;
	case HEIGHT:
	    ArGetPRsu(fiP, &pgP->height);
	    break;
	case LMARGIN:
	    ArGetPZRsu(fiP, &pgP->left_margin);
	    break;
	case RMARGIN :
	    ArGetPZRsu(fiP, &pgP->right_margin);
	    break;
	case IMARGIN:
	    ArGetPZRsu(fiP, &pgP->inner_margin);
	    break;
	case OMARGIN :
	    ArGetPZRsu(fiP, &pgP->outer_margin);
	    break;
        case TMARGIN :
	    ArGetPZRsu(fiP, &pgP->top_margin);
	    break;
	case WIDTH:
	    ArGetPZRsu(fiP, &pgP->width);
	    /** have to check below if 0 is allowed.   BT **/
	    break;
	case ORIENT:
	    ArFprintf(stderr, "%s %s\n",
		"Found obsolete 'Orientation' property.",
		"Resave the document and try again.\n");
	    exit( -1 );
	    break;
	case LD_GUTTER:
	    ArGetPZRsu(fiP, &pgP->gutter);
	    break;
	case LD_COLUMNS:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint > 0)
		pgP->columns = tempint;
	    break;
	case LD_LADDER:
	    pgP->ladder_count = AR_HYPHEN_ANY;
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0) {
		if(tempint > AR_MAX_HYPHEN) tempint = AR_MAX_HYPHEN;
		pgP->ladder_count = tempint;
	    } 
	    else {
		ArSkipPastComma(fiP);
	    }
	    break;
	case STARTPAGE:
	    if(ArGetInt(fiP, &tempint, YES, NO)) {
		pgP->start_page_num = tempint;
		pgP->flags.pgnum_absolute = YES;
	    } 
	    else {
		ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
		if(ArStrncmpShr(al_token_shr, "Inherit", 7, LOOSE) == 0) {
		    pgP->flags.pgnum_absolute = NO;
		}
		else {
		    ArTellError2(ERR_NO_SUCH_CVAL, al_token_shr, 0);
		}
	    }
	    break;
	case LD_1ST_PAGE:
	    if((token = ArGetLR(fiP)) == -1) {
		ArTellError(ERR_PAGE1);
		break;
	    }
	    pgP->first_page = token;
	    break;
	case PN_TYPE:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	    pgP->pgnum_style = 
		ArTokenShr(fiP,al_token_shr, al_pagenum_tokens, NUMPNTS, LOOSE);
	    if (pgP->pgnum_style  == -1) {
		ArTellError(ERR_PN_TYPE);
		pgP->pgnum_style = PN_ARABIC;
	    }
	    break;
	case PN_PREFIX:
	    ArGetWordShrC(fiP, NOCHAR, pgP->pgnum_prefix, AR_MAX_WORD-1, COMMA);
	    break;
	case VJUST:
	    tempshort = FALSE;
	    if(ArGetYesNoShort(fiP, &tempshort) != NO) {
		if(tempshort) pgP->flags.vert_just = YES;
		else          pgP->flags.vert_just = NO;
	    }
	    break;
	case BLEED:
	    tempshort = FALSE;
	    if(ArGetYesNoShort(fiP, &tempshort) != NO) {
		if(tempshort) pgP->flags.bleed = YES;
		else          pgP->flags.bleed = NO;
	    }
	    break;
	case LD_PAGE_HYPHEN:
	    tempshort = TRUE;
	    ArGetYesNoShort(fiP, &tempshort);
	    if(tempshort) pgP->flags.hyphenation = YES;
	    else          pgP->flags.hyphenation = NO;
	    break;
	case LD_REV_BAR_PLACE:
	    /* some day could allow " = Auto" etc. */
	    if((tempint = ArGetLR(fiP)) != -1) {
		if (tempint == d_LEFT) {
		    pgP->flags.rev_bar_place = REV_BAR_LEFT;
		}
		else {
		    pgP->flags.rev_bar_place = REV_BAR_RIGHT;
		}
	    }
	    break;
	case BAL_COL:
	    tempshort = FALSE;
	    if(ArGetYesNoShort(fiP, &tempshort) != NO) {
		if(tempshort) pgP->flags.bal_columns = YES;
		else          pgP->flags.bal_columns = NO;
	    }
	    break;
	case FEATHER:
	    tempshort = FALSE;
	    if(ArGetYesNoShort(fiP, &tempshort)) {
		if(tempshort) pgP->flags.feathering = YES;
		else          pgP->flags.feathering = NO;
	    }
	    break;
	case VJUST_PAGES:
	    tempshort = FALSE;
	    if(ArGetYesNoShort(fiP, &tempshort) != NO) {
		if(tempshort) pgP->flags.vert_just_pages = YES;
		else          pgP->flags.vert_just_pages = NO;
	    }
	    break;
	case MAR_STR:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0 && tempint <= 400)
		pgP->margin_stretch = tempint;
	    break;
	case MAR_SHR:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0 && tempint <= 100)
		pgP->margin_shrink = tempint;
	    break;
	case FRAME_MAR_STR:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0 && tempint <= 100)
		pgP->frame_margin_stretch = tempint;
	    break;
	case MAX_FEATHER:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0 && tempint <= 100)
		pgP->max_feathering = tempint;
	    break;
	case DEPTH_PB:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0 && tempint <= 100)
		pgP->depth_pbreak = tempint;
	    break;
	case DEPTH_NPB:
	    if(ArGetInt(fiP, &tempint, YES, NO) && tempint >= 0 && tempint <= 100)
		pgP->depth_no_pbreak = tempint;
	    break;
	case LR_MARG:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	    if(ArStrncmpShr(al_token_shr, "Left/Right", 10, LOOSE) == 0) {
		pgP->l_r_margins = YES;
	    }	
	    else { 
		pgP->l_r_margins = NO;
	    }
	    break;
	case FREEZE_NUMS:
	    tempshort = NO;
	    if(ArGetYesNoShort(fiP, &tempshort) != NO) {
		if(tempshort) pgP->flags.frozen_autonums = YES;
		else          pgP->flags.frozen_autonums = NO;
	    }
	    break;
	case TURNED_PAGES:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr,AR_MAX_WORD-1,COMMA);
	    if (!ArStrncmpShr(al_token_shr,"90",2,LOOSE))
	      pgP->flags.turned_pages = TURNED_90;
	    else if (!ArStrncmpShr(al_token_shr,"270",3,LOOSE))
	      pgP->flags.turned_pages = TURNED_270;
	    else
	      pgP->flags.turned_pages = TURNED_0;
	    break;
	case BEGWITH:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr,AR_MAX_WORD-1,COMMA);
	    if (!ArStrncmpShr(al_token_shr,"Odd",3,LOOSE))
	      pgP->flags.page_parity = PG_PARITY_ODD;
	    else if (!ArStrncmpShr(al_token_shr,"Even",4,LOOSE))
	      pgP->flags.page_parity = PG_PARITY_ODD;
	    break;
	default:
	    pgP->unkP = 
		(Unknown_t *)ArBuildUnknownPropEq(fiP, al_token_shr, pgP->unkP);
	    break;
	}
    }
    if (pgP->micro_props_only == NO && pgP->width == 0) {
	ArFprintf(stderr, 
		"Warning: Width == 0 not allowed in non-microdoc <!Page>\n");
    }
    /** ArFprintf(stderr, "Adding PageInfo.\n"); BT **/
    return( (NullObj_t *)pgP );
}
/**--------------------------------------------------------------------------**/
static bool ArAnGetLevel();

NullObj_t *
ArBuildAnStreamInfo(fiP)
  register File_info_t *fiP;
{
    short	name[AR_MAX_ANTAGSIZE];
    register An_level_t *levP;
    int i;
    int c;
    int level;
    int nlevels;
    short tempshort;
    An_stream_info_t *ansP = NEW(An_stream_info_t);
    
    *ansP = default_an_stream;

    ArGetWordShrC(fiP, NOCHAR,  name, AR_MAX_ANTAGSIZE-1, COMMA);
    ansP->an_name = (shar *)ArSafeMalloc(sizeof(shar)*(1+ArShrlen(name)));
    ArShrcpy( ansP->an_name, name);

    ArGetInt(fiP, &nlevels, YES, NO);		/* number of levels */
    if(nlevels <= 0 || nlevels > AR_MAX_ANLEVELS) {
	ArTellError(ERR_AN_LEVELS);
	nlevels = AR_MAX_ANLEVELS; /* ??? */
    }
    ansP->an_levels = nlevels;

    while((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF){
	ArUnGetch(c, fiP);
	if (!ArAnGetLevel(fiP) || !ArGetInt(fiP, &level, YES, NO)
	   || level < 1 || level > nlevels)
	{
	    ArTellError(ERR_AN_LEVELS);
	    ArSkipPastComma(fiP);
	    continue;
	}
	levP = &ansP->an_level_info[level - 1];
	switch(ArGetTokenEq(fiP, al_anum_tokens, NUMATS)){
	case AN_SYMTYPE:
	    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	    for (i=tok_ARABIC; i <= AR_MAX_NUMSTYLE; i++)
		if (ArStrncmpShr(al_token_shr, NumStyle_names[i], 
				 AR_MAX_WORD, LOOSE) == 0) {
		    levP->an_symbol = i;
		    break;
		}
	    if (i > AR_MAX_NUMSTYLE) {
		ArTellError(ERR_AN_SYM);
		levP->an_symbol = tok_ARABIC;
	    }
	    break;
	case AN_PREFIX:
	    ArGetWordShrC(fiP, NOCHAR, levP->an_prefix, AR_MAX_ANFIX - 1, COMMA);
	    break;
	case AN_SUFFIX:
	    ArGetWordShrC(fiP, NOCHAR, levP->an_suffix, AR_MAX_ANFIX - 1, COMMA);
	    break;
	case AN_START:
	    if(ArGetInt(fiP, &i, YES, NO)) {
		if(i < -999 || i > 9999) ArTellError(ERR_START_AN);
		else
		{
		    levP->start_value = i;
		}
	    }
	    break;
	case AN_TRAIL:
	    ArGetYesNoShort(fiP, &levP->an_last_only);
	    break;
	case AN_SHOW:
	    ArGetYesNoShort(fiP, &levP->an_show);
	    break;
	case AN_INHERIT:
	    tempshort = TRUE;
	    ArGetYesNoShort(fiP, &tempshort);	/** ? apparently ignored. BT **/
	    break;
	}
    }
    /** ArFprintf(stderr, "Adding AnStreamInfo.\n"); BT **/
    return( (NullObj_t *)ansP );
}

/*  skip everything until you hit a digit or terminator */
static bool
ArAnGetLevel(fiP)
  register File_info_t *fiP;
{
    register int c;
    while((c = ArGetchNW(fiP)) != COMMA && c != EOF && c != AR_CMD_TERM)
    {
	if (c <= ALD_CHTYP_MAX && ald_chtype[c] == DIG)
	{
	    ArUnGetch(c, fiP);
	    return(YES);
	}
    }
    ArUnGetch(c, fiP);
    return(NO);
}
/**--------------------------------------------------------------------------**/
static int
ArHdrFtrRotation(fiP)
  File_info_t *fiP;
{
    ArGetWordShrC(fiP, NOCHAR, al_token_shr,AR_MAX_WORD-1,COMMA);
    if (ArStrncmpShr(al_token_shr,"90",2,LOOSE) == 0) {
        return( TURNED_90 );
    }
    else if (ArStrncmpShr(al_token_shr,"270",3,LOOSE) == 0) {
        return( TURNED_90 );
    }
    else {
        return( TURNED_0 );
    }
}
 
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildHeaderFooter(fiP, type)
  register File_info_t *fiP;
  HeadFoot_type		type;
{
  HeaderFooter_t *hfP = NEW(HeaderFooter_t);
  ArDgHead_t *diagramP = NULL;
  int c;

    SET_NullObjFields(hfP, ID_HeadFoot);
    hfP->unkP  = NULL;
    hfP->diagram.version  = 0;
    hfP->diagram.contents = NULL;
    hfP->turned = TURNED_0;
    switch(type) {
	case  HEADER: hfP->type = hf_PLAIN_HDR; break;
	case FHEADER: hfP->type = hf_FIRST_HDR; break;
	case LHEADER: hfP->type = hf_LEFT_HDR;  break;
	case RHEADER: hfP->type = hf_RIGHT_HDR; break;
	case  FOOTER: hfP->type = hf_PLAIN_FTR; break;
	case FFOOTER: hfP->type = hf_FIRST_FTR; break;
	case LFOOTER: hfP->type = hf_LEFT_FTR;  break;
	case RFOOTER: hfP->type = hf_RIGHT_FTR; break;
	default:
	    free(hfP);
	    ArFprintf(stderr, 
		      "ArBuildHeaderFooter: unknown type %d.  Get help.\n",
		      type);
	    return( NULL );
    }

    while((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF) {
	ArUnGetch(c, fiP);
        switch(ArGetTokenEq(fiP, headfoot_tokens, NUMHFTS)) {
	case CENTERTEXT:
	case LEFTTEXT:
	case RIGHTTEXT:
	case FIRSTPAGE:
	case HFFONT:
	    ArFprintf(stderr, 
		      "ArBuildHeaderFooter: resave the document in TPS 4.0\n");
	    free( hfP );
	    return( NULL );
	    break;
	case HFFRAME:
	    if ((diagramP = ArSaveDg(fiP)) == NULL) {
		ArFprintf(stderr, 
			"ArBuildHeaderFooter: error reading  diagram.\n");
		free( hfP );
		return( NULL );
	    }
	    hfP->diagram = *diagramP;
	    break;
	case ROTATION:
	    hfP->turned = ArHdrFtrRotation(fiP);
	    break;
        default      :
	    hfP->unkP = 
		(Unknown_t *)ArBuildUnknownPropEq(fiP, al_token_shr, hfP->unkP);
	    break;
	}
    }	
    /** ArFprintf(stderr, "Adding HeaderFooter.\n"); BT **/
    return( (NullObj_t *)hfP );
}
/**--------------------------------------------------------------------------**/
void
arParseUserAttr(fiP,avheadptr)
  File_info_t *fiP;
  Attr_t **avheadptr;
{
  shar buf[AR_MAX_QSTR], buf2[AR_MAX_QSTR];
  Attr_t *atP;
  Attr_t *listP;
  int length = 0;

    atP        = NEW(Attr_t);
    atP->nextP = NULL;

    if (avheadptr != NULL					 &&
	ArGetWordShrC(fiP, NOCHAR, buf, NUM_OF(buf)-1, 0) != FALSE &&
	ArGetchNW(fiP) == '=' 				 	 &&
	ArGetWordShrC(fiP, NOCHAR, buf2, NUM_OF(buf2)-1, 0) != FALSE) {
	    atP->nameP = (shar *)ArSafeMalloc((1+ArShrlen(buf))*sizeof(shar));
	    ArShrcpy(atP->nameP, buf);
	    atP->stringP = (shar *)ArSafeMalloc((1+ArShrlen(buf2))*sizeof(shar));
	    ArShrcpy(atP->stringP, buf2);
	    if (*avheadptr == NULL) {
		*avheadptr = atP;
	    }
	    else {
		for (listP = *avheadptr;listP&&listP->nextP;listP=listP->nextP){
		    /** advance to end of list **/
		    if (ARdebug) {
			ArFprintf(stderr, 
			    "arParseUserAttr: length %d, nameP %Q, stringP%Q\n",
			    length, listP->nameP, listP->stringP);
		    }
		    /**/length++;
		}
		listP->nextP = atP;
	    }
    }
    else {
	free(atP);
    }
    ArSkipPastComma(fiP);
}
/**--------------------------------------------------------------------------**/
