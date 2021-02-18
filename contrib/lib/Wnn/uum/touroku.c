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
/* 登録のためのルーチン */

#include <stdio.h>
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"
#include "jslib.h"

static int touroku_start,touroku_end;
/* these are used only for making yomi string for registered kanji */


static char *end_msg = "登録終り:";
static char *start_msg = "登録始め:";
static char *dic_choice = "辞書: ";
extern	int	cursor_invisible;
	/* 62/06/24 kuri TTY terminalでカーソルが消えてしまうバグを修正 */
static w_char msg_st[6] = {' ',' ',
	(((unsigned char)'\306')<< 8)|(unsigned char)'\311',
	(((unsigned char)'\244')<< 8)|(unsigned char)'\337',':', 0};

/* 登録のための外部関数*/
extern
touroku()
{
    int k;
    w_char yomibuf[1024];
    w_char message[1024];
    w_char hani_buffer[1024];
    char dic_name_heap[2048];
    char *hp = dic_name_heap;
    int hinsi;
    int err;
    int tmp;
    char *dict_name[JISHOKOSUU];
    static int current_dic;
    int current = 0;
    int size;
    wchar comment[LENGTHKANJI];
    char *commessage = "コメント:";
    

    if(update_dic_list(bun_data_) == -1){
	errorkeyin();
	return(-1);
    }
    for(k = 0 , size = 0 ; k < dic_list_size ; k++){
	if((dicinfo[k].type == WNN_UD_DICT || dicinfo[k].type == WNN_REV_DICT)
	   && dicinfo[k].rw == 0){
	    size++;
	}
    }
    if(size == 0){
	print_msg_getc("登録可能形式の辞書が一つもありません。");
	return(-1);
    }
    
 repeat:
    
    for(k = 0 ; k < 1024 ; k++){
	yomibuf[k] = 0;
    }
    tmp = hani_settei(hani_buffer , 1024);
    
    if(tmp == -1){
	return(-1);
    }
    if(Strlen(hani_buffer) >=64) {	/* 62/06/26 kuri */
	print_msg_getc("登録範囲が長過ぎます。(如何)");
	goto	repeat;
    }
    
    message[0] = 0;
    Strcpy(message + Strlen(message),hani_buffer);
    Strcpy(message + Strlen(message),msg_st);
    find_yomi_for_kanji(hani_buffer,yomibuf);
    
 kana_start:
    
    if(kana_in_w_char_msg(message , (0x08 | 0x20) , yomibuf , 1024) == -1){
	return(-1);
    }
    if(Strlen(yomibuf) >=64) {	/* 62/06/26 kuri */
	print_msg_getc("読みが長過ぎます。(如何)");
	goto kana_start;
    }
    if((hinsi = hinsi_in()) == -1){
	return(-1);
    }
    
    for(k = 0, size = 0 ; k < dic_list_size; k++){
	if((dicinfo[k].type == WNN_UD_DICT || dicinfo[k].type == WNN_REV_DICT)
	   && dicinfo[k].rw == 0){
	    if(*dicinfo[k].comment){
		sStrcpy(hp,dicinfo[k].comment);
		dict_name[size] = hp;
		hp += strlen(hp) + 1;
	    }else{
		dict_name[size] = dicinfo[k].fname;
	    }
	    if(dicinfo[k].dic_no == current_dic) current = size;
	    size++;
	}
    }
    if(size == 1){
	k = 0;
    }else{
	k = select_one_element(dict_name,size,current,dic_choice,
			       strlen(dic_choice),0,main_table[4]);
	if(k == -1) {
	    return(-1);
	}
    }

    /* find UD  ADD KURI */
    for (tmp = 0; tmp < dic_list_size; tmp++) {
	if((dicinfo[tmp].type == WNN_UD_DICT ||dicinfo[tmp].type == WNN_REV_DICT)
	   && dicinfo[tmp].rw == 0){
	    k--;
	    if (k < 0)
		break;
	}
    }
    comment[0] = 0;
    if(touroku_comment){
	if(kana_in(commessage , UNDER_LINE_MODE , comment , LENGTHYOMI) == -1){
	    comment[0] = 0;
	}
    }
    if((err = jl_word_add(bun_data_, dicinfo[tmp].dic_no, yomibuf, hani_buffer,
			  comment, hinsi, 0)) == -1){
	errorkeyin();
	return(-1);
    }
    return(0);
}


/** key_tableにバインドして用います*/
int
t_markset()
{
  
  if(c_b->t_m_start == -2) {
    return(0);
  }
  if(c_b->t_m_start == -1) {
    if(c_b->t_c_p == c_b->maxlen){
	return(0);
    }
    c_b->t_m_start = c_b->t_c_p;
    print_buf_msg(c_b->buf_msg = end_msg);
    t_move(find_end_of_tango(c_b->t_c_p));
    kk_cursor_invisible();
    return(0);
  }else{
    if(c_b->t_m_start == c_b->t_c_p){
      c_b->t_m_start = -1;
      print_buf_msg(c_b->buf_msg = start_msg);
      kk_cursor_normal();
      if (! cursor_invisible_fun)
       /* 62/06/24 kuri TTY terminalでカーソルが消えてしまうバグを修正 */
	t_throw();
      flush();
      return(0);
    }else{
      return(1);
    }
  }
}

extern int redraw_nisemono();

make_touroku_buffer(bnst, cbup)
int bnst;
struct buf *cbup;
{
  int k , l;
  wchar *bp;
  wchar	*buffer_end; /** 仮名入力用のバッファーの最後 */

  buffer_end = c_b->buffer + c_b->buflen - 1;
  bp = c_b->buffer;
  for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++){
      if (nobasi_tijimi_mode() && k == cur_bnst_) /* のばし、ちじみモード */
	 break;
      if (k < maxbunsetsu) touroku_bnst[k] = bp - c_b->buffer;
      l = jl_get_kanji(bun_data_, k, k+1, bp);
      bp += l;
      if (bp >= buffer_end) {
	  k--;
	  bp -= l;
	  goto GOT_IT;
      }
  }
  if (cbup->maxlen == 0) {	/* 確定後の登録 ADD KURI */
	/* ヒストリから持ってくる。*/
	get_end_of_history(bp);
  } else if (nobasi_tijimi_mode()) {	/* のばし、ちじみモード */
	Strncpy(bp, cbup->buffer + bunsetsu[cur_bnst_],
		cbup->maxlen - bunsetsu[cur_bnst_]);
	*(bp + cbup->maxlen - bunsetsu[cur_bnst_]) = 0;
  } else {
        /* 途中で解除したかな文字列 */
	Strncpy(bp, cbup->buffer + bunsetsu[jl_bun_suu(bun_data_)],
		cbup->maxlen - bunsetsu[jl_bun_suu(bun_data_)]);
	*(bp + cbup->maxlen - bunsetsu[jl_bun_suu(bun_data_)]) = 0;
  }

 GOT_IT:
  if(k < maxbunsetsu){
      touroku_bnst[k++] = bp - c_b->buffer;
      touroku_bnst_cnt = k;
  }else{
      touroku_bnst[maxbunsetsu - 1] = bp - c_b->buffer;
      touroku_bnst_cnt = maxbunsetsu;
  }
}



/* 範囲設定ルーチン*/
static int
hani_settei(buffer , buflen)
w_char *buffer;
int buflen;
{
  struct buf *c_btmp , c_b1;

  c_btmp = c_b;
  c_b = &c_b1; 

  c_b->buffer = buffer;
  c_b->buflen = buflen;

  
  make_touroku_buffer(0, c_btmp);
  c_b->maxlen = Strlen(buffer);

  c_b->hanten = 0x04;
  c_b->t_c_p = touroku_bnst[cur_bnst_];
  c_b->buf_msg =start_msg;
  c_b->start_col = 9;
  c_b->t_m_start = -1;/* -1:まだセットされていない*/
  set_screen_vars_default();

  c_b->key_table = main_table[5];
  c_b->rk_clear_tbl = romkan_clear_tbl[5];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = NULL;
  init_screen();
  if(buffer_in() == -1){
    c_b = c_btmp;
    kk_cursor_normal();
    return(-1);
  }
  kk_cursor_normal();
  Strncpy(buffer, buffer + c_b->t_m_start, c_b->t_c_p - c_b->t_m_start);
  buffer[c_b->t_c_p - c_b->t_m_start] = 0;
  touroku_start = c_b->t_m_start;
  touroku_end = c_b->t_c_p;
  c_b = c_btmp;
  return(c_b1.t_c_p - c_b1.t_m_start);
}


find_yomi_for_kanji(kanji_string,yomi_buf)
w_char *kanji_string,*yomi_buf;
{
    w_char *w;
    int k;
    int s_bun, s_offset;
    int e_bun, e_offset;
    int kana_start, kana_end;
    int ylen, klen;
    
    for(w = kanji_string ; *w ; w++){
	if(!(YOMICHAR(*w) || KATAP(*w)))break;
    }
    if(!(*w)){
	Strcpy(yomi_buf , kanji_string);
	for(k = Strlen(yomi_buf) - 1 ; k >= 0 ; k--){
	    yomi_buf[k] = HIRA_OF(yomi_buf[k]);
	}
	yomi_buf[k] = 0;
    }else{
	if(!henkan_gop()){
	    yomi_buf[0] = 0;
	}else{
	    for(k = 0; k < jl_bun_suu(bun_data_) && touroku_bnst[k + 1] <= touroku_start;k++);
	    s_bun = k;
	    s_offset = touroku_start - touroku_bnst[k];
	    
	    for(; k < jl_bun_suu(bun_data_) && touroku_bnst[k] < touroku_end ; k++);
	    k--;
	    e_bun = k;
	    e_offset = touroku_end - touroku_bnst[k];
	    

	    kana_end = jl_get_yomi(bun_data_, s_bun, e_bun + 1, yomi_buf) - 
		jl_yomi_len(bun_data_, e_bun, e_bun + 1);


	    ylen = jl_yomi_len(bun_data_, s_bun, s_bun + 1);
	    klen = jl_kanji_len(bun_data_, s_bun, s_bun + 1) - 
		(ylen - jl_jiri_len(bun_data_,s_bun));

	    if(s_offset < klen){
		kana_start = 0;
	    }else{
		kana_start = jl_jiri_len(bun_data_, s_bun) + s_offset - klen;
	    }
	    
	    ylen = jl_yomi_len(bun_data_, e_bun, e_bun + 1);
	    klen = jl_kanji_len(bun_data_, e_bun, e_bun + 1) - 
		(ylen - jl_jiri_len(bun_data_, e_bun));

	    if(e_offset < klen){
		kana_end += jl_jiri_len(bun_data_, s_bun);
	    }else{
		kana_end += jl_jiri_len(bun_data_, e_bun) + e_offset - klen;
	    }
	    Strcpy(yomi_buf , yomi_buf + kana_start,
		      kana_end - kana_start);
	    yomi_buf[kana_end - kana_start] = 0;
	}
    }
}

find_end_of_tango(c)
int c;
{
    int k;
    int jisyu; /* 0: katakana 1: hiragana 2:ascii 3:kanji */
    
    if(KATAP(c_b->buffer[c]))jisyu = 0;
    else if(HIRAP(c_b->buffer[c]))jisyu = 1;
    else if(ASCIIP(c_b->buffer[c])) jisyu = 2;
    else if(KANJIP(c_b->buffer[c])) jisyu = 3;
    else return(c + 1);
    
    for(k = c + 1; k < c_b->maxlen ; k++){
	if(jisyu == 0){
	    if(!KATAP(c_b->buffer[k]))break;
	}else if(jisyu == 1){
	    if(!HIRAP(c_b->buffer[k]))break;
	}else if(jisyu == 2){
	    if(!ASCIIP(c_b->buffer[k]))break;
	}else if(jisyu == 3){
	    if(!KANJIP(c_b->buffer[k]))break;
	}
    }
    return(k);
}

