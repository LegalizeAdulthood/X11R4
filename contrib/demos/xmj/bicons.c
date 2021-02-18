/*
 * $XConsortium: bicons.c,v 1.3 89/05/05 12:18:39 jim Exp $
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

#include <sys/types.h>

/*
** Button tiles bitmaps (B/W)
*/
#include "tiles/bandw/HELP"
#include "tiles/bandw/AGAIN"
#include "tiles/bandw/NEW"
#include "tiles/bandw/UNDO"
#include "tiles/bandw/QUIT"

char *BandW_button_bitmaps[] =
{
	HELP_bits,
	AGAIN_bits,
	NEW_bits,
	UNDO_bits,
	QUIT_bits,
};

/*
** Button Tiles bitmaps (COLOR)
*/
#include "tiles/cHELP"
#include "tiles/cAGAIN"
#include "tiles/cNEW"
#include "tiles/cUNDO"
#include "tiles/cQUIT"

char *Color_button_bitmaps[] =
{
	cHELP_bits,
	cAGAIN_bits,
	cNEW_bits,
	cUNDO_bits,
	cQUIT_bits,
};
