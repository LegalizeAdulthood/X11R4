/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	Version 4.0
 */

#include <stdio.h>
#include <string.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define	XFlush(dpy)

unsigned char	*cancel_title = (unsigned char *)"キャンセル";

int to_save_x, to_save_y;

init_jutil(title, exec_t,  cnt, t1, mes1)
unsigned char *title, *exec_t;
int cnt;
unsigned char *t1[];
unsigned char *mes1[];
{
    XWindowAttributes	attr;
    XJClient	*xjc = jfew->xjclient;
    Jutil	*jutil = jfew->jutil;
    int	screen;
    int	width, height, x_cood, y_cood, root_x, root_y;
    Window	root, child;
    int	win_x, win_y, keys_buttons;
    int j;
    int len_t;

    XSizeHints hints;

    jfew->mode |= JUTIL;
    screen = DefaultScreen(dpy);

    XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&to_save_x, &to_save_y, &win_x, &win_y, &keys_buttons);
    root_x = JUTIL_X;
    root_y = JUTIL_Y;
    XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,root_x,root_y);
    width = FontWidth * 100;
    height = FontHeight * (3 * cnt + 3);
    x_cood = root_x - width/2;
    y_cood = root_y - FontHeight;
    if ((x_cood + width) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    if ((y_cood + height) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;

    jutil->title = title;
    jutil->cancel = cancel_title;
    jutil->exec = exec_t;
    for (j = 0; j < 4; j++) 
	jutil->mes_title[j] = NULL;

    XMoveResizeWindow(dpy, jutil->w, x_cood, y_cood, width, height);
    XMoveResizeWindow(dpy, jutil->title_w,
			FontWidth, FontHeight/2,
			FontWidth * (len_t=strlen(title)) + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jutil->cancel_w,
			FontWidth * (len_t + 5), FontHeight/2,
			FontWidth * 10 + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jutil->exec_w,
			FontWidth * (len_t + 18), FontHeight/2,
			FontWidth * (strlen(exec_t)) + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jutil->rk_mode_w,
			width - FontWidth * (MHL0 + 2), FontHeight/2,
			FontWidth * MHL0 + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);

    hints.flags = USSize | USPosition;
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = width;
    hints.height = height;
    XSetNormalHints(dpy, jutil->w, &hints);

    XMapWindow(dpy, jutil->w);
    XRaiseWindow(dpy, jutil->w);
    XFlush(dpy);

    redraw_ichi_box(jutil->title_w, jutil->title, 0);
    redraw_ichi_box(jutil->cancel_w, jutil->cancel, 0);
    redraw_ichi_box(jutil->exec_w, jutil->exec, 0);
    disp_mode();

    jutil->mode = MESSAGE1;

    for (j = 0; j < cnt; j++) {
	jutil->mes_title[j] = t1[j];
	cur_jutil = jutil->mes_jutil[j];
	JWMline_clear(0);
	XResizeWindow(dpy, jutil->mes_w[j], strlen(t1[j]) * FontWidth,
			FontHeight);
	XMapWindow(dpy, jutil->mes_w[j]);

	hanten_jutil_mes_title(j, ((jutil->mode == j)? 1:0));
	/*
        redraw_ichi_box(jutil->mes_w[j], jutil->mes_title[j],
    			((jutil->mode == j)? 1:0));
	*/

	XMapWindow(dpy, jutil->mes_jutil[j]->cl->w);
	if (mes1[j] && *mes1[j]) {
	    /*
	    cur_jutil = jutil->mes_jutil[j];
	    JWMline_clear(0);
	    */
	    xw_jutil_write_msg(mes1[j]);
	}
    }
}

end_jutil()
{
    int j;
    extern int exec_title_in, cancel_in;

    XClearWindow(dpy, jfew->jutil->cancel_w);
    XClearWindow(dpy, jfew->jutil->exec_w);
    for (j = 0; j < 4; j++)  {
	clear_jutil_buf(jfew->jutil->mes_jutil[j]->buf);
	XClearWindow(dpy, jfew->jutil->mes_w[j]);
	XUnmapWindow(dpy, jfew->jutil->mes_w[j]);
	XClearWindow(dpy, jfew->jutil->mes_jutil[j]->cl->w);
	XUnmapWindow(dpy, jfew->jutil->mes_jutil[j]->cl->w);
    }
    XUnmapWindow(dpy, jfew->jutil->w);
    if (to_save_x > -1)
    	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,
		     to_save_x,to_save_y);
    XFlush(dpy);
    exec_title_in = 0;
    cancel_in = 0;
    jfew->mode &= ~JUTIL;
}

clear_jutil_buf(buf)
int *buf;
{
	register int *x;

	x = buf;
	for (; *x != 0; x++) {
		*x = 0;
	}
}

change_cur_jutil(cur)
int cur;
{
    cur_jutil = jfew->jutil->mes_jutil[cur];
}

change_mes_title(mes, msg, rev)
int mes;
unsigned char *msg;
int rev;
{
    jfew->jutil->mes_title[mes] = msg;
    hanten_jutil_mes_title(mes, rev);

}

hanten_jutil_mes_title(mes, rev)
int	mes;
int	rev;
{
    unsigned char	*x;
    int		column = 0;
    int		ret;

    XClearWindow(dpy, jfew->jutil->mes_w[mes], 0, 0, 0, 0);
    ret = JW3Mputc(jfew->jutil->mes_title[mes], jfew->jutil->mes_w[mes],
			0, rev, 0);
}

reverse_jutil(win, msg, in)
Window	win;
unsigned char	*msg;
int		in;
{
	unsigned char	*x;
	int		column = 0;
	int		ret;
	XJClient	*xjc;

	xjc = jfew->xjclient;
	XFillRectangle(dpy, win, jfew->ichi->invertgc,
			0, 0, strlen(msg) * FontWidth + in * 2,
			FontHeight + in * 2);
}

xw_jutil_write_msg(msg)
unsigned char	*msg;
{
	unsigned char	*x;
	for (x = msg; *x != 0; x++) {
		JW1Mputc(*x);
	}
	if (cur_jutil->cl->currentcol > cur_jutil->cl->max_pos) {
		cur_jutil->cl->max_pos = cur_jutil->cl->currentcol;
	}
	XFlush(dpy);
}
