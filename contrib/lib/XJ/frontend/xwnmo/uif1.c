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
 *	Original Source: ./lib/Wnn/uum/uif1.c
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

jutil_c()
{
#ifdef	WNMO
  int ret;
  static struct wnmo_client *c_c_sv = 0;
#ifndef	X11
  extern  not_move_flag;
  disp_under();
  not_move_flag = 1;
#endif	X11

  if(!isconect_jserver()){
    return (0);
  }

  if (c_c_sv != 0 && c_c != c_c_sv) {
    ring_bell();
    return (-1);
  }
  if (c_c_sv == 0) {
    c_c_sv = c_c;
    c_c->command_func = jutil_c;
#endif	WNMO
#ifndef	X11
  push_cursor();
#endif	X11
  }

/*
  if(jl_isconnect(bun_data_)){
    if ((ret = jutil()) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
  } else {
    print_msg_getc(" ｊｓｅｒｖｅｒとの接続が切れています。");
  }
*/
  if ((ret = jutil()) == BUFFER_IN_CONT) {
    return(BUFFER_IN_CONT);
  }
#ifdef	WNMO
  c_c_sv = 0;
  c_c->command_func = NULL;
#ifndef	X11
  restre_disp_under();
  not_move_flag = 0;
#endif	!X11
#endif	WNMO
#ifndef	X11
  pop_cursor();
  t_print_l();
#endif	!X11
#ifndef	WNMO
  if (empty_modep()) {	       /* added by T.S 10 Jan. '88 */
    kk_restore_cursor();
  }
#endif	WNMO
  return(0);
}

touroku_c()
{
#ifdef	WNMO
  int ret;
  static struct wnmo_client *c_c_sv = 0;
#ifndef	X11
  disp_under0();
#endif	!X11
  if(!isconect_jserver()){
    return (0);
  }

  if (c_c_sv != 0 && c_c != c_c_sv) {
    ring_bell();
    return (-1);
  }
  if (c_c_sv == 0) {
#ifndef	X11
      disp_under1();
#endif	!X11
      c_c_sv = c_c;
#ifndef	X11
      not_move_flag = 1;
#endif	!X11
      c_c->command_func = touroku_c;
#endif	WNMO
#ifndef	X11
  push_cursor();
#endif	!X11
  }
/*
  if(jl_isconnect(bun_data_)){
#ifdef	WNMO
    if ((ret = touroku()) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
#else	WNMO
    touroku();
#endif	WNMO
  } else {
    print_msg_getc(" ｊｓｅｒｖｅｒとの接続が切れています。");
  }
*/
  if ((ret = touroku()) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
  }
#ifdef	WNMO
#ifndef	X11
  clr_line_all();
#endif	!X11
  c_c_sv = 0;
  c_c->command_func = NULL;
#ifndef	X11
  restre_disp_under();
  not_move_flag = 0;
#endif	!X11
#endif	WNMO
#ifndef	X11
  pop_cursor();
  t_print_l();
#endif	!X11
  if (ret == -1)
	return(-1);
  return(0);
}

/** jserverとのコネクションを再確立する。*/
#ifdef	X11
static char	*para_exec = "実行";
#endif	X11

reconnect_jserver_body()
{
  int ret, k;
  int c;
  static int c_p = 0;
  static Window w;
  static struct wnmo_client *c_c_sv = 0;
  static int j_con_step = 0;
  static char *message1 = " 接 続 ";
  static char *message2 = " 接続ホスト名を入力して下さい";
  static char hostname[32];
  extern int t_quit(), henkan_off();

  if (c_c_sv != 0 && c_c != c_c_sv) {
    ring_bell();
    return (-1);
  }
  if (c_c_sv == 0) {
    for (k = 0; k < 32; hostname[k++] = NULL);
    w = jfew->ichi->nyuu_w;
    c_c_sv = c_c;
    c_p = 0;
  }
  if (j_con_step == 0) {
    if (init_ichiran(NULL, 0, -1,message1, message2, para_exec, 
			32, NYUURYOKU) == -1) {
	ring_bell();
	c_c_sv = 0;
	return(-1);
    }
    sprintf(hostname, "%s", jserver_name_in_uumrc);
    c_p = strlen(hostname);
    JW3Mputc(hostname, w,0,0,0);
    JW3mark_cursor(w, c_p);
    j_con_step++;
    return(BUFFER_IN_CONT);
  }
  if (j_con_step == 1) {
    c = rk_getkey();
    if (select_return == -2) {
      end_ichiran();
      select_return = -1;
      c_c_sv = 0;
      j_con_step = 0;
      romkan_clear();
      return(-1);
    }
    if (c < 256){
      if ((c == ESC) || (t_quit == main_table[5][c])){
	end_ichiran();
	c_c_sv = 0;
        romkan_clear();
	return(-1);
      } else if (henkan_off == main_table[5][c]) {
	ring_bell();
	return(BUFFER_IN_CONT);
      } else if (c == rubout_code && c_p) {
	JW3remark_cursor(w, c_p);
	hostname[--c_p] = NULL;
	JW3mark_cursor(w, c_p);
	return(BUFFER_IN_CONT);
      } else if (c == NEWLINE || c == CR) {
	if(c_p == 0) {
	  return(BUFFER_IN_CONT);
	} else {
	  end_ichiran();
	  goto NEXT;
	}
      } else if(c > 20 && c < 128) {
	JW3remark_cursor(w, c_p);
	hostname[c_p++] = (char)c;
	JW3Mputc(hostname, w, 0, 0, 0);
	JW3mark_cursor(w, c_p);
	return(BUFFER_IN_CONT);
      } else {
	ring_bell();
	return(BUFFER_IN_CONT);
      }
    } else {
      ring_bell();
      return(BUFFER_IN_CONT);
    }
  }
NEXT:
  strcpy(jserver_name_in_uumrc, hostname);

/*
  print_msg(" 接続中・・・");
*/
  if(connect_jserver(1) < 0){
    print_msg_getc(" 接続がうまくいきません。");
  } else if(!jl_isconnect(bun_data_)) {
    print_msg_getc(" 接続に失敗しました。");
  }
  else {
    print_msg_getc(" 接続が完了しました。");
  }
  c_c_sv = 0;
  j_con_step = 0;
  romkan_clear();
  return(0);
}
