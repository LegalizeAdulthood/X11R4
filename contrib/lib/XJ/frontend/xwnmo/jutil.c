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
 *	Original Source: ./lib/Wnn/uum/jutil.c
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
/* jisho utility routine for otasuke (user interface) process */
/*	活辞書とはカレントユーザ辞書のことだよ	*/

#include <stdio.h>
#include "xwnmo.h"
#include "jslib.h"
#include "jllib.h"
#include "commonhd.h"
#ifdef SYSVR2
#ifndef	X11R3
#   include <fcntl.h>
#endif	/* X11R3 */
#endif
#ifdef BSD42
#ifndef	X11R3
#   include <sys/file.h>
#endif	/* X11R3 */
#endif
#include "sdefine.h"
#include "sheader.h"
#include <stdio.h>

#define MESSAGE_REC 3
#define MESSAGE_NUM 5


char *sh_fname();

static int ret() ; /* remove sonota() , nullfun() */
 int touroku() , kensaku() ;
 int jishoadd() , dicsv(),fuzoku_set();
 int  paramchg() , dicinfoout(), select_one_dict9();
 /*
 int jishodel();
 int sakujyo();
 int dicstatuschg();
 int unable_entry();
 */

static struct msg_fun {
  char *msg;
  int (*fun)();
};

static struct msg_fun message1[] = { 
  {"辞書追加", jishoadd},
  {"辞書一覧",select_one_dict9},
  {"登録", touroku},
  {"検索", kensaku},
  {"パラメータ変更",paramchg},
  {"頻度セーブ",dicsv},
  {"辞書情報",dicinfoout},
  {"付属語変更", fuzoku_set}
/* この4つはもう要らないんですがどうしましょう。
  {"削除", sakujyo},
  {"無効/有効", unable_entry},
  {"辞書削除",jishodel},
  {"辞書使用/不使用", dicstatuschg},
*/
/*
  {"終了",ret}
*/
};

#ifdef	X11
static char	*jutil_title = "辞書操作";
#endif	X11
jutil()
{
    static int last = 3;
    int not_redrawtmp = not_redraw;
    static int c, ret;
    static int k;
    static char *buf[sizeof(message1) / sizeof(struct msg_fun)];
    static struct wnmo_client *c_c_sv = 0;
    static jutil_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (0);
    }
    if (jutil_step == 0) {
	c_c_sv = c_c;
	not_redraw = 1;
	for(k = 0 ; k <( sizeof(message1) / sizeof(struct msg_fun));k++){
	    buf[k]=message1[k].msg;
	}
	jutil_step++;
    }
    if (jutil_step == 1) {
	c = xw_select_one_element(buf,sizeof(message1)/sizeof(struct msg_fun),
				 last,jutil_title,SENTAKU,main_table[4]);
	if(c == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
	if(c == -1 || c == -3) {
	    not_redraw = not_redrawtmp;
	    c_c_sv = 0;
	    jutil_step = 0;
	    return(0);
	}
	jutil_step++;
	last = c;
    }
    if (jutil_step == 2) {
	ret = (*message1[c].fun)();
	if (ret == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
	}
    }
    not_redraw = not_redrawtmp;
    c_c_sv = 0;
    jutil_step = 0;
    return(0);
}

#ifdef	X11
static char	*para_exec = "実行";
static char	*para_title = "パラメータ変更";
#endif	X11

paramchg()
{
    xw_paramchg();
    return(0);
}


dicsv()
{
/*
    print_msg("只今、辞書の頻度とユーザ辞書をセーブしています。");
*/
    if(jl_dic_save_all(bun_data_) == -1){
    	errorkeyin();
    }
    else {
	print_msg_getc("辞書の頻度とユーザ辞書のセーブを完了しました。");
    }
    return(0);
}

update_dic_list(buf)
struct wnn_buf *buf;
{
    if((dic_list_size = jl_dic_list(bun_data_, &dicinfo)) == -1) {
	errorkeyin();
	return(-1);
    }
    return(0);
}

dic_nickname(dic_no, buf)
int dic_no;
char *buf;
{
    int j;

    if((j = find_dic_by_no(dic_no)) == -1)return(-1);
    if(*dicinfo[j].comment) sStrcpy(buf, dicinfo[j].comment);
    else strcpy(buf, dicinfo[j].fname);
    return(0);
}

find_dic_by_no(dic_no)
int dic_no;
{
    int j;

    for(j = 0 ; j < dic_list_size; j++){
	if(dicinfo[j].dic_no == dic_no)
	    return(j);
    }
    return(-1);
}


dicinfoout()
{
    xw_dicinfoout();
    return(0);
}


select_one_dict9()
{
    xw_select_one_dict9();
    return(0);
}


fuzoku_set()
{
    xw_fuzoku_set();
    return(0);
}

jishoadd()
{
    xw_jishoadd();
    return(0);
}

kensaku()
{
    xw_kensaku();
    return(0);
}


char *
sh_fname(s , n , d)  /* file名 sが長い時に、その最後の方を取って n文字にまとめて dに入れる   */ 
char *s , *d;
int n;
{
  int k;
  char *c , *c1;

  k = strlen(s);
  if(k < n){
    strcpy(d , s);
    return(d);
  }else{
    c = s + k - n + 2;
    c1 = d;
    *c1++ = '-';
    *c1++ = '-';
    for(;*c && *c != '/';c++){
      *c1++ = '-';
    }
    strcpy(c1 , c);
    return(d);
  }
}


yes_or_no(string)
char *string;
{
    XJClient *xjc_sv = 0;
    int c1;

    xjc_sv = cur_p;
    init_yes_or_no(string,1);
    for(;;) {
	c1 = keyin0();
	if (select_return == 1) {
	    end_yes_or_no();
	    cur_cl_change3(xjc_sv);
	    select_return = -1;
	    return(1);
	} else if (select_return == 0) {
	    end_yes_or_no();
	    cur_cl_change3(xjc_sv);
	    select_return = -1;
	    return(0);
	} else {
	    ring_bell();
	    cur_cl_change3(xjc_sv);
	    select_return = -1;
	}
    }
}


yes_or_no_or_newline(string)
char *string;
{
    return(yes_or_no(string));
}
