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
 *	Original Source: ./lib/Wnn/uum/basic_op.c
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
bufferの使い方とかbufferの内容がどのように画面に表示されるかとか
の詳しい説明は、buffer.hか又は、screen.cを参照して下さい。
このファイルは、baffer を制御するための基本関数で、
ユーザーインターフェース、登録時、その他の入力に共通するような関数を
定義しています。
*/
#include <stdio.h>
#include "jllib.h"
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"

/** bufferを使って、仮名文字列を入力します。
buflenはバッファーの長さ。modeは反転、下線の状態
bufferの中に初期文字列を入れて置いて下さい。
msgは、仮名入力行の先頭に表すメッセージ文(w_charであることに注意)
横にスクロールする時には、このメッセージも一緒にスクロールします
これは、bufferを使う高レベルのかな入力ルーチンです。
更に高度な指定をしたい人は、buffer_inを用いて下さい。
*/

redraw_nisemono()
{
    t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
    check_scroll();
}


/** かな入力を行い、画面制御するための基本ルーチン
グローバル変数c_bの指す所(struct buf)に、種種な初期設定をしてから
これを呼んで下さい。
文字列は、c_b->bufferに入って終了します。
c_b->maxlenに文字列の長さが入ります。
*/
int buffer_in()
{
  int c;
  int ret;
  int romkan;
#ifndef	WNMO
  int ignored = 0;
#endif	!WNMO
  int not_redrawtmp = not_redraw;
  int tmp_send = 0;

  extern int kakutei();		/* ADD KURI */
  extern int send_string();	/* ADD KURI */
  extern int return_it();	/* ADD KURI */
  
#ifdef	WNMO
  if (c_c->command_func != 0){
    if ((ret = (*c_c->command_func)()) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    } else if (ret == -1){
	not_redraw = not_redrawtmp;
	c_c->buffer_in = 0;
	return(BUFFER_IN_CONT);
    }
  }
#endif	WNMO
  if (c_c->buffer_in == 0) {
    if (not_redraw != 2)	/* 登録、検索、削除は、リドローしない */
      not_redraw = 0;
    ignored = 0;
    c_c->buffer_in = 1;
  }
  while(1){
    c = ((romkan = romkan_getc()) & 0x0000ffff);

    if(isSPCL(romkan)){
	if(romkan == REDRAW){
		(*c_b->redraw_fun)();
#ifdef	WNMO
		not_redraw = not_redrawtmp;
		return(BUFFER_IN_CONT);
/********************/
#else	WNMO
		continue;
#endif	WNMO
	} else if(romkan == CHMSIG) { /* mode changed */
		disp_mode();
#ifdef	WNMO
		not_redraw = not_redrawtmp;
		return(BUFFER_IN_CONT);
/********************/
#else	WNMO
		continue;
#endif	WNMO
	} else if(romkan == NISEBP) { /* error */
		ring_bell();
#ifdef	WNMO
		not_redraw = not_redrawtmp;
		return(BUFFER_IN_CONT);
/********************/
#else	WNMO
		continue;
#endif	WNMO
/*
	} else if(romkan == SENDCH){
		if(empty_modep()){
			c = ((romkan = romkan_getc()) & 0x0000ffff);
			tmp_send = 1;
		} else
#ifdef	WNMO
		not_redraw = not_redrawtmp;
		return(BUFFER_IN_CONT);
#else	WNMO
		continue;
#endif	WNMO
*/
	} else {
#ifdef	WNMO
		not_redraw = not_redrawtmp;
		return(BUFFER_IN_CONT);
/********************/
#else	WNMO
		continue; /* if (romkan == EOLTTR) */
#endif	WNMO
	}
    }
    if (ignored){
      if (isNISE(romkan)) {
	if (c == rubout_code) {
	  --ignored;
	} else {
	  ++ignored;
	}
      } else {
	ignored = 0;
      }
#ifdef	WNMO
		not_redraw = not_redrawtmp;
		return(BUFFER_IN_CONT);
/********************/
#else	WNMO
		continue;
#endif	WNMO
    }
	/* 偽物のコントロールコードは画面表示しないので後で来るrubout
	のコードも無視する必要がある。ignoredは無視すべき
	ruboutの個数を保持する。*/
    if (isNISE(romkan) && (ESCAPE_CHAR(c) ||  NOT_NORMAL_CHAR(c)) &&
	c != rubout_code ){
      ++ignored;
#ifdef	WNMO
	not_redraw = not_redrawtmp;
	return(BUFFER_IN_CONT);
/********************/
#else	WNMO
	continue;
#endif	WNMO
    }

    if(c < TBL_SIZE &&  c_b->key_table[c]){
#ifdef	nodef
	if (c != rubout_code &&	/* ADD KURI */
	    (*c_b->key_table[c]) != kakutei &&
	    (*c_b->key_table[c]) != send_string &&
	    (*c_b->key_table[c]) != return_it)
#endif
	/* KURI
	if (romkan_clear_tbl[c])
	*/
	if (c_b->rk_clear_tbl[c])
	    romkan_clear();
	ret = (*c_b->key_table[c])(c,romkan);
#ifdef	nodef
	if (c != rubout_code &&	/* ADD KURI */
	    (*c_b->key_table[c]) != kakutei &&
	    (*c_b->key_table[c]) != send_string &&
	    (*c_b->key_table[c]) != return_it)
	    romkan_clear();
#endif
    }else if(c >= 0x80 && c < 0xff){
	if(tmp_send){
		ret = return_it(c, romkan);
		tmp_send = 0;
	} else {
		ret = 0;
		ring_bell();
	}
    }else if(!(ESCAPE_CHAR(c)) || c == 0x09){	/* TAB is not ESCAPE char */
      if(c_b->maxlen < c_b->buflen){
	if(tmp_send){
		ret = return_it(c);
		tmp_send = 0;
	} else if(c_b->key_in_fun){
		ret = (*c_b->key_in_fun)(c , romkan);
	} else {
		ret = insert_char(c);
		/* change KURI
		t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,1);
		*/
		t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
#ifdef	X11
		check_scroll();
		kk_cursor_normal();
#endif	X11
		if (henkan_on_kuten && c_b->t_c_p == c_b->maxlen) /* KURI */
		    henkan_if_maru(c);
	}
      }else{
	romkan_clear();
	ret = 0;
      }

    }else{
#ifdef	nodef	/* H.T. */
      if ((*c_b->key_table[c]) != return_it)	/* ADD KURI */
	      romkan_clear();	/* ADD KURI */
#endif
      if(c_b->ctrl_code_fun){
	ret = (*c_b->ctrl_code_fun)(c,romkan);
      }else{
	ret = 0;
	ring_bell();
      }
    }
    if(ret == 1){
      break;
    }else if(ret == -1){
	not_redraw = not_redrawtmp;
#ifdef	WNMO
	c_c->buffer_in = 0;
#endif	WNMO
	return(-1);
    }
    not_redraw = not_redrawtmp;
    return(BUFFER_IN_CONT);
/********************/
  }
  not_redraw = not_redrawtmp;
#ifdef	WNMO
  c_c->buffer_in = 0;
#endif	WNMO
  return(0);
}

int
t_rubout(c, romkan)
int c, romkan;
{

  if(c_b->t_c_p != 0){
      backward();
      t_delete_char();
  }
  return(0);
}


int
delete_char1()
{
  
  if(c_b->t_c_p < c_b->maxlen){	
    Strncpy(c_b->buffer + c_b->t_c_p , c_b->buffer + c_b->t_c_p + 1,
	    c_b->maxlen - c_b->t_c_p - 1);
    c_b->maxlen -= 1;
  }
}

t_delete_char()
{
    delete_char1();
#ifdef	X11
    kk_cursor_invisible();
#endif	X11
    t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
#ifdef	X11
    check_scroll();
    kk_cursor_normal();
#endif	X11
    return(0);
}

extern int in_kuten();
extern int in_jis();

kuten()
{
#ifdef	WNMO
    static struct wnmo_client *c_c_sv = 0;
    int ret;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(0);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	c_c->command_func = kuten;
    }
    if ((ret = input_a_char_from_function(in_kuten)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    c_c_sv = 0;
    c_c->command_func = NULL;
#else	WNMO
    input_a_char_from_function(in_kuten);
#endif	WNMO
    return(0);
}

jis()
{
#ifdef	WNMO
    static struct wnmo_client *c_c_sv = 0;
    int ret;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(0);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	c_c->command_func = jis;
    }
    if ((ret = input_a_char_from_function(in_jis)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    c_c_sv = 0;
    c_c->command_func = NULL;
#else	WNMO
    input_a_char_from_function(in_jis);
#endif	WNMO
    return(0);
}

input_a_char_from_function(fun)
int (*fun)();
{
  int c;
  int ret;
  
  if((c = (*fun)()) != -1){
#ifdef	WNMO
      if (c == BUFFER_IN_CONT) {
	  return(BUFFER_IN_CONT);
      }
#endif	WNMO
      if(c_b->key_in_fun){
	  ret = (*c_b->key_in_fun)(c , c);
      }else{
	  ret = insert_char(c);
	  t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
#ifdef	X11
	  check_scroll();
#endif	X11
      }
  }
  return(ret);
}

int t_kill()
{
  kill_buffer_offset = min(c_b->maxlen - c_b->t_c_p , maxchg);
  Strncpy(kill_buffer , c_b->buffer + c_b->t_c_p , kill_buffer_offset);
  c_b->maxlen = c_b->t_c_p;
  t_redraw_move(c_b->t_c_p ,c_b->t_c_p, c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

int t_yank()
{
  if(kill_buffer_offset <= c_b->buflen - c_b->t_c_p){
    Strncpy(c_b->buffer + c_b->t_c_p + kill_buffer_offset,
	      c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
    Strncpy(c_b->buffer + c_b->t_c_p,
	      kill_buffer, kill_buffer_offset);
    c_b->maxlen += kill_buffer_offset;
    t_redraw_move(c_b->t_c_p + kill_buffer_offset, c_b->t_c_p, c_b->maxlen,0);
#ifdef	X11
  check_scroll();
#endif	X11
  }
}  

int t_ret()
{
  return(1);
}

int
t_quit()
{
  return(-1);
}


int 
c_top()
{
  if(c_b->t_m_start < 0){
    t_move(0);
  }else{
    t_move(c_b->t_m_start);
  }
  return(0);
}

int 
c_end()
{
    t_move(c_b->maxlen);
    return(0);
}

int t_jmp_backward()
{
  int k;
  
  for(k = 1 ; k < touroku_bnst_cnt; k++){
    if(touroku_bnst[k] >= c_b->t_c_p){
      if(touroku_bnst[k - 1] >= c_b->t_m_start){
	  t_move(touroku_bnst[k - 1]);
      }else{
	  c_top();
      }
      return(0);
    }
  }
  backward();
  return(0);
}

int t_jmp_forward()
{
  int k;
  
  for(k = 1 ; k < touroku_bnst_cnt; k++){
    if(touroku_bnst[k] > c_b->t_c_p){
	t_move(touroku_bnst[k ]);
	return(0);
    }
  }
  forward();
  return(0);
}


int
forward()
{
  if(c_b->t_c_p < c_b->maxlen){
      t_move(c_b->t_c_p + 1);
  }
  return(0);
}

int
backward()
{
  if(c_b->t_c_p >= 1 && c_b->t_c_p > c_b->t_m_start){
      t_move(c_b->t_c_p - 1);
  }
  return(0);
}

int
insert_char(c)
{
    if(c_b->maxlen < c_b->buflen){
	Strncpy(c_b->buffer + c_b->t_c_p + 1, c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
	c_b->maxlen += 1;
	c_b->buffer[c_b->t_c_p] = c;
    }
    return(0);
}
int
insert_string(string, len)
w_char *string;
int len;
{
  if(c_b->maxlen + len <= c_b->buflen){
    Strncpy(c_b->buffer + c_b->t_c_p + len, c_b->buffer + c_b->t_c_p , c_b->maxlen - c_b->t_c_p);
    Strncpy(c_b->buffer + c_b->t_c_p, string , len);
    c_b->maxlen += len;
  }
}

/** vlenとduplicateを適当と思われる状態に設定します。*/
set_screen_vars_default()
{
#ifndef	X11
  c_b->vlen = maxlength * conv_lines - c_b->start_col - 3 - disp_mode_length ;
  c_b->vst = 0;
  c_b->duplicate = max((c_b->vlen + 3) >> 2 , 2);
#else	X11
  c_b->vlen = maxchg * 2;
  c_b->vst = 0;
  c_b->duplicate = 0;
#endif	!X11
}

quote()
{
    int c;
#ifdef	WNMO
    static struct wnmo_client *c_c_sv = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(0);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	c_c->command_func = quote;
	return(BUFFER_IN_CONT);
    }
    /*
    c = romkan_getc();
    */
    c = rk_getkey();
#else	WNMO
    c = romkan_next();
#endif	WNMO
#ifndef	X11
    if (empty_modep() && c_b->key_in_fun)
	return ((*c_b->key_in_fun)(c, c));
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
#else	X11
    if (empty_modep()) {
	if (send_ascii_char) {
	    c_c_sv = 0;
	    c_c->command_func = NULL;
	    return (return_it_if_ascii(c));
	} else {
	    insert_char_and_change_to_insert_mode(c);
	}
    } else {
    	insert_char(c);
    	t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
	check_scroll();
    }
#endif	!X11
    romkan_clear();
    c_c_sv = 0;
    c_c->command_func = NULL;
    return(0);
}

