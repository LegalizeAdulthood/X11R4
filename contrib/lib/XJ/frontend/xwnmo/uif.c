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
 ���Υե�����ϡ�ɸ��桼�������󥿡��ե������δؿ���������ޤ���
buffer_in���Ѥ��Ƥ���Τǡ�����������⻲�Ȥ��Ʋ�������
���줾��δؿ���ư��ϡ�key_bind.c����Ǥɤ��˥Х���ɤ���Ƥ��뤫
�ߤ���ˤ�ꡢʬ����Ȼפ��ޤ���
�⡼�ɤ�¸�ߤ��ޤ�(henkan_mode)�������ޤ����Ѥ��ʤ��褦��̳��Ƥ��ޤ���
*/

#include "xwnmo.h"
#include <stdio.h>
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h" /* defines of CHMSIG, NISEBP */
#include "buffer.h"


/*
static int henkan_mode;
*/
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

/** ɸ�।�󥿡��ե������Υȥå״ؿ��Ǥ���
���δؿ��ϡ�mojiretsu������Ѵ���̤�����ơ�ʸ�����Ĺ�����֤��ޤ���
*/
kk()
{
  int	len;
#ifdef	WNMO
  c_c->buffer_in_nest = 0;
#endif	WNMO

#ifndef	WNMO
  if(henkan_off_flag){
    henkan_off();
  }
#else	WNMO
/* henkan_off �� jhlp.c do_main �Ǥ�Ƥ뤫�顢¿ʬ�פ�ʤ���
  if(henkan_off_flag){
    if (henkan_off() == BUFFER_IN_CONT)
	return(-1);
  }
*/
#endif	WNMO
#ifdef	WNMO
  if (buffer_in() == BUFFER_IN_CONT)
    return(-1);
  c_c->command_func = NULL;
#else
  buffer_in();
#endif	WNMO
  Strncpy(return_buf , c_b->buffer , c_b->maxlen);
  len = c_b->maxlen;
  clear_c_b();
  return(len);
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


/** ��������δؿ��ϡ� c_b->key_in_fun, c_b->ctrl_code_fun ��
�⡼�ɤ˱��������ꤹ���ΤǤ�������λ����ϡ����β��Υ⡼���ѹ��δؿ�����
�ߤƲ�������
*/

return_it(c)
int c;
{
#ifndef	X11
  c_b->maxlen = 1;
  c_b->buffer[0] = c;
  return(1);
#else	X11
  return_cl_it();
  return(0);
#endif	X11
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
  kakutei(c);
  return(1);
}

ignore_it_and_clear_romkan()
{
  romkan_clear();
  return(0);
}


insert_char_and_change_to_insert_mode(c)
int c;
{
    extern send_ascii_char_quote;
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    change_to_insert_mode();
    if (send_ascii_char_quote == 1 && send_ascii_char == 0){ /* ADD KURI */
	send_ascii_char = 1;
	send_ascii_char_quote = 0;
    }
#ifdef	X11
    check_scroll();
#endif	X11
    return(0);
}

/** ��������δؿ��ϡ��⡼���ѹ��λ��˸ƤФ���ΤǤ���
   ���ꤹ��Τϡ�key_table,key_in_fun,ctrl_code_fun,hanten�Ǥ���	
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

change_to_nobasi_tijimi_mode()
{
  c_b->key_table = main_table[2];
  c_b->rk_clear_tbl =  romkan_clear_tbl[2];
  c_b->key_in_fun = ignore_it_and_clear_romkan;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten = 0x04;
  henkan_mode = 2;
  kk_cursor_invisible();
}


change_to_empty_mode()
{
  c_b->key_table = main_table[3];
  c_b->rk_clear_tbl =  romkan_clear_tbl[3];
  if(send_ascii_char){
      c_b->key_in_fun = return_it_if_ascii;
  }else{
      c_b->key_in_fun = insert_char_and_change_to_insert_mode;
  }
  c_b->ctrl_code_fun = return_it;
  c_b->hanten =  0x08 | 0x20;
  henkan_mode = 3;
  throw_col(0);
  kk_cursor_normal();
  flush();
}

change_to_henkango_mode()
{
  c_b->key_table = main_table[0];
  c_b->rk_clear_tbl = romkan_clear_tbl[0];
  c_b->key_in_fun = push_char_return;
  c_b->ctrl_code_fun = NULL;
  /*
  c_b->hanten = 0x04;
  */
  c_b->hanten = 0x04 | 0x40;	/* add by KUWA */
  henkan_mode = 0;
  kk_cursor_invisible();
}


/** �Ѵ����ʸ�����bnst���ܤ�ʸ�ᤫ�����c_b->buffer������ޤ���
Ʊ���ˡ�bunsetsu�����ꤷ�ޤ���
*/
make_kanji_buffer(bnst)
int bnst;
{
  int k , l;
  wchar *bp;
  wchar	*buffer_end; /** ��̾�����ѤΥХåե����κǸ� */

  buffer_end = c_b->buffer + c_b->buflen - 1;
  if(bnst) bp = c_b->buffer + bunsetsuend[bnst - 1];
  else bp = c_b->buffer;
  for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++){
#ifdef old
      if(k != 0 && jl_dai_top(bun_data_, k) ){
	  Strcpy(bp, kugiri_str);
	  bp += Strlen(bp);
      }
#endif
      if (k < maxbunsetsu) bunsetsu[k] = bp - c_b->buffer;
      l = jl_get_kanji(bun_data_, k, k+1, bp);
      bp += l;
      if (k < maxbunsetsu) bunsetsuend[k] = bp - c_b->buffer;
      /* if (bp >= buffer_end) {
	  k--;
	  bp -= l;
	  break;
      }
      */
      /* change by kuwari 1/4/3 */
      if (bp > buffer_end) {
	  c_b->maxlen = bp - c_b->buffer;
      }
  }
  if(k < maxbunsetsu){
    bunsetsu[k++] = bp - c_b->buffer;
    bunsetsuend[k++] = bp - c_b->buffer;
/*     bunsetsucnt = k; */
  }else{
    bunsetsu[maxbunsetsu - 1] = bp - c_b->buffer;
    bunsetsuend[maxbunsetsu - 1] = bp - c_b->buffer;
  }
  c_b->maxlen = bp - c_b->buffer;
}


/** Ϣʸ���Ѵ� */
ren_henkan()
{
    int ret;

    jl_env_set(bun_data_, env_kanakan);
    ret = ren_henkan0();
    return (ret);
}

/** Ϣʸ����������Ѵ� */
kankana_ren_henkan()	/* ADD KURI */
{
    int ret;

    if (*kankana_env_name_in_uumrc != NULL) {
/* kaijyo() mo kankei suru
	to_han_or_hira();
*/
	not_to_han_or_hira();
	jl_env_set(bun_data_, env_kankana);
	ret = ren_henkan0();
	return (ret);
    } else {
	ring_bell();
	return (0);
    }
}


/** Ϣʸ�ᤫ�ʥ��޻��Ѵ� */
kanarom_ren_henkan()	/* ADD KURI */
{
    int ret;

    if (*kanarom_env_name_in_uumrc != NULL) {
	to_han_or_hira();
	jl_env_set(bun_data_, env_kanarom);
	ret = ren_henkan0();
	return (ret);
    } else {
	ring_bell();
	return (0);
    }
}

isconect_jserver()
{
    if (!jl_isconnect(bun_data_)) {
	connect_jserver(1);
    }
    if (!jl_isconnect(bun_data_)) {
	print_msg_getc(" jserver�Ȥ���³���ڤ�Ƥ��ޤ���");
	return (0);
    }
    return(1);
}

print_mes_dicconect_js()
{
#ifndef	WNMO
    push_cursor();
#endif	!WNMO
    print_msg_getc(" ���������Ȥ���³���ڤ�Ƥ��ޤ���");
#ifndef	WNMO
    pop_cursor();
    t_print_l();
#endif	!WNMO
}

ren_henkan0()
{
  w_char yomi[256];
  int moji_suu;
  register int i;

  if (!isconect_jserver()) {
    return(0);
  }

  c_b->buflen = maxchg;
  moji_suu =  c_b->maxlen - c_b->t_m_start;
  if(moji_suu > 0){
    int ret;
      Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
      yomi[moji_suu] = 0;
      ret = jl_ren_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE );
      for (i = cur_bnst_; i < jl_bun_suu(bun_data_); i++)
	  bunsetsu_env[i] = jl_env_get(bun_data_);
      if(ret == -1){
	  errorkeyin();
	  t_print_l();
	  return(0);
      }
      make_kanji_buffer(0);
  }
  change_to_henkango_mode();
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1]; /* add by KUWA */
  t_redraw_move(bunsetsu[cur_bnst_ ]
		,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

/** ñʸ���Ѵ�*/
tan_henkan()
{
    int tmp;

    jl_env_set(bun_data_, env_kanakan);
    tmp = tan_conv(WNN_SHO);
    return (tmp);
}

tan_henkan_dai()
{
    int tmp;

    jl_env_set(bun_data_, env_kanakan);
    tmp = tan_conv(WNN_DAI);
    return (tmp);
}

tan_conv(daip)
int daip;
{
    w_char yomi[512];
    int moji_suu;
    register int i;

    jl_env_set(bun_data_, env_kanakan);
    if (!isconect_jserver()) {
	return(0);
    }

    c_b->buflen = maxchg;
    moji_suu =  c_b->maxlen - c_b->t_m_start;
    if(moji_suu > 0){
	int ret;
	Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
	yomi[moji_suu] = 0;
	ret = jl_tan_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE, daip);
	for (i = cur_bnst_; i < dai_end(bun_data_, cur_bnst_); i++)
	    bunsetsu_env[i] = jl_env_get(bun_data_);
	if(ret == -1){
	    errorkeyin();
	    t_print_l();
	    return(0);
      }
      make_kanji_buffer(0);
    }
    change_to_henkango_mode();
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
    return(0);
}


/** ñʸ���Ѵ�*/
nobi_henkan()
{
    int tmp;

    tmp = nobi_conv(WNN_SHO, NULL);
    return (tmp);
}

nobi_henkan_dai()
{
    int tmp;

    tmp = nobi_conv(WNN_DAI, NULL);
    return (tmp);
}

/* ADD KURI */
romaji_nobi_henkan()
{
    int tmp;
    struct wnn_env *env_sv;

    if (env_kanarom != NULL) {
	/*
	to_han_or_hira();
	*/
	not_to_han_or_hira();
	env_sv = jl_env_get(bun_data_);
	jl_env_set(bun_data_, env_kanarom);
	tmp = nobi_conv(WNN_DAI, env_sv);
	return (tmp);
    } else {
	ring_bell();
	return (0);
    }
}

/* nobi_conv(daip) */
nobi_conv(daip, env)
struct wnn_env *env; /* ADD KURI */
int daip;
{
    int tmp;
    if(!jl_isconnect(bun_data_)) {
	connect_jserver(1);
    }
    tmp = tmp = c_b->t_b_st;	/* ADD KURI */
    if(tan_henkan1(daip, env) == 0){
	c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	t_redraw_move(bunsetsu[cur_bnst_ ] ,min(tmp, c_b->t_b_st), c_b->maxlen,1); /* change KURI */
#ifdef	X11
	check_scroll();
#endif	X11
    }
    return(0);
}

/*ñʸ���Ѵ����뤬�����̤�ɽ�����ʤ�*/
/* tan_henkan1(daip) */
tan_henkan1(daip, env) /* KURI */
struct wnn_env *env; /* ADD KURI */
int daip;
{
  int ret;
  int moji_suu = c_b->t_c_p - c_b->t_m_start;
  register int i;

  if(!jl_isconnect(bun_data_)) {
    print_mes_dicconect_js();
    return(-1);
  }

  if(moji_suu == 0){
    return(-1);
  }
  /* KURI
  if(jl_nobi_conv(bun_data_, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip) == -1){
  KURI */
  bunsetsu_env[cur_bnst_] = jl_env_get(bun_data_);
  if(env == NULL) {
    ret = jl_nobi_conv(bun_data_, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip);
  } else {
    ret = jl_nobi_conv_e2(bun_data_, env, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip);
  }

  if (ret == -1) {
    errorkeyin();
#ifndef WNMO
    t_print_l();
#endif WNMO
    return(-1);
  }
  make_kanji_buffer(0);
  for (i = cur_bnst_+1; i < jl_bun_suu(bun_data_); i++)
    bunsetsu_env[i] = jl_env_get(bun_data_);
  change_to_henkango_mode();
  c_b->t_c_p = bunsetsu[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];/* add by KUWA */
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];/* add by KUWA */
  return(0);
}

/* c_b->buffer�Υޡ����ΰ��֤��顢kana_buf��cur_bnst_�ʹߤ�
ʸ����򥳥ԡ����� */
/* make_yomi() : DELETED */

/* �������Ϥ��Ѵ����� */
henkan_if_maru(c)
{
	if (ISKUTENCODE(c) && c_b->key_table == main_table[1])
		ren_henkan();
}

/* ���� */
int kakutei()
{
    int moji_suu;
    wchar *w;
    wchar yomi[512];
     /* conversion nashi de kakutei surubaai wo kouryo seyo. */

   /* ��������ɤߤ�Ф��Ƥ���  remember_me() �Τ��� */
    remember_buf[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_yomi(bun_data_, 0, -1, remember_buf);
    }
    /* �������ʸ��������гФ��Ƥ��� */
    if(insert_modep()){
	moji_suu =  c_b->maxlen - c_b->t_m_start;
	if(moji_suu > 0){
	    w = remember_buf + Strlen(remember_buf);
	    Strncat(w,c_b->buffer + c_b->t_m_start, moji_suu);
	}
    }else if(nobasi_tijimi_mode()){
    /* �ΤФ��������ߥ⡼�ɤ��ä��顢�Ҥ餬�ʤ��᤹����˲������ */
    /* ��� si te iino ?? */
	moji_suu = jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
	Strcpy(c_b->buffer + c_b->t_c_p ,yomi);
	c_b->maxlen = Strlen(c_b->buffer);
	jl_kill(bun_data_, cur_bnst_, -1);
    }
    c_b->buffer[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_kanji(bun_data_, 0, -1, c_b->buffer);
	if(jl_update_hindo(bun_data_, 0, -1) == -1){
	    errorkeyin();
	    redraw_line();
	}
    } 
    if (insert_modep() && moji_suu > 0) { /* �������ʸ�������� */
	    Strcat(c_b->buffer,w);
    }else if(nobasi_tijimi_mode() && moji_suu > 0){ /* �ΤФ��������ߥ⡼�� */
	Strcat(c_b->buffer,yomi);
    }
    c_b->maxlen = Strlen(c_b->buffer);

    throw_c(0);
    clr_line_all();
    flush();
    return(1);
}

/*yank*/
int yank_c()
{

  if(empty_modep()){
	change_to_insert_mode();
  }
  t_yank();
  return(0);
}

/** �ɤߤκ�����*/
int remember_me()
{
    if(jl_bun_suu(bun_data_) == 0){
	if(c_b->key_in_fun){
	    (*c_b->key_in_fun)(-1);
	    c_b->key_in_fun = NULL;
	}
	Strcpy(c_b->buffer ,remember_buf);	/* KURI */
	c_b->maxlen = Strlen(remember_buf);	/* KURI */
	t_redraw_move(0, 0, c_b->maxlen,1);
#ifdef	X11
	check_scroll();
#endif	X11
    }
    return(0);
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

end_bunsetsu()
{
  int tmp = cur_bnst_;
  cur_bnst_ = jl_bun_suu(bun_data_) - 1;
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  /* 
  t_redraw_move(bunsetsu[cur_bnst_],c_b->t_c_p,bunsetsuend[cur_bnst_],0);
  */ 
  /* change by KUWA */
  t_redraw_move(bunsetsu[cur_bnst_],
		bunsetsu[dai_top(bun_data_, tmp)],
		bunsetsuend[cur_bnst_],0);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

top_bunsetsu()
{
  /*
  int tmp = c_b->t_m_start;
  */
  /* change by KUWA */
  int tmp = cur_bnst_;
  cur_bnst_ = 0;
  c_b->t_m_start = bunsetsuend[0];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  /*
  t_redraw_move(bunsetsu[0],0,tmp,0);
  */
  /* change by KUWA */
  t_redraw_move(bunsetsu[0],0,bunsetsuend[dai_end(bun_data_, tmp) -1],0);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

forward_bunsetsu()
{
  if(cur_bnst_ < jl_bun_suu(bun_data_) - 1){
    cur_bnst_ += 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];/* add by KUWA */
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];/* add by KUWA */
    /*
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[cur_bnst_ - 1],bunsetsuend[cur_bnst_ ],0);
    */
    /* change by KUWA */
    t_redraw_move(bunsetsu[cur_bnst_ ]
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_ - 1)]
		  ,bunsetsuend[dai_end(bun_data_, cur_bnst_) -1],0);
#ifdef	X11
    check_scroll();
#endif	X11
  }
  return(0);
}

backward_bunsetsu()
{
  if(cur_bnst_ > 0){
    cur_bnst_ -= 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];/* add by KUWA */
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];/* add by KUWA */
    /*
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[cur_bnst_ ],bunsetsuend[cur_bnst_ + 1],0);
    */
    /* change by KUWA */
    t_redraw_move(bunsetsu[cur_bnst_ ]
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],
		  bunsetsuend[dai_end(bun_data_, cur_bnst_ + 1) - 1],0);
#ifdef	X11
    check_scroll();
#endif	X11
  }
  return(0);
}

int kaijo()
{
  wchar yomi[512];

  c_b->t_b_end = c_b->t_b_st;   /* add by KUWA */
  t_redraw_move(bunsetsu[cur_bnst_],
		bunsetsu[dai_top(bun_data_, cur_bnst_)],
		bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1],0);
  change_to_insert_mode();
  c_b->t_c_p = bunsetsu[cur_bnst_];
  c_b->t_m_start = c_b->t_c_p;
  if (jl_env(bun_data_) == env_kanakan)
      jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
  else if (jl_env(bun_data_) == env_kankana)
      jl_get_yomi(bun_data_, cur_bnst_, -1, yomi); /* kankana_ren_henkan() */
  else
      recover_to_han_or_hira(yomi);
  Strcpy(c_b->buffer + c_b->t_c_p ,yomi);
/************************************/
/*  c_b->buflen = maxchg - (Strlen(yomi) + c_b->t_c_p); */
/*  c_b->buflen = maxchg - c_b->t_c_p;  What????*/
  c_b->buflen = maxchg;		/* Sonomama de yorosii H.T.*/
  c_b->maxlen = Strlen(c_b->buffer);
  jl_kill(bun_data_, cur_bnst_, -1);
/*   bunsetsucnt = b_suu_ + 1; */
  t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

enlarge_bunsetsu()
{
    wchar yomi[512];
    int area_len, len1;

    area_len = jl_yomi_len(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_));
    len1 = jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);

    if(area_len < len1)
	area_len ++;

  change_to_nobasi_tijimi_mode();
  c_b->t_m_start = c_b->t_c_p;
  c_b->t_b_end = c_b->t_c_p;    /* add by KUWA */
  Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
  c_b->maxlen = Strlen(c_b->buffer);
  t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start, c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}

smallen_bunsetsu()
{
    wchar yomi[512];
    int area_len;

    area_len = jl_yomi_len(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_));
    jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);

    if (area_len > 1) area_len--;

  change_to_nobasi_tijimi_mode();
  c_b->t_m_start = c_b->t_c_p;
  c_b->t_b_end = c_b->t_c_p;    /* add by KUWA */
  Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
  c_b->maxlen = Strlen(c_b->buffer);
  t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start , c_b->maxlen,1);
#ifdef	X11
  check_scroll();
#endif	X11
  return(0);
}


send_string(c)
int c;
{
  kakutei();
  c_b->buffer[c_b->maxlen] = c;
  c_b->maxlen += 1;
  c_b->t_c_p = c_b->maxlen;
  return(1);
}

tijime()
{
    if(c_b->t_c_p > c_b->t_m_start + 1){
	backward();
    }
    return(0);
}

jmptijime()
{
    t_move(c_b->t_m_start + 1);
    return(0);
}


henkan_forward()
{

  if(!jl_isconnect(bun_data_)) {
    return(0);
  }

    if(tan_henkan1(WNN_DAI, NULL) == -1){
	return(0);
    }
    zenkouho_dai_c();
    forward_bunsetsu();
    return(0);
}

henkan_backward()
{

  if(!jl_isconnect(bun_data_)) {
    return(0);
  }

    if (cur_bnst_ <= 0) {	/* ��ü�ʤ��Ѵ��Τ� */
	nobi_henkan_dai();
    } else {
    	if(tan_henkan1(WNN_DAI, NULL) == -1){
		return(0);
    	}
    	zenkouho_dai_c();
    	backward_bunsetsu();
    }
    return(0);
}

backward_c()
{
    if(!isconect_jserver()) {
	return(0);
    }
    if(c_b->t_c_p == c_b->t_m_start){
	if(c_b->t_c_p != 0) {
	    ren_henkan0();
	    backward_bunsetsu();
	}
    }else{
	backward();
    }
    return(0);
}


insert_it_as_yomi()
{
    kakutei();
    change_to_insert_mode();
    c_b->t_m_start = 0;
    cur_bnst_ = 0;
    t_print_l();
    if(bun_data_)
	jl_kill(bun_data_, 0, -1);	/* ADD KURI */
}

/****history *******/
previous_history()
{
    int k;
  if (jl_bun_suu(bun_data_) == 0) {	/* ADD KURI */
    k = previous_history1(c_b->buffer);
    if(k > 0){
	change_to_insert_mode();
	c_b->vst = 0;
	c_b->maxlen = k;
	c_b->t_c_p = 0;
	t_print_l();
    }
  }
    return(0);
}

next_history()
{
    int k;
  if (jl_bun_suu(bun_data_) == 0) {	/* ADD KURI */
    k = next_history1(c_b->buffer);
    if(k > 0){
	change_to_insert_mode();
	c_b->vst = 0;
	c_b->maxlen = k;
	c_b->t_c_p = 0;
	t_print_l();
    }
  }
    return(0);
}

empty_modep()
{
    return(henkan_mode == 3);
}

nobasi_tijimi_mode()
{
    return(henkan_mode == 2);
}
insert_modep()
{
    return(henkan_mode == 1);
}

henkan_gop()
{
    return(henkan_mode == 0 || henkan_mode == 2);
}


static int send_ascii_stack = 0;

send_ascii()
{
  send_ascii_stack = send_ascii_char;
  send_ascii_char = 1;
  return(0);
}

not_send_ascii()
{
  send_ascii_stack = send_ascii_char;
  send_ascii_char = 0;
  return(0);
}

toggle_send_ascii()
{
  send_ascii_stack = send_ascii_char;
  if(send_ascii_char == 0){
    send_ascii_char = 1;
  }else{
    send_ascii_char = 0;
  }
  return(0);
}

pop_send_ascii()
{
  send_ascii_char = send_ascii_stack;
  return(0);
}

/* ADD KURI */
int send_ascii_char_quote = 0;
quote_send_ascii()
{
  if(send_ascii_char == 1){
    send_ascii_char = 0;
    send_ascii_char_quote = 1;
  }
}
/* ADD KURI */

check_empty_mode_keyin_fun()
{
  if(send_ascii_char){
    c_b->key_in_fun = return_it_if_ascii;
  }else{
    c_b->key_in_fun = insert_char_and_change_to_insert_mode;
  }
}

send_ascii_e()
{
  send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

not_send_ascii_e()
{
  not_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

toggle_send_ascii_e()
{
  toggle_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

pop_send_ascii_e()
{
  pop_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

/* ADD KURI */
quote_send_ascii_e()
{
  quote_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}
/* ADD KURI */

/** jserver�ȤΥ��ͥ�������Ƴ�Ω���롣*/
reconnect_jserver()
{
  int ret;
  static struct wnmo_client *c_c_sv = 0;

/*
  if(jl_isconnect(bun_data_)) return(0);
*/
  if(!jl_isconnect_e(env_kanakan) ||
    env_kankana != 0 && !jl_isconnect_e(env_kankana) ||
    env_kanarom != 0 && !jl_isconnect_e(env_kanarom)) {

  if (c_c_sv != 0 && c_c != c_c_sv) {
    ring_bell();
    return(0);
  }
  if (c_c_sv == 0) {
    c_c_sv = c_c;
    c_c->command_func = reconnect_jserver;
  }

  if ((ret = reconnect_jserver_body()) == BUFFER_IN_CONT) {
    return(BUFFER_IN_CONT);
  }
  c_c_sv = 0;
  c_c->command_func = NULL;
  }
  return(0);
}

/** jserver �Ȥ� connection ���ڤ롣*/
disconnect_jserver()
{
  if (jl_isconnect_e(env_kanarom)) {
     jl_dic_save_all_e(env_kanarom);
     jl_disconnect(env_kanarom);
     env_kanarom =0;
  }
  if (jl_isconnect_e(env_kankana)) {
     jl_dic_save_all_e(env_kankana);
     jl_disconnect(env_kankana);
     env_kankana =0;
  }
  if (jl_isconnect_e(env_kanakan)) {
     jl_dic_save_all_e(env_kanakan);
     jl_disconnect(env_kanakan);
     env_kanakan =0;
  }
  return(1);
}

/* MOVED from jhlp.c */

/** �Ѵ����ջ��Υ롼�� */

henkan_off()
{
    unsigned char c;
    int i;
    extern int (*keyin_when_henkan_off)();
    static w_char wc[1];
    static int two_byte_first = 0;

    if (cur_p->w_c->command_func == NULL) {
#ifndef	X11
	push_cursor();
	if (TTY->dispmode == XJP_ROOT) {
	    display_title();	/* ���ʴ����Ѵ�������ɥ�ɽ�� */
	} else {
	    display_henkan_off_mode();
	}
	invisual_note();
	invisual_sheet();	/* ��ɽ�� KURI 63/1/29 */
#else	X11
	if (cur_x->dispmode == 1) {
	    invisual_window1();
	}
	xw_display_title();
#endif	!X11
	henkan_off_flag = 1;
	c_c->command_func = henkan_off;
	return(BUFFER_IN_CONT);
    } else if (cur_p->w_c->command_func != henkan_off) {
	ring_bell();
	return(0);
    }

    
    for(;;) {
	if ((i = (*keyin_when_henkan_off)()) == -1) /* convert_key ha tooru. */
	    return(BUFFER_IN_CONT);
	c = i;
	if ((!quote_flag) && (c == quote_code)) {
	    quote_flag = 1;
	    return(BUFFER_IN_CONT);
	}
	if ((quote_flag == 0) && (c_b->key_table[c] == henkan_off)){
	    break;
	}
        if (c == 0x8e || (c >= 0xa0 && c <= 0xfe)) {
	    if(two_byte_first == 1) {
		wc[0] += c;
#ifdef	GMW
		gmw_write(wc, 1);
#endif	GMW
#ifdef	X11
		xw_write(wc, 1);
#endif	X11
		two_byte_first = 0;
		quote_flag = 0;
	    } else {
		wc[0] = c * 0x100;
		two_byte_first = 1;
	    }
	} else {
	    wc[0] = c;
#ifndef	X11
	    gmw_write(wc, 1);
#else	X11
	    xw_write(wc, 1);
#endif	!X11
	    two_byte_first = 0;
	}
	quote_flag = 0;
	return(BUFFER_IN_CONT);
    }
    henkan_off_flag = 0;
    c_c->command_func = NULL;
#ifndef	X11
    if (TTY->dispmode == XJP_ROOT) {
	display_title_off();	 /* ���ʴ����Ѵ�������ɥ���ɽ�� */
    } else {
	disp_mode();
    }
#else	X11
    xw_display_title_off();
    disp_mode();
#endif	!X11
    if (!empty_modep()) {
	if (cur_x->dispmode == 1) {
	    visual_window1();
	}
    }
#ifndef	X11
    pop_cursor();
#endif	!X11
    return(0);
}

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
	if ((wc & 0xff00) == 0xa500)	/* �Ҳ�̾ */
	    return (wc & 0xa4ff);

	for ( i = 0; i < 16*6; i++) {
	    if (wc == _tatojis[i])	/* ASCII */
		return (i + ' ');
	}
	return(wc);
    }
}

not_to_han_or_hira()
{
    cur_bnst_sv = -1;
    save_yomi[0] = 0;
}

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
    } else if (cur_bnst_sv >= 0 && cur_bnst_sv < cur_bnst_) {
	i = jl_yomi_len(bun_data_, cur_bnst_sv, cur_bnst_);
	Strcpy(yomi, save_yomi + i);
    } else {
        jl_get_yomi(bun_data_, cur_bnst_, cur_bnst_sv, yomi);
	yomi[jl_yomi_len(bun_data_, cur_bnst_, cur_bnst_sv)] = 0;
	Strcat(yomi, save_yomi);
    }
}
