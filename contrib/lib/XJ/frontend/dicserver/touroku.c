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
 *	Original Source: ./lib/Wnn/uum/touroku.c
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
/* 登録のためのルーチン */

#include <stdio.h>
#include "xwnmo.h"
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"
#include "jslib.h"

static int touroku_start,touroku_end;
/* these are used only for making yomi string for registered kanji */


int touroku_mode = 0;
static char *touroku_title = "辞書登録";
static char *touroku_exec_title = "辞書登録";
static char *kanji_start_msg = "登録はじめ:";
static char *kanji_end_msg = "登録おわり:";
static char *yomi_msg = "読み:";
static char *hinsi_msg = "品詞:";
static char *jisho_msg = "辞書:";
static char *dic_choice = "    辞書    ";
static w_char msg_st[6] = {' ',' ',
	(((unsigned char)'\306')<< 8)|(unsigned char)'\311',
	(((unsigned char)'\244')<< 8)|(unsigned char)'\337',':', 0};

/* 登録のための外部関数*/
extern
touroku()
{
    int i , k;
    int ud_no, real_dic_no;
    w_char yomibuf[1024];
    char *message[4];
    char *message1[4];
    w_char hani_buffer[1024];
    w_char get_buffer[1024];
    char dic_name_heap[2048];
    char *hp = dic_name_heap;
    int hinsi;
    int err;
    int tmp;
    char *dict_name[JISHOKOSUU];
    int current_dic;
    int current = -1;
    int size;
    char *ch;
    
    cur_bnst_ = get_touroku_data(get_buffer);
    if(update_dic_list(bun_data_) == -1){
	errorkeyin();
	return;
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
    for(ud_no = 0, size = 0 ; ud_no < dic_list_size; ud_no++){
	if((dicinfo[ud_no].type == WNN_UD_DICT || dicinfo[ud_no].type == WNN_REV_DICT)
	   && dicinfo[ud_no].rw == 0){
	    if(*dicinfo[ud_no].comment){
		sStrcpy(hp,dicinfo[ud_no].comment);
		dict_name[size] = hp;
		hp += strlen(hp) + 1;
	    }else{
		dict_name[size] = dicinfo[ud_no].fname;
	    }
	    if(dicinfo[ud_no].dic_no == current_dic) current = size;
	    size++;
	}
    }
    if (size == 1)
	    ud_no = 0;
    message[0] = kanji_start_msg;
    message[1] = yomi_msg;
    message[2] = hinsi_msg;
    message[3] = jisho_msg;
    message1[0] = message1[1] = message1[2] = NULL;
    message1[3] = dict_name[((current == -1)? ud_no:current)];

    init_jutil(touroku_title,touroku_exec_title,4,message, message1);
    touroku_mode = KANJI_IN_START;

repeat:
    for(;;) {
	switch(touroku_mode) {
    	    case KANJI_IN_START:
		change_cur_jutil(MESSAGE1);
		change_mes_title(MESSAGE1,kanji_start_msg,1);
		kk_cursor_invisible();
		JWMline_clear(0);
		kk_cursor_normal();
		hani_buffer[0] = 0;
		Strcpy(hani_buffer, get_buffer);
		if((tmp = xw_hani_settei(hani_buffer , 1024)) < 0)
			break;
		if(Strlen(hani_buffer) >= 64) {
		    print_msg_getc("登録範囲が長過ぎます。");
		    touroku_mode = KANJI_IN_START;
		}
		kk_cursor_invisible();
		break;
    	    case YOMI_IN:
		for(i = 0 ; i < LENGTHYOMI + 1 ; i++){
		    yomibuf[i] = 0;
		}
		yomibuf[LENGTHYOMI] = '0';
		change_cur_jutil(MESSAGE2);
		hanten_jutil_mes_title(MESSAGE2,1);
		JWMline_clear(0);
		kk_cursor_normal();
		select_return = -1;
		kana_in_w_char_msg((0x08 | 0x20), yomibuf, LENGTHYOMI);
		if (select_return == -2) {
		    touroku_mode = CANCEL;
		} else if (select_return == 0) {
		    touroku_mode = TOUROKU_GO;
		} else if (select_return == 1) {
      		    if (jfew->jutil->mode == MESSAGE1) {
	  		    touroku_mode = KANJI_IN_START;
      		    } else if (jfew->jutil->mode == MESSAGE2) {
	  		    touroku_mode = YOMI_IN;
      		    } else if (jfew->jutil->mode == MESSAGE3) {
	  		    touroku_mode = HINSI_IN;
      		    } else if (jfew->jutil->mode == MESSAGE4) {
	  		    touroku_mode = JISHO_IN;
      		    }
		} else {
		    touroku_mode = HINSI_IN;
		}
		hanten_jutil_mes_title(MESSAGE2,0);
		kk_cursor_invisible();
		break;
    	    case HINSI_IN:
		change_cur_jutil(MESSAGE3);
		hanten_jutil_mes_title(MESSAGE3,1);
		JWMline_clear(0);
		kk_cursor_invisible();
		hinsi = hinsi_in();
		hanten_jutil_mes_title(MESSAGE3,0);
		if (touroku_mode == HINSI_IN)
		    touroku_mode = JISHO_IN;
		break;
    	    case JISHO_IN:
		if(size == 1){
		    ud_no = 0;
		}else{
		    change_cur_jutil(MESSAGE4);
		    hanten_jutil_mes_title(MESSAGE4,1);
		    JWMline_clear(0);
		    kk_cursor_invisible();
		    ud_no = xw_select_one_element(dict_name,size,current,
					dic_choice, 0, NULL, NULL, NULL);
		    hanten_jutil_mes_title(MESSAGE4,0);
		    xw_jutil_write_msg(dict_name[ud_no]);
		    if (ud_no == -3) {
			touroku_mode = EXECUTE;
		    } else if (ud_no == -1) {
			touroku_mode = CANCEL;
		    }
		}
		if (touroku_mode == JISHO_IN)
		    touroku_mode = EXECUTE;
		break;
    	    case EXECUTE:
		xw_exec_mode();
		break;
    	    case TOUROKU_GO:
		goto do_touroku;
		break;
    	    case CANCEL:
		end_jutil();
 		return (-1);
	}
    }

do_touroku:
    for (real_dic_no = 0; real_dic_no < dic_list_size; real_dic_no++) {
	if((dicinfo[real_dic_no].type == WNN_UD_DICT ||dicinfo[real_dic_no].type == WNN_REV_DICT)
	   && dicinfo[real_dic_no].rw == 0){
	    ud_no--;
	    if (ud_no < 0)
		break;
	}
    }

    if(jl_word_add(bun_data_, dicinfo[real_dic_no].dic_no, yomibuf, hani_buffer,
			  			"", hinsi, 0) == -1){
        errorkeyin();
        touroku_mode = EXECUTE;
	goto repeat;
    }
    end_jutil();
    return(0);
}


/** key_tableにバインドして用います*/
int
t_markset()
{
    if (select_return == -2) {
	touroku_mode = CANCEL;
	hanten_jutil_mes_title(MESSAGE1,0);
	select_return = -1;
        return(-1);
    }
  
    if(c_b->t_m_start == -2) {
	hanten_jutil_mes_title(MESSAGE1,0);
        return(0);
    }
    if(c_b->t_m_start == -1) {
	if(jfew->jutil->mode != MESSAGE1) {
            touroku_mode = KANJI_IN_START;
	    change_mes_title(MESSAGE1,kanji_start_msg,1);
            return(0);
	}
        if(c_b->t_c_p == c_b->maxlen){
            touroku_mode = KANJI_IN_END;
	    change_mes_title(MESSAGE1,kanji_end_msg,1);
            return(0);
        }
        c_b->t_m_start = c_b->t_c_p;
        touroku_mode = KANJI_IN_END;
	change_mes_title(MESSAGE1,kanji_end_msg,1);
        t_move(find_end_of_tango(c_b->t_c_p));
        kk_cursor_invisible();
        return(0);
    }else{
	if(c_b->t_m_start == c_b->t_c_p) {
            c_b->t_m_start = -1;
            touroku_mode = KANJI_IN_START;
	    change_mes_title(MESSAGE1,kanji_start_msg,1);
            kk_cursor_normal();
            return(-1);
	} else {
	    if (jfew->jutil->mode == MESSAGE1)
		jfew->jutil->mode = MESSAGE2;
            hanten_jutil_mes_title(MESSAGE1,0);
            return(1);
        }
    }
}

extern int redraw_nisemono();
xw_exec_mode()
{
    int c1;
    int ret;

    for(;;) {
	c1 = romkan_next();
	if (select_return == -2) {
	    touroku_mode = CANCEL;
	    select_return = -1;
	    return(-1);
	} else if(select_return == 0) {
	    touroku_mode = TOUROKU_GO;
	    select_return = -1;
	    return(0);
	} else if (select_return == 1) {
	    if (jfew->jutil->mode == MESSAGE1) {
		touroku_mode = KANJI_IN_START;
	    } else if (jfew->jutil->mode == MESSAGE2) {
		touroku_mode = YOMI_IN;
	    } else if (jfew->jutil->mode == MESSAGE3) {
		touroku_mode = HINSI_IN;
	    } else if (jfew->jutil->mode == MESSAGE4) {
		touroku_mode = JISHO_IN;
	    }
	    select_return = -1;
	    return(0);
	} else if ((c1 < 256) && (main_table[4][c1] != NULL)) {
	    if ((ret = (*main_table[4][c1])()) == 1) {
		touroku_mode = TOUROKU_GO;
		return(0);
	    } else if (ret == -1) {
		touroku_mode = CANCEL;
		return(-1);
	    } else {
		ring_bell();
	    }
	} else {
	    ring_bell();
	}
    }
}

/* 範囲設定ルーチン*/
static int
xw_hani_settei(buffer , buflen)
w_char *buffer;
int buflen;
{
  struct buf *c_btmp , c_b1;
  int first = 0;
  int buffer_in_ret;
  int (*func_sv)();

    c_btmp = c_b;
    c_b = &c_b1; 

    c_b->buffer = buffer;
    c_b->buflen = buflen;

    c_b->maxlen = Strlen(buffer);

    c_b->hanten = 0x04;
    c_b->t_c_p = touroku_bnst[cur_bnst_];
#ifndef	X11
    c_b->buf_msg =start_msg;
    c_b->start_col = 9;
#else	X11
    c_b->buf_msg ="";
    c_b->start_col = 0;
    hanten_jutil_mes_title(MESSAGE1,1);
#endif	!X11
    c_b->t_m_start = -1;/* -1:まだセットされていない*/
    set_screen_vars_default();

    c_b->key_table = main_table[5];
    c_b->rk_clear_tbl = romkan_clear_tbl[5];
    c_b->key_in_fun = NULL;
    c_b->redraw_fun = redraw_nisemono;
    c_b->ctrl_code_fun = NULL;
    init_screen();
    if(buffer_in() == -1) {
	if (touroku_mode == KANJI_IN_START || touroku_mode == KANJI_IN_END)
	    touroku_mode = CANCEL;
	/*
	    touroku_mode = KANJI_IN_START;
	*/
	c_b = c_btmp;
	return(-1);
    }
    if (select_return == -2) {
	touroku_mode = CANCEL;
	select_return = -1;
	c_b = c_btmp;
	return(-1);
    } else if (select_return == 0) {
	touroku_mode = TOUROKU_GO;
	select_return = -1;
	c_b = c_btmp;
	return(0);
    } else if (select_return == 1) {
        if (jfew->jutil->mode != MESSAGE1) {
	    if(jfew->jutil->mode == MESSAGE2) {
		touroku_mode = YOMI_IN;
	    } else if(jfew->jutil->mode == MESSAGE3) {
		touroku_mode = HINSI_IN;
	    } else if(jfew->jutil->mode == MESSAGE4) {
		touroku_mode = JISHO_IN;
	    }
        } else {
	    touroku_mode = KANJI_IN_START;
	    c_b = c_btmp;
	    return(0);
	}
    } else {
	touroku_mode = YOMI_IN;
    }
    Strncpy(buffer, buffer + c_b->t_m_start, c_b->t_c_p - c_b->t_m_start);
    buffer[c_b->t_c_p - c_b->t_m_start] = 0;
    touroku_start = c_b->t_m_start;
    touroku_end = c_b->t_c_p;
    c_b = c_btmp;
    return(c_b1.t_c_p - c_b1.t_m_start);
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

