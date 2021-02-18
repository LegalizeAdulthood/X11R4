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

void DoKey()
{
    XComposeStatus status;
    int offset=0, buflen,oldstate;
    XLookupString(&event.xkey, keybuf, 16, &ks, &status);

    if(IsModifierKey(ks))
	return;
    if(*keybuf == 'Q')
	exit(2);

    switch(ClassifyWin(event.xkey.window)) {
      case INPUTWIN:
      case LIFEWIN:
	if((state!=RUN) && (state!=STOP) && (state!=HIDE)){
	    if((ks != XK_Return) && (ks != XK_Linefeed)) {
		if((ks == XK_BackSpace) || (ks == XK_Delete)) {
		    buflen = strlen(inpbuf);
		    if(buflen>minbuflen) {
			inpbuf[buflen - 1] = 0;
			XClearWindow(disp, inputw);
			offset = (buflen > INPUTLEN) ? buflen - INPUTLEN : 0;
			XDrawString(disp, inputw, ntextgc, ICOORDS(0,0), inpbuf + offset, buflen);
		    }
		}
		else {
		    if(ks == '~'){
			inpbuf[minbuflen] = NULL;
			XClearWindow(disp,inputw);
		    }
		    strcat(inpbuf, keybuf);
		    buflen = strlen(inpbuf);
		    if (buflen > INPBUFLEN) inpbuf[INPBUFLEN] = 0;
		    offset = (buflen > INPUTLEN) ? buflen - INPUTLEN : 0;
		    if (offset) XClearWindow(disp, inputw);
		    XDrawString(disp, inputw, ntextgc, ICOORDS(0,0), inpbuf + offset, buflen);
		}
	    }
	    else {
		XClearWindow(disp, inputw);
		oldstate=state;
		state=STOP;
		switch(oldstate){
		  case LOAD:
		    loadfile();
		    break;
		  case SAVE:
		    savefile();
		    break;
		  case RULES:
		    newrules();
		    break;
		  case HELP:
		    help();
		    break;
		  default:
		    break;
		}
		inpbuf[0]=0;
	    }
	}
	else{
	    DoKeyIn(keybuf);
	}
	
	break;
      default:
	write(1, keybuf, strlen(keybuf));
	fflush(stdout);
	break;
    }
}
