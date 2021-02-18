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
 *	Original Source: ./lib/Wnn/uum/uif.c
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
/*
 このファイルは、標準ユーザーインターフェースの関数を定義します。
buffer_inを用いているので、そちらの方も参照して下さい。
それぞれの関数の動作は、key_bind.cの中でどこにバインドされているか
みる事により、分かると思います。
モードが存在します(henkan_mode)が、あまり利用しないように務めています。
*/

#include "xwnmo.h"
#include <stdio.h>
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h" /* defines of CHMSIG, NISEBP */
#include "buffer.h"


static int henkan_mode;
/*  0: select
    1: yomi_nyuuryoku
    2: bunsetu_nobasi
    3: kara
*/

redraw_nisemono_c()
{
    redraw_nisemono();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
}

clear_c_b()
{
  initialize_vars();
  if (bun_data_) jl_kill(bun_data_, 0, -1);
  c_b->buffer = input_buffer;
  c_b->buf_msg = "";
  c_b->start_col = 0;
  c_b->t_c_p = 0;
  c_b->maxlen = 0;
  c_b->buflen = maxchg;
  c_b->redraw_fun = redraw_nisemono_c;
  c_b->t_m_start = 0;
  c_b->t_b_st = 0;	/* add by KUWA */
  c_b->t_b_end = 0;	/* add by KUWA */
  set_screen_vars_default();
  change_to_empty_mode();
  init_screen();
}


/** ここからの関数は、 c_b->key_in_fun, c_b->ctrl_code_fun に
モードに応じて設定するものです。設定の仕方は、この下のモード変更の関数群を
みて下さい。
*/

return_it(c)
int c;
{
}

return_it_if_ascii(c , nisemono)
int c;
int nisemono;  /* it means that what romkan returned is a nisemono character. */
{
    if((NORMAL_CHAR(c) || c == 0x09) && is_HON(nisemono)){ /* TAB is not ESCAPE char */
	c_b->maxlen = 1;
	c_b->buffer[0] = c;
	return(1);
    }else{
	insert_char_and_change_to_insert_mode(c);
	return(0);
    }
}

push_char_return(c,romkan)
int c,romkan;
{
  romkan_ungetc(romkan);
  return(1);
}

insert_char_and_change_to_insert_mode(c)
int c;
{
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    change_to_insert_mode();
#ifdef	X11
    check_scroll();
#endif	X11
    return(0);
}

/** ここからの関数は、モード変更の時に呼ばれるものです。
   設定するのは、key_table,key_in_fun,ctrl_code_fun,hantenです。	
*/

change_to_insert_mode()
{
  c_b->key_table = main_table[1];
  c_b->rk_clear_tbl =  romkan_clear_tbl[1];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten = 0x08 | 0x20;
  henkan_mode = 1;
  kk_cursor_normal();
}


change_to_empty_mode()
{
  c_b->key_table = main_table[3];
  c_b->rk_clear_tbl =  romkan_clear_tbl[3];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten =  0x08 | 0x20;
  henkan_mode = 3;
  throw_col(0);
  kk_cursor_normal();
  flush();
}

isconect_jserver()
{
    if (!jl_isconnect(bun_data_)) {
	connect_jserver(1);
    }
     if (!jl_isconnect(bun_data_)) {
	push_cursor();
	print_msg_getc(" jserver(?)");
	pop_cursor();
	t_print_l();
	return (0);
    }
    return(1);
}

initialize_vars()
{
  cur_bnst_ = 0;
}

int kill_c()
{
    t_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
}

int delete_c(c,romkan)
int c,romkan;
{
    t_delete_char(c , romkan);
    if((c_b->maxlen == 0) && is_HON(romkan)){
	change_to_empty_mode();
    }
}

int rubout_c(c , romkan)
{
  t_rubout(c, romkan);
  if((c_b->maxlen == 0) && is_HON(romkan)){
      change_to_empty_mode();
  }
}  



backward_c()
{
    if(!isconect_jserver()) {
	return(0);
    }
    if(c_b->t_c_p == c_b->t_m_start){
    }else{
	backward();
    }
    return(0);
}


insert_it_as_yomi()
{
    change_to_insert_mode();
    c_b->t_m_start = 0;
    cur_bnst_ = 0;
    t_print_l();
    if(bun_data_)
	jl_kill(bun_data_, 0, -1);	/* ADD KURI */
}

empty_modep()
{
    return(henkan_mode == 3);
}

insert_modep()
{
    return(henkan_mode == 1);
}

henkan_gop()
{
    return(henkan_mode == 0 || henkan_mode == 2);
}


check_empty_mode_keyin_fun()
{
    c_b->key_in_fun = NULL;
}

/** jserverとのコネクションを再確立する。*/
reconnect_jserver()
{
  return(0);
}

/** jserver との connection を切る。*/
disconnect_jserver()
{
  extern int save_env_id;

  if (bun_data_ != NULL && jl_isconnect(bun_data_)) {
     bun_data_->env->env_id = save_env_id;
     jl_close(bun_data_);
  }
  return(1);
}

/* MOVED from jhlp.c */

/** 変換オフ時のループ */

static wchar_t	_tatojis[] = {
	0xa1a1, 0xa1aa, 0xa1c9, 0xa1f4, 0xa1f0, 0xa1f3, 0xa1f5, 0xa1c7, 
	0xa1ca, 0xa1cb, 0xa1f6, 0xa1dc, 0xa1a4, 0xa1dd, 0xa1a5, 0xa1bf, 
	0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7, 
	0xa3b8, 0xa3b9, 0xa1a7, 0xa1a8, 0xa1e3, 0xa1e1, 0xa1e4, 0xa1a9, 
	0xa1f7, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7, 
	0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf, 
	0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7, 
	0xa3d8, 0xa3d9, 0xa3da, 0xa1ce, 0xa1ef, 0xa1cf, 0xa1b0, 0xa1b2, 
	0xa1ae, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7, 
	0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef, 
	0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7, 
	0xa3f8, 0xa3f9, 0xa3fa, 0xa1d0, 0xa1c3, 0xa1d1, 0xa1b1, 0x007f
};

static wchar save_yomi[512];
static int cur_bnst_sv;

static
wchar
_to_han_or_hira(wc)
register wchar wc;
{
    register int i;

    for ( ; ; ) {
	if ((wc & 0xff00) == 0xa500)	/* 片仮名 */
	    return (wc & 0xa4ff);

	for ( i = 0; i < 16*6; i++) {
	    if (wc == _tatojis[i])	/* ASCII */
		return (i + ' ');
	}
	return(wc);
    }
}


static
to_han_or_hira()
{
    register wchar *yomi_buf;
    register int moji_suu;
    register int i;

    cur_bnst_sv = cur_bnst_;

    moji_suu =  c_b->maxlen - c_b->t_m_start;
    save_yomi[moji_suu] = 0;
    if(moji_suu > 0){
	yomi_buf = c_b->buffer + c_b->t_m_start;
	Strncpy(save_yomi, yomi_buf, moji_suu);
	for (i = 0; i < moji_suu; i++) {
	    *yomi_buf++ = _to_han_or_hira(save_yomi[i]);
	}
    }
    yomi_buf[moji_suu] = 0;
}

static
recover_to_han_or_hira(yomi)
register wchar *yomi;
{
    int i;
    if (cur_bnst_sv == cur_bnst_) {
	Strcpy(yomi, save_yomi);
    } else if (cur_bnst_sv < cur_bnst_) {
	i = jl_yomi_len(bun_data_, cur_bnst_sv, cur_bnst_);
	Strcpy(yomi, save_yomi + i);
    } else {
        jl_get_yomi(bun_data_, cur_bnst_, cur_bnst_sv, yomi);
	yomi[jl_yomi_len(bun_data_, cur_bnst_, cur_bnst_sv)] = 0;
	Strcat(yomi, save_yomi);
    }
}
