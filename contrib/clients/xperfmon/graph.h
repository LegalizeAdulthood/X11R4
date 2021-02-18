/*
 * $XConsortium: graph.h,v 1.3 89/09/19 14:44:14 jim Exp $
 *
 * Perfmon Performance Monitor
 *
 * Copyright 1985, Massachusetts Institute of Technology
 * Copyright 1989, PCS Computer Systeme GmbH, West Germany
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of PCS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  PCS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL PCS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Emanuel Jay Berkenbilt, MIT Project Athena
 * Author:           Thomas A. Baghli, PCS Computer Systeme GmbH, West Germany
 *                   tom@meepmeep.pcs.com
 */


/* This is a padding along the inner border of the
 * window inside of which nothing will be drawn.
 */
#define INNER_BORDER ((int) (.3 * (float) pw->perfmon.font_height))
/* This is a similar border but it appears around each graph. */
#define GRAPH_BORDER ((int) (.2 * (float) pw->perfmon.font_height))
/* This appears around each label. */
#define FONT_PAD ((int) (.1 * (float) pw->perfmon.font_height))
/* height in pixels of the name of the host and the padding around it */
#define HOST_HEIGHT (FONT_PAD * 2 + pw->perfmon.font_height)
/* height in pixels of a single graph when the window is at its smallest */
#define MIN_GRAPH_HEIGHT ((graph_set.lines_per_label + 1) \
    * (pw->perfmon.font_height + 2 * FONT_PAD))
/* default height of the graph */
#define GRAPH_HEIGHT ((graph_set.lines_per_label * 2) \
    * pw->perfmon.font_height)
/* height of the drawable area inside of a graph */
#define DRAW_GRAPH_HEIGHT(i) (graph_list[i]->min_pix - graph_list[i]->max_pix)

/* Amount of space on the top and bottom of the
 * window that will not be used by the graphs
 */
#define TOPSTUFF (INNER_BORDER + (int) pw->core.border_width + HOST_HEIGHT)
#define BOTTOMSTUFF (INNER_BORDER + (int) pw->core.border_width)

/* An expression that will scale a pixel value for graph number i.
 */
#define SCALE(i,n) (((n) * DRAW_GRAPH_HEIGHT(i)) \
     / ((graph_list[i]->max_value == graph_list[i]->min_value) ? 1 : \
	(graph_list[i]->max_value-graph_list[i]->min_value)))


/* graph variables */
/* These variables are necessary for the interface
 * between a list of numbers and a set of graphs.
 */

/* graph_set contains some constant information about the set of graphs */
struct {
    int num_graphs;
    int lines_per_label;
} graph_set;


/* There is an instance of this structure for each graph.  It contains 
 * information needed for drawing the graph.  The first three fields (label,
 * min_value, and max_value) are initialized at the beginning of the program.
 * The remaining fields change every time the window is resized.
 */
struct graph {
    char **label;
    int min_value;  /* min and max are raw values used to calculate scale. */
    int max_value;  /* See the comment at the top for a description of */
		    /* what is meant by "raw values." */
    int min_pix;    /* y pixel values for the bottom and top of the */
    int max_pix;    /* area in which the graph is actually drawn. */
    int zero;       /* y pixel value for zero on the graph */
};

struct {
    int g_width;    /* width of drawable graph in pixels */
    int left_pix;   /* x pixel values for left of graph drawing area */
    int cur_value;  /* saved value -index to the current item in saved_values */
    int **saved_values; /* This is an array int[value_num][graph_num] of
		     * raw values.  It is used for refreshing the graph.
		     * There is one saved value for each step on the 
		     * graph.  Each saved value corresponds to 
		     * x_params.step_size pixel values.
		     */
} graph_data;
