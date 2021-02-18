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

int	two_byte_first0 = 0;
unsigned char	save_char0;
unsigned char	cursor_char[2] = " ";

/*****************************************************************************
	JW1 への出力ルーチン
			from JWMflushw_buf()
*****************************************************************************/
JW1Mputc(x)
register unsigned char	x;
{
    unsigned short	cdata = 0;
    unsigned char		cc[2];
    unsigned char		cc1[2];
    register XJClient	*xjc;
    GC			currentGC;

    xjc = cur_jutil->cl;
    cur_JW_buf = cur_jutil->cl->buf;
    visual_window();
    if (xjc->currentcol >= xjc->maxcolumns)
	return(-1);

    if ((x & 0x80) != 0) {
	if (two_byte_first0 == 1) {
	    if ((save_char0 & 0xff) == 0x8e) {
		cc[0] = (x & 0x7f);
		*(cur_JW_buf + xjc->currentcol) = cc[0];
		if (xjc->r_flag == 0) {
			currentGC = jfew->gcr;
		} else {
			currentGC = jfew->reversegcr;
			*(cur_JW_buf + xjc->currentcol) |= REV;
			check_move(xjc, 0);
		}
		XDrawImageString(dpy, xjc->w1, currentGC,
			xjc->currentcol * FontWidth,
			FontAscent, cc, 1);
		*(cur_JW_buf + xjc->currentcol) |= KANA;
		if (xjc->b_flag == 1) {
		    XDrawString(dpy, xjc->w1, currentGC,
			xjc->currentcol * FontWidth + 1,
			FontAscent, cc, 1);
		    *(cur_JW_buf + xjc->currentcol) |= BOLD;
		}
		if (xjc->u_line_flag == 1) {
		    XDrawLine(dpy, xjc->w1, jfew->gch,
			xjc->currentcol * FontWidth, FontAscent + 1,
			(xjc->currentcol + 1) * FontWidth,
			FontAscent + 1);
		    *(cur_JW_buf + xjc->currentcol) |= UNDER;
		}
		two_byte_first0 = 0;
		save_char0 = NULL;
		xjc->currentcol += 1;
	    } else {
		cc[0] =  save_char0 & 0x7f;
		cc[1] =  x & 0x7f;
		*(cur_JW_buf + xjc->currentcol) = cc[0];
		*(cur_JW_buf + xjc->currentcol + 1) = cc[1];
		*(cur_JW_buf + xjc->currentcol) |= KAN1;
		*(cur_JW_buf + xjc->currentcol + 1) |= KAN2;
		if (xjc->r_flag == 0) {
			currentGC = jfew->gck;
		} else {
			currentGC = jfew->reversegck;
			*(cur_JW_buf + xjc->currentcol) |= REV;
			*(cur_JW_buf + xjc->currentcol + 1) |= REV;
			check_move(xjc, 1);
		}
		XDrawImageString16(dpy, xjc->w1, currentGC,
			xjc->currentcol * FontWidth,
			FontAscent, (XChar2b *)cc, 1);
		if (xjc->b_flag == 1) {
		    XDrawString16(dpy, xjc->w1, currentGC,
			xjc->currentcol * FontWidth + 1,
			FontAscent, (XChar2b *)cc, 1);
		    *(cur_JW_buf + xjc->currentcol) |= BOLD;
		    *(cur_JW_buf + xjc->currentcol + 1) |= BOLD;
		}
		if (xjc->u_line_flag == 1) {
		    XDrawLine(dpy, xjc->w1, jfew->gch,
			xjc->currentcol * FontWidth,
			FontAscent + 1,
			(xjc->currentcol + 2) * FontWidth,
			FontAscent + 1);
		    *(cur_JW_buf + xjc->currentcol) |= UNDER;
		    *(cur_JW_buf + xjc->currentcol + 1) |= UNDER;
		}
		two_byte_first0 = 0;
		save_char0 = NULL;
		xjc->currentcol += 2;
	    }
	} else {
	    save_char0 = x;
	    two_byte_first0 = 1;
	}
    } else {
	if (x < 0x20)
	    return(0);
	cc[0] = x;
	*(cur_JW_buf + xjc->currentcol) = cc[0];
	if (xjc->r_flag == 0) {
	    currentGC = jfew->gch;
	} else {
	    currentGC = jfew->reversegch;
	    *(cur_JW_buf + xjc->currentcol) |= REV;
	    check_move(xjc, 0);
	}
	XDrawImageString(dpy, xjc->w1, currentGC,
		xjc->currentcol * FontWidth,
		FontAscent, cc, 1);
	if (xjc->b_flag == 1) {
	    XDrawString(dpy, xjc->w1, currentGC,
		xjc->currentcol * FontWidth + 1,
		FontAscent, cc, 1);
	    *(cur_JW_buf + xjc->currentcol) |= BOLD;
	}
	if (xjc->u_line_flag == 1) {
	    XDrawLine(dpy, xjc->w1, jfew->gch, xjc->currentcol * FontWidth,
		FontAscent + 1, (xjc->currentcol + 1) * FontWidth,
		FontAscent + 1);
	    *(cur_JW_buf + xjc->currentcol) |= UNDER;
	}
	xjc->currentcol += 1;
    }   
    return(1);
}

/*****************************************************************************
	文字表示 ルーチン
			from putchar(), flushw_buf0()
*****************************************************************************/
JWMflushw_buf(w_buf, len)
wchar_t	*w_buf;
int	len;
{
    register wchar_t	*x;
    register XJClient	*xjc;

    xjc = cur_jutil->cl;

    for(x = w_buf; len > 0; len--, x++) {
	if(NORMAL_CHAR(*x)) {
	    JW1Mputc((char)(*x));
	} else {
	    JW1Mputc((char)((*x >> 8) & 0xff));
	    JW1Mputc((char)(*x & 0xff));
	}
    }
    if (xjc->currentcol > xjc->max_pos) {
	xjc->max_pos = xjc->currentcol;
    }
    XFlush(dpy);
}


jw_disp_mode(mode)
unsigned char *mode;
{
    if (jfew->mode & JUTIL) {
	JW3Mputc(mode, jfew->jutil->rk_mode_w, 0, 0, IN_BORDER);
    }
    if ((jfew->mode & ICHIRAN) && (jfew->mode & NYUURYOKU)) {
	JW3Mputc(mode, jfew->ichi->rk_mode_w, 0, 0, IN_BORDER);
    }
    return;
}

JWMline_clear(x)
register int	x;
{
    register XJClient	*xjc;
    register int	cnt;

    xjc = cur_jutil->cl;
    cur_JW_buf = cur_jutil->cl->buf;
    for(cnt = x; cnt <= xjc->max_pos; cnt++) {
	*(cur_JW_buf + cnt) = 0;
    }
    XClearArea(dpy, xjc->w1, x * FontWidth, 0,
	    cnt * FontWidth, 0, False);
    if(x <= (xjc->vst + xjc->max_columns)) {
	JWM_note_delete_back();
	xjc->note_back = 0;
	XUnmapWindow(dpy, xjc->wnn);
    }
    xjc->currentcol = x;
    xjc->max_pos = x;
    XFlush(dpy);
}

/*****************************************************************************
	変換行表示 ルーチン
			from xw_read(), JWM1putc(), JWMflushw_buf(),
			     JWcursor_move()
*****************************************************************************/
visual_window()
{
    XMapWindow(dpy, cur_jutil->cl->wp1);
    XMapWindow(dpy, cur_jutil->cl->w1);
    XFlush(dpy);
}

/*****************************************************************************
	入力行の前半が欠けている印フラグセット ルーチン
			from t_cont_line_note_delete()
*****************************************************************************/
JWM_note_delete_fore()
{
    invisual_note_fore();
}
JWM_note_delete_back()
{
    invisual_note_back();
}

/*****************************************************************************
	入力行の前半が欠けている印表示 ルーチン
			from t_cont_line_note(), visual_window3()
*****************************************************************************/
JWM_note_fore()
{
    register XJClient	*xjc;
    unsigned char	cdata[3];

    cdata[0] = 0x24;
    xjc = cur_jutil->cl;

    if (!xjc->note_fore) {
	XMapWindow(dpy, xjc->wnf);
	XDrawImageString(dpy, xjc->wnf, jfew->gch, 0, FontAscent, cdata, 1);
	XFlush(dpy);
    }
}

JWM_note_back()
{
    register XJClient	*xjc;
    unsigned char	cdata[3];

    cdata[0] = 0x24;
    xjc = cur_jutil->cl;

    if (!xjc->note_back) {
	XMapWindow(dpy, xjc->wnb);
	XDrawImageString(dpy, xjc->wnb, jfew->gch, 0, FontAscent, cdata, 1);
	XFlush(dpy);
    }
}

/*****************************************************************************
	アンダーラインフラグセット ルーチン
			from u_s_on_raw()
*****************************************************************************/
JWMset_under()
{
    cur_jutil->cl->u_line_flag = 1;
}

/*****************************************************************************
	アンダーラインフラグリセット ルーチン
			from u_s_off_raw()
*****************************************************************************/
JWMreset_under()
{
    cur_jutil->cl->u_line_flag = 0;
}

/*****************************************************************************
	リバースフラグセット ルーチン
			from h_r_on_raw()
*****************************************************************************/
JWMset_reverse()
{
    cur_jutil->cl->r_flag = 1;
}

/*****************************************************************************
	リバースフラグリセット ルーチン
			from h_r_off_raw()
*****************************************************************************/
JWMreset_reverse()
{
    cur_jutil->cl->r_flag = 0;
}

/*****************************************************************************
	Boldフラグリセット ルーチン
			from b_r_off_raw()
*****************************************************************************/
JWMset_bold()
{
    cur_jutil->cl->b_flag = 1;
}

/*****************************************************************************
	Boldフラグリセット ルーチン
			from b_r_off_raw()
*****************************************************************************/
JWMreset_bold()
{
    cur_jutil->cl->b_flag = 0;
}


/*****************************************************************************
	カーソル表示 ルーチン
			from JWMflushw_buf(), JWMline_clear(), JWcursor_visible()
			     JWcursor_move(),
*****************************************************************************/
JWmark_cursor()
{
    register XJClient	*xjc;
    unsigned char	s_c[2];
    Window		currentW;

    xjc = cur_jutil->cl;
    cur_JW_buf = cur_jutil->cl->buf;
    visual_window();
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 0) {
	    if ((*(cur_JW_buf + xjc->currentcol) & REV) == 0) {
		if ((*(cur_JW_buf + xjc->currentcol) & KAN1) != 0){
		    check_move(xjc, 1);
		    s_c[0] = (char)(*(cur_JW_buf + xjc->currentcol) & 0xff);
		    s_c[1] = (char)(*(cur_JW_buf + xjc->currentcol + 1) & 0xff);
		    XDrawImageString16(dpy, xjc->w1, jfew->reversegck,
			xjc->currentcol * FontWidth, FontAscent,
			(XChar2b *)s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & BOLD) != 0)
			XDrawString16(dpy, xjc->w1, jfew->reversegck,
			    xjc->currentcol * FontWidth + 1,
			    FontAscent, (XChar2b *)s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, jfew->reversegch,
			    xjc->currentcol * FontWidth, FontAscent+1,
			    (xjc->currentcol + 2) * FontWidth,
			    FontAscent + 1);
		} else if ((*(cur_JW_buf + xjc->currentcol) & KANA) != 0) {
		    check_move(xjc, 0);
		    s_c[0] = (char)(*(cur_JW_buf + xjc->currentcol) & 0xff);
		    XDrawImageString(dpy, xjc->w1, jfew->reversegcr,
			xjc->currentcol * FontWidth, FontAscent,
			s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & BOLD) != 0)
			XDrawString(dpy, xjc->w1, jfew->reversegcr,
			    xjc->currentcol * FontWidth + 1,
			    FontAscent, s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & UNDER) != 0)
		  	XDrawLine(dpy, xjc->w1, jfew->reversegcr,
			    xjc->currentcol * FontWidth, FontAscent+1,
			    (xjc->currentcol + 1) * FontWidth,
			    FontAscent + 1);
		} else if (*(cur_JW_buf + xjc->currentcol) != 0){
		    check_move(xjc, 0);
		    s_c[0] = (char)(*(cur_JW_buf + xjc->currentcol) & 0xff);
		    XDrawImageString(dpy, xjc->w1, jfew->reversegch,
			xjc->currentcol * FontWidth, FontAscent,
			s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & BOLD) != 0)
			XDrawString(dpy, xjc->w1, jfew->reversegch,
			    xjc->currentcol * FontWidth + 1,
			    FontAscent, s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & UNDER) != 0)
		  	XDrawLine(dpy, xjc->w1, jfew->reversegch,
			    xjc->currentcol * FontWidth, FontAscent+1,
			    (xjc->currentcol + 1) * FontWidth,
			    FontAscent + 1);
		} else {
		    check_move(xjc, 0);
		    XDrawImageString(dpy, xjc->w1, jfew->reversegch,
			xjc->currentcol * FontWidth, FontAscent,
			cursor_char, 1);
		}
	    }
	xjc->mark_flag = 1;
	XFlush(dpy);
    }
}


/*****************************************************************************
	カーソル非表示 ルーチン
			from JWMflushw_buf(), JWMline_clear(), JWcursor_invisible()
			     JWcursor_move()
*****************************************************************************/
JWremark_cursor()
{
    register XJClient	*xjc;
    unsigned char	s_c[2];
    Window		currentW;

    xjc = cur_jutil->cl;
    cur_JW_buf = cur_jutil->cl->buf;
    visual_window();
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 1) {
	    if ((*(cur_JW_buf + xjc->currentcol) & REV) == 0) {
		if ((*(cur_JW_buf + xjc->currentcol) & KAN1) != 0) {
		    s_c[0] = (char)(*(cur_JW_buf + xjc->currentcol) & 0xff);
		    s_c[1] = (char)(*(cur_JW_buf + xjc->currentcol + 1) & 0xff);
		    XDrawImageString16(dpy, xjc->w1, jfew->gck,
			xjc->currentcol * FontWidth, FontAscent,
			(XChar2b *)s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & BOLD) != 0)
			XDrawString16(dpy, xjc->w1, jfew->gck,
			    xjc->currentcol * FontWidth + 1,
			    FontAscent, (XChar2b *)s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, jfew->gch,
			    xjc->currentcol * FontWidth, FontAscent+1,
			    (xjc->currentcol + 2) * FontWidth,
			    FontAscent + 1);
		} else if ((*(cur_JW_buf + xjc->currentcol)  & KANA) != 0) {
		    s_c[0] = (char)(*(cur_JW_buf + xjc->currentcol) & 0xff);
		    XDrawImageString(dpy, xjc->w1, jfew->gcr,
			xjc->currentcol * FontWidth, FontAscent,
			s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & BOLD) != 0)
			XDrawString(dpy, xjc->w1, jfew->gcr,
			    xjc->currentcol * FontWidth + 1,
			    FontAscent, s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, jfew->gcr,
			    xjc->currentcol * FontWidth, FontAscent+1,
			    (xjc->currentcol + 1) * FontWidth,
			    FontAscent + 1);
		} else if (*(cur_JW_buf + xjc->currentcol) != 0){
		    s_c[0] = (char)(*(cur_JW_buf + xjc->currentcol) & 0xff);
		    XDrawImageString(dpy, xjc->w1, jfew->gch,
			xjc->currentcol * FontWidth, FontAscent,
			s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & BOLD) != 0)
			XDrawString(dpy, xjc->w1, jfew->gch,
			    xjc->currentcol * FontWidth + 1,
			    FontAscent, s_c, 1);
		    if ((*(cur_JW_buf + xjc->currentcol) & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, jfew->gch,
			    xjc->currentcol * FontWidth, FontAscent+1,
			    (xjc->currentcol + 1) * FontWidth,
			    FontAscent + 1);
		} else {
		    XDrawImageString(dpy, xjc->w1, jfew->gch,
			xjc->currentcol * FontWidth, FontAscent,
			cursor_char, 1);
		}
	    }
	xjc->mark_flag = 0;
	XFlush(dpy);
    }
}

/*****************************************************************************
	カーソル表示フラグセット ルーチン
			from cursor_normal_raw()
*****************************************************************************/
JWcursor_visible()
{
    cur_jutil->cl->cursor_flag = 1;
    JWmark_cursor();
}

/*****************************************************************************
	カーソル非表示フラグセット ルーチン
			from cursor_invisible_raw()
*****************************************************************************/
JWcursor_invisible()
{
    JWremark_cursor();
    cur_jutil->cl->cursor_flag = 0;
}

/*****************************************************************************
	カーソル移動 ルーチン
			from throw_cur_raw()
*****************************************************************************/
JWcursor_move(x)
{
    register XJClient *xjc;

    xjc = cur_jutil->cl;
    visual_window();
    JWremark_cursor();
    xjc->currentcol = x;
    if (x >= 0)
	JWmark_cursor();
}

/*****************************************************************************
	入力行の前半が欠けている印非表示 ルーチン
			from JWM_note_delete()
*****************************************************************************/
invisual_note_fore()
{
    if (cur_jutil->cl->note_fore) {
	XUnmapWindow(dpy, cur_jutil->cl->wnf);
	XFlush(dpy);
    }
}

invisual_note_back()
{
    if (cur_jutil->cl->note_back) {
	XUnmapWindow(dpy, cur_jutil->cl->wnb);
	XFlush(dpy);
    }
}

/*****************************************************************************
	入力行の前半が欠けている印書き直し ルーチン
			from xw_read()
*****************************************************************************/
redraw_note_fore(ju)
Jutil	*ju;
{
    register XJClient	*xjc;
    unsigned char	cdata[3];

    cdata[0] = 0x24;
    xjc = cur_jutil->cl;
    XDrawImageString(dpy, xjc->wnf, jfew->gch, 0, FontAscent, cdata, 1);
    XFlush(dpy);
}
redraw_note_back(ju)
Jutil	*ju;
{
    register XJClient	*xjc;
    unsigned char	cdata[3];

    cdata[0] = 0x24;
    xjc = cur_jutil->cl;
    XDrawImageString(dpy, xjc->wnb, jfew->gch, 0, FontAscent, cdata, 1);
    XFlush(dpy);
}
