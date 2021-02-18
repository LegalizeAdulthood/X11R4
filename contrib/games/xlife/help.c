/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "defs.h"
#include "stdio.h"
#include "data.h"
#include "stdio.h"
    
static char *strs[] = { "     XLife Help",
			"",
			"Move up            :8",
			"Move down          :2",
			"Move right         :6",
			"Move left          :4",
			"Center             :5",
			"Zoom in            :=,+",
			"Zoom out           :-",
			"Generate on/off    :g",
			"One generation     :o",
			"Cell count on/off  :c",
			"Save               :s",
			"Load               :l",
			"Hide               :h",
			"Draw               :Button 1",
			"Center on cursor   :Button 2",
			"Delete             :Button 3",
			"Help               :?",
			"Randomize          :!",
			"Redraw screen      :r",
			"New rules          :R",
			"Clear              :C",
			"Quit               :Q",
			"",
			"Hit Return to continue",
			0 };

void help() {

	if (state == HELP) {
		int i = 0;

		XClearWindow(disp,lifew);
		while (strs[i]) {
			XDrawString(disp,lifew,ntextgc,10,i*18+25,strs[i],strlen(strs[i]));
			i++;
		}
	} else {
		XClearWindow(disp,lifew);
		redrawscreen();
	}
}



