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
 *  This file includes:
 *	init_clr_selector_clrs
 *	display_clr_picked_area
 *	display_picked_clr
 *	erase_picked_clr
 *	display_clr_wheel_area
 *	display_clr_mixer_area
 *	display_fine_tuner_area
 *	display_clr_selector_pal_area
 *	display_arrow
 *	draw_knob
 *	draw_arrow
 */

#include <math.h>
#include "key.h"
#include "defs.h"
#include "draw.h"
#include "functbl.h"
#include "clr_selector.h"
#include "color.h"
#include "trans.h"
#include <xgks.h>

BOOLEAN clr_picked;			/* index of color picked */
IDX display_clr_picked;			/* color picked up by user */
IDX mixer_clr;				/* first color in mixer from 
					   left to right */
IDX tuner_clr;				/* fine tuner color */
IDX wheel_start_clr;			/* first color in color wheel */
IDX num_wheel_clrs;			/* num clrs in clr wheel */

Gpoint clr_wheel_center;		/* center of clr clr wheel */
Gfloat outside_rad,inside_rad;		/* inside and outside rad of
					   color wheeel */
Gfloat theta;

Gpoint left_extent[2],middle_extent[2],
	right_extent[2];		/* mixer areas */
Gpoint tuner_extent[2];			/* tuner area */

ARROW mixer_arrow;			/* mixer arrow info */
ARROW val_arrow;			/* value arrow info */
ARROW sat_arrow;			/* sat arrow info */
ARROW hue_arrow;			/* hue arrow info */

/*
 *  init_clr_selector_clrs
 *
 *  description:	allocates color table space
 *			for color selecor colors
 *
 *  parameters:		none
 */

init_clr_selector_clrs()
{
	HSV hsv_clr;
	Gfloat theta_degrees;		/* theta in degrees */
	Gcobundl rgb_clr;
	int i;

	display_clr_picked = alloc_clr_tbl_entries(1);
	mixer_clr = alloc_clr_tbl_entries(3);
	tuner_clr = alloc_clr_tbl_entries(1);
	num_wheel_clrs = get_num_free_clrs();
	wheel_start_clr = alloc_clr_tbl_entries(num_wheel_clrs);

	theta = 2.0 * PI / ((Gfloat) num_wheel_clrs);
	theta_degrees = 360 / (2 * PI) * theta;
	hsv_clr.sat = 1.0;
	hsv_clr.val = 1.0;
	for (i=0; i<num_wheel_clrs; i++)
	{
		hsv_clr.hue = i * theta_degrees;
		hsv_to_rgb(&hsv_clr,&rgb_clr);
		set_color((wheel_start_clr + i),&rgb_clr);
	}

}  /* end init_clr_selector_clrs */


/*
 *  display_clr_picked_area
 *
 *  description:	allocates trans for clr picked area
 *
 *  parameters:		none
 */

display_clr_picked_area()
{
	if (find_trans(CLR_PICKED_AREA) == NON_EXISTENT)
		alloc_trans(CLR_PICKED_AREA);
	clr_picked = FALSE;

}  /* end display_clr_picked_area */


/*
 *  display_clr_picked
 *
 *  description:	sets clr_picked to clrno and
 *			displays clr_picked
 *
 *  parameters:		rgb_clr (Gcobundl) - color picked in Gcobundl form
 */

display_picked_clr(rgb_clr)
Gcobundl rgb_clr;
{
	Gfloat wd, ht;			/* wd and ht of conic */
	Gpoint window[2];		/* boundary of area */
	Gpoint center;			
	static char *lines[2] =	
	{
		"COLOR",
		"SELECTED"
	};

	push_curr_trans();
	activate(CLR_PICKED_AREA);
	set_aspect_flags(GBUNDLED);
	get_window(CLR_PICKED_AREA,window);

	/* display clr picked */

	wd = (3.0 / 4.0) * (window[MAX].x - window[MIN].x);
	ht = (1.0 / 2.0) * (window[MAX].y - window[MIN].y);
	center.x = (window[MAX].x - window[MIN].x) 
		* (1.0 / 2.0) + window[MIN].x;
	center.y = (window[MAX].y - window[MIN].y) 
		* (3.0 / 8.0) + window[MIN].y;

	set_color(display_clr_picked,&rgb_clr);
	set_free_fill_rep(display_clr_picked,GSOLID);
	gsetfillind(FREE_FILL_AREA_ENTRY);
	draw_conic(center,wd,ht);
	gsetfillind(WHITE_HOLLOW);
	draw_conic(center,wd,ht);

	/* display text */

	center.y = (window[MAX].y - window[MIN].y) 
		* (13.0 / 16.0) + window[MIN].y;
	center_text(2,lines,&center,white,CLR_PICKED_TEXT);

	clr_picked = TRUE;
	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();

	prompt(6);

}  /* end display_picked_clr */


/*
 *  erase_picked_clr
 *
 *  description:	erases clr_picked area and sets
 *			clr_picked to NO_CLR_PICKED
 *
 *  parameters:		none
 */

erase_picked_clr()
{
	erase_area(CLR_PICKED_AREA);
	clr_picked = FALSE;
	prompt(7);

}  /* end erase_picked_clr */


/*
 *  display_clr_wheel_area
 *
 *  description:	displays clr wheel area
 *
 *  parameters:		none
 */

display_clr_wheel_area()
{
	Gpoint window[4];
	Gpoint pts[4];
	int i;
	static char *lines[2] =
	{
		"COLOR",
		"WHEEL"
	};

	get_viewport(CLR_WHEEL_AREA,window);
	set_window(CLR_WHEEL_AREA,window);
	if (find_trans(CLR_WHEEL_AREA) == NON_EXISTENT)
		alloc_trans(CLR_WHEEL_AREA);
	push_curr_trans();
	activate(CLR_WHEEL_AREA);
	set_aspect_flags(GBUNDLED);

	outside_rad = min2((window[MAX].x - window[MIN].x),
		(window[MAX].y - window[MIN].y)) / 2.0;
	inside_rad = outside_rad * 2.0/ 5.0;
	clr_wheel_center.x = (window[MIN].x + window[MAX].x) 
		/ 2.0;
	clr_wheel_center.y = (window[MIN].y + window[MAX].y) 
		/ 2.0;

	pts[2].x = cos(- theta) * outside_rad 
		+ clr_wheel_center.x;
	pts[2].y = sin(- theta) * outside_rad 
		+ clr_wheel_center.y;
	pts[3].x = cos(- theta) * inside_rad + clr_wheel_center.x;
	pts[3].y = sin(- theta) * inside_rad + clr_wheel_center.y;
	for (i=0; i<num_wheel_clrs; i++)
	{
		pts[1].x = pts[2].x;
		pts[1].y = pts[2].y;
		pts[0].x = pts[3].x;
		pts[0].y = pts[3].y;

		pts[2].x = cos(i * theta) * outside_rad 
			+ clr_wheel_center.x;
		pts[2].y = sin(i * theta) * outside_rad 
			+ clr_wheel_center.y;
		pts[3].x = cos(i * theta) * inside_rad 
			+ clr_wheel_center.x;
		pts[3].y = sin(i * theta) * inside_rad 
			+ clr_wheel_center.y;

		set_free_fill_rep((wheel_start_clr + i),GSOLID);
		gsetfillind(FREE_FILL_AREA_ENTRY);
		gfillarea(4,pts);
	}

	center_text(2,lines,&clr_wheel_center,white,CLR_WHEEL_TEXT);
	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();

}  /* end display_clr_wheel_area*/


/*
 *  display_clr_mixer_area
 *
 *  description:	displays clr mixer on screen
 *			with left side containing black,
 *			right side containing white,
 *			and middle_extent containing medium grey
 *
 *  parameters:		none
 */

display_clr_mixer_area()
{
	Gpoint window[4];
	Gpoint left_box[4],right_box[4],
		middle_box[4];
	Gcobundl rgb_clr;
	Gpoint center;
	static char *lines[1] =
	{
		"COLOR MIXER"
	};

	get_viewport(MIXER_AREA,window);
	set_window(MIXER_AREA,window);
	if (find_trans(MIXER_AREA) == NON_EXISTENT)
		alloc_trans(MIXER_AREA);
	push_curr_trans();
	activate(MIXER_AREA);
	set_aspect_flags(GBUNDLED);

	/* draw_mixer */

	left_extent[MIN].x = window[MIN].x;
	left_extent[MAX].x = window[MIN].x + (window[MAX].x -
		window[MIN].x) / 4.0;
	left_extent[MIN].y = window[MIN].y + (window[MAX].y -
		window[MIN].y) / 4.0;
	left_extent[MAX].y = window[MAX].y - (window[MAX].y -
		window[MIN].y) / 4.0;

	right_extent[MIN].x = window[MAX].x - (window[MAX].x -
		window[MIN].x) / 4.0;
	right_extent[MAX].x = window[MAX].x;
	right_extent[MIN].y = left_extent[MIN].y;
	right_extent[MAX].y = left_extent[MAX].y;

	middle_extent[MIN].x = left_extent[MAX].x;
	middle_extent[MAX].x = right_extent[MIN].x;
	middle_extent[MIN].y = left_extent[MIN].y;
	middle_extent[MAX].y = left_extent[MAX].y;

	load_box(left_box,
		left_extent[MIN].x,left_extent[MAX].x,
		left_extent[MIN].y,left_extent[MAX].y);
	load_box(right_box,
		right_extent[MIN].x,right_extent[MAX].x,
		right_extent[MIN].y,right_extent[MAX].y);
	load_box(middle_box,
		middle_extent[MIN].x,middle_extent[MAX].x,
		middle_extent[MIN].y,middle_extent[MAX].y);

	rgb_clr.red = 0.0;
	rgb_clr.green = 0.0;
	rgb_clr.blue = 0.0;

	set_color(LEFT_CLR,&rgb_clr);

	rgb_clr.red = 1.0;
	rgb_clr.green = 1.0;
	rgb_clr.blue = 1.0;

	set_color(RIGHT_CLR,&rgb_clr);

	rgb_clr.red = 0.5;
	rgb_clr.green = 0.5;
	rgb_clr.blue = 0.5;

	set_color(MIDDLE_CLR,&rgb_clr);

	set_free_fill_rep(LEFT_CLR,GSOLID);
	gsetfillind(FREE_FILL_AREA_ENTRY);
	gfillarea(4,left_box);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,left_box);

	set_free_fill_rep(RIGHT_CLR,GSOLID);
	gsetfillind(FREE_FILL_AREA_ENTRY);
	gfillarea(4,right_box);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,right_box);

	set_free_fill_rep(MIDDLE_CLR,GSOLID);
	gsetfillind(FREE_FILL_AREA_ENTRY);
	gfillarea(4,middle_box);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,middle_box);

	/* draw mixer arrow */

	mixer_arrow.direction = HORZ;
	mixer_arrow.mini = window[MIN].x;
	mixer_arrow.maxi = window[MAX].x;
	mixer_arrow.con = window[MAX].y - (window[MAX].y -
		window[MIN].y) / 8.0;
	mixer_arrow.knob_pos = NO_KNOB_DRAWN;

	display_arrow(MIXER_AREA,&mixer_arrow,
		((mixer_arrow.mini + mixer_arrow.maxi) / 2.0));

	/* display "COLOR MIXER" */

	center.x = (window[MIN].x + window[MAX].x) / 2.0;
	center.y = window[MIN].y + (window[MAX].y -
		window[MIN].y) / 8.0;

	set_aspect_flags(GBUNDLED);
	center_text(1,lines,&center,white,MIXER_TEXT);
	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();

}  /* end display_clr_mixer */


/*
 *  display_fine_tuner_area
 *
 *  description:	displays fine tuner on screen
 *
 *  parameters:		none
 */

display_fine_tuner_area()
{
	Gpoint window[4];
	Gpoint box[4];
	Gcobundl rgb_clr;
	HSV hsv_clr;
	Gpoint center;
	static char *lines[4] =
	{
		"FINE TUNER",
		"HUE",
		"VALUE",
		"SATURATION"
	};
	char **ln_ptr;

	get_viewport(TUNER_AREA,window);
	set_window(TUNER_AREA,window);
	if (find_trans(TUNER_AREA) == NON_EXISTENT)
		alloc_trans(TUNER_AREA);
	push_curr_trans();
	activate(TUNER_AREA);
	set_aspect_flags(GBUNDLED);

	/* draw box with color */

	hsv_clr.hue = 0;
	hsv_clr.sat = 0.5;
	hsv_clr.val = 1.0;

	hsv_to_rgb(&hsv_clr,&rgb_clr);
	set_color(tuner_clr,&rgb_clr);
	set_free_fill_rep(tuner_clr,GSOLID);

	tuner_extent[MIN].x = window[MIN].x + (window[MAX].x
		- window[MIN].x) / 7.0;
	tuner_extent[MAX].x = window[MAX].x - (window[MAX].x
		- window[MIN].x) / 7.0;
	tuner_extent[MIN].y = window[MIN].y + (window[MAX].x
		- window[MIN].x) / 7.0;
	tuner_extent[MAX].y = window[MAX].y - (window[MAX].x
		- window[MIN].x) / 20.0;

	load_box(box,tuner_extent[MIN].x,tuner_extent[MAX].x,
		tuner_extent[MIN].y,tuner_extent[MAX].y);
	gsetfillind(FREE_FILL_AREA_ENTRY);
	gfillarea(4,box);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,box);

	/* display "FINE TUNER" */

	center.x = (tuner_extent[MIN].x + tuner_extent[MAX].x)
		/ 2.0;
	center.y = tuner_extent[MAX].y + (window[MAX].y
		- tuner_extent[MAX].y) / 2.0;
	ln_ptr = lines;
	center_text(1,ln_ptr++,&center,white,TUNER_TEXT);

	/* draw and label hue arrow */

	hue_arrow.direction = HORZ;
	hue_arrow.mini = tuner_extent[MIN].x;
	hue_arrow.maxi = tuner_extent[MAX].x;
	hue_arrow.con = tuner_extent[MIN].y - 
		(tuner_extent[MIN].y - window[MIN].y) / 3.0;
	hue_arrow.knob_pos = NO_KNOB_DRAWN;

	display_arrow(TUNER_AREA,&hue_arrow,((hsv_clr.hue / 360.0) * 
		(hue_arrow.maxi - hue_arrow.mini) + hue_arrow.mini));
	center.y = window[MIN].y + (tuner_extent[MIN].y - 
		window[MIN].y) / 4.0;

	set_aspect_flags(GBUNDLED);
	center_text(1,ln_ptr++,&center,white,TUNER_TEXT);

	/* draw and label value arrow */

	val_arrow.direction = VERT;
	val_arrow.mini = tuner_extent[MIN].y;
	val_arrow.maxi = tuner_extent[MAX].y;
	val_arrow.con = tuner_extent[MIN].x - 
		(tuner_extent[MIN].x - window[MIN].x) / 3.0;
	val_arrow.knob_pos = NO_KNOB_DRAWN;

	display_arrow(TUNER_AREA,&val_arrow,(hsv_clr.val * 
		(val_arrow.maxi - val_arrow.mini) + val_arrow.mini));
	center.x = window[MIN].x + 
		(tuner_extent[MIN].x - window[MIN].x) / 4.0;
	center.y = (val_arrow.mini + val_arrow.maxi) / 2.0;

	set_aspect_flags(GBUNDLED);
	center_text_vert(*ln_ptr,&center,white,TUNER_TEXT);
	ln_ptr++;

	/* draw and label saturation arrow */

	sat_arrow.direction = VERT;
	sat_arrow.mini = tuner_extent[MIN].y;
	sat_arrow.maxi = tuner_extent[MAX].y;
	sat_arrow.con = tuner_extent[MAX].x +
		(window[MAX].x - tuner_extent[MAX].x) / 3.0;
	sat_arrow.knob_pos = NO_KNOB_DRAWN;

	display_arrow(TUNER_AREA,&sat_arrow,(hsv_clr.sat * 
		(sat_arrow.maxi - sat_arrow.mini) + sat_arrow.mini));
	center.x = window[MAX].x - 
		(window[MAX].x - tuner_extent[MAX].x) / 4.0;

	set_aspect_flags(GBUNDLED);
	center_text_vert(*ln_ptr,&center,white,TUNER_TEXT);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();
}  /* end display_fine_tuner */


/*
 *  display_clr_selector_pal_area
 *
 *  description:	assumes palette already drawn
 *			just allocates trans
 *
 *  parameters:		area (AREA) - screen area
 *			transno (IDX) - transno corresponding
 *				to area
 *			pt (Gpoint) - pt picked
 */

display_clr_selector_pal_area(area,transno,pt)
AREA area;
IDX transno;
Gpoint pt;
{
	alloc_trans(CLR_SELECTOR_PAL_AREA);
}  /* end display_clr_selector_pal_area */


/*
 *  display_arrow
 *
 *  description:	displays arrow on screen with knob
 *			updated to new position
 *
 *  parameter:		arrow (ARROW) - arrow to display
 *			new_knob_pos (Gfloat) - new position for knob
 */

display_arrow(area,arrow,new_knob_pos)
AREA area;
ARROW *arrow;
Gfloat new_knob_pos;
{
	push_curr_trans();
	activate(area);
	set_aspect_flags(GBUNDLED);

	/* if old knob exists erase it */

	if (arrow->knob_pos != NO_KNOB_DRAWN)
	{
		draw_knob(arrow,BLACK_SOLID);
	}

	/* redraw (or if not previously on screen then ignore 're')
	  arrow */

	draw_arrow(arrow,white);

	/* draw knob in new position */

	arrow->knob_pos = new_knob_pos;
	draw_knob(arrow,WHITE_SOLID);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();
};  /* end display_arrow */


/*
 *  draw_knob
 *
 *  description:	draws knob in specified color on screen
 *
 *  parameters:		arrow (ARROW *) - arrow info
 *			fill_color (IDX) - index in fill_area bundle tbl
 */

draw_knob(arrow,fill_color)
ARROW *arrow;
IDX fill_color;
{
	Gfloat pos;			/* adjusted position of knob */
	Gpoint box[4];			/* pts of knob */

	pos = arrow->knob_pos;

	if (arrow->direction == VERT)
	{
		load_box(box,(arrow->con - KNOB_LEN / 2.0),
			(arrow->con + KNOB_LEN / 2.0),
			(pos - KNOB_WD / 2.0), (pos + KNOB_WD / 2.0));
	}
	else /* direction = HORZ */
	{
		load_box(box,(pos - KNOB_WD / 2.0),
			(pos + KNOB_WD / 2.0),
			(arrow->con - KNOB_LEN / 2.0),
			(arrow->con + KNOB_LEN / 2.0));
	}

	gsetfillind(fill_color);
	gfillarea(4,box);
};  /* end draw_knob */


/*
 *  draw_arrow
 *
 *  description:	draws arrow on screen using specified color
 *
 *  parameters:		arrow (ARROW *) - arrow to draw
 *			line_color (IDX) - color tbl index
 */

draw_arrow(arrow,line_color)
ARROW *arrow;
IDX line_color;
{
	Gpoint pts[3];

	/* draw main line of double-headed arrow */

	if (arrow->direction == VERT)
	{
		pts[0].x = arrow->con;
		pts[1].x = arrow->con;

		pts[0].y = arrow->mini;
		pts[1].y = arrow->maxi;
	}
	else
	{
		pts[0].x = arrow->mini;
		pts[1].x = arrow->maxi;

		pts[0].y = arrow->con;
		pts[1].y = arrow->con;
	}

	gsetlineind(line_color);
	gpolyline(2,pts);

	/* draw arrow heads */

	if (arrow->direction == VERT)
	{
		/* draw arrow at 'maxi' end of arrow */

		pts[0].x = arrow->con - ARROW_LEN;
		pts[1].x = arrow->con;
		pts[2].x = arrow->con + ARROW_LEN;

		pts[0].y = arrow->maxi - ARROW_LEN;
		pts[1].y = arrow->maxi;
		pts[2].y = arrow->maxi - ARROW_LEN;

		gpolyline(3,pts);

		/* draw arrow at 'mini' end of arrow */

		pts[0].y = arrow->mini + ARROW_LEN;
		pts[1].y = arrow->mini;
		pts[2].y = arrow->mini + ARROW_LEN;

		gpolyline(3,pts);
	}
	else
	{
		/* draw arrow at 'maxi' end of arrow */

		pts[0].y = arrow->con - ARROW_LEN;
		pts[1].y = arrow->con;
		pts[2].y = arrow->con + ARROW_LEN;

		pts[0].x = arrow->maxi - ARROW_LEN;
		pts[1].x = arrow->maxi;
		pts[2].x = arrow->maxi - ARROW_LEN;

		gpolyline(3,pts);

		/* draw arrow at 'mini' end of arrow */

		pts[0].x = arrow->mini + ARROW_LEN;
		pts[1].x = arrow->mini;
		pts[2].x = arrow->mini + ARROW_LEN;

		gpolyline(3,pts);
	}
};  /* end draw_color */
