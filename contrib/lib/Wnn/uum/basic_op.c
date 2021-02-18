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

extern int cursor_invisible;

redraw_nisemono()
{
    t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
}


int kana_in_w_char_msg(msg , mode , buffer , buflen)
w_char *msg;
int mode;
w_char *buffer;
int buflen;
{
  struct buf c_b1, *c_btmp;
  int msg_len = Strlen(msg);
  c_btmp = c_b;
  c_b = &c_b1; 
  if((c_b->buffer =(w_char *)malloc((buflen  + msg_len)* 2)) == NULL){
      print_msg_getc("Mallocに失敗しました。(如何)");
      return(-1);
  }
  Strcpy(c_b->buffer , msg);
  c_b->t_m_start = Strlen(c_b->buffer);
  Strcpy(c_b->buffer + msg_len , buffer);
  c_b->buf_msg = "";

  c_b->hanten = mode;
  c_b->start_col = 0;
  c_b->maxlen = Strlen(c_b->buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen + msg_len;
  set_screen_vars_default();
  c_b->redraw_fun = redraw_nisemono;
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  kill_buffer_offset = 0;

  init_screen();
  if(buffer_in() == -1){
      free(c_b->buffer);
      c_b = c_btmp;
      return(-1);
  }
  free(c_b->buffer);
  Strncpy(buffer , c_b->buffer + msg_len , c_b->maxlen - msg_len);
  buffer[c_b->maxlen - msg_len] = 0;
  c_b = c_btmp;
  return(0);
}

/** bufferを使って、仮名文字列を入力します。
kana_in_w_char_msgとの違いは、
msgが、charであることと、横にスクロールする時には、このメッセージを除いて
スクロールすること。
*/
int  kana_in(msg, mode, buffer, buflen)
char *msg;
int mode;
w_char *buffer;
int buflen;
{
  struct buf c_b1, *c_btmp;

  c_btmp = c_b;
  c_b = &c_b1; 

  c_b->buffer = buffer;
  c_b->buf_msg = msg;

  c_b->hanten = mode;
  c_b->start_col = strlen(msg);
  c_b->maxlen = Strlen(buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen;
  c_b->t_m_start = -2;
  set_screen_vars_default();
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = NULL;
  kill_buffer_offset = 0;

  init_screen();
  if(buffer_in() == -1){
      c_b = c_btmp;
      return(-1);
  }
  c_b->buffer[c_b->maxlen] = 0;
  c_b = c_btmp;
  return(0);
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
  int ignored = 0;
  int not_redrawtmp = not_redraw;
  int tmp_send = 0;

  extern int kakutei();		/* ADD KURI */
  extern int send_string();	/* ADD KURI */
  extern int return_it();	/* ADD KURI */
  
  not_redraw = 0;
  while(1){

    if(cursor_invisible && !cursor_invisible_fun){
	throw_col(maxlength - 1);
	flush();
    }
    c = ((romkan = romkan_getc()) & 0x0000ffff);
    if(cursor_invisible && !cursor_invisible_fun){
	t_throw();
    }

    if(isSPCL(romkan)){
	if(romkan == REDRAW){
		(*c_b->redraw_fun)();
		continue;
	} else if(romkan == CHMSIG) { /* mode changed */
		disp_mode();
		continue;
	} else if(romkan == NISEBP) { /* error */
		ring_bell();
		continue;
/*
	} else if(romkan == SENDCH){
		if(empty_modep()){
			c = ((romkan = romkan_getc()) & 0x0000ffff);
			tmp_send = 1;
		} else
			continue;
*/
	} else continue; /* if (romkan == EOLTTR) */
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
      continue;
    }
	/* 偽物のコントロールコードは画面表示しないので後で来るrubout
	のコードも無視する必要がある。ignoredは無視すべき
	ruboutの個数を保持する。*/
    if (isNISE(romkan) && (ESCAPE_CHAR(c) ||  NOT_NORMAL_CHAR(c)) &&
	c != rubout_code ){
      ++ignored;
      continue;
    }

    if(c < TBL_SIZE &&  c_b->key_table[c]){
#ifdef nodef			/* H.T nuku*/
	if (c != rubout_code &&	/* ADD KURI */
	    (*c_b->key_table[c]) != kakutei &&
	    (*c_b->key_table[c]) != send_string &&
	    (*c_b->key_table[c]) != return_it)   /* あり得ない */
#endif
	/* KURI
	if (romkan_clear_tbl[c])
	*/
	if (c_b->rk_clear_tbl[c])
	    romkan_clear();
	ret = (*c_b->key_table[c])(c,romkan);
#ifdef nodef
	if (c != rubout_code &&	/* ADD KURI */
	    (*c_b->key_table[c]) != kakutei &&
	    (*c_b->key_table[c]) != send_string &&
	    (*c_b->ctrl_code_fun) != return_it)  /* あり得ない */
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
		if (henkan_on_kuten && c_b->t_c_p == c_b->maxlen) /* KURI */
		    henkan_if_maru(c);
	}
      }else{
	romkan_clear();
	ret = 0;
      }

    }else{
#ifdef nodef  /* H.T. */
      if ((*c_b->ctrl_code_fun) != return_it)	/* ADD KURI */
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
	return(-1);
    }
  }
  not_redraw = not_redrawtmp;
  return(0);
}

int
t_rubout(c, romkan)
int c, romkan;
{

  if(c_b->t_c_p != 0){
      backward();
      /* KURI 確定後より確定前の文字数が多いことがある
      偽物のラブアウトでもリドロウする。
      if(isNISE(romkan)){
	  delete_char1();
      }else{
	  t_delete_char();
      }
      */
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
    t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
    return(0);
}

extern int in_kuten();
extern int in_jis();

kuten()
{
    input_a_char_from_function(in_kuten);
    return(0);
}

jis()
{
    input_a_char_from_function(in_jis);
    return(0);
}

input_a_char_from_function(fun)
int (*fun)();
{
  int c;
  int ret;
  
  if((c = (*fun)()) != -1){
      if(c_b->key_in_fun){
	  ret = (*c_b->key_in_fun)(c , c);
      }else{
	  ret = insert_char(c);
	  t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
      }
  }
  redraw_line();
  return(ret);
}

int t_kill()
{
  kill_buffer_offset = min(c_b->maxlen - c_b->t_c_p , maxchg);
  Strncpy(kill_buffer , c_b->buffer + c_b->t_c_p , kill_buffer_offset);
  c_b->maxlen = c_b->t_c_p;
  t_redraw_move(c_b->t_c_p ,c_b->t_c_p, c_b->maxlen,1);
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
  c_b->vlen = maxlength * conv_lines - c_b->start_col - 3 - disp_mode_length ;
  c_b->vst = 0;
  c_b->duplicate = max((c_b->vlen + 3) >> 2 , 2);
}

quote()
{
    int c;
    c = romkan_next();
    if (empty_modep() && c_b->key_in_fun)
	return ((*c_b->key_in_fun)(c, c));
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    return(0);
}

