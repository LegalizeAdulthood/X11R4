
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
/*	ar5strings.c                                 */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  06/88    BT	Original coding
 */

/**--------------------------------------------------------------------------**/
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
#include "ar5defs.h"
#include "ar5private.h"
/**--------------------------------------------------------------------------**/
char AR_DECL_START_FMT[]		= "\n<!%s";
char AR_DECL_END_FMT[]			= ">\n";
char AR_OPS_DECL_FMT[]			= "<!%s";
char AR_CMD_PLAIN_FMT[]			= "<%s>";
char AR_CMD_START_FMT[]			= "<%s";
char AR_CMD_START_LINE_FMT[]		= "\n<%s";
char AR_CMD_END_FMT[]			= ">";
char AR_CMPN_START_FMT[]		= "<%Q";
char AR_CMPN_START_LINE_FMT[]		= "\n<%Q";
char AR_FONT_START_FMT[]		= "<";
char AR_FONT_END_FMT[]			= ">";
char AR_OBJ_NAME_FMT[]			= ", %Q";
char AR_INDEX_HEAD_FMT[]		= ",\n    %Q";
char AR_NAME_PAIR_FMT[]			= "%Q %Q ";
char AR_AN_LEVELS_FMT[]			= " %d";
char AR_PLAIN_STRING_FMT[]		= "%s";
char AR_PROP_NAME_FMT[]			= ",\n    %s = %Q";
char AR_PROP_LEVEL_NAME_FMT[]		= ",\n    Level %d %s = %Q";
char AR_PROP_STRING_FMT[]		= ",\n    %s = %s";
char AR_PROP_LEVEL_STRING_FMT[]		= ",\n    Level %d %s = %s";
char AR_PROP_YESNO_FMT[]		= ",\n    %s = %s";
char AR_PROP_LEVEL_YESNO_FMT[]		= ",\n    Level %d %s = %s";
char AR_PROP_PREFIX_YESNO_FMT[]		= ",\n    %s%s = %s";
char AR_PROP_DECIMAL_FMT[]		= ",\n    %s = %d";
char AR_PROP_LEVEL_DECIMAL_FMT[]	= ",\n    Level %d %s = %d";
char AR_PROP_COLUMN_DECIMAL_FMT[]	= ",\n    Column %d %s = %d";
char AR_PROP_PREFIX_DECIMAL_FMT[]	= ",\n    %s%s = %d";
char AR_PROP_PREFIX_SUFFIX_DEC_FMT[]    =",\n    %s%s = %d %s";
char AR_PROP_STRING_DECIMAL_FMT[]	= ",\n    %s = \"%d\"";
char AR_PROP_INCH_FMT[]			= ",\n    %s = %.3f inches";
char AR_PROP_COLUMN_INCH_FMT[]		= ",\n    Column %d %s = %.3f inches";
char AR_PROP_PREFIX_INCH_FMT[]		= ",\n    %s%s = %.3f inches";
char AR_PROP_LINES_FMT[]		= ",\n    %s = %.3f lines";
char AR_PROP_3F_FMT[]			= ",\n    %s = %.3f";
char AR_PLAIN_3F_FMT[]			= ",\n    %.3f";
char AR_TAB_3F_FMT[]			= "%.3f%s";
char AR_ATTR_VAL_FMT[]			= ",\n    @%Q = %Q";
char AR_FONT_DEF_FMT[]			= ",\n    F%d = %#S %d %s";
char AR_PROP_KEYWORD_FMT[]		= ",\n    %s";
char AR_PROP_KEYWORD_EQ_FMT[]		= ",\n    %s = ";
char AR_OPS_VERSION_FMT[]		= ",\n    Version = %d.%d";
char AR_TYPE_INDEX_FMT[]		= ",\n    %C%d = ";
char AR_COLOR_DESC_FMT[]		= "%.3f, %.3f, %.3f";
char AR_LEFT_TAB_FMT[]			= ",\n    LeftTab = ";
char AR_RIGHT_TAB_FMT[]			= ",\n    RightTab = ";
char AR_CENTER_TAB_FMT[]		= ",\n    CenterTab = ";
char AR_DEC_TAB_FMT[]			= ",\n    DecimalTab = ";
char AR_DEC_COMMA_TAB_FMT[]		= ",\n    DecimalCommaTab = ";
char AR_UNKNOWN_DECL_FMT[]		= "<!%S --an unknown declaration-->\n";
char AR_UNKNOWN_PROP_FMT[]		= ",\n    %S  --an unknown property--";
char AR_DECL_COMMENT_FMT[]		= "\n<!Comment, %s>";
char AR_TEXT_CHAR_FMT[]			= "%#C";
char AR_CHAR_SPECIAL_FMT[]		= "<#%x>";
char AR_PATTERN_HEX_FMT[]		= "%04x";
char AR_PLUS_OFFSET_INCH_FMT[]		= " + %f inches";
char AR_PLUS_OFFSET_FMT[]		= " + %f";
char AR_MINUS_OFFSET_INCH_FMT[]		= " - %f inches";
char AR_MINUS_OFFSET_FMT[]		= " - %f";
char AR_TIMES_OFFSET_FMT[]		= " * %.2f";
char AR_PAGE_BREAK_FMT[]		= "<|,%#S>";
char AR_COMMENT_DECL_FMT[]		= "\n<!Comment,%#S>\n";
char AR_TAB_PLAIN_FMT[]			= "<Tab>";
char AR_TAB_LEADER_FMT[]		= "<Tab%c>";
char AR_FONT_LANG_FMT[]			= "@L%s";
/**--------------------------------------------------------------------------**/
shar EmptyName_shar[AR_MAX_NAME] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/**--------------------------------------------------------------------------**/
/** keep this in order with the 'lang_* defined values!			     **/
char *Language_names[] =
{
    "am",
    "br",
    "ca",
    "da",
    "du",
    "fi",
    "fr",
    "ge",
    "it",
    "nl",	/** the 'no language' case **/
    "no",
    "po",
    "sp",
    "sw",
};

/**--------------------------------------------------------------------------**/
char *Measure_names[] = 
{
    "Inches",
    "MM",
    "Picas",
    "Ciceros"
};
/**--------------------------------------------------------------------------**/
/** shared by autonumbers and a-page number styles. **/
char *NumStyle_names[] =
{
    "Arabic",
    "LowerRoman",
    "UpperRoman",
    "LowerAlpha",
    "UpperAlpha"
};
/**--------------------------------------------------------------------------**/
/**	Component contents are one of these.				     **/
char *Content_names[] = 
{
    "Private",
    "Prefix",
    "Shared",
    "PrefixShared"
};
/**--------------------------------------------------------------------------**/
/**	A Font face is one of these, or absent to indicate Roman.	     **/
char *FontFace_names[] =
{
    "Unknown",		/** should never happen **/
    "Roman",		/** used by ArIndexWrite() **/
    "Bold",		/** these three get used lots of places**/
    "Italic",
    "BoldItalic"
};
/**--------------------------------------------------------------------------**/
/**	Frame placement locations.					     **/
char *FramePlace_names[] =
{
    "AtAnchor",
    "FollowingAnchor",
    "FollowingText", 
    "Bottom", 
    "Middle", 
    "Top", 
    "Underlay", 
    "Overlay"
};
/**--------------------------------------------------------------------------**/
/**	Frame Horizontal Alignment locations.				     **/
char *FrameHAlign_names[] =
{
    "Left",
    "Center",
    "Right",
    "Inner",
    "Outer"
};
/**--------------------------------------------------------------------------**/
/**	Frame Vertical Alignment locations.				     **/
char *FrameVAlign_names[] =
{
    "Baseline",
    "Bottom",
    "Center",
    "Top",
};
/**--------------------------------------------------------------------------**/
/**	Frame Vertical Reference locations.				     **/
char *FrameVRef_names[] =
{
    "PageWithoutMargins",
    "PageWithBothMargins",
    "TopPageMargin",
    "BottomPageMargin",
    "Anchor"
};
/**--------------------------------------------------------------------------**/
/**	Frame Horizontal Reference locations.				     **/
char *FrameHRef_names[] =
{
    "PageWithoutMargins",
    "PageWithBothMargins",
    "LeftPageMargin",
    "RightPageMargin",
    "InnerPageMargin",
    "OuterPageMargin",
    "Column",
    "Anchor",
    "LeftGutter",
    "RightGutter"
};
/**--------------------------------------------------------------------------**/
/**	Frame Autosize Height locations.				     **/
/**	keep these in order with the #define values!		     **/
/**	empty strings are placeholders for values that need special **/
/**	action in the dumper.					    **/
char *FrameAutoHgt_names[] =
{
    "", 			/** ASZNOY	**/
    "Fixed", 			/** ASZHFIX	**/
    "", 			/** ASZEXY	**/
    "", 			/** ASZINY	**/
    "PageWithoutMargins", 	/** ASZHPAG	**/
    "PageWithBothMargins", 	/** ASZHPAGM   	**/
    "PageWithTopMargin", 	/** ASZHPAGT   	**/
    "PageWithBottomMargin", 	/** ASZHPAGB   	**/
    "TopPageMargin", 		/** ASZHTMARG  	**/
    "BottomPageMargin" 		/** ASZHBMARG  	**/
};
/**--------------------------------------------------------------------------**/
/**	Frame Autosize Width locations.				     **/
/**	keep these in order with the #define values!		     **/
/**	empty strings are placeholders for values that need special **/
/**	action in the dumper.					    **/
char *FrameAutoWid_names[] =
{
    "", 			/** ASZNOX	**/
    "Fixed", 			/** ASZWFIX	**/
    "", 			/** ASZEXX	**/
    "", 			/** ASZEXXCOL	**/
    "", 			/** ASZINX	**/
    "PageWithoutMargins", 	/** ASZWPAG	**/
    "Column", 			/** ASZWCOL	**/
    "PageWithBothMargins", 	/** ASZWPAGM	**/
    "PageWithLeftMargin", 	/** ASZWPAGL	**/
    "PageWithRightMargin", 	/** ASZWPAGR	**/
    "PageWithOuterMargin", 	/** ASZWPAGO	**/
    "PageWithInnerMargin", 	/** ASZWPAGI	**/
    "LeftPageMargin", 		/** ASZWLMARG	**/
    "RightPageMargin", 		/** ASZWRMARG	**/
    "OuterPageMargin", 		/** ASZWOMARG	**/
    "InnerPageMargin", 		/** ASZWIMARG	**/
    "Gutter" 			/** ASZWGUTTER	**/
    
};
/**--------------------------------------------------------------------------**/
char *HeaderFooter_names[] =
{
    "PageHeader",
    "PageFooter",
    "LeftPageHeader",
    "LeftPageFooter",
    "RightPageHeader",
    "RightPageFooter",
    "FirstPageHeader",
    "FirstPageFooter",
};
/**--------------------------------------------------------------------------**/
char *Composition_names[] =
{
    "Overset",
    "Optimum"
};
/**--------------------------------------------------------------------------**/
char *Attr_range_names[] =
{
    "Page",
    "Sheet",
    "Cmpn"
};
/**--------------------------------------------------------------------------**/
char *Attr_oper_names[] =
{
    "Max",
    "Min"
};
/**--------------------------------------------------------------------------**/
short arNameSearch(sP, cPP, limit)
  shar  *sP;
  char **cPP;
  int    limit;
{
  int result;

    for (result = 0; result <= limit; result++, cPP++) {
	if (ArShrstrcmp(sP, *cPP) == 0) {
	    return( result );
	}
    }
    return( -1 );
}
/**--------------------------------------------------------------------------**/
