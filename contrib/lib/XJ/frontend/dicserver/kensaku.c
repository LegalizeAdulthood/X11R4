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
 *	Original Source: ./lib/Wnn/uum/kensaku.c
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
#include "commonhd.h"
#include "xwnmo.h"
#include "sdefine.h"
#include "sheader.h"

static char msg1[] = " はありません。(如何?)";
static char msg2[] = "削除";
static char msg3[] = "使用/不使用";
static char msg4[] = "コメント";
extern int (*jutil_table[2][3])();

/* Dirty,  but share it with select_elemet.c */
struct wnn_jdata *word_searched;

struct wnn_jdata *
word_list1(flag, tbl)
int flag, tbl;
{
    int k,j;
    char *c;
    char tmp[WNN_HINSI_NAME_LEN * 2];
    w_char kana_buf[2048];
    char buf[16384];
    char *info[MAXJIKOUHO];
    char imatukattayo;
    int cnt;
    int rcnt;
    int tcnt;
    int type;

    static char message1[] = "検索";
    static char message2[] = "読み";
    static char message3[] = "実行";
    static char message4[2048];
    char	menubuf[32];
    int ret;
    char *table_t[3];

    get_bun_data();	/* get property and set bun_data_ */

    if (init_input(message1, message2, message3) == -1) {
	    ring_bell();
	    return(NULL);
    }
retry:
    JWMline_clear(0);
    throw_col(0);
    kk_cursor_normal();
    kana_buf[0] = 0;
    if((ret = kana_in(UNDER_LINE_MODE  , kana_buf , 1024)) == -1) {
	end_input();
	return(NULL);
    }
    if (kana_buf[0] == 0) {
	ring_bell();
	goto retry;
    }
    if((cnt = jl_word_search_by_env(bun_data_, kana_buf, &word_searched)) < 0){
	end_input();
	errorkeyin();
	return(NULL);
    }
    
    update_dic_list(bun_data_);

    if(cnt == 0){
	print_msg_getc("そのような語はありません。");
	goto retry;
    }
    /* KURI 語数を、事前に調べる */
    if (flag) {
	    tcnt = cnt;
    } else {
	    for(k = 0, tcnt = 0 ; k < cnt ; k++){
		type = dicinfo[find_dic_by_no(word_searched[k].dic_no)].type;
		if(type == WNN_UD_DICT || type == WNN_REV_DICT){
		    tcnt++;
		}
	    }
    }
    for(c = buf,k = 0,rcnt = 0 ; k < cnt ; k++){
	type = dicinfo[find_dic_by_no(word_searched[k].dic_no)].type;
	if(flag || type == WNN_UD_DICT || type == WNN_REV_DICT){
	    info[rcnt] = c;
	    w_sttost(word_searched[k].kanji,c);
	    if(word_searched[k].com && word_searched[k].com[0]){
		strcat(c, "   {");
		w_sttost(word_searched[k].com,c + strlen(c));
		strcat(c,"}");
	    }
	    strcat(c,"   (");
	    dic_nickname(word_searched[k].dic_no, c + strlen(c));
	    sprintf(c + strlen(c),":%d",word_searched[k].serial );
	    strcat(c,")  ");
	    if(word_searched[k].ima) imatukattayo = '*';
	    else imatukattayo = ' ';
	    if(word_searched[k].hindo == -1){
		sprintf(c + strlen(c),"----  ");
	    }else{
		sprintf(c + strlen(c),"%c%-3d  ",imatukattayo , word_searched[k].hindo );
	    }
	    if(!(word_searched[k].int_ima && word_searched[k].int_hindo == -1)){
				/* Internal Hindo exist */
		if(word_searched[k].int_hindo == -1){
		    sprintf(c + strlen(c),"----");
		}else{
		    sprintf(c + strlen(c),"(%c%-3d)  ", (word_searched[k].int_ima)? '*':' ', word_searched[k].int_hindo );
		}
	    }
	    sStrcpy(tmp, jl_hinsi_name(bun_data_, word_searched[k].hinshi));
	    strcat(c,tmp);
	    rcnt ++;
	    sprintf(c + strlen(c),"  %3d/%-3d",rcnt,tcnt);
	    c += strlen(c) + 1;
	}
    }
    end_input();
    sprintf(message4, "%s %s : %s", message1, message2, kana_buf);

    table_t[0] = msg2;
    table_t[1] = msg3;
    table_t[2] = msg4;
    k =  xw_select_one_element(info,rcnt,0,message1,
				3,table_t,jutil_table[0], main_table[8],  NULL);
    if (k == -1 || k == -3) {
	return(NULL);
    }
    if (flag == 0) {
	for(rcnt = 0; k >= 0 && rcnt < cnt; rcnt++) {
	    type = dicinfo[find_dic_by_no(word_searched[rcnt].dic_no)].type;
	    if(type == WNN_UD_DICT || type == WNN_REV_DICT){
		k--;
	    }
	}
	k = rcnt - 1;
    }
    return(word_searched + k);
}

word_list8(flag)
int flag;
{
    word_list1(flag, 8);
}

kensaku()
{
    word_list8(1);		/* 1 for all dicts */
}
