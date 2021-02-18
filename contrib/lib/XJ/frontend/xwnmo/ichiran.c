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
#include <stdio.h>
#include <string.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define	MAXLINES	20
#define	MAXCOLUMNS	200

#define strlen(s)	euclen(s)

extern XJInput *cur_input;
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
int	hilited_ji_item = -1;
int	item_width;
int	item_height;
int	in_window = 0;
int	lock_flg = 0;

extern int	button_x;
extern int	button_y;

unsigned char	*ichiran_cancel_msg = (unsigned char *)"キャンセル";
unsigned char	*ichiran_mouse_msg = (unsigned char *)"マウス選択";
unsigned char	*ichiran_key_msg = (unsigned char *)" キー選択 ";
unsigned char	*ichiran_next_msg = (unsigned char *)"次ページ";
unsigned char	*ichiran_pre_msg = (unsigned char *)"前ページ";

unsigned char	*dummy_title = (unsigned char *)"          ";

unsigned char	save_buf[MAXLINES][MAXCOLUMNS];

int mouse = 0;
int cancel_in = 0;
int next_in = 0;
int back_in = 0;
int mouse_in = 0;
int exec_in = 0;
int yes_w_in = 0;
int no_w_in = 0;
int del_in = 0;
int use_in = 0;

int ichi_save_x, ichi_save_y;

init_ichiran(info, kosuu,init, title, subtitle, exec_title, nyuu_col, mode)
unsigned char **info;
int kosuu;
unsigned char *title;
unsigned char *subtitle;
unsigned char *exec_title;
int nyuu_col, mode;
{
    XWindowAttributes	attr;
    XWindowAttributes	to_attr;
    XJClient	*xjc = jfew->jutilxjc;
    int	x_cood, y_cood, width, height, bwidth;
    int		maxlen;
    int		def_maxlen;
    int		def_height;
    int		cnt, ret;
    int		line = 0, column = 0;
    register unsigned char	*x;
    unsigned char	dummy_buf[2];
    int		tmp = 0;
    int		jikkou = 0;
    int		nyuu = 0;
    int		cntt;
    int		new_kosuu;
    int		len_t;
    Window	root, child;
    int	root_x, root_y, win_x, win_y, keys_buttons;

    XSizeHints hints;

    if (jfew->j_c)
	return(-1);
    jfew->j_c = cur_p;

    if (*subtitle == NULL) {
	/*
	subtitle = dummy_title;
	*/
	nyuu = 1;
    }
    if (*exec_title == NULL) {
	/*
	exec_title = dummy_title;
	*/
	jikkou = 1;
    }

    jfew->ichi->kosuu = kosuu;
    jfew->ichi->title = title;
    jfew->ichi->subtitle = subtitle;
    jfew->ichi->exec_title = exec_title;
    jfew->ichi->cancel = ichiran_cancel_msg;

    if (mouse) {
	jfew->ichi->mouse = ichiran_mouse_msg;
	XSelectInput(dpy, jfew->ichi->w1,
		EnterWindowMask | LeaveWindowMask | PointerMotionMask |
		ButtonPressMask | ButtonReleaseMask | ExposureMask);
	XDefineCursor(dpy, jfew->ichi->w1, XCreateFontCursor(dpy,52));
    } else {
	jfew->ichi->mouse = ichiran_key_msg;
    }

    jfew->ichi->mode = mode;
    screen = DefaultScreen(dpy);

    if (jikkou) {
	if (mode == JIKOUHO || mode == JIS_IN || mode == KUTEN_IN)
	    def_maxlen = strlen(title) + 32;
	else
	    def_maxlen = strlen(title) + 18;
    } else {
	def_maxlen = strlen(title) + strlen(exec_title) + 20;
    }
    def_height = FontHeight(xjc) * 4;

    if (mode != NYUURYOKU) {
	if (mode == SENTAKU) {
	    for (cnt = 0, cntt = 0; cnt < kosuu; cnt++) {
		/*
		sprintf(save_buf[cnt], "%c. ",((cntt > 9)? cntt - 10 + 'A': cntt + '0'));
		strcat(save_buf[cnt], info[cnt]);
		*/
		sprintf(save_buf[cnt], " %s ", info[cnt]);
		jfew->ichi->buf[cnt] = save_buf[cnt];
		if (cntt == 29)
		    cntt == 0;
		else
		    cntt++;
	    }
	} else {
	    new_kosuu = xw_set_jikouho(info,kosuu);
	    kosuu = new_kosuu;
	}
	maxlen = strlen(jfew->ichi->buf[0]);
	for (cnt = 1; cnt < kosuu; cnt++) {
	    if ((ret = strlen(jfew->ichi->buf[cnt])) > maxlen)
		maxlen = ret + 2;
	}
	start_line = 0;
	kosuu_all = kosuu;
	cur_page = 1;
	if (kosuu > 30) {
	    page = kosuu / 30 + (kosuu % 30 ? 1 : 0);
	    max_line = 30;
	    end_line = max_line - 1;
	    height = 30 * (FontHeight(xjc) + IN_BORDER) 
			+ (nyuu? 2:4) * FontHeight(xjc);
	} else {
	    page = 1;
	    max_line = kosuu;
	    end_line = kosuu - 1;
	    height = kosuu * (FontHeight(xjc) + IN_BORDER) 
			+ (nyuu? 2:4) * FontHeight(xjc);
	}
	if (!nyuu && (def_maxlen < (strlen(subtitle) + 8)))
	    def_maxlen = strlen(title) + 8;
	if (maxlen < def_maxlen)
	    maxlen = def_maxlen;
    } else if (mode == NYUURYOKU) {
	if (def_maxlen < (strlen(subtitle) + nyuu_col + 4))
	    maxlen = strlen(subtitle) + nyuu_col + 4;
	else
	    maxlen = def_maxlen;
	height = def_height;
    } else {
	maxlen = def_maxlen;
	height = def_height;
    }
    bwidth = 2;
    width  = item_width = maxlen * FontWidth(xjc);
    item_height = height;
    ichiran_maxlen = maxlen;

    if (select_button) {
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
		      &ichi_save_x, &ichi_save_y, &win_x, &win_y, &keys_buttons);
	x_cood = button_x - width/2;
	y_cood = button_y;
	select_button = 0;
    }else{
	if (cur_x->dispmode == 2) {
	    XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&ichi_save_x, &ichi_save_y, &win_x, &win_y, &keys_buttons);
	    XGetWindowAttributes(dpy, jfew->xjclient->wp, &to_attr);
	    /*
	    x_cood = to_attr.x;
	    y_cood = to_attr.y - height;
	    */
	    XTranslateCoordinates(dpy, jfew->xjclient->wp,
				  RootWindow(dpy, screen), to_attr.x, to_attr.y,
				  &root_x, &root_y, &child);
	    x_cood = root_x;
	    y_cood = root_y - height;
	} else {
	    XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&ichi_save_x, &ichi_save_y, &win_x, &win_y, &keys_buttons);
	/*
	    x_cood = root_x;
	    y_cood = root_y;
	*/
	    XGetWindowAttributes(dpy, cur_x->w, &to_attr);
	    x_cood = to_attr.x;
	    y_cood = to_attr.y + to_attr.height;
	}
    }
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    else if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;
    else if (y_cood < 0)
	y_cood = 0;

    XWarpPointer(dpy, None, RootWindow(dpy, screen), 0, 0, 0, 0,
			 x_cood + width/2, y_cood + FontHeight(xjc));

    XMoveResizeWindow(dpy, jfew->ichi->w,x_cood, y_cood, width, height);

    XMoveResizeWindow(dpy, jfew->ichi->w1,-1, FontHeight(xjc) * (nyuu ? 2:4),
			width,
			((page > 1)? 30:kosuu) * (FontHeight(xjc) + IN_BORDER));

    XMoveResizeWindow(dpy, jfew->ichi->title_w,
			FontWidth(xjc) * 1, FontHeight(xjc)/2,
			FontWidth(xjc) * (len_t=strlen(title)) + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->cancel_w,
			FontWidth(xjc) * (len_t + 5),FontHeight(xjc)/2,
			10 * FontWidth(xjc) + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->exec_title_w,
			FontWidth(xjc) * (len_t + 18) , FontHeight(xjc)/2,
			strlen(exec_title) * FontWidth(xjc) + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->subtitle_w,
			FontWidth(xjc), FontHeight(xjc) * 2 + FontHeight(xjc)/2,
			strlen(subtitle) * FontWidth(xjc) + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
    XMoveResizeWindow(dpy, jfew->ichi->mouse_w,
			FontWidth(xjc) * (len_t + 19),FontHeight(xjc)/2,
			10 * FontWidth(xjc) + IN_BORDER * 2, 
			FontHeight(xjc) + IN_BORDER * 2);
    /*
    XMoveResizeWindow(dpy, jfew->ichi->nyuuryoku_jutil->cl->w,
			FontWidth(xjc) * (strlen(subtitle) + 3),
			FontHeight(xjc) * 2 + FontHeight(xjc)/2,
			nyuu_col * FontWidth(xjc), FontHeight(xjc));
    */
    XMoveResizeWindow(dpy, jfew->ichi->nyuu_w,
			FontWidth(xjc) * (strlen(subtitle) + 3),
			FontHeight(xjc) * 2 + FontHeight(xjc)/2,
			nyuu_col * FontWidth(xjc), FontHeight(xjc));

    hints.flags = USSize | USPosition;
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = width;
    hints.height = height;
    XSetNormalHints(dpy, jfew->ichi->w, &hints);

    XRaiseWindow(dpy, jfew->ichi->w);
    XMapWindow(dpy, jfew->ichi->w);
    redraw_ichi_box(jfew->ichi->title_w, jfew->ichi->title, 0);
    redraw_ichi_box(jfew->ichi->cancel_w, jfew->ichi->cancel, 0);
    if (!nyuu) {
	XMapWindow(dpy, jfew->ichi->subtitle_w);
	/*
	XMapWindow(dpy, jfew->ichi->nyuuryoku_jutil->cl->w);
	*/
	XMapWindow(dpy, jfew->ichi->nyuu_w);
	redraw_ichi_box(jfew->ichi->subtitle_w, jfew->ichi->subtitle, 0);
    }
    if (!jikkou) {
	XMapWindow(dpy, jfew->ichi->exec_title_w);
	redraw_ichi_box(jfew->ichi->exec_title_w, jfew->ichi->exec_title, 0);
    }
    if (jfew->ichi->kosuu) {
	XMapWindow(dpy, jfew->ichi->w1);
	redraw_ichi_w(NULL);
	if (mode == JIKOUHO || mode == JIS_IN || mode == KUTEN_IN) {
	    XMapWindow(dpy, jfew->ichi->mouse_w);
	    redraw_ichi_box(jfew->ichi->mouse_w, jfew->ichi->mouse, 0);
	    hilited_ji_item = init;
	    xw_ji_fillrec();
	} else if (mode == SENTAKU) {
	    hilited_ji_item = -1;
	    XSelectInput(dpy, jfew->ichi->w1,
			EnterWindowMask | LeaveWindowMask
			| PointerMotionMask | ButtonPressMask
			| ButtonReleaseMask | ExposureMask);
	    XDefineCursor(dpy, jfew->ichi->w1, 
			XCreateFontCursor(dpy,132));
	    if(page > 1) {
		jfew->ichi->next = ichiran_next_msg;
		jfew->ichi->back = ichiran_pre_msg;
		XMoveResizeWindow(dpy, jfew->ichi->next_w,
			FontWidth(xjc) * (len_t + 18),FontHeight(xjc)/2,
			8 * FontWidth(xjc) + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
		XMoveResizeWindow(dpy, jfew->ichi->back_w,
			FontWidth(xjc) * (len_t + 28),FontHeight(xjc)/2,
			8 * FontWidth(xjc) + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
		redraw_ichi_box(jfew->ichi->next_w, jfew->ichi->next, 0);
		redraw_ichi_box(jfew->ichi->back_w, jfew->ichi->back, 0);
		XMapWindow(dpy, jfew->ichi->next_w);
		XMapWindow(dpy, jfew->ichi->back_w);
	    }

	}
    }
    XFlush(dpy);
}

end_ichiran()
{
    int	cnt;
    if (jfew->ichi->kosuu) {
	if (hilited_item != -1)
	    xw_fillrec();
	hilited_item = -1;
	if (hilited_ji_item != -1)
	    xw_ji_fillrec();
	hilited_ji_item = -1;
	XUnmapWindow(dpy, jfew->ichi->w1);
    }
    XDefineCursor(dpy, jfew->ichi->w1);
    XSelectInput(dpy, jfew->ichi->w1, NoEventMask);
    XClearWindow(dpy, jfew->ichi->subtitle_w);
    XClearWindow(dpy, jfew->ichi->cancel_w);
    XClearWindow(dpy, jfew->ichi->mouse_w);
    XClearWindow(dpy, jfew->ichi->exec_title_w);
    XClearWindow(dpy, jfew->ichi->w1);
    XUnmapWindow(dpy, jfew->ichi->w);
    XUnmapWindow(dpy, jfew->ichi->next_w);
    XUnmapWindow(dpy, jfew->ichi->back_w);
    XUnmapWindow(dpy, jfew->ichi->subtitle_w);
    /*
    XUnmapWindow(dpy, jfew->ichi->nyuuryoku_jutil->cl->w);
    */
    XUnmapWindow(dpy, jfew->ichi->nyuu_w);
    XUnmapWindow(dpy, jfew->ichi->exec_title_w);
    XUnmapWindow(dpy, jfew->ichi->mouse_w);
    if (ichi_save_x > -1)
	XWarpPointer(dpy, None, RootWindow(dpy, screen), 0, 0, 0, 0,
		     ichi_save_x, ichi_save_y);
    XFlush(dpy);
    select_button = 0;
    lock_flg = 0;
    cancel_in = mouse_in = exec_in = 0;
    jfew->j_c = NULL;
}

lock_inspect()
{
    lock_flg = 1;
}

unlock_inspect()
{
    lock_flg = 0;
}

unsigned char	*inspect_title = (unsigned char *)"インスペクト";
unsigned char	*inspect_del_msg = (unsigned char *)"削除";
unsigned char	*inspect_use_msg = (unsigned char *)"使用/不使用";
unsigned char	*inspect_next_msg = (unsigned char *)"次へ";
unsigned char	*inspect_back_msg = (unsigned char *)"前へ";
int inspect_len = 0;

int ins_save_x, ins_save_y;

init_inspect(msg)
unsigned char *msg;
{
    int cnt;
    int len, x, y;
    int width, height, x_cood, y_cood;
    Window	root, child;
    int	root_x, root_y, win_x, win_y, keys_buttons;
    Inspect *ins = jfew->inspect;
    XJClient *xjc = jfew->jutilxjc;
    XWindowAttributes	to_attr;

    XSizeHints hints;

    if (jfew->j_c != NULL)
	return(-1);
    jfew->j_c = cur_p;

    inspect_len = strlen(msg);
    width = FontWidth(xjc) * inspect_len;
    height = FontHeight(xjc) * 3 + IN_BORDER * 2;
    if (select_button) {
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
		      &ins_save_x, &ins_save_y, &win_x, &win_y, &keys_buttons);
	x_cood = button_x - width/2;
	y_cood = button_y - FontHeight(xjc);
	select_button = 0;
    }else{
	if (cur_x->dispmode == 2) {
	    XGetWindowAttributes(dpy, jfew->xjclient->wp, &to_attr);
	    /*
	    x_cood = to_attr.x;
	    y_cood = to_attr.y - height;
	    */
	    XTranslateCoordinates(dpy, jfew->xjclient->wp,
				  RootWindow(dpy, screen), to_attr.x, to_attr.y,
				  &root_x, &root_y, &child);
	    x_cood = root_x;
	    y_cood = root_y - height;
	    XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
		      &ins_save_x, &ins_save_y, &win_x, &win_y, &keys_buttons);
	} else {
	/*
	    XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&root_x, &root_y, &win_x, &win_y, &keys_buttons);
	    x_cood = root_x - width/2;
	    y_cood = root_y - FontHeight(xjc);
	*/
	    XGetWindowAttributes(dpy, cur_x->w, &to_attr);
	    /*
	    x_cood = to_attr.x;
	    y_cood = to_attr.y + to_attr.height;
	    */
	    XTranslateCoordinates(dpy, cur_x->w, RootWindow(dpy, screen),
				  to_attr.x, to_attr.y, &root_x, &root_y, &child);
	    x_cood = root_x;
	    y_cood = root_y + to_attr.height;
	    XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
		      &ins_save_x, &ins_save_y, &win_x, &win_y, &keys_buttons);
	}
    }
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;
    if (y_cood < 0)
	y_cood = 0;

    XWarpPointer(dpy, None, RootWindow(dpy, screen), 0, 0, 0, 0,
			 x_cood + width/2, y_cood + FontHeight(xjc));
    ins->msg = msg;
    ins->title = inspect_title;
    ins->cancel = ichiran_cancel_msg;
    ins->del_t = inspect_del_msg;
    ins->use_t = inspect_use_msg;
    ins->next = inspect_next_msg;
    ins->back = inspect_back_msg;

    XMoveResizeWindow(dpy, ins->w, x_cood, y_cood, width, height);

    hints.flags = USSize | USPosition;
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = width;
    hints.height = height;
    XSetNormalHints(dpy, ins->w, &hints);

    height = FontHeight(xjc) + IN_BORDER * 2;
    XResizeWindow(dpy, ins->w1, width, height);
    XRaiseWindow(dpy, ins->w);
    XMapWindow(dpy, ins->w);
    XFlush(dpy);
    redraw_ichi_box(ins->title_w, ins->title, 0);
    redraw_ichi_box(ins->cancel_w, ins->cancel, 0);
    redraw_ichi_box(ins->del_w, ins->del_t, 0);
    redraw_ichi_box(ins->use_w, ins->use_t, 0);
    redraw_ichi_box(ins->next_w, ins->next, 0);
    redraw_ichi_box(ins->back_w, ins->back, 0);
    JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
    XFlush(dpy);
    return(0);
}

end_inspect()
{
    Inspect *ins = jfew->inspect;

    XClearWindow(dpy, ins->cancel_w); 
    XClearWindow(dpy, ins->del_w);
    XClearWindow(dpy, ins->use_w);
    XClearWindow(dpy, ins->next_w);
    XClearWindow(dpy, ins->back_w);
    XClearWindow(dpy, ins->w1);
    XUnmapWindow(dpy, ins->w);
    if (ins_save_x > -1)
	XWarpPointer(dpy, None, RootWindow(dpy, screen), 0, 0, 0, 0,
		     ins_save_x, ins_save_y);
    XFlush(dpy);
    cancel_in = del_in = use_in = next_in = back_in = 0;
    select_return = -1;
    unlock_inspect();
    jfew->j_c = NULL;
}

xw_next_inspect()
{
    unsigned char *s;
    XJClient *xjc = jfew->jutilxjc;
    Inspect *ins = jfew->inspect;
    int len;
    extern unsigned char *next_inspect();
    extern unsigned char *previous_inspect();

    s = next_inspect();
    if ((len = strlen(s)) > inspect_len) {
	XResizeWindow(dpy, ins->w,
		FontWidth(xjc) * len, FontHeight(xjc) * 3 + IN_BORDER * 2);
	XResizeWindow(dpy, ins->w1,
		FontWidth(xjc) * len, FontHeight(xjc) + IN_BORDER * 2);
	inspect_len = len;
    }
    ins->msg = s;
    XClearWindow(dpy, ins->w1);
    JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
    XFlush(dpy);
    return(0);
}

xw_back_inspect()
{
    unsigned char *s;
    XJClient *xjc = jfew->jutilxjc;
    Inspect *ins = jfew->inspect;
    int len;

    s = previous_inspect();
    if ((len = strlen(s)) > inspect_len) {
	XResizeWindow(dpy, ins->w,
		FontWidth(xjc) * len, FontHeight(xjc) * 3 + IN_BORDER * 2);
	XResizeWindow(dpy, ins->w1,
		FontWidth(xjc) * len, FontHeight(xjc) + IN_BORDER * 2);
	inspect_len = len;
    }
    ins->msg = s;
    XClearWindow(dpy, ins->w1);
    JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
    XFlush(dpy);
    return(0);
}

struct jikouho_ichi {
	int	x_s;
	int	y_s;
	int	x_e;
	int	y_e;
	int	line;
} jikouho_ichi[MAXJIKOUHO + 1];

int jikouho_line[MAXJIKOUHO + 1];

xw_set_jikouho(info,kosuu)
unsigned char **info;
int kosuu;
{
    int ret;
    int	cnt, cntt;
    int	width;
    unsigned char *p;
    unsigned char tmp[256];
    int	columns = 0;
    XJClient *xjc;

    xjc = jfew->jutilxjc;
    width = DisplayWidth(dpy, screen)/FontWidth(xjc);
    jikouho_ichi[0].x_s = 0;
    jikouho_line[1] = 0;

    p = jfew->ichi->buf[0] = save_buf[0];
    *p = 0;
    for (cnt = 0, cntt = 0; cnt < kosuu; cnt++) {
	if (jfew->ichi->mode == JIKOUHO)
	    sprintf(tmp, "  %s  ", info[cnt]);
	else if (jfew->ichi->mode == JIS_IN)
	    sprintf(tmp, "%2x. %s ",
		cnt + ((kosuu < 16)? 1 : 33), info[cnt]);
	else if (jfew->ichi->mode == KUTEN_IN)
	    sprintf(tmp, "%2d. %s ",cnt + 1, info[cnt]);
	if ((columns + = strlen(tmp) ) > width) {
/*
	if (jfew->ichi->mode == JIKOUHO)
	    ret = sprintf(tmp, "  %s  ", info[cnt]);
	else if (jfew->ichi->mode == JIS_IN)
	    ret = sprintf(tmp, "%2x. %s ",
		cnt + ((kosuu < 16)? 1 : 33), info[cnt]);
	else if (jfew->ichi->mode == KUTEN_IN)
	    ret = sprintf(tmp, "%2d. %s ",cnt + 1, info[cnt]);
	if ((columns += ret) > width) {
*/
	    jikouho_line[++cntt + 1] = cnt;
	    jikouho_ichi[cnt--].x_s = 0;
	    columns = 0;
	    p = jfew->ichi->buf[cntt] = save_buf[cntt];
	    *p = 0;
	    continue;
	}
	jikouho_ichi[cnt + 1].x_s = columns * FontWidth(xjc);
	jikouho_ichi[cnt].x_e = columns * FontWidth(xjc) - 1;
	jikouho_ichi[cnt].y_s = cntt * (FontHeight(xjc) + IN_BORDER);
	jikouho_ichi[cnt].y_e = (cntt + 1) * (FontHeight(xjc)  + IN_BORDER) - 1;
	jikouho_ichi[cnt].line = cntt + 1;
	p = (unsigned char *)strcat(p, tmp);
    }
    jikouho_line[++cntt + 1] = kosuu;
    return(cntt);
}

serch_jikouho_table(x, y)
int x, y;
{
    int	tmp, gap, kosuu = jfew->ichi->kosuu;

    for (tmp = kosuu / 2, gap = kosuu / 4; (tmp >= 0 && tmp < kosuu);gap /= 2) {
	if (gap == 0) gap = 1;
	if (y > jikouho_ichi[tmp].y_e) {
	    tmp += gap;
	} else if (y < jikouho_ichi[tmp].y_s) {
	    tmp -= gap;
	} else {
	    for ( ; tmp >= 0 && tmp < kosuu; ) {
		if (x > jikouho_ichi[tmp].x_e) {
		    tmp++;
		} else if (x < jikouho_ichi[tmp].x_s) {
		    tmp--;
		} else if (y <= jikouho_ichi[tmp].y_e && y >=jikouho_ichi[tmp].y_s) {
		    return (tmp);
		}
	    }
	    return(-1);
	}
    }
    return(-1);
}

xw_jikouho_mousemove(event)
XMotionEvent *event;
{
    int item;

    if (jfew->j_c) {
	item = serch_jikouho_table(event->x, event->y);
	if (item != -1 && item != hilited_ji_item) {
	    if (hilited_ji_item != -1)
		xw_ji_fillrec();
	    if ((hilited_ji_item = item) != -1) {
		xw_ji_fillrec();
	    }
	}
    }
}
xw_jikouho_mouseleave()
{
XJClient *xjc;

/*
    if (jfew->j_c) {
	xjc = jfew->xjclient;

	if (hilited_ji_item != -1) {
		xw_ji_fillrec();
		hilited_ji_item = -1;
	}
    }
*/
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

    xjc = jfew->jutilxjc;

    if(rev) {
	tmpGC = xjc->reversegch;
	tmpGCK = xjc->reversegck;
	tmpGCR = xjc->reversegcr;
    } else {
	tmpGC = xjc->gch;
	tmpGCK = xjc->gck;
	tmpGCR = xjc->gcr;
    }

    for (x = s; x != 0 && *x != 0; x++) {
	if ((*x & 0x80) != 0 && *x != 0x8e) {
	    cc[0] =  *x++ & 0x7f;
	    cc[1] =  *x & 0x7f;
	    XDrawImageString16(dpy, w, tmpGCK, col * FontWidth(xjc) + in,
		li * (FontHeight(xjc) + in) + FontAscent(xjc) + in,
		(XChar2b *)cc, 1);
	    col += 2;
	} else if(*x == 0x8e) {
	    cc[0] = *++x & 0x7f;
	    XDrawImageString(dpy, w, tmpGCR, col * FontWidth(xjc) + in,
		li * (FontHeight(xjc) + in) + FontAscent(xjc) + in,
		cc, 1);
	    col++;
	} else {
	    cc[0] = *x & 0x7f;
	    XDrawImageString(dpy, w, tmpGC, col * FontWidth(xjc) + in,
		li * (FontHeight(xjc) + in) + FontAscent(xjc) + in,
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
    register XJClient *xjc = jfew->jutilxjc;

    XDrawImageString(dpy, w, xjc->reversegch, col * FontWidth(xjc),
			FontAscent(xjc), cursor_char, 1);
    XFlush(dpy);
}

JW3remark_cursor(w, col)
Window w;
int col;
{
    register XJClient *xjc = jfew->jutilxjc;

    XDrawImageString(dpy, w, xjc->gch, col * FontWidth(xjc),
			FontAscent(xjc), cursor_char, 1);
    XFlush(dpy);
}

redraw_ichi_w(event)
XEvent *event;
{
    Ichiran	 *ichi;
    int cnt, ret;
    int re_start_line, re_end_line;
    int write_line;
    int line = 0;
    register unsigned char *x;
    int rev = 0;

    ichi = jfew->ichi;

    re_start_line = 0;
    re_end_line = max_line - 1;

    XClearWindow(dpy, ichi->w1);
    for (line = re_start_line, write_line = (re_start_line + start_line);
     (line <= re_end_line && write_line < kosuu_all); line++, write_line++) {
	x = ichi->buf[write_line];
	if (line == hilited_item)
		rev = 1;
	else
		rev = 0;
	JW3Mputc(x, ichi->w1, line, rev, IN_BORDER);
    }
    if (jfew->ichi->mode == SENTAKU && hilited_item != -1) {
	xw_fillrec();
    } else if (jfew->ichi->mode != NYUURYOKU && hilited_ji_item != -1) {
	xw_ji_fillrec();
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

find_ji_hilite()
{
    return(hilited_ji_item);
}

xw_forward_select()
{
  if (jfew->ichi->mode == JIKOUHO || jfew->ichi->mode == JIS_IN
      || jfew->ichi->mode == KUTEN_IN) {
    if (hilited_ji_item != -1)
	xw_ji_fillrec();
    if(hilited_ji_item < jfew->ichi->kosuu - 1){
	hilited_ji_item += 1;
    } else {
	hilited_ji_item = 0;
    }
    xw_ji_fillrec();
  }
}

xw_backward_select()
{
  if (jfew->ichi->mode == JIKOUHO || jfew->ichi->mode == JIS_IN
      || jfew->ichi->mode == KUTEN_IN) {
    if (hilited_ji_item != -1)
	xw_ji_fillrec();
    if(hilited_ji_item > 0) {
	hilited_ji_item -= 1;
    } else {
	hilited_ji_item = jfew->ichi->kosuu - 1;
    }
    xw_ji_fillrec();
  }
}

xw_next_select()
{
    int next, next_l, next_l2;
    int cur_l = jikouho_ichi[hilited_ji_item].line;

  if (jfew->ichi->mode == JIKOUHO || jfew->ichi->mode == JIS_IN
      || jfew->ichi->mode == KUTEN_IN) {
    if (max_line == 1) 
	return(0);
    if (hilited_ji_item != -1)
	xw_ji_fillrec();
    if (cur_l > max_line - 1) {
	next_l = jikouho_line[1];
	next_l2 = jikouho_line[2];
    } else {
	next_l = jikouho_line[cur_l + 1];
	next_l2 = jikouho_line[cur_l + 2];
    }
    next = hilited_ji_item - jikouho_line[cur_l] + next_l;
    if (next >= next_l2) {
	hilited_ji_item = next_l2 - 1;
    } else {
	hilited_ji_item = next;
    }
    xw_ji_fillrec();
  }
}
xw_previous_select()
{
    int prev, prev_l, prev_l2;
    int cur_l = jikouho_ichi[hilited_ji_item].line;

  if (jfew->ichi->mode == JIKOUHO || jfew->ichi->mode == JIS_IN
      || jfew->ichi->mode == KUTEN_IN) {
    if (max_line == 1) 
	return(0);
    if (hilited_ji_item != -1)
	xw_ji_fillrec();
    if (cur_l == 1) {
	prev_l = jikouho_line[max_line];
	prev_l2 = jikouho_line[max_line + 1];
    } else {
	prev_l = jikouho_line[cur_l - 1];
	prev_l2 = jikouho_line[cur_l];
    }
    prev = hilited_ji_item - jikouho_line[cur_l] + prev_l;
    if (prev >= prev_l2) {
	hilited_ji_item = prev_l2 - 1;
    } else {
	hilited_ji_item = prev;
    }
    xw_ji_fillrec();
  }
}

xw_linestart_select()
{
  if (jfew->ichi->mode == JIKOUHO || jfew->ichi->mode == JIS_IN
      || jfew->ichi->mode == KUTEN_IN) {
    if (hilited_ji_item != -1)
	xw_ji_fillrec();
    hilited_ji_item = jikouho_line[jikouho_ichi[hilited_ji_item].line];
    xw_ji_fillrec();
  }
}

xw_lineend_select()
{
  if (jfew->ichi->mode == JIKOUHO || jfew->ichi->mode == JIS_IN
      || jfew->ichi->mode == KUTEN_IN) {
    if (hilited_ji_item != -1)
	xw_ji_fillrec();
    hilited_ji_item = jikouho_line[jikouho_ichi[hilited_ji_item].line + 1] - 1;
    xw_ji_fillrec();
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
    XJClient *xjc = jfew->jutilxjc;

    yes_no->title = str;

    def_width = FontWidth(xjc) * 18;
    height = FontHeight(xjc) * 4;

    if((width = FontWidth(xjc) * ((len = strlen(str)) + 2)) < def_width)
	width = def_width;
    XMoveResizeWindow(dpy, yes_no->title_w,
			FontWidth(xjc) * 1, FontHeight(xjc)/2 - IN_BORDER,
			FontWidth(xjc) * len + IN_BORDER * 2,
			FontHeight(xjc) + IN_BORDER * 2);
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
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&yes_save_x, &yes_save_y, &win_x, &win_y, &keys_buttons);
	x_cood = button_x - FontWidth(xjc) * 3;
	y_cood = button_y - FontHeight(xjc) * 3;
	select_button = 0;
    }else{
	XQueryPointer(dpy, RootWindow(dpy, screen), &root, &child,
			&yes_save_x, &yes_save_y, &win_x, &win_y, &keys_buttons);
	root_x = MESSAGE_X;
	root_y = MESSAGE_Y;
	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,root_x,root_y);
	x_cood = root_x - FontWidth(xjc) * 3;
	y_cood = root_y - FontHeight(xjc) * 3;
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
    	XWarpPointer(dpy, None, RootWindow(dpy, screen),0,0,0,0,yes_save_x,yes_save_y);
    XFlush(dpy);
    yes_w_in = 0;
    no_w_in = 0;
    select_return = -1;
}

xw_expose(event)
XExposeEvent *event;
{
    int t;
    XJClient *cur_x_sv, *cur_p_sv;
    Ichiran *ichi;
    Inspect *ins;
    YesOrNo *yes_no;

    cur_x_sv = cur_x;
    cur_p_sv = cur_p;
    for (t = 0; t < MAX_CLIENTS; t++) {
      if(client_mem[t] != NULL) {
	cur_x = client_mem[t];
	if (cur_x->dispmode != 2)
	    cur_p = cur_x;
	else
	    cur_p = jfew->xjclient;

	if (cur_p->w1 == event->window) {
	    if (cur_x->dispmode == 0)
		redraw_window3(event->x, event->width);
	    else
		redraw_line1(event->x, event->width);
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	} else if (cur_p->w2 == event->window) {
	    redraw_line2(event->x, event->width);
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	} else if (cur_p->w3 == event->window) {
	    redraw_line3(event->x, event->width);
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	} else if (cur_p->wnf == event->window) {
	    redraw_note_fore();
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	} else if (cur_p->wnb == event->window) {
	    redraw_note_back();
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	} else if (cur_p->ws == event->window) {
	    redraw_window0();
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	} else if (cur_p->wt == event->window) {
	    redraw_windowt(cur_p);
	    cur_p = cur_p_sv;
	    cur_x = cur_x_sv;
	    return(0);
	}
      }
    }
    cur_p = cur_p_sv;
    cur_x = cur_x_sv;
    ichi = jfew->ichi;
    if (ichi->w1 == event->window) {
	redraw_ichi_w(event);
	return(0);
    } else if (ichi->title_w == event->window) {
	redraw_ichi_box(ichi->title_w, ichi->title, 0);
	return(0);
    } else if (ichi->cancel_w == event->window) {
	redraw_ichi_box(ichi->cancel_w, ichi->cancel, 0);
	return(0);
    } else if (ichi->mouse_w == event->window) {
	redraw_ichi_box(ichi->mouse_w, ichi->mouse, 0);
	return(0);
    } else if (ichi->subtitle_w == event->window) {
	redraw_ichi_box(ichi->subtitle_w, ichi->subtitle, 0);
	return(0);
    } else if (ichi->exec_title_w == event->window) {
	redraw_ichi_box(ichi->exec_title_w, ichi->exec_title, 0);
	return(0);
    } else if (ichi->next_w == event->window) {
	redraw_ichi_box(ichi->next_w, ichi->next, 0);
	return(0);
    } else if (ichi->back_w == event->window) {
	redraw_ichi_box(ichi->back_w, ichi->back, 0);
	return(0);
    }
    yes_no = jfew->yes_no;
    if (yes_no->title_w == event->window) {
	redraw_ichi_box(yes_no->title_w, yes_no->title, 0);
	return(0);
    } else if (yes_no->yes_w == event->window) {
	redraw_ichi_box(yes_no->yes_w, yes_no->yes_t, 0);
	return(0);
    } else if (yes_no->no_w == event->window) {
	redraw_ichi_box(yes_no->no_w, yes_no->no_t, 0);
	return(0);
    }
    ins = jfew->inspect;
    if (ins->w1 == event->window) {
	JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
	return(0);
    } else if (ins->title_w == event->window) {
	redraw_ichi_box(ins->title_w, ins->title, 0);
	return(0);
    } else if (ins->cancel_w == event->window) {
	redraw_ichi_box(ins->cancel_w, ins->cancel, 0);
	return(0);
    } else if (ins->del_w == event->window) {
	redraw_ichi_box(ins->del_w, ins->del_t, 0);
	return(0);
    } else if (ins->use_w == event->window) {
	redraw_ichi_box(ins->use_w, ins->use_t, 0);
	return(0);
    } else if (ins->next_w == event->window) {
	redraw_ichi_box(ins->next_w, ins->next, 0);
	return(0);
    } else if (ins->back_w == event->window) {
	redraw_ichi_box(ins->back_w, ins->back, 0);
	return(0);
    }
}

xw_buttonpress(event)
XButtonEvent *event;
{
    Ichiran *ichi = jfew->ichi;
    Inspect *ins = jfew->inspect;
    int i;

    select_return = -1;
    if (jfew->j_c == NULL) {
	    for(i = 0; i < MAX_CLIENTS; i++) {
		if (client_mem[i] != 0 && event->window == client_mem[i]->ws) {
		    select_button = 1;
		    cur_cl_change3(client_mem[i]);
		    jutil_c();
		    return(-1);
	    }
	}
    }
    if (event->window == ichi->w1 && !lock_flg) {
	select_button = 1;
	if (ichi->mode == SENTAKU)
	    xw_select_button(event);
	else
	    xw_select_jikouho_button(event);
	cur_cl_change3(jfew->j_c);
	return(0);
    } else if (event->window == jfew->yes_no->yes_w) {
	select_return = 1;
	return(0);
    } else if (event->window == jfew->yes_no->no_w) {
	select_return = 0;
	return(0);
    } else if (event->window == ichi->cancel_w && !lock_flg) {
	select_return = -2;
	cur_cl_change3(jfew->j_c);
	return(0);
    } else if (event->window == ichi->exec_title_w && !lock_flg) {
	select_button = 1;
	cur_cl_change3(jfew->j_c);
	return(0);
    } else if (event->window == ichi->next_w && !lock_flg) {
	next_ichi();
	return(0);
    } else if (event->window == ichi->back_w && !lock_flg) {
	back_ichi();
	return(0);
    } else if (event->window == ichi->mouse_w && !lock_flg) {
	xw_mouse_select();
	return(-1);
    } else if (event->window == ins->cancel_w && !lock_flg) {
	select_return = -2;
	cur_cl_change3(jfew->j_c);
	return(0);
    } else if (event->window == ins->del_w && !lock_flg) {
	select_button = 1;
	cur_cl_change3(jfew->j_c);
	select_return = 0;
	return(0);
    } else if (event->window == ins->use_w && !lock_flg) {
	select_button = 1;
	cur_cl_change3(jfew->j_c);
	select_return = 1;
	return(0);
    } else if (event->window == ins->next_w && !lock_flg) {
	xw_next_inspect();
	return(-1);
    } else if (event->window == ins->back_w && !lock_flg) {
	xw_back_inspect();
	return(-1);
    }
    return(-1);
}

cur_cl_change3(xjc)
XJClient *xjc;
{
    if (cur_p == 0 || xjc == 0) {
	cur_p = jfew->xjclient;
	c_c = cur_p->w_c;
    } else {
	if (xjc->dispmode == 2) {
	    cur_p = jfew->xjclient;
	    c_c = jfew->xjclient->w_c;
	} else {
	    cur_p = xjc;
	    c_c = xjc->w_c;
	}
    }
    cur_rk = c_c->rk;
    cur_input = 0;
}

xw_fillrec()
{
    XJClient *xjc = jfew->jutilxjc;

    if (hilited_item != -1) {
	XFillRectangle(dpy, jfew->ichi->w1, jfew->ichi->invertgc,
		0, hilited_item * (FontHeight(xjc) + IN_BORDER),
		item_width, (FontHeight(xjc) + IN_BORDER));
	XFlush(dpy);
    }
}

xw_ji_fillrec()
{
    XJClient *xjc = jfew->jutilxjc;

    if (hilited_ji_item != -1) {
	XFillRectangle(dpy, jfew->ichi->w1, jfew->ichi->invertgc,
	jikouho_ichi[hilited_ji_item].x_s, jikouho_ichi[hilited_ji_item].y_s,
	jikouho_ichi[hilited_ji_item].x_e - jikouho_ichi[hilited_ji_item].x_s,
	jikouho_ichi[hilited_ji_item].y_e - jikouho_ichi[hilited_ji_item].y_s);
	XFlush(dpy);
    }
}

xw_select_button(event)
XButtonEvent *event;
{
    static int y;
    int item;

    if (jfew->j_c) {
	y = event->y;
	if ((item = Y_InItem(y)) != hilited_item) {
	    if (hilited_item != -1)
		xw_fillrec();
	    if ((hilited_item = item) != -1) {
		xw_fillrec();
	    }
	}
	XFlush(dpy);
	select_return = item;
    }
}
xw_select_jikouho_button(event)
XButtonEvent *event;
{
    int item;

    if (jfew->j_c) {
	if ((item=serch_jikouho_table(event->x,event->y)) != hilited_ji_item) {
	    if (hilited_ji_item != -1)
		xw_ji_fillrec();
	    if ((hilited_ji_item = item) != -1) {
		xw_ji_fillrec();
	    }
	}
	XFlush(dpy);
	select_return = item;
    }
}

xw_enterleave(event, el)
XEvent *event;
int el;
{
    int cnt;
    Ichiran *ichi = jfew->ichi;
    Inspect *ins = jfew->inspect;

    if (event->xcrossing.window == jfew->yes_no->yes_w) {
	if (el != yes_w_in) {
	    reverse_ichiran(jfew->yes_no->yes_w,jfew->yes_no->yes_t, IN_BORDER);
	    yes_w_in = el;
	}
	return(-1);
    } else if (event->xcrossing.window == jfew->yes_no->no_w) {
	if (el != no_w_in) {
	    reverse_ichiran(jfew->yes_no->no_w,
			jfew->yes_no->no_t, IN_BORDER);
	    no_w_in = el;
	}
	return(-1);
    }
    if (event->xcrossing.window == ichi->w1 && !lock_flg) {
	if (el) {
	    in_window = 1;
	    if (ichi->mode == SENTAKU)
		xw_mousemove(event);
	    else
		xw_jikouho_mousemove(event);
	} else {
	    in_window = 0;
	    if (ichi->mode == SENTAKU)
		xw_mouseleave();
	    else
		xw_jikouho_mouseleave();
	}
    } else if (event->xcrossing.window == ichi->cancel_w) {
	if (el != cancel_in) {
	    reverse_ichiran(ichi->cancel_w, ichi->cancel, IN_BORDER);
	    cancel_in = el;
	}
    } else if (event->xcrossing.window == ichi->mouse_w && !lock_flg) {
	if (el != mouse_in) {
	    reverse_ichiran(ichi->mouse_w, ichi->mouse, IN_BORDER);
	    mouse_in = el;
	}
    } else if (event->xcrossing.window == ichi->exec_title_w && !lock_flg){
	if (el != exec_in) {
	    reverse_ichiran(ichi->exec_title_w, ichi->exec_title, IN_BORDER);
	    exec_in = el;
	}
    } else if (event->xcrossing.window == ichi->next_w && !lock_flg){
	if (el != next_in) {
	    reverse_ichiran(ichi->next_w, ichi->next, IN_BORDER);
	    next_in = el;
	}
    } else if (event->xcrossing.window == ichi->back_w && !lock_flg){
	if (el != back_in) {
	    reverse_ichiran(ichi->back_w, ichi->back, IN_BORDER);
	    back_in = el;
	}
    } else if (event->xcrossing.window == ins->cancel_w && !lock_flg) {
	if (el != cancel_in) {
	    reverse_ichiran(ins->cancel_w, ins->cancel, IN_BORDER);
	    cancel_in = el;
	}
    } else if (event->xcrossing.window == ins->del_w && !lock_flg) {
	if (el != del_in) {
	    reverse_ichiran(ins->del_w, ins->del_t, IN_BORDER);
	    del_in = el;
	}
    } else if (event->xcrossing.window == ins->use_w && !lock_flg) {
	if (el != use_in) {
	    reverse_ichiran(ins->use_w,
				ins->use_t, IN_BORDER);
	    use_in = el;
	}
    } else if (event->xcrossing.window == ins->next_w && !lock_flg) {
	if (el != next_in) {
	    reverse_ichiran(ins->next_w, ins->next, IN_BORDER);
	    next_in = el;
	}
    } else if (event->xcrossing.window == ins->back_w && !lock_flg) {
	if (el != back_in) {
	    reverse_ichiran(ins->back_w, ins->back, IN_BORDER);
	    back_in = el;
	}
    }
    return(-1);
}

xw_mousemove(event)
XMotionEvent *event;
{
    static int y;
    int item;

    if (jfew->j_c) {
	if (jfew->ichi->mode == SENTAKU) {
	    y = event->y;
	    item = Y_InItem(y);
	    if (item != -1 && item != hilited_item) {
		if (hilited_item != -1)
		    xw_fillrec();
		if ((hilited_item = item) != -1) {
		    xw_fillrec();
		}
	    }
	} else {
	    xw_jikouho_mousemove(event);
	}
    }
}

xw_mouseleave()
{
    if (jfew->j_c && jfew->ichi->kosuu == 1) {
	if (hilited_item != -1) {
	    xw_fillrec();
	    hilited_item = -1;
	}
    }
}

Y_InItem(y)
int y;
{
    XJClient *xjc = jfew->jutilxjc;

    if (y < 0 || y > item_height)
	return(-1);
    return((y/(FontHeight(xjc) + IN_BORDER)));
}

xw_mouse_select()
{
    if (mouse == 0) {
	XSelectInput(dpy, jfew->ichi->w1,EnterWindowMask | LeaveWindowMask
		| PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask);
	XDefineCursor(dpy, jfew->ichi->w1, XCreateFontCursor(dpy,52));
	jfew->ichi->mouse = ichiran_mouse_msg;
	mouse = 1;
    } else {
	XSelectInput(dpy, jfew->ichi->w1,  ExposureMask);
	XUndefineCursor(dpy, jfew->ichi->w1);
	jfew->ichi->mouse = ichiran_key_msg;
	mouse = 0;
    }
    JW3Mputc(jfew->ichi->mouse, jfew->ichi->mouse_w, 0, 1, IN_BORDER);
    XFlush(dpy);
}

change_border(win, s, xjc, f)
Window win;
char *s;
XJClient *xjc;
int f;
{
    int width = strlen(s) * FontWidth(xjc) + IN_BORDER * 2;
    int height = FontHeight(xjc) + IN_BORDER * 2;

    if (f) {
	XFillRectangle(dpy, win, jfew->ichi->invertgc, 0, 0, width, height);
    } else {
	XFillRectangle(dpy, win, jfew->ichi->invertgc, 0, 0, width, height);
    }
    JW3Mputc(s, win, 0, 0, IN_BORDER);
    XFlush(dpy);
}

reverse_ichiran(win, msg, in)
Window	win;
unsigned char	*msg;
int		in;
{
	unsigned char	*x;
	int		column = 0;
	int		ret;
	XJClient	*xjc = jfew->jutilxjc;

	XFillRectangle(dpy, win, jfew->ichi->invertgc,
			0, 0, strlen(msg) * FontWidth(xjc) + in * 2,
			FontHeight(xjc) + in * 2);
}

int euclen(s)
char *s;
{
    register int n;
    register char *c;

    for (c = s, n = 0; *c != 0; c++,  n++) {
	if ((*c & 0xff) == 0x8e) c++;
    }
    return n;
}
