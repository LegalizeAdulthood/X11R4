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

void DoKeyIn(kbuf)
 char kbuf[16];
{
    switch(kbuf[0]){
      	case '4':
	  XClearWindow(disp,lifew);
	  xpos-= width/2 >> scale;
	  redrawscreen();
	break;
	case '6':
	  XClearWindow(disp,lifew);
	  xpos+= width/2 >> scale;
	  redrawscreen();
	break;
	case '8':
	  XClearWindow(disp,lifew);
	  ypos-= height/2 >> scale;
	  redrawscreen();
	break;
	case '2':
	  XClearWindow(disp,lifew);
	  ypos+= height/2 >> scale;
	  redrawscreen();
	break;
	case '5':
	  center();
	  break;
	case 'r':
	  redrawscreen();
	break;
	case 'R':
	  state=RULES;
	  newrules();
	  break;
	case '=':
	case '+':
	  if(scale < 7){
	      XClearWindow(disp,lifew);
	      scale++;
	      xpos+= width >> scale;
	      ypos+= height >> scale;
	      redrawscreen();
	  }
	  break;
	case '-':
	  if(scale > 1){
	      XClearWindow(disp,lifew);
	      xpos-= width >> scale;
	      ypos-= height >> scale;
	      scale--;
	      redrawscreen();
	  }
	  break;
	case 'g':
	  state=(state==RUN)?STOP:RUN;
	  break;
	case 'c':
	  dispcells ^=1;
	  break;
	case 'o':
	  generate();
	  break;
	case 'C':
	  clear();
	  break;
	case 's':
	  state=SAVE;
	  savefile();
	  break;
	case 'l':
	  state=LOAD;
	  loadfile();
	  break;
	case 'h':
	  state=(state==HIDE)?STOP:HIDE;
	  if(state!=HIDE){
	      XClearWindow(disp,lifew);
	      redrawscreen();
	  }
	  break;
	case '?':
	  state=(state==HELP)?STOP:HELP;
	  help();
	  break;
	case '!':
	  randomize();
	  break;
      default:
	break;
    }
}
