
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

/*	Copyright 1987, Interleaf Inc.		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/

#ifndef _ASC_LOAD_ERRS
#define _ASC_LOAD_ERRS

#define ERR_TAB_NUMBER	    1
#define ERR_UNITS_NUM	    2
#define ERR_RSU_UNITS	    3
#define ERR_MISS_EQ	    4
#define ERR_YESNO	    5
#define ERR_ALIGN	    6
#define ERR_DEFLT_TAB       7
#define ERR_CLASS_EXISTS    8
#define ERR_NO_CLASS        9
#define ERR_NAME_TOOLONG    10
#define ERR_NO_SUCH_CVAL    11
#define ERR_EXPECTED_TERM   12
#define ERR_EXPECTED_COMMA  13
#define ERR_NO_SUCH_DECL    14
#define ERR_ILLEGAL_CHAR    15
#define ERR_HEX		    16
#define ERR_NO_CMPN	    17
#define ERR_FONT_NUMBER     18
#define ERR_FONT0	    19
#define ERR_MAXFONT	    20
#define ERR_FONT_NAME	    21
#define ERR_FONT_REDEF	    22
#define ERR_FONT_SIZE	    23
#define ERR_FONT_CLOSE	    24
#define ERR_UNDEF_FONT      25
#define ERR_HEX_HIGH	    26
#define ERR_MICRO_FRAME	    27
#define ERR_HF_TOOLONG	    28
#define ERR_HF_REDEF	    29
#define ERR_WID_VAL	    30
#define ERR_ORPH_VAL	    31
#define ERR_DEF_NAME	    32
#define ERR_SELECT	    33
#define ERR_LINES_UNITS	    34
#define ERR_RSU_LINES	    35
#define ERR_LINES_RANGE	    36
#define ERR_TOOMANY_TOK	    37
#define ERR_DIAG	    38
#define ERR_TOOMANY_DIAG    39
#define ERR_SHARED	    40
#define ERR_NO_DIAG	    41
#define ERR_NO_FRM_MASTER   42
#define ERR_DUP_AN	    43
#define ERR_AN_LEVELS	    44
#define ERR_TOOMANY_TABS    45
#define ERR_SAME_TAB	    46
#define ERR_CMPN_CONTENTS   47
#define ERR_RSU_SIZE	    48
#define ERR_RSU_GT0	    49
#define ERR_FRM_PLACE	    50
#define ERR_FRM_HALIGN      51
#define ERR_DISC_HYPH	    52
#define ERR_PLACEMENT	    53
#define ERR_VALIGN_FLOAT    54
#define ERR_RSU_GE0	    55
#define ERR_PAGE_CONSIST    56
#define ERR_CMARGIN	    57
#define ERR_TBMARGINS	    58
#define ERR_LRMARGINS	    59
#define ERR_CIND	    60
#define ERR_CHIGH	    61
#define ERR_INCLUDE	    62
#define ERR_ATT_PREV	    63
#define ERR_FONT_QUACK	    64
#define ERR_ORIENT	    65
#define ERR_AFTER_CMPN	    66
#define ERR_MIXED_HF	    67
#define ERR_LR_PAGE	    68
#define ERR_PAGE1	    69
#define ERR_PN_TYPE	    70
#define ERR_CMPN_HYPH	    71
#define ERR_START_AN	    72
#define ERR_USCORE	    73
#define ERR_BACKSPACE	    74
#define ERR_PRTYPE	    75
#define ERR_NOT_ASCII	    76
#define ERR_INCLUDE_DECL    77
#define ERR_AN_SYM	    78
#define ERR_AN_DEF	    79
#define ERR_DUP_FRM_MASTER  80
#define ERR_TOOMANY_CELLS	    	81
#define ERR_UNEXPECTED_CHARACTER	82	/* Takes %c arg. */
#define ERR_NO_CREATE_ROWS		83
#define ERR_MISSING_ARG_AFTER_EQUALS	84	/* No arg. */
#define ERR_INDENT_COUNT_VAL		85
#define ERR_PROFILE_START		86
#define ERR_PROFILE_COUNT		87
#define ERR_PROFILE_INDENT		88
#define ERR_PROFILE_OVERLAP		89
#define ERR_FRM_VALIGN			90
#define ERR_DUP_TBL			91
#define ERR_TBL_COLS			92
#define ERR_TBL_FRAME			93
#define ERR_AN_RESTART			94
#define ERR_MICRO_PAGE			95
#define ERR_NULL_ATTRVAL_ATTR		96
#define ERR_NULL_ATTRVAL_VAL		97
#define ERR_CT_BAD_LOG_BASE		98
#define ERR_TABLE_ROW			99
#define ERR_NO_MASTER_ROWS		100
#define ERR_NO_SUB_END			101
#define ERR_MASTER_SUB			102
#define ERR_FORMSPACE			103
#define ERR_HALIGN_FIXED		104
#define ERR_FRM_AUTOWID			105
#define ERR_FRM_AUTOHGT			106
#define ERR_FRM_REPEAT			107
#define ERR_FRM_HREF			108
#define ERR_FRM_VREF			109
#define ERR_FIX_FRM_NUM			110
#define ERR_REPEAT_FRM_NUM		111
#define ERR_FRM_SUP_NONUM		112
#define ERR_NUM_FRM_INVIS		113
#define ERR_TBL_ROW			114
#define ERR_TBL_NOROWS			115
#define ERR_TBL_EMPTY			116
#define ERR_TBL_NO_MASTER		117
#define ERR_SHORT_ROW			118
#define ERR_CT_LABEL_TOO_LONG		119
#define ERR_COLOR_PROC			120

#endif /* ASC_LOAD_ERRS */
