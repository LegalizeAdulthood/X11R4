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
 *	Original Source: ./lib/Wnn/uum/jikouho.c
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
#include "jllib.h"
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h" /* defines of CHMSIG, NISEBP */
#include "buffer.h"



/* 次候補を一つずつ表示する関数
num = 1 のとき次候補を、num = -1 のとき前候補を表示する */
jikouho(num)
int num;
{
  int tmp;	/* ADD KURI */
  int mae_p;	/* ADD KURI 89/6/7 */
    /* 注目している文節の次候補がまだ用意されていない時は
       まずそれを用意する。*/
  jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
  if (bunsetsu_env[cur_bnst_] == bunsetsu_env[cur_bnst_+1])
	mae_p = WNN_USE_ZENGO;
  else
	mae_p = WNN_USE_MAE;
  if (jl_zenkouho_bun(bun_data_) != cur_bnst_ || jl_zenkouho_daip(bun_data_)){
      if (jl_zenkouho(bun_data_, cur_bnst_, mae_p, WNN_UNIQ) < 0) {
	errorkeyin();
	return(-1);
      }
  }

  if (num == 1) 
      jl_next(bun_data_);
  else if (num == -1) 
      jl_previous(bun_data_);
  else 
      return(-1);
  make_kanji_buffer(cur_bnst_);

  tmp = c_b->t_b_st;		/* ADD KURI */
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)]; /* ADD KURI */
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[cur_bnst_], min(tmp, c_b->t_b_st), c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

/* 次候補を一つずつ表示する関数
num = 1 のとき次候補を、num = -1 のとき前候補を表示する */
jikouho_dai(num)
int num;
{
  int tmp;
  int mae_p;
    /* 注目している文節の次候補がまだ用意されていない時は
       まずそれを用意する。*/
  jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
  if (bunsetsu_env[cur_bnst_] == bunsetsu_env[dai_end(bun_data_, cur_bnst_)])
	mae_p = WNN_USE_ZENGO;
else
	mae_p = WNN_USE_MAE;
  if (jl_zenkouho_bun(bun_data_) != cur_bnst_ || !jl_zenkouho_daip(bun_data_)){
      if (jl_zenkouho_dai(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_),
			  mae_p, WNN_UNIQ) < 0) {
	errorkeyin();
	return(-1);
      }
  }

  if (num == 1) 
      jl_next_dai(bun_data_);
  else if (num == -1) 
      jl_previous_dai(bun_data_);
  else 
      return(-1);
  make_kanji_buffer(cur_bnst_);
  for (tmp = cur_bnst_+1; tmp < jl_bun_suu(bun_data_); tmp++)
	bunsetsu_env[tmp] = jl_env_get(bun_data_);
  tmp = c_b->t_b_st;
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[cur_bnst_], min(tmp, c_b->t_b_st), c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}
    
    

int jikouho_c()
{
    jikouho(1);
    return(0);
}

int zenkouho_c()
{
    jikouho(-1);
    return(0);
}

int jikouho_dai_c()
{
    jikouho_dai(1);
    return(0);
}

int zenkouho_dai_c()
{
    jikouho_dai(-1);
    return(0);
}

select_jikouho()
{
    int ret;
    static struct wnmo_client *c_c_sv = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(0);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	c_c->command_func = select_jikouho;
    }
    if((ret = select_jikouho1(WNN_SHO)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    } else if (ret == -1){
	if (cur_p->visible == 3) {
	    t_print_l();
	}
    }else{
	kk_cursor_invisible();
	redraw_line();
	kk_cursor_normal();
    }
    c_c_sv = 0;
    c_c->command_func = NULL;
    check_scroll();
    return(0);
}

select_jikouho_dai()
{
    int ret;
    static struct wnmo_client *c_c_sv = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(0);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	c_c->command_func = select_jikouho_dai;
    }
    if((ret = select_jikouho1(WNN_DAI)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    } else if (ret == -1){
	if (cur_p->visible == 3) {
	    t_print_l();
	}
    }else{
	kk_cursor_invisible();
	redraw_line();
	kk_cursor_normal();
    }
    c_c_sv = 0;
    c_c->command_func = NULL;
    check_scroll();
    return(0);
}

/* add by KUWA */
dai_top(buf, bnst)	
struct wnn_buf *buf;
int bnst;
{
    if(bnst >= jl_bun_suu(buf)) return(jl_bun_suu(buf));
    for(;bnst >= 0 && !jl_dai_top(buf, bnst); bnst--);
    return(bnst);
}
/* add by KUWA */

dai_end(buf, bnst)
struct wnn_buf *buf;
int bnst;
{
    for(bnst++;bnst < jl_bun_suu(buf) && !jl_dai_top(buf, bnst); bnst++);
    return(bnst);
}

select_jikouho1(daip)
int daip;
{
  static char *j_retu[MAXJIKOUHO];
  static char jikouho_heap[MAXJIKOUHO * 10];
  int c;
  int tmp;
  static char *jikouho_title = "次候補選択(小文節)";
  static char *jikouho_dai_title = "次候補選択(大文節)";
  static int jikouho_step = 0;
  static struct wnmo_client *c_c_sv = 0;


  if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
  }
  if (jikouho_step == 0) {
    jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
    if (jl_zenkouho_bun(bun_data_) != cur_bnst_  ||
        jl_zenkouho_daip(bun_data_) != daip ){
        if(daip){
  	    if(jl_zenkouho_dai(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_),
  			 WNN_USE_ZENGO, WNN_UNIQ) == -1) {
		errorkeyin();
		return(-1);
	    }
        }else{
  	    if(jl_zenkouho(bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ) == -1) {
		errorkeyin();
		return(-1);
	    }
        }
    }
    if(make_jikouho_retu(jikouho_heap, MAXJIKOUHO * 10, j_retu, MAXJIKOUHO) == -1){
      print_msg_getc("候補が多過ぎて次候補が取り出せません。");
      return(-1);
    }
    c_c_sv = c_c;
    jikouho_step++;
  }

  if((c = xw_select_one_jikouho(j_retu,jl_zenkouho_suu(bun_data_),
			     jl_c_zenkouho(bun_data_),
			     (daip ? jikouho_dai_title : jikouho_title)
			     ,JIKOUHO, main_table[4])) == -1){
    jikouho_step = 0;
    c_c_sv = 0;
    return(-1);
  } else if (c == BUFFER_IN_CONT) {
    return(BUFFER_IN_CONT);
  }
  if(daip == WNN_SHO){
      jl_set_jikouho(bun_data_, c);
  }else{
      jl_set_jikouho_dai(bun_data_, c);
  }
  make_kanji_buffer(cur_bnst_);
  for (tmp = cur_bnst_+1; tmp < jl_bun_suu(bun_data_); tmp++)
	bunsetsu_env[tmp] = jl_env_get(bun_data_);
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_c_p = bunsetsu[cur_bnst_];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  jikouho_step = 0;
  c_c_sv = 0;
  return(0);
}


make_jikouho_retu(h,h_m,r,r_m)
char *h;
int h_m;
int r_m;
char *r[];
{
  int k;
  char *c = h;
  wchar tmp[256];
  char tmp1[256];

  if(r_m <= jl_zenkouho_suu(bun_data_)) return(-1);

  for(k = 0 ; k < jl_zenkouho_suu(bun_data_); k++){
    r[k] = c;
    tmp[0] = 0;
    jl_get_zenkouho_kanji(bun_data_, k, tmp);

    sStrcpy(tmp1, tmp);
    if(c + strlen(tmp1) + 1 >= h_m + h){
      return(-1);
    }
    set_escape_code(tmp1);	/* add by KUWA */
    strcpy(c,tmp1);
    c += strlen(tmp1) + 1;
  }
  return(0);
}
  
set_escape_code(x)
char *x;
{
    int m;
    char buf[512];
    char *tmp;

    for(m = 0,tmp = x;*tmp;tmp++) {
	if (ESCAPE_CHAR((*tmp & 0xff))) {
	    buf[m++] = '^';
	    if(*tmp == 0x7f)buf[m++] = '?';
	    else buf[m++] = (*tmp + 'A' - 1);
	} else {
	    buf[m++] = *tmp;
	}
    }
    buf[m] = NULL;
    strcpy(x, buf);
}


/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
