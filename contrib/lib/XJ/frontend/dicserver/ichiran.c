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

#define	MAXLINES	128
#define	MAXCOLUMNS	200

#define strlen(s)	euclen(s)
int	screen;

int	max_line;
int	start_line;
int	end_line;
int	page;
int	cur_page;
int	kosuu_all;
int	ichiran_maxlen;
int	select_button;
int	hilited_item = -1;
int	item_width;
int	item_height;
int	in_window;
int	lock_flg = 0;

extern int	button_x;
extern int	button_y;

unsigned char	*ichiran_cancel_msg = (unsigned char *)"キャンセル";
unsigned char	*ichiran_next_msg = (unsigned char *)"次ページ";
unsigned char	*ichiran_pre_msg = (unsigned char *)"前ページ";

int cancel_in = 0;
int exec_title_in = 0;
int next_in = 0;
int back_in = 0;
int key_w0_in = 0;
int key_w1_in = 0;
int key_w2_in = 0;
int yes_w_in = 0;
int no_w_in = 0;

int ichi_save_x, ichi_save_y;

init_ichiran(info, kosuu,init, title, comment)
unsigned char **info;
int kosuu;
unsigned char *title;
unsigned char *comment;
{
    XJClient	*xjc;
    int	x_cood, y_cood, width, height, bwidth;
    int		maxlen;
    int		def_maxlen;
    int		def_height;
    int		cnt, ret;
    int		len_t;
    Window	root, child;
    int	root_x, root_y, win_x, win_y, keys_buttons;

    XSizeHints	hints;

    jfew->mode |= ICHIRAN;
    xjc = jfew->xjclient;
    if(comment != NULL)
	jfew->ichi->comment = comment;
    else
	jfew->ichi->comment = NULL;
    jfew->ichi->kosuu = kosuu;
    jfew->ichi->title = title;
    jfew->ichi->cancel = ichiran_cancel_msg;
    lock_flg = 0;

    XSelectInput(dpy, jfew->ichi->w1,
		EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		ButtonPressMask | ButtonReleaseMask);
	XDefineCursor(dpy, jfew->ichi->w1, XCreateFontCursor(dpy,52));

    screen = DefaultScreen(dpy);

    def_height = FontHeight * 4;

    start_line = 0;
    kosuu_all = kosuu;
    cur_page = 1;
    if (kosuu > 30) {
	page = kosuu / 30 + (kosuu % 30 ? 1 : 0);
	max_line = 30;
	end_line = max_line - 1;
	height = ((comment != NULL)?31:30) * (FontHeight 
		+ IN_BORDER) + 2 * FontHeight;
	def_maxlen = strlen(title) + 35;
    } else {
	page = 1;
	max_line = kosuu;
	end_line = kosuu - 1;
	height = ((comment != NULL)?kosuu+1:kosuu) * (FontHeight
		+ IN_BORDER) + 2 * FontHeight;
	def_maxlen = strlen(title) + 18;
    }
    if(comment != NULL)
	maxlen =  strlen(comment);
    else
    	maxlen = strlen(info[0]);
    for (cnt = 0; cnt < kosuu; cnt++) {
	jfew->ichi->buf[cnt] = info[cnt];
	if ((ret = strlen(jfew->ichi->buf[cnt])) > maxlen)
	    maxlen = ret + 2;
    }
    if (maxlen < def_maxlen) maxlen = def_maxlen;

    bwidth = 2;
    width  = item_width = maxlen * FontWidth;
    item_height = height;
    ichiran_maxlen = maxlen;

    if (select_button) {
	x_cood = button_x - width/2;
	y_cood = button_y - FontHeight;
	select_button = 0;
	ichi_save_x = ichi_save_y = -1;
    }else{
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&ichi_save_x, &ichi_save_y, &win_x, &win_y, &keys_buttons);
	root_x = JUTIL_X;
	root_y = JUTIL_Y;
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,root_x,root_y);
	x_cood = root_x - width/2;
	y_cood = root_y - FontHeight;
	
    }
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;
    if (y_cood < 0)
	y_cood = 0;

    XMoveResizeWindow(dpy, jfew->ichi->w,x_cood, y_cood, width, height);

    hints.flags = USSize | USPosition;
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = width;
    hints.height = height;
    XSetNormalHints(dpy, jfew->ichi->w, &hints);

    if(comment != NULL) {
	y_cood = FontHeight * 3 + IN_BORDER;
	XMoveResizeWindow(dpy, jfew->ichi->comment_w,0, FontHeight * 2,
			width, FontHeight);
	XMapWindow(dpy, jfew->ichi->comment_w);
	redraw_ichi_box(jfew->ichi->comment_w, jfew->ichi->comment, 0);
    } else {
	y_cood = FontHeight * 2;
    }
    XMoveResizeWindow(dpy, jfew->ichi->w1,-1, y_cood,
			width,
			((page > 1)? 30:kosuu) * (FontHeight + IN_BORDER));

    XMoveResizeWindow(dpy, jfew->ichi->title_w,
			FontWidth * 1, FontHeight/2,
			FontWidth * (len_t=strlen(title)) + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->cancel_w,
			FontWidth * (len_t + 5),FontHeight/2,
			10 * FontWidth + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    redraw_ichi_box(jfew->ichi->title_w, jfew->ichi->title, 0);
    redraw_ichi_box(jfew->ichi->cancel_w, jfew->ichi->cancel, 0);
    if(page > 1) {
	jfew->ichi->next = ichiran_next_msg;
	jfew->ichi->back = ichiran_pre_msg;
	XMoveResizeWindow(dpy, jfew->ichi->next_w,
			FontWidth * (len_t + 18),FontHeight/2,
			8 * FontWidth + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
	XMoveResizeWindow(dpy, jfew->ichi->back_w,
			FontWidth * (len_t + 28),FontHeight/2,
			8 * FontWidth + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
	redraw_ichi_box(jfew->ichi->next_w, jfew->ichi->next, 0);
	redraw_ichi_box(jfew->ichi->back_w, jfew->ichi->back, 0);
    	XMapWindow(dpy, jfew->ichi->next_w);
    	XMapWindow(dpy, jfew->ichi->back_w);
    }

    XRaiseWindow(dpy, jfew->ichi->w);
    XMapWindow(dpy, jfew->ichi->w);
    if (jfew->ichi->kosuu) {
	XMapWindow(dpy, jfew->ichi->w1);
	redraw_ichi_w(NULL);
	if (init > -1) {
	    hilited_item = init;
	    xw_fillrec();
	}
	XSelectInput(dpy, jfew->ichi->w1,
			EnterWindowMask | LeaveWindowMask
			| PointerMotionMask | ButtonPressMask
			| ButtonReleaseMask);
	XDefineCursor(dpy, jfew->ichi->w1, 
			XCreateFontCursor(dpy,132));
    }
    XFlush(dpy);
}

lock_ichiran()
{
    lock_flg = 1;
}

end_ichiran()
{
    int	cnt;
    if (jfew->ichi->kosuu) {
	if (hilited_item != -1)
	    xw_fillrec();
	hilited_item = -1;
	XUnmapWindow(dpy, jfew->ichi->w1);
    }
    XDefineCursor(dpy, jfew->ichi->w1);
    XSelectInput(dpy, jfew->ichi->w1, NoEventMask);
    XClearWindow(dpy, jfew->ichi->cancel_w);
    XClearWindow(dpy, jfew->ichi->w1);
    if (jfew->ichi->comment != NULL) {
	XClearWindow(dpy, jfew->ichi->comment_w);
	XUnmapWindow(dpy, jfew->ichi->comment_w);
    }
    XUnmapWindow(dpy, jfew->ichi->next_w);
    XUnmapWindow(dpy, jfew->ichi->back_w);
    XUnmapWindow(dpy, jfew->ichi->w);
    if (ichi_save_x > -1)
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,
		     ichi_save_x,ichi_save_y);
    XFlush(dpy);
    select_button = 0;
    lock_flg = 0;
    cancel_in = 0;
    jfew->mode &= ~ICHIRAN;
    select_return = -1;
}

unsigned char *off_mode_str = (unsigned char *)"[   ]";

int in_save_x, in_save_y;

init_input(msg1, msg2,msg3)
unsigned char *msg1, *msg2, *msg3;
{
    XJClient	*xjc;
    int	x_cood, y_cood, width, height, bwidth;
    int		maxlen;
    int		def_maxlen;
    int		cnt, ret;
    int		len_t;
    Window	root, child;
    int	root_x, root_y, win_x, win_y, keys_buttons;
    extern char *romkan_dispmode();
    char *p;

    XSizeHints	hints;

    cur_jutil = jfew->ichi->nyuuryoku_jutil;
    jfew->mode |= (ICHIRAN | NYUURYOKU);
    xjc = jfew->xjclient;
    jfew->ichi->kosuu = 0;
    jfew->ichi->title = msg1;
    jfew->ichi->subtitle = msg2;
    jfew->ichi->exec_title = msg3;
    jfew->ichi->cancel = ichiran_cancel_msg;
    if(NULL == (p = romkan_dispmode()))
	jfew->ichi->rk_mode = off_mode_str;
    else
    jfew->ichi->rk_mode = (unsigned char *)p;

    screen = DefaultScreen(dpy);

    height = FontHeight * 6;

    def_maxlen = MHL0 + 64;
    maxlen = strlen(msg1) + strlen(msg3) + 18; 
    if (maxlen < def_maxlen) maxlen = def_maxlen;

    bwidth = 2;
    width  = maxlen * FontWidth;

    if (select_button) {
	x_cood = button_x - width/2;
	y_cood = button_y - FontHeight;
	select_button = 0;
	in_save_x = in_save_y = -1;
    }else{
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&in_save_x, &in_save_y, &win_x, &win_y, &keys_buttons);
	root_x = JUTIL_X;
	root_y = JUTIL_Y;
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,root_x,root_y);
	x_cood = root_x - width/2;
	y_cood = root_y - FontHeight;
    }
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width - bwidth;
    if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height - bwidth;
    if (y_cood < 0)
	y_cood = 0;

    XMoveResizeWindow(dpy, jfew->ichi->w,x_cood, y_cood, width, height);

    XMoveResizeWindow(dpy, jfew->ichi->title_w,
			FontWidth, FontHeight/2,
			FontWidth * (len_t=strlen(msg1)) + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);

    XMoveResizeWindow(dpy, jfew->ichi->cancel_w,
			FontWidth * (len_t + 5),FontHeight/2,
			10 * FontWidth + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->exec_title_w,
			FontWidth * (len_t + 17),FontHeight/2,
			strlen(msg3) * FontWidth + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->subtitle_w,
			FontWidth * 2,FontHeight*2+FontHeight/2,
			strlen(msg2) * FontWidth + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->rk_mode_w,
			FontWidth * 1,
			FontHeight * 4 + FontHeight/2,
			FontWidth * MHL0, FontHeight + IN_BORDER * 2);

    hints.flags = USSize | USPosition;
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = width;
    hints.height = height;
    XSetNormalHints(dpy, jfew->ichi->w, &hints);

    XSelectInput(dpy, jfew->ichi->w, KeyPressMask);
    XRaiseWindow(dpy, jfew->ichi->w);
    XMapWindow(dpy, jfew->ichi->w);
    XMapWindow(dpy, jfew->ichi->exec_title_w);
    XMapWindow(dpy, jfew->ichi->subtitle_w);
    XMapWindow(dpy, jfew->ichi->rk_mode_w);
    XMapWindow(dpy, jfew->ichi->nyuuryoku_jutil->cl->w);
    XMapWindow(dpy, jfew->ichi->w);
    redraw_ichi_box(jfew->ichi->title_w, jfew->ichi->title, 0);
    redraw_ichi_box(jfew->ichi->cancel_w, jfew->ichi->cancel, 0);
    redraw_ichi_box(jfew->ichi->exec_title_w, jfew->ichi->exec_title, 0);
    redraw_ichi_box(jfew->ichi->subtitle_w, jfew->ichi->subtitle, 0);
    redraw_ichi_box(jfew->ichi->rk_mode_w, jfew->ichi->rk_mode, 0);
    XFlush(dpy);
}
end_input()
{
    XClearWindow(dpy, jfew->ichi->cancel_w);
    XClearWindow(dpy, jfew->ichi->exec_title_w);
    XUnmapWindow(dpy, jfew->ichi->exec_title_w);
    XUnmapWindow(dpy, jfew->ichi->subtitle_w);
    XUnmapWindow(dpy, jfew->ichi->rk_mode_w);
    XUnmapWindow(dpy, jfew->ichi->nyuuryoku_jutil->cl->w);
    XUnmapWindow(dpy, jfew->ichi->w);
    if (in_save_x > -1)
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,
		     in_save_x, in_save_y);
    XFlush(dpy);
    select_button = 0;
    cancel_in = 0;
    exec_title_in = 0;
    jfew->mode &= ~(ICHIRAN|NYUURYOKU);
    select_return = -1;
}

int key_save_x, key_save_y;

init_keytable(kosuu, title)
int kosuu;
unsigned char *title[];
{
    int cnt;
    int len, x, y;
    int width, height, x_cood, y_cood;
    Window	root, child;
    int	root_x, root_y, win_x, win_y, keys_buttons;
    Keytable *keytable = jfew->ichi->keytable;
    XJClient *xjc = jfew->xjclient;

    jfew->mode |= ICHIRAN;
    x = FontWidth * 2;
    y = FontHeight/2 - IN_BORDER;
    height = FontHeight + IN_BORDER * 2;
    keytable->cnt = kosuu;
    for (cnt = 0; cnt < kosuu; cnt++) {
	len = strlen(title[cnt]);
	keytable->key_t[cnt] = title[cnt];
	XMoveResizeWindow(dpy, keytable->key_w[cnt], x, y,
			FontWidth * len + IN_BORDER * 2, height);
	XMapWindow(dpy, keytable->key_w[cnt]);
	redraw_ichi_box(keytable->key_w[cnt], keytable->key_t[cnt], 0);
	x += FontWidth * (len + 2) + IN_BORDER * 2;
    }
    width = x;
    height = FontHeight * 2;
    if (select_button) {
	x_cood = button_x - width/2;
	y_cood = button_y - FontHeight;
	select_button = 0;
	key_save_x = key_save_y = -1;
    }else{
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&key_save_x, &key_save_y, &win_x, &win_y, &keys_buttons);
	root_x = JUTIL_X;
	root_y = JUTIL_Y;
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,root_x,root_y);
	x_cood = root_x - width/2;
	y_cood = root_y - FontHeight;
    }
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;
    if (y_cood < 0)
	y_cood = 0;
    XMoveResizeWindow(dpy, keytable->w, x_cood, y_cood, width, height);
    XRaiseWindow(dpy, keytable->w);
    XMapWindow(dpy, keytable->w);
    XFlush(dpy);
}

end_keytable()
{
    int cnt;

    for (cnt = 0; cnt < jfew->ichi->keytable->cnt; cnt++) {
	XClearWindow(dpy, jfew->ichi->keytable->key_w[cnt]);
	XUnmapWindow(dpy, jfew->ichi->keytable->key_w[cnt]);
    }
    XUnmapWindow(dpy, jfew->ichi->keytable->w);
    if (key_save_x > -1)
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,
		     key_save_x, key_save_y);
    XFlush(dpy);
    jfew->ichi->keytable->cnt = 0;
    key_w0_in = 0;
    key_w1_in = 0;
    key_w2_in = 0;
    jfew->mode &= ~ICHIRAN;
    select_return = -1;
}

unsigned char *yes_string = (unsigned char *)" はい ";
unsigned char *no_string = (unsigned char *)"いいえ";
unsigned char *ok_string = (unsigned char *)"ＯＫ？";

int yes_save_x, yes_save_y;

init_yes_or_no(str, flg)
unsigned char *str;
int flg;
{
    int cnt;
    int len, def_width, width, height, x_cood, y_cood;
    Window	root, child;
    int	root_x, root_y, win_x, win_y, keys_buttons;
    YesOrNo *yes_no = jfew->yes_no;
    XJClient *xjc = jfew->xjclient;

    yes_no->title = str;

    def_width = FontWidth * 18;
    height = FontHeight * 4;

    if((width = FontWidth * ((len = strlen(str)) + 2)) < def_width)
	width = def_width;
    XMoveResizeWindow(dpy, yes_no->title_w,
			FontWidth * 1, FontHeight/2 - IN_BORDER,
			FontWidth * len + IN_BORDER * 2,
			FontHeight + IN_BORDER * 2);
    redraw_ichi_box(yes_no->title_w, yes_no->title, 0);
    if (flg != 3) {
	if (flg == 1) {
	    yes_no->yes_t = yes_string;
	    yes_no->no_t = no_string;
	    XMapWindow(dpy, yes_no->yes_w);
	    XMapWindow(dpy, yes_no->no_w);
	    redraw_ichi_box(yes_no->yes_w, yes_no->yes_t, 0);
	    redraw_ichi_box(yes_no->no_w, yes_no->no_t, 0);
	} else {
	    yes_no->yes_t = ok_string;
	    XMapWindow(dpy, yes_no->yes_w);
	    redraw_ichi_box(yes_no->yes_w, yes_no->yes_t, 0);
	}
    }

    if (select_button) {
	x_cood = button_x - FontWidth * 3;
	y_cood = button_y - FontHeight * 3;
	select_button = 0;
	yes_save_x = yes_save_y = -1;
    }else{
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&yes_save_x, &yes_save_y, &win_x, &win_y, &keys_buttons);
	root_x = MESSAGE_X;
	root_y = MESSAGE_Y;
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,root_x,root_y);
	x_cood = root_x - FontWidth * 3;
	y_cood = root_y - FontHeight * 3;
    }
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;
    if (y_cood < 0)
	y_cood = 0;
    XMoveResizeWindow(dpy, yes_no->w, x_cood, y_cood, width, height);
    XRaiseWindow(dpy, yes_no->w);
    XMapWindow(dpy, yes_no->w);
    XFlush(dpy);
}

end_yes_or_no()
{
    XClearWindow(dpy, jfew->yes_no->title_w);
    XClearWindow(dpy, jfew->yes_no->yes_w);
    XClearWindow(dpy, jfew->yes_no->no_w);
    XUnmapWindow(dpy, jfew->yes_no->yes_w);
    XUnmapWindow(dpy, jfew->yes_no->no_w);
    XUnmapWindow(dpy, jfew->yes_no->w);
    if (yes_save_x > -1)
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,
		     yes_save_x, yes_save_y);
    XFlush(dpy);
    yes_w_in = 0;
    no_w_in = 0;
    select_return = -1;
}

JW3Mputc(s, w, li, rev, in)
unsigned char*s;
Window w;
int li, rev, in;
{
    unsigned short cdata = 0;
    unsigned char cc[2];
    register XJClient	*xjc;
    register int col = 0;
    register unsigned char *x;
    GC tmpGC, tmpGCK, tmpGCR;

    xjc = jfew->xjclient;

    if(rev) {
	tmpGC = jfew->reversegch;
	tmpGCK = jfew->reversegck;
	tmpGCR = jfew->reversegcr;
    } else {
	tmpGC = jfew->gch;
	tmpGCK = jfew->gck;
	tmpGCR = jfew->gcr;
    }

    for (x = s; x != 0 && *x != 0; x++) {
	if ((*x & 0x80) != 0 && *x != 0x8e) {
	    cc[0] =  *x++ & 0x7f;
	    cc[1] =  *x & 0x7f;
	    XDrawImageString16(dpy, w, tmpGCK, col * FontWidth + in,
		li * (FontHeight + in) + FontAscent + in,
		(XChar2b *)cc, 1);
	    col += 2;
	} else if(*x == 0x8e) {
	    cc[0] = (*++x & 0x7f);
	    XDrawImageString(dpy, w, tmpGCR, col * FontWidth + in,
		li * (FontHeight + in) + FontAscent + in,
		cc, 1);
	    col++;
	} else {
	    cc[0] = *x;
	    XDrawImageString(dpy, w, tmpGC, col * FontWidth + in,
		li * (FontHeight + in) + FontAscent + in,
		cc, 1);
	    col++;
	}
    }
}

extern unsigned char cursor_char[];

JW3mark_cursor(w, col)
Window w;
int col;
{
    register XJClient *xjc = jfew->xjclient;;

    XDrawImageString(dpy, w, jfew->reversegch, col * FontWidth,
			FontAscent, cursor_char, 1);
    XFlush(dpy);
}

JW3remark_cursor(w, col)
Window w;
int col;
{
    register XJClient *xjc = jfew->xjclient;;

    XDrawImageString(dpy, w, jfew->gch, col * FontWidth,
			FontAscent, cursor_char, 1);
    XFlush(dpy);
}

redraw_ichi_w(event)
XEvent *event;
{
    XJClient *xjc;
    Ichiran	 *ichi;
    int cnt, ret;
    int re_start_line, re_end_line;
    int write_line;
    int line = 0;
    register unsigned char *x;
    int rev = 0;

    xjc = jfew->xjclient;
    ichi = jfew->ichi;

    re_start_line = 0;
    re_end_line = max_line - 1;

    for (line = re_start_line, write_line = (re_start_line + start_line);
     (line <= re_end_line && write_line < kosuu_all); line++, write_line++) {
	x = ichi->buf[write_line];
	/*
	if (line == hilited_item)
		rev = 1;
	else
	*/
		rev = 0;
	JW3Mputc(x, ichi->w1, line, rev, IN_BORDER);
    }
    if (hilited_item != -1) {
	xw_fillrec();
    }
    XFlush(dpy);
}

next_ichi()
{
    if (page < 2)
	return(0);
    if (++cur_page > page)
	cur_page = 1;
    start_line = (cur_page - 1) * max_line;
    end_line = start_line + max_line - 1;
    XClearWindow(dpy, jfew->ichi->w1);
    redraw_ichi_w(NULL);
}

back_ichi()
{
    if (page < 2)
	return(0);
    if (--cur_page < 1)
	cur_page = page;
    start_line = (cur_page - 1) * max_line;
    end_line = start_line + max_line - 1;
    XClearWindow(dpy, jfew->ichi->w1);
    redraw_ichi_w(NULL);
}

find_hilite()
{
    return((cur_page - 1) * max_line + hilited_item);
}

xw_next_select()
{
  if (!lock_flg) {
    if (hilited_item != -1)
	xw_fillrec();
    if(hilited_item < jfew->ichi->kosuu - 1){
	hilited_item += 1;
    } else {
	hilited_item = 0;
    }
    xw_fillrec();
  }
}

xw_previous_select()
{
  if (!lock_flg) {
    if (hilited_item != -1)
	xw_fillrec();
    if(hilited_item > 0) {
	hilited_item -= 1;
    } else {
	hilited_item = jfew->ichi->kosuu - 1;
    }
    xw_fillrec();
  }
}


redraw_ichi_box(win,msg, rev)
Window	win;
unsigned char *msg;
int rev;
{
    int cnt, ret;
    int line = 0;

    JW3Mputc(msg, win, line, rev, IN_BORDER);
    XFlush(dpy);
}

xw_expose(event)
XExposeEvent *event;
{
}
xw_buttonpress(event)
XButtonEvent *event;
{
    Ichiran *ichi;
    Jutil *jutil;

    select_return = -1;
    if (event->window == jfew->yes_no->yes_w) {
	select_return = 1;
	return(0);
    } else if (event->window == jfew->yes_no->no_w) {
	select_return = 0;
	return(0);
    }
    if (jfew->mode & ICHIRAN) {
	ichi = jfew->ichi;
	if (event->window == ichi->w1 && !lock_flg) {
	    select_button = 1;
	    xw_select_button(event);
	    return(0);
	} else if (event->window == ichi->cancel_w) {
	    select_return = -2;
	    return(0);
	} else if (event->window == ichi->exec_title_w && !lock_flg) {
	    return(0);
	} else if (event->window == ichi->next_w && !lock_flg) {
	    next_ichi();
	    return(-1);
	} else if (event->window == ichi->back_w && !lock_flg) {
	    back_ichi();
	    return(-1);
	} else if (event->window == ichi->keytable->key_w[0]) {
	    select_return = 0;
	    return(0);
	} else if (event->window == ichi->keytable->key_w[1]) {
	    select_return = 1;
	    return(0);
	} else if (event->window == ichi->keytable->key_w[2]) {
	    select_return = 2;
	    return(0);
	}
    } else if (jfew->mode & JUTIL) {
	jutil = jfew->jutil;
	if (event->window == jutil->cancel_w) {
	    select_return = -2;
	    return(0);
	} else if (event->window == jutil->exec_w) {
	    select_return = 0;
	    return(0);
	} else if (event->window == jutil->mes_w[MESSAGE1]) {
	    jutil->mode = MESSAGE1;
	    select_return = 1;
	    return(0);
	} else if (event->window == jutil->mes_w[MESSAGE2]) {
	    jutil->mode = MESSAGE2;
	    select_return = 1;
	    return(0);
	} else if (event->window == jutil->mes_w[MESSAGE3]) {
	    jutil->mode = MESSAGE3;
	    select_return = 1;
	    return(0);
	} else if (event->window == jutil->mes_w[MESSAGE4]) {
	    jutil->mode = MESSAGE4;
	    select_return = 1;
	    return(0);
	}
    }
    return(-1);
}

xw_fillrec()
{
    XJClient *xjc = jfew->xjclient;

    if (hilited_item != -1) {
	XFillRectangle(dpy, jfew->ichi->w1, jfew->ichi->invertgc,
		0, hilited_item * (FontHeight + IN_BORDER),
		item_width, (FontHeight + IN_BORDER));
	XFlush(dpy);
    }
}

xw_select_button(event)
XButtonEvent *event;
{
    static int y;
    int item;

    y = event->y;
    if ((item = Y_InItem(y)) != hilited_item) {
	if (hilited_item != -1)
	    xw_fillrec();
    }
    XFlush(dpy);
    select_return = item;
}

xw_enterleave(event, el)
XEvent *event;
int el;
{
    int cnt;
    Ichiran *ichi;
    Jutil *jutil;

    if (event->xcrossing.window == jfew->yes_no->yes_w) {
	if (el != yes_w_in) {
	    reverse_jutil(jfew->yes_no->yes_w,
			jfew->yes_no->yes_t, IN_BORDER);
	    yes_w_in = el;
	}
	return(-1);
    } else if (event->xcrossing.window == jfew->yes_no->no_w) {
	if (el != no_w_in) {
	    reverse_jutil(jfew->yes_no->no_w,
			jfew->yes_no->no_t, IN_BORDER);
	    no_w_in = el;
	}
	return(-1);
    }
  if(jfew->mode & ICHIRAN) {
    ichi = jfew->ichi;
    if (event->xcrossing.window == ichi->w1 && !lock_flg) {
	if (el) {
	    in_window = 1;
	    xw_mousemove(event);
	} else {
	    in_window = 0;
	    xw_mouseleave();
	}
    } else if (event->xcrossing.window == ichi->cancel_w) {
	if (el != cancel_in) {
	    reverse_jutil(ichi->cancel_w,ichi->cancel,IN_BORDER);
	    cancel_in = el;
	}
    } else if (event->xcrossing.window == ichi->exec_title_w) {
	if (el != exec_title_in) {
	    reverse_jutil(ichi->exec_title_w,ichi->exec_title,IN_BORDER);
	    exec_title_in = el;
	}
    } else if (event->xcrossing.window == ichi->next_w) {
	if (el != next_in) {
	    reverse_jutil(ichi->next_w,ichi->next,IN_BORDER);
	    next_in = el;
	}
    } else if (event->xcrossing.window == ichi->back_w) {
	if (el != back_in) {
	    reverse_jutil(ichi->back_w,ichi->back,IN_BORDER);
	    back_in = el;
	}
    } else if (event->xcrossing.window == ichi->keytable->key_w[0]) {
	if (el != key_w0_in) {
	    reverse_jutil(ichi->keytable->key_w[0],
			ichi->keytable->key_t[0], IN_BORDER);
	    key_w0_in = el;
	}
    } else if (event->xcrossing.window == ichi->keytable->key_w[1]) {
	if (el != key_w1_in) {
	    reverse_jutil(ichi->keytable->key_w[1],
			ichi->keytable->key_t[1], IN_BORDER);
	    key_w1_in = el;
	}
    } else if (event->xcrossing.window == ichi->keytable->key_w[2]) {
	if (el != key_w2_in) {
	    reverse_jutil(ichi->keytable->key_w[2],
			ichi->keytable->key_t[2], IN_BORDER);
	    key_w2_in = el;
	}
    }
  } else if (jfew->mode & JUTIL) {
    jutil = jfew->jutil;
    if (event->xcrossing.window == jutil->cancel_w) {
	if (el != cancel_in) {
	    reverse_jutil(jutil->cancel_w,jutil->cancel,IN_BORDER);
	    cancel_in = el;
	}
    } else if (event->xcrossing.window == jutil->exec_w) {
	if (el != exec_title_in) {
	    reverse_jutil(jutil->exec_w,jutil->exec,IN_BORDER);
	    exec_title_in = el;
	}
    }
  }
    return(-1);
}

xw_mousemove(event)
XMotionEvent *event;
{
    static int y;
    int item;

  if(!lock_flg && jfew->mode & ICHIRAN) {
    y = event->y;
    item = Y_InItem(y);
    if (item != -1 && item != hilited_item) {
	if (hilited_item != -1)
	    xw_fillrec();
	if ((hilited_item = item) != -1)
	    xw_fillrec();
    }
  }
}

xw_mouseleave()
{
  if(!lock_flg && jfew->ichi->kosuu == 1 && jfew->mode & ICHIRAN) {
    if (hilited_item != -1) {
	xw_fillrec();
	hilited_item = -1;
    }
  }
}

Y_InItem(y)
int y;
{
    XJClient *xjc = jfew->xjclient;

    if (y < 0 || y > item_height)
	return(-1);
    return((y/(FontHeight + IN_BORDER)));
}

change_border(win, s, xjc, f)
Window win;
char *s;
XJClient *xjc;
int f;
{
    int width = strlen(s) * FontWidth + IN_BORDER * 2;
    int height = FontHeight + IN_BORDER * 2;

    if (f) {
	XFillRectangle(dpy, win, jfew->ichi->invertgc, 0, 0, width, height);
    } else {
	XFillRectangle(dpy, win, jfew->ichi->invertgc, 0, 0, width, height);
    }
    JW3Mputc(s, win, 0, 0, IN_BORDER);
    XFlush(dpy);
}

int euclen(s)
char *s;
{
    register int n;
    register char *c;

    for (c = s, n = 0; *c != 0; c++, n++) {
	if ((*c & 0xff) == 0x8e) c++;
    }
    return n;
}
