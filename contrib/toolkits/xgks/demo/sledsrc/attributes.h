/* 
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 */

#ifndef _gks_types
#include <xgks.h>
#endif

#include "changes.h"

/* This file is not needed at all under new xGKS. */

/* variables needed for call inquire primitive attribute values */

struct prim_attr_val	/* equivalent to Gpriattr */ 
{
	Gint error;
	IDX pline_idx;
	IDX pmark_idx;
	IDX text_idx;
	IDX fill_idx;
	Gfloat char_height;
	Gpoint char_up;
	Gpoint pattern_size;
	Gpoint pattern_ref_pt;
	ENUM text_path;
	ENUM text_align_horz;
	ENUM text_align_vert;
	PICKID pickid;
};

struct indiv_attr_val	/* equiv. to Gindattr */
{
	Gint error;
	ENUM pline_type;
	ENUM pmark_type;
	ENUM text_precision;
	IDX text_font;
	ENUM aspect_flags[13];
	Gfloat pline_width;
	Gfloat pmark_size;
	Gfloat text_expansion;
	Gfloat text_spacing;
	IDX pline_color;
	IDX pmark_color;
	IDX text_color;
	ENUM fill_style;
	IDX fill_style_idx;
	IDX fill_color;
};

struct state_tbl_sz	/* Is replaced by type "Gwstables." */
{
	ENUM pline;
	ENUM pmark;
	ENUM text;
	ENUM fill;
	ENUM pattern;
	ENUM color;
};

struct pline_fac_val	/* Is replaced by type "Glnfac." */
{
	Gint ntypes;
	Gint nwidths;
	Gint npredef;
	Gfloat nom_width;
	Gfloat min_width;
	Gfloat max_width;
	ENUM *list;
};

struct font_pair
{
    int font;
    ENUM precision;
};

typedef struct font_pair FONT_PAIR;

struct text_fac_val	/*  Is replaced by type "Gtxfac." */
{
	Gint nfonts;
	Gint nheights;
	Gint nexpansion;
	Gint npredef;
	Gfloat min_height;
	Gfloat max_height;
	Gfloat min_expansion;
	Gfloat max_expansion;
	FONT_PAIR *fontpairs;
};

void get_cur_primitive_attributes( struct prim_attr_val *attr);
