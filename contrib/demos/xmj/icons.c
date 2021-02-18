/*
 * $XConsortium: icons.c,v 1.3 89/05/05 12:18:58 jim Exp $
 *
 * Original source Copyright 1988 Exceptions
 * X11 portions Copyright 1989 Concurrent Computer Corporation
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Exceptions and Concurrent not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Exceptions and Concurrent make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 * 
 * EXCEPTIONS AND CONCURRENT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL EXCEPTIONS OR CONCURRENT BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Mark Holm, Exceptions, nosun!tessi!exc!markh@sun.com
 * 
 * Acknowledgments to Dorothy Robinson for her artistic abilities in drawing
 * the icons and to Jim Batch for technical support and graphical concepts 
 * Jim Batch for techical support and graphical concepts.
 * 
 * X11 port by:  Jay Estabrook, Concurrent Computer Corporation
 */


/*   This is the file for building the icon images for the */
/*	tiles both color and b/w.                             */

#include <sys/types.h>

/* Number tiles bitmaps (B/W) */

#include "tiles/bandw/n_0"
#include "tiles/bandw/n_1"
#include "tiles/bandw/n_2"
#include "tiles/bandw/n_3"
#include "tiles/bandw/n_4"
#include "tiles/bandw/n_5"
#include "tiles/bandw/n_6"
#include "tiles/bandw/n_7"
#include "tiles/bandw/n_8"
#include "tiles/bandw/n_9"

char *BandW_num_bitmaps[] = {
	n_0_bits,
	n_1_bits,
	n_2_bits,
	n_3_bits,
	n_4_bits,
	n_5_bits,
	n_6_bits,
	n_7_bits,
	n_8_bits,
	n_9_bits
};

/* Playing Tiles bitmaps (B/W) */

#include "tiles/bandw/blank"
#include "tiles/bandw/black"
#include "tiles/bandw/bam1"
#include "tiles/bandw/bam2"
#include "tiles/bandw/bam3"
#include "tiles/bandw/bam4"
#include "tiles/bandw/bam5"
#include "tiles/bandw/bam6"
#include "tiles/bandw/bam7"
#include "tiles/bandw/bam8"
#include "tiles/bandw/bam9"
#include "tiles/bandw/circ1"
#include "tiles/bandw/circ2"
#include "tiles/bandw/circ3"
#include "tiles/bandw/circ4"
#include "tiles/bandw/circ5"
#include "tiles/bandw/circ6"
#include "tiles/bandw/circ7"
#include "tiles/bandw/circ8"
#include "tiles/bandw/circ9"
#include "tiles/bandw/char1"
#include "tiles/bandw/char2"
#include "tiles/bandw/char3"
#include "tiles/bandw/char4"
#include "tiles/bandw/char5"
#include "tiles/bandw/char6"
#include "tiles/bandw/char7"
#include "tiles/bandw/char8"
#include "tiles/bandw/char9"
#include "tiles/bandw/c_east"
#include "tiles/bandw/c_west"
#include "tiles/bandw/c_south"
#include "tiles/bandw/c_north"
#include "tiles/bandw/d_green"
#include "tiles/bandw/d_red"
#include "tiles/bandw/d_white"
#include "tiles/bandw/s_autumn"
#include "tiles/bandw/s_spring"
#include "tiles/bandw/s_summer"
#include "tiles/bandw/s_winter"
#include "tiles/bandw/f_bam"
#include "tiles/bandw/f_mum"
#include "tiles/bandw/f_orch"
#include "tiles/bandw/f_plum"

char *BandW_blank_bitmaps[] = {
	blank_bits,
	black_bits,
};

char *BandW_tile_bitmaps[] = {
	circ1_bits,
	circ2_bits,
	circ3_bits,
	circ4_bits,
	circ5_bits,
	circ6_bits,
	circ7_bits,
	circ8_bits,
	circ9_bits,
	bam1_bits,
	bam2_bits,
	bam3_bits,
	bam4_bits,
	bam5_bits,
	bam6_bits,
	bam7_bits,
	bam8_bits,
	bam9_bits,
	char1_bits,
	char2_bits,
	char3_bits,
	char4_bits,
	char5_bits,
	char6_bits,
	char7_bits,
	char8_bits,
	char9_bits,
	d_green_bits,
	d_red_bits,
	d_white_bits,
	c_east_bits,
	c_west_bits,
	c_south_bits,
	c_north_bits,
	s_autumn_bits,
	s_summer_bits,
	s_spring_bits,
	s_winter_bits,
	f_orch_bits,
	f_mum_bits,
	f_bam_bits,
	f_plum_bits
};

