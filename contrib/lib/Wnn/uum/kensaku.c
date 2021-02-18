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
#include "sdefine.h"
#include "sheader.h"


static char msg1[] = " はありません。(如何?)";

/* Dirty,  but share it with select_elemet.c */
struct wnn_jdata *word_searched;

struct wnn_jdata *
word_list1(flag, tbl)
int flag, tbl;
{
    int k;
    char *c;
    char tmp[WNN_HINSI_NAME_LEN * 2];
    static char message[] = "読み: ";
    w_char kana_buf[2048];
    char buf[16384];
    char *info[MAXJIKOUHO];
    char imatukattayo;
    int cnt;
    int rcnt;
    int tcnt;
    int type;

    throw_c(0);
    clr_line();
    kana_buf[0] = 0;
    if(kana_in(message ,UNDER_LINE_MODE  , kana_buf , 1024) == -1) {
	return(NULL);
    }
    
    if((cnt = jl_word_search_by_env(bun_data_, kana_buf, &word_searched)) < 0){
	errorkeyin();
	return(NULL);
    }
    
    update_dic_list(bun_data_);

    /* KURI 語数を、事前に調べる */
    if (flag) {
    /* flag = 1 は, UD & SD, 0 は, UD only */
	tcnt = cnt;
    } else {
	for(k = 0, tcnt = 0 ; k < cnt ; k++){
	    type = dicinfo[find_dic_by_no(word_searched[k].dic_no)].type;
	    if(type == WNN_UD_DICT || type == WNN_REV_DICT){
		tcnt++;
	    }
	}
	if (tcnt == 0)	/* ADD KURI */
	    cnt = 0;
    }
    if(cnt == 0){
	throw_c(0);clr_line();u_s_on();
	w_printf(kana_buf , maxlength - strlen(msg1) - 2);
	u_s_off();printf(msg1);flush();
	romkan_next();
	return(NULL);   /* changed 11/1 by Nide */
    }
    for(c = buf,k = 0,rcnt = 0 ; k < cnt ; k++){
	type = dicinfo[find_dic_by_no(word_searched[k].dic_no)].type;
	if(flag || type == WNN_UD_DICT || type == WNN_REV_DICT){
	    info[rcnt] = c;
	    w_sttost(word_searched[k].kanji,c);
	    if(word_searched[k].com && word_searched[k].com[0]){
		strcat(c, " {");
		w_sttost(word_searched[k].com,c + strlen(c));
	    strcat(c,"}");
	    }
	    strcat(c," (");
	    dic_nickname(word_searched[k].dic_no, c + strlen(c));
	    sprintf(c + strlen(c),":%d",word_searched[k].serial );
	    strcat(c,") ");
	    if(word_searched[k].ima) imatukattayo = '*';
	    else imatukattayo = ' ';
	    if(word_searched[k].hindo == -1){
		sprintf(c + strlen(c),"---  ");
	    }else{
		sprintf(c + strlen(c),"%c%-3d  ",imatukattayo , word_searched[k].hindo );
	    }
	    if(!(word_searched[k].int_ima && word_searched[k].int_hindo == -1)){
				/* Internal Hindo exist */
		if(word_searched[k].int_hindo == -1){
		    sprintf(c + strlen(c),"---");
		}else{
		    sprintf(c + strlen(c),"(%c%d) ", (word_searched[k].int_ima)? '*':' ', word_searched[k].int_hindo );
		}
	    }
	    sStrcpy(tmp, jl_hinsi_name(bun_data_, word_searched[k].hinshi));
	    strcat(c,tmp);
	    rcnt ++;
	    sprintf(c + strlen(c),"%3d/%-3d",rcnt,tcnt);
	    c += strlen(c) + 1;
	}
    }
    if((k = select_line_element(info,rcnt,0,"",0,0,main_table[tbl])) ==-1) {
	return(NULL);
    }
    /* ADD KURI */
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


#ifdef not_use
struct wnn_jdata *
word_list(flag)  /* flag == 1: all dicts, 0: only updatable dicts */
int flag;
{
    return(word_list1(flag, 7));
}
#endif not_use

word_list8(flag)
int flag;
{
    word_list1(flag, 8);
}

#ifdef not_use
sakujyo()
{

    struct wnn_jdata *word;

    if((word = word_list(0)) == NULL)return;

    if(jl_word_delete(bun_data_, word->dic_no, word->serial) == -1){
	errorkeyin();
	return;
    }
    throw_c(0);clr_line();
    h_r_on();w_printf(word->kanji,35);h_r_off();
    printf(" は削除されました。(如何?)");
    flush();
    romkan_next();
    return(0);
}
#endif not_use

kensaku()
{

    word_list8(1);		/* 1 for all dicts */
}

#ifdef not_use
unable_entry()
{

    struct wnn_jdata *word;

    if((word = word_list(1)) == NULL)return;

    if(jl_word_use(bun_data_, word->dic_no, word->serial) == -1){
	errorkeyin();
	return;
    }
}
#endif not_use
