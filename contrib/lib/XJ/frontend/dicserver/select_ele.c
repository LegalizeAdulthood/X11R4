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
 *	Original Source: ./lib/Wnn/uum/select_ele.c
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

/* 一つの要素をユーザーに取り出してもらうためのルーチン */

#define DD_MAX 512

#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "buffer.h"
#include "jslib.h"
#include "jllib.h"

#define printh(mm)\
  if(state == 0){\
    printf("%c.",((mm - dd[cc]) > 9)? mm - dd[cc] - 10  + 'A':mm - dd[cc] + '0');\
    printf(data[mm]);\
  }else{\
    printf("%c.",((mm > 9)? mm - 10  + 'A':mm + '0'));\
    printf(data[mm]);}
		    

#define kouho_len(x) (strlen(x) + 4)

/*
#define WIDTH (maxlength - disp_mode_length - 6)
*/
#define LEND c_b->vlen + 1
#define WIDTH (LEND - 7)


static char **data; /* mojiretu data area*/
static int dd[512]; /* gamen no sentou to naru youso wo motte iru */
static int dd_size; /* dd no size */
static char *msg;
static int msg_len;
static int cc; /* ima no gamen */
static int mm;/* hyouji mojiretu */
static int state; /* choose whether to number all or not */
				/* 0: line by line */
				/* 1: all */
static int kosuu; /* kosuu of elements */
extern hilited_item;
extern max_line;

xw_select_one_element(c,kosuu1,init,title, bcnt,btable_t,b_tbl,key_tbl,comment)
char **c; /*表示文字列の配列のポインタ */
int kosuu1;
int init;
char *title;
int bcnt;
char *btable_t[];
int (**b_tbl)();
int (**key_tbl)();
char *comment;
{
  int oldmm;
  int d;
  int c1;
  int ret;

  kosuu = kosuu1;
  init_ichiran(c, kosuu1,init,title, comment);
  for(;;) {
    c1 = romkan_next() ;
    if (select_return == -2) {
        end_ichiran();
	romkan_clear();
	return(-1);
    } else if (select_return != -1) {
	d = find_hilite();
	select_return = -1;
    } else {
	if ((c1 < 256) && (main_table[4][c1] != NULL)) {
	    if ((ret = (*main_table[4][c1])()) == 1) {
		d = find_hilite();
	    } else if (ret == -1) {
		end_ichiran();
		romkan_clear();
		return(-1);
	    } else {
		ring_bell();
		continue;
	    }
	} else {
	    ring_bell();
	    continue;
	}
    }
    if(d >= 0 && d < kosuu){
	romkan_clear();
	if(!bcnt) {
	    end_ichiran();
	    return(d);
	} else {
	    cc = d;
	    lock_ichiran();
	}
	init_keytable(bcnt, btable_t);
	for (;;) {
	    c1 = romkan_next() ;
	    if (select_return == -2) {
		end_ichiran();
		end_keytable();
		romkan_clear();
		return(-1);
	    } else if (select_return != -1) {
		d = select_return;
		select_return = -1;
		if((d < bcnt) && (b_tbl[d] != NULL)){
		    end_ichiran();
		    end_keytable();
		    (*b_tbl[d])();
    		    romkan_clear();
		    return(0);
		} else {
		    ring_bell();
		}
	    } else {
		if((c1 < 256) && (key_tbl[c1] != NULL)){
		    end_ichiran();
		    end_keytable();
		    (*key_tbl[c1])();
    		    romkan_clear();
		    return(0);
		} else {
		    ring_bell();
		    select_return = -1;
		    continue;
		}
	    }
	}
    }
    romkan_clear();
  }
}


static 
display_all()
{
    redraw_ichi_w(NULL);
}

static int
change_decimal(c1)
{
  if(c1 >= '0' && c1 <= '9'){
    return(c1 - '0');
  }
  if(c1 >= 'A' && c1 <= 'Z'){
    return(c1 - 'A' + 10);
  }
  if(c1 >= 'a' && c1 <= 'z'){
    return(c1 - 'a' + 10);
  }
  return(-1);
}


select_select()
{
  return(1); /* return mm from upper function */
}

quit_select()
{
  return(-1);
}

/*
 *
 *  Functions Called during select element (dictionaries).
 *
 */ 

/* jishodel for kensaku */
int kdicdel()
{
    char buf[512];

    sprintf(buf, "辞書 %s を削除します。", dicinfo[cc].fname);
    if(yes_or_no(buf) == 1){
	if(jl_dic_delete(bun_data_, dicinfo[cc].dic_no) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}


int kdicuse()
{
    char buf[512];

    if(dicinfo[cc].enablef){
	sprintf(buf, "辞書 %s の使用を中止します。", dicinfo[cc].fname);
    }else{
	sprintf(buf, "辞書 %s の使用を再開します。", dicinfo[cc].fname);
    }
    if(yes_or_no(buf) == 1){
	if(jl_dic_use(bun_data_, dicinfo[cc].dic_no,!dicinfo[cc].enablef) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int kdiccom()
{
    w_char com[512];
    char st[32];

    com[0] = 0;
    init_input("コメント","コメントを入力してください。", "実行");
    cur_jutil->cl->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    if(kana_in(UNDER_LINE_MODE ,com,512) == -1) {
	end_input();
	return(0);
    }
    end_input();
    if(jl_dic_comment_set(bun_data_, dicinfo[cc].dic_no, com) == -1){
	    errorkeyin();
	    return(0);
    }
    return(1);
}

/*
 *
 *  Functions Called during select element (dic_entries).
 *
 */ 

/* Defined in kensaku.c */
extern struct wnn_jdata *word_searched;

kworddel()
{
    char buf[512];
    int type;

    type = dicinfo[find_dic_by_no(word_searched[cc].dic_no)].type;
    if(type != WNN_UD_DICT && type != WNN_REV_DICT){
	print_msg_getc("固定形式の辞書のエントリは削除出来ません。");
	return(0);
    }
    if(dicinfo[find_dic_by_no(word_searched[cc].dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc("リードオンリーの辞書のエントリは削除出来ません。");
	return(0);
    }
	
    strcpy(buf, "「");
    w_sttost(word_searched[cc].kanji,buf + strlen(buf));
    sprintf(buf + strlen(buf), "」を削除します。");
    if(yes_or_no(buf) == 1){
	if(jl_word_delete(bun_data_, word_searched[cc].dic_no, 
			  word_searched[cc].serial) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

kworduse()
{
    char buf[512];

    if(dicinfo[find_dic_by_no(word_searched[cc].dic_no)].hindo_rw == WNN_DIC_RDONLY){
	print_msg_getc("リードオンリーの頻度で変更出来ません。");
	return(0);
    }
    strcpy(buf, "「");
    w_sttost(word_searched[cc].kanji,buf + strlen(buf));
    if(word_searched[cc].hindo == -1 || 
       (word_searched[cc].int_hindo == -1 && !word_searched[cc].int_ima)){
	sprintf(buf + strlen(buf), "」の使用を再開します。");
    }else{
	sprintf(buf + strlen(buf), "」の使用を中止します。");
    }
    if(yes_or_no(buf) == 1){
	if(jl_word_use(bun_data_, word_searched[cc].dic_no, 
			  word_searched[cc].serial) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int kwordcom()
{
    w_char com[512];
    char st[32];
    int type;

    type = dicinfo[find_dic_by_no(word_searched[cc].dic_no)].type;
    if(type != WNN_UD_DICT && type != WNN_REV_DICT){
	print_msg_getc("固定形式辞書のエントリのコメントは変更出来ません。");
	return(0);
    }
    if(dicinfo[find_dic_by_no(word_searched[cc].dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc("リードオンリー辞書のエントリのコメントは変更出来ません。");
	return(0);
    }

    com[0] = 0;
    init_input("コメント","コメントを入力してください。", "実行");
    cur_jutil->cl->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    if(kana_in(UNDER_LINE_MODE ,com,512) == -1) {
	end_input();
	return(0);
    }
    end_input();
    if(jl_word_comment_set(bun_data_, word_searched[cc].dic_no,
			   word_searched[cc].serial, com) == -1){
	errorkeyin();
	return(0);
    }
    return(1);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
