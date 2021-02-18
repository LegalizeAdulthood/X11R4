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

/** 標準インターフェースのトップ関数です。
この関数は、mojiretsuの中に変換結果を入れて、文字列の長さを返します。
*/
kk()
{
  static struct buf c_b1;

  initialize_vars();
  if(bun_data_) jl_kill(bun_data_, 0, -1);
  c_b = &c_b1;
  c_b->buffer = input_buffer;
  c_b->buf_msg = "";
  c_b->start_col = 0;
  c_b->t_c_p = 0;
  c_b->t_b_st = 0;	/* add by KUWA */
  c_b->t_b_end = 0;	/* add by KUWA */
  c_b->maxlen = 0;
  c_b->buflen = maxchg;
  c_b->redraw_fun = redraw_nisemono_c;
  c_b->t_m_start = 0;
/*   bunsetsucnt = 0; */
  set_screen_vars_default();
  change_to_empty_mode();
  init_screen();
/* added by T.S Sep.27 1987 */
  if(henkan_off_flag){
    henkan_off();
  }
/* end of addition */
  buffer_in();
  Strncpy(return_buf , c_b->buffer , c_b->maxlen);
  return(c_b->maxlen);
}

/** ここからの関数は、 c_b->key_in_fun, c_b->ctrl_code_fun に
モードに応じて設定するものです。設定の仕方は、この下のモード変更の関数群を
みて下さい。
*/

return_it(c)
int c;
{
  c_b->maxlen = 1;
  c_b->buffer[0] = c;
  return(1);
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
  kk_restore_cursor();
  kk_cursor_normal();
  flush();
}

change_to_henkango_mode()
{
  c_b->key_table = main_table[0];
  c_b->rk_clear_tbl =  romkan_clear_tbl[0];
  c_b->key_in_fun = push_char_return;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten = 0x04 | 0x40;	/* add by KUWA */
  henkan_mode = 0;
  kk_cursor_invisible();
}


/** 変換後の文字列のbnst番目の文節から先を、c_b->bufferに入れます。
同時に、bunsetsuも設定します。
*/
make_kanji_buffer(bnst)
int bnst;
{
  int k , l;
  wchar *bp;
  wchar	*buffer_end; /** 仮名入力用のバッファーの最後 */

  buffer_end = c_b->buffer + c_b->buflen - 1;
  if(bnst) bp = c_b->buffer + bunsetsuend[bnst - 1];
  else bp = c_b->buffer;
  for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++){
#ifdef old
      if(k != 0 && jl_dai_top(bun_data_, k) ){
	  Strcpy(bp, kugiri_str);
	  bp += Strlen(kugiri_str);
      }
#endif old
      if (k < maxbunsetsu) bunsetsu[k] = bp - c_b->buffer;
      l = jl_get_kanji(bun_data_, k, k+1, bp);
      bp += l;
      if (k < maxbunsetsu) bunsetsuend[k] = bp - c_b->buffer;
      if (bp > buffer_end) {
	  c_b->maxlen = bp - c_b->buffer;
      }
  }
  if(k < maxbunsetsu){
    bunsetsu[k++] = bp - c_b->buffer;
    bunsetsuend[k++] = bp - c_b->buffer;
  }else{
    bunsetsu[maxbunsetsu - 1] = bp - c_b->buffer;
    bunsetsuend[maxbunsetsu - 1] = bp - c_b->buffer;
  }
  c_b->maxlen = bp - c_b->buffer;
}

/** 連文節変換 */
ren_henkan()
{
    int ret;

    jl_env_set(bun_data_, env_kanakan);
    ret = ren_henkan0();
    return (ret);
}

/** 連文節漢字かな変換 */
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


/** 連文節かなローマ字変換 */
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
	push_cursor();
	print_msg_getc(" jserverとの接続が切れています。(如何?)");
	pop_cursor();
	t_print_l();
	return (0);
    }
    return(1);
}


ren_henkan0()
{
  w_char yomi[512];
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
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[cur_bnst_ ] 
		,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
  return(0);
}

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


/** 単文節変換*/
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

nobi_conv(daip, env)
struct wnn_env *env;
int daip;
{
    int tmp;
    if(!isconect_jserver()) {
	return(0);
    }
    tmp = tmp = c_b->t_b_st;	/* ADD KURI */
    if(tan_henkan1(daip, env) == 0){
	c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	t_redraw_move(bunsetsu[cur_bnst_ ] ,min(tmp, c_b->t_b_st), c_b->maxlen,1);
    }
    return(0);
}

/*単文節変換するが、画面に表示しない*/
tan_henkan1(daip, env)
struct wnn_env *env;
int daip;
{
  int ret;
  register int i;
  int moji_suu = c_b->t_c_p - c_b->t_m_start;

  if(!isconect_jserver()) {
    return(-1);
  }

  if(moji_suu == 0){
    return(-1);
  }
  bunsetsu_env[cur_bnst_] = jl_env_get(bun_data_);
  if(env == NULL) {
      ret = jl_nobi_conv(bun_data_, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip);
  } else {
      ret = jl_nobi_conv_e2(bun_data_, env, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip);
  }

  if (ret == -1) {
    errorkeyin();
    t_print_l();
    return(-1);
  }
  make_kanji_buffer(0);

  for (i = cur_bnst_+1; i < jl_bun_suu(bun_data_); i++)
    bunsetsu_env[i] = jl_env_get(bun_data_);
  /**/
  change_to_henkango_mode();
  c_b->t_c_p = bunsetsu[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  return(0);
}


/* c_b->bufferのマークの位置から、kana_bufのcur_bnst_以降に
文字列をコピーする */
/* make_yomi() : DELETED */

/* 区点入力で変換する */
henkan_if_maru(c)
{
	if (ISKUTENCODE(c) && c_b->key_table == main_table[1])
		ren_henkan();
}

/* 確定 */
int kakutei()
{
    int moji_suu;
    wchar *w;
    wchar yomi[512];
     /* conversion nashi de kakutei surubaai wo kouryo seyo. */

   /* 確定時に読みを覚えておく  remember_me() のため */
    remember_buf[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_yomi(bun_data_, 0, -1, remember_buf);
    }
    /* 解除した文字があれば覚えておく */
    if(insert_modep()){
	moji_suu =  c_b->maxlen - c_b->t_m_start;
	if(moji_suu > 0){
	    w = remember_buf + Strlen(remember_buf);
	    Strncat(w,c_b->buffer + c_b->t_m_start, moji_suu);
	}
    }else if(nobasi_tijimi_mode()){
    /* のばし、ちじみモードだったら、ひらがなに戻すために解除する */
    /* 解除 si te iino ?? */
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
    if (insert_modep() && moji_suu > 0) { /* 解除した文字がある */
	Strcat(c_b->buffer,w);
    }else if(nobasi_tijimi_mode() && moji_suu > 0){ /* のばし、ちじみモード */
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

/** 読みの再入力*/
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
  t_redraw_move(bunsetsu[cur_bnst_],
		bunsetsu[dai_top(bun_data_, tmp)],
		bunsetsuend[cur_bnst_],0);
}

top_bunsetsu()
{
  int tmp = cur_bnst_;
  cur_bnst_ = 0;
  c_b->t_m_start = bunsetsuend[0];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[0],0,bunsetsuend[dai_end(bun_data_, tmp) -1],0);
}

forward_bunsetsu()
{
  if(cur_bnst_ < jl_bun_suu(bun_data_) - 1){
    cur_bnst_ += 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_ - 1)]
		  ,bunsetsuend[dai_end(bun_data_, cur_bnst_) -1],0);
  }
  return(0);
}

backward_bunsetsu()
{
  if(cur_bnst_ > 0){
    cur_bnst_ -= 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],
		  bunsetsuend[dai_end(bun_data_, cur_bnst_ + 1) - 1],0);
  }
  return(0);
}

int kaijo()
{
  wchar yomi[512];

  c_b->t_b_end = c_b->t_b_st;	/* add by KUWA */
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
  c_b->buflen = maxchg;
  c_b->maxlen = Strlen(c_b->buffer);
  jl_kill(bun_data_, cur_bnst_, -1);
  t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 1);
  return(0);
}


jutil_c()
{

    if(!isconect_jserver()){
	return (0);
    }
    push_cursor();
    jutil();
    pop_cursor();
    t_print_l();
    if (empty_modep()) {	       /* added by T.S 10 Jan. '88 */
	kk_restore_cursor();
    }
    return(0);
}

touroku_c()
{
    if(!isconect_jserver()){
	return(0);
    }
    push_cursor();
    touroku();
    pop_cursor();
    t_print_l();
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
  c_b->t_b_end = c_b->t_c_p;	/* add by KUWA */
  Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
  c_b->maxlen = Strlen(c_b->buffer);
  t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start, c_b->maxlen,1);
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
  c_b->t_b_end = c_b->t_c_p;	/* add by KUWA */
  Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
  c_b->maxlen = Strlen(c_b->buffer);
  t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start , c_b->maxlen,1);
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
    if(!isconect_jserver()) {
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
    if(!isconect_jserver()) {
	return(0);
    }

    if (cur_bnst_ <= 0) {	/* 左端なら変換のみ */
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

quote_send_ascii_e()
{
  quote_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

/** jserverとのコネクションを再確立する。*/
reconnect_jserver()
{
    if(!jl_isconnect_e(env_kanakan) ||
       env_kankana != 0 && !jl_isconnect_e(env_kankana) ||
       env_kanarom != 0 && !jl_isconnect_e(env_kanarom)) {
	push_cursor();
	reconnect_jserver_body();
	pop_cursor();
	t_print_l();
    }
    return(0);
}

reconnect_jserver_body()
{
  char *message = " 接続ホスト名 > ";
  w_char *kana_buf[1024];
  char hostname[128];

  chartowchar(jserver_name_in_uumrc,kana_buf);
  if (kana_in(message, UNDER_LINE_MODE, kana_buf, 1024) == -1) return(0);
  wchartochar(kana_buf, hostname);
  if(strlen(hostname) == 0)return(0);
  strcpy(jserver_name_in_uumrc, hostname);

  print_msg(" 接続中・・・");
  if(connect_jserver(1) < 0){
    print_msg_getc(" 接続がうまくいきません。(如何?)");
  } else if(!jl_isconnect(bun_data_)) {
    print_msg_getc(" 接続に失敗しました。(如何?)");
  }
  return(0);
}


/** jserver との connection を切る。*/
disconnect_jserver()
{
  /* ADD KURI 89/6/6 */
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
     jl_env_set(bun_data_, env_kanakan);
     jl_close(bun_data_);
     env_kanakan =0;
  }
  return(1);
}

/* MOVED from jhlp.c */

/** 変換オフ時のループ */

henkan_off()
{
    unsigned char c;
    int i;
    extern int (*keyin_when_henkan_off)();
    extern int ptyfd;
    static char outbuf[16];
    extern int (*code_trans[])();
    extern int henkan_off_flag;

    push_cursor();
    display_henkan_off_mode();
    flush();

    henkan_off_flag = 1;
    
    for(;;) {
	if ((i = (*keyin_when_henkan_off)()) == -1) /* convert_key ha tooru. */
	    continue;
	c = i;
	if ((!quote_flag) && (c == quote_code)) {
	    quote_flag = 1;
	    continue;
	}
	if ((quote_flag == 0) && (c_b->key_table[c] == henkan_off)){
	    break;
	}
	/* コード変換 ADD KURI */
	if (pty_c_flag == tty_c_flag)
	    i=through(outbuf, &c, 1);
	else
	    i=(*code_trans[(J_EUJIS<<2)|pty_c_flag])(outbuf, &c, 1);
	if (i <= 0) continue;
	write(ptyfd, outbuf , i);
	quote_flag = 0;
    }
    henkan_off_flag = 0;
    pop_cursor();
    redraw_line();
    flush();
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
	if ((wc & 0xff00) == 0xa500)	/* 片仮名 */
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
