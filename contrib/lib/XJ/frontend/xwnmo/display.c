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

extern char	xwnmo_title[80];

unsigned char	cursor_char[2] = " ";
unsigned char	dol_char[2] = "$";

/*****************************************************************************
	JW0 への出力ルーチン
			from JWMflushw_buf(), in_jis(), in_kuten()
*****************************************************************************/
JW0Mputwc(wx, len)
w_char *wx;
int len;
{
    unsigned char cc[2];
    register int cnt = len;
    register unsigned short *x = wx;
    register XJClient *xjc;
    register int *JW_buf;
    GC currentGC;

    xjc = cur_p;

    if (xjc->currentcol0 >= xjc->columns)
	return(-1);

    for (; cnt > 0; cnt--, x++) {
        JW_buf = xjc->buf0 + xjc->currentcol0;
	if ((*x & 0x8000) != 0 && (*x & 0xff00) != 0x8e00) {
	    *JW_buf = (cc[0]=((*x & 0x7f00)>>8)) | KAN1;
	    *(JW_buf + 1) = (cc[1] = (*x & 0x7f)) | KAN2;
	    if (xjc->r_flag == 0) {
			currentGC = xjc->gck;
	    } else {
			currentGC = xjc->reversegck;
			*JW_buf |= REV;
			*(JW_buf + 1) |= REV;
			check_move(xjc, 1);
	    }
	    XDrawImageString16(dpy, xjc->ws, currentGC,
			xjc->currentcol0 * FontWidth(xjc),
			FontAscent(xjc), (XChar2b *)cc, 1);
	    if (xjc->b_flag == 1) {
		    XDrawString16(dpy, xjc->ws, currentGC,
			xjc->currentcol0 * FontWidth(xjc) + 1,
			FontAscent(xjc), (XChar2b *)cc, 1);
		    *JW_buf |= BOLD;
		    *(JW_buf + 1) |= BOLD;
	    }
		if (xjc->u_line_flag == 1) {
		    XDrawLine(dpy, xjc->ws, xjc->gch,
			xjc->currentcol0 * FontWidth(xjc),
			FontAscent(xjc) + 1,
			(xjc->currentcol0 + 2) * FontWidth(xjc),
			FontAscent(xjc) + 1);
		    *JW_buf |= UNDER;
		    *(JW_buf + 1) |= UNDER;
		}
		if ((xjc->currentcol0 - xjc->vst + 1) >= xjc->max_cur) {
		    xjc->max_cur = xjc->currentcol0 + 2 - xjc->vst;
		}
		xjc->currentcol0 += 2;
	} else {
	    if (*x < 0x20)
		return(0);
	    *JW_buf = cc[0] = *x & 0x7f;
	    if ((*JW_buf & KAN1) != 0)
		*(JW_buf + 1) = 0;
	    if (xjc->r_flag == 0) {
		if ((*x & 0xff00) == 0x8e00 || (*x & 0x80)) {
		    currentGC = xjc->gcr;
		    *JW_buf |= KANA;
		} else {
		    currentGC = xjc->gch;
		}
	    } else {
		if ((*x & 0xff00) == 0x8e00 || (*x & 0x80)) {
		    currentGC = xjc->reversegcr;
		    *JW_buf |= KANA;
		} else {
		    currentGC = xjc->reversegch;
		}
		*JW_buf |= REV;
		check_move(xjc, 0);
	    }
	    XDrawImageString(dpy, xjc->ws, currentGC,
			     xjc->currentcol0 * FontWidth(xjc),
			     FontAscent(xjc), cc, 1);
	    if (xjc->b_flag == 1) {
		XDrawString(dpy, xjc->ws, currentGC,
			    xjc->currentcol0 * FontWidth(xjc) + 1,
			    FontAscent(xjc), cc, 1);
		*JW_buf |= BOLD;
	    }
	    if (xjc->u_line_flag == 1) {
		XDrawLine(dpy, xjc->ws, xjc->gch,
			  xjc->currentcol0 * FontWidth(xjc), FontAscent(xjc) + 1,
			  (xjc->currentcol0 + 1) * FontWidth(xjc),
			  FontAscent(xjc) + 1);
		*JW_buf |= UNDER;
	    }
	    if ((xjc->currentcol - xjc->vst) >= xjc->max_cur) {
		xjc->max_cur = xjc->currentcol0 - xjc->vst + 1;
	    }
	    xjc->currentcol0 += 1;
	}
    }   
    return(1);
}

/*****************************************************************************
	JW1 への出力ルーチン
			from JWMflushw_buf()
*****************************************************************************/
JW1Mputwc(wx, len)
w_char	*wx;
int len;
{
    unsigned char cc[2];
    register int cnt = len;
    register unsigned short *x = wx;
    register XJClient *xjc;
    register int *JW_buf;
    GC currentGC;

    xjc = cur_p;

    visual_window();
    if (xjc->currentcol >= xjc->maxcolumns)
	return(-1);

    for (; cnt > 0; cnt--, x++) {
	JW_buf = xjc->buf + xjc->currentcol;
	if ((*x & 0x8000) != 0 && (*x & 0xff00) != 0x8e00) {
	    *JW_buf = (cc[0]=((*x & 0x7f00)>>8)) | KAN1;
	    *(JW_buf + 1) = (cc[1] = (*x & 0x7f)) | KAN2;
	    if (xjc->r_flag == 0) {
			currentGC = xjc->gck;
	    } else {
			currentGC = xjc->reversegck;
			*JW_buf |= REV;
			*(JW_buf + 1) |= REV;
			check_move(xjc, 1);
	    }
	    XDrawImageString16(dpy, xjc->w1, currentGC,
			xjc->currentcol * FontWidth(xjc),
			FontAscent(xjc), (XChar2b *)cc, 1);
	    if (xjc->b_flag == 1) {
		    XDrawString16(dpy, xjc->w1, currentGC,
			xjc->currentcol * FontWidth(xjc) + 1,
			FontAscent(xjc), (XChar2b *)cc, 1);
		    *JW_buf |= BOLD;
		    *(JW_buf + 1) |= BOLD;
		}
		if (xjc->u_line_flag == 1) {
		    XDrawLine(dpy, xjc->w1, xjc->gch,
			xjc->currentcol * FontWidth(xjc),
			FontAscent(xjc) + 1,
			(xjc->currentcol + 2) * FontWidth(xjc),
			FontAscent(xjc) + 1);
		    *JW_buf |= UNDER;
		    *(JW_buf + 1) |= UNDER;
		}
		if ((xjc->currentcol - xjc->vst + 1) >= xjc->max_cur) {
		    xjc->max_cur = xjc->currentcol + 2 - xjc->vst;
		}
		xjc->currentcol += 2;
	} else {
	    if (*x < 0x20)
		return(0);
	    *JW_buf = cc[0] = *x & 0x7f;
	    if (xjc->r_flag == 0) {
		if ((*x & 0xff00) == 0x8e00 || (*x & 0x80)) {
		    currentGC = xjc->gcr;
		    *JW_buf |= KANA;
		} else {
		    currentGC = xjc->gch;
		}
	    } else {
		if ((*x & 0xff00) == 0x8e00 || (*x & 0x80)) {
		    currentGC = xjc->reversegcr;
		    *JW_buf |= KANA;
		} else {
		    currentGC = xjc->reversegch;
		}
		*JW_buf |= REV;
		check_move(xjc, 0);
	    }
	    XDrawImageString(dpy, xjc->w1, currentGC,
			     xjc->currentcol * FontWidth(xjc),
			     FontAscent(xjc), cc, 1);
	    if (xjc->b_flag == 1) {
		XDrawString(dpy, xjc->w1, currentGC,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), cc, 1);
		*JW_buf |= BOLD;
	    }
	    if (xjc->u_line_flag == 1) {
		XDrawLine(dpy, xjc->w1, xjc->gch,
			  xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
			  (xjc->currentcol + 1) * FontWidth(xjc),
			  FontAscent(xjc) + 1);
		*JW_buf |= UNDER;
	    }
	    if ((xjc->currentcol - xjc->vst) >= xjc->max_cur) {
		xjc->max_cur = xjc->currentcol - xjc->vst + 1;
	    }
	    xjc->currentcol += 1;
	}
    }   
    return(1);
}

/*****************************************************************************
	JW への出力ルーチン
			from JWMflushw_buf()
*****************************************************************************/
JWMputwc(wx, len)
register w_char	*wx;
int		len;
{
    unsigned char cc[2];
    register int cnt = len;
    register int column = 0;
    register unsigned short *x = wx;
    register XJClient *xjc;
    register int *JW_buf;
    GC currentGC;
    Window currentW;

    xjc = cur_p;

    if (xjc->currentcol == 0 && xjc->visible == 0)
	visual_window1();
    if (xjc->currentcol >= xjc->maxcolumns)
	return(-1);

    for (; cnt > 0; cnt--, x++) {
	if ((*x & 0x8000) && (*x & 0xff00) != 0x8e00)
	    column += 2;
	else
	    column += 1;
    }
    if ((xjc->currentcol + column - xjc->vst) > xjc->max_cur) {
	xjc->max_cur = xjc->currentcol + column - xjc->vst;
	xjc->m_cur_flag = 1;
	Resize_jw();
    }

    for (cnt = len, x = wx; cnt > 0; cnt--, x++) {
	JW_buf = xjc->buf + xjc->currentcol;
	if ((*x & 0x8000) != 0 && (*x & 0xff00) != 0x8e00) {
	    if ((xjc->currentcol + 2 - xjc->vst) > xjc->c0
	    && (xjc->currentcol - xjc->vst) <= xjc->c0
	    && xjc->c1) {
		xjc->linefeed1 = xjc->c0 + xjc->vst - xjc->currentcol;
		visible_line2();
		if (xjc->visible_line < 2)
		    xjc->visible_line = 2;
		if (xjc->linefeed1 == 1) {
		    XClearArea(dpy, xjc->w1, 
		        (xjc->c0 + xjc->vst - 1) * FontWidth(xjc), 0,
			0, 0, False);
		}
	    }
	    if (((xjc->currentcol + xjc->linefeed1 + 2 - xjc->vst) >
	    (xjc->c0 + xjc->c1)
	    && (xjc->currentcol + xjc->linefeed1 - xjc->vst) <=
	    (xjc->c0 + xjc->c1))
	    && (xjc->c0 + xjc->c1) < xjc->max_columns) {
		xjc->linefeed2 =
		    xjc->c0 + xjc->c1 + xjc->vst - xjc->currentcol;
		visible_line3();
		if (xjc->visible_line < 3)
		    xjc->visible_line = 3;
		if ((xjc->linefeed2 - xjc->linefeed1) == 1) {
		    XClearArea(dpy, xjc->w2, 
		        (xjc->c0 + xjc->c1 + xjc->vst - xjc->linefeed2)
		        * FontWidth(xjc), 0,
		        0, 0, False);
		}
	    }
	    *JW_buf = (cc[0]=((*x & 0x7f00)>>8)) | KAN1;
	    *(JW_buf + 1) = (cc[1] = (*x & 0x7f)) | KAN2;

	    if (xjc->r_flag == 0) {
		currentGC = xjc->gck;
	    } else {
		currentGC = xjc->reversegck;
		*JW_buf |= REV;
		*(JW_buf + 1) |= REV;
		check_move(xjc, 1);
	    }
	    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 2))
	    	currentW = xjc->w1;
	    else if ((xjc->currentcol - xjc->vst)
		 <= (xjc->c0 + xjc->c1 - 2 - xjc->linefeed1))
	    	currentW = xjc->w2;
	    else
	    	currentW = xjc->w3;

	    XDrawImageString16(dpy, currentW, currentGC,
		xjc->currentcol * FontWidth(xjc),
		FontAscent(xjc), (XChar2b *)cc, 1);
	    if (xjc->b_flag == 1) {
		XDrawString16(dpy, currentW, currentGC,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), (XChar2b *)cc, 1);
		*JW_buf |= BOLD;
		*(JW_buf + 1) |= BOLD;
	    }
	    if (xjc->u_line_flag == 1) {
		XDrawLine(dpy, currentW, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 2) * FontWidth(xjc),FontAscent(xjc) + 1);
		*JW_buf |= UNDER;
		*(JW_buf + 1) |= UNDER;
	    }
		xjc->currentcol += 2;
	} else {
	    if (*x < 0x20)
		return(0);
	    if ((xjc->currentcol - xjc->vst) == xjc->c0 
	    && xjc->c0 < xjc->max_columns) {
		xjc->linefeed1 = 0;
		visible_line2();
		if (xjc->visible_line < 2)
		    xjc->visible_line = 2;
	    }
	    if (((xjc->currentcol + xjc->linefeed1 - xjc->vst) ==
	    (xjc->c0 + xjc->c1))
	    && (xjc->c0 + xjc->c1) < xjc->max_columns) {
		xjc->linefeed2 = xjc->linefeed1;
		visible_line3();
		if (xjc->visible_line < 3)
		    xjc->visible_line = 3;
	    }
	    *JW_buf = cc[0] = *x & 0x7f;
	    if (xjc->r_flag == 0) {
		if ((*x & 0xff00) == 0x8e00 || (*x & 0x80)) {
		    currentGC = xjc->gcr;
		    *JW_buf |= KANA;
		} else {
		    currentGC = xjc->gch;
		}
	    } else {
		if ((*x & 0xff00) == 0x8e00 || (*x & 0x80)) {
		    currentGC = xjc->reversegcr;
		    *JW_buf |= KANA;
		} else {
		    currentGC = xjc->reversegch;
		}
		*JW_buf |= REV;
		check_move(xjc, 0);
	    }
	    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
		currentW = xjc->w1;
	    else if ((xjc->currentcol - xjc->vst) <=
	    (xjc->c0 + xjc->c1 - 1 - xjc->linefeed1))
		currentW = xjc->w2;
	    else
		currentW = xjc->w3;

	    XDrawImageString(dpy, currentW, currentGC,
		xjc->currentcol * FontWidth(xjc), FontAscent(xjc), cc, 1);

	    if (xjc->b_flag == 1) {
		XDrawString(dpy, currentW, currentGC,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
		*JW_buf |= BOLD;
	    }

	    if (xjc->u_line_flag == 1) {
		XDrawLine(dpy, currentW, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc),FontAscent(xjc) + 1);
		*JW_buf |= UNDER;
	    }
	    xjc->currentcol += 1;
	}
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
#ifdef	SX8870
    register w_char	*x;
#else	/* !SX8870 */
    register wchar_t	*x;
#endif	/* !SX8870 */
    register XJClient	*xjc;

    xjc = cur_p;

    if(cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	JW1Mputwc(w_buf, len);
    } else {
	JWMputwc(w_buf, len);
    }
    if (xjc->currentcol > xjc->max_pos) {
	xjc->max_pos = xjc->currentcol;
    }
    XFlush(dpy);
}

jw_disp_mode(mode)
unsigned char *mode;
{
    register wchar_t w_buf[10];
    int len;

    cur_p->currentcol0 = 0;
    len = Sstrcpy(w_buf , mode);
    JW0Mputwc(w_buf, len);
    if (cur_p->currentcol0 > cur_p->max_pos0) {
	   cur_p->max_pos0 = cur_p->currentcol0;
    }
    return;
}

Resize_jw()
{
    XWindowChanges	values;
    register XJClient	*xjc;


    xjc = cur_p;
    if (cur_x->dispmode == 1 && xjc->m_cur_flag) {
	values.width = xjc->max_cur * FontWidth(xjc);
	if (xjc->c2) {
	    if (xjc->max_cur <= (xjc->c0 + xjc->c1 + xjc->c2 - xjc->linefeed2)){
		XResizeWindow(dpy, xjc->wp3,
		    (xjc->max_cur + xjc->linefeed2) * FontWidth(xjc),
		    FontHeight(xjc));
		xjc->max_l3 = 0;
	    } else if (!xjc->max_l3) {
		XResizeWindow(dpy, xjc->wp3,
		    (xjc->c0 + xjc->c1 + xjc->c2) * FontWidth(xjc),
		    FontHeight(xjc));
		xjc->max_l3 = 1;
	    }
	}
	if (xjc->c1) {
	    if (xjc->max_cur <= (xjc->c0 + xjc->c1 - xjc->linefeed1)) {
		XResizeWindow(dpy, xjc->wp2,
		    (xjc->max_cur + xjc->linefeed1) * FontWidth(xjc),
		    FontHeight(xjc));
		xjc->max_l2 = 0;
	    } else if (!xjc->max_l2) {
		XResizeWindow(dpy, xjc->wp2,
		    (xjc->c0 + xjc->c1) * FontWidth(xjc), FontHeight(xjc));
		xjc->max_l2 = 1;
	    }
	}
	if (xjc->c0) {
	    if (xjc->max_cur <= xjc->c0) {
		XResizeWindow(dpy, xjc->wp1,
		    xjc->max_cur * FontWidth(xjc), FontHeight(xjc));
		xjc->max_l1 = 0;
	    } else if (!xjc->max_l1) {
		XResizeWindow(dpy, xjc->wp1,
		    xjc->c0 * FontWidth(xjc), FontHeight(xjc));
		xjc->max_l1 = 1;
	    }
	}
	XFlush(dpy);
	xjc->m_cur_flag = 0;
    }
}

/*****************************************************************************
	JW xカラム以降をクリアする ルーチン
			from clr_end_screen()
*****************************************************************************/
JWMline_clear(x)
register int	x;
{
    register XJClient *xjc;
    register int cnt;
    register int *JW_buf;

    xjc = cur_p;
    JW_buf = xjc->buf;

    for(JW_buf = xjc->buf + x, cnt = 0; *JW_buf != 0; JW_buf++, cnt++)
	*JW_buf = 0;
    if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	XClearArea(dpy, xjc->w1, x * FontWidth(xjc), 0,
	    cnt * FontWidth(xjc) + 1, 0, False);
	if(x <= (xjc->vst + xjc->max_columns)) {
	    invisual_note_back();
	    xjc->note_back = 0;
	    XUnmapWindow(dpy, xjc->wnn);
	}
	if (x <= 0) {
	    invisual_window();
	}
    } else if (cur_x->dispmode == 1) {
	xjc->del_x = x;
    }
    xjc->currentcol = x;
    xjc->max_pos = x;
    XFlush(dpy);
}

JWMline_clear1()
{
    register XJClient	*xjc;
    register int	cnt;
    register int	xx;

    xjc = cur_p;
    xx = xjc->del_x;
    if (xjc->currentcol == xjc->max_pos) xx++;
    if (((xx - xjc->vst) <= (xjc->c0 + xjc->c1 - xjc->linefeed1))
    || (xjc->visible_line < 3)) {
	    XUnmapWindow(dpy, xjc->wp3);
	    xjc->visible_line = 2;
    }
    if (((xx - xjc->vst) <= xjc->c0)
    || (xjc->visible_line < 2)) {
	    XUnmapWindow(dpy, xjc->wp2);
	    xjc->visible_line = 1;
    }
    xjc->m_cur_flag = 1;
    if (xjc->del_x <= 0) {
	invisual_window();
	xjc->visible_line = 0;
	xjc->max_l1 = 0;
	xjc->max_cur = 0;
	xjc->m_cur_flag = 1;
    }
    xjc->del_x = -1;
    xjc->max_cur = xx - xjc->vst;
    Resize_jw();
    XFlush(dpy);
}


/*****************************************************************************
	変換行表示 ルーチン
			from xw_read(), JWM1putc(), JWMflushw_buf(),
			     JWcursor_move()
*****************************************************************************/
visual_window()
{
/*
    if (_disp_window) {
*/
	if (cur_x->dispmode == 2) {
	    if(cur_p->visible == 0)
		visual_window3();
	} else if (cur_x->dispmode == 0 && cur_p->visible == 0) {
	    visual_window3();
	} else if (cur_x->dispmode == 1 && cur_p->visible == 0){
	    visual_window1();
	}
	XFlush(dpy);
/*
    }
*/
}

xw_display_title()
{
    if (cur_p->dispmode != 1) {
	XMapWindow(dpy, cur_p->wt);
	XFlush(dpy);
    } else {
	XUnmapWindow(dpy, cur_p->ws);
    }
}

xw_display_title_off()
{
    if (cur_p->dispmode != 1) {
	XUnmapWindow(dpy, cur_p->wt);
	XFlush(dpy);
    } else {
	xw_disp_mode1();
    }
}

xw_disp_mode1()
{
    register XJClient	*xjc;

    xjc = cur_p;
    if (xjc->status_lock == 0) {
	xjc->status_x = xjc->x;
	xjc->status_y = xjc->y - FontHeight(xjc);
	if ((xjc->x + FontWidth(xjc) * MHL0) > xjc->p_width)
	    xjc->status_x = (xjc->p_width - FontWidth(xjc) * MHL0);
	if ((xjc->y - FontHeight(xjc)) < 0)
	    xjc->status_y = xjc->p_height - FontHeight(xjc);
	XMoveWindow(dpy, xjc->ws, xjc->status_x, xjc->status_y);
    }
    XMapWindow(dpy, xjc->ws);
    XFlush(dpy);
}
/*****************************************************************************
	デフォルト変換行表示 ルーチン
			from wisual_window()
*****************************************************************************/
visual_window3()
{
    register XJClient	*xjc;

    xjc = cur_p;

    XMapWindow(dpy, xjc->wp1);
    XMapWindow(dpy, xjc->w1);
    if (xjc->max_pos) {
	redraw_window3(0, xjc->max_pos * FontWidth(xjc));
    }
    if (cur_x->dispmode == 2) {
	xjc->visible = 3;
    } else {
	xjc->visible = 1;
	xjc->visible_line = 1;
    }
    if (xjc->note_fore != 0) JWM_note_fore();
    if (xjc->note_back != 0) JWM_note_back();
}

/*****************************************************************************
	クライアント用変換行表示 ルーチン
			from wisual_window()
*****************************************************************************/
visual_window1()
{

    int	width;
    int	w_width;
    char	cc[2];
    register XJClient	*xjc;
    int max_pos_tmp;

    xjc = cur_p;
    if (xjc->visible == 0) {
	w_width = xjc->p_width / FontWidth(xjc);
	if (cur_x->dispmode == 1) {
	    if ((xjc->c0 = (xjc->p_width - xjc->x) / FontWidth(xjc))
	    > xjc->max_columns) {
		xjc->c0 = xjc->max_columns;
		xjc->c1 = xjc->c2 = 0;
	    } else if ((xjc->c1 = xjc->max_columns - xjc->c0) > w_width) {
		xjc->c1 = w_width;
		xjc->c2 = xjc->max_columns - xjc->c0 - xjc->c1;
	    } else {
		xjc->c2 = 0;
	    }
	    xjc->max_cur = xjc->max_pos - xjc->vst;
	    if (xjc->max_cur > xjc->c0)
		width = xjc->c0 * FontWidth(xjc);
	    else
		width = xjc->max_cur * FontWidth(xjc);
	    if (width == 0) width = 1;
	    XMoveResizeWindow(dpy, xjc->wp1, xjc->x, xjc->y,
		width, FontHeight(xjc));
	    XMoveWindow(dpy, xjc->w1, -(xjc->vst * FontWidth(xjc)), 0);
	    XFlush(dpy);
	}
	if (xjc->visible_line == 0)
	    xjc->visible_line = 1;

	XMapWindow(dpy, xjc->wp1);
	XRaiseWindow(dpy, xjc->wp1);
	XFlush(dpy);
	if (xjc->max_pos > xjc->c0 - 1 && xjc->buf[xjc->c0] != 0 && xjc->c0 < xjc->max_columns) {
	    if (check_kan(xjc->buf,xjc->c0 - 1) == 1)
		xjc->linefeed1 = 1;
	    else
		xjc->linefeed1 = 0;
	    visible_line2();
	    if (xjc->visible_line < 2)
		xjc->visible_line = 2;
	}
	if (xjc->max_pos > (xjc->c0 + xjc->c1 - 1 - xjc->linefeed1)
	&& (xjc->c0 + xjc->c1) < xjc->max_columns)  {
	    if (check_kan(xjc->buf,xjc->c0 + xjc->c1 - 1 - xjc->linefeed1) == 1)
		xjc->linefeed2 = xjc->linefeed1 + 1;
	    else
		xjc->linefeed2 = xjc->linefeed1;
	    visible_line3();
	    xjc->visible_line = 3;
	}
	if (xjc->vst) {
	    if (!xjc->note_fore) {
		JWM_note_fore();
		xjc->note_fore = 1;
	    }
	} else {
	    if (!xjc->note_back) {
		invisual_note_fore();
		xjc->note_fore = 0;
	    }
	}
	if (xjc->visible_line == 3)
		max_pos_tmp = xjc->vst + xjc->max_columns - xjc->linefeed2;
	else if (xjc->visible_line == 2)
		max_pos_tmp = xjc->vst + xjc->max_columns - xjc->linefeed1;
	else
		max_pos_tmp = xjc->vst + xjc->max_columns;

	if (xjc->max_pos > max_pos_tmp) {
		if (!xjc->note_back) {
			JWM_note_back();
			xjc->note_back = 1;
		}
		if ((check_kan(xjc->buf,max_pos_tmp - 1)) == 1)
			JWM_note_null();
		else
			XUnmapWindow(dpy, xjc->wnn);
	} else {
		if (xjc->note_back) {
			invisual_note_back();
			xjc->note_back = 0;
		}
		XUnmapWindow(dpy, xjc->wnn);
	}
	xjc->visible = 1;
    }
}


/*****************************************************************************
	クライアント用変換行２表示 ルーチン
			from visual_window1(), JWmark_cursor(),redraw_line2()
*****************************************************************************/
visible_line2()
{
    register XJClient	*xjc;
    int		x, y, width;

    xjc = cur_p;

    if (cur_x->dispmode == 1) {
	x = -(xjc->c0 * FontWidth(xjc));
	y = xjc->y + FontHeight(xjc);
	if (y + FontHeight(xjc) > xjc->p_height) y = 0;
	if (xjc->max_cur > (xjc->c0 + xjc->c1))
	    width = (xjc->c0 + xjc->c1) * FontWidth(xjc);
	else
	    width = (xjc->max_cur + xjc->linefeed1) * FontWidth(xjc);
	XMoveResizeWindow(dpy, xjc->wp2, x, y, width, FontHeight(xjc));
	XMoveWindow(dpy, xjc->w2,
		-((xjc->vst - xjc->linefeed1) * FontWidth(xjc)), 0);
    }
    XMapWindow(dpy, xjc->wp2);
    XRaiseWindow(dpy, xjc->wp2);
    XFlush(dpy);
}

/*****************************************************************************
	クライアント用変換行３表示 ルーチン
			from visual_window1(), JWmark_cursor(),redraw_line3()
*****************************************************************************/
visible_line3()
{
    register XJClient	*xjc;
    int		x, y, width;

    xjc = cur_p;

    if (cur_x->dispmode == 1) {
	x = -((xjc->c0 + xjc->c1) * FontWidth(xjc));
	y = xjc->y + FontHeight(xjc) * 2;
	if ((y + FontHeight(xjc) * 2) > xjc->p_height)
	    if ((y + FontHeight(xjc)) > xjc->p_height)
		y = FontHeight(xjc);
	    else
		y = 0;
	if (xjc->max_cur > (xjc->c0 + xjc->c1 + xjc->c2))
	    width = (xjc->c0 + xjc->c1 + xjc->c2) * FontWidth(xjc);
	else
	    width = (xjc->max_cur + xjc->linefeed2) * FontWidth(xjc);
	XMoveResizeWindow(dpy, xjc->wp3, x, y, width, FontHeight(xjc));
	XMoveWindow(dpy, xjc->w3,
		-((xjc->vst - xjc->linefeed2) * FontWidth(xjc)), 0);
    }
    XMapWindow(dpy, xjc->wp3);
    XRaiseWindow(dpy, xjc->wp3);
    XFlush(dpy);
}


/*****************************************************************************
	変換行非表示 ルーチン
			from xw_read(), JWMline_clear()
*****************************************************************************/
invisual_window()
{
    if (cur_p->visible != 0) {
	if (cur_x->dispmode == 2 || cur_x->dispmode == 0) 
	    invisual_window3();
	else if (cur_x->dispmode == 1)
	    invisual_window1();
	cur_p->visible = 0;
    }
    XFlush(dpy);
}


/*****************************************************************************
	デフォルト変換行非表示 ルーチン
			from JW0Mputc(), invisual_window()
*****************************************************************************/
invisual_window3()
{
    register XJClient	*xjc;

    xjc = cur_p;

    if (xjc->note_fore) {
	XUnmapWindow(dpy, xjc->wnf);
	xjc->note_fore = 0;
    }
    if (xjc->note_back) {
	XUnmapWindow(dpy, xjc->wnb);
	xjc->note_back = 0;
    }
    if (cur_x->dispmode == 2)
	xjc->visible = 0;
    else
	xjc->visible_line = 0;
    xjc->vst = 0;
    XClearWindow(dpy, xjc->w1);
    XUnmapWindow(dpy, xjc->wp1);
    JWwindow_move(xjc);
}


/*****************************************************************************
	クライアント用変換行非表示 ルーチン
			from invisual_window()
*****************************************************************************/
invisual_window1()
{
    register XJClient	*xjc;

    xjc = cur_p;

    XUnmapWindow(dpy, xjc->wp1);
    invisual_line2();
    invisual_line3();
    xjc->visible = 0;
    if (xjc->note_fore) {
	XUnmapWindow(dpy, xjc->wnf);
	xjc->note_fore = 0;
    }
    if (xjc->note_back) {
	XUnmapWindow(dpy, xjc->wnb);
	xjc->note_back = 0;
    }
    XUnmapWindow(dpy, xjc->wnn);
}

/*****************************************************************************
	入力行の前半が欠けている印表示 ルーチン
			from t_cont_line_note(), visual_window3()
*****************************************************************************/
JWM_note_fore()
{
    register XJClient	*xjc;

    xjc = cur_p;

    if (!xjc->note_fore) {
	if (cur_x->dispmode == 2  || cur_x->dispmode == 0) {
	    XMapWindow(dpy, xjc->wnf);
	    XDrawImageString(dpy, xjc->wnf, xjc->gch,
		0, FontAscent(xjc), dol_char, 1);
	} else if (cur_x->dispmode == 1) {
	    XMoveWindow(dpy, xjc->wnf, (xjc->x - FontWidth(xjc)), xjc->y);
	    XMapWindow(dpy, xjc->wnf);
	    XRaiseWindow(dpy, xjc->wnf);
	    XDrawImageString(dpy, xjc->wnf, xjc->gch,
		0, FontAscent(xjc), dol_char, 1);
	}
	XFlush(dpy);
    }
}

JWM_note_back()
{
    register XJClient	*xjc;
    int	x, y;

    xjc = cur_p;

    if (!xjc->note_back) {
	if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	    XMapWindow(dpy, xjc->wnb);
	    XDrawImageString(dpy, xjc->wnb, xjc->gch,
		0, FontAscent(xjc), dol_char, 1);
	} else if (cur_x->dispmode == 1) {
	    if (xjc->c2){
		x = xjc->c2 * FontWidth(xjc);
		y = xjc->y + FontHeight(xjc) * 2;
		if ((y + FontHeight(xjc) * 2) > xjc->p_height)
		    if ((y + FontHeight(xjc)) > xjc->p_height)
			y = FontHeight(xjc);
		    else
			y = 0;
	    } else if (xjc->c1) {
		x = xjc->c1 * FontWidth(xjc);
		y = xjc->y + FontHeight(xjc);
		if ((y + FontHeight(xjc)) > xjc->p_height)
		    y = 0;
	    } else {
		x = xjc->x + xjc->c0 * FontWidth(xjc);
		y = xjc->y;
	    }
	    XMoveWindow(dpy, xjc->wnb, x, y);
	    XMapWindow(dpy, xjc->wnb);
	    XRaiseWindow(dpy, xjc->wnb);
	    XDrawImageString(dpy, xjc->wnb, xjc->gch,
		0, FontAscent(xjc), dol_char, 1);
	}
	XFlush(dpy);
    }
}

/*****************************************************************************
	カーソル表示 ルーチン
			from JWMflushw_buf(), JWMline_clear(), JWcursor_visible()
			     JWcursor_move(), redraw_window3(), redraw_line1()
			     redraw_line2(), redraw_line3()
*****************************************************************************/
JWmark_cursor()
{
    register XJClient	*xjc;
    register int	*JW_buf;
    unsigned char	s_c[2];
    Window		currentW;

    xjc = cur_p;
    JW_buf = xjc->buf + xjc->currentcol;

/*
    if (_disp_window == 0) {
	return;
    }
*/
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 0) {
	if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	    if ((*JW_buf & REV) == 0) {
		if ((*JW_buf & KAN1) != 0){
		    check_move(xjc, 1);
		    s_c[0] = (char)(*JW_buf & 0xff);
		    s_c[1] = (char)(*(JW_buf + 1) & 0xff);
		    XDrawImageString16(dpy, xjc->w1, xjc->reversegck,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			(XChar2b *)s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString16(dpy, xjc->w1, xjc->reversegck,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), (XChar2b *)s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 2) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if ((*JW_buf & KANA) != 0) {
		    check_move(xjc, 0);
		    s_c[0] = (char)(*JW_buf & 0xff);
		    XDrawImageString(dpy, xjc->w1, xjc->reversegcr,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, xjc->w1, xjc->reversegcr,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
		  	XDrawLine(dpy, xjc->w1, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if (*JW_buf != 0){
		    check_move(xjc, 0);
		    s_c[0] = (char)(*JW_buf & 0xff);
		    XDrawImageString(dpy, xjc->w1, xjc->reversegch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, xjc->w1, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
		  	XDrawLine(dpy, xjc->w1, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else {
		    check_move(xjc, 0);
		    if ((xjc->currentcol - xjc->vst) >= xjc->max_cur) {
			xjc->max_cur += 1;
			Resize_jw();
		    }
		    XDrawImageString(dpy, xjc->w1, xjc->reversegch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			cursor_char, 1);
		}
	    }
	} else {
	    if ((*JW_buf & REV) == 0) {
		if ((*JW_buf & KAN1) != 0){
		    check_move(xjc, 1);
		    s_c[0] = (char)(*JW_buf & 0xff);
		    s_c[1] = (char)(*(JW_buf + 1) & 0xff);
		    if ((xjc->currentcol + 2 - xjc->vst) <= xjc->c0)
			currentW = xjc->w1;
		    else if ((xjc->currentcol + xjc->linefeed1 + 2 - xjc->vst)
		    <= (xjc->c0 + xjc->c1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString16(dpy, currentW, xjc->reversegck,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			(XChar2b *)s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString16(dpy, currentW, xjc->reversegck,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), (XChar2b *)s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
	 		XDrawLine(dpy, currentW, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 2) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if ((*JW_buf & KANA) != 0) {
	  	    check_move(xjc, 0);
		    s_c[0] = (char)(*JW_buf & 0xff);
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
			currentW = xjc->w1;
		    else if ((xjc->currentcol + xjc->linefeed1 + 1 - xjc->vst)
		    <= (xjc->c0 + xjc->c1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString(dpy, currentW, xjc->reversegcr,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, currentW, xjc->reversegcr,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, currentW, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if (*JW_buf != 0) {
	  	    check_move(xjc, 0);
		    s_c[0] = (char)(*JW_buf & 0xff);
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
			currentW = xjc->w1;
		    else if ((xjc->currentcol + xjc->linefeed1 + 1 - xjc->vst)
		    <= (xjc->c0 + xjc->c1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString(dpy, currentW, xjc->reversegch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, currentW, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, currentW, xjc->reversegch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else {
		    check_move(xjc, 0);
		    if ((xjc->currentcol - xjc->vst)== xjc->c0
		    && xjc->c0 < xjc->max_columns) {
			xjc->linefeed1 = 0;
			visible_line2();
			if (xjc->visible_line < 2)
			    xjc->visible_line = 2;
		    }
		    if (((xjc->currentcol  + xjc->linefeed1 - xjc->vst)
		    == (xjc->c0 + xjc->c1))
		    && (xjc->c0 + xjc->c1) < xjc->max_columns) {
			xjc->linefeed2 = xjc->linefeed1;
			visible_line3();
			if (xjc->visible_line < 3)
			    xjc->visible_line = 3;
		    }
		    if ((xjc->currentcol - xjc->vst) >= xjc->max_cur) {
			xjc->max_cur += 1;
			xjc->m_cur_flag = 1;
			Resize_jw();
		    }
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
			currentW = xjc->w1;
		    else if ((xjc->currentcol - xjc->vst)
		    <= (xjc->c0 + xjc->c1 - 1 - xjc->linefeed1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString(dpy, currentW, xjc->reversegch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			cursor_char, 1);
		}
	    }
	    if ((xjc->max_pos - xjc->vst) > xjc->max_cur && !xjc->m_cur_flag) {
		xjc->max_cur = xjc->max_pos - xjc->vst;
		xjc->m_cur_flag = 1;
		Resize_jw();
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
    register XJClient *xjc;
    register int *JW_buf;
    unsigned char s_c[2];
    Window currentW;

    xjc = cur_p;
    JW_buf = xjc->buf + xjc->currentcol;

/*
    if (_disp_window == 0) {
	return;
    }
*/
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 1) {
	if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	    if ((*JW_buf & REV) == 0) {
		if ((*JW_buf & KAN1) != 0) {
		    s_c[0] = (char)(*JW_buf & 0xff);
		    s_c[1] = (char)(*(JW_buf + 1) & 0xff);
		    XDrawImageString16(dpy, xjc->w1, xjc->gck,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			(XChar2b *)s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString16(dpy, xjc->w1, xjc->gck,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), (XChar2b *)s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 2) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if ((*JW_buf & KANA) != 0) {
		    s_c[0] = (char)(*JW_buf & 0xff);
		    XDrawImageString(dpy, xjc->w1, xjc->gcr,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, xjc->w1, xjc->gcr,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if (*JW_buf != 0){
		    s_c[0] = (char)(*JW_buf & 0xff);
		    XDrawImageString(dpy, xjc->w1, xjc->gch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, xjc->w1, xjc->gch,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, xjc->w1, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else {
		    XDrawImageString(dpy, xjc->w1, xjc->gch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			cursor_char, 1);
		}
	    }
	} else {
	    if ((*JW_buf & REV) == 0) {
		if ((*JW_buf & KAN1) != 0){
		    s_c[0] = (char)(*JW_buf & 0xff);
		    s_c[1] = (char)(*(JW_buf + 1) & 0xff);
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 2))
			currentW = xjc->w1;
		    else if ((xjc->currentcol - xjc->vst)
		    <= (xjc->c0 + xjc->c1 - 2 - xjc->linefeed1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString16(dpy, currentW, xjc->gck,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			(XChar2b *)s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString16(dpy, currentW, xjc->gck,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), (XChar2b *)s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, currentW, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 2) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		 } else if ((*JW_buf & KANA) != 0) {
		    s_c[0] = (char)(*JW_buf & 0xff);
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
			currentW = xjc->w1;
		    else if ((xjc->currentcol - xjc->vst)
		    <= (xjc->c0 + xjc->c1 - 1 - xjc->linefeed1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString(dpy, currentW, xjc->gcr,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, currentW, xjc->gcr,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, currentW, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else if (*JW_buf != 0){
		    s_c[0] = (char)(*JW_buf & 0xff);
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
			currentW = xjc->w1;
		    else if ((xjc->currentcol - xjc->vst)
		    <= (xjc->c0 + xjc->c1 - 1 - xjc->linefeed1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString(dpy, currentW, xjc->gch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			s_c, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, currentW, xjc->gch,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), s_c, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, currentW, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		} else {
		    if ((xjc->currentcol - xjc->vst) <= (xjc->c0 - 1))
			currentW = xjc->w1;
		    else if ((xjc->currentcol - xjc->vst)
		    <= (xjc->c0 + xjc->c1 - 1 - xjc->linefeed1))
			currentW = xjc->w2;
		    else
			currentW = xjc->w3;
		    XDrawImageString(dpy, currentW, xjc->gch,
			xjc->currentcol * FontWidth(xjc), FontAscent(xjc),
			cursor_char, 1);
		    if ((*JW_buf & BOLD) != 0)
			XDrawString(dpy, currentW, xjc->gch,
			    xjc->currentcol * FontWidth(xjc) + 1,
			    FontAscent(xjc), cursor_char, 1);
		    if ((*JW_buf & UNDER) != 0)
			XDrawLine(dpy, currentW, xjc->gch,
			    xjc->currentcol * FontWidth(xjc), FontAscent(xjc)+1,
			    (xjc->currentcol + 1) * FontWidth(xjc),
			    FontAscent(xjc) + 1);
		}
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
    register XJClient *xjc;

    xjc = cur_p;

/*
    if (_disp_window == 0) {
	xjc->cursor_flag0 = 1;
    } else {
*/
	xjc->cursor_flag = 1;
/*
    }
*/
    JWmark_cursor();
}

/*****************************************************************************
	カーソル非表示フラグセット ルーチン
			from cursor_invisible_raw()
*****************************************************************************/
JWcursor_invisible()
{
    register XJClient *xjc;

    xjc = cur_p;

    JWremark_cursor();
/*
      if (_disp_window == 0) {
	xjc->cursor_flag0 = 0;
    } else {
*/
	xjc->cursor_flag = 0;
/*
    }
*/
}

/*****************************************************************************
	カーソル移動 ルーチン
			from throw_cur_raw()
*****************************************************************************/
JWcursor_move(x)
{
    register XJClient *xjc;

    xjc = cur_p;

    if (xjc->visible != 0) {
				/* 63/5/10 KURI 確定後のカーソル移動
				   フラグメントがインビジュアルな時に
				   ビジュアルにしない */
	visual_window();
	JWremark_cursor();
	xjc->currentcol = x;
	if (x >= 0)
	    JWmark_cursor();
    } else {
				/* 63/5/10 KURI 確定後のカーソル移動
				   フラグメントがインビジュアルな時に
				   ビジュアルにしない */
	if (xjc->visible_line)
	    JWremark_cursor();
	xjc->currentcol = x;
	if (xjc->visible_line && x >= 0)
	    JWmark_cursor();
    }
}

/*****************************************************************************
	入力行の前半が欠けている印非表示 ルーチン
*****************************************************************************/
invisual_note_fore()
{
    register XJClient	*xjc;

    xjc = cur_p;

    if (xjc->note_fore) {
	if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	    XUnmapWindow(dpy, xjc->wnf);
	} else if(cur_x->dispmode == 1){
	    XUnmapWindow(dpy, xjc->wnf);
	}
	XFlush(dpy);
    }
}

invisual_note_back()
{
    register XJClient	*xjc;

    xjc = cur_p;

    if (xjc->note_back) {
	if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	    XUnmapWindow(dpy, xjc->wnb);
	} else if(cur_x->dispmode == 1) {
	    XUnmapWindow(dpy, xjc->wnb);
	}
	XFlush(dpy);
    }
}

/*****************************************************************************
	クライアント用変換行２非表示 ルーチン
			from invisual_window1()
*****************************************************************************/
invisual_line2()
{
    XUnmapWindow(dpy, cur_p->wp2);
    XFlush(dpy);
}

/*****************************************************************************
	クライアント用変換行３非表示 ルーチン
			from invisual_window1()
*****************************************************************************/
invisual_line3()
{
    XUnmapWindow(dpy, cur_p->wp3);
    XFlush(dpy);
}

/*****************************************************************************
	ＸＷＮＭＯウィンドウ書き直し ルーチン
			from xw_read()
*****************************************************************************/
redraw_window0()
{
    register XJClient	*xjc;
    register int	cnt;
    unsigned char	cc[2];
    int		u_line_flag_tmp = 0;
    int		b_flag_tmp = 0;
    GC currentGC, currentGCK, currentGCR;

    xjc = cur_p;

    for ( cnt = 0; cnt < xjc->max_pos0 ; cnt++) {
	if (xjc->buf0[cnt] & REV) {
	    currentGC = xjc->reversegch;
	    currentGCK = xjc->reversegck;
	    currentGCR = xjc->reversegcr;
	} else {
	    currentGC = xjc->gch;
	    currentGCK = xjc->gck;
	    currentGCR = xjc->gcr;
	}
	if (xjc->buf0[cnt] & BOLD)
	    b_flag_tmp = 1;
	else
	    b_flag_tmp = 0;
	if (xjc->buf0[cnt] & UNDER)
	    u_line_flag_tmp = 1;
	else
	    u_line_flag_tmp = 0;

	if ((xjc->buf0[cnt] & KAN1) != 0) {
	    cc[0] = (char)xjc->buf0[cnt] & 0xff;
	    cc[1] = (char)xjc->buf0[cnt + 1] & 0xff;
	    XDrawImageString16(dpy, xjc->ws, currentGCK,
		    cnt * FontWidth(xjc),
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString16(dpy, xjc->ws, currentGCK,
		    cnt * FontWidth(xjc) + 1,
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->ws, xjc->gch,
		    cnt * FontWidth(xjc),
		    FontAscent(xjc) + 1,
		    (cnt + 2) * FontWidth(xjc),
		    FontAscent(xjc) + 1);
	    cnt++;
	} else if ((xjc->buf0[cnt] & KANA) != 0) {
	    cc[0] = (char)xjc->buf0[cnt] & 0xff;
	    XDrawImageString(dpy, xjc->ws, currentGCR,
		    cnt * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->ws, currentGCR,
		    cnt * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		 XDrawLine(dpy, xjc->ws, xjc->gch,
		    cnt * FontWidth(xjc),
		    FontAscent(xjc) + 1,
		    (cnt + 1) * FontWidth(xjc),
		    FontAscent(xjc) + 1);
	} else {
	    cc[0] = (char)xjc->buf0[cnt] & 0xff;
	    XDrawImageString(dpy, xjc->ws, currentGC,
		    cnt * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->ws, currentGC,
		    cnt * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		 XDrawLine(dpy, xjc->ws, xjc->gch,
		    cnt * FontWidth(xjc),
		    FontAscent(xjc) + 1,
		    (cnt + 1) * FontWidth(xjc),
		    FontAscent(xjc) + 1);
	}
    }
    if(xjc->cursor_flag0 == 1 && xjc->mark_flag0 == 1) {
	xjc->mark_flag0 = 0;
    }
}

redraw_windowt(xjc)
register XJClient	*xjc;
{
    register char	*x;
    unsigned char	cc[2];
    register int	tmp_currentcol = 0;

    if (xjc->dispmode != 1) {
    for ( x = xwnmo_title; *x != NULL; x++) {
	if ((*x & 0x80) != 0 && (*x & 0xff) != 0x8e) {
	    cc[0] = (*x++ & 0x7f);
	    cc[1] = (*x & 0x7f);
	    XDrawImageString16(dpy, xjc->wt, xjc->gck,
		tmp_currentcol * FontWidth(xjc),
		xjc->firsty + FontAscent(xjc), (XChar2b *)cc, 1);
	    tmp_currentcol += 2;
	} else if ((*x & 0xff) == 0x8e) {
	    XDrawImageString(dpy, xjc->wt, xjc->gcr,
		tmp_currentcol * FontWidth(xjc),
		xjc->firsty + FontAscent(xjc), ++x, 1);
		tmp_currentcol += 1;
	} else {
	    XDrawImageString(dpy, xjc->wt, xjc->gch,
		tmp_currentcol * FontWidth(xjc),
		xjc->firsty + FontAscent(xjc), x, 1);
		tmp_currentcol += 1;
	}
    }
    }
}

/*****************************************************************************
	デフォルト変換行書き直し ルーチン
			from xw_read()
*****************************************************************************/
redraw_window3(x, width)
int	x, width;
{
    register XJClient *xjc;
    register int *JW_buf;
    unsigned char cc[2];
    int r_flag_tmp = 0;
    int b_flag_tmp = 0;
    int u_line_flag_tmp = 0;
    int currentcol_tmp = 0;
    GC currentGC, currentGCK, currentGCR;
    int start, end;

    if (cur_x->dispmode == 2)
	xjc = jfew->xjclient;
    else if (cur_x->dispmode == 0)
	xjc = cur_x;
    else
	return;

    currentcol_tmp = xjc->currentcol;


    start = x / FontWidth(xjc);
    end = start + width / FontWidth(xjc);
    if (check_kan(xjc->buf,start) == 2) start--;
    if (check_kan(xjc->buf,end) == 1) end++;
    for ( xjc->currentcol = start; xjc->currentcol < end; xjc->currentcol++) {
	JW_buf = xjc->buf + xjc->currentcol;
	if (*JW_buf & REV) {
	    r_flag_tmp = 1;
	    currentGC = xjc->reversegch;
	    currentGCK = xjc->reversegck;
	    currentGCR = xjc->reversegcr;
	} else {
	    r_flag_tmp = 0;
	    currentGC = xjc->gch;
	    currentGCK = xjc->gck;
	    currentGCR = xjc->gcr;
	}
	if (*JW_buf & BOLD)
	    b_flag_tmp = 1;
	else
	    b_flag_tmp = 0;
	if (*JW_buf & UNDER)
	    u_line_flag_tmp = 1;
	else
	    u_line_flag_tmp = 0;
	if ((*JW_buf & KAN1) != 0) {
	    cc[0] = (char)(*JW_buf) & 0xff;
	    cc[1] = (char)(*(JW_buf + 1)) & 0xff;
	    if (r_flag_tmp)
		check_move(xjc, 1);
	    XDrawImageString16(dpy, xjc->w1, currentGCK,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (b_flag_tmp == 1)
		XDrawString16(dpy, xjc->w1, currentGCK,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w1, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 2) * FontWidth(xjc), FontAscent(xjc)+1);
	    xjc->currentcol++;
	} else if ((*JW_buf & KANA) != 0) {
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move(xjc, 0);
	    XDrawImageString(dpy, xjc->w1, currentGCR,
		xjc->currentcol * FontWidth(xjc),
		FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
		XDrawString(dpy, xjc->w1, currentGCR,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w1, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	} else {
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move(xjc, 0);
	    XDrawImageString(dpy, xjc->w1, currentGC,
		xjc->currentcol * FontWidth(xjc),
		FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
		XDrawString(dpy, xjc->w1, currentGC,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w1, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	}
    }
    xjc->currentcol = currentcol_tmp;
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 1) {
	xjc->mark_flag = 0;
	JWmark_cursor();
    }
}

/*****************************************************************************
	クライアント用変換行書き直し ルーチン
*****************************************************************************/
redraw_line1(x, width)
{
    register XJClient *xjc;
    register int *JW_buf;
    unsigned char cc[2];
    int currentcol_tmp = 0;
    int r_flag_tmp = 0;
    int b_flag_tmp = 0;
    int u_line_flag_tmp = 0;
    int start, end;
    GC currentGC, currentGCK, currentGCR;

    xjc = cur_p;
    currentcol_tmp = xjc->currentcol;
    start = x / FontWidth(xjc);
    end = start + width / FontWidth(xjc);
    if (check_kan(xjc->buf,start) == 2) start--;
    if (check_kan(xjc->buf,end) == 1) end++;
    for ( xjc->currentcol = start; xjc->currentcol < end; xjc->currentcol++) {
	JW_buf = xjc->buf + xjc->currentcol;
	if (*JW_buf & REV) {
	    currentGC = xjc->reversegch;
	    currentGCK = xjc->reversegck;
	    currentGCR = xjc->reversegcr;
	} else {
	    currentGC = xjc->gch;
	    currentGCK = xjc->gck;
	    currentGCR = xjc->gcr;
	}
	if (*JW_buf & BOLD)
	    b_flag_tmp = 1;
	else
	    b_flag_tmp = 0;
	if (*JW_buf & UNDER)
	    u_line_flag_tmp = 1;
	else
	    u_line_flag_tmp = 0;
	if ((*JW_buf & KAN1) != 0) {
	    if (xjc->currentcol > xjc->c0 + xjc->vst - 2) {
		if ((xjc->buf[xjc->c0 + xjc->vst - 1] & KAN1) != 0)
		    xjc->linefeed1 = 1;
		else
		    xjc->linefeed1 = 0;
		break;
	    }
	    cc[0] = (char)(*JW_buf) & 0xff;
	    cc[1] = (char)(*(JW_buf + 1)) & 0xff;
	    if (r_flag_tmp)
		check_move1(xjc, 1);
	    XDrawImageString16(dpy, xjc->w1, currentGCK,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString16(dpy, xjc->w1, currentGCK,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w1, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 2) * FontWidth(xjc), FontAscent(xjc)+1);
	    xjc->currentcol++;
	} else if ((*JW_buf & KANA) != 0) {
	    if (xjc->currentcol > xjc->c0 + xjc->vst - 1) {
		xjc->linefeed1 = 0;
		break;
	    }
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move1(xjc, 0);
	    XDrawImageString(dpy, xjc->w1, currentGCR,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->w1, currentGCR,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w1, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	} else {
	    if (xjc->currentcol > xjc->c0 + xjc->vst - 1) {
		xjc->linefeed1 = 0;
		break;
	    }
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move1(xjc, 0);
	    XDrawImageString(dpy, xjc->w1, currentGC,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->w1, currentGC,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w1, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	}
    }
    XFlush(dpy);
    xjc->currentcol = currentcol_tmp;
    if (xjc->linefeed1)
	XClearArea(dpy, xjc->w1,
	    (xjc->c0 + xjc->vst - 1) * FontWidth(xjc), 0, 0, 0, False);
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 1
    && xjc->currentcol < (xjc->c0 + xjc->vst)) {
	xjc->mark_flag = 0;
	JWmark_cursor();
    }
}
/*****************************************************************************
	クライアント用変換行書き直し ルーチン
*****************************************************************************/
redraw_line2(x, width)
{
    register XJClient *xjc;
    register int *JW_buf;
    unsigned char cc[2];
    int currentcol_tmp = 0;
    int r_flag_tmp = 0;
    int b_flag_tmp = 0;
    int u_line_flag_tmp = 0;
    int start, end;
    GC currentGC, currentGCK, currentGCR;

    xjc = cur_p;
    currentcol_tmp = xjc->currentcol;
    start = x / FontWidth(xjc);
    end = start + width / FontWidth(xjc);
    if (check_kan(xjc->buf,start) == 2) start--;
    if (check_kan(xjc->buf,end) == 1) end++;
    for ( xjc->currentcol = start; xjc->currentcol < end; xjc->currentcol++) {
	JW_buf = xjc->buf + xjc->currentcol;
	if (*JW_buf & REV) {
	    r_flag_tmp = 0;
	    currentGC = xjc->reversegch;
	    currentGCK = xjc->reversegck;
	    currentGCR = xjc->reversegcr;
	} else {
	    r_flag_tmp = 0;
	    currentGC = xjc->gch;
	    currentGCK = xjc->gck;
	    currentGCR = xjc->gcr;
	}
	if (*JW_buf & BOLD)
	    b_flag_tmp = 1;
	else
	    b_flag_tmp = 0;
	if (*JW_buf & UNDER)
	    u_line_flag_tmp = 1;
	else
	    u_line_flag_tmp = 0;
	if ((*JW_buf & KAN1) != 0) {
	    if (xjc->currentcol > (xjc->c0 + xjc->c1 + xjc->vst - 2 - xjc->linefeed1)) {
		if (check_kan(xjc->buf,xjc->c0 + xjc->c1 + xjc->vst - xjc->linefeed1 - 1) == 1)
			xjc->linefeed2 = xjc->linefeed1 + 1;
		else
			xjc->linefeed2 = xjc->linefeed1;
		break;
	    }
	    cc[0] = (char)(*JW_buf) & 0xff;
	    cc[1] = (char)(*(JW_buf + 1)) & 0xff;
	    if (r_flag_tmp)
		check_move1(xjc, 1);
	    XDrawImageString16(dpy, xjc->w2, currentGCK,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString16(dpy, xjc->w2, currentGCK,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w2, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 2) * FontWidth(xjc), FontAscent(xjc)+1);
	    xjc->currentcol++;
	} else if ((*JW_buf & KANA) != 0) {
	    if (xjc->currentcol > (xjc->c0+xjc->c1+xjc->vst-1-xjc->linefeed1)) {
		xjc->linefeed2 = xjc->linefeed1;
		break;
	    }
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move1(xjc, 0);
	    XDrawImageString(dpy, xjc->w2, currentGCR,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->w2, currentGCR,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w2, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	} else {
	    if (xjc->currentcol > (xjc->c0+xjc->c1+xjc->vst-1-xjc->linefeed1)) {
		xjc->linefeed2 = xjc->linefeed1;
		break;
	    }
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move1(xjc, 0);
	    XDrawImageString(dpy, xjc->w2, currentGC,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->w2, currentGC,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w2, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	}
    }
    XFlush(dpy);
    xjc->currentcol = currentcol_tmp;
    if (xjc->linefeed2)
	XClearArea(dpy, xjc->w2,
	    (xjc->c0+xjc->c1+xjc->vst+xjc->linefeed1-1) * FontWidth(xjc),
	    0, 0, 0, False);
    if (xjc->cursor_flag == 1 && xjc->mark_flag == 1
    && xjc->currentcol < (xjc->c0 + xjc->c1 + xjc->vst - xjc->linefeed1)
    && xjc->currentcol >= (xjc->c0 + xjc->vst - xjc->linefeed1)) {
	xjc->mark_flag = 0;
	JWmark_cursor();
    }
}
/*****************************************************************************
	クライアント用変換行書き直し ルーチン
*****************************************************************************/
redraw_line3(x, width)
{
    register XJClient	*xjc;
    register int *JW_buf;
    unsigned char	cc[2];
    int		currentcol_tmp = 0;
    int		r_flag_tmp = 0;
    int		b_flag_tmp = 0;
    int		u_line_flag_tmp = 0;
    int		start;
    GC currentGC, currentGCK, currentGCR;

    xjc = cur_p;
    currentcol_tmp = xjc->currentcol;
    start = x / FontWidth(xjc);
    if (check_kan(xjc->buf,start) == 2) start--;
    for ( xjc->currentcol = start; xjc->currentcol < xjc->max_pos;
    xjc->currentcol++) {
	JW_buf = xjc->buf + xjc->currentcol;
	if (*JW_buf & REV) {
	    r_flag_tmp = 1;
	    currentGC = xjc->reversegch;
	    currentGCK = xjc->reversegck;
	    currentGCR = xjc->reversegcr;
	} else {
	    r_flag_tmp = 0;
	    currentGC = xjc->gch;
	    currentGCK = xjc->gck;
	    currentGCR = xjc->gcr;
	}
	if (*JW_buf & BOLD)
	    b_flag_tmp = 1;
	else
	    b_flag_tmp = 0;
	if (*JW_buf & UNDER)
	    u_line_flag_tmp = 1;
	else
	    u_line_flag_tmp = 0;
	if ((*JW_buf & KAN1) != 0) {
	    cc[0] = (char)(*JW_buf) & 0xff;
	    cc[1] = (char)(*(JW_buf + 1)) & 0xff;
	    if (r_flag_tmp)
		check_move(xjc, 1);
	    XDrawImageString16(dpy, xjc->w3, currentGCK,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (b_flag_tmp == 1)
		XDrawString16(dpy, xjc->w3, currentGCK,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), (XChar2b *)cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w3, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 2) * FontWidth(xjc), FontAscent(xjc)+1);
	    xjc->currentcol++;
	} else if ((*JW_buf & KANA) != 0) {
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move(xjc, 0);
	    XDrawImageString(dpy, xjc->w3, currentGCR,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->w3, currentGCR,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w3, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	} else {
	    cc[0] = (char)(*JW_buf) & 0xff;
	    if (r_flag_tmp)
		check_move(xjc, 0);
	    XDrawImageString(dpy, xjc->w3, currentGC,
		    xjc->currentcol * FontWidth(xjc),
		    FontAscent(xjc), cc, 1);
	    if (b_flag_tmp == 1)
	        XDrawString(dpy, xjc->w3, currentGC,
		    xjc->currentcol * FontWidth(xjc) + 1,
		    FontAscent(xjc), cc, 1);
	    if (u_line_flag_tmp == 1)
		XDrawLine(dpy, xjc->w3, xjc->gch,
		    xjc->currentcol * FontWidth(xjc), FontAscent(xjc) + 1,
		    (xjc->currentcol + 1) * FontWidth(xjc), FontAscent(xjc)+1);
	}
    }
    XFlush(dpy);
    xjc->currentcol = currentcol_tmp;
    if(xjc->cursor_flag == 1 && xjc->mark_flag == 1
	&& xjc->currentcol >= (xjc->c0 + xjc->c1 + xjc->vst - xjc->linefeed1)) {
	xjc->mark_flag = 0;
	JWmark_cursor();
    }
}
/*****************************************************************************
	入力行の前半が欠けている印書き直し ルーチン
			from xw_read()
*****************************************************************************/
redraw_note_fore()
{
    XDrawImageString(dpy, cur_p->wnf, cur_p->gch, 0, FontAscent(cur_p), dol_char, 1);
    XFlush(dpy);
}
redraw_note_back()
{
    XDrawImageString(dpy, cur_p->wnb, cur_p->gch, 0, FontAscent(cur_p), dol_char, 1);
    XFlush(dpy);
}

redraw_xj_all()
{
    register XJClient	*xjc;

    xjc = cur_p;

    redraw_window0();
    if (cur_x->dispmode == 2 || cur_x->dispmode == 0) {
	redraw_window3(0, xjc->max_pos * FontWidth(xjc));
    } else {
	redraw_line1(xjc->vst * FontWidth(xjc), xjc->max_pos * FontWidth(xjc));
	redraw_line2((xjc->vst + xjc->c0 - xjc->linefeed1) * FontWidth(xjc),
	    	     xjc->max_pos * FontWidth(xjc));
	redraw_line3((xjc->vst + xjc->c0 + xjc->c1 - xjc->linefeed2)
		       * FontWidth(xjc), xjc->max_pos * FontWidth(xjc));
    }
    if (xjc->note_fore != 0) JWM_note_fore();
    if (xjc->note_back != 0) JWM_note_back();
}
