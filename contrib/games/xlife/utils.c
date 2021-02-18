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

fatal(s)
char *s;
{
    fprintf(stderr, s);
    exit(-22);
}

int ClassifyWin(win)
Window win;
{
    if(win == lifew)
	return LIFEWIN;
    if(win == inputw)
	return INPUTWIN;
    if(win == mainw)
	return MAINWIN;
}

char *itoa(n)
int n;
{
    static char buf[16];
    char sign;
    char *ptr;

    if (n >= 0)
	sign = ' ';
    else {
	n = -n;
	sign = '-';
    }
    buf[15] = 0;
    ptr = buf + 14;
    do {
	*--ptr = n % 10 + '0';
	if (ptr <= buf)
	    return(buf);
	n /= 10;
    } while (n > 0);
    *--ptr = sign;
    return(ptr);
}

drawpoint(x,y,c)
 int x,y,c;
{
    if(c){
	XDrawPoint(disp,lifew,whitegc,x,y);
    }
    else{
	XDrawPoint(disp,lifew,blackgc,x,y);
    }
}

drawbox(x,y,c)
 int x,y,c;
{
    int sc;
    sc = 1 << scale -1;
    if(c){
	XFillRectangle(disp,lifew,whitegc,x,y,sc,sc);
    }
    else{
	XFillRectangle(disp,lifew,blackgc,x,y,sc,sc);
    }
}
