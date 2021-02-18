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

extern int kdicdel();
extern int kdicuse();
extern int kdiccom();
extern int kworddel();
extern int kworduse();
extern int kwordcom();

int (*jutil_table[2][3])() = {
  {kworddel, kworduse, kwordcom},
  {kdicdel, kdicuse, kdiccom}
};


#ifdef	X11
static char	*para_exec = "実行";
static char	*para_title = "パラメータ変更";
#endif	X11

paramchg()
{
    int c,k,ret;
    int newval;
    char *ch;
    char st[80];
    char message1[80];
    char message2[80];
    w_char kana_buf[1024];

    static char *ccc[] = {"解析文節数" , "小文節の最大数",
"頻度重み" , "小文節長重み" , "自立語長重み" , "最近使用状況重み" , "辞書優先度重み", "小文節の評価値重み", "大文節長重み", "小文節数重み", /* "幹語長重み", */
"数字の頻度", "カナの頻度", "英数の頻度", "記号の頻度", "閉括弧の頻度", "付属語の頻度", "開括弧の頻度"};
    struct wnn_param para;
    int *paraary;
    char *cccmsg[(sizeof(ccc) / sizeof(char *))];
    char buf[1024];

    get_bun_data();	/* get property and set bun_data_ */

    paraary = (int *)&para;
    if(jl_param_get(bun_data_, &para) < 0) {
	errorkeyin();
	return(-1);
    }
    for(k = 0,ch = buf ; k < (sizeof(ccc) / sizeof(char *)) ; k++){
	cccmsg[k] = ch;
	sprintf(ch,"%s[%d]",ccc[k],paraary[k]);
 	ch += strlen(ch) + 1;
    }
    c = xw_select_one_element(cccmsg,(sizeof(ccc) / sizeof(char *)),
			       0,para_title, 0, NULL, NULL, NULL, NULL);
    if (c == -1 || c == -3) {
        return(-1);
    }
    if(c == 0){
	sprintf(message1 , "現在 %d文節解析法を用いています。" ,paraary[0]);
	sprintf(message2 , "何文節解析法を用いますか ?");
    }else{
	sprintf(message1 , "現在の%sは %dです、",ccc[c] ,paraary[c]);
	sprintf(message2 , "幾つにしますか ? ");
    }
    init_input(message1, message2, para_exec);
Retry:
    cur_jutil->cl->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    kana_buf[0] = 0;
    if(kana_in(UNDER_LINE_MODE , kana_buf , 1024) == -1) {
		end_input();
		return(-1);
    }
    if(wchartochar(kana_buf , st) || sscanf(st , "%d" , &newval) <= 0 ){
	print_msg_getc("数字を入力してください。");
	goto Retry;
    } else if(c == 0 && newval <= 0){
	print_msg_getc("正の数字を入力してください。");
	goto Retry;
    }
    paraary[c] = newval;
    if(jl_param_set(bun_data_, &para) < 0) {
	    errorkeyin();
    }
    end_input();
    return(0);
}


update_dic_list(buf)
struct wnn_buf *buf;
{
    if((dic_list_size = jl_dic_list(bun_data_, &dicinfo)) == -1)return(-1);
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

#ifdef	X11
static char	*jisyo_kensaku_msg = "辞書検索";
static char	*dicdel_msg = "辞書削除";
static char	*dicuse_msg = "使用/不使用";
static char	*diccom_msg = "コメント";
#endif	X11

dicinfoout()
{
    int k;
    int i;
    int cnt;
    char buf[4096];
    char *info[JISHOKOSUU];
    char *comment;
    int size;
    int ret;
    char *table_t[3];

    get_bun_data();	/* get property and set bun_data_ */

    if(update_dic_list(bun_data_) == -1){
	errorkeyin();
	return;
    }
    cnt = dic_list_size;
    cnt += 1; /* for the comment line */
    size = 4096 / cnt;
    if(make_info_out(buf, size, dicinfo, cnt) == -1){
	print_msg_getc("残念、辞書が多すぎます。");
    }
    comment = buf;
    for(k = 1 ; k < cnt ; k++){
	info[k-1] = buf + k * size;
    }
    /*
    xw_select_one_element(info,cnt,0,jisyo_kensaku_msg,"","",SENTAKU,
				main_table[9]);
    */
    table_t[0] = dicdel_msg;
    table_t[1] = dicuse_msg;
    table_t[2] = diccom_msg;
    xw_select_one_element(info,cnt-1,0,jisyo_kensaku_msg,
			  3, table_t, jutil_table[1], main_table[9], comment);
    return(0);
}

int
select_one_dict(t)
char	*t;
{
    int	ret;

    return(select_one_dict1(t, 7));
}

static  char	*jishoichi_title = "辞書一覧";

select_one_dict9()
{
    int ret;

    return(select_one_dict1(jishoichi_title, 9));
}

select_one_dict1(title, tbl)
char	*title;
int	tbl;
{
    int l, ret;
    char *c;
    char buf[4096];
    char *buf_ptr[JISHOKOSUU];
    char baka[1024];

    get_bun_data();	/* get property and set bun_data_ */

    if(update_dic_list(bun_data_) == -1){
	errorkeyin();
	return;
    }
    if(dic_list_size == 0){
	print_msg_getc("辞書が一つも有りません。");
	return;
    }

    for(c = buf,l = 0 ; l < dic_list_size ; l++){
	buf_ptr[l] = c;
	dic_nickname(dicinfo[l].dic_no, baka);
	sprintf(c,"%s(%s) %s %d/%d",baka, dicinfo[l].hfname,
				dicinfo[l].enablef?  "使用中":"中断中",
				l + 1,dic_list_size);
	c += strlen(c) + 1;
    }
    ret = xw_select_one_element(buf_ptr, dic_list_size, 0,title,
				0, NULL, NULL, NULL, NULL);
    if (ret == -1 || ret == -3)
	return(-1);
    return(ret);
}

print_msg1(X)
char *X;
{
    init_yes_or_no(X, 3);
}


fuzoku_set()
{
    int ret;
    char *message1 = "付属語ファイル";
    char *message2 = "付属語ファイル名を入力して下さい";
    char fname[128];
    w_char kana_buf[1024];

    get_bun_data();	/* get property and set bun_data_ */

    if (init_input(message1, message2, para_exec) == -1) {
	    ring_bell();
	    return(-1);
    }
    cur_jutil->cl->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    Strcpy(kana_buf , fuzokugopath);
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	end_input();
	return(-1);
    }
    wchartochar(kana_buf , fname);
    if(strlen(fname) == 0) {
	end_input();
	return;
    }
    Strcpy(fuzokugopath , kana_buf);
    if(jl_fuzokugo_set(bun_data_,fname) == -1){
	errorkeyin();
    }
    end_input();
    return(0);
}

jishoadd()
{
    int ret;
    char *x;
    char *message1 = "辞書追加";
    char *message2 = "次へ";
    char *message = "新しい辞書名を入力して下さい";
    char *hmessage = "頻度ファイル名を入力して下さい";
    char *priomessage = "辞書の優先度の数字を指定して下さい";
    int k;
    int size;
    char fname[128];
    char tmp[128];
    char hfname[128];
    char shortname[128];
    int prio;
    int rdonly;
    int hrdonly;
    int c, id, l;
    w_char kana_buf[1024];


    get_bun_data();	/* get property and set bun_data_ */

    if (init_input(message1, message, message2) == -1) {
	    ring_bell();
	    return(-1);
    }
    JWMline_clear(0);
    throw_col(0);
    kk_cursor_normal();
    Strcpy(kana_buf , jishopath);
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	    end_input();
	    return(-1);
    }
    end_input();
    wchartochar(kana_buf , fname);
    if(strlen(fname) == 0) {
	return(-1);
    }
    Strcpy(jishopath , kana_buf);
    Strcpy(kana_buf,hindopath);
    getfname(hfname,fname);
    if(strcmp(hfname + strlen(hfname) - 4, ".usr") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".sys") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".dic") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".rev") == 0) {
	hfname[strlen(hfname) - 4] = 0;
	strcat(hfname , ".h");
    } else if(strcmp(hfname + strlen(hfname) - 5, ".usrR") == 0 ||
       strcmp(hfname + strlen(hfname) - 5, ".sysR") == 0) {
	hfname[strlen(hfname) - 5] = 0;
	strcat(hfname , ".hR");
    } else {
	strcat(hfname , ".h");
    }
    chartowchar(hfname, kana_buf + Strlen(kana_buf));
    if (init_input(message1, hmessage, message2) == -1) {
	ring_bell();
	return(-1);
    }
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	end_input();
	return(-1);
    }
    end_input();
    wchartochar(kana_buf , hfname);
    sprintf(tmp,"%d",JISHO_PRIO_DEFAULT);
    chartowchar(tmp,kana_buf);
    if (init_input(message1, priomessage, message2) == -1) {
	    ring_bell();
	    return(-1);
    }
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	    end_input();
	    return(-1);
    }
    end_input();
    wchartochar(kana_buf , tmp);
    prio = atoi(tmp);
    if(yes_or_no_or_newline("辞書を更新するモードで使いますか?") == 1){
	rdonly = 0;
    }else{
	rdonly = 1;
    }
    if(yes_or_no_or_newline("頻度を更新するモードで使いますか?") == 1){
	hrdonly = 0;
    }else{
	hrdonly = 1;
    }

    if((id = jl_dic_add(bun_data_,fname,hfname,0,prio,rdonly, hrdonly, NULL, NULL, yes_or_no_or_newline, print_msg1)) == -1 && wnn_errorno != -1){
	errorkeyin();  
    }
    update_dic_list(bun_data_);
    for(l = 0 ; l < dic_list_size ; l++){
	 if(dicinfo[l].dic_no == id){
	     break;
	}
    }
    if(l != dic_list_size && env_kankana_id > -1 && dicinfo[l].type == WNN_REV_DICT){
	bun_data_->env->env_id = env_kankana_id;
	if(jl_dic_add_e(bun_data_->env, fname,hfname, 1,prio,rdonly, hrdonly, NULL, NULL, yes_or_no_or_newline, print_msg1) == -1 && wnn_errorno != -1){
	    errorkeyin();
	}
    }
    /*
    clr_line_all();
    */
    end_yes_or_no();
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
    int c1;
    init_yes_or_no(string,1);
    for(;;) {
	c1 = romkan_next() ;
	if (select_return == 1) {
	    end_yes_or_no();
	    select_return = -1;
	    return(1);
	} else if (select_return == 0) {
	    end_yes_or_no();
	    select_return = -1;
	    return(0);
	} else {
	    ring_bell();
	    select_return = -1;
	}
    }
}


yes_or_no_or_newline(string)
char *string;
{
    return(yes_or_no(string));
}




static char *info_out_data[] ={
  "No.", 
  "Type",
  "こめんと",
  "ファイル名",
  "頻度ファイル名",
  "語彙数",
  "優先度",
  "登録可能",
  "頻度更新可能",
  "使用中か",
  "登録パスワード",
  "頻度更新パスワード",
  ""
};

make_info_out(buf,size,infobuf,cnt)
char buf[];
WNN_DIC_INFO infobuf[];
int cnt;
int size;
{
  int max1 = 0;
  int max0 = 0;
  int i,k,j;
  
  for(k = 0 ; k < cnt ; k++){
    for(j = 0 ; j < size ; j++){
      buf[k * size + j] = 0;
    }
  }

  for(i = 0 ; i < 14;i++){
    if(i == 13){
      sprintf(buf + max0,"%3d/%-3d",0,cnt - 1);
    }else{
      sprintf(buf + max0,"%s",info_out_data[i]);
    }
    max1 = max(max1, strlen(buf));
    if(max1 >= size)return(-1);
    for(k = 1 ; k < cnt;k++){
      switch(i){
      case 0:
	sprintf(buf + size * k + max0,"%3d ",infobuf[k - 1].dic_no);
	break;
      case 1:
	sprintf(buf + size * k + max0,"%8s",
		wnn_dic_types[infobuf[k - 1].type]);
	break;
      case 2:
	sStrcpy(buf + size * k + max0,infobuf[k - 1].comment);
	break;
      case 3:
	strcpy(buf + size * k + max0,infobuf[k - 1].fname);
	break;
      case 4:
	strcpy(buf + size * k + max0,infobuf[k - 1].hfname);
	break;
      case 5:
	sprintf(buf + size * k + max0,"%6d",infobuf[k - 1].gosuu);
	break;
      case 6:
	sprintf(buf + size * k + max0,"%4d",infobuf[k - 1].nice);
	break;
      case 7:
	strcpy(buf + size * k + max0,(!infobuf[k - 1].rw)? "Yes":"No");
	break;
      case 8:
	strcpy(buf + size * k + max0,(!infobuf[k - 1].hindo_rw)? "Yes":"No");
	break;
      case 9:
	strcpy(buf + size * k + max0,(infobuf[k - 1].enablef)? "使用中":"中断中");
	break;
      case 10:
	strcpy(buf + size * k + max0,infobuf[k - 1].passwd);
	break;
      case 11:
	strcpy(buf + size * k + max0,infobuf[k - 1].hpasswd);
	break;
      case 12:
	sprintf(buf + size * k + max0,"%3d/%-3d",k,cnt - 1);
	break;
      }
      max1 = max(max1, strlen(buf + size * k));
      if(max1 >= size)return(-1);
    }
    max0 = max1 + 1;
    for(k = 0 ; k < cnt;k++){
	fill(buf + size * k,max0);
    }
  }
  return(1);
}


fill(c,x)
char *c;
int x;
{
  for(;*c;c++,x--);
  for(;x;x--,c++){
    *c = ' ';
  }
}

