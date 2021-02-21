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
 * This file contains:			New xGKS commands:
 *	get_cur_primitive_attributes	replaced by: ginqprimattr
 *	get_cur_individual_attributes	" " : ginqindivattr
 *	get_max_length_ws_tables	" " : ginqmaxwssttables
 *	get_pline_fac			" " : ginqlinefacil
 *	get_text_fac			" " : ginqtextfacil
 */

#include "attributes.h"
#include "ws.h"


/*
 *  get_cur_primitive_attributes
 *
 *  description:	returns list of current primitive attr values
 *
 *  parameters:		attr (struct prim_attr_val *) - struct to hold
 *			list of attributes
 */

get_cur_primitive_attributes(attr) 
struct prim_attr_val *attr;
{
	i_cur_primitive_attributes(&(attr->error),&(attr->pline_idx),
		&(attr->pmark_idx),&(attr->text_idx),
		&(attr->char_height),&(attr->char_up),
		&(attr->text_path),&(attr->text_align_horz),
		&(attr->text_align_vert),&(attr->fill_idx),
		&(attr->pattern_size), &(attr->pattern_ref_pt),
		&(attr->pickid));

} /* end get_cur_primitive_attributes */


/*
 *  get_cur_individual_attributes
 *
 *  description:	returns list of current inidividual attr values
 *
 *  parameters:		attr (struct indiv_attr_val *) - struct to hold
 *			list of attributes
 */

get_cur_individual_attributes(attr)
struct indiv_attr_val *attr;
{
	i_cur_individual_attributes(&(attr->error),&(attr->pline_type),
		&(attr->pline_width),&(attr->pline_color),
		&(attr->pmark_type),&(attr->pmark_size),
		&(attr->pmark_color),&(attr->text_font),
		&(attr->text_precision),&(attr->text_expansion),
		&(attr->text_spacing),&(attr->text_color),
		&(attr->fill_style),&(attr->fill_style_idx),
		&(attr->fill_color),attr->aspect_flags);

}  /* end get_cur_individual_attributes */



/*
 *  get_max_length_ws_tables
 *
 *  description:	returns list of workstation table sizes
 *
 *  parameters:		attr (struct state_tbl_sz *) - struct to hold
 *			list of table sizes
 */

get_max_length_ws_tables(tbl_sz)
struct state_tbl_sz *tbl_sz;
{
	Gint error;

	i_max_length_ws_tables(ws_type,&error,&(tbl_sz->pline),
		&(tbl_sz->pmark),&(tbl_sz->text),
		&(tbl_sz->fill),&(tbl_sz->pattern),
		&(tbl_sz->color));

}	/* end get_max_length_ws_tables */



/*
 *  get_pline_fac
 *
 *  description:	inquires about polyline attributes
 *
 *  parameters:		attr (struct pline_fac_val *) - structure
 *				to hold info on line facilities
 */

get_pline_fac(attr) 
struct pline_fac_val *attr;
{
	Gint error;

	i_pline_fac(ws_type,&error,&(attr->ntypes),
		&(attr->list),&(attr->nwidths),&(attr->nom_width),
		&(attr->min_width),&(attr->max_width),&(attr->npredef));
}  /* end get_pline_fac */


/*
 *  get_text_fac
 *
 *  description:	inquires about text attributes
 *
 *  parameters:		attr (struct pline_fac_val *) - structure
 *				to hold info on text facilities
 */

get_text_fac(attr) 
struct text_fac_val *attr;
{
	Gint error;

	i_text_fac(ws_type,&error,&(attr->nfonts),
		&(attr->fontpairs), &(attr->nheights),
		&(attr->min_height), &(attr->max_height),
		&(attr->nexpansion), &(attr->min_expansion),
		&(attr->max_expansion), &(attr->npredef));
}  /* end get_text_fac */

