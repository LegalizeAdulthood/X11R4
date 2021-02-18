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
 *	Original Source: ./lib/Wnn/uum/cursor.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>

#include "xwnmo.h"
#include "commonhd.h"

#include "sdefine.h"
#include "sheader.h"

int cursor_colum = 0;
static int cursor_upscreen = 1;
static int cursor_reverse = 0;
static int cursor_underline = 0;
static int cursor_bold = 0;	/* add by KUWA */
int cursor_invisible = 1;

throw_col(col)
int col;
{
#ifndef	X11
    throw_cur_raw(col % maxlength, (col / maxlength));
#else	X11
    throw_cur_raw(col, (col / maxlength));
#endif	!X11
    cursor_colum = col;
}

h_r_on()
{
    if(!cursor_reverse){
        h_r_on_raw();
        cursor_reverse = 1;
    }
}

h_r_off()
{
    if(cursor_reverse){
        h_r_off_raw();
        cursor_reverse = 0;
    }
    if(cursor_bold){
	b_s_on_raw();
    }
}

u_s_on()
{
    if(!cursor_underline){
        u_s_on_raw();
        cursor_underline = 1;
    }
}

u_s_off()
{
    if(cursor_underline){
        u_s_off_raw();
        cursor_underline = 0;
    }
}

b_s_on()	/* add by KUWA */
{
    if(!cursor_bold){
        b_s_on_raw();
        cursor_bold = 1;
    }
    if(cursor_reverse){
	h_r_on_raw();
    }
}

b_s_off()	/* add by KUWA */
{
    if(cursor_bold){
        b_s_off_raw();
        cursor_bold = 0;
    }
}

kk_cursor_invisible()
{
      if(cursor_invisible == 0){
          cursor_invisible_raw();
          cursor_invisible = 1;
      }
}
 
kk_cursor_normal()
{
    if(cursor_invisible){
        cursor_normal_raw();
        cursor_invisible = 0;
    }
}

reset_cursor_status()
{
    kk_cursor_normal();
    h_r_off();
    u_s_off();
}

set_cursor_status()
{
    if(cursor_invisible){
	cursor_invisible_raw();
    }else{
	cursor_normal_raw();
    }
    if(cursor_reverse){
	  h_r_on_raw();
    }
    if(cursor_underline){
	  u_s_on_raw();
    }
    throw_cur_raw(cursor_colum % maxlength, (cursor_colum / maxlength));
}

int clr_line_all()
{
    throw_c(0);
    clr_end_screen();
#ifndef	X11
    t_cont_line_note_delete();
#endif	!X11
}	    

#define STACKLENGTH 20

struct cursor_state {
    int colum;
    int reverse;
    int underline;
    int invisible;
};
static struct cursor_state savedupscreen[STACKLENGTH];
static int top = 0;


/* 現在のカーソルの位置を保存する*/
push_cursor()
{
    if(top >= STACKLENGTH ){
        printf("Error Cursor Stack");
    }
    savedupscreen[top].colum = cursor_colum;
    savedupscreen[top].reverse = cursor_reverse;
    savedupscreen[top].underline = cursor_underline;
    savedupscreen[top].invisible = cursor_invisible;
    top++;
    reset_cursor_status();
}

/* push_cursorで保存された位置にカーソルを飛ばす。*/
pop_cursor()
{
    if(top <= 0){
        printf("Error Empty Stack");
    }
    top--;
    cursor_normal_raw();
    cursor_colum = savedupscreen[top].colum;
    cursor_reverse = savedupscreen[top].reverse;
    cursor_underline = savedupscreen[top].underline;
    cursor_invisible = savedupscreen[top].invisible;
    set_cursor_status();
}


static int saved_cursor_rev;
static int saved_cursor_und;
push_hrus()
{
    saved_cursor_rev = cursor_reverse;
    saved_cursor_und = cursor_underline;
    h_r_off();
    u_s_off();
}

pop_hrus()
{
    if(saved_cursor_rev)
        h_r_on();
    if(saved_cursor_und)
        u_s_on();
}

set_hanten_ul(x,y)
int x,y;
{
    if(!x)h_r_off();
    if(!y)u_s_off();
    if(x)h_r_on();
    if(y)u_s_on();
}

set_bold(x)	/* add by KUWA */
int x;
{
    if(x)b_s_on();
    flush();
}

reset_bold(x)	/* add by KUWA */
int x;
{
    if(x)b_s_off();
    flush();
}
